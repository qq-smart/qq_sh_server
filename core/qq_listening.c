/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


void
qq_create_listening(qq_listening_t *listening, int type, int port,
    size_t pool_size, qq_connection_handler_pt handler)
{
    qq_log_debug("qq_create_listening()");

    listening->sockaddr.sin_family = AF_INET;
    listening->sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listening->sockaddr.sin_port = htons(port);
    listening->socklen = sizeof(struct sockaddr);

    listening->addr_text_len = qq_sock_ntop(&listening->sockaddr, listening->addr_text);
    qq_log_debug("listening address: %s, address len: %d",
        listening->addr_text, listening->addr_text_len);

    listening->fd = (qq_socket_t) -1;
    listening->type = type;

    listening->backlog = QQ_LISTEN_BACKLOG;
    listening->rcvbuf = QQ_SOCK_REVBUF_SIZE;
    listening->sndbuf = QQ_SOCK_SNDBUF_SIZE;

    listening->handler = handler;
    listening->pool_size = pool_size;
}

qq_int_t
qq_open_listening_sockets(qq_cycle_t *cycle)
{
    int              reuseaddr = 1;
    qq_uint_t        i, tries, failed;
    qq_err_t         err;
    qq_socket_t      s;
    qq_listening_t  *ls;

    qq_log_debug("qq_open_listening_sockets()");

    for (tries = 5; tries; tries--) {
        failed = 0;

        ls = cycle->listening;
        for (i = 0; i < cycle->nlistening; i++) {
            if (ls[i].fd != (qq_socket_t) -1) {
                continue;
            }

            s = socket(ls[i].sockaddr.sin_family, ls[i].type, 0);
            if (s == (qq_socket_t) -1) {
                qq_log_error(errno, "create sockets %s failed", ls[i].addr_text);
                return QQ_ERROR;
            }

            if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                          (const void *) &reuseaddr, sizeof(int))
                == -1)
            {
                qq_log_error(errno, "setsockopt(SO_REUSEADDR) %s failed", ls[i].addr_text);

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %s failed", ls[i].addr_text);
                }
                return QQ_ERROR;
            }

#if (QQ_HAVE_REUSEPORT)
            int  reuseport = 1;
            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_REUSEPORT,
                          (const void *) &reuseport, sizeof(int))
                == -1)
            {
                qq_log_error(errno, "setsockopt(SO_REUSEPORT) %s failed, ignored", ls[i].addr_text);

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %s failed", ls[i].addr_text);
                }
                return QQ_ERROR;
            }
#endif

            if (qq_nonblocking(s) == -1) {
                qq_log_error(errno, "set sockets nonblocking %s failed", ls[i].addr_text);
                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %s failed", ls[i].addr_text);
                }
                return QQ_ERROR;
            }

            qq_log_debug("bind() %s #%d ", ls[i].addr_text, s);

            if (bind(s, (struct sockaddr *)&ls[i].sockaddr, ls[i].socklen) == -1) {
                err = errno;

                if (err != QQ_EADDRINUSE) {
                    qq_log_error(err, "bind() to %s failed", ls[i].addr_text);
                }

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %s failed", ls[i].addr_text);
                }

                if (err != QQ_EADDRINUSE) {
                    return QQ_ERROR;
                }

                failed = 1;

                continue;
            }

            if (ls[i].type != SOCK_STREAM) {
                ls[i].fd = s;
                continue;
            }

            if (listen(s, ls[i].backlog) == -1) {
                err = errno;
                if (err != QQ_EADDRINUSE) {
                    qq_log_error(err, "listen() to %s, backlog %d failed",
                                 ls[i].addr_text, ls[i].backlog);
                }

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %s failed", ls[i].addr_text);
                }

                if (err != QQ_EADDRINUSE) {
                    return QQ_ERROR;
                }

                failed = 1;

                continue;
            }

            ls[i].fd = s;
        }

        if (failed == 0) {
            break;
        }

        qq_log_error(0, "try again to bind() after 500ms");
        usleep(500000);
    }

    if (failed) {
        qq_log_error(0, "still could not bind()");
        return QQ_ERROR;
    }

    return QQ_OK;
}

qq_int_t
qq_configure_listening_sockets(qq_cycle_t *cycle)
{
    int                        value;
    qq_uint_t                  i;
    qq_listening_t            *ls;

#if (QQ_HAVE_DEFERRED_ACCEPT && defined SO_ACCEPTFILTER)
    struct accept_filter_arg   af;
#endif

    qq_log_debug("qq_configure_listening_sockets()");

    ls = cycle->listening;
    for (i = 0; i < cycle->nlistening; i++) {
        if (setsockopt(ls[i].fd, SOL_SOCKET, SO_RCVBUF,
                      (const void *) &ls[i].rcvbuf, sizeof(int))
                == -1)
        {
            qq_log_error(errno,
                         "setsockopt(SO_RCVBUF, %d) %s failed, ignored",
                         ls[i].rcvbuf, ls[i].addr_text);
            return QQ_ERROR;
        }

        if (setsockopt(ls[i].fd, SOL_SOCKET, SO_SNDBUF,
                      (const void *) &ls[i].sndbuf, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(SO_SNDBUF, %d) %s failed, ignored",
                          ls[i].sndbuf, ls[i].addr_text);
            return QQ_ERROR;
        }

#if (QQ_HAVE_KEEPALIVE)
        value = 1;
        if (setsockopt(ls[i].fd, SOL_SOCKET, SO_KEEPALIVE,
                      (const void *) &value, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(SO_KEEPALIVE, %d) %s failed, ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }
#endif

#if (QQ_HAVE_KEEPALIVE_TUNABLE)
        value = QQ_TCP_KEEPIDLE;
        value *= QQ_KEEPALIVE_FACTOR;
        if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_KEEPIDLE,
                      (const void *) &value, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(TCP_KEEPIDLE, %d) %s failed, ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }

        value = QQ_TCP_KEEPINTVL;
        value *= QQ_KEEPALIVE_FACTOR;
        if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_KEEPINTVL,
                      (const void *) &value, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(TCP_KEEPINTVL, %d) %s failed, ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }

        
        value = QQ_TCP_KEEPCNT;
        if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_KEEPCNT,
                      (const void *) &ls[i].keepcnt, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(TCP_KEEPCNT, %d) %s failed, ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }
#endif

#if (QQ_HAVE_SETFIB)
        value = QQ_SETFIB;
        if (setsockopt(ls[i].fd, SOL_SOCKET, SO_SETFIB,
                      (const void *) &value, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(SO_SETFIB, %d) %s failed, ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }
#endif

#if (QQ_HAVE_TCP_FASTOPEN)
        value = QQ_TCP_FASTOPEN;
        if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_FASTOPEN,
                      (const void *) &value, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(TCP_FASTOPEN, %d) %s failed, ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }
#endif

        if (listen(ls[i].fd, ls[i].backlog) == -1) {
            qq_log_error(errno,
                         "listen() to %s, backlog %d failed, ignored",
                         ls[i].addr_text, ls[i].backlog);
            return QQ_ERROR;
        }

#if (QQ_HAVE_DEFERRED_ACCEPT)
#if (QQ_HAVE_ACCEPTFILTER)
        if (setsockopt(ls[i].fd, SOL_SOCKET, SO_ACCEPTFILTER, NULL, 0)
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(SO_ACCEPTFILTER, NULL) "
                         "for %s failed, ignored",
                         ls[i].addr_text);
            return QQ_ERROR;
        }

#if (QQ_HAVE_DEFERRED)
        struct accept_filter_arg  af;
        char accept_filter[16] = QQ_ACCEPT_FILTER_STR;

        memset(&af, 0, sizeof(struct accept_filter_arg));
        (void) qq_cpystrn((u_char *) af.af_name,
                          (u_char *) accept_filter, 16);

        if (setsockopt(ls[i].fd, SOL_SOCKET, SO_ACCEPTFILTER,
                       &af, sizeof(struct accept_filter_arg))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(SO_ACCEPTFILTER, \"%s\") "
                         "for %s failed, ignored",
                         accept_filter, ls[i].addr_text);
            return QQ_ERROR;
        }
#endif
#endif

#if (QQ_HAVE_TCP_DEFER_ACCEPT)
        value = 1;
        if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_DEFER_ACCEPT,
                       &value, sizeof(int))
            == -1)
        {
            qq_log_error(errno,
                         "setsockopt(TCP_DEFER_ACCEPT, %d) for %s failed, "
                         "ignored",
                         value, ls[i].addr_text);
            return QQ_ERROR;
        }
#endif
#endif /* QQ_HAVE_DEFERRED_ACCEPT */

#if (QQ_HAVE_IP_RECVDSTADDR)
        if (ls[i].type == SOCK_DGRAM
            && ls[i].sockaddr->sa_family == AF_INET)
        {
            value = 1;

            if (setsockopt(ls[i].fd, IPPROTO_IP, IP_RECVDSTADDR,
                          (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                             "setsockopt(IP_RECVDSTADDR) "
                             "for %s failed, ignored",
                             ls[i].addr_text);
                return QQ_ERROR;
            }
        }

#elif (QQ_HAVE_IP_PKTINFO)
        if (ls[i].type == SOCK_DGRAM
            && ls[i].sockaddr->sa_family == AF_INET)
        {
            value = 1;

            if (setsockopt(ls[i].fd, IPPROTO_IP, IP_PKTINFO,
                          (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                             "setsockopt(IP_PKTINFO) "
                             "for %s failed, ignored",
                             ls[i].addr_text);
                return QQ_ERROR;
            }
        }
#endif
    }

    return QQ_OK;
}

void
qq_close_listening_sockets(qq_cycle_t *cycle)
{
    qq_uint_t         i;
    qq_listening_t   *ls;
    qq_connection_t  *c;

    qq_log_debug("qq_close_listening_sockets()");

    ls = cycle->listening;
    for (i = 0; i < cycle->nlistening; i++) {
        c = ls[i].connection;
        if (c) {
            if (c->read->active) {
                qq_epoll_del_event(c->read, QQ_READ_EVENT, 0);
            }
            qq_free_connection(c);
            c->fd = (qq_socket_t) -1;
        }

        qq_log_debug("close listening %s #%d ", ls[i].addr_text, ls[i].fd);

        if (close(ls[i].fd) == -1) {
            qq_log_error(errno, "close socket %s failed", ls[i].addr_text);
        }


        ls[i].fd = (qq_socket_t) -1;
    }
}

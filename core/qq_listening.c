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
    listening->sockaddr.sin_family = AF_INET;
    listening->sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listening->sockaddr.sin_port = htons(port);

    listening->socklen = sizeof(struct sockaddr_in);
}


qq_int_t
qq_open_listening_sockets(qq_cycle_t *cycle)
{
    int              reuseaddr;
    qq_uint_t        i, tries, failed;
    qq_err_t         err;
    qq_socket_t      s;
    qq_listening_t  *ls;

    reuseaddr = 1;
#if (QQ_SUPPRESS_WARN)
    failed = 0;
#endif

    for (tries = 5; tries; tries--) {
        failed = 0;

        ls = cycle->listening;
        for (i = 0; i < cycle->nlistening; i++) {
            if (ls[i].ignore) {
                continue;
            }

#if (QQ_HAVE_REUSEPORT)
            if (ls[i].add_reuseport) {
                int  reuseport = 1;

                if (setsockopt(ls[i].fd, SOL_SOCKET, SO_REUSEPORT,
                               (const void *) &reuseport, sizeof(int))
                    == -1)
                {
                    qq_log_error(errno,
                                  "setsockopt(SO_REUSEPORT) %V failed, ignored",
                                  &ls[i].addr_text);
                }

                ls[i].add_reuseport = 0;
            }
#endif

            if (ls[i].fd != (qq_socket_t) -1) {
                continue;
            }

            if (ls[i].inherited) {
                continue;
            }

            s = socket(ls[i].sockaddr->sa_family, ls[i].type, 0);
            if (s == (qq_socket_t) -1) {
                qq_log_error(errno, "create sockets %V failed", &ls[i].addr_text);
                return QQ_ERROR;
            }

            if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                           (const void *) &reuseaddr, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_REUSEADDR) %V failed",
                              &ls[i].addr_text);

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %V failed",
                                  &ls[i].addr_text);
                }

                return QQ_ERROR;
            }

#if (QQ_HAVE_REUSEPORT)
            if (ls[i].reuseport) {
                int  reuseport;

                reuseport = 1;

                if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT,
                               (const void *) &reuseport, sizeof(int))
                    == -1)
                {
                    qq_log_error(errno,
                                  "setsockopt(SO_REUSEPORT) %V failed",
                                  &ls[i].addr_text);

                    if (close(s) == -1) {
                        qq_log_error(errno, "close sockets %V failed", &ls[i].addr_text);
                    }

                    return QQ_ERROR;
                }
            }
#endif

#if (QQ_HAVE_INET6 && defined IPV6_V6ONLY)
            if (ls[i].sockaddr->sa_family == AF_INET6) {
                int  ipv6only;

                ipv6only = ls[i].ipv6only;

                if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY,
                               (const void *) &ipv6only, sizeof(int))
                    == -1)
                {
                    qq_log_error(errno,
                                  "setsockopt(IPV6_V6ONLY) %V failed, ignored",
                                  &ls[i].addr_text);
                }
            }
#endif

            if (qq_nonblocking(s) == -1) {
                qq_log_error(errno, "set sockets nonblocking %V failed", &ls[i].addr_text);
                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %V failed", &ls[i].addr_text);
                }

                return QQ_ERROR;
            }

            qq_log_debug("bind() %V #%d ", &ls[i].addr_text, s);

            if (bind(s, ls[i].sockaddr, ls[i].socklen) == -1) {
                err = errno;

                if (err != QQ_EADDRINUSE) {
                    qq_log_error(err, "bind() to %V failed", &ls[i].addr_text);
                }

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %V failed", &ls[i].addr_text);
                }

                if (err != QQ_EADDRINUSE) {
                    return QQ_ERROR;
                }

                continue;
            }

#if (QQ_HAVE_UNIX_DOMAIN)
            if (ls[i].sockaddr->sa_family == AF_UNIX) {
                mode_t   mode;
                u_char  *name;

                name = ls[i].addr_text.data + sizeof("unix:") - 1;
                mode = (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

                if (chmod((char *) name, mode) == -1) {
                    qq_log_error(errno, "chmod() \"%s\" failed", name);
                }
            }
#endif

            if (ls[i].type != SOCK_STREAM) {
                ls[i].fd = s;
                continue;
            }

            if (listen(s, ls[i].backlog) == -1) {
                err = errno;
                if (err != QQ_EADDRINUSE) {
                    qq_log_error(err,
                                  "listen() to %V, backlog %d failed",
                                  &ls[i].addr_text, ls[i].backlog);
                }

                if (close(s) == -1) {
                    qq_log_error(errno, "close sockets %V failed", &ls[i].addr_text);
                }

                if (err != QQ_EADDRINUSE) {
                    return QQ_ERROR;
                }

                continue;
            }

            ls[i].listen = 1;
            ls[i].fd = s;
        }

        if (!failed) {
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

void
qq_configure_listening_sockets(qq_cycle_t *cycle)
{
    int                        value;
    qq_uint_t                  i;
    qq_listening_t            *ls;

#if (QQ_HAVE_DEFERRED_ACCEPT && defined SO_ACCEPTFILTER)
    struct accept_filter_arg   af;
#endif

    ls = cycle->listening;
    for (i = 0; i < cycle->nlistening; i++) {
        if (ls[i].rcvbuf != -1) {
            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_RCVBUF,
                           (const void *) &ls[i].rcvbuf, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_RCVBUF, %d) %V failed, ignored",
                              ls[i].rcvbuf, &ls[i].addr_text);
            }
        }

        if (ls[i].sndbuf != -1) {
            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_SNDBUF,
                           (const void *) &ls[i].sndbuf, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_SNDBUF, %d) %V failed, ignored",
                              ls[i].sndbuf, &ls[i].addr_text);
            }
        }

        if (ls[i].keepalive) {
            value = (ls[i].keepalive == 1) ? 1 : 0;

            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_KEEPALIVE,
                           (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_KEEPALIVE, %d) %V failed, ignored",
                              value, &ls[i].addr_text);
            }
        }

#if (QQ_HAVE_KEEPALIVE_TUNABLE)
        if (ls[i].keepidle) {
            value = ls[i].keepidle;

#if (QQ_KEEPALIVE_FACTOR)
            value *= QQ_KEEPALIVE_FACTOR;
#endif

            if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_KEEPIDLE,
                           (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(TCP_KEEPIDLE, %d) %V failed, ignored",
                              value, &ls[i].addr_text);
            }
        }

        if (ls[i].keepintvl) {
            value = ls[i].keepintvl;

#if (QQ_KEEPALIVE_FACTOR)
            value *= QQ_KEEPALIVE_FACTOR;
#endif

            if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_KEEPINTVL,
                           (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                             "setsockopt(TCP_KEEPINTVL, %d) %V failed, ignored",
                             value, &ls[i].addr_text);
            }
        }

        if (ls[i].keepcnt) {
            if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_KEEPCNT,
                           (const void *) &ls[i].keepcnt, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(TCP_KEEPCNT, %d) %V failed, ignored",
                              ls[i].keepcnt, &ls[i].addr_text);
            }
        }

#endif

#if (QQ_HAVE_SETFIB)
        if (ls[i].setfib != -1) {
            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_SETFIB,
                           (const void *) &ls[i].setfib, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_SETFIB, %d) %V failed, ignored",
                              ls[i].setfib, &ls[i].addr_text);
            }
        }
#endif

#if (QQ_HAVE_TCP_FASTOPEN)
        if (ls[i].fastopen != -1) {
            if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_FASTOPEN,
                           (const void *) &ls[i].fastopen, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(TCP_FASTOPEN, %d) %V failed, ignored",
                              ls[i].fastopen, &ls[i].addr_text);
            }
        }
#endif

        if (ls[i].listen) {
            if (listen(ls[i].fd, ls[i].backlog) == -1) {
                qq_log_error(errno,
                              "listen() to %V, backlog %d failed, ignored",
                              &ls[i].addr_text, ls[i].backlog);
            }
        }

#if (QQ_HAVE_DEFERRED_ACCEPT)
#ifdef SO_ACCEPTFILTER
        if (ls[i].delete_deferred) {
            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_ACCEPTFILTER, NULL, 0)
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_ACCEPTFILTER, NULL) "
                              "for %V failed, ignored",
                              &ls[i].addr_text);

                if (ls[i].accept_filter) {
                    qq_log_error(0,
                                  "could not change the accept filter "
                                  "to \"%s\" for %V, ignored",
                                  ls[i].accept_filter, &ls[i].addr_text);
                }

                continue;
            }

            ls[i].deferred_accept = 0;
        }

        if (ls[i].add_deferred) {
            memset(&af, 0, sizeof(struct accept_filter_arg));
            (void) qq_cpystrn((u_char *) af.af_name,
                               (u_char *) ls[i].accept_filter, 16);

            if (setsockopt(ls[i].fd, SOL_SOCKET, SO_ACCEPTFILTER,
                           &af, sizeof(struct accept_filter_arg))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(SO_ACCEPTFILTER, \"%s\") "
                              "for %V failed, ignored",
                              ls[i].accept_filter, &ls[i].addr_text);
                continue;
            }

            ls[i].deferred_accept = 1;
        }
#endif

#ifdef TCP_DEFER_ACCEPT
        if (ls[i].add_deferred || ls[i].delete_deferred) {
            if (ls[i].add_deferred) {
                value = 1;
            } else {
                value = 0;
            }

            if (setsockopt(ls[i].fd, IPPROTO_TCP, TCP_DEFER_ACCEPT,
                           &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(TCP_DEFER_ACCEPT, %d) for %V failed, "
                              "ignored",
                              value, &ls[i].addr_text);

                continue;
            }
        }

        if (ls[i].add_deferred) {
            ls[i].deferred_accept = 1;
        }

#endif

#endif /* QQ_HAVE_DEFERRED_ACCEPT */

#if (QQ_HAVE_IP_RECVDSTADDR)
        if (ls[i].wildcard
            && ls[i].type == SOCK_DGRAM
            && ls[i].sockaddr->sa_family == AF_INET)
        {
            value = 1;

            if (setsockopt(ls[i].fd, IPPROTO_IP, IP_RECVDSTADDR,
                           (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(IP_RECVDSTADDR) "
                              "for %V failed, ignored",
                              &ls[i].addr_text);
            }
        }

#elif (QQ_HAVE_IP_PKTINFO)
        if (ls[i].wildcard
            && ls[i].type == SOCK_DGRAM
            && ls[i].sockaddr->sa_family == AF_INET)
        {
            value = 1;

            if (setsockopt(ls[i].fd, IPPROTO_IP, IP_PKTINFO,
                           (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(IP_PKTINFO) "
                              "for %V failed, ignored",
                              &ls[i].addr_text);
            }
        }

#endif

#if (QQ_HAVE_INET6 && QQ_HAVE_IPV6_RECVPKTINFO)
        if (ls[i].wildcard
            && ls[i].type == SOCK_DGRAM
            && ls[i].sockaddr->sa_family == AF_INET6)
        {
            value = 1;

            if (setsockopt(ls[i].fd, IPPROTO_IPV6, IPV6_RECVPKTINFO,
                           (const void *) &value, sizeof(int))
                == -1)
            {
                qq_log_error(errno,
                              "setsockopt(IPV6_RECVPKTINFO) "
                              "for %V failed, ignored",
                              &ls[i].addr_text);
            }
        }
#endif
    }

    return;
}

void
qq_close_listening_sockets(qq_cycle_t *cycle)
{
    qq_uint_t         i;
    qq_listening_t   *ls;
    qq_connection_t  *c;

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

        qq_log_debug("close listening %V #%d ", &ls[i].addr_text, ls[i].fd);

        if (close(ls[i].fd) == -1) {
            qq_log_error(errno, "close socket %V failed", &ls[i].addr_text);
        }

#if (QQ_HAVE_UNIX_DOMAIN)
        if (ls[i].sockaddr->sa_family == AF_UNIX) {
            u_char *name = ls[i].addr_text.data + sizeof("unix:") - 1;

            if (qq_delete_file(name) == QQ_FILE_ERROR) {
                qq_log_error(errno, "delete file %s failed", name);
            }
        }
#endif

        ls[i].fd = (qq_socket_t) -1;
    }
}

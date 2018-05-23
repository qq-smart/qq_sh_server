/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static qq_int_t qq_enable_accept_events(qq_cycle_t *cycle);
static qq_int_t qq_disable_accept_events(qq_cycle_t *cycle);
static void qq_close_accepted_connection(qq_connection_t *c);


void
qq_event_accept(qq_event_t *ev)
{
    socklen_t          socklen;
    qq_err_t           err;
    qq_socket_t        s;
    qq_event_t        *rev, *wev;
    qq_sockaddr_t      sa;
    qq_listening_t    *ls;
    qq_connection_t   *c, *lc;
#if (QQ_HAVE_ACCEPT4)
    static qq_uint_t   use_accept4 = 1;
#endif

    if (ev->timedout) {
        if (qq_enable_accept_events((qq_cycle_t *) qq_cycle) != QQ_OK) {
            return;
        }

        ev->timedout = 0;
    }

    lc = ev->data;
    ls = lc->listening;
    ev->ready = 0;

    qq_log_debug(0, "accept on %V, ready: %d", &ls->addr_text, ev->available);

    do {
        socklen = sizeof(qq_sockaddr_t);

#if (QQ_HAVE_ACCEPT4)
        s = accept4(lc->fd, &sa.sockaddr, &socklen, SOCK_NONBLOCK);
#else
        s = accept(lc->fd, &sa.sockaddr, &socklen);
#endif

        if (s == (qq_socket_t) -1) {
            err = errno;

            if (err == QQ_EAGAIN) {
                qq_log_debug("accept() not ready");
                return;
            }

#if (NGX_HAVE_ACCEPT4)
            qq_log_error(err, "accept4() failed");
            if (err == QQ_ENOSYS) {
                continue;
            }
#else
            qq_log_error(err, "accept() failed");
#endif

            if (err == QQ_ECONNABORTED) {
                if (ev->available) {
                    continue;
                }
            }

            if (err == QQ_EMFILE || err == QQ_ENFILE) {
                if (qq_disable_accept_events((qq_cycle_t *) qq_cycle)
                    != QQ_OK)
                {
                    return;
                }
            }

            return;
        }

        c = qq_get_connection(s);
        if (c == NULL) {
            if (close(s) == -1) {
                qq_log_error(errno, "close sockets failed");
            }

            return;
        }
        c->type = SOCK_STREAM;

        c->pool = qq_create_pool(ls->pool_size);
        if (c->pool == NULL) {
            qq_close_accepted_connection(c);
            return;
        }

        if (socklen > (socklen_t) sizeof(qq_sockaddr_t)) {
            socklen = sizeof(qq_sockaddr_t);
        }
        c->sockaddr = qq_palloc(c->pool, socklen);
        if (c->sockaddr == NULL) {
            qq_close_accepted_connection(c);
            return;
        }
        memcpy(c->sockaddr, &sa, socklen);

        if (qq_nonblocking(s) == -1) {
            qq_log_error(errno, "set sockets nonblocking_n failed");
                    qq_close_accepted_connection(c);
            return;
        }

        /*c->recv = qq_recv;
        c->send = qq_send;
        c->recv_chain = qq_recv_chain;
        c->send_chain = qq_send_chain;*/

        c->socklen = socklen;
        c->listening = ls;
        c->local_sockaddr = ls->sockaddr;
        c->local_socklen = ls->socklen;

#if (NGX_HAVE_UNIX_DOMAIN)
        if (c->sockaddr->sa_family == AF_UNIX) {
            c->tcp_nopush = QQ_TCP_NOPUSH_DISABLED;
            c->tcp_nodelay = QQ_TCP_NODELAY_DISABLED;
#if (NGX_SOLARIS)
            /* Solaris's sendfilev() supports AF_NCA, AF_INET, and AF_INET6 */
            c->sendfile = 0;
#endif
        }
#endif

        rev = c->read;
        wev = c->write;

        wev->ready = 1;

        if (ev->deferred_accept) {
            rev->ready = 1;
        }

        c->addr_text.data = qq_pnalloc(c->pool, ls->addr_text_max_len);
        if (c->addr_text.data == NULL) {
            qq_close_accepted_connection(c);
            return;
        }

        c->addr_text.len = qq_sock_ntop(c->sockaddr, c->socklen,
                                        c->addr_text.data,
                                        ls->addr_text_max_len, 0);
        if (c->addr_text.len == 0) {
            qq_close_accepted_connection(c);
            return;
        }

        if (qq_epoll_add_connection(c) == QQ_ERROR) {
            qq_close_accepted_connection(c);
            return;
        }

        ls->handler(c);
    } while (ev->available);
}

void
qq_event_recvmsg(qq_event_t *ev)
{
    ssize_t            n;
    qq_err_t           err;
    qq_event_t        *rev, *wev;
    struct iovec       iov[1];
    struct msghdr      msg;
    qq_sockaddr_t      sa;
    qq_listening_t    *ls;
    qq_connection_t   *c, *lc;
    static u_char      buffer[65535];

#if (QQ_HAVE_MSGHDR_MSG_CONTROL)

#if (QQ_HAVE_IP_RECVDSTADDR)
    u_char             msg_control[CMSG_SPACE(sizeof(struct in_addr))];
#elif (QQ_HAVE_IP_PKTINFO)
    u_char             msg_control[CMSG_SPACE(sizeof(struct in_pktinfo))];
#endif

#if (QQ_HAVE_INET6 && QQ_HAVE_IPV6_RECVPKTINFO)
    u_char             msg_control6[CMSG_SPACE(sizeof(struct in6_pktinfo))];
#endif

#endif

    if (ev->timedout) {
        if (qq_enable_accept_events((qq_cycle_t *) qq_cycle) != QQ_OK) {
            return;
        }

        ev->timedout = 0;
    }

    lc = ev->data;
    ls = lc->listening;
    ev->ready = 0;

    qq_log_debug("recvmsg on %V, ready: %d", &ls->addr_text, ev->available);

    do {
        memset(&msg, 0, sizeof(struct msghdr));

        iov[0].iov_base = (void *) buffer;
        iov[0].iov_len = sizeof(buffer);

        msg.msg_name = &sa;
        msg.msg_namelen = sizeof(qq_sockaddr_t);
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;

#if (QQ_HAVE_MSGHDR_MSG_CONTROL)

        if (ls->wildcard) {

#if (QQ_HAVE_IP_RECVDSTADDR || QQ_HAVE_IP_PKTINFO)
            if (ls->sockaddr->sa_family == AF_INET) {
                msg.msg_control = &msg_control;
                msg.msg_controllen = sizeof(msg_control);
            }
#endif

#if (QQ_HAVE_INET6 && QQ_HAVE_IPV6_RECVPKTINFO)
            if (ls->sockaddr->sa_family == AF_INET6) {
                msg.msg_control = &msg_control6;
                msg.msg_controllen = sizeof(msg_control6);
            }
#endif
        }

#endif

        n = recvmsg(lc->fd, &msg, 0);

        if (n == -1) {
            err = errno;
            if (err == QQ_EAGAIN) {
                qq_log_debug("recvmsg() not ready");
                return;
            }

            qq_log_error(err, "recvmsg() failed");

            return;
        }


#if (QQ_HAVE_MSGHDR_MSG_CONTROL)
        if (msg.msg_flags & (MSG_TRUNC|MSG_CTRUNC)) {
            qq_log_error(0, "recvmsg() truncated data");
            continue;
        }
#endif

        c = qq_get_connection(lc->fd);
        if (c == NULL) {
            return;
        }

        c->shared = 1;
        c->type = SOCK_DGRAM;
        c->socklen = msg.msg_namelen;

        if (c->socklen > (socklen_t) sizeof(qq_sockaddr_t)) {
            c->socklen = sizeof(qq_sockaddr_t);
        }

        c->pool = qq_create_pool(ls->pool_size);
        if (c->pool == NULL) {
            qq_close_accepted_connection(c);
            return;
        }

        c->sockaddr = qq_palloc(c->pool, c->socklen);
        if (c->sockaddr == NULL) {
            qq_close_accepted_connection(c);
            return;
        }

        memcpy(c->sockaddr, msg.msg_name, c->socklen);

        /*c->send = qq_udp_send;
        c->send_chain = qq_udp_send_chain;*/

        c->listening = ls;
        c->local_sockaddr = ls->sockaddr;
        c->local_socklen = ls->socklen;

#if (QQ_HAVE_MSGHDR_MSG_CONTROL)

        if (ls->wildcard) {
            struct cmsghdr   *cmsg;
            struct sockaddr  *sockaddr;

            sockaddr = qq_palloc(c->pool, c->local_socklen);
            if (sockaddr == NULL) {
                qq_close_accepted_connection(c);
                return;
            }

            memcpy(sockaddr, c->local_sockaddr, c->local_socklen);
            c->local_sockaddr = sockaddr;

            for (cmsg = CMSG_FIRSTHDR(&msg);
                 cmsg != NULL;
                 cmsg = CMSG_NXTHDR(&msg, cmsg))
            {

#if (QQ_HAVE_IP_RECVDSTADDR)

                if (cmsg->cmsg_level == IPPROTO_IP
                    && cmsg->cmsg_type == IP_RECVDSTADDR
                    && sockaddr->sa_family == AF_INET)
                {
                    struct in_addr      *addr;
                    struct sockaddr_in  *sin;

                    addr = (struct in_addr *) CMSG_DATA(cmsg);
                    sin = (struct sockaddr_in *) sockaddr;
                    sin->sin_addr = *addr;

                    break;
                }

#elif (QQ_HAVE_IP_PKTINFO)

                if (cmsg->cmsg_level == IPPROTO_IP
                    && cmsg->cmsg_type == IP_PKTINFO
                    && sockaddr->sa_family == AF_INET)
                {
                    struct in_pktinfo   *pkt;
                    struct sockaddr_in  *sin;

                    pkt = (struct in_pktinfo *) CMSG_DATA(cmsg);
                    sin = (struct sockaddr_in *) sockaddr;
                    sin->sin_addr = pkt->ipi_addr;

                    break;
                }

#endif

#if (QQ_HAVE_INET6 && QQ_HAVE_IPV6_RECVPKTINFO)

                if (cmsg->cmsg_level == IPPROTO_IPV6
                    && cmsg->cmsg_type == IPV6_PKTINFO
                    && sockaddr->sa_family == AF_INET6)
                {
                    struct in6_pktinfo   *pkt6;
                    struct sockaddr_in6  *sin6;

                    pkt6 = (struct in6_pktinfo *) CMSG_DATA(cmsg);
                    sin6 = (struct sockaddr_in6 *) sockaddr;
                    sin6->sin6_addr = pkt6->ipi6_addr;

                    break;
                }

#endif

            }
        }

#endif

        /*c->buffer = qq_create_temp_buf(c->pool, n);
        if (c->buffer == NULL) {
            qq_close_accepted_connection(c);
            return;
        }
        c->buffer->last = qq_cpymem(c->buffer->last, buffer, n);*/

        rev = c->read;
        wev = c->write;

        wev->ready = 1;

        c->addr_text.data = qq_pnalloc(c->pool, ls->addr_text_max_len);
        if (c->addr_text.data == NULL) {
            qq_close_accepted_connection(c);
            return;
        }

        c->addr_text.len = qq_sock_ntop(c->sockaddr, c->socklen,
                                        c->addr_text.data,
                                        ls->addr_text_max_len, 0);
        if (c->addr_text.len == 0) {
            qq_close_accepted_connection(c);
            return;
        }

        ls->handler(c);
    } while (ev->available);
}


static qq_int_t
qq_enable_accept_events(qq_cycle_t *cycle)
{
    qq_uint_t         i;
    qq_listening_t   *ls;
    qq_connection_t  *c;

    ls = cycle->listening;
    for (i = 0; i < cycle->nlistening; i++) {
        c = ls[i].connection;
        if (c == NULL || c->read->active) {
            continue;
        }

        if (qq_epoll_add_event(c->read, QQ_READ_EVENT) == QQ_ERROR) {
            return QQ_ERROR;
        }
    }

    return QQ_OK;
}

static qq_int_t
qq_disable_accept_events(qq_cycle_t *cycle)
{
    qq_uint_t         i;
    qq_listening_t   *ls;
    qq_connection_t  *c;

    ls = cycle->listening;
    for (i = 0; i < cycle->nlistening; i++) {
        c = ls[i].connection;
        if (c == NULL || !c->read->active) {
            continue;
        }

        if (qq_epoll_del_event(c->read, QQ_READ_EVENT, QQ_DISABLE_EVENT)
            == QQ_ERROR)
        {
            return QQ_ERROR;
        }
    }

    return QQ_OK;
}

static void
qq_close_accepted_connection(qq_connection_t *c)
{
    qq_socket_t  fd;

    qq_free_connection(c);

    fd = c->fd;
    c->fd = (qq_socket_t) -1;

    if (close(fd) == -1) {
        qq_log_error(errno, "close sockets failed");
    }
}

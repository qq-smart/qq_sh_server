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
    struct sockaddr    sa;
    qq_listening_t    *ls;
    qq_connection_t   *c, *lc;

    qq_log_debug("qq_event_accept(%p)", ev);

    if (ev->timedout) {
        if (qq_enable_accept_events((qq_cycle_t *) qq_cycle) != QQ_OK) {
            return;
        }
        ev->timedout = 0;
    }

    lc = ev->data;
    ls = lc->listening;
    ev->ready = 0;

    qq_log_debug("accept on %s", ls->addr_text);

    for (;;) {
        socklen = sizeof(struct sockaddr);

        s = accept(lc->fd, &sa, &socklen);
        if (s == (qq_socket_t) -1) {
            err = errno;

            if (err == QQ_EAGAIN) {
                qq_log_debug("accept() not ready");
                return;
            }

            qq_log_error(err, "accept() failed");

            if (err == QQ_ECONNABORTED) {
                continue;
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

        c->recv = qq_tcp_recv;
        c->send = qq_tcp_send;

        c->socklen = socklen;
        c->listening = ls;
        c->local_sockaddr = (struct sockaddr*) &ls->sockaddr;
        c->local_socklen = ls->socklen;

        rev = c->read;
        wev = c->write;
        wev->ready = 1;

        if (ev->deferred_accept) {
            rev->ready = 1;
        }

        c->addr_text = qq_pnalloc(c->pool, QQ_SOCKADDR_STRLEN);
        if (c->addr_text == NULL) {
            qq_close_accepted_connection(c);
            return;
        }
        memset(c->addr_text, 0, QQ_SOCKADDR_STRLEN);
        c->addr_text_len = qq_sock_ntop((struct sockaddr_in *)c->sockaddr, c->addr_text);
        if (c->addr_text_len == 0) {
            qq_close_accepted_connection(c);
            return;
        }        
        qq_log_debug("connection address: %s, address len: %d",
            c->addr_text, c->addr_text_len);

        if (qq_epoll_add_connection(c) == QQ_ERROR) {
            qq_close_accepted_connection(c);
            return;
        }

        ls->handler(c);
        return;
    }
}

void
qq_event_recvmsg(qq_event_t *ev)
{
    ssize_t            n;
    size_t             size;
    qq_err_t           err;
    struct iovec       iov[1];
    struct msghdr      msg;
    struct sockaddr    sa;
    qq_listening_t    *ls;
    qq_connection_t   *c;

    if (ev->timedout) {
        if (qq_enable_accept_events((qq_cycle_t *) qq_cycle) != QQ_OK) {
            return;
        }
        ev->timedout = 0;
    }

    c = ev->data;
    ls = c->listening;
    ev->ready = 0;

    qq_log_debug("qq_event_recvmsg() on %s, ready: %d", ls->addr_text, ev->available);

    if (c->pool == NULL) {
        qq_log_debug("c->pool == NULL");

        c->pool = qq_create_pool(ls->pool_size);
        if (c->pool == NULL) {
            qq_log_error(0, "qq_create_pool(%d) failed", ls->pool_size);
            return;
        }

        c->socklen = sizeof(struct sockaddr);
        c->sockaddr = qq_palloc(c->pool, c->socklen);
        if (c->sockaddr == NULL) {
            qq_destroy_pool(c->pool);
            c->pool = NULL;
            return;
        }

        c->addr_text = qq_palloc(c->pool, QQ_SOCKADDR_STRLEN);
        if (c->addr_text == NULL) {
            qq_destroy_pool(c->pool);
            c->pool = NULL;
            return;
        }

        c->buffer = qq_pcalloc(c->pool, sizeof(qq_buf_t));
        if (c->buffer == NULL) {
            qq_destroy_pool(c->pool);
            c->pool = NULL;
            return;
        }
        size = ls->pool_size -
            ((size_t)((uintptr_t)c->buffer - (uintptr_t)c->pool) + sizeof(qq_buf_t));
        c->buffer->start = qq_pnalloc(c->pool, size);
        if (c->buffer->start == NULL) {
            qq_destroy_pool(c->pool);
            c->pool = NULL;
            return;
        }
        c->buffer->pos = c->buffer->start;
        c->buffer->last = c->buffer->start;
        c->buffer->end = c->buffer->last + size;
        c->buffer->temporary = 1;

        c->shared = 1;
        c->send = qq_udp_send;
        c->local_sockaddr = (struct sockaddr *) &ls->sockaddr;
        c->local_socklen = ls->socklen;
    }

    memset(&msg, 0, sizeof(struct msghdr));

    iov[0].iov_base = (void *) c->buffer->pos;
    iov[0].iov_len = c->buffer->end - c->buffer->start;
    qq_log_debug("recvmsg(size: %d)", c->buffer->end - c->buffer->start);

    msg.msg_name = &sa;
    msg.msg_namelen = sizeof(struct sockaddr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    n = recvmsg(c->fd, &msg, 0);

    if (n == -1) {
        err = errno;
        if (err == QQ_EAGAIN) {
            qq_log_debug("recvmsg() not ready");
            return;
        }

        qq_log_error(err, "recvmsg() failed");
        return;
    }

    memcpy(c->sockaddr, msg.msg_name, c->socklen);

    c->buffer->last += n;

    c->write->ready = 1;

    memset(c->addr_text, 0, QQ_SOCKADDR_STRLEN);
    c->addr_text_len = qq_sock_ntop((struct sockaddr_in *)c->sockaddr, c->addr_text);
    if (c->addr_text_len != 0) {
        qq_log_debug("connection address: %s, address len: %d",
            c->addr_text, c->addr_text_len);
    }

    ls->handler(c);
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

        if (qq_epoll_del_event(c->read, QQ_READ_EVENT)
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

    if (c->pool) {
        qq_destroy_pool(c->pool);
    }
}

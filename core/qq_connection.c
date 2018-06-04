/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_connection_t *
qq_get_connection(qq_socket_t s)
{
    qq_uint_t         instance;
    qq_event_t       *rev, *wev;
    qq_connection_t  *c;

    c = qq_cycle->free_connections;
    if (c == NULL) {
        qq_log_error(0, 
                     "qq_get_connection(connection_n:%u) worker_connections are not enough",
                     qq_cycle->connection_n);

        return NULL;
    }

    qq_cycle->free_connections = c->data;
    qq_cycle->free_connection_n--;

    rev = c->read;
    wev = c->write;

    memset(c, 0, sizeof(qq_connection_t));

    c->read = rev;
    c->write = wev;
    c->fd = s;

    instance = rev->instance;

    memset(rev, 0, sizeof(qq_event_t));
    memset(wev, 0, sizeof(qq_event_t));

    rev->instance = !instance;
    wev->instance = !instance;

    rev->index = QQ_INVALID_INDEX;
    wev->index = QQ_INVALID_INDEX;

    rev->data = c;
    wev->data = c;

    wev->write = 1;

    return c;
}

void
qq_free_connection(qq_connection_t *c)
{
    c->data = qq_cycle->free_connections;
    qq_cycle->free_connections = c;
    qq_cycle->free_connection_n++;
}

void
qq_close_connection(qq_connection_t *c)
{
    qq_socket_t  fd;

    if (c->fd == (qq_socket_t) -1) {
        qq_log_error(0, "qq_close_connection() connection already closed");
        return;
    }

    if (c->read->timer_set) {
        qq_event_del_timer(c->read);
    }

    if (c->write->timer_set) {
        qq_event_del_timer(c->write);
    }

    qq_epoll_del_connection(c);

    if (c->read->posted) {
        qq_delete_posted_event(c->read);
    }

    if (c->write->posted) {
        qq_delete_posted_event(c->write);
    }

    c->read->closed = 1;
    c->write->closed = 1;

    qq_free_connection(c);

    fd = c->fd;
    c->fd = (qq_socket_t) -1;

    if (close(fd) == -1) {
        qq_log_error(errno, "qq_close_connection(fd:%d) failed", fd);
    }
}

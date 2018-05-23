/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_int_t
qq_event_init(qq_cycle_t *cycle)
{
    qq_uint_t           i;
    qq_event_t         *rev, *wev;
    qq_listening_t     *ls;
    qq_connection_t    *c, *next;

    if (qq_event_timer_init() == QQ_ERROR) {
        return QQ_ERROR;
    }
	
    if (qq_epoll_init() == QQ_ERROR) {
        return QQ_ERROR;
    }

    cycle->connections = qq_alloc(sizeof(qq_connection_t) * cycle->connection_n);
    if (cycle->connections == NULL) {
        return QQ_ERROR;
    }
    c = cycle->connections;

    cycle->read_events = qq_alloc(sizeof(qq_event_t) * cycle->connection_n);
    if (cycle->read_events == NULL) {
        return QQ_ERROR;
    }
    rev = cycle->read_events;
    for (i = 0; i < cycle->connection_n; i++) {
        rev[i].closed = 1;
        rev[i].instance = 1;
    }

    cycle->write_events = qq_alloc(sizeof(qq_event_t) * cycle->connection_n);
    if (cycle->write_events == NULL) {
        return QQ_ERROR;
    }
    wev = cycle->write_events;
    for (i = 0; i < cycle->connection_n; i++) {
        wev[i].closed = 1;
    }

    i = cycle->connection_n;
    next = NULL;
    do {
        i--;

        c[i].data = next;
        c[i].read = &cycle->read_events[i];
        c[i].write = &cycle->write_events[i];
        c[i].fd = (qq_socket_t) -1;

        next = &c[i];
    } while (i);
    cycle->free_connections = next;
    cycle->free_connection_n = cycle->connection_n;

    ls = cycle->listening;
    for (i = 0; i < cycle->nlistening; i++) {
        c = qq_get_connection(ls[i].fd);
        if (c == NULL) {
            return QQ_ERROR;
        }

        c->type = ls[i].type;
        c->listening = &ls[i];
        ls[i].connection = c;

        rev = c->read;
        rev->accept = 1;

        rev->handler = (c->type == SOCK_STREAM) ? qq_event_accept
                                                : qq_event_recvmsg;

        if (qq_epoll_add_event(rev, QQ_READ_EVENT) == QQ_ERROR) {
            return QQ_ERROR;
        }
    }

    return QQ_OK;
}

void
qq_events_process(void)
{
    qq_msec_t  delta;

    delta = qq_current_msec;
    qq_epoll_process_events(qq_event_find_timer(), QQ_UPDATE_TIME);
    delta = qq_current_msec - delta;

    qq_log_debug("timer delta: %d", delta);

    if (delta) {
        qq_event_expire_timers();
    }
}

qq_int_t
qq_handle_read_event(qq_event_t *rev)
{
    if (!rev->active && !rev->ready) {
        if (qq_epoll_add_event(rev, QQ_READ_EVENT)
            == QQ_ERROR)
        {
            return QQ_ERROR;
        }
    }

    return QQ_OK;
}


qq_int_t
qq_handle_write_event(qq_event_t *wev)
{
    if (!wev->active && !wev->ready) {
        if (qq_epoll_add_event(wev, QQ_WRITE_EVENT)
            == QQ_ERROR)
        {
            return QQ_ERROR;
        }
    }

    return QQ_OK;
}

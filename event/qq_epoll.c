/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static int                  ep = -1;
static struct epoll_event  *event_list = NULL;
static qq_uint_t            nevents;


qq_int_t
qq_epoll_init(qq_cycle_t *cycle)
{
    if (ep == -1) {
        ep = epoll_create(cycle->connection_n / 2);

        if (ep == -1) {
            qq_log_error("epoll_create() failed");
            return QQ_ERROR;
        }
    }

    nevents = QQ_EVENT_NUMBER;

    if (event_list) {
        free(event_list);
    }

    event_list = qq_alloc(sizeof(struct epoll_event) * nevents);
    if (event_list == NULL) {
        return QQ_ERROR;
    }

    return QQ_OK;
}

void
qq_epoll_done(qq_cycle_t *cycle)
{
    if (close(ep) == -1) {
        qq_log_error("epoll close() failed");
    }

    ep = -1;

    free(event_list);

    event_list = NULL;
    nevents = 0;
}

qq_int_t
qq_epoll_add_event(qq_event_t *ev, qq_int_t event)
{
    int                  op;
    uint32_t             events, prev;
    qq_event_t          *e;
    qq_connection_t     *c;
    struct epoll_event   ee;

    c = ev->data;

    if (event == QQ_READ_EVENT) {
        e = c->write;
        prev = EPOLLOUT;
        events = EPOLLIN|EPOLLRDHUP;
    } else {
        e = c->read;
        prev = EPOLLIN|EPOLLRDHUP;
        events = EPOLLOUT;
    }

    if (e->active) {
        op = EPOLL_CTL_MOD;
        events |= prev;
    } else {
        op = EPOLL_CTL_ADD;
    }

    ee.events = events | EPOLLET;
    ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);

    qq_log_debug("epoll add event: fd:%d op:%d ev:%08XD", c->fd, op, ee.events);

    if (epoll_ctl(ep, op, c->fd, &ee) == -1) {
        ngx_log_error("epoll_ctl(%d, %d) failed", op, c->fd);
        return QQ_ERROR;
    }

    ev->active = 1;

    return QQ_OK;
}

qq_int_t
qq_epoll_del_event(qq_event_t *ev, qq_int_t event, qq_uint_t flags)
{
    int                  op;
    uint32_t             prev;
    qq_event_t          *e;
    qq_connection_t     *c;
    struct epoll_event   ee;

    if (flags & QQ_CLOSE_EVENT) {
        ev->active = 0;
        return QQ_OK;
    }

    c = ev->data;

    if (event == QQ_READ_EVENT) {
        e = c->write;
        prev = EPOLLOUT;
    } else {
        e = c->read;
        prev = EPOLLIN|EPOLLRDHUP;
    }

    if (e->active) {
        op = EPOLL_CTL_MOD;
        ee.events = prev | EPOLLET;
        ee.data.ptr = (void *) ((uintptr_t) c | ev->instance);
    } else {
        op = EPOLL_CTL_DEL;
        ee.events = 0;
        ee.data.ptr = NULL;
    }

    qq_log_debug("epoll del event: fd:%d op:%d ev:%08XD", c->fd, op, ee.events);

    if (epoll_ctl(ep, op, c->fd, &ee) == -1) {
        qq_log_error("epoll_ctl(%d, %d) failed", op, c->fd);
        return QQ_ERROR;
    }

    ev->active = 0;

    return QQ_OK;
}

qq_int_t
qq_epoll_add_connection(qq_connection_t *c)
{
    struct epoll_event  ee;

    ee.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
    ee.data.ptr = (void *) ((uintptr_t) c | c->read->instance);

    qq_log_debug("epoll add connection: fd:%d ev:%08XD", c->fd, ee.events);

    if (epoll_ctl(ep, EPOLL_CTL_ADD, c->fd, &ee) == -1) {
        qq_log_error("epoll_ctl(EPOLL_CTL_ADD, %d) failed", c->fd);
        return QQ_ERROR;
    }

    c->read->active = 1;
    c->write->active = 1;

    return QQ_OK;
}

qq_int_t
qq_epoll_del_connection(qq_connection_t *c, qq_uint_t flags)
{
    int                 op;
    struct epoll_event  ee;

    if (flags & QQ_CLOSE_EVENT) {
        c->read->active = 0;
        c->write->active = 0;
        return QQ_OK;
    }

    qq_log_debug("epoll del connection: fd:%d", c->fd);

    op = EPOLL_CTL_DEL;
    ee.events = 0;
    ee.data.ptr = NULL;

    if (epoll_ctl(ep, op, c->fd, &ee) == -1) {
        qq_log_error("epoll_ctl(%d, %d) failed", op, c->fd);
        return QQ_ERROR;
    }

    c->read->active = 0;
    c->write->active = 0;

    return QQ_OK;
}

qq_int_t
qq_epoll_process_events(qq_cycle_t *cycle, qq_msec_t timer, qq_uint_t flags)
{
    int                events;
    uint32_t           revents;
    qq_int_t           instance, i;
    qq_uint_t          level;
    qq_err_t           err;
    qq_event_t        *rev, *wev;
    qq_queue_t        *queue;
    qq_connection_t   *c;

    qq_log_debug("epoll timer: %d", timer);

    events = epoll_wait(ep, event_list, (int) nevents, timer);

    err = (events == -1) ? qq_errno : 0;

    if (flags & QQ_UPDATE_TIME) {
        qq_time_update();
    }

    if (err) {
        qq_log_error("epoll_wait() failed");
        return QQ_ERROR;
    }

    if (events == 0) {
        if (timer != QQ_TIMER_INFINITE) {
            return QQ_OK;
        }

        qq_log_error("epoll_wait() returned no events without timeout");
        return QQ_ERROR;
    }

    for (i = 0; i < events; i++) {
        c = event_list[i].data.ptr;

        instance = (uintptr_t) c & 1;
        c = (qq_connection_t *) ((uintptr_t) c & (uintptr_t) ~1);

        rev = c->read;

        if (c->fd == -1 || rev->instance != instance) {
            qq_log_debug("epoll: stale event %p", c);
            continue;
        }

        revents = event_list[i].events;

        qq_log_debug("epoll: fd:%d ev:%04XD d:%p", c->fd, revents, event_list[i].data.ptr);

        if (revents & (EPOLLERR|EPOLLHUP)) {
            qq_log_debug("epoll_wait() error on fd:%d ev:%04XD", c->fd, revents);
            revents |= EPOLLIN|EPOLLOUT;
        }

        if ((revents & EPOLLIN) && rev->active) {
            rev->ready = 1;

            if (flags & QQ_POST_EVENTS) {
                queue = rev->accept ? &qq_posted_accept_events
                                    : &qq_posted_events;

                qq_post_event(rev, queue);

            } else {
                rev->handler(rev);
            }
        }

        wev = c->write;

        if ((revents & EPOLLOUT) && wev->active) {
            if (c->fd == -1 || wev->instance != instance) {
                qq_log_debug("epoll: stale event %p", c);
                continue;
            }

            wev->ready = 1;

            if (flags & QQ_POST_EVENTS) {
                qq_post_event(wev, &qq_posted_events);

            } else {
                wev->handler(wev);
            }
        }
    }

    return QQ_OK;
}

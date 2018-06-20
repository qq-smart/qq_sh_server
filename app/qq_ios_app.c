/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_ios_app.h"
#include "qq_app_core.h"


static void qq_ios_app_init_connection_handler(qq_connection_t *c);
static void qq_ios_app_wait_request_handler(qq_event_t *ev);
static void qq_ios_app_process_request_handler(qq_event_t *rev);
static void qq_ios_app_process_package(qq_connection_t *c);
static void qq_ios_app_write_event_handler(qq_event_t *ev);


qq_int_t
qq_ios_app_init(qq_cycle_t *cycle)
{
    if (qq_add_listening_config(SOCK_STREAM,
        QQ_IOS_APP_TCP_LISTENING_PORT,
        QQ_IOS_APP_TCP_LISTENING_POOL_SIZE,
        qq_ios_app_init_connection_handler) == QQ_ERROR)
    {
        qq_log_error(0, "qq_ios_app_init()->qq_add_listening_config() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

void
qq_ios_app_done(void)
{
    qq_log_debug("qq_ios_app_done()");
}

qq_ios_app_t *
qq_ios_app_node(char *id)
{
    return NULL;
}

void
qq_ios_app_and_device_associate(void *app_id, void *device_id)
{
}


static void
qq_ios_app_init_connection_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_ios_app_init_connection_handler()");

    rev = c->read;
    rev->handler = qq_ios_app_wait_request_handler;

    wev = c->write;
    wev->handler = qq_ios_app_write_event_handler;

    qq_event_add_timer(rev, QQ_IOS_APP_ACCEPT_TIMEOUT);
}


static void
qq_ios_app_wait_request_handler(qq_event_t *rev)
{
    qq_connection_t   *c;
    ssize_t            n;
    size_t             size;
    qq_buf_t          *b;
    qq_listening_t    *ls;

    c = rev->data;
    ls = c->listening;

    qq_log_debug("qq_ios_app_wait_request_handler()");

    if (rev->timedout) {
        qq_app_close_connection(c);
        return;
    }

    if (c->close) {
        qq_app_close_connection(c);
        return;
    }

    b = c->buffer;
    if (b == NULL) {
        b = qq_pcalloc(c->pool, sizeof(qq_buf_t));
        if (b == NULL) {
            qq_app_close_connection(c);
            return;
        }
        c->buffer = b;

        size = ls->pool_size -
            ((size_t)((uintptr_t)b - (uintptr_t)c->pool) + sizeof(qq_buf_t));
        b->start = qq_pnalloc(c->pool, size);
        if (b->start == NULL) {
            qq_app_close_connection(c);
            return;
        }
        b->pos = b->start;
        b->last = b->start;
        b->end = b->last + size;
        b->temporary = 1;
    }

    n = c->recv(c, b->pos, (size_t)((uintptr_t)b->end - (uintptr_t)b->start));
    if (n == QQ_AGAIN) {
        if (!rev->timer_set) {
            qq_event_add_timer(rev, QQ_IOS_APP_ACCEPT_TIMEOUT);
        }
        return;
    }

    if (n == QQ_ERROR) {
        qq_app_close_connection(c);
        return;
    }

    if (n == 0) {
        qq_log_error(0, "client closed connection");
        qq_app_close_connection(c);
        return;
    }
    b->last += n;

    rev->handler = qq_ios_app_process_request_handler;
    qq_ios_app_process_request_handler(rev);
}

static void
qq_ios_app_process_request_handler(qq_event_t *rev)
{
    qq_connection_t   *c;
    ssize_t            n;
    size_t             size;

    c = rev->data;

    if (c->buffer->last - c->buffer->pos > 0) {
        qq_ios_app_process_package(c);
        c->buffer->last = c->buffer->pos;
        return;
    }

    n = c->recv(c, (u_char *)c->buffer->pos,
        (size_t)((uintptr_t)c->buffer->end - (uintptr_t)c->buffer->start));
    if (n == QQ_AGAIN) {
        if (qq_handle_read_event(rev) != QQ_OK) {
            qq_app_close_connection(c);
        }
        return;
    }

    if (n == QQ_ERROR) {
        qq_app_close_connection(c);
        return;
    }

    if (n == 0) {
        qq_log_error(0, "client closed connection");
        qq_app_close_connection(c);
        return;
    }

    c->buffer->last += n;
    qq_ios_app_process_package(c);
    c->buffer->last = c->buffer->pos;
}

static void
qq_ios_app_process_package(qq_connection_t *c)
{
}

static void
qq_ios_app_write_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_ios_app_write_event_handler()");
}

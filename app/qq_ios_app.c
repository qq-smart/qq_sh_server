/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static void qq_ios_app_wait_request_handler(qq_event_t *ev);
static void qq_ios_app_write_event_handler(qq_event_t *ev);


qq_int_t
qq_ios_app_init(qq_cycle_t *cycle)
{
    return QQ_OK;
}

void
qq_ios_app_done(void)
{
    qq_log_debug("qq_ios_app_done()");
}


void
qq_ios_app_init_connection_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_ios_app_init_connection_handler()");

    rev = c->read;
    rev->handler = qq_ios_app_wait_request_handler;

    wev = c->write;
    wev->handler = qq_ios_app_write_event_handler;

    qq_event_add_timer(rev, c->listening->post_accept_timeout);

    if (qq_handle_read_event(rev) != QQ_OK) {
        qq_log_error(0, "qq_handle_read_event() failed");
        qq_app_close_connection(c);
    }
}


static void
qq_ios_app_wait_request_handler(qq_event_t *rev)
{
    qq_connection_t   *c;
    ssize_t            n;
    size_t             size;

    u_char             buf[3];

    c = rev->data;

    qq_log_debug("qq_ios_app_wait_request_handler()");

    if (rev->timedout) {
        qq_log_error(QQ_ETIMEDOUT, "client timed out");
        qq_app_close_connection(c);
        return;
    }

    if (c->close) {
        qq_app_close_connection(c);
        return;
    }

    n = c->recv(c, buf, 3);
    if (n == QQ_AGAIN) {
        if (!rev->timer_set) {
            qq_event_add_timer(rev, c->listening->post_accept_timeout);
        }

        if (qq_handle_read_event(rev) != QQ_OK) {
            qq_app_close_connection(c);
            return;
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

    c->send(c, buf, 3);
}

static void
qq_ios_app_write_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_ios_app_write_event_handler()");
}

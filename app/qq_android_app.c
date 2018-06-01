/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static void qq_android_app_read_event_handler(qq_event_t *ev);
static void qq_android_app_write_event_handler(qq_event_t *ev);


qq_int_t
qq_android_app_init(qq_cycle_t *cycle)
{
    qq_log_debug("qq_android_app_init()");
    return QQ_OK;
}

void
qq_android_app_done(void)
{
    qq_log_debug("qq_android_app_done()");
}


void
qq_android_app_connection_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_android_app_connection_handler()");

    rev = c->read;
    rev->handler = qq_android_app_read_event_handler;

    wev = c->write;
    wev->handler = qq_android_app_write_event_handler;
}


static void
qq_android_app_read_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_android_app_read_event_handler()");
}

static void
qq_android_app_write_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_android_app_write_event_handler()");
}

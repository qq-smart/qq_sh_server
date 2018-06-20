/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_android_app.h"


static void qq_android_app_init_connection_handler(qq_connection_t *c);
static void qq_android_app_read_event_handler(qq_event_t *ev);
static void qq_android_app_write_event_handler(qq_event_t *ev);


qq_int_t
qq_android_app_init(qq_cycle_t *cycle)
{
    if (qq_add_listening_config(SOCK_STREAM,
        QQ_ANDROID_APP_TCP_LISTENING_PORT,
        QQ_ANDROID_APP_TCP_LISTENING_POOL_SIZE,
        qq_android_app_init_connection_handler) == QQ_ERROR)
    {
        qq_log_error(0, "qq_android_app_init()->qq_add_listening_config() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

void
qq_android_app_done(void)
{
    qq_log_debug("qq_android_app_done()");
}

qq_android_app_t *
qq_android_app_node(char *id)
{
    return NULL;
}

void
qq_android_app_and_device_associate(void *app_id, void *device_id)
{
}


static void
qq_android_app_init_connection_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_android_app_init_connection_handler()");

    rev = c->read;
    rev->handler = qq_android_app_read_event_handler;

    wev = c->write;
    wev->handler = qq_android_app_write_event_handler;
}


static void
qq_android_app_read_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_android_app_read_event_handler()");

    qq_connection_t   *c;
    u_char             buf[3];
    int                len;

    c = ev->data;
    len = c->recv(c, buf, 3);

    c->send(c, buf, 3);
}

static void
qq_android_app_write_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_android_app_write_event_handler()");
}

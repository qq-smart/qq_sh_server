/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_wifi_device.h"


static void qq_wifi_device_init_connection_handler(qq_connection_t *c);
static void qq_wifi_device_read_event_handler(qq_event_t *ev);
static void qq_wifi_device_write_event_handler(qq_event_t *ev);


qq_int_t
qq_wifi_device_init(qq_cycle_t *cycle)
{
    if (qq_add_listening_config(SOCK_STREAM,
        QQ_WIFI_DEVICE_TCP_LISTENING_PORT,
        QQ_WIFI_DEVICE_TCP_LISTENING_POOL_SIZE,
        qq_wifi_device_init_connection_handler) == QQ_ERROR)
    {
        qq_log_error(0, "qq_wifi_device_init()->qq_add_listening_config() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

void
qq_wifi_device_done(void)
{
    qq_log_debug("qq_wifi_device_done()");
}


static void
qq_wifi_device_init_connection_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_wifi_device_init_connection_handler()");

    rev = c->read;
    rev->handler = qq_wifi_device_read_event_handler;

    wev = c->write;
    wev->handler = qq_wifi_device_write_event_handler;
}


static void
qq_wifi_device_read_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_wifi_device_read_event_handler()");

    qq_connection_t   *c;
    u_char             buf[3];
    int                len;

    c = ev->data;
    len = c->recv(c, buf, 3);

    c->send(c, buf, 3);
}

static void
qq_wifi_device_write_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_wifi_device_write_event_handler()");
}

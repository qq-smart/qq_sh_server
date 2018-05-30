/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static qq_int_t qq_app_listening_init(qq_cycle_t *cycle);
static void qq_app_ios_app_handler(qq_connection_t *c);
static void qq_app_android_app_handler(qq_connection_t *c);
static void qq_app_wifi_device_handler(qq_connection_t *c);


qq_int_t
qq_app_init(qq_cycle_t *cycle)
{
    qq_log_debug("qq_app_init()");

    if (qq_app_listening_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_app_listening_init() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}


qq_int_t
qq_app_listening_init(qq_cycle_t *cycle)
{
    size_t                  size;
    qq_listening_config_t  *lcf;

    qq_log_debug("qq_app_listening_init()");

    cycle->nlistening = QQ_APP_LISTENING_NUMBER;
    size = sizeof(qq_listening_config_t) * cycle->nlistening;
    lcf = qq_pcalloc(cycle->pool, size);
    if (lcf == NULL) {
        qq_log_error(0, "qq_listening_config_t malloc(%uz) failed", size);
        return QQ_ERROR;
    }
    cycle->listening_config = lcf;

    /********** initialize user listening **************/
    lcf[0].type      = SOCK_STREAM;
    lcf[0].port      = QQ_IOS_APP_TCP_LISTENING_PORT;
    lcf[0].pool_size = QQ_IOS_APP_TCP_LISTENING_POOL_SIZE;
    lcf[0].handler   = qq_app_ios_app_handler;

    lcf[1].type      = SOCK_STREAM;
    lcf[1].port      = QQ_ANDROID_APP_TCP_LISTENING_PORT;
    lcf[1].pool_size = QQ_ANDROID_APP_TCP_LISTENING_POOL_SIZE;
    lcf[1].handler   = qq_app_android_app_handler;

    lcf[2].type      = SOCK_STREAM;
    lcf[2].port      = QQ_WIFI_DEVICE_TCP_LISTENING_PORT;
    lcf[2].pool_size = QQ_WIFI_DEVICE_TCP_LISTENING_POOL_SIZE;
    lcf[2].handler   = qq_app_wifi_device_handler;

    return QQ_OK;
}


static void
qq_app_ios_app_handler(qq_connection_t *c)
{
}

static void
qq_app_android_app_handler(qq_connection_t *c)
{
}

static void
qq_app_wifi_device_handler(qq_connection_t *c)
{
}

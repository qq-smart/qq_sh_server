/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_mysql.h"
#include "qq_ios_app.h"
#include "qq_android_app.h"
#include "qq_wifi_device.h"
#include "qq_video_audio_device.h"


qq_int_t
qq_app_init(qq_cycle_t *cycle)
{
    if (qq_mysql_init() == QQ_ERROR) {
        qq_log_error(0, "qq_mysql_init() failed");
        return QQ_ERROR;
    }

    if (qq_ios_app_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_ios_app_init() failed");
        return QQ_ERROR;
    }

    if (qq_android_app_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_android_app_init() failed");
        return QQ_ERROR;
    }

    if (qq_wifi_device_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_wifi_device_init() failed");
        return QQ_ERROR;
    }

    if (qq_video_audio_device_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_video_audio_device_init() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

void
qq_app_done(qq_cycle_t *cycle)
{
    qq_log_debug("qq_app_done()");

    qq_mysql_close();
    qq_ios_app_done();
    qq_android_app_done();
    qq_wifi_device_done();
    qq_video_audio_device_done();
}

void
qq_app_close_connection(qq_connection_t *c)
{
    qq_pool_t  *pool;

    qq_log_debug("qq_app_close_connection(fd:%d)", c->fd);

    c->destroyed = 1;

    pool = c->pool;

    qq_close_connection(c);

    qq_destroy_pool(pool);
}

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_cycle_t qq_cycle;


qq_int_t
qq_cycle_init(void)
{
    qq_log_debug("qq_cycle_init()");

    if (qq_app_init(&qq_cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_app_init() failed");
        return QQ_ERROR;
    }

    if (qq_open_listening_sockets(&qq_cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_open_listening_sockets() failed");
        return QQ_ERROR;
    }

    if (qq_configure_listening_sockets(&qq_cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_configure_listening_sockets() failed");
        return QQ_ERROR;
    }

    if (qq_event_init(&qq_cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_event_init() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

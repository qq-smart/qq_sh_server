/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_cycle_t   *qq_cycle;


qq_int_t
qq_cycle_init(void)
{
    qq_log_debug("qq_cycle_init()");

    qq_pool_t          *pool;
    qq_cycle_t         *cycle;

    pool = qq_create_pool(QQ_CYCLE_POOL_SIZE);
    if (pool == NULL) {
        qq_log_error(0, "qq_create_pool(%d) failed", QQ_CYCLE_POOL_SIZE);
        return QQ_ERROR;
    }

    cycle = qq_pcalloc(pool, sizeof(qq_cycle_t));
    if (cycle == NULL) {
        qq_log_error(0, "qq_pcalloc() failed");
        qq_destroy_pool(pool);
        return QQ_ERROR;
    }
    cycle->pool = pool;
    qq_cycle = cycle;

    if (qq_app_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_app_init() failed");
        return QQ_ERROR;
    }

    if (qq_create_listening(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_create_listening() failed");
        return QQ_ERROR;
    }

    if (qq_open_listening_sockets(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_open_listening_sockets() failed");
        return QQ_ERROR;
    }

    if (qq_configure_listening_sockets(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_configure_listening_sockets() failed");
        return QQ_ERROR;
    }

    cycle->connection_n = QQ_CONNECTION_NUMBER;
    if (qq_event_init(cycle) == QQ_ERROR) {
        qq_log_error(0, "qq_event_init() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

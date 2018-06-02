/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_CYCLE_H_INCLUDED_
#define _QQ_CYCLE_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_CYCLE_POOL_SIZE   QQ_DEFAULT_POOL_SIZE


struct qq_cycle_s {
    qq_pool_t               *pool;

    qq_uint_t                connection_n;
    qq_connection_t         *connections;
    qq_connection_t         *free_connections;
    qq_uint_t                free_connection_n;

    qq_event_t              *read_events;
    qq_event_t              *write_events;

    size_t                   nlistening;
    qq_listening_t          *listening;
    qq_listening_config_t   *listening_config;
};
extern qq_cycle_t  *qq_cycle;


qq_int_t qq_cycle_init(void);
void qq_cycle_done(void);


#endif /* _QQ_CYCLE_H_INCLUDED_ */

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_CYCLE_H_INCLUDED_
#define _QQ_CYCLE_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


struct qq_cycle_s {
    qq_uint_t                connection_n;
    qq_connection_t         *connections;
    qq_connection_t         *free_connections;
    qq_uint_t                free_connection_n;
    qq_event_t              *read_events;
    qq_event_t              *write_events;

    qq_listening_t          *listening;
    qq_uint_t                nlistening;
};


extern qq_cycle_t  *qq_cycle;


#endif /* _QQ_CYCLE_H_INCLUDED_ */

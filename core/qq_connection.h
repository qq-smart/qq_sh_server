/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_CONNECTION_H_INCLUDED_
#define _QQ_CONNECTION_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct {
    void              *data;
    qq_event_t        *read;
    qq_event_t        *write;

    qq_socket_t        fd;
} qq_connection_t;


#endif /* _QQ_CONNECTION_H_INCLUDED_ */

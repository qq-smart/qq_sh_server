/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_CONNECTION_H_INCLUDED_
#define _QQ_CONNECTION_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


struct qq_connection_s {
    void               *data;
    qq_event_t         *read;
    qq_event_t         *write;

    qq_socket_t         fd;

    qq_buf_t           *buffer;

    qq_recv_pt          recv;
    qq_send_pt          send;

    qq_listening_t     *listening;

    qq_pool_t          *pool;
    int                 type;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;
    char               *addr_text;
    int                 addr_text_len;

    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;

    qq_queue_t          queue;
    qq_uint_t           requests;

    unsigned            timedout:1;
    unsigned            error:1;
    unsigned            destroyed:1;

    unsigned            idle:1;
    unsigned            reusable:1;
    unsigned            close:1;
    unsigned            shared:1;
};


qq_connection_t *qq_get_connection(qq_socket_t s);
void qq_free_connection(qq_connection_t *c);
void qq_close_connection(qq_connection_t *c);


#endif /* _QQ_CONNECTION_H_INCLUDED_ */

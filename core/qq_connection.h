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

    qq_recv_pt          recv;
    qq_send_pt          send;

    qq_listening_t     *listening;

    qq_pool_t          *pool;
    int                 type;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;
    qq_str_t            addr_text;

    qq_str_t            proxy_protocol_addr;
    in_port_t           proxy_protocol_port;

#if (QQ_SSL || QQ_COMPAT)
    qq_ssl_connection_t  *ssl;
#endif

    struct sockaddr    *local_sockaddr;
    socklen_t           local_socklen;

    qq_queue_t          queue;
    qq_uint_t           requests;

    unsigned            buffered:8;

    unsigned            timedout:1;
    unsigned            error:1;
    unsigned            destroyed:1;

    unsigned            idle:1;
    unsigned            reusable:1;
    unsigned            close:1;
    unsigned            shared:1;

    unsigned            sendfile:1;
    unsigned            sndlowat:1;
    unsigned            tcp_nodelay:2;   /* qq_connection_tcp_nodelay_e */
    unsigned            tcp_nopush:2;    /* qq_connection_tcp_nopush_e */

    unsigned            need_last_buf:1;

#if (QQ_HAVE_AIO_SENDFILE || QQ_COMPAT)
    unsigned            busy_count:2;
#endif
};


qq_connection_t *qq_get_connection(qq_socket_t s);
void qq_free_connection(qq_connection_t *c);
void qq_close_connection(qq_connection_t *c);


#endif /* _QQ_CONNECTION_H_INCLUDED_ */

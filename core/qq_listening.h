/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_LISTENING_H_INCLUDED_
#define _QQ_LISTENING_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct {
    int    type;
    int    port;
    size_t pool_size;
    qq_connection_handler_pt handler;
} qq_listening_config_t;


typedef struct qq_listening_s  qq_listening_t;

struct qq_listening_s {
    qq_socket_t         fd;

    struct sockaddr_in  sockaddr;
    socklen_t           socklen;    /* size of sockaddr */
    char                addr_text[QQ_SOCKADDR_STRLEN];
    int                 addr_text_len;

    int                 type;

    int                 backlog;
    int                 rcvbuf;
    int                 sndbuf;

    qq_connection_handler_pt   handler;

    size_t              pool_size;

    qq_connection_t    *connection;

    qq_msec_t           post_accept_timeout;
};


qq_int_t qq_create_listening(qq_cycle_t *cycle);
qq_int_t qq_open_listening_sockets(qq_cycle_t *cycle);
qq_int_t qq_configure_listening_sockets(qq_cycle_t *cycle);
void qq_close_listening_sockets(qq_cycle_t *cycle);


#endif /* _QQ_LISTENING_H_INCLUDED_ */

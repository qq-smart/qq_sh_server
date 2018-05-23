/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_LISTENING_H_INCLUDED_
#define _QQ_LISTENING_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct qq_listening_s  qq_listening_t;

struct qq_listening_s {
    qq_socket_t         fd;

    struct sockaddr    *sockaddr;
    socklen_t           socklen;    /* size of sockaddr */
    size_t              addr_text_max_len;
    qq_str_t            addr_text;

    int                 type;

    int                 backlog;
    int                 rcvbuf;
    int                 sndbuf;
#if (QQ_HAVE_KEEPALIVE_TUNABLE)
    int                 keepidle;
    int                 keepintvl;
    int                 keepcnt;
#endif

    /* handler of accepted connection */
    qq_connection_handler_pt   handler;

    size_t              pool_size;

    /* should be here because of the AcceptEx() preread */
    size_t              post_accept_buffer_size;
    /* should be here because of the deferred accept */
    qq_msec_t           post_accept_timeout;

    qq_listening_t     *next;
    qq_connection_t    *connection;

    qq_uint_t           worker;

    unsigned            open:1;
    unsigned            remain:1;
    unsigned            ignore:1;

    unsigned            bound:1;       /* already bound */
    unsigned            inherited:1;   /* inherited from previous process */
    unsigned            nonblocking_accept:1;
    unsigned            listen:1;
    unsigned            nonblocking:1;
    unsigned            shared:1;    /* shared between threads or processes */
    unsigned            addr_ntop:1;
    unsigned            wildcard:1;

#if (QQ_HAVE_INET6)
    unsigned            ipv6only:1;
#endif
    unsigned            reuseport:1;
    unsigned            add_reuseport:1;
    unsigned            keepalive:2;

    unsigned            deferred_accept:1;
    unsigned            delete_deferred:1;
    unsigned            add_deferred:1;
#if (QQ_HAVE_DEFERRED_ACCEPT && defined SO_ACCEPTFILTER)
    char               *accept_filter;
#endif
#if (QQ_HAVE_SETFIB)
    int                 setfib;
#endif

#if (QQ_HAVE_TCP_FASTOPEN)
    int                 fastopen;
#endif
};


qq_int_t qq_open_listening_sockets(qq_cycle_t *cycle);
void qq_configure_listening_sockets(qq_cycle_t *cycle);
void qq_close_listening_sockets(qq_cycle_t *cycle);


#endif /* _QQ_LISTENING_H_INCLUDED_ */

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_EVENT_H_INCLUDED_
#define _QQ_EVENT_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef void (*qq_event_handler_pt)(void *ev);


typedef struct {
    void            *data;

    unsigned         write:1;
    unsigned         accept:1;
    unsigned         instance:1;
    unsigned         active:1;
    unsigned         disabled:1;
    unsigned         ready:1;

    unsigned         oneshot:1;
    unsigned         complete:1;
    unsigned         eof:1;
    unsigned         error:1;

    unsigned         timedout:1;
    unsigned         timer_set:1;

    unsigned         delayed:1;

    unsigned         deferred_accept:1;
    unsigned         pending_eof:1;

    unsigned         posted:1;

    unsigned         closed:1;

    unsigned         channel:1;
    unsigned         resolver:1;

    unsigned         cancelable:1;

    qq_event_handler_pt  handler;

    qq_rbtree_node_t   timer;
    qq_queue_t         queue;
} qq_event_t;


#define QQ_READ_EVENT     0
#define QQ_WRITE_EVENT    1
#define QQ_CLOSE_EVENT    2
#define QQ_UPDATE_TIME    4

#endif /* _QQ_EVENT_H_INCLUDED_ */

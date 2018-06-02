/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_EVENT_H_INCLUDED_
#define _QQ_EVENT_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_INVALID_INDEX  0xd0d0d0d0


struct qq_event_s {
    void            *data;

    unsigned         write:1;

    unsigned         accept:1;

    /* used to detect the stale events in kqueue and epoll */
    unsigned         instance:1;

    /*
     * the event was passed or would be passed to a kernel;
     * in aio mode - operation was posted.
     */
    unsigned         active:1;

    unsigned         disabled:1;

    /* the ready event; in aio mode 0 means that no operation can be posted */
    unsigned         ready:1;

    unsigned         oneshot:1;

    /* aio operation is complete */
    unsigned         complete:1;

    unsigned         eof:1;
    unsigned         error:1;

    unsigned         timedout:1;
    unsigned         timer_set:1;

    unsigned         delayed:1;

    unsigned         deferred_accept:1;

    /* the pending eof reported by kqueue, epoll or in aio chain operation */
    unsigned         pending_eof:1;

    unsigned         posted:1;

    unsigned         closed:1;

    /* to test on worker exit */
    unsigned         channel:1;
    unsigned         resolver:1;

    unsigned         cancelable:1;

    unsigned         available:1;

    qq_event_handler_pt  handler;

    qq_uint_t       index;

    qq_rbtree_node_t   timer;

    /* the posted queue */
    qq_queue_t      queue;
};


#define QQ_READ_EVENT     1
#define QQ_WRITE_EVENT    2

#define QQ_CLOSE_EVENT    1
#define QQ_DISABLE_EVENT  2

#define QQ_UPDATE_TIME    1


qq_int_t qq_event_init(qq_cycle_t *cycle);
void qq_events_process(void);
void qq_event_done(qq_cycle_t *cycle);


#endif /* _QQ_EVENT_H_INCLUDED_ */

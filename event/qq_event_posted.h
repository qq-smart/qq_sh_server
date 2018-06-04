/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_EVENT_POSTED_H_INCLUDED_
#define _QQ_EVENT_POSTED_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define qq_post_event(ev, q)                                                  \
    if (!(ev)->posted) {                                                      \
        (ev)->posted = 1;                                                     \
        qq_queue_insert_tail(q, &(ev)->queue);                                \
        qq_log_debug("qq_post_event(ev:%p)", ev);                             \
    } else  {                                                                 \
        qq_log_debug("update posted event %p", ev);                           \
    }


#define qq_delete_posted_event(ev)                                            \
    (ev)->posted = 0;                                                         \
    qq_queue_remove(&(ev)->queue);                                            \
    qq_log_debug("qq_delete_posted_event(ev:%p)", ev);



void qq_event_process_posted(qq_queue_t *posted);


extern qq_queue_t  qq_posted_accept_events;
extern qq_queue_t  qq_posted_events;


#endif /* _QQ_EVENT_POSTED_H_INCLUDED_ */

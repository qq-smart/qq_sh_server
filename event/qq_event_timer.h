/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_EVENT_TIMER_H_INCLUDED_
#define _QQ_EVENT_TIMER_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_TIMER_INFINITE    (qq_msec_t) - 1
#define QQ_TIMER_LAZY_DELAY  300


qq_int_t qq_event_timer_init(void);
qq_msec_t qq_event_find_timer(void);
void qq_event_expire_timers(void);


extern qq_rbtree_t  qq_event_timer_rbtree;


static inline void
qq_event_del_timer(qq_event_t *ev)
{
    qq_rbtree_delete(&qq_event_timer_rbtree, &ev->timer);
    ev->timer_set = 0;
}


static inline void
qq_event_add_timer(qq_event_t *ev, qq_msec_t timer)
{
    qq_msec_t      key;
    qq_msec_int_t  diff;

    key = qq_current_msec + timer;

    if (ev->timer_set) {
        diff = (qq_msec_int_t) (key - ev->timer.key);
        if (qq_abs(diff) < QQ_TIMER_LAZY_DELAY) {
            return;
        }

        qq_event_del_timer(ev);
    }

    ev->timer.key = key;
    qq_rbtree_insert(&qq_event_timer_rbtree, &ev->timer);
    ev->timer_set = 1;
}


#endif /* _QQ_EVENT_TIMER_H_INCLUDED_ */

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_rbtree_t              qq_event_timer_rbtree;
static qq_rbtree_node_t  qq_event_timer_sentinel;


qq_int_t
qq_event_timer_init(void)
{
    qq_rbtree_init(&qq_event_timer_rbtree, &qq_event_timer_sentinel,
                    qq_rbtree_insert_timer_value);

    return QQ_OK;
}


qq_msec_t
qq_event_find_timer(void)
{
    qq_msec_int_t      timer;
    qq_rbtree_node_t  *node, *root, *sentinel;

    if (qq_event_timer_rbtree.root == &qq_event_timer_sentinel) {
        return QQ_TIMER_INFINITE;
    }

    root     = qq_event_timer_rbtree.root;
    sentinel = qq_event_timer_rbtree.sentinel;

    node = qq_rbtree_min(root, sentinel);
    timer = (qq_msec_int_t) (node->key - qq_current_msec);

    return (qq_msec_t) (timer > 0 ? timer : 0);
}


void
qq_event_expire_timers(void)
{
    qq_event_t        *ev;
    qq_rbtree_node_t  *node, *root, *sentinel;

    sentinel = qq_event_timer_rbtree.sentinel;

    for ( ;; ) {
        root = qq_event_timer_rbtree.root;

        if (root == sentinel) {
            return;
        }

        node = qq_rbtree_min(root, sentinel);

        /* node->key > qq_current_msec */

        if ((qq_msec_int_t) (node->key - qq_current_msec) > 0) {
            return;
        }

        ev = (qq_event_t *) ((char *) node - offsetof(qq_event_t, timer));
        qq_rbtree_delete(&qq_event_timer_rbtree, &ev->timer);

        ev->timer_set = 0;
        ev->timedout = 1;
        ev->handler(ev);
    }
}


qq_int_t
qq_event_no_timers_left(void)
{
    qq_event_t        *ev;
    qq_rbtree_node_t  *node, *root, *sentinel;

    sentinel = qq_event_timer_rbtree.sentinel;
    root     = qq_event_timer_rbtree.root;

    if (root == sentinel) {
        return QQ_OK;
    }

    for (node = qq_rbtree_min(root, sentinel);
         node;
         node = qq_rbtree_next(&qq_event_timer_rbtree, node))
    {
        ev = (qq_event_t *) ((char *) node - offsetof(qq_event_t, timer));

        if (!ev->cancelable) {
            return QQ_AGAIN;
        }
    }

    /* only cancelable timers left */

    return QQ_OK;
}

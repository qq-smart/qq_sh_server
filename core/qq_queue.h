/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


#ifndef _QQ_QUEUE_H_INCLUDED_
#define _QQ_QUEUE_H_INCLUDED_


typedef struct qq_queue_s  qq_queue_t;

struct qq_queue_s {
    qq_queue_t  *prev;
    qq_queue_t  *next;
};


#define qq_queue_init(q)                                                      \
    (q)->prev = q;                                                            \
    (q)->next = q

#define qq_queue_empty(h)                                                     \
    (h == (h)->prev)

#define qq_queue_insert_head(h, x)                                            \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x

#define qq_queue_insert_after   qq_queue_insert_head

#define qq_queue_insert_tail(h, x)                                            \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x

#define qq_queue_head(h)                                                      \
    (h)->next

#define qq_queue_last(h)                                                      \
    (h)->prev

#define qq_queue_sentinel(h)                                                  \
    (h)

#define qq_queue_next(q)                                                      \
    (q)->next

#define qq_queue_prev(q)                                                      \
    (q)->prev

#define qq_queue_remove(x)                                                    \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#define qq_queue_split(h, q, n)                                               \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;

#define qq_queue_add(h, n)                                                    \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;

#define qq_queue_data(q, type, link)                                          \
    (type *) ((u_char *) q - offsetof(type, link))


qq_queue_t *qq_queue_middle(qq_queue_t *queue);

void qq_queue_sort(qq_queue_t *queue,
    qq_int_t (*cmp)(const qq_queue_t *, const qq_queue_t *));


#endif /* _QQ_QUEUE_H_INCLUDED_ */

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_queue_t *
qq_queue_middle(qq_queue_t *queue)
{
    qq_queue_t  *middle, *next;

    middle = qq_queue_head(queue);

    if (middle == qq_queue_last(queue)) {
        return middle;
    }

    next = qq_queue_head(queue);

    for ( ;; ) {
        middle = qq_queue_next(middle);

        next = qq_queue_next(next);

        if (next == qq_queue_last(queue)) {
            return middle;
        }

        next = qq_queue_next(next);

        if (next == qq_queue_last(queue)) {
            return middle;
        }
    }
}

void
qq_queue_sort(qq_queue_t *queue,
    qq_int_t (*cmp)(const qq_queue_t *, const qq_queue_t *))
{
    qq_queue_t  *q, *prev, *next;

    q = qq_queue_head(queue);
    if (q == qq_queue_last(queue)) {
        return;
    }

    for (q = qq_queue_next(q); q != qq_queue_sentinel(queue); q = next) {
        prev = qq_queue_prev(q);
        next = qq_queue_next(q);

        qq_queue_remove(q);

        do {
            if (cmp(prev, q) <= 0) {
                break;
            }

            prev = qq_queue_prev(prev);

        } while (prev != qq_queue_sentinel(queue));

        qq_queue_insert_after(prev, q);
    }
}


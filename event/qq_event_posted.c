/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_queue_t  qq_posted_accept_events;
qq_queue_t  qq_posted_events;


void
qq_event_process_posted(qq_queue_t *posted)
{
    qq_queue_t  *q;
    qq_event_t  *ev;

    while (!qq_queue_empty(posted)) {
        q = qq_queue_head(posted);
        ev = qq_queue_data(q, qq_event_t, queue);

        qq_log_debug("qq_event_process_posted(ev:%p)", ev);

        qq_delete_posted_event(ev);

        ev->handler(ev);
    }
}

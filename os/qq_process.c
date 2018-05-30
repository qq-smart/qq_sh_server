/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static void qq_worker_process_init(qq_cycle_t *cycle);
static void qq_worker_process_exit(qq_cycle_t *cycle);


void
qq_process_cycle(qq_cycle_t *cycle)
{
    qq_worker_process_init(cycle);

    for (;;) {
        qq_events_process();
    }

    qq_worker_process_exit(cycle);
}


static void
qq_worker_process_init(qq_cycle_t *cycle)
{
    qq_log_debug("qq_worker_process_init()");
}

static void
qq_worker_process_exit(qq_cycle_t *cycle)
{
    qq_log_debug("qq_worker_process_exit()");
}

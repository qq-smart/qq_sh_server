/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"
#include "qq_main.h"


int
main(int argc, char *const *argv)
{
    qq_time_init();

    qq_log_init(getpid());

    if (qq_cycle_init() == QQ_ERROR) {
        qq_log_error(0, "qq_cycle_init() failed");
        return 1;
    }

    qq_process_cycle(qq_cycle);
 
    return 0;
}

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
    printf("qq_sh_server running!\n");

    qq_time_init();
    qq_log_init(getpid());

    errno = 2345;
    qq_log_error("qq_log_error: hello %s %d\n", "111", 567);
    qq_log_debug("qq_log_debug: hello %s %d\n", "1110", 456);
    errno = 89009;
    qq_log_error("qq_log_error: hello %s %d\n", "1a", 666);
    qq_log_debug("qq_log_debug: hello %s %d\n", "11dd10", 756);
    errno = 987577;
    qq_log_error("qq_log_error: hello %s %d\n", "11fdfd1", 557);
    qq_log_debug("qq_log_debug: hello %s %d\n", "1110fdfd", 6);
    return 0;
}

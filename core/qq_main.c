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
    int i=100;
    char str[] = "yyyy";

    printf("qq_sh_server running!\n");

    qq_log_error(890, "qq_log_error: hello %s %d\n", str, i);
    qq_log_debug(900, "qq_log_debug: hello %s %d\n", str, i);

    return 0;
}

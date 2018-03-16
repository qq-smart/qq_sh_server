/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


volatile qq_str_t  qq_log_time;


void
qq_time_init(void)
{
    qq_str_text(&qq_log_time, "1234");
}

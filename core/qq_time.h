/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_TIME_H_INCLUDED_
#define _QQ_TIME_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


void qq_time_init(void);
void qq_time_update(void);
void qq_localtime(time_t t, struct tm *tp);


extern volatile qq_str_t    qq_cached_err_log_time;
extern volatile qq_msec_t   qq_current_msec;


#endif /* _QQ_TIME_H_INCLUDED_ */

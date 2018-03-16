/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_LOG_H_INCLUDED_
#define _QQ_LOG_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


void qq_log_error(qq_pid_t pid, const char *fmt, ...);

#ifdef QQ_DEBUG
void qq_log_debug(qq_pid_t pid, const char *fmt, ...);
#else
#define qq_log_debug(pid, fmt, ...)
#endif


#endif /* _QQ_LOG_H_INCLUDED_ */

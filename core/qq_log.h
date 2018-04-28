/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_LOG_H_INCLUDED_
#define _QQ_LOG_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define qq_log_time_value_len()   strlen("1979/01/01_00:00:00")
#define qq_log_pid_value_len()    strlen("   4294967295   ")
#define qq_log_errno_value_len()  strlen("4294967295   ")

#define qq_log_err_info_prefix_len() \
    qq_log_time_value_len() + qq_log_pid_value_len() + qq_log_errno_value_len()

#define qq_log_debug_info_prefix_len() \
    qq_log_time_value_len() + qq_log_pid_value_len()


#define QQ_ERR_LOG_FILE_NAME "log"


typedef struct {
    u_char    str[QQ_MAX_LOG_STR];

    u_char   *time_ptr;
    u_char   *pid_ptr;
    u_char   *errno_ptr;
    u_char   *err_desc_ptr;
    u_char   *debug_desc_ptr;

    int       err_prefix_len;
    int       debug_prefix_len;

    qq_fd_t   err_fd;
} qq_log_t;


void qq_log_init(qq_pid_t pid);

void qq_log_error(const char *fmt, ...);

#if (QQ_HAVE_DEBUG)

void qq_log_debug(const char *fmt, ...);

#else

#define qq_log_debug(fmt, ...)

#endif


#endif /* _QQ_LOG_H_INCLUDED_ */

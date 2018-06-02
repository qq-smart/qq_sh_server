/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_LOG_H_INCLUDED_
#define _QQ_LOG_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_MAX_LOG_STR  256

#define qq_log_time_value_len()   strlen("1979/01/01_00:00:00")
#define qq_log_pid_value_len()    strlen("   4294967295   ")
#define qq_log_errno_value_len()  strlen("4294967295   ")

#define qq_log_err_info_prefix_len() \
    qq_log_time_value_len() + qq_log_pid_value_len() + qq_log_errno_value_len()

#define qq_log_debug_info_prefix_len() \
    qq_log_time_value_len() + qq_log_pid_value_len()


#define QQ_ERR_LOG_FILE_NAME "log"

#define QQ_LOG_DEBUG     0
#define QQ_LOG_ERROR     1


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
void qq_log_error(qq_err_t err, const char *fmt, ...);
#if (QQ_HAVE_DEBUG)
    void qq_log_debug(const char *fmt, ...);
#else
    #define qq_log_debug(fmt, ...)
#endif


void qq_log_stderr(const char *fmt, ...);

static inline void
qq_write_stderr(char *text)
{
    (void) write(STDERR_FILENO, text, strlen(text));
}

static inline void
qq_write_stdout(char *text)
{
    (void) write(STDOUT_FILENO, text, strlen(text));
}


#define QQ_LINEFEED              "\x0a"
#define qq_linefeed(p)          *p++ = LF;
#define QQ_LINEFEED_SIZE         1


#endif /* _QQ_LOG_H_INCLUDED_ */

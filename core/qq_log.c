/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static qq_log_t qq_log;


static void qq_log_error_debug_core(int type, qq_err_t err, const char *fmt, va_list args);


void
qq_log_init(qq_pid_t pid)
{
    char *p, *dir;
    char name[100];

    memset(qq_log.str, 0, QQ_MAX_LOG_STR);

    p = qq_log.str;
    qq_log.time_ptr = p;
    p += qq_log_time_value_len();

    qq_log.pid_ptr = p;
    sprintf(qq_log.pid_ptr, "   %10d   ", pid);
    p += qq_log_pid_value_len();

    qq_log.debug_desc_ptr = p;

    qq_log.errno_ptr = p;
    p += qq_log_errno_value_len();

    qq_log.err_desc_ptr = p;

    qq_log.err_prefix_len = qq_log_err_info_prefix_len();
    qq_log.debug_prefix_len = qq_log_debug_info_prefix_len();

    dir = (char *)get_current_dir_name();
    p = memcpy(name, dir, strlen(dir)) + strlen(dir);
    *p++ = '/';
    memcpy(p, QQ_ERR_LOG_FILE_NAME, strlen(QQ_ERR_LOG_FILE_NAME));

    qq_log.err_fd = qq_open_file(name, QQ_FILE_APPEND,
                                 QQ_FILE_CREATE_OR_OPEN,
                                 QQ_FILE_DEFAULT_ACCESS);
    if (qq_log.err_fd == -1) {
        qq_log.err_fd = STDERR_FILENO;
    }
}

void
qq_log_error(qq_err_t err, const char *fmt, ...)
{
    va_list  args;

    va_start(args, fmt);
    qq_log_error_debug_core(QQ_LOG_ERROR, err, fmt, args);
    va_end(args);
}

#if (QQ_HAVE_DEBUG)

void
qq_log_debug(const char *fmt, ...)
{
    va_list  args;

    va_start(args, fmt);
    qq_log_error_debug_core(QQ_LOG_DEBUG, 0, fmt, args);
    va_end(args);
}

#endif

static void
qq_log_error_debug_core(int type, qq_err_t err, const char *fmt, va_list args)
{
    int  len;

    memcpy(qq_log.time_ptr, qq_cached_err_log_time.data, qq_cached_err_log_time.len);

    if (type == QQ_LOG_ERROR) {
        sprintf(qq_log.errno_ptr, "%10d   ", err);
        len = vsprintf(qq_log.err_desc_ptr, fmt, args);
        write(qq_log.err_fd, qq_log.str, qq_log.err_prefix_len + len);
    }
    else if (type == QQ_LOG_DEBUG){
        len = vsprintf(qq_log.debug_desc_ptr, fmt, args);
        write(STDERR_FILENO, qq_log.str, qq_log.debug_prefix_len + len);
    }
}

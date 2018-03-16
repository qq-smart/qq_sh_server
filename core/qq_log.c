/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


void
qq_log_error(qq_pid_t pid, const char *fmt, ...)
{
    u_char  str[QQ_MAX_LOG_STR] = { 0 };
    u_char  *p;
    int     len;
    va_list arg;

    p = memcpy(str, qq_log_time.data, qq_log_time.len) + qq_log_time.len;

    len = sprintf(p, " %d ", pid);
    p += (len == -1) ? 0 : len;

    va_start(arg, fmt);
    len = vsprintf(p, fmt, arg);
    p += (len == -1) ? 0 : len;
    write(STDERR_FILENO, str, p - str);
    va_end(arg);
}

#ifdef QQ_DEBUG
void
qq_log_debug(qq_pid_t pid, const char *fmt, ...)
{
    u_char  str[QQ_MAX_LOG_STR] = { 0 };
    u_char  *p;
    int     len;
    va_list arg;

    p = str;
    len = sprintf(p, " %d ", pid);
    p += (len == -1) ? 0 : len;

    va_start(arg, fmt);
    len = vsprintf(p, fmt, arg);
    p += (len == -1) ? 0 : len;
    write(STDERR_FILENO, str, p - str);
    va_end(arg);
}
#endif

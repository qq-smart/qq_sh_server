/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


volatile qq_msec_t      qq_current_msec;

volatile qq_str_t       qq_cached_err_log_time;
static   u_char         cached_err_log_time[sizeof("1970/01/01_00:00:00")];


void
qq_time_init(void)
{
    qq_cached_err_log_time.len  = strlen("1970/01/01_00:00:00");

    qq_time_update();
}

void
qq_time_update(void)
{
    u_char          *p;
    struct tm        t;
    time_t           sec;
    qq_uint_t        msec;
    struct timeval   tv;

    gettimeofday(&tv, NULL);

    sec = tv.tv_sec;
    msec = tv.tv_usec / 1000;

    qq_current_msec = (qq_msec_t) sec * 1000 + msec;

    qq_localtime(sec, &t);

    p = cached_err_log_time;
    (void) sprintf(p, "%4d/%02d/%02d_%02d:%02d:%02d",
                   t.tm_year, t.tm_mon,
                   t.tm_mday, t.tm_hour,
                   t.tm_min,  t.tm_sec);

    qq_cached_err_log_time.data = p;
}

void
qq_localtime(time_t s, struct tm *tp)
{
    struct tm  *t;

    t = localtime(&s);
    *tp = *t;

    tp->tm_mon++;
    tp->tm_year += 1900;
}

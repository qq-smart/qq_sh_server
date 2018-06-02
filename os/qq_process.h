/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_PROCESS_H_INCLUDED_
#define _QQ_PROCESS_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct {
    int     signo;
    char   *signame;
    char   *name;
    void  (*handler)(int signo);
} qq_signal_t;


qq_int_t qq_signals_init(void);

void qq_process_cycle(qq_cycle_t *cycle);


#endif /* _QQ_PROCESS_H_INCLUDED_ */

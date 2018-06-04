/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_IOS_APP_H_INCLUDED_
#define _QQ_IOS_APP_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


qq_int_t qq_ios_app_init(qq_cycle_t *cycle);
void qq_ios_app_done(void);

void qq_ios_app_init_connection_handler(qq_connection_t *c);


#endif /* _QQ_IOS_APP_H_INCLUDED_ */

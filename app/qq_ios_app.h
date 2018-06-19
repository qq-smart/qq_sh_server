/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_IOS_APP_H_INCLUDED_
#define _QQ_IOS_APP_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_IOS_APP_TCP_LISTENING_PORT           10000
#define QQ_IOS_APP_TCP_LISTENING_POOL_SIZE      1024

#define QQ_IOS_APP_ACCEPT_TIMEOUT               3000


qq_int_t qq_ios_app_init(qq_cycle_t *cycle);
void qq_ios_app_done(void);


#endif /* _QQ_IOS_APP_H_INCLUDED_ */

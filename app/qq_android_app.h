/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_ANDROID_APP_H_INCLUDED_
#define _QQ_ANDROID_APP_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_ANDROID_APP_TCP_LISTENING_PORT       11000
#define QQ_ANDROID_APP_TCP_LISTENING_POOL_SIZE  1024


typedef struct {
    qq_connection_t *c;
} qq_android_app_t;


qq_int_t qq_android_app_init(qq_cycle_t *cycle);
void qq_android_app_done(void);

qq_android_app_t *qq_android_app_node(char *id);
void qq_android_app_and_device_associate(void *app_id, void *device_id);

#endif /* _QQ_ANDROID_APP_H_INCLUDED_ */

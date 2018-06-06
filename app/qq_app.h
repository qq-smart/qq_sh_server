/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_APP_H_INCLUDED_
#define _QQ_APP_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_APP_LISTENING_NUMBER             4

#define QQ_IOS_APP_TCP_LISTENING_PORT           10000
#define QQ_IOS_APP_TCP_LISTENING_POOL_SIZE      1024

#define QQ_ANDROID_APP_TCP_LISTENING_PORT       11000
#define QQ_ANDROID_APP_TCP_LISTENING_POOL_SIZE  1024

#define QQ_WIFI_DEVICE_TCP_LISTENING_PORT       12000
#define QQ_WIFI_DEVICE_TCP_LISTENING_POOL_SIZE  512

#define QQ_VIDEO_AUDIO_DEVICE_UDP_LISTENING_PORT       13000
#define QQ_VIDEO_AUDIO_DEVICE_UDP_LISTENING_POOL_SIZE  512


qq_int_t qq_app_init(qq_cycle_t *cycle);
void qq_app_done(qq_cycle_t *cycle);

void qq_app_close_connection(qq_connection_t *c);


#endif /* _QQ_APP_H_INCLUDED_ */

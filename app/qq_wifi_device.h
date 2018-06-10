/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_WIFI_DEVICE_H_INCLUDED_
#define _QQ_WIFI_DEVICE_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_WIFI_DEVICE_TCP_LISTENING_PORT       12000
#define QQ_WIFI_DEVICE_TCP_LISTENING_POOL_SIZE  512


qq_int_t qq_wifi_device_init(qq_cycle_t *cycle);
void qq_wifi_device_done(void);


#endif /* _QQ_WIFI_DEVICE_H_INCLUDED_ */

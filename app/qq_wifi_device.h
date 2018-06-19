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

#define QQ_WIFI_DEVICE_CONNECTION_TIMEOUT       3000
#define QQ_WIFI_DEVICE_FIRST_STATUS_TIMEOUT     3000
#define QQ_WIFI_DEVICE_PING_TIMEOUT             60000

#define QQ_WIFI_DEVICE_PKG_FAILED             1
#define QQ_WIFI_DEVICE_MD5_FAILED             2
#define QQ_WIFI_DEVICE_NOTHINGNESS            3
#define QQ_WIFI_DEVICE_CONFLICT               4
#define QQ_WIFI_DEVICE_CONNECTION_CLOSE       5
#define QQ_WIFI_DEVICE_CONNECTION_ERROR       6
#define QQ_WIFI_DEVICE_DISCONNECTION          7

#define QQ_WIFI_DEVICE_MD5_STR      "yfdali32439f0385mvkie"


#define qq_wifi_device_rbtree_key(id)  *(uint32_t *)id

typedef struct {
    qq_rbtree_node_t   rbtree;
    u_char             id[16];
} qq_wifi_device_rbtree_node_t;


typedef struct {
    u_char *buf_start;
    u_char *buf_end;
    size_t  buf_size;

    u_char *status_start;
    u_char *status_pos;
    u_char *status_end;
    size_t  status_size;

    u_char *tmp_start;
    u_char *tmp_pos;
    u_char *tmp_last;
    u_char *tmp_end;
    size_t  tmp_size;

    qq_wifi_device_rbtree_node_t  rbtree;
    u_char     mac[6];

    unsigned   connection:1;
    unsigned   status:1;
} qq_wifi_device_info_t;


extern qq_rbtree_t  qq_event_timer_rbtree;


qq_int_t qq_wifi_device_init(qq_cycle_t *cycle);
void qq_wifi_device_done(void);


#endif /* _QQ_WIFI_DEVICE_H_INCLUDED_ */

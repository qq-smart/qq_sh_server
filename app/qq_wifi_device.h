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
#define QQ_WIFI_DEVICE_NO_APP                 4
#define QQ_WIFI_DEVICE_APP_OVERRANGING        5
#define QQ_WIFI_DEVICE_MAIN_APP_ERROR         6
#define QQ_WIFI_DEVICE_CONFLICT               7
#define QQ_WIFI_DEVICE_INSUFFICIENT_MEMORY    8
#define QQ_WIFI_DEVICE_CONNECTION_CLOSE       9
#define QQ_WIFI_DEVICE_CONNECTION_ERROR       10
#define QQ_WIFI_DEVICE_DISCONNECTION          11

#define QQ_WIFI_DEVICE_MD5_CORE_STR         "yfdali32439f0385mvkie"


#define QQ_WIFI_DEVICE_MAX_APP_NUMBER  10

typedef struct {
    void  *app;
    int    app_type;
} qq_wifi_device_app_t;


#define qq_wifi_device_rbtree_key(id)  *(uint32_t *)id

typedef struct {
    qq_rbtree_node_t   rbtree;
    u_char             id[16];
} qq_wifi_device_rbtree_node_t;


typedef struct {
    qq_connection_t *c;

    u_char *buf;
    size_t  buf_size;
    size_t  buf_ndata;

    u_char *status;
    size_t  status_size;

    qq_wifi_device_rbtree_node_t  rbtree;
    u_char                        mac[6];

    qq_wifi_device_app_t  app[QQ_WIFI_DEVICE_MAX_APP_NUMBER];
    qq_int_t              napp;

    unsigned   connection:1;
    unsigned   first_status:1;
} qq_wifi_device_t;


extern qq_rbtree_t  qq_event_timer_rbtree;


qq_int_t qq_wifi_device_init(qq_cycle_t *cycle);
void qq_wifi_device_done(void);

qq_wifi_device_t *qq_wifi_device_node(char *id);
void qq_wifi_device_and_app_associate(void *device_id, void *app_id);


#endif /* _QQ_WIFI_DEVICE_H_INCLUDED_ */

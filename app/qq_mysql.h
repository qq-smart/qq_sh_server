/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_APP_MYSQL_H_INCLUDED_
#define _QQ_APP_MYSQL_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"

#include "/usr/include/mysql/mysql.h"


#define QQ_MYSQL_HOST_NAME      "localhost"
#define QQ_MYSQL_USER_NAME      "root"
#define QQ_MYSQL_USER_PWD       "wei0206."
#define QQ_MYSQL_DB_NAME        "qq_sh_server"
#define QQ_MYSQL_DB_PORT        3306

#define QQ_MYSQL_TABLE_WIFI_DEVICE wifi_device


qq_int_t qq_mysql_init(void);
void qq_mysql_close(void);


MYSQL_RES *qq_mysql_wifi_device_select(char *device_id);
qq_int_t qq_mysql_wifi_device_insert_into(char *device_id);
qq_int_t qq_mysql_wifi_device_update(char *device_id, char *app_id, int app_type);
qq_int_t qq_mysql_wifi_device_delete(char *device_id);

MYSQL_RES *qq_mysql_android_app_select(char *app_id);
qq_int_t qq_mysql_android_app_insert_into(char *app_id);
qq_int_t qq_mysql_android_app_update(char *app_id, char *device_id, int device_type);
qq_int_t qq_mysql_android_app_delete(char *app_id);

MYSQL_RES *qq_mysql_ios_app_select(char *app_id);
qq_int_t qq_mysql_ios_app_insert_into(char *app_id);
qq_int_t qq_mysql_ios_app_update(char *app_id, char *device_id, int device_type);
qq_int_t qq_mysql_ios_app_delete(char *app_id);


#endif /* _QQ_APP_MYSQL_H_INCLUDED_ */

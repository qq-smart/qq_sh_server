/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_mysql.h"


static MYSQL       *qq_mysql;
static MYSQL_RES   *qq_res;
static MYSQL_ROW    qq_row;
static char         qq_str[256];


qq_int_t qq_mysql_init(void)
{
    qq_mysql = mysql_init(NULL);
    if (qq_mysql == NULL) {
        qq_log_error(0, "mysql_init() failed");
        return QQ_ERROR;
    }

    if (!mysql_real_connect(qq_mysql,
        QQ_MYSQL_HOST_NAME,
        QQ_MYSQL_USER_NAME,
        QQ_MYSQL_USER_PWD,
        QQ_MYSQL_DB_NAME,
        QQ_MYSQL_DB_PORT,
        NULL,
        0))
    {
        qq_log_error(0, "mysql_real_connect() failed");
        return QQ_ERROR;
    }

    if (mysql_query(qq_mysql, "set names utf8")) {
        qq_log_error(0, "mysql_query(set names utf8) failed");
        return QQ_ERROR;
    }
    

    return QQ_OK;
}

void qq_mysql_close(void)
{
    mysql_close(qq_mysql);
}


MYSQL_RES *
qq_mysql_wifi_device_select(char *device_id)
{
    memset(qq_str, 0, 256);
    sprintf(qq_str, "SELECT app_id,app_type FROM wifi_device WHERE device_id='%s'", device_id);

    if (mysql_query(qq_mysql, qq_str)) {
        qq_log_error(0, "qq_mysql_wifi_device_select(%s) query failed", device_id);
        return NULL;
    }

    qq_res = mysql_store_result(qq_mysql);
    if (qq_res == NULL) {
        qq_log_error(0, "qq_mysql_wifi_device_select(%s) result failed", device_id);
        return NULL;
    }

    return qq_res;
}

qq_int_t
qq_mysql_wifi_device_insert_into(char *device_id)
{
    memset(qq_str, 0, 256);
    sprintf(qq_str, "INSERT INTO wifi_device(device_id, app_id, app_type) VALUES ('%s', '%s', %d)",
        device_id, "\0", 0);

    if (mysql_query(qq_mysql, qq_str)) {
        qq_log_error(0, "qq_mysql_wifi_insert_into(device_id: %s) failed", device_id);
        return QQ_ERROR;
    }

    return QQ_OK;
}

qq_int_t
qq_mysql_wifi_device_update(char *device_id, char *app_id, int app_type)
{
    memset(qq_str, 0, 256);
    sprintf(qq_str, "UPDATE wifi_device SET app_id='%s' and app_type=%d WHERE device_id='%s'",
        app_id, app_type, device_id);

    if (mysql_query(qq_mysql, qq_str)) {
        qq_log_error(0, "qq_mysql_wifi_device_update(device_id: %s, app_id: %s, app_type: %d) failed",
            device_id, app_id, app_type);
        return QQ_ERROR;
    }

    return QQ_OK;
}

qq_int_t
qq_mysql_wifi_device_delete(char *device_id)
{
    memset(qq_str, 0, 256);
    sprintf(qq_str, "DELETE FROM wifi_device WHERE device_id='%s'", device_id);

    if (mysql_query(qq_mysql, qq_str)) {
        qq_log_error(0, "qq_mysql_wifi_device_delete(device_id: %s) failed", device_id);
        return QQ_ERROR;
    }

    return QQ_OK;
}


MYSQL_RES *
qq_mysql_android_app_select(char *app_id)
{
}

qq_int_t
qq_mysql_android_app_insert_into(char *app_id)
{
}

qq_int_t
qq_mysql_android_app_update(char *app_id, char *device_id, int device_type)
{
}

qq_int_t
qq_mysql_android_app_delete(char *app_id)
{
}


MYSQL_RES *
qq_mysql_ios_app_select(char *app_id)
{
}

qq_int_t
qq_mysql_ios_app_insert_into(char *app_id)
{
}

qq_int_t
qq_mysql_ios_app_update(char *app_id, char *device_id, int device_type)
{
}

qq_int_t
qq_mysql_ios_app_delete(char *app_id)
{
}

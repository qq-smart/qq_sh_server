/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_mysql.h"
#include "qq_wifi_device.h"
#include "qq_ios_app.h"
#include "qq_android_app.h"
#include "qq_app_core.h"


static void qq_wifi_device_init_connection_handler(qq_connection_t *c);
static void qq_wifi_device_pretreatment_process_handler(qq_event_t *rev);
static void qq_wifi_device_normal_process_handler(qq_event_t *rev);
static void qq_wifi_device_write_event_handler(qq_event_t *wev);

static void qq_wifi_device_connection_package_process(qq_connection_t *c, cJSON *root);
static void qq_wifi_device_status_package_process(qq_connection_t *c, cJSON *root);
static void qq_wifi_device_ping_package_process(qq_connection_t *c, cJSON *root);
static void qq_wifi_device_disconnection_package_process(qq_connection_t *c, cJSON *root);

static void qq_wifi_device_close(qq_connection_t *c, qq_int_t reason);
static qq_int_t qq_wifi_device_md5_confirm(char *id, u_char *pwd);
static qq_int_t qq_wifi_device_conflict_confirm(u_char *id, u_char *mac);
static qq_int_t qq_wifi_device_mysql_confirm(qq_connection_t *c, u_char *id);

static void qq_wifi_device_rbtree_insert_value(qq_rbtree_node_t *temp,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel);
static qq_wifi_device_rbtree_node_t *qq_wifi_device_rbtree_lookup(qq_rbtree_t *rbtree, u_char *id);


qq_rbtree_t              qq_wifi_device_rbtree;
static qq_rbtree_node_t  qq_wifi_device_sentinel;

static char              qq_wifi_device_md5_str[256] = {0};
static qq_uint_t         qq_wifi_device_md5_id_str_pos;

qq_int_t
qq_wifi_device_init(qq_cycle_t *cycle)
{
    if (qq_add_listening_config(SOCK_STREAM,
        QQ_WIFI_DEVICE_TCP_LISTENING_PORT,
        QQ_WIFI_DEVICE_TCP_LISTENING_POOL_SIZE,
        qq_wifi_device_init_connection_handler) == QQ_ERROR)
    {
        qq_log_error(0, "qq_wifi_device_init()->qq_add_listening_config() failed");
        return QQ_ERROR;
    }

    qq_rbtree_init(&qq_wifi_device_rbtree, &qq_wifi_device_sentinel,
                    qq_wifi_device_rbtree_insert_value);

    qq_wifi_device_md5_id_str_pos = strlen(QQ_WIFI_DEVICE_MD5_CORE_STR);
    memcpy(qq_wifi_device_md5_str, QQ_WIFI_DEVICE_MD5_CORE_STR, strlen(QQ_WIFI_DEVICE_MD5_CORE_STR));

    return QQ_OK;
}

void
qq_wifi_device_done(void)
{
    qq_log_debug("qq_wifi_device_done()");
}

qq_wifi_device_t *
qq_wifi_device_node(char *id)
{
    return NULL;
}

void
qq_wifi_device_and_app_associate(void *device_id, void *app_id)
{
}


static void
qq_wifi_device_init_connection_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_wifi_device_init_connection_handler()");

    rev = c->read;
    rev->handler = qq_wifi_device_pretreatment_process_handler;

    wev = c->write;
    wev->handler = qq_wifi_device_write_event_handler;

    qq_event_add_timer(rev, QQ_WIFI_DEVICE_CONNECTION_TIMEOUT);
}

static void
qq_wifi_device_pretreatment_process_handler(qq_event_t *rev)
{
    qq_connection_t        *c;
    ssize_t                 n;
    size_t                  size;
    qq_wifi_device_t       *wd;
    qq_listening_t         *ls;
    cJSON                  *root, *cmd;

    qq_log_debug("qq_wifi_device_pretreatment_process_handler()");

    c = rev->data;
    ls = c->listening;

    if (rev->timedout) {
        qq_log_debug("timeout");
        qq_app_close_connection(c);
        return;
    }

    if (c->close) {
        qq_app_close_connection(c);
        return;
    }

    wd = c->data;
    if (wd == NULL) {
        wd = qq_pcalloc(c->pool, sizeof(qq_wifi_device_t));
        if (wd == NULL) {
            qq_app_close_connection(c);
            return;
        }
        c->data = (void *) wd;

        size = ls->pool_size -
            ((size_t)((uintptr_t)c->data - (uintptr_t)c->pool) + sizeof(qq_wifi_device_t));
        wd->buf = qq_pnalloc(c->pool, size);
        if (wd->buf == NULL) {
            qq_app_close_connection(c);
            return;
        }
        wd->buf_size = size;
    }

    n = c->recv(c, wd->buf, wd->buf_size);
    if (n == QQ_AGAIN) {
        qq_log_debug("recv not reday");
        return;
    }

    if (n == QQ_ERROR) {
        qq_app_close_connection(c);
        return;
    }

    if (n == 0) {
        qq_app_close_connection(c);
        return;
    }

    wd->buf_ndata = n;

    root = cJSON_Parse(wd->buf);
    if(root == NULL) {
        qq_log_error(0, "qq_wifi_device_process_connection_package()->cJSON_Parse() failed, before: %s",
            cJSON_GetErrorPtr());
    }
    else {
        cmd = cJSON_GetObjectItem(root, QQ_PKG_KEY_CMD);
        if (cmd != NULL && cmd->type == cJSON_String) {
            if (strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_CONN) == 0) {
                qq_wifi_device_connection_package_process(c, root);
            }
            else {
                if (wd->connection && strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_DEV_REPORT_STA) == 0) {
                    qq_wifi_device_status_package_process(c, root);
                }
            }
        }
        cJSON_Delete(root);
    }
}

static void
qq_wifi_device_normal_process_handler(qq_event_t *rev)
{
    qq_connection_t        *c;
    ssize_t                 n;
    size_t                  size;
    qq_wifi_device_t       *wd;
    cJSON                  *root, *cmd;

    qq_log_debug("qq_wifi_device_normal_process_handler()");

    c = rev->data;
    if (rev->timedout) {
        qq_wifi_device_close(c, QQ_WIFI_DEVICE_CONNECTION_CLOSE);
        return;
    }

    if (c->close) {
        qq_wifi_device_close(c, QQ_WIFI_DEVICE_CONNECTION_CLOSE);
        return;
    }

    wd = c->data;
    n = c->recv(c, wd->buf, wd->buf_size);
    if (n == QQ_AGAIN) {
        qq_log_debug("recv not reday");
        return;
    }

    if (n == QQ_ERROR) {
        qq_wifi_device_close(c, QQ_WIFI_DEVICE_CONNECTION_ERROR);
        return;
    }

    if (n == 0) {
        qq_wifi_device_close(c, QQ_WIFI_DEVICE_CONNECTION_CLOSE);
        return;
    }

    wd->buf_ndata = n;

    root = cJSON_Parse(wd->buf);
    if(root == NULL) {
        qq_log_error(0, "qq_wifi_device_process_connection_package()->cJSON_Parse() failed, before: %s", cJSON_GetErrorPtr());
    }
    else {
        cmd = cJSON_GetObjectItem(root, QQ_PKG_KEY_CMD);
        if (cmd != NULL && cmd->type == cJSON_String) {
            if (strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_DEV_REPORT_STA) == 0) {
                qq_wifi_device_status_package_process(c, root);
            }
            else if (strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_PING) == 0) {
                qq_wifi_device_ping_package_process(c, root);
            }
            else if (strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_DISCONN) == 0) {
                qq_wifi_device_disconnection_package_process(c, root);
            }
        }
        cJSON_Delete(root);
    }
}

static void
qq_wifi_device_write_event_handler(qq_event_t *wev)
{
}


static void
qq_wifi_device_connection_package_process(qq_connection_t *c, cJSON *root)
{
    qq_wifi_device_t       *wd;
    qq_int_t                reason;
    cJSON                  *json_id, *json_mac, *json_pwd;
    u_char                  id[16], mac[6], pwd[16];

    qq_log_debug("qq_wifi_device_connection_package_process(%s)", root);

    json_id = cJSON_GetObjectItem(root, QQ_PKG_KEY_ID);
    json_mac = cJSON_GetObjectItem(root, QQ_PKG_KEY_MAC);
    json_pwd = cJSON_GetObjectItem(root, QQ_PKG_KEY_PWD);

    if (json_id != NULL && json_id->type == cJSON_String && strlen(json_id->valuestring) == 32 &&
        json_mac != NULL && json_mac->type == cJSON_String && strlen(json_mac->valuestring) == 12 &&
        json_pwd != NULL && json_pwd->type == cJSON_String && strlen(json_pwd->valuestring) == 32)
    {
        if (qq_str_to_hex(json_id->valuestring, id, 32) == QQ_ERROR ||
            qq_str_to_hex(json_mac->valuestring, mac, 12) == QQ_ERROR ||
            qq_str_to_hex(json_pwd->valuestring, pwd, 32) == QQ_ERROR)
        {
            qq_wifi_device_close(c, QQ_WIFI_DEVICE_PKG_FAILED);
            return;
        }

        if (qq_wifi_device_md5_confirm(json_id->valuestring, pwd) == QQ_WIFI_DEVICE_MD5_FAILED) {
            qq_wifi_device_close(c, QQ_WIFI_DEVICE_MD5_FAILED);
            return;
        }

        if (qq_wifi_device_conflict_confirm(id, mac) == QQ_WIFI_DEVICE_CONFLICT) {
            qq_wifi_device_close(c, QQ_WIFI_DEVICE_CONFLICT);
            return;
        }

        if ((reason = qq_wifi_device_mysql_confirm(c, json_id->valuestring)) != QQ_OK) {
            qq_wifi_device_close(c, reason);
            return;
        }

        c->send(c, QQ_JSON_PKG_OK_STR, QQ_JSON_PKG_OK_STR_SIZE);

        wd = c->data;
        wd->connection = 1;
        qq_event_add_timer(c->read, QQ_WIFI_DEVICE_FIRST_STATUS_TIMEOUT);
        return;
    }

    qq_wifi_device_close(c, QQ_WIFI_DEVICE_PKG_FAILED);
}

static void
qq_wifi_device_status_package_process(qq_connection_t *c, cJSON *root)
{
    qq_wifi_device_t  *wd;
    qq_int_t           i, app_type;
    u_char            *p;

    qq_log_debug("qq_wifi_device_status_package_process(%s)", root);

    wd = c->data;
    if (wd->first_status == 0) {
        wd->first_status = 1;

        if (wd->buf_size < (wd->buf_ndata * 2)) {
            qq_wifi_device_close(c, QQ_WIFI_DEVICE_INSUFFICIENT_MEMORY);
            return;
        }

        wd->status = wd->buf;
        wd->status_size = wd->buf_ndata;
        wd->buf = wd->status + wd->status_size;
        wd->buf_size = wd->buf_ndata;

        for (i = 0;i < QQ_WIFI_DEVICE_MAX_APP_NUMBER;i++) {
            if (i == wd->napp) {
                break;
            }

            if (wd->app[i].app != NULL) {
                app_type = wd->app[i].app_type & ~QQ_MAIN_APP_FLAGS;

                if (app_type == QQ_APP_TYPE_ANDROID) {
                    qq_android_app_and_device_associate(wd->app[i].app, wd);
                }
                else if (app_type == QQ_APP_TYPE_IOS) {
                    qq_ios_app_and_device_associate(wd->app[i].app, wd);
                }
            }
        }
    }
    else {
        p = wd->status;
        wd->status = wd->buf;
        wd->buf = p;
    }

    for (i = 0;i < QQ_WIFI_DEVICE_MAX_APP_NUMBER;i++) {
        if (i == wd->napp) {
            break;
        }

        if (wd->app[i].app != NULL) {
            c->send(wd->app[i].app, wd->status, wd->status_size);
        }
    }
}

static void
qq_wifi_device_ping_package_process(qq_connection_t *c, cJSON *root)
{
    char *out;

    qq_log_debug("qq_wifi_device_ping_package_process(%s)", root);

    qq_event_add_timer(c->read, QQ_WIFI_DEVICE_PING_TIMEOUT);

    cJSON_AddNumberToObject(root, QQ_PKG_KEY_TIMESTAMP, qq_current_msec);
    out = cJSON_Print(root);

    c->send(c, out, strlen(out));
}

static void
qq_wifi_device_disconnection_package_process(qq_connection_t *c, cJSON *root)
{
    qq_log_debug("qq_wifi_device_disconnection_package_process(%s)", root);

    qq_wifi_device_close(c, QQ_WIFI_DEVICE_DISCONNECTION);
}


static void
qq_wifi_device_close(qq_connection_t *c, qq_int_t reason)
{
    switch (reason) {
        case QQ_WIFI_DEVICE_PKG_FAILED:
            c->send(c, QQ_JSON_PKG_PKG_FAILED_STR, QQ_JSON_PKG_PKG_FAILED_STR_SIZE);
            return;
        case QQ_WIFI_DEVICE_MD5_FAILED:
            c->send(c, QQ_JSON_PKG_MD5_FAILED_STR, QQ_JSON_PKG_MD5_FAILED_STR_SIZE);
            break;
        case QQ_WIFI_DEVICE_NOTHINGNESS:
            c->send(c, QQ_JSON_PKG_DEVICE_NOTHINGNESS_STR, QQ_JSON_PKG_DEVICE_NOTHINGNESS_STR_SIZE);
            break;
        case QQ_WIFI_DEVICE_NO_APP:
            c->send(c, QQ_JSON_PKG_NO_APP, QQ_JSON_PKG_NO_APP_SIZE);
            break;
        case QQ_WIFI_DEVICE_APP_OVERRANGING:
            c->send(c, QQ_JSON_PKG_APP_OVERRANGING, QQ_JSON_PKG_APP_OVERRANGING_SIZE);
            break;
        case QQ_WIFI_DEVICE_MAIN_APP_ERROR:
            c->send(c, QQ_JSON_PKG_MAIN_APP_ERROR, QQ_JSON_PKG_MAIN_APP_ERROR_SIZE);
            break;
        case QQ_WIFI_DEVICE_CONFLICT:
            c->send(c, QQ_JSON_PKG_DEVICE_CONFLICT_STR, QQ_JSON_PKG_DEVICE_CONFLICT_STR_SIZE);
            break;
        case QQ_WIFI_DEVICE_INSUFFICIENT_MEMORY:
            c->send(c, QQ_JSON_PKG_INSUFFICIENT_MEMORY, QQ_JSON_PKG_INSUFFICIENT_MEMORY_SIZE);
            break;
        case QQ_WIFI_DEVICE_CONNECTION_CLOSE:
            break;
        case QQ_WIFI_DEVICE_CONNECTION_ERROR:
            break;
        case QQ_WIFI_DEVICE_DISCONNECTION:
            break; 
    }

    qq_app_close_connection(c);
}

static qq_int_t
qq_wifi_device_md5_confirm(char *id, u_char *pwd)
{
    MD5_CTX   md5;
    u_char    md5_pwd[16];

    memcpy(&qq_wifi_device_md5_str[qq_wifi_device_md5_id_str_pos], id, 32);

    MD5Init(&md5);
    MD5Update(&md5, qq_wifi_device_md5_str, strlen((char *)qq_wifi_device_md5_str));
    MD5Final(&md5, md5_pwd);

    if (memcmp(pwd, md5_pwd, 16) == 0) {
        return QQ_OK;
    }

    return QQ_WIFI_DEVICE_MD5_FAILED;
}

static qq_int_t
qq_wifi_device_conflict_confirm(u_char *id, u_char *mac)
{
    qq_wifi_device_rbtree_node_t  *node;
    qq_wifi_device_t              *wd;

    node = qq_wifi_device_rbtree_lookup(&qq_wifi_device_rbtree, id);

    if (node == NULL) {
        return QQ_OK;
    }
    else {
        wd = (qq_wifi_device_t *) ((char *) node - offsetof(qq_wifi_device_t, rbtree));
        if (memcmp(wd->mac, mac, 6) == 0) {
            return QQ_OK;
        }
    }

    return QQ_WIFI_DEVICE_CONFLICT;
}

static qq_int_t
qq_wifi_device_mysql_confirm(qq_connection_t *c, u_char *id)
{
    MYSQL_RES              *res;
    MYSQL_ROW               row;
    qq_uint_t               n, i;
    qq_wifi_device_t       *wd;
    qq_int_t                reason, main_app, app_type;

    reason = QQ_OK;
    main_app = 0;

    res = qq_mysql_wifi_device_select(id);
    if (res == NULL) {
        return QQ_WIFI_DEVICE_NOTHINGNESS;
    }

    n = mysql_num_rows(res);
    if (n == 0) {
        reason = QQ_WIFI_DEVICE_NO_APP;
    }
    else if (n > QQ_WIFI_DEVICE_MAX_APP_NUMBER) {
        reason = QQ_WIFI_DEVICE_APP_OVERRANGING;
    }

    if (reason == QQ_OK) {
        wd = c->data;
        wd->napp = n;

        i = 0;
        while ((row = mysql_fetch_row(res)) != NULL) {
            wd->app[i].app_type = (int) *row[1];
            app_type = wd->app[i].app_type & ~QQ_MAIN_APP_FLAGS;
            main_app += (wd->app[i].app_type & QQ_MAIN_APP_FLAGS) ? 1 : 0;

            if (app_type == QQ_APP_TYPE_ANDROID) {
                wd->app[i].app = (void *) qq_android_app_node((char *) row[0]);
            }
            else if (app_type == QQ_APP_TYPE_IOS) {
                wd->app[i].app = (void *) qq_ios_app_node((char *) row[0]);
            }
        }
    }

    if (main_app != 1) {
        reason = QQ_WIFI_DEVICE_MAIN_APP_ERROR;
    }

    mysql_free_result(res);

    return reason;
}


static void
qq_wifi_device_rbtree_insert_value(qq_rbtree_node_t *temp,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel)
{
    qq_wifi_device_rbtree_node_t   *n, *t;
    qq_rbtree_node_t               **p;

    for ( ;; ) {
        n = (qq_wifi_device_rbtree_node_t *) node;
        t = (qq_wifi_device_rbtree_node_t *) temp;

        if (node->key != temp->key) {
            p = (node->key < temp->key) ? &temp->left : &temp->right;
        } else {
            p = (memcmp(n->id, t->id, 16) < 0)
                 ? &temp->left : &temp->right;
        }

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    qq_rbt_red(node);
}

static qq_wifi_device_rbtree_node_t *
qq_wifi_device_rbtree_lookup(qq_rbtree_t *rbtree, u_char *id)
{
    qq_int_t                       rc;
    qq_wifi_device_rbtree_node_t  *n;
    qq_rbtree_node_t              *node, *sentinel;
    uint32_t                       hash;

    node = rbtree->root;
    sentinel = rbtree->sentinel;
    hash = qq_wifi_device_rbtree_key(id);

    while (node != sentinel) {
        n = (qq_wifi_device_rbtree_node_t *) node;

        if (hash != node->key) {
            node = (hash < node->key) ? node->left : node->right;
            continue;
        }

        rc = memcmp(id, n->id, 16);
        if (rc < 0) {
            node = node->left;
            continue;
        }
        if (rc > 0) {
            node = node->right;
            continue;
        }

        return n;
    }

    return NULL;
}

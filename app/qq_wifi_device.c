/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_wifi_device.h"
#include "qq_app_package.h"


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
static qq_int_t qq_wifi_device_validation_checking(u_char *id);
static qq_int_t qq_wifi_device_conflict_check(u_char *id, u_char *mac);

static void qq_wifi_device_rbtree_insert_value(qq_rbtree_node_t *temp,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel);
static qq_wifi_device_rbtree_node_t *qq_wifi_device_rbtree_lookup(qq_rbtree_t *rbtree, u_char *id);


qq_rbtree_t              qq_wifi_device_rbtree;
static qq_rbtree_node_t  qq_wifi_device_sentinel;

static char              qq_wifi_device_md5_str[256] = {0};
static qq_uint_t         qq_wifi_device_md5_id;

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

    qq_wifi_device_md5_id = strlen(QQ_WIFI_DEVICE_MD5_STR);
    memcpy(qq_wifi_device_md5_str, QQ_WIFI_DEVICE_MD5_STR, qq_wifi_device_md5_id);
    qq_wifi_device_md5_id = strlen(QQ_WIFI_DEVICE_MD5_STR);

    return QQ_OK;
}

void
qq_wifi_device_done(void)
{
    qq_log_debug("qq_wifi_device_done()");
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
    qq_wifi_device_info_t  *info;
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

    info = c->data;
    if (info == NULL) {
        info = qq_pcalloc(c->pool, sizeof(qq_wifi_device_info_t));
        if (info == NULL) {
            qq_app_close_connection(c);
            return;
        }
        c->data = (void *) info;

        size = ls->pool_size -
            ((size_t)((uintptr_t)c->data - (uintptr_t)c->pool) + sizeof(qq_wifi_device_info_t));
        info->buf_start = qq_pnalloc(c->pool, size);
        if (info->buf_start == NULL) {
            qq_app_close_connection(c);
            return;
        }
        info->buf_end = info->buf_start + size;
        info->buf_size = size;

        info->tmp_start = info->buf_start;
        info->tmp_pos = info->buf_start;
        info->tmp_last = info->buf_start;
        info->tmp_end = info->buf_end;
        info->tmp_size = info->buf_size;
    }

    n = c->recv(c, info->tmp_pos, info->tmp_size);
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

    root = cJSON_Parse(info->tmp_pos);
    if(root == NULL) {
        qq_log_error(0, "qq_wifi_device_process_connection_package()->cJSON_Parse() failed, before: %s", cJSON_GetErrorPtr());
    }
    else {
        cmd = cJSON_GetObjectItem(root, QQ_PKG_KEY_CMD);
        if (cmd != NULL && cmd->type == cJSON_String) {
            if (strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_CONN) == 0) {
                qq_wifi_device_connection_package_process(c, root);
            }
            else {
                if (info->connection && strcmp(cmd->valuestring, QQ_PKG_VALUE_CMD_DEV_REPORT_STA) == 0) {
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
    qq_wifi_device_info_t  *info;
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

    info = c->data;
    n = c->recv(c, info->tmp_pos, info->tmp_size);
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

    root = cJSON_Parse(info->tmp_pos);
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
    qq_wifi_device_info_t  *info;
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

        if (qq_wifi_device_validation_checking(id) == QQ_WIFI_DEVICE_NOTHINGNESS) {
            qq_wifi_device_close(c, QQ_WIFI_DEVICE_NOTHINGNESS);
            return;
        }

        if (qq_wifi_device_conflict_check(id, mac) == QQ_WIFI_DEVICE_CONFLICT) {
            qq_wifi_device_close(c, QQ_WIFI_DEVICE_CONFLICT);
            return;
        }

        c->send(c, QQ_JSON_PKG_OK_STR, QQ_JSON_PKG_OK_STR_SIZE);

        info = c->data;
        info->connection = 1;
        qq_event_add_timer(c->read, QQ_WIFI_DEVICE_FIRST_STATUS_TIMEOUT);
        return;
    }

    qq_wifi_device_close(c, QQ_WIFI_DEVICE_PKG_FAILED);
}

static void
qq_wifi_device_status_package_process(qq_connection_t *c, cJSON *root)
{
    qq_log_debug("qq_wifi_device_status_package_process(%s)", root);
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
            break;
        case QQ_WIFI_DEVICE_MD5_FAILED:
            break;
        case QQ_WIFI_DEVICE_NOTHINGNESS:
            break;
        case QQ_WIFI_DEVICE_CONFLICT:
            break;
        case QQ_WIFI_DEVICE_CONNECTION_CLOSE:
            break;
        case QQ_WIFI_DEVICE_CONNECTION_ERROR:
            break;
        case QQ_WIFI_DEVICE_DISCONNECTION:
            break; 
    }
}

static qq_int_t
qq_wifi_device_md5_confirm(char *id, u_char *pwd)
{
    MD5_CTX   md5;
    u_char    md5_pwd[16];

    memcpy(&qq_wifi_device_md5_str[qq_wifi_device_md5_id],
        id, 16);

    MD5Init(&md5);
    MD5Update(&md5, qq_wifi_device_md5_str, strlen((char *)qq_wifi_device_md5_str));
    MD5Final(&md5, md5_pwd);

    if (memcmp(pwd, md5_pwd, 16) == 0) {
        return QQ_OK;
    }

    return QQ_WIFI_DEVICE_MD5_FAILED;
}

static qq_int_t
qq_wifi_device_validation_checking(u_char *id)
{
    return QQ_OK;
}

static qq_int_t
qq_wifi_device_conflict_check(u_char *id, u_char *mac)
{
    qq_wifi_device_rbtree_node_t  *node;
    qq_wifi_device_info_t         *info;

    node = qq_wifi_device_rbtree_lookup(&qq_wifi_device_rbtree, id);

    if (node == NULL) {
        return QQ_OK;
    }
    else {
        info = (qq_wifi_device_info_t *) ((char *) node - offsetof(qq_wifi_device_info_t, rbtree));
        if (memcmp(info->mac, mac, 6) == 0) {
            return QQ_OK;
        }
    }

    return QQ_WIFI_DEVICE_CONFLICT;
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

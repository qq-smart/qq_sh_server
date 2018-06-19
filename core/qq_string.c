/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


size_t
qq_strnlen(u_char *p, size_t n)
{
    size_t  i;

    for (i = 0; i < n; i++) {
        if (p[i] == '\0') {
            return i;
        }
    }

    return n;
}

u_char *
qq_cpystrn(u_char *dst, u_char *src, size_t n)
{
    if (n == 0) {
        return dst;
    }

    while (--n) {
        *dst = *src;

        if (*dst == '\0') {
            return dst;
        }

        dst++;
        src++;
    }

    *dst = '\0';

    return dst;
}

u_char *
qq_hex_dump(u_char *dst, u_char *src, size_t len)
{
    static u_char  hex[] = "0123456789abcdef";

    while (len--) {
        *dst++ = hex[*src >> 4];
        *dst++ = hex[*src++ & 0xf];
    }

    return dst;
}

qq_int_t
qq_str_to_hex(u_char *str, u_char *hex, size_t strlen)
{
    if (strlen % 2) {
        return QQ_ERROR;
    }

    while (strlen) {
        if (*str >= 48 && *str <= 57) {
            *hex = *str - 48;
        } else if (*str >= 65 && *str <= 70) {
            *hex = *str - 55;
        } else if (*str >= 97 && *str <= 102) {
            *hex = *str - 87;
        } else {
            return QQ_ERROR;
        }

        str ++;
        *hex = *hex << 4;

        if (*str >= 48 && *str <= 57) {
            *hex += *str - 48;
        } else if (*str >= 65 && *str <= 70) {
            *hex += *str - 55;
        } else if (*str >= 97 && *str <= 102) {
            *hex += *str - 87;
        } else {
            return QQ_ERROR;
        }

        str ++;
        hex ++;

        strlen -= 2;
    }

    return QQ_OK;
}


void
qq_str_rbtree_insert_value(qq_rbtree_node_t *temp,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel)
{
    qq_str_rbtree_node_t   *n, *t;
    qq_rbtree_node_t      **p;

    for ( ;; ) {
        n = (qq_str_rbtree_node_t *) node;
        t = (qq_str_rbtree_node_t *) temp;

        if (node->key != temp->key) {
            p = (node->key < temp->key) ? &temp->left : &temp->right;
        } else if (n->str.len != t->str.len) {
            p = (n->str.len < t->str.len) ? &temp->left : &temp->right;
        } else {
            p = (memcmp(n->str.data, t->str.data, n->str.len) < 0)
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


qq_str_rbtree_node_t *
qq_str_rbtree_lookup(qq_rbtree_t *rbtree, qq_str_t *val, uint32_t hash)
{
    qq_int_t               rc;
    qq_str_rbtree_node_t  *n;
    qq_rbtree_node_t      *node, *sentinel;

    node = rbtree->root;
    sentinel = rbtree->sentinel;

    while (node != sentinel) {
        n = (qq_str_rbtree_node_t *) node;

        if (hash != node->key) {
            node = (hash < node->key) ? node->left : node->right;
            continue;
        }

        if (val->len != n->str.len) {
            node = (val->len < n->str.len) ? node->left : node->right;
            continue;
        }

        rc = memcmp(val->data, n->str.data, val->len);
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

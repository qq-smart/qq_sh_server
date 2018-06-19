/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_STRING_H_INCLUDED_
#define _QQ_STRING_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct {
    size_t      len;
    u_char     *data;
} qq_str_t;


#define qq_string(str)         { sizeof(str) - 1, (u_char *) str }
#define qq_null_string         { 0, NULL }
#define qq_str_set(str, text)  (str)->len = sizeof(text) - 1; (str)->data = (u_char *) text
#define qq_str_null(str)       (str)->len = 0; (str)->data = NULL

#define qq_tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define qq_toupper(c)      (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

#define qq_cpymem(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))

size_t qq_strnlen(u_char *p, size_t n);
u_char *qq_cpystrn(u_char *dst, u_char *src, size_t n);

u_char *qq_hex_dump(u_char *dst, u_char *src, size_t len);
qq_int_t qq_str_to_hex(u_char *str, u_char *hex, size_t strlen);


typedef struct {
    qq_rbtree_node_t   rbtree;
    qq_str_t           str;
} qq_str_rbtree_node_t;

void qq_str_rbtree_insert_value(qq_rbtree_node_t *temp,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel);
qq_str_rbtree_node_t *qq_str_rbtree_lookup(qq_rbtree_t *rbtree, qq_str_t *name,
    uint32_t hash);


#define qq_value_helper(n)   #n
#define qq_value(n)          qq_value_helper(n)


#endif /* _QQ_STRING_H_INCLUDED_ */

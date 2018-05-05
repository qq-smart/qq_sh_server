/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_RBTREE_H_INCLUDED_
#define _QQ_RBTREE_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct qq_rbtree_node_s  qq_rbtree_node_t;

struct qq_rbtree_node_s {
    qq_rbtree_key_t       key;
    qq_rbtree_node_t     *left;
    qq_rbtree_node_t     *right;
    qq_rbtree_node_t     *parent;
    u_char                color;
    u_char                data;
};


typedef struct qq_rbtree_s  qq_rbtree_t;

typedef void (*qq_rbtree_insert_pt) (qq_rbtree_node_t *root,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel);

struct qq_rbtree_s {
    qq_rbtree_node_t     *root;
    qq_rbtree_node_t     *sentinel;
    qq_rbtree_insert_pt   insert;
};


#define qq_rbtree_init(tree, s, i)                                           \
    qq_rbtree_sentinel_init(s);                                              \
    (tree)->root = s;                                                         \
    (tree)->sentinel = s;                                                     \
    (tree)->insert = i


void qq_rbtree_insert(qq_rbtree_t *tree, qq_rbtree_node_t *node);
void qq_rbtree_delete(qq_rbtree_t *tree, qq_rbtree_node_t *node);
void qq_rbtree_insert_value(qq_rbtree_node_t *root, qq_rbtree_node_t *node,
    qq_rbtree_node_t *sentinel);
void qq_rbtree_insert_timer_value(qq_rbtree_node_t *root,
    qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel);
qq_rbtree_node_t *qq_rbtree_next(qq_rbtree_t *tree,
    qq_rbtree_node_t *node);


#define qq_rbt_red(node)               ((node)->color = 1)
#define qq_rbt_black(node)             ((node)->color = 0)
#define qq_rbt_is_red(node)            ((node)->color)
#define qq_rbt_is_black(node)          (!qq_rbt_is_red(node))
#define qq_rbt_copy_color(n1, n2)      (n1->color = n2->color)


/* a sentinel must be black */

#define qq_rbtree_sentinel_init(node)  qq_rbt_black(node)


static inline qq_rbtree_node_t *
qq_rbtree_min(qq_rbtree_node_t *node, qq_rbtree_node_t *sentinel)
{
    while (node->left != sentinel) {
        node = node->left;
    }

    return node;
}


#endif /* _QQ_RBTREE_H_INCLUDED_ */

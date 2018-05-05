/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static inline void
qq_rbtree_left_rotate(qq_rbtree_node_t **root, qq_rbtree_node_t *sentinel,
    qq_rbtree_node_t *node)
{
    qq_rbtree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;
    } else if (node == node->parent->left) {
        node->parent->left = temp;
    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}

static inline void
qq_rbtree_right_rotate(qq_rbtree_node_t **root, qq_rbtree_node_t *sentinel,
    qq_rbtree_node_t *node)
{
    qq_rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;
    } else if (node == node->parent->right) {
        node->parent->right = temp;
    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}


void
qq_rbtree_insert(qq_rbtree_t *tree, qq_rbtree_node_t *node)
{
    qq_rbtree_node_t  **root, *temp, *sentinel;

    root = &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        qq_rbt_black(node);
        *root = node;

        return;
    }

    tree->insert(*root, node, sentinel);

    while (node != *root && qq_rbt_is_red(node->parent)) {
        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (qq_rbt_is_red(temp)) {
                qq_rbt_black(node->parent);
                qq_rbt_black(temp);
                qq_rbt_red(node->parent->parent);
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    qq_rbtree_left_rotate(root, sentinel, node);
                }

                qq_rbt_black(node->parent);
                qq_rbt_red(node->parent->parent);
                qq_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }
        } else {
            temp = node->parent->parent->left;

            if (qq_rbt_is_red(temp)) {
                qq_rbt_black(node->parent);
                qq_rbt_black(temp);
                qq_rbt_red(node->parent->parent);
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    qq_rbtree_right_rotate(root, sentinel, node);
                }

                qq_rbt_black(node->parent);
                qq_rbt_red(node->parent->parent);
                qq_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    qq_rbt_black(*root);
}

void
qq_rbtree_insert_value(qq_rbtree_node_t *temp, qq_rbtree_node_t *node,
    qq_rbtree_node_t *sentinel)
{
    qq_rbtree_node_t  **p;

    for ( ;; ) {
        p = (node->key < temp->key) ? &temp->left : &temp->right;

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

void
qq_rbtree_insert_timer_value(qq_rbtree_node_t *temp, qq_rbtree_node_t *node,
    qq_rbtree_node_t *sentinel)
{
    qq_rbtree_node_t  **p;

    for ( ;; ) {
        /*
         * Timer values
         * 1) are spread in small range, usually several minutes,
         * 2) and overflow each 49 days, if milliseconds are stored in 32 bits.
         * The comparison takes into account that overflow.
         */

        /*  node->key < temp->key */

        p = ((qq_rbtree_key_int_t) (node->key - temp->key) < 0)
            ? &temp->left : &temp->right;

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

void
qq_rbtree_delete(qq_rbtree_t *tree, qq_rbtree_node_t *node)
{
    qq_uint_t           red;
    qq_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    root = &tree->root;
    sentinel = tree->sentinel;

    if (node->left == sentinel) {
        temp = node->right;
        subst = node;
    } else if (node->right == sentinel) {
        temp = node->left;
        subst = node;
    } else {
        subst = qq_rbtree_min(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {
        *root = temp;
        qq_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    red = qq_rbt_is_red(subst);

    if (subst == subst->parent->left) {
        subst->parent->left = temp;
    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {
        temp->parent = subst->parent;
    } else {
        if (subst->parent == node) {
            temp->parent = subst;
        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        qq_rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;
        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {
        return;
    }

    /* a delete fixup */

    while (temp != *root && qq_rbt_is_black(temp)) {
        if (temp == temp->parent->left) {
            w = temp->parent->right;

            if (qq_rbt_is_red(w)) {
                qq_rbt_black(w);
                qq_rbt_red(temp->parent);
                qq_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (qq_rbt_is_black(w->left) && qq_rbt_is_black(w->right)) {
                qq_rbt_red(w);
                temp = temp->parent;
            } else {
                if (qq_rbt_is_black(w->right)) {
                    qq_rbt_black(w->left);
                    qq_rbt_red(w);
                    qq_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                qq_rbt_copy_color(w, temp->parent);
                qq_rbt_black(temp->parent);
                qq_rbt_black(w->right);
                qq_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        } else {
            w = temp->parent->left;

            if (qq_rbt_is_red(w)) {
                qq_rbt_black(w);
                qq_rbt_red(temp->parent);
                qq_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (qq_rbt_is_black(w->left) && qq_rbt_is_black(w->right)) {
                qq_rbt_red(w);
                temp = temp->parent;
            } else {
                if (qq_rbt_is_black(w->left)) {
                    qq_rbt_black(w->right);
                    qq_rbt_red(w);
                    qq_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                qq_rbt_copy_color(w, temp->parent);
                qq_rbt_black(temp->parent);
                qq_rbt_black(w->left);
                qq_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    qq_rbt_black(temp);
}

qq_rbtree_node_t *
qq_rbtree_next(qq_rbtree_t *tree, qq_rbtree_node_t *node)
{
    qq_rbtree_node_t  *root, *sentinel, *parent;

    sentinel = tree->sentinel;

    if (node->right != sentinel) {
        return qq_rbtree_min(node->right, sentinel);
    }

    root = tree->root;

    for ( ;; ) {
        parent = node->parent;

        if (node == root) {
            return NULL;
        }

        if (node == parent->left) {
            return parent;
        }

        node = parent;
    }
}

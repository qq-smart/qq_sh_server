/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_buf_t *
qq_create_temp_buf(qq_pool_t *pool, size_t size)
{
    qq_buf_t *b;

    b = qq_calloc_buf(pool);
    if (b == NULL) {
        return NULL;
    }

    b->start = qq_palloc(pool, size);
    if (b->start == NULL) {
        return NULL;
    }

    b->pos = b->start;
    b->last = b->start;
    b->end = b->last + size;
    b->temporary = 1;

    return b;
}

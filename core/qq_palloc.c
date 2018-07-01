/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


qq_pool_t *
qq_create_pool(size_t size)
{
    qq_pool_t  *p;

    p = qq_memalign(QQ_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }

    p->last = (u_char *) p + sizeof(qq_pool_t);
    p->end = (u_char *) p + size;

    return p;
}

void *
qq_palloc(qq_pool_t *pool, size_t size)
{
    u_char      *m;
    qq_pool_t   *p;

    p = pool;
    m = p->last;

    m = qq_align_ptr(m, QQ_ALIGNMENT);

    if ((size_t) (p->end - m) >= size) {
        p->last = m + size;
        return m;
    }

    return NULL;
}

void *
qq_pnalloc(qq_pool_t *pool, size_t size)
{
    u_char      *m;
    qq_pool_t   *p;

    p = pool;
    m = p->last;

    if ((size_t) (p->end - m) >= size) {
        p->last = m + size;
        return m;
    }
    
    return NULL;
}

void *
qq_pcalloc(qq_pool_t *pool, size_t size)
{
    void *p;

    p = qq_palloc(pool, size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}

void *
qq_pcnalloc(qq_pool_t *pool, size_t size)
{
    void *p;

    p = qq_pnalloc(pool, size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}

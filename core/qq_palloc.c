/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


static inline void *qq_palloc_small(qq_pool_t *pool, size_t size,
    qq_uint_t align);
static void *qq_palloc_block(qq_pool_t *pool, size_t size);
static void *qq_palloc_large(qq_pool_t *pool, size_t size);


qq_pool_t *
qq_create_pool(size_t size)
{
    qq_pool_t  *p;

    p = qq_memalign(QQ_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (u_char *) p + sizeof(qq_pool_t);
    p->d.end = (u_char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(qq_pool_t);
    p->max = (size < QQ_MAX_ALLOC_FROM_POOL) ? size : QQ_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;

    return p;
}

void
qq_destroy_pool(qq_pool_t *pool)
{
    qq_pool_t          *p, *n;
    qq_pool_large_t    *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        free(p);

        if (n == NULL) {
            break;
        }
    }
}

void
qq_reset_pool(qq_pool_t *pool)
{
    qq_pool_t        *p;
    qq_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (u_char *) p + sizeof(qq_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->large = NULL;
}

void *
qq_palloc(qq_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return qq_palloc_small(pool, size, 1);
    }

    return qq_palloc_large(pool, size);
}

void *
qq_pnalloc(qq_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return qq_palloc_small(pool, size, 0);
    }

    return qq_palloc_large(pool, size);
}

static inline void *
qq_palloc_small(qq_pool_t *pool, size_t size, qq_uint_t align)
{
    u_char      *m;
    qq_pool_t   *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = qq_align_ptr(m, QQ_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;
            return m;
        }

        p = p->d.next;
    } while (p);

    return qq_palloc_block(pool, size);
}

static void *
qq_palloc_block(qq_pool_t *pool, size_t size)
{
    u_char      *m;
    size_t       psize;
    qq_pool_t   *p, *new;

    psize = (size_t) (pool->d.end - (u_char *) pool);

    m = qq_memalign(QQ_POOL_ALIGNMENT, psize);
    if (m == NULL) {
        return NULL;
    }

    new = (qq_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(qq_pool_data_t);
    m = qq_align_ptr(m, QQ_ALIGNMENT);
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}

static void *
qq_palloc_large(qq_pool_t *pool, size_t size)
{
    void              *p;
    qq_uint_t          n;
    qq_pool_large_t   *large;

    p = qq_alloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = qq_palloc_small(pool, sizeof(qq_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

void *
qq_pmemalign(qq_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    qq_pool_large_t   *large;

    p = qq_memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = qq_palloc_small(pool, sizeof(qq_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

qq_int_t
qq_pfree(qq_pool_t *pool, void *p)
{
    qq_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            qq_log_debug("free: %p", l->alloc);
            free(l->alloc);
            l->alloc = NULL;

            return QQ_OK;
        }
    }

    return QQ_DECLINED;
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

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_PALLOC_H_INCLUDED_
#define _QQ_PALLOC_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_MAX_ALLOC_FROM_POOL  (qq_pagesize - 1)

#define QQ_DEFAULT_POOL_SIZE    (16 * 1024)

#define QQ_POOL_ALIGNMENT       16
#define QQ_MIN_POOL_SIZE \
    qq_align((sizeof(qq_pool_t) + 2 * sizeof(qq_pool_large_t)), \
              QQ_POOL_ALIGNMENT)


typedef struct qq_pool_large_s  qq_pool_large_t;

struct qq_pool_large_s {
    qq_pool_large_t      *next;
    void                 *alloc;
};


typedef struct {
    u_char               *last;
    u_char               *end;
    qq_pool_t            *next;
    qq_uint_t             failed;
} qq_pool_data_t;


struct qq_pool_s {
    qq_pool_data_t        d;
    size_t                max;
    qq_pool_t            *current;
    qq_pool_large_t      *large;
};


void *qq_alloc(size_t size);
void *qq_calloc(size_t size);

qq_pool_t *qq_create_pool(size_t size);
void qq_destroy_pool(qq_pool_t *pool);
void qq_reset_pool(qq_pool_t *pool);

void *qq_palloc(qq_pool_t *pool, size_t size);
void *qq_pnalloc(qq_pool_t *pool, size_t size);
void *qq_pcalloc(qq_pool_t *pool, size_t size);
void *qq_pmemalign(qq_pool_t *pool, size_t size, size_t alignment);
qq_int_t qq_pfree(qq_pool_t *pool, void *p);


#endif /* _QQ_PALLOC_H_INCLUDED_ */

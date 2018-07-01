/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_PALLOC_H_INCLUDED_
#define _QQ_PALLOC_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_POOL_ALIGNMENT       16
#define QQ_DEFAULT_POOL_SIZE    (16 * 1024)


struct qq_pool_s {
    u_char  *last;
    u_char  *end;
};


qq_pool_t *qq_create_pool(size_t size);
#define qq_destroy_pool(p)  free(p)

void *qq_palloc(qq_pool_t *pool, size_t size);
void *qq_pnalloc(qq_pool_t *pool, size_t size);
void *qq_pcalloc(qq_pool_t *pool, size_t size);
void *qq_pcnalloc(qq_pool_t *pool, size_t size);


#endif /* _QQ_PALLOC_H_INCLUDED_ */

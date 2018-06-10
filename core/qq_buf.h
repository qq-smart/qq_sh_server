/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_BUF_H_INCLUDED_
#define _QQ_BUF_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct qq_buf_s  qq_buf_t;

struct qq_buf_s {
    u_char          *pos;
    u_char          *last;
    off_t            file_pos;
    off_t            file_last;

    u_char          *start;         /* start of buffer */
    u_char          *end;           /* end of buffer */

    /* the buf's content could be changed */
    unsigned         temporary:1;

    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
     */
    unsigned         memory:1;

    /* the buf's content is mmap()ed and must not be changed */
    unsigned         mmap:1;

    unsigned         recycled:1;
    unsigned         in_file:1;
    unsigned         flush:1;
    unsigned         sync:1;
    unsigned         last_buf:1;
    unsigned         last_in_chain:1;

    unsigned         last_shadow:1;
    unsigned         temp_file:1;
};


qq_buf_t *qq_create_temp_buf(qq_pool_t *pool, size_t size);

#define qq_alloc_buf(pool)  qq_palloc(pool, sizeof(qq_buf_t))
#define qq_calloc_buf(pool) qq_pcalloc(pool, sizeof(qq_buf_t))


#endif /* _QQ_BUF_H_INCLUDED_ */

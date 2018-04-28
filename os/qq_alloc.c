/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


void *
qq_alloc(size_t size)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        qq_log_error("malloc(%uz) failed", size);
    }

    return p;
}

void *
qq_calloc(size_t size)
{
    void  *p;

    p = qq_alloc(size);

    if (p) {
        memset(p, 0, size);
    }

    return p;
}

void *
qq_memalign(size_t alignment, size_t size)
{
    void  *p;

    p = memalign(alignment, size);
    if (p == NULL) {
        qq_log_error("memalign(%uz, %uz) failed", alignment, size);
    }

    return p;
}

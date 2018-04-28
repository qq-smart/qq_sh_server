/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_ALLOC_H_INCLUDED_
#define _QQ_ALLOC_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


void *qq_alloc(size_t size);
void *qq_calloc(size_t size);
void *qq_memalign(size_t alignment, size_t size);


#endif /* _QQ_ALLOC_H_INCLUDED_ */

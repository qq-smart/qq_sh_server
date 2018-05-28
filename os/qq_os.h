/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_OS_H_INCLUDED_
#define _QQ_OS_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef ssize_t (*qq_recv_pt)(qq_connection_t *c, u_char *buf, size_t size);
typedef ssize_t (*qq_send_pt)(qq_connection_t *c, u_char *buf, size_t size);


#endif /* _QQ_OS_H_INCLUDED_ */

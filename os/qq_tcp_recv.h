/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_TCP_RECV_H_INCLUDED_
#define _QQ_TCP_RECV_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


ssize_t qq_tcp_recv(qq_connection_t *c, u_char *buf, size_t size);


#endif /* _QQ_TCP_RECV_H_INCLUDED_ */

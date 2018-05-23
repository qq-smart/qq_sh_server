/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_SOCKET_H_INCLUDED_
#define _QQ_SOCKET_H_INCLUDED_


#include "qq_config.h"


#define qq_nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)
#define qq_nonblocking_n   "fcntl(O_NONBLOCK)"

#define qq_blocking(s)     fcntl(s, F_SETFL, fcntl(s, F_GETFL) & ~O_NONBLOCK)
#define qq_blocking_n      "fcntl(!O_NONBLOCK)"


int qq_tcp_nopush(qq_socket_t s);
int qq_tcp_push(qq_socket_t s);


#endif /* _QQ_SOCKET_H_INCLUDED_ */

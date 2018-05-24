/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_INET_H_INCLUDED_
#define _QQ_INET_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_INET_ADDRSTRLEN   (sizeof("255.255.255.255") - 1)
#define QQ_SOCKADDR_STRLEN   (QQ_INET_ADDRSTRLEN + sizeof(":65535") - 1)

/* compatibility */
#define QQ_SOCKADDRLEN       sizeof(qq_sockaddr_t)


size_t qq_sock_ntop(struct sockaddr_in *sin, u_char *text);


#endif /* _QQ_INET_H_INCLUDED_ */

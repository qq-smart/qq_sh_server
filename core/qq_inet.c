/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


size_t
qq_sock_ntop(struct sockaddr_in *sin, u_char *text)
{
    u_char               *p;

    p = (u_char *) &sin->sin_addr;

    return sprintf(text, "%ud.%ud.%ud.%ud:%d",
                   p[0], p[1], p[2], p[3], ntohs(sin->sin_port));
}

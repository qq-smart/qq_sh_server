/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_INET_H_INCLUDED_
#define _QQ_INET_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_INET_ADDRSTRLEN   (sizeof("255.255.255.255") - 1)
#define QQ_INET6_ADDRSTRLEN                                                 \
    (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") - 1)
#define QQ_UNIX_ADDRSTRLEN                                                  \
    (sizeof("unix:") - 1 +                                                   \
     sizeof(struct sockaddr_un) - offsetof(struct sockaddr_un, sun_path))

#if (QQ_HAVE_UNIX_DOMAIN)
#define QQ_SOCKADDR_STRLEN   QQ_UNIX_ADDRSTRLEN
#elif (QQ_HAVE_INET6)
#define QQ_SOCKADDR_STRLEN   (QQ_INET6_ADDRSTRLEN + sizeof("[]:65535") - 1)
#else
#define QQ_SOCKADDR_STRLEN   (QQ_INET_ADDRSTRLEN + sizeof(":65535") - 1)
#endif

/* compatibility */
#define QQ_SOCKADDRLEN       sizeof(qq_sockaddr_t)


typedef union {
    struct sockaddr           sockaddr;
    struct sockaddr_in        sockaddr_in;
#if (QQ_HAVE_INET6)
    struct sockaddr_in6       sockaddr_in6;
#endif
#if (QQ_HAVE_UNIX_DOMAIN)
    struct sockaddr_un        sockaddr_un;
#endif
} qq_sockaddr_t;


in_addr_t qq_inet_addr(u_char *text, size_t len);
#if (QQ_HAVE_INET6)
qq_int_t qq_inet6_addr(u_char *p, size_t len, u_char *addr);
size_t qq_inet6_ntop(u_char *p, u_char *text, size_t len);
#endif
size_t qq_sock_ntop(struct sockaddr *sa, socklen_t socklen, u_char *text,
    size_t len, qq_uint_t port);
size_t qq_inet_ntop(int family, void *addr, u_char *text, size_t len);


#endif /* _QQ_INET_H_INCLUDED_ */

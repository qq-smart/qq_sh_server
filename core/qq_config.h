/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_CONFIG_H_INCLUDED_
#define _QQ_CONFIG_H_INCLUDED_


#include "qq_app_config.h"
#include "qq_linux.h"


typedef intptr_t   qq_int_t;
typedef uintptr_t  qq_uint_t;
typedef intptr_t   qq_flag_t;
typedef int        qq_fd_t;
typedef pid_t      qq_pid_t;
typedef qq_uint_t  qq_rbtree_key_t;
typedef qq_int_t   qq_rbtree_key_int_t;
typedef qq_uint_t  qq_msec_t;
typedef qq_int_t   qq_msec_int_t;
typedef int        qq_socket_t;
typedef int        qq_err_t;


#define QQ_ALIGNMENT   sizeof(unsigned long)

#define qq_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define qq_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))


#define qq_signal_helper(n)     SIG##n
#define qq_signal_value(n)      qq_signal_helper(n)

#define QQ_SHUTDOWN_SIGNAL      QUIT
#define QQ_TERMINATE_SIGNAL     TERM


#endif /* _QQ_CONFIG_H_INCLUDED_ */

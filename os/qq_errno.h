/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_ERRNO_H_INCLUDED_
#define _QQ_ERRNO_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_EPERM         EPERM
#define QQ_ENOENT        ENOENT
#define QQ_ENOPATH       ENOENT
#define QQ_ESRCH         ESRCH
#define QQ_EINTR         EINTR
#define QQ_ECHILD        ECHILD
#define QQ_ENOMEM        ENOMEM
#define QQ_EACCES        EACCES
#define QQ_EBUSY         EBUSY
#define QQ_EEXIST        EEXIST
#define QQ_EEXIST_FILE   EEXIST
#define QQ_EXDEV         EXDEV
#define QQ_ENOTDIR       ENOTDIR
#define QQ_EISDIR        EISDIR
#define QQ_EINVAL        EINVAL
#define QQ_ENFILE        ENFILE
#define QQ_EMFILE        EMFILE
#define QQ_ENOSPC        ENOSPC
#define QQ_EPIPE         EPIPE
#define QQ_EINPROGRESS   EINPROGRESS
#define QQ_ENOPROTOOPT   ENOPROTOOPT
#define QQ_EOPNOTSUPP    EOPNOTSUPP
#define QQ_EADDRINUSE    EADDRINUSE
#define QQ_ECONNABORTED  ECONNABORTED
#define QQ_ECONNRESET    ECONNRESET
#define QQ_ENOTCONN      ENOTCONN
#define QQ_ETIMEDOUT     ETIMEDOUT
#define QQ_ECONNREFUSED  ECONNREFUSED
#define QQ_ENAMETOOLONG  ENAMETOOLONG
#define QQ_ENETDOWN      ENETDOWN
#define QQ_ENETUNREACH   ENETUNREACH
#define QQ_EHOSTDOWN     EHOSTDOWN
#define QQ_EHOSTUNREACH  EHOSTUNREACH
#define QQ_ENOSYS        ENOSYS
#define QQ_ECANCELED     ECANCELED
#define QQ_EILSEQ        EILSEQ
#define QQ_ENOMOREFILES  0
#define QQ_ELOOP         ELOOP
#define QQ_EBADF         EBADF

#if (QQ_HAVE_OPENAT)
#define QQ_EMLINK        EMLINK
#endif

#if (__hpux__)
#define QQ_EAGAIN        EWOULDBLOCK
#else
#define QQ_EAGAIN        EAGAIN
#endif


#endif /* _QQ_ERRNO_H_INCLUDED_ */

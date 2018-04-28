/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_FILES_H_INCLUDED_
#define _QQ_FILES_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define qq_open_file(name, mode, create, access) \
    open((const char *) name, mode|create, access)

#define qq_close_file           close
#define qq_delete_file(name)    unlink((const char *) name)



#define QQ_FILE_RDONLY          O_RDONLY
#define QQ_FILE_WRONLY          O_WRONLY
#define QQ_FILE_RDWR            O_RDWR
#define QQ_FILE_CREATE_OR_OPEN  O_CREAT
#define QQ_FILE_OPEN            0
#define QQ_FILE_TRUNCATE        (O_CREAT|O_TRUNC)
#define QQ_FILE_APPEND          (O_WRONLY|O_APPEND)
#define QQ_FILE_NONBLOCK        O_NONBLOCK

#define QQ_FILE_DEFAULT_ACCESS  0644
#define QQ_FILE_OWNER_ACCESS    0600


#endif /* _QQ_FILES_H_INCLUDED_ */

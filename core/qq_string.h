/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_STRING_H_INCLUDED_
#define _QQ_STRING_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


typedef struct {
    size_t      len;
    u_char     *data;
} qq_str_t;


#define qq_string(str)         { sizeof(str) - 1, (u_char *) str }
#define qq_null_string         { 0, NULL }
#define qq_str_set(str, text)  (str)->len = sizeof(text) - 1; (str)->data = (u_char *) text
#define qq_str_text(str, text) (str)->len = strlen(text); (str)->data = (u_char *)text
#define qq_str_null(str)       (str)->len = 0; (str)->data = NULL


#endif /* _QQ_STRING_H_INCLUDED_ */

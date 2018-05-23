/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_EPOLL_H_INCLUDED_
#define _QQ_EPOLL_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


qq_int_t qq_epoll_init(void);
void qq_epoll_done(void);
qq_int_t qq_epoll_add_event(qq_event_t *ev, qq_int_t event);
qq_int_t qq_epoll_del_event(qq_event_t *ev, qq_int_t event, qq_uint_t flags);
qq_int_t qq_epoll_add_connection(qq_connection_t *c);
qq_int_t qq_epoll_del_connection(qq_connection_t *c, qq_uint_t flags);
qq_int_t qq_epoll_process_events(qq_msec_t timer, qq_uint_t flags);


#endif /* _QQ_EPOLL_H_INCLUDED_ */

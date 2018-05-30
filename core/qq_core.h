/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_CORE_H_INCLUDED_
#define _QQ_CORE_H_INCLUDED_


#include "qq_config.h"


typedef struct qq_cycle_s           qq_cycle_t;
typedef struct qq_pool_s            qq_pool_t;
typedef struct qq_event_s           qq_event_t;
typedef struct qq_connection_s      qq_connection_t;


typedef void (*qq_event_handler_pt)(qq_event_t *ev);
typedef void (*qq_connection_handler_pt)(qq_connection_t *c);


#define QQ_OK          0
#define QQ_ERROR      -1
#define QQ_AGAIN      -2
#define QQ_BUSY       -3
#define QQ_DONE       -4
#define QQ_DECLINED   -5
#define QQ_ABORT      -6

#define QQ_TRUE   1
#define QQ_FALSE  0


#include "qq_errno.h"
#include "qq_socket.h"
#include "qq_queue.h"
#include "qq_rbtree.h"
#include "qq_string.h"
#include "qq_log.h"
#include "qq_time.h"
#include "qq_process.h"
#include "qq_inet.h"
#include "qq_alloc.h"
#include "qq_palloc.h"
#include "qq_files.h"
#include "qq_event.h"
#include "qq_listening.h"
#include "qq_os.h"
#include "qq_connection.h"
#include "qq_tcp_send.h"
#include "qq_tcp_recv.h"
#include "qq_udp_send.h"
#include "qq_udp_recv.h"
#include "qq_cycle.h"
#include "qq_app.h"
#include "qq_event_posted.h"
#include "qq_event_accept.h"
#include "qq_event_timer.h"
#include "qq_epoll.h"


#define qq_abs(value)       (((value) >= 0) ? (value) : - (value))
#define qq_max(val1, val2)  ((val1 < val2) ? (val2) : (val1))
#define qq_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))


#endif /* _QQ_CORE_H_INCLUDED_ */

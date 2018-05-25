/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


/**************** log *******************/
#define QQ_HAVE_DEBUG    1
#define QQ_MAX_LOG_STR   256


/**************** network *******************/
#define QQ_LISTEN_BACKLOG     511
#define QQ_SOCK_REVBUF_SIZE   8192
#define QQ_SOCK_SNDBUF_SIZE   8192

#define QQ_HAVE_REUSEPORT             0

#define QQ_HAVE_KEEPALIVE             0
#define QQ_HAVE_KEEPALIVE_TUNABLE     0
#define QQ_TCP_KEEPIDLE               0
#define QQ_TCP_KEEPINTVL              0
#define QQ_TCP_KEEPCNT                0
#define QQ_KEEPALIVE_FACTOR           1000

#define QQ_HAVE_SETFIB                0
#define QQ_SETFIB                     0

#define QQ_HAVE_TCP_FASTOPEN          0
#define QQ_TCP_FASTOPEN               0

#define QQ_HAVE_DEFERRED_ACCEPT       0
#define QQ_HAVE_ACCEPTFILTER          0
#define QQ_HAVE_DEFERRED              0
#define QQ_HAVE_TCP_DEFER_ACCEPT      0
#define QQ_ACCEPT_FILTER_STR          ""

#define QQ_HAVE_IP_RECVDSTADDR        0
#define QQ_HAVE_IP_PKTINFO            0


#define QQ_CONNECTION_NUMBER  20000

#define QQ_EVENT_NUMBER       10000

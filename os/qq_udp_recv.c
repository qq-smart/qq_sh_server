/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


ssize_t
qq_udp_recv(qq_connection_t *c, u_char *buf, size_t size)
{
    ssize_t       n;
    qq_err_t      err;
    qq_event_t   *rev;

    rev = c->read;

    do {
        n = recv(c->fd, buf, size, 0);

        qq_log_debug("recv: fd:%d %d of %d", c->fd, n, size);

        if (n >= 0) {
            return n;
        }

        err = errno;

        if (err == QQ_EAGAIN || err == QQ_EINTR) {
            qq_log_debug("recv() not ready");
            n = QQ_AGAIN;
        } else {
            n = QQ_ERROR;
            break;
        }
    } while (err == QQ_EINTR);

    rev->ready = 0;

    if (n == QQ_ERROR) {
        rev->error = 1;
    }
	
    return n;
}

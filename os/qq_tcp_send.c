/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


ssize_t
qq_tcp_send(qq_connection_t *c, u_char *buf, size_t size)
{
    ssize_t       n;
    qq_err_t      err;
    qq_event_t   *wev;

    wev = c->write;

    for ( ;; ) {
        n = send(c->fd, buf, size, 0);
        qq_log_debug("send: fd:%d %z of %uz", c->fd, n, size);

        if (n > 0) {
            if (n < (ssize_t) size) {
                wev->ready = 0;
            }
            return n;
        }

        err = errno;

        if (n == 0) {
            qq_log_error(err, "send() returned zero");
            wev->ready = 0;
            return n;
        }

        if (err == QQ_EAGAIN || err == QQ_EINTR) {
            wev->ready = 0;

            qq_log_debug("send() not ready");

            if (err == QQ_EAGAIN) {
                return QQ_AGAIN;
            }
        } else {
            wev->error = 1;
            return QQ_ERROR;
        }
    }
}

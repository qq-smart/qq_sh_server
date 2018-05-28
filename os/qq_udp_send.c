/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


ssize_t
qq_udp_send(qq_connection_t *c, u_char *buf, size_t size)
{
    ssize_t       n;
    qq_err_t      err;
    qq_event_t   *wev;

    wev = c->write;

    for ( ;; ) {
        n = sendto(c->fd, buf, size, 0, c->sockaddr, c->socklen);

        qq_log_debug("sendto: fd:%d %z of %uz to \"%V\"",
                     c->fd, n, size, &c->addr_text);

        if (n >= 0) {
            if ((size_t) n != size) {
                wev->error = 1;
                return QQ_ERROR;
            }
            return n;
        }

        err = errno;

        if (err == QQ_EAGAIN) {
            wev->ready = 0;
            qq_log_debug("sendto() not ready");
            return QQ_AGAIN;
        }

        if (err != QQ_EINTR) {
            wev->error = 1;
            return QQ_ERROR;
        }
    }
}

/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"


ssize_t
qq_tcp_recv(qq_connection_t *c, u_char *buf, size_t size)
{
    ssize_t       n;
    qq_err_t      err;
    qq_event_t   *rev;

    rev = c->read;

#if (QQ_HAVE_EPOLLRDHUP)
    qq_log_debug("recv: eof:%d, avail:%d", rev->pending_eof, rev->available);
    if (!rev->available && !rev->pending_eof) {
        rev->ready = 0;
        return QQ_AGAIN;
    }
#endif

    do {
        n = recv(c->fd, buf, size, 0);
        qq_log_debug("recv: fd:%d n:%d of size:%d", c->fd, n, size);

        if (n == 0) {
            rev->ready = 0;
            rev->eof = 1;
            return 0;
        }

        if (n > 0) {
#if (QQ_HAVE_EPOLLRDHUP)
            if ((size_t) n < size) {
                if (!rev->pending_eof) {
                    rev->ready = 0;
                }
                rev->available = 0;
            }
            return n;
#endif

            if ((size_t) n < size) {
                rev->ready = 0;
            }
            return n;
        }

        err = errno;
        if (err == QQ_EAGAIN || err == QQ_EINTR) {
            qq_log_debug("recv() not ready");
            n = QQ_AGAIN;
        } else {
            qq_log_debug("recv() failed");
            n = QQ_ERROR;
            break;
        }
    } while (err == QQ_EINTR);

    rev->ready = 0;
    rev->error = 1;

    return n;
}

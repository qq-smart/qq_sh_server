/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#include "qq_config.h"
#include "qq_core.h"

#include "qq_video_audio_device.h"


static void qq_video_audio_device_process_request_handler(qq_connection_t *c);
static void qq_video_audio_device_write_event_handler(qq_event_t *ev);


qq_int_t
qq_video_audio_device_init(qq_cycle_t *cycle)
{
    if (qq_add_listening_config(SOCK_DGRAM,
        QQ_VIDEO_AUDIO_DEVICE_UDP_LISTENING_PORT,
        QQ_VIDEO_AUDIO_DEVICE_UDP_LISTENING_POOL_SIZE,
        qq_video_audio_device_process_request_handler) == QQ_ERROR)
    {
        qq_log_error(0, "qq_video_audio_device_init()->qq_add_listening_config() failed");
        return QQ_ERROR;
    }

    return QQ_OK;
}

void
qq_video_audio_device_done(void)
{
    qq_log_debug("qq_video_audio_device_done()");
}


static void
qq_video_audio_device_process_request_handler(qq_connection_t *c)
{
    qq_event_t   *rev, *wev;

    qq_log_debug("qq_video_audio_device_init_connection_handler()");

    c->send(c, c->buffer->pos, c->buffer->last - c->buffer->pos);
    c->buffer->last = c->buffer->pos;
}

static void
qq_video_audio_device_write_event_handler(qq_event_t *ev)
{
    qq_log_debug("qq_video_audio_device_write_event_handler()");
}

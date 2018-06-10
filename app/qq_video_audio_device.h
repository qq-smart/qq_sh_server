/*
 * Copyright (C) lilinwei
 * Copyright (C) qq_sh_server, Inc.
 */


#ifndef _QQ_VIDEO_AUDIO_DEVICE_H_INCLUDED_
#define _QQ_VIDEO_AUDIO_DEVICE_H_INCLUDED_


#include "qq_config.h"
#include "qq_core.h"


#define QQ_VIDEO_AUDIO_DEVICE_UDP_LISTENING_PORT       13000
#define QQ_VIDEO_AUDIO_DEVICE_UDP_LISTENING_POOL_SIZE  512


qq_int_t qq_video_audio_device_init(qq_cycle_t *cycle);
void qq_video_audio_device_done(void);


#endif /* _QQ_VIDEO_AUDIO_DEVICE_H_INCLUDED_ */

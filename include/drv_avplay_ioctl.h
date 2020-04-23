/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: avplay ioctl interface
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef __DRV_AVPLAY_IOCTL_H__
#define __DRV_AVPLAY_IOCTL_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    AVPLAY_CHANNEL_AUD = 0,
    AVPLAY_CHANNEL_VID,

    AVPLAY_CHANNEL_MAX
} avplay_channel_type;

typedef enum {
    AVPLAY_RESOURCE_TYPE_VID_DEMUX = 0,
    AVPLAY_RESOURCE_TYPE_AUD_DEMUX,
    AVPLAY_RESOURCE_TYPE_PCR_DEMUX,
    AVPLAY_RESOURCE_TYPE_VDEC,
    AVPLAY_RESOURCE_TYPE_WINDOW,
    AVPLAY_RESOURCE_TYPE_SYNC,

    AVPLAY_RESOURCE_TYPE_MAX
} avplay_resource_type;

typedef enum {
    AVPLAY_EVENT_VID_RENDER_UNDERLOAD = 0,  /* param: unload_cnt */

    AVPLAY_EVENT_VID_DMX_ERR,       /* param: HI_NULL */
    AVPLAY_EVENT_AUD_DMX_ERR,       /* param: HI_NULL */

    AVPLAY_EVENT_VID_ERR_FRAME,    /* param: hi_vdec_frm_type */
    AVPLAY_EVENT_VID_TYPE_ERR,      /* param: probeCodecType */
    AVPLAY_EVENT_VID_UNSUPPORT,     /* param: HI_NULL */

    AVPLAY_EVENT_VID_DECODE_FORMAT_CHANGE, /* param: HI_NULL */
    AVPLAY_EVENT_VID_DECODE_PACKING_CHANGE, /* param: HI_UNF_VIDEO_FRAME_PACKING_TYPE_E */

    AVPLAY_EVENT_VID_NEW_FRAME,     /* param: frame_cnt */
    AVPLAY_EVENT_VID_NEW_USER_DATA, /* param: HI_NULL */

    AVPLAY_EVENT_VID_RLS_PES,       /* param: pes_cnt */
    AVPLAY_EVENT_VID_NEW_PES,       /* param: pes_cnt */

    AVPLAY_EVENT_AUD_NEW_FRAME,     /* param: frame_cnt */
    AVPLAY_EVENT_AUD_NEW_PES,       /* param: pes_cnt */

    AVPLAY_EVENT_SYNC_PTS_JUMP,     /* param: HI_UNF_SYNC_PTSJUMP_PARAM_S */
    AVPLAY_EVENT_SYNC_STAT_CHANGE,  /* param: HI_UNF_SYNC_STAT_PARAM_S */

    AVPLAY_EVENT_VID_FIRST_PTS,     /* param: pts */
    AVPLAY_EVENT_VID_SECOND_PTS,    /* param: pts */

    AVPLAY_EVENT_VID_DMX_EOS,       /* param: HI_NULL */
    AVPLAY_EVENT_VID_DECODE_EOS,    /* param: HI_NULL */
    AVPLAY_EVENT_VID_RENDER_EOS,    /* param: HI_NULL */

    AVPLAY_EVENT_AUD_DMX_EOS,       /* param: HI_NULL */

    AVPLAY_EVENT_TYPE_MAX
} avplay_event_type;

typedef struct {
    hi_handle avplay;   /* out */
} avplay_create_param;

typedef struct {
    hi_handle avplay;               /* in */
    avplay_channel_type chn;           /* in */
} avplay_reset_param;

typedef struct {
    hi_handle avplay;               /* in */
    hi_handle handle;               /* in */
    avplay_resource_type type;    /* in */
    hi_u32    param;                /* in */
} avplay_resource_param;

typedef struct {
    hi_handle avplay;           /* in */
    hi_s64    timeout;          /* in */
    hi_u64    event_mask;       /* in */

    avplay_event_type event;  /* out */
    hi_u64              param;  /* out */
} avplay_wait_event_param;

typedef enum {
    IOC_AVPLAY_CREATE = 0,
    IOC_AVPLAY_DESTROY,
    IOC_AVPLAY_RESET,
    IOC_AVPLAY_SET_RESOURCE,
    IOC_AVPLAY_WAIT_EVENT,
    IOC_AVPLAY_WAKEUP
} ioc_avplay_type;

#define CMD_AVPLAY_CREATE               _IOWR(HI_ID_AVPLAY, IOC_AVPLAY_CREATE, avplay_create_param)
#define CMD_AVPLAY_DESTROY              _IOW(HI_ID_AVPLAY, IOC_AVPLAY_DESTROY, hi_handle)
#define CMD_AVPLAY_RESET                _IOW(HI_ID_AVPLAY, IOC_AVPLAY_RESET, avplay_reset_param)
#define CMD_AVPLAY_SET_RESOURCE         _IOW(HI_ID_AVPLAY, IOC_AVPLAY_SET_RESOURCE, avplay_resource_param)
#define CMD_AVPLAY_WAIT_EVENT           _IOWR(HI_ID_AVPLAY, IOC_AVPLAY_WAIT_EVENT, avplay_wait_event_param)
#define CMD_AVPLAY_WAKEUP               _IOW(HI_ID_AVPLAY, IOC_AVPLAY_WAKEUP, hi_handle)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

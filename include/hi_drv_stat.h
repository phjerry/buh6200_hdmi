/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2008-12-17
 */

#ifndef __HI_DRV_STAT_H__
#define __HI_DRV_STAT_H__

#include "hi_type.h"

#define HI_FATAL_STAT(fmt...) HI_FATAL_PRINT(HI_ID_STAT, fmt)
#define HI_ERR_STAT(fmt...)   HI_ERR_PRINT(HI_ID_STAT, fmt)
#define HI_WARN_STAT(fmt...)  HI_WARN_PRINT(HI_ID_STAT, fmt)
#define HI_INFO_STAT(fmt...)  HI_INFO_PRINT(HI_ID_STAT, fmt)
#define HI_DBG_STAT(fmt...)   HI_DBG_PRINT(HI_ID_STAT, fmt)

typedef enum {
    HI_STAT_EVENT_KEY_IN,
    HI_STAT_EVENT_KEY_OUT,
    HI_STAT_EVENT_AUD_STOP,
    HI_STAT_EVENT_VID_STOP,
    HI_STAT_EVENT_CONNECT,
    HI_STAT_EVENT_LOCKED,
    HI_STAT_EVENT_AUD_START,
    HI_STAT_EVENT_VID_START,
    HI_STAT_EVENT_CW_SET,
    HI_STAT_EVENT_STREAM_IN,
    HI_STAT_EVENT_ISTREAM_GET,
    HI_STAT_EVENT_IFRAME_OUT,
    HI_STAT_EVENT_VPSS_GET_FRM,
    HI_STAT_EVENT_VPSS_OUT_FRM,
    HI_STAT_EVENT_AVPLAY_GET_FRM,
    HI_STAT_EVENT_PRE_SYNC,
    HI_STAT_EVENT_BUF_READY,
    HI_STAT_EVENT_FRAME_SYNC_OK,
    HI_STAT_EVENT_VO_GET_FRM,
    HI_STAT_EVENT_IFRAME_INTER,
    HI_STAT_EVENT_MAX
} hi_stat_event;

typedef enum {
    HI_STAT_LD_EVENT_VI_FRM_IN = 0,
    HI_STAT_LD_EVENT_VI_FRM_OUT,
    HI_STAT_LD_EVENT_AVPLAY_DATA_IN,
    HI_STAT_LD_EVENT_SCD_DATA_IN,
    HI_STAT_LD_EVENT_VPSS_FRM_IN,
    HI_STAT_LD_EVENT_VPSS_FRM_OUT,
    HI_STAT_LD_EVENT_VDEC_FRM_IN,
    HI_STAT_LD_EVENT_VDEC_FRM_OUT,
    HI_STAT_LD_EVENT_VENC_FRM_IN,
    HI_STAT_LD_EVENT_VENC_FRM_OUT,
    HI_STAT_LD_EVENT_AVPLAY_FRM_IN,
    HI_STAT_LD_EVENT_AVPLAY_FRM_OUT,
    HI_STAT_LD_EVENT_VO_FRM_IN,
    HI_STAT_LD_EVENT_VO_FRM_OUT,
    HI_STAT_LD_EVENT_CAST_FRM_BEGIN,
    HI_STAT_LD_EVENT_CAST_FRM_OUT,
    HI_STAT_LD_EVENT_MAX,
} hi_stat_ld_event;

typedef enum {
    HI_STAT_ISR_AUDIO = 0x0,
    HI_STAT_ISR_VIDEO,
    HI_STAT_ISR_DEMUX,
    HI_STAT_ISR_SYNC,
    HI_STAT_ISR_VO,
    HI_STAT_ISR_TDE,
    HI_STAT_ISR_MAX
} hi_stat_isr;

typedef enum {
    HI_STAT_LD_SCENES_VID_PLAY = 0,
    HI_STAT_LD_SCENES_VID_CAP,
    HI_STAT_LD_SCENES_VID_CAST,
    HI_STAT_LD_SCENES_MAX,
} hi_stat_ld_scenes;

typedef struct {
    hi_stat_ld_event ld_event;
    hi_handle        handle;
    hi_u32           frame;
    hi_u32           time;
} hi_stat_ld_event_info; /* low delay event definition */

/* interrupt cost time in kernel-state */ /* CNcomment:内核态中断耗时统计 */
#if defined(HI_STAT_ISR_SUPPORTED)
hi_void hi_drv_stat_isr_reset(hi_void);
hi_void hi_drv_stat_isr_enable(hi_void);
hi_void hi_drv_stat_isr_disable(hi_void);
hi_void hi_drv_stat_isr_begin(hi_stat_isr isr);
hi_void hi_drv_stat_isr_end(hi_stat_isr isr);
#endif

hi_void hi_drv_stat_event(hi_stat_event event, hi_u32 value);

#define HI_STAT_MAX_EVENT_QUEUE_SIZE (10 + 1) /* ring queue need one more entry for diff empty and full status */
#define HI_STAT_LD_MAX_SCENES_NUM    (HI_STAT_LD_SCENES_MAX)
#define HI_STAT_LD_MAX_EVENT_NUM     (HI_STAT_LD_EVENT_MAX)

/* define scenes what's the composition of which events */
static const hi_stat_ld_event g_scenes_desc[HI_STAT_LD_MAX_SCENES_NUM][HI_STAT_LD_MAX_EVENT_NUM] __attribute__((unused)) = {
    /* HI_STAT_LD_SCENES_VID_PLAY: VDEC_IN -> VDEC_OUT ->VPSS_IN -> VPSS_OUT -> AVPLAY_IN-> AVPLAY_OUT -> VO_IN->VO_OUT */
#ifdef LOWDELAY_STAT_USE_PTS
    {HI_STAT_LD_EVENT_AVPLAY_DATA_IN, HI_STAT_LD_EVENT_SCD_DATA_IN, HI_STAT_LD_EVENT_VDEC_FRM_IN,
     HI_STAT_LD_EVENT_VDEC_FRM_OUT, HI_STAT_LD_EVENT_VPSS_FRM_IN,
     HI_STAT_LD_EVENT_VPSS_FRM_OUT, HI_STAT_LD_EVENT_AVPLAY_FRM_IN, HI_STAT_LD_EVENT_AVPLAY_FRM_OUT,
     HI_STAT_LD_EVENT_VO_FRM_IN, HI_STAT_LD_EVENT_VO_FRM_OUT, HI_STAT_LD_EVENT_MAX},
#else
    {HI_STAT_LD_EVENT_VDEC_FRM_IN, HI_STAT_LD_EVENT_VDEC_FRM_OUT, HI_STAT_LD_EVENT_VPSS_FRM_IN,
     HI_STAT_LD_EVENT_VPSS_FRM_OUT, HI_STAT_LD_EVENT_AVPLAY_FRM_IN, HI_STAT_LD_EVENT_AVPLAY_FRM_OUT,
     HI_STAT_LD_EVENT_VO_FRM_IN, HI_STAT_LD_EVENT_VO_FRM_OUT, HI_STAT_LD_EVENT_MAX},
#endif
    /* HI_STAT_LD_SCENES_VID_CAP: VI_IN -> VPSS_IN -> VPSS_OUT -> VI_OUT -> VENC_IN -> VENC_OUT */
    {HI_STAT_LD_EVENT_VI_FRM_IN, HI_STAT_LD_EVENT_VPSS_FRM_IN, HI_STAT_LD_EVENT_VPSS_FRM_OUT,
     HI_STAT_LD_EVENT_VI_FRM_OUT, HI_STAT_LD_EVENT_VENC_FRM_IN, HI_STAT_LD_EVENT_VENC_FRM_OUT, HI_STAT_LD_EVENT_MAX},
    /* HI_STAT_LD_SCENES_VID_CAST: DISP_OUT -> VENC_IN -> VENC_OUT */
    {HI_STAT_LD_EVENT_CAST_FRM_BEGIN, HI_STAT_LD_EVENT_CAST_FRM_OUT, HI_STAT_LD_EVENT_VENC_FRM_IN,
     HI_STAT_LD_EVENT_VENC_FRM_OUT, HI_STAT_LD_EVENT_MAX},
};

/* define event name */
static const hi_char *g_event_name[HI_STAT_LD_MAX_EVENT_NUM] __attribute__((unused)) = {
    [HI_STAT_LD_EVENT_VI_FRM_IN]      = "VI_IN",
    [HI_STAT_LD_EVENT_VI_FRM_OUT]     = "VI_OUT",
    [HI_STAT_LD_EVENT_AVPLAY_DATA_IN] = "AVPLAY_ES_IN",
    [HI_STAT_LD_EVENT_SCD_DATA_IN]    = "SCD_IN",
    [HI_STAT_LD_EVENT_VPSS_FRM_IN]    = "VPSS_IN",
    [HI_STAT_LD_EVENT_VPSS_FRM_OUT]   = "VPSS_OUT",
    [HI_STAT_LD_EVENT_VDEC_FRM_IN]    = "VDEC_IN",
    [HI_STAT_LD_EVENT_VDEC_FRM_OUT]   = "VDEC_OUT",
    [HI_STAT_LD_EVENT_VENC_FRM_IN]    = "VENC_IN",
    [HI_STAT_LD_EVENT_VENC_FRM_OUT]   = "VENC_OUT",
    [HI_STAT_LD_EVENT_AVPLAY_FRM_IN]  = "AVPLAY_IN",
    [HI_STAT_LD_EVENT_AVPLAY_FRM_OUT] = "AVPLAY_OUT",
    [HI_STAT_LD_EVENT_VO_FRM_IN]      = "VO_IN",
    [HI_STAT_LD_EVENT_VO_FRM_OUT]     = "VO_OUT",
    [HI_STAT_LD_EVENT_CAST_FRM_BEGIN] = "CAST_FRAME_BEGIN",
    [HI_STAT_LD_EVENT_CAST_FRM_OUT]   = "CAST_OUT",
};

hi_s32  hi_drv_stat_ld_start(hi_stat_ld_scenes scenes, hi_handle filter_handle);
hi_void hi_drv_stat_ld_stop(hi_void);
hi_void hi_drv_stat_ld_notify_event(hi_stat_ld_event_info *event_info);

#endif /* __HI_DRV_STAT_H__ */


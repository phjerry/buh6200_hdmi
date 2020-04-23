/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync ioctl interface
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef _DRV_SYNC_IOCTL_H_
#define _DRV_SYNC_IOCTL_H_

#include "hi_type.h"
#include "hi_drv_sync.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SYNC_INVALID_PTS               (0xffffffff)
#define SYNC_INVALID_TIME              (0xffffffff)
#define SYNC_MAX_NUM                   16

#if defined(CHIP_TYPE_hi3716mv430)
#define  SYNC_DEBUG_DETAIL
#endif

#define SYS_TIME_MAX                   0xFFFFFFFFU
#define PCR_TIME_MAX                   95443718

#define PTS_SERIES_COUNT               2
#define PCR_ABNORMAL_MAX_SERIES_COUNT  3
#define PCR_MAX_DELTA                  200
#define PCR_STC_DIFF_MAX_PERCENT       5

#define PCR_LEAD_ADJUST_THRESHOLD      200
#define PCR_LAG_ADJUST_THRESHOLD       200

#define PCR_LEAD_STOP_THRESHOLD        100
#define PCR_LAG_STOP_THRESHOLD         100

#define PCR_TIMEOUTMS                  3000
#define AUD_TIMEOUTMS                  200

#define PRE_SYNC_MIN_TIME              300
#define BUF_FUND_TIMEOUT               200

#define VID_LEAD_DISCARD_THRESHOLD     10000
#define VID_LAG_DISCARD_THRESHOLD      10000

#define PCR_SYNC_THRESHOLD             10000
#define PCR_SYNC_MAX_DELTA             5000
#define PCR_DELTA_ADJUST_STEP          20

#define AUD_RESYNC_TIMEOUT             2000  /* Temporarily reduce the timeout value
                                                 of audio resynchronization to 1000ms. 10000 */
#define AUD_RESYNC_ADJUST_THRESHOLD    10000 /* aud resync adjust threshold */

#define SYNC_AD_AUD_STOP_PLUS          200
#define SYNC_AD_AUD_STOP_NEGATIVE      (-200)
#define SYNC_AD_AUD_START_PLUS         300
#define SYNC_AD_AUD_START_NEGATIVE     (-300)
#define SYNC_AD_AUD_PTS_MAX_DIFF       3000

#define PCR_DISCARD_THRESHOLD          2000
#define PCR_ADJUST_THRESHOLD           500

#define SCR_DISCARD_THRESHOLD          10000

#define VID_PTS_GAP                    300
#define AUD_PTS_GAP                    200
#define AUD_SYNC_REPEAT_THRESHOLD      200

#define VID_SMOOTH_DISCARD_INTERVAL    2     /* discard one frame in every interval frame */
#define VID_SMOOTH_REPEAT_INTERVAL     2     /* repeat  one frame in every interval frames, including repeated frame */

#define SYNC_FRAME_VID_EMPTY_BUFNUM    4     /* vo bufnum waterline */
#define SYNC_ES_VID_EMPTY_PERCENT      2     /* vid es buf waterline */

#define SYNC_PTS_JUMP_FRM_NUM          2

#define PTS_LOOPBACK_TIMEOUT           2000
#define PTS_LOOPBACK_THRESHOLD         10000
#define MAX_REWIND_PTS                 1000

#define PCR_MAX_STEP                   (2700000 * 2)   /* pcr max step 0.2s */
#define PCR_STC_MAX_DELTA_DIFF         (6 * 25)        /* pcr stc delta max diff */
#define STC_MAX_PPM                    (20 * 2)              /* max PPM */
#define PCR_DELTA_FILTER_NUMBER        10
#define PPM_FILTER_NUMBER              40
#define PPM_ADJUST_THRESHOLD           10              /* stc precision is 0.1(PPM) */

#define SYNC_AUD_BUF_NORMAL_FRMNUM     (5)

#define VID_REPEAT_MAX_COUNT           (8)             /* vid repeat max conut */

#define SYNC_ADJUST_DELAY_CNT          (2)

#define SYNC_DBG_LIST_MAX_LEN          (600)

typedef  hi_s32 (*sync_manage_fn)(hi_handle *sync);

typedef struct {
    hi_handle  sync;
    sync_attr  attr;
} sync_create_param;

typedef struct {
    hi_handle   sync;
    sync_attr   attr;
} sync_drv_attr;

typedef struct {
    hi_handle sync;
    sync_chan_type chn;
} sync_drv_start_opt;

typedef struct {
    hi_handle sync;
    sync_chan_type chn;
} sync_drv_stop_opt;

typedef struct {
    hi_handle sync;
    hi_s64    seek_pts;
} sync_drv_seek_opt;

typedef struct {
    hi_handle       sync;
    sync_aud_info   info;
    sync_aud_opt    opt;
} sync_aud_judge_status;

typedef struct {
    hi_handle         sync;
    sync_vid_info     info;
    sync_vid_opt      opt;
} sync_vid_judge_status;

typedef struct {
    hi_handle       sync;
    sync_event_info evt;
} sync_drv_evt_info;

typedef struct {
    hi_handle            sync;
    sync_status_info     status;
} sync_drv_status;

typedef struct {
    hi_handle         sync;
    sync_presync_info info;
} sync_drv_presync_info;

typedef struct {
    hi_handle sync;
    hi_s64    pcr;
    hi_s64    stc;
} sync_drv_pcr_stc_info;

typedef struct {
    hi_handle       sync;
    sync_buf_status status;
} sync_drv_buf_status;

typedef struct {
    hi_handle      sync;
    sync_invoke_param param;
} sync_drv_invoke_param;

typedef enum {
    SYNC_IOC_CREATE = 0,
    SYNC_IOC_DESTROY,
    SYNC_IOC_SET_ATTR,
    SYNC_IOC_GET_ATTR,
    SYNC_IOC_CHECK_NUM,

    SYNC_IOC_START_SYNC,
    SYNC_IOC_STOP_SYNC,
    SYNC_IOC_AUD_JUDGE,
    SYNC_IOC_AD_AUD_JUDGE,
    SYNC_IOC_VID_JUDGE,
    SYNC_IOC_PLAY_SYNC,
    SYNC_IOC_PAUSE_SYNC,
    SYNC_IOC_RESUME_SYNC,
    SYNC_IOC_TPLAY_SYNC,
    SYNC_IOC_SEEK_SYNC,
    SYNC_IOC_NEW_EVENT,
    SYNC_IOC_GET_TIME,
    SYNC_IOC_GET_STATUS,
    SYNC_IOC_GET_PRESYNC_INFO,
    SYNC_IOC_GET_PPCR_STC_INFO,
    SYNC_IOC_SET_BUF_STATUS,
    SYNC_IOC_INVOKE,

    SYNC_IOC_SET_MAX
} sync_ioc_type;

#define CMD_SYNC_CREATE                _IOWR(HI_ID_SYNC, SYNC_IOC_CREATE, sync_create_param)
#define CMD_SYNC_DESTROY               _IOW(HI_ID_SYNC, SYNC_IOC_DESTROY, hi_handle)
#define CMD_SYNC_SET_ATTR              _IOW(HI_ID_SYNC, SYNC_IOC_SET_ATTR, sync_drv_attr)
#define CMD_SYNC_GET_ATTR              _IOWR(HI_ID_SYNC, SYNC_IOC_GET_ATTR, sync_drv_attr)
#define CMD_SYNC_CHECK_NUM             _IOWR(HI_ID_SYNC, SYNC_IOC_CHECK_NUM, hi_u32)

#define CMD_SYNC_START_SYNC            _IOW(HI_ID_SYNC, SYNC_IOC_START_SYNC, sync_drv_start_opt)
#define CMD_SYNC_STOP_SYNC             _IOW(HI_ID_SYNC, SYNC_IOC_STOP_SYNC, sync_drv_stop_opt)
#define CMD_SYNC_AUD_JUDGE             _IOWR(HI_ID_SYNC, SYNC_IOC_AUD_JUDGE, sync_aud_judge_status)
#define CMD_SYNC_AD_AUD_JUDGE          _IOWR(HI_ID_SYNC, SYNC_IOC_AD_AUD_JUDGE, sync_aud_judge_status)
#define CMD_SYNC_VID_JUDGE             _IOWR(HI_ID_SYNC, SYNC_IOC_VID_JUDGE, sync_vid_judge_status)
#define CMD_SYNC_PLAY_SYNC             _IOW(HI_ID_SYNC, SYNC_IOC_PLAY_SYNC, hi_handle)
#define CMD_SYNC_PAUSE_SYNC            _IOW(HI_ID_SYNC, SYNC_IOC_PAUSE_SYNC, hi_handle)
#define CMD_SYNC_RESUME_SYNC           _IOW(HI_ID_SYNC, SYNC_IOC_RESUME_SYNC, hi_handle)
#define CMD_SYNC_TPLAY_SYNC            _IOW(HI_ID_SYNC, SYNC_IOC_TPLAY_SYNC, hi_handle)
#define CMD_SYNC_SEEK_SYNC             _IOW(HI_ID_SYNC, SYNC_IOC_SEEK_SYNC, sync_drv_seek_opt)

#define CMD_SYNC_GET_NEW_EVENT         _IOWR(HI_ID_SYNC, SYNC_IOC_NEW_EVENT, sync_drv_evt_info)
#define CMD_SYNC_GET_STATUS            _IOWR(HI_ID_SYNC, SYNC_IOC_GET_STATUS, sync_drv_status)
#define CMD_SYNC_GET_PRESYNC_INFO      _IOWR(HI_ID_SYNC, SYNC_IOC_GET_PRESYNC_INFO, sync_drv_status)
#define CMD_SYNC_GET_PCR_STC_INFO      _IOWR(HI_ID_SYNC, SYNC_IOC_GET_PPCR_STC_INFO, sync_drv_pcr_stc_info)
#define CMD_SYNC_SET_BUF_STATUS        _IOWR(HI_ID_SYNC, SYNC_IOC_SET_BUF_STATUS, sync_drv_buf_status)
#define CMD_SYNC_INVOKE                _IOWR(HI_ID_SYNC, SYNC_IOC_INVOKE, sync_drv_invoke_param)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


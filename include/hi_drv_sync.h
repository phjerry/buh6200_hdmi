/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync kernel interface declaration
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef __HI_DRV_SYNC_H__
#define __HI_DRV_SYNC_H__

#ifdef HI_VIDEO_TB_SUPPORT
#include "hi_drv_video.h"
#endif
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif

#define HI_ERR_VSYNC(chn, fmt...)\
        if (ctx->is_print)\
        {\
            if (SYNC_CHAN_VID == chn)\
            {\
                HI_ERR_PRINT(HI_ID_VSYNC, fmt);\
            }\
        }  \

#define HI_ERR_ASYNC(chn, fmt...)\
        if (ctx->is_print)\
        {\
            if (SYNC_CHAN_AUD == chn)\
            {\
                HI_ERR_PRINT(HI_ID_ASYNC, fmt);\
            }\
        } \

#define HI_INFO_VSYNC(chn, fmt...)\
        if (ctx->is_print)\
        {\
            if (SYNC_CHAN_VID == chn)\
            {\
                HI_INFO_PRINT(HI_ID_VSYNC, fmt);\
            }\
        }  \

#define HI_INFO_ASYNC(chn, fmt...)\
        if (ctx->is_print)\
        {\
            if (SYNC_CHAN_AUD == chn)\
            {\
                HI_INFO_PRINT(HI_ID_ASYNC, fmt);\
            }\
        } \

/* ref to hi_unf_sync_pts_chan */
typedef enum {
    SYNC_CHAN_VID,
    SYNC_CHAN_AUD,
    SYNC_CHAN_PCR,
    SYNC_CHAN_SCR,
    SYNC_CHAN_EXT,
    SYNC_CHAN_MAX
} sync_chan_type;

/* ref to hi_unf_sync_ref_mode */
typedef enum {
    SYNC_REF_NONE = 0,
    SYNC_REF_AUDIO,
    SYNC_REF_VIDEO,
    SYNC_REF_PCR,
    SYNC_REF_SCR,

    SYNC_REF_MAX
} sync_ref_type;

typedef enum {
    SYNC_STATUS_STOP = 0,
    SYNC_STATUS_PLAY,
    SYNC_STATUS_TPLAY,
    SYNC_STATUS_PAUSE,
    SYNC_STATUS_MAX
} sync_status;

typedef enum {
    SYNC_PROC_DISCARD,
    SYNC_PROC_REPEAT,
    SYNC_PROC_PLAY,
    SYNC_PROC_QUICKOUTPUT,
    SYNC_PROC_TPLAY,
    SYNC_PROC_CONTINUE,
    SYNC_PROC_BLOCK,
    SYNC_PROC_MAX
} sync_proc_mode;

typedef enum {
    SYNC_AUD_SPEED_ADJUST_NORMAL,
    SYNC_AUD_SPEED_ADJUST_UP,
    SYNC_AUD_SPEED_ADJUST_DOWN,
    SYNC_AUD_SPEED_ADJUST_MUTE_REPEAT,
    SYNC_AUD_SPEED_ADJUST_MAX
} sync_aud_speed_adjust_mode;

typedef enum {
    SYNC_BUF_STATE_EMPTY = 0,   /**< The buffer is idle. */
    SYNC_BUF_STATE_LOW,         /**< The buffer usage is too low. */
    SYNC_BUF_STATE_NORMAL,      /**< The buffer works normally. */
    SYNC_BUF_STATE_HIGH,        /**< The buffer usage is too high. */
    SYNC_BUF_STATE_FULL,        /**< The buffer is full. */
    SYNC_BUF_STATE_MAX
} sync_buf_state_mode;

typedef enum {
    SYNC_INVOKE_SET_VID_FIRST_PTS = 0,  /* param: hi_s64 pts, in us */
    SYNC_INVOKE_SET_VID_SECOND_PTS,     /* param: hi_s64 pts, in us */
    SYNC_INVOKE_SET_VID_STOP_REGIN,     /* param: hi_bool use_stop_regin */

    SYNC_INVOKE_SET_MS12_FLAG,          /* param: hi_bool is_ms12_mode */
    SYNC_INVOKE_SET_DDP_TEST_FLAG,      /* param: hi_bool is_ddp_test */
    SYNC_INVOKE_SET_AUD_REWIND_FLAG,    /* param: hi_bool is_aud_rewind */
    SYNC_INVOKE_GET_PCR_REWIND_FLAG,    /* param: hi_bool is_pcr_rewind */

    SYNC_INVOKE_SET_SYNC_FINISH_FLAG,   /* param: hi_bool is_sync_finsih */
    SYNC_INVOKE_SET_EXTERNAL_PLL_FLAG,  /* param: hi_bool use_external_pll */
    SYNC_INVOKE_SET_STC_CALIBRATION,    /* param: hi_s32 ppm */
} sync_invoke_id;

/* ref to HI_UNF_SYNC_REGION_S */
typedef struct {
    hi_s32  vid_plus_time;
    hi_s32  vid_negative_time;
    hi_bool smooth_play;
} sync_region_info;

/* ref to HI_UNF_SYNC_ATTR_S */
typedef struct {
    sync_ref_type       sync_ref;
    sync_region_info    start_region;
    sync_region_info    novel_region;

    hi_s64              vid_pts_adjust;
    hi_s64              aud_pts_adjust;
    hi_u32              pre_sync_timeout;
    hi_bool             quick_output;
} sync_attr;

/* ref to HI_UNF_SYNC_STATUS_S */
typedef struct {
    hi_s64 first_aud_pts;
    hi_s64 first_vid_pts;
    hi_s64 last_aud_pts;
    hi_s64 last_vid_pts;
    hi_s64 vid_aud_diff;
    hi_s64 play_time;
    hi_s64 local_time;
} sync_status_info;

/* ref to HI_UNF_SYNC_STAT_PARAM_S */
typedef struct {
    hi_s64 vid_aud_diff;
    hi_s64 vid_pcr_diff;
    hi_s64 aud_pcr_diff;
    hi_s64 vid_local_time;
    hi_s64 aud_local_time;
    hi_s64 pcr_local_time;
} sync_stat_param;

/* ref to HI_UNF_SYNC_PTSJUMP_PARAM_S */
typedef struct {
    sync_chan_type  pts_chan;
    hi_bool loop_back;
    hi_s64  first_pts;
    hi_s64  first_valid_pts;
    hi_s64  cur_src_pts;
    hi_s64  cur_pts;
    hi_s64  last_src_pts;
    hi_s64  last_pts;
} sync_pts_jump_param;

typedef struct {
    hi_s64   src_pts;      /* original pts */
    hi_s64   pts;          /* amendatory pts */
    hi_bool  is_pts_valid;
    hi_s64   frame_time;    /* duration time of one video frame */
    hi_s64   disp_time;
    hi_bool  is_progressive;
    hi_s64   delay_time;    /* delay time from sync judge to display */
    hi_s64   disp_rate;
} sync_vid_info;

typedef struct {
    sync_proc_mode            proc;
    hi_u32                    repeat;
    hi_u32                    discard;
    hi_u32                    vdec_discard_time;
#ifdef HI_VIDEO_TB_SUPPORT
    hi_drv_video_tb_adjust    tb_adjust_type;
#endif
} sync_vid_opt;

typedef struct {
    hi_s64   src_pts;            /* original pts */
    hi_s64   pts;                /* amendatory pts */
    hi_bool  is_pts_valid;
    hi_s64   frame_time;         /* duration time of one audio frame */
    hi_s64   buf_time;           /* duration time in ao buffer */
    hi_u32   frame_num;          /* audio frame number in adec buffer */
    hi_bool  is_aud_tplay;       /* audio tplay */
} sync_aud_info;

typedef struct {
    sync_proc_mode               proc;
    sync_aud_speed_adjust_mode   speed_adjust;
} sync_aud_opt;

/* sync event struct */
typedef struct {
    hi_bool             is_vid_pts_jump;
    hi_bool             is_aud_pts_jump;
    hi_bool             is_stat_change;
    sync_pts_jump_param vid_pts_jump_param;
    sync_pts_jump_param aud_pts_jump_param;
    sync_stat_param     stat_param;
} sync_event_info;

typedef struct {
    hi_u32              vid_buf_percent;
    hi_u32              aud_buf_percent;
    sync_buf_state_mode vid_buf_state;
    sync_buf_state_mode aud_buf_state;

    hi_s64              aud_buf_time;
    hi_s64              vid_buf_time;
    hi_s64              buf_time_max;
    hi_s64              buf_time_min;
    hi_s64              buf_time_normal;
    hi_bool             is_buf_time_need_ctrl;
    hi_bool             is_overflow_disc_frm;
} sync_buf_status;

typedef struct {
    hi_s64              target_pts;         /* amendatory pts */
    hi_bool             is_presync_target_init;
    hi_bool             is_presync_finish;
    hi_bool             is_sync_finish;
    sync_chan_type      presync_target;
} sync_presync_info;

typedef struct {
    sync_invoke_id invoke_id;
    hi_s64         vid_first_dec_pts;
    hi_s64         vid_second_dec_pts;
    hi_bool        is_use_stop_region;
    hi_bool        is_ms12_mode;
    hi_bool        is_ddp_test;
    hi_bool        is_aud_rewind;
    hi_bool        is_pcr_rewind;
    hi_bool        is_sync_finsih;
    hi_bool        use_external_pll;
    hi_s32         ppm;
} sync_invoke_param;

//#ifdef HI_MCE_SUPPORT
hi_s32 hi_drv_sync_init(void);
hi_s32 hi_drv_sync_deinit(void);
hi_s32 hi_drv_sync_create(sync_attr *attr, hi_handle *sync, const void *unique);
hi_s32 hi_drv_sync_destroy(hi_handle sync);

hi_s32 hi_drv_sync_attr_get(hi_handle sync, sync_attr *attr);
hi_s32 hi_drv_sync_attr_set(hi_handle sync, const sync_attr *attr);
hi_s32 hi_drv_sync_check_num(hi_u32 *num, const void *unique);
hi_s32 hi_drv_sync_start(hi_handle sync, sync_chan_type chn);
hi_s32 hi_drv_sync_stop(hi_handle sync, sync_chan_type chn);

hi_s32 hi_drv_sync_play(hi_handle sync);
hi_s32 hi_drv_sync_seek(hi_handle sync, hi_s64 pts);
hi_s32 hi_drv_sync_pause(hi_handle sync);
hi_s32 hi_drv_sync_resume(hi_handle sync);
hi_s32 hi_drv_sync_tplay(hi_handle sync);

hi_s32 hi_drv_sync_aud_judge(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt);
hi_s32 hi_drv_sync_ad_aud_judge(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt);
hi_s32 hi_drv_sync_vid_judge(hi_handle sync, sync_vid_info *info, sync_vid_opt *opt);
hi_s32 hi_drv_sync_pcr_proc(hi_handle sync, hi_s64 pcr, hi_s64 stc);
hi_s32 hi_drv_sync_get_new_event(hi_handle sync, sync_event_info *info);
hi_s32 hi_drv_sync_get_status(hi_handle sync, sync_status_info *status);
hi_s32 hi_drv_sync_get_presync_info(hi_handle sync, sync_presync_info *info);
hi_s32 hi_drv_sync_get_pcr_stc_val(hi_handle sync, hi_s64 *pcr, hi_s64 *stc);
hi_s32 hi_drv_sync_set_buf_status(hi_handle sync, const sync_buf_status *status);

hi_s32 hi_drv_sync_invoke(hi_handle sync, sync_invoke_param* invoke);

//#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

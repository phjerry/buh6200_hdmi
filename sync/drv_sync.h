/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drive sync interface declaration
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef __DRV_SYNC_H__
#define __DRV_SYNC_H__

#include "linux/hisilicon/securec.h"
#include "hi_osal.h"
#include "drv_sync_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define sync_abs(x) abs(x)

typedef enum {
    SYNC_DBG_INFO_SYS_INIT,
    SYNC_DBG_INFO_SYS_DEINIT,
    SYNC_DBG_INFO_LIST_INITIAL,
    SYNC_DBG_INFO_LIST_SIZE,
    SYNC_DBG_INFO_LIST_RELEASE,
    SYNC_DBG_INFO_LIST_VIDADD,
    SYNC_DBG_INFO_MAX
} sync_dbg_info_mode;

typedef enum {
    SYNC_PCR_ADJUST_SCR,
    SYNC_AUD_ADJUST_SCR,
    SYNC_SCR_ADJUST_MAX
} sync_pcr_adjust_type;

typedef struct {
    hi_s32    sum;         /* ppm sum */
    hi_s32    num;         /* ppm number */
    hi_s32    valid;       /* ppm valid = ppm sum / ppm num */
} sync_ppm_info;

typedef struct {
    hi_u64     stc_last_val;         /* last stc value 27M */
    hi_u32     stc_last;             /* last stc value ms */
    hi_s32     stc_delta_sum;        /* stc  delta sum value */
    hi_bool    is_stc_adjust_fail;
    hi_bool    is_stc_adjust_init;
} sync_stc_info;

typedef struct {
    hi_bool                    is_pcr_first_come;       /* arrive flag of the first pcr */
    hi_bool                    is_pcr_aud_sync_ok;
    hi_bool                    is_pcr_vid_sync_ok;
    hi_bool                    is_pcr_adjust_delta_ok;
    hi_u32                     pcr_first_sys_time;      /* arrive time of the first pcr */
    hi_u32                     pcr_first;               /* first pcr value */
    hi_u32                     pcr_last;                /* last  pcr value ms */
    hi_u32                     pcr_last_valid;
    hi_bool                    is_pcr_valid;
    hi_u32                     pcr_last_sys_time;       /* last system time of setting pcr localtime */
    hi_u32                     pcr_last_local_time;     /* last pcr localtime */
    hi_u32                     pcr_pause_local_time;    /* pcr localtime when pause */
    hi_u32                     pcr_series_cnt;          /* pcr successive count */
    hi_u32                     pcr_lead_series_cnt;
    hi_u32                     pcr_lag_series_cnt;
    hi_u32                     pcr_abnormal_series_cnt;
    hi_u32                     pcr_sync_start_sys_time;  /* start time of pcr synchronization,set when sync start */
    hi_bool                    is_pcr_local_time;        /* valid flag of pcr localtime */
    hi_bool                    is_pcr_adjust_delta;
    sync_pcr_adjust_type       pcr_adjust_type;          /* which way to adjust scr */
    hi_s32                     pcr_delta;                /* adjust pcr value */

    hi_s32                     aud_pcr_diff;        /* difference value between audio localtime and pcr localtime */
    hi_s32                     vid_pcr_diff;        /* difference value between video localtime and pcr localtime */
    hi_s32                     last_vid_pcr_diff;
    hi_s32                     last_aud_pcr_diff;

    hi_bool                    is_pcr_loop_rewind;
    hi_u32                     pcr_gradient;        /* gradient of pcr and system time */
                                                    /**< CNcomment: PCRo¨ª?¦Ì¨ª3¨º¡À??¡À¨¨¨¤yD¡À?¨º(*100) */

    sync_ppm_info              ppm_info;            /* PPM calculate */
    sync_stc_info              stc_info;

    hi_s32                     pcr_delta_sum;
    hi_s32                     pcr_delta_num;
    hi_u64                     pcr_last_val;        /* last  pcr value */
    hi_bool                    is_stc_adjust_refer;
} sync_pcr_info;

/* sync region status enum */
typedef enum {
    SYNC_REGION_STAT_IN_STOP,
    SYNC_REGION_STAT_IN_START,
    SYNC_REGION_STAT_IN_NOVEL,
    SYNC_REGION_STAT_IN_DISCARD,
    SYNC_REGION_STAT_OUT_DISCARD,
    SYNC_REGION_STAT_MAX
} sync_region_stat_mode;

typedef struct {
    hi_u32              aud_src_pts;
    hi_u32              aud_pts;
    hi_u32              aud_local_time;
    hi_u32              aud_buf_time;
    hi_u32              aud_frame_time;

    hi_u32              vid_src_pts;
    hi_u32              vid_pts;
    hi_u32              vid_local_time;
    hi_u32              vid_delay_time;
    hi_u32              vid_frame_time;
    hi_u32              vid_aud_diff;
    hi_u32              disp_time;

    hi_u32              pcr_last;
    hi_u32              pcr_local_time;
    hi_u32              pcr_delta_time;
    hi_u32              vid_pcr_diff;
    hi_u32              aud_pcr_diff;

    hi_u32              system_time;
    hi_u32              idx;
    sync_proc_mode      proc_type;
    struct list_head    list;
} sync_dbg_info_status;

typedef struct {
    sync_attr                  attr;             /* sync attributes set by user */

    hi_bool                    is_vid_enable;    /* video enable flag, set when start video */
    hi_bool                    is_aud_enable;    /* audio enable flag, set when start audio */
    hi_bool                    is_ddp_mode;      /* for DDP test only */

    /* AV common information. reset only when both of av are stoped */
    sync_status                cur_status;       /* current status */
    sync_buf_status            cur_buf_status;   /* current buf status */

    sync_event_info            evt;
    hi_u32                     loop_rewind_time;
    hi_bool                    is_loop_rewind;

    hi_u32                     vid_first_dec_pts;
    hi_u32                     vid_second_dec_pts;

    hi_u32                     presync_start_sys_time;  /* start time of presynchronization,set when presync start */
    hi_u32                     presync_end_sys_time;    /* end time of presynchronization,set when presync end */
    hi_bool                    is_presync_finish;       /* finish flag of presynchronization */
    sync_chan_type             presync_target;          /* presynchronization target */
    hi_u32                     presync_target_time;     /* target time of presynchronization */
    hi_bool                    is_presync_target_init;  /* initialize flag of presynchronization target */
    hi_bool                    is_buf_fund_finish;      /* finish flag of audio and video data cumulation */
    hi_u32                     buf_fund_end_sys_time;   /* end time of audio and video data cumulation  */
    hi_u32                     ext_presync_target_time;
    hi_bool                    is_use_ext_presync_target;
    sync_pcr_info              pcr_sync_info;  /* some information while sync reference set HI_UNF_SYNC_REF_MODE_PCR */

    /* video statistics, reset when stop */
    hi_bool                    is_vid_first_come;      /* arrive flag of the first video frame */
    hi_u32                     vid_first_sys_time;     /* arrive time of the first video frame */
    hi_u32                     vid_first_pts;          /* PTS of the first video frame */
    hi_bool                    is_vid_first_valid_come;
    hi_u32                     vid_first_valid_pts;
    hi_u32                     vid_last_pts;           /* PTS of the last video frame */
    hi_u32                     vid_last_src_pts;
    hi_u32                     vid_last_valid_pts;
    hi_bool                    is_vid_presync_target_init; /* initialize flag of video presync target */
    hi_u32                     vid_presync_target_time;    /* video presync target time */
    hi_bool                    is_vid_local_time;          /* valid flag of video localtime */
    hi_u32                     vid_last_sys_time;          /* last system time of setting video localtime */
    hi_u32                     vid_last_local_time;        /* last video localtime */
    hi_u32                     vid_pause_local_time;       /* video localtime when pause */
    hi_u32                     vid_pts_series_cnt;         /* video pts successive count */
    hi_bool                    is_vid_sync_adjust;         /* adjust flag */
    /* TODO: sdk  the usage of the  following three members */
    hi_u32                     vid_discard_play_cnt;       /* video play count when discard */
    hi_u32                     vid_discard_cnt;            /* video discard count */
    hi_u32                     vid_repeat_play_cnt;        /* video play count when repeat */
    hi_u32                     vid_repeat_cnt;             /* video repeat count */
    sync_vid_info              vid_info;                   /* video channel information,set by vo and used by sync */
    sync_vid_opt               vid_opt;                    /* video adjust mode,set by sync */
    hi_bool                    is_vid_first_play;          /* played flag of the first video frame */
    hi_u32                     vid_first_play_time;        /* time of playing the first video frame */
    hi_bool                    is_vid_normal_play;
    hi_s32                     vid_aud_diff;        /* difference value between video localtime and audio localtime */
    hi_s32                     last_vid_aud_diff;
    hi_bool                    is_vid_pts_loop_rewind;

    /* audio statistics,reset when stop */
    hi_bool                    is_aud_first_come;         /* arrive flag of the first audio frame */
    hi_u32                     aud_first_sys_time;        /* arrive time of the first audio frame */
    hi_u32                     aud_first_pts;             /* pts of the first audio frame */
    hi_bool                    is_aud_first_valid_come;
    hi_u32                     aud_first_valid_pts;
    hi_u32                     aud_last_pts;              /* pts of the last audio frame */
    hi_u32                     aud_last_src_pts;
    hi_u32                     aud_last_valid_pts;
    hi_u32                     aud_last_buf_time;          /* buftime value when last audio pts arrived */
    hi_bool                    is_aud_presync_target_init; /* initialize flag of audio presync target */
    hi_u32                     aud_presync_target_time;    /* audio presync target time */
    hi_bool                    is_aud_local_time;          /* valid flag of audio localtime */
    hi_u32                     aud_last_sys_time;          /* last system time of setting audio localtime */
    hi_u32                     aud_last_local_time;        /* last audio localtime */
    hi_u32                     aud_pause_local_time;       /* audio localtime when pause */
    hi_u32                     aud_pts_series_cnt;         /* audio pts successive count */
    hi_bool                    is_aud_resync;              /* audio resync flag */
    hi_bool                    is_aud_rebuf_fund;          /* audio recumulate flag */
    hi_u32                     aud_play_cnt;               /* audio play count */
    hi_u32                     aud_repeat_cnt;             /* audio repeat count */
    hi_u32                     aud_discard_cnt;            /* audio discard count */
    sync_aud_info              aud_info;                   /* audio channel information,set by adec and used by sync */
    sync_aud_opt               aud_opt;                    /* audio adjust mode,set by sync */
    sync_aud_info              ad_aud_info;                /* audio channel information,set by adec and used by sync */
    sync_aud_opt               ad_aud_opt;                 /* audio adjust mode,set by sync */
    hi_bool                    is_aud_first_play;          /* played flag of the first audio frame */
    hi_u32                     aud_first_play_time;        /* time of playing the first audio frame */
    hi_bool                    is_aud_pts_loop_rewind;

    hi_u32                     scr_first_sys_time;      /* last scr system time */
    hi_u32                     scr_first_local_time;    /* last scr local time */
    hi_u32                     scr_last_sys_time;       /* last scr system time */
    hi_u32                     scr_last_local_time;     /* last scr local time */
    hi_u32                     scr_pause_local_time;    /* scr localtime when pause */
    hi_s32                     aud_scr_diff;            /* difference time between audio localtime and scr localtime */
    hi_s32                     vid_scr_diff;            /* difference time between vidio localtime and scr localtime */
    hi_bool                    is_scr_init;             /* scr be inited or not */

    hi_bool                    is_print;

    hi_bool                    is_use_stop_region;      /* use stop region or not */
    hi_bool                    is_first_sync_state_report;

    hi_bool                    is_pcr_stc_changed;     /* pcr and stc changed or not */
    hi_bool                    is_ext_pll_adjust;      /* use external pll adjusting method or not */
    hi_s32                     external_ppm;           /* External PPM config by user */
    hi_u64                     cur_pcr_val;            /* current pcr value */
    hi_u64                     cur_stc_val;            /* current stc value */

    hi_u32                     aud_fund_begin_time;
    hi_bool                    is_ms12;
    hi_bool                    is_pcr_rewind;
    hi_bool                    is_vid_rewind;
    hi_bool                    is_aud_rewind;
    hi_u32                     loop_rewind_start_time;

    hi_u32                     sync_adjust_delay_frm_cnt;
    hi_bool                    is_sync_finish;         /* finish flag of synchronization */

    hi_u32                     sync_finish_continue_cnt;
    hi_u32                     sync_novel_continue_cnt;
    hi_u32                     aud_loop_rewind_pts;
    hi_u32                     vid_loop_rewind_pts;

    hi_bool                    is_show_dbg_detail;
    hi_u32                     dbg_list_node_idx;
    hi_u32                     dbg_list_node_cnt;
    hi_u32                     dbg_list_max_len;

    hi_bool                    is_buf_adjust;
#ifdef SYNC_DEBUG_DETAIL
    struct list_head           dbg_info_list_head;
#endif
} sync_context;

typedef struct {
    sync_context  *ctx;          /* SYNC kernel space pointer */
    const void    *unique;       /* SYNC unique id */
} sync_state_info;

typedef struct {
    sync_state_info   info[SYNC_MAX_NUM];
    hi_u32            cnt;
    sync_manage_fn    add_ins;
    sync_manage_fn    del_ins;
} sync_global_state;

hi_bool sync_handle_verify(hi_handle sync);
hi_s32  sync_start(hi_handle sync);
hi_s32  sync_pause(hi_u32 id);
hi_s32  sync_resume(hi_u32 id);
hi_s32  sync_get_time(hi_u32 id, hi_u32 *local_time, hi_u32 *play_time);
hi_u32  sync_get_local_time(sync_context *ctx, sync_chan_type chn);
hi_u32  sync_get_sys_time(void);
void sync_vid_proc(hi_handle sync, sync_vid_info *info, sync_vid_opt *opt);
void sync_aud_proc(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt);
void sync_ad_aud_proc(hi_handle sync, sync_aud_info *info, sync_aud_opt *opt);
void sync_pcr_proc(hi_handle sync, hi_u64 pcr, hi_u64 stc);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef __DRV_WINDOW_H__
#define __DRV_WINDOW_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    FRAME_SYNC_PLAY = 0,
    FRAME_SYNC_REPEAT,
    FRAME_SYNC_DISCARD,
    FRAME_SYNC_MAX
} win_framesync_result;

typedef struct {
    hi_drv_video_frame *config_frame;
    hi_drv_video_frame *displaying_frame;

    /* in and out frame count. */
    hi_u32 aquire_frame_cnt;
    hi_u32 rls_frame_cnt;

    /* queue time  interval statistics */
    hi_u32 last_queue_time;
    hi_u32 current_queue_time;
    hi_u32 queue_interval;
    hi_u32 max_queue_interval;

    /* about fence. */
    hi_u32 src_fence_timeout_cnts;
    hi_u32 src_fence_timeout_fd;
    hi_u32 src_fence_destroy_cnts;
    hi_u32 src_fence_access_cnts;
    hi_u32 src_fence_rls_cnts;
    hi_u32 src_rls_atonce_cnts;
    hi_u32 sink_fence_create_cnts;
    hi_u32 sink_fence_signal_cnts;

    /* frame duration time statistics */
    hi_u32 min_frame_stay_time;
    hi_u32 max_frame_stay_time;
    hi_u32 newest_frame_stay_time;

    /* config statistics */
    hi_u32 err_frame_counts;

    hi_u32 tbfield_notmatch_count;
    hi_bool is_tunnel_fence;
    hi_bool is_android_deque_fence;
    hi_bool is_next_frm;
    hi_bool is_stepmode;
    hi_bool is_vpss_bypass_mode;

    /* tplay speed setting. */
    hi_u32 tplay_speed_integer;
    hi_u32 tplay_speed_decimal;
} win_proc_play_status;

typedef struct {
} win_proc_instance_status;

hi_s32  win_init(hi_void);
hi_void win_deinit(hi_void);
hi_s32 drv_win_destroy_window(hi_handle win_handle);
hi_s32 drv_win_create_window(hi_drv_win_attr *win_attr,hi_handle *win_handle);
hi_s32 drv_win_freeze(hi_handle win_handle, hi_drv_win_freeze_mode freeze_mode);
hi_s32 drv_win_get_freeze_status(hi_handle win_handle, hi_drv_win_freeze_mode *freeze_mode);
hi_s32 drv_win_set_quickout(hi_handle win_handle, hi_bool quickout_enable);
hi_s32 drv_win_get_quickout_status(hi_handle win_handle, hi_bool *quickout_enable);
hi_s32 drv_win_pause(hi_handle win_handle, hi_bool pause_enable);
hi_s32 drv_win_get_pause_status(hi_handle win_handle, hi_bool *pause_enable);

hi_s32 drv_win_set_enable(hi_handle win_handle, hi_bool is_enable);
hi_s32 drv_win_get_enable(hi_handle win_handle, hi_bool *is_enable);
hi_s32 drv_win_set_flip(hi_handle win_handle, hi_bool flip_hori, hi_bool flip_vert);
hi_s32 drv_win_get_flip(hi_handle win_handle, hi_bool *flip_hori, hi_bool *flip_vert);
hi_s32 drv_win_set_rotation(hi_handle win_handle, hi_drv_rot_angle rotation);
hi_s32 drv_win_get_rotation(hi_handle win_handle, hi_drv_rot_angle *rotation);
hi_s32 drv_win_set_zorder(hi_handle win_handle, hi_drv_disp_zorder zflag);
hi_s32 drv_win_get_zorder(hi_handle win_handle, hi_u32 *zorder);
hi_s32 drv_win_set_source(hi_handle win_handle, hi_handle source_handle);
hi_s32 drv_win_set_attr(hi_handle win_handle, hi_drv_win_attr *win_attr);
hi_s32 drv_win_get_attr(hi_handle win_handle, hi_drv_win_attr *win_attr);
hi_s32 drv_win_reset(hi_handle win_handle, hi_drv_win_reset_mode reset_mode);
hi_s32 drv_win_dequeue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 drv_win_queue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info, hi_s32 *fence_fd);
hi_s32 drv_win_aquire_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 drv_win_release_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 drv_win_acquire_captured_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 drv_win_release_captured_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);

hi_s32 drv_win_transfer_dma_to_fd(hi_drv_video_frame *frame_info, hi_bool is_producer);
hi_s32 drv_win_transfer_fd_to_dma(hi_drv_video_frame *frame_info, hi_bool is_producer);

hi_s32 drv_win_send_airesult(hi_drv_win_ai_config *ai_cfg);
hi_s32 drv_win_get_ai_result(hi_handle win_handle, hi_u32 frameindex, hi_drv_win_ai_config *ai_cfg);

hi_s32 drv_win_vpss_queue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 drv_win_vpss_dequeue_frame(hi_handle win_handle, hi_drv_video_frame *frame_info);
hi_s32 drv_win_set_sync(hi_handle win_handle, hi_drv_win_sync sync_mode);
hi_s32 drv_win_set_playctl_info(hi_handle win_handle, hi_drv_win_play_ctrl *playctrl_info);
hi_s32 drv_win_get_playctl_info(hi_handle win_handle, hi_drv_win_play_ctrl *playctrl_info);

hi_s32 drv_win_get_play_info(hi_handle win_handle, hi_drv_win_play_info *play_info);
hi_drv_video_frame *drv_win_update_cfg_frame(hi_handle win_handle, win_framesync_result framesync_result);

hi_s32 drv_win_update_expected_attr(hi_handle win_handle, hi_drv_video_frame *frame_info);

hi_void drv_win_update_disp_frame(hi_handle win_handle);
hi_s32 drv_win_get_status(hi_handle win_handle, hi_bool  *win_enable);

hi_s32 drv_win_proc_get_playstatus(hi_handle win_handle, win_proc_play_status *win_play_status);
hi_s32 drv_win_proc_get_frameinfo(hi_handle win_handle, hi_drv_video_frame *frame_status);
hi_s32 drv_win_proc_get_wininfo(hi_handle win_handle, win_proc_instance_status *win_status);

hi_s32 drv_win_set_frame_proginterleave_mode(hi_handle win_handle, hi_drv_win_forceframeprog_flag frame_mode);
hi_s32 drv_win_get_frame_proginterleave_mode(hi_handle win_handle, hi_drv_win_forceframeprog_flag *frame_mode);

hi_s32 drv_win_get_handle(hi_drv_display disp_id, hi_drv_win_handle* win_handle);

hi_s32 win_suspend(hi_void);
hi_s32 win_resume(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

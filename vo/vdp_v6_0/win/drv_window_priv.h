/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef __DRV_WINDOW_PRIV_H__
#define __DRV_WINDOW_PRIV_H__

#include "hi_type.h"
#include "hi_drv_video.h"
#include "hi_drv_win.h"
#include "drv_win_common.h"
#include "drv_xdp_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WIN_MAX_ASPECT_RATIO 16

#define WIN_FRAME_MIN_WIDTH  64
#define WIN_FRAME_MIN_HEIGHT 64
#define WIN_FRAME_MAX_WIDTH  8192
#define WIN_FRAME_MAX_HEIGHT 8192 /* 4680*8192 */
#define WIN_BUFFER_NODE_NUM 30

#define WIN_INRECT_MIN_WIDTH   64
#define WIN_INRECT_MAX_WIDTH   8192
#define WIN_INRECT_MIN_HEIGHT  64
#define WIN_INRECT_MAX_HEIGHT  8192

#define WIN_OUTRECT_MIN_WIDTH  64
#define WIN_OUTRECT_MAX_WIDTH  7680
#define WIN_OUTRECT_MIN_HEIGHT 64
#define WIN_OUTRECT_MAX_HEIGHT 4320

#define WIN_CROPRECT_MAX_OFFSET_TOP     128
#define WIN_CROPRECT_MAX_OFFSET_LEFT    128
#define WIN_CROPRECT_MAX_OFFSET_BOTTOM  128
#define WIN_CROPRECT_MAX_OFFSET_RIGHT   128

typedef enum {
    HI_DATA_FLOW_QUEUE_DEQUEUE = 0,
    HI_DATA_FLOW_QUEUE_FENCE,
    HI_DATA_FLOW_BUTT
} drv_win_data_flow;

typedef enum {
    RECORD_QUEUE_FRAME_TIME = 0,
    RECORD_CONFIG_FRAME_TIME,
    RECORD_RLS_FRAME_TIME,
    RECORD_TIME_MAX
} win_record_frame_time;

typedef struct {
    hi_bool win_enable;
    hi_bool win_suspend;
    hi_bool stepplay_enable;
    hi_bool quickout_enalbe;
    hi_drv_win_freeze_mode freeze_ctrl;
    hi_bool pause_enalbe;

    hi_drv_win_forceframeprog_flag frame_mode;
    hi_u32 tplay_speed_integer;
    hi_u32 tplay_speed_decimal;
} win_play_ctrl_s;

typedef struct {
    hi_u64 dma_buf_handle_y;
    hi_u64 dma_buf_handle_c;
    hi_u64 addr_offset_y;
    hi_u64 addr_offset_c;
} win_phy_addr_info;

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
    drv_win_data_flow win_play_path;
    hi_bool is_tunnel_fence;
    hi_bool is_android_deque_fence;
    hi_bool is_next_frm;
    hi_bool is_stepmode;
    hi_bool is_vpss_bypass_mode;
    hi_bool is_captured;

    /* tplay speed setting. */
    hi_u32 tplay_speed_integer;
    hi_u32 tplay_speed_decimal;

    /* capture frame private info */
    hi_u64 captured_win_meta_info;
    hi_u64 captured_vpss_meta_info;
} win_play_status_s;

typedef struct {
    hi_bool virt_window_enable;
    hi_bool virt_screen_enable;

    hi_drv_display disp_chn;

    hi_drv_aspect_ratio user_def_aspect_ratio;
    hi_drv_asp_rat_mode aspect_ratio_mode;

    hi_bool user_crop_enable;
    hi_drv_crop_rect crop_rect;

    hi_rect in_rect;
    hi_rect out_rect;

    /* only for virtual window */
    hi_bool user_alloc_buffer;
    hi_u32 buf_number; /* [1,16] */
    hi_drv_pixel_format data_format;
    /* debug for hisi-self path:1->hisi-self hdr path,0->other hdr path */
    hi_bool hisi_hdr;

    hi_u32 win_zorder;
    hi_drv_rot_angle rotation;
    hi_u32 alpha_value;

    hi_bool vert_flip_enable;
    hi_bool horz_flip_enable;
    hi_bool right_eye_first_enable;
} win_user_setting_attr_s;

typedef struct {
    hi_rect crop_rect;
    hi_rect video_rect;
    hi_rect disp_rect;
    hi_bool win_enable;
    hi_bool is_full_screen;
    hi_u32 win_zorder;
    hi_u32 alpha_value;
} win_runtime_attr_s;

typedef struct {
    hi_u32 layer_order;
    hi_u32 layer_id;
    hi_u32 layer_region_id;
} win_layermapping_result;

typedef struct {
    hi_u32 rwzb_stream_id;
    volatile hi_bool is_in_interrupt;
    hi_bool is_mce;
    hi_bool is_secure_mode;
    hi_bool is_update_frame_in_cycle;
} win_misc_dynamic_status;

typedef struct {
    hi_drv_win_sync sync_mode;
    hi_s32 need_sync_adjust;
    hi_s32 total_sync_adjust_cnts;
    hi_u32 newest_frame_play_index;
    hi_u32 all_need_play_cnt;
    hi_u32 display_frame_play_index;
    hi_u32 display_frame_need_play_cnt;
    hi_s64 display_frame_pts;
    hi_u32 display_frame_disp_time;
    hi_u32 debug_discard_cnt;
    hi_u32 debug_repeat_cnt;
    hi_u32 debug_change_time;
    hi_u32 debug_complete_time;
} win_avsync_info;

typedef struct {
    hi_handle h_frc;
    hi_handle h_buf;
    hi_handle h_vpss;
    hi_handle h_vpss_capture;
    hi_handle h_attr;
    hi_handle h_fence;
} win_component_resouce_info;

typedef struct {
    hi_drv_video_frame  frame_info;
    struct osal_list_head    list_node;
}win_frame_node;

typedef struct {
    hi_bool force_vpss_bypass;
    hi_bool force_pq_close;
    hi_bool rls_frame_atonce;
} win_vpss_policy_info;

typedef struct {
    common_list     frame_empty_list;
    common_list     frame_full_list;
    osal_spinlock      win_frame_lock;
} win_frame_relative_info;

typedef struct {
    /* win type and channel */
    hi_handle win_index;
    hi_drv_win_type win_type;

    /* avplay handle, just for linking  disp/virt/ai win together. */
    hi_handle src_handle;

    /* usersetting */
    win_user_setting_attr_s win_user_attr;
    win_play_ctrl_s win_play_ctrl;

    /* status */
    win_misc_dynamic_status win_misc_status;
    win_play_status_s win_play_status;

    /* runtime atomic attr */
    win_runtime_attr_s expected_win_attr;

    win_layermapping_result expect_win_layermapping;

    /* avsync info */
    win_avsync_info avsync_info;

    /* hfrc,hfence,hvpss and so on */
    win_component_resouce_info win_component;
    win_frame_relative_info win_frame_info;

    hi_handle slave_win;
    hi_handle ai_window;
    hi_handle virt_window;

    osal_spinlock spinlock;
} win_descriptor;

typedef struct {
    /* register a virt win to HI_DRV_DISPLAY_2 */
    win_descriptor *win_array[HI_DRV_DISPLAY_BUTT][WINDOW_MAX_NUMBER];
    hi_handle       win_handle_array[HI_DRV_DISPLAY_BUTT][WINDOW_MAX_NUMBER];
    win_play_status_s win_global_proc_info[HI_DRV_DISPLAY_BUTT][WINDOW_MAX_NUMBER];

    hi_u32 current_win_num[HI_DRV_DISPLAY_BUTT];
    /* iso or non-iso. */
    hi_bool is_isogeny;
    hi_u32 win_creat_count;

    osal_spinlock cross_win_spinlock;
} win_management_info;

#ifdef AI_MENTADATA_FUNCTION
typedef struct tag_win_buffer {
    hi_u32 buf_num;
    vdp_list ai_result_empty_list;
    vdp_list ai_result_full_list;

    vdp_list in_f_menta_full_list;
    vdp_list in_f_menta_empty_list;
    spinlock_t in_frame_buf_lock;
} win_ai_buffer;

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

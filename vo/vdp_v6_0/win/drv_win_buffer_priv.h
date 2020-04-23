/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: definition for window buffer
* Author: VDP
* Create: 2019-6-25
 */

#ifndef __DRV_WIN_BUFFER_PRIV_H__
#define __DRV_WIN_BUFFER_PRIV_H__

#include "hi_type.h"
#include "hi_drv_win.h"
#include "hi_drv_proc.h"
#include "hi_osal.h"
#include "drv_win_buffer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WIN_BUFFER_METADATA_SIZE (64 * 1024)
#define WIN_BUFFER_STATE_INIT 0
#define WIN_BUFFER_STATE_DEINIT 1

typedef struct osal_list_head vdp_list;
typedef struct tag_win_meta_buffer {
    vdp_list node;
    hi_u8 vdp_meta_data[WIN_BUFFER_METADATA_SIZE];
    hi_drv_win_vpss_meta_info vdp_meta_info;
} win_meta_buffer;

typedef struct tag_hi_drv_vdp_frame {
    vdp_list node;
    hi_drv_video_frame video_frame;
    hi_bool captured_flag;
} hi_drv_vdp_frame;

typedef struct tag_win_buffer_state {
    hi_u32 try_queue;
    hi_u32 suc_queue;
    hi_u32 try_de_queue;
    hi_u32 suc_dequeue;

    hi_u32 try_acquire;
    hi_u32 suc_acquire;
    hi_u32 try_release;
    hi_u32 suc_release;

    hi_u32 try_que_meta_info;
    hi_u32 suc_que_meta_info;
    hi_u32 try_de_que_meta_info;
    hi_u32 suc_de_que_meta_info;

    hi_u32 under_load;
    hi_bool under_load_enable;

    hi_u32 try_acquire_capture;
    hi_u32 suc_acquire_capture;
    hi_u32 try_release_capture;
    hi_u32 suc_release_capture;
} win_buffer_state;

typedef struct tag_win_buffer {
    hi_drv_win_type win_type;
    hi_u32 buf_num;
    win_buffer_mode buff_mode;
    hi_u32 reset_flag;
    hi_drv_win_reset_mode reset_mode;
    hi_drv_win_freeze_mode freeze_mode;
    hi_bool is_pause;
    hi_bool is_quick_out;
    vdp_list meta_info_empty_list;
    vdp_list meta_info_full_list;
    vdp_list frame_info_empty_list;
    vdp_list frame_info_full_list;
    vdp_list frame_info_disp_list;
    vdp_list frame_info_release_list;
    hi_drv_vdp_frame *capture_frame;
    win_buffer_state buf_state;
    osal_spinlock win_buffer_spin_lock;
} win_buffer;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


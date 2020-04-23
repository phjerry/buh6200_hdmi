/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: functions define
* Author: VDP
* Create: 2019-6-25
*/

#ifndef __DRV_WIN_BUFFER_H__
#define __DRV_WIN_BUFFER_H__

#include "hi_type.h"
#include "hi_drv_video.h"
#include "hi_win_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum tag_win_buffer_mode {
    WIN_BUFFER_MODE_ONLINE = 0,
    WIN_BUFFER_MODE_OFFLINE,
    WIN_BUFFER_MODE_LOGIC_BUFF,
    WIN_BUFFER_MODE_MAX
} win_buffer_mode;

hi_s32 win_buffer_init(void);
hi_s32 win_buffer_deinit(void);
hi_handle win_buffer_create(hi_u32 buf_num, win_buffer_mode buf_mode, hi_drv_win_type win_type);
hi_s32 win_buffer_destroy(hi_handle buf_handle);
hi_s32 win_buffer_queue_frame(hi_handle buf_handle, hi_drv_video_frame *frame);
hi_s32 win_buffer_dequeue_frame(hi_handle buf_handle, hi_drv_video_frame *frame);
hi_s32 win_buffer_set_work_mode(hi_handle buf_handle, win_buffer_mode buf_mode);
hi_s32 win_buffer_get_work_mode(hi_handle buf_handle, win_buffer_mode* buf_mode);
hi_s32 win_buffer_reset(hi_handle buf_handle, hi_drv_win_reset_mode reset_mode);
hi_s32 win_buffer_freeze(hi_handle buf_handle, hi_drv_win_freeze_mode freeze_mode);
hi_s32 win_buffer_pause(hi_handle buf_handle, hi_bool is_pause);
hi_s32 win_buffer_quickout(hi_handle buf_handle, hi_bool is_quick_out);
hi_s32 win_buffer_check_full_frame(hi_handle buf_handle, hi_drv_video_frame **frame);
hi_s32 win_buffer_acquire_frame(hi_handle buf_handle, hi_drv_video_frame **frame);
hi_s32 win_buffer_acquire_lastest_frame(hi_handle buf_handle, hi_drv_video_frame **frame);

hi_s32 win_buffer_release_frame(hi_handle buf_handle, hi_drv_video_frame *frame);
hi_s32 win_buffer_acquire_captured_frame(hi_handle buf_handle, hi_drv_video_frame **capture_frame);
hi_s32 win_buffer_release_captured_frame(hi_handle buf_handle, hi_drv_video_frame *capture_frame);
hi_s32 win_buffer_acquire_metadata(hi_handle buf_handle, hi_drv_win_vpss_meta_info *meta_data, hi_u64 *meta_addr);
hi_s32 win_buffer_release_metadata(hi_handle buf_handle, hi_u64 metadata_addr);
hi_void win_buffer_proc_cnt_info(hi_handle buf_handle, struct seq_file *p);
hi_void win_buffer_proc_node_info(hi_handle buf_handle, struct seq_file *p);
hi_void win_buffer_get_full_frame_info(hi_handle buf_handle,hi_u32 *frame_num, hi_u32 *frame_play_cnt);
hi_u32 win_buffer_get_underload_cnt(hi_handle buf_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

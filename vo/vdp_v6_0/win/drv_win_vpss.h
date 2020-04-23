/*
 * * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * * Description: header file of win_vpss component
 * * Author: vdp
 * * Create: 2019-07-2
 * *
 */

#ifndef __DRV_WIN_VPSS__
#define __DRV_WIN_VPSS__

#include "hi_type.h"
#include "hi_drv_win.h"
#include "hi_drv_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_s32 (*pfn_vpss_get_ai_func)(hi_handle dst_id, hi_drv_win_ai_config ai_cfg);
typedef hi_s32 (*pfn_vpss_get_config)(hi_handle dst_id,
                                      hi_drv_video_frame *videoframe, hi_drv_win_out_config out_cfg);

typedef enum {
    WIN_VPSS_START,
    WIN_VPSS_STOP,
    WIN_VPSS_RESET,
    WIN_VPSS_PAUSE,
    WIN_VPSS_WAKEUP,
    WIN_VPSS_HOLD,
    WIN_VPSS_CMD_MAX,
} win_vpss_set_command;

typedef struct {
    fn_vpss_sink_func vpss_queue_frame;
    fn_vpss_sink_func vpss_dequeue_frame;
    fn_vpss_src_func vpss_complete_frame;
    pfn_vpss_get_ai_func vpss_get_ai_result;
    fn_vpss_get_port_cfg_by_frame vpss_get_out_config;
} win_vpss_export_function;

hi_s32 win_vpss_init(win_vpss_export_function *vpss_reg_func);

hi_s32 win_vpss_deinit(hi_void);

hi_handle win_vpss_create(hi_handle h_window, hi_handle h_source, hi_bool is_passive);

hi_handle win_vpss_update_src(hi_handle h_window, hi_handle h_source);

hi_void win_vpss_destroy(hi_handle h_vpss);

hi_s32 win_vpss_send_command(hi_handle h_vpss, win_vpss_set_command vpss_command, hi_void *p_args);

hi_s32 win_vpss_queue_frame(hi_handle h_vpss, hi_drv_video_frame *src_frame);

hi_s32 win_vpss_dequeue_frame(hi_handle h_vpss, hi_drv_video_frame *src_frame);

hi_s32 win_vpss_set_frame_proginterleave_mode(hi_handle h_vpss, hi_drv_win_forceframeprog_flag frame_mode);

hi_s32 win_vpss_get_frame_proginterleave_mode(hi_handle h_vpss, hi_drv_win_forceframeprog_flag *frame_mode);

hi_void win_vpss_proc_info(hi_handle h_vpss, struct seq_file *p);

hi_void win_vpss_proc_set_bypass(hi_handle h_vpss, hi_bool is_vpss_bypss_mode);

/* only for capture, transfer frame type to sp420 */
hi_s32 win_vpss_transfer_frame(hi_handle h_vpss, hi_drv_video_frame * frame);

hi_s32 win_vpss_release_transferd_frame(hi_handle h_vpss, hi_drv_video_frame * frame);

hi_s32 win_vpss_suspend(hi_void);

hi_s32 win_vpss_resume(hi_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

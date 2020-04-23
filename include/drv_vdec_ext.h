/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2019-05-16
 */
#ifndef __DRV_VDEC_EXT_H__
#define __DRV_VDEC_EXT_H__

#include "hi_vdec_type.h"
#include "hi_drv_video.h"
/* CNcomment: 强制约束，严禁include其它外部头文件，要实现解耦，才能做到独立于SDK共主线演进 */

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

typedef hi_s32 (*fn_drv_vdec_create)
    (hi_handle *handle, const hi_vdec_opt_create *option);

typedef hi_s32 (*fn_drv_vdec_destroy)
    (hi_handle handle, const hi_vdec_opt_destroy *option);

typedef hi_s32 (*fn_drv_vdec_start)
    (hi_handle handle, const hi_vdec_opt_start *option);

typedef hi_s32 (*fn_drv_vdec_stop)
    (hi_handle handle, const hi_vdec_opt_stop *option);

typedef hi_s32 (*fn_drv_vdec_reset)
    (hi_handle handle, const hi_vdec_opt_reset *option);

typedef hi_s32 (*fn_drv_vdec_flush)
    (hi_handle handle, const hi_vdec_opt_flush *option);

typedef hi_s32 (*fn_drv_vdec_decode)
    (hi_handle handle, const hi_vdec_opt_decode *option, hi_vdec_stream *stream, hi_drv_video_frame *frame);

typedef hi_s32 (*fn_drv_vdec_get_attr)
    (hi_handle handle, hi_vdec_attr *attr);

typedef hi_s32 (*fn_drv_vdec_set_attr)
    (hi_handle handle, const hi_vdec_attr *attr);

typedef hi_s32 (*fn_drv_vdec_register_event)
    (hi_handle handle, hi_u32 event_map);

typedef hi_s32 (*fn_drv_vdec_listen_event)
    (hi_handle handle, const hi_vdec_opt_listen *option, hi_vdec_event *event_param);

typedef hi_s32 (*fn_drv_vdec_queue_stream)
    (hi_handle handle, const hi_vdec_opt_que_stm *option, const hi_vdec_stream *stream);

typedef hi_s32 (*fn_drv_vdec_dequeue_stream)
    (hi_handle handle, const hi_vdec_opt_deq_stm *option, hi_vdec_stream *stream);

typedef hi_s32 (*fn_drv_vdec_acquire_frame)
    (hi_handle handle, const hi_vdec_opt_acq_frm *option, hi_drv_video_frame *frame, hi_vdec_ext_frm_info *ext_info);

typedef hi_s32 (*fn_drv_vdec_release_frame)
    (hi_handle handle, const hi_vdec_opt_rel_frm *option, const hi_drv_video_frame *frame);

typedef hi_s32 (*fn_drv_vdec_get_param)
    (hi_handle handle, hi_vdec_param_id param_id, hi_void *param, hi_u32 param_size);

typedef hi_s32 (*fn_drv_vdec_set_param)
    (hi_handle handle, hi_vdec_param_id param_id, const hi_void *param, hi_u32 param_size);

typedef hi_s32 (*fn_drv_vdec_command)
    (hi_handle handle, hi_vdec_cmd_id cmd_id, hi_void *param, hi_u32 param_size);

typedef hi_s32 (*fn_drv_vdec_set_callback)
    (hi_handle vdec_handle, hi_handle user_handle, fn_drv_vdec_callback fn_cb);

typedef struct {
    fn_drv_vdec_create         pfn_drv_vdec_create;
    fn_drv_vdec_destroy        pfn_drv_vdec_destroy;
    fn_drv_vdec_start          pfn_drv_vdec_start;
    fn_drv_vdec_stop           pfn_drv_vdec_stop;
    fn_drv_vdec_reset          pfn_drv_vdec_reset;
    fn_drv_vdec_flush          pfn_drv_vdec_flush;
    fn_drv_vdec_decode         pfn_drv_vdec_decode;
    fn_drv_vdec_get_attr       pfn_drv_vdec_get_attr;
    fn_drv_vdec_set_attr       pfn_drv_vdec_set_attr;
    fn_drv_vdec_register_event pfn_drv_vdec_register_event;
    fn_drv_vdec_listen_event   pfn_drv_vdec_listen_event;
    fn_drv_vdec_get_param      pfn_drv_vdec_get_param;
    fn_drv_vdec_set_param      pfn_drv_vdec_set_param;
    fn_drv_vdec_command        pfn_drv_vdec_command;
    fn_drv_vdec_queue_stream   pfn_drv_vdec_queue_stream;
    fn_drv_vdec_dequeue_stream pfn_drv_vdec_dequeue_stream;
    fn_drv_vdec_acquire_frame  pfn_drv_vdec_acquire_frame;
    fn_drv_vdec_release_frame  pfn_drv_vdec_release_frame;
    fn_drv_vdec_set_callback   pfn_drv_vdec_set_callback;
} drv_vdec_export_func;

hi_s32  drv_vdec_init_module(hi_void);
hi_void drv_vdec_exit_module(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


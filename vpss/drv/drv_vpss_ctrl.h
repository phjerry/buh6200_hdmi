/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_ctrl.h hander file vpss control define
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __DRV_VPSS_CTRL_H__
#define __DRV_VPSS_CTRL_H__

#include "hi_errno.h"
#include "hi_drv_vpss.h"
#include "vpss_define.h"
#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 vpss_ctrl_init(hi_void);
hi_s32 vpss_ctrl_deinit(hi_void);
hi_void vpss_ctrl_context_init(hi_void);
hi_void vpss_ctrl_context_deinit(hi_void);
hi_s32 vpss_ctrl_create_instance(hi_drv_vpss_instance_cfg *instance_cfg, hi_handle *instance_id);
hi_s32 vpss_ctrl_destory_instance(hi_handle h_vpss);
hi_s32 vpss_ctrl_set_vpss_cfg(hi_handle h_vpss, hi_drv_vpss_instance_cfg *instance_cfg);
hi_s32 vpss_ctrl_get_vpss_cfg(hi_handle h_vpss, hi_drv_vpss_instance_cfg *instance_cfg);
hi_s32 vpss_ctrl_get_default_cfg(hi_drv_vpss_instance_cfg *instance_cfg);
hi_s32 vpss_ctrl_create_port(hi_handle h_vpss, hi_drv_vpss_port_cfg *vpss_port_cfg, hi_handle *ph_port);
hi_s32 vpss_ctrl_destroy_port(hi_handle h_port);
hi_s32 vpss_ctrl_set_port_cfg(hi_handle h_port, hi_drv_vpss_port_cfg *vpss_port_cfg);
hi_s32 vpss_ctrl_get_port_cfg(hi_handle h_port, hi_drv_vpss_port_cfg *vpss_port_cfg);
hi_s32 vpss_ctrl_get_default_port_cfg(hi_drv_vpss_port_cfg *vpss_port_cfg);
hi_s32 vpss_ctrl_enable_port(hi_handle h_port, hi_bool enable);
hi_s32 vpss_ctrl_send_command(hi_handle h_vpss, hi_drv_vpss_user_command command, hi_void *args);
hi_s32 vpss_ctrl_queue_frame(hi_handle h_vpss, hi_drv_video_frame *frame);
hi_s32 vpss_ctrl_dequeue_frame(hi_handle h_vpss, hi_drv_video_frame *frame);
hi_s32 vpss_ctrl_acquire_frame(hi_handle h_port, hi_drv_video_frame *frame);
hi_s32 vpss_ctrl_release_frame(hi_handle h_port, hi_drv_video_frame *frame);
hi_s32 vpss_ctrl_regist_hook_event(hi_handle instance_id, hi_handle dst_id, fn_vpss_event_callback event_callback);
hi_s32 vpss_ctrl_regist_callback(hi_handle h_port, hi_handle h_sink, hi_handle h_src,
    hi_drv_vpss_callback_func *fn_vpss_callback);

hi_s32 vpss_ctrl_dev_suspend(hi_void);
hi_s32 vpss_ctrl_dev_resume(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss extern function define
 * Author: zhangjunyu
 * Create: 2016/06/26
 */
#ifndef __DRV_VPSS_EXT_H__
#define __DRV_VPSS_EXT_H__

#include "hi_type.h"
#include "hi_drv_vpss.h"

typedef hi_s32 (*fn_vpss_init)(hi_void);
typedef hi_s32 (*fn_vpss_deinit)(hi_void);
typedef hi_s32 (*fn_vpss_get_instance_default_cfg)(hi_drv_vpss_instance_cfg *instance_cfg);
typedef hi_s32 (*fn_vpss_create_instance)(hi_handle *instance_id, hi_drv_vpss_instance_cfg *instance_cfg);
typedef hi_s32 (*fn_vpss_destroy_instance)(hi_handle instance_id);
typedef hi_s32 (*fn_vpss_set_instance_cfg)(hi_handle instance_id, hi_drv_vpss_instance_cfg *instance_cfg);
typedef hi_s32 (*fn_vpss_get_instance_cfg)(hi_handle instance_id, hi_drv_vpss_instance_cfg *instance_cfg);
typedef hi_s32 (*fn_vpss_get_port_default_cfg)(hi_drv_vpss_port_cfg *port_cfg);
typedef hi_s32 (*fn_vpss_create_port)(hi_handle instance_id, hi_drv_vpss_port_cfg *port_cfg, hi_handle *port_id);
typedef hi_s32 (*fn_vpss_destroy_port)(hi_handle port_id);
typedef hi_s32 (*fn_vpss_get_port_cfg)(hi_handle port_id, hi_drv_vpss_port_cfg *port_cfg);
typedef hi_s32 (*fn_vpss_set_port_cfg)(hi_handle port_id, hi_drv_vpss_port_cfg *port_cfg);
typedef hi_s32 (*fn_vpss_enable_port)(hi_handle port_id, hi_bool enable);
typedef hi_s32 (*fn_vpss_send_command)(hi_handle instance_id, hi_drv_vpss_user_command user_command, hi_void *args);
typedef hi_s32 (*fn_vpss_regist_hook_event)(hi_handle instance_id, hi_handle dst_id,
    fn_vpss_event_callback event_callback);
typedef hi_s32 (*fn_vpss_regist_callback)(hi_handle h_port, hi_handle h_sink,
    hi_handle h_src, hi_drv_vpss_callback_func *fn_vpss_callback);

typedef hi_s32 (*fn_vpss_queue_frame)(hi_handle instance_id, hi_drv_video_frame *image);
typedef hi_s32 (*fn_vpss_dequeue_frame)(hi_handle instance_id, hi_drv_video_frame *image);

typedef hi_s32  (*fn_vpss_acquire_frame)(hi_handle port_id, hi_drv_video_frame *vpss_frame);
typedef hi_s32  (*fn_vpss_release_frame)(hi_handle port_id, hi_drv_video_frame *vpss_frame);
typedef hi_s32  (*fn_vpss_suspend)(hi_void);
typedef hi_s32  (*fn_vpss_resume)(hi_void);

typedef struct {
    fn_vpss_init vpss_init;
    fn_vpss_deinit vpss_deinit;
    fn_vpss_get_instance_default_cfg vpss_get_instance_default_cfg;
    fn_vpss_create_instance vpss_create_instance;
    fn_vpss_destroy_instance vpss_destroy_instance;
    fn_vpss_set_instance_cfg vpss_set_instance_cfg;
    fn_vpss_get_instance_cfg vpss_get_instance_cfg;
    fn_vpss_get_port_default_cfg vpss_get_port_default_cfg;
    fn_vpss_create_port vpss_create_port;
    fn_vpss_destroy_port vpss_destroy_port;
    fn_vpss_get_port_cfg vpss_get_port_cfg;
    fn_vpss_set_port_cfg vpss_set_port_cfg;
    fn_vpss_enable_port vpss_enable_port;
    fn_vpss_send_command vpss_send_command;
    fn_vpss_regist_hook_event vpss_regist_hook_event;
    fn_vpss_regist_callback vpss_regist_callback;
    fn_vpss_queue_frame vpss_queue_frame;
    fn_vpss_dequeue_frame vpss_dequeue_frame;

    fn_vpss_acquire_frame vpss_acquire_frame;
    fn_vpss_release_frame vpss_release_frame;

    fn_vpss_suspend vpss_suspend;
    fn_vpss_resume vpss_resume;
} vpss_export_func;

hi_s32 vpss_drv_mod_init(hi_void);
hi_void vpss_drv_mod_exit(hi_void);

#endif


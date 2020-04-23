/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss drv define
 * Author: zhangjunyu
 * Create: 2016/06/26
 */
#ifndef __HI_DRV_VPSS_H__
#define __HI_DRV_VPSS_H__

#include "hi_drv_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*************************** macro definition ****************************/
#define DEF_HI_DRV_VPSS_MAX_PORT_NUM     3
#define DEF_HI_DRV_VPSS_INSTANCE_MAX_NUM 20

typedef enum {
    HI_DRV_VPSS_PROG_DETECT_FORCE_PROGRESSIVE = 0,
    HI_DRV_VPSS_PROG_DETECT_FORCE_INTERLACE = 1,
    HI_DRV_VPSS_PROG_DETECT_AUTO = 2,
    HI_DRV_VPSS_PROG_DETECT_OFF = 3,

    HI_DRV_VPSS_PROG_DETECT_MAX
} hi_drv_vpss_prog_detect;

/* defines the pass through mode. */
/* c_ncomment:port 的透传控制枚举 */
typedef enum {
    HI_DRV_VPSS_WORKMODE_AUTO = 0, /* 透传由VPSS决策 */
    HI_DRV_VPSS_WORKMODE_BYPASS,   /* 强制透传，逻辑不做处理 */
    HI_DRV_VPSS_WORKMODE_PROCESS,  /* 禁止透传，逻辑需要处理 */

    HI_DRV_VPSS_WORKMODE_MAX
} hi_drv_vpss_work_mode;

typedef enum {
    HI_DRV_VPSS_PQMODE_ALLPQ = 0, /* VPSS 的PQ 处理按照最优PQ 处理 */
    HI_DRV_VPSS_PQMODE_NOPQ,      /* VPSS 的PQ 处理按照不做PQ 处理, 但基本功能PQ处理如DI/ZME/HDR处理自行决定 */

    HI_DRV_VPSS_PQMODE_MAX
} hi_drv_vpss_pq_mode;

/* defines the vpss instance attr. */
/* c_ncomment:定义实例配置 */
typedef struct {
    hi_u32 priority;      /* defines the instance priority. default 16 0 is valid ,1 ~ 31 is more and more prior */
    hi_bool quick_enable; /* prog process, input lowdelay, output lowdelay */
    hi_drv_vpss_prog_detect prog_detect;
    hi_drv_vpss_work_mode work_mode;
    hi_drv_vpss_pq_mode pq_mode;
} hi_drv_vpss_instance_cfg;

/* defines the out buffer mode. */
/* c_ncomment:定义输出帧存管理模式 */
typedef enum {
    HI_DRV_VPSS_BUF_VPSS_ALLOC_MANAGE = 0,
    HI_DRV_VPSS_BUF_USER_ALLOC_MANAGE,

    HI_DRV_VPSS_BUF_TYPE_MAX
} hi_drv_vpss_buffer_type;

typedef struct {
    hi_drv_vpss_buffer_type buf_type;
    hi_u32 buf_num; /* buf_num must be <= DEF_HI_DRV_VPSS_PORT_BUFFER_MAX_NUMBER */
} hi_drv_vpss_buffer_cfg;

/* defines the rotation mode. */
/* c_ncomment:旋转角度枚举 */
typedef enum {
    HI_DRV_VPSS_ROTATION_DISABLE = 0,
    HI_DRV_VPSS_ROTATION_90,
    HI_DRV_VPSS_ROTATION_180,
    HI_DRV_VPSS_ROTATION_270,

    HI_DRV_VPSS_ROTATION_MAX
} hi_drv_vpss_rotation;

typedef enum {
    HI_DRV_VPSS_PORT_TYPE_NORMAL = 0,
    HI_DRV_VPSS_PORT_TYPE_NPU = 1,  /* ai npu port */
    HI_DRV_VPSS_PORT_TYPE_3D_DETECT = 2, /* 3d detect port */

    HI_DRV_VPSS_PORT_TYPE_MAX
} hi_drv_vpss_port_type;

/* defines the port attr. */
/* c_ncomment:定义输出端口配置 */
typedef struct {
    hi_drv_vpss_port_type port_type;
    hi_drv_vpss_work_mode bypass_mode;

    hi_drv_vpss_buffer_cfg buf_cfg;
    hi_bool ori_frame_rate; /* 不管做不做DEI处理，输出帧率都按照原始帧率输出，此时还会丢一半帧 */
    hi_u32 max_frame_rate;  /* in 1/100 HZ */
    hi_bool enable_3d;

    /* frame level */
    /* asp Info */
    hi_bool user_crop_en; /* same as vdp */
    hi_drv_crop_rect in_offset_rect; /* crop mode 1 : offset rect */
    hi_drv_rect crop_rect;    /* crop mode 2 : crop rect */
    hi_drv_rect video_rect;   /* video rect */
    hi_drv_rect out_rect;     /* out rect */

    hi_drv_pixel_format format; /* output pix format */
    hi_drv_compress_info cmp_info; /* out cmp info */
    hi_drv_pixel_bitwidth bit_width;

    hi_drv_vpss_rotation rotation;
    hi_bool vertical_flip;
    hi_bool horizontal_flip;

    hi_bool hdr_follow_src;
    hi_drv_hdr_type out_hdr_type;
    hi_drv_color_descript out_color_desp;
} hi_drv_vpss_port_cfg;

/* defines the port attr. */
/* c_ncomment:定义输出端口配置 */
typedef struct {
    /* frame level */
    hi_bool user_crop_en; /* same as vdp */
    hi_drv_crop_rect in_offset_rect; /* crop mode 1 : offset rect */
    hi_drv_rect crop_rect;    /* crop mode 2 : crop rect */
    hi_drv_rect video_rect;   /* video rect */
    hi_drv_rect out_rect;     /* out rect */

    hi_drv_pixel_format format; /* output pix format */
    hi_drv_compress_info cmp_info; /* out cmp info */
    hi_drv_pixel_bitwidth bit_width;

    hi_drv_vpss_rotation rotation;
    hi_bool vertical_flip;
    hi_bool horizontal_flip;

    hi_bool hdr_follow_src;
    hi_drv_hdr_type out_hdr_type;
    hi_drv_color_descript out_color_desp;
} hi_drv_vpss_port_frame_cfg;

typedef struct {
    hi_u32 ai_cfg; /* ai cfg tmp */
}hi_drv_vpss_ai_cfg;

/* defines the user control command. */
/* c_ncomment:定义用户控制命令 */
typedef enum {
    HI_DRV_VPSS_USER_COMMAND_START = 0,
    HI_DRV_VPSS_USER_COMMAND_STOP = 1,
    HI_DRV_VPSS_USER_COMMAND_RESET = 2, /* clear buffer */
    HI_DRV_VPSS_USER_COMMAND_PAUSE = 3,
    HI_DRV_VPSS_USER_COMMAND_WAKEUP = 4,
    HI_DRV_VPSS_USER_COMMAND_HOLD = 5, /* HOLD src last process frame */

    HI_DRV_VPSS_USER_COMMAND_MAX
} hi_drv_vpss_user_command;

typedef hi_s32 (*fn_vpss_src_func)(hi_handle h_src, hi_drv_video_frame *image);
typedef hi_s32 (*fn_vpss_sink_func)(hi_handle dst_id, hi_drv_video_frame *image);
typedef hi_s32 (*fn_vpss_get_port_cfg_by_frame)(hi_handle dst_id, hi_drv_video_frame *image, hi_drv_vpss_port_frame_cfg *port_cfg);
typedef hi_s32 (*fn_vpss_get_ai_cfg_by_frame)(hi_handle dst_id, hi_drv_video_frame *image, hi_drv_vpss_ai_cfg *ai_cfg);

typedef struct {
    fn_vpss_sink_func sink_queue_frame;
    fn_vpss_sink_func sink_dequeue_frame;

    fn_vpss_src_func src_complete;

    fn_vpss_get_port_cfg_by_frame get_port_cfg_by_frame;
    fn_vpss_get_ai_cfg_by_frame get_ai_cfg_by_frame;
} hi_drv_vpss_callback_func;

typedef enum {
    VPSS_EVENT_BUFLIST_FULL,
    VPSS_EVENT_GET_FRMBUFFER,
    VPSS_EVENT_REL_FRMBUFFER,
    VPSS_EVENT_NEW_FRAME,
    VPSS_EVENT_CHECK_FRAMEBUFFER,
    VPSS_EVENT_TASK_COMPLETE,

    VPSS_EVENT_MAX,
} hi_drv_vpss_event;

typedef hi_s32 (*fn_vpss_event_callback)(hi_handle dst_id, hi_drv_vpss_event event_id, hi_void *args);

/******************************* API declaration *****************************/
hi_s32 hi_drv_vpss_init(hi_void);

hi_s32 hi_drv_vpss_deinit(hi_void);

hi_s32 hi_drv_vpss_get_instance_default_cfg(hi_drv_vpss_instance_cfg *instance_cfg);

hi_s32 hi_drv_vpss_create_instance(hi_handle *instance_id, hi_drv_vpss_instance_cfg *instance_cfg);

hi_s32 hi_drv_vpss_destroy_instance(hi_handle instance_id);

hi_s32 hi_drv_vpss_set_instance_cfg(hi_handle instance_id, hi_drv_vpss_instance_cfg *instance_cfg);

hi_s32 hi_drv_vpss_get_instance_cfg(hi_handle instance_id, hi_drv_vpss_instance_cfg *instance_cfg);

hi_s32 hi_drv_vpss_get_port_default_cfg(hi_drv_vpss_port_cfg *port_cfg);

hi_s32 hi_drv_vpss_create_port(hi_handle instance_id, hi_drv_vpss_port_cfg *port_cfg, hi_handle *port_id);

hi_s32 hi_drv_vpss_destroy_port(hi_handle port_id);

hi_s32 hi_drv_vpss_get_port_cfg(hi_handle port_id, hi_drv_vpss_port_cfg *port_cfg);

hi_s32 hi_drv_vpss_set_port_cfg(hi_handle port_id, hi_drv_vpss_port_cfg *port_cfg);

hi_s32 hi_drv_vpss_enable_port(hi_handle port_id, hi_bool enable);

hi_s32 hi_drv_vpss_send_command(hi_handle instance_id, hi_drv_vpss_user_command user_command, hi_void *args);

hi_s32 hi_drv_vpss_regist_hook_event(hi_handle instance_id, hi_handle dst_id, fn_vpss_event_callback event_callback);

hi_s32 hi_drv_vpss_regist_callback(hi_handle h_port, hi_handle h_sink,
    hi_handle h_src, hi_drv_vpss_callback_func *fn_vpss_callback);

/**********************************VPSS内部分配buffer模式，对外提供以下四个接口 */
/* VPSS SRC buf 被动模式，外部模块主动Quene 源buffer 给VPSS 使用 */
hi_s32 hi_drv_vpss_queue_frame(hi_handle instance_id, hi_drv_video_frame *image);

/* VPSS SRC buf 被动模式，外部模块主动Dequene VPSS 使用完的源buffer */
hi_s32 hi_drv_vpss_dequeue_frame(hi_handle instance_id, hi_drv_video_frame *image);

/* VPSS 输出port buf 被动模式，外部模块主动acquire 输出port buffer 去使用 */
hi_s32 hi_drv_vpss_acquire_frame(hi_handle port_id, hi_drv_video_frame *image);

/* VPSS 输出port buf 被动模式，外部模块归还release 输出port buffer 给VPSS 去使用 */
hi_s32 hi_drv_vpss_release_frame(hi_handle port_id, hi_drv_video_frame *image);

hi_s32 hi_drv_vpss_suspend(hi_void);
hi_s32 hi_drv_vpss_resume(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2019-05-16
 */
#ifndef __DRV_VDEC_IOCTL_H__
#define __DRV_VDEC_IOCTL_H__

#include "hi_drv_module.h"
#include "hi_vdec_type.h"
#include "hi_drv_video.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif

#define VDEC_DEV_NAME "hi_vdec"
#define VDEC_DEV_PATH "/dev/hi_vdec"

#define VDEC_IOCTL_BUF_SIZE    256

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_create option;
} vdec_ioctl_create;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_destroy option;
} vdec_ioctl_destroy;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_start option;
} vdec_ioctl_start;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_stop option;
} vdec_ioctl_stop;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_reset option;
} vdec_ioctl_reset;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_flush option;
} vdec_ioctl_flush;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_decode option;
    hi_vdec_stream stream;
    hi_drv_video_frame frame;
    hi_vdec_ext_frm_info ext_info;
} vdec_ioctl_decode;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_attr attr;
} vdec_ioctl_get_attr;

typedef struct {
    hi_u32 chan_id;
    hi_handle vdec_handle;
    hi_vdec_attr attr;
} vdec_ioctl_set_attr;

typedef struct {
    hi_u32 chan_id;
    hi_u32 event_map;
} vdec_ioctl_register;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_listen option;
    hi_vdec_event event;
} vdec_ioctl_listen;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_que_stm option;
    hi_vdec_stream stream;
} vdec_ioctl_que_stream;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_deq_stm option;
    hi_vdec_stream stream;
} vdec_ioctl_deq_stream;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_acq_frm option;
    hi_drv_video_frame frame;
    hi_vdec_ext_frm_info ext_frm_info;
} vdec_ioctl_acq_frame;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_opt_rel_frm option;
    hi_drv_video_frame frame;
} vdec_ioctl_rel_frame;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_param_id param_id;
    hi_u8  param[VDEC_IOCTL_BUF_SIZE];
    hi_u32 param_size;
} vdec_ioctl_param;

typedef struct {
    hi_u32 chan_id;
    hi_vdec_cmd_id cmd_id;
    hi_u8  param[VDEC_IOCTL_BUF_SIZE];
    hi_u32 param_size;
} vdec_ioctl_command;

#define UMAPC_VDEC_CREATE           _IOWR(HI_ID_VDEC, 1,  vdec_ioctl_create)
#define UMAPC_VDEC_DESTROY          _IOW (HI_ID_VDEC, 2,  vdec_ioctl_destroy)
#define UMAPC_VDEC_START            _IOW (HI_ID_VDEC, 3,  vdec_ioctl_start)
#define UMAPC_VDEC_STOP             _IOW (HI_ID_VDEC, 4,  vdec_ioctl_stop)
#define UMAPC_VDEC_RESET            _IOW (HI_ID_VDEC, 5,  vdec_ioctl_reset)
#define UMAPC_VDEC_FLUSH            _IOW (HI_ID_VDEC, 6,  vdec_ioctl_flush)
#define UMAPC_VDEC_DECODE           _IOWR(HI_ID_VDEC, 7,  vdec_ioctl_decode)
#define UMAPC_VDEC_GET_ATTR         _IOR (HI_ID_VDEC, 8,  vdec_ioctl_get_attr)
#define UMAPC_VDEC_SET_ATTR         _IOW (HI_ID_VDEC, 9,  vdec_ioctl_set_attr)
#define UMAPC_VDEC_REGISTER_EVENT   _IOW (HI_ID_VDEC, 10, vdec_ioctl_register)
#define UMAPC_VDEC_LISTEN_EVENT     _IOWR(HI_ID_VDEC, 11, vdec_ioctl_listen)
#define UMAPC_VDEC_QUEUE_STREAM     _IOW (HI_ID_VDEC, 12, vdec_ioctl_que_stream)
#define UMAPC_VDEC_DEQUEUE_STREAM   _IOWR(HI_ID_VDEC, 13, vdec_ioctl_deq_stream)
#define UMAPC_VDEC_ACQUIRE_FRAME    _IOWR(HI_ID_VDEC, 14, vdec_ioctl_acq_frame)
#define UMAPC_VDEC_RELEASE_FRAME    _IOW (HI_ID_VDEC, 15, vdec_ioctl_rel_frame)
#define UMAPC_VDEC_GET_PARAM        _IOWR(HI_ID_VDEC, 16, vdec_ioctl_param)
#define UMAPC_VDEC_SET_PARAM        _IOW (HI_ID_VDEC, 17, vdec_ioctl_param)
#define UMAPC_VDEC_COMMAND          _IOWR(HI_ID_VDEC, 18, vdec_ioctl_command)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

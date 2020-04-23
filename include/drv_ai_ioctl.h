/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: implement of ai ioctl
 * Author: audio
 * Create: 2019-06-21
 * Notes:  NA
 * History: 2019-06-21 Initial version for Hi3796CV300
 */

#ifndef __DRV_AI_IOCTL_H__
#define __DRV_AI_IOCTL_H__

#include "hi_drv_dev.h"

#include "hi_ai_type.h"
#include "hi_drv_ao.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct {
    hi_ai_port           ai_port;
    hi_ai_attr           attr;
} ai_get_df_attr_param, *ai_get_df_attr_param_p;

typedef struct {
    hi_ai_port           ai_port;
    hi_ai_attr           attr;
    hi_handle            ai;
} ai_create_param, *ai_create_param_p;

typedef struct {
    hi_handle            ai;
    hi_bool              ai_enable;
} ai_enable_param, *ai_enable_param_p;

typedef struct {
    hi_handle            ai;
    hi_u32               need_bytes;
} ai_frame_param, *ai_frame_param_p;

typedef struct {
    hi_handle            ai;
    hi_ai_attr           attr;
} ai_attr_param, *ai_attr_param_p;

typedef struct {
    hi_handle            ai;
    ai_buf_attr          ai_buf;
} ai_buf_param, *ai_buf_param_p;

typedef struct {
    hi_handle            ai;
    hi_ai_delay          delay_comps;
} ai_delay_comps_param, *ai_delay_comps_param_p;

typedef struct {
    hi_handle    ai;
    hi_u64       proc_phy_addr;
    hi_u32       proc_buf_size;
    hi_s32       proc_buf_map_fd;
} ai_proc_init_param, *ai_proc_init_param_p;

/* AI device command code */
#define CMD_AI_GETDEFAULTATTR _IOWR  (HI_ID_AI, 0x00, ai_get_df_attr_param)
#define CMD_AI_CREATE _IOWR  (HI_ID_AI, 0x01, ai_create_param)
#define CMD_AI_DESTROY _IOW  (HI_ID_AI, 0x02, hi_handle)
#define CMD_AI_SETENABLE _IOW  (HI_ID_AI, 0x03, ai_enable_param)
#define CMD_AI_GETENABLE _IOWR  (HI_ID_AI, 0x04, ai_enable_param)
#define CMD_AI_ACQUIREFRAME _IOWR  (HI_ID_AI, 0x05, ai_frame_param)
#define CMD_AI_RELEASEFRAME _IOW  (HI_ID_AI, 0x06, hi_handle)
#define CMD_AI_SETATTR _IOW  (HI_ID_AI, 0x07, ai_attr_param)
#define CMD_AI_GETATTR _IOWR  (HI_ID_AI, 0x08, ai_attr_param)
#define CMD_AI_GETBUFINFO _IOWR  (HI_ID_AI, 0x09, ai_buf_param)
#define CMD_AI_SETBUFINFO _IOW  (HI_ID_AI, 0x0a, ai_buf_param)
#define CMD_AI_SETDELAYCOMPS _IOW  (HI_ID_AI, 0x0b, ai_delay_comps_param)
#define CMD_AI_GETDELAYCOMPS _IOWR  (HI_ID_AI, 0x0c, ai_delay_comps_param)

#define CMD_AI_PROCINIT _IOWR  (HI_ID_AI, 0x10, ai_proc_init_param)
#define CMD_AI_PROCDEINIT _IOW  (HI_ID_AI, 0x11, hi_handle)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc_die.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_WBC_DIE_H__
#define __HAL_VPSS_WBC_DIE_H__

#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u32 width;                      /* 帧信息宽 */
    hi_u32 height;                     /* 帧信息高 */
    hi_drv_pixel_format pixel_format;  /* 帧信息像素格式 */
    hi_drv_pixel_bitwidth bit_width;   /* 帧信息图像bit位,10 bit or 8 bit */
} vpss_wbc_deirfr_calc;

typedef struct {
    hi_u32 width;                      /* 帧信息宽 */
    hi_u32 height;                     /* 帧信息高 */
    hi_drv_pixel_format pixel_format;  /* 帧信息像素格式 */
    hi_drv_pixel_bitwidth bit_width;   /* 帧信息图像bit位,10 bit or 8 bit */
    hi_bool secure;
} vpss_wbc_deirfr_attr;

typedef struct {
    hi_bool init;                       /* 初始化标识 */
    hi_u32 complete_count;              /* 回写链表轮转次数 */
    hi_drv_vpss_video_frame wbc_frame;       /* 回写帧信息 */
    vpss_wbc_deirfr_attr dei_wbc_attr;  /* 回写链表属性 */
    drv_vpss_mem_info vpss_mem;           /* 帧存信息 */
} vpss_wbc_deirfr;

hi_s32 vpss_wbc_deirfr_init(vpss_wbc_deirfr *dei_wbc, vpss_wbc_deirfr_attr *in_attr);

hi_s32 vpss_wbc_deirfr_deinit(vpss_wbc_deirfr *dei_wbc);

/* 获取回写队列中的参考帧 */
hi_s32 vpss_wbc_deirfr_get_ref_info(vpss_wbc_deirfr *dei_wbc, hi_drv_vpss_video_frame **ref_frame);
/* 获取回写队列中的回写帧 */
hi_s32 vpss_wbc_deirfr_get_wbc_info(vpss_wbc_deirfr *dei_wbc, hi_drv_vpss_video_frame **wbc_frame);
/* 设置回写队列轮转 */
hi_s32 vpss_wbc_deirfr_complete_image(vpss_wbc_deirfr *dei_wbc);
hi_s32 vpss_wbc_deirfr_reset(vpss_wbc_deirfr *dei_wbc);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif






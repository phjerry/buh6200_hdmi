/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#ifndef __DRV_VPSS_WBC_H__
#define __DRV_VPSS_WBC_H__

#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool interlace;
    hi_bool secure;
    hi_u32 width;
    hi_u32 height;
    hi_drv_source source;
    hi_drv_pixel_bitwidth bit_width;
    hi_drv_pixel_format pixel_format;
} vpss_wbc_attr;

typedef struct {
    hi_drv_vpss_video_frame *w_frame;
    hi_drv_vpss_video_frame *rp1_frame;
    hi_drv_vpss_video_frame *rp2_frame;
    hi_drv_vpss_video_frame *rp3_frame;
} vpss_wbc_nr_cfg;

typedef struct {
    hi_drv_vpss_video_frame *w_frame;
    hi_drv_vpss_video_frame *rp2_frame;
    hi_drv_vpss_video_frame *rp4_frame;
    hi_drv_vpss_video_frame *rp8_frame;
} vpss_wbc_cccl_cfg;

typedef struct {
    hi_drv_vpss_video_frame *w_frame;
    hi_drv_vpss_video_frame *r_frame;
} vpss_wbc_dei_cfg;

typedef struct {
    hi_drv_vpss_video_frame *w_frame;
    hi_drv_vpss_video_frame *rp2_frame;
} vpss_wbc_hds_cfg;

typedef struct {
    hi_drv_vpss_video_frame *w_frame;
    hi_drv_vpss_video_frame *rp2_frame;
} vpss_wbc_hvds_cfg;

typedef struct {
    vpss_wbc_nr_cfg wbc_nr_cfg;
    vpss_wbc_cccl_cfg wbc_cccl_cfg;
    vpss_wbc_dei_cfg wbc_dei_cfg;
    vpss_wbc_hds_cfg wbc_hds_cfg;
    vpss_wbc_hvds_cfg wbc_hvds_cfg;
} vpss_wbc_cfg;

typedef struct {
    hi_bool init;
    hi_u32 complete_count;
    vpss_wbc_attr wbc_attr;
    hi_void *contex;
} vpss_wbc;

hi_s32 vpss_wbc_init(vpss_wbc *wbc, vpss_wbc_attr *attr);
hi_s32 vpss_wbc_deinit(vpss_wbc *wbc);
hi_s32 vpss_wbc_reset(vpss_wbc *wbc);
hi_s32 vpss_wbc_complete(vpss_wbc *wbc);
hi_s32 vpss_wbc_fill_wbc_frame_info(hi_void *fill_instance, hi_drv_3d_eye_type en3_dtype);
hi_s32 vpss_wbc_get_cfg(vpss_wbc *wbc, vpss_wbc_cfg *wbc_cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif






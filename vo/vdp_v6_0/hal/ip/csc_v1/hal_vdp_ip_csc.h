/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_CSC_H__
#define __HAL_VDP_IP_CSC_H__

#include "vdp_chip_define.h"
#include "hi_drv_video.h"
#include "hi_drv_disp.h"

typedef enum {
    VDP_CSC_ID_VID1 = 0, /* 0x03200 */
    VDP_CSC_ID_VID3,     /* 0x05200 */
    VDP_CSC_ID_VID0,     /* 0x32200 */

    VDP_CSC_ID_DHD0_SR0,  /* 0x94480 */
    VDP_CSC_ID_DHD0_SR1,  /* 0x94580 */
    VDP_CSC_ID_INTF_MIPI, /* 0x91760 */

    VDP_CSC_ID_BUTT
} vdp_csc_id;

typedef enum {
    VDP_CSC_YUV_MODE = 0,
    VDP_CSC_RGB_MODE = 1,
    VDP_CSC_MODE_BUTT,
} vdp_csc_mode;

typedef struct {
    hi_bool csc_disable_for_hdr;
    hi_drv_color_descript i_mode;
    hi_drv_color_descript o_mode;
} vdp_csc_info;

typedef struct {
    hi_bool ink_en;
    hi_bool ink_sel;
    hi_bool data_fmt; /* 0:YCbCr  1:RGB */
    hi_bool cross_en;
    hi_u32 color_mode;

    hi_u32 x;
    hi_u32 y;
} vdp_csc_ink_info;

hi_void vdp_ip_csc_disable(vdp_csc_id csc_id);
hi_void vdp_ip_csc_set(vdp_csc_id csc_id, hi_drv_color_descript i_mode, hi_drv_color_descript o_mode);
hi_s32 vdp_ip_csc_set_ink(hi_u32 layer, vdp_csc_ink_info *ink_info);
hi_s32 vdp_ip_csc_get_ink(hi_u32 layer, vdp_bkg *bkg);
hi_void vdp_ip_csc_data_transfer(hi_drv_disp_color *rgb,
    hi_drv_color_descript cs,  vdp_bkg *yuv);
#endif


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_VID_H__
#define __HAL_VDP_IP_VID_H__

#include "vdp_chip_define.h"

typedef struct {
    hi_bool enable;
    hi_bool secure_en;
    vdp_bkg bkg; /* set the mixv bg */
    vdp_rect video_rect;
    vdp_rect disp_rect;
    hi_u32 alpha;
} vdp_vid_cfg;

typedef struct {
    vdp_rect vp0_rect;
    vdp_bkg bkg;
    hi_u32 alpha;
} vdp_vp_cfg;

hi_void vdp_ip_vid_set_cfg(hi_u32 layer, vdp_vid_cfg *vid_cfg);
hi_void vdp_ip_vp_set_cfg(hi_u32 layer, vdp_vp_cfg *vp_cfg);
hi_void vdp_ip_vid_regup(hi_void);
hi_bool vdp_ip_vid_get_mark_status(hi_u32 layer);

#endif


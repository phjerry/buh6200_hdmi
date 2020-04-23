/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_NXG_H__
#define __HAL_VDP_IP_NXG_H__

#include "vdp_chip_define.h"

typedef enum {
    HI_TEE_VWM_NEXGUARD_SETTING_GENERIC = 0,    /* <Generic setting */
    HI_TEE_VWM_NEXGUARD_SETTING_8bit,           /* <SDR 8bit setting */
    HI_TEE_VWM_NEXGUARD_SETTING_10bit,          /* <SDR 10bit setting */
    HI_TEE_VWM_NEXGUARD_SETTING_HDR10,          /* <HDR setting */
    HI_TEE_VWM_NEXGUARD_SETTING_HLG,            /* <HLG setting */
    HI_TEE_VWM_NEXGUARD_SETTING_DOLBY,          /* <DOLBY setting */
    HI_TEE_VWM_NEXGUARD_SETTING_BUTT
} hi_tee_vwm_nxg_type;

typedef struct {
    hi_u32 enable;
    hi_u32 debug_en;
    hi_u32 fmt;
    hi_u32 frame_type;
    hi_u32 frame_rate;
    hi_tee_vwm_nxg_type data_type;

    hi_u32 keyin;
    hi_u32 payload;
    hi_u32 setting[6]; /* 6 is an index */
    hi_u32 video_yfpos;
    hi_u32 video_xfpos;
    hi_u32 video_ylpos;
    hi_u32 video_xlpos;
} vdp_nxg_info;

hi_s32 vdp_nxg_set_mode(hi_u32 layer, vdp_nxg_info *cfg);

#endif


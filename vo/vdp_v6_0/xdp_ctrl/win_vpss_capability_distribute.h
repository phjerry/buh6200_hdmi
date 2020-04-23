/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef WIN_LAYER_MAPPING_H
#define WIN_LAYER_MAPPING_H
#include "hi_type.h"
#include "hi_drv_disp.h"
#include "hi_drv_video.h"

typedef struct hiwin_attr_status {
    HI_DRV_PIXEL_BITWIDTH_E bit_width;
    HI_DRV_DISP_OUT_TYPE_E output_hdr_type;
    HI_DRV_COLOR_SPACE_E output_csc_type;
    HI_DRV_PIX_FORMAT_E pixel_format;
    HI_DRV_PIX_FORMAT_E pixel_format;
} win_vpss_frame_format;

#endif

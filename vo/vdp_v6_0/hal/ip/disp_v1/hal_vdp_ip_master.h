/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_MASTER_H__
#define __HAL_VDP_IP_MASTER_H__

#include "vdp_chip_define.h"
#include "hi_drv_disp.h"

typedef enum {
    VDP_MASTER_LAYER_HARDEN0 = 0,
    VDP_MASTER_LAYER_HARDEN1 = 1,
    VDP_MASTER_LAYER_HARDEN_BUTT,
} vdp_master_layer_harden;

typedef enum {
    VDP_MASTER_SEL_0 = 0,
    VDP_MASTER_SEL_1,
    VDP_MASTER_SEL_2,
    VDP_MASTER_SEL_BUTT
} vdp_master_sel;

typedef enum {
    VDP_MASTER_AXI_EDGE_MODE_128 = 0,
    VDP_MASTER_AXI_EDGE_MODE_256,
    VDP_MASTER_AXI_EDGE_MODE_1024,
    VDP_MASTER_AXI_EDGE_MODE_2048,
    VDP_MASTER_AXI_EDGE_MODE_4096,
    VDP_MASTER_AXI_EDGE_MODE_BUTT
} vdp_master_axi_edge_mode;

typedef enum {
    // core_harden
    VDP_MASTER_RCHN_PARA_CORE = 0,
    VDP_MASTER_RCHN_PARA_SUB = 1,
    VDP_MASTER_RCHN_V0P = 2,
    VDP_MASTER_RCHN_V1 = 3,
    VDP_MASTER_RCHN_V2 = 4,
    VDP_MASTER_RCHN_V3 = 5,
    VDP_MASTER_RCHN_G0 = 6,
    VDP_MASTER_RCHN_G0P = 7,
    VDP_MASTER_RCHN_G1 = 8,
    VDP_MASTER_RCHN_G2 = 9,
    VDP_MASTER_RCHN_G3 = 10,
    // v0_harden
    VDP_MASTER_RCHN_PARA_V0 = 0,
    VDP_MASTER_RCHN_V0 = 1,
    VDP_MASTER_RCHN_DC = 2,
    VDP_MASTER_RCHN_BUTT = 11
} vdp_master_rchn;

typedef enum {
    VDP_MASTER_WCHN_WBC = 0,
    VDP_MASTER_WCHN_STT,
    VDP_MASTER_WCHN_BUTT
} vdp_master_wchn;

typedef enum {
    VDP_MASTER_BALENCE_1_0 = 0,
    VDP_MASTER_BALENCE_0_1,
    VDP_MASTER_BALENCE_1_1,
    VDP_MASTER_BALENCE_1_2,
    VDP_MASTER_BALENCE_2_1,
    VDP_MASTER_BALENCE_1_3,
    VDP_MASTER_BALENCE_3_1,
    VDP_MASTER_BALENCE_1_4,
    VDP_MASTER_BALENCE_4_1,
    VDP_MASTER_BALENCE_BUTT
} vdp_master_balence_mode;

hi_void vdp_ip_master_init(hi_void);
hi_s32 vdp_ip_master_get_error(hi_void);
hi_void vdp_ip_master_clean_error(hi_void);
hi_void vdp_ip_master_update(vdp_dispchn_chn chn_id, hi_drv_disp_fmt fmt);

#endif


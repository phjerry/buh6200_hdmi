/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_VID_H__
#define __HAL_VDP_VID_H__

#include "hal_layer_intf.h"

#include "vdp_chip_define.h"
#include "hal_vdp_ip_zme.h"
#include "hal_vdp_ip_fdr.h"
#include "hal_vdp_ip_csc.h"
#include "hal_vdp_ip_hdr.h"
#include "hal_vdp_ip_vid.h"
#include "hal_vdp_ip_dispchn.h"

typedef struct {
    vdp_vid_cfg vid_cfg;
    vdp_vp_cfg vp_cfg;
    vdp_zme_info zme_info;
    vdp_fdr_init_info mac_info;
    vdp_fdr_info fdr_info;
    vdp_fdr_addr fdr_addr;
    vdp_csc_info csc_info;
} vdp_vid_info;

typedef struct {
    hi_bool fdr_addr_update;
    hi_bool vid_cfg_update;
    hi_bool vp_cfg_update;
    hi_bool zme_info_update;
    hi_bool mac_info_update;
    hi_bool fdr_info_update;
    hi_bool csc_info_update;
} vdp_vid_update_info;

hi_void vdp_vid_update(vdp_vid_info *vid_info, vdp_vid_info *save_info, vdp_vid_update_info *update_info);
hi_void vdp_vid_assert(vdp_layer_vid layer, vdp_layer_info *layer_info, vdp_vid_info *info);
hi_void vdp_vid_transfer(vdp_layer_vid layer_id, vdp_layer_info *layer_info, vdp_vid_info *vid_info);
hi_void vdp_vid_set_layer(hi_u32 layer, vdp_vid_info *vid_info, vdp_vid_update_info* update_info);
hi_void vdp_vid_regup(hi_void);
hi_bool vdp_vid_get_mark_status(hi_u32 layer);
hi_void vdp_vid_disable_layer(vdp_layer_vid layer_id, hi_u32 region_num);

#endif


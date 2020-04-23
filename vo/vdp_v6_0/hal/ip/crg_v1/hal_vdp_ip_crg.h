/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_CRG_H__
#define __HAL_VDP_IP_CRG_H__

#include "hi_drv_disp.h"
#include "vdp_chip_define.h"
#include "hal_vdp_ip_dispchn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_drv_disp_fmt disp_fmt;
    hi_u32 pixel_clk;
    vdp_dispchn_multichn multichn_en;
} vdp_disp_crg;

hi_void vdp_ip_crg_attach_intf(vdp_dispchn_chn chn_id, vdp_disp_intf intf);
hi_void vdp_ip_crg_set_chn_clk(vdp_dispchn_chn chn_id, hi_drv_disp_intf *intf_attr,
                               vdp_disp_crg *crg_info);
hi_void vdp_ip_crg_work_clk_enable(hi_bool enable);
hi_void vdp_ip_crg_soft_reset(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


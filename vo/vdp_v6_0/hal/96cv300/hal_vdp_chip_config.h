/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HAL_VDP_CHIP_CONFIG_H__
#define __HAL_VDP_CHIP_CONFIG_H__

#include "hal_layer_intf.h"
#include "hal_disp_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void vdp_chip_capaciblity_init(hi_void);
vdp_capacity *hal_vdp_get_chip_capacity(void);
layer_capacity *hal_layer_get_chip_capacity(hi_u32 layer);
disp_capacity *hal_disp_get_chip_capacity(hi_drv_display chn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_VO_HAL_H__ */

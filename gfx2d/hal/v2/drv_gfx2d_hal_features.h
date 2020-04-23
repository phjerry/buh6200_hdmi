/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Initialization of filter coefficients
 * Create: 2019-11-05
 */

#ifndef __LINUX_DRV_GFX2D_DRV_GFX2D_HAL_FEATURES_H__
#define __LINUX_DRV_GFX2D_DRV_GFX2D_HAL_FEATURES_H__

#include "hi_type.h"
#include "drv_gfx2d_config.h"
#include "drv_gfx2d_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hwc_layer_id select_layer_id_from_features(drv_gfx2d_compose_msg *compose, hi_u8 *ref, hi_u32 ref_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_GFX2D_HAL_V2_FILTER_PARA__ */

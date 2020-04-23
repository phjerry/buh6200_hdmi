/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: gfx2d config
 * Author: sdk
 * Create: 2018-01-01
 */

#ifndef _GFX2D_CONFIG_H_
#define _GFX2D_CONFIG_H_

#include "hi_type.h"
#include "hi_osal.h"

/**********************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* **************************** Macro Definition ********************************************** */

#define GFX2D_MAX_LAYERS 8

typedef enum {
    HWC_LAYER_VHD = 0x1,
    HWC_LAYER_G0,
    HWC_LAYER_G1,
    HWC_LAYER_G2,
    HWC_LAYER_G3,
    HWC_LAYER_G4,
    HWC_LAYER_G5,
    HWC_LAYER_G6,
    HWC_LAYER_G7,
    HWC_LAYER_G8,
    HWC_LAYER_BUTT
} hwc_layer_id;

typedef hi_void (*GFX2D_FUNC_CB)(hi_void *param);

/* ****************************** API declaration ********************************************* */
hi_s32 GFX2D_CONFIG_SetMemSize(hi_u32 u32MemSize);

hi_u32 GFX2D_CONFIG_GetMemSize(hi_void);

hi_u32 GFX2D_CONFIG_GetNodeNum(hi_void);

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_CONFIG_StartTime(hi_void);

hi_u32 GFX2D_CONFIG_EndTime(hi_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _GFX2D_CONFIG_H_ */

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb proc header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_PROC_H__
#define __DRV_HIFB_PROC_H__

/* ********************************add include here************************************************ */

#include "drv_hifb_config.h"
#include "drv_hifb_type.h"

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifdef CONFIG_GFX_PROC_SUPPORT

/* **************************** Macro Definition ************************************************** */

/* ************************** Structure Definition ************************************************ */
typedef struct tagDrvHiFBProcS {
    hi_bool bCreateProc;
    hi_bool b3DStatus;
    hi_bool bTrace;
    hi_bool bCloseInterrupt;
    drv_hifb_compress_mode compress_mode; /* * decompress status from hard composer refresh    */
    hi_u32 WbcLayerID;
    hi_u32 HwcRefreshInDispFmt;    /* * display pixel format from hard composer refresh */
    hi_u32 HwcRefreshInDispStride; /* * display stride from hard composer refresh       */
    hi_u32 HwcRefreshInDispAdress; /* * display address from hard composer refresh      */
} DRV_HIFB_PROC_S;

/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */
hi_s32 DRV_HIFB_PROC_Create(hi_u32 LayerID);
hi_void DRV_HIFB_PROC_Destory(hi_u32 LayerID);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

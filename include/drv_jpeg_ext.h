/***************************************************************************************************
*
* Copyright (C) 2018 Hisilicon Technologies Co., Ltd.  All rights reserved.
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon),
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
****************************************************************************************************
File Name        : drv_jpeg_ext.c
Version          : Initial Draft
Author           : sdk
Created          : 2018/01/01
Description      :
Function List    :


History          :
Date                Author                Modification
2018/01/01          sdk                   Created file
****************************************************************************************************/

#ifndef __DRV_JPEG_EXT_H__
#define __DRV_JPEG_EXT_H__

/* **************************** add include here**************************************************** */

#if defined(CHIP_TYPE_hi3712) || defined(CHIP_TYPE_hi3716m) || defined(CHIP_TYPE_hi3716mv310) || \
    defined(CHIP_TYPE_hi3110ev500) || defined(CHIP_TYPE_hi3716mv320) || defined(CHIP_TYPE_hi3716mv330)
#include "drv_dev_ext.h"
#else
#include "hi_drv_dev.h"
#endif
/***************************************************************************************************/

#include "hi_drv_jpeg.h"

/***************************************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition *************************************************** */

/* **************************** Structure Definition *********************************************** */

/* **************************** Global Variable declaration **************************************** */

/* **************************** API forward declarations ******************************************* */

/* **************************** API realization **************************************************** */
typedef hi_s32 (*FN_JPEG_ModInit)(HI_VOID);
typedef HI_VOID (*FN_JPEG_ModeExit)(HI_VOID);
typedef hi_s32 (*FN_JPEG_Suspend)(struct device *dev);
typedef hi_s32 (*FN_JPEG_Resume)(struct device *dev);

#ifndef CONFIG_JPEG_UT_UNSUPPORT
typedef hi_s32 (*FN_JPEG_OSR_GetIntStatus)(JPEG_GETINTTYPE_S *);
typedef hi_u32 (*FN_JPEG_HAL_ReadReg)(hi_u32);
typedef HI_VOID (*FN_JPEG_HAL_WriteReg)(hi_u32, hi_u32);
typedef HI_VOID (*FN_JPEG_HAL_GetIntStatus)(hi_u32 *);
#endif

typedef struct {
    FN_JPEG_ModInit pfnJpegModInit;
    FN_JPEG_ModeExit pfnJpegModExit;
    FN_JPEG_Suspend pfnJpegSuspend;
    FN_JPEG_Resume pfnJpegResume;
#ifndef CONFIG_JPEG_UT_UNSUPPORT
    FN_JPEG_OSR_GetIntStatus pfnJpegOsrGetIntStatus;
    FN_JPEG_HAL_ReadReg pfnJpegHalReadReg;
    FN_JPEG_HAL_WriteReg pfnJpegHalWriteReg;
    FN_JPEG_HAL_GetIntStatus pfnJpegHalGetIntStatus;
#endif
} JPEG_EXPORT_FUNC_S;

HI_VOID JPEG_DRV_ModExit(HI_VOID);
hi_s32 JPEG_DRV_ModInit(HI_VOID);
hi_s32 JPEG_DRV_K_ModInit(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_JPEG_EXT_H__ */

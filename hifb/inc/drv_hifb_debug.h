/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb debug header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_DEBUG_H__
#define __DRV_HIFB_DEBUG_H__

#include "hi_type.h"
#include "hi_gfx_sys_k.h"

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************** */

/* ************************** Structure Definition ************************************************ */

/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */
hi_void DRV_HIFB_OutputMessage(const hi_char *pFunc, const hi_u32 Line, const GRAPHIC_DFX_TYPE_E eMsgType,
                               const hi_u32 Format, ...);
hi_void DRV_HIFB_SetLogFunc(hi_char *pFunc);
hi_void DRV_HIFB_SetLogLevel(hi_u32 LogLevel);
hi_void DRV_HIFB_SetLogSave(hi_u32 LogSave);
hi_void DRV_HIFB_ProcMsg(hi_void *p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* *__DRV_HIFB_DEBUG_H__* */

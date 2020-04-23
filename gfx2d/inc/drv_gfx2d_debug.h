/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d debug
 * Author: sdk
 * Create: 2019-05-13
 */

#ifndef __GFX2D_DEBUG_H__
#define __GFX2D_DEBUG_H__

/* ********************************add include here************************************************ */
#include <linux/seq_file.h>
#include "hi_gfx_sys_k.h"
#include "hi_osal.h"

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* ************************** Structure Definition ************************************************ */
typedef enum {
    GFX2D_DEBUG_CLOSE_LEVEL_0 = 0,
    GFX2D_DEBUG_ERR_LEVEL_1,
    GFX2D_DEBUG_FUNC_LEVEL_2,
    GFX2D_DEBUG_VALUE_LEVEL_3,
    GFX2D_DEBUG_ALL_LEVEL_4,
    GFX2D_DEBUG_NODE_LEVEL_5,
    GFX2D_DEBUG_LEVEL_BUTT,
} GFX2D_DEBUG_LEVEL_E;

/* ****************************** API declaration ************************************************* */
hi_void DRV_GFX2D_OutputMessage(const hi_char *pFunc, const hi_u32 Line, const GRAPHIC_DFX_TYPE_E eMsgType,
                                const hi_u32 Format, ...);
hi_void DRV_GFX2D_SetLogFunc(hi_char *pFunc);
hi_void DRV_GFX2D_SetLogLevel(hi_u32 LogLevel);
hi_void DRV_GFX2D_SetLogSave(hi_u32 LogSave);
hi_void DRV_GFX2D_ProcMsg(struct seq_file *p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* *__GFX2D_DEBUG_H__* */

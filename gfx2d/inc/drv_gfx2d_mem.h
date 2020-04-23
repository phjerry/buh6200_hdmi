/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: memory operation header
 * Author: sdk
 * Create: 2019-05-17
 */

#ifndef _GFX2D_MEM_H_
#define _GFX2D_MEM_H_

/* ********************************add include here******************************************** */

#include "drv_gfx2d_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* ****************************** API declaration ********************************************* */

hi_s32 GFX2D_MEM_Init(const hi_u32 u32Size);

hi_void GFX2D_MEM_Deinit(hi_void);

hi_s32 GFX2D_MEM_Register(const hi_u32 u32BlockSize, const hi_u32 u32BlockNum);

hi_void GFX2D_Mem_UnRegister(const hi_u32 u32BlockSize);

hi_void *GFX2D_MEM_Alloc(const hi_u32 u32Size);

hi_s32 GFX2D_MEM_Free(hi_void *pBuf);

hi_u64 GFX2D_MEM_GetPhyaddr(hi_void *pBuf);

#ifdef CONFIG_GFX_PROC_SUPPORT

hi_void GFX2D_MEM_ReadProc(hi_void *p, hi_void *v);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _GFX2D_MEM_H_ */

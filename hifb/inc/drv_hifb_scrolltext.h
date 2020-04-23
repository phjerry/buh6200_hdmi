/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb scrolltext header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_SCROLLTEXT_H__
#define __DRV_HIFB_SCROLLTEXT_H__

/* ********************************add include here********************************************** */
#include "hi_type.h"
#include <linux/fb.h>
#include "hi_osal.h"

/************************************************************************************************/

/************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************ */

#define SCROLLTEXT_CACHE_NUM 2 /* * 双buffer  * */
#define SCROLLTEXT_NUM 2       /* * 最大只支持上下两个字幕  * */

/* ************************** Structure Definition ********************************************** */
typedef enum {
    HIFB_SCROLLTEXT_HD0_HANDLE0 = 0x00,
    HIFB_SCROLLTEXT_HD0_HANDLE1 = 0x01,
    HIFB_SCROLLTEXT_HD1_HANDLE0 = 0x10,
    HIFB_SCROLLTEXT_HD1_HANDLE1 = 0x11,
    HIFB_SCROLLTEXT_HD2_HANDLE0 = 0x20,
    HIFB_SCROLLTEXT_HD2_HANDLE1 = 0x21,
    HIFB_SCROLLTEXT_SD0_HANDLE0 = 0x40,
    HIFB_SCROLLTEXT_SD0_HANDLE1 = 0x41,
    HIFB_SCROLLTEXT_BUTT_HANDLE
} HIFB_SCROLLTEXT_HANDLE;

/* ************************** Structure Definition ********************************************** */
typedef struct {
    hi_bool bInusing;
    hi_u32 u32PhyAddr;
    hi_u8 *pVirAddr;
    hi_void *dmabuf;
} HIFB_SCROLLTEXT_CACHE;

typedef struct {
    hi_bool bAvailable;
    hi_bool bPause;
    hi_bool bDeflicker;
    hi_bool bBliting;
    hi_u32 u32cachebufnum;
    hi_u32 u32Stride;
    hi_s32 s32TdeBlitHandle;
    HIFB_SCROLLTEXT_HANDLE enHandle;
    volatile hi_u32 u32IdleFlag;
    osal_wait wbEvent;
    HIFB_RECT stRect;
    DRV_HIFB_COLOR_FMT_E ePixelFmt;
    HIFB_SCROLLTEXT_CACHE stCachebuf[SCROLLTEXT_CACHE_NUM];
} HIFB_SCROLLTEXT_S;

typedef struct {
    hi_bool bAvailable;
    hi_u32 u32textnum;
    hi_u32 u32ScrollTextId;
    HIFB_SCROLLTEXT_S stScrollText[SCROLLTEXT_NUM];
    osal_workqueue blitScrollTextWork;
} HIFB_SCROLLTEXT_INFO_S;

/* ********************* Global Variable declaration ******************************************** */

/* ****************************** API declaration *********************************************** */
hi_s32 DRV_HIFB_SCROLLTEXT_GetHandle(hi_u32 u32Handle, hi_u32 *pU32LayerId, hi_u32 *pScrollTextId);

hi_s32 DRV_HIFB_SCROLLTEXT_FreeCacheBuf(HIFB_SCROLLTEXT_S *pstScrollText);

hi_s32 DRV_HIFB_SCROLLTEXT_Create(hi_u32 u32LayerId, HIFB_SCROLLTEXT_CREATE_S *stScrollText);

hi_s32 DRV_HIFB_SCROLLTEXT_FillText(HIFB_SCROLLTEXT_DATA_S *stScrollTextData);

hi_s32 DRV_HIFB_SCROLLTEXT_Destory(hi_u32 u32LayerID, hi_u32 u32ScrollTextID);

hi_s32 DRV_HIFB_SCROLLTEXT_Blit(hi_u32 u32LayerId);

hi_s32 drv_init_scrolltext_lock(hi_void);

hi_void drv_deinit_scrolltext_lock(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

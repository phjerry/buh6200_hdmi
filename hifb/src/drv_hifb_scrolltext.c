/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb scrolltext
 * Author: sdk
 * Create: 2016-01-01
 */

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
/* ********************************add include here********************************************** */
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/fb.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include "hi_osal.h"

#include "drv_hifb_type.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_mem.h"
#include "drv_hifb_scrolltext.h"
#include "hi_gfx_sys_k.h"

static osal_spinlock scrolltextLock = {NULL};

static inline hi_s32 DRV_SCROLLTEXT_SwitchTextBuffer(HIFB_SCROLLTEXT_S *pstScrollText, hi_u32 BufNum);

/***************************************************************************************
 * func         : HIFB_SCROLLTEXT_AllocHandle
 * description  : 判断滚动字幕参数是否合法
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_u32 HIFB_SCROLLTEXT_AllocHandle(hi_u32 LayerId)
{
    hi_u32 u32ScrollTextHandle;
    HIFB_SCROLLTEXT_INFO_S *pstScrollTextInfo = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (LayerId >= sizeof(s_stTextLayer) / sizeof(HIFB_SCROLLTEXT_INFO_S)) {
        return HIFB_SCROLLTEXT_BUTT_HANDLE;
    }
    pstScrollTextInfo = &s_stTextLayer[LayerId];
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstScrollTextInfo, HIFB_SCROLLTEXT_BUTT_HANDLE);

    u32ScrollTextHandle = pstScrollTextInfo->u32ScrollTextId++;
    if (pstScrollTextInfo->u32ScrollTextId > 1) {
        pstScrollTextInfo->u32ScrollTextId = 0;
    }

    if (!pstScrollTextInfo->bAvailable) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the scroll text was invalid");
        return HIFB_SCROLLTEXT_BUTT_HANDLE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return ((0xf0 & (LayerId << 4)) | (0x0f & u32ScrollTextHandle));
}

/***************************************************************************************
 * func         : hifb_parse_scrolltexthandle
 * description  : 获取图层ID和字幕的ID
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_SCROLLTEXT_GetHandle(hi_u32 u32Handle, hi_u32 *pU32LayerId, hi_u32 *pScrollTextId)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pU32LayerId, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pScrollTextId, HI_FAILURE);

    if (u32Handle >= HIFB_SCROLLTEXT_BUTT_HANDLE) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "invalid scrolltext handle");
        return HI_FAILURE;
    }

    *pU32LayerId = (u32Handle & 0xf0) >> 4;
    *pScrollTextId = u32Handle & 0x0f;

    if (*pU32LayerId >= HIFB_LAYER_ID_BUTT) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "invalid scrolltext handle");
        *pU32LayerId = HIFB_LAYER_ID_BUTT;
        return HI_FAILURE;
    }

    if (*pScrollTextId >= SCROLLTEXT_NUM) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "invalid scrolltext handle");
        *pScrollTextId = SCROLLTEXT_NUM;
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_SCROLLTEXT_IsOverLay
 * description  : check these two rectangle overlay each other
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static inline hi_bool HIFB_SCROLLTEXT_IsOverLay(HIFB_RECT *pstSrcRect, HIFB_RECT *pstDstRect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSrcRect, HI_FALSE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstRect, HI_FALSE);

    if ((pstSrcRect->x >= (pstDstRect->x + pstDstRect->w)) || (pstDstRect->x >= (pstSrcRect->x + pstSrcRect->w))) {
        return HI_FALSE;
    }

    if ((pstSrcRect->y >= (pstDstRect->y + pstDstRect->h)) || (pstDstRect->y >= (pstSrcRect->y + pstSrcRect->h))) {
        return HI_FALSE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_TRUE;
}

/***************************************************************************************
 * func         : HIFB_SCROLLTEXT_CheckPara
 * description  : 判断滚动字幕参数是否合法
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 HIFB_SCROLLTEXT_CheckPara(hi_u32 u32LayerId, HIFB_SCROLLTEXT_ATTR_S *stAttr)
{
    hi_u32 i;
    HIFB_RECT stScrollTextRect, stSrcRect;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (u32LayerId >= sizeof(s_stLayer) / sizeof(HIFB_LAYER_S)) {
        return HI_FAILURE;
    }
    if (u32LayerId >= sizeof(s_stTextLayer) / sizeof(HIFB_SCROLLTEXT_INFO_S)) {
        return HI_FAILURE;
    }

    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stAttr, HI_FAILURE);
    stScrollTextRect = stAttr->stRect;

    memset(&stScrollTextRect, 0x0, sizeof(stScrollTextRect));
    memset(&stSrcRect, 0x0, sizeof(stSrcRect));

    if ((0 == stAttr->u16CacheNum) || (stAttr->u16CacheNum > SCROLLTEXT_CACHE_NUM)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the cachenum u applied was invalid");
        return HI_FAILURE;
    }

    if (s_stTextLayer[u32LayerId].u32textnum >= SCROLLTEXT_NUM) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, s_stTextLayer[u32LayerId].u32textnum);
        return HI_FAILURE;
    }

    if ((0 > stScrollTextRect.x) || (0 > stScrollTextRect.y) ||
        (pstPar->stExtendInfo.stPos.s32XPos > stScrollTextRect.x) ||
        (pstPar->stExtendInfo.stPos.s32YPos > stScrollTextRect.y)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.y);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.stPos.s32XPos);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.stPos.s32YPos);
        return HI_FAILURE;
    }

    if ((0 > stScrollTextRect.w) || (0 > stScrollTextRect.h) ||
        (pstPar->stExtendInfo.DisplayWidth < stScrollTextRect.w) ||
        (pstPar->stExtendInfo.DisplayHeight < stScrollTextRect.h)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.h);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayHeight);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,
                             "failed to create the scrolltext because of wrong width or height");
        return HI_FAILURE;
    }

    if ((pstPar->stExtendInfo.stPos.s32XPos + pstPar->stExtendInfo.DisplayWidth) <
            (stScrollTextRect.w + stScrollTextRect.x) ||
        (pstPar->stExtendInfo.stPos.s32YPos + pstPar->stExtendInfo.DisplayHeight) <
            (stScrollTextRect.h + stScrollTextRect.y)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.y);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stScrollTextRect.h);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.stPos.s32XPos);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.stPos.s32YPos);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstPar->stExtendInfo.DisplayHeight);
        return HI_FAILURE;
    }

    for (i = 0; i < s_stTextLayer[u32LayerId].u32textnum; i++) {
        if (s_stTextLayer[u32LayerId].stScrollText[i].bAvailable) {
            stSrcRect = s_stTextLayer[u32LayerId].stScrollText[i].stRect;
            if (HIFB_SCROLLTEXT_IsOverLay(&stSrcRect, &stScrollTextRect)) {
                GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_SCROLLTEXT_IsOverLay, FAILURE_TAG);
                return HI_FAILURE;
            }
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : DRV_HIFB_SCROLLTEXT_FreeCacheBuf
 * description  : 释放滚动字幕buffer
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_SCROLLTEXT_FreeCacheBuf(HIFB_SCROLLTEXT_S *pstScrollText)
{
    hi_u32 i;
    hi_char *pBuf = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstScrollText, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(pstScrollText->u32cachebufnum, SCROLLTEXT_CACHE_NUM + 1, HI_FAILURE);

    for (i = 0; i < pstScrollText->u32cachebufnum; i++) {
        pBuf = pstScrollText->stCachebuf[i].pVirAddr;
        if (NULL != pBuf) {
            drv_hifb_mem_unmap(pstScrollText->stCachebuf[i].dmabuf, pBuf);
        }

        if (pstScrollText->stCachebuf[i].u32PhyAddr != 0) {
            drv_hifb_mem_free(pstScrollText->stCachebuf[i].dmabuf);
        }

        pstScrollText->stCachebuf[i].pVirAddr = NULL;
        pstScrollText->stCachebuf[i].u32PhyAddr = 0;
        pstScrollText->stCachebuf[i].bInusing = HI_FALSE;
        pstScrollText->stCachebuf[i].dmabuf = HI_NULL;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_SCROLLTEXT_AllocBuf
 * description  : 创建滚动字幕buffer
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 HIFB_SCROLLTEXT_AllocBuf(hi_u32 u32LayerId, HIFB_SCROLLTEXT_ATTR_S *stAttr)
{
    struct fb_info *info = NULL;
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32StartAddr;
    hi_u32 i, u32cacheSize, u32Pitch;
    hi_char name[256] = {'\0'};
    hi_char *pBuf = NULL;
    hi_u32 u32Index = 0;
    HIFB_SCROLLTEXT_S *pstScrollText = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (u32LayerId >= sizeof(s_stLayer) / sizeof(HIFB_LAYER_S)) {
        return HI_FAILURE;
    }
    if (u32LayerId >= sizeof(s_stTextLayer) / sizeof(HIFB_SCROLLTEXT_INFO_S)) {
        return HI_FAILURE;
    }

    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);

    u32Index = s_stTextLayer[u32LayerId].u32ScrollTextId;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, SCROLLTEXT_NUM, HI_FAILURE);

    pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32Index]);

    if (pstScrollText->bAvailable) {
        DRV_HIFB_SCROLLTEXT_FreeCacheBuf(pstScrollText);
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_WIDTH | HIFB_PARA_CHECK_HEIGHT | HIFB_PARA_CHECK_BITSPERPIXEL,
                                  stAttr->stRect.w, stAttr->stRect.h, 0, 0, info->var.bits_per_pixel);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stAttr->stRect.w);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stAttr->stRect.h);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->var.bits_per_pixel);
        return HI_FAILURE;
    }
    u32Pitch = (((hi_u32)stAttr->stRect.w * (hi_u32)info->var.bits_per_pixel >> 3) + 15) >> 4;
    u32Pitch = u32Pitch << 4;

    u32cacheSize = u32Pitch * stAttr->stRect.h;

    if (stAttr->u16CacheNum > SCROLLTEXT_CACHE_NUM) {
        return HI_FAILURE;
    }

    for (i = 0; i < stAttr->u16CacheNum; i++) {
        snprintf(name, sizeof(name), "HIFB_Layer%d_Scroll%d", u32LayerId, i);
        name[sizeof(name) - 1] = '\0';

        pstScrollText->stCachebuf[i].dmabuf = osal_mem_alloc(name, u32cacheSize, OSAL_NSSMMU_TYPE, HI_NULL, 0);
        if (pstScrollText->stCachebuf[i].dmabuf == HI_NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_mem_alloc, FAILURE_TAG);
            return HI_FAILURE;
        }
        pstScrollText->stCachebuf[i].u32PhyAddr = drv_hifb_mem_map_to_smmu(pstScrollText->stCachebuf[i].dmabuf);

        pBuf = (hi_char *)drv_hifb_mem_map(pstScrollText->stCachebuf[i].dmabuf);
        if (pBuf == NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_mem_map, FAILURE_TAG);
            drv_hifb_mem_free(pstScrollText->stCachebuf[i].dmabuf);
            return HI_FAILURE;
        }

        memset(pBuf, 0, u32cacheSize);

        pstScrollText->stCachebuf[i].bInusing = HI_FALSE;
        pstScrollText->stCachebuf[i].pVirAddr = pBuf;
    }

    pstScrollText->bAvailable = HI_TRUE;
    pstScrollText->u32Stride = u32Pitch;
    pstScrollText->u32cachebufnum = stAttr->u16CacheNum;
    pstScrollText->bDeflicker = stAttr->bDeflicker;
    pstScrollText->ePixelFmt = stAttr->ePixelFmt;
    memcpy(&(pstScrollText->stRect), &(stAttr->stRect), sizeof(HIFB_RECT));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void HIFB_SCROLLTEXT_BlitWork(osal_workqueue *work);

/***************************************************************************************
 * func         : DRV_HIFB_SCROLLTEXT_Create
 * description  : 创建滚动字幕
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_SCROLLTEXT_Create(hi_u32 u32LayerId, HIFB_SCROLLTEXT_CREATE_S *stScrollText)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Index = 0;
    HIFB_SCROLLTEXT_ATTR_S stAttr;
    HIFB_SCROLLTEXT_INFO_S *pstTextInfo = NULL;
    HIFB_SCROLLTEXT_S *stText = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stScrollText, HI_FAILURE);
    stAttr = stScrollText->stAttr;

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    pstTextInfo = &s_stTextLayer[u32LayerId];

    Ret = HIFB_SCROLLTEXT_CheckPara(u32LayerId, &stAttr);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_SCROLLTEXT_CheckPara, Ret);
        return HI_FAILURE;
    }

    Ret = HIFB_SCROLLTEXT_AllocBuf(u32LayerId, &stAttr);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_SCROLLTEXT_AllocBuf, Ret);
        return HI_FAILURE;
    }

    pstTextInfo->u32textnum++;

    if (!pstTextInfo->bAvailable) {
        Ret = osal_workqueue_init(&(s_stTextLayer[u32LayerId].blitScrollTextWork), HIFB_SCROLLTEXT_BlitWork);
        if (Ret != 0) {
            u32Index = s_stTextLayer[u32LayerId].u32ScrollTextId;
            DRV_HIFB_SCROLLTEXT_FreeCacheBuf(s_stTextLayer[u32LayerId].stScrollText[u32Index]);
            return HI_FAILURE;
        }
    }

    pstTextInfo->bAvailable = HI_TRUE;

    stScrollText->u32Handle = HIFB_SCROLLTEXT_AllocHandle(u32LayerId);

    u32Index = stScrollText->u32Handle & 0x0f;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32Index, SCROLLTEXT_NUM, HI_FAILURE);
    stText = &(pstTextInfo->stScrollText[u32Index]);

    stText->enHandle = stScrollText->u32Handle;
    stText->u32IdleFlag = 1;

    if (osal_wait_init(&(stText->wbEvent)) != 0) {
        u32Index = s_stTextLayer[u32LayerId].u32ScrollTextId;
        DRV_HIFB_SCROLLTEXT_FreeCacheBuf(s_stTextLayer[u32LayerId].stScrollText[u32Index]);
        osal_workqueue_destroy(&(s_stTextLayer[u32LayerId].blitScrollTextWork));
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_hifb_get_idle_flag(const void *param)
{
    HIFB_SCROLLTEXT_S *pstScrollText = (HIFB_SCROLLTEXT_S *)param;

    if (pstScrollText == HI_NULL) {
        return 0;
    }
    return pstScrollText->u32IdleFlag;
}

/***************************************************************************************
 * func         : DRV_HIFB_SCROLLTEXT_FillText
 * description  : 数据处理，这个地方是重头戏，处理比较复杂
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_SCROLLTEXT_FillText(HIFB_SCROLLTEXT_DATA_S *stScrollTextData)
{
    hi_u32 i;
    hi_u32 u32LayerId = HIFB_LAYER_ID_BUTT;
    hi_u32 u32TextId = SCROLLTEXT_NUM;
    hi_u32 u32Handle;
    hi_s32 Ret;
    HIFB_SCROLLTEXT_CACHE stCacheBuf;
    HIFB_BUFFER_S stTempBuf, stCanvasBuf;
    HIFB_BLIT_OPT_S stBlitOpt;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    HIFB_SCROLLTEXT_S *pstScrollText = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stScrollTextData, HI_FAILURE);
    u32Handle = stScrollTextData->u32Handle;

    memset(&stBlitOpt, 0, sizeof(stBlitOpt));

    Ret = DRV_HIFB_SCROLLTEXT_GetHandle(u32Handle, &u32LayerId, &u32TextId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_GetHandle, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);
    info = s_stLayer[u32LayerId].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);

    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32TextId, SCROLLTEXT_NUM, HI_FAILURE);
    pstScrollText = &(s_stTextLayer[u32LayerId].stScrollText[u32TextId]);
    if (pstScrollText->u32cachebufnum > SCROLLTEXT_CACHE_NUM) {
        return HI_FAILURE;
    }

    if (HI_FALSE == pstScrollText->bAvailable) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "the scrolltext was invalid");
        return HI_FAILURE;
    }

    if (HI_TRUE == pstScrollText->bPause) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    GRAPHIC_CHECK_EQUAL_ZERO_RETURN_VALUE(pstScrollText->stRect.h, HI_FAILURE);

    GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(pstScrollText->stRect.h - 1, stScrollTextData->u32Stride,
                                                      HI_FAILURE);

    GRAPHIC_CHECK_UINT_ADDITION_REVERSAL_RETURN((pstScrollText->stRect.h - 1) * stScrollTextData->u32Stride,
                                                pstScrollText->u32Stride, HI_FAILURE);

    if ((pstScrollText->stRect.h - 1) * stScrollTextData->u32Stride + pstScrollText->u32Stride >
        stScrollTextData->addr_len) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstScrollText->stRect.h);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stScrollTextData->u32Stride);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, stScrollTextData->addr_len);
        return HI_FAILURE;
    }

    if (0 != stScrollTextData->u32PhyAddr) {
        if (!pstScrollText->u32IdleFlag) {
            osal_wait_timeout_interruptible(&pstScrollText->wbEvent, drv_hifb_get_idle_flag, pstScrollText, 100 * HZ);
        }

        stCanvasBuf.stCanvas.u32PhyAddr = stScrollTextData->u32PhyAddr;
        stCanvasBuf.stCanvas.enFmt = pstScrollText->ePixelFmt;
        stCanvasBuf.stCanvas.u32Width = pstScrollText->stRect.w;
        stCanvasBuf.stCanvas.u32Height = pstScrollText->stRect.h;
        stCanvasBuf.stCanvas.u32Pitch = stScrollTextData->u32Stride;
        stCanvasBuf.UpdateRect.x = 0;
        stCanvasBuf.UpdateRect.y = 0;
        stCanvasBuf.UpdateRect.w = pstScrollText->stRect.w;
        stCanvasBuf.UpdateRect.h = pstScrollText->stRect.h;

        for (i = 0; i < pstScrollText->u32cachebufnum; i++) {
            stCacheBuf = pstScrollText->stCachebuf[i];
            if (HI_FALSE == stCacheBuf.bInusing) {
                stTempBuf.stCanvas.u32PhyAddr = stCacheBuf.u32PhyAddr;
                stTempBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
                stTempBuf.stCanvas.u32Width = pstScrollText->stRect.w;
                stTempBuf.stCanvas.u32Height = pstScrollText->stRect.h;
                stTempBuf.stCanvas.u32Pitch = pstScrollText->u32Stride;
                stTempBuf.UpdateRect.x = 0;
                stTempBuf.UpdateRect.y = 0;
                stTempBuf.UpdateRect.w = pstScrollText->stRect.w;
                stTempBuf.UpdateRect.h = pstScrollText->stRect.h;

                if (stTempBuf.stCanvas.u32Pitch != stCanvasBuf.stCanvas.u32Pitch) {
                    stBlitOpt.bScale = HI_TRUE;
                }

                if (pstScrollText->bDeflicker && (pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE)) {
                    stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
                }

                stBlitOpt.bBlock = HI_TRUE;
                stBlitOpt.bCallBack = HI_FALSE;

                pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
                pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
                GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit, HI_FAILURE);
                Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(&stCanvasBuf, &stTempBuf, &stBlitOpt, HI_TRUE);
                if (Ret <= 0) {
                    GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
                    return HI_FAILURE;
                }

                i = DRV_SCROLLTEXT_SwitchTextBuffer(pstScrollText, i);

                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }
        }
    } else if (NULL != stScrollTextData->pu8VirAddr) {
        hi_char *pBuf = NULL;
        if (pstPar->stExtendInfo.enColFmt != pstScrollText->ePixelFmt) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstPar->stExtendInfo.enColFmt);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstScrollText->ePixelFmt);
            return HI_FAILURE;
        }

        if (!pstScrollText->u32IdleFlag) {
            osal_wait_timeout_interruptible(&pstScrollText->wbEvent, drv_hifb_get_idle_flag, pstScrollText, 100 * HZ);
        }

        for (i = 0; i < pstScrollText->u32cachebufnum; i++) {
            hi_u32 u32LineNum = 0;
            stCacheBuf = pstScrollText->stCachebuf[i];
            if (HI_FALSE == stCacheBuf.bInusing) {
                GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstScrollText->stCachebuf[i].pVirAddr, HI_FAILURE);
                GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stScrollTextData->pu8VirAddr, HI_FAILURE);

                pBuf = pstScrollText->stCachebuf[i].pVirAddr;
                for (u32LineNum = 0; u32LineNum < pstScrollText->stRect.h; u32LineNum++) {
                    if (osal_copy_from_user(pBuf + u32LineNum * pstScrollText->u32Stride,
                                            stScrollTextData->pu8VirAddr + u32LineNum * stScrollTextData->u32Stride,
                                            pstScrollText->u32Stride)) {
                        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_from_user, FAILURE_TAG);
                        return -EFAULT;
                    }
                }

                i = DRV_SCROLLTEXT_SwitchTextBuffer(pstScrollText, i);

                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }
        }
    }

    return HI_FAILURE;
}

hi_s32 drv_init_scrolltext_lock(hi_void)
{
    if (osal_spin_lock_init(&scrolltextLock) != 0) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void drv_deinit_scrolltext_lock(hi_void)
{
    if (scrolltextLock.lock != HI_NULL) {
        osal_spin_lock_destory(&scrolltextLock);
        scrolltextLock.lock = HI_NULL;
    }

    return;
}

static inline hi_s32 DRV_SCROLLTEXT_SwitchTextBuffer(HIFB_SCROLLTEXT_S *pstScrollText, hi_u32 BufNum)
{
    hi_ulong flags = 0;
    hi_s32 s32Cnt = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    osal_spin_lock_irqsave(&scrolltextLock, &flags);
    if (NULL == pstScrollText) {
        osal_spin_unlock_irqrestore(&scrolltextLock, &flags);
        osal_spin_lock_destory(&scrolltextLock);
        scrolltextLock.lock = HI_NULL;
        return HI_FAILURE;
    }

    if (BufNum >= SCROLLTEXT_CACHE_NUM) {
        osal_spin_unlock_irqrestore(&scrolltextLock, &flags);
        osal_spin_lock_destory(&scrolltextLock);
        scrolltextLock.lock = HI_NULL;
        return HI_FAILURE;
    }
    pstScrollText->stCachebuf[BufNum].bInusing = HI_TRUE;

    pstScrollText->u32IdleFlag = 0;

    if (pstScrollText->u32cachebufnum >= SCROLLTEXT_CACHE_NUM) {
        osal_spin_unlock_irqrestore(&scrolltextLock, &flags);
        osal_spin_lock_destory(&scrolltextLock);
        scrolltextLock.lock = HI_NULL;
        return HI_FAILURE;
    }

    while (BufNum < pstScrollText->u32cachebufnum) {
        hi_u32 u32LastUsingCache = BufNum;
        hi_u32 j = BufNum + 1;

        while (j < pstScrollText->u32cachebufnum) {
            if (HI_TRUE == pstScrollText->stCachebuf[j].bInusing) {
                hi_bool bInusing = pstScrollText->stCachebuf[j].bInusing;
                hi_u32 u32PhyAddr = pstScrollText->stCachebuf[j].u32PhyAddr;
                hi_u8 *pVirAddr = pstScrollText->stCachebuf[j].pVirAddr;
                pstScrollText->stCachebuf[j].bInusing = pstScrollText->stCachebuf[u32LastUsingCache].bInusing;
                pstScrollText->stCachebuf[j].u32PhyAddr = pstScrollText->stCachebuf[u32LastUsingCache].u32PhyAddr;
                pstScrollText->stCachebuf[j].pVirAddr = pstScrollText->stCachebuf[u32LastUsingCache].pVirAddr;
                pstScrollText->stCachebuf[u32LastUsingCache].bInusing = bInusing;
                pstScrollText->stCachebuf[u32LastUsingCache].u32PhyAddr = u32PhyAddr;
                pstScrollText->stCachebuf[u32LastUsingCache].pVirAddr = pVirAddr;
                u32LastUsingCache = j;
            } else {
                pstScrollText->u32IdleFlag = 1;
            }

            j++;
        }

        BufNum++;
    }

    if (HI_FALSE == pstScrollText->stCachebuf[0].bInusing) {
        pstScrollText->u32IdleFlag = 1;
        for (s32Cnt = 1; s32Cnt < pstScrollText->u32cachebufnum; s32Cnt++) {
            hi_bool bInusing = pstScrollText->stCachebuf[s32Cnt - 1].bInusing;
            hi_u32 u32PhyAddr = pstScrollText->stCachebuf[s32Cnt - 1].u32PhyAddr;
            hi_u8 *pVirAddr = pstScrollText->stCachebuf[s32Cnt - 1].pVirAddr;
            pstScrollText->stCachebuf[s32Cnt - 1].bInusing = pstScrollText->stCachebuf[s32Cnt].bInusing;
            pstScrollText->stCachebuf[s32Cnt - 1].u32PhyAddr = pstScrollText->stCachebuf[s32Cnt].u32PhyAddr;
            pstScrollText->stCachebuf[s32Cnt - 1].pVirAddr = pstScrollText->stCachebuf[s32Cnt].pVirAddr;
            pstScrollText->stCachebuf[s32Cnt].bInusing = bInusing;
            pstScrollText->stCachebuf[s32Cnt].u32PhyAddr = u32PhyAddr;
            pstScrollText->stCachebuf[s32Cnt].pVirAddr = pVirAddr;
        }
    }

    osal_spin_unlock_irqrestore(&scrolltextLock, &flags);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return BufNum;
}

/***************************************************************************************
 * func         : DRV_HIFB_SCROLLTEXT_Destory
 * description  : 销毁滚动字幕
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_SCROLLTEXT_Destory(hi_u32 u32LayerID, hi_u32 u32ScrollTextID)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_SCROLLTEXT_S *pstScrollText = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerID, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32ScrollTextID, SCROLLTEXT_NUM, HI_FAILURE);

    if (!s_stTextLayer[u32LayerID].stScrollText[u32ScrollTextID].bAvailable) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s_stTextLayer[u32LayerID].stScrollText[u32ScrollTextID].bAvailable);
        return HI_FAILURE;
    }

    if (0 == s_stTextLayer[u32LayerID].u32textnum) {
        return HI_FAILURE;
    }

    s_stTextLayer[u32LayerID].u32textnum--;

    if (0 == s_stTextLayer[u32LayerID].u32textnum) {
        s_stTextLayer[u32LayerID].bAvailable = HI_FALSE;
        osal_workqueue_flush(&s_stTextLayer[u32LayerID].blitScrollTextWork);
    }

    pstScrollText = &(s_stTextLayer[u32LayerID].stScrollText[u32ScrollTextID]);

    if (pstScrollText->s32TdeBlitHandle) {
        if (NULL != g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_WaitForDone) {
            Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_WaitForDone(pstScrollText->s32TdeBlitHandle, 1000);
        }
        if (Ret < 0) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_WaitForDone, Ret);
            return HI_FAILURE;
        }
    }
    osal_wait_destroy(&(pstScrollText->wbEvent));

    DRV_HIFB_SCROLLTEXT_FreeCacheBuf(pstScrollText);

    s_stTextLayer[u32LayerID].u32ScrollTextId = u32ScrollTextID;
    osal_workqueue_destroy(&s_stTextLayer[u32LayerID].blitScrollTextWork);
    memset(pstScrollText, 0, sizeof(HIFB_SCROLLTEXT_S));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : HIFB_SCROLLTEXT_CallBack
 * description  : 滚动字幕回调，TDE blit之后回调的
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_s32 HIFB_SCROLLTEXT_CallBack(hi_void *pParaml, hi_void *pParamr)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32TextLayerId = HIFB_LAYER_ID_BUTT;
    hi_u32 u32TextId = SCROLLTEXT_NUM;
    hi_u32 u32Handle;
    HIFB_SCROLLTEXT_S *pstScrollText = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pParaml, HI_FAILURE);
    u32Handle = *(hi_u32 *)pParaml;

    Ret = DRV_HIFB_SCROLLTEXT_GetHandle(u32Handle, &u32TextLayerId, &u32TextId);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_SCROLLTEXT_GetHandle, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32TextLayerId, HI_FAILURE);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(u32TextId, SCROLLTEXT_NUM, HI_FAILURE);

    pstScrollText = &(s_stTextLayer[u32TextLayerId].stScrollText[u32TextId]);
    pstScrollText->stCachebuf[0].bInusing = HI_FALSE;

    if (pstScrollText->bAvailable) {
        pstScrollText->s32TdeBlitHandle = 0;
        pstScrollText->bBliting = HI_FALSE;

        pstScrollText->u32IdleFlag = 1;
        osal_wait_wakeup(&(pstScrollText->wbEvent));
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/***************************************************************************************
 * func         : hifb_scrolltext_blit
 * description  : 滚动字幕blit
 * param[in]    :
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
static hi_void HIFB_SCROLLTEXT_BlitWork(osal_workqueue *work)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 i, j;
    hi_u32 u32StartAddr;
    HIFB_SCROLLTEXT_INFO_S *pstScrollTextInfo = NULL;
    HIFB_SCROLLTEXT_S *pstScrollText = NULL;
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    hi_u32 u32LayerID;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(work);
    pstScrollTextInfo = (HIFB_SCROLLTEXT_INFO_S *)container_of(work, HIFB_SCROLLTEXT_INFO_S, blitScrollTextWork);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstScrollTextInfo);

    u32LayerID = pstScrollTextInfo - &s_stTextLayer[0];

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(u32LayerID);
    info = s_stLayer[u32LayerID].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);

    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    u32StartAddr = pstPar->stRunInfo.CurScreenAddr;

    if (s_stTextLayer[u32LayerID].bAvailable) {
        for (i = 0; i < SCROLLTEXT_NUM; i++) {
            j = 0;
            pstScrollText = &(pstScrollTextInfo->stScrollText[i]);

            if (pstScrollText->bAvailable && !pstScrollText->bPause && pstScrollText->stCachebuf[j].bInusing &&
                !pstScrollText->bBliting) {
                HIFB_BUFFER_S stTempBuf, stCanvasBuf;
                HIFB_BLIT_OPT_S stBlitOpt;

                memset(&stBlitOpt, 0, sizeof(stBlitOpt));

                stCanvasBuf.stCanvas.u32PhyAddr = pstScrollText->stCachebuf[j].u32PhyAddr;
                stCanvasBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
                stCanvasBuf.stCanvas.u32Width = pstScrollText->stRect.w;
                stCanvasBuf.stCanvas.u32Height = pstScrollText->stRect.h;
                stCanvasBuf.stCanvas.u32Pitch = pstScrollText->u32Stride;

                stCanvasBuf.UpdateRect.x = 0;
                stCanvasBuf.UpdateRect.y = 0;
                stCanvasBuf.UpdateRect.w = pstScrollText->stRect.w;
                stCanvasBuf.UpdateRect.h = pstScrollText->stRect.h;

                stTempBuf.stCanvas.u32PhyAddr = u32StartAddr;
                stTempBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
                stTempBuf.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
                stTempBuf.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
                stTempBuf.stCanvas.u32Pitch = info->fix.line_length;

                if (pstScrollText->stRect.x >= pstPar->stExtendInfo.stPos.s32XPos) {
                    stTempBuf.UpdateRect.x = pstScrollText->stRect.x - pstPar->stExtendInfo.stPos.s32XPos;
                }

                if (pstScrollText->stRect.y >= pstPar->stExtendInfo.stPos.s32YPos) {
                    stTempBuf.UpdateRect.y = pstScrollText->stRect.y - pstPar->stExtendInfo.stPos.s32YPos;
                }

                stTempBuf.UpdateRect.w = pstScrollText->stRect.w;
                stTempBuf.UpdateRect.h = pstScrollText->stRect.h;

                if (stTempBuf.stCanvas.u32Width != stCanvasBuf.stCanvas.u32Width ||
                    stTempBuf.stCanvas.u32Height != stCanvasBuf.stCanvas.u32Height) {
                    stBlitOpt.bScale = HI_TRUE;
                }

                if (pstScrollText->bDeflicker && pstPar->stBaseInfo.enAntiflickerMode == HIFB_ANTIFLICKER_TDE) {
                    stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
                }

                stBlitOpt.bBlock = HI_FALSE;
                stBlitOpt.bCallBack = HI_TRUE;
                stBlitOpt.pfnCallBack = (IntCallBack)HIFB_SCROLLTEXT_CallBack;
                stBlitOpt.pParam = &(pstScrollText->enHandle);
                pstScrollText->bBliting = HI_TRUE;

                pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
                pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
                GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit);
                Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(&stCanvasBuf, &stTempBuf, &stBlitOpt, HI_TRUE);
                if (Ret <= 0) {
                    GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_Blit, Ret);
                    return;
                }

                pstScrollText->s32TdeBlitHandle = Ret;
            }
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 DRV_HIFB_SCROLLTEXT_Blit(hi_u32 u32LayerId)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(u32LayerId, HI_FAILURE);

    if (s_stTextLayer[u32LayerId].bAvailable) {
        osal_workqueue_schedule(&(s_stTextLayer[u32LayerId].blitScrollTextWork));
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

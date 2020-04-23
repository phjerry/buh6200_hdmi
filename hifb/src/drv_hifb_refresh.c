/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb refresh
 * Author: sdk
 * Create: 2016-01-01
 */

#include <linux/hisilicon/securec.h>
#include "hi_osal.h"

#include "drv_hifb_type.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_config.h"
#include "drv_hifb_fence.h"
#include "drv_hifb_debug.h"
#include "drv_hifb_param_convert.h"
/* **************************** Macro Definition ************************************************** */

/* ************************** Structure Definition ************************************************ */
#ifdef CONFIG_HIFB_FENCE_SUPPORT
typedef struct tagHifbRefreshWorkQueue_S {
    HIFB_PAR_S *pstPar;
    HIFB_HWC_LAYERINFO_S stLayerInfo;
    struct hifb_fence *pSyncfence;
    struct work_struct FenceRefreshWork;
} HIFB_REFRESH_WORKQUEUE_S;
#endif

/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */
static hi_s32 HIFB_REFRESH_PanDisplayForStereo(struct fb_var_screeninfo *var, struct fb_info *info);
static hi_void HIFB_REFRESH_PanDisplayForNoStero(struct fb_var_screeninfo *var, struct fb_info *info);
static hi_void HIFB_REFRESH_UpNonStereoADDR(struct fb_var_screeninfo *var, struct fb_info *info);
static hi_u32 HIFB_REFRESH_GetNonStereoADDR(struct fb_var_screeninfo *var, struct fb_info *info);

#ifdef CONFIG_HIFB_FENCE_SUPPORT
static hi_void HIFB_REFRESH_HwcWork(struct work_struct *work);
static hi_void HIFB_REFRESH_Flip(HIFB_REFRESH_WORKQUEUE_S *pstWork);
#endif

#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
static hi_void HIFB_REFRESH_SetLowPowerInfo(HIFB_PAR_S *pstPar, HIFB_HWC_LAYERINFO_S *pstHwcLayerInfo);
#endif

/* ****************************** API realization ************************************************* */
hi_s32 DRV_HIFB_REFRESH_PanDisplay(struct fb_var_screeninfo *var, struct fb_info *info)
{
    hi_u32 NowTimeMs = 0;
    hi_u32 EndTimeMs = 0;
    hi_ulong StereoLockFlag = 0;
    HIFB_PAR_S *pstPar = NULL;

    HI_GFX_TINIT();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* **************************beg check par ********************************* */
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(var, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);
    if (HIFB_REFRESH_MODE_WITH_PANDISPLAY != pstPar->stExtendInfo.enBufMode) {
        return HI_SUCCESS;
    }
    /* **************************end check par ********************************* */

    /* **************************beg count times ********************************* */
    HI_GFX_TSTART(NowTimeMs);
    pstPar->stFrameInfo.TwiceRefreshTimeMs = NowTimeMs - pstPar->stFrameInfo.PreRefreshTimeMs;
    pstPar->stFrameInfo.PreRefreshTimeMs = NowTimeMs;
    /* **************************end count times ********************************* */

    DRV_HIFB_Lock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);
    if (HI_TRUE == pstPar->st3DInfo.IsStereo) {
        pstPar->st3DInfo.BegUpStereoInfo = HI_TRUE;
        DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);

        HIFB_REFRESH_PanDisplayForStereo(var, info);

        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
    DRV_HIFB_UnLock(&pstPar->st3DInfo.StereoLock, &StereoLockFlag);

    HIFB_REFRESH_PanDisplayForNoStero(var, info);

    if (pstPar->is_first_pandisplay == HI_FALSE) {
        pstPar->up_mute = HI_FALSE;
    }
    pstPar->is_first_pandisplay = HI_FALSE;
    pstPar->hifb_sync = HI_FALSE;
    pstPar->stFrameInfo.RefreshFrame++;

    /* **************************beg count times ********************************* */
    HI_GFX_TEND(EndTimeMs);
    pstPar->stFrameInfo.RunRefreshTimeMs = EndTimeMs - NowTimeMs;
    /* **************************end count times ********************************* */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 HIFB_REFRESH_PanDisplayForStereo(struct fb_var_screeninfo *var, struct fb_info *info)
{
#ifdef CONFIG_HIFB_STEREO_SUPPORT
    hi_s32 Ret = HI_SUCCESS;
    HIFB_PAR_S *pstPar = NULL;
    HIFB_BUFFER_S stInputDataBuf;
    HIFB_BLIT_OPT_S stBlitOpt;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(var, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, HI_FAILURE);

    if ((HIFB_STEREO_FRMPACKING == pstPar->st3DInfo.StereoMode) || (pstPar->st3DInfo.mem_info_3d.smmu_addr == 0)) {
        HIFB_REFRESH_UpNonStereoADDR(var, info);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    memset(&stInputDataBuf, 0x0, sizeof(stInputDataBuf));
    stInputDataBuf.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stInputDataBuf.stCanvas.u32Pitch = info->fix.line_length;
    stInputDataBuf.stCanvas.u32PhyAddr = HIFB_REFRESH_GetNonStereoADDR(var, info);
    stInputDataBuf.stCanvas.u32Width = info->var.xres;
    stInputDataBuf.stCanvas.u32Height = info->var.yres;

    stInputDataBuf.UpdateRect.x = 0;
    stInputDataBuf.UpdateRect.y = 0;
    stInputDataBuf.UpdateRect.w = stInputDataBuf.stCanvas.u32Width;
    stInputDataBuf.UpdateRect.h = stInputDataBuf.stCanvas.u32Height;

    pstPar->st3DInfo.st3DSurface.enFmt = stInputDataBuf.stCanvas.enFmt;
    pstPar->st3DInfo.st3DSurface.u32Width = stInputDataBuf.stCanvas.u32Width;
    pstPar->st3DInfo.st3DSurface.u32Height = stInputDataBuf.stCanvas.u32Height;

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(pstPar->stRunInfo.u32IndexForInt, CONFIG_HIFB_LAYER_BUFFER_MAX_NUM,
                                          HI_FAILURE);
    pstPar->st3DInfo.st3DSurface.u32PhyAddr = pstPar->st3DInfo.u32DisplayAddr[pstPar->stRunInfo.u32IndexForInt];

    memset(&stBlitOpt, 0, sizeof(stBlitOpt));
    stBlitOpt.bScale = HI_TRUE;
    stBlitOpt.bBlock = HI_TRUE;
    stBlitOpt.bRegionDeflicker = HI_TRUE;
    if (HIFB_ANTIFLICKER_TDE == pstPar->stBaseInfo.enAntiflickerMode) {
        stBlitOpt.enAntiflickerLevel = pstPar->stBaseInfo.enAntiflickerLevel;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    stBlitOpt.pParam = &(pstPar->stBaseInfo.u32LayerID);
    stBlitOpt.bCallBack = HI_TRUE;
    stBlitOpt.pfnCallBack = (IntCallBack)DRV_HIFB_BlitFinishCallBack;

    Ret = DRV_HIFB_UpStereoData(pstPar->stBaseInfo.u32LayerID, &stInputDataBuf, &stBlitOpt);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_UpStereoData, Ret);
        return HI_FAILURE;
    }

    /* * can relese stereo memory * */
    pstPar->st3DInfo.BegUpStereoInfo = HI_FALSE;
    osal_wait_wakeup(&pstPar->st3DInfo.WaiteFinishUpStereoInfoMutex);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#else
#endif

    return HI_SUCCESS;
}

static hi_void HIFB_REFRESH_PanDisplayForNoStero(struct fb_var_screeninfo *var, struct fb_info *info)
{
    HIFB_PAR_S *pstPar = NULL;
    hi_u32 CurDispAddr = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(var);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    HIFB_REFRESH_UpNonStereoADDR(var, info);

    if ((var->activate & FB_ACTIVATE_VBL) && (HI_TRUE == pstPar->bVblank)) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        DRV_HIFB_WaitVBlank(pstPar->stBaseInfo.u32LayerID);

        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerAddr(pstPar->stBaseInfo.u32LayerID, &CurDispAddr);
        if (CurDispAddr != pstPar->stRunInfo.CurScreenAddr) {
            DRV_HIFB_WaitVBlank(pstPar->stBaseInfo.u32LayerID);
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void HIFB_REFRESH_UpNonStereoADDR(struct fb_var_screeninfo *var, struct fb_info *info)
{
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    pstPar->stRunInfo.bModifying = HI_TRUE;
    pstPar->stRunInfo.CurScreenAddr = HIFB_REFRESH_GetNonStereoADDR(var, info);
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_STRIDE;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_INRECT;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_DISPLAYADDR;
    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_REFRESH;
    pstPar->stRunInfo.bModifying = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_u32 HIFB_REFRESH_GetNonStereoADDR(struct fb_var_screeninfo *var, struct fb_info *info)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_u32 u32Align = 0xf;
    hi_u32 PerPixDepth = 0;
    hi_u32 PixXOffset = 0;
    hi_u32 PixYOffset = 0;
    hi_u32 u32TmpSize = 0;
    hi_u32 StartDisplayAddr = 0;
    HIFB_PAR_S *pstPar = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(var, 0x0);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, 0x0);
    pstPar = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstPar, 0x0);

    if (var->xoffset > CONFIG_HIFB_LAYER_MAXWIDTH) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, var->xoffset);
        return info->fix.smem_start;
    }

    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_HEIGHT | HIFB_PARA_CHECK_STRIDE | HIFB_PARA_CHECK_BITSPERPIXEL, 0,
                                  var->yres, info->fix.line_length, 0, var->bits_per_pixel);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, var->bits_per_pixel);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, info->fix.line_length);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_PARA_CHECK_SUPPORT, Ret);
        return info->fix.smem_start;
    }

    PerPixDepth = DRV_HIFB_GetPixDepth(var->bits_per_pixel);
    PixXOffset = var->xoffset * PerPixDepth;
    PixYOffset = info->fix.line_length * var->yoffset;

    if (0 != pstPar->stRunInfo.LastScreenAddr) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return pstPar->stRunInfo.LastScreenAddr;
    }

    u32TmpSize = PixYOffset + PixXOffset;
    GRAPHIC_CHECK_U64_ADDITION_REVERSAL_RETURN(info->fix.smem_start, u32TmpSize, info->fix.smem_start);
    GRAPHIC_CHECK_ULONG_TO_UINT_REVERSAL_RETURN((info->fix.smem_start + u32TmpSize), info->fix.smem_start);

    if ((info->var.bits_per_pixel == 24) && ((info->var.xoffset != 0) || (info->var.yoffset != 0))) {
        StartDisplayAddr = (info->fix.smem_start + u32TmpSize) / 16 / 3;
        StartDisplayAddr = StartDisplayAddr * 16 * 3;
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    } else {
        StartDisplayAddr = (info->fix.smem_start + u32TmpSize) & (~u32Align);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "debug point");
    }

    u32TmpSize = HI_HIFB_GetMemSize(info->fix.line_length, var->yres);
    GRAPHIC_CHECK_U64_ADDITION_REVERSAL_RETURN(StartDisplayAddr, u32TmpSize, info->fix.smem_start);

    if ((StartDisplayAddr + u32TmpSize) > (info->fix.smem_start + info->fix.smem_len)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, StartDisplayAddr + u32TmpSize);
        return info->fix.smem_start;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return StartDisplayAddr;
}

#ifdef CONFIG_HIFB_FENCE_SUPPORT
hi_s32 drv_hifb_refresh_flip(HIFB_PAR_S *par, hifb_layerinfo *layer_info, hi_void *args)
{
    hi_s32 fence_fd = -1;
    HIFB_REFRESH_WORKQUEUE_S *work_queue = NULL;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, -EFAULT);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(layer_info, -EFAULT);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(args, -EFAULT);

    if (par->pFenceRefreshWorkqueue == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, par->pFenceRefreshWorkqueue);
        return -EFAULT;
    }

    work_queue = (HIFB_REFRESH_WORKQUEUE_S *)HI_GFX_KMALLOC(HIGFX_FB_ID, sizeof(HIFB_REFRESH_WORKQUEUE_S), GFP_KERNEL);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(work_queue, -EFAULT);

    hifb_convert_layer_info_unf_to_drv(layer_info, &work_queue->stLayerInfo);

    work_queue->stLayerInfo.u32LayerAddr = drv_hifb_mem_get_smmu_from_fd(work_queue->stLayerInfo.mem_handle);

#ifdef CONFIG_GFX_PROC_SUPPORT
    par->stProcInfo.b3DStatus = par->st3DInfo.IsStereo;
    par->stProcInfo.compress_mode = work_queue->stLayerInfo.compress_mode;
    par->stProcInfo.HwcRefreshInDispFmt = (hi_u32)work_queue->stLayerInfo.eFmt;
    par->stProcInfo.HwcRefreshInDispStride = work_queue->stLayerInfo.u32Stride;
    par->stProcInfo.HwcRefreshInDispAdress = work_queue->stLayerInfo.u32LayerAddr;
#endif

    fence_fd = drv_hifb_fence_create(par->stBaseInfo.u32LayerID);
    if (fence_fd < 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_fence_create, fence_fd);
        HI_GFX_KFREE(HIGFX_FB_ID, work_queue);
        return -EFAULT;
    }

    work_queue->pstPar = par;
    layer_info->release_fd = fence_fd;

    if (work_queue->stLayerInfo.s32AcquireFenceFd >= 0) {
        work_queue->pSyncfence = drv_hifb_fd_get_fence(work_queue->stLayerInfo.s32AcquireFenceFd);
    } else {
        work_queue->pSyncfence = NULL;
    }

    work_queue->stLayerInfo.bStereo = par->st3DInfo.IsStereo;
    if (osal_copy_to_user(args, layer_info, sizeof(hifb_layerinfo))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        put_unused_fd(fence_fd);
        HI_GFX_KFREE(HIGFX_FB_ID, work_queue);
        return -EFAULT;
    }

    par->bEndFenceRefresh = HI_FALSE;
    INIT_WORK(&(work_queue->FenceRefreshWork), HIFB_REFRESH_HwcWork);
    queue_work(par->pFenceRefreshWorkqueue, &(work_queue->FenceRefreshWork));

    par->bEndFenceRefresh = HI_FALSE;

    return HI_SUCCESS;
}

static hi_void HIFB_REFRESH_HwcWork(struct work_struct *work)
{
    HIFB_PAR_S *pstPar = NULL;
    HIFB_REFRESH_WORKQUEUE_S *pstWork = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(work);
    pstWork = (HIFB_REFRESH_WORKQUEUE_S *)container_of(work, HIFB_REFRESH_WORKQUEUE_S, FenceRefreshWork);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstWork);

    /**<-- waite draw fence -->**/
    if (NULL != pstWork->pSyncfence) {
        drv_hifb_fence_wait(pstWork->pSyncfence, 4000); /* 4000 ms */
        drv_hifb_fence_put(pstWork->pSyncfence);
        pstWork->pSyncfence = NULL;
    }

    pstPar = pstWork->pstPar;
    if (NULL == pstPar) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "pstPar is null");
        HI_GFX_KFREE(HIGFX_FB_ID, pstWork);
        return;
    }

    HIFB_REFRESH_Flip(pstWork);

    HI_GFX_KFREE(HIGFX_FB_ID, pstWork);

    pstPar->up_mute = HI_FALSE;
    pstPar->bEndFenceRefresh = HI_TRUE;
    osal_wait_wakeup(&pstPar->WaiteEndFenceRefresh);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void HIFB_REFRESH_Flip(HIFB_REFRESH_WORKQUEUE_S *pstWork)
{
    hi_bool bDispEnable = HI_FALSE;
    hi_ulong LockParFlag = 0;
    hi_bool bShouldClosePreMult = HI_FALSE;
    HIFB_PAR_S *pstPar = NULL;
    struct fb_info *info = NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstWork);
    pstPar = pstWork->pstPar;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(pstPar->stBaseInfo.u32LayerID);
    info = s_stLayer[pstPar->stBaseInfo.u32LayerID].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);

    if ((pstWork->stLayerInfo.compress_mode != DRV_HIFB_CMP_MODE_NONE) && (pstPar->st3DInfo.IsStereo == HI_TRUE)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstWork->stLayerInfo.compress_mode);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstPar->st3DInfo.IsStereo);
        return;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetClosePreMultState(pstPar->stBaseInfo.u32LayerID, &bShouldClosePreMult);
    if (HI_TRUE == bShouldClosePreMult) {
        pstPar->stBaseInfo.bPreMul = HI_FALSE;
    } else {
        pstPar->stBaseInfo.bPreMul = pstWork->stLayerInfo.bPreMul;
    }

    pstPar->stRunInfo.LastScreenAddr = pstWork->stLayerInfo.u32LayerAddr;
    pstPar->stExtendInfo.enColFmt = pstWork->stLayerInfo.eFmt;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDataFmt(pstPar->stBaseInfo.u32LayerID, pstWork->stLayerInfo.eFmt);

    if (HI_TRUE == pstPar->st3DInfo.IsStereo) {
        pstPar->compress_mode = DRV_HIFB_CMP_MODE_NONE;
        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetDeCmpSwitch(pstPar->stBaseInfo.u32LayerID, HI_FALSE);

        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, pstWork->stLayerInfo.u32Stride);
        GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_NOVALUE(info->fix.smem_start, pstWork->stLayerInfo.u32LayerAddr);
        info->var.yoffset = (pstWork->stLayerInfo.u32LayerAddr - info->fix.smem_start) / pstWork->stLayerInfo.u32Stride;

        DRV_HIFB_REFRESH_PanDisplay(&info->var, info);

        g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetHaltDispStatus(pstPar->stBaseInfo.u32LayerID, &bDispEnable);

        drv_hifb_fence_inc_refresh_time(bDispEnable, pstPar->stBaseInfo.u32LayerID);

        return;
    }
#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
    HIFB_REFRESH_SetLowPowerInfo(pstPar, &(pstWork->stLayerInfo));
#endif

    DRV_HIFB_Lock(&pstPar->stBaseInfo.lock, &LockParFlag);
    if (memcpy_s(&pstPar->hwc_layer_info[pstPar->windex], sizeof(HIFB_HWC_LAYERINFO_S), &pstWork->stLayerInfo,
                 sizeof(pstWork->stLayerInfo)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    pstPar->windex++;
    pstPar->windex = pstPar->windex % CONFIG_HIFB_LAYER_BUFFER_MAX_NUM;
    pstPar->hwc_refresh_cnt++;
    DRV_HIFB_UnLock(&pstPar->stBaseInfo.lock, &LockParFlag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#else
static hi_s32 drv_hifb_refresh_setreo(HIFB_PAR_S *par, HIFB_HWC_LAYERINFO_S *drv_layer_info)
{
    struct fb_info *info = NULL;

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(par->stBaseInfo.u32LayerID, -EFAULT);

    info = s_stLayer[par->stBaseInfo.u32LayerID].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);

    par->compress_mode = DRV_HIFB_CMP_MODE_NONE;
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetDeCmpSwitch(par->stBaseInfo.u32LayerID, HI_FALSE);

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, drv_layer_info->u32Stride, -EFAULT);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(info->fix.smem_start, drv_layer_info->u32LayerAddr, HI_FAILURE);
    info->var.yoffset = (drv_layer_info->u32LayerAddr - info->fix.smem_start) / drv_layer_info->u32Stride;

    return DRV_HIFB_REFRESH_PanDisplay(&info->var, info);
}

static hi_void drv_hifb_refresh_to_proc(HIFB_PAR_S *par, HIFB_HWC_LAYERINFO_S *drv_layer_info)
{
#ifdef CONFIG_GFX_PROC_SUPPORT
    par->stProcInfo.b3DStatus = par->st3DInfo.IsStereo;
    par->stProcInfo.compress_mode = drv_layer_info->compress_mode;
    par->stProcInfo.HwcRefreshInDispFmt = (hi_u32)drv_layer_info->eFmt;
    par->stProcInfo.HwcRefreshInDispStride = drv_layer_info->u32Stride;
    par->stProcInfo.HwcRefreshInDispAdress = drv_layer_info->u32LayerAddr;
#endif
}

hi_s32 drv_hifb_refresh_flip(HIFB_PAR_S *par, hifb_layerinfo *layer_info, hi_void *args)
{
    hi_ulong lock_flags = 0;
    HIFB_HWC_LAYERINFO_S drv_layer_info = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(layer_info, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(args, HI_FAILURE);

    hifb_convert_layer_info_unf_to_drv(layer_info, &drv_layer_info);

    drv_layer_info.u32LayerAddr = drv_hifb_mem_get_smmu_from_fd(drv_layer_info.mem_handle);

    drv_hifb_refresh_to_proc(par, &drv_layer_info);

    layer_info->is_stereo = par->st3DInfo.IsStereo;
    if (osal_copy_to_user(args, layer_info, sizeof(hifb_layerinfo))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_copy_to_user, FAILURE_TAG);
        return HI_FAILURE;
    }

    if ((drv_layer_info.compress_mode != DRV_HIFB_CMP_MODE_NONE) && (par->st3DInfo.IsStereo == HI_TRUE)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "decompress and stereo only support one");
        return HI_FAILURE;
    }

    par->stRunInfo.LastScreenAddr = drv_layer_info.u32LayerAddr;
    par->stExtendInfo.enColFmt = drv_layer_info.eFmt;

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetLayerDataFmt(par->stBaseInfo.u32LayerID, drv_layer_info.eFmt);

    if (par->st3DInfo.IsStereo == HI_TRUE) {
        return drv_hifb_refresh_setreo(par, &drv_layer_info);
    }

#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
    HIFB_REFRESH_SetLowPowerInfo(par, &drv_layer_info);
#endif

    DRV_HIFB_Lock(&par->stBaseInfo.lock, &lock_flags);
    if (memcpy_s(&par->hwc_layer_info[par->windex], sizeof(HIFB_HWC_LAYERINFO_S), &drv_layer_info,
                 sizeof(HIFB_HWC_LAYERINFO_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    par->windex++;
    par->windex = par->windex % CONFIG_HIFB_LAYER_BUFFER_MAX_NUM;
    par->hwc_refresh_cnt++;
    DRV_HIFB_UnLock(&par->stBaseInfo.lock, &lock_flags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
static hi_void HIFB_REFRESH_SetLowPowerInfo(HIFB_PAR_S *pstPar, HIFB_HWC_LAYERINFO_S *pstHwcLayerInfo)
{
    hi_s32 Index = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstHwcLayerInfo);

    pstPar->stLowPowerInfo.LowPowerEn = pstHwcLayerInfo->stLowPowerInfo.LowPowerEn;

    for (Index = 0; Index < CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT; Index++) {
        pstPar->stLowPowerInfo.LpInfo[Index] = pstHwcLayerInfo->stLowPowerInfo.LpInfo[Index];
    }

    pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_LOWPOWER;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

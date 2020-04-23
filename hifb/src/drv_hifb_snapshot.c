/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb snapshot
 * Author: sdk
 * Create: 2016-01-01
 */

/* ********************************add include here************************************************ */
#if defined(CONFIG_GFX_PROC_SUPPORT) || defined(CONFIG_HIFB_CAPTURE_BMP_SUPPORT)
#include "hi_osal.h"

#include "drv_hifb_type.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_mem.h"
#include "drv_hifb_proc.h"
#include "hi_drv_file.h"
#include "drv_hifb_debug.h"
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
#include "drv_hifb_wbc.h"
#endif
#include <linux/hisilicon/securec.h>

/* **************************** Macro Definition ************************************************** */
#ifdef CFG_HIFB_ANDROID_SUPPORT
#define CONFIG_HIFB_SAVE_DIR "/data"
#else
#define CONFIG_HIFB_SAVE_DIR "/tmp"
#endif
#endif

#if defined(CONFIG_GFX_PROC_SUPPORT) && defined(CONFIG_HIFB_CAPTURE_BMP_SUPPORT)
/* ************************** Structure Definition ************************************************ */
typedef struct {
    hi_u16 u16Type;      /* 文件类型，设为0x4D42                   */
    hi_u32 u32Size;      /* 文件大小，像素数据加上头文件大小sizeof */
    hi_u16 u16Reserved1; /* 保留位                                 */
    hi_u16 u16Reserved2; /* 保留位                                 */
    hi_u32 u32OffBits;   /* 文件头到实际位图数据的偏移量           */
} __attribute__((packed)) BMP_BMFHEADER_S;

typedef struct tagBITMAPINFOHEADER {
    hi_u32 u32Size;          /* 位图信息头的大小,sizeof(BMP_BMIHEADER_S) */
    hi_u32 u32Width;         /* 图像宽度                                 */
    hi_u32 u32Height;        /* 图像高度                                 */
    hi_u16 u32Planes;        /* 位图位面数，设为1                        */
    hi_u16 u32PixbitCount;   /* 每个像素的位数，如RGB8888就是32          */
    hi_u32 u32Compression;   /* 位图数据压缩类型，设为0，表示不会压缩    */
    hi_u32 u32SizeImage;     /* 位图数据大小，设为0                      */
    hi_u32 u32XPelsPerMeter; /* 位图水平分辨率，与图像宽度相同           */
    hi_u32 u32YPelsPerMeter; /* 位图垂直分辨率，与图像高度相同           */
    hi_u32 u32ClrUsed;       /* 说明位图实际使用的彩色表中的颜色索引数，设为0 */
    hi_u32 u32ClrImportant;  /* 对图像显示很重要的颜色索引数，设为0      */
} BMP_BMIHEADER_S;

/* ********************* Global Variable declaration ********************************************** */

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
extern DRV_HIFB_WBC_GFX_S g_stGfxWbc2;
#endif
extern HIFB_DRV_TDEOPS_S g_stGfx2dCallBackFunction;

/* ****************************** API declaration ************************************************* */

extern hi_u32 DRV_HIFB_MEM_GetBppByFmt(DRV_HIFB_COLOR_FMT_E enColorFmt);

/* ****************************** API realization ************************************************* */
static inline hi_void HIFB_SnapshotPrint(HIFB_BUFFER_S *pstSrcBuffer, HIFB_BUFFER_S *pstDstBuffer)
{
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstSrcBuffer->stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSrcBuffer->stCanvas.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSrcBuffer->stCanvas.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSrcBuffer->stCanvas.u32Pitch);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstDstBuffer->stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstDstBuffer->stCanvas.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstDstBuffer->stCanvas.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstDstBuffer->stCanvas.u32Pitch);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    return;
}

static inline hi_s32 HIFB_SNAPSHOT_CopyDispbufToSaveBuf(HIFB_PAR_S *pstPar, hi_u32 CmpStride,
                                                        HIFB_BUFFER_S *pstSrcBuffer, HIFB_BUFFER_S *pstDstBuffer,
                                                        hi_bool CaptureData)
{
    hi_s32 Ret = HI_SUCCESS;
    HIFB_BUFFER_S stSrcBuffer;
    HIFB_BUFFER_S stDstBuffer;
    HIFB_BLIT_OPT_S stBlitOpt;
#ifndef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
    hi_u32 HeadSize = 0;
    hi_u32 ARHeadDdr = 0;
    hi_u32 ARDataDdr = 0;
    hi_u32 GBHeadDdr = 0;
    hi_u32 GBDataDdr = 0;
#endif
    memset_s(&stSrcBuffer, sizeof(stSrcBuffer), 0, sizeof(stSrcBuffer));
    memset_s(&stDstBuffer, sizeof(stDstBuffer), 0, sizeof(stDstBuffer));
    if (memcpy_s(&stSrcBuffer, sizeof(HIFB_BUFFER_S), pstSrcBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    if (memcpy_s(&stDstBuffer, sizeof(HIFB_BUFFER_S), pstDstBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }

    memset_s(&stBlitOpt, sizeof(stBlitOpt), 0, sizeof(stBlitOpt));
    stBlitOpt.bBlock = HI_TRUE;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy, HI_FAILURE);

    if (HI_TRUE == CaptureData) {
        Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy(&stSrcBuffer, &stDstBuffer, HI_FALSE);
        return (Ret < 0) ? (HI_FAILURE) : (HI_SUCCESS);
    }

    if (pstPar->compress_mode == DRV_HIFB_CMP_MODE_NONE) {
        pstPar->stFrameInfo.bBlitBlock = stBlitOpt.bBlock;
        pstPar->stFrameInfo.BlockTime = CONFIG_BLOCK_TIME;
        Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_Blit(&stSrcBuffer, &stDstBuffer, &stBlitOpt, HI_TRUE);
        HIFB_SnapshotPrint(&stSrcBuffer, &stDstBuffer);
        return (Ret < 0) ? (HI_FAILURE) : (HI_SUCCESS);
    }

#ifdef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy(&stSrcBuffer, &stDstBuffer, HI_FALSE);
#else
    Ret = HIFB_PARA_CHECK_SUPPORT(HIFB_PARA_CHECK_HEIGHT | HIFB_PARA_CHECK_STRIDE, 0, stDstBuffer.stCanvas.u32Height,
                                  CmpStride, 0, 0);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_PARA_CHECK_SUPPORT, Ret);
        return HI_FAILURE;
    }

    HeadSize = (16 * stDstBuffer.stCanvas.u32Height + 0xff) & 0xffffff00;
    ARHeadDdr = stDstBuffer.stCanvas.u32PhyAddr;

    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(ARHeadDdr, HeadSize, HI_FAILURE);
    ARDataDdr = ARHeadDdr + HeadSize;

    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(ARHeadDdr, (CmpStride * stDstBuffer.stCanvas.u32Height), HI_FAILURE);
    GBHeadDdr = ARDataDdr + CmpStride * stDstBuffer.stCanvas.u32Height;

    GRAPHIC_CHECK_INT_ADDITION_REVERSAL_RETURN(GBHeadDdr, HeadSize, HI_FAILURE);
    GBDataDdr = GBHeadDdr + HeadSize;

    stSrcBuffer.stCanvas.u32PhyAddr = pstPar->stRunInfo.CurScreenAddr;
    stSrcBuffer.stCanvas.u32Width = 4;
    stSrcBuffer.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stSrcBuffer.stCanvas.u32Pitch = 16;
    stSrcBuffer.stCanvas.enFmt = DRV_HIFB_FMT_ARGB8888;
    stSrcBuffer.UpdateRect.x = 0;
    stSrcBuffer.UpdateRect.y = 0;
    stSrcBuffer.UpdateRect.w = stSrcBuffer.stCanvas.u32Width;
    stSrcBuffer.UpdateRect.h = stSrcBuffer.stCanvas.u32Height;

    if (memcpy_s(&stDstBuffer, sizeof(HIFB_BUFFER_S), &stSrcBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    stDstBuffer.stCanvas.u32PhyAddr = ARHeadDdr;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stSrcBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stDstBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Pitch);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy(&stSrcBuffer, &stDstBuffer, HI_FALSE);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_QuickCopy, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(stSrcBuffer.stCanvas.u32PhyAddr, HeadSize, HI_FAILURE);
    stSrcBuffer.stCanvas.u32PhyAddr = stSrcBuffer.stCanvas.u32PhyAddr + HeadSize;

    stSrcBuffer.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stSrcBuffer.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stSrcBuffer.stCanvas.u32Pitch = CmpStride;
    stSrcBuffer.stCanvas.enFmt = DRV_HIFB_FMT_ARGB1555;
    stSrcBuffer.UpdateRect.x = 0;
    stSrcBuffer.UpdateRect.y = 0;
    stSrcBuffer.UpdateRect.w = stSrcBuffer.stCanvas.u32Width;
    stSrcBuffer.UpdateRect.h = stSrcBuffer.stCanvas.u32Height;

    if (memcpy_s(&stDstBuffer, sizeof(HIFB_BUFFER_S), &stSrcBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    stDstBuffer.stCanvas.u32PhyAddr = ARDataDdr;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stSrcBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stDstBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Pitch);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy(&stSrcBuffer, &stDstBuffer, HI_FALSE);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_QuickCopy, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(stSrcBuffer.stCanvas.u32PhyAddr,
                                                      (CmpStride * stSrcBuffer.stCanvas.u32Height), HI_FAILURE);
    stSrcBuffer.stCanvas.u32PhyAddr = stSrcBuffer.stCanvas.u32PhyAddr + CmpStride * stSrcBuffer.stCanvas.u32Height;

    stSrcBuffer.stCanvas.u32Width = 4;
    stSrcBuffer.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stSrcBuffer.stCanvas.u32Pitch = 16;
    stSrcBuffer.stCanvas.enFmt = DRV_HIFB_FMT_ARGB8888;
    stSrcBuffer.UpdateRect.x = 0;
    stSrcBuffer.UpdateRect.y = 0;
    stSrcBuffer.UpdateRect.w = stSrcBuffer.stCanvas.u32Width;
    stSrcBuffer.UpdateRect.h = stSrcBuffer.stCanvas.u32Height;

    if (memcpy_s(&stDstBuffer, sizeof(HIFB_BUFFER_S), &stSrcBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    stDstBuffer.stCanvas.u32PhyAddr = GBHeadDdr;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stSrcBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stDstBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Pitch);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy(&stSrcBuffer, &stDstBuffer, HI_FALSE);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_QuickCopy, Ret);
        return HI_FAILURE;
    }

    GRAPHIC_CHECK_UINT_MULTIPLICATION_REVERSAL_RETURN(stSrcBuffer.stCanvas.u32PhyAddr, HeadSize, HI_FAILURE);
    stSrcBuffer.stCanvas.u32PhyAddr = stSrcBuffer.stCanvas.u32PhyAddr + HeadSize;

    stSrcBuffer.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stSrcBuffer.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stSrcBuffer.stCanvas.u32Pitch = CmpStride;
    stSrcBuffer.stCanvas.enFmt = DRV_HIFB_FMT_ARGB1555;
    stSrcBuffer.UpdateRect.x = 0;
    stSrcBuffer.UpdateRect.y = 0;
    stSrcBuffer.UpdateRect.w = stSrcBuffer.stCanvas.u32Width;
    stSrcBuffer.UpdateRect.h = stSrcBuffer.stCanvas.u32Height;

    if (memcpy_s(&stDstBuffer, sizeof(HIFB_BUFFER_S), &stSrcBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    stDstBuffer.stCanvas.u32PhyAddr = GBDataDdr;

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stSrcBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, (unsigned long)stDstBuffer.stCanvas.u32PhyAddr);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stSrcBuffer.stCanvas.u32Pitch);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===========================================");
    Ret = g_stGfx2dCallBackFunction.DRV_HIFB_GFX2D_QuickCopy(&stSrcBuffer, &stDstBuffer, HI_FALSE);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_GFX2D_QuickCopy, Ret);
        return HI_FAILURE;
    }
#endif

    return Ret;
}

static inline hi_void HIFB_SNAPSHOT_WriteDispDataToFile(hi_bool bDeCompress, hi_char *pData, hi_u32 BufSize,
                                                        hi_u32 Width, hi_u32 Height, hi_u32 Stride, hi_bool CaptureData)
{
    hi_u32 Row = 0;
    hi_s8 *pTemp = NULL;
    static hi_s32 Cnt = 0;
    struct file *pFileFd = NULL;
    BMP_BMFHEADER_S sBmpHeader;
    BMP_BMIHEADER_S sBmpInfoHeader;
    hi_char SaveFileName[HIFB_FILE_PATH_MAX_LEN] = {'\0'};
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pData);
    GRAPHIC_CHECK_EQUAL_ZERO_RETURN_NOVALUE(BufSize);
    GRAPHIC_CHECK_EQUAL_ZERO_RETURN_NOVALUE(Width);
    GRAPHIC_CHECK_EQUAL_ZERO_RETURN_NOVALUE(Height);
    GRAPHIC_CHECK_EQUAL_ZERO_RETURN_NOVALUE(Stride);

    memset_s(&sBmpHeader, sizeof(sBmpHeader), 0x0, sizeof(sBmpHeader));
    memset_s(&sBmpInfoHeader, sizeof(sBmpInfoHeader), 0x0, sizeof(sBmpInfoHeader));

    sBmpHeader.u16Type = 0x4D42;
    sBmpHeader.u32Size = BufSize + sizeof(BMP_BMFHEADER_S) + sizeof(BMP_BMIHEADER_S);
    sBmpHeader.u16Reserved1 = 0;
    sBmpHeader.u16Reserved2 = 0;
    sBmpHeader.u32OffBits = sizeof(BMP_BMFHEADER_S) + sizeof(BMP_BMIHEADER_S);

    sBmpInfoHeader.u32Size = sizeof(BMP_BMIHEADER_S);
    sBmpInfoHeader.u32Width = Width;
    sBmpInfoHeader.u32Height = Height;
    sBmpInfoHeader.u32Planes = 1;
    sBmpInfoHeader.u32PixbitCount = 24; /* 24 bits for RGB888 */
    sBmpInfoHeader.u32Compression = 0;
    sBmpInfoHeader.u32SizeImage = 0;
    sBmpInfoHeader.u32XPelsPerMeter = Width;
    sBmpInfoHeader.u32YPelsPerMeter = Height;
    sBmpInfoHeader.u32ClrUsed = 0;
    sBmpInfoHeader.u32ClrImportant = 0;

    Cnt++;
    if (Cnt > 100) {
        Cnt = 0;
    }

    if (HI_TRUE == CaptureData) {
        snprintf(SaveFileName, sizeof(SaveFileName), "%s/hifb_snapshot_%d_%d_%d_num%d.data", CONFIG_HIFB_SAVE_DIR,
                 Width, Height, Stride, Cnt);
    } else if (HI_TRUE == bDeCompress) {
        snprintf(SaveFileName, sizeof(SaveFileName), "%s/hifb_snapshot_%d_%d_%d_num%d.cmp", CONFIG_HIFB_SAVE_DIR, Width,
                 Height, Stride, Cnt);
    } else {
        snprintf(SaveFileName, sizeof(SaveFileName), "%s/hifb_snapshot_%d_%d_%d_num%d.bmp", CONFIG_HIFB_SAVE_DIR, Width,
                 Height, Stride, Cnt);
    }
    SaveFileName[sizeof(SaveFileName) - 1] = '\0';
    pFileFd = osal_klib_fopen(SaveFileName, OSAL_O_WRONLY | OSAL_O_CREAT | O_LARGEFILE, 0664); /* 0664 is user auth */
    if (IS_ERR(pFileFd)) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_klib_fopen, FAILURE_TAG);
        return;
    }

    if (HI_TRUE == bDeCompress) {
#ifdef CONFIG_HIFB_CALCSTRIDE_WITH_ONEPOINTTHREE
        for (Row = 0; Row < Height; Row++) {
            pTemp = pData + Row * Stride;
            hi_drv_file_write(pFileFd, (hi_s8 *)pTemp, Stride);
        }
#else
        hi_drv_file_write(pFileFd, (hi_s8 *)pData, BufSize);
#endif
        goto FINISH_SAVE;
    }

    if (HI_TRUE == CaptureData) { /* * save argb data * */
        hi_drv_file_write(pFileFd, (hi_s8 *)pData, BufSize);
        goto FINISH_SAVE;
    }

    if (sizeof(BMP_BMFHEADER_S) != hi_drv_file_write(pFileFd, (hi_s8 *)&sBmpHeader, sizeof(BMP_BMFHEADER_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_file_write, FAILURE_TAG);
        goto FINISH_SAVE;
    }

    if (sizeof(BMP_BMIHEADER_S) != hi_drv_file_write(pFileFd, (hi_s8 *)&sBmpInfoHeader, sizeof(BMP_BMIHEADER_S))) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_file_write, FAILURE_TAG);
        goto FINISH_SAVE;
    }

    Row = Height;
    pTemp = pData;
    pTemp += (Stride * (Height - 1));
    while (Row) {
        hi_drv_file_write(pFileFd, (hi_s8 *)pTemp, Width * 3); /* 3 for RGB888 */
        pTemp -= Stride;
        Row--;
    }

FINISH_SAVE:
    if (NULL != pFileFd) {
        hi_drv_file_close(pFileFd);
    }

    GRAPHIC_COMM_PRINT("success to capture fb, store in file %s\n", SaveFileName);
    return;
}

hi_void HI_UNF_HIFB_CaptureImgFromLayer(hi_u32 u32LayerID, hi_u32 LineLength, hi_bool CaptureData)
{
    struct fb_info *info = NULL;
    HIFB_PAR_S *pstPar = NULL;
    hi_s32 Ret = HI_SUCCESS;
    hi_s8 *pData = NULL;
    hi_u32 u32BufSize = 0;
    hi_u32 u32Bpp = 0;
    hi_u32 CmpStride = 0;
    hi_u32 Stride = 0;
    HIFB_BUFFER_S stSrcBuffer, stDstBuffer;
    hi_char MemZoneName[HIFB_FILE_NAME_MAX_LEN] = {'\0'};
    hi_void *dmabuf = HI_NULL;

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    hi_u32 WbcWriteAddr = 0;
    hi_u32 WbcStride = 0;

    if (OPTM_SLAVER_LAYERID == u32LayerID) {
        DRV_HIFB_HAL_WBC_GetLayerAddr(OPTM_VDP_LAYER_WBC_GP0, &WbcWriteAddr, &WbcStride);
        u32BufSize = WbcStride * g_stGfxWbc2.stOutRect.h;
        if (g_stGfxWbc2.wbc_buf_mgr.stFrameBuffer.virtualBuf != NULL) {
            pData = (hi_s8 *)(g_stGfxWbc2.wbc_buf_mgr.stFrameBuffer.virtualBuf +
                              (WbcWriteAddr - g_stGfxWbc2.wbc_buf_mgr.buf_info[0].buffer_addr));
        }
        HIFB_SNAPSHOT_WriteDispDataToFile(HI_FALSE, pData, u32BufSize, g_stGfxWbc2.stOutRect.w, g_stGfxWbc2.stOutRect.h,
                                          WbcStride, CaptureData);
        return;
    }
#endif
    memset_s(&stSrcBuffer, sizeof(stSrcBuffer), 0x0, sizeof(stSrcBuffer));
    memset_s(&stDstBuffer, sizeof(stDstBuffer), 0x0, sizeof(stDstBuffer));

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(u32LayerID);
    info = s_stLayer[u32LayerID].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);
    pstPar = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstPar);

    stSrcBuffer.stCanvas.enFmt = pstPar->stExtendInfo.enColFmt;
    stSrcBuffer.stCanvas.u32Width = pstPar->stExtendInfo.DisplayWidth;
    stSrcBuffer.stCanvas.u32Height = pstPar->stExtendInfo.DisplayHeight;
    stSrcBuffer.stCanvas.u32PhyAddr = pstPar->stRunInfo.CurScreenAddr;
    stSrcBuffer.stCanvas.u32Pitch = LineLength;
    if (HIFB_LAYER_BUF_NONE == pstPar->stExtendInfo.enBufMode) {
        stSrcBuffer.stCanvas.enFmt = pstPar->stDispInfo.stUserBuffer.stCanvas.enFmt;
        stSrcBuffer.stCanvas.u32Pitch = pstPar->stDispInfo.stUserBuffer.stCanvas.u32Pitch;
    }

    if ((HI_TRUE == pstPar->st3DInfo.IsStereo) && (HIFB_STEREO_FRMPACKING != pstPar->st3DInfo.StereoMode)) {
        if (memcpy_s(&stSrcBuffer.stCanvas, sizeof(stSrcBuffer.stCanvas), &pstPar->st3DInfo.st3DSurface,
                     sizeof(stSrcBuffer.stCanvas)) != EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
        }
    }

    stSrcBuffer.UpdateRect.x = 0;
    stSrcBuffer.UpdateRect.y = 0;
    stSrcBuffer.UpdateRect.w = stSrcBuffer.stCanvas.u32Width;
    stSrcBuffer.UpdateRect.h = stSrcBuffer.stCanvas.u32Height;

    if (memcpy_s(&stDstBuffer, sizeof(HIFB_BUFFER_S), &stSrcBuffer, sizeof(HIFB_BUFFER_S)) != EOK) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "memcpy_s failed\n");
    }
    if ((pstPar->compress_mode != DRV_HIFB_CMP_MODE_NONE) || (HI_TRUE == CaptureData)) {
        stDstBuffer.stCanvas.enFmt = DRV_HIFB_FMT_ARGB8888;
    } else {
        stDstBuffer.stCanvas.enFmt = DRV_HIFB_FMT_RGB888;
    }

    u32Bpp = DRV_HIFB_MEM_GetBppByFmt(stDstBuffer.stCanvas.enFmt);
    Stride = CONIFG_HIFB_GetMaxStride(stSrcBuffer.stCanvas.u32Width, u32Bpp, &CmpStride, CONFIG_HIFB_STRIDE_ALIGN);

    stDstBuffer.stCanvas.u32Pitch = (pstPar->compress_mode == DRV_HIFB_CMP_MODE_NONE) ? (Stride) : (CmpStride);

    u32BufSize = HI_HIFB_GetMemSize(Stride, stSrcBuffer.stCanvas.u32Height);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(u32BufSize, 0);

    snprintf(MemZoneName, sizeof(MemZoneName), "hifb%d_snapshot", u32LayerID);
    MemZoneName[sizeof(MemZoneName) - 1] = '\0';

    dmabuf = osal_mem_alloc(MemZoneName, u32BufSize, OSAL_NSSMMU_TYPE, HI_NULL, 0);
    osal_mem_flush(dmabuf);
    if (dmabuf == HI_NULL) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_mem_alloc, FAILURE_TAG);
        return;
    }
    stDstBuffer.stCanvas.u32PhyAddr = drv_hifb_mem_map_to_smmu(dmabuf);
    if (stDstBuffer.stCanvas.u32PhyAddr == 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_hifb_mem_map_to_smmu, FAILURE_TAG);
        goto ERR_EXIT;
    }

    pData = drv_hifb_mem_map(dmabuf);
    if (pData == HI_NULL) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, DRV_HIFB_MEM_Map, FAILURE_TAG);
        goto ERR_EXIT;
    }
    memset_s(pData, u32BufSize, 0x0, u32BufSize);

    Ret = HIFB_SNAPSHOT_CopyDispbufToSaveBuf(pstPar, CmpStride, &stSrcBuffer, &stDstBuffer, CaptureData);
    if (HI_SUCCESS != Ret) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HIFB_SNAPSHOT_CopyDispbufToSaveBuf, FAILURE_TAG);
        goto ERR_EXIT;
    }

    HIFB_SNAPSHOT_WriteDispDataToFile(pstPar->compress_mode != DRV_HIFB_CMP_MODE_NONE, pData, u32BufSize,
                                      stDstBuffer.stCanvas.u32Width, stDstBuffer.stCanvas.u32Height,
                                      stDstBuffer.stCanvas.u32Pitch, CaptureData);

ERR_EXIT:
    if (pData != HI_NULL) {
        drv_hifb_mem_unmap(dmabuf, pData);
    }

    if (dmabuf != HI_NULL) {
        drv_hifb_mem_free(dmabuf);
    }

    return;
}
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
extern HIFB_DRV_OPS_S g_stDrvAdpCallBackFunction;
hi_void DRV_HIFB_SNAPSHOT_CaptureRegister(hi_void)
{
    hi_u32 Index = 0;
    hi_u32 Loop = (CONFIG_VDP_SIZE) / 16;
    static hi_u32 Cnt = 0;
    hi_u32 *pRegBuf = NULL;
    struct file *pFileFd = NULL;
    hi_char pStr[HIFB_FILE_PATH_MAX_LEN] = {'\0'};
    hi_char SaveFileName[HIFB_FILE_PATH_MAX_LEN] = {'\0'};

    pRegBuf = (hi_u32 *)HI_GFX_VMALLOC(HIGFX_FB_ID, (CONFIG_VDP_SIZE));
    if (NULL == pRegBuf) {
        return;
    }

    memset_s(pRegBuf, CONFIG_VDP_SIZE, '\0', (CONFIG_VDP_SIZE));
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_ReadRegister(0, pRegBuf);

    Cnt = (Cnt >= 100) ? (0) : (Cnt + 1);
    snprintf(SaveFileName, sizeof(SaveFileName), "%s/hifb_snapshot_%d.data", CONFIG_HIFB_SAVE_DIR, Cnt);
    SaveFileName[sizeof(SaveFileName) - 1] = '\0';

    pFileFd = osal_klib_fopen(SaveFileName, OSAL_O_WRONLY | OSAL_O_CREAT | O_LARGEFILE, 0644); /* 0644 is user auth */
    if (IS_ERR(pFileFd)) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_klib_fopen, FAILURE_TAG);
        HI_GFX_VFREE(HIGFX_FB_ID, pRegBuf);
        return;
    }

    for (Index = 0; Index < Loop; Index++) {
        snprintf(pStr, sizeof(pStr), "%04x: %08x %08x %08x %08x\n", Index * 16, *(pRegBuf + Index * 4),
                 *(pRegBuf + Index * 4 + 1), *(pRegBuf + Index * 4 + 2), *(pRegBuf + Index * 4 + 3));
        hi_drv_file_write(pFileFd, (hi_s8 *)pStr, 42);
    }

    hi_drv_file_close(pFileFd);
    HI_GFX_VFREE(HIGFX_FB_ID, pRegBuf);

    GRAPHIC_COMM_PRINT("success to capture more, store in file %s\n", SaveFileName);
    return;
}
#endif

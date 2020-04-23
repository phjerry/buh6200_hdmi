/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:adp
 * Author: sdk
 * Create: 2019-02-20
 */
#ifndef __SOURCE_MSP_DRV_INCLUDE_HAL_INC_ADP__
#define __SOURCE_MSP_DRV_INCLUDE_HAL_INC_ADP__

#include "drv_gfx2d_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_u8 u8MaxLayerNum; /* Maximum number of support input layers */ /* CNcomment: 最大支持输入层个数 */
    hi_u8 u8MaxGLayerNum; /* Max graphic layer */                     /* CNcomment: 最大图形层个数 */
    hi_u8 u8MaxVLayerNum; /* Max video layer */                       /* CNcomment: 最大视频层个数 */
    hi_u32 au32InputFmtBitmap[2]; /* storage pixel format */          /* CNcomment: 用2个数组存放所有的像素格式 */
    hi_u32 au32OutputFmtBitmap[2]; /* storage pixel format */         /* CNcomment: 用2个数组存放所有的像素格式 */
    hi_u32 u32MinStride;
    hi_u32 u32MaxStride;
    hi_u8 u8StrideAlign;
    hi_u32 u32MinWidth;
    hi_u32 u32MaxWidth;
    hi_u32 u32MinHeight;
    hi_u32 u32MaxHeight;
    hi_u32 u32MinVWidth; /* min width of video layer */                     /* CNcomment: 视频层最小宽度 */
    hi_u32 u32MinVHeight; /* min height of video layer */                   /* 视频层最小高度 */
    hi_u32 u32MaxHorizonZmeRatio; /* maximum horizontal reduction factor */ /* CNcomment: 水平最大缩小倍数 */
    hi_u32 u32MaxVerticalZmeRatio; /* vertical maximum reduction factor */  /* CNcomment: 垂直最大缩小倍数 */
    hi_u8 u32CmpWidthAlign; /* compression width alignment requirements */  /* CNcomment: 压缩宽度对齐要求 */
    hi_u32 au32CmpFmtBitmap[2]; /* Compression supported formats */         /* CNcomment: 2个数组，压缩支持的格式 */
    hi_bool bGZmeSupport; /* Does the Graphics Layer Support Scaling? */    /* CNcomment: 图形层是否支持缩放 */
    hi_bool bClipSupport; /* Does Support clip */                           /* CNcomment: 是否支持clip */
    hi_u8 u8ZmeLayerNum;
    hi_u8 u8ZmeAlign;
} gfx2d_capability_info;

hi_void HWC_ADP_GetCapability(gfx2d_capability_info *pstCapability);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_INCLUDE_HAL_INC_ADP__ */

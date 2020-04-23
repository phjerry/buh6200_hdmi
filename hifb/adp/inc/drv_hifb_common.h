/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb common header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_COMMON_H__
#define __DRV_HIFB_COMMON_H__

#ifndef HI_BUILD_IN_BOOT
/* ********************************add include here*********************************************** */
#include "drv_hifb_type.h"
#include "drv_hifb_mem.h"

/*************************************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************* */

#define HIFB_HWC_REFRESH_WORK_QUEUE "HIFB_HWC_REFRESH_WorkQueque"
#define HIFB_WORK_QUEUE "HIFB_WorkQueque"
#define HIFB_FREE_FRAME_BUFFER_QUEUE "HIFB_FreeFrameBufferQuque"
#define HIFB_WBC_BUFFER "HIFB_WBC_Buffer"
#define HIFB_WBC_FIELD_BUFFER "HIFB_WBC_FIELD_Buffer"
#define GRAPHIC_LAYER_BUF_HD "GRAPHIC_HIFB_HD"
#define GRAPHIC_LAYER_BUF_SD "GRAPHIC_HIFB_SD"
#define HIFB_ZME_COEF_BUFFER "GFX_ZmeCoef"
#define OPTM_GFX_CMP_BUFFER "HIFB_GfxCmp"
#define OPTM_GFX_CMP_QUEUE "HIFB_CompressQueque"

typedef hi_s32 (*IntCallBack)(hi_void *pParaml, hi_void *pParamr);

#define IS_HD_LAYER(enLayerId) (enLayerId < HIFB_LAYER_HD_1)
#define IS_SD_LAYER(enLayerId) ((enLayerId >= HIFB_LAYER_SD_0) && (enLayerId < HIFB_LAYER_SD_1))
#define IS_MINOR_HD_LAYER(enLayerId) ((enLayerId <= HIFB_LAYER_HD_3) && (enLayerId >= HIFB_LAYER_HD_1))

/* ************************** Structure Definition *********************************************** */

typedef struct {
    hi_bool bKeyEnable;                               /* * colorkey enable flag * */  /* CNcomment:colorkey 是否使能 */
    hi_bool bMaskEnable;                              /* * key mask enable flag * */ /* CNcomment:key mask 是否使能 */
    hi_u32 u32Key;                                    /* * key value * */
    hi_u8 u8RedMask;                                  /* * red mask   * */
    hi_u8 u8GreenMask;                                /* * green mask * */
    hi_u8 u8BlueMask;                                 /* * blue mask  * */
    hi_u8 u8Reserved;
    hi_u32 u32KeyMode; /* 0:In region; 1:Out region */

    /* Max colorkey value of red component */
    /* CNcomment:colorkey红色分量最大值 */
    hi_u8 u8RedMax;

    /* Max colorkey value of Green component */
    /* CNcomment:colorkey绿色分量最大值 */
    hi_u8 u8GreenMax;

    /* Max colorkey value of blue component */
    /* CNcomment:colorkey蓝色分量最大值 */
    hi_u8 u8BlueMax;
    hi_u8 u8Reserved1;

    /* Min colorkey value of red component */
    /* CNcomment:colorkey红色分量最小值 */
    hi_u8 u8RedMin;

    /* Min colorkey value of Green component */
    /* CNcomment:colorkey绿色分量最小值 */
    hi_u8 u8GreenMin;

    /* Min colorkey value of blue component */
    /* CNcomment:colorkey蓝色分量最小值 */
    hi_u8 u8BlueMin;
    hi_u8 u8Reserved2;
} HIFB_COLORKEYEX_S;

typedef enum {
    /* * VO vertical timing interrupt * */
    /* * CNcomment:垂直时序中断       * */
    HIFB_CALLBACK_TYPE_VO = 0x1,
    /* * 3D Mode changed interrupt * */
    /* * CNcomment:3D模式改变中断  * */
    HIFB_CALLBACK_TYPE_3DMode_CHG = 0x2,
    /* * Frame end interrupt    * */
    /* * CNcomment:帧结束中断   * */
    HIFB_CALLBACK_TYPE_FRAME_END = 0x4,
    HIFB_CALLBACK_TYPE_BUTT,
} HIFB_CALLBACK_TPYE_E;

typedef enum {
    HIFB_LAYER_PARAMODIFY_FMT = 0x1,
    HIFB_LAYER_PARAMODIFY_STRIDE = 0x2,
    HIFB_LAYER_PARAMODIFY_ALPHA = 0x4,
    HIFB_LAYER_PARAMODIFY_COLORKEY = 0x8,
    HIFB_LAYER_PARAMODIFY_INRECT = 0x10,
    HIFB_LAYER_PARAMODIFY_OUTRECT = 0x20,
    HIFB_LAYER_PARAMODIFY_DISPLAYADDR = 0x40,
    HIFB_LAYER_PARAMODIFY_SHOW = 0x80,
    HIFB_LAYER_PARAMODIFY_BMUL = 0x100,
    HIFB_LAYER_PARAMODIFY_ANTIFLICKERLEVEL = 0x200,
    HIFB_LAYER_PARAMODIFY_FENCE = 0x400,
    HIFB_LAYER_PARAMODIFY_REFRESH = 0x800,
    HIFB_LAYER_PARAMODIFY_LOWPOWER = 0x1000,
    HIFB_LAYER_PARAMODIFY_ALL = 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80 | 0x100 | 0x200 | 0x400 | 0x800 |
                                0x1000,
    HIFB_LAYER_PARAMODIFY_BUTT
} HIFB_LAYER_PARAMODIFY_MASKBIT_E;

/* scan mode */
typedef enum {
    HIFB_SCANMODE_P,
    HIFB_SCANMODE_I,
    HIFB_SCANMODE_BUTT,
} HIFB_SCAN_MODE_E;

/* layer state */
typedef enum {
    HIFB_LAYER_STATE_ENABLE = 0x0,
    /* Layer enable */ /* CNcomment:层使能 */
    HIFB_LAYER_STATE_DISABLE,
    /* Layer disable */ /* CNcomment:层未使能 */
    HIFB_LAYER_STATE_INVALID,
    /* Layer invalid */ /* CNcomment:层无效,不存在 */
    HIFB_LAYER_STATE_BUTT
} HIFB_LAYER_STATE_E;

/* GFX mode  */
typedef enum tagHIFB_GFX_MODE_EN {
    HIFB_GFX_MODE_NORMAL = 0,
    HIFB_GFX_MODE_HD_WBC,
    HIFB_GFX_MODE_BUTT
} HIFB_GFX_MODE_EN;

typedef struct {
    HIFB_LAYER_STATE_E eState;
    HIFB_GFX_MODE_EN eGfxWorkMode;
    hi_bool bPreMul;
    hi_bool bDePreMul;
    hi_u32 u32RegPhyAddr;
    hi_u32 u32Stride;
    hi_u32 DisplayWidth;
    hi_u32 DisplayHeight;
    HIFB_RECT stOutRect;
    HIFB_RECT stInRect;
    DRV_HIFB_COLOR_FMT_E eFmt;
    HIFB_ALPHA_S stAlpha;
    HIFB_COLORKEYEX_S stColorKey;
    HIFB_SCAN_MODE_E eScanMode;
    HIFB_LAYER_ID_E enSlaveryLayerID;
} HIFB_OSD_DATA_S;

typedef enum hiHIFB_LOGO_PIX_FMT_E {
    HIFB_LOGO_PIX_FMT_ARGB8888 = 0,
    HIFB_LOGO_PIX_FMT_ARGB1555,
    HIFB_LOGO_PIX_FMT_YUV400,
    HIFB_LOGO_PIX_FMT_YUV420,
    HIFB_LOGO_PIX_FMT_YUV422_21,
    HIFB_LOGO_PIX_FMT_YUV422_12,
    HIFB_LOGO_PIX_FMT_YUV444,
    HIFB_LOGO_PIX_FMT_BUTT
} HIFB_LOGO_PIX_FMT_E;

typedef struct {
    hi_bool Support4KLogo;
    hi_bool LogoEnable;
    hi_u32 LogoYAddr;
    hi_u32 LogoCbCrAddr;
    hi_u32 LogoWidth;
    hi_u32 LogoHeight;
    hi_u32 LogoYStride;
    hi_u32 LogoCbCrStride;
    HIFB_RECT stOutRect;
    HIFB_LOGO_PIX_FMT_E eLogoPixFmt;
} HIFB_LOGO_DATA_S;

typedef struct {
    hi_bool bShow;
    hi_bool bOpen;
    HIFB_GFX_MODE_EN eGfxWorkMode;
    hi_u32 u32Stride;
    hi_u32 u32WbcBufNum;
    hi_u32 u32WbcBufSize;
    hi_u32 u32ReadBufAddr;

    /* source buffer of write back */
    HIFB_RECT stSrcBufRect;
    /* current buffer of write back */
    HIFB_RECT stCurWBCBufRect;
    /* max buffer of write back */
    HIFB_RECT stMaxWbcBufRect;
    /* display region of write back */
    HIFB_RECT stScreenRect;

    DRV_HIFB_COLOR_FMT_E eFmt;
    HIFB_SCAN_MODE_E eScanMode;
    HIFB_LAYER_ID_E enLayerID;
} HIFB_SLVLAYER_DATA_S;
#endif

typedef enum {
    HIFB_PARA_CHECK_FMT = 0x1,
    HIFB_PARA_CHECK_WIDTH = 0x2,
    HIFB_PARA_CHECK_HEIGHT = 0x4,
    HIFB_PARA_CHECK_STRIDE = 0x8,
    HIFB_PARA_CHECK_BITSPERPIXEL = 0x10,
    HIFB_PARA_CHECK_BUTT
} HIFB_PARA_CHECK_MASKBIT_E;

/* ********************* Global Variable declaration ********************************************* */

/* ****************************** API declaration ************************************************ */

#ifdef __cplusplus

#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_HIFB_COMMON_H__ */

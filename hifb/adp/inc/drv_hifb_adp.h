/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb adp header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_ADP_H__
#define __DRV_HIFB_ADP_H__

/* ********************************add include here************************************************ */
#ifndef HI_BUILD_IN_BOOT
#include "hi_type.h"
#include "hi_drv_video.h"
// #include "hi_common.h"
#include "hi_drv_disp.h"
#include "drv_hifb_common.h"
#include "hi_gfx_comm_k.h"
#else
#include "hifb_debug.h"
#include "hi_drv_disp.h"
#include "hi_gfx_comm.h"
#endif

#include "drv_hifb_hal.h"
#include "optm_define.h"

#ifdef CONFIG_GFX_PQ
#include "drv_pq_ext.h"
#endif

/**************************************************************************************************/

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************** */
#define OPTM_ENABLE 1
#define OPTM_DISABLE 0

#define OPTM_MASTER_GPID HIFB_ADP_GP0
#define OPTM_SLAVER_GPID HIFB_ADP_GP1
#define OPTM_SLAVER_LAYERID HIFB_LAYER_SD_0

#define OPTM_CMAP_SIZE 0x400

#ifndef CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT
#define CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT 17
#endif

#define IS_MASTER_GP(enGpId) ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP0 == enGpId))
#define IS_SLAVER_GP(enGpId) ((HIFB_GFX_MODE_HD_WBC == g_enOptmGfxWorkMode) && (HIFB_ADP_GP1 == enGpId))

#define OPTM_CheckGPMask_BYGPID(enGPId) do { \
    if (enGPId >= HIFB_ADP_GP_BUTT) {                                                        \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enGPId);                                 \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HIFB_ADP_GP_BUTT);                  \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "enGPId >= HIFB_ADP_GP_BUTT"); \
        return HI_FAILURE;                                                                   \
    }                                                                                        \
    if (g_pstGfxGPDevice[enGPId]->bMaskFlag) {                                               \
        return HI_SUCCESS;                                                                   \
    }                                                                                        \
} while (0)

#define GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(LayerId, Ret)                          \
    if (LayerId >= HIFB_LAYER_ID_BUTT) {                                                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerId);                             \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HIFB_LAYER_ID_BUTT);             \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "layer id is not support"); \
        return Ret;                                                                       \
    }

#define GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerId)                             \
    if (LayerId >= HIFB_LAYER_ID_BUTT) {                                                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, LayerId);                             \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HIFB_LAYER_ID_BUTT);             \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "layer id is not support"); \
        return;                                                                           \
    }

#define GRAPHIC_CHECK_GPID_SUPPORT_RETURN_VALUE(GpId, Ret)                             \
    if (GpId >= HIFB_ADP_GP_BUTT) {                                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, GpId);                             \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "gp id is not support"); \
        return Ret;                                                                    \
    }

#define GRAPHIC_CHECK_GPID_SUPPORT_RETURN_NOVALUE(GpId)                                \
    if (GpId >= HIFB_ADP_GP_BUTT) {                                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, GpId);                             \
        GRAPHIC_DFX_MINI_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "gp id is not support"); \
        return;                                                                        \
    }

/* **************************  The enum of  ******************************************************* */
typedef enum tagHIFB_GP_ID_E {
    HIFB_ADP_GP0 = 0x0,
    HIFB_ADP_GP1,
    HIFB_ADP_GP_BUTT
} HIFB_GP_ID_E;

/* ************************** Structure Definition ************************************************ */
typedef struct tagOPTM_GFX_CSC_PARA_S {
    hi_u32 u32Bright;
    hi_u32 u32Contrast;
    hi_u32 u32Saturation;
    hi_u32 u32Hue;
    hi_u32 u32Kr;
    hi_u32 u32Kg;
    hi_u32 u32Kb;
} OPTM_GFX_CSC_PARA_S;

typedef struct tagOPTM_GFX_OFFSET_S {
    hi_u32 u32Left;   /* * left offset   */
    hi_u32 u32Top;    /* * top offset    */
    hi_u32 u32Right;  /* * right offset  */
    hi_u32 u32Bottom; /* * bottom offset */
} OPTM_GFX_OFFSET_S;

/* * csc state */
typedef enum tagOPTM_CSC_STATE_E {
    OPTM_CSC_SET_PARA_ENABLE = 0x0,
    OPTM_CSC_SET_PARA_RGB,
    OPTM_CSC_SET_PARA_BGR,
    OPTM_CSC_SET_PARA_CLUT,
    OPTM_CSC_SET_PARA_CbYCrY,
    OPTM_CSC_SET_PARA_YCbYCr,
    OPTM_CSC_SET_PARA_BUTT
} OPTM_CSC_STATE_E;

typedef enum optm_COLOR_SPACE_E {
    OPTM_CS_UNKNOWN = 0,

    OPTM_CS_BT601_YUV_LIMITED, /* * for BT.601 mod * */
    OPTM_CS_BT601_YUV_FULL,
    OPTM_CS_BT601_RGB_LIMITED,
    OPTM_CS_BT601_RGB_FULL,

    OPTM_CS_BT709_YUV_LIMITED, /* * for BT.709 mod  * */
    OPTM_CS_BT709_YUV_FULL,
    OPTM_CS_BT709_RGB_LIMITED,
    OPTM_CS_BT709_RGB_FULL,

    OPTM_CS_BUTT
} OPTM_COLOR_SPACE_E;

#ifndef HI_BUILD_IN_BOOT
typedef struct tagOPTM_GFX_WORK_S {
    hi_bool bOpenLayer;
    hi_u32 u32Data;
    struct work_struct work;
} OPTM_GFX_WORK_S;
#endif

typedef union tagOPTM_GFX_UP_FLAG_U {
    struct {
        unsigned int IsNeedRegUp : 1;     /*  [0] */
        unsigned int IsEnable : 1;        /*  [1] */
        unsigned int OutRect : 1;         /*  [2] */
        unsigned int IsNeedUpAlpha : 1;   /*  [3] */
        unsigned int IsNeedUpPreMult : 1; /*  [4] */
        unsigned int WbcMode : 1;         /*  [5] */
        unsigned int Reserved : 25;       /*  [31...7] */
    } bits;
    unsigned int u32;
} OPTM_GFX_UP_FLAG_U;

typedef enum tagOPTM_DISPCHANNEL_E {
    OPTM_DISPCHANNEL_0 = 0, /* * gfx4,gfx5             * */
    OPTM_DISPCHANNEL_1 = 1, /* * gfx0,gfx1,gfx2,gfx3   * */
    OPTM_DISPCHANNEL_BUTT
} OPTM_DISPCHANNEL_E;

typedef struct tagHIFB_ADP_LOWPOWER_INFO_S {
    hi_bool LowPowerEn;
    hi_u32 LpInfo[CONFIG_HIFB_LOWPOWER_MAX_PARA_CNT];
} HIFB_ADP_LOWPOWER_INFO_S;

typedef struct tagOPTM_GFX_GP_S {
    hi_bool bOpen;
    hi_bool bHdr;
    hi_bool bPreHdr;
    hi_bool bMaskFlag;
    hi_bool bBGRState;
    hi_bool bInterface;
    hi_bool bGpClose;
    hi_bool bHasBeenReOpen[2]; /* * 要是待机或者切制式，这个时候没有中断了，中断需要的信息需要复位 * */
    hi_bool bRecoveryInNextVT;
    hi_bool bNeedExtractLine;
    hi_bool bDispInitial;
    hi_drv_rect stInRect;
    hi_drv_rect stOutRect;
    hi_u32 u32ZmeDeflicker;
    osal_spinlock gp_lock;
#ifdef CHIP_TYPE_hi3798cv200 /* * hdmi cts need * */
    HI_DRV_DISP_FMT_E enDispFmt;
#endif
    HIFB_STEREO_MODE_E enTriDimMode;
    OPTM_GFX_CSC_PARA_S stCscPara;
#ifndef CONFIG_GFX_PQ
    OPTM_COLOR_SPACE_E enInputCsc;
    OPTM_COLOR_SPACE_E enOutputCsc;
#else
    hi_drv_color_descript in_csc;
    hi_drv_color_descript out_csc;
#endif
    OPTM_VDP_LAYER_GP_E enGpHalId;
    OPTM_DISPCHANNEL_E enDispCh;
    OPTM_GFX_UP_FLAG_U unUpFlag;

#ifndef HI_BUILD_IN_BOOT
    struct workqueue_struct *queue;
    OPTM_GFX_WORK_S stStartWbcWork;
    OPTM_GFX_WORK_S stStartStereoWork;
#endif
    hi_u32 u32Prior;
    hi_u32 u32Alpha;
    OPTM_VDP_DATA_RMODE_E enReadMode;
    OPTM_VDP_BKG_S stBkg;
    OPTM_VDP_CBM_MIX_E enMixg;
} OPTM_GFX_GP_S;

typedef struct {
    hi_bool bOpened;
    hi_bool bMaskFlag;
    hi_bool bSharpEnable;
    hi_bool bExtractLine;
    hi_bool bEnable;
    hi_s32 s32Depth;
    hi_bool bPreMute;
    hi_u32 u32ZOrder;
    hi_bool bDmpOpened;
    hi_bool bBufferChged;
    hi_u32 s32BufferChgCount;
    hi_u32 NoCmpBufAddr;
    hi_u32 ARHeadDdr;
    hi_u32 ARDataDdr;
    hi_u32 GBHeadDdr;
    hi_u32 GBDataDdr;
    hi_u32 u32TriDimAddr;
    hi_u16 Stride;    /* * no compression mode stride  */
    hi_u16 CmpStride; /* * compression mode stride     */
    DRV_HIFB_COLOR_FMT_E enDataFmt;
    DRV_HIFB_COLOR_FMT_E old_fmt;
    HIFB_RECT stInRect; /* * Inres of gfx                */
    HIFB_ALPHA_S stAlpha;
    HIFB_COLORKEYEX_S stColorkey;
    HIFB_STEREO_MODE_E enTriDimMode;
    OPTM_VDP_BKG_S stBkg;
    OPTM_VDP_GFX_BITEXTEND_E enBitExtend; /* * the g_ctrl of bitext        * */
    OPTM_VDP_DATA_RMODE_E enReadMode;
    OPTM_VDP_DATA_RMODE_E enUpDateMode;
    OPTM_VDP_LAYER_GFX_E enGfxHalId; /* * the gfx's hal id            */
    HIFB_GP_ID_E enGPId;             /* * which gp the gfx belong to  */
    OPTM_CSC_STATE_E CscState;
    volatile hi_u32 vblflag;
    osal_wait vblEvent;
    drv_gfx_mem_info clut_table;
} OPTM_GFX_LAYER_S;

typedef struct {
    DRV_HIFB_COLOR_FMT_E fmt;
    hi_u32 ar_header;
    hi_u32 ar_data;
    hi_u32 gb_header;
    hi_u32 gb_data;
    hi_u32 stride;
} hifb_decmp_info;

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            initial operation
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
typedef struct {
    hi_void (*DRV_HIFB_ADP_CloseLayer)(HIFB_LAYER_ID_E enLayerId);
#ifndef HI_BUILD_IN_BOOT
    hi_s32 (*DRV_HIFB_ADP_ColorConvert)(const struct fb_var_screeninfo *pstVar, HIFB_COLORKEYEX_S *pCkey);
#endif
    hi_s32 (*DRV_HIFB_ADP_SetEnable)(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable);
    hi_void (*DRV_HIFB_ADP_GetEnable)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbEnable);
    hi_void (*DRV_HIFB_ADP_GetDevCap)(const HIFB_CAPABILITY_S **pstCap);
#ifndef HI_BUILD_IN_BOOT
    hi_void (*DRV_HIFB_ADP_GetOSDData)(HIFB_LAYER_ID_E enLayerId, HIFB_OSD_DATA_S *pstLayerData);
    hi_void (*DRV_HIFB_ADP_GetLogoData)(HIFB_LAYER_ID_E enLayerId, HIFB_LOGO_DATA_S *pstLogoData);
#endif
    hi_s32 (*DRV_HIFB_ADP_GetLayerPriority)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pU32Priority);
    hi_s32 (*DRV_HIFB_ADP_DeInit)(hi_void);
    hi_s32 (*DRV_HIFB_ADP_Init)(hi_void);
    hi_s32 (*DRV_HIFB_ADP_OpenLayer)(HIFB_LAYER_ID_E enLayerId);
    hi_s32 (*DRV_HIFB_ADP_SetColorReg)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32OffSet, hi_u32 u32Color, hi_s32 UpFlag);
    hi_s32 (*DRV_HIFB_ADP_SetTriDimMode)(HIFB_LAYER_ID_E enLayerId, HIFB_STEREO_MODE_E enStereoMode,
                                         HIFB_STEREO_MODE_E enWbcSteroMode);
    hi_s32 (*DRV_HIFB_ADP_SetTriDimAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32StereoAddr);
    hi_s32 (*DRV_HIFB_ADP_SetLayerAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Addr);
    hi_s32 (*DRV_HIFB_ADP_GetLayerAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pu32Addr);
    hi_void (*DRV_HIFB_ADP_GetLayerWillWorkAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pAddress);
    hi_void (*DRV_HIFB_ADP_GetLayerWorkAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pAddress);
    hi_s32 (*DRV_HIFB_ADP_SetLayerAlpha)(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha);
    hi_s32 (*DRV_HIFB_ADP_SetLayerDataFmt)(HIFB_LAYER_ID_E enLayerId, DRV_HIFB_COLOR_FMT_E enDataFmt);
    hi_s32 (*DRV_HIFB_ADP_SetLayerDeFlicker)(HIFB_LAYER_ID_E enLayerId, HIFB_DEFLICKER_S *pstDeFlicker);
    hi_s32 (*DRV_HIFB_ADP_SetLayerPriority)(HIFB_LAYER_ID_E enLayerId, HIFB_ZORDER_E enZOrder);
    hi_s32 (*DRV_HIFB_ADP_UpLayerReg)(HIFB_LAYER_ID_E enLayerId);
    hi_void (*DRV_HIFB_ADP_GetDhd0Info)(HIFB_LAYER_ID_E LayerId, hi_ulong *pExpectIntLineNumsForVoCallBack,
                                        hi_ulong *pExpectIntLineNumsForEndCallBack,
                                        hi_ulong *pActualIntLineNumsForVoCallBack, hi_ulong *pHardIntCntForVoCallBack);
    hi_void (*DRV_HIFB_ADP_GetCloseState)(HIFB_LAYER_ID_E LayerId, hi_bool *pHasBeenClosedForVoCallBack,
                                          hi_bool *pHasBeenClosedForEndCallBack);
    hi_s32 (*DRV_HIFB_ADP_WaitVBlank)(HIFB_LAYER_ID_E enLayerId);
    hi_s32 (*DRV_HIFB_ADP_SetLayKeyMask)(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S *pstColorkey);
    hi_s32 (*DRV_HIFB_ADP_SetLayerPreMult)(HIFB_LAYER_ID_E enLayerId, hi_bool bPreMul);
    hi_void (*DRV_HIFB_ADP_GetLayerPreMult)(HIFB_LAYER_ID_E enLayerId, hi_bool *pPreMul, hi_bool *pDePreMult);
    hi_void (*DRV_HIFB_ADP_GetLayerHdr)(HIFB_LAYER_ID_E enLayerId, hi_bool *pHdr);
    hi_void (*DRV_HIFB_ADP_GetClosePreMultState)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbShouldClosePreMult);
    hi_void (*DRV_HIFB_ADP_ReadRegister)(hi_u32 Offset, hi_u32 *pRegBuf);
    hi_void (*DRV_HIFB_ADP_WriteRegister)(hi_u32 Offset, hi_u32 Value);
    hi_void (*DRV_HIFB_ADP_WhetherDiscardFrame)(HIFB_LAYER_ID_E enLayerId, hi_bool *pNoDiscardFrame);
    hi_s32 (*DRV_HIFB_ADP_SetCallback)(HIFB_CALLBACK_TPYE_E enCType, IntCallBack pCallBack, HIFB_LAYER_ID_E enLayerId);
    hi_s32 (*DRV_HIFB_ADP_SetLayerStride)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Stride);
    hi_void (*DRV_HIFB_ADP_GetLayerStride)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pStride);
    hi_s32 (*DRV_HIFB_ADP_SetLayerRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstInputRect);
    hi_s32 (*DRV_HIFB_ADP_GetOutRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);
    hi_s32 (*DRV_HIFB_ADP_GetLayerRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);
    hi_s32 (*DRV_HIFB_ADP_SetGPMask)(HIFB_LAYER_ID_E enLayerId, hi_bool bFlag);
    hi_void (*DRV_HIFB_ADP_GetGPMask)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbMask);
    hi_s32 (*DRV_HIFB_ADP_GetDispFMTSize)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);
    hi_s32 (*DRV_HIFB_ADP_ClearLogoOsd)(HIFB_LAYER_ID_E enLayerId);
    hi_s32 (*DRV_HIFB_ADP_SetStereoDepth)(HIFB_LAYER_ID_E enLayerId, hi_s32 s32Depth);
    hi_s32 (*DRV_HIFB_ADP_SetTCFlag)(hi_bool bFlag);
    hi_void (*DRV_HIFB_ADP_SetDeCmpSwitch)(HIFB_LAYER_ID_E enLayerId, hi_bool bOpen);
    hi_void (*DRV_HIFB_ADP_GetDeCmpSwitch)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbOpen);
    hi_void (*drv_hifb_adp_set_decmp_info)(HIFB_LAYER_ID_E layer_id, hifb_decmp_info *dec_info);
    hi_void (*DRV_HIFB_ADP_SetLowPowerInfo)(HIFB_LAYER_ID_E enLayerId, HIFB_ADP_LOWPOWER_INFO_S *pstLowPowerInfo);
    hi_void (*DRV_HIFB_ADP_GetDecompressStatus)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbARDataDecompressErr,
                                                hi_bool *pbGBDataDecompressErr, hi_bool bCloseInterrupt);
    hi_s32 (*DRV_HIFB_ADP_GetHaltDispStatus)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbDispInit);
    hi_void (*DRV_HIFB_ADP_OpenMute)(HIFB_LAYER_ID_E LayerId);
    hi_void (*DRV_HIFB_ADP_CloseMute)(HIFB_LAYER_ID_E LayerId);
    hi_void (*drv_hifb_adp_set_up_mute)(HIFB_LAYER_ID_E layer_id, hi_bool mute);
    hi_void (*drv_hifb_adp_get_up_mute)(HIFB_LAYER_ID_E layer_id, hi_bool *mute);
} HIFB_DRV_OPS_S;
/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */
/**************************************************************************************************
 * func        : DRV_HIFB_ADP_GetCallBackFunction
 * description : CNcomment: 获取设备上下文 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************************/
hi_void DRV_HIFB_ADP_GetCallBackFunction(HIFB_DRV_OPS_S *AdpCallBackFunction);

/***************************************************************************************
 * func         : DRV_HIFB_ADP_OpenLayer
 * description  : CNcomment: 打开图层 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_OpenLayer(HIFB_LAYER_ID_E enLayerId);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_GFX_InitLayer
 * description   : CNcomment: 初始化图层信息 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_GFX_InitLayer(HIFB_LAYER_ID_E enLayerId);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_GP_Open
 * description   : CNcomment: 打开GP设备并设置ZME系数 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_GP_Open(HIFB_GP_ID_E enGPId);

/***************************************************************************************
 * func         : DRV_HIFB_ADP_SetLayerPreMult
 * description  : CNcomment: 设置图层预乘 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerPreMult(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_ReleaseClutBuf
 * description   : CNcomment: 释放调色板内存 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_void DRV_HIFB_ADP_ReleaseClutBuf(HIFB_LAYER_ID_E enLayerId);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_SetReadMode
 * description   : CNcomment: 设置读取数据模式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetReadMode(HIFB_LAYER_ID_E enLayerId, OPTM_VDP_DATA_RMODE_E enReadMode);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_PixerFmtTransferToHalFmt
 * description   : CNcomment: 根据像素数据转换成像素格式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
OPTM_VDP_GFX_IFMT_E DRV_HIFB_ADP_PixerFmtTransferToHalFmt(DRV_HIFB_COLOR_FMT_E enDataFmt);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_UpDataCallBack
 * description   : CNcomment: 注册寄存器更新回调函数 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_void DRV_HIFB_ADP_UpDataCallBack(hi_void *u32Param0, hi_void *u32Param1);

/**************************************************************************************************
 * func            : DRV_HIFB_ADP_SetLayerAddr
 * description    : CNcomment: 设置显示地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:        : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerAddr(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Addr);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_SetLayerStride
 * description   : CNcomment: 设置图层stride CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerStride(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Stride);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_SetCallbackToDisp
 * description   : CNcomment: 向Display注册中断 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetCallbackToDisp(HIFB_GP_ID_E enGPId, IntCallBack pCallBack, hi_drv_disp_callback_type eType,
                                      hi_bool bFlag);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_SetEnable
 * description   : CNcomment: 设置图层使能 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetEnable(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_SetLayerRect
 * description   : CNcomment: 设置图层分辨率 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetLayerRect(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_SetGpRect
 * description   : CNcomment: 设置图层对应的GP分辨率 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_s32 DRV_HIFB_ADP_SetGpRect(HIFB_GP_ID_E enGpId, const HIFB_RECT *pstInputRect);
hi_s32 hifb_set_wbc_gp_rect(HIFB_GP_ID_E gp_id, const HIFB_RECT *rect);
hi_s32 hifb_set_wbc_gp_zme(HIFB_GP_ID_E gp_id);

/***************************************************************************************
 * func        : DRV_HIFB_ADP_GetGpOutputRect
 * description : CNcomment: 获取显示输出的大小 CNend\n
 * param[in]   : eGpId
 * param[in]   : eGpId
 * retval      : HI_SUCCESS
 * retval      : HI_FAILURE
 * others:     : NA
 ***************************************************************************************/
hi_s32 DRV_HIFB_ADP_GetGpOutputRect(HIFB_GP_ID_E eGpId, HIFB_RECT *pstOutputRect);

/**************************************************************************************************
 * func          : DRV_HIFB_ADP_CloseLayer
 * description   : CNcomment: 关闭图层 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************************/
hi_void DRV_HIFB_ADP_CloseLayer(HIFB_LAYER_ID_E enLayerId);

hi_void hifb_adp_set_vdp_layer_enable(hi_u32 layer_id, hi_bool enable);

/***************************************************************************************
 * func        : HIFB_PARA_CHECK_SUPPORT
 * description : CNcomment: 判断输入参数是否支持 CNend\n
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_s32 HIFB_PARA_CHECK_SUPPORT(hi_u32 Mask, hi_u32 Width, hi_u32 Height, hi_u32 Stride, hi_u32 Fmt,
                               hi_u32 BitsPerPixel);

hi_s32 OPTM_GfxSetDispFMTSize(HIFB_GP_ID_E enGpId, const hi_drv_rect *pstOutRect);

#ifdef HI_BUILD_IN_BOOT
hi_bool DRV_HIFB_ADP_WhetherSupportWbc(hi_void);
hi_s32 DRV_HIFB_ADP_Init(hi_void);
hi_s32 DRV_HIFB_ADP_SetTCFlag(hi_bool bFlag);
hi_s32 DRV_HIFB_ADP_LOGO_InitGpFromDisp(HIFB_GP_ID_E enGPId, hi_bool Support4KLogo);
hi_s32 DRV_HIFB_ADP_OpenLayer(HIFB_LAYER_ID_E enLayerId);
hi_s32 DRV_HIFB_ADP_SetLayerAlpha(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha);
hi_s32 DRV_HIFB_ADP_SetLayerDataFmt(HIFB_LAYER_ID_E enLayerId, DRV_HIFB_COLOR_FMT_E enDataFmt);
hi_s32 DRV_HIFB_ADP_UpLayerReg(HIFB_LAYER_ID_E enLayerId);
#endif

hi_u32 hifb_adp_get_layer_max_size(HIFB_LAYER_ID_E layer);
hi_s32 hifb_check_whether_resolution_support(HIFB_LAYER_ID_E layer, hi_u32 width, hi_u32 height);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

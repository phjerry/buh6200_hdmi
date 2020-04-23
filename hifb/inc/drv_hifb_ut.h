/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb ut header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_UT_H__
#define __DRV_HIFB_UT_H__

/* ********************************add include here************************************************ */
#include "drv_hifb_ext.h"
#include "drv_hifb_type.h"

/**************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************** */
typedef hi_s32 (*FN_HIFB_ADP_ParaCheckSupport)(hi_u32 Mask, hi_u32 Width, hi_u32 Height, hi_u32 Stride, hi_u32 Fmt,
                                               hi_u32 BitsPerPixel);
typedef hi_u32 (*FN_HIFB_ADP_Align)(const hi_u32 x, const hi_u32 a);
typedef hi_u32 (*FN_HIFB_ADP_GetGfxHalId)(HIFB_LAYER_ID_E LayerId);
typedef hi_u32 (*FN_HIFB_ADP_GetGpId)(HIFB_LAYER_ID_E LayerId);
typedef hi_u32 (*FN_HIFB_ADP_GetGpHalId)(HIFB_GP_ID_E GpId);
typedef hi_u32 (*FN_HIFB_ADP_GetCscTyeFromDisp)(hi_drv_color_space enHiDrvCsc);
typedef hi_u32 (*FN_HIFB_ADP_GetStereoModeFromDisp)(OPTM_VDP_DISP_MODE_E enDispStereo);
typedef hi_void (*FN_HIFB_ADP_StereoCallBack)(struct work_struct *data);
typedef hi_s32 (*FN_HIFB_ADP_GpOpen)(HIFB_GP_ID_E enGPId);
typedef hi_s32 (*FN_HIFB_ADP_GpClose)(HIFB_GP_ID_E enGPId);
typedef hi_s32 (*FN_HIFB_ADP_SetReadMode)(HIFB_LAYER_ID_E enLayerId, OPTM_VDP_DATA_RMODE_E enReadMode);
typedef hi_s32 (*FN_HIFB_ADP_InitLayer)(HIFB_LAYER_ID_E enLayerId);
typedef hi_void (*FN_HIFB_ADP_ReleaseClutBuf)(HIFB_LAYER_ID_E enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_SetGpCsc)(HIFB_GP_ID_E enGfxGpId, hi_bool bIsBGRIn);
typedef hi_s32 (*FN_HIFB_ADP_SetCallback)(HIFB_CALLBACK_TPYE_E enCType, IntCallBack pCallBack,
                                          HIFB_LAYER_ID_E enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_CheckGfxCallbackReg)(HIFB_GP_ID_E enGPId, HIFB_CALLBACK_TPYE_E eCallbackType);
typedef hi_s32 (*FN_HIFB_ADP_GetScreenRectFromDispInfo)(const hi_rect *tmp_virtscreen,
                                                        const OPTM_GFX_OFFSET_S *stOffsetInfo,
                                                        const hi_rect *stFmtResolution,
                                                        const hi_rect *stPixelFmtResolution, hi_drv_rect *stScreenRect);
typedef hi_void (*FN_HIFB_ADP_FrameEndCallBack)(hi_void *u32Param0, hi_void *u32Param1);
typedef hi_bool (*FN_HIFB_ADP_DispInfoProcess)(HIFB_GP_ID_E eGpId, hi_disp_display_info *pstDispInfo);
typedef hi_s32 (*FN_HIFB_ADP_GpSetMask)(HIFB_GP_ID_E enGPId, hi_bool bFlag);
typedef hi_void (*FN_HIFB_ADP_SetLowPowerInfo)(HIFB_LAYER_ID_E enLayerId, HIFB_ADP_LOWPOWER_INFO_S *pstLowPowerInfo);
typedef hi_void (*FN_HIFB_ADP_SetDeCmpDdrInfo)(HIFB_LAYER_ID_E layer_id, hifb_decmp_info *dec_info);
typedef hi_void (*FN_HIFB_ADP_GetDecompressStatus)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbARDataDecompressErr,
                                                   hi_bool *pbGBDataDecompressErr, hi_bool bCloseInterrupt);
typedef hi_void (*FN_HIFB_ADP_DecompressOpen)(HIFB_LAYER_ID_E enLayerId);
typedef hi_void (*FN_HIFB_ADP_DecompressClose)(HIFB_LAYER_ID_E enLayerId);
typedef hi_void (*FN_HIFB_ADP_UpDataCallBack)(hi_void *u32Param0, hi_void *u32Param1);
typedef hi_void (*FN_HIFB_ADP_ReOpen)(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
typedef hi_void (*FN_HIFB_ADP_ReClose)(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
typedef hi_void (*FN_HIFB_ADP_DispSetting)(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
typedef hi_void (*FN_HIFB_ADP_UpdataLayerInfo)(HIFB_GP_ID_E enGpId, hi_disp_display_info *pstDispInfo);
typedef hi_drv_display (*FN_HIFB_ADP_GfxChn2DispChn)(OPTM_DISPCHANNEL_E enDispCh);
typedef hi_s32 (*FN_HIFB_ADP_SetCallbackToDisp)(HIFB_GP_ID_E enGPId, IntCallBack pCallBack,
                                                hi_drv_disp_callback_type eType, hi_bool bFlag);
typedef hi_s32 (*FN_HIFB_ADP_OpenLayer)(HIFB_LAYER_ID_E enLayerId);
typedef hi_void (*FN_HIFB_ADP_CloseLayer)(HIFB_LAYER_ID_E enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_SetEnable)(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable);
typedef hi_void (*FN_HIFB_ADP_GetEnable)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbEnable);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Addr);
typedef hi_s32 (*FN_HIFB_ADP_GetLayerAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pu32Addr);
typedef hi_void (*FN_HIFB_ADP_GetLayerWorkAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pAddress);
typedef hi_void (*FN_HIFB_ADP_GetLayerWillWorkAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pAddress);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerStride)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32Stride);
typedef hi_void (*FN_HIFB_ADP_GetLayerStride)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pStride);
typedef DRV_HIFB_COLOR_FMT_E (*FN_HIFB_ADP_HalFmtTransferToPixerFmt)(OPTM_VDP_GFX_IFMT_E enDataFmt);
typedef OPTM_VDP_GFX_IFMT_E (*FN_HIFB_ADP_PixerFmtTransferToHalFmt)(DRV_HIFB_COLOR_FMT_E enDataFmt);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerDataFmt)(HIFB_LAYER_ID_E enLayerId, DRV_HIFB_COLOR_FMT_E enDataFmt);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerDataBigEndianFmt)(HIFB_LAYER_ID_E enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerDataLittleEndianFmt)(HIFB_LAYER_ID_E enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_SetColorReg)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32OffSet, hi_u32 u32Color, hi_s32 UpFlag);
typedef hi_void (*FN_HIFB_ADP_GfxWVBCallBack)(hi_u32 enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_WaitVBlank)(HIFB_LAYER_ID_E enLayerId);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerAlpha)(HIFB_LAYER_ID_E enLayerId, HIFB_ALPHA_S *pstAlpha);
typedef hi_s32 (*FN_HIFB_ADP_GetLayerRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstRect);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerRect)(HIFB_LAYER_ID_E enLayerId, const HIFB_RECT *pstRect);
typedef hi_s32 (*FN_HIFB_ADP_GetDispFMTSize)(HIFB_LAYER_ID_E LayerId, HIFB_RECT *pstOutRect);
typedef hi_s32 (*FN_HIFB_ADP_SetGpRect)(HIFB_GP_ID_E enGpId, const HIFB_RECT *pstInputRect);
typedef hi_void (*FN_HIFB_ADP_ResetLayerInRect)(HIFB_GP_ID_E enGpId);
typedef hi_s32 (*FN_HIFB_ADP_GetOutRect)(HIFB_LAYER_ID_E enLayerId, HIFB_RECT *pstOutputRect);
typedef hi_s32 (*FN_HIFB_ADP_SetLayKeyMask)(HIFB_LAYER_ID_E enLayerId, const HIFB_COLORKEYEX_S *pstColorkey);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerPreMult)(HIFB_LAYER_ID_E enLayerId, hi_bool bEnable);
typedef hi_void (*FN_HIFB_ADP_GetLayerPreMult)(HIFB_LAYER_ID_E enLayerId, hi_bool *pPreMul, hi_bool *pDePreMult);
typedef hi_void (*FN_HIFB_ADP_GetLayerHdr)(HIFB_LAYER_ID_E enLayerId, hi_bool *pHdr);
typedef hi_s32 (*FN_HIFB_ADP_GetClosePreMultState)(HIFB_LAYER_ID_E LayerId, hi_bool *pbShouldClosePreMult);
typedef hi_void (*FN_HIFB_ADP_WhetherDiscardFrame)(HIFB_LAYER_ID_E enLayerId, hi_bool *pNoDiscardFrame);
typedef hi_void (*FN_HIFB_ADP_GetOSDData)(HIFB_LAYER_ID_E enLayerId, HIFB_OSD_DATA_S *pstLayerData);
typedef hi_void (*FN_HIFB_ADP_GetLogoData)(HIFB_LAYER_ID_E enLayerId, HIFB_LOGO_DATA_S *pstLogoData);
typedef hi_s32 (*FN_HIFB_ADP_UpLayerReg)(HIFB_LAYER_ID_E enLayerId);
typedef hi_void (*FN_HIFB_ADP_GetCloseState)(HIFB_LAYER_ID_E LayerId, hi_bool *pHasBeenClosedForVoCallBack,
                                             hi_bool *pHasBeenClosedForEndCallBack);
typedef hi_void (*FN_HIFB_ADP_GetDhd0Info)(HIFB_LAYER_ID_E LayerId, hi_ulong *pExpectIntLineNumsForVoCallBack,
                                           hi_ulong *pExpectIntLineNumsForEndCallBack,
                                           hi_ulong *pActualIntLineNumsForVoCallBack,
                                           hi_ulong *pHardIntCntForVoCallBack);
typedef hi_s32 (*FN_HIFB_ADP_SetTriDimMode)(HIFB_LAYER_ID_E enLayerId, HIFB_STEREO_MODE_E enMode,
                                            HIFB_STEREO_MODE_E enWbcSteroMode);
typedef hi_s32 (*FN_HIFB_ADP_SetTriDimAddr)(HIFB_LAYER_ID_E enLayerId, hi_u32 u32TriDimAddr);
typedef hi_s32 (*FN_HIFB_ADP_GetLayerPriority)(HIFB_LAYER_ID_E enLayerId, hi_u32 *pPriority);
typedef hi_s32 (*FN_HIFB_ADP_SetLayerPriority)(HIFB_LAYER_ID_E enLayerId, HIFB_ZORDER_E enZOrder);
typedef hi_s32 (*FN_HIFB_ADP_DispInfoUpdate)(HIFB_GP_ID_E enGPId);
typedef hi_s32 (*FN_HIFB_ADP_GPRecovery)(HIFB_GP_ID_E enGPId);
typedef hi_s32 (*FN_HIFB_ADP_VoCallback)(hi_void *u32Param0, hi_void *u32Param1);
typedef hi_s32 (*FN_HIFB_ADP_DistributeCallback)(hi_void *u32Param0, hi_void *u32Param1);
typedef hi_s32 (*FN_HIFB_ADP_SetGPMask)(HIFB_LAYER_ID_E LayerId, hi_bool bFlag);
typedef hi_void (*FN_HIFB_ADP_GetGPMask)(HIFB_LAYER_ID_E LayerId, hi_bool *pbMask);
typedef hi_s32 (*FN_HIFB_ADP_SetStereoDepth)(HIFB_LAYER_ID_E enLayerId, hi_s32 s32Depth);
typedef hi_s32 (*FN_HIFB_ADP_GetHaltDispStatus)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbDispInit);
typedef hi_void (*FN_HIFB_ADP_SetDeCmpSwitch)(HIFB_LAYER_ID_E enLayerId, hi_bool bOpen);
typedef hi_void (*FN_HIFB_ADP_GetDeCmpSwitch)(HIFB_LAYER_ID_E enLayerId, hi_bool *pbOpen);
typedef hi_void (*FN_HIFB_ADP_AdpOpenMute)(HIFB_LAYER_ID_E LayerId);
typedef hi_void (*FN_HIFB_ADP_CloseMute)(HIFB_LAYER_ID_E LayerId);
typedef hi_void (*fn_hifb_adp_set_up_mute)(HIFB_LAYER_ID_E layer_id, hi_bool mute);
typedef hi_void (*fn_hifb_adp_get_up_mute)(HIFB_LAYER_ID_E layer_id, hi_bool *mute);

/* **************************** Now Has not been used ******************************************** */

/* ************************** Structure Definition ************************************************ */
typedef struct {
    hifb_mod_init pfnHifbModInit;
    hifb_mod_exit pfnHifbModExit;
    hifb_get_settings_info pfnHifbGetSettingInfo;
    hifb_set_logo_layer_enable pfnHifbSetLogoLayerEnable;
    FN_HIFB_ADP_ParaCheckSupport pfnHifbAdpParaCheckSupport;
    FN_HIFB_ADP_Align pfnHifbAdpAlign;
    FN_HIFB_ADP_GetGfxHalId pfnHifbAdpGetGfxHalId;
    FN_HIFB_ADP_GetGpId pfnHifbAdpGetGpId;
    FN_HIFB_ADP_GetGpHalId pfnHifbAdpGetGpHalId;
    FN_HIFB_ADP_GetCscTyeFromDisp pfnHifbAdpGetCscTyeFromDisp;
    FN_HIFB_ADP_GetStereoModeFromDisp pfnHifbAdpGetStereoModeFromDisp;
    FN_HIFB_ADP_StereoCallBack pfnHifbAdpStereoCallBack;
    FN_HIFB_ADP_GpOpen pfnHifbAdpGpOpen;
    FN_HIFB_ADP_GpClose pfnHifbAdpGpClose;
    FN_HIFB_ADP_SetReadMode pfnHifbAdpSetReadMode;
    FN_HIFB_ADP_InitLayer pfnHifbAdpInitLayer;
    FN_HIFB_ADP_ReleaseClutBuf pfnHifbAdpReleaseClutBuf;
    FN_HIFB_ADP_SetGpCsc pfnHifbAdpSetGpCsc;
    FN_HIFB_ADP_SetCallback pfnHifbAdpSetCallback;
    FN_HIFB_ADP_CheckGfxCallbackReg pfnHifbAdpCheckGfxCallbackReg;
    FN_HIFB_ADP_GetScreenRectFromDispInfo pfnHifbAdpGetScreenRectFromDispInfo;
    FN_HIFB_ADP_FrameEndCallBack pfnHifbAdpFrameEndCallBack;
    FN_HIFB_ADP_DispInfoProcess pfnHifbAdpDispInfoProcess;
    FN_HIFB_ADP_GpSetMask pfnHifbAdpGpSetMask;
    FN_HIFB_ADP_SetLowPowerInfo pfnHifbAdpSetLowPowerInfo;
    FN_HIFB_ADP_SetDeCmpDdrInfo pfnHifbAdpSetDeCmpDdrInfo;
    FN_HIFB_ADP_GetDecompressStatus pfnHifbAdpGetDecompressStatus;
    FN_HIFB_ADP_DecompressOpen pfnHifbAdpDecompressOpen;
    FN_HIFB_ADP_DecompressClose pfnHifbAdpDecompressClose;
    FN_HIFB_ADP_UpDataCallBack pfnHifbAdpUpDataCallBack;
    FN_HIFB_ADP_ReOpen pfnHifbAdpReOpen;
    FN_HIFB_ADP_ReClose pfnHifbAdpReClose;
    FN_HIFB_ADP_DispSetting pfnHifbAdpDispSetting;
    FN_HIFB_ADP_UpdataLayerInfo pfnHifbAdpUpdataLayerInfo;
    FN_HIFB_ADP_GfxChn2DispChn pfnHifbAdpGfxChn2DispChn;
    FN_HIFB_ADP_SetCallbackToDisp pfnHifbAdpSetCallbackToDisp;
    FN_HIFB_ADP_OpenLayer pfnHifbAdpOpenLayer;
    FN_HIFB_ADP_CloseLayer pfnHifbAdpCloseLayer;
    FN_HIFB_ADP_SetEnable pfnHifbAdpSetEnable;
    FN_HIFB_ADP_GetEnable pfnHifbAdpGetEnable;
    FN_HIFB_ADP_SetLayerAddr pfnHifbAdpSetLayerAddr;
    FN_HIFB_ADP_GetLayerAddr pfnHifbAdpGetLayerAddr;
    FN_HIFB_ADP_GetLayerWorkAddr pfnHifbAdpGetLayerWorkAddr;
    FN_HIFB_ADP_GetLayerWillWorkAddr pfnHifbAdpGetLayerWillWorkAddr;
    FN_HIFB_ADP_SetLayerStride pfnHifbAdpSetLayerStride;
    FN_HIFB_ADP_GetLayerStride pfnHifbAdpGetLayerStride;
    FN_HIFB_ADP_HalFmtTransferToPixerFmt pfnHifbAdpHalFmtTransferToPixerFmt;
    FN_HIFB_ADP_PixerFmtTransferToHalFmt pfnHifbAdpPixerFmtTransferToHalFmt;
    FN_HIFB_ADP_SetLayerDataFmt pfnHifbAdpSetLayerDataFmt;
    FN_HIFB_ADP_SetLayerDataBigEndianFmt pfnHifbAdpSetLayerDataBigEndianFmt;
    FN_HIFB_ADP_SetLayerDataLittleEndianFmt pfnHifbAdpSetLayerDataLittleEndianFmt;
    FN_HIFB_ADP_SetColorReg pfnHifbAdpSetColorReg;
    FN_HIFB_ADP_GfxWVBCallBack pfnHifbAdpGfxWVBCallBack;
    FN_HIFB_ADP_WaitVBlank pfnHifbAdpWaitVBlank;
    FN_HIFB_ADP_SetLayerAlpha pfnHifbAdpSetLayerAlpha;
    FN_HIFB_ADP_GetLayerRect pfnHifbAdpGetLayerRect;
    FN_HIFB_ADP_SetLayerRect pfnHifbAdpSetLayerRect;
    FN_HIFB_ADP_GetDispFMTSize pfnHifbAdpGetDispFMTSize;
    FN_HIFB_ADP_SetGpRect pfnHifbAdpSetGpRect;
    FN_HIFB_ADP_ResetLayerInRect pfnHifbAdpResetLayerInRect;
    FN_HIFB_ADP_GetOutRect pfnHifbAdpGetOutRect;
    FN_HIFB_ADP_SetLayKeyMask pfnHifbAdpSetLayKeyMask;
    FN_HIFB_ADP_SetLayerPreMult pfnHifbAdpSetLayerPreMult;
    FN_HIFB_ADP_GetLayerPreMult pfnHifbAdpGetLayerPreMult;
    FN_HIFB_ADP_GetLayerHdr pfnHifbAdpGetLayerHdr;
    FN_HIFB_ADP_WhetherDiscardFrame pfnHifbAdpWhetherDiscardFrame;
    FN_HIFB_ADP_GetOSDData pfnHifbAdpGetOSDData;
    FN_HIFB_ADP_GetLogoData pfnHifbAdpGetLogoData;
    FN_HIFB_ADP_UpLayerReg pfnHifbAdpUpLayerReg;
    FN_HIFB_ADP_GetCloseState pfnHifbAdpGetCloseState;
    FN_HIFB_ADP_GetDhd0Info pfnHifbAdpGetDhd0Info;
    FN_HIFB_ADP_SetTriDimMode pfnHifbAdpSetTriDimMode;
    FN_HIFB_ADP_SetTriDimAddr pfnHifbAdpSetTriDimAddr;
    FN_HIFB_ADP_GetLayerPriority pfnHifbAdpGetLayerPriority;
    FN_HIFB_ADP_SetLayerPriority pfnHifbAdpSetLayerPriority;
    FN_HIFB_ADP_DispInfoUpdate pfnHifbAdpDispInfoUpdate;
    FN_HIFB_ADP_GPRecovery pfnHifbAdpGPRecovery;
    FN_HIFB_ADP_VoCallback pfnHifbAdpVoCallback;
    FN_HIFB_ADP_DistributeCallback pfnHifbAdpDistributeCallback;
    FN_HIFB_ADP_SetGPMask pfnHifbAdpSetGPMask;
    FN_HIFB_ADP_GetGPMask pfnHifbAdpGetGPMask;
    FN_HIFB_ADP_SetStereoDepth pfnHifbAdpSetStereoDepth;
    FN_HIFB_ADP_GetHaltDispStatus pfnHifbAdpGetHaltDispStatus;
    FN_HIFB_ADP_SetDeCmpSwitch pfnHifbAdpSetDeCmpSwitch;
    FN_HIFB_ADP_GetDeCmpSwitch pfnHifbAdpGetDeCmpSwitch;
    FN_HIFB_ADP_AdpOpenMute pfnHifbAdpOpenMute;
    FN_HIFB_ADP_CloseMute pfnHifbAdpCloseMute;
    fn_hifb_adp_set_up_mute pfn_hifb_adp_set_up_mute;
    fn_hifb_adp_get_up_mute pfn_hifb_adp_get_up_mute;
    /* **************************** Now Has not been used ******************************************** */
} HIFB_EXPORT_UT_FUNC_S;

/* ********************* Global Variable declaration ********************************************** */

/* ****************************** API declaration ************************************************* */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb main
 * Author: sdk
 * Create: 2016-01-01
 */

#include "drv_hifb_config.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_debug.h"
#include "drv_hifb_ut.h"

/* **************************** Macro Definition *********************************************** */
#define HIFB_NAME "HI_FB"

#define HIFB_DISPCHN_HD 1
#define HIFB_DISPCHN_SD 0
/* ************************** Structure Definition ********************************************* */

/* ********************* Global Variable declaration ******************************************* */
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
static volatile hi_u32 *g_u32SDLogoCtrlReg = NULL;
static volatile hi_u32 *g_u32SDCtrlReg = NULL;
static volatile hi_u32 *g_u32WBCCtrlReg = NULL;
#endif

static hi_bool gs_bLogoOff = HI_FALSE;

extern HIFB_DRV_OPS_S g_stDrvAdpCallBackFunction;

/* ****************************** API declaration ********************************************** */

extern hi_void DRV_HIFB_ADP_RegistAdpFunction(HIFB_EXPORT_UT_FUNC_S *RegistAdpFunction);

/* ****************************** API release ************************************************** */

/***************************************************************************************
 * func          : HifbSetLogoLayerEnable
 * description   : CNcomment: logo图层使能，在这里是关logo使用 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void HifbSetLogoLayerEnable(hi_bool bEnable)
{
    // PDM_EXPORT_FUNC_S *ps_PdmExportFuncs = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (gs_bLogoOff) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void DRV_HIFB_SetLogoEnable(hi_bool bEnable)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    HifbSetLogoLayerEnable(bEnable);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetGPMask(HIFB_LAYER_HD_0, HI_FALSE);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_SetGPMask(HIFB_LAYER_SD_0, HI_FALSE);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
 * func          : s_HifbExportFuncs
 * description   : CNcomment: 向外注册函数给其它模块调用 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
static HIFB_EXPORT_UT_FUNC_S s_HifbExportFuncs = {
    .pfnHifbModInit = HIFB_DRV_ModInit,
    .pfnHifbModExit = HIFB_DRV_ModExit,
    .pfnHifbGetSettingInfo = HIFB_DRV_GetSettingInfo,
    .pfnHifbSetLogoLayerEnable = DRV_HIFB_SetLogoEnable,
};

hi_s32 DRV_HIFB_ModInit_K(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    g_u32SDLogoCtrlReg = (hi_u32 *)osal_ioremap_nocache(CONFIG_HIFB_SD_LOGO_REG_BASEADDR, 8);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_u32SDLogoCtrlReg, HI_FAILURE);

    g_u32SDCtrlReg = (hi_u32 *)osal_ioremap_nocache(CONFIG_HIFB_WBC_SLAYER_REG_BASEADDR, 8);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_u32SDCtrlReg, HI_FAILURE);

    g_u32WBCCtrlReg = (hi_u32 *)osal_ioremap_nocache(CONFIG_HIFB_WBC_GP0_REG_BASEADDR, 8);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(g_u32WBCCtrlReg, HI_FAILURE);
#endif

    DRV_HIFB_ADP_RegistAdpFunction(&s_HifbExportFuncs);

    Ret = hi_gfx_module_register(HIGFX_FB_ID, HIFB_NAME, &s_HifbExportFuncs);
    if (HI_SUCCESS != Ret) {
        DRV_HIFB_ModExit_K();
        return Ret;
    }

    return HI_SUCCESS;
}

hi_void DRV_HIFB_ModExit_K(hi_void)
{
    hi_gfx_module_unregister(HIGFX_FB_ID);

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if (NULL != g_u32SDLogoCtrlReg) {
        osal_iounmap(g_u32SDLogoCtrlReg);
        g_u32SDLogoCtrlReg = NULL;
    }
    if (NULL != g_u32SDCtrlReg) {
        osal_iounmap(g_u32SDCtrlReg);
        g_u32SDCtrlReg = NULL;
    }
    if (NULL != g_u32WBCCtrlReg) {
        osal_iounmap(g_u32WBCCtrlReg);
        g_u32WBCCtrlReg = NULL;
    }
#endif

    return;
}

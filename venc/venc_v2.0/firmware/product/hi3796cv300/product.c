/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-07-28
 */
#include "venc_hal_ext.h"
#include "hi_reg_common.h"
#include "drv_venc_osal.h"
#include "hi_drv_sys.h"
#include "linux/hisilicon/securec.h"
#include "hi_osal.h"

enum {
    VENC_CLK_SEL_240 = 0,
    VENC_CLK_SEL_24 = 1
};

static hi_void DRV_VENC_PDT_OpenHardware(hi_void)
{
    U_PERI_CRG336 tmp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
#ifdef HI_SMMU_SUPPORT
    U_PERI_CRG180 tmp_smmu_cfg;

    /* if support SMMU should open the smmu clock */
    tmp_smmu_cfg.u32 = reg_crg->PERI_CRG180.u32;
    tmp_smmu_cfg.bits.venc_smmu_cken = 1;
    reg_crg->PERI_CRG180.u32 = tmp_smmu_cfg.u32;
#endif

    VENC_CHECK_NEQ_VOID(memset_s(&tmp_value, sizeof(U_PERI_CRG336), 0, sizeof(U_PERI_CRG336)), HI_SUCCESS);
    /* reset */
    reg_crg->PERI_CRG336.u32 = tmp_value.u32;

    /* open vedu clock */
    tmp_value.bits.venc_core_cken = 1;
    tmp_value.bits.venc_axi_cken  = 1;
    tmp_value.bits.venc_cken = 1;
    tmp_value.bits.vedu_cksel  = VENC_CLK_SEL_240;
    reg_crg->PERI_CRG336.u32 = tmp_value.u32;
}

static hi_void DRV_VENC_PDT_CloseHardware(hi_void)
{
    U_PERI_CRG336 tmp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
#ifdef HI_SMMU_SUPPORT
    U_PERI_CRG180 tmp_smmu_cfg;
    tmp_smmu_cfg.u32 = reg_crg->PERI_CRG180.u32;
    tmp_smmu_cfg.bits.venc_smmu_cken = 0;
    reg_crg->PERI_CRG180.u32 = tmp_smmu_cfg.u32;
#endif

    /* close vedu clock */
    tmp_value.u32 = reg_crg->PERI_CRG336.u32;
    tmp_value.bits.venc_core_cken = 0;
    tmp_value.bits.venc_axi_cken  = 0;
    tmp_value.bits.venc_cken = 0;
    reg_crg->PERI_CRG336.u32 = tmp_value.u32;
}

static hi_void DRV_VENC_PDT_ResetHardware(hi_void)
{
    U_PERI_CRG336 tmp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
#if defined(HI_SMMU_SUPPORT) && !defined(HI_TEE_SUPPORT)
    U_PERI_CRG180 tmp_smmu_cfg;

    tmp_smmu_cfg.u32 = reg_crg->PERI_CRG180.u32;
    tmp_smmu_cfg.bits.venc_smmu_srst_req  = 1;
    reg_crg->PERI_CRG180.u32 = tmp_smmu_cfg.u32;
    osal_udelay(5); /* wait 5 */

    tmp_smmu_cfg.bits.venc_smmu_srst_req  = 0;
    reg_crg->PERI_CRG180.u32 = tmp_smmu_cfg.u32;

    osal_udelay(10); /* wait 10 */
#endif
    tmp_value.u32 = reg_crg->PERI_CRG336.u32;

    tmp_value.bits.venc_srst_req = 1;
    reg_crg->PERI_CRG336.u32 = tmp_value.u32;
    osal_udelay(5); /* wait 5 */

    tmp_value.bits.venc_srst_req = 0;
    reg_crg->PERI_CRG336.u32 = tmp_value.u32;
    osal_udelay(10); /* wait 10 */
}

static hi_void DRV_VENC_PDT_InitHalInterface(hi_void)
{
    g_venc_hal_fun_ptr.pfun_hal_clear_int      = drv_venc_hal_v400_r008_c02_clr_int;
    g_venc_hal_fun_ptr.pfun_hal_disable_int    = drv_venc_hal_v400_r008_c02_disable_int;
    g_venc_hal_fun_ptr.pfun_hal_read_reg       = drv_venc_hal_v400_r008_c02_read_reg;
#ifdef HI_SMMU_SUPPORT
    g_venc_hal_fun_ptr.pfun_hal_read_smmu_reg  = drv_venc_hal_v400_r008_c02_read_reg_smmu;
#endif
    g_venc_hal_fun_ptr.pfun_hal_read_cfg_reg   = drv_venc_hal_v400_r008_c02_cfg_reg;

    g_venc_hal_fun_ptr.pfun_hal_init          = drv_venc_hal_v400_r008_c02_init;
    g_venc_hal_fun_ptr.pfun_hal_deinit        = drv_venc_hal_v400_r008_c02_deinit;

    g_venc_hal_fun_ptr.pfun_hal_open_hardware  = DRV_VENC_PDT_OpenHardware;
    g_venc_hal_fun_ptr.pfun_hal_close_hardware = DRV_VENC_PDT_CloseHardware;
    g_venc_hal_fun_ptr.pfun_hal_reset_hardware = DRV_VENC_PDT_ResetHardware;
}

SINT32 drv_venc_pdt_v500_r001_init(hi_void)
{
    DRV_VENC_PDT_InitHalInterface();

    return 0;
}




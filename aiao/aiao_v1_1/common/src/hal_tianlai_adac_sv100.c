/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adac hardware driver
 * Author: audio
 * Create: 2019-09-25
 * Notes: NA
 * History: 2019-09-25 first version of sv100 adac driver
 */

#include "hi_osal.h"

#include "hi_reg_common.h"
#include "hi_drv_sys.h"

#include "drv_ao_private.h"
#include "hal_tianlai_adac.h"
#include "hal_tianlai_adac_sv100.h"

static volatile acodec_reg *g_reg_adac = HI_NULL;

#define VREF_BITS 12

#define REG17_VREF_BITS 8
#define REG17_VREF_MASK 0xff0

#define REG20_VREF_BITS 4
#define REG20_VREF_MASK 0xf

#define VREF_TIME_OUT_MS 200
#define ADC_RCTUNE_EN_TIME_OUT_US 30

#define REG_ADAC 0x00EB0000

static hi_void dac_analog_reset(hi_void)
{
    /* write default value into register */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0xC080DEFF;
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = 0x0ECE2900;
    g_reg_adac->AUDIO_ANA_CTRL_2.u32 = 0x40550076;
    g_reg_adac->AUDIO_ANA_CTRL_3.u32 = 0x3584B555;
    g_reg_adac->AUDIO_ANA_CTRL_4.u32 = 0x8AFF0000;
    g_reg_adac->AUDIO_ANA_CTRL_5.u32 = 0x0;
}

static hi_void dac_analog_power_up(hi_void)
{
    U_AUDIO_ANA_CTRL_3 ctrl_3;

    dac_analog_reset();

    ctrl_3.u32 = g_reg_adac->AUDIO_ANA_CTRL_3.u32;
    ctrl_3.bits.VREF_SEL = 0x0;
    g_reg_adac->AUDIO_ANA_CTRL_3.u32 = ctrl_3.u32;

    /* step 1: RST = 0 */
    ctrl_3.u32 = g_reg_adac->AUDIO_ANA_CTRL_3.u32;
    ctrl_3.bits.RST = 0x0;
    g_reg_adac->AUDIO_ANA_CTRL_3.u32 = ctrl_3.u32;

    /* step 2: PD-(IBIAS/CTCM/CTCMibias/DAC-VREF) = 0; PD_DACL/R = 0, PDB-DACclk = 1 */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0x808022FF;
    g_reg_adac->AUDIO_ANA_CTRL_2.u32 = 0x40550074;
    g_reg_adac->AUDIO_ANA_CTRL_4.u32 = 0x9AFF0000;

    /* step 3: PU_POP_PULLB_REG_S = 1 */
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = 0x8ECE2900;

    /* step 4: PD-VREF = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0x808022FE;

    /* step 5: VREF-PU-PDB-S = 1 */
    g_reg_adac->AUDIO_ANA_CTRL_4.u32 = 0xDAFF0000;

    /* step 6: wait 200ms */
    osal_msleep(VREF_TIME_OUT_MS);

    /* step 7: LINEOUT-L/R-PD-ORG-08 = 0 */
    ctrl_3.u32 = g_reg_adac->AUDIO_ANA_CTRL_3.u32;
    ctrl_3.bits.LINEOUTR_PD_ORG = 0x0;
    ctrl_3.bits.LINEOUTL_PD_ORG_08 = 0x0;
    g_reg_adac->AUDIO_ANA_CTRL_3.u32 = ctrl_3.u32;

    /* step 8: PMD_LINEOUT = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0x808022BE;

    /* step 9: Aux-EN = 0 */
    ctrl_3.u32 = g_reg_adac->AUDIO_ANA_CTRL_3.u32;
    ctrl_3.bits.Aux_EN = 0x0;
    g_reg_adac->AUDIO_ANA_CTRL_3.u32 = ctrl_3.u32;

    /* step 10: POP-LINEOUT-PULL-VCM-S = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_2.u32 = 0x40552070;

    /* step 11: PU_POP_MUTE = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0x8080223E;

    /* step 12: PD-LINEIN-L/R, PD-ADC-L/R, PD-RCTUNE = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0x80802200;
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = 0xAECE2900;

    /* step 13: ADC_RCTUNE_EN = 1 */
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = 0xEEEE2900;

    /* step 14: wait 30us */
    osal_udelay(ADC_RCTUNE_EN_TIME_OUT_US);

    /* step 15: MUTE_DAC_L/R = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = 0xEE2E2900;

    /* step 16: RSTB-DAC = 1 */
    g_reg_adac->AUDIO_ANA_CTRL_4.u32 = 0xFAFF0000;

    /* step 17: MUTE-LINEIN-LR = 0 */
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = 0x00002000;
}

static hi_void dac_analog_power_down(hi_void)
{
    hi_u32 i;
    hi_u32 value = 0;

    U_AUDIO_ANA_CTRL_0  ctrl_0;
    U_AUDIO_ANA_CTRL_1  ctrl_1;
    U_AUDIO_ANA_CTRL_2  ctrl_2;
    U_AUDIO_ANA_CTRL_3  ctrl_3;
    U_AUDIO_ANA_CTRL_4  ctrl_4;
    U_AUDIO_ANA_CTRL_5  ctrl_5;

    ctrl_1.u32 = g_reg_adac->AUDIO_ANA_CTRL_1.u32;
    ctrl_1.bits.mute_dacl = 0x1;
    ctrl_1.bits.mute_dacr = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = ctrl_1.u32;

    /* step 1: POP_MUTE_SEL = 111, PULLOUT_WEAK = 1 */
    ctrl_5.u32 = g_reg_adac->AUDIO_ANA_CTRL_5.u32;
    ctrl_5.bits.POP_MUTE_SEL = 0x7;
    ctrl_5.bits.PULLOUT_WEAK = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_5.u32 = ctrl_5.u32;

    /* step 2: PU_POP_MUTE = 1 */
    ctrl_0.u32 = g_reg_adac->AUDIO_ANA_CTRL_0.u32;
    ctrl_0.bits.PU_POP_MUTE = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = ctrl_0.u32;

    /* step 3: Aux_EN = 1, PDM = 1 */
    ctrl_3.u32 = g_reg_adac->AUDIO_ANA_CTRL_3.u32;
    ctrl_3.bits.Aux_EN = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_3.u32 = ctrl_3.u32;

    ctrl_0.u32 = g_reg_adac->AUDIO_ANA_CTRL_0.u32;
    ctrl_0.bits.PDM_LINEOUT = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = ctrl_0.u32;

    /* step 4: PULL_VCM = 1 */
    ctrl_2.u32 = g_reg_adac->AUDIO_ANA_CTRL_2.u32;
    ctrl_2.bits.POP_LINEOUT_PULL_VCM = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_2.u32 = ctrl_2.u32;

    /* step 5: PD_VREF = 1 */
    ctrl_0.u32 = g_reg_adac->AUDIO_ANA_CTRL_0.u32;
    ctrl_0.bits.PD_VREF = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_0.u32 = ctrl_0.u32;

    /* step 6: set VREF_PULLDOWN_RES_SEL */
    for (i = 0; i < VREF_BITS; i++) {
        value = ((value << 1) + 1);
        ctrl_5.u32 = g_reg_adac->AUDIO_ANA_CTRL_5.u32;
        ctrl_5.bits.VREF_PULLDOWN_RES_SEL = (value & REG20_VREF_MASK);
        g_reg_adac->AUDIO_ANA_CTRL_5.u32 = ctrl_5.u32;

        ctrl_4.u32 = g_reg_adac->AUDIO_ANA_CTRL_4.u32;
        ctrl_4.bits.VREF_PULLDOWN_RES_SEL = ((value & REG17_VREF_MASK) >> REG20_VREF_BITS);
        g_reg_adac->AUDIO_ANA_CTRL_4.u32 = ctrl_4.u32;
        osal_msleep(1);
    }

    /* step 7: PU_POP_PULLB_REG = 1 */
    ctrl_1.u32 = g_reg_adac->AUDIO_ANA_CTRL_1.u32;
    ctrl_1.bits.PU_POP_PULLB_REG = 0x1;
    g_reg_adac->AUDIO_ANA_CTRL_1.u32 = ctrl_1.u32;

    /* step 8: write default value */
    dac_analog_reset();
}

static hi_void dac_digital_init(hi_void)
{
    g_reg_adac->AUDIO_CTRL_REG_1.u32 = 0xff035a4a;
    g_reg_adac->AUDIO_DAC_REG_1.u32 = 0x06062424;
    g_reg_adac->AUDIO_DAC_REG_0.u32 = 0x08000001;
}

static hi_void dac_digital_deinit(hi_void)
{
    g_reg_adac->AUDIO_CTRL_REG_1.u32 = 0x7f7f2424;
}

static hi_void adac_reset(hi_void)
{
    U_PERI_CRG456 tmp;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    if (reg_crg == HI_NULL) {
        HI_ERR_AIAO("call hi_drv_sys_get_crg_reg_ptr failed\n");
        return;
    }

    tmp.u32 = reg_crg->PERI_CRG456.u32;
    tmp.bits.audio_codec_ana_srst_req = 1;
    tmp.bits.audio_codec_dig_srst_req = 1;
    reg_crg->PERI_CRG456.u32 = tmp.u32;
}

static hi_void adac_unreset(hi_void)
{
    U_PERI_CRG456 tmp;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    if (reg_crg == HI_NULL) {
        HI_ERR_AIAO("call hi_drv_sys_get_crg_reg_ptr failed\n");
        return;
    }

    adac_reset();

    tmp.u32 = reg_crg->PERI_CRG456.u32;
    tmp.bits.audio_codec_ana_srst_req = 0;
    tmp.bits.audio_codec_dig_srst_req = 0;
    reg_crg->PERI_CRG456.u32 = tmp.u32;
}

#ifdef HI_SND_MUTECTL_SUPPORT
static hi_void fast_power_enable(hi_bool enable)
{
}
#endif

static hi_void adac_init(hi_sample_rate sample_rate, hi_bool resume)
{
    g_reg_adac = (volatile acodec_reg *)osal_ioremap_nocache(REG_ADAC, sizeof(acodec_reg));
    if (g_reg_adac == HI_NULL) {
        HI_ERR_AIAO("osal_ioremap_nocache REG_ADAC failed\n");
        return;
    }
    adac_unreset();
    dac_digital_init();
    dac_analog_power_up();

    osal_iounmap((hi_void *)g_reg_adac);
    g_reg_adac = HI_NULL;
}

static hi_void adac_deinit(hi_bool suspend)
{
    g_reg_adac = (volatile acodec_reg *)osal_ioremap_nocache(REG_ADAC, sizeof(acodec_reg));
    if (g_reg_adac == HI_NULL) {
        HI_ERR_AIAO("osal_ioremap_nocache REG_ADAC failed\n");
        return;
    }

    dac_digital_deinit();
    dac_analog_power_down();
    adac_reset();

    osal_iounmap((hi_void *)g_reg_adac);
    g_reg_adac = HI_NULL;
}

static adac_platform_driver g_adac_driver = {
    .version = ADAC_TIANLAI_SV100,
#ifdef HI_SND_MUTECTL_SUPPORT
    .fast_power_enable = fast_power_enable,
#endif
    .init = adac_init,
    .deinit = adac_deinit,
};

hi_void adac_tianlai_sv100_register_driver(struct osal_list_head *head)
{
    osal_list_add_tail(&g_adac_driver.node, head);
}


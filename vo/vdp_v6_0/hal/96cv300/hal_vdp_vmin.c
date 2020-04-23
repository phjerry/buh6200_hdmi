/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "drv_xdp_osal.h"
#include "hal_vdp_vmin.h"

#ifndef __DISP_PLATFORM_BOOT__
#include <asm/io.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define get_min(a, b)   ((a) < (b) ? (a) : (b))
#define get_max(a, b)   ((a) > (b) ? (a) : (b))

hi_u32 hal_get_vdp_hmp(hi_u8 *pmc_vir,hi_u8 *opt_vir)
{
    hi_u32 test_hmp;
    hi_u32 otp_hmp;
    hi_u32 data;
    hi_u32 hmp;

    *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL0) = HPM7_CTRL0_VAL0;
    *(volatile hi_u32 *)(pmc_vir + PWM7_CTRL0) = PWM7_CTRL0_VAL;
    *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL3) = HPM7_CTRL3_VAL;
    *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL4) = HPM7_CTRL4_VAL;
    *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL0) = HPM7_CTRL0_VAL1;

    disp_udelay(1000); /* delay 1000 us */

    data = *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL0);
    data = *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL1);
    test_hmp = data & HPM_VALUE;
    test_hmp += (data >> HPM_BIT_OFS) & HPM_VALUE;
    data = *(volatile hi_u32 *)(pmc_vir + HPM7_CTRL2);
    test_hmp += data & HPM_VALUE;
    test_hmp += (data >> HPM_BIT_OFS) & HPM_VALUE;
    test_hmp = test_hmp / 4; /* div 4 get avg val */

    otp_hmp = *(volatile hi_u32 *)(opt_vir + OPT_VDP_HMP);
    otp_hmp = (otp_hmp >> OPT_VDP_OFS) & HPM_VALUE;

    if (otp_hmp == 0) {
        hmp = test_hmp;
    } else {
        hmp = get_min(test_hmp, otp_hmp);
    }

    return hmp;
}

hi_void hal_set_vdp_vmin(hi_void)
{
    hi_u32 comp_en;
    hi_u32 comp_val;
    hi_u32 hmp;
    hi_u32 vmin;
    hi_u32 pwm;
    hi_u32 const_abs;
    hi_u32 ratio_abs;
    hi_u8 *gen_vir = HI_NULL;
    hi_u8 *pmc_vir = HI_NULL;
    hi_u8 *opt_vir = HI_NULL;

    /* test hpm */
    pmc_vir = osal_ioremap_nocache(PMC_ADDR, PMC_LEN);

    /* opt hpm */
    opt_vir = osal_ioremap_nocache(OPT_ADDR, OPT_LEN);
    hmp = hal_get_vdp_hmp(pmc_vir, opt_vir);

    comp_val = 0;
    comp_en = *(volatile hi_u32 *)(opt_vir + OPT_COMP_EN);
    comp_en = (comp_en >> OPT_COMP_EN_OFS) & 0x1; /* 4 comp en ofs */
    if (comp_en) {
        comp_val = *(volatile hi_u32 *)(opt_vir + OPT_COMP_VAL);
        comp_val = comp_val >> OPT_COMP_VAL_OFS;
        if ((comp_val >> 3) & 1) { /* 3 comp en ofs */
            comp_val = ((comp_val & 7) + 1) * 10; /* 7:3bit, 10:mv */
        }
    }

    /* ratio para */
    gen_vir = osal_ioremap_nocache(GEN_ADDR, GEN_LEN);
    const_abs = *(volatile hi_u32 *)(gen_vir) & CONST_VAL;
    ratio_abs = *(volatile hi_u32 *)(gen_vir) >> RATIO_BIT;
    osal_iounmap(gen_vir);

    if (const_abs == 0 || ratio_abs == 0) {
        const_abs = CONST_ABS;
        ratio_abs = RATIO_ABS;
    }

    vmin = const_abs - ratio_abs * hmp / 1000; /* div 1000 to float */
    vmin += comp_val;
    vmin = get_min(VOL_MAX, get_max(vmin, VOL_MIN));

    /* vmin to pwm */
    pwm = 1 + (VOL_MAX - vmin) * VOL_STEP / VOL_MIN * 2; /* 2 double */
    pwm = (pwm << 16) + 0xdd; /* 16 bit 0xdd ofs */

    *(volatile hi_u32 *)(pmc_vir + PWM7_CTRL0) = pwm;
    disp_udelay(10); /* delay 10us */
    *(volatile hi_u32 *)(pmc_vir + PWM7_MUX_CTRL) = 0x7; /* 0x7 ctrl pwm7 */
    disp_udelay(10); /* delay 10us */

    osal_iounmap(opt_vir);
    osal_iounmap(pmc_vir);
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

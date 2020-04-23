/*
* Copyright (c) Hisilicon Tech. Co., Ltd. 20019-2019 All rights reserved.
* Description   : drv_stc_hal
* Author        : Hisilicon multimedia software group
* Create        : 2019/12/13
*/

#include "drv_stc_hal.h"
#include "hi_drv_sys.h"

/* 27M clock (vpll) */
void stc_hal_read_basic_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    fbdiv = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL25.u32;
    *int_val = fbdiv & (0xfff);       /* 0xfff:int value is bite 0 ~ 11 */

    frac = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL24.u32;
    *frac_val = frac & (0xffffff);    /* 0xffffff:frac value is bite 0 ~ 23 */

    return;
}

void stc_hal_read_basic_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    *int_val  = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG115.bits.vpll_tune_int_cfg;
    *frac_val = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG116.bits.vpll_tune_frac_cfg;

    return;
}

void stc_hal_write_basic_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val)
{
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG115.bits.vpll_tune_int_cfg  = int_val;
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG116.bits.vpll_tune_frac_cfg = frac_val;

    return;
}

void stc_hal_tune_basic_pll_freq(hi_u32 fbdiv, hi_u32 frac)
{
    stc_hal_write_basic_pll_tune_freq_crg(fbdiv, frac);

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG117.bits.vpll_tune_step_int  = 0;        /* 0: step int */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG118.bits.vpll_tune_step_frac = 0x10;     /* 0x10: step frac */

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_en        = 0;        /* 0->1: tune frq */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_cken      = 1;        /* 1: clock enable */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_srst_req  = 0;        /* 0: soft rest disable */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_mode      = 1;        /* 1: enable tune frq mode */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_divval    = 1;        /* 1: tune frq div value */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_soft_cfg  = 0;        /* 0: disable soft config */

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG119.bits.vpll_tune_en = 0x1;             /* 0->1: tune frq */
    return;
}

hi_bool stc_hal_is_basic_pll_tune_freq_enable(void)
{
    hi_u32 value;

    value = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL25.u32;
    value = (value >> 25) & 0x01;    /* [25] bit:tune frq enable flag */

    return (value != 0) ? HI_TRUE : HI_FALSE;
}

hi_bool stc_hal_is_basic_pll_tune_freq_busy(void)
{
    return hi_drv_sys_get_crg_reg_ptr()->PERI_CRG514.bits.vpll_tune_busy;
}

/* audio output ctrl pll (epll) */
void stc_hal_read_aud_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    fbdiv = hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_1.u32;
    *int_val = fbdiv & (0xfff);       /* 0xfff:int value is bite 0 ~ 11 */

    frac = hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_0.u32;
    *frac_val = frac & (0xffffff);    /* 0xffffff:frac value is bite 0 ~ 23 */

    return;
}

void stc_hal_read_aud_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    *int_val  = hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_4.bits.epll_tune_int_cfg;
    *frac_val = hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_5.bits.epll_tune_frac_cfg;

    return;
}

void stc_hal_write_aud_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val)
{
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_4.bits.epll_tune_int_cfg  = int_val;
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_5.bits.epll_tune_frac_cfg = frac_val;

    return;
}

void stc_hal_tune_aud_pll_freq(hi_u32 fbdiv, hi_u32 frac)
{
    stc_hal_write_aud_pll_tune_freq_crg(fbdiv, frac);

    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_6.bits.epll_tune_step_int  = 0;        /* 0: step int */
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_7.bits.epll_tune_step_frac = 0x10;     /* 0x10: step frac */

    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_en        = 0;        /* 0->1: tune frq */
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_cken      = 1;        /* 1: clock enable */
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_srst_req  = 0;        /* 0: soft rest disable */
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_mode      = 1;        /* 1: enable tune frq mode */
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_divval    = 1;        /* 1: tune frq div value */
    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_soft_cfg  = 0;        /* 0: disable soft config */

    hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_8.bits.epll_tune_en = 0x1;             /* 0->1: tune frq */
    return;
}

hi_bool stc_hal_is_aud_pll_tune_freq_enable(void)
{
    hi_u32 value;

    value = hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_1.u32;
    value = (value >> 25) & 0x01;    /* [25] bit:tune frq enable flag */

    return (value != 0) ? HI_TRUE : HI_FALSE;
}

hi_bool stc_hal_is_aud_pll_tune_freq_busy(void)
{
    return hi_drv_sys_get_ctrl_reg_ptr()->SC_CRG_PLL_9.bits.epll_tune_busy;
}

/* tx0 output pll */
void stc_hal_read_vid_port0_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    fbdiv = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL13.u32;
    *int_val = fbdiv & (0xfff);       /* 0xfff:int value is bite 0 ~ 11 */

    frac = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL12.u32;
    *frac_val = frac & (0xffffff);    /* 0xffffff:frac value is bite 0 ~ 23 */

    return;
}

void stc_hal_read_vid_port0_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    *int_val  = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG100.bits.hpll0_tune_int_cfg;
    *frac_val = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG101.bits.hpll0_tune_frac_cfg;

    return;
}

void stc_hal_write_vid_port0_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val)
{
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG100.bits.hpll0_tune_int_cfg  = int_val;
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG101.bits.hpll0_tune_frac_cfg = frac_val;

    return;
}

void stc_hal_tune_vid_port0_pll_freq(hi_u32 fbdiv, hi_u32 frac)
{
    stc_hal_write_vid_port0_pll_tune_freq_crg(fbdiv, frac);

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG102.bits.hpll0_tune_step_int  = 0;        /* 0x0: step int */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG103.bits.hpll0_tune_step_frac = 0x10;     /* 0x10: step frac */

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_en        = 0;        /* 0->1: tune frq */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_cken      = 1;        /* 1: clock enable */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_srst_req  = 0;        /* 0: soft rest disable */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_mode      = 1;        /* 1: enable tune frq mode */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_divval    = 1;        /* 1: tune frq div value */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_soft_cfg  = 0;        /* 0: disable soft config */

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_en = 0x1;             /* 0->1: tune frq */

    return;
}

void stc_hal_disable_tune_vid_port0_pll_freq(void)
{
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG104.bits.hpll0_tune_mode = 0x0;           /* 0x0: disable tune mode */

    return;
}

hi_bool stc_hal_is_vid_port0_tune_freq_enable(void)
{
    hi_u32 value;

    value = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL13.u32;
    value = (value >> 25) & 0x01;     /* [25] bit:tune frq enable flag */

    return (value != 0) ? HI_TRUE : HI_FALSE;
}

hi_bool stc_hal_is_vid_port0_pll_tune_freq_busy(void)
{
    return hi_drv_sys_get_crg_reg_ptr()->PERI_CRG508.bits.hpll0_tune_busy;
}

/* tx1 ouput pll */
void stc_hal_read_vid_port1_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    fbdiv = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL16.u32;
    *int_val = fbdiv & (0xfff);       /* 0xfff:int value is bite 0 ~ 11 */

    frac = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL15.u32;
    *frac_val = frac & (0xffffff);    /* 0xffffff:frac value is bite 0 ~ 23 */

    return;
}

void stc_hal_read_vid_port1_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val)
{
    *int_val  = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG105.bits.hpll1_tune_int_cfg;
    *frac_val = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG106.bits.hpll1_tune_frac_cfg;

    return;
}

void stc_hal_write_vid_port1_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val)
{
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG105.bits.hpll1_tune_int_cfg  = int_val;
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG106.bits.hpll1_tune_frac_cfg = frac_val;

    return;
}

void stc_hal_tune_vid_port1_pll_freq(hi_u32 fbdiv, hi_u32 frac)
{
    stc_hal_write_vid_port1_pll_tune_freq_crg(fbdiv, frac);

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG107.bits.hpll1_tune_step_int  = 0;        /* 0: step int */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG108.bits.hpll1_tune_step_frac = 0x10;     /* 0x10: step frac */

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_en        = 0;        /* 0->1: tune frq */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_cken      = 1;        /* 1: clock enable */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_srst_req  = 0;        /* 0: soft rest disable */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_mode      = 1;        /* 1: enable tune frq mode */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_divval    = 1;        /* 1: tune frq div value */
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_soft_cfg  = 0;        /* 0: disable soft config */

    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_en = 0x1;             /* 0->1: tune frq */

    return;
}

void stc_hal_disable_tune_vid_port1_pll_freq(void)
{
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG109.bits.hpll1_tune_mode = 0x0;           /* 0x0: disable tune mode */

    return;
}

hi_bool stc_hal_is_vid_port1_tune_freq_enable(void)
{
    hi_u32 value;

    value = hi_drv_sys_get_crg_reg_ptr()->PERI_CRG_PLL16.u32;
    value = (value >> 25) & 0x01;     /* [25] bit:tune frq enable flag */

    return (value != 0) ? HI_TRUE : HI_FALSE;
}

hi_bool stc_hal_is_vid_port1_pll_tune_freq_busy(void)
{
    return hi_drv_sys_get_crg_reg_ptr()->PERI_CRG510.bits.hpll1_tune_busy;
}


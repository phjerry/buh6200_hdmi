/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: clk spread configuration
 * Author: BSP
 * Create: 2019-11-29
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <asm/io.h>
#include "hi_debug.h"
#include "hi_drv_sys.h"
#include "hi_reg_common.h"
#include "hi_drv_spread.h"
#include "hi_osal.h"

/* ---------------- clk spread ---------------- */
#ifndef hi_ss_reg_write_bits
#define hi_ss_reg_write_bits(pu32_vir_reg_addr, value, mask) \
    do { \
        hi_u32 reg_value_ = 0; \
        reg_value_ = osal_readl(pu32_vir_reg_addr); \
        reg_value_ &= ~(mask); \
        reg_value_ |= (value) & (mask); \
        osal_writel(reg_value_, pu32_vir_reg_addr); \
    } while (0)
#endif

#ifndef hi_ss_reg_read_bits
#define hi_ss_reg_read_bits(pu32_vir_reg_addr, off_set, mask, pu32_value) \
    do { \
        hi_u32 reg_value_ = 0; \
        reg_value_ = osal_readl(pu32_vir_reg_addr); \
        *(pu32_value) = (reg_value_ >> (off_set)) & (mask); \
    } while (0)
#endif

#define SS_FREQ_MASK   0x0F
#define SS_FREQ_OFFSET 0x09

#define SS_RATIO_MASK   0x1F
#define SS_RATIO_OFFSET 0x04

#define SS_DOWN_MASK   0x01
#define SS_DOWN_OFFSET 0x03

#define SS_ENABLE_MASK   0x01
#define SS_ENABLE_OFFSET 0x02

#define SS_RST_MASK   0x01
#define SS_RST_OFFSET 0x01

#define SS_CLK_MASK   0x01
#define SS_CLK_OFFSET 0x00

#define CI_CLK_MASK   0x01
#define CI_CLK_OFFSET 0x04

#define CI_RST_MASK   0x01
#define CI_RST_OFFSET 0x01

/* [0-31] 0：0；1：0.1%；2：0.2%3：0.3%；4：0.4%；5：0.5%；6：0.6%；7：0.7%...31：3.1% */
#define SS_SPREAD_RATIO_MAX 31
static hi_u32 g_ddr_spread_ratio_max = 10;

/* [2-5] 93KHZ,62KHZ,46KHZ,37KHZ,31KHZ */
#define SS_SPREAD_FREQ_MIN 2
#define SS_SPREAD_FREQ_MAX 5

/* wait at least 8*41.66*REFDIV(ns) */
#define SS_WAIT_US 5

#define DDR_SS_CTRL_OFFSET   0x27C /* DDR spread ctrl register */
#define EMMC_SS_CTRL_OFFSET  0x1B8 /* EMMC spread ctrl register */
#define GMAC_SS_CTRL_OFFSET  0x134 /* GMAC spread ctrl register */
/* #define GMAC_CLK_CTRL_OFFSET 0x0CC */ /* GMAC spread ctrl register */
#define CI_CRG_CLK_OFFSET    0x188 /* CI spread ctrl register */
#define I2C_CRG_CLK_OFFSET   0x06C

hi_s32 hi_drv_ss_set_ddr_max_ratio(hi_u32 max_ratio)
{
    if (max_ratio < 0 && max_ratio > SS_SPREAD_RATIO_MAX) {
        hi_err_spread("max_ratio is invalid!\n");
        return HI_FAILURE;
    }

    g_ddr_spread_ratio_max = max_ratio;

    hi_dbg_spread("g_ddr_spread_ratio_max is %u!\n", g_ddr_spread_ratio_max);

    return HI_SUCCESS;
}

/* open/close clk */
hi_s32 hi_drv_ss_set_clk_en(hi_u32 *pu32_vir_reg_addr, hi_bool b_enable)
{
    if (pu32_vir_reg_addr == HI_NULL) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }

    if (b_enable) {
        hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_TRUE << SS_CLK_OFFSET), (SS_CLK_MASK << SS_CLK_OFFSET));
        hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_FALSE << SS_RST_OFFSET), (SS_RST_MASK << SS_RST_OFFSET));
    } else {
        hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_FALSE << SS_CLK_OFFSET), (SS_CLK_MASK << SS_CLK_OFFSET));
        hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_TRUE << SS_RST_OFFSET), (SS_RST_MASK << SS_RST_OFFSET));
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_ss_set_spread_en(hi_u32 *pu32_vir_reg_addr, hi_bool b_enable)
{
    hi_s32 ret;
    hi_u64 enable;
    if (pu32_vir_reg_addr == HI_NULL) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }

    hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_TRUE << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_FALSE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_TRUE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    udelay(SS_WAIT_US);
    enable = (~(hi_u64)b_enable);
    hi_ss_reg_write_bits(pu32_vir_reg_addr, (enable << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));

    return HI_SUCCESS;
}

/* 0. read the status of spread enable */
/* 1. stop spread */
/* 2. close ssmod clk */
/* 3. config spread ratio */
/* 4. open ssmod clk */
/* 5. wait at least 8*41.66*REFDIV(ns) before opening spread */
hi_s32 hi_drv_ss_set_spread_down(hi_u32 *pu32_vir_reg_addr, hi_bool b_enable)
{
    hi_s32 ret;
    hi_u32 reg_value;
    hi_bool b_spread_disable;
    if (pu32_vir_reg_addr == HI_NULL) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }

    reg_value = osal_readl(pu32_vir_reg_addr);
    b_spread_disable = (reg_value & (SS_ENABLE_MASK << SS_ENABLE_OFFSET)) ? HI_TRUE : HI_FALSE;
    hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_TRUE << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_FALSE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    hi_ss_reg_write_bits(pu32_vir_reg_addr, ((hi_u32)(b_enable) << SS_DOWN_OFFSET), (SS_DOWN_MASK << SS_DOWN_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_TRUE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    udelay(SS_WAIT_US);
    hi_ss_reg_write_bits(pu32_vir_reg_addr,
        ((hi_u32)(b_spread_disable) << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));

    return HI_SUCCESS;
}

/* 0. read the status of spread enable */
/* 1. stop spread */
/* 2. close ssmod clk */
/* 3. config spread ratio */
/* 4. open ssmod clk */
/* 5. wait at least 8*41.66*REFDIV(ns) before opening spread */
hi_s32 hi_drv_ss_set_spread_ratio(hi_u32 *pu32_vir_reg_addr, hi_u32 spread_ratio)
{
    hi_s32 ret;
    hi_u32 reg_value;
    hi_bool b_spread_disable;
    if (pu32_vir_reg_addr == HI_NULL) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }

    reg_value = osal_readl(pu32_vir_reg_addr);
    b_spread_disable = (reg_value & (SS_ENABLE_MASK << SS_ENABLE_OFFSET)) ? HI_TRUE : HI_FALSE;
    hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_TRUE << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_FALSE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    hi_ss_reg_write_bits(pu32_vir_reg_addr, (spread_ratio << SS_RATIO_OFFSET), (SS_RATIO_MASK << SS_RATIO_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_TRUE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    udelay(SS_WAIT_US);
    hi_ss_reg_write_bits(pu32_vir_reg_addr,
        ((hi_u32)(b_spread_disable) << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));

    return HI_SUCCESS;
}

/* 0. read the status of spread enable */
/* 1. stop spread */
/* 2. close ssmod clk */
/* 3. config spread freq */
/* 4. open ssmod clk */
/* 5. wait at least 8*41.66*REFDIV(ns) before opening spread */
hi_s32 hi_drv_ss_set_spread_freq(hi_u32 *pu32_vir_reg_addr, hi_u32 spread_freq)
{
    hi_s32 ret;
    hi_u32 reg_value;
    hi_bool b_spread_disable;
    if (pu32_vir_reg_addr == HI_NULL) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }

    reg_value = osal_readl(pu32_vir_reg_addr);
    b_spread_disable = (reg_value & (SS_ENABLE_MASK << SS_ENABLE_OFFSET)) ? HI_TRUE : HI_FALSE;
    hi_ss_reg_write_bits(pu32_vir_reg_addr, (HI_TRUE << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_FALSE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    hi_ss_reg_write_bits(pu32_vir_reg_addr, (spread_freq << SS_FREQ_OFFSET), (SS_FREQ_MASK << SS_FREQ_OFFSET));
    ret = hi_drv_ss_set_clk_en(pu32_vir_reg_addr, HI_TRUE);
    if (ret == HI_FAILURE) {
        hi_err_spread("pu32_vir_reg_addr is null point!\n");
        return HI_FAILURE;
    }
    udelay(SS_WAIT_US);
    hi_ss_reg_write_bits(pu32_vir_reg_addr,
        ((hi_u32)(b_spread_disable) << SS_ENABLE_OFFSET), (SS_ENABLE_MASK << SS_ENABLE_OFFSET));

    return HI_SUCCESS;
}

/* ---------------- DDR spread func ---------------- */
hi_s32 hi_drv_ss_set_ddr_spread_en(hi_bool *pb_enable)
{
    hi_s32 ret;
    hi_bool b_enable;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pb_enable == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    b_enable = *pb_enable;
    ret = hi_drv_ss_set_spread_en((hi_void *)reg_crg + DDR_SS_CTRL_OFFSET, b_enable);

    return ret;
}

hi_s32 hi_drv_ss_set_ddr_spread_ratio(hi_u32 *pu32_spread_ratio)
{
    hi_s32 ret;
    hi_u32 spread_ratio;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pu32_spread_ratio == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    spread_ratio = *pu32_spread_ratio;
    if (spread_ratio > g_ddr_spread_ratio_max) {
        hi_err_spread("ddr_spread_ratio[%d] is an invalid value\n", spread_ratio);
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_spread_ratio((hi_void *)reg_crg + DDR_SS_CTRL_OFFSET, spread_ratio);

    return ret;
}

hi_s32 hi_drv_ss_set_ddr_spread_freq(hi_u32 *pu32_spread_freq)
{
    hi_s32 ret;
    hi_u32 spread_freq;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pu32_spread_freq == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    spread_freq = *pu32_spread_freq;

    if ((spread_freq > SS_SPREAD_FREQ_MAX) || (spread_freq < SS_SPREAD_FREQ_MIN)) {
        hi_err_spread("ddr_spread_freq is an invalid value\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_spread_freq((hi_void *)reg_crg + DDR_SS_CTRL_OFFSET, spread_freq);

    return ret;
}

/* ---------------- GMAC spread func ---------------- */
hi_s32 hi_drv_ss_set_gmac_clk_en(hi_bool *pb_enable)
{
    hi_s32 ret;
    hi_bool b_enable;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pb_enable == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    b_enable = *pb_enable;
    ret = hi_drv_ss_set_clk_en((hi_void *)reg_crg + GMAC_SS_CTRL_OFFSET, b_enable);

    return ret;
}

hi_s32 hi_drv_ss_set_gmac_spread_en(hi_bool *pb_enable)
{
    hi_s32 ret;
    hi_bool b_enable;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pb_enable == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    b_enable = *pb_enable;
    ret = hi_drv_ss_set_spread_en((hi_void *)reg_crg + GMAC_SS_CTRL_OFFSET, b_enable);

    return ret;
}

hi_s32 hi_drv_ss_set_gmac_spread_ratio(hi_u32 *pu32_spread_ratio)
{
    hi_s32 ret;
    hi_u32 spread_ratio;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pu32_spread_ratio == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    spread_ratio = *pu32_spread_ratio;

    if (spread_ratio > SS_SPREAD_RATIO_MAX) {
        hi_err_spread("gmac_spread_ratio is an invalid value\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_spread_ratio((hi_void *)reg_crg + GMAC_SS_CTRL_OFFSET, spread_ratio);

    return ret;
}

hi_s32 hi_drv_ss_set_gmac_spread_freq(hi_u32 *pu32_spread_freq)
{
    hi_s32 ret;
    hi_u32 spread_freq;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pu32_spread_freq == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    spread_freq = *pu32_spread_freq;

    if ((spread_freq > SS_SPREAD_FREQ_MAX) || (spread_freq < SS_SPREAD_FREQ_MIN)) {
        hi_err_spread("gmac_spread_freq is an invalid value\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_spread_freq((hi_void *)reg_crg + GMAC_SS_CTRL_OFFSET, spread_freq);

    return ret;
}

/* ---------------- EMMC spread func ---------------- */
hi_s32 hi_drv_ss_set_emmc_clk_en(hi_bool *pb_enable)
{
    hi_s32 ret;
    hi_bool b_enable;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pb_enable == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    b_enable = *pb_enable;
    ret = hi_drv_ss_set_clk_en((hi_void *)reg_crg + EMMC_SS_CTRL_OFFSET, b_enable);

    return ret;
}

hi_s32 hi_drv_ss_set_emmc_spread_en(hi_bool *pb_enable)
{
    hi_s32 ret;
    hi_bool b_enable;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pb_enable == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    b_enable = *pb_enable;
    ret = hi_drv_ss_set_spread_en((hi_void *)reg_crg + EMMC_SS_CTRL_OFFSET, b_enable);

    return ret;
}

hi_s32 hi_drv_ss_set_emmc_spread_ratio(hi_u32 *pu32_spread_ratio)
{
    hi_s32 ret;
    hi_u32 spread_ratio;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pu32_spread_ratio == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    spread_ratio = *pu32_spread_ratio;

    if (spread_ratio > SS_SPREAD_RATIO_MAX) {
        hi_err_spread("emmc_spread_ratio is an invalid value\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_spread_ratio((hi_void *)reg_crg + EMMC_SS_CTRL_OFFSET, spread_ratio);

    return ret;
}

hi_s32 hi_drv_ss_set_emmc_spread_freq(hi_u32 *pu32_spread_freq)
{
    hi_s32 ret;
    hi_u32 spread_freq;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pu32_spread_freq == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    spread_freq = *pu32_spread_freq;

    if ((spread_freq > SS_SPREAD_FREQ_MAX) || (spread_freq < SS_SPREAD_FREQ_MIN)) {
        hi_err_spread("emmc_spread_freq is an invalid value\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_spread_freq((hi_void *)reg_crg + EMMC_SS_CTRL_OFFSET, spread_freq);

    return ret;
}

/* ---------------- CI spread func ---------------- */
hi_s32 hi_drv_ss_set_ci_clk_en(hi_bool *pb_enable)
{
    hi_bool b_enable;
    hi_void *reg_crg = (hi_void *)hi_drv_sys_get_crg_reg_ptr();

    if (pb_enable == HI_NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    b_enable = *pb_enable;
    if (b_enable) {
        hi_ss_reg_write_bits((hi_void *)reg_crg + CI_CRG_CLK_OFFSET, (HI_TRUE << CI_CLK_OFFSET),
                            (CI_CLK_MASK << CI_CLK_OFFSET));
        hi_ss_reg_write_bits((hi_void *)reg_crg + CI_CRG_CLK_OFFSET, (HI_FALSE << CI_RST_OFFSET),
                            (CI_RST_MASK << CI_RST_OFFSET));
    } else {
        hi_ss_reg_write_bits((hi_void *)reg_crg + CI_CRG_CLK_OFFSET, (HI_TRUE << CI_RST_OFFSET),
                            (CI_RST_MASK << CI_RST_OFFSET));
        hi_ss_reg_write_bits((hi_void *)reg_crg + CI_CRG_CLK_OFFSET, (HI_FALSE << CI_CLK_OFFSET),
                            (CI_CLK_MASK << CI_CLK_OFFSET));
    }

    return HI_SUCCESS;
}

EXPORT_SYMBOL(hi_drv_ss_set_ddr_spread_en);
EXPORT_SYMBOL(hi_drv_ss_set_ddr_spread_ratio);
EXPORT_SYMBOL(hi_drv_ss_set_ddr_spread_freq);

EXPORT_SYMBOL(hi_drv_ss_set_gmac_clk_en);
EXPORT_SYMBOL(hi_drv_ss_set_gmac_spread_en);
EXPORT_SYMBOL(hi_drv_ss_set_gmac_spread_ratio);
EXPORT_SYMBOL(hi_drv_ss_set_gmac_spread_freq);

EXPORT_SYMBOL(hi_drv_ss_set_emmc_clk_en);
EXPORT_SYMBOL(hi_drv_ss_set_emmc_spread_en);
EXPORT_SYMBOL(hi_drv_ss_set_emmc_spread_ratio);
EXPORT_SYMBOL(hi_drv_ss_set_emmc_spread_freq);
EXPORT_SYMBOL(hi_drv_ss_set_ci_clk_en);

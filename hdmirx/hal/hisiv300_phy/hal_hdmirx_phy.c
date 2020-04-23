/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of phy functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/3
 */
#include "hal_hdmirx_comm.h"
#include "hal_hdmirx_damix_reg.h"
#include "hi_drv_hdmirx.h"

#define FIX_EQ_CNT 7 /* 7: 8 fix eq code for sel */

static hi_u8 g_fix_code[3][8] = { /* 3: 3 case 25-42.5-75; 8: 8 cfg 8 */
    /* 8 fix code for phy sel */
    { 0x8, 0xa, 0xc, 0x6, 0x4, 0x2, 0x1, 0x0 }, /* HDMI1.4: 25M-42.5M */
    { 0xa, 0xc, 0xc, 0x6, 0x4, 0x2, 0x1, 0x0 }, /* HDMI1.4: 42.5M-75M */
    { 0xc, 0xa, 0x8, 0x6, 0x4, 0x2, 0x1, 0x0 }, /* HDMI1.4: 75M-150M */
};

static hi_u8 g_pd_cfg[10][8] = { /* 10 power mode cfg for HDMI input , 8 for regs */
    /* RG_PD_RT, RG_PD_PHDAC, RG_PD_LDO, RG_PD_LANE, RG_PD_CLK, RG_PD_BG, RG_PD_PLL, RG_PD_LDO_NODIE */
    { 0xf, 0xf, 0x1, 0xf, 0x1, 0x1, 0x1, 0x1 }, /* OFF */
    { 0x0, 0x8, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0 }, /* TMDS1.4 */
    { 0x0, 0xf, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0 }, /* TMDS2.0 */
    { 0x0, 0xf, 0x0, 0x8, 0x1, 0x0, 0x0, 0x0 }, /* FRL3L3G */
    { 0x0, 0xf, 0x0, 0x8, 0x1, 0x0, 0x0, 0x0 }, /* FRL3L6G */
    { 0x0, 0xf, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }, /* FRL4L6G */
    { 0x0, 0xf, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }, /* FRL4L8G */
    { 0x0, 0xf, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }, /* FRL4L10G */
    { 0x0, 0xf, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0 }, /* FRL4L12G */
    { 0xf, 0xf, 0x1, 0xf, 0x1, 0x1, 0x1, 0x1 }, /* MAX */
};

static hdmirx_clk_zone hal_damix_clk_zone_detect(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hdmirx_clk_zone clk_zone = 0;
    hi_u8 value;

    value = hdmirx_hal_damix_reg_read_fld_align(port, REG_CLK_DETECT0, HDMI_ZONE_CTRL);
    switch (type) {
        case HDMIRX_INPUT_14:
            if (value == 0x0) {
                clk_zone = HDMIRX_CLK_ZONE_TMDS14_25_42P5;
            } else if (value == 0x1) { /* 0x1: TMDS1.4 42.5M - 75M */
                clk_zone = HDMIRX_CLK_ZONE_TMDS14_42P5_75;
            } else if (value == 0x2) { /* 0x2: TMDS1.4 75M - 150M */
                clk_zone = HDMIRX_CLK_ZONE_TMDS14_75_150;
            } else if (value == 0x3) { /* 0x3: TMDS1.4 150M - 250M */
                clk_zone = HDMIRX_CLK_ZONE_TMDS14_150_250;
            } else if (value == 0x4) { /* 0x4: TMDS1.4 250M - 340M */
                clk_zone = HDMIRX_CLK_ZONE_TMDS14_250_340;
            }
            break;
        case HDMIRX_INPUT_20:
            if (value == 0x0) {
                clk_zone = HDMIRX_CLK_ZONE_TMDS20_85_100;
            } else if (value == 0x1) { /* 0x1: TMDS2.0 100M - 200M */
                clk_zone = HDMIRX_CLK_ZONE_TMDS20_100_150;
            }
            break;
        case HDMIRX_INPUT_FRL3L3G:
        case HDMIRX_INPUT_FRL3L6G:
        case HDMIRX_INPUT_FRL4L6G:
        case HDMIRX_INPUT_FRL4L8G:
        case HDMIRX_INPUT_FRL4L10G:
        case HDMIRX_INPUT_FRL4L12G:
            if (value == 0x0) {
                clk_zone = HDMIRX_CLK_ZONE_FRL_3G;
            } else if (value == 0x1) { /* 0x1: FRL 6G */
                clk_zone = HDMIRX_CLK_ZONE_FRL_6G;
            } else if (value == 0x2) { /* 0x2: FRL 8G */
                clk_zone = HDMIRX_CLK_ZONE_FRL_8G;
            } else if (value == 0x3) { /* 0x3: FRL 10G */
                clk_zone = HDMIRX_CLK_ZONE_FRL_10G;
            } else if (value == 0x4) { /* 0x4: FRL 12G */
                clk_zone = HDMIRX_CLK_ZONE_FRL_12G;
            }
            break;
        default:
            break;
    }
    return clk_zone;
}

static hi_void hal_damix_adaptive_eq_mode(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A3, REG_DA_HDMI14_EQ_SEL0, en);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A3, REG_DA_HDMI14_EQ_SEL1, en);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A3, REG_DA_HDMI14_EQ_SEL2, en);
}

static hi_void hal_cal_amp_en(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hi_u8 tmds_clk;

    tmds_clk = hal_damix_clk_zone_detect(port, type);
    if (type == HDMIRX_INPUT_14) {
        if ((tmds_clk == HDMIRX_CLK_ZONE_TMDS14_25_42P5) ||
            (tmds_clk == HDMIRX_CLK_ZONE_TMDS14_42P5_75) ||
            (tmds_clk == HDMIRX_CLK_ZONE_TMDS14_75_150)) {
            hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG9, RG_EN_CAL_AMP, 0x0); /* 0x0: for HDMI14 low */
        } else {
            hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG9, RG_EN_CAL_AMP, 0x7); /* 0x7: for HDMI20 */
        }
    } else if ((type == HDMIRX_INPUT_20) || (type == HDMIRX_INPUT_FRL3L3G) ||
        (type == HDMIRX_INPUT_FRL3L6G)) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG9, RG_EN_CAL_AMP, 0x7); /* 0x7: for FRL 3L */
    } else if ((type == HDMIRX_INPUT_FRL4L6G) || (type == HDMIRX_INPUT_FRL4L8G) ||
        (type == HDMIRX_INPUT_FRL4L10G) || (type == HDMIRX_INPUT_FRL4L12G)) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG9, RG_EN_CAL_AMP, 0xf); /* 0xF: for FRL4L */
    }
}

static hi_void hal_rterm_ctrl_set(hi_drv_hdmirx_port port)
{
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG9, RG_RT_CTRL, 0x8);     /* 0x8: RT */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG11, RG_RT_CTRL_CK, 0x8); /* 0x8: RT CK */
}

static hi_void hal_pd_calibration_set(hi_drv_hdmirx_port port)
{
    hdmirx_hal_damix_reg_set_bits(port, REG_ANALOG7, 0x4, HI_TRUE); /* 0x4: ATOP bit Set */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG11, RG_CAL_START, HI_FALSE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG11, RG_CAL_START, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG11, RG_CAL_START, HI_FALSE);
}

static hi_void hal_pll_ref_clk_set(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    if ((type == HDMIRX_INPUT_14) || (type == HDMIRX_INPUT_20)) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L0, 0x0);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L1, 0x0);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L2, 0x0);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L3, 0x0);
    } else if ((type == HDMIRX_INPUT_FRL3L3G) || (type == HDMIRX_INPUT_FRL3L6G)) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L0, 0x1);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L1, 0x1);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L2, 0x1);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L3, 0x0);
    } else if ((type == HDMIRX_INPUT_FRL4L6G) || (type == HDMIRX_INPUT_FRL4L8G) ||
        (type == HDMIRX_INPUT_FRL4L10G) || (type == HDMIRX_INPUT_FRL4L12G)) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L0, 0x1);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L1, 0x1);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L2, 0x1);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG0, RG_SEL_CLKIN_L3, 0x1);
    }
}

static hi_void hal_damix_div_post_set(hi_drv_hdmirx_port port, hi_bool div_post_en)
{
    if (div_post_en == HI_TRUE) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN0, HI_TRUE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN1, HI_TRUE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN2, HI_TRUE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN3, HI_TRUE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST0, 0x0);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST1, 0x0);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST2, 0x0);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST3, 0x0);
    } else {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN0, HI_FALSE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN1, HI_FALSE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN2, HI_FALSE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A1, REG_DA_DIVSEL_POST_EN3, HI_FALSE);
    }
}

static hi_void hal_damix_sweep_set(hi_drv_hdmirx_port port)
{
    hdmirx_hal_damix_reg_write_fld_align(port, REG_SWEEP_EQ_CFG, REG_EQ_SWEEP_MODE, HI_FALSE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_SWEEP_EQ_CFG, REG_EQ_ERR_CHECK_MODE, HI_TRUE);
}

static hi_void hal_damix_sweep_clr(hi_drv_hdmirx_port port)
{
    hdmirx_hal_damix_reg_write_fld_align(port, REG_SWEEP_EQ_CFG, REG_EQ_SWEEP_MODE, HI_FALSE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_SWEEP_EQ_CFG, REG_EQ_ERR_CHECK_MODE, HI_FALSE);
}

static hi_void hal_damix_set_rctrl(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    switch (type) {
        case HDMIRX_INPUT_14:
        case HDMIRX_INPUT_20:
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL_EN0, HI_FALSE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL_EN1, HI_FALSE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL_EN2, HI_FALSE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_RCTRL_PLL_EN3, HI_FALSE);
            break;
        case HDMIRX_INPUT_FRL3L3G:
        case HDMIRX_INPUT_FRL3L6G:
        case HDMIRX_INPUT_FRL4L6G:
        case HDMIRX_INPUT_FRL4L8G:
        case HDMIRX_INPUT_FRL4L10G:
        case HDMIRX_INPUT_FRL4L12G:
            /* 2: channel rctrl_pll value */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL0, 0x2);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL1, 0x2);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL2, 0x2);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_RCTRL_PLL3, 0x2);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL_EN0, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL_EN1, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A7, REG_DA_RCTRL_PLL_EN2, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_RCTRL_PLL_EN3, HI_TRUE);
            break;
        default:
            break;
    }
}

static hi_void hal_damix_set_ictrl(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    switch (type) {
        case HDMIRX_INPUT_14:
        case HDMIRX_INPUT_20:
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A4, REG_DA_ICTRL_PD_EN0, HI_FALSE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A4, REG_DA_ICTRL_PD_EN1, HI_FALSE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A4, REG_DA_ICTRL_PD_EN2, HI_FALSE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A11, REG_DA_ICTRL_PD_EN3, HI_FALSE);
            break;
        case HDMIRX_INPUT_FRL3L3G:
        case HDMIRX_INPUT_FRL3L6G:
        case HDMIRX_INPUT_FRL4L6G:
        case HDMIRX_INPUT_FRL4L8G:
        case HDMIRX_INPUT_FRL4L10G:
        case HDMIRX_INPUT_FRL4L12G:
            break;
        default:
            break;
    }
}

static hi_void hal_damix_set_kvco(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    switch (type) {
        case HDMIRX_INPUT_14:
        case HDMIRX_INPUT_20:
            break;
        case HDMIRX_INPUT_FRL3L3G:
        case HDMIRX_INPUT_FRL3L6G:
        case HDMIRX_INPUT_FRL4L6G:
        case HDMIRX_INPUT_FRL4L8G:
        case HDMIRX_INPUT_FRL4L10G:
        case HDMIRX_INPUT_FRL4L12G:
            hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG29, BIT3_0, 0x2); /* 2: dig sel test clk */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG1, BIT3_0, 0x2); /* 2: dig sel test clk */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG2, BIT3_0, 0x2); /* 2: dig sel test clk */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG3, BIT3_0, 0x2); /* 2: dig sel test clk */
            break;
        default:
            break;
    }
}

static hi_void hal_damix_set_eq_bandwidth(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hdmirx_clk_zone zone;
    hi_u32 bandwidth = 0;
    zone = hal_damix_clk_zone_detect(port, type);

    switch (type) {
        case HDMIRX_INPUT_14:
            if (zone == HDMIRX_CLK_ZONE_TMDS14_150_250) {
                bandwidth = 0x3; /* 3: bandwidth value */
            }else if (zone == HDMIRX_CLK_ZONE_TMDS14_250_340) {
                bandwidth = 0x2; /* 2: bandwidth value */
            }
            break;
        case HDMIRX_INPUT_20:
            bandwidth = 0x6;  /* 6: bandwidth value */
            break;
        case HDMIRX_INPUT_FRL3L3G:
            bandwidth = 0x2;  /* 2: bandwidth value */
            break;
        case HDMIRX_INPUT_FRL3L6G:
        case HDMIRX_INPUT_FRL4L6G:
            bandwidth = 0x0;  /* 0: bandwidth value */
            break;
        case HDMIRX_INPUT_FRL4L8G:
            bandwidth = 0x5; /* 5: bandwidth value */
            break;
        case HDMIRX_INPUT_FRL4L10G:
            bandwidth = 0x4; /* 4: bandwidth value */
            break;
        case HDMIRX_INPUT_FRL4L12G:
            bandwidth = 0x0; /* 0: bandwidth value */
            break;
        default:
            break;
    }
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL0, bandwidth);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL1, bandwidth);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL2, bandwidth);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL3, bandwidth);

    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL_EN0, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL_EN1, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL_EN2, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A10, REG_DA_BW_SEL_EN3, HI_TRUE);
}

hi_void hal_phy_init(hi_drv_hdmirx_port port)
{
    /* 0x80044020: test_pll value */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG29, RG_TEST_PLL_L3_LOW, 0x80044020);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG1, RG_TEST_PLL_L2_LOW, 0x80044020);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG2, RG_TEST_PLL_L1_LOW, 0x80044020);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG3, RG_TEST_PLL_L0_LOW, 0x80044020);

    /* 0x1f: eq coarse symbol detect times */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ1, REG_EQ_COARSE_SYMBOL_DET_TIMES, 0x1f);
    /* 0x1f: hdmi pat detect times */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ7, REG_HDMI_PAT_DET_TIMES1, 0x1f);
    /* 0xf0: counter period times */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_CLK_DETECT1, REG_CLK_STABLE_TIMES, 0xf0);
    /* 0xf0: ref lock timer */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_PLL_LOCK0, REG_REF_LOCK_TIMER, 0xf0);
    /* 0x1: pll lock times */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_PLL_LOCK0, REG_PLL_LOCK_TIMES, 0x1);
}

hi_void hal_phy_set_power(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hi_info_hdmirx("[HDMI]== set power port %d type %d", port, type);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_RT, g_pd_cfg[type][0]);         /* 0: Pd RT */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_PHDAC, g_pd_cfg[type][1]);      /* 1: Pd PHDAC */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_LDO, g_pd_cfg[type][2]);        /* 2: Pd LDO */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_LANE, g_pd_cfg[type][3]);       /* 3: Pd LANE */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_CLK, g_pd_cfg[type][4]);        /* 4: Pd CLK */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_BG, g_pd_cfg[type][5]);         /* 5: Pd BG */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_PLL, g_pd_cfg[type][6]);        /* 6: Pd PLL */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG33, RG_PD_LDO_NODIE, g_pd_cfg[type][7]); /* 7: Pd NODIE */
}

hi_void hal_damix_set_term_mode(hi_drv_hdmirx_port port, hdmirx_term_sel mode)
{
    if (mode == HDMIRX_TERM_SEL_OFF) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_RT, 0xF);
    } else if (mode == HDMIRX_TERM_SEL_HDMI) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_ANALOG8, RG_PD_RT, 0x0); /* F: RT SET */
    }
}

/* reg cfg: cfg_cnt_sw_en->ced_cnt_sw0 */
hi_u32 hal_dphy_get_ced_err_cnt(hi_drv_hdmirx_port port, hi_u8 channel)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_SUCCESS;
}

/* reg cfg: cfg_cnt_sw_clr */
hi_void hal_dphy_clear_ced_erro_cnt(hi_drv_hdmirx_port port);

hi_void hal_damix_set_handle_eq_code_single(hi_drv_hdmirx_port port, hi_u8 u8_ch_sel, hi_u8 eq_code)
{
    switch (u8_ch_sel) {
        case 0: /* 0: ch0 */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN0, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE0, eq_code);
            break;
        case 1: /* 1: ch1 */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN1, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE1, eq_code);
            break;
        case 2: /* 2: ch2 */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN2, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE2, eq_code);
            break;
        case 3: /* 3: ch3 */
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN3, HI_TRUE);
            hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE3, eq_code);
            break;
        default:
            break;
    }
}

hi_void hal_damix_set_handle_eq_code(hi_drv_hdmirx_port port, hi_u8 eq_code)
{
    /* Port0 Handle Eq Enable */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN0, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN1, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN2, HI_TRUE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN3, HI_TRUE);

    /* Port0 Handle Eq Set Value */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE0, eq_code);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE1, eq_code);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE2, eq_code);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE3, eq_code);
}

hi_void hal_damix_clr_handle_eq_code(hi_drv_hdmirx_port port)
{
    /* Port0 Handle Eq Enable */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN0, HI_FALSE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN1, HI_FALSE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN2, HI_FALSE);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE_EN3, HI_FALSE);
    /* Port0 Handle Eq Set Value */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE0, 0x0);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE1, 0x0);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE2, 0x0);
    hdmirx_hal_damix_reg_write_fld_align(port, REG_D2A2, REG_DA_EQ_CODE3, 0x0);
}

hi_void hal_damix_get_eq_result(hi_drv_hdmirx_port port, hi_u32 *eq_result, hi_u32 len)
{
    if (eq_result == HI_NULL) {
        return;
    }

    if (len == 3) { /* 3: 3 chanels */
        eq_result[0] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG5, DA_GC_EQ_CODE_L0); /* 0: lane0 eq code */
        eq_result[1] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG5, DA_GC_EQ_CODE_L1); /* 1: lane1 eq code */
        eq_result[2] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG5, DA_GC_EQ_CODE_L2); /* 2: lane2 eq code */
    } else if (len == 4) { /* 4: 4 chanels */
        eq_result[0] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG5, DA_GC_EQ_CODE_L0);  /* 0: lane0 eq code */
        eq_result[1] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG5, DA_GC_EQ_CODE_L1);  /* 1: lane1 eq code */
        eq_result[2] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG5, DA_GC_EQ_CODE_L2);  /* 2: lane2 eq code */
        eq_result[3] = hdmirx_hal_damix_reg_read_fld_align(port, REG_AREG11, DA_GC_EQ_CODE_L3); /* 3: lane3 eq code */
    } else {
        return;
    }
}

hi_bool hal_damix_get_eq_mode(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_TRUE;
}

hi_void hal_damix_set_handle_def_code_single(hi_drv_hdmirx_port port, hi_u8 u8_ch_sel, hi_u8 dfe_code)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return;
}

hi_void hal_damix_set_handle_def_code(hi_drv_hdmirx_port port, hi_u8 dfe_code)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return;
}

hi_void hal_damix_get_handle_auto_def_result(hi_drv_hdmirx_port port, hi_u32 *dfe_result, hi_u32 len)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return;
}

hi_bool hal_damix_get_def_mode(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_TRUE;
}

hi_void hal_damix_set_auto_def_en(hi_drv_hdmirx_port port, hi_bool auto_dfe_en, hdmirx_input_type type)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return;
}

hi_bool hal_damix_get_fix_eq_result(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hi_bool fix_eq_state;
    hi_u8 lane0_eq_done, lane1_eq_done, lane2_eq_done;

    lane0_eq_done = hdmirx_hal_damix_reg_read_fld_align(port, REG_DAMIX_INTR_STATE, DAMIX_INTR_STAT3);
    lane1_eq_done = hdmirx_hal_damix_reg_read_fld_align(port, REG_DAMIX_INTR_STATE, DAMIX_INTR_STAT4);
    lane2_eq_done = hdmirx_hal_damix_reg_read_fld_align(port, REG_DAMIX_INTR_STATE, DAMIX_INTR_STAT5);
    if ((lane0_eq_done == HI_TRUE) && (lane1_eq_done == HI_TRUE) && (lane2_eq_done == HI_TRUE)) {
        fix_eq_state = HI_TRUE;
    } else {
        fix_eq_state = HI_FALSE;
    }

    return fix_eq_state;
}

hi_bool hal_damix_get_auto_eq_result(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hi_bool result = 0;
    hi_u32 eq_status0;
    hi_u32 eq_status1;
    hi_u32 eq_status2;
    hi_u32 eq_status3;

    eq_status0 = hdmirx_hal_damix_reg_read_fld_align(port, REG_AUTOEQ11, AUTO_EQ_ST_LANE0);
    eq_status1 = hdmirx_hal_damix_reg_read_fld_align(port, REG_AUTOEQ11, AUTO_EQ_ST_LANE1);
    eq_status2 = hdmirx_hal_damix_reg_read_fld_align(port, REG_AUTOEQ11, AUTO_EQ_ST_LANE2);
    eq_status3 = hdmirx_hal_damix_reg_read_fld_align(port, REG_AUTOEQ20, AUTO_EQ_ST_LANE3);
    if ((type == HDMIRX_INPUT_14) || (type == HDMIRX_INPUT_20) ||
        (type == HDMIRX_INPUT_FRL3L3G) || (type == HDMIRX_INPUT_FRL3L6G)) {
            if ((eq_status0 == 0x100) && (eq_status1 == 0x100) && (eq_status2 == 0x100)) { /* 0x100: auto_eq ready */
                result = HI_TRUE;
            } else {
                result = HI_FALSE;
            }
    } else if ((type == HDMIRX_INPUT_FRL4L6G) || (type == HDMIRX_INPUT_FRL4L8G) ||
        (type == HDMIRX_INPUT_FRL4L10G) || (type == HDMIRX_INPUT_FRL4L12G)) {
        if ((eq_status0 == 0x100) && (eq_status1 == 0x100) && /* 0x100: auto_eq ready */
            (eq_status2 == 0x100) && (eq_status3 == 0x100)) { /* 0x100: auto_eq ready */
            result = HI_TRUE;
        } else {
            result = HI_FALSE;
        }
    }
    return result;
}

hdmirx_damix_status hal_damix_get_status(hi_drv_hdmirx_port port,
    hdmirx_input_type input_type, hdmirx_damix_type damix_type)
{
    hdmirx_damix_status status = 0;

    switch (damix_type) {
        case HDMIRX_DAMIX_PLL:
            status = hdmirx_hal_damix_reg_read_fld_align(port, REG_PLL_LOCK1, PLL_LOCK);
            break;
        case HDMIRX_DAMIX_CLK:
            status = hdmirx_hal_damix_reg_read_fld_align(port, REG_CLK_DETECT0, HDMI_CLK_STABLE);
            break;
        case HDMIRX_DAMIX_AUTO_EQ:
            status = hal_damix_get_auto_eq_result(port, input_type);
            break;
        case HDMIRX_DAMIX_FIX_EQ:
            status = hal_damix_get_fix_eq_result(port, input_type);
            break;
        case HDMIRX_DAMIX_DFE:
            break;
        default:
            break;
        }
    return status;
}

hi_u32 hal_phy_get_cfg(hi_drv_hdmirx_port port, hdmirx_damix_cfg cfgtype, int *cfg, int maxlen)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_SUCCESS;
}

hi_void hal_phy_set_cfg(hi_drv_hdmirx_port port, hdmirx_damix_cfg cfgtype, int *cfg, int maxlen)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
}

hi_s32 hal_dphy_get_status(hi_drv_hdmirx_port port, hdmirx_dphy_type type)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_SUCCESS;
}

hi_s32 hal_dphy_get_ltp_status(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_SUCCESS;
}

hi_bool hal_dphy_get_inter_align(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_TRUE;
}

hi_void hal_dphy_set_in_fifo_rst(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
}

hi_bool hal_dphy_get_scramber_status(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_TRUE;
}

hi_void hal_damix_pre_set(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hal_cal_amp_en(port, type);
    hal_rterm_ctrl_set(port);
    hal_damix_div_post_set(port, HI_TRUE);
    hal_pd_calibration_set(port);
    hal_damix_div_post_set(port, HI_FALSE);
    hal_pll_ref_clk_set(port, type);
    hal_damix_set_rctrl(port, type);
    hal_damix_set_ictrl(port, type);
    hal_damix_set_kvco(port, type);
}

hi_bool hal_damix_need_autoeq_mode(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hdmirx_clk_zone tmds_clk;
    hi_bool result = HI_FALSE;
    /* get tmds clk for HDMI1.4 Eq mode */
    tmds_clk = hal_damix_clk_zone_detect(port, type);

    /* if tmds_clk >= 1.5G, Auto Eq, else case use handle Eq */
    switch (type) {
        case HDMIRX_INPUT_20:
        case HDMIRX_INPUT_FRL3L3G:
        case HDMIRX_INPUT_FRL3L6G:
        case HDMIRX_INPUT_FRL4L6G:
        case HDMIRX_INPUT_FRL4L8G:
        case HDMIRX_INPUT_FRL4L10G:
        case HDMIRX_INPUT_FRL4L12G:
             result = HI_TRUE;
            break;
        case HDMIRX_INPUT_14:
            if ((tmds_clk == HDMIRX_CLK_ZONE_TMDS14_150_250) ||
                (tmds_clk == HDMIRX_CLK_ZONE_TMDS14_250_340)) {
                result = HI_TRUE;
            } else {
                result = HI_FALSE;
            }
            break;
        default:
            break;
    }
    return result;
}

hi_void hal_damix_fix_eq_init(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hdmirx_clk_zone clk_zone;
    const hi_u8 len_max = 3; /* 3: max 3 zone to sel */

    clk_zone = hal_damix_clk_zone_detect(port, type);
    if (clk_zone >= len_max) {
        return;
    }

    hal_damix_clr_handle_eq_code(port);
    hal_damix_adaptive_eq_mode(port, HI_TRUE);

    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_EQ_SEL, FIX_EQ_CNT);
    /* 8 eq for different clk zone */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG0, REG_EQ_FIXED0,
                                         g_fix_code[clk_zone][0]); /* 0: fixeq 0 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG0, REG_EQ_FIXED1,
                                         g_fix_code[clk_zone][1]); /* 1: fixeq 1 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG0, REG_EQ_FIXED2,
                                         g_fix_code[clk_zone][2]); /* 2: fixeq 2 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG0, REG_EQ_FIXED3,
                                         g_fix_code[clk_zone][3]); /* 3: fixeq 3 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG0, REG_EQ_FIXED4,
                                         g_fix_code[clk_zone][4]); /* 4: fixeq 4 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_EQ_FIXED5,
                                         g_fix_code[clk_zone][5]); /* 5: fixeq 5 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_EQ_FIXED6,
                                         g_fix_code[clk_zone][6]); /* 6: fixeq 6 */
    hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_EQ_FIXED7,
                                         g_fix_code[clk_zone][7]); /* 7: fixeq 7 */

    return;
}

hi_void hal_damix_set_fix_eq_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hi_bool en)
{
    if (en == HI_TRUE) {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_FIXED_SCAN_EQ_EN, HI_FALSE);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_FIXED_SCAN_EQ_EN, HI_TRUE);
    } else {
        hdmirx_hal_damix_reg_write_fld_align(port, REG_FIXED_EQ_CFG1, REG_FIXED_SCAN_EQ_EN, HI_FALSE);
    }
}

hi_void hal_damix_auto_eq_init(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    if (type == HDMIRX_INPUT_14) {
        hal_damix_clr_handle_eq_code(port);
        hal_damix_set_fix_eq_en(port, type, HI_FALSE);

        hal_damix_sweep_set(port);
        hal_damix_set_eq_bandwidth(port, type);
        hal_damix_adaptive_eq_mode(port, HI_TRUE);

        /* 0x3f80,0x2,0xf: auto eq error detect cfg */
        hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ10, REG_EQ_ERROR_CHECK_DURATION, 0x3f80);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ10, REG_EQ_ERROR_STEP, 0x2);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, REG_PHASE_STABLE_THRESHOLD, 0xf);
    } else if (type == HDMIRX_INPUT_20) {
        hal_damix_clr_handle_eq_code(port);
        hal_damix_set_fix_eq_en(port, type, HI_FALSE);

        hal_damix_sweep_clr(port);
        hal_damix_set_eq_bandwidth(port, type);

        /* 0x3f80,0x2,0xf: auto eq error detect cfg */
        hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ10, REG_EQ_ERROR_CHECK_DURATION, 0x3f80);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ10, REG_EQ_ERROR_STEP, 0x2);
        hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, REG_PHASE_STABLE_THRESHOLD, 0xf);
        hal_damix_adaptive_eq_mode(port, HI_FALSE);
    } else if ((type == HDMIRX_INPUT_FRL3L3G) || (type == HDMIRX_INPUT_FRL3L6G) ||
               (type == HDMIRX_INPUT_FRL4L6G) || (type == HDMIRX_INPUT_FRL4L8G) ||
               (type == HDMIRX_INPUT_FRL4L6G) || (type == HDMIRX_INPUT_FRL4L8G)) {
        hal_damix_adaptive_eq_mode(port, HI_TRUE);
        hal_damix_set_eq_bandwidth(port, type);
    }
}

hi_void hal_damix_set_force_eq(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    hdmirx_clk_zone zone;
    zone = hal_damix_clk_zone_detect(port, type);

    switch (type) {
        case HDMIRX_INPUT_14:
            if ((zone == HDMIRX_CLK_ZONE_TMDS14_25_42P5) || (zone == HDMIRX_CLK_ZONE_TMDS14_42P5_75) ||
                (zone == HDMIRX_CLK_ZONE_TMDS14_75_150)) {
                hal_damix_set_handle_eq_code(port, 0x8); /* 0x8: HDMI14 low eq code */
            } else if ((zone == HDMIRX_CLK_ZONE_TMDS14_150_250) ||
                       (zone == HDMIRX_CLK_ZONE_TMDS14_250_340)) {
                hal_damix_set_handle_eq_code(port, 0xf); /* 0xF: HDMI1.4 high eq code */
            }
            break;
        case HDMIRX_INPUT_20:
            hal_damix_set_handle_eq_code(port, 0x12); /* 0x8: HDMI20 low eq code */
            break;
        case HDMIRX_INPUT_FRL3L3G:
            hal_damix_set_handle_eq_code(port, 0x12); /* 0x12: FRL 3G eq code */
            break;
        case HDMIRX_INPUT_FRL3L6G:
            hal_damix_set_handle_eq_code(port, 0xf); /* 0xF: FRL 3L6G eq code */
            break;
        case HDMIRX_INPUT_FRL4L6G:
            hal_damix_set_handle_eq_code(port, 0xf); /* 0xf: FRL 4L6G low eq code */
            break;
        case HDMIRX_INPUT_FRL4L8G:
            hal_damix_set_handle_eq_code(port, 0x16); /* 0x8: FRL 8G low eq code */
            break;
        case HDMIRX_INPUT_FRL4L10G:
            hal_damix_set_handle_eq_code(port, 0x18); /* 0x8: FRL 10G  eq code */
            break;
        case HDMIRX_INPUT_FRL4L12G:
            hal_damix_set_handle_eq_code(port, 0x1a); /* 0x8: FRL 12G low eq code */
            break;
        default:
            break;
    }
}

hi_void hal_damix_set_auto_eq_en(hi_drv_hdmirx_port port, hdmirx_input_type type, hi_bool en)
{
    if (en == HI_TRUE) {
        switch (type) {
            case HDMIRX_INPUT_FRL3L3G:
            case HDMIRX_INPUT_FRL3L6G:
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE0, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE1, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE2, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE3, HI_FALSE);

                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE0, HI_TRUE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE1, HI_TRUE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE2, HI_TRUE);
            break;
            case HDMIRX_INPUT_FRL4L6G:
            case HDMIRX_INPUT_FRL4L8G:
            case HDMIRX_INPUT_FRL4L10G:
            case HDMIRX_INPUT_FRL4L12G:
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE0, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE1, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE2, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE3, HI_FALSE);

                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE0, HI_TRUE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE1, HI_TRUE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE2, HI_TRUE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, CFG_FRL_AUTO_EQ_EN_LANE3, HI_TRUE);
                break;
            case HDMIRX_INPUT_14:
            case HDMIRX_INPUT_20:
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, REG_MHL_AUTO_EQ_EN, HI_FALSE);
                hdmirx_hal_damix_reg_write_fld_align(port, REG_AUTOEQ0, REG_MHL_AUTO_EQ_EN, HI_TRUE);
                break;
            default:
                break;
        }
    } else {
    }
}

hi_bool hal_damix_need_dfe(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
    return HI_TRUE;
}

hi_void hal_damix_dfe_init(hi_drv_hdmirx_port port, hdmirx_input_type type)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
}

hi_void hal_damix_set_dfe_mode_en(hi_drv_hdmirx_port port, hdmirx_dfe_mode mode, hi_bool en)
{
    /* this vertion is not nessasery now, will be coding in next vertions */
}

hi_bool hal_damix_get_dfe_result(hi_drv_hdmirx_port port)
{
    /* this vertion is not nessasery now , will be coding in next vertions */
    return HI_TRUE;
}

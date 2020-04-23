/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal combo phy module
* Author: sdk
* Create: 2019-04-03
*/

#include "hi_type.h"
#include "drv_panel_define.h"
#include "hal_dphy_reg.h"
#include "hal_panel_combophy.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PREEMPHASIS_MAX_VALUE 31

#define CONTROL_SIGNAL_P2P_4MA 0x410410

#define CONTROL_SIGNAL_P2P_100OHM 0x924

#define CONFIG_FALSE 0x0

#define CONFIG_28BITS_TRUE 0xFFFFFFF
#define MAX_28BITS_VALUE   0xFFFFFFF

#define COMBO_PHY_PORT_MUX1 0
#define COMBO_PHY_PORT_MUX2 1
#define COMBO_PHY_PORT_MUX3 2
#define COMBO_PHY_PORT_MUX4 3
#define COMBO_PHY_PORT_MUX_MAX 4

/* 200mv   250mv   300mv   350mv  400mv   450mv  500mv */
static hi_u8 g_lvds_or_mlvds_swing[7] = { 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa };  /* 7: default parm */
static hi_u8 g_p2p_or_vbo_swing[7] = { 0x20, 0x18, 0x1c, 0x2c, 0x34, 0x38, 0x3c };  /* 7: default parm */
/* 400mv 450mv   500mv    550mv   600mv  650mv 700mv 800mv 900mv
   1000mv 1100mv 1150mv 1200mv 1250mv 1300mv  1400mv 1500mv */
static hi_u8 g_com_voltage_reg[17] = {  /* 17: default parm */
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
    0xa, 0xb, 0xc, 0xd, 0xe, 0xe, 0xe
};
static hi_u8 g_low_power_mode_pre_emphasis[7][10] = { /* 7, 10: default parm */
    { 0x0, 0x1, 0x2, 0x3, 0x5, 0x6,  0x8,  0xa,  0xc,  0xf },
    { 0x0, 0x1, 0x3, 0x4, 0x6, 0x8,  0xa,  0xc,  0x10, 0x12 },
    { 0x0, 0x2, 0x3, 0x5, 0x7, 0x9,  0xc,  0xc,  0xc,  0xc },
    { 0x0, 0x2, 0x4, 0x6, 0x8, 0xb,  0xe,  0xe,  0xe,  0xe },
    { 0x0, 0x2, 0x4, 0x7, 0x9, 0xd,  0x10, 0x10, 0x10, 0x10 },
    { 0x0, 0x2, 0x5, 0x8, 0xb, 0xe,  0x12, 0x12, 0x12, 0x12 },
    { 0x0, 0x3, 0x6, 0x8, 0xc, 0x10, 0x12, 0x12, 0x12, 0x12 }
};
static hi_u8 g_p2p_vbo_mode_pre_emphasis[7][10] = {  /* 7, 10: default parm */
    { 0x0, 0x4, 0x8,  0xe,  0x14, 0x1a, 0x1f, 0x1f, 0x1f, 0x1f },
    { 0x0, 0x5, 0xa,  0x10, 0x18, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
    { 0x0, 0x6, 0xc,  0x18, 0x1c, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
    { 0x0, 0x7, 0xe,  0x18, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
    { 0x0, 0x8, 0x10, 0x1b, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
    { 0x0, 0x8, 0x14, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
    { 0x0, 0xc, 0x14, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f }
};

static hi_u32 g_combo_phy_port_sort[HAL_PANEL_SORTTYPE_BUTT][COMBO_PHY_PORT_MUX_MAX] = {
    { 0x0, 0x00020100, 0x04030500, 0x00060007 },
    { 0x04000302, 0x0e060501, 0x0807090f, 0x0c0a0d0b },
    { 0x04000302, 0x06060501, 0x00070107, 0x04020503 },
    { 0x04000a0a, 0x06020501, 0x0a0a0703, 0x00060007 },
    { 0x01000a0a, 0x05040302, 0x0a0a0706, 0x00060007 },
};

hi_void hal_panel_set_combo_dphy_reset(hi_bool phy_srst)
{
    u_combo_reset combo_reset;
    combo_reset.u32 = panel_reg_read(&(panel_dphy_reg()->combo_reset.u32));
    combo_reset.bits.dphy_srst_req = phy_srst;
    combo_reset.bits.pll_srst_req = CONFIG_FALSE;
    panel_reg_write(&(panel_dphy_reg()->combo_reset.u32), combo_reset.u32);
}

hi_void hal_panel_set_aphy_intf_power_mode(hal_panel_aphy_power_mode aphy_mode)
{
    return;
}

hi_void hal_panel_set_combo_aphy_clk_mode(hal_panel_intftype intf_type)
{
    u_aphy_ctrl6 aphy_ctrl6;

    aphy_ctrl6.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl6.u32));
    switch (intf_type) {
        case HAL_PANEL_INTFTYPE_LVDS:
        case HAL_PANEL_INTFTYPE_MINILVDS:
            aphy_ctrl6.bits.cfg2phy_sel_clk = 0x1;
            break;
        case HAL_PANEL_INTFTYPE_EPI:
        case HAL_PANEL_INTFTYPE_ISP:
        case HAL_PANEL_INTFTYPE_CEDS:
        case HAL_PANEL_INTFTYPE_VBONE:
        case HAL_PANEL_INTFTYPE_CHPI:
            aphy_ctrl6.bits.cfg2phy_sel_clk = 0x0;
            aphy_ctrl6.bits.cfg2phy_sel_ldo = 0x3; /* 0x3 is default value */
            break;
        default:
            break;
    }
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl6.u32), aphy_ctrl6.u32);
}

hi_void hal_panel_set_combo_aphy_over_sample(hal_panel_phy_over_sample over_sample)
{
    u_aphy_ctrl6 aphy_ctrl6;

    aphy_ctrl6.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl6.u32));
    aphy_ctrl6.bits.cfg2phy_mux_divsel = (hi_u8)over_sample;
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl6.u32), aphy_ctrl6.u32);
}

hi_void hal_panel_set_combo_phy_atop(hal_panel_intftype intf_type)
{
    return;
}

hi_void hal_panel_set_combo_phy_txpll_div_fb(hal_panel_phy_div_feedback aphy_div_fb)
{
    u_aphy_ctrl9 aphy_ctrl9;

    aphy_ctrl9.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl9.u32));
    aphy_ctrl9.bits.cfg2phy_txpll_divsel_fb = aphy_div_fb;
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl9.u32), aphy_ctrl9.u32);
}

hi_void hal_panel_set_combo_phy_txpll_div_in(hal_panel_phy_div_in aphy_div_in)
{
    u_aphy_ctrl9 aphy_ctrl9;

    aphy_ctrl9.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl9.u32));
    aphy_ctrl9.bits.cfg2phy_txpll_divsel_in = aphy_div_in;
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl9.u32), aphy_ctrl9.u32);
}

hi_void hal_panel_set_combo_phy_txpll_icp_current(hal_panel_phy_icp_current aphy_icp_current)
{
    u_aphy_ctrl9 aphy_ctrl9;

    aphy_ctrl9.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl9.u32));
    aphy_ctrl9.bits.cfg2phy_txpll_icp_ictrl = aphy_icp_current;
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl9.u32), aphy_ctrl9.u32);
}

hi_void panel_select_low_power_mode(hi_u8 *result_value_p, const hi_u32 value_num, const hi_u8 *value1_p,
                                    const hi_u8 *value2_p, hal_panel_combophy_eyesatt eye_attr)
{
    hi_u32 i = 0;
    if (eye_attr == HAL_PANEL_COMBOPHY_EYESATT_SWING) {
        for (i = 0; i < value_num; i++) {
            result_value_p[i] = g_lvds_or_mlvds_swing[value1_p[i]];
        }
    } else {
        for (i = 0; i < value_num; i++) {
            result_value_p[i] = g_low_power_mode_pre_emphasis[value1_p[i]][value2_p[i]];
        }
    }
}
hi_void panel_select_high_power_mode(hi_u8 *result_value_p, const hi_u32 value_num, const hi_u8 *value1_p,
                                     const hi_u8 *value2_p, hal_panel_combophy_eyesatt eye_attr)
{
    hi_u32 i = 0;
    if (eye_attr == HAL_PANEL_COMBOPHY_EYESATT_SWING) {
        for (i = 0; i < value_num; i++) {
            result_value_p[i] = g_p2p_or_vbo_swing[value1_p[i]];
        }
    } else {
        for (i = 0; i < value_num; i++) {
            result_value_p[i] = g_p2p_vbo_mode_pre_emphasis[value1_p[i]][value2_p[i]];
        }
    }
}
hi_void hal_panel_set_combo_phy_current(hal_panel_intftype intf_type, const hi_u8 *drv_current_p)
{
    u_aphy_ctrl11 aphy_ctrl11;
    u_aphy_ctrl12 aphy_ctrl12;
    u_aphy_ctrl13 aphy_ctrl13;
    u_aphy_ctrl14 aphy_ctrl14;

    hi_u32 swing_lane15to0_value;
    hi_u32 swing_lane31to16_value;
    hi_u32 swing_lane47to32_value;
    hi_u32 swing_lane63to48_value;

    hi_u8 swing_value[LANE_NUM] = { 0 };

    if (intf_type == HAL_PANEL_INTFTYPE_LVDS || intf_type == HAL_PANEL_INTFTYPE_MINILVDS) {
        panel_select_low_power_mode(swing_value, LANE_NUM,
            drv_current_p, drv_current_p, HAL_PANEL_COMBOPHY_EYESATT_SWING);
    } else {
        panel_select_high_power_mode(swing_value, LANE_NUM,
            drv_current_p, drv_current_p, HAL_PANEL_COMBOPHY_EYESATT_SWING);
    }

    swing_lane15to0_value = (swing_value[3] << 18) |     /* 3,18: default parm */
                           (swing_value[2] << 12) |     /* 2,12: default parm */
                           (swing_value[1] << 6) |      /* 1,6: default parm */
                           (swing_value[0]);            /* 0: default parm */
    swing_lane31to16_value = (swing_value[7] << 18) |     /* 7,18: default parm */
                           (swing_value[6] << 12) |     /* 6,12: default parm */
                           (swing_value[5] << 6) |      /* 5,6: default parm */
                           (swing_value[4]);            /* 4: default parm */
    swing_lane47to32_value = (swing_value[11] << 18) |   /* 11,18: default parm */
                            (swing_value[10] << 12) |   /* 10, 12: default parm */
                            (swing_value[9] << 6) |     /* 9,6: default parm */
                            (swing_value[8]);           /* 8: default parm */
    swing_lane63to48_value = (swing_value[15] << 18) |  /* 15,18: default parm */
                             (swing_value[14] << 12) |  /* 14,12: default parm */
                             (swing_value[13] << 6) |   /* 13,6: default parm */
                             (swing_value[12]);         /* 12: default parm */

    /* 1.firstly, to config the value of swing */
    aphy_ctrl11.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl11.u32));
    aphy_ctrl12.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl12.u32));
    aphy_ctrl13.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl13.u32));
    aphy_ctrl14.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl14.u32));

    aphy_ctrl11.bits.cfg2phy_isel_main_d15to0 = swing_lane15to0_value;
    aphy_ctrl12.bits.cfg2phy_isel_main_d31to16 = swing_lane31to16_value;
    aphy_ctrl13.bits.cfg2phy_isel_main_d47to32 = swing_lane47to32_value;
    aphy_ctrl14.bits.cfg2phy_isel_main_d63to48 = swing_lane63to48_value;

    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl11.u32), aphy_ctrl11.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl12.u32), aphy_ctrl12.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl13.u32), aphy_ctrl13.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl14.u32), aphy_ctrl14.u32);
}

hi_void hal_panel_set_combo_phy_pre_emphasis(hal_panel_intftype intf_type, const hi_u8 *drv_current_p,
                                             const hi_u8 *pre_emphasis_p)
{
    u_aphy_ctrl15 aphy_ctrl15;
    u_aphy_ctrl16 aphy_ctrl16;
    u_aphy_ctrl17 aphy_ctrl17;
    u_aphy_ctrl18 aphy_ctrl18;

    hi_u32 emp_lane15to0_value;
    hi_u32 emp_lane31to16_value;
    hi_u32 emp_lane47to32_value;
    hi_u32 emp_lane63to48_value;

    hi_u8 emp_value[LANE_NUM] = { 0 };

    if (intf_type == HAL_PANEL_INTFTYPE_LVDS || intf_type == HAL_PANEL_INTFTYPE_MINILVDS) {
        panel_select_low_power_mode(emp_value, LANE_NUM,
                                    drv_current_p,
                                    pre_emphasis_p,
                                    HAL_PANEL_COMBOPHY_EYESATT_EMPHASIS);
    } else {
        panel_select_high_power_mode(emp_value, LANE_NUM,
                                     drv_current_p,
                                     pre_emphasis_p,
                                     HAL_PANEL_COMBOPHY_EYESATT_EMPHASIS);
    }

    emp_lane15to0_value = (emp_value[3] << 18) | (emp_value[2] << 12) |  /* 3, 2, 12, 18: default parm */
                          (emp_value[1] << 6) | (emp_value[0]);          /* 1, 0, 6: default parm */
    emp_lane31to16_value = (emp_value[7] << 18) | (emp_value[6] << 12) |  /* 7, 6, 12, 18: default parm */
                          (emp_value[5] << 6) | (emp_value[4]);          /* 5, 4, 6: default parm */
    emp_lane47to32_value = (emp_value[11] << 18) | (emp_value[10] << 12) | /* 11, 10, 12, 18: default parm */
                           (emp_value[9] << 6) | (emp_value[8]);           /* 9, 8, 6: default parm */
    emp_lane63to48_value = (emp_value[15] << 18) | (emp_value[14] << 12) | /* 15, 14, 12, 18: default parm */
                            (emp_value[13] << 6) | (emp_value[12]);       /* 13, 12, 6: default parm */

    aphy_ctrl15.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl15.u32));
    aphy_ctrl16.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl16.u32));
    aphy_ctrl17.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl17.u32));
    aphy_ctrl18.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl18.u32));

    aphy_ctrl15.bits.cfg2phy_isel_main_de_d15to0 = emp_lane15to0_value;
    aphy_ctrl16.bits.cfg2phy_isel_main_de_d31to16 = emp_lane31to16_value;
    aphy_ctrl17.bits.cfg2phy_isel_main_de_d47to32 = emp_lane47to32_value;
    aphy_ctrl18.bits.cfg2phy_isel_main_de_d63to48 = emp_lane63to48_value;

    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl15.u32), aphy_ctrl15.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl16.u32), aphy_ctrl16.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl17.u32), aphy_ctrl17.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl18.u32), aphy_ctrl18.u32);
}

/* the range of comvoltage set by MW is 0~10 */
hi_void panel_set_com_voltage(hi_u8 *com_voltage_p, hi_u32 length)
{
    u_aphy_ctrl32 aphy_ctrl32;
    u_aphy_ctrl33 aphy_ctrl33;

    hi_u32 com_voltage_lane15to0_value;
    hi_u32 com_voltage_lane31to16_value;
    hi_u32 com_voltage_lane47to32_value;
    hi_u32 com_voltage_lane63to48_value;

    hi_u8 au8_com_voltage_value[LANE_NUM] = { 0 };
    hi_u32 i = 0;

    if (length > LANE_NUM) {
        hi_log_err("com_voltage length is out of range!");
        return;
    }
    for (i = 0; i < length; i++) {
        au8_com_voltage_value[i] = g_com_voltage_reg[com_voltage_p[i]];
    }

    com_voltage_lane15to0_value = (au8_com_voltage_value[3] << 12) |    /* 3,12: default */
                                 (au8_com_voltage_value[2] << 8) |     /* 2,8: default */
                                 (au8_com_voltage_value[1] << 4) |     /* 1,4: default */
                                 (au8_com_voltage_value[0]);           /* 0: default */
    com_voltage_lane31to16_value = (au8_com_voltage_value[7] << 12) |    /* 7,12: default */
                                 (au8_com_voltage_value[6] << 8) |     /* 6,8: default */
                                 (au8_com_voltage_value[5] << 4) |     /* 5,4: default */
                                 (au8_com_voltage_value[4]);           /* 4: default */
    com_voltage_lane47to32_value = (au8_com_voltage_value[11] << 12) |  /* 11,12: default */
                                  (au8_com_voltage_value[10] << 8) |   /* 8,10: default */
                                  (au8_com_voltage_value[9] << 4) |    /* 9,4: default */
                                  (au8_com_voltage_value[8]);          /* 8: default */
    com_voltage_lane63to48_value = (au8_com_voltage_value[15] << 12) | /* 12,15: default */
                                   (au8_com_voltage_value[14] << 8) |  /* 8,14: default */
                                   (au8_com_voltage_value[13] << 4) |  /* 13,4: default */
                                   (au8_com_voltage_value[12]);        /* 12: default */

    aphy_ctrl32.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl32.u32));
    aphy_ctrl33.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl33.u32));

    aphy_ctrl32.bits.cfg2phy_vcm_sel_d15to0 = com_voltage_lane15to0_value;
    aphy_ctrl32.bits.cfg2phy_vcm_sel_d31to16 = com_voltage_lane31to16_value;
    aphy_ctrl33.bits.cfg2phy_vcm_sel_d47to32 = com_voltage_lane47to32_value;
    aphy_ctrl33.bits.cfg2phy_vcm_sel_d63to48 = com_voltage_lane63to48_value;

    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl32.u32), aphy_ctrl32.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl33.u32), aphy_ctrl33.u32);
}

hi_void hal_panel_set_current_control_signal(hal_panel_phy_predriver phy_pre_driver)
{
    u_aphy_ctrl19 aphy_ctrl19;
    u_aphy_ctrl20 aphy_ctrl20;
    u_aphy_ctrl21 aphy_ctrl21;
    u_aphy_ctrl22 aphy_ctrl22;

    /* predriver resistance */
    u_aphy_ctrl27 aphy_ctrl27;
    u_aphy_ctrl28 aphy_ctrl28;

    aphy_ctrl19.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl19.u32));
    aphy_ctrl20.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl20.u32));
    aphy_ctrl21.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl21.u32));
    aphy_ctrl22.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl22.u32));

    aphy_ctrl27.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl27.u32));
    aphy_ctrl28.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl28.u32));

    aphy_ctrl19.bits.cfg2phy_isel_pre_d15to0 = phy_pre_driver;
    aphy_ctrl20.bits.cfg2phy_isel_pre_d31to16 = phy_pre_driver;
    aphy_ctrl21.bits.cfg2phy_isel_pre_d47to32 = phy_pre_driver;
    aphy_ctrl22.bits.cfg2phy_isel_pre_d63to48 = phy_pre_driver;

    aphy_ctrl27.bits.cfg2phy_rsel_pre_d15to0 = 0xAEB;  /* 0xAEB:HAL_PANEL_PHY_PREDRIVER_RESISTANCE_125OHM; */
    aphy_ctrl27.bits.cfg2phy_rsel_pre_d31to16 = HAL_PANEL_PHY_PREDRIVER_RESISTANCE_125OHM;
    aphy_ctrl28.bits.cfg2phy_rsel_pre_d47to32 = HAL_PANEL_PHY_PREDRIVER_RESISTANCE_125OHM;
    aphy_ctrl28.bits.cfg2phy_rsel_pre_d63to48 = HAL_PANEL_PHY_PREDRIVER_RESISTANCE_125OHM;

    /* predriver */
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl19.u32), aphy_ctrl19.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl20.u32), aphy_ctrl20.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl21.u32), aphy_ctrl21.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl22.u32), aphy_ctrl22.u32);

    /* predriver resistance */
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl27.u32), aphy_ctrl27.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl28.u32), aphy_ctrl28.u32);
}

hi_void hal_panel_set_pre_emp_control_signal(hal_panel_phy_preemp_predriver pre_emp_driver)
{
    /* pre_empdriver */
    u_aphy_ctrl23 aphy_ctrl23;
    u_aphy_ctrl24 aphy_ctrl24;
    u_aphy_ctrl25 aphy_ctrl25;
    u_aphy_ctrl26 aphy_ctrl26;

    /* pre_empdriver resistance */
    u_aphy_ctrl29 aphy_ctrl29;
    u_aphy_ctrl30 aphy_ctrl30;

    /* pre_empdriver */
    aphy_ctrl23.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl23.u32));
    aphy_ctrl24.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl24.u32));
    aphy_ctrl25.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl25.u32));
    aphy_ctrl26.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl26.u32));

    /* pre_empdriver resistance */
    aphy_ctrl29.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl29.u32));
    aphy_ctrl30.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_ctrl30.u32));

    aphy_ctrl23.bits.cfg2phy_isel_pre_de_d15to0 = pre_emp_driver;
    aphy_ctrl24.bits.cfg2phy_isel_pre_de_d31to16 = pre_emp_driver;
    aphy_ctrl25.bits.cfg2phy_isel_pre_de_d47to32 = pre_emp_driver;
    aphy_ctrl26.bits.cfg2phy_isel_pre_de_d63to48 = pre_emp_driver;

    aphy_ctrl29.bits.cfg2phy_rsel_pre_de_d15to0 = 0xAEB; /* 0xAEB:HAL_PANEL_PHY_PREEMP_RESISTANCE_250OHM; */
    aphy_ctrl29.bits.cfg2phy_rsel_pre_de_d31to16 = HAL_PANEL_PHY_PREEMP_RESISTANCE_250OHM;
    aphy_ctrl30.bits.cfg2phy_rsel_pre_de_d47to32 = HAL_PANEL_PHY_PREEMP_RESISTANCE_250OHM;
    aphy_ctrl30.bits.cfg2phy_rsel_pre_de_d63to48 = HAL_PANEL_PHY_PREEMP_RESISTANCE_250OHM;

    /* pre_empdriver */
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl23.u32), aphy_ctrl23.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl24.u32), aphy_ctrl24.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl25.u32), aphy_ctrl25.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl26.u32), aphy_ctrl26.u32);

    /* pre_empdriver resistance */
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl29.u32), aphy_ctrl29.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_ctrl30.u32), aphy_ctrl30.u32);
}

/* the mask is to control the enable of combo_dphy, have no data when the mask is high voltage */
hi_void panel_set_dphy_port_mask(hi_u32 port, hi_bool port_mask)
{
    u_port_mask1 port_mask1;
    u_port_mask2 port_mask2;

    port_mask1.u32 = panel_reg_read(&(panel_dphy_reg()->port_mask1.u32));
    port_mask2.u32 = panel_reg_read(&(panel_dphy_reg()->port_mask2.u32));

    if (port_mask) {
        port_mask1.u32 |= (0x1 << port);
        port_mask2.u32 |= (0x1 << port);
    } else {
        port_mask1.u32 &= ~(0x1 << port);
        port_mask2.u32 &= ~(0x1 << port);
    }
    panel_reg_write(&(panel_dphy_reg()->port_mask1.u32), port_mask1.u32);
    panel_reg_write(&(panel_dphy_reg()->port_mask2.u32), port_mask2.u32);
}

hi_void hal_panel_set_dphy_port_pn_swap(hi_u32 port_pn_swap)
{
    u_port_chinv1 port_chinv1;

    port_chinv1.u32 = panel_reg_read(&(panel_dphy_reg()->port_chinv1.u32));

    port_chinv1.bits.cfg2phy_port0_chinv = (port_pn_swap) & (0x1);
    port_chinv1.bits.cfg2phy_port1_chinv = (port_pn_swap >> 1) & (0x1); /* 1: default parm */
    port_chinv1.bits.cfg2phy_port2_chinv = (port_pn_swap >> 2) & (0x1); /* 2: default parm */
    port_chinv1.bits.cfg2phy_port3_chinv = (port_pn_swap >> 3) & (0x1); /* 3: default parm */
    port_chinv1.bits.cfg2phy_port4_chinv = (port_pn_swap >> 4) & (0x1); /* 4: default parm */
    port_chinv1.bits.cfg2phy_port5_chinv = (port_pn_swap >> 5) & (0x1); /* 5: default parm */
    port_chinv1.bits.cfg2phy_port6_chinv = (port_pn_swap >> 6) & (0x1); /* 6: default parm */
    port_chinv1.bits.cfg2phy_port7_chinv = (port_pn_swap >> 7) & (0x1); /* 7: default parm */
    port_chinv1.bits.cfg2phy_port8_chinv = (port_pn_swap >> 8) & (0x1); /* 8: default parm */
    port_chinv1.bits.cfg2phy_port9_chinv = (port_pn_swap >> 9) & (0x1); /* 9: default parm */
    port_chinv1.bits.cfg2phy_port10_chinv = (port_pn_swap >> 10) & (0x1); /* 10: default parm */
    port_chinv1.bits.cfg2phy_port11_chinv = (port_pn_swap >> 11) & (0x1); /* 11: default parm */
    port_chinv1.bits.cfg2phy_port12_chinv = (port_pn_swap >> 12) & (0x1); /* 12: default parm */
    port_chinv1.bits.cfg2phy_port13_chinv = (port_pn_swap >> 13) & (0x1); /* 13: default parm */
    port_chinv1.bits.cfg2phy_port14_chinv = (port_pn_swap >> 14) & (0x1); /* 14: default parm */
    port_chinv1.bits.cfg2phy_port15_chinv = (port_pn_swap >> 15) & (0x1); /* 15: default parm */
    port_chinv1.bits.cfg2phy_port16_chinv = (port_pn_swap >> 16) & (0x1); /* 16: default parm */
    port_chinv1.bits.cfg2phy_port17_chinv = (port_pn_swap >> 17) & (0x1); /* 17: default parm */
    port_chinv1.bits.cfg2phy_port18_chinv = (port_pn_swap >> 18) & (0x1); /* 18: default parm */
    port_chinv1.bits.cfg2phy_port19_chinv = (port_pn_swap >> 19) & (0x1); /* 19: default parm */
    port_chinv1.bits.cfg2phy_port20_chinv = (port_pn_swap >> 20) & (0x1); /* 20: default parm */
    port_chinv1.bits.cfg2phy_port21_chinv = (port_pn_swap >> 21) & (0x1); /* 21: default parm */
    port_chinv1.bits.cfg2phy_port22_chinv = (port_pn_swap >> 22) & (0x1); /* 22: default parm */
    port_chinv1.bits.cfg2phy_port23_chinv = (port_pn_swap >> 23) & (0x1); /* 23: default parm */
    port_chinv1.bits.cfg2phy_port24_chinv = (port_pn_swap >> 24) & (0x1); /* 24: default parm */
    port_chinv1.bits.cfg2phy_port25_chinv = (port_pn_swap >> 25) & (0x1); /* 25: default parm */
    port_chinv1.bits.cfg2phy_port26_chinv = (port_pn_swap >> 26) & (0x1); /* 26: default parm */
    port_chinv1.bits.cfg2phy_port27_chinv = (port_pn_swap >> 27) & (0x1); /* 27: default parm */
    port_chinv1.bits.cfg2phy_port28_chinv = (port_pn_swap >> 28) & (0x1); /* 28: default parm */
    port_chinv1.bits.cfg2phy_port29_chinv = (port_pn_swap >> 29) & (0x1); /* 29: default parm */
    port_chinv1.bits.cfg2phy_port30_chinv = (port_pn_swap >> 30) & (0x1); /* 30: default parm */
    port_chinv1.bits.cfg2phy_port31_chinv = (port_pn_swap >> 31) & (0x1); /* 31: default parm */

    panel_reg_write(&(panel_dphy_reg()->port_chinv1.u32), port_chinv1.u32);
    panel_reg_write(&(panel_dphy_reg()->port_chinv2.u32), port_chinv1.u32);
}

hi_void hal_panel_set_dphy_port_sort(hal_panel_sorttype sort_type, hi_u32 first_group_line,
    hi_u32 second_group_line, hi_u32 third_group_line, hi_u32 fourth_group_line)
{
    u_port_mux1 port_mux1;
    u_port_mux2 port_mux2;
    u_port_mux3 port_mux3;
    u_port_mux4 port_mux4;

    port_mux1.u32 = panel_reg_read(&(panel_dphy_reg()->port_mux1.u32));
    port_mux2.u32 = panel_reg_read(&(panel_dphy_reg()->port_mux2.u32));
    port_mux3.u32 = panel_reg_read(&(panel_dphy_reg()->port_mux3.u32));
    port_mux4.u32 = panel_reg_read(&(panel_dphy_reg()->port_mux4.u32));
    if (sort_type >= HAL_PANEL_SORTTYPE_BUTT || sort_type < HAL_PANEL_SORTTYPE_8LINK) {
        hi_log_err("sort type out off range.\n");
        return;
    }

    port_mux1.u32 = g_combo_phy_port_sort[sort_type][COMBO_PHY_PORT_MUX1];
    port_mux2.u32 = g_combo_phy_port_sort[sort_type][COMBO_PHY_PORT_MUX2];
    port_mux3.u32 = g_combo_phy_port_sort[sort_type][COMBO_PHY_PORT_MUX3];
    port_mux4.u32 = g_combo_phy_port_sort[sort_type][COMBO_PHY_PORT_MUX4];

    /* custom_defined mode */
    if (first_group_line != 0) {
        port_mux1.u32 = first_group_line;
    }

    if (second_group_line != 0) {
        port_mux2.u32 = second_group_line;
    }

    if (third_group_line != 0) {
        port_mux3.u32 = third_group_line;
    }

    if (fourth_group_line != 0) {
        port_mux4.u32 = fourth_group_line;
    }

    panel_reg_write(&(panel_dphy_reg()->port_mux1.u32), port_mux1.u32);
    panel_reg_write(&(panel_dphy_reg()->port_mux2.u32), port_mux2.u32);
    panel_reg_write(&(panel_dphy_reg()->port_mux3.u32), port_mux3.u32);
    panel_reg_write(&(panel_dphy_reg()->port_mux4.u32), port_mux4.u32);
}

hi_void hal_panel_set_combo_dphy_over_sample(hal_panel_phy_over_sample over_sample)
{
    u_dphy_ctrl55 dphy_ctrl55;

    dphy_ctrl55.u32 = panel_reg_read(&(panel_dphy_reg()->dphy_ctrl55.u32));
    dphy_ctrl55.bits.cfg2phy_sample_cnt = (hi_u8)over_sample;
    panel_reg_write(&(panel_dphy_reg()->dphy_ctrl55.u32), dphy_ctrl55.u32);
}

hi_void hal_panel_set_combo_dphy_intf_type(hal_panel_intftype intf_type)
{
    u_dphy_ctrl55 dphy_ctrl55;

    dphy_ctrl55.u32 = panel_reg_read(&(panel_dphy_reg()->dphy_ctrl55.u32));
    switch (intf_type) {
        case HAL_PANEL_INTFTYPE_LVDS:
            dphy_ctrl55.bits.cfg2phy_intf_sel = 0x2;
            break;
        case HAL_PANEL_INTFTYPE_VBONE:
            dphy_ctrl55.bits.cfg2phy_intf_sel = 0x1;
            break;
        case HAL_PANEL_INTFTYPE_MINILVDS:
            dphy_ctrl55.bits.cfg2phy_intf_sel = 0x3;
            break;
        case HAL_PANEL_INTFTYPE_EPI:
        case HAL_PANEL_INTFTYPE_ISP:
        case HAL_PANEL_INTFTYPE_CEDS:
        case HAL_PANEL_INTFTYPE_CHPI:
            dphy_ctrl55.bits.cfg2phy_intf_sel = 0x0;
            break;
        default:
            break;
    }
    panel_reg_write(&(panel_dphy_reg()->dphy_ctrl55.u32), dphy_ctrl55.u32);
}

hi_void hal_panel_set_dphy_bit_width(hal_panel_dphy_bitwidth dphy_bit_width)
{
    u_dphy_ctrl55 dphy_ctrl55;

    dphy_ctrl55.u32 = panel_reg_read(&(panel_dphy_reg()->dphy_ctrl55.u32));
    dphy_ctrl55.bits.cfg2phy_bit_num = (hi_u8)dphy_bit_width;
    panel_reg_write(&(panel_dphy_reg()->dphy_ctrl55.u32), dphy_ctrl55.u32);
}

hi_void hal_panel_set_combo_dphy_clk_cfg(hal_panel_intftype intf_type)
{
    return;
}

hi_void hal_panel_set_combo_aphy_enable(hi_bool power_on)
{
    u_aphy_pd_ctrl aphy_pd_ctrl;
    u_aphy_pd_rt   aphy_pd_rt;

    aphy_pd_ctrl.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_pd_ctrl.u32));
    aphy_pd_rt.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_pd_rt.u32));

    aphy_pd_ctrl.bits.cfg2phy_pd_bg = !power_on;
    aphy_pd_ctrl.bits.cfg2phy_pd_ldo = ((hi_u32)(!power_on) << 1) | ((hi_u32)!power_on);
    aphy_pd_ctrl.bits.cfg2phy_txpll_pd = !power_on;
    aphy_pd_rt.bits.cfg2phy_pd_rt = !power_on; /* 1: low consumption; 0: high performance */

    panel_reg_write(&(panel_dphy_reg()->aphy_pd_ctrl.u32), aphy_pd_ctrl.u32);
    panel_reg_write(&(panel_dphy_reg()->aphy_pd_rt.u32), aphy_pd_rt.u32);
}

hi_void hal_panel_set_combo_aphy_pll(hi_void)
{
    u_pll_ctrl pll_ctrl;

    pll_ctrl.u32 = panel_reg_read(&(panel_dphy_reg()->pll_ctrl.u32));
    pll_ctrl.bits.cfg2pll_lock_sel = 0x3;

    panel_reg_write(&(panel_dphy_reg()->pll_ctrl.u32), pll_ctrl.u32);
}

hi_s32 hal_panel_check_combo_phy_pll_lock(hi_void)
{
    u_ro_pll1 ro_pll1;
    hi_u32 i_pll_check_count = 0;

    while (1) {
        i_pll_check_count++;
        ro_pll1.u32 = panel_reg_read(&(panel_dphy_reg()->ro_pll1.u32));
        if (ro_pll1.bits.aphy_clk_lock_state == 0x1) {
            hi_log_info("dphy PLL LOCKED\n");
            break;
        }
        if (i_pll_check_count > 100) {      /* 100: default parm */
            hi_log_err("dphy PLL UNLOCKED\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_void panel_set_aphy_gpio_sel(hi_void)
{
    u_gpio_ctrl1 gpio_ctrl1;

    gpio_ctrl1.u32 = panel_reg_read(&(panel_dphy_reg()->gpio_ctrl1.u32));
    gpio_ctrl1.bits.cfg2phy_gpio_sel = 0x1;

    panel_reg_write(&(panel_dphy_reg()->gpio_ctrl1.u32), gpio_ctrl1.u32);
}

hi_void hal_panel_set_aphy_pre_emp_power_on(hi_bool pre_emphasis_on)
{
    return;
}

hi_void hal_panel_set_aphy_lane_power_on(hi_bool lane_power_on, hi_u32 lane_used)
{
    return;
}

hi_void hal_panel_set_combo_aphy_clock_port(hi_u32 clock_port)
{
    return;
}

/* 1'b0: work; 1'b1: power down */
hi_void hal_panel_set_aphy_power_down_31_to_0(hi_u32 value)
{
    u_aphy_pd1 aphy_pd1;

    aphy_pd1.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_pd1.u32));
    aphy_pd1.bits.cfg2phy_pd_31to0 = value;

    panel_reg_write(&(panel_dphy_reg()->aphy_pd1.u32), aphy_pd1.u32);

    return;
}

/* 1'b0: work; 1'b1: power down */
hi_void hal_panel_set_aphy_power_down_63_to_32(hi_u32 value)
{
    u_aphy_pd2 aphy_pd2;

    aphy_pd2.u32 = panel_reg_read(&(panel_dphy_reg()->aphy_pd2.u32));
    aphy_pd2.bits.cfg2phy_pd_63to32 = value;

    panel_reg_write(&(panel_dphy_reg()->aphy_pd2.u32), aphy_pd2.u32);

    return;
}

hi_void hal_panel_set_ioshare(hi_void)
{
    U_PERI_CTRL_2 peri_ctrl_2;

    peri_ctrl_2.u32 = panel_reg_read(&(hi_drv_sys_get_peri_reg_ptr()->PERI_CTRL_2.u32));
    peri_ctrl_2.bits.peri_sdio0_ioshare_sel = 1;

    panel_reg_write(&(hi_drv_sys_get_peri_reg_ptr()->PERI_CTRL_2.u32), peri_ctrl_2.u32);

    return;
}

hi_void hal_panel_set_lockn(hi_void)
{
    U_ioshare_170 ioshare_170_tmp;

    ioshare_170_tmp.u32 = panel_reg_read(&(hi_drv_sys_get_io_reg_ptr()->ioshare_170.u32));
    ioshare_170_tmp.bits.ioshare_170 = 0x4; /* 0x4 is VBO_LOCKN3 */

    panel_reg_write(&(hi_drv_sys_get_io_reg_ptr()->ioshare_170.u32), ioshare_170_tmp.u32);

    return;
}

hi_void hal_panel_set_htpdn(hi_void)
{
    U_ioshare_176 ioshare_176_tmp;

    ioshare_176_tmp.u32 = panel_reg_read(&(hi_drv_sys_get_io_reg_ptr()->ioshare_176.u32));
    ioshare_176_tmp.bits.ioshare_176 = 0x4; /* 0x4 is VBO_HTP0 */

    panel_reg_write(&(hi_drv_sys_get_io_reg_ptr()->ioshare_176.u32), ioshare_176_tmp.u32);

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


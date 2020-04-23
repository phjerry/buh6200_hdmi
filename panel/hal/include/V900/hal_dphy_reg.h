/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-11-20
*/

#ifndef __UNION_DEFINE_DPHY_H_
#define __UNION_DEFINE_DPHY_H_

/* define the union u_combo_reset */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 3   ; /* [2..0]  */
        unsigned int    dphy_srst_req         : 1   ; /* [3]  */
        unsigned int    reserved_1            : 4   ; /* [7..4]  */
        unsigned int    pll_srst_req          : 1   ; /* [8]  */
        unsigned int    reserved_2            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_combo_reset;

/* define the union u_aphy_ctrl2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_mode_31to0     : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl2;
/* define the union u_aphy_ctrl3 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_mode_63to32    : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl3;
/* define the union u_aphy_ctrl4 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_en_clkch_31to0 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl4;
/* define the union u_aphy_ctrl5 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_en_clkch_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl5;
/* define the union u_aphy_ctrl6 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_sel_bg        : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 2   ; /* [3..2]  */
        unsigned int    cfg2phy_sel_ldo       : 6   ; /* [9..4]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    cfg2phy_test_dc       : 2   ; /* [13..12]  */
        unsigned int    reserved_2            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_mux_divsel    : 3   ; /* [18..16]  */
        unsigned int    reserved_3            : 1   ; /* [19]  */
        unsigned int    cfg2phy_sel_clk       : 1   ; /* [20]  */
        unsigned int    reserved_4            : 11  ; /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl6;

/* define the union u_aphy_ctrl8 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_txpll_test     : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl8;
/* define the union u_aphy_ctrl9 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_txpll_divsel_fb : 9   ; /* [8..0]  */
        unsigned int    reserved_0            : 3   ; /* [11..9]  */
        unsigned int    cfg2phy_txpll_divsel_in : 4   ; /* [15..12]  */
        unsigned int    cfg2phy_txpll_icp_ictrl : 5   ; /* [20..16]  */
        unsigned int    reserved_1            : 3   ; /* [23..21]  */
        unsigned int    cfg2phy_txpll_reset   : 1   ; /* [24]  */
        unsigned int    cfg2phy_reset_inv     : 1   ; /* [25]  */
        unsigned int    cfg2phy_manual_reset  : 1   ; /* [26]  */
        unsigned int    reserved_2            : 5   ; /* [31..27]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl9;

/* define the union u_aphy_ctrl11 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_d15to0 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl11;

/* define the union u_aphy_ctrl12 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_d31to16 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl12;

/* define the union u_aphy_ctrl13 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_d47to32 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl13;

/* define the union u_aphy_ctrl14 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_d63to48 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl14;

/* define the union u_aphy_ctrl15 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_de_d15to0 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl15;

/* define the union u_aphy_ctrl16 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_de_d31to16 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl16;

/* define the union u_aphy_ctrl17 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_de_d47to32 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl17;

/* define the union u_aphy_ctrl18 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_main_de_d63to48 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl18;

/* define the union u_aphy_ctrl19 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_d15to0 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl19;

/* define the union u_aphy_ctrl20 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_d31to16 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl20;

/* define the union u_aphy_ctrl21 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_d47to32 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl21;

/* define the union u_aphy_ctrl22 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_d63to48 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl22;

/* define the union u_aphy_ctrl23 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_de_d15to0 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl23;

/* define the union u_aphy_ctrl24 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_de_d31to16 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl24;

/* define the union u_aphy_ctrl25 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_de_d47to32 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl25;

/* define the union u_aphy_ctrl26 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_isel_pre_de_d63to48 : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl26;

/* define the union u_aphy_ctrl27 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rsel_pre_d15to0 : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 4   ; /* [15..12]  */
        unsigned int    cfg2phy_rsel_pre_d31to16 : 12  ; /* [27..16]  */
        unsigned int    reserved_1            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl27;

/* define the union u_aphy_ctrl28 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rsel_pre_d47to32 : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 4   ; /* [15..12]  */
        unsigned int    cfg2phy_rsel_pre_d63to48 : 12  ; /* [27..16]  */
        unsigned int    reserved_1            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl28;

/* define the union u_aphy_ctrl29 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rsel_pre_de_d15to0 : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 4   ; /* [15..12]  */
        unsigned int    cfg2phy_rsel_pre_de_d31to16 : 12  ; /* [27..16]  */
        unsigned int    reserved_1            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl29;

/* define the union u_aphy_ctrl30 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rsel_pre_de_d47to32 : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 4   ; /* [15..12]  */
        unsigned int    cfg2phy_rsel_pre_de_d63to48 : 12  ; /* [27..16]  */
        unsigned int    reserved_1            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl30;

/* define the union u_aphy_ctrl31 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_sel_ck_phase_ch0 : 4   ; /* [3..0]  */
        unsigned int    cfg2phy_sel_ck_phase_ch1 : 4   ; /* [7..4]  */
        unsigned int    cfg2phy_sel_ck_phase_ch2 : 4   ; /* [11..8]  */
        unsigned int    cfg2phy_sel_ck_phase_ch3 : 4   ; /* [15..12]  */
        unsigned int    cfg2phy_sel_ck_phase_ch4 : 4   ; /* [19..16]  */
        unsigned int    cfg2phy_sel_ck_phase_ch5 : 4   ; /* [23..20]  */
        unsigned int    cfg2phy_sel_ck_phase_ch6 : 4   ; /* [27..24]  */
        unsigned int    cfg2phy_sel_ck_phase_ch7 : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl31;

/* define the union u_aphy_ctrl32 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_vcm_sel_d15to0 : 16  ; /* [15..0]  */
        unsigned int    cfg2phy_vcm_sel_d31to16 : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl32;

/* define the union u_aphy_ctrl33 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_vcm_sel_d47to32 : 16  ; /* [15..0]  */
        unsigned int    cfg2phy_vcm_sel_d63to48 : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl33;

/* define the union u_aphy_ctrl34 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rt_d15to0     : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl34;

/* define the union u_aphy_ctrl35 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rt_d31to16    : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl35;

/* define the union u_aphy_ctrl36 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rt_d47to32    : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl36;

/* define the union u_aphy_ctrl37 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rt_d63to48    : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl37;

/* define the union u_aphy_ctrl38 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_test_atop_31to0 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl38;
/* define the union u_aphy_ctrl39 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_test_atop_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl39;
/* define the union u_aphy_ctrl40 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_test_drv_31to0 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl40;
/* define the union u_aphy_ctrl41 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_test_drv_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_ctrl41;
/* define the union u_aphy_ctrl42 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_bist_delay_sel_d15to0 : 16  ; /* [15..0]  */
        unsigned int    cfg2phy_bist_delay_sel_d31to16 : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl42;

/* define the union u_aphy_ctrl43 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_bist_delay_sel_d47to32 : 16  ; /* [15..0]  */
        unsigned int    cfg2phy_bist_delay_sel_d63to48 : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_ctrl43;

/* define the union u_aphy_pd_sel */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_en_bcc_mode   : 28  ; /* [27..0]  */
        unsigned int    cfg2phy_sel_boe_pd    : 1   ; /* [28]  */
        unsigned int    reserved_0            : 3   ; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_pd_sel;

/* define the union u_aphy_pd1 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_pd_31to0       : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_pd1;
/* define the union u_aphy_pd2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_pd_63to32      : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_pd2;
/* define the union u_aphy_pd_bist1 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_pd_bist_31to0  : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_pd_bist1;
/* define the union u_aphy_pd_bist2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_pd_bist_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_pd_bist2;
/* define the union u_aphy_pd_de1 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_pd_de_31to0    : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_pd_de1;
/* define the union u_aphy_pd_de2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_pd_de_63to32   : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_aphy_pd_de2;
/* define the union u_aphy_pd_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_pd_bg         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    cfg2phy_pd_ldo        : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_txpll_pd      : 1   ; /* [8]  */
        unsigned int    reserved_2            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_pd_ctrl;

/* define the union u_aphy_pd_rt */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_en_bist_3gbps : 1   ; /* [0]  */
        unsigned int    cfg2phy_txpll_en_sscdiv : 1   ; /* [1]  */
        unsigned int    cfg2phy_txpll_divsel_ssc : 5   ; /* [6..2]  */
        unsigned int    cfg2phy_pd_rt         : 1   ; /* [7]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_aphy_pd_rt;

/* define the union u_port_mask1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port0_mask    : 1   ; /* [0]  */
        unsigned int    cfg2phy_port1_mask    : 1   ; /* [1]  */
        unsigned int    cfg2phy_port2_mask    : 1   ; /* [2]  */
        unsigned int    cfg2phy_port3_mask    : 1   ; /* [3]  */
        unsigned int    cfg2phy_port4_mask    : 1   ; /* [4]  */
        unsigned int    cfg2phy_port5_mask    : 1   ; /* [5]  */
        unsigned int    cfg2phy_port6_mask    : 1   ; /* [6]  */
        unsigned int    cfg2phy_port7_mask    : 1   ; /* [7]  */
        unsigned int    cfg2phy_port8_mask    : 1   ; /* [8]  */
        unsigned int    cfg2phy_port9_mask    : 1   ; /* [9]  */
        unsigned int    cfg2phy_port10_mask   : 1   ; /* [10]  */
        unsigned int    cfg2phy_port11_mask   : 1   ; /* [11]  */
        unsigned int    cfg2phy_port12_mask   : 1   ; /* [12]  */
        unsigned int    cfg2phy_port13_mask   : 1   ; /* [13]  */
        unsigned int    cfg2phy_port14_mask   : 1   ; /* [14]  */
        unsigned int    cfg2phy_port15_mask   : 1   ; /* [15]  */
        unsigned int    cfg2phy_port16_mask   : 1   ; /* [16]  */
        unsigned int    cfg2phy_port17_mask   : 1   ; /* [17]  */
        unsigned int    cfg2phy_port18_mask   : 1   ; /* [18]  */
        unsigned int    cfg2phy_port19_mask   : 1   ; /* [19]  */
        unsigned int    cfg2phy_port20_mask   : 1   ; /* [20]  */
        unsigned int    cfg2phy_port21_mask   : 1   ; /* [21]  */
        unsigned int    cfg2phy_port22_mask   : 1   ; /* [22]  */
        unsigned int    cfg2phy_port23_mask   : 1   ; /* [23]  */
        unsigned int    cfg2phy_port24_mask   : 1   ; /* [24]  */
        unsigned int    cfg2phy_port25_mask   : 1   ; /* [25]  */
        unsigned int    cfg2phy_port26_mask   : 1   ; /* [26]  */
        unsigned int    cfg2phy_port27_mask   : 1   ; /* [27]  */
        unsigned int    cfg2phy_port28_mask   : 1   ; /* [28]  */
        unsigned int    cfg2phy_port29_mask   : 1   ; /* [29]  */
        unsigned int    cfg2phy_port30_mask   : 1   ; /* [30]  */
        unsigned int    cfg2phy_port31_mask   : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mask1;

/* define the union u_port_mask2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port32_mask   : 1   ; /* [0]  */
        unsigned int    cfg2phy_port33_mask   : 1   ; /* [1]  */
        unsigned int    cfg2phy_port34_mask   : 1   ; /* [2]  */
        unsigned int    cfg2phy_port35_mask   : 1   ; /* [3]  */
        unsigned int    cfg2phy_port36_mask   : 1   ; /* [4]  */
        unsigned int    cfg2phy_port37_mask   : 1   ; /* [5]  */
        unsigned int    cfg2phy_port38_mask   : 1   ; /* [6]  */
        unsigned int    cfg2phy_port39_mask   : 1   ; /* [7]  */
        unsigned int    cfg2phy_port40_mask   : 1   ; /* [8]  */
        unsigned int    cfg2phy_port41_mask   : 1   ; /* [9]  */
        unsigned int    cfg2phy_port42_mask   : 1   ; /* [10]  */
        unsigned int    cfg2phy_port43_mask   : 1   ; /* [11]  */
        unsigned int    cfg2phy_port44_mask   : 1   ; /* [12]  */
        unsigned int    cfg2phy_port45_mask   : 1   ; /* [13]  */
        unsigned int    cfg2phy_port46_mask   : 1   ; /* [14]  */
        unsigned int    cfg2phy_port47_mask   : 1   ; /* [15]  */
        unsigned int    cfg2phy_port48_mask   : 1   ; /* [16]  */
        unsigned int    cfg2phy_port49_mask   : 1   ; /* [17]  */
        unsigned int    cfg2phy_port50_mask   : 1   ; /* [18]  */
        unsigned int    cfg2phy_port51_mask   : 1   ; /* [19]  */
        unsigned int    cfg2phy_port52_mask   : 1   ; /* [20]  */
        unsigned int    cfg2phy_port53_mask   : 1   ; /* [21]  */
        unsigned int    cfg2phy_port54_mask   : 1   ; /* [22]  */
        unsigned int    cfg2phy_port55_mask   : 1   ; /* [23]  */
        unsigned int    cfg2phy_port56_mask   : 1   ; /* [24]  */
        unsigned int    cfg2phy_port57_mask   : 1   ; /* [25]  */
        unsigned int    cfg2phy_port58_mask   : 1   ; /* [26]  */
        unsigned int    cfg2phy_port59_mask   : 1   ; /* [27]  */
        unsigned int    cfg2phy_port60_mask   : 1   ; /* [28]  */
        unsigned int    cfg2phy_port61_mask   : 1   ; /* [29]  */
        unsigned int    cfg2phy_port62_mask   : 1   ; /* [30]  */
        unsigned int    cfg2phy_port63_mask   : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mask2;

/* define the union u_port_chinv1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port0_chinv   : 1   ; /* [0]  */
        unsigned int    cfg2phy_port1_chinv   : 1   ; /* [1]  */
        unsigned int    cfg2phy_port2_chinv   : 1   ; /* [2]  */
        unsigned int    cfg2phy_port3_chinv   : 1   ; /* [3]  */
        unsigned int    cfg2phy_port4_chinv   : 1   ; /* [4]  */
        unsigned int    cfg2phy_port5_chinv   : 1   ; /* [5]  */
        unsigned int    cfg2phy_port6_chinv   : 1   ; /* [6]  */
        unsigned int    cfg2phy_port7_chinv   : 1   ; /* [7]  */
        unsigned int    cfg2phy_port8_chinv   : 1   ; /* [8]  */
        unsigned int    cfg2phy_port9_chinv   : 1   ; /* [9]  */
        unsigned int    cfg2phy_port10_chinv  : 1   ; /* [10]  */
        unsigned int    cfg2phy_port11_chinv  : 1   ; /* [11]  */
        unsigned int    cfg2phy_port12_chinv  : 1   ; /* [12]  */
        unsigned int    cfg2phy_port13_chinv  : 1   ; /* [13]  */
        unsigned int    cfg2phy_port14_chinv  : 1   ; /* [14]  */
        unsigned int    cfg2phy_port15_chinv  : 1   ; /* [15]  */
        unsigned int    cfg2phy_port16_chinv  : 1   ; /* [16]  */
        unsigned int    cfg2phy_port17_chinv  : 1   ; /* [17]  */
        unsigned int    cfg2phy_port18_chinv  : 1   ; /* [18]  */
        unsigned int    cfg2phy_port19_chinv  : 1   ; /* [19]  */
        unsigned int    cfg2phy_port20_chinv  : 1   ; /* [20]  */
        unsigned int    cfg2phy_port21_chinv  : 1   ; /* [21]  */
        unsigned int    cfg2phy_port22_chinv  : 1   ; /* [22]  */
        unsigned int    cfg2phy_port23_chinv  : 1   ; /* [23]  */
        unsigned int    cfg2phy_port24_chinv  : 1   ; /* [24]  */
        unsigned int    cfg2phy_port25_chinv  : 1   ; /* [25]  */
        unsigned int    cfg2phy_port26_chinv  : 1   ; /* [26]  */
        unsigned int    cfg2phy_port27_chinv  : 1   ; /* [27]  */
        unsigned int    cfg2phy_port28_chinv  : 1   ; /* [28]  */
        unsigned int    cfg2phy_port29_chinv  : 1   ; /* [29]  */
        unsigned int    cfg2phy_port30_chinv  : 1   ; /* [30]  */
        unsigned int    cfg2phy_port31_chinv  : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_chinv1;

/* define the union u_port_chinv2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port32_chinv  : 1   ; /* [0]  */
        unsigned int    cfg2phy_port33_chinv  : 1   ; /* [1]  */
        unsigned int    cfg2phy_port34_chinv  : 1   ; /* [2]  */
        unsigned int    cfg2phy_port35_chinv  : 1   ; /* [3]  */
        unsigned int    cfg2phy_port36_chinv  : 1   ; /* [4]  */
        unsigned int    cfg2phy_port37_chinv  : 1   ; /* [5]  */
        unsigned int    cfg2phy_port38_chinv  : 1   ; /* [6]  */
        unsigned int    cfg2phy_port39_chinv  : 1   ; /* [7]  */
        unsigned int    cfg2phy_port40_chinv  : 1   ; /* [8]  */
        unsigned int    cfg2phy_port41_chinv  : 1   ; /* [9]  */
        unsigned int    cfg2phy_port42_chinv  : 1   ; /* [10]  */
        unsigned int    cfg2phy_port43_chinv  : 1   ; /* [11]  */
        unsigned int    cfg2phy_port44_chinv  : 1   ; /* [12]  */
        unsigned int    cfg2phy_port45_chinv  : 1   ; /* [13]  */
        unsigned int    cfg2phy_port46_chinv  : 1   ; /* [14]  */
        unsigned int    cfg2phy_port47_chinv  : 1   ; /* [15]  */
        unsigned int    cfg2phy_port48_chinv  : 1   ; /* [16]  */
        unsigned int    cfg2phy_port49_chinv  : 1   ; /* [17]  */
        unsigned int    cfg2phy_port50_chinv  : 1   ; /* [18]  */
        unsigned int    cfg2phy_port51_chinv  : 1   ; /* [19]  */
        unsigned int    cfg2phy_port52_chinv  : 1   ; /* [20]  */
        unsigned int    cfg2phy_port53_chinv  : 1   ; /* [21]  */
        unsigned int    cfg2phy_port54_chinv  : 1   ; /* [22]  */
        unsigned int    cfg2phy_port55_chinv  : 1   ; /* [23]  */
        unsigned int    cfg2phy_port56_chinv  : 1   ; /* [24]  */
        unsigned int    cfg2phy_port57_chinv  : 1   ; /* [25]  */
        unsigned int    cfg2phy_port58_chinv  : 1   ; /* [26]  */
        unsigned int    cfg2phy_port59_chinv  : 1   ; /* [27]  */
        unsigned int    cfg2phy_port60_chinv  : 1   ; /* [28]  */
        unsigned int    cfg2phy_port61_chinv  : 1   ; /* [29]  */
        unsigned int    cfg2phy_port62_chinv  : 1   ; /* [30]  */
        unsigned int    cfg2phy_port63_chinv  : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_chinv2;

/* define the union u_dphy_ctrl55 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_test_en       : 1   ; /* [0]  */
        unsigned int    cfg2phy_test_inv      : 1   ; /* [1]  */
        unsigned int    reserved_0            : 2   ; /* [3..2]  */
        unsigned int    cfg2phy_test_mode     : 2   ; /* [5..4]  */
        unsigned int    cfg2phy_bist_sel      : 1   ; /* [6]  */
        unsigned int    reserved_1            : 1   ; /* [7]  */
        unsigned int    cfg2phy_bit_num       : 6   ; /* [13..8]  */
        unsigned int    reserved_2            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_intf_sel      : 2   ; /* [17..16]  */
        unsigned int    reserved_3            : 2   ; /* [19..18]  */
        unsigned int    cfg2phy_rd_data_thr   : 5   ; /* [24..20]  */
        unsigned int    reserved_4            : 3   ; /* [27..25]  */
        unsigned int    cfg2phy_sample_cnt    : 3   ; /* [30..28]  */
        unsigned int    reserved_5            : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl55;

/* define the union u_dphy_ctrl56 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_bist_pattern1 : 20  ; /* [19..0]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl56;

/* define the union u_dphy_ctrl57 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_bist_pattern2 : 20  ; /* [19..0]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl57;

/* define the union u_misc_ctrl2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_misc_ctrl2     : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
        unsigned int    u32;
} u_misc_ctrl2;
/* define the union u_pll_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2pll_ref_unlock_timer : 8   ; /* [7..0]  */
        unsigned int    cfg2pll_ref_lock_timer : 8   ; /* [15..8]  */
        unsigned int    cfg2pll_lockext_sel   : 3   ; /* [18..16]  */
        unsigned int    reserved_0            : 1   ; /* [19]  */
        unsigned int    cfg2pll_fb_prd_sel    : 2   ; /* [21..20]  */
        unsigned int    cfg2pll_fb_div_sel    : 2   ; /* [23..22]  */
        unsigned int    cfg2pll_lock_times    : 1   ; /* [24]  */
        unsigned int    cfg2pll_unlock_sel    : 1   ; /* [25]  */
        unsigned int    cfg2pll_clk_stable    : 1   ; /* [26]  */
        unsigned int    reserved_1            : 1   ; /* [27]  */
        unsigned int    cfg2pll_lock_sel      : 2   ; /* [29..28]  */
        unsigned int    reserved_2            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_pll_ctrl;

/* define the union u_dphy_debug1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_rd_valid_data : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_debug1;

/* define the union u_dphy_ctrl58 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2dphy_fifo15to0_delay_cnt : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl58;
/* define the union u_dphy_ctrl59 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2dphy_fifo31to16_delay_cnt : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl59;
/* define the union u_dphy_ctrl60 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2dphy_mlvds_mode   : 1   ; /* [0]  */
        unsigned int    reserved_0            : 8   ; /* [8..1]  */
        unsigned int    cfg2phy_fifo_srst     : 1   ; /* [9]  */
        unsigned int    reserved_1            : 2   ; /* [11..10]  */
        unsigned int    cfg2phy_int_mask      : 1   ; /* [12]  */
        unsigned int    combotx_int           : 1   ; /* [13]  */
        unsigned int    cfg2phy_int_clr       : 1   ; /* [14]  */
        unsigned int    cfg2phy_fifo_err_clr  : 1   ; /* [15]  */
        unsigned int    combo_pll_lock_cnt    : 8   ; /* [23..16]  */
        unsigned int    reserved_2            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl60;

/* define the union u_dphy_ctrl61 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2dphy_fifo_mask_31to0 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl61;
/* define the union u_dphy_ctrl62 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2dphy_fifo_mask_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ctrl62;
/* define the union u_port_mux1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port0_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port1_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port2_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port3_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux1;

/* define the union u_port_mux2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port4_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port5_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port6_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port7_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux2;

/* define the union u_port_mux3 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port8_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port9_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port10_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port11_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux3;

/* define the union u_port_mux4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port12_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port13_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port14_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port15_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux4;

/* define the union u_port_mux5 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port16_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port17_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port18_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port19_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux5;

/* define the union u_port_mux6 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port20_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port21_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port22_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port23_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux6;

/* define the union u_port_mux7 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port24_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port25_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port26_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port27_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux7;

/* define the union u_port_mux8 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port28_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port29_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port30_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port31_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux8;

/* define the union u_port_mux9 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port32_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port33_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port34_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port35_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux9;

/* define the union u_port_mux10 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port36_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port37_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port38_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port39_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux10;

/* define the union u_port_mux11 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port40_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port41_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port42_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port43_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux11;

/* define the union u_port_mux12 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port44_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port45_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port46_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port47_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux12;

/* define the union u_port_mux13 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port48_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port49_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port50_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port51_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux13;

/* define the union u_port_mux14 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port52_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port53_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port54_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port55_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux14;

/* define the union u_port_mux15 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port56_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port57_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port58_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port59_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux15;

/* define the union u_port_mux16 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2phy_port60_mux_sel : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cfg2phy_port61_mux_sel : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    cfg2phy_port62_mux_sel : 6   ; /* [21..16]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    cfg2phy_port63_mux_sel : 6   ; /* [29..24]  */
        unsigned int    reserved_3            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_port_mux16;

/* define the union u_dphy_ssc0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2dphy_step         : 10  ; /* [9..0]  */
        unsigned int    cfg2dphy_ssc_sync     : 1   ; /* [10]  */
        unsigned int    cfg2dphy_ssc_mode     : 1   ; /* [11]  */
        unsigned int    cfg2dphy_span         : 14  ; /* [25..12]  */
        unsigned int    cfg2dphy_set_load     : 1   ; /* [26]  */
        unsigned int    cfg2dphy_tune_reverse : 1   ; /* [27]  */
        unsigned int    cfg2dphy_hkmash_en    : 1   ; /* [28]  */
        unsigned int    cfg2dphy_mash111_hk_sel : 1   ; /* [29]  */
        unsigned int    reserved_0            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ssc0;

/* define the union u_dphy_ssc1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2dphy_testd        : 8   ; /* [7..0]  */
        unsigned int    cfg2dphy_set          : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ssc1;

/* define the union u_dphy_ssc2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2dphy_tune_step    : 8   ; /* [7..0]  */
        unsigned int    cfg2dphy_sscfifo_upper_thresh : 10  ; /* [17..8]  */
        unsigned int    cfg2dphy_deep_color   : 2   ; /* [19..18]  */
        unsigned int    cfg2dphy_sscfifo_lower_thresh : 10  ; /* [29..20]  */
        unsigned int    cfg2dphy_sscfifo_tune_en : 1   ; /* [30]  */
        unsigned int    cfg2dphy_sscfifo_depth_clr : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ssc2;

/* define the union u_dphy_ssc3 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2dphy_tune_interval : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ssc3;
/* define the union u_dphy_ssc4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2dphy_dnum         : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_dphy_ssc4;

/* define the union u_gpio_ctrl1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2misc_sel_bistclk  : 6   ; /* [5..0]  */
        unsigned int    cfg2phy_gpio_sel      : 1   ; /* [6]  */
        unsigned int    cfg2phy_gpio_oen_sel  : 1   ; /* [7]  */
        unsigned int    cfg2gpio_pin_sel      : 6   ; /* [13..8]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_gpio_ctrl1;

/* define the union u_gpio_ctrl2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_en_gpio_31to0  : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_gpio_ctrl2;
/* define the union u_gpio_ctrl3 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_en_gpio_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_gpio_ctrl3;
/* define the union u_gpio_ctrl4 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_en_gpio_input_mode_31to0 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_gpio_ctrl4;
/* define the union u_gpio_ctrl5 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int cfg2phy_en_gpio_input_mode_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_gpio_ctrl5;
/* define the union u_mem_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cfg2dphy_mem_ctrl     : 13  ; /* [12..0]  */
        unsigned int    reserved_0            : 19  ; /* [31..13]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_mem_ctrl;

/* define the union u_ro_pll1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    test_pll_cnt_state    : 17  ; /* [16..0]  */
        unsigned int    reserved_0            : 3   ; /* [19..17]  */
        unsigned int    aphy_clk_lock_state   : 1   ; /* [20]  */
        unsigned int    pll_lock_oneshot_state : 1   ; /* [21]  */
        unsigned int    pll_ena_edge_state    : 1   ; /* [22]  */
        unsigned int    clk_fb_exist_state    : 1   ; /* [23]  */
        unsigned int    reserved_1            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_pll1;

/* define the union u_ro_pll2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    clk_ref_dig_state     : 1   ; /* [0]  */
        unsigned int    clk_fb_dig_state      : 1   ; /* [1]  */
        unsigned int    ad_txpll_lock_state   : 1   ; /* [2]  */
        unsigned int    ad_txpll_hvflag_state : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_pll2;

/* define the union u_ro_fifo1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    fifo_err_out          : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_fifo1;

/* define the union u_ro_fifo2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ro_rd_valid_data      : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_fifo2;

/* define the union u_bist_clk_cnt */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    bist_clk_cnt          : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_bist_clk_cnt;

/* define the union u_pll_ssc_state1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    sscfb_ssc_offset      : 24  ; /* [23..0]  */
        unsigned int    sscfb_div             : 5   ; /* [28..24]  */
        unsigned int    sscfb_reverse_flag    : 1   ; /* [29]  */
        unsigned int    sscfb_segment         : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_pll_ssc_state1;

/* define the union u_pll_ssc_state2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    sscfb_sscfifo_depth_min : 10  ; /* [9..0]  */
        unsigned int    sscfb_sscfifo_depth_max : 10  ; /* [19..10]  */
        unsigned int    fb_ssc_off            : 1   ; /* [20]  */
        unsigned int    reserved_0            : 11  ; /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_pll_ssc_state2;

/* define the union u_pll_ssc_state3 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    sscfb_span_cnt        : 15  ; /* [14..0]  */
        unsigned int    sscfb_stop_flag       : 1   ; /* [15]  */
        unsigned int    fbmash_span_cnt       : 15  ; /* [30..16]  */
        unsigned int    reserved_0            : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_pll_ssc_state3;

/* define the union u_pll_ssc_state4 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    fbmash_sscfifo_depth_max : 10  ; /* [9..0]  */
        unsigned int    fbmash_sscfifo_depth_min : 10  ; /* [19..10]  */
        unsigned int    fbmash_stop_flag      : 1   ; /* [20]  */
        unsigned int    fbmash_ssc_off        : 1   ; /* [21]  */
        unsigned int    reserved_0            : 10  ; /* [31..22]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_pll_ssc_state4;

/* define the union u_pll_ssc_state5 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    fbmash_ssc_offset     : 24  ; /* [23..0]  */
        unsigned int    fbmash_div            : 5   ; /* [28..24]  */
        unsigned int    fbmash_reverse_flag   : 1   ; /* [29]  */
        unsigned int    fbmash_segment        : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_pll_ssc_state5;

/* define the union u_ro_gpio1 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int ad_gpio_out_state      : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_gpio1;
/* define the union u_ro_gpio2 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int ad_bistclk_state_31to0 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_gpio2;
/* define the union u_ro_gpio3 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int ad_bistclk_state_63to32 : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} u_ro_gpio3;

/* define the global struct */
typedef struct {
    unsigned int                    reserved_1                       ; /* 0x0 */
    u_combo_reset                   combo_reset                      ; /* 0x4 */
    unsigned int                    reserved_2                       ; /* 0x8 */
    u_aphy_ctrl2                    aphy_ctrl2                       ; /* 0xc */
    u_aphy_ctrl3                    aphy_ctrl3                       ; /* 0x10 */
    u_aphy_ctrl4                    aphy_ctrl4                       ; /* 0x14 */
    u_aphy_ctrl5                    aphy_ctrl5                       ; /* 0x18 */
    u_aphy_ctrl6                    aphy_ctrl6                       ; /* 0x1c */
    unsigned int                    reserved_3                       ; /* 0x20 */
    u_aphy_ctrl8                    aphy_ctrl8                       ; /* 0x24 */
    u_aphy_ctrl9                    aphy_ctrl9                       ; /* 0x28 */
    unsigned int                    reserved_4                       ; /* 0x2c */
    u_aphy_ctrl11                   aphy_ctrl11                      ; /* 0x30 */
    u_aphy_ctrl12                   aphy_ctrl12                      ; /* 0x34 */
    u_aphy_ctrl13                   aphy_ctrl13                      ; /* 0x38 */
    u_aphy_ctrl14                   aphy_ctrl14                      ; /* 0x3c */
    u_aphy_ctrl15                   aphy_ctrl15                      ; /* 0x40 */
    u_aphy_ctrl16                   aphy_ctrl16                      ; /* 0x44 */
    u_aphy_ctrl17                   aphy_ctrl17                      ; /* 0x48 */
    u_aphy_ctrl18                   aphy_ctrl18                      ; /* 0x4c */
    u_aphy_ctrl19                   aphy_ctrl19                      ; /* 0x50 */
    u_aphy_ctrl20                   aphy_ctrl20                      ; /* 0x54 */
    u_aphy_ctrl21                   aphy_ctrl21                      ; /* 0x58 */
    u_aphy_ctrl22                   aphy_ctrl22                      ; /* 0x5c */
    u_aphy_ctrl23                   aphy_ctrl23                      ; /* 0x60 */
    u_aphy_ctrl24                   aphy_ctrl24                      ; /* 0x64 */
    u_aphy_ctrl25                   aphy_ctrl25                      ; /* 0x68 */
    u_aphy_ctrl26                   aphy_ctrl26                      ; /* 0x6c */
    u_aphy_ctrl27                   aphy_ctrl27                      ; /* 0x70 */
    u_aphy_ctrl28                   aphy_ctrl28                      ; /* 0x74 */
    u_aphy_ctrl29                   aphy_ctrl29                      ; /* 0x78 */
    u_aphy_ctrl30                   aphy_ctrl30                      ; /* 0x7c */
    u_aphy_ctrl31                   aphy_ctrl31                      ; /* 0x80 */
    u_aphy_ctrl32                   aphy_ctrl32                      ; /* 0x84 */
    u_aphy_ctrl33                   aphy_ctrl33                      ; /* 0x88 */
    unsigned int                    reserved_5                       ; /* 0x8c */
    u_aphy_ctrl34                   aphy_ctrl34                      ; /* 0x90 */
    u_aphy_ctrl35                   aphy_ctrl35                      ; /* 0x94 */
    u_aphy_ctrl36                   aphy_ctrl36                      ; /* 0x98 */
    u_aphy_ctrl37                   aphy_ctrl37                      ; /* 0x9c */
    u_aphy_ctrl38                   aphy_ctrl38                      ; /* 0xa0 */
    u_aphy_ctrl39                   aphy_ctrl39                      ; /* 0xa4 */
    u_aphy_ctrl40                   aphy_ctrl40                      ; /* 0xa8 */
    u_aphy_ctrl41                   aphy_ctrl41                      ; /* 0xac */
    u_aphy_ctrl42                   aphy_ctrl42                      ; /* 0xb0 */
    u_aphy_ctrl43                   aphy_ctrl43                      ; /* 0xb4 */
    unsigned int                    reserved_6[18]                   ; /* 0xb8~0xfc, 18 is byte num */
    u_aphy_pd_sel                   aphy_pd_sel                      ; /* 0x100 */
    u_aphy_pd1                      aphy_pd1                         ; /* 0x104 */
    u_aphy_pd2                      aphy_pd2                         ; /* 0x108 */
    u_aphy_pd_bist1                 aphy_pd_bist1                    ; /* 0x10c */
    u_aphy_pd_bist2                 aphy_pd_bist2                    ; /* 0x110 */
    u_aphy_pd_de1                   aphy_pd_de1                      ; /* 0x114 */
    u_aphy_pd_de2                   aphy_pd_de2                      ; /* 0x118 */
    u_aphy_pd_ctrl                  aphy_pd_ctrl                     ; /* 0x11c */
    unsigned int                    reserved_7[2]                    ; /* 0x120~0x124, 2 is byte num */
    u_aphy_pd_rt                    aphy_pd_rt                       ; /* 0x128 */
    unsigned int                    reserved_8[53]                   ; /* 0x12c~0x1fc, 53 is byte num */
    u_port_mask1                    port_mask1                       ; /* 0x200 */
    u_port_mask2                    port_mask2                       ; /* 0x204 */
    u_port_chinv1                   port_chinv1                      ; /* 0x208 */
    u_port_chinv2                   port_chinv2                      ; /* 0x20c */
    unsigned int                    reserved_9[5]                    ; /* 0x210~0x220, 5 is byte num */
    u_dphy_ctrl55                   dphy_ctrl55                      ; /* 0x224 */
    u_dphy_ctrl56                   dphy_ctrl56                      ; /* 0x228 */
    u_dphy_ctrl57                   dphy_ctrl57                      ; /* 0x22c */
    u_misc_ctrl2                    misc_ctrl2                       ; /* 0x230 */
    u_pll_ctrl                      pll_ctrl                         ; /* 0x234 */
    u_dphy_debug1                   dphy_debug1                      ; /* 0x238 */
    unsigned int                    reserved_10                      ; /* 0x23c */
    u_dphy_ctrl58                   dphy_ctrl58                      ; /* 0x240 */
    u_dphy_ctrl59                   dphy_ctrl59                      ; /* 0x244 */
    u_dphy_ctrl60                   dphy_ctrl60                      ; /* 0x248 */
    u_dphy_ctrl61                   dphy_ctrl61                      ; /* 0x24c */
    u_dphy_ctrl62                   dphy_ctrl62                      ; /* 0x250 */
    u_port_mux1                     port_mux1                        ; /* 0x254 */
    u_port_mux2                     port_mux2                        ; /* 0x258 */
    u_port_mux3                     port_mux3                        ; /* 0x25c */
    u_port_mux4                     port_mux4                        ; /* 0x260 */
    u_port_mux5                     port_mux5                        ; /* 0x264 */
    u_port_mux6                     port_mux6                        ; /* 0x268 */
    u_port_mux7                     port_mux7                        ; /* 0x26c */
    u_port_mux8                     port_mux8                        ; /* 0x270 */
    u_port_mux9                     port_mux9                        ; /* 0x274 */
    u_port_mux10                    port_mux10                       ; /* 0x278 */
    u_port_mux11                    port_mux11                       ; /* 0x27c */
    u_port_mux12                    port_mux12                       ; /* 0x280 */
    u_port_mux13                    port_mux13                       ; /* 0x284 */
    u_port_mux14                    port_mux14                       ; /* 0x288 */
    u_port_mux15                    port_mux15                       ; /* 0x28c */
    u_port_mux16                    port_mux16                       ; /* 0x290 */
    u_dphy_ssc0                     dphy_ssc0                        ; /* 0x294 */
    u_dphy_ssc1                     dphy_ssc1                        ; /* 0x298 */
    u_dphy_ssc2                     dphy_ssc2                        ; /* 0x29c */
    u_dphy_ssc3                     dphy_ssc3                        ; /* 0x2a0 */
    u_dphy_ssc4                     dphy_ssc4                        ; /* 0x2a4 */
    unsigned int                    reserved_11[16]                  ; /* 0x2a8~0x2e4, 16 is byte num */
    u_gpio_ctrl1                    gpio_ctrl1                       ; /* 0x2e8 */
    u_gpio_ctrl2                    gpio_ctrl2                       ; /* 0x2ec */
    u_gpio_ctrl3                    gpio_ctrl3                       ; /* 0x2f0 */
    u_gpio_ctrl4                    gpio_ctrl4                       ; /* 0x2f4 */
    u_gpio_ctrl5                    gpio_ctrl5                       ; /* 0x2f8 */
    u_mem_ctrl                      mem_ctrl                         ; /* 0x2fc */
    unsigned int                    reserved_12[3]                   ; /* 0x300~0x308, 3 is byte num */
    u_ro_pll1                       ro_pll1                          ; /* 0x30c */
    u_ro_pll2                       ro_pll2                          ; /* 0x310 */
    u_ro_fifo1                      ro_fifo1                         ; /* 0x314 */
    u_ro_fifo2                      ro_fifo2                         ; /* 0x318 */
    u_bist_clk_cnt                  bist_clk_cnt                     ; /* 0x31c */
    u_pll_ssc_state1                pll_ssc_state1                   ; /* 0x320 */
    u_pll_ssc_state2                pll_ssc_state2                   ; /* 0x324 */
    u_pll_ssc_state3                pll_ssc_state3                   ; /* 0x328 */
    u_pll_ssc_state4                pll_ssc_state4                   ; /* 0x32c */
    u_pll_ssc_state5                pll_ssc_state5                   ; /* 0x330 */
    unsigned int                    reserved_13                      ; /* 0x334 */
    u_ro_gpio1                      ro_gpio1                         ; /* 0x338 */
    u_ro_gpio2                      ro_gpio2                         ; /* 0x33c */
    u_ro_gpio3                      ro_gpio3                         ; /* 0x340 */
} s_dphy_regs_type;

/* declare the struct pointor of the module dphy */
extern s_dphy_regs_type *g_dphy_all_reg;

#endif /* __c_union_define_dphy_h__ */

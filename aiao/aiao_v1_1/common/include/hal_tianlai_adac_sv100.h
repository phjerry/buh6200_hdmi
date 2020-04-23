/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adac hardware driver
 * Author: audio
 * Create: 2019-09-25
 * Notes: NA
 * History: 2019-09-25 first version of sv100 adac driver
 */

#ifndef __HAL_TIANLAI_ADAC_SV100_H__
#define __HAL_TIANLAI_ADAC_SV100_H__

/* Define the union U_AUDIO_ANA_CTRL_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    PD_VREF               : 1   ; /* [0]  */
        unsigned int    pd_rctune             : 1   ; /* [1]  */
        unsigned int    pd_adcl               : 1   ; /* [2]  */
        unsigned int    pd_adcr               : 1   ; /* [3]  */
        unsigned int    pd_linein_l           : 1   ; /* [4]  */
        unsigned int    pd_linein_r           : 1   ; /* [5]  */
        unsigned int    PDM_LINEOUT           : 1   ; /* [6]  */
        unsigned int    PU_POP_MUTE           : 1   ; /* [7]  */
        unsigned int    byp_chop_ctcm_rx      : 1   ; /* [8]  */
        unsigned int    ana_loop              : 1   ; /* [9]  */
        unsigned int    pd_dacl               : 1   ; /* [10]  */
        unsigned int    pd_dacr               : 1   ; /* [11]  */
        unsigned int    pd_dac_vref           : 1   ; /* [12]  */
        unsigned int    pdb_ctcm_ibias        : 1   ; /* [13]  */
        unsigned int    pd_ctcm               : 1   ; /* [14]  */
        unsigned int    pd_bias               : 1   ; /* [15]  */
        unsigned int    linein_l_gain         : 5   ; /* [20..16]  */
        unsigned int    adcl_gain_boost       : 1   ; /* [21]  */
        unsigned int    adc_dwa_byps          : 1   ; /* [22]  */
        unsigned int    mute_linein_l         : 1   ; /* [23]  */
        unsigned int    linein_r_gain         : 5   ; /* [28..24]  */
        unsigned int    adcr_gain_boost       : 1   ; /* [29]  */
        unsigned int    pd_ctcm_rx            : 1   ; /* [30]  */
        unsigned int    mute_linein_r         : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ANA_CTRL_0;

/* Define the union U_AUDIO_ANA_CTRL_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    linein_l_sel          : 4   ; /* [3..0]  */
        unsigned int    linein_r_sel          : 4   ; /* [7..4]  */
        unsigned int    adc_flstn             : 2   ; /* [9..8]  */
        unsigned int    adc_adatn             : 2   ; /* [11..10]  */
        unsigned int    adc_clktiming_sel     : 1   ; /* [12]  */
        unsigned int    adc_clk_edge_inv_sel  : 1   ; /* [13]  */
        unsigned int    sel_clk_chop_mic      : 2   ; /* [15..14]  */
        unsigned int    adc_tune_int1_code    : 5   ; /* [20..16]  */
        unsigned int    adc_rctune_en         : 1   ; /* [21]  */
        unsigned int    mute_dacl             : 1   ; /* [22]  */
        unsigned int    mute_dacr             : 1   ; /* [23]  */
        unsigned int    adc_tune_int2_code    : 5   ; /* [28..24]  */
        unsigned int    rctune_rstn           : 1   ; /* [29]  */
        unsigned int    adc_tune_sel          : 1   ; /* [30]  */
        unsigned int    PU_POP_PULLB_REG      : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ANA_CTRL_1;

/* Define the union U_AUDIO_ANA_CTRL_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    adc_clk_dly_sel       : 1   ; /* [0]  */
        unsigned int    pd_ctcm_tx            : 1   ; /* [1]  */
        unsigned int    POP_LINEOUT_PULL_VCM  : 1   ; /* [2]  */
        unsigned int    adc_tune_int3_code    : 5   ; /* [7..3]  */
        unsigned int    sel_clk_chop_ctcm     : 2   ; /* [9..8]  */
        unsigned int    sel_clk_chop_dac_vref : 2   ; /* [11..10]  */
        unsigned int    sel_clk_chop_adc_ph   : 1   ; /* [12]  */
        unsigned int    ctrl_mclk_ph          : 1   ; /* [13]  */
        unsigned int    ctrl_clk_dac_ph       : 1   ; /* [14]  */
        unsigned int    ctrl_clk_adc_ph       : 1   ; /* [15]  */
        unsigned int    sel_clk_chop_linein   : 2   ; /* [17..16]  */
        unsigned int    sel_clk_chop_vb       : 2   ; /* [19..18]  */
        unsigned int    sel_clk_chop_adc_st1  : 2   ; /* [21..20]  */
        unsigned int    sel_clk_chop_adc_st2  : 2   ; /* [23..22]  */
        unsigned int    byp_chop_ctcm         : 1   ; /* [24]  */
        unsigned int    byp_chop_dac_vref     : 1   ; /* [25]  */
        unsigned int    byp_chop_linein       : 1   ; /* [26]  */
        unsigned int    byp_chop_adc_vb       : 1   ; /* [27]  */
        unsigned int    byp_chop_adc_st2      : 1   ; /* [28]  */
        unsigned int    byp_chop_adc_st1      : 1   ; /* [29]  */
        unsigned int    pop_spd_cfg           : 2   ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ANA_CTRL_2;

/* Define the union U_AUDIO_ANA_CTRL_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ibadj_micbias         : 2   ; /* [1..0]  */
        unsigned int    ibadj_ctcm            : 2   ; /* [3..2]  */
        unsigned int    ibadj_dac             : 2   ; /* [5..4]  */
        unsigned int    ibadj_adc             : 2   ; /* [7..6]  */
        unsigned int    ibadj_linein          : 2   ; /* [9..8]  */
        unsigned int    ibadj_dac_vref        : 2   ; /* [11..10]  */
        unsigned int    micbias_adj           : 2   ; /* [13..12]  */
        unsigned int    byp_chop_mic          : 1   ; /* [14]  */
        unsigned int    RST                   : 1   ; /* [15]  */
        unsigned int    VREF_SEL              : 5   ; /* [20..16]  */
        unsigned int    VREF_FS               : 1   ; /* [21]  */
        unsigned int    vref_exmode           : 1   ; /* [22]  */
        unsigned int    pop_r_track_l_en      : 1   ; /* [23]  */
        unsigned int    pop_pdm_dly_cfg_08    : 2   ; /* [25..24]  */
        unsigned int    LINEOUTR_PD_ORG       : 1   ; /* [26]  */
        unsigned int    pop_dis_08            : 1   ; /* [27]  */
        unsigned int    LINEOUTL_PD_ORG_08    : 1   ; /* [28]  */
        unsigned int    Aux_EN                : 1   ; /* [29]  */
        unsigned int    reserved_0            : 1   ; /* [30]  */
        unsigned int    pop_sel_0h1s          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ANA_CTRL_3;

/* Define the union U_AUDIO_ANA_CTRL_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    adc_mis_seln            : 4   ; /* [3..0]  */
        unsigned int    adc_mis_selp            : 4   ; /* [7..4]  */
        unsigned int    VREF_PULLDOWN_RES_SEL   : 8   ; /* [15..8]  */
        unsigned int    pop_rmpn_tn             : 4   ; /* [19..16]  */
        unsigned int    pop_rmpp_tn             : 4   ; /* [23..20]  */
        unsigned int    rmp_bias_sel            : 2   ; /* [25..24]  */
        unsigned int    rmp_pulse_sel           : 2   ; /* [27..26]  */
        unsigned int    pdb_dac_clk             : 1   ; /* [28]  */
        unsigned int    rstb_dac                : 1   ; /* [29]  */
        unsigned int    vref_pu_pdb             : 1   ; /* [30]  */
        unsigned int    reserved_0              : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ANA_CTRL_4;

/* Define the union U_AUDIO_ANA_CTRL_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    POP_MUTE_SEL            : 3   ; /* [2..0]  */
        unsigned int    PULLOUT_WEAK            : 1   ; /* [3]  */
        unsigned int    VREF_PULLDOWN_RES_SEL   : 4   ; /* [7..4]  */
        unsigned int    reserved_0              : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ANA_CTRL_5;

/* Define the union U_AUDIO_CTRL_REG_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 8   ; /* [7..0]  */
        unsigned int    i2s2_fs_sel           : 5   ; /* [12..8]  */
        unsigned int    i2s1_fs_sel           : 5   ; /* [17..13]  */
        unsigned int    dig_loop              : 1   ; /* [18]  */
        unsigned int    dig_bypass            : 1   ; /* [19]  */
        unsigned int    i2s2_data_bits        : 2   ; /* [21..20]  */
        unsigned int    i2s1_data_bits        : 2   ; /* [23..22]  */
        unsigned int    adcr_en               : 1   ; /* [24]  */
        unsigned int    adcl_en               : 1   ; /* [25]  */
        unsigned int    dacr_en               : 1   ; /* [26]  */
        unsigned int    dacl_en               : 1   ; /* [27]  */
        unsigned int    adcr_rst_n            : 1   ; /* [28]  */
        unsigned int    adcl_rst_n            : 1   ; /* [29]  */
        unsigned int    dacr_rst_n            : 1   ; /* [30]  */
        unsigned int    dacl_rst_n            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_CTRL_REG_1;

/* Define the union U_AUDIO_DAC_REG_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dacr_lrsel            : 1   ; /* [0]  */
        unsigned int    dacr_i2ssel           : 1   ; /* [1]  */
        unsigned int    dacl_lrsel            : 1   ; /* [2]  */
        unsigned int    dal_i2ssel            : 1   ; /* [3]  */
        unsigned int    reserved_0            : 15  ; /* [18..4]  */
        unsigned int    dacr_deemph           : 2   ; /* [20..19]  */
        unsigned int    dacl_deemph           : 2   ; /* [22..21]  */
        unsigned int    muter_rate            : 2   ; /* [24..23]  */
        unsigned int    mutel_rate            : 2   ; /* [26..25]  */
        unsigned int    dacvu                 : 1   ; /* [27]  */
        unsigned int    sunmuter              : 1   ; /* [28]  */
        unsigned int    sunmutel              : 1   ; /* [29]  */
        unsigned int    smuter                : 1   ; /* [30]  */
        unsigned int    smutel                : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_DAC_REG_0;

/* Define the union U_AUDIO_DAC_REG_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dacl2dacr_vol         : 7   ; /* [6..0]  */
        unsigned int    dacl2dacr_en          : 1   ; /* [7]  */
        unsigned int    dacr2dacl_vol         : 7   ; /* [14..8]  */
        unsigned int    dacr2dacl_en          : 1   ; /* [15]  */
        unsigned int    dacr_vol              : 7   ; /* [22..16]  */
        unsigned int    dacr_mute             : 1   ; /* [23]  */
        unsigned int    dacl_vol              : 7   ; /* [30..24]  */
        unsigned int    dacl_mute             : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_DAC_REG_1;

/* Define the union U_AUDIO_ADC_REG_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    adcr_lrsel            : 1   ; /* [0]  */
        unsigned int    adcr_i2ssel           : 1   ; /* [1]  */
        unsigned int    adcl_lrsel            : 1   ; /* [2]  */
        unsigned int    adcl_i2ssel           : 1   ; /* [3]  */
        unsigned int    reserved_0            : 10  ; /* [13..4]  */
        unsigned int    adcr_hpf_en           : 1   ; /* [14]  */
        unsigned int    adcl_hpf_en           : 1   ; /* [15]  */
        unsigned int    adcr_vol              : 7   ; /* [22..16]  */
        unsigned int    adcr_mute             : 1   ; /* [23]  */
        unsigned int    adcl_vol              : 7   ; /* [30..24]  */
        unsigned int    adcl_mute             : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ADC_REG_0;

/* Define the union U_AUDIO_ADC_REG_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    adcr2dacr_vol         : 7   ; /* [6..0]  */
        unsigned int    adcr2dacr_en          : 1   ; /* [7]  */
        unsigned int    adcl2dacr_vol         : 7   ; /* [14..8]  */
        unsigned int    adcl2dacr_en          : 1   ; /* [15]  */
        unsigned int    adcr2dacl_vol         : 7   ; /* [22..16]  */
        unsigned int    adcr2dacl_en          : 1   ; /* [23]  */
        unsigned int    adcl2dacl_vol         : 7   ; /* [30..24]  */
        unsigned int    adcl2dacl_en          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_ADC_REG_1;

/* Define the union U_AUDIO_REG_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2s_audiocodec_mux_mode : 2   ; /* [1..0]  */
        unsigned int    audio_mclk_sel        : 1   ; /* [2]  */
        unsigned int    i2s_pad_mclk_sel      : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AUDIO_REG_1;

/* Define the global struct */
typedef struct {
    volatile U_AUDIO_ANA_CTRL_0     AUDIO_ANA_CTRL_0; /* 0x00 */
    volatile U_AUDIO_ANA_CTRL_1     AUDIO_ANA_CTRL_1; /* 0x04 */
    volatile U_AUDIO_ANA_CTRL_2     AUDIO_ANA_CTRL_2; /* 0x08 */
    volatile U_AUDIO_ANA_CTRL_3     AUDIO_ANA_CTRL_3; /* 0x0c */
    volatile U_AUDIO_ANA_CTRL_4     AUDIO_ANA_CTRL_4; /* 0x10 */
    volatile U_AUDIO_ANA_CTRL_5     AUDIO_ANA_CTRL_5; /* 0x14 */
    volatile unsigned int           reserved[((0xCC - 0x14) >> 2) - 1]; /* register(s) reserved between 0x14 ~ 0xcc */
    volatile U_AUDIO_CTRL_REG_1     AUDIO_CTRL_REG_1; /* 0xcc */
    volatile U_AUDIO_DAC_REG_0      AUDIO_DAC_REG_0;
    volatile U_AUDIO_DAC_REG_1      AUDIO_DAC_REG_1;
    volatile U_AUDIO_ADC_REG_0      AUDIO_ADC_REG_0;
    volatile U_AUDIO_ADC_REG_1      AUDIO_ADC_REG_1;
    volatile U_AUDIO_REG_1          AUDIO_REG_1;
} S_ACODEC_REGS_TYPE, acodec_reg;

#if defined(HI_TIANLAI_SV100)
hi_void adac_tianlai_sv100_register_driver(struct osal_list_head *head);
#else
#define adac_tianlai_sv100_register_driver(head)
#endif

#endif /* __HAL_TIANLAI_ADAC_SV100_H__ */


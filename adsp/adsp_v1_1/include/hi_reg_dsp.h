/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp driver regs define.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __HI_REG_DSP_H__
#define __HI_REG_DSP_H__

/* Define the union U_INTR_RAW_SECUE_REGS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dsp2cpu_secue_int     : 1  ; /* [0]  */
        unsigned int    secue_sw_com_int      : 4  ; /* [4..1]  */
        unsigned int    reserved_0            : 27 ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INTR_RAW_SECUE_REGS;

/* Define the union U_INTR_SECUE_MSK_REGS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tcm_aw_int_mask       : 1  ; /* [0]  */
        unsigned int    tcm_ar_int_mask       : 1  ; /* [1]  */
        unsigned int    doubleexc_err_int_mask : 1  ; /* [2]  */
        unsigned int    pfataerr_int_mask     : 1  ; /* [3]  */
        unsigned int    slv_aw_int_mask       : 1  ; /* [4]  */
        unsigned int    slv_ar_int_mask       : 1  ; /* [5]  */
        unsigned int    mmu_secue_int_mask    : 1  ; /* [6]  */
        unsigned int    aiao_int_mask         : 1  ; /* [7]  */
        unsigned int    mad_int_mask          : 1  ; /* [8]  */
        unsigned int    mad_rx_int_mask       : 1  ; /* [9]  */
        unsigned int    dsp2cpu_secue_int_mask : 1  ; /* [10]  */
        unsigned int    secue_sw_com_int_mask : 4  ; /* [14..11]  */
        unsigned int    reserved_0            : 17 ; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INTR_SECUE_MSK_REGS;

/* Define the union U_INTR_SECUE_STATUS_REGS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tcm_wr_int            : 1  ; /* [0]  */
        unsigned int    tcm_rd_int            : 1  ; /* [1]  */
        unsigned int    double_exc_int        : 1  ; /* [2]  */
        unsigned int    pfatal_int            : 1  ; /* [3]  */
        unsigned int    slv_wr_int            : 1  ; /* [4]  */
        unsigned int    slv_rd_int            : 1  ; /* [5]  */
        unsigned int    mmu2dsp_secue_int     : 1  ; /* [6]  */
        unsigned int    aiao_int              : 1  ; /* [7]  */
        unsigned int    mad_int               : 1  ; /* [8]  */
        unsigned int    mad_rx_int            : 1  ; /* [9]  */
        unsigned int    dsp2cpu_secue_int_sta : 1  ; /* [10]  */
        unsigned int    secue_sw_com_int_sta  : 4  ; /* [14..11]  */
        unsigned int    reserved_0            : 17 ; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INTR_SECUE_STATUS_REGS;

/* Define the union U_PFAULTINFO */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int pfaultinfo             : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PFAULTINFO;
/* Define the union U_TCM_AW_ADDR */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_aw_addr            : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_AW_ADDR;
/* Define the union U_TCM_AR_ADDR */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_ar_addr            : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_AR_ADDR;
/* Define the union U_DSP_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    xocdmode              : 1  ; /* [0]  */
        unsigned int    debugmode             : 1  ; /* [1]  */
        unsigned int    reserved_0            : 30 ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DSP_STATUS;

/* Define the union U_TCM_AW_INT_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tcm_aw_int_clr        : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_AW_INT_CLR;

/* Define the union U_TCM_AR_INT_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tcm_ar_int_clr        : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_AR_INT_CLR;

/* Define the union U_SLV_AW_INT_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    slv_aw_int_clr        : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_SLV_AW_INT_CLR;

/* Define the union U_SLV_AR_INT_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    slv_ar_int_clr        : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_SLV_AR_INT_CLR;

/* Define the union U_DOUBLE_EXC_INT_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    double_exc_int_clr    : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DOUBLE_EXC_INT_CLR;

/* Define the union U_PFATAL_INT_CLR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pfatal_int_clr        : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PFATAL_INT_CLR;

/* Define the union U_MMU_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_clean             : 1  ; /* [0]  */
        unsigned int    mmu_start             : 1  ; /* [1]  */
        unsigned int    mmu_awdummy           : 1  ; /* [2]  */
        unsigned int    mmu_ardummy           : 1  ; /* [3]  */
        unsigned int    mmu_chn0_awinfo       : 3  ; /* [6..4]  */
        unsigned int    mmu_chn0_arinfo       : 3  ; /* [9..7]  */
        unsigned int    mmu_chn1_awinfo       : 3  ; /* [12..10]  */
        unsigned int    mmu_chn1_arinfo       : 3  ; /* [15..13]  */
        unsigned int    mmu_chn2_awinfo       : 3  ; /* [18..16]  */
        unsigned int    mmu_chn2_arinfo       : 3  ; /* [21..19]  */
        unsigned int    reserved_0            : 10 ; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_MMU_CTRL;

/* Define the union U_MEMORY_CTRL */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int memory_ctrl            : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_MEMORY_CTRL;
/* Define the union U_TCM_BUF0_START_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf0_start_addr0   : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF0_START_ADDR0;
/* Define the union U_TCM_BUF0_END_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf0_end_addr0     : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF0_END_ADDR0;
/* Define the union U_TCM_BUF1_START_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf1_start_addr0   : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF1_START_ADDR0;
/* Define the union U_TCM_BUF1_END_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf1_end_addr0     : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF1_END_ADDR0;
/* Define the union U_TCM_BUF2_START_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf2_start_addr0   : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF2_START_ADDR0;
/* Define the union U_TCM_BUF2_END_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf2_end_addr0     : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF2_END_ADDR0;
/* Define the union U_TCM_BUF3_START_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf3_start_addr0   : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF3_START_ADDR0;
/* Define the union U_TCM_BUF3_END_ADDR0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int tcm_buf3_end_addr0     : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF3_END_ADDR0;
/* Define the union U_TCM_BUF_INFO */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    buf0_wr_en            : 1  ; /* [0]  */
        unsigned int    buf0_rd_en            : 1  ; /* [1]  */
        unsigned int    buf0_secue_en         : 1  ; /* [2]  */
        unsigned int    buf1_wr_en            : 1  ; /* [3]  */
        unsigned int    buf1_rd_en            : 1  ; /* [4]  */
        unsigned int    buf1_secue_en         : 1  ; /* [5]  */
        unsigned int    buf2_wr_en            : 1  ; /* [6]  */
        unsigned int    buf2_rd_en            : 1  ; /* [7]  */
        unsigned int    buf2_secue_en         : 1  ; /* [8]  */
        unsigned int    buf3_wr_en            : 1  ; /* [9]  */
        unsigned int    buf3_rd_en            : 1  ; /* [10]  */
        unsigned int    buf3_secue_en         : 1  ; /* [11]  */
        unsigned int    reserved_0            : 20 ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TCM_BUF_INFO;

/* Define the union U_DSP_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dsp_en                : 1  ; /* [0]  */
        unsigned int    runstall              : 1  ; /* [1]  */
        unsigned int    ocdhaltonreset        : 1  ; /* [2]  */
        unsigned int    syn_dbg_sel           : 1  ; /* [3]  */
        unsigned int    reserved_0            : 28 ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DSP_CTRL;

/* Define the union U_DDR_BUF0_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf0_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF0_START;
/* Define the union U_DDR_BUF1_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf1_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF1_START;
/* Define the union U_DDR_BUF2_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf2_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF2_START;
/* Define the union U_DDR_BUF3_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf3_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF3_START;
/* Define the union U_DDR_BUF4_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf4_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF4_START;
/* Define the union U_DDR_BUF5_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf5_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF5_START;
/* Define the union U_DDR_BUF6_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf6_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF6_START;
/* Define the union U_DDR_BUF7_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf7_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF7_START;
/* Define the union U_DDR_BUF8_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf8_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF8_START;
/* Define the union U_DDR_BUF9_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf9_start         : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF9_START;
/* Define the union U_DDR_BUF10_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf10_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF10_START;
/* Define the union U_DDR_BUF11_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf11_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF11_START;
/* Define the union U_DDR_BUF12_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf12_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF12_START;
/* Define the union U_DDR_BUF13_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf13_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF13_START;
/* Define the union U_DDR_BUF14_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf14_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF14_START;
/* Define the union U_DDR_BUF15_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf15_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF15_START;
/* Define the union U_DDR_BUF16_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf16_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF16_START;
/* Define the union U_DDR_BUF17_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf17_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF17_START;
/* Define the union U_DDR_BUF18_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf18_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF18_START;
/* Define the union U_DDR_BUF19_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf19_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF19_START;
/* Define the union U_DDR_BUF20_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf20_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF20_START;
/* Define the union U_DDR_BUF21_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf21_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF21_START;
/* Define the union U_DDR_BUF22_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf22_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF22_START;
/* Define the union U_DDR_BUF23_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf23_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF23_START;
/* Define the union U_DDR_BUF24_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf24_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF24_START;
/* Define the union U_DDR_BUF25_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf25_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF25_START;
/* Define the union U_DDR_BUF26_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf26_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF26_START;
/* Define the union U_DDR_BUF27_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf27_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF27_START;
/* Define the union U_DDR_BUF28_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf28_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF28_START;
/* Define the union U_DDR_BUF29_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf29_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF29_START;
/* Define the union U_DDR_BUF30_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf30_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF30_START;
/* Define the union U_DDR_BUF31_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf31_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF31_START;
/* Define the union U_DDR_BUF32_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf32_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF32_START;
/* Define the union U_DDR_BUF33_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf33_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF33_START;
/* Define the union U_DDR_BUF34_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf34_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF34_START;
/* Define the union U_DDR_BUF35_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf35_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF35_START;
/* Define the union U_DDR_BUF36_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf36_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF36_START;
/* Define the union U_DDR_BUF37_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf37_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF37_START;
/* Define the union U_DDR_BUF38_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf38_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF38_START;
/* Define the union U_DDR_BUF39_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf39_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF39_START;
/* Define the union U_DDR_BUF40_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf40_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF40_START;
/* Define the union U_DDR_BUF41_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf41_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF41_START;
/* Define the union U_DDR_BUF42_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf42_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF42_START;
/* Define the union U_DDR_BUF43_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf43_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF43_START;
/* Define the union U_DDR_BUF44_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf44_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF44_START;
/* Define the union U_DDR_BUF45_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf45_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF45_START;
/* Define the union U_DDR_BUF46_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf46_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF46_START;
/* Define the union U_DDR_BUF47_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf47_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF47_START;
/* Define the union U_DDR_BUF48_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf48_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF48_START;
/* Define the union U_DDR_BUF49_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf49_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF49_START;
/* Define the union U_DDR_BUF50_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf50_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF50_START;
/* Define the union U_DDR_BUF51_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf51_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF51_START;
/* Define the union U_DDR_BUF52_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf52_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF52_START;
/* Define the union U_DDR_BUF53_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf53_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF53_START;
/* Define the union U_DDR_BUF54_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf54_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF54_START;
/* Define the union U_DDR_BUF55_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf55_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF55_START;
/* Define the union U_DDR_BUF56_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf56_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF56_START;
/* Define the union U_DDR_BUF57_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf57_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF57_START;
/* Define the union U_DDR_BUF58_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf58_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF58_START;
/* Define the union U_DDR_BUF59_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf59_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF59_START;
/* Define the union U_DDR_BUF60_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf60_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF60_START;
/* Define the union U_DDR_BUF61_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf61_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF61_START;
/* Define the union U_DDR_BUF62_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf62_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF62_START;
/* Define the union U_DDR_BUF63_START */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf63_start        : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF63_START;
/* Define the union U_DDR_BUF0_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf0_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF0_END;
/* Define the union U_DDR_BUF1_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf1_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF1_END;
/* Define the union U_DDR_BUF2_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf2_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF2_END;
/* Define the union U_DDR_BUF3_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf3_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF3_END;
/* Define the union U_DDR_BUF4_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf4_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF4_END;
/* Define the union U_DDR_BUF5_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf5_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF5_END;
/* Define the union U_DDR_BUF6_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf6_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF6_END;
/* Define the union U_DDR_BUF7_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf7_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF7_END;
/* Define the union U_DDR_BUF8_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf8_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF8_END;
/* Define the union U_DDR_BUF9_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf9_end           : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF9_END;
/* Define the union U_DDR_BUF10_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf10_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF10_END;
/* Define the union U_DDR_BUF11_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf11_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF11_END;
/* Define the union U_DDR_BUF12_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf12_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF12_END;
/* Define the union U_DDR_BUF13_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf13_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF13_END;
/* Define the union U_DDR_BUF14_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf14_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF14_END;
/* Define the union U_DDR_BUF15_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf15_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF15_END;
/* Define the union U_DDR_BUF16_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf16_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF16_END;
/* Define the union U_DDR_BUF17_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf17_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF17_END;
/* Define the union U_DDR_BUF18_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf18_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF18_END;
/* Define the union U_DDR_BUF19_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf19_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF19_END;
/* Define the union U_DDR_BUF20_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf20_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF20_END;
/* Define the union U_DDR_BUF21_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf21_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF21_END;
/* Define the union U_DDR_BUF22_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf22_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF22_END;
/* Define the union U_DDR_BUF23_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf23_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF23_END;
/* Define the union U_DDR_BUF24_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf24_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF24_END;
/* Define the union U_DDR_BUF25_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf25_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF25_END;
/* Define the union U_DDR_BUF26_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf26_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF26_END;
/* Define the union U_DDR_BUF27_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf27_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF27_END;
/* Define the union U_DDR_BUF28_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf28_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF28_END;
/* Define the union U_DDR_BUF29_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf29_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF29_END;
/* Define the union U_DDR_BUF30_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf30_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF30_END;
/* Define the union U_DDR_BUF31_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf31_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF31_END;
/* Define the union U_DDR_BUF32_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf32_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF32_END;
/* Define the union U_DDR_BUF33_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf33_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF33_END;
/* Define the union U_DDR_BUF34_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf34_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF34_END;
/* Define the union U_DDR_BUF35_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf35_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF35_END;
/* Define the union U_DDR_BUF36_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf36_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF36_END;
/* Define the union U_DDR_BUF37_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf37_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF37_END;
/* Define the union U_DDR_BUF38_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf38_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF38_END;
/* Define the union U_DDR_BUF39_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf39_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF39_END;
/* Define the union U_DDR_BUF40_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf40_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF40_END;
/* Define the union U_DDR_BUF41_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf41_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF41_END;
/* Define the union U_DDR_BUF42_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf42_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF42_END;
/* Define the union U_DDR_BUF43_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf43_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF43_END;
/* Define the union U_DDR_BUF44_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf44_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF44_END;
/* Define the union U_DDR_BUF45_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf45_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF45_END;
/* Define the union U_DDR_BUF46_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf46_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF46_END;
/* Define the union U_DDR_BUF47_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf47_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF47_END;
/* Define the union U_DDR_BUF48_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf48_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF48_END;
/* Define the union U_DDR_BUF49_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf49_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF49_END;
/* Define the union U_DDR_BUF50_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf50_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF50_END;
/* Define the union U_DDR_BUF51_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf51_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF51_END;
/* Define the union U_DDR_BUF52_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf52_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF52_END;
/* Define the union U_DDR_BUF53_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf53_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF53_END;
/* Define the union U_DDR_BUF54_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf54_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF54_END;
/* Define the union U_DDR_BUF55_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf55_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF55_END;
/* Define the union U_DDR_BUF56_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf56_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF56_END;
/* Define the union U_DDR_BUF57_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf57_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF57_END;
/* Define the union U_DDR_BUF58_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf58_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF58_END;
/* Define the union U_DDR_BUF59_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf59_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF59_END;
/* Define the union U_DDR_BUF60_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf60_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF60_END;
/* Define the union U_DDR_BUF61_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf61_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF61_END;
/* Define the union U_DDR_BUF62_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf62_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF62_END;
/* Define the union U_DDR_BUF63_END */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ddr_buf63_end          : 32 ; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF63_END;
/* Define the union U_DDR_BUF_INFO0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ddr_buf0_info         : 2  ; /* [1..0]  */
        unsigned int    ddr_buf1_info         : 2  ; /* [3..2]  */
        unsigned int    ddr_buf2_info         : 2  ; /* [5..4]  */
        unsigned int    ddr_buf3_info         : 2  ; /* [7..6]  */
        unsigned int    ddr_buf4_info         : 2  ; /* [9..8]  */
        unsigned int    ddr_buf5_info         : 2  ; /* [11..10]  */
        unsigned int    ddr_buf6_info         : 2  ; /* [13..12]  */
        unsigned int    ddr_buf7_info         : 2  ; /* [15..14]  */
        unsigned int    ddr_buf8_info         : 2  ; /* [17..16]  */
        unsigned int    ddr_buf9_info         : 2  ; /* [19..18]  */
        unsigned int    ddr_buf10_info        : 2  ; /* [21..20]  */
        unsigned int    ddr_buf11_info        : 2  ; /* [23..22]  */
        unsigned int    ddr_buf12_info        : 2  ; /* [25..24]  */
        unsigned int    ddr_buf13_info        : 2  ; /* [27..26]  */
        unsigned int    ddr_buf14_info        : 2  ; /* [29..28]  */
        unsigned int    ddr_buf15_info        : 2  ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF_INFO0;

/* Define the union U_DDR_BUF_INFO1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ddr_buf16_info        : 2  ; /* [1..0]  */
        unsigned int    ddr_buf17_info        : 2  ; /* [3..2]  */
        unsigned int    ddr_buf18_info        : 2  ; /* [5..4]  */
        unsigned int    ddr_buf19_info        : 2  ; /* [7..6]  */
        unsigned int    ddr_buf20_info        : 2  ; /* [9..8]  */
        unsigned int    ddr_buf21_info        : 2  ; /* [11..10]  */
        unsigned int    ddr_buf22_info        : 2  ; /* [13..12]  */
        unsigned int    ddr_buf23_info        : 2  ; /* [15..14]  */
        unsigned int    ddr_buf24_info        : 2  ; /* [17..16]  */
        unsigned int    ddr_buf25_info        : 2  ; /* [19..18]  */
        unsigned int    ddr_buf26_info        : 2  ; /* [21..20]  */
        unsigned int    ddr_buf27_info        : 2  ; /* [23..22]  */
        unsigned int    ddr_buf28_info        : 2  ; /* [25..24]  */
        unsigned int    ddr_buf29_info        : 2  ; /* [27..26]  */
        unsigned int    ddr_buf30_info        : 2  ; /* [29..28]  */
        unsigned int    ddr_buf31_info        : 2  ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF_INFO1;

/* Define the union U_DDR_BUF_INFO2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ddr_buf32_info        : 2  ; /* [1..0]  */
        unsigned int    ddr_buf33_info        : 2  ; /* [3..2]  */
        unsigned int    ddr_buf34_info        : 2  ; /* [5..4]  */
        unsigned int    ddr_buf35_info        : 2  ; /* [7..6]  */
        unsigned int    ddr_buf36_info        : 2  ; /* [9..8]  */
        unsigned int    ddr_buf37_info        : 2  ; /* [11..10]  */
        unsigned int    ddr_buf38_info        : 2  ; /* [13..12]  */
        unsigned int    ddr_buf39_info        : 2  ; /* [15..14]  */
        unsigned int    ddr_buf40_info        : 2  ; /* [17..16]  */
        unsigned int    ddr_buf41_info        : 2  ; /* [19..18]  */
        unsigned int    ddr_buf42_info        : 2  ; /* [21..20]  */
        unsigned int    ddr_buf43_info        : 2  ; /* [23..22]  */
        unsigned int    ddr_buf44_info        : 2  ; /* [25..24]  */
        unsigned int    ddr_buf45_info        : 2  ; /* [27..26]  */
        unsigned int    ddr_buf46_info        : 2  ; /* [29..28]  */
        unsigned int    ddr_buf47_info        : 2  ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF_INFO2;

/* Define the union U_DDR_BUF_INFO3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ddr_buf48_info        : 2  ; /* [1..0]  */
        unsigned int    ddr_buf49_info        : 2  ; /* [3..2]  */
        unsigned int    ddr_buf50_info        : 2  ; /* [5..4]  */
        unsigned int    ddr_buf51_info        : 2  ; /* [7..6]  */
        unsigned int    ddr_buf52_info        : 2  ; /* [9..8]  */
        unsigned int    ddr_buf53_info        : 2  ; /* [11..10]  */
        unsigned int    ddr_buf54_info        : 2  ; /* [13..12]  */
        unsigned int    ddr_buf55_info        : 2  ; /* [15..14]  */
        unsigned int    ddr_buf56_info        : 2  ; /* [17..16]  */
        unsigned int    ddr_buf57_info        : 2  ; /* [19..18]  */
        unsigned int    ddr_buf58_info        : 2  ; /* [21..20]  */
        unsigned int    ddr_buf59_info        : 2  ; /* [23..22]  */
        unsigned int    ddr_buf60_info        : 2  ; /* [25..24]  */
        unsigned int    ddr_buf61_info        : 2  ; /* [27..26]  */
        unsigned int    ddr_buf62_info        : 2  ; /* [29..28]  */
        unsigned int    ddr_buf63_info        : 2  ; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DDR_BUF_INFO3;

/* Define the union U_DSP_WORK_ON */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dsp_work_on           : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DSP_WORK_ON;

/* Define the global struct */
typedef struct {
    U_INTR_RAW_SECUE_REGS   INTR_RAW_SECUE_REGS             ; /* 0x0 */
    U_INTR_SECUE_MSK_REGS   INTR_SECUE_MSK_REGS             ; /* 0x4 */
    U_INTR_SECUE_STATUS_REGS   INTR_SECUE_STATUS_REGS       ; /* 0x8 */
    U_PFAULTINFO           PFAULTINFO                       ; /* 0xc */
    U_TCM_AW_ADDR          TCM_AW_ADDR                      ; /* 0x10 */
    U_TCM_AR_ADDR          TCM_AR_ADDR                      ; /* 0x14 */
    U_DSP_STATUS           DSP_STATUS                       ; /* 0x18 */
    U_TCM_AW_INT_CLR       TCM_AW_INT_CLR                   ; /* 0x1c */
    U_TCM_AR_INT_CLR       TCM_AR_INT_CLR                   ; /* 0x20 */
    U_SLV_AW_INT_CLR       SLV_AW_INT_CLR                   ; /* 0x24 */
    U_SLV_AR_INT_CLR       SLV_AR_INT_CLR                   ; /* 0x28 */
    U_DOUBLE_EXC_INT_CLR   DOUBLE_EXC_INT_CLR               ; /* 0x2c */
    U_PFATAL_INT_CLR       PFATAL_INT_CLR                   ; /* 0x30 */
    U_MMU_CTRL             MMU_CTRL                         ; /* 0x34 */
    U_MEMORY_CTRL          MEMORY_CTRL                      ; /* 0x38 */
    U_TCM_BUF0_START_ADDR0   TCM_BUF0_START_ADDR0           ; /* 0x3c */
    U_TCM_BUF0_END_ADDR0   TCM_BUF0_END_ADDR0               ; /* 0x40 */
    U_TCM_BUF1_START_ADDR0   TCM_BUF1_START_ADDR0           ; /* 0x44 */
    U_TCM_BUF1_END_ADDR0   TCM_BUF1_END_ADDR0               ; /* 0x48 */
    U_TCM_BUF2_START_ADDR0   TCM_BUF2_START_ADDR0           ; /* 0x4c */
    U_TCM_BUF2_END_ADDR0   TCM_BUF2_END_ADDR0               ; /* 0x50 */
    U_TCM_BUF3_START_ADDR0   TCM_BUF3_START_ADDR0           ; /* 0x54 */
    U_TCM_BUF3_END_ADDR0   TCM_BUF3_END_ADDR0               ; /* 0x58 */
    U_TCM_BUF_INFO         TCM_BUF_INFO                     ; /* 0x5c */
    U_DSP_CTRL             DSP_CTRL                         ; /* 0x60 */
    U_DDR_BUF0_START       DDR_BUF0_START                   ; /* 0x64 */
    U_DDR_BUF1_START       DDR_BUF1_START                   ; /* 0x68 */
    U_DDR_BUF2_START       DDR_BUF2_START                   ; /* 0x6c */
    U_DDR_BUF3_START       DDR_BUF3_START                   ; /* 0x70 */
    U_DDR_BUF4_START       DDR_BUF4_START                   ; /* 0x74 */
    U_DDR_BUF5_START       DDR_BUF5_START                   ; /* 0x78 */
    U_DDR_BUF6_START       DDR_BUF6_START                   ; /* 0x7c */
    U_DDR_BUF7_START       DDR_BUF7_START                   ; /* 0x80 */
    U_DDR_BUF8_START       DDR_BUF8_START                   ; /* 0x84 */
    U_DDR_BUF9_START       DDR_BUF9_START                   ; /* 0x88 */
    U_DDR_BUF10_START      DDR_BUF10_START                  ; /* 0x8c */
    U_DDR_BUF11_START      DDR_BUF11_START                  ; /* 0x90 */
    U_DDR_BUF12_START      DDR_BUF12_START                  ; /* 0x94 */
    U_DDR_BUF13_START      DDR_BUF13_START                  ; /* 0x98 */
    U_DDR_BUF14_START      DDR_BUF14_START                  ; /* 0x9c */
    U_DDR_BUF15_START      DDR_BUF15_START                  ; /* 0xa0 */
    U_DDR_BUF16_START      DDR_BUF16_START                  ; /* 0xa4 */
    U_DDR_BUF17_START      DDR_BUF17_START                  ; /* 0xa8 */
    U_DDR_BUF18_START      DDR_BUF18_START                  ; /* 0xac */
    U_DDR_BUF19_START      DDR_BUF19_START                  ; /* 0xb0 */
    U_DDR_BUF20_START      DDR_BUF20_START                  ; /* 0xb4 */
    U_DDR_BUF21_START      DDR_BUF21_START                  ; /* 0xb8 */
    U_DDR_BUF22_START      DDR_BUF22_START                  ; /* 0xbc */
    U_DDR_BUF23_START      DDR_BUF23_START                  ; /* 0xc0 */
    U_DDR_BUF24_START      DDR_BUF24_START                  ; /* 0xc4 */
    U_DDR_BUF25_START      DDR_BUF25_START                  ; /* 0xc8 */
    U_DDR_BUF26_START      DDR_BUF26_START                  ; /* 0xcc */
    U_DDR_BUF27_START      DDR_BUF27_START                  ; /* 0xd0 */
    U_DDR_BUF28_START      DDR_BUF28_START                  ; /* 0xd4 */
    U_DDR_BUF29_START      DDR_BUF29_START                  ; /* 0xd8 */
    U_DDR_BUF30_START      DDR_BUF30_START                  ; /* 0xdc */
    U_DDR_BUF31_START      DDR_BUF31_START                  ; /* 0xe0 */
    U_DDR_BUF32_START      DDR_BUF32_START                  ; /* 0xe4 */
    U_DDR_BUF33_START      DDR_BUF33_START                  ; /* 0xe8 */
    U_DDR_BUF34_START      DDR_BUF34_START                  ; /* 0xec */
    U_DDR_BUF35_START      DDR_BUF35_START                  ; /* 0xf0 */
    U_DDR_BUF36_START      DDR_BUF36_START                  ; /* 0xf4 */
    U_DDR_BUF37_START      DDR_BUF37_START                  ; /* 0xf8 */
    U_DDR_BUF38_START      DDR_BUF38_START                  ; /* 0xfc */
    U_DDR_BUF39_START      DDR_BUF39_START                  ; /* 0x100 */
    U_DDR_BUF40_START      DDR_BUF40_START                  ; /* 0x104 */
    U_DDR_BUF41_START      DDR_BUF41_START                  ; /* 0x108 */
    U_DDR_BUF42_START      DDR_BUF42_START                  ; /* 0x10c */
    U_DDR_BUF43_START      DDR_BUF43_START                  ; /* 0x110 */
    U_DDR_BUF44_START      DDR_BUF44_START                  ; /* 0x114 */
    U_DDR_BUF45_START      DDR_BUF45_START                  ; /* 0x118 */
    U_DDR_BUF46_START      DDR_BUF46_START                  ; /* 0x11c */
    U_DDR_BUF47_START      DDR_BUF47_START                  ; /* 0x120 */
    U_DDR_BUF48_START      DDR_BUF48_START                  ; /* 0x124 */
    U_DDR_BUF49_START      DDR_BUF49_START                  ; /* 0x128 */
    U_DDR_BUF50_START      DDR_BUF50_START                  ; /* 0x12c */
    U_DDR_BUF51_START      DDR_BUF51_START                  ; /* 0x130 */
    U_DDR_BUF52_START      DDR_BUF52_START                  ; /* 0x134 */
    U_DDR_BUF53_START      DDR_BUF53_START                  ; /* 0x138 */
    U_DDR_BUF54_START      DDR_BUF54_START                  ; /* 0x13c */
    U_DDR_BUF55_START      DDR_BUF55_START                  ; /* 0x140 */
    U_DDR_BUF56_START      DDR_BUF56_START                  ; /* 0x144 */
    U_DDR_BUF57_START      DDR_BUF57_START                  ; /* 0x148 */
    U_DDR_BUF58_START      DDR_BUF58_START                  ; /* 0x14c */
    U_DDR_BUF59_START      DDR_BUF59_START                  ; /* 0x150 */
    U_DDR_BUF60_START      DDR_BUF60_START                  ; /* 0x154 */
    U_DDR_BUF61_START      DDR_BUF61_START                  ; /* 0x158 */
    U_DDR_BUF62_START      DDR_BUF62_START                  ; /* 0x15c */
    U_DDR_BUF63_START      DDR_BUF63_START                  ; /* 0x160 */
    U_DDR_BUF0_END         DDR_BUF0_END                     ; /* 0x164 */
    U_DDR_BUF1_END         DDR_BUF1_END                     ; /* 0x168 */
    U_DDR_BUF2_END         DDR_BUF2_END                     ; /* 0x16c */
    U_DDR_BUF3_END         DDR_BUF3_END                     ; /* 0x170 */
    U_DDR_BUF4_END         DDR_BUF4_END                     ; /* 0x174 */
    U_DDR_BUF5_END         DDR_BUF5_END                     ; /* 0x178 */
    U_DDR_BUF6_END         DDR_BUF6_END                     ; /* 0x17c */
    U_DDR_BUF7_END         DDR_BUF7_END                     ; /* 0x180 */
    U_DDR_BUF8_END         DDR_BUF8_END                     ; /* 0x184 */
    U_DDR_BUF9_END         DDR_BUF9_END                     ; /* 0x188 */
    U_DDR_BUF10_END        DDR_BUF10_END                    ; /* 0x18c */
    U_DDR_BUF11_END        DDR_BUF11_END                    ; /* 0x190 */
    U_DDR_BUF12_END        DDR_BUF12_END                    ; /* 0x194 */
    U_DDR_BUF13_END        DDR_BUF13_END                    ; /* 0x198 */
    U_DDR_BUF14_END        DDR_BUF14_END                    ; /* 0x19c */
    U_DDR_BUF15_END        DDR_BUF15_END                    ; /* 0x1a0 */
    U_DDR_BUF16_END        DDR_BUF16_END                    ; /* 0x1a4 */
    U_DDR_BUF17_END        DDR_BUF17_END                    ; /* 0x1a8 */
    U_DDR_BUF18_END        DDR_BUF18_END                    ; /* 0x1ac */
    U_DDR_BUF19_END        DDR_BUF19_END                    ; /* 0x1b0 */
    U_DDR_BUF20_END        DDR_BUF20_END                    ; /* 0x1b4 */
    U_DDR_BUF21_END        DDR_BUF21_END                    ; /* 0x1b8 */
    U_DDR_BUF22_END        DDR_BUF22_END                    ; /* 0x1bc */
    U_DDR_BUF23_END        DDR_BUF23_END                    ; /* 0x1c0 */
    U_DDR_BUF24_END        DDR_BUF24_END                    ; /* 0x1c4 */
    U_DDR_BUF25_END        DDR_BUF25_END                    ; /* 0x1c8 */
    U_DDR_BUF26_END        DDR_BUF26_END                    ; /* 0x1cc */
    U_DDR_BUF27_END        DDR_BUF27_END                    ; /* 0x1d0 */
    U_DDR_BUF28_END        DDR_BUF28_END                    ; /* 0x1d4 */
    U_DDR_BUF29_END        DDR_BUF29_END                    ; /* 0x1d8 */
    U_DDR_BUF30_END        DDR_BUF30_END                    ; /* 0x1dc */
    U_DDR_BUF31_END        DDR_BUF31_END                    ; /* 0x1e0 */
    U_DDR_BUF32_END        DDR_BUF32_END                    ; /* 0x1e4 */
    U_DDR_BUF33_END        DDR_BUF33_END                    ; /* 0x1e8 */
    U_DDR_BUF34_END        DDR_BUF34_END                    ; /* 0x1ec */
    U_DDR_BUF35_END        DDR_BUF35_END                    ; /* 0x1f0 */
    U_DDR_BUF36_END        DDR_BUF36_END                    ; /* 0x1f4 */
    U_DDR_BUF37_END        DDR_BUF37_END                    ; /* 0x1f8 */
    U_DDR_BUF38_END        DDR_BUF38_END                    ; /* 0x1fc */
    U_DDR_BUF39_END        DDR_BUF39_END                    ; /* 0x200 */
    U_DDR_BUF40_END        DDR_BUF40_END                    ; /* 0x204 */
    U_DDR_BUF41_END        DDR_BUF41_END                    ; /* 0x208 */
    U_DDR_BUF42_END        DDR_BUF42_END                    ; /* 0x20c */
    U_DDR_BUF43_END        DDR_BUF43_END                    ; /* 0x210 */
    U_DDR_BUF44_END        DDR_BUF44_END                    ; /* 0x214 */
    U_DDR_BUF45_END        DDR_BUF45_END                    ; /* 0x218 */
    U_DDR_BUF46_END        DDR_BUF46_END                    ; /* 0x21c */
    U_DDR_BUF47_END        DDR_BUF47_END                    ; /* 0x220 */
    U_DDR_BUF48_END        DDR_BUF48_END                    ; /* 0x224 */
    U_DDR_BUF49_END        DDR_BUF49_END                    ; /* 0x228 */
    U_DDR_BUF50_END        DDR_BUF50_END                    ; /* 0x22c */
    U_DDR_BUF51_END        DDR_BUF51_END                    ; /* 0x230 */
    U_DDR_BUF52_END        DDR_BUF52_END                    ; /* 0x234 */
    U_DDR_BUF53_END        DDR_BUF53_END                    ; /* 0x238 */
    U_DDR_BUF54_END        DDR_BUF54_END                    ; /* 0x23c */
    U_DDR_BUF55_END        DDR_BUF55_END                    ; /* 0x240 */
    U_DDR_BUF56_END        DDR_BUF56_END                    ; /* 0x244 */
    U_DDR_BUF57_END        DDR_BUF57_END                    ; /* 0x248 */
    U_DDR_BUF58_END        DDR_BUF58_END                    ; /* 0x24c */
    U_DDR_BUF59_END        DDR_BUF59_END                    ; /* 0x250 */
    U_DDR_BUF60_END        DDR_BUF60_END                    ; /* 0x254 */
    U_DDR_BUF61_END        DDR_BUF61_END                    ; /* 0x258 */
    U_DDR_BUF62_END        DDR_BUF62_END                    ; /* 0x25c */
    U_DDR_BUF63_END        DDR_BUF63_END                    ; /* 0x260 */
    U_DDR_BUF_INFO0        DDR_BUF_INFO0                    ; /* 0x264 */
    U_DDR_BUF_INFO1        DDR_BUF_INFO1                    ; /* 0x268 */
    U_DDR_BUF_INFO2        DDR_BUF_INFO2                    ; /* 0x26c */
    U_DDR_BUF_INFO3        DDR_BUF_INFO3                    ; /* 0x270 */
    U_DSP_WORK_ON          DSP_WORK_ON                      ; /* 0x274 */
} S_DSP_REGS_TYPE;

/* Define the union U_INTR_RAW_UNSECUE_REGS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dsp2cpu_unsecue_int   : 1  ; /* [0]  */
        unsigned int    dsp2dsp_o_int         : 1  ; /* [1]  */
        unsigned int    unsecue_sw_com_int    : 4  ; /* [5..2]  */
        unsigned int    reserved_0            : 26 ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INTR_RAW_UNSECUE_REGS;

/* Define the union U_INTR_MSK_UNSECUE_REGS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mmu_unsecue_int_mask  : 1  ; /* [0]  */
        unsigned int    dsp2cpu_unsecue_int_mask : 1  ; /* [1]  */
        unsigned int    unsecue_sw_com_int_mask : 4  ; /* [5..2]  */
        unsigned int    reserved_0            : 26 ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INTR_MSK_UNSECUE_REGS;

/* Define the union U_INTR_STATUS_UNSECUE_REGS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dsp2cpu_unsecue_int_sta : 1  ; /* [0]  */
        unsigned int    mmu_unsecue_int_sta   : 1  ; /* [1]  */
        unsigned int    dsp2dsp_o_int_sta     : 1  ; /* [2]  */
        unsigned int    unsecue_sw_com_int_sta : 4  ; /* [6..3]  */
        unsigned int    reserved_0            : 25 ; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INTR_STATUS_UNSECUE_REGS;

/* Define the union U_DSP_WAITI_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    waiti                 : 1  ; /* [0]  */
        unsigned int    dsp_work_on_state     : 1  ; /* [1]  */
        unsigned int    reserved_0            : 30 ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DSP_WAITI_STATUS;

/* Define the union U_DSP_POWER_ON */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dsp_power_on          : 1  ; /* [0]  */
        unsigned int    reserved_0            : 31 ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DSP_POWER_ON;

/* Define the global struct */
typedef struct {
    U_INTR_RAW_UNSECUE_REGS   INTR_RAW_UNSECUE_REGS         ; /* 0x0 */
    U_INTR_MSK_UNSECUE_REGS   INTR_MSK_UNSECUE_REGS         ; /* 0x4 */
    U_INTR_STATUS_UNSECUE_REGS   INTR_STATUS_UNSECUE_REGS   ; /* 0x8 */
    U_DSP_WAITI_STATUS     DSP_WAITI_STATUS                 ; /* 0xc */
    U_DSP_POWER_ON         DSP_POWER_ON                     ; /* 0x10 */
} S_DSP_UNSECUE_REGS_TYPE;

#endif /* __HI_REG_DSP_H__ */

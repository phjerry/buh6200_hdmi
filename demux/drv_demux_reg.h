/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux register definition.
 * Author: sdk
 * Create: 2017-06-05
 */

#ifndef __DRV_DEMUX_REG_H__
#define __DRV_DEMUX_REG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define BITS_PER_REG                  32
#define DMX_CHAN_CLEAR_TIMEOUT_CNT    100

#define DMX_READ_REG(base, offset)  ioread32((void *)((base) + (offset)))
#define DMX_WRITE_REG(base, offset, value)   iowrite32((value), (void*)((base) + (offset)))
#ifdef DMX_REG_DEBUG
#define DMX_COM_EQUAL(exp, act)                                                         \
    do {                                                                                \
        if ((exp) != (act)) {                                                           \
            HI_ERR_DEMUX("write register error, exp=0x%x, act=0x%x\n", (exp), (act));   \
        }                                                                               \
    } while (0)
#else
#define DMX_COM_EQUAL(exp, act)
#endif

#define DMX_READ_REG_SUB(base, subbase, offset)  ioread32((void *)(base + subbase + offset))
#define DMX_WRITE_REG_SUB(base, subbase, offset, value)   iowrite32(value, (void*)(base + subbase + offset))
/******************************************************************************/
/* hi_demux_v200 DMX PAR register definition  begin */
/******************************************************************************/
#define DMX_READ_REG_PAR(base, offset)  ioread32((void *)((base) + DMX_REGS_PAR_BASE + (offset)))
#define DMX_WRITE_REG_PAR(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_PAR_BASE + (offset)))

/* Define the union U_PAR_INT_TYPE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    par_tei_int           : 1   ; /* [0] */
        unsigned int    reserved_0            : 15  ; /* [15..1] */
        unsigned int    par_cc_int            : 1   ; /* [16] */
        unsigned int    reserved_1            : 7   ; /* [23..17] */
        unsigned int    par_dsc_int           : 1   ; /* [24] */
        unsigned int    reserved_2            : 6   ; /* [30..25] */
        unsigned int    par_cfg_over_int      : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PAR_INT_TYPE;
#define PAR_INT_TYPE   (0xBFFC)  /* PAR interupt type register */

#define IENA0_CC_CHECK  (0xC000) /* CC disc check enable register 0 ~ 31 <RW> */
#define IENA0_TEI_CHECK (0xC008) /* TEI check enable register 0 ~ 31 <RW> */

#define IRAW0_CC_CHECK  (0xC010) /* CC disc check raw status register  0 ~ 31 <WC> */
#define IRAW0_TEI_CHECK (0xC018) /* TEI check raw status register  0 ~ 31 <WC> */

#define ISTA0_CC_CHECK  (0xC020) /* CC disc check status register  0 ~ 31 <RO> */
#define ISTA0_TEI_CHECK (0xC028) /* TEI check status register  0 ~ 31 <RO> */

#define IENA_DSC_CHECK  (0xC050) /* DSC check enable register 0 ~ 31 <RW> */
#define IRAW_DSC_CHECK  (0xC054) /* DSC check raw status register  0 ~ 31 <WC> */
#define ISTA_DSC_CHECK  (0xC058) /* DSC check status register  0 ~ 31 <RO> */

/******************************************************************************/
/* 96cv300 DMX INF register definition  begin */
/******************************************************************************/
#define DMX_READ_REG_INF(base, offset)  ioread32((void *)((base) + DMX_REGS_INF_BASE + (offset)))
#define DMX_WRITE_REG_INF(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_INF_BASE + (offset)))

/* Define the union U_DVB_INF_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_dvb_clk_mode       : 1   ; /* [0] */ /* 0: single edge-triggered, 1: double edge-triggered */
        unsigned int    reserved_0            : 15  ; /* [15..1] */
        unsigned int    sw_out_sync_mode_sel  : 1   ; /* [16] */
        unsigned int    reserved_1            : 3   ; /* [19..17] */
        unsigned int    sw_valid_burst        : 1   ; /* [20] */
        unsigned int    reserved_2            : 3   ; /* [23..21] */
        unsigned int    sync_mode             : 2   ; /* [25..24] */
        unsigned int    reserved_3            : 2   ; /* [27..26] */
        unsigned int    ser_par_sel           : 1   ; /* [28] */
        unsigned int    reserved_4            : 2   ; /* [30..29] */
        unsigned int    port_open             : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_INF_CTRL;
#define DVB_INF_CTRL(dvb_port)                (0x0 + ((dvb_port) << 8))

/* Define the union U_DVB_SERIAL_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ser_bit0_sel          : 3   ; /* [2..0] */
        unsigned int    reserved_0            : 1   ; /* [3] */
        unsigned int    ser_bit1_sel          : 3   ; /* [6..4] */
        unsigned int    reserved_1            : 1   ; /* [7] */
        unsigned int    sw_ser_msb_sel        : 1   ; /* [8] */
        unsigned int    reserved_2            : 3   ; /* [11..9] */
        unsigned int    ser_bit_mode          : 1   ; /* [12] */
        unsigned int    reserved_3            : 3   ; /* [15..13] */
        unsigned int    ser_nosync_clrth      : 4   ; /* [19..16] */
        unsigned int    reserved_4            : 4   ; /* [23..20] */
        unsigned int    unify_ser_len_bypass  : 1   ; /* [24] */
        unsigned int    reserved_5            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_SERIAL_CTRL;
#define DVB_SERIAL_CTRL(dvb_port)             (0x4 + ((dvb_port) << 8))

/* Define the union U_DVB_PARALLEL_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    parallel_msb_sel      : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_PARALLEL_CTRL;
#define DVB_PARALLEL_CTRL(dvb_port)          (0x8 + ((dvb_port) << 8))

/* Define the union U_DVB_INF_SUB_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sync_on_th            : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 1   ; /* [3]  */
        unsigned int    sync_off_th           : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    fifo_rate             : 3   ; /* [10..8]  */
        unsigned int    reserved_2            : 4   ; /* [14..11]  */
        unsigned int    fifo_rate_en          : 1   ; /* [15]  */
        unsigned int    ser_sync_bit          : 1   ; /* [16]  */
        unsigned int    ser_sync_bit_mode     : 1   ; /* [17]  */
        unsigned int    reserved_3            : 2   ; /* [19..18]  */
        unsigned int    sync_pol              : 1   ; /* [20]  */
        unsigned int    vld_pol               : 1   ; /* [21]  */
        unsigned int    reserved_4            : 2   ; /* [23..22]  */
        unsigned int    sw_47_replace         : 1   ; /* [24]  */
        unsigned int    reserved_5            : 3   ; /* [27..25]  */
        unsigned int    valid_sync_chg        : 1   ; /* [28]  */
        unsigned int    reserved_6            : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_INF_SUB_CTRL;
#define DVB_INF_SUB_CTRL(dvb_port)           (0xC + ((dvb_port) << 8))

/* Define the union U_DVB_SYNC_CTRL0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sync_byte0            : 8   ; /* [7..0]  */
        unsigned int    sync_byte1            : 8   ; /* [15..8]  */
        unsigned int    nosync_188_len        : 8   ; /* [23..16]  */
        unsigned int    nosync_204_len        : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_SYNC_CTRL0;
#define DVB_SYNC_CTRL0(dvb_port)            (0x10 + ((dvb_port) << 8))

/* Define the union U_DVB_SYNC_CTRL1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_sync_out_nolimit   : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    tsp_port_align        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    sync_204_success      : 1   ; /* [8]  */
        unsigned int    sync_188_success      : 1   ; /* [9]  */
        unsigned int    reserved_2            : 6   ; /* [15..10]  */
        unsigned int    sw_noworking          : 1   ; /* [16]  */
        unsigned int    reserved_3            : 15  ; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_SYNC_CTRL1;
#define DVB_SYNC_CTRL1(dvb_port)            (0x14 + ((dvb_port) << 8))

/* Define the union U_DVB_FORCE_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    force_dvb_s           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 1   ; /* [1]  */
        unsigned int    force_dvb_t           : 1   ; /* [2]  */
        unsigned int    reserved_1            : 1   ; /* [3]  */
        unsigned int    force_dvb_c           : 1   ; /* [4]  */
        unsigned int    reserved_2            : 1   ; /* [5]  */
        unsigned int    force_dvb_x           : 1   ; /* [6]  */
        unsigned int    reserved_3            : 1   ; /* [7]  */
        unsigned int    sync_head_x           : 8   ; /* [15..8]  */
        unsigned int    reserved_4            : 8   ; /* [23..16]  */
        unsigned int    detect_err_th         : 3   ; /* [26..24]  */
        unsigned int    reserved_5            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_FORCE_CTRL;
#define DVB_FORCE_CTRL(dvb_port)            (0x18 + ((dvb_port) << 8))

/* Define the union U_DVB_DUMMY_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_ser_allsave        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    sw_dummy_force        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    sw_dummy_sync         : 1   ; /* [8]  */
        unsigned int    reserved_2            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DVB_DUMMY_CTRL;
#define DVB_DUMMY_CTRL(dvb_port)            (0x1C + ((dvb_port) << 8))

/* Define the union U_INF_CLK_GT_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    inf_unify_clk_gt_en   : 1   ; /* [0]  */
        unsigned int    inf_sync_clk_gt_en    : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    dbg_unify_clk_gt_state : 1   ; /* [8]  */
        unsigned int    dbg_sync_clk_gt_state : 1   ; /* [9]  */
        unsigned int    reserved_1            : 22  ; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INF_CLK_GT_CTRL;
#define INF_CLK_GT_CTRL(dvb_port)            (0x30 + ((dvb_port) << 8))

/* Define the union U_TS_COUNT_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ts_count_ctrl         : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TS_COUNT_CTRL;
#define TS_COUNT_CTRL(dvb_port)             (0x40 + ((dvb_port) << 8))
#define TS_COUNT(dvb_port)                  (0x44 + ((dvb_port) << 8))
#define ETS_COUNT(dvb_port)                 (0x48 + ((dvb_port) << 8))

/* Define the union U_TS_AFIFO_WFULL_STATUS */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wfull_dmx             : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    wfull_err             : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    wfull_dmx_cnt         : 4   ; /* [11..8]  */
        unsigned int    reserved_2            : 1   ; /* [12]  */
        unsigned int    detect_err_cnt        : 3   ; /* [15..13]  */
        unsigned int    sync_err_cnt          : 8   ; /* [23..16]  */
        unsigned int    force_sync_err_cnt    : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TS_AFIFO_WFULL_STATUS;
#define TS_AFIFO_WFULL_STATUS(dvb_port)      (0x50 + ((dvb_port) << 8))

/* Define the union U_TS_READ_FIFO_FSM */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 4   ; /* [3..0]  */
        unsigned int    fifo_raddr            : 3   ; /* [6..4]  */
        unsigned int    reserved_1            : 1   ; /* [7]  */
        unsigned int    fifo_waddr            : 3   ; /* [10..8]  */
        unsigned int    reserved_2            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TS_READ_FIFO_FSM;

/* Define the union U_TS_SYNC_FSM */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fsm_sync204           : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 4   ; /* [7..4]  */
        unsigned int    fsm_sync188           : 4   ; /* [11..8]  */
        unsigned int    reserved_1            : 12  ; /* [23..12]  */
        unsigned int    period_sync_on        : 1   ; /* [24]  */
        unsigned int    reserved_2            : 3   ; /* [27..25]  */
        unsigned int    period_sync_off       : 1   ; /* [28]  */
        unsigned int    reserved_3            : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TS_SYNC_FSM;

/* Define the union U_DBG_PID_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_pid_detect_clr     : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_PID_CTRL;

/* Define the union U_DBG_PID_CNT */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_pid_value         : 13  ; /* [12..0]  */
        unsigned int    reserved_0            : 3   ; /* [15..13]  */
        unsigned int    dbg_pid_vld           : 1   ; /* [16]  */
        unsigned int    reserved_1            : 7   ; /* [23..17]  */
        unsigned int    dbg_pid_cnt           : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_PID_CNT;

/* Define the union U_DBG_SYNC_LEN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_nosync_188_len    : 8   ; /* [7..0]  */
        unsigned int    dbg_nosync_204_len    : 8   ; /* [15..8]  */
        unsigned int    dbg_sw_sync_out_nolimit : 1   ; /* [16]  */
        unsigned int    reserved_0            : 3   ; /* [19..17]  */
        unsigned int    dng_sw_out_sync_mode_sel : 1   ; /* [20]  */
        unsigned int    reserved_1            : 11  ; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_SYNC_LEN;

/* Define the union U_DBG_SYNC_CLR_TH */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_sw_ser_nosync_clrth : 4   ; /* [3..0]  */
        unsigned int    dbg_sw_tsp_port_align : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_SYNC_CLR_TH;

/* Define the union U_DBG_SYNC_BYTE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_sw_sync_byte0     : 8   ; /* [7..0]  */
        unsigned int    dbg_sw_sync_byte1     : 8   ; /* [15..8]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_SYNC_BYTE;

/* Define the union U_DBG_DETECT_EN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sw_dbg_freq_en        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_DETECT_EN;

/* Define the union U_DBG_UNIFY_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_c_s_cntb          : 8   ; /* [7..0]  */
        unsigned int    dbg_c_s_cntp          : 4   ; /* [11..8]  */
        unsigned int    dbg_cnt_clr           : 4   ; /* [15..12]  */
        unsigned int    dbg_ser_in_valid_cnt  : 5   ; /* [20..16]  */
        unsigned int    reserved_0            : 2   ; /* [22..21]  */
        unsigned int    dvb_i_d1_sync_passed  : 1   ; /* [23]  */
        unsigned int    dbg_serial_nosync_fail : 1   ; /* [24]  */
        unsigned int    reserved_1            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_UNIFY_0;

/* Define the union U_DBG_UNIFY_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    c_port_mode           : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 8   ; /* [23..16]  */
        unsigned int    dbg_cnt_csync         : 4   ; /* [27..24]  */
        unsigned int    dbg_cnt_cvalid        : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_UNIFY_1;

/* Define the union U_DBG_UNIFY_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_cdata_d1          : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 1   ; /* [8]  */
        unsigned int    dbg_cvalid_d1         : 1   ; /* [9]  */
        unsigned int    dbg_csync_d1          : 1   ; /* [10]  */
        unsigned int    reserved_1            : 6   ; /* [16..11]  */
        unsigned int    dbg_cdata             : 8   ; /* [24..17]  */
        unsigned int    dbg_cvalid            : 1   ; /* [25]  */
        unsigned int    dbg_csync             : 1   ; /* [26]  */
        unsigned int    reserved_2            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_UNIFY_2;

/* Define the union U_DBG_UNIFY_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_s2p_count         : 8   ; /* [7..0]  */
        unsigned int    dbg_cnt_188           : 8   ; /* [15..8]  */
        unsigned int    dbg_sft_bit           : 4   ; /* [19..16]  */
        unsigned int    dbg_sft_vld           : 4   ; /* [23..20]  */
        unsigned int    dbg_sft_sync          : 4   ; /* [27..24]  */
        unsigned int    s2pfsm_curr_state     : 4   ; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_UNIFY_3;

/* Define the union U_IRAW_INF */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    iraw_sync_on          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    iraw_sync_off         : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_IRAW_INF;

/* Define the union U_ISTA_INF */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ista_sync_on          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    ista_sync_off         : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_ISTA_INF;

/* Define the union U_IENA_INF */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    iena_sync_on          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    iena_sync_off         : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_IENA_INF;

/* Define the union U_IRAW_INF_TOP */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    iraw_inf_cfg_overflow : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_IRAW_INF_TOP;

/* Define the union U_ISTA_INF_TOP */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ista_inf_cfg_overflow : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_ISTA_INF_TOP;

/* Define the union U_IENA_INF_TOP */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    iena_inf_cfg_overflow : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_IENA_INF_TOP;

/******************************************************************************/
/*                      96cv300 DMX CFG register definition  begin                   */
/******************************************************************************/
#define DMX_READ_REG_CFG(base, offset)  ioread32((void *)((base) + DMX_REGS_CFG_BASE + (offset)))
#define DMX_WRITE_REG_CFG(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_CFG_BASE + (offset)))

/* define the union U_DMX_IS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    inf2cpu_int           : 1   ; /* [0]  */
        unsigned int    swh2cpu_int           : 1   ; /* [1]  */
        unsigned int    par2cpu_int           : 1   ; /* [2]  */
        unsigned int    scd2cpu_int           : 1   ; /* [3]  */
        unsigned int    flt2cpu_int           : 1   ; /* [4]  */
        unsigned int    dav2cpu_int           : 1   ; /* [5]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    dmx_int               : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_IS;
#define DMX_IS_ALL_INT_STATUS           (0x0000)  /* DMX all interrupt status register */

/* define the union U_DMX_IE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    dmx_ie                : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_IE;
#define DMX_IE_ALL_INT_ENABLE          (0x0004)  /* DMX all interrupt enable register */

/******************************************************************************/
/*                      hi_demux_v200 DMX SWH register definition  begin                                   */
/******************************************************************************/
#define DMX_READ_REG_SWH(base, offset)  ioread32((void *)((base) + DMX_REGS_SWH_BASE + (offset)))
#define DMX_WRITE_REG_SWH(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_SWH_BASE + (offset)))

/* define the union U_DMX_BAND_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmx_mux_num           : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 3   ; /* [7..5]  */
        unsigned int    dmx_mux_type          : 3   ; /* [10..8]  */
        unsigned int    reserved_1            : 5   ; /* [15..11]  */
        unsigned int    full_ts_en            : 1   ; /* [16]  */
        unsigned int    full_ts_drop_en       : 1   ; /* [17]  */
        unsigned int    reserved_2            : 6   ; /* [23..18]  */
        unsigned int    clear_pkt_req         : 1   ; /* [24]  */
        unsigned int    reserved_3            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DMX_BAND_CTRL;
#define DMX_BAND_CTRL(band)                (0x0000 + ((band) << 2))   /* band config register */

/* Define the union U_QAM_IN_RATE0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ts_cnt                : 24  ; /* [23..0]  */
        unsigned int    reserved0             : 4   ; /* [27..24]  */
        unsigned int    read_will_empty       : 1   ; /* [28]  */
        unsigned int    read_empty            : 1   ; /* [29]  */
        unsigned int    write_overflow        : 1   ; /* [30]  */
        unsigned int    reserved1             : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_QAM0_STATE;

#define DBG_QAM0_STATE(if_id)              (0x0460 + ((if_id) << 2))   /* band config register */

/* define the union U_DMX_PCR_SCR_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pcr_scr_en            : 1   ; /* [0]  */
        unsigned int    reserved_0            : 15  ; /* [15..1]  */
        unsigned int    scr_div_sel           : 2   ; /* [17..16]  */
        unsigned int    reserved_1            : 5   ; /* [31..18]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_PCR_SCR_CTRL;
#define DMX_PCR_SCR_CTRL                    (0x0500)   /* PCR SCR contrl register */


/* define the union U_DMX_PCR_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pcr_ts_mux_num        : 7   ; /* [6..0]  */
        unsigned int    reserved_0            : 1   ; /* [7]  */
        unsigned int    pcr_ts_mux_type       : 3   ; /* [10..8]  */
        unsigned int    reserved_1            : 4   ; /* [14..11]  */
        unsigned int    pcr_ts_mux_mode       : 1   ; /* [15]  */
        unsigned int    pcr_pid               : 13  ; /* [28..16]  */
        unsigned int    reserved_2            : 3   ; /* [31..29]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_PCR_CTRL;

#define DMX_PCR_CTRL(pcr_id)                 (0x0510 + ((pcr_id) << 5))   /* PCR contrl register */

/* define the union U_DMX_PCR_VALUE0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pcr_base_32           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    pcr_extra_8_0         : 9   ; /* [16..8]  */
        unsigned int    reserved_1            : 15  ; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_PCR_VALUE0;

#define DMX_PCR_VALUE0(pcr_id)               (0x0520 + ((pcr_id) << 5))   /* PCR value0 register */

#define DMX_PCR_VALUE1(pcr_id)               (0x0524 + ((pcr_id) << 5))   /* PCR value1 register */

/* define the union U_DMX_PCR_SCR_VALUE0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pcr_scr_base_32       : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    pcr_scr_extra_8_0     : 9   ; /* [16..8]  */
        unsigned int    reserved_1            : 15  ; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_PCR_SCR_VALUE0;

#define DMX_PCR_SCR_VALUE0(pcr_id)               (0x0528 + ((pcr_id) << 5))   /* PCR SCR value register */

#define DMX_PCR_SCR_VALUE1(pcr_id)               (0x052C + ((pcr_id) << 5))   /* PCR SCR value register */

#define DMX_PCR_SCR_SET1                        (0x0508)   /* low 32bit value of current SCR ,generated by 90K clk */

#define TSN_TAG_LOW_REG(tag_dual_id, tag_id)          (0x1000 + ((tag_dual_id) << 8) + ((tag_id) << 4))
#define TSN_TAG_MID_REG(tag_dual_id, tag_id)          (0x1004 + ((tag_dual_id) << 8) + ((tag_id) << 4))
#define TSN_TAG_HIGH_REG(tag_dual_id, tag_id)         (0x1008 + ((tag_dual_id) << 8) + ((tag_id) << 4))

/* Define the union U_TAG_MUX_CONFIG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tag_length            : 4   ; /* [3..0]  */
        unsigned int    sync_mode_sel         : 1   ; /* [4]  */
        unsigned int    dual_mode             : 1   ; /* [5]  */
        unsigned int    reserved_0            : 25  ; /* [30..6]  */
        unsigned int    mux_work_en           : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TAG_MUX_CONFIG;

#define TAG_MUX_CONFIG(tag_dual_id)                   (0x10F0 + ((tag_dual_id) << 8))   /* tag source register */

/* Define the union U_TAG_MUX_SOURCE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tag_deal_mux0_num     : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 3   ; /* [7..5]  */
        unsigned int    tag_deal_mux0_type    : 3   ; /* [10..8]  */
        unsigned int    reserved_1            : 5   ; /* [15..11]  */
        unsigned int    tag_deal_mux1_num     : 5   ; /* [20..16]  */
        unsigned int    reserved_2            : 3   ; /* [23..21]  */
        unsigned int    tag_deal_mux1_type    : 3   ; /* [26..24]  */
        unsigned int    reserved_3            : 5   ; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TAG_MUX_SOURCE;

#define TAG_MUX_SOURCE(tag_dual_id)                   (0x10F8 + ((tag_dual_id) << 8))   /* tag source register */

/* Define the union U_TAG_CLK_GT_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tag_clk_gt_en         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    dbg_tag_clk_gt_state  : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TAG_CLK_GT_CTRL;

#define TAG_CLK_GT_CTRL(tag_dual_id)                   (0x10FC + ((tag_dual_id) << 8))   /* tag source register */

/* define the union U_RMX_SRC_SEL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    remux_mux_num         : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 3   ; /* [7..5]  */
        unsigned int    remux_mux_type        : 3   ; /* [10..8]  */
        unsigned int    reserved_1            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_RMX_SRC_SEL;

#define RMX_SRC_SEL(id, rmx_scr_port_id)           (0x2000 + (0x500 * id) + (rmx_scr_port_id << 2))

/* define the union U_RMX_ALL_PASS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    sw_rmx_all_pass       : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_RMX_ALL_PASS;

#define RMX_ALL_PARSS(id)                   (0x2140 + (0x500 * id))

/* define the union U_RMX0_FULL_CNT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    dbg_full_err_cnt0     : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 15  ; /* [30..16]  */
        unsigned int    sw_rmx_full_cnten0    : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_RMX0_FULL_CNT;

#define RMX0_FULL_CNT(id, rmx_scr_port_id)       (0x2000 + (0x500 * id) + (rmx_scr_port_id << 2))

/* define the union U_RMX_PID_TABLE0¡«63 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    original_pid          : 13  ; /* [12..0]  */
        unsigned int    remap_source_id       : 3   ; /* [15..13]  */
        unsigned int    pid_remap_pid         : 13  ; /* [28..16]  */
        unsigned int    pidtab_en             : 1   ; /* [29]  */
        unsigned int    pid_remap_mode        : 1   ; /* [30]  */
        unsigned int    pid_del_mode          : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_RMX_PID_TABLE0;

#define RMX_PID_TABLE0(id, pump_id)         (0x2300 + (0x500 * id) + (pump_id << 2))

/* interrupt register of pcr */
#define   IRAW_SWH_PCR_CATCH               (0x3540)    /* raw interrupt regiter of pcr */
#define   ISTA_SWH_PCR_CATCH               (0x3544)    /* interrupt state regiter of pcr */
#define   IENA_SWH_PCR_CATCH               (0x3548)    /* interrupt enable regiter of pcr */

/******************************************************************************/
/*                      hi_demux_v200 DMX FLT register definition  begin                                   */
/******************************************************************************/
#define DMX_READ_REG_FLT(base, offset)  ioread32((void *)((base) + DMX_REGS_FLT_BASE + (offset)))
#define DMX_WRITE_REG_FLT(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_FLT_BASE + (offset)))

/* define the union U_DMX_FILTER */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    wdata_mask            : 8   ; /* [7..0]  */
        unsigned int    wdata_content         : 8   ; /* [15..8]  */
        unsigned int    wdata_mode            : 1   ; /* [16]  */
        unsigned int    reserved_0            : 1   ; /* [17]  */
        unsigned int    dbg_flt_tab_result    : 1   ; /* [18]  */
        unsigned int    reserved_1            : 1   ; /* [19]  */
        unsigned int    filter_id             : 10  ; /* [29..20]  */
        unsigned int    reserved_2            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_FILTER;
#define DMX_FILTER(flt_byte_idx)                (0x0000 + ((flt_byte_idx) << 2))   /* filter config register */

/* define the union U_DMX_PES_SEC_ID */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pes_sec_id            : 9   ; /* [8..0]  */
        unsigned int    reserved_0            : 7   ; /* [15..9]  */
        unsigned int    pes_sec_id_lock       : 1   ; /* [16]  */
        unsigned int    reserved_1            : 15  ; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_PES_SEC_ID;
#define DMX_PES_SEC_ID                          (0x0100)   /* filter pes sec id */

/* define the union U_DMX_FILTER_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    crc_mode              : 3   ; /* [2..0]  */
        unsigned int    reserved_0            : 5   ; /* [7..3]  */
        unsigned int    pes_len_err_drop_dis  : 1   ; /* [8]  */
        unsigned int    reserved_1            : 7   ; /* [15..9]  */
        unsigned int    flt_num               : 5   ; /* [20..16]  */
        unsigned int    reserved_2            : 3   ; /* [23..21]  */
        unsigned int    flt_min               : 4   ; /* [27..24]  */
        unsigned int    reserved_3            : 1   ; /* [28]  */
        unsigned int    no_flt_mode           : 1   ; /* [29]  */
        unsigned int    reserved_4            : 1   ; /* [30]  */
        unsigned int    flt_pes_sec_lock      : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DMX_FILTER_CTRL;

#define DMX_FILTER_CTRL                         (0x0104)   /* filter ctrl register */

/* define the union U_DMX_FILTER_ID */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    fit_id                : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_FILTER_ID;

#define DMX_FILTER_ID(flt_id)                    (0x010C + ((flt_id) << 2))   /* filter id register */


/* define the union U_DMX_FILTER_BUF_ID */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    flt_buf_id            : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_FILTER_BUF_ID;

#define DMX_FILTER_BUF_ID                       (0x018C)   /* filter buffer id */

/* define the union U_DMX_SEC_GLOBAL_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    point_err_mode        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    point_err_deal        : 1   ; /* [4]  */
        unsigned int    reserved_1            : 3   ; /* [7..5]  */
        unsigned int    new_sec_pusi_point    : 1   ; /* [8]  */
        unsigned int    new_sec_pusi_nopint   : 1   ; /* [9]  */
        unsigned int    new_sec_nopusi        : 1   ; /* [10]  */
        unsigned int    reserved_2            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_SEC_GLOBAL_CTRL;

#define DMX_SEC_GLOBAL_CTRL                     (0x0200)   /* section global ctrl register */


/* define the union U_DMX_PES_GLOBAL_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    av_pes_len_err_drop   : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_PES_GLOBAL_CTRL;

#define DMX_PES_GLOBAL_CTRL                  (0x0204)   /* pes global ctrl register */

/* define the union U_DMX_FLT_CLK_GT_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    split_clk_gt_en       : 1   ; /* [0]  */
        unsigned int    sec_clk_gt_en         : 1   ; /* [1]  */
        unsigned int    pes_clk_gt_en         : 1   ; /* [2]  */
        unsigned int    av_clk_gt_en          : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DMX_FLT_CLK_GT_CTRL;

#define DMX_FLT_CLK_GT_CTR                  (0x0500)   /* flt clock ctrl register */

/* Define the union U_DBG_FLT_FSM_RAM_ID */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_flt_fsm_id        : 9   ; /* [8..0]  */
        unsigned int    reserved_0            : 23  ; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_FLT_FSM_RAM_ID;

#define   DBG_FLT_FSM_RAM_ID                  (0x1000)


/* define the union U_CLEAR_FLT_FSM */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    clr_flt_fsm_req       : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_CLEAR_FLT_FSM;

#define CLEAR_FLT_FSM                        (0x1004)   /* clear filter channel require register */

/* Define the union U_DBG_AV_FSM_RAM_ID */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_av_fsm_id         : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_AV_FSM_RAM_ID;

#define   DBG_AV_FSM_RAM_ID                  (0x1020)


/* define the union U_CLEAR_AV_FSM */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    clr_av_fsm_req        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_CLEAR_AV_FSM;
#define CLEAR_AV_FSM                        (0x1024)   /* clear avpes channel require register */


#define DMX_FILTER_EN                           (0x0108)   /* filter enable register */

/******************************************************************************/
/*                      hi_demux_v200 DMX SCD register definition  begin                   */
/******************************************************************************/
#define DMX_READ_REG_SCD(base, offset)  ioread32((void *)((base) + DMX_REGS_SCD_BASE + (offset)))
#define DMX_WRITE_REG_SCD(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_SCD_BASE + (offset)))

/* Define the union U_FLT_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    flt_value_lo          : 8   ; /* [7..0]  */
        unsigned int    flt_value_hi          : 8   ; /* [15..8]  */
        unsigned int    flt_mask              : 8   ; /* [23..16]  */
        unsigned int    flt_inv               : 1   ; /* [24]  */
        unsigned int    reserved_0            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_FLT_CFG;

#define FLT_CFG(flt_id)             (0x0100 + ((flt_id << 2)))

/* Define the union U_TTS_RLD_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tts_scr_rlden         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    tts_scr_mode          : 1   ; /* [4]  */
        unsigned int    tts_27m_en            : 1   ; /* [5]  */
        unsigned int    reserved_1            : 26  ; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TTS_RLD_CFG;

#define TTS_RLD_CFG                (0x0054)

/* Define the union U_TS_REC_FLUSH */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ts_flush_id           : 8   ; /* [7..0]  */
        unsigned int    ts_flush_en           : 1   ; /* [8]  */
        unsigned int    flush_error           : 1   ; /* [9]  */
        unsigned int    reserved_0            : 5   ; /* [14..10]  */
        unsigned int    ts_flush_type         : 1   ; /* [15]  */
        unsigned int    ts_flush_done         : 1   ; /* [16]  */
        unsigned int    reserved_1            : 15  ; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_TS_REC_FLUSH;

#define TS_REC_FLUSH                (0x00F8)

/* define the union U_TS_REC_CFG_H32 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rec_cnt_h8            : 8   ; /* [7..0]  */
        unsigned int    ctrl_mode             : 1   ; /* [8]  */
        unsigned int    af_error              : 3   ; /* [11..9]  */
        unsigned int    crc_ctrl_err          : 1   ; /* [12]  */
        unsigned int    crc_sync_err          : 1   ; /* [13]  */
        unsigned int    ctrl_edit_dis         : 1   ; /* [14]  */
        unsigned int    chn_crc_en            : 1   ; /* [15]  */
        unsigned int    rec_bufid             : 10  ; /* [25..16]  */
        unsigned int    avpes_drop_en         : 1   ; /* [26]  */
        unsigned int    avpes_cut_dis         : 1   ; /* [27]  */
        unsigned int    avpes_len_dis         : 1   ; /* [28]  */
        unsigned int    chn_mode              : 1   ; /* [29]  */
        unsigned int    chn_tee_lock          : 1   ; /* [30]  */
        unsigned int    rec_chn_en            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_REC_CFG_H32;


#define TS_REC_CFG_H32(rec_id)      (0x0200 + ((rec_id) << 2))   /* rec configure high 32bits register */
#define TS_REC_CFG_L32(rec_id)      (0x0400 + ((rec_id) << 2))   /* rec configure low 32bits register */

/* define the union U_TS_SCD_CFG_H32 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    scd_flth_en           : 8   ; /* [7..0]  */
        unsigned int    lock_drop_en          : 1   ; /* [8]  */
        unsigned int    idx_mode              : 1   ; /* [9]  */
        unsigned int    reserved_0            : 5   ; /* [14..10]  */
        unsigned int    pes_len_det_en        : 1   ; /* [15]  */
        unsigned int    ts_scd_bufid          : 10  ; /* [25..16]  */
        unsigned int    scd_es_short_en       : 1   ; /* [26]  */
        unsigned int    scd_es_long_en        : 1   ; /* [27]  */
        unsigned int    scd_pes_en            : 1   ; /* [28]  */
        unsigned int    scd_tpit_en           : 1   ; /* [29]  */
        unsigned int    scd_tee_lock          : 1   ; /* [30]  */
        unsigned int    scd_chn_en            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_SCD_CFG_H32;

#define TS_SCD_CFG_H32(scd_id)      (0x0600 + ((scd_id) << 2))   /* scd configure high 32bits register */

#define TS_SCD_CFG_L32(scd_id)      (0x0A00 + ((scd_id) << 2))   /* scd configure low 32bits register */

/******************************************************************************/
/* 96cv300 DMX PAR register definition  begin */
/******************************************************************************/
#define DMX_READ_REG_PAR(base, offset)  ioread32((void *)((base) + DMX_REGS_PAR_BASE + (offset)))
#define DMX_WRITE_REG_PAR(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_PAR_BASE + (offset)))

/* define the union U_PID_TAB_FILTER */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    mark_id               : 5   ; /* [4..0]  */
        unsigned int    reserved_0            : 3   ; /* [7..5]  */
        unsigned int    dmx_id                : 6   ; /* [13..8]  */
        unsigned int    reserved_1            : 2   ; /* [15..14]  */
        unsigned int    pid                   : 13  ; /* [28..16]  */
        unsigned int    reserved_2            : 2   ; /* [30..29]  */
        unsigned int    pid_tab_en            : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PID_TAB_FILTER;

#define PID_TAB_FILTER(pid_ch)           (0x0000 + ((pid_ch) << 4))   /* pid table filter register */

/* define the union U_PID_TAB_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pid_head_lock         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    pid_copy_en           : 1   ; /* [8]  */
        unsigned int    reserved_1            : 1   ; /* [9]  */
        unsigned int    cw_en                 : 1   ; /* [10]  */
        unsigned int    reserved_2            : 1   ; /* [11]  */
        unsigned int    whole_ts_en           : 1   ; /* [12]  */
        unsigned int    reserved_3            : 1   ; /* [13]  */
        unsigned int    pes_sec_en            : 1   ; /* [14]  */
        unsigned int    reserved_4            : 1   ; /* [15]  */
        unsigned int    av_pes_en             : 1   ; /* [16]  */
        unsigned int    reserved_5            : 1   ; /* [17]  */
        unsigned int    rec_en                : 1   ; /* [18]  */
        unsigned int    reserved_6            : 3   ; /* [21..19]  */
        unsigned int    ts_scd_en             : 1   ; /* [22]  */
        unsigned int    reserved_7            : 1   ; /* [23]  */
        unsigned int    pes_scd_en            : 1   ; /* [24]  */
        unsigned int    reserved_8            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PID_TAB_CTRL;

#define PID_TAB_CTRL(pid_ch)           (0x0004 + ((pid_ch) << 4))   /* pid table control register */

/* define the union U_PID_TAB_SUB_ID */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pid_copy_id           : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    cw_id                 : 8   ; /* [15..8]  */
        unsigned int    whole_sec_av_id       : 9   ; /* [24..16]  */
        unsigned int    reserved_1            : 3   ; /* [27..25]  */
        unsigned int    cc_repeat_drop        : 1   ; /* [28]  */
        unsigned int    cc_err_drop           : 1   ; /* [29]  */
        unsigned int    dsc_rec_mode          : 1   ; /* [30]  */
        unsigned int    reserved_2            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PID_TAB_SUB_ID;

#define PID_TAB_SUB_ID(pid_ch)         (0x0008 + ((pid_ch) << 4))   /* pid table control register */

/* define the union U_PID_TAB_REC_SCD */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rec_id                : 7   ; /* [6..0]  */
        unsigned int    reserved_0            : 9   ; /* [15..7]  */
        unsigned int    scd_id                : 8   ; /* [23..16]  */
        unsigned int    reserved_1            : 1   ; /* [24]  */
        unsigned int    pes_scd_id            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PID_TAB_REC_SCD;

#define PID_TAB_REC_SCD(pid_ch)        (0x000C + ((pid_ch) << 4))   /* rec scd table control register */

/* define the union U_PID_COPY_TAB_0 */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int mark_valid             : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PID_COPY_TAB_0;

#define PID_COPY_TAB_0(pcch)          (0x8000 + ((pcch) << 3))    /* pid copy table 0 register:mark_valid */

/* define the union U_PID_COPY_TAB_1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    copy_buf_id           : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 25  ; /* [30..6]  */
        unsigned int    pid_copy_lock          : 1  ; /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PID_COPY_TAB_1;

#define PID_COPY_TAB_1(pcch)          (0x8004 + ((pcch) << 3))    /* pid copy table 1 register:copy_buf_id */

/* define the union U_CW_TAB0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ts_descram            : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    pes_descram           : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_CW_TAB;

#define CW_TAB0(dsc_id)           (0x8200 + ((dsc_id) << 2))   /* cw 0~127 control register */
#define CW_TAB1(dsc_id)           (0x8e00 + ((dsc_id) << 2))   /* cw 128~255 control register */

/* define the union U_WHOLE_TS_TAB */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    whole_ts_buf_id       : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    whole_af_check_dis    : 1   ; /* [16]  */
        unsigned int    reserved_1            : 14  ; /* [30..17]  */
        unsigned int    whole_ts_lock         : 1 ; /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_WHOLE_TS_TAB;

#define WHOLE_TS_TAB(ts_ch_id)           (0x8400 + ((ts_ch_id) << 2))   /* whole ts table control register */

/* define the union U_AV_PES_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    av_pes_buf_id         : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    av_pusi_en            : 1   ; /* [16]  */
        unsigned int    reserved_1            : 7   ; /* [23..17]  */
        unsigned int    flt_rec_sel           : 1   ; /* [24]  */
        unsigned int    reserved_2            : 3   ; /* [27..25]  */
        unsigned int    av_pes_len_det        : 1   ; /* [28]  */
        unsigned int    reserved_3            : 2   ; /* [30..29]  */
        unsigned int    av_pes_lock           : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_AV_PES_TAB;

#define AV_PES_TAB(av_ch_idx)            (0x9200 + ((av_ch_idx) << 2))   /* avpes table control register */

/* define the union U_PES_SEC_TAB0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    data_type             : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    pes_sec_pusi_en       : 1   ; /* [8]  */
        unsigned int    reserved_1            : 7   ; /* [15..9]  */
        unsigned int    pes_sec_len_det       : 1   ; /* [16]  */
        unsigned int    reserved_2            : 7   ; /* [23..17]  */
        unsigned int    pes_sec_lock          : 1   ; /* [24]  */
        unsigned int    reserved_3            : 7   ; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PES_SEC_TAB0;


#define PES_SEC_TAB0(pes_sec_ch_inx)     (0x9600 + ((pes_sec_ch_inx) << 2))   /* pes section table control register */

/* define the union U_DMX_TS_CTRL_TAB */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    full_ts_buf_id        : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 1   ; /* [10]  */
        unsigned int    tei_drop              : 1   ; /* [11]  */
        unsigned int    cc_err_pusi_save      : 1   ; /* [12]  */
        unsigned int    cc_repeat_pusi_save   : 1   ; /* [13]  */
        unsigned int    dmx_ts_replace_47     : 1   ; /* [14]  */
        unsigned int    reserved_1            : 16  ; /* [30..15]  */
        unsigned int    dmx_tab_lock          : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DMX_TS_CTRL_TAB;

#define DMX_TS_CTRL_TAB(band)               (0x9e00 + ((band) << 2))

/* define the union U_WHOLE_TS_OUT_SEL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    full_ts_out_sel       : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    whole_ts_out_sel      : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_WHOLE_TS_OUT_SEL;

#define WHOLE_TS_OUT_SEL               (0xA000)   /* whole ts out select */

/* define the union U_TS_PARSER_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    af_check_dis          : 1   ; /* [0]  */
        unsigned int    all_af_drop           : 1   ; /* [1]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    cc_eq_rule            : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_TS_PARSER_CTRL;

#define TS_PARSER_CTRL                (0xA020)   /* ts parse control register  */

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    state_ram_id          : 11  ; /* [10..0]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_STATE_RAM_ID;

#define STATE_RAM_ID                (0xA010)

/* Define the union U_STATE_RAM_CTRL */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    state_data            : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6  ; /* [15..10]  */
        unsigned int    pid_chn_cc_int        : 1 ; /* [16] */
        unsigned int    pid_chn_tei_int       : 1 ; /* [17] */
        unsigned int    reserved_1            : 14 ; /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_STATE_RAM_CTRL;

#define STATE_RAM_CTRL          (0xA014)

/* Define the union U_DBG_PID_CNT_CHN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbg_pid_cnt_chn       : 11  ; /* [10..0]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DBG_PID_CNT_CHN;
#define DBG_PID_CNT_CHN(chan_id)          (0xE020 + ((chan_id) << 3))

#define DBG_PID_PKT_CNT(chan_id)          (0xE024 + ((chan_id) << 3))

/******************************************************************************/
/* hi_demux_v200 DMX DAV(except RAM) register definition  begin */
/******************************************************************************/
#define DMX_READ_REG_DAV(base, offset)  ioread32((void *)((base) + DMX_REGS_DAV_BASE + (offset)))
#define DMX_WRITE_REG_DAV(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_DAV_BASE + (offset)))

/* define the union U_DAV_INT_MASK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    dav_int_mask          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DAV_INT_MASK;
#define DAV_INT_MASK                (0xB0C4)   /* dav all interrupt mask register */

/* define the union U_DAV_INT */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    timeout_int_flg       : 1   ; /* [0]  */
        unsigned int    ts_buf_int_flg        : 1   ; /* [1]  */
        unsigned int    seop_int_flg          : 1   ; /* [2]  */
        unsigned int    prs_ovfl_int_flg      : 1   ; /* [3]  */
        unsigned int    pc_ovfl_int_flg       : 1   ; /* [4]  */
        unsigned int    fq_rd_int_flg         : 1   ; /* [5]  */
        unsigned int    fq_check_failed_int_flg : 1   ; /* [6]  */
        unsigned int    reserved_0            : 1   ; /* [7]  */
        unsigned int    prs_clrchn_int_flg    : 1   ; /* [8]  */
        unsigned int    ip_clrchn_int_flg     : 1   ; /* [9]  */
        unsigned int    ip_sync_int_flg       : 1   ; /* [10]  */
        unsigned int    ip_loss_int_flg       : 1   ; /* [11]  */
        unsigned int    pc_clrchn_int_flg     : 1   ; /* [12]  */
        unsigned int    pc_ap_ip_int_flg      : 1   ; /* [13]  */
        unsigned int    mmu_int_flg           : 1   ; /* [14]  */
        unsigned int    clr_pkt_int_flg       : 1   ; /* [15]  */
        unsigned int    pc_clr_pkt_int_flg    : 1   ; /* [16]  */
        unsigned int    reg_ovfl_int_flg      : 1   ; /* [17]  */
        unsigned int    info_ovlf_int_flg     : 1   ; /* [18]  */
        unsigned int    reserved_1            : 13  ; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_DAV_INT;

#define DAV_INT_ALL_INT_STATUS        (0xB0C8)   /* dav all interrupt status register */

/* Define the union U_CHN_INT_FLG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bak_ts_int            : 1   ; /* [0]  */
        unsigned int    bak_seop_int          : 1   ; /* [1]  */
        unsigned int    bak_ovfl_int          : 1   ; /* [2]  */
        unsigned int    bak_timeout_int       : 1   ; /* [3]  */
        unsigned int    bak_prs_clr_int       : 1   ; /* [4]  */
        unsigned int    bak_ovfl_type         : 1   ; /* [5]  */
        unsigned int    bak_buf_id            : 10  ; /* [15..6]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_CHN_INT_FLG;

#define CHN_INT_FLG                 (0xB0CC + (buf_group) << 2)   /* buffer group is form 0 to 7, total 8 group */

/* Define the union U_BUF_INT_CHN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    buf_int_chn           : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_BUF_INT_CHN;

#define BUF_INT_CHN                 (0xB0EC)

/******************************************************************************/
/* hi_demux_v200 DAV(RAM PORT) register definition  begin */
/******************************************************************************/
#define DMX_READ_REG_RAM(base, offset)  ioread32((void *)((base) + DMX_REGS_DAV_BASE + (offset)))
#define DMX_WRITE_REG_RAM(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_DAV_BASE + (offset)))

/* define the union U_IP_FQ_BUF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ip_fqsa              : 20  ; // [19..0] 4K align
        unsigned int    ip_fqsize            : 10  ; // [29..20] the depth of fq description queue
        unsigned int    ip_ip_rd_int_en      : 1   ; // [30]
        unsigned int    ip_tread_int_en      : 1   ; // [31]
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_FQ_BUF;
#define IP_FQ_BUF(port_id)               (0xB900 + ((port_id) << 5))  /* IP channel 0 FQ descrition */

/* define the union U_IP_FQ_PTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ip_fq_wptr            : 10  ; /* [9..0]  */
        unsigned int    ip_fq_rptr            : 10  ; /* [19..10]  */
        unsigned int    reserved_0            : 12  ; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_FQ_PTR;
#define IP_FQ_PTR(port_id)               (0xB904 + ((port_id) << 5))  /* IP channel 0 description word queue read-write address */

/* define the union U_IP_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    sync_en               : 1   ; /* [0]  */
        unsigned int    sync_type             : 2   ; /* [2..1]  */
        unsigned int    sync_th               : 3   ; /* [5..3]  */
        unsigned int    lost_en               : 1   ; /* [6]  */
        unsigned int    lost_th               : 2   ; /* [8..7]  */
        unsigned int    reserved_0            : 1   ; /* [9]  */
        unsigned int    ip_sync_int_en        : 1   ; /* [10]  */
        unsigned int    ip_loss_int_en        : 1   ; /* [11]  */
        unsigned int    reserved_1            : 20  ; /* [31..12]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_CFG;
#define IP_CFG(port_id)                  (0xB908 + ((port_id) << 5))  /* IP channel 0 description configuration */

/* define the union U_IP_SYNC_LEN */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ip_nosync_188_len     : 8   ; /* [7..0]  */
        unsigned int    ip_nosync_204_len     : 8   ; /* [15..8] */
        unsigned int    ip_nosync_region      : 8   ; /* [23:16] */
        unsigned int    ip_nosync_step        : 2   ; /* [25:24] */
        unsigned int    reserved0             : 6   ; /* [31:26] */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_SYNC_LEN;
#define IP_SYNC_LEN(port_id)             (0xB90C + ((port_id) << 5))  /* IP port sync len config reg */

/* define the union U_IP_RATE_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ip_rate_cfg           : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_RATE_CFG;
#define IP_RATE_CFG(port_id)             (0xB910 + ((port_id) << 5))  /* IP channel 0 rate configuration  */

/* define the union U_IP_DESC_ADD */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    desc_addvld           : 1   ; /* [0]  */
        unsigned int    desc_add              : 10  ; /* [10..1]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_DESC_ADD;
#define IP_DESC_ADD(port_id)             (0xB914 + ((port_id) << 5))  /* IP channel 0 description word queue multi-word interrupt water-line set */

/* define the union U_IP_RX_EN */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ip_crc_en             : 1   ; /* [0]  */
        unsigned int    ip_rx_en              : 1   ; /* [1]  */
        unsigned int    ip_ctrl_en            : 1   ; /* [2] */
        unsigned int    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_RX_EN;
#define IP_RX_EN(port_id)                (0xB918 + ((port_id) << 5))  /* IP channel 0 sync check configuration */

/* Define the union U_IP_FQ_SESSION */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ip_fq_session         : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_IP_FQ_SESSION;
#define IP_FQ_SESSION(port_id)           (0xB91C + ((port_id) << 5))  /* tsbuffer sesstion */

/* define the union U_IP_SEC_ATTR */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int ip_sec_attr            : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_SEC_ATTR;
#define IP_SEC_ATTR                     (0xBF84)  /*  */

/* define the union U_DESC_RD_INT */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int ip_rd_int              : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DESC_RD_INT;
#define IP_DESC_RD_INT                  (0xBF88)  /*  */

/* define the union U_RAW_DESC_RD_INT */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int raw_ip_rd_int          : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_RAW_DESC_RD_INT;
#define IP_RAW_DESC_RD_INT              (0xBF8C)  /*  */

/* define the union U_RAW_IP_TREAD_INT */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int raw_ip_tread_int       : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_RAW_IP_TREAD_INT;
#define IP_RAW_IP_TREAD_INT             (0xBF90)  /*  */

/* define the union U_IP_TREAD_INT */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int ip_tread_int           : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_TREAD_INT;
#define IP_TREAD_INT                    (0xBF94)  /*  */

/* define the union U_IP_FQ_CHECK_CODE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ip_fq_check_code      : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_IP_FQ_CHECK_CODE;
#define IP_FQ_CHECK_CODE                (0xBF98)  /*  */

/* some simple registers */
#define AP_CLEAR2                       (0xB788)  /* ram port ap status clear register */
#define RAW_IP_SYNC_INT                 (0xBF9C)  /* ram sync error raw interrupt regitster */
#define IP_SYNC_INT                     (0xBFA0)  /* ram sync error interrupt status regitster */
#define RAW_IP_LOSS_INT                 (0xBFA4)  /* ram sync loss raw interrupt regitster */
#define IP_LOSS_INT                     (0xBFA8)  /* ram sync loss interrupt status regitster */
#define VIR_CHN_FLG                     (0xBFAC)  /* virtual multi ramport enable register */
#define IP_CKGT_ENA                     (0xBFB0)  /* ram clock gate enable, used for low power manage  */
#define IP_CLR_CHN_REQ                  (0xD200)  /* ram clear channel require bits  */
#define RAW_IP_CLR_INT                  (0xD204)  /* ram clear channel raw interrupt register  */
#define IP_CLR_INT_EN                   (0xD208)  /* ram clear channel interrupt enable register  */
#define IP_CLR_INT                      (0xD20C)  /* ram clear channel interrupt status register  */
#define IP_PKT_CNT(port_id)             (0xDA80 + ((port_id) << 2)) /* ip channel output packets counter. */

/******************************************************************************/
/*                      96cv300 DMX DAV(buf) register definition  begin       */
/***************96cv300 DMX DAV(buf) register definition  begin ***************/
#define DMX_READ_REG_BUF(base, offset)  ioread32((void *)((base) + DMX_REGS_DAV_BASE + (offset)))
#define DMX_WRITE_REG_BUF(base, offset, value)   iowrite32((value), (void*)((base) + DMX_REGS_DAV_BASE + (offset)))

/* Define the union U_BUF_SET */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    buf_sec_attr          : 4   ; /* [3..0]  */
        unsigned int    buf_lock              : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_BUF_SET;
#define BUF_SET(buf_id)              (0x0 + ((buf_id) << 2)) /* buffer secure attribute */

/* define the union U_INT_SET */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ts_int_en             : 1   ; /* [0]  */
        unsigned int    seop_int_en           : 1   ; /* [1]  */
        unsigned int    ovfl_int_en           : 1   ; /* [2]  */
        unsigned int    timeout_int_en        : 1   ; /* [3]  */
        unsigned int    prs_clr_int_en        : 1   ; /* [4]  */
        unsigned int    reserved_0            : 3   ; /* [7..5]  */
        unsigned int    ts_int_th             : 2   ; /* [9..8]  */
        unsigned int    timeout_int_th        : 3   ; /* [12..10]  */
        unsigned int    seop_int_th           : 4   ; /* [16..13]  */
        unsigned int    seop_cnt              : 7   ; /* [23..17]  */
        unsigned int    ap_th                 : 7   ; /* [30..24]  */
        unsigned int    reserved_1            : 1   ; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_INT_SET;

#define INT_SET(buf_id)   (0x1000 + ((buf_id) << 2))  /* buf interrupt and threshold configure register */

/* define the union U_REG_STATE_RAM */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    state_ram_en          : 1   ; /* [0]  */
        unsigned int    sop_flag              : 1   ; /* [1]  */
        unsigned int    drop_flag             : 1   ; /* [2]  */
        unsigned int    state_data_type       : 3   ; /* [5..3]  */
        unsigned int    pesd_en               : 1   ; /* [6]  */
        unsigned int    mq_en                 : 1   ; /* [7]  */
        unsigned int    ts_int                : 1   ; /* [8]  */
        unsigned int    seop_int              : 1   ; /* [9]  */
        unsigned int    ovfl_int              : 1   ; /* [10]  */
        unsigned int    timeout_int           : 1   ; /* [11]  */
        unsigned int    prs_clr_int           : 1   ; /* [12]  */
        unsigned int    ovfl_type_flg         : 1   ; /* [13]  */
        unsigned int    reserved_0            : 2   ; /* [15..14]  */
        unsigned int    raw_ts_int            : 1   ; /* [16]  */
        unsigned int    raw_seop_int          : 1   ; /* [17]  */
        unsigned int    raw_ovfl_int          : 1   ; /* [18]  */
        unsigned int    raw_timeout_int       : 1   ; /* [19]  */
        unsigned int    raw_prs_clr_int       : 1   ; /* [20]  */
        unsigned int    reserved_1            : 11  ; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_REG_STATE_RAM;

#define   REG_STATE_RAM(buf_id) (0x2000 + ((buf_id) << 2))  /* buf start address configure register */

/* define the union U_SEOP_ADDR */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    seop_addr             : 26  ; /* [25..0]  */
        unsigned int    reserved_0            : 2   ; /* [27..26]  */
        unsigned int    ap_flg                : 1   ; /* [28]  */
        unsigned int    reserved_1            : 3   ; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_SEOP_ADDR;

#define SEOP_ADDR(buf_id)   (0x3000 + ((buf_id) << 2))  /* seop address register */

/* define the union U_MQ_BUF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    mqsa                  : 20  ; /* [19..0]  */
        unsigned int    mqsize                : 12  ; /* [31..20]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_MQ_BUF;
#define MQ_BUF(mq_id)   (0x4000 + ((mq_id) << 2))  /* mq start address and depth configure register */

/* define the union U_MQ_PTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    mqwptr                : 12  ; /* [11..0]  */
        unsigned int    mqrptr                : 12  ; /* [23..12]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_MQ_PTR;
#define MQ_PTR(mq_id)   (0x5000 + ((mq_id) << 2))  /* mq read and write configure register */

/* Define the union U_MQ_SESSION */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mq_session            : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_MQ_SESSION;
#define MQ_SESSION      (0xB0AC)  /* mq read and write configure register */

/* define the union U_BUFSA */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bufsa                 : 20  ; /* [19..0]  */
        unsigned int    buf_session           : 4   ; /* [23..20]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_BUFSA;

#define BUFSA(buf_id)   (0x6000 + ((buf_id) << 2))  /* buf start address configure register */

/* define the union U_BUFSIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    bufsize               : 14  ; /* [13..0]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_BUFSIZE;
#define BUFSIZE(buf_id)  (0x7000 + ((buf_id) << 2))  /* buf start address configure register */

/* define the union U_BUFWPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    bufwptr               : 26  ; /* [25..0]  */
        unsigned int    reserved_0            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_BUFWPTR;
#define BUFWPTR(buf_id)  (0x8000 + ((buf_id) << 2))  /* buf write pointer register */

/* define the union U_BUFRPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    bufrptr               : 26  ; /* [25..0]  */
        unsigned int    reserved_0            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_BUFRPTR;
#define BUFRPTR(buf_id)  (0x9000 + ((buf_id) << 2))  /* buf read pointer configure register */

/* define the union U_BUFEPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    bufeptr               : 26  ; /* [25..0]  */
        unsigned int    reserved_0            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_BUFEPTR;
#define BUFEPTR(buf_id)  (0xA000 + ((buf_id) << 2))  /* buf end pointer register */

/* define the union U_BUF_INT_SET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    seop_buf_int_set      : 3   ; /* [2..0]  */
        unsigned int    ts_buf_int_set        : 8   ; /* [10..3]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_BUF_INT_SET;
#define BUF_INT_SET        (0xB0C0)  /* buf base interrupt threshold register */

#define AP_CLEAR0(reg_idx)  (0xB780 + ((reg_idx) << 2))  /* pid copy ap status clear register */

/* Define the union U_MQ_AP_TH */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mq_ap_th              : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_MQ_AP_TH;
#define MQ_AP_TH  (0xB78C)  /* mq ap threthold */

/* define the union U_PC_BUF_ID_LOCK */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_buf_id_lock        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PC_BUF_ID_LOCK;
#define PC_BUF_ID_LOCK     (0xB7FC)  /* pid copy buffer lock register */

/* define the union U_PC_BUF_ID_SET */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_buf_id             : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PC_BUF_ID_SET;
#define PC_BUF_ID_SET      (0xB800)  /* pid copy buffer ID set register */

/* define the union U_PC_BUFSA */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_bufsa              : 20  ; /* [19..0]  */
        unsigned int    pc_session            : 4   ; /* [23..20] */
        unsigned int    reserved_0            : 8  ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PC_BUFSA;
#define PC_BUFSA        (0xB804)  /* pid copy buffer start address register */

/* define the union U_PC_STATE_RAM */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_state_ram_en       : 1   ; /* [0]  */
        unsigned int    pc_ovfl_int_en        : 1   ; /* [1]  */
        unsigned int    pc_wrap_flag          : 1   ; /* [3]  */
        unsigned int    pc_bufsize            : 14  ; /* [16..3]  */
        unsigned int    reserved_0            : 15  ; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PC_STATE_RAM;
#define PC_STATE_RAM               (0xB808)  /* pid copy buffer state register */

/* define the union U_PC_BUFWPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_bufwptr            : 26  ; /* [25..0]  */
        unsigned int    reserved_0            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PC_BUFWPTR;
#define PC_BUFWPTR                 (0xB80C)  /* pid copy buffer write pointer */

/* define the union U_PC_BUFRPTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_bufrptr            : 26  ; /* [25..0]  */
        unsigned int    reserved_0            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_PC_BUFRPTR;
#define PC_BUFRPTR                 (0xB810)  /* pid copy buffer read pointer */

/* Define the union U_PC_BUFCFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pc_sec_attr           : 4   ; /* [3..0]  */
        unsigned int    pc_lock               : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PC_BUFCFG;
#define PC_BUFCFG                  (0xB814)  /* pid copy buffer read pointer */

#define PC_RX_EN0(reg_idx)         (0xB830 + ((reg_idx) << 2))   /* pid copy RX enable register */

/* define the union U_PC_AP_TH */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_ap_th              : 10   ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;
    /* define an unsigned member */
    unsigned int    u32;
} U_PC_AP_TH;
#define PC_AP_TH                   (0xB848)   /* pid copy ap threshold register */

#define RAW_PC_AP_IP_INT           (0xB84C)   /* pid copy buffer ap raw interrupt register */

/* define the union PC_CLR_CHN_CFG */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pc_clr_chn_start     : 1   ; /* [0]  */
        unsigned int    pc_clr_chn_id         : 6   ; /* [6..1]  */
        unsigned int    reserved0              : 25 ; /* [31..7] */
    } bits;
    /* define an unsigned member */
    unsigned int    u32;
} U_PC_CLR_CHN_CFG;

#define PC_CLR_CHN_CFG          (0xD210)

#define RAW_PC0_CLR_INT(pc_index)         (0xD214 + (pc_index) * 0xC)
#define RAW_CLR0_PKT_INT(band_index)        (0xd22c + ((band_index) * 0xC))

/* Define the union U_PRS_CLR_CHN_CFG */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    prs_clr_chn_start     : 1   ; /* [0]  */
        unsigned int    prs_clr_chn_id        : 10  ; /* [10..1]  */
        unsigned int    reserved_0            : 21  ; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_PRS_CLR_CHN_CFG;

#define PRS_CLR_CHN_CFG         (0xD000)

#define PRS_CLR_INT_FLAG        (0xD188)

typedef union {
    struct {
        unsigned int    dmx2tsio_ap_set_0   : 16; /* [15..0] */
        unsigned int    dmx2tsio_ap_set_1   : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_DMX2TSIO_AP_SET;

#define DMX2TSIO_AP_SET(index)    (0xD420 + ((index) << 2))

typedef union {
    struct {
        unsigned int    dmx2tsio_ap_clear   : 16; /* [15..0] */
        unsigned int    reserved_0           : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_DMX2TSIO_AP_CLEAR;

#define DMX2TSIO_AP_CLEAR   (0xD520)

typedef union {
    struct {
        unsigned int    dmx2tsio_tx_ap_flg  : 16; /* [15..0] */
        unsigned int    dmx2tsio_pc_ap_flg  : 16; /* [31..0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} U_DMX2TSIO_AP_FLG;

#define DMX2TSIO_AP_FLG (0XD524)

typedef union {
    struct {
        unsigned int    dmx2tsio_pc_ap_set_0    : 16; /* [15..0] */
        unsigned int    dmx2tsio_pc_ap_set_1    : 16; /* [31..16] */
    } bits;

    /* Define an unsinged member */
    unsigned int u32;
} U_DMX2TSIO_PC_AP_SET;

#define DMX2TSIO_PC_AP_SET(index) (0xD630 + ((index) << 2))

#define MMU_SEC_TLB             (0xF000)
#define MMU_SEC_EADDR           (0xF008)
#define MMU_R_SEC_EADDR         (0xF108)

#define MMU_NOSEC_TLB           (0xF004)
#define MMU_NOSEC_EADDR         (0xF00C)
#define MMU_R_NOSEC_EADDR       (0xF10C)

/* Define the union U_NOSEC_EADDR_SESSION */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    nosec_eaddr_session   : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_NOSEC_EADDR_SESSION;
#define NOSEC_EADDR_SESSION     (0xF104)

/* Define the union U_NOSEC_R_EADDR_SESSION */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    nosec_r_eaddr_session : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_NOSEC_R_EADDR_SESSION;
#define NOSEC_R_EADDR_SESSION   (0xF110)

/* buf_idx from 0 to 31 */
#define MMU_BUF_DIS(buf_idx)    (0xF010 + ((buf_idx) << 2))
/* pc_idx from 0 to 1 */
#define MMU_PC_WDIS_0(pc_idx)   (0xF090 + ((pc_idx) << 2))
#define MMU_PC_RDIS_0(pc_idx)   (0xF098 + ((pc_idx) << 2))

#define MMU_IP_DIS              (0xF0A0)
#define MMU_IP_DES_DIS          (0xF0F0)

/* Define the union U_SEC_MMU_EN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sec_mmu_en            : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_SEC_MMU_EN;
#define SEC_MMU_EN              (0xF0AC)

/* Define the union U_NOSEC_MMU_EN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    nosec_mmu_en          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} U_NOSEC_MMU_EN;
#define NOSEC_MMU_EN            (0xF0B0)

/******************************************************************************/
/*                      hi96cv300 DMX MDSC register definition  begin                  */
/******************************************************************************/
#define DMX_READ_REG_MDSC(base, offset)  ioread32((void *)((base) + (offset)))
#define DMX_WRITE_REG_MDSC(base, offset, value)   iowrite32((value), (void*)((base) + (offset)))

#define KEY_ENCRPTY_SEL(reg_idx)      (0x0000 + ((reg_idx) << 2))  /* IV or CW KEY even/odd bits select register */
#define CSA2_ENTROPY_CLOSE(reg_idx)   (0x0020 + ((reg_idx) << 2))  /* CSA2 entropy decrease register */

/* define the union U_MDSC_EN */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 8   ; /* [7..0]  */
        unsigned int    ca_en                 : 1   ; /* [8]  */
        unsigned int    reserved_1            : 3   ; /* [11..9]  */
        unsigned int    ts_ctrl_dsc_change_en : 1   ; /* [12]  */
        unsigned int    reserved_2            : 11  ; /* [23..13]  */
        unsigned int    cw_iv_en              : 1   ; /* [24]  */
        unsigned int    reserved_3            : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_MDSC_EN;
#define MDSC_EN                     (0x0050)          /* MDSC enable register  */

/* define the union U_MDSC_CPD_CORE_DISABLE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cpd_core_disable      : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_MDSC_CPD_CORE_DISABLE;
#define MDSC_CPD_CORE_DISABLE       (0x0054)          /* MDSC CPD core enable register  */

/* define the union U_MDSC_CA_CORE_DISABLE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ca_core_disable       : 28  ; /* [27..0]  */
        unsigned int    reserved_0            : 4   ; /* [31..28]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_MDSC_CA_CORE_DISABLE;
#define MDSC_CA_CORE_DISABLE        (0x0058)          /* MDSC CA core enable register  */

/* define the union U_MDSC_CPS_CORE_DISABLE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cps_core_disable      : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_MDSC_CPS_CORE_DISABLE;
#define MDSC_CPS_CORE_DISABLE       (0x005C)          /* MDSC CPS core enable register  */

#define MULTI2_SYS_KEY0             (0x0480)          /* MULTI2 sys_key0 */
#define MULTI2_SYS_KEY1             (0x0484)          /* MULTI2 sys_key1 */
#define MULTI2_SYS_KEY2             (0x0488)          /* MULTI2 sys_key2 */
#define MULTI2_SYS_KEY3             (0x048c)          /* MULTI2 sys_key3 */
#define MULTI2_SYS_KEY4             (0x0490)          /* MULTI2 sys_key4 */
#define MULTI2_SYS_KEY5             (0x0494)          /* MULTI2 sys_key5 */
#define MULTI2_SYS_KEY6             (0x0498)          /* MULTI2 sys_key6 */
#define MULTI2_SYS_KEY7             (0x049c)          /* MULTI2 sys_key7 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_REG_H__

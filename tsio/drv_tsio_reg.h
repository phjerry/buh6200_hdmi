/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio register definition.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_TSIO_REG_H__
#define __DRV_TSIO_REG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TSIO_READ_REG(base, offset)  ioread32((void *)(base + offset))
#define TSIO_WRITE_REG(base, offset, value) iowrite32(value, (void*)(base + offset))

/* 0000 */
typedef union {
    struct {
        unsigned int    service_id            : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 2   ; /* [7..6]  */
        unsigned int    tsid                  : 4   ; /* [11..8]  */
        unsigned int    reserved_1            : 3   ; /* [14..12]  */
        unsigned int    tsid_type             : 1   ; /* [15]  */
        unsigned int    pid                   : 13  ; /* [28..16]  */
        unsigned int    reserved_2            : 2   ; /* [30..29]  */
        unsigned int    pid_table_en          : 1   ; /* [31]  */
    } bits;

    unsigned int    u32;
} U_PID_TABLE;

#define TSIO_REG_PID_TABLE(id)                (0x0000 + (id << 2))

typedef union {
    struct {
        unsigned int    tsio2dmx_sw_ts_sel_cfg_0 : 8   ; /* [7..0]  */
        unsigned int    tsio2dmx_sw_ts_sel_cfg_1 : 8   ; /* [15..8]  */
        unsigned int    tsio2dmx_sw_ts_sel_cfg_2 : 8   ; /* [23..16]  */
        unsigned int    tsio2dmx_sw_ts_sel_cfg_3 : 8   ; /* [31..24]  */
    } bits;

    unsigned int    u32;
} U_TSIO2DMX_SW_TS_SEL0;

#define TSIO_REG_TSIO2DMX_SW_TS_SEL0          0x0c00

typedef union {
    struct {
        unsigned int    tsio2dmx_sw_ts_sel_cfg_4 : 8   ; /* [7..0]  */
        unsigned int    tsio2dmx_sw_ts_sel_cfg_5 : 8   ; /* [15..8]  */
        unsigned int    tsio2dmx_sw_ts_sel_cfg_6 : 8   ; /* [23..16]  */
        unsigned int    tsio2dmx_sw_ts_sel_cfg_7 : 8   ; /* [31..24]  */
    } bits;

    unsigned int    u32;
} U_TSIO2DMX_SW_TS_SEL1;

#define TSIO_REG_TSIO2DMX_SW_TS_SEL1          0x0c04

/* 1000 */
typedef union {
    struct {
        unsigned int    cc_send_length        : 9    ; /* [8..0] */
        unsigned int    reserved              : 22   ; /* [30..9] */
        unsigned int    cc_send_rdy           : 1    ; /* [31] */
    } bits;

    unsigned int    u32;
} U_CC_LEN;

#define TSIO_REG_CC_LEN                       0x1000

#define TSIO_REG_CC_DATA(id)                  (0x1004 + (id << 2))

typedef union {
    struct {
        unsigned int    stuff_sid             : 6   ; /* [5..0]  */
        unsigned int    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    unsigned int    u32;
} U_STUFF_SID;

#define TSIO_REG_STUFF_SID                    0x1200

typedef union {
    struct {
        unsigned int    live_2_rr_dma         : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    unsigned int    u32;
} U_LIVE_DMA_RR;

#define TSIO_REG_LIVE_DMA_RR                   0x1204

typedef union {
    struct {
        unsigned int    timer_en              : 1   ; /* [0]  */
        unsigned int    reserved_0            : 7   ; /* [7..1]  */
        unsigned int    flow_ctrl_rst         : 1   ; /* [8]  */
        unsigned int    flow_ctrl_en          : 1   ; /* [9] */
        unsigned int    reserved_1            : 22  ; /* [31..10]  */
    } bits;

    unsigned int    u32;
} U_TIMER_CTRL;

#define TSIO_REG_TIMER_CTL                     0x1208

typedef union {
    struct {
        unsigned int    dbg_sc_cts_en         : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    unsigned int    u32;
} U_DBG_SC_CTS;

#define TSIO_REG_DBG_SC_CTS                   0x1428

typedef union {
    struct {
        unsigned int    dbg_sc_gen_open       : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} U_DBG_SC_GEN_OPEN;

#define TSIO_REG_DBG_SC_GEN_OPEN               0x142c


/* 2000 */
typedef union {
    struct {
        unsigned int    buf_id                : 8   ; /* [7..0]  */
        unsigned int    outport_id            : 3   ; /* [10..8]  */
        unsigned int    reserved_0            : 5   ; /* [15..11]  */
        unsigned int    trans_type            : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    dma_en                : 1   ; /* [20]  */
        unsigned int    outport_en            : 1   ; /* [21]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    sp_save               : 1   ; /* [24]  */
        unsigned int    reserved_3            : 6   ; /* [30..25]  */
        unsigned int    sid_table_en          : 1   ; /* [31]  */
    } bits;

    unsigned int    u32;
} U_SID_TABLE;

#define TSIO_REG_SID_TABLE(id)                 (0x2000 + (id << 2))

typedef union {
    struct {
        unsigned int    cc_rsv_length         : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    unsigned int    u32;
} U_CC_RAM_LEN;

#define TSIO_REG_CC_RAM_LEN                    0x2100

#define TSIO_REG_CC_RAM_DATA(id)               (0x2104 + (id << 2))

typedef union {
    struct {
        unsigned int    cc_ram_rdone          : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    unsigned int    u32;
} U_CC_RAM_RDONE;

#define TSIO_REG_CC_RAM_RDONE                  0x2208

typedef union {
    struct {
        unsigned int cc_rev_hold_conflict   : 1    ; /* [0] */
        unsigned int reserved_0             : 31   ; /* [31 ..1] */
    } bits;

    unsigned int    u32;
} U_CC_REV_HOLD_CONFLICT;

#define TSIO_REG_CC_REV_HOLD_CONFLICT         0x220c

#define TSIO_REG_SID_COUNTER(id)               (0x2400 + (id << 2))

/* 4000 */
typedef union {
    struct {
        unsigned int    tx_cc_send_done_int   : 1   ; /* [0]  */
        unsigned int    rx_cc_done_int        : 1   ; /* [1]  */
        unsigned int    rx_cts_int            : 1   ; /* [2]  */
        unsigned int    reserved_0            : 21  ; /* [23..3]  */
        unsigned int    rx_route_fifo_overflow_int : 1   ; /* [24]  */
        unsigned int    reserved_1            : 3   ; /* [27..25]  */
        unsigned int    rx_parser_err_int     : 1   ; /* [28]  */
        unsigned int    tx_rr_err_int         : 1   ; /* [29]  */
        unsigned int    pid_filter_err_int    : 1   ; /* [30]  */
        unsigned int    dma_int               : 1   ; /* [31]  */
    } bits;

    unsigned int    u32;
} U_TSIO_RIS;

#define TSIO_REG_INT_RIS                      0x4000

typedef union {
    struct {
        unsigned int    tx_cc_send_done_mis   : 1   ; /* [0]  */
        unsigned int    rx_cc_done_mis        : 1   ; /* [1]  */
        unsigned int    rx_cts_mis            : 1   ; /* [2]  */
        unsigned int    reserved_0            : 21  ; /* [23..3]  */
        unsigned int    rx_route_fifo_overflow_mis : 1   ; /* [24]  */
        unsigned int    reserved_1            : 3   ; /* [27..25]  */
        unsigned int    rx_parser_err_mis     : 1   ; /* [28]  */
        unsigned int    tx_rr_err_mis         : 1   ; /* [29]  */
        unsigned int    pid_filter_err_mis    : 1   ; /* [30]  */
        unsigned int    dma_mis               : 1   ; /* [31]  */
    } bits;

    unsigned int    u32;
} U_TSIO_MIS;

#define TSIO_REG_INT_MIS                      0x4004

typedef union {
    struct {
        unsigned int    tx_cc_send_done_ie    : 1   ; /* [0]  */
        unsigned int    rx_cc_done_ie         : 1   ; /* [1]  */
        unsigned int    rx_cts_ie             : 1   ; /* [2]  */
        unsigned int    reserved_0            : 21  ; /* [23..3]  */
        unsigned int    rx_route_fifo_overflow_ie : 1   ; /* [24]  */
        unsigned int    reserved_1            : 6   ; /* [30..25]  */
        unsigned int    tsio_ie               : 1   ; /* [31]  */
    } bits;

    unsigned int    u32;
} U_TSIO_IE;

#define TSIO_REG_INT_IE                       0x4008

#define TSIO_REG_PID_FILTER_ERR_INT_RIS       0x4010
#define TSIO_REG_PID_FILTER_ERR_INT_MIS       0x4014
#define TSIO_REG_PID_FILTER_ERR_INT_IE        0x4018

#define TSIO_REG_TX_RR_ERR_INT_RIS            0x4020
#define TSIO_REG_TX_RR_ERR_INT_MIS            0x4024
#define TSIO_REG_TX_RR_ERR_INT_IE             0x4028

#define TSIO_REG_RX_PARSER_ERR_INT_RIS        0x4030
#define TSIO_REG_RX_PARSER_ERR_INT_MIS        0x4034

typedef union {
    struct {
        unsigned int    rx_phy_sp_err_ie      : 1   ; /* [0]  */
        unsigned int    rx_fifo_overflow_ie   : 1   ; /* [1]  */
        unsigned int    reserved_0            : 1   ; /* [2]  */
        unsigned int    rx_sp_sync_err_ie     : 1   ; /* [3]  */
        unsigned int    rx_sp_rfu0_err_ie     : 1   ; /* [4]  */
        unsigned int    rx_sp_dma_end_err_ie  : 1   ; /* [5]  */
        unsigned int    rx_sp_encry_en_err_ie : 1   ; /* [6]  */
        unsigned int    rx_sp_tsid_err_ie     : 1   ; /* [7]  */
        unsigned int    rx_sp_soc_define_err_ie : 1   ; /* [8]  */
        unsigned int    rx_sp_rfu1_err_ie     : 1   ; /* [9]  */
        unsigned int    rx_sp_rfu2_err_ie     : 1   ; /* [10]  */
        unsigned int    rx_sp_stuff_load_err_ie : 1   ; /* [11]  */
        unsigned int    rx_sp_sc_gen_err_ie   : 1   ; /* [12]  */
        unsigned int    reserved_1            : 3  ; /* [15..13]  */
        unsigned int    rx_cc_err_type_ie     : 4  ; /* [19..16]  */
        unsigned int    reserved_2            : 12  ; /* [31..20]  */
    } bits;

    unsigned int    u32;
} U_RX_PARSER_ERR_IE;

#define TSIO_REG_RX_PARSER_ERR_INT_IE         0x4038


/* 5000 */
typedef union {
    struct {
        unsigned int    dma_mmu_en            : 1   ; /* [0]  */
        unsigned int    chnl_pend_int_en      : 1   ; /* [1]  */
        unsigned int    obuf_pack_int_en      : 1   ; /* [2]  */
        unsigned int    obuf_nr_int_en        : 1   ; /* [3]  */
        unsigned int    dma_err_int_en        : 1   ; /* [4]  */
        unsigned int    live_ts_buf_en        : 1   ; /* [5]  */
        unsigned int    dma_end_int_en        : 1   ; /* [6]  */
        unsigned int    dma_flush_int_en      : 1   ; /* [7]  */
        unsigned int    dmux_pend_en          : 1   ; /* [8]  */
        unsigned int    des_end_en            : 1   ; /* [9]  */
        unsigned int    dma_timeout_int_en   : 1   ; /* [10] */
        unsigned int    reserved_0            : 5   ; /* [15..11]  */
        unsigned int    dma_bid_err_en        : 1   ; /* [16] */
        unsigned int    dma_live_oflw_err_en  : 1   ; /* [17] */
        unsigned int    chk_code_err_en       : 1   ; /* [18] */
        unsigned int    obuf_oflw_err_en      : 1   ; /* [19] */
        unsigned int    des_type_err_en       : 1   ; /* [20] */
        unsigned int    ichl_wptr_oflw_err_en : 1   ; /* [21] */
        unsigned int    reserved_1            : 10  ; /* [22..31] */
    } bits;

    unsigned int    u32;
} U_DMA_CTRL;

#define TSIO_REG_DMA_CTRL                    0x5000

typedef union {
    struct {
        unsigned int    dma_int_msk           : 1   ; /* [0]  */
        unsigned int    chnl_pend_status      : 1   ; /* [1]  */
        unsigned int    dma_pack_int          : 1   ; /* [2]  */
        unsigned int    dma_obuf_nr_full      : 1   ; /* [3]  */
        unsigned int    dma_end_int           : 1   ; /* [4]  */
        unsigned int    dma_flush_int         : 1   ; /* [5]  */
        unsigned int    des_end_status        : 1   ; /* [6]  */
        unsigned int    reserved_0            : 3   ; /* [9..7]  */
        unsigned int    dma_timeout_int       : 1   ; /* [10] */
        unsigned int    reserved_1             : 5  ; /* [15..11] */
        unsigned int    dma_bid_err_int       : 1   ; /* [16]  */
        unsigned int    dma_live_oflw_err_int : 1   ; /* [17]  */
        unsigned int    chk_code_err_int      : 1   ; /* [18]  */
        unsigned int    obuf_oflw_err_int     : 1   ; /* [19]  */
        unsigned int    des_type_err_int      : 1   ; /* [20]  */
        unsigned int    ichl_wptr_oflw_err_int : 1   ; /* [21]  */
        unsigned int    reserved_2            : 2   ; /* [23..22]  */
        unsigned int    dma_overflow_err_int  : 1   ; /* [24]  */
        unsigned int    dma_tlb_err_int       : 1   ; /* [25]  */
        unsigned int    dma_bus_err_int       : 1   ; /* [26]  */
        unsigned int    reserved_3            : 5   ; /* [31..27]  */
    } bits;

    unsigned int    u32;
} U_DMA_GLB_STAT;

#define TSIO_REG_DMA_GBL_STAT                0x5004

#define TSIO_REG_DMA_TLB_BASE                0x5010

#define TSIO_REG_DMA_DES_END_INT             0x5028

#define TSIO_REG_DMA_CHNL_STAT               0x5030

#define TSIO_REG_DMA_CHNL_EN                 0x5034

typedef union {
    struct {
        unsigned int    dma_chanls_dis        : 16  ; /* [15..0]  */
        unsigned int    dma_pi_mmu_dis        : 16  ; /* [31..16]  */
    } bits;

    unsigned int    u32;
} U_DMA_CHNL_DIS;

#define TSIO_REG_DMA_CHNL_DIS                0x5038

#define TSIO_REG_DMA_CHNL_PEND_INT           0x503c

#define TSIO_REG_DQCT_TAB_ADDR(id)           (0x5050 + (id << 2))

typedef union {
    struct {
        unsigned int    dma_chnl_depth        : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    unsigned int    u32;
} U_DMA_CHNL_DEPTH;

#define TSIO_REG_DMA_CHNL_DEPTH(id)           (0x50c0 + (id << 2))

typedef union {
    struct {
        unsigned int    dma_chnl_pace         : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 24  ; /* [31..8]  */
    } bits;

    unsigned int    u32;
} U_DMA_CHNL_PACE;

#define TSIO_REG_DMA_CHNL_PACE(id)            (0x5140 + (id << 2))

#define TSIO_REG_DQCT_TAB_ADDR_SESSION_ID(id) (0x51a0 + (id << 2))

typedef union {
    struct {
        unsigned int    hw_pi_rptr            : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    hw_pi_wptr            : 10  ; /* [25..16]  */
        unsigned int    reserved_1            : 6   ; /* [31..26]  */
    } bits;

    unsigned int    u32;
} U_DMA_SLOT_PI_R;

#define TSIO_REG_DMA_SLOT_PI_R(id)            (0x5200 + (id << 2))

typedef union {
    struct {
        unsigned int    sw_pi_wptr            : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    sw_pi_w_bid           : 6   ; /* [21..16]  */
        unsigned int    reserved_1            : 10  ; /* [31..22]  */
    } bits;

    unsigned int    u32;
} U_DMA_SLOT_PI_W;

#define TSIO_REG_DMA_SLOT_PI_W                0x5280

typedef union {
    struct {
        unsigned int    hw_po_wptr            : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 7   ; /* [30..24]  */
        unsigned int    po_wptr_loopback      : 1   ; /* [31]  */
    } bits;

    unsigned int    u32;
} U_DMA_SLOT_PO_W;

#define TSIO_REG_DMA_SLOT_PO_W(id)             (0x5300 + (id << 2))

typedef union {
    struct {
        unsigned int    sw_po_rptr            : 24  ; /* [23..0]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    unsigned int    u32;
} U_DMA_SLOT_PO_R;

#define TSIO_REG_DMA_SLOT_PO_R(id)            (0x5400 + (id << 2))

#define TSIO_REG_DMA_OBUF_ADDR(id)            (0x5500 + (id << 2))

typedef union {
    struct {
        unsigned int    dma_obuf_length       : 28  ; /* [27..0]  */
        unsigned int    reserved_0            : 4   ; /* [31..28]  */
    } bits;

    unsigned int    u32;
} U_DMA_OBUF_LEN;

#define TSIO_REG_DMA_OBUF_LEN(id)             (0x5600 + (id << 2))

typedef union {
    struct {
        unsigned int    dma_obuf_thresh       : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    unsigned int    u32;
} U_DMA_OBUF_THRED;

#define TSIO_REG_DMA_OBUF_THRED(id)           (0x5700 + (id << 2))

#define TSIO_REG_DMA_OBUF_ENB_L               0x5808
#define TSIO_REG_DMA_OBUF_ENB_H               0x580c

#define TSIO_REG_DMA_OBUF_DIS_L               0x5810
#define TSIO_REG_DMA_OBUF_DIS_H               0x5814

#define TSIO_REG_DMA_PACK_INT_L               0x5820
#define TSIO_REG_DMA_PACK_INT_H               0x5824

#define TSIO_REG_DMA_END_INT_L                0x5828
#define TSIO_REG_DMA_END_INT_H                0x582c

typedef union {
    struct {
        unsigned int    coal_time_cyc         : 16  ; /* [15..0]  */
        unsigned int    coal_cnt_nums         : 16  ; /* [31..16]  */
    } bits;

    unsigned int    u32;
} U_DMA_COAL_CFG;

#define TSIO_REG_DMA_COAL_CFG                 0x5830

#define TSIO_REG_DMA_FLUSH_L                  0x5838
#define TSIO_REG_DMA_FLUSH_H                  0x583c

#define TSIO_REG_DMA_OBUF_OVFLW_L             0x5840
#define TSIO_REG_DMA_OBUF_OVFLW_H             0x5844

#define TSIO_REG_DMA_MMU_DIS_L                0x5850
#define TSIO_REG_DMA_MMU_DIS_H                0x5854

typedef union {
    struct {
        unsigned int pulse_cyc                  : 16 ; /* [15 .. 0] */
        unsigned int reserved                   : 16 ; /* [31 .. 16] */
    } bits;

    unsigned int u32;
} U_DMA_CNT_UNIT;

#define TSIO_REG_DMA_CNT_UNIT               (0x5910)

#define TSIO_REG_DMA_OBUF_ADDR_HIGH(id)      (0x5a00 + (id << 2))

/* c000 : from tsio_phy_spec.pdf */
typedef union {
    struct {
        unsigned int phy_rst_n                : 1   ; /* [0] */
        unsigned int phy_ready                : 1   ; /* [1] */
        unsigned int tx_rst_n                 : 1   ; /* [2] */
        unsigned int rx_rst_n                 : 1   ; /* [3] */
        unsigned int init_start               : 1   ; /* [4] */
        unsigned int init_finish              : 1   ; /* [5] */
        unsigned int init_fail_status         : 2   ; /* [7..6] */
        unsigned int pwon_pll                 : 1   ; /* [8] */
        unsigned int pwon_clk                 : 1   ; /* [9] */
        unsigned int pwon_data                : 1   ; /* [10] */
        unsigned int pwon_rx                  : 1   ; /* [11] */
        unsigned int power_manual             : 1   ; /* [12] */
        unsigned int reserved                 : 19  ; /* [31.. 13] */
    } bits;

    unsigned int u32;
} U_PHY_CTRL;

#define TSIO_REG_PHY_CTRL                     0x10004

typedef union {
    struct {
        unsigned int tx_soft_rst_n            : 1   ; /* [0] */
        unsigned int tx_rst_ctrl              : 1   ; /* [1] */
        unsigned int rx_soft_rst_n            : 1   ; /* [2] */
        unsigned int rx_rst_ctrl              : 1   ; /* [3] */
        unsigned int training_finish          : 1   ; /* [4] */
        unsigned int training_error           : 1   ; /* [5] */
        unsigned int latch_training_status    : 1   ; /* [6] */
        unsigned int training_pattern_received : 1   ; /* [7] */
        unsigned int sync_finish              : 1   ; /* [8] */
        unsigned int sync_error               : 1   ; /* [9] */
        unsigned int latch_sync_status        : 1   ; /* [10] */
        unsigned int sync_ready               : 1   ; /* [11] */
        unsigned int sync_det                 : 1   ; /* [12] */
        unsigned int sync_stage               : 1   ; /* [13] */
        unsigned int reserved                 : 18  ; /* [31..14] */
    } bits;

    unsigned int u32;
} U_PHY_INIT_REG;

#define TSIO_REG_PHY_INIT_REG                 0x10008

typedef union {
    struct {
        unsigned int sync_time               : 28   ; /* [27..0] */
        unsigned int sync_cnt                : 4    ; /* [31..28] */
    } bits;

    unsigned int u32;
} U_PHY_SYNC_LIMIT;

#define TSIO_REG_PHY_SYNC_LIMIT              0x10010

typedef union {
    struct {
        unsigned int internal_loopback        : 1   ; /* [0] */
        unsigned int rx_to_tx_lb               : 1   ; /* [1] */
        unsigned int pattern_sel              : 2   ; /* [3..2] */
        unsigned int bist_finish              : 1   ; /* [4] */
        unsigned int bist_fail                : 1   ; /* [5] */
        unsigned int reserved_0               : 26  ; /* [31..6] */
    } bits;

    unsigned int u32;
} U_PHY_BIST_REG;

#define TSIO_REG_PHY_BIST_REG                 0x1001c

typedef union {
    struct {
        unsigned int offset_ctl               : 3   ; /* [2..0] */
        unsigned int reserved                 : 29  ; /* [31..3] */
    } bits;

    unsigned int u32;
} U_PHY_OFFSET_CTL;

#define TSIO_REG_PHY_OFFSET_CTL               0x10020

typedef union {
    struct {
        unsigned int swing_ctl                : 3   ; /* [2..0] */
        unsigned int reserved                 : 29  ; /* [31..3] */
    } bits;

    unsigned int u32;
} U_PHY_SWING_CTL;

#define TSIO_REG_PHY_SWING_CTL               0x10024

typedef union {
    struct {
        unsigned int pre_emphasis             : 3   ; /* [2..0] */
        unsigned int reserved                 : 29  ; /* [31..3] */
    } bits;

    unsigned int u32;
} U_PHY_PRE_EMPHASIS;

#define TSIO_REG_PHY_PRE_EMPHASIS             0x10028

typedef union {
    struct {
        unsigned int slew_ctl                 : 2   ; /* [1..0] */
        unsigned int reserved                 : 30  ; /* [31..2] */
    } bits;

    unsigned int u32;
} U_PHY_SLEW_CTL;

#define TSIO_REG_PHY_SLEW_CTL                 0x1002c

typedef union {
    struct {
        unsigned int skew_ctl                 : 3   ; /* [2..0] */
        unsigned int swap_clock_output        : 1   ; /* [3] */
        unsigned int reserved                 : 28  ; /* [31..4] */
    } bits;

    unsigned int u32;
} U_PHY_CLK_DATA_SKEW;

#define TSIO_REG_PHY_CLK_DATA_SKEW            0x10030

typedef union {
    struct {
        unsigned int lock_cnt                 : 12  ; /* [11..0] */
        unsigned int reserved                 : 20  ; /* [31..12] */
    } bits;

    unsigned int u32;
} U_PHY_PLL_LOCK_CTL;

#define TSIO_REG_PHY_PLL_LOCK_CTL             0x10034

typedef union {
    struct {
        unsigned int int_mask                 : 1   ; /* [0] */
        unsigned int bypass_pll_lock          : 1   ; /* [1] */
        unsigned int invert_rx_clk            : 1   ; /* [2] */
        unsigned int invert_tx_clk            : 1   ; /* [3] */
        unsigned int invert_pll_clk           : 1   ; /* [4] */
        unsigned int data_tx                  : 1   ; /* [5] */
        unsigned int pll_lock                 : 1   ; /* [6] */
        unsigned int pll_clk                  : 1   ; /* [7] */
        unsigned int bit_index                : 3   ; /* [10..8] */
        unsigned int reserved                 : 21  ; /* [31..11] */
    } bits;

    unsigned int u32;
} U_PHY_MISC;

#define TSIO_REG_PHY_MISC                     0x1003c

typedef union {
    struct {
        unsigned int resync_en          : 1;  /* [0] */
        unsigned int resered            : 31; /* [31..1] */
    } bits;

    unsigned int u32;
} U_PHY_RESYNC_CTRL;

#define TSIO_REG_PHY_RESYNC_CTRL    0x10040

#define TSIO_REG_PHY_DESYNC_CNT     0x10048

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_TSIO_REG_H__

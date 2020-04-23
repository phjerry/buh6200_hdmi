/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi tx reg module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __MIPI_TX_REG_H__
#define __MIPI_TX_REG_H__

/* define the union mipi_tx_pwr_up */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    pwr_up                : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_pwr_up;

/* define the union mipi_tx_clkmgr_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    tx_esc_clk_division   : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 8   ; /* [15..8]  */
        unsigned int    reserved_1            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_clkmgr_cfg;

/* define the union mipi_tx_vcid */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vcid                  : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vcid;

/* define the union mipi_tx_color_coding */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    color_coding          : 4   ; /* [3..0]  */
        unsigned int    reserved_0            : 4   ; /* [7..4]  */
        unsigned int    loosely18_en          : 1   ; /* [8]  */
        unsigned int    reserved_1            : 23  ; /* [31..9]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_color_coding;

/* define the union mipi_tx_lp_cmd_tim */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    invact_lpcmd_time     : 8   ; /* [7..0]  */
        unsigned int    reserved_0            : 8   ; /* [15..8]  */
        unsigned int    outvact_lpcmd_time    : 8   ; /* [23..16]  */
        unsigned int    reserved_1            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_lp_cmd_tim;

/* define the union mipi_tx_pckhdl_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    eotp_tx_en            : 1   ; /* [0]  */
        unsigned int    eotp_rx_en            : 1   ; /* [1]  */
        unsigned int    bta_en                : 1   ; /* [2]  */
        unsigned int    ecc_rx_en             : 1   ; /* [3]  */
        unsigned int    crc_rx_en             : 1   ; /* [4]  */
        unsigned int    reserved_0            : 27  ; /* [31..5]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_pckhdl_cfg;

/* define the union mipi_tx_gen_vcid */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    gen_vcid_rx           : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_gen_vcid;

/* define the union mipi_tx_mode_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    cmd_video_mode        : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_mode_cfg;

/* define the union mipi_tx_vid_mode_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_mode_type         : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    lp_vsa_en             : 1   ; /* [8]  */
        unsigned int    lp_vbp_en             : 1   ; /* [9]  */
        unsigned int    lp_vfp_en             : 1   ; /* [10]  */
        unsigned int    lp_vact_en            : 1   ; /* [11]  */
        unsigned int    lp_hbp_en             : 1   ; /* [12]  */
        unsigned int    lp_hfp_en             : 1   ; /* [13]  */
        unsigned int    frame_bta_ack_en      : 1   ; /* [14]  */
        unsigned int    lp_cmd_en             : 1   ; /* [15]  */
        unsigned int    vpg_en                : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    vpg_mode              : 1   ; /* [20]  */
        unsigned int    reserved_2            : 3   ; /* [23..21]  */
        unsigned int    vpg_orientation       : 1   ; /* [24]  */
        unsigned int    reserved_3            : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_mode_cfg;

/* define the union mipi_tx_vid_pkt_size */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_pkt_size          : 14  ; /* [13..0]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_pkt_size;

/* define the union mipi_tx_vid_num_chunks */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_num_chunks        : 13  ; /* [12..0]  */
        unsigned int    reserved_0            : 19  ; /* [31..13]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_num_chunks;

/* define the union mipi_tx_vid_null_size */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_null_size         : 13  ; /* [12..0]  */
        unsigned int    reserved_0            : 19  ; /* [31..13]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_null_size;

/* define the union mipi_tx_vid_hsa_time */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_hsa_time          : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_hsa_time;

/* define the union mipi_tx_vid_hbp_time */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_hbp_time          : 12  ; /* [11..0]  */
        unsigned int    reserved_0            : 20  ; /* [31..12]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_hbp_time;

/* define the union mipi_tx_vid_hline_time */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vid_hline_time        : 15  ; /* [14..0]  */
        unsigned int    reserved_0            : 17  ; /* [31..15]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_hline_time;

/* define the union mipi_tx_vid_vsa_lines */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vsa_lines             : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_vsa_lines;

/* define the union mipi_tx_vid_vbp_lines */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vbp_lines             : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_vbp_lines;

/* define the union mipi_tx_vid_vfp_lines */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vfp_lines             : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_vfp_lines;

/* define the union mipi_tx_vid_vactive_lines */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    v_active_lines        : 14  ; /* [13..0]  */
        unsigned int    reserved_0            : 18  ; /* [31..14]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vid_vactive_lines;

/* define the union mipi_tx_edpi_cmd_size */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    edpi_allowed_cmd_size : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_edpi_cmd_size;

/* define the union mipi_tx_cmd_mode_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 1   ; /* [0]  */
        unsigned int    reserved_1            : 1   ; /* [1]  */
        unsigned int    reserved_2            : 6   ; /* [7..2]  */
        unsigned int    gen_sw_0p_tx          : 1   ; /* [8]  */
        unsigned int    gen_sw_1p_tx          : 1   ; /* [9]  */
        unsigned int    gen_sw_2p_tx          : 1   ; /* [10]  */
        unsigned int    gen_sr_0p_tx          : 1   ; /* [11]  */
        unsigned int    gen_sr_1p_tx          : 1   ; /* [12]  */
        unsigned int    gen_sr_2p_tx          : 1   ; /* [13]  */
        unsigned int    gen_lw_tx             : 1   ; /* [14]  */
        unsigned int    reserved_3            : 1   ; /* [15]  */
        unsigned int    dcs_sw_0p_tx          : 1   ; /* [16]  */
        unsigned int    dcs_sw_1p_tx          : 1   ; /* [17]  */
        unsigned int    dcs_sr_0p_tx          : 1   ; /* [18]  */
        unsigned int    dcs_lw_tx             : 1   ; /* [19]  */
        unsigned int    reserved_4            : 4   ; /* [23..20]  */
        unsigned int    max_rd_pkt_size       : 1   ; /* [24]  */
        unsigned int    reserved_5            : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_cmd_mode_cfg;

/* define the union mipi_tx_gen_hdr */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    gen_dt                : 6   ; /* [5..0]  */
        unsigned int    gen_vc                : 2   ; /* [7..6]  */
        unsigned int    gen_wc_lsbyte         : 8   ; /* [15..8]  */
        unsigned int    gen_wc_msbyte         : 8   ; /* [23..16]  */
        unsigned int    reserved_0            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_gen_hdr;

/* define the union mipi_tx_gen_pld_data */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    gen_pld_b1            : 8   ; /* [7..0]  */
        unsigned int    gen_pld_b2            : 8   ; /* [15..8]  */
        unsigned int    gen_pld_b3            : 8   ; /* [23..16]  */
        unsigned int    gen_pld_b4            : 8   ; /* [31..24]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_gen_pld_data;

/* define the union mipi_tx_cmd_pkt_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    gen_cmd_empty         : 1   ; /* [0]  */
        unsigned int    gen_cmd_full          : 1   ; /* [1]  */
        unsigned int    gen_pld_w_empty       : 1   ; /* [2]  */
        unsigned int    gen_pld_w_full        : 1   ; /* [3]  */
        unsigned int    gen_pld_r_empty       : 1   ; /* [4]  */
        unsigned int    gen_pld_r_full        : 1   ; /* [5]  */
        unsigned int    gen_rd_cmd_busy       : 1   ; /* [6]  */
        unsigned int    reserved_0            : 25  ; /* [31..7]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_cmd_pkt_status;

/* define the union mipi_tx_lp_wr_to_cnt */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lp_wr_to_cnt          : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_lp_wr_to_cnt;

/* define the union mipi_tx_bta_to_cnt */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    bta_to_cnt            : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_bta_to_cnt;

/* define the union mipi_tx_lpclk_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_txrequestclkhs    : 1   ; /* [0]  */
        unsigned int    auto_clklane_ctrl     : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_lpclk_ctrl;

/* define the union mipi_tx_phy_tmr_lpclk_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_clklp2hs_time     : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    phy_clkhs2lp_time     : 10  ; /* [25..16]  */
        unsigned int    reserved_1            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_tmr_lpclk_cfg;

/* define the union mipi_tx_phy_tmr_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_lp2hs_time        : 10  ; /* [9..0]  */
        unsigned int    reserved_0            : 6   ; /* [15..10]  */
        unsigned int    phy_hs2lp_time        : 10  ; /* [25..16]  */
        unsigned int    reserved_1            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_tmr_cfg;

/* define the union mipi_tx_phy_rstz */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_shutdownz         : 1   ; /* [0]  */
        unsigned int    phy_rstz              : 1   ; /* [1]  */
        unsigned int    phy_enableclk         : 1   ; /* [2]  */
        unsigned int    phy_forcepll          : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_rstz;

/* define the union mipi_tx_phy_if_cfg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    n_lanes               : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 6   ; /* [7..2]  */
        unsigned int    phy_stop_wait_time    : 8   ; /* [15..8]  */
        unsigned int    reserved_1            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_if_cfg;

/* define the union mipi_tx_phy_ulps_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_txrequlpsclk      : 1   ; /* [0]  */
        unsigned int    phy_txexitulpsclk     : 1   ; /* [1]  */
        unsigned int    phy_txrequlpslan      : 1   ; /* [2]  */
        unsigned int    phy_txexitulpslan     : 1   ; /* [3]  */
        unsigned int    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_ulps_ctrl;

/* define the union mipi_tx_phy_status */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_lock               : 1   ; /* [0]  */
        unsigned int    phy_direction          : 1   ; /* [1]  */
        unsigned int    phy_stopstateclklane   : 1   ; /* [2]  */
        unsigned int    phy_ulpsactivenotclk   : 1   ; /* [3]  */
        unsigned int    phy_stopstate0lane     : 1   ; /* [4]  */
        unsigned int    phy_ulpsactivenot0lane : 1   ; /* [5]  */
        unsigned int    phy_rxulpsesc0lane     : 1   ; /* [6]  */
        unsigned int    phy_stopstate1lane     : 1   ; /* [7]  */
        unsigned int    phy_ulpsactivenot1lane : 1   ; /* [8]  */
        unsigned int    phy_stopstate2lane     : 1   ; /* [9]  */
        unsigned int    phy_ulpsactivenot2lane : 1   ; /* [10]  */
        unsigned int    phy_stopstate3lane     : 1   ; /* [11]  */
        unsigned int    phy_ulpsactivenot3lane : 1   ; /* [12]  */
        unsigned int    reserved_0             : 19  ; /* [31..13]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_status;

/* define the union mipi_tx_phy_tst_ctrl0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_testclr           : 1   ; /* [0]  */
        unsigned int    phy_testclk           : 1   ; /* [1]  */
        unsigned int    reserved_0            : 30  ; /* [31..2]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_tst_ctrl0;

/* define the union mipi_tx_phy_tst_ctrl1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    phy_testdin           : 8   ; /* [7..0]  */
        unsigned int    phy_testdout          : 8   ; /* [15..8]  */
        unsigned int    phy_testen            : 1   ; /* [16]  */
        unsigned int    reserved_0            : 15  ; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_tst_ctrl1;

/* define the union mipi_tx_int_st0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    ack_with_err_0        : 1   ; /* [0]  */
        unsigned int    ack_with_err_1        : 1   ; /* [1]  */
        unsigned int    ack_with_err_2        : 1   ; /* [2]  */
        unsigned int    ack_with_err_3        : 1   ; /* [3]  */
        unsigned int    ack_with_err_4        : 1   ; /* [4]  */
        unsigned int    ack_with_err_5        : 1   ; /* [5]  */
        unsigned int    ack_with_err_6        : 1   ; /* [6]  */
        unsigned int    ack_with_err_7        : 1   ; /* [7]  */
        unsigned int    ack_with_err_8        : 1   ; /* [8]  */
        unsigned int    ack_with_err_9        : 1   ; /* [9]  */
        unsigned int    ack_with_err_10       : 1   ; /* [10]  */
        unsigned int    ack_with_err_11       : 1   ; /* [11]  */
        unsigned int    ack_with_err_12       : 1   ; /* [12]  */
        unsigned int    ack_with_err_13       : 1   ; /* [13]  */
        unsigned int    ack_with_err_14       : 1   ; /* [14]  */
        unsigned int    ack_with_err_15       : 1   ; /* [15]  */
        unsigned int    dphy_errors_0         : 1   ; /* [16]  */
        unsigned int    dphy_errors_1         : 1   ; /* [17]  */
        unsigned int    dphy_errors_2         : 1   ; /* [18]  */
        unsigned int    dphy_errors_3         : 1   ; /* [19]  */
        unsigned int    dphy_errors_4         : 1   ; /* [20]  */
        unsigned int    reserved_0            : 11  ; /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_int_st0;

/* define the union mipi_tx_int_st1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    to_hs_tx              : 1   ; /* [0]  */
        unsigned int    to_lp_rx              : 1   ; /* [1]  */
        unsigned int    ecc_single_err        : 1   ; /* [2]  */
        unsigned int    ecc_multi_err         : 1   ; /* [3]  */
        unsigned int    crc_err               : 1   ; /* [4]  */
        unsigned int    pkt_size_err          : 1   ; /* [5]  */
        unsigned int    eopt_err              : 1   ; /* [6]  */
        unsigned int    dpi_pld_wr_err        : 1   ; /* [7]  */
        unsigned int    gen_cmd_wr_err        : 1   ; /* [8]  */
        unsigned int    gen_pld_wr_err        : 1   ; /* [9]  */
        unsigned int    gen_pld_send_err      : 1   ; /* [10]  */
        unsigned int    gen_pld_rd_err        : 1   ; /* [11]  */
        unsigned int    gen_pld_recev_err     : 1   ; /* [12]  */
        unsigned int    reserved_0            : 3   ; /* [15..13]  */
        unsigned int    vsstart               : 1   ; /* [16]  */
        unsigned int    reserved_1            : 3   ; /* [19..17]  */
        unsigned int    rxtrigger_0           : 1   ; /* [20]  */
        unsigned int    rxtrigger_1           : 1   ; /* [21]  */
        unsigned int    rxtrigger_2           : 1   ; /* [22]  */
        unsigned int    rxtrigger_3           : 1   ; /* [23]  */
        unsigned int    hss_abnormal          : 1   ; /* [24]  */
        unsigned int    reserved_2            : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_int_st1;

/* define the union mipi_tx_int_msk0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    mask_ack_with_err_0   : 1   ; /* [0]  */
        unsigned int    mask_ack_with_err_1   : 1   ; /* [1]  */
        unsigned int    mask_ack_with_err_2   : 1   ; /* [2]  */
        unsigned int    mask_ack_with_err_3   : 1   ; /* [3]  */
        unsigned int    mask_ack_with_err_4   : 1   ; /* [4]  */
        unsigned int    mask_ack_with_err_5   : 1   ; /* [5]  */
        unsigned int    mask_ack_with_err_6   : 1   ; /* [6]  */
        unsigned int    mask_ack_with_err_7   : 1   ; /* [7]  */
        unsigned int    mask_ack_with_err_8   : 1   ; /* [8]  */
        unsigned int    mask_ack_with_err_9   : 1   ; /* [9]  */
        unsigned int    mask_ack_with_err_10  : 1   ; /* [10]  */
        unsigned int    mask_ack_with_err_11  : 1   ; /* [11]  */
        unsigned int    mask_ack_with_err_12  : 1   ; /* [12]  */
        unsigned int    mask_ack_with_err_13  : 1   ; /* [13]  */
        unsigned int    mask_ack_with_err_14  : 1   ; /* [14]  */
        unsigned int    mask_ack_with_err_15  : 1   ; /* [15]  */
        unsigned int    mask_dphy_errors_0    : 1   ; /* [16]  */
        unsigned int    mask_dphy_errors_1    : 1   ; /* [17]  */
        unsigned int    mask_dphy_errors_2    : 1   ; /* [18]  */
        unsigned int    mask_dphy_errors_3    : 1   ; /* [19]  */
        unsigned int    mask_dphy_errors_4    : 1   ; /* [20]  */
        unsigned int    reserved_0            : 11  ; /* [31..21]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_int_msk0;

/* define the union mipi_tx_int_msk1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    mask_to_hs_tx          : 1   ; /* [0]  */
        unsigned int    mask_to_lp_rx          : 1   ; /* [1]  */
        unsigned int    mask_ecc_single_err    : 1   ; /* [2]  */
        unsigned int    mask_ecc_multi_err     : 1   ; /* [3]  */
        unsigned int    mask_crc_err           : 1   ; /* [4]  */
        unsigned int    mask_pkt_size_err      : 1   ; /* [5]  */
        unsigned int    mask_eopt_err          : 1   ; /* [6]  */
        unsigned int    mask_dpi_pld_wr_err    : 1   ; /* [7]  */
        unsigned int    mask_gen_cmd_wr_err    : 1   ; /* [8]  */
        unsigned int    mask_gen_pld_wr_err    : 1   ; /* [9]  */
        unsigned int    mask_gen_pld_send_err  : 1   ; /* [10]  */
        unsigned int    mask_gen_pld_rd_err    : 1   ; /* [11]  */
        unsigned int    mask_gen_pld_recev_err : 1   ; /* [12]  */
        unsigned int    reserved_0             : 3   ; /* [15..13]  */
        unsigned int    mask_vsstart           : 1   ; /* [16]  */
        unsigned int    reserved_1             : 3   ; /* [19..17]  */
        unsigned int    mask_trigger_0         : 1   ; /* [20]  */
        unsigned int    mask_trigger_1         : 1   ; /* [21]  */
        unsigned int    mask_trigger_2         : 1   ; /* [22]  */
        unsigned int    mask_trigger_3         : 1   ; /* [23]  */
        unsigned int    mask_hss_abnormal      : 1   ; /* [24]  */
        unsigned int    reserved_2             : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_int_msk1;

/* define the union mipi_tx_phy_cal */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    txskewcalhs           : 1   ; /* [0]  */
        unsigned int    reserved_0            : 31  ; /* [31..1]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_phy_cal;

/* define the union mipi_tx_int_force1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 1   ; /* [0]  */
        unsigned int    reserved_1            : 1   ; /* [1]  */
        unsigned int    reserved_2            : 1   ; /* [2]  */
        unsigned int    reserved_3            : 1   ; /* [3]  */
        unsigned int    reserved_4            : 1   ; /* [4]  */
        unsigned int    reserved_5            : 1   ; /* [5]  */
        unsigned int    reserved_6            : 1   ; /* [6]  */
        unsigned int    reserved_7            : 1   ; /* [7]  */
        unsigned int    reserved_8            : 1   ; /* [8]  */
        unsigned int    reserved_9            : 1   ; /* [9]  */
        unsigned int    reserved_10           : 1   ; /* [10]  */
        unsigned int    reserved_11           : 1   ; /* [11]  */
        unsigned int    reserved_12           : 1   ; /* [12]  */
        unsigned int    reserved_13           : 3   ; /* [15..13]  */
        unsigned int    reserved_14           : 1   ; /* [16]  */
        unsigned int    reserved_15           : 3   ; /* [19..17]  */
        unsigned int    reserved_16           : 1   ; /* [20]  */
        unsigned int    reserved_17           : 1   ; /* [21]  */
        unsigned int    reserved_18           : 1   ; /* [22]  */
        unsigned int    reserved_19           : 1   ; /* [23]  */
        unsigned int    reserved_20           : 1   ; /* [24]  */
        unsigned int    reserved_21           : 7   ; /* [31..25]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_int_force1;

/* define the union mipi_tx_operation_mode */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 1   ; /* [0]  */
        unsigned int    mem_ck_en             : 1   ; /* [1]  */
        unsigned int    reserved_1            : 2   ; /* [3..2]  */
        unsigned int    reserved_2            : 1   ; /* [4]  */
        unsigned int    init_skew_en          : 1   ; /* [5]  */
        unsigned int    period_skew_en        : 1   ; /* [6]  */
        unsigned int    reserved_3            : 1   ; /* [7]  */
        unsigned int    reserved_4            : 1   ; /* [8]  */
        unsigned int    reserved_5            : 1   ; /* [9]  */
        unsigned int    reserved_6            : 1   ; /* [10]  */
        unsigned int    reserved_7            : 1   ; /* [11]  */
        unsigned int    reserved_8            : 1   ; /* [12]  */
        unsigned int    reserved_9            : 1   ; /* [13]  */
        unsigned int    reserved_10           : 1   ; /* [14]  */
        unsigned int    reserved_11           : 1   ; /* [15]  */
        unsigned int    read_empty_vsync_en   : 1   ; /* [16]  */
        unsigned int    reserved_12           : 1   ; /* [17]  */
        unsigned int    buf_clr_en            : 1   ; /* [18]  */
        unsigned int    reserved_13           : 1   ; /* [19]  */
        unsigned int    hss_abnormal_rst      : 1   ; /* [20]  */
        unsigned int    reserved_14           : 3   ; /* [23..21]  */
        unsigned int    reserved_15           : 1   ; /* [24]  */
        unsigned int    reserved_16           : 1   ; /* [25]  */
        unsigned int    reserved_17           : 1   ; /* [26]  */
        unsigned int    reserved_18           : 1   ; /* [27]  */
        unsigned int    dsc_soft_rst          : 1   ; /* [28]  */
        unsigned int    reserved_19           : 2   ; /* [30..29]  */
        unsigned int    input_en              : 1   ; /* [31]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_operation_mode;

/* define the union mipi_tx_vert_det */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vact_det              : 16  ; /* [15..0]  */
        unsigned int    vall_det              : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vert_det;

/* define the union mipi_tx_hori0_det */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    hact_det              : 16  ; /* [15..0]  */
        unsigned int    hline_det             : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_hori0_det;

/* define the union mipi_tx_hori1_det */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    hsa_det               : 16  ; /* [15..0]  */
        unsigned int    hbp_det               : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_hori1_det;

/* define the union mipi_tx_vsa_det */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    vsa_det               : 16  ; /* [15..0]  */
        unsigned int    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_vsa_det;

/* define the union mipi_tx_v_h_send */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    h_send                : 16  ; /* [15..0]  */
        unsigned int    v_send                : 16  ; /* [31..16]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_v_h_send;

/* define the union mipi_tx_datatype0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    reserved_0            : 6   ; /* [5..0]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    dt_hss                : 6   ; /* [13..8]  */
        unsigned int    reserved_2            : 2   ; /* [15..14]  */
        unsigned int    dt_vse                : 6   ; /* [21..16]  */
        unsigned int    reserved_3            : 2   ; /* [23..22]  */
        unsigned int    dt_vss                : 6   ; /* [29..24]  */
        unsigned int    reserved_4            : 2   ; /* [31..30]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_datatype0;

/* define the union mipi_tx_csi_ctrl */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    csi_en                : 1   ; /* [0]  */
        unsigned int    reserved_0            : 3   ; /* [3..1]  */
        unsigned int    reserved_1            : 1   ; /* [4]  */
        unsigned int    reserved_2            : 3   ; /* [7..5]  */
        unsigned int    reserved_3            : 1   ; /* [8]  */
        unsigned int    reserved_4            : 3   ; /* [11..9]  */
        unsigned int    reserved_5            : 1   ; /* [12]  */
        unsigned int    reserved_6            : 3   ; /* [15..13]  */
        unsigned int    reserved_7            : 1   ; /* [16]  */
        unsigned int    reserved_8            : 15  ; /* [31..17]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_csi_ctrl;

/* define the union mipi_tx_skew_begin */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int skew_begin             : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_skew_begin;
/* define the union mipi_tx_skew_end */
typedef union {
    /* define the struct bits  */
    struct {
        unsigned int skew_end               : 32  ; /* [31..0]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_skew_end;
/* define the union mipi_tx_lane_id */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int    lane0_id              : 2   ; /* [1..0]  */
        unsigned int    reserved_0            : 2   ; /* [3..2]  */
        unsigned int    lane1_id              : 2   ; /* [5..4]  */
        unsigned int    reserved_1            : 2   ; /* [7..6]  */
        unsigned int    lane2_id              : 2   ; /* [9..8]  */
        unsigned int    reserved_2            : 2   ; /* [11..10]  */
        unsigned int    lane3_id              : 2   ; /* [13..12]  */
        unsigned int    reserved_3            : 18  ; /* [31..14]  */
    } bits;

    /* define an unsigned member */
    unsigned int    u32;
} mipi_tx_lane_id;

/* define the global struct */
typedef struct {
    unsigned int                  reserved_0        ; /* 0x0 */
    mipi_tx_pwr_up                pwr_up            ; /* 0x4 */
    mipi_tx_clkmgr_cfg            clkmgr_cfg        ; /* 0x8 */
    mipi_tx_vcid                  vcid              ; /* 0xc */
    mipi_tx_color_coding          color_coding      ; /* 0x10 */
    unsigned int                  reserved_1        ; /* 0x14 */
    mipi_tx_lp_cmd_tim            lp_cmd_tim        ; /* 0x18 */
    unsigned int                  reserved_2[4]     ; /* 0x1c~0x28 4 uint */
    mipi_tx_pckhdl_cfg            pckhdl_cfg        ; /* 0x2c */
    mipi_tx_gen_vcid              gen_vcid          ; /* 0x30 */
    mipi_tx_mode_cfg              mode_cfg          ; /* 0x34 */
    mipi_tx_vid_mode_cfg          vid_mode_cfg      ; /* 0x38 */
    mipi_tx_vid_pkt_size          vid_pkt_size      ; /* 0x3c */
    mipi_tx_vid_num_chunks        vid_num_chunks    ; /* 0x40 */
    mipi_tx_vid_null_size         vid_null_size     ; /* 0x44 */
    mipi_tx_vid_hsa_time          vid_hsa_time      ; /* 0x48 */
    mipi_tx_vid_hbp_time          vid_hbp_time      ; /* 0x4c */
    mipi_tx_vid_hline_time        vid_hline_time    ; /* 0x50 */
    mipi_tx_vid_vsa_lines         vid_vsa_lines     ; /* 0x54 */
    mipi_tx_vid_vbp_lines         vid_vbp_lines     ; /* 0x58 */
    mipi_tx_vid_vfp_lines         vid_vfp_lines     ; /* 0x5c */
    mipi_tx_vid_vactive_lines     vid_vactive_lines ; /* 0x60 */
    mipi_tx_edpi_cmd_size         edpi_cmd_size     ; /* 0x64 */
    mipi_tx_cmd_mode_cfg          cmd_mode_cfg      ; /* 0x68 */
    mipi_tx_gen_hdr               gen_hdr           ; /* 0x6c */
    mipi_tx_gen_pld_data          gen_pld_data      ; /* 0x70 */
    mipi_tx_cmd_pkt_status        cmd_pkt_status    ; /* 0x74 */
    unsigned int                  reserved_3[4]     ; /* 0x78~0x84 */
    mipi_tx_lp_wr_to_cnt          lp_wr_to_cnt      ; /* 0x88 */
    mipi_tx_bta_to_cnt            bta_to_cnt        ; /* 0x8c */
    unsigned int                  reserved_4        ; /* 0x90 */
    mipi_tx_lpclk_ctrl            lpclk_ctrl        ; /* 0x94 */
    mipi_tx_phy_tmr_lpclk_cfg     phy_tmr_lpclk_cfg ; /* 0x98 */
    mipi_tx_phy_tmr_cfg           phy_tmr_cfg       ; /* 0x9c */
    mipi_tx_phy_rstz              phy_rstz          ; /* 0xa0 */
    mipi_tx_phy_if_cfg            phy_if_cfg        ; /* 0xa4 */
    mipi_tx_phy_ulps_ctrl         phy_ulps_ctrl     ; /* 0xa8 */
    unsigned int                  reserved_5        ; /* 0xac */
    mipi_tx_phy_status            phy_status        ; /* 0xb0 */
    mipi_tx_phy_tst_ctrl0         phy_tst_ctrl0     ; /* 0xb4 */
    mipi_tx_phy_tst_ctrl1         phy_tst_ctrl1     ; /* 0xb8 */
    mipi_tx_int_st0               int_st0           ; /* 0xbc */
    mipi_tx_int_st1               int_st1           ; /* 0xc0 */
    mipi_tx_int_msk0              int_msk0          ; /* 0xc4 */
    mipi_tx_int_msk1              int_msk1          ; /* 0xc8 */
    mipi_tx_phy_cal               phy_cal           ; /* 0xcc */
    unsigned int                  reserved_6[3]     ; /* 0xd0~0xd8 3 uint */
    mipi_tx_int_force1            int_force1        ; /* 0xdc */
    unsigned int                  reserved_7[74]    ; /* 0xe0~0x204 74 uint */
    mipi_tx_operation_mode        operation_mode    ; /* 0x208 */
    unsigned int                  reserved_8[2]     ; /* 0x20c~0x210 */
    volatile mipi_tx_vert_det     vert_det          ; /* 0x214 */
    volatile mipi_tx_hori0_det    hori0_det         ; /* 0x218 */
    volatile mipi_tx_hori1_det    hori1_det         ; /* 0x21c */
    volatile mipi_tx_vsa_det      vsa_det           ; /* 0x220 */
    mipi_tx_v_h_send              v_h_send          ; /* 0x224 */
    unsigned int                  reserved_9[2]     ; /* 0x228~0x22c */
    mipi_tx_datatype0             datatype0         ; /* 0x230 */
    unsigned int                  datatype1         ; /* 0x234 */
    mipi_tx_csi_ctrl              csi_ctrl          ; /* 0x238 */
    mipi_tx_skew_begin            skew_begin        ; /* 0x23c */
    mipi_tx_skew_end              skew_end          ; /* 0x240 */
    unsigned int                  reserved_11[3]    ; /* 0x244~0x24c 3 uint */
    mipi_tx_lane_id               lane_id           ; /* 0x250 */
} mipi_tx_regs_type;

#endif /* __mipi_tx_reg_h__ */

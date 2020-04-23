/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi dsc reg module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __MIPI_DSC_REG_H__
#define __MIPI_DSC_REG_H__

#include "hi_type.h"

typedef union {
    struct {
        hi_u32    dsc_enable            : 1  ; /* [0]  */
        hi_u32    dsc_ck_gt_en          : 1  ; /* [1]  */
        hi_u32    timing_polarity       : 1  ; /* [2]  */
        hi_u32    reserved_0            : 1  ; /* [3]  */
        hi_u32    dsc_delay             : 12 ; /* [15..4]  */
        hi_u32    reserved_1            : 16 ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_ctrl;

typedef union {
    struct {
        hi_u32    dsc_frm_done_sta      : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    enc0_rc_err_sta       : 1   ; /* [4]  */
        hi_u32    reserved_1            : 27  ; /* [31..5]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_int_sta;

typedef union {
    struct {
        hi_u32    dsc_frm_done_mask     : 1   ; /* [0]  */
        hi_u32    reserved_0            : 3   ; /* [3..1]  */
        hi_u32    enc0_rc_err_mask      : 1   ; /* [4]  */
        hi_u32    reserved_1            : 27  ; /* [31..5]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_int_mask;

typedef union {
    struct {
        hi_u32    rcb_bits              : 16   ; /* [15..0]  */
        hi_u32    reserved_0            : 16   ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_bits;

typedef union {
    struct {
        hi_u32    full_ich_err_precision : 1   ; /* [0]  */
        hi_u32    block_pred_enable      : 1   ; /* [1]  */
        hi_u32    reserved_0             : 30   ; /* [31..2]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_alg_ctrl;

typedef union {
    struct {
        hi_u32    bits_per_component    : 4   ; /* [3..0]  */
        hi_u32    reserved_0            : 28  ; /* [31..4]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_bpc;

typedef union {
    struct {
        hi_u32    convert_rgb           : 1   ; /* [0]  */
        hi_u32    native_420            : 1   ; /* [1]  */
        hi_u32    native_422            : 1   ; /* [2]  */
        hi_u32    reserved_0            : 29  ; /* [31..3]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_sample;

typedef union {
    struct {
        hi_u32    bits_per_pixel        : 10   ; /* [9..0]  */
        hi_u32    reserved_0            : 6    ; /* [15..10]  */
        hi_u32    chunk_size            : 16   ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_bpp_chk;

typedef union {
    struct {
        hi_u32    pic_width             : 16   ; /* [15..0]  */
        hi_u32    pic_height            : 16   ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_pic_reso;

typedef union {
    struct {
        hi_u32    slice_width           : 16  ; /* [15..0]  */
        hi_u32    slice_height          : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_slc_reso;

typedef union {
    struct {
        hi_u32    initial_xmit_delay    : 10  ; /* [9..0]  */
        hi_u32    reserved_0            : 22  ; /* [31..10]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_init_xmit_dly;

typedef union {
    struct {
        hi_u32    initial_dec_delay     : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_init_dec_dly;

typedef union {
    struct {
        hi_u32    initial_scale_value   : 6   ; /* [5..0]  */
        hi_u32    reserved_0            : 26  ; /* [31..6]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_init_scale;

typedef union {
    struct {
        hi_u32    scale_decrement_interval  : 12  ; /* [11..0]  */
        hi_u32    reserved_0                : 4   ; /* [15..12]  */
        hi_u32    scale_increment_interval  : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_scale_intval;

typedef union {
    struct {
        hi_u32    nfl_bpg_offset        : 16  ; /* [15..0]  */
        hi_u32    first_line_bpg_ofs    : 5   ; /* [20..16]  */
        hi_u32    reserved_0            : 11  ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_first_bpg;

typedef union {
    struct {
        hi_u32    nsl_bpg_offset        : 16  ; /* [15..0]  */
        hi_u32    second_line_bpg_ofs   : 5   ; /* [20..16]  */
        hi_u32    reserved_0            : 11  ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_second_bpg;

typedef union {
    struct {
        hi_u32    second_line_ofs_adj   : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_second_adj;

typedef union {
    struct {
        hi_u32    final_offset          : 16  ; /* [15..0]  */
        hi_u32    initial_offset        : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_init_finl_ofs;

typedef union {
    struct {
        hi_u32    slice_bpg_offset      : 16  ; /* [15..0]  */
        hi_u32    reserved_0            : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_slc_bpg;

typedef union {
    struct {
        hi_u32    flatness_max_qp       : 5   ; /* [4..0]  */
        hi_u32    reserved_0            : 3   ; /* [7..5]  */
        hi_u32    flatness_min_qp       : 5   ; /* [12..8]  */
        hi_u32    reserved_1            : 19  ; /* [31..13]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_flat_range;

typedef union {
    struct {
        hi_u32    rc_edge_factor        : 4   ; /* [3..0]  */
        hi_u32    reserved_0            : 12  ; /* [15..4]  */
        hi_u32    rc_model_size         : 16  ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_mod_edge;

typedef union {
    struct {
        hi_u32    rc_quant_incr_limit0  : 5   ; /* [4..0]  */
        hi_u32    reserved_0            : 3   ; /* [7..5]  */
        hi_u32    rc_quant_incr_limit1  : 5   ; /* [12..8]  */
        hi_u32    reserved_1            : 3   ; /* [15..13]  */
        hi_u32    rc_tgt_offset_hi      : 4   ; /* [19..16]  */
        hi_u32    rc_tgt_offset_lo      : 4   ; /* [23..20]  */
        hi_u32    reserved_2            : 8   ; /* [31..24]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_qua_tgt;

typedef union {
    struct {
        hi_u32    rc_buf_thresh_00      : 8  ; /* [7..0]  */
        hi_u32    rc_buf_thresh_01      : 8  ; /* [15..8]  */
        hi_u32    rc_buf_thresh_02      : 8  ; /* [23..16]  */
        hi_u32    rc_buf_thresh_03      : 8  ; /* [31..24]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_thre_01;

typedef union {
    struct {
        hi_u32    rc_buf_thresh_04      : 8  ; /* [7..0]  */
        hi_u32    rc_buf_thresh_05      : 8  ; /* [15..8]  */
        hi_u32    rc_buf_thresh_06      : 8  ; /* [23..16]  */
        hi_u32    rc_buf_thresh_07      : 8  ; /* [31..24]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_thre_02;

typedef union {
    struct {
        hi_u32    rc_buf_thresh_08      : 8  ; /* [7..0]  */
        hi_u32    rc_buf_thresh_09      : 8  ; /* [15..8]  */
        hi_u32    rc_buf_thresh_10      : 8  ; /* [23..16]  */
        hi_u32    rc_buf_thresh_11      : 8  ; /* [31..24]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_thre_03;

typedef union {
    struct {
        hi_u32    rc_buf_thresh_12      : 8  ; /* [7..0]  */
        hi_u32    rc_buf_thresh_13      : 8  ; /* [15..8]  */
        hi_u32    reserved_0            : 16 ; /* [31..16]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_thre_04;

typedef union {
    struct {
        hi_u32    range_bpg_offset_00   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_00       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_00       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_00;

typedef union {
    struct {
        hi_u32    range_bpg_offset_01   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_01       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_01       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_01;

typedef union {
    struct {
        hi_u32    range_bpg_offset_02   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_02       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_02       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_02;

typedef union {
    struct {
        hi_u32    range_bpg_offset_03   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_03       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_03       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_03;

typedef union {
    struct {
        hi_u32    range_bpg_offset_04   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_04       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_04       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_04;

typedef union {
    struct {
        hi_u32    range_bpg_offset_05   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_05       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_05       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_05;

typedef union {
    struct {
        hi_u32    range_bpg_offset_06   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_06       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_06       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_06;

typedef union {
    struct {
        hi_u32    range_bpg_offset_07   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_07       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_07       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_07;

typedef union {
    struct {
        hi_u32    range_bpg_offset_08   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_08       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_08       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_08;

typedef union {
    struct {
        hi_u32    range_bpg_offset_09   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_09       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_09       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_09;

typedef union {
    struct {
        hi_u32    range_bpg_offset_10   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_10       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_10       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_10;

typedef union {
    struct {
        hi_u32    range_bpg_offset_11   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_11       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_11       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_11;

typedef union {
    struct {
        hi_u32    range_bpg_offset_12   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_12       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_12       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_12;

typedef union {
    struct {
        hi_u32    range_bpg_offset_13   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_13       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_13       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_13;

typedef union {
    struct {
        hi_u32    range_bpg_offset_14   : 6  ; /* [5..0]  */
        hi_u32    reserved_0            : 2  ; /* [7..6]  */
        hi_u32    range_max_qp_14       : 5  ; /* [12..8]  */
        hi_u32    reserved_1            : 3  ; /* [15..13]  */
        hi_u32    range_min_qp_14       : 5  ; /* [20..16]  */
        hi_u32    reserved_2            : 11 ; /* [31..21]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_rc_param_14;

typedef union {
    struct {
        hi_u32    rc_err_type_0         : 8  ; /* [7..0]  */
        hi_u32    debug_enc0            : 24 ; /* [31..8]  */
    } bits;

    hi_u32    u32;
} mipi_dsc_enc_0_debug;

typedef struct {
    hi_u32                   dsc_id           ; /* 0x0 */
    mipi_dsc_ctrl            dsc_ctrl         ; /* 0x4 */
    mipi_dsc_int_sta         dsc_int_sta      ; /* 0x8 */
    mipi_dsc_int_mask        dsc_int_mask     ; /* 0xc */
    mipi_dsc_rc_bits         dsc_rc_bits      ; /* 0x10 */
    mipi_dsc_alg_ctrl        dsc_alg_ctrl     ; /* 0x14 */
    mipi_dsc_bpc             dsc_bpc          ; /* 0x18 */
    mipi_dsc_sample          dsc_sample       ; /* 0x1c */
    mipi_dsc_bpp_chk         dsc_bpp_chk      ; /* 0x20 */
    mipi_dsc_pic_reso        dsc_pic_reso     ; /* 0x24 */
    mipi_dsc_slc_reso        dsc_slc_reso     ; /* 0x28 */
    mipi_dsc_init_xmit_dly   dsc_init_xmit_dly; /* 0x2c */
    mipi_dsc_init_dec_dly    dsc_init_dec_dly ; /* 0x30 */
    mipi_dsc_init_scale      dsc_init_scale   ; /* 0x34 */
    mipi_dsc_scale_intval    dsc_scale_intval ; /* 0x38 */
    mipi_dsc_first_bpg       dsc_first_bpg    ; /* 0x3c */
    mipi_dsc_second_bpg      dsc_second_bpg   ; /* 0x40 */
    mipi_dsc_second_adj      dsc_second_adj   ; /* 0x44 */
    mipi_dsc_init_finl_ofs   dsc_init_finl_ofs; /* 0x48 */
    mipi_dsc_slc_bpg         dsc_slc_bpg      ; /* 0x4c */
    mipi_dsc_flat_range      dsc_flat_range   ; /* 0x50 */
    mipi_dsc_rc_mod_edge     dsc_rc_mod_edge  ; /* 0x54 */
    mipi_dsc_rc_qua_tgt      dsc_rc_qua_tgt   ; /* 0x58 */
    mipi_dsc_rc_thre_01      dsc_rc_thre_01   ; /* 0x5c */
    mipi_dsc_rc_thre_02      dsc_rc_thre_02   ; /* 0x60 */
    mipi_dsc_rc_thre_03      dsc_rc_thre_03   ; /* 0x64 */
    mipi_dsc_rc_thre_04      dsc_rc_thre_04   ; /* 0x68 */
    mipi_dsc_rc_param_00     dsc_rc_param_00  ; /* 0x6c */
    mipi_dsc_rc_param_01     dsc_rc_param_01  ; /* 0x70 */
    mipi_dsc_rc_param_02     dsc_rc_param_02  ; /* 0x74 */
    mipi_dsc_rc_param_03     dsc_rc_param_03  ; /* 0x78 */
    mipi_dsc_rc_param_04     dsc_rc_param_04  ; /* 0x7c */
    mipi_dsc_rc_param_05     dsc_rc_param_05  ; /* 0x80 */
    mipi_dsc_rc_param_06     dsc_rc_param_06  ; /* 0x84 */
    mipi_dsc_rc_param_07     dsc_rc_param_07  ; /* 0x88 */
    mipi_dsc_rc_param_08     dsc_rc_param_08  ; /* 0x8c */
    mipi_dsc_rc_param_09     dsc_rc_param_09  ; /* 0x90 */
    mipi_dsc_rc_param_10     dsc_rc_param_10  ; /* 0x94 */
    mipi_dsc_rc_param_11     dsc_rc_param_11  ; /* 0x98 */
    mipi_dsc_rc_param_12     dsc_rc_param_12  ; /* 0x9c */
    mipi_dsc_rc_param_13     dsc_rc_param_13  ; /* 0xa0 */
    mipi_dsc_rc_param_14     dsc_rc_param_14  ; /* 0xa4 */
    hi_u32                   dsc_cmc_mode_lo  ; /* 0xa8 */
    hi_u32                   dsc_cmc_mode_hi  ; /* 0xac */
    hi_u32                   dsc_enc_0_ctrl   ; /* 0xb0 */
    hi_u32                   dsc_enc_0_eco    ; /* 0xb4 */
    mipi_dsc_enc_0_debug     dsc_enc_0_debug  ; /* 0xb8 */
} mipi_dsc_regs_type;

#endif

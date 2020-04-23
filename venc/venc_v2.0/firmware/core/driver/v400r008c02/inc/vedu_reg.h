/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#ifndef __VEDU_REG_H__
#define __VEDU_REG_H__

/* Define the union u_vedu_vcpi_intmask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_enable_ve_eop    : 1   ; /* [0] */
        unsigned int vcpi_enable_vedu_slice_end : 1   ; /* [1] */
        unsigned int vcpi_enable_ve_buffull : 1   ; /* [2] */
        unsigned int vcpi_enable_ve_pbitsover : 1   ; /* [3] */
        unsigned int vcpi_enable_axi_rdbus_err : 1   ; /* [4] */
        unsigned int vcpi_enable_axi_wrbus_err : 1   ; /* [5] */
        unsigned int vcpi_enable_vedu_timeout : 1   ; /* [6] */
        unsigned int vcpi_enable_decoder_linenum_end : 1   ; /* [7] */
        unsigned int vcpi_enable_decoder_sed_err_core : 1   ; /* [8] */
        unsigned int vcpi_enable_decoder_sed_err_ext : 1   ; /* [9] */
        unsigned int reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_intmask;

/* Define the union u_vedu_vcpi_intclr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_clr_ve_eop       : 1   ; /* [0] */
        unsigned int vcpi_clr_vedu_slice_end : 1   ; /* [1] */
        unsigned int vcpi_clr_ve_buffull   : 1   ; /* [2] */
        unsigned int vcpi_clr_ve_pbitsover : 1   ; /* [3] */
        unsigned int vcpi_clr_axi_rdbus_err : 1   ; /* [4] */
        unsigned int vcpi_clr_axi_wrbus_err : 1   ; /* [5] */
        unsigned int vcpi_clr_vedu_timeout : 1   ; /* [6] */
        unsigned int vcpi_clr_decoder_linenum_end : 1   ; /* [7] */
        unsigned int vcpi_clr_decoder_sed_err_core : 1   ; /* [8] */
        unsigned int vcpi_clr_decoder_sed_err_ext : 1   ; /* [9] */
        unsigned int reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_intclr;

/* Define the union u_vedu_vcpi_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_vstart           : 1   ; /* [0] */
        unsigned int reserved_0            : 31  ; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_start;


/* Define the union u_vedu_vcpi_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_vedsel           : 1   ; /* [0] */
        unsigned int vedu_selfrst_en       : 1   ; /* [1] */
        unsigned int vcpi_protocol         : 2   ; /* [3..2] */
        unsigned int vcpi_cfg_mode         : 1   ; /* [4] */
        unsigned int vcpi_slice_int_en     : 1   ; /* [5] */
        unsigned int vcpi_sao_luma         : 1   ; /* [6] */
        unsigned int vcpi_sao_chroma       : 1   ; /* [7] */
        unsigned int vcpi_rec_cmp_en       : 1   ; /* [8] */
        unsigned int vcpi_img_improve_en   : 1   ; /* [9] */
        unsigned int vcpi_frame_type       : 2   ; /* [11..10] */
        unsigned int vcpi_entropy_mode     : 1   ; /* [12] */
        unsigned int vcpi_long_term_refpic : 1   ; /* [13] */
        unsigned int vcpi_ref_num          : 1   ; /* [14] */
        unsigned int vcpi_pr_inter_en      : 1   ; /* [15] */
        unsigned int vcpi_idr_pic          : 1   ; /* [16] */
        unsigned int vcpi_pskip_en         : 1   ; /* [17] */
        unsigned int vcpi_trans_mode       : 2   ; /* [19..18] */
        unsigned int vcpi_blk8_inter       : 1   ; /* [20] */
        unsigned int vcpi_sobel_weight_en  : 1   ; /* [21] */
        unsigned int vcpi_iscaling_type    : 2   ; /* [23..22] */
        unsigned int reserved_0            : 4   ; /* [27..24] */
        unsigned int vcpi_time_en          : 1   ; /* [28] */
        unsigned int reserved_1            : 1   ; /* [29] */
        unsigned int vcpi_ref_cmp_en       : 1   ; /* [30] */
        unsigned int vcpi_refc_nload       : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_mode;


/* Define the union u_vedu_vcpi_picsize_pix */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_imgwidth_pix     : 16  ; /* [15..0] */
        unsigned int vcpi_imgheight_pix    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_picsize_pix;

/* Define the union u_vedu_vcpi_qpcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_cr_qp_offset     : 5   ; /* [4..0] */
        unsigned int vcpi_cb_qp_offset     : 5   ; /* [9..5] */
        unsigned int vcpi_frm_qp           : 6   ; /* [15..10] */
        unsigned int reserved_0            : 2   ; /* [17..16] */
        unsigned int vcpi_intra_det_qp_en  : 1   ; /* [18] */
        unsigned int vcpi_rc_cu_madi_en    : 1   ; /* [19] */
        unsigned int reserved_1            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_qpcfg;

/* Define the union u_vedu_vcpi_outstd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_r_outstanding    : 7   ; /* [6..0] */
        unsigned int reserved_0            : 1   ; /* [7] */
        unsigned int vcpi_w_outstanding    : 4   ; /* [11..8] */
        unsigned int reserved_1            : 4   ; /* [15..12] */
        unsigned int vcpi_latency_statis_en : 1   ; /* [16] */
        unsigned int reserved_2            : 3   ; /* [19..17] */
        unsigned int vcpi_axi_lock_en      : 1   ; /* [20] */
        unsigned int vcpi_axi_unlock_en    : 1   ; /* [21] */
        unsigned int reserved_3            : 10  ; /* [31..22] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_outstd;

/* Define the union u_vedu_vcpi_basic_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mbamt_to_dec          : 18  ; /* [17..0] */
        unsigned int reserved_0            : 4   ; /* [21..18] */
        unsigned int marker_bit_detect_en  : 1   ; /* [22] */
        unsigned int ac_last_detect_en     : 1   ; /* [23] */
        unsigned int coef_idx_detect_en    : 1   ; /* [24] */
        unsigned int vop_type_detect_en    : 1   ; /* [25] */
        unsigned int work_mode             : 2   ; /* [27..26] */
        unsigned int reserved_1            : 2   ; /* [29..28] */
        unsigned int ld_qmatrix_flag       : 1   ; /* [30] */
        unsigned int reserved_2            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_basic_cfg0;

/* Define the union u_vedu_vcpi_basic_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 13  ; /* [12..0] */
        unsigned int uv_order_en           : 1   ; /* [13] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int max_slcgrp_num        : 12  ; /* [27..16] */
        unsigned int vcpi_linenum_output_en : 1   ; /* [28] */
        unsigned int dblk_2d_en            : 1   ; /* [29] */
        unsigned int reserved_2            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_basic_cfg1;

/* Define the union u_vedu_vcpi_decoder_d0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 18  ; /* [17..0] */
        unsigned int pcm_enabled_flag      : 1   ; /* [18] */
        unsigned int chroma_format_idc     : 2   ; /* [20..19] */
        unsigned int amp_enabled_flag      : 1   ; /* [21] */
        unsigned int sample_adaptive_offset_enabled_flag : 1   ; /* [22] */
        unsigned int pcm_loop_filter_disabled_flag : 1   ; /* [23] */
        unsigned int scaling_list_enabled_flag : 1   ; /* [24] */
        unsigned int reserved_1            : 1   ; /* [25] */
        unsigned int transform_8x8_mode_flag : 1   ; /* [26] */
        unsigned int reserved_2            : 5   ; /* [31..27] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_decoder_d0;

/* Define the union u_vedu_vcpi_decoder_d1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ctb_log2_size_y          : 3   ; /* [2..0] */
        unsigned int log2_min_cb_size_y        : 3   ; /* [5..3] */
        unsigned int max_cu_depth            : 3   ; /* [8..6] */
        unsigned int log2_max_trafo_size      : 3   ; /* [11..9] */
        unsigned int log2_min_trafo_size      : 3   ; /* [14..12] */
        unsigned int log2_max_ipcm_cb_size_y    : 3   ; /* [17..15] */
        unsigned int log2_min_ipcm_cb_size_y    : 3   ; /* [20..18] */
        unsigned int max_transform_hierarchy_depth_intra : 3   ; /* [23..21] */
        unsigned int max_transform_hierarchy_depth_inter : 3   ; /* [26..24] */
        unsigned int weighted_pred_flag    : 1   ; /* [27] */
        unsigned int weighted_bipred_flag  : 1   ; /* [28] */
        unsigned int log2_parallel_merge_level_minus2 : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_decoder_d1;

/* Define the union u_vedu_vcpi_decoder_d2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bit_depth_y             : 4   ; /* [3..0] */
        unsigned int bit_depth_c             : 4   ; /* [7..4] */
        unsigned int pcm_bit_depth_y          : 4   ; /* [11..8] */
        unsigned int pcm_bit_depth_c          : 4   ; /* [15..12] */
        unsigned int qp_bd_offset_y           : 6   ; /* [21..16] */
        unsigned int qp_bd_offset_c           : 6   ; /* [27..22] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;

} u_vedu_vcpi_decoder_d2;

/* Define the union u_vedu_vcpi_decoder_d3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 11  ; /* [10..0] */
        unsigned int transquant_bypass_enabled_flag : 1   ; /* [11] */
        unsigned int cu_qp_delta_enabled_flag : 1   ; /* [12] */
        unsigned int diff_cu_qp_delta_depth : 3   ; /* [15..13] */
        unsigned int log2_min_cu_qp_delta_size  : 3   ; /* [18..16] */
        unsigned int reserved_1            : 2   ; /* [20..19] */
        unsigned int entropy_coding_sync_enabled_flag : 1   ; /* [21] */
        unsigned int sign_data_hiding_enabled_flag : 1   ; /* [22] */
        unsigned int transform_skip_enabled_flag : 1   ; /* [23] */
        unsigned int reserved_2            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_decoder_d3;

/* Define the union u_vedu_cmp_line_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int recst_cmp_line_level  : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_cmp_line_level;

/* Define the union u_vedu_bus_idle_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vedu_bus_idle_req     : 1   ; /* [0] */
        unsigned int reserved_0            : 31  ; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_bus_idle_req;

/* Define the union u_vedu_vcpi_multislc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_slice_size       : 16  ; /* [15..0] */
        unsigned int vcpi_slcspilt_mod     : 2   ; /* [17..16] */
        unsigned int reserved_0            : 2   ; /* [19..18] */
        unsigned int vcpi_multislc_en      : 1   ; /* [20] */
        unsigned int reserved_1            : 11  ; /* [31..21] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_multislc;

/* Define the union u_vedu_vcpi_rc_enable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int  vcpi_rc_cu_qp_en      : 1   ; /* [0] */
        unsigned int  vcpi_rc_row_qp_en     : 1   ; /* [1] */
        unsigned int  vcpi_move_scene_en    : 1   ; /* [2] */
        unsigned int  reserved_0            : 1   ; /* [3] */
        unsigned int  vcpi_strong_edge_move_en : 1   ; /* [4] */
        unsigned int  reserved_1            : 3   ; /* [7..5] */
        unsigned int  vcpi_rc_low_luma_en   : 1   ; /* [8] */
        unsigned int  reserved_2            : 2   ; /* [10..9] */
        unsigned int  vcpi_rd_min_sad_flag_en : 1   ; /* [11] */
        unsigned int  reserved_3            : 2   ; /* [13..12] */
        unsigned int  vcpi_low_min_sad_en   : 1   ; /* [14] */
        unsigned int  vcpi_prev_min_sad_en  : 1   ; /* [15] */
        unsigned int  vcpi_qpgld_en         : 1   ; /* [16] */
        unsigned int  vcpi_map_roikeep_en   : 1   ; /* [17] */
        unsigned int  vcpi_flat_region_en   : 1   ; /* [18] */
        unsigned int  vcpi_qp_restrain_large_sad : 1   ; /* [19] */
        unsigned int  reserved_4            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_rc_enable;

/* Define the union u_vedu_vlcst_ptbits_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlcst_ptbits_en       : 1   ; /* [0] */
        unsigned int reserved_0            : 31  ; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vlcst_ptbits_en;

/* Define the union u_vedu_qpg_max_min_qp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_min_qp            : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_max_qp            : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int qpg_cu_qp_delta_enable_flag : 1   ; /* [16] */
        unsigned int reserved_2            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_max_min_qp;

/* Define the union u_vedu_qpg_rc_max_min_qp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_rc_min_qp         : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_rc_max_qp         : 6   ; /* [13..8] */
        unsigned int reserved_1            : 18  ; /* [31..14] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_rc_max_min_qp;

/* Define the union u_vedu_qpg_smart_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 2   ; /* [1..0] */
        unsigned int qpg_smart_get_cu32_qp_mode : 2   ; /* [3..2] */
        unsigned int qpg_smart_get_cu64_qp_mode : 2   ; /* [5..4] */
        unsigned int qpg_qp_detlta_size_cu64 : 1   ; /* [6] */
        unsigned int reserved_1            : 25  ; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_smart_reg;

/* Define the union u_vedu_qpg_row_target_bits */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_qp_delta          : 4   ; /* [3..0] */
        unsigned int reserved_0            : 2   ; /* [5..4] */
        unsigned int qpg_row_target_bits   : 25  ; /* [30..6] */
        unsigned int reserved_1            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_row_target_bits;

/* Define the union u_vedu_qpg_average_lcu_bits */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_ave_lcu_bits      : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_average_lcu_bits;

/* Define the union u_vedu_qpg_cu_qp_delta_thresh_reg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_cu_qp_delta_thresh0 : 8   ; /* [7..0] */
        unsigned int qpg_cu_qp_delta_thresh1 : 8   ; /* [15..8] */
        unsigned int qpg_cu_qp_delta_thresh2 : 8   ; /* [23..16] */
        unsigned int qpg_cu_qp_delta_thresh3 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_cu_qp_delta_thresh_reg0;

/* Define the union u_vedu_qpg_cu_qp_delta_thresh_reg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_cu_qp_delta_thresh4 : 8   ; /* [7..0] */
        unsigned int qpg_cu_qp_delta_thresh5 : 8   ; /* [15..8] */
        unsigned int qpg_cu_qp_delta_thresh6 : 8   ; /* [23..16] */
        unsigned int qpg_cu_qp_delta_thresh7 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_cu_qp_delta_thresh_reg1;

/* Define the union u_vedu_qpg_cu_qp_delta_thresh_reg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_cu_qp_delta_thresh8 : 8   ; /* [7..0] */
        unsigned int qpg_cu_qp_delta_thresh9 : 8   ; /* [15..8] */
        unsigned int qpg_cu_qp_delta_thresh10 : 8   ; /* [23..16] */
        unsigned int qpg_cu_qp_delta_thresh11 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_cu_qp_delta_thresh_reg2;

/* Define the union u_vedu_qpg_cu_qp_delta_thresh_reg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_cu_qp_delta_thresh12 : 8   ; /* [7..0] */
        unsigned int qpg_cu_qp_delta_thresh13 : 8   ; /* [15..8] */
        unsigned int qpg_cu_qp_delta_thresh14 : 8   ; /* [23..16] */
        unsigned int qpg_cu_qp_delta_thresh15 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_cu_qp_delta_thresh_reg3;

/* Define the union u_vedu_qpg_delta_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_qp_delta_level_0  : 2   ; /* [1..0] */
        unsigned int qpg_qp_delta_level_1  : 2   ; /* [3..2] */
        unsigned int qpg_qp_delta_level_2  : 2   ; /* [5..4] */
        unsigned int qpg_qp_delta_level_3  : 2   ; /* [7..6] */
        unsigned int qpg_qp_delta_level_4  : 2   ; /* [9..8] */
        unsigned int qpg_qp_delta_level_5  : 2   ; /* [11..10] */
        unsigned int qpg_qp_delta_level_6  : 2   ; /* [13..12] */
        unsigned int qpg_qp_delta_level_7  : 2   ; /* [15..14] */
        unsigned int qpg_qp_delta_level_8  : 2   ; /* [17..16] */
        unsigned int qpg_qp_delta_level_9  : 2   ; /* [19..18] */
        unsigned int qpg_qp_delta_level_10 : 2   ; /* [21..20] */
        unsigned int qpg_qp_delta_level_11 : 2   ; /* [23..22] */
        unsigned int qpg_qp_delta_level_12 : 2   ; /* [25..24] */
        unsigned int qpg_qp_delta_level_13 : 2   ; /* [27..26] */
        unsigned int qpg_qp_delta_level_14 : 2   ; /* [29..28] */
        unsigned int qpg_qp_delta_level_15 : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_delta_level;

/* Define the union u_vedu_qpg_madi_switch_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_qp_madi_switch_thr : 5   ; /* [4..0] */
        unsigned int reserved_0            : 27  ; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_madi_switch_thr;

/* Define the union u_vedu_qpg_curr_sad_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_cu_sad_en     : 1   ; /* [0] */
        unsigned int reserved_0            : 3   ; /* [3..1] */
        unsigned int vcpi_sad_switch_thr   : 5   ; /* [8..4] */
        unsigned int reserved_1            : 7   ; /* [15..9] */
        unsigned int vcpi_rc_cu_sad_mod    : 2   ; /* [17..16] */
        unsigned int vcpi_rc_cu_sad_offset : 8   ; /* [25..18] */
        unsigned int vcpi_rc_cu_sad_gain   : 4   ; /* [29..26] */
        unsigned int reserved_2            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_curr_sad_en;

/* Define the union u_vedu_qpg_curr_sad_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_curr_sad_level_0 : 2   ; /* [1..0] */
        unsigned int vcpi_curr_sad_level_1 : 2   ; /* [3..2] */
        unsigned int vcpi_curr_sad_level_2 : 2   ; /* [5..4] */
        unsigned int vcpi_curr_sad_level_3 : 2   ; /* [7..6] */
        unsigned int vcpi_curr_sad_level_4 : 2   ; /* [9..8] */
        unsigned int vcpi_curr_sad_level_5 : 2   ; /* [11..10] */
        unsigned int vcpi_curr_sad_level_6 : 2   ; /* [13..12] */
        unsigned int vcpi_curr_sad_level_7 : 2   ; /* [15..14] */
        unsigned int vcpi_curr_sad_level_8 : 2   ; /* [17..16] */
        unsigned int vcpi_curr_sad_level_9 : 2   ; /* [19..18] */
        unsigned int vcpi_curr_sad_level_10 : 2   ; /* [21..20] */
        unsigned int vcpi_curr_sad_level_11 : 2   ; /* [23..22] */
        unsigned int vcpi_curr_sad_level_12 : 2   ; /* [25..24] */
        unsigned int vcpi_curr_sad_level_13 : 2   ; /* [27..26] */
        unsigned int vcpi_curr_sad_level_14 : 2   ; /* [29..28] */
        unsigned int vcpi_curr_sad_level_15 : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_curr_sad_level;

/* Define the union u_vedu_qpg_curr_sad_thresh0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_curr_sad_thresh_0 : 8   ; /* [7..0] */
        unsigned int vcpi_curr_sad_thresh_1 : 8   ; /* [15..8] */
        unsigned int vcpi_curr_sad_thresh_2 : 8   ; /* [23..16] */
        unsigned int vcpi_curr_sad_thresh_3 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_curr_sad_thresh0;

/* Define the union u_vedu_qpg_curr_sad_thresh1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_curr_sad_thresh_4 : 8   ; /* [7..0] */
        unsigned int vcpi_curr_sad_thresh_5 : 8   ; /* [15..8] */
        unsigned int vcpi_curr_sad_thresh_6 : 8   ; /* [23..16] */
        unsigned int vcpi_curr_sad_thresh_7 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_curr_sad_thresh1;

/* Define the union u_vedu_qpg_curr_sad_thresh2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_curr_sad_thresh_8 : 8   ; /* [7..0] */
        unsigned int vcpi_curr_sad_thresh_9 : 8   ; /* [15..8] */
        unsigned int vcpi_curr_sad_thresh_10 : 8   ; /* [23..16] */
        unsigned int vcpi_curr_sad_thresh_11 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_curr_sad_thresh2;

/* Define the union u_vedu_qpg_curr_sad_thresh3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_curr_sad_thresh_12 : 8   ; /* [7..0] */
        unsigned int vcpi_curr_sad_thresh_13 : 8   ; /* [15..8] */
        unsigned int vcpi_curr_sad_thresh_14 : 8   ; /* [23..16] */
        unsigned int vcpi_curr_sad_thresh_15 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_curr_sad_thresh3;

/* Define the union u_vedu_luma_rc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_luma_en       : 1   ; /* [0] */
        unsigned int vcpi_rc_luma_mode     : 2   ; /* [2..1] */
        unsigned int reserved_0            : 1   ; /* [3] */
        unsigned int vcpi_rc_luma_switch_thr : 5   ; /* [8..4] */
        unsigned int reserved_1            : 3   ; /* [11..9] */
        unsigned int vcpi_rc_luma_low_madi_thr : 8   ; /* [19..12] */
        unsigned int vcpi_rc_luma_high_madi_thr : 8   ; /* [27..20] */
        unsigned int reserved_2            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_luma_rc;

/* Define the union u_vedu_luma_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_luma_level_0  : 2   ; /* [1..0] */
        unsigned int vcpi_rc_luma_level_1  : 2   ; /* [3..2] */
        unsigned int vcpi_rc_luma_level_2  : 2   ; /* [5..4] */
        unsigned int vcpi_rc_luma_level_3  : 2   ; /* [7..6] */
        unsigned int vcpi_rc_luma_level_4  : 2   ; /* [9..8] */
        unsigned int vcpi_rc_luma_level_5  : 2   ; /* [11..10] */
        unsigned int vcpi_rc_luma_level_6  : 2   ; /* [13..12] */
        unsigned int vcpi_rc_luma_level_7  : 2   ; /* [15..14] */
        unsigned int vcpi_rc_luma_level_8  : 2   ; /* [17..16] */
        unsigned int vcpi_rc_luma_level_9  : 2   ; /* [19..18] */
        unsigned int vcpi_rc_luma_level_10 : 2   ; /* [21..20] */
        unsigned int vcpi_rc_luma_level_11 : 2   ; /* [23..22] */
        unsigned int vcpi_rc_luma_level_12 : 2   ; /* [25..24] */
        unsigned int vcpi_rc_luma_level_13 : 2   ; /* [27..26] */
        unsigned int vcpi_rc_luma_level_14 : 2   ; /* [29..28] */
        unsigned int vcpi_rc_luma_level_15 : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_luma_level;

/* Define the union u_vedu_luma_thresh0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_luma_thresh_0 : 8   ; /* [7..0] */
        unsigned int vcpi_rc_luma_thresh_1 : 8   ; /* [15..8] */
        unsigned int vcpi_rc_luma_thresh_2 : 8   ; /* [23..16] */
        unsigned int vcpi_rc_luma_thresh_3 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_luma_thresh0;

/* Define the union u_vedu_luma_thresh1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_luma_thresh_4 : 8   ; /* [7..0] */
        unsigned int vcpi_rc_luma_thresh_5 : 8   ; /* [15..8] */
        unsigned int vcpi_rc_luma_thresh_6 : 8   ; /* [23..16] */
        unsigned int vcpi_rc_luma_thresh_7 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_luma_thresh1;

/* Define the union u_vedu_luma_thresh2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_luma_thresh_8 : 8   ; /* [7..0] */
        unsigned int vcpi_rc_luma_thresh_9 : 8   ; /* [15..8] */
        unsigned int vcpi_rc_luma_thresh_10 : 8   ; /* [23..16] */
        unsigned int vcpi_rc_luma_thresh_11 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_luma_thresh2;

/* Define the union u_vedu_luma_thresh3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rc_luma_thresh_12 : 8   ; /* [7..0] */
        unsigned int vcpi_rc_luma_thresh_13 : 8   ; /* [15..8] */
        unsigned int vcpi_rc_luma_thresh_14 : 8   ; /* [23..16] */
        unsigned int vcpi_rc_luma_thresh_15 : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_luma_thresh3;

/* Define the union u_vedu_vctrl_lcu_baseline */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_lcu_performance_baseline : 16  ; /* [15..0] */
        unsigned int vctrl_lcu_performance_en : 1   ; /* [16] */
        unsigned int reserved_0            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_lcu_baseline;

/* Define the union u_vedu_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rashd_rme             : 1; /* [0] */
        unsigned int    rashd_rm              : 4; /* [4..1] */
        unsigned int    rfshd_rme             : 1; /* [5] */
        unsigned int    rfshd_rm              : 4; /* [9..6] */
        unsigned int    rfshs_rme             : 1; /* [10] */
        unsigned int    rfshs_rm              : 4; /* [14..11] */
        unsigned int    rfthd_rma_high        : 1; /* [15] */
        unsigned int    rfthd_rmea            : 1; /* [16] */
        unsigned int    rfthd_rma             : 3; /* [19..17] */
        unsigned int    rfthd_rmb_high        : 1; /* [20] */
        unsigned int    rfthd_rmeb            : 1; /* [21] */
        unsigned int    rfthd_rmb             : 3; /* [24..22] */
        unsigned int    reserved_0            : 7; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mem_ctrl;

/* Define the union u_vedu_safe_mode_flag */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int safe_flag_vcpi_llild  : 1   ; /* [0] */
        unsigned int safe_flag_curld_osd   : 1   ; /* [1] */
        unsigned int safe_flag_curld_lowdly : 1   ; /* [2] */
        unsigned int safe_flag_curld_src   : 1   ; /* [3] */
        unsigned int safe_flag_refld_ref   : 1   ; /* [4] */
        unsigned int safe_flag_pmeld_ds    : 1   ; /* [5] */
        unsigned int safe_flag_pmest_ds    : 1   ; /* [6] */
        unsigned int safe_flag_recst_rec   : 1   ; /* [7] */
        unsigned int safe_flag_nbi_tmv     : 1   ; /* [8] */
        unsigned int safe_flag_pmeinfoldst_info : 1   ; /* [9] */
        unsigned int safe_flag_qpgld_info  : 1   ; /* [10] */
        unsigned int safe_flag_pmeinfoldst_skipweight : 1   ; /* [11] */
        unsigned int safe_flag_tqitq_qm    : 1   ; /* [12] */
        unsigned int safe_flag_vlcst_strm  : 1   ; /* [13] */
        unsigned int safe_flag_vlcst_swptr : 1   ; /* [14] */
        unsigned int safe_flag_vlcst_srptr : 1   ; /* [15] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_safe_mode_flag;

/* Define the union u_vedu_vcpi_ddr_cross */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_ddr_cross_idx    : 11  ; /* [10..0] */
        unsigned int reserved_0            : 5   ; /* [15..11] */
        unsigned int vcpi_ddr_cross_en     : 1   ; /* [16] */
        unsigned int reserved_1            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_ddr_cross;

/* Define the union u_vedu_debug_md5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_sad_thr0         : 10  ; /* [9..0] */
        unsigned int vcpi_sad_thr1         : 10  ; /* [19..10] */
        unsigned int vcpi_sad_thr2         : 10  ; /* [29..20] */
        unsigned int vcpi_md5_en           : 1   ; /* [30] */
        unsigned int reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_debug_md5;

/* Define the union u_vedu_chn_bypass */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_llild_bypass     : 1   ; /* [0] */
        unsigned int curld_lowdly_bypass   : 1   ; /* [1] */
        unsigned int curld_srcy_bypass     : 1   ; /* [2] */
        unsigned int curld_srcu_bypass     : 1   ; /* [3] */
        unsigned int curld_srcv_bypass     : 1   ; /* [4] */
        unsigned int pmeldst_bypass        : 1   ; /* [5] */
        unsigned int pmeinfoldst_bypass    : 1   ; /* [6] */
        unsigned int pmeinfold_qpgld_bypass : 1   ; /* [7] */
        unsigned int pmeinfold_skipweight_bypass : 1   ; /* [8] */
        unsigned int refld_bypass          : 1   ; /* [9] */
        unsigned int reserved_0            : 1   ; /* [10] */
        unsigned int nbildst_tmv_bypass    : 1   ; /* [11] */
        unsigned int nbildst_up_bypass     : 1   ; /* [12] */
        unsigned int reserved_1            : 1   ; /* [13] */
        unsigned int recst_bypass          : 1   ; /* [14] */
        unsigned int vlcst_strm_rptr_bypass : 1   ; /* [15] */
        unsigned int vlcst_strm_wptr_bypass : 1   ; /* [16] */
        unsigned int vlcst_para_bypass     : 1   ; /* [17] */
        unsigned int vlcst_strm_bypass     : 1   ; /* [18] */
        unsigned int refld_head_bypass     : 1   ; /* [19] */
        unsigned int recst_head_bypass     : 1   ; /* [20] */
        unsigned int curld_head_y_bypass   : 1   ; /* [21] */
        unsigned int curld_head_c_bypass   : 1   ; /* [22] */
        unsigned int curld_osd_bypass      : 1   ; /* [23] */
        unsigned int reserved_2            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chn_bypass;

/* Define the union u_vedu_vcpi_cross_tile_slc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_cross_slice      : 1   ; /* [0] */
        unsigned int vcpi_cross_tile       : 1   ; /* [1] */
        unsigned int reserved_0            : 30  ; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_cross_tile_slc;

/* Define the union u_vedu_vcpi_ref_flag */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 1   ; /* [0] */
        unsigned int vcpi_col_from_l0_flag : 1   ; /* [1] */
        unsigned int vcpi_curr_ref_long_flag : 2   ; /* [3..2] */
        unsigned int reserved_1            : 2   ; /* [5..4] */
        unsigned int vcpi_predflag_sel     : 2   ; /* [7..6] */
        unsigned int reserved_2            : 24  ; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_ref_flag;

/* Define the union u_vedu_pmv_tmv_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pmv_tmv_en            : 1   ; /* [0] */
        unsigned int reserved_0            : 31  ; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pmv_tmv_en;

/* Define the union u_vedu_vcpi_tmv_load */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_tmv_wr_rd_avail  : 2   ; /* [1..0] */
        unsigned int reserved_0            : 30  ; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_tmv_load;

/* Define the union u_vedu_cabac_glb_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 8   ; /* [7..0] */
        unsigned int cabac_max_num_mergecand : 3   ; /* [10..8] */
        unsigned int reserved_1            : 5   ; /* [15..11] */
        unsigned int cabac_nal_unit_head   : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_cabac_glb_cfg;

/* Define the union u_vedu_ice_cmc_mode_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_mem_clkgate_en    : 1   ; /* [0] */
        unsigned int reserved_0            : 1   ; /* [1] */
        unsigned int rfs_emaw              : 2   ; /* [3..2] */
        unsigned int rfs_ema               : 3   ; /* [6..4] */
        unsigned int rft_colldisn          : 1   ; /* [7] */
        unsigned int rft_emaa              : 3   ; /* [10..8] */
        unsigned int rft_emab              : 3   ; /* [13..11] */
        unsigned int rft_emasa             : 1   ; /* [14] */
        unsigned int rfsuhd_wtsel          : 2   ; /* [16..15] */
        unsigned int rfsuhd_rtsel          : 2   ; /* [18..17] */
        unsigned int rfs_wtsel             : 2   ; /* [20..19] */
        unsigned int rfs_rtsel             : 2   ; /* [22..21] */
        unsigned int rfts_wct              : 2   ; /* [24..23] */
        unsigned int rfts_rct              : 2   ; /* [26..25] */
        unsigned int rfts_kp               : 3   ; /* [29..27] */
        unsigned int rftf_wct              : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ice_cmc_mode_cfg0;

/* Define the union u_vedu_ice_cmc_mode_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rftf_rct              : 2   ; /* [1..0] */
        unsigned int rftf_kp               : 3   ; /* [4..2] */
        unsigned int rfthd_wtsel           : 2   ; /* [6..5] */
        unsigned int rfthd_mtsel           : 2   ; /* [8..7] */
        unsigned int rfthd_rtsel           : 2   ; /* [10..9] */
        unsigned int rasshds_wtsel         : 2   ; /* [12..11] */
        unsigned int rasshds_rtsel         : 2   ; /* [14..13] */
        unsigned int rasshdm_wtsel         : 2   ; /* [16..15] */
        unsigned int rasshdm_rtsel         : 2   ; /* [18..17] */
        unsigned int rashds_wtsel          : 2   ; /* [20..19] */
        unsigned int rashds_rtsel          : 2   ; /* [22..21] */
        unsigned int rashdm_wtsel          : 2   ; /* [24..23] */
        unsigned int rashdm_rtsel          : 2   ; /* [26..25] */
        unsigned int ras_wtsel             : 2   ; /* [28..27] */
        unsigned int ras_rtsel             : 2   ; /* [30..29] */
        unsigned int reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ice_cmc_mode_cfg1;

/* Define the union u_vedu_vcpi_dblkcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_dblk_beta        : 4   ; /* [3..0] */
        unsigned int vcpi_dblk_alpha       : 4   ; /* [7..4] */
        unsigned int vcpi_dblk_filter_flag : 2   ; /* [9..8] */
        unsigned int reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_dblkcfg;

/* Define the union u_vedu_intra_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int constrained_intra_pred_flag : 1   ; /* [0] */
        unsigned int reserved_0            : 3   ; /* [3..1] */
        unsigned int intra_smooth          : 1   ; /* [4] */
        unsigned int reserved_1            : 27  ; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_cfg;

/* Define the union u_vedu_curld_gcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd0_global_en  : 1   ; /* [0] */
        unsigned int curld_osd1_global_en  : 1   ; /* [1] */
        unsigned int curld_osd2_global_en  : 1   ; /* [2] */
        unsigned int curld_osd3_global_en  : 1   ; /* [3] */
        unsigned int curld_osd4_global_en  : 1   ; /* [4] */
        unsigned int curld_osd5_global_en  : 1   ; /* [5] */
        unsigned int curld_osd6_global_en  : 1   ; /* [6] */
        unsigned int curld_osd7_global_en  : 1   ; /* [7] */
        unsigned int reserved_0            : 6   ; /* [13..8] */
        unsigned int curld_col2gray_en     : 1   ; /* [14] */
        unsigned int curld_clip_en         : 1   ; /* [15] */
        unsigned int reserved_1            : 4   ; /* [19..16] */
        unsigned int curld_read_interval   : 8   ; /* [27..20] */
        unsigned int curld_lowdly_en       : 1   ; /* [28] */
        unsigned int reserved_2            : 1   ; /* [29] */
        unsigned int curld_osd_rgbfmt      : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_gcfg;

/* Define the union u_vedu_vcpi_strfmt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_crop_en          : 1   ; /* [0] */
        unsigned int vcpi_scale_en         : 1   ; /* [1] */
        unsigned int vcpi_store_mode       : 1   ; /* [2] */
        unsigned int vcpi_blk_type         : 3   ; /* [5..3] */
        unsigned int vcpi_str_fmt          : 4   ; /* [9..6] */
        unsigned int vcpi_package_sel      : 8   ; /* [17..10] */
        unsigned int vcpi_recst_disable    : 1   ; /* [18] */
        unsigned int reserved_0            : 13  ; /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_strfmt;

/* Define the union u_vedu_vctrl_lcu_target_bit */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_lcu_target_bit  : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_lcu_target_bit;

/* Define the union u_vedu_vcpi_refld_dw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_refld_hdw        : 16  ; /* [15..0] */
        unsigned int vcpi_refld_vdw        : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_refld_dw;

/* Define the union u_vedu_vcpi_i_slc_insert */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_insert_i_slc_idx : 8   ; /* [7..0] */
        unsigned int vcpi_insert_i_slc_en  : 1   ; /* [8] */
        unsigned int reserved_0            : 23  ; /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_i_slc_insert;

/* Define the union u_vedu_pme_safe_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_safe_line         : 16  ; /* [15..0] */
        unsigned int pme_safe_line_val     : 1   ; /* [16] */
        unsigned int pme_safe_line_mode    : 1   ; /* [17] */
        unsigned int reserved_0            : 14  ; /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_safe_cfg;

/* Define the union u_vedu_pme_iblk_refresh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_iblk_refresh_en   : 1   ; /* [0] */
        unsigned int pme_iblk_refresh_mode : 1   ; /* [1] */
        unsigned int reserved_0            : 30  ; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_iblk_refresh;

/* Define the union u_vedu_pme_iblk_refresh_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_iblk_refresh_start : 12  ; /* [11..0] */
        unsigned int pme_iblk_refresh_size : 12  ; /* [23..12] */
        unsigned int reserved_0            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_iblk_refresh_para;

/* Define the union u_vedu_intra_chnl4_ang_oen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu4_mode0_en    : 1   ; /* [0] */
        unsigned int intra_cu4_mode1_en    : 1   ; /* [1] */
        unsigned int intra_cu4_mode2_en    : 1   ; /* [2] */
        unsigned int intra_cu4_mode3_en    : 1   ; /* [3] */
        unsigned int intra_cu4_mode4_en    : 1   ; /* [4] */
        unsigned int intra_cu4_mode5_en    : 1   ; /* [5] */
        unsigned int intra_cu4_mode6_en    : 1   ; /* [6] */
        unsigned int intra_cu4_mode7_en    : 1   ; /* [7] */
        unsigned int intra_cu4_mode8_en    : 1   ; /* [8] */
        unsigned int intra_cu4_mode9_en    : 1   ; /* [9] */
        unsigned int intra_cu4_mode10_en   : 1   ; /* [10] */
        unsigned int intra_cu4_mode11_en   : 1   ; /* [11] */
        unsigned int intra_cu4_mode12_en   : 1   ; /* [12] */
        unsigned int intra_cu4_mode13_en   : 1   ; /* [13] */
        unsigned int intra_cu4_mode14_en   : 1   ; /* [14] */
        unsigned int intra_cu4_mode15_en   : 1   ; /* [15] */
        unsigned int intra_cu4_mode16_en   : 1   ; /* [16] */
        unsigned int intra_cu4_mode17_en   : 1   ; /* [17] */
        unsigned int intra_cu4_mode18_en   : 1   ; /* [18] */
        unsigned int intra_cu4_mode19_en   : 1   ; /* [19] */
        unsigned int intra_cu4_mode20_en   : 1   ; /* [20] */
        unsigned int intra_cu4_mode21_en   : 1   ; /* [21] */
        unsigned int intra_cu4_mode22_en   : 1   ; /* [22] */
        unsigned int intra_cu4_mode23_en   : 1   ; /* [23] */
        unsigned int intra_cu4_mode24_en   : 1   ; /* [24] */
        unsigned int intra_cu4_mode25_en   : 1   ; /* [25] */
        unsigned int intra_cu4_mode26_en   : 1   ; /* [26] */
        unsigned int intra_cu4_mode27_en   : 1   ; /* [27] */
        unsigned int intra_cu4_mode28_en   : 1   ; /* [28] */
        unsigned int intra_cu4_mode29_en   : 1   ; /* [29] */
        unsigned int intra_cu4_mode30_en   : 1   ; /* [30] */
        unsigned int intra_cu4_mode31_en   : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl4_ang_oen;

/* Define the union u_vedu_intra_chnl4_ang_1en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu4_mode32_en   : 1   ; /* [0] */
        unsigned int intra_cu4_mode33_en   : 1   ; /* [1] */
        unsigned int intra_cu4_mode34_en   : 1   ; /* [2] */
        unsigned int reserved_0            : 29  ; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl4_ang_1en;

/* Define the union u_vedu_intra_chnl8_ang_0en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu8_mode0_en    : 1   ; /* [0] */
        unsigned int intra_cu8_mode1_en    : 1   ; /* [1] */
        unsigned int intra_cu8_mode2_en    : 1   ; /* [2] */
        unsigned int intra_cu8_mode3_en    : 1   ; /* [3] */
        unsigned int intra_cu8_mode4_en    : 1   ; /* [4] */
        unsigned int intra_cu8_mode5_en    : 1   ; /* [5] */
        unsigned int intra_cu8_mode6_en    : 1   ; /* [6] */
        unsigned int intra_cu8_mode7_en    : 1   ; /* [7] */
        unsigned int intra_cu8_mode8_en    : 1   ; /* [8] */
        unsigned int intra_cu8_mode9_en    : 1   ; /* [9] */
        unsigned int intra_cu8_mode10_en   : 1   ; /* [10] */
        unsigned int intra_cu8_mode11_en   : 1   ; /* [11] */
        unsigned int intra_cu8_mode12_en   : 1   ; /* [12] */
        unsigned int intra_cu8_mode13_en   : 1   ; /* [13] */
        unsigned int intra_cu8_mode14_en   : 1   ; /* [14] */
        unsigned int intra_cu8_mode15_en   : 1   ; /* [15] */
        unsigned int intra_cu8_mode16_en   : 1   ; /* [16] */
        unsigned int intra_cu8_mode17_en   : 1   ; /* [17] */
        unsigned int intra_cu8_mode18_en   : 1   ; /* [18] */
        unsigned int intra_cu8_mode19_en   : 1   ; /* [19] */
        unsigned int intra_cu8_mode20_en   : 1   ; /* [20] */
        unsigned int intra_cu8_mode21_en   : 1   ; /* [21] */
        unsigned int intra_cu8_mode22_en   : 1   ; /* [22] */
        unsigned int intra_cu8_mode23_en   : 1   ; /* [23] */
        unsigned int intra_cu8_mode24_en   : 1   ; /* [24] */
        unsigned int intra_cu8_mode25_en   : 1   ; /* [25] */
        unsigned int intra_cu8_mode26_en   : 1   ; /* [26] */
        unsigned int intra_cu8_mode27_en   : 1   ; /* [27] */
        unsigned int intra_cu8_mode28_en   : 1   ; /* [28] */
        unsigned int intra_cu8_mode29_en   : 1   ; /* [29] */
        unsigned int intra_cu8_mode30_en   : 1   ; /* [30] */
        unsigned int intra_cu8_mode31_en   : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl8_ang_0en;

/* Define the union u_vedu_intra_chnl8_ang_1en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu8_mode32_en   : 1   ; /* [0] */
        unsigned int intra_cu8_mode33_en   : 1   ; /* [1] */
        unsigned int intra_cu8_mode34_en   : 1   ; /* [2] */
        unsigned int reserved_0            : 29  ; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl8_ang_1en;

/* Define the union u_vedu_intra_chnl16_ang_0en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu16_mode0_en   : 1   ; /* [0] */
        unsigned int intra_cu16_mode1_en   : 1   ; /* [1] */
        unsigned int intra_cu16_mode2_en   : 1   ; /* [2] */
        unsigned int intra_cu16_mode3_en   : 1   ; /* [3] */
        unsigned int intra_cu16_mode4_en   : 1   ; /* [4] */
        unsigned int intra_cu16_mode5_en   : 1   ; /* [5] */
        unsigned int intra_cu16_mode6_en   : 1   ; /* [6] */
        unsigned int intra_cu16_mode7_en   : 1   ; /* [7] */
        unsigned int intra_cu16_mode8_en   : 1   ; /* [8] */
        unsigned int intra_cu16_mode9_en   : 1   ; /* [9] */
        unsigned int intra_cu16_mode10_en  : 1   ; /* [10] */
        unsigned int intra_cu16_mode11_en  : 1   ; /* [11] */
        unsigned int intra_cu16_mode12_en  : 1   ; /* [12] */
        unsigned int intra_cu16_mode13_en  : 1   ; /* [13] */
        unsigned int intra_cu16_mode14_en  : 1   ; /* [14] */
        unsigned int intra_cu16_mode15_en  : 1   ; /* [15] */
        unsigned int intra_cu16_mode16_en  : 1   ; /* [16] */
        unsigned int intra_cu16_mode17_en  : 1   ; /* [17] */
        unsigned int intra_cu16_mode18_en  : 1   ; /* [18] */
        unsigned int intra_cu16_mode19_en  : 1   ; /* [19] */
        unsigned int intra_cu16_mode20_en  : 1   ; /* [20] */
        unsigned int intra_cu16_mode21_en  : 1   ; /* [21] */
        unsigned int intra_cu16_mode22_en  : 1   ; /* [22] */
        unsigned int intra_cu16_mode23_en  : 1   ; /* [23] */
        unsigned int intra_cu16_mode24_en  : 1   ; /* [24] */
        unsigned int intra_cu16_mode25_en  : 1   ; /* [25] */
        unsigned int intra_cu16_mode26_en  : 1   ; /* [26] */
        unsigned int intra_cu16_mode27_en  : 1   ; /* [27] */
        unsigned int intra_cu16_mode28_en  : 1   ; /* [28] */
        unsigned int intra_cu16_mode29_en  : 1   ; /* [29] */
        unsigned int intra_cu16_mode30_en  : 1   ; /* [30] */
        unsigned int intra_cu16_mode31_en  : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl16_ang_0en;

/* Define the union u_vedu_intra_chnl16_ang_1en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu16_mode32_en  : 1   ; /* [0] */
        unsigned int intra_cu16_mode33_en  : 1   ; /* [1] */
        unsigned int intra_cu16_mode34_en  : 1   ; /* [2] */
        unsigned int reserved_0            : 29  ; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;

} u_vedu_intra_chnl16_ang_1en;

/* Define the union u_vedu_intra_chnl32_ang_0en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu32_mode0_en   : 1   ; /* [0] */
        unsigned int intra_cu32_mode1_en   : 1   ; /* [1] */
        unsigned int intra_cu32_mode2_en   : 1   ; /* [2] */
        unsigned int intra_cu32_mode3_en   : 1   ; /* [3] */
        unsigned int intra_cu32_mode4_en   : 1   ; /* [4] */
        unsigned int intra_cu32_mode5_en   : 1   ; /* [5] */
        unsigned int intra_cu32_mode6_en   : 1   ; /* [6] */
        unsigned int intra_cu32_mode7_en   : 1   ; /* [7] */
        unsigned int intra_cu32_mode8_en   : 1   ; /* [8] */
        unsigned int intra_cu32_mode9_en   : 1   ; /* [9] */
        unsigned int intra_cu32_mode10_en  : 1   ; /* [10] */
        unsigned int intra_cu32_mode11_en  : 1   ; /* [11] */
        unsigned int intra_cu32_mode12_en  : 1   ; /* [12] */
        unsigned int intra_cu32_mode13_en  : 1   ; /* [13] */
        unsigned int intra_cu32_mode14_en  : 1   ; /* [14] */
        unsigned int intra_cu32_mode15_en  : 1   ; /* [15] */
        unsigned int intra_cu32_mode16_en  : 1   ; /* [16] */
        unsigned int intra_cu32_mode17_en  : 1   ; /* [17] */
        unsigned int intra_cu32_mode18_en  : 1   ; /* [18] */
        unsigned int intra_cu32_mode19_en  : 1   ; /* [19] */
        unsigned int intra_cu32_mode20_en  : 1   ; /* [20] */
        unsigned int intra_cu32_mode21_en  : 1   ; /* [21] */
        unsigned int intra_cu32_mode22_en  : 1   ; /* [22] */
        unsigned int intra_cu32_mode23_en  : 1   ; /* [23] */
        unsigned int intra_cu32_mode24_en  : 1   ; /* [24] */
        unsigned int intra_cu32_mode25_en  : 1   ; /* [25] */
        unsigned int intra_cu32_mode26_en  : 1   ; /* [26] */
        unsigned int intra_cu32_mode27_en  : 1   ; /* [27] */
        unsigned int intra_cu32_mode28_en  : 1   ; /* [28] */
        unsigned int intra_cu32_mode29_en  : 1   ; /* [29] */
        unsigned int intra_cu32_mode30_en  : 1   ; /* [30] */
        unsigned int intra_cu32_mode31_en  : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl32_ang_0en;

/* Define the union u_vedu_intra_chnl32_ang_1en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu32_mode32_en  : 1   ; /* [0] */
        unsigned int intra_cu32_mode33_en  : 1   ; /* [1] */
        unsigned int intra_cu32_mode34_en  : 1   ; /* [2] */
        unsigned int reserved_0            : 29  ; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_chnl32_ang_1en;

/* Define the union u_vedu_pack_cu_parameter */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 4   ; /* [3..0] */
        unsigned int pack_vcpi2cu_qp_min_cu_size : 3   ; /* [6..4] */
        unsigned int reserved_1            : 25  ; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pack_cu_parameter;

/* Define the union u_vedu_rgb2yuv_coef_p0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_coef_00       : 10  ; /* [9..0] */
        unsigned int rgb2yuv_coef_01       : 10  ; /* [19..10] */
        unsigned int rgb2yuv_coef_02       : 10  ; /* [29..20] */
        unsigned int reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_coef_p0;

/* Define the union u_vedu_rgb2yuv_coef_p1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_coef_10       : 10  ; /* [9..0] */
        unsigned int rgb2yuv_coef_11       : 10  ; /* [19..10] */
        unsigned int rgb2yuv_coef_12       : 10  ; /* [29..20] */
        unsigned int reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_coef_p1;

/* Define the union u_vedu_rgb2yuv_coef_p2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_coef_20       : 10  ; /* [9..0] */
        unsigned int rgb2yuv_coef_21       : 10  ; /* [19..10] */
        unsigned int rgb2yuv_coef_22       : 10  ; /* [29..20] */
        unsigned int reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_coef_p2;

/* Define the union u_vedu_rgb2yuv_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_offset_y      : 10  ; /* [9..0] */
        unsigned int rgb2yuv_offset_u      : 10  ; /* [19..10] */
        unsigned int rgb2yuv_offset_v      : 10  ; /* [29..20] */
        unsigned int reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_offset;

/* Define the union u_vedu_rgb2yuv_clip_thr_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_clip_min_y    : 10  ; /* [9..0] */
        unsigned int reserved_0            : 6   ; /* [15..10] */
        unsigned int rgb2yuv_clip_max_y    : 10  ; /* [25..16] */
        unsigned int reserved_1            : 6   ; /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_clip_thr_y;

/* Define the union u_vedu_rgb2yuv_clip_thr_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_clip_min_c    : 10  ; /* [9..0] */
        unsigned int reserved_0            : 6   ; /* [15..10] */
        unsigned int rgb2yuv_clip_max_c    : 10  ; /* [25..16] */
        unsigned int reserved_1            : 6   ; /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_clip_thr_c;

/* Define the union u_vedu_rgb2yuv_shift_width */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb2yuv_shift_width   : 4   ; /* [3..0] */
        unsigned int reserved_0            : 28  ; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_rgb2yuv_shift_width;

/* Define the union u_vedu_vcpi_osd_enable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd0_en          : 1   ; /* [0] */
        unsigned int vcpi_osd1_en          : 1   ; /* [1] */
        unsigned int vcpi_osd2_en          : 1   ; /* [2] */
        unsigned int vcpi_osd3_en          : 1   ; /* [3] */
        unsigned int vcpi_osd4_en          : 1   ; /* [4] */
        unsigned int vcpi_osd5_en          : 1   ; /* [5] */
        unsigned int vcpi_osd6_en          : 1   ; /* [6] */
        unsigned int vcpi_osd7_en          : 1   ; /* [7] */
        unsigned int vcpi_osd0_absqp       : 1   ; /* [8] */
        unsigned int vcpi_osd1_absqp       : 1   ; /* [9] */
        unsigned int vcpi_osd2_absqp       : 1   ; /* [10] */
        unsigned int vcpi_osd3_absqp       : 1   ; /* [11] */
        unsigned int vcpi_osd4_absqp       : 1   ; /* [12] */
        unsigned int vcpi_osd5_absqp       : 1   ; /* [13] */
        unsigned int vcpi_osd6_absqp       : 1   ; /* [14] */
        unsigned int vcpi_osd7_absqp       : 1   ; /* [15] */
        unsigned int vcpi_roi_osd_sel_0    : 1   ; /* [16] */
        unsigned int vcpi_roi_osd_sel_1    : 1   ; /* [17] */
        unsigned int vcpi_roi_osd_sel_2    : 1   ; /* [18] */
        unsigned int vcpi_roi_osd_sel_3    : 1   ; /* [19] */
        unsigned int vcpi_roi_osd_sel_4    : 1   ; /* [20] */
        unsigned int vcpi_roi_osd_sel_5    : 1   ; /* [21] */
        unsigned int vcpi_roi_osd_sel_6    : 1   ; /* [22] */
        unsigned int vcpi_roi_osd_sel_7    : 1   ; /* [23] */
        unsigned int vcpi_osd_en           : 1   ; /* [24] */
        unsigned int reserved_0            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_enable;

/* Define the union u_vedu_vcpi_osd_pos_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd0_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd0_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_0;

/* Define the union u_vedu_vcpi_osd_pos_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd1_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd1_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_1;

/* Define the union u_vedu_vcpi_osd_pos_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd2_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd2_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_2;

/* Define the union u_vedu_vcpi_osd_pos_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd3_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd3_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_3;

/* Define the union u_vedu_vcpi_osd_pos_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd4_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd4_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_4;

/* Define the union u_vedu_vcpi_osd_pos_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd5_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd5_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_5;

/* Define the union u_vedu_vcpi_osd_pos_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd6_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd6_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_6;

/* Define the union u_vedu_vcpi_osd_pos_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd7_x           : 16  ; /* [15..0] */
        unsigned int vcpi_osd7_y           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_pos_7;

/* Define the union u_vedu_vcpi_osd_size_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd0_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd0_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_0;

/* Define the union u_vedu_vcpi_osd_size_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd1_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd1_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_1;

/* Define the union u_vedu_vcpi_osd_size_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd2_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd2_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_2;

/* Define the union u_vedu_vcpi_osd_size_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd3_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd3_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_3;

/* Define the union u_vedu_vcpi_osd_size_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd4_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd4_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_4;

/* Define the union u_vedu_vcpi_osd_size_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd5_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd5_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_5;

/* Define the union u_vedu_vcpi_osd_size_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd6_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd6_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_6;

/* Define the union u_vedu_vcpi_osd_size_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd7_w           : 16  ; /* [15..0] */
        unsigned int vcpi_osd7_h           : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_size_7;

/* Define the union u_vedu_vcpi_osd_layerid */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd0_layer_id    : 3   ; /* [2..0] */
        unsigned int reserved_0            : 1   ; /* [3] */
        unsigned int vcpi_osd1_layer_id    : 3   ; /* [6..4] */
        unsigned int reserved_1            : 1   ; /* [7] */
        unsigned int vcpi_osd2_layer_id    : 3   ; /* [10..8] */
        unsigned int reserved_2            : 1   ; /* [11] */
        unsigned int vcpi_osd3_layer_id    : 3   ; /* [14..12] */
        unsigned int reserved_3            : 1   ; /* [15] */
        unsigned int vcpi_osd4_layer_id    : 3   ; /* [18..16] */
        unsigned int reserved_4            : 1   ; /* [19] */
        unsigned int vcpi_osd5_layer_id    : 3   ; /* [22..20] */
        unsigned int reserved_5            : 1   ; /* [23] */
        unsigned int vcpi_osd6_layer_id    : 3   ; /* [26..24] */
        unsigned int reserved_6            : 1   ; /* [27] */
        unsigned int vcpi_osd7_layer_id    : 3   ; /* [30..28] */
        unsigned int reserved_7            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_layerid;

/* Define the union u_vedu_vcpi_osd_qp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd0_qp          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 1   ; /* [7] */
        unsigned int vcpi_osd1_qp          : 7   ; /* [14..8] */
        unsigned int reserved_1            : 1   ; /* [15] */
        unsigned int vcpi_osd2_qp          : 7   ; /* [22..16] */
        unsigned int reserved_2            : 1   ; /* [23] */
        unsigned int vcpi_osd3_qp          : 7   ; /* [30..24] */
        unsigned int reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_qp0;

/* Define the union u_vedu_vcpi_osd_qp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_osd4_qp          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 1   ; /* [7] */
        unsigned int vcpi_osd5_qp          : 7   ; /* [14..8] */
        unsigned int reserved_1            : 1   ; /* [15] */
        unsigned int vcpi_osd6_qp          : 7   ; /* [22..16] */
        unsigned int reserved_2            : 1   ; /* [23] */
        unsigned int vcpi_osd7_qp          : 7   ; /* [30..24] */
        unsigned int reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_osd_qp1;

/* Define the union u_vedu_curld_osd01_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd0_alpha0     : 8   ; /* [7..0] */
        unsigned int curld_osd0_alpha1     : 8   ; /* [15..8] */
        unsigned int curld_osd1_alpha0     : 8   ; /* [23..16] */
        unsigned int curld_osd1_alpha1     : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd01_alpha;

/* Define the union u_vedu_curld_osd23_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd2_alpha0     : 8   ; /* [7..0] */
        unsigned int curld_osd2_alpha1     : 8   ; /* [15..8] */
        unsigned int curld_osd3_alpha0     : 8   ; /* [23..16] */
        unsigned int curld_osd3_alpha1     : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd23_alpha;

/* Define the union u_vedu_curld_osd45_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd4_alpha0     : 8   ; /* [7..0] */
        unsigned int curld_osd4_alpha1     : 8   ; /* [15..8] */
        unsigned int curld_osd5_alpha0     : 8   ; /* [23..16] */
        unsigned int curld_osd5_alpha1     : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd45_alpha;

/* Define the union u_vedu_curld_osd67_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd6_alpha0     : 8   ; /* [7..0] */
        unsigned int curld_osd6_alpha1     : 8   ; /* [15..8] */
        unsigned int curld_osd7_alpha0     : 8   ; /* [23..16] */
        unsigned int curld_osd7_alpha1     : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd67_alpha;

/* Define the union u_vedu_curld_osd_galpha0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd0_global_alpha : 8   ; /* [7..0] */
        unsigned int curld_osd1_global_alpha : 8   ; /* [15..8] */
        unsigned int curld_osd2_global_alpha : 8   ; /* [23..16] */
        unsigned int curld_osd3_global_alpha : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd_galpha0;

/* Define the union u_vedu_curld_osd_galpha1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd4_global_alpha : 8   ; /* [7..0] */
        unsigned int curld_osd5_global_alpha : 8   ; /* [15..8] */
        unsigned int curld_osd6_global_alpha : 8   ; /* [23..16] */
        unsigned int curld_osd7_global_alpha : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd_galpha1;

/* Define the union u_vedu_curld_osd01_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd0_stride     : 16  ; /* [15..0] */
        unsigned int curld_osd1_stride     : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd01_stride;

/* Define the union u_vedu_curld_osd23_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd2_stride     : 16  ; /* [15..0] */
        unsigned int curld_osd3_stride     : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd23_stride;

/* Define the union u_vedu_curld_osd45_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd4_stride     : 16  ; /* [15..0] */
        unsigned int curld_osd5_stride     : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd45_stride;

/* Define the union u_vedu_curld_osd67_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_osd6_stride     : 16  ; /* [15..0] */
        unsigned int curld_osd7_stride     : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_osd67_stride;

/* Define the union u_vedu_vcpi_vlc_config */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_ref_idc          : 2   ; /* [1..0] */
        unsigned int reserved_0            : 2   ; /* [3..2] */
        unsigned int vcpi_cabac_init_idc   : 2   ; /* [5..4] */
        unsigned int reserved_1            : 6   ; /* [11..6] */
        unsigned int vcpi_byte_stuffing    : 1   ; /* [12] */
        unsigned int reserved_2            : 19  ; /* [31..13] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_vlc_config;

/* Define the union u_vedu_cabac_slchdr_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cabac_slchdr_size_part1 : 5   ; /* [4..0] */
        unsigned int reserved_0            : 11  ; /* [15..5] */
        unsigned int cabac_slchdr_size_part2 : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_cabac_slchdr_size;

/* Define the union u_vedu_cabac_slchdr_part1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cabac_slchdr_part1    : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_cabac_slchdr_part1;

/* Define the union u_vedu_cabac_slchdr_size_i */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cabac_slchdr_size_part1_i : 5   ; /* [4..0] */
        unsigned int reserved_0            : 11  ; /* [15..5] */
        unsigned int cabac_slchdr_size_part2_i : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_cabac_slchdr_size_i;

/* Define the union u_vedu_cabac_slchdr_part1_i */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cabac_slchdr_part1_i  : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_cabac_slchdr_part1_i;

/* Define the union u_vedu_vlc_slchdrpara */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlc_markingbit        : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int vlc_reorderbit        : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int vlc_parabit           : 7   ; /* [22..16] */
        unsigned int reserved_2            : 9   ; /* [31..23] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vlc_slchdrpara;

/* Define the union u_vedu_vlc_svc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlc_svc_en            : 1   ; /* [0] */
        unsigned int vlc_svc_strm          : 24  ; /* [24..1] */
        unsigned int reserved_0            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vlc_svc;

/* Define the union u_vedu_vlc_slchdrpara_i */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlc_markingbit_i      : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int vlc_reorderbit_i      : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int vlc_parabit_i         : 7   ; /* [22..16] */
        unsigned int reserved_2            : 9   ; /* [31..23] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vlc_slchdrpara_i;

/* Define the union u_vedu_ice_v3r2_seg_256x1_dcmp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 1   ; /* [0] */
        unsigned int is_lossless           : 1   ; /* [1] */
        unsigned int reserved_1            : 14  ; /* [15..2] */
        unsigned int head_tword            : 2   ; /* [17..16] */
        unsigned int reserved_2            : 14  ; /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ice_v3r2_seg_256x1_dcmp_ctrl;

/* Define the union u_vedu_vcpi_low_power */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_intra_lowpow_en  : 1   ; /* [0] */
        unsigned int vcpi_fme_lowpow_en    : 1   ; /* [1] */
        unsigned int vcpi_ime_lowpow_en    : 1   ; /* [2] */
        unsigned int reserved_0            : 13  ; /* [15..3] */
        unsigned int vcpi_tqitq_gtck_en    : 1   ; /* [16] */
        unsigned int vcpi_mrg_gtck_en      : 1   ; /* [17] */
        unsigned int reserved_1            : 1   ; /* [18] */
        unsigned int vcpi_clkgate_en       : 2   ; /* [20..19] */
        unsigned int vcpi_mem_clkgate_en   : 1   ; /* [21] */
        unsigned int vcpi_ref1_clkgate_en  : 1   ; /* [22] */
        unsigned int reserved_2            : 1   ; /* [23] */
        unsigned int vcpi_osd_clkgete_en   : 1   ; /* [24] */
        unsigned int vcpi_rec_cmp_clkgate_en : 1   ; /* [25] */
        unsigned int vcpi_curld_dcmp_clkgate_en : 1   ; /* [26] */
        unsigned int vcpi_refld_dcmp_clkgate_en : 1   ; /* [27] */
        unsigned int vcpi_cpi_clkgate_en   : 1   ; /* [28] */
        unsigned int vcpi_vlc_clkgate_en   : 1   ; /* [29] */
        unsigned int reserved_3            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_low_power;

/* Define the union u_vedu_vcpi_pre_judge_ext_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 1   ; /* [0] */
        unsigned int vcpi_pblk_pre_en      : 1   ; /* [1] */
        unsigned int vcpi_force_inter      : 1   ; /* [2] */
        unsigned int vcpi_pintra_inter_flag_disable : 1   ; /* [3] */
        unsigned int vcpi_ext_edge_en      : 1   ; /* [4] */
        unsigned int reserved_1            : 27  ; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pre_judge_ext_en;

/* Define the union u_vedu_vcpi_iblk_pre_mv_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_iblk_pre_mv_dif_thr0 : 8   ; /* [7..0] */
        unsigned int vcpi_iblk_pre_mv_dif_thr1 : 8   ; /* [15..8] */
        unsigned int vcpi_iblk_pre_mvx_thr : 8   ; /* [23..16] */
        unsigned int vcpi_iblk_pre_mvy_thr : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_iblk_pre_mv_thr;

/* Define the union u_vedu_vcpi_iblk_pre_cost_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_iblk_pre_cost_thr_h264 : 16  ; /* [15..0] */
        unsigned int pme_intrablk_det_cost_thr1 : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_iblk_pre_cost_thr;

/* Define the union u_vedu_vcpi_pre_judge_cost_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_iblk_pre_cost_thr : 16  ; /* [15..0] */
        unsigned int vcpi_pblk_pre_cost_thr : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pre_judge_cost_thr;

/* Define the union u_vedu_vcpi_pme_param */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 1   ; /* [0] */
        unsigned int vcpi_move_sad_en      : 1   ; /* [1] */
        unsigned int reserved_1            : 14  ; /* [15..2] */
        unsigned int vcpi_pblk_pre_mvx_thr : 8   ; /* [23..16] */
        unsigned int vcpi_pblk_pre_mvy_thr : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pme_param;

/* Define the union u_vedu_pme_pblk_pre1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_pblk_pre_mv_dif_thr1 : 8   ; /* [7..0] */
        unsigned int pme_pblk_pre_mv_dif_thr0 : 8   ; /* [15..8] */
        unsigned int pme_pblk_pre_mv_dif_cost_thr : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_pblk_pre1;

/* Define the union u_vedu_pme_pblk_pre2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_pblk_pre_madi_times : 8   ; /* [7..0] */
        unsigned int pme_pblk_pre_offset   : 8   ; /* [15..8] */
        unsigned int pme_pblk_pre_gain     : 8   ; /* [23..16] */
        unsigned int reserved_0            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_pblk_pre2;

/* Define the union u_vedu_pme_sw_adapt_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_l0_psw_adapt_en   : 1   ; /* [0] */
        unsigned int pme_l1_psw_adapt_en   : 1   ; /* [1] */
        unsigned int reserved_0            : 30  ; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_sw_adapt_en;

/* Define the union u_vedu_pme_window_size0_l0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_l0_win0_width     : 10  ; /* [9..0] */
        unsigned int reserved_0            : 6   ; /* [15..10] */
        unsigned int pme_l0_win0_height    : 9   ; /* [24..16] */
        unsigned int reserved_1            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_window_size0_l0;

/* Define the union u_vedu_pme_window_size0_l1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_l1_win0_width     : 10  ; /* [9..0] */
        unsigned int reserved_0            : 6   ; /* [15..10] */
        unsigned int pme_l1_win0_height    : 9   ; /* [24..16] */
        unsigned int reserved_1            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_window_size0_l1;

/* Define the union u_vedu_pme_psw_lpw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_psw_lp_diff_thy   : 4   ; /* [3..0] */
        unsigned int reserved_0            : 4   ; /* [7..4] */
        unsigned int pme_psw_lp_diff_thx   : 4   ; /* [11..8] */
        unsigned int reserved_1            : 20  ; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_psw_lpw;

/* Define the union u_vedu_pme_skip_pre */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_skipblk_pre_cost_thr : 16  ; /* [15..0] */
        unsigned int pme_skipblk_pre_en    : 1   ; /* [16] */
        unsigned int reserved_0            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_skip_pre;

/* Define the union u_vedu_pme_intrablk_det */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_intrablk_det_cost_thr0 : 16  ; /* [15..0] */
        unsigned int pme_pskip_mvy_consistency_thr : 8   ; /* [23..16] */
        unsigned int pme_pskip_mvx_consistency_thr : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_intrablk_det;

/* Define the union u_vedu_pme_skip_flag */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_pskip_strongedge_madi_thr : 8   ; /* [7..0] */
        unsigned int vcpi_pskip_strongedge_madi_times : 8   ; /* [15..8] */
        unsigned int vcpi_pskip_flatregion_madi_thr : 8   ; /* [23..16] */
        unsigned int reserved_0            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_skip_flag;

/* Define the union u_vedu_pme_tr_weightx */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_tr_weightx_0      : 9   ; /* [8..0] */
        unsigned int pme_tr_weightx_1      : 9   ; /* [17..9] */
        unsigned int pme_tr_weightx_2      : 9   ; /* [26..18] */
        unsigned int reserved_0            : 5   ; /* [31..27] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_tr_weightx;

/* Define the union u_vedu_pme_tr_weighty */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_tr_weighty_0      : 8   ; /* [7..0] */
        unsigned int pme_tr_weighty_1      : 8   ; /* [15..8] */
        unsigned int pme_tr_weighty_2      : 8   ; /* [23..16] */
        unsigned int reserved_0            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_tr_weighty;

/* Define the union u_vedu_pme_sr_weight */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_sr_weight_0       : 4   ; /* [3..0] */
        unsigned int pme_sr_weight_1       : 4   ; /* [7..4] */
        unsigned int pme_sr_weight_2       : 4   ; /* [11..8] */
        unsigned int reserved_0            : 20  ; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_sr_weight;

/* Define the union u_vedu_ime_fme_lpow_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_lowpow_fme_thr0   : 6   ; /* [5..0] */
        unsigned int ime_lowpow_fme_thr1   : 6   ; /* [11..6] */
        unsigned int reserved_0            : 20  ; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_fme_lpow_thr;

/* Define the union u_vedu_fme_pu64_lwp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_pu64_lwp_flag     : 1   ; /* [0] */
        unsigned int reserved_0            : 3   ; /* [3..1] */
        unsigned int vcpi_detect_low3_en   : 1   ; /* [4] */
        unsigned int vcpi_strong_edge_with_space_en : 1   ; /* [5] */
        unsigned int reserved_1            : 2   ; /* [7..6] */
        unsigned int reserved_2            : 24  ; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_fme_pu64_lwp;

/* Define the union u_vedu_mrg_force_zero_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_force_zero_en     : 1   ; /* [0] */
        unsigned int mrg_force_y_zero_en   : 1   ; /* [1] */
        unsigned int mrg_force_u_zero_en   : 1   ; /* [2] */
        unsigned int mrg_force_v_zero_en   : 1   ; /* [3] */
        unsigned int fme_rdo_lpw_en        : 1   ; /* [4] */
        unsigned int dct4_en               : 1   ; /* [5] */
        unsigned int force_adapt_en        : 1   ; /* [6] */
        unsigned int reserved_0            : 5   ; /* [11..7] */
        unsigned int rqt_bias_weight       : 4   ; /* [15..12] */
        unsigned int fme_rdo_lpw_th        : 10  ; /* [25..16] */
        unsigned int mrg_skip_weight_en    : 1   ; /* [26] */
        unsigned int reserved_1            : 5   ; /* [31..27] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_force_zero_en;

/* Define the union u_vedu_mrg_force_skip_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int inter32_use_tu16_en   : 1   ; /* [0] */
        unsigned int mrg_not_use_sad_en    : 1   ; /* [1] */
        unsigned int mrg_detect_intercu32_use_tu16 : 1   ; /* [2] */
        unsigned int reserved_0            : 2   ; /* [4..3] */
        unsigned int mrg_not_use_sad_th    : 18  ; /* [22..5] */
        unsigned int reserved_1            : 9   ; /* [31..23] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_force_skip_en;

/* Define the union u_vedu_ime_inter_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_layer3to2_en      : 1   ; /* [0] */
        unsigned int ime_inter8x8_en       : 1   ; /* [1] */
        unsigned int reserved_0            : 2   ; /* [3..2] */
        unsigned int ime_high3pre_en       : 2   ; /* [5..4] */
        unsigned int reserved_1            : 2   ; /* [7..6] */
        unsigned int ime_intra4_lowpow_en  : 1   ; /* [8] */
        unsigned int reserved_2            : 23  ; /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_inter_mode;

/* Define the union u_vedu_ime_layer3to2_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_layer3to2_thr0    : 10  ; /* [9..0] */
        unsigned int reserved_0            : 6   ; /* [15..10] */
        unsigned int ime_layer3to2_thr1    : 10  ; /* [25..16] */
        unsigned int reserved_1            : 6   ; /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_layer3to2_thr;

/* Define the union u_vedu_ime_layer3to2_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_layer3to2_cost_diff_thr : 10  ; /* [9..0] */
        unsigned int reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_layer3to2_thr1;

/* Define the union u_vedu_ime_layer3to1_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_layer3to1_en      : 1   ; /* [0] */
        unsigned int reserved_0            : 7   ; /* [7..1] */
        unsigned int ime_layer3to1_pu64_madi_thr : 7   ; /* [14..8] */
        unsigned int reserved_1            : 17  ; /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_layer3to1_thr;

/* Define the union u_vedu_ime_layer3to1_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_layer3to1_pu32_cost_thr : 16  ; /* [15..0] */
        unsigned int ime_layer3to1_pu64_cost_thr : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_layer3to1_thr1;

/* Define the union u_vedu_vcpi_intra32_low_power */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_intra32_low_power_thr : 16  ; /* [15..0] */
        unsigned int vcpi_intra32_low_power_en : 1   ; /* [16] */
        unsigned int reserved_0            : 3   ; /* [19..17] */
        unsigned int vcpi_intra32_low_power_gain : 6   ; /* [25..20] */
        unsigned int vcpi_intra32_low_power_offset : 6   ; /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_intra32_low_power;

/* Define the union u_vedu_vcpi_intra16_low_power */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_intra16_low_power_thr : 16  ; /* [15..0] */
        unsigned int vcpi_intra16_low_power_en : 1   ; /* [16] */
        unsigned int reserved_0            : 3   ; /* [19..17] */
        unsigned int vcpi_intra16_low_power_gain : 6   ; /* [25..20] */
        unsigned int vcpi_intra16_low_power_offset : 6   ; /* [31..26] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_intra16_low_power;

/* Define the union u_vedu_pme_intra_lowpow */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_intra16_madi_thr  : 8   ; /* [7..0] */
        unsigned int pme_intra32_madi_thr  : 8   ; /* [15..8] */
        unsigned int pme_intra_lowpow_en   : 1   ; /* [16] */
        unsigned int reserved_0            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_intra_lowpow;

/* Define the union u_vedu_intra_low_pow0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cu16_weak_ang_thr     : 4   ; /* [3..0] */
        unsigned int cu16_medium_ang_thr   : 4   ; /* [7..4] */
        unsigned int cu16_strong_ang_thr   : 4   ; /* [11..8] */
        unsigned int cu16_rdo_num          : 3   ; /* [14..12] */
        unsigned int cu16_adaptive_reduce_rdo_en : 1   ; /* [15] */
        unsigned int cu32_weak_ang_thr     : 4   ; /* [19..16] */
        unsigned int cu32_medium_ang_thr   : 4   ; /* [23..20] */
        unsigned int cu32_strong_ang_thr   : 4   ; /* [27..24] */
        unsigned int cu32_rdo_num          : 3   ; /* [30..28] */
        unsigned int cu32_adaptive_reduce_rdo_en : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_low_pow0;

/* Define the union u_vedu_intra_low_pow1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_force_cu16_low_pow : 1   ; /* [0] */
        unsigned int reserved_0            : 3   ; /* [3..1] */
        unsigned int vcpi_force_cu32_low_pow : 1   ; /* [4] */
        unsigned int reserved_1            : 3   ; /* [7..5] */
        unsigned int vcpi_detect_close_intra32_en : 1   ; /* [8] */
        unsigned int reserved_2            : 23  ; /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_low_pow1;

/* Define the union u_vedu_vcpi_intra_inter_cu_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_intra_cu_en      : 4   ; /* [3..0] */
        unsigned int vcpi_ipcm_en          : 1   ; /* [4] */
        unsigned int vcpi_intra_h264_cutdiag : 1   ; /* [5] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int vcpi_fme_cu_en        : 4   ; /* [11..8] */
        unsigned int vcpi_mrg_cu_en        : 4   ; /* [15..12] */
        unsigned int reserved_1            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_intra_inter_cu_en;

/* Define the union u_vedu_pack_pcm_parameter */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pack_vcpi2pu_log2_min_ipcm_cbsizey : 3   ; /* [2..0] */
        unsigned int reserved_0            : 1   ; /* [3] */
        unsigned int pack_vcpi2pu_log2_max_ipcm_cbsizey : 3   ; /* [6..4] */
        unsigned int reserved_1            : 25  ; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pack_pcm_parameter;

/* Define the union u_vedu_qpg_cu32_delta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_cu32_delta_low    : 4   ; /* [3..0] */
        unsigned int qpg_cu32_delta_high   : 4   ; /* [7..4] */
        unsigned int reserved_0            : 24  ; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_cu32_delta;

/* Define the union u_vedu_qpg_qp_restrain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_qp_restrain_madi_thr : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_qp_restrain_en    : 1   ; /* [8] */
        unsigned int reserved_1            : 3   ; /* [11..9] */
        unsigned int qpg_qp_restrain_delta_blk16 : 4   ; /* [15..12] */
        unsigned int qpg_qp_restrain_delta_blk32 : 4   ; /* [19..16] */
        unsigned int reserved_2            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_restrain;

/* Define the union u_vedu_pme_qpg_rc_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_min_sad_thr_offset : 8   ; /* [7..0] */
        unsigned int pme_min_sad_thr_gain  : 4   ; /* [11..8] */
        unsigned int reserved_0            : 8   ; /* [19..12] */
        unsigned int pme_min_sad_thr_offset_cur : 8   ; /* [27..20] */
        unsigned int pme_min_sad_thr_gain_cur : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_qpg_rc_thr1;

/* Define the union u_vedu_qpg_cu_min_sad_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_min_sad_level     : 8   ; /* [7..0] */
        unsigned int reserved_0            : 8   ; /* [15..8] */
        unsigned int qpg_low_min_sad_mode  : 2   ; /* [17..16] */
        unsigned int reserved_1            : 2   ; /* [19..18] */
        unsigned int qpg_min_sad_madi_en   : 1   ; /* [20] */
        unsigned int qpg_min_sad_qp_restrain_en : 1   ; /* [21] */
        unsigned int reserved_2            : 10  ; /* [31..22] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_cu_min_sad_reg;

/* Define the union u_vedu_qpg_lambda_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda_qp_offset  : 5   ; /* [4..0] */
        unsigned int reserved_0            : 3   ; /* [7..5] */
        unsigned int qpg_rdo_lambda_choose_mode : 2   ; /* [9..8] */
        unsigned int reserved_1            : 2   ; /* [11..10] */
        unsigned int qpg_lambda_inter_stredge_en : 1   ; /* [12] */
        unsigned int reserved_2            : 19  ; /* [31..13] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_lambda_mode;

/* Define the union u_vedu_vcpi_sw_l0_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_sw_l0_width      : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int vcpi_sw_l0_height     : 6   ; /* [21..16] */
        unsigned int reserved_1            : 10  ; /* [31..22] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_sw_l0_size;

/* Define the union u_vedu_vcpi_sw_l1_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_sw_l1_width      : 7   ; /* [6..0] */
        unsigned int reserved_0            : 9   ; /* [15..7] */
        unsigned int vcpi_sw_l1_height     : 6   ; /* [21..16] */
        unsigned int reserved_1            : 10  ; /* [31..22] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_sw_l1_size;

/* Define the union u_vedu_pme_new_cost */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_cost_lamda0       : 4   ; /* [3..0] */
        unsigned int pme_cost_lamda1       : 4   ; /* [7..4] */
        unsigned int pme_cost_lamda2       : 4   ; /* [11..8] */
        unsigned int pme_new_cost_en       : 2   ; /* [13..12] */
        unsigned int reserved_0            : 2   ; /* [15..14] */
        unsigned int pme_cost_lamda_en     : 2   ; /* [17..16] */
        unsigned int pme_mvp3median_en     : 1   ; /* [18] */
        unsigned int reserved_1            : 13  ; /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_new_cost;

/* Define the union u_vedu_pme_new_lambda */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_new_lambda       : 8   ; /* [7..0] */
        unsigned int reserved_0            : 24  ; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_new_lambda;

/* Define the union u_vedu_pme_new_madi_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_new_madi_th0     : 8   ; /* [7..0] */
        unsigned int vcpi_new_madi_th1     : 8   ; /* [15..8] */
        unsigned int vcpi_new_madi_th2     : 8   ; /* [23..16] */
        unsigned int vcpi_new_madi_th3     : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_new_madi_th;

/* Define the union u_vedu_pme_cost_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_l0_cost_offset    : 16  ; /* [15..0] */
        unsigned int pme_l1_cost_offset    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_cost_offset;

/* Define the union u_vedu_pme_adjust_pmemv_h264 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_adjust_pmemv_dist_times : 8   ; /* [7..0] */
        unsigned int pme_adjust_pmemv_en   : 1   ; /* [8] */
        unsigned int reserved_0            : 23  ; /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_adjust_pmemv_h264;

/* Define the union u_vedu_pme_qpg_rc_th0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 16  ; /* [15..0] */
        unsigned int pme_madi_dif_thr      : 7   ; /* [22..16] */
        unsigned int pme_cur_madi_dif_thr  : 7   ; /* [29..23] */
        unsigned int reserved_1            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_qpg_rc_th0;

/* Define the union u_vedu_vlcst_descriptor */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlcst_idrind          : 1   ; /* [0] */
        unsigned int reserved_0            : 15  ; /* [15..1] */
        unsigned int vlcst_chnid           : 8   ; /* [23..16] */
        unsigned int reserved_1            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vlcst_descriptor;

/* Define the union u_vedu_curld_clip_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int curld_clip_luma_min   : 8   ; /* [7..0] */
        unsigned int curld_clip_luma_max   : 8   ; /* [15..8] */
        unsigned int curld_clip_chrm_min   : 8   ; /* [23..16] */
        unsigned int curld_clip_chrm_max   : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_curld_clip_thr;

/* Define the union u_vedu_tqitq_deadzone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tqitq_deadzone_intra_slice : 8   ; /* [7..0] */
        unsigned int tqitq_deadzone_inter_slice : 8   ; /* [15..8] */
        unsigned int tqitq_bitest_magth    : 3   ; /* [18..16] */
        unsigned int reserved_0            : 13  ; /* [31..19] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_tqitq_deadzone;

/* Define the union u_vedu_vcpi_pintra_texture_strength */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_pintra_intra8_texture_strength_multi : 3   ; /* [2..0] */
        unsigned int reserved_0            : 13  ; /* [15..3] */
        unsigned int vcpi_pintra_intra8_texture_strength : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pintra_texture_strength;

/* Define the union u_vedu_vctrl_roi_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_region0en       : 1   ; /* [0] */
        unsigned int vctrl_region1en       : 1   ; /* [1] */
        unsigned int vctrl_region2en       : 1   ; /* [2] */
        unsigned int vctrl_region3en       : 1   ; /* [3] */
        unsigned int vctrl_region4en       : 1   ; /* [4] */
        unsigned int vctrl_region5en       : 1   ; /* [5] */
        unsigned int vctrl_region6en       : 1   ; /* [6] */
        unsigned int vctrl_region7en       : 1   ; /* [7] */
        unsigned int vctrl_absqp0          : 1   ; /* [8] */
        unsigned int vctrl_absqp1          : 1   ; /* [9] */
        unsigned int vctrl_absqp2          : 1   ; /* [10] */
        unsigned int vctrl_absqp3          : 1   ; /* [11] */
        unsigned int vctrl_absqp4          : 1   ; /* [12] */
        unsigned int vctrl_absqp5          : 1   ; /* [13] */
        unsigned int vctrl_absqp6          : 1   ; /* [14] */
        unsigned int vctrl_absqp7          : 1   ; /* [15] */
        unsigned int vctrl_region0keep     : 1   ; /* [16] */
        unsigned int vctrl_region1keep     : 1   ; /* [17] */
        unsigned int vctrl_region2keep     : 1   ; /* [18] */
        unsigned int vctrl_region3keep     : 1   ; /* [19] */
        unsigned int vctrl_region4keep     : 1   ; /* [20] */
        unsigned int vctrl_region5keep     : 1   ; /* [21] */
        unsigned int vctrl_region6keep     : 1   ; /* [22] */
        unsigned int vctrl_region7keep     : 1   ; /* [23] */
        unsigned int vctrl_roi_en          : 1   ; /* [24] */
        unsigned int reserved_0            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_cfg0;

/* Define the union u_vedu_vctrl_roi_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_roiqp0          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 1   ; /* [7] */
        unsigned int vctrl_roiqp1          : 7   ; /* [14..8] */
        unsigned int reserved_1            : 1   ; /* [15] */
        unsigned int vctrl_roiqp2          : 7   ; /* [22..16] */
        unsigned int reserved_2            : 1   ; /* [23] */
        unsigned int vctrl_roiqp3          : 7   ; /* [30..24] */
        unsigned int reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_cfg1;

/* Define the union u_vedu_vctrl_roi_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_roiqp4          : 7   ; /* [6..0] */
        unsigned int reserved_0            : 1   ; /* [7] */
        unsigned int vctrl_roiqp5          : 7   ; /* [14..8] */
        unsigned int reserved_1            : 1   ; /* [15] */
        unsigned int vctrl_roiqp6          : 7   ; /* [22..16] */
        unsigned int reserved_2            : 1   ; /* [23] */
        unsigned int vctrl_roiqp7          : 7   ; /* [30..24] */
        unsigned int reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_cfg2;

/* Define the union u_vedu_vctrl_roi_size_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size0_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size0_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_0;

/* Define the union u_vedu_vctrl_roi_size_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size1_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size1_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_1;

/* Define the union u_vedu_vctrl_roi_size_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size2_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size2_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_2;

/* Define the union u_vedu_vctrl_roi_size_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size3_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size3_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_3;

/* Define the union u_vedu_vctrl_roi_size_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size4_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size4_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_4;

/* Define the union u_vedu_vctrl_roi_size_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size5_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size5_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_5;

/* Define the union u_vedu_vctrl_roi_size_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size6_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size6_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_6;

/* Define the union u_vedu_vctrl_roi_size_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_size7_roiwidth  : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vctrl_size7_roiheight : 13  ; /* [28..16] */
        unsigned int reserved_1            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_size_7;

/* Define the union u_vedu_vctrl_roi_start_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start0_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start0_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_0;

/* Define the union u_vedu_vctrl_roi_start_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start1_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start1_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_1;

/* Define the union u_vedu_vctrl_roi_start_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start2_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start2_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_2;

/* Define the union u_vedu_vctrl_roi_start_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start3_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start3_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_3;

/* Define the union u_vedu_vctrl_roi_start_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start4_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start4_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_4;

/* Define the union u_vedu_vctrl_roi_start_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start5_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start5_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_5;

/* Define the union u_vedu_vctrl_roi_start_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start6_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start6_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_6;

/* Define the union u_vedu_vctrl_roi_start_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_start7_roistartx : 12  ; /* [11..0] */
        unsigned int reserved_0            : 4   ; /* [15..12] */
        unsigned int vctrl_start7_roistarty : 12  ; /* [27..16] */
        unsigned int reserved_1            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_roi_start_7;

/* Define the union u_vedu_intra_bit_weight */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_bit_weight      : 4   ; /* [3..0] */
        unsigned int mpm8_th               : 3   ; /* [6..4] */
        unsigned int reserved_0            : 25  ; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_bit_weight;

/* Define the union u_vedu_intra_rdo_cost_offset_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_h264_rdo_cost_offset : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_rdo_cost_offset_3;

/* Define the union u_vedu_vctrl_intra_rdo_factor_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_norm_intra_cu4_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_norm_intra_cu8_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_norm_intra_cu16_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_norm_intra_cu32_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_strmov_intra_cu4_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_strmov_intra_cu8_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_strmov_intra_cu16_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_strmov_intra_cu32_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_intra_rdo_factor_0;

/* Define the union u_vedu_vctrl_intra_rdo_factor_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_skin_intra_cu4_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_skin_intra_cu8_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_skin_intra_cu16_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_skin_intra_cu32_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_sobel_str_intra_cu4_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_sobel_str_intra_cu8_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_sobel_str_intra_cu16_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_sobel_str_intra_cu32_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_intra_rdo_factor_1;

/* Define the union u_vedu_vctrl_intra_rdo_factor_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_hedge_intra_cu4_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_hedge_intra_cu8_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_hedge_intra_cu16_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_hedge_intra_cu32_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_sobel_tex_intra_cu4_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_sobel_tex_intra_cu8_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_sobel_tex_intra_cu16_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_sobel_tex_intra_cu32_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_intra_rdo_factor_2;

/* Define the union u_vedu_intra_rdo_cost_offset_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu16_rdo_cost_offset : 16  ; /* [15..0] */
        unsigned int intra_cu32_rdo_cost_offset : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_rdo_cost_offset_0;

/* Define the union u_vedu_intra_rdo_cost_offset_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu4_rdo_cost_offset : 16  ; /* [15..0] */
        unsigned int intra_cu8_rdo_cost_offset : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_rdo_cost_offset_1;

/* Define the union u_vedu_intra_no_dc_cost_offset_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu16_non_dc_mode_offset : 16  ; /* [15..0] */
        unsigned int intra_cu32_non_dc_mode_offset : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_no_dc_cost_offset_0;

/* Define the union u_vedu_intra_no_dc_cost_offset_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intra_cu4_non_dc_mode_offset : 16  ; /* [15..0] */
        unsigned int intra_cu8_non_dc_mode_offset : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_intra_no_dc_cost_offset_1;

/* Define the union u_vedu_pme_skip_large_res */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_skip_sad_thr_offset : 8   ; /* [7..0] */
        unsigned int pme_skip_sad_thr_gain : 4   ; /* [11..8] */
        unsigned int pme_skip_large_res_det : 1   ; /* [12] */
        unsigned int reserved_0            : 19  ; /* [31..13] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_skip_large_res;

/* Define the union u_vedu_mrg_adj_weight */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cu8_fz_weight         : 4   ; /* [3..0] */
        unsigned int cu16_fz_weight        : 4   ; /* [7..4] */
        unsigned int cu32_fz_weight        : 4   ; /* [11..8] */
        unsigned int cu64_fz_weight        : 4   ; /* [15..12] */
        unsigned int cu8_fz_adapt_weight   : 4   ; /* [19..16] */
        unsigned int cu16_fz_adapt_weight  : 4   ; /* [23..20] */
        unsigned int cu32_fz_adapt_weight  : 4   ; /* [27..24] */
        unsigned int cu64_fz_adapt_weight  : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_adj_weight;

/* Define the union u_vedu_qpg_res_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_res_coef_en      : 1   ; /* [0] */
        unsigned int reserved_0            : 3   ; /* [3..1] */
        unsigned int vcpi_large_res_coef   : 5   ; /* [8..4] */
        unsigned int reserved_1            : 3   ; /* [11..9] */
        unsigned int vcpi_small_res_coef   : 5   ; /* [16..12] */
        unsigned int reserved_2            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_res_coef;

/* Define the union u_vedu_pme_bias_cost0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_pu8_bias_cost     : 16  ; /* [15..0] */
        unsigned int fme_pu16_bias_cost    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_bias_cost0;

/* Define the union u_vedu_pme_bias_cost1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fme_pu32_bias_cost    : 16  ; /* [15..0] */
        unsigned int fme_pu64_bias_cost    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_bias_cost1;

/* Define the union u_vedu_mrg_bias_cost0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_pu8_bias_cost     : 16  ; /* [15..0] */
        unsigned int mrg_pu16_bias_cost    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_bias_cost0;

/* Define the union u_vedu_mrg_bias_cost1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_pu32_bias_cost    : 16  ; /* [15..0] */
        unsigned int mrg_pu64_bias_cost    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_bias_cost1;

/* Define the union u_vedu_mrg_abs_offset0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_pu8_abs_offset    : 16  ; /* [15..0] */
        unsigned int mrg_pu16_abs_offset   : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_abs_offset0;

/* Define the union u_vedu_mrg_abs_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mrg_pu32_abs_offset   : 16  ; /* [15..0] */
        unsigned int mrg_pu64_abs_offset   : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_mrg_abs_offset1;

/* Define the union u_vedu_vctrl_mrg_rdo_factor_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_norm_mrg_cu8_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_norm_mrg_cu16_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_norm_mrg_cu32_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_norm_mrg_cu64_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_strmov_mrg_cu8_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_strmov_mrg_cu16_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_strmov_mrg_cu32_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_strmov_mrg_cu64_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_mrg_rdo_factor_0;

/* Define the union u_vedu_vctrl_mrg_rdo_factor_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_skin_mrg_cu8_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_skin_mrg_cu16_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_skin_mrg_cu32_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_skin_mrg_cu64_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_sobel_str_mrg_cu8_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_sobel_str_mrg_cu16_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_sobel_str_mrg_cu32_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_sobel_str_mrg_cu64_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_mrg_rdo_factor_1;

/* Define the union u_vedu_vctrl_mrg_rdo_factor_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_hedge_mrg_cu8_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_hedge_mrg_cu16_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_hedge_mrg_cu32_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_hedge_mrg_cu64_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_sobel_tex_mrg_cu8_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_sobel_tex_mrg_cu16_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_sobel_tex_mrg_cu32_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_sobel_tex_mrg_cu64_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_mrg_rdo_factor_2;

/* Define the union u_vedu_vctrl_fme_rdo_factor_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_norm_fme_cu8_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_norm_fme_cu16_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_norm_fme_cu32_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_norm_fme_cu64_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_strmov_fme_cu8_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_strmov_fme_cu16_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_strmov_fme_cu32_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_strmov_fme_cu64_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_fme_rdo_factor_0;

/* Define the union u_vedu_vctrl_fme_rdo_factor_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_skin_fme_cu8_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_skin_fme_cu16_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_skin_fme_cu32_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_skin_fme_cu64_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_sobel_str_fme_cu8_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_sobel_str_fme_cu16_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_sobel_str_fme_cu32_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_sobel_str_fme_cu64_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_fme_rdo_factor_1;

/* Define the union u_vedu_vctrl_fme_rdo_factor_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_hedge_fme_cu8_rdcost_offset : 4   ; /* [3..0] */
        unsigned int vctrl_hedge_fme_cu16_rdcost_offset : 4   ; /* [7..4] */
        unsigned int vctrl_hedge_fme_cu32_rdcost_offset : 4   ; /* [11..8] */
        unsigned int vctrl_hedge_fme_cu64_rdcost_offset : 4   ; /* [15..12] */
        unsigned int vctrl_sobel_tex_fme_cu8_rdcost_offset : 4   ; /* [19..16] */
        unsigned int vctrl_sobel_tex_fme_cu16_rdcost_offset : 4   ; /* [23..20] */
        unsigned int vctrl_sobel_tex_fme_cu32_rdcost_offset : 4   ; /* [27..24] */
        unsigned int vctrl_sobel_tex_fme_cu64_rdcost_offset : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_fme_rdo_factor_2;

/* Define the union u_vedu_ime_rdocfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ime_lambdaoff8        : 16  ; /* [15..0] */
        unsigned int ime_lambdaoff16       : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_ime_rdocfg;

/* Define the union u_vedu_vcpi_pintra_thresh0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_pintra_pu16_amp_th : 8   ; /* [7..0] */
        unsigned int vcpi_pintra_pu32_amp_th : 8   ; /* [15..8] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pintra_thresh0;

/* Define the union u_vedu_vcpi_pintra_thresh1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 8   ; /* [7..0] */
        unsigned int vcpi_pintra_pu32_std_th : 8   ; /* [15..8] */
        unsigned int reserved_1            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pintra_thresh1;

/* Define the union u_vedu_vcpi_pintra_thresh2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0            : 24  ; /* [23..0] */
        unsigned int vcpi_rpintra_pu4_strong_edge_th : 3   ; /* [26..24] */
        unsigned int reserved_1            : 1   ; /* [27] */
        unsigned int vcpi_rpintra_pu4_mode_distance_th : 2   ; /* [29..28] */
        unsigned int vcpi_rpintra_bypass   : 1   ; /* [30] */
        unsigned int reserved_2            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pintra_thresh2;

/* Define the union u_vedu_sel_offset_strength */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sel_offset_strength   : 3   ; /* [2..0] */
        unsigned int reserved_0            : 29  ; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sel_offset_strength;

/* Define the union u_vedu_sel_cu32_dc_ac_th_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sel_cu32_dc_ac_th_offset : 2   ; /* [1..0] */
        unsigned int reserved_0            : 30  ; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sel_cu32_dc_ac_th_offset;

/* Define the union u_vedu_sel_cu32_qp_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sel_cu32_qp0_th       : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int sel_cu32_qp1_th       : 6   ; /* [13..8] */
        unsigned int reserved_1            : 18  ; /* [31..14] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sel_cu32_qp_th;

/* Define the union u_vedu_sel_res_dc_ac_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sel_res16_luma_dc_th  : 4   ; /* [3..0] */
        unsigned int sel_res16_chroma_dc_th : 4   ; /* [7..4] */
        unsigned int sel_res16_luma_ac_th  : 4   ; /* [11..8] */
        unsigned int sel_res16_chroma_ac_th : 4   ; /* [15..12] */
        unsigned int sel_res32_luma_dc_th  : 4   ; /* [19..16] */
        unsigned int sel_res32_chroma_dc_th : 4   ; /* [23..20] */
        unsigned int sel_res32_luma_ac_th  : 4   ; /* [27..24] */
        unsigned int sel_res32_chroma_ac_th : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sel_res_dc_ac_th;

/* Define the union u_vedu_pme_skin_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_skin_u_max_thr    : 8   ; /* [7..0] */
        unsigned int pme_skin_u_min_thr    : 8   ; /* [15..8] */
        unsigned int pme_skin_v_max_thr    : 8   ; /* [23..16] */
        unsigned int pme_skin_v_min_thr    : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_skin_thr;

/* Define the union u_vedu_pme_strong_edge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_skin_num          : 9   ; /* [8..0] */
        unsigned int pme_strong_edge_thr   : 8   ; /* [16..9] */
        unsigned int pme_strong_edge_cnt   : 5   ; /* [21..17] */
        unsigned int pme_still_scene_thr   : 9   ; /* [30..22] */
        unsigned int reserved_0            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_strong_edge;

/* Define the union u_vedu_pme_skin_sad_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_skin_sad_thr_offset : 8   ; /* [7..0] */
        unsigned int reserved_0            : 4   ; /* [11..8] */
        unsigned int vcpi_skin_sad_thr_gain : 4   ; /* [15..12] */
        unsigned int reserved_1            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_skin_sad_thr;

/* Define the union u_vedu_qpg_skin */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_skin_min_qp       : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_skin_max_qp       : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int qpg_skin_qp_delta     : 4   ; /* [19..16] */
        unsigned int reserved_2            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_skin;

/* Define the union u_vedu_pme_inter_strong_edge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_interdiff_max_min_madi_abs : 8   ; /* [7..0] */
        unsigned int pme_interdiff_max_min_madi_times : 8   ; /* [15..8] */
        unsigned int pme_interstrongedge_madi_thr : 8   ; /* [23..16] */
        unsigned int reserved_0            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_inter_strong_edge;

/* Define the union u_vedu_qpg_hedge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_hedge_min_qp      : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_hedge_max_qp      : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int qpg_hedge_qp_delta    : 4   ; /* [19..16] */
        unsigned int reserved_2            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_hedge;

/* Define the union u_vedu_pme_large_move_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_move_scene_thr    : 8   ; /* [7..0] */
        unsigned int pme_move_sad_thr      : 14  ; /* [21..8] */
        unsigned int reserved_0            : 10  ; /* [31..22] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_large_move_thr;

/* Define the union u_vedu_qpg_hedge_move */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_hedge_move_min_qp : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_hedge_move_max_qp : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int qpg_hedge_move_qp_delta : 4   ; /* [19..16] */
        unsigned int reserved_2            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_hedge_move;

/* Define the union u_vedu_pme_move_scene_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_move_scene_mv_thr : 13  ; /* [12..0] */
        unsigned int reserved_0            : 3   ; /* [15..13] */
        unsigned int vcpi_move_scene_mv_en : 1   ; /* [16] */
        unsigned int reserved_1            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_move_scene_thr;

/* Define the union u_vedu_pme_low_luma_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_low_luma_thr      : 8   ; /* [7..0] */
        unsigned int pme_low_luma_madi_thr : 8   ; /* [15..8] */
        unsigned int pme_high_luma_thr     : 8   ; /* [23..16] */
        unsigned int reserved_0            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_low_luma_thr;

/* Define the union u_vedu_qpg_lowluma */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lowluma_min_qp    : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_lowluma_max_qp    : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int qpg_lowluma_qp_delta  : 4   ; /* [19..16] */
        unsigned int reserved_2            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_lowluma;

/* Define the union u_vedu_pme_chroma_flat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_flat_v_thr_high   : 8   ; /* [7..0] */
        unsigned int pme_flat_v_thr_low    : 8   ; /* [15..8] */
        unsigned int pme_flat_u_thr_high   : 8   ; /* [23..16] */
        unsigned int pme_flat_u_thr_low    : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_chroma_flat;

/* Define the union u_vedu_pme_luma_flat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_flat_pmemv_thr    : 8   ; /* [7..0] */
        unsigned int pme_flat_luma_madi_thr : 8   ; /* [15..8] */
        unsigned int pme_flat_low_luma_thr : 8   ; /* [23..16] */
        unsigned int pme_flat_high_luma_thr : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_luma_flat;

/* Define the union u_vedu_pme_madi_flat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_flat_pmesad_thr   : 14  ; /* [13..0] */
        unsigned int pme_flat_icount_thr   : 9   ; /* [22..14] */
        unsigned int pme_flat_region_cnt   : 5   ; /* [27..23] */
        unsigned int pme_flat_madi_times   : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_madi_flat;

/* Define the union u_vedu_qpg_flat_region */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_flat_region_qp_delta : 4   ; /* [3..0] */
        unsigned int qpg_flat_region_max_qp : 6   ; /* [9..4] */
        unsigned int reserved_0            : 2   ; /* [11..10] */
        unsigned int qpg_flat_region_min_qp : 6   ; /* [17..12] */
        unsigned int reserved_1            : 6   ; /* [23..18] */
        unsigned int vcpi_cu32_use_cu16_mean_en : 1   ; /* [24] */
        unsigned int reserved_2            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_flat_region;

/* Define the union u_vedu_chroma_protect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_prot_en   : 1   ; /* [0] */
        unsigned int reserved_0            : 3   ; /* [3..1] */
        unsigned int vcpi_chroma_max_qp    : 6   ; /* [9..4] */
        unsigned int reserved_1            : 2   ; /* [11..10] */
        unsigned int vcpi_chroma_min_qp     : 6   ; /* [17..12] */
        unsigned int reserved_2            : 2   ; /* [19..18] */
        unsigned int vcpi_chroma_qp_delta  : 4   ; /* [23..20] */
        unsigned int reserved_3            : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_protect;

/* Define the union u_vedu_pme_chroma_strong_edge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_strong_edge_thr_u : 8   ; /* [7..0] */
        unsigned int reserved_0            : 2   ; /* [9..8] */
        unsigned int vcpi_strong_edge_cnt_u : 5   ; /* [14..10] */
        unsigned int reserved_1            : 1   ; /* [15] */
        unsigned int vcpi_strong_edge_thr_v : 8   ; /* [23..16] */
        unsigned int reserved_2            : 2   ; /* [25..24] */
        unsigned int vcpi_strong_edge_cnt_v : 5   ; /* [30..26] */
        unsigned int reserved_3            : 1   ; /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_chroma_strong_edge;

/* Define the union u_vedu_chroma_sad_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_sad_thr_offset : 8   ; /* [7..0] */
        unsigned int vcpi_chroma_sad_thr_gain : 4   ; /* [11..8] */
        unsigned int reserved_0            : 20  ; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_sad_thr;

/* Define the union u_vedu_chroma_fg_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_u0_thr_min : 8   ; /* [7..0] */
        unsigned int vcpi_chroma_u0_thr_max : 8   ; /* [15..8] */
        unsigned int vcpi_chroma_v0_thr_min : 8   ; /* [23..16] */
        unsigned int vcpi_chroma_v0_thr_max : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_fg_thr;

/* Define the union u_vedu_chroma_bg_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_u1_thr_min : 8   ; /* [7..0] */
        unsigned int vcpi_chroma_u1_thr_max : 8   ; /* [15..8] */
        unsigned int vcpi_chroma_v1_thr_min : 8   ; /* [23..16] */
        unsigned int vcpi_chroma_v1_thr_max : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_bg_thr;

/* Define the union u_vedu_chroma_sum_fg_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_uv0_thr_min : 9   ; /* [8..0] */
        unsigned int reserved_0            : 7   ; /* [15..9] */
        unsigned int vcpi_chroma_uv0_thr_max : 9   ; /* [24..16] */
        unsigned int reserved_1            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_sum_fg_thr;

/* Define the union u_vedu_chroma_sum_bg_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_uv1_thr_min : 9   ; /* [8..0] */
        unsigned int reserved_0            : 7   ; /* [15..9] */
        unsigned int vcpi_chroma_uv1_thr_max : 9   ; /* [24..16] */
        unsigned int reserved_1            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_sum_bg_thr;

/* Define the union u_vedu_chroma_fg_count_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_count0_thr_min : 9   ; /* [8..0] */
        unsigned int reserved_0            : 7   ; /* [15..9] */
        unsigned int vcpi_chroma_count0_thr_max : 9   ; /* [24..16] */
        unsigned int reserved_1            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_fg_count_thr;

/* Define the union u_vedu_chroma_bg_count_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_chroma_count1_thr_min : 9   ; /* [8..0] */
        unsigned int reserved_0            : 7   ; /* [15..9] */
        unsigned int vcpi_chroma_count1_thr_max : 9   ; /* [24..16] */
        unsigned int reserved_1            : 7   ; /* [31..25] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_chroma_bg_count_thr;

/* Define the union u_vedu_pme_intrablk_det_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pme_intrablk_det_mv_dif_thr1 : 8   ; /* [7..0] */
        unsigned int pme_intrablk_det_mv_dif_thr0 : 8   ; /* [15..8] */
        unsigned int pme_intrablk_det_mvy_thr : 8   ; /* [23..16] */
        unsigned int pme_intrablk_det_mvx_thr : 8   ; /* [31..24] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_pme_intrablk_det_thr;

/* Define the union u_vedu_qpg_intra_det */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_intra_det_min_qp  : 6   ; /* [5..0] */
        unsigned int reserved_0            : 2   ; /* [7..6] */
        unsigned int qpg_intra_det_max_qp  : 6   ; /* [13..8] */
        unsigned int reserved_1            : 2   ; /* [15..14] */
        unsigned int qpg_intra_det_qp_delta : 4   ; /* [19..16] */
        unsigned int reserved_2            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_intra_det;

/* Define the union u_vedu_vcpi_noforcezero */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_bislayer0flag    : 1   ; /* [0] */
        unsigned int vcpi_bnoforcezero_flag : 1   ; /* [1] */
        unsigned int vcpi_bnoforcezero_posx : 4   ; /* [5..2] */
        unsigned int vcpi_bnoforcezero_posy : 4   ; /* [9..6] */
        unsigned int reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_noforcezero;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda00          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg00;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda01          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg01;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda02          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg02;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda03          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg03;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg04 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda04          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg04;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg05 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda05          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg05;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg06 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda06          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg06;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg07 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda07          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg07;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg08 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda08          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg08;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg09 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda09          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg09;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda10          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg10;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda11          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg11;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda12          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg12;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda13          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg13;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda14          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg14;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda15          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg15;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda16          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg16;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda17          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg17;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda18          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg18;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda19          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg19;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda20          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg20;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda21          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg21;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda22          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg22;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda23          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg23;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg24 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda24          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg24;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg25 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda25          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg25;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg26 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda26          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg26;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg27 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda27          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg27;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg28 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda28          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg28;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg29 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda29          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg29;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg30 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda30          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg30;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg31 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda31          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg31;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg32 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda32          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg32;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg33 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda33          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg33;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg34 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda34          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg34;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg35 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda35          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg35;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg36 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda36          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg36;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg37 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda37          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg37;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg38 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda38          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg38;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg39 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda39          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg39;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg40 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda40          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg40;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg41 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda41          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg41;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg42 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda42          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg42;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg43 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda43          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg43;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg44 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda44          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg44;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg45 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda45          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg45;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg46 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda46          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg46;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg47 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda47          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg47;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg48 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda48          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg48;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg49 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda49          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg49;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg50 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda50          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg50;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg51 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda51          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg51;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg52 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda52          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg52;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg53 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda53          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg53;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg54 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda54          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg54;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg55 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda55          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg55;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg56 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda56          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg56;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg57 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda57          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg57;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg58 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda58          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg58;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg59 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda59          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg59;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg60 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda60          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg60;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg61 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda61          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg61;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg62 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda62          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg62;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg63 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda63          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg63;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg64 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda64          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg64;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg65 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda65          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg65;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg66 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda66          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg66;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg67 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda67          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg67;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg68 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda68          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg68;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg69 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda69          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg69;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg70 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda70          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg70;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg71 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda71          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg71;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg72 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda72          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg72;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg73 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda73          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg73;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg74 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda74          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg74;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg75 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda75          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg75;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg76 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda76          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg76;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg77 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda77          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg77;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg78 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda78          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg78;

/* Define the union u_vedu_qpg_qp_lambda_ctrl_reg79 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qpg_lambda79          : 20  ; /* [19..0] */
        unsigned int reserved_0            : 12  ; /* [31..20] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_qpg_qp_lambda_ctrl_reg79;

/* Define the union u_vedu_sao_ssd_area0_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area0_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area0_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area0_start;

/* Define the union u_vedu_sao_ssd_area0_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area0_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area0_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area0_end;

/* Define the union u_vedu_sao_ssd_area1_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area1_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area1_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area1_start;

/* Define the union u_vedu_sao_ssd_area1_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area1_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area1_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area1_end;

/* Define the union u_vedu_sao_ssd_area2_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area2_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area2_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area2_start;

/* Define the union u_vedu_sao_ssd_area2_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area2_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area2_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area2_end;

/* Define the union u_vedu_sao_ssd_area3_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area3_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area3_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area3_start;

/* Define the union u_vedu_sao_ssd_area3_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area3_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area3_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area3_end;

/* Define the union u_vedu_sao_ssd_area4_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area4_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area4_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area4_start;

/* Define the union u_vedu_sao_ssd_area4_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area4_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area4_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area4_end;

/* Define the union u_vedu_sao_ssd_area5_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area5_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area5_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area5_start;

/* Define the union u_vedu_sao_ssd_area5_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area5_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area5_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area5_end;

/* Define the union u_vedu_sao_ssd_area6_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area6_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area6_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area6_start;

/* Define the union u_vedu_sao_ssd_area6_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area6_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area6_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area6_end;

/* Define the union u_vedu_sao_ssd_area7_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area7_start_lcux  : 16  ; /* [15..0] */
        unsigned int sao_area7_start_lcuy  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area7_start;

/* Define the union u_vedu_sao_ssd_area7_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_area7_end_lcux    : 16  ; /* [15..0] */
        unsigned int sao_area7_end_lcuy    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_ssd_area7_end;

/* Define the union u_vedu_sao_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sao_bo_mode_off_en    : 1   ; /* [0] */
        unsigned int sao_eo_mode_off_en    : 1   ; /* [1] */
        unsigned int sao_merge_mode_off    : 2   ; /* [3..2] */
        unsigned int sao_merge_mode_qp     : 6   ; /* [9..4] */
        unsigned int reserved_0            : 22  ; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_sao_mode;

/* Define the union u_vedu_vcpi_pic_strong_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_skin_en          : 1   ; /* [0] */
        unsigned int vcpi_strong_edge_en   : 1   ; /* [1] */
        unsigned int vcpi_still_en         : 1   ; /* [2] */
        unsigned int vcpi_skin_close_angle : 1   ; /* [3] */
        unsigned int vcpi_rounding_sobel_en : 1   ; /* [4] */
        unsigned int reserved_0            : 27  ; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_pic_strong_en;

/* Define the union u_vedu_vctrl_nm_acoffset_denoise */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_i_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_low_freq_ac_blk16  : 4   ; /* [15..12] */
        unsigned int vctrl_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_low_freq_ac_blk32  : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_nm_acoffset_denoise;

/* Define the union u_vedu_vctrl_nm_engthr_denoise */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_chos_oth_blk_thr3  : 6   ; /* [13..8] */
        unsigned int vctrl_chos_oth_blk_thr2  : 6   ; /* [19..14] */
        unsigned int vctrl_chos_oth_blk_thr   : 5   ; /* [24..20] */
        unsigned int vctrl_rm_all_high_ac_thr  : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_nm_engthr_denoise;

/* Define the union U_VEDU_VCTRL_NM_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_ring_eng_thr      : 4   ; /* [3..0] */
        unsigned int vctrl_ring_ac_thr       : 4   ; /* [7..4] */
        unsigned int vctrl_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_i_low_freq_ac_blk8   : 3   ; /* [22..20] */
        unsigned int vctrl_blk8_enable_flag  : 1   ; /* [23] */
        unsigned int vctrl_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_nm_tu8_denoise;

/* Define the union U_VEDU_VCTRL_SK_ACOFFSET_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_skin_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_skin_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_skin_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_skin_low_freq_ac_blk16 : 4   ; /* [15..12] */
        unsigned int vctrl_skin_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_skin_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_skin_low_freq_ac_blk32 : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_sk_acoffset_denoise;

/* Define the union U_VEDU_VCTRL_SK_ENGTHR_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_skin_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_skin_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_skin_chos_oth_blk_thr3 : 6   ; /* [13..8] */
        unsigned int vctrl_skin_chos_oth_blk_thr2 : 6   ; /* [19..14] */
        unsigned int vctrl_skin_chos_oth_blk_thr : 5   ; /* [24..20] */
        unsigned int vctrl_skin_rm_all_high_ac_thr : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_sk_engthr_denoise;

/* Define the union U_VEDU_VCTRL_SK_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_skin_ring_eng_thr  : 4   ; /* [3..0] */
        unsigned int vctrl_skin_ring_ac_thr   : 4   ; /* [7..4] */
        unsigned int vctrl_skin_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_skin_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_skin_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_skin_low_freq_ac_blk8 : 3   ; /* [22..20] */
        unsigned int vctrl_skin_blk8_enable_flag : 1   ; /* [23] */
        unsigned int vctrl_skin_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_skin_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_sk_tu8_denoise;

/* Define the union U_VEDU_VCTRL_ST_ACOFFSET_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_still_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_still_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_still_i_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_still_low_freq_ac_blk16 : 4   ; /* [15..12] */
        unsigned int vctrl_still_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_still_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_still_low_freq_ac_blk32 : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_st_acoffset_denoise;

/* Define the union U_VEDU_VCTRL_ST_ENGTHR_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_still_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_still_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_still_chos_oth_blk_thr3 : 6   ; /* [13..8] */
        unsigned int vctrl_still_chos_oth_blk_thr2 : 6   ; /* [19..14] */
        unsigned int vctrl_still_chos_oth_blk_thr : 5   ; /* [24..20] */
        unsigned int vctrl_still_rm_all_high_ac_thr : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_st_engthr_denoise;

/* Define the union U_VEDU_VCTRL_ST_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_still_ring_eng_thr : 4   ; /* [3..0] */
        unsigned int vctrl_still_ring_ac_thr  : 4   ; /* [7..4] */
        unsigned int vctrl_still_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_still_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_still_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_still_low_freq_ac_blk8 : 3   ; /* [22..20] */
        unsigned int vctrl_still_blk8_enable_flag : 1   ; /* [23] */
        unsigned int vctrl_still_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_still_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_st_tu8_denoise;

/* Define the union U_VEDU_VCTRL_SE_ACOFFSET_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_edge_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_edge_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_edge_i_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_edge_low_freq_ac_blk16 : 4   ; /* [15..12] */
        unsigned int vctrl_edge_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_edge_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_edge_low_freq_ac_blk32 : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_se_acoffset_denoise;

/* Define the union U_VEDU_VCTRL_SE_ENGTHR_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_edge_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_edge_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_edge_chos_oth_blk_thr3 : 6   ; /* [13..8] */
        unsigned int vctrl_edge_chos_oth_blk_thr2 : 6   ; /* [19..14] */
        unsigned int vctrl_edge_chos_oth_blk_thr : 5   ; /* [24..20] */
        unsigned int vctrl_edge_rm_all_high_ac_thr : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_se_engthr_denoise;

/* Define the union U_VEDU_VCTRL_SE_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_edge_ring_eng_thr  : 4   ; /* [3..0] */
        unsigned int vctrl_edge_ring_ac_thr   : 4   ; /* [7..4] */
        unsigned int vctrl_edge_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_edge_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_edge_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_edge_low_freq_ac_blk8 : 3   ; /* [22..20] */
        unsigned int vctrl_edge_blk8_enable_flag : 1   ; /* [23] */
        unsigned int vctrl_edge_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_edge_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_se_tu8_denoise;

/* Define the union U_VEDU_VCTRL_SELM_ACOFFSET_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_edge_and_move_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_edge_and_move_i_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_edge_and_move_low_freq_ac_blk16 : 4   ; /* [15..12] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_edge_and_move_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_edge_and_move_low_freq_ac_blk32 : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_selm_acoffset_denoise;

/* Define the union U_VEDU_VCTRL_SELM_ENGTHR_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_edge_and_move_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_thr3 : 6   ; /* [13..8] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_thr2 : 6   ; /* [19..14] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_thr : 5   ; /* [24..20] */
        unsigned int vctrl_edge_and_move_rm_all_high_ac_thr : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_selm_engthr_denoise;

/* Define the union U_VEDU_VCTRL_SELM_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_edge_and_move_ring_eng_thr : 4   ; /* [3..0] */
        unsigned int vctrl_edge_and_move_ring_ac_thr : 4   ; /* [7..4] */
        unsigned int vctrl_edge_and_move_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_edge_and_move_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_edge_and_move_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_edge_and_move_low_freq_ac_blk8 : 3   ; /* [22..20] */
        unsigned int vctrl_edge_and_move_blk8_enable_flag : 1   ; /* [23] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_edge_and_move_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_selm_tu8_denoise;

/* Define the union U_VEDU_VCTRL_WS_ACOFFSET_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_weak_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_weak_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_weak_i_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_weak_low_freq_ac_blk16 : 4   ; /* [15..12] */
        unsigned int vctrl_weak_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_weak_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_weak_low_freq_ac_blk32 : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_ws_acoffset_denoise;

/* Define the union U_VEDU_VCTRL_WS_ENGTHR_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_weak_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_weak_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_weak_chos_oth_blk_thr3 : 6   ; /* [13..8] */
        unsigned int vctrl_weak_chos_oth_blk_thr2 : 6   ; /* [19..14] */
        unsigned int vctrl_weak_chos_oth_blk_thr : 5   ; /* [24..20] */
        unsigned int vctrl_weak_rm_all_high_ac_thr : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_ws_engthr_denoise;

/* Define the union U_VEDU_VCTRL_WS_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_weak_ring_eng_thr  : 4   ; /* [3..0] */
        unsigned int vctrl_weak_ring_ac_thr   : 4   ; /* [7..4] */
        unsigned int vctrl_weak_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_weak_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_weak_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_weak_low_freq_ac_blk8 : 3   ; /* [22..20] */
        unsigned int vctrl_weak_blk8_enable_flag : 1   ; /* [23] */
        unsigned int vctrl_weak_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_weak_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_ws_tu8_denoise;

/* Define the union U_VEDU_VCTRL_SSSE_ACOFFSET_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_strong_chos_oth_blk_spec_offset : 4   ; /* [3..0] */
        unsigned int vctrl_strong_chos_oth_blk_offset16 : 4   ; /* [7..4] */
        unsigned int vctrl_strong_i_chos_cur_blk_offset16 : 4   ; /* [11..8] */
        unsigned int vctrl_strong_low_freq_ac_blk16 : 4   ; /* [15..12] */
        unsigned int vctrl_strong_chos_oth_blk_offset32 : 4   ; /* [19..16] */
        unsigned int vctrl_strong_i_chos_cur_blk_offset32 : 4   ; /* [23..20] */
        unsigned int vctrl_strong_low_freq_ac_blk32 : 4   ; /* [27..24] */
        unsigned int reserved_0            : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_ssse_acoffset_denoise;

/* Define the union U_VEDU_VCTRL_SSSE_ENGTHR_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_strong_chos_oth_blk_spec_thr : 4   ; /* [3..0] */
        unsigned int vctrl_strong_chos_oth_blk_eng_thr : 4   ; /* [7..4] */
        unsigned int vctrl_strong_chos_oth_blk_thr3 : 6   ; /* [13..8] */
        unsigned int vctrl_strong_chos_oth_blk_thr2 : 6   ; /* [19..14] */
        unsigned int vctrl_strong_chos_oth_blk_thr : 5   ; /* [24..20] */
        unsigned int vctrl_strong_rm_all_high_ac_thr : 4   ; /* [28..25] */
        unsigned int reserved_0            : 3   ; /* [31..29] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_ssse_engthr_denoise;

/* Define the union U_VEDU_VCTRL_SSSE_TU8_DENOISE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_strong_ring_eng_thr : 4   ; /* [3..0] */
        unsigned int vctrl_strong_ring_ac_thr : 4   ; /* [7..4] */
        unsigned int vctrl_strong_p_chos_oth_blk_offset : 4   ; /* [11..8] */
        unsigned int vctrl_strong_p_chos_oth_blk_offset8 : 4   ; /* [15..12] */
        unsigned int vctrl_strong_i_chos_cur_blk_offset8 : 4   ; /* [19..16] */
        unsigned int vctrl_strong_low_freq_ac_blk8 : 3   ; /* [22..20] */
        unsigned int vctrl_strong_blk8_enable_flag : 1   ; /* [23] */
        unsigned int vctrl_strong_chos_oth_blk_offset_sec16 : 4   ; /* [27..24] */
        unsigned int vctrl_strong_chos_oth_blk_offset_sec32 : 4   ; /* [31..28] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_ssse_tu8_denoise;

/* Define the union U_VEDU_VCTRL_PR_INTER_OFFSET0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_normal_inter_thr_offset : 6   ; /* [5..0] */
        unsigned int vctrl_skin_inter_thr_offset : 6   ; /* [11..6] */
        unsigned int vctrl_still_inter_thr_offset : 6   ; /* [17..12] */
        unsigned int vctrl_s_edge_inter_thr_offset : 6   ; /* [23..18] */
        unsigned int vctrl_e_move_inter_thr_offset : 6   ; /* [29..24] */
        unsigned int reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_pr_inter_offset0;

/* Define the union U_VEDU_VCTRL_PR_INTER_OFFSET1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vctrl_week_s_inter_thr_offset : 6   ; /* [5..0] */
        unsigned int vctrl_sobel_sse_inter_thr_offset : 6   ; /* [11..6] */
        unsigned int reserved_0            : 20  ; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vctrl_pr_inter_offset1;

/* Define the union U_VEDU_VCPI_STRIDE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_curld_c_stride   : 16  ; /* [15..0] */
        unsigned int vcpi_curld_y_stride   : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_stride;

/* Define the union U_VEDU_VCPI_REC_STRIDE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_recst_ystride    : 16  ; /* [15..0] */
        unsigned int vcpi_recst_cstride    : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_rec_stride;

/* Define the union U_VEDU_VCPI_REF_L0_STRIDE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_refc_l0_stride   : 16  ; /* [15..0] */
        unsigned int vcpi_refy_l0_stride   : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_ref_l0_stride;

/* Define the union U_VEDU_VCPI_REF_L1_STRIDE */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_refc_l1_stride   : 16  ; /* [15..0] */
        unsigned int vcpi_refy_l1_stride   : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_ref_l1_stride;

/* Define the union U_VEDU_VCPI_REF_L0_LENGTH */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_ref0_luma_length : 16  ; /* [15..0] */
        unsigned int vcpi_ref0_chroma_length : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_ref_l0_length;

/* Define the union U_VEDU_VCPI_REF_L1_LENGTH */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_ref1_luma_length : 16  ; /* [15..0] */
        unsigned int vcpi_ref1_chroma_length : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_ref_l1_length;

/* Define the union U_VEDU_VCPI_REC_LENGTH */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_rec_luma_length  : 16  ; /* [15..0] */
        unsigned int vcpi_rec_chroma_length : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vcpi_rec_length;

/* Define the union U_VEDU_VLCST_PARAMETER */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlcst_para_set_len    : 8   ; /* [7..0] */
        unsigned int vlcst_para_sprat_en   : 1   ; /* [8] */
        unsigned int reserved_0            : 3   ; /* [11..9] */
        unsigned int vlcst_store_type      : 1   ; /* [12] */
        unsigned int reserved_1            : 3   ; /* [15..13] */
        unsigned int vlcst_para_set_en     : 1   ; /* [16] */
        unsigned int reserved_2            : 15  ; /* [31..17] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_vlcst_parameter;

/* Define the union U_VEDU_EMAR_WCH_PRESS_EN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch00_en  : 1   ; /* [0] */
        unsigned int vcpi_awpress_ch01_en  : 1   ; /* [1] */
        unsigned int vcpi_awpress_ch02_en  : 1   ; /* [2] */
        unsigned int vcpi_awpress_ch03_en  : 1   ; /* [3] */
        unsigned int vcpi_awpress_ch04_en  : 1   ; /* [4] */
        unsigned int vcpi_awpress_ch05_en  : 1   ; /* [5] */
        unsigned int vcpi_awpress_ch06_en  : 1   ; /* [6] */
        unsigned int vcpi_awpress_ch07_en  : 1   ; /* [7] */
        unsigned int vcpi_awpress_ch08_en  : 1   ; /* [8] */
        unsigned int vcpi_awpress_ch09_en  : 1   ; /* [9] */
        unsigned int vcpi_awpress_ch10_en  : 1   ; /* [10] */
        unsigned int vcpi_awpress_ch11_en  : 1   ; /* [11] */
        unsigned int vcpi_awpress_ch12_en  : 1   ; /* [12] */
        unsigned int vcpi_awpress_ch13_en  : 1   ; /* [13] */
        unsigned int vcpi_awpress_ch14_en  : 1   ; /* [14] */
        unsigned int vcpi_wpress_ch00_en   : 1   ; /* [15] */
        unsigned int vcpi_wpress_ch01_en   : 1   ; /* [16] */
        unsigned int vcpi_wpress_ch02_en   : 1   ; /* [17] */
        unsigned int vcpi_wpress_ch03_en   : 1   ; /* [18] */
        unsigned int vcpi_wpress_ch04_en   : 1   ; /* [19] */
        unsigned int vcpi_wpress_ch05_en   : 1   ; /* [20] */
        unsigned int vcpi_wpress_ch06_en   : 1   ; /* [21] */
        unsigned int vcpi_wpress_ch07_en   : 1   ; /* [22] */
        unsigned int vcpi_wpress_ch08_en   : 1   ; /* [23] */
        unsigned int vcpi_wpress_ch09_en   : 1   ; /* [24] */
        unsigned int vcpi_wpress_ch10_en   : 1   ; /* [25] */
        unsigned int vcpi_wpress_ch11_en   : 1   ; /* [26] */
        unsigned int vcpi_wpress_ch12_en   : 1   ; /* [27] */
        unsigned int vcpi_wpress_ch13_en   : 1   ; /* [28] */
        unsigned int vcpi_wpress_ch14_en   : 1   ; /* [29] */
        unsigned int reserved_0            : 2   ; /* [31..30] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wch_press_en;

/* Define the union U_VEDU_EMAR_RCH_PRESS_EN */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch00_en  : 1   ; /* [0] */
        unsigned int vcpi_arpress_ch01_en  : 1   ; /* [1] */
        unsigned int vcpi_arpress_ch02_en  : 1   ; /* [2] */
        unsigned int vcpi_arpress_ch03_en  : 1   ; /* [3] */
        unsigned int vcpi_arpress_ch04_en  : 1   ; /* [4] */
        unsigned int vcpi_arpress_ch05_en  : 1   ; /* [5] */
        unsigned int vcpi_arpress_ch06_en  : 1   ; /* [6] */
        unsigned int vcpi_arpress_ch07_en  : 1   ; /* [7] */
        unsigned int vcpi_arpress_ch08_en  : 1   ; /* [8] */
        unsigned int vcpi_arpress_ch09_en  : 1   ; /* [9] */
        unsigned int vcpi_arpress_ch10_en  : 1   ; /* [10] */
        unsigned int vcpi_arpress_ch11_en  : 1   ; /* [11] */
        unsigned int vcpi_arpress_ch12_en  : 1   ; /* [12] */
        unsigned int vcpi_arpress_ch13_en  : 1   ; /* [13] */
        unsigned int vcpi_arpress_ch14_en  : 1   ; /* [14] */
        unsigned int vcpi_arpress_ch15_en  : 1   ; /* [15] */
        unsigned int vcpi_arpress_ch16_en  : 1   ; /* [16] */
        unsigned int vcpi_arpress_ch17_en  : 1   ; /* [17] */
        unsigned int reserved_0            : 14  ; /* [31..18] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_rch_press_en;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch00_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch01_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;

} u_vedu_emar_awpress_num_00;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch02_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch03_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_01;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch04_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch05_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_02;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch06_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch07_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_03;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_04 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch08_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch09_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_04;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_05 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch10_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch11_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_05;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_06 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch12_num : 16  ; /* [15..0] */
        unsigned int vcpi_awpress_ch13_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_06;

/* Define the union U_VEDU_EMAR_AWPRESS_NUM_07 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_awpress_ch14_num : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_awpress_num_07;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch00_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch01_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_00;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch02_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch03_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_01;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch04_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch05_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_02;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch06_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch07_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_03;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_04 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch08_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch09_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_04;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_05 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch10_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch11_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_05;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_06 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch12_num  : 16  ; /* [15..0] */
        unsigned int vcpi_wpress_ch13_num  : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_06;

/* Define the union U_VEDU_EMAR_WPRESS_NUM_07 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_wpress_ch14_num  : 16  ; /* [15..0] */
        unsigned int reserved_0            : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_wpress_num_07;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch00_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch01_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_00;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch02_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch03_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_01;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch04_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch05_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_02;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch06_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch07_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_03;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_04 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch08_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch09_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_04;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_05 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch10_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch11_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_05;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_06 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch12_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch13_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_06;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_07 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch14_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch15_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_07;

/* Define the union U_VEDU_EMAR_ARPRESS_NUM_08 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vcpi_arpress_ch16_num : 16  ; /* [15..0] */
        unsigned int vcpi_arpress_ch17_num : 16  ; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int data;
} u_vedu_emar_arpress_num_08;

typedef struct {
    volatile u_vedu_vcpi_intmask vedu_vcpi_intmask;
    volatile u_vedu_vcpi_intclr vedu_vcpi_intclr;
    volatile u_vedu_vcpi_start vedu_vcpi_start;
    volatile unsigned int vedu_vcpi_frameno;
    volatile u_vedu_vcpi_mode vedu_vcpi_mode;
    volatile u_vedu_vcpi_picsize_pix vedu_vcpi_picsize_pix;
    volatile u_vedu_vcpi_qpcfg vedu_vcpi_qpcfg;
    volatile u_vedu_vcpi_outstd vedu_vcpi_outstd;
    volatile u_vedu_vcpi_basic_cfg0 vedu_vcpi_basic_cfg0;
    volatile u_vedu_vcpi_basic_cfg1 vedu_vcpi_basic_cfg1;
    volatile u_vedu_vcpi_decoder_d0 vedu_vcpi_decoder_d0;
    volatile u_vedu_vcpi_decoder_d1 vedu_vcpi_decoder_d1;
    volatile u_vedu_vcpi_decoder_d2 vedu_vcpi_decoder_d2;
    volatile u_vedu_vcpi_decoder_d3 vedu_vcpi_decoder_d3;
    volatile u_vedu_cmp_line_level  vedu_cmp_line_level;
    volatile unsigned int vedu_vcpi_linenum_addr;
    volatile unsigned int vedu_vcpi_dec_strmaddr;
    volatile unsigned int vedu_vcpi_avm_addr;
    volatile unsigned int vedu_vcpi_vam_addr;
    volatile u_vedu_bus_idle_req vedu_bus_idle_req;
    volatile unsigned int vedu_vcpi_timeout;
    volatile u_vedu_vcpi_multislc vedu_vcpi_multislc;
    volatile unsigned int vedu_vcpi_llild_addr_l;
    volatile unsigned int vedu_vcpi_llild_addr_h;
    volatile u_vedu_vcpi_rc_enable vedu_vcpi_rc_enable;
    volatile u_vedu_vlcst_ptbits_en vedu_vlcst_ptbits_en;
    volatile unsigned int vedu_vlcst_ptbits;
    volatile u_vedu_qpg_max_min_qp vedu_qpg_max_min_qp;
    volatile u_vedu_qpg_rc_max_min_qp vedu_qpg_rc_max_min_qp;
    volatile u_vedu_qpg_smart_reg vedu_qpg_smart_reg;
    volatile u_vedu_qpg_row_target_bits vedu_qpg_row_target_bits;
    volatile u_vedu_qpg_average_lcu_bits vedu_qpg_average_lcu_bits;
    volatile u_vedu_qpg_cu_qp_delta_thresh_reg0 vedu_qpg_cu_qp_delta_thresh_reg0;
    volatile u_vedu_qpg_cu_qp_delta_thresh_reg1 vedu_qpg_cu_qp_delta_thresh_reg1;
    volatile u_vedu_qpg_cu_qp_delta_thresh_reg2 vedu_qpg_cu_qp_delta_thresh_reg2;
    volatile u_vedu_qpg_cu_qp_delta_thresh_reg3 vedu_qpg_cu_qp_delta_thresh_reg3;
    volatile u_vedu_qpg_delta_level vedu_qpg_delta_level;
    volatile u_vedu_qpg_madi_switch_thr vedu_qpg_madi_switch_thr;
    volatile u_vedu_qpg_curr_sad_en vedu_qpg_curr_sad_en;
    volatile u_vedu_qpg_curr_sad_level vedu_qpg_curr_sad_level;
    volatile u_vedu_qpg_curr_sad_thresh0 vedu_qpg_curr_sad_thresh0;
    volatile u_vedu_qpg_curr_sad_thresh1 vedu_qpg_curr_sad_thresh1;
    volatile u_vedu_qpg_curr_sad_thresh2 vedu_qpg_curr_sad_thresh2;
    volatile u_vedu_qpg_curr_sad_thresh3 vedu_qpg_curr_sad_thresh3;
    volatile u_vedu_luma_rc vedu_luma_rc;
    volatile u_vedu_luma_level vedu_luma_level;
    volatile u_vedu_luma_thresh0 vedu_luma_thresh0;
    volatile u_vedu_luma_thresh1 vedu_luma_thresh1;
    volatile u_vedu_luma_thresh2 vedu_luma_thresh2;
    volatile u_vedu_luma_thresh3 vedu_luma_thresh3;
    volatile u_vedu_vctrl_lcu_baseline vedu_vctrl_lcu_baseline;
    volatile u_vedu_mem_ctrl vedu_mem_ctrl;
    volatile unsigned int vedu_reserved_03[4]; /* 4: length of register array */
    volatile u_vedu_safe_mode_flag vedu_safe_mode_flag;
    volatile u_vedu_vcpi_ddr_cross vedu_vcpi_ddr_cross;
    volatile u_vedu_debug_md5 vedu_debug_md5;
    volatile u_vedu_chn_bypass vedu_chn_bypass;
    volatile unsigned int vedu_reserved_04[68]; /* 68: length of register array */
    volatile u_vedu_vcpi_cross_tile_slc vedu_vcpi_cross_tile_slc;
    volatile u_vedu_vcpi_ref_flag vedu_vcpi_ref_flag;
    volatile u_vedu_pmv_tmv_en vedu_pmv_tmv_en;
    volatile u_vedu_vcpi_tmv_load vedu_vcpi_tmv_load;
    volatile unsigned int vedu_pmv_poc_0;
    volatile unsigned int vedu_pmv_poc_1;
    volatile unsigned int vedu_pmv_poc_2;
    volatile unsigned int vedu_pmv_poc_3;
    volatile unsigned int vedu_pmv_poc_4;
    volatile unsigned int vedu_pmv_poc_5;
    volatile u_vedu_cabac_glb_cfg vedu_cabac_glb_cfg;
    volatile u_vedu_ice_cmc_mode_cfg0 vedu_ice_cmc_mode_cfg0;
    volatile u_vedu_ice_cmc_mode_cfg1 vedu_ice_cmc_mode_cfg1;
    volatile u_vedu_vcpi_dblkcfg vedu_vcpi_dblkcfg;
    volatile u_vedu_intra_cfg vedu_intra_cfg;
    volatile u_vedu_curld_gcfg vedu_curld_gcfg;
    volatile u_vedu_vcpi_strfmt vedu_vcpi_strfmt;
    volatile u_vedu_vctrl_lcu_target_bit vedu_vctrl_lcu_target_bit;
    volatile u_vedu_vcpi_refld_dw vedu_vcpi_refld_dw;
    volatile u_vedu_vcpi_i_slc_insert vedu_vcpi_i_slc_insert;
    volatile u_vedu_pme_safe_cfg vedu_pme_safe_cfg;
    volatile u_vedu_pme_iblk_refresh vedu_pme_iblk_refresh;
    volatile u_vedu_pme_iblk_refresh_para vedu_pme_iblk_refresh_para;
    volatile u_vedu_intra_chnl4_ang_oen vedu_intra_chnl4_ang_0_en;
    volatile u_vedu_intra_chnl4_ang_1en vedu_intra_chnl4_ang_1_en;
    volatile u_vedu_intra_chnl8_ang_0en vedu_intra_chnl8_ang_0_en;
    volatile u_vedu_intra_chnl8_ang_1en vedu_intra_chnl8_ang_1_en;
    volatile u_vedu_intra_chnl16_ang_0en vedu_intra_chnl16_ang_0_en;
    volatile u_vedu_intra_chnl16_ang_1en vedu_intra_chnl16_ang_1_en;
    volatile u_vedu_intra_chnl32_ang_0en vedu_intra_chnl32_ang_0_en;
    volatile u_vedu_intra_chnl32_ang_1en vedu_intra_chnl32_ang_1_en;
    volatile u_vedu_pack_cu_parameter vedu_pack_cu_parameter;
    volatile unsigned int vedu_qpg_readline_interval;
    volatile unsigned int vedu_pmv_readline_interval;
    volatile u_vedu_rgb2yuv_coef_p0 vedu_rgb2_yuv_coef_p0;
    volatile u_vedu_rgb2yuv_coef_p1 vedu_rgb2_yuv_coef_p1;
    volatile u_vedu_rgb2yuv_coef_p2 vedu_rgb2_yuv_coef_p2;
    volatile u_vedu_rgb2yuv_offset  vedu_rgb2_yuv_offset;
    volatile u_vedu_rgb2yuv_clip_thr_y vedu_rgb2_yuv_clip_thr_y;
    volatile u_vedu_rgb2yuv_clip_thr_c vedu_rgb2_yuv_clip_thr_c;
    volatile u_vedu_rgb2yuv_shift_width vedu_rgb2_yuv_shift_width;
    volatile unsigned int vedu_reserved_05[23]; /* 23: length of register array */
    volatile u_vedu_vcpi_osd_enable vedu_vcpi_osd_enable;
    volatile u_vedu_vcpi_osd_pos_0 vedu_vcpi_osd_pos_0;
    volatile u_vedu_vcpi_osd_pos_1 vedu_vcpi_osd_pos_1;
    volatile u_vedu_vcpi_osd_pos_2 vedu_vcpi_osd_pos_2;
    volatile u_vedu_vcpi_osd_pos_3 vedu_vcpi_osd_pos_3;
    volatile u_vedu_vcpi_osd_pos_4 vedu_vcpi_osd_pos_4;
    volatile u_vedu_vcpi_osd_pos_5 vedu_vcpi_osd_pos_5;
    volatile u_vedu_vcpi_osd_pos_6 vedu_vcpi_osd_pos_6;
    volatile u_vedu_vcpi_osd_pos_7 vedu_vcpi_osd_pos_7;
    volatile u_vedu_vcpi_osd_size_0 vedu_vcpi_osd_size_0;
    volatile u_vedu_vcpi_osd_size_1 vedu_vcpi_osd_size_1;
    volatile u_vedu_vcpi_osd_size_2 vedu_vcpi_osd_size_2;
    volatile u_vedu_vcpi_osd_size_3 vedu_vcpi_osd_size_3;
    volatile u_vedu_vcpi_osd_size_4 vedu_vcpi_osd_size_4;
    volatile u_vedu_vcpi_osd_size_5 vedu_vcpi_osd_size_5;
    volatile u_vedu_vcpi_osd_size_6 vedu_vcpi_osd_size_6;
    volatile u_vedu_vcpi_osd_size_7 vedu_vcpi_osd_size_7;
    volatile u_vedu_vcpi_osd_layerid vedu_vcpi_osd_layerid;
    volatile u_vedu_vcpi_osd_qp0 vedu_vcpi_osd_qp0;
    volatile u_vedu_vcpi_osd_qp1 vedu_vcpi_osd_qp1;
    volatile u_vedu_curld_osd01_alpha vedu_curld_osd01_alpha;
    volatile u_vedu_curld_osd23_alpha vedu_curld_osd23_alpha;
    volatile u_vedu_curld_osd45_alpha vedu_curld_osd45_alpha;
    volatile u_vedu_curld_osd67_alpha vedu_curld_osd67_alpha;
    volatile u_vedu_curld_osd_galpha0 vedu_curld_osd_galpha0;
    volatile u_vedu_curld_osd_galpha1 vedu_curld_osd_galpha1;
    volatile unsigned int vedu_curld_osd0_addr_l;
    volatile unsigned int vedu_curld_osd0_addr_h;
    volatile unsigned int vedu_curld_osd1_addr_l;
    volatile unsigned int vedu_curld_osd1_addr_h;
    volatile unsigned int vedu_curld_osd2_addr_l;
    volatile unsigned int vedu_curld_osd2_addr_h;
    volatile unsigned int vedu_curld_osd3_addr_l;
    volatile unsigned int vedu_curld_osd3_addr_h;
    volatile unsigned int vedu_curld_osd4_addr_l;
    volatile unsigned int vedu_curld_osd4_addr_h;
    volatile unsigned int vedu_curld_osd5_addr_l;
    volatile unsigned int vedu_curld_osd5_addr_h;
    volatile unsigned int vedu_curld_osd6_addr_l;
    volatile unsigned int vedu_curld_osd6_addr_h;
    volatile unsigned int vedu_curld_osd7_addr_l;
    volatile unsigned int vedu_curld_osd7_addr_h;
    volatile u_vedu_curld_osd01_stride vedu_curld_osd01_stride;
    volatile u_vedu_curld_osd23_stride vedu_curld_osd23_stride;
    volatile u_vedu_curld_osd45_stride vedu_curld_osd45_stride;
    volatile u_vedu_curld_osd67_stride vedu_curld_osd67_stride;
    volatile unsigned int vedu_reserved_06[18]; /* 18: length of register array */
    volatile u_vedu_vcpi_vlc_config vedu_vcpi_vlc_config;
    volatile u_vedu_cabac_slchdr_size vedu_cabac_slchdr_size;
    volatile u_vedu_cabac_slchdr_part1 vedu_cabac_slchdr_part1;
    volatile unsigned int vedu_cabac_slchdr_part2_seg1;
    volatile unsigned int vedu_cabac_slchdr_part2_seg2;
    volatile unsigned int vedu_cabac_slchdr_part2_seg3;
    volatile unsigned int vedu_cabac_slchdr_part2_seg4;
    volatile unsigned int vedu_cabac_slchdr_part2_seg5;
    volatile unsigned int vedu_cabac_slchdr_part2_seg6;
    volatile unsigned int vedu_cabac_slchdr_part2_seg7;
    volatile unsigned int vedu_cabac_slchdr_part2_seg8;
    volatile u_vedu_cabac_slchdr_size_i vedu_cabac_slchdr_size_i;
    volatile u_vedu_cabac_slchdr_part1_i vedu_cabac_slchdr_part1_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg1_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg2_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg3_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg4_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg5_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg6_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg7_i;
    volatile unsigned int vedu_cabac_slchdr_part2_seg8_i;
    volatile unsigned int vedu_vlc_slchdrstrm0;
    volatile unsigned int vedu_vlc_slchdrstrm1;
    volatile unsigned int vedu_vlc_slchdrstrm2;
    volatile unsigned int vedu_vlc_slchdrstrm3;
    volatile unsigned int vedu_vlc_reorderstrm0;
    volatile unsigned int vedu_vlc_reorderstrm1;
    volatile unsigned int vedu_vlc_markingstrm0;
    volatile unsigned int vedu_vlc_markingstrm1;
    volatile u_vedu_vlc_slchdrpara  vedu_vlc_slchdrpara;
    volatile u_vedu_vlc_svc vedu_vlc_svc;
    volatile unsigned int vedu_vlc_slchdrstrm0_i;
    volatile unsigned int vedu_vlc_slchdrstrm1_i;
    volatile unsigned int vedu_vlc_slchdrstrm2_i;
    volatile unsigned int vedu_vlc_slchdrstrm3_i;
    volatile unsigned int vedu_vlc_reorderstrm0_i;
    volatile unsigned int vedu_vlc_reorderstrm1_i;
    volatile unsigned int vedu_vlc_markingstrm0_i;
    volatile unsigned int vedu_vlc_markingstrm1_i;
    volatile u_vedu_vlc_slchdrpara_i vedu_vlc_slchdrpara_i;
    volatile unsigned int vedu_vlcst_slc_cfg0;
    volatile unsigned int vedu_vlcst_slc_cfg1;
    volatile unsigned int vedu_vlcst_slc_cfg2;
    volatile unsigned int vedu_vlcst_slc_cfg3;
    volatile u_vedu_ice_v3r2_seg_256x1_dcmp_ctrl vedu_ice_v3_r2_seg_256_x1_dcmp_ctrl;
    volatile unsigned int vedu_reserved_07[19]; /* 19: length of register array */
    volatile u_vedu_vcpi_low_power vedu_vcpi_low_power;
    volatile u_vedu_vcpi_pre_judge_ext_en vedu_vcpi_pre_judge_ext_en;
    volatile u_vedu_vcpi_iblk_pre_mv_thr vedu_vcpi_iblk_pre_mv_thr;
    volatile u_vedu_vcpi_iblk_pre_cost_thr vedu_pme_iblk_cost_thr;
    volatile u_vedu_vcpi_pre_judge_cost_thr vedu_vcpi_pre_judge_cost_thr;
    volatile u_vedu_vcpi_pme_param vedu_vcpi_pme_param;
    volatile u_vedu_pme_pblk_pre1 vedu_pme_pblk_pre1;
    volatile u_vedu_pme_pblk_pre2 vedu_pme_pblk_pre2;
    volatile u_vedu_pme_sw_adapt_en vedu_pme_sw_adapt_en;
    volatile u_vedu_pme_window_size0_l0 vedu_pme_window_size0_l0;
    volatile u_vedu_pme_window_size0_l1 vedu_pme_window_size0_l1;
    volatile u_vedu_pme_psw_lpw vedu_pme_psw_lpw;
    volatile u_vedu_pme_skip_pre vedu_pme_skip_pre;
    volatile u_vedu_pme_intrablk_det vedu_pme_intrablk_det;
    volatile u_vedu_pme_skip_flag vedu_pme_skip_flag;
    volatile u_vedu_pme_tr_weightx vedu_pme_tr_weightx;
    volatile u_vedu_pme_tr_weighty vedu_pme_tr_weighty;
    volatile u_vedu_pme_sr_weight vedu_pme_sr_weight;
    volatile u_vedu_ime_fme_lpow_thr vedu_ime_fme_lpow_thr;
    volatile u_vedu_fme_pu64_lwp vedu_fme_pu64_lwp;
    volatile u_vedu_mrg_force_zero_en vedu_mrg_force_zero_en;
    volatile u_vedu_mrg_force_skip_en vedu_mrg_force_skip_en;
    volatile u_vedu_ime_inter_mode vedu_ime_inter_mode;
    volatile u_vedu_ime_layer3to2_thr vedu_ime_layer3_to2_thr;
    volatile u_vedu_ime_layer3to2_thr1 vedu_ime_layer3_to2_thr1;
    volatile u_vedu_ime_layer3to1_thr vedu_ime_layer3_to1_thr;
    volatile u_vedu_ime_layer3to1_thr1 vedu_ime_layer3_to1_thr1;
    volatile u_vedu_vcpi_intra32_low_power vedu_vcpi_intra32_low_power;
    volatile u_vedu_vcpi_intra16_low_power vedu_vcpi_intra16_low_power;
    volatile u_vedu_pme_intra_lowpow vedu_pme_intra_lowpow;
    volatile u_vedu_intra_low_pow0 vedu_intra_low_pow0;
    volatile u_vedu_intra_low_pow1 vedu_intra_low_pow1;
    volatile unsigned int vedu_reserved_08[32]; /* 32: length of register array */
    volatile u_vedu_vcpi_intra_inter_cu_en vedu_vcpi_intra_inter_cu_en;
    volatile u_vedu_pack_pcm_parameter vedu_pack_pcm_parameter;
    volatile u_vedu_qpg_cu32_delta vedu_qpg_cu32_delta;
    volatile u_vedu_qpg_qp_restrain vedu_qpg_qp_restrain;
    volatile u_vedu_pme_qpg_rc_thr1 vedu_pme_qpg_rc_thr1;
    volatile u_vedu_qpg_cu_min_sad_reg vedu_qpg_cu_min_sad_reg;
    volatile unsigned int vedu_qpg_cu_min_sad_thresh_0;
    volatile u_vedu_qpg_lambda_mode vedu_qpg_lambda_mode;
    volatile u_vedu_vcpi_sw_l0_size vedu_vcpi_sw_l0_size;
    volatile u_vedu_vcpi_sw_l1_size vedu_vcpi_sw_l1_size;
    volatile unsigned int vedu_vcpi_pmest_stride;
    volatile u_vedu_pme_new_cost vedu_pme_new_cost;
    volatile u_vedu_pme_new_lambda vedu_pme_new_lambda;
    volatile u_vedu_pme_new_madi_th vedu_pme_new_madi_th;
    volatile u_vedu_pme_cost_offset vedu_pme_cost_offset;
    volatile u_vedu_pme_adjust_pmemv_h264 vedu_pme_adjust_pmemv_h264;
    volatile u_vedu_pme_qpg_rc_th0 vedu_pme_qpg_rc_thr0;
    volatile u_vedu_vlcst_descriptor vedu_vlcst_descriptor;
    volatile u_vedu_curld_clip_thr vedu_curld_clip_thr;
    volatile u_vedu_tqitq_deadzone vedu_tqitq_deadzone;
    volatile u_vedu_vcpi_pintra_texture_strength vedu_vcpi_pintra_texture_strength;
    volatile unsigned int vedu_reserved_09[11]; /* 11: length of register array */
    volatile u_vedu_vctrl_roi_cfg0 vedu_vctrl_roi_cfg0;
    volatile u_vedu_vctrl_roi_cfg1 vedu_vctrl_roi_cfg1;
    volatile u_vedu_vctrl_roi_cfg2 vedu_vctrl_roi_cfg2;
    volatile u_vedu_vctrl_roi_size_0 vedu_vctrl_roi_size_0;
    volatile u_vedu_vctrl_roi_size_1 vedu_vctrl_roi_size_1;
    volatile u_vedu_vctrl_roi_size_2 vedu_vctrl_roi_size_2;
    volatile u_vedu_vctrl_roi_size_3 vedu_vctrl_roi_size_3;
    volatile u_vedu_vctrl_roi_size_4 vedu_vctrl_roi_size_4;
    volatile u_vedu_vctrl_roi_size_5 vedu_vctrl_roi_size_5;
    volatile u_vedu_vctrl_roi_size_6 vedu_vctrl_roi_size_6;
    volatile u_vedu_vctrl_roi_size_7 vedu_vctrl_roi_size_7;
    volatile u_vedu_vctrl_roi_start_0 vedu_vctrl_roi_start_0;
    volatile u_vedu_vctrl_roi_start_1 vedu_vctrl_roi_start_1;
    volatile u_vedu_vctrl_roi_start_2 vedu_vctrl_roi_start_2;
    volatile u_vedu_vctrl_roi_start_3 vedu_vctrl_roi_start_3;
    volatile u_vedu_vctrl_roi_start_4 vedu_vctrl_roi_start_4;
    volatile u_vedu_vctrl_roi_start_5 vedu_vctrl_roi_start_5;
    volatile u_vedu_vctrl_roi_start_6 vedu_vctrl_roi_start_6;
    volatile u_vedu_vctrl_roi_start_7 vedu_vctrl_roi_start_7;
    volatile unsigned int vedu_reserved_10[13]; /* 13: length of register array */
    volatile u_vedu_intra_bit_weight vedu_intra_bit_weight;
    volatile u_vedu_intra_rdo_cost_offset_3 vedu_intra_rdo_cost_offset_3;
    volatile u_vedu_vctrl_intra_rdo_factor_0 vedu_vctrl_intra_rdo_factor_0;
    volatile u_vedu_vctrl_intra_rdo_factor_1 vedu_vctrl_intra_rdo_factor_1;
    volatile u_vedu_vctrl_intra_rdo_factor_2 vedu_vctrl_intra_rdo_factor_2;
    volatile u_vedu_intra_rdo_cost_offset_0 vedu_intra_rdo_cost_offset_0;
    volatile u_vedu_intra_rdo_cost_offset_1 vedu_intra_rdo_cost_offset_1;
    volatile u_vedu_intra_no_dc_cost_offset_0 vedu_intra_no_dc_cost_offset_0;
    volatile u_vedu_intra_no_dc_cost_offset_1 vedu_intra_no_dc_cost_offset_1;
    volatile u_vedu_pme_skip_large_res vedu_pme_skip_large_res;
    volatile u_vedu_mrg_adj_weight vedu_mrg_adj_weight;
    volatile u_vedu_qpg_res_coef vedu_qpg_res_coef;
    volatile u_vedu_pme_bias_cost0 vedu_fme_bias_cost0;
    volatile u_vedu_pme_bias_cost1 vedu_fme_bias_cost1;
    volatile u_vedu_mrg_bias_cost0 vedu_mrg_bias_cost0;
    volatile u_vedu_mrg_bias_cost1 vedu_mrg_bias_cost1;
    volatile u_vedu_mrg_abs_offset0 vedu_mrg_abs_offset0;
    volatile u_vedu_mrg_abs_offset1 vedu_mrg_abs_offset1;
    volatile u_vedu_vctrl_mrg_rdo_factor_0 vedu_vctrl_mrg_rdo_factor_0;
    volatile u_vedu_vctrl_mrg_rdo_factor_1 vedu_vctrl_mrg_rdo_factor_1;
    volatile u_vedu_vctrl_mrg_rdo_factor_2 vedu_vctrl_mrg_rdo_factor_2;
    volatile u_vedu_vctrl_fme_rdo_factor_0 vedu_vctrl_fme_rdo_factor_0;
    volatile u_vedu_vctrl_fme_rdo_factor_1 vedu_vctrl_fme_rdo_factor_1;
    volatile u_vedu_vctrl_fme_rdo_factor_2 vedu_vctrl_fme_rdo_factor_2;
    volatile u_vedu_ime_rdocfg vedu_ime_rdocfg;
    volatile u_vedu_vcpi_pintra_thresh0 vedu_vcpi_pintra_thresh0;
    volatile u_vedu_vcpi_pintra_thresh1 vedu_vcpi_pintra_thresh1;
    volatile u_vedu_vcpi_pintra_thresh2 vedu_vcpi_pintra_thresh2;
    volatile u_vedu_sel_offset_strength vedu_sel_offset_strength;
    volatile u_vedu_sel_cu32_dc_ac_th_offset vedu_sel_cu32_dc_ac_th_offset;
    volatile u_vedu_sel_cu32_qp_th vedu_sel_cu32_qp_th;
    volatile u_vedu_sel_res_dc_ac_th vedu_sel_res_dc_ac_th;
    volatile unsigned int vedu_reserved_11[32]; /* 32: length of register array */
    volatile u_vedu_pme_skin_thr vedu_pme_skin_thr;
    volatile u_vedu_pme_strong_edge vedu_pme_strong_edge;
    volatile u_vedu_pme_skin_sad_thr vedu_pme_skin_sad_thr;
    volatile u_vedu_qpg_skin vedu_qpg_skin;
    volatile u_vedu_pme_inter_strong_edge vedu_pme_inter_strong_edge;
    volatile u_vedu_qpg_hedge vedu_qpg_hedge;
    volatile u_vedu_pme_large_move_thr vedu_pme_large_move_thr;
    volatile u_vedu_qpg_hedge_move vedu_qpg_hedge_move;
    volatile u_vedu_pme_move_scene_thr vedu_pme_move_scene_thr;
    volatile u_vedu_pme_low_luma_thr vedu_pme_low_luma_thr;
    volatile u_vedu_qpg_lowluma vedu_qpg_lowluma;
    volatile u_vedu_pme_chroma_flat vedu_pme_chroma_flat;
    volatile u_vedu_pme_luma_flat vedu_pme_luma_flat;
    volatile u_vedu_pme_madi_flat vedu_pme_madi_flat;
    volatile u_vedu_qpg_flat_region vedu_qpg_flat_region;
    volatile u_vedu_chroma_protect vedu_chroma_protect;
    volatile u_vedu_pme_chroma_strong_edge vedu_pme_chroma_strong_edge;
    volatile u_vedu_chroma_sad_thr vedu_chroma_sad_thr;
    volatile u_vedu_chroma_fg_thr vedu_chroma_fg_thr;
    volatile u_vedu_chroma_bg_thr vedu_chroma_bg_thr;
    volatile u_vedu_chroma_sum_fg_thr vedu_chroma_sum_fg_thr;
    volatile u_vedu_chroma_sum_bg_thr vedu_chroma_sum_bg_thr;
    volatile u_vedu_chroma_fg_count_thr vedu_chroma_fg_count_thr;
    volatile u_vedu_chroma_bg_count_thr vedu_chroma_bg_count_thr;
    volatile u_vedu_pme_intrablk_det_thr vedu_pme_intrablk_det_thr;
    volatile u_vedu_qpg_intra_det vedu_qpg_intra_det;
    volatile u_vedu_vcpi_noforcezero vedu_vcpi_noforcezero;
    volatile unsigned int vedu_reserved_12[37]; /* 37: length of register array */
    volatile u_vedu_qpg_qp_lambda_ctrl_reg00   vedu_qpg_qp_lambda_ctrl_reg00[80]; /* 80: length of register array */
    volatile unsigned int vedu_reserved_13[48]; /* 48: length of register array */
    volatile u_vedu_sao_ssd_area0_start vedu_sao_ssd_area0_start;
    volatile u_vedu_sao_ssd_area0_end vedu_sao_ssd_area0_end;
    volatile u_vedu_sao_ssd_area1_start vedu_sao_ssd_area1_start;
    volatile u_vedu_sao_ssd_area1_end vedu_sao_ssd_area1_end;
    volatile u_vedu_sao_ssd_area2_start vedu_sao_ssd_area2_start;
    volatile u_vedu_sao_ssd_area2_end vedu_sao_ssd_area2_end;
    volatile u_vedu_sao_ssd_area3_start vedu_sao_ssd_area3_start;
    volatile u_vedu_sao_ssd_area3_end vedu_sao_ssd_area3_end;
    volatile u_vedu_sao_ssd_area4_start vedu_sao_ssd_area4_start;
    volatile u_vedu_sao_ssd_area4_end vedu_sao_ssd_area4_end;
    volatile u_vedu_sao_ssd_area5_start vedu_sao_ssd_area5_start;
    volatile u_vedu_sao_ssd_area5_end vedu_sao_ssd_area5_end;
    volatile u_vedu_sao_ssd_area6_start vedu_sao_ssd_area6_start;
    volatile u_vedu_sao_ssd_area6_end vedu_sao_ssd_area6_end;
    volatile u_vedu_sao_ssd_area7_start vedu_sao_ssd_area7_start;
    volatile u_vedu_sao_ssd_area7_end vedu_sao_ssd_area7_end;
    volatile u_vedu_sao_mode vedu_sao_mode;
    volatile unsigned int vedu_reserved_14[15]; /* 15: length of register array */
    volatile u_vedu_vcpi_pic_strong_en vedu_vcpi_pic_strong_en;
    volatile u_vedu_vctrl_nm_acoffset_denoise vedu_vctrl_nm_acoffset_denoise;
    volatile u_vedu_vctrl_nm_engthr_denoise vedu_vctrl_nm_engthr_denoise;
    volatile u_vedu_vctrl_nm_tu8_denoise vedu_vctrl_nm_tu8_denoise;
    volatile u_vedu_vctrl_sk_acoffset_denoise vedu_vctrl_sk_acoffset_denoise;
    volatile u_vedu_vctrl_sk_engthr_denoise vedu_vctrl_sk_engthr_denoise;
    volatile u_vedu_vctrl_sk_tu8_denoise vedu_vctrl_sk_tu8_denoise;
    volatile u_vedu_vctrl_st_acoffset_denoise vedu_vctrl_st_acoffset_denoise;
    volatile u_vedu_vctrl_st_engthr_denoise vedu_vctrl_st_engthr_denoise;
    volatile u_vedu_vctrl_st_tu8_denoise vedu_vctrl_st_tu8_denoise;
    volatile u_vedu_vctrl_se_acoffset_denoise vedu_vctrl_se_acoffset_denoise;
    volatile u_vedu_vctrl_se_engthr_denoise vedu_vctrl_se_engthr_denoise;
    volatile u_vedu_vctrl_se_tu8_denoise vedu_vctrl_se_tu8_denoise;
    volatile u_vedu_vctrl_selm_acoffset_denoise vedu_vctrl_selm_acoffset_denoise;
    volatile u_vedu_vctrl_selm_engthr_denoise vedu_vctrl_selm_engthr_denoise;
    volatile u_vedu_vctrl_selm_tu8_denoise vedu_vctrl_selm_tu8_denoise;
    volatile u_vedu_vctrl_ws_acoffset_denoise vedu_vctrl_ws_acoffset_denoise;
    volatile u_vedu_vctrl_ws_engthr_denoise vedu_vctrl_ws_engthr_denoise;
    volatile u_vedu_vctrl_ws_tu8_denoise vedu_vctrl_ws_tu8_denoise;
    volatile u_vedu_vctrl_ssse_acoffset_denoise vedu_vctrl_ssse_acoffset_denoise;
    volatile u_vedu_vctrl_ssse_engthr_denoise vedu_vctrl_ssse_engthr_denoise;
    volatile u_vedu_vctrl_ssse_tu8_denoise vedu_vctrl_ssse_tu8_denoise;
    volatile u_vedu_vctrl_pr_inter_offset0 vedu_vctrl_pr_inter_offset0;
    volatile u_vedu_vctrl_pr_inter_offset1 vedu_vctrl_pr_inter_offset1;
    volatile unsigned int vedu_reserved_15[8]; /* 8: length of register array */
    volatile unsigned int vedu_vcpi_tunlcell_addr_l;
    volatile unsigned int vedu_vcpi_tunlcell_addr_h;
    volatile unsigned int vedu_vcpi_src_yaddr_l;
    volatile unsigned int vedu_vcpi_src_yaddr_h;
    volatile unsigned int vedu_vcpi_src_caddr_l;
    volatile unsigned int vedu_vcpi_src_caddr_h;
    volatile unsigned int vedu_vcpi_src_vaddr_l;
    volatile unsigned int vedu_vcpi_src_vaddr_h;
    volatile unsigned int vedu_vcpi_yh_addr_l;
    volatile unsigned int vedu_vcpi_yh_addr_h;
    volatile unsigned int vedu_vcpi_ch_addr_l;
    volatile unsigned int vedu_vcpi_ch_addr_h;
    volatile unsigned int vedu_vcpi_rec_yh_addr_l;
    volatile unsigned int vedu_vcpi_rec_yh_addr_h;
    volatile unsigned int vedu_vcpi_rec_ch_addr_l;
    volatile unsigned int vedu_vcpi_rec_ch_addr_h;
    volatile unsigned int vedu_vcpi_pmeld_l0_addr_l;
    volatile unsigned int vedu_vcpi_pmeld_l0_addr_h;
    volatile unsigned int vedu_vcpi_pmeld_l1_addr_l;
    volatile unsigned int vedu_vcpi_pmeld_l1_addr_h;
    volatile unsigned int vedu_vcpi_pmest_addr_l;
    volatile unsigned int vedu_vcpi_pmest_addr_h;
    volatile unsigned int vedu_vcpi_swptraddr_l;
    volatile unsigned int vedu_vcpi_swptraddr_h;
    volatile unsigned int vedu_vcpi_srptraddr_l;
    volatile unsigned int vedu_vcpi_srptraddr_h;
    volatile unsigned int vedu_vcpi_pmeinfo_st_addr_l;
    volatile unsigned int vedu_vcpi_pmeinfo_st_addr_h;
    volatile unsigned int vedu_vcpi_pmeinfo_ld0_addr_l;
    volatile unsigned int vedu_vcpi_pmeinfo_ld0_addr_h;
    volatile unsigned int vedu_vcpi_qpgld_inf_addr_l;
    volatile unsigned int vedu_vcpi_qpgld_inf_addr_h;
    volatile unsigned int vedu_skipweight_ld_addr_l;
    volatile unsigned int vedu_skipweight_ld_addr_h;
    volatile unsigned int vedu_tqitq_matrix_base_addr_l;
    volatile unsigned int vedu_tqitq_matrix_base_addr_h;
    volatile u_vedu_vcpi_stride vedu_vcpi_stride;
    volatile unsigned int vedu_vcpi_pmeld_stride;
    volatile unsigned int vedu_vcpi_refy_l0_addr0_l;
    volatile unsigned int vedu_vcpi_refy_l0_addr0_h;
    volatile unsigned int vedu_vcpi_refy_l0_addr1_l;
    volatile unsigned int vedu_vcpi_refy_l0_addr1_h;
    volatile unsigned int vedu_vcpi_refc_l0_addr0_l;
    volatile unsigned int vedu_vcpi_refc_l0_addr0_h;
    volatile unsigned int vedu_vcpi_refc_l0_addr1_l;
    volatile unsigned int vedu_vcpi_refc_l0_addr1_h;
    volatile unsigned int vedu_vcpi_refyh_l0_addr_l;
    volatile unsigned int vedu_vcpi_refyh_l0_addr_h;
    volatile unsigned int vedu_vcpi_refch_l0_addr_l;
    volatile unsigned int vedu_vcpi_refch_l0_addr_h;
    volatile unsigned int vedu_vcpi_refy_l1_addr0_l;
    volatile unsigned int vedu_vcpi_refy_l1_addr0_h;
    volatile unsigned int vedu_vcpi_refy_l1_addr1_l;
    volatile unsigned int vedu_vcpi_refy_l1_addr1_h;
    volatile unsigned int vedu_vcpi_refc_l1_addr0_l;
    volatile unsigned int vedu_vcpi_refc_l1_addr0_h;
    volatile unsigned int vedu_vcpi_refc_l1_addr1_l;
    volatile unsigned int vedu_vcpi_refc_l1_addr1_h;
    volatile unsigned int vedu_vcpi_refyh_l1_addr_l;
    volatile unsigned int vedu_vcpi_refyh_l1_addr_h;
    volatile unsigned int vedu_vcpi_refch_l1_addr_l;
    volatile unsigned int vedu_vcpi_refch_l1_addr_h;
    volatile unsigned int vedu_vcpi_rec_yaddr0_l;
    volatile unsigned int vedu_vcpi_rec_yaddr0_h;
    volatile unsigned int vedu_vcpi_rec_yaddr1_l;
    volatile unsigned int vedu_vcpi_rec_yaddr1_h;
    volatile unsigned int vedu_vcpi_rec_caddr0_l;
    volatile unsigned int vedu_vcpi_rec_caddr0_h;
    volatile unsigned int vedu_vcpi_rec_caddr1_l;
    volatile unsigned int vedu_vcpi_rec_caddr1_h;
    volatile u_vedu_vcpi_rec_stride vedu_vcpi_rec_stride;
    volatile unsigned int vedu_vcpi_nbi_mvst_addr_l;
    volatile unsigned int vedu_vcpi_nbi_mvst_addr_h;
    volatile unsigned int vedu_vcpi_nbi_mvld_addr_l;
    volatile unsigned int vedu_vcpi_nbi_mvld_addr_h;
    volatile u_vedu_vcpi_ref_l0_stride vedu_vcpi_ref_l0_stride;
    volatile u_vedu_vcpi_ref_l1_stride vedu_vcpi_ref_l1_stride;
    volatile unsigned int vedu_secure_info_l;
    volatile unsigned int vedu_secure_info_h;
    volatile u_vedu_vcpi_ref_l0_length vedu_vcpi_ref_l0_length;
    volatile u_vedu_vcpi_ref_l1_length vedu_vcpi_ref_l1_length;
    volatile u_vedu_vcpi_rec_length vedu_vcpi_rec_length;
    volatile u_vedu_vlcst_parameter vedu_vlcst_parameter;
    volatile unsigned int vedu_vlcst_para_addr_l;
    volatile unsigned int vedu_vlcst_para_addr_h;
    volatile unsigned int vedu_vlcst_strmaddr0_l;
    volatile unsigned int vedu_vlcst_strmaddr0_h;
    volatile unsigned int vedu_vlcst_strmaddr1_l;
    volatile unsigned int vedu_vlcst_strmaddr1_h;
    volatile unsigned int vedu_vlcst_strmaddr2_l;
    volatile unsigned int vedu_vlcst_strmaddr2_h;
    volatile unsigned int vedu_vlcst_strmaddr3_l;
    volatile unsigned int vedu_vlcst_strmaddr3_h;
    volatile unsigned int vedu_vlcst_strmaddr4_l;
    volatile unsigned int vedu_vlcst_strmaddr4_h;
    volatile unsigned int vedu_vlcst_strmaddr5_l;
    volatile unsigned int vedu_vlcst_strmaddr5_h;
    volatile unsigned int vedu_vlcst_strmaddr6_l;
    volatile unsigned int vedu_vlcst_strmaddr6_h;
    volatile unsigned int vedu_vlcst_strmaddr7_l;
    volatile unsigned int vedu_vlcst_strmaddr7_h;
    volatile unsigned int vedu_vlcst_strmaddr8_l;
    volatile unsigned int vedu_vlcst_strmaddr8_h;
    volatile unsigned int vedu_vlcst_strmaddr9_l;
    volatile unsigned int vedu_vlcst_strmaddr9_h;
    volatile unsigned int vedu_vlcst_strmaddr10_l;
    volatile unsigned int vedu_vlcst_strmaddr10_h;
    volatile unsigned int vedu_vlcst_strmaddr11_l;
    volatile unsigned int vedu_vlcst_strmaddr11_h;
    volatile unsigned int vedu_vlcst_strmaddr12_l;
    volatile unsigned int vedu_vlcst_strmaddr12_h;
    volatile unsigned int vedu_vlcst_strmaddr13_l;
    volatile unsigned int vedu_vlcst_strmaddr13_h;
    volatile unsigned int vedu_vlcst_strmaddr14_l;
    volatile unsigned int vedu_vlcst_strmaddr14_h;
    volatile unsigned int vedu_vlcst_strmaddr15_l;
    volatile unsigned int vedu_vlcst_strmaddr15_h;
    volatile unsigned int vedu_vlcst_strmaddr16_l;
    volatile unsigned int vedu_vlcst_strmaddr16_h;
    volatile unsigned int vedu_vlcst_strmaddr17_l;
    volatile unsigned int vedu_vlcst_strmaddr17_h;
    volatile unsigned int vedu_vlcst_strmaddr18_l;
    volatile unsigned int vedu_vlcst_strmaddr18_h;
    volatile unsigned int vedu_vlcst_strmaddr19_l;
    volatile unsigned int vedu_vlcst_strmaddr19_h;
    volatile unsigned int vedu_vlcst_strmaddr20_l;
    volatile unsigned int vedu_vlcst_strmaddr20_h;
    volatile unsigned int vedu_vlcst_strmaddr21_l;
    volatile unsigned int vedu_vlcst_strmaddr21_h;
    volatile unsigned int vedu_vlcst_strmaddr22_l;
    volatile unsigned int vedu_vlcst_strmaddr22_h;
    volatile unsigned int vedu_vlcst_strmaddr23_l;
    volatile unsigned int vedu_vlcst_strmaddr23_h;
    volatile unsigned int vedu_vlcst_strmaddr24_l;
    volatile unsigned int vedu_vlcst_strmaddr24_h;
    volatile unsigned int vedu_vlcst_strmaddr25_l;
    volatile unsigned int vedu_vlcst_strmaddr25_h;
    volatile unsigned int vedu_vlcst_strmaddr26_l;
    volatile unsigned int vedu_vlcst_strmaddr26_h;
    volatile unsigned int vedu_vlcst_strmaddr27_l;
    volatile unsigned int vedu_vlcst_strmaddr27_h;
    volatile unsigned int vedu_vlcst_strmaddr28_l;
    volatile unsigned int vedu_vlcst_strmaddr28_h;
    volatile unsigned int vedu_vlcst_strmaddr29_l;
    volatile unsigned int vedu_vlcst_strmaddr29_h;
    volatile unsigned int vedu_vlcst_strmaddr30_l;
    volatile unsigned int vedu_vlcst_strmaddr30_h;
    volatile unsigned int vedu_vlcst_strmaddr31_l;
    volatile unsigned int vedu_vlcst_strmaddr31_h;
    volatile unsigned int vedu_vlcst_strmbuflen0;
    volatile unsigned int vedu_vlcst_strmbuflen1;
    volatile unsigned int vedu_vlcst_strmbuflen2;
    volatile unsigned int vedu_vlcst_strmbuflen3;
    volatile unsigned int vedu_vlcst_strmbuflen4;
    volatile unsigned int vedu_vlcst_strmbuflen5;
    volatile unsigned int vedu_vlcst_strmbuflen6;
    volatile unsigned int vedu_vlcst_strmbuflen7;
    volatile unsigned int vedu_vlcst_strmbuflen8;
    volatile unsigned int vedu_vlcst_strmbuflen9;
    volatile unsigned int vedu_vlcst_strmbuflen10;
    volatile unsigned int vedu_vlcst_strmbuflen11;
    volatile unsigned int vedu_vlcst_strmbuflen12;
    volatile unsigned int vedu_vlcst_strmbuflen13;
    volatile unsigned int vedu_vlcst_strmbuflen14;
    volatile unsigned int vedu_vlcst_strmbuflen15;
    volatile unsigned int vedu_vlcst_strmbuflen16;
    volatile unsigned int vedu_vlcst_strmbuflen17;
    volatile unsigned int vedu_vlcst_strmbuflen18;
    volatile unsigned int vedu_vlcst_strmbuflen19;
    volatile unsigned int vedu_vlcst_strmbuflen20;
    volatile unsigned int vedu_vlcst_strmbuflen21;
    volatile unsigned int vedu_vlcst_strmbuflen22;
    volatile unsigned int vedu_vlcst_strmbuflen23;
    volatile unsigned int vedu_vlcst_strmbuflen24;
    volatile unsigned int vedu_vlcst_strmbuflen25;
    volatile unsigned int vedu_vlcst_strmbuflen26;
    volatile unsigned int vedu_vlcst_strmbuflen27;
    volatile unsigned int vedu_vlcst_strmbuflen28;
    volatile unsigned int vedu_vlcst_strmbuflen29;
    volatile unsigned int vedu_vlcst_strmbuflen30;
    volatile unsigned int vedu_vlcst_strmbuflen31;
    volatile unsigned int vedu_vlcst_para_data[48]; /* 48: length of register array */
    volatile u_vedu_emar_wch_press_en vedu_emar_wch_press_en;
    volatile u_vedu_emar_rch_press_en vedu_emar_rch_press_en;
    volatile u_vedu_emar_awpress_num_00 vedu_emar_awpress_num_00;
    volatile u_vedu_emar_awpress_num_01 vedu_emar_awpress_num_01;
    volatile u_vedu_emar_awpress_num_02 vedu_emar_awpress_num_02;
    volatile u_vedu_emar_awpress_num_03 vedu_emar_awpress_num_03;
    volatile u_vedu_emar_awpress_num_04 vedu_emar_awpress_num_04;
    volatile u_vedu_emar_awpress_num_05 vedu_emar_awpress_num_05;
    volatile u_vedu_emar_awpress_num_06 vedu_emar_awpress_num_06;
    volatile u_vedu_emar_awpress_num_07 vedu_emar_awpress_num_07;
    volatile u_vedu_emar_wpress_num_00 vedu_emar_wpress_num_00;
    volatile u_vedu_emar_wpress_num_01 vedu_emar_wpress_num_01;
    volatile u_vedu_emar_wpress_num_02 vedu_emar_wpress_num_02;
    volatile u_vedu_emar_wpress_num_03 vedu_emar_wpress_num_03;
    volatile u_vedu_emar_wpress_num_04 vedu_emar_wpress_num_04;
    volatile u_vedu_emar_wpress_num_05 vedu_emar_wpress_num_05;
    volatile u_vedu_emar_wpress_num_06 vedu_emar_wpress_num_06;
    volatile u_vedu_emar_wpress_num_07 vedu_emar_wpress_num_07;
    volatile u_vedu_emar_arpress_num_00 vedu_emar_arpress_num_00;
    volatile u_vedu_emar_arpress_num_01 vedu_emar_arpress_num_01;
    volatile u_vedu_emar_arpress_num_02 vedu_emar_arpress_num_02;
    volatile u_vedu_emar_arpress_num_03 vedu_emar_arpress_num_03;
    volatile u_vedu_emar_arpress_num_04 vedu_emar_arpress_num_04;
    volatile u_vedu_emar_arpress_num_05 vedu_emar_arpress_num_05;
    volatile u_vedu_emar_arpress_num_06 vedu_emar_arpress_num_06;
    volatile u_vedu_emar_arpress_num_07 vedu_emar_arpress_num_07;
    volatile u_vedu_emar_arpress_num_08 vedu_emar_arpress_num_08;
    volatile unsigned int vedu_reserved_16[2048]; /* 2048: length of register array */
} s_vedu_regs_type;

#endif /* __C_UNION_DEFINE_VEDU_H__ */

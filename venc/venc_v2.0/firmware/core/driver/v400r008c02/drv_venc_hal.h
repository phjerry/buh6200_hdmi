/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#ifndef __DRV_VENC_V500R002_HAL_H__
#define __DRV_VENC_V500R002_HAL_H__

#include "venc_hal_ext.h"

static const hi_u32  g_h264_lambda_ssd[40] = { /* 40 : size of g_h264_lambda_ssd */
    1,    1,    1,    2,    2,    3,    3,    4,    5,    7,
    9,   11,   14,   17,   22,   27,   34,   43,   54,   69,
    86,  109,  137,  173,  218,  274,  345,  435,  548,  691,
    870, 1097, 1382, 1741, 2193, 2763, 3482, 4095, 4095, 4095
};
static const hi_u32 g_h264_lambda_sad[40] = { /* 40 : size of g_h264_lambda_sad */
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 6,
    6, 7, 8, 9, 10, 11, 13, 14, 16, 18, 20, 23, 25, 29, 32, 36,
    40, 45, 51, 57, 64, 72, 81, 91
};

static const hi_u32  g_h265_lambda_ssd[50] = { /* 50 : size of g_h265_lambda_ssd */
    18, 22, 28, 36, 45, 57, 72, 91, 115, 145,
    183, 231, 291, 367, 463, 583, 735, 926, 1167, 1470,
    1853, 2334, 2941, 3706, 4669, 5883, 7412, 9338, 11766, 14824,
    18677, 23532, 29649, 37355, 47065, 59298, 74711, 94130, 118596, 131071,
    188260, 237192, 262142
};

static const hi_u32 g_h265_lambda_sad[50] = { /* 50 : size of g_h265_lambda_sad */
    193, 216, 243, 273, 306, 344, 386, 433, 487, 546,
    613, 688, 773, 867, 974, 1093, 1227, 1377, 1546, 1735,
    1948, 2186, 2454, 2755, 3092, 3471, 3896, 4373, 4908, 5510,
    6184, 6942, 7792, 8746, 9817, 11020, 12369, 13884, 15584, 17493,
    19636, 22040, 23170
};

#define D_VENC_ALIGN_UP(val, align) (((val)+((align)-1))&~((align)-1))

#define PARA_ADAPT_ENCODER  0

typedef struct {
    hi_u32 skin_qp_delta;
    hi_u32 rc_low_luma_en ;
    hi_u32 move_scene_en;
    hi_u32 move_sad_en;
    hi_u32 qp_delta;
    hi_u32 skin_max_qp;
    hi_u32 cu_qp_delta_thresh[16]; /* 16: size of cu_qp_delta_thresh */
    hi_u32 lambda[80]; /* 80: size of lambda */
} qpg_attr_dynamic;

typedef struct {
    hi_u32 qp_delta_level[16]; /* 16: size of qp_delta_level */
    hi_u32 qp_madi_switch_thr;

    hi_u32 h264_lowluma_qp_delta;

    hi_u32 bfg_roi_qp_delta;
    hi_u32 large_move_qp_delta;
    hi_u32 strong_edge_qp_delta;
    hi_u32 strong_edge_move_qp_delta;
    hi_u32 strong_edge_move_max_qp;
    hi_u32 strong_edge_move_min_qp;

    hi_u32 bfg_roi_min_qp;
    hi_u32 lowluma_qp_delta;
    hi_u32 intra_det_qp_delta;

    hi_u32 large_move_max_qp;
    hi_u32 strong_edge_max_qp;
    hi_u32 strong_edge_min_qp;
    hi_u32 move_edge_max_qp;
    hi_u32 intra_det_max_qp;
    hi_u32 lowluma_max_qp;
    hi_u32 lowluma_min_qp;

    hi_u32 h264_smooth_qp_delta;
    hi_u32 h264_smooth_qp_delta1;
    hi_u32 h264_smooth_max_qp;
    hi_u32 cu32_delta_low;
    hi_u32 cu32_delta_high;

    hi_u32 ave_lcu_bits;

    hi_u32 qp_restrain_madi_thr;
    hi_u32 qp_restrain_mode;
    hi_u32 qp_restrain_en;
    hi_u32 qp_restrain_delta_blk32;
    hi_u32 lambda_qp_offset;

    hi_u32 post_cu_high_switch_thr;
    hi_u32 post_cu_low_switch_thr;
    hi_u32 post_cu_high_madi_thresh;
    hi_u32 post_cu_low_madi_thresh;
    hi_u32 post_qp_high_level;
    hi_u32 post_qp_low_level;
    hi_u32 post_qp_low_thrsh[8]; /* 8: size of post_qp_low_thrsh */
    hi_u32 post_qp_high_thrsh[8]; /* 8: size of post_qp_high_thrsh */

    hi_u32 vcpi_rc_cu_sad_en;
    hi_u32 vcpi_sad_switch_thr;
    hi_u32 vcpi_rc_cu_sad_mod;
    hi_u32 vcpi_rc_cu_sad_offset;
    hi_u32 vcpi_rc_cu_sad_gain;
    hi_u32 vcpi_curr_sad_level[16]; /* 16: size of vcpi_curr_sad_level */
    hi_u32 vcpi_curr_sad_thresh[16]; /* 16: size of vcpi_curr_sad_thresh */

    hi_u32 vcpi_rc_luma_en;
    hi_u32 vcpi_rc_luma_mode;
    hi_u32 vcpi_rc_luma_switch_thr;
    hi_u32 vcpi_rc_luma_low_madi_thr;
    hi_u32 vcpi_rc_luma_high_madi_thr;
    hi_u32 vcpi_rc_luma_level[16]; /* 16: size of vcpi_rc_luma_level */
    hi_u32 vcpi_rc_luma_thresh[16]; /* 16: size of vcpi_rc_luma_thresh */

    /* chroma prot */
    hi_u32 vcpi_chroma_prot_en;
    hi_u32 vcpi_chroma_max_qp;
    hi_u32 vcpi_chroma_min_qp;
    hi_u32 vcpi_chroma_qp_delta;

    hi_u32 qpg_rc_max_qp;
    hi_u32 qpg_rc_min_qp;

    hi_u32 low_min_sad_en;
    hi_u32 high_min_sad_en;

    hi_u32 rc_h264_smooth_mb_en;
    hi_u32 rc_cu_qp_en;
    hi_u32 rc_row_qp_en;
    hi_u32 rc_line_mode_en;
    hi_u32 rc_line_qp_delta;

    hi_u32 rc_cu_madi_en;
    hi_u32 rc_qp_restrain_en;
    hi_u32 vcpi_bfg_roi_qp_en;
    hi_u32 strong_edge_move_en;
    hi_u32 intra_det_qp_en;
    hi_u32 flat_region_en;
    hi_u32 post_rc_cu_en;
    hi_u32 post_cu_skin_en;
    hi_u32 qp_restrain_large_sad;
    hi_u32 min_sad_thresh_low;
    hi_u32 min_sad_thresh_high;

    hi_u32 min_sad_level;
    hi_u32 low_min_sad_mode;
    hi_u32 high_min_sad_mode;
    hi_u32 qp_restrain_delta_blk16;
    hi_u32 min_sad_madi_en;
    hi_u32 min_sad_qp_restrain_en;
    hi_u32 rdo_lambda_choose_mode;
    hi_u32 lambda_inter_stredge_en;

    hi_u32 rd_smooth_mb_en;
    hi_u32 rd_min_sad_flag_en;
    hi_u32 wr_min_sad_flag_en;
    hi_u32 prev_min_sad_en;
    hi_u32 qpgld_en;
    hi_u32 map_roikeep_en;
    hi_u32 qp_detlta_size_cu64;
    hi_u32 smart_get_cu64_qp_mode;
    hi_u32 smart_get_cu32_qp_mode;
    hi_u32 smart_cu_level_qp_mode;
    hi_u32 smart_abs_qp_mode;

    hi_u32 skin_min_qp;
    hi_u32 stredge_move_min_qp;
    hi_u32 intra_det_min_qp;
    hi_u32 large_move_min_qp;
    hi_u32 stredge_min_qp;
    hi_u32 smooth_min_qp;
    hi_u32 row_target_bits;

    hi_u32 flat_region_min_qp;
    hi_u32 flat_region_max_qp;
    hi_u32 flat_region_qp_delta;

    hi_u32 vcpi_res_coef_en;
    hi_u32 vcpi_large_res_coef;
    hi_u32 vcpi_small_res_coef;
} qpg_attr_static;

typedef struct {
    hi_u32 still_scene_thr;
    hi_u32 high_edge_cnt;
    hi_u32 high_edge_thr;
    hi_u32 interstrongedge_madi_thr;
    hi_u32 interdiff_max_min_madi_times;
    hi_u32 interdiff_max_min_madi_abs;
    hi_u32 move_sad_thr ;
    hi_u32 skin_num;
} pme_attr_dynamic;

typedef struct {
    hi_u32 prdo_en;
    hi_u32 l0_psw_adapt_en;

    hi_u32 l0_psw_thr0;
    hi_u32 l0_psw_thr1;
    hi_u32 l0_psw_thr2;

    hi_u32 l1_psw_adapt_en;
    hi_u32 l1_psw_thr0;
    hi_u32 l1_psw_thr1;
    hi_u32 l1_psw_thr2;

    hi_u32 tr_weightx_2;
    hi_u32 tr_weightx_1;
    hi_u32 tr_weightx_0;
    hi_u32 tr_weighty_2;
    hi_u32 tr_weighty_1;
    hi_u32 tr_weighty_0;
    hi_u32 sr_weight_2;
    hi_u32 sr_weight_1;
    hi_u32 sr_weight_0;
    hi_u32 pme_pskip_strongedge_madi_times;
    hi_u32 pme_pskip_strongedge_madi_thr;

    hi_u32 skipblk_pre_en ;
    hi_u32 skipblk_pre_cost_thr;

    hi_u32 skin_v_min_thr;
    hi_u32 skin_v_max_thr;
    hi_u32 skin_u_min_thr;
    hi_u32 skin_u_max_thr;
    hi_u32 pme_flat_u_thr_low;
    hi_u32 pme_flat_u_thr_high;
    hi_u32 pme_flat_v_thr_low;
    hi_u32 pme_flat_v_thr_high;

    hi_u32 pme_flat_high_luma_thr;
    hi_u32 pme_flat_low_luma_thr;
    hi_u32 pme_flat_luma_madi_thr;
    hi_u32 pme_flat_pmemv_thr;

    hi_u32 pme_flat_madi_times;
    hi_u32 pme_flat_region_cnt;
    hi_u32 pme_flat_icount_thr;
    hi_u32 pme_flat_pmesad_thr;



    hi_u32 pme_intra16_madi_thr;
    hi_u32 pme_intra32_madi_thr;

    hi_u32 pme_intra_lowpow_en;
    hi_u32 pme_iblk_pre_cost_thr_h264;
    hi_u32 pme_intrablk_det_cost_thr1;

    hi_u32 prdo_f1[64]; /* 64: size of prdo_f1 */
    hi_u32 prdo_f2[64]; /* 64: size of prdo_f2 */
    hi_u32 fg_detect_thr;
    hi_u32 fg_detectcnt_thr;
    hi_u32 move_scene_thr;
    hi_u32 cost_lamda_en;
    hi_u32 mvp3median_en;
    hi_u32 new_cost_en;
    hi_u32 cost_lamda2;
    hi_u32 cost_lamda1;
    hi_u32 cost_lamda0;

    hi_u32 l0_win0_width;
    hi_u32 l0_win0_height;
    hi_u32 l1_win0_width;
    hi_u32 l1_win0_height;
    hi_u32 l0_win1_width;
    hi_u32 l0_win1_height;
    hi_u32 l0_win2_width;
    hi_u32 l0_win2_height;
    hi_u32 l0_win3_width;
    hi_u32 l0_win3_height;

    hi_u32 l0_cost_offset;
    hi_u32 l1_cost_offset;
    hi_u32 pme_pskip_mvy_consistency_thr;
    hi_u32 pme_pskip_mvx_consistency_thr;

    hi_u32 pme_intrablk_det_cost_thr0;
    hi_u32 pme_intrablk_det_mvx_thr;
    hi_u32 pme_intrablk_det_mvy_thr;
    hi_u32 pme_intrablk_det_mv_dif_thr0;
    hi_u32 pme_intrablk_det_mv_dif_thr1;

    hi_u32 pme_safe_line;
    hi_u32 pme_safe_line_val;
    hi_u32 pme_safe_line_mode;

    hi_u32 pme_iblk_refresh_en;
    hi_u32 pme_iblk_refresh_mode;

    hi_u32 pme_iblk_refresh_start_num;
    hi_u32 pme_iblk_refresh_num;

    hi_u32 pme_high_luma_thr;
    hi_u32 smooth_madi_thr;
    hi_u32 pme_inter_first;
    hi_u32 low_luma_thr;
    hi_u32 low_luma_madi_thr;
    hi_u32 madi_dif_thr;
    hi_u32 cur_madi_dif_thr;
    hi_u32 min_sad_thr_gain;
    hi_u32 min_sad_thr_offset;
    hi_u32 pme_min_sad_thr_offset_cur;
    hi_u32 pme_min_sad_thr_gain_cur;

    hi_u32 pskip_mvy_consistency_thr;
    hi_u32 pskip_mvx_consistency_thr;

    hi_u32 vcpi_strong_edge_thr_u;
    hi_u32 vcpi_strong_edge_cnt_u;
    hi_u32 vcpi_strong_edge_thr_v;
    hi_u32 vcpi_strong_edge_cnt_v;

    hi_u32 vcpi_chroma_sad_thr_offset;
    hi_u32 vcpi_chroma_sad_thr_gain;

    hi_u32 vcpi_chroma_u0_thr_min;
    hi_u32 vcpi_chroma_u0_thr_max;
    hi_u32 vcpi_chroma_v0_thr_min;
    hi_u32 vcpi_chroma_v0_thr_max;
    hi_u32 vcpi_chroma_u1_thr_min;
    hi_u32 vcpi_chroma_u1_thr_max;
    hi_u32 vcpi_chroma_v1_thr_min;
    hi_u32 vcpi_chroma_v1_thr_max;
    hi_u32 vcpi_chroma_uv0_thr_min;
    hi_u32 vcpi_chroma_uv0_thr_max;
    hi_u32 vcpi_chroma_uv1_thr_min;
    hi_u32 vcpi_chroma_uv1_thr_max;
    hi_u32 vcpi_chroma_count0_thr_min;
    hi_u32 vcpi_chroma_count0_thr_max;
    hi_u32 vcpi_chroma_count1_thr_min;
    hi_u32 vcpi_chroma_count1_thr_max;

    hi_u32 vcpi_move_scene_mv_thr;
    hi_u32 vcpi_move_scene_mv_en;
    hi_u32 pme_skip_sad_thr_offset;
    hi_u32 pme_skip_sad_thr_gain;
    hi_u32 pme_skip_large_res_det;
    hi_u32 vcpi_skin_sad_thr_offset;
    hi_u32 vcpi_skin_sad_thr_gain;
    hi_u32 pme_adjust_pmemv_dist_times;
    hi_u32 pme_adjust_pmemv_en;
    hi_u32 vcpi_new_madi_th0;
    hi_u32 vcpi_new_madi_th1;
    hi_u32 vcpi_new_madi_th2;
    hi_u32 vcpi_new_madi_th3;
    hi_u32 vcpi_new_lambda;
    hi_u32 vcpi_pskip_strongedge_madi_thr;
    hi_u32 vcpi_pskip_strongedge_madi_times;
    hi_u32 vcpi_pskip_flatregion_madi_thr;
    hi_u32 pme_psw_lp_diff_thx;
    hi_u32 pme_psw_lp_diff_thy;
    hi_u32 pme_pblk_pre_madi_times;
    hi_u32 pme_pblk_pre_offset;
    hi_u32 pme_pblk_pre_gain;

    hi_u32 pme_pblk_pre_mv_dif_thr1;
    hi_u32 pme_pblk_pre_mv_dif_thr0;
    hi_u32 pme_pblk_pre_mv_dif_cost_thr;
} pme_attr_static;

typedef struct {
    hi_u32 static_en;
    hi_u32 highedge_en;
    hi_u32 norm16_tr1_denois_max_num;
    hi_u32 skin16_coeff_protect_num;
    hi_u32 static16_tr1_denois_max_num;
    hi_u32 static16_coeff_protect_num;
    hi_u32 hedge16_tr1_denois_max_num;
    hi_u32 hedge16_coeff_protect_num;
    hi_u32 strmov_intra_cu16_rdcost_offset;
    hi_u32 strmov_mrg_cu16_rdcost_offset;
    hi_u32 strmov_fme_cu16_rdcost_offset;
    hi_u32 skin_intra_cu16_rdcost_offset;
    hi_u32 skin_mrg_cu16_rdcost_offset;
    hi_u32 skin_fme_cu16_rdcost_offset;
    hi_u32 hedge_intra_cu16_rdcost_offset;
    hi_u32 edgemov16_coeff_protect_num;
    hi_u32 edgemov16_tr1_denois_max_num;
    hi_u32 rounding_still_mechanism;
    hi_u32 skin_inter_cu_rdcost_offset;
    hi_u32 hedge_inter_cu_rdcost_offset;
    hi_u32 strmov_inter_cu_rdcost_offset;
} img_improve_attr_dynamic;

typedef struct {
    hi_u32 img_improve_en ;
    hi_u32 skin_en ;

    hi_u32 norm32_tr1_denois_max_num;
    hi_u32 norm32_coeff_protect_num;
    hi_u32 norm16_coeff_protect_num;
    hi_u32 skin32_tr1_denois_max_num;
    hi_u32 skin32_coeff_protect_num;
    hi_u32 skin16_tr1_denois_max_num;
    hi_u32 static32_tr1_denois_max_num;
    hi_u32 static32_coeff_protect_num;
    hi_u32 hedge32_tr1_denois_max_num;
    hi_u32 hedge32_coeff_protect_num;

    hi_u32 norm_intra_cu32_rdcost_offset;
    hi_u32 norm_intra_cu16_rdcost_offset;
    hi_u32 norm_intra_cu8_rdcost_offset;
    hi_u32 norm_intra_cu4_rdcost_offset;

    hi_u32 strmov_intra_cu32_rdcost_offset;
    hi_u32 strmov_intra_cu8_rdcost_offset;
    hi_u32 strmov_intra_cu4_rdcost_offset;

    hi_u32 norm_mrg_cu64_rdcost_offset;
    hi_u32 norm_mrg_cu32_rdcost_offset;
    hi_u32 norm_mrg_cu16_rdcost_offset;
    hi_u32 norm_mrg_cu8_rdcost_offset;
    hi_u32 strmov_mrg_cu64_rdcost_offset;
    hi_u32 strmov_mrg_cu32_rdcost_offset;
    hi_u32 strmov_mrg_cu8_rdcost_offset;


    hi_u32 norm_fme_cu64_rdcost_offset;
    hi_u32 norm_fme_cu32_rdcost_offset;
    hi_u32 norm_fme_cu16_rdcost_offset;
    hi_u32 norm_fme_cu8_rdcost_offset;
    hi_u32 strmov_fme_cu64_rdcost_offset;
    hi_u32 strmov_fme_cu32_rdcost_offset;
    hi_u32 strmov_fme_cu8_rdcost_offset;

    hi_u32 skin_intra_cu32_rdcost_offset;
    hi_u32 skin_intra_cu8_rdcost_offset;
    hi_u32 skin_intra_cu4_rdcost_offset;
    hi_u32 sobel_str_intra_cu32_rdcost_offset;
    hi_u32 sobel_str_intra_cu16_rdcost_offset;
    hi_u32 sobel_str_intra_cu8_rdcost_offset;
    hi_u32 sobel_str_intra_cu4_rdcost_offset;

    hi_u32 skin_mrg_cu64_rdcost_offset;
    hi_u32 skin_mrg_cu32_rdcost_offset;
    hi_u32 skin_mrg_cu8_rdcost_offset;
    hi_u32 sobel_str_mrg_cu64_rdcost_offset;
    hi_u32 sobel_str_mrg_cu32_rdcost_offset;
    hi_u32 sobel_str_mrg_cu16_rdcost_offset;
    hi_u32 sobel_str_mrg_cu8_rdcost_offset;

    hi_u32 skin_fme_cu64_rdcost_offset;
    hi_u32 skin_fme_cu32_rdcost_offset;
    hi_u32 skin_fme_cu8_rdcost_offset;
    hi_u32 sobel_str_fme_cu64_rdcost_offset;
    hi_u32 sobel_str_fme_cu32_rdcost_offset;
    hi_u32 sobel_str_fme_cu16_rdcost_offset;
    hi_u32 sobel_str_fme_cu8_rdcost_offset;

    hi_u32 hedge_intra_cu32_rdcost_offset;
    hi_u32 hedge_intra_cu8_rdcost_offset;
    hi_u32 hedge_intra_cu4_rdcost_offset;
    hi_u32 sobel_tex_intra_cu32_rdcost_offset;
    hi_u32 sobel_tex_intra_cu16_rdcost_offset;
    hi_u32 sobel_tex_intra_cu8_rdcost_offset;
    hi_u32 sobel_tex_intra_cu4_rdcost_offset;

    hi_u32 hedge_mrg_cu64_rdcost_offset;
    hi_u32 hedge_mrg_cu32_rdcost_offset;
    hi_u32 hedge_mrg_cu16_rdcost_offset;
    hi_u32 hedge_mrg_cu8_rdcost_offset;
    hi_u32 sobel_tex_mrg_cu64_rdcost_offset;
    hi_u32 sobel_tex_mrg_cu32_rdcost_offset;
    hi_u32 sobel_tex_mrg_cu16_rdcost_offset;
    hi_u32 sobel_tex_mrg_cu8_rdcost_offset;

    hi_u32 hedge_fme_cu64_rdcost_offset;
    hi_u32 hedge_fme_cu32_rdcost_offset;
    hi_u32 hedge_fme_cu16_rdcost_offset;
    hi_u32 hedge_fme_cu8_rdcost_offset;
    hi_u32 sobel_tex_fme_cu64_rdcost_offset;
    hi_u32 sobel_tex_fme_cu32_rdcost_offset;
    hi_u32 sobel_tex_fme_cu16_rdcost_offset;
    hi_u32 sobel_tex_fme_cu8_rdcost_offset;

    hi_u32 rounding_edge_mov_mechanism;
    hi_u32 rounding_edge_mov_degree_thresh;
    hi_u32 rounding_edge_mov_force_zero_resid_thresh;
    hi_u32 rounding_edge_mov_ac_32_sum;
    hi_u32 rounding_edge_mov_ac_16_sum;
    hi_u32 rounding_edge_mov_low_freq_ac_blk_32;
    hi_u32 rounding_edge_mov_low_freq_ac_blk_16;
    hi_u32 rounding_sobel_str_mechanism;
    hi_u32 rounding_sobel_str_degree_thresh;
    hi_u32 rounding_sobel_str_force_zero_resid_thresh;
    hi_u32 rounding_sobel_str_ac_32_sum;
    hi_u32 rounding_sobel_str_ac_16_sum;
    hi_u32 rounding_sobel_str_low_freq_ac_blk_32;
    hi_u32 rounding_sobel_str_low_freq_ac_blk_16 ;
    hi_u32 rounding_sobel_weak_mechanism;
    hi_u32 rounding_sobel_weak_degree_thresh;
    hi_u32 rounding_sobel_weak_force_zero_resid_thresh ;
    hi_u32 rounding_sobel_weak_ac_32_sum;
    hi_u32 rounding_sobel_weak_ac_16_sum;
    hi_u32 rounding_sobel_weak_low_freq_ac_blk_32;
    hi_u32 rounding_sobel_weak_low_freq_ac_blk_16;

    hi_u32 rounding_offset_16x16;
    hi_u32 rounding_offset_32x32;
    hi_u32 rounding_skin_offset_16x16;
    hi_u32 rounding_skin_offset_32x32;
    hi_u32 rounding_still_offset_16x16;
    hi_u32 rounding_still_offset_32x32;
    hi_u32 rounding_edge_offset_16x16;
    hi_u32 rounding_edge_offset_32x32;
    hi_u32 rounding_edge_mov_offset_16x16;
    hi_u32 rounding_edge_mov_offset_32x32;
    hi_u32 rounding_sobel_str_offset_16x16;
    hi_u32 rounding_sobel_str_offset_32x32;
    hi_u32 rounding_sobel_weak_offset_16x16;
    hi_u32 rounding_sobel_weak_offset_32x32;
    hi_u32 edgemov32_coeff_protect_num;
    hi_u32 edgemov32_tr1_denois_max_num;
    hi_u32 sobelstr32_coeff_protect_num;
    hi_u32 sobelstr32_tr1_denois_max_num;
    hi_u32 sobelstr16_coeff_protect_num;
    hi_u32 sobelstr16_tr1_denois_max_num;
    hi_u32 sobelweak32_coeff_protect_num;
    hi_u32 sobelweak32_tr1_denois_max_num;
    hi_u32 sobelweak16_coeff_protect_num;
    hi_u32 sobelweak16_tr1_denois_max_num;

    hi_u32 norm_isolate_ac_enable;
    hi_u32 norm_force_zero_cnt;
    hi_u32 norm_engsum_32;
    hi_u32 norm_engcnt_32;
    hi_u32 norm_engsum_16;
    hi_u32 norm_engcnt_16;
    hi_u32 skin_isolate_ac_enable;
    hi_u32 skin_force_zero_cnt;
    hi_u32 skin_engsum_32;
    hi_u32 skin_engcnt_32;
    hi_u32 skin_engsum_16;
    hi_u32 skin_engcnt_16;
    hi_u32 still_isolate_ac_enable;
    hi_u32 still_force_zero_cnt;
    hi_u32 still_engsum_32;
    hi_u32 still_engcnt_32;
    hi_u32 still_engsum_16;
    hi_u32 still_engcnt_16;
    hi_u32 stredge_isolate_ac_enable;
    hi_u32 stredge_force_zero_cnt;
    hi_u32 stredge_engsum_32;
    hi_u32 stredge_engcnt_32;
    hi_u32 stredge_engsum_16;
    hi_u32 stredge_engcnt_16;
    hi_u32 edgemov_isolate_ac_enable;
    hi_u32 edgemov_force_zero_cnt;
    hi_u32 edgemov_engsum_32;
    hi_u32 edgemov_engcnt_32;
    hi_u32 edgemov_engsum_16;
    hi_u32 edgemov_engcnt_16;
    hi_u32 sobelstr_isolate_ac_enable;
    hi_u32 sobelstr_force_zero_cnt;
    hi_u32 sobelstr_engsum_32;
    hi_u32 sobelstr_engcnt_32;
    hi_u32 sobelstr_engsum_16;
    hi_u32 sobelstr_engcnt_16;
    hi_u32 sobelwk_isolate_ac_enable;
    hi_u32 sobelwk_force_zero_cnt;
    hi_u32 sobelwk_engsum_32;
    hi_u32 sobelwk_engcnt_32;
    hi_u32 sobelwk_engsum_16;
    hi_u32 sobelwk_engcnt_16;

    hi_u32 rounding_mechanism;
    hi_u32 rounding_degree_thresh;
    hi_u32 rounding_force_zero_resid_thresh;
    hi_u32 rounding_ac_32_sum;
    hi_u32 rounding_ac_16_sum;
    hi_u32 rounding_low_freq_ac_blk_32;
    hi_u32 rounding_low_freq_ac_blk_16;

    hi_u32 rounding_skin_mechanism;
    hi_u32 rounding_skin_degree_thresh;
    hi_u32 rounding_skin_force_zero_resid_thresh;
    hi_u32 rounding_skin_ac_32_sum;
    hi_u32 rounding_skin_ac_16_sum;
    hi_u32 rounding_skin_low_freq_ac_blk_32;
    hi_u32 rounding_skin_low_freq_ac_blk_16;

    hi_u32 rounding_still_degree_thresh;
    hi_u32 rounding_still_force_zero_resid_thresh;
    hi_u32 rounding_still_ac_32_sum;
    hi_u32 rounding_still_ac_16_sum;
    hi_u32 rounding_still_low_freq_ac_blk_32;
    hi_u32 rounding_still_low_freq_ac_blk_16;

    hi_u32 rounding_edge_mechanism;
    hi_u32 rounding_edge_degree_thresh;
    hi_u32 rounding_edge_force_zero_resid_thresh;
    hi_u32 rounding_edge_ac_32_sum;
    hi_u32 rounding_edge_ac_16_sum;
    hi_u32 rounding_edge_low_freq_ac_blk_32;
    hi_u32 rounding_edge_low_freq_ac_blk_16;

    hi_u32 norm_sel_cu8_rd_offset;
    hi_u32 skin_sel_cu8_rd_offset;
    hi_u32 hedge_sel_cu8_rd_offset;
    hi_u32 strmov_sel_cu8_rd_offset;
    hi_u32 sobelstr_sel_cu8_rd_offset;
    hi_u32 sobeltex_sel_cu8_rd_offset;

    hi_u32 skin_layer_rdcost_offset;

    hi_u32 sobelstr_inter_cu_rdcost_offset;
    hi_u32 sobeltex_inter_cu_rdcost_offset;
    hi_u32 hedge_layer_rdcost_offset;

    hi_u32 norm_inter_cu_rdcost_offset;
    hi_u32 strmov_layer_rdcost_offset;

    hi_u32 sobelstr_layer_rdcost_offset;
    hi_u32 sobeltex_layer_rdcost_offset;
    hi_u32 norm_layer_rdcost_offset;

    hi_u32 sel_intra4_8_abs_offset;
    hi_u32 sel_intra16_abs_offset;
    hi_u32 sel_inter8_16_abs_offset;
    hi_u32 sel_inter32_abs_offset;
    hi_u32 sel_layer32_abs_offset;

    hi_u32 sel_offset_strength;
} img_improve_attr_static;

typedef struct {
    hi_u32 enable_cfg_err;
    hi_u32 enable_vedu_timeout;
    hi_u32 enable_vedu_step;
    hi_u32 enable_vedu_brkpt;
    hi_u32 enable_vedu_slice_end;
    hi_u32 enable_ve_pbitsover;
    hi_u32 enable_ve_buffull;
    hi_u32 enable_ve_eop;

    hi_u32 lcu_target_bit;
    hi_u32 vcpi_dbgmod;
    hi_u32 vcpi_bkp_en;
    hi_u32 vcpi_time_en;
    hi_u32 vedu_timeout;
    hi_u32 vcpi_refc_nload;
    hi_u32 vcpi_cfg_mode;
    hi_u32 vcpi_lcu_time_sel;
    hi_u32 vcpi_vedsel;
    hi_u32 vcpi_slice_int_en;

    hi_u32 vcpi_cnt_clr;
    hi_u32 vcpi_frame_no;
    hi_u32 vcpi_selfrst_en;
    hi_u32 intra_period;
    hi_u32 vcpi_bp_lcu_y;
    hi_u32 vcpi_bp_lcu_x;
    hi_u32 vcpi_cb_qp_offset;
    hi_u32 vcpi_cr_qp_offset;
    hi_u32 cu_qp_delta_enable_flag;
    hi_u32 vcpi_crop_en;
    hi_u32 vcpi_scale_en;
    hi_u32 vcpi_crop_xstart;
    hi_u32 vcpi_crop_ystart;
    hi_u32 vcpi_crop_xend;
    hi_u32 vcpi_crop_yend;
    hi_u32 vcpi_xscale;
    hi_u32 vcpi_yscale;
    hi_u32 vcpi_oriwidth_pix;
    hi_u32 vcpi_oriheight_pix;
    hi_u32 vcpi_blk_type;
    hi_u32 vcpi_store_mode;
    hi_u32 vcpi_byte_stuffing;
    hi_u32 vcpi_num_refidx;
    hi_u32 vcpi_cabac_init_idc;
    hi_u32 vcpi_ref_idc;
    hi_u32 vcpi_protocol;
    hi_u32 frame_type;

    hi_u32 vcpi_package_sel;
    hi_u32 vcpi_str_fmt;

    hi_u32 vcpi_mrg_cu_en;
    hi_u32 vcpi_fme_cu_en;
    hi_u32 vcpi_force_inter;
    hi_u32 vcpi_pintra_inter_flag_disable;
    hi_u32 vcpi_ipcm_en;
    hi_u32 vcpi_intra_h264_cutdiag;
    hi_u32 vcpi_inter8x8_en;  /* for H264 */
    hi_u32 vcpi_rec_cmp_en;
    hi_u32 vcpi_high_speed_en;
    hi_u32 vcpi_lcu_size;
    hi_u32 vcpi_blk8_inter;
    hi_u32 vcpi_sobel_weight_en;
    hi_u32 vcpi_pr_inter_en;
    hi_u32 vcpi_pskp_en;
    hi_u32 vcpi_tiles_en;
    hi_u32 vcpi_ref1_cmp_en;
    hi_u32 vcpi_ref0_cmp_en;
    hi_u32 vcpi_idr_pic;
    hi_u32 vcpi_dblk_filter_flag;
    hi_s32 vcpi_dblk_alpha;
    hi_s32 vcpi_dblk_beta;
    hi_u32 vcpi_sao_chroma;
    hi_u32 vcpi_sao_luma;
    hi_u32 vcpi_tmv_wr_rd_avail;
    hi_u32 vcpi_cross_tile;
    hi_u32 vcpi_cross_slice;
    hi_u32 vcpi_rft_emasa;
    hi_u32 vcpi_rft_emaa;
    hi_u32 vcpi_rft_emab;
    hi_u32 vcpi_rfs_ema;
    hi_u32 vcpi_rfs_emaw;
    hi_u32 vcpi_ras_ema;
    hi_u32 vcpi_ras_emaw;

    hi_u32 inter_avail;
    hi_u32 intra_smooth;
    hi_u32 intra_dc_shift_en;
    hi_u32 intra_dc_16mode_en;
    hi_u32 intra_dc_8mode_en;
    hi_u32 intra_dc_4mode_en;
    hi_u32 intra_dc_16shift;
    hi_u32 intra_dc_8shift;
    hi_u32 intra_dc_4shift;

    hi_u32 intra_bit_weight;
    hi_u32 intra_dc_16madi;
    hi_u32 intra_dc_8madi;
    hi_u32 intra_dc_4madi;

    hi_u32 intra_cu4_rdo_cost_offset;
    hi_u32 intra_cu8_rdo_cost_offset;
    hi_u32 intra_cu16_non_dc_mode_offset;
    hi_u32 intra_cu32_non_dc_mode_offset;
    hi_u32 intra_cu4_non_dc_mode_offset;
    hi_u32 intra_cu8_non_dc_mode_offset;

    hi_u32 intra_cu4_mode0_31_en;
    hi_u32 intra_cu4_mode32_34_en;
    hi_u32 intra_cu8_mode0_31_en;
    hi_u32 intra_cu8_mode32_34_en;
    hi_u32 intra_cu16_mode0_31_en;
    hi_u32 intra_cu16_mode32_34_en;
    hi_u32 intra_cu32_mode0_31_en;
    hi_u32 intra_cu32_mode32_34_en;
    hi_u32 h264chroma_mode0_en;
    hi_u32 h264chroma_mode1_en;
    hi_u32 h264chroma_mode2_en;
    hi_u32 h264chroma_mode3_en;

    hi_u32 vcpi_tile_height;
    hi_u32 vcpi_tile_width;
    hi_u32 vcpi_pblk_pre_en;
    hi_u32 vcpi_iblk_pre_en;
    hi_u32 vcpi_pblk_pre_cost_thr;
    hi_u32 vcpi_iblk_pre_cost_thr;
    hi_u32 vcpi_iblk_pre_mvy_thr;
    hi_u32 vcpi_iblk_pre_mvx_thr;
    hi_u32 vcpi_iblk_pre_mv_dif_thr1;
    hi_u32 vcpi_iblk_pre_mv_dif_thr0;

    hi_u32 vcpi_ext_edge_en;
    hi_u32 vcpi_skin_close_angle;
    hi_u32 vcpi_rounding_sobel_en;
    hi_u32 vcpi_sw_l0_height;
    hi_u32 vcpi_sw_l0_width;

    hi_u32 narrow_tile_width;
    hi_u32 lcu_performance_baseline;
    hi_u32 vcpi_long_term_refpic ;
    hi_u32 vcpi_ref_num          ;
    hi_u32 num_ref_idx_l0_active_minus1;
    hi_u32 num_ref_idx_l1_active_minus1;
    hi_u32 short_term_ref_pic_set_idx;
    hi_u32 num_short_term_ref_pic_sets;

    hi_u32 refld_vcpi2cont_title_en;
    hi_u32 refld_posx_limit;
    hi_u32 refld_res_thr;
    hi_u32  refld_col2gray_en;

    hi_u32 ime_no_intra_ang_en;
    hi_u32 ime_no_intra_ang_cost_thr;
    hi_u32 ime_no_intra_ang_mvx_thr;
    hi_u32 ime_no_intra_ang_mvy_thr;
    hi_u32 vcpi_lambdaoff16;
    hi_u32 vcpi_lambdaoff8;
    hi_u32 mrg_force_v_zero_en;
    hi_u32 mrg_force_u_zero_en;
    hi_u32 mrg_force_y_zero_en;
    hi_u32 mrg_force_zero_en;
    hi_u32 mrg_force_skip_en;
    hi_u32 pack_sybtax_config;
    hi_u32 vcpi2cu_qp_min_cu_size;
    hi_u32 vcpi2cu_tq_bypass_enabled_flag;
    hi_u32 vcpi2pu_log2_max_ipcm_cbsizey;
    hi_u32 vcpi2pu_log2_min_ipcm_cbsizey;
    hi_u32 vcpi2res_tf_skip_enabled_flag;
    hi_u32 byte_stuff_en;

    hi_u32 max_num_mergecand;
    hi_u32 cabac_max_num_mergecand;
    hi_u32 cabac_nal_unit_head;
    hi_u32 vlc_markingbit;
    hi_u32 vlc_reorderbit;
    hi_u32 vlc_parabit;
    hi_u32 vlc_svc_en;
    hi_u32 vlc_svc_strm;

    hi_u32 tmv_en;
    hi_u32 force_zero_en;
    hi_u32 force_v_zero_en;
    hi_u32 force_u_zero_en;
    hi_u32 force_y_zero_en;
    hi_u32 force_skip_en;
    hi_u32 recst_disable;
    hi_u32 pmv_poc[6]; /* 6: size of  pmv_poc */
    hi_u32 vcpi_svc_strm;
    hi_u32 vcpi_svc_en;
    hi_u32 MaxDecPicBufferM1;
    hi_u32 MaxNumReorderPics;

    hi_u32 curld_col2gray_en;
    hi_u32 curld_clip_en;
    hi_u32 clip_chrm_max;
    hi_u32 clip_chrm_min;
    hi_u32 clip_luma_max;
    hi_u32 clip_luma_min;

    hi_u32 curld_osd0_stride;
    hi_u32 curld_osd1_stride;
    hi_u32 curld_osd2_stride;
    hi_u32 curld_osd3_stride;
    hi_u32 curld_osd4_stride;
    hi_u32 curld_osd5_stride;
    hi_u32 curld_osd6_stride;
    hi_u32 curld_osd7_stride;

    hi_u32 mrg_pu8_bias_cost;
    hi_u32 mrg_pu16_bias_cost;
    hi_u32 mrg_pu32_bias_cost;
    hi_u32 mrg_pu64_bias_cost;

    hi_u32 fme_pu8_bias_cost;
    hi_u32 fme_pu16_bias_cost;
    hi_u32 fme_pu32_bias_cost;
    hi_u32 fme_pu64_bias_cost;

    hi_u32 vcpi_predflag_sel;
    hi_u32 vcpi_pintra_pu16_amp_th;
    hi_u32 vcpi_pintra_pu32_amp_th;
    hi_u32 vcpi_pintra_pu64_amp_th;
    hi_u32 vcpi_pintra_pu16_std_th;
    hi_u32 vcpi_pintra_pu32_std_th;
    hi_u32 vcpi_pintra_pu16_angel_cost_th;
    hi_u32 vcpi_pintra_pu32_angel_cost_th;

    hi_u32 vcpi_insert_i_slc_en;
    hi_u32 vcpi_insert_i_slc_idx;

    hi_u32 vcpi_clkdiv_en;

    hi_u32 pblk_pre_mvy_thr;
    hi_u32 pblk_pre_mvx_thr;
    hi_u32 venc_yuv_file;

    hi_u32 vcpi_sw_l1_height;
    hi_u32 vcpi_sw_l1_width;
} base_attr;

typedef struct {
    hi_u32 vcpi_high3pre_en;
    hi_u32 vcpi_ime_lowpow;
    hi_u32 vcpi_force_intra_lowpwr_en;
    hi_u32 vcpi_fme_lowpow;
    hi_u32 vcpi_intra_lowpow;
    hi_u32 flat_region_force_low3layer;
    hi_u32 vcpi_lowpow_fme_thr1;
    hi_u32 vcpi_lowpow_fme_thr0;
    hi_u32 vcpi_ddr_cross_en;
    hi_u32 fme_gtck_en;
    hi_u32 mrg_gtck_en;
    hi_u32 tqitq_gtck_en;
    hi_u32 vcpi_ddr_cross_idx;
} low_power_attr;

typedef struct {
    hi_u32 deadzone_intra_slice;
    hi_u32 deadzone_inter_slice;
} q_scaling_attr;

typedef struct {
    hi_u32 enable[8];    /* 8: size of enable, only used at H264  */
    hi_u32 abs_qp_en[8]; /* 8: size of abs_qp_en */
    hi_s32 qp[8];        /* 8: size of qp, -26 ~ 25 or 0 ~ 51 */
    hi_u32 width[8];     /* 8: size of width, size in MB */
    hi_u32 height[8];    /* 8: size of height, size in MB */
    hi_u32 start_x[8];    /* 8: size of start_x, size in MB */
    hi_u32 start_y[8];    /* 8: size of start_y, size in MB */
    hi_u32 keep[8];      /* 8: size of keep */
} roi_attr;

#if PARA_ADAPT_ENCODER
typedef struct {
    img_improve_attr_dynamic img_improve_cfg_dynamic[5]; /* 5: size of img_improve_cfg_dynamic */
    qpg_attr_dynamic qpg_cfg_dynamic[5]; /* 5: size of qpg_cfg_dynamic */
    pme_attr_dynamic pme_cfg_dynamic[5]; /* 5: size of pme_cfg_dynamic */
} attr_table;
#endif

typedef struct {
    img_improve_attr_dynamic img_improve_cfg_dynamic;
    qpg_attr_dynamic qpg_cfg_dynamic;
    pme_attr_dynamic pme_cfg_dynamic;
#if PARA_ADAPT_ENCODER
    attr_table enc_para_table[2]; /* 2: size of enc_para_table */
#endif
    hi_u32 tunl_read_intvl;

    hi_u32 vcpi_intra_cu_en;
    hi_u32 vcpi_trans_mode;
    hi_u32 vcpi_entropy_mode;

    /* frame buffer parameter */
    hi_u32 rcn_y_addr_head[2]; /* 2: size of rcn_y_addr_head */
    hi_u32 rcn_c_addr_head[2]; /* 2: size of rcn_c_addr_head */

    hi_u32 slc_hdr_bits_i;
    hi_u32 slc_hdr_stream_i[4]; /* 4: size of slc_hdr_stream_i */
    hi_u32 reorder_stream_i[2]; /* 2: size of reorder_stream_i */
    hi_u32 marking_stream_i[2]; /* 2: size of marking_stream_i */

    hi_u32 cabac_slc_hdr_part2_seg_i[8]; /* 8: size of cabac_slc_hdr_part2_seg_i */
    hi_u32 slchdr_part1_i;
    hi_u32 slchdr_size_part1_i;
    hi_u32 slchdr_size_part2_i;

} venc_hal_dynamic_cfg;

typedef struct {
    img_improve_attr_static img_improve_cfg_static;
    qpg_attr_static qpg_cfg_static;
    pme_attr_static pme_cfg_static;
#ifdef VENC_SUPPORT_ROI
    roi_attr roi_cfg;
#endif
    base_attr base_cfg;
    low_power_attr low_power_cfg;
    q_scaling_attr q_scaling_cfg;

    hi_s32 w_out_std_num;
    hi_s32 r_out_std_num;

    /* smmu */
    hi_s32 glb_bypass;
    hi_s32 int_en;

    hi_s32 vcpi_refy_bypass;
    hi_s32 vcpi_refc_bypass;
    hi_s32 vcpi_strm_bypass;

    hi_s32 vcpi_srcy_bypass;
    hi_s32 vcpi_srcv_bypass;
    hi_s32 vcpi_srcu_bypass;           /* read reg */
    hi_s32 vcpi_pmeinfold1_bypass;
    hi_s32 vcpi_pmeinfold0_bypass;
    hi_s32 vcpi_refch_bypass;
    hi_s32 vcpi_refyh_bypass;
    hi_s32 vcpi_pmeld_bypass;
    hi_s32 vcpi_nbild_bypass;

    hi_s32 vcpi_pmeinfost_bypass;   /* write reg */
    hi_s32 vcpi_recc_bypass;
    hi_s32 vcpi_recy_bypass;
    hi_s32 vcpi_recch_bypass;
    hi_s32 vcpi_recyh_bypass;
    hi_s32 vcpi_pmest_bypass;
    hi_s32 vcpi_nbist_bypass;

    hi_s32 clk_gate_en;
    hi_s32 mem_clk_gate_en;
    hi_u32 time_out;
    hi_u32 pt_bits;
} venc_hal_static_cfg;

typedef struct {
    venc_hal_cfg reg_ext_cfg;
    venc_hal_dynamic_cfg reg_dynamic_cfg;
    venc_hal_static_cfg reg_static_cfg;
} venc_hal_reg_info;

#endif

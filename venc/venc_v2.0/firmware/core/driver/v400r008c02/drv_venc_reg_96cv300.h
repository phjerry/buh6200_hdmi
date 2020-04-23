/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#include "./inc/func_reg.h"
#include "./inc/vedu_reg.h"
#include "./inc/mmu_reg.h"

#ifndef __C_UNION_DEFINE_VEDU_REG_H__
#define __C_UNION_DEFINE_VEDU_REG_H__

/*-------------- Vcpi start  0x20620000 ---------------------*/
typedef struct {
    /* Vcpi寄存器结构体   0x20620000  */
    volatile s_vedu_regs_type vedu_regs;
    /* func寄存器结构体   0x20620000  */
    volatile func_regs_type vedu_func_regs;

    volatile s_mmu_regs_type vedu_mmu_regs;
} vedu_regs_type;


typedef struct {
    hi_u32 cabac_glb_cfg;
    hi_u32 cabac_slchdr_size;
    hi_u32 cabac_slchdr_part1;
    hi_u32 cabac_slchdr_part2_seg1;

    hi_u32 cabac_slchdr_part2_seg2;
    hi_u32 cabac_slchdr_part2_seg3;
    hi_u32 cabac_slchdr_part2_seg4;
    hi_u32 cabac_slchdr_part2_seg5;

    hi_u32 cabac_slchdr_part2_seg6;
    hi_u32 cabac_slchdr_part2_seg7;
    hi_u32 cabac_slchdr_part2_seg8;
    hi_u32 cabac_slchdr_size_i;

    hi_u32 cabac_slchdr_part1_i;
    hi_u32 cabac_slchdr_part2_seg1_i;
    hi_u32 cabac_slchdr_part2_seg2_i;
    hi_u32 cabac_slchdr_part2_seg3_i;

    hi_u32 cabac_slchdr_part2_seg4_i;
    hi_u32 cabac_slchdr_part2_seg5_i;
    hi_u32 cabac_slchdr_part2_seg6_i;
    hi_u32 cabac_slchdr_part2_seg7_i;

    hi_u32 cabac_slchdr_part2_seg8_i;
    hi_u32 reserved[11]; /* 11:size of reserved */
} cabac_slice_head_type;

typedef struct {
    hi_u32 vlc_slchdrstrm0;
    hi_u32 vlc_slchdrstrm1;
    hi_u32 vlc_slchdrstrm2;
    hi_u32 vlc_slchdrstrm3;

    hi_u32 vlc_reorderstrm0;
    hi_u32 vlc_reorderstrm1;
    hi_u32 vlc_markingstrm0;
    hi_u32 vlc_markingstrm1;

    hi_u32 vlc_slchdrpara;
    hi_u32 vlc_svc;
    hi_u32 vlc_slchdrstrm0_i;
    hi_u32 vlc_slchdrstrm1_i;

    hi_u32 vlc_slchdrstrm2_i;
    hi_u32 vlc_slchdrstrm3_i;
    hi_u32 vlc_reorderstrm0_i;
    hi_u32 vlc_reorderstrm1_i;

    hi_u32 vlc_markingstrm0_i;
    hi_u32 vlc_markingstrm1_i;
    hi_u32 vlc_slchdrpara_i;
    hi_u32 reserved[13]; /* 13:size of reserved */
} vlc_slice_head_type;

typedef union {
    cabac_slice_head_type  cabac_slice_head;
    vlc_slice_head_type    vlc_slice_head;
} slice_head_type;

typedef struct {
    hi_u32 vedu_vcpi_tile_size;
    hi_u32 vedu_vcpi_picsize_pix;
    hi_u32 vedu_vcpi_multislc;
    hi_u32 vedu_vcpi_qpcfg;
    hi_u32 vedu_vcpi_dblkcfg;
    hi_u32 vedu_vcpi_tmv_load;
    hi_u32 vedu_vcpi_cross_tile_slc;
    hi_u32 vedu_vcpi_intra_inter_cu_en;
    hi_u32 vedu_vcpi_vlc_config;
    hi_u32 vedu_vcpi_pre_judge_ext_en;
    hi_u32 vedu_vcpi_pre_judge_cost_thr;
    hi_u32 vedu_vcpi_iblk_pre_mv_thr;
    hi_u32 vedu_vcpi_pme_param;
    hi_u32 vedu_vcpi_pic_strong_en;
    hi_u32 vedu_vcpi_ref_flag;
    hi_u32 vedu_vcpi_rc_enable;
    hi_u32 vedu_vcpi_i_slc_insert;
    hi_u32 vedu_vcpi_clkdiv_enable;
    hi_u32 vedu_vcpi_wchn_bypass;
    hi_u32 vedu_vcpi_rchn_bypass;
    hi_u32 vedu_vcpi_sw_l0_size;
    hi_u32 vedu_vcpi_sw_l1_size;
    hi_u32 vedu_vcpi_pmeinfo_st_addr;
    hi_u32 vedu_vcpi_pmeinfo_ld0_addr;
    hi_u32 vedu_vcpi_pmeinfo_ld1_addr;
    hi_u32 vedu_vcpi_tunlcell_addr;
    hi_u32 vedu_vcpi_src_yaddr;
    hi_u32 vedu_vcpi_src_caddr;
    hi_u32 vedu_vcpi_src_vaddr;
    hi_u32 vedu_vcpi_yh_addr;
    hi_u32 vedu_vcpi_ch_addr;
    hi_u32 vedu_vcpi_stride;
    hi_u32 vedu_vcpi_rec_yaddr;
    hi_u32 vedu_vcpi_rec_caddr;
    hi_u32 vedu_vcpi_rec_stride;
    hi_u32 vedu_vcpi_rec_yh_addr;
    hi_u32 vedu_vcpi_rec_ch_addr;
    hi_u32 vedu_vcpi_rec_head_stride;
    hi_u32 vedu_vcpi_refy_l0_addr;
    hi_u32 vedu_vcpi_refc_l0_addr;
    hi_u32 vedu_vcpi_ref_l0_stride;
    hi_u32 vedu_vcpi_refyh_l0_addr;
    hi_u32 vedu_vcpi_refch_l0_addr;
    hi_u32 vedu_vcpi_refh_l0_stride;
    hi_u32 vedu_vcpi_pmeld_l0_addr;
    hi_u32 vedu_vcpi_refy_l1_addr;
    hi_u32 vedu_vcpi_refc_l1_addr;
    hi_u32 vedu_vcpi_ref_l1_stride;
    hi_u32 vedu_vcpi_refyh_l1_addr;
    hi_u32 vedu_vcpi_refch_l1_addr;
    hi_u32 vedu_vcpi_refh_l1_stride;
    hi_u32 vedu_vcpi_pmeld_l1_addr;
    hi_u32 vedu_vcpi_pmest_addr;
    hi_u32 vedu_vcpi_nbi_upst_addr;
    hi_u32 vedu_vcpi_nbi_mvst_addr;
    hi_u32 vedu_vcpi_nbi_mvld_addr;
    hi_u32 vedu_vcpi_strmaddr;
    hi_u32 vedu_vcpi_swptraddr;
    hi_u32 vedu_vcpi_srptraddr;
    hi_u32 vedu_vcpi_strfmt;
    hi_u32 vedu_vcpi_pmest_stride;
    hi_u32 vedu_vcpi_pmeld_stride;
    hi_u32 reserved1[2]; /* 2:size of reserved1 */
    hi_u32 vedu_vctrl_roi_cfg0;
    hi_u32 vedu_vctrl_roi_cfg1;
    hi_u32 vedu_vctrl_roi_cfg2;
    hi_u32 vedu_vctrl_roi_size_0;
    hi_u32 vedu_vctrl_roi_size_1;
    hi_u32 vedu_vctrl_roi_size_2;
    hi_u32 vedu_vctrl_roi_size_3;
    hi_u32 vedu_vctrl_roi_size_4;
    hi_u32 vedu_vctrl_roi_size_5;
    hi_u32 vedu_vctrl_roi_size_6;
    hi_u32 vedu_vctrl_roi_size_7;
    hi_u32 vedu_vctrl_roi_start_0;
    hi_u32 vedu_vctrl_roi_start_1;
    hi_u32 vedu_vctrl_roi_start_2;
    hi_u32 vedu_vctrl_roi_start_3;
    hi_u32 vedu_vctrl_roi_start_4;
    hi_u32 vedu_vctrl_roi_start_5;
    hi_u32 vedu_vctrl_roi_start_6;
    hi_u32 vedu_vctrl_roi_start_7;
    hi_u32 vedu_vctrl_norm_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_norm_coeff_denoise;
    hi_u32 vedu_vctrl_norm_end_denoise;
    hi_u32 vedu_vctrl_skin_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_skin_coeff_denoise;
    hi_u32 vedu_vctrl_skin_end_denoise;
    hi_u32 vedu_vctrl_hedge_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_hedge_coeff_denoise;
    hi_u32 vedu_vctrl_hedge_end_denoise;
    hi_u32 vedu_vctrl_hedgemov_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_hedgemov_coeff_denoise;
    hi_u32 vedu_vctrl_hedgemov_end_denoise;
    hi_u32 vedu_vctrl_static_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_static_coeff_denoise;
    hi_u32 vedu_vctrl_static_end_denoise;
    hi_u32 vedu_vctrl_sobelstr_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_sobelstr_coeff_denoise;
    hi_u32 vedu_vctrl_sobelstr_end_denoise;
    hi_u32 vedu_vctrl_sobelweak_tr16x16_coeff_denoise;
    hi_u32 vedu_vctrl_sobelweak_coeff_denoise;
    hi_u32 vedu_vctrl_sobelweak_end_denoise;
    hi_u32 vedu_vctrl_intra_rdo_factor_0;
    hi_u32 vedu_vctrl_intra_rdo_factor_1;
    hi_u32 vedu_vctrl_intra_rdo_factor_2;
    hi_u32 vedu_vctrl_mrg_rdo_factor_0;
    hi_u32 vedu_vctrl_mrg_rdo_factor_1;
    hi_u32 vedu_vctrl_mrg_rdo_factor_2;
    hi_u32 vedu_vctrl_fme_rdo_factor_0;
    hi_u32 vedu_vctrl_fme_rdo_factor_1;
    hi_u32 vedu_vctrl_fme_rdo_factor_2;
    hi_u32 vedu_vctrl_sel_intra_rdo_factor;
    hi_u32 vedu_vctrl_sel_intra_rdo_factor_0;
    hi_u32 vedu_vctrl_sel_intra_rdo_factor_1;
    hi_u32 vedu_vctrl_sel_intra_rdo_factor_2;
    hi_u32 vedu_vctrl_sel_layer_rdo_factor;
    hi_u32 reserved2[2]; /* 2:size of reserved2 */
    hi_u32 vedu_curld_gcfg;
    hi_u32 vedu_curld_clip_thr;
    hi_u32 vedu_pme_sw_adapt_en;
    hi_u32 vedu_pme_sw_thr0;
    hi_u32 vedu_pme_sw_thr1;
    hi_u32 vedu_pme_sw_thr2;
    hi_u32 vedu_pme_skip_pre;
    hi_u32 vedu_pme_tr_weightx;
    hi_u32 vedu_pme_tr_weighty;
    hi_u32 vedu_pme_sr_weight;
    hi_u32 vedu_pme_intrablk_det;
    hi_u32 vedu_pme_intrablk_det_thr;
    hi_u32 vedu_pme_skin_thr;
    hi_u32 vedu_pme_intra_lowpow;
    hi_u32 vedu_pme_iblk_cost_thr;
    hi_u32 vedu_pme_strong_edge;
    hi_u32 vedu_pme_large_move_thr;
    hi_u32 vedu_pme_inter_strong_edge;
    hi_u32 vedu_pme_new_cost;
    hi_u32 vedu_pme_window_size0_l0;
    hi_u32 vedu_pme_window_size1_l0;
    hi_u32 vedu_pme_window_size2_l0;
    hi_u32 vedu_pme_window_size3_l0;
    hi_u32 vedu_pme_cost_offset;
    hi_u32 vedu_pme_safe_cfg;
    hi_u32 vedu_pme_iblk_refresh;
    hi_u32 vedu_pme_iblk_refresh_num;
    hi_u32 vedu_pme_qpg_rc_thr0;
    hi_u32 vedu_pme_qpg_rc_thr1;
    hi_u32 vedu_pme_low_luma_thr;
    hi_u32 vedu_pme_pblk_pre1;
    hi_u32 vedu_pme_chroma_flat;
    hi_u32 vedu_pme_luma_flat;
    hi_u32 vedu_pme_madi_flat;
    hi_u32 reserved3[2]; /* 2:size of reserved3 */
    hi_u32 vedu_qpg_max_min_qp;
    hi_u32 vedu_qpg_row_target_bits;
    hi_u32 vedu_qpg_average_lcu_bits;
    hi_u32 vedu_qpg_lowluma;
    hi_u32 vedu_qpg_hedge;
    hi_u32 vedu_qpg_hedge_move;
    hi_u32 vedu_qpg_large_move;
    hi_u32 vedu_qpg_skin;
    hi_u32 vedu_qpg_intra_det;
    hi_u32 vedu_qpg_h264_smooth;
    hi_u32 vedu_qpg_cu_qp_delta_thresh_reg0;
    hi_u32 vedu_qpg_cu_qp_delta_thresh_reg1;
    hi_u32 vedu_qpg_cu_qp_delta_thresh_reg2;
    hi_u32 vedu_qpg_cu_qp_delta_thresh_reg3;
    hi_u32 vedu_qpg_delta_level;
    hi_u32 vedu_qpg_madi_switch_thr;
    hi_u32 vedu_qpg_cu32_delta;
    hi_u32 vedu_qpg_qp_lambda_ctrl_reg00[80]; /* [0~79], 80: size of vedu_qpg_qp_lambda_ctrl_reg00 */
    hi_u32 vedu_qpg_lambda_mode;
    hi_u32 vedu_qpg_qp_restrain;
    hi_u32 vedu_qpg_cu_min_sad_thresh_0;
    hi_u32 vedu_qpg_cu_min_sad_thresh_1;
    hi_u32 vedu_qpg_cu_min_sad_reg;
    hi_u32 vedu_qpg_flat_region;
    hi_u32 reserved4[1]; /* 1:size of reserved4 */
    hi_u32 vedu_ime_inter_mode;
    hi_u32 vedu_ime_rdocfg;
    hi_u32 vedu_ime_fme_lpow_thr;
    hi_u32 vedu_ime_no_intra_and_en;
    hi_u32 vedu_ime_no_intra_and_mv_thr;
    hi_u32 vedu_mrg_force_zero_en;
    hi_u32 vedu_mrg_force_skip_en;
    hi_u32 vedu_intra_inter_avail;
    hi_u32 vedu_intra_dc_shift;
    hi_u32 vedu_intra_bit_weight;
    hi_u32 vedu_intra_dc_madi;
    hi_u32 vedu_intra_rdo_cost_offset;
    hi_u32 vedu_intra_chnl4_ang_0en;
    hi_u32 vedu_intra_chnl8_ang_0en;
    hi_u32 vedu_intra_chnl8_ang_1en;
    hi_u32 vedu_intra_chnl16_ang_0en;
    hi_u32 vedu_intra_chnl16_ang_1en;
    hi_u32 vedu_intra_chroma_ang_en;
    hi_u32 vedu_pmv_poc_0;
    hi_u32 vedu_pmv_poc_1;
    hi_u32 vedu_pmv_poc_2;
    hi_u32 vedu_pmv_poc_3;
    hi_u32 vedu_pmv_poc_4;
    hi_u32 vedu_pmv_poc_5;
    hi_u32 vedu_pmv_tmv_en;
    hi_u32 vedu_tqitq_deadzone;
    hi_u32 vedu_sel_intra_abs_offset0;
    hi_u32 vedu_sel_intra_abs_offset1;
    hi_u32 vedu_sel_inter_abs_offset0;
    hi_u32 vedu_sel_offset_strength;
    hi_u32 vedu_sel_cu32_dc_ac_th_offset;
    hi_u32 vedu_sel_cu32_qp_th;
    hi_u32 vedu_sel_res_dc_ac_th;
    hi_u32 vedu_cabac_glb_cfg;
    hi_u32 vedu_cabac_slchdr_size;
    hi_u32 vedu_cabac_slchdr_part1;
    hi_u32 vedu_cabac_slchdr_part2_seg1;
    hi_u32 vedu_cabac_slchdr_part2_seg2;
    hi_u32 vedu_cabac_slchdr_part2_seg3;
    hi_u32 vedu_cabac_slchdr_part2_seg4;
    hi_u32 vedu_cabac_slchdr_part2_seg5;
    hi_u32 vedu_cabac_slchdr_part2_seg6;
    hi_u32 vedu_cabac_slchdr_part2_seg7;
    hi_u32 vedu_cabac_slchdr_part2_seg8;
    hi_u32 vedu_cabac_slchdr_size_i;
    hi_u32 vedu_cabac_slchdr_part1_i;
    hi_u32 vedu_cabac_slchdr_part2_seg1_i;
    hi_u32 vedu_cabac_slchdr_part2_seg2_i;
    hi_u32 vedu_cabac_slchdr_part2_seg3_i;
    hi_u32 vedu_cabac_slchdr_part2_seg4_i;
    hi_u32 vedu_cabac_slchdr_part2_seg5_i;
    hi_u32 vedu_cabac_slchdr_part2_seg6_i;
    hi_u32 vedu_cabac_slchdr_part2_seg7_i;
    hi_u32 vedu_cabac_slchdr_part2_seg8_i;
    hi_u32 vedu_vlc_slchdrstrm0;
    hi_u32 vedu_vlc_slchdrstrm1;
    hi_u32 vedu_vlc_slchdrstrm2;
    hi_u32 vedu_vlc_slchdrstrm3;
    hi_u32 vedu_vlc_reorderstrm0;
    hi_u32 vedu_vlc_reorderstrm1;
    hi_u32 vedu_vlc_markingstrm0;
    hi_u32 vedu_vlc_markingstrm1;
    hi_u32 vedu_vlc_slchdrpara;
    hi_u32 vedu_vlc_svc;
    hi_u32 vedu_vlc_slchdrstrm0_i;
    hi_u32 vedu_vlc_slchdrstrm1_i;
    hi_u32 vedu_vlc_slchdrstrm2_i;
    hi_u32 vedu_vlc_slchdrstrm3_i;
    hi_u32 vedu_vlc_reorderstrm0_i;
    hi_u32 vedu_vlc_reorderstrm1_i;
    hi_u32 vedu_vlc_markingstrm0_i;
    hi_u32 vedu_vlc_markingstrm1_i;
    hi_u32 vedu_vlc_slchdrpara_i;
    hi_u32 vedu_vlcst_ptbits_en;
    hi_u32 vedu_vlcst_ptbits;
    hi_u32 vedu_vlcst_strmbuflen[16];    /* 16:size of vedu_vlcst_strmbuflen */
    hi_u32 vedu_vlcst_strmaddr[16];      /* 16:size of vedu_vlcst_strmaddr */
    hi_u32 vedu_vlcst_para_addr;
    hi_u32 vedu_vlcst_parameter;
    hi_u32 vedu_vlcst_para_data[48];     /* 48:size of vedu_vlcst_para_data */
    hi_u32 vedu_env_chn;
    hi_u32 reserved5[2];                 /* 2:size of reserved5 */
} hevc_avc_node_type;

#endif /* __C_UNION_DEFINE_VCPI_H__ */


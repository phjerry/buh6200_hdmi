/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_cmp_frm_nr.c source file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */
#include "vpss_comm.h"
#include "hal_vpss_cmp_frm_nr.h"

hi_void vpss_print_cmp_frm_alone_cfg(cmp_hqv4_alone_para_input_t *cfg)
{
}

hi_void vpss_print_dcmp_frm_a_lone_cfg(dcmp_hqv4_alone_parm_t *cfg)
{
}

hi_void vpss_mac_set_dcmp_frm_nr(vpss_reg_type *reg, ice_frm_cfg *ice_cfg)
{
    cmp_hqv4_alone_para_input_t cmp_parm_input;
    cmp_hqv4_alone_parm_t cmp_parm_y;
    cmp_hqv4_alone_parm_t cmp_parm_c;
    dcmp_hqv4_alone_parm_t dcmp_parm_y;
    dcmp_hqv4_alone_parm_t dcmp_parm_c;
    hi_bool chrom_cmp_en = HI_FALSE;
    memset(&cmp_parm_input, 0, sizeof(cmp_hqv4_alone_para_input_t));
    memset(&cmp_parm_y, 0, sizeof(cmp_hqv4_alone_parm_t));
    memset(&cmp_parm_c, 0, sizeof(cmp_hqv4_alone_parm_t));
    memset(&dcmp_parm_y, 0, sizeof(dcmp_hqv4_alone_parm_t));
    memset(&dcmp_parm_c, 0, sizeof(dcmp_hqv4_alone_parm_t));
    cmp_parm_input.frame_width = ice_cfg->frame_wth;
    cmp_parm_input.frame_height = ice_cfg->frame_hgt;
    cmp_parm_input.frame_bitdepth = ice_cfg->bit_depth;
    cmp_parm_input.cmp_mode = ice_cfg->is_raw_en;
    cmp_parm_input.is_lossless = !ice_cfg->is_lossy_y;
    cmp_parm_input.pixel_format = ice_cfg->data_fmt;
    cmp_parm_input.comp_ratio_y_int = ice_cfg->cmp_ratio_y;
    cmp_parm_input.comp_ratio_c_int = ice_cfg->cmp_ratio_c;
    cmp_parm_input.rc_type_mode = ice_cfg->cmp_cfg_mode;
    cmp_parm_input.rm_debug_en = 0;
    cmp_parm_input.part_cmp_en = ice_cfg->part_mode_cfg.en;
    cmp_parm_input.pcmp_start_hpos_y = ice_cfg->part_mode_cfg.start_pos;
    cmp_parm_input.pcmp_end_hpos_y = ice_cfg->part_mode_cfg.end_pos;
    cmp_parm_input.pcmp_start_hpos_c = ice_cfg->part_mode_cfg.start_pos_c;
    cmp_parm_input.pcmp_end_hpos_c = ice_cfg->part_mode_cfg.end_pos_c;
    vpss_print_cmp_frm_alone_cfg(&cmp_parm_input);
    chrom_cmp_en = ice_cfg->cmp_en;

    if (ice_cfg->cmp_en) {
        vpss_cmphqv4_compresser_init_y(&cmp_parm_y, &cmp_parm_input);
        cmp_parm_input.is_lossless = !ice_cfg->is_lossy_c;
        vpss_cmphqv4_compresser_init_c(&cmp_parm_c, &cmp_parm_input);
        vpss_cmphqv4_rand_reg_assert_alone_yc(&cmp_parm_y, &cmp_parm_c);
        vpss_cmphqv4_decompress_init_alone(&dcmp_parm_y, &cmp_parm_y, &cmp_parm_input);
        vpss_cmphqv4_decompress_init_alone(&dcmp_parm_c, &cmp_parm_c, &cmp_parm_input);
        vpss_print_dcmp_frm_a_lone_cfg(&dcmp_parm_y);
        vpss_print_dcmp_frm_a_lone_cfg(&dcmp_parm_c);
        vpss_ice_set_dcmp_frm_alone_y(reg, &dcmp_parm_y, ice_cfg->cmp_en);
        vpss_ice_set_dcmp_frm_alone_c(reg, &dcmp_parm_c, chrom_cmp_en);
    }
}

hi_void vpss_mac_set_cmp_frm_nr(vpss_reg_type *reg, ice_frm_cfg *ice_cfg)
{
    cmp_hqv4_alone_para_input_t cmp_parm_input;
    cmp_hqv4_alone_parm_t cmp_parm_y;
    cmp_hqv4_alone_parm_t cmp_parm_c;
    hi_bool chrom_cmp_en = HI_FALSE;
    memset(&cmp_parm_input, 0, sizeof(cmp_hqv4_alone_para_input_t));
    memset(&cmp_parm_y, 0, sizeof(cmp_hqv4_alone_parm_t));
    memset(&cmp_parm_c, 0, sizeof(cmp_hqv4_alone_parm_t));
    cmp_parm_input.frame_width = ice_cfg->frame_wth;
    cmp_parm_input.frame_height = ice_cfg->frame_hgt;
    cmp_parm_input.frame_bitdepth = ice_cfg->bit_depth;
    cmp_parm_input.cmp_mode = ice_cfg->is_raw_en;
    cmp_parm_input.is_lossless = !ice_cfg->is_lossy_y;
    cmp_parm_input.pixel_format = ice_cfg->data_fmt;
    cmp_parm_input.comp_ratio_y_int = ice_cfg->cmp_ratio_y;
    cmp_parm_input.comp_ratio_c_int = ice_cfg->cmp_ratio_c;
    cmp_parm_input.rc_type_mode = ice_cfg->cmp_cfg_mode;
    cmp_parm_input.rm_debug_en = 0;
    cmp_parm_input.part_cmp_en = ice_cfg->part_mode_cfg.en;
    cmp_parm_input.pcmp_start_hpos_y = ice_cfg->part_mode_cfg.start_pos;
    cmp_parm_input.pcmp_end_hpos_y = ice_cfg->part_mode_cfg.end_pos;
    cmp_parm_input.pcmp_start_hpos_c = ice_cfg->part_mode_cfg.start_pos_c;
    cmp_parm_input.pcmp_end_hpos_c = ice_cfg->part_mode_cfg.end_pos_c;
    vpss_print_cmp_frm_alone_cfg(&cmp_parm_input);
    chrom_cmp_en = ice_cfg->cmp_en;

    if (ice_cfg->cmp_en) {
        vpss_cmphqv4_compresser_init_y(&cmp_parm_y, &cmp_parm_input);
        cmp_parm_input.is_lossless = !ice_cfg->is_lossy_c;
        vpss_cmphqv4_compresser_init_c(&cmp_parm_c, &cmp_parm_input);
        vpss_cmphqv4_rand_reg_assert_alone_yc(&cmp_parm_y, &cmp_parm_c);
        vpss_ice_set_cmp_frm_alone_y(reg, &cmp_parm_y, ice_cfg->cmp_en);
        vpss_ice_set_cmp_frm_alone_c(reg, &cmp_parm_c, chrom_cmp_en);
    }
}

// nr y dcmp
hi_void vpss_ice_set_dcmp_frm_alone_y(vpss_reg_type *reg, dcmp_hqv4_alone_parm_t *cmp_parm, hi_bool dcmp_en)
{
    vpss_mac_set_nr_refy_dcmp_en(reg, dcmp_en);
    vpss_mac_set_nr_refy_bd_det_en(reg, cmp_parm->bd_det_en);
    vpss_mac_set_nr_refy_budget_mb_bits(reg, cmp_parm->budget_mb_bits);
    vpss_mac_set_nr_refy_buffer_fullness_thr(reg, cmp_parm->buffer_fullness_thr);
    vpss_mac_set_nr_refy_buffer_init_bits(reg, cmp_parm->buffer_init_bits);
    vpss_mac_set_nr_refy_cmp_mode(reg, cmp_parm->cmp_mode);
    vpss_mac_set_nr_refy_frame_bitdepth(reg, cmp_parm->frame_bitdepth);
    vpss_mac_set_nr_refy_frame_height(reg, cmp_parm->frame_height - 1);
    vpss_mac_set_nr_refy_frame_width(reg, cmp_parm->frame_width - 1);
    vpss_mac_set_nr_refy_is_lossless(reg, cmp_parm->is_lossless);
    vpss_mac_set_nr_refy_pixel_format(reg, cmp_parm->pixel_format);
}

// nr c dcmp
hi_void vpss_ice_set_dcmp_frm_alone_c(vpss_reg_type *reg, dcmp_hqv4_alone_parm_t *cmp_parm, hi_bool dcmp_en)
{
    vpss_mac_set_nr_refc_dcmp_en(reg, dcmp_en);
    vpss_mac_set_nr_refc_bd_det_en(reg, cmp_parm->bd_det_en);
    vpss_mac_set_nr_refc_budget_mb_bits(reg, cmp_parm->budget_mb_bits);
    vpss_mac_set_nr_refc_buffer_fullness_thr(reg, cmp_parm->buffer_fullness_thr);
    vpss_mac_set_nr_refc_buffer_init_bits(reg, cmp_parm->buffer_init_bits);
    vpss_mac_set_nr_refc_cmp_mode(reg, cmp_parm->cmp_mode);
    vpss_mac_set_nr_refc_frame_bitdepth(reg, cmp_parm->frame_bitdepth);

    if (ICE_DATA_FMT_YUV420 == cmp_parm->pixel_format) {
        vpss_mac_set_nr_refc_frame_height(reg, cmp_parm->frame_height / 2 - 1); // c height =  frame height /2
    } else if (ICE_DATA_FMT_YUV422 == cmp_parm->pixel_format) {
        vpss_mac_set_nr_refc_frame_height(reg, cmp_parm->frame_height - 1);
    } else {
        vpss_assert_not_ret(0);
    }

    vpss_mac_set_nr_refc_frame_width(reg, cmp_parm->frame_width - 1);
    vpss_mac_set_nr_refc_is_lossless(reg, cmp_parm->is_lossless);
    vpss_mac_set_nr_refc_pixel_format(reg, cmp_parm->pixel_format);
}

// nr y cmp
hi_void vpss_ice_set_cmp_frm_alone_y(vpss_reg_type *reg, cmp_hqv4_alone_parm_t *cmp_parm, hi_bool cmp_en)
{
    vpss_mac_set_nr_rfry_cmp_en(reg, cmp_en);
    vpss_mac_set_nr_rfry_big_grad_thr(reg, cmp_parm->big_grad_thr);
    vpss_mac_set_nr_rfry_bits_offset_reg0(reg, cmp_parm->bits_offset_reg0);
    vpss_mac_set_nr_rfry_bits_offset_reg1(reg, cmp_parm->bits_offset_reg1);
    vpss_mac_set_nr_rfry_bits_offset_reg2(reg, cmp_parm->bits_offset_reg2);
    vpss_mac_set_nr_rfry_buf_fullness_thr_reg0(reg, cmp_parm->buf_fullness_thr_reg0);
    vpss_mac_set_nr_rfry_buf_fullness_thr_reg1(reg, cmp_parm->buf_fullness_thr_reg1);
    vpss_mac_set_nr_rfry_buf_fullness_thr_reg2(reg, cmp_parm->buf_fullness_thr_reg2);
    vpss_mac_set_nr_rfry_buffer_init_bits(reg, cmp_parm->buffer_init_bits);
    vpss_mac_set_nr_rfry_cmp_mode(reg, cmp_parm->cmp_mode);
    vpss_mac_set_nr_rfry_diff_thr(reg, cmp_parm->diff_thr);
    vpss_mac_set_nr_rfry_est_err_gain_map(reg, cmp_parm->est_err_gain_map);
    vpss_mac_set_nr_rfry_first_col_adj_bits(reg, cmp_parm->first_col_adj_bits);
    vpss_mac_set_nr_rfry_first_mb_adj_bits(reg, cmp_parm->first_mb_adj_bits);
    vpss_mac_set_nr_rfry_first_row_adj_bits(reg, cmp_parm->first_row_adj_bits);
    vpss_mac_set_nr_rfry_frame_bitdepth(reg, cmp_parm->frame_bitdepth);
    vpss_mac_set_nr_rfry_frame_height(reg, cmp_parm->frame_height - 1);
    vpss_mac_set_nr_rfry_frame_width(reg, cmp_parm->frame_width - 1);
    vpss_mac_set_nr_rfry_grph_bits_penalty(reg, cmp_parm->grph_bits_penalty);
    vpss_mac_set_nr_rfry_grph_en(reg, cmp_parm->grph_en);
    vpss_mac_set_nr_rfry_grph_ideal_bits(reg, cmp_parm->grph_ideal_bits);
    vpss_mac_set_nr_rfry_is_lossless(reg, cmp_parm->is_lossless);
    vpss_mac_set_nr_rfry_mb_bits(reg, cmp_parm->mb_bits);
    vpss_mac_set_nr_rfry_min_mb_bits(reg, cmp_parm->min_mb_bits);
    vpss_mac_set_nr_rfry_noise_pix_num_thr(reg, cmp_parm->noise_pix_num_thr);
    vpss_mac_set_nr_rfry_part_cmp_en(reg, cmp_parm->part_cmp_en);
    vpss_mac_set_nr_rfry_pcmp_end_hpos(reg, cmp_parm->pcmp_end_hpos);
    vpss_mac_set_nr_rfry_pcmp_start_hpos(reg, cmp_parm->pcmp_start_hpos);
    vpss_mac_set_nr_rfry_pixel_format(reg, cmp_parm->pixel_format);
    vpss_mac_set_nr_rfry_qp_dec1_bits_thr(reg, cmp_parm->qp_dec1_bits_thr);
    vpss_mac_set_nr_rfry_qp_dec2_bits_thr(reg, cmp_parm->qp_dec2_bits_thr);
    vpss_mac_set_nr_rfry_qp_force_en(reg, cmp_parm->qp_force_en);
    vpss_mac_set_nr_rfry_qp_inc1_bits_thr(reg, cmp_parm->qp_inc1_bits_thr);
    vpss_mac_set_nr_rfry_qp_inc2_bits_thr(reg, cmp_parm->qp_inc2_bits_thr);
    vpss_mac_set_nr_rfry_qp_rge_reg0(reg, cmp_parm->qp_rge_reg0);
    vpss_mac_set_nr_rfry_qp_rge_reg1(reg, cmp_parm->qp_rge_reg1);
    vpss_mac_set_nr_rfry_qp_rge_reg2(reg, cmp_parm->qp_rge_reg2);
    vpss_mac_set_nr_rfry_smooth_status_thr(reg, cmp_parm->smooth_status_thr);
    vpss_mac_set_nr_rfry_smth_pix_num_thr(reg, cmp_parm->smth_pix_num_thr);
    vpss_mac_set_nr_rfry_smth_thr(reg, cmp_parm->smth_thr);
    vpss_mac_set_nr_rfry_still_pix_num_thr(reg, cmp_parm->still_pix_num_thr);
    vpss_mac_set_nr_rfry_still_thr(reg, cmp_parm->still_thr);
}

// nr c cmp
hi_void vpss_ice_set_cmp_frm_alone_c(vpss_reg_type *reg, cmp_hqv4_alone_parm_t *cmp_parm, hi_bool cmp_en)
{
    vpss_mac_set_nr_rfrc_cmp_en(reg, cmp_en);
    vpss_mac_set_nr_rfrc_big_grad_thr(reg, cmp_parm->big_grad_thr);
    vpss_mac_set_nr_rfrc_bits_offset_reg0(reg, cmp_parm->bits_offset_reg0);
    vpss_mac_set_nr_rfrc_bits_offset_reg1(reg, cmp_parm->bits_offset_reg1);
    vpss_mac_set_nr_rfrc_bits_offset_reg2(reg, cmp_parm->bits_offset_reg2);
    vpss_mac_set_nr_rfrc_buf_fullness_thr_reg0(reg, cmp_parm->buf_fullness_thr_reg0);
    vpss_mac_set_nr_rfrc_buf_fullness_thr_reg1(reg, cmp_parm->buf_fullness_thr_reg1);
    vpss_mac_set_nr_rfrc_buf_fullness_thr_reg2(reg, cmp_parm->buf_fullness_thr_reg2);
    vpss_mac_set_nr_rfrc_buffer_init_bits(reg, cmp_parm->buffer_init_bits);
    vpss_mac_set_nr_rfrc_cmp_mode(reg, cmp_parm->cmp_mode);
    vpss_mac_set_nr_rfrc_diff_thr(reg, cmp_parm->diff_thr);
    vpss_mac_set_nr_rfrc_est_err_gain_map(reg, cmp_parm->est_err_gain_map);
    vpss_mac_set_nr_rfrc_first_col_adj_bits(reg, cmp_parm->first_col_adj_bits);
    vpss_mac_set_nr_rfrc_first_mb_adj_bits(reg, cmp_parm->first_mb_adj_bits);
    vpss_mac_set_nr_rfrc_first_row_adj_bits(reg, cmp_parm->first_row_adj_bits);
    vpss_mac_set_nr_rfrc_frame_bitdepth(reg, cmp_parm->frame_bitdepth);

    if (ICE_DATA_FMT_YUV420 == cmp_parm->pixel_format) {
        vpss_mac_set_nr_rfrc_frame_height(reg, cmp_parm->frame_height / 2 - 1); // c height =  frame height /2
    } else if (ICE_DATA_FMT_YUV422 == cmp_parm->pixel_format) {
        vpss_mac_set_nr_rfrc_frame_height(reg, cmp_parm->frame_height - 1);
    } else {
        vpss_assert_not_ret(0);
    }

    vpss_mac_set_nr_rfrc_frame_width(reg, cmp_parm->frame_width - 1);
    vpss_mac_set_nr_rfrc_grph_bits_penalty(reg, cmp_parm->grph_bits_penalty);
    vpss_mac_set_nr_rfrc_grph_en(reg, cmp_parm->grph_en);
    vpss_mac_set_nr_rfrc_grph_ideal_bits(reg, cmp_parm->grph_ideal_bits);
    vpss_mac_set_nr_rfrc_is_lossless(reg, cmp_parm->is_lossless);
    vpss_mac_set_nr_rfrc_mb_bits(reg, cmp_parm->mb_bits);
    vpss_mac_set_nr_rfrc_min_mb_bits(reg, cmp_parm->min_mb_bits);
    vpss_mac_set_nr_rfrc_noise_pix_num_thr(reg, cmp_parm->noise_pix_num_thr);
    vpss_mac_set_nr_rfrc_part_cmp_en(reg, cmp_parm->part_cmp_en);
    vpss_mac_set_nr_rfrc_pcmp_end_hpos(reg, cmp_parm->pcmp_end_hpos);
    vpss_mac_set_nr_rfrc_pcmp_start_hpos(reg, cmp_parm->pcmp_start_hpos);
    vpss_mac_set_nr_rfrc_pixel_format(reg, cmp_parm->pixel_format);
    vpss_mac_set_nr_rfrc_qp_dec1_bits_thr(reg, cmp_parm->qp_dec1_bits_thr);
    vpss_mac_set_nr_rfrc_qp_dec2_bits_thr(reg, cmp_parm->qp_dec2_bits_thr);
    vpss_mac_set_nr_rfrc_qp_force_en(reg, cmp_parm->qp_force_en);
    vpss_mac_set_nr_rfrc_qp_inc1_bits_thr(reg, cmp_parm->qp_inc1_bits_thr);
    vpss_mac_set_nr_rfrc_qp_inc2_bits_thr(reg, cmp_parm->qp_inc2_bits_thr);
    vpss_mac_set_nr_rfrc_qp_rge_reg0(reg, cmp_parm->qp_rge_reg0);
    vpss_mac_set_nr_rfrc_qp_rge_reg1(reg, cmp_parm->qp_rge_reg1);
    vpss_mac_set_nr_rfrc_qp_rge_reg2(reg, cmp_parm->qp_rge_reg2);
    vpss_mac_set_nr_rfrc_smooth_status_thr(reg, cmp_parm->smooth_status_thr);
    vpss_mac_set_nr_rfrc_smth_pix_num_thr(reg, cmp_parm->smth_pix_num_thr);
    vpss_mac_set_nr_rfrc_smth_thr(reg, cmp_parm->smth_thr);
    vpss_mac_set_nr_rfrc_still_pix_num_thr(reg, cmp_parm->still_pix_num_thr);
    vpss_mac_set_nr_rfrc_still_thr(reg, cmp_parm->still_thr);
}


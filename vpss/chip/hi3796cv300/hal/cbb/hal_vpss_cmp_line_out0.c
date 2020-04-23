/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_cmp_line_out0.c source file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */
#include "vpss_comm.h"
#include "hal_vpss_cmp_line_out0.h"
#include "hal_vpss_reg.h"

hi_void print_ice_cfg(ice_v3_r2_line_input *cfg)
{
#ifdef MAC_DRV_DEBUG
    vpss_error("frame_width:%d \n", cfg->frame_width);
    vpss_error("frame_height:%d \n", cfg->frame_height);
    vpss_error("frame_bitdepth:%d \n", cfg->frame_bitdepth);
    vpss_error("cmp_mode:%d \n", cfg->cmp_mode);
    vpss_error("is_lossless:%d \n", cfg->is_lossless);
    vpss_error("pixel_format:%d \n", cfg->pixel_format);
    vpss_error("part_cmp_en:%d \n", cfg->part_cmp_en);
    vpss_error("pcmp_start_hpos:%d \n", cfg->pcmp_start_hpos);
    vpss_error("pcmp_end_hpos:%d \n", cfg->pcmp_end_hpos);
    vpss_error("slice_cmp_en:%d \n", cfg->slice_cmp_en);
    vpss_error("start_slice_idx:%d \n", cfg->start_slice_idx);
    vpss_error("comp_ratio_int:%d \n", cfg->comp_ratio_int);
    vpss_error("rm_debug_en:%d \n", cfg->rm_debug_en);
    vpss_error("rc_type_mode:%d \n", cfg->rc_type_mode);
#endif
}

hi_void vpss_ice_set_ycmp_line_out0(vpss_reg_type *reg, ice_v3r2_line_cfg *cmp_parm, hi_bool cmp_en)
{
    vpss_set_out0y_ice_en(reg, 0, cmp_en);
    vpss_set_out0y_bit_depth(reg, 0, cmp_parm->bit_depth);
    vpss_set_out0y_esl_qp(reg, 0, cmp_parm->esl_qp);
    vpss_set_out0y_chroma_en(reg, 0, cmp_parm->chroma_en);
    vpss_set_out0y_is_lossless(reg, 0, cmp_parm->is_lossless);
    vpss_set_out0y_cmp_mode(reg, 0, cmp_parm->cmp_mode);
    vpss_set_out0y_frame_width(reg, 0, cmp_parm->frame_width - 1);
    vpss_set_out0y_frame_height(reg, 0, cmp_parm->frame_height - 1);
    vpss_set_out0y_min_mb_bits(reg, 0, cmp_parm->min_mb_bits);
    vpss_set_out0y_budget_mb_bits_last(reg, 0, cmp_parm->budget_mb_bits_last);
    vpss_set_out0y_budget_mb_bits(reg, 0, cmp_parm->budget_mb_bits);
    vpss_set_out0y_max_mb_qp(reg, 0, cmp_parm->max_mb_qp);
    vpss_set_out0y_noise_pix_num_thr(reg, 0, cmp_parm->noise_pix_num_thr);
    vpss_set_out0y_smooth_status_thr(reg, 0, cmp_parm->smooth_status_thr);
    vpss_set_out0y_diff_thr(reg, 0, cmp_parm->diff_thr);
    vpss_set_out0y_big_grad_thr(reg, 0, cmp_parm->big_grad_thr);
    vpss_set_out0y_qp_dec2_bits_thr(reg, 0, cmp_parm->qp_dec2_bits_thr);
    vpss_set_out0y_qp_dec1_bits_thr(reg, 0, cmp_parm->qp_dec1_bits_thr);
    vpss_set_out0y_qp_inc2_bits_thr(reg, 0, cmp_parm->qp_inc2_bits_thr);
    vpss_set_out0y_qp_inc1_bits_thr(reg, 0, cmp_parm->qp_inc1_bits_thr);
    vpss_set_out0y_buf_fullness_thr_reg0(reg, 0, cmp_parm->buf_fullness_thr_reg0);
    vpss_set_out0y_buf_fullness_thr_reg1(reg, 0, cmp_parm->buf_fullness_thr_reg1);
    vpss_set_out0y_buf_fullness_thr_reg2(reg, 0, cmp_parm->buf_fullness_thr_reg2);
    vpss_set_out0y_qp_rge_reg0(reg, 0, cmp_parm->qp_rge_reg0);
    vpss_set_out0y_qp_rge_reg1(reg, 0, cmp_parm->qp_rge_reg1);
    vpss_set_out0y_qp_rge_reg2(reg, 0, cmp_parm->qp_rge_reg2);
    vpss_set_out0y_bits_offset_reg0(reg, 0, cmp_parm->bits_offset_reg0);
    vpss_set_out0y_bits_offset_reg1(reg, 0, cmp_parm->bits_offset_reg1);
    vpss_set_out0y_bits_offset_reg2(reg, 0, cmp_parm->bits_offset_reg2);
    vpss_set_out0y_est_err_gain_map(reg, 0, cmp_parm->est_err_gain_map);
    vpss_set_out0y_buffer_size(reg, 0, cmp_parm->buffer_size);
    vpss_set_out0y_buffer_init_bits(reg, 0, cmp_parm->buffer_init_bits);
    vpss_set_out0y_reserve_para0(reg, 0, cmp_parm->reserve_para0);
    vpss_set_out0y_reserve_para1(reg, 0, cmp_parm->reserve_para1);
    vpss_set_out0y_adpqp_thr0(reg, 0, cmp_parm->adpqp_thr0);
    vpss_set_out0y_adpqp_thr1(reg, 0, cmp_parm->adpqp_thr1);
    vpss_set_out0y_smooth_deltabits_thr(reg, 0, cmp_parm->smooth_deltabits_thr);
}

hi_void vpss_ice_set_ccmp_line_out0(vpss_reg_type *reg, ice_v3r2_line_cfg *cmp_parm, hi_bool cmp_en)
{
    vpss_set_out0c_ice_en(reg, 0, cmp_en);
    vpss_set_out0c_bit_depth(reg, 0, cmp_parm->bit_depth);
    vpss_set_out0c_esl_qp(reg, 0, cmp_parm->esl_qp);
    vpss_set_out0c_chroma_en(reg, 0, cmp_parm->chroma_en);
    vpss_set_out0c_is_lossless(reg, 0, cmp_parm->is_lossless);
    vpss_set_out0c_cmp_mode(reg, 0, cmp_parm->cmp_mode);
    vpss_set_out0c_frame_width(reg, 0, 2 * cmp_parm->frame_width - 1); // width * 2
    vpss_set_out0c_frame_height(reg, 0, cmp_parm->frame_height - 1);
    vpss_set_out0c_min_mb_bits(reg, 0, cmp_parm->min_mb_bits);
    vpss_set_out0c_budget_mb_bits_last(reg, 0, cmp_parm->budget_mb_bits_last);
    vpss_set_out0c_budget_mb_bits(reg, 0, cmp_parm->budget_mb_bits);
    vpss_set_out0c_max_mb_qp(reg, 0, cmp_parm->max_mb_qp);
    vpss_set_out0c_noise_pix_num_thr(reg, 0, cmp_parm->noise_pix_num_thr);
    vpss_set_out0c_smooth_status_thr(reg, 0, cmp_parm->smooth_status_thr);
    vpss_set_out0c_diff_thr(reg, 0, cmp_parm->diff_thr);
    vpss_set_out0c_big_grad_thr(reg, 0, cmp_parm->big_grad_thr);
    vpss_set_out0c_qp_dec2_bits_thr(reg, 0, cmp_parm->qp_dec2_bits_thr);
    vpss_set_out0c_qp_dec1_bits_thr(reg, 0, cmp_parm->qp_dec1_bits_thr);
    vpss_set_out0c_qp_inc2_bits_thr(reg, 0, cmp_parm->qp_inc2_bits_thr);
    vpss_set_out0c_qp_inc1_bits_thr(reg, 0, cmp_parm->qp_inc1_bits_thr);
    vpss_set_out0c_buf_fullness_thr_reg0(reg, 0, cmp_parm->buf_fullness_thr_reg0);
    vpss_set_out0c_buf_fullness_thr_reg1(reg, 0, cmp_parm->buf_fullness_thr_reg1);
    vpss_set_out0c_buf_fullness_thr_reg2(reg, 0, cmp_parm->buf_fullness_thr_reg2);
    vpss_set_out0c_qp_rge_reg0(reg, 0, cmp_parm->qp_rge_reg0);
    vpss_set_out0c_qp_rge_reg1(reg, 0, cmp_parm->qp_rge_reg1);
    vpss_set_out0c_qp_rge_reg2(reg, 0, cmp_parm->qp_rge_reg2);
    vpss_set_out0c_bits_offset_reg0(reg, 0, cmp_parm->bits_offset_reg0);
    vpss_set_out0c_bits_offset_reg1(reg, 0, cmp_parm->bits_offset_reg1);
    vpss_set_out0c_bits_offset_reg2(reg, 0, cmp_parm->bits_offset_reg2);
    vpss_set_out0c_est_err_gain_map(reg, 0, cmp_parm->est_err_gain_map);
    vpss_set_out0c_buffer_size(reg, 0, cmp_parm->buffer_size);
    vpss_set_out0c_buffer_init_bits(reg, 0, cmp_parm->buffer_init_bits);
    vpss_set_out0c_reserve_para0(reg, 0, cmp_parm->reserve_para0);
    vpss_set_out0c_reserve_para1(reg, 0, cmp_parm->reserve_para1);
    vpss_set_out0c_adpqp_thr0(reg, 0, cmp_parm->adpqp_thr0);
    vpss_set_out0c_adpqp_thr1(reg, 0, cmp_parm->adpqp_thr1);
    vpss_set_out0c_smooth_deltabits_thr(reg, 0, cmp_parm->smooth_deltabits_thr);
}

hi_void vpss_mac_set_cmp_line_out0(vpss_reg_type *reg, ice_frm_cfg *ice_cfg)
{
    ice_v3_r2_line_input cmp_parm_input_y;
    ice_v3_r2_line_input cmp_parm_input_c;
    ice_v3r2_line_cfg cmp_parm_y;
    ice_v3r2_line_cfg cmp_parm_c;
    memset(&cmp_parm_input_y, 0, sizeof(ice_v3_r2_line_input));
    memset(&cmp_parm_input_c, 0, sizeof(ice_v3_r2_line_input));
    memset(&cmp_parm_y, 0, sizeof(ice_v3r2_line_cfg));
    memset(&cmp_parm_c, 0, sizeof(ice_v3r2_line_cfg));
    cmp_parm_input_y.frame_width = ice_cfg->frame_wth;
    cmp_parm_input_y.frame_height = ice_cfg->frame_hgt;
    cmp_parm_input_y.cmp_mode = ice_cfg->is_raw_en;
    cmp_parm_input_y.bit_depth = ice_cfg->bit_depth;
    cmp_parm_input_y.pix_format = ice_cfg->data_fmt ? 0 : 1;
    cmp_parm_input_y.esl_qp = 0;
    cmp_parm_input_y.is_lossless = !ice_cfg->is_lossy_y;
    cmp_parm_input_y.chroma_en = 0;
    cmp_parm_input_y.comp_ratio_int = ice_cfg->cmp_ratio_y;
    cmp_parm_input_y.rc_type_mode = ice_cfg->cmp_cfg_mode;
    cmp_parm_input_c.frame_width = ice_cfg->frame_wth;
    cmp_parm_input_c.frame_height = ice_cfg->frame_hgt;
    cmp_parm_input_c.cmp_mode = ice_cfg->is_raw_en;
    cmp_parm_input_c.bit_depth = ice_cfg->bit_depth;
    cmp_parm_input_c.pix_format = ice_cfg->data_fmt ? 0 : 1;
    cmp_parm_input_c.esl_qp = 0;
    cmp_parm_input_c.is_lossless = !ice_cfg->is_lossy_c;
    cmp_parm_input_c.chroma_en = 1;
    cmp_parm_input_c.comp_ratio_int = ice_cfg->cmp_ratio_c;
    cmp_parm_input_c.rc_type_mode = ice_cfg->cmp_cfg_mode;
    print_ice_cfg(&cmp_parm_input_y);
    print_ice_cfg(&cmp_parm_input_c);

    if (ice_cfg->cmp_en) {
        ice_v3_r2_line_cfginit(&cmp_parm_y, &cmp_parm_input_y);
        ice_v3_r2_line_cfginit(&cmp_parm_c, &cmp_parm_input_c);
    }

    vpss_ice_set_ycmp_line_out0(reg, &cmp_parm_y, ice_cfg->cmp_en);
    vpss_ice_set_ccmp_line_out0(reg, &cmp_parm_c, ice_cfg->cmp_en);
}

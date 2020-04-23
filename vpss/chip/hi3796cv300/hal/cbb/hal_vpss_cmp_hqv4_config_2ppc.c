/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_cmp_hqv4_config_2ppc.c source file video encoder config
 * Author: zhangjunyu
 * Create: 2016/07/03
 */

#include "hal_vpss_cmp_hqv4_config_2ppc.h"
#include "vpss_comm.h"

// merged yc
void vpss_cmphqv4_compresser_init(cmp_hqv4_parm_t *cmp_parm, cmp_hqv4_para_input_t *para_input_ptr)
{
    int mb_org_bits;
    int min_cfg_bits;
    int mb_num_x;
    int ctl_delta;
    int bit_dep;
    int vbv_qp_map[12][3]; /* 12,3:para */
    int last_mb_pix_num;
    memset(vbv_qp_map, 0, sizeof(vbv_qp_map));
    cmp_parm->cmp_mode = para_input_ptr->cmp_mode;
    cmp_parm->is_lossless = para_input_ptr->is_lossless;
    cmp_parm->frame_width = para_input_ptr->frame_width;
    cmp_parm->frame_height = para_input_ptr->frame_height;
    cmp_parm->frame_bitdepth = para_input_ptr->frame_bitdepth;
    cmp_parm->pixel_format = para_input_ptr->pixel_format;
    cmp_parm->part_cmp_en = para_input_ptr->part_cmp_en;
    cmp_parm->pcmp_start_hpos = para_input_ptr->pcmp_start_hpos;
    cmp_parm->pcmp_end_hpos = para_input_ptr->pcmp_end_hpos;
    cmp_parm->slice_cmp_en = para_input_ptr->slice_cmp_en;
    cmp_parm->rm_debug_en = para_input_ptr->rm_debug_en;

    bit_dep = (cmp_parm->frame_bitdepth == 1) ? 10 : 8; /* 1,10,8:para */
    /* 32,2,32,16:para */
    mb_org_bits = (cmp_parm->pixel_format == 0) ? 32 * 2 * bit_dep : (32 * bit_dep + 16 * bit_dep);
    min_cfg_bits = (cmp_parm->pixel_format == 0) ? 132 : 100; /* 0,132,100:para */
    mb_num_x = (cmp_parm->frame_width + 31) / 32; /* 31,32:para */
    last_mb_pix_num = ((cmp_parm->frame_width & 0x1f) == 0) ? 32 : (cmp_parm->frame_width & 0x1f); /* 32:para */

    /* register parameter init of ratio control */
    if (para_input_ptr->rc_type_mode != 0) {
        osal_printk("can't support this cmp mode %d\n", para_input_ptr->rc_type_mode);
        return;
    }

    /* delta calc */
    cmp_parm->buffer_init_bits = 1024; /* 1024:para */
    cmp_parm->first_mb_adj_bits = (cmp_parm->frame_bitdepth == 0) ? 160 : 200; /* 0,160,200:para */
    /* 2,8192:para */
    ctl_delta = cmphqv4_max(2, (cmp_parm->buffer_init_bits + 8192 + cmp_parm->first_mb_adj_bits * mb_num_x +
        cmp_parm->frame_height * mb_num_x / 0x2 + (cmp_parm->frame_height * mb_num_x *
        mb_org_bits / 127) * 1000 / para_input_ptr->comp_ratio_int) /* 127,1000:para */
        / (cmp_parm->frame_height * mb_num_x));
    ctl_delta = (para_input_ptr->comp_ratio_int < 1200) ? (ctl_delta + 1) : ctl_delta; /* 1200,1:para */
    ctl_delta = (para_input_ptr->comp_ratio_int > 3500) ? (ctl_delta + 1) : ctl_delta; /* 3500,1:para */
    ctl_delta = (last_mb_pix_num != 32) ? (ctl_delta + 1) : ctl_delta; /* 32,1:para */
    ctl_delta = (cmp_parm->frame_bitdepth == 0) ? (ctl_delta + 1) : ctl_delta;
    ctl_delta = ctl_delta + 1;

    cmp_parm->mb_bits = (int)(mb_org_bits * 1000 / para_input_ptr->comp_ratio_int); /* 1000:para */
    cmp_parm->mb_bits = cmphqv4_clip(mb_org_bits, min_cfg_bits, cmp_parm->mb_bits - ctl_delta);
    cmp_parm->smth_thr = 3; /* 3:para */
    cmp_parm->still_thr = 1;
    cmp_parm->big_grad_thr = 40; /* 40:para */
    cmp_parm->smth_pix_num_thr = 6; /* 6:para */
    cmp_parm->still_pix_num_thr = 4; /* 4:para */
    cmp_parm->noise_pix_num_thr = 26; /* 26:para */
    cmp_parm->qp_inc1_bits_thr = (cmp_parm->pixel_format == 0) ? 48 : 44; /* 1,0,48,44:para */
    cmp_parm->qp_inc2_bits_thr = 120; /* 2,120:para */
    cmp_parm->qp_dec1_bits_thr = 32; /* 1,32:para */
    cmp_parm->qp_dec2_bits_thr = 100; /* 2,100:para */
    cmp_parm->min_mb_bits = cmp_parm->mb_bits * 3 / 4; /* 3,4:para */
    cmp_parm->diff_thr = 51; /* 51:para */
    cmp_parm->grph_en = 1;
    /* 0,2,2,3:para */
    cmp_parm->grph_ideal_bits_y = (cmp_parm->pixel_format == 0) ? cmp_parm->mb_bits / 2 : cmp_parm->mb_bits * 2 / 3;
    /* 0,2,3:para */
    cmp_parm->grph_ideal_bits_c = (cmp_parm->pixel_format == 0) ? cmp_parm->mb_bits / 2 : cmp_parm->mb_bits / 3;
    cmp_parm->grph_bits_penalty = (32 << 24) | (0 << 16) | (32 << 8) | 0; /* 32,24,0,16,32,8,0:para */
    cmp_parm->qp_force_en = 0;
    cmp_parm->first_mb_adj_bits = 160; /* 160:para */
    cmp_parm->first_row_adj_bits = 0;
    cmp_parm->first_col_adj_bits = 160; /* 160:para */
    cmp_parm->buffer_init_bits = 1024; /* 1024:para */
    cmp_parm->buf_fullness_thr_reg0 = (71 << 24) | (56 << 16) | (35 << 8) | 18; /* 0,71,24,56,16,35,8,18:para */
    cmp_parm->buf_fullness_thr_reg1 = (116 << 24) | (108 << 16) | (98 << 8) | 85; /* 1,116,24,108,16,98,8,85:para */
    cmp_parm->buf_fullness_thr_reg2 = (0 << 24) | (123 << 16) | (121 << 8) | 119; /* 2,0,24,123,16,121,8,119:para */
    /* 0,3,28,1,24,2,20,0,16,2,12,0,8,2,4,0:para */
    cmp_parm->qp_rge_reg0 = (3 << 28) | (1 << 24) | (2 << 20) | (0 << 16) | (2 << 12) | (0 << 8) | (2 << 4) | 0;
    /* 1,4,28,2,24,4,20,2,16,3,12,1,8,3,4,1:para */
    cmp_parm->qp_rge_reg1 = (4 << 28) | (2 << 24) | (4 << 20) | (2 << 16) | (3 << 12) | (1 << 8) | (3 << 4) | 1;
    /* 2,6,28,3,24,5,20,3,16,5,12,2,8,5,4,2:para */
    cmp_parm->qp_rge_reg2 = (6 << 28) | (3 << 24) | (5 << 20) | (3 << 16) | (5 << 12) | (2 << 8) | (5 << 4) | 2;
    /* 0,4,0,24,2,0,16,0,0,8,2,0:para */
    cmp_parm->bits_offset_reg0 = ((-4 & 0xff) << 24) | ((-2 & 0xff) << 16) | ((0 & 0xff) << 8) | (2 & 0xff);
    /* 1,10,0,24,8,0,16,6,0,8,6,0:para */
    cmp_parm->bits_offset_reg1 = ((-10 & 0xff) << 24) | ((-8 & 0xff) << 16) | ((-6 & 0xff) << 8) | (-6 & 0xff);
    /* 2,12,0,24,10,0,16,10,0,8,10,0:para */
    cmp_parm->bits_offset_reg2 = ((-12 & 0xff) << 24) | ((-10 & 0xff) << 16) | ((-10 & 0xff) << 8) | (-10 & 0xff);
    cmp_parm->est_err_gain_map = (7 << 28) | (7 << 24) | (6 << 20) | (5 << 16) | /* 7,28,7,24,6,20,5,16:para */
                                 (4 << 12) | (4 << 8) | (3 << 4) | 3; /* 4,12,4,8,3,4,3:para */
    cmp_parm->samll_diff_thr = 4; /* 4:para */
    cmp_parm->low_status_thr = 3; /* 3:para */
    cmp_parm->grph_penalty_bit_c = 6; /* 6:para */
    cmp_parm->noise_qp_inc_bits_thr = 20; /* 20:para */
    cmp_parm->noise_status_thr = 7; /* 7:para */
    cmp_parm->pre_noise_mb_thr0 = 1;
    cmp_parm->pre_noise_mb_thr1 = 3; /* 1,3:para */
    cmp_parm->special_qp_dec_bits_thr = 30; /* 30:para */
    cmp_parm->special_status_thr = 10; /* 10:para */
    cmp_parm->smooth_status_thr = 11; /* 11:para */

    if (cmp_parm->frame_bitdepth == 1) {
        cmp_parm->smth_thr = cmp_parm->smth_thr * 4; /* 4:para */
        cmp_parm->still_thr = cmp_parm->still_thr * 4; /* 4:para */
        cmp_parm->big_grad_thr = cmp_parm->big_grad_thr * 4; /* 4:para */
        cmp_parm->qp_inc1_bits_thr = cmp_parm->qp_inc1_bits_thr * 5 / 4; /* 1,1,5,4:para */
        cmp_parm->qp_inc2_bits_thr = cmp_parm->qp_inc2_bits_thr * 5 / 4; /* 2,2,5,4:para */
        cmp_parm->qp_dec1_bits_thr = cmp_parm->qp_dec1_bits_thr * 5 / 4; /* 1,1,5,4:para */
        cmp_parm->qp_dec2_bits_thr = cmp_parm->qp_dec2_bits_thr * 5 / 4; /* 2,2,5,4:para */
        cmp_parm->diff_thr = cmp_parm->diff_thr * 5 / 4; /* 5,4:para */
        cmp_parm->first_mb_adj_bits = cmp_parm->first_mb_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->first_row_adj_bits = cmp_parm->first_row_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->first_col_adj_bits = cmp_parm->first_col_adj_bits * 5 / 4; /* 5,4:para */
        /* 0,4,28,1,24,3,20,1,16,2,12,0,8,2,4,0:para */
        cmp_parm->qp_rge_reg0 = (4 << 28) | (1 << 24) | (3 << 20) | (1 << 16) | (2 << 12) | (0 << 8) | (2 << 4) | 0;
        /* 1,5,28,2,24,4,20,2,16,4,12,2,8,4,4,1:para */
        cmp_parm->qp_rge_reg1 = (5 << 28) | (2 << 24) | (4 << 20) | (2 << 16) | (4 << 12) | (2 << 8) | (4 << 4) | 1;

        if (mb_org_bits * 1000 / cmp_parm->mb_bits < 3000) { /* 1000,3000:para */
            cmp_parm->qp_rge_reg2 = (7 << 28) | (4 << 24) | (7 << 20) | (3 << 16) | /* 2,7,28,4,24,7,20,3,16:para */
                                    (6 << 12) | (3 << 8) | (6 << 4) | 2; /* 6,12,3,8,6,4,2:para */
        } else {
            cmp_parm->qp_rge_reg2 = (8 << 28) | (4 << 24) | (7 << 20) | (3 << 16) | /* 2,8,28,4,24,7,20,3,16:para */
                                    (6 << 12) | (3 << 8) | (6 << 4) | 2; /* 6,12,3,8,6,4,2:para */
        }
    }
}

void vpss_cmphqv4_rand_reg_assert(cmp_hqv4_parm_t *cmp_parm)
{
}

void vpss_cmphqv4_compresser_init_y(cmp_hqv4_alone_parm_t *cmp_parm, cmp_hqv4_alone_para_input_t *para_input_ptr)
{
    int mb_num_x;
    int mb_org_bits;
    int min_cfg_bits;
    int ctl_delta;
    cmp_parm->cmp_mode = para_input_ptr->cmp_mode;
    cmp_parm->is_lossless = para_input_ptr->is_lossless;
    cmp_parm->pixel_format = para_input_ptr->pixel_format;
    cmp_parm->frame_width = para_input_ptr->frame_width;
    cmp_parm->frame_height = para_input_ptr->frame_height;
    cmp_parm->frame_bitdepth = para_input_ptr->frame_bitdepth;
    cmp_parm->part_cmp_en = para_input_ptr->part_cmp_en;
    cmp_parm->pcmp_start_hpos = para_input_ptr->pcmp_start_hpos_y;
    cmp_parm->pcmp_end_hpos = para_input_ptr->pcmp_end_hpos_y;
    cmp_parm->rm_debug_en = para_input_ptr->rm_debug_en;

    mb_num_x = (cmp_parm->frame_width + 31) / 32; /* 31,32:para */
    mb_org_bits = (cmp_parm->frame_bitdepth == 1) ? 320 : 256; /* 1,320,256:para */
    min_cfg_bits = 64; /* 64:para */

    /* register parameter init of ratio control */
    if (para_input_ptr->rc_type_mode != 0) {
        osal_printk("can't support this cmp mode %d\n", para_input_ptr->rc_type_mode);
        return;
    }

    /* delta calc */
    cmp_parm->buffer_init_bits = (cmp_parm->frame_bitdepth == 0) ? 1024 : 2048; /* 0,1024,2048:para */
    cmp_parm->first_mb_adj_bits = (cmp_parm->frame_bitdepth == 0) ? 50 : 62; /* 0,50,62:para */
    /* 4,1,4096,2:para */
    ctl_delta = cmphqv4_max(1, (cmp_parm->buffer_init_bits + 4096 + cmp_parm->first_mb_adj_bits * mb_num_x +
                                cmp_parm->frame_height * mb_num_x / 0x2) / (cmp_parm->frame_height * mb_num_x));
    cmp_parm->mb_bits = (int)(mb_org_bits * 1000 / para_input_ptr->comp_ratio_y_int) - ctl_delta; /* 1000:para */
    cmp_parm->mb_bits = cmphqv4_clip(mb_org_bits, min_cfg_bits, cmp_parm->mb_bits);
    cmp_parm->diff_thr = 25; /* 25:para */
    cmp_parm->smth_thr = 3; /* 3:para */
    cmp_parm->still_thr = 1;
    cmp_parm->big_grad_thr = 16; /* 16:para */
    cmp_parm->smth_pix_num_thr = 6; /* 6:para */
    cmp_parm->still_pix_num_thr = 3; /* 3:para */
    cmp_parm->noise_pix_num_thr = 28; /* 28:para */
    cmp_parm->qp_inc1_bits_thr = 40; /* 1,40:para */
    cmp_parm->qp_inc2_bits_thr = 65; /* 2,65:para */
    cmp_parm->qp_dec1_bits_thr = 40; /* 1,40:para */
    cmp_parm->qp_dec2_bits_thr = 65; /* 2,65:para */
    cmp_parm->min_mb_bits = cmp_parm->mb_bits * 3 / 4; /* 3,4:para */
    cmp_parm->buffer_init_bits = (cmp_parm->frame_bitdepth == 0) ? 1024 : 2048; /* 0,1024,2048:para */
    cmp_parm->qp_force_en = 0;
    cmp_parm->first_mb_adj_bits = 50; /* 50:para */
    cmp_parm->first_row_adj_bits = 0;
    cmp_parm->first_col_adj_bits = 50; /* 50:para */
    cmp_parm->buf_fullness_thr_reg0 = (71 << 24) | (56 << 16) | (35 << 8) | 18; /* 0,71,24,56,16,35,8,18:para */
    cmp_parm->buf_fullness_thr_reg1 = (116 << 24) | (108 << 16) | (98 << 8) | 85; /* 1,116,24,108,16,98,8,85:para */
    cmp_parm->buf_fullness_thr_reg2 = (0 << 24) | (123 << 16) | (121 << 8) | 119; /* 2,0,24,123,16,121,8,119:para */
    /* 0,3,28,1,24,3,20,0,16,2,12,0,8,2,4,0:para */
    cmp_parm->qp_rge_reg0 = (3 << 28) | (1 << 24) | (3 << 20) | (0 << 16) | (2 << 12) | (0 << 8) | (2 << 4) | 0;
    /* 1,4,28,2,24,4,20,2,16,4,12,1,8,3,4,1:para */
    cmp_parm->qp_rge_reg1 = (4 << 28) | (2 << 24) | (4 << 20) | (2 << 16) | (4 << 12) | (1 << 8) | (3 << 4) | 1;
    /* 2,5,28,3,24,5,20,3,16,5,12,2,8,5,4,2:para */
    cmp_parm->qp_rge_reg2 = (5 << 28) | (3 << 24) | (5 << 20) | (3 << 16) | (5 << 12) | (2 << 8) | (5 << 4) | 2;
    /* 0,2,0,24,1,0,16,0,0,8,1,0:para */
    cmp_parm->bits_offset_reg0 = ((-2 & 0xff) << 24) | ((-1 & 0xff) << 16) | ((0 & 0xff) << 8) | (1 & 0xff);
    /* 1,5,0,24,4,0,16,3,0,8,3,0:para */
    cmp_parm->bits_offset_reg1 = ((-5 & 0xff) << 24) | ((-4 & 0xff) << 16) | ((-3 & 0xff) << 8) | (-3 & 0xff);
    /* 2,6,0,24,5,0,16,5,0,8,5,0:para */
    cmp_parm->bits_offset_reg2 = ((-6 & 0xff) << 24) | ((-5 & 0xff) << 16) | ((-5 & 0xff) << 8) | (-5 & 0xff);
    cmp_parm->est_err_gain_map = (7 << 28) | (7 << 24) | (7 << 20) | (7 << 16) | /* 7,28,7,24,7,20,7,16:para */
                                 (6 << 12) | (6 << 8) | (5 << 4) | 5; /* 6,12,6,8,5,4,5:para */
    cmp_parm->smooth_status_thr = 11; /* 11:para */
    cmp_parm->grph_en = 1;
    cmp_parm->grph_ideal_bits = cmp_parm->mb_bits;
    cmp_parm->grph_bits_penalty = (32 << 8) | 0; /* 32,8,0:para */

    if (para_input_ptr->frame_bitdepth == 1) {
        cmp_parm->diff_thr = cmp_parm->diff_thr * 4; /* 4:para */
        cmp_parm->smth_thr = cmp_parm->smth_thr * 4; /* 4:para */
        cmp_parm->still_thr = cmp_parm->still_thr * 4; /* 4:para */
        cmp_parm->big_grad_thr = cmp_parm->big_grad_thr * 4; /* 4:para */
        cmp_parm->qp_inc1_bits_thr = cmp_parm->qp_inc1_bits_thr * 5 / 4; /* 1,1,5,4:para */
        cmp_parm->qp_inc2_bits_thr = cmp_parm->qp_inc2_bits_thr * 5 / 4; /* 2,2,5,4:para */
        cmp_parm->qp_dec1_bits_thr = cmp_parm->qp_dec1_bits_thr * 5 / 4; /* 1,1,5,4:para */
        cmp_parm->qp_dec2_bits_thr = cmp_parm->qp_dec2_bits_thr * 5 / 4; /* 2,2,5,4:para */
        cmp_parm->first_mb_adj_bits = cmp_parm->first_mb_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->first_row_adj_bits = cmp_parm->first_row_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->first_col_adj_bits = cmp_parm->first_col_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->qp_rge_reg0 = (4 << 28) | (1 << 24) | (3 << 20) | (1 << 16) | /* 0,4,28,1,24,3,20,1,16:para */
                                (2 << 12) | (0 << 8) | (2 << 4) | 0; /* 2,12,0,8,2,4,0:para */
        cmp_parm->qp_rge_reg1 = (5 << 28) | (2 << 24) | (4 << 20) | (2 << 16) | /* 1,5,28,2,24,4,20,2,16:para */
                                (4 << 12) | (2 << 8) | (4 << 4) | 1; /* 4,12,2,8,4,4,1:para */

        if (mb_org_bits * 1000 / cmp_parm->mb_bits < 3000) { /* 1000,3000:para */
            cmp_parm->qp_rge_reg2 = (7 << 28) | (4 << 24) | (7 << 20) | (3 << 16) | /* 2,7,28,4,24,7,20,3,16:para */
                                    (6 << 12) | (3 << 8) | (6 << 4) | 2; /* 6,12,3,8,6,4,2:para */
        } else {
            cmp_parm->qp_rge_reg2 = (8 << 28) | (4 << 24) | (7 << 20) | (3 << 16) | /* 2,8,28,4,24,7,20,3,16:para */
                                    (6 << 12) | (3 << 8) | (6 << 4) | 2; /* 6,12,3,8,6,4,2:para */
        }
    }
}

void vpss_cmphqv4_compresser_init_c(cmp_hqv4_alone_parm_t *cmp_parm, cmp_hqv4_alone_para_input_t *para_input_ptr)
{
    int mb_num_x;
    int mb_org_bits;
    int min_cfg_bits;
    int ctl_delta;
    int frame_height_c;
    cmp_parm->cmp_mode = para_input_ptr->cmp_mode;
    cmp_parm->is_lossless = para_input_ptr->is_lossless;
    cmp_parm->pixel_format = para_input_ptr->pixel_format;
    cmp_parm->frame_width = para_input_ptr->frame_width;
    cmp_parm->frame_height = para_input_ptr->frame_height;
    cmp_parm->frame_bitdepth = para_input_ptr->frame_bitdepth;
    cmp_parm->part_cmp_en = para_input_ptr->part_cmp_en;
    cmp_parm->pcmp_start_hpos = para_input_ptr->pcmp_start_hpos_c;
    cmp_parm->pcmp_end_hpos = para_input_ptr->pcmp_end_hpos_c;
    cmp_parm->rm_debug_en = para_input_ptr->rm_debug_en;

    mb_num_x = (cmp_parm->frame_width + 31) / 32; /* 31,32:para */
    mb_org_bits = (cmp_parm->frame_bitdepth == 1) ? 320 : 256; /* 1,320,256:para */
    min_cfg_bits = 64; /* 64:para */
    frame_height_c = (cmp_parm->pixel_format == 0) ? cmp_parm->frame_height : cmp_parm->frame_height / 2; /* 2:para */

    /* register parameter init of ratio control */
    if (para_input_ptr->rc_type_mode != 0) {
        osal_printk("can't support this cmp mode %d\n", para_input_ptr->rc_type_mode);
        return;
    }

    /* delta calc */
    cmp_parm->buffer_init_bits = (cmp_parm->frame_bitdepth == 0) ? 1024 : 2048; /* 0,1024,2048:para */
    cmp_parm->first_mb_adj_bits = (cmp_parm->frame_bitdepth == 0) ? 50 : 62; /* 0,50,62:para */
    /* 4,1,4096:para */
    ctl_delta = cmphqv4_max(1, (cmp_parm->buffer_init_bits + 4096 + cmp_parm->first_mb_adj_bits * mb_num_x +
                                frame_height_c * mb_num_x / 2) / (frame_height_c * mb_num_x)); /* 2:para */
    cmp_parm->mb_bits = (int)(mb_org_bits * 1000 / para_input_ptr->comp_ratio_c_int) - ctl_delta; /* 1000:para */
    cmp_parm->mb_bits = cmphqv4_clip(mb_org_bits, min_cfg_bits, cmp_parm->mb_bits);
    cmp_parm->diff_thr = 25; /* 25:para */
    cmp_parm->smth_thr = 3; /* 3:para */
    cmp_parm->still_thr = 1;
    cmp_parm->big_grad_thr = 16; /* 16:para */
    cmp_parm->smth_pix_num_thr = 4; /* 4:para */
    cmp_parm->still_pix_num_thr = 2; /* 2:para */
    cmp_parm->noise_pix_num_thr = 14; /* 14:para */
    cmp_parm->qp_inc1_bits_thr = 40; /* 1,40:para */
    cmp_parm->qp_inc2_bits_thr = 65; /* 2,65:para */
    cmp_parm->qp_dec1_bits_thr = 40; /* 1,40:para */
    cmp_parm->qp_dec2_bits_thr = 65; /* 2,65:para */
    cmp_parm->min_mb_bits = cmp_parm->mb_bits * 3 / 4; /* 3,4:para */
    cmp_parm->buffer_init_bits = (cmp_parm->frame_bitdepth == 0) ? 1024 : 2048; /* 0,1024,2048:para */
    cmp_parm->qp_force_en = 0;
    cmp_parm->first_mb_adj_bits = 50; /* 50:para */
    cmp_parm->first_row_adj_bits = 0;
    cmp_parm->first_col_adj_bits = 50; /* 50:para */
    cmp_parm->buf_fullness_thr_reg0 = (71 << 24) | (56 << 16) | (35 << 8) | 18; /* 0,71,24,56,16,35,8,18:para */
    cmp_parm->buf_fullness_thr_reg1 = (116 << 24) | (108 << 16) | (98 << 8) | 85; /* 1,116,24,108,16,98,8,85:para */
    cmp_parm->buf_fullness_thr_reg2 = (0 << 24) | (123 << 16) | (121 << 8) | 119; /* 2,0,24,123,16,121,8,119:para */
    /* 0,4,28,1,24,3,20,1,16,2,12,0,8,2,4,0:para */
    cmp_parm->qp_rge_reg0 = (4 << 28) | (1 << 24) | (3 << 20) | (1 << 16) | (2 << 12) | (0 << 8) | (2 << 4) | 0;
    /* 1,5,28,2,24,4,20,2,16,4,12,2,8,4,4,1:para */
    cmp_parm->qp_rge_reg1 = (5 << 28) | (2 << 24) | (4 << 20) | (2 << 16) | (4 << 12) | (2 << 8) | (4 << 4) | 1;
    /* 2,6,28,4,24,6,20,3,16,6,12,3,8,6,4,2:para */
    cmp_parm->qp_rge_reg2 = (6 << 28) | (4 << 24) | (6 << 20) | (3 << 16) | (6 << 12) | (3 << 8) | (6 << 4) | 2;
    /* 0,2,0,24,1,0,16,0,0,8,1,0:para */
    cmp_parm->bits_offset_reg0 = ((-2 & 0xff) << 24) | ((-1 & 0xff) << 16) | ((0 & 0xff) << 8) | (1 & 0xff);
    /* 1,5,0,24,4,0,16,3,0,8,3,0:para */
    cmp_parm->bits_offset_reg1 = ((-5 & 0xff) << 24) | ((-4 & 0xff) << 16) | ((-3 & 0xff) << 8) | (-3 & 0xff);
    /* 2,6,0,24,5,0,16,5,0,8,5,0:para */
    cmp_parm->bits_offset_reg2 = ((-6 & 0xff) << 24) | ((-5 & 0xff) << 16) | ((-5 & 0xff) << 8) | (-5 & 0xff);
    cmp_parm->est_err_gain_map = (7 << 28) | (7 << 24) | (7 << 20) | (7 << 16) | /* 7,28,7,24,7,20,7,16:para */
                                 (6 << 12) | (6 << 8) | (5 << 4) | 5; /* 6,12,6,8,5,4,5:para */
    cmp_parm->smooth_status_thr = 11; /* 11:para */
    cmp_parm->grph_en = 1;
    cmp_parm->grph_ideal_bits = cmp_parm->mb_bits;
    cmp_parm->grph_bits_penalty = (32 << 8) | 0; /* 32,8,0:para */

    if (para_input_ptr->frame_bitdepth == 1) {
        cmp_parm->diff_thr = cmp_parm->diff_thr * 4; /* 4:para */
        cmp_parm->smth_thr = cmp_parm->smth_thr * 4; /* 4:para */
        cmp_parm->still_thr = cmp_parm->still_thr * 4; /* 4:para */
        cmp_parm->big_grad_thr = cmp_parm->big_grad_thr * 4; /* 4:para */
        cmp_parm->qp_inc1_bits_thr = cmp_parm->qp_inc1_bits_thr * 5 / 4; /* 1,1,5,4:para */
        cmp_parm->qp_inc2_bits_thr = cmp_parm->qp_inc2_bits_thr * 5 / 4; /* 2,2,5,4:para */
        cmp_parm->qp_dec1_bits_thr = cmp_parm->qp_dec1_bits_thr * 5 / 4; /* 1,1,5,4:para */
        cmp_parm->qp_dec2_bits_thr = cmp_parm->qp_dec2_bits_thr * 5 / 4; /* 2,2,5,4:para */
        cmp_parm->first_mb_adj_bits = cmp_parm->first_mb_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->first_row_adj_bits = cmp_parm->first_row_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->first_col_adj_bits = cmp_parm->first_col_adj_bits * 5 / 4; /* 5,4:para */
        cmp_parm->qp_rge_reg0 = (4 << 28) | (1 << 24) | (3 << 20) | (1 << 16) | /* 0,4,28,1,24,3,20,1,16:para */
                                (2 << 12) | (0 << 8) | (2 << 4) | 0; /* 2,12,0,8,2,4,0:para */
        cmp_parm->qp_rge_reg1 = (5 << 28) | (2 << 24) | (4 << 20) | (2 << 16) | /* 1,5,28,2,24,4,20,2,16:para */
                                (4 << 12) | (2 << 8) | (4 << 4) | 1; /* 4,12,2,8,4,4,1:para */

        if (mb_org_bits * 1000 / cmp_parm->mb_bits < 3000) { /* 1000,3000:para */
            cmp_parm->qp_rge_reg2 = (7 << 28) | (4 << 24) | (7 << 20) | (3 << 16) | /* 2,7,28,4,24,7,20,3,16:para */
                                    (6 << 12) | (3 << 8) | (6 << 4) | 2; /* 6,12,3,8,6,4,2:para */
        } else {
            cmp_parm->qp_rge_reg2 = (8 << 28) | (4 << 24) | (7 << 20) | (3 << 16) | /* 2,8,28,4,24,7,20,3,16:para */
                                    (6 << 12) | (3 << 8) | (6 << 4) | 2; /* 6,12,3,8,6,4,2:para */
        }
    }
}

void vpss_cmphqv4_decompress_init_alone(dcmp_hqv4_alone_parm_t *dcmp_parm, cmp_hqv4_alone_parm_t *cmp_parm,
                                        cmp_hqv4_alone_para_input_t *para_input_ptr)
{
    dcmp_parm->pixel_format = cmp_parm->pixel_format;
    dcmp_parm->cmp_mode = cmp_parm->cmp_mode;
    dcmp_parm->is_lossless = (cmp_parm->is_lossless || cmp_parm->part_cmp_en);
    dcmp_parm->frame_width = cmp_parm->frame_width;
    dcmp_parm->frame_height = cmp_parm->frame_height;
    dcmp_parm->frame_bitdepth = cmp_parm->frame_bitdepth;
    dcmp_parm->bd_det_en = ((para_input_ptr->rc_type_mode != 0) || (dcmp_parm->cmp_mode == 1) ||
                            (dcmp_parm->is_lossless == 1) ||
                            (cmp_parm->frame_width * dcmp_parm->frame_height < 52480)) ? 0 : 1; /* 52480,0,1:para */
    dcmp_parm->buffer_init_bits = cmp_parm->buffer_init_bits;
    dcmp_parm->buffer_fullness_thr = 4400; /* 4400:para */
    dcmp_parm->budget_mb_bits = cmp_parm->mb_bits;
    dcmp_parm->rm_debug_en = cmp_parm->rm_debug_en;
}

void vpss_cmphqv4_rand_reg_assert_alone(cmp_hqv4_alone_parm_t *cmp_parm)
{
}

void vpss_cmphqv4_rand_reg_assert_alone_yc(cmp_hqv4_alone_parm_t *cmp_parm_y, cmp_hqv4_alone_parm_t *cmp_parm_c)
{
    /* assert p_cmp_parm_y */
    vpss_cmphqv4_rand_reg_assert_alone(cmp_parm_y);
    /* assert p_cmp_parm_c */
    vpss_cmphqv4_rand_reg_assert_alone(cmp_parm_c);

    vpss_assert_not_ret(cmp_parm_y->pixel_format == cmp_parm_c->pixel_format);
    vpss_assert_not_ret(cmp_parm_y->cmp_mode == cmp_parm_c->cmp_mode);
    vpss_assert_not_ret(cmp_parm_y->is_lossless == cmp_parm_c->is_lossless);
    vpss_assert_not_ret(cmp_parm_y->frame_width == cmp_parm_c->frame_width);
    vpss_assert_not_ret(cmp_parm_y->frame_height == cmp_parm_c->frame_height);
    vpss_assert_not_ret(cmp_parm_y->frame_bitdepth == cmp_parm_c->frame_bitdepth);
    vpss_assert_not_ret(cmp_parm_y->part_cmp_en == cmp_parm_c->part_cmp_en);
}

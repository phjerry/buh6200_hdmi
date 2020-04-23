/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: vpss common
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "ice_v3r2_line_input.h"
#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if _RM_EN_

#define cmpseg_clip(max, min, x) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))
#define cmpseg_max(a, b)         ((a) > (b) ? (a) : (b))
#define cmpseg_min(a, b)         ((a) > (b) ? (b) : (a))

enum ice_v3_r2_line_cal_type {
    GETMAX = 0,
    GETMIN,
    GETRAND
};

int ice_v3r2_line_num_gen(int bit_width, int sign, int type)
{
#ifdef TOBEMODIFY
    int val;
    int max_val;
    int tmp;

    if (sign == 0) {  // 0: signed    1: unsigned
        if (type == GETMAX) {
            val = (1 << (bit_width - 1)) - 1;
        } else if (type == GETMIN) {
            val = -(1 << (bit_width - 1));
        } else {
            max_val = (1 << bit_width);
            tmp = (max_val) >> 1;
            val = (rand() % max_val) - tmp;
        }
    } else {
        if (type == GETMAX) {
            val = (1 << bit_width) - 1;
        } else if (type == GETMIN) {
            val = 0;
        } else {
            max_val = (1 << bit_width);

            if (bit_width != 1) {
                val = rand() % max_val;
            } else {
                val = rand() & 1;
            }
        }
    }

    return val;
#endif
    return 0;
}

int ice_v3_r2_line_my_rand_clip(int max_val, int min_val, int i_val)
{
    int o_val = 0;
    int abs_i_val = (i_val > 0) ? i_val : (-i_val);

    if ((i_val <= max_val) && (i_val >= min_val)) {
        o_val = i_val;
    } else if (max_val > min_val) {
        if ((max_val - min_val) == 1) {
            o_val = (i_val & 0x1) ? min_val : max_val;
        } else {
            o_val = abs_i_val % (max_val - min_val) + min_val;
        }
    } else if (max_val == min_val) {
        o_val = max_val;
    } else {
        vpss_error("input error: min_val > max_val!\n");
        vpss_assert_ret(max_val >= min_val);
    }

    return o_val;
}

int ice_v3_r2_line_cfginit(ice_v3r2_line_cfg *rm_cfg, ice_v3_r2_line_input *input)
{
    // bit_depth                0  1   2   3
    int mb_ori_bits;
    int mb_safe_bit;
    int max_cfg_qp = 0;
    int bit_depth;
    int buf_fullness_thr[12] = { 0 }; /* 12,0:para */
    int vbv_qp_map[12][3] = { { 0 } }; /* 12,3,0:para */
    int gain_map[8] = { 0 }; /* 8,0:para */
    int smooth_ana[8] = { 0 }; /* 8,0:para */
    int adp_qp_map[8] = { 0 }; /* 8,0:para */
    int still_thr = 0;
    int still_area_thr = 0;
    int bit_diff0 = 0;
    int bit_diff1 = 0;
    int inc1_thr = 0;
    int inc2_thr = 0;
    int dec1_thr = 0;
    int dec2_thr = 0;
    int mb_num_x, mb_num_y, delta, last_mb_wth;
    rm_cfg->chroma_en = input->chroma_en;
    rm_cfg->is_lossless = (input->cmp_mode == 2) ? 1 : input->is_lossless; /* 2,1:para */
    rm_cfg->cmp_mode = input->cmp_mode;
    rm_cfg->bit_depth = input->bit_depth;
    rm_cfg->esl_qp = ice_min(input->esl_qp, 6); /* 6:para */

    if (rm_cfg->chroma_en == 0) {
        rm_cfg->frame_width = input->frame_width;
        rm_cfg->frame_height = input->frame_height;
    } else {
        rm_cfg->frame_width = input->frame_width / 2; /* 2:para */
        rm_cfg->frame_height = input->pix_format == 0 ? input->frame_height / 2 : input->frame_height; /* 0,2:para */
    }

    rm_cfg->rm_debug_en = input->rc_type_mode ? 0 : 1;
    bit_depth = rm_cfg->bit_depth * 2 + 8; /* 2,8:para */
    last_mb_wth = (rm_cfg->frame_width % 32) ? (rm_cfg->frame_width % 32) : 32; /* 32,32,32:para */
    mb_ori_bits = bit_depth << 5; /* 5:para */
    mb_safe_bit = bit_depth << 1;
    mb_num_x = (rm_cfg->frame_width + 31) >> 5; /* 31,5:para */
    mb_num_y = rm_cfg->frame_height;
    mb_num_x = (rm_cfg->frame_width + MB_WTH - 1) / MB_WTH;
    mb_num_y = rm_cfg->chroma_en ? (rm_cfg->frame_height / 2) : rm_cfg->frame_height; /* 2:para */
    last_mb_wth = (rm_cfg->frame_width % MB_WTH) ? (rm_cfg->frame_width % MB_WTH) : MB_WTH;
    rm_cfg->budget_mb_bits = MB_WTH * bit_depth * 1000 / input->comp_ratio_int; /* 1000:para */
    rm_cfg->budget_mb_bits = input->is_lossless ? MB_WTH * bit_depth : rm_cfg->budget_mb_bits;
    rm_cfg->min_mb_bits = rm_cfg->budget_mb_bits * 3 / 4; /* 3,4:para */
    rm_cfg->max_mb_qp = rm_cfg->is_lossless ? 0 : bit_depth - (rm_cfg->budget_mb_bits - 32) / MB_WTH; /* 0,32:para */
    vpss_assert_ret(rm_cfg->max_mb_qp < 8); /* 8:para */

    if (input->rc_type_mode == 0) {
        rm_cfg->buffer_size = 4096; /* 4096:para */
        rm_cfg->buffer_init_bits = 3896; /* 3896:para */
        delta = ice_max(1, (rm_cfg->buffer_size - rm_cfg->buffer_init_bits + mb_num_x / 2) / mb_num_x); /* 1,2:para */
        if (delta > 1) {
            rm_cfg->rm_debug_en = 0;
        }

        rm_cfg->budget_mb_bits = rm_cfg->is_lossless ? rm_cfg->budget_mb_bits : clip(rm_cfg->budget_mb_bits - delta);
        rm_cfg->budget_mb_bits_last = rm_cfg->budget_mb_bits * last_mb_wth / MB_WTH;
        rm_cfg->big_grad_thr = 20; /* 20:para */
        rm_cfg->diff_thr = 30; /* 30:para */
        rm_cfg->noise_pix_num_thr = 20; /* 20:para */
        rm_cfg->qp_inc1_bits_thr = 32; /* 1,32:para */
        rm_cfg->qp_inc2_bits_thr = 64; /* 2,64:para */
        rm_cfg->qp_dec1_bits_thr = 30; /* 1,30:para */
        rm_cfg->qp_dec2_bits_thr = 50; /* 2,50:para */
        rm_cfg->buf_fullness_thr_reg0 = (65 << 24) | (56 << 16) | (35 << 8) | 18; /* 0,65,24,56,16,35,8,18:para */
        rm_cfg->buf_fullness_thr_reg1 = (90 << 24) | (80 << 16) | (75 << 8) | 70; /* 1,90,24,80,16,75,8,70:para */
        rm_cfg->buf_fullness_thr_reg2 = (0 << 24) | (122 << 16) | (110 << 8) | 100; /* 0,24,122,16,110,8,100:para */
        /* 0,0,0,24,0,0,16,0,0,8,0,0:para */
        rm_cfg->bits_offset_reg0 = ((0 & 0xff) << 24) | ((0 & 0xff) << 16) | ((0 & 0xff) << 8) | (0 & 0xff);
        /* 1,1,0,24,0,0,16,0,0,8,0,0:para */
        rm_cfg->bits_offset_reg1 = ((1 & 0xff) << 24) | ((0 & 0xff) << 16) | ((0 & 0xff) << 8) | (0 & 0xff);
        /* 2,2,0,24,0,0,16,1,0,8,1,0:para */
        rm_cfg->bits_offset_reg2 = ((-2 & 0xff) << 24) | ((-0 & 0xff) << 16) | ((1 & 0xff) << 8) | (1 & 0xff);
        /* 7,28,7,24,6,20,5,16,4,12,4,8,3,4,3:para */
        rm_cfg->est_err_gain_map = (7 << 28) | (7 << 24) | (6 << 20) | (5 << 16) | (4 << 12) | (4 << 8) | (3 << 4) | 3;
        rm_cfg->smooth_status_thr = 10; /* 10:para */
        /* 0,3,0,24,100,0,16,0,0,8,12,0:para */
        rm_cfg->reserve_para0 = ((3 & 0xff) << 24) | ((100 & 0xff) << 16) | ((0 & 0xff) << 8) | (12 & 0xff);
        /* 1,15,0,24,0,0,16,64,0,8,32,0:para */
        rm_cfg->reserve_para1 = ((15 & 0xff) << 24) | ((0 & 0xff) << 16) | ((64 & 0xff) << 8) | (32 & 0xff);
        rm_cfg->smooth_deltabits_thr = 85; /* 85:para */
        /* 0,3,0,24,3,0,16,10,0,8,10,0:para */
        rm_cfg->adpqp_thr0 = ((3 & 0xff) << 24) | ((3 & 0xff) << 16) | ((10 & 0xff) << 8) | (10 & 0xff);
        /* 1,5,0,24,15,0,16,0,0,8,0,0:para */
        rm_cfg->adpqp_thr1 = ((5 & 0xff) << 24) | ((15 & 0xff) << 16) | ((0 & 0xff) << 8) | (0 & 0xff);

        if (rm_cfg->max_mb_qp > 4) { /* 4:para */
            rm_cfg->qp_rge_reg0 = ((rm_cfg->max_mb_qp - 3) << 28) | (0 << 24) | /* 0,3,28,0,24:para */
                                  ((rm_cfg->max_mb_qp - 3) << 20) | /* 3,20:para */
                                  (0 << 16) | ((rm_cfg->max_mb_qp - 3) << 12) | /* 0,16,3,12:para */
                                  (0 << 8) | ((rm_cfg->max_mb_qp - 3) << 4) | 0; /* 0,8,3,4,0:para */
            rm_cfg->qp_rge_reg1 = ((rm_cfg->max_mb_qp - 1) << 28) | (1 << 24) | /* 1,1,28,1,24:para */
                                  ((rm_cfg->max_mb_qp - 2) << 20) | /* 2,20:para */
                                  (1 << 16) | ((rm_cfg->max_mb_qp - 2) << 12) | /* 1,16,2,12:para */
                                  (1 << 8) | ((rm_cfg->max_mb_qp - 3) << 4) | 1; /* 1,8,3,4,1:para */
            rm_cfg->qp_rge_reg2 = (rm_cfg->max_mb_qp << 28) | (2 << 24) | /* 2,28,2,24:para */
                                  ((rm_cfg->max_mb_qp - 0) << 20) | (2 << 16) | /* 0,20,2,16:para */
                                  ((rm_cfg->max_mb_qp - 1) << 12) | (2 << 8) | /* 1,12,2,8:para */
                                  ((rm_cfg->max_mb_qp - 1) << 4) | 2; /* 1,4,2:para */
        } else {  // 3,2,1
            rm_cfg->qp_rge_reg0 = (clip(rm_cfg->max_mb_qp - 3) << 28) | (0 << 24) | /* 0,3,28,0,24:para */
                                  (clip(rm_cfg->max_mb_qp - 3) << 20) | /* 3,20:para */
                                  (0 << 16) | (clip(rm_cfg->max_mb_qp - 3) << 12) | /* 0,16,3,12:para */
                                  (0 << 8) | (clip(rm_cfg->max_mb_qp - 3) << 4) | 0; /* 0,8,3,4,0:para */
            rm_cfg->qp_rge_reg1 = (clip(rm_cfg->max_mb_qp - 2) << 28) | (0 << 24) | /* 1,2,28,0,24:para */
                                  (clip(rm_cfg->max_mb_qp - 2) << 20) | /* 2,20:para */
                                  (0 << 16) | (clip(rm_cfg->max_mb_qp - 2) << 12) | /* 0,16,2,12:para */
                                  (0 << 8) | (clip(rm_cfg->max_mb_qp - 3) << 4) | 0; /* 0,8,3,4,0:para */
            rm_cfg->qp_rge_reg2 = (rm_cfg->max_mb_qp << 28) | (1 << 24) | /* 2,28,1,24:para */
                                  (clip(rm_cfg->max_mb_qp - 1) << 20) | (1 << 16) | /* 1,20,1,16:para */
                                  (clip(rm_cfg->max_mb_qp - 1) << 12) | (1 << 8) | /* 1,12,1,8:para */
                                  (clip(rm_cfg->max_mb_qp - 1) << 4) | 1; /* 1,4,1:para */
        }
    } else if (input->rc_type_mode == 1) {  // rand
        max_cfg_qp = rm_cfg->max_mb_qp;
        rm_cfg->big_grad_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,2,8,1:para */
        rm_cfg->diff_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,2,8,1:para */
        rm_cfg->noise_pix_num_thr = ice_v3r2_line_num_gen(6, 1, GETRAND); /* 3,2,6,1:para */
        rm_cfg->qp_inc1_bits_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        rm_cfg->qp_inc2_bits_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        rm_cfg->qp_dec1_bits_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        rm_cfg->qp_dec2_bits_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        rm_cfg->buffer_size = ice_v3r2_line_num_gen(16, 1, GETRAND); /* 3,2,16,1:para */
        rm_cfg->buffer_init_bits = ice_min(rm_cfg->buffer_size, ice_v3r2_line_num_gen(16, 1, GETRAND)); /* 16,1:para */
        rm_cfg->smooth_status_thr = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 3,2,4,1:para */
        rm_cfg->smooth_deltabits_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,2,8,1:para */
        still_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,2,8,1:para */
        still_area_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,2,8,1:para */
        bit_diff0 = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 0,3,2,8,1:para */
        bit_diff1 = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        inc1_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        inc2_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        dec1_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        dec2_thr = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        buf_fullness_thr[0] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 0,3,2,8,1:para */
        buf_fullness_thr[1] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        buf_fullness_thr[2] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        buf_fullness_thr[3] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,3,2,8,1:para */
        buf_fullness_thr[4] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 4,3,2,8,1:para */
        buf_fullness_thr[5] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 5,3,2,8,1:para */
        buf_fullness_thr[6] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 6,3,2,8,1:para */
        buf_fullness_thr[7] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 7,3,2,8,1:para */
        buf_fullness_thr[8] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 8,3,2,8,1:para */
        buf_fullness_thr[9] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 9,3,2,8,1:para */
        buf_fullness_thr[10] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 10,3,2,8,1:para */
        buf_fullness_thr[11] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 11,3,2,8,1:para */
        vbv_qp_map[0][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 0,0,3,2,4,1:para */
        vbv_qp_map[0][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 0,1,3,2,4,1:para */
        vbv_qp_map[1][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 1,0,3,2,4,1:para */
        vbv_qp_map[1][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 1,1,3,2,4,1:para */
        vbv_qp_map[2][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 2,0,3,2,4,1:para */
        vbv_qp_map[2][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 2,1,3,2,4,1:para */
        vbv_qp_map[3][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 3,0,3,2,4,1:para */
        vbv_qp_map[3][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 3,1,3,2,4,1:para */
        vbv_qp_map[4][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 4,0,3,2,4,1:para */
        vbv_qp_map[4][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 4,1,3,2,4,1:para */
        vbv_qp_map[5][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 5,0,3,2,4,1:para */
        vbv_qp_map[5][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 5,1,3,2,4,1:para */
        vbv_qp_map[6][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 6,0,3,2,4,1:para */
        vbv_qp_map[6][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 6,1,3,2,4,1:para */
        vbv_qp_map[7][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 7,0,3,2,4,1:para */
        vbv_qp_map[7][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 7,1,3,2,4,1:para */
        vbv_qp_map[8][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 8,0,3,2,4,1:para */
        vbv_qp_map[8][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 8,1,3,2,4,1:para */
        vbv_qp_map[9][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 9,0,3,2,4,1:para */
        vbv_qp_map[9][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 9,1,3,2,4,1:para */
        vbv_qp_map[10][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 10,0,3,2,4,1:para */
        vbv_qp_map[10][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 10,1,3,2,4,1:para */
        vbv_qp_map[11][0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 11,0,3,2,4,1:para */
        vbv_qp_map[11][1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 11,1,3,2,4,1:para */
        vbv_qp_map[0][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 0,2,3,2,8,0:para */
        vbv_qp_map[1][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 1,2,3,2,8,0:para */
        vbv_qp_map[2][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 2,2,3,2,8,0:para */
        vbv_qp_map[3][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 3,2,3,2,8,0:para */
        vbv_qp_map[4][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 4,2,3,2,8,0:para */
        vbv_qp_map[5][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 5,2,3,2,8,0:para */
        vbv_qp_map[6][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 6,2,3,2,8,0:para */
        vbv_qp_map[7][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 7,2,3,2,8,0:para */
        vbv_qp_map[8][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 8,2,3,2,8,0:para */
        vbv_qp_map[9][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 9,2,3,2,8,0:para */
        vbv_qp_map[10][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 10,2,3,2,8,0:para */
        vbv_qp_map[11][2] = ice_v3r2_line_num_gen(8, 0, GETRAND); /* 11,2,3,2,8,0:para */
        gain_map[0] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 0,3,2,4,1:para */
        gain_map[1] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 1,3,2,4,1:para */
        gain_map[2] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 2,3,2,4,1:para */
        gain_map[3] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 3,3,2,4,1:para */
        gain_map[4] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 4,3,2,4,1:para */
        gain_map[5] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 5,3,2,4,1:para */
        gain_map[6] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 6,3,2,4,1:para */
        gain_map[7] = ice_v3r2_line_num_gen(4, 1, GETRAND); /* 7,3,2,4,1:para */
        smooth_ana[0] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 0,3,2,8,1:para */
        smooth_ana[1] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        smooth_ana[2] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        smooth_ana[3] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,3,2,8,1:para */
        smooth_ana[4] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 4,3,2,8,1:para */
        smooth_ana[5] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 5,3,2,8,1:para */
        smooth_ana[6] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 6,3,2,8,1:para */
        smooth_ana[7] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 7,3,2,8,1:para */
        adp_qp_map[0] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 0,3,2,8,1:para */
        adp_qp_map[1] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 1,3,2,8,1:para */
        adp_qp_map[2] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 2,3,2,8,1:para */
        adp_qp_map[3] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 3,3,2,8,1:para */
        adp_qp_map[4] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 4,3,2,8,1:para */
        adp_qp_map[5] = ice_v3r2_line_num_gen(8, 1, GETRAND); /* 5,3,2,8,1:para */
        adp_qp_map[6] = ice_v3r2_line_num_gen(2, 1, GETRAND); /* 6,3,2,2,1:para */
        adp_qp_map[7] = ice_v3r2_line_num_gen(2, 1, GETRAND); /* 7,3,2,2,1:para */
    } else if (input->rc_type_mode == 2) {  // max /* 2:para */
        max_cfg_qp = rm_cfg->max_mb_qp;
        rm_cfg->big_grad_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        rm_cfg->diff_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        rm_cfg->noise_pix_num_thr = ice_v3r2_line_num_gen(6, 1, GETMAX); /* 3,2,6,1:para */
        rm_cfg->qp_inc1_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        rm_cfg->qp_inc2_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        rm_cfg->qp_dec1_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        rm_cfg->qp_dec2_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        rm_cfg->buffer_size = ice_v3r2_line_num_gen(16, 1, GETMAX); /* 3,2,16,1:para */
        rm_cfg->buffer_init_bits = ice_min(rm_cfg->buffer_size, ice_v3r2_line_num_gen(16, 1, GETMAX)); /* 16,1:para */
        rm_cfg->smooth_status_thr = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        rm_cfg->smooth_deltabits_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        still_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        still_area_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        bit_diff0 = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 0,3,2,8,1:para */
        bit_diff1 = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        inc1_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        inc2_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        dec1_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        dec2_thr = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        buf_fullness_thr[0] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 0,3,2,8,1:para */
        buf_fullness_thr[1] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        buf_fullness_thr[2] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        buf_fullness_thr[3] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,3,2,8,1:para */
        buf_fullness_thr[4] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 4,3,2,8,1:para */
        buf_fullness_thr[5] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 5,3,2,8,1:para */
        buf_fullness_thr[6] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 6,3,2,8,1:para */
        buf_fullness_thr[7] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 7,3,2,8,1:para */
        buf_fullness_thr[8] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 8,3,2,8,1:para */
        buf_fullness_thr[9] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 9,3,2,8,1:para */
        buf_fullness_thr[10] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 10,3,2,8,1:para */
        buf_fullness_thr[11] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 11,3,2,8,1:para */
        vbv_qp_map[0][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 0,0,3,2,4,1:para */
        vbv_qp_map[0][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 0,1,3,2,4,1:para */
        vbv_qp_map[1][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 1,0,3,2,4,1:para */
        vbv_qp_map[1][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 1,1,3,2,4,1:para */
        vbv_qp_map[2][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 2,0,3,2,4,1:para */
        vbv_qp_map[2][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 2,1,3,2,4,1:para */
        vbv_qp_map[3][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,0,3,2,4,1:para */
        vbv_qp_map[3][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,1,3,2,4,1:para */
        vbv_qp_map[4][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 4,0,3,2,4,1:para */
        vbv_qp_map[4][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 4,1,3,2,4,1:para */
        vbv_qp_map[5][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 5,0,3,2,4,1:para */
        vbv_qp_map[5][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 5,1,3,2,4,1:para */
        vbv_qp_map[6][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 6,0,3,2,4,1:para */
        vbv_qp_map[6][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 6,1,3,2,4,1:para */
        vbv_qp_map[7][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 7,0,3,2,4,1:para */
        vbv_qp_map[7][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 7,1,3,2,4,1:para */
        vbv_qp_map[8][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 8,0,3,2,4,1:para */
        vbv_qp_map[8][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 8,1,3,2,4,1:para */
        vbv_qp_map[9][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 9,0,3,2,4,1:para */
        vbv_qp_map[9][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 9,1,3,2,4,1:para */
        vbv_qp_map[10][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 10,0,3,2,4,1:para */
        vbv_qp_map[10][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 10,1,3,2,4,1:para */
        vbv_qp_map[11][0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 11,0,3,2,4,1:para */
        vbv_qp_map[11][1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 11,1,3,2,4,1:para */
        vbv_qp_map[0][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 0,2,3,2,8,0:para */
        vbv_qp_map[1][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 1,2,3,2,8,0:para */
        vbv_qp_map[2][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 2,2,3,2,8,0:para */
        vbv_qp_map[3][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,3,2,8,0:para */
        vbv_qp_map[4][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 4,2,3,2,8,0:para */
        vbv_qp_map[5][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 5,2,3,2,8,0:para */
        vbv_qp_map[6][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 6,2,3,2,8,0:para */
        vbv_qp_map[7][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 7,2,3,2,8,0:para */
        vbv_qp_map[8][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 8,2,3,2,8,0:para */
        vbv_qp_map[9][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 9,2,3,2,8,0:para */
        vbv_qp_map[10][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 10,2,3,2,8,0:para */
        vbv_qp_map[11][2] = ice_v3r2_line_num_gen(8, 0, GETMAX); /* 11,2,3,2,8,0:para */
        gain_map[0] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 0,3,2,4,1:para */
        gain_map[1] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 1,3,2,4,1:para */
        gain_map[2] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 2,3,2,4,1:para */
        gain_map[3] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,3,2,4,1:para */
        gain_map[4] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 4,3,2,4,1:para */
        gain_map[5] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 5,3,2,4,1:para */
        gain_map[6] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 6,3,2,4,1:para */
        gain_map[7] = ice_v3r2_line_num_gen(4, 1, GETMAX); /* 7,3,2,4,1:para */
        smooth_ana[0] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 0,3,2,8,1:para */
        smooth_ana[1] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        smooth_ana[2] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        smooth_ana[3] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,3,2,8,1:para */
        smooth_ana[4] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 4,3,2,8,1:para */
        smooth_ana[5] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 5,3,2,8,1:para */
        smooth_ana[6] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 6,3,2,8,1:para */
        smooth_ana[7] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 7,3,2,8,1:para */
        adp_qp_map[0] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 0,3,2,8,1:para */
        adp_qp_map[1] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 1,3,2,8,1:para */
        adp_qp_map[2] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 2,3,2,8,1:para */
        adp_qp_map[3] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,3,2,8,1:para */
        adp_qp_map[4] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 4,3,2,8,1:para */
        adp_qp_map[5] = ice_v3r2_line_num_gen(8, 1, GETMAX); /* 5,3,2,8,1:para */
        adp_qp_map[6] = ice_v3r2_line_num_gen(2, 1, GETMAX); /* 6,3,2,2,1:para */
        adp_qp_map[7] = ice_v3r2_line_num_gen(2, 1, GETMAX); /* 7,3,2,2,1:para */
    } else if (input->rc_type_mode == 3) {  // min /* 3:para */
        max_cfg_qp = rm_cfg->max_mb_qp;
        rm_cfg->big_grad_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,2,8,1:para */
        rm_cfg->diff_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,2,8,1:para */
        rm_cfg->noise_pix_num_thr = ice_v3r2_line_num_gen(6, 1, GETMIN); /* 3,2,6,1:para */
        rm_cfg->qp_inc1_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        rm_cfg->qp_inc2_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        rm_cfg->qp_dec1_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        rm_cfg->qp_dec2_bits_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        rm_cfg->buffer_size = ice_v3r2_line_num_gen(16, 1, GETMIN); /* 3,2,16,1:para */
        rm_cfg->buffer_init_bits = ice_min(rm_cfg->buffer_size, ice_v3r2_line_num_gen(16, 1, GETMIN)); /* 16,1:para */
        rm_cfg->smooth_status_thr = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 3,2,4,1:para */
        rm_cfg->smooth_deltabits_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,2,8,1:para */
        still_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,2,8,1:para */
        still_area_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,2,8,1:para */
        bit_diff0 = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 0,3,2,8,1:para */
        bit_diff1 = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        inc1_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        inc2_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        dec1_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        dec2_thr = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        buf_fullness_thr[0] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 0,3,2,8,1:para */
        buf_fullness_thr[1] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        buf_fullness_thr[2] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        buf_fullness_thr[3] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,3,2,8,1:para */
        buf_fullness_thr[4] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 4,3,2,8,1:para */
        buf_fullness_thr[5] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 5,3,2,8,1:para */
        buf_fullness_thr[6] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 6,3,2,8,1:para */
        buf_fullness_thr[7] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 7,3,2,8,1:para */
        buf_fullness_thr[8] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 8,3,2,8,1:para */
        buf_fullness_thr[9] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 9,3,2,8,1:para */
        buf_fullness_thr[10] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 10,3,2,8,1:para */
        buf_fullness_thr[11] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 11,3,2,8,1:para */
        vbv_qp_map[0][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 0,0,3,2,4,1:para */
        vbv_qp_map[0][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 0,1,3,2,4,1:para */
        vbv_qp_map[1][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 1,0,3,2,4,1:para */
        vbv_qp_map[1][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 1,1,3,2,4,1:para */
        vbv_qp_map[2][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 2,0,3,2,4,1:para */
        vbv_qp_map[2][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 2,1,3,2,4,1:para */
        vbv_qp_map[3][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 3,0,3,2,4,1:para */
        vbv_qp_map[3][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 3,1,3,2,4,1:para */
        vbv_qp_map[4][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 4,0,3,2,4,1:para */
        vbv_qp_map[4][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 4,1,3,2,4,1:para */
        vbv_qp_map[5][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 5,0,3,2,4,1:para */
        vbv_qp_map[5][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 5,1,3,2,4,1:para */
        vbv_qp_map[6][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 6,0,3,2,4,1:para */
        vbv_qp_map[6][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 6,1,3,2,4,1:para */
        vbv_qp_map[7][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 7,0,3,2,4,1:para */
        vbv_qp_map[7][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 7,1,3,2,4,1:para */
        vbv_qp_map[8][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 8,0,3,2,4,1:para */
        vbv_qp_map[8][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 8,1,3,2,4,1:para */
        vbv_qp_map[9][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 9,0,3,2,4,1:para */
        vbv_qp_map[9][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 9,1,3,2,4,1:para */
        vbv_qp_map[10][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 10,0,3,2,4,1:para */
        vbv_qp_map[10][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 10,1,3,2,4,1:para */
        vbv_qp_map[11][0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 11,0,3,2,4,1:para */
        vbv_qp_map[11][1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 11,1,3,2,4,1:para */
        vbv_qp_map[0][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 0,2,3,2,8,0:para */
        vbv_qp_map[1][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 1,2,3,2,8,0:para */
        vbv_qp_map[2][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 2,2,3,2,8,0:para */
        vbv_qp_map[3][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 3,2,3,2,8,0:para */
        vbv_qp_map[4][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 4,2,3,2,8,0:para */
        vbv_qp_map[5][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 5,2,3,2,8,0:para */
        vbv_qp_map[6][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 6,2,3,2,8,0:para */
        vbv_qp_map[7][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 7,2,3,2,8,0:para */
        vbv_qp_map[8][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 8,2,3,2,8,0:para */
        vbv_qp_map[9][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 9,2,3,2,8,0:para */
        vbv_qp_map[10][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 10,2,3,2,8,0:para */
        vbv_qp_map[11][2] = ice_v3r2_line_num_gen(8, 0, GETMIN); /* 11,2,3,2,8,0:para */
        gain_map[0] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 0,3,2,4,1:para */
        gain_map[1] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 1,3,2,4,1:para */
        gain_map[2] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 2,3,2,4,1:para */
        gain_map[3] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 3,3,2,4,1:para */
        gain_map[4] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 4,3,2,4,1:para */
        gain_map[5] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 5,3,2,4,1:para */
        gain_map[6] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 6,3,2,4,1:para */
        gain_map[7] = ice_v3r2_line_num_gen(4, 1, GETMIN); /* 7,3,2,4,1:para */
        smooth_ana[0] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 0,3,2,8,1:para */
        smooth_ana[1] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        smooth_ana[2] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        smooth_ana[3] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,3,2,8,1:para */
        smooth_ana[4] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 4,3,2,8,1:para */
        smooth_ana[5] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 5,3,2,8,1:para */
        smooth_ana[6] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 6,3,2,8,1:para */
        smooth_ana[7] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 7,3,2,8,1:para */
        adp_qp_map[0] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 0,3,2,8,1:para */
        adp_qp_map[1] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 1,3,2,8,1:para */
        adp_qp_map[2] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 2,3,2,8,1:para */
        adp_qp_map[3] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 3,3,2,8,1:para */
        adp_qp_map[4] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 4,3,2,8,1:para */
        adp_qp_map[5] = ice_v3r2_line_num_gen(8, 1, GETMIN); /* 5,3,2,8,1:para */
        adp_qp_map[6] = ice_v3r2_line_num_gen(2, 1, GETMIN); /* 6,3,2,2,1:para */
        adp_qp_map[7] = ice_v3r2_line_num_gen(2, 1, GETMIN); /* 7,3,2,2,1:para */
    } else if (input->rc_type_mode == 4) { /* 4:para */
        max_cfg_qp = rm_cfg->max_mb_qp;
        /* 3,2,1,1,0,3,2,8,1:para */
        rm_cfg->big_grad_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                               : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,2,1,1,0,3,2,8,1:para */
        rm_cfg->diff_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                           : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,2,1,1,0,3,2,6,1:para */
        rm_cfg->noise_pix_num_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(6, 1, GETMIN)
                                    : ice_v3r2_line_num_gen(6, 1, GETMAX); /* 3,2,6,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        rm_cfg->qp_inc1_bits_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        rm_cfg->qp_inc2_bits_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        rm_cfg->qp_dec1_bits_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        rm_cfg->qp_dec2_bits_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,2,1,1,0,3,2,16,1:para */
        rm_cfg->buffer_size = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(16, 1, GETMIN)
                              : ice_v3r2_line_num_gen(16, 1, GETMAX); /* 3,2,16,1:para */
        rm_cfg->buffer_init_bits = ice_min(rm_cfg->buffer_size, (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ?
            ice_v3r2_line_num_gen(16, 1, GETMIN) : ice_v3r2_line_num_gen(16, 1, GETMAX)); /* 16,1,16,1:para */
        /* 3,2,1,1,0,3,2,4,1:para */
        rm_cfg->smooth_status_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                                    : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 3,2,1,1,0,3,2,8,1:para */
        rm_cfg->smooth_deltabits_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ?
            ice_v3r2_line_num_gen(8, 1, GETMIN) : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,2,1,1,0,3,2,8,1:para */
        still_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                    : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,2,1,1,0,3,2,8,1:para */
        still_area_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                         : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 0,3,2,1,1,0,3,2,8,1:para */
        bit_diff0 = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                    : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        bit_diff1 = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                    : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        inc1_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        inc2_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        dec1_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        dec2_thr = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                   : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 0,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[3] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 4,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[4] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 5,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[5] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 6,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[6] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 7,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[7] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 8,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[8] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 9,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[9] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                              : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 10,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[10] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                               : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 11,3,2,1,1,0,3,2,8,1:para */
        buf_fullness_thr[11] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                               : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 0,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[0][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 0,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[0][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 1,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[1][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 1,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[1][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 2,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[2][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 2,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[2][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 3,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[3][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 3,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[3][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 4,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[4][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 4,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[4][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 5,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[5][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 5,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[5][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 6,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[6][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 6,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[6][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 7,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[7][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 7,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[7][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 8,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[8][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 8,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[8][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 9,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[9][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 9,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[9][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                           : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 10,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[10][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                            : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 10,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[10][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                            : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 11,0,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[11][0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                            : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 11,1,3,2,1,1,0,3,2,4,1:para */
        vbv_qp_map[11][1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                            : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 0,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[0][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 1,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[1][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 2,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[2][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 3,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[3][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 4,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[4][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 5,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[5][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 6,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[6][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 7,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[7][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 8,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[8][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 9,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[9][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                           : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 10,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[10][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                            : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 11,2,3,2,1,1,0,3,2,8,0:para */
        vbv_qp_map[11][2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 0, GETMIN)
                            : ice_v3r2_line_num_gen(8, 0, GETMAX); /* 3,2,8,0:para */
        /* 0,3,2,1,1,0,3,2,4,1:para */
        gain_map[0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 1,3,2,1,1,0,3,2,4,1:para */
        gain_map[1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 2,3,2,1,1,0,3,2,4,1:para */
        gain_map[2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 3,3,2,1,1,0,3,2,4,1:para */
        gain_map[3] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 4,3,2,1,1,0,3,2,4,1:para */
        gain_map[4] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 5,3,2,1,1,0,3,2,4,1:para */
        gain_map[5] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 6,3,2,1,1,0,3,2,4,1:para */
        gain_map[6] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 7,3,2,1,1,0,3,2,4,1:para */
        gain_map[7] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(4, 1, GETMIN)
                      : ice_v3r2_line_num_gen(4, 1, GETMAX); /* 3,2,4,1:para */
        /* 0,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[3] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 4,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[4] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 5,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[5] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 6,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[6] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 7,3,2,1,1,0,3,2,8,1:para */
        smooth_ana[7] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 0,3,2,1,1,0,3,2,8,1:para */
        adp_qp_map[0] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 1,3,2,1,1,0,3,2,8,1:para */
        adp_qp_map[1] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 2,3,2,1,1,0,3,2,8,1:para */
        adp_qp_map[2] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 3,3,2,1,1,0,3,2,8,1:para */
        adp_qp_map[3] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 4,3,2,1,1,0,3,2,8,1:para */
        adp_qp_map[4] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 5,3,2,1,1,0,3,2,8,1:para */
        adp_qp_map[5] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(8, 1, GETMIN)
                        : ice_v3r2_line_num_gen(8, 1, GETMAX); /* 3,2,8,1:para */
        /* 6,3,2,1,1,0,3,2,2,1:para */
        adp_qp_map[6] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(2, 1, GETMIN)
                        : ice_v3r2_line_num_gen(2, 1, GETMAX); /* 3,2,2,1:para */
        /* 7,3,2,1,1,0,3,2,2,1:para */
        adp_qp_map[7] = (ice_v3r2_line_num_gen(1, 1, GETRAND) == 0) ? ice_v3r2_line_num_gen(2, 1, GETMIN)
                        : ice_v3r2_line_num_gen(2, 1, GETMAX); /* 3,2,2,1:para */
    }

    if (input->rc_type_mode == 1) {
        rm_cfg->min_mb_bits = ice_v3_r2_line_my_rand_clip(rm_cfg->budget_mb_bits, 0, rm_cfg->min_mb_bits);
        rm_cfg->buffer_init_bits = ice_v3_r2_line_my_rand_clip(6500, 0, rm_cfg->buffer_init_bits); /* 3,2,6500:para */
        delta = ice_max(1, ((signed int)rm_cfg->buffer_size - (signed int)rm_cfg->buffer_init_bits +
                        (signed int)mb_num_x / 2) / (signed int)mb_num_x); /* 2:para */
        if (delta > 1) {
            rm_cfg->rm_debug_en = 0;
        }

        rm_cfg->budget_mb_bits = rm_cfg->is_lossless ? rm_cfg->budget_mb_bits : clip(rm_cfg->budget_mb_bits - delta);
        rm_cfg->budget_mb_bits = ice_max(rm_cfg->budget_mb_bits, MB_WTH * 1);
        rm_cfg->budget_mb_bits_last = rm_cfg->budget_mb_bits * last_mb_wth / MB_WTH;
        buf_fullness_thr[0] = ice_v3_r2_line_my_rand_clip(255, 0, buf_fullness_thr[0]); /* 0,3,2,255,0,0:para */
        /* 1,3,2,255,0,1:para */
        buf_fullness_thr[1] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[0], buf_fullness_thr[1]);
        /* 2,3,2,255,1,2:para */
        buf_fullness_thr[2] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[1], buf_fullness_thr[2]);
        /* 3,3,2,255,2,3:para */
        buf_fullness_thr[3] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[2], buf_fullness_thr[3]);
        /* 4,3,2,255,3,4:para */
        buf_fullness_thr[4] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[3], buf_fullness_thr[4]);
        /* 5,3,2,255,4,5:para */
        buf_fullness_thr[5] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[4], buf_fullness_thr[5]);
        /* 6,3,2,255,5,6:para */
        buf_fullness_thr[6] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[5], buf_fullness_thr[6]);
        /* 7,3,2,255,6,7:para */
        buf_fullness_thr[7] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[6], buf_fullness_thr[7]);
        /* 8,3,2,255,7,8:para */
        buf_fullness_thr[8] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[7], buf_fullness_thr[8]);
        /* 9,3,2,255,8,9:para */
        buf_fullness_thr[9] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[8], buf_fullness_thr[9]);
        /* 10,3,2,255,9,10:para */
        buf_fullness_thr[10] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[9], buf_fullness_thr[10]);
        /* 11,3,2,255,10,11:para */
        buf_fullness_thr[11] = ice_v3_r2_line_my_rand_clip(255, buf_fullness_thr[10], buf_fullness_thr[11]);
        vbv_qp_map[0][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, 0, vbv_qp_map[0][0]);
        vbv_qp_map[0][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[0][0], vbv_qp_map[0][1]);
        vbv_qp_map[1][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[0][0], vbv_qp_map[1][0]);
        vbv_qp_map[1][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[1][0], vbv_qp_map[0][1]),
                                                       vbv_qp_map[1][1]);
        /* 2,0,3,2,1,0,2,0:para */
        vbv_qp_map[2][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[1][0], vbv_qp_map[2][0]);
        /* 2,1,3,2,2,0,1,1:para */
        vbv_qp_map[2][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[2][0], vbv_qp_map[1][1]),
                                                       vbv_qp_map[2][1]); /* 2,1:para */
        /* 3,0,3,2,2,0,3,0:para */
        vbv_qp_map[3][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[2][0], vbv_qp_map[3][0]);
        /* 3,1,3,2,3,0,2,1:para */
        vbv_qp_map[3][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[3][0], vbv_qp_map[2][1]),
                                                       vbv_qp_map[3][1]); /* 3,1:para */
        /* 4,0,3,2,3,0,4,0:para */
        vbv_qp_map[4][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[3][0], vbv_qp_map[4][0]);
        /* 4,1,3,2,4,0,3,1:para */
        vbv_qp_map[4][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[4][0], vbv_qp_map[3][1]),
                                                       vbv_qp_map[4][1]); /* 4,1:para */
        /* 5,0,3,2,4,0,5,0:para */
        vbv_qp_map[5][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[4][0], vbv_qp_map[5][0]);
        /* 5,1,3,2,5,0,4,1:para */
        vbv_qp_map[5][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[5][0], vbv_qp_map[4][1]),
                                                       vbv_qp_map[5][1]); /* 5,1:para */
        /* 6,0,3,2,5,0,6,0:para */
        vbv_qp_map[6][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[5][0], vbv_qp_map[6][0]);
        /* 6,1,3,2,6,0,5,1:para */
        vbv_qp_map[6][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[6][0], vbv_qp_map[5][1]),
                                                       vbv_qp_map[6][1]); /* 6,1:para */
        /* 7,0,3,2,6,0,7,0:para */
        vbv_qp_map[7][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[6][0], vbv_qp_map[7][0]);
        /* 7,1,3,2,7,0,6,1:para */
        vbv_qp_map[7][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[7][0], vbv_qp_map[6][1]),
                                                       vbv_qp_map[7][1]); /* 7,1:para */
        /* 8,0,3,2,7,0,8,0:para */
        vbv_qp_map[8][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[7][0], vbv_qp_map[8][0]);
        /* 8,1,3,2,8,0,7,1:para */
        vbv_qp_map[8][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[8][0], vbv_qp_map[7][1]),
                                                       vbv_qp_map[8][1]); /* 8,1:para */
        /* 9,0,3,2,8,0,9,0:para */
        vbv_qp_map[9][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[8][0], vbv_qp_map[9][0]);
        /* 9,1,3,2,9,0,8,1:para */
        vbv_qp_map[9][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[9][0], vbv_qp_map[8][1]),
                                                       vbv_qp_map[9][1]); /* 9,1:para */
        /* 10,0,3,2,9,0,10,0:para */
        vbv_qp_map[10][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[9][0], vbv_qp_map[10][0]);
        /* 10,1,3,2,10,0,9,1:para */
        vbv_qp_map[10][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[10][0], vbv_qp_map[9][1]),
                                                        vbv_qp_map[10][1]); /* 10,1:para */
        /* 11,0,3,2,10,0,11,0:para */
        vbv_qp_map[11][0] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, vbv_qp_map[10][0], vbv_qp_map[11][0]);
        /* 11,1,3,2,11,0,10,1:para */
        vbv_qp_map[11][1] = ice_v3_r2_line_my_rand_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[11][0], vbv_qp_map[10][1]),
                                                        vbv_qp_map[11][1]); /* 11,1:para */
        vbv_qp_map[0][2] = ice_v3_r2_line_my_rand_clip(127, -128, vbv_qp_map[0][2]); /* 0,2,3,2,127,128,0,2:para */
        /* 1,2,3,2,0,2,128,1,2:para */
        vbv_qp_map[1][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[0][2], -128, vbv_qp_map[1][2]);
        /* 2,2,3,2,1,2,128,2,2:para */
        vbv_qp_map[2][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[1][2], -128, vbv_qp_map[2][2]);
        /* 3,2,3,2,2,2,128,3,2:para */
        vbv_qp_map[3][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[2][2], -128, vbv_qp_map[3][2]);
        /* 4,2,3,2,3,2,128,4,2:para */
        vbv_qp_map[4][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[3][2], -128, vbv_qp_map[4][2]);
        /* 5,2,3,2,4,2,128,5,2:para */
        vbv_qp_map[5][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[4][2], -128, vbv_qp_map[5][2]);
        /* 6,2,3,2,5,2,128,6,2:para */
        vbv_qp_map[6][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[5][2], -128, vbv_qp_map[6][2]);
        /* 7,2,3,2,6,2,128,7,2:para */
        vbv_qp_map[7][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[6][2], -128, vbv_qp_map[7][2]);
        /* 8,2,3,2,7,2,128,8,2:para */
        vbv_qp_map[8][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[7][2], -128, vbv_qp_map[8][2]);
        /* 9,2,3,2,8,2,128,9,2:para */
        vbv_qp_map[9][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[8][2], -128, vbv_qp_map[9][2]);
        /* 10,2,3,2,9,2,128,10,2:para */
        vbv_qp_map[10][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[9][2], -128, vbv_qp_map[10][2]);
        /* 11,2,3,2,10,2,128,11,2:para */
        vbv_qp_map[11][2] = ice_v3_r2_line_my_rand_clip(vbv_qp_map[10][2], -128, vbv_qp_map[11][2]);
        gain_map[0] = ice_v3_r2_line_my_rand_clip(15, 0, gain_map[0]); /* 0,3,2,15,0,0:para */
        gain_map[1] = ice_v3_r2_line_my_rand_clip(15, gain_map[0], gain_map[1]); /* 1,3,2,15,0,1:para */
        gain_map[2] = ice_v3_r2_line_my_rand_clip(15, gain_map[1], gain_map[2]); /* 2,3,2,15,1,2:para */
        gain_map[3] = ice_v3_r2_line_my_rand_clip(15, gain_map[2], gain_map[3]); /* 3,3,2,15,2,3:para */
        gain_map[4] = ice_v3_r2_line_my_rand_clip(15, gain_map[3], gain_map[4]); /* 4,3,2,15,3,4:para */
        gain_map[5] = ice_v3_r2_line_my_rand_clip(15, gain_map[4], gain_map[5]); /* 5,3,2,15,4,5:para */
        gain_map[6] = ice_v3_r2_line_my_rand_clip(15, gain_map[5], gain_map[6]); /* 6,3,2,15,5,6:para */
        gain_map[7] = ice_v3_r2_line_my_rand_clip(15, gain_map[6], gain_map[7]); /* 7,3,2,15,6,7:para */
        adp_qp_map[6] = adp_qp_map[6] > 2 ? 2 : adp_qp_map[6]; /* 6,6,2,2,6:para */
        adp_qp_map[7] = adp_qp_map[7] > 2 ? 2 : adp_qp_map[7]; /* 7,7,2,2,7:para */
    } else if (input->rc_type_mode != 0) {
        rm_cfg->min_mb_bits = cmpseg_clip(rm_cfg->budget_mb_bits, 0, rm_cfg->min_mb_bits);
        rm_cfg->buffer_init_bits = cmpseg_clip(6500, 0, rm_cfg->buffer_init_bits);
        delta = ice_max(1, ((signed int)rm_cfg->buffer_size - (signed int)rm_cfg->buffer_init_bits +
                        (signed int)mb_num_x / 2) / (signed int)mb_num_x); /* 2:para */
        if (delta > 1) {
            rm_cfg->rm_debug_en = 0;
        }

        rm_cfg->budget_mb_bits = rm_cfg->is_lossless ? rm_cfg->budget_mb_bits : clip(rm_cfg->budget_mb_bits - delta);
        rm_cfg->budget_mb_bits = ice_max(rm_cfg->budget_mb_bits, MB_WTH * 1);
        rm_cfg->budget_mb_bits_last = rm_cfg->budget_mb_bits * last_mb_wth / MB_WTH;
        buf_fullness_thr[0] = cmpseg_clip(255, 0, buf_fullness_thr[0]);
        buf_fullness_thr[1] = cmpseg_clip(255, buf_fullness_thr[0], buf_fullness_thr[1]);
        buf_fullness_thr[2] = cmpseg_clip(255, buf_fullness_thr[1], buf_fullness_thr[2]); /* 2,255,1,2:para */
        buf_fullness_thr[3] = cmpseg_clip(255, buf_fullness_thr[2], buf_fullness_thr[3]); /* 3,255,2,3:para */
        buf_fullness_thr[4] = cmpseg_clip(255, buf_fullness_thr[3], buf_fullness_thr[4]); /* 4,255,3,4:para */
        buf_fullness_thr[5] = cmpseg_clip(255, buf_fullness_thr[4], buf_fullness_thr[5]); /* 5,255,4,5:para */
        buf_fullness_thr[6] = cmpseg_clip(255, buf_fullness_thr[5], buf_fullness_thr[6]); /* 6,255,5,6:para */
        buf_fullness_thr[7] = cmpseg_clip(255, buf_fullness_thr[6], buf_fullness_thr[7]); /* 7,255,6,7:para */
        buf_fullness_thr[8] = cmpseg_clip(255, buf_fullness_thr[7], buf_fullness_thr[8]); /* 8,255,7,8:para */
        buf_fullness_thr[9] = cmpseg_clip(255, buf_fullness_thr[8], buf_fullness_thr[9]); /* 9,255,8,9:para */
        buf_fullness_thr[10] = cmpseg_clip(255, buf_fullness_thr[9], buf_fullness_thr[10]); /* 10,255,9,10:para */
        buf_fullness_thr[11] = cmpseg_clip(255, buf_fullness_thr[10], buf_fullness_thr[11]); /* 11,255,10,11:para */
        vbv_qp_map[0][0] = cmpseg_clip(max_cfg_qp, 0, vbv_qp_map[0][0]);
        vbv_qp_map[0][1] = cmpseg_clip(max_cfg_qp, vbv_qp_map[0][0], vbv_qp_map[0][1]);
        vbv_qp_map[1][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[0][0], vbv_qp_map[1][0]);
        vbv_qp_map[1][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[1][0], vbv_qp_map[0][1]), vbv_qp_map[1][1]);
        vbv_qp_map[2][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[1][0], vbv_qp_map[2][0]); /* 2,0,1,0,2,0:para */
        /* 2,1,2,0,1,1,2,1:para */
        vbv_qp_map[2][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[2][0], vbv_qp_map[1][1]), vbv_qp_map[2][1]);
        vbv_qp_map[3][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[2][0], vbv_qp_map[3][0]); /* 3,0,2,0,3,0:para */
        /* 3,1,3,0,2,1,3,1:para */
        vbv_qp_map[3][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[3][0], vbv_qp_map[2][1]), vbv_qp_map[3][1]);
        vbv_qp_map[4][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[3][0], vbv_qp_map[4][0]); /* 4,0,3,0,4,0:para */
        /* 4,1,4,0,3,1,4,1:para */
        vbv_qp_map[4][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[4][0], vbv_qp_map[3][1]), vbv_qp_map[4][1]);
        vbv_qp_map[5][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[4][0], vbv_qp_map[5][0]); /* 5,0,4,0,5,0:para */
        /* 5,1,5,0,4,1,5,1:para */
        vbv_qp_map[5][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[5][0], vbv_qp_map[4][1]), vbv_qp_map[5][1]);
        vbv_qp_map[6][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[5][0], vbv_qp_map[6][0]); /* 6,0,5,0,6,0:para */
        /* 6,1,6,0,5,1,6,1:para */
        vbv_qp_map[6][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[6][0], vbv_qp_map[5][1]), vbv_qp_map[6][1]);
        vbv_qp_map[7][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[6][0], vbv_qp_map[7][0]); /* 7,0,6,0,7,0:para */
        /* 7,1,7,0,6,1,7,1:para */
        vbv_qp_map[7][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[7][0], vbv_qp_map[6][1]), vbv_qp_map[7][1]);
        vbv_qp_map[8][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[7][0], vbv_qp_map[8][0]); /* 8,0,7,0,8,0:para */
        /* 8,1,8,0,7,1,8,1:para */
        vbv_qp_map[8][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[8][0], vbv_qp_map[7][1]), vbv_qp_map[8][1]);
        vbv_qp_map[9][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[8][0], vbv_qp_map[9][0]); /* 9,0,8,0,9,0:para */
        /* 9,1,9,0,8,1,9,1:para */
        vbv_qp_map[9][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[9][0], vbv_qp_map[8][1]), vbv_qp_map[9][1]);
        vbv_qp_map[10][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[9][0], vbv_qp_map[10][0]); /* 10,0,9,0,10,0:para */
        /* 10,1,10,0,9,1:para */
        vbv_qp_map[10][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[10][0], vbv_qp_map[9][1]),
            vbv_qp_map[10][1]);
        vbv_qp_map[11][0] = cmpseg_clip(max_cfg_qp, vbv_qp_map[10][0], vbv_qp_map[11][0]); /* 11,0,10,0,11,0:para */
        /* 11,1,11,0,10,1:para */
        vbv_qp_map[11][1] = cmpseg_clip(max_cfg_qp, cmpseg_max(vbv_qp_map[11][0], vbv_qp_map[10][1]),
                                        vbv_qp_map[11][1]);
        vbv_qp_map[0][2] = cmpseg_clip(127, -128, vbv_qp_map[0][2]); /* 0,2,127,128,0,2:para */
        vbv_qp_map[1][2] = cmpseg_clip(vbv_qp_map[0][2], -128, vbv_qp_map[1][2]); /* 1,2,0,2,128,1,2:para */
        vbv_qp_map[2][2] = cmpseg_clip(vbv_qp_map[1][2], -128, vbv_qp_map[2][2]); /* 2,2,1,2,128,2,2:para */
        vbv_qp_map[3][2] = cmpseg_clip(vbv_qp_map[2][2], -128, vbv_qp_map[3][2]); /* 3,2,2,2,128,3,2:para */
        vbv_qp_map[4][2] = cmpseg_clip(vbv_qp_map[3][2], -128, vbv_qp_map[4][2]); /* 4,2,3,2,128,4,2:para */
        vbv_qp_map[5][2] = cmpseg_clip(vbv_qp_map[4][2], -128, vbv_qp_map[5][2]); /* 5,2,4,2,128,5,2:para */
        vbv_qp_map[6][2] = cmpseg_clip(vbv_qp_map[5][2], -128, vbv_qp_map[6][2]); /* 6,2,5,2,128,6,2:para */
        vbv_qp_map[7][2] = cmpseg_clip(vbv_qp_map[6][2], -128, vbv_qp_map[7][2]); /* 7,2,6,2,128,7,2:para */
        vbv_qp_map[8][2] = cmpseg_clip(vbv_qp_map[7][2], -128, vbv_qp_map[8][2]); /* 8,2,7,2,128,8,2:para */
        vbv_qp_map[9][2] = cmpseg_clip(vbv_qp_map[8][2], -128, vbv_qp_map[9][2]); /* 9,2,8,2,128,9,2:para */
        vbv_qp_map[10][2] = cmpseg_clip(vbv_qp_map[9][2], -128, vbv_qp_map[10][2]); /* 10,2,9,2,128,10,2:para */
        vbv_qp_map[11][2] = cmpseg_clip(vbv_qp_map[10][2], -128, vbv_qp_map[11][2]); /* 11,2,10,2,128,11,2:para */
        gain_map[0] = cmpseg_clip(15, 0, gain_map[0]);
        gain_map[1] = cmpseg_clip(15, gain_map[0], gain_map[1]);
        gain_map[2] = cmpseg_clip(15, gain_map[1], gain_map[2]); /* 2,15,1,2:para */
        gain_map[3] = cmpseg_clip(15, gain_map[2], gain_map[3]); /* 3,15,2,3:para */
        gain_map[4] = cmpseg_clip(15, gain_map[3], gain_map[4]); /* 4,15,3,4:para */
        gain_map[5] = cmpseg_clip(15, gain_map[4], gain_map[5]); /* 5,15,4,5:para */
        gain_map[6] = cmpseg_clip(15, gain_map[5], gain_map[6]); /* 6,15,5,6:para */
        gain_map[7] = cmpseg_clip(15, gain_map[6], gain_map[7]); /* 7,15,6,7:para */
        adp_qp_map[6] = adp_qp_map[6] > 2 ? 2 : adp_qp_map[6]; /* 6,6,2,2,6:para */
        adp_qp_map[7] = adp_qp_map[7] > 2 ? 2 : adp_qp_map[7]; /* 7,7,2,2,7:para */
    }

    if (input->rc_type_mode != 0) {
        /* 3,24,2,16,1,8,0:para */
        rm_cfg->buf_fullness_thr_reg0 = (buf_fullness_thr[3] << 24) | (buf_fullness_thr[2] << 16) |
                                        (buf_fullness_thr[1] << 8) | buf_fullness_thr[0]; /* 1,8,0:para */
        /* 1,7,24,6,16,5,8,4:para */
        rm_cfg->buf_fullness_thr_reg1 = (buf_fullness_thr[7] << 24) | (buf_fullness_thr[6] << 16) |
                                        (buf_fullness_thr[5] << 8) | buf_fullness_thr[4]; /* 5,8,4:para */
        /* 11,24,10,16:para */
        rm_cfg->buf_fullness_thr_reg2 = (buf_fullness_thr[11] << 24) | (buf_fullness_thr[10] << 16) |
                                        (buf_fullness_thr[9] << 8) | buf_fullness_thr[8]; /* 9,8,8:para */
        rm_cfg->qp_rge_reg0 = (vbv_qp_map[3][1] << 28) | (vbv_qp_map[3][0] << 24) | /* 0,3,1,28,3,0,24:para */
                              (vbv_qp_map[2][1] << 20) | (vbv_qp_map[2][0] << 16) | /* 2,1,20,2,0,16:para */
                              (vbv_qp_map[1][1] << 12) | (vbv_qp_map[1][0] << 8) | /* 1,1,12,1,0,8:para */
                              (vbv_qp_map[0][1] << 4) | vbv_qp_map[0][0]; /* 0,1,4,0,0:para */
        rm_cfg->qp_rge_reg1 = (vbv_qp_map[7][1] << 28) | (vbv_qp_map[7][0] << 24) | /* 1,7,1,28,7,0,24:para */
                              (vbv_qp_map[6][1] << 20) | (vbv_qp_map[6][0] << 16) | /* 6,1,20,6,0,16:para */
                              (vbv_qp_map[5][1] << 12) | (vbv_qp_map[5][0] << 8) | /* 5,1,12,5,0,8:para */
                              (vbv_qp_map[4][1] << 4) | vbv_qp_map[4][0]; /* 4,1,4,4,0:para */
        rm_cfg->qp_rge_reg2 = (vbv_qp_map[11][1] << 28) | (vbv_qp_map[11][0] << 24) | /* 2,11,1,28,11,0,24:para */
                              (vbv_qp_map[10][1] << 20) | (vbv_qp_map[10][0] << 16) | /* 10,1,20,10,0,16:para */
                              (vbv_qp_map[9][1] << 12) | (vbv_qp_map[9][0] << 8) | /* 9,1,12,9,0,8:para */
                              (vbv_qp_map[8][1] << 4) | vbv_qp_map[8][0]; /* 8,1,4,8,0:para */

        rm_cfg->bits_offset_reg0 = ((vbv_qp_map[3][2] & 0xff) << 24) | /* 3,2,24:para */
                                   ((vbv_qp_map[2][2] & 0xff) << 16) | /* 2,2,16:para */
                                   ((vbv_qp_map[1][2] & 0xff) << 8) | /* 1,2,8:para */
                                   (vbv_qp_map[0][2] & 0xff); /* 2:para */
        /* 1,7,2,0,24,6,2,0,16,5,2,0,8,4,2,0:para */
        rm_cfg->bits_offset_reg1 = ((vbv_qp_map[7][2] & 0xff) << 24) | ((vbv_qp_map[6][2] & 0xff) << 16) |
                                   ((vbv_qp_map[5][2] & 0xff) << 8) | (vbv_qp_map[4][2] & 0xff);
        /* 2,11,2,0,24,10,2,0,16,9,2,0,8,8,2,0:para */
        rm_cfg->bits_offset_reg2 = ((vbv_qp_map[11][2] & 0xff) << 24) | ((vbv_qp_map[10][2] & 0xff) << 16) |
                                   ((vbv_qp_map[9][2] & 0xff) << 8) | (vbv_qp_map[8][2] & 0xff);
        rm_cfg->est_err_gain_map = (gain_map[7] << 28) | (gain_map[6] << 24) | /* 7,28,6,24:para */
                                   (gain_map[5] << 20) | (gain_map[4] << 16) | /* 5,20,4,16:para */
                                   (gain_map[3] << 12) | (gain_map[2] << 8) | /* 3,12,2,8:para */
                                   (gain_map[1] << 4) | gain_map[0]; /* 1,4:para */

        rm_cfg->reserve_para0 = ((still_thr & 0xff) << 24) | ((still_area_thr & 0xff) << 16) | /* 0,0,24,0,16:para */
                                ((bit_diff0 & 0xff) << 8) | (bit_diff1 & 0xff); /* 0,0,8,1,0:para */
        rm_cfg->reserve_para1 = ((inc2_thr & 0xff) << 24) | ((inc1_thr & 0xff) << 16) | /* 1,2,0,24,1,0,16:para */
                                ((dec2_thr & 0xff) << 8) | (dec1_thr & 0xff); /* 2,0,8,1,0:para */
        rm_cfg->adpqp_thr0 = ((adp_qp_map[0] & 0xff) << 24) | ((adp_qp_map[1] & 0xff) << 16) | /* 24,1,16:para */
                             ((adp_qp_map[2] & 0xff) << 8) | (adp_qp_map[3] & 0xff); /* 2,8,3,0:para */
        rm_cfg->adpqp_thr1 = ((adp_qp_map[4] & 0xff) << 24) | ((adp_qp_map[5] & 0xff) << 16) | /* 1,4,24,5,16:para */
                             ((adp_qp_map[6] & 0xff) << 8) | (adp_qp_map[7] & 0xff); /* 6,0,8,7,0:para */
    }

    return HI_SUCCESS;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

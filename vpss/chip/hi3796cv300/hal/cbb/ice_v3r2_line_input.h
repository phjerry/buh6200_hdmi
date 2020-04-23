/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: vpss common
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef _ICE_V3R2_LINE_INPUT_H_
#define _ICE_V3R2_LINE_INPUT_H_

#define _RM_EN_ 1

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FILE_LENGTH 255
#define ice_max(a, b)  (((a) >= (b)) ? (a) : (b))
#define ice_min(a, b)  (((a) < (b)) ? (a) : (b))
#define clip(a)        (((a) >= 0) ? (a) : 0)
#define MB_WTH         32

typedef struct {
    char input_file[FILE_LENGTH];
    char output_file[FILE_LENGTH];

    int frame_width;   // input frame width
    int frame_height;  // input frame height
    int cmp_mode;      // 0 -- cmp 1 -- bypass  2--esl_mode
    int bit_depth;     // bit width   0-- 8bit  1-- 10bit
    int pix_format;    // 0 -- 420 1 -- 422
    int esl_qp;
    int is_lossless;  // 1: is lossless compress                   0: is lossy compress
    int chroma_en;
    int comp_ratio_int;  // luma&chroma compression ratio, (=comp_ratio * 1000)
    int rc_type_mode;    // 0: default    1: random     2: max value    3: min value   4: max/min combination
} ice_v3_r2_line_input;

typedef struct {
    unsigned int chroma_en;
    unsigned int is_lossless;
    unsigned int cmp_mode;
    unsigned int bit_depth;
    unsigned int esl_qp;
    unsigned int frame_width;
    unsigned int frame_height;
    unsigned int rm_debug_en;
    unsigned int dcmp_error;

    // rc parameter
    unsigned int big_grad_thr;
    unsigned int diff_thr;
    unsigned int noise_pix_num_thr;
    unsigned int qp_inc1_bits_thr;
    unsigned int qp_inc2_bits_thr;
    unsigned int qp_dec1_bits_thr;
    unsigned int qp_dec2_bits_thr;
    unsigned int buf_fullness_thr_reg0;
    unsigned int buf_fullness_thr_reg1;
    unsigned int buf_fullness_thr_reg2;
    unsigned int qp_rge_reg0;
    unsigned int qp_rge_reg1;
    unsigned int qp_rge_reg2;
    unsigned int bits_offset_reg0;
    unsigned int bits_offset_reg1;
    unsigned int bits_offset_reg2;
    unsigned int est_err_gain_map;
    unsigned int buffer_size;
    unsigned int buffer_init_bits;
    unsigned int smooth_status_thr;
    unsigned int budget_mb_bits_last;
    unsigned int min_mb_bits;
    int max_mb_qp;
    int budget_mb_bits;

    // reserved parameter
    unsigned int reserve_para0;
    unsigned int reserve_para1;

    // add parameter
    unsigned char smooth_deltabits_thr;  // used to determine the number of bits of smooth_delta
    unsigned int adpqp_thr0;             // adpqp_thr0,1,2,3
    unsigned int adpqp_thr1;             // adpQp_thr4,5, and adpQP clip_thr
} ice_v3r2_line_cfg;

int ice_v3_r2_line_cfginit(ice_v3r2_line_cfg *rm_cfg, ice_v3_r2_line_input *input);
unsigned int ice_v3r2_line_codec(ice_v3r2_line_cfg *rm_cfg, unsigned short *ori_data_y,
    unsigned short *ori_data_u, unsigned short *ori_data_v);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

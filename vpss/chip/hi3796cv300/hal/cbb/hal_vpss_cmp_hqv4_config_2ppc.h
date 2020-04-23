/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_cmp_hqv4_config_2ppc.h hander file video encoder config
 * Author: zhangjunyu
 * Create: 2016/07/03
 */

#ifndef _TTV_CMP_HQV4_CONFIG_2PPC_H_
#define _TTV_CMP_HQV4_CONFIG_2PPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define cmphqv4_clip(max, min, x) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))
#define cmphqv4_max(a, b)         ((a) > (b) ? (a) : (b))
#define cmphqv4_min(a, b)         ((a) > (b) ? (b) : (a))

typedef struct {
    int pixel_format;     // YUV sample mode 0: YUV422   1: YUV420
    int cmp_mode;         // 1: is raw frame compress mode             0: is not ...
    int is_lossless;      // 1: is lossless compress                   0:is lossy compress
    int frame_width;      // input frame width
    int frame_height;     // input frame height
    int frame_bitdepth;   // pixel bit depth, 1: 10 bit    0: 8 bit
    int part_cmp_en;      // 0: compress total frame                   1: compress partial frame
    int pcmp_start_hpos;  // start horizontal position of partial cmp
    int pcmp_end_hpos;    // end horizontal position of partial cmp
    int slice_cmp_en;     // 1: slice compress enable   0: slice compress disable
    int rm_debug_en;      // 1: rm debug enable     0: rm debug disable
    int mb_bits;
    int smth_thr;
    int still_thr;
    int big_grad_thr;
    int diff_thr;
    int smth_pix_num_thr;
    int still_pix_num_thr;
    int noise_pix_num_thr;
    int qp_inc1_bits_thr;
    int qp_inc2_bits_thr;
    int qp_dec1_bits_thr;
    int qp_dec2_bits_thr;
    int min_mb_bits;
    int grph_en;
    int grph_ideal_bits_y;
    int grph_ideal_bits_c;
    int grph_bits_penalty;
    int qp_force_en;
    int first_mb_adj_bits;
    int first_row_adj_bits;
    int first_col_adj_bits;
    int buffer_init_bits;
    int buf_fullness_thr_reg0;
    int buf_fullness_thr_reg1;
    int buf_fullness_thr_reg2;
    int qp_rge_reg0;
    int qp_rge_reg1;
    int qp_rge_reg2;
    int bits_offset_reg0;
    int bits_offset_reg1;
    int bits_offset_reg2;
    int est_err_gain_map;
    int samll_diff_thr;
    int low_status_thr;
    int grph_penalty_bit_c;
    int noise_qp_inc_bits_thr;
    int noise_status_thr;
    int pre_noise_mb_thr0;
    int pre_noise_mb_thr1;
    int special_qp_dec_bits_thr;
    int special_status_thr;
    int smooth_status_thr;
} cmp_hqv4_parm_t;
typedef struct {
    int pixel_format;     // YUV sample mode 0: YUV422   1: YUV420
    int cmp_mode;         // 1: is raw frame compress mode             0: is not ...
    int is_lossless;      // 1: is lossless compress                   0: is lossy compress
    int frame_width;      // input frame width
    int frame_height;     // input frame height
    int frame_bitdepth;   // pixel bit depth, 1: 10 bit    0: 8 bit
    int slice_cmp_en;     // 1: slice compress enable   0: slice compress disable
    int start_slice_idx;  // dcmp start slice index
    int bd_det_en;
    int buffer_init_bits;
    int buffer_fullness_thr;
    int budget_mb_bits;
    int rm_debug_en;  // 1: rm debug enable     0: rm debug disable
} dcmp_hqv4_parm_t;
typedef struct {
    int pixel_format;     // YUV sample mode 0: YUV422   1: YUV420
    int cmp_mode;         // 1: is raw frame compress mode             0: is not ...
    int is_lossless;      // 1: is lossless compress                   0:is lossy compress
    int frame_width;      // input frame width
    int frame_height;     // input frame height
    int frame_bitdepth;   // pixel bit depth, 1: 10 bit    0: 8 bit
    int part_cmp_en;      // 0: compress total frame                   1: compress partial frame
    int pcmp_start_hpos;  // start horizontal position of partial cmp
    int pcmp_end_hpos;    // end horizontal position of partial cmp
    int rm_debug_en;      // 1: rm debug enable     0: rm debug disable
    int mb_bits;
    int smth_thr;
    int still_thr;
    int big_grad_thr;
    int diff_thr;
    int smth_pix_num_thr;
    int still_pix_num_thr;
    int noise_pix_num_thr;
    int qp_inc1_bits_thr;
    int qp_inc2_bits_thr;
    int qp_dec1_bits_thr;
    int qp_dec2_bits_thr;
    int min_mb_bits;
    int grph_en;
    int grph_ideal_bits;
    int grph_bits_penalty;
    int qp_force_en;
    int first_mb_adj_bits;
    int first_row_adj_bits;
    int first_col_adj_bits;
    int buffer_init_bits;
    int buf_fullness_thr_reg0;
    int buf_fullness_thr_reg1;
    int buf_fullness_thr_reg2;
    int qp_rge_reg0;
    int qp_rge_reg1;
    int qp_rge_reg2;
    int bits_offset_reg0;
    int bits_offset_reg1;
    int bits_offset_reg2;
    int est_err_gain_map;
    int smooth_status_thr;
} cmp_hqv4_alone_parm_t;
typedef struct {
    int pixel_format;    // YUV sample mode 0: YUV422   1: YUV420
    int cmp_mode;        // 1: is raw frame compress mode             0: is not ...
    int is_lossless;     // 1: is lossless compress                   0: is lossy compress
    int frame_width;     // input frame width
    int frame_height;    // input frame height
    int frame_bitdepth;  // pixel bit depth, 1: 10 bit    0: 8 bit
    int bd_det_en;
    int buffer_init_bits;
    int buffer_fullness_thr;
    int budget_mb_bits;
    int rm_debug_en;  // 1: rm debug enable     0: rm debug disable
} dcmp_hqv4_alone_parm_t;
typedef struct {
    int frame_width;      // input frame width
    int frame_height;     // input frame height
    int frame_bitdepth;   // pixel bit depth, 1: 10 bit    0: 8 bit
    int cmp_mode;         // 1: is raw frame compress mode             0: is not ...
    int is_lossless;      // 1: is lossless compress                   0: is lossy compress
    int pixel_format;     // sample mode 0: YUV422  1: YUV420
    int part_cmp_en;      // 0: compress total frame                   1: compress partial frame
    int pcmp_start_hpos;  // start horizontal position of partial cmp
    int pcmp_end_hpos;    // end horizontal position of partial cmp
    int slice_cmp_en;     // 1: slice compress enable   0: slice compress disable
    int start_slice_idx;  // dcmp start slice index

    int comp_ratio_int;  // luma&chroma compression ratio, (=comp_ratio * 1000)

    int rm_debug_en;   // RM debug enable control   0: disable    1: enable
    int rc_type_mode;  // 0: default    1: random     2: max value    3: min value   4: max/min combination
} cmp_hqv4_para_input_t;

typedef struct {
    int frame_width;        // input frame width
    int frame_height;       // input frame height
    int frame_bitdepth;     // pixel bit depth, 1: 10 bit    0: 8 bit
    int cmp_mode;           // 1: is raw frame compress mode             0: is not ...
    int is_lossless;        // 1: is lossless compress                   0: is lossy compress
    int pixel_format;       // sample mode 0: YUV422  1: YUV420
    int part_cmp_en;        // 0: compress total frame                   1: compress partial frame
    int pcmp_start_hpos_y;  // luma start horizontal position of partial cmp
    int pcmp_end_hpos_y;    // luma end horizontal position of partial cmp
    int pcmp_start_hpos_c;  // chroma start horizontal position of partial cmp
    int pcmp_end_hpos_c;    // chroma end horizontal position of partial cmp

    int comp_ratio_y_int;  // luma compression ratio, (=comp_ratio_y * 1000)
    int comp_ratio_c_int;  // chroma compression ratio, (=comp_ratio_c * 1000)

    int rm_debug_en;   // RM debug enable control   0: disable    1: enable
    int rc_type_mode;  // 0: default    1: random     2: max value    3: min value   4: max/min combination
} cmp_hqv4_alone_para_input_t;

// merged YC
void vpss_cmphqv4_compresser_init(cmp_hqv4_parm_t *cmp_parm, cmp_hqv4_para_input_t *para_input_ptr);

void vpss_cmphqv4_rand_reg_assert(cmp_hqv4_parm_t *cmp_parm);

// alone YC
void vpss_cmphqv4_compresser_init_y(cmp_hqv4_alone_parm_t *cmp_parm, cmp_hqv4_alone_para_input_t *para_input_ptr);

void vpss_cmphqv4_compresser_init_c(cmp_hqv4_alone_parm_t *cmp_parm, cmp_hqv4_alone_para_input_t *para_input_ptr);

void vpss_cmphqv4_decompress_init_alone(dcmp_hqv4_alone_parm_t *dcmp_parm, cmp_hqv4_alone_parm_t *cmp_parm,
                                        cmp_hqv4_alone_para_input_t *para_input_ptr);

void vpss_cmphqv4_rand_reg_assert_alone_yc(cmp_hqv4_alone_parm_t *cmp_parm_y, cmp_hqv4_alone_parm_t *cmp_parm_c);

#ifdef __cplusplus
}
#endif

#endif

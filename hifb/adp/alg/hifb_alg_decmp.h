/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hifb alg decompress header
 * Author: sdk
 * Create: 2019-08-16
 */

#ifndef __DRV_HIFB_ALG_DECMP_H__
#define __DRV_HIFB_ALG_DECMP_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_u32 is_lossless;
    hi_u32 cmp_mode;
    hi_u32 pix_format;
    hi_u32 bit_depth; /* 0 -- 8bit 1 --10bit */
    hi_u32 conv_en;
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 rm_debug_en;
    hi_u32 smth_thr;
    hi_u32 still_thr;
    hi_u32 big_grad_thr;
    hi_u32 diff_thr;
    hi_u32 smth_pix_num_thr;
    hi_u32 still_pix_num_thr;
    hi_u32 noise_pix_num_thr;
    hi_u32 grph_group_num_thr;
    hi_u32 qp_inc1_bits_thr;
    hi_u32 qp_inc2_bits_thr;
    hi_u32 qp_dec1_bits_thr;
    hi_u32 qp_dec2_bits_thr;
    hi_u32 raw_bits_penalty;
    hi_u32 buf_fullness_thr_reg0;
    hi_u32 buf_fullness_thr_reg1;
    hi_u32 buf_fullness_thr_reg2;
    hi_u32 qp_rge_reg0;
    hi_u32 qp_rge_reg1;
    hi_u32 qp_rge_reg2;
    hi_u32 bits_offset_reg0;
    hi_u32 bits_offset_reg1;
    hi_u32 bits_offset_reg2;
    hi_u32 grph_loss_thr;
    hi_u32 est_err_gain_map;
    hi_u32 buffer_size;
    hi_u32 buffer_init_bits;
    hi_u32 smooth_status_thr;
    hi_u32 budget_mb_bits;
    hi_u32 budget_mb_bits_last;
    hi_u32 min_mb_bits;
    hi_u32 max_mb_qp;

    hi_u32 bs_size; /* only use for RM */
    hi_u32 max_left_bits_buffer_enc;
    hi_u32 max_left_bits_buffer_dec;
    float min_line_ratio;
    hi_u32 reserve_para0;
} ice_v3r2_line_osd_cfg;

typedef struct {
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 pix_format; /* 0 --rgb 1 --argb */
    hi_u32 bit_depth;  /* 0 -- 8bit 1 --10bit */
    hi_u32 conv_en;
    hi_u32 cmp_mode;       /* 0 -- cmp 1 -- bypass */
    hi_u32 is_lossless;    /* 1: is lossless compress     0: is lossy compress */
    hi_s32 comp_ratio_int; /* luma&chroma compression ratio, (=comp_ratio * 1000) */
    hi_u32 rc_type_mode;   /* 0: default    1: random     2: max value    3: min value   4: max/min combination */
} ice_v3r2_line_osd_input;

hi_void ice_v3r2_line_osd_cfg_init(ice_v3r2_line_osd_cfg *rm_cfg, ice_v3r2_line_osd_input *input);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

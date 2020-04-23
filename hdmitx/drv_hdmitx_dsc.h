/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver dsc pps compute header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMITX_DSC_H__
#define __DRV_HDMITX_DSC_H__

#include <linux/types.h>
#include <hi_type.h>

#define NUM_BUF_RANGES         15
#define LSB                    64
#define OFFSET_FRACTIONAL_BITS 11
#define NUM_COMPONENTS         4
#define RANGE_BPG_OFFSET_MASK  0x3f

struct para_input_s {
    hi_u8 is_encoder;         /* 1-encoder 0-decoder */
    hi_u8 convert_rgb;        /* 1-rgb be change 0-rgb not change */
    hi_u8 native_420;         /* yuv420 */
    hi_u8 native_422;         /* yuv422 */
    hi_u8 native_444;         /* yuv444 */
    hi_u8 bits_per_component; /* bit depth,8/10/12 */
    hi_u16 bits_per_pixel;
    hi_u16 pic_width;    /* picture width */
    hi_u16 pic_height;   /* picture height */
    hi_u16 slice_width;  /* slice width */
    hi_u16 slice_height; /* slice height */
};

struct dsc_range_cfg_s {
    hi_u8 range_min_qp; /* Min QP allowed for this range */
    hi_u8 range_max_qp; /* Max QP allowed for this range */
    hi_u8 range_bpg_offset;
};

struct dsc_cfg_s {
    hi_u8 linebuf_depth;
    hi_u16 rcb_bits;
    hi_u8 bits_per_component;
    hi_u8 convert_rgb;
    hi_u16 slice_width;
    hi_u16 slice_height;
    hi_u8 simple_422;
    hi_u8 native_422;
    hi_u8 native_420;
    hi_u8 native_444;
    hi_u16 pic_width;
    hi_u16 pic_height;
    hi_u8 rc_tgt_offset_hi;
    hi_u8 rc_tgt_offset_lo;
    hi_u16 bits_per_pixel;
    hi_u8 rc_edge_factor;
    hi_u8 rc_quant_incr_limit1;
    hi_u8 rc_quant_incr_limit0;
    hi_u16 initial_xmit_delay;
    hi_u16 initial_dec_delay;
    hi_u8 block_pred_enable;
    hi_u8 first_line_bpg_ofs;
    hi_u8 second_line_bpg_ofs;
    hi_u16 initial_offset;
    hi_s32 xstart;
    hi_s32 ystart;
    hi_u16 rc_buf_thresh[NUM_BUF_RANGES - 1];
    struct dsc_range_cfg_s rc_range_parameters[NUM_BUF_RANGES];
    hi_u16 rc_model_size;
    hi_u8 flatness_min_qp;
    hi_u8 flatness_max_qp;
    hi_u8 flatness_det_thresh;
    hi_u8 flatness_mod;
    hi_u8 initial_scale_value;
    hi_u16 scale_decrement_interval;
    hi_u16 scale_increment_interval;
    hi_u16 nfl_bpg_offset;
    hi_u16 nsl_bpg_offset;
    hi_u16 slice_bpg_offset;
    hi_u16 final_offset;
    hi_u8 vbr_enable;
    hi_u8 mux_word_size;
    hi_u16 chunk_size;
    hi_u8 pps_identifier;
    hi_u8 very_flat_qp;
    hi_u8 somewhat_flat_qp_delta;
    hi_u8 somewhat_flat_qp_thresh;
    hi_u16 second_line_ofs_adj;
    hi_u8 dsc_version_minor;
    hi_u16 full_ich_err_precision;
};

void printk_cfg(struct dsc_cfg_s *cfg);
void hi_dsc_cfg_compute(struct para_input_s *para_input,
                        struct dsc_cfg_s *tc_cfg);

#endif


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_HDR_H__
#define __HAL_VDP_IP_HDR_H__

#include "vdp_chip_define.h"

typedef struct {
    hi_bool enable;
    hi_u32 cm_pos;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_u32 demo_en;
    hi_u32 bitdepth_out_mode;
    hi_u32 bitdepth_in_mode;
} vdp_cacm_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 tm_pos;
    hi_u32 scale_mix_alpha;
    hi_u32 mix_alpha;
    hi_u32 clip_min;
    hi_u32 clip_max;
    hi_u32 step[8]; /* 8 is number */
    hi_u32 pos[8]; /* 8 is number */
    hi_u32 num[8]; /* 8 is number */
    hi_u32 lum_cal[3]; /* 3 is number */
    hi_u32 scale_lum_cal;
    hi_u32 scale_coef;
    hi_s32 dc_out[3]; /* 3 is number */
} vdp_tmap_v1_cfg;

typedef struct {
    hi_bool enable;
    hi_s32 coef[3][3]; /* 3 is number */
    hi_u32 scale2p;

    hi_s32 clip_min_y;
    hi_s32 clip_max_y;
    hi_s32 clip_min_c;
    hi_s32 clip_max_c;

    hi_s32 dc_out[3]; /* 3 is number */
    hi_s32 dc_in[3]; /* 3 is number */
} vdp_lms2ipt_cfg;

typedef struct {
    hi_bool enable;
    hi_s32 coef[3][3]; /* 3 is number */

    hi_u32 scale2p;
    hi_u32 clip_max;
    hi_u32 clip_min;
    hi_s32 dc_in[3]; /* 3 is number */
} vdp_rgb2lms_cfg;

typedef struct {
    hi_bool enable;
    hi_bool v1_enable;
    hi_bool v0_enable;
    hi_u32 mode;

    hi_u32 scale2p;
    hi_s32 dc_in[3]; /* 3 is number */
    hi_s32 dc_in2[3]; /* 3 is number */
    hi_s32 offinrgb[3]; /* 3 is number */
    hi_s32 offinrgb2[3]; /* 3 is number */
    hi_u32 clip_max;
    hi_u32 clip_min;
    hi_u32 thr_r;
    hi_u32 thr_b;

    hi_s32 coef[3][3]; /* 3 is number */
    hi_s32 coef2[3][3]; /* 3 is number */
} vdp_yuv2rgb_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 clip_c_max;
    hi_u32 clip_c_min;
    hi_u32 scale2p;
    hi_u32 step[8]; /* 8 is number */
    hi_u32 pos[8]; /* 8 is number */
    hi_u32 num[8]; /* 8 is number */
} vdp_cm_cfg;

typedef struct {
    hi_bool enable;
    hi_bool v1_hdr_enable;
    hi_bool v0_hdr_enable;
    hi_bool rshift_round_en;
    hi_bool rshift_en;
    hi_bool ipt_in_sel;
    hi_bool ladj_en;
    hi_bool gamma_en;
    hi_bool degamma_en;
    hi_bool norm_en;

    hi_bool demo_luma_en;
    hi_u32 demo_luma_mode;

    hi_u32 in_width;
    hi_u32 in_height;

    hi_u32 in_color;
    hi_u32 in_bits;

    hi_u32 eotf;
    hi_u32 range_min;
    hi_u32 range;
    hi_u32 range_inv;

    hi_s32 ipt_off[3]; /* 3 is number */
    hi_s32 ipt_scale;
    hi_u32 degamma_clip_max;
    hi_u32 degamma_clip_min;
    hi_u32 ladj_chroma_weight;

    vdp_cacm_cfg cacm;
    vdp_tmap_v1_cfg tmap_v1;
    vdp_yuv2rgb_cfg yuv2rgb;
    vdp_rgb2lms_cfg rgb2lms;
    vdp_lms2ipt_cfg lms2ipt;
} vdp_imap_cfg;

typedef struct {
    hi_bool enable;
    hi_s16 coef[3][3]; /* 3 is number */
    hi_u32 scale2p;
    hi_s32 out_dc[3]; /* 3 is number */
    hi_s32 in_dc[3]; /* 3 is number */
    hi_u32 min;
    hi_u32 max;
} vdp_rgb2yuv_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 min;
    hi_u32 max;
    hi_u32 scale2p;
    hi_s32 out_dc[3]; /* 3 is number */
    hi_s32 in_dc[3]; /* 3 is number */
    hi_s16 coef[3][3]; /* 3 is number */
} vdp_ipt2lms_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 min;
    hi_u32 max;
    hi_u32 scale2p;
    hi_s16 coef[3][3]; /* 3 is number */
} vdp_lms2rgb_cfg;

typedef struct {
    hi_bool enable;

    hi_bool ipt_out_en;
    hi_bool u2s_enable;
    hi_bool lshift_en;
    hi_bool degamma_en;
    hi_bool gamma_en;
    hi_bool denorm_en;
    hi_bool cvm_en;

    hi_u32 range_min;
    hi_u32 range_over;
    hi_u32 out_bits;
    hi_u32 out_color;
    hi_u32 scale;
    hi_u32 oetf;
    hi_s32 ipt_off[3]; /* 3 is number */

    vdp_lms2rgb_cfg lms2rgb;
    vdp_ipt2lms_cfg ipt2lms;
    vdp_rgb2yuv_cfg rgb2yuv;

    vdp_cacm_cfg cm_cfg;
} vdp_omap_cfg;

typedef struct {
    hi_bool enable;
    hi_bool rshift_round_en;
    hi_bool sclut_rd_en;
    hi_bool sslut_rd_en;
    hi_bool tslut_rd_en;
    hi_bool silut_rd_en;
    hi_bool tilut_rd_en;
    hi_bool smc_enable;
    hi_bool rshift_en;
    hi_bool s2u_en;

    hi_u32 c1_expan;
    hi_u32 para_rdata;
    hi_u32 rshift_bit;
} vdp_tmap_cfg;

typedef struct {
    hi_u64 hdr1_addr;
    hi_u64 hdr2_addr;
    hi_u64 hdr3_addr;
    hi_u64 hdr6_addr;

    hi_u64 degamm_addr;
    hi_u64 gamm_addr;
    hi_u64 cm_addr;
} vdp_hdr_coef_addr;

typedef struct {
    hi_bool comp_enable;
    hi_bool stb_enable;
    hi_u32 mode;
    hi_u32 bit_depth;

    hi_u32 max;
    hi_u32 min;

    hi_bool bl_enable;
    hi_u32 bl_bit_depth;
    hi_u32 bl_num_pivots;

    hi_u32 bl_num_pivots_u;
    hi_u32 bl_num_pivots_v;
    hi_u32 mapping_idc_u;
    hi_u32 mapping_idc_v;

    hi_bool el_enable;
    hi_bool el_zme_en;
    hi_u32 el_bit_depth;

    hi_u32 bl_pivot_value_y[9]; /* 9 is number */
    hi_u32 bl_pivot_value_u[5]; /* 5 is number */
    hi_u32 bl_pivot_value_v[5]; /* 5 is number */

    hi_u32 bl_poly_order_y[8]; /* 8 is number */
    hi_u32 bl_poly_order_u[4]; /* 4 is number */
    hi_u32 bl_poly_order_v[4]; /* 4 is number */

    hi_s32 bl_coef_y[8][3]; /* 8, 3 is number */
    hi_s32 bl_coef_u[4][3]; /* 4, 3 is number */
    hi_s32 bl_coef_v[4][3]; /* 4, 3 is number */

    hi_u32 bl_mmr_order_u;
    hi_u32 bl_mmr_order_v;

    hi_s32 bl_mmr_coef_u[22]; /* 22 is number */
    hi_s32 bl_mmr_coef_v[22]; /* 22 is number */

    hi_u32 el_nldq_offset_y;
    hi_u32 el_nldq_offset_u;
    hi_u32 el_nldq_offset_v;

    hi_s32 el_nldq_coef_y[3]; /* 3 is number */
    hi_s32 el_nldq_coef_u[3]; /* 3 is number */
    hi_s32 el_nldq_coef_v[3]; /* 3 is number */
} hal_composer_info;

typedef struct {
    hi_bool enable;

    hi_u32 yuv_range;
    hi_u32 alpha_a;
    hi_u32 alpha_b;

    hi_u32 oct[7]; /* 7 is number */
    hi_u32 d_lut_step[4]; /* 4 is number */
    hi_u32 d_lut_thr[3]; /* 3 is number */

    hi_bool r2y_clip_en;
    hi_u32 r2y_coef[9]; /* 9 is number */
    hi_u32 r2y_scale2p;
    hi_u32 r2y_out_dc[3]; /* 3 is number */
    hi_u32 r2y_clip_min;
    hi_u32 r2y_clip_max;

    hi_bool demo_en;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
} hal_tc_cfg;

typedef struct {
    hal_tc_cfg tc_cfg;
    hi_u64 hdr0_addr;
} hal_tecnicolor_info;

typedef struct {
    hi_bool hdr_enable;

    hi_u32 hdr_link;
    hi_u32 omap_link;

    vdp_cm_cfg cm_cfg;
    vdp_imap_cfg imap_cfg;
    vdp_tmap_cfg tmap_cfg;
    vdp_omap_cfg omap_cfg;

    vdp_hdr_coef_addr coef_addr;
} hal_hdr_info;

hi_void vdp_ip_hdr_set(hi_u32 layer, hal_hdr_info* hdr_info);
hi_void vdp_ip_composer_set(hi_u32 layer, hal_composer_info* comp_info);
hi_void vdp_ip_tecnicolor_set(hi_u32 layer, hal_tecnicolor_info* tc_info);

#endif


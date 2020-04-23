/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_pq_comm.h"

#ifndef __PQ_HAL_VPSSHDR_REGSET_H__
#define __PQ_HAL_VPSSHDR_REGSET_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void vpss_hihdr_v2_set_im_cm_pos(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_pos);
hi_void vpss_hihdr_v2_set_im_tm_pos(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_pos);
hi_void vpss_hihdr_v2_set_im_rshift_round_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 im_rshift_round_en);
hi_void vpss_hihdr_v2_set_im_rshift_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_rshift_en);
hi_void vpss_hihdr_v2_set_im_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_ck_gt_en);
hi_void vpss_hihdr_v2_set_im_ipt_in_sel(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_ipt_in_sel);
hi_void vpss_hihdr_v2_set_im_ladj_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_ladj_en);
hi_void vpss_hihdr_v2_set_imap_eotfparam_eotf(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 imap_eotfparam_eotf);
hi_void vpss_hihdr_v2_set_imap_in_color(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_in_color);
hi_void vpss_hihdr_v2_set_im_in_bits(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_in_bits);
hi_void vpss_hihdr_v2_set_im_gamma_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_gamma_en);
hi_void vpss_hihdr_v2_set_im_degamma_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_degamma_en);
hi_void vpss_hihdr_v2_set_im_norm_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_norm_en);
hi_void vpss_hihdr_v2_set_im_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_en);
hi_void vpss_hihdr_v2_set_imap_eotfparam_range_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_eotfparam_range_min);
hi_void vpss_hihdr_v2_set_imap_eotfparam_range(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 imap_eotfparam_range);
hi_void vpss_hihdr_v2_set_imap_eotfparam_range_inv(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_eotfparam_range_inv);
hi_void vpss_hihdr_v2_set_imap_v3ipt_off0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_v3ipt_off_0);
hi_void vpss_hihdr_v2_set_imap_v3ipt_off1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_v3ipt_off_1);
hi_void vpss_hihdr_v2_set_imap_v3ipt_off2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_v3ipt_off_2);
hi_void vpss_hihdr_v2_set_imap_ipt_scale(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_ipt_scale);
hi_void vpss_hihdr_v2_set_degamma_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 degamma_rd_en);
hi_void vpss_hihdr_v2_set_gamma_lutb_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 gamma_lutb_rd_en);
hi_void vpss_hihdr_v2_set_gamma_luta_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 gamma_luta_rd_en);
hi_void vpss_hihdr_v2_set_gamma_lutx_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 gamma_lutx_rd_en);
hi_void vpss_hihdr_v2_set_vdm_para_data(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vdm_para_data);
hi_void vpss_hihdr_v2_set_vdm_coef1_upd(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vdm_coef1_upd);
hi_void vpss_hihdr_v2_set_vdm_coef_upd(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vdm_coef_upd);
hi_void vpss_hihdr_v2_set_im_degamma_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_degamma_clip_max);
hi_void vpss_hihdr_v2_set_im_degamma_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_degamma_clip_min);
hi_void vpss_hihdr_v2_set_imap_ladj_chroma_weight(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_ladj_chroma_weight);
hi_void vpss_hihdr_v2_set_im_demo_luma_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_demo_luma_mode);
hi_void vpss_hihdr_v2_set_im_demo_luma_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_demo_luma_en);
hi_void vpss_hihdr_v2_set_im_v1_hdr_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v1_hdr_en);
hi_void vpss_hihdr_v2_set_im_v0_hdr_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v0_hdr_en);
hi_void vpss_hihdr_v2_set_im_yuv2rgb_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_mode);
hi_void vpss_hihdr_v2_set_im_v1_y2r_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v1_y2r_en);
hi_void vpss_hihdr_v2_set_im_v0_y2r_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v0_y2r_en);
hi_void vpss_hihdr_v2_set_im_yuv2rgb_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_en);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb00(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_00);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb01(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_01);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb02(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_02);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb10(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_10);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb11(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_11);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb12(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_12);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb20(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_20);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb21(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_21);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb22(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33yuv2rgb_22);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb200(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_00);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb201(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_01);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb202(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_02);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb210(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_10);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb211(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_11);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb212(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_12);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb220(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_20);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb221(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_21);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb222(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_22);
hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb_scale2p(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_m33yuv2rgb_scale2p);
hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_yuv2rgb_dc_in_0);
hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_yuv2rgb_dc_in_1);
hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_yuv2rgb_dc_in_2);
hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_v3yuv2rgb_offinrgb_0);
hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_v3yuv2rgb_offinrgb_1);
hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_v3yuv2rgb_offinrgb_2);
hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_yuv2rgb2_dc_in_0);
hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_yuv2rgb2_dc_in_1);
hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_yuv2rgb2_dc_in_2);
hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                    hi_u32 imap_v3yuv2rgb2_offinrgb_0);
hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                    hi_u32 imap_v3yuv2rgb2_offinrgb_1);
hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                    hi_u32 imap_v3yuv2rgb2_offinrgb_2);
hi_void vpss_hihdr_v2_set_im_yuv2rgb_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_yuv2rgb_clip_min);
hi_void vpss_hihdr_v2_set_im_yuv2rgb_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_yuv2rgb_clip_max);
hi_void vpss_hihdr_v2_set_im_yuv2rgb_thr_r(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_thr_r);
hi_void vpss_hihdr_v2_set_im_yuv2rgb_thr_b(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_thr_b);
hi_void vpss_hihdr_v2_set_im_rgb2lms_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_rgb2lms_en);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms00(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_00);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms01(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_01);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms02(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_02);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms10(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_10);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms11(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_11);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms12(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_12);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms20(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_20);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms21(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_21);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms22(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33rgb2lms_22);
hi_void vpss_hihdr_v2_set_imap_m33rgb2lms_scale2p(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_m33rgb2lms_scale2p);
hi_void vpss_hihdr_v2_set_im_rgb2lms_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_rgb2lms_clip_min);
hi_void vpss_hihdr_v2_set_im_rgb2lms_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_rgb2lms_clip_max);
hi_void vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_rgb2lms_dc_in_0);
hi_void vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_rgb2lms_dc_in_1);
hi_void vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_rgb2lms_dc_in_2);
hi_void vpss_hihdr_v2_set_im_lms2ipt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_lms2ipt_en);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt00(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_00);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt01(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_01);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt02(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_02);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt10(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_10);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt11(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_11);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt12(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_12);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt20(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_20);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt21(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_21);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt22(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 imap_m33lms2ipt_22);
hi_void vpss_hihdr_v2_set_imap_m33lms2ipt_scale2p(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_m33lms2ipt_scale2p);
hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_min_y(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_min_y);
hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_max_y(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_max_y);
hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_min_c(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_min_c);
hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_max_c(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_max_c);
hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_lms2ipt_dc_out_0);
hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_lms2ipt_dc_out_1);
hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_lms2ipt_dc_out_2);
hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_lms2ipt_dc_in_0);
hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_lms2ipt_dc_in_1);
hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_lms2ipt_dc_in_2);
hi_void vpss_hihdr_v2_set_im_tm_v1_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_v1_en);
hi_void vpss_hihdr_v2_set_vhdr_tmap_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vhdr_tmap_rd_en);
hi_void vpss_hihdr_v2_set_vhdr_tmap_para_data(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 vhdr_tmap_para_data);
hi_void vpss_hihdr_v2_set_im_tm_scale_mix_alpha(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_tm_scale_mix_alpha);
hi_void vpss_hihdr_v2_set_im_tm_mix_alpha(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_mix_alpha);
hi_void vpss_hihdr_v2_set_im_tm_xstep3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_3);
hi_void vpss_hihdr_v2_set_im_tm_xstep2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_2);
hi_void vpss_hihdr_v2_set_im_tm_xstep1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_1);
hi_void vpss_hihdr_v2_set_im_tm_xstep0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_0);
hi_void vpss_hihdr_v2_set_im_tm_xstep7(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_7);
hi_void vpss_hihdr_v2_set_im_tm_xstep6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_6);
hi_void vpss_hihdr_v2_set_im_tm_xstep5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_5);
hi_void vpss_hihdr_v2_set_im_tm_xstep4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_4);
hi_void vpss_hihdr_v2_set_im_tm_xpos0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_0);
hi_void vpss_hihdr_v2_set_im_tm_xpos1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_1);
hi_void vpss_hihdr_v2_set_im_tm_xpos2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_2);
hi_void vpss_hihdr_v2_set_im_tm_xpos3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_3);
hi_void vpss_hihdr_v2_set_im_tm_xpos4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_4);
hi_void vpss_hihdr_v2_set_im_tm_xpos5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_5);
hi_void vpss_hihdr_v2_set_im_tm_xpos6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_6);
hi_void vpss_hihdr_v2_set_im_tm_xpos7(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_7);
hi_void vpss_hihdr_v2_set_im_tm_xnum3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_3);
hi_void vpss_hihdr_v2_set_im_tm_xnum2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_2);
hi_void vpss_hihdr_v2_set_im_tm_xnum1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_1);
hi_void vpss_hihdr_v2_set_im_tm_xnum0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_0);
hi_void vpss_hihdr_v2_set_im_tm_xnum7(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_7);
hi_void vpss_hihdr_v2_set_im_tm_xnum6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_6);
hi_void vpss_hihdr_v2_set_im_tm_xnum5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_5);
hi_void vpss_hihdr_v2_set_im_tm_xnum4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_4);
hi_void vpss_hihdr_v2_set_im_tm_m3_lum_cal0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 im_tm_m3_lum_cal_0);
hi_void vpss_hihdr_v2_set_im_tm_m3_lum_cal1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 im_tm_m3_lum_cal_1);
hi_void vpss_hihdr_v2_set_im_tm_m3_lum_cal2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                            hi_u32 im_tm_m3_lum_cal_2);
hi_void vpss_hihdr_v2_set_im_tm_scale_lum_cal(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_tm_scale_lum_cal);
hi_void vpss_hihdr_v2_set_im_tm_scale_coef(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_scale_coef);
hi_void vpss_hihdr_v2_set_im_tm_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_clip_min);
hi_void vpss_hihdr_v2_set_im_tm_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_clip_max);
hi_void vpss_hihdr_v2_set_im_tm_dc_out0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_dc_out_0);
hi_void vpss_hihdr_v2_set_im_tm_dc_out1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_dc_out_1);
hi_void vpss_hihdr_v2_set_im_tm_dc_out2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_dc_out_2);
hi_void vpss_hihdr_v2_set_im_cm_demo_pos(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_demo_pos);
hi_void vpss_hihdr_v2_set_im_cm_demo_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_demo_mode);
hi_void vpss_hihdr_v2_set_im_cm_demo_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_demo_en);
hi_void vpss_hihdr_v2_set_im_cm_bitdepth_out_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 im_cm_bitdepth_out_mode);
hi_void vpss_hihdr_v2_set_im_cm_bitdepth_in_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                 hi_u32 im_cm_bitdepth_in_mode);
hi_void vpss_hihdr_v2_set_im_cm_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_ck_gt_en);
hi_void vpss_hihdr_v2_set_im_cm_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_en);
hi_void vpss_hihdr_v2_set_im_cm_clut_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_clut_rd_en);
hi_void vpss_hihdr_v2_set_im_cm_clut_rdata_rg(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_cm_clut_rdata_rg);
hi_void vpss_hihdr_v2_set_im_cm_clut_rdata_b(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 im_cm_clut_rdata_b);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

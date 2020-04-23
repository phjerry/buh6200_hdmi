/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr regset common api
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __HAL_PQ_HDRV2_REGSET_COMM_H__
#define __HAL_PQ_HDRV2_REGSET_COMM_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_pq_comm.h"
#include "hi_register_vdp.h"

hi_void pq_hihdr_v2_setimcmpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_pos);
hi_void pq_hihdr_v2_setimtmpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_pos);
hi_void pq_hihdr_v2_setimrshiftrounden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rshift_round_en);
hi_void pq_hihdr_v2_setimrshiften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rshift_en);
hi_void pq_hihdr_v2_setimckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_ck_gt_en);
hi_void pq_hihdr_v2_setimiptinsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_ipt_in_sel);
hi_void pq_hihdr_v2_setimladjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_ladj_en);
hi_void pq_hihdr_v2_setimapeotfparameotf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_eotf);
hi_void pq_hihdr_v2_setimapincolor(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_in_color);
hi_void pq_hihdr_v2_setiminbits(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_in_bits);
hi_void pq_hihdr_v2_setimgammaen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_gamma_en);
hi_void pq_hihdr_v2_setimdegammaen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_degamma_en);
hi_void pq_hihdr_v2_setimnormen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_norm_en);
hi_void pq_hihdr_v2_setimen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_en);
hi_void pq_hihdr_v2_setimapeotfparamrangemin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_range_min);
hi_void pq_hihdr_v2_setimapeotfparamrange(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_range);
hi_void pq_hihdr_v2_setimapeotfparamrangeinv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_range_inv);
hi_void pq_hihdr_v2_setimapv3iptoff0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3ipt_off_0);
hi_void pq_hihdr_v2_setimapv3iptoff1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3ipt_off_1);
hi_void pq_hihdr_v2_setimapv3iptoff2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3ipt_off_2);
hi_void pq_hihdr_v2_setimapiptscale(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_ipt_scale);
hi_void pq_hihdr_v2_setdegammarden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 degamma_rd_en);
hi_void pq_hihdr_v2_setgammalutbrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gamma_lutb_rd_en);
hi_void pq_hihdr_v2_setgammalutarden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gamma_luta_rd_en);
hi_void pq_hihdr_v2_setgammalutxrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gamma_lutx_rd_en);
hi_void pq_hihdr_v2_setvdmparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdm_para_data);
hi_void pq_hihdr_v2_setvdmcoef1upd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdm_coef1_upd);
hi_void pq_hihdr_v2_setvdmcoefupd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdm_coef_upd);
hi_void pq_hihdr_v2_setimdegammaclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_degamma_clip_max);
hi_void pq_hihdr_v2_setimdegammaclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_degamma_clip_min);
hi_void pq_hihdr_v2_setimapladjchromaweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_ladj_chroma_weight);
hi_void pq_hihdr_v2_setimdemolumamode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_demo_luma_mode);
hi_void pq_hihdr_v2_setimdemolumaen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_demo_luma_en);
hi_void pq_hihdr_v2_setimv1hdren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v1_hdr_en);
hi_void pq_hihdr_v2_setimv0hdren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v0_hdr_en);
hi_void pq_hihdr_v2_setimyuv2rgbmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_mode);
hi_void pq_hihdr_v2_setimv1y2ren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v1_y2r_en);
hi_void pq_hihdr_v2_setimv0y2ren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v0_y2r_en);
hi_void pq_hihdr_v2_setimyuv2rgben(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_en);
hi_void pq_hihdr_v2_setimapm33yuv2rgb00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_00);
hi_void pq_hihdr_v2_setimapm33yuv2rgb01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_01);
hi_void pq_hihdr_v2_setimapm33yuv2rgb02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_02);
hi_void pq_hihdr_v2_setimapm33yuv2rgb10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_10);
hi_void pq_hihdr_v2_setimapm33yuv2rgb11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_11);
hi_void pq_hihdr_v2_setimapm33yuv2rgb12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_12);
hi_void pq_hihdr_v2_setimapm33yuv2rgb20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_20);
hi_void pq_hihdr_v2_setimapm33yuv2rgb21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_21);
hi_void pq_hihdr_v2_setimapm33yuv2rgb22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_22);
hi_void pq_hihdr_v2_setimapm33yuv2rgb200(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_00);
hi_void pq_hihdr_v2_setimapm33yuv2rgb201(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_01);
hi_void pq_hihdr_v2_setimapm33yuv2rgb202(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_02);
hi_void pq_hihdr_v2_setimapm33yuv2rgb210(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_10);
hi_void pq_hihdr_v2_setimapm33yuv2rgb211(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_11);
hi_void pq_hihdr_v2_setimapm33yuv2rgb212(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_12);
hi_void pq_hihdr_v2_setimapm33yuv2rgb220(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_20);
hi_void pq_hihdr_v2_setimapm33yuv2rgb221(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_21);
hi_void pq_hihdr_v2_setimapm33yuv2rgb222(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_22);
hi_void pq_hihdr_v2_setimapm33yuv2rgbscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_scale2p);
hi_void pq_hihdr_v2_setimv3yuv2rgbdcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb_dc_in_0);
hi_void pq_hihdr_v2_setimv3yuv2rgbdcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb_dc_in_1);
hi_void pq_hihdr_v2_setimv3yuv2rgbdcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb_dc_in_2);
hi_void pq_hihdr_v2_setimapv3yuv2rgboffinrgb0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3yuv2rgb_offinrgb_0);
hi_void pq_hihdr_v2_setimapv3yuv2rgboffinrgb1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3yuv2rgb_offinrgb_1);
hi_void pq_hihdr_v2_setimapv3yuv2rgboffinrgb2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3yuv2rgb_offinrgb_2);
hi_void pq_hihdr_v2_setimv3yuv2rgb2dcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb2_dc_in_0);
hi_void pq_hihdr_v2_setimv3yuv2rgb2dcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb2_dc_in_1);
hi_void pq_hihdr_v2_setimv3yuv2rgb2dcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb2_dc_in_2);
hi_void pq_hihdr_v2_setimapv3yuv2rgb2offinrgb0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data);
hi_void pq_hihdr_v2_setimapv3yuv2rgb2offinrgb1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data);
hi_void pq_hihdr_v2_setimapv3yuv2rgb2offinrgb2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data);
hi_void pq_hihdr_v2_setimyuv2rgbclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_clip_min);
hi_void pq_hihdr_v2_setimyuv2rgbclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_clip_max);
hi_void pq_hihdr_v2_setimyuv2rgbthrr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_thr_r);
hi_void pq_hihdr_v2_setimyuv2rgbthrb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_thr_b);
hi_void pq_hihdr_v2_setimrgb2lmsen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rgb2lms_en);
hi_void pq_hihdr_v2_setimapm33rgb2lms00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_00);
hi_void pq_hihdr_v2_setimapm33rgb2lms01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_01);
hi_void pq_hihdr_v2_setimapm33rgb2lms02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_02);
hi_void pq_hihdr_v2_setimapm33rgb2lms10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_10);
hi_void pq_hihdr_v2_setimapm33rgb2lms11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_11);
hi_void pq_hihdr_v2_setimapm33rgb2lms12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_12);
hi_void pq_hihdr_v2_setimapm33rgb2lms20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_20);
hi_void pq_hihdr_v2_setimapm33rgb2lms21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_21);
hi_void pq_hihdr_v2_setimapm33rgb2lms22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_22);
hi_void pq_hihdr_v2_setimapm33rgb2lmsscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_scale2p);
hi_void pq_hihdr_v2_setimrgb2lmsclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rgb2lms_clip_min);
hi_void pq_hihdr_v2_setimrgb2lmsclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rgb2lms_clip_max);
hi_void pq_hihdr_v2_setimv3rgb2lmsdcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_rgb2lms_dc_in_0);
hi_void pq_hihdr_v2_setimv3rgb2lmsdcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_rgb2lms_dc_in_1);
hi_void pq_hihdr_v2_setimv3rgb2lmsdcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_rgb2lms_dc_in_2);
hi_void pq_hihdr_v2_setimlms2ipten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_en);
hi_void pq_hihdr_v2_setimapm33lms2ipt00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_00);
hi_void pq_hihdr_v2_setimapm33lms2ipt01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_01);
hi_void pq_hihdr_v2_setimapm33lms2ipt02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_02);
hi_void pq_hihdr_v2_setimapm33lms2ipt10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_10);
hi_void pq_hihdr_v2_setimapm33lms2ipt11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_11);
hi_void pq_hihdr_v2_setimapm33lms2ipt12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_12);
hi_void pq_hihdr_v2_setimapm33lms2ipt20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_20);
hi_void pq_hihdr_v2_setimapm33lms2ipt21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_21);
hi_void pq_hihdr_v2_setimapm33lms2ipt22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_22);
hi_void pq_hihdr_v2_setimapm33lms2iptscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_scale2p);
hi_void pq_hihdr_v2_setimlms2iptclipminy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_min_y);
hi_void pq_hihdr_v2_setimlms2iptclipmaxy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_max_y);
hi_void pq_hihdr_v2_setimlms2iptclipminc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_min_c);
hi_void pq_hihdr_v2_setimlms2iptclipmaxc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_max_c);
hi_void pq_hihdr_v2_setimv3lms2iptdcout0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_out_0);
hi_void pq_hihdr_v2_setimv3lms2iptdcout1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_out_1);
hi_void pq_hihdr_v2_setimv3lms2iptdcout2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_out_2);
hi_void pq_hihdr_v2_setimv3lms2iptdcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_in_0);
hi_void pq_hihdr_v2_setimv3lms2iptdcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_in_1);
hi_void pq_hihdr_v2_setimv3lms2iptdcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_in_2);
hi_void pq_hihdr_v2_setimtmv1en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_v1_en);
hi_void pq_hihdr_v2_setvhdrtmaprden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_tmap_rd_en);
hi_void pq_hihdr_v2_setvhdrtmapparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_tmap_para_data);
hi_void pq_hihdr_v2_setimtmscalemixalpha(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_scale_mix_alpha);
hi_void pq_hihdr_v2_setimtmmixalpha(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_mix_alpha);
hi_void pq_hihdr_v2_setimtmxstep3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_3);
hi_void pq_hihdr_v2_setimtmxstep2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_2);
hi_void pq_hihdr_v2_setimtmxstep1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_1);
hi_void pq_hihdr_v2_setimtmxstep0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_0);
hi_void pq_hihdr_v2_setimtmxstep7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_7);
hi_void pq_hihdr_v2_setimtmxstep6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_6);
hi_void pq_hihdr_v2_setimtmxstep5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_5);
hi_void pq_hihdr_v2_setimtmxstep4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_4);
hi_void pq_hihdr_v2_setimtmxpos0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_0);
hi_void pq_hihdr_v2_setimtmxpos1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_1);
hi_void pq_hihdr_v2_setimtmxpos2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_2);
hi_void pq_hihdr_v2_setimtmxpos3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_3);
hi_void pq_hihdr_v2_setimtmxpos4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_4);
hi_void pq_hihdr_v2_setimtmxpos5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_5);
hi_void pq_hihdr_v2_setimtmxpos6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_6);
hi_void pq_hihdr_v2_setimtmxpos7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_7);
hi_void pq_hihdr_v2_setimtmxnum3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_3);
hi_void pq_hihdr_v2_setimtmxnum2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_2);
hi_void pq_hihdr_v2_setimtmxnum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_1);
hi_void pq_hihdr_v2_setimtmxnum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_0);
hi_void pq_hihdr_v2_setimtmxnum7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_7);
hi_void pq_hihdr_v2_setimtmxnum6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_6);
hi_void pq_hihdr_v2_setimtmxnum5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_5);
hi_void pq_hihdr_v2_setimtmxnum4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_4);
hi_void pq_hihdr_v2_setimtmm3lumcal0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_m3_lum_cal_0);
hi_void pq_hihdr_v2_setimtmm3lumcal1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_m3_lum_cal_1);
hi_void pq_hihdr_v2_setimtmm3lumcal2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_m3_lum_cal_2);
hi_void pq_hihdr_v2_setimtmscalelumcal(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_scale_lum_cal);
hi_void pq_hihdr_v2_setimtmscalecoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_scale_coef);
hi_void pq_hihdr_v2_setimtmclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_clip_min);
hi_void pq_hihdr_v2_setimtmclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_clip_max);
hi_void pq_hihdr_v2_setimtmdcout0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_dc_out_0);
hi_void pq_hihdr_v2_setimtmdcout1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_dc_out_1);
hi_void pq_hihdr_v2_setimtmdcout2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_dc_out_2);
hi_void pq_hihdr_v2_setimcmdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_demo_pos);
hi_void pq_hihdr_v2_setimcmdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_demo_mode);
hi_void pq_hihdr_v2_setimcmdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_demo_en);
hi_void pq_hihdr_v2_setimcmbitdepthoutmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_bitdepth_out_mode);
hi_void pq_hihdr_v2_setimcmbitdepthinmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_bitdepth_in_mode);
hi_void pq_hihdr_v2_setimcmckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_ck_gt_en);
hi_void pq_hihdr_v2_setimcmen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_en);
hi_void pq_hihdr_v2_setimcmclutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_clut_rd_en);
hi_void pq_hihdr_v2_setimcmclutrdatarg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_clut_rdata_rg);
hi_void pq_hihdr_v2_setimcmclutrdatab(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_clut_rdata_b);
#endif

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb hal header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_HAL_H__
#define __DRV_HIFB_HAL_H__

/* ********************************add include here********************************************* */
#include "optm_define.h"

/***********************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition *********************************************** */

/* ************************** Structure Definition ********************************************* */

/* **************************  The enum of Jpeg image format  ********************************** */

/* ********************* Global Variable declaration ******************************************* */

/* ****************************** API declaration ********************************************** */
hi_s32 drv_hifb_hal_vdp_initial(hi_void);
hi_void drv_hifb_hal_vdp_deinitial(hi_void);
hi_s32 hifb_check_address(hi_u32 address);

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_disp_get_dhd1_field(hi_bool *is_bottom_field);
hi_void drv_hifb_hal_disp_get_dhd0_vtthd3(hi_ulong *expect_int_line_nums_for_end_call_back);
hi_void drv_hifb_hal_disp_get_dhd0_vtthd(hi_ulong *expect_int_line_nums_for_vo_call_back);
hi_void drv_hifb_hal_disp_get_dhd0_state(hi_ulong *actual_int_line_nums_for_call_back,
                                         hi_ulong *hard_int_cnt_for_call_back);
hi_void drv_hifb_hal_gfx_get_work_layer_addr(hi_u32 data, hi_u32 *addr);
hi_void drv_hifb_hal_gfx_get_will_work_layer_addr(hi_u32 data, hi_u32 *addr);
#endif

hi_void drv_hifb_hal_gfx_set_interrupt(hi_bool open);

hi_void drv_hifb_hal_set_up_mute(hi_u32 layer_id, hi_bool mute);
hi_void drv_hifb_hal_get_up_mute(hi_u32 layer_id, hi_bool *mute);

// -------------------------------------------------------------------
// GFX_BEGIN
// -------------------------------------------------------------------
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_layer_addr(hi_u32 data, hi_u32 *addr);
hi_void drv_hifb_hal_gfx_get_layer_enable(hi_u32 data, hi_u32 *enable);
hi_void drv_hifb_hal_gfx_get_layer_in_rect(hi_u32 data, OPTM_VDP_DISP_RECT_S *in_rect);
hi_void drv_hifb_hal_gfx_get_layer_stride(hi_u32 data, hi_u32 *stride);
hi_void drv_hifb_hal_gfx_get_palpha(hi_u32 data, hi_u32 *alpha_en, hi_u32 *arange, hi_u8 *alpha0, hi_u8 *alpha1);
hi_void drv_hifb_hal_gfx_get_layer_galpha(hi_u32 data, hi_u8 *alpha0);
hi_void drv_hifb_hal_gfx_get_key_mask(hi_u32 data, OPTM_VDP_GFX_MASK_S *msk);
hi_void drv_hifb_hal_gfx_get_color_key(hi_u32 data, hi_u32 *pbkey_en, OPTM_VDP_GFX_CKEY_S *key);
hi_void drv_hifb_hal_gfx_get_in_data_fmt(hi_u32 data, OPTM_VDP_GFX_IFMT_E *data_fmt);
hi_void drv_hifb_hal_wbc_gp_get_enable(OPTM_VDP_LAYER_WBC_E layer, hi_u32 *enable);
#endif

hi_void drv_hifb_hal_cbm_get_mixer_prio(OPTM_VDP_CBM_MIX_E u32mixer_id, hi_u32 *pu32prio);
hi_void drv_hifb_hal_cbm_set_mixg_prio(OPTM_VDP_CBM_MIX_E u32mixer_id, hi_u32 u32prio);
hi_void drv_hifb_hal_gfx_set_layer_reso(hi_u32 data, OPTM_VDP_DISP_RECT_S *rect);
hi_void drv_hifb_hal_gfx_set_layer_enable(hi_u32 data, hi_u32 u32b_enable);
hi_void hifb_hal_set_video_enable(hi_u32 data, hi_u32 enable);
hi_void drv_hifb_hal_gfx_set_layer_addr_ex(hi_u32 data, hi_u32 l_addr);
hi_void drv_hifb_hal_gfx_set_layer_stride(hi_u32 data, hi_u32 stride);
hi_void drv_hifb_hal_gfx_set_in_data_fmt(hi_u32 data, OPTM_VDP_GFX_IFMT_E data_fmt);
hi_void drv_hifb_hal_gfx_set_read_mode(hi_u32 data, hi_u32 mode);
hi_void drv_hifb_hal_gfx_get_read_mode(hi_u32 data, hi_u32 *mode);
hi_void drv_hifb_hal_gfx_set_bit_extend(hi_u32 data, OPTM_VDP_GFX_BITEXTEND_E u32mode);
hi_void drv_hifb_hal_gfx_set_color_key(hi_u32 data, hi_u32 bkey_en, OPTM_VDP_GFX_CKEY_S *key);
hi_void drv_hifb_hal_gfx_set_key_mask(hi_u32 data, OPTM_VDP_GFX_MASK_S *msk);

hi_void drv_hifb_hal_gfx_set_para_upd(hi_u32 data, OPTM_VDP_DISP_COEFMODE_E mode);
hi_void drv_hifb_hal_gfx_set_lut_addr(hi_u32 data, hi_u32 lut_addr);
hi_void drv_hifb_hal_gfx_set_no_sec_flag(hi_u32 data, hi_u32 enable);
hi_void drv_hifb_hal_gfx_set_dcmp_enable(hi_u32 data, hi_u32 u32b_enable);
hi_void drv_hifb_hal_gfx_get_dcmp_enable(hi_u32 data, hi_u32 *enable);
hi_void drv_hifb_hal_gfx_set_de_cmp_ddr_info(hi_u32 data, hi_u32 ar_head_ddr, hi_u32 ar_data_ddr, hi_u32 gb_head_ddr,
                                             hi_u32 gb_data_ddr, hi_u32 de_cmp_stride);
hi_void drv_hifb_hal_gfx_get_de_cmp_status(hi_u32 data, hi_bool *is_ar_error, hi_bool *is_gb_error,
                                           hi_bool close_interrupt);

hi_void drv_hifb_hal_set_typ(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_max_mbqp(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_conv_en(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_pixel_fmt_od(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_is_lossless(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_cmp_mode(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_bit_depth(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_dcmp_type(hi_u32 layer_id, hi_u32 value);
hi_void drv_hifb_hal_set_ice_en(hi_u32 layer_id, hi_u32 value);

// 3D
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_set_three_dim_dof_enable(hi_u32 data, hi_u32 enable);
hi_void drv_hifb_hal_gfx_set_three_dim_dof_step(hi_u32 data, hi_s32 l_step, hi_s32 r_step);
hi_void drv_hifb_hal_gfx_set_low_power_info(hi_u32 data, hi_u32 *low_power_info);
hi_void drv_hifb_hal_gfx_enable_layer_low_power(hi_u32 data, hi_bool en_low_power);
hi_void drv_hifb_hal_gfx_enable_gp_low_power(hi_u32 data, hi_bool en_low_power);
#endif

hi_void drv_hifb_hal_gfx_get_pre_mult_enable(hi_u32 data, hi_u32 *enable, hi_bool *de_pre_mult);
hi_void drv_hifb_hal_gfx_set_pre_mult_enable(hi_u32 data, hi_u32 enable, hi_bool hdr);
hi_void drv_hifb_hal_gfx_set_layer_bkg(hi_u32 data, OPTM_VDP_BKG_S *bkg);
hi_void drv_hifb_hal_gfx_set_layer_galpha(hi_u32 data, hi_u32 alpha0);
hi_void drv_hifb_hal_gfx_set_palpha(hi_u32 data, hi_u32 alpha_en, hi_u32 arange, hi_u32 alpha0, hi_u32 alpha1);
hi_void drv_hifb_hal_gfx_read_register(hi_u32 offset, hi_u32 *reg_buf);
hi_void drv_hifb_hal_gfx_write_register(hi_u32 offset, hi_u32 value);

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_set_layer_naddr(hi_u32 data, hi_u32 n_addr);
#endif
hi_void drv_hifb_hal_gfx_set_upd_mode(hi_u32 data, hi_u32 mode);
hi_void drv_hifb_hal_gfx_set_reg_up(hi_u32 data);

// -------------------------------------------------------------------
// GP_BEGIN
// -------------------------------------------------------------------
hi_void hifb_hal_gp_hdr_set(hi_u32 data, hi_bool enable);

/* csc begin */
hi_void hifb_hal_set_gp_csc_coef00(hi_u32 data, hi_s32 csc_coef00);
hi_void hifb_hal_set_gp_csc_coef01(hi_u32 data, hi_s32 csc_coef01);
hi_void hifb_hal_set_gp_csc_coef02(hi_u32 data, hi_s32 csc_coef02);
hi_void hifb_hal_set_gp_csc_coef10(hi_u32 data, hi_s32 csc_coef10);
hi_void hifb_hal_set_gp_csc_coef11(hi_u32 data, hi_s32 csc_coef11);
hi_void hifb_hal_set_gp_csc_coef12(hi_u32 data, hi_s32 csc_coef12);
hi_void hifb_hal_set_gp_csc_coef20(hi_u32 data, hi_s32 csc_coef20);
hi_void hifb_hal_set_gp_csc_coef21(hi_u32 data, hi_s32 csc_coef21);
hi_void hifb_hal_set_gp_csc_coef22(hi_u32 data, hi_s32 csc_coef22);
hi_void hifb_hal_set_gp_csc_dc_in0(hi_u32 data, hi_s32 csc_in_dc0);
hi_void hifb_hal_set_gp_csc_dc_in1(hi_u32 data, hi_s32 csc_in_dc1);
hi_void hifb_hal_set_gp_csc_dc_in2(hi_u32 data, hi_s32 csc_in_dc2);
hi_void hifb_hal_set_gp_csc_dc_out0(hi_u32 data, hi_s32 csc_out_dc0);
hi_void hifb_hal_set_gp_csc_dc_out1(hi_u32 data, hi_s32 csc_out_dc1);
hi_void hifb_hal_set_gp_csc_dc_out2(hi_u32 data, hi_s32 csc_out_dc2);
hi_void hifb_hal_set_gp_csc_scale2p(hi_u32 data, hi_u32 scale);
hi_void hifb_hal_set_gp_csc_im_en(hi_u32 data, hi_bool is_enable);
hi_void hifb_hal_set_gp_csc_im_yuv2rgb_mode(hi_u32 data, hi_u32 mode);
hi_void hifb_hal_set_gp_csc_im_yuv2rgb_en(hi_u32 data, hi_bool is_enable);
hi_void hifb_hal_set_gp_csc_clip_min(hi_u32 data, hi_u32 clip_min);
hi_void hifb_hal_set_gp_csc_clip_max(hi_u32 data, hi_u32 clip_max);
hi_void hifb_hal_set_gp_csc_thr_r(hi_u32 data, hi_u32 thr_r);
hi_void hifb_hal_set_gp_csc_thr_b(hi_u32 data, hi_u32 thr_b);
hi_void hifb_hal_set_gp1_csc_scale2p(hi_u32 data, hi_u32 scale);
hi_void hifb_hal_set_gp1_csc_clip_max_y(hi_u32 data, hi_u32 clip_max);
hi_void hifb_hal_set_gp1_csc_clip_max_c(hi_u32 data, hi_u32 clip_max);
hi_void hifb_hal_set_gp1_csc_dc_out0(hi_u32 data, hi_s32 csc_out_dc0);
hi_void hifb_hal_set_gp1_csc_dc_out1(hi_u32 data, hi_s32 csc_out_dc1);
hi_void hifb_hal_set_gp1_csc_dc_out2(hi_u32 data, hi_s32 csc_out_dc2);

/* csc end */
/* zme begin */
hi_void hifb_hal_gp_set_zme_ckgt_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hsc_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hamid_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hlmid_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hchmid_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hfir_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hafir_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_hfir_order(hi_u32 data, hi_u32 hfir_order);
hi_void hifb_hal_gp_set_zme_hratio(hi_u32 data, hi_u32 hratio);
hi_void hifb_hal_gp_set_zme_out_height(hi_u32 data, hi_u32 out_height);
hi_void hifb_hal_gp_set_zme_vsc_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_vamid_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_vlmid_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_vchmid_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_vfir_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_vafir_en(hi_u32 data, hi_bool enable);
hi_void hifb_hal_gp_set_zme_vratio(hi_u32 data, hi_u32 vratio);
hi_void hifb_hal_gp_set_zme_vtp_offset(hi_u32 data, hi_u32 offset);
hi_void hifb_hal_gp_set_zme_vbtm_offset(hi_u32 data, hi_u32 offset);
hi_void hifb_hal_gp_set_zme_in_width(hi_u32 data, hi_u32 zme_iw, hi_u32 split);
hi_void hifb_hal_gp_set_zme_out_width(hi_u32 data, hi_u32 zme_ow, hi_u32 split);
hi_void hifb_hal_gp_set_zme_hor_loffset(hi_u32 data, hi_u32 hor_loffset, hi_u32 split);
hi_void hifb_hal_gp_set_zme_hor_coffset(hi_u32 data, hi_u32 hor_coffset, hi_u32 split);
hi_void hifb_hal_gp_set_zme_coef_zme(hi_u32 data, hi_u64 addr);
hi_void hifb_hal_gp_set_para_up_zme(hi_u32 data);
/* zme end */
hi_void drv_hifb_hal_gp_get_rect(hi_u32 data, OPTM_VDP_DISP_RECT_S *rect);
hi_void drv_hifb_hal_gp_set_layer_reso(hi_u32 data, OPTM_VDP_DISP_RECT_S *rect);
hi_void drv_hifb_hal_gp_set_read_mode(hi_u32 data, hi_u32 mode);
hi_void drv_hifb_hal_gp_set_up_mode(hi_u32 data, hi_u32 mode);
hi_void drv_hifb_hal_gp_set_para_upd(hi_u32 data, OPTM_VDP_GP_PARA_E mode);
hi_void drv_hifb_hal_gp_set_reg_up(hi_u32 data);
hi_void drv_hifb_hal_gp_set_layer_galpha(hi_u32 data, hi_u32 alpha);

// -------------------------------------------------------------------
// MIXER_BEGIN
// -------------------------------------------------------------------
hi_void drv_hifb_hal_cbm_open_mute_bk(hi_u32 data);
hi_void drv_hifb_hal_cbm_close_mute_bk(hi_u32 data);
hi_void drv_hifb_hal_cbm_set_mixer_bkg(OPTM_VDP_CBM_MIX_E u32mixer_id, OPTM_VDP_BKG_S *bkg);
hi_void drv_hifb_hal_cbm_set_mixer_prio(OPTM_VDP_CBM_MIX_E u32mixer_id, hi_u32 u32layer_id, hi_u32 u32prio);

// -------------------------------------------------------------------
// WBC_GP_BEGIN
// -------------------------------------------------------------------
hi_void drv_hifb_hal_wbc_gp_set_enable(OPTM_VDP_LAYER_WBC_E layer, hi_u32 enable);
hi_void drv_hifb_hal_wbc_gp_get_work_enable(OPTM_VDP_LAYER_WBC_E layer, hi_u32 *enable, hi_u32 *wbc_state);
hi_void drv_hifb_hal_wbc_set_out_mod(OPTM_VDP_LAYER_WBC_E layer, hi_u32 out_mode);
hi_void drv_hifb_hal_wbc_set_out_fmt(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_WBC_OFMT_E intf_fmt);
hi_void drv_hifb_hal_wbc_set_layer_addr(OPTM_VDP_LAYER_WBC_E layer, hi_u32 addr, hi_u32 stride);
hi_void drv_hifb_hal_wbc_get_layer_addr(OPTM_VDP_LAYER_WBC_E layer, hi_u32 *addr, hi_u32 *stride);
hi_void drv_hifb_hal_wbc_set_layer_reso(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DISP_RECT_S *rect);
hi_void drv_hifb_hal_wbc_set_dither_mode(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DITHER_E dither_mode);
hi_void drv_hifb_hal_wbc_set_crop_reso(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DISP_RECT_S *rect);
hi_void drv_hifb_hal_wbc_set_reg_up(OPTM_VDP_LAYER_WBC_E layer);
hi_void drv_hifb_hal_wbc_set_three_md(OPTM_VDP_LAYER_WBC_E layer, hi_u32 mode);
hi_void drv_hifb_hal_wbc_lbox_enable(OPTM_VDP_LAYER_WBC_E layer, hi_bool enable);
hi_void drv_hifb_hal_wbc_set_lbox_reso(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DISP_RECT_S *rect);
// -------------------------------------------------------------------
// WBC_GP_END
// -------------------------------------------------------------------

hi_u32 drv_hifb_hal_disp_get_int_signal(hi_u32 u32intmask);
hi_void drv_hifb_hal_disp_clear_int_signal(hi_u32 u32intmask);
OPTM_VDP_DISP_MODE_E drv_hifb_hal_disp_get_disp_mode(hi_u32 data);

#ifdef __cplusplus

#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

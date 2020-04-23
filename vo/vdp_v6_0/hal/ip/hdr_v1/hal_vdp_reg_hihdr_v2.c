/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_hihdr_v2.h"
#include "hal_vdp_comm.h"

hi_void vdp_hihdr_v2_setimcmpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_pos)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_cm_pos = im_cm_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_pos)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_tm_pos = im_tm_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimrshiftrounden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rshift_round_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_rshift_round_en = im_rshift_round_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimrshiften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rshift_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_rshift_en = im_rshift_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_ck_gt_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_ck_gt_en = im_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimiptinsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_ipt_in_sel)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_ipt_in_sel = im_ipt_in_sel;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimladjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_ladj_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_ladj_en = im_ladj_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapeotfparameotf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_eotf)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.imap_eotfparam_eotf = imap_eotfparam_eotf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapincolor(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_in_color)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.imap_in_color = imap_in_color;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setiminbits(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_in_bits)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_in_bits = im_in_bits;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimgammaen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_gamma_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_gamma_en = im_gamma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimdegammaen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_degamma_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_degamma_en = im_degamma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimnormen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_norm_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_norm_en = im_norm_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_en)
{
    u_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset));
    db_imap_ctrl.bits.im_en = im_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ctrl.u32) + offset), db_imap_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapeotfparamrangemin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_range_min)
{
    u_db_imap_in_rangemin db_imap_in_rangemin;

    db_imap_in_rangemin.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_in_rangemin.u32) + offset));
    db_imap_in_rangemin.bits.imap_eotfparam_range_min = imap_eotfparam_range_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_in_rangemin.u32) + offset), db_imap_in_rangemin.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapeotfparamrange(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_range)
{
    u_db_imap_in_range db_imap_in_range;

    db_imap_in_range.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_in_range.u32) + offset));
    db_imap_in_range.bits.imap_eotfparam_range = imap_eotfparam_range;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_in_range.u32) + offset), db_imap_in_range.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapeotfparamrangeinv(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_eotfparam_range_inv)
{
    u_db_imap_in_rangeinv db_imap_in_rangeinv;

    db_imap_in_rangeinv.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_in_rangeinv.u32) + offset));
    db_imap_in_rangeinv.bits.imap_eotfparam_range_inv = imap_eotfparam_range_inv;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_in_rangeinv.u32) + offset), db_imap_in_rangeinv.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3iptoff0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3ipt_off_0)
{
    u_db_imap_iptoff0 db_imap_iptoff0;

    db_imap_iptoff0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_iptoff0.u32) + offset));
    db_imap_iptoff0.bits.imap_v3ipt_off_0 = imap_v3ipt_off_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_iptoff0.u32) + offset), db_imap_iptoff0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3iptoff1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3ipt_off_1)
{
    u_db_imap_iptoff1 db_imap_iptoff1;

    db_imap_iptoff1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_iptoff1.u32) + offset));
    db_imap_iptoff1.bits.imap_v3ipt_off_1 = imap_v3ipt_off_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_iptoff1.u32) + offset), db_imap_iptoff1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3iptoff2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3ipt_off_2)
{
    u_db_imap_iptoff2 db_imap_iptoff2;

    db_imap_iptoff2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_iptoff2.u32) + offset));
    db_imap_iptoff2.bits.imap_v3ipt_off_2 = imap_v3ipt_off_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_iptoff2.u32) + offset), db_imap_iptoff2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapiptscale(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_ipt_scale)
{
    u_db_imap_iptscale db_imap_iptscale;

    db_imap_iptscale.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_iptscale.u32) + offset));
    db_imap_iptscale.bits.imap_ipt_scale = imap_ipt_scale;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_iptscale.u32) + offset), db_imap_iptscale.u32);

    return ;
}

hi_void vdp_hihdr_v2_setdegammarden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 degamma_rd_en)
{
    u_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset));
    db_imap_para_ren.bits.degamma_rd_en = degamma_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset), db_imap_para_ren.u32);

    return ;
}

hi_void vdp_hihdr_v2_setgammalutbrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gamma_lutb_rd_en)
{
    u_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset));
    db_imap_para_ren.bits.gamma_lutb_rd_en = gamma_lutb_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset), db_imap_para_ren.u32);

    return ;
}

hi_void vdp_hihdr_v2_setgammalutarden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gamma_luta_rd_en)
{
    u_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset));
    db_imap_para_ren.bits.gamma_luta_rd_en = gamma_luta_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset), db_imap_para_ren.u32);

    return ;
}

hi_void vdp_hihdr_v2_setgammalutxrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gamma_lutx_rd_en)
{
    u_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset));
    db_imap_para_ren.bits.gamma_lutx_rd_en = gamma_lutx_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_ren.u32) + offset), db_imap_para_ren.u32);

    return ;
}

hi_void vdp_hihdr_v2_setvdmparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdm_para_data)
{
    u_db_imap_para_data db_imap_para_data;

    db_imap_para_data.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_data.u32) + offset));
    db_imap_para_data.bits.vdm_para_data = vdm_para_data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_data.u32) + offset), db_imap_para_data.u32);

    return ;
}

hi_void vdp_hihdr_v2_setvdmcoef1upd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdm_coef1_upd)
{
    u_db_imap_para_up db_imap_para_up;

    db_imap_para_up.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_up.u32) + offset));
    db_imap_para_up.bits.vdm_coef1_upd = vdm_coef1_upd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_up.u32) + offset), db_imap_para_up.u32);

    return ;
}

hi_void vdp_hihdr_v2_setvdmcoefupd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vdm_coef_upd)
{
    u_db_imap_para_up db_imap_para_up;

    db_imap_para_up.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_para_up.u32) + offset));
    db_imap_para_up.bits.vdm_coef_upd = vdm_coef_upd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_para_up.u32) + offset), db_imap_para_up.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimdegammaclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_degamma_clip_max)
{
    u_db_imap_degamma_clip_max db_imap_degamma_clip_max;

    db_imap_degamma_clip_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_degamma_clip_max.u32) + offset));
    db_imap_degamma_clip_max.bits.im_degamma_clip_max = im_degamma_clip_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_degamma_clip_max.u32) + offset), db_imap_degamma_clip_max.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimdegammaclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_degamma_clip_min)
{
    u_db_imap_degamma_clip_min db_imap_degamma_clip_min;

    db_imap_degamma_clip_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_degamma_clip_min.u32) + offset));
    db_imap_degamma_clip_min.bits.im_degamma_clip_min = im_degamma_clip_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_degamma_clip_min.u32) + offset), db_imap_degamma_clip_min.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapladjchromaweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_ladj_chroma_weight)
{
    u_db_imap_ladj_weight db_imap_ladj_weight;

    db_imap_ladj_weight.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ladj_weight.u32) + offset));
    db_imap_ladj_weight.bits.imap_ladj_chroma_weight = imap_ladj_chroma_weight;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ladj_weight.u32) + offset), db_imap_ladj_weight.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimdemolumamode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_demo_luma_mode)
{
    u_db_imap_ladj_weight db_imap_ladj_weight;

    db_imap_ladj_weight.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ladj_weight.u32) + offset));
    db_imap_ladj_weight.bits.im_demo_luma_mode = im_demo_luma_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ladj_weight.u32) + offset), db_imap_ladj_weight.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimdemolumaen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_demo_luma_en)
{
    u_db_imap_ladj_weight db_imap_ladj_weight;

    db_imap_ladj_weight.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_ladj_weight.u32) + offset));
    db_imap_ladj_weight.bits.im_demo_luma_en = im_demo_luma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_ladj_weight.u32) + offset), db_imap_ladj_weight.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv1hdren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v1_hdr_en)
{
    u_db_imap_yuv2rgb_ctrl db_imap_yuv2rgb_ctrl;

    db_imap_yuv2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset));
    db_imap_yuv2rgb_ctrl.bits.im_v1_hdr_en = im_v1_hdr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset), db_imap_yuv2rgb_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv0hdren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v0_hdr_en)
{
    u_db_imap_yuv2rgb_ctrl db_imap_yuv2rgb_ctrl;

    db_imap_yuv2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset));
    db_imap_yuv2rgb_ctrl.bits.im_v0_hdr_en = im_v0_hdr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset), db_imap_yuv2rgb_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimyuv2rgbmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_mode)
{
    u_db_imap_yuv2rgb_ctrl db_imap_yuv2rgb_ctrl;

    db_imap_yuv2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset));
    db_imap_yuv2rgb_ctrl.bits.im_yuv2rgb_mode = im_yuv2rgb_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset), db_imap_yuv2rgb_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv1y2ren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v1_y2r_en)
{
    u_db_imap_yuv2rgb_ctrl db_imap_yuv2rgb_ctrl;

    db_imap_yuv2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset));
    db_imap_yuv2rgb_ctrl.bits.im_v1_y2r_en = im_v1_y2r_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset), db_imap_yuv2rgb_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv0y2ren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v0_y2r_en)
{
    u_db_imap_yuv2rgb_ctrl db_imap_yuv2rgb_ctrl;

    db_imap_yuv2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset));
    db_imap_yuv2rgb_ctrl.bits.im_v0_y2r_en = im_v0_y2r_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset), db_imap_yuv2rgb_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimyuv2rgben(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_en)
{
    u_db_imap_yuv2rgb_ctrl db_imap_yuv2rgb_ctrl;

    db_imap_yuv2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset));
    db_imap_yuv2rgb_ctrl.bits.im_yuv2rgb_en = im_yuv2rgb_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_ctrl.u32) + offset), db_imap_yuv2rgb_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_00)
{
    u_db_imap_yuv2rgb_coef00 db_imap_yuv2rgb_coef00;

    db_imap_yuv2rgb_coef00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef00.u32) + offset));
    db_imap_yuv2rgb_coef00.bits.imap_m33yuv2rgb_00 = imap_m33yuv2rgb_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef00.u32) + offset), db_imap_yuv2rgb_coef00.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_01)
{
    u_db_imap_yuv2rgb_coef01 db_imap_yuv2rgb_coef01;

    db_imap_yuv2rgb_coef01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef01.u32) + offset));
    db_imap_yuv2rgb_coef01.bits.imap_m33yuv2rgb_01 = imap_m33yuv2rgb_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef01.u32) + offset), db_imap_yuv2rgb_coef01.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_02)
{
    u_db_imap_yuv2rgb_coef02 db_imap_yuv2rgb_coef02;

    db_imap_yuv2rgb_coef02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef02.u32) + offset));
    db_imap_yuv2rgb_coef02.bits.imap_m33yuv2rgb_02 = imap_m33yuv2rgb_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef02.u32) + offset), db_imap_yuv2rgb_coef02.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_10)
{
    u_db_imap_yuv2rgb_coef03 db_imap_yuv2rgb_coef03;

    db_imap_yuv2rgb_coef03.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef03.u32) + offset));
    db_imap_yuv2rgb_coef03.bits.imap_m33yuv2rgb_10 = imap_m33yuv2rgb_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef03.u32) + offset), db_imap_yuv2rgb_coef03.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_11)
{
    u_db_imap_yuv2rgb_coef04 db_imap_yuv2rgb_coef04;

    db_imap_yuv2rgb_coef04.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef04.u32) + offset));
    db_imap_yuv2rgb_coef04.bits.imap_m33yuv2rgb_11 = imap_m33yuv2rgb_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef04.u32) + offset), db_imap_yuv2rgb_coef04.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_12)
{
    u_db_imap_yuv2rgb_coef05 db_imap_yuv2rgb_coef05;

    db_imap_yuv2rgb_coef05.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef05.u32) + offset));
    db_imap_yuv2rgb_coef05.bits.imap_m33yuv2rgb_12 = imap_m33yuv2rgb_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef05.u32) + offset), db_imap_yuv2rgb_coef05.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_20)
{
    u_db_imap_yuv2rgb_coef06 db_imap_yuv2rgb_coef06;

    db_imap_yuv2rgb_coef06.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef06.u32) + offset));
    db_imap_yuv2rgb_coef06.bits.imap_m33yuv2rgb_20 = imap_m33yuv2rgb_20;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef06.u32) + offset), db_imap_yuv2rgb_coef06.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_21)
{
    u_db_imap_yuv2rgb_coef07 db_imap_yuv2rgb_coef07;

    db_imap_yuv2rgb_coef07.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef07.u32) + offset));
    db_imap_yuv2rgb_coef07.bits.imap_m33yuv2rgb_21 = imap_m33yuv2rgb_21;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef07.u32) + offset), db_imap_yuv2rgb_coef07.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_22)
{
    u_db_imap_yuv2rgb_coef08 db_imap_yuv2rgb_coef08;

    db_imap_yuv2rgb_coef08.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef08.u32) + offset));
    db_imap_yuv2rgb_coef08.bits.imap_m33yuv2rgb_22 = imap_m33yuv2rgb_22;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef08.u32) + offset), db_imap_yuv2rgb_coef08.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb200(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_00)
{
    u_db_imap_yuv2rgb_coef10 db_imap_yuv2rgb_coef10;

    db_imap_yuv2rgb_coef10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef10.u32) + offset));
    db_imap_yuv2rgb_coef10.bits.imap_m33yuv2rgb2_00 = imap_m33yuv2rgb2_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef10.u32) + offset), db_imap_yuv2rgb_coef10.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb201(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_01)
{
    u_db_imap_yuv2rgb_coef11 db_imap_yuv2rgb_coef11;

    db_imap_yuv2rgb_coef11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef11.u32) + offset));
    db_imap_yuv2rgb_coef11.bits.imap_m33yuv2rgb2_01 = imap_m33yuv2rgb2_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef11.u32) + offset), db_imap_yuv2rgb_coef11.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb202(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_02)
{
    u_db_imap_yuv2rgb_coef12 db_imap_yuv2rgb_coef12;

    db_imap_yuv2rgb_coef12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef12.u32) + offset));
    db_imap_yuv2rgb_coef12.bits.imap_m33yuv2rgb2_02 = imap_m33yuv2rgb2_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef12.u32) + offset), db_imap_yuv2rgb_coef12.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb210(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_10)
{
    u_db_imap_yuv2rgb_coef13 db_imap_yuv2rgb_coef13;

    db_imap_yuv2rgb_coef13.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef13.u32) + offset));
    db_imap_yuv2rgb_coef13.bits.imap_m33yuv2rgb2_10 = imap_m33yuv2rgb2_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef13.u32) + offset), db_imap_yuv2rgb_coef13.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb211(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_11)
{
    u_db_imap_yuv2rgb_coef14 db_imap_yuv2rgb_coef14;

    db_imap_yuv2rgb_coef14.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef14.u32) + offset));
    db_imap_yuv2rgb_coef14.bits.imap_m33yuv2rgb2_11 = imap_m33yuv2rgb2_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef14.u32) + offset), db_imap_yuv2rgb_coef14.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb212(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_12)
{
    u_db_imap_yuv2rgb_coef15 db_imap_yuv2rgb_coef15;

    db_imap_yuv2rgb_coef15.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef15.u32) + offset));
    db_imap_yuv2rgb_coef15.bits.imap_m33yuv2rgb2_12 = imap_m33yuv2rgb2_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef15.u32) + offset), db_imap_yuv2rgb_coef15.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb220(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_20)
{
    u_db_imap_yuv2rgb_coef16 db_imap_yuv2rgb_coef16;

    db_imap_yuv2rgb_coef16.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef16.u32) + offset));
    db_imap_yuv2rgb_coef16.bits.imap_m33yuv2rgb2_20 = imap_m33yuv2rgb2_20;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef16.u32) + offset), db_imap_yuv2rgb_coef16.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb221(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_21)
{
    u_db_imap_yuv2rgb_coef17 db_imap_yuv2rgb_coef17;

    db_imap_yuv2rgb_coef17.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef17.u32) + offset));
    db_imap_yuv2rgb_coef17.bits.imap_m33yuv2rgb2_21 = imap_m33yuv2rgb2_21;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef17.u32) + offset), db_imap_yuv2rgb_coef17.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgb222(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb2_22)
{
    u_db_imap_yuv2rgb_coef18 db_imap_yuv2rgb_coef18;

    db_imap_yuv2rgb_coef18.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef18.u32) + offset));
    db_imap_yuv2rgb_coef18.bits.imap_m33yuv2rgb2_22 = imap_m33yuv2rgb2_22;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_coef18.u32) + offset), db_imap_yuv2rgb_coef18.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33yuv2rgbscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33yuv2rgb_scale2p)
{
    u_db_imap_yuv2rgb_scale2p db_imap_yuv2rgb_scale2p;

    db_imap_yuv2rgb_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_scale2p.u32) + offset));
    db_imap_yuv2rgb_scale2p.bits.imap_m33yuv2rgb_scale2p = imap_m33yuv2rgb_scale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_scale2p.u32) + offset), db_imap_yuv2rgb_scale2p.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3yuv2rgbdcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb_dc_in_0)
{
    u_db_imap_yuv2rgb_in_dc00 db_imap_yuv2rgb_in_dc00;

    db_imap_yuv2rgb_in_dc00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc00.u32) + offset));
    db_imap_yuv2rgb_in_dc00.bits.im_v3_yuv2rgb_dc_in_0 = im_v3_yuv2rgb_dc_in_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc00.u32) + offset), db_imap_yuv2rgb_in_dc00.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3yuv2rgbdcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb_dc_in_1)
{
    u_db_imap_yuv2rgb_in_dc01 db_imap_yuv2rgb_in_dc01;

    db_imap_yuv2rgb_in_dc01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc01.u32) + offset));
    db_imap_yuv2rgb_in_dc01.bits.im_v3_yuv2rgb_dc_in_1 = im_v3_yuv2rgb_dc_in_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc01.u32) + offset), db_imap_yuv2rgb_in_dc01.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3yuv2rgbdcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb_dc_in_2)
{
    u_db_imap_yuv2rgb_in_dc02 db_imap_yuv2rgb_in_dc02;

    db_imap_yuv2rgb_in_dc02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc02.u32) + offset));
    db_imap_yuv2rgb_in_dc02.bits.im_v3_yuv2rgb_dc_in_2 = im_v3_yuv2rgb_dc_in_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc02.u32) + offset), db_imap_yuv2rgb_in_dc02.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3yuv2rgboffinrgb0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3yuv2rgb_offinrgb_0)
{
    u_db_imap_yuv2rgb_out_dc00 db_imap_yuv2rgb_out_dc00;

    db_imap_yuv2rgb_out_dc00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc00.u32) + offset));
    db_imap_yuv2rgb_out_dc00.bits.imap_v3yuv2rgb_offinrgb_0 = imap_v3yuv2rgb_offinrgb_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc00.u32) + offset), db_imap_yuv2rgb_out_dc00.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3yuv2rgboffinrgb1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3yuv2rgb_offinrgb_1)
{
    u_db_imap_yuv2rgb_out_dc01 db_imap_yuv2rgb_out_dc01;

    db_imap_yuv2rgb_out_dc01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc01.u32) + offset));
    db_imap_yuv2rgb_out_dc01.bits.imap_v3yuv2rgb_offinrgb_1 = imap_v3yuv2rgb_offinrgb_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc01.u32) + offset), db_imap_yuv2rgb_out_dc01.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3yuv2rgboffinrgb2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_v3yuv2rgb_offinrgb_2)
{
    u_db_imap_yuv2rgb_out_dc02 db_imap_yuv2rgb_out_dc02;

    db_imap_yuv2rgb_out_dc02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc02.u32) + offset));
    db_imap_yuv2rgb_out_dc02.bits.imap_v3yuv2rgb_offinrgb_2 = imap_v3yuv2rgb_offinrgb_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc02.u32) + offset), db_imap_yuv2rgb_out_dc02.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3yuv2rgb2dcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb2_dc_in_0)
{
    u_db_imap_yuv2rgb_in_dc10 db_imap_yuv2rgb_in_dc10;

    db_imap_yuv2rgb_in_dc10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc10.u32) + offset));
    db_imap_yuv2rgb_in_dc10.bits.im_v3_yuv2rgb2_dc_in_0 = im_v3_yuv2rgb2_dc_in_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc10.u32) + offset), db_imap_yuv2rgb_in_dc10.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3yuv2rgb2dcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb2_dc_in_1)
{
    u_db_imap_yuv2rgb_in_dc11 db_imap_yuv2rgb_in_dc11;

    db_imap_yuv2rgb_in_dc11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc11.u32) + offset));
    db_imap_yuv2rgb_in_dc11.bits.im_v3_yuv2rgb2_dc_in_1 = im_v3_yuv2rgb2_dc_in_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc11.u32) + offset), db_imap_yuv2rgb_in_dc11.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3yuv2rgb2dcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_yuv2rgb2_dc_in_2)
{
    u_db_imap_yuv2rgb_in_dc12 db_imap_yuv2rgb_in_dc12;

    db_imap_yuv2rgb_in_dc12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc12.u32) + offset));
    db_imap_yuv2rgb_in_dc12.bits.im_v3_yuv2rgb2_dc_in_2 = im_v3_yuv2rgb2_dc_in_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_in_dc12.u32) + offset), db_imap_yuv2rgb_in_dc12.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3yuv2rgb2offinrgb0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_db_imap_yuv2rgb_out_dc10 db_imap_yuv2rgb_out_dc10;

    db_imap_yuv2rgb_out_dc10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc10.u32) + offset));
    db_imap_yuv2rgb_out_dc10.bits.imap_v3yuv2rgb2_offinrgb_0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc10.u32) + offset), db_imap_yuv2rgb_out_dc10.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3yuv2rgb2offinrgb1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_db_imap_yuv2rgb_out_dc11 db_imap_yuv2rgb_out_dc11;

    db_imap_yuv2rgb_out_dc11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc11.u32) + offset));
    db_imap_yuv2rgb_out_dc11.bits.imap_v3yuv2rgb2_offinrgb_1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc11.u32) + offset), db_imap_yuv2rgb_out_dc11.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapv3yuv2rgb2offinrgb2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_db_imap_yuv2rgb_out_dc12 db_imap_yuv2rgb_out_dc12;

    db_imap_yuv2rgb_out_dc12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc12.u32) + offset));
    db_imap_yuv2rgb_out_dc12.bits.imap_v3yuv2rgb2_offinrgb_2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_out_dc12.u32) + offset), db_imap_yuv2rgb_out_dc12.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimyuv2rgbclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_clip_min)
{
    u_db_imap_yuv2rgb_min db_imap_yuv2rgb_min;

    db_imap_yuv2rgb_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_min.u32) + offset));
    db_imap_yuv2rgb_min.bits.im_yuv2rgb_clip_min = im_yuv2rgb_clip_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_min.u32) + offset), db_imap_yuv2rgb_min.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimyuv2rgbclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_clip_max)
{
    u_db_imap_yuv2rgb_max db_imap_yuv2rgb_max;

    db_imap_yuv2rgb_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_max.u32) + offset));
    db_imap_yuv2rgb_max.bits.im_yuv2rgb_clip_max = im_yuv2rgb_clip_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_max.u32) + offset), db_imap_yuv2rgb_max.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimyuv2rgbthrr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_thr_r)
{
    u_db_imap_yuv2rgb_thr db_imap_yuv2rgb_thr;

    db_imap_yuv2rgb_thr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_thr.u32) + offset));
    db_imap_yuv2rgb_thr.bits.im_yuv2rgb_thr_r = im_yuv2rgb_thr_r;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_thr.u32) + offset), db_imap_yuv2rgb_thr.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimyuv2rgbthrb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_yuv2rgb_thr_b)
{
    u_db_imap_yuv2rgb_thr db_imap_yuv2rgb_thr;

    db_imap_yuv2rgb_thr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_thr.u32) + offset));
    db_imap_yuv2rgb_thr.bits.im_yuv2rgb_thr_b = im_yuv2rgb_thr_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_yuv2rgb_thr.u32) + offset), db_imap_yuv2rgb_thr.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimrgb2lmsen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rgb2lms_en)
{
    u_db_imap_rgb2lms_ctrl db_imap_rgb2lms_ctrl;

    db_imap_rgb2lms_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_ctrl.u32) + offset));
    db_imap_rgb2lms_ctrl.bits.im_rgb2lms_en = im_rgb2lms_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_ctrl.u32) + offset), db_imap_rgb2lms_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_00)
{
    u_db_imap_rgb2lms_coef0 db_imap_rgb2lms_coef0;

    db_imap_rgb2lms_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef0.u32) + offset));
    db_imap_rgb2lms_coef0.bits.imap_m33rgb2lms_00 = imap_m33rgb2lms_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef0.u32) + offset), db_imap_rgb2lms_coef0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_01)
{
    u_db_imap_rgb2lms_coef1 db_imap_rgb2lms_coef1;

    db_imap_rgb2lms_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef1.u32) + offset));
    db_imap_rgb2lms_coef1.bits.imap_m33rgb2lms_01 = imap_m33rgb2lms_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef1.u32) + offset), db_imap_rgb2lms_coef1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_02)
{
    u_db_imap_rgb2lms_coef2 db_imap_rgb2lms_coef2;

    db_imap_rgb2lms_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef2.u32) + offset));
    db_imap_rgb2lms_coef2.bits.imap_m33rgb2lms_02 = imap_m33rgb2lms_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef2.u32) + offset), db_imap_rgb2lms_coef2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_10)
{
    u_db_imap_rgb2lms_coef3 db_imap_rgb2lms_coef3;

    db_imap_rgb2lms_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef3.u32) + offset));
    db_imap_rgb2lms_coef3.bits.imap_m33rgb2lms_10 = imap_m33rgb2lms_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef3.u32) + offset), db_imap_rgb2lms_coef3.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_11)
{
    u_db_imap_rgb2lms_coef4 db_imap_rgb2lms_coef4;

    db_imap_rgb2lms_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef4.u32) + offset));
    db_imap_rgb2lms_coef4.bits.imap_m33rgb2lms_11 = imap_m33rgb2lms_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef4.u32) + offset), db_imap_rgb2lms_coef4.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_12)
{
    u_db_imap_rgb2lms_coef5 db_imap_rgb2lms_coef5;

    db_imap_rgb2lms_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef5.u32) + offset));
    db_imap_rgb2lms_coef5.bits.imap_m33rgb2lms_12 = imap_m33rgb2lms_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef5.u32) + offset), db_imap_rgb2lms_coef5.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_20)
{
    u_db_imap_rgb2lms_coef6 db_imap_rgb2lms_coef6;

    db_imap_rgb2lms_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef6.u32) + offset));
    db_imap_rgb2lms_coef6.bits.imap_m33rgb2lms_20 = imap_m33rgb2lms_20;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef6.u32) + offset), db_imap_rgb2lms_coef6.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_21)
{
    u_db_imap_rgb2lms_coef7 db_imap_rgb2lms_coef7;

    db_imap_rgb2lms_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef7.u32) + offset));
    db_imap_rgb2lms_coef7.bits.imap_m33rgb2lms_21 = imap_m33rgb2lms_21;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef7.u32) + offset), db_imap_rgb2lms_coef7.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lms22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_22)
{
    u_db_imap_rgb2lms_coef8 db_imap_rgb2lms_coef8;

    db_imap_rgb2lms_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef8.u32) + offset));
    db_imap_rgb2lms_coef8.bits.imap_m33rgb2lms_22 = imap_m33rgb2lms_22;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_coef8.u32) + offset), db_imap_rgb2lms_coef8.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33rgb2lmsscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33rgb2lms_scale2p)
{
    u_db_imap_rgb2lms_scale2p db_imap_rgb2lms_scale2p;

    db_imap_rgb2lms_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_scale2p.u32) + offset));
    db_imap_rgb2lms_scale2p.bits.imap_m33rgb2lms_scale2p = imap_m33rgb2lms_scale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_scale2p.u32) + offset), db_imap_rgb2lms_scale2p.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimrgb2lmsclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rgb2lms_clip_min)
{
    u_db_imap_rgb2lms_min db_imap_rgb2lms_min;

    db_imap_rgb2lms_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_min.u32) + offset));
    db_imap_rgb2lms_min.bits.im_rgb2lms_clip_min = im_rgb2lms_clip_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_min.u32) + offset), db_imap_rgb2lms_min.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimrgb2lmsclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_rgb2lms_clip_max)
{
    u_db_imap_rgb2lms_max db_imap_rgb2lms_max;

    db_imap_rgb2lms_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_max.u32) + offset));
    db_imap_rgb2lms_max.bits.im_rgb2lms_clip_max = im_rgb2lms_clip_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_max.u32) + offset), db_imap_rgb2lms_max.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3rgb2lmsdcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_rgb2lms_dc_in_0)
{
    u_db_imap_rgb2lms_in_dc0 db_imap_rgb2lms_in_dc0;

    db_imap_rgb2lms_in_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_in_dc0.u32) + offset));
    db_imap_rgb2lms_in_dc0.bits.im_v3_rgb2lms_dc_in_0 = im_v3_rgb2lms_dc_in_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_in_dc0.u32) + offset), db_imap_rgb2lms_in_dc0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3rgb2lmsdcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_rgb2lms_dc_in_1)
{
    u_db_imap_rgb2lms_in_dc1 db_imap_rgb2lms_in_dc1;

    db_imap_rgb2lms_in_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_in_dc1.u32) + offset));
    db_imap_rgb2lms_in_dc1.bits.im_v3_rgb2lms_dc_in_1 = im_v3_rgb2lms_dc_in_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_in_dc1.u32) + offset), db_imap_rgb2lms_in_dc1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3rgb2lmsdcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_rgb2lms_dc_in_2)
{
    u_db_imap_rgb2lms_in_dc2 db_imap_rgb2lms_in_dc2;

    db_imap_rgb2lms_in_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_in_dc2.u32) + offset));
    db_imap_rgb2lms_in_dc2.bits.im_v3_rgb2lms_dc_in_2 = im_v3_rgb2lms_dc_in_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_rgb2lms_in_dc2.u32) + offset), db_imap_rgb2lms_in_dc2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimlms2ipten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_en)
{
    u_db_imap_lms2ipt_ctrl db_imap_lms2ipt_ctrl;

    db_imap_lms2ipt_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_ctrl.u32) + offset));
    db_imap_lms2ipt_ctrl.bits.im_lms2ipt_en = im_lms2ipt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_ctrl.u32) + offset), db_imap_lms2ipt_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_00)
{
    u_db_imap_lms2ipt_coef0 db_imap_lms2ipt_coef0;

    db_imap_lms2ipt_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef0.u32) + offset));
    db_imap_lms2ipt_coef0.bits.imap_m33lms2ipt_00 = imap_m33lms2ipt_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef0.u32) + offset), db_imap_lms2ipt_coef0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_01)
{
    u_db_imap_lms2ipt_coef1 db_imap_lms2ipt_coef1;

    db_imap_lms2ipt_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef1.u32) + offset));
    db_imap_lms2ipt_coef1.bits.imap_m33lms2ipt_01 = imap_m33lms2ipt_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef1.u32) + offset), db_imap_lms2ipt_coef1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_02)
{
    u_db_imap_lms2ipt_coef2 db_imap_lms2ipt_coef2;

    db_imap_lms2ipt_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef2.u32) + offset));
    db_imap_lms2ipt_coef2.bits.imap_m33lms2ipt_02 = imap_m33lms2ipt_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef2.u32) + offset), db_imap_lms2ipt_coef2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_10)
{
    u_db_imap_lms2ipt_coef3 db_imap_lms2ipt_coef3;

    db_imap_lms2ipt_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef3.u32) + offset));
    db_imap_lms2ipt_coef3.bits.imap_m33lms2ipt_10 = imap_m33lms2ipt_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef3.u32) + offset), db_imap_lms2ipt_coef3.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_11)
{
    u_db_imap_lms2ipt_coef4 db_imap_lms2ipt_coef4;

    db_imap_lms2ipt_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef4.u32) + offset));
    db_imap_lms2ipt_coef4.bits.imap_m33lms2ipt_11 = imap_m33lms2ipt_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef4.u32) + offset), db_imap_lms2ipt_coef4.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_12)
{
    u_db_imap_lms2ipt_coef5 db_imap_lms2ipt_coef5;

    db_imap_lms2ipt_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef5.u32) + offset));
    db_imap_lms2ipt_coef5.bits.imap_m33lms2ipt_12 = imap_m33lms2ipt_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef5.u32) + offset), db_imap_lms2ipt_coef5.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_20)
{
    u_db_imap_lms2ipt_coef6 db_imap_lms2ipt_coef6;

    db_imap_lms2ipt_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef6.u32) + offset));
    db_imap_lms2ipt_coef6.bits.imap_m33lms2ipt_20 = imap_m33lms2ipt_20;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef6.u32) + offset), db_imap_lms2ipt_coef6.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_21)
{
    u_db_imap_lms2ipt_coef7 db_imap_lms2ipt_coef7;

    db_imap_lms2ipt_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef7.u32) + offset));
    db_imap_lms2ipt_coef7.bits.imap_m33lms2ipt_21 = imap_m33lms2ipt_21;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef7.u32) + offset), db_imap_lms2ipt_coef7.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2ipt22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_22)
{
    u_db_imap_lms2ipt_coef8 db_imap_lms2ipt_coef8;

    db_imap_lms2ipt_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef8.u32) + offset));
    db_imap_lms2ipt_coef8.bits.imap_m33lms2ipt_22 = imap_m33lms2ipt_22;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_coef8.u32) + offset), db_imap_lms2ipt_coef8.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimapm33lms2iptscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imap_m33lms2ipt_scale2p)
{
    u_db_imap_lms2ipt_scale2p db_imap_lms2ipt_scale2p;

    db_imap_lms2ipt_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_scale2p.u32) + offset));
    db_imap_lms2ipt_scale2p.bits.imap_m33lms2ipt_scale2p = imap_m33lms2ipt_scale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_scale2p.u32) + offset), db_imap_lms2ipt_scale2p.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimlms2iptclipminy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_min_y)
{
    u_db_imap_lms2ipt_min db_imap_lms2ipt_min;

    db_imap_lms2ipt_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_min.u32) + offset));
    db_imap_lms2ipt_min.bits.im_lms2ipt_clip_min_y = im_lms2ipt_clip_min_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_min.u32) + offset), db_imap_lms2ipt_min.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimlms2iptclipmaxy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_max_y)
{
    u_db_imap_lms2ipt_max db_imap_lms2ipt_max;

    db_imap_lms2ipt_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_max.u32) + offset));
    db_imap_lms2ipt_max.bits.im_lms2ipt_clip_max_y = im_lms2ipt_clip_max_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_max.u32) + offset), db_imap_lms2ipt_max.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimlms2iptclipminc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_min_c)
{
    u_db_imap_lms2ipt_min db_imap_lms2ipt_min;

    db_imap_lms2ipt_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_min.u32) + offset));
    db_imap_lms2ipt_min.bits.im_lms2ipt_clip_min_c = im_lms2ipt_clip_min_c;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_min.u32) + offset), db_imap_lms2ipt_min.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimlms2iptclipmaxc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_lms2ipt_clip_max_c)
{
    u_db_imap_lms2ipt_max db_imap_lms2ipt_max;

    db_imap_lms2ipt_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_max.u32) + offset));
    db_imap_lms2ipt_max.bits.im_lms2ipt_clip_max_c = im_lms2ipt_clip_max_c;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_max.u32) + offset), db_imap_lms2ipt_max.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3lms2iptdcout0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_out_0)
{
    u_db_imap_lms2ipt_out_dc0 db_imap_lms2ipt_out_dc0;

    db_imap_lms2ipt_out_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_out_dc0.u32) + offset));
    db_imap_lms2ipt_out_dc0.bits.im_v3_lms2ipt_dc_out_0 = im_v3_lms2ipt_dc_out_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_out_dc0.u32) + offset), db_imap_lms2ipt_out_dc0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3lms2iptdcout1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_out_1)
{
    u_db_imap_lms2ipt_out_dc1 db_imap_lms2ipt_out_dc1;

    db_imap_lms2ipt_out_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_out_dc1.u32) + offset));
    db_imap_lms2ipt_out_dc1.bits.im_v3_lms2ipt_dc_out_1 = im_v3_lms2ipt_dc_out_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_out_dc1.u32) + offset), db_imap_lms2ipt_out_dc1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3lms2iptdcout2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_out_2)
{
    u_db_imap_lms2ipt_out_dc2 db_imap_lms2ipt_out_dc2;

    db_imap_lms2ipt_out_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_out_dc2.u32) + offset));
    db_imap_lms2ipt_out_dc2.bits.im_v3_lms2ipt_dc_out_2 = im_v3_lms2ipt_dc_out_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_out_dc2.u32) + offset), db_imap_lms2ipt_out_dc2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3lms2iptdcin0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_in_0)
{
    u_db_imap_lms2ipt_in_dc0 db_imap_lms2ipt_in_dc0;

    db_imap_lms2ipt_in_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_in_dc0.u32) + offset));
    db_imap_lms2ipt_in_dc0.bits.im_v3_lms2ipt_dc_in_0 = im_v3_lms2ipt_dc_in_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_in_dc0.u32) + offset), db_imap_lms2ipt_in_dc0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3lms2iptdcin1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_in_1)
{
    u_db_imap_lms2ipt_in_dc1 db_imap_lms2ipt_in_dc1;

    db_imap_lms2ipt_in_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_in_dc1.u32) + offset));
    db_imap_lms2ipt_in_dc1.bits.im_v3_lms2ipt_dc_in_1 = im_v3_lms2ipt_dc_in_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_in_dc1.u32) + offset), db_imap_lms2ipt_in_dc1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimv3lms2iptdcin2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_v3_lms2ipt_dc_in_2)
{
    u_db_imap_lms2ipt_in_dc2 db_imap_lms2ipt_in_dc2;

    db_imap_lms2ipt_in_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_in_dc2.u32) + offset));
    db_imap_lms2ipt_in_dc2.bits.im_v3_lms2ipt_dc_in_2 = im_v3_lms2ipt_dc_in_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_lms2ipt_in_dc2.u32) + offset), db_imap_lms2ipt_in_dc2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmv1en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_v1_en)
{
    u_db_imap_tmap_v1_ctrl db_imap_tmap_v1_ctrl;

    db_imap_tmap_v1_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_ctrl.u32) + offset));
    db_imap_tmap_v1_ctrl.bits.im_tm_v1_en = im_tm_v1_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_ctrl.u32) + offset), db_imap_tmap_v1_ctrl.u32);

    return ;
}

hi_void vdp_hihdr_v2_setvhdrtmaprden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_tmap_rd_en)
{
    u_db_imap_tmap_v1_ren db_imap_tmap_v1_ren;

    db_imap_tmap_v1_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_ren.u32) + offset));
    db_imap_tmap_v1_ren.bits.vhdr_tmap_rd_en = vhdr_tmap_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_ren.u32) + offset), db_imap_tmap_v1_ren.u32);

    return ;
}

hi_void vdp_hihdr_v2_setvhdrtmapparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_tmap_para_data)
{
    u_db_imap_tmap_v1_data db_imap_tmap_v1_data;

    db_imap_tmap_v1_data.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_data.u32) + offset));
    db_imap_tmap_v1_data.bits.vhdr_tmap_para_data = vhdr_tmap_para_data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_data.u32) + offset), db_imap_tmap_v1_data.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmscalemixalpha(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_scale_mix_alpha)
{
    u_db_imap_tmap_v1_mix_alpha db_imap_tmap_v1_mix_alpha;

    db_imap_tmap_v1_mix_alpha.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_mix_alpha.u32) + offset));
    db_imap_tmap_v1_mix_alpha.bits.im_tm_scale_mix_alpha = im_tm_scale_mix_alpha;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_mix_alpha.u32) + offset), db_imap_tmap_v1_mix_alpha.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmmixalpha(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_mix_alpha)
{
    u_db_imap_tmap_v1_mix_alpha db_imap_tmap_v1_mix_alpha;

    db_imap_tmap_v1_mix_alpha.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_mix_alpha.u32) + offset));
    db_imap_tmap_v1_mix_alpha.bits.im_tm_mix_alpha = im_tm_mix_alpha;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_mix_alpha.u32) + offset), db_imap_tmap_v1_mix_alpha.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_3)
{
    u_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_3 = im_tm_x_step_3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset), db_imap_tmap_v1_step1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_2)
{
    u_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_2 = im_tm_x_step_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset), db_imap_tmap_v1_step1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_1)
{
    u_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_1 = im_tm_x_step_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset), db_imap_tmap_v1_step1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_0)
{
    u_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_0 = im_tm_x_step_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step1.u32) + offset), db_imap_tmap_v1_step1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_7)
{
    u_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_7 = im_tm_x_step_7;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset), db_imap_tmap_v1_step2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_6)
{
    u_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_6 = im_tm_x_step_6;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset), db_imap_tmap_v1_step2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_5)
{
    u_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_5 = im_tm_x_step_5;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset), db_imap_tmap_v1_step2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxstep4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_step_4)
{
    u_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_4 = im_tm_x_step_4;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_step2.u32) + offset), db_imap_tmap_v1_step2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_0)
{
    u_db_imap_tmap_v1_pos1 db_imap_tmap_v1_pos1;

    db_imap_tmap_v1_pos1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos1.u32) + offset));
    db_imap_tmap_v1_pos1.bits.im_tm_x_pos_0 = im_tm_x_pos_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos1.u32) + offset), db_imap_tmap_v1_pos1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_1)
{
    u_db_imap_tmap_v1_pos2 db_imap_tmap_v1_pos2;

    db_imap_tmap_v1_pos2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos2.u32) + offset));
    db_imap_tmap_v1_pos2.bits.im_tm_x_pos_1 = im_tm_x_pos_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos2.u32) + offset), db_imap_tmap_v1_pos2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_2)
{
    u_db_imap_tmap_v1_pos3 db_imap_tmap_v1_pos3;

    db_imap_tmap_v1_pos3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos3.u32) + offset));
    db_imap_tmap_v1_pos3.bits.im_tm_x_pos_2 = im_tm_x_pos_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos3.u32) + offset), db_imap_tmap_v1_pos3.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_3)
{
    u_db_imap_tmap_v1_pos4 db_imap_tmap_v1_pos4;

    db_imap_tmap_v1_pos4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos4.u32) + offset));
    db_imap_tmap_v1_pos4.bits.im_tm_x_pos_3 = im_tm_x_pos_3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos4.u32) + offset), db_imap_tmap_v1_pos4.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_4)
{
    u_db_imap_tmap_v1_pos5 db_imap_tmap_v1_pos5;

    db_imap_tmap_v1_pos5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos5.u32) + offset));
    db_imap_tmap_v1_pos5.bits.im_tm_x_pos_4 = im_tm_x_pos_4;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos5.u32) + offset), db_imap_tmap_v1_pos5.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_5)
{
    u_db_imap_tmap_v1_pos6 db_imap_tmap_v1_pos6;

    db_imap_tmap_v1_pos6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos6.u32) + offset));
    db_imap_tmap_v1_pos6.bits.im_tm_x_pos_5 = im_tm_x_pos_5;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos6.u32) + offset), db_imap_tmap_v1_pos6.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_6)
{
    u_db_imap_tmap_v1_pos7 db_imap_tmap_v1_pos7;

    db_imap_tmap_v1_pos7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos7.u32) + offset));
    db_imap_tmap_v1_pos7.bits.im_tm_x_pos_6 = im_tm_x_pos_6;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos7.u32) + offset), db_imap_tmap_v1_pos7.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxpos7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_pos_7)
{
    u_db_imap_tmap_v1_pos8 db_imap_tmap_v1_pos8;

    db_imap_tmap_v1_pos8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos8.u32) + offset));
    db_imap_tmap_v1_pos8.bits.im_tm_x_pos_7 = im_tm_x_pos_7;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_pos8.u32) + offset), db_imap_tmap_v1_pos8.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_3)
{
    u_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_3 = im_tm_x_num_3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset), db_imap_tmap_v1_num1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_2)
{
    u_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_2 = im_tm_x_num_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset), db_imap_tmap_v1_num1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_1)
{
    u_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_1 = im_tm_x_num_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset), db_imap_tmap_v1_num1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_0)
{
    u_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_0 = im_tm_x_num_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num1.u32) + offset), db_imap_tmap_v1_num1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_7)
{
    u_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_7 = im_tm_x_num_7;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset), db_imap_tmap_v1_num2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_6)
{
    u_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_6 = im_tm_x_num_6;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset), db_imap_tmap_v1_num2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_5)
{
    u_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_5 = im_tm_x_num_5;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset), db_imap_tmap_v1_num2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmxnum4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_x_num_4)
{
    u_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_4 = im_tm_x_num_4;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_num2.u32) + offset), db_imap_tmap_v1_num2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmm3lumcal0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_m3_lum_cal_0)
{
    u_db_imap_tmap_v1_luma_coef0 db_imap_tmap_v1_luma_coef0;

    db_imap_tmap_v1_luma_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_coef0.u32) + offset));
    db_imap_tmap_v1_luma_coef0.bits.im_tm_m3_lum_cal_0 = im_tm_m3_lum_cal_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_coef0.u32) + offset), db_imap_tmap_v1_luma_coef0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmm3lumcal1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_m3_lum_cal_1)
{
    u_db_imap_tmap_v1_luma_coef1 db_imap_tmap_v1_luma_coef1;

    db_imap_tmap_v1_luma_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_coef1.u32) + offset));
    db_imap_tmap_v1_luma_coef1.bits.im_tm_m3_lum_cal_1 = im_tm_m3_lum_cal_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_coef1.u32) + offset), db_imap_tmap_v1_luma_coef1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmm3lumcal2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_m3_lum_cal_2)
{
    u_db_imap_tmap_v1_luma_coef2 db_imap_tmap_v1_luma_coef2;

    db_imap_tmap_v1_luma_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_coef2.u32) + offset));
    db_imap_tmap_v1_luma_coef2.bits.im_tm_m3_lum_cal_2 = im_tm_m3_lum_cal_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_coef2.u32) + offset), db_imap_tmap_v1_luma_coef2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmscalelumcal(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_scale_lum_cal)
{
    u_db_imap_tmap_v1_luma_scale db_imap_tmap_v1_luma_scale;

    db_imap_tmap_v1_luma_scale.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_scale.u32) + offset));
    db_imap_tmap_v1_luma_scale.bits.im_tm_scale_lum_cal = im_tm_scale_lum_cal;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_luma_scale.u32) + offset), db_imap_tmap_v1_luma_scale.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmscalecoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_scale_coef)
{
    u_db_imap_tmap_v1_coef_scale db_imap_tmap_v1_coef_scale;

    db_imap_tmap_v1_coef_scale.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_coef_scale.u32) + offset));
    db_imap_tmap_v1_coef_scale.bits.im_tm_scale_coef = im_tm_scale_coef;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_coef_scale.u32) + offset), db_imap_tmap_v1_coef_scale.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_clip_min)
{
    u_db_imap_tmap_v1_out_clip_min db_imap_tmap_v1_out_clip_min;

    db_imap_tmap_v1_out_clip_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_clip_min.u32) + offset));
    db_imap_tmap_v1_out_clip_min.bits.im_tm_clip_min = im_tm_clip_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_clip_min.u32) + offset), db_imap_tmap_v1_out_clip_min.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_clip_max)
{
    u_db_imap_tmap_v1_out_clip_max db_imap_tmap_v1_out_clip_max;

    db_imap_tmap_v1_out_clip_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_clip_max.u32) + offset));
    db_imap_tmap_v1_out_clip_max.bits.im_tm_clip_max = im_tm_clip_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_clip_max.u32) + offset), db_imap_tmap_v1_out_clip_max.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmdcout0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_dc_out_0)
{
    u_db_imap_tmap_v1_out_dc0 db_imap_tmap_v1_out_dc0;

    db_imap_tmap_v1_out_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_dc0.u32) + offset));
    db_imap_tmap_v1_out_dc0.bits.im_tm_dc_out_0 = im_tm_dc_out_0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_dc0.u32) + offset), db_imap_tmap_v1_out_dc0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmdcout1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_dc_out_1)
{
    u_db_imap_tmap_v1_out_dc1 db_imap_tmap_v1_out_dc1;

    db_imap_tmap_v1_out_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_dc1.u32) + offset));
    db_imap_tmap_v1_out_dc1.bits.im_tm_dc_out_1 = im_tm_dc_out_1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_dc1.u32) + offset), db_imap_tmap_v1_out_dc1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimtmdcout2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_tm_dc_out_2)
{
    u_db_imap_tmap_v1_out_dc2 db_imap_tmap_v1_out_dc2;

    db_imap_tmap_v1_out_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_dc2.u32) + offset));
    db_imap_tmap_v1_out_dc2.bits.im_tm_dc_out_2 = im_tm_dc_out_2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_tmap_v1_out_dc2.u32) + offset), db_imap_tmap_v1_out_dc2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_demo_pos)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_demo_pos = im_cm_demo_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_demo_mode)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_demo_mode = im_cm_demo_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_demo_en)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_demo_en = im_cm_demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmbitdepthoutmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_bitdepth_out_mode)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_bitdepth_out_mode = im_cm_bitdepth_out_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmbitdepthinmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_bitdepth_in_mode)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_bitdepth_in_mode = im_cm_bitdepth_in_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_ck_gt_en)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_ck_gt_en = im_cm_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_en)
{
    u_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset));
    db_imap_cacm_reg0.bits.im_cm_en = im_cm_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg0.u32) + offset), db_imap_cacm_reg0.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmclutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_clut_rd_en)
{
    u_db_imap_cacm_reg1 db_imap_cacm_reg1;

    db_imap_cacm_reg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg1.u32) + offset));
    db_imap_cacm_reg1.bits.im_cm_clut_rd_en = im_cm_clut_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg1.u32) + offset), db_imap_cacm_reg1.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmclutrdatarg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_clut_rdata_rg)
{
    u_db_imap_cacm_reg2 db_imap_cacm_reg2;

    db_imap_cacm_reg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg2.u32) + offset));
    db_imap_cacm_reg2.bits.im_cm_clut_rdata_rg = im_cm_clut_rdata_rg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg2.u32) + offset), db_imap_cacm_reg2.u32);

    return ;
}

hi_void vdp_hihdr_v2_setimcmclutrdatab(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 im_cm_clut_rdata_b)
{
    u_db_imap_cacm_reg3 db_imap_cacm_reg3;

    db_imap_cacm_reg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_imap_cacm_reg3.u32) + offset));
    db_imap_cacm_reg3.bits.im_cm_clut_rdata_b = im_cm_clut_rdata_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_imap_cacm_reg3.u32) + offset), db_imap_cacm_reg3.u32);

    return ;
}


hi_void vdp_hdr_setvcvmipt2lmsen(vdp_regs_type *vdp_reg, hi_u32 v_ipt2lms_en)
{
    u_db_cvm1_ipt2lms_ctrl db_cvm1_ipt2lms_ctrl;


    db_cvm1_ipt2lms_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)));
    db_cvm1_ipt2lms_ctrl.bits.cvm1_ipt2lms_en = v_ipt2lms_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)), db_cvm1_ipt2lms_ctrl.u32);

    return ;
}

hi_void vdp_hdr_setvcvmlms2rgben(vdp_regs_type *vdp_reg, hi_u32 v_lms2rgb_en)
{
    u_db_cvm1_lms2rgb_ctrl db_cvm1_lms2rgb_ctrl;


    db_cvm1_lms2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)));
    db_cvm1_lms2rgb_ctrl.bits.cvm1_lms2rgb_en = v_lms2rgb_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)), db_cvm1_lms2rgb_ctrl.u32);

    return ;
}
hi_void vdp_hdr_setvcvmrgb2yuven(vdp_regs_type *vdp_reg, hi_u32 v_rgb2yuv_en)
{
    u_db_cvm1_rgb2yuv_ctrl db_cvm1_rgb2yuv_ctrl;


    db_cvm1_rgb2yuv_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)));
    db_cvm1_rgb2yuv_ctrl.bits.cvm1_rgb2yuv_en = v_rgb2yuv_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)), db_cvm1_rgb2yuv_ctrl.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprangeover(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_over)
{
    u_db_cvm1_out_rangeoverone db_cvm1_out_rangeoverone;


    db_cvm1_out_rangeoverone.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_out_rangeoverone.u32)));
    db_cvm1_out_rangeoverone.bits.v_omap_range_over = v_omap_range_over;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_out_rangeoverone.u32)), db_cvm1_out_rangeoverone.u32);

    return ;
}
hi_void vdp_hdr_setcvmoutbits(vdp_regs_type *vdp_reg, hi_u32 cvm_out_bits)
{
    u_db_cvm_ctrl db_cvm_ctrl;


    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.cvm_out_bits = cvm_out_bits;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}
hi_void vdp_hdr_setcvmoutcolor(vdp_regs_type *vdp_reg, hi_u32 cvm_out_color)
{
    u_db_cvm_ctrl db_cvm_ctrl;


    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.cvm_out_color = cvm_out_color;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomapiptoff(vdp_regs_type *vdp_reg, hi_s32 omap_iptoff[3]) // 3 is reg index
{
    u_db_cvm1_omap_iptoff0     db_cvm1_omap_iptoff0 ;
    u_db_cvm1_omap_iptoff1     db_cvm1_omap_iptoff1 ;
    u_db_cvm1_omap_iptoff2     db_cvm1_omap_iptoff2 ;

    db_cvm1_omap_iptoff0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff0.u32)));
    db_cvm1_omap_iptoff1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff1.u32)));
    db_cvm1_omap_iptoff2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff2.u32)));
    db_cvm1_omap_iptoff0.bits.cvm_omap_ipt_0_off = omap_iptoff[0];
    db_cvm1_omap_iptoff1.bits.cvm_omap_ipt_1_off = omap_iptoff[1];
    db_cvm1_omap_iptoff2.bits.cvm_omap_ipt_2_off = omap_iptoff[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff0.u32)), db_cvm1_omap_iptoff0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff1.u32)), db_cvm1_omap_iptoff1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptoff2.u32)), db_cvm1_omap_iptoff2.u32);
}

hi_void vdp_hdr_setvcvmomapiptscale(vdp_regs_type *vdp_reg, hi_u32 scale)
{
    u_db_cvm1_omap_iptscale     db_cvm1_omap_iptscale ;

    db_cvm1_omap_iptscale.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptscale.u32)));
    db_cvm1_omap_iptscale.bits.cvm_omap_ipt_scale = scale;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_omap_iptscale.u32)), db_cvm1_omap_iptscale.u32);
}

hi_void vdp_hdr_setvdmomaplms2rgbmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbmin)
{
    u_db_cvm1_lms2rgb_min db_cvm1_lms2rgb_min;


    db_cvm1_lms2rgb_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_min.u32)));
    db_cvm1_lms2rgb_min.bits.v_omap_lms2rgbmin = v_omap_lms2rgbmin;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_min.u32)), db_cvm1_lms2rgb_min.u32);

    return ;
}

hi_void vdp_hdr_setvdmomaplms2rgbmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbmax)
{
    u_db_cvm1_lms2rgb_max db_cvm1_lms2rgb_max;


    db_cvm1_lms2rgb_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_max.u32)));
    db_cvm1_lms2rgb_max.bits.v_omap_lms2rgbmax = v_omap_lms2rgbmax;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_max.u32)), db_cvm1_lms2rgb_max.u32);

    return ;
}

hi_void vdp_hdr_setvdmomapipt2lmsmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsmin)
{
    u_db_cvm1_ipt2lms_min db_cvm1_ipt2lms_min;


    db_cvm1_ipt2lms_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_min.u32)));
    db_cvm1_ipt2lms_min.bits.v_omap_ipt2lmsmin = v_omap_ipt2lmsmin;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_min.u32)), db_cvm1_ipt2lms_min.u32);

    return ;
}

hi_void vdp_hdr_setvdmomapipt2lmsmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsmax)
{
    u_db_cvm1_ipt2lms_max db_cvm1_ipt2lms_max;


    db_cvm1_ipt2lms_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_max.u32)));
    db_cvm1_ipt2lms_max.bits.v_omap_ipt2lmsmax = v_omap_ipt2lmsmax;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_max.u32)), db_cvm1_ipt2lms_max.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprgb2yuvmin(vdp_regs_type *vdp_reg, hi_u32 v_omap_ygb2yuvmin)
{
    u_db_cvm1_rgb2yuv_min db_cvm1_rgb2yuv_min;


    db_cvm1_rgb2yuv_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_min.u32)));
    db_cvm1_rgb2yuv_min.bits.v_omap_rgb2yuv_min = v_omap_ygb2yuvmin;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_min.u32)), db_cvm1_rgb2yuv_min.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprgb2yuvmax(vdp_regs_type *vdp_reg, hi_u32 v_omap_ygb2yuvmax)
{
    u_db_cvm1_rgb2yuv_max db_cvm1_rgb2yuv_max;


    db_cvm1_rgb2yuv_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_max.u32)));
    db_cvm1_rgb2yuv_max.bits.v_omap_rgb2yuv_max = v_omap_ygb2yuvmax;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_max.u32)), db_cvm1_rgb2yuv_max.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomapipt2lms(vdp_regs_type *vdp_reg, hi_s16 v_omap_ipt2lms[3][3]) // 3 is reg index
{
    u_db_cvm1_ipt2lms_coef0 db_cvm1_ipt2lms_coef0;
    u_db_cvm1_ipt2lms_coef1 db_cvm1_ipt2lms_coef1;
    u_db_cvm1_ipt2lms_coef2 db_cvm1_ipt2lms_coef2;
    u_db_cvm1_ipt2lms_coef3 db_cvm1_ipt2lms_coef3;
    u_db_cvm1_ipt2lms_coef4 db_cvm1_ipt2lms_coef4;
    u_db_cvm1_ipt2lms_coef5 db_cvm1_ipt2lms_coef5;
    u_db_cvm1_ipt2lms_coef6 db_cvm1_ipt2lms_coef6;
    u_db_cvm1_ipt2lms_coef7 db_cvm1_ipt2lms_coef7;
    u_db_cvm1_ipt2lms_coef8 db_cvm1_ipt2lms_coef8;


    db_cvm1_ipt2lms_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef0.u32)));
    db_cvm1_ipt2lms_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef1.u32)));
    db_cvm1_ipt2lms_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef2.u32)));
    db_cvm1_ipt2lms_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef3.u32)));
    db_cvm1_ipt2lms_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef4.u32)));
    db_cvm1_ipt2lms_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef5.u32)));
    db_cvm1_ipt2lms_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef6.u32)));
    db_cvm1_ipt2lms_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef7.u32)));
    db_cvm1_ipt2lms_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef8.u32)));
    db_cvm1_ipt2lms_coef0.bits.v_omap_00_ipt2lms = v_omap_ipt2lms[0][0];
    db_cvm1_ipt2lms_coef1.bits.v_omap_01_ipt2lms = v_omap_ipt2lms[0][1];
    db_cvm1_ipt2lms_coef2.bits.v_omap_02_ipt2lms = v_omap_ipt2lms[0][2]; // 2 is reg index
    db_cvm1_ipt2lms_coef3.bits.v_omap_10_ipt2lms = v_omap_ipt2lms[1][0];
    db_cvm1_ipt2lms_coef4.bits.v_omap_11_ipt2lms = v_omap_ipt2lms[1][1];
    db_cvm1_ipt2lms_coef5.bits.v_omap_12_ipt2lms = v_omap_ipt2lms[1][2]; // 2 is reg index
    db_cvm1_ipt2lms_coef6.bits.v_omap_20_ipt2lms = v_omap_ipt2lms[2][0]; // 2 is reg index
    db_cvm1_ipt2lms_coef7.bits.v_omap_21_ipt2lms = v_omap_ipt2lms[2][1]; // 2 is reg index
    db_cvm1_ipt2lms_coef8.bits.v_omap_22_ipt2lms = v_omap_ipt2lms[2][2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef0.u32)), db_cvm1_ipt2lms_coef0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef1.u32)), db_cvm1_ipt2lms_coef1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef2.u32)), db_cvm1_ipt2lms_coef2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef3.u32)), db_cvm1_ipt2lms_coef3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef4.u32)), db_cvm1_ipt2lms_coef4.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef5.u32)), db_cvm1_ipt2lms_coef5.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef6.u32)), db_cvm1_ipt2lms_coef6.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef7.u32)), db_cvm1_ipt2lms_coef7.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_coef8.u32)), db_cvm1_ipt2lms_coef8.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomapipt2lmsscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_ipt2lmsscale2p)
{
    u_db_cvm1_ipt2lms_scale2p db_cvm1_ipt2lms_scale2p;


    db_cvm1_ipt2lms_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_scale2p.u32)));
    db_cvm1_ipt2lms_scale2p.bits.v_omap_ipt2lmsscale2p = v_omap_ipt2lmsscale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_scale2p.u32)), db_cvm1_ipt2lms_scale2p.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaplms2rgb(vdp_regs_type *vdp_reg, hi_s16 v_omap_lms2rgb[3][3]) // 3 is reg index
{
    u_db_cvm1_lms2rgb_coef0 db_cvm1_lms2rgb_coef0;
    u_db_cvm1_lms2rgb_coef1 db_cvm1_lms2rgb_coef1;
    u_db_cvm1_lms2rgb_coef2 db_cvm1_lms2rgb_coef2;
    u_db_cvm1_lms2rgb_coef3 db_cvm1_lms2rgb_coef3;
    u_db_cvm1_lms2rgb_coef4 db_cvm1_lms2rgb_coef4;
    u_db_cvm1_lms2rgb_coef5 db_cvm1_lms2rgb_coef5;
    u_db_cvm1_lms2rgb_coef6 db_cvm1_lms2rgb_coef6;
    u_db_cvm1_lms2rgb_coef7 db_cvm1_lms2rgb_coef7;
    u_db_cvm1_lms2rgb_coef8 db_cvm1_lms2rgb_coef8;


    db_cvm1_lms2rgb_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)));
    db_cvm1_lms2rgb_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)));
    db_cvm1_lms2rgb_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)));
    db_cvm1_lms2rgb_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)));
    db_cvm1_lms2rgb_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)));
    db_cvm1_lms2rgb_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)));
    db_cvm1_lms2rgb_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)));
    db_cvm1_lms2rgb_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)));
    db_cvm1_lms2rgb_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)));
    db_cvm1_lms2rgb_coef0.bits.v_omap_00_lms2rgb = v_omap_lms2rgb[0][0];
    db_cvm1_lms2rgb_coef1.bits.v_omap_01_lms2rgb = v_omap_lms2rgb[0][1];
    db_cvm1_lms2rgb_coef2.bits.v_omap_02_lms2rgb = v_omap_lms2rgb[0][2]; // 2 is reg index
    db_cvm1_lms2rgb_coef3.bits.v_omap_10_lms2rgb = v_omap_lms2rgb[1][0];
    db_cvm1_lms2rgb_coef4.bits.v_omap_11_lms2rgb = v_omap_lms2rgb[1][1];
    db_cvm1_lms2rgb_coef5.bits.v_omap_12_lms2rgb = v_omap_lms2rgb[1][2]; // 2 is reg index
    db_cvm1_lms2rgb_coef6.bits.v_omap_20_lms2rgb = v_omap_lms2rgb[2][0]; // 2 is reg index
    db_cvm1_lms2rgb_coef7.bits.v_omap_21_lms2rgb = v_omap_lms2rgb[2][1]; // 2 is reg index
    db_cvm1_lms2rgb_coef8.bits.v_omap_22_lms2rgb = v_omap_lms2rgb[2][2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)), db_cvm1_lms2rgb_coef0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)), db_cvm1_lms2rgb_coef1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)), db_cvm1_lms2rgb_coef2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)), db_cvm1_lms2rgb_coef3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)), db_cvm1_lms2rgb_coef4.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)), db_cvm1_lms2rgb_coef5.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)), db_cvm1_lms2rgb_coef6.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)), db_cvm1_lms2rgb_coef7.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)), db_cvm1_lms2rgb_coef8.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaplms2rgbdemo(vdp_regs_type *vdp_reg, hi_s16 v_omap_lms2rgb[3][3]) // 3 is reg index
{
    u_db_cvm1_lms2rgb_coef0_demo db_cvm1_lms2rgb_coef0;
    u_db_cvm1_lms2rgb_coef1_demo db_cvm1_lms2rgb_coef1;
    u_db_cvm1_lms2rgb_coef2_demo db_cvm1_lms2rgb_coef2;
    u_db_cvm1_lms2rgb_coef3_demo db_cvm1_lms2rgb_coef3;
    u_db_cvm1_lms2rgb_coef4_demo db_cvm1_lms2rgb_coef4;
    u_db_cvm1_lms2rgb_coef5_demo db_cvm1_lms2rgb_coef5;
    u_db_cvm1_lms2rgb_coef6_demo db_cvm1_lms2rgb_coef6;
    u_db_cvm1_lms2rgb_coef7_demo db_cvm1_lms2rgb_coef7;
    u_db_cvm1_lms2rgb_coef8_demo db_cvm1_lms2rgb_coef8;


    db_cvm1_lms2rgb_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)));
    db_cvm1_lms2rgb_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)));
    db_cvm1_lms2rgb_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)));
    db_cvm1_lms2rgb_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)));
    db_cvm1_lms2rgb_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)));
    db_cvm1_lms2rgb_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)));
    db_cvm1_lms2rgb_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)));
    db_cvm1_lms2rgb_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)));
    db_cvm1_lms2rgb_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)));
    db_cvm1_lms2rgb_coef0.bits.v_omap_00_lms2rgb_demo = v_omap_lms2rgb[0][0];
    db_cvm1_lms2rgb_coef1.bits.v_omap_01_lms2rgb_demo = v_omap_lms2rgb[0][1];
    db_cvm1_lms2rgb_coef2.bits.v_omap_02_lms2rgb_demo = v_omap_lms2rgb[0][2]; // 2 is reg index
    db_cvm1_lms2rgb_coef3.bits.v_omap_10_lms2rgb_demo = v_omap_lms2rgb[1][0];
    db_cvm1_lms2rgb_coef4.bits.v_omap_11_lms2rgb_demo = v_omap_lms2rgb[1][1];
    db_cvm1_lms2rgb_coef5.bits.v_omap_12_lms2rgb_demo = v_omap_lms2rgb[1][2]; // 2 is reg index
    db_cvm1_lms2rgb_coef6.bits.v_omap_20_lms2rgb_demo = v_omap_lms2rgb[2][0]; // 2 is reg index
    db_cvm1_lms2rgb_coef7.bits.v_omap_21_lms2rgb_demo = v_omap_lms2rgb[2][1]; // 2 is reg index
    db_cvm1_lms2rgb_coef8.bits.v_omap_22_lms2rgb_demo = v_omap_lms2rgb[2][2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef0.u32)), db_cvm1_lms2rgb_coef0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef1.u32)), db_cvm1_lms2rgb_coef1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef2.u32)), db_cvm1_lms2rgb_coef2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef3.u32)), db_cvm1_lms2rgb_coef3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef4.u32)), db_cvm1_lms2rgb_coef4.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef5.u32)), db_cvm1_lms2rgb_coef5.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef6.u32)), db_cvm1_lms2rgb_coef6.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef7.u32)), db_cvm1_lms2rgb_coef7.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_coef8.u32)), db_cvm1_lms2rgb_coef8.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaplms2rgbscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbscale2p)
{
    u_db_cvm1_lms2rgb_scale2p db_cvm1_lms2rgb_scale2p;


    db_cvm1_lms2rgb_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)));
    db_cvm1_lms2rgb_scale2p.bits.v_omap_lms2rgbscale2p = v_omap_lms2rgbscale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)), db_cvm1_lms2rgb_scale2p.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaplms2rgbscale2pdemo(vdp_regs_type *vdp_reg, hi_u32 v_omap_lms2rgbscale2p)
{
    u_db_cvm1_lms2rgb_scale2p_demo db_cvm1_lms2rgb_scale2p;


    db_cvm1_lms2rgb_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)));
    db_cvm1_lms2rgb_scale2p.bits.v_omap_lms2rgbscale2p_demo = v_omap_lms2rgbscale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_scale2p.u32)), db_cvm1_lms2rgb_scale2p.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprgb2yuv(vdp_regs_type *vdp_reg, hi_s16 v_omap_rgb2yuv[3][3]) // 3 is reg index
{
    u_db_cvm1_rgb2yuv_coef0 db_cvm1_rgb2yuv_coef0;
    u_db_cvm1_rgb2yuv_coef1 db_cvm1_rgb2yuv_coef1;
    u_db_cvm1_rgb2yuv_coef2 db_cvm1_rgb2yuv_coef2;
    u_db_cvm1_rgb2yuv_coef3 db_cvm1_rgb2yuv_coef3;
    u_db_cvm1_rgb2yuv_coef4 db_cvm1_rgb2yuv_coef4;
    u_db_cvm1_rgb2yuv_coef5 db_cvm1_rgb2yuv_coef5;
    u_db_cvm1_rgb2yuv_coef6 db_cvm1_rgb2yuv_coef6;
    u_db_cvm1_rgb2yuv_coef7 db_cvm1_rgb2yuv_coef7;
    u_db_cvm1_rgb2yuv_coef8 db_cvm1_rgb2yuv_coef8;


    db_cvm1_rgb2yuv_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef0.u32)));
    db_cvm1_rgb2yuv_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef1.u32)));
    db_cvm1_rgb2yuv_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef2.u32)));
    db_cvm1_rgb2yuv_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef3.u32)));
    db_cvm1_rgb2yuv_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef4.u32)));
    db_cvm1_rgb2yuv_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef5.u32)));
    db_cvm1_rgb2yuv_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef6.u32)));
    db_cvm1_rgb2yuv_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef7.u32)));
    db_cvm1_rgb2yuv_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef8.u32)));
    db_cvm1_rgb2yuv_coef0.bits.v_omap_00_rgb2yuv = v_omap_rgb2yuv[0][0];
    db_cvm1_rgb2yuv_coef1.bits.v_omap_01_rgb2yuv = v_omap_rgb2yuv[0][1];
    db_cvm1_rgb2yuv_coef2.bits.v_omap_02_rgb2yuv = v_omap_rgb2yuv[0][2]; // 2 is reg index
    db_cvm1_rgb2yuv_coef3.bits.v_omap_10_rgb2yuv = v_omap_rgb2yuv[1][0];
    db_cvm1_rgb2yuv_coef4.bits.v_omap_11_rgb2yuv = v_omap_rgb2yuv[1][1];
    db_cvm1_rgb2yuv_coef5.bits.v_omap_12_rgb2yuv = v_omap_rgb2yuv[1][2]; // 2 is reg index
    db_cvm1_rgb2yuv_coef6.bits.v_omap_20_rgb2yuv = v_omap_rgb2yuv[2][0]; // 2 is reg index
    db_cvm1_rgb2yuv_coef7.bits.v_omap_21_rgb2yuv = v_omap_rgb2yuv[2][1]; // 2 is reg index
    db_cvm1_rgb2yuv_coef8.bits.v_omap_22_rgb2yuv = v_omap_rgb2yuv[2][2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef0.u32)), db_cvm1_rgb2yuv_coef0.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef1.u32)), db_cvm1_rgb2yuv_coef1.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef2.u32)), db_cvm1_rgb2yuv_coef2.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef3.u32)), db_cvm1_rgb2yuv_coef3.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef4.u32)), db_cvm1_rgb2yuv_coef4.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef5.u32)), db_cvm1_rgb2yuv_coef5.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef6.u32)), db_cvm1_rgb2yuv_coef6.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef7.u32)), db_cvm1_rgb2yuv_coef7.u32);
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_coef8.u32)), db_cvm1_rgb2yuv_coef8.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprgb2yuvscale2p(vdp_regs_type *vdp_reg, hi_u32 v_omap_rgb2yuvscale2p)
{
    u_db_cvm1_rgb2yuv_scale2p db_cvm1_rgb2yuv_scale2p;


    db_cvm1_rgb2yuv_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_scale2p.u32)));
    db_cvm1_rgb2yuv_scale2p.bits.v_omap_rgb2yuvscale2p = v_omap_rgb2yuvscale2p;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_scale2p.u32)), db_cvm1_rgb2yuv_scale2p.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprgb2yuvoutdc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_rgb2yuv_out_dc)
{
    u_db_cvm1_rgb2yuv_out_dc0 db_cvm1_rgb2yuv_out_dc0;
    u_db_cvm1_rgb2yuv_out_dc1 db_cvm1_rgb2yuv_out_dc1;
    u_db_cvm1_rgb2yuv_out_dc2 db_cvm1_rgb2yuv_out_dc2;


    db_cvm1_rgb2yuv_out_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc0.u32)));
    db_cvm1_rgb2yuv_out_dc0.bits.v_omap_rgb2yuv_0_out_dc = v_omap_rgb2yuv_out_dc[0];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc0.u32)), db_cvm1_rgb2yuv_out_dc0.u32);

    db_cvm1_rgb2yuv_out_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc1.u32)));
    db_cvm1_rgb2yuv_out_dc1.bits.v_omap_rgb2yuv_1_out_dc = v_omap_rgb2yuv_out_dc[1];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc1.u32)), db_cvm1_rgb2yuv_out_dc1.u32);

    db_cvm1_rgb2yuv_out_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc2.u32)));
    db_cvm1_rgb2yuv_out_dc2.bits.v_omap_rgb2yuv_2_out_dc = v_omap_rgb2yuv_out_dc[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_out_dc2.u32)), db_cvm1_rgb2yuv_out_dc2.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprgb2yuvindc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_rgb2yuv_in_dc)
{
    u_db_cvm1_rgb2yuv_in_dc0 db_cvm1_rgb2yuv_in_dc0;
    u_db_cvm1_rgb2yuv_in_dc1 db_cvm1_rgb2yuv_in_dc1;
    u_db_cvm1_rgb2yuv_in_dc2 db_cvm1_rgb2yuv_in_dc2;


    db_cvm1_rgb2yuv_in_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc0.u32)));
    db_cvm1_rgb2yuv_in_dc0.bits.om_v3_rgb2yuv_dc_in_0 = v_omap_rgb2yuv_in_dc[0];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc0.u32)), db_cvm1_rgb2yuv_in_dc0.u32);

    db_cvm1_rgb2yuv_in_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc1.u32)));
    db_cvm1_rgb2yuv_in_dc1.bits.om_v3_rgb2yuv_dc_in_1 = v_omap_rgb2yuv_in_dc[1];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc1.u32)), db_cvm1_rgb2yuv_in_dc1.u32);

    db_cvm1_rgb2yuv_in_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc2.u32)));
    db_cvm1_rgb2yuv_in_dc2.bits.om_v3_rgb2yuv_dc_in_2 = v_omap_rgb2yuv_in_dc[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_in_dc2.u32)), db_cvm1_rgb2yuv_in_dc2.u32);


    return ;
}

hi_void vdp_hdr_setvcvmomapipt2lmsoutdc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_ipt2lms_out_dc)
{
    u_db_cvm1_ipt2lms_out_dc00 db_cvm1_ipt2lms_out_dc00;
    u_db_cvm1_ipt2lms_out_dc01 db_cvm1_ipt2lms_out_dc01;
    u_db_cvm1_ipt2lms_out_dc02 db_cvm1_ipt2lms_out_dc02;


    db_cvm1_ipt2lms_out_dc00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc00.u32)));
    db_cvm1_ipt2lms_out_dc00.bits.om_v3_ipt2lms_dc_out_0 = v_omap_ipt2lms_out_dc[0];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc00.u32)), db_cvm1_ipt2lms_out_dc00.u32);

    db_cvm1_ipt2lms_out_dc01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc01.u32)));
    db_cvm1_ipt2lms_out_dc01.bits.om_v3_ipt2lms_dc_out_1 = v_omap_ipt2lms_out_dc[1];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc01.u32)), db_cvm1_ipt2lms_out_dc01.u32);

    db_cvm1_ipt2lms_out_dc02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc02.u32)));
    db_cvm1_ipt2lms_out_dc02.bits.om_v3_ipt2lms_dc_out_2 = v_omap_ipt2lms_out_dc[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_out_dc02.u32)), db_cvm1_ipt2lms_out_dc02.u32);


    return ;
}

hi_void vdp_hdr_setvcvmomapipt2lmsindc(vdp_regs_type *vdp_reg, hi_s32 *v_omap_ipt2lms_in_dc)
{
    u_db_cvm1_ipt2lms_in_dc00 db_cvm1_ipt2lms_in_dc00;
    u_db_cvm1_ipt2lms_in_dc01 db_cvm1_ipt2lms_in_dc01;
    u_db_cvm1_ipt2lms_in_dc02 db_cvm1_ipt2lms_in_dc02;

    db_cvm1_ipt2lms_in_dc00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc00.u32)));
    db_cvm1_ipt2lms_in_dc00.bits.om_v3_ipt2lms_dc_in_0 = v_omap_ipt2lms_in_dc[0];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc00.u32)), db_cvm1_ipt2lms_in_dc00.u32);

    db_cvm1_ipt2lms_in_dc01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc01.u32)));
    db_cvm1_ipt2lms_in_dc01.bits.om_v3_ipt2lms_dc_in_1 = v_omap_ipt2lms_in_dc[1];
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc01.u32)), db_cvm1_ipt2lms_in_dc01.u32);

    db_cvm1_ipt2lms_in_dc02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc02.u32)));
    db_cvm1_ipt2lms_in_dc02.bits.om_v3_ipt2lms_dc_in_2 = v_omap_ipt2lms_in_dc[2]; // 2 is reg index
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_in_dc02.u32)), db_cvm1_ipt2lms_in_dc02.u32);

    return ;
}

hi_void vdp_hdr_setvcvmomaprangemin(vdp_regs_type *vdp_reg, hi_u32 v_omap_range_min)
{
    u_db_cvm1_out_rangemin db_cvm1_out_rangemin;


    db_cvm1_out_rangemin.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_out_rangemin.u32)));
    db_cvm1_out_rangemin.bits.v_omap_range_min = v_omap_range_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_out_rangemin.u32)), db_cvm1_out_rangemin.u32);

    return ;
}
hi_void vdp_hipp_dbhdr_setomen(vdp_regs_type *vdp_reg, hi_u32 om_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_en = om_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomu2sen(vdp_regs_type *vdp_reg, hi_u32 om_u2s_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_u2s_en = om_u2s_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}
hi_void vdp_hipp_dbhdr_setomlshiften(vdp_regs_type *vdp_reg, hi_u32 om_lshift_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_lshift_en = om_lshift_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}
hi_void vdp_hipp_dbhdr_setcvm1ipt2lmsen(vdp_regs_type *vdp_reg, hi_u32 cvm1_ipt2lms_en)
{
    u_db_cvm1_ipt2lms_ctrl db_cvm1_ipt2lms_ctrl;

    db_cvm1_ipt2lms_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)));
    db_cvm1_ipt2lms_ctrl.bits.cvm1_ipt2lms_en = cvm1_ipt2lms_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_ipt2lms_ctrl.u32)), db_cvm1_ipt2lms_ctrl.u32);

    return ;
}
hi_void vdp_hipp_dbhdr_setomdegammaen(vdp_regs_type *vdp_reg, hi_u32 om_degamma_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_degamma_en = om_degamma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}
hi_void vdp_hipp_dbhdr_setcvm1lms2rgben(vdp_regs_type *vdp_reg, hi_u32 cvm1_lms2rgb_en)
{
    u_db_cvm1_lms2rgb_ctrl db_cvm1_lms2rgb_ctrl;

    db_cvm1_lms2rgb_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)));
    db_cvm1_lms2rgb_ctrl.bits.cvm1_lms2rgb_en = cvm1_lms2rgb_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_lms2rgb_ctrl.u32)), db_cvm1_lms2rgb_ctrl.u32);

    return ;
}
hi_void vdp_hipp_dbhdr_setomgammaen(vdp_regs_type *vdp_reg, hi_u32 om_gamma_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.om_gamma_en = om_gamma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setvdenormen(vdp_regs_type *vdp_reg, hi_u32 v_denorm_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.v_denorm_en = v_denorm_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomvcmen(vdp_regs_type *vdp_reg, hi_u32 om_vcvm_en)
{
    u_db_cvm_ctrl db_cvm_ctrl;

    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.vcvm_en = om_vcvm_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmen(vdp_regs_type *vdp_reg, hi_u32 om_cm_en)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_en = om_cm_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmckgten(vdp_regs_type *vdp_reg, hi_u32 om_cm_ck_gt_en)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_ck_gt_en = om_cm_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmbitdepthinmode(vdp_regs_type *vdp_reg, hi_u32 om_cm_bitdepth_in_mode)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_bitdepth_in_mode = om_cm_bitdepth_in_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmbitdepthoutmode(vdp_regs_type *vdp_reg, hi_u32 om_cm_bitdepth_out_mode)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_bitdepth_out_mode = om_cm_bitdepth_out_mode ;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmdemoen(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_en)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_demo_en = om_cm_demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmdemomode(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_mode)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_demo_mode = om_cm_demo_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setomcmdemopos(vdp_regs_type *vdp_reg, hi_u32 om_cm_demo_pos)
{
    u_db_hipp_cm2_reg0 db_hipp_cm2_reg0;

    db_hipp_cm2_reg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)));
    db_hipp_cm2_reg0.bits.om_cm_demo_pos = om_cm_demo_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_hipp_cm2_reg0.u32)), db_hipp_cm2_reg0.u32);

    return ;
}

hi_void vdp_hipp_dbhdr_setcvm1rgb2yuven(vdp_regs_type *vdp_reg, hi_u32 cvm1_rgb2yuv_en)
{
    u_db_cvm1_rgb2yuv_ctrl db_cvm1_rgb2yuv_ctrl;

    db_cvm1_rgb2yuv_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)));
    db_cvm1_rgb2yuv_ctrl.bits.cvm1_rgb2yuv_en = cvm1_rgb2yuv_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm1_rgb2yuv_ctrl.u32)), db_cvm1_rgb2yuv_ctrl.u32);

    return ;
}

hi_void vdp_hdr_setvcvmoutoetf(vdp_regs_type *vdp_reg, hi_u32 cvm_out_oetf)
{
    u_db_cvm_ctrl db_cvm_ctrl;


    db_cvm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)));
    db_cvm_ctrl.bits.cvm_out_oetf = cvm_out_oetf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->db_cvm_ctrl.u32)), db_cvm_ctrl.u32);

    return ;
}


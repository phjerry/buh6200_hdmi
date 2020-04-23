/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */
#include "pq_hal_vpsshdr_regset.h"
#include "pq_hal_hdrv2_regset_comm.h"
#include "pq_hal_comm.h"

hi_void vpss_hihdr_v2_set_im_cm_pos(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_pos)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_cm_pos = im_cm_pos;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_pos(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_pos)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_tm_pos = im_tm_pos;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_rshift_round_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 im_rshift_round_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_rshift_round_en = im_rshift_round_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_rshift_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_rshift_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_rshift_en = im_rshift_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_ck_gt_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_ck_gt_en = im_ck_gt_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_ipt_in_sel(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_ipt_in_sel)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_ipt_in_sel = im_ipt_in_sel;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_ladj_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_ladj_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_ladj_en = im_ladj_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_eotfparam_eotf(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 imap_eotfparam_eotf)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.imap_eotfparam_eotf = imap_eotfparam_eotf;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_in_color(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_in_color)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.imap_in_color = imap_in_color;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_in_bits(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_in_bits)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_in_bits = im_in_bits;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_gamma_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_gamma_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_gamma_en = im_gamma_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_degamma_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_degamma_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_degamma_en = im_degamma_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_norm_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_norm_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_norm_en = im_norm_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_en)
{
    u_vpss_db_imap_ctrl db_imap_ctrl;

    db_imap_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ctrl.u32) + addr_offset));
    db_imap_ctrl.bits.im_en = im_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ctrl.u32) + addr_offset), db_imap_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_eotfparam_range_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_eotfparam_range_min)
{
    u_vpss_db_imap_in_rangemin db_imap_in_rangemin;

    db_imap_in_rangemin.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_in_rangemin.u32) + addr_offset));
    db_imap_in_rangemin.bits.imap_eotfparam_range_min = imap_eotfparam_range_min;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_in_rangemin.u32) + addr_offset), db_imap_in_rangemin.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_eotfparam_range(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 imap_eotfparam_range)
{
    u_vpss_db_imap_in_range db_imap_in_range;

    db_imap_in_range.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_in_range.u32) + addr_offset));
    db_imap_in_range.bits.imap_eotfparam_range = imap_eotfparam_range;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_in_range.u32) + addr_offset), db_imap_in_range.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_eotfparam_range_inv(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_eotfparam_range_inv)
{
    u_vpss_db_imap_in_rangeinv db_imap_in_rangeinv;

    db_imap_in_rangeinv.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_in_rangeinv.u32) + addr_offset));
    db_imap_in_rangeinv.bits.imap_eotfparam_range_inv = imap_eotfparam_range_inv;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_in_rangeinv.u32) + addr_offset), db_imap_in_rangeinv.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3ipt_off0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_v3ipt_off_0)
{
    u_vpss_db_imap_iptoff0 db_imap_iptoff0;

    db_imap_iptoff0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_iptoff0.u32) + addr_offset));
    db_imap_iptoff0.bits.imap_v3ipt_off_0 = imap_v3ipt_off_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_iptoff0.u32) + addr_offset), db_imap_iptoff0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3ipt_off1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_v3ipt_off_1)
{
    u_vpss_db_imap_iptoff1 db_imap_iptoff1;

    db_imap_iptoff1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_iptoff1.u32) + addr_offset));
    db_imap_iptoff1.bits.imap_v3ipt_off_1 = imap_v3ipt_off_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_iptoff1.u32) + addr_offset), db_imap_iptoff1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3ipt_off2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_v3ipt_off_2)
{
    u_vpss_db_imap_iptoff2 db_imap_iptoff2;

    db_imap_iptoff2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_iptoff2.u32) + addr_offset));
    db_imap_iptoff2.bits.imap_v3ipt_off_2 = imap_v3ipt_off_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_iptoff2.u32) + addr_offset), db_imap_iptoff2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_ipt_scale(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_ipt_scale)
{
    u_vpss_db_imap_iptscale db_imap_iptscale;

    db_imap_iptscale.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_iptscale.u32) + addr_offset));
    db_imap_iptscale.bits.imap_ipt_scale = imap_ipt_scale;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_iptscale.u32) + addr_offset), db_imap_iptscale.u32);

    return;
}

hi_void vpss_hihdr_v2_set_degamma_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 degamma_rd_en)
{
    u_vpss_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_ren.u32) + addr_offset));
    db_imap_para_ren.bits.degamma_rd_en = degamma_rd_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_ren.u32) + addr_offset), db_imap_para_ren.u32);

    return;
}

hi_void vpss_hihdr_v2_set_gamma_lutb_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 gamma_lutb_rd_en)
{
    u_vpss_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_ren.u32) + addr_offset));
    db_imap_para_ren.bits.gamma_lutb_rd_en = gamma_lutb_rd_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_ren.u32) + addr_offset), db_imap_para_ren.u32);

    return;
}

hi_void vpss_hihdr_v2_set_gamma_luta_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 gamma_luta_rd_en)
{
    u_vpss_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_ren.u32) + addr_offset));
    db_imap_para_ren.bits.gamma_luta_rd_en = gamma_luta_rd_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_ren.u32) + addr_offset), db_imap_para_ren.u32);

    return;
}

hi_void vpss_hihdr_v2_set_gamma_lutx_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 gamma_lutx_rd_en)
{
    u_vpss_db_imap_para_ren db_imap_para_ren;

    db_imap_para_ren.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_ren.u32) + addr_offset));
    db_imap_para_ren.bits.gamma_lutx_rd_en = gamma_lutx_rd_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_ren.u32) + addr_offset), db_imap_para_ren.u32);

    return;
}

hi_void vpss_hihdr_v2_set_vdm_para_data(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vdm_para_data)
{
    u_vpss_db_imap_para_data db_imap_para_data;

    db_imap_para_data.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_data.u32) + addr_offset));
    db_imap_para_data.bits.vdm_para_data = vdm_para_data;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_data.u32) + addr_offset), db_imap_para_data.u32);

    return;
}

hi_void vpss_hihdr_v2_set_vdm_coef1_upd(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vdm_coef1_upd)
{
    u_vpss_db_imap_para_up db_imap_para_up;

    db_imap_para_up.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_up.u32) + addr_offset));
    db_imap_para_up.bits.vdm_coef1_upd = vdm_coef1_upd;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_up.u32) + addr_offset), db_imap_para_up.u32);

    return;
}

hi_void vpss_hihdr_v2_set_vdm_coef_upd(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vdm_coef_upd)
{
    u_vpss_db_imap_para_up db_imap_para_up;

    db_imap_para_up.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_para_up.u32) + addr_offset));
    db_imap_para_up.bits.vdm_coef_upd = vdm_coef_upd;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_para_up.u32) + addr_offset), db_imap_para_up.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_degamma_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_degamma_clip_max)
{
    u_vpss_db_imap_degamma_clip_max db_imap_degamma_clip_max;

    db_imap_degamma_clip_max.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_degamma_clip_max.u32) + addr_offset));
    db_imap_degamma_clip_max.bits.im_degamma_clip_max = im_degamma_clip_max;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_degamma_clip_max.u32) + addr_offset), db_imap_degamma_clip_max.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_degamma_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_degamma_clip_min)
{
    u_vpss_db_imap_degamma_clip_min db_imap_degamma_clip_min;

    db_imap_degamma_clip_min.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_degamma_clip_min.u32) + addr_offset));
    db_imap_degamma_clip_min.bits.im_degamma_clip_min = im_degamma_clip_min;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_degamma_clip_min.u32) + addr_offset), db_imap_degamma_clip_min.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_ladj_chroma_weight(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_ladj_chroma_weight)
{
    u_vpss_db_imap_ladj_weight db_imap_ladj_weight;

    db_imap_ladj_weight.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ladj_weight.u32) + addr_offset));
    db_imap_ladj_weight.bits.imap_ladj_chroma_weight = imap_ladj_chroma_weight;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ladj_weight.u32) + addr_offset), db_imap_ladj_weight.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_demo_luma_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_demo_luma_mode)
{
    u_vpss_db_imap_ladj_weight db_imap_ladj_weight;

    db_imap_ladj_weight.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ladj_weight.u32) + addr_offset));
    db_imap_ladj_weight.bits.im_demo_luma_mode = im_demo_luma_mode;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ladj_weight.u32) + addr_offset), db_imap_ladj_weight.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_demo_luma_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_demo_luma_en)
{
    u_vpss_db_imap_ladj_weight db_imap_ladj_weight;

    db_imap_ladj_weight.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_ladj_weight.u32) + addr_offset));
    db_imap_ladj_weight.bits.im_demo_luma_en = im_demo_luma_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_ladj_weight.u32) + addr_offset), db_imap_ladj_weight.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v1_hdr_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v1_hdr_en)
{
    u_vpss_db_imap_yuv2_rgb_ctrl db_imap_yuv2_rgb_ctrl;

    db_imap_yuv2_rgb_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset));
    db_imap_yuv2_rgb_ctrl.bits.im_v1_hdr_en = im_v1_hdr_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset), db_imap_yuv2_rgb_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v0_hdr_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v0_hdr_en)
{
    u_vpss_db_imap_yuv2_rgb_ctrl db_imap_yuv2_rgb_ctrl;

    db_imap_yuv2_rgb_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset));
    db_imap_yuv2_rgb_ctrl.bits.im_v0_hdr_en = im_v0_hdr_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset), db_imap_yuv2_rgb_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_yuv2rgb_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_mode)
{
    u_vpss_db_imap_yuv2_rgb_ctrl db_imap_yuv2_rgb_ctrl;

    db_imap_yuv2_rgb_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset));
    db_imap_yuv2_rgb_ctrl.bits.im_yuv2rgb_mode = im_yuv2rgb_mode;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset), db_imap_yuv2_rgb_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v1_y2r_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v1_y2r_en)
{
    u_vpss_db_imap_yuv2_rgb_ctrl db_imap_yuv2_rgb_ctrl;

    db_imap_yuv2_rgb_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset));
    db_imap_yuv2_rgb_ctrl.bits.im_v1_y2r_en = im_v1_y2r_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset), db_imap_yuv2_rgb_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v0_y2r_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_v0_y2r_en)
{
    u_vpss_db_imap_yuv2_rgb_ctrl db_imap_yuv2_rgb_ctrl;

    db_imap_yuv2_rgb_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset));
    db_imap_yuv2_rgb_ctrl.bits.im_v0_y2r_en = im_v0_y2r_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset), db_imap_yuv2_rgb_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_yuv2rgb_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_en)
{
    u_vpss_db_imap_yuv2_rgb_ctrl db_imap_yuv2_rgb_ctrl;

    db_imap_yuv2_rgb_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset));
    db_imap_yuv2_rgb_ctrl.bits.im_yuv2rgb_en = im_yuv2rgb_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_ctrl.u32) + addr_offset), db_imap_yuv2_rgb_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb00(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_00)
{
    u_vpss_db_imap_yuv2_rgb_coef00 db_imap_yuv2_rgb_coef00;

    db_imap_yuv2_rgb_coef00.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef00.u32) + addr_offset));
    db_imap_yuv2_rgb_coef00.bits.imap_m33yuv2rgb_00 = imap_m33yuv2rgb_00;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef00.u32) + addr_offset), db_imap_yuv2_rgb_coef00.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb01(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_01)
{
    u_vpss_db_imap_yuv2_rgb_coef01 db_imap_yuv2_rgb_coef01;

    db_imap_yuv2_rgb_coef01.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef01.u32) + addr_offset));
    db_imap_yuv2_rgb_coef01.bits.imap_m33yuv2rgb_01 = imap_m33yuv2rgb_01;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef01.u32) + addr_offset), db_imap_yuv2_rgb_coef01.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb02(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_02)
{
    u_vpss_db_imap_yuv2_rgb_coef02 db_imap_yuv2_rgb_coef02;

    db_imap_yuv2_rgb_coef02.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef02.u32) + addr_offset));
    db_imap_yuv2_rgb_coef02.bits.imap_m33yuv2rgb_02 = imap_m33yuv2rgb_02;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef02.u32) + addr_offset), db_imap_yuv2_rgb_coef02.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb10(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_10)
{
    u_vpss_db_imap_yuv2_rgb_coef03 db_imap_yuv2_rgb_coef03;

    db_imap_yuv2_rgb_coef03.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef03.u32) + addr_offset));
    db_imap_yuv2_rgb_coef03.bits.imap_m33yuv2rgb_10 = imap_m33yuv2rgb_10;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef03.u32) + addr_offset), db_imap_yuv2_rgb_coef03.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb11(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_11)
{
    u_vpss_db_imap_yuv2_rgb_coef04 db_imap_yuv2_rgb_coef04;

    db_imap_yuv2_rgb_coef04.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef04.u32) + addr_offset));
    db_imap_yuv2_rgb_coef04.bits.imap_m33yuv2rgb_11 = imap_m33yuv2rgb_11;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef04.u32) + addr_offset), db_imap_yuv2_rgb_coef04.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb12(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_12)
{
    u_vpss_db_imap_yuv2_rgb_coef05 db_imap_yuv2_rgb_coef05;

    db_imap_yuv2_rgb_coef05.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef05.u32) + addr_offset));
    db_imap_yuv2_rgb_coef05.bits.imap_m33yuv2rgb_12 = imap_m33yuv2rgb_12;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef05.u32) + addr_offset), db_imap_yuv2_rgb_coef05.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb20(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_20)
{
    u_vpss_db_imap_yuv2_rgb_coef06 db_imap_yuv2_rgb_coef06;

    db_imap_yuv2_rgb_coef06.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef06.u32) + addr_offset));
    db_imap_yuv2_rgb_coef06.bits.imap_m33yuv2rgb_20 = imap_m33yuv2rgb_20;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef06.u32) + addr_offset), db_imap_yuv2_rgb_coef06.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb21(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_21)
{
    u_vpss_db_imap_yuv2_rgb_coef07 db_imap_yuv2_rgb_coef07;

    db_imap_yuv2_rgb_coef07.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef07.u32) + addr_offset));
    db_imap_yuv2_rgb_coef07.bits.imap_m33yuv2rgb_21 = imap_m33yuv2rgb_21;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef07.u32) + addr_offset), db_imap_yuv2_rgb_coef07.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb22(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33yuv2rgb_22)
{
    u_vpss_db_imap_yuv2_rgb_coef08 db_imap_yuv2_rgb_coef08;

    db_imap_yuv2_rgb_coef08.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef08.u32) + addr_offset));
    db_imap_yuv2_rgb_coef08.bits.imap_m33yuv2rgb_22 = imap_m33yuv2rgb_22;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef08.u32) + addr_offset), db_imap_yuv2_rgb_coef08.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb200(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_00)
{
    u_vpss_db_imap_yuv2_rgb_coef10 db_imap_yuv2_rgb_coef10;

    db_imap_yuv2_rgb_coef10.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef10.u32) + addr_offset));
    db_imap_yuv2_rgb_coef10.bits.imap_m33yuv2rgb2_00 = imap_m33yuv2rgb2_00;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef10.u32) + addr_offset), db_imap_yuv2_rgb_coef10.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb201(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_01)
{
    u_vpss_db_imap_yuv2_rgb_coef11 db_imap_yuv2_rgb_coef11;

    db_imap_yuv2_rgb_coef11.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef11.u32) + addr_offset));
    db_imap_yuv2_rgb_coef11.bits.imap_m33yuv2rgb2_01 = imap_m33yuv2rgb2_01;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef11.u32) + addr_offset), db_imap_yuv2_rgb_coef11.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb202(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_02)
{
    u_vpss_db_imap_yuv2_rgb_coef12 db_imap_yuv2_rgb_coef12;

    db_imap_yuv2_rgb_coef12.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef12.u32) + addr_offset));
    db_imap_yuv2_rgb_coef12.bits.imap_m33yuv2rgb2_02 = imap_m33yuv2rgb2_02;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef12.u32) + addr_offset), db_imap_yuv2_rgb_coef12.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb210(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_10)
{
    u_vpss_db_imap_yuv2_rgb_coef13 db_imap_yuv2_rgb_coef13;

    db_imap_yuv2_rgb_coef13.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef13.u32) + addr_offset));
    db_imap_yuv2_rgb_coef13.bits.imap_m33yuv2rgb2_10 = imap_m33yuv2rgb2_10;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef13.u32) + addr_offset), db_imap_yuv2_rgb_coef13.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb211(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_11)
{
    u_vpss_db_imap_yuv2_rgb_coef14 db_imap_yuv2_rgb_coef14;

    db_imap_yuv2_rgb_coef14.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef14.u32) + addr_offset));
    db_imap_yuv2_rgb_coef14.bits.imap_m33yuv2rgb2_11 = imap_m33yuv2rgb2_11;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef14.u32) + addr_offset), db_imap_yuv2_rgb_coef14.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb212(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_12)
{
    u_vpss_db_imap_yuv2_rgb_coef15 db_imap_yuv2_rgb_coef15;

    db_imap_yuv2_rgb_coef15.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef15.u32) + addr_offset));
    db_imap_yuv2_rgb_coef15.bits.imap_m33yuv2rgb2_12 = imap_m33yuv2rgb2_12;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef15.u32) + addr_offset), db_imap_yuv2_rgb_coef15.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb220(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_20)
{
    u_vpss_db_imap_yuv2_rgb_coef16 db_imap_yuv2_rgb_coef16;

    db_imap_yuv2_rgb_coef16.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef16.u32) + addr_offset));
    db_imap_yuv2_rgb_coef16.bits.imap_m33yuv2rgb2_20 = imap_m33yuv2rgb2_20;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef16.u32) + addr_offset), db_imap_yuv2_rgb_coef16.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb221(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_21)
{
    u_vpss_db_imap_yuv2_rgb_coef17 db_imap_yuv2_rgb_coef17;

    db_imap_yuv2_rgb_coef17.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef17.u32) + addr_offset));
    db_imap_yuv2_rgb_coef17.bits.imap_m33yuv2rgb2_21 = imap_m33yuv2rgb2_21;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef17.u32) + addr_offset), db_imap_yuv2_rgb_coef17.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb222(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 imap_m33yuv2rgb2_22)
{
    u_vpss_db_imap_yuv2_rgb_coef18 db_imap_yuv2_rgb_coef18;

    db_imap_yuv2_rgb_coef18.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_coef18.u32) + addr_offset));
    db_imap_yuv2_rgb_coef18.bits.imap_m33yuv2rgb2_22 = imap_m33yuv2rgb2_22;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_coef18.u32) + addr_offset), db_imap_yuv2_rgb_coef18.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33yuv2rgb_scale2p(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_m33yuv2rgb_scale2p)
{
    u_vpss_db_imap_yuv2_rgb_scale2_p db_imap_yuv2_rgb_scale2_p;

    db_imap_yuv2_rgb_scale2_p.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_scale2_p.u32) + addr_offset));
    db_imap_yuv2_rgb_scale2_p.bits.imap_m33yuv2rgb_scale2p = imap_m33yuv2rgb_scale2p;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_scale2_p.u32) + addr_offset), db_imap_yuv2_rgb_scale2_p.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_yuv2rgb_dc_in_0)
{
    u_vpss_db_imap_yuv2_rgb_in_dc00 db_imap_yuv2_rgb_in_dc00;

    db_imap_yuv2_rgb_in_dc00.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_in_dc00.u32) + addr_offset));
    db_imap_yuv2_rgb_in_dc00.bits.im_v3_yuv2rgb_dc_in_0 = im_v3_yuv2rgb_dc_in_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_in_dc00.u32) + addr_offset), db_imap_yuv2_rgb_in_dc00.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_yuv2rgb_dc_in_1)
{
    u_vpss_db_imap_yuv2_rgb_in_dc01 db_imap_yuv2_rgb_in_dc01;

    db_imap_yuv2_rgb_in_dc01.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_in_dc01.u32) + addr_offset));
    db_imap_yuv2_rgb_in_dc01.bits.im_v3_yuv2rgb_dc_in_1 = im_v3_yuv2rgb_dc_in_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_in_dc01.u32) + addr_offset), db_imap_yuv2_rgb_in_dc01.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_yuv2rgb_dc_in_2)
{
    u_vpss_db_imap_yuv2_rgb_in_dc02 db_imap_yuv2_rgb_in_dc02;

    db_imap_yuv2_rgb_in_dc02.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_in_dc02.u32) + addr_offset));
    db_imap_yuv2_rgb_in_dc02.bits.im_v3_yuv2rgb_dc_in_2 = im_v3_yuv2rgb_dc_in_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_in_dc02.u32) + addr_offset), db_imap_yuv2_rgb_in_dc02.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_v3yuv2rgb_offinrgb_0)
{
    u_vpss_db_imap_yuv2_rgb_out_dc00 db_imap_yuv2_rgb_out_dc00;

    db_imap_yuv2_rgb_out_dc00.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_out_dc00.u32) + addr_offset));
    db_imap_yuv2_rgb_out_dc00.bits.imap_v3yuv2rgb_offinrgb_0 = imap_v3yuv2rgb_offinrgb_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_out_dc00.u32) + addr_offset), db_imap_yuv2_rgb_out_dc00.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_v3yuv2rgb_offinrgb_1)
{
    u_vpss_db_imap_yuv2_rgb_out_dc01 db_imap_yuv2_rgb_out_dc01;

    db_imap_yuv2_rgb_out_dc01.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_out_dc01.u32) + addr_offset));
    db_imap_yuv2_rgb_out_dc01.bits.imap_v3yuv2rgb_offinrgb_1 = imap_v3yuv2rgb_offinrgb_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_out_dc01.u32) + addr_offset), db_imap_yuv2_rgb_out_dc01.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb_offinrgb2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                   hi_u32 imap_v3yuv2rgb_offinrgb_2)
{
    u_vpss_db_imap_yuv2_rgb_out_dc02 db_imap_yuv2_rgb_out_dc02;

    db_imap_yuv2_rgb_out_dc02.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_out_dc02.u32) + addr_offset));
    db_imap_yuv2_rgb_out_dc02.bits.imap_v3yuv2rgb_offinrgb_2 = imap_v3yuv2rgb_offinrgb_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_out_dc02.u32) + addr_offset), db_imap_yuv2_rgb_out_dc02.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_yuv2rgb2_dc_in_0)
{
    u_vpss_db_imap_yuv2_rgb_in_dc10 db_imap_yuv2_rgb_in_dc10;

    db_imap_yuv2_rgb_in_dc10.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_in_dc10.u32) + addr_offset));
    db_imap_yuv2_rgb_in_dc10.bits.im_v3_yuv2rgb2_dc_in_0 = im_v3_yuv2rgb2_dc_in_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_in_dc10.u32) + addr_offset), db_imap_yuv2_rgb_in_dc10.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_yuv2rgb2_dc_in_1)
{
    u_vpss_db_imap_yuv2_rgb_in_dc11 db_imap_yuv2_rgb_in_dc11;

    db_imap_yuv2_rgb_in_dc11.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_in_dc11.u32) + addr_offset));
    db_imap_yuv2_rgb_in_dc11.bits.im_v3_yuv2rgb2_dc_in_1 = im_v3_yuv2rgb2_dc_in_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_in_dc11.u32) + addr_offset), db_imap_yuv2_rgb_in_dc11.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_yuv2rgb2_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_yuv2rgb2_dc_in_2)
{
    u_vpss_db_imap_yuv2_rgb_in_dc12 db_imap_yuv2_rgb_in_dc12;

    db_imap_yuv2_rgb_in_dc12.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_in_dc12.u32) + addr_offset));
    db_imap_yuv2_rgb_in_dc12.bits.im_v3_yuv2rgb2_dc_in_2 = im_v3_yuv2rgb2_dc_in_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_in_dc12.u32) + addr_offset), db_imap_yuv2_rgb_in_dc12.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                    hi_u32 imap_v3yuv2rgb2_offinrgb_0)
{
    u_vpss_db_imap_yuv2_rgb_out_dc10 db_imap_yuv2_rgb_out_dc10;

    db_imap_yuv2_rgb_out_dc10.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_out_dc10.u32) + addr_offset));
    db_imap_yuv2_rgb_out_dc10.bits.imap_v3yuv2rgb2_offinrgb_0 = imap_v3yuv2rgb2_offinrgb_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_out_dc10.u32) + addr_offset), db_imap_yuv2_rgb_out_dc10.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                    hi_u32 imap_v3yuv2rgb2_offinrgb_1)
{
    u_vpss_db_imap_yuv2_rgb_out_dc11 db_imap_yuv2_rgb_out_dc11;

    db_imap_yuv2_rgb_out_dc11.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_out_dc11.u32) + addr_offset));
    db_imap_yuv2_rgb_out_dc11.bits.imap_v3yuv2rgb2_offinrgb_1 = imap_v3yuv2rgb2_offinrgb_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_out_dc11.u32) + addr_offset), db_imap_yuv2_rgb_out_dc11.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_v3yuv2rgb2_offinrgb2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                    hi_u32 imap_v3yuv2rgb2_offinrgb_2)
{
    u_vpss_db_imap_yuv2_rgb_out_dc12 db_imap_yuv2_rgb_out_dc12;

    db_imap_yuv2_rgb_out_dc12.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_out_dc12.u32) + addr_offset));
    db_imap_yuv2_rgb_out_dc12.bits.imap_v3yuv2rgb2_offinrgb_2 = imap_v3yuv2rgb2_offinrgb_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_out_dc12.u32) + addr_offset), db_imap_yuv2_rgb_out_dc12.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_yuv2rgb_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_yuv2rgb_clip_min)
{
    u_vpss_db_imap_yuv2_rgb_min db_imap_yuv2_rgb_min;

    db_imap_yuv2_rgb_min.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_min.u32) + addr_offset));
    db_imap_yuv2_rgb_min.bits.im_yuv2rgb_clip_min = im_yuv2rgb_clip_min;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_min.u32) + addr_offset), db_imap_yuv2_rgb_min.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_yuv2rgb_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_yuv2rgb_clip_max)
{
    u_vpss_db_imap_yuv2_rgb_max db_imap_yuv2_rgb_max;

    db_imap_yuv2_rgb_max.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_max.u32) + addr_offset));
    db_imap_yuv2_rgb_max.bits.im_yuv2rgb_clip_max = im_yuv2rgb_clip_max;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_max.u32) + addr_offset), db_imap_yuv2_rgb_max.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_yuv2rgb_thr_r(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_thr_r)
{
    u_vpss_db_imap_yuv2_rgb_thr db_imap_yuv2_rgb_thr;

    db_imap_yuv2_rgb_thr.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_thr.u32) + addr_offset));
    db_imap_yuv2_rgb_thr.bits.im_yuv2rgb_thr_r = im_yuv2rgb_thr_r;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_thr.u32) + addr_offset), db_imap_yuv2_rgb_thr.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_yuv2rgb_thr_b(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_yuv2rgb_thr_b)
{
    u_vpss_db_imap_yuv2_rgb_thr db_imap_yuv2_rgb_thr;

    db_imap_yuv2_rgb_thr.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_yuv2_rgb_thr.u32) + addr_offset));
    db_imap_yuv2_rgb_thr.bits.im_yuv2rgb_thr_b = im_yuv2rgb_thr_b;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_yuv2_rgb_thr.u32) + addr_offset), db_imap_yuv2_rgb_thr.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_rgb2lms_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_rgb2lms_en)
{
    u_vpss_db_imap_rgb2_lms_ctrl db_imap_rgb2_lms_ctrl;

    db_imap_rgb2_lms_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_ctrl.u32) + addr_offset));
    db_imap_rgb2_lms_ctrl.bits.im_rgb2lms_en = im_rgb2lms_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_ctrl.u32) + addr_offset), db_imap_rgb2_lms_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms00(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_00)
{
    u_vpss_db_imap_rgb2_lms_coef0 db_imap_rgb2_lms_coef0;

    db_imap_rgb2_lms_coef0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef0.u32) + addr_offset));
    db_imap_rgb2_lms_coef0.bits.imap_m33rgb2lms_00 = imap_m33rgb2lms_00;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef0.u32) + addr_offset), db_imap_rgb2_lms_coef0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms01(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_01)
{
    u_vpss_db_imap_rgb2_lms_coef1 db_imap_rgb2_lms_coef1;

    db_imap_rgb2_lms_coef1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef1.u32) + addr_offset));
    db_imap_rgb2_lms_coef1.bits.imap_m33rgb2lms_01 = imap_m33rgb2lms_01;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef1.u32) + addr_offset), db_imap_rgb2_lms_coef1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms02(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_02)
{
    u_vpss_db_imap_rgb2_lms_coef2 db_imap_rgb2_lms_coef2;

    db_imap_rgb2_lms_coef2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef2.u32) + addr_offset));
    db_imap_rgb2_lms_coef2.bits.imap_m33rgb2lms_02 = imap_m33rgb2lms_02;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef2.u32) + addr_offset), db_imap_rgb2_lms_coef2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms10(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_10)
{
    u_vpss_db_imap_rgb2_lms_coef3 db_imap_rgb2_lms_coef3;

    db_imap_rgb2_lms_coef3.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef3.u32) + addr_offset));
    db_imap_rgb2_lms_coef3.bits.imap_m33rgb2lms_10 = imap_m33rgb2lms_10;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef3.u32) + addr_offset), db_imap_rgb2_lms_coef3.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms11(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_11)
{
    u_vpss_db_imap_rgb2_lms_coef4 db_imap_rgb2_lms_coef4;

    db_imap_rgb2_lms_coef4.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef4.u32) + addr_offset));
    db_imap_rgb2_lms_coef4.bits.imap_m33rgb2lms_11 = imap_m33rgb2lms_11;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef4.u32) + addr_offset), db_imap_rgb2_lms_coef4.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms12(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_12)
{
    u_vpss_db_imap_rgb2_lms_coef5 db_imap_rgb2_lms_coef5;

    db_imap_rgb2_lms_coef5.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef5.u32) + addr_offset));
    db_imap_rgb2_lms_coef5.bits.imap_m33rgb2lms_12 = imap_m33rgb2lms_12;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef5.u32) + addr_offset), db_imap_rgb2_lms_coef5.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms20(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_20)
{
    u_vpss_db_imap_rgb2_lms_coef6 db_imap_rgb2_lms_coef6;

    db_imap_rgb2_lms_coef6.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef6.u32) + addr_offset));
    db_imap_rgb2_lms_coef6.bits.imap_m33rgb2lms_20 = imap_m33rgb2lms_20;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef6.u32) + addr_offset), db_imap_rgb2_lms_coef6.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms21(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_21)
{
    u_vpss_db_imap_rgb2_lms_coef7 db_imap_rgb2_lms_coef7;

    db_imap_rgb2_lms_coef7.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef7.u32) + addr_offset));
    db_imap_rgb2_lms_coef7.bits.imap_m33rgb2lms_21 = imap_m33rgb2lms_21;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef7.u32) + addr_offset), db_imap_rgb2_lms_coef7.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms22(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33rgb2lms_22)
{
    u_vpss_db_imap_rgb2_lms_coef8 db_imap_rgb2_lms_coef8;

    db_imap_rgb2_lms_coef8.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_coef8.u32) + addr_offset));
    db_imap_rgb2_lms_coef8.bits.imap_m33rgb2lms_22 = imap_m33rgb2lms_22;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_coef8.u32) + addr_offset), db_imap_rgb2_lms_coef8.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33rgb2lms_scale2p(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_m33rgb2lms_scale2p)
{
    u_vpss_db_imap_rgb2_lms_scale2_p db_imap_rgb2_lms_scale2_p;

    db_imap_rgb2_lms_scale2_p.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_scale2_p.u32) + addr_offset));
    db_imap_rgb2_lms_scale2_p.bits.imap_m33rgb2lms_scale2p = imap_m33rgb2lms_scale2p;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_scale2_p.u32) + addr_offset), db_imap_rgb2_lms_scale2_p.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_rgb2lms_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_rgb2lms_clip_min)
{
    u_vpss_db_imap_rgb2_lms_min db_imap_rgb2_lms_min;

    db_imap_rgb2_lms_min.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_min.u32) + addr_offset));
    db_imap_rgb2_lms_min.bits.im_rgb2lms_clip_min = im_rgb2lms_clip_min;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_min.u32) + addr_offset), db_imap_rgb2_lms_min.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_rgb2lms_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_rgb2lms_clip_max)
{
    u_vpss_db_imap_rgb2_lms_max db_imap_rgb2_lms_max;

    db_imap_rgb2_lms_max.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_max.u32) + addr_offset));
    db_imap_rgb2_lms_max.bits.im_rgb2lms_clip_max = im_rgb2lms_clip_max;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_max.u32) + addr_offset), db_imap_rgb2_lms_max.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_rgb2lms_dc_in_0)
{
    u_vpss_db_imap_rgb2_lms_in_dc0 db_imap_rgb2_lms_in_dc0;

    db_imap_rgb2_lms_in_dc0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_in_dc0.u32) + addr_offset));
    db_imap_rgb2_lms_in_dc0.bits.im_v3_rgb2lms_dc_in_0 = im_v3_rgb2lms_dc_in_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_in_dc0.u32) + addr_offset), db_imap_rgb2_lms_in_dc0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_rgb2lms_dc_in_1)
{
    u_vpss_db_imap_rgb2_lms_in_dc1 db_imap_rgb2_lms_in_dc1;

    db_imap_rgb2_lms_in_dc1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_in_dc1.u32) + addr_offset));
    db_imap_rgb2_lms_in_dc1.bits.im_v3_rgb2lms_dc_in_1 = im_v3_rgb2lms_dc_in_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_in_dc1.u32) + addr_offset), db_imap_rgb2_lms_in_dc1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_rgb2lms_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_rgb2lms_dc_in_2)
{
    u_vpss_db_imap_rgb2_lms_in_dc2 db_imap_rgb2_lms_in_dc2;

    db_imap_rgb2_lms_in_dc2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_rgb2_lms_in_dc2.u32) + addr_offset));
    db_imap_rgb2_lms_in_dc2.bits.im_v3_rgb2lms_dc_in_2 = im_v3_rgb2lms_dc_in_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_rgb2_lms_in_dc2.u32) + addr_offset), db_imap_rgb2_lms_in_dc2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_lms2ipt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_lms2ipt_en)
{
    u_vpss_db_imap_lms2_ipt_ctrl db_imap_lms2_ipt_ctrl;

    db_imap_lms2_ipt_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_ctrl.u32) + addr_offset));
    db_imap_lms2_ipt_ctrl.bits.im_lms2ipt_en = im_lms2ipt_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_ctrl.u32) + addr_offset), db_imap_lms2_ipt_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt00(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_00)
{
    u_vpss_db_imap_lms2_ipt_coef0 db_imap_lms2_ipt_coef0;

    db_imap_lms2_ipt_coef0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef0.u32) + addr_offset));
    db_imap_lms2_ipt_coef0.bits.imap_m33lms2ipt_00 = imap_m33lms2ipt_00;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef0.u32) + addr_offset), db_imap_lms2_ipt_coef0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt01(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_01)
{
    u_vpss_db_imap_lms2_ipt_coef1 db_imap_lms2_ipt_coef1;

    db_imap_lms2_ipt_coef1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef1.u32) + addr_offset));
    db_imap_lms2_ipt_coef1.bits.imap_m33lms2ipt_01 = imap_m33lms2ipt_01;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef1.u32) + addr_offset), db_imap_lms2_ipt_coef1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt02(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_02)
{
    u_vpss_db_imap_lms2_ipt_coef2 db_imap_lms2_ipt_coef2;

    db_imap_lms2_ipt_coef2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef2.u32) + addr_offset));
    db_imap_lms2_ipt_coef2.bits.imap_m33lms2ipt_02 = imap_m33lms2ipt_02;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef2.u32) + addr_offset), db_imap_lms2_ipt_coef2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt10(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_10)
{
    u_vpss_db_imap_lms2_ipt_coef3 db_imap_lms2_ipt_coef3;

    db_imap_lms2_ipt_coef3.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef3.u32) + addr_offset));
    db_imap_lms2_ipt_coef3.bits.imap_m33lms2ipt_10 = imap_m33lms2ipt_10;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef3.u32) + addr_offset), db_imap_lms2_ipt_coef3.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt11(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_11)
{
    u_vpss_db_imap_lms2_ipt_coef4 db_imap_lms2_ipt_coef4;

    db_imap_lms2_ipt_coef4.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef4.u32) + addr_offset));
    db_imap_lms2_ipt_coef4.bits.imap_m33lms2ipt_11 = imap_m33lms2ipt_11;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef4.u32) + addr_offset), db_imap_lms2_ipt_coef4.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt12(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_12)
{
    u_vpss_db_imap_lms2_ipt_coef5 db_imap_lms2_ipt_coef5;

    db_imap_lms2_ipt_coef5.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef5.u32) + addr_offset));
    db_imap_lms2_ipt_coef5.bits.imap_m33lms2ipt_12 = imap_m33lms2ipt_12;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef5.u32) + addr_offset), db_imap_lms2_ipt_coef5.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt20(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_20)
{
    u_vpss_db_imap_lms2_ipt_coef6 db_imap_lms2_ipt_coef6;

    db_imap_lms2_ipt_coef6.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef6.u32) + addr_offset));
    db_imap_lms2_ipt_coef6.bits.imap_m33lms2ipt_20 = imap_m33lms2ipt_20;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef6.u32) + addr_offset), db_imap_lms2_ipt_coef6.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt21(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_21)
{
    u_vpss_db_imap_lms2_ipt_coef7 db_imap_lms2_ipt_coef7;

    db_imap_lms2_ipt_coef7.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef7.u32) + addr_offset));
    db_imap_lms2_ipt_coef7.bits.imap_m33lms2ipt_21 = imap_m33lms2ipt_21;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef7.u32) + addr_offset), db_imap_lms2_ipt_coef7.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt22(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 imap_m33lms2ipt_22)
{
    u_vpss_db_imap_lms2_ipt_coef8 db_imap_lms2_ipt_coef8;

    db_imap_lms2_ipt_coef8.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_coef8.u32) + addr_offset));
    db_imap_lms2_ipt_coef8.bits.imap_m33lms2ipt_22 = imap_m33lms2ipt_22;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_coef8.u32) + addr_offset), db_imap_lms2_ipt_coef8.u32);

    return;
}

hi_void vpss_hihdr_v2_set_imap_m33lms2ipt_scale2p(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 imap_m33lms2ipt_scale2p)
{
    u_vpss_db_imap_lms2_ipt_scale2_p db_imap_lms2_ipt_scale2_p;

    db_imap_lms2_ipt_scale2_p.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_scale2_p.u32) + addr_offset));
    db_imap_lms2_ipt_scale2_p.bits.imap_m33lms2ipt_scale2p = imap_m33lms2ipt_scale2p;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_scale2_p.u32) + addr_offset), db_imap_lms2_ipt_scale2_p.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_min_y(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_min_y)
{
    u_vpss_db_imap_lms2_ipt_min db_imap_lms2_ipt_min;

    db_imap_lms2_ipt_min.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_min.u32) + addr_offset));
    db_imap_lms2_ipt_min.bits.im_lms2ipt_clip_min_y = im_lms2ipt_clip_min_y;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_min.u32) + addr_offset), db_imap_lms2_ipt_min.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_max_y(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_max_y)
{
    u_vpss_db_imap_lms2_ipt_max db_imap_lms2_ipt_max;

    db_imap_lms2_ipt_max.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_max.u32) + addr_offset));
    db_imap_lms2_ipt_max.bits.im_lms2ipt_clip_max_y = im_lms2ipt_clip_max_y;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_max.u32) + addr_offset), db_imap_lms2_ipt_max.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_min_c(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_min_c)
{
    u_vpss_db_imap_lms2_ipt_min db_imap_lms2_ipt_min;

    db_imap_lms2_ipt_min.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_min.u32) + addr_offset));
    db_imap_lms2_ipt_min.bits.im_lms2ipt_clip_min_c = im_lms2ipt_clip_min_c;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_min.u32) + addr_offset), db_imap_lms2_ipt_min.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_lms2ipt_clip_max_c(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_lms2ipt_clip_max_c)
{
    u_vpss_db_imap_lms2_ipt_max db_imap_lms2_ipt_max;

    db_imap_lms2_ipt_max.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_max.u32) + addr_offset));
    db_imap_lms2_ipt_max.bits.im_lms2ipt_clip_max_c = im_lms2ipt_clip_max_c;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_max.u32) + addr_offset), db_imap_lms2_ipt_max.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_lms2ipt_dc_out_0)
{
    u_vpss_db_imap_lms2_ipt_out_dc0 db_imap_lms2_ipt_out_dc0;

    db_imap_lms2_ipt_out_dc0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_out_dc0.u32) + addr_offset));
    db_imap_lms2_ipt_out_dc0.bits.im_v3_lms2ipt_dc_out_0 = im_v3_lms2ipt_dc_out_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_out_dc0.u32) + addr_offset), db_imap_lms2_ipt_out_dc0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_lms2ipt_dc_out_1)
{
    u_vpss_db_imap_lms2_ipt_out_dc1 db_imap_lms2_ipt_out_dc1;

    db_imap_lms2_ipt_out_dc1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_out_dc1.u32) + addr_offset));
    db_imap_lms2_ipt_out_dc1.bits.im_v3_lms2ipt_dc_out_1 = im_v3_lms2ipt_dc_out_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_out_dc1.u32) + addr_offset), db_imap_lms2_ipt_out_dc1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_out2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_v3_lms2ipt_dc_out_2)
{
    u_vpss_db_imap_lms2_ipt_out_dc2 db_imap_lms2_ipt_out_dc2;
    db_imap_lms2_ipt_out_dc2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_out_dc2.u32) + addr_offset));
    db_imap_lms2_ipt_out_dc2.bits.im_v3_lms2ipt_dc_out_2 = im_v3_lms2ipt_dc_out_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_out_dc2.u32) + addr_offset), db_imap_lms2_ipt_out_dc2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in0(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_lms2ipt_dc_in_0)
{
    u_vpss_db_imap_lms2_ipt_in_dc0 db_imap_lms2_ipt_in_dc0;

    db_imap_lms2_ipt_in_dc0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_in_dc0.u32) + addr_offset));
    db_imap_lms2_ipt_in_dc0.bits.im_v3_lms2ipt_dc_in_0 = im_v3_lms2ipt_dc_in_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_in_dc0.u32) + addr_offset), db_imap_lms2_ipt_in_dc0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in1(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_lms2ipt_dc_in_1)
{
    u_vpss_db_imap_lms2_ipt_in_dc1 db_imap_lms2_ipt_in_dc1;

    db_imap_lms2_ipt_in_dc1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_in_dc1.u32) + addr_offset));
    db_imap_lms2_ipt_in_dc1.bits.im_v3_lms2ipt_dc_in_1 = im_v3_lms2ipt_dc_in_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_in_dc1.u32) + addr_offset), db_imap_lms2_ipt_in_dc1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_v3_lms2ipt_dc_in2(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                               hi_u32 im_v3_lms2ipt_dc_in_2)
{
    u_vpss_db_imap_lms2_ipt_in_dc2 db_imap_lms2_ipt_in_dc2;

    db_imap_lms2_ipt_in_dc2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_lms2_ipt_in_dc2.u32) + addr_offset));
    db_imap_lms2_ipt_in_dc2.bits.im_v3_lms2ipt_dc_in_2 = im_v3_lms2ipt_dc_in_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_lms2_ipt_in_dc2.u32) + addr_offset), db_imap_lms2_ipt_in_dc2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_v1_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_v1_en)
{
    u_vpss_db_imap_tmap_v1_ctrl db_imap_tmap_v1_ctrl;

    db_imap_tmap_v1_ctrl.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_ctrl.u32) + addr_offset));
    db_imap_tmap_v1_ctrl.bits.im_tm_v1_en = im_tm_v1_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_ctrl.u32) + addr_offset), db_imap_tmap_v1_ctrl.u32);

    return;
}

hi_void vpss_hihdr_v2_set_vhdr_tmap_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 vhdr_tmap_rd_en)
{
    u_vpss_db_imap_tmap_v1_ren db_imap_tmap_v1_ren;

    db_imap_tmap_v1_ren.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_ren.u32) + addr_offset));
    db_imap_tmap_v1_ren.bits.vhdr_tmap_rd_en = vhdr_tmap_rd_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_ren.u32) + addr_offset), db_imap_tmap_v1_ren.u32);

    return;
}

hi_void vpss_hihdr_v2_set_vhdr_tmap_para_data(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 vhdr_tmap_para_data)
{
    u_vpss_db_imap_tmap_v1_data db_imap_tmap_v1_data;

    db_imap_tmap_v1_data.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_data.u32) + addr_offset));
    db_imap_tmap_v1_data.bits.vhdr_tmap_para_data = vhdr_tmap_para_data;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_data.u32) + addr_offset), db_imap_tmap_v1_data.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_scale_mix_alpha(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                hi_u32 im_tm_scale_mix_alpha)
{
    u_vpss_db_imap_tmap_v1_mix_alpha db_imap_tmap_v1_mix_alpha;

    db_imap_tmap_v1_mix_alpha.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_mix_alpha.u32) + addr_offset));
    db_imap_tmap_v1_mix_alpha.bits.im_tm_scale_mix_alpha = im_tm_scale_mix_alpha;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_mix_alpha.u32) + addr_offset), db_imap_tmap_v1_mix_alpha.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_mix_alpha(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_mix_alpha)
{
    u_vpss_db_imap_tmap_v1_mix_alpha db_imap_tmap_v1_mix_alpha;

    db_imap_tmap_v1_mix_alpha.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_mix_alpha.u32) + addr_offset));
    db_imap_tmap_v1_mix_alpha.bits.im_tm_mix_alpha = im_tm_mix_alpha;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_mix_alpha.u32) + addr_offset), db_imap_tmap_v1_mix_alpha.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_3)
{
    u_vpss_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_3 = im_tm_x_step_3;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset), db_imap_tmap_v1_step1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_2)
{
    u_vpss_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_2 = im_tm_x_step_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset), db_imap_tmap_v1_step1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_1)
{
    u_vpss_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_1 = im_tm_x_step_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset), db_imap_tmap_v1_step1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_0)
{
    u_vpss_db_imap_tmap_v1_step1 db_imap_tmap_v1_step1;

    db_imap_tmap_v1_step1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset));
    db_imap_tmap_v1_step1.bits.im_tm_x_step_0 = im_tm_x_step_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step1.u32) + addr_offset), db_imap_tmap_v1_step1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep7(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_7)
{
    u_vpss_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_7 = im_tm_x_step_7;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset), db_imap_tmap_v1_step2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_6)
{
    u_vpss_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_6 = im_tm_x_step_6;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset), db_imap_tmap_v1_step2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_5)
{
    u_vpss_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_5 = im_tm_x_step_5;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset), db_imap_tmap_v1_step2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xstep4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_step_4)
{
    u_vpss_db_imap_tmap_v1_step2 db_imap_tmap_v1_step2;

    db_imap_tmap_v1_step2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset));
    db_imap_tmap_v1_step2.bits.im_tm_x_step_4 = im_tm_x_step_4;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_step2.u32) + addr_offset), db_imap_tmap_v1_step2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_0)
{
    u_vpss_db_imap_tmap_v1_pos1 db_imap_tmap_v1_pos1;

    db_imap_tmap_v1_pos1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos1.u32) + addr_offset));
    db_imap_tmap_v1_pos1.bits.im_tm_x_pos_0 = im_tm_x_pos_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos1.u32) + addr_offset), db_imap_tmap_v1_pos1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_1)
{
    u_vpss_db_imap_tmap_v1_pos2 db_imap_tmap_v1_pos2;

    db_imap_tmap_v1_pos2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos2.u32) + addr_offset));
    db_imap_tmap_v1_pos2.bits.im_tm_x_pos_1 = im_tm_x_pos_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos2.u32) + addr_offset), db_imap_tmap_v1_pos2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_2)
{
    u_vpss_db_imap_tmap_v1_pos3 db_imap_tmap_v1_pos3;

    db_imap_tmap_v1_pos3.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos3.u32) + addr_offset));
    db_imap_tmap_v1_pos3.bits.im_tm_x_pos_2 = im_tm_x_pos_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos3.u32) + addr_offset), db_imap_tmap_v1_pos3.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_3)
{
    u_vpss_db_imap_tmap_v1_pos4 db_imap_tmap_v1_pos4;

    db_imap_tmap_v1_pos4.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos4.u32) + addr_offset));
    db_imap_tmap_v1_pos4.bits.im_tm_x_pos_3 = im_tm_x_pos_3;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos4.u32) + addr_offset), db_imap_tmap_v1_pos4.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_4)
{
    u_vpss_db_imap_tmap_v1_pos5 db_imap_tmap_v1_pos5;

    db_imap_tmap_v1_pos5.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos5.u32) + addr_offset));
    db_imap_tmap_v1_pos5.bits.im_tm_x_pos_4 = im_tm_x_pos_4;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos5.u32) + addr_offset), db_imap_tmap_v1_pos5.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_5)
{
    u_vpss_db_imap_tmap_v1_pos6 db_imap_tmap_v1_pos6;

    db_imap_tmap_v1_pos6.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos6.u32) + addr_offset));
    db_imap_tmap_v1_pos6.bits.im_tm_x_pos_5 = im_tm_x_pos_5;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos6.u32) + addr_offset), db_imap_tmap_v1_pos6.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_6)
{
    u_vpss_db_imap_tmap_v1_pos7 db_imap_tmap_v1_pos7;

    db_imap_tmap_v1_pos7.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos7.u32) + addr_offset));
    db_imap_tmap_v1_pos7.bits.im_tm_x_pos_6 = im_tm_x_pos_6;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos7.u32) + addr_offset), db_imap_tmap_v1_pos7.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xpos7(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_pos_7)
{
    u_vpss_db_imap_tmap_v1_pos8 db_imap_tmap_v1_pos8;

    db_imap_tmap_v1_pos8.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_pos8.u32) + addr_offset));
    db_imap_tmap_v1_pos8.bits.im_tm_x_pos_7 = im_tm_x_pos_7;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_pos8.u32) + addr_offset), db_imap_tmap_v1_pos8.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_3)
{
    u_vpss_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_3 = im_tm_x_num_3;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset), db_imap_tmap_v1_num1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_2)
{
    u_vpss_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_2 = im_tm_x_num_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset), db_imap_tmap_v1_num1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_1)
{
    u_vpss_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_1 = im_tm_x_num_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset), db_imap_tmap_v1_num1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_0)
{
    u_vpss_db_imap_tmap_v1_num1 db_imap_tmap_v1_num1;

    db_imap_tmap_v1_num1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset));
    db_imap_tmap_v1_num1.bits.im_tm_x_num_0 = im_tm_x_num_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num1.u32) + addr_offset), db_imap_tmap_v1_num1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum7(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_7)
{
    u_vpss_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_7 = im_tm_x_num_7;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset), db_imap_tmap_v1_num2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_6)
{
    u_vpss_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_6 = im_tm_x_num_6;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset), db_imap_tmap_v1_num2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_5)
{
    u_vpss_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_5 = im_tm_x_num_5;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset), db_imap_tmap_v1_num2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_xnum4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_x_num_4)
{
    u_vpss_db_imap_tmap_v1_num2 db_imap_tmap_v1_num2;

    db_imap_tmap_v1_num2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset));
    db_imap_tmap_v1_num2.bits.im_tm_x_num_4 = im_tm_x_num_4;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_num2.u32) + addr_offset), db_imap_tmap_v1_num2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_m3_lum_cal0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_m3_lum_cal_0)
{
    u_vpss_db_imap_tmap_v1_luma_coef0 db_imap_tmap_v1_luma_coef0;

    db_imap_tmap_v1_luma_coef0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_luma_coef0.u32) + addr_offset));
    db_imap_tmap_v1_luma_coef0.bits.im_tm_m3_lum_cal_0 = im_tm_m3_lum_cal_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_luma_coef0.u32) + addr_offset), db_imap_tmap_v1_luma_coef0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_m3_lum_cal1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_m3_lum_cal_1)
{
    u_vpss_db_imap_tmap_v1_luma_coef1 db_imap_tmap_v1_luma_coef1;

    db_imap_tmap_v1_luma_coef1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_luma_coef1.u32) + addr_offset));
    db_imap_tmap_v1_luma_coef1.bits.im_tm_m3_lum_cal_1 = im_tm_m3_lum_cal_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_luma_coef1.u32) + addr_offset), db_imap_tmap_v1_luma_coef1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_m3_lum_cal2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_m3_lum_cal_2)
{
    u_vpss_db_imap_tmap_v1_luma_coef2 db_imap_tmap_v1_luma_coef2;

    db_imap_tmap_v1_luma_coef2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_luma_coef2.u32) + addr_offset));
    db_imap_tmap_v1_luma_coef2.bits.im_tm_m3_lum_cal_2 = im_tm_m3_lum_cal_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_luma_coef2.u32) + addr_offset), db_imap_tmap_v1_luma_coef2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_scale_lum_cal(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_tm_scale_lum_cal)
{
    u_vpss_db_imap_tmap_v1_luma_scale db_imap_tmap_v1_luma_scale;

    db_imap_tmap_v1_luma_scale.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_luma_scale.u32) + addr_offset));
    db_imap_tmap_v1_luma_scale.bits.im_tm_scale_lum_cal = im_tm_scale_lum_cal;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_luma_scale.u32) + addr_offset), db_imap_tmap_v1_luma_scale.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_scale_coef(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_scale_coef)
{
    u_vpss_db_imap_tmap_v1_coef_scale db_imap_tmap_v1_coef_scale;

    db_imap_tmap_v1_coef_scale.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_coef_scale.u32) + addr_offset));
    db_imap_tmap_v1_coef_scale.bits.im_tm_scale_coef = im_tm_scale_coef;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_coef_scale.u32) + addr_offset), db_imap_tmap_v1_coef_scale.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_clip_min(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_clip_min)
{
    u_vpss_db_imap_tmap_v1_out_clip_min db_imap_tmap_v1_out_clip_min;

    db_imap_tmap_v1_out_clip_min.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_out_clip_min.u32) +
                                                             addr_offset));
    db_imap_tmap_v1_out_clip_min.bits.im_tm_clip_min = im_tm_clip_min;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_out_clip_min.u32) + addr_offset),
                          db_imap_tmap_v1_out_clip_min.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_clip_max(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_clip_max)
{
    u_vpss_db_imap_tmap_v1_out_clip_max db_imap_tmap_v1_out_clip_max;

    db_imap_tmap_v1_out_clip_max.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_out_clip_max.u32) +
                                                             addr_offset));
    db_imap_tmap_v1_out_clip_max.bits.im_tm_clip_max = im_tm_clip_max;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_out_clip_max.u32) + addr_offset),
                          db_imap_tmap_v1_out_clip_max.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_dc_out0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_dc_out_0)
{
    u_vpss_db_imap_tmap_v1_out_dc0 db_imap_tmap_v1_out_dc0;

    db_imap_tmap_v1_out_dc0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_out_dc0.u32) + addr_offset));
    db_imap_tmap_v1_out_dc0.bits.im_tm_dc_out_0 = im_tm_dc_out_0;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_out_dc0.u32) + addr_offset), db_imap_tmap_v1_out_dc0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_dc_out1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_dc_out_1)
{
    u_vpss_db_imap_tmap_v1_out_dc1 db_imap_tmap_v1_out_dc1;

    db_imap_tmap_v1_out_dc1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_out_dc1.u32) + addr_offset));
    db_imap_tmap_v1_out_dc1.bits.im_tm_dc_out_1 = im_tm_dc_out_1;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_out_dc1.u32) + addr_offset), db_imap_tmap_v1_out_dc1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_tm_dc_out2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_tm_dc_out_2)
{
    u_vpss_db_imap_tmap_v1_out_dc2 db_imap_tmap_v1_out_dc2;

    db_imap_tmap_v1_out_dc2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_tmap_v1_out_dc2.u32) + addr_offset));
    db_imap_tmap_v1_out_dc2.bits.im_tm_dc_out_2 = im_tm_dc_out_2;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_tmap_v1_out_dc2.u32) + addr_offset), db_imap_tmap_v1_out_dc2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_demo_pos(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_demo_pos)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_demo_pos = im_cm_demo_pos;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_demo_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_demo_mode)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_demo_mode = im_cm_demo_mode;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_demo_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_demo_en)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_demo_en = im_cm_demo_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_bitdepth_out_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                  hi_u32 im_cm_bitdepth_out_mode)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_bitdepth_out_mode = im_cm_bitdepth_out_mode;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_bitdepth_in_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                                 hi_u32 im_cm_bitdepth_in_mode)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_bitdepth_in_mode = im_cm_bitdepth_in_mode;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_ck_gt_en)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_ck_gt_en = im_cm_ck_gt_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_en)
{
    u_vpss_db_imap_cacm_reg0 db_imap_cacm_reg0;

    db_imap_cacm_reg0.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset));
    db_imap_cacm_reg0.bits.im_cm_en = im_cm_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg0.u32) + addr_offset), db_imap_cacm_reg0.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_clut_rd_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 im_cm_clut_rd_en)
{
    u_vpss_db_imap_cacm_reg1 db_imap_cacm_reg1;

    db_imap_cacm_reg1.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg1.u32) + addr_offset));
    db_imap_cacm_reg1.bits.im_cm_clut_rd_en = im_cm_clut_rd_en;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg1.u32) + addr_offset), db_imap_cacm_reg1.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_clut_rdata_rg(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                              hi_u32 im_cm_clut_rdata_rg)
{
    u_vpss_db_imap_cacm_reg2 db_imap_cacm_reg2;

    db_imap_cacm_reg2.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg2.u32) + addr_offset));
    db_imap_cacm_reg2.bits.im_cm_clut_rdata_rg = im_cm_clut_rdata_rg;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg2.u32) + addr_offset), db_imap_cacm_reg2.u32);

    return;
}

hi_void vpss_hihdr_v2_set_im_cm_clut_rdata_b(vpss_reg_type *vpss_reg, hi_u32 addr_offset,
                                             hi_u32 im_cm_clut_rdata_b)
{
    u_vpss_db_imap_cacm_reg3 db_imap_cacm_reg3;

    db_imap_cacm_reg3.u32 = pq_hal_vpss_reg_read((&(vpss_reg->db_imap_cacm_reg3.u32) + addr_offset));
    db_imap_cacm_reg3.bits.im_cm_clut_rdata_b = im_cm_clut_rdata_b;
    pq_hal_vpss_reg_write((&(vpss_reg->db_imap_cacm_reg3.u32) + addr_offset), db_imap_cacm_reg3.u32);

    return;
}


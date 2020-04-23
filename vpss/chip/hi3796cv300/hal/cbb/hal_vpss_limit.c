/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_limit.h hander file vpss limit
 * Author: zhangjunyu
 * Create: 2016/07/03
 */

#include "hal_vpss_limit.h"
#include "hal_vpss_zme_inst_para.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_MAX_REGS_NUM_IN_ONE_SCAN ((0x2000 + 15) / 16 * 4)

typedef struct {
    hi_u32 offset;
} xdp_vc1_inst_para;

typedef enum {
    XDP_VC1_ID_CF = 0,
    XDP_VC1_ID_ME_CF = 1,

    XDP_VC1_ID_MAX
} xdp_vc1_id;

hi_u32 vpss_vcti_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /* please add your VPSS_ASSERT_RETion here. */
    if (reg->vpss_hipp_vcti_ctrl.bits.vcti_vcti_en == 1) {
        vpss_assert_ret(cfg_info->wth <= 960); /* 960:support width */
    }

    return HI_SUCCESS;
}
hi_u32 vpss_vc1_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    return HI_SUCCESS;
}

hi_void vpss_hal_crop_config_check(vpss_reg_type *vpss_regs, hi_u32 addr_offset, vpss_cfg_info *cfg_info)
{
    /******************* declairation  *********************/
    u_vpss_vhd0_crop_pos vpss_vhd0_crop_pos;
    u_vpss_ctrl1 vpss_ctrl1;
    u_vpss_vhd0_crop_size vpss_vhd0_crop_size;
    /******************* get reg value *********************/
    vpss_ctrl1.u32 = vpss_reg_read((&(vpss_regs->vpss_ctrl1.u32) + addr_offset));
    vpss_vhd0_crop_pos.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_crop_pos.u32) + addr_offset));
    vpss_vhd0_crop_size.u32 = vpss_reg_read((&(vpss_regs->vpss_vhd0_crop_size.u32) + addr_offset));

    /* please add your VPSS_ASSERT_RETion here. */
    if (vpss_ctrl1.bits.crop_en == 1) {
        vpss_assert_not_ret(vpss_vhd0_crop_pos.bits.vhd0_crop_x % 2 == 0); /* 2:align */
        vpss_assert_not_ret(vpss_vhd0_crop_pos.bits.vhd0_crop_y % 2 == 0); /* 2:align */
        vpss_assert_not_ret((vpss_vhd0_crop_size.bits.vhd0_crop_width + 1) % 2 == 0); /* 2:align */
        vpss_assert_not_ret((vpss_vhd0_crop_size.bits.vhd0_crop_height + 1) % 2 == 0); /* 2:align */
    }

    return;
}

hi_u32 vpss_vzme_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 max_in_width;
    hi_u32 max_in_height;
    hi_u32 lvfir_en;
    hi_u32 out_height;
    hi_u32 lvfir_offset;
    hi_u32 vratio;
    /******************* get reg value *********************/
    max_in_width = 480; /* 480:para */
    max_in_height = 1088; /* 1088:para */
    lvfir_en = reg->vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_lvfir_en;
    out_height = reg->vpss_wr1_hvds_zme_vinfo.bits.vpss_wr1_hvds_out_height + 1;
    lvfir_offset = reg->vpss_wr1_hvds_zme_voffset.bits.vpss_wr1_hvds_vluma_offset;
    vratio = reg->vpss_wr1_hvds_zme_vsp.bits.vpss_wr1_hvds_vratio;

    if (lvfir_en == 1) {
        if (cfg_info->pro == 0) {
            out_height /= 2; /* 2:para */
        }

        vpss_assert_ret(cfg_info->hgt % 2 == 0); /* 2:align */
        vpss_assert_ret(cfg_info->wth <= max_in_width);
        vpss_assert_ret(cfg_info->hgt <= max_in_height);
        vpss_assert_ret(out_height == (cfg_info->hgt / 2)); /* 2:align */
        vpss_assert_ret(lvfir_offset == 0);
        vpss_assert_ret(vratio == (1 << 13)); /* 13:num */
        cfg_info->hgt = out_height;
    }

    return HI_SUCCESS;
}
hi_u32 vpss_tnr_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 tnr_en = reg->vpss_ctrl.bits.tnr_en;
    hi_u32 u8meds = reg->vpss_ctrl.bits.meds_en;
    hi_u32 top_mad_mode = reg->vpss_ctrl.bits.tnr_mad_mode;
    hi_u32 mcnr_en = reg->vpss_ctrl.bits.mcnr_en;
    hi_u32 md_cf_mode = reg->vpss_hipp_tnr_reg36.bits.md_cf_mode;
    hi_u32 tf_cf_mode = reg->vpss_hipp_tnr_reg36.bits.tf_cf_mode;
    hi_u32 blackb_high_thr = reg->vpss_hipp_lbd_thr_2.bits.blackb_high_thr;
    hi_u32 blackb_thr = reg->vpss_hipp_lbd_thr_0.bits.blackb_thr;
    hi_u32 blackt_high_thr = reg->vpss_hipp_lbd_thr_2.bits.blackt_high_thr;
    hi_u32 blackt_thr = reg->vpss_hipp_lbd_thr_0.bits.blackt_thr;
    hi_u32 blackr_high_thr = reg->vpss_hipp_lbd_thr_3.bits.blackr_high_thr;
    hi_u32 blackr_thr = reg->vpss_hipp_lbd_thr_1.bits.blackr_thr;
    hi_u32 blackl_high_thr = reg->vpss_hipp_lbd_thr_3.bits.blackl_high_thr;
    hi_u32 blackl_thr = reg->vpss_hipp_lbd_thr_1.bits.blackl_thr;
    hi_u32 lb_max_grad_thr = reg->vpss_hipp_lbd_thr_6.bits.lb_max_grad_thr;
    hi_u32 lb_grad_thr = reg->vpss_hipp_lbd_thr_6.bits.lb_grad_thr;
    hi_u32 motion_estimate_en = reg->vpss_hipp_tnr_reg62.bits.motion_estimate_en;
    hi_u32 cb_cr_update_en = reg->vpss_hipp_tnr_reg67.bits.cb_cr_update_en;
    hi_u32 y_mc_adj_en = reg->vpss_hipp_tnr_reg70.bits.y_mc_adj_en;
    hi_u32 mc_adj_en = reg->vpss_hipp_tnr_reg70.bits.mc_adj_en;
    hi_u32 mc_mt_sm_nlv_x1 = reg->vpss_hipp_tnr_reg215.bits.mc_mt_sm_nlv_x1;
    hi_u32 mc_mt_sm_nlv_x0 = reg->vpss_hipp_tnr_reg215.bits.mc_mt_sm_nlv_x0;
    hi_u32 int_bicubical_en = reg->vpss_hipp_tnr_reg0.bits.int_bicubical_en;
    hi_u32 pre_spnr_en = reg->vpss_hipp_tnr_reg5.bits.pre_spnr_en;
    hi_u32 mc_mt_sad_adj_en = reg->vpss_hipp_tnr_reg230.bits.mc_mt_sad_adj_en;
    hi_u32 calc_delta_en = reg->vpss_hipp_tnr_reg50.bits.calc_delta_en;
    hi_u32 tnr_mode = reg->vpss_hipp_tnr_reg0.bits.tnr_mode;
    hi_u32 ccs_en = reg->vpss_hipp_tnr_reg188.bits.ccs_en;
    hi_u32 cls3d_en = reg->vpss_hipp_tnr_reg166.bits.cls3d_en;
    hi_u32 cls3d_mc_en = reg->vpss_hipp_tnr_reg166.bits.cls3d_mc_en;
    hi_u32 cc_en = reg->vpss_hipp_tnr_reg214.bits.cc_en;
    hi_u32 cl_en = reg->vpss_hipp_tnr_reg214.bits.cl_en;
    hi_u32 nr_rmad_width = reg->vpss_nr_rmad_size.bits.nr_rmad_width + 1;
    hi_u32 nr_rmad_height = reg->vpss_nr_rmad_size.bits.nr_rmad_height + 1;
    hi_u32 nr_cfmv_width = reg->vpss_nr_cfmv_size.bits.nr_cfmv_width + 1;
    hi_u32 nr_cfmv_height = reg->vpss_nr_cfmv_size.bits.nr_cfmv_height + 1;
    hi_u32 nr_p1rgmv_width = reg->vpss_nr_p1_rgmv_size.bits.nr_p1rgmv_width + 1;
    hi_u32 nr_p1rgmv_height = reg->vpss_nr_p1_rgmv_size.bits.nr_p1rgmv_height + 1;
    hi_u32 nr_rcnt_width = reg->vpss_nr_rcnt_size.bits.nr_rcnt_width + 1;
    hi_u32 nr_rcnt_height = reg->vpss_nr_rcnt_size.bits.nr_rcnt_height + 1;
    hi_u32 nr_ref_en = reg->vpss_nr_ref_ctrl.bits.nr_ref_en;
    hi_u32 nr_rmad_en = reg->vpss_nr_rmad_ctrl.bits.nr_rmad_en;
    hi_u32 nr_cfmv_en = reg->vpss_nr_cfmv_ctrl.bits.nr_cfmv_en;
    hi_u32 nr_p1rgmv_en = reg->vpss_nr_p1_rgmv_ctrl.bits.nr_p1rgmv_en;
    hi_u32 nr_p2rgmv_en = reg->vpss_nr_p2_rgmv_ctrl.bits.nr_p2rgmv_en;
    hi_u32 nr_rcnt_en = reg->vpss_nr_rcnt_ctrl.bits.nr_rcnt_en;
    hi_u32 nr_wmad_en = reg->vpss_nr_wmad_ctrl.bits.nr_wmad_en;
    hi_u32 nr_wcnt_en = reg->vpss_nr_wcnt_ctrl.bits.nr_wcnt_en;
    hi_u32 nr_rfr_en = reg->vpss_nr_rfr_ctrl.bits.nr_rfr_en;
    hi_u32 wth_3drs = u8meds == 1 ? cfg_info->wth >> 1 : cfg_info->wth;

    if (tnr_en == 1) {
        vpss_assert_ret(blackb_high_thr >= blackb_thr);
        vpss_assert_ret(blackt_high_thr >= blackt_thr);
        vpss_assert_ret(blackr_high_thr >= blackr_thr);
        vpss_assert_ret(blackl_high_thr >= blackl_thr);
        vpss_assert_ret(lb_max_grad_thr >= lb_grad_thr);

        if ((cfg_info->wth > 1920) || (cfg_info->hgt > 1088)) { /* 1920,1088:para */
            vpss_assert_ret(mcnr_en == 0);
        }

        if (cfg_info->wth <= 1920) { /* 1920:para */
            vpss_assert_ret(md_cf_mode == 0);
            vpss_assert_ret(tf_cf_mode == 0);
        }

        if (cfg_info->wth < 128) { /* 128:para */
            vpss_assert_ret(motion_estimate_en == 0);
        }

        if (cfg_info->wth > 1920) { /* 1920:para */
            vpss_assert_ret(cb_cr_update_en == 0);
        }

        if (motion_estimate_en == 0) {
            vpss_assert_ret(y_mc_adj_en == 0);
            vpss_assert_ret(mc_adj_en == 0);
        }

        vpss_assert_ret(mc_mt_sm_nlv_x1 > mc_mt_sm_nlv_x0);
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 5); /* 5:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 6); /* 6:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 9); /* 9:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 13); /* 13:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 18); /* 18:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 19); /* 19:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 21); /* 21:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 22); /* 22:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 23); /* 23:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 24); /* 24:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 25); /* 25:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 27); /* 27:para */
        vpss_assert_ret((mc_mt_sm_nlv_x1 - mc_mt_sm_nlv_x0) != 28); /* 28:para */
        vpss_assert_ret(int_bicubical_en == 0);
        vpss_assert_ret(pre_spnr_en == 0);
        vpss_assert_ret(mc_mt_sad_adj_en == 0);

        if ((tf_cf_mode == 0) && (md_cf_mode == 0)) {
            vpss_assert_ret(calc_delta_en == 0);
        }

        if (mcnr_en == 0) {
            vpss_assert_ret(tnr_mode <= 1);
        }

        if (cfg_info->wth > 1920) { /* 1920:para */
            vpss_assert_ret(tnr_mode <= 1);
        }
        /* 960,576:para */
        if ((cfg_info->wth > 960) || (cfg_info->hgt > 576) || (cfg_info->format != XDP_PROC_FMT_SP_422)) {
            vpss_assert_ret(ccs_en == 0);
            vpss_assert_ret(cls3d_en == 0);
        }

        if (mcnr_en == 0) {
            vpss_assert_ret(cls3d_mc_en == 0);
        }

        vpss_assert_ret(cc_en == 0);
        vpss_assert_ret(cl_en == 0);

        if (top_mad_mode == 2) { /* 2:para */
            vpss_assert_ret(nr_rmad_width == ((cfg_info->wth + 7) >> 3) * 2);  /* 7, 3, 2 : para */
        } else {
            vpss_assert_ret(nr_rmad_width == cfg_info->wth);
        }

        vpss_assert_ret(nr_rmad_height == cfg_info->hgt);
        vpss_assert_ret(nr_p1rgmv_width == ((wth_3drs + 33) >> 6));  /* 33, 6:para */
        vpss_assert_ret(nr_p1rgmv_height == ((cfg_info->hgt + 9) >> 4));  /* 9,4:para */
        vpss_assert_ret(nr_ref_en == 1);
        vpss_assert_ret(nr_rmad_en == 1);
        vpss_assert_ret(nr_p1rgmv_en == 1);
        vpss_assert_ret(nr_p2rgmv_en == 1);
        vpss_assert_ret(nr_wmad_en == 1);
        vpss_assert_ret(nr_rfr_en == 1);

        if (mcnr_en == 1) {
            vpss_assert_ret(nr_cfmv_width == ((wth_3drs + 7) >> 3));  /* 7,3:para */
            vpss_assert_ret(nr_cfmv_height == ((cfg_info->hgt + 3) >> 2)); /* 3,2:para */
            vpss_assert_ret(nr_rcnt_width == ((cfg_info->wth + 7) >> 3)); /* 7,3:para */
            vpss_assert_ret(nr_rcnt_height == cfg_info->hgt);
            vpss_assert_ret(nr_cfmv_en == 1);
            vpss_assert_ret(nr_rcnt_en == 1);
            vpss_assert_ret(nr_wcnt_en == 1);
        } else {
            vpss_assert_ret(nr_cfmv_en == 0);
            vpss_assert_ret(nr_rcnt_en == 0);
            vpss_assert_ret(nr_wcnt_en == 0);
        }
    }

    return HI_SUCCESS;
}
hi_u32 vpss_snr_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    hi_u32 snr_en = reg->vpss_ctrl.bits.snr_en;
    hi_u32 tnr_en = reg->vpss_ctrl.bits.tnr_en;
    hi_u32 dei_en = reg->vpss_ctrl.bits.dei_en;
    hi_u32 ma_mc_sel = reg->vpss_ctrl.bits.ma_mac_sel;
    hi_u32 me_version = reg->vpss_ctrl.bits.me_version;
    hi_u32 mad_ctrl = 0;
    hi_u32 mndet_en = reg->vpss_hipp_snr_reg57.bits.mndet_en;
    hi_u32 lum_h_blk_size = reg->vpss_hipp_snr_reg45.bits.lum_h_blk_size;
    hi_u32 lum_v_blk_size = reg->vpss_hipp_snr_reg45.bits.lum_v_blk_size;
    hi_u32 snr_mad_disable = reg->vpss_ctrl.bits.snr_mad_disable;
    hi_u32 snr_out_mode = reg->vpss_hipp_snr_reg0.bits.mode_out;
    hi_u32 snr_en_3 = reg->vpss_hipp_snr_reg0.bits.snr_en_3;
    hi_u32 ccs2d_en = reg->vpss_hipp_snr_reg0.bits.ccs2d_en;
    hi_u32 cbcr_v5h1_flt_en = reg->vpss_hipp_snr_reg0.bits.cbcr_v5h1_flt_en;
    hi_u32 db_lumver_en = reg->vpss_hipp_snr_reg0.bits.db_lumver_en;
    hi_u32 mndet_lumver_en = reg->vpss_hipp_snr_reg57.bits.mndet_lumver_en;
    hi_u32 ds_en = reg->vpss_hipp_snr_reg0.bits.ds_en;
    hi_u32 dr_en = reg->vpss_hipp_snr_reg0.bits.dr_en;
    hi_u32 c2d_winheight = reg->vpss_hipp_snr_reg122.bits.c2d_winheight;
    hi_u32 c2d_winwidth = reg->vpss_hipp_snr_reg122.bits.c2d_winwidth;
    hi_u32 y2d_winheight = reg->vpss_hipp_snr_reg122.bits.y2d_winheight;
    hi_u32 y2d_winwidth = reg->vpss_hipp_snr_reg122.bits.y2d_winwidth;
    hi_u32 snr_rmad_en = reg->vpss_snr_rmad_ctrl.bits.snr_rmad_en;

    /******************* get reg value *********************/
    if (dei_en == 1) {
        mad_ctrl = 1;
    } else if (ma_mc_sel == 1 && me_version == 0) {  // V3
        mad_ctrl = 2; /* 2:para */
    } else {
        mad_ctrl = 0;
    }

    if (snr_en == 1) {
        if (tnr_en == 0 && mad_ctrl == 0) {
            vpss_assert_ret(snr_mad_disable == 1);
        }

        if (mad_ctrl != 0) {
            vpss_assert_ret(snr_rmad_en == 1);
        }

        if (mndet_en == 1) {
            vpss_assert_ret(cfg_info->wth % 2 == 0); /* 2:align */
        }

        vpss_assert_ret(snr_out_mode <= 6);  /* 6:para */
        vpss_assert_ret(lum_h_blk_size >= 4);  /* 4:para */
        vpss_assert_ret(lum_h_blk_size <= 64); /* 64:para */
        vpss_assert_ret(lum_v_blk_size >= 4); /* 4:para */
        vpss_assert_ret(lum_v_blk_size <= 64); /* 64:para */

        if ((snr_en_3 != 1) || (cfg_info->format != XDP_PROC_FMT_SP_422) || (cfg_info->wth > 960)) { /* 1,960:para */
            vpss_assert_ret(ccs2d_en == 0);
            vpss_assert_ret(cbcr_v5h1_flt_en == 0);
        }

        if ((cfg_info->wth > 1920) || (cfg_info->hgt > 1088)) { /* 1920,1088:para */
            vpss_assert_ret(db_lumver_en == 0);
            vpss_assert_ret(mndet_lumver_en == 0);
            vpss_assert_ret(ds_en == 0);
            vpss_assert_ret(dr_en == 0);
        }

        vpss_assert_ret(c2d_winheight <= 3); /* 3:para */
        vpss_assert_ret(c2d_winheight % 2 == 1); /* 2:align */
        vpss_assert_ret(c2d_winwidth <= 5); /* 5:para */
        vpss_assert_ret(c2d_winwidth % 2 == 1); /* 2:para */
        vpss_assert_ret(y2d_winheight <= 5); /* 5:para */
        vpss_assert_ret(y2d_winheight % 2 == 1); /* 2:align */
        vpss_assert_ret(y2d_winwidth <= 9); /* 9:para */
        vpss_assert_ret(y2d_winwidth % 2 == 1); /* 2:align */
        vpss_assert_ret(y2d_winwidth != 7); /* 7:para */
    } else {
        vpss_assert_ret(snr_rmad_en == 0);
    }

    return HI_SUCCESS;
}

hi_u32 vpss_scd_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    return HI_SUCCESS;
}

hi_u32 vpss_scan_config_check(vpss_reg_type *reg_pre, vpss_reg_type *reg)
{
    /******************* declairation  *********************/
    hi_u32 me_en;
    hi_u32 vpss_3drs_en;
    hi_u32 layer;
    hi_u32 me_mv_upsmp_en;
    hi_u32 me_mv_dnsmp_en;
    hi_u32 pre_layer;
    hi_u32 me_cfmv_addr_l;
    hi_u32 me_cfgmv_addr_l;
    hi_u32 pre_me_cfmv_addr_l;
    hi_u32 pre_me_cfgmv_addr_l;
    /******************* get reg value *********************/
    vpss_3drs_en = reg->vpss_ctrl.bits.vpss_3drs_en;
    me_en = reg->scan_ctrl.bits.me_en & vpss_3drs_en;
    layer = reg->scan_ctrl.bits.me_layer;
    me_mv_upsmp_en = reg->scan_ctrl.bits.me_mv_upsmp_en;
    me_mv_dnsmp_en = reg->scan_ctrl.bits.me_mv_dnsmp_en;
    pre_layer = reg_pre->scan_ctrl.bits.me_layer;
    me_cfmv_addr_l = reg->vpss_me_cfmv_addr_low.bits.me_cfmv_addr_l;
    me_cfgmv_addr_l = reg->vpss_me_cfgmv_addr_low.bits.me_cfgmv_addr_l;
    pre_me_cfmv_addr_l = reg_pre->vpss_me_cfmv_addr_low.bits.me_cfmv_addr_l;
    pre_me_cfgmv_addr_l = reg_pre->vpss_me_cfgmv_addr_low.bits.me_cfgmv_addr_l;

    if (me_en == 1) {
        if ((layer == 0) && (pre_layer == 0)) {
            vpss_assert_ret(me_mv_dnsmp_en == 0);
            vpss_assert_ret(me_mv_upsmp_en == 0);
        }

        if ((layer == 0) && (pre_layer == 1)) {
            vpss_assert_ret(me_mv_dnsmp_en == 0);
            vpss_assert_ret(me_mv_upsmp_en == 1);
        }

        if ((layer == 1) && (pre_layer == 0)) {
            vpss_assert_ret(me_mv_dnsmp_en == 1);
            vpss_assert_ret(me_mv_upsmp_en == 0);
        }

        if ((layer == 1) && (pre_layer == 1)) {
            vpss_assert_ret(me_mv_dnsmp_en == 0);
            vpss_assert_ret(me_mv_upsmp_en == 0);
        }

        vpss_assert_ret(me_cfmv_addr_l == pre_me_cfmv_addr_l);
        vpss_assert_ret(me_cfgmv_addr_l == pre_me_cfgmv_addr_l);
    }

    return HI_SUCCESS;
}
hi_u32 vpss_rgme_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 rgme_en;
    hi_u32 vpss_3drs_en;
    hi_u32 rg_rprjh_width;
    hi_u32 rg_rprjh_height;
    hi_u32 rg_rprjv_width;
    hi_u32 rg_rprjv_height;
    hi_u32 rg_p1rgmv_width;
    hi_u32 rg_p1rgmv_height;
    hi_u32 rg_rprjh_en = reg->vpss_rg_rprjh_ctrl.bits.rg_rprjh_en;
    hi_u32 rg_rprjv_en = reg->vpss_rg_rprjv_ctrl.bits.rg_rprjv_en;
    hi_u32 rg_p1rgmv_en = reg->vpss_rg_p1_rgmv_ctrl.bits.rg_p1rgmv_en;
    hi_u32 rg_p2rgmv_en = reg->vpss_rg_p2_rgmv_ctrl.bits.rg_p2rgmv_en;
    hi_u32 rg_wprjh_en = reg->vpss_rg_wprjh_ctrl.bits.rg_wprjh_en;
    hi_u32 rg_wprjv_en = reg->vpss_rg_wprjv_ctrl.bits.rg_wprjv_en;
    hi_u32 rg_cfrgmv_en = reg->vpss_rg_cfrgmv_ctrl.bits.rg_cfrgmv_en;
    /******************* get reg value *********************/
    vpss_3drs_en = reg->vpss_ctrl.bits.vpss_3drs_en;
    rgme_en = reg->vpss_ctrl.bits.rgme_en & vpss_3drs_en;
    rg_rprjh_width = reg->vpss_rg_rprjh_size.bits.rg_rprjh_width + 1;
    rg_rprjh_height = reg->vpss_rg_rprjh_size.bits.rg_rprjh_height + 1;
    rg_rprjv_width = reg->vpss_rg_rprjv_size.bits.rg_rprjv_width + 1;
    rg_rprjv_height = reg->vpss_rg_rprjv_size.bits.rg_rprjv_height + 1;
    rg_p1rgmv_width = reg->vpss_rg_p1_rgmv_size.bits.rg_p1rgmv_width + 1;
    rg_p1rgmv_height = reg->vpss_rg_p1_rgmv_size.bits.rg_p1rgmv_height + 1;

    if (rgme_en == 1) {
        vpss_assert_ret(cfg_info->wth >= 128); /* 128:para */
        vpss_assert_ret(cfg_info->wth <= 2048); /* 2048:para */
        vpss_assert_ret(cfg_info->hgt <= 2304); /* 2304:para */
        vpss_assert_ret(cfg_info->wth >= 64); /* 64:para */
        vpss_assert_ret(rg_p1rgmv_width == ((cfg_info->wth + 33) >> 6)); /* 33 6:para */
        vpss_assert_ret(rg_p1rgmv_height == ((cfg_info->hgt + 9) >> 4)); /* 9 4:para */
        vpss_assert_ret(rg_rprjh_width == (((cfg_info->wth + 33) >> 6) * 128)); /* 33 6 128:para */
        vpss_assert_ret(rg_rprjh_height == ((cfg_info->hgt + 9) >> 4)); /* 9 4:para */
        vpss_assert_ret(rg_rprjv_width == (((cfg_info->wth + 33) >> 6) * 32)); /* 33 6 32:para */
        vpss_assert_ret(rg_rprjv_height == ((cfg_info->hgt + 9) >> 4)); /* 9 4:para */
        vpss_assert_ret(rg_rprjh_en == 1);
        vpss_assert_ret(rg_rprjv_en == 1);
        vpss_assert_ret(rg_p1rgmv_en == 1);
        vpss_assert_ret(rg_p2rgmv_en == 1);
        vpss_assert_ret(rg_wprjh_en == 1);
        vpss_assert_ret(rg_wprjv_en == 1);
        vpss_assert_ret(rg_cfrgmv_en == 1);
    } else {
        vpss_assert_ret(rg_rprjh_en == 0);
        vpss_assert_ret(rg_rprjv_en == 0);
        vpss_assert_ret(rg_p1rgmv_en == 0);
        vpss_assert_ret(rg_p2rgmv_en == 0);
        vpss_assert_ret(rg_wprjh_en == 0);
        vpss_assert_ret(rg_wprjv_en == 0);
        vpss_assert_ret(rg_cfrgmv_en == 0);
    }

    return HI_SUCCESS;
}
hi_u32 vpss_out3_pzme_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 pzme_en = reg->vpss_pzme_ctrl.bits.pzme_en;
    hi_u32 out_width = reg->vpss_pzme_out_img.bits.pzme_out_img_width + 1;
    hi_u32 out_height = reg->vpss_pzme_out_img.bits.pzme_out_img_height + 1;
    hi_u32 h_step = reg->vpss_pzme_h_step.bits.pzme_h_step;
    hi_u32 w_step = reg->vpss_pzme_w_step.bits.pzme_w_step;

    if (pzme_en == 1) {
        if (cfg_info->pro == 0) {
            out_height /= 2; /* 2:para */
        }

        vpss_assert_ret(out_width <= cfg_info->wth);
        vpss_assert_ret(out_width > (cfg_info->wth / 8)); /* 8:para */
        vpss_assert_ret(out_width % 2 == 0); /* 2:para */
        vpss_assert_ret(out_height <= cfg_info->hgt);
        vpss_assert_ret(out_height > (cfg_info->hgt / 8)); /* 8:para */
        vpss_assert_ret(out_height % 2 == 0); /* 2:para */
        vpss_assert_ret(h_step == ((cfg_info->hgt << 16) + out_height / 2) / out_height); /* 16 2 :para */
        vpss_assert_ret(w_step == ((cfg_info->wth << 16) + out_width / 2) / out_width); /* 16 2 :para */
        cfg_info->wth = out_width;
        cfg_info->hgt = out_height;
    }

    return HI_SUCCESS;
}
hi_u32 vpss_out0_pzme_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 pzme_en = reg->vpss_out0_pzme_ctrl.bits.vpss_out0_pzme_en;
    hi_u32 out_width = reg->vpss_out0_pzme_out_img.bits.vpss_out0_pzme_out_img_width + 1;
    hi_u32 out_height = reg->vpss_out0_pzme_out_img.bits.vpss_out0_pzme_out_img_height + 1;
    hi_u32 h_step = reg->vpss_out0_pzme_h_step.bits.vpss_out0_pzme_h_step;
    hi_u32 w_step = reg->vpss_out0_pzme_w_step.bits.vpss_out0_pzme_w_step;
    hi_u32 pzme_mode = reg->vpss_out0_pzme_ctrl.bits.vpss_out0_pzme_mode;

    if (pzme_en == 1) {
        if (cfg_info->pro == 0) {
            out_height /= 2; /* 2:para */
        }

        vpss_assert_ret(out_width <= cfg_info->wth);
        vpss_assert_ret(out_width > (cfg_info->wth / 8)); /* 8:para */
        vpss_assert_ret(out_width % 2 == 0); /* 2:para */
        vpss_assert_ret(out_height <= cfg_info->hgt);
        vpss_assert_ret(out_height > (cfg_info->hgt / 8)); /* 8:para */
        vpss_assert_ret(out_height % 2 == 0); /* 2:para */
        /* 2 4 :para */
        vpss_assert_ret((out_height == cfg_info->hgt / 2 * 2) || (out_height == cfg_info->hgt / 2 / 2 * 2) ||
                        (out_height == cfg_info->hgt / 4 / 2 * 2)); /* 4,2,2:para */
        /* 2 4 0x10000 :para */
        vpss_assert_ret((h_step == 0x10000) || (h_step == 0x10000 * 2) || (h_step == 0x10000 * 4));

        if (out_height == cfg_info->hgt / 2 * 2) { /* 2:para */
            vpss_assert_ret(h_step == 0x10000); /* 0x10000:para */
        } else if (out_height == cfg_info->hgt / 2 / 2 * 2) { /* 2:para */
            vpss_assert_ret(h_step == 0x10000 * 2); /* 0x1000 2:para */
        } else if (out_height == cfg_info->hgt / 4 / 2 * 2) { /* 4 2 :para */
            vpss_assert_ret(h_step == 0x10000 * 4); /* 0x1000 4 :para */
        }

        vpss_assert_ret(w_step == ((cfg_info->wth << 16) + out_width / 2) / out_width); /* 16 2 :para */

        if (cfg_info->format == XDP_PROC_FMT_SP_420) {
            vpss_assert_ret(pzme_mode == 0);
        }

        cfg_info->wth = out_width;
        cfg_info->hgt = out_height;

        if (pzme_mode == 1) {
            cfg_info->format = XDP_PROC_FMT_SP_420;
        }
    }

    return HI_SUCCESS;
}

hi_u32 vpss_me_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 me_en;
    hi_u32 vpss_3drs_en;
    hi_u32 top_bndy;
    hi_u32 bot_bndy;
    hi_u32 lft_bndy;
    hi_u32 rgt_bndy;
    hi_u32 blk_mv_hist_thd_0;
    hi_u32 blk_mv_hist_thd_1;
    hi_u32 blk_mv_hist_thd_2;
    hi_u32 blk_mv_hist_thd_3;
    hi_u32 layer;
    hi_u32 me_mv_upsmp_en;
    hi_u32 me_mv_dnsmp_en;
    hi_u32 me_p1mv_width;
    hi_u32 me_p1mv_height;
    hi_u32 me_prmv_width;
    hi_u32 me_prmv_height;
    hi_u32 me_p1gmv_width;
    hi_u32 me_p1gmv_height;
    hi_u32 me_prgmv_width;
    hi_u32 me_prgmv_height;
    hi_u32 me_p1rgmv_width;
    hi_u32 me_p1rgmv_height;
    hi_u32 me_p2rgmv_width;
    hi_u32 me_p2rgmv_height;
    hi_u32 small_wth;
    hi_u32 blk_wth_big;
    hi_u32 blk_hgt_big;
    hi_u32 blk_wth_sml;
    hi_u32 blk_hgt_sml;
    hi_u32 me_cf_en = reg->vpss_me_cf_ctrl.bits.me_cf_en;
    hi_u32 me_ref_en = reg->vpss_me_ref_ctrl.bits.me_ref_en;
    hi_u32 me_p1mv_en = reg->vpss_me_p1_mv_ctrl.bits.me_p1mv_en;
    hi_u32 me_prmv_en = reg->vpss_me_prmv_ctrl.bits.me_prmv_en;
    hi_u32 me_p1gmv_en = reg->vpss_me_p1_gmv_ctrl.bits.me_p1gmv_en;
    hi_u32 me_prgmv_en = reg->vpss_me_prgmv_ctrl.bits.me_prgmv_en;
    hi_u32 me_p1rgmv_en = reg->vpss_me_p1_rgmv_ctrl.bits.me_p1rgmv_en;
    hi_u32 me_p2rgmv_en = reg->vpss_me_p2_rgmv_ctrl.bits.me_p2rgmv_en;
    hi_u32 me_cfmv_en = reg->vpss_me_cfmv_ctrl.bits.me_cfmv_en;
    hi_u32 me_cfgmv_en = reg->vpss_me_cfgmv_ctrl.bits.me_cfgmv_en;
    /******************* get reg value *********************/
    vpss_3drs_en = 1;
    me_en = reg->scan_ctrl.bits.me_en & vpss_3drs_en;
    top_bndy = reg->vpss_hipp_me_bndy.bits.top_bndy;
    bot_bndy = reg->vpss_hipp_me_bndy.bits.bot_bndy;
    lft_bndy = reg->vpss_hipp_me_bndy.bits.lft_bndy;
    rgt_bndy = reg->vpss_hipp_me_bndy.bits.rgt_bndy;
    blk_mv_hist_thd_0 = reg->vpss_hipp_me_hist.bits.blk_mv_hist_thd_0;
    blk_mv_hist_thd_1 = reg->vpss_hipp_me_hist.bits.blk_mv_hist_thd_1;
    blk_mv_hist_thd_2 = reg->vpss_hipp_me_hist.bits.blk_mv_hist_thd_2;
    blk_mv_hist_thd_3 = reg->vpss_hipp_me_hist.bits.blk_mv_hist_thd_3;
    layer = reg->scan_ctrl.bits.me_layer;
    me_mv_upsmp_en = reg->scan_ctrl.bits.me_mv_upsmp_en;
    me_mv_dnsmp_en = reg->scan_ctrl.bits.me_mv_dnsmp_en;
    me_p1mv_width = reg->vpss_me_p1_mv_size.bits.me_p1mv_width + 1;
    me_p1mv_height = reg->vpss_me_p1_mv_size.bits.me_p1mv_height + 1;
    me_prmv_width = reg->vpss_me_prmv_size.bits.me_prmv_width + 1;
    me_prmv_height = reg->vpss_me_prmv_size.bits.me_prmv_height + 1;
    me_p1gmv_width = reg->vpss_me_p1_gmv_size.bits.me_p1gmv_width + 1;
    me_p1gmv_height = reg->vpss_me_p1_gmv_size.bits.me_p1gmv_height + 1;
    me_prgmv_width = reg->vpss_me_prgmv_size.bits.me_prgmv_width + 1;
    me_prgmv_height = reg->vpss_me_prgmv_size.bits.me_prgmv_height + 1;
    me_p1rgmv_width = reg->vpss_me_p1_rgmv_size.bits.me_p1rgmv_width + 1;
    me_p1rgmv_height = reg->vpss_me_p1_rgmv_size.bits.me_p1rgmv_height + 1;
    me_p2rgmv_width = reg->vpss_me_p2_rgmv_size.bits.me_p2rgmv_width + 1;
    me_p2rgmv_height = reg->vpss_me_p2_rgmv_size.bits.me_p2rgmv_height + 1;
    blk_wth_big = (cfg_info->wth + 7) / 8; /* 7,8:para */
    blk_hgt_big = (cfg_info->hgt + 3) / 4; /* 3,4:para */
    small_wth = (cfg_info->wth % 2 == 1) ? (cfg_info->wth / 2 + 1) : cfg_info->wth / 2; /* 2,1,2,1,2:para */
    blk_wth_sml = (small_wth + 7) / 8; /* 7,8:para */
    blk_hgt_sml = (cfg_info->hgt / 2 + 3) / 4; /* 2,3,4:para */

    if (me_en == 1) {
        if (layer == 0) {
            vpss_assert_ret(me_mv_dnsmp_en == 0);
            vpss_assert_ret(cfg_info->wth >= 136); /* 136:para */
            vpss_assert_ret(cfg_info->hgt >= 72); /* 72:para */
        } else {
            vpss_assert_ret(me_mv_upsmp_en == 0);
            vpss_assert_ret(cfg_info->wth >= 128); /* 128:para */
            vpss_assert_ret(cfg_info->hgt >= 64); /* 64:para */
        }

        vpss_assert_ret(cfg_info->wth <= 960); /* 960:para */
        vpss_assert_ret(cfg_info->hgt <= 1088); /* 1088:para */
        vpss_assert_ret(cfg_info->wth % 2 == 0); /* 2:para */
        vpss_assert_ret(rgt_bndy <= 24); /* 24:para */
        vpss_assert_ret(lft_bndy <= 24); /* 24:para */
        vpss_assert_ret(bot_bndy <= 24); /* 24:para */
        vpss_assert_ret(top_bndy <= 24); /* 24:para */
        vpss_assert_ret(blk_mv_hist_thd_1 >= blk_mv_hist_thd_0);
        vpss_assert_ret(blk_mv_hist_thd_2 >= blk_mv_hist_thd_1);
        vpss_assert_ret(blk_mv_hist_thd_3 >= blk_mv_hist_thd_2);
        vpss_assert_ret(blk_mv_hist_thd_3 <= 162); /* 162:para */
        vpss_assert_ret(me_p1mv_width == blk_wth_big);
        vpss_assert_ret(me_p1mv_height == blk_hgt_big);
        vpss_assert_ret(me_p1gmv_width == 8); /* 8:para */
        vpss_assert_ret(me_p1gmv_height == 15); /* 15:para */
        vpss_assert_ret(me_prgmv_width == 8); /* 8:para */
        vpss_assert_ret(me_prgmv_height == 15); /* 15:para */
        vpss_assert_ret(me_p1rgmv_width == ((cfg_info->wth + 33) >> 6)); /* 33 6 :para */
        vpss_assert_ret(me_p1rgmv_height == ((cfg_info->hgt + 9) >> 4)); /* 9 4 :para */
        vpss_assert_ret(me_p2rgmv_width == ((cfg_info->wth + 33) >> 6)); /* 33 6 :para */
        vpss_assert_ret(me_p2rgmv_height == ((cfg_info->hgt + 9) >> 4)); /* 9 4 :para */

        if ((layer == 1 && me_mv_upsmp_en == 0 && me_mv_dnsmp_en == 0) || (layer == 0 && me_mv_upsmp_en == 1 &&
                me_mv_dnsmp_en == 0)) {
            vpss_assert_ret(me_prmv_width == blk_wth_sml);
            vpss_assert_ret(me_prmv_height == blk_hgt_sml);
        } else {
            vpss_assert_ret(me_prmv_width == blk_wth_big);
            vpss_assert_ret(me_prmv_height == blk_hgt_big);
        }

        vpss_assert_ret(me_cf_en == 1);
        vpss_assert_ret(me_ref_en == 1);
        vpss_assert_ret(me_p1mv_en == 1);
        vpss_assert_ret(me_prmv_en == 1);
        vpss_assert_ret(me_p1gmv_en == 1);
        vpss_assert_ret(me_prgmv_en == 1);
        vpss_assert_ret(me_p1rgmv_en == 1);
        vpss_assert_ret(me_p2rgmv_en == 1);
        vpss_assert_ret(me_cfmv_en == 1);
        vpss_assert_ret(me_cfgmv_en == 1);
    } else {
        vpss_assert_ret(me_cf_en == 0);
        vpss_assert_ret(me_ref_en == 0);
        vpss_assert_ret(me_p1mv_en == 0);
        vpss_assert_ret(me_prmv_en == 0);
        vpss_assert_ret(me_p1gmv_en == 0);
        vpss_assert_ret(me_prgmv_en == 0);
        vpss_assert_ret(me_p1rgmv_en == 0);
        vpss_assert_ret(me_p2rgmv_en == 0);
        vpss_assert_ret(me_cfmv_en == 0);
        vpss_assert_ret(me_cfgmv_en == 0);
    }

    return HI_SUCCESS;
}
hi_u32 vpss_hzme_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    hi_u32 offset = 0;
    hi_u32 max_in_width = 0;
    hi_u32 lhfir_en;
    hi_u32 out_width;
    hi_u32 lhfir_offset;

    if (data == XDP_ZME_ID_VPSS_WR0_HDS_HZME) {
        offset = 0;
        max_in_width = 4096; /* 4096:para */
    } else if (data == XDP_ZME_ID_VPSS_WR1_HVDS_HZME) {
        offset = (0x8A00 - 0x8900) / 4; /* 0,8,00,0,8900,4:para */
        max_in_width = 960; /* 960:para */
    } else if (data == XDP_ZME_ID_VPSS_ME_CF_HDS_HZME) {
        offset = (0xC400 - 0x8900) / 4; /* 0,400,0,8900,4:para */
        max_in_width = 1920; /* 1920:para */
    } else if (data == XDP_ZME_ID_VPSS_ME_P2_HDS_HZME) {
        offset = (0xC500 - 0x8900) / 4; /* 0,500,0,8900,4:para */
        max_in_width = 1920; /* 1920:para */
    }

    reg = (vpss_reg_type *)((hi_u32 *)reg + offset);
    lhfir_en = reg->vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_lhfir_en;
    out_width = reg->vpss_wr0_hds_zme_hinfo.bits.vpss_wr0_hds_out_width + 1;
    lhfir_offset = reg->vpss_wr0_hds_zme_hloffset.bits.vpss_wr0_hds_lhfir_offset;

    if (lhfir_en == 1) {
        vpss_assert_ret(cfg_info->wth % 2 == 0); /* 2:para */
        vpss_assert_ret(cfg_info->wth <= max_in_width);
        vpss_assert_ret(out_width == ((cfg_info->wth / 2 + 1) / 2 * 2)); /* 2:para */
        vpss_assert_ret(lhfir_offset == 0);
        cfg_info->wth = out_width;
    }

    return HI_SUCCESS;
}
hi_u32 vpss_hsp_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    if (reg->vpss_ctrl.bits.hsp_en == 1) {
        vpss_assert_ret(cfg_info->wth <= 1920); /* 1920:para */
    }

    return HI_SUCCESS;
}

hi_u32 vpss_hfr_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    return HI_SUCCESS;
}
hi_u32 vpss_hcti_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    if (reg->vpss_ctrl.bits.hcti_en == 1) {
        vpss_assert_ret(cfg_info->wth <= 960); /* 960:para */
        vpss_assert_ret(reg->vpss_ctrl.bits.hcti_en == reg->vpss_hcti_ctrl.bits.cti_en);
    }

    return HI_SUCCESS;
}

hi_u32 vpss_dei_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 meds_en;
    hi_u32 dei_en;
    hi_u32 vpss_3drs_en;
    hi_u32 mcdi_en;
    hi_u32 igbm_en;
    hi_u32 ifmd_en;
    hi_u32 di_rsady_width;
    hi_u32 di_rsady_height;
    hi_u32 di_rhism_width;
    hi_u32 di_rhism_height;
    hi_u32 di_p1mv_width;
    hi_u32 di_p1mv_height;
    hi_u32 di_cf_en = reg->vpss_di_cf_ctrl.bits.di_cf_en;
    hi_u32 di_p1_en = reg->vpss_di_p1_ctrl.bits.di_p1_en;
    hi_u32 di_p2_en = reg->vpss_di_p2_ctrl.bits.di_p2_en;
    hi_u32 di_p3_en = reg->vpss_di_p3_ctrl.bits.di_p3_en;
    hi_u32 di_p3i_en = reg->vpss_di_p3_i_ctrl.bits.di_p3i_en;
    hi_u32 di_rsady_en = reg->vpss_di_rsady_ctrl.bits.di_rsady_en;
    hi_u32 di_rsadc_en = reg->vpss_di_rsadc_ctrl.bits.di_rsadc_en;
    hi_u32 di_rhism_en = reg->vpss_di_rhism_ctrl.bits.di_rhism_en;
    hi_u32 di_p1mv_en = reg->vpss_di_p1_mv_ctrl.bits.di_p1mv_en;
    hi_u32 di_p2mv_en = reg->vpss_di_p2_mv_ctrl.bits.di_p2mv_en;
    hi_u32 di_p3mv_en = reg->vpss_di_p3_mv_ctrl.bits.di_p3mv_en;
    hi_u32 di_rfr_en = reg->vpss_di_rfr_ctrl.bits.di_rfr_en;
    hi_u32 di_wsady_en = reg->vpss_di_wsady_ctrl.bits.di_wsady_en;
    hi_u32 di_wsadc_en = reg->vpss_di_wsadc_ctrl.bits.di_wsadc_en;
    hi_u32 di_whism_en = reg->vpss_di_whism_ctrl.bits.di_whism_en;
    vpss_3drs_en = reg->vpss_ctrl.bits.vpss_3drs_en;
    dei_en = reg->vpss_ctrl.bits.dei_en & vpss_3drs_en;
    meds_en = reg->vpss_ctrl.bits.meds_en;
    mcdi_en = reg->vpss_ctrl.bits.mcdi_en;
    igbm_en = reg->vpss_ctrl.bits.igbm_en;
    ifmd_en = reg->vpss_ctrl.bits.ifmd_en;
    di_rsady_width = reg->vpss_di_rsady_size.bits.di_rsady_width + 1;
    di_rsady_height = reg->vpss_di_rsady_size.bits.di_rsady_height + 1;
    di_rhism_width = reg->vpss_di_rhism_size.bits.di_rhism_width + 1;
    di_rhism_height = reg->vpss_di_rhism_size.bits.di_rhism_height + 1;
    di_p1mv_width = reg->vpss_di_p1_mv_size.bits.di_p1mv_width + 1;
    di_p1mv_height = reg->vpss_di_p1_mv_size.bits.di_p1mv_height + 1;

    /*******************   VPSS_ASSERT_RETion   *********************/
    // please add your VPSS_ASSERT_RETion here.
    if (dei_en == 1) {
        vpss_assert_ret(cfg_info->pro == 0);

        if (cfg_info->wth > 960) { /* 960:para */
            vpss_assert_ret(meds_en == 1);
        }

        if (cfg_info->wth < 256) { /* 256:para */
            vpss_assert_ret(meds_en == 0);
        }

        vpss_assert_ret(cfg_info->wth <= 1920); /* 1920:para */
        vpss_assert_ret(cfg_info->hgt <= 544); /* 544:para */

        if (mcdi_en == 0) {
            vpss_assert_ret(igbm_en == 0);
            vpss_assert_ret(ifmd_en == 0);
        }

        vpss_assert_ret(di_rsady_width == cfg_info->wth);
        vpss_assert_ret(di_rsady_height == cfg_info->hgt);
        vpss_assert_ret(di_p1mv_width == ((cfg_info->wth >> meds_en) + 7) / 8); /* 7 8 :para */
        vpss_assert_ret(di_p1mv_height == ((cfg_info->hgt + 3) / 4)); /* 3 4 :para */
        vpss_assert_ret(di_rhism_width == ((cfg_info->wth + 2) >> 2)); /* 3 2 :para */
        vpss_assert_ret(di_rhism_height == cfg_info->hgt);
        vpss_assert_ret(di_cf_en == 1);
        vpss_assert_ret(di_p1_en == 1);
        vpss_assert_ret(di_p2_en == 1);
        vpss_assert_ret(di_p3_en == 1);
        vpss_assert_ret(di_p3i_en == 1);
        vpss_assert_ret(di_rsady_en == 1);
        vpss_assert_ret(di_rsadc_en == 1);
        vpss_assert_ret(di_rhism_en == 1);
        vpss_assert_ret(di_p1mv_en == 1);
        vpss_assert_ret(di_p2mv_en == 1);
        vpss_assert_ret(di_p3mv_en == 1);
        vpss_assert_ret(di_rfr_en == 1);
        vpss_assert_ret(di_wsady_en == 1);
        vpss_assert_ret(di_wsadc_en == 1);
        vpss_assert_ret(di_whism_en == 1);
        cfg_info->pro = 1;
        cfg_info->hgt *= 2; /* 2:para */
    } else {
        vpss_assert_ret(di_p1_en == 0);
        vpss_assert_ret(di_p2_en == 0);
        vpss_assert_ret(di_p3_en == 0);
        vpss_assert_ret(di_p3i_en == 0);
        vpss_assert_ret(di_rsady_en == 0);
        vpss_assert_ret(di_rsadc_en == 0);
        vpss_assert_ret(di_rhism_en == 0);
        vpss_assert_ret(di_p1mv_en == 0);
        vpss_assert_ret(di_p2mv_en == 0);
        vpss_assert_ret(di_p3mv_en == 0);
        vpss_assert_ret(di_rfr_en == 0);
        vpss_assert_ret(di_wsady_en == 0);
        vpss_assert_ret(di_wsadc_en == 0);
        vpss_assert_ret(di_whism_en == 0);
    }

    return HI_SUCCESS;
}
hi_u32 vpss_cccl_config_check(vpss_reg_type *reg, hi_u32 data, vpss_cfg_info *cfg_info, hi_u32 ext_info)
{
    /******************* declairation  *********************/
    hi_u32 cc_pr0_en = reg->vpss_cc_pr0_ctrl.bits.cc_pr0_en;
    hi_u32 cc_pr4_en = reg->vpss_cc_pr4_ctrl.bits.cc_pr4_en;
    hi_u32 cc_pr8_en = reg->vpss_cc_pr8_ctrl.bits.cc_pr8_en;
    hi_u32 cc_rccnt_en = reg->vpss_cc_rccnt_ctrl.bits.cc_rccnt_en;
    hi_u32 cc_rycnt_en = reg->vpss_cc_rycnt_ctrl.bits.cc_rycnt_en;
    hi_u32 cc_rfr_en = reg->vpss_cc_rfr_ctrl.bits.cc_rfr_en;
    hi_u32 cc_wccnt_en = reg->vpss_cc_wccnt_ctrl.bits.cc_wccnt_en;
    hi_u32 cc_wycnt_en = reg->vpss_cc_wycnt_ctrl.bits.cc_wycnt_en;

    /*******************   VPSS_ASSERT_RETion   *********************/
    if (reg->vpss_ctrl.bits.cccl_en == 1) {
        vpss_assert_ret(cfg_info->wth <= 960); /* 960:para */
        vpss_assert_ret(cc_pr0_en == 1);
        vpss_assert_ret(cc_pr4_en == 1);
        vpss_assert_ret(cc_pr8_en == 1);
        vpss_assert_ret(cc_rccnt_en == 1);
        vpss_assert_ret(cc_rycnt_en == 1);
        vpss_assert_ret(cc_rfr_en == 1);
        vpss_assert_ret(cc_wccnt_en == 1);
        vpss_assert_ret(cc_wycnt_en == 1);
    } else {
        vpss_assert_ret(cc_pr0_en == 0);
        vpss_assert_ret(cc_pr4_en == 0);
        vpss_assert_ret(cc_pr8_en == 0);
        vpss_assert_ret(cc_rccnt_en == 0);
        vpss_assert_ret(cc_rycnt_en == 0);
        vpss_assert_ret(cc_rfr_en == 0);
        vpss_assert_ret(cc_wccnt_en == 0);
        vpss_assert_ret(cc_wycnt_en == 0);
    }

    return HI_SUCCESS;
}

hi_u32 vpss_get_cfg_info(vpss_reg_type *reg, hi_bool enable, vpss_cfg_info *cfg_info)
{
    hi_u32 cf_c_convert = reg->vpss_chn_cfg_ctrl.bits.cf_c_convert;

    if (enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    /******************* declairation  *********************/
    /******************* get reg value *********************/
    cfg_info->format = reg->vpss_cf_ctrl.bits.cf_format;
    cfg_info->prio = 0;
    cfg_info->img2d3d_mode = 0;
    cfg_info->pro = reg->vpss_cf_ctrl.bits.cf_lm_rmode == 1 ? 1 : 0;
    cfg_info->even = 0;
    cfg_info->bfield = reg->vpss_cf_ctrl.bits.cf_lm_rmode % 2; /* 2:para */
    cfg_info->pre_mult_en = 0;
    cfg_info->comp_valid = 0xffffffff;
    cfg_info->wth = reg->vpss_cf_size.bits.cf_width + 1;
    cfg_info->hgt = reg->vpss_cf_size.bits.cf_height + 1;
    /*******************   VPSS_ASSERT_RETion   *********************/
    vpss_assert_ret(cfg_info->format <= XDP_PROC_FMT_SP_400);
    vpss_assert_ret(cfg_info->format != XDP_PROC_FMT_SP_444);

    if (cfg_info->format == XDP_PROC_FMT_SP_400) {
        cfg_info->format = XDP_PROC_FMT_SP_420;
    }

    if (cf_c_convert == 1) {
        vpss_assert_ret(cfg_info->format == XDP_PROC_FMT_SP_420);
        cfg_info->format = XDP_PROC_FMT_SP_422;
    }

    if (cfg_info->pro == 0) {
        cfg_info->hgt /= 2; /* 2:para */
    }

    return HI_SUCCESS;
}

hi_u32 vpss_sys_apb_config_check(vpss_reg_type *reg, hi_bool enable, vpss_cfg_info *cfg_info)
{
    hi_u32 scb_ttbr = reg->vpss0_smmu_scb_ttbr.bits.scb_ttbr;
    hi_u32 cb_ttbr = reg->vpss0_smmu_cb_ttbr.bits.cb_ttbr;
    hi_u32 err_s_rd_addr = reg->vpss0_smmu_err_rdaddr.bits.err_s_rd_addr;
    hi_u32 err_s_wr_addr = reg->vpss0_smmu_err_wraddr.bits.err_s_wr_addr;
    hi_u32 err_ns_rd_addr = reg->vpss0_smmu_err_rdaddr_ns.bits.err_ns_rd_addr;
    hi_u32 err_ns_wr_addr = reg->vpss0_smmu_err_wraddr_ns.bits.err_ns_wr_addr;

    if (enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    // MMU
    vpss_assert_ret((scb_ttbr & 0x3f) == 0);
    vpss_assert_ret((cb_ttbr & 0x3f) == 0);
    vpss_assert_ret((err_s_rd_addr & 0xff) == 0);
    vpss_assert_ret((err_s_wr_addr & 0xff) == 0);
    vpss_assert_ret((err_ns_rd_addr & 0xff) == 0);
    vpss_assert_ret((err_ns_wr_addr & 0xff) == 0);
    vpss_assert_ret(reg->vpss_mst_ctrl.bits.split_mode <= 4); /* 4:para */
    vpss_assert_ret(reg->vpss_mst_outstanding.bits.mstr0_woutstanding <= 7); /* 7:para */
    return HI_SUCCESS;
}

hi_u32 vpss_sys_config_check(vpss_reg_type *reg, hi_bool enable, vpss_cfg_info *cfg_info)
{
    if (enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    vpss_assert_ret((reg->vpss_chn_cfg_ctrl.bits.prot == 0) || (reg->vpss_chn_cfg_ctrl.bits.prot == 2)); /* 2:para */
    vpss_assert_ret(reg->vpss_chn_cfg_ctrl.bits.max_req_len <= 1);
    vpss_assert_ret(reg->vpss_chn_cfg_ctrl.bits.max_req_num <= 2); /* 2:para */

    if (reg->vpss_chn_cfg_ctrl.bits.dma_en == 1 || reg->vpss_ctrl.bits.rotate_en == 1) {
        vpss_assert_ret(reg->vpss_ctrl.bits.vpss_3drs_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.rgme_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.igbm_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.ifmd_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.mcdi_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.dei_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.mcnr_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.tnr_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.cccl_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.snr_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.hsp_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.vc1_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.scd_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.hcti_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.lbd_en == 0);
        vpss_assert_ret(reg->vpss_ctrl.bits.hfr_en == 0);
    }

    if (reg->vpss_ctrl.bits.rotate_en == 1) {
        vpss_assert_ret(reg->vpss_chn_cfg_ctrl.bits.dma_en == 0);
        vpss_assert_ret(reg->vpss_chn_cfg_ctrl.bits.img_pro_mode != 0);
        vpss_assert_ret(reg->vpss_chn_cfg_ctrl.bits.img_pro_mode <= 2); /* 2:para */
    } else {
        vpss_assert_ret(reg->vpss_chn_cfg_ctrl.bits.img_pro_mode == 0);
    }

    vpss_assert_ret(reg->vpss_cf_vid_read_ctrl.u32 == 0x00120000);  /* 0x00120000 : reg default para */
    vpss_assert_ret(reg->vpss_cf_vid_mac_ctrl.u32 == 0x80000000);  /* 0x80000000 : reg default para */
    vpss_assert_ret(reg->vpss_cf_vid_mute_bk.u32 == 0x00080200); /* 0x00080200 : reg default para */
    vpss_assert_ret(reg->vpss_cf_vid_smmu_bypass.u32 == 0x0000000F); /* 0x0000000F : reg default para */
    vpss_assert_ret(reg->vpss_cf_vid_tunl_ctrl.u32 == 0x00010000); /* 0x00010000 : reg default para */

    return HI_SUCCESS;
}

hi_u32 vpss_mac_config_check(vpss_reg_type *reg, hi_bool enable, vpss_cfg_info *cfg_info)
{
    hi_u32 cfg_chk_scan_cnt;
    hi_u64 scan_pnext_addr;
    hi_u32 addr_cnt;
    vpss_reg_type *reg_scan;
    hi_u32 cfg_chn_low_addr[LAST_CHN_ADDR] = { 0 };
    hi_u32 di_cf_tunl_en;
    hi_u32 snr_rmad_tunl_en;
    hi_u32 nr_cfmv_tunl_en;
    hi_u32 me_p2rgmv_tunl_en;
    hi_u32 chk_sum_en = reg->vpss_chk_sum_ctrl.bits.chk_sum_en;
    hi_u32 cf_en = reg->vpss_cf_ctrl.bits.cf_en;
    hi_u32 out0_en = reg->vpss_out0_ctrl.bits.out0_en;
    hi_u32 nr_rfrh_en = reg->vpss_nr_rfrh_ctrl.bits.nr_rfrh_en;
    hi_u32 nr_rfrhv_en = reg->vpss_nr_rfrhv_ctrl.bits.nr_rfrhv_en;

    if (enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    di_cf_tunl_en = reg->vpss_di_cf_ctrl.bits.di_cf_tunl_en;
    snr_rmad_tunl_en = reg->vpss_snr_rmad_ctrl.bits.snr_rmad_tunl_en;
    nr_cfmv_tunl_en = reg->vpss_nr_cfmv_ctrl.bits.nr_cfmv_tunl_en;
    me_p2rgmv_tunl_en = reg->vpss_me_p2_rgmv_ctrl.bits.me_p2rgmv_tunl_en;
    cfg_chn_low_addr[VPSS_STT_R_ADDR] = reg->vpss_stt_r_addr_low.bits.stt_r_addr_l;
    cfg_chn_low_addr[VPSS_STT_W_ADDR] = reg->vpss_stt_w_addr_low.bits.stt_w_addr_l;
    cfg_chn_low_addr[VPSS_CHK_SUM_ADDR] = reg->vpss_chk_sum_addr_low.bits.chk_sum_addr_l;
    cfg_chn_low_addr[VPSS_CF_RTUNL_ADDR] = reg->vpss_cf_rtunl_addr_low.bits.cf_rtunl_addr_l;
    cfg_chn_low_addr[VPSS_OUT0_WTUNL_ADDR] = reg->vpss_out0_wtunl_addr_low.bits.out0_wtunl_addr_l;
    cfg_chn_low_addr[VPSS_CFY_ADDR] = reg->vpss_cfy_addr_low.bits.cfy_addr_l;
    cfg_chn_low_addr[VPSS_CFC_ADDR] = reg->vpss_cfc_addr_low.bits.cfc_addr_l;
    cfg_chn_low_addr[VPSS_CFCR_ADDR] = reg->vpss_cfcr_addr_low.bits.cfcr_addr_l;
    cfg_chn_low_addr[VPSS_DI_CFY_ADDR] = reg->vpss_di_cfy_addr_low.bits.di_cfy_addr_l;
    cfg_chn_low_addr[VPSS_DI_CFC_ADDR] = reg->vpss_di_cfc_addr_low.bits.di_cfc_addr_l;
    cfg_chn_low_addr[VPSS_DI_CFCR_ADDR] = reg->vpss_di_cfcr_addr_low.bits.di_cfcr_addr_l;
    cfg_chn_low_addr[VPSS_DI_P1Y_ADDR] = reg->vpss_di_p1_y_addr_low.bits.di_p1y_addr_l;
    cfg_chn_low_addr[VPSS_DI_P1C_ADDR] = reg->vpss_di_p1_c_addr_low.bits.di_p1c_addr_l;
    cfg_chn_low_addr[VPSS_DI_P2Y_ADDR] = reg->vpss_di_p2_y_addr_low.bits.di_p2y_addr_l;
    cfg_chn_low_addr[VPSS_DI_P2C_ADDR] = reg->vpss_di_p2_c_addr_low.bits.di_p2c_addr_l;
    cfg_chn_low_addr[VPSS_DI_P3Y_ADDR] = reg->vpss_di_p3_y_addr_low.bits.di_p3y_addr_l;
    cfg_chn_low_addr[VPSS_DI_P3C_ADDR] = reg->vpss_di_p3_c_addr_low.bits.di_p3c_addr_l;
    cfg_chn_low_addr[VPSS_DI_P3IY_ADDR] = reg->vpss_di_p3_iy_addr_low.bits.di_p3iy_addr_l;
    cfg_chn_low_addr[VPSS_DI_P3IC_ADDR] = reg->vpss_di_p3_ic_addr_low.bits.di_p3ic_addr_l;
    cfg_chn_low_addr[VPSS_CC_PR0Y_ADDR] = reg->vpss_cc_pr0_y_addr_low.bits.cc_pr0y_addr_l;
    cfg_chn_low_addr[VPSS_CC_PR0C_ADDR] = reg->vpss_cc_pr0_c_addr_low.bits.cc_pr0c_addr_l;
    cfg_chn_low_addr[VPSS_CC_PR4Y_ADDR] = reg->vpss_cc_pr4_y_addr_low.bits.cc_pr4y_addr_l;
    cfg_chn_low_addr[VPSS_CC_PR4C_ADDR] = reg->vpss_cc_pr4_c_addr_low.bits.cc_pr4c_addr_l;
    cfg_chn_low_addr[VPSS_CC_PR8Y_ADDR] = reg->vpss_cc_pr8_y_addr_low.bits.cc_pr8y_addr_l;
    cfg_chn_low_addr[VPSS_CC_PR8C_ADDR] = reg->vpss_cc_pr8_c_addr_low.bits.cc_pr8c_addr_l;
    cfg_chn_low_addr[VPSS_NR_REFY_ADDR] = reg->vpss_nr_refy_addr_low.bits.nr_refy_addr_l;
    cfg_chn_low_addr[VPSS_NR_REFC_ADDR] = reg->vpss_nr_refc_addr_low.bits.nr_refc_addr_l;
    cfg_chn_low_addr[VPSS_DI_RSADY_ADDR] = reg->vpss_di_rsady_addr_low.bits.di_rsady_addr_l;
    cfg_chn_low_addr[VPSS_DI_RSADC_ADDR] = reg->vpss_di_rsadc_addr_low.bits.di_rsadc_addr_l;
    cfg_chn_low_addr[VPSS_DI_RHISM_ADDR] = reg->vpss_di_rhism_addr_low.bits.di_rhism_addr_l;
    cfg_chn_low_addr[VPSS_DI_P1MV_ADDR] = reg->vpss_di_p1_mv_addr_low.bits.di_p1mv_addr_l;
    cfg_chn_low_addr[VPSS_DI_P2MV_ADDR] = reg->vpss_di_p2_mv_addr_low.bits.di_p2mv_addr_l;
    cfg_chn_low_addr[VPSS_DI_P3MV_ADDR] = reg->vpss_di_p3_mv_addr_low.bits.di_p3mv_addr_l;
    cfg_chn_low_addr[VPSS_CC_RCCNT_ADDR] = reg->vpss_cc_rccnt_addr_low.bits.cc_rccnt_addr_l;
    cfg_chn_low_addr[VPSS_CC_RYCNT_ADDR] = reg->vpss_cc_rycnt_addr_low.bits.cc_rycnt_addr_l;
    cfg_chn_low_addr[VPSS_NR_RMAD_ADDR] = reg->vpss_nr_rmad_addr_low.bits.nr_rmad_addr_l;
    cfg_chn_low_addr[VPSS_SNR_RMAD_ADDR] = reg->vpss_snr_rmad_addr_low.bits.snr_rmad_addr_l;
    cfg_chn_low_addr[VPSS_NR_CFMV_ADDR] = reg->vpss_nr_cfmv_addr_low.bits.nr_cfmv_addr_l;
    cfg_chn_low_addr[VPSS_NR_P1RGMV_ADDR] = reg->vpss_nr_p1_rgmv_addr_low.bits.nr_p1rgmv_addr_l;
    cfg_chn_low_addr[VPSS_NR_P2RGMV_ADDR] = reg->vpss_nr_p2_rgmv_addr_low.bits.nr_p2rgmv_addr_l;
    cfg_chn_low_addr[VPSS_NR_RCNT_ADDR] = reg->vpss_nr_rcnt_addr_low.bits.nr_rcnt_addr_l;
    cfg_chn_low_addr[VPSS_RG_RPRJH_ADDR] = reg->vpss_rg_rprjh_addr_low.bits.rg_rprjh_addr_l;
    cfg_chn_low_addr[VPSS_RG_RPRJV_ADDR] = reg->vpss_rg_rprjv_addr_low.bits.rg_rprjv_addr_l;
    cfg_chn_low_addr[VPSS_RG_P1RGMV_ADDR] = reg->vpss_rg_p1_rgmv_addr_low.bits.rg_p1rgmv_addr_l;
    cfg_chn_low_addr[VPSS_RG_P2RGMV_ADDR] = reg->vpss_rg_p2_rgmv_addr_low.bits.rg_p2rgmv_addr_l;
    cfg_chn_low_addr[VPSS_OUT0Y_ADDR] = reg->vpss_out0_y_addr_low.bits.out0y_addr_l;
    cfg_chn_low_addr[VPSS_OUT0C_ADDR] = reg->vpss_out0_c_addr_low.bits.out0c_addr_l;
    cfg_chn_low_addr[VPSS_OUT3Y_ADDR] = reg->vpss_out3_y_addr_low.bits.out3y_addr_l;
    cfg_chn_low_addr[VPSS_NR_RFRY_ADDR] = reg->vpss_nr_rfry_addr_low.bits.nr_rfry_addr_l;
    cfg_chn_low_addr[VPSS_NR_RFRC_ADDR] = reg->vpss_nr_rfrc_addr_low.bits.nr_rfrc_addr_l;
    cfg_chn_low_addr[VPSS_NR_RFRHY_ADDR] = reg->vpss_nr_rfrhy_addr_low.bits.nr_rfrhy_addr_l;
    cfg_chn_low_addr[VPSS_NR_RFRHVY_ADDR] = reg->vpss_nr_rfrhvy_addr_low.bits.nr_rfrhvy_addr_l;
    cfg_chn_low_addr[VPSS_CC_RFRY_ADDR] = reg->vpss_cc_rfry_addr_low.bits.cc_rfry_addr_l;
    cfg_chn_low_addr[VPSS_CC_RFRC_ADDR] = reg->vpss_cc_rfrc_addr_low.bits.cc_rfrc_addr_l;
    cfg_chn_low_addr[VPSS_DI_RFRY_ADDR] = reg->vpss_di_rfry_addr_low.bits.di_rfry_addr_l;
    cfg_chn_low_addr[VPSS_DI_RFRC_ADDR] = reg->vpss_di_rfrc_addr_low.bits.di_rfrc_addr_l;
    cfg_chn_low_addr[VPSS_NR_WMAD_ADDR] = reg->vpss_nr_wmad_addr_low.bits.nr_wmad_addr_l;
    cfg_chn_low_addr[VPSS_NR_WCNT_ADDR] = reg->vpss_nr_wcnt_addr_low.bits.nr_wcnt_addr_l;
    cfg_chn_low_addr[VPSS_CC_WCCNT_ADDR] = reg->vpss_cc_wccnt_addr_low.bits.cc_wccnt_addr_l;
    cfg_chn_low_addr[VPSS_CC_WYCNT_ADDR] = reg->vpss_cc_wycnt_addr_low.bits.cc_wycnt_addr_l;
    cfg_chn_low_addr[VPSS_DI_WSADY_ADDR] = reg->vpss_di_wsady_addr_low.bits.di_wsady_addr_l;
    cfg_chn_low_addr[VPSS_DI_WSADC_ADDR] = reg->vpss_di_wsadc_addr_low.bits.di_wsadc_addr_l;
    cfg_chn_low_addr[VPSS_DI_WHISM_ADDR] = reg->vpss_di_whism_addr_low.bits.di_whism_addr_l;
    cfg_chn_low_addr[VPSS_RG_WPRJH_ADDR] = reg->vpss_rg_wprjh_addr_low.bits.rg_wprjh_addr_l;
    cfg_chn_low_addr[VPSS_RG_WPRJV_ADDR] = reg->vpss_rg_wprjv_addr_low.bits.rg_wprjv_addr_l;
    cfg_chn_low_addr[VPSS_RG_CFRGMV_ADDR] = reg->vpss_rg_cfrgmv_addr_low.bits.rg_cfrgmv_addr_l;
    vpss_assert_ret(chk_sum_en == 0);
    vpss_assert_ret(cf_en == 1);
    vpss_assert_ret(out0_en == 1);

    vpss_assert_ret((cfg_chn_low_addr[VPSS_CF_RTUNL_ADDR] & 0x1f) == 0);
    vpss_assert_ret((cfg_chn_low_addr[VPSS_OUT0_WTUNL_ADDR] & 0x1f) == 0);

    if (di_cf_tunl_en == 1) {
        vpss_assert_ret(cfg_chn_low_addr[VPSS_NR_RFRY_ADDR] == cfg_chn_low_addr[VPSS_DI_CFY_ADDR]);
        vpss_assert_ret(cfg_chn_low_addr[VPSS_NR_RFRC_ADDR] == cfg_chn_low_addr[VPSS_DI_CFC_ADDR]);
    }

    if (snr_rmad_tunl_en == 1) {
        vpss_assert_ret(cfg_chn_low_addr[VPSS_SNR_RMAD_ADDR] == cfg_chn_low_addr[VPSS_NR_WMAD_ADDR]);
    }

    scan_pnext_addr = ((hi_u64)reg->scan_start_addr_h.u32 << 20) + reg->scan_start_addr_l.u32; /* 64,32,20,32:para */
    cfg_chk_scan_cnt = 0;
    vpss_assert_ret(scan_pnext_addr != 0);

    while (scan_pnext_addr != 0) {
        reg_scan = (vpss_reg_type *)((hi_u32 *)reg + VPSS_MAX_REGS_NUM_IN_ONE_SCAN * cfg_chk_scan_cnt);

        if (reg_scan->scan_ctrl.bits.me_en == 1) {
            vpss_assert_ret(nr_rfrh_en == 1);
            vpss_assert_ret(nr_rfrhv_en == 1);
        }

        cfg_chn_low_addr[VPSS_ME_CFY_ADDR] = reg_scan->vpss_me_cfy_addr_low.bits.me_cfy_addr_l;
        cfg_chn_low_addr[VPSS_ME_REFY_ADDR] = reg_scan->vpss_me_refy_addr_low.bits.me_refy_addr_l;
        cfg_chn_low_addr[VPSS_ME_P1MV_ADDR] = reg_scan->vpss_me_p1_mv_addr_low.bits.me_p1mv_addr_l;
        cfg_chn_low_addr[VPSS_ME_PRMV_ADDR] = reg_scan->vpss_me_prmv_addr_low.bits.me_prmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_P1GMV_ADDR] = reg_scan->vpss_me_p1_gmv_addr_low.bits.me_p1gmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_PRGMV_ADDR] = reg_scan->vpss_me_prgmv_addr_low.bits.me_prgmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_P1RGMV_ADDR] = reg_scan->vpss_me_p1_rgmv_addr_low.bits.me_p1rgmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_P2RGMV_ADDR] = reg_scan->vpss_me_p2_rgmv_addr_low.bits.me_p2rgmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_CFMV_ADDR] = reg_scan->vpss_me_cfmv_addr_low.bits.me_cfmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_CFGMV_ADDR] = reg_scan->vpss_me_cfgmv_addr_low.bits.me_cfgmv_addr_l;
        cfg_chn_low_addr[VPSS_ME_STT_W_ADDR] = reg_scan->vpss_me_stt_w_addr_low.bits.me_stt_w_addr_l;

        for (addr_cnt = VPSS_ME_CFY_ADDR; addr_cnt < LAST_CHN_ADDR; addr_cnt++) {
#ifndef __VPSS_UT__
            vpss_assert_ret((cfg_chn_low_addr[addr_cnt] & 0xf) == 0);
#endif
        }

        if (cfg_chk_scan_cnt >= 2) { /* 2:para */
            vpss_assert_ret(cfg_chn_low_addr[VPSS_ME_PRMV_ADDR] == cfg_chn_low_addr[VPSS_ME_CFMV_ADDR]);
            vpss_assert_ret(cfg_chn_low_addr[VPSS_ME_PRGMV_ADDR] == cfg_chn_low_addr[VPSS_ME_CFGMV_ADDR]);
        }

        if ((nr_cfmv_tunl_en == 1) && (cfg_chk_scan_cnt == 0)) {
            vpss_assert_ret(cfg_chn_low_addr[VPSS_NR_CFMV_ADDR] == cfg_chn_low_addr[VPSS_ME_CFMV_ADDR]);
        }

        if (me_p2rgmv_tunl_en == 1) {
            vpss_assert_ret(cfg_chn_low_addr[VPSS_ME_P2RGMV_ADDR] == cfg_chn_low_addr[VPSS_RG_CFRGMV_ADDR]);
        }
        /* 20:para */
        scan_pnext_addr = ((hi_u64)reg_scan->scan_pnext_addr_h.u32 << 20) + reg_scan->scan_pnext_addr_l.u32;
        cfg_chk_scan_cnt += 1;
    }

    return HI_SUCCESS;
}

hi_u32 vpss_ip_config_check(vpss_reg_type *reg, hi_bool enable, vpss_cfg_info *cfg_info)
{
    hi_u32 ext_info = 0;
    hi_u32 cfg_chk_scan_cnt;
    hi_u64 scan_pnext_addr;
    vpss_reg_type *reg_scan = HI_NULL;
    vpss_reg_type *reg_scan_pre = HI_NULL;
    vpss_cfg_info cfg_info_out0 = { 0 };
    vpss_cfg_info cfg_info_out3 = { 0 };
    vpss_cfg_info cfg_info_rgme = { 0 };
    vpss_cfg_info cfg_info_me = { 0 };
    vpss_cfg_info cfg_info_me_cf = { 0 };
    vpss_cfg_info cfg_info_me_p2 = { 0 };

    if (enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    if (reg->vpss_ctrl.bits.hfr_en == 1) {
        vpss_assert_ret(reg->vpss_ctrl.bits.cccl_en == 0);
    }

    vpss_hfr_config_check(reg, 0, cfg_info, ext_info);
    vpss_cccl_config_check(reg, 0, cfg_info, ext_info);
    vpss_vc1_config_check(reg, XDP_VC1_ID_CF, cfg_info, ext_info);
    vpss_scd_config_check(reg, 0, cfg_info, ext_info);

    if ((reg->vpss_ctrl.bits.dei_en == 1) || (cfg_info->wth < 160) || (cfg_info->hgt < 64)) { /* 1,160,64:para */
        vpss_assert_ret(reg->vpss_nr_ref_ctrl.bits.nr_ref_dcmp_mode == 0);
        vpss_assert_ret(reg->vpss_nr_rfr_ctrl.bits.nr_rfr_cmp_mode == 0);
        vpss_assert_ret(reg->vpss_fcmp_nr_cfy_y_glb_info.bits.nr_cfy_cmp_en == 0);
    }

    vpss_assert_ret(reg->vpss_fcmp_nr_cfy_y_glb_info.bits.nr_cfy_cmp_en ==
                    reg->vpss_fcmp_nr_cfc_c_glb_info.bits.nr_cfc_cmp_en);
    vpss_assert_ret(reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_dcmp_en ==
                    reg->vpss_fdcmp_nr_refc_fhd_c_glb_info.bits.nr_refc_dcmp_en);

    if (reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_dcmp_en == 0) {
        vpss_assert_ret(reg->vpss_nr_ref_ctrl.bits.nr_ref_dcmp_mode == 0);
    }

    if (reg->vpss_fdcmp_nr_refy_fhd_y_glb_info.bits.nr_refy_dcmp_en == 1) {
        vpss_assert_ret(reg->vpss_nr_ref_ctrl.bits.nr_ref_dcmp_mode == 3); /* 3:para */
    }

    vpss_assert_ret(reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_cmp_en ==
                    reg->vpss_fcmp_nr_rfrc_fhd_c_glb_info.bits.nr_rfrc_cmp_en);

    if (reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_cmp_en == 0) {
        vpss_assert_ret(reg->vpss_nr_rfr_ctrl.bits.nr_rfr_cmp_mode == 0);
    }

    if (reg->vpss_fcmp_nr_rfry_fhd_y_glb_info.bits.nr_rfry_cmp_en == 1) {
        vpss_assert_ret(reg->vpss_nr_rfr_ctrl.bits.nr_rfr_cmp_mode == 3); /* 3:para */
    }

    vpss_tnr_config_check(reg, 0, cfg_info, ext_info);
    memcpy(&cfg_info_rgme, cfg_info, sizeof(vpss_cfg_info));
    vpss_dei_config_check(reg, 0, cfg_info, ext_info);
    vpss_snr_config_check(reg, 0, cfg_info, ext_info);
    vpss_hsp_config_check(reg, 0, cfg_info, ext_info);
    vpss_hcti_config_check(reg, 0, cfg_info, ext_info);
    vpss_vcti_config_check(reg, 0, cfg_info, ext_info);
    memcpy(&cfg_info_out0, cfg_info, sizeof(vpss_cfg_info));
    memcpy(&cfg_info_out3, cfg_info, sizeof(vpss_cfg_info));
    vpss_out3_pzme_config_check(reg, 0, &cfg_info_out3, ext_info);
    vpss_out0_pzme_config_check(reg, 0, &cfg_info_out0, ext_info);
    /* 1,0,160,0,64:para */
    if ((reg->vpss_hipp_vcti_ctrl.bits.vcti_vcti_en == 1) || (cfg_info_out0.wth < 160) || (cfg_info_out0.hgt < 64)) {
        vpss_assert_ret(reg->vpss_out0_ctrl.bits.out0_cmp_mode == 0);
    }

#ifdef DPT
    if (reg->vpss_fcmp_out0_4_k_glb_info.bits.out0_cmp_en == 0) {
        vpss_assert_ret(reg->vpss_out0_ctrl.bits.out0_cmp_mode == 0);
    }

    if (reg->vpss_fcmp_out0_4_k_glb_info.bits.out0_cmp_en == 1) {
        vpss_assert_ret(reg->vpss_out0_ctrl.bits.out0_cmp_mode == 3); /* 3:para */
    }
#endif
    vpss_assert_ret(reg->vpss_ctrl.bits.meds_en == reg->vpss_wr0_hds_zme_hsp.bits.vpss_wr0_hds_lhfir_en);
    vpss_hzme_config_check(reg, XDP_ZME_ID_VPSS_WR0_HDS_HZME, &cfg_info_rgme, ext_info);
    vpss_rgme_config_check(reg, 0, &cfg_info_rgme, ext_info);
    memcpy(&cfg_info_me, &cfg_info_rgme, sizeof(vpss_cfg_info));
    vpss_hzme_config_check(reg, XDP_ZME_ID_VPSS_WR1_HVDS_HZME, &cfg_info_rgme, ext_info);
    vpss_vzme_config_check(reg, XDP_ZME_ID_VPSS_WR1_HVDS_VZME, &cfg_info_rgme, ext_info);
    scan_pnext_addr = ((hi_u64)reg->scan_start_addr_h.u32 << 20) + reg->scan_start_addr_l.u32; /* 64,32,20,32:para */
    cfg_chk_scan_cnt = 0;

    while (scan_pnext_addr != 0) {
        reg_scan = (vpss_reg_type *)((hi_u32 *)reg + VPSS_MAX_REGS_NUM_IN_ONE_SCAN * cfg_chk_scan_cnt);

        if (cfg_chk_scan_cnt > 0) {
            reg_scan_pre = (vpss_reg_type *)((hi_u32 *)reg + VPSS_MAX_REGS_NUM_IN_ONE_SCAN * (cfg_chk_scan_cnt - 1));
        }

        cfg_info_me_cf.format = reg_scan->vpss_me_cf_ctrl.bits.me_cf_format;
        cfg_info_me_cf.prio = 0;
        cfg_info_me_cf.img2d3d_mode = 0;
        cfg_info_me_cf.pro = reg_scan->vpss_me_cf_ctrl.bits.me_cf_lm_rmode == 1 ? 1 : 0;
        cfg_info_me_cf.even = 0;
        cfg_info_me_cf.bfield = reg_scan->vpss_me_cf_ctrl.bits.me_cf_lm_rmode % 2; /* 2:para */
        cfg_info_me_cf.pre_mult_en = 0;
        cfg_info_me_cf.comp_valid = 0xffffffff;
        cfg_info_me_cf.wth = reg_scan->vpss_me_cf_size.bits.me_cf_width + 1;
        cfg_info_me_cf.hgt = reg_scan->vpss_me_cf_size.bits.me_cf_height + 1;

        if (cfg_info_me_cf.format == XDP_PROC_FMT_SP_400) {
            cfg_info_me_cf.format = XDP_PROC_FMT_SP_420;
        }

        if (cfg_info_me_cf.pro == 0) {
            cfg_info_me_cf.hgt /= 2; /* 2:para */
        }

        cfg_info_me_p2.format = reg_scan->vpss_me_ref_ctrl.bits.me_ref_format;
        cfg_info_me_p2.prio = 0;
        cfg_info_me_p2.img2d3d_mode = 0;
        cfg_info_me_p2.pro = cfg_info_me_cf.pro;
        cfg_info_me_p2.even = 0;
        cfg_info_me_p2.bfield = cfg_info_me_cf.bfield;
        cfg_info_me_p2.pre_mult_en = 0;
        cfg_info_me_p2.comp_valid = 0xffffffff;
        cfg_info_me_p2.wth = reg_scan->vpss_me_ref_size.bits.me_ref_width + 1;
        cfg_info_me_p2.hgt = reg_scan->vpss_me_ref_size.bits.me_ref_height + 1;

        if (cfg_info_me_p2.format == XDP_PROC_FMT_SP_400) {
            cfg_info_me_p2.format = XDP_PROC_FMT_SP_420;
        }

        if (cfg_info_me_p2.pro == 0) {
            cfg_info_me_p2.hgt /= 2; /* 2,2:para */
        }

        vpss_vc1_config_check(reg_scan, XDP_VC1_ID_ME_CF, &cfg_info_me_cf, ext_info);
        vpss_hzme_config_check(reg_scan, XDP_ZME_ID_VPSS_ME_CF_HDS_HZME, &cfg_info_me_cf, ext_info);
        vpss_hzme_config_check(reg_scan, XDP_ZME_ID_VPSS_ME_P2_HDS_HZME, &cfg_info_me_p2, ext_info);
        vpss_me_config_check(reg_scan, 0, &cfg_info_me, ext_info);

        if (cfg_chk_scan_cnt > 0) {
            vpss_scan_config_check(reg_scan_pre, reg_scan);
        }
        /* 20:para */
        scan_pnext_addr = ((hi_u64)reg_scan->scan_pnext_addr_h.u32 << 20) + reg_scan->scan_pnext_addr_l.u32;
        cfg_chk_scan_cnt += 1;
    }

    return HI_SUCCESS;
}

hi_u32 vpss_hal_logic_cfg_check(vpss_reg_type *reg)
{
    vpss_cfg_info cfg_info;
    vpss_get_cfg_info(reg, HI_TRUE, &cfg_info);
    vpss_sys_apb_config_check(reg, HI_TRUE, &cfg_info);
    vpss_sys_config_check(reg, HI_TRUE, &cfg_info);
    vpss_mac_config_check(reg, HI_TRUE, &cfg_info);
    vpss_ip_config_check(reg, HI_TRUE, &cfg_info);
    vpss_hal_crop_config_check(reg, 0, &cfg_info);

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



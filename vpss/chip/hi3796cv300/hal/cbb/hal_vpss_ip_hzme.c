/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss hal_vpss_ip_hzme
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "vpss_comm.h"
#include "hal_vpss_ip_hzme.h"
#include "hal_vpss_zme_inst_para.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_REG_BASE_HZME_ADDR      0x8900 /* VPSS_REG_WR0_HVDS_HZME_ADDR */
#define VPSS_REG_WR1_HVDS_HZME_ADDR  0x8A00
#define VPSS_REG_ME_CF_HDS_HZME_ADDR 0xE900
#define VPSS_REG_ME_P2_HDS_HZME_ADDR 0xEA00
#define VPSS_REG_CH_HZME_ADDR        0xB100
#define VPSS_REG_OUT2_CH_HZME_ADDR   0xB300 /* es no such reg */
#define VPSS_REG_HDR_IFIR_ADDR       0xCC00
#define VPSS_REG_HDR_DFIR_ADDR       0xCD00
#define VPSS_REG_NPU_HZME_ADDR       0xD100 /* npu zme */
#define VPSS_REG_BASE_VZME_ADDR      0x8B00 /* VPSS_REG_WR1_HVDS_VZME_ADDR */
#define VPSS_REG_CH_VZME_ADDR        0xB200
#define VPSS_REG_OUT2_VZME_ADDR      0xB400 /* es no such reg */
#define VPSS_REG_HDR_VDS_ADDR        0xCE00 /* es no such reg */
#define VPSS_REG_NPU_VZME_ADDR       0xD200

hi_void vpss_func_set_hzme_mode(vpss_reg_type *vpss_regs, hi_u32 id, vpss_hzme_mode hzme_mode,
                                vpss_hzme_cfg *cfg, vpss_hzme_pq *pq_cfg)
{
    hi_u32 offset;
    xdp_zme_inst_para zme_inst_para = { 0 };
    // filed declare
    hi_u32 zme_hprec = VPSS_HZME_PRECISION;
    hi_u32 out_fmt;
    hi_u32 ck_gt_en;
    hi_u32 out_width;
    hi_u32 lhfir_en;
    hi_u32 chfir_en;
    hi_u32 lhmid_en;
    hi_u32 chmid_en;
    hi_u32 non_lnr_en;
    hi_u32 lhfir_mode;
    hi_u32 chfir_mode;
    hi_u32 hfir_order = 0;
    hi_u32 hratio;
    hi_u32 lhfir_offset;
    hi_u32 chfir_offset;
    hi_s32 zone0_delta = 0;
    hi_s32 zone2_delta = 0;
    hi_u32 zone1_end = 0;
    hi_u32 zone0_end = 0;
    hi_u32 hl_shootctrl_en;
    hi_u32 hl_shootctrl_mode;
    hi_u32 hl_flatdect_mode;
    hi_u32 hl_coringadj_en;
    hi_u32 hl_gain;
    hi_u32 hl_coring;
    hi_u32 hc_shootctrl_en;
    hi_u32 hc_shootctrl_mode;
    hi_u32 hc_flatdect_mode;
    hi_u32 hc_coringadj_en;
    hi_u32 hc_gain;
    hi_u32 hc_coring;
    non_lnr_en = pq_cfg->nonlinear_scl_en;
    out_width = cfg->out_width - 1;
    out_fmt = cfg->out_fmt;
    ck_gt_en = cfg->ck_gt_en;
    lhfir_en = cfg->lh_fir_en;
    lhfir_mode = cfg->lh_fir_mode;
    chfir_en = cfg->ch_fir_en;
    chfir_mode = cfg->ch_fir_mode;
    hl_shootctrl_en = pq_cfg->lh_stc2nd_en;
    hc_shootctrl_en = pq_cfg->ch_stc2nd_en;
    lhmid_en = pq_cfg->lh_med_en;
    chmid_en = pq_cfg->ch_med_en;

    if (cfg->out_width == 0) {
        vpss_error("zme out_w is zero ! \n");
        return;
    }

    hratio = osal_div64_u64((cfg->in_width * zme_hprec), cfg->out_width);
    lhfir_offset = pq_cfg->lhfir_offset;
    chfir_offset = pq_cfg->chfir_offset;
    vpss_get_zme_inst_para(id, &zme_inst_para);

    if (id == XDP_ZME_ID_VPSS_OUT2_HZME) {
        hratio = (zme_hprec) / 4; /* dc zme fixed 4 times */
    }

    lhfir_offset = pq_cfg->lhfir_offset << 20; /* 20:para */
    chfir_offset = pq_cfg->chfir_offset << 20; /* 20:para */
    vpss_get_zme_inst_para(id, &zme_inst_para);
    offset = zme_inst_para.offset;

    if (hzme_mode == VPSS_HZME_TYP) {
        hl_shootctrl_mode = 0;
        hl_flatdect_mode = 0;
        hl_coringadj_en = 0;
        hl_gain = 0;
        hl_coring = 0;
        hc_shootctrl_mode = 0;
        hc_flatdect_mode = 0;
        hc_coringadj_en = 0;
        hc_gain = 0;
        hc_coring = 0;
    }

    vpss_hzme_set_out_fmt(vpss_regs, offset, out_fmt);
    vpss_hzme_set_ck_gt_en(vpss_regs, offset, ck_gt_en);
    vpss_hzme_set_out_width(vpss_regs, offset, out_width);
    vpss_hzme_set_lhfir_en(vpss_regs, offset, lhfir_en);
    vpss_hzme_set_chfir_en(vpss_regs, offset, chfir_en);
    vpss_hzme_set_lhmid_en(vpss_regs, offset, lhmid_en);
    vpss_hzme_set_chmid_en(vpss_regs, offset, chmid_en);
    vpss_hzme_set_non_lnr_en(vpss_regs, offset, non_lnr_en);
    vpss_hzme_set_lhfir_mode(vpss_regs, offset, lhfir_mode);
    vpss_hzme_set_chfir_mode(vpss_regs, offset, chfir_mode);
    vpss_hzme_set_hfir_order(vpss_regs, offset, hfir_order);
    vpss_hzme_set_hratio(vpss_regs, offset, hratio);
    vpss_hzme_set_lhfir_offset(vpss_regs, offset, lhfir_offset);
    vpss_hzme_set_chfir_offset(vpss_regs, offset, chfir_offset);
    vpss_hzme_set_zone0_delta(vpss_regs, offset, zone0_delta);
    vpss_hzme_set_zone2_delta(vpss_regs, offset, zone2_delta);
    vpss_hzme_set_zone1_end(vpss_regs, offset, zone1_end);
    vpss_hzme_set_zone0_end(vpss_regs, offset, zone0_end);
    vpss_hzme_set_hl_shootctrl_en(vpss_regs, offset, hl_shootctrl_en);
    vpss_hzme_set_hl_shootctrl_mode(vpss_regs, offset, hl_shootctrl_mode);
    vpss_hzme_set_hl_flatdect_mode(vpss_regs, offset, hl_flatdect_mode);
    vpss_hzme_set_hl_coringadj_en(vpss_regs, offset, hl_coringadj_en);
    vpss_hzme_set_hl_gain(vpss_regs, offset, hl_gain);
    vpss_hzme_set_hl_coring(vpss_regs, offset, hl_coring);
    vpss_hzme_set_hc_shootctrl_en(vpss_regs, offset, hc_shootctrl_en);
    vpss_hzme_set_hc_shootctrl_mode(vpss_regs, offset, hc_shootctrl_mode);
    vpss_hzme_set_hc_flatdect_mode(vpss_regs, offset, hc_flatdect_mode);
    vpss_hzme_set_hc_coringadj_en(vpss_regs, offset, hc_coringadj_en);
    vpss_hzme_set_hc_gain(vpss_regs, offset, hc_gain);
    vpss_hzme_set_hc_coring(vpss_regs, offset, hc_coring);
}

hi_void vpss_get_zme_inst_para(hi_u32 id, xdp_zme_inst_para *zme_inst_para)
{
    if (id == XDP_ZME_ID_VPSS_WR0_HDS_HZME) {
        zme_inst_para->offset = 0;
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 5; /* 5:para */
        zme_inst_para->zme_tap_hc = 5; /* 5:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = 2097152; /* 2097152:para */
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = (1 << 20) * 1; /* 1,20,1:para */
        zme_inst_para->zme_vc_rat = (1 << 12) * 1; /* 1,12,1:para */
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_WR1_HVDS_HZME) {
        zme_inst_para->offset = (VPSS_REG_WR1_HVDS_HZME_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 1,4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 8; /* 8:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 5; /* 5:para */
        zme_inst_para->zme_tap_hc = 5; /* 5:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = 2097152; /* 2097152:para */
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = (1 << 20) * 1; /* 1,20,1:para */
        zme_inst_para->zme_vc_rat = (1 << 12) * 1; /* 1,12,1:para */
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_WR1_HVDS_VZME) {
        zme_inst_para->offset = 0;
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 8; /* 8:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 3; /* 3:para */
        zme_inst_para->zme_tap_hc = 3; /* 3:para */
        zme_inst_para->zme_tap_vl = 3; /* 3:para */
        zme_inst_para->zme_tap_vc = 3; /* 3:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;  // 2097152;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 480; /* 480:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 480; /* 480:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_ME_CF_HDS_HZME) {
        zme_inst_para->offset = (VPSS_REG_ME_CF_HDS_HZME_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 5; /* 5:para */
        zme_inst_para->zme_tap_hc = 5; /* 5:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = 2097152; /* 2097152:para */
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = (1 << 20) * 1; /* 1,20,1:para */
        zme_inst_para->zme_vc_rat = (1 << 12) * 1; /* 1,12,1:para */
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_ME_P2_HDS_HZME) {
        zme_inst_para->offset = (VPSS_REG_ME_P2_HDS_HZME_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 2,4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 5; /* 5:para */
        zme_inst_para->zme_tap_hc = 5; /* 5:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = 2097152; /* 2097152:para */
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = (1 << 20) * 1; /* 1,20,1:para */
        zme_inst_para->zme_vc_rat = (1 << 12) * 1; /* 1,12,1:para */
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_CH_HZME) {
        zme_inst_para->offset = (VPSS_REG_CH_HZME_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 8; /* 8:para */
        zme_inst_para->zme_tap_hc = 8; /* 8:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_CH_VZME) {
        zme_inst_para->offset = (VPSS_REG_CH_VZME_ADDR - VPSS_REG_BASE_VZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 6; /* 6:para */
        zme_inst_para->zme_tap_hc = 6; /* 6:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_OUT2_HZME) {
        zme_inst_para->offset = (VPSS_REG_OUT2_CH_HZME_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 2,4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 4; /* 4:para */
        zme_inst_para->zme_tap_hc = 4; /* 4:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = (4 << 20); /* 4,20:para */
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = (1 << 20) * 1; /* 1,20,1:para */
        zme_inst_para->zme_vc_rat = (1 << 12) * 1; /* 1,12,1:para */
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_OUT2_VZME) {
        zme_inst_para->offset = (VPSS_REG_OUT2_VZME_ADDR - VPSS_REG_BASE_VZME_ADDR) / 4; /* 2,4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 0;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 5; /* 5:para */
        zme_inst_para->zme_tap_hc = 5; /* 5:para */
        zme_inst_para->zme_tap_vl = 4; /* 4:para */
        zme_inst_para->zme_tap_vc = 4; /* 4:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 2; /* 2:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = (4 << 12) * 1; /* 4,12,1:para */
        zme_inst_para->zme_vc_rat = (1 << 12) * 1; /* 1,12,1:para */
        zme_inst_para->zme_y_coef_fix = 1;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_HDR_IFIR) {
        zme_inst_para->offset = (VPSS_REG_HDR_IFIR_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 0;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 8; /* 8:para */
        zme_inst_para->zme_tap_hc = 8; /* 8:para */
        zme_inst_para->zme_tap_vl = 2; /* 2:para */
        zme_inst_para->zme_tap_vc = 2; /* 2:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_HDR_HDS) {
        zme_inst_para->offset = (VPSS_REG_HDR_DFIR_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 0;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 8; /* 8:para */
        zme_inst_para->zme_tap_hc = 8; /* 8:para */
        zme_inst_para->zme_tap_vl = 2; /* 2:para */
        zme_inst_para->zme_tap_vc = 2; /* 2:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_HDR_VDS) {
        zme_inst_para->offset = (VPSS_REG_HDR_VDS_ADDR - VPSS_REG_BASE_VZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 0;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 8; /* 8:para */
        zme_inst_para->zme_tap_hc = 8; /* 8:para */
        zme_inst_para->zme_tap_vl = 2; /* 2:para */
        zme_inst_para->zme_tap_vc = 2; /* 2:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_NPU_HZME) {
        zme_inst_para->offset = (VPSS_REG_NPU_HZME_ADDR - VPSS_REG_BASE_HZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 8; /* 8:para */
        zme_inst_para->zme_tap_hc = 8; /* 8:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    } else if (id == XDP_ZME_ID_VPSS_NPU_VZME) {
        zme_inst_para->offset = (VPSS_REG_NPU_VZME_ADDR - VPSS_REG_BASE_VZME_ADDR) / 4; /* 4:para */
        zme_inst_para->zme_y_en = 1;
        zme_inst_para->zme_c_en = 1;
        zme_inst_para->zme_dw = 10; /* 10:para */
        zme_inst_para->zme_cw = 10; /* 10:para */
        zme_inst_para->zme_tap_hl = 6; /* 6:para */
        zme_inst_para->zme_tap_hc = 6; /* 6:para */
        zme_inst_para->zme_tap_vl = 6; /* 6:para */
        zme_inst_para->zme_tap_vc = 6; /* 6:para */
        zme_inst_para->zme_coeff_norm = 10 - 1; /* 10,1:para */
        zme_inst_para->zme_phase = 32; /* 32:para */
        zme_inst_para->zme_hl_rat = 0;
        zme_inst_para->zme_hc_rat = 0;
        zme_inst_para->zme_vl_rat = 0;
        zme_inst_para->zme_vc_rat = 0;
        zme_inst_para->zme_y_coef_fix = 0;
        zme_inst_para->zme_y_coef_set = 0;
        zme_inst_para->zme_c_coef_fix = 0;
        zme_inst_para->zme_c_coef_set = 0;
        zme_inst_para->max_iw = 8192; /* 8192:para */
        zme_inst_para->max_ih = 4320; /* 4320:para */
        zme_inst_para->max_ow = 8192; /* 8192:para */
        zme_inst_para->max_oh = 4320; /* 4320:para */
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sr register source file
 * Author: pq
 * Create: 2019-09-29
 */
#include "pq_hal_comm.h"
#include "pq_hal_sr_regset.h"
#include "pq_hal_sr.h"


hi_void pq_reg_sr_setsren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_en)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.sr_en = sr_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setoutresult(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 outresult)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.outresult = outresult;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_sethippsrckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_sr_ck_gt_en)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.hipp_sr_ck_gt_en = hipp_sr_ck_gt_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setscalemode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scale_mode)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.scale_mode = scale_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.demo_en = demo_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.demo_mode = demo_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_pos)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.demo_pos = demo_pos;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setsr2dsrlsmoothdircoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_l_smooth_dir_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_l_smooth_dir_coef = sr_2dsr_l_smooth_dir_coef;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void pq_reg_sr_setsr2dsrlsmoothnoncoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_l_smooth_non_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_l_smooth_non_coef = sr_2dsr_l_smooth_non_coef;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void pq_reg_sr_setsr2dsrcdircoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_c_dir_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_c_dir_coef = sr_2dsr_c_dir_coef;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void pq_reg_sr_setsr2dsrcnoncoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_c_non_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_c_non_coef = sr_2dsr_c_non_coef;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void pq_reg_sr_setsr2dsrcoefdata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_coef_data)
{
    u_srself_coef_rdata srself_coef_rdata;

    srself_coef_rdata.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->srself_coef_rdata.u32) + offset));
    srself_coef_rdata.bits.sr_2dsr_coef_data = sr_2dsr_coef_data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->srself_coef_rdata.u32) + offset), srself_coef_rdata.u32);

    return;
}

hi_void pq_reg_sr_setdbsrdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_demo_en)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.dbsr_demo_en = dbsr_demo_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setdbsrdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_demo_mode)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.dbsr_demo_mode = dbsr_demo_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setgraphsoften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 graph_soft_en)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.graph_soft_en = graph_soft_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setparamode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 para_mode)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.para_mode = para_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void pq_reg_sr_setwsumlmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wsum_lmt)
{
    u_dbsr_wsum_coef dbsr_wsum_coef;

    dbsr_wsum_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wsum_coef.u32) + offset));
    dbsr_wsum_coef.bits.wsum_lmt = wsum_lmt;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wsum_coef.u32) + offset), dbsr_wsum_coef.u32);

    return;
}

hi_void pq_reg_sr_setfixiwendbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fix_iw_en_dbsr)
{
    u_dbsr_pxl_wgt_coef dbsr_pxl_wgt_coef;

    dbsr_pxl_wgt_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset));
    dbsr_pxl_wgt_coef.bits.fix_iw_en_dbsr = fix_iw_en_dbsr;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset), dbsr_pxl_wgt_coef.u32);

    return;
}

hi_void pq_reg_sr_setfixiwdbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fix_iw_dbsr)
{
    u_dbsr_pxl_wgt_coef dbsr_pxl_wgt_coef;

    dbsr_pxl_wgt_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset));
    dbsr_pxl_wgt_coef.bits.fix_iw_dbsr = fix_iw_dbsr;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset), dbsr_pxl_wgt_coef.u32);

    return;
}

hi_void pq_reg_sr_setcorekmagpdif(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 core_kmag_pdif)
{
    u_dbsr_core_pdif_coef dbsr_core_pdif_coef;

    dbsr_core_pdif_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset));
    dbsr_core_pdif_coef.bits.core_kmag_pdif = core_kmag_pdif;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset), dbsr_core_pdif_coef.u32);

    return;
}

hi_void pq_reg_sr_setcoremadpdif(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 core_mad_pdif)
{
    u_dbsr_core_pdif_coef dbsr_core_pdif_coef;

    dbsr_core_pdif_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset));
    dbsr_core_pdif_coef.bits.core_mad_pdif = core_mad_pdif;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset), dbsr_core_pdif_coef.u32);

    return;
}

hi_void pq_reg_sr_setkdifof(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_dif_of)
{
    u_dbsr_cur_wgt_coef1 dbsr_cur_wgt_coef1;

    dbsr_cur_wgt_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset));
    dbsr_cur_wgt_coef1.bits.k_dif_of = k_dif_of;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset), dbsr_cur_wgt_coef1.u32);

    return;
}

hi_void pq_reg_sr_setxconfw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_confw)
{
    u_dbsr_cur_wgt_coef1 dbsr_cur_wgt_coef1;

    dbsr_cur_wgt_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset));
    dbsr_cur_wgt_coef1.bits.x_confw = x_confw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset), dbsr_cur_wgt_coef1.u32);

    return;
}

hi_void pq_reg_sr_setkconfw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_confw)
{
    u_dbsr_cur_wgt_coef1 dbsr_cur_wgt_coef1;

    dbsr_cur_wgt_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset));
    dbsr_cur_wgt_coef1.bits.k_confw = k_confw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset), dbsr_cur_wgt_coef1.u32);

    return;
}

hi_void pq_reg_sr_setxofw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_ofw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.x_ofw = x_ofw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void pq_reg_sr_setkofw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ofw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.k_ofw = k_ofw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void pq_reg_sr_setxpdifw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_pdifw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.x_pdifw = x_pdifw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void pq_reg_sr_setkpdifw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_pdifw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.k_pdifw = k_pdifw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void pq_reg_sr_setx1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_mag_dbw)
{
    u_dbsr_mag_dbw_coefx dbsr_mag_dbw_coefx;

    dbsr_mag_dbw_coefx.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset));
    dbsr_mag_dbw_coefx.bits.x1_mag_dbw = x1_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset), dbsr_mag_dbw_coefx.u32);

    return;
}

hi_void pq_reg_sr_setx0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dbw)
{
    u_dbsr_mag_dbw_coefx dbsr_mag_dbw_coefx;

    dbsr_mag_dbw_coefx.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset));
    dbsr_mag_dbw_coefx.bits.x0_mag_dbw = x0_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset), dbsr_mag_dbw_coefx.u32);

    return;
}

hi_void pq_reg_sr_setk1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dbw)
{
    u_dbsr_mag_dbw_coefk0 dbsr_mag_dbw_coefk0;

    dbsr_mag_dbw_coefk0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset));
    dbsr_mag_dbw_coefk0.bits.k1_mag_dbw = k1_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset), dbsr_mag_dbw_coefk0.u32);

    return;
}

hi_void pq_reg_sr_setk0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k0_mag_dbw)
{
    u_dbsr_mag_dbw_coefk0 dbsr_mag_dbw_coefk0;

    dbsr_mag_dbw_coefk0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset));
    dbsr_mag_dbw_coefk0.bits.k0_mag_dbw = k0_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset), dbsr_mag_dbw_coefk0.u32);

    return;
}

hi_void pq_reg_sr_setk2magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_mag_dbw)
{
    u_dbsr_mag_dbw_coefk1 dbsr_mag_dbw_coefk1;

    dbsr_mag_dbw_coefk1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk1.u32) + offset));
    dbsr_mag_dbw_coefk1.bits.k2_mag_dbw = k2_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk1.u32) + offset), dbsr_mag_dbw_coefk1.u32);

    return;
}

hi_void pq_reg_sr_setg2magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_mag_dbw)
{
    u_dbsr_mag_dbw_coefg dbsr_mag_dbw_coefg;

    dbsr_mag_dbw_coefg.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset));
    dbsr_mag_dbw_coefg.bits.g2_mag_dbw = g2_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset), dbsr_mag_dbw_coefg.u32);

    return;
}

hi_void pq_reg_sr_setg1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dbw)
{
    u_dbsr_mag_dbw_coefg dbsr_mag_dbw_coefg;

    dbsr_mag_dbw_coefg.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset));
    dbsr_mag_dbw_coefg.bits.g1_mag_dbw = g1_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset), dbsr_mag_dbw_coefg.u32);

    return;
}

hi_void pq_reg_sr_setg0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dbw)
{
    u_dbsr_mag_dbw_coefg dbsr_mag_dbw_coefg;

    dbsr_mag_dbw_coefg.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset));
    dbsr_mag_dbw_coefg.bits.g0_mag_dbw = g0_mag_dbw;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset), dbsr_mag_dbw_coefg.u32);

    return;
}

hi_void pq_reg_sr_setlmtconflst(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lmt_conf_lst)
{
    u_dbsr_lmt_conf_coef dbsr_lmt_conf_coef;

    dbsr_lmt_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset));
    dbsr_lmt_conf_coef.bits.lmt_conf_lst = lmt_conf_lst;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset), dbsr_lmt_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setklmtconf0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_lmt_conf0)
{
    u_dbsr_lmt_conf_coef dbsr_lmt_conf_coef;

    dbsr_lmt_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset));
    dbsr_lmt_conf_coef.bits.k_lmt_conf0 = k_lmt_conf0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset), dbsr_lmt_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setk1magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_magk_conf)
{
    u_dbsr_magk_conf_coef dbsr_magk_conf_coef;

    dbsr_magk_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset));
    dbsr_magk_conf_coef.bits.k1_magk_conf = k1_magk_conf;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset), dbsr_magk_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setg0magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_magk_conf)
{
    u_dbsr_magk_conf_coef dbsr_magk_conf_coef;

    dbsr_magk_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset));
    dbsr_magk_conf_coef.bits.g0_magk_conf = g0_magk_conf;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset), dbsr_magk_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setx0magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_magk_conf)
{
    u_dbsr_magk_conf_coef dbsr_magk_conf_coef;

    dbsr_magk_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset));
    dbsr_magk_conf_coef.bits.x0_magk_conf = x0_magk_conf;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset), dbsr_magk_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setk1magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_magw_conf)
{
    u_dbsr_magw_conf_coef dbsr_magw_conf_coef;

    dbsr_magw_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset));
    dbsr_magw_conf_coef.bits.k1_magw_conf = k1_magw_conf;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset), dbsr_magw_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setg0magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_magw_conf)
{
    u_dbsr_magw_conf_coef dbsr_magw_conf_coef;

    dbsr_magw_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset));
    dbsr_magw_conf_coef.bits.g0_magw_conf = g0_magw_conf;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset), dbsr_magw_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setx0magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_magw_conf)
{
    u_dbsr_magw_conf_coef dbsr_magw_conf_coef;

    dbsr_magw_conf_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset));
    dbsr_magw_conf_coef.bits.x0_magw_conf = x0_magw_conf;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset), dbsr_magw_conf_coef.u32);

    return;
}

hi_void pq_reg_sr_setx1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefx dbsr_mag_dbw_bld_coefx;

    dbsr_mag_dbw_bld_coefx.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset));
    dbsr_mag_dbw_bld_coefx.bits.x1_mag_dbw_bld = x1_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset), dbsr_mag_dbw_bld_coefx.u32);

    return;
}

hi_void pq_reg_sr_setx0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefx dbsr_mag_dbw_bld_coefx;

    dbsr_mag_dbw_bld_coefx.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset));
    dbsr_mag_dbw_bld_coefx.bits.x0_mag_dbw_bld = x0_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset), dbsr_mag_dbw_bld_coefx.u32);

    return;
}

hi_void pq_reg_sr_setk1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefk0 dbsr_mag_dbw_bld_coefk0;

    dbsr_mag_dbw_bld_coefk0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset));
    dbsr_mag_dbw_bld_coefk0.bits.k1_mag_dbw_bld = k1_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset), dbsr_mag_dbw_bld_coefk0.u32);

    return;
}

hi_void pq_reg_sr_setk0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k0_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefk0 dbsr_mag_dbw_bld_coefk0;

    dbsr_mag_dbw_bld_coefk0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset));
    dbsr_mag_dbw_bld_coefk0.bits.k0_mag_dbw_bld = k0_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset), dbsr_mag_dbw_bld_coefk0.u32);

    return;
}

hi_void pq_reg_sr_setk2magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefk1 dbsr_mag_dbw_bld_coefk1;

    dbsr_mag_dbw_bld_coefk1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk1.u32) + offset));
    dbsr_mag_dbw_bld_coefk1.bits.k2_mag_dbw_bld = k2_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk1.u32) + offset), dbsr_mag_dbw_bld_coefk1.u32);

    return;
}

hi_void pq_reg_sr_setg2magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefg dbsr_mag_dbw_bld_coefg;

    dbsr_mag_dbw_bld_coefg.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset));
    dbsr_mag_dbw_bld_coefg.bits.g2_mag_dbw_bld = g2_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset), dbsr_mag_dbw_bld_coefg.u32);

    return;
}

hi_void pq_reg_sr_setg1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefg dbsr_mag_dbw_bld_coefg;

    dbsr_mag_dbw_bld_coefg.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset));
    dbsr_mag_dbw_bld_coefg.bits.g1_mag_dbw_bld = g1_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset), dbsr_mag_dbw_bld_coefg.u32);

    return;
}

hi_void pq_reg_sr_setg0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefg dbsr_mag_dbw_bld_coefg;

    dbsr_mag_dbw_bld_coefg.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset));
    dbsr_mag_dbw_bld_coefg.bits.g0_mag_dbw_bld = g0_mag_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset), dbsr_mag_dbw_bld_coefg.u32);

    return;
}

hi_void pq_reg_sr_setkbpdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_bp_dbw_bld)
{
    u_dbsr_bp_dbw_bld_coef dbsr_bp_dbw_bld_coef;

    dbsr_bp_dbw_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset));
    dbsr_bp_dbw_bld_coef.bits.k_bp_dbw_bld = k_bp_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset), dbsr_bp_dbw_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setxbpdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_bp_dbw_bld)
{
    u_dbsr_bp_dbw_bld_coef dbsr_bp_dbw_bld_coef;

    dbsr_bp_dbw_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset));
    dbsr_bp_dbw_bld_coef.bits.x_bp_dbw_bld = x_bp_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset), dbsr_bp_dbw_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setg1magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.g1_mag_dirw_adj = g1_mag_dirw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setk1magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.k1_mag_dirw_adj = k1_mag_dirw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setg0magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.g0_mag_dirw_adj = g0_mag_dirw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setx0magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.x0_mag_dirw_adj = x0_mag_dirw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setg1magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.g1_mag_ssw_adj = g1_mag_ssw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setk1magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.k1_mag_ssw_adj = k1_mag_ssw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setg0magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.g0_mag_ssw_adj = g0_mag_ssw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setx0magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.x0_mag_ssw_adj = x0_mag_ssw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setg1magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.g1_mag_ssw_kadj = g1_mag_ssw_kadj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void pq_reg_sr_setk1magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.k1_mag_ssw_kadj = k1_mag_ssw_kadj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void pq_reg_sr_setg0magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.g0_mag_ssw_kadj = g0_mag_ssw_kadj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void pq_reg_sr_setx0magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.x0_mag_ssw_kadj = x0_mag_ssw_kadj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void pq_reg_sr_setksswdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_dirw_adj)
{
    u_dbsr_wgt_adj_coef dbsr_wgt_adj_coef;

    dbsr_wgt_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset));
    dbsr_wgt_adj_coef.bits.k_ssw_dirw_adj = k_ssw_dirw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset), dbsr_wgt_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setksswsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_ssw_adj)
{
    u_dbsr_wgt_adj_coef dbsr_wgt_adj_coef;

    dbsr_wgt_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset));
    dbsr_wgt_adj_coef.bits.k_ssw_ssw_adj = k_ssw_ssw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset), dbsr_wgt_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setxdbwsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_dbw_ssw_adj)
{
    u_dbsr_wgt_adj_coef dbsr_wgt_adj_coef;

    dbsr_wgt_adj_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset));
    dbsr_wgt_adj_coef.bits.x_dbw_ssw_adj = x_dbw_ssw_adj;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset), dbsr_wgt_adj_coef.u32);

    return;
}

hi_void pq_reg_sr_setkminwdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_minw_dbw_bld)
{
    u_dbsr_wgt_bld_coef dbsr_wgt_bld_coef;

    dbsr_wgt_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset));
    dbsr_wgt_bld_coef.bits.k_minw_dbw_bld = k_minw_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset), dbsr_wgt_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setkidbwsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_idbw_ssw_bld)
{
    u_dbsr_wgt_bld_coef dbsr_wgt_bld_coef;

    dbsr_wgt_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset));
    dbsr_wgt_bld_coef.bits.k_idbw_ssw_bld = k_idbw_ssw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset), dbsr_wgt_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setxidbwsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_idbw_ssw_bld)
{
    u_dbsr_wgt_bld_coef dbsr_wgt_bld_coef;

    dbsr_wgt_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset));
    dbsr_wgt_bld_coef.bits.x_idbw_ssw_bld = x_idbw_ssw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset), dbsr_wgt_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setklpfbpdb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_lpf_bp_db)
{
    u_dbsr_bp_db_coef dbsr_bp_db_coef;

    dbsr_bp_db_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset));
    dbsr_bp_db_coef.bits.k_lpf_bp_db = k_lpf_bp_db;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset), dbsr_bp_db_coef.u32);

    return;
}

hi_void pq_reg_sr_setcoringbpdb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 coring_bp_db)
{
    u_dbsr_bp_db_coef dbsr_bp_db_coef;

    dbsr_bp_db_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset));
    dbsr_bp_db_coef.bits.coring_bp_db = coring_bp_db;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset), dbsr_bp_db_coef.u32);

    return;
}

hi_void pq_reg_sr_setksswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.k_ssw_bld = k_ssw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setbsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_ssw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.b_ssw_bld = b_ssw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setkdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_dbw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.k_dbw_bld = k_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setbdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_dbw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.b_dbw_bld = b_dbw_bld;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void pq_reg_sr_setc0graphsplit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c0_graph_split)
{
    u_dbsr_graph_wgt_cal_coef_split dbsr_graph_wgt_cal_coef_split;

    dbsr_graph_wgt_cal_coef_split.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) +
                                                       offset));
    dbsr_graph_wgt_cal_coef_split.bits.c0_graph_split = c0_graph_split;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) + offset),
                    dbsr_graph_wgt_cal_coef_split.u32);

    return;
}

hi_void pq_reg_sr_setc1graphsplit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c1_graph_split)
{
    u_dbsr_graph_wgt_cal_coef_split dbsr_graph_wgt_cal_coef_split;

    dbsr_graph_wgt_cal_coef_split.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) +
                                                       offset));
    dbsr_graph_wgt_cal_coef_split.bits.c1_graph_split = c1_graph_split;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) + offset),
                    dbsr_graph_wgt_cal_coef_split.u32);

    return;
}

hi_void pq_reg_sr_setc0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c0_graph)
{
    u_dbsr_graph_wgt_cal_coef0 dbsr_graph_wgt_cal_coef0;

    dbsr_graph_wgt_cal_coef0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset));
    dbsr_graph_wgt_cal_coef0.bits.c0_graph = c0_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset), dbsr_graph_wgt_cal_coef0.u32);

    return;
}

hi_void pq_reg_sr_setc1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c1_graph)
{
    u_dbsr_graph_wgt_cal_coef0 dbsr_graph_wgt_cal_coef0;

    dbsr_graph_wgt_cal_coef0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset));
    dbsr_graph_wgt_cal_coef0.bits.c1_graph = c1_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset), dbsr_graph_wgt_cal_coef0.u32);

    return;
}

hi_void pq_reg_sr_setr0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r0_graph)
{
    u_dbsr_graph_wgt_cal_coef1 dbsr_graph_wgt_cal_coef1;

    dbsr_graph_wgt_cal_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset));
    dbsr_graph_wgt_cal_coef1.bits.r0_graph = r0_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset), dbsr_graph_wgt_cal_coef1.u32);

    return;
}

hi_void pq_reg_sr_setr1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r1_graph)
{
    u_dbsr_graph_wgt_cal_coef1 dbsr_graph_wgt_cal_coef1;

    dbsr_graph_wgt_cal_coef1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset));
    dbsr_graph_wgt_cal_coef1.bits.r1_graph = r1_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset), dbsr_graph_wgt_cal_coef1.u32);

    return;
}

hi_void pq_reg_sr_setdifth0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth0_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth0_graph = difth0_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void pq_reg_sr_setdifth1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth1_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth1_graph = difth1_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void pq_reg_sr_setdifth2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth2_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth2_graph = difth2_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void pq_reg_sr_setdifth3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth3_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth3_graph = difth3_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void pq_reg_sr_setcorek1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek1_graph)
{
    u_dbsr_graph_wgt_cal_coef3 dbsr_graph_wgt_cal_coef3;

    dbsr_graph_wgt_cal_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset));
    dbsr_graph_wgt_cal_coef3.bits.corek1_graph = corek1_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset), dbsr_graph_wgt_cal_coef3.u32);

    return;
}

hi_void pq_reg_sr_setcorek2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek2_graph)
{
    u_dbsr_graph_wgt_cal_coef3 dbsr_graph_wgt_cal_coef3;

    dbsr_graph_wgt_cal_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset));
    dbsr_graph_wgt_cal_coef3.bits.corek2_graph = corek2_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset), dbsr_graph_wgt_cal_coef3.u32);

    return;
}

hi_void pq_reg_sr_setcorek3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek3_graph)
{
    u_dbsr_graph_wgt_cal_coef3 dbsr_graph_wgt_cal_coef3;

    dbsr_graph_wgt_cal_coef3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset));
    dbsr_graph_wgt_cal_coef3.bits.corek3_graph = corek3_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset), dbsr_graph_wgt_cal_coef3.u32);

    return;
}

hi_void pq_reg_sr_setx0kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef4 dbsr_graph_wgt_cal_coef4;

    dbsr_graph_wgt_cal_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset));
    dbsr_graph_wgt_cal_coef4.bits.x0_kk_graph = x0_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset), dbsr_graph_wgt_cal_coef4.u32);

    return;
}

hi_void pq_reg_sr_setx1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef4 dbsr_graph_wgt_cal_coef4;

    dbsr_graph_wgt_cal_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset));
    dbsr_graph_wgt_cal_coef4.bits.x1_kk_graph = x1_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset), dbsr_graph_wgt_cal_coef4.u32);

    return;
}

hi_void pq_reg_sr_setx2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x2_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef4 dbsr_graph_wgt_cal_coef4;

    dbsr_graph_wgt_cal_coef4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset));
    dbsr_graph_wgt_cal_coef4.bits.x2_kk_graph = x2_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset), dbsr_graph_wgt_cal_coef4.u32);

    return;
}

hi_void pq_reg_sr_setg1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef5 dbsr_graph_wgt_cal_coef5;

    dbsr_graph_wgt_cal_coef5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset));
    dbsr_graph_wgt_cal_coef5.bits.g1_kk_graph = g1_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset), dbsr_graph_wgt_cal_coef5.u32);

    return;
}

hi_void pq_reg_sr_setg2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef5 dbsr_graph_wgt_cal_coef5;

    dbsr_graph_wgt_cal_coef5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset));
    dbsr_graph_wgt_cal_coef5.bits.g2_kk_graph = g2_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset), dbsr_graph_wgt_cal_coef5.u32);

    return;
}

hi_void pq_reg_sr_setg0kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef6 dbsr_graph_wgt_cal_coef6;

    dbsr_graph_wgt_cal_coef6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef6.u32) + offset));
    dbsr_graph_wgt_cal_coef6.bits.g0_kk_graph = g0_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef6.u32) + offset), dbsr_graph_wgt_cal_coef6.u32);

    return;
}

hi_void pq_reg_sr_setk3kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k3_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef7 dbsr_graph_wgt_cal_coef7;

    dbsr_graph_wgt_cal_coef7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset));
    dbsr_graph_wgt_cal_coef7.bits.k3_kk_graph = k3_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset), dbsr_graph_wgt_cal_coef7.u32);

    return;
}

hi_void pq_reg_sr_setg3kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g3_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef7 dbsr_graph_wgt_cal_coef7;

    dbsr_graph_wgt_cal_coef7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset));
    dbsr_graph_wgt_cal_coef7.bits.g3_kk_graph = g3_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset), dbsr_graph_wgt_cal_coef7.u32);

    return;
}

hi_void pq_reg_sr_setk1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef8 dbsr_graph_wgt_cal_coef8;

    dbsr_graph_wgt_cal_coef8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset));
    dbsr_graph_wgt_cal_coef8.bits.k1_kk_graph = k1_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset), dbsr_graph_wgt_cal_coef8.u32);

    return;
}

hi_void pq_reg_sr_setk2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef8 dbsr_graph_wgt_cal_coef8;

    dbsr_graph_wgt_cal_coef8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset));
    dbsr_graph_wgt_cal_coef8.bits.k2_kk_graph = k2_kk_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset), dbsr_graph_wgt_cal_coef8.u32);

    return;
}

hi_void pq_reg_sr_setk1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.k1_graph = k1_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void pq_reg_sr_setk2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.k2_graph = k2_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void pq_reg_sr_setk3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k3_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.k3_graph = k3_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void pq_reg_sr_setwgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.w_graph = w_graph;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase03(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase03)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase03 = dbsr_bicubic_phase03;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase02)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase02 = dbsr_bicubic_phase02;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase01)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase01 = dbsr_bicubic_phase01;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase00)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase00 = dbsr_bicubic_phase00;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase13(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase13)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase13 = dbsr_bicubic_phase13;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase12)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase12 = dbsr_bicubic_phase12;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase11)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase11 = dbsr_bicubic_phase11;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void pq_reg_sr_setdbsrbicubicphase10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase10)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase10 = dbsr_bicubic_phase10;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void pq_reg_sr_setopdstdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_pdst_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.o_pdst_dneed = o_pdst_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setopdstdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_pdst_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.o_pdst_drdy = o_pdst_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setiplrydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_plry_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_plry_dneed = i_plry_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setiplrydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_plry_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_plry_drdy = i_plry_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setipsrcylowdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_low_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_low_dneed = i_psrcy_low_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setipsrcylowdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_low_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_low_drdy = i_psrcy_low_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setipsrcyhighdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_high_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_high_dneed = i_psrcy_high_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setipsrcyhighdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_high_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_high_drdy = i_psrcy_high_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setwgtavgcnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wgt_avg_cnt_y)
{
    u_dbsr_debug1 dbsr_debug1;

    dbsr_debug1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset));
    dbsr_debug1.bits.wgt_avg_cnt_y = wgt_avg_cnt_y;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset), dbsr_debug1.u32);

    return;
}

hi_void pq_reg_sr_setwgtavgcntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wgt_avg_cnt_x)
{
    u_dbsr_debug1 dbsr_debug1;

    dbsr_debug1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset));
    dbsr_debug1.bits.wgt_avg_cnt_x = wgt_avg_cnt_x;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset), dbsr_debug1.u32);

    return;
}

hi_void pq_reg_sr_setdbsrparacoefdata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_para_coef_data)
{
    u_dbsr_coef_data dbsr_coef_data;

    dbsr_coef_data.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->dbsr_coef_data.u32) + offset));
    dbsr_coef_data.bits.dbsr_para_coef_data = dbsr_para_coef_data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->dbsr_coef_data.u32) + offset), dbsr_coef_data.u32);

    return;
}

hi_void pq_reg_sr_setosrcdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_c_dneed = o_sr_c_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setosrcdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_c_drdy = o_sr_c_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setosrydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_y_dneed = o_sr_y_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setosrydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_y_drdy = o_sr_y_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_seticlmcdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_c_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_c_dneed = i_clm_c_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_seticlmcdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_c_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_c_drdy = i_clm_c_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_seticlmydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_y_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_y_dneed = i_clm_y_dneed;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_seticlmydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_y_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_y_drdy = i_clm_y_drdy;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void pq_reg_sr_setosrycnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_cnt_y)
{
    u_sr_debug1 sr_debug1;

    sr_debug1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset));
    sr_debug1.bits.o_sr_y_cnt_y = o_sr_y_cnt_y;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset), sr_debug1.u32);

    return;
}

hi_void pq_reg_sr_setosrycntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_cnt_x)
{
    u_sr_debug1 sr_debug1;

    sr_debug1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset));
    sr_debug1.bits.o_sr_y_cnt_x = o_sr_y_cnt_x;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset), sr_debug1.u32);

    return;
}

hi_void pq_reg_sr_setosrccnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_cnt_y)
{
    u_sr_debug2 sr_debug2;

    sr_debug2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset));
    sr_debug2.bits.o_sr_c_cnt_y = o_sr_c_cnt_y;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset), sr_debug2.u32);

    return;
}

hi_void pq_reg_sr_setosrccntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_cnt_x)
{
    u_sr_debug2 sr_debug2;

    sr_debug2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset));
    sr_debug2.bits.o_sr_c_cnt_x = o_sr_c_cnt_x;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset), sr_debug2.u32);

    return;
}

hi_void pq_reg_sr_setwgraphrtl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_graph_rtl)
{
    u_graph_rtl graph_rtl;

    graph_rtl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->graph_rtl.u32) + offset));
    graph_rtl.bits.w_graph_rtl = w_graph_rtl;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->graph_rtl.u32) + offset), graph_rtl.u32);

    return;
}

hi_void pq_reg_sr_setdifstatraw0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw0 difstat_raw0;

    difstat_raw0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->difstat_raw0.u32) + offset));
    difstat_raw0.bits.difstat_raw0 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->difstat_raw0.u32) + offset), difstat_raw0.u32);

    return;
}

hi_void pq_reg_sr_setdifstatraw1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw1 difstat_raw1;

    difstat_raw1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->difstat_raw1.u32) + offset));
    difstat_raw1.bits.difstat_raw1 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->difstat_raw1.u32) + offset), difstat_raw1.u32);

    return;
}

hi_void pq_reg_sr_setdifstatraw2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw2 difstat_raw2;

    difstat_raw2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->difstat_raw2.u32) + offset));
    difstat_raw2.bits.difstat_raw2 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->difstat_raw2.u32) + offset), difstat_raw2.u32);

    return;
}

hi_void pq_reg_sr_setdifstatraw3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw3 difstat_raw3;

    difstat_raw3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->difstat_raw3.u32) + offset));
    difstat_raw3.bits.difstat_raw3 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->difstat_raw3.u32) + offset), difstat_raw3.u32);

    return;
}



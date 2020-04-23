/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_hipp_sr.h"
#include "hal_vdp_comm.h"

hi_void vdp_hipp_sr_setsren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_en)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.sr_en = sr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setoutresult(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 outresult)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.outresult = outresult;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_sethippsrckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_sr_ck_gt_en)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.hipp_sr_ck_gt_en = hipp_sr_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setscalemode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scale_mode)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.scale_mode = scale_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.demo_en = demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.demo_mode = demo_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_pos)
{
    u_hipp_sr_ctrl hipp_sr_ctrl;

    hipp_sr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset));
    hipp_sr_ctrl.bits.demo_pos = demo_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_sr_ctrl.u32) + offset), hipp_sr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setbicubicphase03(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bicubic_phase03)
{
    u_srself_bicubic_coef_0 srself_bicubic_coef_0;

    srself_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset));
    srself_bicubic_coef_0.bits.bicubic_phase03 = bicubic_phase03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset), srself_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setbicubicphase02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bicubic_phase02)
{
    u_srself_bicubic_coef_0 srself_bicubic_coef_0;

    srself_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset));
    srself_bicubic_coef_0.bits.bicubic_phase02 = bicubic_phase02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset), srself_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setbicubicphase01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bicubic_phase01)
{
    u_srself_bicubic_coef_0 srself_bicubic_coef_0;

    srself_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset));
    srself_bicubic_coef_0.bits.bicubic_phase01 = bicubic_phase01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset), srself_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setbicubicphase00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bicubic_phase00)
{
    u_srself_bicubic_coef_0 srself_bicubic_coef_0;

    srself_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset));
    srself_bicubic_coef_0.bits.bicubic_phase00 = bicubic_phase00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_bicubic_coef_0.u32) + offset), srself_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setbicubicphase11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bicubic_phase11)
{
    u_srself_bicubic_coef_1 srself_bicubic_coef_1;

    srself_bicubic_coef_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_bicubic_coef_1.u32) + offset));
    srself_bicubic_coef_1.bits.bicubic_phase11 = bicubic_phase11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_bicubic_coef_1.u32) + offset), srself_bicubic_coef_1.u32);

    return;
}

hi_void vdp_hipp_sr_setbicubicphase10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bicubic_phase10)
{
    u_srself_bicubic_coef_1 srself_bicubic_coef_1;

    srself_bicubic_coef_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_bicubic_coef_1.u32) + offset));
    srself_bicubic_coef_1.bits.bicubic_phase10 = bicubic_phase10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_bicubic_coef_1.u32) + offset), srself_bicubic_coef_1.u32);

    return;
}

hi_void vdp_hipp_sr_setsaddiffthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_diff_thr2)
{
    u_srself_sad_adj_0 srself_sad_adj_0;

    srself_sad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset));
    srself_sad_adj_0.bits.sad_diff_thr2 = sad_diff_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset), srself_sad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsaddiffthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_diff_thr1)
{
    u_srself_sad_adj_0 srself_sad_adj_0;

    srself_sad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset));
    srself_sad_adj_0.bits.sad_diff_thr1 = sad_diff_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset), srself_sad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsaddiffweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_diff_weight2)
{
    u_srself_sad_adj_0 srself_sad_adj_0;

    srself_sad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset));
    srself_sad_adj_0.bits.sad_diff_weight2 = sad_diff_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset), srself_sad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsaddiffweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_diff_weight1)
{
    u_srself_sad_adj_0 srself_sad_adj_0;

    srself_sad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset));
    srself_sad_adj_0.bits.sad_diff_weight1 = sad_diff_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sad_adj_0.u32) + offset), srself_sad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsaddiffslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_diff_slop)
{
    u_srself_sad_adj_1 srself_sad_adj_1;

    srself_sad_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sad_adj_1.u32) + offset));
    srself_sad_adj_1.bits.sad_diff_slop = sad_diff_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sad_adj_1.u32) + offset), srself_sad_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_ratio)
{
    u_srself_sad_adj_1 srself_sad_adj_1;

    srself_sad_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sad_adj_1.u32) + offset));
    srself_sad_adj_1.bits.min_sad_ratio = min_sad_ratio;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sad_adj_1.u32) + offset), srself_sad_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setsndsaddiffthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 snd_sad_diff_thr2)
{
    u_srself_minmad2_adj_0 srself_minmad2_adj_0;

    srself_minmad2_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset));
    srself_minmad2_adj_0.bits.snd_sad_diff_thr2 = snd_sad_diff_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset), srself_minmad2_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsndsaddiffthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 snd_sad_diff_thr1)
{
    u_srself_minmad2_adj_0 srself_minmad2_adj_0;

    srself_minmad2_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset));
    srself_minmad2_adj_0.bits.snd_sad_diff_thr1 = snd_sad_diff_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset), srself_minmad2_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsndsaddiffweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 snd_sad_diff_weight2)
{
    u_srself_minmad2_adj_0 srself_minmad2_adj_0;

    srself_minmad2_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset));
    srself_minmad2_adj_0.bits.snd_sad_diff_weight2 = snd_sad_diff_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset), srself_minmad2_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsndsaddiffweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 snd_sad_diff_weight1)
{
    u_srself_minmad2_adj_0 srself_minmad2_adj_0;

    srself_minmad2_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset));
    srself_minmad2_adj_0.bits.snd_sad_diff_weight1 = snd_sad_diff_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_0.u32) + offset), srself_minmad2_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setmatchweightclip(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_weight_clip)
{
    u_srself_minmad2_adj_1 srself_minmad2_adj_1;

    srself_minmad2_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_1.u32) + offset));
    srself_minmad2_adj_1.bits.match_weight_clip = match_weight_clip;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_1.u32) + offset), srself_minmad2_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setblock6x6amendmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 block6x6_amend_mode)
{
    u_srself_minmad2_adj_1 srself_minmad2_adj_1;

    srself_minmad2_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_1.u32) + offset));
    srself_minmad2_adj_1.bits.block6x6_amend_mode = block6x6_amend_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_1.u32) + offset), srself_minmad2_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setsndsaddiffslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 snd_sad_diff_slop)
{
    u_srself_minmad2_adj_1 srself_minmad2_adj_1;

    srself_minmad2_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minmad2_adj_1.u32) + offset));
    srself_minmad2_adj_1.bits.snd_sad_diff_slop = snd_sad_diff_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minmad2_adj_1.u32) + offset), srself_minmad2_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadadjustthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_adjust_thr1)
{
    u_srself_minsad_thr srself_minsad_thr;

    srself_minsad_thr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_thr.u32) + offset));
    srself_minsad_thr.bits.min_sad_adjust_thr1 = min_sad_adjust_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_thr.u32) + offset), srself_minsad_thr.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadadjustthr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_adjust_thr0)
{
    u_srself_minsad_thr srself_minsad_thr;

    srself_minsad_thr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_thr.u32) + offset));
    srself_minsad_thr.bits.min_sad_adjust_thr0 = min_sad_adjust_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_thr.u32) + offset), srself_minsad_thr.u32);

    return;
}

hi_void vdp_hipp_sr_setsumsadthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sum_sad_thr1)
{
    u_srself_sumsad_adj_0 srself_sumsad_adj_0;

    srself_sumsad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sumsad_adj_0.u32) + offset));
    srself_sumsad_adj_0.bits.sum_sad_thr1 = sum_sad_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sumsad_adj_0.u32) + offset), srself_sumsad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsumsadweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sum_sad_weight2)
{
    u_srself_sumsad_adj_0 srself_sumsad_adj_0;

    srself_sumsad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sumsad_adj_0.u32) + offset));
    srself_sumsad_adj_0.bits.sum_sad_weight2 = sum_sad_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sumsad_adj_0.u32) + offset), srself_sumsad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsumsadweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sum_sad_weight1)
{
    u_srself_sumsad_adj_0 srself_sumsad_adj_0;

    srself_sumsad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sumsad_adj_0.u32) + offset));
    srself_sumsad_adj_0.bits.sum_sad_weight1 = sum_sad_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sumsad_adj_0.u32) + offset), srself_sumsad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setsumsadslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sum_sad_slop)
{
    u_srself_sumsad_adj_1 srself_sumsad_adj_1;

    srself_sumsad_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sumsad_adj_1.u32) + offset));
    srself_sumsad_adj_1.bits.sum_sad_slop = sum_sad_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sumsad_adj_1.u32) + offset), srself_sumsad_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setsumsadthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sum_sad_thr2)
{
    u_srself_sumsad_adj_1 srself_sumsad_adj_1;

    srself_sumsad_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sumsad_adj_1.u32) + offset));
    srself_sumsad_adj_1.bits.sum_sad_thr2 = sum_sad_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sumsad_adj_1.u32) + offset), srself_sumsad_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_thr1)
{
    u_srself_minsad_adj_0 srself_minsad_adj_0;

    srself_minsad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_adj_0.u32) + offset));
    srself_minsad_adj_0.bits.min_sad_thr1 = min_sad_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_adj_0.u32) + offset), srself_minsad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_weight2)
{
    u_srself_minsad_adj_0 srself_minsad_adj_0;

    srself_minsad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_adj_0.u32) + offset));
    srself_minsad_adj_0.bits.min_sad_weight2 = min_sad_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_adj_0.u32) + offset), srself_minsad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_weight1)
{
    u_srself_minsad_adj_0 srself_minsad_adj_0;

    srself_minsad_adj_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_adj_0.u32) + offset));
    srself_minsad_adj_0.bits.min_sad_weight1 = min_sad_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_adj_0.u32) + offset), srself_minsad_adj_0.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_slop)
{
    u_srself_minsad_adj_1 srself_minsad_adj_1;

    srself_minsad_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_adj_1.u32) + offset));
    srself_minsad_adj_1.bits.min_sad_slop = min_sad_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_adj_1.u32) + offset), srself_minsad_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_thr2)
{
    u_srself_minsad_adj_1 srself_minsad_adj_1;

    srself_minsad_adj_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_minsad_adj_1.u32) + offset));
    srself_minsad_adj_1.bits.min_sad_thr2 = min_sad_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_minsad_adj_1.u32) + offset), srself_minsad_adj_1.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_gain)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.min_sad_gain = min_sad_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_sethorvsadoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 horv_sad_offset)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.horv_sad_offset = horv_sad_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_setdiagsadlimitgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 diagsad_limitgain)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.diagsad_limitgain = diagsad_limitgain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmononeibside0gain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sadmono_neib_side0_gain)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.sadmono_neib_side0_gain = sadmono_neib_side0_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_setweightnongain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 weight_non_gain)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.weight_non_gain = weight_non_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_setsadcorrectgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_correct_gain)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.sad_correct_gain = sad_correct_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_setneibsadgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 neib_sad_gain)
{
    u_srself_sr2d_dirdet0 srself_sr2d_dirdet0;

    srself_sr2d_dirdet0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset));
    srself_sr2d_dirdet0.bits.neib_sad_gain = neib_sad_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet0.u32) + offset), srself_sr2d_dirdet0.u32);

    return;
}

hi_void vdp_hipp_sr_setweightlpfgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 weight_lpf_gain)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.weight_lpf_gain = weight_lpf_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_sethorvsadgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 horv_sad_gain)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.horv_sad_gain = horv_sad_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_setweightnonoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 weight_non_offset)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.weight_non_offset = weight_non_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmononeibside0maxgain(vdp_regs_type *vdp_reg, hi_u32 offset,
                                               hi_u32 sadmono_neib_side0_max_gain)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.sadmono_neib_side0_max_gain = sadmono_neib_side0_max_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_setdiffsadweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 diff_sad_weight)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.diff_sad_weight = diff_sad_weight;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_setavgsadweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 avg_sad_weight)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.avg_sad_weight = avg_sad_weight;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_setneibdirweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 neib_dir_weight)
{
    u_srself_sr2d_dirdet1 srself_sr2d_dirdet1;

    srself_sr2d_dirdet1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset));
    srself_sr2d_dirdet1.bits.neib_dir_weight = neib_dir_weight;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet1.u32) + offset), srself_sr2d_dirdet1.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmononeibside1gain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sadmono_neib_side1_gain)
{
    u_srself_sr2d_dirdet2 srself_sr2d_dirdet2;

    srself_sr2d_dirdet2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset));
    srself_sr2d_dirdet2.bits.sadmono_neib_side1_gain = sadmono_neib_side1_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset), srself_sr2d_dirdet2.u32);

    return;
}

hi_void vdp_hipp_sr_sethvdirdiffthr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hvdirdiffthr)
{
    u_srself_sr2d_dirdet2 srself_sr2d_dirdet2;

    srself_sr2d_dirdet2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset));
    srself_sr2d_dirdet2.bits.hvdirdiffthr = hvdirdiffthr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset), srself_sr2d_dirdet2.u32);

    return;
}

hi_void vdp_hipp_sr_setmaxweightgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 maxweight_gain)
{
    u_srself_sr2d_dirdet2 srself_sr2d_dirdet2;

    srself_sr2d_dirdet2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset));
    srself_sr2d_dirdet2.bits.maxweight_gain = maxweight_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset), srself_sr2d_dirdet2.u32);

    return;
}

hi_void vdp_hipp_sr_setflatdetmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 flat_detmode)
{
    u_srself_sr2d_dirdet2 srself_sr2d_dirdet2;

    srself_sr2d_dirdet2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset));
    srself_sr2d_dirdet2.bits.flat_detmode = flat_detmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset), srself_sr2d_dirdet2.u32);

    return;
}

hi_void vdp_hipp_sr_setflatcoringgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 flat_coring_gain)
{
    u_srself_sr2d_dirdet2 srself_sr2d_dirdet2;

    srself_sr2d_dirdet2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset));
    srself_sr2d_dirdet2.bits.flat_coring_gain = flat_coring_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset), srself_sr2d_dirdet2.u32);

    return;
}

hi_void vdp_hipp_sr_setminsadcoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_sad_coring)
{
    u_srself_sr2d_dirdet2 srself_sr2d_dirdet2;

    srself_sr2d_dirdet2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset));
    srself_sr2d_dirdet2.bits.min_sad_coring = min_sad_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_dirdet2.u32) + offset), srself_sr2d_dirdet2.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3coring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_coring)
{
    u_srself_sr2d_inter srself_sr2d_inter;

    srself_sr2d_inter.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset));
    srself_sr2d_inter.bits.sad_max3_coring = sad_max3_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset), srself_sr2d_inter.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax1clip(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max1_clip)
{
    u_srself_sr2d_inter srself_sr2d_inter;

    srself_sr2d_inter.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset));
    srself_sr2d_inter.bits.sad_max1_clip = sad_max1_clip;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset), srself_sr2d_inter.u32);

    return;
}

hi_void vdp_hipp_sr_setangleadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_adj_en)
{
    u_srself_sr2d_inter srself_sr2d_inter;

    srself_sr2d_inter.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset));
    srself_sr2d_inter.bits.angle_adj_en = angle_adj_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset), srself_sr2d_inter.u32);

    return;
}

hi_void vdp_hipp_sr_setlowangdirweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 low_ang_dir_weight)
{
    u_srself_sr2d_inter srself_sr2d_inter;

    srself_sr2d_inter.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset));
    srself_sr2d_inter.bits.low_ang_dir_weight = low_ang_dir_weight;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset), srself_sr2d_inter.u32);

    return;
}

hi_void vdp_hipp_sr_sethighangdirweight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 high_ang_dir_weight)
{
    u_srself_sr2d_inter srself_sr2d_inter;

    srself_sr2d_inter.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset));
    srself_sr2d_inter.bits.high_ang_dir_weight = high_ang_dir_weight;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_sr2d_inter.u32) + offset), srself_sr2d_inter.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3thr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_thr2)
{
    u_srself_ang_adj0 srself_ang_adj0;

    srself_ang_adj0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj0.u32) + offset));
    srself_ang_adj0.bits.sad_max3_thr2 = sad_max3_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj0.u32) + offset), srself_ang_adj0.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3thr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_thr1)
{
    u_srself_ang_adj0 srself_ang_adj0;

    srself_ang_adj0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj0.u32) + offset));
    srself_ang_adj0.bits.sad_max3_thr1 = sad_max3_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj0.u32) + offset), srself_ang_adj0.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3thr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_thr0)
{
    u_srself_ang_adj0 srself_ang_adj0;

    srself_ang_adj0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj0.u32) + offset));
    srself_ang_adj0.bits.sad_max3_thr0 = sad_max3_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj0.u32) + offset), srself_ang_adj0.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3adj0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_adj0)
{
    u_srself_ang_adj1 srself_ang_adj1;

    srself_ang_adj1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset));
    srself_ang_adj1.bits.sad_max3_adj0 = sad_max3_adj0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset), srself_ang_adj1.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3k3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_k3)
{
    u_srself_ang_adj1 srself_ang_adj1;

    srself_ang_adj1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset));
    srself_ang_adj1.bits.sad_max3_k3 = sad_max3_k3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset), srself_ang_adj1.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3k2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_k2)
{
    u_srself_ang_adj1 srself_ang_adj1;

    srself_ang_adj1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset));
    srself_ang_adj1.bits.sad_max3_k2 = sad_max3_k2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset), srself_ang_adj1.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3k1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_k1)
{
    u_srself_ang_adj1 srself_ang_adj1;

    srself_ang_adj1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset));
    srself_ang_adj1.bits.sad_max3_k1 = sad_max3_k1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset), srself_ang_adj1.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3k0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_k0)
{
    u_srself_ang_adj1 srself_ang_adj1;

    srself_ang_adj1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset));
    srself_ang_adj1.bits.sad_max3_k0 = sad_max3_k0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj1.u32) + offset), srself_ang_adj1.u32);

    return;
}

hi_void vdp_hipp_sr_setangdiffgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_diff_gain)
{
    u_srself_ang_adj2 srself_ang_adj2;

    srself_ang_adj2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset));
    srself_ang_adj2.bits.ang_diff_gain = ang_diff_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset), srself_ang_adj2.u32);

    return;
}

hi_void vdp_hipp_sr_setangadjthr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_adj_thr0)
{
    u_srself_ang_adj2 srself_ang_adj2;

    srself_ang_adj2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset));
    srself_ang_adj2.bits.ang_adj_thr0 = ang_adj_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset), srself_ang_adj2.u32);

    return;
}

hi_void vdp_hipp_sr_setweightadjclip(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 weight_adj_clip)
{
    u_srself_ang_adj2 srself_ang_adj2;

    srself_ang_adj2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset));
    srself_ang_adj2.bits.weight_adj_clip = weight_adj_clip;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset), srself_ang_adj2.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3adj2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_adj2)
{
    u_srself_ang_adj2 srself_ang_adj2;

    srself_ang_adj2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset));
    srself_ang_adj2.bits.sad_max3_adj2 = sad_max3_adj2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset), srself_ang_adj2.u32);

    return;
}

hi_void vdp_hipp_sr_setsadmax3adj1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sad_max3_adj1)
{
    u_srself_ang_adj2 srself_ang_adj2;

    srself_ang_adj2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset));
    srself_ang_adj2.bits.sad_max3_adj1 = sad_max3_adj1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj2.u32) + offset), srself_ang_adj2.u32);

    return;
}

hi_void vdp_hipp_sr_setanglehamendthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_h_amend_thr1)
{
    u_srself_ang_adj3 srself_ang_adj3;

    srself_ang_adj3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset));
    srself_ang_adj3.bits.angle_h_amend_thr1 = angle_h_amend_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset), srself_ang_adj3.u32);

    return;
}

hi_void vdp_hipp_sr_setanglehamendweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_h_amend_weight2)
{
    u_srself_ang_adj3 srself_ang_adj3;

    srself_ang_adj3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset));
    srself_ang_adj3.bits.angle_h_amend_weight2 = angle_h_amend_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset), srself_ang_adj3.u32);

    return;
}

hi_void vdp_hipp_sr_setanglehamendweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_h_amend_weight1)
{
    u_srself_ang_adj3 srself_ang_adj3;

    srself_ang_adj3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset));
    srself_ang_adj3.bits.angle_h_amend_weight1 = angle_h_amend_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset), srself_ang_adj3.u32);

    return;
}

hi_void vdp_hipp_sr_setangadjthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_adj_thr1)
{
    u_srself_ang_adj3 srself_ang_adj3;

    srself_ang_adj3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset));
    srself_ang_adj3.bits.ang_adj_thr1 = ang_adj_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_adj3.u32) + offset), srself_ang_adj3.u32);

    return;
}

hi_void vdp_hipp_sr_setanglevamendweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_v_amend_weight2)
{
    u_srself_ang_amend0 srself_ang_amend0;

    srself_ang_amend0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset));
    srself_ang_amend0.bits.angle_v_amend_weight2 = angle_v_amend_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset), srself_ang_amend0.u32);

    return;
}

hi_void vdp_hipp_sr_setanglevamendweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_v_amend_weight1)
{
    u_srself_ang_amend0 srself_ang_amend0;

    srself_ang_amend0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset));
    srself_ang_amend0.bits.angle_v_amend_weight1 = angle_v_amend_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset), srself_ang_amend0.u32);

    return;
}

hi_void vdp_hipp_sr_setanglehamendslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_h_amend_slop)
{
    u_srself_ang_amend0 srself_ang_amend0;

    srself_ang_amend0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset));
    srself_ang_amend0.bits.angle_h_amend_slop = angle_h_amend_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset), srself_ang_amend0.u32);

    return;
}

hi_void vdp_hipp_sr_setanglehamendthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_h_amend_thr2)
{
    u_srself_ang_amend0 srself_ang_amend0;

    srself_ang_amend0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset));
    srself_ang_amend0.bits.angle_h_amend_thr2 = angle_h_amend_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend0.u32) + offset), srself_ang_amend0.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelamendweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_amend_weight1)
{
    u_srself_ang_amend1 srself_ang_amend1;

    srself_ang_amend1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset));
    srself_ang_amend1.bits.ang_rel_amend_weight1 = ang_rel_amend_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset), srself_ang_amend1.u32);

    return;
}

hi_void vdp_hipp_sr_setanglevamendslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_v_amend_slop)
{
    u_srself_ang_amend1 srself_ang_amend1;

    srself_ang_amend1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset));
    srself_ang_amend1.bits.angle_v_amend_slop = angle_v_amend_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset), srself_ang_amend1.u32);

    return;
}

hi_void vdp_hipp_sr_setanglevamendthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_v_amend_thr2)
{
    u_srself_ang_amend1 srself_ang_amend1;

    srself_ang_amend1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset));
    srself_ang_amend1.bits.angle_v_amend_thr2 = angle_v_amend_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset), srself_ang_amend1.u32);

    return;
}

hi_void vdp_hipp_sr_setanglevamendthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_v_amend_thr1)
{
    u_srself_ang_amend1 srself_ang_amend1;

    srself_ang_amend1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset));
    srself_ang_amend1.bits.angle_v_amend_thr1 = angle_v_amend_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend1.u32) + offset), srself_ang_amend1.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelhamendweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_h_amend_weight1)
{
    u_srself_ang_amend2 srself_ang_amend2;

    srself_ang_amend2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset));
    srself_ang_amend2.bits.ang_rel_h_amend_weight1 = ang_rel_h_amend_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset), srself_ang_amend2.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelamendthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_amend_thr2)
{
    u_srself_ang_amend2 srself_ang_amend2;

    srself_ang_amend2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset));
    srself_ang_amend2.bits.ang_rel_amend_thr2 = ang_rel_amend_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset), srself_ang_amend2.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelamendthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_amend_thr1)
{
    u_srself_ang_amend2 srself_ang_amend2;

    srself_ang_amend2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset));
    srself_ang_amend2.bits.ang_rel_amend_thr1 = ang_rel_amend_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset), srself_ang_amend2.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelamendweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_amend_weight2)
{
    u_srself_ang_amend2 srself_ang_amend2;

    srself_ang_amend2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset));
    srself_ang_amend2.bits.ang_rel_amend_weight2 = ang_rel_amend_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend2.u32) + offset), srself_ang_amend2.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelhamendthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_h_amend_thr1)
{
    u_srself_ang_amend3 srself_ang_amend3;

    srself_ang_amend3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend3.u32) + offset));
    srself_ang_amend3.bits.ang_rel_h_amend_thr1 = ang_rel_h_amend_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend3.u32) + offset), srself_ang_amend3.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelhamendweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_h_amend_weight2)
{
    u_srself_ang_amend3 srself_ang_amend3;

    srself_ang_amend3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend3.u32) + offset));
    srself_ang_amend3.bits.ang_rel_h_amend_weight2 = ang_rel_h_amend_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend3.u32) + offset), srself_ang_amend3.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelamendslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_amend_slop)
{
    u_srself_ang_amend3 srself_ang_amend3;

    srself_ang_amend3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend3.u32) + offset));
    srself_ang_amend3.bits.ang_rel_amend_slop = ang_rel_amend_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend3.u32) + offset), srself_ang_amend3.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelvamendweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_v_amend_weight1)
{
    u_srself_ang_amend4 srself_ang_amend4;

    srself_ang_amend4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend4.u32) + offset));
    srself_ang_amend4.bits.ang_rel_v_amend_weight1 = ang_rel_v_amend_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend4.u32) + offset), srself_ang_amend4.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelhamendslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_h_amend_slop)
{
    u_srself_ang_amend4 srself_ang_amend4;

    srself_ang_amend4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend4.u32) + offset));
    srself_ang_amend4.bits.ang_rel_h_amend_slop = ang_rel_h_amend_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend4.u32) + offset), srself_ang_amend4.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelhamendthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_h_amend_thr2)
{
    u_srself_ang_amend4 srself_ang_amend4;

    srself_ang_amend4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend4.u32) + offset));
    srself_ang_amend4.bits.ang_rel_h_amend_thr2 = ang_rel_h_amend_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend4.u32) + offset), srself_ang_amend4.u32);

    return;
}

hi_void vdp_hipp_sr_setangdiffthr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_diff_thr)
{
    u_srself_ang_amend5 srself_ang_amend5;

    srself_ang_amend5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset));
    srself_ang_amend5.bits.ang_diff_thr = ang_diff_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset), srself_ang_amend5.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelvamendthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_v_amend_thr2)
{
    u_srself_ang_amend5 srself_ang_amend5;

    srself_ang_amend5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset));
    srself_ang_amend5.bits.ang_rel_v_amend_thr2 = ang_rel_v_amend_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset), srself_ang_amend5.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelvamendthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_v_amend_thr1)
{
    u_srself_ang_amend5 srself_ang_amend5;

    srself_ang_amend5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset));
    srself_ang_amend5.bits.ang_rel_v_amend_thr1 = ang_rel_v_amend_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset), srself_ang_amend5.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelvamendweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_v_amend_weight2)
{
    u_srself_ang_amend5 srself_ang_amend5;

    srself_ang_amend5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset));
    srself_ang_amend5.bits.ang_rel_v_amend_weight2 = ang_rel_v_amend_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_ang_amend5.u32) + offset), srself_ang_amend5.u32);

    return;
}

hi_void vdp_hipp_sr_setcfiltercoef00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cfilter_coef_00)
{
    u_srself_resbld0 srself_resbld0;

    srself_resbld0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld0.u32) + offset));
    srself_resbld0.bits.cfilter_coef_00 = cfilter_coef_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld0.u32) + offset), srself_resbld0.u32);

    return;
}

hi_void vdp_hipp_sr_setangleweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_weight1)
{
    u_srself_resbld0 srself_resbld0;

    srself_resbld0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld0.u32) + offset));
    srself_resbld0.bits.angle_weight1 = angle_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld0.u32) + offset), srself_resbld0.u32);

    return;
}

hi_void vdp_hipp_sr_setangrelvamendslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ang_rel_v_amend_slop)
{
    u_srself_resbld0 srself_resbld0;

    srself_resbld0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld0.u32) + offset));
    srself_resbld0.bits.ang_rel_v_amend_slop = ang_rel_v_amend_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld0.u32) + offset), srself_resbld0.u32);

    return;
}

hi_void vdp_hipp_sr_setangleweight4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_weight4)
{
    u_srself_resbld1 srself_resbld1;

    srself_resbld1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld1.u32) + offset));
    srself_resbld1.bits.angle_weight4 = angle_weight4;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld1.u32) + offset), srself_resbld1.u32);

    return;
}

hi_void vdp_hipp_sr_setangleweight3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_weight3)
{
    u_srself_resbld1 srself_resbld1;

    srself_resbld1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld1.u32) + offset));
    srself_resbld1.bits.angle_weight3 = angle_weight3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld1.u32) + offset), srself_resbld1.u32);

    return;
}

hi_void vdp_hipp_sr_setangleweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_weight2)
{
    u_srself_resbld1 srself_resbld1;

    srself_resbld1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld1.u32) + offset));
    srself_resbld1.bits.angle_weight2 = angle_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld1.u32) + offset), srself_resbld1.u32);

    return;
}

hi_void vdp_hipp_sr_setanglethr3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_thr3)
{
    u_srself_resbld2 srself_resbld2;

    srself_resbld2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld2.u32) + offset));
    srself_resbld2.bits.angle_thr3 = angle_thr3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld2.u32) + offset), srself_resbld2.u32);

    return;
}

hi_void vdp_hipp_sr_setanglethr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_thr2)
{
    u_srself_resbld2 srself_resbld2;

    srself_resbld2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld2.u32) + offset));
    srself_resbld2.bits.angle_thr2 = angle_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld2.u32) + offset), srself_resbld2.u32);

    return;
}

hi_void vdp_hipp_sr_setanglethr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_thr1)
{
    u_srself_resbld2 srself_resbld2;

    srself_resbld2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld2.u32) + offset));
    srself_resbld2.bits.angle_thr1 = angle_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld2.u32) + offset), srself_resbld2.u32);

    return;
}

hi_void vdp_hipp_sr_setangleslop1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_slop1)
{
    u_srself_resbld3 srself_resbld3;

    srself_resbld3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld3.u32) + offset));
    srself_resbld3.bits.angle_slop1 = angle_slop1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld3.u32) + offset), srself_resbld3.u32);

    return;
}

hi_void vdp_hipp_sr_setanglethr4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_thr4)
{
    u_srself_resbld3 srself_resbld3;

    srself_resbld3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld3.u32) + offset));
    srself_resbld3.bits.angle_thr4 = angle_thr4;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld3.u32) + offset), srself_resbld3.u32);

    return;
}

hi_void vdp_hipp_sr_setbpdiffweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bp_diff_weight1)
{
    u_srself_resbld4 srself_resbld4;

    srself_resbld4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld4.u32) + offset));
    srself_resbld4.bits.bp_diff_weight1 = bp_diff_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld4.u32) + offset), srself_resbld4.u32);

    return;
}

hi_void vdp_hipp_sr_setangleslop2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 angle_slop2)
{
    u_srself_resbld4 srself_resbld4;

    srself_resbld4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld4.u32) + offset));
    srself_resbld4.bits.angle_slop2 = angle_slop2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld4.u32) + offset), srself_resbld4.u32);

    return;
}

hi_void vdp_hipp_sr_setbpdiffthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bp_diff_thr2)
{
    u_srself_resbld5 srself_resbld5;

    srself_resbld5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld5.u32) + offset));
    srself_resbld5.bits.bp_diff_thr2 = bp_diff_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld5.u32) + offset), srself_resbld5.u32);

    return;
}

hi_void vdp_hipp_sr_setbpdiffthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bp_diff_thr1)
{
    u_srself_resbld5 srself_resbld5;

    srself_resbld5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld5.u32) + offset));
    srself_resbld5.bits.bp_diff_thr1 = bp_diff_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld5.u32) + offset), srself_resbld5.u32);

    return;
}

hi_void vdp_hipp_sr_setbpdiffweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bp_diff_weight2)
{
    u_srself_resbld5 srself_resbld5;

    srself_resbld5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld5.u32) + offset));
    srself_resbld5.bits.bp_diff_weight2 = bp_diff_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld5.u32) + offset), srself_resbld5.u32);

    return;
}

hi_void vdp_hipp_sr_setgradweight1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 grad_weight1)
{
    u_srself_resbld6 srself_resbld6;

    srself_resbld6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld6.u32) + offset));
    srself_resbld6.bits.grad_weight1 = grad_weight1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld6.u32) + offset), srself_resbld6.u32);

    return;
}

hi_void vdp_hipp_sr_setbpdiffslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bp_diff_slop)
{
    u_srself_resbld6 srself_resbld6;

    srself_resbld6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld6.u32) + offset));
    srself_resbld6.bits.bp_diff_slop = bp_diff_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld6.u32) + offset), srself_resbld6.u32);

    return;
}

hi_void vdp_hipp_sr_setgradthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 grad_thr2)
{
    u_srself_resbld7 srself_resbld7;

    srself_resbld7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld7.u32) + offset));
    srself_resbld7.bits.grad_thr2 = grad_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld7.u32) + offset), srself_resbld7.u32);

    return;
}

hi_void vdp_hipp_sr_setgradthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 grad_thr1)
{
    u_srself_resbld7 srself_resbld7;

    srself_resbld7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld7.u32) + offset));
    srself_resbld7.bits.grad_thr1 = grad_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld7.u32) + offset), srself_resbld7.u32);

    return;
}

hi_void vdp_hipp_sr_setgradweight2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 grad_weight2)
{
    u_srself_resbld7 srself_resbld7;

    srself_resbld7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld7.u32) + offset));
    srself_resbld7.bits.grad_weight2 = grad_weight2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld7.u32) + offset), srself_resbld7.u32);

    return;
}

hi_void vdp_hipp_sr_setcfiltercoef01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cfilter_coef_01)
{
    u_srself_resbld8 srself_resbld8;

    srself_resbld8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld8.u32) + offset));
    srself_resbld8.bits.cfilter_coef_01 = cfilter_coef_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld8.u32) + offset), srself_resbld8.u32);

    return;
}

hi_void vdp_hipp_sr_setgradslop(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 grad_slop)
{
    u_srself_resbld8 srself_resbld8;

    srself_resbld8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_resbld8.u32) + offset));
    srself_resbld8.bits.grad_slop = grad_slop;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_resbld8.u32) + offset), srself_resbld8.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda1coring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda1_coring)
{
    u_srself_cfilter0 srself_cfilter0;

    srself_cfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset));
    srself_cfilter0.bits.lamda1_coring = lamda1_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset), srself_cfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setcfiltercoef11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cfilter_coef_11)
{
    u_srself_cfilter0 srself_cfilter0;

    srself_cfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset));
    srself_cfilter0.bits.cfilter_coef_11 = cfilter_coef_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset), srself_cfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setcfiltercoef10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cfilter_coef_10)
{
    u_srself_cfilter0 srself_cfilter0;

    srself_cfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset));
    srself_cfilter0.bits.cfilter_coef_10 = cfilter_coef_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset), srself_cfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setcfiltercoef03(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cfilter_coef_03)
{
    u_srself_cfilter0 srself_cfilter0;

    srself_cfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset));
    srself_cfilter0.bits.cfilter_coef_03 = cfilter_coef_03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset), srself_cfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setcfiltercoef02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cfilter_coef_02)
{
    u_srself_cfilter0 srself_cfilter0;

    srself_cfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset));
    srself_cfilter0.bits.cfilter_coef_02 = cfilter_coef_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_cfilter0.u32) + offset), srself_cfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda1thr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda1_thr)
{
    u_srself_lamdal1 srself_lamdal1;

    srself_lamdal1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset));
    srself_lamdal1.bits.lamda1_thr = lamda1_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset), srself_lamdal1.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda1k2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda1_k2)
{
    u_srself_lamdal1 srself_lamdal1;

    srself_lamdal1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset));
    srself_lamdal1.bits.lamda1_k2 = lamda1_k2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset), srself_lamdal1.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda1k1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda1_k1)
{
    u_srself_lamdal1 srself_lamdal1;

    srself_lamdal1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset));
    srself_lamdal1.bits.lamda1_k1 = lamda1_k1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset), srself_lamdal1.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda1g1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda1_g1)
{
    u_srself_lamdal1 srself_lamdal1;

    srself_lamdal1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset));
    srself_lamdal1.bits.lamda1_g1 = lamda1_g1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal1.u32) + offset), srself_lamdal1.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda2thr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda2_thr)
{
    u_srself_lamdal2 srself_lamdal2;

    srself_lamdal2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset));
    srself_lamdal2.bits.lamda2_thr = lamda2_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset), srself_lamdal2.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda2k2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda2_k2)
{
    u_srself_lamdal2 srself_lamdal2;

    srself_lamdal2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset));
    srself_lamdal2.bits.lamda2_k2 = lamda2_k2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset), srself_lamdal2.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda2k1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda2_k1)
{
    u_srself_lamdal2 srself_lamdal2;

    srself_lamdal2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset));
    srself_lamdal2.bits.lamda2_k1 = lamda2_k1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset), srself_lamdal2.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda2g1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda2_g1)
{
    u_srself_lamdal2 srself_lamdal2;

    srself_lamdal2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset));
    srself_lamdal2.bits.lamda2_g1 = lamda2_g1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_lamdal2.u32) + offset), srself_lamdal2.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionhsumthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_hsum_thr1)
{
    u_srself_yfilter0 srself_yfilter0;

    srself_yfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset));
    srself_yfilter0.bits.bisection_hsum_thr1 = bisection_hsum_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset), srself_yfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionhsumthr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_hsum_thr0)
{
    u_srself_yfilter0 srself_yfilter0;

    srself_yfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset));
    srself_yfilter0.bits.bisection_hsum_thr0 = bisection_hsum_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset), srself_yfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setyfiltercoef02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 yfilter_coef_02)
{
    u_srself_yfilter0 srself_yfilter0;

    srself_yfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset));
    srself_yfilter0.bits.yfilter_coef_02 = yfilter_coef_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset), srself_yfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setyfiltercoef01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 yfilter_coef_01)
{
    u_srself_yfilter0 srself_yfilter0;

    srself_yfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset));
    srself_yfilter0.bits.yfilter_coef_01 = yfilter_coef_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset), srself_yfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setyfiltercoef00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 yfilter_coef_00)
{
    u_srself_yfilter0 srself_yfilter0;

    srself_yfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset));
    srself_yfilter0.bits.yfilter_coef_00 = yfilter_coef_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset), srself_yfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setlamda2coring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lamda2_coring)
{
    u_srself_yfilter0 srself_yfilter0;

    srself_yfilter0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset));
    srself_yfilter0.bits.lamda2_coring = lamda2_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter0.u32) + offset), srself_yfilter0.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionvsumthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_vsum_thr2)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.bisection_vsum_thr2 = bisection_vsum_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionvsumthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_vsum_thr1)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.bisection_vsum_thr1 = bisection_vsum_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionvsumthr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_vsum_thr0)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.bisection_vsum_thr0 = bisection_vsum_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionhsumthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_hsum_thr2)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.bisection_hsum_thr2 = bisection_hsum_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setyfiltercoef11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 yfilter_coef_11)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.yfilter_coef_11 = yfilter_coef_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setyfiltercoef10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 yfilter_coef_10)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.yfilter_coef_10 = yfilter_coef_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setyfiltercoef03(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 yfilter_coef_03)
{
    u_srself_yfilter1 srself_yfilter1;

    srself_yfilter1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset));
    srself_yfilter1.bits.yfilter_coef_03 = yfilter_coef_03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_yfilter1.u32) + offset), srself_yfilter1.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionvcountthr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_vcount_thr)
{
    u_srself_dsline srself_dsline;

    srself_dsline.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_dsline.u32) + offset));
    srself_dsline.bits.bisection_vcount_thr = bisection_vcount_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_dsline.u32) + offset), srself_dsline.u32);

    return;
}

hi_void vdp_hipp_sr_setbisectionhcountthr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bisection_hcount_thr)
{
    u_srself_dsline srself_dsline;

    srself_dsline.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_dsline.u32) + offset));
    srself_dsline.bits.bisection_hcount_thr = bisection_hcount_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_dsline.u32) + offset), srself_dsline.u32);

    return;
}

hi_void vdp_hipp_sr_setsr2dsrlsmoothdircoef(vdp_regs_type *vdp_reg, hi_u32 offset,
                                            hi_u32 sr_2dsr_l_smooth_dir_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_l_smooth_dir_coef = sr_2dsr_l_smooth_dir_coef;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void vdp_hipp_sr_setsr2dsrlsmoothnoncoef(vdp_regs_type *vdp_reg, hi_u32 offset,
                                            hi_u32 sr_2dsr_l_smooth_non_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_l_smooth_non_coef = sr_2dsr_l_smooth_non_coef;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void vdp_hipp_sr_setsr2dsrcdircoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_c_dir_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_c_dir_coef = sr_2dsr_c_dir_coef;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void vdp_hipp_sr_setsr2dsrcnoncoef(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_c_non_coef)
{
    u_srself_rd_coef_en srself_rd_coef_en;

    srself_rd_coef_en.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset));
    srself_rd_coef_en.bits.sr_2dsr_c_non_coef = sr_2dsr_c_non_coef;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_rd_coef_en.u32) + offset), srself_rd_coef_en.u32);

    return;
}

hi_void vdp_hipp_sr_setsr2dsrcoefdata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sr_2dsr_coef_data)
{
    u_srself_coef_rdata srself_coef_rdata;

    srself_coef_rdata.u32 = vdp_regread((uintptr_t)(&(vdp_reg->srself_coef_rdata.u32) + offset));
    srself_coef_rdata.bits.sr_2dsr_coef_data = sr_2dsr_coef_data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->srself_coef_rdata.u32) + offset), srself_coef_rdata.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_demo_en)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.dbsr_demo_en = dbsr_demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_demo_mode)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.dbsr_demo_mode = dbsr_demo_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setgraphsoften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 graph_soft_en)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.graph_soft_en = graph_soft_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setparamode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 para_mode)
{
    u_hipp_dbsr_ctrl hipp_dbsr_ctrl;

    hipp_dbsr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset));
    hipp_dbsr_ctrl.bits.para_mode = 0;  // para_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->hipp_dbsr_ctrl.u32) + offset), hipp_dbsr_ctrl.u32);

    return;
}

hi_void vdp_hipp_sr_setwsumlmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wsum_lmt)
{
    u_dbsr_wsum_coef dbsr_wsum_coef;

    dbsr_wsum_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wsum_coef.u32) + offset));
    dbsr_wsum_coef.bits.wsum_lmt = wsum_lmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wsum_coef.u32) + offset), dbsr_wsum_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setfixiwendbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fix_iw_en_dbsr)
{
    u_dbsr_pxl_wgt_coef dbsr_pxl_wgt_coef;

    dbsr_pxl_wgt_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset));
    dbsr_pxl_wgt_coef.bits.fix_iw_en_dbsr = fix_iw_en_dbsr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset), dbsr_pxl_wgt_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setfixiwdbsr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fix_iw_dbsr)
{
    u_dbsr_pxl_wgt_coef dbsr_pxl_wgt_coef;

    dbsr_pxl_wgt_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset));
    dbsr_pxl_wgt_coef.bits.fix_iw_dbsr = fix_iw_dbsr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_pxl_wgt_coef.u32) + offset), dbsr_pxl_wgt_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setcorekmagpdif(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 core_kmag_pdif)
{
    u_dbsr_core_pdif_coef dbsr_core_pdif_coef;

    dbsr_core_pdif_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset));
    dbsr_core_pdif_coef.bits.core_kmag_pdif = core_kmag_pdif;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset), dbsr_core_pdif_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setcoremadpdif(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 core_mad_pdif)
{
    u_dbsr_core_pdif_coef dbsr_core_pdif_coef;

    dbsr_core_pdif_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset));
    dbsr_core_pdif_coef.bits.core_mad_pdif = core_mad_pdif;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_core_pdif_coef.u32) + offset), dbsr_core_pdif_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setkdifof(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_dif_of)
{
    u_dbsr_cur_wgt_coef1 dbsr_cur_wgt_coef1;

    dbsr_cur_wgt_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset));
    dbsr_cur_wgt_coef1.bits.k_dif_of = k_dif_of;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset), dbsr_cur_wgt_coef1.u32);

    return;
}

hi_void vdp_hipp_sr_setxconfw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_confw)
{
    u_dbsr_cur_wgt_coef1 dbsr_cur_wgt_coef1;

    dbsr_cur_wgt_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset));
    dbsr_cur_wgt_coef1.bits.x_confw = x_confw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset), dbsr_cur_wgt_coef1.u32);

    return;
}

hi_void vdp_hipp_sr_setkconfw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_confw)
{
    u_dbsr_cur_wgt_coef1 dbsr_cur_wgt_coef1;

    dbsr_cur_wgt_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset));
    dbsr_cur_wgt_coef1.bits.k_confw = k_confw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef1.u32) + offset), dbsr_cur_wgt_coef1.u32);

    return;
}

hi_void vdp_hipp_sr_setxofw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_ofw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.x_ofw = x_ofw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setkofw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ofw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.k_ofw = k_ofw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setxpdifw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_pdifw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.x_pdifw = x_pdifw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setkpdifw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_pdifw)
{
    u_dbsr_cur_wgt_coef2 dbsr_cur_wgt_coef2;

    dbsr_cur_wgt_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset));
    dbsr_cur_wgt_coef2.bits.k_pdifw = k_pdifw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_cur_wgt_coef2.u32) + offset), dbsr_cur_wgt_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setx1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_mag_dbw)
{
    u_dbsr_mag_dbw_coefx dbsr_mag_dbw_coefx;

    dbsr_mag_dbw_coefx.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset));
    dbsr_mag_dbw_coefx.bits.x1_mag_dbw = x1_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset), dbsr_mag_dbw_coefx.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dbw)
{
    u_dbsr_mag_dbw_coefx dbsr_mag_dbw_coefx;

    dbsr_mag_dbw_coefx.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset));
    dbsr_mag_dbw_coefx.bits.x0_mag_dbw = x0_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefx.u32) + offset), dbsr_mag_dbw_coefx.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dbw)
{
    u_dbsr_mag_dbw_coefk0 dbsr_mag_dbw_coefk0;

    dbsr_mag_dbw_coefk0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset));
    dbsr_mag_dbw_coefk0.bits.k1_mag_dbw = k1_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset), dbsr_mag_dbw_coefk0.u32);

    return;
}

hi_void vdp_hipp_sr_setk0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k0_mag_dbw)
{
    u_dbsr_mag_dbw_coefk0 dbsr_mag_dbw_coefk0;

    dbsr_mag_dbw_coefk0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset));
    dbsr_mag_dbw_coefk0.bits.k0_mag_dbw = k0_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk0.u32) + offset), dbsr_mag_dbw_coefk0.u32);

    return;
}

hi_void vdp_hipp_sr_setk2magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_mag_dbw)
{
    u_dbsr_mag_dbw_coefk1 dbsr_mag_dbw_coefk1;

    dbsr_mag_dbw_coefk1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk1.u32) + offset));
    dbsr_mag_dbw_coefk1.bits.k2_mag_dbw = k2_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefk1.u32) + offset), dbsr_mag_dbw_coefk1.u32);

    return;
}

hi_void vdp_hipp_sr_setg2magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_mag_dbw)
{
    u_dbsr_mag_dbw_coefg dbsr_mag_dbw_coefg;

    dbsr_mag_dbw_coefg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset));
    dbsr_mag_dbw_coefg.bits.g2_mag_dbw = g2_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset), dbsr_mag_dbw_coefg.u32);

    return;
}

hi_void vdp_hipp_sr_setg1magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dbw)
{
    u_dbsr_mag_dbw_coefg dbsr_mag_dbw_coefg;

    dbsr_mag_dbw_coefg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset));
    dbsr_mag_dbw_coefg.bits.g1_mag_dbw = g1_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset), dbsr_mag_dbw_coefg.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magdbw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dbw)
{
    u_dbsr_mag_dbw_coefg dbsr_mag_dbw_coefg;

    dbsr_mag_dbw_coefg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset));
    dbsr_mag_dbw_coefg.bits.g0_mag_dbw = g0_mag_dbw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_coefg.u32) + offset), dbsr_mag_dbw_coefg.u32);

    return;
}

hi_void vdp_hipp_sr_setlmtconflst(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lmt_conf_lst)
{
    u_dbsr_lmt_conf_coef dbsr_lmt_conf_coef;

    dbsr_lmt_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset));
    dbsr_lmt_conf_coef.bits.lmt_conf_lst = lmt_conf_lst;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset), dbsr_lmt_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setklmtconf0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_lmt_conf0)
{
    u_dbsr_lmt_conf_coef dbsr_lmt_conf_coef;

    dbsr_lmt_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset));
    dbsr_lmt_conf_coef.bits.k_lmt_conf0 = k_lmt_conf0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_lmt_conf_coef.u32) + offset), dbsr_lmt_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_magk_conf)
{
    u_dbsr_magk_conf_coef dbsr_magk_conf_coef;

    dbsr_magk_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset));
    dbsr_magk_conf_coef.bits.k1_magk_conf = k1_magk_conf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset), dbsr_magk_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_magk_conf)
{
    u_dbsr_magk_conf_coef dbsr_magk_conf_coef;

    dbsr_magk_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset));
    dbsr_magk_conf_coef.bits.g0_magk_conf = g0_magk_conf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset), dbsr_magk_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magkconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_magk_conf)
{
    u_dbsr_magk_conf_coef dbsr_magk_conf_coef;

    dbsr_magk_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset));
    dbsr_magk_conf_coef.bits.x0_magk_conf = x0_magk_conf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_magk_conf_coef.u32) + offset), dbsr_magk_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_magw_conf)
{
    u_dbsr_magw_conf_coef dbsr_magw_conf_coef;

    dbsr_magw_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset));
    dbsr_magw_conf_coef.bits.k1_magw_conf = k1_magw_conf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset), dbsr_magw_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_magw_conf)
{
    u_dbsr_magw_conf_coef dbsr_magw_conf_coef;

    dbsr_magw_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset));
    dbsr_magw_conf_coef.bits.g0_magw_conf = g0_magw_conf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset), dbsr_magw_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magwconf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_magw_conf)
{
    u_dbsr_magw_conf_coef dbsr_magw_conf_coef;

    dbsr_magw_conf_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset));
    dbsr_magw_conf_coef.bits.x0_magw_conf = x0_magw_conf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_magw_conf_coef.u32) + offset), dbsr_magw_conf_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setx1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefx dbsr_mag_dbw_bld_coefx;

    dbsr_mag_dbw_bld_coefx.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset));
    dbsr_mag_dbw_bld_coefx.bits.x1_mag_dbw_bld = x1_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset), dbsr_mag_dbw_bld_coefx.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefx dbsr_mag_dbw_bld_coefx;

    dbsr_mag_dbw_bld_coefx.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset));
    dbsr_mag_dbw_bld_coefx.bits.x0_mag_dbw_bld = x0_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefx.u32) + offset), dbsr_mag_dbw_bld_coefx.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefk0 dbsr_mag_dbw_bld_coefk0;

    dbsr_mag_dbw_bld_coefk0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset));
    dbsr_mag_dbw_bld_coefk0.bits.k1_mag_dbw_bld = k1_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset), dbsr_mag_dbw_bld_coefk0.u32);

    return;
}

hi_void vdp_hipp_sr_setk0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k0_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefk0 dbsr_mag_dbw_bld_coefk0;

    dbsr_mag_dbw_bld_coefk0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset));
    dbsr_mag_dbw_bld_coefk0.bits.k0_mag_dbw_bld = k0_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk0.u32) + offset), dbsr_mag_dbw_bld_coefk0.u32);

    return;
}

hi_void vdp_hipp_sr_setk2magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefk1 dbsr_mag_dbw_bld_coefk1;

    dbsr_mag_dbw_bld_coefk1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk1.u32) + offset));
    dbsr_mag_dbw_bld_coefk1.bits.k2_mag_dbw_bld = k2_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefk1.u32) + offset), dbsr_mag_dbw_bld_coefk1.u32);

    return;
}

hi_void vdp_hipp_sr_setg2magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefg dbsr_mag_dbw_bld_coefg;

    dbsr_mag_dbw_bld_coefg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset));
    dbsr_mag_dbw_bld_coefg.bits.g2_mag_dbw_bld = g2_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset), dbsr_mag_dbw_bld_coefg.u32);

    return;
}

hi_void vdp_hipp_sr_setg1magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefg dbsr_mag_dbw_bld_coefg;

    dbsr_mag_dbw_bld_coefg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset));
    dbsr_mag_dbw_bld_coefg.bits.g1_mag_dbw_bld = g1_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset), dbsr_mag_dbw_bld_coefg.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dbw_bld)
{
    u_dbsr_mag_dbw_bld_coefg dbsr_mag_dbw_bld_coefg;

    dbsr_mag_dbw_bld_coefg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset));
    dbsr_mag_dbw_bld_coefg.bits.g0_mag_dbw_bld = g0_mag_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dbw_bld_coefg.u32) + offset), dbsr_mag_dbw_bld_coefg.u32);

    return;
}

hi_void vdp_hipp_sr_setkbpdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_bp_dbw_bld)
{
    u_dbsr_bp_dbw_bld_coef dbsr_bp_dbw_bld_coef;

    dbsr_bp_dbw_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset));
    dbsr_bp_dbw_bld_coef.bits.k_bp_dbw_bld = k_bp_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset), dbsr_bp_dbw_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setxbpdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_bp_dbw_bld)
{
    u_dbsr_bp_dbw_bld_coef dbsr_bp_dbw_bld_coef;

    dbsr_bp_dbw_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset));
    dbsr_bp_dbw_bld_coef.bits.x_bp_dbw_bld = x_bp_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_dbw_bld_coef.u32) + offset), dbsr_bp_dbw_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg1magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.g1_mag_dirw_adj = g1_mag_dirw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.k1_mag_dirw_adj = k1_mag_dirw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.g0_mag_dirw_adj = g0_mag_dirw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_dirw_adj)
{
    u_dbsr_mag_dirw_adj_coef dbsr_mag_dirw_adj_coef;

    dbsr_mag_dirw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset));
    dbsr_mag_dirw_adj_coef.bits.x0_mag_dirw_adj = x0_mag_dirw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_dirw_adj_coef.u32) + offset), dbsr_mag_dirw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg1magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.g1_mag_ssw_adj = g1_mag_ssw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.k1_mag_ssw_adj = k1_mag_ssw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.g0_mag_ssw_adj = g0_mag_ssw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_ssw_adj)
{
    u_dbsr_mag_ssw_adj_coef dbsr_mag_ssw_adj_coef;

    dbsr_mag_ssw_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset));
    dbsr_mag_ssw_adj_coef.bits.x0_mag_ssw_adj = x0_mag_ssw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_adj_coef.u32) + offset), dbsr_mag_ssw_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg1magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.g1_mag_ssw_kadj = g1_mag_ssw_kadj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setk1magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.k1_mag_ssw_kadj = k1_mag_ssw_kadj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setg0magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.g0_mag_ssw_kadj = g0_mag_ssw_kadj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setx0magsswkadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_mag_ssw_kadj)
{
    u_dbsr_mag_ssw_kadj_coef dbsr_mag_ssw_kadj_coef;

    dbsr_mag_ssw_kadj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset));
    dbsr_mag_ssw_kadj_coef.bits.x0_mag_ssw_kadj = x0_mag_ssw_kadj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_mag_ssw_kadj_coef.u32) + offset), dbsr_mag_ssw_kadj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setksswdirwadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_dirw_adj)
{
    u_dbsr_wgt_adj_coef dbsr_wgt_adj_coef;

    dbsr_wgt_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset));
    dbsr_wgt_adj_coef.bits.k_ssw_dirw_adj = k_ssw_dirw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset), dbsr_wgt_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setksswsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_ssw_adj)
{
    u_dbsr_wgt_adj_coef dbsr_wgt_adj_coef;

    dbsr_wgt_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset));
    dbsr_wgt_adj_coef.bits.k_ssw_ssw_adj = k_ssw_ssw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset), dbsr_wgt_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setxdbwsswadj(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_dbw_ssw_adj)
{
    u_dbsr_wgt_adj_coef dbsr_wgt_adj_coef;

    dbsr_wgt_adj_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset));
    dbsr_wgt_adj_coef.bits.x_dbw_ssw_adj = x_dbw_ssw_adj;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_adj_coef.u32) + offset), dbsr_wgt_adj_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setkminwdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_minw_dbw_bld)
{
    u_dbsr_wgt_bld_coef dbsr_wgt_bld_coef;

    dbsr_wgt_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset));
    dbsr_wgt_bld_coef.bits.k_minw_dbw_bld = k_minw_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset), dbsr_wgt_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setkidbwsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_idbw_ssw_bld)
{
    u_dbsr_wgt_bld_coef dbsr_wgt_bld_coef;

    dbsr_wgt_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset));
    dbsr_wgt_bld_coef.bits.k_idbw_ssw_bld = k_idbw_ssw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset), dbsr_wgt_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setxidbwsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_idbw_ssw_bld)
{
    u_dbsr_wgt_bld_coef dbsr_wgt_bld_coef;

    dbsr_wgt_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset));
    dbsr_wgt_bld_coef.bits.x_idbw_ssw_bld = x_idbw_ssw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_wgt_bld_coef.u32) + offset), dbsr_wgt_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setklpfbpdb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_lpf_bp_db)
{
    u_dbsr_bp_db_coef dbsr_bp_db_coef;

    dbsr_bp_db_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset));
    dbsr_bp_db_coef.bits.k_lpf_bp_db = k_lpf_bp_db;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset), dbsr_bp_db_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setcoringbpdb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 coring_bp_db)
{
    u_dbsr_bp_db_coef dbsr_bp_db_coef;

    dbsr_bp_db_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset));
    dbsr_bp_db_coef.bits.coring_bp_db = coring_bp_db;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bp_db_coef.u32) + offset), dbsr_bp_db_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setksswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_ssw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.k_ssw_bld = k_ssw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setbsswbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_ssw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.b_ssw_bld = b_ssw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setkdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k_dbw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.k_dbw_bld = k_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setbdbwbld(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_dbw_bld)
{
    u_dbsr_bld_coef dbsr_bld_coef;

    dbsr_bld_coef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset));
    dbsr_bld_coef.bits.b_dbw_bld = b_dbw_bld;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bld_coef.u32) + offset), dbsr_bld_coef.u32);

    return;
}

hi_void vdp_hipp_sr_setc0graphsplit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c0_graph_split)
{
    u_dbsr_graph_wgt_cal_coef_split dbsr_graph_wgt_cal_coef_split;

    dbsr_graph_wgt_cal_coef_split.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) +
                                                     offset));
    dbsr_graph_wgt_cal_coef_split.bits.c0_graph_split = c0_graph_split;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) + offset),
                 dbsr_graph_wgt_cal_coef_split.u32);

    return;
}

hi_void vdp_hipp_sr_setc1graphsplit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c1_graph_split)
{
    u_dbsr_graph_wgt_cal_coef_split dbsr_graph_wgt_cal_coef_split;

    dbsr_graph_wgt_cal_coef_split.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) +
                                                     offset));
    dbsr_graph_wgt_cal_coef_split.bits.c1_graph_split = c1_graph_split;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef_split.u32) + offset),
                 dbsr_graph_wgt_cal_coef_split.u32);

    return;
}

hi_void vdp_hipp_sr_setc0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c0_graph)
{
    u_dbsr_graph_wgt_cal_coef0 dbsr_graph_wgt_cal_coef0;

    dbsr_graph_wgt_cal_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset));
    dbsr_graph_wgt_cal_coef0.bits.c0_graph = c0_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset), dbsr_graph_wgt_cal_coef0.u32);

    return;
}

hi_void vdp_hipp_sr_setc1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c1_graph)
{
    u_dbsr_graph_wgt_cal_coef0 dbsr_graph_wgt_cal_coef0;

    dbsr_graph_wgt_cal_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset));
    dbsr_graph_wgt_cal_coef0.bits.c1_graph = c1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef0.u32) + offset), dbsr_graph_wgt_cal_coef0.u32);

    return;
}

hi_void vdp_hipp_sr_setr0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r0_graph)
{
    u_dbsr_graph_wgt_cal_coef1 dbsr_graph_wgt_cal_coef1;

    dbsr_graph_wgt_cal_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset));
    dbsr_graph_wgt_cal_coef1.bits.r0_graph = r0_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset), dbsr_graph_wgt_cal_coef1.u32);

    return;
}

hi_void vdp_hipp_sr_setr1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r1_graph)
{
    u_dbsr_graph_wgt_cal_coef1 dbsr_graph_wgt_cal_coef1;

    dbsr_graph_wgt_cal_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset));
    dbsr_graph_wgt_cal_coef1.bits.r1_graph = r1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef1.u32) + offset), dbsr_graph_wgt_cal_coef1.u32);

    return;
}

hi_void vdp_hipp_sr_setdifth0graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth0_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth0_graph = difth0_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setdifth1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth1_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth1_graph = difth1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setdifth2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth2_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth2_graph = difth2_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setdifth3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 difth3_graph)
{
    u_dbsr_graph_wgt_cal_coef2 dbsr_graph_wgt_cal_coef2;

    dbsr_graph_wgt_cal_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset));
    dbsr_graph_wgt_cal_coef2.bits.difth3_graph = difth3_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef2.u32) + offset), dbsr_graph_wgt_cal_coef2.u32);

    return;
}

hi_void vdp_hipp_sr_setcorek1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek1_graph)
{
    u_dbsr_graph_wgt_cal_coef3 dbsr_graph_wgt_cal_coef3;

    dbsr_graph_wgt_cal_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset));
    dbsr_graph_wgt_cal_coef3.bits.corek1_graph = corek1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset), dbsr_graph_wgt_cal_coef3.u32);

    return;
}

hi_void vdp_hipp_sr_setcorek2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek2_graph)
{
    u_dbsr_graph_wgt_cal_coef3 dbsr_graph_wgt_cal_coef3;

    dbsr_graph_wgt_cal_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset));
    dbsr_graph_wgt_cal_coef3.bits.corek2_graph = corek2_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset), dbsr_graph_wgt_cal_coef3.u32);

    return;
}

hi_void vdp_hipp_sr_setcorek3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 corek3_graph)
{
    u_dbsr_graph_wgt_cal_coef3 dbsr_graph_wgt_cal_coef3;

    dbsr_graph_wgt_cal_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset));
    dbsr_graph_wgt_cal_coef3.bits.corek3_graph = corek3_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef3.u32) + offset), dbsr_graph_wgt_cal_coef3.u32);

    return;
}

hi_void vdp_hipp_sr_setx0kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x0_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef4 dbsr_graph_wgt_cal_coef4;

    dbsr_graph_wgt_cal_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset));
    dbsr_graph_wgt_cal_coef4.bits.x0_kk_graph = x0_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset), dbsr_graph_wgt_cal_coef4.u32);

    return;
}

hi_void vdp_hipp_sr_setx1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x1_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef4 dbsr_graph_wgt_cal_coef4;

    dbsr_graph_wgt_cal_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset));
    dbsr_graph_wgt_cal_coef4.bits.x1_kk_graph = x1_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset), dbsr_graph_wgt_cal_coef4.u32);

    return;
}

hi_void vdp_hipp_sr_setx2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x2_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef4 dbsr_graph_wgt_cal_coef4;

    dbsr_graph_wgt_cal_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset));
    dbsr_graph_wgt_cal_coef4.bits.x2_kk_graph = x2_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef4.u32) + offset), dbsr_graph_wgt_cal_coef4.u32);

    return;
}

hi_void vdp_hipp_sr_setg1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g1_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef5 dbsr_graph_wgt_cal_coef5;

    dbsr_graph_wgt_cal_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset));
    dbsr_graph_wgt_cal_coef5.bits.g1_kk_graph = g1_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset), dbsr_graph_wgt_cal_coef5.u32);

    return;
}

hi_void vdp_hipp_sr_setg2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g2_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef5 dbsr_graph_wgt_cal_coef5;

    dbsr_graph_wgt_cal_coef5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset));
    dbsr_graph_wgt_cal_coef5.bits.g2_kk_graph = g2_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef5.u32) + offset), dbsr_graph_wgt_cal_coef5.u32);

    return;
}

hi_void vdp_hipp_sr_setg0kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g0_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef6 dbsr_graph_wgt_cal_coef6;

    dbsr_graph_wgt_cal_coef6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef6.u32) + offset));
    dbsr_graph_wgt_cal_coef6.bits.g0_kk_graph = g0_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef6.u32) + offset), dbsr_graph_wgt_cal_coef6.u32);

    return;
}

hi_void vdp_hipp_sr_setk3kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k3_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef7 dbsr_graph_wgt_cal_coef7;

    dbsr_graph_wgt_cal_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset));
    dbsr_graph_wgt_cal_coef7.bits.k3_kk_graph = k3_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset), dbsr_graph_wgt_cal_coef7.u32);

    return;
}

hi_void vdp_hipp_sr_setg3kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 g3_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef7 dbsr_graph_wgt_cal_coef7;

    dbsr_graph_wgt_cal_coef7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset));
    dbsr_graph_wgt_cal_coef7.bits.g3_kk_graph = g3_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef7.u32) + offset), dbsr_graph_wgt_cal_coef7.u32);

    return;
}

hi_void vdp_hipp_sr_setk1kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef8 dbsr_graph_wgt_cal_coef8;

    dbsr_graph_wgt_cal_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset));
    dbsr_graph_wgt_cal_coef8.bits.k1_kk_graph = k1_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset), dbsr_graph_wgt_cal_coef8.u32);

    return;
}

hi_void vdp_hipp_sr_setk2kkgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_kk_graph)
{
    u_dbsr_graph_wgt_cal_coef8 dbsr_graph_wgt_cal_coef8;

    dbsr_graph_wgt_cal_coef8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset));
    dbsr_graph_wgt_cal_coef8.bits.k2_kk_graph = k2_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef8.u32) + offset), dbsr_graph_wgt_cal_coef8.u32);

    return;
}

hi_void vdp_hipp_sr_setk1graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k1_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.k1_graph = k1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void vdp_hipp_sr_setk2graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k2_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.k2_graph = k2_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void vdp_hipp_sr_setk3graph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 k3_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.k3_graph = k3_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void vdp_hipp_sr_setwgraph(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_graph)
{
    u_dbsr_graph_wgt_cal_coef9 dbsr_graph_wgt_cal_coef9;

    dbsr_graph_wgt_cal_coef9.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset));
    dbsr_graph_wgt_cal_coef9.bits.w_graph = w_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_graph_wgt_cal_coef9.u32) + offset), dbsr_graph_wgt_cal_coef9.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase03(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase03)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase03 = dbsr_bicubic_phase03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase02)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase02 = dbsr_bicubic_phase02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase01)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase01 = dbsr_bicubic_phase01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase00)
{
    u_dbsr_bicubic_coef_0 dbsr_bicubic_coef_0;

    dbsr_bicubic_coef_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset));
    dbsr_bicubic_coef_0.bits.dbsr_bicubic_phase00 = dbsr_bicubic_phase00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_0.u32) + offset), dbsr_bicubic_coef_0.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase13(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase13)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase13 = dbsr_bicubic_phase13;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase12)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase12 = dbsr_bicubic_phase12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase11)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase11 = dbsr_bicubic_phase11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrbicubicphase10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_bicubic_phase10)
{
    u_dbsr_bicubic_coef_1 dbsr_bicubic_coef_1;

    dbsr_bicubic_coef_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset));
    dbsr_bicubic_coef_1.bits.dbsr_bicubic_phase10 = dbsr_bicubic_phase10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_bicubic_coef_1.u32) + offset), dbsr_bicubic_coef_1.u32);

    return;
}

hi_void vdp_hipp_sr_setopdstdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_pdst_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.o_pdst_dneed = o_pdst_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setopdstdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_pdst_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.o_pdst_drdy = o_pdst_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setiplrydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_plry_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_plry_dneed = i_plry_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setiplrydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_plry_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_plry_drdy = i_plry_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setipsrcylowdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_low_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_low_dneed = i_psrcy_low_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setipsrcylowdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_low_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_low_drdy = i_psrcy_low_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setipsrcyhighdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_high_dneed)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_high_dneed = i_psrcy_high_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setipsrcyhighdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_psrcy_high_drdy)
{
    u_dbsr_debug0 dbsr_debug0;

    dbsr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset));
    dbsr_debug0.bits.i_psrcy_high_drdy = i_psrcy_high_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug0.u32) + offset), dbsr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setwgtavgcnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wgt_avg_cnt_y)
{
    u_dbsr_debug1 dbsr_debug1;

    dbsr_debug1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset));
    dbsr_debug1.bits.wgt_avg_cnt_y = wgt_avg_cnt_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset), dbsr_debug1.u32);

    return;
}

hi_void vdp_hipp_sr_setwgtavgcntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wgt_avg_cnt_x)
{
    u_dbsr_debug1 dbsr_debug1;

    dbsr_debug1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset));
    dbsr_debug1.bits.wgt_avg_cnt_x = wgt_avg_cnt_x;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_debug1.u32) + offset), dbsr_debug1.u32);

    return;
}

hi_void vdp_hipp_sr_setdbsrparacoefdata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbsr_para_coef_data)
{
    u_dbsr_coef_data dbsr_coef_data;

    dbsr_coef_data.u32 = vdp_regread((uintptr_t)(&(vdp_reg->dbsr_coef_data.u32) + offset));
    dbsr_coef_data.bits.dbsr_para_coef_data = dbsr_para_coef_data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->dbsr_coef_data.u32) + offset), dbsr_coef_data.u32);

    return;
}

hi_void vdp_hipp_sr_setosrcdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_c_dneed = o_sr_c_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setosrcdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_c_drdy = o_sr_c_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setosrydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_y_dneed = o_sr_y_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setosrydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.o_sr_y_drdy = o_sr_y_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_seticlmcdneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_c_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_c_dneed = i_clm_c_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_seticlmcdrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_c_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_c_drdy = i_clm_c_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_seticlmydneed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_y_dneed)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_y_dneed = i_clm_y_dneed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_seticlmydrdy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 i_clm_y_drdy)
{
    u_sr_debug0 sr_debug0;

    sr_debug0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset));
    sr_debug0.bits.i_clm_y_drdy = i_clm_y_drdy;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug0.u32) + offset), sr_debug0.u32);

    return;
}

hi_void vdp_hipp_sr_setosrycnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_cnt_y)
{
    u_sr_debug1 sr_debug1;

    sr_debug1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset));
    sr_debug1.bits.o_sr_y_cnt_y = o_sr_y_cnt_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset), sr_debug1.u32);

    return;
}

hi_void vdp_hipp_sr_setosrycntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_y_cnt_x)
{
    u_sr_debug1 sr_debug1;

    sr_debug1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset));
    sr_debug1.bits.o_sr_y_cnt_x = o_sr_y_cnt_x;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug1.u32) + offset), sr_debug1.u32);

    return;
}

hi_void vdp_hipp_sr_setosrccnty(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_cnt_y)
{
    u_sr_debug2 sr_debug2;

    sr_debug2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset));
    sr_debug2.bits.o_sr_c_cnt_y = o_sr_c_cnt_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset), sr_debug2.u32);

    return;
}

hi_void vdp_hipp_sr_setosrccntx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 o_sr_c_cnt_x)
{
    u_sr_debug2 sr_debug2;

    sr_debug2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset));
    sr_debug2.bits.o_sr_c_cnt_x = o_sr_c_cnt_x;
    vdp_regwrite((uintptr_t)(&(vdp_reg->sr_debug2.u32) + offset), sr_debug2.u32);

    return;
}

#if 1
hi_void vdp_hipp_sr_setwgraphrtl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_graph_rtl)
{
    u_graph_rtl graph_rtl;

    graph_rtl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->graph_rtl.u32) + offset));
    graph_rtl.bits.w_graph_rtl = w_graph_rtl;
    vdp_regwrite((uintptr_t)(&(vdp_reg->graph_rtl.u32) + offset), graph_rtl.u32);

    return;
}
#endif

hi_void vdp_hipp_sr_setdifstatraw0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw0 difstat_raw0;

    difstat_raw0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->difstat_raw0.u32) + offset));
    difstat_raw0.bits.difstat_raw0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->difstat_raw0.u32) + offset), difstat_raw0.u32);

    return;
}

hi_void vdp_hipp_sr_setdifstatraw1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw1 difstat_raw1;

    difstat_raw1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->difstat_raw1.u32) + offset));
    difstat_raw1.bits.difstat_raw1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->difstat_raw1.u32) + offset), difstat_raw1.u32);

    return;
}

hi_void vdp_hipp_sr_setdifstatraw2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw2 difstat_raw2;

    difstat_raw2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->difstat_raw2.u32) + offset));
    difstat_raw2.bits.difstat_raw2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->difstat_raw2.u32) + offset), difstat_raw2.u32);

    return;
}

hi_void vdp_hipp_sr_setdifstatraw3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_difstat_raw3 difstat_raw3;

    difstat_raw3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->difstat_raw3.u32) + offset));
    difstat_raw3.bits.difstat_raw3 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->difstat_raw3.u32) + offset), difstat_raw3.u32);

    return;
}


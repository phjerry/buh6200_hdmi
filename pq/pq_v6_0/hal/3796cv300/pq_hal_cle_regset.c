/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: cle register source file
 * Author: pq
 * Create: 2019-09-21
 */
#include "pq_hal_comm.h"
#include "pq_hal_cle_regset.h"
#include "pq_hal_cle.h"

hi_void pq_reg_cle_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = (1 << (u32chnnum - CLE_PARAUP_COEF_OFFSET));
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

hi_void pq_reg_cle_set_8k_cle_para_src(vdp_regs_type *vdp_reg, hi_u32 src_mode)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = pq_new_regread((uintptr_t)&(vdp_reg->v0_ctrl.u32));
    v0_ctrl.bits.sr_8k_cle_para_src = src_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);

    return;
}

hi_void pq_reg_cle_para_addr_v0_chn20(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn20 para_addr_v0_chn20;

    para_addr_v0_chn20.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_addr_v0_chn20.u32));
    para_addr_v0_chn20.bits.para_addr_v0_chn20 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn20.u32)), para_addr_v0_chn20.u32);

    return;
}

hi_void pq_reg_cle_para_addr_v0_chn22(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn22 para_addr_v0_chn22;

    para_addr_v0_chn22.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_addr_v0_chn22.u32));
    para_addr_v0_chn22.bits.para_addr_v0_chn22 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn22.u32)), para_addr_v0_chn22.u32);

    return;
}

/* ***********************************CLE IP REG**************************************************** */
hi_void pq_reg_cle_set_lti_limit_clip_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 lti_limit_clip_en)
{
    u_hipp_cle_reg0 hipp_cle_reg0;

    hipp_cle_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr));
    hipp_cle_reg0.bits.lti_limit_clip_en = lti_limit_clip_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr), hipp_cle_reg0.u32);

    return;
}

hi_void pq_reg_cle_set_enltih(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 enltih)
{
    u_hipp_cle_reg0 hipp_cle_reg0;

    hipp_cle_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr));
    hipp_cle_reg0.bits.enltih = enltih;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr), hipp_cle_reg0.u32);

    return;
}

hi_void pq_reg_cle_set_cle_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 cle_en)
{
    u_hipp_cle_reg0 hipp_cle_reg0;

    hipp_cle_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr));
    hipp_cle_reg0.bits.cle_en = cle_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr), hipp_cle_reg0.u32);

    return;
}

hi_void pq_reg_cle_set_demo_pos(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 demo_pos)
{
    u_hipp_cle_reg0 hipp_cle_reg0;

    hipp_cle_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr));
    hipp_cle_reg0.bits.demo_pos = demo_pos;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr), hipp_cle_reg0.u32);

    return;
}

hi_void pq_reg_cle_set_demo_mode(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 demo_mode)
{
    u_hipp_cle_reg0 hipp_cle_reg0;

    hipp_cle_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr));
    hipp_cle_reg0.bits.demo_mode = demo_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr), hipp_cle_reg0.u32);

    return;
}

hi_void pq_reg_cle_set_demo_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 demo_en)
{
    u_hipp_cle_reg0 hipp_cle_reg0;

    hipp_cle_reg0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr));
    hipp_cle_reg0.bits.demo_en = demo_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg0.u32) + offset_addr), hipp_cle_reg0.u32);

    return;
}

hi_void pq_reg_cle_set_raisr_gain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 raisr_gain)
{
    u_hipp_cle_reg52 hipp_cle_reg52;

    hipp_cle_reg52.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg52.u32) + offset_addr));
    hipp_cle_reg52.bits.raisr_gain = raisr_gain;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg52.u32) + offset_addr), hipp_cle_reg52.u32);

    return;
}

hi_void pq_reg_cle_set_nlsr_gain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 nlsr_gain)
{
    u_hipp_cle_reg53 hipp_cle_reg53;

    hipp_cle_reg53.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg53.u32) + offset_addr));
    hipp_cle_reg53.bits.nlsr_gain = nlsr_gain;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg53.u32) + offset_addr), hipp_cle_reg53.u32);

    return;
}

hi_void pq_reg_cle_set_peakgain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 peakgain)
{
    u_hipp_cle_reg129 hipp_cle_reg129;
    hipp_cle_reg129.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg129.u32) + offset_addr));
    hipp_cle_reg129.bits.peakgain = peakgain;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg129.u32) + offset_addr), hipp_cle_reg129.u32);

    return;
}

hi_void pq_reg_cle_set_shp_clut_rd_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 shp_clut_rd_en)
{
    u_hipp_cle_reg2 hipp_cle_reg2;

    hipp_cle_reg2.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg2.u32) + offset_addr));
    hipp_cle_reg2.bits.shp_clut_rd_en = shp_clut_rd_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg2.u32) + offset_addr), hipp_cle_reg2.u32);

    return;
}

hi_void pq_reg_cle_set_clut_addr(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 clut_addr)
{
    u_hipp_cle_reg3 hipp_cle_reg3;

    hipp_cle_reg3.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg3.u32) + offset_addr));
    hipp_cle_reg3.bits.clut_addr = clut_addr;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg3.u32) + offset_addr), hipp_cle_reg3.u32);

    return;
}

hi_void pq_reg_cle_set_clut_rdata(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 clut_rdata)
{
    u_hipp_cle_reg4 hipp_cle_reg4;

    hipp_cle_reg4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg4.u32) + offset_addr));
    hipp_cle_reg4.bits.clut_rdata = clut_rdata;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg4.u32) + offset_addr), hipp_cle_reg4.u32);

    return;
}

hi_void pq_reg_cle_set_face_num(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 face_num)
{
    u_hipp_cle_reg5 hipp_cle_reg5;

    hipp_cle_reg5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr));
    hipp_cle_reg5.bits.face_num = face_num;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr), hipp_cle_reg5.u32);

    return;
}

hi_void pq_reg_cle_set_face_confidence_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 face_confidence_en)
{
    u_hipp_cle_reg5 hipp_cle_reg5;

    hipp_cle_reg5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr));
    hipp_cle_reg5.bits.face_confidence_en = face_confidence_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr), hipp_cle_reg5.u32);

    return;
}

hi_void pq_reg_cle_set_cle_ck_gt_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 cle_ck_gt_en)
{
    u_hipp_cle_reg5 hipp_cle_reg5;

    hipp_cle_reg5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr));
    hipp_cle_reg5.bits.cle_ck_gt_en = cle_ck_gt_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr), hipp_cle_reg5.u32);

    return;
}

hi_void pq_reg_cle_set_peak_limit_clip_en(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 peak_limit_clip_en)
{
    u_hipp_cle_reg5 hipp_cle_reg5;

    hipp_cle_reg5.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr));
    hipp_cle_reg5.bits.peak_limit_clip_en = peak_limit_clip_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg5.u32) + offset_addr), hipp_cle_reg5.u32);

    return;
}

hi_void pq_reg_cle_set_facepos01(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_1)
{
    u_hipp_cle_reg6 hipp_cle_reg6;

    hipp_cle_reg6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg6.u32) + offset_addr));
    hipp_cle_reg6.bits.facepos0_1 = facepos0_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg6.u32) + offset_addr), hipp_cle_reg6.u32);

    return;
}

hi_void pq_reg_cle_set_facepos00(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_0)
{
    u_hipp_cle_reg6 hipp_cle_reg6;

    hipp_cle_reg6.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg6.u32) + offset_addr));
    hipp_cle_reg6.bits.facepos0_0 = facepos0_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg6.u32) + offset_addr), hipp_cle_reg6.u32);

    return;
}

hi_void pq_reg_cle_set_facepos04(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_4)
{
    u_hipp_cle_reg7 hipp_cle_reg7;

    hipp_cle_reg7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg7.u32) + offset_addr));
    hipp_cle_reg7.bits.facepos0_4 = facepos0_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg7.u32) + offset_addr), hipp_cle_reg7.u32);

    return;
}

hi_void pq_reg_cle_set_facepos03(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_3)
{
    u_hipp_cle_reg7 hipp_cle_reg7;

    hipp_cle_reg7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg7.u32) + offset_addr));
    hipp_cle_reg7.bits.facepos0_3 = facepos0_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg7.u32) + offset_addr), hipp_cle_reg7.u32);

    return;
}

hi_void pq_reg_cle_set_facepos02(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos0_2)
{
    u_hipp_cle_reg7 hipp_cle_reg7;

    hipp_cle_reg7.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg7.u32) + offset_addr));
    hipp_cle_reg7.bits.facepos0_2 = facepos0_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg7.u32) + offset_addr), hipp_cle_reg7.u32);

    return;
}

hi_void pq_reg_cle_set_facepos11(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_1)
{
    u_hipp_cle_reg8 hipp_cle_reg8;

    hipp_cle_reg8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg8.u32) + offset_addr));
    hipp_cle_reg8.bits.facepos1_1 = facepos1_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg8.u32) + offset_addr), hipp_cle_reg8.u32);

    return;
}

hi_void pq_reg_cle_set_facepos10(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_0)
{
    u_hipp_cle_reg8 hipp_cle_reg8;

    hipp_cle_reg8.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg8.u32) + offset_addr));
    hipp_cle_reg8.bits.facepos1_0 = facepos1_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg8.u32) + offset_addr), hipp_cle_reg8.u32);

    return;
}

hi_void pq_reg_cle_set_facepos14(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_4)
{
    u_hipp_cle_reg9 hipp_cle_reg9;

    hipp_cle_reg9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg9.u32) + offset_addr));
    hipp_cle_reg9.bits.facepos1_4 = facepos1_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg9.u32) + offset_addr), hipp_cle_reg9.u32);

    return;
}

hi_void pq_reg_cle_set_facepos13(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_3)
{
    u_hipp_cle_reg9 hipp_cle_reg9;

    hipp_cle_reg9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg9.u32) + offset_addr));
    hipp_cle_reg9.bits.facepos1_3 = facepos1_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg9.u32) + offset_addr), hipp_cle_reg9.u32);

    return;
}

hi_void pq_reg_cle_set_facepos12(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos1_2)
{
    u_hipp_cle_reg9 hipp_cle_reg9;

    hipp_cle_reg9.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg9.u32) + offset_addr));
    hipp_cle_reg9.bits.facepos1_2 = facepos1_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg9.u32) + offset_addr), hipp_cle_reg9.u32);

    return;
}

hi_void pq_reg_cle_set_facepos21(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_1)
{
    u_hipp_cle_reg10 hipp_cle_reg10;

    hipp_cle_reg10.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg10.u32) + offset_addr));
    hipp_cle_reg10.bits.facepos2_1 = facepos2_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg10.u32) + offset_addr), hipp_cle_reg10.u32);

    return;
}

hi_void pq_reg_cle_set_facepos20(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_0)
{
    u_hipp_cle_reg10 hipp_cle_reg10;

    hipp_cle_reg10.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg10.u32) + offset_addr));
    hipp_cle_reg10.bits.facepos2_0 = facepos2_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg10.u32) + offset_addr), hipp_cle_reg10.u32);

    return;
}

hi_void pq_reg_cle_set_facepos24(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_4)
{
    u_hipp_cle_reg11 hipp_cle_reg11;

    hipp_cle_reg11.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg11.u32) + offset_addr));
    hipp_cle_reg11.bits.facepos2_4 = facepos2_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg11.u32) + offset_addr), hipp_cle_reg11.u32);

    return;
}

hi_void pq_reg_cle_set_facepos23(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_3)
{
    u_hipp_cle_reg11 hipp_cle_reg11;

    hipp_cle_reg11.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg11.u32) + offset_addr));
    hipp_cle_reg11.bits.facepos2_3 = facepos2_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg11.u32) + offset_addr), hipp_cle_reg11.u32);

    return;
}

hi_void pq_reg_cle_set_facepos22(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos2_2)
{
    u_hipp_cle_reg11 hipp_cle_reg11;

    hipp_cle_reg11.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg11.u32) + offset_addr));
    hipp_cle_reg11.bits.facepos2_2 = facepos2_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg11.u32) + offset_addr), hipp_cle_reg11.u32);

    return;
}

hi_void pq_reg_cle_set_facepos31(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_1)
{
    u_hipp_cle_reg12 hipp_cle_reg12;

    hipp_cle_reg12.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg12.u32) + offset_addr));
    hipp_cle_reg12.bits.facepos3_1 = facepos3_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg12.u32) + offset_addr), hipp_cle_reg12.u32);

    return;
}

hi_void pq_reg_cle_set_facepos30(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_0)
{
    u_hipp_cle_reg12 hipp_cle_reg12;

    hipp_cle_reg12.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg12.u32) + offset_addr));
    hipp_cle_reg12.bits.facepos3_0 = facepos3_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg12.u32) + offset_addr), hipp_cle_reg12.u32);

    return;
}

hi_void pq_reg_cle_set_facepos34(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_4)
{
    u_hipp_cle_reg13 hipp_cle_reg13;

    hipp_cle_reg13.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg13.u32) + offset_addr));
    hipp_cle_reg13.bits.facepos3_4 = facepos3_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg13.u32) + offset_addr), hipp_cle_reg13.u32);

    return;
}

hi_void pq_reg_cle_set_facepos33(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_3)
{
    u_hipp_cle_reg13 hipp_cle_reg13;

    hipp_cle_reg13.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg13.u32) + offset_addr));
    hipp_cle_reg13.bits.facepos3_3 = facepos3_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg13.u32) + offset_addr), hipp_cle_reg13.u32);

    return;
}

hi_void pq_reg_cle_set_facepos32(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos3_2)
{
    u_hipp_cle_reg13 hipp_cle_reg13;

    hipp_cle_reg13.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg13.u32) + offset_addr));
    hipp_cle_reg13.bits.facepos3_2 = facepos3_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg13.u32) + offset_addr), hipp_cle_reg13.u32);

    return;
}

hi_void pq_reg_cle_set_facepos41(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_1)
{
    u_hipp_cle_reg14 hipp_cle_reg14;

    hipp_cle_reg14.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg14.u32) + offset_addr));
    hipp_cle_reg14.bits.facepos4_1 = facepos4_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg14.u32) + offset_addr), hipp_cle_reg14.u32);

    return;
}

hi_void pq_reg_cle_set_facepos40(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_0)
{
    u_hipp_cle_reg14 hipp_cle_reg14;

    hipp_cle_reg14.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg14.u32) + offset_addr));
    hipp_cle_reg14.bits.facepos4_0 = facepos4_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg14.u32) + offset_addr), hipp_cle_reg14.u32);

    return;
}

hi_void pq_reg_cle_set_facepos44(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_4)
{
    u_hipp_cle_reg15 hipp_cle_reg15;

    hipp_cle_reg15.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg15.u32) + offset_addr));
    hipp_cle_reg15.bits.facepos4_4 = facepos4_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg15.u32) + offset_addr), hipp_cle_reg15.u32);

    return;
}

hi_void pq_reg_cle_set_facepos43(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_3)
{
    u_hipp_cle_reg15 hipp_cle_reg15;

    hipp_cle_reg15.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg15.u32) + offset_addr));
    hipp_cle_reg15.bits.facepos4_3 = facepos4_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg15.u32) + offset_addr), hipp_cle_reg15.u32);

    return;
}

hi_void pq_reg_cle_set_facepos42(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos4_2)
{
    u_hipp_cle_reg15 hipp_cle_reg15;

    hipp_cle_reg15.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg15.u32) + offset_addr));
    hipp_cle_reg15.bits.facepos4_2 = facepos4_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg15.u32) + offset_addr), hipp_cle_reg15.u32);

    return;
}

hi_void pq_reg_cle_set_facepos51(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_1)
{
    u_hipp_cle_reg16 hipp_cle_reg16;

    hipp_cle_reg16.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg16.u32) + offset_addr));
    hipp_cle_reg16.bits.facepos5_1 = facepos5_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg16.u32) + offset_addr), hipp_cle_reg16.u32);

    return;
}

hi_void pq_reg_cle_set_facepos50(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_0)
{
    u_hipp_cle_reg16 hipp_cle_reg16;

    hipp_cle_reg16.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg16.u32) + offset_addr));
    hipp_cle_reg16.bits.facepos5_0 = facepos5_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg16.u32) + offset_addr), hipp_cle_reg16.u32);

    return;
}

hi_void pq_reg_cle_set_facepos54(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_4)
{
    u_hipp_cle_reg17 hipp_cle_reg17;

    hipp_cle_reg17.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg17.u32) + offset_addr));
    hipp_cle_reg17.bits.facepos5_4 = facepos5_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg17.u32) + offset_addr), hipp_cle_reg17.u32);

    return;
}

hi_void pq_reg_cle_set_facepos53(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_3)
{
    u_hipp_cle_reg17 hipp_cle_reg17;

    hipp_cle_reg17.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg17.u32) + offset_addr));
    hipp_cle_reg17.bits.facepos5_3 = facepos5_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg17.u32) + offset_addr), hipp_cle_reg17.u32);

    return;
}

hi_void pq_reg_cle_set_facepos52(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos5_2)
{
    u_hipp_cle_reg17 hipp_cle_reg17;

    hipp_cle_reg17.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg17.u32) + offset_addr));
    hipp_cle_reg17.bits.facepos5_2 = facepos5_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg17.u32) + offset_addr), hipp_cle_reg17.u32);

    return;
}

hi_void pq_reg_cle_set_facepos61(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_1)
{
    u_hipp_cle_reg18 hipp_cle_reg18;

    hipp_cle_reg18.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg18.u32) + offset_addr));
    hipp_cle_reg18.bits.facepos6_1 = facepos6_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg18.u32) + offset_addr), hipp_cle_reg18.u32);

    return;
}

hi_void pq_reg_cle_set_facepos60(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_0)
{
    u_hipp_cle_reg18 hipp_cle_reg18;

    hipp_cle_reg18.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg18.u32) + offset_addr));
    hipp_cle_reg18.bits.facepos6_0 = facepos6_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg18.u32) + offset_addr), hipp_cle_reg18.u32);

    return;
}

hi_void pq_reg_cle_set_facepos64(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_4)
{
    u_hipp_cle_reg19 hipp_cle_reg19;

    hipp_cle_reg19.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg19.u32) + offset_addr));
    hipp_cle_reg19.bits.facepos6_4 = facepos6_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg19.u32) + offset_addr), hipp_cle_reg19.u32);

    return;
}

hi_void pq_reg_cle_set_facepos63(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_3)
{
    u_hipp_cle_reg19 hipp_cle_reg19;

    hipp_cle_reg19.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg19.u32) + offset_addr));
    hipp_cle_reg19.bits.facepos6_3 = facepos6_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg19.u32) + offset_addr), hipp_cle_reg19.u32);

    return;
}

hi_void pq_reg_cle_set_facepos62(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos6_2)
{
    u_hipp_cle_reg19 hipp_cle_reg19;

    hipp_cle_reg19.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg19.u32) + offset_addr));
    hipp_cle_reg19.bits.facepos6_2 = facepos6_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg19.u32) + offset_addr), hipp_cle_reg19.u32);

    return;
}

hi_void pq_reg_cle_set_facepos71(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_1)
{
    u_hipp_cle_reg20 hipp_cle_reg20;

    hipp_cle_reg20.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg20.u32) + offset_addr));
    hipp_cle_reg20.bits.facepos7_1 = facepos7_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg20.u32) + offset_addr), hipp_cle_reg20.u32);

    return;
}

hi_void pq_reg_cle_set_facepos70(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_0)
{
    u_hipp_cle_reg20 hipp_cle_reg20;

    hipp_cle_reg20.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg20.u32) + offset_addr));
    hipp_cle_reg20.bits.facepos7_0 = facepos7_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg20.u32) + offset_addr), hipp_cle_reg20.u32);

    return;
}

hi_void pq_reg_cle_set_facepos74(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_4)
{
    u_hipp_cle_reg21 hipp_cle_reg21;

    hipp_cle_reg21.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg21.u32) + offset_addr));
    hipp_cle_reg21.bits.facepos7_4 = facepos7_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg21.u32) + offset_addr), hipp_cle_reg21.u32);

    return;
}

hi_void pq_reg_cle_set_facepos73(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_3)
{
    u_hipp_cle_reg21 hipp_cle_reg21;

    hipp_cle_reg21.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg21.u32) + offset_addr));
    hipp_cle_reg21.bits.facepos7_3 = facepos7_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg21.u32) + offset_addr), hipp_cle_reg21.u32);

    return;
}

hi_void pq_reg_cle_set_facepos72(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos7_2)
{
    u_hipp_cle_reg21 hipp_cle_reg21;

    hipp_cle_reg21.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg21.u32) + offset_addr));
    hipp_cle_reg21.bits.facepos7_2 = facepos7_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg21.u32) + offset_addr), hipp_cle_reg21.u32);

    return;
}

hi_void pq_reg_cle_set_facepos81(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_1)
{
    u_hipp_cle_reg22 hipp_cle_reg22;

    hipp_cle_reg22.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg22.u32) + offset_addr));
    hipp_cle_reg22.bits.facepos8_1 = facepos8_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg22.u32) + offset_addr), hipp_cle_reg22.u32);

    return;
}

hi_void pq_reg_cle_set_facepos80(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_0)
{
    u_hipp_cle_reg22 hipp_cle_reg22;

    hipp_cle_reg22.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg22.u32) + offset_addr));
    hipp_cle_reg22.bits.facepos8_0 = facepos8_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg22.u32) + offset_addr), hipp_cle_reg22.u32);

    return;
}

hi_void pq_reg_cle_set_facepos84(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_4)
{
    u_hipp_cle_reg23 hipp_cle_reg23;

    hipp_cle_reg23.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg23.u32) + offset_addr));
    hipp_cle_reg23.bits.facepos8_4 = facepos8_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg23.u32) + offset_addr), hipp_cle_reg23.u32);

    return;
}

hi_void pq_reg_cle_set_facepos83(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_3)
{
    u_hipp_cle_reg23 hipp_cle_reg23;

    hipp_cle_reg23.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg23.u32) + offset_addr));
    hipp_cle_reg23.bits.facepos8_3 = facepos8_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg23.u32) + offset_addr), hipp_cle_reg23.u32);

    return;
}

hi_void pq_reg_cle_set_facepos82(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos8_2)
{
    u_hipp_cle_reg23 hipp_cle_reg23;

    hipp_cle_reg23.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg23.u32) + offset_addr));
    hipp_cle_reg23.bits.facepos8_2 = facepos8_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg23.u32) + offset_addr), hipp_cle_reg23.u32);

    return;
}

hi_void pq_reg_cle_set_facepos91(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_1)
{
    u_hipp_cle_reg24 hipp_cle_reg24;

    hipp_cle_reg24.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg24.u32) + offset_addr));
    hipp_cle_reg24.bits.facepos9_1 = facepos9_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg24.u32) + offset_addr), hipp_cle_reg24.u32);

    return;
}

hi_void pq_reg_cle_set_facepos90(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_0)
{
    u_hipp_cle_reg24 hipp_cle_reg24;

    hipp_cle_reg24.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg24.u32) + offset_addr));
    hipp_cle_reg24.bits.facepos9_0 = facepos9_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg24.u32) + offset_addr), hipp_cle_reg24.u32);

    return;
}

hi_void pq_reg_cle_set_facepos94(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_4)
{
    u_hipp_cle_reg25 hipp_cle_reg25;

    hipp_cle_reg25.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg25.u32) + offset_addr));
    hipp_cle_reg25.bits.facepos9_4 = facepos9_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg25.u32) + offset_addr), hipp_cle_reg25.u32);

    return;
}

hi_void pq_reg_cle_set_facepos93(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_3)
{
    u_hipp_cle_reg25 hipp_cle_reg25;

    hipp_cle_reg25.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg25.u32) + offset_addr));
    hipp_cle_reg25.bits.facepos9_3 = facepos9_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg25.u32) + offset_addr), hipp_cle_reg25.u32);

    return;
}

hi_void pq_reg_cle_set_facepos92(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos9_2)
{
    u_hipp_cle_reg25 hipp_cle_reg25;

    hipp_cle_reg25.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg25.u32) + offset_addr));
    hipp_cle_reg25.bits.facepos9_2 = facepos9_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg25.u32) + offset_addr), hipp_cle_reg25.u32);

    return;
}

hi_void pq_reg_cle_set_facepos101(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_1)
{
    u_hipp_cle_reg26 hipp_cle_reg26;

    hipp_cle_reg26.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg26.u32) + offset_addr));
    hipp_cle_reg26.bits.facepos10_1 = facepos10_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg26.u32) + offset_addr), hipp_cle_reg26.u32);

    return;
}

hi_void pq_reg_cle_set_facepos100(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_0)
{
    u_hipp_cle_reg26 hipp_cle_reg26;

    hipp_cle_reg26.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg26.u32) + offset_addr));
    hipp_cle_reg26.bits.facepos10_0 = facepos10_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg26.u32) + offset_addr), hipp_cle_reg26.u32);

    return;
}

hi_void pq_reg_cle_set_facepos104(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_4)
{
    u_hipp_cle_reg27 hipp_cle_reg27;

    hipp_cle_reg27.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg27.u32) + offset_addr));
    hipp_cle_reg27.bits.facepos10_4 = facepos10_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg27.u32) + offset_addr), hipp_cle_reg27.u32);

    return;
}

hi_void pq_reg_cle_set_facepos103(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_3)
{
    u_hipp_cle_reg27 hipp_cle_reg27;

    hipp_cle_reg27.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg27.u32) + offset_addr));
    hipp_cle_reg27.bits.facepos10_3 = facepos10_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg27.u32) + offset_addr), hipp_cle_reg27.u32);

    return;
}

hi_void pq_reg_cle_set_facepos102(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos10_2)
{
    u_hipp_cle_reg27 hipp_cle_reg27;

    hipp_cle_reg27.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg27.u32) + offset_addr));
    hipp_cle_reg27.bits.facepos10_2 = facepos10_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg27.u32) + offset_addr), hipp_cle_reg27.u32);

    return;
}

hi_void pq_reg_cle_set_facepos111(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_1)
{
    u_hipp_cle_reg28 hipp_cle_reg28;

    hipp_cle_reg28.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg28.u32) + offset_addr));
    hipp_cle_reg28.bits.facepos11_1 = facepos11_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg28.u32) + offset_addr), hipp_cle_reg28.u32);

    return;
}

hi_void pq_reg_cle_set_facepos110(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_0)
{
    u_hipp_cle_reg28 hipp_cle_reg28;

    hipp_cle_reg28.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg28.u32) + offset_addr));
    hipp_cle_reg28.bits.facepos11_0 = facepos11_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg28.u32) + offset_addr), hipp_cle_reg28.u32);

    return;
}

hi_void pq_reg_cle_set_facepos114(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_4)
{
    u_hipp_cle_reg29 hipp_cle_reg29;

    hipp_cle_reg29.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg29.u32) + offset_addr));
    hipp_cle_reg29.bits.facepos11_4 = facepos11_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg29.u32) + offset_addr), hipp_cle_reg29.u32);

    return;
}

hi_void pq_reg_cle_set_facepos113(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_3)
{
    u_hipp_cle_reg29 hipp_cle_reg29;

    hipp_cle_reg29.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg29.u32) + offset_addr));
    hipp_cle_reg29.bits.facepos11_3 = facepos11_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg29.u32) + offset_addr), hipp_cle_reg29.u32);

    return;
}

hi_void pq_reg_cle_set_facepos112(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos11_2)
{
    u_hipp_cle_reg29 hipp_cle_reg29;

    hipp_cle_reg29.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg29.u32) + offset_addr));
    hipp_cle_reg29.bits.facepos11_2 = facepos11_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg29.u32) + offset_addr), hipp_cle_reg29.u32);

    return;
}

hi_void pq_reg_cle_set_facepos121(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_1)
{
    u_hipp_cle_reg30 hipp_cle_reg30;

    hipp_cle_reg30.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg30.u32) + offset_addr));
    hipp_cle_reg30.bits.facepos12_1 = facepos12_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg30.u32) + offset_addr), hipp_cle_reg30.u32);

    return;
}

hi_void pq_reg_cle_set_facepos120(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_0)
{
    u_hipp_cle_reg30 hipp_cle_reg30;

    hipp_cle_reg30.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg30.u32) + offset_addr));
    hipp_cle_reg30.bits.facepos12_0 = facepos12_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg30.u32) + offset_addr), hipp_cle_reg30.u32);

    return;
}

hi_void pq_reg_cle_set_facepos124(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_4)
{
    u_hipp_cle_reg31 hipp_cle_reg31;

    hipp_cle_reg31.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg31.u32) + offset_addr));
    hipp_cle_reg31.bits.facepos12_4 = facepos12_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg31.u32) + offset_addr), hipp_cle_reg31.u32);

    return;
}

hi_void pq_reg_cle_set_facepos123(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_3)
{
    u_hipp_cle_reg31 hipp_cle_reg31;

    hipp_cle_reg31.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg31.u32) + offset_addr));
    hipp_cle_reg31.bits.facepos12_3 = facepos12_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg31.u32) + offset_addr), hipp_cle_reg31.u32);

    return;
}

hi_void pq_reg_cle_set_facepos122(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos12_2)
{
    u_hipp_cle_reg31 hipp_cle_reg31;

    hipp_cle_reg31.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg31.u32) + offset_addr));
    hipp_cle_reg31.bits.facepos12_2 = facepos12_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg31.u32) + offset_addr), hipp_cle_reg31.u32);

    return;
}

hi_void pq_reg_cle_set_facepos131(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_1)
{
    u_hipp_cle_reg32 hipp_cle_reg32;

    hipp_cle_reg32.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg32.u32) + offset_addr));
    hipp_cle_reg32.bits.facepos13_1 = facepos13_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg32.u32) + offset_addr), hipp_cle_reg32.u32);

    return;
}

hi_void pq_reg_cle_set_facepos130(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_0)
{
    u_hipp_cle_reg32 hipp_cle_reg32;

    hipp_cle_reg32.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg32.u32) + offset_addr));
    hipp_cle_reg32.bits.facepos13_0 = facepos13_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg32.u32) + offset_addr), hipp_cle_reg32.u32);

    return;
}

hi_void pq_reg_cle_set_facepos134(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_4)
{
    u_hipp_cle_reg33 hipp_cle_reg33;

    hipp_cle_reg33.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg33.u32) + offset_addr));
    hipp_cle_reg33.bits.facepos13_4 = facepos13_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg33.u32) + offset_addr), hipp_cle_reg33.u32);

    return;
}

hi_void pq_reg_cle_set_facepos133(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_3)
{
    u_hipp_cle_reg33 hipp_cle_reg33;

    hipp_cle_reg33.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg33.u32) + offset_addr));
    hipp_cle_reg33.bits.facepos13_3 = facepos13_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg33.u32) + offset_addr), hipp_cle_reg33.u32);

    return;
}

hi_void pq_reg_cle_set_facepos132(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos13_2)
{
    u_hipp_cle_reg33 hipp_cle_reg33;

    hipp_cle_reg33.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg33.u32) + offset_addr));
    hipp_cle_reg33.bits.facepos13_2 = facepos13_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg33.u32) + offset_addr), hipp_cle_reg33.u32);

    return;
}

hi_void pq_reg_cle_set_facepos141(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_1)
{
    u_hipp_cle_reg34 hipp_cle_reg34;

    hipp_cle_reg34.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg34.u32) + offset_addr));
    hipp_cle_reg34.bits.facepos14_1 = facepos14_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg34.u32) + offset_addr), hipp_cle_reg34.u32);

    return;
}

hi_void pq_reg_cle_set_facepos140(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_0)
{
    u_hipp_cle_reg34 hipp_cle_reg34;

    hipp_cle_reg34.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg34.u32) + offset_addr));
    hipp_cle_reg34.bits.facepos14_0 = facepos14_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg34.u32) + offset_addr), hipp_cle_reg34.u32);

    return;
}

hi_void pq_reg_cle_set_facepos144(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_4)
{
    u_hipp_cle_reg35 hipp_cle_reg35;

    hipp_cle_reg35.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg35.u32) + offset_addr));
    hipp_cle_reg35.bits.facepos14_4 = facepos14_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg35.u32) + offset_addr), hipp_cle_reg35.u32);

    return;
}

hi_void pq_reg_cle_set_facepos143(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_3)
{
    u_hipp_cle_reg35 hipp_cle_reg35;

    hipp_cle_reg35.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg35.u32) + offset_addr));
    hipp_cle_reg35.bits.facepos14_3 = facepos14_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg35.u32) + offset_addr), hipp_cle_reg35.u32);

    return;
}

hi_void pq_reg_cle_set_facepos142(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos14_2)
{
    u_hipp_cle_reg35 hipp_cle_reg35;

    hipp_cle_reg35.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg35.u32) + offset_addr));
    hipp_cle_reg35.bits.facepos14_2 = facepos14_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg35.u32) + offset_addr), hipp_cle_reg35.u32);

    return;
}

hi_void pq_reg_cle_set_facepos151(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_1)
{
    u_hipp_cle_reg36 hipp_cle_reg36;

    hipp_cle_reg36.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg36.u32) + offset_addr));
    hipp_cle_reg36.bits.facepos15_1 = facepos15_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg36.u32) + offset_addr), hipp_cle_reg36.u32);

    return;
}

hi_void pq_reg_cle_set_facepos150(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_0)
{
    u_hipp_cle_reg36 hipp_cle_reg36;

    hipp_cle_reg36.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg36.u32) + offset_addr));
    hipp_cle_reg36.bits.facepos15_0 = facepos15_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg36.u32) + offset_addr), hipp_cle_reg36.u32);

    return;
}

hi_void pq_reg_cle_set_facepos154(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_4)
{
    u_hipp_cle_reg37 hipp_cle_reg37;

    hipp_cle_reg37.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg37.u32) + offset_addr));
    hipp_cle_reg37.bits.facepos15_4 = facepos15_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg37.u32) + offset_addr), hipp_cle_reg37.u32);

    return;
}

hi_void pq_reg_cle_set_facepos153(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_3)
{
    u_hipp_cle_reg37 hipp_cle_reg37;

    hipp_cle_reg37.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg37.u32) + offset_addr));
    hipp_cle_reg37.bits.facepos15_3 = facepos15_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg37.u32) + offset_addr), hipp_cle_reg37.u32);

    return;
}

hi_void pq_reg_cle_set_facepos152(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos15_2)
{
    u_hipp_cle_reg37 hipp_cle_reg37;

    hipp_cle_reg37.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg37.u32) + offset_addr));
    hipp_cle_reg37.bits.facepos15_2 = facepos15_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg37.u32) + offset_addr), hipp_cle_reg37.u32);

    return;
}

hi_void pq_reg_cle_set_facepos161(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_1)
{
    u_hipp_cle_reg38 hipp_cle_reg38;

    hipp_cle_reg38.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg38.u32) + offset_addr));
    hipp_cle_reg38.bits.facepos16_1 = facepos16_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg38.u32) + offset_addr), hipp_cle_reg38.u32);

    return;
}

hi_void pq_reg_cle_set_facepos160(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_0)
{
    u_hipp_cle_reg38 hipp_cle_reg38;

    hipp_cle_reg38.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg38.u32) + offset_addr));
    hipp_cle_reg38.bits.facepos16_0 = facepos16_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg38.u32) + offset_addr), hipp_cle_reg38.u32);

    return;
}

hi_void pq_reg_cle_set_facepos164(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_4)
{
    u_hipp_cle_reg39 hipp_cle_reg39;

    hipp_cle_reg39.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg39.u32) + offset_addr));
    hipp_cle_reg39.bits.facepos16_4 = facepos16_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg39.u32) + offset_addr), hipp_cle_reg39.u32);

    return;
}

hi_void pq_reg_cle_set_facepos163(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_3)
{
    u_hipp_cle_reg39 hipp_cle_reg39;

    hipp_cle_reg39.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg39.u32) + offset_addr));
    hipp_cle_reg39.bits.facepos16_3 = facepos16_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg39.u32) + offset_addr), hipp_cle_reg39.u32);

    return;
}

hi_void pq_reg_cle_set_facepos162(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos16_2)
{
    u_hipp_cle_reg39 hipp_cle_reg39;

    hipp_cle_reg39.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg39.u32) + offset_addr));
    hipp_cle_reg39.bits.facepos16_2 = facepos16_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg39.u32) + offset_addr), hipp_cle_reg39.u32);

    return;
}

hi_void pq_reg_cle_set_facepos171(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_1)
{
    u_hipp_cle_reg40 hipp_cle_reg40;

    hipp_cle_reg40.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg40.u32) + offset_addr));
    hipp_cle_reg40.bits.facepos17_1 = facepos17_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg40.u32) + offset_addr), hipp_cle_reg40.u32);

    return;
}

hi_void pq_reg_cle_set_facepos170(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_0)
{
    u_hipp_cle_reg40 hipp_cle_reg40;

    hipp_cle_reg40.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg40.u32) + offset_addr));
    hipp_cle_reg40.bits.facepos17_0 = facepos17_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg40.u32) + offset_addr), hipp_cle_reg40.u32);

    return;
}

hi_void pq_reg_cle_set_facepos174(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_4)
{
    u_hipp_cle_reg41 hipp_cle_reg41;

    hipp_cle_reg41.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg41.u32) + offset_addr));
    hipp_cle_reg41.bits.facepos17_4 = facepos17_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg41.u32) + offset_addr), hipp_cle_reg41.u32);

    return;
}

hi_void pq_reg_cle_set_facepos173(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_3)
{
    u_hipp_cle_reg41 hipp_cle_reg41;

    hipp_cle_reg41.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg41.u32) + offset_addr));
    hipp_cle_reg41.bits.facepos17_3 = facepos17_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg41.u32) + offset_addr), hipp_cle_reg41.u32);

    return;
}

hi_void pq_reg_cle_set_facepos172(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos17_2)
{
    u_hipp_cle_reg41 hipp_cle_reg41;

    hipp_cle_reg41.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg41.u32) + offset_addr));
    hipp_cle_reg41.bits.facepos17_2 = facepos17_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg41.u32) + offset_addr), hipp_cle_reg41.u32);

    return;
}

hi_void pq_reg_cle_set_facepos181(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_1)
{
    u_hipp_cle_reg42 hipp_cle_reg42;

    hipp_cle_reg42.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg42.u32) + offset_addr));
    hipp_cle_reg42.bits.facepos18_1 = facepos18_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg42.u32) + offset_addr), hipp_cle_reg42.u32);

    return;
}

hi_void pq_reg_cle_set_facepos180(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_0)
{
    u_hipp_cle_reg42 hipp_cle_reg42;

    hipp_cle_reg42.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg42.u32) + offset_addr));
    hipp_cle_reg42.bits.facepos18_0 = facepos18_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg42.u32) + offset_addr), hipp_cle_reg42.u32);

    return;
}

hi_void pq_reg_cle_set_facepos184(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_4)
{
    u_hipp_cle_reg43 hipp_cle_reg43;

    hipp_cle_reg43.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg43.u32) + offset_addr));
    hipp_cle_reg43.bits.facepos18_4 = facepos18_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg43.u32) + offset_addr), hipp_cle_reg43.u32);

    return;
}

hi_void pq_reg_cle_set_facepos183(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_3)
{
    u_hipp_cle_reg43 hipp_cle_reg43;

    hipp_cle_reg43.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg43.u32) + offset_addr));
    hipp_cle_reg43.bits.facepos18_3 = facepos18_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg43.u32) + offset_addr), hipp_cle_reg43.u32);

    return;
}

hi_void pq_reg_cle_set_facepos182(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos18_2)
{
    u_hipp_cle_reg43 hipp_cle_reg43;

    hipp_cle_reg43.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg43.u32) + offset_addr));
    hipp_cle_reg43.bits.facepos18_2 = facepos18_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg43.u32) + offset_addr), hipp_cle_reg43.u32);

    return;
}

hi_void pq_reg_cle_set_facepos191(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_1)
{
    u_hipp_cle_reg44 hipp_cle_reg44;

    hipp_cle_reg44.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg44.u32) + offset_addr));
    hipp_cle_reg44.bits.facepos19_1 = facepos19_1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg44.u32) + offset_addr), hipp_cle_reg44.u32);

    return;
}

hi_void pq_reg_cle_set_facepos190(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_0)
{
    u_hipp_cle_reg44 hipp_cle_reg44;

    hipp_cle_reg44.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg44.u32) + offset_addr));
    hipp_cle_reg44.bits.facepos19_0 = facepos19_0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg44.u32) + offset_addr), hipp_cle_reg44.u32);

    return;
}

hi_void pq_reg_cle_set_facepos194(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_4)
{
    u_hipp_cle_reg45 hipp_cle_reg45;

    hipp_cle_reg45.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg45.u32) + offset_addr));
    hipp_cle_reg45.bits.facepos19_4 = facepos19_4;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg45.u32) + offset_addr), hipp_cle_reg45.u32);

    return;
}

hi_void pq_reg_cle_set_facepos193(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_3)
{
    u_hipp_cle_reg45 hipp_cle_reg45;

    hipp_cle_reg45.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg45.u32) + offset_addr));
    hipp_cle_reg45.bits.facepos19_3 = facepos19_3;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg45.u32) + offset_addr), hipp_cle_reg45.u32);

    return;
}

hi_void pq_reg_cle_set_facepos192(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 facepos19_2)
{
    u_hipp_cle_reg45 hipp_cle_reg45;

    hipp_cle_reg45.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg45.u32) + offset_addr));
    hipp_cle_reg45.bits.facepos19_2 = facepos19_2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg45.u32) + offset_addr), hipp_cle_reg45.u32);

    return;
}

hi_void pq_reg_cle_set_contrastbit(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 contrastbit)
{
    u_hipp_cle_reg46 hipp_cle_reg46;

    hipp_cle_reg46.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr));
    hipp_cle_reg46.bits.contrastbit = contrastbit;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr), hipp_cle_reg46.u32);

    return;
}

hi_void pq_reg_cle_set_contrastthr(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 contrastthr)
{
    u_hipp_cle_reg46 hipp_cle_reg46;

    hipp_cle_reg46.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr));
    hipp_cle_reg46.bits.contrastthr = contrastthr;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr), hipp_cle_reg46.u32);

    return;
}

hi_void pq_reg_cle_set_flatbit(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 flatbit)
{
    u_hipp_cle_reg46 hipp_cle_reg46;

    hipp_cle_reg46.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr));
    hipp_cle_reg46.bits.flatbit = flatbit;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr), hipp_cle_reg46.u32);

    return;
}

hi_void pq_reg_cle_set_flatthrl(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 flatthrl)
{
    u_hipp_cle_reg46 hipp_cle_reg46;

    hipp_cle_reg46.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr));
    hipp_cle_reg46.bits.flatthrl = flatthrl;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr), hipp_cle_reg46.u32);

    return;
}

hi_void pq_reg_cle_set_a1slp(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 a1slp)
{
    u_hipp_cle_reg46 hipp_cle_reg46;

    hipp_cle_reg46.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr));
    hipp_cle_reg46.bits.a1slp = a1slp;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr), hipp_cle_reg46.u32);

    return;
}

hi_void pq_reg_cle_set_kneg(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 kneg)
{
    u_hipp_cle_reg46 hipp_cle_reg46;

    hipp_cle_reg46.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr));
    hipp_cle_reg46.bits.kneg = kneg;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg46.u32) + offset_addr), hipp_cle_reg46.u32);

    return;
}

hi_void pq_reg_cle_set_a1thrl(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 a1thrl)
{
    u_hipp_cle_reg47 hipp_cle_reg47;

    hipp_cle_reg47.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr));
    hipp_cle_reg47.bits.a1thrl = a1thrl;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr), hipp_cle_reg47.u32);

    return;
}

hi_void pq_reg_cle_set_gradmonoslp(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 gradmonoslp)
{
    u_hipp_cle_reg47 hipp_cle_reg47;

    hipp_cle_reg47.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr));
    hipp_cle_reg47.bits.gradmonoslp = gradmonoslp;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr), hipp_cle_reg47.u32);

    return;
}

hi_void pq_reg_cle_set_gradmonomin(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 gradmonomin)
{
    u_hipp_cle_reg47 hipp_cle_reg47;

    hipp_cle_reg47.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr));
    hipp_cle_reg47.bits.gradmonomin = gradmonomin;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr), hipp_cle_reg47.u32);

    return;
}

hi_void pq_reg_cle_set_gradmonomax(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 gradmonomax)
{
    u_hipp_cle_reg47 hipp_cle_reg47;

    hipp_cle_reg47.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr));
    hipp_cle_reg47.bits.gradmonomax = gradmonomax;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr), hipp_cle_reg47.u32);

    return;
}

hi_void pq_reg_cle_set_pixmonothr(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 pixmonothr)
{
    u_hipp_cle_reg47 hipp_cle_reg47;

    hipp_cle_reg47.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr));
    hipp_cle_reg47.bits.pixmonothr = pixmonothr;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg47.u32) + offset_addr), hipp_cle_reg47.u32);

    return;
}

hi_void pq_reg_cle_set_ltiratio(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 ltiratio)
{
    u_hipp_cle_reg48 hipp_cle_reg48;

    hipp_cle_reg48.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg48.u32) + offset_addr));
    hipp_cle_reg48.bits.ltiratio = ltiratio;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg48.u32) + offset_addr), hipp_cle_reg48.u32);

    return;
}

hi_void pq_reg_cle_set_ltigain(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 ltigain)
{
    u_hipp_cle_reg48 hipp_cle_reg48;

    hipp_cle_reg48.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg48.u32) + offset_addr));
    hipp_cle_reg48.bits.ltigain = ltigain;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg48.u32) + offset_addr), hipp_cle_reg48.u32);

    return;
}

hi_void pq_reg_cle_set_stcv_sclwgtratio(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 stcv_sclwgtratio)
{
    u_hipp_cle_reg63 hipp_cle_reg63;

    hipp_cle_reg63.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg63.u32) + offset_addr));
    hipp_cle_reg63.bits.stcv_sclwgtratio = stcv_sclwgtratio;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg63.u32) + offset_addr), hipp_cle_reg63.u32);

    return;
}

hi_void pq_reg_cle_set_skin_yvalue_mixratio(vdp_regs_type *vdp_reg, hi_u32 offset_addr, hi_u32 skin_yvalue_mixratio)
{
    u_hipp_cle_reg72 hipp_cle_reg72;

    hipp_cle_reg72.u32 = pq_hal_cle_ddr_regread((uintptr_t)(&(vdp_reg->hipp_cle_reg72.u32) + offset_addr));
    hipp_cle_reg72.bits.skin_yvalue_mixratio = skin_yvalue_mixratio;
    pq_hal_cle_ddr_regwrite((uintptr_t)(&(vdp_reg->hipp_cle_reg72.u32) + offset_addr), hipp_cle_reg72.u32);

    return;
}

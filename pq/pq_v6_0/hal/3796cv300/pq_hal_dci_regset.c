/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dci register source file
 * Author: pq
 * Create: 2019-01-01
 */

#include "pq_hal_comm.h"
#include "pq_hal_dci_regset.h"
#include "pq_hal_dci.h"

hi_void pq_reg_dci_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = (1 << (u32chnnum - DCI_PARAUP_COEF_OFFSET));
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

hi_void pq_reg_dci_para_haddr_v0_chn15(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn15 para_haddr_v0_chn15;

    para_haddr_v0_chn15.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_haddr_v0_chn15.u32));
    para_haddr_v0_chn15.bits.para_haddr_v0_chn15 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn15.u32)), para_haddr_v0_chn15.u32);

    return;
}

hi_void pq_reg_dci_para_addr_v0_chn15(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn15 para_addr_v0_chn15;

    para_addr_v0_chn15.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_addr_v0_chn15.u32));
    para_addr_v0_chn15.bits.para_addr_v0_chn15 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn15.u32)), para_addr_v0_chn15.u32);

    return;
}

hi_void pq_reg_dci_set_link_ctrl(vdp_regs_type *vdp_reg, hi_u32 link_type)
{
    u_v0_ctrl v0_ctrl;

    v0_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->v0_ctrl.u32)));
    v0_ctrl.bits.dci_link = link_type;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->v0_ctrl.u32)), v0_ctrl.u32);
    return;
}

hi_void pq_reg_dci_get_mean_value(vdp_regs_type *reg, hi_u32 offset, hi_u32 *mean_value)
{
    u_hipp_dci_debug118 hipp_dci_debug118;

    hipp_dci_debug118.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_debug118.u32) + offset));
    *mean_value = hipp_dci_debug118.bits.dci_mean_value;

    return;
}

hi_void pq_reg_dci_set_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 enable)
{
    u_hipp_dci_ctrl0 hipp_dci_ctrl0;

    hipp_dci_ctrl0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_ctrl0.u32) + offset));
    hipp_dci_ctrl0.bits.dci_en = enable;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_ctrl0.u32) + offset), hipp_dci_ctrl0.u32);

    return;
}

hi_void pq_reg_dci_set_demo_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en)
{
    u_hipp_dci_ctrl0 hipp_dci_ctrl0;

    hipp_dci_ctrl0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_ctrl0.u32) + offset));
    hipp_dci_ctrl0.bits.demo_en = demo_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_ctrl0.u32) + offset), hipp_dci_ctrl0.u32);
    return;
}

hi_void pq_reg_dci_set_demo_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode)
{
    u_hipp_dci_reg4 hipp_dci_reg4;

    hipp_dci_reg4.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg4.u32) + offset));
    hipp_dci_reg4.bits.demo_mode = demo_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg4.u32) + offset), hipp_dci_reg4.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain0)
{
    u_hipp_dci_reg13 hipp_dci_reg13;

    hipp_dci_reg13.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13.u32) + offset));
    hipp_dci_reg13.bits.global_gain_0 = global_gain0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13.u32) + offset), hipp_dci_reg13.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain1)
{
    u_hipp_dci_reg13 hipp_dci_reg13;

    hipp_dci_reg13.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13.u32) + offset));
    hipp_dci_reg13.bits.global_gain_1 = global_gain1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13.u32) + offset), hipp_dci_reg13.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain2)
{
    u_hipp_dci_reg13 hipp_dci_reg13;

    hipp_dci_reg13.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13.u32) + offset));
    hipp_dci_reg13.bits.global_gain_2 = global_gain2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13.u32) + offset), hipp_dci_reg13.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain0_s0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain0)
{
    u_hipp_dci_reg13_s0 hipp_dci_reg13_s0;

    hipp_dci_reg13_s0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s0.u32) + offset));
    hipp_dci_reg13_s0.bits.global_gain_0 = global_gain0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s0.u32) + offset), hipp_dci_reg13_s0.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain1_s0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain1)
{
    u_hipp_dci_reg13_s0 hipp_dci_reg13_s0;

    hipp_dci_reg13_s0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s0.u32) + offset));
    hipp_dci_reg13_s0.bits.global_gain_1 = global_gain1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s0.u32) + offset), hipp_dci_reg13_s0.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain2_s0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain2)
{
    u_hipp_dci_reg13_s0 hipp_dci_reg13_s0;

    hipp_dci_reg13_s0.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s0.u32) + offset));
    hipp_dci_reg13_s0.bits.global_gain_2 = global_gain2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s0.u32) + offset), hipp_dci_reg13_s0.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain0_s1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain0)
{
    u_hipp_dci_reg13_s1 hipp_dci_reg13_s1;

    hipp_dci_reg13_s1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s1.u32) + offset));
    hipp_dci_reg13_s1.bits.global_gain_0 = global_gain0;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s1.u32) + offset), hipp_dci_reg13_s1.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain1_s1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain1)
{
    u_hipp_dci_reg13_s1 hipp_dci_reg13_s1;

    hipp_dci_reg13_s1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s1.u32) + offset));
    hipp_dci_reg13_s1.bits.global_gain_1 = global_gain1;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s1.u32) + offset), hipp_dci_reg13_s1.u32);
    return;
}

hi_void pq_reg_dci_set_global_gain2_s1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 global_gain2)
{
    u_hipp_dci_reg13_s1 hipp_dci_reg13_s1;

    hipp_dci_reg13_s1.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s1.u32) + offset));
    hipp_dci_reg13_s1.bits.global_gain_2 = global_gain2;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->hipp_dci_reg13_s1.u32) + offset), hipp_dci_reg13_s1.u32);
    return;
}

hi_void pq_reg_dci_set_dci_masic_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 dci_masic_en)
{
    u_hipp_dci_ctrl0 hipp_dci_ctrl0;

    hipp_dci_ctrl0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_ctrl0.u32) + offset));
    hipp_dci_ctrl0.bits.dci_masic_en = dci_masic_en;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_ctrl0.u32) + offset), hipp_dci_ctrl0.u32);

    return;
}

hi_void pq_reg_dci_set_dci_ck_gt_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 dci_ck_gt_en)
{
    u_hipp_dci_ctrl0 hipp_dci_ctrl0;

    hipp_dci_ctrl0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_ctrl0.u32) + offset));
    hipp_dci_ctrl0.bits.dci_ck_gt_en = dci_ck_gt_en;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_ctrl0.u32) + offset), hipp_dci_ctrl0.u32);

    return;
}

hi_void pq_reg_dci_set_scd_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 scd_en)
{
    u_hipp_dci_ctrl0 hipp_dci_ctrl0;

    hipp_dci_ctrl0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_ctrl0.u32) + offset));
    hipp_dci_ctrl0.bits.scd_en = scd_en;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_ctrl0.u32) + offset), hipp_dci_ctrl0.u32);

    return;
}

hi_void pq_reg_dci_set_dci_dither_en(vdp_regs_type *reg, hi_u32 offset, hi_u32 dci_dither_en)
{
    u_hipp_dci_ctrl1 hipp_dci_ctrl1;

    hipp_dci_ctrl1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_ctrl1.u32) + offset));
    hipp_dci_ctrl1.bits.dci_dither_en = dci_dither_en;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_ctrl1.u32) + offset), hipp_dci_ctrl1.u32);

    return;
}

hi_void pq_reg_dci_set_input_full_range(vdp_regs_type *reg, hi_u32 offset, hi_u32 input_full_range)
{
    u_hipp_dci_ctrl1 hipp_dci_ctrl1;

    hipp_dci_ctrl1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_ctrl1.u32) + offset));
    hipp_dci_ctrl1.bits.input_full_range = input_full_range;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_ctrl1.u32) + offset), hipp_dci_ctrl1.u32);

    return;
}

hi_void pq_reg_dci_set_output_full_range(vdp_regs_type *reg, hi_u32 offset, hi_u32 output_full_range)
{
    u_hipp_dci_ctrl1 hipp_dci_ctrl1;

    hipp_dci_ctrl1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_ctrl1.u32) + offset));
    hipp_dci_ctrl1.bits.output_full_range = output_full_range;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_ctrl1.u32) + offset), hipp_dci_ctrl1.u32);

    return;
}

hi_void pq_reg_dci_set_demo_split_pos(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_split_pos)
{
    u_hipp_dci_reg4 hipp_dci_reg4;

    hipp_dci_reg4.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4.u32) + offset));
    hipp_dci_reg4.bits.demo_split_pos = demo_split_pos;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4.u32) + offset), hipp_dci_reg4.u32);

    return;
}

hi_void pq_reg_dci_set_flat_mode(vdp_regs_type *reg, hi_u32 offset, hi_u32 flat_mode)
{
    u_hipp_dci_reg5 hipp_dci_reg5;

    hipp_dci_reg5.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg5.u32) + offset));
    hipp_dci_reg5.bits.flat_mode = flat_mode;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg5.u32) + offset), hipp_dci_reg5.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_hor_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_end)
{
    u_hipp_dci_reg9 hipp_dci_reg9;

    hipp_dci_reg9.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg9.u32) + offset));
    hipp_dci_reg9.bits.global_in_hor_end = global_in_hor_end;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg9.u32) + offset), hipp_dci_reg9.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_hor_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_start)
{
    u_hipp_dci_reg9 hipp_dci_reg9;

    hipp_dci_reg9.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg9.u32) + offset));
    hipp_dci_reg9.bits.global_in_hor_start = global_in_hor_start;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg9.u32) + offset), hipp_dci_reg9.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_ver_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_end)
{
    u_hipp_dci_reg10 hipp_dci_reg10;

    hipp_dci_reg10.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg10.u32) + offset));
    hipp_dci_reg10.bits.global_in_ver_end = global_in_ver_end;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg10.u32) + offset), hipp_dci_reg10.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_ver_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_start)
{
    u_hipp_dci_reg10 hipp_dci_reg10;

    hipp_dci_reg10.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg10.u32) + offset));
    hipp_dci_reg10.bits.global_in_ver_start = global_in_ver_start;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg10.u32) + offset), hipp_dci_reg10.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_hor_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_end)
{
    u_hipp_dci_reg11 hipp_dci_reg11;

    hipp_dci_reg11.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg11.u32) + offset));
    hipp_dci_reg11.bits.global_out_hor_end = global_out_hor_end;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg11.u32) + offset), hipp_dci_reg11.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_hor_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_start)
{
    u_hipp_dci_reg11 hipp_dci_reg11;

    hipp_dci_reg11.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg11.u32) + offset));
    hipp_dci_reg11.bits.global_out_hor_start = global_out_hor_start;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg11.u32) + offset), hipp_dci_reg11.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_ver_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_end)
{
    u_hipp_dci_reg12 hipp_dci_reg12;

    hipp_dci_reg12.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg12.u32) + offset));
    hipp_dci_reg12.bits.global_out_ver_end = global_out_ver_end;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg12.u32) + offset), hipp_dci_reg12.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_ver_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_start)
{
    u_hipp_dci_reg12 hipp_dci_reg12;

    hipp_dci_reg12.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg12.u32) + offset));
    hipp_dci_reg12.bits.global_out_ver_start = global_out_ver_start;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg12.u32) + offset), hipp_dci_reg12.u32);

    return;
}

hi_void pq_reg_dci_set_scd_flag(vdp_regs_type *reg, hi_u32 offset, hi_u32 scd_flag)
{
    u_hipp_dci_reg20 hipp_dci_reg20;

    hipp_dci_reg20.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg20.u32) + offset));
    hipp_dci_reg20.bits.scd_flag = scd_flag;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg20.u32) + offset), hipp_dci_reg20.u32);

    return;
}

hi_void pq_reg_dci_get_scd_sad_thrsh(vdp_regs_type *reg, hi_u32 offset, hi_u32 *scd_sad_thrsh)
{
    u_hipp_dci_reg20 hipp_dci_reg20;

    hipp_dci_reg20.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg20.u32) + offset));
    *scd_sad_thrsh = hipp_dci_reg20.bits.scd_sad_thrsh;

    return;
}

hi_void pq_reg_dci_get_scd_sad_shift(vdp_regs_type *reg, hi_u32 offset, hi_u32 *scd_sad_shift)
{
    u_hipp_dci_reg20 hipp_dci_reg20;

    hipp_dci_reg20.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg20.u32) + offset));
    *scd_sad_shift = hipp_dci_reg20.bits.scd_sad_shift;

    return;
}

hi_void pq_reg_dci_set_local_hor_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_end)
{
    u_hipp_dci_reg40 hipp_dci_reg40;

    hipp_dci_reg40.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40.u32) + offset));
    hipp_dci_reg40.bits.local_hor_end = local_hor_end;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40.u32) + offset), hipp_dci_reg40.u32);

    return;
}

hi_void pq_reg_dci_set_local_hor_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_start)
{
    u_hipp_dci_reg40 hipp_dci_reg40;

    hipp_dci_reg40.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40.u32) + offset));
    hipp_dci_reg40.bits.local_hor_start = local_hor_start;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40.u32) + offset), hipp_dci_reg40.u32);

    return;
}

hi_void pq_reg_dci_set_local_ver_end(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_end)
{
    u_hipp_dci_reg41 hipp_dci_reg41;

    hipp_dci_reg41.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41.u32) + offset));
    hipp_dci_reg41.bits.local_ver_end = local_ver_end;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41.u32) + offset), hipp_dci_reg41.u32);

    return;
}

hi_void pq_reg_dci_set_local_ver_start(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_start)
{
    u_hipp_dci_reg41 hipp_dci_reg41;

    hipp_dci_reg41.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41.u32) + offset));
    hipp_dci_reg41.bits.local_ver_start = local_ver_start;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41.u32) + offset), hipp_dci_reg41.u32);

    return;
}

hi_void pq_reg_dci_set_cg_div_width(vdp_regs_type *reg, hi_u32 offset, hi_u32 cg_div_width)
{
    u_hipp_dci_reg47 hipp_dci_reg47;

    hipp_dci_reg47.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg47.u32) + offset));
    hipp_dci_reg47.bits.cg_div_width = cg_div_width;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg47.u32) + offset), hipp_dci_reg47.u32);

    return;
}

hi_void pq_reg_dci_set_cg_div_height(vdp_regs_type *reg, hi_u32 offset, hi_u32 cg_div_height)
{
    u_hipp_dci_reg48 hipp_dci_reg48;

    hipp_dci_reg48.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg48.u32) + offset));
    hipp_dci_reg48.bits.cg_div_height = cg_div_height;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg48.u32) + offset), hipp_dci_reg48.u32);

    return;
}

hi_void pq_reg_dci_set_ncount_rshf(vdp_regs_type *reg, hi_u32 offset, hi_u32 ncount_rshf)
{
    u_hipp_dci_reg50 hipp_dci_reg50;

    hipp_dci_reg50.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg50.u32) + offset));
    hipp_dci_reg50.bits.ncount_rshf = ncount_rshf;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg50.u32) + offset), hipp_dci_reg50.u32);

    return;
}

hi_void pq_reg_dci_get_bs_min_black_thrsh(vdp_regs_type *reg, hi_u32 offset, hi_u32 *min_black_thrsh)
{
    u_hipp_dci_reg7 hipp_dci_reg7;
    hipp_dci_reg7.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg7.u32) + offset));
    *min_black_thrsh = hipp_dci_reg7.bits.bs_min_black_thrsh;
    return;
}

hi_void pq_reg_dci_set_glb_adj_lcl_gain(vdp_regs_type *reg, hi_u32 offset, hi_u32 glb_adj_lcl_gain)
{
    u_hipp_dci_reg51 hipp_dci_reg51;

    hipp_dci_reg51.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg51.u32) + offset));
    hipp_dci_reg51.bits.glb_adj_lcl_gain = glb_adj_lcl_gain;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg51.u32) + offset), hipp_dci_reg51.u32);

    return;
}

hi_void pq_reg_dci_set_global_total_pix_num(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_total_pix_num)
{
    u_hipp_dci_reg52 hipp_dci_reg52;

    hipp_dci_reg52.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg52.u32) + offset));
    hipp_dci_reg52.bits.global_total_pix_num = global_total_pix_num;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg52.u32) + offset), hipp_dci_reg52.u32);

    return;
}

hi_void pq_reg_dci_set_global_total_pix_num_bld(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_total_pix_num_bld)
{
    u_hipp_dci_reg53 hipp_dci_reg53;

    hipp_dci_reg53.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg53.u32) + offset));
    hipp_dci_reg53.bits.global_total_pix_num_bld = global_total_pix_num_bld;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg53.u32) + offset), hipp_dci_reg53.u32);

    return;
}

hi_void pq_reg_dci_set_split_point_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_point_s0)
{
    u_hipp_dci_reg4_s0 hipp_dci_reg4_s0;

    hipp_dci_reg4_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset));
    hipp_dci_reg4_s0.bits.split_point_s0 = split_point_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset), hipp_dci_reg4_s0.u32);

    return;
}

hi_void pq_reg_dci_set_split_type_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_type_s0)
{
    u_hipp_dci_reg4_s0 hipp_dci_reg4_s0;

    hipp_dci_reg4_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset));
    hipp_dci_reg4_s0.bits.split_type_s0 = split_type_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset), hipp_dci_reg4_s0.u32);

    return;
}

hi_void pq_reg_dci_set_split_hist_mode_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_hist_mode_s0)
{
    u_hipp_dci_reg4_s0 hipp_dci_reg4_s0;

    hipp_dci_reg4_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset));
    hipp_dci_reg4_s0.bits.split_hist_mode_s0 = split_hist_mode_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset), hipp_dci_reg4_s0.u32);

    return;
}

hi_void pq_reg_dci_set_demo_side_shift_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_side_shift_s0)
{
    u_hipp_dci_reg4_s0 hipp_dci_reg4_s0;

    hipp_dci_reg4_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset));
    hipp_dci_reg4_s0.bits.demo_side_shift_s0 = demo_side_shift_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset), hipp_dci_reg4_s0.u32);

    return;
}

hi_void pq_reg_dci_set_demo_split_pos_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_split_pos_s0)
{
    u_hipp_dci_reg4_s0 hipp_dci_reg4_s0;

    hipp_dci_reg4_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset));
    hipp_dci_reg4_s0.bits.demo_split_pos_s0 = demo_split_pos_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset), hipp_dci_reg4_s0.u32);

    return;
}

hi_void pq_reg_dci_set_demo_mode_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_mode_s0)
{
    u_hipp_dci_reg4_s0 hipp_dci_reg4_s0;

    hipp_dci_reg4_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset));
    hipp_dci_reg4_s0.bits.demo_mode_s0 = demo_mode_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s0.u32) + offset), hipp_dci_reg4_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_hor_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_end_s0)
{
    u_hipp_dci_reg9_s0 hipp_dci_reg9_s0;

    hipp_dci_reg9_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg9_s0.u32) + offset));
    hipp_dci_reg9_s0.bits.global_in_hor_end_s0 = global_in_hor_end_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg9_s0.u32) + offset), hipp_dci_reg9_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_hor_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_start_s0)
{
    u_hipp_dci_reg9_s0 hipp_dci_reg9_s0;

    hipp_dci_reg9_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg9_s0.u32) + offset));
    hipp_dci_reg9_s0.bits.global_in_hor_start_s0 = global_in_hor_start_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg9_s0.u32) + offset), hipp_dci_reg9_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_ver_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_end_s0)
{
    u_hipp_dci_reg10_s0 hipp_dci_reg10_s0;

    hipp_dci_reg10_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg10_s0.u32) + offset));
    hipp_dci_reg10_s0.bits.global_in_ver_end_s0 = global_in_ver_end_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg10_s0.u32) + offset), hipp_dci_reg10_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_ver_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_start_s0)
{
    u_hipp_dci_reg10_s0 hipp_dci_reg10_s0;

    hipp_dci_reg10_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg10_s0.u32) + offset));
    hipp_dci_reg10_s0.bits.global_in_ver_start_s0 = global_in_ver_start_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg10_s0.u32) + offset), hipp_dci_reg10_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_hor_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_end_s0)
{
    u_hipp_dci_reg11_s0 hipp_dci_reg11_s0;

    hipp_dci_reg11_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg11_s0.u32) + offset));
    hipp_dci_reg11_s0.bits.global_out_hor_end_s0 = global_out_hor_end_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg11_s0.u32) + offset), hipp_dci_reg11_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_hor_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_start_s0)
{
    u_hipp_dci_reg11_s0 hipp_dci_reg11_s0;

    hipp_dci_reg11_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg11_s0.u32) + offset));
    hipp_dci_reg11_s0.bits.global_out_hor_start_s0 = global_out_hor_start_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg11_s0.u32) + offset), hipp_dci_reg11_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_ver_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_end_s0)
{
    u_hipp_dci_reg12_s0 hipp_dci_reg12_s0;

    hipp_dci_reg12_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg12_s0.u32) + offset));
    hipp_dci_reg12_s0.bits.global_out_ver_end_s0 = global_out_ver_end_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg12_s0.u32) + offset), hipp_dci_reg12_s0.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_ver_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_start_s0)
{
    u_hipp_dci_reg12_s0 hipp_dci_reg12_s0;

    hipp_dci_reg12_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg12_s0.u32) + offset));
    hipp_dci_reg12_s0.bits.global_out_ver_start_s0 = global_out_ver_start_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg12_s0.u32) + offset), hipp_dci_reg12_s0.u32);

    return;
}

hi_void pq_reg_dci_set_local_hor_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_end_s0)
{
    u_hipp_dci_reg40_s0 hipp_dci_reg40_s0;

    hipp_dci_reg40_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40_s0.u32) + offset));
    hipp_dci_reg40_s0.bits.local_hor_end_s0 = local_hor_end_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40_s0.u32) + offset), hipp_dci_reg40_s0.u32);

    return;
}

hi_void pq_reg_dci_set_local_hor_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_start_s0)
{
    u_hipp_dci_reg40_s0 hipp_dci_reg40_s0;

    hipp_dci_reg40_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40_s0.u32) + offset));
    hipp_dci_reg40_s0.bits.local_hor_start_s0 = local_hor_start_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40_s0.u32) + offset), hipp_dci_reg40_s0.u32);

    return;
}

hi_void pq_reg_dci_set_local_gain_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_gain_s0)
{
    u_hipp_dci_reg40_s0 hipp_dci_reg40_s0;

    hipp_dci_reg40_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40_s0.u32) + offset));
    hipp_dci_reg40_s0.bits.local_gain_s0 = local_gain_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40_s0.u32) + offset), hipp_dci_reg40_s0.u32);

    return;
}

hi_void pq_reg_dci_set_local_centre_adj_wgt_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_centre_adj_wgt_s0)
{
    u_hipp_dci_reg41_s0 hipp_dci_reg41_s0;

    hipp_dci_reg41_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41_s0.u32) + offset));
    hipp_dci_reg41_s0.bits.local_centre_adj_wgt_s0 = local_centre_adj_wgt_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41_s0.u32) + offset), hipp_dci_reg41_s0.u32);

    return;
}

hi_void pq_reg_dci_set_local_ver_end_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_end_s0)
{
    u_hipp_dci_reg41_s0 hipp_dci_reg41_s0;

    hipp_dci_reg41_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41_s0.u32) + offset));
    hipp_dci_reg41_s0.bits.local_ver_end_s0 = local_ver_end_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41_s0.u32) + offset), hipp_dci_reg41_s0.u32);

    return;
}

hi_void pq_reg_dci_set_local_ver_start_s0(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_start_s0)
{
    u_hipp_dci_reg41_s0 hipp_dci_reg41_s0;

    hipp_dci_reg41_s0.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41_s0.u32) + offset));
    hipp_dci_reg41_s0.bits.local_ver_start_s0 = local_ver_start_s0;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41_s0.u32) + offset), hipp_dci_reg41_s0.u32);

    return;
}

hi_void pq_reg_dci_set_split_point_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_point_s1)
{
    u_hipp_dci_reg4_s1 hipp_dci_reg4_s1;

    hipp_dci_reg4_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset));
    hipp_dci_reg4_s1.bits.split_point_s1 = split_point_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset), hipp_dci_reg4_s1.u32);

    return;
}

hi_void pq_reg_dci_set_split_type_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_type_s1)
{
    u_hipp_dci_reg4_s1 hipp_dci_reg4_s1;

    hipp_dci_reg4_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset));
    hipp_dci_reg4_s1.bits.split_type_s1 = split_type_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset), hipp_dci_reg4_s1.u32);

    return;
}

hi_void pq_reg_dci_set_split_hist_mode_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 split_hist_mode_s1)
{
    u_hipp_dci_reg4_s1 hipp_dci_reg4_s1;

    hipp_dci_reg4_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset));
    hipp_dci_reg4_s1.bits.split_hist_mode_s1 = split_hist_mode_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset), hipp_dci_reg4_s1.u32);

    return;
}

hi_void pq_reg_dci_set_demo_side_shift_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_side_shift_s1)
{
    u_hipp_dci_reg4_s1 hipp_dci_reg4_s1;

    hipp_dci_reg4_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset));
    hipp_dci_reg4_s1.bits.demo_side_shift_s1 = demo_side_shift_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset), hipp_dci_reg4_s1.u32);

    return;
}

hi_void pq_reg_dci_set_demo_split_pos_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_split_pos_s1)
{
    u_hipp_dci_reg4_s1 hipp_dci_reg4_s1;

    hipp_dci_reg4_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset));
    hipp_dci_reg4_s1.bits.demo_split_pos_s1 = demo_split_pos_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset), hipp_dci_reg4_s1.u32);

    return;
}

hi_void pq_reg_dci_set_demo_mode_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 demo_mode_s1)
{
    u_hipp_dci_reg4_s1 hipp_dci_reg4_s1;

    hipp_dci_reg4_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset));
    hipp_dci_reg4_s1.bits.demo_mode_s1 = demo_mode_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg4_s1.u32) + offset), hipp_dci_reg4_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_hor_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_end_s1)
{
    u_hipp_dci_reg9_s1 hipp_dci_reg9_s1;

    hipp_dci_reg9_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg9_s1.u32) + offset));
    hipp_dci_reg9_s1.bits.global_in_hor_end_s1 = global_in_hor_end_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg9_s1.u32) + offset), hipp_dci_reg9_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_hor_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_hor_start_s1)
{
    u_hipp_dci_reg9_s1 hipp_dci_reg9_s1;

    hipp_dci_reg9_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg9_s1.u32) + offset));
    hipp_dci_reg9_s1.bits.global_in_hor_start_s1 = global_in_hor_start_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg9_s1.u32) + offset), hipp_dci_reg9_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_ver_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_end_s1)
{
    u_hipp_dci_reg10_s1 hipp_dci_reg10_s1;

    hipp_dci_reg10_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg10_s1.u32) + offset));
    hipp_dci_reg10_s1.bits.global_in_ver_end_s1 = global_in_ver_end_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg10_s1.u32) + offset), hipp_dci_reg10_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_in_ver_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_in_ver_start_s1)
{
    u_hipp_dci_reg10_s1 hipp_dci_reg10_s1;

    hipp_dci_reg10_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg10_s1.u32) + offset));
    hipp_dci_reg10_s1.bits.global_in_ver_start_s1 = global_in_ver_start_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg10_s1.u32) + offset), hipp_dci_reg10_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_hor_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_end_s1)
{
    u_hipp_dci_reg11_s1 hipp_dci_reg11_s1;

    hipp_dci_reg11_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg11_s1.u32) + offset));
    hipp_dci_reg11_s1.bits.global_out_hor_end_s1 = global_out_hor_end_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg11_s1.u32) + offset), hipp_dci_reg11_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_hor_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_hor_start_s1)
{
    u_hipp_dci_reg11_s1 hipp_dci_reg11_s1;

    hipp_dci_reg11_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg11_s1.u32) + offset));
    hipp_dci_reg11_s1.bits.global_out_hor_start_s1 = global_out_hor_start_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg11_s1.u32) + offset), hipp_dci_reg11_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_ver_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_end_s1)
{
    u_hipp_dci_reg12_s1 hipp_dci_reg12_s1;

    hipp_dci_reg12_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg12_s1.u32) + offset));
    hipp_dci_reg12_s1.bits.global_out_ver_end_s1 = global_out_ver_end_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg12_s1.u32) + offset), hipp_dci_reg12_s1.u32);

    return;
}

hi_void pq_reg_dci_set_global_out_ver_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 global_out_ver_start_s1)
{
    u_hipp_dci_reg12_s1 hipp_dci_reg12_s1;

    hipp_dci_reg12_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg12_s1.u32) + offset));
    hipp_dci_reg12_s1.bits.global_out_ver_start_s1 = global_out_ver_start_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg12_s1.u32) + offset), hipp_dci_reg12_s1.u32);

    return;
}

hi_void pq_reg_dci_set_local_hor_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_end_s1)
{
    u_hipp_dci_reg40_s1 hipp_dci_reg40_s1;

    hipp_dci_reg40_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40_s1.u32) + offset));
    hipp_dci_reg40_s1.bits.local_hor_end_s1 = local_hor_end_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40_s1.u32) + offset), hipp_dci_reg40_s1.u32);

    return;
}

hi_void pq_reg_dci_set_local_hor_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_hor_start_s1)
{
    u_hipp_dci_reg40_s1 hipp_dci_reg40_s1;

    hipp_dci_reg40_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40_s1.u32) + offset));
    hipp_dci_reg40_s1.bits.local_hor_start_s1 = local_hor_start_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40_s1.u32) + offset), hipp_dci_reg40_s1.u32);

    return;
}

hi_void pq_reg_dci_set_local_gain_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_gain_s1)
{
    u_hipp_dci_reg40_s1 hipp_dci_reg40_s1;

    hipp_dci_reg40_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg40_s1.u32) + offset));
    hipp_dci_reg40_s1.bits.local_gain_s1 = local_gain_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg40_s1.u32) + offset), hipp_dci_reg40_s1.u32);

    return;
}

hi_void pq_reg_dci_set_local_centre_adj_wgt_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_centre_adj_wgt_s1)
{
    u_hipp_dci_reg41_s1 hipp_dci_reg41_s1;

    hipp_dci_reg41_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41_s1.u32) + offset));
    hipp_dci_reg41_s1.bits.local_centre_adj_wgt_s1 = local_centre_adj_wgt_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41_s1.u32) + offset), hipp_dci_reg41_s1.u32);

    return;
}

hi_void pq_reg_dci_set_local_ver_end_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_end_s1)
{
    u_hipp_dci_reg41_s1 hipp_dci_reg41_s1;

    hipp_dci_reg41_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41_s1.u32) + offset));
    hipp_dci_reg41_s1.bits.local_ver_end_s1 = local_ver_end_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41_s1.u32) + offset), hipp_dci_reg41_s1.u32);

    return;
}

hi_void pq_reg_dci_set_local_ver_start_s1(vdp_regs_type *reg, hi_u32 offset, hi_u32 local_ver_start_s1)
{
    u_hipp_dci_reg41_s1 hipp_dci_reg41_s1;

    hipp_dci_reg41_s1.u32 = pq_new_regread((uintptr_t)(&(reg->hipp_dci_reg41_s1.u32) + offset));
    hipp_dci_reg41_s1.bits.local_ver_start_s1 = local_ver_start_s1;
    pq_new_regwrite((uintptr_t)(&(reg->hipp_dci_reg41_s1.u32) + offset), hipp_dci_reg41_s1.u32);

    return;
}

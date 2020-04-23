/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_vhdr_cm.h"
#include "hal_vdp_comm.h"

hi_void vdp_vhdr_cm_setvhdrcmckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_ck_gt_en)
{
    u_vhdr_cm_ctrl vhdr_cm_ctrl;

    vhdr_cm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset));
    vhdr_cm_ctrl.bits.vhdr_cm_ck_gt_en = vhdr_cm_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset), vhdr_cm_ctrl.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvhdrcmen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_en)
{
    u_vhdr_cm_ctrl vhdr_cm_ctrl;

    vhdr_cm_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset));
    vhdr_cm_ctrl.bits.vhdr_cm_en = vhdr_cm_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_ctrl.u32) + offset), vhdr_cm_ctrl.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvhdrcmclipcmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_max)
{
    u_vhdr_cm_clip vhdr_cm_clip;

    vhdr_cm_clip.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset));
    vhdr_cm_clip.bits.vhdr_cm_clip_c_max = vhdr_cm_clip_c_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset), vhdr_cm_clip.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvhdrcmclipcmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_min)
{
    u_vhdr_cm_clip vhdr_cm_clip;

    vhdr_cm_clip.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset));
    vhdr_cm_clip.bits.vhdr_cm_clip_c_min = vhdr_cm_clip_c_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_clip.u32) + offset), vhdr_cm_clip.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvhdrcmscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vhdr_cm_scale2p vhdr_cm_scale2p;

    vhdr_cm_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_scale2p.u32) + offset));
    vhdr_cm_scale2p.bits.vhdr_cm_scale2p = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_scale2p.u32) + offset), vhdr_cm_scale2p.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvhdrcmrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_rd_en)
{
    u_vhdr_cm_para_ren vhdr_cm_para_ren;

    vhdr_cm_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_para_ren.u32) + offset));
    vhdr_cm_para_ren.bits.vhdr_cm_rd_en = vhdr_cm_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_para_ren.u32) + offset), vhdr_cm_para_ren.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvhdrcmparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vhdr_cm_para_data vhdr_cm_para_data;

    vhdr_cm_para_data.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_para_data.u32) + offset));
    vhdr_cm_para_data.bits.vhdr_cm_para_data = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_para_data.u32) + offset), vhdr_cm_para_data.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx4step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x4_step = v_cm_x4_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx3step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x3_step = v_cm_x3_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx2step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x2_step = v_cm_x2_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx1step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_step)
{
    u_vhdr_cm_step1 vhdr_cm_step1;

    vhdr_cm_step1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset));
    vhdr_cm_step1.bits.v_cm_x1_step = v_cm_x1_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step1.u32) + offset), vhdr_cm_step1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx8step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x8_step = v_cm_x8_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx7step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x7_step = v_cm_x7_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx6step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x6_step = v_cm_x6_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx5step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_step)
{
    u_vhdr_cm_step2 vhdr_cm_step2;

    vhdr_cm_step2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset));
    vhdr_cm_step2.bits.v_cm_x5_step = v_cm_x5_step;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_step2.u32) + offset), vhdr_cm_step2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx1pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_pos)
{
    u_vhdr_cm_pos1 vhdr_cm_pos1;

    vhdr_cm_pos1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos1.u32) + offset));
    vhdr_cm_pos1.bits.v_cm_x1_pos = v_cm_x1_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos1.u32) + offset), vhdr_cm_pos1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx2pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_pos)
{
    u_vhdr_cm_pos2 vhdr_cm_pos2;

    vhdr_cm_pos2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos2.u32) + offset));
    vhdr_cm_pos2.bits.v_cm_x2_pos = v_cm_x2_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos2.u32) + offset), vhdr_cm_pos2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx3pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_pos)
{
    u_vhdr_cm_pos3 vhdr_cm_pos3;

    vhdr_cm_pos3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos3.u32) + offset));
    vhdr_cm_pos3.bits.v_cm_x3_pos = v_cm_x3_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos3.u32) + offset), vhdr_cm_pos3.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx4pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_pos)
{
    u_vhdr_cm_pos4 vhdr_cm_pos4;

    vhdr_cm_pos4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos4.u32) + offset));
    vhdr_cm_pos4.bits.v_cm_x4_pos = v_cm_x4_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos4.u32) + offset), vhdr_cm_pos4.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx5pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_pos)
{
    u_vhdr_cm_pos5 vhdr_cm_pos5;

    vhdr_cm_pos5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos5.u32) + offset));
    vhdr_cm_pos5.bits.v_cm_x5_pos = v_cm_x5_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos5.u32) + offset), vhdr_cm_pos5.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx6pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_pos)
{
    u_vhdr_cm_pos6 vhdr_cm_pos6;

    vhdr_cm_pos6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos6.u32) + offset));
    vhdr_cm_pos6.bits.v_cm_x6_pos = v_cm_x6_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos6.u32) + offset), vhdr_cm_pos6.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx7pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_pos)
{
    u_vhdr_cm_pos7 vhdr_cm_pos7;

    vhdr_cm_pos7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos7.u32) + offset));
    vhdr_cm_pos7.bits.v_cm_x7_pos = v_cm_x7_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos7.u32) + offset), vhdr_cm_pos7.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx8pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_pos)
{
    u_vhdr_cm_pos8 vhdr_cm_pos8;

    vhdr_cm_pos8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_pos8.u32) + offset));
    vhdr_cm_pos8.bits.v_cm_x8_pos = v_cm_x8_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_pos8.u32) + offset), vhdr_cm_pos8.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx4num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x4_num = v_cm_x4_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx3num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x3_num = v_cm_x3_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx2num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x2_num = v_cm_x2_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx1num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_num)
{
    u_vhdr_cm_num1 vhdr_cm_num1;

    vhdr_cm_num1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset));
    vhdr_cm_num1.bits.v_cm_x1_num = v_cm_x1_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num1.u32) + offset), vhdr_cm_num1.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx8num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x8_num = v_cm_x8_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx7num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x7_num = v_cm_x7_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx6num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x6_num = v_cm_x6_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return ;
}

hi_void vdp_vhdr_cm_setvcmx5num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_num)
{
    u_vhdr_cm_num2 vhdr_cm_num2;

    vhdr_cm_num2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset));
    vhdr_cm_num2.bits.v_cm_x5_num = v_cm_x5_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vhdr_cm_num2.u32) + offset), vhdr_cm_num2.u32);

    return ;
}




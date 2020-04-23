/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_hipp_csc.h"

hi_void vdp_hipp_csc_sethippcscckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_ck_gt_en)
{
    u_vdp_hipp_csc_ctrl hipp_csc_ctrl;

    hipp_csc_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ctrl.u32) + offset));
    hipp_csc_ctrl.bits.hipp_csc_ck_gt_en = hipp_csc_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ctrl.u32) + offset), hipp_csc_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_demo_en)
{
    u_vdp_hipp_csc_ctrl hipp_csc_ctrl;

    hipp_csc_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ctrl.u32) + offset));
    hipp_csc_ctrl.bits.hipp_csc_demo_en = hipp_csc_demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ctrl.u32) + offset), hipp_csc_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_en)
{
    u_vdp_hipp_csc_ctrl hipp_csc_ctrl;

    hipp_csc_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ctrl.u32) + offset));
    hipp_csc_ctrl.bits.hipp_csc_en = hipp_csc_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ctrl.u32) + offset), hipp_csc_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef00 hipp_csc_coef00;

    hipp_csc_coef00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef00.u32) + offset));
    hipp_csc_coef00.bits.hipp_csc_coef00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef00.u32) + offset), hipp_csc_coef00.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef01 hipp_csc_coef01;

    hipp_csc_coef01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef01.u32) + offset));
    hipp_csc_coef01.bits.hipp_csc_coef01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef01.u32) + offset), hipp_csc_coef01.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef02 hipp_csc_coef02;

    hipp_csc_coef02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef02.u32) + offset));
    hipp_csc_coef02.bits.hipp_csc_coef02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef02.u32) + offset), hipp_csc_coef02.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef10 hipp_csc_coef10;

    hipp_csc_coef10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef10.u32) + offset));
    hipp_csc_coef10.bits.hipp_csc_coef10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef10.u32) + offset), hipp_csc_coef10.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef11 hipp_csc_coef11;

    hipp_csc_coef11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef11.u32) + offset));
    hipp_csc_coef11.bits.hipp_csc_coef11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef11.u32) + offset), hipp_csc_coef11.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef12 hipp_csc_coef12;

    hipp_csc_coef12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef12.u32) + offset));
    hipp_csc_coef12.bits.hipp_csc_coef12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef12.u32) + offset), hipp_csc_coef12.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef20 hipp_csc_coef20;

    hipp_csc_coef20.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef20.u32) + offset));
    hipp_csc_coef20.bits.hipp_csc_coef20 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef20.u32) + offset), hipp_csc_coef20.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef21 hipp_csc_coef21;

    hipp_csc_coef21.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef21.u32) + offset));
    hipp_csc_coef21.bits.hipp_csc_coef21 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef21.u32) + offset), hipp_csc_coef21.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsccoef22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_coef22 hipp_csc_coef22;

    hipp_csc_coef22.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef22.u32) + offset));
    hipp_csc_coef22.bits.hipp_csc_coef22 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_coef22.u32) + offset), hipp_csc_coef22.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscscale(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_scale hipp_csc_scale;

    hipp_csc_scale.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_scale.u32) + offset));
    hipp_csc_scale.bits.hipp_csc_scale = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_scale.u32) + offset), hipp_csc_scale.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscidc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_idc0 hipp_csc_idc0;

    hipp_csc_idc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_idc0.u32) + offset));
    hipp_csc_idc0.bits.hipp_csc_idc0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_idc0.u32) + offset), hipp_csc_idc0.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscidc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_idc1 hipp_csc_idc1;

    hipp_csc_idc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_idc1.u32) + offset));
    hipp_csc_idc1.bits.hipp_csc_idc1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_idc1.u32) + offset), hipp_csc_idc1.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscidc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_idc2 hipp_csc_idc2;

    hipp_csc_idc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_idc2.u32) + offset));
    hipp_csc_idc2.bits.hipp_csc_idc2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_idc2.u32) + offset), hipp_csc_idc2.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscodc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_odc0 hipp_csc_odc0;

    hipp_csc_odc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_odc0.u32) + offset));
    hipp_csc_odc0.bits.hipp_csc_odc0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_odc0.u32) + offset), hipp_csc_odc0.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscodc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_odc1 hipp_csc_odc1;

    hipp_csc_odc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_odc1.u32) + offset));
    hipp_csc_odc1.bits.hipp_csc_odc1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_odc1.u32) + offset), hipp_csc_odc1.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscodc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_odc2 hipp_csc_odc2;

    hipp_csc_odc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_odc2.u32) + offset));
    hipp_csc_odc2.bits.hipp_csc_odc2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_odc2.u32) + offset), hipp_csc_odc2.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscminy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_min_y hipp_csc_min_y;

    hipp_csc_min_y.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_min_y.u32) + offset));
    hipp_csc_min_y.bits.hipp_csc_min_y = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_min_y.u32) + offset), hipp_csc_min_y.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscminc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_min_c hipp_csc_min_c;

    hipp_csc_min_c.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_min_c.u32) + offset));
    hipp_csc_min_c.bits.hipp_csc_min_c = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_min_c.u32) + offset), hipp_csc_min_c.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscmaxy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_max_y hipp_csc_max_y;

    hipp_csc_max_y.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_max_y.u32) + offset));
    hipp_csc_max_y.bits.hipp_csc_max_y = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_max_y.u32) + offset), hipp_csc_max_y.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcscmaxc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc_max_c hipp_csc_max_c;

    hipp_csc_max_c.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_max_c.u32) + offset));
    hipp_csc_max_c.bits.hipp_csc_max_c = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_max_c.u32) + offset), hipp_csc_max_c.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef00 hipp_csc2_coef00;

    hipp_csc2_coef00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef00.u32) + offset));
    hipp_csc2_coef00.bits.hipp_csc2_coef00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef00.u32) + offset), hipp_csc2_coef00.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef01 hipp_csc2_coef01;

    hipp_csc2_coef01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef01.u32) + offset));
    hipp_csc2_coef01.bits.hipp_csc2_coef01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef01.u32) + offset), hipp_csc2_coef01.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef02 hipp_csc2_coef02;

    hipp_csc2_coef02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef02.u32) + offset));
    hipp_csc2_coef02.bits.hipp_csc2_coef02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef02.u32) + offset), hipp_csc2_coef02.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef10 hipp_csc2_coef10;

    hipp_csc2_coef10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef10.u32) + offset));
    hipp_csc2_coef10.bits.hipp_csc2_coef10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef10.u32) + offset), hipp_csc2_coef10.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef11 hipp_csc2_coef11;

    hipp_csc2_coef11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef11.u32) + offset));
    hipp_csc2_coef11.bits.hipp_csc2_coef11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef11.u32) + offset), hipp_csc2_coef11.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef12 hipp_csc2_coef12;

    hipp_csc2_coef12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef12.u32) + offset));
    hipp_csc2_coef12.bits.hipp_csc2_coef12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef12.u32) + offset), hipp_csc2_coef12.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef20 hipp_csc2_coef20;

    hipp_csc2_coef20.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef20.u32) + offset));
    hipp_csc2_coef20.bits.hipp_csc2_coef20 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef20.u32) + offset), hipp_csc2_coef20.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef21 hipp_csc2_coef21;

    hipp_csc2_coef21.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef21.u32) + offset));
    hipp_csc2_coef21.bits.hipp_csc2_coef21 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef21.u32) + offset), hipp_csc2_coef21.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2coef22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_coef22 hipp_csc2_coef22;

    hipp_csc2_coef22.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef22.u32) + offset));
    hipp_csc2_coef22.bits.hipp_csc2_coef22 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_coef22.u32) + offset), hipp_csc2_coef22.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2scale(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_scale hipp_csc2_scale;

    hipp_csc2_scale.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_scale.u32) + offset));
    hipp_csc2_scale.bits.hipp_csc2_scale = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_scale.u32) + offset), hipp_csc2_scale.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2idc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_idc0 hipp_csc2_idc0;

    hipp_csc2_idc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_idc0.u32) + offset));
    hipp_csc2_idc0.bits.hipp_csc2_idc0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_idc0.u32) + offset), hipp_csc2_idc0.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2idc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_idc1 hipp_csc2_idc1;

    hipp_csc2_idc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_idc1.u32) + offset));
    hipp_csc2_idc1.bits.hipp_csc2_idc1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_idc1.u32) + offset), hipp_csc2_idc1.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2idc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_idc2 hipp_csc2_idc2;

    hipp_csc2_idc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_idc2.u32) + offset));
    hipp_csc2_idc2.bits.hipp_csc2_idc2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_idc2.u32) + offset), hipp_csc2_idc2.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2odc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_odc0 hipp_csc2_odc0;

    hipp_csc2_odc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_odc0.u32) + offset));
    hipp_csc2_odc0.bits.hipp_csc2_odc0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_odc0.u32) + offset), hipp_csc2_odc0.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2odc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_odc1 hipp_csc2_odc1;

    hipp_csc2_odc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_odc1.u32) + offset));
    hipp_csc2_odc1.bits.hipp_csc2_odc1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_odc1.u32) + offset), hipp_csc2_odc1.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2odc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_odc2 hipp_csc2_odc2;

    hipp_csc2_odc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_odc2.u32) + offset));
    hipp_csc2_odc2.bits.hipp_csc2_odc2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_odc2.u32) + offset), hipp_csc2_odc2.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2miny(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_min_y hipp_csc2_min_y;

    hipp_csc2_min_y.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_min_y.u32) + offset));
    hipp_csc2_min_y.bits.hipp_csc2_min_y = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_min_y.u32) + offset), hipp_csc2_min_y.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2minc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_min_c hipp_csc2_min_c;

    hipp_csc2_min_c.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_min_c.u32) + offset));
    hipp_csc2_min_c.bits.hipp_csc2_min_c = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_min_c.u32) + offset), hipp_csc2_min_c.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2maxy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_max_y hipp_csc2_max_y;

    hipp_csc2_max_y.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_max_y.u32) + offset));
    hipp_csc2_max_y.bits.hipp_csc2_max_y = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_max_y.u32) + offset), hipp_csc2_max_y.u32);

    return;
}

hi_void vdp_hipp_csc_sethippcsc2maxc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_vdp_hipp_csc2_max_c hipp_csc2_max_c;

    hipp_csc2_max_c.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_max_c.u32) + offset));
    hipp_csc2_max_c.bits.hipp_csc2_max_c = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc2_max_c.u32) + offset), hipp_csc2_max_c.u32);

    return;
}

hi_void vdp_hipp_csc_setcolormode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 color_mode)
{
    u_vdp_hipp_csc_ink_ctrl hipp_csc_ink_ctrl;

    hipp_csc_ink_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset));
    hipp_csc_ink_ctrl.bits.color_mode = color_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset), hipp_csc_ink_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_setcrossenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cross_enable)
{
    u_vdp_hipp_csc_ink_ctrl hipp_csc_ink_ctrl;

    hipp_csc_ink_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset));
    hipp_csc_ink_ctrl.bits.cross_enable = cross_enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset), hipp_csc_ink_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_setdatafmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data_fmt)
{
    u_vdp_hipp_csc_ink_ctrl hipp_csc_ink_ctrl;

    hipp_csc_ink_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset));
    hipp_csc_ink_ctrl.bits.data_fmt = data_fmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset), hipp_csc_ink_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_setinksel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ink_sel)
{
    u_vdp_hipp_csc_ink_ctrl hipp_csc_ink_ctrl;

    hipp_csc_ink_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset));
    hipp_csc_ink_ctrl.bits.ink_sel = ink_sel;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset), hipp_csc_ink_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_setinken(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ink_en)
{
    u_vdp_hipp_csc_ink_ctrl hipp_csc_ink_ctrl;

    hipp_csc_ink_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset));
    hipp_csc_ink_ctrl.bits.ink_en = ink_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_ctrl.u32) + offset), hipp_csc_ink_ctrl.u32);

    return;
}

hi_void vdp_hipp_csc_setypos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 y_pos)
{
    u_vdp_hipp_csc_ink_pos hipp_csc_ink_pos;

    hipp_csc_ink_pos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_pos.u32) + offset));
    hipp_csc_ink_pos.bits.y_pos = y_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_pos.u32) + offset), hipp_csc_ink_pos.u32);

    return;
}

hi_void vdp_hipp_csc_setxpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_pos)
{
    u_vdp_hipp_csc_ink_pos hipp_csc_ink_pos;

    hipp_csc_ink_pos.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_pos.u32) + offset));
    hipp_csc_ink_pos.bits.x_pos = x_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_pos.u32) + offset), hipp_csc_ink_pos.u32);

    return;
}

hi_u32 vdp_hipp_csc_getinkdata(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    return vdp_regread ((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_data.u32) + offset));
}

hi_u32 vdp_hipp_csc_getinkdata2(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    return vdp_regread ((uintptr_t)(&(vdp_reg->vdp_hipp_csc_ink_data2.u32) + offset));
}

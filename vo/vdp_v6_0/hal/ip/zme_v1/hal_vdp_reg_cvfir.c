/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_cvfir.h"

hi_void vdp_cvfir_setvzmeckgten(vdp_regs_type *vdp_reg, hi_u32 vzme_ck_gt_en)
{
    u_v3_cvfir_vinfo v3_cvfir_vinfo;

    v3_cvfir_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)));
    v3_cvfir_vinfo.bits.vzme_ck_gt_en = vzme_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)), v3_cvfir_vinfo.u32);

    return ;
}

hi_void vdp_cvfir_setoutpro(vdp_regs_type *vdp_reg, hi_u32 out_pro)
{
    u_v3_cvfir_vinfo v3_cvfir_vinfo;

    v3_cvfir_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)));
    v3_cvfir_vinfo.bits.out_pro = out_pro;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)), v3_cvfir_vinfo.u32);

    return ;
}

hi_void vdp_cvfir_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 out_fmt)
{
    u_v3_cvfir_vinfo v3_cvfir_vinfo;

    v3_cvfir_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)));
    v3_cvfir_vinfo.bits.out_fmt = out_fmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)), v3_cvfir_vinfo.u32);

    return ;
}

hi_void vdp_cvfir_setoutheight(vdp_regs_type *vdp_reg, hi_u32 out_height)
{
    u_v3_cvfir_vinfo v3_cvfir_vinfo;

    v3_cvfir_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)));
    v3_cvfir_vinfo.bits.out_height = out_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vinfo.u32)), v3_cvfir_vinfo.u32);

    return ;
}

hi_void vdp_cvfir_setcvfiren(vdp_regs_type *vdp_reg, hi_u32 cvfir_en)
{
    u_v3_cvfir_vsp v3_cvfir_vsp;

    v3_cvfir_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)));
    v3_cvfir_vsp.bits.cvfir_en = cvfir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)), v3_cvfir_vsp.u32);

    return ;
}

hi_void vdp_cvfir_setcvmiden(vdp_regs_type *vdp_reg, hi_u32 cvmid_en)
{
    u_v3_cvfir_vsp v3_cvfir_vsp;

    v3_cvfir_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)));
    v3_cvfir_vsp.bits.cvmid_en = cvmid_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)), v3_cvfir_vsp.u32);

    return ;
}

hi_void vdp_cvfir_setcvfirmode(vdp_regs_type *vdp_reg, hi_u32 cvfir_mode)
{
    u_v3_cvfir_vsp v3_cvfir_vsp;

    v3_cvfir_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)));
    v3_cvfir_vsp.bits.cvfir_mode = cvfir_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)), v3_cvfir_vsp.u32);

    return ;
}

hi_void vdp_cvfir_setvratio(vdp_regs_type *vdp_reg, hi_u32 vratio)
{
    u_v3_cvfir_vsp v3_cvfir_vsp;

    v3_cvfir_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)));
    v3_cvfir_vsp.bits.vratio = vratio;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vsp.u32)), v3_cvfir_vsp.u32);

    return ;
}

hi_void vdp_cvfir_setvchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vchroma_offset)
{
    u_v3_cvfir_voffset v3_cvfir_voffset;

    v3_cvfir_voffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_voffset.u32)));
    v3_cvfir_voffset.bits.vchroma_offset = vchroma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_voffset.u32)), v3_cvfir_voffset.u32);

    return ;
}

hi_void vdp_cvfir_setvbchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vbchroma_offset)
{
    u_v3_cvfir_vboffset v3_cvfir_vboffset;

    v3_cvfir_vboffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vboffset.u32)));
    v3_cvfir_vboffset.bits.vbchroma_offset = vbchroma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vboffset.u32)), v3_cvfir_vboffset.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef00(vdp_regs_type *vdp_reg, hi_u32 vccoef00)
{
    u_v3_cvfir_vcoef0 v3_cvfir_vcoef0;

    v3_cvfir_vcoef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef0.u32)));
    v3_cvfir_vcoef0.bits.vccoef00 = vccoef00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef0.u32)), v3_cvfir_vcoef0.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef01(vdp_regs_type *vdp_reg, hi_u32 vccoef01)
{
    u_v3_cvfir_vcoef0 v3_cvfir_vcoef0;

    v3_cvfir_vcoef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef0.u32)));
    v3_cvfir_vcoef0.bits.vccoef01 = vccoef01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef0.u32)), v3_cvfir_vcoef0.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef02(vdp_regs_type *vdp_reg, hi_u32 vccoef02)
{
    u_v3_cvfir_vcoef0 v3_cvfir_vcoef0;

    v3_cvfir_vcoef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef0.u32)));
    v3_cvfir_vcoef0.bits.vccoef02 = vccoef02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef0.u32)), v3_cvfir_vcoef0.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef03(vdp_regs_type *vdp_reg, hi_u32 vccoef03)
{
    u_v3_cvfir_vcoef1 v3_cvfir_vcoef1;

    v3_cvfir_vcoef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef1.u32)));
    v3_cvfir_vcoef1.bits.vccoef03 = vccoef03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef1.u32)), v3_cvfir_vcoef1.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef10(vdp_regs_type *vdp_reg, hi_u32 vccoef10)
{
    u_v3_cvfir_vcoef1 v3_cvfir_vcoef1;

    v3_cvfir_vcoef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef1.u32)));
    v3_cvfir_vcoef1.bits.vccoef10 = vccoef10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef1.u32)), v3_cvfir_vcoef1.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef11(vdp_regs_type *vdp_reg, hi_u32 vccoef11)
{
    u_v3_cvfir_vcoef1 v3_cvfir_vcoef1;

    v3_cvfir_vcoef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef1.u32)));
    v3_cvfir_vcoef1.bits.vccoef11 = vccoef11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef1.u32)), v3_cvfir_vcoef1.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef12(vdp_regs_type *vdp_reg, hi_u32 vccoef12)
{
    u_v3_cvfir_vcoef2 v3_cvfir_vcoef2;

    v3_cvfir_vcoef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef2.u32)));
    v3_cvfir_vcoef2.bits.vccoef12 = vccoef12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef2.u32)), v3_cvfir_vcoef2.u32);

    return ;
}

hi_void vdp_cvfir_setvccoef13(vdp_regs_type *vdp_reg, hi_u32 vccoef13)
{
    u_v3_cvfir_vcoef2 v3_cvfir_vcoef2;

    v3_cvfir_vcoef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef2.u32)));
    v3_cvfir_vcoef2.bits.vccoef13 = vccoef13;
    vdp_regwrite((uintptr_t)(&(vdp_reg->v3_cvfir_vcoef2.u32)), v3_cvfir_vcoef2.u32);

    return ;
}


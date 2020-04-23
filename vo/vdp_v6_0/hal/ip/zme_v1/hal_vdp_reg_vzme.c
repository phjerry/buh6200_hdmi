/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_vzme.h"
#include "hal_vdp_comm.h"

hi_void vdp_vzme_setckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ck_gt_en)
{
    u_vdp_zme_vinfo vdp_zme_vinfo;

    vdp_zme_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vinfo.u32) + offset));
    vdp_zme_vinfo.bits.ck_gt_en = ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vinfo.u32) + offset), vdp_zme_vinfo.u32);

    return;
}

hi_void vdp_vzme_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_fmt)
{
    u_vdp_zme_vinfo vdp_zme_vinfo;

    vdp_zme_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vinfo.u32) + offset));
    vdp_zme_vinfo.bits.out_fmt = out_fmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vinfo.u32) + offset), vdp_zme_vinfo.u32);

    return;
}

hi_void vdp_vzme_setoutheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_height)
{
    u_vdp_zme_vinfo vdp_zme_vinfo;

    vdp_zme_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vinfo.u32) + offset));
    vdp_zme_vinfo.bits.out_height = out_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vinfo.u32) + offset), vdp_zme_vinfo.u32);

    return;
}

hi_void vdp_vzme_setlvfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lvfir_en)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.lvfir_en = lvfir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setcvfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cvfir_en)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.cvfir_en = cvfir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setlvmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lvmid_en)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.lvmid_en = lvmid_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setcvmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cvmid_en)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.cvmid_en = cvmid_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setlvfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lvfir_mode)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.lvfir_mode = lvfir_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setcvfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cvfir_mode)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.cvfir_mode = cvfir_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_settapmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tap_mode)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.tap_mode = tap_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setvratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vratio)
{
    u_vdp_zme_vsp vdp_zme_vsp;

    vdp_zme_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset));
    vdp_zme_vsp.bits.vratio = vratio;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vsp.u32) + offset), vdp_zme_vsp.u32);

    return;
}

hi_void vdp_vzme_setvlumaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vluma_offset)
{
    u_vdp_zme_voffset vdp_zme_voffset;

    vdp_zme_voffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_voffset.u32) + offset));
    vdp_zme_voffset.bits.vluma_offset = vluma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_voffset.u32) + offset), vdp_zme_voffset.u32);

    return;
}

hi_void vdp_vzme_setvchromaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vchroma_offset)
{
    u_vdp_zme_voffset vdp_zme_voffset;

    vdp_zme_voffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_voffset.u32) + offset));
    vdp_zme_voffset.bits.vchroma_offset = vchroma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_voffset.u32) + offset), vdp_zme_voffset.u32);

    return;
}

hi_void vdp_vzme_setvblumaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vbluma_offset)
{
    u_vdp_zme_vboffset vdp_zme_vboffset;

    vdp_zme_vboffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vboffset.u32) + offset));
    vdp_zme_vboffset.bits.vbluma_offset = vbluma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vboffset.u32) + offset), vdp_zme_vboffset.u32);

    return;
}

hi_void vdp_vzme_setvbchromaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vbchroma_offset)
{
    u_vdp_zme_vboffset vdp_zme_vboffset;

    vdp_zme_vboffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vboffset.u32) + offset));
    vdp_zme_vboffset.bits.vbchroma_offset = vbchroma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vboffset.u32) + offset), vdp_zme_vboffset.u32);

    return;
}

hi_void vdp_vzme_setvlshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_shootctrl_en)
{
    u_vdp_zme_vl_shootctrl vdp_zme_vl_shootctrl;

    vdp_zme_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset));
    vdp_zme_vl_shootctrl.bits.vl_shootctrl_en = vl_shootctrl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset), vdp_zme_vl_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvlshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_shootctrl_mode)
{
    u_vdp_zme_vl_shootctrl vdp_zme_vl_shootctrl;

    vdp_zme_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset));
    vdp_zme_vl_shootctrl.bits.vl_shootctrl_mode = vl_shootctrl_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset), vdp_zme_vl_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvlflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_flatdect_mode)
{
    u_vdp_zme_vl_shootctrl vdp_zme_vl_shootctrl;

    vdp_zme_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset));
    vdp_zme_vl_shootctrl.bits.vl_flatdect_mode = vl_flatdect_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset), vdp_zme_vl_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvlcoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_coringadj_en)
{
    u_vdp_zme_vl_shootctrl vdp_zme_vl_shootctrl;

    vdp_zme_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset));
    vdp_zme_vl_shootctrl.bits.vl_coringadj_en = vl_coringadj_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset), vdp_zme_vl_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvlgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_gain)
{
    u_vdp_zme_vl_shootctrl vdp_zme_vl_shootctrl;

    vdp_zme_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset));
    vdp_zme_vl_shootctrl.bits.vl_gain = vl_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset), vdp_zme_vl_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvlcoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_coring)
{
    u_vdp_zme_vl_shootctrl vdp_zme_vl_shootctrl;

    vdp_zme_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset));
    vdp_zme_vl_shootctrl.bits.vl_coring = vl_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vl_shootctrl.u32) + offset), vdp_zme_vl_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvcshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_shootctrl_en)
{
    u_vdp_zme_vc_shootctrl vdp_zme_vc_shootctrl;

    vdp_zme_vc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset));
    vdp_zme_vc_shootctrl.bits.vc_shootctrl_en = vc_shootctrl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset), vdp_zme_vc_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvcshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_shootctrl_mode)
{
    u_vdp_zme_vc_shootctrl vdp_zme_vc_shootctrl;

    vdp_zme_vc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset));
    vdp_zme_vc_shootctrl.bits.vc_shootctrl_mode = vc_shootctrl_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset), vdp_zme_vc_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvcflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_flatdect_mode)
{
    u_vdp_zme_vc_shootctrl vdp_zme_vc_shootctrl;

    vdp_zme_vc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset));
    vdp_zme_vc_shootctrl.bits.vc_flatdect_mode = vc_flatdect_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset), vdp_zme_vc_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvccoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_coringadj_en)
{
    u_vdp_zme_vc_shootctrl vdp_zme_vc_shootctrl;

    vdp_zme_vc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset));
    vdp_zme_vc_shootctrl.bits.vc_coringadj_en = vc_coringadj_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset), vdp_zme_vc_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvcgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_gain)
{
    u_vdp_zme_vc_shootctrl vdp_zme_vc_shootctrl;

    vdp_zme_vc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset));
    vdp_zme_vc_shootctrl.bits.vc_gain = vc_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset), vdp_zme_vc_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setvccoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_coring)
{
    u_vdp_zme_vc_shootctrl vdp_zme_vc_shootctrl;

    vdp_zme_vc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset));
    vdp_zme_vc_shootctrl.bits.vc_coring = vc_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vc_shootctrl.u32) + offset), vdp_zme_vc_shootctrl.u32);

    return;
}

hi_void vdp_vzme_setapbvhdvflren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vf_lren)
{
    u_vdp_zme_vrcoef vdp_zme_vrcoef;

    vdp_zme_vrcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vrcoef.u32) + offset));
    vdp_zme_vrcoef.bits.apb_vhd_vf_lren = apb_vhd_vf_lren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vrcoef.u32) + offset), vdp_zme_vrcoef.u32);

    return;
}

hi_void vdp_vzme_setapbvhdvfcren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vf_cren)
{
    u_vdp_zme_vrcoef vdp_zme_vrcoef;

    vdp_zme_vrcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vrcoef.u32) + offset));
    vdp_zme_vrcoef.bits.apb_vhd_vf_cren = apb_vhd_vf_cren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vrcoef.u32) + offset), vdp_zme_vrcoef.u32);

    return;
}

hi_void vdp_vzme_setapbvhdvycoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vycoef_raddr)
{
    u_vdp_zme_vycoefad vdp_zme_vycoefad;

    vdp_zme_vycoefad.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vycoefad.u32) + offset));
    vdp_zme_vycoefad.bits.apb_vhd_vycoef_raddr = apb_vhd_vycoef_raddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vycoefad.u32) + offset), vdp_zme_vycoefad.u32);

    return;
}

hi_void vdp_vzme_setapbvhdvccoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vccoef_raddr)
{
    u_vdp_zme_vccoefad vdp_zme_vccoefad;

    vdp_zme_vccoefad.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_vccoefad.u32) + offset));
    vdp_zme_vccoefad.bits.apb_vhd_vccoef_raddr = apb_vhd_vccoef_raddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_vccoefad.u32) + offset), vdp_zme_vccoefad.u32);

    return;
}

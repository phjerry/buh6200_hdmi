/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_hzme.h"
#include "hal_vdp_comm.h"

hi_void vdp_hzme_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_fmt)
{
    u_vdp_zme_hinfo vdp_zme_hinfo;

    vdp_zme_hinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hinfo.u32) + offset));
    vdp_zme_hinfo.bits.out_fmt = out_fmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hinfo.u32) + offset), vdp_zme_hinfo.u32);

    return;
}

hi_void vdp_hzme_setckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ck_gt_en)
{
    u_vdp_zme_hinfo vdp_zme_hinfo;

    vdp_zme_hinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hinfo.u32) + offset));
    vdp_zme_hinfo.bits.ck_gt_en = ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hinfo.u32) + offset), vdp_zme_hinfo.u32);

    return;
}

hi_void vdp_hzme_setoutwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_width)
{
    u_vdp_zme_hinfo vdp_zme_hinfo;

    vdp_zme_hinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hinfo.u32) + offset));
    vdp_zme_hinfo.bits.out_width = out_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hinfo.u32) + offset), vdp_zme_hinfo.u32);

    return;
}

hi_void vdp_hzme_setinwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 in_width)
{
    u_vdp_zme_ireso vdp_zme_ireso;

    vdp_zme_ireso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_ireso.u32) + offset));
    vdp_zme_ireso.bits.in_width = in_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_ireso.u32) + offset), vdp_zme_ireso.u32);

    return;
}

hi_void vdp_hzme_setlhfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhfir_en)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.lhfir_en = lhfir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setchfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chfir_en)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.chfir_en = chfir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setlhmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhmid_en)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.lhmid_en = lhmid_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setchmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chmid_en)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.chmid_en = chmid_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setnonlnren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 non_lnr_en)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.non_lnr_en = non_lnr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setlhfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhfir_mode)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.lhfir_mode = lhfir_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setchfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chfir_mode)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.chfir_mode = chfir_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_sethfirorder(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hfir_order)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.hfir_order = hfir_order;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_sethratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hratio)
{
    u_vdp_zme_hsp vdp_zme_hsp;

    vdp_zme_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset));
    vdp_zme_hsp.bits.hratio = hratio;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hsp.u32) + offset), vdp_zme_hsp.u32);

    return;
}

hi_void vdp_hzme_setlhfiroffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhfir_offset)
{
    u_vdp_zme_hloffset vdp_zme_hloffset;

    vdp_zme_hloffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hloffset.u32) + offset));
    vdp_zme_hloffset.bits.lhfir_offset = lhfir_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hloffset.u32) + offset), vdp_zme_hloffset.u32);

    return;
}

hi_void vdp_hzme_setchfiroffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chfir_offset)
{
    u_vdp_zme_hcoffset vdp_zme_hcoffset;

    vdp_zme_hcoffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hcoffset.u32) + offset));
    vdp_zme_hcoffset.bits.chfir_offset = chfir_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hcoffset.u32) + offset), vdp_zme_hcoffset.u32);

    return;
}

hi_void vdp_hzme_setzone0delta(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone0_delta)
{
    u_vdp_zme_hzone0delta vdp_zme_hzone0delta;

    vdp_zme_hzone0delta.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hzone0delta.u32) + offset));
    vdp_zme_hzone0delta.bits.zone0_delta = zone0_delta;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hzone0delta.u32) + offset), vdp_zme_hzone0delta.u32);

    return;
}

hi_void vdp_hzme_setzone2delta(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone2_delta)
{
    u_vdp_zme_hzone2delta vdp_zme_hzone2delta;

    vdp_zme_hzone2delta.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hzone2delta.u32) + offset));
    vdp_zme_hzone2delta.bits.zone2_delta = zone2_delta;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hzone2delta.u32) + offset), vdp_zme_hzone2delta.u32);

    return;
}

hi_void vdp_hzme_setzone1end(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone1_end)
{
    u_vdp_zme_hzoneend vdp_zme_hzoneend;

    vdp_zme_hzoneend.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hzoneend.u32) + offset));
    vdp_zme_hzoneend.bits.zone1_end = zone1_end;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hzoneend.u32) + offset), vdp_zme_hzoneend.u32);

    return;
}

hi_void vdp_hzme_setzone0end(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone0_end)
{
    u_vdp_zme_hzoneend vdp_zme_hzoneend;

    vdp_zme_hzoneend.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hzoneend.u32) + offset));
    vdp_zme_hzoneend.bits.zone0_end = zone0_end;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hzoneend.u32) + offset), vdp_zme_hzoneend.u32);

    return;
}

hi_void vdp_hzme_sethlshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_shootctrl_en)
{
    u_vdp_zme_hl_shootctrl vdp_zme_hl_shootctrl;

    vdp_zme_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset));
    vdp_zme_hl_shootctrl.bits.hl_shootctrl_en = hl_shootctrl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset), vdp_zme_hl_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethlshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_shootctrl_mode)
{
    u_vdp_zme_hl_shootctrl vdp_zme_hl_shootctrl;

    vdp_zme_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset));
    vdp_zme_hl_shootctrl.bits.hl_shootctrl_mode = hl_shootctrl_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset), vdp_zme_hl_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethlflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_flatdect_mode)
{
    u_vdp_zme_hl_shootctrl vdp_zme_hl_shootctrl;

    vdp_zme_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset));
    vdp_zme_hl_shootctrl.bits.hl_flatdect_mode = hl_flatdect_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset), vdp_zme_hl_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethlcoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_coringadj_en)
{
    u_vdp_zme_hl_shootctrl vdp_zme_hl_shootctrl;

    vdp_zme_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset));
    vdp_zme_hl_shootctrl.bits.hl_coringadj_en = hl_coringadj_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset), vdp_zme_hl_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethlgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_gain)
{
    u_vdp_zme_hl_shootctrl vdp_zme_hl_shootctrl;

    vdp_zme_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset));
    vdp_zme_hl_shootctrl.bits.hl_gain = hl_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset), vdp_zme_hl_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethlcoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_coring)
{
    u_vdp_zme_hl_shootctrl vdp_zme_hl_shootctrl;

    vdp_zme_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset));
    vdp_zme_hl_shootctrl.bits.hl_coring = hl_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hl_shootctrl.u32) + offset), vdp_zme_hl_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethcshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_shootctrl_en)
{
    u_vdp_zme_hc_shootctrl vdp_zme_hc_shootctrl;

    vdp_zme_hc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset));
    vdp_zme_hc_shootctrl.bits.hc_shootctrl_en = hc_shootctrl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset), vdp_zme_hc_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethcshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_shootctrl_mode)
{
    u_vdp_zme_hc_shootctrl vdp_zme_hc_shootctrl;

    vdp_zme_hc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset));
    vdp_zme_hc_shootctrl.bits.hc_shootctrl_mode = hc_shootctrl_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset), vdp_zme_hc_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethcflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_flatdect_mode)
{
    u_vdp_zme_hc_shootctrl vdp_zme_hc_shootctrl;

    vdp_zme_hc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset));
    vdp_zme_hc_shootctrl.bits.hc_flatdect_mode = hc_flatdect_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset), vdp_zme_hc_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethccoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_coringadj_en)
{
    u_vdp_zme_hc_shootctrl vdp_zme_hc_shootctrl;

    vdp_zme_hc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset));
    vdp_zme_hc_shootctrl.bits.hc_coringadj_en = hc_coringadj_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset), vdp_zme_hc_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethcgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_gain)
{
    u_vdp_zme_hc_shootctrl vdp_zme_hc_shootctrl;

    vdp_zme_hc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset));
    vdp_zme_hc_shootctrl.bits.hc_gain = hc_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset), vdp_zme_hc_shootctrl.u32);

    return;
}

hi_void vdp_hzme_sethccoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_coring)
{
    u_vdp_zme_hc_shootctrl vdp_zme_hc_shootctrl;

    vdp_zme_hc_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset));
    vdp_zme_hc_shootctrl.bits.hc_coring = hc_coring;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hc_shootctrl.u32) + offset), vdp_zme_hc_shootctrl.u32);

    return;
}

hi_void vdp_hzme_setapbvhdhflren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hf_lren)
{
    u_vdp_zme_hrcoef vdp_zme_hrcoef;

    vdp_zme_hrcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hrcoef.u32) + offset));
    vdp_zme_hrcoef.bits.apb_vhd_hf_lren = apb_vhd_hf_lren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hrcoef.u32) + offset), vdp_zme_hrcoef.u32);

    return;
}

hi_void vdp_hzme_setapbvhdhfcren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hf_cren)
{
    u_vdp_zme_hrcoef vdp_zme_hrcoef;

    vdp_zme_hrcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hrcoef.u32) + offset));
    vdp_zme_hrcoef.bits.apb_vhd_hf_cren = apb_vhd_hf_cren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hrcoef.u32) + offset), vdp_zme_hrcoef.u32);

    return;
}

hi_void vdp_hzme_setapbvhdhycoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hycoef_raddr)
{
    u_vdp_zme_hycoefad vdp_zme_hycoefad;

    vdp_zme_hycoefad.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hycoefad.u32) + offset));
    vdp_zme_hycoefad.bits.apb_vhd_hycoef_raddr = apb_vhd_hycoef_raddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hycoefad.u32) + offset), vdp_zme_hycoefad.u32);

    return;
}

hi_void vdp_hzme_setapbvhdhccoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hccoef_raddr)
{
    u_vdp_zme_hccoefad vdp_zme_hccoefad;

    vdp_zme_hccoefad.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_zme_hccoefad.u32) + offset));
    vdp_zme_hccoefad.bits.apb_vhd_hccoef_raddr = apb_vhd_hccoef_raddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_zme_hccoefad.u32) + offset), vdp_zme_hccoefad.u32);

    return;
}

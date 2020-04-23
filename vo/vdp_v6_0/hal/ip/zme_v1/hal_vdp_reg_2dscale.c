/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_2dscale.h"
#include "hal_vdp_comm.h"

hi_void vdp_xdp_2dscale_setclk2dscalefixen(vdp_regs_type *vdp_reg, hi_u32 clk_2dscale_fix_en)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.clk_2dscale_fix_en = clk_2dscale_fix_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setclk2dscalediv2en(vdp_regs_type *vdp_reg, hi_u32 clk_2dscale_div2_en)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.clk_2dscale_div2_en = clk_2dscale_div2_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_settapreduceen(vdp_regs_type *vdp_reg, hi_u32 tap_reduce_en)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.tap_reduce_en = tap_reduce_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setckgten(vdp_regs_type *vdp_reg, hi_u32 ck_gt_en)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.ck_gt_en = ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setoutpro(vdp_regs_type *vdp_reg, hi_u32 out_pro)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.out_pro = out_pro;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 out_fmt)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.out_fmt = out_fmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setoutheight(vdp_regs_type *vdp_reg, hi_u32 out_height)
{
    u_xdp_2dscale_vinfo xdp_2dscale_vinfo;

    xdp_2dscale_vinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)));
    xdp_2dscale_vinfo.bits.out_height = out_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vinfo.u32)), xdp_2dscale_vinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsclen(vdp_regs_type *vdp_reg, hi_u32 lv_scl_en)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.lv_scl_en = lv_scl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcvsclen(vdp_regs_type *vdp_reg, hi_u32 cv_scl_en)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.cv_scl_en = cv_scl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setscl2den(vdp_regs_type *vdp_reg, hi_u32 scl2d_en)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.scl2d_en = scl2d_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcvmeden(vdp_regs_type *vdp_reg, hi_u32 cv_med_en)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.cv_med_en = cv_med_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvflten(vdp_regs_type *vdp_reg, hi_u32 lv_flt_en)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.lv_flt_en = lv_flt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcvflten(vdp_regs_type *vdp_reg, hi_u32 cv_flt_en)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.cv_flt_en = cv_flt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setvratio(vdp_regs_type *vdp_reg, hi_u32 vratio)
{
    u_xdp_2dscale_vsp xdp_2dscale_vsp;

    xdp_2dscale_vsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)));
    xdp_2dscale_vsp.bits.vratio = vratio;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vsp.u32)), xdp_2dscale_vsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setvlumaoffset(vdp_regs_type *vdp_reg, hi_u32 vluma_offset)
{
    u_xdp_2dscale_voffset xdp_2dscale_voffset;

    xdp_2dscale_voffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_voffset.u32)));
    xdp_2dscale_voffset.bits.vluma_offset = vluma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_voffset.u32)), xdp_2dscale_voffset.u32);

    return;
}

hi_void vdp_xdp_2dscale_setvchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vchroma_offset)
{
    u_xdp_2dscale_voffset xdp_2dscale_voffset;

    xdp_2dscale_voffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_voffset.u32)));
    xdp_2dscale_voffset.bits.vchroma_offset = vchroma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_voffset.u32)), xdp_2dscale_voffset.u32);

    return;
}

hi_void vdp_xdp_2dscale_setvblumaoffset(vdp_regs_type *vdp_reg, hi_u32 vbluma_offset)
{
    u_xdp_2dscale_vboffset xdp_2dscale_vboffset;

    xdp_2dscale_vboffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vboffset.u32)));
    xdp_2dscale_vboffset.bits.vbluma_offset = vbluma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vboffset.u32)), xdp_2dscale_vboffset.u32);

    return;
}

hi_void vdp_xdp_2dscale_setvbchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vbchroma_offset)
{
    u_xdp_2dscale_vboffset xdp_2dscale_vboffset;

    xdp_2dscale_vboffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vboffset.u32)));
    xdp_2dscale_vboffset.bits.vbchroma_offset = vbchroma_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vboffset.u32)), xdp_2dscale_vboffset.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2nden(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_en)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2nd_en = lv_stc2nd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2ndusen(vdp_regs_type *vdp_reg, hi_u32 lv_stc2ndus_en)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2ndus_en = lv_stc2ndus_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2ndosen(vdp_regs_type *vdp_reg, hi_u32 lv_stc2ndos_en)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2ndos_en = lv_stc2ndos_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2ndflatmode(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_flatmode)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2nd_flatmode = lv_stc2nd_flatmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2ndcoradjen(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_coradjen)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2nd_coradjen = lv_stc2nd_coradjen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2ndgain(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_gain)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2nd_gain = lv_stc2nd_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvstc2ndcor(vdp_regs_type *vdp_reg, hi_u32 lv_stc2nd_cor)
{
    u_xdp_2dscale_vl_shootctrl xdp_2dscale_vl_shootctrl;

    xdp_2dscale_vl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)));
    xdp_2dscale_vl_shootctrl.bits.lv_stc2nd_cor = lv_stc2nd_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vl_shootctrl.u32)), xdp_2dscale_vl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setapbvhdvflren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_vf_lren)
{
    u_xdp_2dscale_vcoef xdp_2dscale_vcoef;

    xdp_2dscale_vcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vcoef.u32)));
    xdp_2dscale_vcoef.bits.apb_vhd_vf_lren = apb_vhd_vf_lren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vcoef.u32)), xdp_2dscale_vcoef.u32);

    return;
}

hi_void vdp_xdp_2dscale_setapbvhdvfcren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_vf_cren)
{
    u_xdp_2dscale_vcoef xdp_2dscale_vcoef;

    xdp_2dscale_vcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vcoef.u32)));
    xdp_2dscale_vcoef.bits.apb_vhd_vf_cren = apb_vhd_vf_cren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vcoef.u32)), xdp_2dscale_vcoef.u32);

    return;
}

hi_void vdp_xdp_2dscale_setapbvhdvcoefraddr(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_vcoef_raddr)
{
    u_xdp_2dscale_vcoefad xdp_2dscale_vcoefad;

    xdp_2dscale_vcoefad.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vcoefad.u32)));
    xdp_2dscale_vcoefad.bits.apb_vhd_vcoef_raddr = apb_vhd_vcoef_raddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vcoefad.u32)), xdp_2dscale_vcoefad.u32);

    return;
}

hi_void vdp_xdp_2dscale_setoutwidth(vdp_regs_type *vdp_reg, hi_u32 out_width)
{
    u_xdp_2dscale_hinfo xdp_2dscale_hinfo;

    xdp_2dscale_hinfo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hinfo.u32)));
    xdp_2dscale_hinfo.bits.out_width = out_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hinfo.u32)), xdp_2dscale_hinfo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsclen(vdp_regs_type *vdp_reg, hi_u32 lh_scl_en)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.lh_scl_en = lh_scl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setchsclen(vdp_regs_type *vdp_reg, hi_u32 ch_scl_en)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.ch_scl_en = ch_scl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setchmeden(vdp_regs_type *vdp_reg, hi_u32 ch_med_en)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.ch_med_en = ch_med_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setnonlinearsclen(vdp_regs_type *vdp_reg, hi_u32 nonlinear_scl_en)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.nonlinear_scl_en = nonlinear_scl_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhflten(vdp_regs_type *vdp_reg, hi_u32 lh_flt_en)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.lh_flt_en = lh_flt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setchflten(vdp_regs_type *vdp_reg, hi_u32 ch_flt_en)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.ch_flt_en = ch_flt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_sethratio(vdp_regs_type *vdp_reg, hi_u32 hratio)
{
    u_xdp_2dscale_hsp xdp_2dscale_hsp;

    xdp_2dscale_hsp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)));
    xdp_2dscale_hsp.bits.hratio = hratio;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hsp.u32)), xdp_2dscale_hsp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhfiroffset(vdp_regs_type *vdp_reg, hi_u32 lhfir_offset)
{
    u_xdp_2dscale_hloffset xdp_2dscale_hloffset;

    xdp_2dscale_hloffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hloffset.u32)));
    xdp_2dscale_hloffset.bits.lhfir_offset = lhfir_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hloffset.u32)), xdp_2dscale_hloffset.u32);

    return;
}

hi_void vdp_xdp_2dscale_setchfiroffset(vdp_regs_type *vdp_reg, hi_u32 chfir_offset)
{
    u_xdp_2dscale_hcoffset xdp_2dscale_hcoffset;

    xdp_2dscale_hcoffset.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoffset.u32)));
    xdp_2dscale_hcoffset.bits.chfir_offset = chfir_offset;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoffset.u32)), xdp_2dscale_hcoffset.u32);

    return;
}

hi_void vdp_xdp_2dscale_setzone0delta(vdp_regs_type *vdp_reg, hi_u32 zone0_delta)
{
    u_xdp_2dscale_hzone0delta xdp_2dscale_hzone0delta;

    xdp_2dscale_hzone0delta.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hzone0delta.u32)));
    xdp_2dscale_hzone0delta.bits.zone0_delta = zone0_delta;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hzone0delta.u32)), xdp_2dscale_hzone0delta.u32);

    return;
}

hi_void vdp_xdp_2dscale_setzone2delta(vdp_regs_type *vdp_reg, hi_u32 zone2_delta)
{
    u_xdp_2dscale_hzone2delta xdp_2dscale_hzone2delta;

    xdp_2dscale_hzone2delta.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hzone2delta.u32)));
    xdp_2dscale_hzone2delta.bits.zone2_delta = zone2_delta;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hzone2delta.u32)), xdp_2dscale_hzone2delta.u32);

    return;
}

hi_void vdp_xdp_2dscale_setzone1end(vdp_regs_type *vdp_reg, hi_u32 zone1_end)
{
    u_xdp_2dscale_hzoneend xdp_2dscale_hzoneend;

    xdp_2dscale_hzoneend.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hzoneend.u32)));
    xdp_2dscale_hzoneend.bits.zone1_end = zone1_end;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hzoneend.u32)), xdp_2dscale_hzoneend.u32);

    return;
}

hi_void vdp_xdp_2dscale_setzone0end(vdp_regs_type *vdp_reg, hi_u32 zone0_end)
{
    u_xdp_2dscale_hzoneend xdp_2dscale_hzoneend;

    xdp_2dscale_hzoneend.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hzoneend.u32)));
    xdp_2dscale_hzoneend.bits.zone0_end = zone0_end;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hzoneend.u32)), xdp_2dscale_hzoneend.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2nden(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_en)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2nd_en = lh_stc2nd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2ndusen(vdp_regs_type *vdp_reg, hi_u32 lh_stc2ndus_en)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2ndus_en = lh_stc2ndus_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2ndosen(vdp_regs_type *vdp_reg, hi_u32 lh_stc2ndos_en)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2ndos_en = lh_stc2ndos_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2ndflatmode(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_flatmode)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2nd_flatmode = lh_stc2nd_flatmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2ndcoradjen(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_coradjen)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2nd_coradjen = lh_stc2nd_coradjen;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2ndgain(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_gain)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2nd_gain = lh_stc2nd_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhstc2ndcor(vdp_regs_type *vdp_reg, hi_u32 lh_stc2nd_cor)
{
    u_xdp_2dscale_hl_shootctrl xdp_2dscale_hl_shootctrl;

    xdp_2dscale_hl_shootctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)));
    xdp_2dscale_hl_shootctrl.bits.lh_stc2nd_cor = lh_stc2nd_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hl_shootctrl.u32)), xdp_2dscale_hl_shootctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setapbvhdhflren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_hf_lren)
{
    u_xdp_2dscale_hcoef xdp_2dscale_hcoef;

    xdp_2dscale_hcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoef.u32)));
    xdp_2dscale_hcoef.bits.apb_vhd_hf_lren = apb_vhd_hf_lren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoef.u32)), xdp_2dscale_hcoef.u32);

    return;
}

hi_void vdp_xdp_2dscale_setapbvhdhfcren(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_hf_cren)
{
    u_xdp_2dscale_hcoef xdp_2dscale_hcoef;

    xdp_2dscale_hcoef.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoef.u32)));
    xdp_2dscale_hcoef.bits.apb_vhd_hf_cren = apb_vhd_hf_cren;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoef.u32)), xdp_2dscale_hcoef.u32);

    return;
}

hi_void vdp_xdp_2dscale_setapbvhdhcoefraddr(vdp_regs_type *vdp_reg, hi_u32 apb_vhd_hcoef_raddr)
{
    u_xdp_2dscale_hcoefad xdp_2dscale_hcoefad;

    xdp_2dscale_hcoefad.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoefad.u32)));
    xdp_2dscale_hcoefad.bits.apb_vhd_hcoef_raddr = apb_vhd_hcoef_raddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hcoefad.u32)), xdp_2dscale_hcoefad.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdren(vdp_regs_type *vdp_reg, hi_u32 lv_dr_en)
{
    u_xdp_2dscale_vctrl xdp_2dscale_vctrl;

    xdp_2dscale_vctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vctrl.u32)));
    xdp_2dscale_vctrl.bits.lv_dr_en = lv_dr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vctrl.u32)), xdp_2dscale_vctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdiren(vdp_regs_type *vdp_reg, hi_u32 lv_dir_en)
{
    u_xdp_2dscale_vctrl xdp_2dscale_vctrl;

    xdp_2dscale_vctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vctrl.u32)));
    xdp_2dscale_vctrl.bits.lv_dir_en = lv_dir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vctrl.u32)), xdp_2dscale_vctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvreslpfgain(vdp_regs_type *vdp_reg, hi_u32 lv_reslpf_gain)
{
    u_xdp_2dscale_vdirinterp xdp_2dscale_vdirinterp;

    xdp_2dscale_vdirinterp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vdirinterp.u32)));
    xdp_2dscale_vdirinterp.bits.lv_reslpf_gain = lv_reslpf_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vdirinterp.u32)), xdp_2dscale_vdirinterp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvreslpfen(vdp_regs_type *vdp_reg, hi_u32 lv_reslpf_en)
{
    u_xdp_2dscale_vdirinterp xdp_2dscale_vdirinterp;

    xdp_2dscale_vdirinterp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vdirinterp.u32)));
    xdp_2dscale_vdirinterp.bits.lv_reslpf_en = lv_reslpf_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vdirinterp.u32)), xdp_2dscale_vdirinterp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvprelpfthr(vdp_regs_type *vdp_reg, hi_u32 lv_prelpf_thr)
{
    u_xdp_2dscale_vdirinterp xdp_2dscale_vdirinterp;

    xdp_2dscale_vdirinterp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vdirinterp.u32)));
    xdp_2dscale_vdirinterp.bits.lv_prelpf_thr = lv_prelpf_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vdirinterp.u32)), xdp_2dscale_vdirinterp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrgradk(vdp_regs_type *vdp_reg, hi_u32 lv_dr_gradk)
{
    u_xdp_2dscale_vdering xdp_2dscale_vdering;

    xdp_2dscale_vdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vdering.u32)));
    xdp_2dscale_vdering.bits.lv_dr_gradk = lv_dr_gradk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vdering.u32)), xdp_2dscale_vdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrgradcor(vdp_regs_type *vdp_reg, hi_u32 lv_dr_gradcor)
{
    u_xdp_2dscale_vdering xdp_2dscale_vdering;

    xdp_2dscale_vdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vdering.u32)));
    xdp_2dscale_vdering.bits.lv_dr_gradcor = lv_dr_gradcor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vdering.u32)), xdp_2dscale_vdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlpfgain(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lpfgain)
{
    u_xdp_2dscale_vdering xdp_2dscale_vdering;

    xdp_2dscale_vdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_vdering.u32)));
    xdp_2dscale_vdering.bits.lv_dr_lpfgain = lv_dr_lpfgain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_vdering.u32)), xdp_2dscale_vdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdren(vdp_regs_type *vdp_reg, hi_u32 lh_dr_en)
{
    u_xdp_2dscale_hctrl xdp_2dscale_hctrl;

    xdp_2dscale_hctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hctrl.u32)));
    xdp_2dscale_hctrl.bits.lh_dr_en = lh_dr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hctrl.u32)), xdp_2dscale_hctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdiren(vdp_regs_type *vdp_reg, hi_u32 lh_dir_en)
{
    u_xdp_2dscale_hctrl xdp_2dscale_hctrl;

    xdp_2dscale_hctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hctrl.u32)));
    xdp_2dscale_hctrl.bits.lh_dir_en = lh_dir_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hctrl.u32)), xdp_2dscale_hctrl.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhreslpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_reslpf_gain)
{
    u_xdp_2dscale_hdirinterp xdp_2dscale_hdirinterp;

    xdp_2dscale_hdirinterp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdirinterp.u32)));
    xdp_2dscale_hdirinterp.bits.lh_reslpf_gain = lh_reslpf_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdirinterp.u32)), xdp_2dscale_hdirinterp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhreslpfen(vdp_regs_type *vdp_reg, hi_u32 lh_reslpf_en)
{
    u_xdp_2dscale_hdirinterp xdp_2dscale_hdirinterp;

    xdp_2dscale_hdirinterp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdirinterp.u32)));
    xdp_2dscale_hdirinterp.bits.lh_reslpf_en = lh_reslpf_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdirinterp.u32)), xdp_2dscale_hdirinterp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhprelpfthr(vdp_regs_type *vdp_reg, hi_u32 lh_prelpf_thr)
{
    u_xdp_2dscale_hdirinterp xdp_2dscale_hdirinterp;

    xdp_2dscale_hdirinterp.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdirinterp.u32)));
    xdp_2dscale_hdirinterp.bits.lh_prelpf_thr = lh_prelpf_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdirinterp.u32)), xdp_2dscale_hdirinterp.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrgradlpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_dr_gradlpfgain)
{
    u_xdp_2dscale_hdering xdp_2dscale_hdering;

    xdp_2dscale_hdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)));
    xdp_2dscale_hdering.bits.lh_dr_gradlpfgain = lh_dr_gradlpfgain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)), xdp_2dscale_hdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrgradk(vdp_regs_type *vdp_reg, hi_u32 lh_dr_gradk)
{
    u_xdp_2dscale_hdering xdp_2dscale_hdering;

    xdp_2dscale_hdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)));
    xdp_2dscale_hdering.bits.lh_dr_gradk = lh_dr_gradk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)), xdp_2dscale_hdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrgradcor(vdp_regs_type *vdp_reg, hi_u32 lh_dr_gradcor)
{
    u_xdp_2dscale_hdering xdp_2dscale_hdering;

    xdp_2dscale_hdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)));
    xdp_2dscale_hdering.bits.lh_dr_gradcor = lh_dr_gradcor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)), xdp_2dscale_hdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lpfgain)
{
    u_xdp_2dscale_hdering xdp_2dscale_hdering;

    xdp_2dscale_hdering.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)));
    xdp_2dscale_hdering.bits.lh_dr_lpfgain = lh_dr_lpfgain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_hdering.u32)), xdp_2dscale_hdering.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdemoen(vdp_regs_type *vdp_reg, hi_u32 demo_en)
{
    u_xdp_2dscale_demo xdp_2dscale_demo;

    xdp_2dscale_demo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)));
    xdp_2dscale_demo.bits.demo_en = demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)), xdp_2dscale_demo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdemomode(vdp_regs_type *vdp_reg, hi_u32 demo_mode)
{
    u_xdp_2dscale_demo xdp_2dscale_demo;

    xdp_2dscale_demo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)));
    xdp_2dscale_demo.bits.demo_mode = demo_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)), xdp_2dscale_demo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdemoincol(vdp_regs_type *vdp_reg, hi_u32 demo_in_col)
{
    u_xdp_2dscale_demo xdp_2dscale_demo;

    xdp_2dscale_demo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)));
    xdp_2dscale_demo.bits.demo_in_col = demo_in_col;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)), xdp_2dscale_demo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdemooutcol(vdp_regs_type *vdp_reg, hi_u32 demo_out_col)
{
    u_xdp_2dscale_demo xdp_2dscale_demo;

    xdp_2dscale_demo.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)));
    xdp_2dscale_demo.bits.demo_out_col = demo_out_col;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_demo.u32)), xdp_2dscale_demo.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcolend(vdp_regs_type *vdp_reg, hi_u32 col_end)
{
    u_xdp_2dscale_graph_0 xdp_2dscale_graph_0;

    xdp_2dscale_graph_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_0.u32)));
    xdp_2dscale_graph_0.bits.col_end = col_end;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_0.u32)), xdp_2dscale_graph_0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcolstart(vdp_regs_type *vdp_reg, hi_u32 col_start)
{
    u_xdp_2dscale_graph_0 xdp_2dscale_graph_0;

    xdp_2dscale_graph_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_0.u32)));
    xdp_2dscale_graph_0.bits.col_start = col_start;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_0.u32)), xdp_2dscale_graph_0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setgraphdeten(vdp_regs_type *vdp_reg, hi_u32 graph_det_en)
{
    u_xdp_2dscale_graph_0 xdp_2dscale_graph_0;

    xdp_2dscale_graph_0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_0.u32)));
    xdp_2dscale_graph_0.bits.graph_det_en = graph_det_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_0.u32)), xdp_2dscale_graph_0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setrowend(vdp_regs_type *vdp_reg, hi_u32 row_end)
{
    u_xdp_2dscale_graph_1 xdp_2dscale_graph_1;

    xdp_2dscale_graph_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_1.u32)));
    xdp_2dscale_graph_1.bits.row_end = row_end;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_1.u32)), xdp_2dscale_graph_1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setrowstart(vdp_regs_type *vdp_reg, hi_u32 row_start)
{
    u_xdp_2dscale_graph_1 xdp_2dscale_graph_1;

    xdp_2dscale_graph_1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_1.u32)));
    xdp_2dscale_graph_1.bits.row_start = row_start;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_1.u32)), xdp_2dscale_graph_1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdivheight(vdp_regs_type *vdp_reg, hi_u32 div_height)
{
    u_xdp_2dscale_graph_2 xdp_2dscale_graph_2;

    xdp_2dscale_graph_2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_2.u32)));
    xdp_2dscale_graph_2.bits.div_height = div_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_2.u32)), xdp_2dscale_graph_2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdivwidth(vdp_regs_type *vdp_reg, hi_u32 div_width)
{
    u_xdp_2dscale_graph_2 xdp_2dscale_graph_2;

    xdp_2dscale_graph_2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_2.u32)));
    xdp_2dscale_graph_2.bits.div_width = div_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_2.u32)), xdp_2dscale_graph_2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdifth3graph(vdp_regs_type *vdp_reg, hi_u32 difth3_graph)
{
    u_xdp_2dscale_graph_3 xdp_2dscale_graph_3;

    xdp_2dscale_graph_3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)));
    xdp_2dscale_graph_3.bits.difth3_graph = difth3_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)), xdp_2dscale_graph_3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdifth2graph(vdp_regs_type *vdp_reg, hi_u32 difth2_graph)
{
    u_xdp_2dscale_graph_3 xdp_2dscale_graph_3;

    xdp_2dscale_graph_3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)));
    xdp_2dscale_graph_3.bits.difth2_graph = difth2_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)), xdp_2dscale_graph_3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdifth1graph(vdp_regs_type *vdp_reg, hi_u32 difth1_graph)
{
    u_xdp_2dscale_graph_3 xdp_2dscale_graph_3;

    xdp_2dscale_graph_3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)));
    xdp_2dscale_graph_3.bits.difth1_graph = difth1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)), xdp_2dscale_graph_3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setdifth0graph(vdp_regs_type *vdp_reg, hi_u32 difth0_graph)
{
    u_xdp_2dscale_graph_3 xdp_2dscale_graph_3;

    xdp_2dscale_graph_3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)));
    xdp_2dscale_graph_3.bits.difth0_graph = difth0_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_3.u32)), xdp_2dscale_graph_3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcorek3graph(vdp_regs_type *vdp_reg, hi_u32 corek3_graph)
{
    u_xdp_2dscale_graph_4 xdp_2dscale_graph_4;

    xdp_2dscale_graph_4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_4.u32)));
    xdp_2dscale_graph_4.bits.corek3_graph = corek3_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_4.u32)), xdp_2dscale_graph_4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcorek2graph(vdp_regs_type *vdp_reg, hi_u32 corek2_graph)
{
    u_xdp_2dscale_graph_4 xdp_2dscale_graph_4;

    xdp_2dscale_graph_4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_4.u32)));
    xdp_2dscale_graph_4.bits.corek2_graph = corek2_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_4.u32)), xdp_2dscale_graph_4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setcorek1graph(vdp_regs_type *vdp_reg, hi_u32 corek1_graph)
{
    u_xdp_2dscale_graph_4 xdp_2dscale_graph_4;

    xdp_2dscale_graph_4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_4.u32)));
    xdp_2dscale_graph_4.bits.corek1_graph = corek1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_4.u32)), xdp_2dscale_graph_4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setx1kkgraph(vdp_regs_type *vdp_reg, hi_u32 x1_kk_graph)
{
    u_xdp_2dscale_graph_5 xdp_2dscale_graph_5;

    xdp_2dscale_graph_5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_5.u32)));
    xdp_2dscale_graph_5.bits.x1_kk_graph = x1_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_5.u32)), xdp_2dscale_graph_5.u32);

    return;
}

hi_void vdp_xdp_2dscale_setx0kkgraph(vdp_regs_type *vdp_reg, hi_u32 x0_kk_graph)
{
    u_xdp_2dscale_graph_5 xdp_2dscale_graph_5;

    xdp_2dscale_graph_5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_5.u32)));
    xdp_2dscale_graph_5.bits.x0_kk_graph = x0_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_5.u32)), xdp_2dscale_graph_5.u32);

    return;
}

hi_void vdp_xdp_2dscale_setg0kkgraph(vdp_regs_type *vdp_reg, hi_u32 g0_kk_graph)
{
    u_xdp_2dscale_graph_6 xdp_2dscale_graph_6;

    xdp_2dscale_graph_6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_6.u32)));
    xdp_2dscale_graph_6.bits.g0_kk_graph = g0_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_6.u32)), xdp_2dscale_graph_6.u32);

    return;
}

hi_void vdp_xdp_2dscale_setx2kkgraph(vdp_regs_type *vdp_reg, hi_u32 x2_kk_graph)
{
    u_xdp_2dscale_graph_6 xdp_2dscale_graph_6;

    xdp_2dscale_graph_6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_6.u32)));
    xdp_2dscale_graph_6.bits.x2_kk_graph = x2_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_6.u32)), xdp_2dscale_graph_6.u32);

    return;
}

hi_void vdp_xdp_2dscale_setg2kkgraph(vdp_regs_type *vdp_reg, hi_u32 g2_kk_graph)
{
    u_xdp_2dscale_graph_7 xdp_2dscale_graph_7;

    xdp_2dscale_graph_7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_7.u32)));
    xdp_2dscale_graph_7.bits.g2_kk_graph = g2_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_7.u32)), xdp_2dscale_graph_7.u32);

    return;
}

hi_void vdp_xdp_2dscale_setg1kkgraph(vdp_regs_type *vdp_reg, hi_u32 g1_kk_graph)
{
    u_xdp_2dscale_graph_7 xdp_2dscale_graph_7;

    xdp_2dscale_graph_7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_7.u32)));
    xdp_2dscale_graph_7.bits.g1_kk_graph = g1_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_7.u32)), xdp_2dscale_graph_7.u32);

    return;
}

hi_void vdp_xdp_2dscale_setk1kkgraph(vdp_regs_type *vdp_reg, hi_u32 k1_kk_graph)
{
    u_xdp_2dscale_graph_8 xdp_2dscale_graph_8;

    xdp_2dscale_graph_8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_8.u32)));
    xdp_2dscale_graph_8.bits.k1_kk_graph = k1_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_8.u32)), xdp_2dscale_graph_8.u32);

    return;
}

hi_void vdp_xdp_2dscale_setg3kkgraph(vdp_regs_type *vdp_reg, hi_u32 g3_kk_graph)
{
    u_xdp_2dscale_graph_8 xdp_2dscale_graph_8;

    xdp_2dscale_graph_8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_8.u32)));
    xdp_2dscale_graph_8.bits.g3_kk_graph = g3_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_8.u32)), xdp_2dscale_graph_8.u32);

    return;
}

hi_void vdp_xdp_2dscale_setk3kkgraph(vdp_regs_type *vdp_reg, hi_u32 k3_kk_graph)
{
    u_xdp_2dscale_graph_9 xdp_2dscale_graph_9;

    xdp_2dscale_graph_9.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_9.u32)));
    xdp_2dscale_graph_9.bits.k3_kk_graph = k3_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_9.u32)), xdp_2dscale_graph_9.u32);

    return;
}

hi_void vdp_xdp_2dscale_setk2kkgraph(vdp_regs_type *vdp_reg, hi_u32 k2_kk_graph)
{
    u_xdp_2dscale_graph_9 xdp_2dscale_graph_9;

    xdp_2dscale_graph_9.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_9.u32)));
    xdp_2dscale_graph_9.bits.k2_kk_graph = k2_kk_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_9.u32)), xdp_2dscale_graph_9.u32);

    return;
}

hi_void vdp_xdp_2dscale_setk3graph(vdp_regs_type *vdp_reg, hi_u32 k3_graph)
{
    u_xdp_2dscale_graph_a xdp_2dscale_graph_a;

    xdp_2dscale_graph_a.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_a.u32)));
    xdp_2dscale_graph_a.bits.k3_graph = k3_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_a.u32)), xdp_2dscale_graph_a.u32);

    return;
}

hi_void vdp_xdp_2dscale_setk2graph(vdp_regs_type *vdp_reg, hi_u32 k2_graph)
{
    u_xdp_2dscale_graph_a xdp_2dscale_graph_a;

    xdp_2dscale_graph_a.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_a.u32)));
    xdp_2dscale_graph_a.bits.k2_graph = k2_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_a.u32)), xdp_2dscale_graph_a.u32);

    return;
}

hi_void vdp_xdp_2dscale_setk1graph(vdp_regs_type *vdp_reg, hi_u32 k1_graph)
{
    u_xdp_2dscale_graph_a xdp_2dscale_graph_a;

    xdp_2dscale_graph_a.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_a.u32)));
    xdp_2dscale_graph_a.bits.k1_graph = k1_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_a.u32)), xdp_2dscale_graph_a.u32);

    return;
}

hi_void vdp_xdp_2dscale_setshiftgsgraph(vdp_regs_type *vdp_reg, hi_u32 shift_gs_graph)
{
    u_xdp_2dscale_graph_b xdp_2dscale_graph_b;

    xdp_2dscale_graph_b.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_b.u32)));
    xdp_2dscale_graph_b.bits.shift_gs_graph = shift_gs_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_b.u32)), xdp_2dscale_graph_b.u32);

    return;
}

hi_void vdp_xdp_2dscale_setx1gsgraph(vdp_regs_type *vdp_reg, hi_u32 x1_gs_graph)
{
    u_xdp_2dscale_graph_b xdp_2dscale_graph_b;

    xdp_2dscale_graph_b.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_b.u32)));
    xdp_2dscale_graph_b.bits.x1_gs_graph = x1_gs_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_b.u32)), xdp_2dscale_graph_b.u32);

    return;
}

hi_void vdp_xdp_2dscale_setx0gsgraph(vdp_regs_type *vdp_reg, hi_u32 x0_gs_graph)
{
    u_xdp_2dscale_graph_b xdp_2dscale_graph_b;

    xdp_2dscale_graph_b.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_b.u32)));
    xdp_2dscale_graph_b.bits.x0_gs_graph = x0_gs_graph;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_b.u32)), xdp_2dscale_graph_b.u32);

    return;
}

hi_void vdp_xdp_2dscale_setgraphpsb(vdp_regs_type *vdp_reg, hi_u32 graph_psb)
{
    u_xdp_2dscale_graph_psb xdp_2dscale_graph_psb;

    xdp_2dscale_graph_psb.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_psb.u32)));
    xdp_2dscale_graph_psb.bits.graph_psb = graph_psb;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_psb.u32)), xdp_2dscale_graph_psb.u32);

    return;
}

hi_void vdp_xdp_2dscale_setgscoremode(vdp_regs_type *vdp_reg, hi_u32 gscore_mode)
{
    u_xdp_2dscale_graph_gscore xdp_2dscale_graph_gscore;

    xdp_2dscale_graph_gscore.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_gscore.u32)));
    xdp_2dscale_graph_gscore.bits.gscore_mode = gscore_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_gscore.u32)), xdp_2dscale_graph_gscore.u32);

    return;
}

hi_void vdp_xdp_2dscale_setgscorecfg(vdp_regs_type *vdp_reg, hi_u32 gscore_cfg)
{
    u_xdp_2dscale_graph_gscore xdp_2dscale_graph_gscore;

    xdp_2dscale_graph_gscore.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_gscore.u32)));
    xdp_2dscale_graph_gscore.bits.gscore_cfg = gscore_cfg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_graph_gscore.u32)), xdp_2dscale_graph_gscore.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightneiblmtgain(vdp_regs_type *vdp_reg, hi_u32 lv_weight_neiblmt_gain)
{
    u_xdp_2dscale_dirdect_v0 xdp_2dscale_dirdect_v0;

    xdp_2dscale_dirdect_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)));
    xdp_2dscale_dirdect_v0.bits.lv_weight_neiblmt_gain = lv_weight_neiblmt_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)), xdp_2dscale_dirdect_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadmononeibgain(vdp_regs_type *vdp_reg, hi_u32 lv_sadmono_neibgain)
{
    u_xdp_2dscale_dirdect_v0 xdp_2dscale_dirdect_v0;

    xdp_2dscale_dirdect_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)));
    xdp_2dscale_dirdect_v0.bits.lv_sadmono_neibgain = lv_sadmono_neibgain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)), xdp_2dscale_dirdect_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightnongain(vdp_regs_type *vdp_reg, hi_u32 lv_weightnon_gain)
{
    u_xdp_2dscale_dirdect_v0 xdp_2dscale_dirdect_v0;

    xdp_2dscale_dirdect_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)));
    xdp_2dscale_dirdect_v0.bits.lv_weightnon_gain = lv_weightnon_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)), xdp_2dscale_dirdect_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadneibbldgain(vdp_regs_type *vdp_reg, hi_u32 lv_sad_neibbld_gain)
{
    u_xdp_2dscale_dirdect_v0 xdp_2dscale_dirdect_v0;

    xdp_2dscale_dirdect_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)));
    xdp_2dscale_dirdect_v0.bits.lv_sad_neibbld_gain = lv_sad_neibbld_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v0.u32)), xdp_2dscale_dirdect_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightnonb(vdp_regs_type *vdp_reg, hi_u32 lv_weight_non_b)
{
    u_xdp_2dscale_dirdect_v1 xdp_2dscale_dirdect_v1;

    xdp_2dscale_dirdect_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)));
    xdp_2dscale_dirdect_v1.bits.lv_weight_non_b = lv_weight_non_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)), xdp_2dscale_dirdect_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadmonoaddk(vdp_regs_type *vdp_reg, hi_u32 lv_sadmono_add_k)
{
    u_xdp_2dscale_dirdect_v1 xdp_2dscale_dirdect_v1;

    xdp_2dscale_dirdect_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)));
    xdp_2dscale_dirdect_v1.bits.lv_sadmono_add_k = lv_sadmono_add_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)), xdp_2dscale_dirdect_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsaddifthr(vdp_regs_type *vdp_reg, hi_u32 lv_sad_dif_thr)
{
    u_xdp_2dscale_dirdect_v1 xdp_2dscale_dirdect_v1;

    xdp_2dscale_dirdect_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)));
    xdp_2dscale_dirdect_v1.bits.lv_sad_dif_thr = lv_sad_dif_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)), xdp_2dscale_dirdect_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadavgthr(vdp_regs_type *vdp_reg, hi_u32 lv_sad_avg_thr)
{
    u_xdp_2dscale_dirdect_v1 xdp_2dscale_dirdect_v1;

    xdp_2dscale_dirdect_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)));
    xdp_2dscale_dirdect_v1.bits.lv_sad_avg_thr = lv_sad_avg_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v1.u32)), xdp_2dscale_dirdect_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvminsadcor(vdp_regs_type *vdp_reg, hi_u32 lv_minsad_cor)
{
    u_xdp_2dscale_dirdect_v2 xdp_2dscale_dirdect_v2;

    xdp_2dscale_dirdect_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)));
    xdp_2dscale_dirdect_v2.bits.lv_minsad_cor = lv_minsad_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)), xdp_2dscale_dirdect_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadamendlmtk(vdp_regs_type *vdp_reg, hi_u32 lv_sad_amendlmt_k)
{
    u_xdp_2dscale_dirdect_v2 xdp_2dscale_dirdect_v2;

    xdp_2dscale_dirdect_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)));
    xdp_2dscale_dirdect_v2.bits.lv_sad_amendlmt_k = lv_sad_amendlmt_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)), xdp_2dscale_dirdect_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadamendlmtb(vdp_regs_type *vdp_reg, hi_u32 lv_sad_amendlmt_b)
{
    u_xdp_2dscale_dirdect_v2 xdp_2dscale_dirdect_v2;

    xdp_2dscale_dirdect_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)));
    xdp_2dscale_dirdect_v2.bits.lv_sad_amendlmt_b = lv_sad_amendlmt_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)), xdp_2dscale_dirdect_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsaddifhvk(vdp_regs_type *vdp_reg, hi_u32 lv_sad_difhv_k)
{
    u_xdp_2dscale_dirdect_v2 xdp_2dscale_dirdect_v2;

    xdp_2dscale_dirdect_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)));
    xdp_2dscale_dirdect_v2.bits.lv_sad_difhv_k = lv_sad_difhv_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v2.u32)), xdp_2dscale_dirdect_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirstopnum(vdp_regs_type *vdp_reg, hi_u32 lv_dir_stop_num)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_dir_stop_num = lv_dir_stop_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvsadmonopunishen(vdp_regs_type *vdp_reg, hi_u32 lv_sad_monopunish_en)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_sad_monopunish_en = lv_sad_monopunish_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirweightmixmode(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weight_mixmode)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_dir_weight_mixmode = lv_dir_weight_mixmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirhvdiaglmten(vdp_regs_type *vdp_reg, hi_u32 lv_dir_hvdiaglmt_en)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_dir_hvdiaglmt_en = lv_dir_hvdiaglmt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirhvlmten(vdp_regs_type *vdp_reg, hi_u32 lv_dir_hvlmt_en)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_dir_hvlmt_en = lv_dir_hvlmt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirtblmten(vdp_regs_type *vdp_reg, hi_u32 lv_dir_tblmt_en)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_dir_tblmt_en = lv_dir_tblmt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirweightmaxk(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightmax_k)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_dir_weightmax_k = lv_dir_weightmax_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvminsadgain(vdp_regs_type *vdp_reg, hi_u32 lv_minsad_gain)
{
    u_xdp_2dscale_dirdect_v3 xdp_2dscale_dirdect_v3;

    xdp_2dscale_dirdect_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)));
    xdp_2dscale_dirdect_v3.bits.lv_minsad_gain = lv_minsad_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v3.u32)), xdp_2dscale_dirdect_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightbldgmin(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_gmin)
{
    u_xdp_2dscale_dirdect_v4 xdp_2dscale_dirdect_v4;

    xdp_2dscale_dirdect_v4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)));
    xdp_2dscale_dirdect_v4.bits.lv_weight_bld_gmin = lv_weight_bld_gmin;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)), xdp_2dscale_dirdect_v4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightbldgmax(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_gmax)
{
    u_xdp_2dscale_dirdect_v4 xdp_2dscale_dirdect_v4;

    xdp_2dscale_dirdect_v4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)));
    xdp_2dscale_dirdect_v4.bits.lv_weight_bld_gmax = lv_weight_bld_gmax;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)), xdp_2dscale_dirdect_v4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightbldk(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_k)
{
    u_xdp_2dscale_dirdect_v4 xdp_2dscale_dirdect_v4;

    xdp_2dscale_dirdect_v4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)));
    xdp_2dscale_dirdect_v4.bits.lv_weight_bld_k = lv_weight_bld_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)), xdp_2dscale_dirdect_v4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvweightbldcor(vdp_regs_type *vdp_reg, hi_u32 lv_weight_bld_cor)
{
    u_xdp_2dscale_dirdect_v4 xdp_2dscale_dirdect_v4;

    xdp_2dscale_dirdect_v4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)));
    xdp_2dscale_dirdect_v4.bits.lv_weight_bld_cor = lv_weight_bld_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_v4.u32)), xdp_2dscale_dirdect_v4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselinetype(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_type)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_type = lv_denseline_type;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselinecor(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_cor)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_cor = lv_denseline_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselinecork(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_cork)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_cork = lv_denseline_cork;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselinegain(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_gain)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_gain = lv_denseline_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselineflattype(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_flattype)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_flattype = lv_denseline_flattype;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselinewintype(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_wintype)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_wintype = lv_denseline_wintype;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdenselinedeten(vdp_regs_type *vdp_reg, hi_u32 lv_denseline_det_en)
{
    u_xdp_2dscale_denseline_v xdp_2dscale_denseline_v;

    xdp_2dscale_denseline_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)));
    xdp_2dscale_denseline_v.bits.lv_denseline_det_en = lv_denseline_det_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_v.u32)), xdp_2dscale_denseline_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirgain(vdp_regs_type *vdp_reg, hi_u32 lv_dir_gain)
{
    u_xdp_2dscale_weight_lpf_v xdp_2dscale_weight_lpf_v;

    xdp_2dscale_weight_lpf_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)));
    xdp_2dscale_weight_lpf_v.bits.lv_dir_gain = lv_dir_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)), xdp_2dscale_weight_lpf_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirweightlpfgain(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightlpf_gain)
{
    u_xdp_2dscale_weight_lpf_v xdp_2dscale_weight_lpf_v;

    xdp_2dscale_weight_lpf_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)));
    xdp_2dscale_weight_lpf_v.bits.lv_dir_weightlpf_gain = lv_dir_weightlpf_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)), xdp_2dscale_weight_lpf_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirweightlpfneibmode(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightlpf_neibmode)
{
    u_xdp_2dscale_weight_lpf_v xdp_2dscale_weight_lpf_v;

    xdp_2dscale_weight_lpf_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)));
    xdp_2dscale_weight_lpf_v.bits.lv_dir_weightlpf_neibmode = lv_dir_weightlpf_neibmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)), xdp_2dscale_weight_lpf_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdirweightlpfen(vdp_regs_type *vdp_reg, hi_u32 lv_dir_weightlpf_en)
{
    u_xdp_2dscale_weight_lpf_v xdp_2dscale_weight_lpf_v;

    xdp_2dscale_weight_lpf_v.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)));
    xdp_2dscale_weight_lpf_v.bits.lv_dir_weightlpf_en = lv_dir_weightlpf_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_v.u32)), xdp_2dscale_weight_lpf_v.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut103(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_03)
{
    u_xdp_2dscale_lut1_v0 xdp_2dscale_lut1_v0;

    xdp_2dscale_lut1_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)));
    xdp_2dscale_lut1_v0.bits.lv_dr_lut1_03 = lv_dr_lut1_03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)), xdp_2dscale_lut1_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut102(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_02)
{
    u_xdp_2dscale_lut1_v0 xdp_2dscale_lut1_v0;

    xdp_2dscale_lut1_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)));
    xdp_2dscale_lut1_v0.bits.lv_dr_lut1_02 = lv_dr_lut1_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)), xdp_2dscale_lut1_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut101(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_01)
{
    u_xdp_2dscale_lut1_v0 xdp_2dscale_lut1_v0;

    xdp_2dscale_lut1_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)));
    xdp_2dscale_lut1_v0.bits.lv_dr_lut1_01 = lv_dr_lut1_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)), xdp_2dscale_lut1_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut100(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_00)
{
    u_xdp_2dscale_lut1_v0 xdp_2dscale_lut1_v0;

    xdp_2dscale_lut1_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)));
    xdp_2dscale_lut1_v0.bits.lv_dr_lut1_00 = lv_dr_lut1_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v0.u32)), xdp_2dscale_lut1_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut107(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_07)
{
    u_xdp_2dscale_lut1_v1 xdp_2dscale_lut1_v1;

    xdp_2dscale_lut1_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)));
    xdp_2dscale_lut1_v1.bits.lv_dr_lut1_07 = lv_dr_lut1_07;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)), xdp_2dscale_lut1_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut106(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_06)
{
    u_xdp_2dscale_lut1_v1 xdp_2dscale_lut1_v1;

    xdp_2dscale_lut1_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)));
    xdp_2dscale_lut1_v1.bits.lv_dr_lut1_06 = lv_dr_lut1_06;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)), xdp_2dscale_lut1_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut105(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_05)
{
    u_xdp_2dscale_lut1_v1 xdp_2dscale_lut1_v1;

    xdp_2dscale_lut1_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)));
    xdp_2dscale_lut1_v1.bits.lv_dr_lut1_05 = lv_dr_lut1_05;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)), xdp_2dscale_lut1_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut104(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_04)
{
    u_xdp_2dscale_lut1_v1 xdp_2dscale_lut1_v1;

    xdp_2dscale_lut1_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)));
    xdp_2dscale_lut1_v1.bits.lv_dr_lut1_04 = lv_dr_lut1_04;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v1.u32)), xdp_2dscale_lut1_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut111(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_11)
{
    u_xdp_2dscale_lut1_v2 xdp_2dscale_lut1_v2;

    xdp_2dscale_lut1_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)));
    xdp_2dscale_lut1_v2.bits.lv_dr_lut1_11 = lv_dr_lut1_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)), xdp_2dscale_lut1_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut110(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_10)
{
    u_xdp_2dscale_lut1_v2 xdp_2dscale_lut1_v2;

    xdp_2dscale_lut1_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)));
    xdp_2dscale_lut1_v2.bits.lv_dr_lut1_10 = lv_dr_lut1_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)), xdp_2dscale_lut1_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut109(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_09)
{
    u_xdp_2dscale_lut1_v2 xdp_2dscale_lut1_v2;

    xdp_2dscale_lut1_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)));
    xdp_2dscale_lut1_v2.bits.lv_dr_lut1_09 = lv_dr_lut1_09;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)), xdp_2dscale_lut1_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut108(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_08)
{
    u_xdp_2dscale_lut1_v2 xdp_2dscale_lut1_v2;

    xdp_2dscale_lut1_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)));
    xdp_2dscale_lut1_v2.bits.lv_dr_lut1_08 = lv_dr_lut1_08;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v2.u32)), xdp_2dscale_lut1_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut115(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_15)
{
    u_xdp_2dscale_lut1_v3 xdp_2dscale_lut1_v3;

    xdp_2dscale_lut1_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)));
    xdp_2dscale_lut1_v3.bits.lv_dr_lut1_15 = lv_dr_lut1_15;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)), xdp_2dscale_lut1_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut114(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_14)
{
    u_xdp_2dscale_lut1_v3 xdp_2dscale_lut1_v3;

    xdp_2dscale_lut1_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)));
    xdp_2dscale_lut1_v3.bits.lv_dr_lut1_14 = lv_dr_lut1_14;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)), xdp_2dscale_lut1_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut113(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_13)
{
    u_xdp_2dscale_lut1_v3 xdp_2dscale_lut1_v3;

    xdp_2dscale_lut1_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)));
    xdp_2dscale_lut1_v3.bits.lv_dr_lut1_13 = lv_dr_lut1_13;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)), xdp_2dscale_lut1_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut112(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut1_12)
{
    u_xdp_2dscale_lut1_v3 xdp_2dscale_lut1_v3;

    xdp_2dscale_lut1_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)));
    xdp_2dscale_lut1_v3.bits.lv_dr_lut1_12 = lv_dr_lut1_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_v3.u32)), xdp_2dscale_lut1_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut203(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_03)
{
    u_xdp_2dscale_lut2_v0 xdp_2dscale_lut2_v0;

    xdp_2dscale_lut2_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)));
    xdp_2dscale_lut2_v0.bits.lv_dr_lut2_03 = lv_dr_lut2_03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)), xdp_2dscale_lut2_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut202(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_02)
{
    u_xdp_2dscale_lut2_v0 xdp_2dscale_lut2_v0;

    xdp_2dscale_lut2_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)));
    xdp_2dscale_lut2_v0.bits.lv_dr_lut2_02 = lv_dr_lut2_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)), xdp_2dscale_lut2_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut201(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_01)
{
    u_xdp_2dscale_lut2_v0 xdp_2dscale_lut2_v0;

    xdp_2dscale_lut2_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)));
    xdp_2dscale_lut2_v0.bits.lv_dr_lut2_01 = lv_dr_lut2_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)), xdp_2dscale_lut2_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut200(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_00)
{
    u_xdp_2dscale_lut2_v0 xdp_2dscale_lut2_v0;

    xdp_2dscale_lut2_v0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)));
    xdp_2dscale_lut2_v0.bits.lv_dr_lut2_00 = lv_dr_lut2_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v0.u32)), xdp_2dscale_lut2_v0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut207(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_07)
{
    u_xdp_2dscale_lut2_v1 xdp_2dscale_lut2_v1;

    xdp_2dscale_lut2_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)));
    xdp_2dscale_lut2_v1.bits.lv_dr_lut2_07 = lv_dr_lut2_07;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)), xdp_2dscale_lut2_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut206(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_06)
{
    u_xdp_2dscale_lut2_v1 xdp_2dscale_lut2_v1;

    xdp_2dscale_lut2_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)));
    xdp_2dscale_lut2_v1.bits.lv_dr_lut2_06 = lv_dr_lut2_06;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)), xdp_2dscale_lut2_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut205(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_05)
{
    u_xdp_2dscale_lut2_v1 xdp_2dscale_lut2_v1;

    xdp_2dscale_lut2_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)));
    xdp_2dscale_lut2_v1.bits.lv_dr_lut2_05 = lv_dr_lut2_05;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)), xdp_2dscale_lut2_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut204(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_04)
{
    u_xdp_2dscale_lut2_v1 xdp_2dscale_lut2_v1;

    xdp_2dscale_lut2_v1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)));
    xdp_2dscale_lut2_v1.bits.lv_dr_lut2_04 = lv_dr_lut2_04;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v1.u32)), xdp_2dscale_lut2_v1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut211(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_11)
{
    u_xdp_2dscale_lut2_v2 xdp_2dscale_lut2_v2;

    xdp_2dscale_lut2_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)));
    xdp_2dscale_lut2_v2.bits.lv_dr_lut2_11 = lv_dr_lut2_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)), xdp_2dscale_lut2_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut210(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_10)
{
    u_xdp_2dscale_lut2_v2 xdp_2dscale_lut2_v2;

    xdp_2dscale_lut2_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)));
    xdp_2dscale_lut2_v2.bits.lv_dr_lut2_10 = lv_dr_lut2_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)), xdp_2dscale_lut2_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut209(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_09)
{
    u_xdp_2dscale_lut2_v2 xdp_2dscale_lut2_v2;

    xdp_2dscale_lut2_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)));
    xdp_2dscale_lut2_v2.bits.lv_dr_lut2_09 = lv_dr_lut2_09;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)), xdp_2dscale_lut2_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut208(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_08)
{
    u_xdp_2dscale_lut2_v2 xdp_2dscale_lut2_v2;

    xdp_2dscale_lut2_v2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)));
    xdp_2dscale_lut2_v2.bits.lv_dr_lut2_08 = lv_dr_lut2_08;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v2.u32)), xdp_2dscale_lut2_v2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut215(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_15)
{
    u_xdp_2dscale_lut2_v3 xdp_2dscale_lut2_v3;

    xdp_2dscale_lut2_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)));
    xdp_2dscale_lut2_v3.bits.lv_dr_lut2_15 = lv_dr_lut2_15;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)), xdp_2dscale_lut2_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut214(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_14)
{
    u_xdp_2dscale_lut2_v3 xdp_2dscale_lut2_v3;

    xdp_2dscale_lut2_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)));
    xdp_2dscale_lut2_v3.bits.lv_dr_lut2_14 = lv_dr_lut2_14;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)), xdp_2dscale_lut2_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut213(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_13)
{
    u_xdp_2dscale_lut2_v3 xdp_2dscale_lut2_v3;

    xdp_2dscale_lut2_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)));
    xdp_2dscale_lut2_v3.bits.lv_dr_lut2_13 = lv_dr_lut2_13;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)), xdp_2dscale_lut2_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlvdrlut212(vdp_regs_type *vdp_reg, hi_u32 lv_dr_lut2_12)
{
    u_xdp_2dscale_lut2_v3 xdp_2dscale_lut2_v3;

    xdp_2dscale_lut2_v3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)));
    xdp_2dscale_lut2_v3.bits.lv_dr_lut2_12 = lv_dr_lut2_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_v3.u32)), xdp_2dscale_lut2_v3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightneiblmtgain(vdp_regs_type *vdp_reg, hi_u32 lh_weight_neiblmt_gain)
{
    u_xdp_2dscale_dirdect_h0 xdp_2dscale_dirdect_h0;

    xdp_2dscale_dirdect_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)));
    xdp_2dscale_dirdect_h0.bits.lh_weight_neiblmt_gain = lh_weight_neiblmt_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)), xdp_2dscale_dirdect_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadmononeibgain(vdp_regs_type *vdp_reg, hi_u32 lh_sadmono_neibgain)
{
    u_xdp_2dscale_dirdect_h0 xdp_2dscale_dirdect_h0;

    xdp_2dscale_dirdect_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)));
    xdp_2dscale_dirdect_h0.bits.lh_sadmono_neibgain = lh_sadmono_neibgain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)), xdp_2dscale_dirdect_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightnongain(vdp_regs_type *vdp_reg, hi_u32 lh_weightnon_gain)
{
    u_xdp_2dscale_dirdect_h0 xdp_2dscale_dirdect_h0;

    xdp_2dscale_dirdect_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)));
    xdp_2dscale_dirdect_h0.bits.lh_weightnon_gain = lh_weightnon_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)), xdp_2dscale_dirdect_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadneibbldgain(vdp_regs_type *vdp_reg, hi_u32 lh_sad_neibbld_gain)
{
    u_xdp_2dscale_dirdect_h0 xdp_2dscale_dirdect_h0;

    xdp_2dscale_dirdect_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)));
    xdp_2dscale_dirdect_h0.bits.lh_sad_neibbld_gain = lh_sad_neibbld_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h0.u32)), xdp_2dscale_dirdect_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightnonb(vdp_regs_type *vdp_reg, hi_u32 lh_weight_non_b)
{
    u_xdp_2dscale_dirdect_h1 xdp_2dscale_dirdect_h1;

    xdp_2dscale_dirdect_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)));
    xdp_2dscale_dirdect_h1.bits.lh_weight_non_b = lh_weight_non_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)), xdp_2dscale_dirdect_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadmonoaddk(vdp_regs_type *vdp_reg, hi_u32 lh_sadmono_add_k)
{
    u_xdp_2dscale_dirdect_h1 xdp_2dscale_dirdect_h1;

    xdp_2dscale_dirdect_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)));
    xdp_2dscale_dirdect_h1.bits.lh_sadmono_add_k = lh_sadmono_add_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)), xdp_2dscale_dirdect_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsaddifthr(vdp_regs_type *vdp_reg, hi_u32 lh_sad_dif_thr)
{
    u_xdp_2dscale_dirdect_h1 xdp_2dscale_dirdect_h1;

    xdp_2dscale_dirdect_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)));
    xdp_2dscale_dirdect_h1.bits.lh_sad_dif_thr = lh_sad_dif_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)), xdp_2dscale_dirdect_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadavgthr(vdp_regs_type *vdp_reg, hi_u32 lh_sad_avg_thr)
{
    u_xdp_2dscale_dirdect_h1 xdp_2dscale_dirdect_h1;

    xdp_2dscale_dirdect_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)));
    xdp_2dscale_dirdect_h1.bits.lh_sad_avg_thr = lh_sad_avg_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h1.u32)), xdp_2dscale_dirdect_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhminsadcor(vdp_regs_type *vdp_reg, hi_u32 lh_minsad_cor)
{
    u_xdp_2dscale_dirdect_h2 xdp_2dscale_dirdect_h2;

    xdp_2dscale_dirdect_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)));
    xdp_2dscale_dirdect_h2.bits.lh_minsad_cor = lh_minsad_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)), xdp_2dscale_dirdect_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadamendlmtk(vdp_regs_type *vdp_reg, hi_u32 lh_sad_amendlmt_k)
{
    u_xdp_2dscale_dirdect_h2 xdp_2dscale_dirdect_h2;

    xdp_2dscale_dirdect_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)));
    xdp_2dscale_dirdect_h2.bits.lh_sad_amendlmt_k = lh_sad_amendlmt_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)), xdp_2dscale_dirdect_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadamendlmtb(vdp_regs_type *vdp_reg, hi_u32 lh_sad_amendlmt_b)
{
    u_xdp_2dscale_dirdect_h2 xdp_2dscale_dirdect_h2;

    xdp_2dscale_dirdect_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)));
    xdp_2dscale_dirdect_h2.bits.lh_sad_amendlmt_b = lh_sad_amendlmt_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)), xdp_2dscale_dirdect_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsaddifhvk(vdp_regs_type *vdp_reg, hi_u32 lh_sad_difhv_k)
{
    u_xdp_2dscale_dirdect_h2 xdp_2dscale_dirdect_h2;

    xdp_2dscale_dirdect_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)));
    xdp_2dscale_dirdect_h2.bits.lh_sad_difhv_k = lh_sad_difhv_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h2.u32)), xdp_2dscale_dirdect_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirstopnum(vdp_regs_type *vdp_reg, hi_u32 lh_dir_stop_num)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_dir_stop_num = lh_dir_stop_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhsadmonopunishen(vdp_regs_type *vdp_reg, hi_u32 lh_sad_monopunish_en)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_sad_monopunish_en = lh_sad_monopunish_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirweightmixmode(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weight_mixmode)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_dir_weight_mixmode = lh_dir_weight_mixmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirhvdiaglmten(vdp_regs_type *vdp_reg, hi_u32 lh_dir_hvdiaglmt_en)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_dir_hvdiaglmt_en = lh_dir_hvdiaglmt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirhvlmten(vdp_regs_type *vdp_reg, hi_u32 lh_dir_hvlmt_en)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_dir_hvlmt_en = lh_dir_hvlmt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirtblmten(vdp_regs_type *vdp_reg, hi_u32 lh_dir_tblmt_en)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_dir_tblmt_en = lh_dir_tblmt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirweightmaxk(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightmax_k)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_dir_weightmax_k = lh_dir_weightmax_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhminsadgain(vdp_regs_type *vdp_reg, hi_u32 lh_minsad_gain)
{
    u_xdp_2dscale_dirdect_h3 xdp_2dscale_dirdect_h3;

    xdp_2dscale_dirdect_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)));
    xdp_2dscale_dirdect_h3.bits.lh_minsad_gain = lh_minsad_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h3.u32)), xdp_2dscale_dirdect_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightbldgmin(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_gmin)
{
    u_xdp_2dscale_dirdect_h4 xdp_2dscale_dirdect_h4;

    xdp_2dscale_dirdect_h4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)));
    xdp_2dscale_dirdect_h4.bits.lh_weight_bld_gmin = lh_weight_bld_gmin;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)), xdp_2dscale_dirdect_h4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightbldgmax(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_gmax)
{
    u_xdp_2dscale_dirdect_h4 xdp_2dscale_dirdect_h4;

    xdp_2dscale_dirdect_h4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)));
    xdp_2dscale_dirdect_h4.bits.lh_weight_bld_gmax = lh_weight_bld_gmax;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)), xdp_2dscale_dirdect_h4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightbldk(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_k)
{
    u_xdp_2dscale_dirdect_h4 xdp_2dscale_dirdect_h4;

    xdp_2dscale_dirdect_h4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)));
    xdp_2dscale_dirdect_h4.bits.lh_weight_bld_k = lh_weight_bld_k;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)), xdp_2dscale_dirdect_h4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhweightbldcor(vdp_regs_type *vdp_reg, hi_u32 lh_weight_bld_cor)
{
    u_xdp_2dscale_dirdect_h4 xdp_2dscale_dirdect_h4;

    xdp_2dscale_dirdect_h4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)));
    xdp_2dscale_dirdect_h4.bits.lh_weight_bld_cor = lh_weight_bld_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_dirdect_h4.u32)), xdp_2dscale_dirdect_h4.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselinetype(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_type)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_type = lh_denseline_type;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselinecor(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_cor)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_cor = lh_denseline_cor;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselinecork(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_cork)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_cork = lh_denseline_cork;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselinegain(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_gain)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_gain = lh_denseline_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselineflattype(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_flattype)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_flattype = lh_denseline_flattype;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselinewintype(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_wintype)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_wintype = lh_denseline_wintype;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdenselinedeten(vdp_regs_type *vdp_reg, hi_u32 lh_denseline_det_en)
{
    u_xdp_2dscale_denseline_h xdp_2dscale_denseline_h;

    xdp_2dscale_denseline_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)));
    xdp_2dscale_denseline_h.bits.lh_denseline_det_en = lh_denseline_det_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_denseline_h.u32)), xdp_2dscale_denseline_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirgain(vdp_regs_type *vdp_reg, hi_u32 lh_dir_gain)
{
    u_xdp_2dscale_weight_lpf_h xdp_2dscale_weight_lpf_h;

    xdp_2dscale_weight_lpf_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)));
    xdp_2dscale_weight_lpf_h.bits.lh_dir_gain = lh_dir_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)), xdp_2dscale_weight_lpf_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirweightlpfgain(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightlpf_gain)
{
    u_xdp_2dscale_weight_lpf_h xdp_2dscale_weight_lpf_h;

    xdp_2dscale_weight_lpf_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)));
    xdp_2dscale_weight_lpf_h.bits.lh_dir_weightlpf_gain = lh_dir_weightlpf_gain;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)), xdp_2dscale_weight_lpf_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirweightlpfneibmode(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightlpf_neibmode)
{
    u_xdp_2dscale_weight_lpf_h xdp_2dscale_weight_lpf_h;

    xdp_2dscale_weight_lpf_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)));
    xdp_2dscale_weight_lpf_h.bits.lh_dir_weightlpf_neibmode = lh_dir_weightlpf_neibmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)), xdp_2dscale_weight_lpf_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdirweightlpfen(vdp_regs_type *vdp_reg, hi_u32 lh_dir_weightlpf_en)
{
    u_xdp_2dscale_weight_lpf_h xdp_2dscale_weight_lpf_h;

    xdp_2dscale_weight_lpf_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)));
    xdp_2dscale_weight_lpf_h.bits.lh_dir_weightlpf_en = lh_dir_weightlpf_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_weight_lpf_h.u32)), xdp_2dscale_weight_lpf_h.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut103(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_03)
{
    u_xdp_2dscale_lut1_h0 xdp_2dscale_lut1_h0;

    xdp_2dscale_lut1_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)));
    xdp_2dscale_lut1_h0.bits.lh_dr_lut1_03 = lh_dr_lut1_03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)), xdp_2dscale_lut1_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut102(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_02)
{
    u_xdp_2dscale_lut1_h0 xdp_2dscale_lut1_h0;

    xdp_2dscale_lut1_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)));
    xdp_2dscale_lut1_h0.bits.lh_dr_lut1_02 = lh_dr_lut1_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)), xdp_2dscale_lut1_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut101(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_01)
{
    u_xdp_2dscale_lut1_h0 xdp_2dscale_lut1_h0;

    xdp_2dscale_lut1_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)));
    xdp_2dscale_lut1_h0.bits.lh_dr_lut1_01 = lh_dr_lut1_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)), xdp_2dscale_lut1_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut100(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_00)
{
    u_xdp_2dscale_lut1_h0 xdp_2dscale_lut1_h0;

    xdp_2dscale_lut1_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)));
    xdp_2dscale_lut1_h0.bits.lh_dr_lut1_00 = lh_dr_lut1_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h0.u32)), xdp_2dscale_lut1_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut107(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_07)
{
    u_xdp_2dscale_lut1_h1 xdp_2dscale_lut1_h1;

    xdp_2dscale_lut1_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)));
    xdp_2dscale_lut1_h1.bits.lh_dr_lut1_07 = lh_dr_lut1_07;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)), xdp_2dscale_lut1_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut106(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_06)
{
    u_xdp_2dscale_lut1_h1 xdp_2dscale_lut1_h1;

    xdp_2dscale_lut1_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)));
    xdp_2dscale_lut1_h1.bits.lh_dr_lut1_06 = lh_dr_lut1_06;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)), xdp_2dscale_lut1_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut105(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_05)
{
    u_xdp_2dscale_lut1_h1 xdp_2dscale_lut1_h1;

    xdp_2dscale_lut1_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)));
    xdp_2dscale_lut1_h1.bits.lh_dr_lut1_05 = lh_dr_lut1_05;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)), xdp_2dscale_lut1_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut104(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_04)
{
    u_xdp_2dscale_lut1_h1 xdp_2dscale_lut1_h1;

    xdp_2dscale_lut1_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)));
    xdp_2dscale_lut1_h1.bits.lh_dr_lut1_04 = lh_dr_lut1_04;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h1.u32)), xdp_2dscale_lut1_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut111(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_11)
{
    u_xdp_2dscale_lut1_h2 xdp_2dscale_lut1_h2;

    xdp_2dscale_lut1_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)));
    xdp_2dscale_lut1_h2.bits.lh_dr_lut1_11 = lh_dr_lut1_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)), xdp_2dscale_lut1_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut110(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_10)
{
    u_xdp_2dscale_lut1_h2 xdp_2dscale_lut1_h2;

    xdp_2dscale_lut1_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)));
    xdp_2dscale_lut1_h2.bits.lh_dr_lut1_10 = lh_dr_lut1_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)), xdp_2dscale_lut1_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut109(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_09)
{
    u_xdp_2dscale_lut1_h2 xdp_2dscale_lut1_h2;

    xdp_2dscale_lut1_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)));
    xdp_2dscale_lut1_h2.bits.lh_dr_lut1_09 = lh_dr_lut1_09;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)), xdp_2dscale_lut1_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut108(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_08)
{
    u_xdp_2dscale_lut1_h2 xdp_2dscale_lut1_h2;

    xdp_2dscale_lut1_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)));
    xdp_2dscale_lut1_h2.bits.lh_dr_lut1_08 = lh_dr_lut1_08;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h2.u32)), xdp_2dscale_lut1_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut115(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_15)
{
    u_xdp_2dscale_lut1_h3 xdp_2dscale_lut1_h3;

    xdp_2dscale_lut1_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)));
    xdp_2dscale_lut1_h3.bits.lh_dr_lut1_15 = lh_dr_lut1_15;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)), xdp_2dscale_lut1_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut114(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_14)
{
    u_xdp_2dscale_lut1_h3 xdp_2dscale_lut1_h3;

    xdp_2dscale_lut1_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)));
    xdp_2dscale_lut1_h3.bits.lh_dr_lut1_14 = lh_dr_lut1_14;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)), xdp_2dscale_lut1_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut113(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_13)
{
    u_xdp_2dscale_lut1_h3 xdp_2dscale_lut1_h3;

    xdp_2dscale_lut1_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)));
    xdp_2dscale_lut1_h3.bits.lh_dr_lut1_13 = lh_dr_lut1_13;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)), xdp_2dscale_lut1_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut112(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut1_12)
{
    u_xdp_2dscale_lut1_h3 xdp_2dscale_lut1_h3;

    xdp_2dscale_lut1_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)));
    xdp_2dscale_lut1_h3.bits.lh_dr_lut1_12 = lh_dr_lut1_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut1_h3.u32)), xdp_2dscale_lut1_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut203(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_03)
{
    u_xdp_2dscale_lut2_h0 xdp_2dscale_lut2_h0;

    xdp_2dscale_lut2_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)));
    xdp_2dscale_lut2_h0.bits.lh_dr_lut2_03 = lh_dr_lut2_03;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)), xdp_2dscale_lut2_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut202(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_02)
{
    u_xdp_2dscale_lut2_h0 xdp_2dscale_lut2_h0;

    xdp_2dscale_lut2_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)));
    xdp_2dscale_lut2_h0.bits.lh_dr_lut2_02 = lh_dr_lut2_02;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)), xdp_2dscale_lut2_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut201(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_01)
{
    u_xdp_2dscale_lut2_h0 xdp_2dscale_lut2_h0;

    xdp_2dscale_lut2_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)));
    xdp_2dscale_lut2_h0.bits.lh_dr_lut2_01 = lh_dr_lut2_01;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)), xdp_2dscale_lut2_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut200(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_00)
{
    u_xdp_2dscale_lut2_h0 xdp_2dscale_lut2_h0;

    xdp_2dscale_lut2_h0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)));
    xdp_2dscale_lut2_h0.bits.lh_dr_lut2_00 = lh_dr_lut2_00;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h0.u32)), xdp_2dscale_lut2_h0.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut207(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_07)
{
    u_xdp_2dscale_lut2_h1 xdp_2dscale_lut2_h1;

    xdp_2dscale_lut2_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)));
    xdp_2dscale_lut2_h1.bits.lh_dr_lut2_07 = lh_dr_lut2_07;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)), xdp_2dscale_lut2_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut206(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_06)
{
    u_xdp_2dscale_lut2_h1 xdp_2dscale_lut2_h1;

    xdp_2dscale_lut2_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)));
    xdp_2dscale_lut2_h1.bits.lh_dr_lut2_06 = lh_dr_lut2_06;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)), xdp_2dscale_lut2_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut205(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_05)
{
    u_xdp_2dscale_lut2_h1 xdp_2dscale_lut2_h1;

    xdp_2dscale_lut2_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)));
    xdp_2dscale_lut2_h1.bits.lh_dr_lut2_05 = lh_dr_lut2_05;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)), xdp_2dscale_lut2_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut204(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_04)
{
    u_xdp_2dscale_lut2_h1 xdp_2dscale_lut2_h1;

    xdp_2dscale_lut2_h1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)));
    xdp_2dscale_lut2_h1.bits.lh_dr_lut2_04 = lh_dr_lut2_04;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h1.u32)), xdp_2dscale_lut2_h1.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut211(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_11)
{
    u_xdp_2dscale_lut2_h2 xdp_2dscale_lut2_h2;

    xdp_2dscale_lut2_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)));
    xdp_2dscale_lut2_h2.bits.lh_dr_lut2_11 = lh_dr_lut2_11;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)), xdp_2dscale_lut2_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut210(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_10)
{
    u_xdp_2dscale_lut2_h2 xdp_2dscale_lut2_h2;

    xdp_2dscale_lut2_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)));
    xdp_2dscale_lut2_h2.bits.lh_dr_lut2_10 = lh_dr_lut2_10;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)), xdp_2dscale_lut2_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut209(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_09)
{
    u_xdp_2dscale_lut2_h2 xdp_2dscale_lut2_h2;

    xdp_2dscale_lut2_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)));
    xdp_2dscale_lut2_h2.bits.lh_dr_lut2_09 = lh_dr_lut2_09;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)), xdp_2dscale_lut2_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut208(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_08)
{
    u_xdp_2dscale_lut2_h2 xdp_2dscale_lut2_h2;

    xdp_2dscale_lut2_h2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)));
    xdp_2dscale_lut2_h2.bits.lh_dr_lut2_08 = lh_dr_lut2_08;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h2.u32)), xdp_2dscale_lut2_h2.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut215(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_15)
{
    u_xdp_2dscale_lut2_h3 xdp_2dscale_lut2_h3;

    xdp_2dscale_lut2_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)));
    xdp_2dscale_lut2_h3.bits.lh_dr_lut2_15 = lh_dr_lut2_15;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)), xdp_2dscale_lut2_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut214(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_14)
{
    u_xdp_2dscale_lut2_h3 xdp_2dscale_lut2_h3;

    xdp_2dscale_lut2_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)));
    xdp_2dscale_lut2_h3.bits.lh_dr_lut2_14 = lh_dr_lut2_14;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)), xdp_2dscale_lut2_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut213(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_13)
{
    u_xdp_2dscale_lut2_h3 xdp_2dscale_lut2_h3;

    xdp_2dscale_lut2_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)));
    xdp_2dscale_lut2_h3.bits.lh_dr_lut2_13 = lh_dr_lut2_13;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)), xdp_2dscale_lut2_h3.u32);

    return;
}

hi_void vdp_xdp_2dscale_setlhdrlut212(vdp_regs_type *vdp_reg, hi_u32 lh_dr_lut2_12)
{
    u_xdp_2dscale_lut2_h3 xdp_2dscale_lut2_h3;

    xdp_2dscale_lut2_h3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)));
    xdp_2dscale_lut2_h3.bits.lh_dr_lut2_12 = lh_dr_lut2_12;
    vdp_regwrite((uintptr_t)(&(vdp_reg->xdp_2dscale_lut2_h3.u32)), xdp_2dscale_lut2_h3.u32);

    return;
}

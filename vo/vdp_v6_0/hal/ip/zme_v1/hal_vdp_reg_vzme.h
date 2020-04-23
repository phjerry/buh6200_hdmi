/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_VZME_H__
#define __HAL_VDP_REG_VZME_H__

#include "hi_reg_vdp.h"

hi_void vdp_vzme_setckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ck_gt_en);
hi_void vdp_vzme_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_fmt);
hi_void vdp_vzme_setoutheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_height);
hi_void vdp_vzme_setlvfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lvfir_en);
hi_void vdp_vzme_setcvfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cvfir_en);
hi_void vdp_vzme_setlvmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lvmid_en);
hi_void vdp_vzme_setcvmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cvmid_en);
hi_void vdp_vzme_setlvfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lvfir_mode);
hi_void vdp_vzme_setcvfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cvfir_mode);
hi_void vdp_vzme_settapmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tap_mode);
hi_void vdp_vzme_setvratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vratio);
hi_void vdp_vzme_setvlumaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vluma_offset);
hi_void vdp_vzme_setvchromaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vchroma_offset);
hi_void vdp_vzme_setvblumaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vbluma_offset);
hi_void vdp_vzme_setvbchromaoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vbchroma_offset);
hi_void vdp_vzme_setvlshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_shootctrl_en);
hi_void vdp_vzme_setvlshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_shootctrl_mode);
hi_void vdp_vzme_setvlflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_flatdect_mode);
hi_void vdp_vzme_setvlcoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_coringadj_en);
hi_void vdp_vzme_setvlgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_gain);
hi_void vdp_vzme_setvlcoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vl_coring);
hi_void vdp_vzme_setvcshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_shootctrl_en);
hi_void vdp_vzme_setvcshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_shootctrl_mode);
hi_void vdp_vzme_setvcflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_flatdect_mode);
hi_void vdp_vzme_setvccoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_coringadj_en);
hi_void vdp_vzme_setvcgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_gain);
hi_void vdp_vzme_setvccoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vc_coring);
hi_void vdp_vzme_setapbvhdvflren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vf_lren);
hi_void vdp_vzme_setapbvhdvfcren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vf_cren);
hi_void vdp_vzme_setapbvhdvycoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vycoef_raddr);
hi_void vdp_vzme_setapbvhdvccoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_vccoef_raddr);

#endif




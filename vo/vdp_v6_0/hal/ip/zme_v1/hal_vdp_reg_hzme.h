/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_HZME_H__
#define __HAL_VDP_REG_HZME_H__

#include "hi_reg_vdp.h"

hi_void vdp_hzme_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_fmt);
hi_void vdp_hzme_setckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ck_gt_en);
hi_void vdp_hzme_setoutwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_width);
hi_void vdp_hzme_setlhfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhfir_en);
hi_void vdp_hzme_setchfiren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chfir_en);
hi_void vdp_hzme_setlhmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhmid_en);
hi_void vdp_hzme_setchmiden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chmid_en);
hi_void vdp_hzme_setnonlnren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 non_lnr_en);
hi_void vdp_hzme_setlhfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhfir_mode);
hi_void vdp_hzme_setchfirmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chfir_mode);
hi_void vdp_hzme_sethfirorder(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hfir_order);
hi_void vdp_hzme_sethratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hratio);
hi_void vdp_hzme_setlhfiroffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lhfir_offset);
hi_void vdp_hzme_setchfiroffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chfir_offset);
hi_void vdp_hzme_setzone0delta(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone0_delta);
hi_void vdp_hzme_setzone2delta(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone2_delta);
hi_void vdp_hzme_setzone1end(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone1_end);
hi_void vdp_hzme_setzone0end(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 zone0_end);
hi_void vdp_hzme_sethlshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_shootctrl_en);
hi_void vdp_hzme_sethlshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_shootctrl_mode);
hi_void vdp_hzme_sethlflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_flatdect_mode);
hi_void vdp_hzme_sethlcoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_coringadj_en);
hi_void vdp_hzme_sethlgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_gain);
hi_void vdp_hzme_sethlcoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hl_coring);
hi_void vdp_hzme_sethcshootctrlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_shootctrl_en);
hi_void vdp_hzme_sethcshootctrlmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_shootctrl_mode);
hi_void vdp_hzme_sethcflatdectmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_flatdect_mode);
hi_void vdp_hzme_sethccoringadjen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_coringadj_en);
hi_void vdp_hzme_sethcgain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_gain);
hi_void vdp_hzme_sethccoring(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hc_coring);
hi_void vdp_hzme_setapbvhdhflren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hf_lren);
hi_void vdp_hzme_setapbvhdhfcren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hf_cren);
hi_void vdp_hzme_setapbvhdhycoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hycoef_raddr);
hi_void vdp_hzme_setapbvhdhccoefraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 apb_vhd_hccoef_raddr);
hi_void vdp_hzme_setinwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 in_width);
#endif




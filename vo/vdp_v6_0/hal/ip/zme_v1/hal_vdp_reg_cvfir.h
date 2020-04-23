/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_CVFIR_H__
#define __HAL_VDP_REG_CVFIR_H__

#include "hi_reg_vdp.h"

hi_void vdp_cvfir_setvzmeckgten(vdp_regs_type *vdp_reg, hi_u32 vzme_ck_gt_en);
hi_void vdp_cvfir_setoutpro(vdp_regs_type *vdp_reg, hi_u32 out_pro);
hi_void vdp_cvfir_setoutfmt(vdp_regs_type *vdp_reg, hi_u32 out_fmt);
hi_void vdp_cvfir_setoutheight(vdp_regs_type *vdp_reg, hi_u32 out_height);
hi_void vdp_cvfir_setcvfiren(vdp_regs_type *vdp_reg, hi_u32 cvfir_en);
hi_void vdp_cvfir_setcvmiden(vdp_regs_type *vdp_reg, hi_u32 cvmid_en);
hi_void vdp_cvfir_setcvfirmode(vdp_regs_type *vdp_reg, hi_u32 cvfir_mode);
hi_void vdp_cvfir_setvratio(vdp_regs_type *vdp_reg, hi_u32 vratio);
hi_void vdp_cvfir_setvchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vchroma_offset);
hi_void vdp_cvfir_setvbchromaoffset(vdp_regs_type *vdp_reg, hi_u32 vbchroma_offset);
hi_void vdp_cvfir_setvccoef00(vdp_regs_type *vdp_reg, hi_u32 vccoef00);
hi_void vdp_cvfir_setvccoef01(vdp_regs_type *vdp_reg, hi_u32 vccoef01);
hi_void vdp_cvfir_setvccoef02(vdp_regs_type *vdp_reg, hi_u32 vccoef02);
hi_void vdp_cvfir_setvccoef03(vdp_regs_type *vdp_reg, hi_u32 vccoef03);
hi_void vdp_cvfir_setvccoef10(vdp_regs_type *vdp_reg, hi_u32 vccoef10);
hi_void vdp_cvfir_setvccoef11(vdp_regs_type *vdp_reg, hi_u32 vccoef11);
hi_void vdp_cvfir_setvccoef12(vdp_regs_type *vdp_reg, hi_u32 vccoef12);
hi_void vdp_cvfir_setvccoef13(vdp_regs_type *vdp_reg, hi_u32 vccoef13);

#endif



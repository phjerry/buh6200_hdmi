/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_HIPP_CSC_H__
#define __HAL_VDP_REG_HIPP_CSC_H__

#include "hi_reg_vdp.h"

hi_void vdp_hipp_csc_sethippcscckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_ck_gt_en);
hi_void vdp_hipp_csc_sethippcscdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_demo_en);
hi_void vdp_hipp_csc_sethippcscen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_en);
hi_void vdp_hipp_csc_sethippcsccoef00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef00);
hi_void vdp_hipp_csc_sethippcsccoef01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef01);
hi_void vdp_hipp_csc_sethippcsccoef02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef02);
hi_void vdp_hipp_csc_sethippcsccoef10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef10);
hi_void vdp_hipp_csc_sethippcsccoef11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef11);
hi_void vdp_hipp_csc_sethippcsccoef12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef12);
hi_void vdp_hipp_csc_sethippcsccoef20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef20);
hi_void vdp_hipp_csc_sethippcsccoef21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef21);
hi_void vdp_hipp_csc_sethippcsccoef22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_coef22);
hi_void vdp_hipp_csc_sethippcscscale(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_scale);
hi_void vdp_hipp_csc_sethippcscidc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_idc0);
hi_void vdp_hipp_csc_sethippcscidc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_idc1);
hi_void vdp_hipp_csc_sethippcscidc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_idc2);
hi_void vdp_hipp_csc_sethippcscodc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_odc0);
hi_void vdp_hipp_csc_sethippcscodc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_odc1);
hi_void vdp_hipp_csc_sethippcscodc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_odc2);
hi_void vdp_hipp_csc_sethippcscminy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_min_y);
hi_void vdp_hipp_csc_sethippcscminc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_min_c);
hi_void vdp_hipp_csc_sethippcscmaxy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_max_y);
hi_void vdp_hipp_csc_sethippcscmaxc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc_max_c);
hi_void vdp_hipp_csc_sethippcsc2coef00(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef00);
hi_void vdp_hipp_csc_sethippcsc2coef01(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef01);
hi_void vdp_hipp_csc_sethippcsc2coef02(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef02);
hi_void vdp_hipp_csc_sethippcsc2coef10(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef10);
hi_void vdp_hipp_csc_sethippcsc2coef11(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef11);
hi_void vdp_hipp_csc_sethippcsc2coef12(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef12);
hi_void vdp_hipp_csc_sethippcsc2coef20(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef20);
hi_void vdp_hipp_csc_sethippcsc2coef21(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef21);
hi_void vdp_hipp_csc_sethippcsc2coef22(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_coef22);
hi_void vdp_hipp_csc_sethippcsc2scale(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_scale);
hi_void vdp_hipp_csc_sethippcsc2idc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_idc0);
hi_void vdp_hipp_csc_sethippcsc2idc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_idc1);
hi_void vdp_hipp_csc_sethippcsc2idc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_idc2);
hi_void vdp_hipp_csc_sethippcsc2odc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_odc0);
hi_void vdp_hipp_csc_sethippcsc2odc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_odc1);
hi_void vdp_hipp_csc_sethippcsc2odc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_odc2);
hi_void vdp_hipp_csc_sethippcsc2miny(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_min_y);
hi_void vdp_hipp_csc_sethippcsc2minc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_min_c);
hi_void vdp_hipp_csc_sethippcsc2maxy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_max_y);
hi_void vdp_hipp_csc_sethippcsc2maxc(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 hipp_csc2_max_c);
hi_void vdp_hipp_csc_setcolormode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 color_mode);
hi_void vdp_hipp_csc_setcrossenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cross_enable);
hi_void vdp_hipp_csc_setdatafmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data_fmt);
hi_void vdp_hipp_csc_setinksel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ink_sel);
hi_void vdp_hipp_csc_setinken(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ink_en);
hi_void vdp_hipp_csc_setypos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 y_pos);
hi_void vdp_hipp_csc_setxpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 x_pos);
hi_u32 vdp_hipp_csc_getinkdata(vdp_regs_type *vdp_reg, hi_u32 offset);
hi_u32 vdp_hipp_csc_getinkdata2(vdp_regs_type *vdp_reg, hi_u32 offset);

#endif


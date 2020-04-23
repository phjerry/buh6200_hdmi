/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_TCHDR_H__
#define __HAL_VDP_REG_TCHDR_H__

#include "hi_reg_vdp.h"

hi_void vdp_tchdr_settchdrdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_demo_pos);
hi_void vdp_tchdr_settchdrdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_demo_mode);
hi_void vdp_tchdr_settchdrdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_demo_en);
hi_void vdp_tchdr_settchdrckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_ck_gt_en);
hi_void vdp_tchdr_settchdren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_en);
hi_void vdp_tchdr_settchdrr2yclipen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_clip_en);
hi_void vdp_tchdr_settchdryuvrange(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_yuv_range);
hi_void vdp_tchdr_settchdralphab(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_alpha_b);
hi_void vdp_tchdr_settchdralphaa(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_alpha_a);
hi_void vdp_tchdr_settchdroct2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct2);
hi_void vdp_tchdr_settchdroct1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct1);
hi_void vdp_tchdr_settchdroct0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct0);
hi_void vdp_tchdr_settchdroct5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct5);
hi_void vdp_tchdr_settchdroct4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct4);
hi_void vdp_tchdr_settchdroct3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct3);
hi_void vdp_tchdr_settchdroct6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct6);
hi_void vdp_tchdr_settchdrdlutstep3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step3);
hi_void vdp_tchdr_settchdrdlutstep2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step2);
hi_void vdp_tchdr_settchdrdlutstep1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step1);
hi_void vdp_tchdr_settchdrdlutstep0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step0);
hi_void vdp_tchdr_settchdrdlutthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_thr1);
hi_void vdp_tchdr_settchdrdlutthr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_thr0);
hi_void vdp_tchdr_settchdrdlutthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_thr2);
hi_void vdp_tchdr_settchdrparadrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_d_rd_en);
hi_void vdp_tchdr_settchdrparairden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_i_rd_en);
hi_void vdp_tchdr_settchdrparasrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_s_rd_en);
hi_void vdp_tchdr_settchdrparaprden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_p_rd_en);
hi_void vdp_tchdr_settchdrparacrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_c_rd_en);
hi_void vdp_tchdr_settchdrparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_data);
hi_void vdp_tchdr_settchdrr2ycoef1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef1);
hi_void vdp_tchdr_settchdrr2ycoef0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef0);
hi_void vdp_tchdr_settchdrr2ycoef3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef3);
hi_void vdp_tchdr_settchdrr2ycoef2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef2);
hi_void vdp_tchdr_settchdrr2ycoef5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef5);
hi_void vdp_tchdr_settchdrr2ycoef4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef4);
hi_void vdp_tchdr_settchdrr2ycoef7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef7);
hi_void vdp_tchdr_settchdrr2ycoef6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef6);
hi_void vdp_tchdr_settchdrr2ycoef8(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef8);
hi_void vdp_tchdr_settchdrr2yscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_scale2p);
hi_void vdp_tchdr_settchdrr2youtdc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_out_dc0);
hi_void vdp_tchdr_settchdrr2youtdc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_out_dc1);
hi_void vdp_tchdr_settchdrr2youtdc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_out_dc2);
hi_void vdp_tchdr_settchdrr2yclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_clip_min);
hi_void vdp_tchdr_settchdrr2yclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_clip_max);

#endif



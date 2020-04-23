/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_tchdr.h"
#include "hal_vdp_comm.h"

hi_void vdp_tchdr_settchdrdemopos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_demo_pos)
{
    u_tchdr_ctrl tchdr_ctrl;

    tchdr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset));
    tchdr_ctrl.bits.tchdr_demo_pos = tchdr_demo_pos;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset), tchdr_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdemomode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_demo_mode)
{
    u_tchdr_ctrl tchdr_ctrl;

    tchdr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset));
    tchdr_ctrl.bits.tchdr_demo_mode = tchdr_demo_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset), tchdr_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdemoen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_demo_en)
{
    u_tchdr_ctrl tchdr_ctrl;

    tchdr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset));
    tchdr_ctrl.bits.tchdr_demo_en = tchdr_demo_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset), tchdr_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdrckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_ck_gt_en)
{
    u_tchdr_ctrl tchdr_ctrl;

    tchdr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset));
    tchdr_ctrl.bits.tchdr_ck_gt_en = tchdr_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset), tchdr_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_en)
{
    u_tchdr_ctrl tchdr_ctrl;

    tchdr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset));
    tchdr_ctrl.bits.tchdr_en = tchdr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_ctrl.u32) + offset), tchdr_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2yclipen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_clip_en)
{
    u_tchdr_alg_ctrl tchdr_alg_ctrl;

    tchdr_alg_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_alg_ctrl.u32) + offset));
    tchdr_alg_ctrl.bits.tchdr_r2y_clip_en = tchdr_r2y_clip_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_alg_ctrl.u32) + offset), tchdr_alg_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdryuvrange(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_yuv_range)
{
    u_tchdr_alg_ctrl tchdr_alg_ctrl;

    tchdr_alg_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_alg_ctrl.u32) + offset));
    tchdr_alg_ctrl.bits.tchdr_yuv_range = tchdr_yuv_range;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_alg_ctrl.u32) + offset), tchdr_alg_ctrl.u32);

    return ;
}

hi_void vdp_tchdr_settchdralphab(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_alpha_b)
{
    u_tchdr_alpha tchdr_alpha;

    tchdr_alpha.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_alpha.u32) + offset));
    tchdr_alpha.bits.tchdr_alpha_b = tchdr_alpha_b;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_alpha.u32) + offset), tchdr_alpha.u32);

    return ;
}

hi_void vdp_tchdr_settchdralphaa(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_alpha_a)
{
    u_tchdr_alpha tchdr_alpha;

    tchdr_alpha.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_alpha.u32) + offset));
    tchdr_alpha.bits.tchdr_alpha_a = tchdr_alpha_a;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_alpha.u32) + offset), tchdr_alpha.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_oct2)
{
    u_tchdr_oct0 tchdr_oct0;

    tchdr_oct0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct0.u32) + offset));
    tchdr_oct0.bits.tchdr_oct2 = tchdr_oct2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct0.u32) + offset), tchdr_oct0.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_oct0 tchdr_oct0;

    tchdr_oct0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct0.u32) + offset));
    tchdr_oct0.bits.tchdr_oct1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct0.u32) + offset), tchdr_oct0.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_oct0 tchdr_oct0;

    tchdr_oct0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct0.u32) + offset));
    tchdr_oct0.bits.tchdr_oct0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct0.u32) + offset), tchdr_oct0.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_oct1 tchdr_oct1;

    tchdr_oct1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct1.u32) + offset));
    tchdr_oct1.bits.tchdr_oct5 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct1.u32) + offset), tchdr_oct1.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_oct1 tchdr_oct1;

    tchdr_oct1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct1.u32) + offset));
    tchdr_oct1.bits.tchdr_oct4 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct1.u32) + offset), tchdr_oct1.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_oct1 tchdr_oct1;

    tchdr_oct1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct1.u32) + offset));
    tchdr_oct1.bits.tchdr_oct3 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct1.u32) + offset), tchdr_oct1.u32);

    return ;
}

hi_void vdp_tchdr_settchdroct6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_oct2 tchdr_oct2;

    tchdr_oct2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_oct2.u32) + offset));
    tchdr_oct2.bits.tchdr_oct6 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_oct2.u32) + offset), tchdr_oct2.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutstep3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step3)
{
    u_tchdr_d_lut_step tchdr_d_lut_step;

    tchdr_d_lut_step.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset));
    tchdr_d_lut_step.bits.tchdr_d_lut_step3 = tchdr_d_lut_step3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset), tchdr_d_lut_step.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutstep2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step2)
{
    u_tchdr_d_lut_step tchdr_d_lut_step;

    tchdr_d_lut_step.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset));
    tchdr_d_lut_step.bits.tchdr_d_lut_step2 = tchdr_d_lut_step2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset), tchdr_d_lut_step.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutstep1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step1)
{
    u_tchdr_d_lut_step tchdr_d_lut_step;

    tchdr_d_lut_step.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset));
    tchdr_d_lut_step.bits.tchdr_d_lut_step1 = tchdr_d_lut_step1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset), tchdr_d_lut_step.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutstep0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_step0)
{
    u_tchdr_d_lut_step tchdr_d_lut_step;

    tchdr_d_lut_step.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset));
    tchdr_d_lut_step.bits.tchdr_d_lut_step0 = tchdr_d_lut_step0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_step.u32) + offset), tchdr_d_lut_step.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutthr1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_thr1)
{
    u_tchdr_d_lut_thr0 tchdr_d_lut_thr0;

    tchdr_d_lut_thr0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_thr0.u32) + offset));
    tchdr_d_lut_thr0.bits.tchdr_d_lut_thr1 = tchdr_d_lut_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_thr0.u32) + offset), tchdr_d_lut_thr0.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutthr0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_d_lut_thr0 tchdr_d_lut_thr0;

    tchdr_d_lut_thr0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_thr0.u32) + offset));
    tchdr_d_lut_thr0.bits.tchdr_d_lut_thr0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_thr0.u32) + offset), tchdr_d_lut_thr0.u32);

    return ;
}

hi_void vdp_tchdr_settchdrdlutthr2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_d_lut_thr2)
{
    u_tchdr_d_lut_thr1 tchdr_d_lut_thr1;

    tchdr_d_lut_thr1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_d_lut_thr1.u32) + offset));
    tchdr_d_lut_thr1.bits.tchdr_d_lut_thr2 = tchdr_d_lut_thr2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_d_lut_thr1.u32) + offset), tchdr_d_lut_thr1.u32);

    return ;
}

hi_void vdp_tchdr_settchdrparadrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_d_rd_en)
{
    u_tchdr_para_ren tchdr_para_ren;

    tchdr_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset));
    tchdr_para_ren.bits.tchdr_para_d_rd_en = tchdr_para_d_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset), tchdr_para_ren.u32);

    return ;
}

hi_void vdp_tchdr_settchdrparairden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_i_rd_en)
{
    u_tchdr_para_ren tchdr_para_ren;

    tchdr_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset));
    tchdr_para_ren.bits.tchdr_para_i_rd_en = tchdr_para_i_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset), tchdr_para_ren.u32);

    return ;
}

hi_void vdp_tchdr_settchdrparasrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_s_rd_en)
{
    u_tchdr_para_ren tchdr_para_ren;

    tchdr_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset));
    tchdr_para_ren.bits.tchdr_para_s_rd_en = tchdr_para_s_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset), tchdr_para_ren.u32);

    return ;
}

hi_void vdp_tchdr_settchdrparaprden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_p_rd_en)
{
    u_tchdr_para_ren tchdr_para_ren;

    tchdr_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset));
    tchdr_para_ren.bits.tchdr_para_p_rd_en = tchdr_para_p_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset), tchdr_para_ren.u32);

    return ;
}

hi_void vdp_tchdr_settchdrparacrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_para_c_rd_en)
{
    u_tchdr_para_ren tchdr_para_ren;

    tchdr_para_ren.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset));
    tchdr_para_ren.bits.tchdr_para_c_rd_en = tchdr_para_c_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_para_ren.u32) + offset), tchdr_para_ren.u32);

    return ;
}

hi_void vdp_tchdr_settchdrparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_para_data tchdr_para_data;

    tchdr_para_data.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_para_data.u32) + offset));
    tchdr_para_data.bits.tchdr_para_data = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_para_data.u32) + offset), tchdr_para_data.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef1)
{
    u_tchdr_r2y_coef0 tchdr_r2y_coef0;

    tchdr_r2y_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef0.u32) + offset));
    tchdr_r2y_coef0.bits.tchdr_r2y_coef1 = tchdr_r2y_coef1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef0.u32) + offset), tchdr_r2y_coef0.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_r2y_coef0 tchdr_r2y_coef0;

    tchdr_r2y_coef0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef0.u32) + offset));
    tchdr_r2y_coef0.bits.tchdr_r2y_coef0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef0.u32) + offset), tchdr_r2y_coef0.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef3)
{
    u_tchdr_r2y_coef1 tchdr_r2y_coef1;

    tchdr_r2y_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef1.u32) + offset));
    tchdr_r2y_coef1.bits.tchdr_r2y_coef3 = tchdr_r2y_coef3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef1.u32) + offset), tchdr_r2y_coef1.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef2)
{
    u_tchdr_r2y_coef1 tchdr_r2y_coef1;

    tchdr_r2y_coef1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef1.u32) + offset));
    tchdr_r2y_coef1.bits.tchdr_r2y_coef2 = tchdr_r2y_coef2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef1.u32) + offset), tchdr_r2y_coef1.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef5(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef5)
{
    u_tchdr_r2y_coef2 tchdr_r2y_coef2;

    tchdr_r2y_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef2.u32) + offset));
    tchdr_r2y_coef2.bits.tchdr_r2y_coef5 = tchdr_r2y_coef5;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef2.u32) + offset), tchdr_r2y_coef2.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef4(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef4)
{
    u_tchdr_r2y_coef2 tchdr_r2y_coef2;

    tchdr_r2y_coef2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef2.u32) + offset));
    tchdr_r2y_coef2.bits.tchdr_r2y_coef4 = tchdr_r2y_coef4;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef2.u32) + offset), tchdr_r2y_coef2.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef7(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef7)
{
    u_tchdr_r2y_coef3 tchdr_r2y_coef3;

    tchdr_r2y_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef3.u32) + offset));
    tchdr_r2y_coef3.bits.tchdr_r2y_coef7 = tchdr_r2y_coef7;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef3.u32) + offset), tchdr_r2y_coef3.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef6(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef6)
{
    u_tchdr_r2y_coef3 tchdr_r2y_coef3;

    tchdr_r2y_coef3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef3.u32) + offset));
    tchdr_r2y_coef3.bits.tchdr_r2y_coef6 = tchdr_r2y_coef6;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef3.u32) + offset), tchdr_r2y_coef3.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2ycoef8(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_coef8)
{
    u_tchdr_r2y_coef4 tchdr_r2y_coef4;

    tchdr_r2y_coef4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_coef4.u32) + offset));
    tchdr_r2y_coef4.bits.tchdr_r2y_coef8 = tchdr_r2y_coef8;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_coef4.u32) + offset), tchdr_r2y_coef4.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2yscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_r2y_scale2p tchdr_r2y_scale2p;

    tchdr_r2y_scale2p.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_scale2p.u32) + offset));
    tchdr_r2y_scale2p.bits.tchdr_r2y_scale2p = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_scale2p.u32) + offset), tchdr_r2y_scale2p.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2youtdc0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_r2y_out_dc0 tchdr_r2y_out_dc0;

    tchdr_r2y_out_dc0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_out_dc0.u32) + offset));
    tchdr_r2y_out_dc0.bits.tchdr_r2y_out_dc0 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_out_dc0.u32) + offset), tchdr_r2y_out_dc0.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2youtdc1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_r2y_out_dc1 tchdr_r2y_out_dc1;

    tchdr_r2y_out_dc1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_out_dc1.u32) + offset));
    tchdr_r2y_out_dc1.bits.tchdr_r2y_out_dc1 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_out_dc1.u32) + offset), tchdr_r2y_out_dc1.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2youtdc2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data)
{
    u_tchdr_r2y_out_dc2 tchdr_r2y_out_dc2;

    tchdr_r2y_out_dc2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_out_dc2.u32) + offset));
    tchdr_r2y_out_dc2.bits.tchdr_r2y_out_dc2 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_out_dc2.u32) + offset), tchdr_r2y_out_dc2.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2yclipmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_clip_min)
{
    u_tchdr_r2y_min tchdr_r2y_min;

    tchdr_r2y_min.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_min.u32) + offset));
    tchdr_r2y_min.bits.tchdr_r2y_clip_min = tchdr_r2y_clip_min;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_min.u32) + offset), tchdr_r2y_min.u32);

    return ;
}

hi_void vdp_tchdr_settchdrr2yclipmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tchdr_r2y_clip_max)
{
    u_tchdr_r2y_max tchdr_r2y_max;

    tchdr_r2y_max.u32 = vdp_regread((uintptr_t)(&(vdp_reg->tchdr_r2y_max.u32) + offset));
    tchdr_r2y_max.bits.tchdr_r2y_clip_max = tchdr_r2y_clip_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->tchdr_r2y_max.u32) + offset), tchdr_r2y_max.u32);

    return ;
}



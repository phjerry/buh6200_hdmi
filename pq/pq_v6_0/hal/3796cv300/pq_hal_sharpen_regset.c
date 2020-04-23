/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sharpen reg layer function file
 * Author: pq
 * Create: 2019-11-11
 */


#include "pq_hal_comm.h"
#include "pq_hal_sharpen_regset.h"
#include "pq_hal_sharpen.h"


hi_void pq_reg_sharp_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = (1 << (u32chnnum - SHARPEN_PARAUP_COEF_OFFSET));
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

hi_void pq_reg_sharp_para_addr_v0_chn14(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn14 para_addr_v0_chn14;

    para_addr_v0_chn14.u32 = pq_new_regread((uintptr_t)&(vdp_reg->para_addr_v0_chn14.u32));
    para_addr_v0_chn14.bits.para_addr_v0_chn14 = data;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn14.u32)), para_addr_v0_chn14.u32);

    return;
}

hi_void pq_reg_sharp_set_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sharp_en)
{
    u_vdp_spctrl vdp_spctrl;

    vdp_spctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset));
    vdp_spctrl.bits.sharp_en = sharp_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset), vdp_spctrl.u32);

    return;
}

hi_void pq_reg_sharp_set_demo_en(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_en)
{
    u_vdp_spctrl vdp_spctrl;

    vdp_spctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset));
    vdp_spctrl.bits.demo_en = demo_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset), vdp_spctrl.u32);

    return;
}

hi_void pq_reg_sharp_set_demo_mode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_mode)
{
    u_vdp_spctrl vdp_spctrl;

    vdp_spctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset));
    vdp_spctrl.bits.demo_mode = demo_mode;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset), vdp_spctrl.u32);

    return;
}

hi_void pq_reg_sharp_set_demo_pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 demo_pos)
{
    u_vdp_spctrl vdp_spctrl;

    vdp_spctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset));
    vdp_spctrl.bits.demo_pos = demo_pos;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->vdp_spctrl.u32) + offset), vdp_spctrl.u32);

    return;
}

hi_void pq_reg_sharp_set_peak_gain(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 peak_gain)
{
    u_vdp_splticontrastthr vdp_splticontrastthr;
    pq_hal_sharp_ddr_regread((uintptr_t)(&(vdp_reg->vdp_splticontrastthr.u32) + offset), &vdp_splticontrastthr.u32);
    vdp_splticontrastthr.bits.peakgain = peak_gain;
    pq_hal_sharp_ddr_regwrite((uintptr_t)(&(vdp_reg->vdp_splticontrastthr.u32) + offset), vdp_splticontrastthr.u32);

    return;
}

hi_void pq_reg_sharp_set_peak_ratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 peak_ratio)
{
    u_vdp_splticontrastthr vdp_splticontrastthr;

    pq_hal_sharp_ddr_regread((uintptr_t)(&(vdp_reg->vdp_splticontrastthr.u32) + offset), &vdp_splticontrastthr.u32);
    vdp_splticontrastthr.bits.peakratio = peak_ratio;
    pq_hal_sharp_ddr_regwrite((uintptr_t)(&(vdp_reg->vdp_splticontrastthr.u32) + offset), vdp_splticontrastthr.u32);

    return;
}

hi_void pq_reg_sharp_set_lti_ratio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lti_ratio)
{
    u_vdp_spltigrad vdp_spltigrad;

    pq_hal_sharp_ddr_regread((uintptr_t)(&(vdp_reg->vdp_spltigrad.u32) + offset), &vdp_spltigrad.u32);
    vdp_spltigrad.bits.ltiratio = lti_ratio;
    pq_hal_sharp_ddr_regwrite((uintptr_t)(&(vdp_reg->vdp_spltigrad.u32) + offset), vdp_spltigrad.u32);

    return;
}

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_mdata.h"

hi_void vdp_mdata_set_chn_en(vdp_regs_type *reg, hi_u32 chn_en)
{
    u_msreq_ctrl msreq_ctrl;

    msreq_ctrl.u32 = vdp_regread((uintptr_t)(&(reg->msreq_ctrl.u32)));
    msreq_ctrl.bits.chn_en = chn_en;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_ctrl.u32)), msreq_ctrl.u32);

    return ;
}

hi_void vdp_mdata_set_nosec_flag(vdp_regs_type *reg, hi_u32 nosec_flag)
{
    u_msreq_ctrl msreq_ctrl;

    msreq_ctrl.u32 = vdp_regread((uintptr_t)(&(reg->msreq_ctrl.u32)));
    msreq_ctrl.bits.nosec_flag = nosec_flag;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_ctrl.u32)), msreq_ctrl.u32);

    return ;
}


hi_void vdp_mdata_set_mmu_bypass(vdp_regs_type *reg, hi_u32 mmu_bypass)
{
    u_msreq_ctrl msreq_ctrl;

    msreq_ctrl.u32 = vdp_regread((uintptr_t)(&(reg->msreq_ctrl.u32)));
    msreq_ctrl.bits.mmu_bypass = mmu_bypass;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_ctrl.u32)), msreq_ctrl.u32);

    return ;
}


hi_void vdp_mdata_set_avi_valid(vdp_regs_type *reg, hi_u32 avi_valid)
{
    u_msreq_ctrl msreq_ctrl;

    msreq_ctrl.u32 = vdp_regread((uintptr_t)(&(reg->msreq_ctrl.u32)));
    msreq_ctrl.bits.avi_valid = avi_valid;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_ctrl.u32)), msreq_ctrl.u32);

    return ;
}


hi_void vdp_mdata_set_smd_valid(vdp_regs_type *reg, hi_u32 smd_valid)
{
    u_msreq_ctrl msreq_ctrl;

    msreq_ctrl.u32 = vdp_regread((uintptr_t)(&(reg->msreq_ctrl.u32)));
    msreq_ctrl.bits.smd_valid = smd_valid;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_ctrl.u32)), msreq_ctrl.u32);

    return ;
}


hi_void vdp_mdata_set_sreq_delay(vdp_regs_type *reg, hi_u32 sreq_delay)
{
    u_msreq_ctrl msreq_ctrl;

    msreq_ctrl.u32 = vdp_regread((uintptr_t)(&(reg->msreq_ctrl.u32)));
    msreq_ctrl.bits.sreq_delay = sreq_delay;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_ctrl.u32)), msreq_ctrl.u32);

    return ;
}


hi_void vdp_mdata_set_regup(vdp_regs_type *reg, hi_u32 regup)
{
    u_msreq_rupd msreq_rupd;

    msreq_rupd.u32 = vdp_regread((uintptr_t)(&(reg->msreq_rupd.u32)));
    msreq_rupd.bits.regup = regup;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_rupd.u32)), msreq_rupd.u32);

    return ;
}


hi_void vdp_mdata_set_maddr_l(vdp_regs_type *reg, hi_u32 maddr)
{
    u_msreq_laddr msreq_laddr;

    msreq_laddr.u32 = vdp_regread((uintptr_t)(&(reg->msreq_laddr.u32)));
    msreq_laddr.bits.maddr = maddr;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_laddr.u32)), msreq_laddr.u32);

    return ;
}

hi_void vdp_mdata_set_maddr_h(vdp_regs_type *reg, hi_u32 maddr)
{
    u_msreq_haddr msreq_haddr;

    msreq_haddr.u32 = vdp_regread((uintptr_t)(&(reg->msreq_haddr.u32)));
    msreq_haddr.bits.maddr1 = maddr;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_haddr.u32)), msreq_haddr.u32);

    return ;
}

hi_void vdp_mdata_set_mburst(vdp_regs_type *reg, hi_u32 mburst)
{
    u_msreq_burst msreq_burst;

    msreq_burst.u32 = vdp_regread((uintptr_t)(&(reg->msreq_burst.u32)));
    msreq_burst.bits.mburst = mburst;
    vdp_regwrite ((uintptr_t)(&(reg->msreq_burst.u32)), msreq_burst.u32);

    return ;
}









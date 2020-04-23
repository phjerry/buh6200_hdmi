/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_master.h"
#include "hal_vdp_comm.h"

hi_void vdp_master_setmstr2woutstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr2_woutstanding)
{
    u_vdp_core_mst_outstanding vdp_core_mst_outstanding;

    vdp_core_mst_outstanding.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset));
    vdp_core_mst_outstanding.bits.mstr2_woutstanding = mstr2_woutstanding;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset), vdp_core_mst_outstanding.u32);

    return;
}

hi_void vdp_master_setmstr2routstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr2_routstanding)
{
    u_vdp_core_mst_outstanding vdp_core_mst_outstanding;

    vdp_core_mst_outstanding.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset));
    vdp_core_mst_outstanding.bits.mstr2_routstanding = mstr2_routstanding;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset), vdp_core_mst_outstanding.u32);

    return;
}

hi_void vdp_master_setmstr1woutstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr1_woutstanding)
{
    u_vdp_core_mst_outstanding vdp_core_mst_outstanding;

    vdp_core_mst_outstanding.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset));
    vdp_core_mst_outstanding.bits.mstr1_woutstanding = mstr1_woutstanding;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset), vdp_core_mst_outstanding.u32);

    return;
}

hi_void vdp_master_setmstr1routstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr1_routstanding)
{
    u_vdp_core_mst_outstanding vdp_core_mst_outstanding;

    vdp_core_mst_outstanding.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset));
    vdp_core_mst_outstanding.bits.mstr1_routstanding = mstr1_routstanding;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset), vdp_core_mst_outstanding.u32);

    return;
}

hi_void vdp_master_setmstr0woutstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr0_woutstanding)
{
    u_vdp_core_mst_outstanding vdp_core_mst_outstanding;

    vdp_core_mst_outstanding.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset));
    vdp_core_mst_outstanding.bits.mstr0_woutstanding = mstr0_woutstanding;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset), vdp_core_mst_outstanding.u32);

    return;
}

hi_void vdp_master_setqos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 qos0, hi_u32 qos1)
{
    u_vdp_core_mst_qos_map0 vdp_core_mst_qos_map0;
    u_vdp_core_mst_qos_map1 vdp_core_mst_qos_map1;

    vdp_core_mst_qos_map0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_qos_map0.u32) + offset));
    vdp_core_mst_qos_map0.u32 = qos0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_qos_map0.u32) + offset), vdp_core_mst_qos_map0.u32);

    vdp_core_mst_qos_map1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_qos_map1.u32) + offset));
    vdp_core_mst_qos_map1.u32 = qos1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_qos_map1.u32) + offset), vdp_core_mst_qos_map1.u32);

    return;
}

hi_void vdp_master_setmstr0routstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr0_routstanding)
{
    u_vdp_core_mst_outstanding vdp_core_mst_outstanding;

    vdp_core_mst_outstanding.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset));
    vdp_core_mst_outstanding.bits.mstr0_routstanding = mstr0_routstanding;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_outstanding.u32) + offset), vdp_core_mst_outstanding.u32);

    return;
}

hi_void vdp_master_setwportsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wport_sel)
{
    u_vdp_core_mst_ctrl vdp_core_mst_ctrl;

    vdp_core_mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset));
    vdp_core_mst_ctrl.bits.wport_sel = wport_sel;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset), vdp_core_mst_ctrl.u32);

    return;
}

hi_void vdp_master_setcasmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cas_mmu_bypass)
{
    u_vdp_core_mst_ctrl vdp_core_mst_ctrl;

    vdp_core_mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset));
    vdp_core_mst_ctrl.bits.cas_mmu_bypass = cas_mmu_bypass;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset), vdp_core_mst_ctrl.u32);

    return;
}

hi_void vdp_master_setsplittype(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 split_type)
{
    u_vdp_core_mst_ctrl vdp_core_mst_ctrl;

    vdp_core_mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset));
    vdp_core_mst_ctrl.bits.split_type = split_type;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset), vdp_core_mst_ctrl.u32);

    return;
}

hi_void vdp_master_setmidenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mid_enable)
{
    u_vdp_core_mst_ctrl vdp_core_mst_ctrl;

    vdp_core_mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset));
    vdp_core_mst_ctrl.bits.mid_enable = mid_enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset), vdp_core_mst_ctrl.u32);

    return;
}

hi_void vdp_master_setarbmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 arb_mode)
{
    u_vdp_core_mst_ctrl vdp_core_mst_ctrl;

    vdp_core_mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset));
    vdp_core_mst_ctrl.bits.arb_mode = arb_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset), vdp_core_mst_ctrl.u32);

    return;
}

hi_void vdp_master_setsplitmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 split_mode)
{
    u_vdp_core_mst_ctrl vdp_core_mst_ctrl;

    vdp_core_mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset));
    vdp_core_mst_ctrl.bits.split_mode = split_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_ctrl.u32) + offset), vdp_core_mst_ctrl.u32);

    return;
}

hi_void vdp_master_setrchnprio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 prio)
{
    hi_u32 u32prio = prio & 0x1;
    hi_u32 u32rchnprio = 0;

    if (id < 32) { // 32 is reg value
        u32rchnprio = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_prio0.u32) + offset));
        u32rchnprio &= (~(0x1 << id));
        u32rchnprio |= (u32prio << id);
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_prio0.u32) + offset), u32rchnprio);
    } else {
        u32rchnprio = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_prio1.u32) + offset));
        u32rchnprio &= (~(0x1 << (id - 32))); // 32 is reg offset
        u32rchnprio |= (u32prio << (id - 32)); // 32 is reg offset
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_prio1.u32) + offset), u32rchnprio);
    }

    return;
}

hi_void vdp_master_setwchnprio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 prio)
{
    hi_u32 u32prio = prio & 0x1;
    hi_u32 u32wchnprio = 0;

    if (id < 32) { // 32 is reg value
        u32wchnprio = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_prio0.u32) + offset));
        u32wchnprio &= (~(0x1 << id));
        u32wchnprio |= (u32prio << id);
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_prio0.u32) + offset), u32wchnprio);
    } else {
        u32wchnprio = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_prio1.u32) + offset));
        u32wchnprio &= (~(0x1 << (id - 32))); // 32 is reg offset
        u32wchnprio |= (u32prio << (id - 32)); // 32 is reg offset
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_prio1.u32) + offset), u32wchnprio);
    }

    return;
}

hi_void vdp_master_setrchnsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 sel)
{
    hi_u32 u32sel = sel & 0x3;
    hi_u32 u32rchnsel = 0;

    if (id < 16) { // 16 is reg value
        u32rchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel0.u32) + offset));
        u32rchnsel &= (~(0x3 << (id * 2))); // 0x3 is reg value 2 is reg width
        u32rchnsel |= (u32sel << (id * 2)); // 0x3 is reg value 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel0.u32) + offset), u32rchnsel);
    } else if (id < 32) { // 32 is reg value
        u32rchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel1.u32) + offset));
        u32rchnsel &= (~(0x3 << ((id - 16) * 2))); // 16 is reg offset 2 is reg width
        u32rchnsel |= (u32sel << ((id - 16) * 2));  // 16 is reg offset 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel1.u32) + offset), u32rchnsel);
    } else if (id < 48) { // 48 is reg value
        u32rchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel2.u32) + offset));
        u32rchnsel &= (~(0x3 << ((id - 32) * 2))); // 32 is reg offset 2 is reg width
        u32rchnsel |= (u32sel << ((id - 32) * 2)); // 32 is reg offset 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel2.u32) + offset), u32rchnsel);
    } else {
        u32rchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel3.u32) + offset));
        u32rchnsel &= (~(0x3 << ((id - 48) * 2))); // 48 is reg offset 2 is reg width
        u32rchnsel |= (u32sel << ((id - 48) * 2)); // 48 is reg offset 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_rchn_sel3.u32) + offset), u32rchnsel);
    }

    return;
}

hi_void vdp_master_setwchnsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 sel)
{
    hi_u32 u32sel = sel & 0x3;  // 3 is reg value
    hi_u32 u32wchnsel = 0;

    if (id < 16) { // 16 is reg value
        u32wchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel0.u32) + offset));
        u32wchnsel &= (~(0x3 << (id * 2))); // 3 is reg value 2 is reg width
        u32wchnsel |= (u32sel << (id * 2)); // 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel0.u32) + offset), u32wchnsel);
    } else if (id < 32) { // 32 is reg value
        u32wchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel1.u32) + offset));
        u32wchnsel &= (~(0x3 << ((id - 16) * 2))); // 16 is reg offset 2 is reg width
        u32wchnsel |= (u32sel << ((id - 16) * 2)); // 16 is reg offset 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel1.u32) + offset), u32wchnsel);
    } else if (id < 48) { // 48 is reg value
        u32wchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel2.u32) + offset));
        u32wchnsel &= (~(0x3 << ((id - 32) * 2))); // 32 is reg offset 2 is reg width
        u32wchnsel |= (u32sel << ((id - 32) * 2)); // 32 is reg offset 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel2.u32) + offset), u32wchnsel);
    } else {
        u32wchnsel = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel3.u32) + offset));
        u32wchnsel &= (~(0x3 << ((id - 48) * 2))); // 48 is reg offset 2 is reg width
        u32wchnsel |= (u32sel << ((id - 48) * 2)); // 48 is reg offset 2 is reg width
        vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_wchn_sel3.u32) + offset), u32wchnsel);
    }

    return;
}

hi_void vdp_master_setbuserrorclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bus_error_clr)
{
    u_vdp_core_mst_bus_err_clr vdp_core_mst_bus_err_clr;

    vdp_core_mst_bus_err_clr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err_clr.u32) + offset));
    vdp_core_mst_bus_err_clr.bits.bus_error_clr = bus_error_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err_clr.u32) + offset), vdp_core_mst_bus_err_clr.u32);

    return;
}

hi_void vdp_master_setmst2werror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_w_error)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset));
    vdp_core_mst_bus_err.bits.mst2_w_error = mst2_w_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset), vdp_core_mst_bus_err.u32);

    return;
}

hi_void vdp_master_setmst2rerror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_r_error)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset));
    vdp_core_mst_bus_err.bits.mst2_r_error = mst2_r_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset), vdp_core_mst_bus_err.u32);

    return;
}

hi_void vdp_master_setmst1werror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_w_error)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset));
    vdp_core_mst_bus_err.bits.mst1_w_error = mst1_w_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset), vdp_core_mst_bus_err.u32);

    return;
}

hi_void vdp_master_setmst1rerror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_r_error)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset));
    vdp_core_mst_bus_err.bits.mst1_r_error = mst1_r_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset), vdp_core_mst_bus_err.u32);

    return;
}

hi_void vdp_master_setmst0werror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_w_error)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset));
    vdp_core_mst_bus_err.bits.mst0_w_error = mst0_w_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset), vdp_core_mst_bus_err.u32);

    return;
}

hi_void vdp_master_setmst0rerror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_r_error)
{
    u_vdp_core_mst_bus_err vdp_core_mst_bus_err;

    vdp_core_mst_bus_err.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset));
    vdp_core_mst_bus_err.bits.mst0_r_error = mst0_r_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_bus_err.u32) + offset), vdp_core_mst_bus_err.u32);

    return;
}

hi_void vdp_master_setsrc0status0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src0_status0)
{
    u_vdp_core_mst_src0_status0 vdp_core_mst_src0_status0;

    vdp_core_mst_src0_status0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_src0_status0.u32) + offset));
    vdp_core_mst_src0_status0.bits.src0_status0 = src0_status0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_src0_status0.u32) + offset), vdp_core_mst_src0_status0.u32);

    return;
}

hi_void vdp_master_setsrc0status1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src0_status1)
{
    u_vdp_core_mst_src0_status1 vdp_core_mst_src0_status1;

    vdp_core_mst_src0_status1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_src0_status1.u32) + offset));
    vdp_core_mst_src0_status1.bits.src0_status1 = src0_status1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_src0_status1.u32) + offset), vdp_core_mst_src0_status1.u32);

    return;
}

hi_void vdp_master_setsrc1status0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src1_status0)
{
    u_vdp_core_mst_src1_status0 vdp_core_mst_src1_status0;

    vdp_core_mst_src1_status0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_src1_status0.u32) + offset));
    vdp_core_mst_src1_status0.bits.src1_status0 = src1_status0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_src1_status0.u32) + offset), vdp_core_mst_src1_status0.u32);

    return;
}

hi_void vdp_master_setsrc1status1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src1_status1)
{
    u_vdp_core_mst_src1_status1 vdp_core_mst_src1_status1;

    vdp_core_mst_src1_status1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_src1_status1.u32) + offset));
    vdp_core_mst_src1_status1.bits.src1_status1 = src1_status1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_src1_status1.u32) + offset), vdp_core_mst_src1_status1.u32);

    return;
}

hi_void vdp_master_setsrc2status0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src2_status0)
{
    u_vdp_core_mst_src2_status0 vdp_core_mst_src2_status0;

    vdp_core_mst_src2_status0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_src2_status0.u32) + offset));
    vdp_core_mst_src2_status0.bits.src2_status0 = src2_status0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_src2_status0.u32) + offset), vdp_core_mst_src2_status0.u32);

    return;
}

hi_void vdp_master_setsrc2status1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src2_status1)
{
    u_vdp_core_mst_src2_status1 vdp_core_mst_src2_status1;

    vdp_core_mst_src2_status1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_src2_status1.u32) + offset));
    vdp_core_mst_src2_status1.bits.src2_status1 = src2_status1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_src2_status1.u32) + offset), vdp_core_mst_src2_status1.u32);

    return;
}

hi_void vdp_master_setaxidetenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 axi_det_enable)
{
    u_vdp_core_mst_debug_ctrl vdp_core_mst_debug_ctrl;

    vdp_core_mst_debug_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_debug_ctrl.u32) + offset));
    vdp_core_mst_debug_ctrl.bits.axi_det_enable = axi_det_enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_debug_ctrl.u32) + offset), vdp_core_mst_debug_ctrl.u32);

    return;
}

hi_void vdp_master_setaxidetclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 axi_det_clr)
{
    u_vdp_core_mst_debug_clr vdp_core_mst_debug_clr;

    vdp_core_mst_debug_clr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst_debug_clr.u32) + offset));
    vdp_core_mst_debug_clr.bits.axi_det_clr = axi_det_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst_debug_clr.u32) + offset), vdp_core_mst_debug_clr.u32);

    return;
}

hi_void vdp_master_setmacdebuginfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mac_debug_info)
{
    u_vdp_core_mst0_debug_info vdp_core_mst0_debug_info;

    vdp_core_mst0_debug_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_debug_info.u32) + offset));
    vdp_core_mst0_debug_info.bits.mac_debug_info = mac_debug_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_debug_info.u32) + offset), vdp_core_mst0_debug_info.u32);

    return;
}

hi_void vdp_master_setmst0rdinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_info)
{
    u_vdp_core_mst0_rd_info vdp_core_mst0_rd_info;

    vdp_core_mst0_rd_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_rd_info.u32) + offset));
    vdp_core_mst0_rd_info.bits.mst0_rd_info = mst0_rd_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_rd_info.u32) + offset), vdp_core_mst0_rd_info.u32);

    return;
}

hi_void vdp_master_setmst0wrinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_info)
{
    u_vdp_core_mst0_wr_info vdp_core_mst0_wr_info;

    vdp_core_mst0_wr_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_wr_info.u32) + offset));
    vdp_core_mst0_wr_info.bits.mst0_wr_info = mst0_wr_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_wr_info.u32) + offset), vdp_core_mst0_wr_info.u32);

    return;
}

hi_void vdp_master_setmst1rdinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_info)
{
    u_vdp_core_mst1_rd_info vdp_core_mst1_rd_info;

    vdp_core_mst1_rd_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_rd_info.u32) + offset));
    vdp_core_mst1_rd_info.bits.mst1_rd_info = mst1_rd_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_rd_info.u32) + offset), vdp_core_mst1_rd_info.u32);

    return;
}

hi_void vdp_master_setmst1wrinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_info)
{
    u_vdp_core_mst1_wr_info vdp_core_mst1_wr_info;

    vdp_core_mst1_wr_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_wr_info.u32) + offset));
    vdp_core_mst1_wr_info.bits.mst1_wr_info = mst1_wr_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_wr_info.u32) + offset), vdp_core_mst1_wr_info.u32);

    return;
}

hi_void vdp_master_setmst2rdinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_rd_info)
{
    u_vdp_core_mst2_rd_info vdp_core_mst2_rd_info;

    vdp_core_mst2_rd_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst2_rd_info.u32) + offset));
    vdp_core_mst2_rd_info.bits.mst2_rd_info = mst2_rd_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst2_rd_info.u32) + offset), vdp_core_mst2_rd_info.u32);

    return;
}

hi_void vdp_master_setmst2wrinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_wr_info)
{
    u_vdp_core_mst2_wr_info vdp_core_mst2_wr_info;

    vdp_core_mst2_wr_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst2_wr_info.u32) + offset));
    vdp_core_mst2_wr_info.bits.mst2_wr_info = mst2_wr_info;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst2_wr_info.u32) + offset), vdp_core_mst2_wr_info.u32);

    return;
}

hi_void vdp_master_setmst0rdmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_max)
{
    u_vdp_core_mst0_det_latency0 vdp_core_mst0_det_latency0;

    vdp_core_mst0_det_latency0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency0.u32) +
                                                  offset));
    vdp_core_mst0_det_latency0.bits.mst0_rd_max = mst0_rd_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency0.u32) + offset),
                 vdp_core_mst0_det_latency0.u32);

    return;
}

hi_void vdp_master_setmst0rdaver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_aver)
{
    u_vdp_core_mst0_det_latency1 vdp_core_mst0_det_latency1;

    vdp_core_mst0_det_latency1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency1.u32) +
                                                  offset));
    vdp_core_mst0_det_latency1.bits.mst0_rd_aver = mst0_rd_aver;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency1.u32) + offset),
                 vdp_core_mst0_det_latency1.u32);

    return;
}

hi_void vdp_master_setmst0rdcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_cmd)
{
    u_vdp_core_mst0_det_latency2 vdp_core_mst0_det_latency2;

    vdp_core_mst0_det_latency2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency2.u32) +
                                                  offset));
    vdp_core_mst0_det_latency2.bits.mst0_rd_cmd = mst0_rd_cmd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency2.u32) + offset),
                 vdp_core_mst0_det_latency2.u32);

    return;
}

hi_void vdp_master_setmst0wrmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_max)
{
    u_vdp_core_mst0_det_latency3 vdp_core_mst0_det_latency3;

    vdp_core_mst0_det_latency3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency3.u32) +
                                                  offset));
    vdp_core_mst0_det_latency3.bits.mst0_wr_max = mst0_wr_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency3.u32) + offset),
                 vdp_core_mst0_det_latency3.u32);

    return;
}

hi_void vdp_master_setmst0wraver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_aver)
{
    u_vdp_core_mst0_det_latency4 vdp_core_mst0_det_latency4;

    vdp_core_mst0_det_latency4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency4.u32) +
                                                  offset));
    vdp_core_mst0_det_latency4.bits.mst0_wr_aver = mst0_wr_aver;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency4.u32) + offset),
                 vdp_core_mst0_det_latency4.u32);

    return;
}

hi_void vdp_master_setmst0wrcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_cmd)
{
    u_vdp_core_mst0_det_latency5 vdp_core_mst0_det_latency5;

    vdp_core_mst0_det_latency5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency5.u32) +
                                                  offset));
    vdp_core_mst0_det_latency5.bits.mst0_wr_cmd = mst0_wr_cmd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst0_det_latency5.u32) + offset),
                 vdp_core_mst0_det_latency5.u32);

    return;
}

hi_void vdp_master_setmst1rdmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_max)
{
    u_vdp_core_mst1_det_latency0 vdp_core_mst1_det_latency0;

    vdp_core_mst1_det_latency0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency0.u32) +
                                                  offset));
    vdp_core_mst1_det_latency0.bits.mst1_rd_max = mst1_rd_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency0.u32) + offset),
                 vdp_core_mst1_det_latency0.u32);

    return;
}

hi_void vdp_master_setmst1rdaver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_aver)
{
    u_vdp_core_mst1_det_latency1 vdp_core_mst1_det_latency1;

    vdp_core_mst1_det_latency1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency1.u32) +
                                                  offset));
    vdp_core_mst1_det_latency1.bits.mst1_rd_aver = mst1_rd_aver;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency1.u32) + offset),
                 vdp_core_mst1_det_latency1.u32);

    return;
}

hi_void vdp_master_setmst1rdcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_cmd)
{
    u_vdp_core_mst1_det_latency2 vdp_core_mst1_det_latency2;

    vdp_core_mst1_det_latency2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency2.u32) +
                                                  offset));
    vdp_core_mst1_det_latency2.bits.mst1_rd_cmd = mst1_rd_cmd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency2.u32) + offset),
                 vdp_core_mst1_det_latency2.u32);

    return;
}

hi_void vdp_master_setmst1wrmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_max)
{
    u_vdp_core_mst1_det_latency3 vdp_core_mst1_det_latency3;

    vdp_core_mst1_det_latency3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency3.u32) +
                                                  offset));
    vdp_core_mst1_det_latency3.bits.mst1_wr_max = mst1_wr_max;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency3.u32) + offset),
                 vdp_core_mst1_det_latency3.u32);

    return;
}

hi_void vdp_master_setmst1wraver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_aver)
{
    u_vdp_core_mst1_det_latency4 vdp_core_mst1_det_latency4;

    vdp_core_mst1_det_latency4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency4.u32) +
                                                  offset));
    vdp_core_mst1_det_latency4.bits.mst1_wr_aver = mst1_wr_aver;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency4.u32) + offset),
                 vdp_core_mst1_det_latency4.u32);

    return;
}

hi_void vdp_master_setmst1wrcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_cmd)
{
    u_vdp_core_mst1_det_latency5 vdp_core_mst1_det_latency5;

    vdp_core_mst1_det_latency5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency5.u32) +
                                                  offset));
    vdp_core_mst1_det_latency5.bits.mst1_wr_cmd = mst1_wr_cmd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_mst1_det_latency5.u32) + offset),
                 vdp_core_mst1_det_latency5.u32);

    return;
}

hi_void vdp_master_setqosmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 qos_mode)
{
    u_mst_ctrl mst_ctrl;

    mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mst_ctrl.u32) + offset));
    mst_ctrl.bits.qos_mode = qos_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mst_ctrl.u32) + offset), mst_ctrl.u32);

    return;
}

hi_void vdp_master_setqosconfigure(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 qos_cfg)
{
    u_mst_ctrl mst_ctrl;

    mst_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->mst_ctrl.u32) + offset));
    mst_ctrl.bits.qos_cfg = qos_cfg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->mst_ctrl.u32) + offset), mst_ctrl.u32);

    return;
}


hi_void vdp_master_vid_setreqbalance(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lum_req, hi_u32 chm_req)
{
    u_vid_req_ctrl vid_req_ctrl;

    vid_req_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_req_ctrl.u32)));
    vid_req_ctrl.bits.rrobin_ctrl_l = lum_req;
    vid_req_ctrl.bits.rrobin_ctrl_c = chm_req;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_req_ctrl.u32)), vid_req_ctrl.u32);

    return;
}

hi_void vdp_master_gfx_setreqbalance(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gfx_req)
{
    u_gfx_req_ctrl gfx_req_ctrl;

    gfx_req_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->gfx_req_ctrl.u32)));
    gfx_req_ctrl.bits.rrobin_ctrl = gfx_req;
    vdp_regwrite((uintptr_t)(&(vdp_reg->gfx_req_ctrl.u32)), gfx_req_ctrl.u32);

    return;
}


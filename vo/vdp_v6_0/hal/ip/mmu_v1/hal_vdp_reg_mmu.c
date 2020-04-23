/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_comm.h"
#include "hal_vdp_reg_mmu.h"

hi_void vdp_mmu_setsrclock(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src_lock)
{
    u_vdp1_smmu_scr vdp1_smmu_scr;

    vdp1_smmu_scr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_scr.u32) + offset));
    vdp1_smmu_scr.bits.src_lock = src_lock;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_scr.u32) + offset), vdp1_smmu_scr.u32);

    return;
}

hi_void vdp_mmu_setpagetyps(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 page_typ_s)
{
    u_vdp1_smmu_scr vdp1_smmu_scr;

    vdp1_smmu_scr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_scr.u32) + offset));
    vdp1_smmu_scr.bits.page_typ_s = page_typ_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_scr.u32) + offset), vdp1_smmu_scr.u32);

    return;
}

hi_void vdp_mmu_setglbbypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 glb_bypass)
{
    u_vdp1_smmu_scr vdp1_smmu_scr;

    vdp1_smmu_scr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_scr.u32) + offset));
    vdp1_smmu_scr.bits.glb_bypass = glb_bypass;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_scr.u32) + offset), vdp1_smmu_scr.u32);

    return;
}

hi_void vdp_mmu_setptwpf(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ptw_pf)
{
    u_vdp1_smmu_ctrl vdp1_smmu_ctrl;

    vdp1_smmu_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_ctrl.u32) + offset));
    vdp1_smmu_ctrl.bits.ptw_pf = ptw_pf;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_ctrl.u32) + offset), vdp1_smmu_ctrl.u32);

    return;
}

hi_void vdp_mmu_setpagetypns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 page_typ_ns)
{
    u_vdp1_smmu_ctrl vdp1_smmu_ctrl;

    vdp1_smmu_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_ctrl.u32) + offset));
    vdp1_smmu_ctrl.bits.page_typ_ns = page_typ_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_ctrl.u32) + offset), vdp1_smmu_ctrl.u32);

    return;
}

hi_void vdp_mmu_setinten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 int_en)
{
    u_vdp1_smmu_ctrl vdp1_smmu_ctrl;

    vdp1_smmu_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_ctrl.u32) + offset));
    vdp1_smmu_ctrl.bits.int_en = int_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_ctrl.u32) + offset), vdp1_smmu_ctrl.u32);

    return;
}

hi_void vdp_mmu_setmstclkgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst_clk_gt_en)
{
    u_vdp1_smmu_lp_ctrl vdp1_smmu_lp_ctrl;

    vdp1_smmu_lp_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_lp_ctrl.u32) + offset));
    vdp1_smmu_lp_ctrl.bits.mst_clk_gt_en = mst_clk_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_lp_ctrl.u32) + offset), vdp1_smmu_lp_ctrl.u32);

    return;
}

hi_void vdp_mmu_setcomclkgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 com_clk_gt_en)
{
    u_vdp1_smmu_lp_ctrl vdp1_smmu_lp_ctrl;

    vdp1_smmu_lp_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_lp_ctrl.u32) + offset));
    vdp1_smmu_lp_ctrl.bits.com_clk_gt_en = com_clk_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_lp_ctrl.u32) + offset), vdp1_smmu_lp_ctrl.u32);

    return;
}

hi_void vdp_mmu_setautoclkgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 auto_clk_gt_en)
{
    u_vdp1_smmu_lp_ctrl vdp1_smmu_lp_ctrl;

    vdp1_smmu_lp_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_lp_ctrl.u32) + offset));
    vdp1_smmu_lp_ctrl.bits.auto_clk_gt_en = auto_clk_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_lp_ctrl.u32) + offset), vdp1_smmu_lp_ctrl.u32);

    return;
}

hi_void vdp_mmu_setrfsret1n(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rfs_ret1n)
{
    u_vdp1_smmu_mem_speedctrl vdp1_smmu_mem_speedctrl;

    vdp1_smmu_mem_speedctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_mem_speedctrl.u32) + offset));
    vdp1_smmu_mem_speedctrl.bits.rfs_ret1n = rfs_ret1n;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_mem_speedctrl.u32) + offset), vdp1_smmu_mem_speedctrl.u32);

    return;
}

hi_void vdp_mmu_setrfsema(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rfs_ema)
{
    u_vdp1_smmu_mem_speedctrl vdp1_smmu_mem_speedctrl;

    vdp1_smmu_mem_speedctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_mem_speedctrl.u32) + offset));
    vdp1_smmu_mem_speedctrl.bits.rfs_ema = rfs_ema;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_mem_speedctrl.u32) + offset), vdp1_smmu_mem_speedctrl.u32);

    return;
}

hi_void vdp_mmu_setrfsemaw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rfs_emaw)
{
    u_vdp1_smmu_mem_speedctrl vdp1_smmu_mem_speedctrl;

    vdp1_smmu_mem_speedctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_mem_speedctrl.u32) + offset));
    vdp1_smmu_mem_speedctrl.bits.rfs_emaw = rfs_emaw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_mem_speedctrl.u32) + offset), vdp1_smmu_mem_speedctrl.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchwrmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_wr_msk)
{
    u_vdp1_smmu_intmask_s vdp1_smmu_intmask_s;

    vdp1_smmu_intmask_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset));
    vdp1_smmu_intmask_s.bits.ints_tlbunmatch_wr_msk = ints_tlbunmatch_wr_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset), vdp1_smmu_intmask_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchrdmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_rd_msk)
{
    u_vdp1_smmu_intmask_s vdp1_smmu_intmask_s;

    vdp1_smmu_intmask_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset));
    vdp1_smmu_intmask_s.bits.ints_tlbunmatch_rd_msk = ints_tlbunmatch_rd_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset), vdp1_smmu_intmask_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidwrmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_wr_msk)
{
    u_vdp1_smmu_intmask_s vdp1_smmu_intmask_s;

    vdp1_smmu_intmask_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset));
    vdp1_smmu_intmask_s.bits.ints_tlbinvalid_wr_msk = ints_tlbinvalid_wr_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset), vdp1_smmu_intmask_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidrdmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_rd_msk)
{
    u_vdp1_smmu_intmask_s vdp1_smmu_intmask_s;

    vdp1_smmu_intmask_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset));
    vdp1_smmu_intmask_s.bits.ints_tlbinvalid_rd_msk = ints_tlbinvalid_rd_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset), vdp1_smmu_intmask_s.u32);

    return;
}

hi_void vdp_mmu_setintsptwtransmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_ptw_trans_msk)
{
    u_vdp1_smmu_intmask_s vdp1_smmu_intmask_s;

    vdp1_smmu_intmask_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset));
    vdp1_smmu_intmask_s.bits.ints_ptw_trans_msk = ints_ptw_trans_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset), vdp1_smmu_intmask_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbmissmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbmiss_msk)
{
    u_vdp1_smmu_intmask_s vdp1_smmu_intmask_s;

    vdp1_smmu_intmask_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset));
    vdp1_smmu_intmask_s.bits.ints_tlbmiss_msk = ints_tlbmiss_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_s.u32) + offset), vdp1_smmu_intmask_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchwrraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_wr_raw)
{
    u_vdp1_smmu_intraw_s vdp1_smmu_intraw_s;

    vdp1_smmu_intraw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset));
    vdp1_smmu_intraw_s.bits.ints_tlbunmatch_wr_raw = ints_tlbunmatch_wr_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset), vdp1_smmu_intraw_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchrdraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_rd_raw)
{
    u_vdp1_smmu_intraw_s vdp1_smmu_intraw_s;

    vdp1_smmu_intraw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset));
    vdp1_smmu_intraw_s.bits.ints_tlbunmatch_rd_raw = ints_tlbunmatch_rd_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset), vdp1_smmu_intraw_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidwrraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_wr_raw)
{
    u_vdp1_smmu_intraw_s vdp1_smmu_intraw_s;

    vdp1_smmu_intraw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset));
    vdp1_smmu_intraw_s.bits.ints_tlbinvalid_wr_raw = ints_tlbinvalid_wr_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset), vdp1_smmu_intraw_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidrdraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_rd_raw)
{
    u_vdp1_smmu_intraw_s vdp1_smmu_intraw_s;

    vdp1_smmu_intraw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset));
    vdp1_smmu_intraw_s.bits.ints_tlbinvalid_rd_raw = ints_tlbinvalid_rd_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset), vdp1_smmu_intraw_s.u32);

    return;
}

hi_void vdp_mmu_setintsptwtransraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_ptw_trans_raw)
{
    u_vdp1_smmu_intraw_s vdp1_smmu_intraw_s;

    vdp1_smmu_intraw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset));
    vdp1_smmu_intraw_s.bits.ints_ptw_trans_raw = ints_ptw_trans_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset), vdp1_smmu_intraw_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbmissraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbmiss_raw)
{
    u_vdp1_smmu_intraw_s vdp1_smmu_intraw_s;

    vdp1_smmu_intraw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset));
    vdp1_smmu_intraw_s.bits.ints_tlbmiss_raw = ints_tlbmiss_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_s.u32) + offset), vdp1_smmu_intraw_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchwrstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_wr_stat)
{
    u_vdp1_smmu_intstat_s vdp1_smmu_intstat_s;

    vdp1_smmu_intstat_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset));
    vdp1_smmu_intstat_s.bits.ints_tlbunmatch_wr_stat = ints_tlbunmatch_wr_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset), vdp1_smmu_intstat_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchrdstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_rd_stat)
{
    u_vdp1_smmu_intstat_s vdp1_smmu_intstat_s;

    vdp1_smmu_intstat_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset));
    vdp1_smmu_intstat_s.bits.ints_tlbunmatch_rd_stat = ints_tlbunmatch_rd_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset), vdp1_smmu_intstat_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidwrstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_wr_stat)
{
    u_vdp1_smmu_intstat_s vdp1_smmu_intstat_s;

    vdp1_smmu_intstat_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset));
    vdp1_smmu_intstat_s.bits.ints_tlbinvalid_wr_stat = ints_tlbinvalid_wr_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset), vdp1_smmu_intstat_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidrdstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_rd_stat)
{
    u_vdp1_smmu_intstat_s vdp1_smmu_intstat_s;

    vdp1_smmu_intstat_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset));
    vdp1_smmu_intstat_s.bits.ints_tlbinvalid_rd_stat = ints_tlbinvalid_rd_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset), vdp1_smmu_intstat_s.u32);

    return;
}

hi_void vdp_mmu_setintsptwtransstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_ptw_trans_stat)
{
    u_vdp1_smmu_intstat_s vdp1_smmu_intstat_s;

    vdp1_smmu_intstat_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset));
    vdp1_smmu_intstat_s.bits.ints_ptw_trans_stat = ints_ptw_trans_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset), vdp1_smmu_intstat_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbmissstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbmiss_stat)
{
    u_vdp1_smmu_intstat_s vdp1_smmu_intstat_s;

    vdp1_smmu_intstat_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset));
    vdp1_smmu_intstat_s.bits.ints_tlbmiss_stat = ints_tlbmiss_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_s.u32) + offset), vdp1_smmu_intstat_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchwrclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_wr_clr)
{
    u_vdp1_smmu_intclr_s vdp1_smmu_intclr_s;

    vdp1_smmu_intclr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset));
    vdp1_smmu_intclr_s.bits.ints_tlbunmatch_wr_clr = ints_tlbunmatch_wr_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset), vdp1_smmu_intclr_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbunmatchrdclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbunmatch_rd_clr)
{
    u_vdp1_smmu_intclr_s vdp1_smmu_intclr_s;

    vdp1_smmu_intclr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset));
    vdp1_smmu_intclr_s.bits.ints_tlbunmatch_rd_clr = ints_tlbunmatch_rd_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset), vdp1_smmu_intclr_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidwrclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_wr_clr)
{
    u_vdp1_smmu_intclr_s vdp1_smmu_intclr_s;

    vdp1_smmu_intclr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset));
    vdp1_smmu_intclr_s.bits.ints_tlbinvalid_wr_clr = ints_tlbinvalid_wr_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset), vdp1_smmu_intclr_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbinvalidrdclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbinvalid_rd_clr)
{
    u_vdp1_smmu_intclr_s vdp1_smmu_intclr_s;

    vdp1_smmu_intclr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset));
    vdp1_smmu_intclr_s.bits.ints_tlbinvalid_rd_clr = ints_tlbinvalid_rd_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset), vdp1_smmu_intclr_s.u32);

    return;
}

hi_void vdp_mmu_setintsptwtransclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_ptw_trans_clr)
{
    u_vdp1_smmu_intclr_s vdp1_smmu_intclr_s;

    vdp1_smmu_intclr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset));
    vdp1_smmu_intclr_s.bits.ints_ptw_trans_clr = ints_ptw_trans_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset), vdp1_smmu_intclr_s.u32);

    return;
}

hi_void vdp_mmu_setintstlbmissclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ints_tlbmiss_clr)
{
    u_vdp1_smmu_intclr_s vdp1_smmu_intclr_s;

    vdp1_smmu_intclr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset));
    vdp1_smmu_intclr_s.bits.ints_tlbmiss_clr = ints_tlbmiss_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_s.u32) + offset), vdp1_smmu_intclr_s.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchwrmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_wr_msk)
{
    u_vdp1_smmu_intmask_ns vdp1_smmu_intmask_ns;

    vdp1_smmu_intmask_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset));
    vdp1_smmu_intmask_ns.bits.intns_tlbunmatch_wr_msk = intns_tlbunmatch_wr_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset), vdp1_smmu_intmask_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchrdmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_rd_msk)
{
    u_vdp1_smmu_intmask_ns vdp1_smmu_intmask_ns;

    vdp1_smmu_intmask_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset));
    vdp1_smmu_intmask_ns.bits.intns_tlbunmatch_rd_msk = intns_tlbunmatch_rd_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset), vdp1_smmu_intmask_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidwrmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_wr_msk)
{
    u_vdp1_smmu_intmask_ns vdp1_smmu_intmask_ns;

    vdp1_smmu_intmask_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset));
    vdp1_smmu_intmask_ns.bits.intns_tlbinvalid_wr_msk = intns_tlbinvalid_wr_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset), vdp1_smmu_intmask_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidrdmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_rd_msk)
{
    u_vdp1_smmu_intmask_ns vdp1_smmu_intmask_ns;

    vdp1_smmu_intmask_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset));
    vdp1_smmu_intmask_ns.bits.intns_tlbinvalid_rd_msk = intns_tlbinvalid_rd_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset), vdp1_smmu_intmask_ns.u32);

    return;
}

hi_void vdp_mmu_setintnsptwtransmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_ptw_trans_msk)
{
    u_vdp1_smmu_intmask_ns vdp1_smmu_intmask_ns;

    vdp1_smmu_intmask_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset));
    vdp1_smmu_intmask_ns.bits.intns_ptw_trans_msk = intns_ptw_trans_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset), vdp1_smmu_intmask_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbmissmsk(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbmiss_msk)
{
    u_vdp1_smmu_intmask_ns vdp1_smmu_intmask_ns;

    vdp1_smmu_intmask_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset));
    vdp1_smmu_intmask_ns.bits.intns_tlbmiss_msk = intns_tlbmiss_msk;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intmask_ns.u32) + offset), vdp1_smmu_intmask_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchwrraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_wr_raw)
{
    u_vdp1_smmu_intraw_ns vdp1_smmu_intraw_ns;

    vdp1_smmu_intraw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
    vdp1_smmu_intraw_ns.bits.intns_tlbunmatch_wr_raw = intns_tlbunmatch_wr_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset), vdp1_smmu_intraw_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchrdraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_rd_raw)
{
    u_vdp1_smmu_intraw_ns vdp1_smmu_intraw_ns;

    vdp1_smmu_intraw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
    vdp1_smmu_intraw_ns.bits.intns_tlbunmatch_rd_raw = intns_tlbunmatch_rd_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset), vdp1_smmu_intraw_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidwrraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_wr_raw)
{
    u_vdp1_smmu_intraw_ns vdp1_smmu_intraw_ns;

    vdp1_smmu_intraw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
    vdp1_smmu_intraw_ns.bits.intns_tlbinvalid_wr_raw = intns_tlbinvalid_wr_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset), vdp1_smmu_intraw_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidrdraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_rd_raw)
{
    u_vdp1_smmu_intraw_ns vdp1_smmu_intraw_ns;

    vdp1_smmu_intraw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
    vdp1_smmu_intraw_ns.bits.intns_tlbinvalid_rd_raw = intns_tlbinvalid_rd_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset), vdp1_smmu_intraw_ns.u32);

    return;
}

hi_void vdp_mmu_setintnsptwtransraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_ptw_trans_raw)
{
    u_vdp1_smmu_intraw_ns vdp1_smmu_intraw_ns;

    vdp1_smmu_intraw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
    vdp1_smmu_intraw_ns.bits.intns_ptw_trans_raw = intns_ptw_trans_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset), vdp1_smmu_intraw_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbmissraw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbmiss_raw)
{
    u_vdp1_smmu_intraw_ns vdp1_smmu_intraw_ns;

    vdp1_smmu_intraw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
    vdp1_smmu_intraw_ns.bits.intns_tlbmiss_raw = intns_tlbmiss_raw;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset), vdp1_smmu_intraw_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchwrstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_wr_stat)
{
    u_vdp1_smmu_intstat_ns vdp1_smmu_intstat_ns;

    vdp1_smmu_intstat_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset));
    vdp1_smmu_intstat_ns.bits.intns_tlbunmatch_wr_stat = intns_tlbunmatch_wr_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset), vdp1_smmu_intstat_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchrdstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_rd_stat)
{
    u_vdp1_smmu_intstat_ns vdp1_smmu_intstat_ns;

    vdp1_smmu_intstat_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset));
    vdp1_smmu_intstat_ns.bits.intns_tlbunmatch_rd_stat = intns_tlbunmatch_rd_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset), vdp1_smmu_intstat_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidwrstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_wr_stat)
{
    u_vdp1_smmu_intstat_ns vdp1_smmu_intstat_ns;

    vdp1_smmu_intstat_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset));
    vdp1_smmu_intstat_ns.bits.intns_tlbinvalid_wr_stat = intns_tlbinvalid_wr_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset), vdp1_smmu_intstat_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidrdstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_rd_stat)
{
    u_vdp1_smmu_intstat_ns vdp1_smmu_intstat_ns;

    vdp1_smmu_intstat_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset));
    vdp1_smmu_intstat_ns.bits.intns_tlbinvalid_rd_stat = intns_tlbinvalid_rd_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset), vdp1_smmu_intstat_ns.u32);

    return;
}

hi_void vdp_mmu_setintnsptwtransstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_ptw_trans_stat)
{
    u_vdp1_smmu_intstat_ns vdp1_smmu_intstat_ns;

    vdp1_smmu_intstat_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset));
    vdp1_smmu_intstat_ns.bits.intns_ptw_trans_stat = intns_ptw_trans_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset), vdp1_smmu_intstat_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbmissstat(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbmiss_stat)
{
    u_vdp1_smmu_intstat_ns vdp1_smmu_intstat_ns;

    vdp1_smmu_intstat_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset));
    vdp1_smmu_intstat_ns.bits.intns_tlbmiss_stat = intns_tlbmiss_stat;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intstat_ns.u32) + offset), vdp1_smmu_intstat_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchwrclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_wr_clr)
{
    u_vdp1_smmu_intclr_ns vdp1_smmu_intclr_ns;

    vdp1_smmu_intclr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset));
    vdp1_smmu_intclr_ns.bits.intns_tlbunmatch_wr_clr = intns_tlbunmatch_wr_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), vdp1_smmu_intclr_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbunmatchrdclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbunmatch_rd_clr)
{
    u_vdp1_smmu_intclr_ns vdp1_smmu_intclr_ns;

    vdp1_smmu_intclr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset));
    vdp1_smmu_intclr_ns.bits.intns_tlbunmatch_rd_clr = intns_tlbunmatch_rd_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), vdp1_smmu_intclr_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidwrclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_wr_clr)
{
    u_vdp1_smmu_intclr_ns vdp1_smmu_intclr_ns;

    vdp1_smmu_intclr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset));
    vdp1_smmu_intclr_ns.bits.intns_tlbinvalid_wr_clr = intns_tlbinvalid_wr_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), vdp1_smmu_intclr_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbinvalidrdclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbinvalid_rd_clr)
{
    u_vdp1_smmu_intclr_ns vdp1_smmu_intclr_ns;

    vdp1_smmu_intclr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset));
    vdp1_smmu_intclr_ns.bits.intns_tlbinvalid_rd_clr = intns_tlbinvalid_rd_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), vdp1_smmu_intclr_ns.u32);

    return;
}

hi_void vdp_mmu_setintnsptwtransclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_ptw_trans_clr)
{
    u_vdp1_smmu_intclr_ns vdp1_smmu_intclr_ns;

    vdp1_smmu_intclr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset));
    vdp1_smmu_intclr_ns.bits.intns_ptw_trans_clr = intns_ptw_trans_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), vdp1_smmu_intclr_ns.u32);

    return;
}

hi_void vdp_mmu_setintnstlbmissclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 intns_tlbmiss_clr)
{
    u_vdp1_smmu_intclr_ns vdp1_smmu_intclr_ns;

    vdp1_smmu_intclr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset));
    vdp1_smmu_intclr_ns.bits.intns_tlbmiss_clr = intns_tlbmiss_clr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), vdp1_smmu_intclr_ns.u32);

    return;
}

hi_void vdp_mmu_setscbttbr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scb_ttbr)
{
    u_vdp1_smmu_scb_ttbr vdp1_smmu_scb_ttbr;

    vdp1_smmu_scb_ttbr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_scb_ttbr.u32) + offset));
    vdp1_smmu_scb_ttbr.bits.scb_ttbr = scb_ttbr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_scb_ttbr.u32) + offset), vdp1_smmu_scb_ttbr.u32);

    return;
}

hi_void vdp_mmu_setscbttbrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scb_ttbr_h)
{
    u_vdp1_smmu_scb_ttbr_h vdp1_smmu_scb_ttbr_h;

    vdp1_smmu_scb_ttbr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_scb_ttbr_h.u32) + offset));
    vdp1_smmu_scb_ttbr_h.bits.scb_ttbr_h = scb_ttbr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_scb_ttbr_h.u32) + offset), vdp1_smmu_scb_ttbr_h.u32);

    return;
}

hi_void vdp_mmu_setscmdtagrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scmd_tag_rd_en)
{
    u_vdp1_smmu_stag_rd_ctrl vdp1_smmu_stag_rd_ctrl;

    vdp1_smmu_stag_rd_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_rd_ctrl.u32) + offset));
    vdp1_smmu_stag_rd_ctrl.bits.scmd_tag_rd_en = scmd_tag_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_rd_ctrl.u32) + offset), vdp1_smmu_stag_rd_ctrl.u32);

    return;
}

hi_void vdp_mmu_setscmdrdaccess0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scmd_rd_access0)
{
    u_vdp1_smmu_stag_rd_access0 vdp1_smmu_stag_rd_access0;

    vdp1_smmu_stag_rd_access0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_rd_access0.u32) + offset));
    vdp1_smmu_stag_rd_access0.bits.scmd_rd_access0 = scmd_rd_access0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_rd_access0.u32) + offset), vdp1_smmu_stag_rd_access0.u32);

    return;
}

hi_void vdp_mmu_setscmdrdaccess1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scmd_rd_access1)
{
    u_vdp1_smmu_stag_rd_access1 vdp1_smmu_stag_rd_access1;

    vdp1_smmu_stag_rd_access1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_rd_access1.u32) + offset));
    vdp1_smmu_stag_rd_access1.bits.scmd_rd_access1 = scmd_rd_access1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_rd_access1.u32) + offset), vdp1_smmu_stag_rd_access1.u32);

    return;
}

hi_void vdp_mmu_setscmdtagwren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scmd_tag_wr_en)
{
    u_vdp1_smmu_stag_wr_ctrl vdp1_smmu_stag_wr_ctrl;

    vdp1_smmu_stag_wr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_wr_ctrl.u32) + offset));
    vdp1_smmu_stag_wr_ctrl.bits.scmd_tag_wr_en = scmd_tag_wr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_wr_ctrl.u32) + offset), vdp1_smmu_stag_wr_ctrl.u32);

    return;
}

hi_void vdp_mmu_setscmdwraccess0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scmd_wr_access0)
{
    u_vdp1_smmu_stag_wr_access0 vdp1_smmu_stag_wr_access0;

    vdp1_smmu_stag_wr_access0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_wr_access0.u32) + offset));
    vdp1_smmu_stag_wr_access0.bits.scmd_wr_access0 = scmd_wr_access0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_wr_access0.u32) + offset), vdp1_smmu_stag_wr_access0.u32);

    return;
}

hi_void vdp_mmu_setscmdwraccess1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scmd_wr_access1)
{
    u_vdp1_smmu_stag_wr_access1 vdp1_smmu_stag_wr_access1;

    vdp1_smmu_stag_wr_access1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_wr_access1.u32) + offset));
    vdp1_smmu_stag_wr_access1.bits.scmd_wr_access1 = scmd_wr_access1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_stag_wr_access1.u32) + offset), vdp1_smmu_stag_wr_access1.u32);

    return;
}

hi_void vdp_mmu_seterrsrdaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_s_rd_addr)
{
    u_vdp1_smmu_err_rdaddr_s vdp1_smmu_err_rdaddr_s;

    vdp1_smmu_err_rdaddr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_s.u32) + offset));
    vdp1_smmu_err_rdaddr_s.bits.err_s_rd_addr = err_s_rd_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_s.u32) + offset), vdp1_smmu_err_rdaddr_s.u32);

    return;
}

hi_void vdp_mmu_seterrsrdaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_s_rd_addr_h)
{
    u_vdp1_smmu_err_rdaddr_h_s vdp1_smmu_err_rdaddr_h_s;

    vdp1_smmu_err_rdaddr_h_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_h_s.u32) + offset));
    vdp1_smmu_err_rdaddr_h_s.bits.err_s_rd_addr_h = err_s_rd_addr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_h_s.u32) + offset), vdp1_smmu_err_rdaddr_h_s.u32);

    return;
}

hi_void vdp_mmu_seterrswraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_s_wr_addr)
{
    u_vdp1_smmu_err_wraddr_s vdp1_smmu_err_wraddr_s;

    vdp1_smmu_err_wraddr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_s.u32) + offset));
    vdp1_smmu_err_wraddr_s.bits.err_s_wr_addr = err_s_wr_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_s.u32) + offset), vdp1_smmu_err_wraddr_s.u32);

    return;
}

hi_void vdp_mmu_seterrswraddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_s_wr_addr_h)
{
    u_vdp1_smmu_err_wraddr_h_s vdp1_smmu_err_wraddr_h_s;

    vdp1_smmu_err_wraddr_h_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_h_s.u32) + offset));
    vdp1_smmu_err_wraddr_h_s.bits.err_s_wr_addr_h = err_s_wr_addr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_h_s.u32) + offset), vdp1_smmu_err_wraddr_h_s.u32);

    return;
}

hi_void vdp_mmu_setcbttbr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cb_ttbr)
{
    u_vdp1_smmu_cb_ttbr vdp1_smmu_cb_ttbr;

    vdp1_smmu_cb_ttbr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_cb_ttbr.u32) + offset));
    vdp1_smmu_cb_ttbr.bits.cb_ttbr = cb_ttbr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_cb_ttbr.u32) + offset), vdp1_smmu_cb_ttbr.u32);

    return;
}

hi_void vdp_mmu_setcbttbrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cb_ttbr_h)
{
    u_vdp1_smmu_cb_ttbr_h vdp1_smmu_cb_ttbr_h;

    vdp1_smmu_cb_ttbr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_cb_ttbr_h.u32) + offset));
    vdp1_smmu_cb_ttbr_h.bits.cb_ttbr_h = cb_ttbr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_cb_ttbr_h.u32) + offset), vdp1_smmu_cb_ttbr_h.u32);

    return;
}

hi_void vdp_mmu_setcmdtagrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmd_tag_rd_en)
{
    u_vdp1_smmu_tag_rd_ctrl vdp1_smmu_tag_rd_ctrl;

    vdp1_smmu_tag_rd_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_rd_ctrl.u32) + offset));
    vdp1_smmu_tag_rd_ctrl.bits.cmd_tag_rd_en = cmd_tag_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_rd_ctrl.u32) + offset), vdp1_smmu_tag_rd_ctrl.u32);

    return;
}

hi_void vdp_mmu_setcmdrdaccess0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmd_rd_access0)
{
    u_vdp1_smmu_tag_rd_access0 vdp1_smmu_tag_rd_access0;

    vdp1_smmu_tag_rd_access0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_rd_access0.u32) + offset));
    vdp1_smmu_tag_rd_access0.bits.cmd_rd_access0 = cmd_rd_access0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_rd_access0.u32) + offset), vdp1_smmu_tag_rd_access0.u32);

    return;
}

hi_void vdp_mmu_setcmdrdaccess1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmd_rd_access1)
{
    u_vdp1_smmu_tag_rd_access1 vdp1_smmu_tag_rd_access1;

    vdp1_smmu_tag_rd_access1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_rd_access1.u32) + offset));
    vdp1_smmu_tag_rd_access1.bits.cmd_rd_access1 = cmd_rd_access1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_rd_access1.u32) + offset), vdp1_smmu_tag_rd_access1.u32);

    return;
}

hi_void vdp_mmu_setcmdtagwren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmd_tag_wr_en)
{
    u_vdp1_smmu_tag_wr_ctrl vdp1_smmu_tag_wr_ctrl;

    vdp1_smmu_tag_wr_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_wr_ctrl.u32) + offset));
    vdp1_smmu_tag_wr_ctrl.bits.cmd_tag_wr_en = cmd_tag_wr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_wr_ctrl.u32) + offset), vdp1_smmu_tag_wr_ctrl.u32);

    return;
}

hi_void vdp_mmu_setcmdwraccess0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmd_wr_access0)
{
    u_vdp1_smmu_tag_wr_access0 vdp1_smmu_tag_wr_access0;

    vdp1_smmu_tag_wr_access0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_wr_access0.u32) + offset));
    vdp1_smmu_tag_wr_access0.bits.cmd_wr_access0 = cmd_wr_access0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_wr_access0.u32) + offset), vdp1_smmu_tag_wr_access0.u32);

    return;
}

hi_void vdp_mmu_setcmdwraccess1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmd_wr_access1)
{
    u_vdp1_smmu_tag_wr_access1 vdp1_smmu_tag_wr_access1;

    vdp1_smmu_tag_wr_access1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_wr_access1.u32) + offset));
    vdp1_smmu_tag_wr_access1.bits.cmd_wr_access1 = cmd_wr_access1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tag_wr_access1.u32) + offset), vdp1_smmu_tag_wr_access1.u32);

    return;
}

hi_void vdp_mmu_seterrnsrdaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_ns_rd_addr)
{
    u_vdp1_smmu_err_rdaddr_ns vdp1_smmu_err_rdaddr_ns;

    vdp1_smmu_err_rdaddr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_ns.u32) + offset));
    vdp1_smmu_err_rdaddr_ns.bits.err_ns_rd_addr = err_ns_rd_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_ns.u32) + offset), vdp1_smmu_err_rdaddr_ns.u32);

    return;
}

hi_void vdp_mmu_seterrnsrdaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_ns_rd_addr_h)
{
    u_vdp1_smmu_err_rdaddr_h_ns vdp1_smmu_err_rdaddr_h_ns;

    vdp1_smmu_err_rdaddr_h_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_h_ns.u32) + offset));
    vdp1_smmu_err_rdaddr_h_ns.bits.err_ns_rd_addr_h = err_ns_rd_addr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_rdaddr_h_ns.u32) + offset), vdp1_smmu_err_rdaddr_h_ns.u32);

    return;
}

hi_void vdp_mmu_seterrnswraddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_ns_wr_addr)
{
    u_vdp1_smmu_err_wraddr_ns vdp1_smmu_err_wraddr_ns;

    vdp1_smmu_err_wraddr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_ns.u32) + offset));
    vdp1_smmu_err_wraddr_ns.bits.err_ns_wr_addr = err_ns_wr_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_ns.u32) + offset), vdp1_smmu_err_wraddr_ns.u32);

    return;
}

hi_void vdp_mmu_seterrnswraddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 err_ns_wr_addr_h)
{
    u_vdp1_smmu_err_wraddr_h_ns vdp1_smmu_err_wraddr_h_ns;

    vdp1_smmu_err_wraddr_h_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_h_ns.u32) + offset));
    vdp1_smmu_err_wraddr_h_ns.bits.err_ns_wr_addr_h = err_ns_wr_addr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_err_wraddr_h_ns.u32) + offset), vdp1_smmu_err_wraddr_h_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultaddrptws(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_addr_ptw_s)
{
    u_vdp1_smmu_fault_addr_ptw_s vdp1_smmu_fault_addr_ptw_s;

    vdp1_smmu_fault_addr_ptw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_ptw_s.u32) +
                                                  offset));
    vdp1_smmu_fault_addr_ptw_s.bits.fault_addr_ptw_s = fault_addr_ptw_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_ptw_s.u32) + offset),
                 vdp1_smmu_fault_addr_ptw_s.u32);

    return;
}

hi_void vdp_mmu_setfaultsidptws(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_sid_ptw_s)
{
    u_vdp1_smmu_fault_id_ptw_s vdp1_smmu_fault_id_ptw_s;

    vdp1_smmu_fault_id_ptw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_s.u32) + offset));
    vdp1_smmu_fault_id_ptw_s.bits.fault_sid_ptw_s = fault_sid_ptw_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_s.u32) + offset), vdp1_smmu_fault_id_ptw_s.u32);

    return;
}

hi_void vdp_mmu_setfaultiidptws(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_iid_ptw_s)
{
    u_vdp1_smmu_fault_id_ptw_s vdp1_smmu_fault_id_ptw_s;

    vdp1_smmu_fault_id_ptw_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_s.u32) + offset));
    vdp1_smmu_fault_id_ptw_s.bits.fault_iid_ptw_s = fault_iid_ptw_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_s.u32) + offset), vdp1_smmu_fault_id_ptw_s.u32);

    return;
}

hi_void vdp_mmu_setfaultaddrptwns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_addr_ptw_ns)
{
    u_vdp1_smmu_fault_addr_ptw_ns vdp1_smmu_fault_addr_ptw_ns;

    vdp1_smmu_fault_addr_ptw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_ptw_ns.u32) +
                                                   offset));
    vdp1_smmu_fault_addr_ptw_ns.bits.fault_addr_ptw_ns = fault_addr_ptw_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_ptw_ns.u32) + offset),
                 vdp1_smmu_fault_addr_ptw_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultsidptwns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_sid_ptw_ns)
{
    u_vdp1_smmu_fault_id_ptw_ns vdp1_smmu_fault_id_ptw_ns;

    vdp1_smmu_fault_id_ptw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_ns.u32) + offset));
    vdp1_smmu_fault_id_ptw_ns.bits.fault_sid_ptw_ns = fault_sid_ptw_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_ns.u32) + offset), vdp1_smmu_fault_id_ptw_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultiidptwns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_iid_ptw_ns)
{
    u_vdp1_smmu_fault_id_ptw_ns vdp1_smmu_fault_id_ptw_ns;

    vdp1_smmu_fault_id_ptw_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_ns.u32) + offset));
    vdp1_smmu_fault_id_ptw_ns.bits.fault_iid_ptw_ns = fault_iid_ptw_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_ptw_ns.u32) + offset), vdp1_smmu_fault_id_ptw_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultaddrwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_addr_wr_s)
{
    u_vdp1_smmu_fault_addr_wr_s vdp1_smmu_fault_addr_wr_s;

    vdp1_smmu_fault_addr_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_wr_s.u32) + offset));
    vdp1_smmu_fault_addr_wr_s.bits.fault_addr_wr_s = fault_addr_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_wr_s.u32) + offset), vdp1_smmu_fault_addr_wr_s.u32);

    return;
}

hi_void vdp_mmu_setfaulttlbwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_tlb_wr_s)
{
    u_vdp1_smmu_fault_tlb_wr_s vdp1_smmu_fault_tlb_wr_s;

    vdp1_smmu_fault_tlb_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_wr_s.u32) + offset));
    vdp1_smmu_fault_tlb_wr_s.bits.fault_tlb_wr_s = fault_tlb_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_wr_s.u32) + offset), vdp1_smmu_fault_tlb_wr_s.u32);

    return;
}

hi_void vdp_mmu_setfaultsidwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_sid_wr_s)
{
    u_vdp1_smmu_fault_id_wr_s vdp1_smmu_fault_id_wr_s;

    vdp1_smmu_fault_id_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_s.u32) + offset));
    vdp1_smmu_fault_id_wr_s.bits.fault_sid_wr_s = fault_sid_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_s.u32) + offset), vdp1_smmu_fault_id_wr_s.u32);

    return;
}

hi_void vdp_mmu_setfaultiidwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_iid_wr_s)
{
    u_vdp1_smmu_fault_id_wr_s vdp1_smmu_fault_id_wr_s;

    vdp1_smmu_fault_id_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_s.u32) + offset));
    vdp1_smmu_fault_id_wr_s.bits.fault_iid_wr_s = fault_iid_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_s.u32) + offset), vdp1_smmu_fault_id_wr_s.u32);

    return;
}

hi_void vdp_mmu_setfaultaddrwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_addr_wr_ns)
{
    u_vdp1_smmu_fault_addr_wr_ns vdp1_smmu_fault_addr_wr_ns;

    vdp1_smmu_fault_addr_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_wr_ns.u32) +
                                                  offset));
    vdp1_smmu_fault_addr_wr_ns.bits.fault_addr_wr_ns = fault_addr_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_wr_ns.u32) + offset),
                 vdp1_smmu_fault_addr_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setfaulttlbwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_tlb_wr_ns)
{
    u_vdp1_smmu_fault_tlb_wr_ns vdp1_smmu_fault_tlb_wr_ns;

    vdp1_smmu_fault_tlb_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_wr_ns.u32) + offset));
    vdp1_smmu_fault_tlb_wr_ns.bits.fault_tlb_wr_ns = fault_tlb_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_wr_ns.u32) + offset), vdp1_smmu_fault_tlb_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultsidwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_sid_wr_ns)
{
    u_vdp1_smmu_fault_id_wr_ns vdp1_smmu_fault_id_wr_ns;

    vdp1_smmu_fault_id_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_ns.u32) + offset));
    vdp1_smmu_fault_id_wr_ns.bits.fault_sid_wr_ns = fault_sid_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_ns.u32) + offset), vdp1_smmu_fault_id_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultiidwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_iid_wr_ns)
{
    u_vdp1_smmu_fault_id_wr_ns vdp1_smmu_fault_id_wr_ns;

    vdp1_smmu_fault_id_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_ns.u32) + offset));
    vdp1_smmu_fault_id_wr_ns.bits.fault_iid_wr_ns = fault_iid_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_wr_ns.u32) + offset), vdp1_smmu_fault_id_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultaddrrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_addr_rd_s)
{
    u_vdp1_smmu_fault_addr_rd_s vdp1_smmu_fault_addr_rd_s;

    vdp1_smmu_fault_addr_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_rd_s.u32) + offset));
    vdp1_smmu_fault_addr_rd_s.bits.fault_addr_rd_s = fault_addr_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_rd_s.u32) + offset), vdp1_smmu_fault_addr_rd_s.u32);

    return;
}

hi_void vdp_mmu_setfaulttlbrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_tlb_rd_s)
{
    u_vdp1_smmu_fault_tlb_rd_s vdp1_smmu_fault_tlb_rd_s;

    vdp1_smmu_fault_tlb_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_rd_s.u32) + offset));
    vdp1_smmu_fault_tlb_rd_s.bits.fault_tlb_rd_s = fault_tlb_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_rd_s.u32) + offset), vdp1_smmu_fault_tlb_rd_s.u32);

    return;
}

hi_void vdp_mmu_setfaultsidrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_sid_rd_s)
{
    u_vdp1_smmu_fault_id_rd_s vdp1_smmu_fault_id_rd_s;

    vdp1_smmu_fault_id_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_s.u32) + offset));
    vdp1_smmu_fault_id_rd_s.bits.fault_sid_rd_s = fault_sid_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_s.u32) + offset), vdp1_smmu_fault_id_rd_s.u32);

    return;
}

hi_void vdp_mmu_setfaultiidrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_iid_rd_s)
{
    u_vdp1_smmu_fault_id_rd_s vdp1_smmu_fault_id_rd_s;

    vdp1_smmu_fault_id_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_s.u32) + offset));
    vdp1_smmu_fault_id_rd_s.bits.fault_iid_rd_s = fault_iid_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_s.u32) + offset), vdp1_smmu_fault_id_rd_s.u32);

    return;
}

hi_void vdp_mmu_setfaultaddrrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_addr_rd_ns)
{
    u_vdp1_smmu_fault_addr_rd_ns vdp1_smmu_fault_addr_rd_ns;

    vdp1_smmu_fault_addr_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_rd_ns.u32) +
                                                  offset));
    vdp1_smmu_fault_addr_rd_ns.bits.fault_addr_rd_ns = fault_addr_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_rd_ns.u32) + offset),
                 vdp1_smmu_fault_addr_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setfaulttlbrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_tlb_rd_ns)
{
    u_vdp1_smmu_fault_tlb_rd_ns vdp1_smmu_fault_tlb_rd_ns;

    vdp1_smmu_fault_tlb_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_rd_ns.u32) + offset));
    vdp1_smmu_fault_tlb_rd_ns.bits.fault_tlb_rd_ns = fault_tlb_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_tlb_rd_ns.u32) + offset), vdp1_smmu_fault_tlb_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultsidrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_sid_rd_ns)
{
    u_vdp1_smmu_fault_id_rd_ns vdp1_smmu_fault_id_rd_ns;

    vdp1_smmu_fault_id_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_ns.u32) + offset));
    vdp1_smmu_fault_id_rd_ns.bits.fault_sid_rd_ns = fault_sid_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_ns.u32) + offset), vdp1_smmu_fault_id_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setfaultiidrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fault_iid_rd_ns)
{
    u_vdp1_smmu_fault_id_rd_ns vdp1_smmu_fault_id_rd_ns;

    vdp1_smmu_fault_id_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_ns.u32) + offset));
    vdp1_smmu_fault_id_rd_ns.bits.fault_iid_rd_ns = fault_iid_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_id_rd_ns.u32) + offset), vdp1_smmu_fault_id_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchaddrwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_addr_wr_s)
{
    u_vdp1_smmu_match_addr_wr_s vdp1_smmu_match_addr_wr_s;

    vdp1_smmu_match_addr_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_wr_s.u32) + offset));
    vdp1_smmu_match_addr_wr_s.bits.match_addr_wr_s = match_addr_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_wr_s.u32) + offset), vdp1_smmu_match_addr_wr_s.u32);

    return;
}

hi_void vdp_mmu_setmatchtlbwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_tlb_wr_s)
{
    u_vdp1_smmu_match_tlb_wr_s vdp1_smmu_match_tlb_wr_s;

    vdp1_smmu_match_tlb_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_tlb_wr_s.u32) + offset));
    vdp1_smmu_match_tlb_wr_s.bits.match_tlb_wr_s = match_tlb_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_tlb_wr_s.u32) + offset), vdp1_smmu_match_tlb_wr_s.u32);

    return;
}

hi_void vdp_mmu_setmatchsidwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_sid_wr_s)
{
    u_vdp1_smmu_match_id_wr_s vdp1_smmu_match_id_wr_s;

    vdp1_smmu_match_id_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_wr_s.u32) + offset));
    vdp1_smmu_match_id_wr_s.bits.match_sid_wr_s = match_sid_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_wr_s.u32) + offset), vdp1_smmu_match_id_wr_s.u32);

    return;
}

hi_void vdp_mmu_setmatchiidwrs(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_iid_wr_s)
{
    u_vdp1_smmu_match_id_wr_s vdp1_smmu_match_id_wr_s;

    vdp1_smmu_match_id_wr_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_wr_s.u32) + offset));
    vdp1_smmu_match_id_wr_s.bits.match_iid_wr_s = match_iid_wr_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_wr_s.u32) + offset), vdp1_smmu_match_id_wr_s.u32);

    return;
}

hi_void vdp_mmu_setmatchaddrwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_addr_wr_ns)
{
    u_vdp1_smmu_match_addr_wr_ns vdp1_smmu_match_addr_wr_ns;

    vdp1_smmu_match_addr_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_wr_ns.u32) +
                                                  offset));
    vdp1_smmu_match_addr_wr_ns.bits.match_addr_wr_ns = match_addr_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_wr_ns.u32) + offset),
                 vdp1_smmu_match_addr_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchtlbwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_tlb_wr_ns)
{
    u_vdp1_smmu_match__tlb_wr_ns vdp1_smmu_match__tlb_wr_ns;

    vdp1_smmu_match__tlb_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match__tlb_wr_ns.u32) +
                                                  offset));
    vdp1_smmu_match__tlb_wr_ns.bits.match_tlb_wr_ns = match_tlb_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match__tlb_wr_ns.u32) + offset),
                 vdp1_smmu_match__tlb_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchsidwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_sid_wr_ns)
{
    u_vdp1_smmu_match__id_wr_ns vdp1_smmu_match__id_wr_ns;

    vdp1_smmu_match__id_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match__id_wr_ns.u32) + offset));
    vdp1_smmu_match__id_wr_ns.bits.match_sid_wr_ns = match_sid_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match__id_wr_ns.u32) + offset), vdp1_smmu_match__id_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchiidwrns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_iid_wr_ns)
{
    u_vdp1_smmu_match__id_wr_ns vdp1_smmu_match__id_wr_ns;

    vdp1_smmu_match__id_wr_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match__id_wr_ns.u32) + offset));
    vdp1_smmu_match__id_wr_ns.bits.match_iid_wr_ns = match_iid_wr_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match__id_wr_ns.u32) + offset), vdp1_smmu_match__id_wr_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchaddrrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_addr_rd_s)
{
    u_vdp1_smmu_match_addr_rd_s vdp1_smmu_match_addr_rd_s;

    vdp1_smmu_match_addr_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_rd_s.u32) + offset));
    vdp1_smmu_match_addr_rd_s.bits.match_addr_rd_s = match_addr_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_rd_s.u32) + offset), vdp1_smmu_match_addr_rd_s.u32);

    return;
}

hi_void vdp_mmu_setmatchtlbrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_tlb_rd_s)
{
    u_vdp1_smmu_match_tlb_rd_s vdp1_smmu_match_tlb_rd_s;

    vdp1_smmu_match_tlb_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_tlb_rd_s.u32) + offset));
    vdp1_smmu_match_tlb_rd_s.bits.match_tlb_rd_s = match_tlb_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_tlb_rd_s.u32) + offset), vdp1_smmu_match_tlb_rd_s.u32);

    return;
}

hi_void vdp_mmu_setmatchsidrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_sid_rd_s)
{
    u_vdp1_smmu_match_id_rd_s vdp1_smmu_match_id_rd_s;

    vdp1_smmu_match_id_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_s.u32) + offset));
    vdp1_smmu_match_id_rd_s.bits.match_sid_rd_s = match_sid_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_s.u32) + offset), vdp1_smmu_match_id_rd_s.u32);

    return;
}

hi_void vdp_mmu_setmatchiidrds(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_iid_rd_s)
{
    u_vdp1_smmu_match_id_rd_s vdp1_smmu_match_id_rd_s;

    vdp1_smmu_match_id_rd_s.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_s.u32) + offset));
    vdp1_smmu_match_id_rd_s.bits.match_iid_rd_s = match_iid_rd_s;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_s.u32) + offset), vdp1_smmu_match_id_rd_s.u32);

    return;
}

hi_void vdp_mmu_setmatchaddrrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_addr_rd_ns)
{
    u_vdp1_smmu_match_addr_rd_ns vdp1_smmu_match_addr_rd_ns;

    vdp1_smmu_match_addr_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_rd_ns.u32) +
                                                  offset));
    vdp1_smmu_match_addr_rd_ns.bits.match_addr_rd_ns = match_addr_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_addr_rd_ns.u32) + offset),
                 vdp1_smmu_match_addr_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchtlbrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_tlb_rd_ns)
{
    u_vdp1_smmu_match_tlb_rd_ns vdp1_smmu_match_tlb_rd_ns;

    vdp1_smmu_match_tlb_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_tlb_rd_ns.u32) + offset));
    vdp1_smmu_match_tlb_rd_ns.bits.match_tlb_rd_ns = match_tlb_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_tlb_rd_ns.u32) + offset), vdp1_smmu_match_tlb_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchsidrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_sid_rd_ns)
{
    u_vdp1_smmu_match_id_rd_ns vdp1_smmu_match_id_rd_ns;

    vdp1_smmu_match_id_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_ns.u32) + offset));
    vdp1_smmu_match_id_rd_ns.bits.match_sid_rd_ns = match_sid_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_ns.u32) + offset), vdp1_smmu_match_id_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setmatchiidrdns(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 match_iid_rd_ns)
{
    u_vdp1_smmu_match_id_rd_ns vdp1_smmu_match_id_rd_ns;

    vdp1_smmu_match_id_rd_ns.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_ns.u32) + offset));
    vdp1_smmu_match_id_rd_ns.bits.match_iid_rd_ns = match_iid_rd_ns;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_match_id_rd_ns.u32) + offset), vdp1_smmu_match_id_rd_ns.u32);

    return;
}

hi_void vdp_mmu_setprefdbg0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 pref_dbg0)
{
    u_vdp1_smmu_pref_dbg0 vdp1_smmu_pref_dbg0;

    vdp1_smmu_pref_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg0.u32) + offset));
    vdp1_smmu_pref_dbg0.bits.pref_dbg0 = pref_dbg0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg0.u32) + offset), vdp1_smmu_pref_dbg0.u32);

    return;
}

hi_void vdp_mmu_setdbgprefidle(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbg_pref_idle)
{
    u_vdp1_smmu_pref_dbg0 vdp1_smmu_pref_dbg0;

    vdp1_smmu_pref_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg0.u32) + offset));
    vdp1_smmu_pref_dbg0.bits.dbg_pref_idle = dbg_pref_idle;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg0.u32) + offset), vdp1_smmu_pref_dbg0.u32);

    return;
}

hi_void vdp_mmu_setdbgprefosd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dbg_pref_osd)
{
    u_vdp1_smmu_pref_dbg0 vdp1_smmu_pref_dbg0;

    vdp1_smmu_pref_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg0.u32) + offset));
    vdp1_smmu_pref_dbg0.bits.dbg_pref_osd = dbg_pref_osd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg0.u32) + offset), vdp1_smmu_pref_dbg0.u32);

    return;
}

hi_void vdp_mmu_setprefdbg1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 pref_dbg1)
{
    u_vdp1_smmu_pref_dbg1 vdp1_smmu_pref_dbg1;

    vdp1_smmu_pref_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg1.u32) + offset));
    vdp1_smmu_pref_dbg1.bits.pref_dbg1 = pref_dbg1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg1.u32) + offset), vdp1_smmu_pref_dbg1.u32);

    return;
}

hi_void vdp_mmu_setprefdbg2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 pref_dbg2)
{
    u_vdp1_smmu_pref_dbg2 vdp1_smmu_pref_dbg2;

    vdp1_smmu_pref_dbg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg2.u32) + offset));
    vdp1_smmu_pref_dbg2.bits.pref_dbg2 = pref_dbg2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg2.u32) + offset), vdp1_smmu_pref_dbg2.u32);

    return;
}

hi_void vdp_mmu_setprefdbg3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 pref_dbg3)
{
    u_vdp1_smmu_pref_dbg3 vdp1_smmu_pref_dbg3;

    vdp1_smmu_pref_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg3.u32) + offset));
    vdp1_smmu_pref_dbg3.bits.pref_dbg3 = pref_dbg3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_pref_dbg3.u32) + offset), vdp1_smmu_pref_dbg3.u32);

    return;
}

hi_void vdp_mmu_settburidle(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tbu_r_idle)
{
    u_vdp1_smmu_tbu_dbg0 vdp1_smmu_tbu_dbg0;

    vdp1_smmu_tbu_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset));
    vdp1_smmu_tbu_dbg0.bits.tbu_r_idle = tbu_r_idle;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset), vdp1_smmu_tbu_dbg0.u32);

    return;
}

hi_void vdp_mmu_settbudbg0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tbu_dbg0)
{
    u_vdp1_smmu_tbu_dbg0 vdp1_smmu_tbu_dbg0;

    vdp1_smmu_tbu_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset));
    vdp1_smmu_tbu_dbg0.bits.tbu_dbg0 = tbu_dbg0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset), vdp1_smmu_tbu_dbg0.u32);

    return;
}

hi_void vdp_mmu_setrdummycnt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r_dummy_cnt)
{
    u_vdp1_smmu_tbu_dbg0 vdp1_smmu_tbu_dbg0;

    vdp1_smmu_tbu_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset));
    vdp1_smmu_tbu_dbg0.bits.r_dummy_cnt = r_dummy_cnt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset), vdp1_smmu_tbu_dbg0.u32);

    return;
}

hi_void vdp_mmu_setrincnt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r_in_cnt)
{
    u_vdp1_smmu_tbu_dbg0 vdp1_smmu_tbu_dbg0;

    vdp1_smmu_tbu_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset));
    vdp1_smmu_tbu_dbg0.bits.r_in_cnt = r_in_cnt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset), vdp1_smmu_tbu_dbg0.u32);

    return;
}

hi_void vdp_mmu_setroutcnt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 r_out_cnt)
{
    u_vdp1_smmu_tbu_dbg0 vdp1_smmu_tbu_dbg0;

    vdp1_smmu_tbu_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset));
    vdp1_smmu_tbu_dbg0.bits.r_out_cnt = r_out_cnt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg0.u32) + offset), vdp1_smmu_tbu_dbg0.u32);

    return;
}

hi_void vdp_mmu_settbuwidle(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tbu_w_idle)
{
    u_vdp1_smmu_tbu_dbg1 vdp1_smmu_tbu_dbg1;

    vdp1_smmu_tbu_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset));
    vdp1_smmu_tbu_dbg1.bits.tbu_w_idle = tbu_w_idle;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset), vdp1_smmu_tbu_dbg1.u32);

    return;
}

hi_void vdp_mmu_settbudbg1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tbu_dbg1)
{
    u_vdp1_smmu_tbu_dbg1 vdp1_smmu_tbu_dbg1;

    vdp1_smmu_tbu_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset));
    vdp1_smmu_tbu_dbg1.bits.tbu_dbg1 = tbu_dbg1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset), vdp1_smmu_tbu_dbg1.u32);

    return;
}

hi_void vdp_mmu_setwdummycnt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_dummy_cnt)
{
    u_vdp1_smmu_tbu_dbg1 vdp1_smmu_tbu_dbg1;

    vdp1_smmu_tbu_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset));
    vdp1_smmu_tbu_dbg1.bits.w_dummy_cnt = w_dummy_cnt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset), vdp1_smmu_tbu_dbg1.u32);

    return;
}

hi_void vdp_mmu_setwincnt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_in_cnt)
{
    u_vdp1_smmu_tbu_dbg1 vdp1_smmu_tbu_dbg1;

    vdp1_smmu_tbu_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset));
    vdp1_smmu_tbu_dbg1.bits.w_in_cnt = w_in_cnt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset), vdp1_smmu_tbu_dbg1.u32);

    return;
}

hi_void vdp_mmu_setwoutcnt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 w_out_cnt)
{
    u_vdp1_smmu_tbu_dbg1 vdp1_smmu_tbu_dbg1;

    vdp1_smmu_tbu_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset));
    vdp1_smmu_tbu_dbg1.bits.w_out_cnt = w_out_cnt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg1.u32) + offset), vdp1_smmu_tbu_dbg1.u32);

    return;
}

hi_void vdp_mmu_settbudbg2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tbu_dbg2)
{
    u_vdp1_smmu_tbu_dbg2 vdp1_smmu_tbu_dbg2;

    vdp1_smmu_tbu_dbg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg2.u32) + offset));
    vdp1_smmu_tbu_dbg2.bits.tbu_dbg2 = tbu_dbg2;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg2.u32) + offset), vdp1_smmu_tbu_dbg2.u32);

    return;
}

hi_void vdp_mmu_settbudbg3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tbu_dbg3)
{
    u_vdp1_smmu_tbu_dbg3 vdp1_smmu_tbu_dbg3;

    vdp1_smmu_tbu_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg3.u32) + offset));
    vdp1_smmu_tbu_dbg3.bits.tbu_dbg3 = tbu_dbg3;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_tbu_dbg3.u32) + offset), vdp1_smmu_tbu_dbg3.u32);

    return;
}

hi_void vdp_mmu_setinoutcmdcntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 in_out_cmd_cnt_rd)
{
    u_vdp1_smmu_master_dbg0 vdp1_smmu_master_dbg0;

    vdp1_smmu_master_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset));
    vdp1_smmu_master_dbg0.bits.in_out_cmd_cnt_rd = in_out_cmd_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset), vdp1_smmu_master_dbg0.u32);

    return;
}

hi_void vdp_mmu_setrdydebugrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rdy_debug_rd)
{
    u_vdp1_smmu_master_dbg0 vdp1_smmu_master_dbg0;

    vdp1_smmu_master_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset));
    vdp1_smmu_master_dbg0.bits.rdy_debug_rd = rdy_debug_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset), vdp1_smmu_master_dbg0.u32);

    return;
}

hi_void vdp_mmu_setvlddebugrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vld_debug_rd)
{
    u_vdp1_smmu_master_dbg0 vdp1_smmu_master_dbg0;

    vdp1_smmu_master_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset));
    vdp1_smmu_master_dbg0.bits.vld_debug_rd = vld_debug_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset), vdp1_smmu_master_dbg0.u32);

    return;
}

hi_void vdp_mmu_setcurmisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg0 vdp1_smmu_master_dbg0;

    vdp1_smmu_master_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset));
    vdp1_smmu_master_dbg0.bits.cur_miss_cnt_rd = cur_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset), vdp1_smmu_master_dbg0.u32);

    return;
}

hi_void vdp_mmu_setlastmisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg0 vdp1_smmu_master_dbg0;

    vdp1_smmu_master_dbg0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset));
    vdp1_smmu_master_dbg0.bits.last_miss_cnt_rd = last_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg0.u32) + offset), vdp1_smmu_master_dbg0.u32);

    return;
}

hi_void vdp_mmu_setinoutcmdcntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 in_out_cmd_cnt_wr)
{
    u_vdp1_smmu_master_dbg1 vdp1_smmu_master_dbg1;

    vdp1_smmu_master_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset));
    vdp1_smmu_master_dbg1.bits.in_out_cmd_cnt_wr = in_out_cmd_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset), vdp1_smmu_master_dbg1.u32);

    return;
}

hi_void vdp_mmu_setrdydebugwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rdy_debug_wr)
{
    u_vdp1_smmu_master_dbg1 vdp1_smmu_master_dbg1;

    vdp1_smmu_master_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset));
    vdp1_smmu_master_dbg1.bits.rdy_debug_wr = rdy_debug_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset), vdp1_smmu_master_dbg1.u32);

    return;
}

hi_void vdp_mmu_setvlddebugwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vld_debug_wr)
{
    u_vdp1_smmu_master_dbg1 vdp1_smmu_master_dbg1;

    vdp1_smmu_master_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset));
    vdp1_smmu_master_dbg1.bits.vld_debug_wr = vld_debug_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset), vdp1_smmu_master_dbg1.u32);

    return;
}

hi_void vdp_mmu_setcurmisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg1 vdp1_smmu_master_dbg1;

    vdp1_smmu_master_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset));
    vdp1_smmu_master_dbg1.bits.cur_miss_cnt_wr = cur_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset), vdp1_smmu_master_dbg1.u32);

    return;
}

hi_void vdp_mmu_setlastmisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg1 vdp1_smmu_master_dbg1;

    vdp1_smmu_master_dbg1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset));
    vdp1_smmu_master_dbg1.bits.last_miss_cnt_wr = last_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg1.u32) + offset), vdp1_smmu_master_dbg1.u32);

    return;
}

hi_void vdp_mmu_setcurdoubleupdcntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_double_upd_cnt_rd)
{
    u_vdp1_smmu_master_dbg2 vdp1_smmu_master_dbg2;

    vdp1_smmu_master_dbg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset));
    vdp1_smmu_master_dbg2.bits.cur_double_upd_cnt_rd = cur_double_upd_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset), vdp1_smmu_master_dbg2.u32);

    return;
}

hi_void vdp_mmu_setlastdoubleupdcntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_double_upd_cnt_rd)
{
    u_vdp1_smmu_master_dbg2 vdp1_smmu_master_dbg2;

    vdp1_smmu_master_dbg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset));
    vdp1_smmu_master_dbg2.bits.last_double_upd_cnt_rd = last_double_upd_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset), vdp1_smmu_master_dbg2.u32);

    return;
}

hi_void vdp_mmu_setcurdoublemisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_double_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg2 vdp1_smmu_master_dbg2;

    vdp1_smmu_master_dbg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset));
    vdp1_smmu_master_dbg2.bits.cur_double_miss_cnt_rd = cur_double_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset), vdp1_smmu_master_dbg2.u32);

    return;
}

hi_void vdp_mmu_setlastdoublemisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_double_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg2 vdp1_smmu_master_dbg2;

    vdp1_smmu_master_dbg2.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset));
    vdp1_smmu_master_dbg2.bits.last_double_miss_cnt_rd = last_double_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg2.u32) + offset), vdp1_smmu_master_dbg2.u32);

    return;
}

hi_void vdp_mmu_setmstfsmcur(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst_fsm_cur)
{
    u_vdp1_smmu_master_dbg3 vdp1_smmu_master_dbg3;

    vdp1_smmu_master_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset));
    vdp1_smmu_master_dbg3.bits.mst_fsm_cur = mst_fsm_cur;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset), vdp1_smmu_master_dbg3.u32);

    return;
}

hi_void vdp_mmu_setcurdoubleupdcntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_double_upd_cnt_wr)
{
    u_vdp1_smmu_master_dbg3 vdp1_smmu_master_dbg3;

    vdp1_smmu_master_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset));
    vdp1_smmu_master_dbg3.bits.cur_double_upd_cnt_wr = cur_double_upd_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset), vdp1_smmu_master_dbg3.u32);

    return;
}

hi_void vdp_mmu_setlastdoubleupdcntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_double_upd_cnt_wr)
{
    u_vdp1_smmu_master_dbg3 vdp1_smmu_master_dbg3;

    vdp1_smmu_master_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset));
    vdp1_smmu_master_dbg3.bits.last_double_upd_cnt_wr = last_double_upd_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset), vdp1_smmu_master_dbg3.u32);

    return;
}

hi_void vdp_mmu_setcurdoublemisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_double_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg3 vdp1_smmu_master_dbg3;

    vdp1_smmu_master_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset));
    vdp1_smmu_master_dbg3.bits.cur_double_miss_cnt_wr = cur_double_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset), vdp1_smmu_master_dbg3.u32);

    return;
}

hi_void vdp_mmu_setlastdoublemisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_double_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg3 vdp1_smmu_master_dbg3;

    vdp1_smmu_master_dbg3.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset));
    vdp1_smmu_master_dbg3.bits.last_double_miss_cnt_wr = last_double_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg3.u32) + offset), vdp1_smmu_master_dbg3.u32);

    return;
}

hi_void vdp_mmu_setcursel1chnmisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_sel1_chn_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg4 vdp1_smmu_master_dbg4;

    vdp1_smmu_master_dbg4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg4.u32) + offset));
    vdp1_smmu_master_dbg4.bits.cur_sel1_chn_miss_cnt_rd = cur_sel1_chn_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg4.u32) + offset), vdp1_smmu_master_dbg4.u32);

    return;
}

hi_void vdp_mmu_setlastsel1chnmisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_sel1_chn_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg4 vdp1_smmu_master_dbg4;

    vdp1_smmu_master_dbg4.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg4.u32) + offset));
    vdp1_smmu_master_dbg4.bits.last_sel1_chn_miss_cnt_rd = last_sel1_chn_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg4.u32) + offset), vdp1_smmu_master_dbg4.u32);

    return;
}

hi_void vdp_mmu_setcursel2chnmisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_sel2_chn_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg5 vdp1_smmu_master_dbg5;

    vdp1_smmu_master_dbg5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg5.u32) + offset));
    vdp1_smmu_master_dbg5.bits.cur_sel2_chn_miss_cnt_rd = cur_sel2_chn_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg5.u32) + offset), vdp1_smmu_master_dbg5.u32);

    return;
}

hi_void vdp_mmu_setlastsel2chnmisscntrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_sel2_chn_miss_cnt_rd)
{
    u_vdp1_smmu_master_dbg5 vdp1_smmu_master_dbg5;

    vdp1_smmu_master_dbg5.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg5.u32) + offset));
    vdp1_smmu_master_dbg5.bits.last_sel2_chn_miss_cnt_rd = last_sel2_chn_miss_cnt_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg5.u32) + offset), vdp1_smmu_master_dbg5.u32);

    return;
}

hi_void vdp_mmu_setcursel1chnmisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_sel1_chn_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg6 vdp1_smmu_master_dbg6;

    vdp1_smmu_master_dbg6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg6.u32) + offset));
    vdp1_smmu_master_dbg6.bits.cur_sel1_chn_miss_cnt_wr = cur_sel1_chn_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg6.u32) + offset), vdp1_smmu_master_dbg6.u32);

    return;
}

hi_void vdp_mmu_setlastsel1chnmisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_sel1_chn_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg6 vdp1_smmu_master_dbg6;

    vdp1_smmu_master_dbg6.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg6.u32) + offset));
    vdp1_smmu_master_dbg6.bits.last_sel1_chn_miss_cnt_wr = last_sel1_chn_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg6.u32) + offset), vdp1_smmu_master_dbg6.u32);

    return;
}

hi_void vdp_mmu_setcursel2chnmisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cur_sel2_chn_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg7 vdp1_smmu_master_dbg7;

    vdp1_smmu_master_dbg7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg7.u32) + offset));
    vdp1_smmu_master_dbg7.bits.cur_sel2_chn_miss_cnt_wr = cur_sel2_chn_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg7.u32) + offset), vdp1_smmu_master_dbg7.u32);

    return;
}

hi_void vdp_mmu_setlastsel2chnmisscntwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 last_sel2_chn_miss_cnt_wr)
{
    u_vdp1_smmu_master_dbg7 vdp1_smmu_master_dbg7;

    vdp1_smmu_master_dbg7.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg7.u32) + offset));
    vdp1_smmu_master_dbg7.bits.last_sel2_chn_miss_cnt_wr = last_sel2_chn_miss_cnt_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg7.u32) + offset), vdp1_smmu_master_dbg7.u32);

    return;
}

hi_void vdp_mmu_setsel1chnrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sel1_chn_rd)
{
    u_vdp1_smmu_master_dbg8 vdp1_smmu_master_dbg8;

    vdp1_smmu_master_dbg8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset));
    vdp1_smmu_master_dbg8.bits.sel1_chn_rd = sel1_chn_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset), vdp1_smmu_master_dbg8.u32);

    return;
}

hi_void vdp_mmu_setsel2chnrd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sel2_chn_rd)
{
    u_vdp1_smmu_master_dbg8 vdp1_smmu_master_dbg8;

    vdp1_smmu_master_dbg8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset));
    vdp1_smmu_master_dbg8.bits.sel2_chn_rd = sel2_chn_rd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset), vdp1_smmu_master_dbg8.u32);

    return;
}

hi_void vdp_mmu_setsel1chnwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sel1_chn_wr)
{
    u_vdp1_smmu_master_dbg8 vdp1_smmu_master_dbg8;

    vdp1_smmu_master_dbg8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset));
    vdp1_smmu_master_dbg8.bits.sel1_chn_wr = sel1_chn_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset), vdp1_smmu_master_dbg8.u32);

    return;
}

hi_void vdp_mmu_setsel2chnwr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 sel2_chn_wr)
{
    u_vdp1_smmu_master_dbg8 vdp1_smmu_master_dbg8;

    vdp1_smmu_master_dbg8.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset));
    vdp1_smmu_master_dbg8.bits.sel2_chn_wr = sel2_chn_wr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_master_dbg8.u32) + offset), vdp1_smmu_master_dbg8.u32);

    return;
}


hi_u32 vdp_mmu_get_error_addr_ns(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    return vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_fault_addr_rd_ns.u32) + offset));
}

hi_u32 vdp_mmu_get_error_state(vdp_regs_type *vdp_reg, hi_u32 offset)
{
    return vdp_regread((uintptr_t)(&(vdp_reg->vdp1_smmu_intraw_ns.u32) + offset));
}

hi_void vdp_mmu_clear_error_state(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 state)
{
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp1_smmu_intclr_ns.u32) + offset), state);
    return;
}
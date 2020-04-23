/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_fdr.h"
#include "hal_vdp_comm.h"

hi_void vdp_fdr_vid_setlmdrawmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_draw_mode)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.lm_draw_mode = lm_draw_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setchmdrawmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_draw_mode)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.chm_draw_mode = chm_draw_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgmutemode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mute_mode)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.mrg_mute_mode = mrg_mute_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setfdrckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fdr_ck_gt_en)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.fdr_ck_gt_en = fdr_ck_gt_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_enable)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.mrg_enable = mrg_enable;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setvicapmuteen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vicap_mute_en)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.vicap_mute_en = vicap_mute_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmutereqen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mute_req_en)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.mute_req_en = mute_req_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmuteen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mute_en)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.mute_en = mute_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setrmode3d(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rmode_3d)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.rmode_3d = rmode_3d;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setchmcopyen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_copy_en)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.chm_copy_en = chm_copy_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setflipen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 flip_en)
{
    u_vdp_core_v1_vid_read_ctrl vdp_core_v1_vid_read_ctrl;

    vdp_core_v1_vid_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset));
    vdp_core_v1_vid_read_ctrl.bits.flip_en = flip_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_ctrl.u32) + offset), vdp_core_v1_vid_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setprerden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 pre_rd_en)
{
    u_vdp_core_v1_vid_mac_ctrl vdp_core_v1_vid_mac_ctrl;

    vdp_core_v1_vid_mac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset));
    vdp_core_v1_vid_mac_ctrl.bits.pre_rd_en = pre_rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset), vdp_core_v1_vid_mac_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setreqldmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 req_ld_mode)
{
    u_vdp_core_v1_vid_mac_ctrl vdp_core_v1_vid_mac_ctrl;

    vdp_core_v1_vid_mac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset));
    vdp_core_v1_vid_mac_ctrl.bits.req_ld_mode = req_ld_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset), vdp_core_v1_vid_mac_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setreqinterval(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 req_interval)
{
    u_vdp_core_v1_vid_mac_ctrl vdp_core_v1_vid_mac_ctrl;

    vdp_core_v1_vid_mac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset));
    vdp_core_v1_vid_mac_ctrl.bits.req_interval = req_interval;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset), vdp_core_v1_vid_mac_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setoflmaster(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ofl_master)
{
    u_vdp_core_v1_vid_mac_ctrl vdp_core_v1_vid_mac_ctrl;

    vdp_core_v1_vid_mac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset));
    vdp_core_v1_vid_mac_ctrl.bits.ofl_master = ofl_master;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset), vdp_core_v1_vid_mac_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setreqlen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 req_len)
{
    u_vdp_core_v1_vid_mac_ctrl vdp_core_v1_vid_mac_ctrl;

    vdp_core_v1_vid_mac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset));
    vdp_core_v1_vid_mac_ctrl.bits.req_len = req_len;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset), vdp_core_v1_vid_mac_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setreqctrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 req_ctrl)
{
    u_vdp_core_v1_vid_mac_ctrl vdp_core_v1_vid_mac_ctrl;

    vdp_core_v1_vid_mac_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset));
    vdp_core_v1_vid_mac_ctrl.bits.req_ctrl = req_ctrl;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mac_ctrl.u32) + offset), vdp_core_v1_vid_mac_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setlmrmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_rmode)
{
    u_vdp_core_v1_vid_read_mode vdp_core_v1_vid_read_mode;

    vdp_core_v1_vid_read_mode.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_mode.u32) + offset));
    vdp_core_v1_vid_read_mode.bits.lm_rmode = lm_rmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_mode.u32) + offset), vdp_core_v1_vid_read_mode.u32);

    return;
}

hi_void vdp_fdr_vid_setchmrmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_rmode)
{
    u_vdp_core_v1_vid_read_mode vdp_core_v1_vid_read_mode;

    vdp_core_v1_vid_read_mode.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_mode.u32) + offset));
    vdp_core_v1_vid_read_mode.bits.chm_rmode = chm_rmode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_read_mode.u32) + offset), vdp_core_v1_vid_read_mode.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpfrmmaxen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_frm_max_en)
{
    u_vdp_core_v1_vid_out_ctrl vdp_core_v1_vid_out_ctrl;

    vdp_core_v1_vid_out_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset));
    vdp_core_v1_vid_out_ctrl.bits.dcmp_frm_max_en = dcmp_frm_max_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset), vdp_core_v1_vid_out_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_settestpatternen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 testpattern_en)
{
    u_vdp_core_v1_vid_out_ctrl vdp_core_v1_vid_out_ctrl;

    vdp_core_v1_vid_out_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset));
    vdp_core_v1_vid_out_ctrl.bits.testpattern_en = testpattern_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset), vdp_core_v1_vid_out_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setsingleportmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 single_port_mode)
{
    u_vdp_core_v1_vid_out_ctrl vdp_core_v1_vid_out_ctrl;

    vdp_core_v1_vid_out_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset));
    vdp_core_v1_vid_out_ctrl.bits.single_port_mode = single_port_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset), vdp_core_v1_vid_out_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setuvorderen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 uv_order_en)
{
    u_vdp_core_v1_vid_src_info vdp_core_v1_vid_src_info;

    vdp_core_v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset));
    vdp_core_v1_vid_src_info.bits.uv_order_en = uv_order_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset), vdp_core_v1_vid_src_info.u32);

    return;
}

hi_void vdp_fdr_vid_setdrawpixelmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 draw_pixel_mode)
{
    u_vdp_core_v1_vid_out_ctrl vdp_core_v1_vid_out_ctrl;

    vdp_core_v1_vid_out_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset));
    vdp_core_v1_vid_out_ctrl.bits.draw_pixel_mode = draw_pixel_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_out_ctrl.u32) + offset), vdp_core_v1_vid_out_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmutey(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mute_y)
{
    u_vdp_core_v1_vid_mute_bk vdp_core_v1_vid_mute_bk;

    vdp_core_v1_vid_mute_bk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mute_bk.u32) + offset));
    vdp_core_v1_vid_mute_bk.bits.mute_y = mute_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mute_bk.u32) + offset), vdp_core_v1_vid_mute_bk.u32);

    return;
}

hi_void vdp_fdr_vid_setmutecb(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mute_cb)
{
    u_vdp_core_v1_vid_mute_bk vdp_core_v1_vid_mute_bk;

    vdp_core_v1_vid_mute_bk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mute_bk.u32) + offset));
    vdp_core_v1_vid_mute_bk.bits.mute_cb = mute_cb;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mute_bk.u32) + offset), vdp_core_v1_vid_mute_bk.u32);

    return;
}

hi_void vdp_fdr_vid_setmutecr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mute_cr)
{
    u_vdp_core_v1_vid_mute_bk vdp_core_v1_vid_mute_bk;

    vdp_core_v1_vid_mute_bk.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mute_bk.u32) + offset));
    vdp_core_v1_vid_mute_bk.bits.mute_cr = mute_cr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mute_bk.u32) + offset), vdp_core_v1_vid_mute_bk.u32);

    return;
}

hi_void vdp_fdr_vid_setchmbypass3d(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_bypass_3d)
{
    u_vdp_core_v1_vid_smmu_bypass vdp_core_v1_vid_smmu_bypass;

    vdp_core_v1_vid_smmu_bypass.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) +
                                                   offset));
    vdp_core_v1_vid_smmu_bypass.bits.chm_bypass_3d = chm_bypass_3d;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) + offset),
                 vdp_core_v1_vid_smmu_bypass.u32);

    return;
}

hi_void vdp_fdr_vid_setlmbypass3d(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_bypass_3d)
{
    u_vdp_core_v1_vid_smmu_bypass vdp_core_v1_vid_smmu_bypass;

    vdp_core_v1_vid_smmu_bypass.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) +
                                                   offset));
    vdp_core_v1_vid_smmu_bypass.bits.lm_bypass_3d = lm_bypass_3d;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) + offset),
                 vdp_core_v1_vid_smmu_bypass.u32);

    return;
}

hi_void vdp_fdr_vid_setchmbypass2d(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_bypass_2d)
{
    u_vdp_core_v1_vid_smmu_bypass vdp_core_v1_vid_smmu_bypass;

    vdp_core_v1_vid_smmu_bypass.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) +
                                                   offset));
    vdp_core_v1_vid_smmu_bypass.bits.chm_bypass_2d = chm_bypass_2d;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) + offset),
                 vdp_core_v1_vid_smmu_bypass.u32);

    return;
}

hi_void vdp_fdr_vid_setlmbypass2d(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_bypass_2d)
{
    u_vdp_core_v1_vid_smmu_bypass vdp_core_v1_vid_smmu_bypass;

    vdp_core_v1_vid_smmu_bypass.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) +
                                                   offset));
    vdp_core_v1_vid_smmu_bypass.bits.lm_bypass_2d = lm_bypass_2d;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_smmu_bypass.u32) + offset),
                 vdp_core_v1_vid_smmu_bypass.u32);

    return;
}

hi_void vdp_fdr_vid_setyfrmsize(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 y_frm_size)
{
    u_vdp_core_v1_vid_src_lfsize vdp_core_v1_vid_src_lfsize;

    vdp_core_v1_vid_src_lfsize.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_lfsize.u32) +
                                                  offset));
    vdp_core_v1_vid_src_lfsize.bits.y_frm_size = y_frm_size;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_lfsize.u32) + offset),
                 vdp_core_v1_vid_src_lfsize.u32);

    return;
}

hi_void vdp_fdr_vid_setcfrmsize(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_frm_size)
{
    u_vdp_core_v1_vid_src_cfsize vdp_core_v1_vid_src_cfsize;

    vdp_core_v1_vid_src_cfsize.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_cfsize.u32) +
                                                  offset));
    vdp_core_v1_vid_src_cfsize.bits.c_frm_size = c_frm_size;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_cfsize.u32) + offset),
                 vdp_core_v1_vid_src_cfsize.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmptype(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_type)
{
    u_vdp_core_v1_vid_src_info vdp_core_v1_vid_src_info;

    vdp_core_v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset));
    vdp_core_v1_vid_src_info.bits.dcmp_type = dcmp_type;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset), vdp_core_v1_vid_src_info.u32);

    return;
}

hi_void vdp_fdr_vid_setdispmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 disp_mode)
{
    u_vdp_core_v1_vid_src_info vdp_core_v1_vid_src_info;

    vdp_core_v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset));
    vdp_core_v1_vid_src_info.bits.disp_mode = disp_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset), vdp_core_v1_vid_src_info.u32);

    return;
}

hi_void vdp_fdr_vid_setfieldtype(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 field_type)
{
    u_vdp_core_v1_vid_src_info vdp_core_v1_vid_src_info;

    vdp_core_v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset));
    vdp_core_v1_vid_src_info.bits.field_type = field_type;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset), vdp_core_v1_vid_src_info.u32);

    return;
}

hi_void vdp_fdr_vid_setdatawidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data_width)
{
    u_vdp_core_v1_vid_src_bitw vdp_core_v1_vid_src_bitw;

    vdp_core_v1_vid_src_bitw.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_bitw.u32) + offset));
    vdp_core_v1_vid_src_bitw.bits.data_width = data_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_bitw.u32) + offset), vdp_core_v1_vid_src_bitw.u32);

    return;
}

hi_void vdp_fdr_vid_setdatafmt(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data_fmt)
{
    u_vdp_core_v1_vid_src_info vdp_core_v1_vid_src_info;

    vdp_core_v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset));
    vdp_core_v1_vid_src_info.bits.data_fmt = data_fmt;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset), vdp_core_v1_vid_src_info.u32);

    return;
}

hi_void vdp_fdr_vid_setdatatype(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 data_type)
{
    u_vdp_core_v1_vid_src_info vdp_core_v1_vid_src_info;

    vdp_core_v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset));
    vdp_core_v1_vid_src_info.bits.data_type = data_type;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_info.u32) + offset), vdp_core_v1_vid_src_info.u32);

    return;
}

hi_void vdp_fdr_vid_setsrch(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src_h)
{
    u_vdp_core_v1_vid_src_reso vdp_core_v1_vid_src_reso;

    vdp_core_v1_vid_src_reso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_reso.u32) + offset));
    vdp_core_v1_vid_src_reso.bits.src_h = src_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_reso.u32) + offset), vdp_core_v1_vid_src_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setsrcw(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src_w)
{
    u_vdp_core_v1_vid_src_reso vdp_core_v1_vid_src_reso;

    vdp_core_v1_vid_src_reso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_reso.u32) + offset));
    vdp_core_v1_vid_src_reso.bits.src_w = src_w;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_reso.u32) + offset), vdp_core_v1_vid_src_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setsrccropy(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src_crop_y)
{
    u_vdp_core_v1_vid_src_crop vdp_core_v1_vid_src_crop;

    vdp_core_v1_vid_src_crop.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_crop.u32) + offset));
    vdp_core_v1_vid_src_crop.bits.src_crop_y = src_crop_y;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_crop.u32) + offset), vdp_core_v1_vid_src_crop.u32);

    return;
}

hi_void vdp_fdr_vid_setsrccropx(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src_crop_x)
{
    u_vdp_core_v1_vid_src_crop vdp_core_v1_vid_src_crop;

    vdp_core_v1_vid_src_crop.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_crop.u32) + offset));
    vdp_core_v1_vid_src_crop.bits.src_crop_x = src_crop_x;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_src_crop.u32) + offset), vdp_core_v1_vid_src_crop.u32);

    return;
}

hi_void vdp_fdr_vid_setiresoh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ireso_h)
{
    u_vdp_core_v1_vid_in_reso vdp_core_v1_vid_in_reso;

    vdp_core_v1_vid_in_reso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_reso.u32) + offset));
    vdp_core_v1_vid_in_reso.bits.ireso_h = ireso_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_reso.u32) + offset), vdp_core_v1_vid_in_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setiresow(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ireso_w)
{
    u_vdp_core_v1_vid_in_reso vdp_core_v1_vid_in_reso;

    vdp_core_v1_vid_in_reso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_reso.u32) + offset));
    vdp_core_v1_vid_in_reso.bits.ireso_w = ireso_w;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_reso.u32) + offset), vdp_core_v1_vid_in_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdaddr_h)
{
    u_vdp_core_v1_vid_addr_h vdp_core_v1_vid_addr_h;

    vdp_core_v1_vid_addr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_addr_h.u32) + offset));
    vdp_core_v1_vid_addr_h.bits.vhdaddr_h = vhdaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_addr_h.u32) + offset), vdp_core_v1_vid_addr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdaddr_l)
{
    u_vdp_core_v1_vid_addr_l vdp_core_v1_vid_addr_l;

    vdp_core_v1_vid_addr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_addr_l.u32) + offset));
    vdp_core_v1_vid_addr_l.bits.vhdaddr_l = vhdaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_addr_l.u32) + offset), vdp_core_v1_vid_addr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdcaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdcaddr_h)
{
    u_vdp_core_v1_vid_caddr_h vdp_core_v1_vid_caddr_h;

    vdp_core_v1_vid_caddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_caddr_h.u32) + offset));
    vdp_core_v1_vid_caddr_h.bits.vhdcaddr_h = vhdcaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_caddr_h.u32) + offset), vdp_core_v1_vid_caddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdcaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdcaddr_l)
{
    u_vdp_core_v1_vid_caddr_l vdp_core_v1_vid_caddr_l;

    vdp_core_v1_vid_caddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_caddr_l.u32) + offset));
    vdp_core_v1_vid_caddr_l.bits.vhdcaddr_l = vhdcaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_caddr_l.u32) + offset), vdp_core_v1_vid_caddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdnaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdnaddr_h)
{
    u_vdp_core_v1_vid_naddr_h vdp_core_v1_vid_naddr_h;

    vdp_core_v1_vid_naddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_naddr_h.u32) + offset));
    vdp_core_v1_vid_naddr_h.bits.vhdnaddr_h = vhdnaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_naddr_h.u32) + offset), vdp_core_v1_vid_naddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdnaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdnaddr_l)
{
    u_vdp_core_v1_vid_naddr_l vdp_core_v1_vid_naddr_l;

    vdp_core_v1_vid_naddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_naddr_l.u32) + offset));
    vdp_core_v1_vid_naddr_l.bits.vhdnaddr_l = vhdnaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_naddr_l.u32) + offset), vdp_core_v1_vid_naddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdncaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdncaddr_h)
{
    u_vdp_core_v1_vid_ncaddr_h vdp_core_v1_vid_ncaddr_h;

    vdp_core_v1_vid_ncaddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_ncaddr_h.u32) + offset));
    vdp_core_v1_vid_ncaddr_h.bits.vhdncaddr_h = vhdncaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_ncaddr_h.u32) + offset), vdp_core_v1_vid_ncaddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setvhdncaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdncaddr_l)
{
    u_vdp_core_v1_vid_ncaddr_l vdp_core_v1_vid_ncaddr_l;

    vdp_core_v1_vid_ncaddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_ncaddr_l.u32) + offset));
    vdp_core_v1_vid_ncaddr_l.bits.vhdncaddr_l = vhdncaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_ncaddr_l.u32) + offset), vdp_core_v1_vid_ncaddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_sethvhdaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 h_vhdaddr_h)
{
    u_vdp_core_v1_vid_head_addr_h vdp_core_v1_vid_head_addr_h;

    vdp_core_v1_vid_head_addr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_addr_h.u32) +
                                                   offset));
    vdp_core_v1_vid_head_addr_h.bits.h_vhdaddr_h = h_vhdaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_addr_h.u32) + offset),
                 vdp_core_v1_vid_head_addr_h.u32);

    return;
}

hi_void vdp_fdr_vid_sethvhdaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 h_vhdaddr_l)
{
    u_vdp_core_v1_vid_head_addr_l vdp_core_v1_vid_head_addr_l;

    vdp_core_v1_vid_head_addr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_addr_l.u32) +
                                                   offset));
    vdp_core_v1_vid_head_addr_l.bits.h_vhdaddr_l = h_vhdaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_addr_l.u32) + offset),
                 vdp_core_v1_vid_head_addr_l.u32);

    return;
}

hi_void vdp_fdr_vid_sethvhdcaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 h_vhdcaddr_h)
{
    u_vdp_core_v1_vid_head_caddr_h vdp_core_v1_vid_head_caddr_h;

    vdp_core_v1_vid_head_caddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_caddr_h.u32) +
                                                    offset));
    vdp_core_v1_vid_head_caddr_h.bits.h_vhdcaddr_h = h_vhdcaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_caddr_h.u32) + offset),
                 vdp_core_v1_vid_head_caddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_sethvhdcaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 h_vhdcaddr_l)
{
    u_vdp_core_v1_vid_head_caddr_l vdp_core_v1_vid_head_caddr_l;

    vdp_core_v1_vid_head_caddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_caddr_l.u32) +
                                                    offset));
    vdp_core_v1_vid_head_caddr_l.bits.h_vhdcaddr_l = h_vhdcaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_caddr_l.u32) + offset),
                 vdp_core_v1_vid_head_caddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdaddr_h)
{
    u_vdp_core_v1_vid_2bit_addr_h vdp_core_v1_vid_2bit_addr_h;

    vdp_core_v1_vid_2bit_addr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_addr_h.u32) +
                                                   offset));
    vdp_core_v1_vid_2bit_addr_h.bits.b_vhdaddr_h = b_vhdaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_addr_h.u32) + offset),
                 vdp_core_v1_vid_2bit_addr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdaddr_l)
{
    u_vdp_core_v1_vid_2bit_addr_l vdp_core_v1_vid_2bit_addr_l;

    vdp_core_v1_vid_2bit_addr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_addr_l.u32) +
                                                   offset));
    vdp_core_v1_vid_2bit_addr_l.bits.b_vhdaddr_l = b_vhdaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_addr_l.u32) + offset),
                 vdp_core_v1_vid_2bit_addr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdcaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdcaddr_h)
{
    u_vdp_core_v1_vid_2bit_caddr_h vdp_core_v1_vid_2bit_caddr_h;

    vdp_core_v1_vid_2bit_caddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_caddr_h.u32) +
                                                    offset));
    vdp_core_v1_vid_2bit_caddr_h.bits.b_vhdcaddr_h = b_vhdcaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_caddr_h.u32) + offset),
                 vdp_core_v1_vid_2bit_caddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdcaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdcaddr_l)
{
    u_vdp_core_v1_vid_2bit_caddr_l vdp_core_v1_vid_2bit_caddr_l;

    vdp_core_v1_vid_2bit_caddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_caddr_l.u32) +
                                                    offset));
    vdp_core_v1_vid_2bit_caddr_l.bits.b_vhdcaddr_l = b_vhdcaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_caddr_l.u32) + offset),
                 vdp_core_v1_vid_2bit_caddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setchmstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_stride)
{
    u_vdp_core_v1_vid_stride vdp_core_v1_vid_stride;

    vdp_core_v1_vid_stride.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_stride.u32) + offset));
    vdp_core_v1_vid_stride.bits.chm_stride = chm_stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_stride.u32) + offset), vdp_core_v1_vid_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setlmstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_stride)
{
    u_vdp_core_v1_vid_stride vdp_core_v1_vid_stride;

    vdp_core_v1_vid_stride.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_stride.u32) + offset));
    vdp_core_v1_vid_stride.bits.lm_stride = lm_stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_stride.u32) + offset), vdp_core_v1_vid_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setchmtilestride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_tile_stride)
{
    u_vdp_core_v1_vid_2bit_stride vdp_core_v1_vid_2bit_stride;

    vdp_core_v1_vid_2bit_stride.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_stride.u32) +
                                                   offset));
    vdp_core_v1_vid_2bit_stride.bits.chm_tile_stride = chm_tile_stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_stride.u32) + offset),
                 vdp_core_v1_vid_2bit_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setlmtilestride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_tile_stride)
{
    u_vdp_core_v1_vid_2bit_stride vdp_core_v1_vid_2bit_stride;

    vdp_core_v1_vid_2bit_stride.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_stride.u32) +
                                                   offset));
    vdp_core_v1_vid_2bit_stride.bits.lm_tile_stride = lm_tile_stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_stride.u32) + offset),
                 vdp_core_v1_vid_2bit_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setchmheadstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chm_head_stride)
{
    u_vdp_core_v1_vid_head_stride vdp_core_v1_vid_head_stride;

    vdp_core_v1_vid_head_stride.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_stride.u32) +
                                                   offset));
    vdp_core_v1_vid_head_stride.bits.chm_head_stride = chm_head_stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_stride.u32) + offset),
                 vdp_core_v1_vid_head_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setlmheadstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lm_head_stride)
{
    u_vdp_core_v1_vid_head_stride vdp_core_v1_vid_head_stride;

    vdp_core_v1_vid_head_stride.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_stride.u32) +
                                                   offset));
    vdp_core_v1_vid_head_stride.bits.lm_head_stride = lm_head_stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_head_stride.u32) + offset),
                 vdp_core_v1_vid_head_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdnaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdnaddr_h)
{
    u_vdp_core_v1_vid_2bit_naddr_h vdp_core_v1_vid_2bit_naddr_h;

    vdp_core_v1_vid_2bit_naddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_naddr_h.u32) +
                                                    offset));
    vdp_core_v1_vid_2bit_naddr_h.bits.b_vhdnaddr_h = b_vhdnaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_naddr_h.u32) + offset),
                 vdp_core_v1_vid_2bit_naddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdnaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdnaddr_l)
{
    u_vdp_core_v1_vid_2bit_naddr_l vdp_core_v1_vid_2bit_naddr_l;

    vdp_core_v1_vid_2bit_naddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_naddr_l.u32) +
                                                    offset));
    vdp_core_v1_vid_2bit_naddr_l.bits.b_vhdnaddr_l = b_vhdnaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_naddr_l.u32) + offset),
                 vdp_core_v1_vid_2bit_naddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdncaddrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdncaddr_h)
{
    u_vdp_core_v1_vid_2bit_ncaddr_h vdp_core_v1_vid_2bit_ncaddr_h;

    vdp_core_v1_vid_2bit_ncaddr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_ncaddr_h.u32) +
                                                     offset));
    vdp_core_v1_vid_2bit_ncaddr_h.bits.b_vhdncaddr_h = b_vhdncaddr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_ncaddr_h.u32) + offset),
                 vdp_core_v1_vid_2bit_ncaddr_h.u32);

    return;
}

hi_void vdp_fdr_vid_setbvhdncaddrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 b_vhdncaddr_l)
{
    u_vdp_core_v1_vid_2bit_ncaddr_l vdp_core_v1_vid_2bit_ncaddr_l;

    vdp_core_v1_vid_2bit_ncaddr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_ncaddr_l.u32) +
                                                     offset));
    vdp_core_v1_vid_2bit_ncaddr_l.bits.b_vhdncaddr_l = b_vhdncaddr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_2bit_ncaddr_l.u32) + offset),
                 vdp_core_v1_vid_2bit_ncaddr_l.u32);

    return;
}

hi_void vdp_fdr_vid_settpmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tp_mode)
{
    u_vdp_core_v1_vid_testpat_cfg vdp_core_v1_vid_testpat_cfg;

    vdp_core_v1_vid_testpat_cfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) +
                                                   offset));
    vdp_core_v1_vid_testpat_cfg.bits.tp_mode = tp_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) + offset),
                 vdp_core_v1_vid_testpat_cfg.u32);

    return;
}

hi_void vdp_fdr_vid_settpcolormode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tp_color_mode)
{
    u_vdp_core_v1_vid_testpat_cfg vdp_core_v1_vid_testpat_cfg;

    vdp_core_v1_vid_testpat_cfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) +
                                                   offset));
    vdp_core_v1_vid_testpat_cfg.bits.tp_color_mode = tp_color_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) + offset),
                 vdp_core_v1_vid_testpat_cfg.u32);

    return;
}

hi_void vdp_fdr_vid_settplinew(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tp_line_w)
{
    u_vdp_core_v1_vid_testpat_cfg vdp_core_v1_vid_testpat_cfg;

    vdp_core_v1_vid_testpat_cfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) +
                                                   offset));
    vdp_core_v1_vid_testpat_cfg.bits.tp_line_w = tp_line_w;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) + offset),
                 vdp_core_v1_vid_testpat_cfg.u32);

    return;
}

hi_void vdp_fdr_vid_settpspeed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tp_speed)
{
    u_vdp_core_v1_vid_testpat_cfg vdp_core_v1_vid_testpat_cfg;

    vdp_core_v1_vid_testpat_cfg.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) +
                                                   offset));
    vdp_core_v1_vid_testpat_cfg.bits.tp_speed = tp_speed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_cfg.u32) + offset),
                 vdp_core_v1_vid_testpat_cfg.u32);

    return;
}

hi_void vdp_fdr_vid_settpseed(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tp_seed)
{
    u_vdp_core_v1_vid_testpat_seed vdp_core_v1_vid_testpat_seed;

    vdp_core_v1_vid_testpat_seed.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_seed.u32) +
                                                    offset));
    vdp_core_v1_vid_testpat_seed.bits.tp_seed = tp_seed;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_testpat_seed.u32) + offset),
                 vdp_core_v1_vid_testpat_seed.u32);

    return;
}

hi_void vdp_fdr_vid_setfdrpressen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 fdr_press_en)
{
    u_vdp_core_v1_vid_debug_ctrl vdp_core_v1_vid_debug_ctrl;

    vdp_core_v1_vid_debug_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_debug_ctrl.u32) +
                                                  offset));
    vdp_core_v1_vid_debug_ctrl.bits.fdr_press_en = fdr_press_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_debug_ctrl.u32) + offset),
                 vdp_core_v1_vid_debug_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setchecksumen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 checksum_en)
{
    u_vdp_core_v1_vid_debug_ctrl vdp_core_v1_vid_debug_ctrl;

    vdp_core_v1_vid_debug_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_debug_ctrl.u32) +
                                                  offset));
    vdp_core_v1_vid_debug_ctrl.bits.checksum_en = checksum_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_debug_ctrl.u32) + offset),
                 vdp_core_v1_vid_debug_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setscopedrdynum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scope_drdy_num)
{
    u_vdp_core_v1_vid_press_ctrl vdp_core_v1_vid_press_ctrl;

    vdp_core_v1_vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) +
                                                  offset));
    vdp_core_v1_vid_press_ctrl.bits.scope_drdy_num = scope_drdy_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) + offset),
                 vdp_core_v1_vid_press_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmindrdynum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_drdy_num)
{
    u_vdp_core_v1_vid_press_ctrl vdp_core_v1_vid_press_ctrl;

    vdp_core_v1_vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) +
                                                  offset));
    vdp_core_v1_vid_press_ctrl.bits.min_drdy_num = min_drdy_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) + offset),
                 vdp_core_v1_vid_press_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setscopenodrdynum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 scope_nodrdy_num)
{
    u_vdp_core_v1_vid_press_ctrl vdp_core_v1_vid_press_ctrl;

    vdp_core_v1_vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) +
                                                  offset));
    vdp_core_v1_vid_press_ctrl.bits.scope_nodrdy_num = scope_nodrdy_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) + offset),
                 vdp_core_v1_vid_press_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setminnodrdynum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 min_nodrdy_num)
{
    u_vdp_core_v1_vid_press_ctrl vdp_core_v1_vid_press_ctrl;

    vdp_core_v1_vid_press_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) +
                                                  offset));
    vdp_core_v1_vid_press_ctrl.bits.min_nodrdy_num = min_nodrdy_num;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_press_ctrl.u32) + offset),
                 vdp_core_v1_vid_press_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_settunlthd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tunl_thd)
{
    u_vdp_core_v1_vid_tunl_ctrl vdp_core_v1_vid_tunl_ctrl;

    vdp_core_v1_vid_tunl_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_ctrl.u32) + offset));
    vdp_core_v1_vid_tunl_ctrl.bits.tunl_thd = tunl_thd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_ctrl.u32) + offset), vdp_core_v1_vid_tunl_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_settunlinterval(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tunl_interval)
{
    u_vdp_core_v1_vid_tunl_ctrl vdp_core_v1_vid_tunl_ctrl;

    vdp_core_v1_vid_tunl_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_ctrl.u32) + offset));
    vdp_core_v1_vid_tunl_ctrl.bits.tunl_interval = tunl_interval;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_ctrl.u32) + offset), vdp_core_v1_vid_tunl_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_settunladdrh(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tunl_addr_h)
{
    u_vdp_core_v1_vid_tunl_addr_h vdp_core_v1_vid_tunl_addr_h;

    vdp_core_v1_vid_tunl_addr_h.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_addr_h.u32) +
                                                   offset));
    vdp_core_v1_vid_tunl_addr_h.bits.tunl_addr_h = tunl_addr_h;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_addr_h.u32) + offset),
                 vdp_core_v1_vid_tunl_addr_h.u32);

    return;
}

hi_void vdp_fdr_vid_settunladdrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tunl_addr_l)
{
    u_vdp_core_v1_vid_tunl_addr_l vdp_core_v1_vid_tunl_addr_l;

    vdp_core_v1_vid_tunl_addr_l.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_addr_l.u32) +
                                                   offset));
    vdp_core_v1_vid_tunl_addr_l.bits.tunl_addr_l = tunl_addr_l;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_tunl_addr_l.u32) + offset),
                 vdp_core_v1_vid_tunl_addr_l.u32);

    return;
}

hi_void vdp_fdr_vid_setlchecksum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_checksum0)
{
    u_vdp_core_v1_vid_in_l_checksum0 vdp_core_v1_vid_in_l_checksum0;

    vdp_core_v1_vid_in_l_checksum0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_l_checksum0.u32) +
                                                      offset));
    vdp_core_v1_vid_in_l_checksum0.bits.l_checksum0 = l_checksum0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_l_checksum0.u32) + offset),
                 vdp_core_v1_vid_in_l_checksum0.u32);

    return;
}

hi_void vdp_fdr_vid_setlchecksum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_checksum1)
{
    u_vdp_core_v1_vid_in_l_checksum1 vdp_core_v1_vid_in_l_checksum1;

    vdp_core_v1_vid_in_l_checksum1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_l_checksum1.u32) +
                                                      offset));
    vdp_core_v1_vid_in_l_checksum1.bits.l_checksum1 = l_checksum1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_l_checksum1.u32) + offset),
                 vdp_core_v1_vid_in_l_checksum1.u32);

    return;
}

hi_void vdp_fdr_vid_setcchecksum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_checksum0)
{
    u_vdp_core_v1_vid_in_c_checksum0 vdp_core_v1_vid_in_c_checksum0;

    vdp_core_v1_vid_in_c_checksum0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_c_checksum0.u32) +
                                                      offset));
    vdp_core_v1_vid_in_c_checksum0.bits.c_checksum0 = c_checksum0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_c_checksum0.u32) + offset),
                 vdp_core_v1_vid_in_c_checksum0.u32);

    return;
}

hi_void vdp_fdr_vid_setcchecksum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_checksum1)
{
    u_vdp_core_v1_vid_in_c_checksum1 vdp_core_v1_vid_in_c_checksum1;

    vdp_core_v1_vid_in_c_checksum1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_c_checksum1.u32) +
                                                      offset));
    vdp_core_v1_vid_in_c_checksum1.bits.c_checksum1 = c_checksum1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_in_c_checksum1.u32) + offset),
                 vdp_core_v1_vid_in_c_checksum1.u32);

    return;
}

hi_void vdp_fdr_vid_setlfifosta0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_fifosta0)
{
    u_vdp_core_v1_vid_l_fifosta0 vdp_core_v1_vid_l_fifosta0;

    vdp_core_v1_vid_l_fifosta0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_fifosta0.u32) +
                                                  offset));
    vdp_core_v1_vid_l_fifosta0.bits.l_fifosta0 = l_fifosta0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_fifosta0.u32) + offset),
                 vdp_core_v1_vid_l_fifosta0.u32);

    return;
}

hi_void vdp_fdr_vid_setlfifototal(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_fifo_total)
{
    u_vdp_core_v1_vid_l_fifosta1 vdp_core_v1_vid_l_fifosta1;

    vdp_core_v1_vid_l_fifosta1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_fifosta1.u32) +
                                                  offset));
    vdp_core_v1_vid_l_fifosta1.bits.l_fifo_total = l_fifo_total;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_fifosta1.u32) + offset),
                 vdp_core_v1_vid_l_fifosta1.u32);

    return;
}

hi_void vdp_fdr_vid_setcfifosta0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_fifosta0)
{
    u_vdp_core_v1_vid_c_fifosta0 vdp_core_v1_vid_c_fifosta0;

    vdp_core_v1_vid_c_fifosta0.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_fifosta0.u32) +
                                                  offset));
    vdp_core_v1_vid_c_fifosta0.bits.c_fifosta0 = c_fifosta0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_fifosta0.u32) + offset),
                 vdp_core_v1_vid_c_fifosta0.u32);

    return;
}

hi_void vdp_fdr_vid_setcfifototal(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_fifo_total)
{
    u_vdp_core_v1_vid_c_fifosta1 vdp_core_v1_vid_c_fifosta1;

    vdp_core_v1_vid_c_fifosta1.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_fifosta1.u32) +
                                                  offset));
    vdp_core_v1_vid_c_fifosta1.bits.c_fifo_total = c_fifo_total;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_fifosta1.u32) + offset),
                 vdp_core_v1_vid_c_fifosta1.u32);

    return;
}

hi_void vdp_fdr_vid_setlcurflow(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_cur_flow)
{
    u_vdp_core_v1_vid_l_cur_flow vdp_core_v1_vid_l_cur_flow;

    vdp_core_v1_vid_l_cur_flow.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_cur_flow.u32) +
                                                  offset));
    vdp_core_v1_vid_l_cur_flow.bits.l_cur_flow = l_cur_flow;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_cur_flow.u32) + offset),
                 vdp_core_v1_vid_l_cur_flow.u32);

    return;
}

hi_void vdp_fdr_vid_setlcursreqtime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_cur_sreq_time)
{
    u_vdp_core_v1_vid_l_cur_sreq_time vdp_core_v1_vid_l_cur_sreq_time;

    vdp_core_v1_vid_l_cur_sreq_time.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_cur_sreq_time.u32)
                                                       + offset));
    vdp_core_v1_vid_l_cur_sreq_time.bits.l_cur_sreq_time = l_cur_sreq_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_cur_sreq_time.u32) + offset),
                 vdp_core_v1_vid_l_cur_sreq_time.u32);

    return;
}

hi_void vdp_fdr_vid_setccurflow(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_cur_flow)
{
    u_vdp_core_v1_vid_c_cur_flow vdp_core_v1_vid_c_cur_flow;

    vdp_core_v1_vid_c_cur_flow.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_cur_flow.u32) +
                                                  offset));
    vdp_core_v1_vid_c_cur_flow.bits.c_cur_flow = c_cur_flow;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_cur_flow.u32) + offset),
                 vdp_core_v1_vid_c_cur_flow.u32);

    return;
}

hi_void vdp_fdr_vid_setccursreqtime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_cur_sreq_time)
{
    u_vdp_core_v1_vid_c_cur_sreq_time vdp_core_v1_vid_c_cur_sreq_time;

    vdp_core_v1_vid_c_cur_sreq_time.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_cur_sreq_time.u32)
                                                       + offset));
    vdp_core_v1_vid_c_cur_sreq_time.bits.c_cur_sreq_time = c_cur_sreq_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_cur_sreq_time.u32) + offset),
                 vdp_core_v1_vid_c_cur_sreq_time.u32);

    return;
}

hi_void vdp_fdr_vid_setllastflow(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_last_flow)
{
    u_vdp_core_v1_vid_l_last_flow vdp_core_v1_vid_l_last_flow;

    vdp_core_v1_vid_l_last_flow.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_last_flow.u32) +
                                                   offset));
    vdp_core_v1_vid_l_last_flow.bits.l_last_flow = l_last_flow;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_last_flow.u32) + offset),
                 vdp_core_v1_vid_l_last_flow.u32);

    return;
}

hi_void vdp_fdr_vid_setllastsreqtime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_last_sreq_time)
{
    u_vdp_core_v1_vid_l_last_sreq_time vdp_core_v1_vid_l_last_sreq_time;

    vdp_core_v1_vid_l_last_sreq_time.u32 = vdp_regread((uintptr_t)(&
                                                        (vdp_reg->vdp_core_v1_vid_l_last_sreq_time.u32) + offset));
    vdp_core_v1_vid_l_last_sreq_time.bits.l_last_sreq_time = l_last_sreq_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_last_sreq_time.u32) + offset),
                 vdp_core_v1_vid_l_last_sreq_time.u32);

    return;
}

hi_void vdp_fdr_vid_setclastflow(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_last_flow)
{
    u_vdp_core_v1_vid_c_last_flow vdp_core_v1_vid_c_last_flow;

    vdp_core_v1_vid_c_last_flow.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_last_flow.u32) +
                                                   offset));
    vdp_core_v1_vid_c_last_flow.bits.c_last_flow = c_last_flow;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_last_flow.u32) + offset),
                 vdp_core_v1_vid_c_last_flow.u32);

    return;
}

hi_void vdp_fdr_vid_setclastsreqtime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_last_sreq_time)
{
    u_vdp_core_v1_vid_c_last_sreq_time vdp_core_v1_vid_c_last_sreq_time;

    vdp_core_v1_vid_c_last_sreq_time.u32 = vdp_regread((uintptr_t)(&
                                                        (vdp_reg->vdp_core_v1_vid_c_last_sreq_time.u32) + offset));
    vdp_core_v1_vid_c_last_sreq_time.bits.c_last_sreq_time = c_last_sreq_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_last_sreq_time.u32) + offset),
                 vdp_core_v1_vid_c_last_sreq_time.u32);

    return;
}

hi_void vdp_fdr_vid_setoutheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_height)
{
    u_vdp_core_v1_vid_work_reso vdp_core_v1_vid_work_reso;

    vdp_core_v1_vid_work_reso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_reso.u32) + offset));
    vdp_core_v1_vid_work_reso.bits.out_height = out_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_reso.u32) + offset), vdp_core_v1_vid_work_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setoutwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_width)
{
    u_vdp_core_v1_vid_work_reso vdp_core_v1_vid_work_reso;

    vdp_core_v1_vid_work_reso.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_reso.u32) + offset));
    vdp_core_v1_vid_work_reso.bits.out_width = out_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_reso.u32) + offset), vdp_core_v1_vid_work_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setoutfinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 out_finfo)
{
    u_vdp_core_v1_vid_work_finfo vdp_core_v1_vid_work_finfo;

    vdp_core_v1_vid_work_finfo.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_finfo.u32) + offset));
    vdp_core_v1_vid_work_finfo.bits.out_finfo = out_finfo;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_finfo.u32) + offset),
                 vdp_core_v1_vid_work_finfo.u32);

    return;
}

hi_void vdp_fdr_vid_setylastaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 y_last_addr)
{
    u_vdp_core_v1_vid_last_yaddr vdp_core_v1_vid_last_yaddr;

    vdp_core_v1_vid_last_yaddr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_last_yaddr.u32) + offset));
    vdp_core_v1_vid_last_yaddr.bits.y_last_addr = y_last_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_last_yaddr.u32) + offset),
                 vdp_core_v1_vid_last_yaddr.u32);

    return;
}

hi_void vdp_fdr_vid_setclastcaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_last_caddr)
{
    u_vdp_core_v1_vid_last_caddr vdp_core_v1_vid_last_caddr;

    vdp_core_v1_vid_last_caddr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_last_caddr.u32) + offset));
    vdp_core_v1_vid_last_caddr.bits.c_last_caddr = c_last_caddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_last_caddr.u32) + offset),
                 vdp_core_v1_vid_last_caddr.u32);

    return;
}

hi_void vdp_fdr_vid_setworkreadctrl(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 work_read_ctrl)
{
    u_vdp_core_v1_vid_work_read_ctrl vdp_core_v1_vid_work_read_ctrl;

    vdp_core_v1_vid_work_read_ctrl.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_read_ctrl.u32) + offset));
    vdp_core_v1_vid_work_read_ctrl.bits.work_read_ctrl = work_read_ctrl;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_read_ctrl.u32) + offset),
                 vdp_core_v1_vid_work_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setworksmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 work_smmu_bypass)
{
    u_vdp_core_v1_vid_work_smmu_ctrl vdp_core_v1_vid_work_smmu_ctrl;

    vdp_core_v1_vid_work_smmu_ctrl.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_smmu_ctrl.u32) + offset));
    vdp_core_v1_vid_work_smmu_ctrl.bits.work_smmu_bypass = work_smmu_bypass;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_smmu_ctrl.u32) + offset),
                 vdp_core_v1_vid_work_smmu_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setworkyaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 work_y_addr)
{
    u_vdp_core_v1_vid_work_y_addr vdp_core_v1_vid_work_y_addr;

    vdp_core_v1_vid_work_y_addr.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_y_addr.u32) + offset));
    vdp_core_v1_vid_work_y_addr.bits.work_y_addr = work_y_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_y_addr.u32) + offset),
                 vdp_core_v1_vid_work_y_addr.u32);

    return;
}

hi_void vdp_fdr_vid_setworkcaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 work_c_addr)
{
    u_vdp_core_v1_vid_work_c_addr vdp_core_v1_vid_work_c_addr;

    vdp_core_v1_vid_work_c_addr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_c_addr.u32) + offset));
    vdp_core_v1_vid_work_c_addr.bits.work_c_addr = work_c_addr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_c_addr.u32) + offset),
                 vdp_core_v1_vid_work_c_addr.u32);

    return;
}

hi_void vdp_fdr_vid_setworkynaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 work_y_naddr)
{
    u_vdp_core_v1_vid_work_y_naddr vdp_core_v1_vid_work_y_naddr;

    vdp_core_v1_vid_work_y_naddr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_y_naddr.u32) + offset));
    vdp_core_v1_vid_work_y_naddr.bits.work_y_naddr = work_y_naddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_y_naddr.u32) + offset),
                 vdp_core_v1_vid_work_y_naddr.u32);

    return;
}

hi_void vdp_fdr_vid_setworkcnaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 work_c_naddr)
{
    u_vdp_core_v1_vid_work_c_naddr vdp_core_v1_vid_work_c_naddr;

    vdp_core_v1_vid_work_c_naddr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_c_naddr.u32) + offset));
    vdp_core_v1_vid_work_c_naddr.bits.work_c_naddr = work_c_naddr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_work_c_naddr.u32) + offset),
                 vdp_core_v1_vid_work_c_naddr.u32);

    return;
}

hi_void vdp_fdr_vid_setlbusytime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_busy_time)
{
    u_vdp_core_v1_vid_l_busy_time vdp_core_v1_vid_l_busy_time;

    vdp_core_v1_vid_l_busy_time.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_busy_time.u32) + offset));
    vdp_core_v1_vid_l_busy_time.bits.l_busy_time = l_busy_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_busy_time.u32) + offset),
                 vdp_core_v1_vid_l_busy_time.u32);

    return;
}

hi_void vdp_fdr_vid_setlneednordytime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l_neednordy_time)
{
    u_vdp_core_v1_vid_l_neednordy_time vdp_core_v1_vid_l_neednordy_time;

    vdp_core_v1_vid_l_neednordy_time.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_neednordy_time.u32) + offset));
    vdp_core_v1_vid_l_neednordy_time.bits.l_neednordy_time = l_neednordy_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l_neednordy_time.u32) + offset),
                 vdp_core_v1_vid_l_neednordy_time.u32);

    return;
}

hi_void vdp_fdr_vid_setl2neednordytime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 l2_neednordy_time)
{
    u_vdp_core_v1_vid_l2_neednordy_time vdp_core_v1_vid_l2_neednordy_time;

    vdp_core_v1_vid_l2_neednordy_time.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l2_neednordy_time.u32) + offset));
    vdp_core_v1_vid_l2_neednordy_time.bits.l2_neednordy_time = l2_neednordy_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_l2_neednordy_time.u32) + offset),
                 vdp_core_v1_vid_l2_neednordy_time.u32);

    return;
}

hi_void vdp_fdr_vid_setcbusytime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_busy_time)
{
    u_vdp_core_v1_vid_c_busy_time vdp_core_v1_vid_c_busy_time;

    vdp_core_v1_vid_c_busy_time.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_busy_time.u32) + offset));
    vdp_core_v1_vid_c_busy_time.bits.c_busy_time = c_busy_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_busy_time.u32) + offset),
                 vdp_core_v1_vid_c_busy_time.u32);

    return;
}

hi_void vdp_fdr_vid_setcneednordytime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c_neednordy_time)
{
    u_vdp_core_v1_vid_c_neednordy_time vdp_core_v1_vid_c_neednordy_time;

    vdp_core_v1_vid_c_neednordy_time.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_neednordy_time.u32) + offset));
    vdp_core_v1_vid_c_neednordy_time.bits.c_neednordy_time = c_neednordy_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c_neednordy_time.u32) + offset),
                 vdp_core_v1_vid_c_neednordy_time.u32);

    return;
}

hi_void vdp_fdr_vid_setc2neednordytime(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 c2_neednordy_time)
{
    u_vdp_core_v1_vid_c2_neednordy_time vdp_core_v1_vid_c2_neednordy_time;

    vdp_core_v1_vid_c2_neednordy_time.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c2_neednordy_time.u32) + offset));
    vdp_core_v1_vid_c2_neednordy_time.bits.c2_neednordy_time = c2_neednordy_time;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_c2_neednordy_time.u32) + offset),
                 vdp_core_v1_vid_c2_neednordy_time.u32);

    return;
}

hi_void vdp_fdr_vid_setstridelum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stride)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.stride = stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setbitdepthlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bit_depth)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.bit_depth = bit_depth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setstrideenlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stride_en)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.stride_en = stride_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setmaxmbqplum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 max_mb_qp)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.max_mb_qp = max_mb_qp;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setchromaenlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chroma_en)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.chroma_en = chroma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setcmpmodelum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmp_mode)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.cmp_mode = cmp_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setislosslesslum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 is_lossless)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.is_lossless = is_lossless;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_seticeenlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ice_en)
{
    u_vdp_core_v1_vid_line_ldcmp_glb_info vdp_core_v1_vid_line_ldcmp_glb_info;

    vdp_core_v1_vid_line_ldcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_glb_info.bits.ice_en = ice_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setframeheightlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 frame_height)
{
    u_vdp_core_v1_vid_line_ldcmp_frame_size vdp_core_v1_vid_line_ldcmp_frame_size;

    vdp_core_v1_vid_line_ldcmp_frame_size.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_frame_size.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_frame_size.bits.frame_height = frame_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_frame_size.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_frame_size.u32);

    return;
}

hi_void vdp_fdr_vid_setframewidthlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 frame_width)
{
    u_vdp_core_v1_vid_line_ldcmp_frame_size vdp_core_v1_vid_line_ldcmp_frame_size;

    vdp_core_v1_vid_line_ldcmp_frame_size.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_frame_size.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_frame_size.bits.frame_width = frame_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_frame_size.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_frame_size.u32);

    return;
}

hi_void vdp_fdr_vid_setadpqpthr0lum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 adpqp_thr0)
{
    u_vdp_core_v1_vid_line_ldcmp_adpqp_thr0 vdp_core_v1_vid_line_ldcmp_adpqp_thr0;

    vdp_core_v1_vid_line_ldcmp_adpqp_thr0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_adpqp_thr0.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_adpqp_thr0.bits.adpqp_thr0 = adpqp_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_adpqp_thr0.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_adpqp_thr0.u32);

    return;
}

hi_void vdp_fdr_vid_setadpqpthr1lum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 adpqp_thr1)
{
    u_vdp_core_v1_vid_line_ldcmp_adpqp_thr1 vdp_core_v1_vid_line_ldcmp_adpqp_thr1;

    vdp_core_v1_vid_line_ldcmp_adpqp_thr1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_adpqp_thr1.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_adpqp_thr1.bits.adpqp_thr1 = adpqp_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_adpqp_thr1.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_adpqp_thr1.u32);

    return;
}

hi_void vdp_fdr_vid_setsmoothdeltabitsthrlum(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 smooth_deltabits_thr)
{
    u_vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr;

    vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr.bits.smooth_deltabits_thr = smooth_deltabits_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_smth_deltabits_thr.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumelum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta0 vdp_core_v1_vid_line_ldcmp_error_sta0;

    vdp_core_v1_vid_line_ldcmp_error_sta0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta0.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta0.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivelum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta0 vdp_core_v1_vid_line_ldcmp_error_sta0;

    vdp_core_v1_vid_line_ldcmp_error_sta0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta0.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta0.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorlum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta0 vdp_core_v1_vid_line_ldcmp_error_sta0;

    vdp_core_v1_vid_line_ldcmp_error_sta0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta0.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta0.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbglum0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_ldcmp_dbg_reg0 vdp_core_v1_vid_line_ldcmp_dbg_reg0;

    vdp_core_v1_vid_line_ldcmp_dbg_reg0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg0.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_dbg_reg0.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg0.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_dbg_reg0.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumelum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta1 vdp_core_v1_vid_line_ldcmp_error_sta1;

    vdp_core_v1_vid_line_ldcmp_error_sta1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta1.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta1.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivelum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta1 vdp_core_v1_vid_line_ldcmp_error_sta1;

    vdp_core_v1_vid_line_ldcmp_error_sta1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta1.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta1.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorlum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta1 vdp_core_v1_vid_line_ldcmp_error_sta1;

    vdp_core_v1_vid_line_ldcmp_error_sta1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta1.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta1.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbglum1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_ldcmp_dbg_reg1 vdp_core_v1_vid_line_ldcmp_dbg_reg1;

    vdp_core_v1_vid_line_ldcmp_dbg_reg1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg1.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_dbg_reg1.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg1.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_dbg_reg1.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumelum2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta2 vdp_core_v1_vid_line_ldcmp_error_sta2;

    vdp_core_v1_vid_line_ldcmp_error_sta2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta2.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta2.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivelum2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta2 vdp_core_v1_vid_line_ldcmp_error_sta2;

    vdp_core_v1_vid_line_ldcmp_error_sta2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta2.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta2.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorlum2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta2 vdp_core_v1_vid_line_ldcmp_error_sta2;

    vdp_core_v1_vid_line_ldcmp_error_sta2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta2.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta2.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbglum2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_ldcmp_dbg_reg2 vdp_core_v1_vid_line_ldcmp_dbg_reg2;

    vdp_core_v1_vid_line_ldcmp_dbg_reg2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg2.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_dbg_reg2.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg2.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_dbg_reg2.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumelum3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta3 vdp_core_v1_vid_line_ldcmp_error_sta3;

    vdp_core_v1_vid_line_ldcmp_error_sta3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta3.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta3.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivelum3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta3 vdp_core_v1_vid_line_ldcmp_error_sta3;

    vdp_core_v1_vid_line_ldcmp_error_sta3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta3.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta3.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorlum3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_ldcmp_error_sta3 vdp_core_v1_vid_line_ldcmp_error_sta3;

    vdp_core_v1_vid_line_ldcmp_error_sta3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_error_sta3.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_error_sta3.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbglum3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_ldcmp_dbg_reg3 vdp_core_v1_vid_line_ldcmp_dbg_reg3;

    vdp_core_v1_vid_line_ldcmp_dbg_reg3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg3.u32) + offset));
    vdp_core_v1_vid_line_ldcmp_dbg_reg3.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_ldcmp_dbg_reg3.u32) + offset),
                 vdp_core_v1_vid_line_ldcmp_dbg_reg3.u32);

    return;
}

hi_void vdp_fdr_vid_setstridechm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stride)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.stride = stride;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setbitdepthchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bit_depth)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.bit_depth = bit_depth;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setstrideenchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stride_en)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.stride_en = stride_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setmaxmbqpchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 max_mb_qp)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.max_mb_qp = max_mb_qp;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setchromaenchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chroma_en)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.chroma_en = chroma_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setcmpmodechm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cmp_mode)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.cmp_mode = cmp_mode;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setislosslesschm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 is_lossless)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.is_lossless = is_lossless;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_seticeenchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 ice_en)
{
    u_vdp_core_v1_vid_line_cdcmp_glb_info vdp_core_v1_vid_line_cdcmp_glb_info;

    vdp_core_v1_vid_line_cdcmp_glb_info.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_glb_info.bits.ice_en = ice_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_glb_info.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_glb_info.u32);

    return;
}

hi_void vdp_fdr_vid_setframeheightchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 frame_height)
{
    u_vdp_core_v1_vid_line_cdcmp_frame_size vdp_core_v1_vid_line_cdcmp_frame_size;

    vdp_core_v1_vid_line_cdcmp_frame_size.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_frame_size.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_frame_size.bits.frame_height = frame_height;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_frame_size.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_frame_size.u32);

    return;
}

hi_void vdp_fdr_vid_setframewidthchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 frame_width)
{
    u_vdp_core_v1_vid_line_cdcmp_frame_size vdp_core_v1_vid_line_cdcmp_frame_size;

    vdp_core_v1_vid_line_cdcmp_frame_size.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_frame_size.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_frame_size.bits.frame_width = frame_width;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_frame_size.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_frame_size.u32);

    return;
}

hi_void vdp_fdr_vid_setadpqpthr0chm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 adpqp_thr0)
{
    u_vdp_core_v1_vid_line_cdcmp_adpqp_thr0 vdp_core_v1_vid_line_cdcmp_adpqp_thr0;

    vdp_core_v1_vid_line_cdcmp_adpqp_thr0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_adpqp_thr0.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_adpqp_thr0.bits.adpqp_thr0 = adpqp_thr0;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_adpqp_thr0.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_adpqp_thr0.u32);

    return;
}

hi_void vdp_fdr_vid_setadpqpthr1chm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 adpqp_thr1)
{
    u_vdp_core_v1_vid_line_cdcmp_adpqp_thr1 vdp_core_v1_vid_line_cdcmp_adpqp_thr1;

    vdp_core_v1_vid_line_cdcmp_adpqp_thr1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_adpqp_thr1.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_adpqp_thr1.bits.adpqp_thr1 = adpqp_thr1;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_adpqp_thr1.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_adpqp_thr1.u32);

    return;
}

hi_void vdp_fdr_vid_setsmoothdeltabitsthrchm(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 smooth_deltabits_thr)
{
    u_vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr;

    vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr.bits.smooth_deltabits_thr = smooth_deltabits_thr;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_smth_deltabits_thr.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumechm0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta0 vdp_core_v1_vid_line_cdcmp_error_sta0;

    vdp_core_v1_vid_line_cdcmp_error_sta0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta0.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta0.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta0.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta0.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivechm0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta0 vdp_core_v1_vid_line_cdcmp_error_sta0;

    vdp_core_v1_vid_line_cdcmp_error_sta0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta0.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta0.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta0.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta0.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorchm0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta0 vdp_core_v1_vid_line_cdcmp_error_sta0;

    vdp_core_v1_vid_line_cdcmp_error_sta0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta0.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta0.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta0.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta0.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbgchm0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_cdcmp_dbg_reg0 vdp_core_v1_vid_line_cdcmp_dbg_reg0;

    vdp_core_v1_vid_line_cdcmp_dbg_reg0.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg0.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_dbg_reg0.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg0.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_dbg_reg0.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumechm1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta1 vdp_core_v1_vid_line_cdcmp_error_sta1;

    vdp_core_v1_vid_line_cdcmp_error_sta1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta1.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta1.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta1.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta1.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivechm1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta1 vdp_core_v1_vid_line_cdcmp_error_sta1;

    vdp_core_v1_vid_line_cdcmp_error_sta1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta1.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta1.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta1.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta1.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorchm1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta1 vdp_core_v1_vid_line_cdcmp_error_sta1;

    vdp_core_v1_vid_line_cdcmp_error_sta1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta1.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta1.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta1.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta1.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbgchm1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_cdcmp_dbg_reg1 vdp_core_v1_vid_line_cdcmp_dbg_reg1;

    vdp_core_v1_vid_line_cdcmp_dbg_reg1.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg1.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_dbg_reg1.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg1.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_dbg_reg1.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumechm2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta2 vdp_core_v1_vid_line_cdcmp_error_sta2;

    vdp_core_v1_vid_line_cdcmp_error_sta2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta2.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta2.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta2.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta2.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivechm2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta2 vdp_core_v1_vid_line_cdcmp_error_sta2;

    vdp_core_v1_vid_line_cdcmp_error_sta2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta2.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta2.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta2.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta2.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorchm2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta2 vdp_core_v1_vid_line_cdcmp_error_sta2;

    vdp_core_v1_vid_line_cdcmp_error_sta2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta2.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta2.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta2.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta2.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbgchm2(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_cdcmp_dbg_reg2 vdp_core_v1_vid_line_cdcmp_dbg_reg2;

    vdp_core_v1_vid_line_cdcmp_dbg_reg2.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg2.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_dbg_reg2.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg2.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_dbg_reg2.u32);

    return;
}

hi_void vdp_fdr_vid_setconsumechm3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 consume)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta3 vdp_core_v1_vid_line_cdcmp_error_sta3;

    vdp_core_v1_vid_line_cdcmp_error_sta3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta3.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta3.bits.consume = consume;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta3.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta3.u32);

    return;
}

hi_void vdp_fdr_vid_setforgivechm3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 forgive)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta3 vdp_core_v1_vid_line_cdcmp_error_sta3;

    vdp_core_v1_vid_line_cdcmp_error_sta3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta3.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta3.bits.forgive = forgive;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta3.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta3.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmperrorchm3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_error)
{
    u_vdp_core_v1_vid_line_cdcmp_error_sta3 vdp_core_v1_vid_line_cdcmp_error_sta3;

    vdp_core_v1_vid_line_cdcmp_error_sta3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta3.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_error_sta3.bits.dcmp_error = dcmp_error;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_error_sta3.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_error_sta3.u32);

    return;
}

hi_void vdp_fdr_vid_setdcmpdbgchm3(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_dbg)
{
    u_vdp_core_v1_vid_line_cdcmp_dbg_reg3 vdp_core_v1_vid_line_cdcmp_dbg_reg3;

    vdp_core_v1_vid_line_cdcmp_dbg_reg3.u32 =
        vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg3.u32) + offset));
    vdp_core_v1_vid_line_cdcmp_dbg_reg3.bits.dcmp_dbg = dcmp_dbg;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_line_cdcmp_dbg_reg3.u32) + offset),
                 vdp_core_v1_vid_line_cdcmp_dbg_reg3.u32);

    return;
}

hi_void vdp_fdr_vid_setreqbalance(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lum_req, hi_u32 chm_req)
{
    u_vid_req_ctrl vid_req_ctrl;

    vid_req_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_req_ctrl.u32)));
    vid_req_ctrl.bits.rrobin_ctrl_l = lum_req;
    vid_req_ctrl.bits.rrobin_ctrl_c = chm_req;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_req_ctrl.u32)), vid_req_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setdfifoqosthr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 high_thd, hi_u32 low_thd)
{
    u_vid_fifo_set vid_fifo_set;

    vid_fifo_set.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_fifo_set.u32)));
    vid_fifo_set.bits.dfifo_qos_hthr = high_thd;
    vid_fifo_set.bits.dfifo_qos_lthr = low_thd;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_fifo_set.u32)), vid_fifo_set.u32);

    return;
}

hi_void vdp_fdr_vid_setqosthren(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 thr_en)
{
    u_vid_fifo_set vid_fifo_set;

    vid_fifo_set.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vid_fifo_set.u32) + offset));
    vid_fifo_set.bits.qos_thr_en = thr_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vid_fifo_set.u32) + offset), vid_fifo_set.u32);

    return;
}


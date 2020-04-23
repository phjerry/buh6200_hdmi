/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_region.h"
#include "hal_vdp_comm.h"

hi_void vdp_fdr_vid_setmrgrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rd_en)
{
    u_vdp_core_v1_vid_mrg_read_en vdp_core_v1_vid_mrg_read_en;

    vdp_core_v1_vid_mrg_read_en.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_read_en.u32) +
                                                   offset));
    vdp_core_v1_vid_mrg_read_en.bits.rd_en = rd_en;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_read_en.u32) + offset),
                 vdp_core_v1_vid_mrg_read_en.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgrdregion(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rd_region)
{
    u_vdp_core_v1_vid_mrg_read_ctrl vdp_core_v1_vid_mrg_read_ctrl;

    vdp_core_v1_vid_mrg_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_read_ctrl.u32) +
                                                     offset));
    vdp_core_v1_vid_mrg_read_ctrl.bits.rd_region = rd_region;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_read_ctrl.u32) + offset),
                 vdp_core_v1_vid_mrg_read_ctrl.u32);

    return;
}
hi_void vdp_fdr_vid_setglbmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 glb_mmu_bypass)
{
    u_vdp_core_v1_vid_mrg_read_ctrl vdp_core_v1_vid_mrg_read_ctrl;

    vdp_core_v1_vid_mrg_read_ctrl.u32 = vdp_regread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_read_ctrl.u32) +
                                                     offset));
    vdp_core_v1_vid_mrg_read_ctrl.bits.glb_mmu_bypass = glb_mmu_bypass;
    vdp_regwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_read_ctrl.u32) + offset),
                 vdp_core_v1_vid_mrg_read_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_en)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_en = mrg_en;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgmuteen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mute_en)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_mute_en = mrg_mute_en;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrglmmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mmu_bypass)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_lm_mmu_bypass = mrg_mmu_bypass;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgchmmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mmu_bypass)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_chm_mmu_bypass = mrg_mmu_bypass;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgdcmpen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_dcmp_en)
{
#if 0
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_dcmp_en = mrg_dcmp_en;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);
#endif

    return;
}

hi_void vdp_fdr_vid_setmrgcropen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_crop_en)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_crop_en = mrg_crop_en;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgedgetyp(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_edge_typ)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_edge_typ = mrg_edge_typ;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgedgeen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_edge_en)
{
    u_vdp_core_v1_vid_mrg_ctrl vdp_core_v1_vid_mrg_ctrl;

    vdp_core_v1_vid_mrg_ctrl.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset));
    vdp_core_v1_vid_mrg_ctrl.bits.mrg_edge_en = mrg_edge_en;
    vdp_ddrwrite ((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ctrl.u32) + offset), vdp_core_v1_vid_mrg_ctrl.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgypos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_ypos)
{
    u_vdp_core_v1_vid_mrg_disp_pos vdp_core_v1_vid_mrg_disp_pos;

    vdp_core_v1_vid_mrg_disp_pos.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_pos.u32) +
                                                    offset));
    vdp_core_v1_vid_mrg_disp_pos.bits.mrg_ypos = mrg_ypos;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_pos.u32) + offset),
                 vdp_core_v1_vid_mrg_disp_pos.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgxpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_xpos)
{
    u_vdp_core_v1_vid_mrg_disp_pos vdp_core_v1_vid_mrg_disp_pos;

    vdp_core_v1_vid_mrg_disp_pos.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_pos.u32) +
                                                    offset));
    vdp_core_v1_vid_mrg_disp_pos.bits.mrg_xpos = mrg_xpos;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_pos.u32) + offset),
                 vdp_core_v1_vid_mrg_disp_pos.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_height)
{
    u_vdp_core_v1_vid_mrg_disp_reso vdp_core_v1_vid_mrg_disp_reso;

    vdp_core_v1_vid_mrg_disp_reso.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_reso.u32) +
                                                     offset));
    vdp_core_v1_vid_mrg_disp_reso.bits.mrg_height = mrg_height;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_reso.u32) + offset),
                 vdp_core_v1_vid_mrg_disp_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_width)
{
    u_vdp_core_v1_vid_mrg_disp_reso vdp_core_v1_vid_mrg_disp_reso;

    vdp_core_v1_vid_mrg_disp_reso.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_reso.u32) +
                                                     offset));
    vdp_core_v1_vid_mrg_disp_reso.bits.mrg_width = mrg_width;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_disp_reso.u32) + offset),
                 vdp_core_v1_vid_mrg_disp_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgsrcheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_height)
{
    u_vdp_core_v1_vid_mrg_src_reso vdp_core_v1_vid_mrg_src_reso;

    vdp_core_v1_vid_mrg_src_reso.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_reso.u32) +
                                                    offset));
    vdp_core_v1_vid_mrg_src_reso.bits.mrg_src_height = mrg_src_height;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_reso.u32) + offset),
                 vdp_core_v1_vid_mrg_src_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgsrcwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_width)
{
    u_vdp_core_v1_vid_mrg_src_reso vdp_core_v1_vid_mrg_src_reso;

    vdp_core_v1_vid_mrg_src_reso.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_reso.u32) +
                                                    offset));
    vdp_core_v1_vid_mrg_src_reso.bits.mrg_src_width = mrg_src_width;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_reso.u32) + offset),
                 vdp_core_v1_vid_mrg_src_reso.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgsrcvoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_voffset)
{
    u_vdp_core_v1_vid_mrg_src_offset vdp_core_v1_vid_mrg_src_offset;

    vdp_core_v1_vid_mrg_src_offset.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_offset.u32) +
                                                      offset));
    vdp_core_v1_vid_mrg_src_offset.bits.mrg_src_voffset = mrg_src_voffset;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_offset.u32) + offset),
                 vdp_core_v1_vid_mrg_src_offset.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgsrchoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_hoffset)
{
    u_vdp_core_v1_vid_mrg_src_offset vdp_core_v1_vid_mrg_src_offset;

    vdp_core_v1_vid_mrg_src_offset.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_offset.u32) +
                                                      offset));
    vdp_core_v1_vid_mrg_src_offset.bits.mrg_src_hoffset = mrg_src_hoffset;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_src_offset.u32) + offset),
                 vdp_core_v1_vid_mrg_src_offset.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgyaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_y_addr)
{
    u_vdp_core_v1_vid_mrg_y_addr vdp_core_v1_vid_mrg_y_addr;

    vdp_core_v1_vid_mrg_y_addr.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_y_addr.u32) +
                                                  offset));
    vdp_core_v1_vid_mrg_y_addr.bits.mrg_y_addr = mrg_y_addr;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_y_addr.u32) + offset),
                 vdp_core_v1_vid_mrg_y_addr.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgcaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_c_addr)
{
    u_vdp_core_v1_vid_mrg_c_addr vdp_core_v1_vid_mrg_c_addr;

    vdp_core_v1_vid_mrg_c_addr.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_c_addr.u32) +
                                                  offset));
    vdp_core_v1_vid_mrg_c_addr.bits.mrg_c_addr = mrg_c_addr;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_c_addr.u32) + offset),
                 vdp_core_v1_vid_mrg_c_addr.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgystride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_y_stride)
{
    u_vdp_core_v1_vid_mrg_stride vdp_core_v1_vid_mrg_stride;

    vdp_core_v1_vid_mrg_stride.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_stride.u32) +
                                                  offset));
    vdp_core_v1_vid_mrg_stride.bits.mrg_y_stride = mrg_y_stride;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_stride.u32) + offset),
                 vdp_core_v1_vid_mrg_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgcstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_c_stride)
{
    u_vdp_core_v1_vid_mrg_stride vdp_core_v1_vid_mrg_stride;

    vdp_core_v1_vid_mrg_stride.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_stride.u32) +
                                                  offset));
    vdp_core_v1_vid_mrg_stride.bits.mrg_c_stride = mrg_c_stride;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_stride.u32) + offset),
                 vdp_core_v1_vid_mrg_stride.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgyhaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_yh_addr)
{
    u_vdp_core_v1_vid_mrg_yh_addr vdp_core_v1_vid_mrg_yh_addr;

    vdp_core_v1_vid_mrg_yh_addr.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_yh_addr.u32) +
                                                   offset));
    vdp_core_v1_vid_mrg_yh_addr.bits.mrg_yh_addr = mrg_yh_addr;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_yh_addr.u32) + offset),
                 vdp_core_v1_vid_mrg_yh_addr.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgchaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_ch_addr)
{
    u_vdp_core_v1_vid_mrg_ch_addr vdp_core_v1_vid_mrg_ch_addr;

    vdp_core_v1_vid_mrg_ch_addr.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ch_addr.u32) +
                                                   offset));
    vdp_core_v1_vid_mrg_ch_addr.bits.mrg_ch_addr = mrg_ch_addr;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_ch_addr.u32) + offset),
                 vdp_core_v1_vid_mrg_ch_addr.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgyhstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_yh_stride)
{
    u_vdp_core_v1_vid_mrg_hstride vdp_core_v1_vid_mrg_hstride;

    vdp_core_v1_vid_mrg_hstride.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_hstride.u32) +
                                                   offset));
    vdp_core_v1_vid_mrg_hstride.bits.mrg_yh_stride = mrg_yh_stride;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_hstride.u32) + offset),
                 vdp_core_v1_vid_mrg_hstride.u32);

    return;
}

hi_void vdp_fdr_vid_setmrgchstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_ch_stride)
{
    u_vdp_core_v1_vid_mrg_hstride vdp_core_v1_vid_mrg_hstride;

    vdp_core_v1_vid_mrg_hstride.u32 = vdp_ddrread((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_hstride.u32) +
                                                   offset));
    vdp_core_v1_vid_mrg_hstride.bits.mrg_ch_stride = mrg_ch_stride;
    vdp_ddrwrite((uintptr_t)(&(vdp_reg->vdp_core_v1_vid_mrg_hstride.u32) + offset),
                 vdp_core_v1_vid_mrg_hstride.u32);

    return;
}



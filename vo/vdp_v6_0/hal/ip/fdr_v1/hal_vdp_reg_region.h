/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_REGION_H__
#define __HAL_VDP_REG_REGION_H__

#include "hi_reg_vdp.h"

hi_void vdp_fdr_vid_setmrgrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rd_en);
hi_void vdp_fdr_vid_setmrgrdregion(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rd_region);
hi_void vdp_fdr_vid_setglbmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 glb_bmmu_bypass);

hi_void vdp_fdr_vid_setmrgen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_en);
hi_void vdp_fdr_vid_setmrgmuteen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mute_en);
hi_void vdp_fdr_vid_setmrglmmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mmu_bypass);
hi_void vdp_fdr_vid_setmrgchmmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_mmu_bypass);
hi_void vdp_fdr_vid_setmrgdcmpen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_dcmp_en);
hi_void vdp_fdr_vid_setmrgcropen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_crop_en);
hi_void vdp_fdr_vid_setmrgedgetyp(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_edge_typ);
hi_void vdp_fdr_vid_setmrgedgeen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_edge_en);
hi_void vdp_fdr_vid_setmrgypos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_ypos);
hi_void vdp_fdr_vid_setmrgxpos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_xpos);
hi_void vdp_fdr_vid_setmrgheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_height);
hi_void vdp_fdr_vid_setmrgwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_width);
hi_void vdp_fdr_vid_setmrgsrcheight(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_height);
hi_void vdp_fdr_vid_setmrgsrcwidth(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_width);
hi_void vdp_fdr_vid_setmrgsrcvoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_voffset);
hi_void vdp_fdr_vid_setmrgsrchoffset(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_src_hoffset);
hi_void vdp_fdr_vid_setmrgyaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_y_addr);
hi_void vdp_fdr_vid_setmrgcaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_c_addr);
hi_void vdp_fdr_vid_setmrgystride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_y_stride);
hi_void vdp_fdr_vid_setmrgcstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_c_stride);
hi_void vdp_fdr_vid_setmrgyhaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_yh_addr);
hi_void vdp_fdr_vid_setmrgchaddr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_c_addr);
hi_void vdp_fdr_vid_setmrgyhstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_y_stride);
hi_void vdp_fdr_vid_setmrgchstride(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mrg_c_stride);

#endif

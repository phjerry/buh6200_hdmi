/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_TMAP_V2_H__
#define __HAL_VDP_REG_TMAP_V2_H__

#include "hi_reg_vdp.h"

hi_void vdp_tmap_v2_settmrshiftrounden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_rshift_round_en);
hi_void vdp_tmap_v2_setstbtonemapen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 stb_tonemap_en);
hi_void vdp_tmap_v2_settmapsclutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sclut_rd_en);
hi_void vdp_tmap_v2_settmapsslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_sslut_rd_en);
hi_void vdp_tmap_v2_settmaptslutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tslut_rd_en);
hi_void vdp_tmap_v2_settmapsilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_silut_rd_en);
hi_void vdp_tmap_v2_settmaptilutrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_tilut_rd_en);
hi_void vdp_tmap_v2_settmapparardata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tmap_para_rdata);
hi_void vdp_tmap_v2_settmc1expan(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_c1_expan);
hi_void vdp_tmap_v2_settmsmcenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 tm_smc_enable);
hi_void vdp_tmap_v2_setimtmrshiftbit(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_bit);
hi_void vdp_tmap_v2_setimtmrshiften(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_rshift_en);
hi_void vdp_tmap_v2_setimtms2uen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 imtm_s2u_en);

#endif

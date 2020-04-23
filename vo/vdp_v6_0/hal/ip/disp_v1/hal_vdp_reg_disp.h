/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_DISP_H__
#define __HAL_VDP_REG_DISP_H__

#include "hi_reg_vdp.h"

hi_void vdp_dsp_setchn0mux0(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void vdp_dsp_setchn0mux1(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void vdp_dsp_setchn0mux2(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void vdp_dsp_setchn0splitenable(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void vdp_dsp_setchn0splitoverlap(vdp_regs_type *vdp_reg, hi_u32 data);

hi_void vdp_dispchn_set_cbm_mix(vdp_regs_type *vdp_reg, vdp_dispchn_cbm_mix mixer_id, hi_u32 layer_id,
                                hi_u32 prio);
hi_void vdp_dispchn_set_cbm_bkg(vdp_dispchn_cbm_mix chn_id, vdp_bkg *vdp_bkg);
hi_void vdp_dispchn_reset_cbm_mix(vdp_regs_type *vdp_reg, vdp_dispchn_cbm_mix mixer_id);

hi_u32 vdp_dispchn_get_voint(vdp_regs_type *g_vdp_reg);
hi_u32 vdp_dispchn_get_voint_masked(vdp_regs_type *vdp_reg);
hi_void vdp_dispchn_clean_voint(vdp_regs_type *vdp_reg, hi_u32 clean_value);

#endif


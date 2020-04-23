/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_VHDR_CM_H__
#define __HAL_VDP_REG_VHDR_CM_H__

#include "hi_reg_vdp.h"

hi_void vdp_vhdr_cm_setvhdrcmckgten(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_ck_gt_en);
hi_void vdp_vhdr_cm_setvhdrcmen(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_en);
hi_void vdp_vhdr_cm_setvhdrcmclipcmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_max);
hi_void vdp_vhdr_cm_setvhdrcmclipcmin(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_clip_c_min);
hi_void vdp_vhdr_cm_setvhdrcmscale2p(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_scale2p);
hi_void vdp_vhdr_cm_setvhdrcmrden(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_rd_en);
hi_void vdp_vhdr_cm_setvhdrcmparadata(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 vhdr_cm_para_data);
hi_void vdp_vhdr_cm_setvcmx4step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_step);
hi_void vdp_vhdr_cm_setvcmx3step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_step);
hi_void vdp_vhdr_cm_setvcmx2step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_step);
hi_void vdp_vhdr_cm_setvcmx1step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_step);
hi_void vdp_vhdr_cm_setvcmx8step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_step);
hi_void vdp_vhdr_cm_setvcmx7step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_step);
hi_void vdp_vhdr_cm_setvcmx6step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_step);
hi_void vdp_vhdr_cm_setvcmx5step(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_step);
hi_void vdp_vhdr_cm_setvcmx1pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_pos);
hi_void vdp_vhdr_cm_setvcmx2pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_pos);
hi_void vdp_vhdr_cm_setvcmx3pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_pos);
hi_void vdp_vhdr_cm_setvcmx4pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_pos);
hi_void vdp_vhdr_cm_setvcmx5pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_pos);
hi_void vdp_vhdr_cm_setvcmx6pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_pos);
hi_void vdp_vhdr_cm_setvcmx7pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_pos);
hi_void vdp_vhdr_cm_setvcmx8pos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_pos);
hi_void vdp_vhdr_cm_setvcmx4num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x4_num);
hi_void vdp_vhdr_cm_setvcmx3num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x3_num);
hi_void vdp_vhdr_cm_setvcmx2num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x2_num);
hi_void vdp_vhdr_cm_setvcmx1num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x1_num);
hi_void vdp_vhdr_cm_setvcmx8num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x8_num);
hi_void vdp_vhdr_cm_setvcmx7num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x7_num);
hi_void vdp_vhdr_cm_setvcmx6num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x6_num);
hi_void vdp_vhdr_cm_setvcmx5num(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 v_cm_x5_num);

#endif



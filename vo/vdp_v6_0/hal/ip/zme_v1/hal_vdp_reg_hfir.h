/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_HFIR_H__
#define __HAL_VDP_REG_HFIR_H__

#include "hi_reg_vdp.h"
#include "vdp_chip_define.h"

hi_void vdp_vid_setifirmode(vdp_regs_type *vdp_reg, hi_u32 u32data, vdp_ifirmode enmode);
hi_void vdp_vid_setifirmiden(vdp_regs_type *vdp_reg, hi_u32 u32data, hi_u32 u32en);
hi_void vdp_vid_setifirckgten(vdp_regs_type *vdp_reg, hi_u32 u32data, hi_u32 u32en);
hi_void vdp_vid_setifircoef(vdp_regs_type *vdp_reg, hi_u32 u32data, hi_s32 *s32coef);

#endif

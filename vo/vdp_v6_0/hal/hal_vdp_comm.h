/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HAL_VDP_COMM_H__
#define __HAL_VDP_COMM_H__

#include "vdp_chip_define.h"
#include "hi_reg_vdp.h"
#include "hal_vdp_debug.h"
#include "hi_reg_common.h"
#include "hi_osal.h"

extern vdp_regs_type *g_vdp_reg;
extern hi_reg_crg *g_vdp_crg_reg;

hi_s32 vdp_reg_map(hi_void);
hi_void vdp_reg_unmap(hi_void);

hi_u32 vdp_regread(uintptr_t a);
hi_void vdp_regwrite(uintptr_t a, hi_u32 b);

hi_u32 vdp_ddrread(uintptr_t a);
hi_void vdp_ddrwrite(uintptr_t a, hi_u32 b);

#endif


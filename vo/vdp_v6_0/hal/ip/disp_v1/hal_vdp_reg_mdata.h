/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __VDP_HAL_IP_MDATA_H__
#define __VDP_HAL_IP_MDATA_H__

#include "hi_reg_vdp.h"


hi_void vdp_mdata_set_chn_en     ( vdp_regs_type *reg, hi_u32 chn_en     );
hi_void vdp_mdata_set_nosec_flag ( vdp_regs_type *reg, hi_u32 nosec_flag );
hi_void vdp_mdata_set_mmu_bypass ( vdp_regs_type *reg, hi_u32 mmu_bypass );
hi_void vdp_mdata_set_avi_valid  ( vdp_regs_type *reg, hi_u32 avi_valid  );
hi_void vdp_mdata_set_smd_valid  ( vdp_regs_type *reg, hi_u32 smd_valid  );
hi_void vdp_mdata_set_sreq_delay ( vdp_regs_type *reg, hi_u32 sreq_delay );
hi_void vdp_mdata_set_regup     ( vdp_regs_type *reg, hi_u32 regup      );
hi_void vdp_mdata_set_maddr_h     ( vdp_regs_type *reg, hi_u32 maddr      );
hi_void vdp_mdata_set_maddr_l     ( vdp_regs_type *reg, hi_u32 maddr      );
hi_void vdp_mdata_set_mburst    ( vdp_regs_type *reg, hi_u32 mburst     );

#endif

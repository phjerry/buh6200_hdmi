/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_MMU_H__
#define __HAL_VDP_IP_MMU_H__

#include "vdp_chip_define.h"

typedef enum {
    VDP_MMU_SEL_VDP_0 = 0,
    VDP_MMU_SEL_VDP_1,
    VDP_MMU_SEL_VDP_2,
    VDP_MMU_SEL_BUTT
} vdp_mmu_sel;

hi_void vdp_ip_mmu_set(hi_bool);
hi_s32 vdp_ip_mmu_get_interrupt_state(hi_void);
hi_void vdp_ip_mmu_clean_interrupt_state(hi_s32 state);
hi_s32 vdp_ip_mmu_get_error_addr_ns(hi_void);

#endif


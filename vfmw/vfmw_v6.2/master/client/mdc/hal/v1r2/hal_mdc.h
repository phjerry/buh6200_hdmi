/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __HAL_MDC0__
#define __HAL_MDC0__

#include "vfmw.h"

hi_s32 mdc_hal_load(hi_void);
hi_s32 mdc_hal_unload(hi_void);
hi_s32 mdc_hal_is_wfi(hi_void);
hi_void mdc_hal_print_mdc_state(hi_void);
hi_void mdc_hal_dump_reg(hi_void *buf, hi_u32 len, hi_s32 *used);
hi_void mdc_hal_get_base_time(hi_u64 *arm_us, hi_u64 *mdc_tick);

#endif

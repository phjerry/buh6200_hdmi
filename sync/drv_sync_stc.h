/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync stc interface
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef __SYNC_DRV_STC_H__
#define __SYNC_DRV_STC_H__

#include "drv_sync.h"
#define  SYNC_INVAILDE_PPM    10000

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#ifdef  AIAO_SRC_PLL_HAS_CREECTED
typedef enum {
    SYNC_AUD_PLL_VALUE_TYPE_INT,
    SYNC_AUD_PLL_VALUE_TYPE_FRAC,
} sync_aud_pll_value_type;

hi_void sync_set_aud_pll_src_value(sync_aud_pll_value_type type, hi_u32 value);
hi_u32 sync_get_aud_pll_src_value(sync_aud_pll_value_type type);
#endif

void sync_stc_sys_init(void);
void sync_stc_sys_deinit(void);

void sync_stc_adjust(sync_context *ctx, hi_u64 pcr, hi_u64 stc);
hi_s32 sync_stc_adjust_by_external(sync_context *ctx, hi_s32 ppm);
hi_s32 sync_hpll_conf_flag_set(hi_bool is_conf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


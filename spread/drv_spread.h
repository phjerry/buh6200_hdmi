/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_spread.h
 * Author: BSP
 * Create: 2019-11-29
 */

#ifndef __DRV_SPREAD_H__
#define __DRV_SPREAD_H__

#include "hi_type.h"

hi_s32 hi_drv_ss_set_ddr_spread_en(hi_bool *pb_enable);
hi_s32 hi_drv_ss_set_ddr_spread_ratio(hi_u32 *pu32_spread_ratio);
hi_s32 hi_drv_ss_set_ddr_spread_freq(hi_u32 *pu32_spread_freq);
hi_s32 hi_drv_ss_set_ddr_max_ratio(hi_u32 max_ratio);

hi_s32 hi_drv_ss_set_gmac_clk_en(hi_bool *pb_enable);
hi_s32 hi_drv_ss_set_gmac_spread_en(hi_bool *pb_enable);
hi_s32 hi_drv_ss_set_gmac_spread_ratio(hi_u32 *pu32_spread_ratio);
hi_s32 hi_drv_ss_set_gmac_spread_freq(hi_u32 *pu32_spread_freq);

hi_s32 hi_drv_ss_set_emmc_clk_en(hi_bool *pb_enable);
hi_s32 hi_drv_ss_set_emmc_spread_en(hi_bool *pb_enable);
hi_s32 hi_drv_ss_set_emmc_spread_ratio(hi_u32 *pu32_spread_ratio);
hi_s32 hi_drv_ss_set_emmc_spread_freq(hi_u32 *pu32_spread_freq);

hi_s32 hi_drv_ss_set_ci_clk_en(hi_bool *pb_enable);
#endif /* __DRV_SPREAD_H__ */

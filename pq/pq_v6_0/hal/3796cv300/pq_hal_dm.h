/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq hal dm define
 * Author: pq
 * Create: 2019-01-1
 */

#ifndef __PQ_HAL_DM_H__
#define __PQ_HAL_DM_H__

#include "hi_type.h"
#include "pq_hal_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 pq_hal_enable_dm(hi_u32 handle, hi_bool on_off);
hi_s32 pq_hal_enable_dm_demo(hi_u32 handle, hi_bool on_off);
hi_s32 pq_hal_set_dm_strength(hi_u32 handle, hi_u32 strength);
hi_s32 pq_hal_update_dm_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input);
hi_s32 pq_hal_set_dm_api_reg(hi_u32 handle_no, hi_drv_pq_dm_output_reg *dm_api_reg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

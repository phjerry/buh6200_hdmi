/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: deshoot
 * Author: pq
 * Create: 2019-11-4
 */

#ifndef __PQ_HAL_DESHOOT_H__
#define __PQ_HAL_DESHOOT_H__

#include "hi_type.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 pq_hal_enable_deshoot(hi_u32 handle_no, hi_bool ds_on_off);

hi_s32 pq_hal_deshoot_set_strength(hi_s32 handle_no, hi_u32 blendgainov, hi_u32 blendgainud);

hi_s32 pq_hal_enable_deshoot_demo(hi_u32 handle_no, hi_bool ds_demo_en);

hi_s32 pq_hal_set_deshoot_demo_mode(hi_u32 handle_no, pq_demo_mode mode);

hi_s32 pq_hal_set_deshoot_demo_coor(hi_u32 handle_no, hi_u32 x_pos);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

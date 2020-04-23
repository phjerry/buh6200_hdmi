/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dering
 * Author: pq
 * Create: 2019-11-7
 */

#ifndef __PQ_HAL_DERING_H__
#define __PQ_HAL_DERING_H__

#include "hi_type.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 pq_hal_set_dering_demo_enable(hi_u32 handle_no, hi_bool demo_en);

hi_s32 pq_hal_set_dering_demo_mode(hi_u32 handle_no, pq_demo_mode mode);

hi_s32 pq_hal_set_dering_enable(hi_u32 handle_no, hi_bool enable);

hi_s32 pq_hal_set_dering_demo_coor(hi_u32 handle_no, hi_u32 x_pos);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



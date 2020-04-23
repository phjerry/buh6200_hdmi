/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal dei api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_DEI_H__
#define __PQ_MNG_DEI_H__

#include "hi_type.h"
#include "drv_pq_comm.h"
#include "pq_hal_dei.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* input of sofeware */
typedef struct {
    hi_u16 au16_global_motion_thd[6]; /* 6: global_motion_thd len */
    hi_u16 au16_global_motion_ratio[5]; /* 5: global_motion_ratio len */
    hi_s16 as16_global_motion_slope[4]; /* 4: global_motion_slope len */
    hi_u16 max_global_motion_ratio;
    hi_u16 min_global_motion_ratio;
    hi_u32 small_motion_thd;
    hi_u32 large_motion_thd;
} motion_calc; /* MotionCalc_S */

typedef struct {
    hi_bool init;
    hi_bool enable;
    hi_s32 fr_count_mc;
    hi_bool demo_enable;
    hi_bool mc_only_enable;
    pq_demo_mode demo_mode;
    motion_calc motion_calc;
} alg_dei;

hi_s32 pq_mng_register_dei(pq_reg_type type);

hi_s32 pq_mng_unregister_dei(hi_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


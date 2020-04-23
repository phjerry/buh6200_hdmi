/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: deshoot
 * Author: pq
 * Create: 2019-11-4
 */

#ifndef __PQ_MNG_DESHOOT_H__
#define __PQ_MNG_DESHOOT_H__

#include "hi_type.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool init;
    hi_bool enable;
    hi_u32 strength;
    hi_bool demo_enable;
    pq_demo_mode demo_mode;
} alg_ds;

hi_s32 pq_mng_register_ds(pq_reg_type type);

hi_s32 pq_mng_unregister_ds(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



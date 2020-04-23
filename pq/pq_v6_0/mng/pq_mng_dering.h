/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dering
 * Author: pq
 * Create: 2019-11-7
 */

#ifndef __PQ_MNG_DERING_H__
#define __PQ_MNG_DERING_H__

#include "hi_type.h"
#include "drv_pq_comm.h"
#include "pq_hal_dering.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool init;
    hi_bool enable;
    hi_bool demo_enable;
    pq_demo_mode demo_mode;
} alg_dr;

hi_s32 pq_mng_register_dr(pq_reg_type type);

hi_s32 pq_mng_unregister_dr(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



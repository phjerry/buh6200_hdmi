/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq mng tnr define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_TNR_H__
#define __PQ_MNG_TNR_H__

#include "drv_pq_comm.h"
#include "hi_vpss_register.h"
#include "pq_hal_tnr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

typedef struct {
    hi_bool init;
    hi_bool enable;
    hi_u32 strength;
    hi_bool demo_enable;
    pq_demo_mode demo_mode;
} alg_tnr;

hi_s32 pq_mng_register_tnr(pq_reg_type type);

hi_s32 pq_mng_unregister_tnr(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif

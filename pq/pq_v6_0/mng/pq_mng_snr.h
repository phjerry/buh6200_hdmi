/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq mng snr api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_SNR_H__
#define __PQ_MNG_SNR_H__

#include "drv_pq_comm.h"
#include "pq_hal_snr.h"

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
} alg_snr;

hi_s32 pq_mng_register_snr(pq_reg_type type);

hi_s32 pq_mng_unregister_snr(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif




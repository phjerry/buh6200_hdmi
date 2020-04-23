/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal ifmd api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_IFMD_H__
#define __PQ_MNG_IFMD_H__

#include "hi_type.h"

#include "pq_hal_comm.h"
#include "pq_hal_fmd.h"
#include "drv_pq_table.h"
#include "pq_mng_ifmd_alg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool is_ifmd_init;
    hi_bool is_ifmd_enable;
} pq_fmd_alg;

hi_s32 pq_mng_register_fmd(pq_reg_type type);

hi_s32 pq_mng_unregister_fmd(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif

#endif

#endif


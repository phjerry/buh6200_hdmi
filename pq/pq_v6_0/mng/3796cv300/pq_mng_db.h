/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: db mng layer header file
 * Author: pq
 * Create:
 */


#ifndef __PQ_MNG_DB_H__
#define __PQ_MNG_DB_H__

#include "drv_pq_comm.h"
#include "pq_hal_db.h"
#include "pq_hal_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct
{
    hi_bool is_init;
    hi_bool enable;
    hi_u32  strength;
    hi_bool demo_enable;
    hi_u32  demo_pos;
    pq_demo_mode demo_mode;
} alg_db;


hi_s32 pq_mng_register_db(hi_void);
hi_s32 pq_mng_unregister_db(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif




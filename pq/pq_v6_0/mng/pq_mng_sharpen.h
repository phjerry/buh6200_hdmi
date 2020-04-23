/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sharpen mng layer header file
 * Author: pq
 * Create: 2019-11-11
 */

#ifndef __PQ_MNG_SHARPEN_H__
#define __PQ_MNG_SHARPEN_H__

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool       is_init;
    hi_bool       enable;

    hi_u32        strength;
    hi_bool       demo_en;
    pq_demo_mode  demo_mode;
    hi_u32        demo_pos;

    pq_sharpen_reso  sharpen_reso;
} alg_sharpen;


hi_s32 pq_mng_register_sharp(pq_reg_type type);
hi_s32 pq_mng_unregister_sharp(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


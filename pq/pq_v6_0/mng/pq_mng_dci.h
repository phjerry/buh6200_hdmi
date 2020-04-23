/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dci mng layer header file
 * Author: pq
 * Create: 2019-01-01
 */

#ifndef __PQ_MNG_DCI_H__
#define __PQ_MNG_DCI_H__

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool       is_init;
    hi_bool       enable;
    hi_bool       scd_en;

    hi_u32        strength;
    hi_bool       demo_en;
    pq_demo_mode  demo_mode;
    hi_u32        demo_pos;

    pq_dci_ref_reso   dci_ref_reso;
} alg_dci;

hi_s32 pq_mng_register_dci(hi_void);
hi_s32 pq_mng_unregister_dci(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



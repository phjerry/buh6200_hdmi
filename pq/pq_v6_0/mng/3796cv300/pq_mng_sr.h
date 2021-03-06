/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: sr mng layer header file
 * Author: pq
 * Create: 2019-09-29
 */

#ifndef __PQ_MNG_SR_H__
#define __PQ_MNG_SR_H__
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ************************** structure definition *************************** */
hi_s32 pq_mng_register_sr(hi_void);
hi_s32 pq_mng_unregister_sr(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

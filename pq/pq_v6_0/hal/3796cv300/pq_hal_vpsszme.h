/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal vpsszme define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_VPSSZME_H__
#define __PQ_HAL_VPSSZME_H__

#include "pq_hal_comm.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS_COEF_SIZE_HZME (4096 * 128 / 8)
#define VPSS_COEF_SIZE_VZME (4096 * 128 / 8)

#define VPSS_ZME_COEF_SIZE (VPSS_COEF_SIZE_HZME + VPSS_COEF_SIZE_VZME)

typedef enum {
    PQ_HAL_VPSSZME_COEF_1 = 0,
    PQ_HAL_VPSSZME_COEF_E1 = 1,
    PQ_HAL_VPSSZME_COEF_075 = 2,
    PQ_HAL_VPSSZME_COEF_05 = 3,
    PQ_HAL_VPSSZME_COEF_033 = 4,
    PQ_HAL_VPSSZME_COEF_025 = 5,
    PQ_HAL_VPSSZME_COEF_0 = 6,

    PQ_HAL_VPSSZME_COEF_MAX
} pq_hal_vpsszme_coef_ratio;

extern hi_s16 g_vpsszme_hl_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_vpsszme_hc_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_vpsszme_vl_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_vpsszme_vc_coef[17][8]; /* 17:phase size, 8: tap size */

hi_void pq_hal_get_vpsszme_coef(hi_u32 offset, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq zme api define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_VPSSZME_H__
#define __PQ_MNG_VPSSZME_H__

#include <linux/string.h>

#include "hi_type.h"
#include "hi_reg_common.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PQ_HZME_PRECISION 1048576
#define PQ_VZME_PRECISION 4096
#define PQ_HZME_MAX_RATIO 16
#define PQ_VZME_MAX_RATIO 16

#define PQ_VPSSZME_WIDTH_4096 4096

#define PQ_VPSSZME_RATIO_4096 4096
#define PQ_VPSSZME_RATIO_3072 3072
#define PQ_VPSSZME_RATIO_2048 2048
#define PQ_VPSSZME_RATIO_1365 1365
#define PQ_VPSSZME_RATIO_1024 1024

typedef enum {
    VPSSZME_COEF_8T17P_LH = 0,
    VPSSZME_COEF_6T17P_LV,
    VPSSZME_COEF_8T17P_CH,
    VPSSZME_COEF_6T17P_CV,

    VPSSZME_COEF_TYPE_MAX
} pq_vpsszme_coef_type;

typedef enum {
    PQ_VPSSZME_COEF_BUF_HZME,
    PQ_VPSSZME_COEF_BUF_VZME,

    PQ_VPSSZME_COEF_BUF_MAX
} pq_vpsszme_coef_buf;

typedef struct {
    hi_u32 size;
    hi_u8 *vir_addr[PQ_VPSSZME_COEF_BUF_MAX + 1];
    hi_u64 phy_addr[PQ_VPSSZME_COEF_BUF_MAX + 1];
} pq_vpsszme_coef_addr;

typedef struct {
    hi_u32 ratio_h;
    hi_u32 ratio_v;
} pq_vpsszme_ratio;

typedef struct {
    hi_bool zme_init_flag;

    hi_bool zme_fir_en;
    hi_bool zme_med_en;
} pq_vpss_zme;

hi_s32 pq_mng_register_vpsszme(hi_void);

hi_s32 pq_mng_unregister_vpsszme(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

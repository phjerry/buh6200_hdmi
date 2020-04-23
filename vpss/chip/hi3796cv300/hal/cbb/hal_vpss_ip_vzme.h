/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_ip_vzme.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __HAL_VPSS_IP_VZME_V__
#define __HAL_VPSS_IP_VZME_V__

#include "hal_vpss_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VPSS_VZME_TYP = 0,
    VPSS_VZME_TYP1,
    VPSS_VZME_RAND,
    VPSS_VZME_MAX
} vpss_vzme_mode;

typedef struct {
    hi_u64 in_height;
    hi_u64 in_width;
    hi_u64 out_width;
    hi_u64 out_height;
    hi_u32 in_fmt;
    hi_u32 out_fmt;
    hi_u32 ck_gt_en;

    hi_u32 lv_fir_en;
    hi_u32 cv_fir_en;
    hi_u32 lv_fir_mode;
    hi_u32 cv_fir_mode;
} vpss_vzme_cfg;

typedef struct {
    hi_u32 lv_stc2nd_en;
    hi_u32 cv_stc2nd_en;
    hi_u32 lv_med_en;
    hi_u32 cv_med_en;
    hi_u32 lvfir_offset;
    hi_u32 cvfir_offset;
    hi_u32 lvfir_boffset;
    hi_u32 cvfir_boffset;
} vpss_vzme_pq;

hi_void vpss_func_set_vzme_mode(vpss_reg_type *vpss_reg, hi_u32 id, vpss_vzme_mode vzme_mode,
                                vpss_vzme_cfg *cfg, vpss_vzme_pq *pq_cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif




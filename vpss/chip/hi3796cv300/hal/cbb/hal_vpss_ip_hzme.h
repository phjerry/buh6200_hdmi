/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_ip_hzme.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/03
 */
#ifndef __HAL_VPSS_IP_HZME_H__
#define __HAL_VPSS_IP_HZME_H__

#include "hal_vpss_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VPSS_HZME_TYP = 0,
    VPSS_HZME_TYP1,
    VPSS_HZME_ZERO,
    VPSS_HZME_MAX
} vpss_hzme_mode;

typedef struct {
    hi_u64 in_height;
    hi_u64 in_width;
    hi_u64 out_width;
    hi_u64 out_height;
    hi_u32 out_fmt;
    hi_u32 ck_gt_en;

    hi_u32 lh_fir_mode;
    hi_u32 lh_fir_en;
    hi_u32 ch_fir_mode;
    hi_u32 ch_fir_en;
} vpss_hzme_cfg;

typedef struct {
    hi_u32 lh_stc2nd_en;
    hi_u32 ch_stc2nd_en;
    hi_u32 lh_med_en;
    hi_u32 ch_med_en;
    hi_u32 nonlinear_scl_en;
    hi_u32 lhfir_offset;
    hi_u32 chfir_offset;
} vpss_hzme_pq;

hi_void vpss_func_set_hzme_mode(vpss_reg_type *vpss_regs, hi_u32 id, vpss_hzme_mode hzme_mode,
                                vpss_hzme_cfg *cfg, vpss_hzme_pq *pq_cfg);
typedef struct {
    hi_u32 start_step;
    hi_u32 l_zone_end_pos;
    hi_u32 m_zone_end_pos;
    hi_s32 l_zone_delta;
    hi_s32 r_zone_delta;
    hi_bool enable;
} pic_zme_nonlinear;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif




/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description:
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_MNG_GFXCSC_H__
#define __PQ_MNG_GFXCSC_H__

#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CSC_PRE          (1 << 10)
#define PQ_GFX_TABLE_MAX 61
#define PQ_MULT_10000 10000
#define PQ_MULT_100000 100000
#define PQ_MULT_1000 1000
#define PQ_MULT_1000000 1000000
/* CSC 参数结构 */
typedef struct {
    hi_u32 bright;  /* bright adjust value,range[0,100],default setting 50 */
    hi_u32 contrst; /* contrast adjust value,range[0,100],default setting 50 */
    hi_u32 hue;     /* hue adjust value,range[0,100],default setting 50 */
    hi_u32 satur;   /* saturation adjust value,range[0,100],default setting 50 */
    hi_u32 wcg_temperature;   /* saturation adjust value,range[0,100],default setting 50 */
    hi_u32 kr;      /* red   component gain adjust value for color temperature adjust,range[0,100],default setting 50 */
    hi_u32 kg;      /* green component gain adjust value for color temperature adjust,range[0,100],default setting 50 */
    hi_u32 kb;      /* blue  component gain adjust value for color temperature adjust,range[0,100],default setting 50 */
} pq_gfx_csc_tuning;

typedef struct {
    hi_u32 csc_coef_pre;        /* csc 系数精度 */
    hi_u32 csc_in_dc_coef_pre;  /* csc 直流分量精度 */
    hi_u32 csc_out_dc_coef_pre; /* csc 直流分量精度 */
} pq_gfx_csc_pre;

hi_s32 pq_mng_register_gfx_csc(hi_void);

hi_s32 pq_mng_unregister_gfx_csc(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

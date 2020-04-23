/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend math function
 * Author: SDK
 * Created: 2017-06-30
 */
#ifndef __DRV_FE_MATH_H__
#define __DRV_FE_MATH_H__

#include "hi_drv_frontend.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

/* 100 * log10(in_value) */
hi_u32 drv_fe_log10_convert(hi_u32 in_value);

/* 100 * log10(a/b) */
hi_s32 drv_fe_div_log10_convert(hi_u32 a, hi_u32 b);

/* conver a/b to scientific counting */
hi_void drv_fe_ber_convert(hi_u32 a, hi_u32 b, hi_drv_frontend_scientific_num *num);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif  /* __DRV_FE_MATH_H__ */

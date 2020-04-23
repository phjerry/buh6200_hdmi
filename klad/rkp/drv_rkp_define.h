/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Define public macros for klad drivers.
 * Author: Linux SDK team
 * Create: 2019/06/24
 */
#ifndef __RKP_DEFINE_H__
#define __RKP_DEFINE_H__

#include "drv_klad_com.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#ifdef HI_TEE_SUPPORT
#define  RKP_IRQ_NUMBER            (179 + 32)
#define  RKP_IRQ_NAME              "int_rkp_ree"
#else
#define  RKP_IRQ_NUMBER            (178 + 32)
#define  RKP_IRQ_NAME              "int_rkp_tee"
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __RKP_DEFINE_H__ */

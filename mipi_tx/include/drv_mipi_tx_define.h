/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: mipi define module
* Author: sdk
* Create: 2019-11-20
*/
#ifndef __DRV_MIPI_TX_DEFINE_H__
#define __DRV_MIPI_TX_DEFINE_H__

#include "hi_type.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef HI_LOG_SUPPORT
#define hi_fatal_mipi(fmt...)   HI_FATAL_PRINT(HI_ID_MIPI, fmt)
#define hi_err_mipi(fmt...)     HI_ERR_PRINT(HI_ID_MIPI, fmt)
#define hi_warn_mipi(fmt...)    HI_WARN_PRINT(HI_ID_MIPI, fmt)
#define hi_info_mipi(fmt...)    HI_INFO_PRINT(HI_ID_MIPI, fmt)
#define mipi_func_enter(fmt...) HI_DBG_PRINT(HI_ID_MIPI, "[ENTER]:%s\n", __FUNCTION__)
#define mipi_func_exit(fmt...)  HI_DBG_PRINT(HI_ID_MIPI, "[EXIT] :%s\n", __FUNCTION__)
#else
#define hi_fatal_mipi(fmt...)
#define hi_err_mipi(fmt...)
#define hi_warn_mipi(fmt...)
#define hi_info_mipi(fmt...)
#define mipi_func_enter(fmt...)
#define mipi_func_exit(fmt...)
#endif

#define mipi_err_print_point(val)             hi_err_mipi("%s = %p\n", #val, val)
#define mipi_print_err_code(err_code)       hi_err_mipi("return [0x%08x]\n", err_code)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

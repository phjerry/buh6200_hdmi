 /*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of hi_drv_ci
 * Author: ÕÔ¹ðºé z00268517
 * Create: 2014-08-02
 */

#ifndef __HI_DRV_CI_H__
#define __HI_DRV_CI_H__

#undef  LOG_D_MODULE_ID
#define LOG_D_MODULE_ID             HI_ID_CI
#define LOG_D_FUNCTRACE             (0)
#define LOG_D_UNFTRACE              (0)
#include "hi_debug.h"
#include "hi_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHECK_CIPORT_VALID(port) { \
        if (port >= HI_CI_PORT_MAX) { \
            HI_LOG_ERR("Invalid CI Port.\n"); \
            hi_err_print_u32(port); \
            return HI_ERR_CI_INVALID_PARA; \
        } \
    }

#define CHECK_PCCD_VALID(card) { \
        if (card >= HI_CI_PCCD_MAX) { \
            HI_LOG_ERR("Invalid Card Id.\n"); \
            hi_err_print_u32(card); \
            return HI_ERR_CI_INVALID_PARA; \
        } \
    }
#define HI_LOG_CHECK_PARAM(val)                        \
do                                                      \
{                                                       \
    if (val) {                                          \
        hi_err_print_err_code(HI_ERR_CI_INVALID_PARA);    \
        return HI_ERR_CI_INVALID_PARA;                  \
    }                                                   \
} while (0)

#define CHECK_EXIT(exp) \
    do {    \
        if ((ret = expr) != HI_SUCCESS) {   \
            hi_err_print_call_fun_err(expr, ret);   \
            goto exit__;    \
        }   \
    } while (0)

#define CHECK_RETURN(expr)                          \
    do {                                            \
        hi_s32 ret_ = (expr);                        \
        if (ret_ != HI_SUCCESS) {                   \
            hi_err_print_call_fun_err(expr, ret_);   \
            return;                                 \
        }                                           \
    } while (0)

#define CHECK_RETURN_RET(expr)                      \
    do {                                            \
        hi_s32 ret_ = (expr);                        \
        if (ret_ != HI_SUCCESS) {                   \
            hi_err_print_call_fun_err(expr, ret_);   \
            return ret_;                            \
        }                                           \
    } while (0)

#define CHECK_FUNC_RET(expr)                        \
    do {                                            \
        hi_s32 ret_ = (expr);                        \
        if (ret_ != HI_SUCCESS) {                   \
            hi_err_print_call_fun_err(expr, ret_);   \
            return ret_;                            \
        }                                           \
    } while (0)

#define MODULE_CI "HI_CI"

#ifdef __cplusplus
}
#endif
#endif


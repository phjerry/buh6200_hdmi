/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: debug file
 */

#ifndef __DRV_PMOC_DEBUG_H__
#define __DRV_PMOC_DEBUG_H__

#define LOG_MODULE_ID  HI_ID_PM
#define LOG_FUNC_TRACE 1

#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PMOC_CHECK_PARAM(value, ret) do{ \
    if (value) { \
        LOG_ERR_PRINT("ERR: %s\n", #value); \
        return (ret); \
    } \
} while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_PMOC_DEBUG_H__ */

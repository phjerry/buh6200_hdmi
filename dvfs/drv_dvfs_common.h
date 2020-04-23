/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: common file
 */

#ifndef __DRV_DVFS_COMMON_H__
#define __DRV_DVFS_COMMON_H__

#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define dvfs_check_param(value, ret) do{ \
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

#endif /* End of #ifndef __DRV_DVFS_COMMON_H__ */

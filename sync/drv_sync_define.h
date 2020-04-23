/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync define
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef _DRV_SYNC_DEFINE_H_
#define _DRV_SYNC_DEFINE_H_

#define SYNC_GET_ID(handle)       ((handle) & 0xffff)
#define SYNC_GET_MODID(handle)    (((handle) >> 24) & 0xff)

#define TIME_RATE                 1000
#define PERCENT_MAX_VALUE         100

#define HI_FATAL_SYNC(fmt...)  HI_FATAL_PRINT(HI_ID_SYNC, fmt)
#define HI_ERR_SYNC(fmt...)    HI_ERR_PRINT(HI_ID_SYNC, fmt)
#define HI_WARN_SYNC(fmt...)   HI_WARN_PRINT(HI_ID_SYNC, fmt)
#define HI_INFO_SYNC(fmt...)   HI_INFO_PRINT(HI_ID_SYNC, fmt)
#define HI_TRACE_SYNC(fmt...)  HI_TRACE(HI_TRACE_LEVEL_TRACE, HI_ID_SYNC, fmt)

#define LINE_LITERAL(x) #x
#define LINE_CALL(x) LINE_LITERAL(x)

#define RETURN_WHEN_ERROR(ret, description) do { \
    if ((ret) != HI_SUCCESS) { \
        HI_ERR_SYNC("[%s] failed, return 0x%x\n", LINE_CALL(description), (ret)); \
        return (ret); \
    } \
} while (0)

#define RETURN_WHEN_CHECK_FAIL(condition, ret) do { \
    if (!(condition)) { \
        HI_ERR_SYNC("Check [%s] failed, return 0x%x\n", LINE_CALL(condition), (ret)); \
        return (ret); \
    } \
} while (0)

#define CHECK_PARAM(condition) RETURN_WHEN_CHECK_FAIL(condition, HI_ERR_SYNC_INVALID_PARA)
#define CHECK_NULL(pointer) RETURN_WHEN_CHECK_FAIL((pointer != NULL), HI_ERR_SYNC_NULL_PTR)

#define SYNC_DRV_CHECK_HANDLE(sync) do { \
        if (SYNC_GET_MODID(sync) != HI_ID_SYNC) {\
            HI_ERR_SYNC("this is invalid handle.\n"); \
            return HI_ERR_SYNC_INVALID_PARA; \
        } \
        if (SYNC_GET_ID(sync) >= SYNC_MAX_NUM) { \
            HI_ERR_SYNC(" sync ID out of range.\n"); \
            return HI_ERR_SYNC_INVALID_PARA; \
        } \
    } while (0)

#endif

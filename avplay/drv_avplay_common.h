/******************************************************************************
  * Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
  * Description   :
  * Author        : Hisilicon multimedia software group
  * Create        : 2009/12/21
  * History       :
 *******************************************************************************/
#ifndef __DRV_AVPLAY_COMMON_H__
#define __DRV_AVPLAY_COMMON_H__

#ifdef AVPLAY_NOT_SUPPORT_OSAL
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/vmalloc.h>
#endif

#include "hi_debug.h"
#include "hi_type.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef AVPLAY_SUPPORT_DRV_MUTEX
typedef osal_mutex avplay_drv_mutex_t;
#define AVPLAY_MUTEX_INIT(mutex) osal_mutex_init(mutex)
#define AVPLAY_MUTEX_LOCK(mutex) osal_mutex_lock(mutex)
#define AVPLAY_MUTEX_UNLOCK(mutex) osal_mutex_unlock(mutex)
#define AVPLAY_MUTEX_DEINIT(mutex) osal_mutex_destory(mutex)
#else
typedef hi_bool avplay_drv_mutex_t;
#define AVPLAY_MUTEX_INIT(mutex)    *(mutex) = HI_FALSE
#define AVPLAY_MUTEX_DEINIT(mutex)  *(mutex) = HI_FALSE
#define AVPLAY_MUTEX_LOCK(mutex)    *(mutex) = HI_TRUE
#define AVPLAY_MUTEX_UNLOCK(mutex)  *(mutex) = HI_FALSE
#endif

typedef osal_spinlock avplay_drv_spin_t;
#define AVPLAY_SPIN_INIT(spin) osal_spin_lock_init(spin)
#define AVPLAY_SPIN_DEINIT(spin) osal_spin_lock_destory(spin)

#define AVPLAY_SPIN_LOCK(spin, flags) osal_spin_lock_irqsave(spin, &(flags))
#define AVPLAY_SPIN_UNLOCK(spin, flags) osal_spin_unlock_irqrestore(spin, &(flags))

#define AVPLAY_MALLOC(size) osal_vmalloc(HI_ID_AVPLAY, (size))
#define AVPLAY_FREE(addr)   osal_vfree(HI_ID_AVPLAY, (addr))

#define AVPLAY_GET_HANDLE(id)           ((HI_ID_AVPLAY << 24) | (id))
#define AVPLAY_GET_ID(handle)           ((handle) & 0xFFFF)
#define AVPLAY_GET_MODID(handle)        (((handle) >> 24) & 0xff)

#define HI_ASSERT_AVPLAY(condition) HI_ASSERT_RET(condition)

#define HI_FATAL_AVPLAY(fmt...)     HI_TRACE(HI_TRACE_LEVEL_FATAL, HI_ID_AVPLAY, fmt)
#define HI_ERR_AVPLAY(fmt...)       HI_TRACE(HI_TRACE_LEVEL_ERROR, HI_ID_AVPLAY, fmt)
#define HI_WARN_AVPLAY(fmt...)      HI_TRACE(HI_TRACE_LEVEL_WARN, HI_ID_AVPLAY, fmt)
#define HI_INFO_AVPLAY(fmt...)      HI_TRACE(HI_TRACE_LEVEL_INFO, HI_ID_AVPLAY, fmt)
#define HI_DBG_AVPLAY(fmt...)       HI_TRACE(HI_TRACE_LEVEL_DBG, HI_ID_AVPLAY, fmt)
#define HI_TRACE_AVPLAY(fmt...)     HI_TRACE(HI_TRACE_LEVEL_TRACE, HI_ID_AVPLAY, fmt)

#define LINE_LITERAL(x) #x
#define LINE_CALL(x) LINE_LITERAL(x)

#define LOG_WHEN_ERROR(ret, description) do { \
    if ((ret) != HI_SUCCESS) { \
        HI_ERR_AVPLAY("[%s] failed, errcode = 0x%x\n", LINE_CALL(description), (ret)); \
    } \
} while (0)

#define LOG_WHEN_CHECK_FAIL(condition) do { \
    if (!(condition)) { \
        HI_ERR_AVPLAY("Check [%s] failed\n", LINE_CALL(condition)); \
    } \
} while (0)

#define RETURN_WHEN_ERROR(ret, description) do { \
    if ((ret) != HI_SUCCESS) { \
        HI_ERR_AVPLAY("[%s] failed, return 0x%x\n", LINE_CALL(description), (ret)); \
        return (ret); \
    } \
} while (0)

#define RETURN_WHEN_CHECK_FAIL(condition, ret) do { \
    if (!(condition)) { \
        HI_ERR_AVPLAY("Check [%s] failed, return 0x%x\n", LINE_CALL(condition), (ret)); \
        return (ret); \
    } \
} while (0)

#define GOTO_WHEN_ERROR(ret, description, tag) do { \
    if ((ret) != HI_SUCCESS) { \
        HI_ERR_AVPLAY("[%s] failed, errcode = 0x%x, goto [%s]\n", LINE_CALL(description), (ret), LINE_CALL(tag)); \
        goto (tag); \
    } \
} while (0)

#define GOTO_WHEN_CTX_IS_NULL(ctx, ret, tag) do { \
    if ((ctx) == HI_NULL) { \
        HI_ERR_AVPLAY("Avplay context is NULL, maybe has been destoryed\n"); \
        (ret) = HI_ERR_AVPLAY_NULL_PTR; \
        goto (tag); \
    } \
} while (0)

#define GET_BIT(who, bit) (((who) >> (bit)) & (0x1))
#define IS_BIT_ENABLE(who, bit) (GET_BIT(who, bit) != 0)
#define HAS_FLAG(who, flag) (((hi_u32)(who) & (hi_u32)(flag)) != 0)
#define SET_FLAG(who, flag) (who) = (hi_u32)(who) | (hi_u32)(flag)
#define CLR_FLAG(who, flag) (who) = (hi_u32)(who) & (~(hi_u32)(flag))

#define BOOL2STR(flag)   ((flag) ? "TRUE" : "FALSE")
#define ERRCODE2STR(ret) (((ret) == HI_SUCCESS) ? "SUCCESS" : "FAIL")
#define CHECK_PARAM(condition) RETURN_WHEN_CHECK_FAIL(condition, HI_ERR_AVPLAY_INVALID_PARA)
#define CHECK_NULL(pointer) RETURN_WHEN_CHECK_FAIL((pointer != NULL), HI_ERR_AVPLAY_NULL_PTR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

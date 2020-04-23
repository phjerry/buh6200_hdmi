/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_OSAL_H__
#define __VMM_OSAL_H__

#include "vfmw_osal.h"

typedef enum {
    DEV_SCREEN = 1,
    DEV_SYSLOG,
    DEV_FILE,
    DEV_MEM
} vmm_print_device_type;

typedef enum {
    PRN_FATAL = 0,
    PRN_ERROR,
    PRN_WARN,
    PRN_IMP,
    PRN_TD,
    PRN_INFO,
    PRN_DBG,
    PRN_ALWS = 32
} vmm_print_type;

extern hi_u32 g_vmm_print_enable;

#ifndef HI_ADVCA_FUNCTION_RELEASE
#define PRINT(type, fmt, arg...)                                               \
    do {                                                                       \
        if ((type == PRN_ALWS) || ((g_vmm_print_enable & (1 << type)) != 0)) { \
            OS_PRINT(fmt, ##arg);                                              \
        }                                                                      \
    } while (0)
#else
#define PRINT(type, fmt, arg...)
#endif

#define VMM_PRIVATE static

#define POS() PRINT(PRN_ALWS, "############### %s,%d\n", __func__, __LINE__)

#define VMM_OSAL_SUCCESS         (hi_s32)(0)
#define VMM_OSAL_FAILURE         (hi_s32)(0xFFFFFFFF)
#define VMM_TRUE                 (hi_s32)(1)
#define VMM_FALSE                (hi_s32)(0)
#define VMM_DEFAULT_PRINT_ENABLE (PRN_FATAL | PRN_ERROR)
#define VMM_DEFAULT_PRINT_DEVICE (DEV_SCREEN)

#define D_VMM_CHECK_PTR_RET(ptr, value)      \
    do {                                     \
        if ((ptr) == HI_NULL) {                   \
            PRINT(PRN_ERROR, "PTR '%s' is HI_NULL.\n", #ptr); \
            return value;                    \
        }                                    \
    } while (0)

#define D_VMM_CHECK_PTR(ptr)                 \
    do {                                     \
        if ((ptr) == HI_NULL) {                   \
            PRINT(PRN_ERROR, "PTR '%s' is HI_NULL.\n", #ptr); \
            return;                          \
        }                                    \
    } while (0)

#define VMM_ASSERT(condition)                              \
    do {                                                   \
        if (!(condition)) {                                \
            PRINT(PRN_ERROR, "[%s %d]assert warning\n", __func__, __LINE__); \
        }                                                  \
    } while (0)

#define D_VMM_ASSERT_RET(condition, value)                 \
    do {                                                   \
        if (!(condition)) {                                \
            PRINT(PRN_ERROR, "[%s %d]assert Error\n", __func__, __LINE__); \
            return value;                                  \
        }                                                  \
    } while (0)

#define VMM_MUTEX_INIT(mutex) \
    do {                      \
        OS_SEMA_INIT(mutex);  \
    } while (0)

#define VMM_MUTEX_LOCK(mutex)       \
    do {                            \
        hi_s32 ret;                 \
        ret = OS_SEMA_DOWN(*mutex); \
    } while (0)

#define VMM_MUTEX_UNLOCK(mutex)    \
    do {                           \
            (hi_void)OS_SEMA_UP(*mutex); \
    } while (0)

#define VMM_MUTEX_EXIT(mutex) \
    do {                      \
        OS_SEMA_EXIT(*mutex); \
    } while (0)

#define VMM_SPINLOCK_INIT(lock)  \
    do {                         \
        OS_SPIN_LOCK_INIT(lock); \
    } while (0)

#define VMM_SPINLOCK_LOCK(lock, flags) \
    do {                               \
        OS_SPIN_LOCK(*lock, flags);    \
    } while (0)

#define VMM_SPINLOCK_UNLOCK(lock, flags) \
    do {                                 \
        OS_SPIN_UNLOCK(*lock, flags);    \
    } while (0)

#define VMM_SPINLOCK_EXIT(lock)   \
    do {                          \
        OS_SPIN_LOCK_EXIT(*lock); \
    } while (0)

#define VMM_EVENT_INIT(event, init_value) \
    do {                                  \
        OS_EVENT_INIT(event, init_value); \
    } while (0)

#define VMM_EVENT_GIVE(event)  \
    do {                       \
        OS_EVENT_GIVE(*event); \
    } while (0)

#define VMM_EVENT_WAIT(event, time_ms)  \
    do {                                \
        OS_EVENT_WAIT(*event, time_ms); \
    } while (0)

#define VMM_EVENT_EXIT(event)  \
    do {                       \
        OS_EVENT_EXIT(*event); \
    } while (0)

#endif /* __VMM_OSAL_H__ */

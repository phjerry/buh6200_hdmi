/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __DRV_VDP_DEBUG_H__
#define __DRV_VDP_DEBUG_H__

#include "hi_type.h"
#include "hi_drv_module.h"
#include "drv_win_common.h"
#include "hi_drv_mem.h"
#include "hi_osal.h"

#if defined(__DISP_PLATFORM_BOOT__)
#include <uboot.h>
#include "hi_error_mpi.h"
#include "hi_common.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#if defined(__DISP_PLATFORM_SDK__)
#define DISP_PRINT(fmt...) \
    HI_INFO_PRINT(HI_ID_DISP, fmt)

#define DISP_FATAL(fmt...) \
    HI_FATAL_PRINT(HI_ID_DISP, fmt)

#define DISP_ERROR(fmt...) \
    HI_ERR_PRINT(HI_ID_DISP, fmt)

#define DISP_WARN(fmt...) \
    HI_WARN_PRINT(HI_ID_DISP, fmt)

#define DISP_INFO(fmt...) \
    HI_INFO_PRINT(HI_ID_DISP, fmt)

#define DISP_FATAL_RETURN()                          \
    do {                                             \
        HI_FATAL_PRINT(HI_ID_DISP, "\n"); \
        return HI_FAILURE;                           \
    } while (0)

#define WIN_FATAL(fmt...) \
    HI_FATAL_PRINT(HI_ID_WIN, fmt)

#define WIN_ERROR(fmt...) \
    HI_ERR_PRINT(HI_ID_WIN, fmt)

#define WIN_WARN(fmt...) \
    HI_WARN_PRINT(HI_ID_WIN, fmt)

#define WIN_INFO(fmt...) \
    HI_INFO_PRINT(HI_ID_WIN, fmt)
#define DISP_ASSERT(exp) HI_ASSERT(exp)

#define DISP_MALLOC(a) ({                                     \
    hi_void *b = 0;                                           \
    if (a > VDP_MEM_MAX) {                                    \
        DISP_ERROR("alloc mem size (%d) more than MAX\n", a); \
    } else {                                                  \
        b = osal_vmalloc(HI_ID_DISP, a);                      \
        if (b) {memset((hi_void *)b, 0, a);}                 \
    }                                                         \
    b;                                                        \
})


#define DISP_FREE(a)                  \
    do {                              \
        if (a) {                      \
            osal_vfree(HI_ID_DISP, a); \
            a = HI_NULL;              \
        }                             \
    } while (0)

#define DISP_MEMSET(a, b, c) memset_s(a, c, b, c)
#define disp_msleep(a) osal_msleep_uninterruptible(a)
#define disp_udelay(a)       osal_udelay(a)
#define DISP_DSB() dsb()

#elif defined(__DISP_PLATFORM_BOOT__)
#define DISP_PRINT(fmt...) HI_INFO_PRINT(HI_ID_DISP, fmt)
#define DISP_FATAL(fmt...) HI_ERR_PRINT(HI_ID_DISP, fmt)
#define DISP_ERROR(fmt...) HI_ERR_PRINT(HI_ID_DISP, fmt)
#define DISP_WARN(fmt...) HI_INFO_PRINT(HI_ID_DISP, fmt)
#define DISP_INFO(fmt...) HI_INFO_PRINT(HI_ID_DISP, fmt)

#define DISP_FATAL_RETURN()     \
    do {                        \
        printf("\n"); \
        return HI_FAILURE;      \
    } while (0)

#define DISP_MALLOC(a) ({                                     \
    hi_void *b = 0;                                           \
    if (a > VDP_MEM_MAX) {                                    \
        DISP_ERROR("alloc mem size (%d) more than MAX\n", a); \
    } else {                                                  \
        b = malloc(a);                                        \
        if (b) {memset((hi_void *)b, 0, a);}                 \
    }                                                         \
    b;                                                        \
})

#define DISP_FREE(a)         free
#define DISP_MEMSET(a, b, c) memset(a, b, c)
#define disp_msleep(a)       osal_udelay(a * 1000)
#define disp_udelay(a)       osal_udelay(a)
#endif

#ifndef __DISP_PLATFORM_BOOT__
typedef spinlock_t disp_osal_spin;
typedef struct osal_list_head disp_osal_list;
hi_s32 disp_osal_init_spin(disp_osal_spin *pLock);
hi_s32 disp_osal_down_spin(disp_osal_spin *pLock, unsigned long *flags);
hi_s32 disp_osal_upspin(disp_osal_spin *pLock, unsigned long *flags);
hi_s32 disp_osal_try_lockspin(disp_osal_spin *pLock, unsigned long *flags);
#endif

typedef enum {
    AVSYNC_INFOR     = 1 << 1,
    BUFFER_INFOR     = 1 << 2,
    VPSS_INFOR       = 1 << 3,
    WINATTR_INFOR    = 1 << 4,
    IOCTL_INFOR      = 1 << 5,
    AI_WIN_INFOR     = 1 << 6,
    VIRT_WIN_INFOR   = 1 << 7,
    FILE_INFOR       = 1 << 8,
    ANDROID_FENCE_INFOR  = 1 << 9,
    CHECK_SUM       = 1 << 10,
    BUFFER_FENCE_INFOR  = 1 << 11,
    HDR_INFOR  = 1 << 12,
    DEBUG_MASK_MAX
} debug_mask_type;

typedef enum {
    INFOR_OUTPUT,
    WARN_OUTPUT,
    ERROR_OUTPUT,
    FATAL_OUTPUT,
    OUTPUT_MAX
} debug_output_level;

#define ISR_SYSTEM_TIME_US 1000
#define WIN_MASK_VALUE  (AVSYNC_INFOR |\
                         BUFFER_INFOR |\
                         VPSS_INFOR |\
                         WINATTR_INFOR |\
                         IOCTL_INFOR |\
                         AI_WIN_INFOR |\
                         VIRT_WIN_INFOR |\
                         ANDROID_FENCE_INFOR)

hi_void disp_set_debug_flag(hi_u32 u32mask,hi_bool close_print);
hi_u64 hi_drv_sys_get_time_stamp_us(hi_void);
extern hi_u32 g_debug_mask;

#define COMMON_DEBUG(FUNCTION, fmt...) do {\
    if ((FUNCTION & g_debug_mask) != 0) { \
            printk(fmt);\
    }\
}while(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_disp_osal_H__ */



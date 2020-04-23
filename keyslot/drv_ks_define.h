/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Define public macros for keyslot drivers.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KS_DEFINE_H__
#define __DRV_KS_DEFINE_H__

#include "linux/hisilicon/securec.h"
#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_osal.h"
#include "hi_osal.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define HI_DEBUG_KS(fmt...)              HI_DBG_PRINT(HI_ID_KEYSLOT, fmt)
#define HI_FATAL_KS(fmt...)              HI_FATAL_PRINT(HI_ID_KEYSLOT, fmt)
#define HI_ERR_KS(fmt...)                HI_ERR_PRINT(HI_ID_KEYSLOT, fmt)
#define HI_WARN_KS(fmt...)               HI_WARN_PRINT(HI_ID_KEYSLOT, fmt)
#define HI_INFO_KS(fmt...)               HI_INFO_PRINT(HI_ID_KEYSLOT, fmt)

#define KS_FUNC_ENTER()                  HI_DEBUG_KS("[ENTER]:%s\n", __FUNCTION__)
#define KS_FUNC_EXIT()                   HI_DEBUG_KS("[EXIT] :%s\n", __FUNCTION__)

#define print_dbg_hex(val)               HI_INFO_KS("%s = 0x%08x\n", #val, val)
#define print_dbg_hex2(x, y)             HI_INFO_KS("%s = 0x%08x %s = 0x%08x\n", #x, x, #y, y)
#define print_dbg_hex3(x, y, z)          HI_INFO_KS("%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #x, x, #y, y, #z, z)

#define print_err_hex(val)               HI_ERR_KS("%s = 0x%08x\n", #val, val)
#define print_err_hex2(x, y)             HI_ERR_KS("%s = 0x%08x %s = 0x%08x\n", #x, x, #y, y)
#define print_err_hex3(x, y, z)          HI_ERR_KS("%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #x, x, #y, y, #z, z)
#define print_err_hex4(w, x, y, z)       HI_ERR_KS("%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #w, \
                                                     w, #x, x, #y, y, #z, z)

#define print_dbg_func_hex(func, val)    HI_INFO_KS("call [%s]%s = 0x%08x\n", #func, #val, val)
#define print_dbg_func_hex2(func, x, y)  HI_INFO_KS("call [%s]%s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y)
#define print_dbg_func_hex3(func, x, y, z) \
    HI_INFO_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y, #z, z)
#define print_dbg_func_hex4(func, w, x, y, z) \
    HI_INFO_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #w,  w, #x, x, #y, y, #z, z)

#define print_err_func_hex(func, val)    HI_ERR_KS("call [%s]%s = 0x%08x\n", #func, #val, val)
#define print_err_func_hex2(func, x, y)  HI_ERR_KS("call [%s]%s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y)
#define print_err_func_hex3(func, x, y, z) \
    HI_ERR_KS("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y, #z, z)
#define print_err_func_hex4(func, w, x, y, z) \
    HI_ERR_KS("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #w,  w, #x, x, #y, y, #z, z)

#define dbg_print_dbg_hex(val)           HI_DEBUG_KS("%s = 0x%08x\n", #val, val)
#define print_err_val(val)               HI_ERR_KS("%s = %d\n", #val, val)
#define print_err_point(val)             HI_ERR_KS("%s = %p\n", #val, val)
#define print_err_code(err_code)         HI_ERR_KS("return [0x%08x]\n", err_code)
#define print_warn_code(err_code)        HI_WARN_KS("return [0x%08x]\n", err_code)
#define print_err_func(func, err_code)   HI_ERR_KS("call [%s] return [0x%08x]\n", #func, err_code)

/* it depends on how many pages can be maped, 0:one page, 1:two pages, 2:four pages, 3: eight pages */
#define PAGE_NUM_SHIFT   0
#define KS_MAP_PAGE_NUM  (1ULL << PAGE_NUM_SHIFT)
#define KS_MAP_MASK      (((KS_MAP_PAGE_NUM) << PAGE_SHIFT) - 1)
#define KS_MAP_SIZE      0x10000

/* Register read and write */
#define reg_read(addr, result)  ((result) = *(volatile unsigned int *)(uintptr_t)(addr))
#define reg_write(addr, result)  (*(volatile unsigned int *)(uintptr_t)(addr) = (result))

#define hi_malloc(x)                        (0 < (x) ? osal_kmalloc(HI_ID_KEYSLOT, x, GFP_KERNEL) : HI_NULL)
#define hi_free(x)                          { if (HI_NULL != (x))  osal_kfree(HI_ID_KEYSLOT, x); }

#ifndef __mutex_lock
#define __mutex_lock(lock) \
    do { \
        HI_DEBUG_KS("[DRV]%s[%d] %s lock\n", __FUNCTION__, __LINE__, #lock); \
        osal_mutex_lock(lock); \
    } while (0)
#define __mutex_unlock(lock) \
    do { \
        HI_DEBUG_KS("[DRV]%s[%d] %s unlock\n", __FUNCTION__, __LINE__, #lock); \
        osal_mutex_unlock(lock); \
    } while (0)
#endif

/*
 * HANDLE macro
 */
#define ID_2_HANDLE(id, key) HI_HANDLE_INIT(HI_ID_KEYSLOT, (key), (id))
#define HANDLE_2_ID(handle)  HI_HANDLE_GET_CHAN_ID(handle)
#define HANDLE_2_TYPE(handle) HI_HANDLE_GET_PRIVATE_DATA(handle)

#define is_invalid_handle(handle) ({\
    hi_bool ret_ = HI_FALSE; \
    if (HI_HANDLE_GET_MODULE_ID(handle) != HI_ID_KEYSLOT) \
        ret_ = HI_TRUE; \
    ret_; \
})

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __DRV_KS_H__ */

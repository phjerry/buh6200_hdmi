/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Define public macros for klad drivers.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KLAD_COM_H__
#define __DRV_KLAD_COM_H__

#include <linux/kernel.h>
#include <asm/current.h>
#include "linux/hisilicon/securec.h"
#include "hi_drv_sys.h"
#include "hi_drv_osal.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_drv_keyslot.h"
#include "hi_osal.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define HI_DBG_KLAD(fmt, args...)         HI_DBG_PRINT(HI_ID_KLAD, "[%04d %04d]" fmt, \
                                              osal_get_pid(), (hi_u32)(task_pid_vnr(current)),\
                                              ##args)
#define HI_FATAL_KLAD(fmt, args...)       HI_FATAL_PRINT(HI_ID_KLAD, "[%04d %04d]" fmt, \
                                              osal_get_pid(), (hi_u32)(task_pid_vnr(current)),\
                                              ##args)
#define HI_ERR_KLAD(fmt, args...)         HI_ERR_PRINT(HI_ID_KLAD, "[%04d %04d]" fmt, \
                                              osal_get_pid(), (hi_u32)(task_pid_vnr(current)),\
                                              ##args)
#define HI_WARN_KLAD(fmt, args...)        HI_WARN_PRINT(HI_ID_KLAD, "[%04d %04d]" fmt, \
                                              osal_get_pid(), (hi_u32)(task_pid_vnr(current)),\
                                              ##args)
#define HI_INFO_KLAD(fmt, args...)        HI_INFO_PRINT(HI_ID_KLAD, "[%04d %04d]" fmt, \
                                              osal_get_pid(), (hi_u32)(task_pid_vnr(current)),\
                                              ##args)

#define KLAD_FUNC_ENTER()                HI_DBG_KLAD("[ENTER]:%s\n", __FUNCTION__)
#define KLAD_FUNC_EXIT()                 HI_DBG_KLAD("[EXIT] :%s\n", __FUNCTION__)

#define print_err(val)                   HI_ERR_KLAD("%s\n", val)

#define print_dbg_hex(val)               HI_DBG_KLAD("%s = 0x%08x\n", #val, val)
#define print_dbg_hex2(x, y)             HI_DBG_KLAD("%s = 0x%08x %s = 0x%08x\n", #x, x, #y, y)
#define print_dbg_hex3(x, y, z)          HI_DBG_KLAD("%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #x, x, #y, y, #z, z)
#define print_dbg_hex4(w, x, y, z)       HI_DBG_KLAD("%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #w, \
                                                     w, #x, x, #y, y, #z, z)

#define print_err_hex(val)               HI_ERR_KLAD("%s = 0x%08x\n", #val, val)
#define print_err_hex2(x, y)             HI_ERR_KLAD("%s = 0x%08x %s = 0x%08x\n", #x, x, #y, y)
#define print_err_hex3(x, y, z)          HI_ERR_KLAD("%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #x, x, #y, y, #z, z)
#define print_err_hex4(w, x, y, z)       HI_ERR_KLAD("%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #w, \
                                                     w, #x, x, #y, y, #z, z)

#define print_dbg_func_hex(func, val)    HI_DBG_KLAD("call [%s]%s = 0x%08x\n", #func, #val, val)
#define print_dbg_func_hex2(func, x, y)  HI_DBG_KLAD("call [%s]%s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y)
#define print_dbg_func_hex3(func, x, y, z) \
    HI_DBG_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y, #z, z)
#define print_dbg_func_hex4(func, w, x, y, z) \
    HI_DBG_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #w,  w, #x, x, #y, y, #z, z)

#define print_err_func_hex(func, val)    HI_ERR_KLAD("call [%s]%s = 0x%08x\n", #func, #val, val)
#define print_err_func_hex2(func, x, y)  HI_ERR_KLAD("call [%s]%s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y)
#define print_err_func_hex3(func, x, y, z) \
    HI_ERR_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y, #z, z)
#define print_err_func_hex4(func, w, x, y, z) \
    HI_ERR_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #w,  w, #x, x, #y, y, #z, z)

#define dbg_print_dbg_hex(val)           HI_DBG_KLAD("%s = 0x%08x\n", #val, val)
#define print_err_val(val)               HI_ERR_KLAD("%s = %d\n", #val, val)
#define print_err_point(val)             HI_ERR_KLAD("%s = %p\n", #val, val)
#define print_err_code(err_code)         HI_ERR_KLAD("return [0x%08x]\n", err_code)
#define print_warn_code(err_code)        HI_WARN_KLAD("return [0x%08x]\n", err_code)
#define print_err_func(func, err_code)   HI_ERR_KLAD("call [%s] return [0x%08x]\n", #func, err_code)

/* Register read and write */
#define reg_read(addr, result)  ((result) = *(volatile unsigned int *)(uintptr_t)(addr))
#define reg_write(addr, result)  (*(volatile unsigned int *)(uintptr_t)(addr) = (result))

#define hi_malloc(x)                        (0 < (x) ? HI_KZALLOC(HI_ID_KLAD, x, GFP_KERNEL) : HI_NULL)
#define hi_free(x)                          { if (HI_NULL != (x))  HI_KFREE(HI_ID_KLAD, x); }

#ifndef __mutex_lock
#define __mutex_lock(lock) \
    do { HI_DBG_KLAD("[DRV]%s[%d] %s lock\n", __FUNCTION__, __LINE__, #lock); \
        osal_mutex_lock(lock); \
    } while (0)
#define __mutex_unlock(lock) \
    do { HI_DBG_KLAD("[DRV]%s[%d] %s unlock\n", __FUNCTION__, __LINE__, #lock); \
        osal_mutex_unlock(lock); \
    } while (0)
#endif

#define TIME_S2MS  1000
#define TIME_S2US  1000000
#define TIME_S2NS  1000000000
#define TIME_MS2US 1000
#define TIME_MS2NS 1000000
#define TIME_US2NS 1000

#define MODULE_ID_CNT         0x04

#define HANDLE_2_ID(handle)   HI_HANDLE_GET_CHAN_ID(handle)
#define HANDLE_2_MOD(handle)  HI_HANDLE_GET_MODULE_ID(handle)
#define HANDLE_2_TYPE(handle) HI_HANDLE_GET_PRIVATE_DATA(handle)

#define is_invalid_ks_handle(handle) ({\
    hi_bool ret_ = HI_FALSE; \
    if (HANDLE_2_MOD(handle) != HI_ID_KEYSLOT) \
        ret_ = HI_TRUE; \
    ret_; \
})

#define is_invalid_klad_handle(handle) ({\
    hi_bool ret_ = HI_FALSE; \
    if (HANDLE_2_MOD(handle) != HI_ID_KLAD) \
        ret_ = HI_TRUE; \
    ret_; \
})

#define is_tscipher_ks_handle(handle) ({\
    hi_bool ret_ = HI_FALSE; \
    if (HANDLE_2_TYPE(handle) == HI_KEYSLOT_TYPE_TSCIPHER) \
        ret_ = HI_TRUE; \
    ret_; \
})

#define is_mcipher_ks_handle(handle) ({\
    hi_bool ret_ = HI_FALSE; \
    if (HANDLE_2_TYPE(handle) == HI_KEYSLOT_TYPE_MCIPHER) \
        ret_ = HI_TRUE; \
    ret_; \
})

#define unused(x) (x) = (x)

#define RKP_ERR 0x9 /* RKP error return 0x9x000000 */
#define KL_ERR  0xa /* Keyladder error return 0xax000000 */
#define KC_ERR  0xb /* Key ctrl error return 0xax000000 */

/*
* record logic err code.
* bit 31~28: error module type
* bit 27~24: error bit number, only record 6 bit at most.
* bit 23~0: record error bit position.
*           1):position in bit0~bit15, record the real position.
*           2):position in bit16~bit31, record the position is real position minus 16.
*  eg:  RKP error register is 0b0000 0000 0000 0001 0100 0001 0000 1010, return 0x9500e831.
*/
static inline hi_s32 err_code_gen(hi_u8 mod, hi_u32 reg)
{
    hi_s32 i;
    hi_s32 cnt;
    hi_u32 err_code = mod << 28; /* bit 31~28 indicate module type */

    for (i = 0, cnt = 0; (i < 0x20) && (cnt < 0x07); i++) {
        if ((reg & (1 << i)) == 0) {
            continue;
        }
        err_code |= ((i % 0x10) << (cnt * 0x04));
        cnt++;
    }
    err_code |= (cnt << 24); /* bit 27~24 indicate err bit number, record 6 bit at most. */
    return (hi_s32)err_code;
}

struct time_ns {
    hi_ulong tv_sec;
    hi_ulong tv_nsec;
};
hi_void timestamp(struct time_ns *time);
hi_void get_cost(hi_char *str, struct time_ns *time_b, struct time_ns *time_e);
hi_void get_curr_cost(hi_char *str, struct time_ns *time_b);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KLAD_COM_H__

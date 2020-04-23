/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Define public macros for OTP drivers.
 * Author: Linux SDK team
 * Create: 2019/06/20
 */
#ifndef __DRV_OTP_DEFINE_H__
#define __DRV_OTP_DEFINE_H__

#include "linux/hisilicon/securec.h"
#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_osal.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define HI_DEBUG_OTP(fmt...)             HI_DBG_PRINT(HI_ID_OTP, fmt)
#define HI_FATAL_OTP(fmt...)             HI_FATAL_PRINT(HI_ID_OTP, fmt)
#define HI_ERR_OTP(fmt...)               HI_ERR_PRINT(HI_ID_OTP, fmt)
#define HI_WARN_OTP(fmt...)              HI_WARN_PRINT(HI_ID_OTP, fmt)
#define HI_INFO_OTP(fmt...)              HI_INFO_PRINT(HI_ID_OTP, fmt)

#define OTP_FUNC_ENTER()                 HI_DEBUG_OTP("[ENTER]:%s\n", __FUNCTION__)
#define OTP_FUNC_EXIT()                  HI_DEBUG_OTP("[EXIT] :%s\n", __FUNCTION__)

#define print_dbg_hex(val)               HI_INFO_OTP("%s = 0x%08x\n", #val, val)
#define print_dbg_hex2(x, y)             HI_INFO_OTP("%s = 0x%08x %s = 0x%08x\n", #x, x, #y, y)
#define print_dbg_hex3(x, y, z)          HI_INFO_OTP("%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #x, x, #y, y, #z, z)

#define print_err_hex(val)               HI_ERR_OTP("%s = 0x%08x\n", #val, val)
#define print_err_hex2(x, y)             HI_ERR_OTP("%s = 0x%08x %s = 0x%08x\n", #x, x, #y, y)
#define print_err_hex3(x, y, z)          HI_ERR_OTP("%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #x, x, #y, y, #z, z)
#define print_err_hex4(w, x, y, z)       HI_ERR_OTP("%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #w, \
                                                     w, #x, x, #y, y, #z, z)

#define print_dbg_func_hex(func, val)    HI_INFO_OTP("call [%s]%s = 0x%08x\n", #func, #val, val)
#define print_dbg_func_hex2(func, x, y)  HI_INFO_OTP("call [%s]%s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y)
#define print_dbg_func_hex3(func, x, y, z) \
    HI_INFO_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y, #z, z)
#define print_dbg_func_hex4(func, w, x, y, z) \
    HI_INFO_KLAD("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #w,  w, #x, x, #y, y, #z, z)

#define print_err_func_hex(func, val)    HI_ERR_OTP("call [%s]%s = 0x%08x\n", #func, #val, val)
#define print_err_func_hex2(func, x, y)  HI_ERR_OTP("call [%s]%s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y)
#define print_err_func_hex3(func, x, y, z) \
    HI_ERR_OTP("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #x, x, #y, y, #z, z)
#define print_err_func_hex4(func, w, x, y, z) \
    HI_ERR_OTP("call [%s]%s = 0x%08x %s = 0x%08x %s = 0x%08x %s = 0x%08x\n", #func, #w,  w, #x, x, #y, y, #z, z)

#define dbg_print_dbg_hex(val)           HI_DEBUG_OTP("%s = 0x%08x\n", #val, val)
#define print_err_val(val)               HI_ERR_OTP("%s = %d\n", #val, val)
#define print_err_point(val)             HI_ERR_OTP("%s = %p\n", #val, val)
#define print_err_code(err_code)         HI_ERR_OTP("return [0x%08x]\n", err_code)
#define print_warn_code(err_code)        HI_WARN_OTP("return [0x%08x]\n", err_code)
#define print_err_func(func, err_code)   HI_ERR_OTP("call [%s] return [0x%08x]\n", #func, err_code)


/* it depends on how many pages can be maped, 0:one page, 1:two pages, 2:four pages, 3: eight pages */
#define PAGE_NUM_SHIFT    0
#define OTP_MAP_PAGE_NUM  (1ULL << PAGE_NUM_SHIFT)
#define OTP_MAP_MASK      (((OTP_MAP_PAGE_NUM) << PAGE_SHIFT) - 1)
#define OTP_MAP_SIZE      0x10000

/* Bit map
GENMASKREV(05, 00)=0xffffffc0 GENMASK(05, 00)=0x0000003f
GENMASKREV(32, 05)=0x0000001f GENMASK(32, 05)=0xffffffe0
GENMASKREV(00, 01)=0xffffffff GENMASK(00, 01)=0x00000000
*/
#define BITS_PER_INT 32
#define GEN_MASK(h, l) \
    (((~0U) << (l)) & (~0U >> (BITS_PER_INT - 1 - (h))))
#define GEN_MASK_REV(h, l) \
    ~(((~0U) << (l)) & (~0U >> (BITS_PER_INT - 1 - (h))))

/* Byte alignment */
#define IS_WORD_ALIGN(addr) (((addr) & 0x03) != 0)
#define WORD_ALIGN(addr) ((addr) & (~0x3))
#define BYTE_GEN(v, addr) (((v) >> (((addr) & 0x3) * 8)) & 0xff)

/* Register read and write */
#define reg_read(addr, result)  ((result) = *(volatile unsigned int *)(uintptr_t)(addr))
#define reg_write(addr, result)  (*(volatile unsigned int *)(uintptr_t)(addr) = (result))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __DRV_OTP_H__ */

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 * Author : Linux SDK team
 * Create: 2019/06/19
 */

#include "drv_otp.h"

#include <linux/mm.h>
#include "linux/hisilicon/securec.h"

#include "drv_ioctl_otp.h"
#include "hi_drv_otp.h"
#ifdef HI_PROC_SUPPORT
#include "drv_otp_proc.h"
#endif
#include "hal_otp.h"

static struct otp_mgmt g_otp_mgmt = {{0}};

struct otp_mgmt *__get_otp_mgmt(hi_void)
{
    return &g_otp_mgmt;
}

/* static internal API definition */
static hi_s32 _valid_mmap_phys_addr_range(hi_size_t pfn, size_t size, hi_size_t mask)
{
    return (pfn + (size >> PAGE_SHIFT)) <= (1 + (mask >> PAGE_SHIFT));
}

static otp_export_func g_otp_export_funcs = {
    .write_byte = hi_drv_otp_write_byte,
    .read_byte = hi_drv_otp_read_byte,
};

hi_s32 drv_otp_init(hi_void)
{
    hi_s32 ret;
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    ret = osal_mutex_init(&mgmt->lock);
    if (ret != HI_SUCCESS) {
        print_err_func(osal_mutex_init, ret);
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_OTP, "HI_OTP", (hi_void *)&g_otp_export_funcs);
    if (ret != HI_SUCCESS) {
        print_err_func(osal_exportfunc_register, ret);
        return ret;
    }
    return hal_otp_init();
}

hi_void drv_otp_deinit(hi_void)
{
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    hal_otp_deinit();
    osal_exportfunc_unregister(HI_ID_OTP);
    osal_mutex_destory(&mgmt->lock);

    return;
}

static hi_void _mutex_lock(hi_void)
{
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    osal_mutex_lock(&mgmt->lock);
}

static hi_void _mutex_unlock(hi_void)
{
    struct otp_mgmt *mgmt = __get_otp_mgmt();

    osal_mutex_unlock(&mgmt->lock);
}

hi_s32 otp_mmap_impl(osal_vm *vm, hi_ulong start, hi_ulong end, hi_ulong vm_pgoff, hi_void *private_data)
{
    size_t size;
    struct vm_area_struct *vma = HI_NULL;

    if (vm == HI_NULL || private_data == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    size = end - start;
    if (size >= OTP_MAP_SIZE) {
        print_err_hex2(size, -EINVAL);
        return -EINVAL;
    }

    /* invalid register address of common  */
    if ((vm_pgoff >> PAGE_NUM_SHIFT) != ((PERM_REG_BASE >> PAGE_SHIFT) >> PAGE_NUM_SHIFT)) {
        print_err_hex2(vm_pgoff, -EINVAL);
        return -EINVAL;
    }

    /* invalid register address or len of common */
    if (!_valid_mmap_phys_addr_range(vm_pgoff, size, PERM_REG_BASE | OTP_MAP_MASK)) {
        print_err_hex3(vm_pgoff, size, -EINVAL);
        return -EINVAL;
    }

    vma = (struct vm_area_struct *)vm->vm;
    vma->vm_page_prot = phys_mem_access_prot((struct file *)private_data, vm_pgoff, size, vma->vm_page_prot);
    if (osal_remap_pfn_range(vm, start, vm_pgoff, size, 0)) {
        return -EAGAIN;
    }
    return HI_SUCCESS;
}

hi_s32 otp_open_impl(hi_void *private_data)
{
    if (private_data == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    private_data = (hi_void *)(uintptr_t)(hi_ulong)osal_get_pid();

    return HI_SUCCESS;
}

hi_s32 otp_release_impl(hi_void *private_data)
{
    if (private_data == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    private_data = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 hi_drv_otp_reset(hi_void)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_reset();

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_reset);

hi_s32 hi_drv_otp_read(hi_u32 addr, hi_u32 *value)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_read(addr, value);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_read);

hi_s32 hi_drv_otp_read_byte(hi_u32 addr, hi_u8 *value)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_read_byte(addr, value);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_read_byte);

hi_s32 hi_drv_otp_read_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 *value)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_read_bits_onebyte(addr, start_bit, bit_width, value);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hal_otp_read_bits_onebyte);

hi_s32 hi_drv_otp_write(hi_u32 addr, hi_u32 value)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_write(addr, value);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_write);

hi_s32 hi_drv_otp_write_byte(hi_u32 addr, hi_u8 value)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_write_byte(addr, value);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_write_byte);

hi_s32 hi_drv_otp_write_bit(hi_u32 addr, hi_u32 bit_pos)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_write_bit(addr, bit_pos);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_write_bit);

hi_s32 hi_drv_otp_write_bits_onebyte(hi_u32 addr, hi_u32 start_bit, hi_u32 bit_width, hi_u8 value)
{
    hi_s32 ret;

    _mutex_lock();

    ret = hal_otp_write_bits_onebyte(addr, start_bit, bit_width, value);

    _mutex_unlock();
    return ret;
}
EXPORT_SYMBOL(hi_drv_otp_write_bits_onebyte);


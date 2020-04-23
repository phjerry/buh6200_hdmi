/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :Module init and exit API for OTP drivers.
* Author : Linux SDK team
* Created : 2019-06-20
*/

#include "drv_ioctl_otp.h"

#include <linux/module.h>
#include <linux/slab.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "drv_otp.h"
#include "drv_otp_proc.h"
#include "hal_otp.h"

static osal_dev g_otp_umap_dev = {{0}};

static hi_s32 otp_open(hi_void *private_data)
{
    if (private_data == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    return otp_open_impl(private_data);
}

static hi_s32 otp_release(hi_void *private_data)
{
    if (private_data == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    return otp_release_impl(private_data);
}


static hi_s32 otp_mmap(osal_vm *vm, hi_ulong start, hi_ulong end, hi_ulong vm_pgoff, hi_void *private_data)
{
    if (private_data == HI_NULL || vm == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    return otp_mmap_impl(vm, start, end, vm_pgoff, private_data);
}

static hi_s32 _drv_otp_read(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    otp_entry *entry = NULL;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }
    entry = (otp_entry *)arg;
    return hal_otp_read(entry->addr, &(entry->value));
}

static hi_s32 _drv_otp_read_byte(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    otp_entry *entry = NULL;
    hi_u8 value = 0;
    hi_s32 ret;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    entry = (otp_entry *)arg;
    ret = hal_otp_read_byte(entry->addr, &value);
    entry->value = value;
    return ret;
}

static hi_s32 _drv_otp_read_bits_one_byte(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    otp_entry *entry = (otp_entry *)arg;
    hi_u8 start_bit;
    hi_u8 bit_width;
    hi_u8 value;
    hi_s32 ret;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    start_bit = OTP_START_BIT(entry->value);
    bit_width = OTP_BIT_WIDTH(entry->value);
    value     = OTP_VALUE_BYTE(entry->value);

    if ((bit_width + start_bit > BYTE_WIDTH) || (start_bit >= BYTE_WIDTH)) {
        print_err_hex3(start_bit, bit_width, HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }

    ret = hal_otp_read_bits_onebyte(entry->addr, start_bit, bit_width, &value);
    entry->value = OTP_GET_PARA(start_bit, bit_width, value);
    return ret;
}

static hi_s32 _drv_otp_write(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    otp_entry *entry = (otp_entry *)arg;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    return hal_otp_write(entry->addr, entry->value);
}

static hi_s32 _drv_otp_write_byte(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    otp_entry *entry = (otp_entry *)arg;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    return hal_otp_write_byte(entry->addr, (hi_u8)entry->value);
}

static hi_s32 _drv_otp_write_bit(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{

    otp_entry *entry = (otp_entry *)arg;
    hi_u8 bit_pos;
    hi_u8 value;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    bit_pos = OTP_START_BIT(entry->value);
    value   = OTP_VALUE_BYTE(entry->value);

    if (bit_pos >= BYTE_WIDTH) {
        print_err_hex2(bit_pos, HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }
    if (value == 0) { /* write zero, do nothing. */
        return HI_SUCCESS;
    } else if (value == 1) { /* write 1, only one bit */
        return hal_otp_write_bit(entry->addr, bit_pos);
    }
    /* only support set 1 bit */
    print_err_hex2(value, HI_ERR_OTP_INVALID_PARA);
    return HI_ERR_OTP_INVALID_PARA;
}

static hi_s32 _drv_otp_write_bits_one_byte(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    otp_entry *entry = (otp_entry *)arg;
    hi_u8 start_bit;
    hi_u8 bit_width;
    hi_u8 value;

    if (arg == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    start_bit = OTP_START_BIT(entry->value);
    bit_width = OTP_BIT_WIDTH(entry->value);
    value     = OTP_VALUE_BYTE(entry->value);

    if ((bit_width + start_bit > BYTE_WIDTH) || (start_bit >= BYTE_WIDTH)) {
        print_err_hex3(start_bit, bit_width, HI_ERR_OTP_INVALID_PARA);
        return HI_ERR_OTP_INVALID_PARA;
    }
    return hal_otp_write_bits_onebyte(entry->addr, start_bit, bit_width, value);
}

static hi_s32 _drv_otp_reset(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    return hal_otp_reset();
}

static osal_ioctl_cmd g_ioctl_func_map[] = {
    { CMD_OTP_READ,               _drv_otp_read },
    { CMD_OTP_READ_BYTE,          _drv_otp_read_byte },
    { CMD_OTP_READ_BITS_ONEBYTE,  _drv_otp_read_bits_one_byte },
    { CMD_OTP_WRITE,              _drv_otp_write },
    { CMD_OTP_WRITE_BYTE,         _drv_otp_write_byte },
    { CMD_OTP_WRITE_BIT,          _drv_otp_write_bit },
    { CMD_OTP_WRITE_BITS_ONEBYTE, _drv_otp_write_bits_one_byte },
    { CMD_OTP_RESET,              _drv_otp_reset },
    /* ****** proc virtualotp begin ******* */
#ifdef HI_PROC_SUPPORT
    { CMD_OTP_TEST,               otp_virtual_test},
#endif
    /* ****** proc virtualotp end   ******* */
    { CMD_OTP_MAX,                HI_NULL},
};

static osal_fileops g_otp_fops = {
    .open           = otp_open,
    .read           = HI_NULL,
    .write          = HI_NULL,
    .llseek         = HI_NULL,
    .release        = otp_release,
    .poll           = HI_NULL,
    .mmap           = otp_mmap,
    .cmd_list       = g_ioctl_func_map,
    .cmd_cnt        = sizeof(g_ioctl_func_map) / sizeof(g_ioctl_func_map[0]),
};

static hi_s32 otp_suspend(hi_void *private_data)
{
    HI_PRINT("otp suspend ok.\n");
    return HI_SUCCESS;
}

static hi_s32 otp_resume(hi_void *private_data)
{
    HI_PRINT("otp resume ok.\n");
    return HI_SUCCESS;
}

static osal_pmops g_otp_pm_ops = {
    .pm_suspend        = otp_suspend,
    .pm_resume         = otp_resume,
    .pm_lowpower_enter   = NULL,
    .pm_lowpower_exit  = HI_NULL,
    .pm_poweroff       = HI_NULL,
    .private_data      = NULL,
};

static hi_s32 otp_register_dev(hi_void)
{
    hi_s32 ret;

    ret = strncpy_s(g_otp_umap_dev.name, sizeof(g_otp_umap_dev.name), HI_DEV_OTP_NAME,
                    sizeof(HI_DEV_OTP_NAME));
    if (ret != HI_SUCCESS) {
        return HI_ERR_OTP_SEC_FAILED;
    }

    g_otp_umap_dev.minor = HI_DEV_OTP_MINOR;
    g_otp_umap_dev.pmops = &g_otp_pm_ops;
    g_otp_umap_dev.fops = &g_otp_fops;

    ret = osal_dev_register(&g_otp_umap_dev);
    if (ret != HI_SUCCESS) {
        print_err_func(osal_dev_register, ret);
        return HI_ERR_OTP_NOT_INITRDY;
    }

    return HI_SUCCESS;
}

static hi_void otp_unregister_dev(hi_void)
{
    osal_dev_unregister(&g_otp_umap_dev);
    return;
}

hi_s32 hi_drv_otp_mod_init(hi_void)
{
    hi_s32 ret;

    ret = otp_register_dev();
    if (ret != HI_SUCCESS) {
        print_err_func(otp_register_dev, ret);
        return ret;
    }
#ifdef HI_PROC_SUPPORT
    ret = otp_register_proc();
    if (ret != HI_SUCCESS) {
        goto RET;
    }
#endif
    ret = drv_otp_init();
    if (ret != HI_SUCCESS) {
        goto RET;
    }
#ifdef MODULE
    HI_PRINT("Load hi_otp.ko success.  \t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
RET:
    otp_unregister_dev();
    return ret;
}

hi_void hi_drv_otp_mod_exit(hi_void)
{
    drv_otp_deinit();
#ifdef HI_PROC_SUPPORT
    otp_remove_proc();
#endif
    otp_unregister_dev();
#ifdef MODULE
    HI_PRINT("remove hi_otp.ko success.\n");
#endif
    return;
}

#ifdef MODULE
module_init(hi_drv_otp_mod_init);
module_exit(hi_drv_otp_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");


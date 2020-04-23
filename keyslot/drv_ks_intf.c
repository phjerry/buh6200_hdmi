/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :Module init and exit API for KeySlot drivers.
* Author : Linux SDK team
* Created : 2019-06-20
*/

#include "drv_ioctl_ks.h"

#include <linux/module.h>
#include <linux/slab.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "drv_ks.h"
#include "drv_ks_func.h"
#include "drv_ks_proc.h"
#include "drv_ks_utils.h"

static osal_dev g_ks_dev = {{0}};

static hi_s32 __drv_ks_create(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    ks_entry *entry = (ks_entry *)arg;
    hi_u32 slot_num = 0;
    hi_keyslot_priv_attr keyslot_attr;

    if ((private_data == HI_NULL) || (arg == HI_NULL)) {
        return HI_ERR_KS_PTR_NULL;
    }

    ret = ks_mgmt_auto_lock(entry->ks_type, &slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex2(ks_mgmt_auto_lock, entry->ks_type, ret);
        return ret;
    }

    keyslot_attr.bits.type = entry->ks_type;
    keyslot_attr.bits.secure = 0; /* 1 means TEE, 0 means REE */

    entry->ks_handle = ID_2_HANDLE(slot_num, keyslot_attr.u8);

    ret = ks_session_add_slot(*(struct ks_session **)private_data, entry->ks_type, slot_num);
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(ks_session_add_slot, entry->ks_type, slot_num, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 __drv_ks_destory(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    ks_entry *entry = (ks_entry *)arg;
    hi_keyslot_priv_attr keyslot_attr;

    if ((arg == HI_NULL) || (private_data == HI_NULL)) {
        return HI_ERR_KS_PTR_NULL;
    }

    if (is_invalid_handle(entry->ks_handle)) {
        print_err_hex2(entry->ks_handle, HI_ERR_KS_INVALID_PARAM);
        return HI_ERR_KS_INVALID_PARAM;
    }

    keyslot_attr.u8 = HANDLE_2_TYPE(entry->ks_handle);
    ret = ks_mgmt_unlock(keyslot_attr.bits.type, HANDLE_2_ID(entry->ks_handle));
    if (ret != HI_SUCCESS) {
        print_err_func_hex2(ks_mgmt_unlock, entry->ks_handle, ret);
        return ret;
    }

    ret = ks_session_del_slot(*(struct ks_session **)private_data, keyslot_attr.bits.type,
                              HANDLE_2_ID(entry->ks_handle));
    if (ret != HI_SUCCESS) {
        print_err_func_hex3(ks_session_add_slot, keyslot_attr.bits.type, HANDLE_2_ID(entry->ks_handle), ret);
        return ret;
    }

    return HI_SUCCESS;
}

static osal_ioctl_cmd g_ioctl_func_map[] = {
    { CMD_KS_CREATE,             __drv_ks_create },
    { CMD_KS_DESTORY,            __drv_ks_destory },
    { CMD_KS_MAX,                HI_NULL },
};

static hi_s32 ks_open(hi_void *private_data)
{
    hi_s32 ret;
    struct ks_session *session = HI_NULL;

    if (private_data == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    ret = ks_session_create(&session);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    *((struct ks_session **)private_data) = session;

out:
    return ret;
}

static hi_s32 ks_release(hi_void *private_data)
{
    hi_s32 ret;
    struct ks_session *session = *((struct ks_session **)private_data);

    if (private_data == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    ret = ks_session_destroy(session);
    if (ret == HI_SUCCESS) {
        *((struct ks_session **)private_data) = HI_NULL;
    }

    return ret;
}

static hi_s32 ks_mmap(osal_vm *vm, unsigned long start, hi_ulong end, hi_ulong vm_pgoff, hi_void *private_data)
{
    if (vm == HI_NULL || private_data == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }
    return ks_mmap_impl(vm, start, end, vm_pgoff, private_data);
}

static osal_fileops g_ks_fops = {
    .open           = ks_open,
    .read           = HI_NULL,
    .write          = HI_NULL,
    .llseek         = HI_NULL,
    .release        = ks_release,
    .poll           = HI_NULL,
    .mmap           = ks_mmap,
    .cmd_list       = g_ioctl_func_map,
    .cmd_cnt        = sizeof(g_ioctl_func_map) / sizeof(osal_ioctl_cmd),
};

static hi_s32 ks_suspend(hi_void *private_data)
{
    hi_s32 ret;

    ret = ks_mgmt_suspend();
    if (ret != HI_SUCCESS) {
        HI_PRINT("keyslot suspend failed.\n");
    } else {
        HI_PRINT("keyslot suspend ok.\n");
    }
    return ret;
}

static hi_s32 ks_resume(hi_void *private_data)
{
    hi_s32 ret;

    ret = ks_mgmt_resume();
    if (ret != HI_SUCCESS) {
        HI_PRINT("keyslot resume failed.\n");
    } else {
        HI_PRINT("keyslot resume ok.\n");
    }
    return ret;
}

static osal_pmops g_ks_pm_ops = {
    .pm_suspend         = ks_suspend,
    .pm_resume          = ks_resume,
    .pm_lowpower_enter  = HI_NULL,
    .pm_lowpower_exit   = HI_NULL,
    .pm_poweroff        = HI_NULL,
    .private_data       = HI_NULL,
};

static hi_s32 ks_register_dev(hi_void)
{
    hi_s32 ret;

    ret = strncpy_s(g_ks_dev.name, sizeof(g_ks_dev.name), HI_DEV_KEYSLOT_NAME, sizeof(HI_DEV_KEYSLOT_NAME));
    if (ret != HI_SUCCESS) {
        return HI_ERR_KS_CHECK_FAILED;
    }

    g_ks_dev.minor = HI_DEV_KEYSLOT_MINOR;
    g_ks_dev.fops = &g_ks_fops;
    g_ks_dev.pmops = &g_ks_pm_ops;

    ret = osal_dev_register(&g_ks_dev);
    if (ret != HI_SUCCESS) {
        print_err_func(osal_dev_register, ret);
        ret = HI_ERR_KS_NOT_INIT;
    }

    return ret;
}

static hi_void ks_unregister_dev(hi_void)
{
    osal_dev_unregister(&g_ks_dev);
    return;
}

hi_s32 hi_drv_ks_mod_init(hi_void)
{
    hi_s32 ret;

    ret = ks_register_dev();
    if (ret != HI_SUCCESS) {
        print_err_func(ks_register_dev, ret);
        return ret;
    }
#ifdef HI_PROC_SUPPORT
    ret = ks_register_proc();
    if (ret != HI_SUCCESS) {
        print_err_func(ks_register_proc, ret);
        goto RET;
    }
#endif
    ret = drv_ks_init();
    if (ret != HI_SUCCESS) {
        print_err_func(drv_ks_init, ret);
        goto RET;
    }
#ifdef MODULE
    HI_PRINT("Load hi_keyslot.ko success. \t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
RET:
    ks_unregister_dev();
    return ret;
}

hi_void hi_drv_ks_mod_exit(hi_void)
{
    drv_ks_deinit();
#ifdef HI_PROC_SUPPORT
    ks_remove_proc();
#endif
    ks_unregister_dev();
#ifdef MODULE
    HI_PRINT("Unload hi_keyslot.ko success.\n");
#endif
    return;
}

#ifdef MODULE
module_init(hi_drv_ks_mod_init);
module_exit(hi_drv_ks_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");


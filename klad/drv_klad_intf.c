/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :Module init and exit API for KeySlot drivers.
* Author : Linux SDK team
* Created : 2019-06-20
*/

#include "drv_ioctl_klad.h"

#include <linux/module.h>
#include <linux/slab.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/mm.h>

#include "hi_drv_dev.h"
#include "drv_klad_sw.h"
#include "drv_klad_proc.h"
#include "drv_rkp.h"
#include "drv_hkl.h"
#include "drv_klad_sw_func.h"
#include "drv_klad_hw_func.h"
#include "drv_klad_timestamp.h"
#include "drv_klad_timestamp_logic.h"
#include "drv_klad_hw_utils.h"
#include "hi_osal.h"

#define PAGE_NUM_SHIFT    0 /* it depends on how many pages can be maped, 0:one page, 1:two pages, 2:four pages,
                            3: eight pages */
#define KLAD_MAP_PAGE_NUM  (1ULL << PAGE_NUM_SHIFT)
#define KLAD_MAP_MASK      (((KLAD_MAP_PAGE_NUM) << PAGE_SHIFT) - 1)
#define KLAD_MAP_SIZE      0x10000

static osal_dev g_klad_dev = {{0}};

/* static internal API definition */
static hi_s32 _valid_mmap_phys_addr_range(hi_size_t pfn, size_t size, hi_size_t mask)
{
    return (pfn + (size >> PAGE_SHIFT)) <= (1 + (mask >> PAGE_SHIFT));
}

static klad_export_func g_klad_export_funcs = {
    .klad_set_clear_key = hi_drv_klad_clear_cw,
    .klad_set_clear_iv = hi_drv_klad_clear_iv,
};

static hi_s32 drv_klad_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_KLAD, "HI_KLAD", (hi_void *)&g_klad_export_funcs);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    klad_timestamp_queue_init();
    klad_timestamp_logic_queue_init();

    ret = rkp_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out1;
    }
    ret = hkl_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out2;
    }
    ret = klad_slot_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out3;
    }
    ret = klad_mgmt_init();
    if (ret != HI_SUCCESS) {
        goto out4;
    }

    return HI_SUCCESS;
out4:
    klad_slot_mgmt_exit();
out3:
    hkl_mgmt_exit();
out2:
    rkp_mgmt_exit();
out1:
    osal_exportfunc_unregister(HI_ID_KLAD);
out:
    return ret;
}

static hi_void drv_klad_exit(hi_void)
{
    klad_slot_mgmt_exit();
    klad_mgmt_exit();
    hkl_mgmt_exit();
    rkp_mgmt_exit();
    klad_timestamp_logic_queue_destory();
    klad_timestamp_queue_destory();
    osal_exportfunc_unregister(HI_ID_KLAD);
    return;
}

static hi_s32 klad_mmap_impl(osal_vm *vm, unsigned long start, unsigned long end,
                             unsigned long vm_pgoff, void *private_data)
{
    size_t size;
    struct vm_area_struct *vma = HI_NULL;

    if ((vm == HI_NULL) || (private_data == HI_NULL)) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    size = end - start;
    if (size >= KLAD_MAP_SIZE) {
        print_err_hex(size);
        return -EINVAL;
    }

    /* invalid register address of common  */
    if ((vm_pgoff >> PAGE_NUM_SHIFT) != ((KLAD_REG_BASE >> PAGE_SHIFT) >> PAGE_NUM_SHIFT)) {
        print_err_hex(vm_pgoff);
        return -EINVAL;
    }

    /* invalid register address or len of common */
    if (!_valid_mmap_phys_addr_range(vm_pgoff, size, KLAD_REG_BASE | KLAD_MAP_MASK)) {
        print_err_hex(vm_pgoff);
        print_err_hex(size);
        return -EINVAL;
    }

    vma = (struct vm_area_struct *)vm->vm;
    vma->vm_page_prot = phys_mem_access_prot((struct file *)private_data, vm_pgoff, size, vma->vm_page_prot);
    if (osal_remap_pfn_range(vm, start, vm_pgoff, size, 0)) {
        return -EAGAIN;
    }
    return HI_SUCCESS;
}

static hi_s32 klad_open(hi_void *private_data)
{
    hi_s32 ret;
    struct klad_hw_session *session = HI_NULL;

    if (private_data == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    ret = klad_hw_session_create(&session);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    *((struct klad_hw_session **)private_data) = (hi_void *)session;
out:
    return ret;
}

static hi_s32 klad_release(hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;

    if (private_data == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    if (private_data) {
        struct klad_hw_session *session = *((struct klad_hw_session **)private_data);

        ret = klad_hw_session_destroy(session);
        if (ret == HI_SUCCESS) {
            private_data = HI_NULL;
        }
    }

    return ret;
}

static hi_s32 klad_mmap(osal_vm *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data)
{
    if ((vm == HI_NULL) || (private_data == HI_NULL)) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    return klad_mmap_impl(vm, start, end, vm_pgoff, private_data);
}

static osal_ioctl_cmd g_ioctl_func_map[] = {
    { CMD_KLAD_COM_CREATE,         fmw_klad_com_create },
    { CMD_KLAD_TA_CREATE,          fmw_klad_ta_create },
    { CMD_KLAD_FP_CREATE,          fmw_klad_fp_create },
    { CMD_KLAD_NONCE_CREATE,       fmw_klad_nonce_create },
    { CMD_KLAD_COM_STARTUP,        fmw_klad_com_startup },
    { CMD_KLAD_TA_STARTUP,         fmw_klad_ta_startup },
    { CMD_KLAD_FP_STARTUP,         fmw_klad_fp_startup },
    { CMD_KLAD_NONCE_STARTUP,      fmw_klad_nonce_startup },
    { CMD_KLAD_CLR_PROCESS,        fmw_klad_clr_process },
    { CMD_KLAD_COM_DESTORY,        fmw_klad_com_destroy },
    { CMD_KLAD_TA_DESTORY,         fmw_klad_ta_destroy },
    { CMD_KLAD_FP_DESTORY,         fmw_klad_fp_destroy },
    { CMD_KLAD_NONCE_DESTORY,      fmw_klad_nonce_destroy },
    { CMD_KLAD_MAX,                HI_NULL }
};

static osal_fileops g_klad_fops = {
    .open           = klad_open,
    .read           = HI_NULL,
    .write          = HI_NULL,
    .llseek         = HI_NULL,
    .release        = klad_release,
    .poll           = HI_NULL,
    .mmap           = klad_mmap,
    .cmd_list       = g_ioctl_func_map,
    .cmd_cnt        = sizeof(g_ioctl_func_map) / sizeof(g_ioctl_func_map[0]),
};

static hi_s32 klad_suspend(hi_void *private_data)
{
    hi_s32 ret;

    ret = klad_mgmt_suspend();
    if (ret != HI_SUCCESS) {
        HI_PRINT("klad suspend failed.\n");
    } else {
        HI_PRINT("klad suspend ok.\n");
    }
    return ret;
}

static hi_s32 klad_resume(hi_void *private_data)
{
    hi_s32 ret;

    ret = klad_mgmt_resume();
    if (ret != HI_SUCCESS) {
        HI_PRINT("klad resume failed.\n");
    } else {
        HI_PRINT("klad resume ok.\n");
    }
    return ret;
}

static osal_pmops g_klad_pm_ops = {
    .pm_suspend          = klad_suspend,
    .pm_resume           = klad_resume,
    .pm_lowpower_enter   = HI_NULL,
    .pm_lowpower_exit    = HI_NULL,
    .pm_poweroff         = HI_NULL,
    .private_data        = HI_NULL,
};

static hi_s32 klad_register_dev(hi_void)
{
    hi_s32 ret;

    ret = strncpy_s(g_klad_dev.name, sizeof(g_klad_dev.name), HI_DEV_KLAD_NAME, sizeof(HI_DEV_KLAD_NAME));
    if (ret != EOK) {
        return HI_ERR_KLAD_SEC_FAILED;
    }
    g_klad_dev.minor = HI_DEV_KLAD_MINOR;
    g_klad_dev.pmops = &g_klad_pm_ops;
    g_klad_dev.fops = &g_klad_fops;

    ret = osal_dev_register(&g_klad_dev);
    if (ret != HI_SUCCESS) {
        print_err_func(osal_dev_register, ret);
        return HI_ERR_KLAD_NOT_INIT;
    }
    return HI_SUCCESS;
}

static hi_void klad_unregister_dev(hi_void)
{
    osal_dev_unregister(&g_klad_dev);

    return;
}

hi_s32 hi_drv_klad_mod_init(hi_void)
{
    hi_s32 ret;

    ret = klad_register_dev();
    if (ret != HI_SUCCESS) {
        print_err_func(klad_register_dev, ret);
        return ret;
    }
#ifdef HI_PROC_SUPPORT
    ret = klad_register_proc();
    if (ret != HI_SUCCESS) {
        goto RET;
    }
#endif
    ret = drv_klad_init();
    if (ret != HI_SUCCESS) {
        goto RET;
    }
#ifdef MODULE
    HI_PRINT("Load hi_klad.ko success.  \t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
RET:
    klad_unregister_dev();
    print_err_func(klad_register_proc, ret);
    return ret;
}

hi_void hi_drv_klad_mod_exit(hi_void)
{
    drv_klad_exit();
#ifdef HI_PROC_SUPPORT
    klad_remove_proc();
#endif
    klad_unregister_dev();
#ifdef MODULE
    HI_PRINT("Unload hi_klad.ko success.\n");
#endif
    return;
}

#ifdef MODULE
module_init(hi_drv_klad_mod_init);
module_exit(hi_drv_klad_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");


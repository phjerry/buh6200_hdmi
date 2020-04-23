/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Key slot driver.Provide all the kernel API and ioctl API.
 * Author : Linux SDK team
 * Create: 2019/06/22
 */

#include "drv_ks.h"

#include <linux/mm.h>
#include "linux/hisilicon/securec.h"

#include "drv_ioctl_ks.h"
#include "drv_ks_utils.h"
#ifdef HI_PROC_SUPPORT
#include "drv_ks_proc.h"
#endif
#include "hal_ks.h"
#include "drv_ks_func.h"
#include "hi_drv_keyslot.h"

static ks_export_func g_ks_export_funcs;

hi_s32 drv_ks_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_KEYSLOT, "HI_KS", (hi_void *)&g_ks_export_funcs);
    if (ret != HI_SUCCESS) {
        print_err_func(osal_exportfunc_register, ret);
        return ret;
    }
    ret = osal_exportfunc_register(HI_ID_USR_START, "HI_USR", HI_NULL);
    if (ret != HI_SUCCESS) {
        osal_exportfunc_unregister(HI_ID_KEYSLOT);
        print_err_func(osal_exportfunc_register, ret);
        return ret;
    }
    return ks_mgmt_init();
}

hi_void drv_ks_deinit(hi_void)
{
    ks_mgmt_exit();
    osal_exportfunc_unregister(HI_ID_KEYSLOT);
    osal_exportfunc_unregister(HI_ID_USR_START);
    return;
}

/* static internal API definition */
static hi_s32 __valid_mmap_phys_addr_range(hi_size_t pfn, size_t size, hi_size_t mask)
{
    return (pfn + (size >> PAGE_SHIFT)) <= (1 + (mask >> PAGE_SHIFT));
}

hi_s32 ks_mmap_impl(osal_vm *vm, unsigned long start, hi_ulong end, hi_ulong vm_pgoff, hi_void *private_data)
{
    size_t size;
    struct vm_area_struct *vma = HI_NULL;

    if (vm == HI_NULL || private_data == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    size = end - start;
    if (size >= KS_MAP_SIZE) {
        print_err_hex2(size, -EINVAL);
        return -EINVAL;
    }

    /* invalid register address of common  */
    if ((vm_pgoff >> PAGE_NUM_SHIFT) != ((KC_REG_BASE >> PAGE_SHIFT) >> PAGE_NUM_SHIFT)) {
        print_err_hex2(vm_pgoff, -EINVAL);
        return -EINVAL;
    }

    /* invalid register address or len of common */
    if (!__valid_mmap_phys_addr_range(vm_pgoff, size, KC_REG_BASE | KS_MAP_MASK)) {
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

hi_s32 hi_drv_ks_create(const hi_keyslot_type slot_type, hi_handle *ks_handle)
{
    hi_s32 ret;
    hi_u32 slot_num = 0;

    ret = ks_mgmt_auto_lock(slot_type, &slot_num);
    *ks_handle = ID_2_HANDLE(slot_num, slot_type);

    return ret;
}
EXPORT_SYMBOL(hi_drv_ks_create);


hi_s32 hi_drv_ks_destory(const hi_keyslot_type slot_type, const hi_handle ks_handle)
{
    return ks_mgmt_unlock(slot_type, HANDLE_2_ID(ks_handle));
}
EXPORT_SYMBOL(hi_drv_ks_destory);

static ks_export_func g_ks_export_funcs = {
    .ext_ks_create = hi_drv_ks_create,
    .ext_ks_destory = hi_drv_ks_destory,
};


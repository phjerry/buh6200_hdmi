/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv vi ext
 * Author: sdk
 * Create: 2019-12-14
 */

#include "hi_debug.h"

#include "vi_comm.h"
#include "vi_type.h"

#include "drv_vi_intf.h"
#include "drv_vi_ext.h"

hi_s32 vi_drv_mod_init(hi_void)
{
    hi_s32 ret;

#ifdef MODULE
    osal_printk("load hi_vi.ko start.\t\t(%s)\n", VERSION_STRING);
#endif

    ret = osal_exportfunc_register(HI_ID_VI, "HI_VI", HI_NULL);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("osal_exportfunc_register failed, mode ID = 0x%08X\n", HI_ID_VI);
        return HI_FAILURE;
    }

    ret = vi_drv_intf_init();
    if (ret != HI_SUCCESS) {
        osal_exportfunc_unregister(HI_ID_VI);
        vi_drv_log_err("vi_drv_intf_init failed, mode ID = 0x%08X\n", HI_ID_VI);
    }

#ifdef MODULE
    osal_printk("load hi_vi.ko success.\t\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void vi_drv_mod_exit(hi_void)
{
#ifdef MODULE
    osal_printk("remove hi_vi.ko start.\t\t(%s)\n", VERSION_STRING);
#endif

    vi_drv_intf_deinit();

    osal_exportfunc_unregister(HI_ID_VI);

#ifdef MODULE
    osal_printk("remove hi_vi.ko success.\t\t(%s)\n", VERSION_STRING);
#endif
}

#ifdef MODULE
module_init(vi_drv_mod_init);
module_exit(vi_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

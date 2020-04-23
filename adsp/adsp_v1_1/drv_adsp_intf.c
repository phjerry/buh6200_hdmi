/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv adsp
 * Author: Audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#include "hi_adsp_debug.h"

#include "drv_adsp_private.h"
#include "drv_adsp_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 adsp_drv_mod_init(hi_void)
{
#ifndef HI_MCE_SUPPORT
    hi_s32 ret = adsp_drv_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_PRINT_FUNC_RES(adsp_drv_init, ret);
        return ret;
    }
#endif

#ifdef HI_PROC_SUPPORT
    adsp_drv_register_proc();
#endif

#ifdef MODULE
    HI_PRINT("Load hi_adsp.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void adsp_drv_mod_exit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    adsp_drv_unregister_proc();
#endif
#ifndef HI_MCE_SUPPORT
    adsp_drv_exit();
#endif
}

#ifdef MODULE
module_init(adsp_drv_mod_init);
module_exit(adsp_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

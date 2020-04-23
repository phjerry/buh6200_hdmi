/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: vdp
* Create: 2019-06-28
 */
#include <linux/miscdevice.h>
#include "hi_type.h"

#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_win.h"
#include "drv_win.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

static struct miscdevice g_win_register_data;

static long vo_drv_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    long ret;

    ret = (long)hi_drv_user_copy(ffile, cmd, arg, drv_win_ioctl);

    return ret;
}

static struct file_operations win_file_ops = {
    .owner          = THIS_MODULE,
    .open           = drv_win_open,
    .unlocked_ioctl = vo_drv_ioctl,
    .compat_ioctl   = vo_drv_ioctl,
    .release        = drv_win_close,
};

#if 0
static hi_dev_pm_base_ops  win_basic_ops   = {
    .probe        = NULL,
    .remove       = NULL,
    .shutdown     = NULL,
    .prepare      = NULL,
    .complete     = NULL,
    .suspend      = drv_win_suspend,
    .suspend_late = NULL,
    .resume_early = drv_win_resume,
    .resume       = NULL,
};
#endif

hi_void win_mod_exit(hi_void)
{
    drv_win_deinit();
#ifndef HI_MCE_SUPPORT
    drv_win_un_register();
#endif
    misc_deregister(&g_win_register_data);

    return;
}

hi_s32 win_mod_init(hi_void)
{
    g_win_register_data.name = HI_DEV_VO_NAME;
    g_win_register_data.fops   = &win_file_ops;
    g_win_register_data.minor  = MISC_DYNAMIC_MINOR;

    if (misc_register(&g_win_register_data) < 0) {
        hi_fatal_win("register win failed.\n");
        return HI_FAILURE;
    }

#ifndef HI_MCE_SUPPORT
    if (drv_win_register() != HI_SUCCESS) {
        misc_deregister(&g_win_register_data);
        hi_fatal_win("drv win reg failed.\n");
        return HI_FAILURE;
    }
#endif
    if (drv_win_init() != HI_SUCCESS) {
        drv_win_un_register();
        misc_deregister(&g_win_register_data);
        hi_fatal_win("drv win init failed.\n");
        return HI_FAILURE;
    }

    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

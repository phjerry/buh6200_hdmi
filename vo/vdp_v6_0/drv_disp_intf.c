/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: isr
* Create: 2019-04-12
 */

#include <linux/module.h>
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_sys.h"
#include "hi_drv_dev.h"

#include "hi_drv_disp.h"
#include "hi_drv_win.h"
#include "drv_disp_ext.h"
#include "drv_disp_ioctl.h"
#include "drv_disp.h"
#include "vdp_ext_func.h"

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static struct class *g_disp_class = HI_NULL;
static dev_t g_disp_devno;
static struct cdev *g_disp_cdev = HI_NULL;
static struct device *g_disp_dev = HI_NULL;

static long disp_fileIoctl(struct file *ffile, hi_u32 cmd, hi_ulong arg)
{
    long ret;

    if (HI_NULL == ffile) {
        hi_fatal_disp("Pointer ffile is null.\n");
        return HI_FAILURE;
    }

    ret = (long)hi_drv_user_copy(ffile, cmd, arg,  drv_disp_ioctl);

    return ret;
}
static long disp_compat_fileIoctl(struct file *ffile, hi_u32 cmd, hi_ulong arg)
{
    long ret;

    ret = (long)hi_drv_user_copy(ffile, cmd, arg, drv_disp_compat_ioctl);

    return ret;
}

static hi_s32 disp_suspend(struct device *dev)
{
    return drv_disp_suspend();
}

static hi_s32 disp_resume_early(struct device *dev)
{
    return HI_SUCCESS;
}

static hi_s32 disp_resume(struct device *dev)
{
    return drv_disp_resume();
}

static struct dev_pm_ops g_disp_pm_ops = {
    .suspend        = disp_suspend,
    .suspend_late   = NULL,
    .resume_early   = disp_resume_early,
    .resume         = disp_resume,
};

static struct file_operations g_disp_fops = {
    .owner = THIS_MODULE,
    .open = disp_file_open,
    .release = disp_file_close,
    .unlocked_ioctl = disp_fileIoctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = disp_compat_fileIoctl,
#endif
};

static hi_s32 disp_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_disp_devno, 0, 1, "disp");
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    g_disp_cdev = cdev_alloc();
    if (IS_ERR(g_disp_cdev)) {
        ret = HI_FAILURE;
        goto OUT0;
    }

    cdev_init(g_disp_cdev, &g_disp_fops);
    g_disp_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_disp_cdev, g_disp_devno, 1);
    if (ret != HI_SUCCESS) {
        goto OUT1;
    }

    g_disp_class = class_create(THIS_MODULE, "disp_class");
    if (IS_ERR(g_disp_class)) {
        ret = HI_FAILURE;
        goto OUT2;
    }

    g_disp_class->pm = &g_disp_pm_ops;

    g_disp_dev = device_create(g_disp_class, HI_NULL, g_disp_devno, HI_NULL, HI_DEV_DISP_NAME);
    if (IS_ERR(g_disp_dev)) {
        ret = HI_FAILURE;
        goto OUT3;
    }

    return HI_SUCCESS;

OUT3:
    class_destroy(g_disp_class);
    g_disp_class = HI_NULL;
OUT2:
    cdev_del(g_disp_cdev);
OUT1:
    kfree(g_disp_cdev);
    g_disp_cdev = HI_NULL;
OUT0:
    unregister_chrdev_region(g_disp_devno, 1);

    return ret;
}

static hi_void disp_unregister_dev(hi_void)
{
    device_destroy(g_disp_class, g_disp_devno);
    g_disp_dev = HI_NULL;

    class_destroy(g_disp_class);
    g_disp_class = HI_NULL;

    cdev_del(g_disp_cdev);

    kfree(g_disp_cdev);
    g_disp_cdev = HI_NULL;

    unregister_chrdev_region(g_disp_devno, 1);

    return;
}

hi_void win_mod_exit(hi_void);
hi_s32 win_mod_init(hi_void);
hi_void hi_drv_disp_mod_exit(hi_void)
{
    drv_disp_shut_down();
    win_mod_exit();
    drv_disp_un_register();
    disp_unregister_dev();

    HI_PRINT("remove hi_vou.ko success.\n");
    return;
}

hi_s32 hi_drv_disp_mod_init(hi_void)
{
    if (disp_register_dev() < 0) {
        hi_fatal_disp("register DISP failed.\n");
        return HI_FAILURE;
    }

    if (vdp_ext_func_init() != HI_SUCCESS) {
        disp_unregister_dev();
        hi_fatal_disp("vdp_ext_func_init failed.\n");
        return HI_FAILURE;
    }

    if (drv_disp_register() != HI_SUCCESS) {
        disp_unregister_dev();
        hi_fatal_disp("DRV_DISP_Init failed.\n");
        return HI_FAILURE;
    }

    if(win_mod_init() != HI_SUCCESS)
    {
        hi_fatal_disp("win_mod_init failed.\n");
        disp_unregister_dev();
        drv_disp_un_register();
        return HI_FAILURE;
    }
    drv_disp_start_up();


    HI_PRINT("Load hi_vou.ko success.\t\t(%s)\n", VERSION_STRING);
    return HI_SUCCESS;
}


#ifdef MODULE
module_init(hi_drv_disp_mod_init);
module_exit(hi_drv_disp_mod_exit);
#else
EXPORT_SYMBOL(hi_drv_disp_mod_init);
EXPORT_SYMBOL(hi_drv_disp_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


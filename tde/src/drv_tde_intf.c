/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dev suspend & resume manage
 * Author: sdk
 * Create: 2019-10-23
 */

#include "drv_tde_intf.h"
#include <linux/cdev.h>
#include "hi_osal.h"

#include "hi_debug.h"
#include "hi_gfx_comm_k.h"
#include "drv_tde_intf.h"

static struct class *g_tde_class = HI_NULL;
static dev_t g_tde_devno;
static struct cdev *g_tde_cdev = HI_NULL;
static struct device *g_tde_dev = HI_NULL;

static hi_void drv_tde_class_destroy(hi_void);
static hi_void drv_tde_cdev_del(hi_void);
static hi_void drv_tde_cdev_free(hi_void);

static struct dev_pm_ops g_tde_pm_ops = {
    .suspend = drv_tde_suspend,
    .suspend_late = NULL,
    .resume_early = drv_tde_resume_early,
    .resume = drv_tde_resume,
};

static struct file_operations g_tde_fops = {
    .owner = THIS_MODULE,
    .open = drv_tde_dev_open,
    .release = drv_tde_dev_release,
    .unlocked_ioctl = drv_tde_dev_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = drv_tde_dev_ioctl,
#endif
};

static hi_void drv_tde_class_destroy(hi_void)
{
    if (g_tde_class != NULL) {
        class_destroy(g_tde_class);
        g_tde_class = HI_NULL;
    }
    drv_tde_cdev_del();
}

static hi_void drv_tde_cdev_del(hi_void)
{
    if (g_tde_cdev != NULL) {
        cdev_del(g_tde_cdev);
    }
    drv_tde_cdev_free();
}

static hi_void drv_tde_cdev_free(hi_void)
{
    if (g_tde_cdev != NULL) {
        osal_kfree(ConvertID(HIGFX_TDE_ID), g_tde_cdev);
        g_tde_cdev = HI_NULL;
    }
    unregister_chrdev_region(g_tde_devno, 1);
}

hi_s32 drv_tde_pm_register(hi_void)
{
    hi_s32 ret;
    ret = alloc_chrdev_region(&g_tde_devno, 0, 1, "hi_tde");
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-tde][err] : %s %d call alloc_chrdev_region failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    g_tde_cdev = cdev_alloc();
    if (IS_ERR(g_tde_cdev)) {
        HI_PRINT("[module-tde][err] : %s %d call cdev_alloc failure\n", __FUNCTION__, __LINE__);
        unregister_chrdev_region(g_tde_devno, 1);
        return HI_FAILURE;
    }

    cdev_init(g_tde_cdev, &g_tde_fops);
    g_tde_cdev->owner = THIS_MODULE;
    ret = cdev_add(g_tde_cdev, g_tde_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-tde][err] : %s %d call cdev_add failure\n", __FUNCTION__, __LINE__);
        drv_tde_cdev_free();
        return ret;
    }

    g_tde_class = class_create(THIS_MODULE, "tde_class");
    if (IS_ERR(g_tde_class)) {
        HI_PRINT("[module-tde][err] : %s %d call class_create failure\n", __FUNCTION__, __LINE__);
        drv_tde_cdev_del();
        return HI_FAILURE;
    }

    g_tde_class->pm = &g_tde_pm_ops;

    g_tde_dev = device_create(g_tde_class, HI_NULL, g_tde_devno, HI_NULL, "hi_tde");
    if (IS_ERR(g_tde_dev)) {
        HI_PRINT("[module-tde][err] : %s %d call device_create failure\n", __FUNCTION__, __LINE__);
        drv_tde_class_destroy();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void drv_tde_pm_unregister(hi_void)
{
    if (g_tde_class != NULL) {
        device_destroy(g_tde_class, g_tde_devno);
        class_destroy(g_tde_class);
        g_tde_class = HI_NULL;
    }
    g_tde_dev = HI_NULL;

    if (g_tde_cdev != NULL) {
        cdev_del(g_tde_cdev);
        osal_kfree(ConvertID(HIGFX_TDE_ID), g_tde_cdev);
        g_tde_cdev = HI_NULL;
    }

    unregister_chrdev_region(g_tde_devno, 1);
    return;
}

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d dev manage
 * Author: sdk
 * Create: 2019-10-23
 */

#include "drv_gfx2d_intf.h"

#include <linux/cdev.h>
#include "hi_osal.h"

#include "hi_debug.h"
#include "hi_gfx_comm_k.h"
#include "drv_gfx2d_intf.h"
#include "drv_gfx2d_device.h"

static struct class *g_gfx2d_class = HI_NULL;
static dev_t g_gfx2d_devno;
static struct cdev *g_gfx2d_cdev = HI_NULL;
static struct device *g_gfx2d_dev = HI_NULL;

static hi_void drv_gfx2d_class_destroy(hi_void);
static hi_void drv_gfx2d_cdev_del(hi_void);
static hi_void drv_gfx2d_cdev_free(hi_void);

static struct dev_pm_ops g_gfx2d_pm_ops = {
    .suspend = drv_gfx2d_dev_suspend,
    .suspend_late = NULL,
    .resume_early = drv_gfx2d_dev_resume_early,
    .resume = drv_gfx2d_dev_resume,
};

static struct file_operations g_gfx2d_fops = {
    .owner = THIS_MODULE,
    .open = drv_gfx2d_dev_open,
    .release = drv_gfx2d_dev_colse,
    .unlocked_ioctl = drv_gfx2d_dev_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = drv_gfx2d_dev_compat_ioctl,
#endif
};

static hi_void drv_gfx2d_class_destroy(hi_void)
{
    if (g_gfx2d_class != NULL) {
        class_destroy(g_gfx2d_class);
        g_gfx2d_class = HI_NULL;
    }
    drv_gfx2d_cdev_del();
}

static hi_void drv_gfx2d_cdev_del(hi_void)
{
    if (g_gfx2d_cdev != NULL) {
        cdev_del(g_gfx2d_cdev);
    }
    drv_gfx2d_cdev_free();
}

static hi_void drv_gfx2d_cdev_free(hi_void)
{
    if (g_gfx2d_cdev != NULL) {
        osal_kfree(ConvertID(HIGFX_GFX2D_ID), g_gfx2d_cdev);
        g_gfx2d_cdev = HI_NULL;
    }
    unregister_chrdev_region(g_gfx2d_devno, 1);
}

hi_s32 drv_gfx2d_pm_register(hi_void)
{
    hi_s32 ret;
    ret = alloc_chrdev_region(&g_gfx2d_devno, 0, 1, "hi_gfx2d");
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-gfx2d][err] : %s %d call alloc_chrdev_region failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    g_gfx2d_cdev = cdev_alloc();
    if (IS_ERR(g_gfx2d_cdev)) {
        HI_PRINT("[module-gfx2d][err] : %s %d call cdev_alloc failure\n", __FUNCTION__, __LINE__);
        unregister_chrdev_region(g_gfx2d_devno, 1);
        return HI_FAILURE;
    }

    cdev_init(g_gfx2d_cdev, &g_gfx2d_fops);
    g_gfx2d_cdev->owner = THIS_MODULE;
    ret = cdev_add(g_gfx2d_cdev, g_gfx2d_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-gfx2d][err] : %s %d call cdev_add failure\n", __FUNCTION__, __LINE__);
        drv_gfx2d_cdev_free();
        return ret;
    }

    g_gfx2d_class = class_create(THIS_MODULE, "gfx2d_class");
    if (IS_ERR(g_gfx2d_class)) {
        HI_PRINT("[module-gfx2d][err] : %s %d call class_create failure\n", __FUNCTION__, __LINE__);
        drv_gfx2d_cdev_del();
        return HI_FAILURE;
    }

    g_gfx2d_class->pm = &g_gfx2d_pm_ops;

    g_gfx2d_dev = device_create(g_gfx2d_class, HI_NULL, g_gfx2d_devno, HI_NULL, "hi_gfx2d");
    if (IS_ERR(g_gfx2d_dev)) {
        HI_PRINT("[module-gfx2d][err] : %s %d call device_create failure\n", __FUNCTION__, __LINE__);
        drv_gfx2d_class_destroy();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void drv_gfx2d_pm_unregister(hi_void)
{
    if (g_gfx2d_class != NULL) {
        device_destroy(g_gfx2d_class, g_gfx2d_devno);
        class_destroy(g_gfx2d_class);
        g_gfx2d_class = HI_NULL;
    }
    g_gfx2d_dev = HI_NULL;

    if (g_gfx2d_cdev != NULL) {
        cdev_del(g_gfx2d_cdev);
        osal_kfree(ConvertID(HIGFX_GFX2D_ID), g_gfx2d_cdev);
        g_gfx2d_cdev = HI_NULL;
    }

    unregister_chrdev_region(g_gfx2d_devno, 1);
    return;
}

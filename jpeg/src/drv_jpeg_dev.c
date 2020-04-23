/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg dev suspend & resume manage
 */

#include "drv_jpeg_dev.h"
#include <linux/cdev.h>
#include "hi_gfx_comm_k.h"
#include "drv_jpeg_intf.h"
#include "hi_debug.h"

static struct class *g_jpeg_class = HI_NULL;
static dev_t g_jpeg_devno;
static struct cdev *g_jpeg_cdev = HI_NULL;
static struct device *g_jpeg_dev = HI_NULL;

static hi_void drv_jpeg_class_destroy(hi_void);
static hi_void drv_jpeg_cdev_del(hi_void);
static hi_void drv_jpeg_cdev_free(hi_void);

static struct dev_pm_ops g_jpeg_pm_ops = {
    .suspend = drv_jpeg_dev_suspend,
    .suspend_late = NULL,
    .resume_early = drv_jpeg_dev_resume_early,
    .resume = drv_jpeg_dev_resume,
};

static struct file_operations g_jpeg_fops = {
    .owner = THIS_MODULE,
    .open = drv_jpeg_dev_open,
    .release = drv_jpeg_dev_close,
    .unlocked_ioctl = drv_jpeg_dev_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = drv_jpeg_dev_compat_ioctl,
#endif
};

static hi_void drv_jpeg_class_destroy(hi_void)
{
    if (g_jpeg_class != NULL) {
        class_destroy(g_jpeg_class);
        g_jpeg_class = HI_NULL;
    }
    drv_jpeg_cdev_del();
}

static hi_void drv_jpeg_cdev_del(hi_void)
{
    if (g_jpeg_cdev != NULL) {
        cdev_del(g_jpeg_cdev);
    }
    drv_jpeg_cdev_free();
}

static hi_void drv_jpeg_cdev_free(hi_void)
{
    if (g_jpeg_cdev != NULL) {
        osal_kfree(ConvertID(HIGFX_JPGDEC_ID), g_jpeg_cdev);
        g_jpeg_cdev = HI_NULL;
    }
    unregister_chrdev_region(g_jpeg_devno, 1);
}

hi_s32 drv_jpeg_pm_register(hi_void)
{
    hi_s32 ret;
    ret = alloc_chrdev_region(&g_jpeg_devno, 0, 1, "hi_jpeg");
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call alloc_chrdev_region failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    g_jpeg_cdev = cdev_alloc();
    if (IS_ERR(g_jpeg_cdev)) {
        HI_PRINT("[module-jpeg][err] : %s %d call cdev_alloc failure\n", __FUNCTION__, __LINE__);
        unregister_chrdev_region(g_jpeg_devno, 1);
        return HI_FAILURE;
    }

    cdev_init(g_jpeg_cdev, &g_jpeg_fops);
    g_jpeg_cdev->owner = THIS_MODULE;
    ret = cdev_add(g_jpeg_cdev, g_jpeg_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call cdev_add failure\n", __FUNCTION__, __LINE__);
        drv_jpeg_cdev_free();
        return ret;
    }

    g_jpeg_class = class_create(THIS_MODULE, "jpeg_class");
    if (IS_ERR(g_jpeg_class)) {
        HI_PRINT("[module-jpeg][err] : %s %d call class_create failure\n", __FUNCTION__, __LINE__);
        drv_jpeg_cdev_del();
        return HI_FAILURE;
    }

    g_jpeg_class->pm = &g_jpeg_pm_ops;

    g_jpeg_dev = device_create(g_jpeg_class, HI_NULL, g_jpeg_devno, HI_NULL, "hi_jpeg");
    if (IS_ERR(g_jpeg_dev)) {
        HI_PRINT("[module-jpeg][err] : %s %d call device_create failure\n", __FUNCTION__, __LINE__);
        drv_jpeg_class_destroy();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void drv_jpeg_pm_unregister(hi_void)
{
    if (g_jpeg_class != NULL) {
        device_destroy(g_jpeg_class, g_jpeg_devno);
        class_destroy(g_jpeg_class);
        g_jpeg_class = HI_NULL;
    }
    g_jpeg_dev = HI_NULL;

    if (g_jpeg_cdev != NULL) {
        cdev_del(g_jpeg_cdev);
        osal_kfree(ConvertID(HIGFX_JPGDEC_ID), g_jpeg_cdev);
        g_jpeg_cdev = HI_NULL;
    }

    unregister_chrdev_region(g_jpeg_devno, 1);
    return;
}

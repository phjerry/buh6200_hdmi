/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:
*/

#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include "hi_drv_spi.h"
#include "hi_drv_dev.h"
#include "hi_drv_osal.h"
#include "drv_spi_ioctl.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_mem.h"
#include "linux/hisilicon/securec.h"

#define MAX_SPI_CHANNEL 2

static struct semaphore sem_spi;
extern hi_s32 HI_DRV_SPI_Ioctl(struct file *file, hi_u32 cmd, hi_size_t arg);
#ifdef CONFIG_COMPAT
extern hi_s32 HI_DRV_SPI_Compat_Ioctl(struct file *file, hi_u32 cmd, hi_size_t arg);
#endif


/* initializes SPI interface routine. */
#ifdef HI_PROC_SUPPORT
hi_s32 SPI_ProcRead(struct seq_file *p, hi_void *v)
{
    hi_u8 sph = 0;
    hi_u8 spo = 0;
    hi_u8 dss = 0;
    hi_u8 blend = 0;
    hi_u8 fform = 0;

    if (p == HI_NULL) {
        HI_ERR_SPI("Pointer p is NULL ! \n");
        return HI_ERR_SPI_INVALID_PARA;
    }
    HI_PROC_PRINT(p, "---------Hisilicon SPI Info---------\n");
    HI_PROC_PRINT(p, "---------------SPI0-----------------\n");

    hi_drv_spi_get_from(0,&fform,&spo,&sph,&dss);
    if(fform == 0) {
        HI_PROC_PRINT(p,"--Motorola SPI--\n");
        HI_PROC_PRINT(p,"--spo=%d--\n",spo);
        HI_PROC_PRINT(p,"--sph=%d--\n",sph);
    } else if(fform == 1) {
        HI_PROC_PRINT(p,"--T1 SPI--\n");
    } else {
        HI_PROC_PRINT(p,"-- Microwire SPI--\n");
    }
    blend = hi_drv_spi_get_blend(0);
    if(blend) {
        HI_PROC_PRINT(p,"--big-end--\n");
    } else {
        HI_PROC_PRINT(p,"--little-end--\n");
    }
    HI_PROC_PRINT(p, "\n");
    HI_PROC_PRINT(p, "---------------SPI1-----------------\n");
    hi_drv_spi_get_from(1,&fform,&spo,&sph,&dss);
    if(fform == 0) {
        HI_PROC_PRINT(p,"--Motorola SPI--\n");
        HI_PROC_PRINT(p,"--spo=%d--\n",spo);
        HI_PROC_PRINT(p,"--sph=%d--\n",sph);
    } else if(fform == 1) {
        HI_PROC_PRINT(p,"--T1 SPI--\n");
    } else {
        HI_PROC_PRINT(p,"-- Microwire SPI--\n");
    }

    blend = hi_drv_spi_get_blend(1);
    if(blend) {
        HI_PROC_PRINT(p,"--big-end--\n");
    } else {
        HI_PROC_PRINT(p,"--little-end--\n");
    }
    HI_PROC_PRINT(p, "\n");

    return HI_SUCCESS;
}
#endif

static hi_slong hi_ssp_intf_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret = HI_FAILURE;
    if (down_interruptible(&sem_spi)) {
        HI_FATAL_SPI("ssp ioctl Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    if (file == HI_NULL) {
        HI_ERR_SPI("file pointer is NULL! \n");
        return HI_ERR_SPI_INVALID_PARA;
    }

    switch (cmd) {
        case CMD_SPI_OPEN:
        case CMD_SPI_CLOSE:
        case CMD_SPI_WRITE:
        case CMD_SPI_READ:
        case CMD_SPI_SET_ATTR:
        case CMD_SPI_GET_ATTR:
        case CMD_SPI_SET_BLEND:
        case CMD_SPI_GET_BLEND:
        case CMD_SPI_READEX:
        case CMD_SPI_SET_LOOP:
        case CMD_SPI_RW_LOOP: {
            ret = HI_DRV_SPI_Ioctl(file, cmd, arg);
            break;
        }

        default:
            ret = -ENOIOCTLCMD;
            break;
    }

    up(&sem_spi);
    return ret;
}

#ifdef CONFIG_COMPAT
static hi_slong hi_ssp_intf_compat_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret = HI_FAILURE;

    if (down_interruptible(&sem_spi)) {
        HI_FATAL_SPI("ssp ioctl Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    if (file == HI_NULL) {
        HI_ERR_SPI("file pointer is NULL! \n");
        return HI_ERR_SPI_INVALID_PARA;
    }

    switch (cmd) {
        case CMD_SPI_OPEN:
        case CMD_SPI_CLOSE:
        case CMD_SPI_WRITE:
        case CMD_SPI_READ:
        case CMD_SPI_SET_ATTR:
        case CMD_SPI_GET_ATTR:
        case CMD_SPI_SET_BLEND:
        case CMD_SPI_GET_BLEND:
        case CMD_SPI_READEX:
        case CMD_SPI_SET_LOOP:
        case CMD_SPI_RW_LOOP: {
            ret = HI_DRV_SPI_Compat_Ioctl(file, cmd, arg);
            break;
        }

        default:
            ret = -ENOIOCTLCMD;
            break;
    }
    up(&sem_spi);
    return ret;
}
#endif

hi_s32 hi_spi_dev_open(struct inode *inode, struct file *filp)
{
    hi_s32 ret = HI_SUCCESS;

    hi_size_t *dev_id = HI_KMALLOC(HI_ID_SPI, sizeof(hi_size_t)*MAX_SPI_CHANNEL,
                                   GFP_KERNEL);
    if (dev_id == HI_NULL) {
        HI_ERR_SPI("spi kmalloc failed\n");
        return HI_FAILURE;
    }

    ret = memset_s(dev_id, sizeof(unsigned long) * MAX_SPI_CHANNEL,
                   0, sizeof(unsigned long) * MAX_SPI_CHANNEL);
    if (ret != EOK) {
        HI_ERR_SPI("memset_s failed! \n");
        HI_KFREE(HI_ID_SPI, dev_id);
        return HI_FAILURE;
    }

    if (filp == HI_NULL) {
        HI_ERR_SPI("filp pointer is NULL! \n");
        HI_KFREE(HI_ID_SPI, dev_id);
        return HI_ERR_SPI_INVALID_PARA;
    }

    filp->private_data = dev_id;
    return HI_SUCCESS;
}


hi_s32 hi_spi_dev_close(struct inode *inode, struct file *filp)
{
    hi_s32 i;
    hi_size_t *dev_id = HI_NULL;

    if (filp == HI_NULL || filp->private_data) {
        HI_ERR_SPI("pointer filp is NULL! \n");
        return HI_ERR_SPI_INVALID_PARA;
    }

    dev_id = filp->private_data;

    for (i = 0; i < MAX_SPI_CHANNEL; i++) {
        if (dev_id && dev_id[i] == 1) {
            hi_drv_spi_close(dev_id[i]);
        }
    }

    HI_KFREE(HI_ID_SPI, dev_id);
    filp->private_data = NULL;
    return HI_SUCCESS;
}

static struct file_operations g_spi_fops = {
    .owner = THIS_MODULE,
    .open = hi_spi_dev_open,
    .unlocked_ioctl = hi_ssp_intf_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = hi_ssp_intf_compat_ioctl,
#endif
    .release = hi_spi_dev_close,
};

static struct class *g_spi_class = HI_NULL;
static dev_t g_spi_devno;
static struct cdev *g_spi_cdev = HI_NULL;
static struct device *g_spi_dev = HI_NULL;

hi_s32 spi_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_spi_devno, 0, 1, "spi");
    if (ret != HI_SUCCESS) {
        HI_FATAL_SPI("spi alloc chrdev region failed\n");
        return HI_FAILURE;
    }

    g_spi_cdev = cdev_alloc();
    if (IS_ERR(g_spi_cdev)) {
        HI_FATAL_SPI("spi alloc cdev failed! \n");

        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_spi_cdev, &g_spi_fops);
    g_spi_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_spi_cdev, g_spi_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SPI("spi add cdev failed, ret(%d).\n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_spi_class = class_create(THIS_MODULE, "spi_class");
    if (IS_ERR(g_spi_class)) {
        HI_FATAL_SPI("spi create dev class failed! \n");

        ret = HI_FAILURE;
        goto out2;
    }

    g_spi_dev = device_create(g_spi_class, HI_NULL, g_spi_devno, HI_NULL, HI_DEV_SPI_NAME);
    if (IS_ERR(g_spi_dev)) {
        HI_FATAL_SPI("spi create dev failed! \n");
        ret = HI_FAILURE;

        goto out3;
    }

    return HI_SUCCESS;

out3:
    class_destroy(g_spi_class);
    g_spi_class = HI_NULL;
out2:
    cdev_del(g_spi_cdev);
out1:
    kfree(g_spi_cdev);
    g_spi_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_spi_devno, 1);

    return ret;
}

static hi_void spi_unregister_dev(hi_void)
{
    device_destroy(g_spi_class, g_spi_devno);
    g_spi_dev= HI_NULL;
    class_destroy(g_spi_class);
    g_spi_class = HI_NULL;
    cdev_del(g_spi_cdev);
    kfree(g_spi_cdev);
    g_spi_cdev = HI_NULL;
    unregister_chrdev_region(g_spi_devno, 1);

    return;
}

hi_s32 SPI_DRV_ModInit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_proc_item  *proc_item = HI_NULL;
#endif

    HI_OSAL_INIT_MUTEX(&sem_spi);
    hi_drv_spi_init();

    if (spi_register_dev() < 0) {
        HI_FATAL_SPI("register SSP failed.\n");
        return HI_FAILURE;
    }
#ifdef HI_PROC_SUPPORT
    /* register PROC funtion */
    proc_item = hi_drv_proc_add_module(HI_MOD_SPI_NAME, HI_NULL, HI_NULL);
    if (proc_item == HI_NULL) {
        HI_INFO_SPI("add SPI proc failed.\n");
        spi_unregister_dev();
        return HI_FAILURE;
    }

    proc_item->read  = SPI_ProcRead;
#endif

#ifdef MODULE
    HI_PRINT("Load hi_spi.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void  SPI_DRV_ModExit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_drv_proc_remove_module(HI_MOD_SPI_NAME);
#endif

    spi_unregister_dev();
    hi_drv_spi_deinit();
}

#ifdef MODULE
module_init(SPI_DRV_ModInit);
module_exit(SPI_DRV_ModExit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");


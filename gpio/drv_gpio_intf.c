/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description: intf for gpio
*/

#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

#include "hi_errno.h"
#include "drv_gpio.h"
#include "drv_gpio_ioctl.h"
#include "hi_drv_dev.h"
#include "drv_gpio_ext.h"
#include "linux/hisilicon/securec.h"

static osal_semaphore g_gpio_sem_intf;

static hi_s32 hi_gpio_ioctl_set_interrupt_type(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_data gpio_data;
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_data, sizeof(gpio_data), arg, sizeof(gpio_data));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = hi_drv_gpio_set_interrupt_type(gpio_data.gpio_num, gpio_data.interrupt_type);
    osal_sem_up(&g_gpio_sem_intf);
    return ret;
}

static hi_s32 hi_gpio_ioctl_set_interrupt_enable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_data gpio_data;
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_data, sizeof(gpio_data), arg, sizeof(gpio_data));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    if (gpio_data.enable != HI_FALSE) {
        ret = hi_drv_gpio_clear_bit_interrupt(gpio_data.gpio_num);
        if (ret != HI_SUCCESS) {
            HI_ERR_GPIO("hi_drv_gpio_clear_bit_interrupt failed! \n");
        }
    }
    ret = hi_drv_gpio_set_bit_interrupt_enable(gpio_data.gpio_num, gpio_data.enable);
    osal_sem_up(&g_gpio_sem_intf);
    return ret;
}

static hi_s32 hi_gpio_ioctl_get_interrupt(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_interrupt gpio_interrupt_value = {0};
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_interrupt_value, sizeof(gpio_interrupt), arg, sizeof(gpio_interrupt));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = drv_gpio_query_interrupt(&gpio_interrupt_value);
    if (ret == HI_SUCCESS) {
        err = memcpy_s((gpio_interrupt *)arg, sizeof(gpio_interrupt), &gpio_interrupt_value, sizeof(gpio_interrupt));
        if (err != EOK) {
            HI_ERR_GPIO("memcpy_s fail!\n");
            osal_sem_up(&g_gpio_sem_intf);
            return HI_FAILURE;
        }
    } else {
        ret = HI_ERR_GPIO_FAILED_GETINT;
    }
    osal_sem_up(&g_gpio_sem_intf);
    return ret;
}

static hi_s32 hi_gpio_ioctl_get_gpio_num(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_get_gpio_num gpio_num = {0};
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }
    ret = hi_drv_gpio_get_gpio_num(&gpio_num);
    if (ret < 0) {
        HI_ERR_GPIO("get gpio num failed! ret = %x\n", ret);
        osal_sem_up(&g_gpio_sem_intf);
        return ret;
    }

    err = memcpy_s((gpio_get_gpio_num *)arg, sizeof(gpio_get_gpio_num), &gpio_num,
                   sizeof(gpio_get_gpio_num));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s data fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    osal_sem_up(&g_gpio_sem_intf);
    return HI_SUCCESS;
}

static hi_s32 hi_gpio_ioctl_set_output_type(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_output_type output_type = {0};
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&output_type, sizeof(gpio_output_type), (gpio_output_type *)(uintptr_t)arg,
                   sizeof(gpio_output_type));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s data fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = drv_gpio_set_output_type(output_type.gpio_num, output_type.output_type);
    osal_sem_up(&g_gpio_sem_intf);
    return ret;
}

static hi_s32 hi_gpio_ioctl_get_output_type(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_output_type output_type = {0};
    errno_t err;

    GPIO_CHECK_POINTER(arg);


    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&output_type, sizeof(gpio_output_type), arg, sizeof(gpio_output_type));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = drv_gpio_get_output_type(output_type.gpio_num, &output_type.output_type);
    if (ret) {
        HI_ERR_GPIO("drv_gpio_get_output_type fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    err = memcpy_s((gpio_output_type *)arg, sizeof(gpio_output_type), &output_type, sizeof(gpio_output_type));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    osal_sem_up(&g_gpio_sem_intf);
    return HI_SUCCESS;
}

static hi_s32 hi_gpio_ioctl_set_direction(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_direction gpio_dir;
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_dir, sizeof(gpio_direction), arg, sizeof(gpio_direction));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = hi_drv_gpio_set_direction_bit(gpio_dir.gpio_num, gpio_dir.input);
    osal_sem_up(&g_gpio_sem_intf);
    return ret;
}

static hi_s32 hi_gpio_ioctl_get_direction(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_direction gpio_dir = {0};
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_dir, sizeof(gpio_direction), arg, sizeof(gpio_direction));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = hi_drv_gpio_get_direction_bit(gpio_dir.gpio_num, &gpio_dir.input);
    if (ret) {
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    err = memcpy_s((gpio_direction *)arg, sizeof(gpio_direction), &gpio_dir, sizeof(gpio_direction));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    osal_sem_up(&g_gpio_sem_intf);
    return HI_SUCCESS;
}

static hi_s32 hi_gpio_ioctl_set_value(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_value gpio_val;
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_val, sizeof(gpio_value), arg, sizeof(gpio_value));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = hi_drv_gpio_write_bit(gpio_val.gpio_num, gpio_val.set);
    osal_sem_up(&g_gpio_sem_intf);
    return ret;
}

static hi_s32 hi_gpio_ioctl_get_value(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_SUCCESS;
    gpio_value gpio_val = {0};
    errno_t err;

    GPIO_CHECK_POINTER(arg);

    ret = osal_sem_down_interruptible(&g_gpio_sem_intf);
    if (ret) {
        HI_ERR_GPIO("Semaphore lock is error. \n");
        return HI_FAILURE;
    }

    err = memcpy_s(&gpio_val, sizeof(gpio_value), arg, sizeof(gpio_value));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    ret = hi_drv_gpio_read_bit(gpio_val.gpio_num, &gpio_val.set);
    if (ret) {
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    err = memcpy_s((gpio_value *)arg, sizeof(gpio_value), &gpio_val, sizeof(gpio_value));
    if (err != EOK) {
        HI_ERR_GPIO("memcpy_s fail!\n");
        osal_sem_up(&g_gpio_sem_intf);
        return HI_FAILURE;
    }

    osal_sem_up(&g_gpio_sem_intf);
    return HI_SUCCESS;
}

static osal_ioctl_cmd g_gpio_cmd_list[] = {
    {CMD_GPIO_SET_INT_TYPE,       hi_gpio_ioctl_set_interrupt_type},
    {CMD_GPIO_SET_INT_ENABLE,     hi_gpio_ioctl_set_interrupt_enable},
    {CMD_GPIO_GET_INT,            hi_gpio_ioctl_get_interrupt},
    {CMD_GPIO_GET_GPIONUM,        hi_gpio_ioctl_get_gpio_num},
    {CMD_GPIO_SET_OUTPUTTYPE,     hi_gpio_ioctl_set_output_type},
    {CMD_GPIO_GET_OUTPUTTYPE,     hi_gpio_ioctl_get_output_type},
    {CMD_GPIO_SET_DIRECTION,      hi_gpio_ioctl_set_direction},
    {CMD_GPIO_GET_DIRECTION,      hi_gpio_ioctl_get_direction},
    {CMD_GPIO_WRITE_BIT,          hi_gpio_ioctl_set_value},
    {CMD_GPIO_READ_BIT,           hi_gpio_ioctl_get_value},
};

static hi_s32 gpio_proc_read(hi_void *seqfile, hi_void *private)
{
    return HI_SUCCESS;
}

hi_s32 gpio_drv_proc_add(hi_void)
{
    hi_s32 ret;
    hi_char proc_name[16] = {0}; /* 存放驱动名，最大长度16 */
    osal_proc_entry *gpio_proc_entry = NULL;
    hi_u32 len = 0;
    ret = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name) - 1, "%s", HI_ID_GPIO);
    if (ret < 0) {
        HI_ERR_GPIO("secure func call error\n");
        return HI_FAILURE;
    }

    len = strlen(proc_name);
    gpio_proc_entry = osal_proc_add(proc_name, len);
    if (gpio_proc_entry == HI_NULL) {
        HI_ERR_GPIO("gpio add proc failed!\n");
        return HI_FAILURE;
    }

    gpio_proc_entry->read = gpio_proc_read;
    return HI_SUCCESS;
}

hi_s32 gpio_drv_proc_del(hi_s8 *proc_name, hi_u32 len)
{
    if (proc_name == NULL) {
        HI_ERR_GPIO("proc_name is null\n");
        return HI_FAILURE;
    }

    osal_proc_remove(proc_name, len);
    return HI_SUCCESS;
}

static osal_fileops g_gpio_fileops = {
    .read = NULL,
    .write = NULL,
    .open = drv_gpio_open,
    .release = drv_gpio_close,
    .cmd_list = g_gpio_cmd_list,
    .cmd_cnt = 0,
};

static osal_pmops g_gpio_pmops = {
    .pm_suspend = hi_drv_gpio_pm_suspend,
    .pm_resume =  hi_drv_gpio_pm_resume,
    .pm_lowpower_enter = NULL,
    .pm_lowpower_exit = NULL,
    .pm_poweroff = NULL,
    .private_data = NULL,
};

static osal_dev g_gpio_device = {
    .minor = HI_DEV_GPIO_MINOR,
    .fops = &g_gpio_fileops,
    .pmops = &g_gpio_pmops,
};

hi_s32 gpio_drv_module_init(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = osal_sem_init(&g_gpio_sem_intf, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("osal_sem_init failed ! \n");
        return HI_FAILURE;
    }
#ifndef HI_MCE_SUPPORT
#ifndef HI_KEYLED_CT1642_KERNEL_SUPPORT
    hi_drv_gpio_init();
#endif
#endif

    g_gpio_fileops.cmd_cnt = sizeof(g_gpio_cmd_list) / sizeof(osal_ioctl_cmd);

    ret = snprintf_s(g_gpio_device.name, sizeof(g_gpio_device.name),
                     sizeof(g_gpio_device.name) - 1, "%s", HI_DEV_GPIO_NAME);
    if (ret < 0) {
        HI_ERR_GPIO("snprintf_s failed!\n");
        goto err0;
    }

    ret = osal_dev_register(&g_gpio_device);
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("register gpio failed.\n");
        return HI_FAILURE;
    }
#ifdef HI_PROC_SUPPORT
    ret = gpio_drv_proc_add();
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("call gpio_drv_proc_add failed!\n");
        osal_dev_unregister(&g_gpio_device);
        return HI_FAILURE;
    }
#endif
#ifdef MODULE
    HI_PRINT("Load hi_gpio.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

err0:
#ifndef HI_MCE_SUPPORT
#ifndef HI_KEYLED_CT1642_KERNEL_SUPPORT
    hi_drv_gpio_deinit();
#endif
#endif
    return HI_FAILURE;
}

hi_void gpio_drv_module_exit(hi_void)
{
    hi_s32 ret;
    errno_t sec_errno;
    hi_char proc_name[16] = {0}; /* 存放驱动名，最大长度16 */
    osal_sem_destory(&g_gpio_sem_intf);
    sec_errno = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name) - 1, "%s", HI_ID_GPIO);
    if (sec_errno < 0) {
        HI_ERR_GPIO("secure func call error\n");
        return;
    }
    osal_dev_unregister(&g_gpio_device);
#ifdef HI_PROC_SUPPORT
    ret = gpio_drv_proc_del(proc_name, strlen(proc_name));
    if (ret != HI_SUCCESS) {
        HI_ERR_GPIO("gpio_drv_proc_del failure!\n");
    }
#endif

#ifndef HI_MCE_SUPPORT
#ifndef HI_KEYLED_CT1642_KERNEL_SUPPORT
    hi_drv_gpio_deinit();
#endif
#endif

#ifdef MODULE
    HI_PRINT("remove hi_gpio.ko ok!\n");
#endif
    return;
}

#ifdef MODULE
module_init(gpio_drv_module_init);
module_exit(gpio_drv_module_exit);
#endif

MODULE_LICENSE("GPL");

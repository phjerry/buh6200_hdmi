/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description:
 */

#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "drv_i2c.h"
#include "drv_i2c_ext.h"
#include "hi_drv_i2c.h"
#include "drv_i2c_ioctl.h"
#include "drv_gpioi2c_ext.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_errno.h"

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

#define I2C_WRITE_REG(addr, value) ((*(volatile hi_u32 *)((uintptr_t)(addr))) = (value))
#define I2C_READ_REG(addr) (*(volatile hi_u32 *)((uintptr_t)(addr)))

static gpio_i2c_ext_func *g_gpio_i2c_ext_funcs = HI_NULL;
extern hi_u32 g_i2c_rate_value[HI_I2C_MAX_NUM];

#ifdef HI_PROC_SUPPORT
hi_s32 i2c_proc_read(struct seq_file *p, hi_void *v)
{
    hi_u8 ii = 0;
    HI_PROC_PRINT(p, "---------Hisilicon Standard I2C Info--------\n");
    HI_PROC_PRINT(p, "No.            Rate\n");

    for (ii = 0; ii < g_hi_i2c_valid_num; ii++) {
        HI_PROC_PRINT(p, "%d             %d\n", ii, g_i2c_rate_value[ii]);
    }

#ifdef HI_GPIOI2C_SUPPORT
    HI_PROC_PRINT(p, "---------Hisilicon GPIO simulate I2C Info---------\n");
    HI_PROC_PRINT(p, "No.            SCL_IO       SDA_IO\n");
    for (ii = 0; ii < HI_I2C_MAX_NUM; ii++) {
        if (g_gpio_i2c[ii].is_used) {
            HI_PROC_PRINT(p, "%d                %d           %d\n", g_gpio_i2c[ii].i2c_num,
                          g_gpio_i2c[ii].scl_gpio_num, g_gpio_i2c[ii].sda_gpio_num);
        }
    }
#endif

    return HI_SUCCESS;
}

static hi_void usage(hi_void)
{
    HI_PRINT("\nUsage:\n");
    HI_PRINT("Read data: echo 'bus' 'device address' 'Register address' > /proc/msp/i2c\n");
    HI_PRINT("Write data: echo 'bus' 'device address' 'Register address' 'data' > /proc/msp/i2c\n");
    HI_PRINT("or:echo 'bus' 'device address' 'Register address' 'data number n(n<=32)' 'data1' ...'datan > /proc/msp/i2c\n");
    HI_PRINT("such as: echo 4 a0 5d 2 b c > /proc/msp/i2c \n");
    HI_PRINT("Look over i2c info: cat /proc/msp/i2c \n");
}

static hi_s32 i2c_proc_read_func(hi_u32 i2c_num, hi_u32 dev_addr, hi_u32 reg_addr, hi_u32 value)
{
    hi_s32 ret = HI_SUCCESS;
    HI_PRINT("ProcRead: i2c=%d, addr=0x%x, reg=0x%x\n", i2c_num, dev_addr, reg_addr);

    if (i2c_num < g_hi_i2c_valid_num) {
        ret = hi_drv_i2c_read(i2c_num, (hi_u8)dev_addr, reg_addr, 1, (hi_u8 *)&value, 1);
        if (ret != HI_SUCCESS) {
            HI_ERR_I2C("Read failed(Ret:0x%x)\n\n", ret);
        } else {
            HI_PRINT("0x%x\n", value);
        }
        return ret;
    }

    if (g_gpio_i2c_ext_funcs && g_gpio_i2c_ext_funcs->pfn_gpio_i2c_is_used) {
        hi_bool is_used = HI_FALSE;

        ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_is_used(i2c_num, &is_used);
        if ((ret != HI_SUCCESS) || (is_used != HI_TRUE)) {
            HI_INFO_I2C("Read failed(i2c_num=%d not valid)! \n", i2c_num);
            return HI_FAILURE;
        }

        if (g_gpio_i2c_ext_funcs->pfn_gpio_i2c_read_ext) {
            ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_read_ext(i2c_num, dev_addr, reg_addr, 1,
                    (hi_u8 *)&value, 1);
            if (ret != HI_SUCCESS) {
                HI_ERR_I2C("Read failed(Ret:0x%x)\n\n", ret);
            } else {
                HI_PRINT("0x%x\n", value);
            }
        }
        return ret;
    }

    HI_INFO_I2C("Read failed(i2c_num=%d not valid)! \n", i2c_num);
    return HI_FAILURE;
}

static hi_s32 i2c_proc_write_func(hi_u32 i2c_num, hi_u32 dev_addr,
                                  hi_u32 reg_addr, hi_u32 buf_len, hi_u8 *send_buf)
{
    hi_s32 ret = HI_SUCCESS;
    HI_PRINT("Write: i2c_num=%d, dev_addr=0x%x, reg_addr=0x%x, write number=0x%x\n", i2c_num,
             dev_addr, reg_addr, buf_len);

    if (i2c_num < g_hi_i2c_valid_num) {
        ret = hi_drv_i2c_write(i2c_num, (hi_u8)dev_addr, reg_addr, 1, send_buf, buf_len);
        if (ret != HI_SUCCESS) {
            HI_ERR_I2C("Write failed(Ret:0x%x)", ret);
        }
        return ret;
    }

    if (g_gpio_i2c_ext_funcs && g_gpio_i2c_ext_funcs->pfn_gpio_i2c_is_used) {
        hi_bool is_used = HI_FALSE;

        ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_is_used(i2c_num, &is_used);
        if ((ret != HI_SUCCESS) || (is_used != HI_TRUE)) {
            HI_INFO_I2C("Write failed(i2c_num=%d not valid)! \n", i2c_num);
            return HI_FAILURE;
        }

        if (g_gpio_i2c_ext_funcs->pfn_gpio_i2c_write_ext) {
            ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_write_ext(i2c_num, dev_addr, reg_addr, 1,
                    send_buf, buf_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_I2C("Write failed(Ret:0x%x)\n\n", ret);
            }
        }
        return ret;
    }

    HI_INFO_I2C("Write failed(i2c_num=%d not valid)! \n", i2c_num);
    return HI_FAILURE;
}

hi_s32 i2c_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    hi_u32 i2c_num = 0, dev_addr = 0, reg_addr = 0, value = 0;
    hi_s32 ret = HI_SUCCESS;
    hi_char input_buf[256];
    hi_char *p  = HI_NULL;
    hi_u32 i = 0;
    hi_u8 send_buf[32] = {0};
    hi_u8 buf_len = 0;
    hi_u8 arg_count = 0;
    hi_bool command_is_err = HI_FALSE;
    hi_bool will_set_rate = HI_FALSE;
    hi_u32 rate_value = 0;

    if (count >= sizeof(input_buf)) {
        HI_PRINT("command line is too long, please try it in 256 Bytes\n");
        return HI_FAILURE;
    }

    if (memset_s(input_buf, sizeof(input_buf), 0, sizeof(input_buf))) {
        HI_PRINT("memset_s input_buf failed\n");
        return HI_FAILURE;
    }

    if (buf == HI_NULL) {
        HI_PRINT("buf is null pointer\n");
        return HI_FAILURE;
    }

    if (copy_from_user(input_buf, buf, count)) {
        return HI_FAILURE;
    }
    input_buf[255] = '\0';

    /* begin deal with set rate cmd */
    p = skip_spaces(input_buf);
    if (p == HI_NULL) {
        HI_PRINT("p is null pointer\n");
        return HI_FAILURE;
    }

    if (!strncmp(p, "SetRate", strlen("SetRate"))) {
        ret = sscanf(p, "SetRate %2d %8d", &i2c_num, &rate_value);
        if (ret != 2) {
            HI_PRINT("%s, i2c:%d, rate:%dHZ\n", p, i2c_num, rate_value);
            will_set_rate = HI_FALSE;
        } else {
            will_set_rate = HI_TRUE;
        }
    }

    if (will_set_rate) {
        if (i2c_num >= g_hi_i2c_valid_num) {
            HI_PRINT("I2c NO.%d not support rate setting!\n", i2c_num);
        } else {
            (hi_void)i2c_drv_set_rate(i2c_num, rate_value);
        }
        return count;
    }
    /* end deal with set rate cmd */

    p = input_buf;
    arg_count = 0;

    for (i = 0; i < count && i < strlen(input_buf); i++) {
        if ((input_buf[i] == ' ') || (input_buf[i] == '\n')) {
            arg_count++;
        }
    }

    if (arg_count < 3) {
        command_is_err = HI_TRUE;
        goto exit;
    }

    i2c_num   = (hi_u32)simple_strtoul(p, &p, 16);
    dev_addr = (hi_u32)simple_strtoul(p + 1, &p, 16);
    reg_addr = (hi_u32)simple_strtoul(p + 1, &p, 16);
    value = (hi_u32)simple_strtoul(p + 1, &p, 16);

    if (arg_count > 4) {
        if (value != (arg_count - 4)) {
            command_is_err = HI_TRUE;
            goto exit;
        }

        if ((arg_count - 4) > sizeof(send_buf)) {
            command_is_err = HI_TRUE;
            goto exit;
        }

        for (i = 0; i < (arg_count - 4); i++) {
            send_buf[i] = (hi_u8)simple_strtoul(p + 1, &p, 16);
        }

        buf_len = arg_count - 4;
    } else {
        send_buf[0] = value;
        buf_len = 1;
    }

    if ((i2c_num >= g_hi_i2c_valid_num) && (g_gpio_i2c_ext_funcs == HI_NULL)) {
        ret = hi_drv_module_get_func(HI_ID_GPIO_I2C, (hi_void **)&g_gpio_i2c_ext_funcs);
        if ((ret != HI_SUCCESS) || (g_gpio_i2c_ext_funcs == HI_NULL)) {
            HI_PRINT("GPIO_I2C Function ERR: ret:0x%08x\n", ret);
            goto exit;
        }
    }

    if (arg_count == 3) {
        i2c_proc_read_func(i2c_num, dev_addr, reg_addr, value);
    } else {
        i2c_proc_write_func(i2c_num, dev_addr, reg_addr, buf_len, send_buf);
    }

exit:
    if (command_is_err) {
        usage();
    }

    return count;
}
#endif

static hi_slong i2c_intf_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret = HI_FAILURE;
    i2c_data data;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;

    if ((file == HI_NULL) || (argp == HI_NULL)) {
        HI_ERR_I2C("null pointer\n");
        return ret;
    }

    switch (cmd) {
        /* 
         * if I2C channel num < 4 ,when execute ioctl select i2c_ioctl,otherwise selet hi_drv_gpio_i2c_ioctl,
         * the same macro will compatible i2c and gpio-i2c
         */
        case CMD_I2C_WRITE:
        case CMD_I2C_READ:
            if (copy_from_user(&data, argp, sizeof(i2c_data))) {
                HI_INFO_I2C("copy data from user fail!\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                return ret;
            }

            if (data.i2c_num >= HI_I2C_MAX_NUM) {
                HI_INFO_I2C("i2c chanenl (NO:%d) is invalid !\n", data.i2c_num);
                return HI_ERR_I2C_INVALID_PARA;
            }

            if (data.i2c_num < g_hi_i2c_valid_num) {
                return i2c_ioctl(file, cmd, arg);
            } else if (g_gpio_i2c_ext_funcs && g_gpio_i2c_ext_funcs->pfn_gpio_i2c_ioctl) {
                ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_ioctl(file, cmd, arg);
            }

            break;

        case CMD_I2C_SET_RATE:
            ret = i2c_ioctl(file, cmd, arg);
            break;

        case CMD_I2C_CONFIG:
        case CMD_I2C_DESTROY:
            if (g_gpio_i2c_ext_funcs && g_gpio_i2c_ext_funcs->pfn_gpio_i2c_ioctl) {
                ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_ioctl(file, cmd, arg);
            }
            break;

        default:
            ret = -ENOIOCTLCMD;
            break;
    }

    return ret;
}

#ifdef CONFIG_COMPAT
static hi_slong i2c_intf_compat_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret = HI_FAILURE;
    i2c_data_compat data;
    hi_void __user *argp = (hi_void __user *)(uintptr_t)arg;
    if ((file == HI_NULL) || (argp == HI_NULL)) {
        HI_ERR_I2C("null pointer\n");
        return ret;
    }

    switch (cmd) {
        /*
         * if I2C channel num < 4 ,when execute ioctl select i2c_ioctl,otherwise selet hi_drv_gpio_i2c_ioctl,
         * the same macro will compatible i2c and gpio-i2c
         */
        case CMD_I2C_WRITE:
        case CMD_I2C_READ:
            if (copy_from_user(&data, argp, sizeof(i2c_data_compat))) {
                HI_INFO_I2C("copy data from user fail\n");
                ret = HI_ERR_I2C_COPY_DATA_ERR;
                return ret;
            }

            if (data.i2c_num >= HI_I2C_MAX_NUM) {
                HI_INFO_I2C("i2c chanenl (NO:%d) is invalid !\n", data.i2c_num);
                return HI_ERR_I2C_INVALID_PARA;
            }

            if (data.i2c_num < g_hi_i2c_valid_num) {
                return i2c_compat_ioctl(file, cmd, arg);
            } else if (g_gpio_i2c_ext_funcs && g_gpio_i2c_ext_funcs->pfn_gpio_i2c_compat_ioctl) {
                ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_compat_ioctl(file, cmd, arg);
            }

            break;

        case CMD_I2C_SET_RATE:
            ret = i2c_compat_ioctl(file, cmd, arg);
            break;

        case CMD_I2C_CONFIG:
        case CMD_I2C_DESTROY:
            if (g_gpio_i2c_ext_funcs && g_gpio_i2c_ext_funcs->pfn_gpio_i2c_compat_ioctl) {
                ret = g_gpio_i2c_ext_funcs->pfn_gpio_i2c_compat_ioctl(file, cmd, arg);
            }

            break;

        default:
            ret = -ENOIOCTLCMD;
            break;
    }

    return ret;
}
#endif

hi_s32 i2c_open(struct inode *inode, struct file *filp)
{
    hi_s32 ret = HI_SUCCESS;

    ret = hi_drv_module_get_func(HI_ID_GPIO_I2C, (hi_void **)&g_gpio_i2c_ext_funcs);
    if ((ret != HI_SUCCESS) || (g_gpio_i2c_ext_funcs == HI_NULL)) {
        HI_INFO_I2C("Get GPIO_I2C Function ERR: ret:0x%08x\n", ret);
    }

    return HI_SUCCESS;
}

hi_s32 i2c_close(struct inode *inode, struct file *filp)
{
    return HI_SUCCESS;
}

hi_s32 i2c_pm_suspend(struct device *dev)
{
    return 0;
}

hi_s32 i2c_pm_resume(struct device *dev)
{
    return 0;
}

static struct dev_pm_ops g_i2c_pm_ops = {
    .suspend        = i2c_pm_suspend,
    .suspend_late   = NULL,
    .resume_early   = NULL,
    .resume         = i2c_pm_resume,
};

static struct file_operations i2c_fops = {
    .owner = THIS_MODULE,
    .open = i2c_open,
    .unlocked_ioctl = i2c_intf_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = i2c_intf_compat_ioctl,
#endif
    .release = i2c_close,
};

static struct class *g_i2c_class = HI_NULL;
static dev_t g_i2c_devno;
static struct cdev *g_i2c_cdev = HI_NULL;
static struct device *g_i2c_dev = HI_NULL;

static hi_s32 i2c_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_i2c_devno, 0, 1, "i2c");
    if (ret != HI_SUCCESS) {
        HI_FATAL_I2C("i2c alloc chrdev region failed\n");

        return HI_FAILURE;
    }

    g_i2c_cdev = cdev_alloc();
    if (IS_ERR(g_i2c_cdev)) {
        HI_FATAL_I2C("i2c alloc cdev failed! \n");

        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_i2c_cdev, &i2c_fops);
    g_i2c_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_i2c_cdev, g_i2c_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_FATAL_I2C("i2c add cdev failed, ret(%d).\n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_i2c_class = class_create(THIS_MODULE, "i2c_class");
    if (IS_ERR(g_i2c_class)) {
        HI_FATAL_I2C("i2c create dev class failed! \n");

        ret = HI_FAILURE;
        goto out2;
    }

    g_i2c_class->pm = &g_i2c_pm_ops;

    g_i2c_dev = device_create(g_i2c_class, HI_NULL, g_i2c_devno, HI_NULL, HI_DEV_I2C_NAME);
    if (IS_ERR(g_i2c_dev)) {
        HI_FATAL_I2C("i2c create dev failed! \n");
        ret = HI_FAILURE;

        goto out3;
    }

    return HI_SUCCESS;

out3:
    class_destroy(g_i2c_class);
    g_i2c_class = HI_NULL;
out2:
    cdev_del(g_i2c_cdev);
out1:
    kfree(g_i2c_cdev);
    g_i2c_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_i2c_devno, 1);

    return ret;
}

static hi_void i2c_unregister_dev(hi_void)
{
    device_destroy(g_i2c_class, g_i2c_devno);
    g_i2c_dev= HI_NULL;
    class_destroy(g_i2c_class);
    g_i2c_class = HI_NULL;
    cdev_del(g_i2c_cdev);
    kfree(g_i2c_cdev);
    g_i2c_cdev = HI_NULL;
    unregister_chrdev_region(g_i2c_devno, 1);

    return;
}

hi_s32 i2c_drv_module_init(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_proc_item  *proc_item = NULL;
#endif
    hi_drv_i2c_init(); /* HI_MCE_SUPPORT=yes or HI_MCE_SUPPORT=no, all will call HI_DRV_I2C_Init, make sure HI_I2C_Open call it */

    if (i2c_register_dev() < 0) {
        HI_FATAL_I2C("register I2C failed.\n");
        return HI_FAILURE;
    }
#ifdef HI_PROC_SUPPORT
    /* register i2c PROC funtion */
    proc_item = hi_drv_proc_add_module(HI_MOD_I2C_NAME, HI_NULL, HI_NULL);
    if (!proc_item) {
        HI_INFO_I2C("add I2C proc failed.\n");
        i2c_unregister_dev();
        return HI_FAILURE;
    }

    proc_item->read  = i2c_proc_read;
    proc_item->write = i2c_proc_write;
#endif

#ifdef MODULE
    HI_PRINT("Load hi_i2c.ko success.  \t(%s)\n", VERSION_STRING);
#endif
    return 0;
}

hi_void i2c_drv_module_exit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_drv_proc_remove_module(HI_MOD_I2C_NAME);
#endif
    i2c_unregister_dev();

    hi_drv_i2c_deinit();

    return;
}

#ifdef MODULE
module_init(i2c_drv_module_init);
module_exit(i2c_drv_module_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");

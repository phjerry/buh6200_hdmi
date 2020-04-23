/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: SSM function file for Hisilicon SSM
 * Author: ssm group
 * Create: 2019/12/11
 * Notes:
 */


#include <linux/uaccess.h>
#include <linux/module.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include "linux/hisilicon/securec.h"
#include <linux/miscdevice.h>

#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_drv_module.h"
#include "teek_client_api.h"
#include "hi_drv_ssm.h"
#include "drv_ssm_intf.h"
#include "drv_ssm.h"
#include "hi_osal.h"

#define HI_FATAL_SSM(fmt...)     printk(fmt);
#define HI_ERROR_SSM(fmt...)     printk(fmt);

static int drv_ssm_open(struct inode *inode, struct file *filp)
{
    return HI_SUCCESS;
}

static int drv_ssm_release(struct inode *inode, struct file *filp)
{
    drv_ssm_error_handler(filp);
    return HI_SUCCESS;
}

hi_slong drv_ssm_ioctl_func(struct file *file, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case CMD_SSM_CREATE: {
            ssm_ioctl_create *p = (ssm_ioctl_create *)arg;

            ret = drv_ssm_teec_create(file, (p->intent), &(p->ssm_handle));
            if (ret != HI_SUCCESS) {
                HI_ERROR_SSM("ioctl to create fail:%x\n", ret);
                goto error;
            }

            break;
        }
        case CMD_SSM_DESTROY: {
            ssm_ioctl_destroy *p = (ssm_ioctl_destroy *)arg;

            ret = drv_ssm_teec_destroy(p->ssm_handle);
            if (ret != HI_SUCCESS) {
                HI_ERROR_SSM("ioctl to destroy fail:%x\n", ret);
                goto error;
            }

            break;
        }

        case CMD_SSM_ADD_RESOURCE: {
            ssm_ioctl_add_resource *p = (ssm_ioctl_add_resource *)arg;

            ret = drv_ssm_teec_add_resource(p->ssm_handle, p->res_info);
            if (ret != HI_SUCCESS) {
                HI_ERROR_SSM("ioctl to add resource fail:%x\n", ret);
                goto error;
            }

            break;
        }

        case CMD_SSM_ATTACH_BUFFER: {
            ssm_ioctl_attach_buffer *p = (ssm_ioctl_attach_buffer *)arg;

            ret = drv_ssm_teec_attach_buffer(p->attach_info, &(p->secure_info_addr));
            if (ret != HI_SUCCESS) {
                HI_ERROR_SSM("ioctl to attach buffer fail:%x\n", ret);
                goto error;
            }

            break;
        }

        case CMD_SSM_GET_INTENT: {
            ssm_ioctl_get_intent *p = (ssm_ioctl_get_intent *)arg;

            ret = drv_ssm_teec_get_intent(p->ssm_handle, &(p->intent));
            if (ret != HI_SUCCESS) {
                HI_ERROR_SSM("ioctl to get intent fail:%x\n", ret);
                goto error;
            }
            break;
        }
#ifdef SSM_TEST_SUPPORT
        case CMD_SSM_CHECK_BUF_INFO: {
            ssm_ioctl_check_info *p = (ssm_ioctl_check_info *)arg;

            ret = drv_ssm_teec_check_buffer(p->check_info);
            if (ret != HI_SUCCESS) {
                HI_ERROR_SSM("ioctl to check info fail:%x\n", ret);
                goto error;
            }
            break;
        }
#endif
        default: {
            HI_ERROR_SSM("no match ioctl cmd!\n");
            ret = HI_FAILURE;
            break;
        }
    }

error:
    return ret;
}

static long drv_ssm_ioctl(struct file *pointer_file, unsigned int cmd, unsigned long arg)
{
    return (long)hi_drv_user_copy(pointer_file, cmd, arg, drv_ssm_ioctl_func);
}


static struct file_operations g_ssm_fops = {
    .owner          = THIS_MODULE,
    .open           = drv_ssm_open,
    .release        = drv_ssm_release,
    .unlocked_ioctl = drv_ssm_ioctl,
    .compat_ioctl   = drv_ssm_ioctl,
};

static struct miscdevice g_ssm_misc_device = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = HI_DEV_SSM_NAME,
    .fops   = &g_ssm_fops,
};

hi_s32 ssm_mod_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_SSM, "HI_SSM", HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SSM("register demux device failed\n");
        return ret;
    }

    ret = misc_register(&g_ssm_misc_device);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SSM("register device failed\n");
        osal_exportfunc_unregister(HI_ID_SSM);
        return HI_FAILURE;
    }

#ifdef SSM_TEE_SUPPORT
    ret = drv_ssm_teec_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_SSM("smm teec init fail\n");
        misc_deregister(&g_ssm_misc_device);
        osal_exportfunc_unregister(HI_ID_SSM);
        return ret;
    }
#endif

#ifdef MODULE
    HI_PRINT("load hi_ssm.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void ssm_mod_exit(hi_void)
{
    misc_deregister(&g_ssm_misc_device);
    osal_exportfunc_unregister(HI_ID_SSM);

#ifdef SSM_TEE_SUPPORT
    drv_ssm_teec_deinit();
#endif
}

module_init(ssm_mod_init);
module_exit(ssm_mod_exit);

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");


/******************************************************************************
  * Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
  * Description   :
  * Author        : Hisilicon multimedia software group
  * Create        : 2009/12/21
  * History       :
 *******************************************************************************/
#ifdef AVPLAY_NOT_SUPPORT_OSAL
#include <linux/uaccess.h>
#include <linux/module.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/pm.h>
#endif

#include "linux/hisilicon/securec.h"
#include "hi_drv_dev.h"
#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "drv_avplay.h"

#define MOD_AVPLAY_NAME "HI_AVPLAY"

static hi_s32 avplay_drv_open(void *private_data)
{
    return avplay_open(private_data);
}

static hi_s32 avplay_drv_release(void *private_data)
{
    return avplay_release(private_data);
}

static hi_s32 avplay_drv_cmd_create(hi_u32 cmd, void *arg, void *private_data)
{
    avplay_create_param *param = arg;
    return hi_drv_avplay_create(&param->avplay, private_data);
}

static hi_s32 avplay_drv_cmd_destroy(hi_u32 cmd, void *arg, void *private_data)
{
    hi_handle *param = arg;
    return hi_drv_avplay_destroy(*param);
}

static hi_s32 avplay_drv_cmd_reset(hi_u32 cmd, void *arg, void *private_data)
{
    avplay_reset_param *param = arg;
    return hi_drv_avplay_reset(param->avplay, param->chn);
}

static hi_s32 avplay_drv_cmd_set_resource(hi_u32 cmd, void *arg, void *private_data)
{
    avplay_resource_param *param = arg;
    return hi_drv_avplay_set_resource(param->avplay, param->handle, param->type, param->param);
}

static hi_s32 avplay_drv_cmd_wait_event(hi_u32 cmd, void *arg, void *private_data)
{
    avplay_wait_event_param *param = arg;
    return hi_drv_avplay_wait_event(param->avplay, param->event_mask, &param->event,
        &param->param, param->timeout);
}

static hi_s32 avplay_drv_cmd_wakeup(hi_u32 cmd, void *arg, void *private_data)
{
    hi_handle *param = arg;
    return hi_drv_avplay_wakeup(*param);
}

static osal_ioctl_cmd g_avplay_cmd_list[] = {
    {CMD_AVPLAY_CREATE,         avplay_drv_cmd_create},
    {CMD_AVPLAY_DESTROY,        avplay_drv_cmd_destroy},
    {CMD_AVPLAY_RESET,          avplay_drv_cmd_reset},
    {CMD_AVPLAY_SET_RESOURCE,   avplay_drv_cmd_set_resource},
    {CMD_AVPLAY_WAIT_EVENT,     avplay_drv_cmd_wait_event},
    {CMD_AVPLAY_WAKEUP,         avplay_drv_cmd_wakeup}
};

static osal_fileops g_avplay_fops = {
    .open           = avplay_drv_open,
    .release        = avplay_drv_release,
    .cmd_list       = g_avplay_cmd_list,
    .cmd_cnt        = sizeof(g_avplay_cmd_list) / sizeof(osal_ioctl_cmd),
};

static hi_s32 avplay_pm_suspend(void *private_data)
{
    HI_PRINT("avplay suspend ok\n");
    return HI_SUCCESS;
}

static hi_s32 avplay_pm_resume(void *private_data)
{
    HI_PRINT("avplay resume ok\n");
    return HI_SUCCESS;
}

static osal_pmops g_avplay_pm_ops = {
    .pm_suspend = avplay_pm_suspend,
    .pm_resume = avplay_pm_resume,
};

static osal_dev g_avplay_dev = {
    .minor = HI_DEV_AVPLAY_MINOR,
    .fops = &g_avplay_fops,
    .pmops = &g_avplay_pm_ops,
};

static hi_s32 avplay_register_dev(void)
{
    hi_s32 ret;
    osal_dev *dev = &g_avplay_dev;

    ret = strncpy_s(dev->name, sizeof(dev->name), HI_DEV_AVPLAY_NAME, strlen(HI_DEV_AVPLAY_NAME));
    if (ret < 0) {
        HI_ERR_AVPLAY("strncpy_s failed. ret = 0x%x\n", ret);
        return ret;
    }

    ret = osal_dev_register(dev);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AVPLAY("osal_dev_register failed\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 avplay_unregister_dev(void)
{
    osal_dev_unregister(&g_avplay_dev);
    return HI_SUCCESS;
}

hi_s32 avplay_drv_mod_init(void)
{
    hi_s32 ret = hi_drv_module_register(HI_ID_AVPLAY, MOD_AVPLAY_NAME, HI_NULL, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AVPLAY("Register avplay module failed, ret = 0x%x\n", ret);
        return ret;
    }

    ret = avplay_register_dev();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AVPLAY("Register avplay device failed, ret = 0x%x\n", ret);
        return ret;
    }

    ret = avplay_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AVPLAY("Init avplay device failed, ret = 0x%x\n", ret);
        return ret;
    }

#ifdef MODULE
    HI_PRINT("Load hi_avplay.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

void avplay_drv_mode_exit(void)
{
    hi_s32 ret = avplay_unregister_dev();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AVPLAY("Unregister avplay device failed, ret = 0x%x\n", ret);
        return;
    }

    ret = hi_drv_module_unregister(HI_ID_AVPLAY);
    if (ret != HI_SUCCESS) {
        HI_ERR_AVPLAY("Unregister avplay module failed, ret = 0x%x\n", ret);
    }

    ret = avplay_deinit();
    if (ret != HI_SUCCESS) {
        HI_ERR_AVPLAY("Deinit avplay module failed, ret = 0x%x\n", ret);
    }

#ifdef MODULE
    HI_PRINT("Unload hi_avplay.ko success.\n");
#endif
}

#ifdef MODULE
module_init(avplay_drv_mod_init);
module_exit(avplay_drv_mode_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");


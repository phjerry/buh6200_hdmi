/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm_intf
 * Author: sm_rm
 * Create: 2015/11/25
 */

#include "securec.h"

#include "hi_osal.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_drv_win.h"

#include "drv_rm_define.h"
#include "drv_rm_ioctl.h"
#include "drv_rm.h"
#include "drv_rm_intf.h"
#include "hi_drv_dev.h"

rm_export_func g_rm_ext_funcs = {
    .pfn_rm_notify_wind_created = hi_drv_rm_notify_wind_created,
    .pfn_rm_notify_wind_destroyed = hi_drv_rm_notify_wind_destroyed,
};

static hi_s32 drv_rm_create(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    rm_create *create_p = NULL;

    if (arg == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (private_data == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    HI_UNUSED(arg);
    HI_UNUSED(private_data);

    create_p = (rm_create *)arg;
    ret = hi_drv_rm_create(&create_p->rm_handle, create_p->pid);
    return ret;
}

static hi_s32 drv_rm_destroy(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_handle rm_handle;

    if (arg == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (private_data == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    HI_UNUSED(arg);
    HI_UNUSED(private_data);

    rm_handle = *(hi_handle *)arg;
    ret = hi_drv_rm_destroy(rm_handle);
    return ret;
}

static hi_s32 drv_rm_enable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_handle rm_handle;

    if (arg == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (private_data == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    HI_UNUSED(arg);
    HI_UNUSED(private_data);

    rm_handle = *(hi_handle *)arg;
    ret = hi_drv_rm_enable(rm_handle);
    return ret;
}

static hi_s32 drv_rm_disable(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_handle rm_handle;

    if (arg == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (private_data == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    HI_UNUSED(arg);
    HI_UNUSED(private_data);

    rm_handle = *(hi_handle *)arg;
    ret = hi_drv_rm_disable(rm_handle);
    return ret;
}

static hi_s32 drv_rm_query(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    rm_event *rm_query_p = NULL;

    if (arg == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (private_data == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    HI_UNUSED(arg);
    HI_UNUSED(private_data);

    rm_query_p = (rm_event *)arg;
    ret = hi_drv_rm_query_event(rm_query_p->rm_handle, &rm_query_p->rm_event);
    return ret;
}

static hi_s32 drv_rm_acquire(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    rm_win_info *rm_win_info_p = NULL;

    if (arg == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    if (private_data == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    HI_UNUSED(arg);
    HI_UNUSED(private_data);

    rm_win_info_p = (rm_win_info *)arg;
    ret = hi_drv_rm_acquire_window_handle(rm_win_info_p->pid);
    return ret;
}

static osal_ioctl_cmd g_rm_cmd_list[] = {
    { CMD_RM_CREATE,     drv_rm_create },
    { CMD_RM_DESTROY,    drv_rm_destroy },
    { CMD_RM_ENABLE,     drv_rm_enable },
    { CMD_RM_DISABLE,    drv_rm_disable },
    { CMD_RM_QUERY,      drv_rm_query },
    { CMD_RM_ACQUIREWIN, drv_rm_acquire }
};

static hi_s32 rm_drv_open(hi_void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 rm_drv_close(hi_void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 rm_drv_suspend(hi_void *private_data)
{
    HI_PRINT("RM suspend OK\n");
    return HI_SUCCESS;
}

hi_s32 rm_drv_resume(hi_void *private_data)
{
    HI_PRINT("RM resume OK\n");
    return HI_SUCCESS;
}

hi_s32 rm_drv_lowpower_enter(hi_void *private_data)
{
    HI_PRINT("RM lowpower_enter OK\n");
    return HI_SUCCESS;
}

hi_s32 rm_drv_lowpower_exit(hi_void *private_data)
{
    HI_PRINT("RM owpower_exit OK\n");
    return HI_SUCCESS;
}

static osal_fileops g_rm_fileops = {
    .open = rm_drv_open,
    .read = NULL,
    .write = NULL,
    .llseek = NULL,
    .release = rm_drv_close,
    .poll = NULL,
    .mmap = NULL,
    .cmd_list = g_rm_cmd_list,
    .cmd_cnt = 0,
};

static osal_pmops g_rm_pmops = {
    .pm_suspend = rm_drv_suspend,
    .pm_resume = rm_drv_resume,
    .pm_lowpower_enter = rm_drv_lowpower_enter,
    .pm_lowpower_exit = rm_drv_lowpower_exit,
    .pm_poweroff = NULL,
};

static osal_dev g_rm_device = {
    .minor = HI_DEV_RM_MINOR,
    .fops = &g_rm_fileops,
    .pmops = &g_rm_pmops,
};

hi_s32 rm_drv_mod_init(hi_void)
{
    hi_s32 ret;

    g_rm_fileops.cmd_cnt = sizeof(g_rm_cmd_list) / sizeof(osal_ioctl_cmd);

    ret = snprintf_s(g_rm_device.name, OSAL_DEV_NAME_LEN - 1, strlen(HI_DEV_RM_NAME),
        "%s", HI_DEV_RM_NAME);
    if (ret < 0) {
        HI_LOG_ERR("snprintf_s failed\n");
        return HI_FAILURE;
    }

    ret = osal_dev_register(&g_rm_device);
    if (ret != HI_SUCCESS) {
        hi_rm_err_print_call_func(osal_dev_register, ret);
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_RM, "HI_RM", &g_rm_ext_funcs);
    if (ret != HI_SUCCESS) {
        hi_rm_err_print_call_func(osal_exportfunc_register, ret);
        osal_dev_unregister(&g_rm_device);
        return HI_FAILURE;
    }

    hi_drv_rm_init();
#ifdef MODULE
    HI_PRINT("load hi_rm.ko success.\t\t(%s)\n", VERSION_STRING);
#else
    HI_PRINT("RM module init success.\t\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void rm_drv_mod_exit(hi_void)
{
    hi_drv_rm_de_init();
    osal_exportfunc_unregister(HI_ID_RM);
    osal_dev_unregister(&g_rm_device);
#ifdef MODULE
    HI_PRINT("remove hi_rm.ko success.\t\t(%s)\n", VERSION_STRING);
#endif
}

#ifdef MODULE
module_init(rm_drv_mod_init);
module_exit(rm_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");


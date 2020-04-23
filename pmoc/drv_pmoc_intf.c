/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: pmoc driver
 */

#include "drv_pmoc_debug.h"

#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/pm.h>
#include <linux/hisilicon/securec.h>

#include "hi_drv_mem.h"
#include "hi_drv_dev.h"
#include "hi_drv_sys.h"
#include "hi_osal.h"
#include "hi_type.h"
#include "drv_pmoc.h"
#include "drv_pmoc_proc.h"
#include "drv_pmoc_ext.h"

#include "hi_drv_pmoc.h"
#include "drv_pmoc_ioctl.h"
#include "drv_pmoc.h"
#include "drv_pmoc_proc.h"

static pmoc_export_func g_pmoc_export_funcs = {
    .get_chip_temperature = pmoc_tsensor_read,
};

static hi_s32 pmoc_suspend(hi_void *private_data)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    /* TBD: send ddr wakeup check areas to hrf */
    ret = pmoc_load_standby_params();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_load_standby_params, ret);
        return HI_FAILURE;
    }

    HI_PRINT("pmoc suspend ok.\n");

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_shutdown(hi_void *private_data)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    ret = pmoc_set_wakeup_type_to_hrf(HI_PMOC_WAKEUP_RESET);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_set_wakeup_type_to_hrf, ret);
        return HI_FAILURE;
    }

    HI_PRINT("pmoc poweroff ok.\n");

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_resume_early(hi_void *private_data)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    ret = pmoc_tsensor_init();
    if (ret != HI_SUCCESS) {
        HI_WARN_PRINT_FUNC_RES(pmoc_tsensor_init, ret);
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_resume(hi_void *private_data)
{
    HI_FUNC_ENTER();

    pmoc_get_wakeup_message(); /* for proc of ddr wakeup */

    HI_PRINT("pmoc resume ok.\n");

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static osal_pmops g_pmoc_pm_ops = {
    .pm_suspend = pmoc_suspend,
    .pm_resume_early   = pmoc_resume_early,
    .pm_resume  = pmoc_resume,
    .pm_lowpower_enter = NULL,
    .pm_lowpower_exit = NULL,
    .pm_poweroff = pmoc_shutdown, /* shutdown */
    .private_data = NULL,
};

static hi_s32 pmoc_open(hi_void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 pmoc_release(hi_void *private_data)
{
    return HI_SUCCESS;
}

static osal_ioctl_cmd g_pmoc_ioctl_func[] = {
    { CMD_PMOC_SET_SUSPEND_ATTR,       pmoc_ioctl },
    { CMD_PMOC_GET_SUSPEND_ATTR,       pmoc_ioctl },
    { CMD_PMOC_SET_WAKEUP_TYPE,        pmoc_ioctl },
    { CMD_PMOC_STANDBY_READY,          pmoc_ioctl },
    { CMD_PMOC_GET_WAKEUP_ATTR,        pmoc_ioctl },
    { CMD_PMOC_GET_STANDBY_PERIOD,     pmoc_ioctl },
    { CMD_PMOC_CLEAN_WAKEUP_PARAM,     pmoc_ioctl },
    { CMD_PMOC_ENTER_ACTIVE_STANDBY,   pmoc_ioctl },
    { CMD_PMOC_QUIT_ACTIVE_STANDBY,    pmoc_ioctl },
    { CMD_PMOC_SET_DISPLAY_PARAM,      pmoc_ioctl },
    { CMD_PMOC_SET_GPIO_POWEROFF,      pmoc_ioctl },
    { CMD_PMOC_GET_CHIP_TEMPERATURE,   pmoc_ioctl },
};

static osal_fileops g_pmoc_fileops = {
    .open = pmoc_open,
    .release = pmoc_release,
    .cmd_list = g_pmoc_ioctl_func,
    .cmd_cnt = 0,
};

static osal_dev g_pmoc_dev = {
    .fops = &g_pmoc_fileops,
    .minor = HI_DEV_PM_MINOR,
    .pmops = &g_pmoc_pm_ops,
};

static hi_s32 pmoc_register_dev(hi_void)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    g_pmoc_fileops.cmd_cnt = sizeof(g_pmoc_ioctl_func) / sizeof(osal_ioctl_cmd);

    ret = pmoc_sem_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_sem_init, ret);
        return ret;
    }

    ret = strncpy_s(g_pmoc_dev.name, sizeof(g_pmoc_dev.name), HI_DEV_PM_NAME, sizeof(HI_DEV_PM_NAME));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(strncpy_s, ret);
    }

    if (osal_dev_register(&g_pmoc_dev) < 0) {
        HI_ERR_PRINT_FUNC_RES(osal_dev_register, ret);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_void pmoc_unregister_dev(hi_void)
{
    HI_FUNC_ENTER();

    osal_dev_unregister(&g_pmoc_dev);

    HI_FUNC_EXIT();
    return;
}

hi_s32 pmoc_drv_mod_init(hi_void)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    ret = osal_exportfunc_register(HI_ID_PM, "HI_PM", (hi_void *)&g_pmoc_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_register, ret);
        return ret;
    }

    ret = pmoc_register_dev();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_register_dev, ret);
        goto out_module_unregister;
    }

    ret = pmoc_register_remap();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_register_remap, ret);
        goto out_dev_unregister;
    }

    ret = pmoc_load_lpmcu();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_load_lpmcu, ret);
        goto out_register_unmap;
    }

#ifdef HI_PROC_SUPPORT
    ret = pmoc_register_proc();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_register_proc, ret);
        goto out_register_unmap;
    }
#endif

    ret = pmoc_tsensor_init();
    if (ret != HI_SUCCESS) {
        HI_WARN_PRINT_FUNC_RES(pmoc_tsensor_init, ret);
    }

    pmoc_get_wakeup_message(); /* for proc of reboot wakeup */

#ifdef MODULE
    HI_PRINT("Load hi_pmoc.ko success.\t(%s)\n", VERSION_STRING);
#endif

    HI_FUNC_EXIT();
    return HI_SUCCESS;

out_register_unmap:
    pmoc_register_unmap();
out_dev_unregister:
    pmoc_unregister_dev();
out_module_unregister:
    (hi_void)osal_exportfunc_unregister(HI_ID_PM);

    return HI_FAILURE;
}

hi_void pmoc_drv_mod_exit(hi_void)
{
    HI_FUNC_ENTER();

#ifdef HI_PROC_SUPPORT
    pmoc_remove_proc();
#endif

    pmoc_register_unmap();

    pmoc_unregister_dev();

    (hi_void)osal_exportfunc_unregister(HI_ID_PM);

    HI_FUNC_EXIT();
    return;
}

#ifdef MODULE
module_init(pmoc_drv_mod_init);
module_exit(pmoc_drv_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");

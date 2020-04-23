/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: device node operation
 * Author: sdk
 * Create: 2018-12-10
 */

#include <linux/module.h>
#include "hi_osal.h"

#include "drv_gfx2d_device.h"
#include "hi_gfx_comm_k.h"

#include "hi_drv_gfx2d.h"
#include "drv_gfx2d_proc.h"
#include "drv_gfx2d_debug.h"
#include "drv_gfx2d_config.h"
#include "drv_gfx2d_ctl.h"
#include "drv_gfx2d_intf.h"
#include "drv_gfx2d_fence.h"
#include "drv_gfx2d_list.h"

/* Macro Definition */
#define GFX2D_DEV_NAME "hi_gfx2d"
#define HI_GFX2D_DEV_ID_0 0x0

/* Global Variable declaration */
static hi_u32 g_kmod_mem_pool_size = 0;

/* API forward declarations */
static hi_s32 drv_gfx2d_dev_register(hi_void);
static hi_void drv_gfx2d_dev_unregister(hi_void);

static hi_void gfx2d_show_version(hi_bool is_load);
#ifdef CONFIG_GFX_SHOW_CMD_SUPPORT
static hi_void gfx2d_show_cmd_value(hi_void);
#endif

static hi_gfx2d_export_func g_gfx2d_export_func = {
    .drv_gfx2d_module_init = drv_gfx2d_dev_register,
    .drv_gfx2d_module_exit = drv_gfx2d_dev_unregister,
    // .drv_gfx2d_module_suspend = drv_gfx2d_dev_suspend,
    // .drv_gfx2d_module_resume  = drv_gfx2d_dev_resume,
};

/* API realization */
hi_s32 drv_gfx2d_dev_suspend(struct device *dev)
{
    (hi_void) gfx2d_wait_done(HI_GFX2D_DEV_ID_0, 0);

    GRAPHIC_COMM_PRINT("gfx2d suspend ok\n");
    return HI_SUCCESS;
}

hi_s32 drv_gfx2d_dev_resume(struct device *dev)
{
    hi_s32 ret;

    ret = GFX2D_CTL_Resume();

    GRAPHIC_COMM_PRINT("gfx2d resume %s\n", (ret == HI_SUCCESS) ? "OK" : "NOK");
    return HI_SUCCESS;
}

hi_s32 drv_gfx2d_dev_resume_early(struct device *dev)
{
    return HI_SUCCESS;
}

hi_s32 drv_gfx2d_dev_open(struct inode *finode, struct file *ffile)
{
    return GFX2D_CTL_Open();
}

hi_s32 drv_gfx2d_dev_colse(struct inode *finode, struct file *ffile)
{
    return GFX2D_CTL_Close();
}

hi_s32 hi_drv_gfx2d_module_init(hi_void)
{
    return drv_gfx2d_dev_register();
}

hi_void hi_drv_gfx2d_module_exit(hi_void)
{
#ifdef GFX2D_FENCE_SUPPORT
    gfx2d_deinit_fence_lock();
#endif
    gfx2d_deinit_task_mem();
    drv_gfx2d_dev_unregister();
    gfx2d_ctl_deinit_mutex();
}

static hi_s32 drv_gfx2d_dev_register(hi_void)
{
    hi_s32 ret;

    ret = hi_gfx_module_register(HIGFX_GFX2D_ID, "HI_GFX2D", &g_gfx2d_export_func);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_gfx_module_register, ret);
        return HI_FAILURE;
    }

    ret = GFX2D_CONFIG_SetMemSize(g_kmod_mem_pool_size);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_CONFIG_SetMemSize, ret);
        hi_gfx_module_unregister(HIGFX_GFX2D_ID);
        return HI_FAILURE;
    }

    ret = GFX2D_CTL_Init();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_CTL_Init, ret);
        hi_gfx_module_unregister(HIGFX_GFX2D_ID);
        return HI_FAILURE;
    }

    ret = drv_gfx2d_pm_register();
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_gfx2d_pm_register, ret);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Register module failed");
        GFX2D_CTL_Deinit();
        hi_gfx_module_unregister(HIGFX_GFX2D_ID);
        return HI_FAILURE;
    }

    if (gfx2d_ctl_init_mutex() != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, gfx2d_init_task_mem, ret);
        GFX2D_CTL_Deinit();
        drv_gfx2d_pm_unregister();
        hi_gfx_module_unregister(HIGFX_GFX2D_ID);
        return HI_FAILURE;
    }

#ifdef GFX2D_FENCE_SUPPORT
    if (gfx2d_init_fence_lock() != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, gfx2d_init_task_mem, ret);
        hi_gfx_module_unregister(HIGFX_GFX2D_ID);
        GFX2D_CTL_Deinit();
        return HI_FAILURE;
    }
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
    gfx2d_register_proc();
#endif

    gfx2d_show_version(HI_TRUE);

#ifdef CONFIG_GFX_SHOW_CMD_SUPPORT
    gfx2d_show_cmd_value();
#endif
    return HI_SUCCESS;
}

#ifdef CONFIG_GFX_SHOW_CMD_SUPPORT
static hi_void gfx2d_show_cmd_value(hi_void)
{
    GRAPHIC_COMM_PRINT("0x%x   /**<-- DRV_GFX2D_CMD_COMPOSE              >**/\n", DRV_GFX2D_CMD_COMPOSE);
    GRAPHIC_COMM_PRINT("0x%x   /**<-- DRV_GFX2D_CMD_WAIT_DONE            >**/\n", DRV_GFX2D_CMD_WAIT_DONE);
    return;
}
#endif

static hi_void drv_gfx2d_dev_unregister(hi_void)
{
#ifdef CONFIG_GFX_PROC_SUPPORT
    gfx2d_unregister_proc();
#endif

#ifdef GFX2D_FENCE_SUPPORT
    gfx2d_deinit_fence_lock();
#endif

    gfx2d_ctl_deinit_mutex();

    drv_gfx2d_pm_unregister();

    GFX2D_CTL_Deinit();

    hi_gfx_module_unregister(HIGFX_GFX2D_ID);

    gfx2d_show_version(HI_FALSE);

    return;
}

static hi_void gfx2d_show_version(hi_bool is_load)
{
#if !defined(CONFIG_GFX_COMM_VERSION_DISABLE) && !defined(CONFIG_GFX_COMM_DEBUG_DISABLE) && defined(MODULE)
    if (is_load == HI_TRUE) {
        GRAPHIC_COMM_PRINT("Load hi_gfx2d.ko success.\t\t(%s)\n", VERSION_STRING);
    } else {
        GRAPHIC_COMM_PRINT("UnLoad hi_gfx2d.ko success.\t(%s)\n", VERSION_STRING);
    }
#endif
}

#ifdef MODULE
module_init(hi_drv_gfx2d_module_init);
module_exit(hi_drv_gfx2d_module_exit);
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
MODULE_DESCRIPTION("Hisilicon GFX2D Device Driver");
MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
#else
MODULE_DESCRIPTION("");
MODULE_AUTHOR("");
MODULE_LICENSE("GPL");
#endif

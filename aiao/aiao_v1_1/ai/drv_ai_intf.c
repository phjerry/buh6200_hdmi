/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ai device and driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_drv_dev.h"

#include "hi_drv_ai.h"
#include "drv_ai_private.h"
#include "drv_ai_ioctl.h"

static osal_ioctl_cmd g_ai_cmd_list[] = {
    { CMD_AI_GETDEFAULTATTR, ai_drv_ioctl },
    { CMD_AI_CREATE, ai_drv_ioctl },
    { CMD_AI_DESTROY, ai_drv_ioctl },
    { CMD_AI_SETENABLE, ai_drv_ioctl },
    { CMD_AI_GETENABLE, ai_drv_ioctl },
    { CMD_AI_ACQUIREFRAME, ai_drv_ioctl },
    { CMD_AI_RELEASEFRAME, ai_drv_ioctl },
    { CMD_AI_SETATTR, ai_drv_ioctl },
    { CMD_AI_GETATTR, ai_drv_ioctl },
    { CMD_AI_GETBUFINFO, ai_drv_ioctl },
    { CMD_AI_SETBUFINFO, ai_drv_ioctl },
    { CMD_AI_SETDELAYCOMPS, ai_drv_ioctl },
    { CMD_AI_GETDELAYCOMPS, ai_drv_ioctl },
#if 0
    { CMD_AI_GETPORTDELAY, ai_drv_ioctl },
    { CMD_AI_GETSTREAMTYPE, ai_drv_ioctl },
    { CMD_AI_SETNRATTRS, ai_drv_ioctl },
    { CMD_AI_GETNRATTRS, ai_drv_ioctl },
    { CMD_AI_SETLINEINVOLUME, ai_drv_ioctl },
    { CMD_AI_GETLINEINVOLUME, ai_drv_ioctl },
    { CMD_AI_SETLINEINMUTE, ai_drv_ioctl },
    { CMD_AI_GETLINEINMUTE, ai_drv_ioctl },
    { CMD_AI_GETSTATUS, ai_drv_ioctl },
#endif
    { CMD_AI_PROCINIT, ai_drv_ioctl },
    { CMD_AI_PROCDEINIT, ai_drv_ioctl },
};

static osal_fileops g_ai_fops = {
    .open = ai_drv_open,
    .read = HI_NULL,
    .write = HI_NULL,
    .llseek = HI_NULL,
    .release = ai_drv_release,
    .poll = HI_NULL,
    .mmap = HI_NULL,
    .cmd_list = g_ai_cmd_list,
    .cmd_cnt = sizeof(g_ai_cmd_list) / sizeof(osal_ioctl_cmd),
};

static osal_pmops g_ai_pm_ops = {
    .pm_suspend = ai_drv_suspend,
    .pm_resume = ai_drv_resume,
    .pm_lowpower_enter = HI_NULL,
    .pm_lowpower_exit = HI_NULL,
    .pm_poweroff = HI_NULL,
    .private_data = HI_NULL,
};

/* ai device */
static osal_dev g_ai_dev = {
    .name   = HI_DEV_AI_NAME,
    .minor  = HI_DEV_AI_MINOR,
    .fops   = &g_ai_fops,
    .pmops  = &g_ai_pm_ops,
};

hi_s32 ai_drv_mod_init(hi_void)
{
    hi_s32 ret;

    ai_osal_init();

    ret = ai_drv_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ai_drv_init, ret);
        return ret;
    }

    ret = osal_dev_register(&g_ai_dev);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_dev_register, ret);
        ai_drv_exit();
        return ret;
    }

    return HI_SUCCESS;
}

hi_void ai_drv_mod_exit(hi_void)
{
    osal_dev_unregister(&g_ai_dev);
    ai_drv_exit();
    ai_osal_deint();
}


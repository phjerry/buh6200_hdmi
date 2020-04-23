/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: osr manage
 * Author: sdk
 * Create: 2019-03-18
 */

#include <linux/module.h>
#include "hi_osal.h"

#include "tde_proc.h"
#include "tde_osilist.h"
#include "tde_debug.h"
#include "drv_tde_intf.h"

/* **************************** Macro Definition ******************************************* */
#define MKSTR(exp) # exp
#define MKMARCOTOSTR(exp) MKSTR(exp)

hi_u32 g_tde_tmp_buffer_size = 0;
module_param(g_tde_tmp_buffer_size, uint, S_IRUGO);

MODULE_PARM_DESC(g_tde_tmp_buffer_size, "TDE Tmp buffer.");

extern hi_s32 drv_tde_module_init_k(hi_void);
extern hi_void drv_tde_module_exit_k(hi_void);

/* ****************************** API Definition ********************************************** */
#ifdef CONFIG_GFX_PROC_SUPPORT
STATIC hi_void HI_GFX_ShowVersionK(hi_bool bLoad)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#if !defined(CONFIG_GFX_COMM_VERSION_DISABLE) && !defined(CONFIG_GFX_COMM_DEBUG_DISABLE) && defined(MODULE)
    if (bLoad == HI_TRUE) {
        GRAPHIC_COMM_PRINT("Load hi_tde.ko success.\t\t(%s)\n", VERSION_STRING);
    } else {
        GRAPHIC_COMM_PRINT("UnLoad hi_tde.ko success.\t\t(%s)\n", VERSION_STRING);
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

#if 0
#include "drv_ioctl_tde.h"
#endif
static hi_void TDE_ShowCmd(hi_void)
{
#if 0
    HI_PRINT("0x%x   /**<-- TDE_BEGIN_JOB                >**/\n", TDE_BEGIN_JOB);
    HI_PRINT("0x%x   /**<-- TDE_BIT_BLIT                 >**/\n", TDE_BIT_BLIT);
    HI_PRINT("0x%x   /**<-- TDE_SOLID_DRAW               >**/\n", TDE_SOLID_DRAW);
    HI_PRINT("0x%x   /**<-- TDE_QUICK_COPY               >**/\n", TDE_QUICK_COPY);
    HI_PRINT("0x%x   /**<-- TDE_QUICK_RESIZE             >**/\n", TDE_QUICK_RESIZE);
    HI_PRINT("0x%x   /**<-- TDE_QUICK_FILL               >**/\n", TDE_QUICK_FILL);
    HI_PRINT("0x%x   /**<-- TDE_QUICK_DEFLICKER          >**/\n", TDE_QUICK_DEFLICKER);
    HI_PRINT("0x%x   /**<-- TDE_MB_BITBLT                >**/\n", TDE_MB_BITBLT);
    HI_PRINT("0x%x   /**<-- TDE_END_JOB                  >**/\n", TDE_END_JOB);
    HI_PRINT("0x%x   /**<-- TDE_WAITFORDONE              >**/\n", TDE_WAITFORDONE);
    HI_PRINT("0x%x   /**<-- TDE_CANCEL_JOB               >**/\n", TDE_CANCEL_JOB);
    HI_PRINT("0x%x   /**<-- TDE_BITMAP_MASKROP           >**/\n", TDE_BITMAP_MASKROP);
    HI_PRINT("0x%x   /**<-- TDE_BITMAP_MASKBLEND         >**/\n", TDE_BITMAP_MASKBLEND);
    HI_PRINT("0x%x   /**<-- TDE_WAITALLDONE              >**/\n", TDE_WAITALLDONE);
    HI_PRINT("0x%x   /**<-- TDE_RESET                    >**/\n", TDE_RESET);
    HI_PRINT("0x%x   /**<-- TDE_TRIGGER_SEL              >**/\n", TDE_TRIGGER_SEL);
    HI_PRINT("0x%x   /**<-- TDE_SET_DEFLICKERLEVEL       >**/\n", TDE_SET_DEFLICKERLEVEL);
    HI_PRINT("0x%x   /**<-- TDE_GET_DEFLICKERLEVEL       >**/\n", TDE_GET_DEFLICKERLEVEL);
    HI_PRINT("0x%x   /**<-- TDE_SET_ALPHATHRESHOLD_VALUE          >**/\n", TDE_SET_ALPHATHRESHOLD_VALUE);
    HI_PRINT("0x%x   /**<-- TDE_GET_ALPHATHRESHOLD_VALUE          >**/\n", TDE_GET_ALPHATHRESHOLD_VALUE);
    HI_PRINT("0x%x   /**<-- TDE_SET_ALPHATHRESHOLD_STATE          >**/\n", TDE_SET_ALPHATHRESHOLD_STATE);
    HI_PRINT("0x%x   /**<-- TDE_GET_ALPHATHRESHOLD_STATE          >**/\n", TDE_GET_ALPHATHRESHOLD_STATE);
    HI_PRINT("0x%x   /**<-- TDE_PATTERN_FILL                      >**/\n", TDE_PATTERN_FILL);
    HI_PRINT("0x%x   /**<-- TDE_ENABLE_REGIONDEFLICKER            >**/\n", TDE_ENABLE_REGIONDEFLICKER);
#endif
    return;
}

/* API realization */
static hi_s32 osal_cmd_check(hi_u32 in_argc, hi_u32 aspect_argc, hi_void *arg, hi_void *private)
{
    if (in_argc < aspect_argc) {
        GRAPHIC_COMM_PRINT("in_argc is %d, aspect_argc is %d\n", in_argc, aspect_argc);
        return HI_FAILURE;
    }

    if (private == HI_NULL) {
        GRAPHIC_COMM_PRINT("private is null\n");
        return HI_FAILURE;
    }

    if (arg == HI_NULL) {
        GRAPHIC_COMM_PRINT("arg is null\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 tde_proc_get_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    if (osal_cmd_check(argc, 1, private, private) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    osal_printk("-------------------hi_tde debug options------------------------------ \n"
                "echo command      args         > /proc/msp/hi_tde                     \n");
    osal_printk("----------------------------------------------------------------------\n");
    osal_printk("echo set_log_level level    > /proc/msp/hi_tde | level(0 ~ n)\n");
    osal_printk("echo save_log      level    > /proc/msp/hi_tde | level(0 ~ n)\n");

    return HI_SUCCESS;
}

static hi_s32 tde_proc_set_log_print_level(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 log_print_level;
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) {  // 2 is para index
        return HI_FAILURE;
    }

    log_print_level = (hi_u32)(osal_strtol(argv[1], NULL, OSAL_BASE_DEC));
    HI_GFX_SetLogLevel(log_print_level);

    return HI_SUCCESS;
}

static hi_s32 tde_proc_save_tde_log(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 log_save_level;
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) {  // 2 is para index
        return HI_FAILURE;
    }

    log_save_level = (hi_u32)(osal_strtol(argv[1], NULL, OSAL_BASE_DEC));
    HI_GFX_SetLogSave(log_save_level);

    return HI_SUCCESS;
}

osal_proc_cmd g_tde_proc[] = {
    { "help", tde_proc_get_help },
    { "set_log_level", tde_proc_set_log_print_level },
    { "save_log", tde_proc_save_tde_log },
};

/***************************************************************************************
* func          : hi_drv_tde_module_init
* description   : init tde mod
                  CNcomment: 加载驱动初始化 CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
hi_s32 hi_drv_tde_module_init(hi_void)
{
    hi_s32 Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_MCE_SUPPORT
    Ret = drv_tde_tasklet_init();
    if (Ret != 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_tasklet_init, FAILURE_TAG);
        return -1;
    }

    Ret = drv_tde_module_init_k();
    if (Ret != 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_module_init_k, FAILURE_TAG);
        drv_tde_tasklet_dinit();
        return -1;
    }
#endif

    Ret = drv_tde_pm_register();
    if (Ret != 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_gfx_pm_register, FAILURE_TAG);
        drv_tde_module_exit_k();
        return -1;
    }

#ifdef CONFIG_GFX_PROC_SUPPORT
    Ret = tde_proc_init();
    if (Ret != 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_proc_init, FAILURE_TAG);
        drv_tde_module_exit_k();
        return -1;
    }
    {
        osal_proc_entry *proc_item = HI_NULL;

        proc_item = osal_proc_add("hi_tde", strlen("hi_tde"));
        if (proc_item == HI_NULL) {
            GRAPHIC_COMM_PRINT("proc_item is null\n");
            tde_proc_dinit();
            drv_tde_module_exit_k();
            return HI_FAILURE;
        }

        proc_item->private = HI_NULL;
        proc_item->read = tde_read_proc;
        proc_item->cmd_list = g_tde_proc;
        proc_item->cmd_cnt = sizeof(g_tde_proc) / sizeof(osal_proc_cmd);
    }
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
    HI_GFX_ShowVersionK(HI_TRUE);
#endif

    TDE_ShowCmd();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return 0;
}

hi_void hi_drv_tde_module_exit(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifdef CONFIG_GFX_PROC_SUPPORT
    osal_proc_remove("hi_tde", strlen("hi_tde"));
    tde_proc_dinit();
#endif

#ifndef HI_MCE_SUPPORT
    drv_tde_module_exit_k();
#endif
    drv_tde_pm_unregister();
#ifdef CONFIG_GFX_PROC_SUPPORT
    HI_GFX_ShowVersionK(HI_FALSE);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

#ifdef MODULE
module_init(hi_drv_tde_module_init);
module_exit(hi_drv_tde_module_exit);
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE("GPL");
MODULE_VERSION(TDE_VERSION);
#else
MODULE_AUTHOR("");
MODULE_DESCRIPTION("");
MODULE_LICENSE("GPL");
MODULE_VERSION("");
#endif

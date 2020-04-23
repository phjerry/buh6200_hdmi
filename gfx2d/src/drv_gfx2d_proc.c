/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: proc operation
 * Author: sdk
 * Create: 2019-05-17
 */

#ifdef CONFIG_GFX_PROC_SUPPORT

#include "drv_gfx2d_proc.h"
#include "hi_gfx_comm_k.h"

#include "drv_gfx2d_ctl.h"
#include "drv_gfx2d_debug.h"
#include "drv_gfx2d_hal_hwc.h"
#include <linux/hisilicon/securec.h>

/* Macro Definition */
#define GFX2D_PROC_NAME "hi_gfx2d"

/* Structure Definition */
typedef struct {
    hi_u32 debug_level;
    hi_u32 open_dev_times;
} gfx2d_proc_info;

/* Global Variable declaration */
static gfx2d_proc_info *g_gfx2d_proc_info = NULL;

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

hi_s32 gfx2d_proc_get_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    if (osal_cmd_check(argc, 1, private, private) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    osal_printk("-------------------gfx2d debug options------------------------------ \n"
                "echo command      args         > /proc/msp/gfx2d                     \n");
    osal_printk("---------------------------------------------------------------------\n");
    osal_printk("echo gfx_log_level level    > /proc/msp/hi_gfx2d | level(0 ~ n)\n");
    osal_printk("echo gfx_log_save  level    > /proc/msp/hi_gfx2d | level(0 ~ n)\n");

    return HI_SUCCESS;
}

static hi_s32 gfx2d_proc_set_log_print_level(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 log_print_level;
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) {  // 2 is para index
        return HI_FAILURE;
    }

    log_print_level = (hi_u32)(osal_strtol(argv[1], NULL, OSAL_BASE_DEC));
    HI_GFX_SetLogLevel(log_print_level);
    gfx2d_set_debug_level(log_print_level);

    return HI_SUCCESS;
}

static hi_s32 gfx2d_proc_save_gfx_log(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 log_save_level;
    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) {  // 2 is para index
        return HI_FAILURE;
    }

    log_save_level = (hi_u32)(osal_strtol(argv[1], NULL, OSAL_BASE_DEC));
    HI_GFX_SetLogSave(log_save_level);

    return HI_SUCCESS;
}

osal_proc_cmd g_gfx2d_proc[] = {
    { "help", gfx2d_proc_get_help },
    { "gfx_log_level", gfx2d_proc_set_log_print_level },
    { "gfx_log_save", gfx2d_proc_save_gfx_log },
};

static hi_s32 gfx2d_proc_read(hi_void *p, hi_void *v)
{
    gfx2d_read_ctl_proc(p, v);
    HI_GFX_ProcMsg(p);
    return HI_SUCCESS;
}

hi_s32 gfx2d_register_proc(hi_void)
{
    osal_proc_entry *proc_item = HI_NULL;

    g_gfx2d_proc_info = (gfx2d_proc_info *)HI_GFX_VMALLOC(HIGFX_GFX2D_ID, sizeof(gfx2d_proc_info));
    if (g_gfx2d_proc_info == NULL) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_VMALLOC, FAILURE_TAG);
        return HI_FAILURE;
    }

    memset_s((hi_void *)g_gfx2d_proc_info, sizeof(gfx2d_proc_info), 0x0, sizeof(gfx2d_proc_info));
    proc_item = osal_proc_add(GFX2D_PROC_NAME, strlen(GFX2D_PROC_NAME));
    if (proc_item == HI_NULL) {
        HI_GFX_VFREE(HIGFX_GFX2D_ID, g_gfx2d_proc_info);
        GRAPHIC_COMM_PRINT("proc_item is null\n");
        return HI_FAILURE;
    }

    proc_item->private = HI_NULL;
    proc_item->read = gfx2d_proc_read;
    proc_item->cmd_list = g_gfx2d_proc;
    proc_item->cmd_cnt = sizeof(g_gfx2d_proc) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void gfx2d_unregister_proc(hi_void)
{
    if (g_gfx2d_proc_info == NULL) {
        return;
    }

    osal_proc_remove(GFX2D_PROC_NAME, strlen(GFX2D_PROC_NAME));
    HI_GFX_VFREE(HIGFX_GFX2D_ID, g_gfx2d_proc_info);
    g_gfx2d_proc_info = NULL;

    return;
}

#endif

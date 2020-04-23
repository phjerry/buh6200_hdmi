/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: proc file
 */

#include "drv_pmoc_proc.h"
#include "drv_pmoc_debug.h"

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_proc.h"
#include "hi_osal.h"

#include "drv_pmoc.h"

#define GPIO_PORT_NUM 8
#define NORMAL_GPIO_NUM 23

static hi_s32 chip_temperature_proc_read(hi_void *seqfile, hi_void *private)
{
    hi_s32 ret;
    hi_s16 temperature = 0;

    HI_FUNC_ENTER();

    ret = pmoc_tsensor_read(&temperature);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_tsensor_read, ret);
        return HI_FAILURE;
    }

    osal_proc_print(seqfile, "Chip Temperature         \t :%d\n", temperature);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pm_proc_read(hi_void *seqfile, hi_void *private)
{
    hi_s32 ret;
    pmoc_proc_info info = {{0}};

    HI_FUNC_ENTER();

    ret = pmoc_get_proc_info(&info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmoc_get_proc_info, ret);
        return HI_FAILURE;
    }

    /* TBD: */

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 pmoc_proc_helper(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("==================PMOC HELP==============\n");
    osal_printk("USAGE: echo [cmd] [value] > /proc/msp/pm\n");
    osal_printk("example:\n");
    osal_printk("echo debug 0/1 > /proc/msp/pm\n");

    return HI_SUCCESS;
}

static hi_s32 pmoc_set_debug_message(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 dbg_mask;

    HI_FUNC_ENTER();

    if (argc != 2) { /* 2 param */
        return pmoc_proc_helper(0, 0, 0);
    }

    dbg_mask = (hi_u32)osal_strtoul(argv[1], NULL, OSAL_BASE_DEC);

    pmoc_set_lpmcu_dbg_level(dbg_mask);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static osal_proc_cmd g_pm_proc_cmd[] = {
    {"debug", pmoc_set_debug_message},
    {"help",  pmoc_proc_helper},
};

hi_s32 pmoc_register_proc(hi_void)
{
    osal_proc_entry *item = HI_NULL;

    HI_FUNC_ENTER();

    item = osal_proc_add("pm", strlen("pm"));
    if (item == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_proc_add, HI_FAILURE);
        return HI_FAILURE;
    }

    item->read  = pm_proc_read;
    item->cmd_list = g_pm_proc_cmd;
    item->cmd_cnt = sizeof(g_pm_proc_cmd) / sizeof(osal_proc_cmd);

    item = osal_proc_add("chip_temperature", strlen("chip_temperature"));
    if (item == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_proc_add, HI_FAILURE);

        osal_proc_remove("pm", strlen("pm"));
        return HI_FAILURE;
    }

    item->read = chip_temperature_proc_read;

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_void pmoc_remove_proc(hi_void)
{
    HI_FUNC_ENTER();

    osal_proc_remove("chip_temperature", strlen("chip_temperature"));
    osal_proc_remove("pm", strlen("pm"));

    HI_FUNC_EXIT();
    return;
}


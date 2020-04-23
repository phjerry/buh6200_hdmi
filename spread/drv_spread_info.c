/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: clk spread drv
 * Author: BSP
 * Create: 2019-11-29
 */

#include <linux/module.h>
#include "hi_debug.h"
#include "hi_osal.h"
#include "hi_reg_common.h"
#include "drv_spread.h"
#include "drv_spread_ioctl.h"
#include "hi_drv_spread.h"

#define UMAP_DEVNAME_SPREAD             "hi_spread"
#define HI_MOD_SPREAD                   "spread"
#define UMAP_MIN_MINOR_SPREAD           0

osal_semaphore g_spread_mutex;

hi_s32 spread_set_ddr_spread_en(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_ddr_spread_en((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_ddr_spread_ratio(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_ddr_spread_ratio((hi_u32 *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_ddr_spread_freq(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_ddr_spread_freq((hi_u32 *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}


hi_s32 spread_set_gmac_clk_en(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_gmac_clk_en((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}


hi_s32 spread_set_gmac_spread_en(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_gmac_spread_en((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_gmac_spread_ratio(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_gmac_spread_ratio((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_gmac_spread_freq(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_gmac_spread_freq((hi_u32 *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_emmc_spread_en(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_emmc_spread_en((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_emmc_clk_en(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_emmc_clk_en((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_emmc_spread_ratio(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_emmc_spread_ratio((hi_u32 *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_emmc_spread_freq(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_emmc_spread_freq((hi_u32 *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

hi_s32 spread_set_ci_clk_en(unsigned int cmd,
    hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    if (arg == NULL) {
        hi_err_spread("null pointer error!\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down(&g_spread_mutex);
    if (ret) {
        hi_err_spread("osal_sem_down failed!\n");
        return HI_FAILURE;
    }

    ret = hi_drv_ss_set_ci_clk_en((hi_bool *)arg);

    osal_sem_up(&g_spread_mutex);

    return ret;
}

static osal_ioctl_cmd g_spread_ioctl_info[] = {
    {CMD_SPREAD_SET_DDR_ENABLE, spread_set_ddr_spread_en},
    {CMD_SPREAD_SET_DDR_RATIO, spread_set_ddr_spread_ratio},
    {CMD_SPREAD_SET_DDR_FREQ, spread_set_ddr_spread_freq},

    {CMD_SPREAD_SET_GMAC_CLKEN, spread_set_gmac_clk_en},
    {CMD_SPREAD_SET_GMAC_ENABLE, spread_set_gmac_spread_en},
    {CMD_SPREAD_SET_GMAC_RATIO, spread_set_gmac_spread_ratio},
    {CMD_SPREAD_SET_GMAC_FREQ, spread_set_gmac_spread_freq},

    {CMD_SPREAD_SET_EMMC_CLKEN, spread_set_emmc_clk_en},
    {CMD_SPREAD_SET_EMMC_ENABLE, spread_set_emmc_spread_en},
    {CMD_SPREAD_SET_EMMC_RATIO, spread_set_emmc_spread_ratio},
    {CMD_SPREAD_SET_EMMC_FREQ, spread_set_emmc_spread_freq},

    {CMD_SPREAD_SET_CI_CLKEN, spread_set_ci_clk_en},
};

static hi_s32 spread_proc_show(hi_void *s, hi_void *p_arg)
{
    if (s == HI_NULL) {
        hi_err_spread("spread_proc_show param is null!\n");
        return -1;
    }

    osal_proc_print(s, "\n------------------------Hisilicon spread info-------------------------\n");

    return 0;
}

hi_s32 spread_proc_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("echo [ddrratio][0~31] > /proc/msp/spread\n");
    return 0;
}

hi_s32 spread_proc_set_ddr_ratio(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 max_ratio;

    if (argc != 2) { /* 2 is param num */
        return -1;
    }

    max_ratio = (hi_u32)osal_strtoul(argv[1], NULL, OSAL_BASE_DEC);

    return hi_drv_ss_set_ddr_max_ratio(max_ratio);
}


static hi_s32 spread_open(hi_void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 spread_close(hi_void *private_data)
{
    return HI_SUCCESS;
}

static osal_fileops g_spread_fops = {
    .open = spread_open,
    .release = spread_close,
    .cmd_list = g_spread_ioctl_info,
    .cmd_cnt = 0,
};


static osal_proc_cmd g_spread_proc_cmd[] = {
    { "help", 	   spread_proc_help },
    { "ddrratio",  spread_proc_set_ddr_ratio }
};


static osal_dev g_spread_dev = {
    .name = UMAP_DEVNAME_SPREAD,
    .minor = UMAP_MIN_MINOR_SPREAD,
    .fops = &g_spread_fops,
    .pmops = NULL,
};


hi_s32 spread_drv_mod_init(hi_void)
{
    osal_proc_entry *proc_item = NULL;

    osal_sem_init(&g_spread_mutex, 1);

    g_spread_fops.cmd_cnt = sizeof(g_spread_ioctl_info) / sizeof(osal_ioctl_cmd);
    hi_info_spread("g_spread_fops cmd_cnt = %d\n", g_spread_fops.cmd_cnt);

    if (osal_dev_register(&g_spread_dev)) {
        hi_err_spread("register system device failed!\n");
        goto OUT;
    }

    proc_item = osal_proc_add(HI_MOD_SPREAD, strlen(HI_MOD_SPREAD));
    if (proc_item == NULL) {
        hi_err_spread("add %s proc failed.\n", HI_MOD_SPREAD);
        osal_dev_unregister(&g_spread_dev);
        return HI_FAILURE;
    }

    proc_item->read = spread_proc_show;
    proc_item->cmd_cnt = sizeof(g_spread_proc_cmd) / sizeof(osal_proc_cmd);
    proc_item->cmd_list = g_spread_proc_cmd;
    proc_item->private = NULL;

    return 0;

OUT:
    hi_warn_spread("load spread ...FAILED!\n");
    return HI_FAILURE;
}

hi_void spread_drv_mod_exit(hi_void)
{
    osal_sem_destory(&g_spread_mutex);
    osal_proc_remove(HI_MOD_SPREAD, strlen(HI_MOD_SPREAD));
    osal_dev_unregister(&g_spread_dev);
}

#ifdef MODULE
module_init(spread_drv_mod_init);
module_exit(spread_drv_mod_exit);
#else
EXPORT_SYMBOL(spread_drv_mod_init);
EXPORT_SYMBOL(spread_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

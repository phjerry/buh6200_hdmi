/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: proc file
 */

#include "drv_dvfs_proc.h"
#include "drv_dvfs_common.h"

#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/regulator/consumer.h>
#include <asm/io.h>

#include "hi_osal.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_proc.h"
#include "drv_dvfs_regulator.h"
#include "drv_dvfs.h"
#ifdef HI_TEMP_CTRL_SUPPORT
#include "drv_dvfs_temperature.h"
#endif

static hi_s32 cpu_proc_helper(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("==================CPU DVFS HELP==============\n");
    osal_printk("USAGE: echo [cmd] [CPU] [value] > /proc/msp/pm_cpu\n");
    osal_printk("example:\n");
    osal_printk("echo freq 0 400000 > /proc/msp/pm_cpu, set cpu0 freq to 400000kHz\n");
    osal_printk("echo freq 4 800000 > /proc/msp/pm_cpu, set cpu4 freq to 800000kHz\n");
    osal_printk("echo volt 0 800 > /proc/msp/pm_cpu, set cpu0 volt to 800mV\n");
    osal_printk("echo volt 4 910 > /proc/msp/pm_cpu, set cpu4 volt to 910mV\n");

    return HI_SUCCESS;
}

static hi_s32 cpu_proc_read(hi_void *seqfile, hi_void *private)
{
    hi_s32 ret;
    hi_u32 freq, volt;
    hi_u32 cpu;
    struct hisi_dvfs_info *info = HI_NULL;

    HI_FUNC_ENTER();

    for_each_online_cpu(cpu) {
        info = dvfs_get_cluster(cpu);
        if (info != HI_NULL) {
            mutex_lock(&info->dvfs_lock);

            freq = clk_get_rate(info->cpu_clk);
            ret = dvfs_round_freq(cpu, &freq);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(dvfs_round_freq, ret);
            }

            volt = regulator_get_voltage(info->cpu_reg);

            osal_proc_print(seqfile,
                "CPU%d: current freq = %d(kHz), current volt = %d(mv) \n",
                cpu, freq, volt / 1000); /* 1000:to mv */

            mutex_unlock(&info->dvfs_lock);
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 cpu_proc_write(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 cpu, value;
    struct hisi_dvfs_info *info = HI_NULL;

    HI_FUNC_ENTER();

    if (argc != 3) { /* 3 param */
        return cpu_proc_helper(0, 0, 0);
    }

    cpu = (hi_u32)osal_strtoul(argv[1], NULL, OSAL_BASE_DEC);

    info = dvfs_get_info(cpu);
    if (IS_ERR(info)) {
        HI_ERR_PRINT_FUNC_RES(dvfs_get_info, -ENODEV);
        return -ENODEV;
    }

    value = (hi_u32)osal_strtoul(argv[2], NULL, OSAL_BASE_DEC);

    if (osal_strncasecmp("freq", strlen("freq"), argv[0], strlen(argv[0])) == 0) {
        ret = clk_set_rate(info->cpu_clk, value);
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(clk_set_rate, ret);
            return ret;
        }
    } else if (osal_strncasecmp("volt", strlen("volt"), argv[0], strlen(argv[0])) == 0) {
        ret = regulator_set_voltage(info->cpu_reg, value * 1000, value * 1000); /* 1000:to uv */
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(regulator_set_voltage, ret);
            return ret;
        }

        osal_msleep_uninterruptible(10);  /* sleep 10ms */
    } else {
        osal_printk("Invaid parameter.\n");
        cpu_proc_helper(0, 0, 0);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 core_proc_helper(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("==================CORE DVFS HELP==============\n");
    osal_printk("USAGE: echo [cmd] [value] > /proc/msp/pm_core\n");
    osal_printk("example:\n");
    osal_printk("echo volt 850 > /proc/msp/pm_core, set core volt to 850mV\n");

    return HI_SUCCESS;
}

static hi_s32 core_proc_read(hi_void *seqfile, hi_void *private)
{
    hi_u32 volt;

    HI_FUNC_ENTER();

    dvfs_get_core_volt(&volt);

    osal_proc_print(seqfile, "CORE: current volt = %d(mv) \n", volt);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 core_proc_write(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 volt;

    HI_FUNC_ENTER();

    if (argc != 2) { /* 2 param */
        return core_proc_helper(0, 0, 0);
    }

    volt = (hi_u32)osal_strtoul(argv[1], NULL, OSAL_BASE_DEC);

    if (osal_strncasecmp("volt", strlen("volt"), argv[0], strlen(argv[0])) == 0) {
        dvfs_set_core_volt(volt, PMC_PWM3_CTRL0);
    } else {
        osal_printk("Invaid parameter.\n");
        core_proc_helper(0, 0, 0);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static osal_proc_cmd g_cpu_proc_cmd[] = {
    {"freq", cpu_proc_write},
    {"volt", cpu_proc_write},
    {"help", cpu_proc_helper},
};

static osal_proc_cmd g_core_proc_cmd[] = {
    {"volt", core_proc_write},
    {"help", core_proc_helper},
};

#ifdef HI_TEMP_CTRL_SUPPORT
static hi_s32 temperature_proc_helper(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("==================temperature ctrl help==============\n");
    osal_printk("USAGE: echo threshold [level] [value] > /proc/msp/pm_temperature\n");

    osal_printk("       level: 1(to set enter_low_temp_comp threshold)\n");
    osal_printk("       level: 2(to set quit_low_temp_comp threshold)\n");
    osal_printk("       level: 3(to set enter_limiting_freq threshold)\n");
    osal_printk("       level: 4(to set quit_limiting_freq threshold)\n");
    osal_printk("       level: 5(to set enter_standby threshold)\n");
    osal_printk("example:\n");
    osal_printk("echo threshold 1 30 > /proc/msp/pm_temperature.\n");

    return HI_SUCCESS;
}

static hi_s32 temperature_proc_read(hi_void *seqfile, hi_void *private)
{
    dvfs_temperature_ctrl_threshold threshold = {0};

    HI_FUNC_ENTER();

    dvfs_get_temp_ctrl_threshold(&threshold);

    osal_printk("temperature ctrl status:\n");
    osal_printk("       0:status normal, 1:enter_low_temp_comp, 2:quit_low_temp_comp\n");
    osal_printk("       3:enter_limiting_freq, 4:quit_limiting_freq, 5:enter_standby\n\n");
    osal_proc_print(seqfile, "temperature ctrl status       \t :%d\n", dvfs_get_temperature_status());
    osal_proc_print(seqfile, "enter_low_temp_comp threshold \t :%d\n", threshold.enter_low_temp_comp);
    osal_proc_print(seqfile, "quit_low_temp_comp threshold  \t :%d\n", threshold.quit_low_temp_comp);
    osal_proc_print(seqfile, "enter_limiting_freq threshold \t :%d\n", threshold.enter_limiting_freq);
    osal_proc_print(seqfile, "quit_limiting_freq threshold  \t :%d\n", threshold.quit_limiting_freq);
    osal_proc_print(seqfile, "enter_standby threshold       \t :%d\n", threshold.enter_standby);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 temperature_proc_write(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 level, threshold;

    HI_FUNC_ENTER();

    if (argc != 3) { /* 3 param */
        return cpu_proc_helper(0, 0, 0);
    }

    level = (hi_u32)osal_strtoul(argv[1], NULL, OSAL_BASE_DEC);
    threshold = (hi_u32)osal_strtoul(argv[2], NULL, OSAL_BASE_DEC);

    if (level <= STATUS_ENTER_STANDBY) {
        dvfs_set_temp_ctrl_threshold(level, threshold);
    } else {
        osal_printk("Invaid parameter.\n");
        temperature_proc_helper(0, 0, 0);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static osal_proc_cmd g_temperature_proc_cmd[] = {
    {"threshold", temperature_proc_write},
    {"help",  temperature_proc_helper},
};
#endif

hi_s32 dvfs_register_proc(hi_void)
{
    hi_s32 ret;
    osal_proc_entry *item = HI_NULL;

    HI_FUNC_ENTER();

    ret = pmc_register_remap();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pmc_register_remap, HI_FAILURE);
        return HI_FAILURE;
    }

    item = osal_proc_add("pm_cpu", strlen("pm_cpu"));
    if (item == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_proc_add, HI_FAILURE);
        goto out_register_unmap;
    }

    item->read = cpu_proc_read;
    item->cmd_list = g_cpu_proc_cmd;
    item->cmd_cnt = sizeof(g_cpu_proc_cmd) / sizeof(osal_proc_cmd);

    item = osal_proc_add("pm_core", strlen("pm_core"));
    if (item == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_proc_add, HI_FAILURE);
        goto out_remove_pm_cpu;
    }

    item->read = core_proc_read;
    item->cmd_list = g_core_proc_cmd;
    item->cmd_cnt = sizeof(g_core_proc_cmd) / sizeof(osal_proc_cmd);

#ifdef HI_TEMP_CTRL_SUPPORT
    item = osal_proc_add("pm_temperature", strlen("pm_temperature"));
    if (item == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_proc_add, HI_FAILURE);
        goto out_remove_pm_core;
    }

    item->read  = temperature_proc_read;
    item->cmd_list = g_temperature_proc_cmd;
    item->cmd_cnt = sizeof(g_temperature_proc_cmd) / sizeof(osal_proc_cmd);
#endif

    HI_FUNC_EXIT();
    return HI_SUCCESS;

#ifdef HI_TEMP_CTRL_SUPPORT
out_remove_pm_core:
    osal_proc_remove("pm_core", strlen("pm_core"));
#endif
out_remove_pm_cpu:
    osal_proc_remove("pm_cpu", strlen("pm_cpu"));
out_register_unmap:
    pmc_register_unmap();

    return HI_FAILURE;
}

hi_void dvfs_remove_proc(hi_void)
{
    HI_FUNC_ENTER();

#ifdef HI_TEMP_CTRL_SUPPORT
    osal_proc_remove("pm_temperature", strlen("pm_temperature"));
#endif
    osal_proc_remove("pm_core", strlen("pm_core"));
    osal_proc_remove("pm_cpu", strlen("pm_cpu"));

    pmc_register_unmap();

    HI_FUNC_EXIT();
    return;
}


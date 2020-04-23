/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: dvfs driver
 */

#include "drv_dvfs.h"
#include "drv_dvfs_common.h"

#include <linux/platform_device.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/pm_opp.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "hi_drv_sys.h"
#include "hi_type.h"

#include "drv_dvfs_update.h"
#include "drv_dvfs_proc.h"
#include "drv_dvfs_regulator.h"
#ifdef HI_TEMP_CTRL_SUPPORT
#include "drv_dvfs_temperature.h"
#endif

static struct platform_device *g_dvfs_platdev;
LIST_HEAD(g_dvfs_info_list);

static hi_s32 init_dvfs_info(hi_u32 cpu, struct hisi_dvfs_info *info)
{
    hi_s32 ret;
    struct device *cpu_dev = HI_NULL;
    struct clk *cpu_clk = HI_NULL;
    struct regulator *cpu_reg = HI_NULL;

    HI_FUNC_ENTER();

    cpumask_clear(&info->cpus);

    cpu_dev = get_cpu_device(cpu);
    if (IS_ERR(cpu_dev)) {
        HI_ERR_PRINT_FUNC_RES(get_cpu_device, -ENODEV);
        HI_ERR_PRINT_U32(cpu);
        return -ENODEV;
    }

    cpu_clk = clk_get(cpu_dev, HI_NULL);
    if (IS_ERR(cpu_clk)) {
        HI_ERR_PRINT_FUNC_RES(clk_get, PTR_ERR(cpu_clk));
        return PTR_ERR(cpu_clk);
    }

    ret = clk_prepare_enable(cpu_clk);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(clk_prepare_enable, ret);
        goto out_put_clk;
    }

    ret = dev_pm_opp_of_get_sharing_cpus(cpu_dev, &info->cpus);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(dev_pm_opp_of_get_sharing_cpus, ret);
        goto out_disable_clk;
    }

    ret = dev_pm_opp_of_cpumask_add_table(&info->cpus);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(dev_pm_opp_of_cpumask_add_table, ret);
        goto out_disable_clk;
    }

    ret = dev_pm_opp_get_opp_count(cpu_dev);
    if (ret <= 0) {
        HI_ERR_PRINT_FUNC_RES(dev_pm_opp_get_opp_count, ret);
        ret = -EPROBE_DEFER;
        goto out_free_opp;
    }

    cpu_reg = regulator_get_optional(cpu_dev, "cpu");
    if (IS_ERR(cpu_reg)) {
        HI_ERR_PRINT_FUNC_RES(regulator_get_optional, ret);
        ret = PTR_ERR(cpu_reg);
        goto out_free_opp;
    }

    info->cpu_dev = cpu_dev;
    info->cpu_clk = cpu_clk;
    info->cpu_reg = cpu_reg;
    mutex_init(&info->dvfs_lock);

    HI_FUNC_EXIT();
    return HI_SUCCESS;

out_free_opp:
    dev_pm_opp_of_cpumask_remove_table(&info->cpus);
out_disable_clk:
    clk_disable_unprepare(cpu_clk);
out_put_clk:
    clk_put(cpu_clk);

    HI_ERR_PRINT_U32(cpu_dev->id);

    return ret;
}

#ifdef HI_CPU_DVFS_SUPPORT
static hi_s32 hisi_cpufreq_target(struct cpufreq_policy *policy, hi_u32 index)
{
    hi_s32 ret;
    hi_u32 new_freq;
    struct cpufreq_frequency_table *freq_table = HI_NULL;
    struct hisi_dvfs_info *info = HI_NULL;

    HI_FUNC_ENTER();

    freq_table = policy->freq_table;

    new_freq = freq_table[index].frequency;

    info = dvfs_get_info(policy->cpu);
    if (info == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(dvfs_get_info, -ENODEV);
        HI_ERR_PRINT_U32(policy->cpu);
        return -ENODEV;
    }

    ret = dvfs_update(info, policy->cur, new_freq);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(dvfs_update, ret);
        HI_ERR_PRINT_U32(policy->cpu);
        return ret;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_u32 hisi_cpufreq_getspeed(hi_u32 cpu)
{
    hi_s32 ret;
    hi_u32 rate;
    struct cpufreq_policy *policy = cpufreq_cpu_get_raw(cpu);

    HI_FUNC_ENTER();

    if (policy == HI_NULL) {
        HI_ERR_PRINT_INFO("faild to get policy for cpu:");
        HI_ERR_PRINT_U32(cpu);
        return 0;
    }

    rate = clk_get_rate(policy->clk);

    ret = dvfs_round_freq(cpu, &rate);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_round_freq, ret);
    }

    HI_FUNC_EXIT();
    return rate;
}

static hi_s32 hisi_cpufreq_init(struct cpufreq_policy *policy)
{
    hi_s32 ret;
    struct hisi_dvfs_info *dvfs_info = HI_NULL;
    struct cpufreq_frequency_table *freq_table = HI_NULL;

    HI_FUNC_ENTER();

    dvfs_info = dvfs_get_info(policy->cpu);
    if (dvfs_info == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(dvfs_get_info, -ENODEV);
        HI_ERR_PRINT_U32(policy->cpu);
        return -ENODEV;
    }

    if (IS_ERR(dvfs_info->cpu_clk)) {
        HI_ERR_PRINT_INFO("faild to get clk for cpu:");
        HI_ERR_PRINT_U32(policy->cpu);
        return PTR_ERR(dvfs_info->cpu_clk);
    }

    ret = dev_pm_opp_init_cpufreq_table(dvfs_info->cpu_dev, &freq_table);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(dev_pm_opp_init_cpufreq_table, ret);
        HI_ERR_PRINT_U32(policy->cpu);
        return ret;
    }

    ret = cpufreq_table_validate_and_show(policy, freq_table);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(cpufreq_table_validate_and_show, ret);
        HI_ERR_PRINT_U32(policy->cpu);
        dev_pm_opp_free_cpufreq_table(dvfs_info->cpu_dev, &freq_table);
        return ret;
    }

    /* CPUs in the same cluster share a clock and power domain. */
    cpumask_copy(policy->cpus, &dvfs_info->cpus);

    policy->clk = dvfs_info->cpu_clk;
    policy->min = policy->cpuinfo.min_freq;
    policy->max = policy->cpuinfo.max_freq;
    policy->cur = policy->max;

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 hisi_cpufreq_exit(struct cpufreq_policy *policy)
{
    struct hisi_dvfs_info *dvfs_info = HI_NULL;

    HI_FUNC_ENTER();

    dvfs_info = dvfs_get_info(policy->cpu);
    if (IS_ERR(dvfs_info)) {
        HI_ERR_PRINT_FUNC_RES(dvfs_get_info, -ENODEV);
        return -ENODEV;
    }

    if(policy->freq_table != HI_NULL) {
        HI_INFO_PRINT_U32(policy->cpu);
        dev_pm_opp_free_cpufreq_table(dvfs_info->cpu_dev, &policy->freq_table);
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static struct cpufreq_driver g_hisi_cpufreq_driver = {
    .flags = CPUFREQ_STICKY | CPUFREQ_NEED_INITIAL_FREQ_CHECK,
    .verify = cpufreq_generic_frequency_table_verify,
    .target_index = hisi_cpufreq_target,
    .get    = hisi_cpufreq_getspeed,
    .init   = hisi_cpufreq_init,
    .exit   = hisi_cpufreq_exit,
    .name   = "hisi-cpufreq",
    .attr   = cpufreq_generic_attr,
};
#endif

static hi_s32 dvfs_probe(struct platform_device *pdev)
{
    hi_s32 ret;
    hi_u32 cpu;
    struct hisi_dvfs_info *info = HI_NULL;
    struct hisi_dvfs_info *tmp = HI_NULL;

    HI_FUNC_ENTER();

    for_each_possible_cpu(cpu) {
        HI_INFO_PRINT_U32(cpu);

        info = dvfs_get_info(cpu);
        if (info) {
            HI_INFO_PRINT_INFO("cpu is in cpumask");
            continue;
        }

        info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
        if (info == NULL) {
            HI_ERR_PRINT_FUNC_RES(devm_kzalloc, -ENOMEM);
            ret =  -ENOMEM;
            goto release_info;
        }

        ret = init_dvfs_info(cpu, info);
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(init_dvfs_info, ret);
            goto release_info;
        }

        list_add(&info->list, &g_dvfs_info_list);
    }

#ifdef HI_CPU_DVFS_SUPPORT
    ret = cpufreq_register_driver(&g_hisi_cpufreq_driver);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(cpufreq_register_driver, ret);
        goto release_info;
    }
#endif

    HI_FUNC_EXIT();
    return HI_SUCCESS;

release_info:
    list_for_each_entry_safe(info, tmp, &g_dvfs_info_list, list) {
        HI_INFO_PRINT_U32(info->cpu_dev->id);

        regulator_put(info->cpu_reg);
        dev_pm_opp_of_cpumask_remove_table(&info->cpus);
        clk_disable_unprepare(info->cpu_clk);
        clk_put(info->cpu_clk);
        mutex_destroy(&info->dvfs_lock);
        list_del(&info->list);

        devm_kfree(&pdev->dev, info);
    }

    return ret;
}

static int dvfs_remove(struct platform_device *pdev)
{
    struct hisi_dvfs_info *info = HI_NULL;
    struct hisi_dvfs_info *tmp = HI_NULL;

    HI_FUNC_ENTER();

#ifdef HI_CPU_DVFS_SUPPORT
    cpufreq_unregister_driver(&g_hisi_cpufreq_driver);
#endif

    list_for_each_entry_safe(info, tmp, &g_dvfs_info_list, list) {
        HI_INFO_PRINT_U32(info->cpu_dev->id);

        regulator_put(info->cpu_reg);
        dev_pm_opp_of_cpumask_remove_table(&info->cpus);
        clk_disable_unprepare(info->cpu_clk);
        clk_put(info->cpu_clk);
        mutex_destroy(&info->dvfs_lock);
        list_del(&info->list);

        devm_kfree(&pdev->dev, info);
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

#ifdef CONFIG_PM
static int dvfs_suspend(struct platform_device *pdev, pm_message_t state)
{
    HI_FUNC_ENTER();

#ifdef HI_TEMP_CTRL_SUPPORT
    dvfs_enable_temperature_ctrl(HI_FALSE);
#endif

    dvfs_save_core_volt();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static int dvfs_resume(struct platform_device *pdev)
{
    HI_FUNC_ENTER();

#ifdef HI_TEMP_CTRL_SUPPORT
    dvfs_enable_temperature_ctrl(HI_TRUE);
#endif

    dvfs_restore_core_volt();

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}
#endif

static struct platform_driver g_dvfs_platdrv = {
    .driver.name = "hisi-cpufreq",
    .probe       = dvfs_probe,
    .remove      = dvfs_remove,
#ifdef CONFIG_PM
    .suspend     = dvfs_suspend,
    .resume      = dvfs_resume,
#endif
};

hi_s32 dvfs_drv_mod_init(hi_void)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    ret = platform_driver_register(&g_dvfs_platdrv);
    if (ret) {
        HI_ERR_PRINT_FUNC_RES(platform_driver_register, ret);
        return ret;
    }

    g_dvfs_platdev = platform_device_register_simple("hisi-cpufreq", -1, NULL, 0);
    if (IS_ERR(g_dvfs_platdev)) {
        HI_ERR_PRINT_FUNC_RES(platform_device_register_simple, PTR_ERR(g_dvfs_platdev));
        goto out_unregister_driver;
    }

    ret = osal_exportfunc_register(HI_ID_DVFS, "HI_DVFS", HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_register, ret);
        goto out_unregister_device;
    }

    ret = dvfs_init_volt_cal_info();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_init_volt_cal_info, ret);
        goto out_unregister_module;
    }

#ifdef HI_TEMP_CTRL_SUPPORT
    ret = dvfs_temperature_ctrl_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_init_volt_cal_info, ret);
        goto out_unregister_module;
    }
#endif

#ifdef HI_PROC_SUPPORT
    ret = dvfs_register_proc();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_register_proc, ret);
        goto out_deinit_temperature_ctrl;
    }
#endif

#ifdef MODULE
    HI_PRINT("Load hi_dvfs.ko success.\t(%s)\n", VERSION_STRING);
#endif

    HI_FUNC_EXIT();
    return HI_SUCCESS;

#ifdef HI_PROC_SUPPORT
out_deinit_temperature_ctrl:
#endif
#ifdef HI_TEMP_CTRL_SUPPORT
    dvfs_temperature_ctrl_deinit();
#endif
out_unregister_module:
    (hi_void)osal_exportfunc_unregister(HI_ID_DVFS);
out_unregister_device:
    platform_device_unregister(g_dvfs_platdev);
out_unregister_driver:
    platform_driver_unregister(&g_dvfs_platdrv);

    return HI_FAILURE;
}

hi_void dvfs_drv_mod_exit(hi_void)
{
    HI_FUNC_ENTER();

#ifdef HI_PROC_SUPPORT
    dvfs_remove_proc();
#endif

#ifdef HI_TEMP_CTRL_SUPPORT
    dvfs_temperature_ctrl_deinit();
#endif

    (hi_void)osal_exportfunc_unregister(HI_ID_DVFS);
    platform_device_unregister(g_dvfs_platdev);
    platform_driver_unregister(&g_dvfs_platdrv);

    HI_FUNC_EXIT();
    return;
}

#ifdef MODULE
module_init(dvfs_drv_mod_init);
module_exit(dvfs_drv_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: temperature control
 */

#include "drv_dvfs_temperature.h"

#include <linux/device.h>
#include <linux/hisilicon/securec.h>
#include <asm/io.h>
#include <linux/reboot.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>

#include "hi_osal.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "drv_dvfs_update.h"

#define PMC_TSENSOR0_CTRL2 0x00A15A08
#define TSENSOR0_OUT_REG 4

typedef struct {
    hi_u32 status;
    hi_s32(*f_driver_status_process)(hi_void);
} dvfs_temperature_ctrl_map;

typedef union {
    hi_u32 val32;
    hi_u16 val16[2];
} u32_data;

static osal_task *g_temp_ctrl_thread = HI_NULL;
static dvfs_temperature_status g_temperature_status = STATUS_NORMAL;
static dvfs_temperature_ctrl_threshold g_threshold;
static hi_bool g_temperature_ctrl_enable;

static hi_s32 dvfs_tsensor_read(hi_s16 *temperature)
{
    hi_u8 i;
    u32_data reg_value;
    hi_u16 tsensor_value = 0;
    hi_u32 *tsensor_vir_addr = HI_NULL;

    HI_FUNC_ENTER();

    tsensor_vir_addr = (hi_u32 *)osal_ioremap_nocache(PMC_TSENSOR0_CTRL2, TSENSOR0_OUT_REG * sizeof(hi_u32));
    if (tsensor_vir_addr == HI_NULL) {
        HI_ERR_PRINT_FUNC_RES(osal_ioremap_nocache, HI_FAILURE);
        return HI_FAILURE;
    }

    for (i = 0; i < TSENSOR0_OUT_REG; i++) {
        reg_value.val32 = readl(tsensor_vir_addr + i);
        tsensor_value += reg_value.val16[0] & 0x3ff;
        tsensor_value += reg_value.val16[1] & 0x3ff;
    }

    tsensor_value /= 8; /* 8: tsensor out number */

    *temperature = ((tsensor_value - 176) * 165 / 736) - 40; /* test formula: (data-176)/736*165-40 */

    osal_iounmap(tsensor_vir_addr);

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_s32 dvfs_low_temp_comp(hi_void)
{
    hi_s32 ret;
    hi_u32 cpu;
    struct hisi_dvfs_info *info = HI_NULL;

    HI_FUNC_ENTER();

    for_each_online_cpu(cpu) {
        info = dvfs_get_cluster(cpu);
        if (info != HI_NULL) {
            ret = dvfs_update(info, 0, 0);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(dvfs_update, ret);
                HI_ERR_PRINT_U32(cpu);
                return ret;
            }
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

#ifdef HI_CPU_DVFS_SUPPORT
static hi_s32 dvfs_set_governor(const char *buf)
{
    hi_s32 ret;
    hi_u32 cpu;
    struct cpufreq_policy *policy = HI_NULL;
    struct hisi_dvfs_info *info = HI_NULL;

    for_each_online_cpu(cpu) {
        policy = cpufreq_cpu_get_raw(cpu);
        if (policy == HI_NULL) {
            HI_ERR_PRINT_FUNC_RES(cpufreq_cpu_get_raw, HI_FAILURE);
            HI_ERR_PRINT_U32(cpu);
            return HI_FAILURE;
        }

        info = dvfs_get_cluster(cpu);
        if (info != HI_NULL) {
            HI_PRINT("set cpufreq governor to %s\n", buf);
            ret = set_scaling_governor(policy, buf);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(set_scaling_governor, ret);
                HI_ERR_PRINT_U32(cpu);
                return ret;
            }
        }
    }

    return HI_SUCCESS;
}
#endif

static hi_s32 dvfs_enter_limiting_freq(hi_void)
{
#ifdef HI_CPU_DVFS_SUPPORT
    hi_s32 ret;

    ret = dvfs_set_governor("powersave");
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_set_governor, ret);
        return ret;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 dvfs_quit_limiting_freq(hi_void)
{
#ifdef HI_CPU_DVFS_SUPPORT
    hi_s32 ret;

    ret = dvfs_set_governor("schedutil");
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_set_governor, ret);
        return ret;
    }
#endif
    return HI_SUCCESS;
}

static dvfs_temperature_ctrl_map g_temperature_ctrl_func[] = {
    { STATUS_ENTER_LOW_TEMP_COMP,   dvfs_low_temp_comp },
    { STATUS_QUIT_LOW_TEMP_COMP,    dvfs_low_temp_comp },
    { STATUS_ENTER_LIMITING_FREQ,   dvfs_enter_limiting_freq },
    { STATUS_QUIT_LIMITING_FREQ,    dvfs_quit_limiting_freq }
};

static hi_s32 temperature_ctrl_process(dvfs_temperature_status status)
{
    hi_s32 ret;
    hi_u8 i;
    hi_u32 func_num;

    HI_FUNC_ENTER();

    func_num = sizeof(g_temperature_ctrl_func) / sizeof(g_temperature_ctrl_func[0]);

    for (i = 0; i < func_num; i++) {
        if (status == g_temperature_ctrl_func[i].status) {
            ret = g_temperature_ctrl_func[i].f_driver_status_process();
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_H32(status);
                HI_ERR_PRINT_S32(ret);
                return ret;
            }

            HI_FUNC_EXIT();
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

hi_s32 temp_ctrl_thread(hi_void *arg)
{
    hi_s32 ret;
    hi_u8 delay_time = DELAY_100MS;
    hi_s16 temperature;
    hi_u32 pre_status;

    HI_FUNC_ENTER();

    g_threshold.enter_low_temp_comp = HI_ENTER_LOW_TEMP_COMP_THRESHOLD;
    g_threshold.quit_low_temp_comp = HI_QUIT_LOW_TEMP_COMP_THRESHOLD;
    g_threshold.enter_limiting_freq = HI_ENTER_LIMITING_FREQ_THRESHOLD;
    g_threshold.quit_limiting_freq = HI_QUIT_LIMITING_FREQ_THRESHOLD;
    g_threshold.enter_standby = HI_ENTER_STANDBY_THRESHOLD;

    g_temperature_ctrl_enable = HI_TRUE;

    while (!osal_kthread_should_stop()) {
        osal_msleep_uninterruptible(delay_time);

        if (g_temperature_ctrl_enable == HI_FALSE) {
            continue;
        }

        pre_status = g_temperature_status;

        ret = dvfs_tsensor_read(&temperature);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(dvfs_tsensor_read, ret);
            return HI_FAILURE;
        }

       // HI_INFO_PRINT_S32(temperature);

        if ((temperature <= g_threshold.enter_low_temp_comp) &&
            (pre_status != STATUS_ENTER_LOW_TEMP_COMP)) {
            g_temperature_status = STATUS_ENTER_LOW_TEMP_COMP;
            delay_time = DELAY_100MS;
        } else if ((temperature >= g_threshold.quit_low_temp_comp) &&
            (pre_status == STATUS_ENTER_LOW_TEMP_COMP)) {
            g_temperature_status = STATUS_QUIT_LOW_TEMP_COMP;
            delay_time = DELAY_100MS;
        } else if ((temperature >= g_threshold.enter_limiting_freq) &&
            (pre_status != STATUS_ENTER_LIMITING_FREQ)) {
            g_temperature_status = STATUS_ENTER_LIMITING_FREQ;
            delay_time = DELAY_20MS;
        } else if ((temperature < g_threshold.quit_limiting_freq) &&
            (pre_status == STATUS_ENTER_LIMITING_FREQ)) {
            g_temperature_status = STATUS_QUIT_LIMITING_FREQ;
            delay_time = DELAY_100MS;
        } else if (temperature >= g_threshold.enter_standby) {
            g_temperature_status = STATUS_ENTER_STANDBY;

            HI_ERR_PRINT_INFO("Tsensor is extremly high, power off system...\n");
            kernel_power_off(); /* enter standby */
        }

        if (pre_status != g_temperature_status) {
            ret = temperature_ctrl_process(g_temperature_status);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(temperature_ctrl_process, ret);
                return HI_FAILURE;
            }
        }
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_u32 dvfs_get_temperature_status(hi_void)
{
    return g_temperature_status;
}

hi_void dvfs_enable_temperature_ctrl(hi_bool enable)
{
    HI_FUNC_ENTER();

    g_temperature_ctrl_enable = enable;

    HI_FUNC_EXIT();
    return;
}

hi_void dvfs_get_temp_ctrl_threshold(dvfs_temperature_ctrl_threshold *threshold)
{
    threshold->enter_low_temp_comp = g_threshold.enter_low_temp_comp;
    threshold->quit_low_temp_comp = g_threshold.quit_low_temp_comp;
    threshold->enter_limiting_freq = g_threshold.enter_limiting_freq;
    threshold->quit_limiting_freq = g_threshold.quit_limiting_freq;
    threshold->enter_standby = g_threshold.enter_standby;

    return;
}

hi_void dvfs_set_temp_ctrl_threshold(dvfs_temperature_status status, hi_u32 threshold)
{
    if (status == STATUS_ENTER_LOW_TEMP_COMP) {
        g_threshold.enter_low_temp_comp = threshold;
    } else if (status == STATUS_QUIT_LOW_TEMP_COMP) {
        g_threshold.quit_low_temp_comp = threshold;
    } else if (status == STATUS_ENTER_LIMITING_FREQ) {
        g_threshold.enter_limiting_freq = threshold;
    } else if (status == STATUS_QUIT_LIMITING_FREQ) {
        g_threshold.quit_limiting_freq = threshold;
    } else if (status == STATUS_ENTER_STANDBY) {
        g_threshold.enter_standby = threshold;
    }

    return;
}

hi_s32 dvfs_temperature_ctrl_init(hi_void)
{
    HI_FUNC_ENTER();

    g_temp_ctrl_thread = osal_kthread_create(temp_ctrl_thread, NULL, "temperature_control", 0);
    if (IS_ERR(g_temp_ctrl_thread)) {
        HI_ERR_PRINT_FUNC_RES(osal_kthread_create, HI_FAILURE);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

hi_void dvfs_temperature_ctrl_deinit(hi_void)
{
    HI_FUNC_ENTER();

    if (g_temp_ctrl_thread) {
        osal_kthread_destroy(g_temp_ctrl_thread, HI_TRUE);
        g_temp_ctrl_thread = HI_NULL;
    }

    HI_FUNC_EXIT();
    return;
}


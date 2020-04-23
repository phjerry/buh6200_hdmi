/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: volt and freq update function
 */
#define LOG_MODULE_ID  HI_ID_DVFS
#define LOG_FUNC_TRACE 1

#include "drv_dvfs_update.h"

#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/hisilicon/securec.h>
#include <linux/regulator/consumer.h>
#include <asm/io.h>

#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "drv_dvfs_common.h"

hi_s32 dvfs_update(struct hisi_dvfs_info *info, hi_u32 cur_freq, hi_u32 new_freq)
{
    hi_s32 ret;
    hi_u32 new_volt;

    HI_FUNC_ENTER();

    dvfs_check_param(info == HI_NULL, HI_FAILURE);

    mutex_lock(&info->dvfs_lock);

    /* temperature trigger volt update */
    if ((cur_freq == 0) && (new_freq == 0)) {
        cur_freq = clk_get_rate(info->cpu_clk);
        new_freq = cur_freq;
    }

    HI_INFO_PRINT_U32(cur_freq);
    HI_INFO_PRINT_U32(new_freq);

    /* get volt for the new freq */
    ret = dvfs_get_volt_for_new_freq(&new_volt, new_freq);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(dvfs_get_volt_for_new_freq, ret);
        goto exit;
    }

    HI_INFO_PRINT_U32(new_volt);

    if (new_freq > cur_freq) {
        ret = regulator_set_voltage(info->cpu_reg, new_volt * 1000, new_volt * 1000); /* 1000:to uv */
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(regulator_set_voltage, ret);
            goto exit;
        }

        msleep(10);  /* sleep 10ms */

        ret = clk_set_rate(info->cpu_clk, new_freq);
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(clk_set_rate, ret);
            goto exit;
        }
    } else if (new_freq < cur_freq) {
        ret = clk_set_rate(info->cpu_clk, new_freq);
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(clk_set_rate, ret);
            goto exit;
        }

        msleep(1);

        ret = regulator_set_voltage(info->cpu_reg, new_volt * 1000, new_volt * 1000); /* 1000:to uv */
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(regulator_set_voltage, ret);
            goto exit;
        }

        msleep(9);  /* sleep 9ms */
    } else {
        ret = regulator_set_voltage(info->cpu_reg, new_volt * 1000, new_volt * 1000); /* 1000:to uv */
        if (ret) {
            HI_ERR_PRINT_FUNC_RES(regulator_set_voltage, ret);
            goto exit;
        }

        msleep(10);  /* sleep 10ms */
    }

exit:
    mutex_unlock(&info->dvfs_lock);

    HI_FUNC_EXIT();
    return ret;
}


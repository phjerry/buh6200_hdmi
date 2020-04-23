/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: dvfs file
 */
#define LOG_MODULE_ID  HI_ID_DVFS
#define LOG_FUNC_TRACE 1

#include "drv_dvfs.h"

#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/hisilicon/securec.h>
#include <asm/io.h>

#include "hi_osal.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_otp.h"

#include "drv_dvfs_common.h"
#include "drv_dvfs_reg.h"
#ifdef HI_TEMP_CTRL_SUPPORT
#include "drv_dvfs_temperature.h"
#endif

typedef union {
    hi_u32 val32;
    hi_u16 val16[2];
} u32_data;

typedef struct {
    hi_u32 freq;
    hi_u32 otp_offset;
    hi_u16 enable_bit;
    hi_u32 type;
    hi_u32 rank_shift;
} dvfs_otp_compensation_info;

typedef struct {
    hi_u32 freq;
    hi_u32 volt;
    hi_u32 volt_otp_added;
    hi_u32 volt_low_temperature_added; /* mv */
    hi_u32 volt_max; /* mv */
} dvfs_volt_cal_message;

extern struct list_head g_dvfs_info_list;

static dvfs_otp_compensation_info g_otp_compensation_info[] = {
    {
        CPU_FREQ_400M,  OTP_400M_COMPENSATION,  FREQ_400M_COMPENSATION_ENABLE_BIT,
        FREQ_400M_COMPENSATION_TYPE,  FREQ_400M_COMPENSATION_RANK
    }, {
        CPU_FREQ_800M,  OTP_800M_COMPENSATION,  FREQ_800M_COMPENSATION_ENABLE_BIT,
        FREQ_800M_COMPENSATION_TYPE,  FREQ_800M_COMPENSATION_RANK
    }, {
        CPU_FREQ_1200M, OTP_1200M_COMPENSATION, FREQ_1200M_COMPENSATION_ENABLE_BIT,
        FREQ_1200M_COMPENSATION_TYPE, FREQ_1200M_COMPENSATION_RANK
    }, {
        CPU_FREQ_1600M, OTP_1600M_COMPENSATION, FREQ_1600M_COMPENSATION_ENABLE_BIT,
        FREQ_1600M_COMPENSATION_TYPE, FREQ_1600M_COMPENSATION_RANK
    },
};

static dvfs_volt_cal_message g_volt_cal_info[] = {
    { CPU_FREQ_400M,  0, 0, 40, 1100 },
    { CPU_FREQ_800M,  0, 0, 40, 1100 },
    { CPU_FREQ_1200M, 0, 0, 20, 1100 },
    { CPU_FREQ_1600M, 0, 0, 20, 1100 },
};

struct hisi_dvfs_info *dvfs_get_info(hi_u32 cpu)
{
    struct hisi_dvfs_info *info = HI_NULL;

    list_for_each_entry(info, &g_dvfs_info_list, list) {
        if(cpumask_test_cpu(cpu, &info->cpus)) {
            HI_INFO_PRINT_U32(cpu);
            return info;
        }
    }

    return HI_NULL;
}

struct hisi_dvfs_info *dvfs_get_cluster(hi_u32 cpu)
{
    struct hisi_dvfs_info *info = HI_NULL;
    struct device *cpu_dev = HI_NULL;

    cpu_dev = get_cpu_device(cpu);
    if (IS_ERR(cpu_dev)) {
        HI_ERR_PRINT_FUNC_RES(get_cpu_device, -ENODEV);
        HI_ERR_PRINT_U32(cpu);
        return HI_NULL;
    }

    list_for_each_entry(info, &g_dvfs_info_list, list) {
        if (cpu_dev == info->cpu_dev) {
            return info;
        }
    }

    return HI_NULL;
}

#ifdef HI_CPU_DVFS_SUPPORT
hi_u32 dvfs_round_freq(hi_u32 cpu, hi_u32 *rate)
{
    hi_u32 freq = *rate;
    struct cpufreq_policy *policy = cpufreq_cpu_get_raw(cpu);
    struct cpufreq_frequency_table *freq_table = HI_NULL;

    HI_FUNC_ENTER();

    if (policy == HI_NULL) {
        HI_ERR_PRINT_INFO("faild to get policy for cpu:");
        HI_ERR_PRINT_U32(cpu);
        return HI_FAILURE;
    }

    freq_table = policy->freq_table;

    while (freq_table->frequency != CPUFREQ_TABLE_END) {
        if (freq <= freq_table->frequency) {
            if ((freq_table->frequency - freq) < MAX_DIFF_VALUE) {
                *rate = freq_table->frequency;
                break;
            }
        }

        freq_table++;
    }

    if (freq_table->frequency == CPUFREQ_TABLE_END) {
        HI_ERR_PRINT_INFO("freq is not in freq_table\n");
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}
#else
hi_u32 dvfs_round_freq(hi_u32 cpu, hi_u32 *rate)
{
    hi_u8 i;
    hi_u32 freq = *rate;

    HI_FUNC_ENTER();

    for (i = 0; i < sizeof(g_volt_cal_info) / sizeof(g_volt_cal_info[0]); i++) {
        if (freq <= g_volt_cal_info[i].freq) {
            if ((g_volt_cal_info[i].freq - freq) < MAX_DIFF_VALUE) {
                *rate = g_volt_cal_info[i].freq;
                break;
            }
        }
    }

    if (i == sizeof(g_volt_cal_info) / sizeof(g_volt_cal_info[0])) {
        HI_ERR_PRINT_INFO("freq is not in freq_table\n");
        HI_ERR_PRINT_U32(freq);
        return HI_FAILURE;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}
#endif

hi_s32 dvfs_init_volt_cal_info(hi_void)
{
    hi_s32 ret;
    hi_u16 k, b, hpm;
    hi_u8 i, otp_value;
    hi_u8 volt_otp_compensation_enable;
    u32_data tmp;
    volatile hi_reg_sys_ctrl *reg_sys = hi_drv_sys_get_ctrl_reg_ptr();
    otp_export_func *otp_export_funcs = HI_NULL;

    /* get hpm for SC_GENx[61]: bit[31:16] */
    tmp.val32 = reg_sys->SC_GENx[SYS_REG_HPM];
    hpm = tmp.val16[1];
    HI_INFO_PRINT_U32(hpm);

    for (i = 0; i < sizeof(g_volt_cal_info) / sizeof(g_volt_cal_info[0]); i++) {
        tmp.val32 = reg_sys->SC_GENx[SYS_REG_HPM_LINE - i];
        b = tmp.val16[0];
        k = tmp.val16[1];
        HI_INFO_PRINT_U32(b);
        HI_INFO_PRINT_U32(k);

        g_volt_cal_info[i].volt = b - (k * hpm) / 1000; /* 1000: for conversion */

        /* get max volt for cpu */
        tmp.val32 = reg_sys->SC_GENx[SYS_REG_VOLT_MAX];
        g_volt_cal_info[i].volt_max = tmp.val16[1];
        HI_INFO_PRINT_U32(g_volt_cal_info[i].volt_max);
    }

    ret = hi_drv_module_get_func(HI_ID_OTP, (hi_void **)&otp_export_funcs);
    if ((ret != HI_SUCCESS) || (otp_export_funcs == HI_NULL)) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_module_get_function, ret);
        return HI_FAILURE;
    }

    /* read enable flag to see whether to do adjust */
    ret = otp_export_funcs->read_byte(OTP_COMPENSATION_ENABLE, &volt_otp_compensation_enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_otp_read_byte, ret);
        return HI_FAILURE;
    }

    if (volt_otp_compensation_enable & CPU_COMPENSATION_ENABLE_BIT) {

        for (i = 0; i < sizeof(g_otp_compensation_info) / sizeof(g_otp_compensation_info[0]); i++) {
            /* read enable flag from one freq */
            ret = otp_export_funcs->read_byte(g_otp_compensation_info[i].otp_offset, &otp_value);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hi_drv_otp_read_byte, ret);
                return HI_FAILURE;
            }

            if (otp_value & g_otp_compensation_info[i].enable_bit) {
                g_volt_cal_info[i].volt_otp_added = ((otp_value >> g_otp_compensation_info[i].rank_shift) &
                    VOLT_COMPENSATION_VALUE_BITS) * VOLT_COMPENSATION_PER_RANK;
                HI_ERR_PRINT_U32(g_volt_cal_info[i].volt_otp_added);
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 dvfs_get_volt_for_new_freq(hi_u32 *volt, hi_u32 new_freq)
{
    hi_u8 i, target;
    hi_u32 new_volt;

    dvfs_check_param(volt == HI_NULL, HI_FAILURE);

    for (i = 0; i < sizeof(g_volt_cal_info) / sizeof(g_volt_cal_info[0]); i++) {
        if (new_freq <= g_volt_cal_info[i].freq) {
            target = i;
          //  HI_INFO_PRINT_U32(g_volt_cal_info[target].freq);
            break;
        }
    }

    if (i == sizeof(g_volt_cal_info) / sizeof(g_volt_cal_info[0])) {
        HI_ERR_PRINT_INFO("new freq is invalid\n");
        HI_ERR_PRINT_U32(new_freq);
        return HI_FAILURE;
    }

    /* get volt for new freq by hpm line */
    new_volt = g_volt_cal_info[target].volt;

    /* otp compensation */
    new_volt += g_volt_cal_info[target].volt_otp_added;

    /* low temperature compensation */
#ifdef HI_TEMP_CTRL_SUPPORT
    if (dvfs_get_temperature_status() == STATUS_ENTER_LOW_TEMP_COMP) {
        new_volt += g_volt_cal_info[target].volt_low_temperature_added;
    }
#endif

    new_volt = (new_volt > g_volt_cal_info[target].volt_max) ? g_volt_cal_info[target].volt_max : new_volt;

    *volt = new_volt;

    return HI_SUCCESS;
}


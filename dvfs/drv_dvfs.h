/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of drv_dvfs
 */

#ifndef __DRV_DVFS_H__
#define __DRV_DVFS_H__

#include <linux/cpufreq.h>
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CPU_FREQ_400M  400000
#define CPU_FREQ_800M  800000
#define CPU_FREQ_1200M 1200000
#define CPU_FREQ_1600M 1600000

#define CLOCK_24M (24 * 1000) /* 24 KHz */
#define MAX_DIFF_VALUE CLOCK_24M

struct hisi_dvfs_info {
    struct cpumask cpus;
    struct device *cpu_dev;
    struct clk *cpu_clk;
    struct regulator *cpu_reg;
    struct mutex dvfs_lock;
    struct list_head list;
};

struct hisi_dvfs_info *dvfs_get_info(hi_u32 cpu);
struct hisi_dvfs_info *dvfs_get_cluster(hi_u32 cpu);
hi_s32 dvfs_init_volt_cal_info(hi_void);
hi_u32 dvfs_round_freq(hi_u32 cpu, hi_u32 *rate);
hi_s32 dvfs_get_volt_for_new_freq(hi_u32 *volt, hi_u32 new_freq);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_DVFS_H__ */

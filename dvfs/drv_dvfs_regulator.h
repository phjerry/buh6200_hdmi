/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of drv_dvfs_regulator
 */

#ifndef __DRV_DVFS_REGULATOR_H__
#define __DRV_DVFS_REGULATOR_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PMC_BASE_ADDR  0x00A15000
#define PMC_PWM1_CTRL0 0xE50
#define PMC_PWM2_CTRL0 0xE60
#define PMC_PWM3_CTRL0 0xE70

#define CORE_VOLT_MAX 1000
#define CORE_VOLT_MIN 600
#define PWM_STEP_NUM 110
#define PWM_STEP     ((CORE_VOLT_MAX - CORE_VOLT_MIN) / PWM_STEP_NUM)
#define PWM_CLASS    2

hi_void dvfs_set_core_volt(hi_u32 new_volt, hi_u32 pwm_addr);
hi_void dvfs_get_core_volt(hi_u32 *volt);
hi_void dvfs_save_core_volt(hi_void);
hi_void dvfs_restore_core_volt(hi_void);
hi_s32 pmc_register_remap(hi_void);
hi_void pmc_register_unmap(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_DVFS_REGULATOR_H__ */

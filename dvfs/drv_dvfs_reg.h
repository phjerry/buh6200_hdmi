/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: reg definition for dvfs
 */

#ifndef __DRV_DVFS_REG_H__
#define __DRV_DVFS_REG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SYS_REG_HPM_LINE (49 - 24) /* SC_GENx[46]~SC_GENx[49]: 1.6G,1.2G,800M,400M */
#define SYS_REG_VOLT_MAX (44 - 24)/* SC_GENx[44]: bit[31:16] */
#define SYS_REG_HPM (61 - 24) /* SC_GENx[61]: bit[31:16]: cpu0 hpm value; bit[15:0]: core0 hpm value */

#define OTP_COMPENSATION_ENABLE 0x340

#define OTP_400M_COMPENSATION  0x341 /* bit[7:4]*/
#define FREQ_400M_COMPENSATION_ENABLE_BIT (1 << 7)
#define FREQ_400M_COMPENSATION_TYPE (1 << 6) /* 0: adjust PWM; 1: adjust HPM */
#define FREQ_400M_COMPENSATION_RANK 4

#define OTP_800M_COMPENSATION  0x342 /* bit[3:0]*/
#define FREQ_800M_COMPENSATION_ENABLE_BIT (1 << 3)
#define FREQ_800M_COMPENSATION_TYPE (1 << 2)
#define FREQ_800M_COMPENSATION_RANK 0

#define OTP_1200M_COMPENSATION 0x342 /* bit[3:0]*/
#define FREQ_1200M_COMPENSATION_ENABLE_BIT (1 << 3)
#define FREQ_1200M_COMPENSATION_TYPE (1 << 2)
#define FREQ_1200M_COMPENSATION_RANK 0

#define OTP_1600M_COMPENSATION 0x343 /* bit[7:4]*/
#define FREQ_1600M_COMPENSATION_ENABLE_BIT (1 << 7)
#define FREQ_1600M_COMPENSATION_TYPE (1 << 6)
#define FREQ_1600M_COMPENSATION_RANK 4

#define CPU_COMPENSATION_ENABLE_BIT (1 << 1)

#define VOLT_COMPENSATION_PER_RANK 15 /* mv */
#define VOLT_COMPENSATION_VALUE_BITS 2

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_DVFS_REG_H__ */

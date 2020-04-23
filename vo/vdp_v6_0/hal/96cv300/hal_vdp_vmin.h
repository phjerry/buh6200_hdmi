/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef __HAL_VDP_VMIN_H__
#define __HAL_VDP_VMIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HPM7_CTRL0 0x5e0
#define HPM7_CTRL1 0x5e4
#define HPM7_CTRL2 0x5e8
#define HPM7_CTRL3 0x5ec
#define HPM7_CTRL4 0x5f0

#define HPM7_CTRL3_VAL 0x03000000
#define HPM7_CTRL0_VAL0 0x0
#define HPM7_CTRL0_VAL1 0x04000000
#define HPM7_CTRL4_VAL  0x8

#define HPM_BIT_OFS 12
#define HPM_VALUE 0x3ff

#define PMC_ADDR 0x00a15000
#define PMC_LEN 0x1000

#define PWM7_MUX_CTRL 0xe1c
#define PWM7_CTRL0 0xeb0
#define PWM7_CTRL0_VAL 0x004300dd

#define OPT_ADDR 0x00b00300
#define OPT_LEN 0x100

#define OPT_VDP_HMP 0x18
#define OPT_VDP_OFS 18

/*vdp [5]*/
#define OPT_COMP_EN 0x40
#define OPT_COMP_EN_OFS 5

/*vdp [7:4]*/
#define OPT_COMP_VAL 0x47
#define OPT_COMP_VAL_OFS 4

#define GEN_ADDR 0x00841878
#define GEN_LEN 0x4
#define CONST_VAL 0xffff
#define RATIO_BIT 16
#define RATIO_ABS 1500
#define CONST_ABS 1410

#define VOL_MAX 1000
#define VOL_MIN 400
#define VOL_STEP 110

#define PWM_BIT 16
#define PWM_OFS 0xdd

hi_void hal_set_vdp_vmin(hi_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_VO_HAL_H__ */

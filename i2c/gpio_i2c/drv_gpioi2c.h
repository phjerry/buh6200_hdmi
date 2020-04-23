#ifndef __DRV_GPIOI2C_H_
#define __DRV_GPIOI2C_H_

#include "hi_type.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 drv_gpio_i2c_config(hi_u32 i2c_num, hi_u32 scl_gpio_num, hi_u32 sda_gpio_num, hi_u32 clock_bit, hi_u32 data_bit);
hi_s32 drv_gpio_i2c_is_used(hi_u32 i2c_num, hi_bool *is_used);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif



#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "hi_type.h"
#include "drv_i2c_ioctl.h"
#include "hi_errno.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define I2C_DFT_RATE      (400000)

extern hi_s32  i2c_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg);
extern hi_s32  i2c_compat_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg);

extern hi_u32 g_hi_i2c_valid_num;

#ifdef HI_GPIOI2C_SUPPORT
extern i2c_gpio g_gpio_i2c[HI_I2C_MAX_NUM];
#endif


hi_s32 i2c_drv_set_rate(hi_u32 i2c_num, hi_u32 rate_value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



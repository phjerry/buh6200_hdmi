/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao alsa driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __ALSA_I2C_H__
#define __ALSA_I2C_H__

#include "hi_drv_audio.h"

extern int init_i2c(void);
extern int remove_i2c(void);
extern void godbox_aic31_reset(void);
extern void i2c_pins_init(void);
extern void godbox_aic31_mute(int mute);

#endif

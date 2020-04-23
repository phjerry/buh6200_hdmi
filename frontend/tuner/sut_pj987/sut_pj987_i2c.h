/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: SUT-PJ987 i2c driver
 * Author: SDK
 * Create: 2019-9-18
 */

#ifndef _SUT_PJ987_I2C_H_
#define _SUT_PJ987_I2C_H_

#include "hi_type.h"

hi_u8 sut_pj987_reg_write(hi_u16 id, hi_u8 subadr, hi_u8 *data, hi_u8 len);
hi_u8 sut_pj987_reg_read(hi_u16 id, hi_u8 subadr, hi_u8 *data, hi_u8 len);

#endif


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv jpeg clock
 */

#ifndef __DRV_JPEG_OSR_H__
#define __DRV_JPEG_OSR_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void drv_jpeg_on_clock(hi_void);
hi_void drv_jpeg_off_clock(hi_void);
hi_void drv_jpeg_reset_reg(hi_void);
hi_void drv_jpeg_cancel_reset(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

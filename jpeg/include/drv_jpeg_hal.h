/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:jpeg reg parse define
 */

#ifndef __DRV_JPEG_INCLUDE_HAL__
#define __DRV_JPEG_INCLUDE_HAL__

#include "drv_jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_void jpeg_set_reg_base(volatile hi_u32 *reg_base);
volatile hi_u32 *jpeg_get_reg_base(hi_void);
hi_void drv_jpeg_start_decompress(hi_jpeg_reg_info *reg_info);
hi_void drv_jpeg_dev_get_status(hi_u32 *status);
hi_void drv_jpeg_dev_clear_status(hi_u32 status);
hi_void drv_jpeg_set_int_mask(hi_u32 int_mask);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

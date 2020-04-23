/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:jpeg csc convert define
 */

#ifndef __DRV_JPEG_CSC_H__
#define __DRV_JPEG_CSC_H__

#include "drv_jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_jpeg_init_tde_dev(hi_void);
hi_void drv_jpeg_deinit_tde_dev(hi_void);
hi_s32 drv_jpeg_hdec_csc_convert(jpeg_hdec_csc_surface *csc_surface);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

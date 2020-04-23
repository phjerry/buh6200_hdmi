/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg memory manage define
 */

#ifndef __DRV_JPEG_MEM_H__
#define __DRV_JPEG_MEM_H__

#include "drv_jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 drv_jpeg_hdec_alloc_mem(hi_void *mem_info);
hi_s32 drv_jpeg_hdec_free_mem(hi_void *mem_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

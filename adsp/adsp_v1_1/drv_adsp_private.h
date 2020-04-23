/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv adsp private header file
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#ifndef __DRV_ADSP_PRIVATE_H__
#define __DRV_ADSP_PRIVATE_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 adsp_drv_init(hi_void);
hi_void adsp_drv_exit(hi_void);

#ifdef HI_PROC_SUPPORT
hi_void adsp_drv_register_proc(hi_void);
hi_void adsp_drv_unregister_proc(hi_void);

hi_void adsp_proc_deinit(hi_void);
hi_s32 adsp_proc_init(hi_u8 *aoe_reg_vir_addr);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_ADSP_PRIVATE_H__ */

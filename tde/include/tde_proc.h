/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: proc manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_PROC__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_PROC__

#include "tde_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT

#define TDE_MAX_PROC_NUM 8

hi_void tde_proc_record_node(tde_hardware_node *hardware_node);
hi_void tde_osi_list_proc(hi_void *p);
hi_s32 tde_read_proc(hi_void *p, hi_void *v);
hi_s32 tde_proc_init(hi_void);
hi_void tde_proc_dinit(hi_void);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_PROC__ */

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: handle manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_HANDLE__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_HANDLE__

/* ********************************add include here********************************************* */
#include "hi_osal.h"
#include "osal_list.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_list.h"
#endif
#include "tde_define.h"
#include "tde_osilist.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TDE_MAX_HANDLE_VALUE 0x7fffffff

typedef struct {
#ifndef HI_BUILD_IN_BOOT
    struct osal_list_head list_head;
    osal_spinlock lock;
#else
    struct list_head list_head;
#endif
    hi_s32 handle;
    hi_void *res;
} tde_handle_mgr;

hi_bool initial_handle(hi_void);
hi_void get_handle(tde_handle_mgr *res, hi_s32 *handle);
hi_bool query_handle(hi_s32 handle, tde_handle_mgr **res);
hi_bool release_handle(hi_s32 handle);
hi_void destroy_handle(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_HANDLE__ */

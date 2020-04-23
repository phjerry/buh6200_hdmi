/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: p00370051
* Create: 2019-6-1
* Notes:
* History: 2019-06-2 p00370051 CSEC
*/

#ifndef __DRV_SYS_STRATEGY_H__
#define __DRV_SYS_STRATEGY_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    hi_mod_id mod_id;
    hi_handle h_handle;
} hdr_strategy_info_item;

typedef struct {
    hi_u32 match_id;
    hi_u32 empty_id;
    hi_u32 src_exist_id;
    hi_u32 wind_exist_id;
} comm_window_attach_id;

typedef struct {
    hi_void *policy_owner;
    hi_sys_policy policy_info;
} comm_sys_policy_t;

hi_s32 hi_drv_sys_policy_init(hi_void);
hi_void hi_drv_sys_policy_deinit(hi_void);
hi_s32 hi_drv_sys_set_policy_file(hi_sys_policy_type policy_type, hi_void *policy_owner);
hi_s32 hi_drv_sys_clean_policy_by_file(hi_void *policy_owner);


#ifdef __cplusplus
}
#endif

#endif



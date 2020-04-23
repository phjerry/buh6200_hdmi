/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: hi_drv_rm.h
 * Author: sm_rm
 * Create: 2015/12/01
 */

#ifndef __HI_DRV_RM_H__
#define __HI_DRV_RM_H__

#include "hi_drv_win.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

typedef enum {
    HI_DRV_RM_EVENT_WIN_LACK,
    HI_DRV_RM_EVENT_WIN_CREATED,
    HI_DRV_RM_EVENT_WIN_DESTROYED,
    HI_DRV_RM_EVENT_MAX,
} hi_drv_rm_win_event;

typedef struct {
    hi_drv_rm_win_event win_event;
} hi_drv_rm_event;

typedef enum {
    HI_DRV_WIN_OWNER_DTV = 0,
    HI_DRV_WIN_OWNER_ATV,
    HI_DRV_WIN_OWNER_MEDIA,
    HI_DRV_WIN_OWNER_BUTT
} hi_drv_win_owner;

typedef struct {
    hi_drv_win_owner win_handle_owner;
} hi_drv_win_owner_s;

hi_s32  hi_drv_rm_init(hi_void);
hi_void hi_drv_rm_de_init(hi_void);
hi_s32  hi_drv_rm_create(hi_handle *rm_handle, hi_u32 pid);
hi_s32  hi_drv_rm_destroy(hi_handle rm_handle);
hi_s32  hi_drv_rm_enable(hi_handle rm_handle);
hi_s32  hi_drv_rm_disable(hi_handle rm_handle);
hi_s32  hi_drv_rm_query_event(hi_handle rm_handle, hi_drv_rm_event *rm_event);
hi_s32  hi_drv_rm_acquire_window_handle(hi_u32 pid);
hi_s32  hi_drv_rm_set_window_owner(hi_drv_win_owner wind_owner);
hi_s32  hi_drv_rm_notify_wind_created(hi_handle win_handle);
hi_s32  hi_drv_rm_notify_wind_destroyed(hi_handle win_handle);

typedef hi_s32  (*fn_rm_notify_wind_created)(hi_handle win_handle);
typedef hi_s32  (*fn_rm_notify_wind_destroyed)(hi_handle win_handle);
typedef hi_s32  (*fn_rm_set_window_owner)(hi_drv_win_owner wind_owner);

typedef struct {
    fn_rm_notify_wind_created   pfn_rm_notify_wind_created;
    fn_rm_notify_wind_destroyed pfn_rm_notify_wind_destroyed;
    fn_rm_set_window_owner      pfn_rm_set_window_owner;
} rm_export_func;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HI_DRV_RM_H__ */


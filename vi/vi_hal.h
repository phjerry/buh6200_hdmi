/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi hal
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_HAL_H__
#define __VI_HAL_H__

#include "hi_type.h"
#include "vi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 vi_hal_creat_host(hi_vi_attr *attr, hi_void **host_handle);
hi_s32 vi_hal_get_vicap_id(hi_void *host_handle);
hi_s32 vi_hal_get_cur_filed_type(hi_void *host_handle);
hi_s32 vi_hal_set_callback(hi_void *host_handle, vi_fun fun, vi_instance *vi_instance_p);

hi_s32 vi_hal_prepare(hi_void *host_handle);
hi_s32 vi_hal_unprepare(hi_void *host_handle);
hi_s32 vi_hal_start(hi_void *host_handle);
hi_s32 vi_hal_stop(hi_void *host_handle);
hi_s32 vi_hal_cfg_node(hi_void *host_handle, vi_buf_node *node_set_p);

hi_s32 vi_hal_suspend(hi_void);
hi_s32 vi_hal_resume(hi_void);
hi_s32 vi_hal_init(hi_void);
hi_s32 vi_hal_deinit(hi_void);

hi_s32 vi_hal_set_gen_date(hi_void *host_handle, hi_u32 pattern_type);
hi_s32 vi_hal_set_gen_timing_date(hi_void *host_handle, hi_u32 pattern_type);

hi_void vi_hal_proc_print(hi_void *host_handle, hi_void *s);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

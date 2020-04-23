/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync ext interface
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef _SYNC_EXT_H_
#define _SYNC_EXT_H_

#include "hi_drv_sync.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void (*sync_vid_proc_fn)(hi_handle, sync_vid_info *, sync_vid_opt *);
typedef void (*sync_pcr_proc_fn)(hi_handle, hi_u64, hi_u64);
typedef hi_s32 (*sync_hpll_conf_flag_set_fn)(hi_bool);

typedef struct {
    sync_vid_proc_fn           vid_proc;
    sync_pcr_proc_fn           pcr_proc;
    sync_hpll_conf_flag_set_fn hpll_conf_flag_set;
} sync_export_func;

hi_s32 sync_drv_mod_init(void);
void sync_drv_mod_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


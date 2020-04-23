/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync interface
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#ifndef __SYNC_INTF_H__
#define __SYNC_INTF_H__

#include "drv_sync.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef HI_SYNC_PROC_SUPPORT
typedef struct {
    int (*rd_proc)(void *seqfile, void *private);
    osal_proc_cmd *cmd_list;
    unsigned int  cmd_cnt;
} sync_register_param;

hi_s32 sync_intf_register(sync_register_param *param);
void sync_intf_unregister(void);

#ifdef SYNC_DEBUG_DETAIL
void sync_proc_debug_info(sync_context *ctx, sync_dbg_info_mode info_type, void *param);
#endif
#endif

sync_context *sync_info_ctx_get(hi_u32 id);
hi_slong sync_ioctl(void *file, unsigned int cmd, void *arg);

hi_s32 sync_drv_init(void);
void sync_drv_exit(void);

hi_s32 sync_drv_open(void *priv_data);
hi_s32 sync_drv_release(void *priv_data);
hi_s32 sync_dev_suspend(void);
hi_s32 sync_dev_resume(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


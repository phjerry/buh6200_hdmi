/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __TEE_CLT_PROC__
#define __TEE_CLT_PROC__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_s32 clt_proc_init(hi_s8 *proc);
hi_void clt_proc_exit(hi_void);
hi_s32 clt_proc_read(log_module module, hi_void *p, hi_s32 chan_id);
hi_s32 clt_proc_write(log_module module, cmd_str_ptr cmd, hi_s32 count);
hi_s32 clt_proc_read_share_mem(hi_void *p, hi_s32 chan_id);

#endif
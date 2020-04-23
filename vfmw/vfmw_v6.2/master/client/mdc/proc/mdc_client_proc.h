/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __MDC_CLIENT_PROC__
#define __MDC_CLIENT_PROC__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_s32 mdc_clt_proc_open(hi_void);
hi_void mdc_clt_proc_close(hi_void);
hi_s32 mdc_clt_proc_init(hi_s8 *proc, hi_void *comm_shr_mem, hi_u32 mem_length);
hi_void mdc_clt_proc_exit(hi_void);
hi_s32 mdc_clt_proc_read(hi_s32 module, hi_void *p, hi_s32 chan_id);
hi_s32 mdc_clt_proc_write(hi_s32 module, cmd_str_ptr cmd, hi_s32 count);
hi_s32 mdc_log_get_log(hi_void *p);
hi_s32 mdc_clt_proc_read_mdc(hi_void *p, hi_s32 chan_id);
hi_s32 mdc_clt_proc_write_mdc(cmd_str_ptr cmd_str, hi_u32 cmd_cnt);
#endif
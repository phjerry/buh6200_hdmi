/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __MDC_ADPT_CLIENT__
#define __MDC_ADPT_CLIENT__

#include "vfmw.h"
#include "mdc_private.h"

typedef struct {
    hi_bool boot_up_ok;
    hi_u32  reg_base_addr;
    hi_s32 (*load) (hi_void);
    hi_s32 (*unload) (hi_void);
    hi_s32 (*low_power_check) (hi_void);
    hi_void (*print_state)(hi_void);
    hi_void (*dump_reg)(hi_void *, hi_u32, hi_s32 *);
    hi_void (*get_base_time)(hi_u64 *, hi_u64 *);
} mdc_adpt_ops;

typedef struct {
    hi_u32 reserve;
    mdc_adpt_ops *ops;
    hi_void *comm_share_mem;
    hi_u32 comm_share_mem_size;
    hi_u32 mdc_num;
} mdc_adpt_ctx;

mdc_adpt_ctx *mdc_adpt_get_ctx(hi_void);
hi_s32 mdc_adpt_open(mdc_comm_intf func);
hi_s32 mdc_adpt_close(hi_void);
hi_s32 mdc_adpt_init(hi_void *comm_shr_mem, hi_u32 mem_length);
hi_s32 mdc_adpt_exit(hi_void);
hi_s32 mdc_adpt_send_block(comm_cmd_param *param);
hi_s32 mdc_adpt_suspend(hi_void *args, hi_u32 length);
hi_s32 mdc_adpt_resume(hi_void *args, hi_u32 length);
hi_s32 mdc_adpt_check_state(hi_void);
hi_s32 mdc_adpt_print_state(hi_void);
hi_s32 mdc_adpt_set_boot_up(hi_u32 mdc_index);

#endif

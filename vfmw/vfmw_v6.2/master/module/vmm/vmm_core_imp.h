/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_CORE_H__
#define __VMM_CORE_H__

#include "vmm_ext.h"

typedef hi_s32 (*fn_vmm_cmd_handler)(hi_void *param_in);

typedef struct {
    hi_u32 cmd_id;
    fn_vmm_cmd_handler handler;
} vmm_cmd_node;

hi_s32 vmm_core_alloc(hi_void *param_in);
hi_s32 vmm_core_release(hi_void *param_in);
hi_s32 vmm_core_map(hi_void *param_in);
hi_s32 vmm_core_unmap(hi_void *param_in);
hi_s32 vmm_core_config_mem_block(hi_void *param_in);
hi_s32 vmm_core_config_mem_node(hi_void *param_in);
hi_s32 vmm_core_set_location(hi_void *param_in);
hi_s32 vmm_core_release_burst(hi_void *param_in);
hi_s32 vmm_core_cancle_cmd(hi_void *param_in);
hi_s32 vmm_core_set_priority_policy(hi_void *param_in);
hi_s32 vmm_core_get_mem_node_info(hi_void *param_in);
hi_s32 vmm_core_add_msg(const vmm_cmd_type cmd_id, vmm_cmd_prio proir, hi_void *args);
hi_s32 vmm_core_send_cmd_block(vmm_cmd_blk_type cmd_id, hi_void *param_in_out);
hi_s32 vmm_core_init(fn_vmm_callback callback);
hi_s32 vmm_core_deinit(hi_void);
hi_void *vmm_core_get_callback_func(hi_void);

#endif

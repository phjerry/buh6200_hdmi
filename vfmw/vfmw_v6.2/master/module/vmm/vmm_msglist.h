/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_MSGLIST_MANAGER_H__
#define __VMM_MSGLIST_MANAGER_H__

#include "vmm_ext.h"
#include "vmm_dbg.h"

typedef union {
    vmm_buffer buf_rec;
    vmm_rls_burst rls_burst;
} input_param;

typedef struct {
    vmm_cmd_type cmd_id;
    input_param param;
    hi_s64 priv_id; /* for ReleaseBurst fast */
    hi_u32 tick;
} vmm_msg_data;

typedef struct {
    struct OS_LIST_HEAD list;
    /* payload */
    vmm_msg_data data;
} vmm_msg_node;

typedef struct {
    hi_u32 cmd_valid;
    vmm_cmd_type cmd_id;
    hi_u32 priv_id_valid;
    hi_s64 priv_id;
    hi_u64 priv_mask;
} vmm_msg_delete_info;

hi_s32 vmm_msg_list_init(hi_void);
hi_void vmm_msg_list_deinit(hi_void);
hi_s32 vmm_msg_list_add_node(vmm_msg_data *data, vmm_cmd_prio proir);
/* the node which be taken out should be deleted at the same time */
hi_s32 vmm_msg_list_take_out_node(vmm_msg_data *data);
hi_s32 vmm_msg_list_delete_specified_node(vmm_msg_delete_info *msg_delete);
hi_s32 vmm_msg_list_is_empty(hi_void);
hi_s32 vmm_msg_list_read_proc(hi_void *p);

#endif /* __VMM_MSGLIST_MANAGER_H__ */

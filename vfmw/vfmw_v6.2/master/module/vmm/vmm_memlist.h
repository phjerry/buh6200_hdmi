/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_MEMLIST_MANAGER_H__
#define __VMM_MEMLIST_MANAGER_H__

#include "vmm_ext.h"
#include "vmm_dbg.h"

typedef enum {
    NODE_STATUS_IDLE = 0,
    NODE_STATUS_INUSE,
    NODE_STATUS_NOUSE,
    NODE_STATUS_MAX
} mem_node_status;

typedef struct {
    hi_u32 alloc_time_consume_in_us;
} mem_dbg_info;

typedef struct {
    mem_node_status status;
    vmm_location location;
    vmm_buffer buf_info;
    hi_u32 ref_cnt;
    mem_dbg_info dbg_info;
} mem_data_info;

typedef struct {
    struct OS_LIST_HEAD list;
    mem_data_info data;
} mem_node_info;

hi_s32 vmm_mem_list_init(hi_void);
hi_void vmm_mem_list_deinit(hi_void);
hi_s32 vmm_mem_list_add_node(mem_data_info *data);
hi_s32 vmm_mem_list_find_node_by_phyaddr(UADDR target_phy_addr, mem_data_info *data);
hi_s32 vmm_mem_list_delete_node_by_phyaddr(UADDR target_phy_addr);
hi_s32 vmm_mem_list_mark_location(UADDR target_phy_addr, vmm_location location);
hi_s32 vmm_mem_list_mark_status(UADDR target_phy_addr, mem_node_status status);
hi_s32 vmm_mem_list_is_empty(hi_void);
hi_s32 vmm_mem_list_print_node_status(hi_void);
hi_s32 vmm_mem_list_read_proc(hi_void *page);
hi_s32 vmm_mem_list_find_one_node_can_be_rls(mem_data_info *data);
hi_s32 vmm_mem_list_is_one_node_can_be_rls(UADDR target_phy_addr);
hi_s32 vmm_mem_list_set_specified_node_no_use(vmm_rls_burst *rls_burst);
hi_s32 vmm_mem_list_takeout_one_node_can_be_rls(vmm_rls_burst *rls_burst, mem_data_info *data);
hi_s32 vmm_mem_list_takeout_node(mem_data_info *data);
hi_s32 vmm_mem_list_update_buf_rec(vmm_buffer *buf_rec);

#endif /* __VMM_MEMLIST_MANAGER_H__ */

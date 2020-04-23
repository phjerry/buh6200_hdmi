/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm_comm
 * Author: sm_rm
 * Create: 2015/11/26
 */

#ifndef __DRV_RM_COMM_H__
#define __DRV_RM_COMM_H__

#include "hi_drv_rm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RM_DRV_NODE_VALID   0xfeedbeef
#define RM_DRV_NODE_INVALID 0xdeadbeef

typedef struct {
    hi_u32 list_node_num;
} rm_drv_list_attr;

typedef struct {
    struct list_head    list_node;
    hi_drv_rm_event     event_info;
    hi_u32              node_index;
    hi_u32              verify_code;
} rm_drv_list_node;

typedef struct {
    hi_u32              free_node;
    hi_u32              used_node;
    rm_drv_list_attr    list_attr;
    rm_drv_list_node    *list_node_p;
    struct list_head    used_event_list;
    struct list_head    free_event_list;
} rm_drv_event_mgr;

hi_s32 rm_drv_comm_init_event_list(rm_drv_list_attr *list_attr_p, hi_void **pph_list);
hi_s32 rm_drv_comm_de_init_event_list(hi_void *list_handle_p);
hi_s32 rm_drv_comm_get_event(hi_handle rm_handle, hi_void *list_handle_p, hi_drv_rm_event *rm_event_p);
hi_s32 rm_drv_comm_put_event(hi_handle rm_handle, hi_void *list_handle_p, hi_drv_rm_event *rm_event_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DRV_RM_COMM_H__ */


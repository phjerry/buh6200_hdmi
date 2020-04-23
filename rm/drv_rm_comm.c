/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm_comm
 * Author: sm_rm
 * Create: 2015/11/26
 */

#include "securec.h"
#include "hi_osal.h"

#include "hi_errno.h"
#include "hi_drv_win.h"
#include "hi_drv_rm.h"
#include "drv_rm.h"
#include "drv_rm_comm.h"
#include "drv_rm_define.h"

hi_s32 rm_drv_comm_init_event_list(rm_drv_list_attr *list_attr_p, hi_void **pph_list)
{
    rm_drv_event_mgr *event_mgr_p = HI_NULL;
    rm_drv_list_node *tmp_list_node_p = HI_NULL;
    hi_u32 i = 0;

    if (list_attr_p == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (pph_list == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    event_mgr_p = (rm_drv_event_mgr *)osal_vmalloc(HI_ID_RM, sizeof(rm_drv_event_mgr));
    if (event_mgr_p == HI_NULL) {
        HI_LOG_ERR("malloc memory failed\n");
        return HI_ERR_RM_INIT_LIST_ERR;
    }

    memset_s(event_mgr_p, sizeof(rm_drv_event_mgr), 0x0, sizeof(rm_drv_event_mgr));
    event_mgr_p->list_node_p = (rm_drv_list_node *)osal_vmalloc(HI_ID_RM,
        (sizeof(rm_drv_list_node) * list_attr_p->list_node_num));
    if (event_mgr_p->list_node_p == HI_NULL) {
        osal_vfree(HI_ID_RM, event_mgr_p);
        event_mgr_p = HI_NULL;
        HI_UNUSED(event_mgr_p);
        HI_LOG_ERR("malloc memory video frame failed\n");
        return HI_ERR_RM_INIT_LIST_ERR;
    }
    memset_s(event_mgr_p->list_node_p, (sizeof(rm_drv_list_node) * list_attr_p->list_node_num), 0x0,
             (sizeof(rm_drv_list_node) * list_attr_p->list_node_num));
    INIT_LIST_HEAD(&event_mgr_p->used_event_list);
    INIT_LIST_HEAD(&event_mgr_p->free_event_list);

    for (i = 0; i < list_attr_p->list_node_num; i++) {
        tmp_list_node_p = event_mgr_p->list_node_p + i;
        tmp_list_node_p->node_index = i;
        tmp_list_node_p->verify_code = RM_DRV_NODE_VALID;
        list_add_tail(&tmp_list_node_p->list_node, &event_mgr_p->free_event_list);
    }

    event_mgr_p->used_node = 0;
    event_mgr_p->free_node = list_attr_p->list_node_num;
    memcpy_s(&event_mgr_p->list_attr, sizeof(rm_drv_list_attr), list_attr_p, sizeof(rm_drv_list_attr));
    *pph_list = event_mgr_p;

    return HI_SUCCESS;
}

hi_s32 rm_drv_comm_de_init_event_list(hi_void *list_handle_p)
{
    hi_u32 i;
    rm_drv_event_mgr *event_mgr_p = HI_NULL;
    rm_drv_list_node *tmp_list_node_p = HI_NULL;

    if (list_handle_p == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    event_mgr_p = (rm_drv_event_mgr *)list_handle_p;

    /* The flag is invalid before the node is released */
    for (i = 0; i < event_mgr_p->list_attr.list_node_num; i++) {
        tmp_list_node_p = event_mgr_p->list_node_p + i;
        tmp_list_node_p->verify_code = RM_DRV_NODE_INVALID;
    }

    if (event_mgr_p->list_node_p != HI_NULL) {
        osal_vfree(HI_ID_RM, event_mgr_p->list_node_p);
        event_mgr_p->list_node_p = HI_NULL;
    }

    if (event_mgr_p != HI_NULL) {
        osal_vfree(HI_ID_RM, event_mgr_p);
        event_mgr_p = HI_NULL;
        HI_UNUSED(event_mgr_p);
    }

    return HI_SUCCESS;
}

static hi_s32 rm_drv_comm_event_pre_check(hi_handle rm_handle,
    hi_void *list_handle_p, hi_drv_rm_event *rm_event_p, rm_drv_path_ctx **rm_drv_path_ctx_p)
{
    hi_u32 rm_path_id = 0;

    if (list_handle_p == NULL) {
        HI_LOG_ERR("para arg is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (rm_event_p == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if (rm_handle == HI_INVALID_HANDLE) {
        HI_LOG_ERR("rm handle is invalid.\n");
        return HI_ERR_RM_INVALID_HANDLE;
    }

    if (((rm_path_id) = (rm_handle) & 0xff) >= RM_DRV_CHN_MAX_NUM) {
        HI_LOG_ERR("rm path id is over range.rm_id = 0x%08X\n", rm_path_id);
        return HI_ERR_RM_INST_OVER_RANGE;
    }

    rm_drv_comm_get_path_ctx(rm_path_id, rm_drv_path_ctx_p);

    if ((*rm_drv_path_ctx_p) == NULL) {
        HI_LOG_ERR("para private_data is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    if ((rm_handle) != ((*rm_drv_path_ctx_p)->rm_handle)) {
        HI_LOG_ERR("src_rm_handle is not equal dest_rm_handle.\n", rm_handle, ((*rm_drv_path_ctx_p)->rm_handle));
        hi_err_print_h32(rm_handle);
        hi_err_print_h32((*rm_drv_path_ctx_p)->rm_handle);
        return HI_ERR_RM_INVALID_HANDLE;
    }

    return HI_SUCCESS;
}

hi_s32 rm_drv_comm_get_event(hi_handle rm_handle, hi_void *list_handle_p, hi_drv_rm_event *rm_event_p)
{
    struct list_head *cur_list_p = NULL;
    rm_drv_event_mgr *event_mgr_p = HI_NULL;
    rm_drv_list_node *tmp_list_node_p = HI_NULL;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    hi_s32 ret;

    ret = rm_drv_comm_event_pre_check(rm_handle, list_handle_p, rm_event_p, &rm_drv_path_ctx_p);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    event_mgr_p = (rm_drv_event_mgr *)list_handle_p;

    if (list_empty(&event_mgr_p->used_event_list)) {
        HI_LOG_DBG("no used buffer.\n");
        rm_drv_path_ctx_p->list_empty = HI_TRUE;
        return HI_FAILURE;
    }

    cur_list_p = event_mgr_p->used_event_list.next;
    if (cur_list_p == HI_NULL) {
        HI_LOG_ERR("cur_list_p is NULL.\n");
        return HI_FAILURE;
    }

    tmp_list_node_p = list_entry(cur_list_p, rm_drv_list_node, list_node);
    if (tmp_list_node_p == HI_NULL) {
        HI_LOG_ERR("tmp_list_node_p is NULL.\n");
        return HI_FAILURE;
    }

    memcpy_s(rm_event_p, sizeof(hi_drv_rm_event), &tmp_list_node_p->event_info, sizeof(hi_drv_rm_event));

    list_del(&tmp_list_node_p->list_node);
    list_add_tail(&tmp_list_node_p->list_node, &event_mgr_p->free_event_list);
    /* record debug info */
    event_mgr_p->used_node--;
    event_mgr_p->free_node++;
    rm_drv_path_ctx_p->used_node = event_mgr_p->used_node;
    rm_drv_path_ctx_p->free_node = event_mgr_p->free_node;
    rm_drv_path_ctx_p->cur_event = rm_event_p->win_event;

    return HI_SUCCESS;
}

/* release A frame to  free list**/
hi_s32 rm_drv_comm_put_event(hi_handle rm_handle, hi_void *list_handle_p, hi_drv_rm_event *rm_event_p)
{
    struct list_head *cur_list_p = NULL;
    rm_drv_event_mgr *event_mgr_p = HI_NULL;
    rm_drv_list_node *tmp_list_node_p = HI_NULL;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    hi_s32 ret;

    ret = rm_drv_comm_event_pre_check(rm_handle, list_handle_p, rm_event_p, &rm_drv_path_ctx_p);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    event_mgr_p = (rm_drv_event_mgr *)list_handle_p;
    if (list_empty(&event_mgr_p->free_event_list)) {
        HI_LOG_DBG("no free buffer.\n");
        return HI_FAILURE;
    }

    cur_list_p = event_mgr_p->free_event_list.next;
    if (cur_list_p == HI_NULL) {
        HI_LOG_ERR("cur_list_p is NULL.\n");
        return HI_FAILURE;
    }

    tmp_list_node_p = list_entry(cur_list_p, rm_drv_list_node, list_node);
    if (tmp_list_node_p == HI_NULL) {
        HI_LOG_ERR("tmp_list_node_p is NULL.\n");
        return HI_FAILURE;
    }

    /* Ensure that the node will not be memcpy after release */
    if (tmp_list_node_p->verify_code != RM_DRV_NODE_VALID) {
        HI_LOG_ERR("rm event node is invalid, code:%x\n", tmp_list_node_p->verify_code);
        return HI_FAILURE;
    }

    memcpy_s(&tmp_list_node_p->event_info, sizeof(hi_drv_rm_event), rm_event_p, sizeof(hi_drv_rm_event));
    list_del(&tmp_list_node_p->list_node);
    list_add_tail(&tmp_list_node_p->list_node, &event_mgr_p->used_event_list);
    /* record debug info */
    event_mgr_p->used_node++;
    event_mgr_p->free_node--;
    rm_drv_path_ctx_p->used_node = event_mgr_p->used_node;
    rm_drv_path_ctx_p->free_node = event_mgr_p->free_node;
    rm_drv_path_ctx_p->cur_index = tmp_list_node_p->node_index;

    return HI_SUCCESS;
}


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi buf
 * Author: sdk
 * Create: 2019-12-14
 */

#include "vi_comm.h"
#include "vi_type.h"

#include "vi_buf.h"

typedef struct {
    vi_buf_attr buf_attr;
    vi_buf_node *vi_node_p;

    struct osal_list_head empty_list;
    struct osal_list_head idle_list;
    struct osal_list_head busy_list;
    struct osal_list_head used_list;

    osal_spinlock buf_lock;

    vi_buf_dbg_info dbg_info;
} vi_buf_mgr;

VI_STATIC hi_s32 buf_get_node_index(vi_buf_mgr *buf_mgr_p, vi_node_status min_status, vi_node_status max_status,
                                    hi_drv_video_frame *video_frame_p, hi_u32 *index_p)
{
    hi_s32 ret;
    hi_u32 i;
    vi_buf_node *vi_node_p = HI_NULL;
    vi_buffer_addr_info *video_buf_addr_p = HI_NULL;

    if (video_frame_p->buf_addr[0].start_addr == 0) {
        vi_drv_log_err("get node_info failure\n");
        return HI_FAILURE;
    }

    ret = vi_comm_query_phy_addr(video_frame_p->buf_addr[0].start_addr);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("query (0x%x) fail!\n", video_frame_p->buf_addr[0].start_addr);
        return HI_FAILURE;
    }

    for (i = 0; i < buf_mgr_p->buf_attr.max_buf_num; i++) {
        vi_node_p = buf_mgr_p->vi_node_p + i;
        video_buf_addr_p = &vi_node_p->frame_addr.video_buf_addr;

        if ((vi_node_p->status >= min_status) && (vi_node_p->status <= max_status) &&
            (video_frame_p->buf_addr[0].start_addr == video_buf_addr_p->buf_handle)) {
            *index_p = i;
            break;
        }
    }

    if (i >= buf_mgr_p->buf_attr.max_buf_num) {
        vi_drv_log_err("get node_info  failure\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 buf_check_attr(vi_buf_attr *buf_attr_p)
{
    if (buf_attr_p->buf_size == 0) {
        vi_drv_log_err("invalid buf size(%d)\n", buf_attr_p->buf_size);
        return HI_FAILURE;
    }

    if (buf_attr_p->min_buf_num == 0) {
        vi_drv_log_err("invalid min buf num(%d)\n", buf_attr_p->min_buf_num);
        return HI_FAILURE;
    }

    if (buf_attr_p->max_buf_num == 0) {
        vi_drv_log_err("invalid max buf num(%d)\n", buf_attr_p->max_buf_num);
        return HI_FAILURE;
    }

    if (buf_attr_p->min_buf_num > buf_attr_p->max_buf_num) {
        vi_drv_log_err("invalid max/min(%d,%d)\n", buf_attr_p->max_buf_num, buf_attr_p->min_buf_num);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 buf_alloc_buf_mgr(vi_buf_attr *buf_attr_p, vi_buf_mgr **buf_mgr_pp)
{
    hi_u32 i;
    hi_u32 nodes_size;
    vi_buf_mgr *buf_mgr_p = HI_NULL;
    vi_buf_node *vi_node_p = HI_NULL;

    /* alloc vicap buffer manager struct */
    buf_mgr_p = (vi_buf_mgr *)osal_vmalloc(HI_ID_VI, sizeof(vi_buf_mgr));
    if (buf_mgr_p == HI_NULL) {
        vi_drv_log_err("vmalloc failed\n");
        goto FAIL;
    }

    memset_s(buf_mgr_p, sizeof(vi_buf_mgr), 0, sizeof(vi_buf_mgr));
    memcpy_s(&buf_mgr_p->buf_attr, sizeof(vi_buf_attr), buf_attr_p, sizeof(vi_buf_attr));

    /* alloc vicap buffer manager member: vi_node_p */
    nodes_size = sizeof(vi_buf_node) * buf_attr_p->max_buf_num;
    buf_mgr_p->vi_node_p = (vi_buf_node *)osal_vmalloc(HI_ID_VI, nodes_size);
    if (buf_mgr_p->vi_node_p == HI_NULL) {
        vi_drv_log_err("vmalloc failed\n");
        goto FAIL;
    }

    memset_s(buf_mgr_p->vi_node_p, nodes_size, 0, nodes_size);

    osal_spin_lock_init(&buf_mgr_p->buf_lock);

    OSAL_INIT_LIST_HEAD(&buf_mgr_p->empty_list);
    OSAL_INIT_LIST_HEAD(&buf_mgr_p->idle_list);
    OSAL_INIT_LIST_HEAD(&buf_mgr_p->busy_list);
    OSAL_INIT_LIST_HEAD(&buf_mgr_p->used_list);

    for (i = 0; i < buf_attr_p->max_buf_num; i++) {
        vi_node_p = buf_mgr_p->vi_node_p + i;

        vi_node_p->index = i;
        vi_node_p->reference = 0;
        vi_node_p->status = VI_NODE_STATUS_EMPTY;
        vi_node_p->valid = HI_FALSE;
        vi_node_p->src_fence = -1;
        vi_node_p->sink_fence = -1;

        osal_list_add_tail(&vi_node_p->list, &buf_mgr_p->empty_list);
        buf_mgr_p->dbg_info.empty_node_num++;
    }

    *buf_mgr_pp = buf_mgr_p;

    return HI_SUCCESS;

FAIL:
    if (buf_mgr_p != HI_NULL) {
        if (buf_mgr_p->vi_node_p != HI_NULL) {
            osal_vfree(HI_ID_VI, buf_mgr_p->vi_node_p);
        }

        if (buf_mgr_p->buf_lock.lock != HI_NULL) {
            osal_spin_lock_destory(&buf_mgr_p->buf_lock);
        }
        osal_vfree(HI_ID_VI, buf_mgr_p);
    }

    return HI_FAILURE;
}

VI_STATIC hi_void buf_free_buf_mgr(vi_buf_mgr *buf_mgr_p)
{
    if (buf_mgr_p == HI_NULL) {
        vi_drv_log_err("null param!\n");
        return;
    }

    osal_spin_lock_destory(&buf_mgr_p->buf_lock);

    osal_vfree(HI_ID_VI, buf_mgr_p->vi_node_p);
    osal_vfree(HI_ID_VI, buf_mgr_p);
}

VI_STATIC hi_s32 buf_alloc_frm_mem(vi_buf_mgr *buf_mgr_p, vi_frame_addr_info *addr_p, hi_u32 id)
{
    hi_s32 ret;
    hi_char chn_name[32] = {0}; /* len is 32 */
    vi_buffer_addr_info *video_buf_addr_p = &addr_p->video_buf_addr;

    /* buffer name */
    ret = snprintf_s(chn_name, sizeof(chn_name), sizeof(chn_name) - 1, "vi_chn_buf_%d", id);
    if (ret < 0) {
        return HI_FAILURE;
    }

    /* frame alloc */
    video_buf_addr_p->size = buf_mgr_p->buf_attr.buf_size;
    video_buf_addr_p->alloc_type = (hi_u32)VI_BUFFER_ALLOC_TYPE_AUTO;

    ret = vi_comm_alloc(chn_name, video_buf_addr_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("alloc frame buffer failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_void buf_free_frm_mem(vi_buf_mgr *buf_mgr_p, vi_frame_addr_info *addr_p)
{
    vi_comm_release(&addr_p->video_buf_addr);
}

VI_STATIC hi_s32 buf_alloc_node(vi_buf_mgr *buf_mgr_p, vi_buf_node *vi_node_p)
{
    hi_s32 ret;
    unsigned long vi_buf_lock_flag;

    ret = buf_alloc_frm_mem(buf_mgr_p, &vi_node_p->frame_addr, buf_mgr_p->dbg_info.alloc_cnt);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("alloc frm err:0x%x\n", ret);
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    if (vi_node_p->status == VI_NODE_STATUS_EMPTY) {
        buf_mgr_p->dbg_info.empty_node_num--;
    } else {
        vi_drv_log_err("node status is error!\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    osal_list_add_tail(&vi_node_p->list, &buf_mgr_p->idle_list);
    vi_node_p->status = VI_NODE_STATUS_IDLE;
    buf_mgr_p->dbg_info.idle_node_num++;
    buf_mgr_p->dbg_info.full_node_num++;
    buf_mgr_p->dbg_info.alloc_cnt++;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;
}

VI_STATIC hi_void buf_free_node(vi_buf_mgr *buf_mgr_p, vi_buf_node *vi_node_p)
{
    unsigned long vi_buf_lock_flag;
    vi_frame_addr_info frame_addr = {0};

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    if (vi_node_p->status == VI_NODE_STATUS_EMPTY) {
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return;
    }

    memcpy_s(&frame_addr, sizeof(vi_frame_addr_info), &vi_node_p->frame_addr, sizeof(vi_frame_addr_info));

    vi_node_p->status = VI_NODE_STATUS_EMPTY;
    vi_node_p->reference = 0;
    vi_node_p->valid = HI_FALSE;
    memset_s(&vi_node_p->frame_info, sizeof(hi_drv_video_frame), 0, sizeof(hi_drv_video_frame));
    memset_s(&vi_node_p->frame_addr, sizeof(vi_frame_addr_info), 0, sizeof(vi_frame_addr_info));

    osal_list_add_tail(&vi_node_p->list, &buf_mgr_p->empty_list);
    buf_mgr_p->dbg_info.empty_node_num++;
    buf_mgr_p->dbg_info.full_node_num--;
    buf_mgr_p->dbg_info.free_cnt++;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    buf_free_frm_mem(buf_mgr_p, &frame_addr);
}

VI_STATIC hi_void buf_free_all_node(vi_buf_mgr *buf_mgr_p)
{
    hi_u32 i;
    vi_buf_node *vi_node_p = HI_NULL;
    unsigned long vi_buf_lock_flag;
    vi_frame_addr_info frame_addr = {0};

    for (i = 0; i < buf_mgr_p->buf_attr.max_buf_num; i++) {
        osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        vi_node_p = buf_mgr_p->vi_node_p + i;

        if (vi_node_p->status == VI_NODE_STATUS_EMPTY) {
            osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
            continue;
        }

        osal_list_del(&vi_node_p->list);
        buf_mgr_p->dbg_info.idle_node_num--;
        buf_mgr_p->dbg_info.full_node_num--;

        memcpy_s(&frame_addr, sizeof(vi_frame_addr_info), &vi_node_p->frame_addr, sizeof(vi_frame_addr_info));

        vi_node_p->status = VI_NODE_STATUS_EMPTY;
        vi_node_p->reference = 0;
        vi_node_p->valid = HI_FALSE;
        memset_s(&vi_node_p->frame_info, sizeof(hi_drv_video_frame), 0, sizeof(hi_drv_video_frame));
        memset_s(&vi_node_p->frame_addr, sizeof(vi_frame_addr_info), 0, sizeof(vi_frame_addr_info));

        osal_list_add_tail(&vi_node_p->list, &buf_mgr_p->empty_list);
        buf_mgr_p->dbg_info.empty_node_num++;
        buf_mgr_p->dbg_info.free_cnt++;

        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

        buf_free_frm_mem(buf_mgr_p, &frame_addr);
    }
}

VI_STATIC hi_void buf_sink_fence_release_try(vi_buf_mgr *buf_mgr_p)
{
    hi_u32 ret;
    unsigned long vi_buf_lock_flag;
    struct osal_list_head *cur_list = NULL;
    vi_buf_node *vi_node_p = HI_NULL;

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    cur_list = buf_mgr_p->used_list.next;

    while (!osal_list_is_last(cur_list, &buf_mgr_p->used_list)) {
        vi_node_p = osal_list_entry(cur_list, vi_buf_node, list);
        cur_list = cur_list->next;

        if (vi_node_p->sink_fence >= 0) {
            ret = osal_fence_trywait(vi_node_p->sink_fence);
            if (ret != HI_SUCCESS) {
                continue;
            }

            vi_drv_log_dbg("release sink_fence[%d]\n", vi_node_p->sink_fence);

            osal_fence_destroy(HI_ID_VI, vi_node_p->sink_fence);
            vi_node_p->sink_fence = -1;
        }

        if (vi_node_p->src_fence >= 0) {
            vi_drv_log_dbg("release source fence[%d]\n", vi_node_p->src_fence);
            osal_fence_destroy(HI_ID_VI, vi_node_p->src_fence);
            vi_node_p->src_fence = -1;
        }

        if (vi_node_p->reference <= 0) {
            osal_list_del(&vi_node_p->list);
            buf_mgr_p->dbg_info.used_node_num--;

            osal_list_add_tail(&vi_node_p->list, &buf_mgr_p->idle_list);
            vi_node_p->status = VI_NODE_STATUS_IDLE;
            buf_mgr_p->dbg_info.idle_node_num++;
        }
    }

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
}

VI_STATIC hi_s32 buf_src_fence_create(hi_s32 *fence_p)
{
    hi_u32 ret;
    hi_s32 fence;
    hi_drv_fence_info *fence_info = HI_NULL;

    ret = osal_fence_create(HI_ID_VI, &fence, sizeof(hi_drv_fence_info));
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("osal_fence_create is error!\n");
        return HI_FAILURE;
    }

    ret = osal_fence_acquire_private(HI_ID_VI, fence, (void *)&fence_info);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("osal_fence_acquire_private is error!\n");
    }

    fence_info->need_display = HI_TRUE;

    ret = osal_fence_release_private(HI_ID_VI, fence, (void *)fence_info);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("osal_fence_release_private is error!\n");
    }

    *fence_p = fence;

    return HI_SUCCESS;
}

VI_STATIC hi_void buf_src_fence_destroy(hi_s32 fence)
{
    osal_fence_destroy(HI_ID_VI, fence);
}

hi_s32 vi_buf_init(vi_buf_attr *buf_attr_p, hi_void **buf_handle_pp)
{
    hi_u32 ret;
    hi_u32 i;
    vi_buf_mgr *buf_mgr_p = HI_NULL;
    vi_buf_node *vi_node_p = HI_NULL;

    vi_drv_func_enter();

    if (buf_attr_p == HI_NULL || buf_handle_pp == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    ret = buf_check_attr(buf_attr_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("check attr err:0x%x\n", ret);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = buf_alloc_buf_mgr(buf_attr_p, &buf_mgr_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("alloc buf err:0x%x\n", ret);
        goto FAIL;
    }

    for (i = 0; i < buf_attr_p->min_buf_num; i++) {
        vi_node_p = buf_mgr_p->vi_node_p + i;
        ret = buf_alloc_node(buf_mgr_p, vi_node_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("alloc node err:0x%x\n", ret);
            goto FAIL;
        }
    }

    *buf_handle_pp = buf_mgr_p;

    vi_drv_func_exit();

    return HI_SUCCESS;

FAIL:
    if (buf_mgr_p != HI_NULL) {
        buf_free_all_node(buf_mgr_p);
        buf_free_buf_mgr(buf_mgr_p);
    }

    *buf_handle_pp = HI_NULL;

    return HI_FAILURE;
}

hi_s32 vi_buf_deinit(hi_void *buf_handle_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;

    vi_drv_func_enter();

    if (buf_mgr_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    buf_free_all_node(buf_mgr_p);

    buf_free_buf_mgr(buf_mgr_p);

    vi_drv_func_exit();

    return HI_SUCCESS;
}

hi_s32 vi_buf_get_frame(hi_void *buf_handle_p, vi_node_status status, hi_drv_video_frame *frame_p)
{
    struct osal_list_head *list_p = NULL;
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    vi_buf_node *vi_node_p = HI_NULL;
    hi_drv_video_frame *tmp_video_frame = HI_NULL;
    unsigned long vi_buf_lock_flag;
    hi_s32 fence = -1;

    if (buf_mgr_p == HI_NULL || frame_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    buf_sink_fence_release_try(buf_mgr_p);

    if (buf_src_fence_create(&fence) != HI_SUCCESS) {
        vi_drv_log_err("error!\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    if (osal_list_empty(&buf_mgr_p->busy_list)) {
        vi_drv_log_err("busy_list empty\n");
        goto FAIL;
    }

    list_p = buf_mgr_p->busy_list.next;

    vi_node_p = osal_list_entry(list_p, vi_buf_node, list);
    tmp_video_frame = &vi_node_p->frame_info;

    if (vi_node_p->status != VI_NODE_STATUS_BUSY) {
        vi_drv_log_err("node status is error(%d)!\n", vi_node_p->status);
        goto FAIL;
    } else if (vi_node_p->reference <= 0) {
        vi_drv_log_err("node quote cnt is error!\n");
        goto FAIL;
    }

    vi_node_p->src_fence = fence;
    vi_node_p->sink_fence = -1;

    osal_list_del(&vi_node_p->list);
    buf_mgr_p->dbg_info.busy_node_num--;

    memcpy_s(frame_p, sizeof(hi_drv_video_frame), tmp_video_frame, sizeof(hi_drv_video_frame));
    osal_list_add_tail(&vi_node_p->list, &buf_mgr_p->used_list);
    vi_node_p->status = VI_NODE_STATUS_USED;
    buf_mgr_p->dbg_info.used_node_num++;

    frame_p->source_fence = vi_node_p->src_fence;
    frame_p->sink_fence = vi_node_p->sink_fence;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    osal_fence_signal(fence);

    return HI_SUCCESS;

FAIL:
    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
    buf_src_fence_destroy(fence);
    return HI_FAILURE;
}

hi_s32 vi_buf_put_frame(hi_void *buf_handle_p, vi_node_status status, hi_drv_video_frame *frame_p)
{
    hi_s32 ret;
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    vi_buf_node *vi_node_p = HI_NULL;
    hi_u32 node_index;
    unsigned long vi_buf_lock_flag;

    if (buf_mgr_p == HI_NULL || frame_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    buf_sink_fence_release_try(buf_mgr_p);

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    ret = buf_get_node_index(buf_mgr_p, VI_NODE_STATUS_USED, VI_NODE_STATUS_USED, frame_p, &node_index);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("get index error!\n");
        goto FAIL;
    }

    vi_node_p = buf_mgr_p->vi_node_p + node_index;

    if (osal_list_empty(&vi_node_p->list)) {
        vi_drv_log_err("this node is a alone node!\n");
        goto FAIL;
    } else if (vi_node_p->status != VI_NODE_STATUS_USED) {
        vi_drv_log_err("node status is error!\n");
        goto FAIL;
    } else if (vi_node_p->reference <= 0) {
        vi_drv_log_err("node quote cnt is error!\n");
        goto FAIL;
    }

    vi_node_p->sink_fence = frame_p->sink_fence;
    vi_drv_log_dbg("add sink_fence[%d]\n", frame_p->sink_fence);

    vi_node_p->reference--;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;

FAIL:
    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
    return HI_FAILURE;
}

hi_s32 vi_buf_get_node(hi_void *buf_handle_p, vi_node_status status, vi_buf_node *node_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    unsigned long vi_buf_lock_flag;
    struct osal_list_head *cur_list = NULL;
    vi_buf_node *tmp_vi_node_p = HI_NULL;

    if (buf_mgr_p == HI_NULL || node_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    buf_sink_fence_release_try(buf_mgr_p);

    if (status != VI_NODE_STATUS_IDLE) {
        vi_drv_log_err("input status error(%d)!\n", status);
        return HI_ERR_VI_INVALID_PARA;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    if (osal_list_empty(&buf_mgr_p->idle_list)) {
        vi_drv_log_err("no idle node.\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    cur_list = buf_mgr_p->idle_list.next;
    tmp_vi_node_p = osal_list_entry(cur_list, vi_buf_node, list);
    if (tmp_vi_node_p->status != VI_NODE_STATUS_IDLE) {
        vi_drv_log_err("node status is error!\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    if (tmp_vi_node_p->reference != 0) {
        vi_drv_log_err("node reference is error!\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    osal_list_del(&tmp_vi_node_p->list);
    OSAL_INIT_LIST_HEAD(&tmp_vi_node_p->list);

    tmp_vi_node_p->status = VI_NODE_STATUS_ALONE;
    tmp_vi_node_p->reference++;
    memcpy_s(node_p, sizeof(vi_buf_node), tmp_vi_node_p, sizeof(vi_buf_node));

    buf_mgr_p->dbg_info.idle_node_num--;
    buf_mgr_p->dbg_info.alone_node_num++;

    vi_drv_log_dbg("get node idex(%d)\n", tmp_vi_node_p->index);

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;
}

hi_s32 vi_buf_put_node(hi_void *buf_handle_p, vi_node_status status, vi_buf_node *node_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    unsigned long vi_buf_lock_flag;
    vi_buf_node *tmp_vi_node_p = HI_NULL;

    if (buf_mgr_p == HI_NULL || node_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    if ((status != VI_NODE_STATUS_BUSY) && (status != VI_NODE_STATUS_IDLE)) {
        vi_drv_log_err("input status error(%d)!\n", status);
        return HI_ERR_VI_INVALID_PARA;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    tmp_vi_node_p = buf_mgr_p->vi_node_p + node_p->index;

    vi_drv_log_dbg("put node idex(%d)\n", node_p->index);

    if (node_p->frame_addr.video_buf_addr.start_phy_addr != tmp_vi_node_p->frame_addr.video_buf_addr.start_phy_addr) {
        vi_drv_log_err("phy addr compare fail!\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    if (!osal_list_empty(&tmp_vi_node_p->list)) {
        vi_drv_log_err("not alone node(%d)!\n", tmp_vi_node_p->status);
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    if (tmp_vi_node_p->status != VI_NODE_STATUS_ALONE) {
        vi_drv_log_err("node status is error(%d)!\n", tmp_vi_node_p->status);
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    if (tmp_vi_node_p->reference != 1) {
        vi_drv_log_err("node quote cnt is error!\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    memcpy_s(&tmp_vi_node_p->frame_info, sizeof(hi_drv_video_frame), &node_p->frame_info, sizeof(hi_drv_video_frame));

    tmp_vi_node_p->status = status;
    buf_mgr_p->dbg_info.alone_node_num--;

    if (status == VI_NODE_STATUS_BUSY) {
        osal_list_add_tail(&tmp_vi_node_p->list, &buf_mgr_p->busy_list);
        buf_mgr_p->dbg_info.busy_node_num++;
    } else if (status == VI_NODE_STATUS_IDLE) {
        osal_list_add_tail(&tmp_vi_node_p->list, &buf_mgr_p->idle_list);
        buf_mgr_p->dbg_info.idle_node_num++;
    }

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;
}

hi_s32 vi_buf_add(hi_void *buf_handle_p, vi_buf_node *node_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    vi_buf_node *tmp_vi_node_p = HI_NULL;
    unsigned long vi_buf_lock_flag;

    if (buf_mgr_p == HI_NULL || node_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    if (node_p->status != VI_NODE_STATUS_ALONE) {
        vi_drv_log_err("node status error(%d)!\n", node_p->status);
        return HI_ERR_VI_INVALID_PARA;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    tmp_vi_node_p = buf_mgr_p->vi_node_p + node_p->index;

    if (node_p->frame_addr.video_buf_addr.start_phy_addr != tmp_vi_node_p->frame_addr.video_buf_addr.start_phy_addr) {
        vi_drv_log_err("phy addr compare fail!\n");
        goto FAIL;
    }

    if (osal_list_empty(&tmp_vi_node_p->list)) {
        vi_drv_log_err("this node is a alone node!\n");
        goto FAIL;
    }

    if ((tmp_vi_node_p->status != VI_NODE_STATUS_BUSY) && (tmp_vi_node_p->status != VI_NODE_STATUS_USED)) {
        vi_drv_log_err("node status is error!\n");
        goto FAIL;
    }

    if (tmp_vi_node_p->reference <= 0) {
        vi_drv_log_err("node quote cnt is error!\n");
        goto FAIL;
    }

    tmp_vi_node_p->reference++;
    buf_mgr_p->dbg_info.user_node_num++;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;

FAIL:
    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
    return HI_FAILURE;
}

hi_s32 vi_buf_sub(hi_void *buf_handle_p, vi_buf_node *node_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    vi_buf_node *tmp_vi_node_p = HI_NULL;
    unsigned long vi_buf_lock_flag;

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    tmp_vi_node_p = buf_mgr_p->vi_node_p + node_p->index;

    if (node_p->frame_addr.video_buf_addr.start_phy_addr != tmp_vi_node_p->frame_addr.video_buf_addr.start_phy_addr) {
        vi_drv_log_err("phy addr compare fail!\n");
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_FAILURE;
    }

    if (osal_list_empty(&tmp_vi_node_p->list)) {
        vi_drv_log_err("this node is a alone node!\n");
        goto FAIL;
    }

    if ((tmp_vi_node_p->status != VI_NODE_STATUS_BUSY) && (tmp_vi_node_p->status != VI_NODE_STATUS_USED)) {
        vi_drv_log_err("node status is error!\n");
        goto FAIL;
    }

    if (tmp_vi_node_p->reference <= 0) {
        vi_drv_log_err("node quote cnt is error!\n");
        goto FAIL;
    }

    tmp_vi_node_p->reference--;
    buf_mgr_p->dbg_info.user_node_num--;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;

FAIL:
    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
    return HI_FAILURE;
}

hi_s32 vi_buf_check_busy(hi_void *buf_handle_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    unsigned long vi_buf_lock_flag;

    if (buf_mgr_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_ERR_VI_NULL_PTR;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    if (osal_list_empty(&buf_mgr_p->busy_list)) {
        osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);
        return HI_ERR_VI_BUF_EMPTY;
    }

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return HI_SUCCESS;
}

hi_u32 vi_buf_get_idle_cnt(hi_void *buf_handle_p)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    unsigned long vi_buf_lock_flag;
    hi_u32 cnt;

    if (buf_mgr_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return 0;
    }

    osal_spin_lock_irqsave(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    cnt = buf_mgr_p->dbg_info.idle_node_num;

    osal_spin_unlock_irqrestore(&buf_mgr_p->buf_lock, &vi_buf_lock_flag);

    return cnt;
}

hi_void vi_buf_proc_print(hi_void *buf_handle_p, hi_void *s)
{
    vi_buf_mgr *buf_mgr_p = (vi_buf_mgr *)buf_handle_p;
    vi_buf_node *vi_node_p = HI_NULL;
    hi_u32 i;

    if (buf_mgr_p == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return;
    }

    osal_proc_print(s, "%-40s:%10d,%10d,%10d\n", "Node(size/min/max)", buf_mgr_p->buf_attr.buf_size,
                    buf_mgr_p->buf_attr.min_buf_num, buf_mgr_p->buf_attr.max_buf_num);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d,%10d\n", "Node(ThreadCnt/Alloc/Free/Empty/Full)",
                    buf_mgr_p->dbg_info.thread_cnt, buf_mgr_p->dbg_info.alloc_cnt, buf_mgr_p->dbg_info.free_cnt,
                    buf_mgr_p->dbg_info.empty_node_num, buf_mgr_p->dbg_info.full_node_num);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d,%10d\n", "Node(User/Idle/Alone/Busy/Used)",
                    buf_mgr_p->dbg_info.user_node_num, buf_mgr_p->dbg_info.idle_node_num,
                    buf_mgr_p->dbg_info.alone_node_num, buf_mgr_p->dbg_info.busy_node_num,
                    buf_mgr_p->dbg_info.used_node_num);

    for (i = 0; i < buf_mgr_p->buf_attr.max_buf_num; i++) {
        vi_node_p = buf_mgr_p->vi_node_p + i;
        osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d,%10llX\n", "Node(ref/status/src/sink/phy_addr)",
                        vi_node_p->reference, vi_node_p->status, vi_node_p->src_fence, vi_node_p->sink_fence,
                        vi_node_p->frame_addr.video_buf_addr.start_phy_addr);
    }
}

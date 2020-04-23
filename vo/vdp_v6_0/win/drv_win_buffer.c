/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: functions define
 * Author: VDP
 * Create: 2019-6-25
 */

#include "osal_list.h"
#include "drv_win_buffer_priv.h"
#include "hi_type.h"
#include "drv_xdp_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WIN_BUFFER_NODE_MAX 30
#define WIN_BUFFER_MAX 32
#define WIN_BUFFER_INDEX_MASK 0x000000FF
#define WIN_BUFFER_PREFIX 0xFF000000
#define INVALID_META_DATA_ADDR 0xFFFFFFFFFFFFFFFF
#define win_buffer_make_handle(index) ((WIN_BUFFER_PREFIX) | (index))
#define win_buff_kmalloc(a)         \
    ({        hi_void *b = HI_NULL;       \
        b = osal_kmalloc(HI_ID_WIN, a, OSAL_GFP_KERNEL); \
        if (b != HI_NULL) {         \
            memset(b, 0, a);        \
        }                           \
        b;                          \
    })
#define win_buff_kfree(a) osal_kfree(HI_ID_WIN, a)

volatile hi_bool g_win_buffer_global_flag = WIN_BUFFER_STATE_DEINIT;
osal_spinlock g_win_buffer_spin_lock;
static win_buffer *g_win_buffer_mngr[WIN_BUFFER_MAX];

static hi_s32 buff_check_parameters(hi_u32 buf_num, win_buffer_mode buf_mode)
{
    if (g_win_buffer_global_flag != WIN_BUFFER_STATE_INIT) {
        WIN_ERROR("cannot create window buffer before initiation!\n");
        return HI_FAILURE;
    }
    if ((buf_num > WIN_BUFFER_NODE_MAX)) {
        WIN_ERROR("window buffer number is over the limit!\n");
        return HI_FAILURE;
    }
    if ((buf_mode < WIN_BUFFER_MODE_ONLINE) || (buf_mode >= WIN_BUFFER_MODE_MAX)) {
        WIN_ERROR("window buffer mode is invalid!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 win_buffer_get_index(hi_s32 *win_buff_index)
{
    hi_s32 i;
    hi_s32 ret = HI_FAILURE;

    for (i = 0; i < WIN_BUFFER_MAX; i++) {
        if (g_win_buffer_mngr[i] == HI_NULL) {
            *win_buff_index = i;
            break;
        }
    }
    if (i != WIN_BUFFER_MAX) {
        ret = HI_SUCCESS;
    }
    return ret;
}

static win_buffer *win_buffer_get_by_handle(hi_handle buf_handle)
{
    return g_win_buffer_mngr[buf_handle & WIN_BUFFER_INDEX_MASK];
}

static hi_s32 buff_prepare(win_buffer *buff)
{
    hi_u32 i;
    hi_drv_vdp_frame *frame_node = HI_NULL;
    win_meta_buffer *meta_node = HI_NULL;

    for (i = 0; i < buff->buf_num; i++) {
        frame_node = (hi_drv_vdp_frame *)win_buff_kmalloc(sizeof(hi_drv_vdp_frame));
        if (frame_node == HI_NULL) {
            WIN_ERROR("frame node malloc failed\n");
            break;
        }
        memset(frame_node, 0, sizeof(hi_drv_vdp_frame));
        frame_node->captured_flag = HI_FALSE;
        osal_list_add_tail(&(frame_node->node), &(buff->frame_info_empty_list));

        meta_node = (win_meta_buffer *)win_buff_kmalloc(sizeof(win_meta_buffer));
        if (meta_node == HI_NULL) {
            WIN_ERROR("meta node malloc failed\n");
            break;
        }
        memset(meta_node, 0, sizeof(win_meta_buffer));
        osal_list_add_tail(&(meta_node->node), &(buff->meta_info_empty_list));
    }

    if (i != buff->buf_num) {
        WIN_ERROR("alloc buffer failed,already alloc %d total %d.\n", i, buff->buf_num);
        return HI_FAILURE;
    }
    buff->reset_mode = HI_DRV_WIN_RESET_MAX;
    buff->freeze_mode = HI_DRV_WIN_FREEZE_MAX;

    return HI_SUCCESS;
}

static hi_void buff_list_head_init(win_buffer *buff)
{
    OSAL_INIT_LIST_HEAD(&(buff->frame_info_empty_list));
    OSAL_INIT_LIST_HEAD(&(buff->frame_info_full_list));
    OSAL_INIT_LIST_HEAD(&(buff->frame_info_disp_list));
    OSAL_INIT_LIST_HEAD(&(buff->frame_info_release_list));
    OSAL_INIT_LIST_HEAD(&(buff->meta_info_empty_list));
    OSAL_INIT_LIST_HEAD(&(buff->meta_info_full_list));
}

static hi_s32 buff_meta_list_deinit(vdp_list *frame_info_list, hi_u32 *del_num)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 del_count = 0;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;

    win_meta_buffer *meta_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_info_list)
    {
        meta_node = osal_list_entry(pos, win_meta_buffer, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }
        osal_list_del_init(pos);
        del_count++;
        if (meta_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        win_buff_kfree(meta_node);
    }

    *del_num = del_count;
    return ret;
}

static hi_s32 buff_frame_list_deinit(vdp_list *frame_info_list, hi_u32 *del_num)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 del_count = 0;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_info_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }
        osal_list_del_init(pos);
        del_count++;
        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        win_buff_kfree(frame_node);
    }

    *del_num = del_count;
    return ret;
}

static hi_bool buff_all_frame_list_deinit(win_buffer *buffer)
{
    hi_s32 ret;
    hi_u32 del_count = 0;
    hi_u32 temp_count = 0;
    hi_bool deinit_success = HI_TRUE;

    ret = buff_frame_list_deinit(&(buffer->frame_info_disp_list), &temp_count);
    del_count += temp_count;
    if (ret != HI_SUCCESS) {
        deinit_success = HI_FALSE;
    }

    ret = buff_frame_list_deinit(&(buffer->frame_info_release_list), &temp_count);
    del_count += temp_count;
    if (ret != HI_SUCCESS) {
        deinit_success = HI_FALSE;
    }

    ret = buff_frame_list_deinit(&(buffer->frame_info_full_list), &temp_count);
    del_count += temp_count;
    if (ret != HI_SUCCESS) {
        deinit_success = HI_FALSE;
    }

    ret = buff_frame_list_deinit(&(buffer->frame_info_empty_list), &temp_count);
    del_count += temp_count;
    if (ret != HI_SUCCESS) {
        deinit_success = HI_FALSE;
    }

    if (del_count != buffer->buf_num) {
        WIN_ERROR("free frame list buffer failed,already free %d total %d.\n", del_count, buffer->buf_num);
        deinit_success = HI_FALSE;
    }
    del_count = 0;
    ret = buff_meta_list_deinit(&(buffer->meta_info_full_list), &temp_count);
    del_count += temp_count;
    if (ret != HI_SUCCESS) {
        deinit_success = HI_FALSE;
    }

    ret = buff_meta_list_deinit(&(buffer->meta_info_empty_list), &temp_count);
    del_count += temp_count;
    if (ret != HI_SUCCESS) {
        deinit_success = HI_FALSE;
    }

    if (del_count != buffer->buf_num) {
        WIN_ERROR("free meta buffer failed,already free %d total %d.\n", del_count, buffer->buf_num);
        deinit_success = HI_FALSE;
    }

    return deinit_success;
}

static hi_drv_vdp_frame *buff_get_one_frame_node_and_delete(vdp_list *frame_list)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            frame_node = HI_NULL;
            ret = HI_FAILURE;
            break;
        }
        osal_list_del_init(pos);
        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        ret = HI_SUCCESS;
        break;
    }

    if (ret == HI_SUCCESS) {
        return frame_node;
    }
    return HI_NULL;
}

static hi_drv_vdp_frame *buff_get_special_frame_node(vdp_list *frame_list, hi_drv_video_frame *capture_frame)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            frame_node = HI_NULL;
            break;
        }

        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }

        if (capture_frame->buf_addr[0].dma_handle == frame_node->video_frame.buf_addr[0].dma_handle) {
            break;
        }
        frame_node = HI_NULL;
    }

    return frame_node;
}

static hi_drv_vdp_frame *buff_get_special_frame_node_and_delete(vdp_list *frame_list, hi_drv_video_frame *capture_frame)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            frame_node = HI_NULL;
            break;
        }
        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }

        if (capture_frame->buf_addr[0].dma_handle == frame_node->video_frame.buf_addr[0].dma_handle) {
            osal_list_del_init(pos);
            break;
        }
        frame_node = HI_NULL;
    }
    return frame_node;
}

static hi_drv_vdp_frame *buff_get_last_frame_node_only(vdp_list *frame_list)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        ret = HI_SUCCESS;
    }

    if (ret == HI_SUCCESS) {
        return frame_node;
    }
    return HI_NULL;
}

static hi_drv_vdp_frame *buff_get_one_frame_node_only(vdp_list *frame_list)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frame_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        ret = HI_SUCCESS;
        break;
    }

    if (ret == HI_SUCCESS) {
        return frame_node;
    }
    return HI_NULL;
}

static hi_bool buff_check_frame_has_been_que(win_buffer *win_buff, hi_drv_video_frame *frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;

    frame_node = buff_get_special_frame_node(&win_buff->frame_info_full_list, frame);
    if (frame_node != HI_NULL) {
        WIN_ERROR("this frame has been in win full list 0x%llx\n", frame->buf_addr[0].dma_handle);
        return HI_TRUE;
    }
    frame_node = buff_get_special_frame_node(&win_buff->frame_info_disp_list, frame);
    if (frame_node != HI_NULL) {
        WIN_ERROR("this frame has been in win disp list 0x%llx\n", frame->buf_addr[0].dma_handle);
        return HI_TRUE;
    }
    frame_node = buff_get_special_frame_node(&win_buff->frame_info_release_list, frame);
    if (frame_node != HI_NULL) {
        WIN_ERROR("this frame has been in win release list 0x%llx\n", frame->buf_addr[0].dma_handle);
        return HI_TRUE;
    }
    return HI_FALSE;
}

#define VDP_INVALID_INDEX 0xffffffff
static hi_s32 acquire_online_frame_when_freeze(win_buffer *win_buff, hi_drv_video_frame **frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;
    hi_u32 play_index = VDP_INVALID_INDEX;
    frame_node = buff_get_one_frame_node_and_delete(&win_buff->frame_info_full_list);
    if (frame_node != HI_NULL) {
        hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;
        win_buff->buf_state.under_load_enable = HI_FALSE;
        osal_list_add_tail(&(frame_node->node), &(win_buff->frame_info_release_list));
        metadata_info = (hi_drv_win_vpss_meta_info *)frame_node->video_frame.video_private.vpss_private.data;
        play_index = metadata_info->play_index;
    } else {
        win_buff->buf_state.under_load++;
        win_buff->buf_state.under_load_enable = HI_TRUE;
    }
    if (win_buff->freeze_mode == HI_DRV_WIN_FREEZE_BLACK) {
        *frame = HI_NULL;
        return HI_FAILURE;
    }
    frame_node = buff_get_last_frame_node_only(&win_buff->frame_info_disp_list);
    if (frame_node != HI_NULL) {
        *frame = &frame_node->video_frame;
        if (play_index != VDP_INVALID_INDEX) {
            hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;
            metadata_info = (hi_drv_win_vpss_meta_info *)frame_node->video_frame.video_private.vpss_private.data;
            metadata_info->play_index = play_index;
        }
        return HI_SUCCESS;
    } else {
        WIN_WARN("acquire frame failed in freeze last mode!\n");
        *frame = HI_NULL;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 acquire_online_frame_when_pause(win_buffer *win_buff, hi_drv_video_frame **frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;

    frame_node = buff_get_last_frame_node_only(&win_buff->frame_info_disp_list);
    if (frame_node != HI_NULL) {
        *frame = &frame_node->video_frame;
        return HI_SUCCESS;
    } else {
        WIN_WARN("cannot acquire frame in pause mode!\n");
        *frame = HI_NULL;
        return HI_FAILURE;
    }
}

static hi_s32 acquire_online_frame_when_quick(win_buffer *win_buff, hi_drv_video_frame **frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;
    hi_drv_vdp_frame *temp_node = HI_NULL;

    frame_node = buff_get_one_frame_node_and_delete(&win_buff->frame_info_full_list);
    if (frame_node == HI_NULL) {
        frame_node = buff_get_one_frame_node_only(&win_buff->frame_info_disp_list);
        if (frame_node != HI_NULL) {
            *frame = &frame_node->video_frame;
            return HI_SUCCESS;
        } else {
            WIN_ERROR("cannot acquire frame in quickout mode!\n");
            *frame = HI_NULL;
            return HI_FAILURE;
        }
    }
    while (1) {
        temp_node = buff_get_one_frame_node_and_delete(&win_buff->frame_info_full_list);
        if (temp_node != HI_NULL) {
            osal_list_add_tail(&(frame_node->node), &(win_buff->frame_info_release_list));
            frame_node = temp_node;
        } else {
            *frame = &frame_node->video_frame;
            osal_list_add_tail(&(frame_node->node), &(win_buff->frame_info_disp_list));
            break;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 acquire_online_frame_normal(win_buffer *win_buff, hi_drv_video_frame **frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;

    frame_node = buff_get_one_frame_node_and_delete(&win_buff->frame_info_full_list);
    if (frame_node == HI_NULL) {
        win_buff->buf_state.under_load++;
        win_buff->buf_state.under_load_enable = HI_TRUE;

        if (win_buff->win_type == HI_DRV_VITUAL_WIN) {
            *frame = HI_NULL;
            return HI_FAILURE;
        }
        frame_node = buff_get_last_frame_node_only(&win_buff->frame_info_disp_list);
        if (frame_node != HI_NULL) {
            *frame = &frame_node->video_frame;
            return HI_SUCCESS;
        } else {
            WIN_INFO("cannot acquire frame in normal mode!\n");
            *frame = HI_NULL;
            return HI_FAILURE;
        }
    } else {
        win_buff->buf_state.under_load_enable = HI_FALSE;
        *frame = &frame_node->video_frame;
        osal_list_add_tail(&(frame_node->node), &(win_buff->frame_info_disp_list));
        return HI_SUCCESS;
    }
}

static hi_s32 win_buffer_acquire_online_frame(win_buffer *win_buff, hi_drv_video_frame **frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;

    if (win_buff->reset_flag == HI_TRUE) {
        *frame = HI_NULL;
        /* full_list is empty when reset */
        if (win_buff->is_pause == HI_TRUE) {
            win_buff->reset_flag = HI_FALSE;
            return HI_FAILURE;
        }
        frame_node = buff_get_one_frame_node_only(&win_buff->frame_info_full_list);
        if (frame_node != HI_NULL) {
            win_buff->reset_flag = HI_FALSE;
            WIN_INFO("reset black is done\n");
        }
        return HI_FAILURE;
    }

    if ((win_buff->freeze_mode == HI_DRV_WIN_FREEZE_BLACK) || (win_buff->freeze_mode == HI_DRV_WIN_FREEZE_LAST)) {
        return acquire_online_frame_when_freeze(win_buff, frame);
    }

    if (win_buff->is_pause == HI_TRUE) {
        return acquire_online_frame_when_pause(win_buff, frame);
    }

    if (win_buff->is_quick_out == HI_TRUE) {
        return acquire_online_frame_when_quick(win_buff, frame);
    }

    return acquire_online_frame_normal(win_buff, frame);
}

static hi_void buff_print_frame_buffer_list_info(struct seq_file *p, vdp_list *frame_list)
{
    hi_u32 i = 0;
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;
    hi_drv_vdp_frame *temp_node = HI_NULL;
    hi_drv_vdp_frame *frame_node[WIN_BUFFER_NODE_MAX] = { 0 };

    osal_list_for_each_safe(pos, n, frame_list)
    {
        temp_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (temp_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            ret = HI_FAILURE;
            break;
        }
        frame_node[list_entry_times - 1] = temp_node;
    }
    if (ret != HI_SUCCESS) {
        return;
    }

    for (i = 0; i < list_entry_times; i++) {
        HI_PROC_PRINT(p, "[%08x,%08llx]", frame_node[i]->video_frame.frame_index,
            frame_node[i]->video_frame.buf_addr[0].dma_handle);
        if ((i % 2) == 1) { /* 2 change print line */
            HI_PROC_PRINT(p, "\n");
            HI_PROC_PRINT(p, "%-40s:", " ");
        }
    }
    HI_PROC_PRINT(p, "\n");
}

static hi_void buff_print_meta_buffer_list_info(struct seq_file *p, vdp_list *frame_list)
{
    hi_u32 i = 0;
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;
    win_meta_buffer *temp_node = HI_NULL;
    win_meta_buffer *meta_buffer_node[WIN_BUFFER_NODE_MAX] = { 0 };

    osal_list_for_each_safe(pos, n, frame_list)
    {
        temp_node = osal_list_entry(pos, win_meta_buffer, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (temp_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            ret = HI_FAILURE;
            break;
        }
        meta_buffer_node[list_entry_times - 1] = temp_node;
    }

    if (ret != HI_SUCCESS) {
        return;
    }

    for (i = 0; i < list_entry_times; i++) {
        HI_PROC_PRINT(p, "[%08x] ", meta_buffer_node[i]->vdp_meta_info.play_index);
        if ((i % 4) == 3) { /* 4 3 change print line */
            HI_PROC_PRINT(p, "\n");
            HI_PROC_PRINT(p, "%-40s:", " ");
        }
    }
    HI_PROC_PRINT(p, "\n");
}

hi_s32 win_buffer_init(void)
{
    hi_u32 i;
    hi_s32 ret;

    if (g_win_buffer_global_flag != WIN_BUFFER_STATE_DEINIT) {
        return HI_SUCCESS;
    }

    ret = osal_spin_lock_init(&g_win_buffer_spin_lock);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("window buffer spin_lock initiation failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < WIN_BUFFER_MAX; i++) {
        g_win_buffer_mngr[i] = HI_NULL;
    }
    g_win_buffer_global_flag = WIN_BUFFER_STATE_INIT;
    WIN_INFO("window buffer initiation is done\n");

    return HI_SUCCESS;
}

hi_s32 win_buffer_deinit(void)
{
    if (g_win_buffer_global_flag != WIN_BUFFER_STATE_INIT) {
        return HI_SUCCESS;
    }
    osal_spin_lock_destory(&g_win_buffer_spin_lock);

    g_win_buffer_global_flag = WIN_BUFFER_STATE_DEINIT;
    WIN_INFO("window buffer deinitiation is done\n");
    return HI_SUCCESS;
}

hi_s32 win_buffer_destroy(hi_handle buf_handle)
{
    hi_ulong flags;
    win_buffer *win_buf;
    hi_s32 ret = HI_SUCCESS;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("window buffer handle is invalid!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    g_win_buffer_mngr[buf_handle & WIN_BUFFER_INDEX_MASK] = HI_NULL;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    if (buff_all_frame_list_deinit(win_buf) != HI_TRUE) {
        WIN_ERROR("cannot destroy window buffer!\n");
        ret = HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    osal_spin_lock_destory(&win_buf->win_buffer_spin_lock);

    win_buff_kfree(win_buf);

    return ret;
}

hi_handle win_buffer_create(hi_u32 buf_num, win_buffer_mode buf_mode, hi_drv_win_type win_type)
{
    hi_u32 win_buffer_index = 0;
    hi_s32 ret;
    hi_handle win_buffer_handle;
    hi_ulong flags;
    win_buffer *win_buf;

    ret = buff_check_parameters(buf_num, buf_mode);
    if (ret != HI_SUCCESS) {
        return HI_INVALID_HANDLE;
    }
    win_buf = (win_buffer *)win_buff_kmalloc(sizeof(win_buffer));
    if (win_buf == HI_NULL) {
        WIN_ERROR("window buff malloc failed\n");
        return HI_INVALID_HANDLE;
    }
    memset(win_buf, 0x0, sizeof(win_buffer));
    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    ret = win_buffer_get_index(&win_buffer_index);
    if (ret) {
        WIN_ERROR("window buff index failed\n");
        win_buff_kfree(win_buf);
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_INVALID_HANDLE;
    }
    win_buffer_handle = win_buffer_make_handle(win_buffer_index);
    g_win_buffer_mngr[win_buffer_handle & WIN_BUFFER_INDEX_MASK] = win_buf;
    win_buf->buff_mode = buf_mode;
    win_buf->win_type = win_type;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    ret = osal_spin_lock_init(&win_buf->win_buffer_spin_lock);
    if (ret != HI_SUCCESS) {
        WIN_ERROR("window buff instance lock inititation failed\n");
        win_buff_kfree(win_buf);
        return HI_INVALID_HANDLE;
    }

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);

    buff_list_head_init(win_buf);
    win_buf->buf_num = buf_num;
    ret = buff_prepare(win_buf);
    if (ret) {
        WIN_ERROR("win buffer preparation failed\n");
        win_buffer_destroy(win_buffer_handle);
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_INVALID_HANDLE;
    }
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return win_buffer_handle;
}

hi_s32 win_buffer_queue_frame(hi_handle buf_handle, hi_drv_video_frame *frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
    win_buf->buf_state.try_queue++;
    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    if (buff_check_frame_has_been_que(win_buf, frame) == HI_TRUE) {
        WIN_ERROR("queue same frame, failed!\n");
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    frame_node = buff_get_one_frame_node_and_delete(&(win_buf->frame_info_empty_list));
    if (frame_node == HI_NULL) {
        WIN_ERROR("cannot get frame in emply list!\n");
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    win_buf->buf_state.suc_queue++;
    memcpy(&frame_node->video_frame, frame, sizeof(hi_drv_video_frame));
    osal_list_add_tail(&(frame_node->node), &(win_buf->frame_info_full_list));
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_dequeue_frame(hi_handle buf_handle, hi_drv_video_frame *frame)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 list_entry_times = 0;
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_drv_vdp_frame *frame_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    win_buf->buf_state.try_de_queue++;
    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    osal_list_for_each_safe(pos, n, &win_buf->frame_info_release_list)
    {
        frame_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            frame_node = HI_NULL;
            ret = HI_FAILURE;
            break;
        }
        if (frame_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        if (frame_node->captured_flag == HI_TRUE) {
            continue;
        }
        osal_list_del_init(pos);
        ret = HI_SUCCESS;
        break;
    }
    if (ret != HI_SUCCESS) {
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    win_buf->buf_state.suc_dequeue++;
    memcpy(frame, &frame_node->video_frame, sizeof(hi_drv_video_frame));
    osal_list_add_tail(&(frame_node->node), &(win_buf->frame_info_empty_list));
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
    return HI_SUCCESS;
}

hi_s32 win_buffer_set_work_mode(hi_handle buf_handle, win_buffer_mode buf_mode)
{
    win_buffer *win_buf;
    hi_ulong flags;

    if ((buf_mode < WIN_BUFFER_MODE_ONLINE) || (buf_mode >= WIN_BUFFER_MODE_MAX)) {
        WIN_ERROR("set work mode failed, invalid word mod!\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    win_buf->buff_mode = buf_mode;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_get_work_mode(hi_handle buf_handle, win_buffer_mode *buf_mode)
{
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    *buf_mode = win_buf->buff_mode;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_reset(hi_handle buf_handle, hi_drv_win_reset_mode reset_mode)
{
    win_buffer *win_buf;
    hi_drv_vdp_frame *frame_node;
    hi_ulong flags;

    if ((reset_mode < HI_DRV_WIN_RESET_LAST) || (reset_mode > HI_DRV_WIN_RESET_MAX)) {
        WIN_ERROR("reset mode(%d) is invalid!\n", reset_mode);
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buf->reset_mode = reset_mode;
    if (win_buf->reset_mode == HI_DRV_WIN_RESET_LAST) {
    } else if (win_buf->reset_mode == HI_DRV_WIN_RESET_BLACK) {
        win_buf->reset_flag = HI_TRUE;
        while (1) {
            frame_node = buff_get_one_frame_node_and_delete(&(win_buf->frame_info_full_list));
            if (frame_node == HI_NULL) {
                break;
            }
            osal_list_add_tail(&(frame_node->node), &(win_buf->frame_info_release_list));
        }
    }
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_freeze(hi_handle buf_handle, hi_drv_win_freeze_mode freeze_mode)
{
    win_buffer *win_buf;
    hi_ulong flags;

    if ((freeze_mode < HI_DRV_WIN_FREEZE_DISABLE) || (freeze_mode >= HI_DRV_WIN_FREEZE_MAX)) {
        WIN_ERROR("freeze mode(%d) is invalid!\n", freeze_mode);
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    if (win_buf->is_pause == HI_TRUE) {
        WIN_ERROR("window is paused now, cannot be freezed!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    if (win_buf->is_quick_out == HI_TRUE) {
        WIN_ERROR("window is in quickout mode now, cannot be freezed!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    win_buf->freeze_mode = freeze_mode;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}
hi_s32 win_buffer_pause(hi_handle buf_handle, hi_bool is_pause)
{
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    if ((win_buf->freeze_mode == HI_DRV_WIN_FREEZE_LAST) || (win_buf->freeze_mode == HI_DRV_WIN_FREEZE_BLACK)) {
        WIN_ERROR("window is freezed now, cannot be paused!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    if (win_buf->is_quick_out == HI_TRUE) {
        WIN_ERROR("window is in quickout mode now, cannot be paused!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    win_buf->is_pause = is_pause;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}
hi_s32 win_buffer_quickout(hi_handle buf_handle, hi_bool is_quick_out)
{
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    if ((win_buf->freeze_mode == HI_DRV_WIN_FREEZE_LAST) || (win_buf->freeze_mode == HI_DRV_WIN_FREEZE_BLACK)) {
        WIN_ERROR("window is freezed now, cannot set quickout mode!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    if (win_buf->is_pause == HI_TRUE) {
        WIN_ERROR("window is paused now, cannot set quickout mode!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    win_buf->is_quick_out = is_quick_out;
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_check_full_frame(hi_handle buf_handle, hi_drv_video_frame **frame)
{
    win_buffer *win_buf;
    hi_ulong flags;
    hi_drv_vdp_frame *frame_node = HI_NULL;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        WIN_ERROR("cannot get window buffer!\n");

        *frame = HI_NULL;
        return HI_FAILURE;
    }

    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    frame_node = buff_get_one_frame_node_only(&win_buf->frame_info_full_list);
    if (frame_node == HI_NULL) {
        *frame = HI_NULL;
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    *frame = &frame_node->video_frame;
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_acquire_frame(hi_handle buf_handle, hi_drv_video_frame **frame)
{
    win_buffer *win_buf;
    hi_s32 ret = HI_FAILURE;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    if (win_buf->buff_mode == WIN_BUFFER_MODE_ONLINE) {
        osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
        win_buf->buf_state.try_acquire++;
        ret = win_buffer_acquire_online_frame(win_buf, frame);
        if (ret == HI_SUCCESS) {
            win_buf->buf_state.suc_acquire++;
        }
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
    } else {
        WIN_ERROR("unsupport window buff mode %d!\n", win_buf->buff_mode);
    }

    return ret;
}

hi_s32 win_buffer_release_frame(hi_handle buf_handle, hi_drv_video_frame *frame)
{
    hi_drv_vdp_frame *target_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        WIN_ERROR("cannot get window buffer!\n");
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buf->buf_state.try_release++;
    target_node = buff_get_special_frame_node_and_delete(&win_buf->frame_info_disp_list, frame);
    if (target_node == HI_NULL) {
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        WIN_ERROR("node is NULL in display list!\n");
        return HI_FAILURE;
    }
    win_buf->buf_state.suc_release++;
    osal_list_add_tail(&(target_node->node), &(win_buf->frame_info_release_list));
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_acquire_lastest_frame(hi_handle buf_handle, hi_drv_video_frame **frame)
{
    win_buffer *win_buf;
    hi_s32 ret = HI_FAILURE;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
    if (win_buf->buff_mode == WIN_BUFFER_MODE_ONLINE) {
        osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
        win_buf->buf_state.try_acquire++;

        if (win_buf->reset_flag == HI_TRUE) {
            win_buf->reset_flag = HI_FALSE;
            WIN_INFO("black frame reset!\n");
            *frame = HI_NULL;
            return HI_FAILURE;
        }
        ret = acquire_online_frame_when_quick(win_buf, frame);
        if (ret == HI_SUCCESS) {
            win_buf->buf_state.suc_acquire++;
        }
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
    } else {
        WIN_ERROR("unsupport window buff mode %d!\n", win_buf->buff_mode);
    }

    return ret;
}

hi_s32 win_buffer_acquire_captured_frame(hi_handle buf_handle, hi_drv_video_frame **capture_frame)
{
    hi_drv_vdp_frame *frame_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buf->buf_state.try_acquire_capture++;
    frame_node = buff_get_last_frame_node_only(&win_buf->frame_info_disp_list);
    if (frame_node == HI_NULL) {
        WIN_ERROR("cannot capture frame!\n");
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }

    *capture_frame = &frame_node->video_frame;
    frame_node->captured_flag = HI_TRUE;
    win_buf->buf_state.suc_acquire_capture++;
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_release_captured_frame(hi_handle buf_handle, hi_drv_video_frame *capture_frame)
{
    hi_drv_vdp_frame *target_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buf->buf_state.try_release_capture++;
    target_node = buff_get_special_frame_node(&win_buf->frame_info_disp_list, capture_frame);
    if (target_node == HI_NULL) {
        target_node = buff_get_special_frame_node(&win_buf->frame_info_release_list, capture_frame);
        if ((target_node == HI_NULL) || (target_node->captured_flag != HI_TRUE)) {
            WIN_ERROR("cannot release frame!\n");
            osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
            return HI_FAILURE;
        }
    }
    target_node->captured_flag = HI_FALSE;
    win_buf->buf_state.suc_release_capture++;
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 win_buffer_acquire_metadata(hi_handle buf_handle, hi_drv_win_vpss_meta_info *meta_data, hi_u64 *meta_addr)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    win_meta_buffer *meta_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer");
        *meta_addr = INVALID_META_DATA_ADDR;
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buf->buf_state.try_que_meta_info++;
    osal_list_for_each_safe(pos, n, &(win_buf->meta_info_empty_list))
    {
        meta_node = osal_list_entry(pos, win_meta_buffer, node);
        list_entry_times++;
        if (list_entry_times > win_buf->buf_num) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }
        osal_list_del_init(pos);
        if (meta_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            continue;
        }
        ret = HI_SUCCESS;
        break;
    }

    if (ret != HI_SUCCESS) {
        WIN_ERROR("cannot get metadata in empty list!\n");
        *meta_addr = INVALID_META_DATA_ADDR;
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    win_buf->buf_state.suc_que_meta_info++;
    /* fill in meta info */
    memcpy(&meta_node->vdp_meta_info, meta_data, sizeof(hi_drv_win_vpss_meta_info));
    osal_list_add_tail(&(meta_node->node), &(win_buf->meta_info_full_list));
    *meta_addr = (hi_u64)&meta_node->vdp_meta_info;
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
    return HI_SUCCESS;
}

hi_s32 win_buffer_release_metadata(hi_handle buf_handle, hi_u64 metadata_addr)
{
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    win_meta_buffer *meta_node = HI_NULL;
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return HI_FAILURE;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buf->buf_state.try_de_que_meta_info++;
    osal_list_for_each_safe(pos, n, &win_buf->meta_info_full_list)
    {
        meta_node = osal_list_entry(pos, win_meta_buffer, node);
        list_entry_times++;
        if (list_entry_times > win_buf->buf_num) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (metadata_addr == (hi_u64)&meta_node->vdp_meta_info) {
            osal_list_del_init(pos);
            memset(&meta_node->vdp_meta_data, 0, WIN_BUFFER_METADATA_SIZE);
            memset(&meta_node->vdp_meta_info, 0, sizeof(hi_drv_win_vpss_meta_info));
            ret = HI_SUCCESS;
            break;
        }
    }
    if (ret != HI_SUCCESS) {
        WIN_ERROR("rls mentadta err:%llx\n", metadata_addr);
        osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);
        return ret;
    }
    win_buf->buf_state.suc_de_que_meta_info++;
    osal_list_add_tail(&(meta_node->node), &(win_buf->meta_info_empty_list));
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return ret;
}

hi_void win_buffer_get_full_frame_num(vdp_list *frame_list, hi_u32 *frame_num, hi_u32 *frame_play_cnt)
{
    hi_u32 i = 0;
    vdp_list *pos = HI_NULL;
    vdp_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_SUCCESS;
    hi_drv_vdp_frame *temp_node = HI_NULL;
    hi_drv_win_vpss_meta_info *metadata_info = HI_NULL;

    *frame_num = 0;
    *frame_play_cnt = 0;

    osal_list_for_each_safe(pos, n, frame_list)
    {
        temp_node = osal_list_entry(pos, hi_drv_vdp_frame, node);
        list_entry_times++;
        if (list_entry_times > WIN_BUFFER_NODE_MAX) {
            WIN_ERROR("list error\n");
            ret = HI_FAILURE;
            break;
        }

        if (temp_node == HI_NULL) {
            WIN_ERROR("node is NULL\n");
            ret = HI_FAILURE;
            break;
        }

        i++;

        metadata_info = (hi_drv_win_vpss_meta_info *)temp_node->video_frame.video_private.win_private.data;
        *frame_play_cnt = *frame_play_cnt + metadata_info->play_cnt;
        *frame_num = i;
    }
    return;
}


hi_void win_buffer_get_full_frame_info(hi_handle buf_handle, hi_u32 *frame_num, hi_u32 *frame_play_cnt)
{
    win_buffer *win_buf = HI_NULL;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer!\n");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    win_buffer_get_full_frame_num(&win_buf->frame_info_full_list, frame_num, frame_play_cnt);
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return;
}

hi_u32 win_buffer_get_underload_cnt(hi_handle buf_handle)
{
    win_buffer *win_buf = HI_NULL;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer for underload!\n");
        return 0;
    }

    return win_buf->buf_state.under_load;
}

hi_void win_buffer_proc_cnt_info(hi_handle buf_handle, struct seq_file *p)
{
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    HI_PROC_PRINT(p, "------------------------------------frame buffer count info---------------------------------\n");

    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "queue(try/OK)", win_buf->buf_state.try_queue,
        win_buf->buf_state.suc_queue);
    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "de_queue(try/OK)", win_buf->buf_state.try_de_queue,
        win_buf->buf_state.suc_dequeue);

    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "acquire(try/OK))", win_buf->buf_state.try_acquire,
        win_buf->buf_state.suc_acquire);

    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "release(try/OK)", win_buf->buf_state.try_release,
        win_buf->buf_state.suc_release);


    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "capture(try/OK))", win_buf->buf_state.try_acquire_capture,
        win_buf->buf_state.suc_acquire_capture);
    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "release(try/OK)", win_buf->buf_state.try_release_capture,
        win_buf->buf_state.suc_release_capture);

    HI_PROC_PRINT(p, "%-40s:%10d\n", "under_load", win_buf->buf_state.under_load);

    HI_PROC_PRINT(p, "------------------------------------meta buffer count info-----------------------------\n");

    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "que(try/OK)", win_buf->buf_state.try_que_meta_info,
        win_buf->buf_state.suc_que_meta_info);
    HI_PROC_PRINT(p, "%-40s:%-10d/%-10d\n", "deque(try/OK)", win_buf->buf_state.try_de_que_meta_info,
        win_buf->buf_state.suc_de_que_meta_info);
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return;
}

hi_void win_buffer_proc_node_info(hi_handle buf_handle, struct seq_file *p)
{
    win_buffer *win_buf;
    hi_ulong flags;

    osal_spin_lock_irqsave(&g_win_buffer_spin_lock, &flags);
    win_buf = win_buffer_get_by_handle(buf_handle);
    if (win_buf == HI_NULL) {
        WIN_ERROR("cannot get window buffer");
        osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);
        return;
    }
    osal_spin_unlock_irqrestore(&g_win_buffer_spin_lock, &flags);

    osal_spin_lock_irqsave(&win_buf->win_buffer_spin_lock, &flags);
    HI_PROC_PRINT(p, "------------------------------------frame node info---------------------------------\n");

    HI_PROC_PRINT(p, "%-40s:", "in frame queue     [index, addr]");
    buff_print_frame_buffer_list_info(p, &win_buf->frame_info_full_list);

    HI_PROC_PRINT(p, "%-40s:", "disp frame queue   [index, addr]");
    buff_print_frame_buffer_list_info(p, &win_buf->frame_info_disp_list);

    HI_PROC_PRINT(p, "%-40s:", "release frame queue[index, addr]");
    buff_print_frame_buffer_list_info(p, &win_buf->frame_info_release_list);

    HI_PROC_PRINT(p, "%-40s:", "empty frame queue  [index, addr]");
    buff_print_frame_buffer_list_info(p, &win_buf->frame_info_empty_list);

    HI_PROC_PRINT(p, "------------------------------------meta buffer node info-----------------------------\n");

    HI_PROC_PRINT(p, "%-40s:", "full meta queue [addr]");
    buff_print_meta_buffer_list_info(p, &win_buf->meta_info_full_list);

    HI_PROC_PRINT(p, "%-40s:", "empty meta queue[addr]");
    buff_print_meta_buffer_list_info(p, &win_buf->meta_info_empty_list);
    osal_spin_unlock_irqrestore(&win_buf->win_buffer_spin_lock, &flags);

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

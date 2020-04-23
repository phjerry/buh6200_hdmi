/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi uf
 * Author: sdk
 * Create: 2019-12-14
 */

#include "vi_comm.h"
#include "vi_type.h"
#include "vi_buf.h"
#include "vi_uf.h"

#define UF_TIMEOUT 1000
#define UF_DEPTH 200

typedef struct {
    vi_uf_attr uf_attr;    /* UF init attr */
    osal_spinlock uf_lock; /* defence Interrupt by ISR */
    osal_wait uf_wait;     /* blocking queue when user gets continuously */

    hi_bool is_sequence;       /* is save sequence frame */
    hi_u32 uf_cnt;             /* user used buffer count */
    hi_u32 cur_cnt;            /* current valid frame in  vi_frame_list[] */
    vi_buf_node *vi_node_list; /* frame info array */
} uf_info;

hi_s32 uf_check_init_attr(vi_uf_attr *uf_attr_p)
{
    if (uf_attr_p == HI_NULL) {
        vi_drv_log_err("uf_attr_p is null point!\n");
        return HI_FAILURE;
    }

    if (uf_attr_p->buf_handle_p == HI_NULL) {
        vi_drv_log_err("buf_handle_p is null point!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vi_uf_init(vi_uf_attr *uf_attr_p, hi_void **uf_handle_pp)
{
    hi_s32 ret;
    uf_info *uf_info_p = HI_NULL;

    ret = uf_check_init_attr(uf_attr_p);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    uf_info_p = (uf_info *)osal_vmalloc(HI_ID_VI, sizeof(uf_info));
    if (uf_info_p == HI_NULL) {
        vi_drv_log_err("null point\n");
        return HI_FAILURE;
    }

    memset_s(uf_info_p, sizeof(uf_info), 0, sizeof(uf_info));
    memcpy_s(&uf_info_p->uf_attr, sizeof(vi_uf_attr), uf_attr_p, sizeof(vi_uf_attr));

    osal_spin_lock_init(&uf_info_p->uf_lock);
    osal_wait_init(&uf_info_p->uf_wait);

    if (uf_info_p->uf_attr.depth > 0) {
        vi_uf_prepare(uf_info_p, &uf_info_p->uf_attr.depth);
    }

    *uf_handle_pp = uf_info_p;

    return HI_SUCCESS;
}

hi_s32 vi_uf_deinit(hi_void *uf_handle_p)
{
    uf_info *uf_info_p = (uf_info *)uf_handle_p;

    if (uf_info_p == HI_NULL) {
        vi_drv_log_err("null point\n");
        return HI_FAILURE;
    }

    if (uf_info_p->uf_attr.depth > 0) {
        vi_uf_unprepare(uf_handle_p);
    }

    osal_wait_destroy(&uf_info_p->uf_wait);
    osal_spin_lock_destory(&uf_info_p->uf_lock);

    osal_vfree(HI_ID_VI, uf_handle_p);

    return HI_SUCCESS;
}

hi_s32 vi_uf_sendfrm(hi_void *uf_handle_p, vi_buf_node *vi_node_p)
{
    uf_info *uf_info_p = (uf_info *)uf_handle_p;
    unsigned long flags = 0;
    hi_drv_video_frame *src_frame_p = HI_NULL;
    vi_buf_node *dst_node_p = HI_NULL;

    osal_spin_lock_irqsave(&uf_info_p->uf_lock, &flags);

    if ((uf_info_p->cur_cnt > 0) && (uf_info_p->uf_cnt < UF_DEPTH)) {
        src_frame_p = &vi_node_p->frame_info;

        if (!(src_frame_p->field_mode == HI_DRV_FIELD_TOP) && (uf_info_p->vi_node_list != HI_NULL)) {
            dst_node_p = &uf_info_p->vi_node_list[uf_info_p->uf_cnt];

            vi_drv_log_notice("cur(%d)ufcnt(%d)index(%d)\n", uf_info_p->cur_cnt, uf_info_p->uf_cnt,
                              src_frame_p->frame_index);

            memcpy_s(dst_node_p, sizeof(vi_buf_node), vi_node_p, sizeof(vi_buf_node));

            vi_buf_add(uf_info_p->uf_attr.buf_handle_p, vi_node_p);

            uf_info_p->cur_cnt--;
            uf_info_p->uf_cnt++;
        }
    }

    osal_spin_unlock_irqrestore(&uf_info_p->uf_lock, &flags);

    osal_wait_wakeup(&uf_info_p->uf_wait);
    return HI_SUCCESS;
}

hi_s32 vi_uf_prepare(hi_void *uf_handle_p, hi_u32 *save_cnt)
{
    unsigned long flags = 0;
    uf_info *uf_info_p = (uf_info *)uf_handle_p;
    hi_u32 free_buf_cnt;
    hi_u32 frame_list_size = sizeof(vi_buf_node) * UF_DEPTH;

    if (uf_handle_p == HI_NULL || save_cnt == HI_NULL) {
        return HI_FAILURE;
    }

    if (uf_info_p->uf_cnt != 0) {
        vi_drv_log_notice("already save cnt(%d), please acquire it first!!!\n", uf_info_p->uf_cnt);
        *save_cnt = uf_info_p->uf_cnt;
        return HI_FAILURE;
    }

    uf_info_p->vi_node_list = (vi_buf_node *)osal_vmalloc(HI_ID_VI, frame_list_size);
    if (uf_info_p->vi_node_list == HI_NULL) {
        vi_drv_log_err("osal_vmalloc fail\n");
        return HI_FAILURE;
    }
    memset_s(uf_info_p->vi_node_list, frame_list_size, 0, frame_list_size);

    free_buf_cnt = vi_buf_get_idle_cnt(uf_info_p->uf_attr.buf_handle_p);

    osal_spin_lock_irqsave(&uf_info_p->uf_lock, &flags);

    if (*save_cnt >= UF_DEPTH) {
        vi_drv_log_notice("save_cnt(%d) over depth, only save(%d) frame\n", *save_cnt, UF_DEPTH);
        *save_cnt = UF_DEPTH;
    }

    if (free_buf_cnt >= *save_cnt + 2) { /* need remain 2 free buffer */
        uf_info_p->is_sequence = HI_TRUE;
        uf_info_p->cur_cnt = *save_cnt;
    } else {
        uf_info_p->is_sequence = HI_FALSE;
        uf_info_p->cur_cnt = 0;
    }

    uf_info_p->uf_cnt = 0;
    uf_info_p->uf_attr.depth = 0;

    osal_spin_unlock_irqrestore(&uf_info_p->uf_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 vi_uf_unprepare(hi_void *uf_handle_p)
{
    hi_u32 i;
    unsigned long flags = 0;
    hi_void *buf_handle_p = HI_NULL;
    uf_info *uf_info_p = (uf_info *)uf_handle_p;
    vi_buf_node *src_node_p = HI_NULL;

    if (uf_info_p == HI_NULL || uf_info_p->vi_node_list == HI_NULL) {
        vi_drv_log_err("pointer is null\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&uf_info_p->uf_lock, &flags);

    buf_handle_p = uf_info_p->uf_attr.buf_handle_p;

    for (i = 0; i < UF_DEPTH; i++) {
        src_node_p = &uf_info_p->vi_node_list[i];
        if (src_node_p->frame_info.buf_addr[0].start_addr != 0) {
            vi_buf_sub(uf_info_p->uf_attr.buf_handle_p, src_node_p);
            memset_s(src_node_p, sizeof(vi_buf_node), 0, sizeof(vi_buf_node));
        }
    }

    uf_info_p->uf_cnt = 0;
    uf_info_p->cur_cnt = 0;
    uf_info_p->is_sequence = HI_FALSE;

    osal_spin_unlock_irqrestore(&uf_info_p->uf_lock, &flags);

    osal_vfree(HI_ID_VI, uf_info_p->vi_node_list);
    uf_info_p->vi_node_list = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 vi_uf_acquire_frm(hi_void *uf_handle_p, hi_u32 index, vi_buf_node *vi_node_p)
{
    hi_s32 ret;
    unsigned long flags = 0;
    uf_info *uf_info_p = (uf_info *)uf_handle_p;
    vi_buf_node *src_node_p = HI_NULL;

    if (uf_info_p == HI_NULL || uf_info_p->vi_node_list == HI_NULL || vi_node_p == HI_NULL) {
        vi_drv_log_err("pointer is null\n");
        return HI_FAILURE;
    }

    if (index >= UF_DEPTH) {
        vi_drv_log_err("index error\n");
        return HI_FAILURE;
    }

    if (!uf_info_p->is_sequence) {
        uf_info_p->cur_cnt = 1;
    }

    while (uf_info_p->cur_cnt > 0) {
        vi_drv_log_info("cur_cnt(%d), uf_cnt(%d)\n", uf_info_p->cur_cnt, uf_info_p->uf_cnt);
        ret = osal_wait_timeout_uninterruptible(&uf_info_p->uf_wait, HI_NULL, HI_NULL, UF_TIMEOUT);
        if (ret <= 0) {
            return HI_FAILURE;
        }
    }

    osal_spin_lock_irqsave(&uf_info_p->uf_lock, &flags);

    src_node_p = &uf_info_p->vi_node_list[index];

    vi_drv_log_notice("ufcnt(%d)user(%d)index(%d)\n", uf_info_p->uf_cnt, index, src_node_p->frame_info.frame_index);

    memcpy_s(vi_node_p, sizeof(vi_buf_node), src_node_p, sizeof(vi_buf_node));

    osal_spin_unlock_irqrestore(&uf_info_p->uf_lock, &flags);

    return HI_SUCCESS;
}

hi_s32 vi_uf_releasefrm(hi_void *uf_handle_p, vi_buf_node *vi_node_p)
{
    hi_u32 i;
    hi_void *buf_handle_p = HI_NULL;
    unsigned long flags = 0;
    uf_info *uf_info_p = (uf_info *)uf_handle_p;
    vi_buf_node *src_node_p = HI_NULL;

    if (uf_info_p == HI_NULL || uf_info_p->vi_node_list == HI_NULL || vi_node_p == HI_NULL) {
        vi_drv_log_err("pointer is null\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&uf_info_p->uf_lock, &flags);

    buf_handle_p = uf_info_p->uf_attr.buf_handle_p;

    for (i = 0; i < UF_DEPTH; i++) {
        src_node_p = &uf_info_p->vi_node_list[i];
        if (src_node_p->frame_info.frame_index == vi_node_p->frame_info.frame_index) {
            vi_buf_sub(uf_info_p->uf_attr.buf_handle_p, src_node_p);
            memset_s(src_node_p, sizeof(vi_buf_node), 0, sizeof(vi_buf_node));
            break;
        }
    }

    osal_spin_unlock_irqrestore(&uf_info_p->uf_lock, &flags);
    return HI_SUCCESS;
}

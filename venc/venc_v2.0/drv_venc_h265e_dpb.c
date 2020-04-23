/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#include "drv_venc_efl.h"
#include "public.h"
#include "drv_venc_h265e_dpb.h"

#define HI_H265E_DPB_ASSERT_RETURN(condition)\
do { \
    if (!(condition)) { \
        HI_ERR_VENC("assert warning\n"); \
        return HI_FAILURE;\
    } \
} while (0)

#define HI_H265E_DPB_ASSERT(condition)                  \
do {                                                \
    if (!(condition)) {                               \
        HI_ERR_VENC("assert warning\n");            \
        return;                                     \
    }                                               \
} while (0)

hi_s32 h265e_get_rcn_pic_buf(vedu_efl_enc_para *enc_para, hi_u32 *pic_id, hi_u32 *pic_addr)
{
    hi_s32 i;
    pic_pool_buffer *pic = HI_NULL;

    HI_H265E_DPB_ASSERT_RETURN(enc_para != HI_NULL && pic_id != HI_NULL && pic_addr != HI_NULL);

    pic = enc_para->pic_pool;

    for (i = 0; i < enc_para->pic_num; i++) {
        if (HI_TRUE != pic[i].used) {
            *pic_id = pic[i].pic_id;
            *pic_addr = pic[i].pic_addr;
            pic[i].used = HI_TRUE;

            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 h265e_get_rcn_pic_info_buf(vedu_efl_enc_para *enc_para, hi_u32 *pic_info_id, hi_u32 *pic_info_addr)
{
    hi_s32 i;

    pic_info_pool_buffer *pic_info = HI_NULL;

    HI_H265E_DPB_ASSERT_RETURN(enc_para != HI_NULL && pic_info_id != HI_NULL && pic_info_addr != HI_NULL);

    pic_info = enc_para->pic_info_pool;

    for (i = 0; i < enc_para->pic_info_num; i++) {
        if (pic_info[i].used != HI_TRUE) {
            *pic_info_id = pic_info[i].pic_info_id;
            *pic_info_addr = pic_info[i].pic_info_addr;
            pic_info[i].used = HI_TRUE;

            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 h265e_empty_pool(vedu_efl_enc_para *enc_para)
{
    hi_s32 i;
    pic_pool_buffer *pic = HI_NULL;
    pic_info_pool_buffer *pic_info = HI_NULL;

    HI_H265E_DPB_ASSERT_RETURN(enc_para != HI_NULL);

    pic = enc_para->pic_pool;
    pic_info = enc_para->pic_info_pool;

    for (i = 0; i < enc_para->pic_num; i++) {
        pic[i].used = HI_FALSE;
    }

    for (i = 0; i < enc_para->pic_info_num; i++) {
        pic_info[i].used = HI_FALSE;
    }

    return HI_SUCCESS;
}

// (1) 为dpb中所有帧分配资源, 每个节点对应一段物理连续内存;
// (2) 清空dpb队列和参考帧队列
hi_s32 h265e_dpb_init(h265e_dpb *dpb, hi_s32 max_frame_num)
{
    hi_s32 i, ret;

    HI_H265E_DPB_ASSERT_RETURN(dpb != HI_NULL && max_frame_num <= H265E_MAX_FRM_SIZE);

    for (i = 0; i < H265E_MAX_FRM_SIZE; i++) {
        dpb->fs[i].reference = 0;
        dpb->fs[i].ref_long = 0;
        dpb->fs[i].id = i;
        dpb->fs[i].used = HI_FALSE;
    }

    dpb->max_frame_num = max_frame_num;
    dpb->used_frame_num = 0;
    dpb->max_used_frame = 0;

    ret = h265e_dpb_flush(dpb);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h265e_dpb_flush fail !\n");
        return ret;
    }
    return HI_SUCCESS;
}

/* 清除dpb中所有的参考帧 */
hi_s32 h265e_dpb_flush(h265e_dpb *dpb)
{
    hi_s32 i;

    HI_H265E_DPB_ASSERT_RETURN(dpb != HI_NULL);

    /* 置空参考列表 */
    for (i = 0; i < dpb->max_frame_num; i++) {
        dpb->fs[i].reference = 0;
        dpb->fs[i].ref_num = 0;
        dpb->fs[i].ref_long = 0;

        if (dpb->fs[i].used != HI_FALSE) {
            dpb->fs[i].used = HI_FALSE;
            dpb->used_frame_num--;
        }
    }
    dpb->ref_frame[0] = dpb->ref_frame[1] = HI_NULL;

    return HI_SUCCESS;
}

static hi_void init_new_frame_info(h265e_dpb *dpb, vedu_efl_enc_para *enc_para, hi_u32 pic_id, hi_u32 pic_info_id,
                                   hi_s32 i)
{
    dpb->fs[i].pic_pool_id = pic_id;
    dpb->fs[i].pic_info_pool_id = pic_info_id;
    dpb->fs[i].used = HI_TRUE;

    dpb->fs[i].width = enc_para->rc.pic_width;
    dpb->fs[i].height = enc_para->rc.pic_height;

    dpb->fs[i].y_stride = D_VENC_ALIGN_UP(enc_para->image.width, 64); /* 64: align */
    dpb->fs[i].c_stride = dpb->fs[i].y_stride;

    return;
}

static hi_s32 get_rcn_buffer_info(h265e_dpb *dpb, vedu_efl_enc_para *enc_para, hi_u32 pic_info_addr, hi_u32 pic_addr,
                                  hi_s32 i)
{
    /* 获取picinfo */
    dpb->fs[i].tmv_addr = pic_info_addr;
    dpb->fs[i].pme_info_addr = dpb->fs[i].tmv_addr + enc_para->tmv_size;
    dpb->fs[i].pme_addr = dpb->fs[i].pme_info_addr + enc_para->pme_info_size;

    /* 获取pic */
    dpb->fs[i].phy_addr[0] = pic_addr;
    dpb->fs[i].phy_addr[1] = dpb->fs[i].phy_addr[0] + enc_para->luma_size;

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        dpb->fs[i].phy_addr[0] = HI_NULL;
        dpb->fs[i].phy_addr[1] = HI_NULL;
    }

    /* 获取重构帧 */
    dpb->rcn_frame = &dpb->fs[i];
    dpb->rcn_frame->ref_num = enc_para->h265e_slc_hdr.num_active_ref_pics;
    dpb->rcn_frame->ref_info[0].poc = 0;
    dpb->rcn_frame->ref_info[1].poc = 0;

    if (enc_para->rc.pic_type != PIC_INTRA) {
        if (dpb->rcn_frame->ref_num < 1 || dpb->rcn_frame->ref_num > 2) { /* 2: max ref num */
            HI_ERR_VENC("ref_num %d is out of range !\n", dpb->rcn_frame->ref_num);
            return HI_FAILURE;
        }

        /* 记录下当前帧的参考帧信息，编码后面的帧需要用到 */
        dpb->rcn_frame->ref_info[0].poc = dpb->ref_frame[0]->poc;
        dpb->rcn_frame->ref_info[0].is_ref = dpb->ref_frame[0]->reference;
        dpb->rcn_frame->ref_info[0].ref_long = dpb->ref_frame[0]->ref_long;

        if (dpb->rcn_frame->ref_num > 1) {
            dpb->rcn_frame->ref_info[1].poc = dpb->ref_frame[1]->poc;
            dpb->rcn_frame->ref_info[1].is_ref = dpb->ref_frame[1]->reference;
            dpb->rcn_frame->ref_info[1].ref_long = dpb->ref_frame[1]->ref_long;
        }
    }

    return HI_SUCCESS;
}

hi_s32 h265e_dpb_get_new_frame(h265e_dpb *dpb, vedu_efl_enc_para *enc_para)
{
    hi_s32 i;
    hi_s32 ret;
    hi_u32 pic_addr = 0;
    hi_u32 pic_info_addr = 0;
    hi_u32 pic_id = 0;
    hi_u32 pic_info_id = 0;

    HI_H265E_DPB_ASSERT_RETURN(dpb != HI_NULL && enc_para != HI_NULL);

    ret = h265e_get_rcn_pic_buf(enc_para, &pic_id, &pic_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h265e_get_rcn_pic_buf fail !\n");
        return ret;
    }

    ret = h265e_get_rcn_pic_info_buf(enc_para, &pic_info_id, &pic_info_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h265e_get_rcn_pic_info_buf fail !\n");
        return ret;
    }

    dpb->used_frame_num++;
    if (dpb->max_used_frame < dpb->used_frame_num) {
        dpb->max_used_frame = dpb->used_frame_num;
    }

    for (i = 0; i < dpb->max_frame_num; i++) {
        if ((dpb->fs[i].reference == 0) && (dpb->fs[i].used == HI_FALSE)) {
            /* 初始化帧信息 */
            init_new_frame_info(dpb, enc_para, pic_id, pic_info_id, i);

            ret = get_rcn_buffer_info(dpb, enc_para, pic_info_addr, pic_addr, i);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("get_rcn_buffer_info fail !\n");
                return ret;
            }

            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 h265e_dpb_deinit(h265e_dpb *dpb)
{
    return HI_SUCCESS;
}

static hi_void mark_short_ref_frame(h265e_dpb *dpb, const h265e_ref_list *ref_list, hi_s32 *list,
    hi_s32 *number, hi_s32 i)
{
    hi_s32 k, poc;
    poc = ref_list->poc_short[i];

    for (k = 0; k < dpb->max_frame_num; k++) {
        if (dpb->fs[k].reference == HI_TRUE &&
            poc == dpb->fs[k].poc) {
            dpb->fs[k].ref_long = 0;
            dpb->fs[k].used = HI_TRUE;

            if (ref_list->short_term_used[i]) {
                list[(*number)++] = dpb->fs[k].id;
            }
            dpb->used_frame_num++;
        }
    }
}

static hi_void mark_short_ref_list(h265e_dpb *dpb, const h265e_ref_list *ref_list, hi_s32 *list,
    hi_s32 *number)
{
    hi_s32 i;

    for (i = 0; i < ref_list->num_negative_pics; i++) {
        mark_short_ref_frame(dpb, ref_list, list, number, i);
    }

    return;
}

static hi_void mark_long_ref_frame(h265e_dpb *dpb, const h265e_ref_list *ref_list, hi_s32 *list,
    hi_s32 *number, hi_s32 i)
{
    hi_s32 k, poc;

    poc = ref_list->poc_long[i];
    for (k = 0; k < dpb->max_frame_num; k++) {
        if (dpb->fs[k].reference == HI_TRUE &&
            poc == dpb->fs[k].poc) {
            dpb->fs[k].ref_long = 1;
            dpb->fs[k].used = HI_TRUE;
            if (ref_list->long_term_used[i]) {
                list[(*number)++] = dpb->fs[k].id;
            }
        }
    }
}

static hi_void mark_long_ref_list(h265e_dpb *dpb, const h265e_ref_list *ref_list, hi_s32 *list,
    hi_s32 *number)
{
    hi_s32 i;

    for (i = 0; i < ref_list->num_long_term_pics; i++) {
        mark_long_ref_frame(dpb, ref_list, list, number, i);
    }

    return;
}

static hi_void mark_backward_ref_frame(h265e_dpb *dpb, const h265e_ref_list *ref_list, hi_s32 *list,
    hi_s32 *number, hi_s32 i)
{
    hi_s32 k, poc;

    poc = ref_list->poc_short_s1[i];
    for (k = 0; k < dpb->max_frame_num; k++) {
        if (dpb->fs[k].reference == HI_TRUE &&
            poc == dpb->fs[k].poc) {
            dpb->fs[k].ref_long = 0;
            dpb->fs[k].used = HI_TRUE;

            if (ref_list->short_term_used_s1[i]) {
                list[(*number)++] = dpb->fs[k].id;
            }
            dpb->used_frame_num++;
        }
    }
}

static hi_void mark_backward_ref_list(h265e_dpb *dpb, const h265e_ref_list *ref_list, hi_s32 *list,
    hi_s32 *number)
{
    hi_s32 i;

    for (i = 0; i < ref_list->num_positive_pics; i++) {
        mark_backward_ref_frame(dpb, ref_list, list, number, i);
    }

    return;
}

hi_s32 h265e_dpb_generate_ref_list(h265e_dpb *dpb, vedu_efl_h265e_slc_hdr *slice, vedu_efl_enc_para *enc_para)
{
    hi_s32 list[2] = {0}; /* 2: max ref num */
    hi_s32 number = 0;
    const h265e_ref_list *ref_list = &slice->ref_list;

    HI_H265E_DPB_ASSERT_RETURN(dpb != HI_NULL && enc_para != HI_NULL && slice != HI_NULL);

    mark_short_ref_list(dpb, ref_list, list, &number);

    mark_long_ref_list(dpb, ref_list, list, &number);

    mark_backward_ref_list(dpb, ref_list, list, &number);

    /* When the number of reference frames is greater than 1, the optimal reference frame and the sub-optimal reference
     * frame need to be selected. In this way, you do not need to modify the reference frame. For the dual-P frame,
     * the optimal reference frame closest to the current frame is used.
     * The latest reference frame is the latest poc value.
     */
    if (number > 1) {
        dpb->ref_frame[0] = &dpb->fs[list[0]];
        dpb->ref_frame[1] = &dpb->fs[list[1]];
    } else {
        dpb->ref_frame[0] = &dpb->fs[list[0]];
        dpb->ref_frame[1] = HI_NULL;
    }

    return HI_SUCCESS;
}

hi_s32 h265e_dpb_insert_frame(h265e_dpb *dpb, hi_s32 poc)
{
    HI_H265E_DPB_ASSERT_RETURN(dpb != HI_NULL);

    dpb->rcn_frame->poc = poc;
    dpb->rcn_frame->reference = 1;
    dpb->rcn_frame = HI_NULL;

    return HI_SUCCESS;
}

static hi_void set_pool_unused_by_id(pic_pool_buffer *pic, pic_info_pool_buffer *pic_info,
                                     h265e_dpb *dpb, vedu_efl_enc_para *enc_para, hi_u32 k)
{
    hi_u32 i;
    for (i = 0; i < enc_para->pic_num; i++) {
        if (enc_para->enable_rcn_ref_share_buf == HI_FALSE) {
            if (dpb->fs[k].pic_pool_id == pic[i].pic_id) {
                pic[i].used = HI_FALSE;
            }
        }
    }

    for (i = 0; i < enc_para->pic_info_num; i++) {
        if (dpb->fs[k].pic_info_pool_id == pic_info[i].pic_info_id) {
            pic_info[i].used = HI_FALSE;
        }
    }
}

hi_s32 h265e_dpb_unmark(vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slice)
{
    h265e_ref_list *ref_list = HI_NULL;
    h265e_dpb *dpb = HI_NULL;
    hi_s32 i, k;
    pic_pool_buffer *pic = HI_NULL;
    pic_info_pool_buffer *pic_info = HI_NULL;

    HI_H265E_DPB_ASSERT_RETURN(enc_para != HI_NULL && slice != HI_NULL);

    ref_list = &slice->ref_list;
    dpb = &enc_para->h265e_dpb;
    pic = enc_para->pic_pool;
    pic_info = enc_para->pic_info_pool;

    for (i = 0; i < ref_list->num_unmark_pics; i++) {
        hi_s32 poc = ref_list->poc_unmark[i];

        for (k = 0; k < dpb->max_frame_num; k++) {
            if (dpb->fs[k].reference == HI_TRUE && poc == dpb->fs[k].poc &&
                dpb->fs[k].used != HI_FALSE) {

                set_pool_unused_by_id(pic, pic_info, dpb, enc_para, k);

                dpb->fs[k].pic_pool_id = HI_INVALID_HANDLE;
                dpb->fs[k].phy_addr[0] = 0;
                dpb->fs[k].reference = 0;
                dpb->fs[k].ref_long = 0;
                dpb->fs[k].ref_num = 0;
                dpb->fs[k].used = HI_FALSE;
                dpb->used_frame_num--;
            }
        }
    }

    return HI_SUCCESS;
}



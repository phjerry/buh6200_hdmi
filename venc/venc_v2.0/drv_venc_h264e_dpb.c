/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#include "drv_venc_efl.h"
#include "public.h"
#include "drv_venc_h264e_dpb.h"

#define HI_H264E_DPB_ASSERT_RETURN(condition)\
do { \
    if (!(condition)) { \
        HI_ERR_VENC("assert warning\n"); \
        return HI_FAILURE;\
    } \
} while (0)
#define HI_H264E_DPB_ASSERT(condition)                  \
do {                                                \
    if (!(condition)) {                               \
        HI_ERR_VENC("assert warning\n");            \
        return;                                     \
    }                                               \
} while (0)

hi_s32 h264e_dpb_init(h264e_dpb *dpb, hi_s32 max_frame_num)
{
    hi_s32 i;

    HI_H264E_DPB_ASSERT_RETURN((dpb != HI_NULL && max_frame_num > 0));

    drv_venc_queue_init(&(dpb->que_frame), dpb->frame, H264E_DPB_MAX_SIZE);

    for (i = 0; i < H264E_DPB_LIST_MAX_SIZE; i++) {
        dpb->past_short_ref_frame_list[i] = HI_NULL;
    }

    for (i = 0; i < H264E_DPB_LIST_MAX_SIZE; i++) {
        dpb->past_ref_pic_list0[i] = HI_NULL;
        dpb->past_ref_pic_list1[i] = HI_NULL;
    }

    dpb->max_dpb_size = max_frame_num - 1;
    dpb->used_size = 0;
    dpb->short_ref_num = 0;
    dpb->long_ref_num = 0;
    dpb->list0_size = 0;
    dpb->list1_size = 0;

    return HI_SUCCESS;
}

hi_s32 h264e_dpb_flush(vedu_efl_enc_para *enc_para)
{
    hi_s32 i, j;
    hi_s32 ret;
    h264e_pic *pic = HI_NULL;
    ULONG rcn_addr = 0;
    pic_pool_buffer *pic_pool = HI_NULL;
    pic_info_pool_buffer *pic_info_pool = HI_NULL;
    h264e_dpb *dpb = HI_NULL;
    hi_s32 used_size;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL));

    pic_pool = enc_para->pic_pool;
    pic_info_pool = enc_para->pic_info_pool;
    dpb = &enc_para->h264_dpb;

    used_size = drv_venc_queue_get_real_len(&(dpb->que_frame));

    for (i = 0; i < used_size; i++) {
        ret = drv_venc_queue_get_from_head(&(dpb->que_frame), &rcn_addr);
        HI_H264E_DPB_ASSERT_RETURN(HI_SUCCESS == ret && rcn_addr != 0);
        pic = (h264e_pic *)(hi_size_t)rcn_addr;
        pic->used = 0;
        pic->ref = REF_NONE;
        pic->frame_num = 0xFFFF;
        pic->frame_num_wrap = 0xFFFF;
        pic->pic_num = 0xFFFF;
        pic->long_term_pic_num = 0xFFFF;

        for (j = 0; j < enc_para->pic_num; j++) {
            if (pic->pool_pic_id == pic_pool[j].pic_id) {
                pic_pool[j].used = HI_FALSE;
            }
        }

        for (j = 0; j < enc_para->pic_info_num; j++) {
            if (pic->pool_pic_info_id == pic_info_pool[j].pic_info_id) {
                pic_info_pool[j].used = HI_FALSE;
            }
        }


    }

    for (j = 0; j < enc_para->pic_num; j++) {
        pic_pool[j].used = HI_FALSE;
    }

    for (j = 0; j < enc_para->pic_info_num; j++) {
        pic_info_pool[j].used = HI_FALSE;
    }

    ret = h264e_dpb_init(dpb, enc_para->rcn_num);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h264e_dpb_init fail\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 memory_management_control_operation(vedu_efl_h264e_slc_hdr *slc_hdr, h264e_dpb *dpb, h264e_pic *frame,
                                                  vedu_efl_enc_para *enc_para)
{
    hi_u32 i = 0;
    while (i < 10 &&  /* 10: max times of operation */
        slc_hdr->mark.memory_management_control_operation[i] != MARKING_OPERATION_END) {
        switch (slc_hdr->mark.memory_management_control_operation[i]) {
            /* 标记短期参考帧为非参考帧 */
            case MARKING_SHORTTERM_TO_NONREF:
                h264e_mm_unmark_short_term_for_reference(dpb, slc_hdr->frame_num, slc_hdr->mark.marking_operation[i]);
                break;
            /* 清空参考帧队列 */
            case MARKING_FLUSH_REF_LIST:
                h264e_dpb_flush(enc_para);
                break;
            /* 标记当前帧为长期参考帧 */
            case MARKING_CURFRAME_TO_LONGTERM:
                HI_H264E_DPB_ASSERT_RETURN(REF_LONG == frame->ref && HI_TRUE == frame->used);
                h264e_mm_mark_current_picture_long_term(dpb, frame, slc_hdr->mark.marking_operation[i]);
                break;
            case MARKING_SET_MAX_LONGETERMIDX:
                h264e_mm_mark_set_max_long_term_idx(dpb, slc_hdr->mark.marking_operation[i]);
                break;
            case MARKING_LONGTERM_TO_NONREF:
                h264e_mm_unmark_long_term_for_reference(dpb, slc_hdr->frame_num, slc_hdr->mark.marking_operation[i]);
                break;

            default:
                HI_H264E_DPB_ASSERT_RETURN(0);
                break;
        }

        i++;
    }

    HI_H264E_DPB_ASSERT_RETURN(i < 10
            && MARKING_OPERATION_END == slc_hdr->mark.memory_management_control_operation[i]);

    return HI_SUCCESS;
}

static hi_void set_pool_unused_by_id(pic_pool_buffer *pic_pool, pic_info_pool_buffer *pic_info_pool,
                                     h264e_pic *frame, vedu_efl_enc_para *enc_para)
{
    hi_u32 i;
    if (enc_para->enable_rcn_ref_share_buf == HI_FALSE) {
        for (i = 0; i < enc_para->pic_num; i++) {
            if (frame->pool_pic_id == pic_pool[i].pic_id) {
                pic_pool[i].used = HI_FALSE;
            }
        }
    }

    for (i = 0; i < enc_para->pic_info_num; i++) {
        if (frame->pool_pic_info_id == pic_info_pool[i].pic_info_id) {
            pic_info_pool[i].used = HI_FALSE;
        }
    }
}

static hi_s32 delete_frame_in_dpb_not_refered(pic_pool_buffer *pic_pool, pic_info_pool_buffer *pic_info_pool,
                                              h264e_dpb *dpb, h264e_pic *frame, vedu_efl_enc_para *enc_para)
{
    hi_u32 i;
    h264e_pic *tmp_frame = HI_NULL;
    ULONG rcn_addr = 0;
    for (i = 0; i < dpb->used_size; i++) {
        drv_venc_queue_get_from_tail(&(dpb->que_frame), &rcn_addr);

        tmp_frame = (h264e_pic *)(hi_size_t)rcn_addr;
        HI_H264E_DPB_ASSERT_RETURN(tmp_frame != HI_NULL);

        if (tmp_frame->used == HI_FALSE && tmp_frame->ref == REF_NONE) {
            set_pool_unused_by_id(pic_pool, pic_info_pool, tmp_frame, enc_para);
        } else {
            drv_venc_queue_put_to_head(&(dpb->que_frame), rcn_addr);
        }
    }
    dpb->used_size = drv_venc_queue_get_real_len(&(dpb->que_frame));

    HI_H264E_DPB_ASSERT_RETURN(dpb->used_size <= dpb->max_dpb_size);

    drv_venc_queue_put_to_head(&(dpb->que_frame), (ULONG)(hi_size_t)frame);

    return HI_SUCCESS;
}

static hi_void fifo_inset_frame_in_dpb(pic_pool_buffer *pic_pool, pic_info_pool_buffer *pic_info_pool, h264e_dpb *dpb,
                                       h264e_pic *frame, vedu_efl_enc_para *enc_para)
{
    h264e_pic *tmp_frame = HI_NULL;
    ULONG rcn_addr = 0;
    hi_bool ref_slide = HI_FALSE;
    hi_u32 i;
    if (dpb->used_size >= dpb->max_dpb_size) {
        for (i = 0; i < dpb->used_size; i++) {
            drv_venc_queue_get_from_tail(&(dpb->que_frame), &rcn_addr);
            tmp_frame = (h264e_pic *)(hi_size_t)rcn_addr;

            if (tmp_frame->used == 1 && tmp_frame->ref == REF_SHORT && !ref_slide) {
                tmp_frame->used = 0;
                tmp_frame->ref = REF_NONE;
                tmp_frame->frame_num = 0xFFFF;
                tmp_frame->frame_num_wrap = 0xFFFF;
                tmp_frame->pic_num = 0xFFFF;
                tmp_frame->long_term_pic_num = 0xFFFF;
                ref_slide = HI_TRUE;

                set_pool_unused_by_id(pic_pool, pic_info_pool, tmp_frame, enc_para);
            } else {
                drv_venc_queue_put_to_head(&(dpb->que_frame), rcn_addr);
            }
        }
    }

    drv_venc_queue_put_to_head (&(dpb->que_frame), (ULONG)(hi_size_t)frame);

    return;
}

hi_s32 h264e_dpb_insert_frame(vedu_efl_h264e_slc_hdr *slc_hdr, h264e_dpb *dpb, h264e_pic *frame,
                              vedu_efl_enc_para *enc_para)
{
    hi_u32 ret;
    pic_pool_buffer *pic_pool = HI_NULL;
    pic_info_pool_buffer *pic_info_pool = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL && frame != HI_NULL && slc_hdr != HI_NULL && dpb != HI_NULL));

    pic_pool = enc_para->pic_pool;
    pic_info_pool = enc_para->pic_info_pool;
    dpb->used_size = drv_venc_queue_get_real_len(&(dpb->que_frame));
    if (slc_hdr->mark.adaptive_ref_pic_marking_mode_flag) {
        ret = memory_management_control_operation(slc_hdr, dpb, frame, enc_para);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("memory_management_control_operation fail \n");
            return ret;
        }

        ret = delete_frame_in_dpb_not_refered(pic_pool, pic_info_pool, dpb, frame, enc_para);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("delete_frame_in_dpb_not_refered\n");
            return ret;
        }
    } else {
        fifo_inset_frame_in_dpb(pic_pool, pic_info_pool, dpb, frame, enc_para);
    }

    return HI_SUCCESS;
}

hi_s32 h264e_update_dpb_frames(vedu_efl_enc_para *enc_para)
{
    h264e_pic *rcn_pic = HI_NULL;
    vedu_efl_h264e_slc_hdr *h264_slc_hdr = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL));

    rcn_pic = enc_para->h264_rcn_pic;
    h264_slc_hdr = &enc_para->h264_slc_hdr;
    rcn_pic->frame_num = h264_slc_hdr->frame_num;
    rcn_pic->poc = enc_para->h264_poc.poc;

    if (h264_slc_hdr->nal_ref_idc == 3 || enc_para->rc.pic_type == PIC_INTRA) { /* 3: current frame to be refered */
        rcn_pic->frame_num = h264_slc_hdr->frame_num;
        rcn_pic->used = 1;
        ret = h264e_dpb_insert_frame(h264_slc_hdr, &enc_para->h264_dpb, rcn_pic, enc_para);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("h264e_dpb_insert_frame fail\n");
        }
    } else {
        rcn_pic->used = 0;
        rcn_pic->ref = REF_NONE;
    }

    return HI_SUCCESS;
}

hi_s32 h264e_update_frame_num(vedu_efl_enc_para *enc_para)
{
    vedu_efl_h264e_slc_hdr *h264_slc_hdr = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL));

    h264_slc_hdr = &enc_para->h264_slc_hdr;

    enc_para->h264_poc.prev_frm_num = enc_para->frame_num;
    enc_para->h264_poc.prev_frm_num_offset = enc_para->h264_poc.frm_num_offset;

    if (h264_slc_hdr->nal_ref_idc == 3 || enc_para->rc.pic_type == PIC_INTRA) { /* 3: current frame to be refered */
        enc_para->frame_num += 1;
    }

    return HI_SUCCESS;
}

hi_s32 h264e_mm_unmark_short_term_for_reference(h264e_dpb *dpb, hi_s32 pic_num, hi_s32 difference_of_pic_nums_minus1)
{
    hi_s32 pic_num_x;
    hi_ulong addr = 0;
    hi_s32 i;
    h264e_pic *temp_frame = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((dpb != HI_NULL));

    pic_num_x = pic_num - (difference_of_pic_nums_minus1 + 1);

    for (i = 0; i < dpb->used_size; i++) {
        drv_venc_queue_search_from_tail(&(dpb->que_frame), &addr, i);

        temp_frame = (h264e_pic *)(hi_size_t)addr;
        HI_H264E_DPB_ASSERT_RETURN(temp_frame != HI_NULL);

        if (temp_frame->used == HI_TRUE && temp_frame->ref == REF_SHORT) {
            if (temp_frame->pic_num == pic_num_x) {
                temp_frame->used = HI_FALSE;
                temp_frame->ref = REF_NONE;
                temp_frame->frame_num = 0xFFFF;
                temp_frame->frame_num_wrap = 0xFFFF;
                temp_frame->long_term_pic_num = 0xFFFF;
                temp_frame->pic_num = 0xFFFF;
                break;
            }
        }
    }
    if (temp_frame != HI_NULL) {
        HI_INFO_VENC("UnMark ShortRef (%d,%d) ref:%d pic_num %d, diff %d, long_term_pic_num:%d \n",
                     pic_num_x, temp_frame->pic_num, temp_frame->ref, pic_num, difference_of_pic_nums_minus1,
                     temp_frame->long_term_pic_num);
    }
    return HI_SUCCESS;
}

hi_s32 h264e_mm_unmark_long_term_for_reference(h264e_dpb *dpb, hi_s32 pic_num, hi_s32 s32LongTermNum)
{
    hi_ulong addr = 0;
    hi_s32 i;
    h264e_pic *temp_frame = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((dpb != HI_NULL));

    for (i = 0; i < dpb->used_size; i++) {
        drv_venc_queue_search_from_tail(&(dpb->que_frame), &addr, i);

        temp_frame = (h264e_pic *)(hi_size_t)addr;
        HI_H264E_DPB_ASSERT_RETURN(temp_frame != HI_NULL);

        if (temp_frame->used == HI_TRUE && temp_frame->ref == REF_LONG) {
            if (temp_frame->long_term_pic_num == s32LongTermNum) {
                temp_frame->used = HI_FALSE;
                temp_frame->ref = REF_NONE;
                temp_frame->frame_num = 0xFFFF;
                temp_frame->frame_num_wrap = 0xFFFF;
                temp_frame->long_term_pic_num = 0xFFFF;
                temp_frame->pic_num = 0xFFFF;
                break;
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 h264e_mm_mark_current_picture_long_term(h264e_dpb *dpb, h264e_pic *frame, hi_s32 long_term_frame_idx)
{
    hi_u32 i = 0;
    hi_ulong addr = 0;
    h264e_pic *temp_frame = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((dpb != HI_NULL && frame != HI_NULL));

    /* unmark has a long-term reference frame with the same long_term_frame_idx. */
    for (i = 0; i < dpb->used_size; i++) {
        drv_venc_queue_search_from_tail(&(dpb->que_frame), &addr, i);

        temp_frame = (h264e_pic *)(hi_size_t)addr;
        HI_H264E_DPB_ASSERT_RETURN(temp_frame != HI_NULL);

        if (temp_frame->used == HI_TRUE && temp_frame->ref == REF_LONG) {
            if (temp_frame->long_term_pic_num == long_term_frame_idx) {
                temp_frame->used = HI_FALSE;
                temp_frame->ref = REF_NONE;
                temp_frame->frame_num = 0xFFFF;
                temp_frame->frame_num_wrap = 0xFFFF;
                temp_frame->long_term_pic_num = 0xFFFF;
                temp_frame->pic_num = 0xFFFF;
                break;
            }
        }
    }

    frame->long_term_pic_num = long_term_frame_idx;
    return HI_SUCCESS;
}

hi_s32 h264e_mm_mark_set_max_long_term_idx(h264e_dpb *dpb, hi_s32 max_long_term_frame_idx_plus1)
{
    hi_u32 i = 0;
    hi_ulong addr = 0;
    h264e_pic *temp_frame = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((dpb != HI_NULL));

    for (i = 0; i < dpb->used_size; i++) {
        drv_venc_queue_search_from_tail(&(dpb->que_frame), &addr, i);

        temp_frame = (h264e_pic *)(hi_size_t)addr;
        HI_H264E_DPB_ASSERT_RETURN(temp_frame != HI_NULL);

        if (temp_frame->used == HI_TRUE && temp_frame->ref == REF_LONG) {
            if (temp_frame->long_term_pic_num > max_long_term_frame_idx_plus1 - 1) {
                temp_frame->used = HI_FALSE;
                temp_frame->ref = REF_NONE;
                temp_frame->frame_num = 0xFFFF;
                temp_frame->frame_num_wrap = 0xFFFF;
                temp_frame->long_term_pic_num = 0xFFFF;
                temp_frame->pic_num = 0xFFFF;
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 h264e_mm_assign_long_term_frame_idx(h264e_dpb *dpb, hi_s32 pic_num, hi_s32 difference_of_pic_nums_minus1,
                                           hi_s32 long_term_frame_idx)
{
    hi_s32 pic_num_x;
    hi_ulong addr = 0;
    hi_s32 i;
    h264e_pic *temp_frame = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((dpb != HI_NULL));

    pic_num_x = pic_num - (difference_of_pic_nums_minus1 + 1);

    for (i = 0; i < dpb->used_size; i++) {
        drv_venc_queue_search_from_tail(&(dpb->que_frame), &addr, i);

        temp_frame = (h264e_pic *)(hi_size_t)addr;
        HI_H264E_DPB_ASSERT_RETURN(temp_frame != HI_NULL);

        if (temp_frame->used == HI_TRUE && temp_frame->ref == REF_SHORT) {
            if (temp_frame->pic_num == pic_num_x) {
                HI_ERR_VENC("UnMark ShortRef (%d,%d) ref:%d pic_num %d, diff %d, long_term_pic_num:%d \n",
                            pic_num_x, temp_frame->pic_num, temp_frame->ref, pic_num, difference_of_pic_nums_minus1,
                            temp_frame->long_term_pic_num);
                temp_frame->ref = REF_LONG;
                temp_frame->frame_num = 0xFFFF;
                temp_frame->frame_num_wrap = 0xFFFF;
                temp_frame->long_term_pic_num = long_term_frame_idx;
                temp_frame->pic_num = 0xFFFF;
                break;
            }
        }
    }
    if (temp_frame != HI_NULL) {
        HI_INFO_VENC("UnMark ShortRef (%d,%d) ref:%d pic_num %d, diff %d, long_term_pic_num:%d \n",
                     pic_num_x, temp_frame->pic_num, temp_frame->ref, pic_num, difference_of_pic_nums_minus1,
                     temp_frame->long_term_pic_num);
    }
    return HI_SUCCESS;
}


hi_s32 h264e_set_rcn_out_pic(vedu_efl_enc_para *enc_para)
{
    pic_pool_buffer *pic = HI_NULL;
    pic_info_pool_buffer *pic_info = HI_NULL;
    h264e_pic *h264_rcn_pic = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL));

    pic = enc_para->pic_pool;
    pic_info = enc_para->pic_info_pool;
    h264_rcn_pic = enc_para->h264_rcn_pic;

    if (enc_para->h264_rcn_pic->ref == REF_NONE || enc_para->rc.recode_flag != 0) {
        set_pool_unused_by_id(pic, pic_info, h264_rcn_pic, enc_para);
    }

    enc_para->h264_rcn_pic = HI_NULL;

    return HI_SUCCESS;
}

static hi_s32 search_ref_frame(vedu_efl_enc_para *enc_para, h264e_dpb *dpb, h264e_pic *frame)
{
    hi_s32 i, used_size;
    hi_s32 max_frame_num;
    ULONG rcn_addr = 0;
    vedu_efl_h264e_slc_hdr *slc_hdr = &(enc_para->h264_slc_hdr);

    used_size = drv_venc_queue_get_real_len(&(dpb->que_frame));
    max_frame_num = (1 << (4 + enc_para->h264_sps.log2_max_frame_num_minus4)); /* 4: fomula for calculate max poc */
    dpb->short_ref_num = 0;
    dpb->long_ref_num = 0;

    for (i = 0; i < used_size; i++) {
        drv_venc_queue_search_from_tail(&(dpb->que_frame), &rcn_addr, i);
        frame = (h264e_pic *)(hi_size_t)rcn_addr;
        HI_H264E_DPB_ASSERT_RETURN(1 == frame->used);

        if (frame->ref == REF_SHORT) {
            if (frame->frame_num > slc_hdr->frame_num) {
                frame->frame_num_wrap = frame->frame_num - max_frame_num;
            } else {
                frame->frame_num_wrap = frame->frame_num;
            }

            frame->pic_num = frame->frame_num_wrap;
            frame->long_term_pic_num = 0xFFFF;
            dpb->past_short_ref_frame_list[dpb->short_ref_num] = frame;
            dpb->short_ref_num++;
        } else if (frame->ref == REF_LONG) {
            frame->pic_num = 0xFFFF;
            dpb->past_long_ref_frame_list[dpb->long_ref_num] = frame;
            dpb->long_ref_num++;
        }
    }

    HI_H264E_DPB_ASSERT_RETURN((dpb->short_ref_num + dpb->long_ref_num) < H264E_DPB_LIST_MAX_SIZE);

    return HI_SUCCESS;
}

static hi_void reorder_ref_frame_list_p(h264e_dpb *dpb, h264e_pic *frame)
{
    hi_s32 i, j;
    /* short ref frame list reorder */
    for (i = 0; i < dpb->short_ref_num - 1; i++) {
        for (j = 0; j < dpb->short_ref_num - i - 1; j++) {
            if (dpb->past_short_ref_frame_list[j]->pic_num < dpb->past_short_ref_frame_list[j + 1]->pic_num) {
                frame = dpb->past_short_ref_frame_list[j];
                dpb->past_short_ref_frame_list[j] = dpb->past_short_ref_frame_list[j + 1];
                dpb->past_short_ref_frame_list[j + 1] = frame;
            }
        }
    }

    /* long ref frame list reorder */
    for (i = 0; i < dpb->long_ref_num - 1; i++) {
        for (j = 0; j < dpb->long_ref_num - i - 1; j++) {
            if (dpb->past_long_ref_frame_list[j]->long_term_pic_num > \
                dpb->past_long_ref_frame_list[j + 1]->long_term_pic_num) {
                frame = dpb->past_long_ref_frame_list[j];
                dpb->past_long_ref_frame_list[j] = dpb->past_long_ref_frame_list[j + 1];
                dpb->past_long_ref_frame_list[j + 1] = frame;
            }
        }
    }
}

hi_s32 h264e_dpb_init_ref_list_p(vedu_efl_enc_para *enc_para)
{
    hi_s32 i, ret;
    h264e_pic *frame = HI_NULL;
    h264e_dpb *dpb = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL));

    dpb = &enc_para->h264_dpb;

    ret = search_ref_frame(enc_para, dpb, frame);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("search_ref_frame fail \n");
        return ret;
    }

    reorder_ref_frame_list_p(dpb, frame);

    /* dpb init */
    for (i = 0; i < dpb->short_ref_num; i++) {
        dpb->past_ref_pic_list0[i] = dpb->past_short_ref_frame_list[i];
        HI_H264E_DPB_ASSERT_RETURN(dpb->past_ref_pic_list0[i] != HI_NULL);
    }

    for (i = 0; i < dpb->long_ref_num; i++) {
        dpb->past_ref_pic_list0[i + dpb->short_ref_num] = dpb->past_long_ref_frame_list[i];
        HI_H264E_DPB_ASSERT_RETURN(dpb->past_ref_pic_list0[i] != HI_NULL);
    }

    dpb->ref_num = dpb->short_ref_num + dpb->long_ref_num;

    dpb->list0_size = dpb->ref_num;
    dpb->list1_size = 0;

    for (i = dpb->ref_num; i < H264E_DPB_LIST_MAX_SIZE; i++) {
        dpb->past_ref_pic_list0[i] = HI_NULL;
    }

    return HI_SUCCESS;
}

static hi_void reorder_ref_frame_list_b(h264e_dpb *dpb, hi_u32 curr_poc)
{
    hi_s32 i, j;
    hi_u32 index_small[H264E_DPB_LIST_MAX_SIZE], index_large[H264E_DPB_LIST_MAX_SIZE], swap;
    hi_s32 poc_small = 0;
    hi_s32 poc_large = 0;

    for (i = 0; i < dpb->short_ref_num; i++) {
        if (dpb->past_short_ref_frame_list[i]->poc < curr_poc) {
            index_small[poc_small++] = i;
        } else if (dpb->past_short_ref_frame_list[i]->poc > curr_poc) {
            index_large[poc_large++] = i;
        }
    }
    for (i = 0; i < poc_small; i++) {
        for (j = 0; j < poc_small - i - 1; j++) {
            if (dpb->past_short_ref_frame_list[index_small[j]]->poc <
		        dpb->past_short_ref_frame_list[index_small[j + 1]]->poc) {
                swap = index_small[j];
                index_small[j] = index_small[j + 1];
                index_small[j + 1] = swap;
            }
        }
    }
    for (i = 0; i < poc_large; i++) {
        for (j = 0; j < poc_large - i - 1; j++) {
            if (dpb->past_short_ref_frame_list[index_large[j]]->poc >
			    dpb->past_short_ref_frame_list[index_large[j + 1]]->poc) {
                swap = index_large[j];
                index_large[j] = index_large[j + 1];
                index_large[j + 1] = swap;
            }
        }
    }
    for (i = 0; i < poc_small; i++) {
        dpb->past_ref_pic_list0[i] = dpb->past_short_ref_frame_list[index_small[i]];
    }
    for (i = 0; i < poc_large; i++) {
        dpb->past_ref_pic_list0[i + poc_small] = dpb->past_short_ref_frame_list[index_large[i]];
    }
    for (i = 0; i < poc_large; i++) {
        dpb->past_ref_pic_list1[i] = dpb->past_short_ref_frame_list[index_large[i]];
    }
    for (i = 0; i < poc_small; i++) {
        dpb->past_ref_pic_list1[i + poc_large] = dpb->past_short_ref_frame_list[index_small[i]];
    }

    return;
}

hi_s32 h264e_dpb_init_ref_list_b(vedu_efl_enc_para *enc_para)
{
    h264e_pic *frame = HI_NULL;
    h264e_dpb *dpb = HI_NULL;
    hi_bool same = HI_TRUE;
    hi_s32 i, ret;
    h264e_ref *ref = HI_NULL;
    hi_u32 curr_poc;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL));

    dpb = &enc_para->h264_dpb;
    ref = &enc_para->h264_ref;

    curr_poc = ref->last_last_p + ref->enhance_cnt + 1;
    ret = search_ref_frame(enc_para, dpb, frame);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("search_ref_frame fail \n");
        return ret;
    }

    /* reoder ref list */
    reorder_ref_frame_list_b(dpb, curr_poc);
    dpb->ref_num = dpb->short_ref_num;
    dpb->list0_size = dpb->ref_num;
    dpb->list1_size = dpb->ref_num;

    /* init reflist 0 && init reflist 1 */
    for (i = dpb->ref_num; i < H264E_DPB_LIST_MAX_SIZE; i++) {
        dpb->past_ref_pic_list0[i] = HI_NULL;
        dpb->past_ref_pic_list1[i] = HI_NULL;
    }

    if (dpb->list0_size == dpb->list1_size && dpb->list1_size > 1) {
        for (i = 0; i < dpb->list0_size; i++) {
            if (dpb->past_ref_pic_list1[i] != dpb->past_ref_pic_list0[i]) {
                same = HI_FALSE;
                break;
            }
        }

        if (same) {
            frame = dpb->past_ref_pic_list1[0];
            dpb->past_ref_pic_list1[0] = dpb->past_ref_pic_list1[1];
            dpb->past_ref_pic_list1[1] = frame;
        }
    }

    return HI_SUCCESS;
}

hi_s32 h264e_dpb_init_ref_list(vedu_efl_enc_para *enc_para, hi_u32 pic_type)
{
    hi_s32 ret = HI_FAILURE;

    HI_H264E_DPB_ASSERT_RETURN(enc_para != HI_NULL && (pic_type >= PIC_INTER && pic_type <= PIC_BIINTER));
    switch (pic_type) {
        case PIC_INTER:
        case PIC_INTERSP:
        case PIC_INTERBP: {
            ret = h264e_dpb_init_ref_list_p(enc_para);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h264e_dpb_init_ref_list_p fail \n");
            }

            break;
        }
        case PIC_BIINTER: {
            ret = h264e_dpb_init_ref_list_b(enc_para);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h264e_dpb_init_ref_list_p fail \n");
            }

            break;
        }
        default:
            HI_ERR_VENC("Unknown pic type!\n");
            break;
    }

    return ret;
}

hi_s32 h264e_get_rcn_pic_buf(vedu_efl_enc_para *enc_para, hi_u32 *pic_id, hi_u32 *pic_addr)
{
    hi_s32 i;
    pic_pool_buffer *pic = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL && pic_id != HI_NULL && pic_addr != HI_NULL));

    pic = enc_para->pic_pool;

    for (i = 0; i < enc_para->pic_num; i++) {
        if (pic[i].used != HI_TRUE) {
            *pic_id = pic[i].pic_id;
            *pic_addr = pic[i].pic_addr;
            pic[i].used = HI_TRUE;

            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 h264e_get_rcn_pic_info_buf(vedu_efl_enc_para *enc_para, hi_u32 *pic_info_id, hi_u32 *pic_info_addr)
{
    hi_s32 i;

    pic_info_pool_buffer *pic_info = HI_NULL;

    HI_H264E_DPB_ASSERT_RETURN((enc_para != HI_NULL && pic_info_id != HI_NULL && pic_info_addr != HI_NULL));

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

hi_s32 h264e_dpb_get_new_frame(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    h264e_pic *tmp_pic = HI_NULL;
    hi_s32 ret;
    hi_s32 i;
    hi_u32 pic_addr = 0;
    hi_u32 pic_info_addr = 0;
    hi_u32 pic_id = 0;
    hi_u32 pic_info_id = 0;

    HI_H264E_DPB_ASSERT_RETURN(enc_para != HI_NULL && enc_in != HI_NULL);

    ret = h264e_get_rcn_pic_buf(enc_para, &pic_id, &pic_addr);
    if (ret != HI_SUCCESS) {

        HI_ERR_VENC("h264e_get_rcn_pic_buf fail !\n");
        return ret;
    }

    ret = h264e_get_rcn_pic_info_buf(enc_para, &pic_info_id, &pic_info_addr);
    if (ret != HI_SUCCESS) {

        HI_ERR_VENC("h264e_get_rcn_pic_info_buf fail !\n");
        return ret;
    }

    for (i = 0; i < H264E_MAX_FRM_SIZE; i++) {
        tmp_pic = &enc_para->h264_pic[i];
        if (tmp_pic->used == 0) {
            enc_para->h264_rcn_pic = tmp_pic;
            break;
        }
    }

    HI_H264E_DPB_ASSERT_RETURN(HI_NULL != enc_para->h264_rcn_pic);

    tmp_pic->pool_pic_id = pic_id;
    tmp_pic->pool_pic_info_id = pic_info_id;
    tmp_pic->width = enc_para->rc.pic_width;
    tmp_pic->height = enc_para->rc.pic_height;
    tmp_pic->y_stride = (enc_para->rc.pic_width + 63) / 64 * 64; /* 63, 64: 64 byte align */
    tmp_pic->c_stride = tmp_pic->y_stride;

    tmp_pic->tmv_addr = pic_info_addr;
    tmp_pic->pme_info_addr = tmp_pic->tmv_addr + enc_para->tmv_size;

    tmp_pic->pme_addr = tmp_pic->pme_info_addr + enc_para->pme_info_size;

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        tmp_pic->phy_addr[0] = HI_NULL;
        tmp_pic->phy_addr[1] = HI_NULL;
    } else {
        tmp_pic->phy_addr[0] = pic_addr;
        tmp_pic->phy_addr[1] = tmp_pic->phy_addr[0] + enc_para->luma_size;
    }

    return HI_SUCCESS;
}


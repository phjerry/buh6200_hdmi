/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#include "drv_venc_h265e_ref.h"
#include "hi_venc_type.h"
#include "drv_venc_efl.h"
#include "public.h"

#define HI_H265E_REF_ASSERT_RETURN(condition)\
do { \
    if (!(condition)) { \
        HI_ERR_VENC("assert warning\n"); \
        return HI_FAILURE;\
    } \
} while (0)

#define HI_H265E_REF_ASSERT(condition)                  \
do {                                                \
    if (!(condition)) {                               \
        HI_ERR_VENC("assert warning\n");            \
        return;                                     \
    }                                               \
} while (0)


hi_s32 h265e_encode_poc(vedu_efl_enc_para *enc_para, hi_u32 pic_type)
{
    h265e_poc *poc = HI_NULL;

    HI_H265E_REF_ASSERT_RETURN(enc_para != HI_NULL);

    poc = &enc_para->h265_poc;

    if (PIC_INTRA == pic_type) {
        poc->pic_order_cnt_lsb = 0;
        poc->pic_order_cnt = 0;
        poc->log2_max_poc_lsb = 16; /* 16: log2_max_poc_lsb */
        poc->max_pic_order_cnt_lsb = 1 << poc->log2_max_poc_lsb;
        return HI_SUCCESS;
    }


    {
        poc->pic_order_cnt++;
        poc->pic_order_cnt_lsb++;
    }

    if (poc->pic_order_cnt_lsb >= poc->max_pic_order_cnt_lsb) {
        poc->pic_order_cnt_lsb = 0;
    }

    return HI_SUCCESS;
}

hi_s32 h265e_ref_init(vedu_efl_enc_para *enc_para, h265e_ref *ref)
{
    HI_H265E_REF_ASSERT_RETURN(enc_para != HI_NULL && ref != HI_NULL);

    ref->poc_last_p = 0;
    ref->b_frm_cnt = enc_para->rc.b_frm_num;
    ref->poc_last_last_p = 0;
    ref->last_idr = 1;

    return HI_SUCCESS;
}


/* 我们这里的长期参考帧都是前向的,若是长期参考帧有后向，则这里需要重新考虑 */
hi_void h265e_get_active_ref_num(vedu_efl_h265e_slc_hdr *slc_hdr)
{
    hi_s32 i = 0;

    slc_hdr->num_active_ref_pics = 0;
    slc_hdr->num_active_ref_pics_s0 = 0;

    for (i = 0; i < slc_hdr->num_negative_pics; i++) {
        if (slc_hdr->used_by_curr_pic_s0_flag[i]) {
            slc_hdr->num_active_ref_pics++;
            slc_hdr->num_active_ref_pics_s0++;
        }
    }

    for (i = 0; i < slc_hdr->num_long_term_pics; i++) {
        if (slc_hdr->used_by_curr_pic_lt_flag[i]) {
            slc_hdr->num_active_ref_pics++;
            slc_hdr->num_active_ref_pics_s0++;
        }
    }

    for (i = 0; i < slc_hdr->num_positive_pics; i++) {
        if (slc_hdr->used_by_curr_pic_s1_flag[i]) {
            slc_hdr->num_active_ref_pics++;
        }
    }

    return;
}

/******************************************************************************
  根据正常参考 生成sliceheader中的reflist && reorder 语法
  当前帧永远做参考帧;
  有效参考帧只有1个
******************************************************************************/
hi_void h265e_set_slc_hdr_ref_normal(h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr)
{
    h265e_ref_list *ref_list;
    hi_s32 curr_poc;

    ref_list = &slc_hdr->ref_list;
    curr_poc = enc_para->h265_poc.pic_order_cnt;

    slc_hdr->num_positive_pics = 0;
    ref_list->num_positive_pics = 0;
    ref_list->num_unmark_pics = 0;

    slc_hdr->num_negative_pics = 1;
    slc_hdr->delta_poc_s0_minus1[0] = 0;  // 图像集中每个参考帧对应的poc
    slc_hdr->used_by_curr_pic_s0_flag[0] = 1;  // 图像集中被当前帧所使用（所参考）的帧
    slc_hdr->num_ref_idx_l0_active_minus1 = 0;
    slc_hdr->short_term_ref_pic_set_idx = 0;

    ref_list->num_negative_pics = 1;
    ref_list->poc_short[0] = curr_poc - 1;
    ref_list->short_term_used[0] = 1;

    h265e_get_active_ref_num(slc_hdr);

    ref_list->num_unmark_pics = 1;
    ref_list->poc_unmark[0] = curr_poc - 1;

    enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
    enc_para->rcn_ref_share_buf.active_ref0_idx = 0;

    return;
}

hi_void set_slc_hdr_ref_for_dual_p_inter_p(vedu_efl_enc_para *enc_para, hi_s32 curr_poc,
                                           vedu_efl_h265e_slc_hdr *slc_hdr, h265e_ref_list *ref_list)
{
    slc_hdr->num_positive_pics = 0;
    slc_hdr->num_negative_pics = 1;
    slc_hdr->delta_poc_s0_minus1[0] = 0;
    slc_hdr->used_by_curr_pic_s0_flag[0] = 1;
    slc_hdr->num_ref_idx_l0_active_minus1 = 0;
    slc_hdr->short_term_ref_pic_set_idx = 1;
    slc_hdr->num_long_term_pics = 0;

    ref_list->num_negative_pics = 1;
    ref_list->num_positive_pics = 0;
    ref_list->poc_short[0] = curr_poc - 1;
    ref_list->short_term_used[0] = 1;
    ref_list->num_long_term_pics = 0;

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
        enc_para->rcn_ref_share_buf.active_ref0_idx = 0;
        enc_para->rcn_ref_share_buf.exchange_ref = HI_FALSE;
    }
    if (enc_para->ref_gap_cnt != 1) {
        slc_hdr->num_negative_pics = 2; /* 2: 2 negative pics */
        slc_hdr->delta_poc_s0_minus1[1] = 0;
        slc_hdr->used_by_curr_pic_s0_flag[1] = 1;

        slc_hdr->num_ref_idx_l0_active_minus1 = 1;
        slc_hdr->short_term_ref_pic_set_idx = 0;

        ref_list->num_negative_pics = 2; /* 2: 2 negative pics */
        ref_list->poc_short[1] = curr_poc - 2; /* 2: 2 negative pics */
        ref_list->short_term_used[1] = 1;

        slc_hdr->num_long_term_pics = 0;

        ref_list->num_unmark_pics = 1;
        ref_list->poc_unmark[0] = curr_poc - 2; /* 2: 2 negative pics */
        enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
        enc_para->rcn_ref_share_buf.active_ref0_idx = 0;
        enc_para->rcn_ref_share_buf.active_ref1_idx = 1;
    }

    return;
}

hi_void set_slc_hdr_ref_for_dual_p_sp_no_interval(vedu_efl_enc_para *enc_para, hi_s32 curr_poc,
                                                  vedu_efl_h265e_slc_hdr *slc_hdr, h265e_ref_list *ref_list)
{
    slc_hdr->num_positive_pics = 0;
    slc_hdr->num_negative_pics = 2; /* 2: 2 negative pics */
    /* 前一P帧 */
    slc_hdr->delta_poc_s0_minus1[0] = 0;
    slc_hdr->used_by_curr_pic_s0_flag[0] = 1;
    /* 前两P帧 */
    slc_hdr->delta_poc_s0_minus1[1] = 0;
    slc_hdr->used_by_curr_pic_s0_flag[1] = 1;

    slc_hdr->num_ref_idx_l0_active_minus1 = 1;
    slc_hdr->short_term_ref_pic_set_idx = 0;

    ref_list->num_negative_pics = 2; /* 2: 2 negative pics */
    ref_list->num_positive_pics = 0;
    ref_list->poc_short[0] = curr_poc - 1;
    ref_list->short_term_used[0] = 1;
    ref_list->poc_short[1] = curr_poc - 2; /* 2: 2 negative pics */
    ref_list->short_term_used[1] = 1;

    ref_list->num_unmark_pics = 1;
    ref_list->poc_unmark[0] = curr_poc - 2; /* 2: 2 negative pics */

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        if (!enc_para->rcn_ref_share_buf.exchange_ref) {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
            enc_para->rcn_ref_share_buf.active_ref0_idx = 1;
            enc_para->rcn_ref_share_buf.active_ref1_idx = 0;
            enc_para->rcn_ref_share_buf.exchange_ref = HI_TRUE;
        } else {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
            enc_para->rcn_ref_share_buf.active_ref0_idx = 0;
            enc_para->rcn_ref_share_buf.active_ref1_idx = 1;
            enc_para->rcn_ref_share_buf.exchange_ref = HI_FALSE;
        }
    }

    return;
}

hi_void set_slc_hdr_ref_for_dual_p_sp_with_interval(vedu_efl_enc_para *enc_para, hi_s32 curr_poc,
                                                    vedu_efl_h265e_slc_hdr *slc_hdr, h265e_ref_list *ref_list,
                                                    hi_venc_gop_attr *gop_attr)
{
    hi_s32 delta_poc;
    hi_s32 poc_lsb_lt;
    hi_s32 ref_poc;

    delta_poc = enc_para->ref_gap_cnt % gop_attr->sp_interval;
    ref_poc = curr_poc - delta_poc;
    poc_lsb_lt = ref_poc % enc_para->h265_poc.max_pic_order_cnt_lsb;

    slc_hdr->num_long_term_pics = 1;
    slc_hdr->used_by_curr_pic_lt_flag[0] = 1;
    slc_hdr->poc_lsb_lt[0] = poc_lsb_lt;
    if (poc_lsb_lt != ref_poc) {
        slc_hdr->delta_poc_msb_present_flag[0] = 1;
        slc_hdr->delta_poc_msb_cycle_lt[0] = (curr_poc >> enc_para->h265_poc.log2_max_poc_lsb) -
                                               (ref_poc >> enc_para->h265_poc.log2_max_poc_lsb);
    } else {
        slc_hdr->delta_poc_msb_present_flag[0] = 0;
        slc_hdr->delta_poc_msb_cycle_lt[0] = 0;
    }

    slc_hdr->num_positive_pics = 0;
    slc_hdr->num_negative_pics = 1;
    /* 前一P帧 */
    slc_hdr->delta_poc_s0_minus1[0] = 0;
    slc_hdr->used_by_curr_pic_s0_flag[0] = 1;

    slc_hdr->num_ref_idx_l0_active_minus1 = 1;
    slc_hdr->short_term_ref_pic_set_idx = 1;

    ref_list->num_long_term_pics = 1;
    ref_list->poc_long[0] = curr_poc - delta_poc;
    ref_list->long_term_used[0] = 1;
    ref_list->num_negative_pics = 1;
    ref_list->num_positive_pics = 0;
    ref_list->poc_short[0] = curr_poc - 1;
    ref_list->short_term_used[0] = 1;

    ref_list->num_unmark_pics = 1;
    ref_list->poc_unmark[0] = curr_poc - 1;

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
        enc_para->rcn_ref_share_buf.active_ref0_idx = 1;
        enc_para->rcn_ref_share_buf.active_ref1_idx = 0;
    }

    return;
}

hi_void set_slc_hdr_ref_for_dual_p_bp(vedu_efl_enc_para *enc_para, hi_s32 curr_poc,
                                      vedu_efl_h265e_slc_hdr *slc_hdr, h265e_ref_list *ref_list,
                                      hi_venc_gop_attr *gop_attr)
{
    hi_s32 delta_poc;
    hi_s32 poc_lsb_lt;
    hi_s32 ref_poc;

    delta_poc = gop_attr->sp_interval;

    ref_poc = curr_poc - delta_poc;
    poc_lsb_lt = ref_poc % enc_para->h265_poc.max_pic_order_cnt_lsb;

    slc_hdr->num_negative_pics = 1;
    slc_hdr->num_positive_pics = 0;
    /* 前一P帧 */
    slc_hdr->delta_poc_s0_minus1[0] = 0;
    slc_hdr->used_by_curr_pic_s0_flag[0] = 1;

    slc_hdr->num_ref_idx_l0_active_minus1 = 1;
    slc_hdr->short_term_ref_pic_set_idx = 1;

    slc_hdr->num_long_term_pics = 1;
    slc_hdr->used_by_curr_pic_lt_flag[0] = 1;
    slc_hdr->poc_lsb_lt[0] = poc_lsb_lt;
    if (poc_lsb_lt != ref_poc) {
        slc_hdr->delta_poc_msb_present_flag[0] = 1;
        slc_hdr->delta_poc_msb_cycle_lt[0] = (curr_poc >> enc_para->h265_poc.log2_max_poc_lsb) -
                                               (ref_poc >> enc_para->h265_poc.log2_max_poc_lsb);
    } else {
        slc_hdr->delta_poc_msb_present_flag[0] = 0;
        slc_hdr->delta_poc_msb_cycle_lt[0] = 0;
    }

    ref_list->num_long_term_pics = 1;
    ref_list->poc_long[0] = curr_poc - delta_poc;
    ref_list->long_term_used[0] = 1;
    ref_list->num_negative_pics = 1;
    ref_list->num_positive_pics = 0;
    ref_list->poc_short[0] = curr_poc - 1;
    ref_list->short_term_used[0] = 1;

    ref_list->num_unmark_pics = 1;
    ref_list->poc_unmark[0] = curr_poc - delta_poc;

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
        enc_para->rcn_ref_share_buf.active_ref0_idx = 1;
        enc_para->rcn_ref_share_buf.active_ref1_idx = 0;
    }

    return;
}

hi_void h265e_set_slc_hdr_ref_dual_p (h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr)
{
    h265e_ref_list *ref_list = &slc_hdr->ref_list;
    hi_s32 curr_poc = enc_para->h265_poc.pic_order_cnt;
    hi_u32 pic_type = enc_para->rc.pic_type;
    hi_venc_gop_attr *gop_attr = &enc_para->gop_attr;


    ref_list->num_unmark_pics = 0;

    enc_para->rcn_ref_share_buf.active_rcn_idx = BUFF_SHARE_DEFAULT;
    enc_para->rcn_ref_share_buf.active_ref0_idx = BUFF_SHARE_DEFAULT;
    enc_para->rcn_ref_share_buf.active_ref1_idx = BUFF_SHARE_DEFAULT;

    if (PIC_INTER == pic_type) {
        set_slc_hdr_ref_for_dual_p_inter_p(enc_para, curr_poc, slc_hdr, ref_list);
    }

    if (pic_type == PIC_INTERSP && gop_attr->sp_interval == 0) {
        set_slc_hdr_ref_for_dual_p_sp_no_interval(enc_para, curr_poc, slc_hdr, ref_list);
    }

    if (PIC_INTERSP == pic_type && gop_attr->sp_interval != 0) {
        set_slc_hdr_ref_for_dual_p_sp_with_interval(enc_para, curr_poc, slc_hdr, ref_list, gop_attr);
    }

    if (PIC_INTERBP == pic_type && (gop_attr->sp_interval > 1)) {
        set_slc_hdr_ref_for_dual_p_bp(enc_para, curr_poc, slc_hdr, ref_list, gop_attr);
    }

    h265e_get_active_ref_num(slc_hdr);

    return;
}

hi_void set_slc_hdr_ref_b_intra(h265e_ref *ref, hi_s32 *curr_poc)
{
    *curr_poc = 0;
    ref->poc_last_p = 0;
    ref->poc_last_last_p = 0;
    ref->last_idr = 1;

    return;
}

hi_void h265e_set_slc_hdr_ref_b_biinter(h265e_ref *ref, hi_s32 *curr_poc, h265e_ref_list *ref_list,
                                        vedu_efl_h265e_slc_hdr *slc_hdr)
{
    hi_s32 poc_short;

    ref->enhance_cnt++;
    *curr_poc = ref->poc_last_last_p + ref->enhance_cnt;
    poc_short = ref->poc_last_last_p;

    slc_hdr->num_negative_pics = 1;
    slc_hdr->delta_poc_s0_minus1[0] = *curr_poc - poc_short - 1;
    slc_hdr->used_by_curr_pic_s0_flag[0] = 1;
    slc_hdr->num_ref_idx_l0_active_minus1 = 0;

    ref_list->num_negative_pics = 1;
    ref_list->poc_short[0] = poc_short;
    ref_list->short_term_used[0] = 1;

    poc_short = ref->poc_last_p;

    slc_hdr->num_positive_pics = 1;
    slc_hdr->delta_poc_s1_minus1[0] = poc_short - *curr_poc - 1;
    slc_hdr->used_by_curr_pic_s1_flag[0] = 1;
    slc_hdr->num_ref_idx_l1_active_minus1 = 0;
    slc_hdr->short_term_ref_pic_set_idx = ref->enhance_cnt + 1;

    ref_list->num_positive_pics = 1;
    ref_list->poc_short_s1[0] = poc_short;
    ref_list->short_term_used_s1[0] = 1;

    ref->last_idr = 0;

    ref_list->num_unmark_pics = 1;
    ref_list->poc_unmark[0] = *curr_poc;

    HI_ERR_VENC("num_negative_pics = %d curr_poc = %d \n", slc_hdr->num_negative_pics, *curr_poc);

    return;
}

hi_void h265e_set_slc_hdr_ref_b_p(h265e_ref *ref, hi_s32 *curr_poc, h265e_ref_list *ref_list,
                                  vedu_efl_h265e_slc_hdr *slc_hdr)
{

    hi_s32 poc_short = 0;
    hi_s32 poc_short1 = 0;
    ref->enhance_cnt = 0;
    *curr_poc = ref->poc_last_p + ref->b_frm_cnt + 1;

    if (ref->last_idr) {
        poc_short = ref->poc_last_p;

        slc_hdr->num_negative_pics = 1;
        slc_hdr->delta_poc_s0_minus1[0] = *curr_poc - poc_short - 1;
        slc_hdr->used_by_curr_pic_s0_flag[0] = 1;
        slc_hdr->num_ref_idx_l0_active_minus1 = 0;
        slc_hdr->short_term_ref_pic_set_idx = 0;

        ref_list->num_negative_pics = 1;
        ref_list->poc_short[0] = poc_short;
        ref_list->short_term_used[0] = 1;

        slc_hdr->num_positive_pics = 0;
        ref_list->num_positive_pics = 0;

        HI_ERR_VENC("num_negative_pics = %d curr_poc = %d \n", slc_hdr->num_negative_pics, *curr_poc);
    } else {
        poc_short = ref->poc_last_p;
        poc_short1 = ref->poc_last_last_p;

        slc_hdr->num_negative_pics = 2; /* 2: 2 negative pics */
        slc_hdr->delta_poc_s0_minus1[0] = *curr_poc - poc_short - 1;
        slc_hdr->used_by_curr_pic_s0_flag[0] = 1;
        slc_hdr->delta_poc_s0_minus1[1] = poc_short - poc_short1 - 1;
        slc_hdr->used_by_curr_pic_s0_flag[1] = 1;
        slc_hdr->num_ref_idx_l0_active_minus1 = 1;
        slc_hdr->short_term_ref_pic_set_idx = 1;

        ref_list->num_negative_pics = 2; /* 2: 2 negative pics */
        ref_list->poc_short[0] = poc_short;
        ref_list->short_term_used[0] = 1;
        ref_list->poc_short[1] = poc_short1;
        ref_list->short_term_used[1] = 1;

        slc_hdr->num_positive_pics = 0;
        ref_list->num_positive_pics = 0;

        ref_list->num_unmark_pics = 1;
        ref_list->poc_unmark[0] = ref_list->poc_short[1];

        HI_ERR_VENC("num_negative_pics = %d curr_poc = %d \n", slc_hdr->num_negative_pics, *curr_poc);
    }

    ref->poc_last_last_p = ref->poc_last_p;
    ref->poc_last_p = *curr_poc;
    ref->last_idr = 0;

    return;
}

hi_void h265e_set_slc_hdr_ref_b (h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr)
{
    h265e_ref_list *ref_list = &slc_hdr->ref_list;
    hi_s32 curr_poc = 0;
    hi_u32 pic_type = enc_para->rc.pic_type;

    ref_list->num_unmark_pics = 0;
    slc_hdr->ref_pic_list_modification_flag_l0 = 0;
    slc_hdr->ref_pic_list_modification_flag_l1 = 0;

    HI_ERR_VENC("pic_type = %d \n", pic_type);

    if (PIC_INTRA == pic_type) {
        set_slc_hdr_ref_b_intra(ref, &curr_poc);
    } else if (PIC_BIINTER == pic_type) {
        h265e_set_slc_hdr_ref_b_biinter(ref, &curr_poc, ref_list, slc_hdr);
    } else {
        h265e_set_slc_hdr_ref_b_p(ref, &curr_poc, ref_list, slc_hdr);
    }

    enc_para->h265_poc.pic_order_cnt = curr_poc;
    enc_para->h265_poc.pic_order_cnt_lsb = curr_poc % enc_para->h265_poc.max_pic_order_cnt_lsb;
    HI_ERR_VENC("curr_poc = %d pic_order_cnt_lsb = %d \n", curr_poc, enc_para->h265_poc.pic_order_cnt_lsb);
    h265e_get_active_ref_num(slc_hdr);

    return;
}

hi_s32 h265e_set_slc_hdr_ref_param(h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr)
{
    hi_venc_gop_mode gop_type;

    HI_H265E_REF_ASSERT_RETURN(ref != HI_NULL && enc_para != HI_NULL && slc_hdr != HI_NULL);
    HI_H265E_REF_ASSERT_RETURN(enc_para->rc.gop_type >= HI_VENC_GOP_MODE_NORMALP &&
                               enc_para->rc.gop_type <= HI_VENC_GOP_MODE_BIPREDB);

    gop_type = enc_para->rc.gop_type;

    switch (gop_type) {
        case HI_VENC_GOP_MODE_NORMALP: {
            h265e_set_slc_hdr_ref_normal(ref, enc_para, slc_hdr);
            break;
        }
        case HI_VENC_GOP_MODE_DUALP: {
            h265e_set_slc_hdr_ref_dual_p (ref, enc_para, slc_hdr);
            break;
        }
        case HI_VENC_GOP_MODE_BIPREDB: {
            h265e_set_slc_hdr_ref_b (ref, enc_para, slc_hdr);
            break;
        }
        default:
            HI_ERR_VENC("Unknown gop type!\n");
            break;
    }

    return HI_SUCCESS;
}

hi_s32 h265e_get_num_ref_frames(hi_u32 gop_type)
{
    hi_u32 rcn_num = 0;

    HI_H265E_REF_ASSERT_RETURN(gop_type >= HI_VENC_GOP_MODE_NORMALP && gop_type <= HI_VENC_GOP_MODE_BIPREDB);

    if (gop_type == HI_VENC_GOP_MODE_NORMALP) {
        rcn_num = 2; /* 2: 2 reconstruction frame */
    } else if (gop_type == HI_VENC_GOP_MODE_DUALP || gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        rcn_num = 3; /* 3: 3 reconstruction frame */
    } else {
        HI_ERR_VENC("unkown gop type! \n");
        return HI_FAILURE;
    }

    return rcn_num;
}


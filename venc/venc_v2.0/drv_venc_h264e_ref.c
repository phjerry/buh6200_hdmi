/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#include "drv_venc_h264e_ref.h"
#include "drv_venc_efl.h"
#include "public.h"
#include "hi_venc_type.h"

#define HI_H264E_REF_ASSERT_RETURN(condition)\
do { \
    if (!(condition)) { \
        HI_ERR_VENC("assert warning\n"); \
        return HI_FAILURE;\
    } \
} while (0)
#define HI_H264E_REF_ASSERT(condition)                  \
do {                                                \
    if (!(condition)) {                               \
        HI_ERR_VENC("assert warning\n");            \
        return;                                     \
    }                                               \
} while (0)

hi_s32 h264e_init_ref(h264e_ref *ref)
{
    HI_H264E_REF_ASSERT_RETURN(ref != HI_NULL);

    ref->last_long_poc = 0;
    ref->last_long_poc_back = 0;

    ref->last_p = 0;
    ref->poc_last_p_bak = 0;

    ref->last_last_p = 0;
    ref->last_last_p_bak = 0;

    return HI_SUCCESS;
}

hi_s32 h264e_back_up_ref(h264e_ref *ref)
{
    HI_H264E_REF_ASSERT_RETURN(ref != HI_NULL);

    ref->last_long_poc = ref->last_long_poc_back;
    ref->last_p = ref->poc_last_p_bak;
    ref->last_last_p = ref->last_last_p_bak;

    return HI_SUCCESS;
}

hi_s32 h264e_init_poc(vedu_efl_enc_para *enc_para)
{
    hi_s32 i = 0;
    hi_venc_gop_mode gop_type;
    hi_u32 b_frm_num;

    HI_H264E_REF_ASSERT_RETURN(enc_para != HI_NULL);

    gop_type = enc_para->rc.gop_type;
    b_frm_num = enc_para->rc.b_frm_num;

    enc_para->h264_poc.pic_order_cnt_type = 0;
    enc_para->h264_poc.pic_order_present_flag = 0;

    if (gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        enc_para->h264_poc.delta_pic_order_always_zero_flag = 0;
    } else {
        enc_para->h264_poc.delta_pic_order_always_zero_flag = 1;
    }

    enc_para->h264_poc.offset_for_top_to_bottom_field = 0;
    enc_para->h264_poc.num_ref_frames_in_pic_order_cnt_cycle = 1;

    enc_para->h264_poc.offset_for_non_ref_pic = 1;
    for (i = 0; i < enc_para->h264_poc.num_ref_frames_in_pic_order_cnt_cycle; i++) {
        if (gop_type == HI_VENC_GOP_MODE_BIPREDB) {
            enc_para->h264_poc.offset_for_ref_frame[i] = 2 * b_frm_num; /* 2: offset_for_ref_frame */
        } else {
            enc_para->h264_poc.offset_for_ref_frame[i] = 2; /* 2: offset_for_ref_frame */
        }
    }

    enc_para->h264_poc.delta_pic_order_cnt[0] = 0;
    enc_para->h264_poc.delta_pic_order_cnt[1] = 0;

    enc_para->h264_poc.log2_max_pic_order_cnt_lsb_minus4 = 0;
    enc_para->h264_poc.pic_order_cnt_lsb = 0;
    enc_para->h264_poc.delta_pic_order_cnt_bottom = 0;

    enc_para->h264_poc.max_poc_lsb = 1 <<
        (enc_para->h264_poc.log2_max_pic_order_cnt_lsb_minus4 + 4);  /* 4: fomula for calculate max poc */
    enc_para->h264_poc.poc = 0;

    if (gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        enc_para->h264_poc.pic_order_cnt_type = 0;
    } else {
        enc_para->h264_poc.pic_order_cnt_type = 2; /* 2: pic_order_cnt_type except of B gop mode */
    }

    return HI_SUCCESS;
}

hi_s32 h264e_encode_poc(vedu_efl_enc_para *enc_para, hi_u32 pic_type)
{
    h264e_poc *poc = HI_NULL;

    HI_H264E_REF_ASSERT_RETURN((enc_para != HI_NULL) && (pic_type >= PIC_INTRA && pic_type <= PIC_BIINTER));
    poc = &enc_para->h264_poc;

    if (pic_type == PIC_INTRA) {
        poc->poc = 0;
        poc->pic_order_cnt_lsb = 0;
        poc->log2_max_pic_order_cnt_lsb_minus4 = 12; /* 12: fomula for calculate max poc */
        poc->max_poc_lsb = 1 << (poc->log2_max_pic_order_cnt_lsb_minus4 + 4); /* 4: fomula for calculate max poc */
        return HI_SUCCESS;
    }

    poc->poc_back = poc->poc;
    poc->pic_order_cnt_lsb_back = poc->pic_order_cnt_lsb;

    poc->poc++;
    poc->pic_order_cnt_lsb++;

    if (poc->pic_order_cnt_lsb >= poc->max_poc_lsb) {
        poc->pic_order_cnt_lsb = 0;
    }

    return HI_SUCCESS;
}

hi_s32 h264e_back_up_poc(vedu_efl_enc_para *enc_para)
{
    h264e_poc *poc = HI_NULL;

    HI_H264E_REF_ASSERT_RETURN((enc_para != HI_NULL));
    poc = &enc_para->h264_poc;

    poc->poc = poc->poc_back;
    poc->pic_order_cnt_lsb = poc->pic_order_cnt_lsb_back;

    return HI_SUCCESS;
}

hi_s32 h264e_set_slc_hdr_ref_normal(vedu_efl_enc_para *enc_para)
{
    vedu_efl_h264e_slc_hdr *slc_hdr = HI_NULL;
    hi_u32 pic_type;

    HI_H264E_REF_ASSERT_RETURN((enc_para != HI_NULL));

    slc_hdr = &enc_para->h264_slc_hdr;
    pic_type = enc_para->rc.pic_type;

    slc_hdr->mark.adaptive_ref_pic_marking_mode_flag = 0;
    slc_hdr->reorder.ref_pic_list_reordering_flag_l0 = 0;
    slc_hdr->reorder.pic_num_lx_pred = enc_para->frame_num;
    slc_hdr->nal_ref_idc = 3; /* 3: current frame to be refered */
    enc_para->h264_rcn_pic->ref = REF_SHORT;

    enc_para->rcn_ref_share_buf.active_rcn_idx = BUFF_SHARE_DEFAULT;
    enc_para->rcn_ref_share_buf.active_ref0_idx = BUFF_SHARE_DEFAULT;
    enc_para->rcn_ref_share_buf.active_ref1_idx = BUFF_SHARE_DEFAULT;

    if (pic_type == PIC_INTRA) {
        slc_hdr->mark.long_term_reference_flag = 0;
        slc_hdr->mark.no_output_of_prior_pics_flag = 0;
        enc_para->h264_reorder_ref[0] = HI_NULL;
        enc_para->h264_reorder_ref[1] = HI_NULL;

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
        }
    } else {
        enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
        enc_para->h264_reorder_ref[1] = HI_NULL;
        HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
            enc_para->rcn_ref_share_buf.active_ref0_idx = 0;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 generate_ref_list_no_sp(vedu_efl_enc_para *enc_para, vedu_efl_h264e_slc_hdr *slc_hdr, hi_u32 pic_type)
{
    if (pic_type  == PIC_INTRA) {
        slc_hdr->mark.long_term_reference_flag = 0;
        slc_hdr->mark.no_output_of_prior_pics_flag = 0;

        enc_para->h264_reorder_ref[0] = HI_NULL;
        enc_para->h264_reorder_ref[1] = HI_NULL;

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
        }
    } else if (enc_para->h264_dpb.short_ref_num == 1) {
        enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
        enc_para->h264_reorder_ref[1] = HI_NULL;
        HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
            enc_para->rcn_ref_share_buf.active_ref0_idx = 0;
            enc_para->rcn_ref_share_buf.exchange_ref = HI_FALSE;
        }
    } else {
        enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
        enc_para->h264_reorder_ref[1] = enc_para->h264_dpb.past_ref_pic_list0[1];
        HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
        HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[1] != HI_NULL);

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            if (enc_para->rcn_ref_share_buf.exchange_ref == HI_FALSE) {
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
    }

    return HI_SUCCESS;
}

static hi_s32 generate_ref_list_sp_for_bp(vedu_efl_enc_para *enc_para, vedu_efl_h264e_slc_hdr *slc_hdr)
{
    slc_hdr->mark.adaptive_ref_pic_marking_mode_flag = 1;
    slc_hdr->mark.memory_management_control_operation[0] = MARKING_LONGTERM_TO_NONREF;
    slc_hdr->mark.marking_operation[0] = 0;
    /* 设置当前帧为长期参考帧，分配长期参考帧序号为0 */
    slc_hdr->mark.memory_management_control_operation[1] = MARKING_CURFRAME_TO_LONGTERM;
    slc_hdr->mark.marking_operation[1] = 0;
    slc_hdr->mark.memory_management_control_operation[2] = MARKING_OPERATION_END; /* 2: array index */
    enc_para->h264_rcn_pic->ref = REF_LONG;
    enc_para->h264_rcn_pic->long_term_pic_num = 0;

    enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
    enc_para->h264_reorder_ref[1] = enc_para->h264_dpb.past_ref_pic_list0[1];

    HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
    HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[1] != HI_NULL);

    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
        enc_para->rcn_ref_share_buf.active_ref0_idx = 1;
        enc_para->rcn_ref_share_buf.active_ref1_idx = 0;
    }

    return HI_SUCCESS;
}

static hi_s32 generate_ref_list_sp(vedu_efl_enc_para *enc_para, vedu_efl_h264e_slc_hdr *slc_hdr, hi_u32 pic_type)
{
    hi_s32 ret;
    if (pic_type == PIC_INTRA) {
        slc_hdr->mark.long_term_reference_flag = 1;
        slc_hdr->mark.no_output_of_prior_pics_flag = 0;
        enc_para->h264_rcn_pic->ref = REF_LONG;
        enc_para->h264_rcn_pic->long_term_pic_num = 0;

        enc_para->h264_reorder_ref[0] = HI_NULL;
        enc_para->h264_reorder_ref[1] = HI_NULL;

        if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
            enc_para->rcn_ref_share_buf.active_rcn_idx = 0;
        }
    } else if (pic_type == PIC_INTERBP) {
        ret = generate_ref_list_sp_for_bp(enc_para, slc_hdr);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("generate_ref_list_sp_for_bp fail\n");
            return ret;
        }

    } else {
        if (enc_para->h264_dpb.short_ref_num == 0) {
            enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
            enc_para->h264_reorder_ref[1] = HI_NULL;
            HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
            if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
                enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
                enc_para->rcn_ref_share_buf.active_ref0_idx = 0;
            }
        } else {
            enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
            enc_para->h264_reorder_ref[1] = enc_para->h264_dpb.past_ref_pic_list0[1];

            HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
            HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[1] != HI_NULL);

            if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
                enc_para->rcn_ref_share_buf.active_rcn_idx = 1;
                enc_para->rcn_ref_share_buf.active_ref0_idx = 1;
                enc_para->rcn_ref_share_buf.active_ref1_idx = 0;
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 h264e_set_slc_hdr_ref_dual_p(vedu_efl_enc_para *enc_para)
{
    vedu_efl_h264e_slc_hdr *slc_hdr = HI_NULL;
    hi_u32 pic_type;
    hi_venc_gop_attr *gop_attr = HI_NULL;
    hi_s32 ret;

    HI_H264E_REF_ASSERT_RETURN((enc_para != HI_NULL));

    slc_hdr = &enc_para->h264_slc_hdr;
    pic_type = enc_para->rc.pic_type;
    gop_attr = &enc_para->gop_attr;

    slc_hdr->mark.adaptive_ref_pic_marking_mode_flag = 0;
    slc_hdr->reorder.ref_pic_list_reordering_flag_l0 = 0;
    slc_hdr->reorder.pic_num_lx_pred = enc_para->frame_num;
    slc_hdr->nal_ref_idc = 3; /* 3: current frame to be refered */
    enc_para->h264_rcn_pic->ref = REF_SHORT;

    if (gop_attr->sp_interval == 0) {
        ret = generate_ref_list_no_sp(enc_para, slc_hdr, pic_type);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("generate_ref_list_no_sp fail \n");
            return ret;
        }
    }

    if (gop_attr->sp_interval != 0) {
        ret = generate_ref_list_sp(enc_para, slc_hdr, pic_type);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("generate_ref_list_sp fail \n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 generate_ref_list_b_frame(vedu_efl_enc_para *enc_para, vedu_efl_h264e_slc_hdr *slc_hdr, h264e_ref *ref,
    hi_u32 pic_type)
{
    hi_s32 curr_poc = 0;

    if (pic_type == PIC_INTRA) {
        curr_poc = 0;
        slc_hdr->mark.long_term_reference_flag = 0;
        slc_hdr->mark.no_output_of_prior_pics_flag = 0;
        enc_para->h264_reorder_ref[0] = HI_NULL;
        enc_para->h264_reorder_ref[1] = HI_NULL;
        ref->last_p = 0;
    } else if (pic_type == PIC_BIINTER) {
        ref->enhance_cnt++;
        curr_poc = ref->last_last_p + ref->enhance_cnt;
        slc_hdr->nal_ref_idc = 0;
        enc_para->h264_rcn_pic->ref = REF_NONE;
        enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
        enc_para->h264_reorder_ref[1] = enc_para->h264_dpb.past_ref_pic_list1[0];
        HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
        HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[1] != HI_NULL);
    } else {
        ref->enhance_cnt = 0;
        curr_poc = ref->last_p + ref->b_frm_cnt + 1;
        ref->last_last_p = ref->last_p;
        ref->last_p = curr_poc;
        if (enc_para->h264_dpb.short_ref_num == 1) {
            enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
            enc_para->h264_reorder_ref[1] = HI_NULL;
            HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
        } else {
            enc_para->h264_reorder_ref[0] = enc_para->h264_dpb.past_ref_pic_list0[0];
            enc_para->h264_reorder_ref[1] = enc_para->h264_dpb.past_ref_pic_list0[1];
            HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[0] != HI_NULL);
            HI_H264E_REF_ASSERT_RETURN(enc_para->h264_reorder_ref[1] != HI_NULL);
        }
    }

    enc_para->h264_poc.poc = curr_poc;
    enc_para->h264_poc.pic_order_cnt_lsb = curr_poc % enc_para->h264_poc.max_poc_lsb;

    return HI_SUCCESS;
}

hi_s32 h264e_set_slc_hdr_ref_b(vedu_efl_enc_para *enc_para)
{
    vedu_efl_h264e_slc_hdr *slc_hdr = HI_NULL;
    h264e_ref *ref = HI_NULL;
    hi_u32 pic_type;
    hi_s32 ret;

    HI_H264E_REF_ASSERT_RETURN((enc_para != HI_NULL));
    slc_hdr = &enc_para->h264_slc_hdr;
    ref = &enc_para->h264_ref;
    pic_type = enc_para->rc.pic_type;

    slc_hdr->mark.adaptive_ref_pic_marking_mode_flag = 0;
    slc_hdr->reorder.ref_pic_list_reordering_flag_l0 = 0;
    slc_hdr->reorder.ref_pic_list_reordering_flag_l1 = 0;
    slc_hdr->reorder.pic_num_lx_pred = enc_para->frame_num;
    slc_hdr->reorder.pic_num_lx_pred1 = enc_para->frame_num;
    slc_hdr->nal_ref_idc = 3; /* 3: current frame to be refered */
    enc_para->h264_rcn_pic->ref = REF_SHORT;

    ref->last_long_poc_back = ref->last_long_poc;
    ref->poc_last_p_bak = ref->last_p;
    ref->last_last_p_bak = ref->last_last_p;

    ret = generate_ref_list_b_frame(enc_para, slc_hdr, ref, pic_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("generate_ref_list_b_frame fail \n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 h264e_slc_hdr_mark_reorder(vedu_efl_enc_para *enc_para)
{
    hi_u32 gop_type;

    HI_H264E_REF_ASSERT_RETURN((enc_para != HI_NULL));

    gop_type = enc_para->rc.gop_type;

    switch (gop_type) {
        case HI_VENC_GOP_MODE_NORMALP: {
            h264e_set_slc_hdr_ref_normal(enc_para);
            break;
        }

        case HI_VENC_GOP_MODE_DUALP: {
            h264e_set_slc_hdr_ref_dual_p(enc_para);
            break;
        }

        case HI_VENC_GOP_MODE_BIPREDB: {
            h264e_set_slc_hdr_ref_b(enc_para);
            break;
        }

        default:
            HI_ERR_VENC("unkown gop type\n");
            break;
    }

    return HI_SUCCESS;
}

hi_s32 h264e_get_num_ref_frames(hi_u32 gop_type)
{
    hi_u32 rcn_num = 0;

    HI_H264E_REF_ASSERT_RETURN((gop_type >= 0 && gop_type < 3));

    if (gop_type ==  HI_VENC_GOP_MODE_NORMALP) {
        rcn_num = 2; /* 2: two restuction frame memory */
    } else if (gop_type == HI_VENC_GOP_MODE_DUALP || gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        rcn_num = 3; /* 3: three restuction frame memory */
    } else {
        HI_ERR_VENC("unkown gop type! \n");
        return HI_FAILURE;
    }

    return rcn_num;
}


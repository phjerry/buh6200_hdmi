/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_VENC_H264E_DPB_H__
#define __DRV_VENC_H264E_DPB_H__

#include "drv_venc_efl.h"
#include "hi_type.h"

hi_s32 h264e_dpb_init(h264e_dpb *dpb, hi_s32 max_frame_num);
hi_s32 h264e_dpb_flush(vedu_efl_enc_para *enc_para);
hi_s32 h264e_dpb_insert_frame(vedu_efl_h264e_slc_hdr *slc_hdr, h264e_dpb *dpb, h264e_pic *frame,
    vedu_efl_enc_para *enc_para);
hi_s32 h264e_update_dpb_frames(vedu_efl_enc_para *enc_para);
hi_s32 h264e_update_frame_num(vedu_efl_enc_para *enc_para);
hi_s32 h264e_mm_unmark_short_term_for_reference(h264e_dpb *dpb, hi_s32 pic_num, hi_s32 difference_of_pic_nums_minus1);
hi_s32 h264e_mm_unmark_long_term_for_reference(h264e_dpb *dpb, hi_s32 pic_num, hi_s32 s32LongTermNum);
hi_s32 h264e_mm_mark_current_picture_long_term(h264e_dpb *dpb, h264e_pic *frame, hi_s32 long_term_frame_idx);
hi_s32 h264e_mm_mark_set_max_long_term_idx(h264e_dpb *dpb, hi_s32 max_long_term_frame_idx_plus1);
hi_s32 h264e_mm_assign_long_term_frame_idx(h264e_dpb *dpb, hi_s32 pic_num, hi_s32 difference_of_pic_nums_minus1,
    hi_s32 long_term_frame_idx);
hi_s32 h264e_set_rcn_out_pic(vedu_efl_enc_para *enc_para);
hi_s32 h264e_dpb_init_ref_list_p(vedu_efl_enc_para *enc_para);
hi_s32 h264e_dpb_init_ref_list_b(vedu_efl_enc_para *enc_para);
hi_s32 h264e_dpb_init_ref_list(vedu_efl_enc_para *enc_para, hi_u32 pic_type);
hi_s32 h264e_dpb_get_new_frame(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in);

#endif

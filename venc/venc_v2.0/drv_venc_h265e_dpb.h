/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_VENC_H265E_DPB_H__
#define __DRV_VENC_H265E_DPB_H__

#include "drv_venc_efl.h"
#include "hi_type.h"

hi_s32 h265e_get_rcn_pic_buf(vedu_efl_enc_para *enc_para, hi_u32 *pic_id, hi_u32 *pic_addr);
hi_s32 h265e_get_rcn_pic_info_buf(vedu_efl_enc_para *enc_para, hi_u32 *pic_info_id, hi_u32 *pic_info_idpic_info_id);
hi_s32 h265e_empty_pool(vedu_efl_enc_para *enc_para);
hi_s32 h265e_dpb_init(h265e_dpb *dpb, hi_s32 max_frame_num);
hi_s32 h265e_dpb_flush(h265e_dpb *dpb);
hi_s32 h265e_dpb_get_new_frame(h265e_dpb *dpb, vedu_efl_enc_para *enc_para);
hi_s32 h265e_dpb_deinit(h265e_dpb *dpb);
hi_s32 h265e_dpb_generate_ref_list(h265e_dpb *dpb, vedu_efl_h265e_slc_hdr *slice, vedu_efl_enc_para *enc_para);
hi_s32 h265e_dpb_insert_frame(h265e_dpb *dpb, hi_s32 poc);
hi_s32 h265e_dpb_unmark(vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slice);

#endif

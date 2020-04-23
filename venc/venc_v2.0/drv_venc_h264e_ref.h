/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_VENC_H264E_REF_H__
#define __DRV_VENC_H264E_REF_H__

#include "hi_type.h"
#include "drv_venc_efl.h"

hi_s32 h264e_init_ref(h264e_ref *ref);
hi_s32 h264e_back_up_ref(h264e_ref *ref);
hi_s32 h264e_back_up_poc(vedu_efl_enc_para *enc_para);
hi_s32 h264e_init_poc(vedu_efl_enc_para *enc_para);
hi_s32 h264e_encode_poc(vedu_efl_enc_para *enc_para, hi_u32 pic_type);
hi_s32 h264e_set_slc_hdr_ref_normal(vedu_efl_enc_para *enc_para);
hi_s32 h264e_set_slc_hdr_ref_dual_p(vedu_efl_enc_para *enc_para);
hi_s32 h264e_set_slc_hdr_ref_b(vedu_efl_enc_para *enc_para);
hi_s32 h264e_slc_hdr_mark_reorder(vedu_efl_enc_para *enc_para);
hi_s32 h264e_get_num_ref_frames(hi_u32 gop_type);

#endif
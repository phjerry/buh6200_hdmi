/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */

#ifndef __DRV_VENC_H265E_REF_H__
#define __DRV_VENC_H265E_REF_H__

#include "hi_type.h"
#include "drv_venc_efl.h"


hi_s32 h265e_encode_poc(vedu_efl_enc_para *enc_para, hi_u32 pic_type);
hi_s32 h265e_ref_init(vedu_efl_enc_para *enc_para, h265e_ref *ref);
hi_void h265e_get_active_ref_num(vedu_efl_h265e_slc_hdr *slc_hdr);
hi_void h265e_set_slc_hdr_ref_normal(h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr);
hi_void h265e_set_slc_hdr_ref_dual_p (h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr);
hi_void h265e_set_slc_hdr_ref_b (h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr);
hi_s32 h265e_set_slc_hdr_ref_param(h265e_ref *ref, vedu_efl_enc_para *enc_para, vedu_efl_h265e_slc_hdr *slc_hdr);
hi_s32 h265e_get_num_ref_frames(hi_u32 gop_type);

#endif

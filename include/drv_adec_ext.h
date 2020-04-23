/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv adec ext header file
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#ifndef __DRV_ADEC_EXT_H__
#define __DRV_ADEC_EXT_H__

#include "hi_drv_ao.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifdef HI_MCE_SUPPORT
/* we don't support MCE for now */
/* #define ADEC_KERNEL_DECODE */
#endif

#ifdef ADEC_KERNEL_DECODE

typedef enum {
    ADEC_KEL_TPYE_MPEG, /* mpeg */
    ADEC_KEL_TPYE_BUTT
} adec_kernel_type;

typedef struct
{
    adec_kernel_type codec_type;
    hi_u32 in_buf_size; /* input buffer size */
    hi_u32 out_buf_num; /* output buffer number, buffer size depend on codec_id */
} adec_slim_attr;

hi_s32 hi_drv_adec_open(hi_handle *h_adec, adec_slim_attr *adec_attr);
hi_s32 hi_drv_adec_close(hi_handle h_adec);
hi_s32 hi_drv_adec_reset(hi_handle h_adec);
hi_s32 hi_drv_adec_set_attr(hi_handle h_adec, adec_slim_attr *adec_attr);
hi_s32 hi_drv_adec_get_attr(hi_handle h_adec, adec_slim_attr *adec_attr);
hi_s32 hi_drv_adec_send_stream(hi_handle h_adec, const hi_stream_buf *stream, hi_u32 pts);
hi_s32 hi_drv_adec_receive_frame(hi_handle h_adec, ao_frame *frame, hi_adec_ext_frame_info *info);
hi_s32 hi_drv_adec_release_frame(hi_handle h_adec, const ao_frame *frame);
hi_s32 hi_drv_adec_get_status_info(hi_handle h_adec, hi_adec_status *status_info);
hi_s32 hi_drv_adec_pull(hi_handle h_adec);
#endif

hi_s32  hi_drv_adec_init(hi_void);
hi_void hi_drv_adec_deinit(hi_void);

hi_s32  adec_drv_mod_init(hi_void);
hi_void adec_drv_mod_exit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_ADEC_EXT_H__ */

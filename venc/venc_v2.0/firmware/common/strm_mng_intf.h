/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_STRMINTF_H__
#define __DRV_STRMINTF_H__

#include "venc_strm_mng.h"
#include "public.h"

#define BUF_SIZE_MULT                10
#define BUF_SIZE_MULT_CHECK          15
#define BUF_SIZE_MULT_SPLIT          15
#define BUF_SIZE_MULT_SPLIT_CHECK    20

#define WORD_ALIGN 0x04       /* Bytes included in one word */

typedef struct {
    hi_u32 packet_len;     /* 64 aligned */
    hi_u32 invld_byte;     /* InvalidByteNum */
    hi_u8 type;
    hi_u8 bot_field;
    hi_u8 field;
    hi_u8 last_slice;
    hi_u32 chn_id;
    hi_u32 pts0;
    hi_u32 pts1;
    hi_u32 pts2;         /* for omx flag now */
    hi_u32 pts3;         /* for omx fillLne now */
    hi_u32 reserved[8]; /* 8 is size 0f reserved */
} vedu_efl_nalu_hdr;

typedef struct {
    hi_void (*fun_strm_init)(hi_void);
    hi_void (*fun_strm_deinit)(hi_void);
    hi_s32  (*fun_strm_creat)(hi_handle handle, strm_mng_inst_config *st_config);
    hi_s32  (*fun_strm_destroy)(hi_handle handle);

    hi_s32  (*fun_strm_get_write_buffer)(hi_handle handle, strm_mng_input_info *input_info,
        strm_mng_stream_info *stream_info);
    hi_s32  (*fun_strm_put_write_buffer)(hi_handle handle, strm_mng_input_info *input_info,
        strm_mng_stream_info *stream_info);

    hi_s32 (*fun_strm_put_buffer_for_recode)(hi_handle handle, strm_mng_input_info *input_info,
        strm_mng_stream_info *stream_info);

    hi_s32  (*fun_strm_acq_read_buffer)(hi_handle handle, strm_mng_stream_info *stream_info,
        vedu_efl_nalu *vedu_packet);
    hi_s32  (*fun_strm_rls_read_buffer)(hi_handle handle, strm_mng_stream_info *stream_info,
        vedu_efl_nalu* vedu_packet);

    hi_s32 (*fun_strm_get_acq_stream_info)(handle_protocol *h_protocol, hi_u8* start_vir_addr,
        vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset *st_buf_off_set);
    hi_s32 (*fun_strm_get_rls_buffer_info)(hi_handle handle, hi_u32 protocol, strm_mng_stream_info *stream_info,
                                          venc_ioctl_acquire_stream* st_acq_strm, vedu_efl_nalu* vedu_packet);
    hi_s32  (*fun_strm_get_rls_buffer_info_intal)(hi_handle handle, hi_venc_stream *stream,
        vedu_efl_nalu* vedu_packet);

    hi_s32  (*fun_strm_check_buffer_size)(hi_handle handle, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size);
    hi_u32  (*fun_strm_cal_slc_size)(hi_handle handle, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size);

    hi_s32  (*fun_strm_get_strm_state)(hi_handle handle, strm_mng_state* strm_stat_info);
    hi_u32  (*fun_strm_get_written_len)(hi_handle handle);
    hi_s32  (*fun_strm_reset_buffer)(hi_handle handle, hi_u32 protocol);
    hi_u32  (*fun_strm_get_bit_buf_size)(hi_handle handle, hi_u32 strm_buf_size, hi_u32 slc_split_en, hi_u32 luma_size,
        hi_u32 *ext_len);
    hi_s32  (*fun_strm_alloc_stream_buf)(hi_handle handle, venc_buffer *stream_mmz_buf,
        strm_mng_alloc_info *alloc_info, hi_bool enable_rcn_ref_share_buf);
    hi_s32  (*fun_strm_alloc_para_set_buf_addr)(hi_handle handle, venc_buffer *para_set_buf, hi_u32 *para_set_addr);
    hi_s32  (*fun_strm_rls_para_set_buf_addr)(hi_handle handle, venc_buffer *para_set_buf);

#ifdef VENC_SUPPORT_JPGE
    hi_s32  (*fun_strm_jpeg_write_header)(hi_handle handle, strm_mng_input_info *input_info,
        strm_mng_stream_info *stream_info);
    hi_s32  (*fun_strm_put_jpeg_write_buffer)(hi_handle handle, strm_mng_input_info *input_info,
        strm_mng_stream_info *stream_info);
    hi_s32  (*fun_strm_put_jpeg_remove_header)(hi_handle handle, strm_mng_input_info *input_info,
        strm_mng_stream_info *stream_info);
    hi_s32  (*fun_strm_get_jpeg_stream)(hi_handle handle, strm_mng_stream_info *stream_info,
        vedu_efl_nalu *vedu_packet, hi_venc_stream *stream, venc_ioctl_buf_offset *pstBufOffSet);
#endif
} venc_strm_fun_ptr;

#ifdef VENC_SUPPORT_H265
hi_venc_h265e_nalu_type trans_nalu_type_num_for_h265(hi_u32 nalu_type);
hi_u32 inverse_trans_nalu_type_num_for_h265(hi_venc_h265e_nalu_type en_hevce_type);
#endif

#endif


/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: encoder
* Author: sdk
* Create: 2019-08-13
*/

#ifndef __DRV_VENCSTRM_H__
#define __DRV_VENCSTRM_H__

#include "drv_venc_ioctl.h"
#include "drv_venc_osal_ext.h"

#define D_VENC_ALIGN_UP(val, align)  ((((val) + (align) - 1) / (align))*(align))
#define D_VENC_ALIGN_DOWN(val, align) ((val) - (val) % (align))

#define H264_NALU_TYPE_SPS     7
#define H264_NALU_TYPE_PPS     8
#define H264_NALU_TYPE_IDR     5
#define H264_NALU_TYPE_P       1

#define H265_NALU_TYPE_VPS     32
#define H265_NALU_TYPE_SPS     33
#define H265_NALU_TYPE_PPS     34
#define H265_NALU_TYPE_IDR     5
#define H265_NALU_TYPE_P       1
#define H265_NALU_TYPE_SEI     39

#define BUF_SIZE_MULT_CHECK_INIT     20

#ifdef VENC_DPT_ONLY
#define VEDU_MMZ_ALIGN       256
#else
#define VEDU_MMZ_ALIGN       64
#endif

#define SLICE_SIZE_LIMIT     3133440    /* 1920*1088*3/2 */

typedef struct {
    hi_void* virt_addr[2]; /* 2 is size 0f virt_addr */
    hi_u32 phy_addr[2]; /* 2 is size 0f phy_addr */
    hi_u32 slc_len[2];  /* 2 is size 0f slc_len */
    hi_u32 pts0;
    hi_u32 pts1;
    hi_u32 ext_flag;
    hi_u32 ext_fill_len;
    hi_u32 frame_end;
    hi_u32 nalu_type;   /* 1(P),5(I),6(SEI),7(SPS) or 8(PPS), only used by H264 */
    hi_u32 invld_byte;
} vedu_efl_nalu;

typedef struct {
    /* for list buf */
    hi_u32 phy_addr;      /* Start physical address. */
    hi_u8* knl_vir_addr;   /* Start kernel virtual address. */
    hi_u32 para_set_phy_addr;
    hi_u8 *para_set_knl_vir_addr;
    hi_u32 size;         /* Size */
    hi_bool tvp;

    /* for loop buf */
    hi_u32 strm_ext_len;
    hi_u32 rsv_byte;
} strm_mng_inst_config;

typedef struct {
    hi_u32 protocol;
    hi_u32 bit_buf_size;
    hi_u32 luma_size;
    hi_u32 me_mv_size;
    hi_u32 rcn_size;
    hi_u32 rcn_block_size;
} strm_mng_alloc_info;

typedef struct {
    hi_u32 used_stream_buf;       /* Used size */
    hi_u32 strm_head_free;        /* Head free size */
    hi_u32 strm_tail_free;        /* Tail free size */
    hi_u32 strm_get_try;          /* GetWriteBuf try times */
    hi_u32 strm_get_ok;           /* GetWriteBuf ok times */
    hi_u32 strm_put_try;          /* PutWriteBuf try times */
    hi_u32 strm_put_ok;           /* PutWriteBuf ok times */
    hi_u32 strm_recv_try;         /* GetReadBuf try times */
    hi_u32 strm_recv_ok;          /* GetReadBuf ok times */
    hi_u32 strm_rls_try;          /* PutReadBuf try times */
    hi_u32 strm_rls_ok;           /* PutReadBuf ok times */
} strm_mng_state;

typedef struct {
    /* paraset */
    hi_s32 prepend_sps_pps_enable;
    hi_u32 vps_bits;
    hi_u32 sps_bits;
    hi_u32 pps_bits;
    hi_u8* vps_stream;
    hi_u8* sps_stream;
    hi_u8* pps_stream;

    /* real stream */
    hi_u32 slice_length[16]; /* 16 is size of slice_length */
    hi_u32 slice_is_end[16]; /* 16 is size of slice_is_end */

    /* common info */
    hi_u16 slc_split_en;
    hi_u32 protocol;
    hi_u32 is_intra_pic;
    hi_u32 pts0;
    hi_u32 pts1;
    hi_u32 ext_flag;
    hi_u32 ext_fill_len;
    hi_u32 target_bits;
    hi_u32 venc_buf_full;
    hi_u32 venc_pbit_overflow;
    hi_u32 get_frame_num_ok;
    hi_s32 low_dly_mod;
    hi_u32 recode_cnt;
    hi_u32 time_out;
    /* JPGE */
#ifdef VENC_SUPPORT_JPGE
    hi_u8* jfif_hdr;
#endif

} strm_mng_input_info;

typedef struct {
    hi_u32 addr;                 /* Only save one address in block, use physical address */
    hi_u32 size;                 /* Size of block */
    hi_u8* vir_addr;             /* Start kernel virtual address. */
    hi_u32 nalu_type;
    hi_u32 invld_byte;           /* InvalidByteNum */
    hi_u32 pts0_ms;              /* pts */
    hi_u32 pts1_ms;
    hi_u32 ext_flag;                 /* just for omx flag */
    hi_u32 ext_fill_len;             /* just for omx Yuv buffer fillLen */
    hi_bool frame_end;
} strm_mng_buf;

typedef struct {
    /* paraset */
    hi_u32 para_set_len;
    hi_u32 para_set_real_len;
    hi_u32 *para_set_array;
    /* real stream */
    hi_u32* strm_buf_rp_vir_addr; /* phy addr for hardware */
    hi_u32* strm_buf_wp_vir_addr;
    hi_u32* stream_buf_addr_array;
    hi_u32* stream_buf_len_array;
    hi_u32 strm_buf_size;
    hi_u32 cur_strm_buf_size;
    hi_u32 stream_total_byte;

    venc_buffer stream_mmz_buf;
    venc_buffer slice_buf;
    strm_mng_buf *buf;

    /* common info */
    hi_u32 slice_idx;
    hi_u32 too_few_buffer_skip;
    hi_bool first_slc_one_frm;
    hi_u32 protocol;

#ifdef VENC_SUPPORT_JPGE
    venc_buffer *jpeg_mmz_buf;
#endif
    hi_bool i_frm_insert_by_save_stream;

    hi_u32 strm_buf_ext_len;
} strm_mng_stream_info;

typedef struct {
    hi_handle handle;
    hi_u32 protocol;
} handle_protocol;

hi_s32 strm_init(hi_void);
hi_s32 strm_deinit(hi_void);
hi_s32 strm_create(hi_u32 handle, strm_mng_stream_info *stream_info);
hi_s32 strm_destroy(hi_u32 handle);
hi_s32 strm_get_write_buffer(hi_u32 handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info);
hi_s32 strm_put_write_buffer(hi_u32 handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info);

hi_s32 strm_put_buffer_for_recode(hi_u32 handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);

hi_s32 strm_acq_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet);
hi_s32 strm_rls_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet);
hi_s32 strm_get_acquire_stream_info(handle_protocol *h_protocol, hi_u8* start_vir_addr,
    vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset *buf_off_set);
hi_s32 strm_get_release_stream_info(hi_handle handle, hi_u32 protocol, strm_mng_stream_info *stream_info,
    venc_ioctl_acquire_stream* st_acq_strm, vedu_efl_nalu* vedu_packet);
hi_s32 strm_get_release_stream_info_intar(hi_handle handle, hi_venc_stream *stream, vedu_efl_nalu* vedu_packet);

hi_s32 strm_check_buffer_size(hi_handle handle, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size);
hi_u32 strm_cal_slc_size(hi_handle handle, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size);
hi_s32 strm_get_strm_state(hi_handle handle, strm_mng_state* strm_stat_info);
hi_u32 strm_get_written_length(hi_handle handle);
hi_s32 strm_get_match_mode(hi_u32 protocol, hi_handle* strm_mng, hi_bool is_chip_idv500r001);

hi_u32 strm_get_bit_buf_size(HANDLE handle, hi_u32 *strm_buf_size, hi_u32 slc_split_en, hi_u32 luma_size,
    hi_u32 *ext_len);
hi_s32 strm_alloc_stream_buf(HANDLE handle, venc_buffer *stream_mmz_buf, strm_mng_alloc_info *alloc_info,
    hi_bool enable_rcn_ref_share_buf);
hi_s32 strm_rls_stream_buf(HANDLE handle, venc_buffer *stream_mmz_buf, hi_u32 protocol);

hi_s32 strm_alloc_write_read_ptr_buf(HANDLE handle, venc_buffer* wp_rp_mmz_buf);
hi_s32 strm_rls_write_read_ptr_buf(HANDLE handle, venc_buffer* wp_rp_mmz_buf);

#ifdef VENC_SUPPORT_JPGE
hi_s32 strm_jpge_write_header(HANDLE handle, strm_mng_input_info* input_info, strm_mng_stream_info *stream_info);
hi_s32 strm_put_jpge_write_buffer(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 strm_jpge_remove_header(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 strm_get_jpge_stream(hi_handle handle, strm_mng_stream_info *stream_info,
    vedu_efl_nalu *vedu_packet, hi_venc_stream *stream, venc_ioctl_buf_offset *buf_offset);
#endif

hi_s32 strm_reset_buffer(HANDLE handle, hi_u32 protocol);
hi_s32 strm_alloc_para_set_buf_addr(HANDLE handle, venc_buffer *para_set_buf, hi_u32 *para_set_addr);
hi_s32 strm_rls_para_set_buf_addr(HANDLE handle, venc_buffer *para_set_buf);

#endif

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#ifndef __DRV_RECYCLE_BUF_H__
#define __DRV_RECYCLE_BUF_H__

#include "strm_mng_intf.h"

/************************struct define***************************************/
#define HI_ERR_STRMMNG(fmt...)       HI_ERR_PRINT(HI_ID_VENC, fmt)
#define HI_FATAL_RECYCLEBUF(fmt...)     HI_FATAL_PRINT(HI_ID_VENC, fmt)

#define RECYCLEBUF_LOCK(sema, ret) do { \
        if (osal_sem_down_interruptible(&(sema))) \
        { \
            HI_ERR_STRMMNG("Lock fail"); \
            ret = HI_FAILURE; \
        } else { \
            ret = HI_SUCCESS; \
        } \
    } while (0)

#define RECYCLEBUF_UNLOCK(sema) do {osal_sem_up(&(sema));} while (0)

#define RECYCLEBUF_SPIN_LOCK(spinlock, ulFlags) \
    do \
    { \
        venc_drv_osal_lock((spinlock), &(ulFlags));\
    } while (0)

#define RECYCLEBUF_SPIN_UNLOCK(spinlock, ulFlags) \
    do \
    { \
        venc_drv_osal_unlock((spinlock), &(ulFlags));\
    } while (0)
/************************struct define***************************************/
/* Global parameter of this module */
typedef struct {
    hi_u16 is_init;
    hi_u16 inst_handle;           /* Allocate handle according to this number */
    osal_semaphore sem;         /* Semaphore ,use for global variable */
    osal_spinlock *spin_lock;          /* Spin lock */
    struct list_head inst_head;    /* Instance list head */
} recycle_buf_global;

typedef struct {
    hi_u32 phy_addr;     /* Loop-buffer's start phy addr */
    hi_void* base;          /* Loop-buffer's start vir addr */
    hi_u32 buf_len;      /* Loop-buffer total len, unit is byte */
    hi_u32 rsv_byte;     /* Judge loop-buffer is full or not, unit is byte */
    hi_u32 rd_head;      /* Loop-buffer read head offset, unit is byte */
    hi_u32 rd_tail;      /* Loop-buffer read tail offset, unit is byte */
    hi_u32 wr_head;      /* Loop-buffer write head offset, unit is byte */
    hi_u32 wr_tail;      /* Loop-buffer read tail offset, unit is byte */
} valg_crcl_buf;

/* Describe a buffer instance */
typedef struct {
    hi_handle buf;               /* Handle of this buffer instance */

    valg_crcl_buf st_recycle_buf;   /* Stream Buffer addr and wp/rp */
    vedu_efl_nalu st_chn_packet;    /* use to release the stream */

    hi_u32 get_try;          /* GetWriteBuf try times */
    hi_u32 get_ok;           /* GetWriteBuf ok times */
    hi_u32 put_try;          /* PutWriteBuf try times */
    hi_u32 put_ok;           /* PutWriteBuf ok times */
    hi_u32 recv_try;         /* GetReadBuf try times */
    hi_u32 recv_ok;          /* GetReadBuf ok times */
    hi_u32 rls_try;          /* PutReadBuf try times */
    hi_u32 rls_ok;           /* PutReadBuf ok times */

    hi_u32 strm_buf_ext_len;   /* use for reround buffer copy */

#ifdef HI_TEE_SUPPORT
    hi_bool tvp;
#endif
    osal_spinlock *spin_lock;          /* Spin lock */
    struct list_head inst_node;    /* Instance node */

} recycle_buf_init;

typedef hi_uchar addr_unit;  /* Addressing step of processor, unit of the address */
typedef hi_u32 cpu_word;   /* Word lenght of processor, unit of W/R operations */

/* Read/write loop-buffer's out data type */
typedef struct {
    hi_void* src[2]; /* 2 is size 0f src */
    hi_u32 len[2]; /* 2 is size 0f u32_len */
} valg_cb_info;

/************************function define***************************************/
hi_void recycle_buffer_init(hi_void);
hi_void recycle_buf_deinit(hi_void);
hi_s32 recycle_buf_create(hi_handle handle, strm_mng_inst_config *st_config);
hi_s32 recycle_buf_destroy(hi_handle handle);

hi_s32 recycle_buf_get_write_buf(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 recycle_buf_put_write_buf(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);

hi_s32 recycle_buf_put_buf_for_recode(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info);

hi_s32 recycle_buf_acq_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu* nalu);
hi_s32 recycle_buf_rls_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu* nalu);

hi_s32 recycle_buf_get_acq_strm_info(handle_protocol *h_protocol, hi_u8* start_vir_addr,
    vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset* st_buf_offset);
hi_s32 recycle_buf_get_rls_strm_info(hi_handle handle, hi_u32 protocol, strm_mng_stream_info *stream_info,
    venc_ioctl_acquire_stream* st_acq_strm, vedu_efl_nalu* vedu_packet);
hi_s32 recycle_buf_get_rls_strm_info_intar(hi_handle handle, hi_venc_stream *stream, vedu_efl_nalu* vedu_packet);

hi_s32 recycle_buf_check_buf_size(hi_handle handle, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size);
hi_u32 recycle_buf_cal_slc_size(hi_handle handle, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size);
hi_u32 recycle_buf_get_written_length(hi_handle handle);

hi_s32 recycle_buf_reset_buffer(hi_handle handle, hi_u32 protocol);
hi_u32 recycle_buf_get_bit_buf_size(hi_handle handle, hi_u32 stream_buf_size, hi_u32 slc_split_en,
    hi_u32 luma_size, hi_u32 *ext_len);
hi_s32 recycle_buf_alloc_stream_buf(hi_handle handle, venc_buffer *stream_mmz_buf,
    strm_mng_alloc_info *alloc_info, hi_bool enable_rcn_ref_share_buf);

hi_s32 recycle_buf_put_buf_for_recode(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info);
hi_s32 recycle_buf_get_stream_state(hi_handle handle, strm_mng_state* strm_stat_info);
hi_s32 recycle_buf_alloc_para_set_buf_addr(hi_handle handle, venc_buffer *para_set_buf, hi_u32 *para_set_addr);
hi_s32 recycle_buf_rls_para_set_buf_addr(hi_handle handle, venc_buffer* para_set_buf);

#ifdef VENC_SUPPORT_JPGE
hi_s32 recycle_buf_write_header(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info *stream_info);
hi_s32 recycle_buf_put_jpge_write_buffer(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 recycle_buf_jpge_remove_header(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 recycle_buf_get_jpge_stream(hi_handle handle, strm_mng_stream_info *stream_info,
    vedu_efl_nalu *vedu_packet,
    hi_venc_stream *stream, venc_ioctl_buf_offset *st_buf_offset);

#endif

#endif


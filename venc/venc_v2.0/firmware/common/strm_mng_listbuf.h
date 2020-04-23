/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2016-07-22
 */
#ifndef __DRV_LIST_BUF_H__
#define __DRV_LIST_BUF_H__

#include "hi_errno.h"
#include "strm_mng_intf.h"

/* Error Definition */
#define HI_ERR_LISTBUF_INVALID_PARA      (HI_ERR_VENC_INVALID_PARA)
#define HI_ERR_LISTBUF_NO_MEMORY         (HI_FAILURE)
#define HI_ERR_LISTBUF_BUFFER_FULL       (HI_FAILURE)
#define HI_ERR_LISTBUF_BUFFER_EMPTY      (HI_ERR_VENC_BUF_EMPTY)
#define HI_ERR_LISTBUF_FREE_ERR          (HI_FAILURE)
#define HI_ERR_LISTBUF_WRITE_FREE_ERR    (HI_FAILURE)
#define HI_ERR_LISTBUF_READ_FREE_ERR     (HI_FAILURE)
#define HI_ERR_LISTBUF_BUSY              (HI_ERR_VENC_INVALID_PARA)

#define HI_FATAL_LISTBUF(fmt...)     HI_FATAL_PRINT(HI_ID_VENC, fmt)
#define HI_ERR_LISTBUF(fmt...)       HI_ERR_PRINT(HI_ID_VENC, fmt)
#define HI_WARN_LISTBUF(fmt...)      HI_WARN_PRINT(HI_ID_VENC, fmt)
#define HI_INFO_LISTBUF(fmt...)      HI_INFO_PRINT(HI_ID_VENC, fmt)

#define LISTBUF_LOCK(sema, ret)  do { \
        if (osal_sem_down_interruptible(&(sema))) \
        { \
            HI_ERR_LISTBUF("Lock fail"); \
            ret = HI_FAILURE; \
        } else { \
            ret = HI_SUCCESS; \
        } \
    } while (0)
#define LISTBUF_UNLOCK(sema) do {osal_sem_up(&(sema));} while (0)

#define LISTBUF_SPIN_LOCK(spinlock, ulFlags) \
    do \
    { \
        venc_drv_osal_lock((spinlock), &(ulFlags));\
    } while (0)
#define LISTBUF_SPIN_UNLOCK(spinlock, ulFlags) \
    do \
    { \
        venc_drv_osal_unlock((spinlock), &(ulFlags));\
    } while (0) /* change end */

/************************ Static Structure Definition ************************/
typedef enum {
    LISTBUF_BLOCK_FREE = 0,
    LISTBUF_BLOCK_READING,
    LISTBUF_BLOCK_WRITING,
    LISTBUF_BLOCK_BUTT
} list_buf_block_status;


/* Describe a buffer block */
typedef struct {
    hi_u32 addr;                 /* Only save one address in block, use physical address */
    hi_u8* vir_addr;              /* Start kernel virtual address. */
    hi_u32 size;                 /* Size of block */
    hi_u32 nalu_type;
    hi_u32 invld_byte;               /* InvalidByteNum */
    hi_u32 fill_len;
    hi_u32 pts0_ms;                 /* pts */
    hi_u32 pts1_ms;
    hi_u32 ext_flag;
    hi_u32 ext_fill_len;
    hi_bool frame_end;
    struct list_head block_node;   /* Block list node */
    list_buf_block_status status; /* Status of block */
} list_buf_block;

/* Describe a buffer instance */
typedef struct {
    hi_handle buf;               /* Handle of this buffer instance */
    hi_u32 phy_addr;           /* Start physical address of the buffer instance. */
    hi_u8 *knl_vir_addr;      /* Start kerenl virtual address of the buffer instance. */
    hi_u32 para_set_phy_addr;
    hi_u8 *para_set_knl_vir_addr;      /* Start kerenl virtual address of the buffer instance. */
    hi_u32 size;            /* Size of the buffer instance */
    hi_u32 strm_head_free;        /* Head free size */
    hi_u32 strm_tail_free;        /* Tail free size */

    hi_u32 get_try;          /* GetWriteBuf try times */
    hi_u32 get_ok;           /* GetWriteBuf ok times */
    hi_u32 put_try;          /* PutWriteBuf try times */
    hi_u32 put_ok;           /* PutWriteBuf ok times */
    hi_u32 recv_try;         /* GetReadBuf try times */
    hi_u32 recv_ok;          /* GetReadBuf ok times */
    hi_u32 rls_try;          /* PutReadBuf try times */
    hi_u32 rls_ok;           /* PutReadBuf ok times */

#ifdef HI_TEE_SUPPORT
    hi_bool tvp;
#endif
    osal_spinlock *spin_lock;          /* Spin lock */
    struct list_head write_block_head;  /* Buffer manager block list head for get buf */
    struct list_head free_block_head;   /* Buffer manager block list head for put buf */
    struct list_head read_block_head;   /* Buffer manager block list head for aqcuire buf */
    struct list_head inst_node;    /* Instance node */
} list_buf_inst;

/* Global parameter of this module */
typedef struct {
    hi_u16 is_init;
    hi_u16 inst_handle;           /* Allocate handle according to this number */
    osal_semaphore sem;          /* Semaphore */
    osal_spinlock *spin_lock;          /* Spin lock */
    struct list_head inst_head;    /* Instance list head */
} buf_mng_global;

hi_void list_buf_init(hi_void);
hi_void list_buf_deinit(hi_void);

hi_s32 list_buf_create(hi_handle buf, strm_mng_inst_config *st_config);
hi_s32 list_buf_destroy(hi_handle buf);

hi_s32 list_buf_get_write_buf(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 list_buf_put_write_buf(hi_handle buf, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info);

hi_s32 list_buf_put_buf_for_recode(hi_handle buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);

hi_s32 list_buf_acquire_bit_stream(hi_handle buf, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet);
hi_s32 list_buf_release_bit_stream(hi_handle buf, strm_mng_stream_info *stream_info, vedu_efl_nalu* vedu_packet);

hi_s32 list_buf_get_acquire_stream_info(handle_protocol *h_protocol, hi_u8* start_vir_addr,
    vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset *st_buf_offset);
hi_s32 list_buf_get_release_stream_info(hi_handle buf, hi_u32 protocol, strm_mng_stream_info *stream_info,
    venc_ioctl_acquire_stream* st_acq_strm, vedu_efl_nalu* vedu_packet);

hi_s32 list_buf_get_release_stream_info_intar(hi_handle buf, hi_venc_stream *stream, vedu_efl_nalu* vedu_packet);

hi_s32 list_buf_check_buffer_size(hi_handle buf, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size);
hi_u32 list_buf_cal_slc_size(hi_handle buf, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size);

hi_s32 list_buf_get_strm_state(hi_handle buf, strm_mng_state* strm_stat_info);
hi_u32 list_buf_get_written_len(hi_handle buf);
hi_u32 list_buf_get_bit_buf_size(hi_handle buf, hi_u32 stream_buf_size, hi_u32 slc_split_en, hi_u32 luma_size,
    hi_u32 *ext_len);
hi_s32 list_buf_alloc_stream_buf(hi_handle buf, venc_buffer* stream_mmz_buf, strm_mng_alloc_info *alloc_info,
    hi_bool enable_rcn_ref_share_buf);
hi_s32 list_buf_alloc_para_set_buf_addr(hi_handle buf, venc_buffer *para_set_buf, hi_u32 *para_set_addr);
hi_s32 list_buf_rls_para_set_buf_addr(hi_handle buf, venc_buffer *para_set_buf);

hi_s32 list_buf_reset_buffer(hi_handle buf, hi_u32 protocol);

#ifdef VENC_SUPPORT_JPGE
hi_s32 list_buf_write_header(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info *stream_info);
hi_s32 list_buf_put_jpeg_write_buffer(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 list_buf_jpeg_remove_header(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info);
hi_s32 list_buf_get_jpeg_stream(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet,
    hi_venc_stream *stream, venc_ioctl_buf_offset *st_buf_offset);
#endif

#endif


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __STREAM_H__
#define __STREAM_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#define STREAM_EOS 2

typedef enum {
    STREAM_STATUS_WRITE = 0,
    STREAM_STATUS_FREE,
    STREAM_STATUS_READ,
} stream_buf_status;

typedef enum {
    STREAM_BUF_ALLOC_OUTER = 0,
    STREAM_BUF_ALLOC_INNER,
} stream_alloc_mode;

typedef struct {
    vfmw_stream_buf buf;
    stream_buf_status status;
    struct OS_LIST_HEAD node;
    hi_bool is_used;
} stream_block;

typedef struct {
    hi_bool is_used;
    hi_u32 chan_id;
    vfmw_adpt_type adpt_type;
    stream_alloc_mode mode;
    hi_u64 mem_dma_buf;
    hi_s64 mem_fd;
    UADDR start_phy_addr;
    hi_u64 start_kern_vir_addr;
    hi_u64 start_usr_vir_addr;
    hi_u32 length;
    hi_u32 write_offst;
    hi_u32 read_offst;
    hi_u32 try_read_cnt;
    hi_u32 read_ok_cnt;
    hi_u32 try_release_cnt;
    hi_u32 release_ok_cnt;
    hi_u32 try_dequeue_cnt;
    hi_u32 dequeue_ok_cnt;
    hi_u32 try_queue_cnt;
    hi_u32 queue_ok_cnt;
    hi_u32 used_len;
    hi_u32 free_len;
    hi_u32 freeze_size;
    hi_u32 raw_num;
    hi_u32 valid_pts_flag;
    hi_bool is_sec;
    hi_bool eos_flag;
    hi_handle vdec_handle;
    hi_handle ssm_handle;
    hi_bool en_idr_pts;
    hi_s64 idr_pts;
    stream_block **block;
    struct OS_LIST_HEAD list;
    hi_u32 evt_map;
    hi_u32 max_raw_num;
    OS_SEMA sema;
} stream_chan;

stream_chan *stream_get_chan(hi_u32 chan_id);

hi_void stream_init(vfmw_init_param *param);
hi_void stream_exit(hi_void);
hi_s32  stream_create_chan(hi_u32 chan_id, vfmw_chan_option *option);
hi_void stream_destroy_chan(hi_u32 chan_id);
hi_s32  stream_alloc_buffer(hi_u32 chan_id, hi_u64 *size);
hi_s32  stream_free_buffer(hi_u32 chan_id);
hi_s32  stream_bind_usr_addr(hi_u32 chan_id, hi_u64 usr_addr);
hi_s32  stream_queue_buffer(hi_u32 chan_id, vfmw_stream_buf *raw_buf);
hi_s32  stream_dequeue_buffer(hi_u32 chan_id, vfmw_stream_buf *raw_buf);
hi_s32  stream_read_buffer(hi_u32 chan_id, vfmw_stream_buf *buffer);
hi_s32  stream_release_buffer(hi_u32 chan_id, vfmw_stream_buf *buffer);
hi_s32  stream_send_eos(hi_u32 chan_id);
hi_s32  stream_reset(hi_u32 chan_id);
hi_s32  stream_get_es_info(hi_u32 chan_id, vfmw_stream_buf *raw_buf);
hi_s32  stream_attach_es_buf(hi_u32 chan_id, vfmw_stream_buf *raw_buf);
hi_s32  stream_set_idr_pts(hi_u32 chan_id, hi_s64 idr_pts);
hi_s32  stream_get_chan_status(hi_u32 chan_id, vfmw_chan_info *status);
hi_s32  stream_set_evt_map(hi_u32 chan_id, hi_u32 evt_map);
hi_s32  stream_config(hi_u32 chan_id, vfmw_chan_cfg *cfg);


#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi buf
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_BUF_H__
#define __VI_BUF_H__

#include "hi_drv_video.h"
#include "vi_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VI_BUF_ALIGN 4

typedef enum {
    VI_NODE_STATUS_EMPTY = 0,
    VI_NODE_STATUS_IDLE,
    VI_NODE_STATUS_ALONE,
    VI_NODE_STATUS_BUSY,
    VI_NODE_STATUS_USED,

    VI_NODE_STATUS_MAX
} vi_node_status;

typedef struct {
    hi_u32 empty_node_num;
    hi_u32 full_node_num;
    hi_u32 idle_node_num;
    hi_u32 alone_node_num;
    hi_u32 busy_node_num;
    hi_u32 used_node_num;
    hi_u32 user_node_num;

    hi_u32 alloc_cnt;
    hi_u32 free_cnt;

    hi_u32 thread_cnt;
} vi_buf_dbg_info;

typedef struct {
    vi_buffer_addr_info video_buf_addr;    /* video frame addr info */
    vi_buffer_addr_info dolby_md_buf_addr; /* dolby metadata addr info */
    vi_buffer_addr_info hdr_md_buf_addr;   /* coverted HDR frame metadata addr info */
} vi_frame_addr_info;

typedef struct {
    hi_u32 buf_size;
    hi_u32 min_buf_num;
    hi_u32 max_buf_num;
} vi_buf_attr;

typedef struct {
    struct osal_list_head list;

    hi_u32 index;
    hi_s32 reference;
    vi_node_status status;

    volatile hi_bool valid;

    hi_s32 src_fence;
    hi_s32 sink_fence;

    hi_drv_video_frame frame_info;
    vi_frame_addr_info frame_addr;
} vi_buf_node;

hi_s32 vi_buf_init(vi_buf_attr *buf_attr_p, hi_void **buf_handle_pp);
hi_s32 vi_buf_deinit(hi_void *buf_handle_p);

hi_s32 vi_buf_get_frame(hi_void *buf_handle_p, vi_node_status status, hi_drv_video_frame *frame_p);
hi_s32 vi_buf_put_frame(hi_void *buf_handle_p, vi_node_status status, hi_drv_video_frame *frame_p);
hi_s32 vi_buf_get_node(hi_void *buf_handle_p, vi_node_status status, vi_buf_node *node_p);
hi_s32 vi_buf_put_node(hi_void *buf_handle_p, vi_node_status status, vi_buf_node *node_p);

hi_s32 vi_buf_add(hi_void *buf_handle_p, vi_buf_node *node_p);
hi_s32 vi_buf_sub(hi_void *buf_handle_p, vi_buf_node *node_p);

hi_s32 vi_buf_check_busy(hi_void *buf_handle_p);
hi_u32 vi_buf_get_idle_cnt(hi_void *buf_handle_p);

hi_void vi_buf_proc_print(hi_void *buf_handle_p, hi_void *s);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __VI_BUF_H__ */

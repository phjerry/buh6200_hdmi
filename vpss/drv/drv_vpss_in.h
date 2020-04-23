/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_in.h hander file vpss in define
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __DRV_VPSS_IN_H__
#define __DRV_VPSS_IN_H__

#include "vpss_comm.h"
#include "drv_vpss_dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEF_IN_NODE_MAX_NUM          40
#define DEF_SRC_NODE_MAX_NUM         (DEF_IN_NODE_MAX_NUM * 2)
#define DEF_SRC_MODE_FRAME_NUMB      1
#define DEF_SRC_MODE_FIELD_NUMB      2
#define DEF_SRC_MODE_SPEC_FIELD_NUMB 1
#define DEF_SRC_MODE_NTSC_NUMB       10
#define DEF_SRC_MODE_PAL_NUMB        10
#define DEF_SRC_TILE_BLOCK_WIDTH     0

typedef hi_drv_vpss_video_frame vpss_src_data;
typedef hi_s32 (*fn_src_func)(hi_handle h_src, vpss_src_data *frame);

typedef struct {
    hi_drv_video_frame in_data;
    list node;
} vpss_in_node;

typedef struct {
    hi_handle h_src_module;
    fn_src_func rls_in_undo_priv_image;
} vpss_in_attr;

typedef struct {
    hi_bool init;
    hi_handle h_src_module;
    vpss_comm_spin src_in_lock;
    vpss_in_node src_node[DEF_IN_NODE_MAX_NUM];

    list free_list;     /* 空队列 */
    list busy_list;     /* 工作队列 */
    list wait_rel_list; /* 待还帧队列,此时已经从busy list中取出送给逻辑去处理，待逻辑处理完 */
    list release_list;  /* 还帧队列 */

    vpss_in_node *target;
    fn_src_func rls_in_undo_priv_image;

    hi_u32 queue_cnt;
    hi_u32 dequeue_cnt;
    hi_u32 acquire_cnt;
    hi_u32 release_cnt;
    hi_u32 queue_total_cnt;
    hi_u32 dequeue_total_cnt;
    hi_u32 acquire_total_cnt;
    hi_u32 release_total_cnt;
    hi_u32 try_decrease_ref;
    hi_u32 suc_decrease_ref;
    vpss_debug_info global_dbg_ctrl;
} vpss_in;

typedef enum {
    SRC_MODE_FRAME = 1,
    SRC_MODE_FIELD,
    SRC_MODE_SPEC_FIELD,
    SRC_MODE_NTSC,
    SRC_MODE_PAL,
    SRC_MODE_MAX
} vpss_src_mode;

typedef struct {
    vpss_src_data src_data;
    list node;
} vpss_src_node;

typedef struct {
    vpss_src_mode mode;
    hi_handle h_src_module;
    fn_src_func rls_src_priv_image;
} vpss_src_attr;

typedef struct {
    hi_bool init;
    hi_u32 buf_num;
    vpss_src_mode mode;
    list *src_target_1;
    vpss_src_node ast_src_node[DEF_IN_NODE_MAX_NUM];
    list empty_src_list;
    list ful_src_list;
    hi_handle h_src_module;
    fn_src_func rls_src_priv_image;
    hi_u32 put_src_count;
    hi_u32 put_src_total;
    hi_u32 complete_src_count;
    hi_u32 release_src_count;
    hi_u32 release_src_total;
    hi_u32 ful_src_list_num;

    hi_u32 try_decrease_ref;
    hi_u32 suc_decrease_ref;
} vpss_src;

hi_s32 vpss_src_init(vpss_src *src, vpss_src_attr attr);
hi_s32 vpss_src_deinit(vpss_src *src);
hi_s32 vpss_src_reset(vpss_src *src);
hi_s32 vpss_src_flush(vpss_src *src);

hi_s32 vpss_src_put_image(vpss_src *src, vpss_src_data *data);
hi_s32 vpss_src_get_process_image(vpss_src *src, vpss_src_data **data);
hi_s32 vpss_src_complete_priv_image(vpss_src *src);
hi_s32 vpss_src_move_next(vpss_src *src, hi_u32 next_count);
hi_s32 vpss_src_show_status(vpss_src *src);
hi_s32 vpss_in_init(vpss_in_attr *attr, vpss_in *in);
hi_s32 vpss_in_deinit(vpss_in *in);
hi_s32 vpss_in_reset(vpss_in *in);
hi_s32 vpss_in_queue_frame(vpss_in *in, hi_drv_video_frame *frame);
hi_s32 vpss_in_dequeue_frame(vpss_in *in, hi_drv_video_frame *frame);
hi_s32 vpss_in_acquire_frame(vpss_in *in, hi_drv_video_frame *frame);
hi_s32 vpss_in_release_frame(vpss_in *in, hi_drv_video_frame *frame);
hi_s32 vpss_in_get_buf_num(vpss_in *in, hi_u32 *out_buf_num);
hi_bool vpss_src_is_empty(vpss_src *src);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif






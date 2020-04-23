/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#ifndef __FENCE_H__
#define __FENCE_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#define FENCE_STM_NUM 16
#define FENCE_FRM_NUM 40

#define FENCE_INVALID  (-1)

typedef enum {
    FENCE_DISABLE = 0,
    FENCE_ENABLE = 1
} fence_flag;

typedef struct {
    hi_s32 is_valid;
    hi_s32 src_fence;
    hi_s32 sink_fence;
    hi_s32 stream_id;
    UADDR stm_addr;
} fence_stream;

typedef struct {
    hi_s32 is_valid;
    hi_s32 src_fence;
    hi_s32 sink_fence;
    hi_s32 rls_src_fence;
    hi_s32 image_id;
    hi_s32 image_id_1;
    hi_s32 share_idx;

    /* for debug */
    hi_u32 src_frm_cnt;
    hi_u32 src_chk_cnt;
    hi_u32 src_chk_start;
    hi_u32 src_chk_end;
    hi_u32 sink_frm_cnt;
    hi_u32 sink_chk_cnt;
    hi_u32 sink_chk_start;
    hi_u32 sink_chk_end;
} fence_frame;

typedef struct {
    hi_bool created;
    hi_s32 chan_id;
    hi_s32 fence_create;
    hi_s32 fence_signal;
    hi_s32 fence_wait;
    hi_s32 fence_free;
    fence_frame fence_frm[FENCE_FRM_NUM];

    /* for debug */
    hi_u32 src_frm_cnt;
    hi_u32 sink_frm_cnt;
} fence_chan;

typedef hi_s32 (*fn_fence_rls_image)(hi_s32 chan_id, const hi_void *image);
typedef hi_s32 (*fn_fence_chk_image)(hi_s32 chan_id, hi_void *image);

typedef struct {
    fn_fence_rls_image rls_image;
    fn_fence_chk_image chk_image;
} fence_init_param;

typedef struct {
    hi_bool inited;
    hi_bool fence_frm_en;
    hi_bool fence_stm_en;
    fn_fence_rls_image rls_image;
    fn_fence_chk_image chk_image;
    fence_chan *chan[VFMW_CHAN_NUM];

    /* for debug */
    hi_u32 src_wait_time;
    hi_u32 sink_wait_time;
} fence_entry;

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_s32 fence_init(fence_init_param *args);
hi_s32 fence_exit(hi_void);
hi_s32 fence_create_chan(hi_u32 chan_id, hi_void *);
hi_s32 fence_destroy_chan(hi_u32 chan_id);
hi_s32 fence_if_frm_enable(hi_u32 chan_id);
hi_s32 fence_set_frm_flag(hi_u32 chan_id, vfmw_image *img);
hi_s32 fence_add_rcv_frm(hi_u32 chan_id, hi_s32 *fence, vfmw_image *img);
hi_s32 fence_add_rls_frm(hi_u32 chan_id, hi_s32 src_fence, hi_s32 sink_fence, vfmw_image *img);
hi_s32 fence_proc_frm_state(hi_u32 chan_id);

fence_entry *fence_get_entry(hi_void);
fence_chan *fence_get_chan(hi_u32 chan_id);

#endif



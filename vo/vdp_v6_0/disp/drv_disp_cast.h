/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp cast
* Create: 2016-04-12
 */

#ifndef __DRV_DISP_CAST_H__
#define __DRV_DISP_CAST_H__

#include "hi_osal.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */


#define DISP_CAST_MIN_W 320
#define DISP_CAST_MAX_W 1920
#define DISP_CAST_MIN_H 240
#define DISP_CAST_MAX_H 1080

#define DISP_CAST_BUFFER_MIN_NUMBER 3
#define DISP_CAST_BUFFER_MAX_NUMBER 16

#define DISP_CAST_LOWDLY_THRESHOLD_NUMERATOR  15
#define DISP_CAST_LOWDLY_THRESHOLD_DENOMINATOR 100
#define DISP_CAST_LOWDLY_LINENUM_INTERVAL     10
/* in ms */
#define DISP_CAST_MAX_FRAME_RATE 3000

#define CAST_BUFFER_STATE_IN_CFGLIST_WRITING      1
#define CAST_BUFFER_STATE_IN_CFGLIST_WRITE_FINISH 2



typedef enum hi_cast_retrive_list_node_status_
{
    CAST_RETRIVE_NODE_EMPTY = 0,
    CAST_RETRIVE_NODE_REALLOCATE,
    CAST_RETRIVE_NODE_READY_TO_RETURN,
    CAST_RETRIVE_NODE_BUTT,
} cast_retrive_list_node_status;

typedef struct tag_disp_cast_attr
{
    HI_DRV_PIX_FORMAT_E format; /* Support ... */

	HI_RECT_S in;
	hi_bool interlace;
	hi_u32 in_rate;
    HI_DRV_COLOR_SPACE_E in_color_space;

	HI_RECT_S out;
	hi_u32 out_rate;
    HI_DRV_COLOR_SPACE_E out_color_space;

	// store output informaiton
	HI_DRV_VIDEO_FRAME_S frame_demo;

    hi_bool               crop;
    HI_DRV_CROP_RECT_S    crop_rect;
}disp_cast_attr;

typedef struct tag_disp_snapshot_priv_frame
{
    hi_u32 magic;                  /* NOTICE!!!sizeof(HI_DRV_VIDEO_PRIVATE_S)=288 bytes,however *
                                       * user space only copy 64*4=256 bytes.so put u32Magic ahead.*/
    HI_DRV_VIDEO_PRIVATE_S priv_info;
}disp_snapshot_priv_frame;

typedef enum
{
    MIRA_SET_CREATE_PTS = 0,
    MIRA_SET_AQUIRE_PTS,
    MIRA_FLAG_BUTT
}mira_get_pts;

typedef struct tag_disp_attach_pair
{
    /*push mode, get the func ptr from back mode.*/
    HI_HANDLE h_sink;
    hi_void* queue_frm;
    hi_void* dequeue_frame;
}disp_attach_pair;


#define DISPLAY_ATTACH_CNT_MAX   3

typedef struct tag_disp_cast
{
    //state
    hi_bool open;
    hi_bool enable;
    hi_bool masked;
    osal_atomic buf_busy;
    hi_bool attached;
	hi_u32 ref;

    hi_bool low_delay;
	/* wbc controll between cast and snapshot. */
	/* Should cast shedule wbc? default no.  */
	volatile hi_bool	schedule_wbc;
	volatile hi_bool	schedule_wbc_status;
    // cfg
    HI_DRV_DISP_CAST_CFG_S config;

    // disp info
    HI_DRV_DISPLAY_E disp;
    //hi_bool bDispSet;
    //hi_bool bDispUpdate;
    HI_DISP_DISPLAY_INFO_S disp_info;
    hi_bool to_get_disp_info;

    // private attr
    disp_wbc wbc;
    //hi_bool bAttrUpdate;
    disp_cast_attr attr;

    hi_u32 periods;
    hi_u32 task_count;

    //mirrorcast
    //DISP_MIRACAST_S stMrCt;

    //algrithm operation

    // buffer
    buf_pool bp;
    hi_u32 frame_cnt;
    hi_u32 last_cfg_buf_id;
    hi_u32 last_frame_buf_id;

    //component operation
    disp_intf_operation intf_opt;

    disp_attach_pair attach_pairs[DISPLAY_ATTACH_CNT_MAX];
    struct task_struct  *k_thread_release_frame;

    /*for cast proc infor stastics.*/
    hi_u32 cast_acquire_try_cnt;
    hi_u32 cast_acquire_ok_cnt;
    hi_u32 cast_release_try_cnt;
    hi_u32 cast_release_ok_cnt;

    hi_u32 cast_intr_cnt;
}disp_cast;


typedef struct tag_disp_snapshot
{
    hi_bool work;
    buf_pool bp;
}disp_snapshot;


typedef struct tag_cast_release_ptr
{
    osal_atomic  at_release_node_status;
    hi_u32   buf_id;
    hi_bool  internal_release;
}cast_release_ptr;

hi_s32 disp_cast_create(HI_DRV_DISPLAY_E disp,
                       HI_DISP_DISPLAY_INFO_S *info,
                       HI_DRV_DISP_CAST_CFG_S *cfg,
                       disp_cast** cast_ptr);
hi_s32 disp_cast_destroy(hi_void* h_cast);
hi_s32 disp_cast_set_enable(hi_void* cast_ptr, hi_bool enable);
hi_s32 disp_cast_get_enable(hi_void* cast_ptr, hi_bool *enable);
hi_s32 disp_cast_acquire_frame(disp_cast* h_cast, HI_DRV_VIDEO_FRAME_S *cast_frame);
hi_s32 disp_cast_release_frame(disp_cast* h_cast, HI_DRV_VIDEO_FRAME_S *cast_frame);
hi_s32 disp_cast_attach_sink(disp_cast* cast_ptr, HI_HANDLE h_sink);
hi_s32 disp_cast_de_attach_sink(disp_cast* cast_ptr, HI_HANDLE h_sink);

hi_s32 disp_cast_set_attr(disp_cast * cast_ptr, HI_DRV_DISP_Cast_Attr_S *cast_attr);
hi_s32 disp_cast_get_attr(disp_cast * cast_ptr, HI_DRV_DISP_Cast_Attr_S *cast_attr);

hi_s32 DISP_Acquire_Snapshot(HI_DRV_DISPLAY_E disp, hi_void **snapshot_handle, HI_DRV_VIDEO_FRAME_S *frame);
hi_s32 DISP_Release_Snapshot(HI_DRV_DISPLAY_E disp, hi_void* snapshot_handle, HI_DRV_VIDEO_FRAME_S *frame);
hi_s32 disp_snapshot_destroy(hi_void* snapshot_ptr);


hi_void disp_cast_cbset_disp_mode(disp_cast * h_cast, const HI_DRV_DISP_CALLBACK_INFO_S *info);

hi_void disp_cast_cb_genarate_frame(hi_void *cast_ptr, const HI_DRV_DISP_CALLBACK_INFO_S *info);
hi_void disp_cast_push_frame(hi_void *cast_ptr, const HI_DRV_DISP_CALLBACK_INFO_S *info);
hi_void DISP_CastGetDlyStatus(HI_HANDLE cast_ptr, hi_bool *low_dly);
#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /*  __DRV_DISP_X_H__  */











/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */
#ifndef __VIR_PP_H__
#define __VIR_PP_H__

#include "vfmw.h"
#include "vfmw_proc.h"
#include "hi_drv_video.h"
#include "hi_vdec_type.h"
#include "vfmw_osal.h"

#ifdef VFMW_FSP_MAX_FRAME_NUM
#define VPP_MAX_PRIVATE_NODE_NUM VFMW_FSP_MAX_FRAME_NUM
#define VPP_PVR_MAX_LIST_LEN     VFMW_FSP_MAX_FRAME_NUM
#else
#define VPP_MAX_PRIVATE_NODE_NUM 32
#define VPP_PVR_MAX_LIST_LEN     32
#endif

typedef enum {
    INVALID_STATE = 0,
    START_STATE = 1,
    STOP_STATE,
    STATE_BUTT
} vir_pp_chan_state;

typedef enum {
    THREA_IDLE = 0,
    THREA_RUNNING = 1,
    THREA_SLEEP,
    THREA_BUTT
} vpp_thread_state;

typedef hi_s32 (*fn_vpp_get_image)(hi_s32 chan_id, hi_void *image);
typedef hi_s32 (*fn_vpp_rls_image)(hi_s32 chan_id, const hi_void *image);

typedef struct {
    hi_s32 image_id;
    hi_s32 image_id_1;
    vfmw_vid_std std;
    hi_bool is_3d;
    UADDR phy_addr;
    hi_u32 width;
    hi_u32 height;
    UADDR phy_addr_1;
    UADDR meta_data_addr;
} vpp_pvr_image_info;

typedef struct {
    hi_drv_video_frame frame;
    hi_vdec_ext_frm_info ext_frm_info;
    vpp_pvr_image_info image_info;
    struct OS_LIST_HEAD node;
} vpp_pvr_list_node;

typedef struct {
    hi_s32 speed;
    vfmw_control_info ctrl_info;
    hi_bool get_first_frame;
    hi_bool get_first_i_frame;
    hi_s32  img_distnace;
    hi_s32 last_frame_gop_num;
    hi_u32 tmp_list_pos;
    struct OS_LIST_HEAD frame_list;
    vpp_pvr_list_node tmp_list[VPP_PVR_MAX_LIST_LEN];
    hi_u32 final_frame_num;
    OS_SEMA pvr_sema;
} vpp_pvr_info;

typedef struct {
    vfmw_private_frm_info vdec_private;
    hi_bool is_use;
} vpp_private_block;

typedef struct {
    hi_bool created;
    hi_s32 chan_id;
    hi_s32 failed_time;
    vir_pp_chan_state state;
    vfmw_vpp_work_mode work_mode;
    hi_s32 is_omx_path;
    vfmw_frm_alloc_type alloc_type;
    hi_drv_source source;
    fun_vfmw_event_report event_report;
    vpp_pvr_info pvr_info;
    hi_drv_3d_type ext_3d_type; /* set by outside */
    hi_drv_3d_type cur_3d_type; /* current 3d type */
    hi_s32 receive_try;
    hi_s32 receive_ok;
    hi_s32 release_try;
    hi_s32 release_ok;
    hi_s32 err_thr;
    vpp_private_block *vpp_private_node[VPP_MAX_PRIVATE_NODE_NUM];
    vfmw_dec_mode dec_mode;
    vfmw_image last_image;
    hi_u32 evt_map;
} vpp_chan;

typedef struct {
    hi_bool inited;
    OS_SEMA sema;
    OS_EVENT event;
    hi_void *thread;
    hi_u32 init_time;
    hi_u32 wait_time;
    fn_vpp_get_image get_image;
    fn_vpp_rls_image rls_image;
    vpp_thread_state thread_state;
    vpp_chan *chan[VFMW_CHAN_NUM];
    hi_bool bypass;
    hi_bool print_frm;
} vpp_entry;

typedef struct {
    fn_vpp_get_image get_image;
    fn_vpp_rls_image rls_image;
} vpp_init_param;

typedef struct {
    vfmw_vpp_work_mode work_mode;
    fun_vfmw_event_report event_report;
} vpp_create_param;

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
vpp_entry *vpp_get_entry(hi_void);
vpp_chan *vpp_get_chan(hi_u32 chan_id);
hi_s32 vpp_init(vpp_init_param *param);
hi_s32 vpp_deinit(hi_void);
hi_s32 vpp_create(hi_u32 chan_id, vpp_create_param *param);
hi_s32 vpp_destroy(hi_u32 chan_id);
hi_s32 vpp_start(hi_u32 chan_id);
hi_s32 vpp_stop(hi_u32 chan_id);
hi_s32 vpp_reset(hi_u32 chan_id);
hi_s32 vpp_config(hi_u32 chan_id, vfmw_chan_cfg *cfg);
hi_s32 vpp_receive_frame(hi_u32 chan_id, hi_void *frame, hi_void *ext_frm_info);
hi_s32 vpp_release_frame(hi_u32 chan_id, const hi_void *frame);
hi_s32 vpp_get_chan_status(hi_u32 chan_id, vfmw_chan_info *status);
hi_s32 vpp_set_pvr_speed(hi_u32 chan_id, hi_s32 speed);
hi_s32 vpp_set_pvr_ctrl_info(hi_u32 chan_id, vfmw_control_info *ctrl);
hi_s32 vpp_set_frm_3d_type(hi_u32 chan_id, hi_void *frm_3d_type);
hi_s32 vpp_get_frm_3d_type(hi_u32 chan_id, hi_void *frm_3d_type);
hi_s32 vpp_set_evt_map(hi_u32 chan_id, hi_u32 evt_map);

#endif



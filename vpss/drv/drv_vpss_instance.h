/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_instance.h hander file vpss instance define
 * Author: zhangjunyu
 * Create: 2016/06/13
 */
#ifndef __DRV_VPSS_INSTANCE_H__
#define __DRV_VPSS_INSTANCE_H__

#include "vpss_comm.h"
#include "hi_drv_vpss.h"
#include "drv_pq_ext.h"
#include "vpss_define.h"
#include "hal_vpss.h"
#include "drv_vpss_in.h"
#include "drv_vpss_out.h"
#include "hal_vpss_wbc.h"
#include "hal_vpss_stt.h"
#include "drv_vpss_dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef hi_s32 (*fn_vpss_src_priv_func)(hi_handle h_vpss, hi_drv_vpss_video_frame *frame);

typedef struct {
    hi_handle h_sink;
    hi_drv_vpss_callback_func dst_func;
} vpss_sink_func;

#define VPSS_FIELD_ORDER_UNKNOWN 2

typedef struct {
    hi_s32 port_id;
    hi_bool enble;

    hi_drv_vpss_port_type port_type;
    hi_drv_vpss_work_mode vpss_work_mode;

    hi_bool user_crop_en;
    hi_drv_crop_rect offset_rect;
    hi_drv_rect crop_rect;
    hi_drv_rect video_rect;
    hi_drv_rect out_rect;
    hi_bool tunnel_enable;
    hi_bool enable_3d;

    hi_drv_rect revise_crop_rect;
    hi_drv_rect revise_video_rect;
    hi_drv_rect revise_out_rect;

    hi_drv_pixel_format format;
    hi_drv_compress_info cmp_info;
    hi_drv_pixel_bitwidth bit_width;

    hi_drv_vpss_buffer_type buf_type;
    hi_drv_vpss_rotation rotation;
    hi_bool hori_flip;
    hi_bool vert_flip;

    /* 不管做不做DEI处理，输出帧率都按照原始帧率输出，此时还会丢一半帧 */
    hi_bool ori_frame_rate;
    hi_u32 max_frame_rate; /* in 1/100 HZ */

    hi_bool hdr_follow_src;
    hi_drv_hdr_type out_hdr_type;
    hi_drv_color_descript out_color_desp;

    hi_u32 out_count;
    vpss_out out_list;
    hi_handle h_dst_module;
    hi_drv_vpss_callback_func call_back_func;
    vpss_sink_func sink_func;
} vpss_port;

typedef enum {
    VPSS_FRAME_STATUS_ARRIVAL = 0,
    VPSS_FRAME_STATUS_BACK,
    VPSS_FRAME_STATUS_BUTT
}vpss_frame_state;

typedef enum {
    VPSS_OUTPUT_FRAME_FIRST = 0,
    VPSS_OUTPUT_FRAME_SECOND,
    VPSS_OUTPUT_FRAME_THIRD,
    VPSS_OUTPUT_FRAME_MAX
} vpss_output_frame_num;

typedef enum {
    INSTANCE_STATE_IDLE = 0,
    INSTANCE_STATE_PREPARE,
    INSTANCE_STATE_CONFIG,
    INSTANCE_STATE_START,
    INSTANCE_STATE_WAIT,
    INSTANCE_STATE_SUCCESS,
    INSTANCE_STATE_CLEAR,
    INSTANCE_STATE_MAX
} vpss_instance_state;

typedef enum {
    VPSS_SOURCE_MODE_USERACTIVE = 0,
    VPSS_SOURCE_MODE_VPSSACTIVE,
    VPSS_SOURCE_MODE_MAX
} vpss_source_mode;

typedef struct {
    fn_vpss_src_priv_func vpss_acq_in_priv_image;
    fn_vpss_src_priv_func vpss_rel_in_priv_image;
    hi_handle h_src;
    fn_vpss_src_func vpss_complete_in_comm_image;
    fn_vpss_get_ai_cfg_by_frame get_ai_cfg_by_frame;
} vpss_source_func;

typedef struct {
    vpss_hal_3drs_type en3_drs;
    hi_u32 me_scan_num;
    hi_drv_pq_vpss_alg_enable alg_ctrl;
} vpss_alg_state;

typedef struct {
    hi_drv_rect lbx_data;
    hi_drv_pq_vpss_info pq_timing;
    vpss_alg_state alg_state;
    hi_drv_pq_vpss_stt_info alg_in_para;
    hi_drv_pq_vpss_cfg_info alg_out_para;
} vpss_alg_info;

typedef struct {
    hi_bool vpss_process;
    hi_bool force_pq_close; /* control tnr/snr/db/dm, no dei/hdr */

    hi_bool secure;
    hi_bool new_scene;
    hi_bool top_first;
    hi_bool real_topfirst;
    hi_bool interlace;
    hi_bool special_field;
    hi_u32 in_rate;
    hi_u32 height;
    hi_u32 width;
    hi_drv_color_sys color_sys;
    hi_drv_3d_type video_3d_type;
    hi_drv_source source;
    hi_drv_field_mode field_mode;
    hi_drv_field_mode pre_fieldmode;
    hi_drv_pixel_format pixel_format;
    hi_drv_compress_info cmp_info;
    hi_drv_pixel_bitwidth bit_width;
    hi_drv_vpss_video_frame orignal_frame;

    hi_drv_hdr_type src_hdr_type;
    hi_drv_color_descript src_color_desp;
} vpss_stream_info;

typedef struct {
    hi_u32 time_err_cnt; /* wait fail times */
    hi_u32 time_warn_cnt; /* wait time longger than 16ms times */
    hi_u32 err_cnt;

    hi_u64 time_start;
    hi_u64 time_end;

    hi_u64 time_warn_start;
    hi_u64 time_warn_end;
} vpss_fence_dfx;

typedef struct {
    hi_u32 queue_cnt;
    hi_u32 queue_suc_cnt;
    hi_u32 dequeue_cnt;
    hi_u32 dequeue_suc_cnt;
} vpss_in_dfx;

typedef struct {
    hi_u32 queue_cnt;
    hi_u32 queue_suc_cnt;
    hi_u32 dequeue_cnt;
    hi_u32 dequeue_suc_cnt;
    hi_u32 acquire_cnt;
    hi_u32 acquire_suc_cnt;
    hi_u32 release_cnt;
    hi_u32 release_suc_cnt;
    hi_u32 queue_trans_cnt;
    hi_u32 queue_trans_suc_cnt;
    hi_u32 release_trans_cnt;
} vpss_out_dfx;

typedef struct {
    hi_u32 get_try_rate;
    hi_u32 get_suc_rate;

    hi_u32 get_try_cnt;
    hi_u32 get_suc_cnt;
} vpss_img_dfx;

typedef struct {
    hi_u32 rel_cnt;
    hi_u32 rel_complete_cnt;
    hi_u32 rel_complete_suc_cnt;
} vpss_src_dfx;

typedef struct {
    hi_u32 try_rate;
    hi_u32 try_suc_rate;
    hi_u32 try_cnt;
    hi_u32 try_suc_cnt;
    hi_u64 last_check_time;

    hi_u64 last_task_time;
    hi_u64 task_time_start;
    hi_u64 task_time_end;
} vpss_check_dfx;

typedef struct {
    hi_u32 try_rate;
    hi_u32 try_suc_rate;
    hi_u32 try_cnt;
    hi_u32 try_suc_cnt;
    /* map umap time cnt */
    hi_u32 try_map_cnt;
    hi_u32 map_suc_cnt;
    hi_u32 try_umap_cnt;
    hi_u32 umap_suc_cnt;

    hi_u64 buf_acq_time_start;
    hi_u64 buf_acq_time_end;

    hi_u64 buf_create_time_start;
    hi_u64 buf_create_time_end;

    hi_u64 buf_destroy_time_start;
    hi_u64 buf_destroy_time_end;

    hi_u64 dq_time_start;
    hi_u64 dq_time_end;
} vpss_buf_dfx;

typedef struct {
    hi_u32 source_frame_drop_cnt;
    hi_u32 scence_chg_cnt;
    hi_u32 reset_cnt;
    hi_u32 frame_drop_count;
    hi_u32 time_out_count;
    hi_u32 error_count;
} vpss_cnt_dfx;

typedef struct {
    hi_u32 try_inc_cnt;
    hi_u32 try_dec_cnt;
    hi_u32 try_inc_suc_cnt;
    hi_u32 try_dec_suc_cnt;
} vpss_refcnt_dfx;

typedef struct {
    hi_u32 task_2d_err_cnt;
    hi_u32 task_3d_err_cnt;
    hi_u32 task_rota_err_cnt;

    hi_u64 task_time_start;
    hi_u64 task_time_end;
    hi_u64 task_2d_time_start;
    hi_u64 task_2d_time_end;
    hi_u64 task_rota_time_start;
    hi_u64 task_rota_time_end;
    hi_u64 task_3d_time_start;
    hi_u64 task_3d_time_end;
    hi_u64 fill_hal_time_start;
    hi_u64 fill_hal_time_end;
    hi_u64 set_node_time_start;
    hi_u64 set_node_time_end;
    hi_u64 vir_node_time_start;
    hi_u64 vir_node_time_end;
} vpss_config_dfx;

typedef struct {
    hi_u64 task_time_start;
    hi_u64 task_time_end;
} vpss_prepare_dfx;

typedef struct {
    hi_u64 task_time_start;
    hi_u64 task_time_end;
} vpss_start_dfx;

typedef struct {
    hi_u64 task_time_start;
    hi_u64 task_time_end;

    hi_u64 send_out_time_start;
    hi_u64 send_out_time_end;

    hi_u64 send_out_start[VPSS_PORT_MAX_NUM];
    hi_u64 send_out_end1[VPSS_PORT_MAX_NUM];
    hi_u64 send_out_end2[VPSS_PORT_MAX_NUM];
    hi_u64 send_out_end3[VPSS_PORT_MAX_NUM];
    hi_u64 send_out_q_end4[VPSS_PORT_MAX_NUM];

    hi_u64 rel_src_time_start;
    hi_u64 rel_src_time_end;

    hi_u64 complete_time_start;
    hi_u64 complete_time_end;
} vpss_complete_dfx;

typedef struct {
    hi_u64 task_time_start;
    hi_u64 task_time_end;
} vpss_logic_dfx;

typedef struct {
    hi_u64 stt_time_start;
    hi_u64 stt_time_end;
    hi_u64 init_time_start;
    hi_u64 init_time_end;
} vpss_pq_dfx;

typedef struct {
    vpss_in_dfx in_dfx;
    vpss_out_dfx out_dfx;
    vpss_src_dfx src_dfx;
    vpss_check_dfx check_dfx;
    vpss_img_dfx img_dfx;
    vpss_buf_dfx buf_dfx;

    vpss_cnt_dfx cnt_dfx;
    vpss_refcnt_dfx refcnt_dfx;
    vpss_fence_dfx fence_dfx;

    vpss_config_dfx config_dfx;
    vpss_prepare_dfx prepare_dfx;
    vpss_start_dfx start_dfx;
    vpss_logic_dfx logic_dfx;
    vpss_complete_dfx complete_dfx;

    vpss_pq_dfx pq_dfx;
} vpss_inst_perfor;

typedef struct {
    hi_u64 proc_check_time;
    hi_u64 proc_prepare_time;
    hi_u64 proc_config_time;
    hi_u64 proc_start_time;
    hi_u64 proc_logic_time;
    hi_u64 proc_complete_time;
    hi_u64 proc_complete_send_time;
    hi_u64 proc_complete_send1_time[VPSS_PORT_MAX_NUM];
    hi_u64 proc_complete_send2_time[VPSS_PORT_MAX_NUM];
    hi_u64 proc_complete_send3_time[VPSS_PORT_MAX_NUM];
    hi_u64 proc_complete_send_q_time[VPSS_PORT_MAX_NUM];

    hi_u64 proc_complete_rel_time;
    hi_u64 proc_complete_complete_time;
    hi_u64 proc_total_time;

    hi_u64 proc_pq_stt_time;
    hi_u64 proc_fence_time;
} vpss_inst_proc_perf;

typedef struct {
    hi_u32 pts_data;
    hi_u32 frame_index;
    hi_bool repeat_first_field;
    hi_bool top_field_first;
    hi_drv_field_mode field_mode;
    list node;
} vpss_pts_node;

typedef struct {
    hi_bool enable;
    hi_bool use_pq_module;
    hi_bool cfg_new;
    hi_bool quick_out;

    hi_drv_vpss_prog_detect prog_detect;
    hi_drv_vpss_work_mode work_mode;
    hi_drv_vpss_pq_mode pq_mode;

    /* hi_bool user_create; */
    hi_bool suspend;
    hi_bool need_src_flip;
    hi_bool pause;
    hi_bool hold;
    hi_u32 id;
    hi_u32 ctrl_id;
    vpss_instance_state state;
    hi_bool prepareing;
    hi_bool user_crop_en;
    hi_drv_crop_rect offset_rect;
    hi_drv_rect src_crop_rect;
    vpss_comm_spin usr_set_spin;
    vpss_comm_spin src_spin;
    hi_drv_vpss_instance_cfg usr_inst_cfg;
    hi_drv_vpss_port_cfg usr_port_cfg[VPSS_PORT_MAX_NUM];
    /* hi_drv_vpss_scenemode scene_mode; */
    vpss_stream_info stream_info;
    vpss_in in_list;
    vpss_src src_list;
    vpss_wbc wbc_list[VPSS_BUFFER_MAX];
    vpss_stt stt_list[VPSS_BUFFER_MAX];
    vpss_port port[VPSS_PORT_MAX_NUM];
    vpss_buffer ro_buf[VPSS_PORT_MAX_NUM];
    vpss_alg_info alg_info;
    vpss_hal_info hal_info;
    vpss_out_node *out_frm_node[VPSS_PORT_MAX_NUM];
    /* user event */
    hi_handle h_dst;
    fn_vpss_event_callback event_callback;

    vpss_source_func src_funcs;
    hi_drv_vpss_video_frame rev_new_frame;
    vpss_inst_perfor perfor;
    vpss_inst_proc_perf proc_perfor;
    vpss_debug_info dbg_ctrl;
    vpss_debug_info *global_dbg_ctrl;
    vpss_pts_node pts_node[VPSS_PTS_NODE_NUM];
    list *first_ref;
    hi_drv_vpss_video_frame cur_process_frame_info;
    hi_drv_vpss_video_frame tran_release_frame_info;
    hi_bool logic_work;

    vpss_perfor_alg_ctrl vpss_alg_ctrl;
} vpss_instance;

typedef struct {
    hi_bool vpss_frame_arrival;
    hi_bool vpss_frame_back;
}vpss_frame_status;

hi_s32 vpss_inst_init(vpss_instance *instance, hi_drv_vpss_instance_cfg *instance_cfg);
hi_void vpss_inst_deinit(vpss_instance *instance);
hi_s32 vpss_inst_get_def_inst_cfg(hi_drv_vpss_instance_cfg *instance_cfg);
hi_s32 vpss_inst_set_inst_cfg(vpss_instance *instance, hi_drv_vpss_instance_cfg *instance_cfg);
hi_u32 vpss_inst_get_inst_cfg(vpss_instance *instance, hi_drv_vpss_instance_cfg *instance_cfg);
hi_s32 vpss_inst_create_port(vpss_instance *instance, hi_drv_vpss_port_cfg *port_cfg, hi_handle *ph_port);
hi_s32 vpss_inst_destory_port(vpss_instance *instance, hi_handle h_port);
hi_u32 vpss_inst_get_def_port_cfg(hi_drv_vpss_port_cfg *port_cfg);
hi_s32 vpss_inst_get_port_cfg(vpss_instance *instance, hi_handle h_port, hi_drv_vpss_port_cfg *port_cfg);
hi_s32 vpss_inst_set_port_cfg(vpss_instance *instance, hi_handle h_port, hi_drv_vpss_port_cfg *port_cfg);
hi_s32 vpss_inst_check_port_cfg(vpss_instance *instance, hi_drv_vpss_port_cfg *vpss_port_cfg);
hi_s32 vpss_inst_enable_port(vpss_instance *instance, hi_handle h_port, hi_bool port_en);
hi_s32 vpss_inst_reply_user_command(vpss_instance *instance, hi_drv_vpss_user_command command, hi_void *args);
hi_s32 vpss_inst_check_image(vpss_instance *instance, hi_drv_video_frame *frame);
hi_s32 vpss_inst_queue_frame(vpss_instance *instance, hi_drv_video_frame *frame);
hi_s32 vpss_inst_dequeue_frame(vpss_instance *instance, hi_drv_video_frame *frame);
hi_s32 vpss_inst_rel_port_frame(vpss_instance *instance, hi_handle h_port, hi_drv_video_frame *frame);
hi_s32 vpss_inst_get_port_frame(vpss_instance *instance, hi_handle h_port, hi_drv_video_frame *frame);

hi_s32 vpss_inst_regist_hook_event(vpss_instance *instance, hi_handle dst_id, fn_vpss_event_callback event_callback);
hi_s32 vpss_inst_regist_callback(vpss_instance *instance, hi_handle h_port, hi_handle h_sink,
                                 hi_handle h_src, hi_drv_vpss_callback_func *fn_vpss_callback);
hi_bool vpss_inst_check_inst_available(vpss_instance *instance, vpss_frame_status *frame_status);
hi_s32 vpss_inst_prepare_task(vpss_instance *instance);
hi_s32 vpss_inst_config_task(vpss_instance *instance);
hi_void vpss_inst_clear_task(vpss_instance *instance);
hi_s32 vpss_inst_complete_task(vpss_instance *instance);
hi_bool vpss_inst_update_state(vpss_instance *instance, vpss_instance_state state);
hi_s32 vpss_inst_create_proc(vpss_instance *instance);
hi_void vpss_inst_set_hal_frame_info(hi_drv_vpss_video_frame *frame,
    vpss_hal_frame *hal_frm, hi_drv_3d_eye_type buf_lr);
hi_void vpss_inst_send_frame(vpss_instance *instance);
hi_void vpss_inst_release_frame(vpss_instance *instance);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif






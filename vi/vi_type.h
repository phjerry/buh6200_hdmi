/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi type
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_TYPE_H__
#define __VI_TYPE_H__

#include "hi_type.h"
#include "hi_osal.h"

#include "hi_drv_vi.h"
#include "vi_buf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef hi_s32 (*vi_fun)(hi_void *arg);

typedef enum {
    VI_INSTANCE_STATUS_DEINIT = 0,
    VI_INSTANCE_STATUS_INIT,
    VI_INSTANCE_STATUS_CREATE,
    VI_INSTANCE_STATUS_ATTACH,
    VI_INSTANCE_STATUS_START,

    VI_INSTANCE_STATUS_MAX
} vi_instance_status;

typedef enum {
    VI_PROC_DBG_COMMAND_BUFNUM = 0,
    VI_PROC_DBG_COMMAND_FRMRATE,
    VI_PROC_DBG_COMMAND_CROP,
    VI_PROC_DBG_COMMAND_GRAPHICMODE,
    VI_PROC_DBG_COMMAND_BYPASSVPSS,
    VI_PROC_DBG_COMMAND_BITWIDTH,
    VI_PROC_DBG_COMMAND_MAX,
} vi_proc_dbg_command;

typedef struct {
    hi_bool user_set_graphic;    /* 调试 proc命令设置，true:表示执行用户设置，false:不执行 */
    hi_bool save_graphic_status; /* 调试 通路默认备份配置，用于恢复默认设置 */
    hi_bool user_graphic_status; /* 调试 proc命令即将设置的值 */

    hi_bool user_set_by_pass_vpss;    /* 调试 proc命令设置，true:表示执行用户设置，false:不执行 */
    hi_bool save_by_pass_vpss_status; /* 调试 通路默认备份配置，用于恢复默认设置 */
    hi_bool user_by_pass_vpss_status; /* 调试 proc命令即将设置的值 */

    hi_bool user_set_crop;
    hi_drv_rect user_crop_rect;

    hi_bool user_set_alloc_buf;
    hi_u32 user_buf_num;

    hi_bool user_set_frame_rate;
    hi_u32 user_frame_rate_num;

    hi_bool user_set_bit_width;
    hi_drv_pixel_bitwidth user_bit_width;

    /*-----------------------------*/
    hi_vi_dbg_info user_info; /* 用户态数据 */

    /*-----------------------------*/
    hi_u32 prepare_status;
    hi_u32 send_status;
    hi_u32 cfg_status;
    hi_u32 complete_cnt;
    hi_u32 send_cnt;          /* 发送帧数 调试用 */
    hi_u32 frm_lost_cnt;      /* 帧或场丢弃数 */
    hi_u32 top_lost_cnt;      /* 顶场丢弃数 */
    hi_u32 bot_lost_cnt;      /* 底场丢弃数 */
    hi_u32 same_field_cnt;    /* 隔行场景:同场计数，调试用 */
    hi_u32 first_not_top_cnt; /* 隔行场景:第一场不是顶场，调试用 */
    hi_u32 get_fail_cnt;      /* 获取视频帧失败次数 */

    /*-----------------------------*/
    hi_u32 sequence; /* 帧序列 */
} vi_instance_dbg_data;

typedef struct {
    hi_vi_access_type access;
    hi_vi_oversample over_sample;
    hi_vi_intf_mode intf_mode;

    hi_drv_source source_type;
    hi_drv_hdr_type hdr_type;
    hi_drv_3d_type video_3d_type;
    hi_drv_pixel_format pix_fmt;
    hi_drv_color_descript color_descript;
    hi_drv_color_sys color_sys;
    hi_drv_pixel_bitwidth bit_width;

    hi_u32 width;
    hi_u32 height;
    hi_bool interlace;
    hi_u32 rate;
    hi_u32 vblank;
} vi_in_attr;

typedef struct {
    hi_drv_hdr_type hdr_type;
    hi_drv_3d_type video_3d_type;
    hi_drv_pixel_format pix_fmt;
    hi_drv_color_descript color_descript;
    hi_drv_color_sys color_sys;
    hi_drv_pixel_bitwidth bit_width;
    hi_u32 width;
    hi_u32 height;
    hi_bool interlace;
    hi_u32 frame_rate;
    hi_u32 y_stride;
    hi_u32 c_stride;
} vi_out_attr;

typedef struct {
    hi_vi_nonstd_info nstd_info;
    hi_drv_field_mode disp_field_mode;
    hi_vi_quantization_range quantization_range;

    hi_drv_rect crop_rect;

    hi_bool vpss_bypass;
    hi_bool low_delay;

    hi_bool check_dolby;
    hi_bool check_hdr;
} vi_ctrls_attr;

typedef struct {
    hi_handle vi_handle; /* vi handle */
    hi_handle vo_handle; /* vo handle */
    vi_instance_status vi_status;

    hi_void *host_handle_p; /* host instance pointer */
    hi_void *buf_handle_p;  /* buffer instance pointer */
    hi_void *uf_handle_p;   /* uf instance pointer */

    vi_instance_dbg_data dbg_data;

    osal_wait poll_wait_queue;
    hi_u32 lost_frame_key;

    hi_drv_field_mode last_field_mode;
    hi_drv_hdr_type last_src_hdr_type;

    vi_buf_node done_node_set;
    vi_buf_node writing_node_set;
    vi_buf_node cfg_node_set;

    hi_vi_attr attr_orig;     /* original attr */
    vi_in_attr attr_in;       /* used attr */
    vi_out_attr attr_out;     /* out attr */
    vi_ctrls_attr attr_ctrls; /* ctrls attr will effect output */
} vi_instance;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

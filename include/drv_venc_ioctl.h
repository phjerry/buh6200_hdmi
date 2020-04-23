/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-07-18
 */

#ifndef __DRV_VENC_IOCTL_H__
#define __DRV_VENC_IOCTL_H__

#include "hi_debug.h"
#include "hi_drv_module.h"
#include "hi_venc_type.h"
#include "drv_venc_ext.h"
#include "drv_venc_ratecontrol.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VENC_DEV_NAME "hi_venc"
#define VENC_DEV_PATH "/dev/hi_venc"

typedef struct {
    hi_u32 strm_buf_offset[2]; /* 2:buffer to recycle */
} venc_ioctl_buf_offset;

typedef struct {
    void* strm_buf_virt_addr;
    hi_u32 strm_buf_size;
    void* slice_buf_virt_addr;
    hi_u32 slice_buf_size;
    void* strm_buf_virt_addr_jpeg;
    hi_u32 strm_buf_size_jpeg;
} hi_ioctl_chan_virt_addr;

typedef struct {
    hi_s32 strm_buf_fd;
    hi_u64 strm_buf_phy_addr;
    hi_s32 jpeg_buf_fd;
    hi_u64 strm_buf_phy_addr_jpeg;
    hi_s32 slice_buf_fd;
    hi_u64 slice_buf_phy_addr;
    hi_u32 buf_size;
    hi_u32 buf_size_jpeg;
    hi_u32 buf_size_slice;
    hi_u32 protocol;
    hi_handle handle;
    hi_handle source_handle;
} hi_ioctl_strm_buf_info;

typedef struct {
    hi_handle chan_id;
    hi_bool omx_chan;
    hi_venc_chan_info attr;
    hi_ioctl_strm_buf_info strm_buf_info;
} venc_ioctl_create;

typedef struct {
    hi_handle chan_id;
    hi_handle source_handle;
    hi_mod_id mode_id;
} venc_ioctl_attach;

typedef struct {
    hi_handle chan_id;
    hi_venc_stream stream;
    hi_u32 block_flag;
    venc_ioctl_buf_offset buf_offset;
    hi_u64 omx_stream_buf;
    hi_u32 h264_stream_off;
    hi_u32 h264_slice_off;
    hi_u32 jpeg_phy_addr;
} venc_ioctl_acquire_stream;

typedef struct {
    hi_handle chan_id;
    hi_drv_video_frame venc_frame;
    hi_venc_user_buf venc_frame_omx;
} venc_ioctl_queue_frame;

typedef struct {
    hi_handle chan_id;
    drv_venc_source_handle venc_source;
} venc_ioctl_set_source;

typedef struct {
    hi_handle chan_id;
    hi_venc_control_rate_type rc_type;
} venc_ioctl_rate_control;

typedef struct {
    hi_handle chan_id;
    hi_venc_frm_rate_type frm_rate_type;
} venc_ioctl_frame_rate_type;

/* just for omxvenc */
typedef struct {
    hi_handle chan_id;
    hi_venc_msg_info msg_info_omx;
} venc_ioctl_get_msg;

typedef struct {
    vedu_rc rc_data;
    hi_s32 rc_calculate_info;
}venc_ioctl_rc_info;

typedef struct {
    hi_handle chan_id;
    hi_venc_port_dir dir;
    hi_mem_handle_t buf_handle;
    hi_u64 phys_addr;
    hi_u8 *virt_addr;
    hi_mem_size_t buf_size;
} venc_ioctl_mmz_map;

typedef struct {
    hi_u64 vir_to_phy_offset;
    hi_u32 chan_id;
    hi_venc_port_dir dir;
    hi_mem_handle_t buffer_handle;
    hi_u32 phy_addr;
    hi_u32 buf_size;
}venc_ioctl_mmz_phy;

typedef struct {
    hi_handle chan_id;
    hi_u32 port_index;
    hi_u32 inter_flush;
} venc_ioctl_flush_port;

typedef struct {
    hi_handle chan_id;
    hi_venc_chan_config config;
} venc_ioctl_chan_info;

#define CMD_VENC_GET_CHN_ATTR _IOWR(HI_ID_VENC, 1, venc_ioctl_create)

#define CMD_VENC_CREATE_CHN _IOWR(HI_ID_VENC, 2, venc_ioctl_create)
#define CMD_VENC_DESTROY_CHN _IOWR(HI_ID_VENC, 3, venc_ioctl_create)

#define CMD_VENC_ATTACH_INPUT _IOW(HI_ID_VENC, 4, venc_ioctl_attach)
#define CMD_VENC_DETACH_INPUT _IOWR(HI_ID_VENC, 5, venc_ioctl_attach)

#define CMD_VENC_ACQUIRE_STREAM _IOWR(HI_ID_VENC, 6, venc_ioctl_acquire_stream)
#define CMD_VENC_RELEASE_STREAM _IOW(HI_ID_VENC, 7, venc_ioctl_acquire_stream)

#define CMD_VENC_START_RECV_PIC _IOW(HI_ID_VENC, 8, hi_handle)
#define CMD_VENC_STOP_RECV_PIC _IOW(HI_ID_VENC, 9, hi_handle)

#define CMD_VENC_SEND_FRAME _IOW(HI_ID_VENC, 0xa, venc_ioctl_acquire_stream)
#define CMD_VENC_REQUEST_I_FRAME _IOW(HI_ID_VENC, 0xb, hi_handle)

#define CMD_VENC_QUEUE_FRAME _IOWR(HI_ID_VENC, 0xc, venc_ioctl_queue_frame)
#define CMD_VENC_DEQUEUE_FRAME _IOWR(HI_ID_VENC, 0xd, venc_ioctl_queue_frame)

#define CMD_VENC_SET_SRC _IOWR(HI_ID_VENC, 0x14, venc_ioctl_set_source)
#define CMD_VENC_GET_CAP _IOWR(HI_ID_VENC, 0x15, hi_venc_cap_info)

#define CMD_VENC_RC_GET_TASK _IOWR(HI_ID_VENC, 0x16, venc_ioctl_rc_info)
#define CMD_VENC_RC_SET_RESULT _IOWR(HI_ID_VENC, 0x17, venc_ioctl_rc_info)

#define CMD_VENC_SET_RC_TYPE _IOWR(HI_ID_VENC, 0x16, venc_ioctl_rate_control)
#define CMD_VENC_GET_RC_TYPE _IOWR(HI_ID_VENC, 0x17, venc_ioctl_rate_control)

#define CMD_VENC_SET_INPUT_FRMRATE_TYPE _IOWR(HI_ID_VENC, 0x18, venc_ioctl_rate_control)
#define CMD_VENC_GET_INPUT_FRMRATE_TYPE _IOWR(HI_ID_VENC, 0x19, venc_ioctl_rate_control)

/* just for omxvenc */
#define CMD_VENC_GET_MSG _IOWR(HI_ID_VENC, 0xf, venc_ioctl_get_msg)
#define CMD_VENC_QUEUE_STREAM _IOWR(HI_ID_VENC, 0x10, venc_ioctl_queue_frame)

#define CMD_VENC_MMZ_MAP _IOWR(HI_ID_VENC, 0x11, venc_ioctl_mmz_phy)
#define CMD_VENC_MMZ_UMMAP _IOWR(HI_ID_VENC, 0x12, venc_ioctl_mmz_phy)

#define CMD_VENC_FLUSH_PORT _IOWR(HI_ID_VENC, 0x13, venc_ioctl_flush_port)

#define CMD_VENC_SET_CHN_CFG _IOWR(HI_ID_VENC, 0x20, venc_ioctl_chan_info)
#define CMD_VENC_GET_CHN_CFG _IOWR(HI_ID_VENC, 0x21, venc_ioctl_chan_info)

/* end */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: encoder
* Author: sdk
* Create: 2019-08-13
*/

#ifndef __DRV_VENC_H__
#define __DRV_VENC_H__

#include <linux/kthread.h>

#include "drv_venc_efl.h"
#include "drv_venc_ioctl.h"
#include "hi_errno.h"
#include "hi_venc_type.h"

#define VENC_FIRMWARE_VERSION "001.007.2019120700"

#define GET_VENC_CHHANDLE(chan_id) ((chan_id) | (HI_ID_VENC << 24))

#define D_VENC_CHECK_CHN(chan) do { \
    if ((chan) >= HI_VENC_MAX_CHN) {    \
        return HI_ERR_VENC_CHN_NOT_EXIST;           \
    }                                          \
} while (0)


enum {
    VEDU_YUV420 = 0,
    VEDU_YUV422 = 1,
    VEDU_YUV444 = 2
};

enum {
    VEDU_SEMIPLANNAR = 0,
    VEDU_PLANNAR = 1,
    VEDU_PACKAGE = 2
};

enum {
    VEDU_ROTATION_0 = 0,
    VEDU_ROTATION_90 = 1,
    VEDU_ROTATION_270 = 2,
    VEDU_ROTATION_180 = 3
};

enum {
    OMX_PACKAGE_ONEINONEOUT = 0,     /* omx channel each head nalu likes VPS/SPS/PPS package with IDR nalu */
    OMX_PACKAGE_ALONE = 1,         /* omx channel each nalu output alone */
    OMX_PACKAGE_SPLIT = 2,   /* omx channel head nalu likes VPS/SPS/PPS output together,and then IDR nalu output */
};

typedef struct {
    struct file *fp_save_file;    /* file pointer */
    hi_char yuv_file_name[64];  /* 64 is size of yuv_file_name */
    hi_char stream_file_name[64]; /* 64 is size of stream_file_name */
    hi_u32  frame_mode_count;    /* number of saved frame, used in frame mode */
    hi_bool time_mode_run;         /* run tag in time mode */
    hi_bool frame_mode_run;        /* run tag in frame mode */
    hi_bool save_yuv_file_run;
} venc_proc_write;

typedef enum {
    VEDU_STATE_CREATE = 0,
    VEDU_STATE_START = 1,
    VEDU_STATE_STOPPING = 2,
    VEDU_STATE_STOP = 3,
    VEDU_STATE_DESTORYING = 4,
    VEDU_STATE_BUTT
} venc_chn_state;

typedef struct {
    hi_bool enable;
    hi_bool omx_chn;
    hi_handle source;
    hi_handle user_handle;     /* user handle will not change after pmoc */
    hi_handle jpge;
    hi_mod_id src_mod_id;
    venc_chn_state chan_state;

    vedu_efl_enc_para *venc_handle;

    hi_u32 frame_num_last_input;           /* use as a static value, save last encoded frame number */
    hi_u32 frame_num_last_encoded;         /* use as a static value, save last input frame number */
    hi_u32 total_byte_last_encoded;        /* use as a static value, save last encoded total byte */
    hi_u32 last_sec_input_fps;             /* input frame rate of last second, source may be VI or VO */
    hi_u32 last_sec_encoded_fps;           /* encoded frame rate of last second by frame control */
    hi_u32 last_sec_kbps;                 /* bit rate of last second, count as Kbps */
    hi_u32 last_sec_try_num;
    hi_u32 last_try_num_total;
    hi_u32 last_sec_ok_num;
    hi_u32 last_ok_num_total;
    hi_u32 last_sec_put_num;
    hi_u32 last_put_num_total;
    struct file *which_file;

    venc_proc_write proc_write;
    hi_venc_chan_info chn_user_cfg;
} optm_venc_chn;

hi_s32 venc_drv_init(hi_void);
hi_void venc_drv_exit(hi_void);
hi_void venc_drv_board_init(hi_void);
hi_void venc_drv_board_deinit(hi_void);
hi_void venc_drv_init_event(hi_void);
hi_void venc_drv_deinit_event(hi_void);
hi_bool venc_drv_check_chn_state_safe(hi_u32 chan_id);

hi_s32 venc_drv_create_chn(vedu_efl_enc_para **ph_venc_chn, hi_venc_chan_info *pst_attr,
    hi_ioctl_strm_buf_info *buf_info, hi_bool omx_chn, struct file *file);
hi_s32    venc_drv_destroy_chn(vedu_efl_enc_para *venc_chn);

hi_s32    venc_drv_attach_input(vedu_efl_enc_para *venc_chn, hi_handle src, hi_mod_id en_mod_id);
hi_s32    venc_drv_detach_input(vedu_efl_enc_para *venc_chn, hi_handle src, hi_mod_id en_mod_id);

hi_s32    venc_drv_quire_stream(vedu_efl_enc_para *venc_chn, hi_venc_stream *pst_stream, hi_u32 u32_timeout_ms,
    venc_ioctl_buf_offset *pst_buf_off_set);
hi_s32    venc_drv_release_stream(vedu_efl_enc_para *venc_chn, venc_ioctl_acquire_stream *pst_stream);

hi_s32    venc_drv_start_receive_pic(vedu_efl_enc_para *enc_handle);
hi_s32    venc_drv_stop_receive_pic(vedu_efl_enc_para *enc_handle);

hi_s32    venc_drv_get_attr(vedu_efl_enc_para *enc_handle, hi_venc_chan_info *pst_attr);

hi_s32    venc_drv_request_i_frame(vedu_efl_enc_para *enc_handle);

hi_s32 venc_drv_set_attr_gop_change(vedu_efl_enc_para* enc_handle, hi_u32 gop_type);
hi_s32 venc_drv_queue_frame(vedu_efl_enc_para *venc_chn, hi_drv_video_frame *pst_frame_info);
hi_s32 venc_drv_dequeue_frame(vedu_efl_enc_para *venc_chn, hi_drv_video_frame *pst_frame_info);

hi_s32 venc_drv_get_capability(hi_venc_cap *pst_capability);
hi_s32 venc_drv_set_src_info(vedu_efl_enc_para *venc_chn, drv_venc_source_handle *pst_src);

hi_s32 venc_drv_proc_add(vedu_efl_enc_para *venc, hi_u32 u32_chn_id);  /* */
hi_void venc_drv_proc_del(vedu_efl_enc_para *venc, hi_u32 u32_chn_id);

hi_void venc_drv_dbg_write_yuv_omx(hi_venc_user_buf *pst_frame_omx, hi_char* file_name, hi_u32 file_name_length);
hi_void venc_drv_dbg_write_yuv(hi_drv_video_frame* pst_frame, hi_char* file_name, hi_u32 file_name_length);
hi_s32 venc_drv_dbg_save_stream(vedu_efl_nalu* nalu, vedu_efl_enc_para* ph_venc_chn);

hi_s32 venc_drv_set_rate_control_type(vedu_efl_enc_para* enc_handle, hi_venc_control_rate_type en_rc_type);
hi_s32 venc_drv_get_rate_control_type(vedu_efl_enc_para* enc_handle, hi_venc_control_rate_type *enrc_type);

hi_s32 venc_drv_set_in_frm_rate_type(vedu_efl_enc_para* enc_handle, hi_venc_frm_rate_type en_in_frm_rate_type);
hi_s32 venc_drv_get_in_frm_rate_type(vedu_efl_enc_para* enc_handle, hi_venc_frm_rate_type *en_in_frm_rate_type);

hi_s32 venc_drv_set_config(vedu_efl_enc_para *enc_para, hi_venc_chan_config *config);
hi_s32 venc_drv_get_config(vedu_efl_enc_para *enc_handle, hi_venc_chan_config *config);

#endif /* __DRV_VENC_H__ */

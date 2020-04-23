/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-07-18
 */

#ifndef __HI_DRV_VENC_H__
#define __HI_DRV_VENC_H__

#include "hi_drv_module.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_FATAL_VENC(fmt...) HI_FATAL_PRINT(HI_ID_VENC, fmt)
#define HI_ERR_VENC(fmt...) HI_ERR_PRINT(HI_ID_VENC, fmt)
#define HI_WARN_VENC(fmt...) HI_WARN_PRINT(HI_ID_VENC, fmt)
#define HI_INFO_VENC(fmt...) HI_INFO_PRINT(HI_ID_VENC, fmt)
#define HI_DBG_VENC(fmt...) HI_DBG_PRINT(HI_ID_VENC, fmt)

#define HI_VENC_TYPE_NUM 3

/*********************************************************************/
/* for omxvenc struction                                             */
/*********************************************************************/
/* VENC msg response types */
#define HI_VENC_MSG_RESP_BASE               0xA0000
#define HI_VENC_MSG_RESP_START_DONE        (HI_VENC_MSG_RESP_BASE + 0x1)
#define HI_VENC_MSG_RESP_STOP_DONE         (HI_VENC_MSG_RESP_BASE + 0x2)
#define HI_VENC_MSG_RESP_PAUSE_DONE        (HI_VENC_MSG_RESP_BASE + 0x3)
#define HI_VENC_MSG_RESP_RESUME_DONE       (HI_VENC_MSG_RESP_BASE + 0x4)
#define HI_VENC_MSG_RESP_FLUSH_INPUT_DONE  (HI_VENC_MSG_RESP_BASE + 0x5)
#define HI_VENC_MSG_RESP_FLUSH_OUTPUT_DONE (HI_VENC_MSG_RESP_BASE + 0x6)
#define HI_VENC_MSG_RESP_INPUT_DONE        (HI_VENC_MSG_RESP_BASE + 0x7)
#define HI_VENC_MSG_RESP_OUTPUT_DONE       (HI_VENC_MSG_RESP_BASE + 0x8)
#define HI_VENC_MSG_RESP_MSG_STOP_DONE     (HI_VENC_MSG_RESP_BASE + 0x9)

#define HI_VENC_OMX_BUFFERFLAG_EOS 0x00000001
#define HI_VENC_OMX_BUFFERFLAG_STARTTIME 0x00000002
#define HI_VENC_OMX_BUFFERFLAG_DECODEONLY 0x00000004
#define HI_VENC_OMX_BUFFERFLAG_DATACORRUPT 0x00000008
#define HI_VENC_OMX_BUFFERFLAG_ENDOFFRAME 0x00000010
#define HI_VENC_OMX_BUFFERFLAG_SYNCFRAME 0x00000020
#define HI_VENC_OMX_BUFFERFLAG_EXTRADATA 0x00000040
#define HI_VENC_OMX_BUFFERFLAG_CODECCONFIG 0x00000080

typedef enum {
    HI_VENC_CONTROL_RATE_DISABLE,
    HI_VENC_CONTROL_RATE_VARIABLE,
    HI_VENC_CONTROL_RATE_CONSTANT,
    HI_VENC_CONTROL_RATE_VARIABLE_SKIP_FRAMES,
    HI_VENC_CONTROL_RATE_CONSTANT_SKIP_FRAMES,
    HI_VENC_CONTROL_RATE_KHRONOS_EXTENSIONS = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    HI_VENC_CONTROL_RATE_VENDOR_START_UNUSED = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    HI_VENC_CONTROL_RATE_MAX = 0x7FFFFFFF
} hi_venc_control_rate_type;

typedef enum {
    HI_VENC_FRMRATE_AUTO,
    HI_VENC_FRMRATE_STREAM,
    HI_VENC_FRMRATE_USER,
    HI_VENC_FRMRATE_BUTT
} hi_venc_frm_rate_type;

typedef enum {
    HI_VENC_STD_H264,
    HI_VENC_STD_H265,
    HI_VENC_STD_JPEG,
    HI_VENC_STD_MAX
}hi_venc_std;

typedef enum {
    HI_VENC_CAP_LEVEL_NULL = 0, /**<Do not decode.*/ /**<CNcomment: 不需要编码 */
    HI_VENC_CAP_LEVEL_QCIF = 0, /**<The resolution of the picture to be decoded is less than or equal to 176x144.*/ /**<CNcomment: 编码的图像大小不超过176*144 */
    HI_VENC_CAP_LEVEL_CIF,      /**<The resolution of the picture to be decoded less than or equal to 352x288.*/ /**<CNcomment: 编码的图像大小不超过352*288 */
    HI_VENC_CAP_LEVEL_D1,       /**<The resolution of the picture to be decoded less than or equal to 720x576.*/ /**<CNcomment: 编码的图像大小不超过720*576 */
    HI_VENC_CAP_LEVEL_720P,     /**<The resolution of the picture to be decoded is less than or equal to 1280x720.*/ /**<CNcomment: 编码的图像大小不超过1280*720 */
    HI_VENC_CAP_LEVEL_FULLHD,   /**<The resolution of the picture to be decoded is less than or equal to 1920x1080.*/ /**<CNcomment: 编码的图像大小不超过1920*1080 */

    HI_VENC_CAP_LEVEL_1280x800, /**<The resolution of the picture to be decoded is less than or equal to 1280x800.*/ /**<CNcomment: 编码的图像大小不超过1280x800*/
    HI_VENC_CAP_LEVEL_800x1280, /**<The resolution of the picture to be decoded is less than or equal to 800x1280.*/ /**<CNcomment: 编码的图像大小不超过800x1280*/
    HI_VENC_CAP_LEVEL_1488x1280, /**<The resolution of the picture to be decoded is less than or equal to 1488x1280.*/ /**<CNcomment: 编码的图像大小不超过1488x1280 */
    HI_VENC_CAP_LEVEL_1280x1488, /**<The resolution of the picture to be decoded is less than or equal to 1280x1488.*/ /**<CNcomment: 编码的图像大小不超过1280x1488 */
    HI_VENC_CAP_LEVEL_2160x1280, /**<The resolution of the picture to be decoded is less than or equal to 2160x1280.*/ /**<CNcomment: 编码的图像大小不超过2160x1280 */
    HI_VENC_CAP_LEVEL_1280x2160, /**<The resolution of the picture to be decoded is less than or equal to 1280x2160.*/ /**<CNcomment: 编码的图像大小不超过1280x2160 */
    HI_VENC_CAP_LEVEL_2160x2160, /**<The resolution of the picture to be decoded is less than or equal to 2160x2160.*/ /**<CNcomment: 编码的图像大小不超过2160x2160 */
    HI_VENC_CAP_LEVEL_4096x2160, /**<The resolution of the picture to be decoded is less than or equal to 4096x2160.*/ /**<CNcomment: 编码的图像大小不超过4096x2160 */
    HI_VENC_CAP_LEVEL_2160x4096, /**<The resolution of the picture to be decoded is less than or equal to 2160x4096.*/ /**<CNcomment: 编码的图像大小不超过2160x4096 */
    HI_VENC_CAP_LEVEL_4096x4096, /**<The resolution of the picture to be decoded is less than or equal to 4096x4096.*/ /**<CNcomment: 编码的图像大小不超过4096x4096 */

    HI_VENC_CAP_LEVEL_MAX
} hi_venc_cap_level;

typedef enum {
    HI_VENC_H264_PROFILE_BASELINE = 0,   /**<Encode H264 stream by baseline profile.*/ /**<CNcomment: 采用基线档次编码H264码流*/
    HI_VENC_H264_PROFILE_MAIN,           /**<Encode H264 stream by main profile.*/     /**<CNcomment:采用主要档次编码H264码流*/
    HI_VENC_H264_PROFILE_EXTENDED,       /**<Encode H264 stream by extended profile.*/ /**<CNcomment:采用扩展档次编码H264码流*/
    HI_VENC_H264_PROFILE_HIGH,           /**<Encode H264 stream by high profile.*/     /**<CNcomment: 采用高级档次编码H264码流*/
    HI_VENC_H264_PROFILE_MAX
} hi_venc_h264_profile;

typedef enum {
    HI_VENC_GOP_MODE_NORMALP    = 0,
    HI_VENC_GOP_MODE_DUALP      = 1,
    HI_VENC_GOP_MODE_BIPREDB    = 2,
    HI_VENC_GOP_MODE_MAX,
} hi_venc_gop_mode;

typedef enum {
    HI_VENC_H264E_NALU_PSLICE = 1,       /**<P slice NALU*/
    HI_VENC_H264E_NALU_ISLICE = 5,       /**<I slice NALU*/
    HI_VENC_H264E_NALU_SEI = 6,          /**<SEI NALU*/
    HI_VENC_H264E_NALU_SPS = 7,          /**<SPS NALU*/
    HI_VENC_H264E_NALU_PPS = 8,          /**<PPS NALU*/
    HI_VENC_H264E_NALU_MAX
} hi_venc_h264e_nalu_type;

/**HEVC NALU type*/
/**CNcomment: HEVC NALU类型 */
typedef enum {
    HI_VENC_H265E_NALU_PSLICE = 1,    /**<P slice NALU*/
    HI_VENC_H265E_NALU_ISLICE,        /**<I slice NALU*/
    HI_VENC_H265E_NALU_VPS,           /**<VPS NALU*/
    HI_VENC_H265E_NALU_SPS,           /**<SPS NALU*/
    HI_VENC_H265E_NALU_PPS,           /**<PPS NALU*/
    HI_VENC_H265E_NALU_SEI,           /**<SEI NALU*/
    HI_VENC_H265E_NALU_MAX
} hi_venc_h265e_nalu_type;

typedef union {
    hi_venc_h264e_nalu_type h264e_type;	/**<H.264 encoding data*/ /**<CNcomment: h264编码数据*/
    hi_venc_h265e_nalu_type h265e_type; /**<HEVC encoding data*/ /**<CNcomment: HEVC编码数据*/
} hi_venc_data_type;

typedef struct {
    hi_venc_gop_mode gop_mode;
    hi_s32 sp_interval;
    hi_u32 b_frm_num;
} hi_venc_gop_attr;

typedef enum {
    HI_VENC_RC_TYPE_CBR,
    HI_VENC_RC_TYPE_AVBR,
    HI_VENC_RC_TYPE_MAX
} hi_venc_rc_type;

typedef struct {
    hi_venc_rc_type type;
    hi_u8    max_qp;
    hi_u8    min_qp;
} hi_venc_rc_param;

typedef enum {
    HI_VENC_FRAME_DROP_NORMAL = 0,
    HI_VENC_FRAME_DROP_SKIP_FRM,
    HI_VENC_FRAME_DROP_MAX,
} hi_venc_frame_drop_mode;

typedef struct {
    hi_bool enable;
    hi_u32  bit_rate_threshold;
    hi_venc_frame_drop_mode mode;
    hi_u32 drop_gap;
}  hi_venc_frame_drop_strategy;

typedef struct {
    hi_u32 width;
    hi_u32 height;

    hi_u32 target_bitrate;
    hi_u32 target_frame_rate;
    hi_u32 input_frame_rate;

    hi_u32 gop;
    hi_bool quick_encode;
    hi_u8 priority;
    hi_u32 qfactor;
    hi_u32 sp_interval;
    hi_venc_frm_rate_type frm_rate_type;
} hi_venc_chan_config;

typedef struct {
    hi_bool prepend_sps_pps;
    hi_bool auto_request_i_frame;
    hi_bool omx_chan;
    hi_venc_control_rate_type control_rate_type;    /* match the enum OMX_VIDEO_CONTROLRATETYPE */
    hi_u32 h264_level;
} hi_venc_omx_private;

typedef struct {
    hi_venc_std standard;
    hi_venc_h264_profile profile;
    hi_u32 max_width;
    hi_u32 max_height;
    hi_u32 strm_buf_size;
    hi_bool slc_split_enable;
    hi_u32 split_size;
    hi_bool secure;
    hi_venc_gop_mode gop_mode;
    hi_venc_chan_config config;
    /* driver specific */
    hi_venc_frame_drop_strategy frm_drop_strategy;
    hi_venc_rc_param rc_param;
    hi_venc_omx_private omx_private;    /* just for omx private */
} hi_venc_chan_info;

typedef enum {
    HI_VENC_PORT_DIR_INPUT,
    HI_VENC_PORT_DIROUTPUT,
    HI_VENC_PORT_DIRBOTH = 0xFFFFFFFF
} hi_venc_port_dir;

/*VENC Capability Information Of One Resolution*/
/**CNcomment: 某一分辨率的编码能力信息*/
typedef struct {
    hi_u32 height;                    /**<support max height*//**<CNcomment: 高度 */
    hi_u32 width;                     /**<support min height*//**<CNcomment: 宽度 */
    hi_u32 max_frame_rate;            /**<support max frame rate*//**<CNcomment: 最大帧率 */
    hi_u32 max_bit_rate;              /**<support max bit rate*//**<CNcomment: 最大码率*/
    hi_u32 min_bit_rate;              /**<support min bit rate*//**<CNcomment: 最小码率*/
    hi_u32 min_buf_size;              /**<support min stream buffer size*//**<CNcomment: 最大能力级对应的最小码流buffer大小*/
} hi_venc_cap_resolution;

typedef struct {
    hi_venc_std standard;                    /**<support venc type*//**<CNcomment: 编码类型*/
    hi_u32 max_chn_num;                      /**<support max channel num*//**<CNcomment: 最大通道数*/
    hi_venc_cap_resolution max_resolution;   /**<support capability information of max resolution*//**<CNcomment: 最大分辨率对应的能力信息*/
    hi_venc_cap_resolution mix_resolution;   /**<support capability information of min resolution*//**<CNcomment: 最小分辨率对应的能力信息*/
} hi_venc_cap_info;

/*VENC Capability Information*/
/**CNcomment: 编码能力信息*/
typedef struct {
    hi_u32 support_type_num;                     /**<support venc type num*//**<CNcomment: 编码器支持的编码类型数量 */
    hi_venc_cap_info cap_info[HI_VENC_TYPE_NUM]; /**<capability information for venc types*//**<CNcomment: 编码器不同编码类型的编码能力信息 */
} hi_venc_cap;

typedef struct {
    hi_u64 virt_addr;           /**<Stream virtual address*/ /**<CNcomment: 码流虚拟地址*/
    hi_u32 slc_len;                /**<Stream length*/ /**<CNcomment: 码流长度*/
    hi_u32 pts_ms;                 /**<Presentation time stamp (PTS), in ms*/ /**<CNcomment: 时间戳，单位是毫秒*/
    hi_bool frame_end;             /**<Frame end or not*/ /**<CNcomment: 标识是否为帧结束*/
    hi_venc_data_type data_type;   /**Encoding data type*/ /**<CNcomment: 编码数据类型*/
} hi_venc_stream;

typedef struct {
    hi_u32 buffer_addr_phy;
    hi_u32 virt_phy_offset;
    hi_u32 buffer_size;
} hi_venc_meta_data_buf;

typedef struct {
    hi_mem_handle_t buffer_handle;
    hi_u32 buffer_addr_phy;
    hi_u32 buffer_size;
    hi_u32 offset_y_c;
    hi_u32 offset_y_cr;
    hi_u32 offset;
    hi_u32 data_len;

    hi_u32 stride_y;
    hi_u32 stride_c;

    hi_u32 store_type;
    hi_u32 sample_type;
    hi_u32 package_sel;
    hi_u32 time_stamp0;
    hi_u32 time_stamp1;
    hi_u32 flags;

    hi_u32 pic_width;
    hi_u32 pic_height;
    hi_venc_port_dir dir;

    hi_u32 meta_data_flag;
    hi_venc_meta_data_buf meta_data;

    hi_s32 pmem_fd;
    hi_s32 mmaped_size;

    hi_u32 client_data;
    hi_u64 virt_to_phy_offset;
} hi_venc_user_buf;

typedef struct {
    void* buffer_addr;           /* user state should be use */
    void* ion_handle;
    void* meta_buffer_addr;

    hi_venc_user_buf user_buf;  /* kernel state should be use */
}hi_venc_user_info;

typedef struct {
    hi_u32 status_code;         /**<Record the return value of the operation*/ /**<CNcomment:记录操作的返回值(success/failure)*/
    hi_u32 msg_code;
    hi_venc_user_buf buf;
    hi_u32 msg_data_size;
} hi_venc_msg_info;

/*********************************************************************/
/* for omxvenc struction  ->end                                      */
/*********************************************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

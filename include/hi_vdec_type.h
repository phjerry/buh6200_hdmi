/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2019-04-30
 */

#ifndef __HI_VDEC_TYPE_H__
#define __HI_VDEC_TYPE_H__

#include "hi_type.h"
/* CNcomment: 强制约束，严禁include其它外部头文件 */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* --------------------------------------- EXTERNAL TYPE DEFINE --------------------------------------- */
/*
 * CNcomment: VDEC对外类型定义，内部类型不允许在本头文件中定义
 * 供用户态内核态共用，需考虑32&64 bit兼容
 */

#define HI_VDEC_MAX_EXT_BUF_NUM  16
#define HI_VDEC_MAX_USD_NUM      4

typedef enum {
    HI_VDEC_STD_START_RESERVE = 0x00000000,

    /* MPEG/ITU-T video format series */
    HI_VDEC_STD_MPEG1 = 0x00010000,
    HI_VDEC_STD_MPEG2,
    HI_VDEC_STD_MPEG4,
    HI_VDEC_STD_H261,
    HI_VDEC_STD_H262,
    HI_VDEC_STD_H263,
    HI_VDEC_STD_SORENSON,
    HI_VDEC_STD_H264,
    HI_VDEC_STD_H264_MVC,
    HI_VDEC_STD_H265,
    /* VPx video format series */
    HI_VDEC_STD_VP3 = 0x00020000,
    HI_VDEC_STD_VP5,
    HI_VDEC_STD_VP6,
    HI_VDEC_STD_VP6F,
    HI_VDEC_STD_VP6A,
    HI_VDEC_STD_VP8,
    HI_VDEC_STD_VP9,
    /* AVS video format series */
    HI_VDEC_STD_AVS = 0x00030000,
    HI_VDEC_STD_AVS2,
    HI_VDEC_STD_AVS3,
    /* SMPTE video format base on WMV3(WMV9) */
    HI_VDEC_STD_VC1 = 0x00040000,
    /* Advance video */
    HI_VDEC_STD_AV1 = 0x00050000,
    /* MJPEG video format series */
    HI_VDEC_STD_MJPEG = 0x00060000,
    HI_VDEC_STD_MJPEGB,
    /* RealNetworks video format series*/
    HI_VDEC_STD_REAL8 = 0x00070000,
    HI_VDEC_STD_REAL9,
    /* Divx video format series */
    HI_VDEC_STD_DIVX3 = 0x00080000,
    HI_VDEC_STD_DIVX4,
    HI_VDEC_STD_DIVX5,
    /* Microsoft private video format series */
    HI_VDEC_STD_WMV1 = 0x00090000,
    HI_VDEC_STD_WMV2,
    HI_VDEC_STD_WMV3,
    HI_VDEC_STD_MSMPEG4V1,
    HI_VDEC_STD_MSMPEG4V2,
    HI_VDEC_STD_MSVIDEO1,
    /* Others */
    HI_VDEC_STD_INDEO2 = 0x000A0000,
    HI_VDEC_STD_INDEO3,
    HI_VDEC_STD_INDEO4,
    HI_VDEC_STD_INDEO5,
    HI_VDEC_STD_CINEPAK,
    HI_VDEC_STD_SVQ1,
    HI_VDEC_STD_SVQ3,
    HI_VDEC_STD_DV,
    HI_VDEC_STD_RV10,
    HI_VDEC_STD_RV20,
    HI_VDEC_STD_RAW,

    HI_VDEC_STD_MAX
}hi_vdec_std;

typedef enum {
    HI_VDEC_DEC_MODE_NORMAL,         /* Decode all frames(default) */
    HI_VDEC_DEC_MODE_IP,             /* Decode only I and P frames */
    HI_VDEC_DEC_MODE_I,              /* Decode only I frames */
    HI_VDEC_DEC_MODE_DROP_INVALID_B, /* Decode all frames except the B frames next to I frames */
                                     /* CNcomment: 解所有帧，除了紧跟着I帧后面的B帧 */
    HI_VDEC_DEC_MODE_FIRST_I,        /* Decode only the first I frame */
    HI_VDEC_DEC_MODE_MAX
} hi_vdec_dec_mode;

typedef enum {
    HI_VDEC_OUT_ORDER_DISPLAY,       /* Output in display order(default) */
    HI_VDEC_OUT_ORDER_DECODE,        /* Output in decode order */
    HI_VDEC_OUT_ORDER_MAX
} hi_vdec_out_order;

typedef enum {
    HI_VDEC_DPB_MODE_NORMAL,              /* Normal DPB procedure */
    HI_VDEC_DPB_MODE_SIMPLE,              /* Simple DPB procedure */
    HI_VDEC_DPB_MODE_SKYPLAY,             /* Skyplay DPB procedure, allow dpb full */
    HI_VDEC_DPB_MODE_MAX
} hi_vdec_dpb_mode;

typedef enum {
    HI_VDEC_EVENT_MODE_ACTIVE,            /* Acitve report event */
    HI_VDEC_EVENT_MODE_PASSIVE,           /* Passive listen event */
    HI_VDEC_EVENT_MODE_MAX
} hi_vdec_evt_mode;

typedef enum {
    HI_VDEC_STM_INTERNAL,            /* vdec input buffer allocated internally */
    HI_VDEC_STM_EXTERNAL,            /* vdec input buffer allocated externally */
    HI_VDEC_STM_MAX
} hi_vdec_stm_buf_mode;

typedef enum {
    HI_VDEC_FRM_INTERNAL,            /* vdec output buffer allocated internally */
    HI_VDEC_FRM_EXTERNAL,            /* vdec output buffer allocated externally */
    HI_VDEC_FRM_MAX
} hi_vdec_frm_buf_mode;

typedef enum {
    HI_VDEC_BUF_EMPTY,           /* Buffer is empty */
    HI_VDEC_BUF_FULL,            /* Buffer is filled with data */
    HI_VDEC_BUF_IN_USE,          /* Buffer is in using */
    HI_VDEC_BUF_MAX
} hi_vdec_buf_state;

typedef enum {
    HI_VDEC_USD_TYPE_UNKNOWN  = 0x0,      /* Unknown type */
    HI_VDEC_USD_TYPE_DVB1_CC  = 0x1,      /* Closed caption data */
    HI_VDEC_USD_TYPE_DVB1_BAR = 0x2,      /* Bar data */
    HI_VDEC_USD_TYPE_AFD      = 0x10000,  /* Active format description */
    HI_VDEC_USD_TYPE_MAX
} hi_vdec_usd_type;

typedef enum {
    HI_VDEC_USD_POS_UNKNOWN,         /* Unknown */
    HI_VDEC_USD_POS_MPEG2_SEQ,       /* MPEG2 sequences */
    HI_VDEC_USD_POS_MPEG2_GOP,       /* MPEG2 GOP */
    HI_VDEC_USD_POS_MPEG2_FRM,       /* MPEG2 frames */
    HI_VDEC_USD_POS_MPEG4_VSOS,      /* MPEG4 sequences of visible objects */
    HI_VDEC_USD_POS_MPEG4_VSO,       /* MPEG4 visible objects */
    HI_VDEC_USD_POS_MPEG4_VOL,       /* MPEG4 video object layer */
    HI_VDEC_USD_POS_MPEG4_GOP,       /* MPEG4 GOP */
    HI_VDEC_USD_POS_H264_REG,        /* H264  regestered usd */
    HI_VDEC_USD_POS_H264_UNREG,      /* H264  unregestered usd */
    HI_VDEC_USD_POS_AVS_SEQ,
    HI_VDEC_USD_POS_AVS_FRM,
    HI_VDEC_USD_POS_MAX
} hi_vdec_usd_pos;

typedef enum {
    HI_VDEC_BIT_DEPTH_8,
    HI_VDEC_BIT_DEPTH_10,
    HI_VDEC_BIT_DEPTH_12,
    HI_VDEC_BIT_DEPTH_MAX
} hi_vdec_bit_depth;

typedef enum {
    HI_VDEC_CS_UNKNOWN,
    HI_VDEC_CS_BT601_YUV_LIMITED,
    HI_VDEC_CS_BT601_YUV_FULL,
    HI_VDEC_CS_BT709_YUV_LIMITED,
    HI_VDEC_CS_BT709_YUV_FULL,
    HI_VDEC_CS_BT2020_YUV_LIMITED,
    HI_VDEC_CS_BT2020_YUV_FULL,
    HI_VDEC_CS_MAX
} hi_vdec_color_space;

typedef enum {
    HI_VDEC_SAMPLE_UNKNOWN,
    HI_VDEC_PROGRESSIVE,
    HI_VDEC_INTERLACED,
} hi_vdec_sampling_type;

typedef enum {
    HI_VDEC_FR_TYPE_INVALID = 0,
    HI_VDEC_FR_TYPE_PTS,         /* Calculated from PTS */
    HI_VDEC_FR_TYPE_STM,         /* Come from stream */
    HI_VDEC_FR_TYPE_USER,        /* Setted by user */
    HI_VDEC_FR_TYPE_USER_PTS,    /* Use frame rate from user until 2nd I frame, then use frame rate from PTS */
    HI_VDEC_FR_TYPE_MAX
} hi_vdec_frm_rate_type;

typedef enum {
    HI_VDEC_SPEED_INALID = 0,
    HI_VDEC_SPEED_FORWARD,       /* Fast speed forward */
    HI_VDEC_SPEED_BACKWARD,      /* Fast speed backward */
    HI_VDEC_SPEED_MAX
} hi_vdec_speed_direct;

typedef enum {
    HI_VDEC_HDR_SS_DL_VES,       /* Single Stream Dual Layer */   /* CNcomment: 单流双层格式 */
    HI_VDEC_HDR_DS_DL_VES,       /* Dual Stream Dual Layer */     /* CNcomment: 双流双层格式 */
    HI_VDEC_HDR_SS_SL_VES,       /* Single Stream Single Layer */ /* CNcomment: 单流单层格式 */
    HI_VDEC_HDR_STM_TYPE_MAX
} hi_vdec_dv_hdr_stm_type;

typedef enum {
    HI_VDEC_HDR_CHAN_NONE,       /* CNcomment: 非DV HDR解码通道*/
    HI_VDEC_HDR_CHAN_BL,         /* CNcomment: BL解码通道*/
    HI_VDEC_HDR_CHAN_EL,         /* CNcomment: EL解码通道*/
    HI_VDEC_HDR_CHAN_MAX
} hi_vdec_dv_hdr_chan_type;

typedef enum {
    /* Semi-Planner */
    HI_VDEC_YUV_FMT_420SP,               /* Spatial sampling format 4:2:0 */
    /* Semi-Planner Force Bit Depth */
    HI_VDEC_YUV_FMT_420SP_FIX_8BIT,      /* Spatial sampling format 4:2:0, fix 8bit */
    HI_VDEC_YUV_FMT_420SP_EXPAND_16BIT,  /* Spatial sampling format 4:2:0, expand 16bit */
    /* Semi-Planner Compress */
    HI_VDEC_YUV_FMT_420SP_CMP,           /* Spatial sampling format 4:2:0, private compress */
    HI_VDEC_YUV_FMT_420SP_AFBC,          /* Spatial sampling format 4:2:0, AFBC compress */
    /* Semi-Planner Tile */
    HI_VDEC_YUV_FMT_420SP_TILE,          /* Spatial sampling format TILE 4:2:0 */
    HI_VDEC_YUV_FMT_MAX
} hi_vdec_yuv_format;

typedef enum {
    HI_VDEC_I_FRM,
    HI_VDEC_P_FRM,
    HI_VDEC_B_FRM,
    HI_VDEC_FRM_TYPE_MAX
} hi_vdec_frm_type;

typedef enum {
    HI_VDEC_FRM_INFO_DRV_VIDEO,          /* MSP DRV define frame type */
    HI_VDEC_FRM_INFO_UNF_VIDEO,          /* MSP UNF define frame type */
    HI_VDEC_FRM_INFO_MAX
} hi_vdec_frm_info_type;

typedef enum {
    HI_VDEC_CMD_START_RESERVE = 0x00000000,

    /* init es buffer when es buffer alloced in vdec param: hi_u32 *es_buf_size */
    HI_VDEC_CMD_INIT_INPUT_BUF = 0x00010000,
    /* deinit es buffer when es buffer alloced in vdec. param: HI_NULL */
    HI_VDEC_CMD_DEINIT_INPUT_BUF,
    /* bind es buffer when es buffer alloced out of vdec. param: hi_vdec_opt_bind_input* */
    HI_VDEC_CMD_BIND_INPUT_BUF,
    /* bind frame buf when frame buffer alloced out of vdec. param: hi_vdec_bind_frm_buf* */
    HI_VDEC_CMD_BIND_FRM_BUF,
    /* unbind frame buf when frame buffer alloced out of vdec. param: HI_NULL */
    HI_VDEC_CMD_UNBIND_FRM_BUF,
    /* bind userdata buf param: hi_vdec_bind_usd_param* */
    HI_VDEC_CMD_BIND_USD_BUF,
    /* check new userdata. param: hi_vdec_user_data* */
    HI_VDEC_CMD_CHECK_USER_DATA,
    /* acquire userdata. param: hi_vdec_user_data* */
    HI_VDEC_CMD_ACQ_USER_DATA,
    /* release userdata. param: hi_vdec_user_data* */
    HI_VDEC_CMD_REL_USER_DATA,
    /* drop stream. param:hi_vdec_drop_stm* */
    HI_VDEC_CMD_DROP_STREAM,
    /* judge if standard is support. param:hi_vdec_opt_is_std_support* */
    HI_VDEC_CMD_IS_STD_SUPPORT,
    /* judge if random stm support. param:hi_vdec_check_random* */
    HI_VDEC_CMD_IS_RANDOM_STM_SUPPORT,
    /* send eos flag. param: hi_bool* */
    HI_VDEC_CMD_SEND_EOS,
    /* set sec flag. param: hi_bool* */
    HI_VDEC_CMD_SET_TVP,

    HI_VDEC_CMD_EXT_START = 0x20000000,
    HI_VDEC_CMD_EXT_END   = 0x30000000,

    HI_VDEC_CMD_MAX
} hi_vdec_cmd_id;

typedef enum {
    HI_VDEC_PARAM_RESERVE = 0x00000000,

    /* set uvmos enable/disable. param: hi_bool* */
    HI_VDEC_PARAM_UVMOS = 0x00010000,
    /* set low delay enable/disable. param: hi_bool* */
    HI_VDEC_PARAM_LOW_DELAY,
    /* set dpb mode. param: hi_vdec_dpb_mode* */
    HI_VDEC_PARAM_DPB_MODE,
    /* set speed info. param: hi_vdec_speed* */
    HI_VDEC_PARAM_FF_FB_SPEED,
    /* set pvr info. param:hi_vdec_pvr_info* */
    HI_VDEC_PARAM_PVR_INFO,
    /* set hdr info. param: hi_vdec_hdr_attr* */
    HI_VDEC_PARAM_HDR_ATTR,
    /* set start pts. param:hi_vdec_start_pts* */
    HI_VDEC_PARAM_START_PTS,
    /* set stream buf mode. param: hi_vdec_stm_buf_mode* */
    HI_VDEC_PARAM_STM_BUF_MODE,
    /* set frame buf mode, param: hi_vdec_frm_buf_mode* */
    HI_VDEC_PARAM_FRM_BUF_MODE,
    /* set/get output format. param: hi_vdec_out_format* */
    HI_VDEC_PARAM_OUTPUT_FORMAT,
    /* set src tag. param: hi_vdec_src_tag* */
    HI_VDEC_PARAM_SOURCE_TAG,
    /* set/get frame rate. param: hi_vdec_frm_rate* */
    HI_VDEC_PARAM_FRAME_RATE,
    /* get out delay. */
    HI_VDEC_PARAM_OUT_DELAY,
    /* get in delay. */
    HI_VDEC_PARAM_IN_DELAY,
    /* get process delay. */
    HI_VDEC_PARAM_PROCESS_DELAY,
    /* get channel feature. param: hi_vdec_feature* */
    HI_VDEC_PARAM_FEATURE,
    /* get channel status. param: hi_vdec_status. */
    HI_VDEC_PARAM_STATUS,
    /* set poc threshold. param: hi_u32* poc */
    HI_VDEC_PARAM_POC_THRSHOLD,
    /* set dec mode param: hi_vdec_dec_mode* */
    HI_VDEC_PARAM_DEC_MODE,
    /* set/get frame packet type. param: hi_drv_3d_type* */
    HI_VDEC_PARAM_FRM_3D_TYPE,

    HI_VDEC_PARAM_MAX
} hi_vdec_param_id;

typedef enum {
    HI_VDEC_EVT_START_RESERVE    = 0,

    HI_VDEC_EVT_NEW_FRAME        = 1 << 0,
    HI_VDEC_EVT_NEW_STREAM       = 1 << 1, /* param hi_vdec_stream* */
    HI_VDEC_EVT_NEW_USER_DATA    = 1 << 2,
    HI_VDEC_EVT_FIRST_PTS        = 1 << 3, /* param hi_u32* */
    HI_VDEC_EVT_SECOND_PTS       = 1 << 4, /* param hi_u32* */
    HI_VDEC_EVT_ERR_FRAME        = 1 << 5, /* param hi_vdec_err_frm* */
    HI_VDEC_EVT_UNSUPPORT        = 1 << 6,
    HI_VDEC_EVT_ERR_STANDARD     = 1 << 7, /* param hi_vdec_std* */
    HI_VDEC_EVT_NORM_CHG         = 1 << 8, /* param hi_vdec_norm_param* */
    HI_VDEC_EVT_FRM_PCK_CHG      = 1 << 9, /* param hi_u32* */

    HI_VDEC_EVT_EOS              = 1 << 30, /* param hi_vdec_eos_param* */
    HI_VDEC_EVT_MAX_RESERVE      = HI_VDEC_EVT_EOS + 1,
    HI_VDEC_EVT_ALL              = 0xFFFFFFFF
} hi_vdec_evt_id;

typedef enum {
    HI_VDEC_CMP_ADJUST = 0,
    HI_VDEC_CMP_OFF,
    HI_VDEC_CMP_ON,
    HI_VDEC_CMP_MAX
} hi_vdec_cmp_mode;

typedef enum {
    HI_VDEC_FLUSH_IN = 0,
    HI_VDEC_FLUSH_OUT,
    HI_VDEC_FLUSH_ALL,
    HI_VDEC_FLUSH_MAX
} hi_vdec_flush_type;

typedef enum {
    HI_VDEC_NORM_PAL = 0,
    HI_VDEC_NORM_NTSC,
    HI_VDEC_NORM_MAX
} hi_vdec_norm_type;

typedef enum {
    HI_VDEC_SUB_STD_UNKNOWN = 0,
    HI_VDEC_SUB_STD_MPEG2_MPEG1,
    HI_VDEC_SUB_STD_MPEG4_SHV,
    HI_VDEC_SUB_STD_MPEG4_XVID,
    HI_VDEC_SUB_STD_MPEG4_DIVX,
    HI_VDEC_SUB_STD_MAX
} hi_vdec_sub_std;

typedef enum {
    HI_VDEC_BROADCAST_DVB,
    HI_VDEC_BROADCAST_DIRECTV,
    HI_VDEC_BROADCAST_ATSC,
    HI_VDEC_BROADCAST_DVD,
    HI_VDEC_BROADCAST_ARIB,
    HI_VDEC_BROADCAST_MAX
} hi_vdec_broadcast;

/*
 * CNcomment: 为兼容32&64 bit用户态和内核态
 * 指针类型统一使用hi_vdec_pxxx，保证占用长度一致
 */
typedef union {
    hi_void *pointer;   /* pointer type */
    hi_u64   integer;   /* integer type */
} hi_vdec_pvoid;

typedef union {
    hi_char *pointer;
    hi_u64   integer;
} hi_vdec_pchar;

typedef union {
    hi_u8 *pointer;
    hi_u64 integer;
} hi_vdec_pu8;

typedef union {
    hi_s8 *pointer;
    hi_u64 integer;
} hi_vdec_ps8;

typedef union {
    hi_u32 *pointer;
    hi_u64 integer;
} hi_vdec_pu32;

typedef union {
    hi_s32 *pointer;
    hi_u64 integer;
} hi_vdec_ps32;

typedef union {
    hi_u64 *pointer;
    hi_u64 integer;
} hi_vdec_pu64;

typedef union {
    hi_s64 *pointer;
    hi_u64 integer;
} hi_vdec_ps64;

typedef struct {
    hi_bool   is_secure;
    hi_u32    buffer_size;
} hi_vdec_init_stm;

typedef struct {
    hi_u32 frame_phy;
    hi_s32 frame_fd;
    hi_u32 frame_size;
    hi_u32 frame_stride;

    hi_u32 meta_phy;
    hi_s32 meta_fd;
    hi_u32 meta_size;
    hi_u32 meta_offset;
} hi_vdec_frm_buf;

typedef struct {
    hi_bool is_secure;
    hi_u32  buf_num;
    hi_vdec_frm_buf ext_buf[HI_VDEC_MAX_EXT_BUF_NUM];
} hi_vdec_bind_frm_buf;

typedef struct {
    hi_vdec_buf_state state;
    hi_vdec_frm_buf ext_buf;
} hi_vdec_unbind_frm_buf;

typedef struct {
    hi_vdec_broadcast broadcast;
    hi_vdec_usd_type type;
    hi_vdec_usd_pos position;
    hi_s64 pts;
    hi_u32 seq_cnt;
    hi_u32 seq_frm_cnt;
    hi_u32 user_data_len;
    hi_handle fd;
    hi_u64 usd_offset;
    hi_bool is_over_flow;
    hi_bool is_top_field_first;
} hi_vdec_user_data;

typedef struct {
    hi_vdec_pchar lib_name;
    hi_u32 name_len;
} hi_vdec_ext_decoder;

typedef struct {
    hi_u32 gap;
    hi_s64 expect_pts;
    hi_s64 actual_pts;
} hi_vdec_drop_stm;

typedef struct {
    hi_bool is_adv_profile;
    hi_u32  codec_version;
} hi_vdec_std_ext_vc1;

typedef struct {
    hi_bool need_reverse;
    hi_u32  disp_width;
    hi_u32  disp_height;
} hi_vdec_std_ext_vp6;

typedef union {
    hi_vdec_std_ext_vc1 vc1;
    hi_vdec_std_ext_vp6 vp6;
} hi_vdec_std_ext;

typedef struct {
    hi_vdec_std standard;
    hi_vdec_std_ext standard_ext;
    hi_bool support;
} hi_vdec_check_random;

typedef struct {
    hi_vdec_speed_direct direct;
    hi_u32 speed_integer;
    hi_u32 speed_decimal;
} hi_vdec_speed;

typedef struct {
    hi_bool is_idr_flag;                   /* Whether is IDR frame */
    hi_bool is_b_frm_ref;                  /* Whether B frame is refer frame */
    hi_bool is_continuous;                 /* Whether frame is continuous */
    hi_bool is_backward_optimize;          /* Whether backward optimize enable */
    hi_bool is_display_optimize;           /* Whether display optimize enable */
} hi_vdec_pvr_info;

typedef struct {
    hi_bool is_dv_hdr_enable;              /* Is Dolby HDR enable or not */
    hi_bool is_dv_frm_compatible;          /* Is Dolby frame compatible or not */
    hi_vdec_std el_standard;               /* EL chan video type */
    hi_vdec_dv_hdr_chan_type chan_type;    /* Chan type, BL or EL */
    hi_vdec_dv_hdr_stm_type stm_type;      /* VES format */
} hi_vdec_hdr_attr;

typedef struct {
    hi_s64 pts;
} hi_vdec_start_pts;

typedef struct {
    hi_vdec_frm_rate_type type;            /* The source of frame rate */
    hi_u32 frame_rate;                     /* frame rate value, 1000 times the original value */
} hi_vdec_frm_rate;

typedef struct {
    hi_s32 tag_value;
} hi_vdec_src_tag;

typedef struct {
    hi_vdec_std standard;
    hi_vdec_sub_std sub_std;
    hi_u32 sub_version;
    hi_u32 profile;
    hi_u32 level;
    hi_u32 dec_width;
    hi_u32 dec_height;
    hi_u32 disp_width;
    hi_u32 disp_height;
    hi_u32 disp_center_x;
    hi_u32 disp_center_y;
    hi_u32 aspect_width;
    hi_u32 aspect_height;
    hi_u32 bit_rate;
    hi_u32 frame_rate;
    hi_vdec_bit_depth bit_depth;
    hi_vdec_sampling_type sampling_type;
    hi_vdec_color_space color_space;
    hi_vdec_norm_type norm_type;
} hi_vdec_feature;

typedef struct {
    hi_vdec_yuv_format yuv_fmt;
    hi_bool is_u_first;
} hi_vdec_out_format;

typedef struct {
    hi_vdec_std standard;
    hi_vdec_std_ext standard_ext;
    hi_vdec_dec_mode dec_mode;
    hi_vdec_evt_mode evt_mode;
    hi_vdec_out_order out_order;
    hi_bool is_user_dec;
    hi_bool is_tvp;
    hi_bool is_tunnel;
    hi_bool enable_keep_codec_config;
    hi_u32  error_cover;
    hi_u32  priority;
    hi_u32  ext_fs_num;
    hi_u32  usd_pool_size;
    hi_u32  max_raw_num;
    hi_vdec_cmp_mode cmp_mode;
    hi_vdec_frm_info_type frm_type;
    hi_vdec_pvoid p_user_private;
} hi_vdec_attr;

typedef struct {
    hi_mem_handle_t handle;
    hi_mem_size_t   offset;
    hi_u32  index;
    hi_vdec_pu8 buf_vir;
    hi_u64  buf_phy;
    hi_u32  buf_size;        /* Buffer size(byte) */
    hi_s64  pts;             /* Stream pts, -1 invalid */
    hi_bool is_end_of_frm;
    hi_bool is_dual_layer;   /* FALSE: DV single Layer stream | TRUE : DV Dual Layer stream */
    /* pvr info */
    hi_u32  disp_enable_flag;
    hi_u32  disp_frm_distance;
    hi_u32  distance_before_first_frame;
    hi_u32  gop_num;
    hi_u64  disp_time;
} hi_vdec_stream;

typedef struct {
    hi_bool is_end_of_stm;
    hi_u32  bit_rate_in_bps;
    hi_u32  total_stm_buf_size;
    hi_u32  free_stm_buf_size;
    hi_u32  used_stm_buf_size;
    hi_u32  undecode_stm_size;
    hi_u32  undecode_stm_num;
} hi_vdec_stm_status;

typedef struct {
    hi_bool is_all_frm_out;
    hi_bool is_field;
    hi_u32  total_output_frm_num;
    hi_u32  error_frm_num;
    hi_u32  wait_output_frm_num;
    hi_u32  out_frm_rate; /* output frame rate */
    hi_u32  dec_frm_rate; /* decode frame rate */
    hi_u32  pvr_buf_num;  /* used for pvr */
} hi_vdec_frm_status;

typedef struct {
    hi_vdec_stm_status stm_state;
    hi_vdec_frm_status frm_state;
} hi_vdec_status;

typedef struct {
    hi_u32 new_width;
    hi_u32 new_height;
    hi_u32 new_stride;
    hi_u32 new_bit_depth;
    hi_u32 new_frm_num;
    hi_u32 new_frm_size;
} hi_vdec_size_change;

typedef struct {
    hi_vdec_frm_type err_frm_type;
    hi_u32 err_ratio;
} hi_vdec_err_frm;

typedef struct {
    hi_u32 reserve;
} hi_vdec_event_frame_param;

typedef struct {
    hi_u32 last_frame_flag; /* 0:last frm has not been token. 1:last frame has been token */
} hi_vdec_eos_param;

typedef struct {
    hi_vdec_norm_type norm_type;
    hi_u32 width;
    hi_u32 height;
    hi_u32 frm_rate;
    hi_bool progressive;
} hi_vdec_norm_param;

typedef struct {
    hi_vdec_evt_id event_id;
    union {
        hi_vdec_stream stream;
        hi_vdec_event_frame_param frame_param;
        hi_vdec_size_change size_change;
        hi_vdec_eos_param eos_param;
        hi_s64 first_pts;
        hi_s64 second_pts;
        hi_vdec_norm_param norm_param;
        hi_u32 frm_3d_type; /* hi_drv_3d_type */
        hi_vdec_std err_std;
        hi_vdec_err_frm err_frm;
    } event_param;
} hi_vdec_event;

typedef struct {
    hi_handle ssm_handle;
} hi_vdec_opt_create;

typedef struct {
    hi_u32 reserve;
} hi_vdec_opt_destroy;

typedef struct {
    hi_u32 reserve;
} hi_vdec_opt_start;

typedef struct {
    hi_u32 reserve;
} hi_vdec_opt_stop;

typedef struct {
    hi_u32 reserve;
} hi_vdec_opt_reset;

typedef struct {
    hi_vdec_flush_type type;
} hi_vdec_opt_flush;

typedef struct {
    hi_u32 time_out;
} hi_vdec_opt_decode;

typedef struct {
    hi_u32 time_out;
} hi_vdec_opt_listen;

typedef struct {
    hi_u32 reserve;
} hi_vdec_opt_que_stm;

typedef struct {
    hi_bool is_kern;
    hi_u32 expect_size;
} hi_vdec_opt_deq_stm;

typedef struct {
    hi_u32 time_out;
    hi_bool still_have_frame;
} hi_vdec_opt_acq_frm;

typedef struct {
    hi_u32 reserve;
} hi_vdec_opt_rel_frm;

typedef struct {
    hi_s32 size;
} hi_vdec_opt_init_input;

typedef struct {
    hi_u64 phy_addr;
    hi_u64 kern_vir_addr;
    hi_s64 handle;
    hi_u32 length;
} hi_vdec_opt_bind_input;

typedef struct {
    hi_vdec_std standard;
    hi_bool support;
} hi_vdec_opt_is_std_support;

typedef struct {
    hi_u32            frame_stream_size;
    hi_u32            avg_qp;
    hi_u32            max_mv;
    hi_u32            min_mv;
    hi_u32            avg_mv;
    hi_u32            skip_ratio;
    hi_vdec_frm_type  frm_type;
} hi_vdec_uvmos;

typedef struct {
    hi_vdec_uvmos uvmos_info;
    hi_u32 usd_num;
} hi_vdec_ext_frm_info;

typedef struct {
    hi_mem_handle_t handle;
    hi_u32 size;
} hi_vdec_bind_usd_param;


typedef hi_s32 (*fn_drv_vdec_callback)
    (hi_handle handle, hi_vdec_evt_id evt_id, const hi_void *param, hi_u32 size);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



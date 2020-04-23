/*
* Copyright (C) huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: hisilicon multimedia software group
* Author: l00214567
* Create: 2012-11-17
*/

#ifndef __HI_DRV_VIDEO_H__
#define __HI_DRV_VIDEO_H__

#include "hi_type.h"
#include "hi_module.h"
#include "hi_drv_color.h"

typedef enum {
    HI_DRV_COLOR_SYS_AUTO = 0,
    HI_DRV_COLOR_SYS_PAL,
    HI_DRV_COLOR_SYS_NTSC,
    HI_DRV_COLOR_SYS_SECAM,
    HI_DRV_COLOR_SYS_PAL_M,
    HI_DRV_COLOR_SYS_PAL_N,
    HI_DRV_COLOR_SYS_PAL_60,
    HI_DRV_COLOR_SYS_NTSC443,
    HI_DRV_COLOR_SYS_NTSC_50,
    HI_DRV_COLOR_SYS_MAX
} hi_drv_color_sys;

typedef enum {
    HI_DRV_PIXEL_BITWIDTH_DEFAULT = 0,
    HI_DRV_PIXEL_BITWIDTH_8BIT,
    HI_DRV_PIXEL_BITWIDTH_10BIT,
    HI_DRV_PIXEL_BITWIDTH_12BIT,
    HI_DRV_PIXEL_BITWIDTH_16BIT,
    HI_DRV_PIXEL_BITWIDTH_MAX
} hi_drv_pixel_bitwidth;

typedef enum {
    /* RGB formats */
    HI_DRV_PIXEL_FMT_RGB332 = 0, /*  8  RGB-3-3-2     */
    HI_DRV_PIXEL_FMT_RGB444,     /* 16  xxxxrrrr ggggbbbb */
    HI_DRV_PIXEL_FMT_RGB555,     /* 16  RGB-5-5-5     */
    HI_DRV_PIXEL_FMT_RGB565,     /* 16  RGB-5-6-5     */
    HI_DRV_PIXEL_FMT_BGR565,     /* 16  RGB-5-6-5     */
    HI_DRV_PIXEL_FMT_RGB555X,    /* 16  RGB-5-5-5 BE  */
    HI_DRV_PIXEL_FMT_RGB565X,    /* 16  RGB-5-6-5 BE  */
    HI_DRV_PIXEL_FMT_BGR666,     /* 18  BGR-6-6-6   */
    HI_DRV_PIXEL_FMT_BGR24,      /* 24  BGR-8-8-8     */
    HI_DRV_PIXEL_FMT_RGB24,      /* 24  RGB-8-8-8     */
    HI_DRV_PIXEL_FMT_BGR32,      /* 32  BGR-8-8-8-8   */
    HI_DRV_PIXEL_FMT_RGB32,      /* 32  RGB-8-8-8-8   */

    /* HISI CLUT formats */
    HI_DRV_PIXEL_FMT_CLUT_1BPP,
    HI_DRV_PIXEL_FMT_CLUT_2BPP,
    HI_DRV_PIXEL_FMT_CLUT_4BPP,
    HI_DRV_PIXEL_FMT_CLUT_8BPP,
    HI_DRV_PIXEL_FMT_ACLUT_44,
    HI_DRV_PIXEL_FMT_ACLUT_88,

    /* HISI 16bit RGB formats */
    HI_DRV_PIXEL_FMT_ARGB4444,
    HI_DRV_PIXEL_FMT_ABGR4444,
    HI_DRV_PIXEL_FMT_RGBA4444,

    HI_DRV_PIXEL_FMT_ARGB1555,
    HI_DRV_PIXEL_FMT_ABGR1555,
    HI_DRV_PIXEL_FMT_RGBA5551,

    /* HISI 24bit RGB formats */
    HI_DRV_PIXEL_FMT_ARGB8565,
    HI_DRV_PIXEL_FMT_ABGR8565,
    HI_DRV_PIXEL_FMT_RGBA5658,

    HI_DRV_PIXEL_FMT_ARGB6666,
    HI_DRV_PIXEL_FMT_RGBA6666,

    /* HISI 32bit RGB formats */
    HI_DRV_PIXEL_FMT_ARGB8888,
    HI_DRV_PIXEL_FMT_ABGR8888,
    HI_DRV_PIXEL_FMT_RGBA8888,

    HI_DRV_PIXEL_FMT_AYUV8888,
    HI_DRV_PIXEL_FMT_YUVA8888,

    /* grey formats */
    HI_DRV_PIXEL_FMT_GREY,        /*  8  greyscale     */
    HI_DRV_PIXEL_FMT_Y4,          /*  4  greyscale     */
    HI_DRV_PIXEL_FMT_Y6,          /*  6  greyscale     */
    HI_DRV_PIXEL_FMT_Y10,         /* 10  greyscale     */
    HI_DRV_PIXEL_FMT_Y12,         /* 12  greyscale     */
    HI_DRV_PIXEL_FMT_Y16,         /* 16  greyscale     */

    /* grey bit-packed formats */
    HI_DRV_PIXEL_FMT_Y10BPACK,       /* 10  greyscale bit-packed */

    /* palette formats */
    HI_DRV_PIXEL_FMT_PAL8,        /*  8  8-bit palette */

    /* luminance+chrominance formats */
    HI_DRV_PIXEL_FMT_YVU410,      /*  9  YVU 4:1:0     */
    HI_DRV_PIXEL_FMT_YVU420,      /* 12  YVU 4:2:0     */
    HI_DRV_PIXEL_FMT_YUYV,        /* 16  YUV 4:2:2     */
    HI_DRV_PIXEL_FMT_YYUV,        /* 16  YUV 4:2:2     */
    HI_DRV_PIXEL_FMT_YVYU,        /* 16  YVU 4:2:2 */
    HI_DRV_PIXEL_FMT_UYVY,        /* 16  YUV 4:2:2     */
    HI_DRV_PIXEL_FMT_VYUY,        /* 16  YUV 4:2:2     */
    HI_DRV_PIXEL_FMT_YUV422P,     /* 16  YVU422 planar */
    HI_DRV_PIXEL_FMT_YUV411P,     /* 16  YVU411 planar */
    HI_DRV_PIXEL_FMT_Y41P,        /* 12  YUV 4:1:1     */
    HI_DRV_PIXEL_FMT_YUV444,      /* 16  xxxxyyyy uuuuvvvv */
    HI_DRV_PIXEL_FMT_YUV555,      /* 16  YUV-5-5-5     */
    HI_DRV_PIXEL_FMT_YUV565,      /* 16  YUV-5-6-5     */
    HI_DRV_PIXEL_FMT_YUV32,       /* 32  YUV-8-8-8-8   */
    HI_DRV_PIXEL_FMT_YUV410,      /*  9  YUV 4:1:0     */
    HI_DRV_PIXEL_FMT_YUV420,      /* 12  YUV 4:2:0     */
    HI_DRV_PIXEL_FMT_HI240,       /*  8  8-bit color   */
    HI_DRV_PIXEL_FMT_HM12,        /*  8  YUV 4:2:0 16x16 macroblocks */
    HI_DRV_PIXEL_FMT_M420,    /* 12  YUV 4:2:0 2 lines y,
                                       1 line uv interleaved */

    /* two planes -- one Y, one cr + cb interleaved  */
    HI_DRV_PIXEL_FMT_NV08,         /*08  Y/cb_cr 4:0:0 @*/
    HI_DRV_PIXEL_FMT_NV80,         /*08  Y/cr_cb 4:0:0 @*/
    HI_DRV_PIXEL_FMT_NV12,        /* 12  Y/cb_cr 4:2:0  */
    HI_DRV_PIXEL_FMT_NV21,        /* 12  Y/cr_cb 4:2:0  */
    HI_DRV_PIXEL_FMT_NV12_411,    /* 12  Y/cb_cr 4:1:1  @*/
    HI_DRV_PIXEL_FMT_NV16,        /* 16  Y/cb_cr 4:2:2  */
    HI_DRV_PIXEL_FMT_NV61,        /* 16  Y/cr_cb 4:2:2  */
    HI_DRV_PIXEL_FMT_NV16_2X1,    /* 16  Y/cb_cr 4:2:2 2X1 @*/
    HI_DRV_PIXEL_FMT_NV61_2X1,    /* 16  Y/cr_cb 4:2:2  2X1 @*/
    HI_DRV_PIXEL_FMT_NV24,        /* 24  Y/cb_cr 4:4:4  */
    HI_DRV_PIXEL_FMT_NV42,        /* 24  Y/cr_cb 4:4:4  */
    HI_DRV_PIXEL_FMT_NV42_RGB,    /* 24  G/RB 4:4:4  */

    /* two non contiguous planes - one Y, one cr + cb interleaved  */
    HI_DRV__PIXEL_FMT_NV12M,      /* 12  Y/cb_cr 4:2:0  */
    HI_DRV__PIXEL_FMT_NV12MT,     /* 12  Y/cb_cr 4:2:0 64x32 macroblocks */

    /* three non contiguous planes - Y, cb, cr */
    HI_DRV_PIXEL_FMT_YUV420M,     /* 12  YUV420 planar */

    /* bayer formats - see http://www.siliconimaging.com/RGB%20_bayer.htm */
    HI_DRV_PIXEL_FMT_SBGGR8,      /*  8  BGBG.. GRGR.. */
    HI_DRV_PIXEL_FMT_SGBRG8,      /*  8  GBGB.. RGRG.. */
    HI_DRV_PIXEL_FMT_SGRBG8,      /*  8  GRGR.. BGBG.. */
    HI_DRV_PIXEL_FMT_SRGGB8,      /*  8  RGRG.. GBGB.. */
    HI_DRV_PIXEL_FMT_SBGGR10,     /* 10  BGBG.. GRGR.. */
    HI_DRV_PIXEL_FMT_SGBRG10,     /* 10  GBGB.. RGRG.. */
    HI_DRV_PIXEL_FMT_SGRBG10,     /* 10  GRGR.. BGBG.. */
    HI_DRV_PIXEL_FMT_SRGGB10,     /* 10  RGRG.. GBGB.. */
    HI_DRV_PIXEL_FMT_SBGGR12,     /* 12  BGBG.. GRGR.. */
    HI_DRV_PIXEL_FMT_SGBRG12,     /* 12  GBGB.. RGRG.. */
    HI_DRV_PIXEL_FMT_SGRBG12,     /* 12  GRGR.. BGBG.. */
    HI_DRV_PIXEL_FMT_SRGGB12,     /* 12  RGRG.. GBGB.. */

    /* HISI three non contiguous planes - Y, cb, cr */
    HI_DRV_PIXEL_FMT_YUV400,     /*08  YUV400 planar @*/
    HI_DRV_PIXEL_FMT_YUV410P,     /*10  YUV410 planar @*/
    HI_DRV_PIXEL_FMT_YUV420P,    /*12  YUV420 planar @*/
    HI_DRV_PIXEL_FMT_YUV411,     /*12  YUV411  planar @*/
    HI_DRV_PIXEL_FMT_YUV422_1X2, /*16  YUV422  planar 1X2 @*/
    HI_DRV_PIXEL_FMT_YUV422_2X1, /*16  YUV422  planar 2X1@*/
    HI_DRV_PIXEL_FMT_YUV_444,    /*24  YUV444  planar @*/

    /* HISI three non contiguous planes - Y, cb, cr */

    HI_DRV_PIXEL_MAX
} hi_drv_pixel_format;

/* video frame filed in buffer */
typedef enum {
    HI_DRV_FIELD_TOP = 0,
    HI_DRV_FIELD_BOTTOM,
    HI_DRV_FIELD_ALL,
    HI_DRV_FIELD_MAX
} hi_drv_field_mode;

/* video frame type */
typedef enum {
    HI_DRV_3D_NONE = 0,
    HI_DRV_3D_SBS_HALF,
    HI_DRV_3D_TAB,
    HI_DRV_3D_FS,
    HI_DRV_3D_FPK,
    HI_DRV_3D_MAX
} hi_drv_3d_type;

/* frame rotation angle */
typedef enum {
    HI_DRV_ROT_ANGLE_0 = 0,
    HI_DRV_ROT_ANGLE_90,
    HI_DRV_ROT_ANGLE_180,
    HI_DRV_ROT_ANGLE_270,
    HI_DRV_ROT_ANGLE_MAX
} hi_drv_rot_angle;

/* video frame aspect ratio mode */
typedef enum {
    HI_DRV_ASP_RAT_MODE_FULL = 0x0,
    HI_DRV_ASP_RAT_MODE_LETTERBOX,
    HI_DRV_ASP_RAT_MODE_PANANDSCAN,
    HI_DRV_ASP_RAT_MODE_COMBINED,
    HI_DRV_ASP_RAT_MODE_FULL_H,
    HI_DRV_ASP_RAT_MODE_FULL_V,
    HI_DRV_ASP_RAT_MODE_CUSTOMER,
    HI_DRV_ASP_RAT_MODE_TV,
    HI_DRV_ASP_RAT_MODE_MAX
} hi_drv_asp_rat_mode;

/*Defines the stream type supported by the decoder.*/
/*CNcomment: */
typedef enum {
    HI_DRV_VCODEC_MPEG2,
    HI_DRV_VCODEC_MPEG4,       /*<MPEG4*/
    HI_DRV_VCODEC_AVS,
    HI_DRV_VCODEC_H263,
    HI_DRV_VCODEC_H264,
    HI_DRV_VCODEC_REAL8,
    HI_DRV_VCODEC_REAL9,
    HI_DRV_VCODEC_VC1,
    HI_DRV_VCODEC_VP6,
    HI_DRV_VCODEC_VP6F,
    HI_DRV_VCODEC_VP6A,
    HI_DRV_VCODEC_MJPEG,
    HI_DRV_VCODEC_SORENSON,    /*<SORENSON SPARK */
    HI_DRV_VCODEC_DIVX3,       /*<Not Supportted */
    HI_DRV_VCODEC_RAW,
    HI_DRV_VCODEC_JPEG,        /*<Used only for VENC */
    HI_DRV_VCODEC_VP8,
    HI_DRV_VCODEC_MSMPEG4V1,   /*<MS private MPEG4 */
    HI_DRV_VCODEC_MSMPEG4V2,
    HI_DRV_VCODEC_MSVIDEO1,    /*<MS video */
    HI_DRV_VCODEC_WMV1,
    HI_DRV_VCODEC_WMV2,
    HI_DRV_VCODEC_RV10,
    HI_DRV_VCODEC_RV20,
    HI_DRV_VCODEC_SVQ1,        /*<Apple video */
    HI_DRV_VCODEC_SVQ3,        /*<Apple video */
    HI_DRV_VCODEC_H261,
    HI_DRV_VCODEC_VP3,
    HI_DRV_VCODEC_VP5,
    HI_DRV_VCODEC_CINEPAK,
    HI_DRV_VCODEC_INDEO2,
    HI_DRV_VCODEC_INDEO3,
    HI_DRV_VCODEC_INDEO4,
    HI_DRV_VCODEC_INDEO5,
    HI_DRV_VCODEC_MJPEGB,
    HI_DRV_VCODEC_MVC,
    HI_DRV_VCODEC_HEVC,
    HI_DRV_VCODEC_DV,
    HI_DRV_VCODEC_VP9,
    HI_DRV_VCODEC_AVS2,
    HI_DRV_VCODEC_AV1,
    HI_DRV_VCODEC_AVS3,
    HI_DRV_VCODEC_MAX
} hi_drv_vcodec_type;

typedef enum {
    HI_DRV_3D_EYE_LEFT  = 0,
    HI_DRV_3D_EYE_RIGHT = 1,  /* only for right eye frame of 3D video */
    HI_DRV_3D_EYE_MAX
} hi_drv_3d_eye_type;

/*Defines the type of the video frame.*/
/*CNcomment: 定义视频帧的类型枚举*/
typedef enum {
    HI_DRV_FRAME_TYPE_UNKNOWN,   /*<Unknown*/ /*<CNcomment: 未知的帧类型*/
    HI_DRV_FRAME_TYPE_I,         /*<I frame*/ /*<CNcomment: I帧*/
    HI_DRV_FRAME_TYPE_P,         /*<P frame*/ /*<CNcomment: P帧*/
    HI_DRV_FRAME_TYPE_B,         /*<B frame*/ /*<CNcomment: B帧*/
    HI_DRV_FRAME_TYPE_IDR,       /*<IDR frame*/ /*<CNcomment: IDR帧*/
    HI_DRV_FRAME_TYPE_BLA,       /*<BLA frame*/ /*<CNcomment: BLA帧*/
    HI_DRV_FRAME_TYPE_CRA,       /*<CRA frame*/ /*<CNcomment: CRA帧*/
    HI_DRV_FRAME_TYPE_MAX
} hi_drv_frame_type;

typedef struct {
    hi_u32 left_offset;
    hi_u32 top_offset;
    hi_u32 right_offset;
    hi_u32 bottom_offset;
} hi_drv_crop_rect;

typedef enum {
    HI_DRV_DISP_ASPECT_RATIO_AUTO,
    HI_DRV_DISP_ASPECT_RATIO_4TO3,
    HI_DRV_DISP_ASPECT_RATIO_16TO9,
    HI_DRV_DISP_ASPECT_RATIO_221TO100,
    HI_DRV_DISP_ASPECT_RATIO_CUSTOM,
    HI_DRV_DISP_ASPECT_RATIO_MODE_MAX
} hi_drv_disp_aspect_mode;

/* aspect ratio, for monitor or pixel.
  0<= arw <= 256, 0<=arh<=256, and '1/16  <= arw/arh <= 16'.
  e.g., if aspect ratio is 16:9, you can set arw as 16 and arh as 9,
  OR arw as 160, arh as 90.
  exceptive :
  0:1 means unknown;
  0:2 means display pixel 1:1
  */
typedef struct {
    hi_u32 aspect_ratio_w;
    hi_u32 aspect_ratio_h;
    hi_drv_disp_aspect_mode aspect_mode;
} hi_drv_aspect_ratio;

/* video frame buffer physical address */
typedef struct {
    hi_mem_handle_t start_addr;  /* frame buffer start address handle */
    hi_u64 dma_handle;
    hi_u32 y_head_offset;
    hi_u32 y_offset;
    hi_u32 c_head_offset;
    hi_u32 c_offset;
    hi_u32 cr_head_offset;
    hi_u32 cr_offset;

    hi_u32 stride_y;
    hi_u32 stride_c;
    hi_u32 stride_cr;

    hi_u32 head_stride;
    hi_u32 head_size;

    hi_u64 vir_addr_y;
    hi_u64 vir_addr_c;
} hi_drv_vid_frame_addr;

typedef enum {
    HI_DRV_SOURCE_DTV = 0,
    HI_DRV_SOURCE_USB,

    HI_DRV_SOURCE_ATV,
    HI_DRV_SOURCE_SCART,
    HI_DRV_SOURCE_SVIDEO,
    HI_DRV_SOURCE_CVBS,
    HI_DRV_SOURCE_VGA,
    HI_DRV_SOURCE_YPBPR,
    HI_DRV_SOURCE_HDMI,
    HI_DRV_SOURCE_MEDIA,
    HI_DRV_SOURCE_NETWORK,

    HI_DRV_SOURCE_MAX
} hi_drv_source;

typedef struct {
    hi_u32  omx_report_done_time;
    hi_u32  overlay_queue_time;
    hi_u32  win_get_frame_time;
    hi_u32  win_config_time;
    hi_u32  win_rls_frame_time;
} hi_drv_lowdelay_stat_info;

typedef enum {
    HI_DRV_AFD_TYPE_UNKNOW = 0,
    HI_DRV_AFD_TYPE_BOX_16_9_TOP = 2,
    HI_DRV_AFD_TYPE_BOX_14_9_TOP,
    HI_DRV_AFD_TYPE_BOX_16_9_CENTER,
    HI_DRV_AFD_TYPE_SAME_AS_SRC = 8,
    HI_DRV_AFD_TYPE_4_3,
    HI_DRV_AFD_TYPE_16_9,
    HI_DRV_AFD_TYPE_14_9,
    HI_DRV_AFD_TYPE_4_3_SHOOT_PRO_14_9 = 13,
    HI_DRV_AFD_TYPE_16_9_SHOOT_PRO_14_9,
    HI_DRV_AFD_TYPE_16_9_SHOOT_PRO_4_3,
    HI_DRV_AFD_TYPE_MAX
} hi_drv_video_afd_type;

/* last video frame flag */
typedef enum {
    HI_DRV_LAST_ERROR_FLAG = 0xff00,
    HI_DRV_LAST_FAKE_FLAG  = 0xff11,
    HI_DRV_LAST_FRAME_FLAG = 0xffee,
    HI_DRV_LAST_FLAG_MAX
} hi_drv_last_frame_flag;

typedef enum {
    HI_DRV_SAMPLE_TYPE_PROGRESSIVE,              /**<progressive*/ /**<c_ncomment: 采样方式为逐行*/
    HI_DRV_SAMPLE_TYPE_INTERLACE,                /**<interlaced*/ /**<c_ncomment: 采样方式为隔行*/
    HI_DRV_SAMPLE_TYPE_UNKNOWN,                  /**<unknown*/ /**<c_ncomment: 未知采样方式*/
    HI_DRV_SAMPLE_TYPE_MAX
} hi_drv_sample_type;

/*U-v_mos param start*/
typedef struct {
    hi_u32            frame_stream_size;
    hi_u32            avg_qp;
    hi_u32            max_mv;
    hi_u32            min_mv;
    hi_u32            avg_mv;
    hi_u32            skip_ratio;   /**<MB ratio */ /**<c_ncomment: 视频帧MB比例0~100*/
    hi_drv_frame_type frame_type;    /*I  P  B frame*/
} hi_drv_video_uvmos;
/*U-v_mos param end*/

typedef struct {
    hi_s32 rect_x;
    hi_s32 rect_y;
    hi_s32 rect_w;
    hi_s32 rect_h;
} hi_drv_rect;

typedef enum {
    HI_DRV_MODULE_AVPLAY,
    HI_DRV_MODULE_VDEC,
    HI_DRV_MODULE_VPSS,
    HI_DRV_MODULE_PQ,
    HI_DRV_MODULE_VO,
    HI_DRV_MODULE_VENC,
    HI_DRV_MODULE_MAX
} hi_drv_module_type;

typedef struct {
    hi_u32 image_id;
    hi_u32 image_id_1;
    hi_s32 gop_num;
    hi_u32 frm_type;
    hi_s32 top_field_frame_format;
    hi_s32 bot_field_frame_format;
    hi_s32 field_flag;
    hi_s32 chan_id;
} hi_drv_vdec_private;

/*module private start*/
typedef struct {
    hi_drv_module_type tpye;
    hi_u32             size;
    hi_u64             data; /* Need to modify to fd */
} hi_drv_module_private;
/*module private start end*/

typedef struct {
    hi_u8  pic_structure; /* 0:frame, 1:top, 2:bottom, 3:mbaff, 4:field pair */
    hi_u8  pic_qp_enable;
    hi_u8  chroma_format_idc; /* 0: yuv400 , 1: yuv420 */
    hi_u8  vc1_profile;

    hi_s32 qp_y;
    hi_s32 qp_u;
    hi_s32 qp_v;
    hi_s32 ranged_frm;

    hi_u8  range_mapy_flag;
    hi_u8  range_mapy;
    hi_u8  range_mapuv_flag;
    hi_u8  range_mapuv;

    hi_u8  bottom_range_mapy_flag;
    hi_u8  bottom_range_mapy;
    hi_u8  bottom_range_mapuv_flag;
    hi_u8  bottom_range_mapuv;
} hi_drv_vc1_range_info;

/*private struct*/
typedef struct {
    hi_drv_source         source;       /*DTV default HI_DRV_SOURCE_DTV*/
    hi_drv_last_frame_flag                last_flag;    /*end frame*/
    hi_drv_color_sys      color_sys;    /*DTV default HI_DRV_COLOR_SYS_AUTO*/
    hi_drv_vc1_range_info vc1_range;
    hi_u32                frm_cnt;
    hi_u32                priv_disp_time;    /*this displaytime is for pvr smooth tplay*/
    hi_drv_module_private vdec_private;
    hi_drv_module_private vpss_private;
    hi_drv_module_private win_private;
    hi_drv_module_private venc_private;
} hi_drv_video_private;

typedef enum {
    DMA_BUF_HEAD_ADDR_HIGH_BIT = 0,
    DMA_BUF_ADDR_HIGH_BIT,
    DMA_BUF_HEAD_ADDR_LOW_BIT,
    DMA_BUF_ADDR_LOW_BIT,
    DMA_BUF_ADDR_MAX,
} hi_drv_dma_buf_addr_type;

/* video frame buffer physical address */
typedef struct {
    /* Y address*/
    hi_u64  phy_addr_y_head; /* only for compress format */
    hi_u64  phy_addr_y;

    /* C OR cb address*/
    hi_u64  phy_addr_c_head;
    hi_u64  phy_addr_c;

    /* cr address*/
    hi_u64  phy_addr_cr_head;
    hi_u64  phy_addr_cr;
} hi_drv_vid_frame_phy_addr;

typedef struct {
    hi_u32 size;
    hi_u64 phy_addr;
    hi_u8 *vir_addr;
    hi_u64 dma_buf;
}mem_map_info;

typedef struct {
    /* vpss use */
    hi_bool fence_is_destroyed;
    hi_bool vpss_process;
    hi_s32 film_type;
    hi_u32 key_value;
    hi_bool freeze_frame;
    hi_bool vpss_reset_frame;
    hi_bool src_interlace;
    hi_bool vpss_crop;
    hi_bool pause_frame;
    hi_bool single_field_mode;
    hi_bool vdp_advance_frame;
    hi_bool force_bypass;
    hi_bool force_pq_close;

    /* win use for stb */
    hi_u32 play_index;
    /* vpss should copy the index to new frame, for ai match. */
    hi_u32 parent_frame_index;
    hi_u64 android_fence_fd;

    hi_u32 play_cnt;
    hi_bool played_cnt;
    hi_u32  frc_play_cnt;

    hi_u64 left_eye_dma_buf;
    hi_u64 right_eye_dma_buf;

    hi_void *hdr_mentadata;
    hi_void *hdr_mentadata_len;
    hi_bool is_displayed;

    /* win use for dpt */
    hi_bool fence_frame;
    hi_handle win_handle;
    hi_u64 hdr_post_process_addr;
    hi_u32 dolby_back_light;
    hi_u32 dolby_back_light_delay_ms;
    hi_u32 frame_cmp_size;
    hi_u32 src_width;
    hi_u32 src_height;

    hi_u32 debug_delay_time;
    hi_u32 debug_queue_time;
    hi_u32 debug_cfg_time;
    hi_s32 sync_adjust_cnts;

    hi_drv_vid_frame_phy_addr     buf_addr[HI_DRV_3D_EYE_MAX];
    hi_drv_vid_frame_phy_addr     buf_addr_lb[HI_DRV_3D_EYE_MAX];

    mem_map_info     buf_mapinfo[HI_DRV_3D_EYE_MAX];
    mem_map_info     buf_mapinfo_lb[HI_DRV_3D_EYE_MAX];
} hi_drv_win_vpss_meta_info;

typedef enum {
    HI_DRV_COMPRESS_MODE_OFF = 0,
    HI_DRV_COMPRESS_MODE_SEG_LOSS,
    HI_DRV_COMPRESS_MODE_SEG_LOSSLESS,
    HI_DRV_COMPRESS_MODE_LINE_LOSS,
    HI_DRV_COMPRESS_MODE_LINE_LOSSLESS,
    HI_DRV_COMPRESS_MODE_FRM_LOSS,
    HI_DRV_COMPRESS_MODE_FRM_LOSSLESS,
    HI_DRV_COMPRESS_MODE_MAX
} hi_drv_compress_mode;

typedef enum {
    HI_DRV_DATA_FMT_LINER= 0x0,
    HI_DRV_DATA_FMT_TILE,
    HI_DRV_DATA_FMT_PKG,
    HI_DRV_DATA_FMT_AFBC,
    HI_DRV_DATA_FMT_MAX
} hi_drv_data_fmt;

typedef struct {
    hi_drv_compress_mode cmp_mode; /* compress mode. def: 0 */
    hi_drv_data_fmt data_fmt;  /* data mode. def: 0 */
    hi_u32 ycmp_rate;              /* 0-1000, def: 0 */
    hi_u32 ccmp_rate;              /* 0-1000, def: 0 */
} hi_drv_compress_info;

typedef struct {
    hi_bool need_display;
} hi_drv_fence_info;

typedef struct {
    hi_u32                    frame_index;

    hi_s32                    source_fence;
    hi_s32                    sink_fence;

    /* st_buf_addr[1] is right eye for stereo video */
    hi_drv_vid_frame_addr     buf_addr[HI_DRV_3D_EYE_MAX];
    hi_drv_vid_frame_addr     buf_addr_lb[HI_DRV_3D_EYE_MAX];

    /* tunnel attributes */
    hi_u64                    tunnel_phy_addr;
    hi_u64                    back_tunnel_phy_addr;
    hi_handle                 tunnel_handle;

    hi_u32                    width;
    hi_u32                    height;
    hi_u16                    disp_ctrl_width;
    hi_u16                    disp_ctrl_height;

    hi_u8                     dv_dual_layer;
    hi_u8                     single_i_frame;
    hi_s64                    src_pts;  /* original pts from the stream */
    hi_s64                    pts;      /* pts after adjust */

    hi_bool                   discard;       /*when error_level than error_thread, this frame is discard and is_discard is 1*/
    hi_u32                    aspect_width;
    hi_u32                    aspect_height;

    hi_u32                    frame_rate;    /* in 1/1000 hz, 0 means unknown */
    hi_u32                    ori_frame_rate;

    hi_drv_pixel_format       pixel_format;
    hi_drv_compress_info      cmp_info;
    hi_drv_sample_type        sample_type;
    hi_drv_field_mode         field_mode;
    hi_drv_field_mode         buf_valid_mode;
    hi_bool                   top_field_first;
    hi_u8                     repeat_first_field;

    hi_bool                   compressd;
    hi_drv_pixel_bitwidth     bit_width;

    /*display region in rectangle (x,y,w,h)*/
    hi_drv_rect               disp_rect;

    hi_drv_3d_type            video_3d_type;

    hi_bool                   flip_h;
    hi_bool                   flip_v;

    hi_u32                    error_level;

    hi_drv_video_private      video_private;
    hi_bool                   secure;
    hi_drv_vcodec_type        codec_type;

    hi_drv_hdr_type           hdr_type;
    hi_drv_hdr_metadata       hdr_info;

    /* current frame color_space, when DTV VPSS will set this, when atv vicap will set this */
    hi_drv_color_descript     color_space;
    hi_drv_lowdelay_stat_info lowdelay_stat;

    hi_drv_video_afd_type     afd_format;
} hi_drv_video_frame;

#define DEF_HI_DRV_VIDEO_BUFFER_MAX_NUMBER 16

/* extern frame buffer setting for virtual window,
   user alloc memory and set this setting to virtual window.*/
typedef struct {
    hi_s32   buf_num;
    hi_s32   buf_stride;
    hi_s32   buf_height;
    hi_u64 phy_addr[DEF_HI_DRV_VIDEO_BUFFER_MAX_NUMBER];
} hi_drv_video_buffer_pool;

/* window private information, it could be calllback to win-sourece */
typedef struct {
    hi_drv_pixel_format pixel_fmt;

    hi_bool                  use_crop_rect;
    hi_drv_rect              in_rect;             /* (0,0,0,0) means full imgae, not clip */
    hi_drv_crop_rect         crop_rect;
    hi_drv_rect              out_rect;

    /* may change when window lives */
    hi_drv_aspect_ratio      asp_custm_;
    hi_drv_asp_rat_mode      asp_cvrs;

    /* external buffer config */
    hi_bool                  use_ext_buf;
    hi_drv_video_buffer_pool ext_buf_pool;

    /* display info */
    hi_bool                  interlaced;
    hi_drv_rect              screen;
    hi_drv_aspect_ratio      asp_screen;
    hi_bool                  in3_d_mode;
    hi_bool                  tunnel_support;
    hi_drv_rot_angle         rotation;
    hi_bool                  vert_flip;
    hi_bool                  hori_flip;
    hi_bool                  pass_through;

    /*display max_rate*/
    hi_u32                   max_rate;     /* in 1/100 hz', if 0, full rate */
    hi_bool                  compress_flag;     /* in 1/100 hz', if 0, full rate */
    hi_drv_pixel_bitwidth    bit_width;
} hi_drv_win_priv_info;

#endif /* __HI_DRV_VIDEO_H__ */

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq common define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_COMM_H__
#define __DRV_PQ_COMM_H__

#include "drv_pq_ext.h"
#include "hi_drv_pq.h"
#include "hi_vpss_register.h"
#include "drv_pq_mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NUM2LEVEL(num)   (((num)*PQ_ALG_MAX_VALUE + PQ_CSC_VIDEO_DEFAULT - 1) / PQ_CSC_VIDEO_MAX)
#define LEVEL2NUM(level) (((level)*PQ_CSC_VIDEO_MAX + PQ_CSC_PICTURE_DEFAULT) / PQ_ALG_MAX_VALUE)

/* 0~1023 trans to 0~100 */
#define u1023_to_u100(level) (((level)*PQ_ALG_MAX_VALUE + PQ_UNF_PARA_DEFAULT - 1) / PQ_UNF_PARA_MAX)
/* 0~100 trans to 0~1023 */
#define u100_to_u1023(level) (((level)*PQ_UNF_PARA_MAX + PQ_CSC_PICTURE_DEFAULT) / PQ_ALG_MAX_VALUE)

#define ALG_NAME_MAX_LEN        32
#define GET_ALG(alg_id)         pq_comm_get_alg(alg_id)
#define GET_ALG_FUN(alg_id)     pq_comm_get_alg(alg_id)->fun
#define PQ_FUNC_CALL(alg, func) (GET_ALG(alg) && GET_ALG_FUN(alg)->func)
#define MAX2(x, y)              ((x) > (y) ? (x) : (y))
#define MIN2(x, y)              ((x) < (y) ? (x) : (y))
#define MAX3(x, y, z)           ((x) > (y) ? MAX2(x, z) : MAX2(y, z))
#define MIN3(x, y, z)           ((x) < (y) ? MIN2(x, z) : MIN2(y, z))
#define pq_abs(x)               ((x) >= 0 ? (x) : (-(x)))
/*********************tmp define start*****************************/
/* source color space */
typedef enum {
    HI_DRV_CS_UNKNOWN = 0,
    HI_DRV_CS_DEFAULT,

    HI_DRV_CS_BT601_YUV_LIMITED, /* BT.601 */
    HI_DRV_CS_BT601_YUV_FULL,
    HI_DRV_CS_BT601_RGB_LIMITED,
    HI_DRV_CS_BT601_RGB_FULL,

    HI_DRV_CS_NTSC1953,

    /* these should be useful.  assume 601 extents. */
    HI_DRV_CS_BT470_SYSTEM_M,
    HI_DRV_CS_BT470_SYSTEM_BG,

    HI_DRV_CS_BT709_YUV_LIMITED, /* BT.709 */
    HI_DRV_CS_BT709_YUV_FULL,
    HI_DRV_CS_BT709_RGB_LIMITED,
    HI_DRV_CS_BT709_RGB_FULL,
    HI_DRV_CS_BT2020_YUV_LIMITED, /* ::::current used:::: BT.2020 */
    HI_DRV_CS_BT2020_YUV_FULL,
    HI_DRV_CS_BT2020_RGB_LIMITED,
    HI_DRV_CS_BT2020_RGB_FULL,   /* ::::current used:::: */

    HI_DRV_CS_REC709,      /* HD and modern captures. */

    HI_DRV_CS_SMPT170M, /* ITU-R 601 -- broadcast NTSC/PAL */
    HI_DRV_CS_SMPT240M, /* 1125-line (US) HDTV */

    HI_DRV_CS_BT878,    /* broken BT878 extents
                           (601, luma range 16-253 instead of 16-235) */

    HI_DRV_CS_XVYCC,

    /* I know there will be cameras that send this.  so, this is
     * unspecified chromaticities and full 0-255 on each of the
     * Y'cb_cr components
     */
    HI_DRV_CS_JPEG,
    HI_DRV_CS_MAX
} drv_pq_internal_color_space;

typedef struct {
    hi_bool csc_en;      /* Current Csc Enable: 1:Open; 0:Close */
    hi_u32 csc_pecision; /* Current Csc Pecision: 10~15 */
    hi_drv_pq_csc_type csc_type;

    drv_pq_internal_color_space csc_in;
    drv_pq_internal_color_space csc_out;
} drv_pq_internal_csc_info;

typedef struct {
    hi_bool is_bgr_in;               /* Is BGR in */
    drv_pq_internal_csc_info gfx_csc_mode; /* input and output gfxcsc mode */
} drv_pq_internal_gfx_csc_info;

typedef struct {
    hi_drv_hdr_type src_hdr_type;       /* input : SDR/HDR10/... */
    hi_drv_hdr_type disp_hdr_type;      /* output : SDR/HDR10/... */
    drv_pq_internal_color_space color_space_in;  /* input color */
    drv_pq_internal_color_space color_space_out; /* output color */
} drv_pq_internal_gfx_hdr_info;

typedef struct {
    hi_u32 window_num; /* window num : 1,vo, >1:multi, 0:invaild */

    hi_u32 handle;
    hi_drv_hdr_type src_hdr_type;  /* src type: SDR/HDR10/... */
    hi_drv_hdr_type disp_hdr_type; /* out type: SDR/HDR10/... */

    union {
        hi_drv_hdr_static_metadata hdr10_info; /* HDR10 frame info. */
#ifdef HI_HDR10PLUS_SUPPORT
        hi_drv_hdr_hdr10plus_metadata hdr10plus_info; /* HDR10+ frame info. */
#endif
        hi_drv_hdr_hlg_metadata hlg_info; /* HLG frame info. */
    } hdr_info;

    drv_pq_internal_color_space color_space_in;  /* input color */
    drv_pq_internal_color_space color_space_out; /* output color */
} drv_pq_internal_xdr_frame_info;

/* ********************tmp define endf**************************** */
/* PQ reg type */
typedef enum {
    REG_TYPE_VPSS = 0,
    REG_TYPE_VDP = 1,
    REG_TYPE_ALL = 2,

    REG_TYPE_MAX,
} pq_reg_type;

/* enum define about ZME enable mode */
typedef enum {
    PQ_ZME_MODE_VDP_FIR = 0, /* VDP ZME fir mode */
    PQ_ZME_MODE_VDP_MED,     /* VDP ZME med mode */
    PQ_ZME_MODE_VPSS_FIR,    /* VPSS ZME fir mode */
    PQ_ZME_MODE_VPSS_MED,    /* VPSS ZME med mode */
    PQ_ZME_MODE_GFX_FIR,     /* GFX ZME fir mode */
    PQ_ZME_MODE_GFX_MED,     /* GFX ZME med mode */

    PQ_ZME_MODE_MAX
} pq_zme_mode;

typedef enum {
    PQ_DEMO_ENABLE_L = 0,
    PQ_DEMO_ENABLE_R,

    PQ_DEMO_MAX
} pq_demo_mode;

typedef enum {
    PQ_BIN_ADAPT_SINGLE = 0,
    PQ_BIN_ADAPT_MULTIPLE,

    PQ_BIN_ADAPT_MAX
} pq_bin_adapt_type;

typedef enum {
    PQ_DIE_OUT_MODE_AUTO = 0,
    PQ_DIE_OUT_MODE_FIR,
    PQ_DIE_OUT_MODE_COPY,

    PQ_DIE_OUT_MODE_MAX
} hi_pq_die_out_mode;

typedef enum {
    PQ_FOD_ENABLE_AUTO  = 1,
    PQ_FOD_TOP_FIRST    = 2,
    PQ_FOD_BOTTOM_FIRST = 3,
    PQ_FOD_UNKOWN = 4,

    PQ_FOD_MODE_MAX
} hi_pq_fod_mode;

/* layer ID */
typedef enum {
    PQ_VDP_LAYER_VID0 = 0, /* VDP_LAYER_V0 */
    PQ_VDP_LAYER_VID1,     /* VDP_LAYER_V1 */
    PQ_VDP_LAYER_VID2,     /* VDP_LAYER_V2 */
    PQ_VDP_LAYER_VID3,     /* VDP_LAYER_V3 */
    PQ_VDP_LAYER_VID4,     /* VDP_LAYER_V4 */

    PQ_VDP_LAYER_VID_MAX
} hi_pq_vdp_layer_vid;

typedef enum {
    PQ_VDP_LAYER_VP0 = 0,
    PQ_VDP_LAYER_VP1 = 1,
    PQ_VDP_LAYER_DHD0 = 2,

    PQ_VDP_LAYER_VP_MAX
} pq_layer_vp;

typedef enum {
    PQ_ACM_LINK_V0 = 0,
    PQ_ACM_LINK_VP = 1,
    PQ_ACM_LINK_MAX
} pq_acm_link;

/* VPSS WBC 传递给de-blocking运算必须的信息 */
typedef struct {
    hi_u32 handle_id;

    hi_u32 bit_depth; /* 通路处理位宽 */
    hi_u32 width;     /* 实际处理的宽度 */
    hi_u32 height;    /* 实际处理的高度 */
    hi_u32 left_offset;
    hi_u32 top_offset;
    hi_u32 right_offset;
    hi_u32 bottom_offset;

    hi_bool db_blk_pos_mode_en; /* VDH 信息使能 */
    hi_bool db_flt_det_mode_en; /* 检测开关 */
    hi_bool db_lum_hor_en;      /* 水平滤波开关 */
    hi_bool db_lum_ver_en;      /* 垂直滤波开关 */

    hi_u32 single_mode_flag; /* 单场模式标识; 1:单场;0:非单场 */
    hi_u32 db_dei_en_flag;   /* DEI 使能标识 */

    hi_u32 protocol;        /* 解码协议 */
    hi_u32 image_avg_qp;    /* 图像平均 QP */
    hi_u32 vdh_info_addr;   /* dbinfo 地址 */
    hi_u32 vdh_info_stride; /* 信息stride, 算法约定为(图像宽度+7)/8 */
    hi_u32 image_struct;    /* 帧/场编码标识 */
    hi_u32 flag_line;       /* 当前场极性标识 */

    vpss_stt_reg_type *vpss_wbc_reg;
} hi_pq_db_calc_info;

typedef enum {
    PQ_SOURCE_MODE_NO = 0,
    PQ_SOURCE_MODE_SD,
    PQ_SOURCE_MODE_FHD,
    PQ_SOURCE_MODE_4K,
    PQ_SOURCE_MODE_8K,

    PQ_SOURCE_MODE_MAX,
} pq_source_mode;

typedef enum {
    PQ_OUTPUT_MODE_NO = 0,
    PQ_OUTPUT_MODE_SD,
    PQ_OUTPUT_MODE_FHD,
    PQ_OUTPUT_MODE_4K,
    PQ_OUTPUT_MODE_8K60,
    PQ_OUTPUT_MODE_8K120,
    PQ_OUTPUT_MODE_PREVIEW,
    PQ_OUTPUT_MODE_REMOTE,

    PQ_OUTPUT_MODE_MAX,
} pq_output_mode;

/* VDP CSC 属性 */
typedef struct {
    hi_bool csc_en;
    drv_pq_internal_color_space csc_mode;
} hi_pq_vdp_csc;

typedef enum {
    HI_PQ_PROC_ZME_NUM_ORI = 0,
    HI_PQ_PROC_ZME_NUM_ONLY_ZME1 = 1, /* NO ZME2 */

    HI_PQ_PROC_ZME_NUM_MAX
} hi_pq_proc_zme_num;

typedef enum {
    HI_PQ_PROC_PREZME_HOR_ORI = 0,
    HI_PQ_PROC_PREZME_HOR_1X = 1,
    HI_PQ_PROC_PREZME_HOR_2X = 2,
    HI_PQ_PROC_PREZME_HOR_4X = 4,
    HI_PQ_PROC_PREZME_HOR_8X = 8,

    HI_PQ_PROC_PREZME_HOR_MAX
} hi_pq_proc_prezme_hor_mul;

typedef enum {
    HI_PQ_PROC_PREZME_VER_ORI = 0,
    HI_PQ_PROC_PREZME_VER_1X = 1,
    HI_PQ_PROC_PREZME_VER_2X = 2,
    HI_PQ_PROC_PREZME_VER_4X = 4,
    HI_PQ_PROC_PREZME_VER_8X = 8,

    HI_PQ_PROC_PREZME_VER_MAX
} hi_pq_proc_prezme_ver_mul;

typedef struct {
    hi_u32 zme_w_in;
    hi_u32 zme_h_in;
    hi_u32 zme_w_out;
    hi_u32 zme_h_out;

    hi_pq_proc_zme_num proc_vdp_zme_num;
    hi_bool proc_vdp_hpzme_en;
    hi_pq_proc_prezme_hor_mul proc_vdp_width_draw_mode;
    hi_pq_proc_prezme_ver_mul proc_vdp_height_draw_mode;
    hi_bool proc_hdcp_en;
    hi_drv_pq_zme_common_out zme_out[HI_DRV_PQ_NODE_MAX];
} hi_pq_proc_vdpzme_strategy;

typedef struct {
    hi_bool is_equal_ratio;
    hi_drv_pq_zme_layer layer_id;
    hi_drv_pq_zme_strategy_in zme_in;
    hi_drv_pq_zme_strategy_out zme_out;
} hi_pq_proc_get_vdpzme_strategy;

typedef struct {
    hi_drv_rect vdp_in_reso;
    hi_drv_rect vdp_outreso;
    hi_drv_rect vdp_zme1_inreso;
    hi_drv_rect vdp_zme1_outreso;
    hi_drv_rect vdp_zme2_inreso;
    hi_drv_rect vdp_zme2_outreso;
} hi_pq_proc_vdpzme_reso;

typedef enum {
    HI_PQ_PROC_WBC_LAYER_DHD = 0,
    HI_PQ_PROC_WBC_LAYER_VP = 1,

    HI_PQ_PROC_WBC_LAYER_MAX
} hi_pq_proc_wbc_layer;

typedef struct {
    hi_u32 point_sel; /* wbcvp : 0:VP,1:V0;  wbcdhd : 0:CBN,1:GP */
    hi_bool p2i_en;
    hi_drv_rect zme1_inreso;
    hi_drv_rect zme1_outreso;
    hi_drv_rect zme2_inreso;
    hi_drv_rect zme2_outreso;
    hi_drv_pq_zme_offset zmeoffset;
} hi_pq_proc_wbc_strategy;

typedef struct {
    hi_drv_rect src_reso;
    hi_drv_rect zme1_in_reso;
    hi_drv_rect zme1_out_reso;
    hi_drv_rect zme2_in_reso;
    hi_drv_rect zme2_out_reso;
    hi_drv_rect intf_reso;
} hi_pq_sharpen_reso;

typedef enum {
    PQ_TUN_NORMAL = 0,
    PQ_TUN_DEBUG,

    PQ_TUN_MAX
} pq_tun_mode;

typedef enum {
    PQ_IMAGE_MODE_NORMAL = 0,
    PQ_IMAGE_MODE_VIDEOPHONE = 1,
    PQ_IMAGE_MODE_GALLERY = 2,

    PQ_IMAGE_MODE_MAX,
} pq_comm_image_mode;

typedef struct {
    hi_drv_pq_gfx_layer gfx_layer;
    hi_bool is_bgr_in;   /* Is BGR in */
    hi_bool csc_en;      /* Current Csc Enable: 1:Open; 0:Close */
    hi_u32 csc_pecision; /* Current Csc Pecision: 10~15 */
    hi_drv_pq_csc_type csc_type;

    drv_pq_internal_color_space csc_in;
    drv_pq_internal_color_space csc_out;
} pq_gfx_csc_proc_info;

/* PQ status info */
typedef struct {
    hi_bool isogeny_mode;
    hi_bool b3d_type;
    hi_drv_pq_vdp_channel_timing timing_info[HI_PQ_DISPLAY_MAX];
    pq_tun_mode tun_mode;
    pq_comm_image_mode image_mode;
} pq_common_status;

typedef enum {
    CLE_ID_V0_4K = 0,
    CLE_ID_V0_8K = 1,
    CLE_ID_MAX
} pq_cle_id;

typedef struct {
    hi_bool   cle_enable;
    hi_bool   clm_enable;

    hi_u32    video_in_width;
    hi_u32    video_in_height;
    hi_u32    cle_in_width;
    hi_u32    cle_in_height;
    hi_u32    video_out_width;
    hi_u32    video_out_height;
} pq_cle_reso;

typedef struct {
    hi_bool  sr_4k_en;
    hi_bool  sr_8k_en;
    hi_u32   sr_4k_in_width;
    hi_u32   sr_4k_in_height;
    hi_u32   sr_8k_out_width;
    hi_u32   sr_8k_out_height;
} pq_dci_ref_reso;

typedef struct {
    pq_cle_reso cle_cfg_proc[CLE_ID_MAX];
    hi_u32      cle_min_scale[CLE_ID_MAX];
    hi_bool     cle_enable[CLE_ID_MAX];
} pq_cle_cfg_proc;

typedef enum {
    SR_ID_4K = 0,
    SR_ID_8K = 1,
    SR_ID_MAX
} pq_sr_id;

typedef struct {
    hi_u32    video_in_width;
    hi_u32    video_in_height;
    hi_u32    sharpen_in_width;
    hi_u32    sharpen_in_height;
    hi_u32    video_out_width;
    hi_u32    video_out_height;
} pq_sharpen_reso;

typedef struct {
    pq_sharpen_reso sharp_reso;
    hi_u32      sharp_min_scale_ratio;
    hi_bool     sharp_enable;
} pq_sharp_cfg_proc;

typedef enum {
    DCI_LINK_BEFORE_SR = 0,
    DCI_LINK_AFTER_SR,

    DCI_LINK_MAX
} pq_dci_link_type;

typedef struct {
    hi_bool          dci_enable;
    pq_dci_ref_reso  dci_reso;
    pq_dci_link_type link_type;
} pq_dci_cfg_proc;

/******************************************Proc struct***********************************************/
typedef struct {
    hi_drv_pq_gfx_layer gfx_layer;
    hi_bool gfx_zme_fir_update;
    hi_bool gfx_zme_offset_update;
    hi_drv_pq_fir_mode gfx_zme_fir_mode;
    hi_bool gfx_zme_med_en;
    hi_drv_pq_gfx_zme_common_out gfx_zme_out;
} gfx_zme_strategy_proc;

/* ******************************************Register function********************************************* */
typedef struct {
    hi_drv_pq_xdr_layer_id layer_id;
    hi_drv_hdr_type src_hdr_type;  /* src type: SDR/HDR10/... */
    hi_drv_hdr_type disp_hdr_type; /* out type: SDR/HDR10/... */
    hi_drv_color_descript color_space_in;  /* input color */
    hi_drv_color_descript color_space_out; /* output color */
    hi_bool hdrv1_cm_en;
    hi_bool imap_en;
    hi_bool tmapv2_en;
    hi_bool omap_en;
} pq_hdr_cfg_proc;

typedef struct {
    hi_bool enable;
    hi_drv_hdr_type src_hdr_type;  /* src type: SDR/HDR10/... */
    hi_drv_hdr_type disp_hdr_type; /* out type: SDR/HDR10/... */
    drv_pq_internal_color_space color_space_in;  /* input color */
    drv_pq_internal_color_space color_space_out; /* output color */
    hi_bool hdrv1_cm_en;
    hi_bool imap_en;
    hi_bool tmapv2_en;
} pq_vpsshdr_cfg_proc;
/* ******************************************Register function********************************************* */
typedef struct {
    /* common */
    hi_s32 (*init)(pq_bin_param *param, hi_bool b_para_use_table_default);
    hi_s32 (*deinit)(hi_void);
    hi_s32 (*set_enable)(hi_bool b_on_off);
    hi_s32 (*get_enable)(hi_bool *pb_on_off);
    hi_s32 (*set_debug_en)(hi_bool on_off);
    hi_s32 (*set_strength)(hi_u32 strength);
    hi_s32 (*get_strength)(hi_u32 *pu32_strength);
    hi_s32 (*set_demo)(hi_bool b_on_off);
    hi_s32 (*get_demo)(hi_bool *pb_on_off);
    hi_s32 (*set_demo_mode)(pq_demo_mode en_mode);
    hi_s32 (*get_demo_mode)(pq_demo_mode *pen_mode);
    hi_s32 (*set_demo_mode_coor)(hi_u32 x_pos);
    hi_s32 (*set_isogeny_mode)(hi_bool b_isogeny_mode);
    hi_s32 (*get_isogeny_mode)(hi_bool *pb_isogeny_mode);
    hi_s32 (*set_resume)(hi_bool b_default);
    hi_s32 (*get_vdp_hdr_cfg)(hi_void);
    hi_s32 (*get_vpss_hdr_cfg)(hi_void);
    /* CSC */
    hi_s32 (*get_csc_video_setting)(hi_pq_display en_disp, hi_pq_image_param *pst_pic_setting);
    hi_s32 (*set_csc_video_setting)(hi_pq_display en_disp, hi_pq_image_param *pst_pic_setting);
    hi_s32 (*get_csc_coef)(hi_drv_pq_csc_layer layer_id, drv_pq_internal_csc_info csc_info,
                           hi_drv_pq_csc_coef *csc_coef);
    hi_s32 (*get_color_temp)(hi_pq_display en_disp, hi_pq_color_temperature *pst_temperature);
    hi_s32 (*set_color_temp)(hi_pq_display en_disp, hi_pq_color_temperature *pst_temperature);
    /* GFX CSC */
    hi_s32 (*get_gfx_csc_coef)(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_csc_info *gfx_csc_info,
                               hi_drv_pq_gfx_csc_coef *gfx_csc_coef);
    hi_s32 (*get_gfx_csc_picture_setting)(hi_pq_display en_disp, hi_pq_image_param *pst_pic_setting);
    hi_s32 (*set_gfx_csc_picture_setting)(hi_pq_display en_disp, hi_pq_image_param *pst_pic_setting);
    hi_void (*get_gfx_csc_info)(pq_gfx_csc_proc_info *gfx_csc_out);

    /* ZME */
    hi_s32 (*set_vdp_zme_mode)(pq_zme_mode en_mode, hi_bool b_on_off);
    hi_s32 (*get_vdp_zme_mode)(pq_zme_mode en_mode, hi_bool *pb_on_off);
    hi_s32 (*get_vdp_zme_coef)(hi_drv_pq_zme_coef_in zme_coef_in,
                               hi_drv_pq_zme_coef_out *zme_coef_out);
    hi_s32 (*get_vdp_zme_strategy)(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                   hi_drv_pq_zme_strategy_out *zme_out);
    hi_s32 (*set_vdp_zme_default)(hi_bool b_on_off);
    hi_s32 (*set_vdp_hdcp_en)(hi_bool b_on_off);
    hi_s32 (*set_proc_vdp_zme_strategy)(hi_pq_proc_vdpzme_strategy st_proc_vdp_zme_strategy);
    hi_s32 (*get_proc_vdp_zme_strategy)(hi_pq_proc_get_vdpzme_strategy *pst_proc_vdp_zme_strategy);
    hi_s32 (*get_proc_vdp_zme_reso)(hi_pq_proc_vdpzme_reso *pst_proc_vdp_zme_reso);

    /* VPSS ZME */
    hi_s32 (*get_vpsszme_coef)(hi_drv_pq_vpss_layer layer_id,
                               hi_drv_pq_vpsszme_in *zme_in,
                               hi_drv_pq_vpsszme_out *zme_out);
    /* GFX ZME */
    hi_s32 (*get_gfx_zme_strategy)(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_zme_in *gfx_zme_in,
                                   hi_drv_pq_gfx_zme_out *gfx_zme_out);
    hi_s32 (*set_strategy_by_proc)(gfx_zme_strategy_proc strategy);
    hi_s32 (*get_strategy_by_proc)(gfx_zme_strategy_proc *strategy);
    /* GFX HDR */
    hi_s32 (*set_gfx_hdr_cfg)(hi_drv_pq_gfx_layer gfx_layer, drv_pq_internal_gfx_hdr_info *hdr_info);
    /* DEI */
    hi_s32 (*dei_param_refresh)(hi_drv_pq_vpss_stt_info *pst_vpss_stt_info);
    hi_s32 (*get_ma_only_enable)(hi_bool *on_off);
    hi_s32 (*get_mc_only_enable)(hi_bool *on_off);
    hi_s32 (*set_mc_only_enable)(hi_bool on_off);
    hi_s32 (*update_dei_stt_info)(hi_drv_pq_vpss_stt_info *info_in, drv_pq_dei_input_info *dei_api_input);
    hi_s32 (*update_dei_cfg)(hi_u32 handle_no, hi_drv_pq_dei_api_output_reg *dei_api_out);
    hi_s32 (*update_dei_rc_info)(hi_u32 handle_no, hi_u32 width, hi_u32 height);
    hi_s32 (*update_field_flag)(hi_u32 handle_no, hi_u32 frm_num);
    /* FMD */
    hi_s32 (*init_fod_param)(hi_u32 handle);
    hi_s32 (*init_ifmd_param)(hi_u32 width, hi_u32 height);
    hi_s32 (*update_fod_ifmd_stt)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_vpss_cfg_info *vpss_cfg_info);
    hi_s32 (*get_fod_dect_info)(hi_u32 *field_order);
    hi_s32 (*set_force_fod_mode)(hi_u32 field_order);
    hi_s32 (*get_force_fod_mode)(hi_u32 *field_order);

    /* DB */
    hi_s32 (*db_param_refresh)(hi_u32 handle_id, hi_u32 height, hi_u32 width, hi_u32 f_rate);
    hi_s32 (*set_dbm_para_upd)(hi_u32 data);
    hi_s32 (*init_dbm)(hi_void);
    hi_s32 (*set_dbm_info)(hi_drv_pq_vpss_stt_info *pst_info);
    hi_s32 (*update_db_stt_info)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_db_input_info *db_api_input_info);
    hi_s32 (*update_db_cfg)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_db_output_info *db_api_output_info);
    /* DM */
    hi_s32 (*update_dm_stt_info)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input);
    hi_s32 (*update_dm_cfg)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_output_reg *dm_api_out);

    /* DR */
    /* DS */
    hi_s32 (*set_deshoot_thr)(hi_u32 id, hi_u32 width);
    /* TNR */
    hi_s32 (*init_tnr)(hi_void);
    hi_s32 (*deinit_tnr)(hi_void);
    hi_s32 (*update_tnr_stt_info)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input);
    hi_s32 (*update_tnr_cfg)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_api_output_reg *nr_output_reg);
    hi_s32 (*enable_mcnr)(hi_bool b_on_off);
    hi_s32 (*get_mcnr_enable)(hi_bool *pb_on_off);
    /* SNR */
    hi_s32 (*init_snr)(hi_void);
    hi_s32 (*update_snr_stt_info)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input);
    hi_s32 (*update_snr_cfg)(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_api_output_reg *nr_output_reg);

    /* DCI */
    hi_s32 (*get_dci_histgram)(hi_pq_dci_histgram *dci_hist);
    hi_s32 (*set_dci_reset)(hi_bool b_reset);
    hi_s32 (*update_dci_cfg)(hi_void);
    hi_s32 (*updat_dci_coef)(hi_pq_vdp_layer_vid layer_vid);
    hi_s32 (*set_dci_reso)(pq_dci_ref_reso *dci_reso);
    hi_s32 (*get_dci_proc_info)(pq_dci_cfg_proc *dci_proc);
    hi_s32 (*get_dci_mean_value)(hi_u32 *mean_value);
    hi_s32 (*set_dci_scd)(hi_bool scene_change);
    hi_s32 (*get_dci_scd)(hi_bool *scene_change);
    /* ACM */
    hi_s32 (*set_flesh_tone_level)(hi_pq_fleshtone gain_level);
    hi_s32 (*get_flesh_tone_level)(hi_u32 *flesh_tone_level);
    hi_s32 (*set_acm_gain)(hi_void);
    hi_s32 (*set_six_base_color)(hi_pq_six_base_color *six_base_color);
    hi_s32 (*get_six_base_color)(hi_pq_six_base_color *six_base_color);
    hi_s32 (*set_color_enhance_mode)(hi_pq_color_spec_mode color_spec_mode);
    hi_s32 (*get_color_enhance_mode)(hi_u32 *color_enhance_mode);
    hi_s32 (*updat_acm_coef)(pq_layer_vp en_layer);
    hi_void (*set_vdp_hdr_type)(hi_drv_hdr_type source_type, hi_drv_hdr_type output_type);
    /* DC */
    hi_s32 (*init_dc_param)(hi_void);
    hi_s32 (*update_dc_stt_info)(hi_void);
    hi_s32 (*update_dc_cfg)(hi_void);

    /* HDR */
    hi_s32 (*set_hdr_dci_histgram)(hi_u32 dci_out_width, hi_u32 dci_out_height,
                                   hi_pq_dci_histgram *pst_dci_histgram);
    hi_s32 (*set_hdr_csc_video_setting)(hi_pq_image_param *pst_pic_setting);
    hi_s32 (*set_hdr_offset)(hi_pq_hdr_offset *pst_hdr_offset_para);
    hi_s32 (*set_hdr_default_cfg)(hi_bool b_default);
    hi_s32 (*set_vdp_hdr_cfg)(hi_drv_pq_xdr_layer_id layer_id, hi_drv_pq_xdr_frame_info *xdr_frame_info);
    hi_s32 (*set_hdr_tm_curve)(hi_pq_hdr_offset *pst_hdr_offset_para);
    hi_s32 (*get_hdr_cfg_by_proc)(pq_hdr_cfg_proc *hdr_cfg);
    /* ZME send para to sharp */
    hi_s32 (*set_sharpen_reso_info)(hi_pq_sharpen_reso *pst_sharpen_reso);
    /* LCHDR */
    hi_s32 (*set_vpss_hdr_cfg)(hi_drv_pq_vpss_layer layer_id, drv_pq_internal_xdr_frame_info *xdr_frame_info,
                               vpss_reg_type *vpss_reg);
    hi_s32 (*get_vpsshdr_cfg_by_proc)(hi_u32 handle, pq_vpsshdr_cfg_proc *hdr_cfg);
    /* CLE */
    hi_s32 (*set_cle_reso)(pq_cle_id id, const pq_cle_reso *cle_reso);
    hi_s32 (*get_cle_reso)(pq_cle_id id, pq_cle_reso *cle_reso);
    hi_s32 (*update_cle_cfg)(hi_void);
    hi_s32 (*cle_ddr_regread)(uintptr_t reg_addr, hi_u32 *value);
    hi_s32 (*cle_ddr_regwrite)(uintptr_t reg_addr, hi_u32 value);
    hi_s32 (*get_cle_cfg_by_proc)(pq_cle_cfg_proc *cle_proc);
    /* SR */
    hi_s32 (*set_sr_scale_mode)(pq_sr_id id, hi_u32 scale_mode);
    /* SHARPEN */
    hi_s32 (*set_sharp_reso)(const pq_sharpen_reso *sharp_reso);
    hi_s32 (*update_sharp_cfg)(hi_void);
    hi_s32 (*get_sharp_proc_info)(pq_sharp_cfg_proc *sharp_proc_info);
    hi_s32 (*sharp_ddr_regread)(uintptr_t reg_addr, hi_u32 *value);
    hi_s32 (*sharp_ddr_regwrite)(uintptr_t reg_addr, hi_u32 value);
} pq_alg_funcs;

typedef struct {
    hi_u32 alg_id;
    hi_u32 type_id;
    hi_u32 adape_type;
    hi_u8 alg_name[ALG_NAME_MAX_LEN];
    const pq_alg_funcs *fun;
} pq_alg_reg;

hi_s32 pq_comm_check_chip_name(hi_char* chip_name, hi_u32 len);

hi_s32 pq_comm_alg_register(hi_pq_module_type module,
                            pq_reg_type reg_type,
                            pq_bin_adapt_type adapt_type,
                            const hi_u8 *alg_name,
                            const pq_alg_funcs *funcs);

hi_s32 pq_comm_alg_unregister(hi_pq_module_type module);

hi_u32 pq_comm_get_alg_type_id(hi_pq_module_type module);

hi_u32 pq_comm_get_alg_adape_type(hi_pq_module_type module);

pq_alg_reg *pq_comm_get_alg(hi_pq_module_type module);

hi_u8 *pq_comm_get_alg_name(hi_pq_module_type module);

hi_u8 pq_comm_get_moudle_ctrl(hi_pq_module_type module, pq_source_mode src_mode,
                              pq_output_mode out_mode);

hi_void pq_comm_set_moudle_ctrl(hi_pq_module_type module, pq_source_mode src_mode,
                                pq_output_mode out_mode, hi_u8 on_off);

pq_source_mode pq_comm_check_source_mode(hi_u32 width);

pq_output_mode pq_comm_check_output_mode(hi_u32 width, hi_u32 refresh_rate);

pq_source_mode pq_comm_get_source_mode(hi_void);

hi_void pq_comm_set_source_mode(pq_source_mode mode);

pq_output_mode pq_comm_get_output_mode(hi_void);

hi_void pq_comm_set_output_mode(pq_output_mode mode);

hi_s32 pq_comm_mem_alloc(drv_pq_mem_attr *in_mem_attr, drv_pq_mem_info *out_mem_info);
hi_s32 pq_comm_mem_free(drv_pq_mem_info *mem_info);
hi_s32 pq_comm_mem_addr_map(hi_u64 fd, drv_pq_mem_fd_info *out_fd_info);
hi_s32 pq_comm_mem_unmap(drv_pq_mem_fd_info *fd_info);
hi_s32 pq_comm_mem_flush(drv_pq_mem_info *mem_info);

/*
 * brief 初始化VDP STT 配置，申请VDP STT内存;
 * attention
 * param[in] : NA
 * retval :HI_SUCCESS
 */
hi_s32 pq_comm_vdp_stt_init(hi_void);

/*
 * brief 去初始化VDP STT 配置，释放VDP STT内存;
 * attention
 * param[in] : NA
 * retval :HI_SUCCESS
 */
hi_s32 pq_comm_vdp_stt_deinit(hi_void);

/*
 * brief 更新VDP STT信息
 * attention
 * param[in] : NA
 * retval :HI_SUCCESS
 */
hi_s32 pq_comm_vdp_stt_update(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

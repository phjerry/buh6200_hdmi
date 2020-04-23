/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_ADP_HAL__
#define __SOURCE_MSP_DRV_TDE_ADP_HAL__

#include "drv_tde_struct.h"
#include "tde_adp.h"
#include "tde_define.h"
#include "tde_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_GFX_TDE_VERSION_3_0
#define TDE_SRC_CSC_IDC0_SIZE (7 * 4);
#define TDE_SRC_CSC_IDC1_SIZE (2 * 4);
#define TDE_DST_CSC_IDC0_SIZE (7 * 4);
#define TDE_DST_CSC_IDC1_SIZE (2 * 4);
#else
#define TDE_SRC_CSC_IDC0_SIZE (7 * 4);
#define TDE_DST_CSC_IDC0_SIZE (7 * 4);
#endif

typedef enum {
    TDE_DRV_COLOR_FMT_ARGB8888 = 0x0,
    TDE_DRV_COLOR_FMT_KRGB8888 = 0x1,
    TDE_DRV_COLOR_FMT_ARGB4444 = 0x2,
    TDE_DRV_COLOR_FMT_ARGB1555 = 0x3,
    TDE_DRV_COLOR_FMT_ARGB8565 = 0x4,
    TDE_DRV_COLOR_FMT_RGB888 = 0x5,
    TDE_DRV_COLOR_FMT_RGB444 = 0x6,
    TDE_DRV_COLOR_FMT_RGB555 = 0x7,
    TDE_DRV_COLOR_FMT_RGB565 = 0x8,
    TDE_DRV_COLOR_FMT_A1 = 0x9,
    TDE_DRV_COLOR_FMT_A8 = 0xa,
    TDE_DRV_COLOR_FMT_YCbCr888 = 0xb,
    TDE_DRV_COLOR_FMT_AYCbCr8888 = 0xc,
    TDE_DRV_COLOR_FMT_A1B = 0xd,
    TDE_DRV_COLOR_FMT_CLUT1 = 0x10,
    TDE_DRV_COLOR_FMT_CLUT2 = 0x11,
    TDE_DRV_COLOR_FMT_CLUT4 = 0x12,
    TDE_DRV_COLOR_FMT_CLUT8 = 0x13,
    TDE_DRV_COLOR_FMT_ACLUT44 = 0x14,
    TDE_DRV_COLOR_FMT_ACLUT88 = 0x15,
    TDE_DRV_COLOR_FMT_CLUT1B = 0x16,
    TDE_DRV_COLOR_FMT_CLUT2B = 0x17,
    TDE_DRV_COLOR_FMT_CLUT4B = 0x18,
    TDE_DRV_COLOR_FMT_ABGR2101010 = 0x19,
    TDE_DRV_COLOR_FMT_FP16 = 0x1A,
    TDE_DRV_COLOR_FMT_ABGR10101010 = 0x1D,
    TDE_DRV_COLOR_FMT_PKGYUYV = 0x20,
    TDE_DRV_COLOR_FMT_YCbCr422 = 0x21,
    TDE_DRV_COLOR_FMT_PKGYYVU = 0x27,
    TDE_DRV_COLOR_FMT_byte = 300,
    TDE_DRV_COLOR_FMT_halfword = 34,
    TDE_DRV_COLOR_FMT_YCbCr400MBP = 0x30,
    TDE_DRV_COLOR_FMT_YCbCr422MBH = 0x31,
    TDE_DRV_COLOR_FMT_YCbCr422MBV = 0x32,
    TDE_DRV_COLOR_FMT_YCbCr420MB = 0x33,
    TDE_DRV_COLOR_FMT_YCbCr444MB = 0x34,
    TDE_DRV_COLOR_FMT_PLANNER420 = 0x35,
    TDE_DRV_COLOR_FMT_RABG8888,
    TDE_DRV_COLOR_FMT_MAX
} tde_color_fmt;

/* branch order */
/* support 24 orders in ARGB, TDE driver can only expose 4 usual orders; if needed, can be added */
typedef enum {
    TDE_DRV_ORDER_ARGB = 0x0,
    TDE_DRV_ORDER_ABGR = 0x5,
    TDE_DRV_ORDER_RABG = 0x7,
    TDE_DRV_ORDER_RGBA = 0x9,
    TDE_DRV_ORDER_BGRA = 0x14,
    TDE_DRV_ORDER_MAX
} tde_argb_order_mode;

/* TDE basic operate mode */
typedef enum {
    TDE_QUIKE_FILL,                  /* Quick fill */
    TDE_QUIKE_COPY,                  /* Quick copy */
    TDE_NORM_FILL_1OPT,              /* Normal fill in single source */
    TDE_NORM_BLIT_1OPT,              /* Normal bilit in single source */
    TDE_NORM_FILL_2OPT,              /* Fill and Rop */
    TDE_NORM_BLIT_2OPT,              /* Normal bilit in double source */
    TDE_MB_C_OPT,                    /* MB chroma zoom */
    TDE_MB_Y_OPT,                    /* MB brightness zoom */
    TDE_MB_2OPT,                     /* MB combinate operation */
    TDE_SINGLE_SRC_PATTERN_FILL_OPT, /* Fill operate in single source mode */
    TDE_DOUBLE_SRC_PATTERN_FILL_OPT  /* Fill operate in double source mode */
} tde_base_opt_mode;

/* Type definition in interrupted state */
typedef enum {
    TDE_DRV_LINK_COMPLD_STATS = 0x1,
    TDE_DRV_NODE_COMPLD_STATS = 0x2,
    TDE_DRV_LINE_SUSP_STATS = 0x4,
    TDE_DRV_RDY_START_STATS = 0x8,
    TDE_DRV_SQ_UPDATE_STATS = 0x10,
    TDE_DRV_INT_ALL_STATS = 0x800F001F
} tde_int_status_mode;

/* ColorKey mode is needed by hardware */
typedef enum {
    TDE_DRV_COLORKEY_BACKGROUND = 0,             /* color key in bkground bitmap */
    TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT = 2, /* color key in foreground bitmap,before CLUT */
    TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT = 3   /* color key in bkground bitmap, after CLUT */
} tde_color_key_mode;

/* color key setting arguments */
typedef struct {
    tde_color_key_mode color_key_mode; /* color key mode */
    hi_tde_color_key color_key_value;  /* color key value */
} tde_color_key_cmd;

/* Deficker filting mode */
typedef enum {
    TDE_DRV_FIXED_COEF0 = 0, /* Deficker by fixed coefficient: 0 */
    TDE_DRV_AUTO_FILTER,     /* Deficker by auto filter */
    TDE_DRV_TEST_FILTER      /* Deficker by test filter */
} tde_flicker_mode;

/* Block type, equipped register note in numerical value reference */
typedef enum {
    TDE_NO_BLOCK_SLICE_TYPE = 0,      /* No block */
    TDE_FIRST_BLOCK_SLICE_TYPE = 0x3, /* First block */
    TDE_LAST_BLOCK_SLICE_TYPE = 0x5,  /* Last block */
    TDE_MID_BLOCK_SLICE_TYPE = 0x1    /* Middle block */
} tde_slice_type;

/* vertical/horizontal filt mode: available for zoom */
typedef enum {
    TDE_DRV_FILTER_NONE = 0, /* none filt */
    TDE_DRV_FILTER_ALL       /* filt on Alpha and color value */
} tde_filter_type;

typedef enum {
    TDE_ZME_FMT_YUV422 = 0, /* none filt */
    TDE_ZME_FMT_YUV420,     /* filt on color parameter */
    TDE_ZME_FMT_YUV444,     /* filt on Alpha value */
    TDE_ZME_FMT_ARGB        /* filt on Alpha and color value */
} tde_zme_fmt_mode;

/* Deflicker operate setting */
typedef struct {
    tde_flicker_mode flicker_mode;
    tde_filter_type filter_type;
    hi_u8 coef0_last_line;
    hi_u8 coef0_cur_line;
    hi_u8 coef0_next_line;
    hi_u8 coef1_last_line;
    hi_u8 coef1_cur_line;
    hi_u8 coef1_next_line;
    hi_u8 coef2_last_line;
    hi_u8 coef2_cur_line;
    hi_u8 coef2_next_line;
    hi_u8 coef3_last_line;
    hi_u8 coef3_cur_line;
    hi_u8 coef3_next_line;
    hi_u8 threshold0;
    hi_u8 threshold1;
    hi_u8 threshold2;
    hi_tde_deflicker_mode deflicker_mode;
} tde_flicker_cmd;

typedef struct {
    hi_u32 hratio : 24;    /* [23..0] */
    hi_u32 hfir_order : 1; /* [24] */
    hi_u32 hchfir_en : 1;  /* [25] */
    hi_u32 hlfir_en : 1;   /* [26] */
    hi_u32 hafir_en : 1;   /* [27] */
    hi_u32 hchmid_en : 1;  /* [28] */
    hi_u32 hlmid_en : 1;   /* [29] */
    hi_u32 hchmsc_en : 1;  /* [30] */
    hi_u32 hlmsc_en : 1;   /* [31] */
} tde_hsp_info;

/* Zoom operate settings */
typedef struct {
    hi_u32 offset_x;
    hi_u32 offset_y;
    hi_u32 step_hor;
    hi_u32 step_ver;
    hi_bool coef_sym;
    hi_bool ver_ring;
    hi_bool hor_ring;
    tde_filter_type filter_type;
    tde_filter_type filter_hor;
    hi_bool first_line_out;
    hi_bool last_line_out;
} tde_resize_cmd;

/* Clip Setting */
typedef struct {
    hi_u16 clip_start_x;
    hi_u16 clip_start_y;
    hi_u16 clip_end_x;
    hi_u16 clip_end_y;
    hi_bool is_inside_clip;
} tde_clip_cmd;

/* clut mode */
typedef enum {
    TDE_COLOR_EXP_CLUT_MODE = 0, /* color expand */
    TDE_COLOR_CORRCT_CLUT_MODE   /* color correct */
} tde_clut_mode;

/* clut setting */
typedef struct {
    tde_clut_mode clut_mode;
    hi_u32 clut_phy_addr;
} tde_clut_cmd;

/* MB Setting */
typedef enum {
    TDE_MB_Y_FILTER = 0,     /* brightness filt */
    TDE_MB_CbCr_FILTER = 2,  /* chroma filt */
    TDE_MB_UPSAMP_CONCA = 4, /* first upsample then contact in chroma and brightness */
    TDE_MB_CONCA_FILTER = 6, /* first contact in chroma and brightness and then filt */
} tde_mb_opt_mode;

/* MB Command Setting */
typedef struct {
    tde_mb_opt_mode mb_mode; /* MB Operate Mode */
} tde_mb_cmd;

/* plane mask command setting */
typedef struct {
    hi_u32 mask;
} tde_mask_cmd;

/* Color zone convert setting */
typedef struct {
    hi_u8 in_metrix_vid;  /* Import Metrix used by color converted:graphic:0/video:1 */
    hi_u8 in_metrix_709;  /* Import standard in color convertion:IT-U601:0/ITU-709:1 */
    hi_u8 out_metrix_vid; /* Export Metrix used by color converted:graphic:0/video:1 */
    hi_u8 out_metrix_709; /* Import standard in color conversion:IT-U601:0/ITU-709:1 */
    hi_u8 in_conv;        /* Enable or unable conversion on importing color zone */
    hi_u8 out_conv;       /* Enable or unable conversion on exporting color zone */
    hi_u8 in_src1_conv;
    hi_u8 in_rgb2yuv; /* import color conversion direction */
} tde_conv_mode_cmd;

/* vertical scanning direction */
typedef enum {
    TDE_SCAN_UP_DOWN = 0, /* form up to down */
    TDE_SCAN_DOWN_UP = 1  /* form down to up */
} tde_vscan_mode;

/* horizontal scanning direction */
typedef enum {
    TDE_SCAN_LEFT_RIGHT = 0, /* form left to right */
    TDE_SCAN_RIGHT_LEFT = 1  /* form right to left */
} tde_hscan_mode;

/* Definition on scanning direction */
typedef struct {
    tde_vscan_mode ver_scan; /* vertical scanning direction */
    tde_hscan_mode hor_scan; /* horizontal scanning direction */
} tde_scandirection_mode;

typedef struct {
    hi_u32 phy_addr;               /* Bitmap head addr */
    tde_color_fmt color_fmt;       /* color format */
    tde_argb_order_mode rgb_order; /* ARGB component order */
    hi_u32 pos_x;                  /* Position X at first */
    hi_u32 pos_y;                  /* Position Y at first */
    hi_u32 height;                 /* Bitmap Height */
    hi_u32 width;                  /* Bitmap Width */
    hi_u32 pitch;                  /* Bitmap Pitch */
    hi_u32 cbcr_phy_addr;          /* CbCr component addr */
    hi_u32 cbcr_pitch;             /* CbCr pitch */
    hi_bool alpha_max_is_255;
    tde_vscan_mode ver_scan; /* Vertical scanning direction */
    tde_hscan_mode hor_scan; /* Horizontal scanning direction */
    hi_bool is_cma;
} tde_surface_msg;

/* MB bitmap info */
typedef struct {
    tde_color_fmt mb_color_fmt;
    hi_u32 y_addr;
    hi_u32 y_width;
    hi_u32 y_height;
    hi_u32 y_stride;
    hi_u32 cbcr_phy_addr;
    hi_u32 cbcr_stride;
} tde_mb_surface_msg;

/* ALU mode */
typedef enum {
    TDE_SRC1_BYPASS = 0,
    TDE_ALU_NONE, /* register has no setting, used in flag */
    TDE_ALU_ROP,
    TDE_SRC2_BYPASS,
    TDE_ALU_MASK_ROP1,
    TDE_ALU_MASK_ROP2,
    TDE_ALU_MASK_BLEND,
    TDE_ALU_BLEND,
    TDE_ALU_BLEND_SRC2,
    TDE_ALU_CONCA,
    TDE_CLIP,
    TDE_BUTT
} tde_alu_mode;

/* Zoom mode in subnode */
typedef enum {
    TDE_CHILD_SCALE_NORM = 0,
    TDE_CHILD_SCALE_MBY,
    TDE_CHILD_SCALE_MBC,
    TDE_CHILD_SCALE_MB_CONCA_H,
    TDE_CHILD_SCALE_MB_CONCA_M,
    TDE_CHILD_SCALE_MB_CONCA_L,
    TDE_CHILD_SCALE_MB_CONCA_CUS,
} tde_child_scale_mode;

/* Info needed in MB format when Y/CbCr change */
typedef struct {
    hi_u32 start_in_x; /* Start X,Y imported after MB adjust */
    hi_u32 start_in_y;
    hi_u32 start_out_x; /* Start X,Y exported after MB adjust */
    hi_u32 start_out_y;
    tde_color_fmt color_fmt; /* color format, MB use it to renew position of Y and CbCr */
    tde_child_scale_mode scale_mode;
} tde_mb_start_adj_info;

/* Adjusting info when double source dispart */
typedef struct {
    hi_bool double_source;
    hi_s32 diff_x;
    hi_s32 diff_y;
} tde_double_src_adj_info;
/*
 * Configure info when set child node
 * up_data_flag :
 * _________________________________________
 * |    |    |    |    |    |    |    |    |
 * | ...| 0  | 0  | 1  | 1  | 1  | 1  |  1 |
 * |____|____|____|____|____|____|____|____|
 *                   |    |    |    |    |
 *                  \/   \/   \/   \/   \/
 *                wo xo HOfst wi xi
 *                ho yo VOfst hi yi
 */
typedef struct {
    hi_u32 u32SliceWidth;
    hi_u32 u32SliceWi;
    hi_s32 s32SliceCOfst;
    hi_s32 s32SliceLOfst;
    hi_u32 u32SliceHoffset;
    hi_u32 u32SliceWo;
    hi_u32 u32SliceWHpzme;
    hi_u32 xi;
    hi_u32 yi;
    hi_u32 wi;
    hi_u32 hi;
    hi_u32 hor_offset;
    hi_u32 ver_offset;
    hi_u32 xo;
    hi_u32 yo;
    hi_u32 wo;
    hi_u32 ho;
    hi_u32 u32SliceDstWidth;
    hi_u32 u32SliceDstHeight;
    hi_u32 u32SliceDstHoffset;
    hi_u64 up_data_flag;
    tde_mb_start_adj_info adj_info;
    tde_double_src_adj_info ds_adj_info;
    tde_slice_type slice_type;
    hi_u32 dst_crop_en;
    hi_u32 dst_crop_start_x;
    hi_u32 dst_crop_end_x;
} tde_child_info;

typedef struct {
    tde_color_fmt color_fmt;
    hi_u32 fill_data;
} tde_color_fill;

typedef enum {
    TDE_DRV_INT_lIST = 0x8,
    TDE_DRV_INT_NODE = 0x1,
    TDE_DRV_INT_ERROR = 0x4,
    TDE_DRV_INT_TIMEOUT = 0x2,
} tde_int_type;

typedef enum {
    TDE_DRV_SRC_NONE = 0,
    TDE_DRV_SRC_S1 = 0x1,
    TDE_DRV_SRC_S2 = 0x2,
    TDE_DRV_SRC_T = 0x4,
} tde_src_mode;

#define TDE_CTRL 0x0500
#define TDE_INT 0x0514
#define TDE_INTCLR 0x0510
#define TDE_PNEXT_LOW 0x1F8 /* Á´±íÊ×µØÖ· */
#define TDE_PNEXT_HI 0x1Fc
#define TDE_STA 0x4000
#define TDE_AQ_ADDR 0x4004
#define TDE_MISCELLANEOUS 0x0530
#define TDE_MST_OUTSTANDING 0x0600
#define TDE_SRC1_CTRL 0x0200

#define TDE_CBM_CFG_REG_OFFSET 0x800
#define TDE_ZME_CFG_REG_OFFSET 0x1000
#define TDE_AFBC_CFG_REG_OFFSET 0x2000
#define TDE_HDR_CFG_REG_OFFSET 0x3600
#define TDE_GFX_CFG_REG_OFFSET 0x7000

#define TDE_AQ_COMP_NODE_MASK_EN 4 /* 0100£ºEnable to interrupt when complete current node in AQ */
#define TDE_AQ_COMP_LIST_MASK_EN 8 /* 1000£ºEnable to interrupt  in complete AQ */

#define TDE_NODE_HEAD_BYTE 16

/* Next node addr¡¢update info¡¢occupied bytes */
#define TDE_NODE_TAIL_BYTE 12

hi_s32 tde_hal_init(hi_u32 base_addr);
hi_void tde_hal_release(hi_void);
hi_bool tde_hal_ctl_is_idle(hi_void);
hi_bool tde_hal_ctl_is_idle_safely(hi_void);
hi_u32 tde_hal_ctl_int_status(hi_void);
hi_void tde_hal_ctl_reset(hi_void);
hi_void tde_hal_set_cfg_reg_offset(tde_hardware_node *hardware_node);
hi_s32 tde_hal_init_node(tde_hardware_node **hardware_node);
hi_void tde_hal_free_node_buf(tde_hardware_node *hardware_node);
hi_void tde_hal_calc_src1_filter_opt(tde_hardware_node *hardware_node, hi_tde_color_fmt in_color_fmt,
                                     hi_tde_color_fmt out_color_fmt, hi_tde_rect *in_rect, hi_tde_rect *out_rect,
                                     hi_tde_filter_mode filter_mode);
hi_void tde_hal_calc_src2_filter_opt(tde_hardware_node *hardware_node, hi_tde_color_fmt in_color_fmt,
                                     hi_tde_color_fmt out_color_fmt, hi_tde_rect *in_rect, hi_tde_rect *out_rect,
                                     hi_bool defilicker, hi_tde_filter_mode filter_mode);
hi_void tde_hal_node_enable_complete_int(hi_void *vir_buf);
hi_void tde_hal_set_src1(tde_hardware_node *hardware_node, tde_surface_msg *surface);
hi_void tde_hal_set_src2(tde_hardware_node *hardware_node, tde_surface_msg *surface);
hi_void tde_hal_node_set_tqt(tde_hardware_node *hardware_node, tde_surface_msg *surface,
                             hi_tde_out_alpha_from alpha_from);
hi_s32 tde_hal_node_set_base_operate(tde_hardware_node *hardware_node, tde_base_opt_mode opt_mode,
                                     tde_alu_mode alu_mode, tde_color_fill *color_fill);
hi_void tde_hal_node_set_global_alpha(tde_hardware_node *hardware_node, hi_u8 alpha, hi_bool enable);
hi_void tde_hal_node_set_src1_alpha(tde_hardware_node *hardware_node);
hi_void tde_hal_node_set_src2_alpha(tde_hardware_node *hardware_node);
hi_void tde_hal_node_set_exp_alpha(tde_hardware_node *hardware_node, tde_src_mode src_mode, hi_u8 alpha0, hi_u8 alpha1);
hi_s32 tde_hal_node_set_rop(tde_hardware_node *hardware_node, hi_tde_rop_mode rgb_rop, hi_tde_rop_mode alpha_rop);
hi_s32 tde_hal_node_set_blend(tde_hardware_node *hardware_node, hi_tde_blend_opt *blend_opt);
hi_s32 tde_hal_node_set_colorize(tde_hardware_node *hardware_node, hi_u32 color_resize);
hi_void tde_hal_node_enable_alpha_rop(tde_hardware_node *hardware_node);
hi_s32 tde_hal_node_set_clut_opt(tde_hardware_node *hardware_node, tde_clut_cmd *clut_cmd, hi_bool reload);
hi_s32 tde_hal_node_set_colorkey(tde_hardware_node *hardware_node, tde_color_fmt_category fmt_cat,
                                 tde_color_key_cmd *color_key);
hi_s32 tde_hal_node_set_clipping(tde_hardware_node *hardware_node, tde_clip_cmd *clip);
hi_s32 tde_hal_node_set_color_convert(tde_hardware_node *hardware_node, tde_conv_mode_cmd *conv);
hi_s32 tde_set_node_csc(tde_hardware_node *hw_node, hi_tde_csc_opt *csc_opt);
hi_s32 tde_hal_set_deflicer_level(hi_tde_deflicker_level deflicker_level);
hi_s32 tde_hal_get_deflicer_level(hi_tde_deflicker_level *deflicker_level);
hi_s32 tde_hal_set_alpha_threshold(hi_u8 threshold_value);
hi_s32 tde_hal_get_alpha_threshold(hi_u8 *threshold_value);
hi_s32 tde_hal_set_alpha_threshold_state(hi_bool alpha_threshold_en);
hi_s32 tde_hal_get_alpha_threshold_state(hi_bool *alpha_threshold_en);
hi_s32 tde_hal_node_execute(hi_u64 node_phy_addr, hi_u64 up_data_flag, hi_bool aq_use_buf);
hi_u32 tde_hal_cur_node(hi_void);
hi_u32 tde_hal_finished_job_id(hi_void);
hi_void tde_hal_resume_init(hi_void);
hi_void tde_hal_suspend(hi_void);
hi_void tde_hal_set_clock(hi_bool enable);

#ifdef CONFIG_GFX_MMU_SUPPORT
hi_void tde_hal_free_tmp_buf(tde_hardware_node *hardware_node);
#endif
struct seq_file *tde_hal_node_print_info(hi_void *p, tde_hardware_node *cur_node);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_ADP_HAL__ */

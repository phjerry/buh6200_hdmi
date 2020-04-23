/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: Hisilicon
* Create: 2019-11-12
*/
#ifndef __HI_DRV_COLOR_H__
#define __HI_DRV_COLOR_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_DRV_HDR_DISPLAY_PRIMARIES_NUM 3
#define SL_HDR_TUNING_NUMBER 10
#define SL_HDR_GAIN_NUMBER 6
#define SL_HDR_METADATA_TABLE_NUMBER 65
#define SL_HDR_MATRIX_COEF_NUMBER 4
#define SL_HDR_CH2LUMA_NUMBER 2
#define SL_HDR_K_COEF_NUMBER 3
#define HDR_PLUS_MAX_SCL_NUMBER 3

typedef struct {
    hi_s32 tm_input_signal_black_level_offset;
    hi_s32 tm_input_signal_white_level_offset;
    hi_s32 shadow_gain;
    hi_s32 highlight_gain;
    hi_s32 mid_tone_width_adj_factor;
    hi_s32 tm_output_fine_tuning_num_val;
    hi_s32 tm_output_fine_tuning_x[SL_HDR_TUNING_NUMBER];
    hi_s32 tm_output_fine_tuning_y[SL_HDR_TUNING_NUMBER];
    hi_s32 saturation_gain_num_val;
    hi_s32 saturation_gain_x[SL_HDR_GAIN_NUMBER];
    hi_s32 saturation_gain_y[SL_HDR_GAIN_NUMBER];
} hi_drv_sl_hdr_metadata_var;

typedef struct {
    hi_s32 luminance_mapping_num_val;
    hi_s32 luminance_mapping_x[SL_HDR_METADATA_TABLE_NUMBER];
    hi_s32 luminance_mapping_y[SL_HDR_METADATA_TABLE_NUMBER];
    hi_s32 colour_correction_num_val;
    hi_s32 colour_correction_x[SL_HDR_METADATA_TABLE_NUMBER];
    hi_s32 colour_correction_y[SL_HDR_METADATA_TABLE_NUMBER];
} hi_drv_sl_hdr_metadata_tab;

typedef struct {
    hi_s32  part_id;
    hi_s32  major_spec_version_id;
    hi_s32  minor_spec_version_id;
    hi_s32  payload_mode;
    hi_s32  hdr_pic_colour_space;
    hi_s32  hdr_display_colour_space;
    hi_s32  hdr_display_max_luminance;
    hi_s32  hdr_display_min_luminance;
    hi_s32  sdr_pic_colour_space;
    hi_s32  sdr_display_colour_space;
    hi_s32  sdr_display_max_luminance;
    hi_s32  sdr_display_min_luminance;
    hi_s32  matrix_coefficient[SL_HDR_MATRIX_COEF_NUMBER];
    hi_s32  chroma_to_luma_injection[SL_HDR_CH2LUMA_NUMBER];
    hi_s32  k_coefficient[SL_HDR_K_COEF_NUMBER];
    union {
        hi_drv_sl_hdr_metadata_var variables;
        hi_drv_sl_hdr_metadata_tab tables;
    } u;
} hi_drv_sl_hdr_metadata;

#define MAX_NUM_WINDOWS 3
#define MAX_NUM_BEZIER_CURVE_ANCHORS 15
#define MAX_NUM_DIST_MAXRGB_PERCENTILES 15

#define MAX_NUM_ROWS_T_DISPLAY_APL 31
#define MAX_NUM_COLS_T_DISPLAY_APL 31
#define MAX_NUM_ROWS_M_DISPLAY_APL 31
#define MAX_NUM_COLS_M_DISPLAY_APL 31

typedef struct {
    hi_u8   itu_country_code;
    hi_u16  itu_terminal_provider_code;
    hi_u16  itu_terminal_provider_oriented_code;
    hi_u8   application_identifier;
    hi_u8   application_version;
    hi_u8   num_windows;

    hi_u16  window_upper_left_corner_x[MAX_NUM_WINDOWS];
    hi_u16  window_upper_left_corner_y[MAX_NUM_WINDOWS];
    hi_u16  window_lower_right_corner_x[MAX_NUM_WINDOWS];
    hi_u16  window_lower_right_corner_y[MAX_NUM_WINDOWS];
    hi_u16  center_of_ellipse_x[MAX_NUM_WINDOWS];
    hi_u16  center_of_ellipse_y[MAX_NUM_WINDOWS];
    hi_u8   rotation_angle[MAX_NUM_WINDOWS];
    hi_u16  semimajor_axis_internal_ellipse[MAX_NUM_WINDOWS];
    hi_u16  semimajor_axis_external_ellipse[MAX_NUM_WINDOWS];
    hi_u16  semiminor_axis_external_ellipse[MAX_NUM_WINDOWS];
    hi_u8   overlap_process_option[MAX_NUM_WINDOWS];

    hi_u32  target_display_maximum_luminance;

    hi_bool target_display_apl_flag;
    hi_u8   num_rows_target_display_apl;
    hi_u8   num_cols_target_display_apl;
    hi_u8   target_display_apl[MAX_NUM_ROWS_T_DISPLAY_APL][MAX_NUM_COLS_T_DISPLAY_APL];

    hi_u32  max_scl[MAX_NUM_WINDOWS][HDR_PLUS_MAX_SCL_NUMBER];
    hi_u32  average_maxrgb[MAX_NUM_WINDOWS];
    hi_u8   num_dist_maxrgb_percentiles[MAX_NUM_WINDOWS];
    hi_u8   dist_maxrgb_percentages[MAX_NUM_WINDOWS][MAX_NUM_DIST_MAXRGB_PERCENTILES];
    hi_u32  dist_maxrgb_percentiles[MAX_NUM_WINDOWS][MAX_NUM_DIST_MAXRGB_PERCENTILES];
    hi_u16  fraction_bright_pixels[MAX_NUM_WINDOWS];

    hi_bool master_display_apl_flag;
    hi_u8   num_rows_master_display_apl;
    hi_u8   num_cols_master_display_apl;
    hi_u8   master_display_apl[MAX_NUM_ROWS_M_DISPLAY_APL][MAX_NUM_COLS_M_DISPLAY_APL];

    hi_bool tone_mapping_flag[MAX_NUM_WINDOWS];
    hi_u16  knee_point_x[MAX_NUM_WINDOWS];
    hi_u16  knee_point_y[MAX_NUM_WINDOWS];
    hi_u8   num_bezier_curve_anchors[MAX_NUM_WINDOWS];
    hi_u16  bezier_curve_anchors[MAX_NUM_WINDOWS][MAX_NUM_BEZIER_CURVE_ANCHORS];

    hi_bool color_saturation_mapping_flag[MAX_NUM_WINDOWS];
    hi_u8   color_saturation_weight[MAX_NUM_WINDOWS];
    hi_bool graphics_overlay_flag;
    hi_bool no_delay_flag;
} hi_drv_video_hdr10plus_detailed_metadata;

typedef enum {
    HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR = 0, /* gammma sdr: ITU1886 */
    HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_HDR,     /* gammma hdr */
    HI_DRV_COLOR_TRANSFER_TYPE_PQ,            /* PQ SMPTE:2084 */
    HI_DRV_COLOR_TRANSFER_TYPE_HLG,           /* ARIB:STD B67 */
    HI_DRV_COLOR_TRANSFER_TYPE_XVYCC,         /* IEC 61966-2-4 */
    HI_DRV_COLOR_TRANSFER_TYPE_MAX
} hi_drv_color_transfer_curve;

typedef enum {
    HI_DRV_COLOR_PRIMARY_UNSPECIFY = 0,
    HI_DRV_COLOR_PRIMARY_BT601_525,
    HI_DRV_COLOR_PRIMARY_BT601_625,
    HI_DRV_COLOR_PRIMARY_BT709,
    HI_DRV_COLOR_PRIMARY_BT2020,
    HI_DRV_COLOR_PRIMARY_CIE1931_XYZ,
    HI_DRV_COLOR_PRIMARY_BT470_SYSM,
    HI_DRV_COLOR_PRIMARY_SMPTE_240M,
    HI_DRV_COLOR_PRIMARY_GENERIC_FILM,
    HI_DRV_COLOR_PRIMARY_SMPTE_RP431,
    HI_DRV_COLOR_PRIMARY_SMPTE_EG432_1,
    HI_DRV_COLOR_PRIMARY_EBU_TECH_3213E,

    HI_DRV_COLOR_PRIMARY_COLOR_MAX
} hi_drv_color_primary;

typedef enum {
    HI_DRV_COLOR_MATRIX_COEFFS_IDENTITY = 0,
    HI_DRV_COLOR_MATRIX_COEFFS_UNSPECIFY,
    /* BT.1358-1 525 ,BT.1700-0 NTSC ,SMPTE 170M */
    HI_DRV_COLOR_MATRIX_COEFFS_BT601_525,
    /* BT.470,BT.1358-1 625,BT.1700-0 625 PAL and 625 SECAM, IEC 61966-2-4 xvYCC601 */
    HI_DRV_COLOR_MATRIX_COEFFS_BT601_625,
    /* BT.1361-0,IEC 61966-2-1 sYCC,IEC 61966-2-4 xvYCC709,SMPTE RP 177(1993) Annex B */
    HI_DRV_COLOR_MATRIX_COEFFS_BT709,
    HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT,
    HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT,
    HI_DRV_COLOR_MATRIX_COEFFS_BT2100_ICTCP,
    HI_DRV_COLOR_MATRIX_COEFFS_USFCC,
    HI_DRV_COLOR_MATRIX_COEFFS_SMPTE_240M,
    HI_DRV_COLOR_MATRIX_COEFFS_YCGCO,
    HI_DRV_COLOR_MATRIX_COEFFS_ST2085,
    HI_DRV_COLOR_MATRIX_COEFFS_CHROMAT_NON_CONSTANT,
    HI_DRV_COLOR_MATRIX_COEFFS_CHROMAT_CONSTANT,

    HI_DRV_COLOR_MATRIX_COEFFS_MAX
} hi_drv_color_matrix_coeffs;

typedef enum {
    HI_DRV_COLOR_CS_YUV = 0,
    HI_DRV_COLOR_CS_RGB,
    HI_DRV_COLOR_CS_MAX
} hi_drv_color_space;

typedef enum {
    HI_DRV_COLOR_LIMITED_RANGE = 0,
    HI_DRV_COLOR_FULL_RANGE,
    HI_DRV_COLOR_RANGE_MAX
} hi_drv_color_quantify_range;

/* after vdec's  unified process. */
typedef struct {
    hi_drv_color_primary    color_primary;
    hi_drv_color_space          color_space;
    hi_drv_color_quantify_range quantify_range;
    hi_drv_color_transfer_curve   transfer_type;
    hi_drv_color_matrix_coeffs    matrix_coef;
} hi_drv_color_descript;

/* before vdec's  unified process,it's raw data. used for cts etc. */
typedef struct {
    hi_u8 full_range_flag;
    hi_u8 colour_primaries;
    hi_u8 transfer_characteristics;
    hi_u8 matrix_coeffs;
} hi_drv_raw_clolr_descript;

typedef enum {
    HI_DRV_HDR_TYPE_SDR = 0,
    HI_DRV_HDR_TYPE_HDR10,
    HI_DRV_HDR_TYPE_HLG,
    HI_DRV_HDR_TYPE_CUVA,

    HI_DRV_HDR_TYPE_JTP_SL_HDR,
    HI_DRV_HDR_TYPE_HDR10PLUS,
    HI_DRV_HDR_TYPE_DOLBYVISION,

    HI_DRV_HDR_TYPE_MAX
} hi_drv_hdr_type;

typedef struct {
    hi_mem_handle mem_addr; /* mem fd, not phy or virt addr. */
    hi_u32        data_length;
} hi_drv_hdr_metadata_buf;

typedef struct {
    hi_u16 display_primaries_x[HI_DRV_HDR_DISPLAY_PRIMARIES_NUM];
    hi_u16 display_primaries_y[HI_DRV_HDR_DISPLAY_PRIMARIES_NUM];
    hi_u16 white_point_x;
    hi_u16 white_point_y;
    hi_u32 max_display_mastering_luminance;
    hi_u32 min_display_mastering_luminance;
} hi_drv_hdr_mastering_display_info;

typedef struct {
    hi_u32 max_content_light_level;
    hi_u32 max_pic_average_light_level;
} hi_drv_hdr_content_light_level;

typedef struct {
    hi_bool mastering_available;
    hi_bool content_available;
    hi_drv_hdr_mastering_display_info mastering_info;
    hi_drv_hdr_content_light_level content_info;
} hi_drv_hdr_static_metadata;

typedef struct {
    hi_bool  static_metadata_available;
    hi_bool  dynamic_metadata_available;

    hi_drv_hdr_static_metadata static_metadata;
    hi_drv_hdr_metadata_buf dynamic_metadata;
} hi_drv_hdr_sl_metadata, hi_drv_hdr_hdr10plus_metadata;

typedef struct {
    hi_bool  mastering_available;
    hi_bool  content_available;
    hi_bool  backwards_compatible;

    hi_drv_hdr_mastering_display_info   mastering_info;
    hi_drv_hdr_content_light_level      content_info;
} hi_drv_hdr_hlg_metadata;

typedef struct {
    hi_bool                     metadata_valid;
    hi_bool                     compatible;
    hi_drv_hdr_metadata_buf     metadata;
} hi_drv_hdr_dolby_metadata;

#define DOLBY_VSIF_MAX_LENGTH 32
typedef struct {
    hi_u32  dolby_back_light;
    hi_u8   dolby_vsif_data[DOLBY_VSIF_MAX_LENGTH]; /* store dolby vsif info. */
    hi_bool dolby_vsif_flag; /* whether dolby vsif or not. */
    hi_bool low_latency;     /* whether dolby lowlatency or not. */
} hi_drv_hdr_dolby_ctrl_info;

typedef union {
    hi_drv_hdr_static_metadata     hdr10_info;       /* HDR10 frame info. */
    hi_drv_hdr_hlg_metadata        hlg_info;         /* HLG frame info. */

    hi_drv_hdr_sl_metadata         sl_hdr_info;
    hi_drv_hdr_hdr10plus_metadata  hdr10_plus_info;  /* HDR10 plus frame info */
    hi_drv_hdr_dolby_metadata      dolby_info;       /* dolby frame info. */
} hi_drv_hdr_metadata;

typedef struct {
    hi_drv_hdr_type        hdr_type;
    hi_drv_color_descript  color_description;
    hi_drv_hdr_metadata    hdr_metadata;
} hi_drv_hdr_setting_info;

/** @} */ /** <!-- ==== API declaration end ==== */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

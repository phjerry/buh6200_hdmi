/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: struct define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __HI_DRV_PQ_V6_H__
#define __HI_DRV_PQ_V6_H__

#include <linux/ioctl.h>
#include "hi_type.h"
#include "hi_debug.h"
#include "drv_pq_define.h"
#include "hi_drv_module.h"
#include "hi_errno.h"
#include "hi_drv_color.h"
#include "hi_drv_video.h"
#include "drv_pq_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define VPSS_HANDLE_NUM 20
#endif

#ifdef PQ_PROC_CTRL_SUPPORT
#define HI_FATAL_PQ(fmt...) HI_TRACE(HI_LOG_LEVEL_FATAL, HI_ID_PQ, fmt)
#define HI_ERR_PQ(fmt...)   HI_TRACE(HI_LOG_LEVEL_ERROR, HI_ID_PQ, fmt)
#define HI_WARN_PQ(fmt...)  HI_TRACE(HI_LOG_LEVEL_WARNING, HI_ID_PQ, fmt)
#define HI_INFO_PQ(fmt...)  HI_TRACE(HI_LOG_LEVEL_INFO, HI_ID_PQ, fmt)
#define HI_DEBUG_PQ(fmt...) HI_TRACE(HI_LOG_LEVEL_DBG, HI_ID_PQ, fmt)
#else
#define HI_FATAL_PQ(fmt...)
#define HI_ERR_PQ(fmt...)
#define HI_WARN_PQ(fmt...)
#define HI_INFO_PQ(fmt...)
#define HI_DEBUG_PQ(fmt...)
#endif

/* PQ Alg Module */
typedef enum {
    HI_PQ_MODULE_ALL = 0,
    HI_PQ_MODULE_SHARPNESS,
    HI_PQ_MODULE_CLE,
    HI_PQ_MODULE_DCI,
    HI_PQ_MODULE_ACM,
    HI_PQ_MODULE_SR,
    HI_PQ_MODULE_CSC,
    HI_PQ_MODULE_ZME,
    HI_PQ_MODULE_DC,
    HI_PQ_MODULE_HDR,
    HI_PQ_MODULE_TNR,
    HI_PQ_MODULE_SNR,
    HI_PQ_MODULE_CCS,
    HI_PQ_MODULE_CLS,
    HI_PQ_MODULE_DB,
    HI_PQ_MODULE_DM,
    HI_PQ_MODULE_DBM,
    HI_PQ_MODULE_DR,
    HI_PQ_MODULE_DS,
    HI_PQ_MODULE_DEI,
    HI_PQ_MODULE_FMD,
    HI_PQ_MODULE_FOD,
    HI_PQ_MODULE_COCO,
    HI_PQ_MODULE_VPSSCSC,
    HI_PQ_MODULE_VPSSZME,
    HI_PQ_MODULE_VPSSHDR,
    HI_PQ_MODULE_GFXCSC,
    HI_PQ_MODULE_GFXZME,
    HI_PQ_MODULE_GFXHDR,

    HI_PQ_MODULE_MAX
} hi_pq_module_type;

typedef struct {
    hi_u16 brightness;
    hi_u16 contrast;
    hi_u16 hue;
    hi_u16 saturation;
    hi_u16 wcg_temperature;
} hi_pq_image_param;

typedef enum {
    HI_PQ_HDR_MODE_HDR10_TO_SDR = 0,
    HI_PQ_HDR_MODE_HDR10_TO_HDR10,
    HI_PQ_HDR_MODE_HDR10_TO_HLG,
    HI_PQ_HDR_MODE_HLG_TO_SDR,
    HI_PQ_HDR_MODE_HLG_TO_HDR10,
    HI_PQ_HDR_MODE_HLG_TO_HLG,
    HI_PQ_HDR_MODE_SDR_TO_SDR,
    HI_PQ_HDR_MODE_SDR_TO_HDR10,
    HI_PQ_HDR_MODE_SDR_TO_HLG,
    HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10PLUS,
    HI_PQ_HDR_MODE_HDR10PLUS_TO_SDR,
    HI_PQ_HDR_MODE_HDR10PLUS_TO_HDR10,
    HI_PQ_HDR_MODE_HDR10PLUS_TO_HLG,

    HI_PQ_HDR_MODE_MAX,
} hi_pq_hdr_mode;

typedef struct {
    hi_pq_hdr_mode hdr_process_scene;
    hi_u32 hdr_mode;
    hi_u32 bright;
    hi_u32 contrast;
    hi_u32 satu;
    hi_u32 hue;
    hi_u32 wcg_temperature;   /* saturation adjust value,range[0,100],default setting 50 */
    hi_u32 r;
    hi_u32 g;
    hi_u32 b;
    hi_u32 dark_cv;
    hi_u32 bright_cv;
    hi_u32 ac_cdark;
    hi_u32 ac_cbrigt;
} hi_pq_hdr_offset;

typedef struct {
    hi_pq_module_type module;
    hi_bool enable;
} hi_pq_module_enable;

typedef struct {
    hi_pq_module_type module;
    hi_u32 strength;
} hi_pq_module_strenght;

typedef struct {
    hi_pq_module_type module;
    hi_bool enable;
} hi_pq_demo_enable;

/* demo mode */
typedef enum {
    HI_PQ_DEMO_MODE_FIXED_R = 0,
    HI_PQ_DEMO_MODE_FIXED_L,
    HI_PQ_DEMO_MODE_SCROLL_R,
    HI_PQ_DEMO_MODE_SCROLL_L,

    HI_PQ_DEMO_MODE_MAX
} hi_pq_demo_mode;

/*
 * register info
 */
typedef struct {
    hi_u64 reg_addr;   /* register addr */
    hi_u8 lsb;         /* register lsb */
    hi_u8 msb;         /* register msb */
    hi_u8 source_mode; /* video source */
    hi_u8 output_mode; /* output mode */
    hi_u32 module;     /* module */
    hi_u32 value;      /* register value */
} hi_pq_register;

/* DCI Clut */
typedef struct {
    hi_s16 wgt_coef0[DCI_WGT_COEF_NUM];
    hi_s16 wgt_coef1[DCI_WGT_COEF_NUM];
    hi_s16 wgt_coef2[DCI_WGT_COEF_NUM];
    hi_u16 gain0;
    hi_u16 gain1;
    hi_u16 gain2;
} hi_pq_dci_wgt;

/* DCI Black Stretch Lut */
typedef struct {
    hi_s16 bs_delta[DCI_BS_LUT];
} hi_pq_dci_bs_lut;

typedef struct {
    hi_u16 gamm_coef[CSC_GAMM_COEF_NUM];
} hi_pq_csc_gamm_para;

typedef struct {
    hi_s32 mapping_max;                /* meanedgeratio max clut value 0~127 */
    hi_s32 mapping_min;                /* meanedgeratio min clut value 0~127 */
    hi_s32 mapping_x[SNR_MEAN_MAPP_X]; /* meanedgeratio X */
    hi_s32 mapping_y[SNR_MEAN_MAPP_Y]; /* meanedgeratio Y */
    hi_s32 mapping_k[SNR_MEAN_MAPP_K]; /* meanedgeratio K */
} hi_pq_snr_mean_ratio;

typedef struct {
    hi_s32 mapping_max;                /* edgestrmapping max clut value 0~63 */
    hi_s32 mapping_min;                /* edgestrmapping min clut value 0~63 */
    hi_s32 mapping_x[SNR_EDGE_MAPP_X]; /* edgestrmapping X */
    hi_s32 mapping_y[SNR_EDGE_MAPP_Y]; /* edgestrmapping Y */
    hi_s32 mapping_k[SNR_EDGE_MAPP_K]; /* edgestrmapping K */
} hi_pq_snr_edge_str;

typedef struct {
    hi_u32 red;     /* Range:0~100 */
    hi_u32 green;   /* Range:0~100 */
    hi_u32 blue;    /* Range:0~100 */
    hi_u32 cyan;    /* Range:0~100 */
    hi_u32 magenta; /* Range:0~100 */
    hi_u32 yellow;  /* Range:0~100 */
} hi_pq_six_base_color;

typedef enum {
    HI_PQ_COLOR_ENHANCE_FLESHTONE = 0,
    HI_PQ_COLOR_ENHANCE_SIX_BASE,
    HI_PQ_COLOR_ENHANCE_SPEC_COLOR_MODE,
    HI_PQ_COLOR_ENHANCE_MAX
} hi_pq_color_enhance_mode;

typedef enum {
    HI_PQ_FLESHTONE_GAIN_OFF = 0,
    HI_PQ_FLESHTONE_GAIN_LOW,
    HI_PQ_FLESHTONE_GAIN_MID,
    HI_PQ_FLESHTONE_GAIN_HIGH,

    HI_PQ_FLESHTONE_GAIN_MAX
} hi_pq_fleshtone;

typedef enum {
    HI_PQ_COLOR_MODE_RECOMMEND = 0,
    HI_PQ_COLOR_MODE_BLUE,
    HI_PQ_COLOR_MODE_GREEN,
    HI_PQ_COLOR_MODE_BG,
    HI_PQ_COLOR_MODE_ORIGINAL,

    HI_PQ_COLOR_MODE_MAX
} hi_pq_color_spec_mode;

typedef struct {
    hi_pq_color_enhance_mode color_enhance_type;
    union {
        hi_pq_fleshtone fleshtone_gain;
        hi_pq_six_base_color six_base_color;
        hi_pq_color_spec_mode color_space_mode;
    } color_gain;
} hi_pq_color_enhance;

typedef struct {
    hi_u32 red_gain;   /* <Red; Range:0~100 */
    hi_u32 green_gain; /* <Green; Range:0~100 */
    hi_u32 blue_gain;  /* <Blue; Range:0~100 */
    hi_u32 red_offset;   /* <Red; Range:0~100 */
    hi_u32 green_offset; /* <Green; Range:0~100 */
    hi_u32 blue_offset;  /* <Blue; Range:0~100 */
} hi_pq_color_temperature;

typedef struct {
    hi_s16 tm_lut_i[HDR_TM_LUT];
    hi_s16 tm_lut_s[HDR_TM_LUT];
    hi_s16 sm_lut_i[HDR_TM_LUT];
    hi_s16 sm_lut_s[HDR_TM_LUT];
} hi_pq_hdr_tm_lut;

typedef struct {
    hi_u32 hdr_tm_mode;
    hi_u32 hdr_sm_mode;
} hi_pq_hdr_para_mode;

typedef struct {
    hi_u32 x_step[HDR_TMAP];
    hi_u32 x_num[HDR_TMAP];
    hi_u32 x_pos[HDR_TMAP];
    hi_u32 y_lut[HDR_TMAP_LUT];
} hi_pq_hdr_tmap;

typedef struct {
    hi_u32 x_step[HDR_SMAP];
    hi_u32 x_num[HDR_SMAP];
    hi_u32 x_pos[HDR_SMAP];
    hi_u32 y_lut[HDR_SMAP_LUT]; /* 310 only 32 segment */
} hi_pq_hdr_smap;

typedef struct {
    hi_u32 para_mode_use; /* HDR para ; 0:para from code ; 1:para from tool adjust */
} hi_pq_hdr_acm_mode;

typedef struct {
    hi_s32 reg_cfg[PQ_HDR_ACM_REGCFG_SIZE];
} hi_pq_hdr_acm_regcfg;

typedef enum {
    HI_PQ_IMAGE_MODE_NORMAL = 0,
    HI_PQ_IMAGE_MODE_VIDEOPHONE,
    HI_PQ_IMAGE_MODE_GALLERY,

    HI_PQ_IMAGE_MODE_MAX,
} hi_pq_image_mode;

typedef enum {
    HI_DRV_PQ_VP_TYPE_NORMAL = 0, /* VPSS VideoPhone Normal */
    HI_DRV_PQ_VP_TYPE_PREVIEW,    /* VPSS VideoPhone Preview */
    HI_DRV_PQ_VP_TYPE_REMOTE,     /* VPSS VideoPhone Remote */

    HI_DRV_PQ_VP_TYPE_MAX
} hi_drv_pq_vp_type;

typedef enum {
    HI_DRV_PQ_VIDEOPHONE_MODE_RECOMMEND = 0, /* <Optimization Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_BLACK,         /* <BlackWhite Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_COLORFUL,      /* <Colorful Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_BRIGHT,        /* <Bright Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_WARM,          /* <Warm Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_COOL,          /* <Cool Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_OLD,           /* <Old Model */
    HI_DRV_PQ_VIDEOPHONE_MODE_USER,          /* <Old Model */

    HI_DRV_PQ_VIDEOPHONE_MODE_MAX
} hi_drv_pq_vp_mode;

/* display ID */
typedef enum {
    HI_PQ_DISPLAY_0 = 0, /**< DISPLAY0,compatible with SD */
    HI_PQ_DISPLAY_1,     /**< DISPLAY1, HD */

    HI_PQ_DISPLAY_MAX
} hi_pq_display;

typedef struct {
    hi_u32 gain_mode; /* 0:SD;1:HD;2:UHD */
    hi_u32 gainluma;  /* Range:0-1023 */
    hi_u32 gainhue;   /* Range:0-1023 */
    hi_u32 gainsat;
} hi_pq_color_gain;

/* ACM table */
typedef struct {
    hi_u32 lut_type;                             /* ACM Clut Type */
    hi_s16 lut[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM]; /* ACM Clut */
} hi_pq_acm_lut;

typedef enum {
    DCI_HISTGRAM_SITE_VO = 0,
    DCI_HISTGRAM_SITE_DHD,

    DCI_HISTGRAM_SITE_BUTT
} drv_pq_dci_histgram_site;

typedef struct {
    hi_bool normalize;
    drv_pq_dci_histgram_site histgram_site;
    hi_s32 histgram[32]; /* 32: hist size */
} drv_pq_dci_histgram_32;

typedef struct {
    hi_bool normalize;
    drv_pq_dci_histgram_site histgram_site;
    hi_s32 histgram[256]; /* 256: hist size */
} drv_pq_dci_histgram_256;

/* DCI Hist */
typedef struct {
    drv_pq_dci_histgram_32 dci_histgram_32;
    drv_pq_dci_histgram_256 dci_histgram_256;
} hi_pq_dci_histgram;

/*******************************vpss soft alg struct start ************************************************/
#define PQ_CHECK_VPSS_HANDLE(handle)                   \
    do {                                               \
        PQ_CHECK_UPPER_LIMIT(handle, VPSS_HANDLE_NUM); \
    } while (0)

#define HI_ERR_PrintU32(val) HI_ERR_PQ("%s = %u\n", #val, val)

#define PQ_CHECK_UPPER_LIMIT(value, upLimit)     \
    do {                                         \
        if ((value) >= (upLimit)) {                  \
            HI_ERR_PQ("param out of uplimit\n"); \
            HI_ERR_PrintU32(value);              \
            HI_ERR_PrintU32(upLimit);            \
            return HI_ERR_PQ_INVALID_PARA;       \
        }                                        \
    } while (0)

typedef struct {
    hi_s32 ar_gm_pnl_nl_offset;
    hi_s32 ar_min_nl_ratio;
    hi_s32 ar_nl_pnl_str;
    hi_s32 ar_nl_pnl_gm_offset;
    hi_s32 ar_dark_level_thd;
    hi_s32 ar_dark_nl_max;
    hi_s32 ar_nd_reli_max;
    hi_s32 ar_set_nl_min;
    hi_s32 ar_set_nl_max;
    hi_s32 ar_set_gm_min;
    hi_s32 ar_set_gm_max;
    hi_s32 ar_md_alpha1_offset;
    hi_s32 ar_md_alpha2_offset;
    hi_s32 ar_flat_info_x_min;
    hi_s32 ar_flat_info_x_max;
    hi_s32 ar_flat_info_y_min;
    hi_s32 ar_flat_info_y_max;
    hi_s32 ar_md_y_gain;
    hi_s32 ar_md_y_core;
    hi_s32 ar_md_c_gain;
    hi_s32 ar_md_c_core;
    hi_s32 ar_mc_y_gain;
    hi_s32 ar_mc_y_core;
    hi_s32 ar_mc_c_gain;
    hi_s32 ar_mc_c_core;
    hi_s32 ar_ref_mv_mode;
    hi_s32 ar_mag_pnl_gain_0mv;
    hi_s32 ar_mag_pnl_core_0mv;
    hi_s32 ar_mag_pnl_gain_xmv;
    hi_s32 ar_mag_pnl_core_xmv;
    hi_s32 ar_std_pnl_gain_0mv;
    hi_s32 ar_std_pnl_core_0mv;
    hi_s32 ar_std_pnl_gain_xmv;
    hi_s32 ar_std_pnl_core_xmv;
    hi_s32 ar_std_core_0mv;
    hi_s32 ar_std_core_xmv;
    hi_s32 ar_std_pnl_gain;
    hi_s32 ar_std_pnl_core;
    hi_s32 ar_adj_0mv_min;
    hi_s32 ar_adj_0mv_max;
    hi_s32 ar_adj_xmv_min;
    hi_s32 ar_adj_xmv_max;
    hi_s32 ar_adj_mv_max;
    hi_s32 ar_adj_mv_min;
    hi_s32 ar_mc1d_alpha;
    hi_s32 ar_tnr_str;
    hi_s32 ar_cnr_str;

    hi_s32 ar_strongest_lut[32]; /* 32: lut size */
    hi_s32 ar_weakest_lut[32]; /* 32: lut size */

    hi_s32 tnr_level_gain[4]; /* 4: gain size */
    hi_s32 mcnr_level_gain[4]; /* 4: gain size */
    hi_s32 snr_level_gain[4]; /* 4: gain size */
    hi_s32 mcnr_mc_mot_en;
    hi_s32 mc_mt2dfs_adj_en;
    hi_s32 y_no_nr_range;
    hi_s32 c_no_nr_range;
} hi_drv_pq_nr_vir_reg;

typedef struct {
    hi_s32 k_y_mcw;
    hi_s32 k_c_mcw;
    hi_s32 k_y_mcbld;
    hi_s32 k_c_mcbld;
    hi_s32 fld_cnt_adjust_en;
    hi_s32 fld_cnt_x;
    hi_s32 fld_cnt_k;
    hi_s32 gs_graph_x0;
    hi_s32 gs_graph_x1;
    hi_s32 shift_gs_graph;
    hi_s32 even_odd_diff_cnt_core;
    hi_s32 art_scene_cnt_core;
    hi_s32 still_scene_cnt_core;
    hi_s32 still_bg_scene_cnt_core;
    hi_s32 fea_scene_cnt_core;

    hi_s32 film_like_32_cnt;
    hi_s32 film_like_22_cnt;
    hi_s32 film_like_cnt;
    hi_s32 add_32_blk_cnt;
    hi_s32 add_32_pxl_cnt;
    hi_s32 film_like_22_main_cnt;
    hi_s32 film_like_22_ass_cnt;
    hi_s32 not_found_scene_cnt;
    hi_s32 not_found_scene_wt;
    hi_s32 tb_luma_diff_cnt;
    hi_s32 tb_luma_diff_wt;
    hi_s32 still_scene_cnt;
    hi_s32 still_scene_wt;
    hi_s32 cont_not_still_scene_cnt;
    hi_s32 art_scene_cnt;
    hi_s32 art_scene_wt;
    hi_s32 cont_not_art_scene_cnt;
    hi_s32 jitter_scene_cnt;
    hi_s32 jitter_scene_wt;
    hi_s32 cont_not_jitter_scene_cnt;
    hi_s32 zoom_scene_cnt;
    hi_s32 zoom_scene_wt;
    hi_s32 cont_not_zoom_scene_cnt;
    hi_s32 jitter_fea_scene_cnt;
    hi_s32 jitter_fea_scene_wt;
    hi_s32 non_jitter_fea_scene_cnt;
    hi_s32 non_jitter_fea_scene_wt;
    hi_s32 v_pan_scan_cnt;
    hi_s32 v_pan_scan_wt;
    hi_s32 v_cont_not_pan_cnt;
    hi_s32 h_pan_scan_cnt;
    hi_s32 h_pan_scan_wt;
    hi_s32 h_cont_not_pan_cnt;
    hi_s32 fast_move_cnt;
    hi_s32 fast_move_wt;
    hi_s32 cont_not_fast_move_cnt;
    hi_s32 h_move_scene_wt;
    hi_s32 v_move_scene_wt;
    hi_s32 still_bg_scene_cnt;
    hi_s32 still_bg_scene_wt;
    hi_s32 cont_not_still_bg_scene_cnt;
    hi_s32 g_score;
    hi_s32 max_err_line[5]; /* 5 : len */
    hi_s32 total_err_line[5]; /* 5 : len */
    hi_s32 blk_fea_cnt_frame[5]; /* 5 : len */
    hi_s32 pxl_fea_cnt_frame[5]; /* 5 : len */
    hi_s32 fld_cnt;
} hi_drv_pq_dei_vir_reg;

typedef struct {
    hi_s32 rate_smth_win;
    hi_s32 as32_limit_lut[4]; /* 4 : len */
    hi_s32 as32_flag_lut[5]; /* 5 : len */
    hi_s32 min_blk_num_hy;
    hi_s32 min_blk_num_vy;
    hi_s32 bord_count_thres_hy;
    hi_s32 bord_count_thres_vy;
    hi_s32 as32_rat_idx_lut[16]; /* 16 : len */
} hi_drv_pq_dbm_vir_reg;

typedef struct {
    hi_s32 mode32_en;
    hi_s32 mode2332_en;
    hi_s32 mode32322_en;
    hi_s32 mode11_2_3_en;
    hi_s32 mode22_en;
    hi_s32 mode55_64_87_en;
    hi_u32 ifmd_debug_en;
    hi_u32 ifmd_debug32_en[3]; /* 3 : len */
    hi_u32 ifmd_debug2332_en[3]; /* 3 : len */
    hi_u32 ifmd_debug32322_en[3]; /* 3 : len */
    hi_u32 ifmd_debug11_2_3_en[3]; /* 3 : len */
    hi_u32 ifmd_debug22_en[3]; /* 3 : len */
    hi_u32 ifmd_debug55_64_87_en[3]; /* 3 : len */
} hi_drv_pq_ifmd_vir_reg;

typedef struct {
    hi_drv_pq_dei_vir_reg dei_reg;
    hi_drv_pq_nr_vir_reg nr_reg;
    hi_drv_pq_dbm_vir_reg dbm_reg;
    hi_drv_pq_ifmd_vir_reg ifmd_reg;
} hi_drv_pq_dbg_vpss_vir_soft;

typedef struct {
    hi_s32 sc_hist_cf[32]; /* 32: hist size */
} drv_pq_scd_stt_reg;

typedef struct {
    hi_u32 nd_noise_point_cnt_max;
    hi_u32 nd_noise_point_cnt_med;
    hi_u32 nd_noise_point_cnt_min;

    hi_u32 nd_noise_point_num;

    hi_u32 nd_sum_noise_info_max;
    hi_u32 nd_sum_noise_info_med;
    hi_u32 nd_sum_noise_info_min;

    hi_u32 me_blk_mv_hist[5]; /* 5: hist size */

    hi_s32 dci_histgram[32]; /* 32: hist size */
    hi_bool dci_enable;
} hi_drv_pq_tnr_stt_reg;

typedef struct {
    hi_bool me_ds_en;
    hi_u32 me_lft_bndy;
    hi_u32 me_rgt_bndy;
    hi_u32 me_top_bndy;
    hi_u32 me_bot_bndy;
} drv_pq_me_input_reg;

typedef struct {
    hi_u16 me_hist_mv[5]; /* 5: hist size */
    hi_u16 me_hist_mag[5]; /* 5: hist size */
} drv_pq_tnr_rgmv_info;

typedef struct {
    hi_s32 ar_sdi_str;
    hi_s32 ar_motion_alpha_max;
} drv_pq_snr_input_reg;

typedef struct {
    hi_s32 ar_gm_pnl_nl_offset;
    hi_s32 ar_min_nl_ratio;
    hi_s32 ar_nl_pnl_str;
    hi_s32 ar_nl_pnl_gm_offset;
    hi_s32 ar_dark_level_thd;
    hi_s32 ar_dark_nl_max;
    hi_s32 ar_nd_reli_max;
    hi_s32 ar_set_nl_min;
    hi_s32 ar_set_nl_max;
    hi_s32 ar_set_gm_min;
    hi_s32 ar_set_gm_max;
    hi_s32 ar_md_alpha1_offset;
    hi_s32 ar_md_alpha2_offset;
    hi_s32 ar_flat_info_x_min;
    hi_s32 ar_flat_info_x_max;
    hi_s32 ar_flat_info_y_min;
    hi_s32 ar_flat_info_y_max;
    hi_s32 ar_md_y_gain;
    hi_s32 ar_md_y_core;
    hi_s32 ar_md_c_gain;
    hi_s32 ar_md_c_core;
    hi_s32 ar_mc_y_gain;
    hi_s32 ar_mc_y_core;
    hi_s32 ar_mc_c_gain;
    hi_s32 ar_mc_c_core;
    hi_s32 ar_ref_mv_mode;
    hi_s32 ar_mag_pnl_gain_0mv;
    hi_s32 ar_mag_pnl_core_0mv;
    hi_s32 ar_mag_pnl_gain_xmv;
    hi_s32 ar_mag_pnl_core_xmv;
    hi_s32 ar_std_pnl_gain_0mv;
    hi_s32 ar_std_pnl_core_0mv;
    hi_s32 ar_std_pnl_gain_xmv;
    hi_s32 ar_std_pnl_core_xmv;
    hi_s32 ar_std_core_0mv;
    hi_s32 ar_std_core_xmv;
    hi_s32 ar_std_pnl_gain;
    hi_s32 ar_std_pnl_core;
    hi_s32 ar_adj_0mv_min;
    hi_s32 ar_adj_0mv_max;
    hi_s32 ar_adj_xmv_min;
    hi_s32 ar_adj_xmv_max;
    hi_s32 ar_adj_mv_max;
    hi_s32 ar_adj_mv_min;
    hi_s32 ar_mc1d_alpha;
    hi_s32 ar_tnr_str;
    hi_s32 ar_cnr_str;

    hi_s32 ar_strongest_lut[32]; /* 32: lut size */
    hi_s32 ar_weakest_lut[32]; /* 32: lut size */

    hi_s32 y_motion_str_lut[128]; /* 128: lut size */
    hi_s32 c_motion_str_lut[128]; /* 128: lut size */

    hi_bool motion_estimation_en;
    hi_bool noise_detect_en;
    hi_bool mcnr_en;
    hi_bool ar_mcnr_mc_mot_en;
    hi_bool ar_mc_mt2dfs_adj_en;

    hi_u8 y_no_nr_range;
    hi_u8 c_no_nr_range;

    hi_u8 reserved;
} hi_drv_pq_tnr_input_reg;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_drv_pq_frame_fmt frame_fmt;
    hi_bool is_nr_ready;
    hi_u32 handle;

    drv_pq_scd_stt_reg scd_stt_reg;
    hi_drv_pq_tnr_stt_reg tnr_stt_reg;
    drv_pq_me_input_reg me_input_reg;
    hi_drv_pq_tnr_input_reg tnr_input_reg;
    drv_pq_snr_input_reg snr_input_reg;
    drv_pq_tnr_rgmv_info rgmv_info;
} hi_drv_pq_nr_input_info;

typedef struct {
    hi_s32 sdi_str;
    hi_s32 motion_alpha;
    hi_s32 scene_change_info;
} hi_drv_pq_snr_output_reg;

typedef struct {
    hi_u8 md_alpha1;
    hi_u8 md_alpha2;

    hi_u8 md_y_gain;
    hi_s8 md_y_core;
    hi_u8 md_c_gain;
    hi_s8 md_c_core;

    hi_u8 mc_y_gain;
    hi_s8 mc_y_core;
    hi_u8 mc_c_gain;
    hi_s8 mc_c_core;

    hi_u8 me_gm_adj;

    hi_bool me_discard_smv_x_en;
    hi_bool me_discard_smv_y_en;
    hi_u8 me_smv_x_thd;
    hi_u8 me_smv_y_thd;

    hi_s32 me_std_pnl_gain_0mv;
    hi_s32 me_std_pnl_core_0mv;
    hi_s32 me_std_pnl_gain_xmv;
    hi_s32 me_std_pnl_core_xmv;

    hi_s32 me_std_core_0mv;
    hi_s32 me_std_core_xmv;
    hi_s32 me_mag_pnl_gain_0mv;
    hi_s32 me_mag_pnl_core_0mv;
    hi_s32 me_mag_pnl_gain_xmv;
    hi_s32 me_mag_pnl_core_xmv;

    hi_s32 me_std_pnl_gain;
    hi_s32 me_std_pnl_core;

    hi_u8 me_adj_mv_max;
    hi_u8 me_adj_mv_min;
    hi_s32 me_adj_0mv_min;
    hi_s32 me_adj_0mv_max;
    hi_s32 me_adj_xmv_min;
    hi_s32 me_adj_xmv_max;

    hi_u8 scd_val;
    hi_s8 noise_level;
    hi_s8 global_motion;  /* 0-31 */
    hi_u8 saltus_level;
    hi_u8 mc1d_alpha;

    hi_u8 y_tf_lut[32]; /* 32: lut size */
    hi_u8 c_tf_lut[32]; /* 32: lut size */

    hi_u32 ori_global_motion;  /* 0-255 */

    hi_bool mcnr_mc_mot_en;
    hi_bool mc_mt_2dfs_adj_en;

    hi_u8 ar_y_no_nr_range;
    hi_u8 ar_c_no_nr_range;
} hi_drv_pq_tnr_output_reg;

typedef struct {
    hi_s32 yh_noise;
} hi_drv_pq_ccs_output_reg;

typedef struct {
    hi_drv_pq_tnr_output_reg tnr_out_reg;
    hi_drv_pq_snr_output_reg snr_out_reg;
    hi_drv_pq_ccs_output_reg ccs_out_reg;
    hi_bool is_nr_output_ready;
} hi_drv_pq_nr_api_output_reg;

/* *************************dei soft alg struct ************************ */
/* dei api struct */
typedef struct {
    hi_s32 x;
    hi_s32 y;
} drv_pq_mvt;

typedef struct {
    drv_pq_mvt mv;
    hi_s32 num;
    hi_s32 good_mv_cnt;
} drv_pq_gmvt;

typedef struct {
    hi_s32 top_bndy;
    hi_s32 bot_bndy;
    hi_s32 lft_bndy;
    hi_s32 rgt_bndy;
    drv_pq_gmvt glbmv[2]; /* 2 : len */
} drv_pq_me_in_parm;

typedef struct {
    hi_s32 max_err_line;
    hi_s32 total_err_line;
    hi_s32 blk_fea_cnt_frame;
    hi_s32 pxl_fea_cnt_frame;
    hi_s32 film_video_line_num;
    hi_s32 film_video_max_num;
    hi_s32 not_found_fea_cnt;
    hi_s32 still_cnt;
    hi_s32 still_fea_cnt;
    hi_s32 jitter_big_mv_num[4]; /* 4 : len */
    hi_s32 jitter_small_mv_num[4]; /* 4 : len */
    hi_s32 jitter_area_mc_fea_cnt;
    hi_s32 non_jitter_area_mc_fea_cnt;
    hi_s32 sigma_cnt;
    hi_s32 scd_value;
    hi_s32 img_mean_value;

    hi_s32 hist_bin[64]; /* 64 : len */
} drv_pq_di_in_parm;

typedef struct {
    hi_s32 k_y_mcw;
    hi_s32 k_c_mcw;
    hi_s32 k_y_mcbld;
    hi_s32 k_c_mcbld;

    hi_s32 gs_graph_x0;
    hi_s32 gs_graph_x1;
    hi_s32 shift_gs_graph;

    /* virtual register */
    hi_s32 even_odd_diff_cnt_core;
    hi_s32 art_scene_cnt_core;
    hi_s32 still_scene_cnt_core;
    hi_s32 still_bg_scene_cnt_core;
    hi_s32 fea_scene_cnt_core;

    /* fld cnt */
    hi_s32 fld_cnt_adjust_en;
    hi_s32 fld_cnt_x;
    hi_s32 fld_cnt_k;
} hi_drv_pq_dei_drv_uparm;

typedef struct {
    /* FILM DRIVE */
    hi_s32 film_like_32_cnt;
    hi_s32 film_like_22_cnt;
    hi_s32 film_like_cnt;
    hi_s32 add_32_blk_cnt;
    hi_s32 add_32_pxl_cnt;
    hi_s32 film_like_22_main_cnt;
    hi_s32 film_like_22_ass_cnt;

    /* slight fea driver */
    hi_s32 not_found_scene_cnt;
    hi_s32 not_found_scene_wt;
    hi_s32 tb_luma_diff_cnt;
    hi_s32 tb_luma_diff_wt;
    hi_s32 still_scene_cnt;
    hi_s32 cont_not_tbluma_diff_cnt;
    hi_s32 still_scene_wt;
    hi_s32 cont_not_still_scene_cnt;
    hi_s32 art_scene_cnt;
    hi_s32 art_scene_wt;
    hi_s32 cont_not_art_scene_cnt;
    /* add new stillcnt almost */
    hi_s32 still_scene_almost_cnt;
    hi_s32 still_scene_almost_wt;

    /* mc fea driver */
    hi_s32 jitter_scene_cnt;
    hi_s32 jitter_scene_wt;
    hi_s32 cont_not_jitter_scene_cnt;
    hi_s32 zoom_scene_cnt;
    hi_s32 zoom_scene_wt;
    hi_s32 cont_not_zoom_scene_cnt;
    hi_s32 jitter_fea_scene_cnt;
    hi_s32 jitter_fea_scene_wt;
    hi_s32 non_jitter_fea_scene_cnt;
    hi_s32 non_jitter_fea_scene_wt;
    hi_s32 v_pan_scan_cnt;
    hi_s32 v_pan_scan_wt;
    hi_s32 v_cont_not_pan_cnt;
    hi_s32 h_pan_scan_cnt;
    hi_s32 h_pan_scan_wt;
    hi_s32 h_cont_not_pan_cnt;
    hi_s32 fast_move_cnt;
    hi_s32 fast_move_wt;
    hi_s32 cont_not_fast_move_cnt;

    hi_s32 h_move_scene_wt;
    hi_s32 v_move_scene_wt;

    hi_s32 still_bg_scene_cnt;
    hi_s32 still_bg_scene_wt;
    hi_s32 cont_not_still_bg_scene_cnt;

    hi_s32 graph_score;

    /* stat */
    hi_s32 max_err_line[5]; /* 5 : len */
    hi_s32 total_err_line[5]; /* 5 : len */
    hi_s32 blk_fea_cnt_frame[5]; /* 5 : len */
    hi_s32 pxl_fea_cnt_frame[5]; /* 5 : len */

    hi_s32 fld_cnt;
    hi_s32 tb_luma_sigma_cnt;
    /* hi_s32 s32GoodMvStatCnt; solve tbluma problem */
    hi_s32 slight_move_cnt;
    hi_s32 slight_move_wt;
    hi_s32 cont_not_slight_move_cnt;
    hi_s32 cont_not_good_mv_stat_cnt;
    hi_s32 good_mv_stat_cnt;
    hi_s32 good_mv_ratio;
    hi_s32 good_mv_hor_mov;
    hi_s32 good_mv_stat_wt;
    hi_s32 cont_not_ma_only_cnt;
    hi_s32 ma_only_cnt;
    /* cornfield */
    hi_s32 full_img_core_fea_cnt;
    hi_s32 full_img_still_cnt;
    hi_s32 scd_chg_spcnt;
    hi_s32 his_scd_chg_info[3]; /* 3 : len */
    /* hmove word 60 drop to 50 */
    hi_s32 ifmd_comb_result[5]; /* 5 : len */
    /* in width, height */
    hi_s32 frm_width;
    hi_s32 frm_height;
    /* for film22 add into */
    hi_s32 sub_film_add;
    hi_s32 sub_film_still;
    hi_s32 sub_film_sub_step;
    hi_s32 main_film_add;
    hi_s32 main_fim_still;
    hi_s32 main_film_sub_step;
    hi_s32 blk_film_add;
    hi_s32 blk_film_still;
    hi_s32 blk_film_sub_step;
    hi_s32 pxl_film_add;
    hi_s32 pxl_film_still;
    hi_s32 pxl_film_sub_step;
} hi_drv_pq_dei_drv_sparm;

typedef struct {
    hi_s32 film_like_weight;
    hi_s32 film_like_line_mode;
    hi_s32 mc_fea_max_wt;
    hi_s32 mc_fea_min_wt;
    hi_s32 jitter_fea_core_wt;
    hi_s32 non_jitter_fea_core_wt;
    hi_s32 check_sub_title;
    hi_s32 mc_not_confirm_wt;
    hi_s32 slight_fea_max_wt;
    hi_s32 slight_fea_mv_med_en;
    hi_s32 slight_fea_mv_core_th;
    hi_s32 fea_top_bndy;
    hi_s32 fea_bot_bndy;
    hi_s32 fea_lft_bndy;
    hi_s32 fea_rgt_bndy;
    hi_s32 mc_mid_dlt_en;
    hi_s32 mc_fea_en;
    hi_s32 mag_wnd_mcw_x0;
    hi_s32 mag_wnd_mcw_k0;
    hi_s32 mag_wnd_mcw_g0;
    hi_s32 mag_wnd_mcw_k1;
    hi_s32 smag_wnd_mcw_x0;
    hi_s32 smag_wnd_mcw_k0;
    hi_s32 smag_wnd_mcw_g0;
    hi_s32 smag_wnd_mcw_k1;
    hi_s32 sad_wnd_mcw_x0;
    hi_s32 sad_wnd_mcw_k0;
    hi_s32 sad_wnd_mcw_g0;
    hi_s32 sad_wnd_mcw_k1;
    hi_s32 mt_adj_fea_en;
    hi_s32 ma_only;
    hi_s32 ma_merge_en;
    hi_s32 k_y_mcw;
    hi_s32 k_y_mcbld;
    hi_s32 k_c_mcw;
    hi_s32 k_c_mcbld;
    hi_s32 gain_bld_zmv_sad;
    /* solve cornfield */
    hi_s32 mc_fea_non_jitter_core_th;
    hi_s32 mc_fea_jitter_core_th;
    hi_s32 slight_fea_vdiff_core_th;
    hi_s32 slight_fea_hdiff_core_th;
    hi_s32 slight_fea_core_th;
    hi_s32 slight_fea_mc_gain;
    hi_s32 mt_adj_fea_zmvsad_add;
    hi_s32 rec_mode_fld_mt_step1;
    hi_s32 fld_motion_gain;
    hi_s32 set_pre_info_mode;
    hi_s32 max_motion_y_en;
    hi_s32 max_motion_uv_en;
    /* scd scene */
    hi_s32 his_motion_en;
    hi_s32 rec_mode_en;
    /* word move */
    hi_s32 edge_str_limit_scale;
} drv_pq_di_out_parm;

typedef struct {
    /* add dei output here */
    int test;
    drv_pq_di_out_parm di;
    hi_drv_pq_dei_drv_sparm dei_drv_param;
    hi_drv_pq_vpss_cfg_info vpss_cfg_info;
    hi_bool is_dei_output_ready;
} hi_drv_pq_dei_api_output_reg;

typedef struct {
    /* add dei input here */
    int test;

    hi_s32 src_width;
    hi_s32 src_height;
    hi_s32 frame_width;
    hi_s32 frame_height;
    hi_s32 blk_siz_h;
    hi_s32 blk_siz_v;
    hi_s32 me_width;
    hi_s32 me_height;
    drv_pq_me_in_parm me;
    drv_pq_di_in_parm di;
    hi_drv_pq_dei_drv_uparm dei_update;
    hi_drv_pq_vpss_cfg_info vpss_cfg_info;

    hi_s32 graph_psb;
    hi_s32 dci_mean_value;     /* adjust SlightFeaStatCoreTh to fix tbluma */
    hi_s32 film_type_from_fmd; /* get film_type from fmd */
    hi_s32 frm_global_motion;  /* get from stt info */

    hi_u32 handle;
    hi_bool is_dei_ready;
    hi_drv_source input_src;
} drv_pq_dei_input_info;
/* *************************dei soft alg struct ************************ */
/* *************************db soft alg struct ************************ */
typedef struct {
    hi_s32 y_width;
    hi_s32 y_height;

    hi_s32 detblk_en;
    hi_s32 hy_en;
    hi_s32 vy_en;
    hi_s8 detsize_hy;
    hi_s8 detsize_vy;
    hi_u32 counter_border_hy;
    hi_u32 counter_border_vy;
    hi_u32 border_index_buf_hy[128]; /* array index 128 */
    hi_u32 border_index_buf_vy[128]; /* array index 128 */
    hi_s32 test_blknum_hy;
    hi_s32 test_blknum_vy;
    hi_u8 bord_str_hy;

    hi_u32 str_row_hy[2048]; /* array index 2048 */
    hi_s32 glb_motion;
} dbd_input_info;

typedef struct {
    hi_s8 using_size_hy;
    hi_s8 using_size_vy;
    hi_u32 using_counter_border_hy;
    hi_u32 using_counter_border_vy;
    hi_u32 using_border_index_buf_hy[128]; /* array index 128 */
    hi_u32 using_border_index_buf_vy[128]; /* array index 128 */
    hi_s32 test_blknum_hy;
    hi_s32 test_blknum_vy;
    hi_u8 bord_str_hy;
} dbd_output_info;

typedef struct {
    hi_s32 y_width;
    hi_s32 y_height;
    hi_s32 bit_depth;

    hi_s32 db_en;
    hi_s32 db_lumhor_en;
    hi_s32 db_lumver_en;
    hi_s32 db_smooth_detnum_en;

    hi_s32 dir_smooth_mode;
    hi_s32 grad_sub_ratio;
    hi_s32 db_ctrst_thresh;
    hi_s32 lum_hor_scale_ratio;
    hi_s32 lum_hor_filter_sel;
    hi_s32 lum_hor_txt_winsize;
    hi_s32 max_lum_hor_db_dist;
    hi_s32 lum_h_blknum;
    hi_s32 lum_hor_adj_gain;
    hi_s32 lum_hor_hf_var_core;
    hi_s32 lum_hor_hf_var_gain1;
    hi_s32 lum_hor_hf_var_gain2;
    hi_s32 lum_hor_bord_adj_gain;
    hi_s32 lum_hor_hf_diff_core;
    hi_s32 lum_hor_hf_diff_gain1;
    hi_s32 lum_hor_hf_diff_gain2;
    hi_s32 ctrst_adj_core;
    hi_s32 ctrst_adj_gain1;
    hi_s32 ctrst_adj_gain2;
    hi_s32 lum_h_str_fade_lut[12]; /* array index 12 */
    hi_s32 lum_hor_delta_lut[17]; /* array index 17 */

    hi_s32 lum_v_blknum;
    hi_s32 lum_ver_scale_ratio;
    hi_s32 lum_ver_txt_winsize;
    hi_s32 lum_v_str_fade_lut[12]; /* array index 12 */
    hi_s32 lum_ver_delta_lut[17]; /* array index 17 */
    hi_s32 lum_ver_adj_gain;
    hi_s32 lum_ver_hf_var_core;
    hi_s32 lum_ver_hf_var_gain1;
    hi_s32 lum_ver_hf_var_gain2;
    hi_s32 lum_ver_bord_adj_gain;

    hi_s32 db_adptflt_mode;
    hi_s32 db_adptflt_mode_final_sel;

    hi_s32 global_static_protect_en;
    hi_s32 min_gm_thres;
    hi_s32 min_bs_hy_thres1;
    hi_s32 min_bs_hy_thres2;
    hi_s32 max_bs_hy_thres;

    hi_s32 blk_detnum_lum_h;
    hi_s32 blk_detnum_lum_v;
    hi_s32 fill_border_num_hy;
    hi_s32 fill_border_num_vy;
    hi_s32 blksize_lum_h;
    hi_s32 blksize_lum_v;
    hi_u8 bord_str_hy;

    hi_s32 glb_motion;
    hi_s32 input_port;
} db_input_info;

typedef struct {
    hi_s32 db_en;
    hi_s32 db_lumhor_en;
    hi_s32 db_lumver_en;

    hi_s32 dir_smooth_mode;
    hi_s32 grad_sub_ratio;
    hi_s32 db_ctrst_thresh;
    hi_s32 lum_hor_scale_ratio;
    hi_s32 lum_hor_filter_sel;
    hi_s32 lum_hor_txt_winsize;
    hi_s32 max_lum_hor_db_dist;
    hi_s32 lum_h_blknum;
    hi_s32 lum_hor_adj_gain;
    hi_s32 lum_hor_hf_var_core;
    hi_s32 lum_hor_hf_var_gain1;
    hi_s32 lum_hor_hf_var_gain2;
    hi_s32 lum_hor_bord_adj_gain;
    hi_s32 lum_hor_hf_diff_core;
    hi_s32 lum_hor_hf_diff_gain1;
    hi_s32 lum_hor_hf_diff_gain2;
    hi_s32 ctrst_adj_core;
    hi_s32 ctrst_adj_gain1;
    hi_s32 ctrst_adj_gain2;
    hi_s32 lum_h_str_fade_lut[12]; /* array index 12 */
    hi_s32 lum_hor_delta_lut[17]; /* array index 17 */

    hi_s32 lum_v_blknum;
    hi_s32 lum_ver_scale_ratio;
    hi_s32 lum_ver_txt_winsize;
    hi_s32 lum_v_str_fade_lut[12]; /* array index 12 */
    hi_s32 lum_ver_delta_lut[17]; /* array index 17 */
    hi_s32 lum_ver_adj_gain;
    hi_s32 lum_ver_hf_var_core;
    hi_s32 lum_ver_hf_var_gain1;
    hi_s32 lum_ver_hf_var_gain2;
    hi_s32 lum_ver_bord_adj_gain;

    hi_s32 db_adptflt_mode_final_sel;

    hi_s32 blksize_lum_h;
    hi_s32 blksize_lum_v;
} db_output_info;

typedef struct {
    hi_s32 width;
    hi_s32 height;
    hi_s32 bit_depth;
    hi_s32 handle;
    dbd_input_info db_det_input_info;
    db_input_info  db_filter_input_info;
    hi_bool is_db_ready;
} hi_drv_pq_db_input_info;

typedef struct {
    dbd_output_info db_det_output_info;
    db_output_info  db_filter_output_info;
    hi_bool         is_db_output_ready;
} hi_drv_pq_db_output_info;

/* ******************************dm soft alg struct end *********************************************** */
#define RGSIZEW_SD 64
#define RGSIZEH_SD 32
#define RGSIZEW_FHD 1920
#define RGSIZEH_FHD 1080
#define RG_SIZE_H 64
#define RGSIZEH 16
#define DM_MAX_WIDTH 4096
#define DM_MAX_HEIGHT 2160
#define DM_ROLLSUB_MAXNUM 30
#define MAXLINESIZE 4096
#define RATE_BUF_LEN 32
#define DM_PARA_LUT_LEN 4

/* dm limits & flag lut option i: */
typedef struct {
    hi_u16 opp_ang_ctrst_t;
    hi_u16 mn_dir_opp_ctrst_t;
    hi_u16 csw_trsnt_lt;
    hi_u16 csw_trsnt_lt_10bit;
    hi_u8  lsw_ratio;
    hi_u8  limit_lsw_ratio;

    hi_s16 mmf_lr;
    hi_s16 mmf_lr_10bit;
    hi_s16 mmf_sr;
    hi_s16 mmf_sr_10bit;

    hi_u8  init_val_step;
    hi_u8  mmf_set;
    hi_u8  mmf_limit_en;

    hi_u16 limit_t;
    hi_u16 limit_t10bit;
    hi_u8  lim_res_blend_str1;
    hi_u8  lim_res_blend_str2;
    hi_u8  dir_blend_str;

    hi_u16 lw_ctrst_t;
    hi_u16 lw_ctrst_t_10bit;
    hi_u16 csw_trsnt_st;
    hi_u16 csw_trsnt_st_10bit;

    hi_u16 *mmf_lim_transband;
} dm_para_info;

typedef struct {
    hi_u16 dbd_limit_lut[4]; /* 4: limit lut len */
    hi_u8  dbd_flag_lut[5]; /* 5: flag lut len */
} dm_limitflag_lut_info;

typedef struct {
    hi_s32 y_width;
    hi_s32 y_height;

    hi_s32 rate_buf[RATE_BUF_LEN];
    hi_s32 *p_new_rate;
    hi_s32 rate_smth_win;
    dm_limitflag_lut_info limit_flag_lut_sd;
    dm_para_info dm_para_sd[DM_PARA_LUT_LEN];
    /* dbd info */
    hi_s32 blkdet_num_lum_h;
    hi_s32 blksize_lum_h;
    /* tnr info */
    hi_s32 global_motion;

    hi_s16 rgmv_col_static[DM_MAX_HEIGHT / RGSIZEH_SD];
    hi_s16 rgmv_x_thd_dn;
    hi_s16 rgmv_x_thd_up;
    hi_s16 rgmv_y_thd_up;
    hi_s16 rgmv_mag_thd_dn;
    hi_s16 mv_uni_thd_dn;
    hi_s16 mid_thd_band_ratio;
    hi_s16 rosublike_row_thd_dn;
    hi_s16 uni_num_row_thd_dn;
    hi_s16 rgmv_uni_area_thd_up;
    hi_s16 rgmv_delta_thd_up_row;
    hi_s16 rosub_height_thd_up;
    hi_s16 rosub_height_thd_dn;
    hi_s16 rosub_width_thd_dn;
    hi_s16 rosublike_area_thd_up;
    hi_s16 rosub_row_chk_thd_dn;
} dm_base_info;

typedef struct {
    hi_s32 y_width;
    hi_s32 y_height;

    hi_u16 opp_ang_ctrst_t;
    hi_u16 mn_dir_opp_ctrst_t;
    hi_u16 csw_trsnt_lt;
    hi_u16 csw_trsnt_lt_10bit;
    hi_u8  lsw_ratio;
    hi_u8  limit_lsw_ratio;
    hi_s16 mmf_lr;
    hi_s16 mmf_lr_10bit;
    hi_s16 mmf_sr;
    hi_s16 mmf_sr_10bit;
    hi_u8  init_val_step;
    hi_u8  mmf_set;
    hi_u8  mmf_limit_en;
    hi_u16 limit_t;
    hi_u16 limit_t10bit;
    hi_u8  lim_res_blend_str1;
    hi_u8  lim_res_blend_str2;
    hi_u8  dir_blend_str;
    hi_u16 lw_ctrst_t;
    hi_u16 lw_ctrst_t_10bit;
    hi_u16 csw_trsnt_st;
    hi_u16 csw_trsnt_st_10bit;
    hi_u16 transband[31]; /* 31: transband len */
    hi_s32 dm_global_str;
    hi_s32 global_motion;

    /* RoSub Detect */
    hi_s16 rgmv_x_buf[(DM_MAX_HEIGHT / RGSIZEH_SD) * (DM_MAX_WIDTH / RGSIZEW_SD)];
    hi_s16 rgmv_y_buf[(DM_MAX_HEIGHT / RGSIZEH_SD) * (DM_MAX_WIDTH / RGSIZEW_SD)];
    hi_u16 rgmv_mag_buf[(DM_MAX_HEIGHT / RGSIZEH_SD) * (DM_MAX_WIDTH / RGSIZEW_SD)];
    hi_u16 rgmv_blknum_h;
    hi_u16 rgmv_blknum_w;

    hi_u32              handle;
    hi_bool             is_dm_ready;
} hi_drv_pq_dm_input_info;

typedef struct {
    hi_u16 px_top_lft;
    hi_u16 py_top_lft;
    hi_u16 px_bot_rgt;
    hi_u16 py_bot_rgt;
    hi_u16 motion_val; /* no use */
} dm_rosub_info;

typedef struct {
    hi_u16 opp_ang_ctrst_t;
    hi_u16 mn_dir_opp_ctrst_t;
    hi_u16 csw_trsnt_lt;
    hi_u16 csw_trsnt_lt10bit;
    hi_u8  lsw_ratio;
    hi_u8  limit_lsw_ratio;
    hi_s16 mmf_lr;
    hi_s16 mmf_lr10bit;
    hi_s16 mmf_sr;
    hi_s16 mmf_sr10bit;
    hi_u8  init_val_step;
    hi_u8  mmf_set;
    hi_u8  mmf_limit_en;
    hi_u16 limit_t;
    hi_u16 limit_t10bit;
    hi_u8  lim_res_blend_str1;
    hi_u8  lim_res_blend_str2;
    hi_u8  dir_blend_str;
    hi_u16 lwc_trst_t;
    hi_u16 lwc_trst_t10bit;
    hi_u16 csw_trsnt_st;
    hi_u16 csw_trsnt_st10bit;
    hi_u16 trans_band[31]; /* 31: transband len */
    hi_s32 dm_global_str;

    dm_rosub_info rosub_buf[DM_ROLLSUB_MAXNUM];
    hi_u16 size_of_rosub; /* size must be less than DM_ROLLSUB_MAXNUM */

    hi_bool is_dm_output_ready;
} hi_drv_pq_dm_output_reg;

/* ******************************soft alg struct end *********************************************** */
/* ******************************vpss thread struct start*********************************************** */
#define ALG_BUFFER_NUM 2

typedef struct {
    /* add db input here */
    int test;
    hi_u32                  handle;
    hi_bool                 is_db_ready;
} drv_pq_db_input_info;

/* input info of vpss alg */
typedef struct {
    hi_bool is_ready;
    hi_u32 handle;
    hi_u32 index;
    hi_drv_pq_nr_input_info nr_api_input;
    drv_pq_dei_input_info dei_api_input;
    hi_drv_pq_db_input_info db_api_input;
    hi_drv_pq_dm_input_info dm_api_input;
} hi_drv_pq_vpss_api_input;

typedef struct {
    /* add db output here */
    int test;
    hi_bool is_db_output_ready;
} hi_drv_pq_db_output_reg;

/* result of vpss alg */
typedef struct {
    hi_bool is_ready;
    hi_u32 handle;
    hi_u32 index;
    hi_drv_pq_nr_api_output_reg nr_api_out;
    hi_drv_pq_dei_api_output_reg dei_api_out;
    hi_drv_pq_db_output_info db_api_out;
    hi_drv_pq_dm_output_reg dm_api_out;
} hi_drv_pq_vpss_api_output;

/* vpss status info */
typedef struct {
    hi_u32 handle;
    hi_bool is_enable;
    hi_drv_pq_frame_fmt frame_fmt;
    /* todo:need to fill width,height,frame_rate */
    hi_u32 width;
    hi_u32 height;
    hi_u32 frame_rate;
    hi_s32 read_addr_fd[ALG_BUFFER_NUM];
    hi_s32 write_addr_fd[ALG_BUFFER_NUM];
} hi_drv_pq_vpss_thread_status;

/* vpss thread info */
typedef struct {
    hi_drv_pq_vpss_thread_status vpss_thread_status[VPSS_HANDLE_NUM];
    hi_bool is_stop;
} hi_drv_pq_vpss_thread_info;
/* ******************************vpss thread struct end *********************************************** */
#define _DRV_ALG_VDP_H_
#define VDP_ALG_BUFFER_NUM 2

typedef struct {
    int test;
    hi_bool is_hdr_ready;
} drv_pq_hdr_input_info;

typedef struct {
    int test;
    hi_bool is_dc_ready;
} drv_pq_dc_input_info;

/* input info of vdp alg */
typedef struct {
    hi_bool is_ready;
    hi_u32 index;
    drv_pq_hdr_input_info hdr_api_input;
    drv_pq_dc_input_info dc_api_input;
} hi_drv_pq_vdp_api_input;

typedef struct {
    int test;
    hi_bool is_hdr_output_ready;
} drv_pq_output_info;

typedef struct {
    int test;
    hi_bool is_dc_output_ready;
} drv_pq_dc_output_info;

typedef struct {
    hi_bool is_ready;
    hi_u32 index;
    drv_pq_output_info hdr_api_out;
    drv_pq_dc_output_info dc_api_out;
} hi_drv_pq_vdp_api_output;

/* vdp status info */
typedef struct {
    hi_bool is_enable;
    hi_drv_pq_frame_fmt frame_fmt;
    /* todo:need to fill width,height,frame_rate */
    hi_u32 width;
    hi_u32 height;
    hi_u32 frame_rate;
    hi_s32 read_addr_fd[VDP_ALG_BUFFER_NUM];
    hi_s32 write_addr_fd[VDP_ALG_BUFFER_NUM];
} hi_drv_pq_vdp_thread_status;

/* vdp thread info */
typedef struct {
    hi_drv_pq_vdp_thread_status vdp_thread_status;
    hi_bool is_stop;
} hi_drv_pq_vdp_thread_info;

typedef struct {
    int test;
} hi_pq_vdp_alg_input;
/* ******************************vdp thread struct end *********************************************** */
/* **************************************Common INFT******************************************* */
hi_s32 hi_drv_pq_init(hi_void);

hi_s32 hi_drv_pq_deinit(hi_void);

hi_s32 hi_drv_pq_suspend(hi_void *pdev);

hi_s32 hi_drv_pq_resume(hi_void *pdev);

/* ***************************************VDP INFT****************************************** */
/*
according input and output source, it will init vdp reg / alg enable status,demo en and demo mode;
ever frame will call it , and it should be called before hdr module to update histgram ;
 */
hi_s32 hi_drv_pq_update_vdp_alg(hi_drv_pq_display_id display_id, hi_drv_pq_vdp_info *vdp_info);

/*
V0 channel, update vdp alg stt information, include hdr and dci histgram
every finished frame will call it and used for next frame
 */
hi_s32 hi_drv_pq_update_vdp_stt_info(hi_void);

/* include csc ac and dc coef, update brightness, hue, contrast, saturation, wcg_temprature */
hi_s32 hi_drv_pq_get_vdp_csc_coef(hi_drv_pq_csc_layer layer_id, hi_drv_pq_csc_info csc_info,
                                  hi_drv_pq_csc_coef *csc_coef);

/* get zme strategy, include fir enable status, fir mode, medium fir enable status, offset, zme coef, draw mul, etc */
hi_s32 hi_drv_pq_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                      hi_drv_pq_zme_strategy_out *zme_out);

hi_s32 hi_drv_pq_get_vdp_zme_coef(hi_drv_pq_zme_coef_in zme_coef_in, hi_drv_pq_zme_coef_out *zme_coef_out);

/* according input and output type, get hdrc cfg and pq writer hdr coef to reg */
hi_s32 hi_drv_pq_set_vdp_hdr_cfg(hi_drv_pq_xdr_layer_id layer_id, hi_drv_pq_xdr_frame_info *xdr_frame_info);

/*
set use pq default param, all alg(but dei) will be closed, zme use copy mode and offset will be set to 0;
brightness, hue, contrast, saturation, wcg_temprature will be reset to 50;
 */
hi_s32 hi_drv_pq_set_default_param(hi_bool is_default);

/* update ai result from NPU, it should be updated every frame */
hi_s32 hi_drv_pq_update_vdp_ai_result(hi_drv_pq_display_id display_id, hi_drv_pq_ai_result *ai_result);

/***************************************VPSS INFT********************************************/
/*
according input and output source, it will init vdp reg alg enable status demo en and demo mode;
for upgrade performance, it can be called in init and scence changed;
 */
hi_s32 hi_drv_pq_init_vpss_alg(hi_drv_pq_vpss_scene_mode scence_mode, hi_drv_pq_vpss_info *vpss_src_info,
                               hi_void *vpss_reg);

/* ever frame will call it */
hi_s32 hi_drv_pq_update_vpss_alg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_vpss_cfg_info *vpss_cfg_info);

/* every finished frame will call it and used for next frame */
hi_s32 hi_drv_pq_update_vpss_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info);

/* get zme strategy, include fir enable status, fir mode, medium fir enable status, offset, zme coef, draw mul, etc */
hi_s32 hi_drv_pq_get_vpsszme_coef(hi_drv_pq_vpss_layer layer_id,
    hi_drv_pq_vpsszme_in *zme_in,
    hi_drv_pq_vpsszme_out *zme_out);
/* according input and output type, get hdrc cfg and writer hdr coef to vpss reg */
hi_s32 hi_drv_pq_set_vpss_hdr_cfg(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_xdr_frame_info *xdr_frame_info,
                                  hi_void *vpss_reg);

/* update ai result from NPU, it should be updated every frame */
hi_s32 hi_drv_pq_update_vpss_ai_result(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_ai_result *ai_result);

/* ***************************************GFX****************************************** */
/* include csc ac and dc coef, update brightness, hue, contrast, saturation, wcg_temprature */
hi_s32 hi_drv_pq_get_gfx_csc_coef(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_csc_info *gfx_csc_info,
                                  hi_drv_pq_gfx_csc_coef *gfx_csc_coef);

/* get zme strategy, include fir enable status, fir mode, medium fir enable status, offset, zme coef, etc */
hi_s32 hi_drv_pq_get_gfx_zme_strategy(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_zme_in *gfx_zme_in,
                                      hi_drv_pq_gfx_zme_out *gfx_zme_out);

/* according input and output type, get hdrc cfg and pq writer hdr coef to reg */
hi_s32 hi_drv_pq_set_gfx_hdr_cfg(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_hdr_info *gfx_hdr_info);

/* ***************************************HWC****************************************** */
hi_s32 hi_drv_pq_get_hwc_hdr_cfg(hi_drv_pq_hwc_layer hwc_layer, hi_drv_pq_hwc_hdr_info *hwc_hdr_info,
                                 hi_drv_pq_hwc_hdr_cfg *hwc_hdr_cfg);
/* ********************************UNF IOCTL*************************************** */
#define HIIOC_PQ_S_SD_BRIGHTNESS _IOW(HI_ID_PQ, 1, hi_u32)  /* set SD BRIGHTNESS level */
#define HIIOC_PQ_G_SD_BRIGHTNESS _IOWR(HI_ID_PQ, 2, hi_u32) /* get SD BRIGHTNESS level */

#define HIIOC_PQ_S_SD_CONTRAST _IOW(HI_ID_PQ, 3, hi_u32)  /* set SD CONTRAST level */
#define HIIOC_PQ_G_SD_CONTRAST _IOWR(HI_ID_PQ, 4, hi_u32) /* get SD CONTRAST level */

#define HIIOC_PQ_S_SD_SATURATION _IOW(HI_ID_PQ, 5, hi_u32)  /* set SD SATURATION level */
#define HIIOC_PQ_G_SD_SATURATION _IOWR(HI_ID_PQ, 6, hi_u32) /* get SD SATURATION level */

#define HIIOC_PQ_S_SD_HUE _IOW(HI_ID_PQ, 7, hi_u32)  /* set SD HUE level */
#define HIIOC_PQ_G_SD_HUE _IOWR(HI_ID_PQ, 8, hi_u32) /* get SD HUE level */

#define HIIOC_PQ_S_HD_BRIGHTNESS _IOW(HI_ID_PQ, 11, hi_u32)  /* set HD BRIGHTNESS level */
#define HIIOC_PQ_G_HD_BRIGHTNESS _IOWR(HI_ID_PQ, 12, hi_u32) /* get HD BRIGHTNESS level */

#define HIIOC_PQ_S_HD_CONTRAST _IOW(HI_ID_PQ, 13, hi_u32)  /* set HD CONTRAST level */
#define HIIOC_PQ_G_HD_CONTRAST _IOWR(HI_ID_PQ, 14, hi_u32) /* get HD CONTRAST level */

#define HIIOC_PQ_S_HD_SATURATION _IOW(HI_ID_PQ, 15, hi_u32)  /* set HD SATURATION level */
#define HIIOC_PQ_G_HD_SATURATION _IOWR(HI_ID_PQ, 16, hi_u32) /* get HD SATURATION level */

#define HIIOC_PQ_S_HD_HUE _IOW(HI_ID_PQ, 17, hi_u32)  /* set HD HUE level */
#define HIIOC_PQ_G_HD_HUE _IOWR(HI_ID_PQ, 18, hi_u32) /* get HD HUE level */

#define HIIOC_PQ_S_GRAPH_SD_PARAM _IOW(HI_ID_PQ, 19, hi_pq_image_param)  /* set gfx SD CSC para */
#define HIIOC_PQ_G_GRAPH_SD_PARAM _IOWR(HI_ID_PQ, 20, hi_pq_image_param) /* get gfx SD CSC para */

#define HIIOC_PQ_S_GRAPH_HD_PARAM _IOW(HI_ID_PQ, 21, hi_pq_image_param)  /* set gfx HD CSC para */
#define HIIOC_PQ_G_GRAPH_HD_PARAM _IOWR(HI_ID_PQ, 22, hi_pq_image_param) /* get gfx HD CSC para */

#define HIIOC_PQ_S_VIDEO_SD_PARAM _IOW(HI_ID_PQ, 23, hi_pq_image_param)  /* set video SD CSC para */
#define HIIOC_PQ_G_VIDEO_SD_PARAM _IOWR(HI_ID_PQ, 24, hi_pq_image_param) /* get video SD CSC para */

#define HIIOC_PQ_S_VIDEO_HD_PARAM _IOW(HI_ID_PQ, 25, hi_pq_image_param)  /* set video HD CSC para */
#define HIIOC_PQ_G_VIDEO_HD_PARAM _IOWR(HI_ID_PQ, 26, hi_pq_image_param) /* get video HD CSC para */

#define HIIOC_PQ_S_SD_TEMPERATURE _IOW(HI_ID_PQ, 27, hi_pq_color_temperature)  /* set SD color temperature */
#define HIIOC_PQ_G_SD_TEMPERATURE _IOWR(HI_ID_PQ, 28, hi_pq_color_temperature) /* get SD color temperature */

#define HIIOC_PQ_S_HD_TEMPERATURE _IOW(HI_ID_PQ, 29, hi_pq_color_temperature)  /* set HD color temperature */
#define HIIOC_PQ_G_HD_TEMPERATURE _IOWR(HI_ID_PQ, 30, hi_pq_color_temperature) /* get HD color temperature */

#define HIIOC_PQ_S_MODULE _IOW(HI_ID_PQ, 31, hi_pq_module_enable)  /* set alg module on/off */
#define HIIOC_PQ_G_MODULE _IOWR(HI_ID_PQ, 32, hi_pq_module_enable) /* get alg module on/off */

#define HIIOC_PQ_S_STRENGTH _IOWR(HI_ID_PQ, 35, hi_pq_module_strenght) /* set level */
#define HIIOC_PQ_G_STRENGTH _IOWR(HI_ID_PQ, 36, hi_pq_module_strenght) /* get level */

#define HIIOC_PQ_G_DEMO _IOW(HI_ID_PQ, 44, hi_pq_demo_enable) /* set alg module demo on/off */
#define HIIOC_PQ_S_DEMO _IOW(HI_ID_PQ, 45, hi_pq_demo_enable) /* set alg module demo on/off */

#define HIIOC_PQ_S_DEMO_MODE _IOW(HI_ID_PQ, 46, hi_pq_demo_mode)  /* set alg module demo mode */
#define HIIOC_PQ_G_DEMO_MODE _IOWR(HI_ID_PQ, 47, hi_pq_demo_mode) /* get alg module demo mode */

#define HIIOC_PQ_S_DEMO_COORDINATE _IOW(HI_ID_PQ, 48, hi_u32)  /* set demo coordinate */
#define HIIOC_PQ_G_DEMO_COORDINATE _IOWR(HI_ID_PQ, 49, hi_u32) /* get demo coordinate */

#define HIIOC_PQ_S_IMAGE_MODE _IOW(HI_ID_PQ, 50, hi_pq_image_mode)  /* set VideoPhone mode */
#define HIIOC_PQ_G_IMAGE_MODE _IOWR(HI_ID_PQ, 51, hi_pq_image_mode) /* get VideoPhone mode */

#define HIIOC_PQ_S_FLESHTONE _IOW(HI_ID_PQ, 52, hi_pq_fleshtone)  /* set FleshTone level */
#define HIIOC_PQ_G_FLESHTONE _IOWR(HI_ID_PQ, 53, hi_pq_fleshtone) /* get FleshTone level */

#define HIIOC_PQ_S_SIXBASECOLOR _IOW(HI_ID_PQ, 54, hi_pq_six_base_color)  /* set acm six base color para */
#define HIIOC_PQ_G_SIXBASECOLOR _IOWR(HI_ID_PQ, 55, hi_pq_six_base_color) /* set acm six base color para */

#define HIIOC_PQ_S_COLOR_ENHANCE_MODE _IOW(HI_ID_PQ, 56, hi_pq_color_spec_mode)  /* set ACM Color Gain Mode */
#define HIIOC_PQ_G_COLOR_ENHANCE_MODE _IOWR(HI_ID_PQ, 57, hi_pq_color_spec_mode) /* get ACM Color Gain Mode */

#define HIIOC_PQ_S_DEFAULT_PARAM _IOW(HI_ID_PQ, 58, hi_bool) /* set PQ para default, used when rwzb test */

/* ************************ONLY MPI USED IOCTRL********************************** */
#define HIIOC_PQ_S_GFX_HDROFFSET_PARAM _IOW(HI_ID_PQ, 100, hi_pq_hdr_offset) /* set GFX HDR Offset Para */
#define HIIOC_PQ_S_HDROFFSET_PARAM _IOW(HI_ID_PQ, 101, hi_pq_hdr_offset)     /* set HDR Offset Para */

#define HIIOC_PQ_S_VP_PREVIEW_IMAGE_MODE _IOW(HI_ID_PQ, 102, hi_drv_pq_vp_mode)  /* set VideoPhone Preview mode */
#define HIIOC_PQ_G_VP_PREVIEW_IMAGE_MODE _IOWR(HI_ID_PQ, 103, hi_drv_pq_vp_mode) /* get VideoPhone Preview mode */

#define HIIOC_PQ_S_VP_REMOTE_IMAGE_MODE _IOW(HI_ID_PQ, 104, hi_drv_pq_vp_mode)  /* set VideoPhone Preview mode */
#define HIIOC_PQ_G_VP_REMOTE_IMAGE_MODE _IOWR(HI_ID_PQ, 105, hi_drv_pq_vp_mode) /* get VideoPhone Preview mode */
/* ************************ONLY PQ TOOL USED DIRECT********************************** */
#define HIIOC_PQ_S_REGISTER _IOW(HI_ID_PQ, 152, hi_pq_register)  /* write regitster */
#define HIIOC_PQ_G_REGISTER _IOWR(HI_ID_PQ, 153, hi_pq_register) /* read register */

#define HIIOC_PQ_S_DCI _IOW(HI_ID_PQ, 154, hi_pq_dci_wgt)  /* set DCI clut */
#define HIIOC_PQ_G_DCI _IOWR(HI_ID_PQ, 155, hi_pq_dci_wgt) /* get DCI clut */

#define HIIOC_PQ_G_BIN_ADDR _IOWR(HI_ID_PQ, 156, hi_u32) /* get PQBin Phy Addr */

#define HIIOC_PQ_S_DCI_BS _IOW(HI_ID_PQ, 157, hi_pq_dci_bs_lut)  /* set DCI BS clut */
#define HIIOC_PQ_G_DCI_BS _IOWR(HI_ID_PQ, 158, hi_pq_dci_bs_lut) /* get DCI BS clut */

#define HIIOC_PQ_S_SNR_MEAN_RATIO _IOW(HI_ID_PQ, 161, hi_pq_snr_mean_ratio)  /* set SNR MEAN RATIO clut */
#define HIIOC_PQ_G_SNR_MEAN_RATIO _IOWR(HI_ID_PQ, 162, hi_pq_snr_mean_ratio) /* get SNR MEAN RATIO clut */

#define HIIOC_PQ_S_SNR_EDGE_STR _IOW(HI_ID_PQ, 163, hi_pq_snr_edge_str)  /* set SNR  EDGE STR clut */
#define HIIOC_PQ_G_SNR_EDGE_STR _IOWR(HI_ID_PQ, 164, hi_pq_snr_edge_str) /* get SNR EDGE STR clut */

#define HIIOC_PQ_S_GAMM_PARA _IOW(HI_ID_PQ, 165, hi_pq_csc_gamm_para)
#define HIIOC_PQ_G_GAMM_PARA _IOWR(HI_ID_PQ, 166, hi_pq_csc_gamm_para)

#define HIIOC_PQ_S_DEGAMM_PARA _IOW(HI_ID_PQ, 167, hi_pq_csc_gamm_para)
#define HIIOC_PQ_G_DEGAMM_PARA _IOWR(HI_ID_PQ, 168, hi_pq_csc_gamm_para)

#define HIIOC_PQ_S_GFX_GAMM_PARA _IOW(HI_ID_PQ, 169, hi_pq_csc_gamm_para)
#define HIIOC_PQ_G_GFX_GAMM_PARA _IOWR(HI_ID_PQ, 170, hi_pq_csc_gamm_para)

#define HIIOC_PQ_S_GFX_DEGAMM_PARA _IOW(HI_ID_PQ, 171, hi_pq_csc_gamm_para)
#define HIIOC_PQ_G_GFX_DEGAMM_PARA _IOWR(HI_ID_PQ, 172, hi_pq_csc_gamm_para)

#define HIIOC_PQ_S_HDR_TMLUT _IOW(HI_ID_PQ, 173, hi_pq_hdr_tm_lut)  /* set HDR TM Lut */
#define HIIOC_PQ_G_HDR_TMLUT _IOWR(HI_ID_PQ, 174, hi_pq_hdr_tm_lut) /* get HDR TM Lut */

#define HIIOC_PQ_S_HDR_PARA_MODE _IOW(HI_ID_PQ, 175, hi_pq_hdr_para_mode)  /* set HDR Lwavg */
#define HIIOC_PQ_G_HDR_PARA_MODE _IOWR(HI_ID_PQ, 176, hi_pq_hdr_para_mode) /* get HDR Lwavg */

#define HIIOC_PQ_S_HDR_TMAP _IOW(HI_ID_PQ, 177, hi_pq_hdr_tmap)  /* set HDR Tmap */
#define HIIOC_PQ_G_HDR_TMAP _IOWR(HI_ID_PQ, 178, hi_pq_hdr_tmap) /* get HDR Tmap */
#define HIIOC_PQ_S_HDR_SMAP _IOW(HI_ID_PQ, 179, hi_pq_hdr_smap)  /* set HDR Smap */
#define HIIOC_PQ_G_HDR_SMAP _IOWR(HI_ID_PQ, 180, hi_pq_hdr_smap) /* get HDR Smap */

#define HIIOC_PQ_S_HDR_ACM_MODE _IOW(HI_ID_PQ, 181, hi_pq_hdr_acm_mode)      /* set Hdr Acm Mode */
#define HIIOC_PQ_G_HDR_ACM_MODE _IOWR(HI_ID_PQ, 182, hi_pq_hdr_acm_mode)     /* get Hdr Acm Mode */
#define HIIOC_PQ_S_HDR_ACM_REGCFG _IOW(HI_ID_PQ, 183, hi_pq_hdr_acm_regcfg)  /* set Hdr Acm RegCfg */
#define HIIOC_PQ_G_HDR_ACM_REGCFG _IOWR(HI_ID_PQ, 184, hi_pq_hdr_acm_regcfg) /* get Hdr Acm RegCfg */

#define HIIOC_PQ_S_HDR_DYN_MODE _IOW(HI_ID_PQ, 185, hi_pq_hdr_dyn_tm_tuning)  /* set Hdr Dyn Cfg */
#define HIIOC_PQ_G_HDR_DYN_MODE _IOWR(HI_ID_PQ, 186, hi_pq_hdr_dyn_tm_tuning) /* get Hdr Dyn Cfg */

#define HIIOC_PQ_S_ACM_GAIN _IOW(HI_ID_PQ, 193, hi_pq_color_gain)  /* set COLOR GAIN */
#define HIIOC_PQ_G_ACM_GAIN _IOWR(HI_ID_PQ, 194, hi_pq_color_gain) /* get COLOR GAIN */

#define HIIOC_PQ_S_ACM_LUMA _IOW(HI_ID_PQ, 195, hi_pq_acm_lut)  /* set COLOR LUMA clut */
#define HIIOC_PQ_G_ACM_LUMA _IOWR(HI_ID_PQ, 196, hi_pq_acm_lut) /* get COLOR LUMA clut */

#define HIIOC_PQ_S_ACM_HUE _IOW(HI_ID_PQ, 197, hi_pq_acm_lut)  /* set COLOR HUE clut */
#define HIIOC_PQ_G_ACM_HUE _IOWR(HI_ID_PQ, 198, hi_pq_acm_lut) /* get COLOR HUE clut */

#define HIIOC_PQ_S_ACM_SAT _IOW(HI_ID_PQ, 199, hi_pq_acm_lut)  /* set COLOR SAT clut */
#define HIIOC_PQ_G_ACM_SAT _IOWR(HI_ID_PQ, 200, hi_pq_acm_lut) /* get COLOR SAT clut */

#define HIIOC_PQ_G_DCI_HIST _IOWR(HI_ID_PQ, 201, hi_pq_dci_histgram) /* get DCI Hist */

#define HIIOC_PQ_G_VPSS_INFO _IOR(HI_ID_PQ, 202, hi_drv_pq_vpss_thread_info) /* get VPSS info */
#define HIIOC_PQ_G_APITHREAD_STATUS _IOR(HI_ID_PQ, 203, hi_bool)             /* get APITHREAD status */
#define HIIOC_PQ_S_APITHREAD_STATUS _IOW(HI_ID_PQ, 204, hi_bool)             /* set APITHREAD status */

#define HIIOC_PQ_G_VDP_INFO _IOR(HI_ID_PQ, 205, hi_drv_pq_vdp_thread_info) /* get VPSS info */
#define HIIOC_PQ_G_VDP_APITHREAD_STATUS _IOR(HI_ID_PQ, 206, hi_bool)       /* get APITHREAD status */
#define HIIOC_PQ_S_VDP_APITHREAD_STATUS _IOW(HI_ID_PQ, 207, hi_bool)       /* set APITHREAD status */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DRV_PQ_V1_H__ */

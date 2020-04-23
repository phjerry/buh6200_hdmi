/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_ice_define.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __ICE_DEFINE_H__
#define __ICE_DEFINE_H__

#include "hi_type.h"

typedef enum {
    ICE_BIT_DEPTH_8 = 0,
    ICE_BIT_DEPTH_10 = 1,
    ICE_BIT_DEPTH_12 = 2,
    ICE_BIT_DEPTH_14 = 3,
    ICE_BIT_DEPTH_MAX
} ice_bit_depth;

typedef enum {
    ICE_REG_CFG_MODE_TYP = 0,
    /* ICE_REG_CFG_MODE_MIN    = 1 ,
    ICE_REG_CFG_MODE_MAX    = 2 ,
    ICE_REG_CFG_MODE_RAND   = 3 , */
    ICE_REG_CFG_MODE_MINMAX = 4,
    ICE_REG_CFG_MODE_MANUAL = 5,
    ICE_REG_CFG_MODE_MAX
} ice_reg_cfg_mode;

typedef enum {
    ICE_DATA_FMT_YUV422 = 0,
    ICE_DATA_FMT_YUV420 = 1,
    ICE_DATA_FMT_YUV444 = 2,
    ICE_DATA_FMT_YUV400 = 3,
    ICE_DATA_FMT_MAX
} ice_data_fmt;

typedef enum {
    ICE_DATA_TYPE_ARGB8888 = 0,
    ICE_DATA_TYPE_RGB888 = 1,
    ICE_DATA_TYPE_MAX
} ice_data_type;
typedef enum {
    ICE_BAYER_FMT_GRBG = 0,
    ICE_BAYER_FMT_GBRG = 1,
    ICE_BAYER_FMT_RGGB = 2,
    ICE_BAYER_FMT_BGGR = 3,
    ICE_BAYER_FMT_MAX
} ice_bayer_fmt;

typedef enum {
    ICE_ERROR_TYPE_NORMAL = 0,
    ICE_ERROR_TYPE_HEAD_ZERO,
    ICE_ERROR_TYPE_HEAD_RAND,
    ICE_ERROR_TYPE_HEAD_MAX,
    ICE_ERROR_TYPE_DATA_ZERO,
    ICE_ERROR_TYPE_DATA_RAND,
    ICE_ERROR_TYPE_DATA_MAX,
    ICE_ERROR_TYPE_FRAMESIZE_MAX,
    ICE_ERROR_TYPE_FRAMESIZE_RAND,
    ICE_ERROR_TYPE_FRAMESIZE_ZERO,
    ICE_ERROR_TYPE_MAX
} ice_error_type;

typedef struct {
    hi_bool en;
    hi_u32 start_pos;
    hi_u32 end_pos;
    hi_u32 start_pos_c;
    hi_u32 end_pos_c;
} ice_frm_part_mode;
typedef struct {
    hi_bool en;
    hi_u32 index;
} ice_frm_slice_mode;

typedef struct {
    /* input */
    hi_u32 layer;
    hi_u32 frame_wth;
    hi_u32 frame_hgt;
    hi_u32 tile_hgt;
    hi_u32 cmp_ratio;
    hi_bool is_lossy;
    hi_bool chm_proc_en;
    hi_bool esl_en;
    ice_bit_depth bit_depth;
    ice_data_fmt data_fmt;
    ice_bayer_fmt bayer_fmt;
    ice_reg_cfg_mode cmp_cfg_mode;
    /* file name */
    char **cmp_file_name;
    /* output */
    hi_u32 *cmp_size;
    /* debug */
    ice_error_type err_type;
} cmp_seg_info;

typedef enum {
    OSD_CMP_MODE_TYP = 0,
    OSD_CMP_MODE_RAND,
    OSD_CMP_MODE_MAX
} osd_cmp_mode;

typedef struct {
    /* input */
    hi_u32 layer;
    osd_cmp_mode cmp_cfg_mode;
    hi_u32 cmp_ratio;
    hi_u32 frm_cnt;
    hi_u32 node_cnt;
    /* file name */
    char *cmp_info;
    char **cmp_file_name;
    char **reg_cfg_file_name;
    /* output */
    hi_u32 *cmp_size;
    /* debug */
    ice_error_type err_type;
} cmp_frm_info;

typedef struct {
    hi_u32 layer;
    hi_u32 frm_cnt;
    hi_u32 node_cnt;
    hi_u32 frame_wth;
    hi_u32 frame_hgt;
    hi_u32 tile_hgt;
    hi_u32 cmp_ratio_y;
    hi_u32 cmp_ratio_c;
    hi_bool cmp_en;
    hi_bool is_raw_en;
    hi_bool is_lossy_y;
    hi_bool is_lossy_c;
    hi_bool chm_proc_en;
    hi_bool esl_en;
    ice_bit_depth bit_depth;
    ice_data_fmt data_fmt;
    ice_bayer_fmt bayer_fmt;
    ice_reg_cfg_mode cmp_cfg_mode;
    ice_frm_part_mode part_mode_cfg;
    ice_frm_slice_mode slice_mode_cfg;
    /* file name */
    char **cmp_file_name;
    /* output */
    hi_u32 *cmp_size;
    /* debug */
    ice_error_type err_type;
} ice_frm_cfg;

typedef struct {
    hi_u32 layer;
    hi_u32 frame_wth;
    hi_u32 frame_hgt;
    hi_u32 tile_hgt;
    hi_u32 cmp_ratio_y;
    hi_u32 cmp_ratio_c;
    hi_bool cmp_en;
    hi_bool is_raw_en;
    hi_bool is_lossy_y;
    hi_bool is_lossy_c;
    hi_bool chm_proc_en;
    hi_bool esl_en;
    hi_bool csc_en;
    ice_bit_depth bit_depth;
    ice_data_fmt data_fmt;
    ice_data_type data_type;
    ice_bayer_fmt bayer_fmt;
    ice_reg_cfg_mode cmp_cfg_mode;
    ice_frm_part_mode part_mode_cfg;
    char *cmp_info;
    char **cmp_file_name;
    hi_u32 *cmp_size;
    ice_error_type err_type;
} ice_seg_cfg;

typedef struct {
    unsigned int chroma_en;
    unsigned int is_lossless;
    unsigned int cmp_mode;
    unsigned int depth;
    unsigned int esl_qp;
    unsigned int frame_width;
    unsigned int frame_height;
    unsigned int rm_debug_en;
    unsigned int dcmp_error;

    /* rc parameter */
    unsigned int big_grad_thr;
    unsigned int diff_thr;
    unsigned int noise_pix_num_thr;
    unsigned int qp_inc1_bits_thr;
    unsigned int qp_inc2_bits_thr;
    unsigned int qp_dec1_bits_thr;
    unsigned int qp_dec2_bits_thr;
    unsigned int buf_fullness_thr_reg0;
    unsigned int buf_fullness_thr_reg1;
    unsigned int buf_fullness_thr_reg2;
    unsigned int qp_rge_reg0;
    unsigned int qp_rge_reg1;
    unsigned int qp_rge_reg2;
    unsigned int bits_offset_reg0;
    unsigned int bits_offset_reg1;
    unsigned int bits_offset_reg2;
    unsigned int est_err_gain_map;
    unsigned int buffer_size;
    unsigned int buffer_init_bits;
    unsigned int smooth_status_thr;
    unsigned int budget_mb_bits_last;
    unsigned int min_mb_bits;
    int max_mb_qp;
    int budget_mb_bits;

    /* reserved parameter */
    unsigned int reserve_para0;
    unsigned int reserve_para1;

    /* add parameter */
    unsigned char smooth_deltabits_thr; /* used to determine the number of bits of smooth_delta */
    unsigned int adpqp_thr0;            /* adpqp_thr0,1,2,3 */
    unsigned int adpqp_thr1;            /* adpQp_thr4,5, and adpQP clip_thr */
} ice_v3_r2_line_cfg;

#endif


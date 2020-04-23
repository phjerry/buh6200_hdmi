/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_FDR_H__
#define __HAL_VDP_IP_FDR_H__

#include "vdp_chip_define.h"

#define FDR_REGION_OFFSET  (0x30 / 4)

typedef enum {
    VDP_FDR_DISP_MODE_2D = 0,
    VDP_FDR_DISP_MODE_SBS = 1,
    VDP_FDR_DISP_MODE_LBL = 2,
    VDP_FDR_DISP_MODE_FS = 3,
    VDP_FDR_DISP_MODE_TAB = 4,
    VDP_FDR_DISP_MODE_FP = 5,
    VDP_FDR_DISP_MODE_BUTT
} vdp_fdr_disp_mode;

typedef enum {
    VDP_FDR_RMODE_3D_INTF = 0,
    VDP_FDR_RMODE_3D_SRC = 1,
    VDP_FDR_RMODE_3D_BUTT
} vdp_fdr_rmode_3d;

typedef enum {
    VDP_FDR_RMODE_INTERFACE = 0,
    VDP_FDR_RMODE_PROGRESSIVE = 1,
    VDP_FDR_RMODE_TOP = 2,
    VDP_FDR_RMODE_BOTTOM = 3,
    VDP_FDR_RMODE_PRO_TOP = 4,
    VDP_FDR_RMODE_PRO_BOTTOM = 5,
    VDP_FDR_RMODE_BUTT
} vdp_fdr_data_rmode;

typedef enum {
    VDP_FDR_DRAW_MODE_1 = 0,
    VDP_FDR_DRAW_MODE_2 = 1,
    VDP_FDR_DRAW_MODE_4 = 2,
    VDP_FDR_DRAW_MODE_8 = 3,
    VDP_FDR_DRAW_MODE_16 = 16,
    VDP_FDR_DRAW_MODE_32 = 32,
    VDP_FDR_DRAW_MODE_BUTT
} vdp_fdr_draw_mode;

typedef enum {
    VDP_FDR_REQ_LENGTH_16 = 0,
    VDP_FDR_REQ_LENGTH_8 = 1,
    VDP_FDR_REQ_LENGTH_BUTT
} vdp_fdr_req_length;

typedef enum {
    VDP_FDR_REQ_CTRL_16BURST_1,
    VDP_FDR_REQ_CTRL_16BURST_2,
    VDP_FDR_REQ_CTRL_16BURST_4,
    VDP_FDR_REQ_CTRL_16BURST_8,
    VDP_FDR_REQ_CTRL_BUTT
} vdp_fdr_req_ctrl;

typedef enum {
    VDP_FDR_VID_DATA_TYPE_RESERVE = 0x0, /* reserve: 422 */
    VDP_FDR_VID_DATA_TYPE_YUV400,
    VDP_FDR_VID_DATA_TYPE_YUV420,
    VDP_FDR_VID_DATA_TYPE_YUV422,
    VDP_FDR_VID_DATA_TYPE_YUV444,
    VDP_FDR_VID_DATA_TYPE_RGB888,
    VDP_FDR_VID_DATA_TYPE_ARGB8888,
    VDP_FDR_VID_DATA_TYPE_BUTT,
} vdp_fdr_vid_data_type;

typedef enum {
    VDP_FDR_VID_IFMT_SP_400 = 0x1,
    VDP_FDR_VID_IFMT_SP_420 = 0x3,
    VDP_FDR_VID_IFMT_SP_422 = 0x4,
    VDP_FDR_VID_IFMT_SP_444 = 0x5,
    VDP_FDR_VID_IFMT_SP_TILE = 0x6,
    VDP_FDR_VID_IFMT_SP_TILE_64 = 0x7,
    VDP_FDR_VID_IFMT_PKG_UYVY = 0x9,
    VDP_FDR_VID_IFMT_PKG_YUYV = 0xa,
    VDP_FDR_VID_IFMT_PKG_YVYU = 0xb,
    VDP_FDR_VID_IFMT_PKG_12 = 0xc,
    VDP_FDR_VID_IFMT_RGB_888 = 0x8,
    VDP_FDR_VID_IFMT_PKG_YUV444 = 0xe,
    VDP_FDR_VID_IFMT_ARGB_8888 = 0xf,
    VDP_FDR_VID_IFMT_AFBC_8888 = 0x10,
    VDP_FDR_VID_IFMT_AFBC_888 = 0x11,
    VDP_FDR_VID_IFMT_AFBC_2101010 = 0x12,
    VDP_FDR_VID_IFMT_BUTT
} vdp_fdr_vid_in_fmt;

typedef enum {
    VDP_FDR_VID_DATA_FMT_LINEAR = 0x0,
    VDP_FDR_VID_DATA_FMT_TILE,
    VDP_FDR_VID_DATA_FMT_PKG,
    VDP_FDR_VID_DATA_FMT_AFBC,
    VDP_FDR_VID_DATA_FMT_BUTT,
} vdp_fdr_vid_data_fmt;

/* dcmp */
typedef enum {
    VDP_FDR_DCMP_TYPE_OFF = 0,
    VDP_FDR_DCMP_TYPE_SEG,
    VDP_FDR_DCMP_TYPE_LINE,
    VDP_FDR_DCMP_TYPE_FRM,
    VDP_FDR_DCMP_TYPE_BUTT
} vdp_fdr_dcmp_type;

typedef struct {
    hi_bool enable;
    hi_u32 start_pos;
    hi_u32 end_pos;
    hi_u32 start_pos_c;
    hi_u32 end_pos_c;
} vdp_fdr_frm_part_mode;

typedef struct {
    hi_bool en;
    hi_u32 index;
} vdp_fdr_frm_slice_mode;

typedef struct {
    hi_bool enable;
    hi_bool lossless_y;
    hi_bool lossless_c;
    hi_u32 cmp_ratio_y;
    hi_u32 cmp_ratio_c;
} vdp_fdr_dcmp_line_cfg;

typedef struct {
    hi_bool dcmp_en;
    hi_bool is_raw_en;
    hi_bool csc_en;
    hi_bool lossless_y;
    hi_u32 cmp_ratio_y;
} vdp_fdr_dcmp_seg_cfg;

typedef struct {
    hi_bool dcmp_en;
    hi_bool is_raw_en;
    hi_bool lossless_y;
    hi_bool lossless_c;
    hi_u32 cmp_ratio_y;
    hi_u32 cmp_ratio_c;
    vdp_fdr_frm_part_mode part_mode_cfg;
    vdp_fdr_frm_slice_mode slice_mode_cfg;
} vdp_fdr_dcmp_frm_cfg;

/* tunl */
typedef enum {
    VDP_FDR_LOWDLY_MODE_OFF = 0,
    VDP_FDR_LOWDLY_MODE_TUNL,
    VDP_FDR_LOWDLY_MODE_VDH,
    VDP_FDR_LOWDLY_MODE_BUTT
} vdp_fdr_lowdly_mode;

typedef struct {
    hi_bool tunl_en;
    vdp_fdr_lowdly_mode req_ld_mode;
    hi_u64 tunl_addr;
    hi_u32 tunl_interval;
} vdp_fdr_tunl_info;


/* multi-region */
typedef struct {
    /* ctrl */
    hi_bool enable;
    hi_bool mute_en;
    hi_bool crop_en;
    hi_bool mmu_bypass; /* lm + chm */
    hi_bool dcmp_en;

    /* rect */
    vdp_rect in_rect;
    vdp_rect disp_rect;

    /* addr */
    vdp_vid_addr addr[VDP_FDR_MRG_ADDR_BUTT];
} vdp_fdr_mgr_info;

/* dcmp coef */
typedef struct{
    hi_u32 chroma_en;
    hi_u32 is_lossless;
    hi_u32 cmp_mode;
    hi_u32 bit_depth;
    hi_u32 esl_qp;
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 rm_debug_en;

    /* rc parameter */
    hi_u32 big_grad_thr;
    hi_u32 diff_thr;
    hi_u32 noise_pix_num_thr;
    hi_u32 qp_inc1_bits_thr;
    hi_u32 qp_inc2_bits_thr;
    hi_u32 qp_dec1_bits_thr;
    hi_u32 qp_dec2_bits_thr;
    hi_u32 buf_fullness_thr_reg0;
    hi_u32 buf_fullness_thr_reg1;
    hi_u32 buf_fullness_thr_reg2;
    hi_u32 qp_rge_reg0;
    hi_u32 qp_rge_reg1;
    hi_u32 qp_rge_reg2;
    hi_u32 bits_offset_reg0;
    hi_u32 bits_offset_reg1;
    hi_u32 bits_offset_reg2;
    hi_u32 est_err_gain_map;
    hi_u32 buffer_size;
    hi_u32 buffer_init_bits;
    hi_u32 smooth_status_thr;
    hi_u32 budget_mb_bits_last;
    hi_u32 min_mb_bits;
    hi_s32 max_mb_qp;
    hi_s32 budget_mb_bits;

    /* reserved parameter */
    hi_u32 reserve_para0;
    hi_u32 reserve_para1;

    /* add parameter */
    hi_u8 smooth_deltabits_thr; /* used to determine the number of bits of smooth_delta */
    hi_u32 adpqp_thr0; /* adpqp_thr 0,1,2,3 */
    hi_u32 adpqp_thr1; /* adpQp_thr 4,5, and adpQP clip_thr */
} vdp_dcmp_line_input;

typedef struct {
    /* mac */
    hi_bool pre_rd_en;
    hi_bool ofl_master; /* DPT 810 use */
    vdp_fdr_req_length req_len;
    vdp_fdr_req_ctrl req_ctrl;
} vdp_fdr_init_info;

typedef struct {
    /* ctrl info */
    hi_bool flip_en; /* DPT use */
    hi_bool chm_copy_en;
    vdp_fdr_rmode_3d rmode_3d;
    vdp_fdr_data_rmode rmode;          /* 逐隔行读取模式 */
    vdp_fdr_draw_mode draw_mode;       /* 抽行 */
    vdp_fdr_draw_mode draw_pixel_mode; /* 抽点 */
    hi_bool smmu_en;                   /* smmu bypass */
    hi_bool secure_en;

    /* src info */
    vdp_data_wth data_width;
    vdp_fdr_vid_data_type data_type;
    vdp_fdr_vid_data_fmt data_fmt;
    vdp_fdr_vid_in_fmt in_fmt;
    vdp_fdr_disp_mode src_disp_mode; /* cv300 not use, 430 use */
    hi_bool uv_order;

    vdp_rect in_rect;
    vdp_rect src_rect;

    /* mute */
    hi_bool mute_en;
    hi_bool mute_req_en;
    vdp_bkg mute_bkg; /* R,G,B */

    /* dcmp */
    vdp_fdr_dcmp_type dcmp_type; /* 压缩类型 */
    vdp_fdr_dcmp_line_cfg dcmp_line_cfg;
    vdp_fdr_dcmp_seg_cfg dcmp_seg_cfg;
    vdp_fdr_dcmp_frm_cfg dcmp_frm_cfg; /* cv300 not use */

    /* tunl */
    vdp_fdr_tunl_info tunl_info; /* cv300 not use */

    /* v1 multi-region */
    hi_bool mrg_mode_en;
    hi_u32 mrg_total_num;
    vdp_fdr_mgr_info mrg_info[FDR_REGION_NUM_MAX];

    /* test */
#ifdef VDP_CBB_TEST_SUPPORT
    hi_void *cbb_test;
#endif
} vdp_fdr_info;

typedef struct {
    vdp_vid_addr addr[VDP_FDR_VID_ADDR_BUTT];
} vdp_fdr_addr;

hi_void vdp_ip_fdr_mac_init(hi_u32 layer, vdp_fdr_init_info *fdr_init_info);
hi_void vdp_ip_fdr_set(hi_u32 layer, vdp_fdr_info *fdr_info);
hi_void vdp_ip_fdr_addr(hi_u32 layer, vdp_fdr_addr *fdr_addr, vdp_fdr_info *fdr_info);
hi_void vdp_ip_fdr_testpattern(hi_u32 layer, vdp_fdr_testpattern *testpattern_info);
hi_s32 vdp_ip_fdr_get_dcmp_error(vdp_dispchn_chn disp_id);
hi_void vdp_ip_fdr_clean_dcmp_error(vdp_dispchn_chn disp_id);
hi_void vdp_ip_fdr_disable_layer(hi_u32 layer, hi_u32 region_index);

#endif


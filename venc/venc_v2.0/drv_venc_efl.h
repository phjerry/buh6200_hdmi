/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2019-08-13
 */
#ifndef __DRV_VENC_EFL_H__
#define __DRV_VENC_EFL_H__

#include "hi_drv_sys.h"
#include "drv_venc_osal.h"
#include "drv_venc_buf_mng.h"
#include "hi_drv_video.h"
#include "drv_venc_queue_mng.h"
#include "drv_venc_ratecontrol.h"
#include "venc_strm_mng.h"
#include "drv_venc_queue.h"

/* #define __VENC_DRV_DBG__ */
/* #define __OMXVENC_ONE_IN_ONE_OUT__ */
#define VEDU_TR_STEP 1

#define H265E_MAX_FRM_SIZE 3
#define H265E_MAX_DPB_SIZE 3
#define H265E_MAX_REF_SIZE 3

#define H264E_MAX_FRM_SIZE 3
#define H264E_DPB_MAX_SIZE 2
#define H264E_DPB_LIST_MAX_SIZE 3

#define MAX_REF_SIZE 3
#define MAX_REF_RCN_SHARE_SIZE 2
#define BUFF_SHARE_DEFAULT 0
/*************************************************************************************/
#define MAX_VEDU_QUEUE_NUM         6
#define MAX_VEDU_STRM_QUEUE_NUM    6
#define MSG_QUEUE_NUM              100
#define INVAILD_CHN_FLAG           (-1)
#define OMX_OUTPUT_PORT_BUFFER_MAX 6
#define OMX_INPUT_PORT_BUFFER_MAX 6

#define PATH_LEN           32

#define FRMBUF_STRIDE_LIMIT  8192       /* 4096*2 */
/*******************************************************************/
#define NUM_REF_FRAMES_IN_POC_CYCLE 1
#define TIMER_INTERVAL_MS 1000
#define WRP_BUF_SIZE 4096
#define LUMA_HEIGHT_ALIGH 16

#define __VEDU_NEW_RC_ALG__

typedef hi_s32 (*ve_image_func)(hi_s32 handle, hi_drv_video_frame *image);
typedef hi_s32 (*ve_image_omx_func)(hi_s32 handle, hi_venc_user_buf *image);
typedef hi_s32 (*ve_change_info_func)(hi_handle handle, hi_u32 width, hi_u32 height, hi_u32 frm_rate);
typedef hi_s32 (*ve_detach_func)(hi_handle handle, hi_handle src);

typedef enum {
    USE_NONE = 0,
    USE_FRAME,
    USE_TOP,
    USE_BOT,
    USE_INTER,
    USE_BUTT
} pic_used;

typedef enum {
    REF_NONE = 0,
    REF_SHORT,
    REF_LONG,
    REF_BUTT,
} ref_state;

typedef struct {
    hi_u32 pro_file_idc;
    hi_u32 frame_width_in_mb;
    hi_u32 frame_height_in_mb;
    hi_u8 frame_crop_left;
    hi_u8 frame_crop_right;
    hi_u8 frame_crop_top;
    hi_u8 frame_crop_bottom;
    hi_u32 pic_order_cnt_type;
    hi_u32 log2_max_frame_num_minus4;
    hi_u32 long_term_reference_flag;
    hi_u32 adaptive_ref_pic_marking_mode_flag;
} vedu_efl_h264e_sps;

typedef struct {
    hi_u32 const_intra;
    hi_s32 chr_qp_offset;
    hi_u32 h264_hp_en;
    hi_u32 h264_cabac_en;
    hi_s32 *scale_8x8;
} vedu_efl_h264e_pps;

typedef struct {
    hi_bool ref_pic_list_reordering_flag_l0;
    hi_s32 reordering_of_pic_nums_idc_l0[33]; /* 33: size of reordering_of_pic_nums_idc_l0 */
    hi_s32 abs_diff_pic_num_minus1_l0[33];    /* 33: size of abs_diff_pic_num_minus1_l0 */
    hi_s32 long_term_pic_idx_l0[33];          /* 33: size of long_term_pic_idx_l0 */

    hi_bool ref_pic_list_reordering_flag_l1;
    hi_s32 reordering_of_pic_nums_idc_l1[33]; /* 33: size of reordering_of_pic_nums_idc_l1 */
    hi_s32 abs_diff_pic_num_minus1_l1[33];    /* 33: size of abs_diff_pic_num_minus1_l1 */
    hi_s32 long_term_pic_idx_l1[33];          /* 33: size of long_term_pic_idx_l1 */

    hi_u32 pic_num_lx_pred;
    hi_u32 pic_num_lx_pred1;
} h264_reorder;

typedef struct {
    hi_bool no_output_of_prior_pics_flag;
    hi_s32 long_term_reference_flag;
    hi_bool adaptive_ref_pic_marking_mode_flag;
    hi_s32 memory_management_control_operation[10]; /* 10: size of memory_management_control_operation */
    hi_s32 marking_operation[10];                   /* 10: size of marking_operation */
} h264e_mark;

typedef enum {
    MARKING_OPERATION_END = 0,
    MARKING_SHORTTERM_TO_NONREF = 1,
    MARKING_LONGTERM_TO_NONREF = 2,
    MARKING_SHORTTERM_TO_LONGTERM = 3,
    MARKING_SET_MAX_LONGETERMIDX = 4,
    MARKING_FLUSH_REF_LIST = 5,
    MARKING_CURFRAME_TO_LONGTERM = 6,
    MARKING_OPERATION_BUTT,
} h264e_marking_operation;

typedef enum {
    MARKING_MODE_SLIDING_WINDOW = 0,
    MARKING_MODE_ADAPTIVE = 1,
    MARKING_MODE__BUTT,
} h264e_marking_mode;

typedef struct {
    hi_u32 slice_type; /* 0-P, 2-I */
    hi_u32 frame_num;
    hi_u32 num_ref_index; /* 0 or 1 */
    hi_u32 pic_order_cnt_type;
    hi_u32 pic_order_cnt_lsb;
    hi_u32 direct_spatial_mv_pred_flag;
    hi_u32 num_ref_idx_l0_active_minus1;
    hi_u32 num_ref_idx_l1_active_minus1;
    hi_u32 ref_pic_list_reordering_flag_l0;
    hi_u32 ref_pic_list_reordering_flag_l1;
    hi_u32 num_active_ref_pics;
    hi_u32 num_ref_idx_active_override_flag;
    hi_u32 nal_ref_idc;
    h264_reorder reorder;
    h264e_mark mark;

    hi_u32 long_term_reference_flag;
    hi_u32 adaptive_ref_pic_marking_mode_flag;
} vedu_efl_h264e_slc_hdr;

typedef struct {
    /* the following should probably go in sequence parameters */
    hi_u32 pic_order_cnt_type;
    /* for poc mode 0 */
    hi_u32 log2_max_pic_order_cnt_lsb_minus4;
    hi_u32 max_poc_lsb;

    /* for poc mode 1 */
    hi_u32 delta_pic_order_always_zero_flag;
    hi_s32 offset_for_non_ref_pic;
    hi_s32 offset_for_top_to_bottom_field;
    hi_u32 num_ref_frames_in_pic_order_cnt_cycle;
    hi_s32 offset_for_ref_frame[NUM_REF_FRAMES_IN_POC_CYCLE];  /* MAX_LENGTH_POC_CYCLE in decoder */

    /* the following should probably go in picture parameters */
    hi_u32 pic_order_present_flag;

    /* the following is for slice header syntax elements of poc */
    /* for poc mode 0. */
    hi_u32 pic_order_cnt_lsb;
    hi_s32 delta_pic_order_cnt_bottom;
    /* for poc mode 1. */
    hi_s32 delta_pic_order_cnt[2];      /* 2: size of delta_pic_order_cnt */

    hi_u32 poc;

    /**< Used to roll back poc when re-encoding */
	/**< CNcomment: 当重编时，用来回退poc */
    hi_u32 poc_back;
    hi_u32 pic_order_cnt_lsb_back;

    /* for poc mode 1. */
    hi_u32 prev_frm_num;
    hi_u32 prev_frm_num_offset;
    hi_u32 frm_num_offset;
} h264e_poc;

typedef struct {
    hi_u32 enhance_cnt;
    hi_u32 b_frm_cnt;
    hi_u32 last_p;
    hi_u32 last_last_p;
    hi_u32 last_long_poc;

    hi_u32 last_long_poc_back;
    hi_u32 poc_last_p_bak;
    hi_u32 last_last_p_bak;
} h264e_ref;

typedef struct {
    hi_u32 phy_addr[2];     /* 2: size of phy_addr */
    hi_u32 width;
    hi_u32 height;
    hi_u32 y_stride;
    hi_u32 c_stride;
    hi_bool idr; /**< Image IDR logo */ /**< CNcomment 图像IDR标志 */
    hi_u32 pool_pic_id;
    hi_u32 pool_pic_info_id;
    hi_bool field;

    hi_bool used; /**< Image using flag */ /**< CNcomment 图像使用标志 */
    ref_state ref;
    hi_s32 frame_num;
    hi_s32 frame_num_wrap;
    hi_s32 long_term_pic_num;
    hi_s32 pic_num;
    hi_u32 poc;

    hi_u32 tmv_addr;
    hi_u32 pme_addr;
    hi_u32 pme_info_addr;
} h264e_pic;

typedef struct {
    hi_ulong frame[H264E_MAX_FRM_SIZE];
    h264e_pic *past_short_ref_frame_list[H264E_DPB_LIST_MAX_SIZE];
    h264e_pic *past_long_ref_frame_list[H264E_DPB_LIST_MAX_SIZE];
    h264e_pic *past_ref_pic_list0[H264E_DPB_LIST_MAX_SIZE];
    h264e_pic *past_ref_pic_list1[H264E_DPB_LIST_MAX_SIZE];
    hi_s32 list0_size;
    hi_s32 list1_size;

	/**< Number of reference frames actually encoded by the current data stream */
	/**< CNcomment: 当前码流实际编码参考帧个数 */
    hi_s32 max_dpb_size;
    hi_s32 used_size;
    hi_s32 short_ref_num;
    hi_s32 long_ref_num;
    hi_s32 ref_num;

    drv_venc_queue que_frame;
} h264e_dpb;


typedef struct {
    hi_bool general_progressive_source_flag;
    hi_bool general_interlaced_source_flag;
    hi_u32 vps_max_sub_layers_minus1;
    hi_u32 vps_max_dec_pic_buffering_minus1[2];     /* 2: size of vps_max_dec_pic_buffering_minus1 */
    hi_u32 vps_max_num_reorder_pics[2];             /* 2: size of vps_max_num_reorder_pics */
    hi_u32 vps_max_latency_increase_plus1[2];       /* 2: size of vps_max_latency_increase_plus1 */
} vedu_efl_h265e_vps;

typedef struct {
    hi_u32 profile_idc;
    hi_u32 pic_width_in_luma_samples;
    hi_u32 pic_height_in_luma_samples;
    hi_u32 pic_cropping_flag;
    hi_u32 pic_crop_left_offset;
    hi_u32 pic_crop_right_offset;
    hi_u32 pic_crop_top_offset;
    hi_u32 pic_crop_bottom_offset;
    hi_u32 pcm_enable;
    hi_u32 ipcm_log2_max_size;      /* pcm_log2_max_size */
    hi_u32 sao_enabled_flag;       /* sao_enabled_flag; */
    hi_u32 ipcm_log2_min_size;
    hi_u32 sps_temporal_mvp_enable_flag;
    hi_u32 strong_intra_smoothing_flag;

    hi_u32 num_short_term_ref_pic_sets;
    hi_u32 sps_max_sub_layers_minus1;
    hi_u32 sps_max_dec_pic_buffering_minus1;
    hi_u32 sps_max_num_reorder_pics;
    hi_u32 sps_max_latency_increase_plus1;

    /* Forward short-term reference frame */ /* <CNcomment: 前向短期参考帧 */
    /* Up to two forward reference frames */ /* <CNcomment: 最多两个前向参考帧 */
    hi_u32 delta_poc_s0_minus1[6][3]; /* 6,3: size of delta_poc_s0_minus1 */
    hi_u32 used_by_curr_pic_s0_flag[6][3]; /* 6,3: size of used_by_curr_pic_s0_flag */
    /* Back short-term reference frame */ /* <CNcomment: 后向短期参考帧 */
    /* Up to two forward reference frames */ /* <CNcomment: 最多两个后向参考帧 */
    hi_u32 delta_poc_s1_minus1[6][3]; /* 6,3: size of delta_poc_s1_minus1 */
    hi_u32 used_by_curr_pic_s1_flag[6][3]; /* 6,3: size of used_by_curr_pic_s1_flag */

    hi_u32 num_negative_pics[6];    /* 6: size of num_negative_pics */
    hi_u32 num_positive_pics[6];    /* 6: size of num_positive_pics */

    hi_u32 long_term_ref_pic_present_flag;
    hi_u32 num_long_term_ref_pics_sps;
    hi_u32 num_long_term_ref_pics_num;
    hi_u32 lt_ref_pic_poc_lsb_sps[3];           /* 3: size of lt_ref_pic_poc_lsb_sps */
    hi_u32 used_by_curr_pic_lt_sps_flag[3];     /* 3: size of used_by_curr_pic_lt_sps_flag */
} vedu_efl_h265e_sps;

/* Generate all POC syntax elements based on user-set poc_type and encoding characteristics */
/* <CNcomment: 所有POC 语法元素。根据用户设置的poc_type和编码特性生成。 */
typedef struct {
    hi_u32 pic_order_cnt_lsb;
    hi_s32 pic_order_cnt;
    hi_s32 max_pic_order_cnt_lsb;
    hi_u32 log2_max_poc_lsb;

    /* Backup poc, used for frame dropping or re-encoding back-off status */
	/* <CNcomment: 备份的poc, 用于丢帧或重编码回退状态 */
    hi_u32 pic_order_cnt_lsb_bak;
    hi_s32 pic_order_cnt_bak;
} h265e_poc;

typedef struct {
    hi_u32 cu_qp_delta_enable;
    hi_s32 i_cb_qp_offset;
    hi_s32 i_cr_qp_offset;

    hi_u32 pic_disable_db_filter;
    hi_s32 frm_qp_minus26;
    hi_u32 lists_modification_present_flag;
} vedu_efl_h265e_pps;

typedef struct {
    hi_s32 num_negative_pics;
    hi_s32 num_positive_pics;
    hi_s32 poc_short[H265E_MAX_REF_SIZE];
    hi_s32 short_term_used[H265E_MAX_REF_SIZE];
    hi_s32 poc_short_s1[H265E_MAX_REF_SIZE];
    hi_s32 short_term_used_s1[H265E_MAX_REF_SIZE];

    hi_s32 num_unmark_pics;
    hi_s32 poc_unmark[H265E_MAX_REF_SIZE];

    /* Up to four reference frames */
	/* <CNcomment: 最多四个参考帧 */
    hi_u32 ref_pic_list_modification_flag_l0;
    hi_u32 ref_pic_list_modification_flag_l1;

    hi_u32 num_long_term_pics;
    hi_u32 poc_long[2];             /* 2: size of poc_long */
    hi_u32 long_term_used[2];       /* 2: size of long_term_used */
} h265e_ref_list;

typedef struct {
    hi_u32 first_slice_in_pic;
    hi_u32 slice_type;          /* 2->I, 1->P, 0->B */
    hi_u32 poc_lsb;
    hi_u32 sps_temporal_mvp_enable_flag;
    hi_u32 sao_enabled_flag;
    hi_u32 slice_sao_luma;
    hi_u32 slice_sao_chroma;
    hi_u32 cabac_init_flag;
    hi_u32 five_minus_max_num_merge_cand;
    hi_s32 i_slice_qp_delta;
    hi_u32 db_filter_ctl_present;
    hi_u32 db_filter_override_enabled;
    hi_u32 df_override_flag;
    hi_u32 slice_header_disable_df;
    hi_u32 slice_lf_across_slice;
    hi_u32 frame_num;
    hi_u32 mvd_l1_zero_flag;
    hi_u32 collocated_from_l0_flag;
    hi_u32 collocated_ref_idx;
    hi_u32 num_ref_idx_l0_active_minus1;
    hi_u32 num_ref_idx_l1_active_minus1;

    hi_u32 short_term_ref_pic_set_idx;

    /* The number of reference frames that really need to be referenced */
	/* <CNcomment: 真正需要参考的参考帧个数 */
    hi_u32 num_active_ref_pics;
    /* The number of forward reference frames that really need to be referenced */
	/* <CNcomment: 真正需要参考的前向参考帧个数 */
    hi_u32 num_active_ref_pics_s0;

    /* Forward short-term reference frame */ /* <CNcomment: 前向短期参考帧 */
    /* Up to two forward reference frames */ /* <CNcomment: 最多两个前向参考帧 */
    hi_u32 delta_poc_s0_minus1[2];          /* 2: size of delta_poc_s0_minus1 */
    hi_u32 used_by_curr_pic_s0_flag[2];     /* 2: size of used_by_curr_pic_s0_flag */
    /* Back short-term reference frame */ /* <CNcomment: 后向短期参考帧 */
    /* Up to two forward reference frames */ /* <CNcomment: 最多两个后向参考帧 */
    hi_u32 delta_poc_s1_minus1[2];          /* 2: size of delta_poc_s1_minus1 */
    hi_u32 used_by_curr_pic_s1_flag[2];     /* 2: size of used_by_curr_pic_s1_flag */

    hi_u32 num_negative_pics;
    hi_u32 num_positive_pics;

    hi_u32 ref_pic_list_modification_flag_l0;
    hi_u32 ref_pic_list_modification_flag_l1;

    hi_u32 slice_poc_lsb_bits;

    hi_u32 num_long_term_sps ;
    hi_u32 num_long_term_pics ;
    hi_u32 num_long_term_ref_pic_sps;
    hi_u32 lt_idx_sps[2] ;                      /* 2: size of lt_idx_sps */
    hi_u32 poc_lsb_lt[2];                       /* 2: size of poc_lsb_lt */
    hi_u32 used_by_curr_pic_lt_flag[2];         /* 2: size of used_by_curr_pic_lt_flag */
    hi_u32 delta_poc_msb_present_flag[2];       /* 2: size of delta_poc_msb_present_flag */
    hi_u32 delta_poc_msb_cycle_lt[2];           /* 2: size of delta_poc_msb_cycle_lt */

    h265e_ref_list ref_list;

} vedu_efl_h265e_slc_hdr;

typedef struct {
    hi_u32 handle;

    ve_image_func get_image;
    ve_image_omx_func get_image_omx;
    ve_image_func put_image;
    ve_change_info_func change_info;
    ve_detach_func detach_func;
} vedu_efl_src_info;

typedef struct {
    hi_bool omx_chn;
    hi_bool auto_request_ifrm;
    hi_bool prepend_sps_pps;
    hi_bool rc_skip_frm_en;

    hi_u32 protocol;      /* VEDU_H264, VEDU_H263 or VEDU_MPEG4 */
    hi_u32 profile;       /* H264 profile */
    hi_u32 max_width;
    hi_u32 max_height;
    hi_u32 frame_width;    /* width    in pixel, 96 ~ 2048 */
    hi_u32 frame_height;   /* height in pixel, 96 ~ 2048 */

    hi_u32 rotation_angle; /* venc don't care */

    hi_u32 package_sel;
    hi_u32 priority;
    hi_u32 stream_buf_size;

    hi_bool quick_encode;
    hi_bool slc_split_en;   /* 0 or 1, slice split enable */
    hi_bool slc_split_mod;  /* 0 or 1, 0:byte; 1:mb line */
    hi_u32 split_size;     /* <512 mb line @ H264, H263 don't care */
    hi_u32 gop;
    hi_u32 gop_type;
    hi_u32 b_frm_num;
    hi_u32 sp_interval;
    hi_bool rcn_ref_share_buf; /* ref rcn buffer share flag */
    hi_u32 q_level;        /* venc don't care */
    hi_u32 drift_rc_thr;    /* the threshold of RC skip frame; vp default 20% */
    hi_bool is_avbr;
    hi_u32 priv_handle;
} vedu_efl_enc_cfg;

typedef struct {
    hi_u32 bit_rate;       /* 32k ~ 20M, bits per second */
    hi_u32 out_frm_rate;    /* 1 ~ in_frm_rate */
    hi_u32 in_frm_rate;     /* 1 ~ 30  */

    hi_u32 max_qp;       /* H264: 0 ~ 51, Mpeg4: 1 ~ 31 */
    hi_u32 min_qp;       /* min_qp should <= max_qp */
    hi_u32 gop;         /* for new RC ALG */
} vedu_efl_rc_attr;

typedef struct {
    hi_u32 vcpi_ipcm_en;
    hi_u32 vcpi_sao_chroma;
    hi_u32 vcpi_sao_luma;
    hi_u32 max_num_mergecand;
    hi_u32 tmv_en;
    hi_u32 vcpi_cabac_init_idc;
    hi_u32 pmv_poc[6];          /* 6: size of delta_poc_msb_cycle_lt */
} vedu_efl_base_attr;

typedef struct {
    hi_u16 slc_split_en;
    hi_u8  slc_split_mod;       /* 0 or 1, byte or mb line ,just select 1 */
    hi_u32 split_size;

    venc_buffer ddr_reg_cfg_buf;
    hi_u32  reg_config_mode;

    venc_buffer slice_buf;
    /* para */
    UADDR  strm_buf_addr;
    hi_s32 strm_buf_fd;
    hi_u32 strm_buf_rp_addr;     /* phy addr for hardware */
    hi_u32 strm_buf_wp_addr;
    hi_u32 strm_buf_size;

    /* frame buffer parameter */
    hi_u32 s_stride_y;
    hi_u32 s_stride_c;
    hi_u32 yuv_store_type;      /* 0, semiplannar; 1, package; 2,planer */
    hi_u32 package_sel;

    hi_u32 src_y_addr;
    hi_u32 src_c_addr;
    hi_u32 src_v_addr;          /* just for input of planner */

    hi_u32 rcn_y_addr[2];       /* 2: size of rcn_y_addr */
    hi_u32 rcn_c_addr[2];       /* 2: size of rcn_c_addr */
    hi_u32 rcn_y_len[2];        /* 2: size of rcn_y_len */
    hi_u32 rcn_c_len[2];        /* 2: size of rcn_c_len */

    hi_u32 ref_y_addr[2];       /* 2: size of ref_y_addr */
    hi_u32 ref_c_addr[2];       /* 2: size of ref_c_addr */
    hi_u32 ref_y_len[2];        /* 2: size of ref_y_len */
    hi_u32 ref_c_len[2];        /* 2: size of ref_c_len */

    hi_u32 rcn_y_block_height;
    hi_u32 rcn_y_block_width;
    hi_u32 rcn_y_pic_height;
    hi_u32 rcn_y_pic_width;
    hi_u32 rcn_block_size;

    hi_u32 rcn_y_next_height;

    hi_u32 tmv_addr[2];         /* 2: size of tmv_addr */
    hi_u32 pme_addr[2];         /* 2: size of pme_addr */
    hi_u32 pme_info_addr[3];    /* 3: size of pme_info_addr */

    hi_u32 slc_hdr_bits;        /* 8bit_0 | mark | reorder | slchdr */
    hi_u32 cabac_slc_hdr_part2_seg[8];      /* 8: size of cabac_slc_hdr_part2_seg */
    hi_u32 slc_hdr_part1;
    hi_u32 slc_hdr_size_part1;
    hi_u32 slc_hdr_size_part2;

    hi_u32 pts0;
    hi_u32 pts1;
    hi_u32 ext_flag;
    hi_u32 ext_fill_len;

    hi_u32 slc_hdr_stream[4];   /* 4: size of slc_hdr_stream */
    hi_u32 reorder_stream[2];   /* 2: size of reorder_stream */
    hi_u32 marking_stream[2];   /* 2: size of marking_stream */

    /* for smmu */
    hi_u32 tunl_cell_addr;
    hi_s32 clk_gate_en;

    vedu_efl_base_attr base_cfg;

#ifdef HI_SMMU_SUPPORT
    hi_ulong smmu_page_base_addr;
    hi_ulong smmu_err_write_addr;
    hi_ulong smmu_err_read_addr;
#endif

    hi_u32 venc_end_of_pic;
    hi_u32 mean_qp;

    /* add for safe mode */
    hi_u32 slice_length[16];          /* 16: size of slice_length */
    hi_u32 slice_is_end[16];          /* 16: size of slice_is_end */
    hi_u32 vedu_strm_addr[16];        /* 16: size of vedu_strm_addr */
    hi_u32 vedu_strm_buf_len[16];     /* 16: size of vedu_strm_buf_len */
    hi_u32 slice_idx;
    hi_bool first_slc_one_frm;

    hi_u32 para_set_phy_addr;
    hi_u32 para_set_len;
    hi_u32 para_set_real_len;
    hi_u32 para_set_array[48];        /* 48: size of para_set_array */
} vedu_hal;

typedef struct {
    hi_bool is_ref;  /**< Whether it is referenced */ /* <CNcomment: 是否被参考 */
    hi_u32 ref_long;
    hi_u32 poc;
} h265e_ref_info;

typedef struct {
    hi_s32 id;
    hi_s32 pic_pool_id;
    hi_s32 pic_info_pool_id;

    hi_s32 used;

    hi_u32 width;
    hi_u32 height;

	/* [0]Luminance first address [1]Chromatic first address */
	/* <CNcomment: [0]亮度分量首地址 [1]色度分量首地址 */
    hi_u32 phy_addr[2];         /* 2: size of phy_addr */
    hi_u32 tmv_addr;
    hi_u32 pme_addr;
    hi_u32 pme_info_addr;

    hi_u32 y_stride;
    hi_u32 c_stride;  /* Same as semi planar 420 YC stride */ /* <CNcomment: semi planar 420 YC stride 一样 */

    hi_s32 valid;  /* The current frame space is occupied */ /* <CNcomment: 当前frame空间被占用 */
	/* Whether it is referenced: 1. Top field reference; 2. Bottom field reference; 3. Frame reference */
	/* CNcomment: 是否被参考  1: 顶场参考;  2: 底场参考;  3: 帧参考 */
    hi_s32 reference;
    hi_s32 poc;
    hi_u32 ref_num;
    hi_u32 ref_long;

    hi_u32 max_frame_num;
    hi_u32 used_frame_num;
    hi_u32 max_used_frame;
    h265e_ref_info ref_info[2];     /* 2: size of ref_info */
} frame_store;

typedef struct {
    frame_store fs[H265E_MAX_FRM_SIZE];
    frame_store *rcn_frame;
    /* 0: Optimal reference frame; 1: Suboptimal reference frame */ /* <CNcomment:  0:最优参考帧 1:次优参考帧 */
    frame_store *ref_frame[2];  /* 2: size of ref_frame */
    hi_s32 max_frame_num;  /* Actual number of frames */ /* <CNcomment: 实际的帧存个数 */

    hi_s32 used_frame_num;
    hi_s32 max_used_frame;
} h265e_dpb;

typedef struct {
    hi_u32 pic_id;
    hi_u32 used;
    hi_u32 pic_addr;
}pic_pool_buffer;

typedef struct {
    hi_u32 pic_info_id;
    hi_u32 used;
    hi_u32 pic_info_addr;
}pic_info_pool_buffer;

typedef struct {
    hi_bool last_idr;
    hi_u32 last_poc;
    /* for BiPred */
    hi_s32 poc_last_p;
    hi_s32 poc_last_last_p;
    hi_u32 enhance_cnt;

    /* The number of actual B frames */ /* <CNcomment: 真正上B帧的个数 */
    hi_u32 b_frm_cnt;
} h265e_ref;

typedef struct {
    hi_u64 rcn_ref_y_base[MAX_REF_RCN_SHARE_SIZE]; /* <Luminance base address */ /* <CNcomment: 亮度基地址 */
    hi_u64 rcn_ref_c_base[MAX_REF_RCN_SHARE_SIZE]; /* <Chromatic base address */ /* <CNcomment: 色度基地址 */
	/* The first address of the current frame Luminance */ /* <CNcomment: 当前帧亮度首地址 */
    hi_u64 rcn_ref_y_addr[MAX_REF_RCN_SHARE_SIZE];
	/* The first address of the current frame Chromatic */ /* <CNcomment: 当前帧色度首地址 */
    hi_u64 rcn_ref_c_addr[MAX_REF_RCN_SHARE_SIZE];
    hi_u64 pme_base[MAX_REF_RCN_SHARE_SIZE]; /* PME base address */ /* <CNcomment: PME基地址 */
    hi_u64 pme_addr[MAX_REF_RCN_SHARE_SIZE]; /* PME first address */ /* <CNcomment: PME首地址 */
	/* Pixel height of current frame Luminance */ /* <CNcomment: 当前帧亮度像素高度 */
    hi_u32 rcn_ref_y_len[MAX_REF_RCN_SHARE_SIZE];
	/* Pixel height of current frame Luminance */ /* <CNcomment: 当前帧亮度像素高度 */
    hi_u32 rcn_ref_y_len1[MAX_REF_RCN_SHARE_SIZE];
	/* Pixel height of current frame Chromatic */ /* <CNcomment: 当前帧色度像素高度 */
    hi_u32 rcn_ref_c_len[MAX_REF_RCN_SHARE_SIZE];
	/* Pixel height of current frame Chromatic */ /* <CNcomment: 当前帧色度像素高度 */
    hi_u32 rcn_ref_c_len1[MAX_REF_RCN_SHARE_SIZE];
	/* Pixel height of the current frame Pme */ /* <CNcomment:  当前帧Pme像素高度 */
    hi_u32 pme_len[MAX_REF_RCN_SHARE_SIZE];
	/* The pixel height of the next frame of Luminance */ /* <CNcomment:  亮度下一帧像素高度 */
    hi_u32 rcn_next_height_y[MAX_REF_RCN_SHARE_SIZE];
	/* The pixel height of the next frame of Chromatic */ /* <CNcomment:  色度下一帧像素高度 */
    hi_u32 rcn_next_height_c[MAX_REF_RCN_SHARE_SIZE];
    hi_u32 pme_next_height[MAX_REF_RCN_SHARE_SIZE]; /* <PME base address */ /* <CNcomment:  PME基地址 */
    hi_u32 active_rcn_idx; /* <Write back frame number */ /* <CNcomment:  回写帧下标索引 */
    /* Reference frame subscript index of 1st frame */ /* <CNcomment:  第1帧参考帧下标索引 */
    hi_u32 active_ref0_idx;
    /* Reference frame subscript index of 2nd frame */ /* <CNcomment:  第1帧参考帧下标索引 */
    hi_u32 active_ref1_idx;
    hi_u32 active_num_ref_frames;
    hi_bool exchange_ref;
} vedu_efl_rcn_ref_share_buf;

typedef struct {
    hi_s32 i_catch_enable;
    hi_s32 prepend_sps_pps_enable;
    hi_s32 auto_skip_frame_en;
    hi_u32 drift_rc_skip_thr;
    hi_s32 frm_cnt;
    /* channel parameter */
    hi_u32 protocol;
    hi_u32 h264_cabac_en;
    hi_u32 h264_hp_en;

    hi_u32 yuv_sample_type;
    hi_u32 rotation_angle;
    hi_u8 priority;
    hi_bool quick_encode;
    hi_bool omx_chn;
    hi_bool i_frm_insert_by_save_strm;

    /* stream buffer parameter */
    venc_buffer stream_mmz_buf;
    venc_buffer wp_rp_mmz_buf;
    venc_buffer omx_output_buf[OMX_OUTPUT_PORT_BUFFER_MAX];
    venc_buffer omx_input_buf[OMX_INPUT_PORT_BUFFER_MAX];

    hi_handle strm_mng;             /* pInst num */
    strm_mng_buf buf[HI_VENC_SLICE_NUM];
    hi_u32 cur_strm_buf_size;
    hi_bool not_enough_buf;
    hi_u32* strm_buf_rp_vir_addr; /* phy addr for hardware */
    hi_u32* strm_buf_wp_vir_addr;

    /* JPGE stream buffer parameter */
    venc_buffer jpg_mmz_buf;
    hi_u32 vir_2_bus_offset;   /* offset = vir - bus, @ stream buffer */

    /* header parameter */
    hi_u8 vps_stream[64],  temp_vps_stream[64];    /* 64: size of vps_stream and temp_vps_stream */
    hi_u8 sps_stream[64],  temp_sps_stream[64];    /* 64: size of sps_stream and temp_sps_stream */
    hi_u8 pps_stream[320], temp_pps_stream[32];    /* 320,32: size of pps_stream and temp_pps_stream */
    hi_u32 vps_bits;
    hi_u32 sps_bits;
    hi_u32 pps_bits;
    hi_bool is_progressive;

    /* header for jpge */
#ifdef VENC_SUPPORT_JPGE
    hi_u8 jfif_hdr[698];      /* 698: size of pps_stream */
    hi_u8 jpge_yqt[64];       /* 64: size of jpge_yqt */
    hi_u8 jpge_cqt[64];       /* 64: size of jpge_cqt */
    hi_u32 q_level;
    volatile hi_u32 waiting_isr_jpge;    /* to block STOP channel */
#endif

    /* other parameter */
    volatile hi_u32 waiting_isr;         /* to block STOP channel */
    hi_bool never_enc;
    hi_bool first_nal_2_send;              /* for omx_venc send output buffer flags */
    hi_bool image_valid;
    hi_bool first_slc_for_low_dly;

    /* frame rate control out */
    hi_u32 tr_count;
    hi_u32 last_tr;
    hi_u32 inter_frm_cnt;
    /* frame rate control mid */
    hi_u32 last_frm_rate[6];             /* from the struction of Image, 6: size of last_frm_rate */
    hi_u32 last_sec_input_frm_rate[2];   /* from Timer stat, 2: size of last_sec_input_frm_rate */

    /* config reg */
    vedu_hal hal;

    /* rate control mid */
    valg_fifo valg_bits_fifo;
    hi_u32 mean_bit;
    hi_u32 bits_fifo[FRM_RATE_CNT_NUM];
    vedu_rc rc;

    /* slice head */
    hi_s32 chr_qp_offset;
    hi_s32 const_intra;
    hi_s32 num_ref_index;
    hi_s32 scale_8x8[128];         /* 128: size of last_sec_input_frm_rate */
    hi_u32 pic_order_cnt_type;
   /* register of low power work mode */
    hi_s32 intra_low_pow_en;
    hi_s32 frac_low_pow_en;
    hi_s32 intp_low_pow_en;

    /* attach vi or vo ,backup for vpss */
    vedu_efl_src_info src_info;
    hi_drv_video_frame image;
    hi_venc_user_buf image_omx;

    /* statistic */
    vedu_state_info stat;

    queue_info *frame_dequeue;
    queue_info *frame_queue;
    queue_info *msg_queue_omx;
    queue_info *stream_queue_omx;
    queue_info *frame_queue_omx;

    hi_s32 rc_dbg_print;
    hi_u32 strm_buf_ext_len;
    hi_u32 idr_pic_id;

    hi_venc_frm_rate_type input_frm_rate_type_default;
    hi_venc_frm_rate_type input_frm_rate_type_config;

    hi_u32 time_out;

    hi_u32 rcn_num;
    hi_u32 luma_size;
    hi_u32 tmv_size;
    hi_u32 pme_info_size;
    hi_u32 pme_size;

    hi_venc_gop_attr gop_attr;

    pic_pool_buffer pic_pool[MAX_REF_SIZE];
    pic_info_pool_buffer pic_info_pool[MAX_REF_SIZE];
    hi_u32 pic_num;
    hi_u32 pic_info_num;

    h265e_poc h265_poc;
    vedu_efl_h265e_slc_hdr h265e_slc_hdr;
    h265e_ref h265_ref;
    h265e_dpb h265e_dpb;

    hi_u32 num_short_term_ref_pic_sets;
    hi_u32 short_term_ref_pic_set_idx;
    hi_u32 lists_modification_present_flag;
    hi_u32 long_term_ref_pic_present_flag;

    vedu_efl_h264e_sps h264_sps;
    vedu_efl_h264e_slc_hdr h264_slc_hdr;
    h264e_poc h264_poc;
    h264e_ref h264_ref;
    h264e_dpb h264_dpb;
    hi_ulong h264_rcn_addr[H264E_MAX_FRM_SIZE];
    h264e_pic h264_pic[H264E_MAX_FRM_SIZE];
    drv_venc_queue h264_rcn_que;
    h264e_pic *h264_rcn_pic;
    h264e_pic *h264_reorder_ref[2];         /* 2: size of last_sec_input_frm_rate */
    hi_u32 frame_num;

    hi_bool enable_rcn_ref_share_buf;
    hi_u32 y_size;
    hi_u32 c_size;
    hi_u32 all_height_y;
    hi_u32 pic_height_y;
    hi_u32 blk_height_y;
    hi_u32 rcn_frm_size;
    hi_u32 frame_info_size;
    hi_u32 rcn_block_size;
    vedu_efl_rcn_ref_share_buf rcn_ref_share_buf;

    hi_u32 ref_gap_cnt;
    hi_u32 stream_flag;
    hi_u32 extra_rcn_flag;
    venc_buffer extra_rcn_buf;

    hi_bool abnormal_exit;
} vedu_efl_enc_para;

typedef struct {
    hi_u32 bus_vi_y;      /* 16-byte aligned  */
    hi_u32 bus_vi_c;      /* 16-byte aligned  */
    hi_u32 bus_vi_v;
    hi_u32 vi_y_stride;   /* 16-byte aligned  */
    hi_u32 vi_c_stride;   /* 16-byte aligned  */

    hi_u32 pts0;
    hi_u32 pts1;

    hi_u32 tunl_cell_addr;
} vedu_efl_enc_in;

typedef struct {
    vedu_efl_enc_para *enc_handle;
} vedu_efl_chn_ctx;

typedef struct {
    hi_u32 ip_free;       /* for channel control */
    vedu_efl_enc_para *curr_handle;   /* used in ISR */
    hi_u32 *reg_base;
    osal_spinlock *chn_lock;     /* lock ChnCtx[MAX_CHN] */

    hi_void *task_frame;  /* for both venc & omxvenc */
    hi_void *task_stream; /* juse for omxvenc */
    hi_u32 stop_task;
    hi_u32 task_running;  /* to block Close IP */
    hi_u32 task_strm_running;
    hi_bool  is_chip_id_v500_r001;    /* for record the chip type from hardware */
} vedu_efl_ip_ctx;

#define D_VENC_CHECK_PTR(ptr) \
    do {\
        if ((ptr) == HI_NULL)\
        { \
            HI_ERR_VENC("PTR '%s' is NULL.\n", # ptr); \
            return HI_ERR_VENC_NULL_PTR;           \
        }  \
    } while (0)

enum {
    VENC_STORE_SEMIPLANNAR   = 0,
    VENC_STORE_PACKAGE       = 1,
    VENC_STORE_PLANNAR       = 2
};
enum {
    VENC_YUV_420    = 0,
    VENC_YUV_422    = 1,
    VENC_YUV_444    = 2,
    VENC_YUV_NONE   = 3
};

enum {
    VENC_V_U        = 0,
    VENC_U_V        = 1
};


hi_s32    venc_drv_efl_open_vedu(hi_void);
hi_void   venc_drv_efl_close_vedu(hi_void);
hi_s32    venc_drv_efl_create_chan(vedu_efl_enc_para **enc_handle, vedu_efl_enc_cfg *enc_cfg);
hi_s32    venc_drv_efl_destroy_venc(vedu_efl_enc_para *enc_handle);
hi_s32    venc_drv_efl_attach_input(vedu_efl_enc_para *enc_handle, vedu_efl_src_info *src_info);
hi_s32    venc_drv_efl_detach_input(vedu_efl_enc_para *enc_handle, vedu_efl_src_info *src_info);
hi_s32    venc_drv_efl_start_venc(vedu_efl_enc_para *enc_handle);
hi_s32    venc_drv_efl_stop_venc(vedu_efl_enc_para *enc_handle);
hi_s32    venc_drv_efl_rc_attr_init(vedu_efl_enc_para *enc_handle, vedu_efl_rc_attr *rc_attr);
hi_s32    venc_drv_efl_rc_set_attr(vedu_efl_enc_para *enc_handle, vedu_efl_rc_attr *rc_attr);
hi_void   venc_drv_efl_request_i_frame(vedu_efl_enc_para *enc_handle);
hi_s32    venc_drv_efl_resume_vedu(hi_void);
hi_s32    venc_drv_efl_query_stat_info(vedu_efl_enc_para *enc_handle, vedu_state_info *stat_info);
hi_s32    venc_drv_efl_queue_frame(vedu_efl_enc_para *enc_handle, hi_drv_video_frame *frame);
hi_s32    venc_drv_efl_dequeue_frame(vedu_efl_enc_para *enc_handle, hi_drv_video_frame *frame);
hi_s32    venc_drv_efl_get_image(hi_s32 enc_usr_handle, hi_drv_video_frame *frame);
hi_s32    venc_drv_efl_get_image_omx(hi_s32 enc_usr_handle, hi_venc_user_buf *frame);
hi_s32    venc_drv_efl_put_image(hi_s32 enc_usr_handle, hi_drv_video_frame *frame);
hi_bool   is_chip_id_v500_r001(hi_chip_type chip_type, hi_chip_version chip_version);
hi_bool   is_support_ref_rcn_share_buf(hi_void);

hi_void venc_drv_efl_sort_priority(hi_void);

hi_void venc_drv_efl_wake_up_thread(hi_void);

hi_s32  venc_drv_efl_q_frame_by_attach(hi_u32 enc_usr_handle, hi_drv_video_frame *pstFrame);
hi_void  venc_drv_efl_rls_all_frame(vedu_efl_enc_para *enc_handle);
hi_void venc_drv_efl_suspend_vedu(hi_void);
hi_void  venc_drv_efl_flush_strm_header(vedu_efl_enc_para  *enc_para);

hi_s32 venc_drv_efl_alloc_buf_2_jpge(vedu_efl_enc_para *enc_handle, hi_u32 max_width, hi_u32 max_height);
hi_void  venc_drv_efl_free_buf_2_jpge(vedu_efl_enc_para *enc_handle);

hi_void set_stream_info(vedu_efl_enc_para *enc_para, strm_mng_stream_info *stream_info);

hi_s32 venc_drv_efl_acq_read_buffer(vedu_efl_enc_para *venc_chn, vedu_efl_nalu *vedu_packet);
hi_s32 venc_drv_efl_rls_read_buffer(vedu_efl_enc_para *venc_chn, vedu_efl_nalu *vedu_packet);

#endif /* __DRV_VENC_EFL_H__ */

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-06-05
 */
#ifndef __DRV_DEMUX_INDEX_H__
#define __DRV_DEMUX_INDEX_H__

/************************************************************************/
/* constant                                                             */
/************************************************************************/
#define  FIDX_VERSION  20090804

#define  FIDX_OK       0
#define  FIDX_ERR      -1

/* define the profiles */
#define H265_MAIN_PROFILE
#define HM11_SYNTAX                   1

#if defined(H265_MAIN_PROFILE)
#define HEVC_MAX_VIDEO_PARAM_SET_ID   16
#define HEVC_MAX_SEQ_PARAM_SET_ID     16
#define HEVC_MAX_PIC_PARAM_SET_ID     64
#define HEVC_MAX_DPB_SIZE             5
#define HEVC_MAX_LONG_TERM_PICS       5
#define HEVC_MAX_DELTA_POC            5
#endif

#define HEVC_MAX_DPB_PIC_BUF          6
#define HEVC_MAX_SLICE_NUM            512

#define HEVC_MAX_LSB_NUM              33
#define HEVC_MAX_TEMPLAYER            8
#define HEVC_MAX_INT                  2147483647
#define HEVC_MAX_UINT                 0xFFFFFFFFU

#define HEVC_FALSE                    0
#define HEVC_TRUE                     1

#if HM11_SYNTAX
#define HEVC_MAX_VPS_OP_SETS_PLUS1    1024
#define HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1  1
#endif

#define HEVC_MAX_FRAME_STORE                  20
#define HEVC_MAX_DPB_NUM                             17
#define HEVC_MAX_NUM_REF_PICS                        16
#define HEVC_MAX_SLOT_NUM                            512
#define HEVC_MAX_TILE_COLUMNS                        64
#define HEVC_MAX_TILE_ROWS                           64
#define HEVC_LOGIC_TILE_COLUMNS_LIMIT                20
#define HEVC_LOGIC_TILE_ROWS_LIMIT                   22

#define HEVC_MAX_CU_DEPTH     7
#define HEVC_MAX_CU_SIZE        (1 << (HEVC_MAX_CU_DEPTH))

#define HEVC_SCALING_LIST_NUM                    6
#define HEVC_SCALING_LIST_DC                      16
#define HEVC_SCALING_LIST_START_VALUE   8
#define HEVC_MAX_MATRIX_COEF_NUM           64
#define HEVC_MAX_LIST_SIZE                           (16 + 1)
#define HEVC_PROFILE_LIST_NUM          6
#define HEVC_LIST_ENTRY_NUM            32
#define HEVC_SIG_LAST_NUM              64
#define HEVC_LIST_X_SIZE                2
#define HEVC_POINT_OFFSET              256
#define HEVC_SCAL_LIST_SIZE            4
#define HEVC_MAX_REF_POC               16
#define HEVC_AUI_SIG_SIZE              128

#define HEVC_SINT8_MAX_VAL             127
#define HEVC_SINT8_MIN_VAL             (-128)
#define HEVC_SINT13_MAX_VAL            4095
#define HEVC_SINT13_MIN_VAL            (-4096)

#define HEVC_I_SLICE                   2
#define HEVC_P_SLICE                   1
#define HEVC_B_SLICE                   0
#define HEVC_ERR_SLICE                   3

#define HEVC_I_FRAME                  0
#define HEVC_P_FRAME                  1
#define HEVC_B_FRAME                  2
#define HEVC_IDR_FRAME                3
#define HEVC_BLA_FRAME                4
#define HEVC_CRA_FRAME                5
#define HEVC_ERR_FRAME                6

#define   HEVC_PIC_DECODING           1
#define   HEVC_PIC_EMPTY              0
#define   MRG_MAX_NUM_CANDS           5

#define   HEVC_DEC_ERR                (-1)
#define   HEVC_DEC_NORMAL             0

typedef struct {
    hi_u8   sub_layer_profile_present_flag[HEVC_PROFILE_LIST_NUM];
    hi_u8   sub_layer_level_present_flag[HEVC_PROFILE_LIST_NUM];
    hi_s32  general_level_idc;
} hevc_profile_tier_level;

typedef enum {
    SCAN_ZIGZAG = 0,
    SCAN_HOR,
    SCAN_VER,
    SCAN_DIAG
} hevc_coeff_scan_type;

typedef enum {
    scaling_list_4x4 = 0,
    scaling_list_8x8,
    scaling_list_16x16,
    scaling_list_32x32,
    SCALING_LIST_SIZE_NUM
} hevc_scaling_list_size;

typedef struct {
    hi_u8    use_transform_skip;
    hi_u8    use_default_scaling_matrix_flag [SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    hi_u8    scaling_list_pred_mode_flag[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    hi_s32   scaling_list_dc_coef_minus8;
    hi_s32   scaling_list_delta_coef;
    hi_s32   scaling_list_dc[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    hi_u32   ref_matrix_id[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    hi_u32   pred_matrix_id[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    hi_s32    scaling_list_coef[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM][HEVC_MAX_MATRIX_COEF_NUM];
    hi_u32   scaling_list_pred_matrix_id_delta[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
} hevc_scaling_list;

typedef struct {
    hi_u32  inter_ref_pic_set_prediction_flag;
    hi_u32  delta_idx;
    hi_u32  delta_rps_sign;
    hi_u32  abs_delta_rps;
    hi_u32  num_negative_pics;
    hi_u32  num_positive_pics;
    hi_u32  num_of_pics;
    hi_u32  num_ref_idc;
    hi_u32  num_of_longterm_pics;
    hi_u32  ref_idc[HEVC_MAX_NUM_REF_PICS + 1];
    hi_u32  used_flag[HEVC_MAX_NUM_REF_PICS + 1];
    hi_s32  delta_poc[HEVC_MAX_NUM_REF_PICS + 1];
    hi_s32  poc[HEVC_MAX_NUM_REF_PICS];
} hevc_short_term_rpset;

typedef struct {
    hi_u8   ref_pic_list_modification_flag_l0;
    hi_u8   ref_pic_list_modification_flag_l1;
    hi_u32  list_entry_l0[HEVC_LIST_ENTRY_NUM];
    hi_u32  list_entry_l1[HEVC_LIST_ENTRY_NUM];
} hevc_ref_pic_lists_moddification;

typedef struct {
    hi_s32 luma_weight_l0_flag[HEVC_MAX_NUM_REF_PICS];
    hi_s32 chroma_weight_l0_flag[HEVC_MAX_NUM_REF_PICS];
    hi_s32 luma_weight_l1_flag[HEVC_MAX_NUM_REF_PICS];
    hi_s32 chroma_weight_l1_flag[HEVC_MAX_NUM_REF_PICS];
} hevc_pred_weight_table;

typedef struct {
    hi_u8  valid;
    hi_u8  vps_temporal_id_nesting_flag;
    hi_u8  is_refresh;
    hi_u8  vps_extension_flag;
    hi_u8  vps_extension_data_flag;
    hi_u8  vps_poc_proportional_to_timing_flag;
    hi_u8  vps_timing_info_present_flag;
    hi_u8  vps_sub_layer_ordering_info_present_flag;

    hi_s32 video_parameter_set_id;
    hi_s32 vps_reserved_three_2bits;
    hi_s32 vps_max_layers_minus1;
    hi_s32 vps_max_sub_layers_minus1;
    hi_s32 vps_reserved_0xffff_16bits;
    hi_s32 vps_max_layer_id;
    hi_s32 vps_num_layer_sets_minus1;
    hi_s32 vps_num_units_in_tick;
    hi_s32 vps_time_scale;
    hi_s32 vps_num_ticks_poc_diff_one_minus1;
    hi_s32 vps_num_hrd_parameters;
    hi_s32 vps_max_dec_pic_buffering[HEVC_MAX_TEMPLAYER];
    hi_s32 vps_num_reorder_pics[HEVC_MAX_TEMPLAYER];
    hi_s32 vps_max_latency_increase[HEVC_MAX_TEMPLAYER];
    hi_s32 layer_id_included_flag[HEVC_MAX_VPS_OP_SETS_PLUS1][HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1];
    hi_s32 hrd_layer_set_idx[HEVC_MAX_VPS_OP_SETS_PLUS1];
    hi_s32 cprms_present_flag[HEVC_MAX_VPS_OP_SETS_PLUS1];

    hevc_profile_tier_level profile_tier_level;
} hevc_video_param_set;

typedef struct {
    hi_u8  is_refresh;
    hi_u8  valid;
    hi_u8  sps_temporal_id_nesting_flag;
    hi_u8  sps_sub_layer_ordering_info_present_flag;
    hi_u8  restricted_ref_pic_lists_flag;
    hi_u8  scaling_list_enabled_flag;
    hi_u8  sps_scaling_list_data_present_flag;
    hi_u8  amp_enabled_flag;
    hi_u8  sample_adaptive_offset_enabled_flag;
    hi_u8  pcm_loop_filter_disable_flag;
    hi_u8  long_term_ref_pics_present_flag;
    hi_u8  sps_temporal_mvp_enable_flag;
    hi_u8  sps_strong_intra_smoothing_enable_flag;
    hi_u8  vui_parameters_present_flag;
    hi_u8  sps_extension_flag;
    hi_u8  sps_extension_data_flag;
    hi_u8  used_by_curr_pic_lt_sps_flag[HEVC_MAX_LSB_NUM];

    hi_u32 video_parameter_set_id;
    hi_u32 sps_max_sub_layers_minus1;
    hi_u32 seq_parameter_set_id;
    hi_u32 chroma_format_idc;
    hi_u32 separate_colour_plane_flag;
    hi_u32 pic_width_in_luma_samples;
    hi_u32 pic_height_in_luma_samples;
    hi_u32 conformance_window_flag;
    hi_u32 conf_win_left_offset;
    hi_u32 conf_win_right_offset;
    hi_u32 conf_win_top_offset;
    hi_u32 conf_win_bottom_offset;
    hi_u32 bit_depth_luma;
    hi_u32 qp_bd_offset_y;
    hi_u32 bit_depth_chroma;
    hi_u32 qp_bd_offset_c;
    hi_u32 pcm_enabled_flag;
    hi_u32 pcm_bit_depth_luma;
    hi_u32 pcm_bit_depth_chroma;
    hi_u32 max_pic_order_cnt_lsb;
    hi_u32 bits_for_poc;
    hi_u32 log2_min_luma_coding_block_size_minus3;
    hi_u32 log2_min_cb_size_y;
    hi_u32 log2_diff_max_min_luma_coding_block_size;
    hi_u32 log2_ctb_size_y;
    hi_u32 min_cb_size_y;
    hi_u32 ctb_size_y;
    hi_u32 max_cu_width;
    hi_u32 max_cu_height;
    hi_u32 log2_min_transform_block_size_minus2;
    hi_u32 quadtree_tu_log2_min_size;
    hi_u32 log2_diff_max_min_transform_block_size;
    hi_u32 quadtree_tu_log2_max_size;
    hi_u32 log2_min_pcm_coding_block_size_minus3;
    hi_u32 pcm_log2_min_size;
    hi_u32 log2_diff_max_min_pcm_coding_block_size;
    hi_u32 pcm_log2_max_size;
    hi_u32 max_transform_hierarchy_depth_inter;
    hi_u32 quadtree_tu_max_depth_inter;
    hi_u32 max_transform_hierarchy_depth_intra;
    hi_u32 quadtree_tu_max_depth_intra;
    hi_u32 max_cu_depth;
    hi_u32 min_tr_depth;
    hi_u32 max_tr_depth;
    hi_u32 num_short_term_ref_pic_sets;
    hi_u32 num_long_term_ref_pic_sps;
    hi_u32 dpb_size;
    hi_u32 ctb_num_width;
    hi_u32 ctb_num_height;
    hi_u32 lt_ref_pic_poc_lsb_sps[HEVC_MAX_LSB_NUM];
    hi_u32 max_dec_pic_buffering[HEVC_MAX_TEMPLAYER];
    hi_u32 num_reorder_pics[HEVC_MAX_TEMPLAYER];
    hi_u32 max_latency_increase[HEVC_MAX_TEMPLAYER];

    hevc_profile_tier_level profile_tier_level;
    hevc_scaling_list scaling_list;
    hevc_short_term_rpset short_term_ref_pic_set[HEVC_MAX_PIC_PARAM_SET_ID];
} hevc_seq_param_set;

typedef struct {
    hi_u8  valid;
    hi_u8  is_refresh;
    hi_u8  dependent_slice_segments_enabled_flag;
    hi_u8  sign_data_hiding_flag;
    hi_u8  output_flag_present_flag;
    hi_u8  cabac_init_present_flag;
    hi_u8  constrained_intra_pred_flag;
    hi_u8  transform_skip_enabled_flag;
    hi_u8  cu_qp_delta_enabled_flag;
    hi_u8  pic_slice_chroma_qp_offsets_present_flag;
    hi_u8  weighted_pred_flag;
    hi_u8  weighted_bipred_flag;
    hi_u8  transquant_bypass_enable_flag;
    hi_u8  tiles_enabled_flag;
    hi_u8  entropy_coding_sync_enabled_flag;
    hi_u8  uniform_spacing_flag;
    hi_u8  loop_filter_across_tiles_enabled_flag;
    hi_u8  loop_filter_across_slices_enabled_flag;
    hi_u8  deblocking_filter_control_present_flag;
    hi_u8  deblocking_filter_override_enabled_flag;
    hi_u8  pic_disable_deblocking_filter_flag;
    hi_u8  pic_scaling_list_data_present_flag;
    hi_u8  lists_modification_present_flag;
    hi_u8  slice_segment_header_extension_present_flag;
    hi_u8  pps_extension_flag;
    hi_u8  pps_extension_data_flag;

    hi_s32 pic_parameter_set_id;
    hi_s32 seq_parameter_set_id;
    hi_u32 num_extra_slice_header_bits;
    hi_s32 num_ref_idx_l0_default_active;
    hi_s32 num_ref_idx_l1_default_active;
    hi_s32 pic_init_qp;
    hi_s32 diff_cu_qp_delta_depth;
    hi_s32 max_cu_qp_delta_depth;
    hi_s32 pic_cb_qp_offset;
    hi_s32 pic_cr_qp_offset;
    hi_s32 num_tile_columns;
    hi_s32 num_tile_rows;
    hi_s32 pps_beta_offset_div2;
    hi_s32 pps_tc_offset_div2;
    hi_s32 log2_parallel_merge_level;
    hi_s32 log2_min_cu_qp_delta_size;
    hi_s32 column_width[HEVC_MAX_TILE_COLUMNS];
    hi_s32 row_height[HEVC_MAX_TILE_ROWS];
    hi_s32 column_bd[HEVC_MAX_TILE_COLUMNS];
    hi_s32 row_bd[HEVC_MAX_TILE_ROWS];

    hevc_scaling_list  scaling_list;
} hevc_pic_param_set;

typedef enum {
    NOT_NEW_PIC = 0,
    IS_NEW_PIC,
    IS_SKIP_PIC,
} hevc_pic_type;

typedef struct {
    hi_u8  pic_output_flag;
    hi_u8  first_slice_segment_in_pic_flag;
    hi_u8  no_output_of_prior_pics_flag;
    hi_u8  dependent_slice_segment_flag;
    hi_u8  short_term_ref_pic_set_sps_flag;
    hi_u8  slice_sao_luma_flag;
    hi_u8  slice_sao_chroma_flag;
    hi_u8  slice_temporal_mvp_enable_flag;
    hi_u8  num_ref_idx_active_override_flag;
    hi_u8  mvd_l1_zero_flag;
    hi_u8  cabac_init_flag;
    hi_u8  collocated_from_l0_flag;
    hi_u8  deblocking_filter_override_flag;
    hi_u8  slice_disable_deblocking_filter_flag;
    hi_u8  slice_loop_filter_across_slices_enabled_flag;
    hi_u8  used_by_curr_pic_lt_flag[HEVC_MAX_NUM_REF_PICS];
    hi_u8  delta_poc_msb_present_flag[HEVC_MAX_NUM_REF_PICS];

    hi_s32 poc;
    hi_s32 prev_poc;
    hi_s32 slice_qp;
    hi_s32 slice_type;
    hi_s32 offset_len;
    hi_u32 nuh_temporal_id;
    hi_u32 nal_unit_type;
    hi_s32 pic_parameter_set_id;
    hi_s32 slice_segment_address;
    hi_s32 colour_plane_id;
    hi_s32 pic_order_cnt_lsb;
    hi_s32 short_term_ref_pic_set_idx;
    hi_u32 num_long_term_sps;
    hi_u32 num_long_term_pics;
    hi_u32 num_ref_idx_l0_active;
    hi_u32 num_ref_idx_l1_active;
    hi_s32 collocated_ref_idx;
    hi_s32 max_num_merge_cand;
    hi_s32 slice_qp_delta;
    hi_s32 slice_cb_qp_offset;
    hi_s32 slice_cr_qp_offset;
    hi_s32 slice_beta_offset_div2;
    hi_s32 slice_tc_offset_div2;
    hi_u32 num_entry_point_offsets;
    hi_u32 slice_segment_header_extension_length;
    hi_u32 slice_segment_header_extension_data_byte;
    hi_u32 dependent_slice_curstart_cuaddr;
    hi_u32 dependent_slice_curend_cuaddr;
    hi_u32 list_xsize[HEVC_LIST_X_SIZE];
    hi_u32 num_ref_idx[3]; /* 3 is the number of reference index. */
    hi_s32 entry_point_offset[HEVC_POINT_OFFSET];
    hi_s32 lt_idx_sps[HEVC_MAX_NUM_REF_PICS];
    hi_s32 poc_lsb_lt[HEVC_MAX_NUM_REF_PICS];
    hi_s32 delta_poc_msb_cycle_lt[HEVC_MAX_NUM_REF_PICS];
    hi_s32 pocLsbLt[HEVC_MAX_NUM_REF_PICS];
    hi_s32 delta_poc_msbcycle_lt[HEVC_MAX_NUM_REF_PICS];
    hi_s32 check_ltmsb[HEVC_MAX_NUM_REF_PICS];

    hevc_pic_type new_pic_type;
    hevc_short_term_rpset short_term_ref_pic_set;
    hevc_ref_pic_lists_moddification ref_pic_lists_modification;
} hevc_slice_segment_header;

typedef struct {
    hi_s32  poc;
    hi_u32  pic_type;
    hi_u32  is_long_term;
    hi_u32  is_short_term;

    struct  hevc_framestore *frame_store;
} hevc_storablepic;

typedef enum {
    FS_NOT_USED = 0,
    FS_IN_DPB,
    FS_DISPLAY_ONLY,
} hevc_fs_state;

typedef struct hevc_framestore {
    hi_u8   non_existing;
    hi_u8   is_reference;
    hi_u8   is_displayed;
    hevc_fs_state  frame_store_state;
    hi_s32  poc;
    hi_u32  pic_type;
    hevc_storablepic  frame;
} hevc_framestore;

typedef struct {
    hi_u8   is_ref_idc;
    hi_u8   state;
    hi_u32  nal_unit_type;
    hi_s32  thispoc;
    hi_u32  pic_type;
    hevc_framestore *frame_store;
} hevc_currpic;

typedef struct {
    hi_u32  size;
    hi_u32  used_size;
    hi_u32  max_long_term_pic_idx;
    hi_u32  ltref_frames_in_buffer;
    hi_u32  negative_ref_frames_in_buffer;
    hi_u32  positive_ref_frames_in_buffer;
    hevc_framestore *fs[HEVC_MAX_DPB_NUM];
    hevc_framestore *fs_negative_ref[HEVC_MAX_DPB_NUM];
    hevc_framestore *fs_positive_ref[HEVC_MAX_DPB_NUM];
    hevc_framestore *fs_ltref[HEVC_MAX_DPB_NUM];
} hevc_dpb;

typedef struct {
    hi_u8   dsp_check_pic_over_flag;
    hi_u8   nal_used_segment;
    hi_u32  nal_bitoffset;
    hi_u32  nal_segment;
    hi_u32  nal_trail_zero_bit_num;
    hi_u32  nal_unit_type;
    hi_u32  nal_ref_idc;
    hi_u32  forbidden_zero_bit;
    hi_u32  nuh_reserved_zero_6bits;
    hi_u32  nuh_temporal_id;
    hi_u32  is_valid;
    hi_u32  nal_integ;
} hevc_nalu;

enum nal_unit_type {
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0,  /* 0 */
    NAL_UNIT_CODED_SLICE_TRAIL_R,

    NAL_UNIT_CODED_SLICE_TSA_N,
    NAL_UNIT_CODED_SLICE_TLA_R,

    NAL_UNIT_CODED_SLICE_STSA_N,
    NAL_UNIT_CODED_SLICE_STSA_R,

    NAL_UNIT_CODED_SLICE_RADL_N,
    NAL_UNIT_CODED_SLICE_RADL_R,

    NAL_UNIT_CODED_SLICE_RASL_N,
    NAL_UNIT_CODED_SLICE_RASL_R,

    NAL_UNIT_RESERVED_VCL_N10,
    NAL_UNIT_RESERVED_VCL_R11,
    NAL_UNIT_RESERVED_VCL_N12,
    NAL_UNIT_RESERVED_VCL_R13,
    NAL_UNIT_RESERVED_VCL_N14,
    NAL_UNIT_RESERVED_VCL_R15,

    NAL_UNIT_CODED_SLICE_BLA_W_LP,     /* 16 */
    NAL_UNIT_CODED_SLICE_BLA_W_RADL,
    NAL_UNIT_CODED_SLICE_BLA_N_LP,
    NAL_UNIT_CODED_SLICE_IDR_W_RADL,
    NAL_UNIT_CODED_SLICE_IDR_N_LP,
    NAL_UNIT_CODED_SLICE_CRA,
    NAL_UNIT_RESERVED_IRAP_VCL22,
    NAL_UNIT_RESERVED_IRAP_VCL23,

    NAL_UNIT_RESERVED_VCL24,
    NAL_UNIT_RESERVED_VCL25,
    NAL_UNIT_RESERVED_VCL26,
    NAL_UNIT_RESERVED_VCL27,
    NAL_UNIT_RESERVED_VCL28,
    NAL_UNIT_RESERVED_VCL29,
    NAL_UNIT_RESERVED_VCL30,
    NAL_UNIT_RESERVED_VCL31,

    NAL_UNIT_VPS,                      /* 32 */
    NAL_UNIT_SPS,
    NAL_UNIT_PPS,
    NAL_UNIT_ACCESS_UNIT_DELIMITER,
    NAL_UNIT_EOS,
    NAL_UNIT_EOB,
    NAL_UNIT_FILLER_DATA,
    NAL_UNIT_PREFIX_SEI,               /* 39 prefix SEI */
    NAL_UNIT_SUFFIX_SEI,               /* 40 suffix SEI */
    NAL_UNIT_RESERVED_NVCL41,
    NAL_UNIT_RESERVED_NVCL42,
    NAL_UNIT_RESERVED_NVCL43,
    NAL_UNIT_RESERVED_NVCL44,
    NAL_UNIT_RESERVED_NVCL45,
    NAL_UNIT_RESERVED_NVCL46,
    NAL_UNIT_RESERVED_NVCL47,

    NAL_UNIT_EOPIC,                    /* NAL_UNIT_UNSPECIFIED_48, */
    NAL_UNIT_EOSTREAM,                 /* NAL_UNIT_UNSPECIFIED_49, */
    NAL_UNIT_UNSPECIFIED_50,
    NAL_UNIT_UNSPECIFIED_51,
    NAL_UNIT_UNSPECIFIED_52,
    NAL_UNIT_UNSPECIFIED_53,
    NAL_UNIT_UNSPECIFIED_54,
    NAL_UNIT_UNSPECIFIED_55,
    NAL_UNIT_UNSPECIFIED_56,
    NAL_UNIT_UNSPECIFIED_57,
    NAL_UNIT_UNSPECIFIED_58,
    NAL_UNIT_UNSPECIFIED_59,
    NAL_UNIT_UNSPECIFIED_60,
    NAL_UNIT_UNSPECIFIED_61,
    NAL_UNIT_UNSPECIFIED_62,
    NAL_UNIT_UNSPECIFIED_63,
    NAL_UNIT_INVALID,
};

typedef union {
    hevc_video_param_set vps_tmp;
    hevc_seq_param_set   sps_tmp;
    hevc_pic_param_set   pps_tmp;
} hevc_tmp_param_set;

typedef struct _tag_bitstream {
    hi_u8  *p_head;
    hi_u8  *p_tail;
    hi_u32  bufa;
    hi_u32  bufb;
    hi_s32  bs_len;
    hi_s32  buf_pos;
    hi_s32  total_pos;
    hi_s32  history_pos;
} BS, *LPBS;

typedef struct {
    hi_u8   b_new_sequence;
    hi_u8   b_no_output_of_prior_pics_flag;
    hi_u8   b_no_rasl_output_flag;
    hi_u8   prev_rapis_bla;
    hi_s8   auc_convert_to_bit[HEVC_MAX_CU_SIZE + 1];

    hi_s32  max_vps_num;
    hi_s32  max_sps_num;
    hi_s32  max_pps_num;
    hi_u32  allow_start_dec;
    hi_s32  prev_slice_poc;
    hi_s32  last_slice_poc;
    hi_u32  bit_depth_y;
    hi_u32  bit_depth_c;
    hi_s32  poc_random_access;
    hi_s32  poc_cra ;
    hi_s32  last_display_poc;
    hi_u32  total_slice_num;
    hi_u32  ui_add_cu_depth;
    hi_u32  prev_poc;
    hi_u32  prev_pic_parameter_set_id;
    hi_u32  scaling_list_size[HEVC_SCAL_LIST_SIZE];
    hi_u32  scaling_list_size_x[HEVC_SCAL_LIST_SIZE];
    hi_u32  sig_last_scan_cg32x32[HEVC_SIG_LAST_NUM];
    hi_u32  scaling_list_num[SCALING_LIST_SIZE_NUM];
    hi_u32  aui_sig_last_scan[HEVC_SCAL_LIST_SIZE][HEVC_MAX_CU_DEPTH][HEVC_AUI_SIG_SIZE * HEVC_AUI_SIG_SIZE];

    hi_s32 hevc_frm_cnt;
    hi_s32 hevc_frm_poc;
    hi_s32 hevc_frm_type;
    hi_s32 hevc_ref_num;
    hi_s32 hevc_ref_poc[HEVC_MAX_REF_POC];

    hi_s32 dmx_hevc_frm_cnt;
    hi_s32 dmx_hevc_frm_poc;
    hi_s32 dmx_hevc_frm_type;
    hi_s32 dmx_hevc_ref_num;
    hi_s32 dmx_hevc_ref_poc[HEVC_MAX_REF_POC];

    BS  *p_bs;
    hevc_video_param_set    p_vps[HEVC_MAX_VIDEO_PARAM_SET_ID];
    hevc_seq_param_set      p_sps[HEVC_MAX_SEQ_PARAM_SET_ID];
    hevc_pic_param_set      p_pps[HEVC_MAX_PIC_PARAM_SET_ID];
    hevc_nalu               *p_curr_nal;
    hevc_storablepic        *p_list_x[HEVC_LIST_X_SIZE][HEVC_MAX_LIST_SIZE];

    BS   bs;
    hevc_dpb  dpb;
    hevc_tmp_param_set       tmp_param;
    hevc_video_param_set     curr_vps;
    hevc_seq_param_set       curr_sps;
    hevc_seq_param_set       seisps;
    hevc_pic_param_set       curr_pps;
    hevc_currpic             curr_pic;
    hevc_slice_segment_header  curr_slice;
    hevc_nalu                nal_array;
    hevc_framestore          frame_store[HEVC_MAX_FRAME_STORE];
} hevc_ctx_info;

hi_s32 dmx_hevc_init(hevc_ctx_info *hevc_ctx);


/************************************************************************/
/* struct & enum                                                        */
/************************************************************************/
/* video standard type */
typedef enum hi_vidstd {
    VIDSTD_MPEG2,
    VIDSTD_MPEG4,
    VIDSTD_AVS,
    VIDSTD_H264,
    VIDSTD_VC1,
    VIDSTD_H263,
    VIDSTD_DIVX3,
    VIDSTD_AUDIO_PES,
    VIDSTD_HEVC,
    VIDSTD_BUTT
} vidstd;

/* stream data type */
typedef enum hi_strm_type {
    STRM_TYPE_ES = 0,
    STRM_TYPE_PES,
    STRM_TYPE_BUTT
} strm_type;

/* frame type */
typedef enum hi_fidx_frame_type {
    FIDX_FRAME_TYPE_UNKNOWN = 0,
    FIDX_FRAME_TYPE_I,
    FIDX_FRAME_TYPE_P,
    FIDX_FRAME_TYPE_B,
    FIDX_FRAME_TYPE_IDR,
    FIDX_FRAME_TYPE_BLA,
    FIDX_FRAME_TYPE_CRA,
    FIDX_FRAME_TYPE_PESH,
    FIDX_FRAME_TYPE_BUTT,
    FIDX_FRAME_TYPE_ERR,
} fidx_frame_type;

/* descriptor for frame information */
typedef struct hi_frame_pos {
    fidx_frame_type  frame_type;
    hi_s64        pre_pts;
    hi_s64        pts;
    hi_u64        global_offset;
    hi_s32        offset_in_packet;
    hi_s32        packet_count;
    hi_s32        frame_size;
    hi_u32        reservd;

    /* hevc private */
    hi_s32        cur_poc;
    hi_s32        ref_num;
    hi_s32        ref_poc[HEVC_MAX_REF_POC];
} frame_pos;

/* pvr index's SCD descriptor                                               */
typedef struct hi_findex_scd {
    hi_u8   index_type;             /* type of index(pts,sc,pause,ts) */
    hi_u8   start_code;             /* type of start code, 1byte after 000001 */
    hi_u16  reservd;
    hi_s64  pts_us;

    hi_u64  global_offset;        /* start code offset in global buffer */
    hi_u8   data_after_sc[8];      /* 1~8 byte next to SC */
    hi_u32  extra_scdata_size;     /* extra data size */
    hi_u32  extra_real_scdata_size; /* real extra data size */
    hi_u8   *extra_scdata;        /* save extra more data */
    hi_u32  extra_scdata_phy_addr;  /* extra data phy addr */
} findex_scd;

/*!***********************************************************************
@brief
    global init, clear residual information, and register call back.
 ************************************************************************/
hi_void fidx_init(hi_void);
hi_void fidx_de_init(hi_void);

/*!***********************************************************************
@brief
    open an instance
@param
    VidStandard:  video standard type
@return
    if success return the instance ID, between 0 and (FIDX_MAX_CTX_NUM-1)
    otherwise return -1
 ************************************************************************/
hi_s32  fidx_open_instance(vidstd vid_standard, strm_type strm_type, hi_u32 *param);

/************************************************************************
@brief
    close specified instance
@param
    InstIdx:  the ID of the instance to be closed
 ************************************************************************/
hi_s32  fidx_close_instance(hi_s32 inst_idx);


/*!***********************************************************************
@brief
    feed start code to FIDX.
    there are 2 method to feed necessary information to FIDX:
    1. feed stream directly. Call FIDX_MakeFrameIndex()
    2. feed start code. In this method, the start code must be scanned outside,
       This call this function to create index.
 ************************************************************************/
hi_void  fidx_feed_start_code(
    hi_s32 inst_idx,                    /*! instance ID */
    const findex_scd *sc);       /* SCD descriptor */

/*!***********************************************************************
@brief
    to see if the SC is usful for index making
@return
    if the SC is usful return FIDX_OK, otherwise return FIDX_ERR
 ************************************************************************/
hi_s32  fidx_is_scuseful(hi_s32 inst_idx, hi_u8 start_code);

#endif





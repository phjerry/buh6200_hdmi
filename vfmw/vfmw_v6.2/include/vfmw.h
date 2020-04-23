#ifndef __VFMW_H__
#define __VFMW_H__

#include "vfmw_ext.h"

#define VFMW_VERSION  (2019121300)

#if defined(HI_DEBUG) || defined(VT_SUPPORT)
#define VFMW_DEBUG 1
#else
#define VFMW_DEBUG 0
#endif

#ifndef INLINE
#ifdef VFMW_DPRINT_SUPPORT
#define INLINE
#else
#define INLINE inline
#endif
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifdef CFG_MAX_CHAN_NUM
#define VFMW_CHAN_NUM CFG_MAX_CHAN_NUM
#else
#define VFMW_CHAN_NUM 32
#endif

#define VFMW_INVALID_PTS ((hi_s64)(-1))
#define VFMW_MIN_PTS     ((hi_s64)(-2))
#define VFMW_MAX_PTS     ((hi_s64)(0x1638E38B88))        /*  us  */

#define VFMW_MEM_ALIGN 0x1000

/* Mem unique_id Format:
 <--   63 ~ 32 (32bit)  --  31 ~ 24 (8bit)  --  23 ~ 16 (8bit)  --  15 ~ 0 (16bit)  -->
 <--   Chan create time --   Mem Group ID   --      Mem ID      --     Chan ID      -->
*/
#define SET_CHAN_UID(unid, chan_id)        (((hi_u64)(unid) << 32) + ((chan_id)&0xffff))
#define SET_MEM_UID(chan_uid, group, type) (((hi_u64)(chan_uid)) | ((((hi_u32)(group)) & 0xff) << 24) | ((((hi_u32)(type)) & 0xff) << 16))
#define GET_CHAN_UID(uid)                  ((uid)&0xffffffff0000ffff)
#define GET_CHAN_ID_BY_UID(uid)            ((uid)&0xffff)
#define GET_MEM_ID_BY_UID(uid)             (((uid)&0x00ff0000) >> 16)

#define AFD_INFO_MIN_LENGTH_IN_BYTES       (5)
#define AFD_IDENTIFIER_LENGTH_IN_BYTES     (4)
#define AFD_IDENTIFIER                     (0x31475444)
#define AFD_ACTIVE_FORMAT_FLAG_BYTE_VALUE  (0x41)

#define UINT64_PTR(ptr) ((hi_void *)(hi_uintptr_t)(ptr))
#define PTR_UINT64(ptr) ((hi_u64)(hi_uintptr_t)(ptr))

#define VFMW_CHECK_SEC_FUNC(ret) \
    do { \
        if ((ret) != EOK) { \
            OS_PRINT("%s %d sec func call error\n", __func__, __LINE__); \
        } \
    } while (0)

typedef enum {
    INVALID_MODE = 0,
    QUERY_MODE = 1,
    REPORT_MODE,
    WORKMODE_BUTT
} vfmw_vpp_work_mode;

/* DAR(Display Aspect Ratio) */
typedef enum {
    VFMW_DAR_UNKNOWN = 0,
    VFMW_DAR_4_3,
    VFMW_DAR_16_9,
    VFMW_DAR_221_100,
    VFMW_DAR_235_100,
    VFMW_DAR_IMG_SIZE,
    VFMW_DAR_END_RESERVED
} vfmw_vdec_dar;

typedef enum {
    VFMW_MEM_GROUP_FSP = 0,
    VFMW_MEM_GROUP_USER = 1,
    VFMW_MEM_GROUP_BUTT
}MEM_GROUP_E;

typedef struct {
    hi_u64 dma_buf;
    hi_u8  is_cached;
    hi_u8  *vir_addr;
    UADDR  phy_addr;
    UADDR  mmu_addr;
    hi_u32    length;
    vfmw_mem_mode  mode;
    hi_handle vdec_handle;
    hi_handle ssm_handle;
} mem_record;

typedef struct {
    hi_u8    *vir_addr;
    UADDR     phy_addr;
    hi_s32    length;
} vfmw_stream_param;

typedef struct {
    hi_s32   mode;
    hi_s32   dis_nums;
} vfmw_dispic_param;

typedef struct {
    UADDR  phy_addr;
    UADDR  mmu_addr;
    hi_u64 vir_addr;
    hi_s32 length;
    vfmw_mem_mode mode;
    hi_char  asz_name[16]; /* asz max size is 16 */
    hi_bool  tvp;
    hi_u32 h_handle;
} vfmw_mem;

typedef struct {
    hi_s32 chan_no_cache_size;
    hi_s32 chan_cache_size;
    hi_s32 hal_size;
} vfmw_svr_mem_need;

typedef struct {
    UADDR  frm_phy_addr;
    hi_u64 frm_vir_addr;
    hi_u32 frm_size;

    UADDR  pmv_phy_addr;
    hi_u64 pmv_vir_addr;
    hi_u32 pmv_size;

    UADDR  meta_data_phy_addr;
    hi_u64 meta_data_addr;
    hi_u32 meta_data_size;
} frame_buf;

typedef struct {
    hi_u8  linear_en;
    hi_u8  compress_en;
    hi_u32 y_stride;
    hi_u32 uv_stride;
    hi_u32 head_stride;
    hi_u32 head_info_size;
    hi_u32 stride_2bit;
    hi_u32 bit_depth_luma;
    hi_u32 bit_depth_chroma;
    hi_u32 dec_width;
    hi_u32 dec_height;

    hi_u64 frm_dma_buf;
    UADDR  luma_phy_addr;
    UADDR  back_tunnel_line_num_phy_addr;
    UADDR  chrom_phy_addr;
    UADDR  luma_phy_addr_2bit;
    UADDR  chrom_phy_addr_2bit;
} image_dec;

typedef struct {
    hi_u16 display_primaries_x[3]; /* 3: array size */
    hi_u16 display_primaries_y[3]; /* 3: array size */
    hi_u16 white_point_x;
    hi_u16 white_point_y;
    hi_u32 max_disp_mastering_luminance;
    hi_u32 min_disp_mastering_luminance;
} mastering_display_colour_volums;

typedef struct {
    hi_u32 max_content_light_level;
    hi_u32 max_pic_average_light_level;
} content_light_level_info;

typedef struct {
    hi_u8 hdr_transfer_characteristic_idc;
} hlg_compatible_info;

typedef struct {
    hi_u8 tm_input_signal_black_level_offset;
    hi_u8 tm_input_signal_white_level_offset;
    hi_u8 shadow_gain;
    hi_u8 highlight_gain;
    hi_u8 mid_tone_width_adj_factor;
    hi_u8 tm_output_fine_tuning_num_val; /* 0~10 */
    hi_u8 tm_output_fine_tuning_x[11]; /* 11: array size */
    hi_u8 tm_output_fine_tuning_y[11]; /* 11: array size */
    hi_u8 saturation_gain_num_val;   /* 0~6 */
    hi_u8 saturation_gain_x[7]; /* 7: array size */
    hi_u8 saturation_gain_y[7]; /* 7: array size */
} sl_hdr1_metadata_variables;

typedef struct {
    hi_u8  luminance_mapping_num_val; /* 0~33 */
    hi_u16 luminance_mapping_x[34]; /* 34: array size */
    hi_u16 luminance_mapping_y[34]; /* 34: array size */
    hi_u8  colour_correction_num_val;  /* 0~33 */
    hi_u16 colour_correction_x[34]; /* 34: array size */
    hi_u16 colour_correction_y[34]; /* 34: array size */
    hi_u16 chroma_to_luma_injection_mu_a;
    hi_u16 chroma_to_luma_injection_mu_b;
} sl_hdr1_metadata_tables;

typedef struct {
    hi_u8  spec_version;
    hi_u8  payload_mode;
    hi_u8  hdr_pic_colour_space;
    hi_u8  hdr_master_display_colour_space;
    hi_u16 hdr_master_display_max_luminance;
    hi_u16 hdr_master_display_min_luminance;
    hi_u8  sdr_pic_colour_space;
    hi_u8  sdr_master_display_colour_space;
    union {
        sl_hdr1_metadata_variables variables;
        sl_hdr1_metadata_tables    tables;
    } metadata;
} sl_hdr1_matadata;

/* avs2 hdr */
typedef struct {
    hi_u32  tm_input_signal_black_level_offset;
    hi_u32  tm_input_signal_white_level_offset;
    hi_u32  shadow_gain;
    hi_u32  highlight_gain;
    hi_u32  mid_tone_width_adj_factor;
    hi_u32  tm_output_fine_tuning_num_val;
    hi_u32  tm_output_fine_tuning_x[10]; /* 10: array size */
    hi_u32  tm_output_fine_tuning_y[10]; /* 10: array size */
    hi_u32  saturation_gain_num_val;
    hi_u32  saturation_gain_x[6]; /* 6: array size */
    hi_u32  saturation_gain_y[6]; /* 6: array size */
} sl_hdr_metadata_var;

typedef struct {
    hi_u32  luminance_mapping_num_val;
    hi_u32  luminance_mapping_x[65]; /* 65: array size */
    hi_u32  luminance_mapping_y[65]; /* 65: array size */
    hi_u32  colour_correction_num_val;
    hi_u32  colour_correction_x[65]; /* 65: array size */
    hi_u32  colour_correction_y[65]; /* 65: array size */
} sl_hdr_metadata_tab;

typedef struct {
    hi_u32  part_id;
    hi_u32  major_spec_version_id;
    hi_u32  minor_spec_version_id;
    hi_u32  payload_mode;
    hi_u32  hdr_pic_colour_space;
    hi_u32  hdr_display_colour_space;
    hi_u32  hdr_display_max_luminance;
    hi_u32  hdr_display_min_luminance;
    hi_u32  sdr_pic_colour_space;
    hi_u32  sdr_display_colour_space;
    hi_u32  sdr_display_max_luminance;
    hi_u32  sdr_display_min_luminance;
    hi_u32  matrix_coefficient[4]; /* 4: array size */
    hi_u32  chroma_to_luma_injection[2]; /* 2: array size */
    hi_u32  k_coefficient[3]; /* 3: array size */
    union {
        sl_hdr_metadata_var variables;
        /* sl_hdr_metadata_tab tables; */
    } u;
} sl_hdr_metadata;

typedef struct {
    hi_u8  is_valid;
    hi_u8  linear_en;
    hi_u8  compress_en;
    hi_u32 y_stride;
    hi_u32 uv_stride;
    hi_u32 head_stride;
    hi_u32 head_info_size;
    hi_u32 stride_2bit;
    hi_u32 disp_width;
    hi_u32 disp_height;
    hi_u32 disp_center_x;
    hi_u32 disp_center_y;

    hi_s64 frm_mem_fd;
    hi_u64 frm_dma_buf;
    hi_s64 frm_mem_fd_1;
    hi_u64 frm_dma_buf_1;
    UADDR  luma_phy_addr;
    hi_u64 luma_vir_addr;
    UADDR  back_tunnel_line_num_phy_addr;
    UADDR  chrom_phy_addr;
    UADDR  luma_phy_addr_1;
    UADDR  chrom_phy_addr_1;
    UADDR  luma_phy_addr_2bit;
    UADDR  chrom_phy_addr_2bit;

    hi_u64 sec_info;
} image_disp;

typedef struct {
    hi_u32 avg_qp;
    hi_u32 frame_stream_size;
    hi_u32 max_mv;
    hi_u32 min_mv;
    hi_u32 avg_mv;
    hi_u32 li_max_mv;
    hi_u32 li_min_mv;
    hi_u32 li_avg_mv;
    hi_u32 skip_ratio;
    vfmw_vid_std vid_std;
} image_uvmos;

typedef struct {
    hi_u32   actual_crc_8bit[2]; /* 2: array size */
    hi_u32   actual_crc_2bit[2]; /* 2: array size */
    hi_u32   actual_crc_8bit_1[2]; /* 2: array size */
} image_crc;

/*********************************************************************************************************************/
/* image_format bits introduce:                                                                                      */
/* [1:0] frame_type: 00(I), 01(P), 10(B), 11(reserved)                                                               */
/* [4:2] sample_type: 000(YUV:4:2:0), 001(YUV:4:0:0), 010~111(reserved)                                              */
/* [7:5] norm_type: 000(component), 001(PLA), 010(NTSC), 011(SECAM), 100(MAC),                                       */
/*                  101(Unspecified Video Format), 110~111(reserved)                                                 */
/* [9:8] source_format: 00(progressive), 01(interlaced), 10(infered_progressive), 11(infered_interlaced)             */
/* [11:10] field_valid_flag: 00(top_field invalid, bottom_field invalid), 01(top_field valid, bottom_field invalid), */
/*                           10(top_field invalid, bottom_field valid), 11(top_field valid, bottom_field valid)      */
/* [13:12]top_field_first: 00(bottom field first), 01(top field first), 10(un-know), 11(reserved)                    */
/* [16:14] aspect_ratio: 000(unspecified), 001(4:3), 010(16:9), 011(2.21:1),100(2.35:1),                             */
/*                       101(origin width and height), 111(reserved)                                                 */
/* [17]    vdh_2d_en: 0: 1d ; 1: 2d                                                                                  */
/* [18]    pts_unit : 0: ms ; 1: us                                                                                  */
/* [31:19] reserved                                                                                                  */
/*********************************************************************************************************************/
typedef struct {
    hi_u8 frame_type;           /*[1:0]*/
    hi_u8 sample_type;          /*[4:2]*/
    hi_u8 video_format;         /*[7:5]*/
    hi_u8 source_format;        /*[9:8]*/
    hi_u8 field_valid_flag;     /*[11:10]*/
    hi_u8 top_field_first;      /*[13:12]*/
    hi_u8 aspect_ratio;         /*[16:14]*/
    hi_u8 frame_rate_valid;     /* [20] */
    hi_u8 repeat_first_filed;
    hi_u8 reserved;
} image_format;

typedef union {
    hi_u32  data;
    struct {
        hi_u32 top_over : 8; /*0 not dec  1 dec success 2 dec error  3 needed dec but in VOQ*/
        hi_u32 bot_over : 8; /*0 not dec  1 dec success 2 dec error  3 needed dec but in VOQ*/
        hi_u32 reserved : 16;
    } bits;
} image_dec_over;

typedef struct {
    hi_bool active_format_flag;
    hi_u32 active_format;
} afd_info;

typedef struct {
    hi_u8  mastering_available;
    hi_u8  content_available;
    hi_u8  s1_hdr1_metadata_available;
    hi_u8  is_backwards_compatible;
    hi_bool dynamic_metadata_valid;

    UADDR  hdr_metadata_phy_addr;
    hi_u32 hdr_metadata_size;
    hi_u64 hdr_metadata_vir_addr;
    hi_u64 hdr_dma_buf;

    vfmw_color_desc colour_info;
    mastering_display_colour_volums mastering_display_colour_volume;
    content_light_level_info content_light_level;
    hlg_compatible_info comapitibility_info;

    hi_u8         be_dv_hdr_chan;
    hi_u8         dv_compatible;
    vfmw_dv_ves_imp ves_imp;
    vfmw_dv_layer_imp layer_imp;
    vfmw_dv_codec_cap dv_capacity;
} vfmw_hdr_info;

typedef enum {
    FRAME_PACKING_TYPE_NONE,             /* normal frame, not a 3D frame */
    FRAME_PACKING_TYPE_SIDE_BY_SIDE,     /* side by side */
    FRAME_PACKING_TYPE_TOP_BOTTOM,       /* top bottom */
    FRAME_PACKING_TYPE_TIME_INTERLACED,  /* time interlaced: one frame for left eye, the next frame for right eye */
    FRAME_PACKING_TYPE_BUTT
} vfmw_frm_pack_type;

typedef struct {
    hi_u8  pic_structure; /* 0: frame, 1: top, 2: bottom, 3: mbaff, 4: field pair */
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
} vfmw_vc1_range;

typedef struct {
    hi_u8  mjpeg;
    hi_s32 is_progressive_seq;
    hi_s32 is_progressive_frm;
    hi_s32 real_frm_rate;
    hi_s32 matrix_coef;
    hi_u32 rwzb;
} optmalg_info;

typedef struct {
    hi_u32 disp_width;
    hi_u32 disp_height;
    hi_u32 bit_depth;

    hi_u32 luma_stride;

    hi_u32 luma_header_offset;
    hi_u32 luma_data_offset;

    hi_u32 chroma_header_offset;
    hi_u32 chroma_data_offset;

    hi_u32 luma_data2_offset;     /* for 10bit or wider bits use */
    hi_u32 chroma_data2_offset;   /* for 10bit or wider bits use */
} vfmw_attr_hfbc;

typedef struct {
    hi_u32      version;
    hi_u32      aspect_width;
    hi_u32      aspect_height;
    hi_u32      disp_enable_flag;
    hi_u32      disp_frame_distance;
    hi_u32      distance_before_first_frame;
    hi_u32      gop_num;

    hi_bool     is_user_dec;
    hi_u32      is_fld_flag;  /* 0:frm, 1:fld */
    hi_u32      top_fld_type;
    hi_u32      bottom_fld_type;
    hi_u32      image_width;
    hi_u32      image_height;
    hi_u32      frame_rate;   /* frame rate, in Q10 */
    hi_u32      image_id;
    hi_u32      error_level;
    hi_u32      seq_img_cnt;

    hi_u32      bit_depth;
    hi_s32      last_frame;
    hi_s32      view_id;      /* h264 mvc */
    hi_s32      image_id_1;
    hi_u32      is_3d;
    UADDR       line_num_phy_addr;
    hi_u32      rwzb;
    hi_u64      frame_ptr;
    hi_u32      out_put_flag;
    hi_u32      left_offset;
    hi_u32      right_offset;
    hi_u32      top_offset;
    hi_u32      bottom_offset;
    hi_u32      is_sec_frm;
    hi_s32      inter_pts_delta;
    hi_u64      usrdat[VFMW_MAX_USD_NUM];
    hi_s64      src_pts;
    hi_s64      pts;
    hi_u64      user_tag;
    hi_u64      disp_time;
    hi_u8       dv_dual_layer;
    hi_s32      shr_index;
    hi_u16      disp_ctrl_width;
    hi_u16      disp_ctrl_height;

    image_dec           dec_info;
    image_disp          disp_info;
    image_uvmos         uvmos_info;
    image_crc           crc_info;
    image_format        format;
    image_dec_over      dec_over;

    afd_info            afd;
    vfmw_yuv_fmt        yuv_format;
    vfmw_hdr_info       hdr_input;
    vfmw_vid_std        vid_std;
    vfmw_frm_pack_type  frm_pack_type;
    vfmw_vc1_range      vc1_range;
    vfmw_image_attached attach_buf;
} vfmw_image;

typedef struct {
    hi_s32 (*mark_image_state)(hi_s32 chan_id, vfmw_image *image, hi_s32 structure, hi_u32 state);
    hi_s32 (*mark_field_flag)(hi_s32 chan_id, vfmw_image *image);
    hi_s32 (*update_image_info)(hi_s32 chan_id, vfmw_image *image);
} vfmw_fsp_frame_intf;

#endif /* *< __VFMW_H__  */


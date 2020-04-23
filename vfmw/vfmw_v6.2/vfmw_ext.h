#ifndef __VFMW_EXT_H__
#define __VFMW_EXT_H__

#include "hi_type.h"

/*  for adapt vfmw v6r2  */
#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define UADDR         hi_u64
#define MAX_VAL_UADDR (-1ULL)
#else
#define UADDR         hi_u32
#define MAX_VAL_UADDR (-1)
#endif

#define VFMW_MAX_USD_NUM         4
#define VFMW_MAX_USD_SIZE       (0x300)
#define VFMW_CREATE_OPT_SIZE     128
#define VFMW_REPORT_MAX_NAME_LEN 32

/* omx buffer flags bits. */
#define VFMW_OMX_STM_FLAG_EOS         0x00000001
#define VFMW_OMX_STM_FLAG_STARTTIME   0x00000002
#define VFMW_OMX_STM_FLAG_DECODEONLY  0x00000004
#define VFMW_OMX_STM_FLAG_DATACORRUPT 0x00000008
#define VFMW_OMX_STM_FLAG_ENDOFFRAME  0x00000010
#define VFMW_OMX_STM_FLAG_SYNCFRAME   0x00000020
#define VFMW_OMX_STM_FLAG_EXTRADATA   0x00000040
#define VFMW_OMX_STM_FLAG_CODECCONFIG 0x00000080

typedef hi_s32 (*fun_vfmw_init)(hi_void *args);
typedef hi_s32 (*fun_vfmw_exit)(hi_void *args);
typedef hi_s32 (*fun_vfmw_suspend)(hi_void *args);
typedef hi_s32 (*fun_vfmw_resume)(hi_void *args);
typedef hi_s32 (*fun_vfmw_get_frame)(hi_s32 chan_id, hi_void *frame, hi_void *ext_frm_info);
typedef hi_s32 (*fun_vfmw_rels_frame)(hi_s32 chan_id, const hi_void *frame);
typedef hi_s32 (*fun_vfmw_control)(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 len);
typedef hi_s32 (*fun_vfmw_event_report)(hi_s32 chan_id, hi_s32 type, hi_void *args, hi_s32 size);

typedef enum {
    VFMW_STD_START = 0,
    VFMW_H264,
    VFMW_VC1,
    VFMW_MPEG4,
    VFMW_MPEG2,
    VFMW_H263,
    VFMW_DIVX3,
    VFMW_AVS,
    VFMW_JPEG,
    VFMW_REAL8,
    VFMW_REAL9,
    VFMW_VP6,
    VFMW_VP6F,
    VFMW_VP6A,
    VFMW_VP8,
    VFMW_SORENSON,
    VFMW_MVC,
    VFMW_HEVC,
    VFMW_VP9,
    VFMW_AVS2,
    VFMW_AV1,
    VFMW_AVS3,
    VFMW_MJPEG,
    VFMW_DBVS,
    VFMW_WMV1,
    VFMW_WMV2,
    VFMW_WMV3,
    VFMW_STD_MAX
} vfmw_vid_std;

typedef enum {
    VFMW_DEC_MODE_NORMAL = 0,
    VFMW_DEC_MODE_IP,
    VFMW_DEC_MODE_I,
    VFMW_DEC_MODE_FIRST_I,
    VFMW_DEC_MODE_DROP_INVALID_B,
    VFMW_DEC_WORK_MODE_KEEP_INVALID_B,
    VFMW_DEC_MODE_MAX
} vfmw_dec_mode;

typedef enum {
    VFMW_DISP_ORDER = 0,
    VFMW_DEC_ORDER,
    VFMW_DEC_ORDER_SIMPLE_DPB,
    VFMW_DEC_ORDER_SKYPLAY,
    VFMW_DISP_ORDER_BASE_LAYER,      /* output base layer only (shvc etc), same as VFMW_DISP_ORDER */
    VFMW_DISP_ORDER_ENHANCED_LAYER,  /* output enhanced layer only (shvc etc) */
    VFMW_DISP_ORDER_ALL_LAYERS,      /* output all layers (mv-hevc/shvc etc) */
    VFMW_OUTPUT_ORDER_MAX
} vfmw_out_order;

typedef enum {
    VFMW_CMP_ADJUST = 0,
    VFMW_CMP_ON,
    VFMW_CMP_OFF,
    VFMW_CMP_MAX
} vfmw_cmp;

typedef enum {
    VFMW_SOURCE_MEDIA = 0,
    VFMW_SOURCE_DTV,
    VFMW_SOURCE_NETWORK,
    VFMW_SOURCE_MAX
} vfmw_source;

typedef enum {
    VFMW_BIT_DEPTH_NORMAL = 0,
    VFMW_BIT_DEPTH_8BIT_ONLY,
    VFMW_BIT_DEPTH_16BIT_EXPANDED,
    VFMW_BIT_DEPTH_MAX
} vfmw_bit_depth_attr;

typedef enum {
    VFMW_FLUSH_IN = 0,
    VFMW_FLUSH_OUT,
    VFMW_FLUSH_ALL,
    VFMW_FLUSH_MAX
} vfmw_flush_type;

typedef enum {
    VFMW_FRM_ALLOC_INNER = 0,
    VFMW_FRM_ALLOC_OUTER,
    VFMW_FRM_ALLOC_MAX
} vfmw_frm_alloc_type;

typedef enum {
    VFMW_CID_GET_CAPABILITY = 0,
    VFMW_CID_CREATE_CHAN,
    VFMW_CID_DESTROY_CHAN,
    VFMW_CID_GET_CHAN_CFG,
    VFMW_CID_CFG_CHAN,
    VFMW_CID_GET_CHAN_STATE,
    VFMW_CID_START_CHAN,
    VFMW_CID_STOP_CHAN,
    VFMW_CID_RESET_CHAN,
    VFMW_CID_SET_STREAM_INTF,
    VFMW_CID_GET_CHAN_MEMSIZE,
    VFMW_CID_RELEASE_STREAM,
    VFMW_CID_RESET_CHAN_WITH_OPTION,
    VFMW_CID_SET_FRAME_INTF,
    VFMW_CID_GET_USRDEC_STREAM,
    VFMW_CID_PUT_USRDEC_STREAM,
    VFMW_CID_GET_USRDEC_FRAME,
    VFMW_CID_PUT_USRDEC_FRAME,
    VFMW_CID_SET_DISCARDPICS_PARAM,
    VFMW_CID_EXTRA_WAKEUP_THREAD,
    VFMW_CID_SET_PTS_TO_SEEK,
    VFMW_CID_SET_TRICK_MODE,
    VFMW_CID_REPORT_DEMUX_RELEASE,
    VFMW_CID_SET_CTRL_INFO,
    VFMW_CID_SET_FRAME_RATE,
    VFMW_CID_START_LOWDLAY_CALC,
    VFMW_CID_STOP_LOWDLAY_CALC,
    VFMW_CID_ALLOC_MEM_TO_CHANNEL,
    VFMW_CID_ALLOC_MEM_TO_DECODER,
    VFMW_CID_SET_FRAME_BUFFER_NUM,
    VFMW_CID_SET_FRAME_MEM,
    VFMW_CID_SET_MORE_GAP_ENABLE,
    VFMW_CID_SET_FRAME_RATE_TYPE,
    VFMW_CID_GET_FRAME_RATE_TYPE,
    VFMW_CID_BIND_MEM,
    VFMW_CID_SET_FRAME_BUFFER_MODE,
    VFMW_CID_BIND_EXTERN_BUFFER,
    VFMW_CID_UNBIND_EXTERN_BUFFER,
    VFMW_CID_INQUIRE_ADPT_TYPE,
    VFMW_CID_ALLOC_SEC_BUF,
    VFMW_CID_FREE_SEC_BUF,
    VFMW_CID_INIT_ES_BUF,
    VFMW_CID_DEINIT_ES_BUF,
    VFMW_CID_BIND_ES_USR_ADDR,
    VFMW_CID_QUEUE_STREAM,
    VFMW_CID_DEQUEUE_STREAM,
    VFMW_CID_SEND_EOS,
    VFMW_CID_SET_EVENT_MAP,
    VFMW_CID_BIND_ES_BUF,
    VFMW_CID_FLUSH_CHAN,
    VFMW_CID_SET_IDR_PTS,
    VFMW_CID_BIND_USD_BUF,
    VFMW_CID_ACQ_USD,
    VFMW_CID_RLS_USD,
    VFMW_CID_CHK_USD,
    VFMW_CID_SET_3D_TYPE,
    VFMW_CID_GET_3D_TYPE,
    VFMW_CID_FLUSH_POST_PROCESS,
    VFMW_CID_MAX
} vfmw_cid;

typedef enum {
    MEM_CMA_CMA = 0,
    MEM_CMA_SEC,
    MEM_MMU_MMU,
    MEM_MMU_SEC,
} vfmw_mem_mode;

/* Dolby Vision elementary stream codec capacity */
typedef enum {
    DV_BL_DECODING_SUPPORT = 0, /* support decoding BL elementary stream only */
    DV_EL_DECODING_SUPPORT,     /* support decoding EL elementary stream only */
    DV_DECODING_MAX,
} vfmw_dv_codec_cap;

/* Dolby Vision VES implementation type */
typedef enum {
    DV_VES_IMP_START = 0,
    DV_SINGLE_VES_IMP,          /* single VES implementation */
    DV_DUAL_VES_IMP,            /* dual VES implementation */
    DV_VES_IMP_MAX
} vfmw_dv_ves_imp;

/* Dolby Vision layer implementation type */
typedef enum {
    DV_LAYER_START = 0,
    DV_SINGLE_LAYER_IMP,        /* single VES implementation */
    DV_DUAL_LAYER_IMP,          /* dual VES implementation */
    DV_LAYER_IMP_MAX
} vfmw_dv_layer_imp;

typedef enum {
    EXT_BUF_STATUS_INVALID = 0,
    EXT_BUF_STATUS_AVAILABLE,
    EXT_BUF_STATUS_UNAVAILABLE,
    EXT_BUF_STATUS_MAX
} vfmw_ext_buf_status;

typedef enum {
    ADPT_TYPE_VDEC = 0,
    ADPT_TYPE_OMX,
    ADPT_TYPE_MAX
} vfmw_adpt_type;

typedef enum {
    PTS_FRMRATE_TYPE_PTS = 0,     /* use frame rate from pts */
    PTS_FRMRATE_TYPE_STREAM,      /* use frame rate from stream */
    PTS_FRMRATE_TYPE_USER,        /* use frame rate set by user */
    PTS_FRMRATE_TYPE_USER_PTS,    /* use frame rate set by user until 2nd I frame, then use frame rate from pts */
    PTS_FRMRATE_TYPE_MAX
} vfmw_pts_framrate_type;

typedef enum {
    VFMW_USD_INVALID = 0,
    VFMW_USD_MP2SEQ,
    VFMW_USD_MP2GOP,
    VFMW_USD_MP2PIC,
    VFMW_USD_MP4VSOS,
    VFMW_USD_MP4VSO,
    VFMW_USD_MP4VOL,
    VFMW_USD_MP4GOP,
    VFMW_USD_H264_REG,
    VFMW_USD_H264_UNREG,
    VFMW_USD_AVSSEQ,
    VFMW_USD_AVSPIC,
    VFMW_USD_MAX
} vfmw_usd_type;

typedef enum {
    EVNT_START             = 0,
    EVNT_NEW_IMAGE         = 1 << 0,
    EVNT_USRDAT            = 1 << 1,
    EVNT_FIND_IFRAME       = 1 << 2,
    EVNT_VDM_ERR           = 1 << 3,
    EVNT_UNSUPPORT         = 1 << 4,
    EVNT_SE_ERR            = 1 << 5,
    EVNT_MEET_NEWFRM       = 1 << 6,
    EVNT_DECSYNTAX_ERR     = 1 << 7,
    EVNT_LAST_FRAME        = 1 << 8,
    EVNT_RESOLUTION_CHANGE = 1 << 9,
    EVNT_NEED_ARRANGE      = 1 << 10,
    EVNT_UNSUPPORT_SPEC    = 1 << 11,
    EVNT_FAKE_FRAME        = 1 << 12,
    EVNT_VIDSTD_ERROR      = 1 << 13,
    EVNT_ERR_FRAME         = 1 << 14,
    EVNT_ALLOC_MEM         = 1 << 15,
    EVNT_RELEASE_MEM       = 1 << 16,
    EVNT_RELEASE_BURST_MEM = 1 << 17,
    EVNT_CANCEL_ALLOC      = 1 << 18,
    EVNT_SET_ADDRESS_IN    = 1 << 19,
    EVNT_NEW_YUV           = 1 << 20,
    EVNT_RLS_STREAM        = 1 << 21,
    EVNT_RECEIVE_FAILED    = 1 << 22,
    EVNT_FIRST_PTS         = 1 << 23,
    EVNT_SECOND_PTS        = 1 << 24,
    EVNT_NORM_CHG          = 1 << 25,
    EVNT_FRM_PCK_CHG       = 1 << 26,
    EVNT_HDR_INFO          = 1 << 27,

    EVNT_MAX               = 1 << 30
} vfmw_evnt_type;

typedef enum {
    VFMW_SPEC_BIT_DEPTH = 0,    /* unsupport bit depth */
    VFMW_SPEC_MAX
} vfmw_unsupport_spec;

typedef enum {
    SPYCbCr420 = 0,
    SPYCbCr422_1X2,
    SPYCbCr422_2X1,
    SPYCbCr444,
    SPYCbCr400,
    SPYCbCr411,
    PLNYCbCr400,
    PLNYCbCr411,
    PLNYCbCr420,
    PLNYCbCr422_1X2,
    PLNYCbCr422_2X1,
    PLNYCbCr444,
    PLNYCbCr410,
    VFMW_YUV_FMT_MAX
} vfmw_yuv_fmt;

typedef union {
    struct {
        hi_s32 is_adv_profile;
        hi_s32 codec_version;
    } vc1_ext;

    struct {
        hi_s32 reversed;
        hi_u32 disp_width;
        hi_u32 disp_height;
    } vp6_ext;
} vfmw_std_ext;

typedef struct {
    hi_u8  is_seek_pending;
    hi_u32 flags;
    hi_u32 buf_len;
    hi_u32 cfg_width;
    hi_u32 cfg_height;
} vfmw_raw_ext;

typedef struct {
    hi_u8  is_available;
    hi_u8  full_rangeFlag;
    hi_u8  colour_primaries;
    hi_u8  matrix_coeffs;
    hi_u32 transfer_characteristics;
} vfmw_color_desc;

typedef struct {
    hi_s64  handle;
    hi_u64  dma_buf;
    hi_u64  offset;
    UADDR   phy_addr;
    hi_u64  kern_vir_addr;
    hi_u64  usr_vir_addr;
    hi_u32  length;
    hi_s64  pts;
    hi_bool is_end_of_frm;
    hi_bool is_dual_layer;
    hi_u32  index;
    hi_u32  marker;
    /* pvr info */
    hi_u32  disp_enable_flag;
    hi_u32  disp_frm_distance;
    hi_u32  distance_before_first_frame;
    hi_u32  gop_num;
    hi_u64  disp_time;
} vfmw_stream_buf;

typedef struct {
    hi_u8  valid;
    hi_u8  is_not_last_packet_flag;
    hi_u8  is_stream_end_flag;
    hi_u8  is_extern_eop_flag;
    UADDR  phy_addr;
    hi_u32 length;
    hi_u32 index;
    hi_u32 discontinue_count;
    hi_u32 disp_enable_flag;
    hi_u32 disp_frame_distance;
    hi_u32 distance_before_first_frame;
    hi_u32 gop_num;
    hi_u64 vir_addr;
    hi_u64 user_tag;
    hi_u64 disp_time;
    hi_s64 pts;
    hi_u64 time_stamp;
    vfmw_raw_ext  vfmw_raw_ext;
    hi_u8  dv_dual_layer; /* 0: SL 1:DL */
} vfmw_stream_data;

typedef struct {
    hi_s32 stream_provider_inst_id;
    hi_s32 (*read_stream)(hi_s32 inst_id, vfmw_stream_data *packet);
    hi_s32 (*release_stream)(hi_s32 inst_id, vfmw_stream_data *packet);
} vfmw_stream_intf;

typedef struct {
    hi_u32 idr_flag;               /* IDR frame Flag, 1 means IDR frame */
    hi_u32 b_frm_ref_flag;         /* whether B frame is refer frame, 1 means B frame is refer frame */
    hi_u32 continuous_flag;        /* whether send frame is continusous. 1 means continusous */
    hi_u32 backward_optimize_flag; /* Backward optimize flag, 1 means optimize the backward fast play performance */
    hi_u32 disp_pptimize_flag;     /* Display optimize flag, 1 means optimize the VO display performance */
} vfmw_control_info;

typedef struct {
    hi_u8   sec_flag;
    hi_char buf_name[VFMW_REPORT_MAX_NAME_LEN];
    hi_u64  dma_buf;
    hi_s64  mem_fd;
    UADDR   phy_addr;
    hi_u64  vir_addr;
    hi_bool is_map;
    hi_bool is_cache;
    hi_u32  size;
    hi_s64  unique_id;
    hi_u32  align_size;
    hi_u32  union_num;
    hi_u64  sec_info;
} vfmw_mem_report;

typedef struct {
    vfmw_mem_report ext_frm_buf;
    vfmw_mem_report ext_info_buf;
    vfmw_mem_report ext_hfbc_buf;
    vfmw_ext_buf_status status;
} vfmw_ext_buf;

typedef struct {
    vfmw_vid_std vid_std;
    vfmw_std_ext std_ext;
    hi_u32 vdec_handle;
    hi_s32 chan_priority;
    vfmw_dec_mode dec_mode;
    hi_s32 dec_order_output;
    vfmw_bit_depth_attr en_bit_depth_cfg;
    hi_s32 chan_err_thr;
    hi_u8  sec_flag;
    hi_s8  is_miracast_chan;
    hi_s32 is_omx_path;
    hi_s32 is_tunneled_path;
    hi_s8  android_test;
    vfmw_cmp v_cmp_mode;
    hi_u32 y_comp_ratio;
    hi_u32 uv_comp_ratio;
    hi_s32 support_all_p;
    hi_u32 poc_threshold;
    hi_s32 module_lowly_enable;
    hi_s32 chan_lowly_enable;
    hi_s8  uvmos_enable;
    hi_bool sample_frd_enable;
    hi_s32 sed_only_enable;
    hi_s32 crc_mode;
    vfmw_source source;
    hi_u32 usd_pool_size;
    vfmw_frm_alloc_type alloc_type;
    hi_s32 extra_frame_store_num;

    hi_s32 max_raw_packet_num;
    hi_s32 max_raw_packet_size;
    hi_s32 max_width;
    hi_s32 max_height;
    hi_u32 max_core_num;
    hi_s32 max_slice_num;
    hi_s32 max_vps_num;
    hi_s32 max_sps_num;
    hi_s32 max_pps_num;
    hi_s32 max_ref_frame_num;

    hi_s8  be_hdr_chan;
    hi_bool dv_compatible;
    vfmw_dv_codec_cap dv_capacity;
    vfmw_dv_ves_imp   ves_imp;
    vfmw_dv_layer_imp layer_imp;

    hi_u64 private;
} vfmw_chan_cfg;

typedef struct {
    hi_u64 dma_buf;
    UADDR  phy_addr;
    UADDR  mmu_addr;
    hi_u64 vir_addr;
    hi_s32 length;
    vfmw_mem_mode mode;
    hi_handle ssm_handle;
    hi_handle vdec_handle;
} vfmw_mem_desc;

typedef struct {
    hi_s32 keep_bs;        /* keep bs in the scd buffer */
    hi_s32 keep_sps_pps;   /* keep global info in ctx for seek reset, default 0 */
    hi_s32 keep_fsp;       /* keep fsp info for seek reset, default 0 */
} vfmw_chan_reset_option;

typedef struct {
    vfmw_mem_desc stream_mem;
} vfmw_start_option;

typedef struct {
    hi_u8  hal_disable;
    hi_u8  seg_bypass_enbale;
    hi_u32 print_word;
    UADDR  glb_shr_base_addr;
    hi_u32 glb_shr_mem_length;
    UADDR  chan_option_addr;
    UADDR  proc_addr;
    UADDR  common_context;
} vfmw_share_data;

typedef struct {
    UADDR  img_phy_addr;
    hi_u64 img_vir_addr;
    UADDR  evt_phy_addr;
    hi_u64 evt_vir_addr;
    UADDR  stm_phy_addr;
    hi_u64 stm_vir_addr;
    UADDR  sta_phy_addr;
    hi_u64 sta_vir_addr;
#ifdef VFMW_MDC_SUPPORT
    UADDR  share_comm_phy_addr;
    hi_u64 share_comm_vir_addr;
#else
    UADDR  cfg_phy_addr;
    hi_u64 cfg_vir_addr;
#endif
    vfmw_mem_desc chan_shr_mem_desc;
} vfmw_shr_chn_mem;

typedef struct {
    hi_u32 chan_id;
    hi_bool is_user_dec;
    hi_handle vdec_handle;
    hi_handle ssm_handle;
    vfmw_adpt_type adpt_type;
    hi_s32 seg_buf_size;
    hi_s32 max_slice_num;
    hi_s32 max_vps_num;
    hi_s32 max_sps_num;
    hi_s32 max_pps_num;
    hi_s32 max_ref_frame_num;
    hi_s32 is_sec_mode;
    vfmw_vid_std  vid_std;
    vfmw_shr_chn_mem tmp_chan_shr_mem;
    vfmw_mem_desc chan_ctx_mem;
    vfmw_mem_desc dev_hal_mem;
} vfmw_chan_option;

typedef struct {
    hi_u32 is_mem_start;
    hi_u8  data[VFMW_MAX_USD_SIZE]; /* usrdat data entity */
    hi_u8  pic_coding_type;
    hi_u8  top_field_first;
    hi_s8  is_registered;           /* for CC, valid when IsRegistered=1 */
    hi_u8  itu_t_t35_country_code;
    hi_u8  itu_t_t35_country_code_extension_byte;
    hi_u16 itu_t_t35_provider_code;
    hi_u32 pic_num_count;
    hi_u32 dnr_used_flag;          /* internal used only, ignore */
    vfmw_usd_type  from;           /* usrdat source */
    hi_u32 seq_cnt;                /* to be removed later */
    hi_u32 seq_img_cnt;
    hi_s32 data_size;              /* usrdat size, in byte */
    hi_s64 pts;                    /* pts of the frame containning the userdata */
    hi_u64 dma_buf;
    hi_u64 offset;
} vfmw_usrdat;

typedef struct {
    hi_bool is_frame_valid;
    hi_u32 width;
    hi_u32 height;
    hi_u32 bit_depth;
    hi_u32 luma_stride;
    hi_u32 chrom_stride;
    UADDR  phy_addr;
    hi_u32 line_num_size;
    hi_u64 chrom_offset;
    hi_u64 line_num_offset;
    hi_s64 pts;
    vfmw_yuv_fmt fmt;
    hi_mem_handle_t mem_handle;
    hi_mem_size_t mem_size;
} vfmw_usrdec_frame_desc;

typedef struct {
    hi_u32 fps_integer;        /* integral part of the frame rate (in frame/s) */
    hi_u32 fps_decimal;        /* fractional part (calculated to three decimal places) of the frame rate (in frame/s) */
} vfmw_pts_frmrate_value;

typedef struct {
    vfmw_pts_framrate_type en_frm_rate_type;  /* source of frame rate */
    vfmw_pts_frmrate_value st_set_frm_rate;   /* setting frame rate */
} vfmw_pts_frmrate;

typedef struct {
    hi_s32 max_chan_num;
    hi_s32 max_bit_rate;
    hi_s32 max_rame_width;
    hi_s32 max_frame_height;
    hi_s32 max_pixel_per_sec;
    vfmw_vid_std supported_std[VFMW_STD_MAX];
} vfmw_cap;

typedef struct {
    hi_s64 attached_handle;
    UADDR  attached_phy_addr;
    hi_u64 attached_vir_addr;
    hi_u32 attached_size;

    hi_s64 hfbc_handle;
    hi_u64 hfbc_vir_addr;
} vfmw_image_attached;

typedef struct {
    vfmw_vid_std std;
    hi_u32  image_width;
    hi_u32  image_height;
    hi_u32  disp_height;
    hi_u32  disp_width;
    hi_u32  disp_center_x;
    hi_u32  disp_center_y;
    hi_u32  aspect_width;
    hi_u32  aspect_height;
    hi_u32  scan_type;
    hi_u32  video_format;
    hi_u32  frame_rate;
    hi_u32  bit_rate;
    hi_u32  sub_video_standard;
    hi_u32  profile;
    hi_u32  level;
    hi_u32  bit_depth;
    vfmw_color_desc color_info;
    hi_s32  total_frame_num;
    hi_s32  error_frame_num;
    hi_s32  buffered_stream_size;   /* the un-decoded stream seg size produced by SCD */
    hi_s32  buffered_stream_num;    /* the un-decoded stream seg num  produced by SCD */
    UADDR   chan_mem_address;
    hi_s32  chan_mem_size;
    hi_s32  total_fstore_num;
    hi_s32  total_pmv_num;
    hi_s32  voque_detail;
    hi_u32  stream_not_enough;
    hi_u32  wait_disp_frame_num;
    hi_u32  mpeg4_shorthead;      /* mpeg4 short head info, 1: sh 2:not sh  0:not mpeg4 stream */
    hi_u32  total_disp_frame_num; /* total display num ( plus extra_disp ) */
    hi_u32  is_field_flg;
    hi_bool is_end_of_stm;
    hi_u32  total_stm_buf_size;
    hi_u32  free_stm_buf_size;
    hi_u32  used_stm_buf_size;
    hi_u32  undecode_stm_size;
    hi_u32  undecode_stm_num;
    hi_u32  pvr_buf_num;
    hi_u32  dec_frm_rate;
} vfmw_chan_info;

typedef struct {
    hi_u8 is_secure;
    vfmw_adpt_type adpt_type;
    vfmw_mem_desc ext_hal_mem;
    fun_vfmw_event_report fn_callback;
    vfmw_share_data share_data;
} vfmw_init_param;

typedef struct {
    hi_u32 image_id;
    hi_u32 image_id_1;
    hi_s32 gop_num;
    hi_u32 frm_type;
    hi_s32 top_field_frame_format;
    hi_s32 bot_field_frame_format;
    hi_s32 field_flag;
    hi_s32 chan_id;
    hi_mem_handle_t mtdt_mem_handle;
    vfmw_color_desc color_desc;
} vfmw_private_frm_info;

/* vfmw external interface definition */
typedef struct {
    fun_vfmw_init       fn_vfmw_init;
    fun_vfmw_exit       fn_vfmw_exit;
    fun_vfmw_suspend    fn_vfmw_suspend;
    fun_vfmw_resume     fn_vfmw_resume;
    fun_vfmw_get_frame  fn_vfmw_get_frame;
    fun_vfmw_rels_frame fn_vfmw_rels_frame;
    fun_vfmw_control    fn_vfmw_control;
} vfmw_export_fun;

#endif


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */
#include <linux/kernel.h>
#include <linux/string.h>

#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"

#include "drv_demux_define.h"
#include "drv_demux_func.h"
#include "drv_demux_index.h"
#include "drv_demux_utils.h"
#include "hi_type.h"
/* !@attention :define the following macro if you want PTS be set to I frames only */
/* #define  PUT_PTS_ON_I_FRAME_ONLY */
/* !=======================================================================
                              constants
   ======================================================================= */
#define  FIDX_MAX_CTX_NUM          DMX_REC_CNT /* maximum channel can be processed are the same as record channel */
#define  SC_SUPPORT_DATA_SIZE      16          /* ! SC support data size, 16 bytes */
#define  HEVC_SPS_SIZE             32
#define  HEVC_PPS_SIZE             256
#define  HEVC_VPS_SIZE             16

/* ! nal unit type */
#define  FIDX_NALU_TYPE_SLICE  1
#define  FIDX_NALU_TYPE_IDR    5
#define  FIDX_NALU_TYPE_SPS    7
#define  FIDX_NALU_TYPE_PPS    8

#define FIDX_HEVC_NAL_SLICE_TRAIL_N                      0
#define FIDX_HEVC_NAL_SLICE_TRAIL_R                      1
#define FIDX_HEVC_NAL_SLICE_TSA_N                         2
#define FIDX_HEVC_NAL_SLICE_TLA_R                         3
#define FIDX_HEVC_NAL_SLICE_STSA_N                       4
#define FIDX_HEVC_NAL_SLICE_STSA_R                       5
#define FIDX_HEVC_NAL_SLICE_RADL_N                       6
#define FIDX_HEVC_NAL_SLICE_RADL_R                       7
#define FIDX_HEVC_NAL_SLICE_RASL_N                        8
#define FIDX_HEVC_NAL_SLICE_RASL_R                        9

#define FIDX_HEVC_NAL_SLICE_BLA_W_LP                   16
#define FIDX_HEVC_NAL_SLICE_BLA_W_RADL              17
#define FIDX_HEVC_NAL_SLICE_BLA_N_LP                    18
#define FIDX_HEVC_NAL_SLICE_IDR_W_RADL               19
#define FIDX_HEVC_NAL_SLICE_IDR_N_LP                     20
#define FIDX_HEVC_NAL_SLICE_CRA                              21
#define FIDX_HEVC_NAL_RESERVED_IRAP_VCL22        22
#define FIDX_HEVC_NAL_RESERVED_IRAP_VCL23        23

#define FIDX_HEVC_NAL_VPS               32
#define FIDX_HEVC_NAL_SPS               33
#define FIDX_HEVC_NAL_PPS               34
#define FIDX_HEVC_NAL_PRE_SEI        39
#define FIDX_HEVC_NAL_SUF_SEI        40

#define FIDX_HEVC_I_SLICE                   2
#define FIDX_HEVC_P_SLICE                 1
#define FIDX_HEVC_B_SLICE                 0

/*!=======================================================================
                               data structures
  =======================================================================*/
/*! start code type */
typedef enum hi_sc_types {
    SC_TYPE_UNKNOWN = 0,
    SC_TYPE_SPS,
    SC_TYPE_PPS,
    SC_TYPE_PIC,
    SC_TYPE_SLICE,
    SC_TYPE_NONSLICE,
    SC_TYPE_VPS,
    SC_TYPE_BUTT
} sc_types;

/*! start code description */
typedef struct hi_sc_info {
    sc_types     sc_type;
    hi_s32        sc_id;           /*! for H.264, assign PPS or SPS ID; for non-h.264, assign the byte after 00 00 01 */
    hi_s32        sup_sc_id;        /*! for H.264 only, record the SPS ID for the current used PPS */
    hi_s64        global_offset;   /*! the offset of the start code, in the global(whole) stream data space */
    hi_s32        offset_in_packet; /*! the offset of the start code, in the stream data packet */
    hi_s32        packet_count;    /*! the stream data packet number where the start code was found */
    hi_s32        profile_id;

    /* VPS */
    /* SPS */
    hi_s32      max_cu_depth;
    hi_s32      max_cu_width;
    hi_s32      max_cu_height;
    hi_s32      pic_width_in_luma_samples;
    hi_s32      pic_height_in_luma_samples;
    /* PPS */
    hi_s32      seq_parameter_set_id;
    hi_s32      dependent_slice_segments_enabled_flag;
    hi_s32      num_extra_slice_header_bits;
} sc_info;

/*! state of the instance */
typedef enum hi_ctx_states {
    CTX_STATE_DISABLE = 0,
    CTX_STATE_ENABLE,
    CTX_STATE_BUTT
} ctx_states;


/*! context */
typedef struct hi_fidx_ctx {
    ctx_states  ctx_state;
    vidstd      video_standard;  /* ! video standard type */
    strm_type   strm_type;       /* ! stream type, ES or PES */
    hi_s64      pts;          /* ! current PTS, usually equals to the pts of the latest stream packet */
    sc_info     sps[HEVC_SPS_SIZE];       /* ! start code of the sequence level parameters.
                                H264  - sps
                                MPEG2 - sequence header
                                AVS   - sequence header
                                MPEG4 - VOL or higher */
    /* !one SPS can be used by one I frame only, an I frame without SPS will be treated as P or B frame */
    hi_u8       sps_fresh[HEVC_SPS_SIZE];
    sc_info     pps[HEVC_PPS_SIZE];      /* ! picture level parameter
                                H264  - pps
                                MPEG2 - picture header
                                AVS   - picture header
                                MPEG4 - VOP header */

    /* previous 2 bytes, for start code detection, to prevent the 00 or 00 00 lost */
    hi_u8       prev2bytes[2]; /* ! store the latest 2 byte */

    /* this SC store the latest tetected start code */
    hi_s32      this_scvalid;  /* ! indicate the support data of this start code is ready for use */
    sc_info     this_sc; /* ! when a start code was found, the support data is probably not enough. if so, this start */
    hi_u8       this_scdata[SC_SUPPORT_DATA_SIZE]; /* ! has to be stored temporarily to wait more data */
    hi_s32      this_scdata_len;  /*! actual support data size, usually equals to SC_SUPPORT_DATA_SIZE */
    /* ! record the SEI start code followed the last slice of the previous picture.
     * generally this SEI is the start of a new picture
     */
    sc_info     seifollow_slice;

    /* frame,a set of frame info probably can be generated after 'this SC' was processed */
    frame_pos   new_frame_pos;     /* ! frame info to be output, temporarily stored here */
    hi_s32      wait_frame_size;  /* ! indicate if most info of new_frame_pos has been ready, except the frame size */
    hi_s32      sps_id;          /* ! H264: SPS ID */
    hi_s32      pps_id;          /* ! H264: PPS ID */
    hi_u32      *param;

    /* HEVC_ENABLE */
    hi_u32      next_pts;
    sc_info     vps[HEVC_VPS_SIZE];
    hi_s32      vps_sps_pps_err;
    hi_s32      is_ref_idc;

    hi_s64      new_frm_offset;
    hi_s64      last_vps_offset;
    hi_s64      last_sps_offset;
    hi_s64      last_pps_offset;
    hi_s64      last_sei_offset;
    hi_s64      first_nal_offset;

    hevc_ctx_info  *hevc_ctx;
    hi_u8       *hevc_scdata;   /* hevc special buffer, refer to this_scdata. */
} fidx_ctx;


/*!=======================================================================
                           static shared data
  =======================================================================*/
static fidx_ctx  *g_fidx_iis = HI_NULL;


/*=======================================================================
                           function declaration
  =======================================================================*/
static hi_s32 process_this_sc(hi_s32 inst_idx);
static hi_s32 process_sc_mpeg2(hi_s32 inst_idx);
static hi_s32 process_sc_avs(hi_s32 inst_idx);
static hi_s32 process_sc_mpeg4(hi_s32 inst_idx);
static hi_s32 process_sc_h264(hi_s32 inst_idx);
//#ifdef HEVC_ENABLE
static hi_s32 process_sc_hevc(hi_s32 inst_idx);
//#endif

/*!=======================================================================
                             macros
  =======================================================================*/
/*! print */
#define PRINT   HI_ERR_DEMUX

/*! assertion */
#define FIDX_ASSERT_RET(cond, else_print) do {                          \
    if(!(cond)) {                                                       \
        PRINT("pvr_index.c,l%d: %s\n", __LINE__, else_print);           \
        return FIDX_ERR;                                                \
    }                                                                   \
} while (0)

#define FIDX_ASSERT(cond, else_print) do {                             \
    if(!(cond)) {                                                      \
        PRINT("pvr_index.c,l%d: %s\n", __LINE__, else_print);          \
        return;                                                        \
    }                                                                  \
} while (0)

/*! fill support data of the SC */
#define FILL_SC_DATA(ptr, len) do {                                            \
    if(ctx->this_scdata_len + (len) >= SC_SUPPORT_DATA_SIZE) {                 \
        memcpy(ctx->this_scdata + ctx->this_scdata_len, ((const void *)(ptr)), \
               (size_t)(SC_SUPPORT_DATA_SIZE - ctx->this_scdata_len));         \
        ctx->this_scdata_len = SC_SUPPORT_DATA_SIZE;                           \
    } else {                                                                   \
        memcpy(ctx->this_scdata + ctx->this_scdata_len, ((const void *)(ptr)), \
              ((size_t)(len));                                                 \
        ctx->this_scdata_len += (len);                                         \
    }                                                                          \
} while (0)

/*! decide if the SC is valid */
#define IS_SC_WRONG()   \
(ctx->this_scdata_len < 3 ||   \
(ctx->this_scdata[0] == 0x00 && ctx->this_scdata[1]==0x00 && ctx->this_scdata[2] == 0x01))

/*! mpeg4 have multi-level sequence parameter, select the highest level */
#define SELECT_SPS(sps, sel_sps) do {                              \
    if((sps).sc_type == SC_TYPE_SPS &&                             \
       (sps).global_offset < (sel_sps)->global_offset &&           \
       (sps).global_offset + 64 >= (sel_sps)->global_offset) {     \
        (sel_sps) = &(sps);                                         \
    }                                                              \
} while (0)

#define BITS_2_BYTES(bitn)   (((bitn) + (7))>>(3))

/*!=======================================================================
                       function implementation
  =======================================================================*/
/*!===============demux index bitstream begin====================================*/
#ifndef FIDX_BIG_ENDIAN

#define  ENDIAN32(x)                \
    ((x)<<24) |                     \
    (((x) & 0x0000ff00) << 8) |     \
    (((x) & 0x00ff0000) >> 8) |     \
    (((x) >> 24) & 0x000000ff)      \

#else
#define  ENDIAN32(x)      (x)

#endif

static hi_u8 g_calc_zero_num[256] = { /* zero num is 256 */
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static hi_u32 zeros_ms_32(hi_u32 data)
{
    hi_u32 i;
    hi_u32 zero_num = 0;
    hi_u32 tmp_zero_num = 0;
    hi_u32 tmpdata = 0;

    for (i = 4; i > 0; i--) { /* 4 loops */
        tmpdata = (data & 0xff000000) >> 24; /* Shift right by 24 bits to get higher 8 bits */
        tmp_zero_num = (hi_u32)g_calc_zero_num[tmpdata];
        zero_num += tmp_zero_num;
        if (tmp_zero_num != 8) { /* tmp_zero_num not equal to 8 */
            break;
        }
        data = data << 8; /* data shift 8 bits to the left */
    }

    return zero_num;
}

/******************************************************/
static hi_void bs_init(BS *bs, hi_u8 *input, hi_s32 length)
{
    hi_u32 data;
    unsigned long align_word = 0x3;

    bs->p_head    = (hi_u8 *)(((unsigned long)input) & (~align_word));
    bs->p_tail    = bs->p_head + 0x8;
    bs->bs_len    = length;

    data = *(hi_u32 *)bs->p_head;
    bs->bufa     = ENDIAN32(data);
    data = *(hi_u32 *)(bs->p_head + 0x4);
    bs->bufb     = ENDIAN32(data);

    bs->buf_pos   = (((unsigned long)input) & 0x3) << 3; /* shift 3 bits to the left */
    bs->total_pos = 0;

    return;
}

/******************************************************/
static hi_s32 bs_pos(BS *bs)
{
    return (bs->total_pos);
}

/******************************************************/
static hi_s32 bs_show(BS *bs, hi_s32 n_bits)
{
    hi_s32 abbuf_pos = n_bits + bs->buf_pos;
    hi_u32 data, data1;

    if (abbuf_pos > 32) { /* abbuf_pos greater 32 */
        data  = bs->bufa << bs->buf_pos;   /* ( abbuf_pos - 32 ); */
        data1 = bs->bufb >> (32 - bs->buf_pos); /* 32 minus buf_pos */
        data |= data1;
        data >>= (32 - n_bits); /* 32 minus n_bits */
    } else {
        data  = (bs->bufa << bs->buf_pos) >> (32 - n_bits); /* 32 minus n_bits */
    }

    return (data);
}

/******************************************************/
static hi_s32 bs_skip(BS *bs, hi_s32 n_bits)
{
    hi_s32 abbuf_pos = n_bits + bs->buf_pos;
    hi_u32 data1;

    bs->total_pos += n_bits;

    if (abbuf_pos >= 32) { /* abbuf_pos greater 32 */
        bs->buf_pos   =  abbuf_pos - 32; /* abbuf_pos minus 32 */

        bs->bufa = bs->bufb;
        data1 = *(hi_u32 *)bs->p_tail;
        bs->bufb = ENDIAN32(data1);

        bs->p_tail += 0x4;
    } else {
        bs->buf_pos   += n_bits;
    }

    return (n_bits);
}

/******************************************************/
static hi_s32 bs_get(BS *bs, hi_s32 n_bits)
{
    hi_u32 data;

    data = bs_show(bs, n_bits);
    bs_skip(bs, n_bits);

    return (data);
}

/******************************************************/
static hi_s32 bs_back(BS *bs, hi_s32 n_bits)
{
    hi_s32 pred_buf_pos;
    hi_u32 data1;

    if (n_bits > bs->total_pos) {
        n_bits = bs->total_pos;
    }

    pred_buf_pos     = (hi_s32)bs->buf_pos - n_bits;
    bs->total_pos -= n_bits;

    if (pred_buf_pos >= 0) {
        bs->buf_pos = pred_buf_pos;
    } else {
        bs->p_tail  -= 0x4;
        bs->bufb   = bs->bufa;
        data1 = *(hi_u32 *)(bs->p_tail - 0x8);
        bs->bufa = ENDIAN32(data1);

        bs->buf_pos = pred_buf_pos + 32; /* add 32 */
    }
    return (n_bits);
}

static inline hi_s32 bs_bits_to_next_byte(BS *bs)
{
    int  skip_len, align_pos;
    align_pos = (bs->total_pos + 0x7) & 0xfffffff8;
    skip_len  = align_pos - bs->total_pos;
    return skip_len;
}


/******************************************************/
static inline hi_s32 bs_to_next_byte(BS *bs)
{
    int  skip_len, align_pos;
    align_pos = (bs->total_pos + 0x7) & 0xfffffff8;
    skip_len  = align_pos - bs->total_pos;

    bs_skip(bs, skip_len);
    return skip_len;
}

/******************************************************/
static inline hi_s32 bs_resid_bits(BS *bs)
{
    return (0x8 * bs->bs_len - bs->total_pos);
}

/******************************************************/
static inline hi_s32 bs_is_byte_aligned(BS *bs)
{
    if (bs->total_pos & 0x7) {
        return (0);
    } else {
        return (1);
    }
}

/******************************************************/
static inline hi_s32 bs_next_bits_byte_aligned(BS *bs, hi_s32 n_bits)
{
    /* 'n_bits' should <= 24, otherwise may be wrong. */
    hi_u32 bits_to_byte, data;

    bits_to_byte = 0x8 - (bs->total_pos & 0x7);

    bs_skip(bs, bits_to_byte);
    data = bs_show(bs, n_bits);
    bs_back(bs, bits_to_byte);

    return (data);
}

static inline hi_u8 *bs_get_next_byte_ptr(BS *bs)
{
    hi_u8 *ptr;
    hi_s32 bits_in_bufa;

    bits_in_bufa = (32 - bs->buf_pos); /* 32 minus buf_pos */
    ptr = ((hi_u8 *)(bs->p_tail)) - (0x4 + bits_in_bufa / 0x8);

    return (ptr);
}
/*!===============demux index bitstream end=====================================*/
/*!***********************************************************************
    @brief global init, clear context, and register call back
 ************************************************************************/
hi_void fidx_init()
{
    hi_s32 i;

    g_fidx_iis = HI_KZALLOC(HI_ID_DEMUX, FIDX_MAX_CTX_NUM * sizeof(fidx_ctx), GFP_KERNEL);
    if (!g_fidx_iis) {
        WARN(1, "malloc fidx ctx failed.\n");
    } else {
        for (i = 0; i < FIDX_MAX_CTX_NUM; i++) {
            g_fidx_iis[i].ctx_state = CTX_STATE_DISABLE;
        }
    }
}

hi_void fidx_de_init(hi_void)
{
    if (g_fidx_iis) {
        HI_KFREE(HI_ID_DEMUX, g_fidx_iis);
        g_fidx_iis = HI_NULL;
    }
}

/*!***********************************************************************
    @brief  open an instance
    @param[in]  VidStandard: video standard
    @return
        if success, return instance ID, 0~(FIDX_MAX_CTX_NUM-1)
        if fail, return -1
 ************************************************************************/
hi_s32  fidx_open_instance(vidstd vid_standard, strm_type strm_type, hi_u32 *param)
{
    hi_s32  ret = -1;
    hi_s32  i;

    FIDX_ASSERT_RET(vid_standard < VIDSTD_BUTT, "'vid_standard' out of range");
    FIDX_ASSERT_RET(strm_type < STRM_TYPE_BUTT, "'strm_type' out of range");

    /*! find an idle instance */
    for (i = 0; i < FIDX_MAX_CTX_NUM; i++) {
        if (g_fidx_iis[i].ctx_state != CTX_STATE_ENABLE) {
            memset(&g_fidx_iis[i], 0, sizeof(fidx_ctx));

            g_fidx_iis[i].ctx_state          = CTX_STATE_ENABLE;
            g_fidx_iis[i].video_standard     = vid_standard;
            g_fidx_iis[i].strm_type          = strm_type;
            g_fidx_iis[i].prev2bytes[0]    = 0xff;
            g_fidx_iis[i].prev2bytes[1]    = 0xff;
            g_fidx_iis[i].param              = param;

            g_fidx_iis[i].vps_sps_pps_err = 0;
            g_fidx_iis[i].new_frm_offset = -1;
            g_fidx_iis[i].first_nal_offset = -1;
            g_fidx_iis[i].last_vps_offset = -1;
            g_fidx_iis[i].last_sps_offset = -1;
            g_fidx_iis[i].last_pps_offset = -1;
            g_fidx_iis[i].last_sei_offset = -1;
            g_fidx_iis[i].hevc_ctx = HI_NULL;

            if (VIDSTD_HEVC == vid_standard) {
                g_fidx_iis[i].hevc_ctx = HI_VMALLOC(HI_ID_DEMUX, sizeof(hevc_ctx_info));
                if (!g_fidx_iis[i].hevc_ctx) {
                    HI_ERR_DEMUX("malloc hevc ctx failed.\n");
                    break;
                }

                dmx_hevc_init(g_fidx_iis[i].hevc_ctx);
            }

            ret = i;

            break;
        }
    }

    return ret;
}

/*!***********************************************************************
    @brief   close instalce
 ************************************************************************/
hi_s32  fidx_close_instance(hi_s32 inst_idx)
{
    FIDX_ASSERT_RET(inst_idx < FIDX_MAX_CTX_NUM, "inst_idx out of range");

    if (g_fidx_iis[inst_idx].ctx_state != CTX_STATE_ENABLE) {
        return FIDX_ERR;
    } else {
        if (g_fidx_iis[inst_idx].hevc_ctx) {
            HI_VFREE(HI_ID_DEMUX, g_fidx_iis[inst_idx].hevc_ctx);
        }

        memset(&g_fidx_iis[inst_idx], 0, sizeof(fidx_ctx));
        g_fidx_iis[inst_idx].ctx_state = CTX_STATE_DISABLE;

        return FIDX_OK;
    }
}

static hi_s32 is_pes_sc(hi_u8 code, vidstd vid_std)
{
    hi_s32 ret = 0;

    if (vid_std != VIDSTD_AUDIO_PES) {
        if (code >= 0xe0 && code <= 0xef) {
            ret = 1;
        }
    } else {
        if (code >= 0xc0 && code <= 0xdf) {
            ret = 1;
        }
    }

    return ret;
}

/*!***********************************************************************
@brief  entry of the start code process
@return
  if success return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
hi_s32 process_this_sc(hi_s32 inst_idx)
{
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    hi_s32 ret = FIDX_ERR;

    switch (ctx->video_standard) {
        case VIDSTD_MPEG2:
            ret = process_sc_mpeg2(inst_idx);
            break;
        case VIDSTD_H264:
            ret = process_sc_h264(inst_idx);
            break;
        case VIDSTD_MPEG4:
            ret = process_sc_mpeg4(inst_idx);
            break;
        case VIDSTD_AVS:
            ret = process_sc_avs(inst_idx);
            break;
        case VIDSTD_HEVC:
            ret = process_sc_hevc(inst_idx);
            break;
        default:
            ret = FIDX_ERR;
            break;
    }

    return ret;
}


/*!***********************************************************************
@brief
    process mpeg2 start code
@return
  if success return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
hi_s32 process_sc_mpeg2(hi_s32 inst_idx)
{
    hi_u8 code;
    hi_s32 ret = FIDX_ERR;
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    sc_info   *sps_sc = &ctx->sps[0];
    sc_info   *this_sc = &ctx->this_sc;
    frame_pos *frm_pos = &ctx->new_frame_pos;

    FIDX_ASSERT_RET(0 != ctx->this_scvalid, "this_sc is not valid\n");
    FIDX_ASSERT_RET(!IS_SC_WRONG(), "not enough data for this_sc\n");

    code = ctx->this_scdata[0];

    /* if a frame is found, but its size have not been calculated, it is time to calc the frame size,
     * and output this frame
     */
    if (frm_pos->frame_type != FIDX_FRAME_TYPE_UNKNOWN) {
        /*! this means end of the formal frame found, the frame size can be calculated */
        if (code == 0xb3 || code == 0x00) {
            frm_pos->frame_size = (hi_s32)(this_sc->global_offset - frm_pos->global_offset);
            if (frm_pos->frame_size < 0) {
                /*! just incase */
                frm_pos->frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            } else {
                dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            }
        }
    }

    if (code == 0xb3) { /*! sequence header */
        this_sc->sc_id = (hi_s32)code;
        this_sc->sc_type = SC_TYPE_SPS;
        memcpy(sps_sc, this_sc, sizeof(sc_info));
        ctx->sps_fresh[0] = 1;
        ret = FIDX_ERR;
    } else if (code == 0x00) { /*! picture header */
        if (sps_sc->sc_type != SC_TYPE_SPS) { /*! seqence parameters still not ready */
            ret = FIDX_ERR;
        } else {
            hi_u8 picture_coding_type;

            picture_coding_type = (ctx->this_scdata[0x2] >> 0x3) & 0x7;
            /*
             * FIDX_ASSERT_RET(picture_coding_type>=1 && picture_coding_type<=3,
             * "MPEG2 picture_coding_type out of range");
             */
            if ((picture_coding_type < 1) || (picture_coding_type > 0x3)) {
                return FIDX_ERR;
            }
            if (picture_coding_type == 1 && ctx->sps_fresh[0] == 1) { /*! I frame */
                frm_pos->frame_type        = FIDX_FRAME_TYPE_I;
                frm_pos->global_offset   = sps_sc->global_offset;
                frm_pos->offset_in_packet = sps_sc->offset_in_packet;
                frm_pos->packet_count    = sps_sc->packet_count;
                ctx->sps_fresh[0] = 0;
                frm_pos->pts            = ctx->pts;
                ctx->pts               = DMX_INVALID_PTS;
            } else { /*! P or B frame */
                frm_pos->frame_type = (picture_coding_type == 0x3) ? FIDX_FRAME_TYPE_B : FIDX_FRAME_TYPE_P;
                frm_pos->global_offset   = this_sc->global_offset;
                frm_pos->offset_in_packet = this_sc->offset_in_packet;
                frm_pos->packet_count    = this_sc->packet_count;
#ifndef PUT_PTS_ON_I_FRAME_ONLY
                frm_pos->pts            = ctx->pts;
                ctx->pts               = DMX_INVALID_PTS;
#endif
            }
            frm_pos->frame_size = 0; /*! frame size need the position of the next start code */
            ret = FIDX_OK;
        }
    }

    return ret;
}

/*!***********************************************************************
@brief
  process the start code of AVS
@return
  if success(find the start of a frame, and can output one index record)
  return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
hi_s32 process_sc_avs(hi_s32 inst_idx)
{
    hi_u8 code;
    hi_s32 ret = FIDX_ERR;
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    sc_info   *sps_sc = &ctx->sps[0];
    sc_info   *this_sc = &ctx->this_sc;
    frame_pos *frm_pos = &ctx->new_frame_pos;

    FIDX_ASSERT_RET(0 != ctx->this_scvalid, "this_sc is not valid\n");
    FIDX_ASSERT_RET(!IS_SC_WRONG(), "not enough data for this_sc\n");

    code = ctx->this_scdata[0];

    /* if a frame is found, but its size have not been calculated, it is time to calc the frame size,
     * and output this frame
     */
    if (frm_pos->frame_type != FIDX_FRAME_TYPE_UNKNOWN) {
        /*! this means end of the formal frame found, the frame size can be calculated */
        if (code == 0xb0 || code == 0xb3 || code == 0xb6) {
            frm_pos->frame_size = (hi_s32)(this_sc->global_offset - frm_pos->global_offset);
            if (frm_pos->frame_size < 0) {
                /*! just incase */
                frm_pos->frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            } else {
                dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            }
        }
    }

    if (code == 0xb0) { /* sequence header */
        this_sc->sc_id = (hi_s32)code;
        this_sc->sc_type = SC_TYPE_SPS;
        this_sc->profile_id = ctx->this_scdata[1];
        memcpy(sps_sc, this_sc, sizeof(sc_info));
        ctx->sps_fresh[0] = 1;
        ret = FIDX_ERR;
    } else if (code == 0xb3 || code == 0xb6) { /* picture header */
        if (sps_sc->sc_type != SC_TYPE_SPS) { /*! seqence parameters still not ready */
            ret = FIDX_ERR;
        } else {
            if (code == 0xb3 && ctx->sps_fresh[0] == 1) { /*! I frame */
                frm_pos->frame_type = FIDX_FRAME_TYPE_I;
                frm_pos->global_offset = sps_sc->global_offset;
                frm_pos->offset_in_packet = sps_sc->offset_in_packet;
                frm_pos->packet_count    = sps_sc->packet_count;

                frm_pos->pts            = ctx->pts;
                ctx->pts               = DMX_INVALID_PTS;
            } else { /*! P or B frame. the I frame without SPS before head also treated as P frame */
                hi_u8 picture_coding_type = 1;
                if (code == 0xb6) {
                    if (this_sc->profile_id == 0x48) {
                        picture_coding_type = (ctx->this_scdata[0x4] >> 0x6) & 0x3;
                    } else {
                        picture_coding_type = (ctx->this_scdata[0x3] >> 0x6) & 0x3;
                    }
                    FIDX_ASSERT_RET(picture_coding_type == 1 || picture_coding_type == 0x2,
                                    "AVS picture_coding_type out of range");
                }

                frm_pos->frame_type = (picture_coding_type == 1) ? FIDX_FRAME_TYPE_P : FIDX_FRAME_TYPE_B;
                frm_pos->global_offset = this_sc->global_offset;
                frm_pos->offset_in_packet = this_sc->offset_in_packet;
                frm_pos->packet_count    = this_sc->packet_count;

#ifndef PUT_PTS_ON_I_FRAME_ONLY
                frm_pos->pts            = ctx->pts;
                ctx->pts               = DMX_INVALID_PTS;
#endif
            }
            frm_pos->frame_size = 0; /*! frame size need the position of the next start code */
            ret = FIDX_OK;
        }
    }

    return ret;
}

/************************************************************************
@brief
  process the start code of MPEG4
@return
  if success(find the start of a frame, and can output one index record)
  return FIDX_OK, otherwise return FIDX_ERR
************************************************************************/
hi_s32 process_sc_mpeg4(hi_s32 inst_idx)
{
    hi_u8 code;
    hi_s32 ret = FIDX_ERR;
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    sc_info   *sps_sc = &ctx->sps[0];
    sc_info   *this_sc = &ctx->this_sc;
    frame_pos *frm_pos = &ctx->new_frame_pos;

    FIDX_ASSERT_RET(0 != ctx->this_scvalid, "this_sc is not valid\n");
    FIDX_ASSERT_RET(!IS_SC_WRONG(), "not enough data for this_sc\n");

    code = ctx->this_scdata[0];

    /* if a frame is found, but its size have not been calculated, it is time to calc the frame size,
     * and output this frame
     */
    if (frm_pos->frame_type != FIDX_FRAME_TYPE_UNKNOWN) {
        if (code != 0xb2) { /*! this means end of the formal frame found, the frame size can be calculated */
            frm_pos->frame_size = (hi_s32)(this_sc->global_offset - frm_pos->global_offset);
            if (frm_pos->frame_size < 0) {
                /*! just incase */
                frm_pos->frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            } else {
                dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            }
        }
    }

    /*! distribution of the sequence level parameters:pstCtx->ThisSCData[x]
       [0]: B0, Visual_Object_Sequence
       [1]: b5, Visual_Object
       [2]: 00~1f, Video_Object
       [3]: 20~2f, Video_Object_Layer
    */
    if (code == 0xb0) { /* visual_object_sequence */
        this_sc->sc_id = (hi_s32)code;
        this_sc->sc_type = SC_TYPE_SPS;
        memcpy(&sps_sc[0], this_sc, sizeof(sc_info));
        ret = FIDX_ERR;
    } else if (code == 0xb5) { /* b5, visual_object */
        this_sc->sc_id = (hi_s32)code;
        this_sc->sc_type = SC_TYPE_SPS;
        memcpy(&sps_sc[1], this_sc, sizeof(sc_info));
        ret = FIDX_ERR;
    } else if (code <= 0x1f) { /* video_object: 0x00 ~ 0x1f */
        this_sc->sc_id = (hi_s32)code;
        this_sc->sc_type = SC_TYPE_SPS;
        memcpy(&sps_sc[0x2], this_sc, sizeof(sc_info));
        ret = FIDX_ERR;
    } else if (code >= 0x20 && code <= 0x2f) { /* video_object_layer */
        this_sc->sc_id = (hi_s32)code;
        this_sc->sc_type = SC_TYPE_SPS;
        memcpy(&sps_sc[0x3], this_sc, sizeof(sc_info));
        ctx->sps_fresh[0x3] = 1;
        ret = FIDX_ERR;
    } else if (code == 0xb6) { /* VOP: picture header */
        if (sps_sc[0x3].sc_type != SC_TYPE_SPS) { /*! seqence parameters still not ready */
            ret = FIDX_ERR;
        } else {
            hi_u8 picture_coding_type;
            sc_info *latest_sps;

            /* some mpeg4 streams have no paramaters above VOL,
             * or those parameters are only transmittered at very low frequency
             */
            /*! select VOL as the highest sequence parameter by default, if have higher, overload it */
            latest_sps = &sps_sc[0x3];
            SELECT_SPS(sps_sc[0x2], latest_sps);
            SELECT_SPS(sps_sc[1], latest_sps);
            SELECT_SPS(sps_sc[0], latest_sps);

            picture_coding_type = (ctx->this_scdata[1] >> 0x6) & 0x3;
            FIDX_ASSERT_RET(picture_coding_type <= 0x2,
                            "MPEG4 picture_coding_type out of range");

            if (picture_coding_type == 0 && ctx->sps_fresh[0x3] == 1) { /*! I frame */
                frm_pos->frame_type        = FIDX_FRAME_TYPE_I;
                frm_pos->global_offset   = latest_sps->global_offset;
                frm_pos->offset_in_packet = latest_sps->offset_in_packet;
                frm_pos->packet_count    = latest_sps->packet_count;
                ctx->sps_fresh[0x3] = 0;

                frm_pos->pts            = ctx->pts;
                ctx->pts               = DMX_INVALID_PTS;
            } else { /*! P or B frame. the I frame without SPS before head also treated as P frame */
                frm_pos->frame_type = (picture_coding_type == 0x2) ? FIDX_FRAME_TYPE_B : FIDX_FRAME_TYPE_P;
                frm_pos->global_offset   = this_sc->global_offset;
                frm_pos->offset_in_packet = this_sc->offset_in_packet;
                frm_pos->packet_count    = this_sc->packet_count;

#ifndef PUT_PTS_ON_I_FRAME_ONLY
                frm_pos->pts            = ctx->pts;
                ctx->pts               = DMX_INVALID_PTS;
#endif
            }
            frm_pos->frame_size = 0; /*! frame size need the position of the next start code */
            ret = FIDX_OK;
        }
    }

    return ret;
}

static hi_u32 simple_ue_v(hi_u32 code, hi_u32 *bit_num)
{
    hi_u32 val;
    hi_u32 zeros;
    zeros = (hi_s32)zeros_ms_32(code);
    if (zeros < 16) {
        *bit_num = (zeros << 1) + 1;
        val = (code >> (32 - (*bit_num))) - 1;
    } else {
        *bit_num = 0;
        val = 0xffffffff;
    }

    return val;
}

static hi_s32  parse_sps(hi_u8 *data, hi_s32 data_len, hi_s32 *sps_id)
{
    hi_u32  code, val, bit_num;

    code = ((hi_u32)data[0x4] << 24) | ((hi_u32)data[0x5] << 16) | /* data[4] shifts 24 bits, data[5] shifts 16 bits */
              ((hi_u32)data[0x6] << 0x8) | ((hi_u32)data[0x7]);

    val = simple_ue_v(code, &bit_num);
    FIDX_ASSERT_RET(val <= 31, "SPS ID out of range!");
    FIDX_ASSERT_RET(((bit_num + 0x7) >> 0x3) <= (hi_u32)(data_len - 0x4), "data NOT enough for SPS");

    *sps_id = (hi_s32)val;
    return FIDX_OK;
}

static hi_s32  parse_pps(hi_u8 *data, hi_s32 data_len, hi_s32 *spsid, hi_s32 *ppsid)
{
    hi_s32 code;
    hi_u32 val, bit_num;
    BS bs;

    bs_init(&bs, data, data_len);
    bs_skip(&bs, 0x8);

    /* pic_parameter_set_id */
    code = bs_show(&bs, 32);
    val = simple_ue_v((hi_u32)code, &bit_num);
    FIDX_ASSERT_RET(val <= 255, "PPS ID out of range!"); /* val range less than 255 */
    FIDX_ASSERT_RET((hi_s32)BITS_2_BYTES((hi_u32)bs_pos(&bs)) <= data_len, "data NOT enough for PPS");
    *ppsid = (hi_s32)val;
    bs_skip(&bs, (hi_s32)bit_num);

    /* seq_parameter_set_id */
    code = bs_show(&bs, 32); /* 32 bits */
    val = simple_ue_v((hi_u32)code, &bit_num);
    FIDX_ASSERT_RET(val <= 31, "SPS ID out of range!");
    FIDX_ASSERT_RET((hi_s32)BITS_2_BYTES((hi_u32)bs_pos(&bs)) <= data_len, "data NOT enough for SPS");
    *spsid = (hi_s32)val;

    return FIDX_OK;
}

static hi_s32  parse_slice_header(hi_u8 *data, hi_s32 data_len, hi_s32 *first_mbin_slice,
                                  hi_s32 *slice_type, hi_s32 *ppsid)
{
    hi_s32  code;
    hi_u32  val, bit_num;
    BS bs;

    bs_init(&bs, data, data_len);
    bs_skip(&bs, 0x8);

    /* first_mb_in_slice */
    code = bs_show(&bs, 32); /* 32 bits */
    val = simple_ue_v((hi_u32)code, &bit_num);
    FIDX_ASSERT_RET(val <= 8192, "'first_mb_in_slice' out of range!"); /* val range less than 8192 */
    FIDX_ASSERT_RET((hi_s32)BITS_2_BYTES((hi_u32)bs_pos(&bs)) <= data_len, "data NOT enough for 'first_mb_in_slice'");
    *first_mbin_slice = (hi_s32)val;
    bs_skip(&bs, (hi_s32)bit_num);

    /* slice_type */
    code = bs_show(&bs, 32); /* 32 bits */
    val = simple_ue_v((hi_u32)code, &bit_num);
    FIDX_ASSERT_RET((val <= 0x2 || (val >= 0x5 && val <= 0x7)), "'slice_type' out of range!");
    FIDX_ASSERT_RET((hi_s32)BITS_2_BYTES((hi_u32)bs_pos(&bs)) <= data_len, "data NOT enough for 'slice_type'");
    *slice_type = (hi_s32)val;
    bs_skip(&bs, (hi_s32)bit_num);

    /* pic_parameter_set_id */
    code = bs_show(&bs, 32); /* 32 bits */
    val = simple_ue_v((hi_u32)code, &bit_num);
    FIDX_ASSERT_RET((val <= 255), "'pic_parameter_set_id' out of range!"); /* val range less than 255 */
    FIDX_ASSERT_RET((hi_s32)BITS_2_BYTES((hi_u32)bs_pos(&bs)) <= data_len, "data NOT enough for pic_parameter_set_id");
    *ppsid = (hi_s32)val;

    return FIDX_OK;
}

/* For H264, produce frame information.
 * This function is to be called when frame border is found.
 * The frame information produced will be stored into 'stNewFramePos'
 */
static hi_void h264make_frame(fidx_ctx *ctx)
{
    sc_info   *sps_sc = &ctx->sps[0];
    sc_info   *end_sc = &ctx->seifollow_slice;
    sc_info   *this_sc = &ctx->this_sc;
    frame_pos *frm_pos = &ctx->new_frame_pos;

    if (frm_pos->frame_type == FIDX_FRAME_TYPE_I && ctx->sps_fresh[ctx->sps_id] == 1) {
        /* ! SPS should be treaded as the start of the I frame following the SPS. */
        /* pst_frm_pos->global_offset = sps_sc[ctx->sps_id].global_offset; */
        frm_pos->offset_in_packet = sps_sc[ctx->sps_id].offset_in_packet;
        frm_pos->packet_count = sps_sc[ctx->sps_id].packet_count;
        ctx->sps_fresh[ctx->sps_id] = 0;
        if (end_sc->sc_type != SC_TYPE_NONSLICE) {
            frm_pos->frame_size = (hi_s32)(this_sc->global_offset -
                                               frm_pos->global_offset);
        } else {
            frm_pos->frame_size = (hi_s32)(end_sc->global_offset -
                                               frm_pos->global_offset);
        }
    } else {
        if (frm_pos->frame_type == FIDX_FRAME_TYPE_I && ctx->sps_fresh[ctx->sps_id] != 1) {
            /* the I frame have no fresh SPS before head should be treaded as P frame */
            frm_pos->frame_type = FIDX_FRAME_TYPE_P;
        }
        if (end_sc->sc_type != SC_TYPE_NONSLICE) {
            frm_pos->frame_size = (hi_s32)(this_sc->global_offset - frm_pos->global_offset);
        } else {
            frm_pos->frame_size = (hi_s32)(end_sc->global_offset - frm_pos->global_offset);
        }
    }

#ifdef PUT_PTS_ON_I_FRAME_ONLY
    /*! if this frame is P or B frame, do NOT consume PTS to prevent the following I frame have a PTS equals to -1 */
    if ((frm_pos->frame_type != FIDX_FRAME_TYPE_I)) {
        /* if the PTS has been used by current frame(and no new PTS was got later), restore the used PTS */
        if (DMX_INVALID_PTS == ctx->pts) {
            ctx->pts = frm_pos->pts;
        }

        /* P or B frame have PTS equals to -1 */
        frm_pos->pts = DMX_INVALID_PTS;
    }
#endif
    return;
}

/* process H.264 start code
 * if success(find the start of a frame, and can output one index record)
 * return FIDX_OK, otherwise return FIDX_ERR
 */
hi_s32 process_sc_h264(hi_s32 inst_idx)
{
    hi_s32 ret = FIDX_ERR;
    hi_s32 nal_unit_type;
    hi_s32 sps_id, pps_id, first_mb, slice_type;
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    sc_info   *sps_sc = &ctx->sps[0];
    sc_info   *pps_sc = &ctx->pps[0];
    sc_info   *end_sc = &ctx->seifollow_slice;
    sc_info   *this_sc = &ctx->this_sc;
    frame_pos *frm_pos = &ctx->new_frame_pos;
    hi_u8       *data = &ctx->this_scdata[0];
    static  hi_s32  slc_type2frm_type[10] = {2, 3, 1, 0, 0, 2, 3, 1, 0, 0}; /* frm type num is 10 */

    FIDX_ASSERT_RET(0 != ctx->this_scvalid, "this_sc is not valid\n");
    FIDX_ASSERT_RET(!IS_SC_WRONG(), "not enough data for this_sc\n");

    if (0 != (data[0] & 0x80)) {
        return FIDX_ERR;
    }
    nal_unit_type = (hi_s32)(data[0] & 31);
    /*! to find the frame border, the position of the start code after the last slice of this frame is needed */
    if (nal_unit_type != FIDX_NALU_TYPE_IDR && nal_unit_type != FIDX_NALU_TYPE_SLICE &&
        end_sc->sc_type == SC_TYPE_UNKNOWN) {
        memcpy(end_sc, this_sc, sizeof(sc_info));
        end_sc->sc_type = SC_TYPE_NONSLICE;
    }

    if (nal_unit_type == FIDX_NALU_TYPE_SPS) {
        if ((parse_sps(data, ctx->this_scdata_len, &sps_id)) == FIDX_OK) {
            /*! if the SPS with the same ID as the former is found, the end of the last frame is also found */
            if (frm_pos->frame_type != FIDX_FRAME_TYPE_UNKNOWN && ctx->sps_id == sps_id) {
                h264make_frame(ctx);
                dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
            }
            this_sc->sc_type = SC_TYPE_SPS;
            this_sc->sc_id = sps_id;
            memcpy(&sps_sc[sps_id], this_sc, sizeof(sc_info));
            ctx->sps_fresh[sps_id] = 1;
        }
    } else if (nal_unit_type == FIDX_NALU_TYPE_PPS) {
        if ((parse_pps(data, ctx->this_scdata_len, &sps_id, &pps_id)) == FIDX_OK) {
            /*! check the if the SPS for this PPS have been parsed OK */
            if (ctx->sps[sps_id].sc_type == SC_TYPE_SPS) {
                /*! if the PPS with the same ID as the former is found, the end of the last frame is also found */
                if (frm_pos->frame_type != FIDX_FRAME_TYPE_UNKNOWN && ctx->pps_id == pps_id) {
                    h264make_frame(ctx);
                    dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                    memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                    ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
                }
                this_sc->sc_type = SC_TYPE_PPS;
                this_sc->sc_id = pps_id;
                this_sc->sup_sc_id = sps_id;
                memcpy(&pps_sc[pps_id], this_sc, sizeof(sc_info));
            } else {
                ret = FIDX_ERR;
            }
        }
    } else if (nal_unit_type == FIDX_NALU_TYPE_IDR ||
        nal_unit_type == FIDX_NALU_TYPE_SLICE) {
        if ((parse_slice_header(data, ctx->this_scdata_len, &first_mb, &slice_type, &pps_id)) == FIDX_OK) {
            /*! check if the SPS and PPS for this slice are parsed OK */
            if (pps_sc[pps_id].sc_type == SC_TYPE_PPS &&
                    sps_sc[pps_sc[pps_id].sup_sc_id].sc_type == SC_TYPE_SPS) {
                this_sc->sc_type = SC_TYPE_SLICE;  /* ! mark this SC is a slice SC */
            }

            /*! find frame border, output one frame information if necessary */
            if (frm_pos->frame_type != FIDX_FRAME_TYPE_UNKNOWN) {
                if ((first_mb == 0 || pps_id != ctx->pps_id)) {
                    /*! find frame border, the end of the last frame found. */
                    h264make_frame(ctx);
                    dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                    memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                    ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
                } else {
                    /*! this SC is still belongs to the last frame. it's slice type is ok to fresh the frame type */
                    hi_s32 frm_type = slc_type2frm_type[slice_type];
                    frm_pos->frame_type = (frm_type > frm_pos->frame_type) ? frm_type : frm_pos->frame_type;
                    this_sc->sc_type = SC_TYPE_UNKNOWN;  /* mark this_sc is not valid */
                }
            }

            /*! if this_sc belongs to the next new frame, create new frame information here */
            if (frm_pos->frame_type == FIDX_FRAME_TYPE_UNKNOWN && this_sc->sc_type == SC_TYPE_SLICE) {
                if (end_sc->sc_type != SC_TYPE_NONSLICE) {
                    frm_pos->global_offset = this_sc->global_offset;
                    frm_pos->offset_in_packet = this_sc->offset_in_packet;
                    frm_pos->packet_count = this_sc->packet_count;
                } else {
                    frm_pos->global_offset = end_sc->global_offset;
                    frm_pos->offset_in_packet = end_sc->offset_in_packet;
                    frm_pos->packet_count = end_sc->packet_count;
                }
                frm_pos->frame_type = (fidx_frame_type)slc_type2frm_type[slice_type];
                frm_pos->frame_size = 0;

                frm_pos->pts = ctx->pts;
                ctx->pts = DMX_INVALID_PTS;

                ctx->pps_id = pps_id;
                ctx->sps_id = pps_sc[pps_id].sup_sc_id;
                end_sc->sc_type = SC_TYPE_UNKNOWN;
            }
        }
        /* clear SEI when slice found to ensure the SEI is the one following the slice */
        end_sc->sc_type = SC_TYPE_UNKNOWN;
    }

    return ret;
}

#define dprint  HI_DBG_DEMUX
#define pos()   dprint("%s %d\n", __func__, __LINE__);

#define MAX(a, b)         (((a) < (b)) ?  (b) : (a))
#define MIN(a, b)         (((a) > (b)) ?  (b) : (a))
#define ABS(x)            (((x) < 0) ? -(x) : (x))
#define SIGN(a)           (((a) < 0) ? (-1) : (1))
#define MEDIAN(a, b, c)   ((a) + (b) + (c) - MIN((a), MIN((b), (c))) - MAX((a), MAX((b), (c))))

#define CLIP1(high, x)             (MAX(MIN((x), high), 0))
#define CLIP3(low, high, x)        (MAX(MIN((x), high), low))
#define CLIP255(x)                 (MAX(MIN((x), 255), 0))

static hi_s32 g_quant_tsdefault4x4[16] = { /* array size is 16 */
    16, 16, 16, 16,
    16, 16, 16, 16,
    16, 16, 16, 16,
    16, 16, 16, 16
};

static hi_s32 g_quant_intra_default8x8[64] = { /* array size is 64 */
    16, 16, 16, 16, 17, 18, 21, 24,
    16, 16, 16, 16, 17, 19, 22, 25,
    16, 16, 17, 18, 20, 22, 25, 29,
    16, 16, 18, 21, 24, 27, 31, 36,
    17, 17, 20, 24, 30, 35, 41, 47,
    18, 19, 22, 27, 35, 44, 54, 65,
    21, 22, 25, 31, 41, 54, 70, 88,
    24, 25, 29, 36, 47, 65, 88, 115
};

static hi_s32 g_quant_inter_default8x8[64] = { /* array size is 64 */
    16, 16, 16, 16, 17, 18, 20, 24,
    16, 16, 16, 17, 18, 20, 24, 25,
    16, 16, 17, 18, 20, 24, 25, 28,
    16, 17, 18, 20, 24, 25, 28, 33,
    17, 18, 20, 24, 25, 28, 33, 41,
    18, 20, 24, 25, 28, 33, 41, 54,
    20, 24, 25, 28, 33, 41, 54, 71,
    24, 25, 28, 33, 41, 54, 71, 91
};

static hi_void hevc_init_sig_last_scan(hevc_ctx_info *hevc_ctx, hi_u32 *buff_z, hi_u32 *buff_h, hi_u32 *buff_v,
    hi_u32 *buff_d, hi_s32 buffer_size, hi_s32 i_width, hi_s32 i_height, hi_s32 i_depth)
{
    hi_u32 ui_num_scan_pos;
    hi_u32 ui_next_scan_pos = 0;
    hi_s32 ui_scan_line, i_prim_dim, i_scnd_dim;
    hi_u32 *buff_temp;
    hi_u32 ui_num_blk_side, ui_num_blks, ui_blk, init_blk_pos;
    hi_s32 log2blk;
    hi_u32 offset_y, offset_x, offset_d, offset_scan;
    hi_s32 ui_cnt, num_blk_side, offset;
    hi_s32 x, y, blk_y, blk_x, i_y, i_x;

    ui_num_scan_pos  = i_width * i_width;

    if (i_width < 16) { /* width less than 16 */
        buff_temp = buff_d;
        if (i_width == 0x8) {
            buff_temp = hevc_ctx->sig_last_scan_cg32x32;
        }
        for (ui_scan_line = 0; ui_next_scan_pos < ui_num_scan_pos; ui_scan_line++) {
            i_prim_dim = ui_scan_line;
            i_scnd_dim = 0;
            while (i_prim_dim >= i_width) {
                i_scnd_dim++;
                i_prim_dim--;
            }
            while (i_prim_dim >= 0 && i_scnd_dim < i_width) {
                buff_temp[ui_next_scan_pos] = i_prim_dim * i_width + i_scnd_dim ;
                ui_next_scan_pos++;
                i_scnd_dim++;
                i_prim_dim--;
            }
        }
    }

    if (i_width > 0x4) {
        ui_num_blk_side = i_width >> 0x2;
        ui_num_blks    = ui_num_blk_side * ui_num_blk_side;
        log2blk      = hevc_ctx->auc_convert_to_bit[ui_num_blk_side] + 1;

        for (ui_blk = 0; ui_blk < ui_num_blks; ui_blk++) {
            ui_next_scan_pos = 0;
            init_blk_pos    = hevc_ctx->aui_sig_last_scan[ SCAN_DIAG ][ log2blk ][ ui_blk ];
            if (i_width == 32) { /* width equal 32 */
                init_blk_pos = hevc_ctx->sig_last_scan_cg32x32[ui_blk];
            }
            offset_y = init_blk_pos / ui_num_blk_side;
            offset_x = init_blk_pos - offset_y * ui_num_blk_side;
            offset_d = 0x4 * (offset_x + offset_y * i_width);
            offset_scan = 16 * ui_blk; /* ui_blk multiply by 16 */
            for (ui_scan_line = 0; ui_next_scan_pos < 16; ui_scan_line++) { /* 16 loops */
                i_prim_dim = ui_scan_line ;
                i_scnd_dim = 0;
                while (i_prim_dim >= 0x4) {
                    i_scnd_dim++;
                    i_prim_dim--;
                }
                while (i_prim_dim >= 0 && i_scnd_dim < 0x4 && ((ui_next_scan_pos + offset_scan) < buffer_size)) {
                    buff_d[ui_next_scan_pos + offset_scan] = i_prim_dim * i_width + i_scnd_dim + offset_d;
                    ui_next_scan_pos++;
                    i_scnd_dim++;
                    i_prim_dim--;
                }
            }
        }
    }

    ui_cnt = 0;
    if (i_width > 0x2) {
        num_blk_side = i_width >> 0x2;
        for (blk_y = 0; blk_y < num_blk_side; blk_y++) {
            for (blk_x = 0; blk_x < num_blk_side; blk_x++) {
                offset = blk_y * 0x4 * i_width + blk_x * 0x4;
                for (y = 0; y < 4; y++) {
                    for (x = 0; x < 0x4; x++) {
                        buff_h[ui_cnt] = y * i_width + x + offset;
                        ui_cnt++;
                    }
                }
            }
        }

        ui_cnt = 0;
        for (blk_x = 0; blk_x < num_blk_side; blk_x++) {
            for (blk_y = 0; blk_y < num_blk_side; blk_y++) {
                offset = blk_y * 0x4 * i_width + blk_x * 0x4;
                for (x = 0; x < 0x4; x++) {
                    for (y = 0; y < 0x4; y++) {
                        buff_v[ui_cnt] = y * i_width + x + offset;
                        ui_cnt++;
                    }
                }
            }
        }
    } else {
        for (i_y = 0; i_y < i_height; i_y++) {
            for (i_x = 0; i_x < i_width; i_x++) {
                buff_h[ui_cnt] = i_y * i_width + i_x;
                ui_cnt++;
            }
        }

        ui_cnt = 0;
        for (i_x = 0; i_x < i_width; i_x++) {
            for (i_y = 0; i_y < i_height; i_y++) {
                buff_v[ui_cnt] = i_y * i_width + i_x;
                ui_cnt++;
            }
        }
    }

    return;
}

static hi_void hevc_init_scaling_order_table(hevc_ctx_info *hevc_ctx)
{
    hi_s32 i, c;

    for (i = 0; i < sizeof(hevc_ctx->auc_convert_to_bit); i++) {
        hevc_ctx->auc_convert_to_bit[i] = -1;
    }

#if 0 // 这个会越界
    c = 0;

    for (i = 4; i < HEVC_MAX_CU_SIZE; i *= 2) {
        hevc_ctx->auc_convert_to_bit[i] = c;
        c++;
    }

    hevc_ctx->auc_convert_to_bit[i] = c;
#else
    c = 0;

    for (i = 0x2; i < HEVC_MAX_CU_DEPTH; i++) {
        hevc_ctx->auc_convert_to_bit[(1 << i)] = c;
        c++;
    }

#endif

    c = 0x2;
    for (i = 0; i < HEVC_MAX_CU_DEPTH; i++) {
        hevc_init_sig_last_scan(hevc_ctx, hevc_ctx->aui_sig_last_scan[0][i], hevc_ctx->aui_sig_last_scan[1][i],
                                hevc_ctx->aui_sig_last_scan[0x2][i],
                                hevc_ctx->aui_sig_last_scan[0x3][i], (HEVC_MAX_CU_SIZE * HEVC_MAX_CU_SIZE), c, c, i);
        c <<= 1;
    }

    return;
}

static hi_s8 hevc_is_idrunit(hi_u32 nal_unit_type)
{
    return (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)
            ? 1 : 0;
}

static hi_s8 hevc_is_blaunit(hi_u32 nal_unit_type)
{
    return (nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP)
            ? 1 : 0;
}

static hi_s8 hevc_is_craunit(hi_u32 nal_unit_type)
{
    return (nal_unit_type == NAL_UNIT_CODED_SLICE_CRA)
            ? 1 : 0;
}

static hi_s32 hevc_is_flush_unit(hi_u32 nal_unit_type)
{
    return (nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP   \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL \
            || nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP)
            ? 1 : 0;
}

static hi_void hevc_init_dec_para(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i;

    pos();

    hevc_ctx->last_display_poc = -HEVC_MAX_INT;

    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        hevc_ctx->frame_store[i].non_existing = 0;      /* a real pic; */
        hevc_ctx->frame_store[i].frame_store_state = FS_NOT_USED;  /* empty; */
        hevc_ctx->frame_store[i].is_reference = 0;                /* not pic used for ref; */
        hevc_ctx->frame_store[i].poc = 0;
        hevc_ctx->frame_store[i].frame.frame_store = &hevc_ctx->frame_store[i];
    }

    /* hevc_ctx->dpb refresh every stream */
    for (i = 0; i < HEVC_MAX_DPB_NUM; i++) {
        hevc_ctx->dpb.fs[i] = hevc_ctx->dpb.fs_negative_ref[i] = hevc_ctx->dpb.fs_positive_ref[i] =
            hevc_ctx->dpb.fs_ltref[i] = NULL;
    }
    hevc_ctx->dpb.used_size = 0;
    hevc_ctx->dpb.max_long_term_pic_idx = 0;
    hevc_ctx->dpb.ltref_frames_in_buffer = 0;
    hevc_ctx->dpb.negative_ref_frames_in_buffer = 0;
    hevc_ctx->dpb.positive_ref_frames_in_buffer = 0;
    hevc_ctx->dpb.size = HEVC_MAX_DPB_NUM;

    for (i = 0; i < HEVC_MAX_LIST_SIZE; i++) {
        hevc_ctx->p_list_x[0][i] = hevc_ctx->p_list_x[1][i] = NULL;
    }

    memset(&hevc_ctx->curr_slice, 0, sizeof(hevc_slice_segment_header));
    hevc_ctx->curr_slice.slice_type = HEVC_I_SLICE;
    hevc_ctx->curr_slice.new_pic_type = IS_NEW_PIC;
    hevc_ctx->curr_pic.pic_type = HEVC_ERR_FRAME;
    hevc_ctx->total_slice_num = 0;

    hevc_ctx->b_new_sequence = HEVC_TRUE;
    hevc_ctx->b_no_output_of_prior_pics_flag = HEVC_FALSE;
    hevc_ctx->b_no_rasl_output_flag = HEVC_TRUE;

    hevc_ctx->allow_start_dec = 0;
    hevc_ctx->poc_random_access = HEVC_MAX_INT;
    hevc_ctx->prev_rapis_bla = HEVC_FALSE;

    hevc_ctx->scaling_list_size[0] = 16; /* scal list size is 16 */
    hevc_ctx->scaling_list_size[1] = 64; /* scal list size is 64 */
    hevc_ctx->scaling_list_size[0x2] = 256; /* scal list size is 256 */
    hevc_ctx->scaling_list_size[0x3] = 1025; /* scal list size is 1025 */

    hevc_ctx->scaling_list_size_x[0] = 4; /* scal list size_x is 4 */
    hevc_ctx->scaling_list_size_x[1] = 8; /* scal list size_x is 8 */
    hevc_ctx->scaling_list_size_x[0x2] = 16; /* scal list size_x is 16 */
    hevc_ctx->scaling_list_size_x[0x3] = 32; /* scal list size_x is 32 */

    hevc_ctx->scaling_list_num[0] = 6; /* scal list num is 6 */
    hevc_ctx->scaling_list_num[1] = 6; /* scal list num is 6 */
    hevc_ctx->scaling_list_num[0x2] = 6; /* scal list num is 6 */
    hevc_ctx->scaling_list_num[0x3] = 2; /* scal list num is 2 */

    memset(&hevc_ctx->bs, 0, sizeof(BS));
    hevc_ctx->p_bs = &hevc_ctx->bs;
    hevc_ctx->p_curr_nal = &hevc_ctx->nal_array;

    return;
}

static hi_u32 hevc_ue_v(BS *bs, hi_s8 *name)
{
    hi_u32 num_bits, tmpbits;
    hi_u32 info;
    hi_u32 leading_zeros;

    tmpbits = bs_show(bs, 32); /* 32 bits */
    leading_zeros = zeros_ms_32(tmpbits);
    if (leading_zeros < 32) { /* less than 32 */
        bs_skip(bs, leading_zeros);
        info = bs_show(bs, (leading_zeros + 1)) - 1;
        bs_skip(bs, (leading_zeros + 1));
        num_bits = (leading_zeros << 1) + 1;
    } else {
        info = 0xffffeeee;
        num_bits = 32; /* 32 bits */
        return info;
    }

    return info;
}

static hi_u32 hevc_u_v(BS *bs, hi_s32 v, hi_s8 *name)
{
    hi_u32 code;

    code = bs_get(bs, v);

    return code;
}

static hi_s32 hevc_dec_ptl(hevc_ctx_info *hevc_ctx, hevc_profile_tier_level *ptl, hi_s32 profile_present_flag,
                           hi_s32 max_num_sub_layers_minus1)
{
    hi_s32 i;

    if ((hevc_ctx == NULL) || (ptl == NULL)) {
        dprint("invalid parameter!\n");
        return HEVC_DEC_ERR;
    }

    if (profile_present_flag) {
        hevc_u_v(hevc_ctx->p_bs, 8, "general_profile_space[]");
        hevc_u_v(hevc_ctx->p_bs, 32, "xxx");
        hevc_u_v(hevc_ctx->p_bs, 20, "general_reserved_zero_44bits[0..15]");
        hevc_u_v(hevc_ctx->p_bs, 28, "general_reserved_zero_44bits[16..31]");
    }

    ptl->general_level_idc = hevc_u_v(hevc_ctx->p_bs, 8, "general_level_idc");

    for (i = 0; i < max_num_sub_layers_minus1 && i < 0x6; i++) {
        if (profile_present_flag) {
            ptl->sub_layer_profile_present_flag[i] = hevc_u_v(hevc_ctx->p_bs, 1, "sub_layer_profile_present_flag");
        }
        ptl->sub_layer_level_present_flag[i]   = hevc_u_v(hevc_ctx->p_bs, 1, "sub_layer_level_present_flag");
    }

    if (max_num_sub_layers_minus1 > 0) {
        for (i = max_num_sub_layers_minus1; i < 0x8; i++) {
            hevc_u_v(hevc_ctx->p_bs, 0x2, "reserved_zero_2bits");
        }
    }

    for (i = 0; i < max_num_sub_layers_minus1 && i < 0x6; i++) {
        if (profile_present_flag && ptl->sub_layer_profile_present_flag[i]) {
            hevc_u_v(hevc_ctx->p_bs, 8, "sub_layer_profile_space");
            hevc_u_v(hevc_ctx->p_bs, 32, "sub_layer_profile_compatibility_flag");
            hevc_u_v(hevc_ctx->p_bs, 20, "general_progressive_source_flag");
            hevc_u_v(hevc_ctx->p_bs, 28, "general_reserved_zero_44bits[16..31]");
        }

        if (ptl->sub_layer_level_present_flag[i]) {
            hevc_u_v(hevc_ctx->p_bs, 0x8, "sub_layer_level_idc");
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_process_vps(hevc_ctx_info *hevc_ctx, hevc_video_param_set *vps)
{
    char buf[100];
    hi_s32 ret;
    hi_s32 i, j;
    hi_s32 vps_max_sub_layers_minus1;

    vps->vps_reserved_three_2bits = hevc_u_v(hevc_ctx->p_bs, 2, "vps_reserved_three_2bits");
    if (vps->vps_reserved_three_2bits != 0x3) {
        dprint("vps_reserved_three_2bits(%d) not equal 0x3.\n", vps->vps_reserved_three_2bits);
    }

    vps->vps_max_layers_minus1 = hevc_u_v(hevc_ctx->p_bs, 0x6, "vps_max_layers_minus1");
    if (vps->vps_max_layers_minus1 < 0 || vps->vps_max_layers_minus1 > 63) {
        dprint("vps_max_layers_minus1 out of range(0,63).\n");
        return HEVC_DEC_ERR;
    }

    vps_max_sub_layers_minus1 = hevc_u_v(hevc_ctx->p_bs, 0x3, "vps_max_sub_layers_minus1");
    if (vps_max_sub_layers_minus1 < 0 || vps_max_sub_layers_minus1 > 0x6) {
        dprint("vps_max_sub_layers_minus1 out of range(0,6).\n");
        return HEVC_DEC_ERR;
    }

    vps->vps_max_sub_layers_minus1 = vps_max_sub_layers_minus1 + 1;
    vps->vps_temporal_id_nesting_flag = hevc_u_v(hevc_ctx->p_bs, 1, "vps_temporal_id_nesting_flag");

    vps->vps_reserved_0xffff_16bits = hevc_u_v(hevc_ctx->p_bs, 16, "vps_reserved_ffff_16bits");

    if (vps->vps_reserved_0xffff_16bits != 0xffff) {
        dprint("vps_reserved_0xffff_16bits not equal 0xffff.\n");
        return HEVC_DEC_ERR;
    }

    ret = hevc_dec_ptl(hevc_ctx, &(vps->profile_tier_level), 1, vps_max_sub_layers_minus1);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("VPS hevc_dec_ptl error.\n");
        return HEVC_DEC_ERR;
    }

    vps->vps_sub_layer_ordering_info_present_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                             "vps_sub_layer_ordering_info_present_flag");

    for (i = 0; i <= vps_max_sub_layers_minus1; i++) {
        vps->vps_max_dec_pic_buffering[i] = hevc_ue_v(hevc_ctx->p_bs, "vps_max_dec_pic_buffering_minus1[i]") + 1;
        vps->vps_num_reorder_pics[i] = hevc_ue_v(hevc_ctx->p_bs, "vps_num_reorder_pics[i]");
        vps->vps_max_latency_increase[i] = hevc_ue_v(hevc_ctx->p_bs, "vps_max_latency_increase_plus1[i]");

        if (!vps->vps_sub_layer_ordering_info_present_flag) {
            for (i++; i <= vps_max_sub_layers_minus1; i++) {
                vps->vps_max_dec_pic_buffering[i] =  vps->vps_max_dec_pic_buffering[0] ;
                vps->vps_num_reorder_pics[i] = vps->vps_num_reorder_pics[0];
                vps->vps_max_latency_increase[i] =  vps->vps_max_latency_increase[0];
            }
            break;
        }
    }

    vps->vps_max_layer_id = hevc_u_v(hevc_ctx->p_bs, 0x6, "vps_max_layer_id");
    if (vps->vps_max_layer_id > HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1) {
        dprint("vps_max_layer_id = %d, out of range\n", vps->vps_max_layer_id);
        return HEVC_DEC_ERR;
    }

    vps->vps_num_layer_sets_minus1 = hevc_ue_v(hevc_ctx->p_bs,  "vps_num_layer_sets_minus1");
    if (vps->vps_num_layer_sets_minus1 < 0 || vps->vps_num_layer_sets_minus1 > HEVC_MAX_VPS_OP_SETS_PLUS1 - 1) {
        dprint("vps_num_layer_sets_minus1(%d) out of range(0,1023).\n", vps->vps_num_layer_sets_minus1);
        return HEVC_DEC_ERR;
    }

    for (i = 1; i <= vps->vps_num_layer_sets_minus1; i++) {
        /* operation point set */
        for (j = 0; j <= vps->vps_max_layer_id; j++) {
            snprintf(buf, sizeof(buf), "layer_id_included_flag[%d][%d]", i, j);
            vps->layer_id_included_flag[i][j] = hevc_u_v(hevc_ctx->p_bs, 1, buf);
        }
    }

    vps->vps_timing_info_present_flag = hevc_u_v(hevc_ctx->p_bs, 1, "vps_timing_info_present_flag");
    if (vps->vps_timing_info_present_flag) {
        vps->vps_num_units_in_tick = hevc_u_v(hevc_ctx->p_bs, 32, "vps_num_units_in_tick");
        vps->vps_time_scale = hevc_u_v(hevc_ctx->p_bs, 32, "vps_time_scale");
        vps->vps_poc_proportional_to_timing_flag = hevc_u_v(hevc_ctx->p_bs, 1, "vps_poc_proportional_to_timing_flag");
        if (vps->vps_poc_proportional_to_timing_flag) {
            vps->vps_num_ticks_poc_diff_one_minus1 = hevc_ue_v(hevc_ctx->p_bs, "vps_num_ticks_poc_diff_one_minus1");
        }
        vps->vps_num_hrd_parameters = hevc_ue_v(hevc_ctx->p_bs, "vps_num_hrd_parameters");

        if (vps->vps_num_hrd_parameters > 0) {
            vps->cprms_present_flag[0] = HEVC_TRUE;
        }
    }
    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_vps(hevc_ctx_info *hevc_ctx)
{
    hi_u32  vps_video_parameter_set_id;
    hevc_video_param_set *vps_tmp = NULL;

    pos();

    vps_tmp = &hevc_ctx->tmp_param.vps_tmp;
    memset(vps_tmp, 0, sizeof(hevc_video_param_set));

    vps_video_parameter_set_id = hevc_u_v(hevc_ctx->p_bs, 0x4, "vps_video_parameter_set_id");
    if (vps_video_parameter_set_id > 15) { /* vps_video_parameter_set_id range is 0 to 15 */
        dprint("p_vps->vps_video_parameter_set_id out of range(0,15).\n");
        return HEVC_DEC_ERR;
    }

    if (hevc_ctx->p_vps[vps_video_parameter_set_id].valid) {
        vps_tmp->video_parameter_set_id  = vps_video_parameter_set_id;

        if (hevc_process_vps(hevc_ctx, vps_tmp) != HEVC_DEC_NORMAL) {
            dprint("VPS[%d] decode error0.\n", vps_video_parameter_set_id);
            return HEVC_DEC_ERR;
        }

        vps_tmp->is_refresh = 1;
        vps_tmp->valid = 1;
        memmove(&(hevc_ctx->p_vps[vps_video_parameter_set_id]), vps_tmp, sizeof(hevc_video_param_set));
    } else {
        hevc_ctx->p_vps[vps_video_parameter_set_id].video_parameter_set_id = vps_video_parameter_set_id;

        if (hevc_process_vps(hevc_ctx, &(hevc_ctx->p_vps[vps_video_parameter_set_id])) != HEVC_DEC_NORMAL) {
            dprint("VPS[%d] decode error1.\n", vps_video_parameter_set_id);
            hevc_ctx->p_vps[vps_video_parameter_set_id].is_refresh = 1;
            hevc_ctx->p_vps[vps_video_parameter_set_id].valid = 0;
            return HEVC_DEC_ERR;
        }
        hevc_ctx->p_vps[vps_video_parameter_set_id].is_refresh = 1;
        hevc_ctx->p_vps[vps_video_parameter_set_id].valid = 1;
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_se_v(BS *bs, hi_s8 *name)
{
    hi_u32 num_bits, tmpbits;
    hi_s32 info;
    hi_u32 leading_zeros;
    hi_u32 info_last_bit;

    tmpbits = bs_show(bs, 32); /* 32 bits */
    leading_zeros = zeros_ms_32(tmpbits);
    if (leading_zeros < 32) { /* less than 32 bits */
        bs_skip(bs, leading_zeros);
        info = bs_show(bs, (leading_zeros + 1)) - 1;
        info_last_bit = info & 1;
        info = info >> 1;
        info = ((info_last_bit & 1) ? (info + 1) : -1 * info);
        bs_skip(bs, (leading_zeros + 1));
        num_bits = (leading_zeros << 1) + 1;
    } else {
        info = 0x7fffffff;
        num_bits = 32; /* 32 bits */
        return info;
    }

    return info;
}

static hi_s32 *hevc_get_scaling_list_default_address(hi_u32 size_id, hi_u32 matrix_id)
{
    hi_s32 *src = NULL;

    switch (size_id) {
        case scaling_list_4x4:
            src = g_quant_tsdefault4x4;
            break;
        case scaling_list_8x8:
            src = (matrix_id < 3) ? g_quant_intra_default8x8 : g_quant_inter_default8x8;
            break;
        case scaling_list_16x16:
            src = (matrix_id < 3) ? g_quant_intra_default8x8 : g_quant_inter_default8x8;
            break;
        case scaling_list_32x32:
            src = (matrix_id < 1) ? g_quant_intra_default8x8 : g_quant_inter_default8x8;
            break;
        default:
            dprint("hevc_get_scaling_list_default_address null.\n");
            src = NULL;
            break;
    }
    return src;
}

static hi_s32 hevc_dec_scaling_list_data(hevc_ctx_info *hevc_ctx, hevc_scaling_list *scaling_list)
{
    hi_u8   scaling_list_pred_mode_flag;
    hi_u32  code, size_id, matrix_id, reflist_id;
    hi_s32  coef_num, i;
    hi_s32  next_coef;
    hi_u32 *scan = NULL;
    hi_s32 *dst_scaling_list = NULL;
    hi_s32 *matrix_address = NULL;

    /* init quantization matrix array */
    memcpy(scaling_list->scaling_list_coef[scaling_list_32x32][0x3],
           scaling_list->scaling_list_coef[scaling_list_32x32][1], HEVC_MAX_MATRIX_COEF_NUM * sizeof(hi_s32));

    /* for each size */
    for (size_id = 0; size_id < SCALING_LIST_SIZE_NUM; size_id++) {
        for (matrix_id = 0; matrix_id < hevc_ctx->scaling_list_num[size_id]; matrix_id++) {
            scaling_list_pred_mode_flag = hevc_u_v(hevc_ctx->p_bs, 1, "scaling_list_pred_mode_flag");
            scaling_list->scaling_list_pred_mode_flag[size_id][matrix_id] = scaling_list_pred_mode_flag;
            coef_num = MIN(HEVC_MAX_MATRIX_COEF_NUM, (hi_s32)hevc_ctx->scaling_list_size[size_id]);
            dst_scaling_list = scaling_list->scaling_list_coef[size_id][matrix_id];
            scan = (0 == size_id) ? hevc_ctx->aui_sig_last_scan[SCAN_DIAG][1] : hevc_ctx->sig_last_scan_cg32x32;

            if (!scaling_list_pred_mode_flag) { /* copy mode */
                code = hevc_ue_v(hevc_ctx->p_bs, "scaling_list_pred_matrix_id_delta");
                if (code > matrix_id || matrix_id >= HEVC_SCALING_LIST_NUM) {
                    dprint("scaling_list_pred_matrix_id_delta out of range(0,matrix_id).\n");
                    return HEVC_DEC_ERR;
                }

                scaling_list->scaling_list_pred_matrix_id_delta[size_id][matrix_id] = code;
                scaling_list->ref_matrix_id[size_id][matrix_id] = (hi_u32)((hi_s32)matrix_id - code);
                if (size_id > scaling_list_8x8) {
                    reflist_id = scaling_list->ref_matrix_id[size_id][matrix_id];
                    code = (matrix_id == reflist_id) ? 16 : scaling_list->scaling_list_dc[size_id][reflist_id];
                    scaling_list->scaling_list_dc[size_id][matrix_id] = code;
                }
                reflist_id = scaling_list->ref_matrix_id[size_id][matrix_id];
                matrix_address = (matrix_id == reflist_id) ?
                    hevc_get_scaling_list_default_address(size_id, reflist_id) :
                    scaling_list->scaling_list_coef[size_id][reflist_id];
                if (matrix_address == NULL) {
                    dprint("hevc_dec_scaling_list_data matrix_address null.\n");
                    return HEVC_DEC_ERR;
                }
                memcpy(dst_scaling_list, matrix_address, sizeof(hi_s32)*coef_num);
            } else { /* DPCM mode */
                next_coef = HEVC_SCALING_LIST_START_VALUE;
                if (size_id > scaling_list_8x8) {
                    scaling_list->scaling_list_dc_coef_minus8 = hevc_se_v(hevc_ctx->p_bs,
                                                                          "scaling_list_dc_coef_minus8");
                    if (scaling_list->scaling_list_dc_coef_minus8 < -7 || /* range is -7 to 247) */
                        scaling_list->scaling_list_dc_coef_minus8 > 247) { /* range is -7 to 247) */
                        dprint("scaling_list_dc_coef_minus8 out of range(-7,247).\n");
                        return HEVC_DEC_ERR;
                    }
                    scaling_list->scaling_list_dc[size_id][matrix_id] = scaling_list->scaling_list_dc_coef_minus8 + 0x8;
                    next_coef = scaling_list->scaling_list_dc[size_id][matrix_id];
                }
                for (i = 0; i < coef_num; i++) {
                    scaling_list->scaling_list_delta_coef = hevc_se_v(hevc_ctx->p_bs, "scaling_list_delta_coef");
                    if (scaling_list->scaling_list_delta_coef < -128 || /* range is -128 to 127) */
                        scaling_list->scaling_list_delta_coef > 127) { /* range is -128 to 127) */
                        dprint("scaling_list_dc_coef_minus8 out of range(-128,127).\n");
                        return HEVC_DEC_ERR;
                    }
                    next_coef = (next_coef + scaling_list->scaling_list_delta_coef + 256) % 256; /* 256 bytes aligned */
                    dst_scaling_list[scan[i]] = next_coef;
                }
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_short_term_ref_pic_set(hevc_ctx_info *hevc_ctx, hevc_seq_param_set *sps,
    hevc_short_term_rpset *short_term_rpset, hi_u32 idx)
{
    hi_u32 r_idx, code, ref_idc;
    hi_u32 k = 0;
    hi_u32 k1 = 0;
    hi_u32 k2 = 0;
    hi_u32 prev = 0;
    hi_s32 delta_rps, j;
    hi_s32 delta_poc, temp;
    hi_u8  used;
    hi_u32 num_neg_pics, i;
    hi_u32 used_by_curr_pic_flag, use_delta_flag;
    hi_u32 delta_poc_s0_minus1, used_by_curr_pic_s0_flag, delta_poc_s1_minus1, used_by_curr_pic_s1_flag;
    hevc_short_term_rpset *temp_rpset = NULL;

    if (idx > 0) {
        short_term_rpset->inter_ref_pic_set_prediction_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                       "inter_ref_pic_set_prediction_flag");
    } else {
        short_term_rpset->inter_ref_pic_set_prediction_flag = HEVC_FALSE;
    }
    if (short_term_rpset->inter_ref_pic_set_prediction_flag) {
        if (idx == sps->num_short_term_ref_pic_sets) {
            code = hevc_ue_v(hevc_ctx->p_bs, "delta_idx_minus1");
            short_term_rpset->delta_idx = code + 1;
        } else {
            code = 0;
            short_term_rpset->delta_idx = 0;
        }

        if (short_term_rpset->delta_idx > idx) {
            dprint("delta_idx(%d) > idx(%d).\n", short_term_rpset->delta_idx, idx);
            return HEVC_DEC_ERR;
        }

        r_idx = idx - 1 - code;
        if (r_idx > (idx - 1)) {
            dprint("r_idx(%d) > (idx-1)(%d) or < 0).\n", r_idx, (idx - 1));
            return HEVC_DEC_ERR;
        }

        temp_rpset = &(sps->short_term_ref_pic_set[r_idx]);

        short_term_rpset->delta_rps_sign = hevc_u_v(hevc_ctx->p_bs, 1, "delta_rps_sign");
        short_term_rpset->abs_delta_rps = hevc_ue_v(hevc_ctx->p_bs, "abs_delta_rps_minus1") + 1;
        delta_rps = (1 - (short_term_rpset->delta_rps_sign << 1)) * short_term_rpset->abs_delta_rps;

        if (temp_rpset->num_of_pics > HEVC_MAX_NUM_REF_PICS) {
            dprint("p_temp_rpset->num_of_pics(%d) out of range(0,15).\n", temp_rpset->num_of_pics);
            return HEVC_DEC_ERR;
        }

        for (i = 0; i <= temp_rpset->num_of_pics; i++) {
            /* first bit is "1" if idc is 1 */
            used_by_curr_pic_flag = hevc_u_v(hevc_ctx->p_bs, 1, "used_by_curr_pic_flag");
            ref_idc = used_by_curr_pic_flag;

            if (ref_idc == 0) {
                /* second bit is "1" if idc is 2, "0" otherwise. */
                use_delta_flag = hevc_u_v(hevc_ctx->p_bs, 1, "use_delta_flag");
                ref_idc = use_delta_flag << 1;
            }
            if (ref_idc == 1 || ref_idc == 0x2) {
                delta_poc = delta_rps + ((i < temp_rpset->num_of_pics) ? temp_rpset->delta_poc[i] : 0);
                short_term_rpset->delta_poc[k] = delta_poc;
                short_term_rpset->used_flag[k] = (1 == ref_idc);

                if (delta_poc < 0) {
                    k1++;
                } else {
                    k2++;
                }
                k++;
            }

            short_term_rpset->ref_idc[i] = ref_idc;
        }
        short_term_rpset->num_ref_idc = temp_rpset->num_of_pics + 1;
        short_term_rpset->num_of_pics = k;
        short_term_rpset->num_negative_pics = k1;
        short_term_rpset->num_positive_pics = k2;

        if (short_term_rpset->num_of_pics > HEVC_MAX_NUM_REF_PICS ||
                short_term_rpset->num_negative_pics > HEVC_MAX_NUM_REF_PICS ||
                short_term_rpset->num_positive_pics > HEVC_MAX_NUM_REF_PICS) {
            dprint("num_of_pics out of range(0,15).\n");
            return HEVC_DEC_ERR;
        }

        // sort_delta_poc: sort in increasing order (smallest first)
        for (i = 1; i < short_term_rpset->num_of_pics; i++) {
            delta_poc = short_term_rpset->delta_poc[i];
            used = short_term_rpset->used_flag[i];
            for (j = i - 1; j >= 0; j--) {
                temp = short_term_rpset->delta_poc[j];
                if (delta_poc < temp) {
                    short_term_rpset->delta_poc[j + 1] = temp;
                    short_term_rpset->used_flag[j + 1] = short_term_rpset->used_flag[j];
                    short_term_rpset->delta_poc[j] = delta_poc;
                    short_term_rpset->used_flag[j] = used;
                }
            }
        }

        // flip the negative values to largest first
        num_neg_pics = short_term_rpset->num_negative_pics;
        for (i = 0, j = (hi_s32)(num_neg_pics - 1); i < (num_neg_pics >> 1); i++, j--) {
            delta_poc = short_term_rpset->delta_poc[i];
            used = short_term_rpset->used_flag[i];
            short_term_rpset->delta_poc[i] = short_term_rpset->delta_poc[j];
            short_term_rpset->used_flag[i] = short_term_rpset->used_flag[j];
            short_term_rpset->delta_poc[j] = delta_poc;
            short_term_rpset->used_flag[j] = used;
        }
    } else {
        short_term_rpset->num_negative_pics = hevc_ue_v(hevc_ctx->p_bs, "num_negative_pics");
        if (short_term_rpset->num_negative_pics > HEVC_MAX_NUM_REF_PICS) {
            dprint("p_temp_rpset->num_negative_pics(%d) out of range(0,15).\n", short_term_rpset->num_negative_pics);
            return HEVC_DEC_ERR;
        }

        short_term_rpset->num_positive_pics = hevc_ue_v(hevc_ctx->p_bs, "num_positive_pics");
        if (short_term_rpset->num_positive_pics > HEVC_MAX_NUM_REF_PICS) {
            dprint("p_temp_rpset->num_positive_pics(%d) out of range(0,15).\n", short_term_rpset->num_negative_pics);
            return HEVC_DEC_ERR;
        }

        short_term_rpset->num_of_pics = short_term_rpset->num_negative_pics + short_term_rpset->num_positive_pics;
        if (short_term_rpset->num_of_pics > HEVC_MAX_NUM_REF_PICS) {
            dprint("p_short_term_rpset->num_of_pics(%d) out of range[0,%d].\n", short_term_rpset->num_of_pics,
                   HEVC_MAX_NUM_REF_PICS);
            return HEVC_DEC_ERR;
        }

        for (i = 0; i < short_term_rpset->num_negative_pics; i++) {
            delta_poc_s0_minus1           = hevc_ue_v(hevc_ctx->p_bs, "delta_poc_s0_minus1");
            if (delta_poc_s0_minus1 > 32767) { /* delta_poc_s0_minus1 less than 32767 */
                dprint("delta_poc_s0_minus1(%d) out of range.\n", delta_poc_s0_minus1);
                return HEVC_DEC_ERR;
            }
            delta_poc                      = prev - delta_poc_s0_minus1 - 1;
            prev                          = delta_poc;
            short_term_rpset->delta_poc[i] = delta_poc;
            used_by_curr_pic_s0_flag      = hevc_u_v(hevc_ctx->p_bs, 1, "used_by_curr_pic_s0_flag");
            short_term_rpset->used_flag[i] = used_by_curr_pic_s0_flag;
        }

        prev = 0;
        for (i = short_term_rpset->num_negative_pics; i < short_term_rpset->num_of_pics; i++) {
            delta_poc_s1_minus1           = hevc_ue_v(hevc_ctx->p_bs, "delta_poc_s1_minus1");
            delta_poc                      = prev + delta_poc_s1_minus1 + 1;
            prev                          = delta_poc;
            short_term_rpset->delta_poc[i] = delta_poc;
            used_by_curr_pic_s1_flag      = hevc_u_v(hevc_ctx->p_bs, 1, "used_by_curr_pic_s1_flag");
            short_term_rpset->used_flag[i] = used_by_curr_pic_s1_flag;
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_process_sps(hevc_ctx_info *hevc_ctx, hevc_seq_param_set *sps)
{
    hi_u32 i;
    hi_s32 ret;
    hi_u32 log2_max_pic_order_cnt_lsb_minus4;

    const hi_s32 g_crop_unit_x[0x4] = {1, 2, 2, 1};
    const hi_s32 g_crop_unit_y[0x4] = {1, 2, 1, 1};

    sps->chroma_format_idc = hevc_ue_v(hevc_ctx->p_bs, "chroma_format_idc");

    // chroma_format_idc equal to 1 (4:2:0)
    if (sps->chroma_format_idc != 1) {
        if (sps->chroma_format_idc > 0x3) {
            dprint("p_sps->chroma_format_idc out of range(0,3).\n");
            return HEVC_DEC_ERR;
        } else {
            if (sps->chroma_format_idc == 0x3) {
                sps->separate_colour_plane_flag = hevc_u_v(hevc_ctx->p_bs, 1, "separate_colour_plane_flag");
                if (sps->separate_colour_plane_flag != 0) {
                    dprint("p_sps->separate_colour_plane_flag not equal 0.\n");
                }
            }
            dprint("p_sps->chroma_format_idc not equal(%d) 1.\n", sps->chroma_format_idc);
            return HEVC_DEC_ERR;
        }
    }

    sps->pic_width_in_luma_samples   = hevc_ue_v(hevc_ctx->p_bs, "pic_width_in_luma_samples");
    if (sps->pic_width_in_luma_samples > 8192) { /* pic_width_in_luma_samples range is 0 to 8192 */
        dprint("pic_width_in_luma_samples out of range(0,8192).\n");
        return HEVC_DEC_ERR;
    }

    sps->pic_height_in_luma_samples  = hevc_ue_v(hevc_ctx->p_bs, "pic_height_in_luma_samples");
    if (sps->pic_height_in_luma_samples > 4096) { /* pic_height_in_luma_samples range is 0 to 4096 */
        dprint("pic_height_in_luma_samples out of range(0,4096).\n");
        return HEVC_DEC_ERR;
    }

    sps->conformance_window_flag = hevc_u_v(hevc_ctx->p_bs, 1, "conformance_window_flag");

    if (sps->conformance_window_flag) {
        sps->conf_win_left_offset   = hevc_ue_v(hevc_ctx->p_bs, "conf_win_left_offset");
        sps->conf_win_left_offset   = sps->conf_win_left_offset  * g_crop_unit_x[sps->chroma_format_idc];
        sps->conf_win_right_offset  = hevc_ue_v(hevc_ctx->p_bs, "conf_win_right_offset");
        sps->conf_win_right_offset  = sps->conf_win_right_offset * g_crop_unit_x[sps->chroma_format_idc];
        if (sps->pic_width_in_luma_samples < (sps->conf_win_left_offset + sps->conf_win_right_offset)) {
            dprint("conf_win_left_offset+conf_win_right_offset out of range.\n");
            return HEVC_DEC_ERR;
        }

        sps->conf_win_top_offset    = hevc_ue_v(hevc_ctx->p_bs, "conf_win_top_offset");
        sps->conf_win_top_offset    = sps->conf_win_top_offset   * g_crop_unit_y[sps->chroma_format_idc];
        sps->conf_win_bottom_offset = hevc_ue_v(hevc_ctx->p_bs, "conf_win_bottom_offset");
        sps->conf_win_bottom_offset = sps->conf_win_bottom_offset * g_crop_unit_y[sps->chroma_format_idc];
        if (sps->pic_height_in_luma_samples < (sps->conf_win_top_offset + sps->conf_win_bottom_offset)) {
            dprint("conf_win_top_offset+conf_win_bottom_offset out of range.\n");
            return HEVC_DEC_ERR;
        }
    }

    sps->bit_depth_luma  = hevc_ue_v(hevc_ctx->p_bs, "bit_depth_luma_minus8") + 0x8;
    if (sps->bit_depth_luma != 0x8) {
        if (sps->bit_depth_luma < 8 || sps->bit_depth_luma > 10) { /* bit_depth_luma range is 8 to 10 */
            dprint("bit_depth_luma(%d) out of range(8,10).\n", sps->bit_depth_luma);
            return HEVC_DEC_ERR;
        }
    }

    sps->bit_depth_chroma = hevc_ue_v(hevc_ctx->p_bs, "bit_depth_chroma_minus8") + 0x8;
    if (sps->bit_depth_chroma != 0x8) {
        if (sps->bit_depth_chroma < 8 || sps->bit_depth_chroma > 14) { /* bit_depth_chroma range is 8 to 14 */
            dprint("bit_depth_chroma(%d) out of range[8,14].\n", sps->bit_depth_chroma);
            return HEVC_DEC_ERR;
        }
    }

    hevc_ctx->bit_depth_y = sps->bit_depth_luma;
    hevc_ctx->bit_depth_c = sps->bit_depth_chroma;

    sps->qp_bd_offset_y = (sps->bit_depth_luma - 0x8)   * 0x6;
    sps->qp_bd_offset_c = (sps->bit_depth_chroma - 0x8) * 0x6;

    log2_max_pic_order_cnt_lsb_minus4 = hevc_ue_v(hevc_ctx->p_bs, "log2_max_pic_order_cnt_lsb_minus4");
    if (log2_max_pic_order_cnt_lsb_minus4 > 12) { /* log2_max_pic_order_cnt_lsb_minus4 range is 0 to 12 */
        dprint("log2_max_pic_order_cnt_lsb_minus4 out of range[0,12].\n");
        return HEVC_DEC_ERR;
    }

    sps->max_pic_order_cnt_lsb = log2_max_pic_order_cnt_lsb_minus4 + 0x4;
    sps->bits_for_poc = sps->max_pic_order_cnt_lsb;

    sps->sps_sub_layer_ordering_info_present_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                             "sps_sub_layer_ordering_info_present_flag");

    for (i = 0; i <= sps->sps_max_sub_layers_minus1; i++) {
        sps->max_dec_pic_buffering[i] = hevc_ue_v(hevc_ctx->p_bs, "sps_max_dec_pic_buffering_minus1") + 1;
        sps->num_reorder_pics[i]      = hevc_ue_v(hevc_ctx->p_bs, "sps_num_reorder_pics");
        sps->max_latency_increase[i]  = hevc_ue_v(hevc_ctx->p_bs, "sps_max_latency_increase_plus1");
        if (!sps->sps_sub_layer_ordering_info_present_flag) {
            for (i++; i <= sps->sps_max_sub_layers_minus1; i++) {
                sps->max_dec_pic_buffering[i] = sps->max_dec_pic_buffering[0];
                sps->num_reorder_pics[i]      = sps->num_reorder_pics[0];
                sps->max_latency_increase[i]  = sps->max_latency_increase[0];
            }
            break;
        }
    }

    sps->log2_min_luma_coding_block_size_minus3   = hevc_ue_v(hevc_ctx->p_bs, "log2_min_coding_block_size_minus3");
    if (sps->log2_min_luma_coding_block_size_minus3 > 0x3) {
        dprint("log2_min_luma_coding_block_size_minus3 out of range(0,3).\n");
        return HEVC_DEC_ERR;
    }

    sps->log2_diff_max_min_luma_coding_block_size = hevc_ue_v(hevc_ctx->p_bs, "log2_diff_max_min_coding_block_size");

    if (sps->log2_diff_max_min_luma_coding_block_size > 0x3) {
        dprint("log2_diff_max_min_luma_coding_block_size out of range(0,3).\n");
        return HEVC_DEC_ERR;
    }

    sps->log2_min_cb_size_y = sps->log2_min_luma_coding_block_size_minus3 + 3;
    sps->log2_ctb_size_y    = sps->log2_min_cb_size_y + sps->log2_diff_max_min_luma_coding_block_size;
    if (sps->log2_ctb_size_y < 0x4 || sps->log2_ctb_size_y > 0x6) {
        dprint("log2_ctb_size_y out of range(4,6).\n");
        return HEVC_DEC_ERR;
    }

    sps->min_cb_size_y     = 1 << sps->log2_min_cb_size_y;
    sps->ctb_size_y        = 1 << sps->log2_ctb_size_y;
    sps->max_cu_width      = 1 << sps->log2_ctb_size_y;
    sps->max_cu_height     = 1 << sps->log2_ctb_size_y;
    sps->ctb_num_width     = (sps->pic_width_in_luma_samples % sps->max_cu_width) ?
        (sps->pic_width_in_luma_samples / sps->max_cu_width + 1) :
        (sps->pic_width_in_luma_samples / sps->max_cu_width);
    sps->ctb_num_height    = (sps->pic_height_in_luma_samples % sps->max_cu_height) ?
        (sps->pic_height_in_luma_samples / sps->max_cu_height + 1) :
        (sps->pic_height_in_luma_samples / sps->max_cu_height);

    sps->log2_min_transform_block_size_minus2 = hevc_ue_v(hevc_ctx->p_bs, "log2_min_transform_block_size_minus2");
    if (sps->log2_min_transform_block_size_minus2 > 0x3) {
        dprint("log2_min_transform_block_size_minus2 out of range(0,3).\n");
        return HEVC_DEC_ERR;
    }

    sps->log2_diff_max_min_transform_block_size = hevc_ue_v(hevc_ctx->p_bs, "log2_diff_max_min_transform_block_size");
    if (sps->log2_diff_max_min_transform_block_size > 0x3) {
        dprint("log2_diff_max_min_transform_block_size out of range(0,3).\n");
        return HEVC_DEC_ERR;
    }
    sps->quadtree_tu_log2_min_size = sps->log2_min_transform_block_size_minus2 + 0x2;
    if (sps->quadtree_tu_log2_min_size >= sps->log2_min_cb_size_y) {
        dprint("quadtree_tu_log2_min_size not less than log2_min_cb_size_y.\n");
        return HEVC_DEC_ERR;
    }

    sps->quadtree_tu_log2_max_size = sps->quadtree_tu_log2_min_size + sps->log2_diff_max_min_transform_block_size;
    if (sps->quadtree_tu_log2_max_size > 0x5 || sps->quadtree_tu_log2_max_size > sps->log2_ctb_size_y) {
        dprint("quadtree_tu_log2_max_size greater than min( ctb_log2size_y, 5 ).\n");
        return HEVC_DEC_ERR;
    }

    sps->max_transform_hierarchy_depth_inter = hevc_ue_v(hevc_ctx->p_bs, "max_transform_hierarchy_depth_inter");
    if (sps->max_transform_hierarchy_depth_inter > sps->log2_ctb_size_y - sps->quadtree_tu_log2_min_size) {
        dprint("max_transform_hierarchy_depth_inter out of range(0,ctb_log2size_y-log2min_trafo_size).\n");
        return HEVC_DEC_ERR;
    }

    sps->max_transform_hierarchy_depth_intra = hevc_ue_v(hevc_ctx->p_bs, "max_transform_hierarchy_depth_intra");
    if (sps->max_transform_hierarchy_depth_intra > sps->log2_ctb_size_y - sps->quadtree_tu_log2_min_size) {
        dprint("max_transform_hierarchy_depth_intra out of range(0,ctb_log2size_y-log2min_trafo_size).\n");
        return HEVC_DEC_ERR;
    }

    sps->quadtree_tu_max_depth_inter = sps->max_transform_hierarchy_depth_inter + 1;
    sps->quadtree_tu_max_depth_intra = sps->max_transform_hierarchy_depth_intra + 1;

    hevc_ctx->ui_add_cu_depth = 0;
    while (((hi_u32)(sps->max_cu_width >> sps->log2_diff_max_min_luma_coding_block_size))
            > (hi_u32)(1 << (sps->quadtree_tu_log2_min_size + hevc_ctx->ui_add_cu_depth))) {
        hevc_ctx->ui_add_cu_depth++;
    }
    sps->max_cu_depth = sps->log2_diff_max_min_luma_coding_block_size + hevc_ctx->ui_add_cu_depth;

    sps->scaling_list_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1, "scaling_list_enabled_flag");
    if (sps->scaling_list_enabled_flag) {
        sps->sps_scaling_list_data_present_flag = hevc_u_v(hevc_ctx->p_bs, 1, "sps_scaling_list_data_present_flag");
        if (sps->sps_scaling_list_data_present_flag) {
            ret = hevc_dec_scaling_list_data(hevc_ctx, &(sps->scaling_list));
            if (ret != HEVC_DEC_NORMAL) {
                dprint("SPS hevc_dec_scaling_list_data error.\n");
                return HEVC_DEC_ERR;
            }
        }
    }

    sps->amp_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1, "amp_enabled_flag");
    sps->sample_adaptive_offset_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1, "sample_adaptive_offset_enabled_flag");

    sps->pcm_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1, "pcm_enabled_flag");
    if (sps->pcm_enabled_flag) {
        sps->pcm_bit_depth_luma = hevc_u_v(hevc_ctx->p_bs, 0x4, "pcm_sample_bit_depth_luma_minus1") + 1;
        sps->pcm_bit_depth_chroma = hevc_u_v(hevc_ctx->p_bs, 0x4, "pcm_sample_bit_depth_chroma_minus1") + 1;

        sps->log2_min_pcm_coding_block_size_minus3 = hevc_ue_v(hevc_ctx->p_bs,
                                                               "log2_min_pcm_luma_coding_block_size_minus3");

        if (sps->log2_min_pcm_coding_block_size_minus3 > 0x2) {
            dprint("log2_min_pcm_coding_block_size_minus3(%d) out of range[0,2].\n",
                   sps->log2_min_pcm_coding_block_size_minus3);
            /* return HEVC_DEC_ERR; */
        }

        sps->log2_diff_max_min_pcm_coding_block_size = hevc_ue_v(hevc_ctx->p_bs,
                                                                 "log2_diff_max_min_pcm_luma_coding_block_size");
        if (sps->log2_diff_max_min_pcm_coding_block_size > 0x2) {
            dprint("log2_diff_max_min_pcm_coding_block_size(%d) out of range[0,2].\n",
                   sps->log2_diff_max_min_pcm_coding_block_size);
            /* return HEVC_DEC_ERR; */
        }
        sps->pcm_log2_min_size = sps->log2_min_pcm_coding_block_size_minus3 + 0x3;
        sps->pcm_log2_max_size = sps->pcm_log2_min_size + sps->log2_diff_max_min_pcm_coding_block_size;
        if (sps->pcm_log2_max_size > 0x5 || sps->pcm_log2_max_size > sps->log2_ctb_size_y) {
            dprint("pcm_log2_max_size greater than min( ctb_log2size_y, 5 ).\n");
            /* return HEVC_DEC_ERR; */
        }

        sps->pcm_loop_filter_disable_flag = hevc_u_v(hevc_ctx->p_bs, 1, "pcm_loop_filter_disable_flag");
    }

    sps->num_short_term_ref_pic_sets  = hevc_ue_v(hevc_ctx->p_bs, "num_short_term_ref_pic_sets");
    if (sps->num_short_term_ref_pic_sets > 64) { /* num_short_term_ref_pic_sets range is 0 to 64 */
        dprint("num_short_term_ref_pic_sets out of range[0,64].\n");
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < sps->num_short_term_ref_pic_sets; i++) {
        /* get short term reference picture sets */
        ret = hevc_dec_short_term_ref_pic_set(hevc_ctx, sps, &(sps->short_term_ref_pic_set[i]), i);
        if (ret != HEVC_DEC_NORMAL) {
            dprint("SPS hevc_dec_short_term_ref_pic_set error.\n");
            return HEVC_DEC_ERR;
        }
    }

    sps->long_term_ref_pics_present_flag = hevc_u_v(hevc_ctx->p_bs, 1, "long_term_ref_pics_present_flag");
    if (sps->long_term_ref_pics_present_flag) {
        sps->num_long_term_ref_pic_sps = hevc_ue_v(hevc_ctx->p_bs, "num_long_term_ref_pic_sps");
        if (sps->num_long_term_ref_pic_sps > HEVC_MAX_LSB_NUM - 1) {
            dprint("num_long_term_ref_pic_sps out of range[0,32].\n");
            return HEVC_DEC_ERR;
        }

        for (i = 0; i < sps->num_long_term_ref_pic_sps; i++) {
            sps->lt_ref_pic_poc_lsb_sps[i] = hevc_u_v(hevc_ctx->p_bs, sps->max_pic_order_cnt_lsb,
                                                      "lt_ref_pic_poc_lsb_sps");
            sps->used_by_curr_pic_lt_sps_flag[i] = hevc_u_v(hevc_ctx->p_bs, 1, "used_by_curr_pic_lt_sps_flag");
        }
    }
    sps->sps_temporal_mvp_enable_flag = hevc_u_v(hevc_ctx->p_bs, 1, "sps_temporal_mvp_enable_flag");
    sps->sps_strong_intra_smoothing_enable_flag = hevc_u_v(hevc_ctx->p_bs, 1, "sps_strong_intra_smoothing_enable_flag");

    sps->is_refresh = 1;

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_sps(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;
    hi_u32 video_parameter_set_id;
    hi_u32 sps_max_sub_layers_minus1;
    hi_u8 sps_temporal_id_nesting_flag;
    hi_u32 sps_seq_parameter_set_id;
    hevc_profile_tier_level profile_tier_level;
    hevc_seq_param_set *sps_tmp = NULL;

    pos();

    sps_tmp = &hevc_ctx->tmp_param.sps_tmp;
    memset(sps_tmp, 0, sizeof(hevc_seq_param_set));
    memset(&profile_tier_level, 0, sizeof(hevc_profile_tier_level));

    video_parameter_set_id    = hevc_u_v(hevc_ctx->p_bs, 0x4, "sps_video_parameter_set_id");
    if (video_parameter_set_id >= (hevc_ctx->max_vps_num)) {
        dprint("sps_video_parameter_set_id out of range(0,%d).\n", hevc_ctx->max_vps_num);
        return HEVC_DEC_ERR;
    }

    sps_max_sub_layers_minus1 = hevc_u_v(hevc_ctx->p_bs, 0x3, "sps_max_sub_layers_minus1");
    if (sps_max_sub_layers_minus1 > HEVC_MAX_TEMPLAYER) {
        dprint("sps_max_sub_layers_minus1 out of range(0,6).\n");
        return HEVC_DEC_ERR;
    }
    sps_temporal_id_nesting_flag = hevc_u_v(hevc_ctx->p_bs, 1, "sps_temporal_id_nesting_flag");

    ret = hevc_dec_ptl(hevc_ctx, &(profile_tier_level), 1, sps_max_sub_layers_minus1);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("SPS hevc_dec_ptl error.\n");
        return HEVC_DEC_ERR;
    }

    sps_seq_parameter_set_id = hevc_ue_v(hevc_ctx->p_bs, "sps_seq_parameter_set_id");
    if (sps_seq_parameter_set_id >= hevc_ctx->max_sps_num) {
        dprint("sps_seq_parameter_set_id out of range(0,%d).\n", hevc_ctx->max_sps_num);
        return HEVC_DEC_ERR;
    }

    if (hevc_ctx->p_sps[sps_seq_parameter_set_id].valid) {
        sps_tmp->video_parameter_set_id  = video_parameter_set_id;
        sps_tmp->sps_max_sub_layers_minus1  = sps_max_sub_layers_minus1;
        sps_tmp->sps_temporal_id_nesting_flag  = sps_temporal_id_nesting_flag;
        memcpy(&(sps_tmp->profile_tier_level), &(profile_tier_level), sizeof(hevc_profile_tier_level));
        sps_tmp->seq_parameter_set_id  = sps_seq_parameter_set_id;

        if (hevc_process_sps(hevc_ctx, sps_tmp) != HEVC_DEC_NORMAL) {
            /* 此时p_hevc_ctx->sps[sps_seq_parameter_set_id]为前一个可用 */
            memcpy(&hevc_ctx->seisps, &hevc_ctx->p_sps[sps_seq_parameter_set_id], sizeof(hevc_seq_param_set));
            dprint("SPS[%d] decode error0.\n", sps_seq_parameter_set_id);
            return HEVC_DEC_ERR;
        }

        /*
         * If you do not make an equal judgment, as long as it can be decoded correctly, then copy it,
         * because the equal judgment is also complicated.
         */
        sps_tmp->is_refresh = 1;
        sps_tmp->valid = 1;
        memmove(&(hevc_ctx->p_sps[sps_seq_parameter_set_id]), sps_tmp, sizeof(hevc_seq_param_set));
        memcpy(&hevc_ctx->seisps, &hevc_ctx->p_sps[sps_seq_parameter_set_id], sizeof(hevc_seq_param_set));
    } else {
        hevc_ctx->p_sps[sps_seq_parameter_set_id].video_parameter_set_id  = video_parameter_set_id;
        hevc_ctx->p_sps[sps_seq_parameter_set_id].sps_max_sub_layers_minus1  = sps_max_sub_layers_minus1;
        hevc_ctx->p_sps[sps_seq_parameter_set_id].sps_temporal_id_nesting_flag  = sps_temporal_id_nesting_flag;
        memcpy(&(hevc_ctx->p_sps[sps_seq_parameter_set_id].profile_tier_level), &(profile_tier_level),
               sizeof(hevc_profile_tier_level));
        hevc_ctx->p_sps[sps_seq_parameter_set_id].seq_parameter_set_id  = sps_seq_parameter_set_id;

        if (hevc_process_sps(hevc_ctx, &(hevc_ctx->p_sps[sps_seq_parameter_set_id])) != HEVC_DEC_NORMAL) {
            dprint("SPS[%d] decode error.\n", sps_seq_parameter_set_id);
            hevc_ctx->p_sps[sps_seq_parameter_set_id].is_refresh = 1;
            hevc_ctx->p_sps[sps_seq_parameter_set_id].valid = 0;
            return HEVC_DEC_ERR;
        }
        hevc_ctx->p_sps[sps_seq_parameter_set_id].is_refresh = 1;
        hevc_ctx->p_sps[sps_seq_parameter_set_id].valid = 1;
        memcpy(&hevc_ctx->seisps, &hevc_ctx->p_sps[sps_seq_parameter_set_id], sizeof(hevc_seq_param_set));
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_process_pps(hevc_ctx_info *hevc_ctx, hevc_pic_param_set *pps)
{
    hevc_seq_param_set *sps;
    hi_s32 init_qp_value;
    hi_s32 ret;
    hi_s32 i;

    pps->loop_filter_across_tiles_enabled_flag = 1;
    pps->num_tile_columns = 1;
    pps->num_tile_rows = 1;
    pps->seq_parameter_set_id = hevc_ue_v(hevc_ctx->p_bs, "pps_seq_parameter_set_id");
    if (pps->seq_parameter_set_id < 0 || pps->seq_parameter_set_id >= (hevc_ctx->max_sps_num)) {
        dprint("pic_parameter_set_id(%d) out of range[0,15].\n", pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    } else if (hevc_ctx->p_sps[pps->seq_parameter_set_id].valid == 0) {
        dprint("SPS(%d) haven't decode.\n", pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];

    pps->dependent_slice_segments_enabled_flag  = hevc_u_v(hevc_ctx->p_bs, 1, "dependent_slice_segments_enabled_flag");
    pps->output_flag_present_flag               = hevc_u_v(hevc_ctx->p_bs, 1, "output_flag_present_flag");
    pps->num_extra_slice_header_bits            = hevc_u_v(hevc_ctx->p_bs, 0x3, "num_extra_slice_header_bits");
    pps->sign_data_hiding_flag                  = hevc_u_v(hevc_ctx->p_bs, 1, "sign_data_hiding_flag");
    pps->cabac_init_present_flag                = hevc_u_v(hevc_ctx->p_bs, 1, "cabac_init_present_flag");

    pps->num_ref_idx_l0_default_active = 1 + hevc_ue_v(hevc_ctx->p_bs, "num_ref_idx_l0_default_active_minus1");
    if (pps->num_ref_idx_l0_default_active < 0 || pps->num_ref_idx_l0_default_active > 15) { /* range is 0 to 15 */
        dprint("num_ref_idx_l0_default_active out of range[0,15].\n");
        return HEVC_DEC_ERR;
    }

    pps->num_ref_idx_l1_default_active = 1 + hevc_ue_v(hevc_ctx->p_bs, "num_ref_idx_l1_default_active_minus1");
    if (pps->num_ref_idx_l1_default_active < 0 || pps->num_ref_idx_l1_default_active > 15) { /* range is 0 to 15 */
        dprint("num_ref_idx_l1_default_active out of range[0,15].\n");
        return HEVC_DEC_ERR;
    }

    init_qp_value = hevc_se_v(hevc_ctx->p_bs, "init_qp_minus26");
    if (init_qp_value == 0x7fffffff) {
        dprint("hevc_se_v failed.\n");
        return HEVC_DEC_ERR;
    }
    pps->pic_init_qp = 26 + init_qp_value;
    if (pps->pic_init_qp < (hi_s32)(-(sps->qp_bd_offset_y)) ||
        pps->pic_init_qp > 51) { /* pic_init_qp range is (-sps->qp_bd_offset_y, 51) */
        dprint("pic_init_qp(%d) out of range[-%d,51].\n", pps->pic_init_qp, (sps->qp_bd_offset_y));
        return HEVC_DEC_ERR;
    }

    pps->constrained_intra_pred_flag   = hevc_u_v(hevc_ctx->p_bs, 1, "constrained_intra_pred_flag");
    pps->transform_skip_enabled_flag   = hevc_u_v(hevc_ctx->p_bs, 1, "transform_skip_enabled_flag");
    pps->cu_qp_delta_enabled_flag      = hevc_u_v(hevc_ctx->p_bs, 1, "cu_qp_delta_enabled_flag");

    if (pps->cu_qp_delta_enabled_flag) {
        pps->diff_cu_qp_delta_depth    = hevc_ue_v(hevc_ctx->p_bs, "diff_cu_qp_delta_depth");
    } else {
        pps->diff_cu_qp_delta_depth    = 0;
    }

    if (pps->diff_cu_qp_delta_depth < 0 || pps->diff_cu_qp_delta_depth > 0x3) {
        dprint("diff_cu_qp_delta_depth out of range[0,3].\n");
        return HEVC_DEC_ERR;
    }
    pps->max_cu_qp_delta_depth         = pps->diff_cu_qp_delta_depth;

    pps->pic_cb_qp_offset              = hevc_se_v(hevc_ctx->p_bs, "pps_cb_qp_offset");
    if (pps->pic_cb_qp_offset < -12 || pps->pic_cb_qp_offset > 12) { /* pic_cb_qp_offset range is -12 to 12 */
        dprint("pic_cb_qp_offset out of range[-12,12].\n");
        return HEVC_DEC_ERR;
    }

    pps->pic_cr_qp_offset              = hevc_se_v(hevc_ctx->p_bs, "pps_cr_qp_offset");
    if (pps->pic_cr_qp_offset < -12 || pps->pic_cr_qp_offset > 12) { /* pic_cr_qp_offset range is -12 to 12 */
        dprint("pic_cr_qp_offset out of range[-12,12].\n");
        return HEVC_DEC_ERR;
    }

    pps->pic_slice_chroma_qp_offsets_present_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                             "pps_slice_chroma_qp_offsets_present_flag");
    pps->weighted_pred_flag                       = hevc_u_v(hevc_ctx->p_bs, 1, "weighted_pred_flag");
    pps->weighted_bipred_flag                     = hevc_u_v(hevc_ctx->p_bs, 1, "weighted_bipred_flag");
    pps->transquant_bypass_enable_flag            = hevc_u_v(hevc_ctx->p_bs, 1, "transquant_bypass_enable_flag");
    pps->tiles_enabled_flag                       = hevc_u_v(hevc_ctx->p_bs, 1, "tiles_enabled_flag");
    pps->entropy_coding_sync_enabled_flag         = hevc_u_v(hevc_ctx->p_bs, 1, "entropy_coding_sync_enabled_flag");

    if (pps->tiles_enabled_flag) {
        pps->num_tile_columns          = hevc_ue_v(hevc_ctx->p_bs, "num_tile_columns_minus1") + 1;
        if (pps->num_tile_columns <= 0 || pps->num_tile_columns > HEVC_MAX_TILE_COLUMNS) {
            dprint("num_tile_columns(%d) out of range(0,%d).\n", pps->num_tile_columns, HEVC_MAX_TILE_COLUMNS);
            return HEVC_DEC_ERR;
        }

        pps->num_tile_rows             = hevc_ue_v(hevc_ctx->p_bs, "num_tile_rows_minus1") + 1;
        if (pps->num_tile_rows <= 0 || pps->num_tile_rows > HEVC_MAX_TILE_ROWS) {
            dprint("num_tile_rows out(%d) of range(0,%d).\n", pps->num_tile_rows, HEVC_MAX_TILE_ROWS);
            return HEVC_DEC_ERR;
        }

        pps->uniform_spacing_flag      = hevc_u_v(hevc_ctx->p_bs, 1, "uniform_spacing_flag");

        if (!pps->uniform_spacing_flag) {
            for (i = 0; i < (pps->num_tile_columns - 1); i++) {
                pps->column_width[i] = hevc_ue_v(hevc_ctx->p_bs, "column_width_minus1") + 1;
                // 宽度约束
                if (pps->column_width[i] < 0 || pps->column_width[i] > sps->ctb_num_width) {
                    dprint("column_width[%d](%d) out of range (0 %d)\n", i, pps->column_width[i], sps->ctb_num_width);
                    return HEVC_DEC_ERR;
                }
            }
            for (i = 0; i < (pps->num_tile_rows - 1); i++) {
                pps->row_height[i] = hevc_ue_v(hevc_ctx->p_bs, "row_height_minus1") + 1;
                // 高度约束
                if (pps->row_height[i] < 0 || pps->row_height[i] > sps->ctb_num_height) {
                    dprint("row_height[%d](%d) out of range (0 %d)\n", i, pps->row_height[i], sps->ctb_num_height);
                    return HEVC_DEC_ERR;
                }
            }
        }

        if (pps->uniform_spacing_flag) {
            for (i = 0; i < pps->num_tile_columns; i++) {
                pps->column_bd[i] = (i + 1) * sps->ctb_num_width / pps->num_tile_columns - (i * sps->ctb_num_width) /
                                     pps->num_tile_columns;
            }
            for (i = 0; i < pps->num_tile_rows; i++) {
                pps->row_bd[i] = (i + 1) * sps->ctb_num_height / pps->num_tile_rows - (i * sps->ctb_num_height) /
                                  pps->num_tile_rows;
            }
        } else {
            pps->column_bd[pps->num_tile_columns - 1] = sps->ctb_num_width;
            for (i = 0; i < pps->num_tile_columns - 1; i++) {
                pps->column_bd[i] = pps->column_width[i];
                pps->column_bd[pps->num_tile_columns - 1] -= pps->column_bd[i];
            }

            if (pps->column_bd[pps->num_tile_columns - 1] <= 0) {
                dprint("column_bd[%d](%d) <= 0, invalid!\n", pps->num_tile_columns - 1,
                       pps->column_bd[pps->num_tile_columns - 1]);
                return HEVC_DEC_ERR;
            }

            pps->row_bd[pps->num_tile_rows - 1] = sps->ctb_num_height;
            for (i = 0; i < pps->num_tile_rows - 1; i++) {
                pps->row_bd[i] = pps->row_height[i];
                pps->row_bd[pps->num_tile_rows - 1] -= pps->row_bd[i];
            }

            if (pps->row_bd[pps->num_tile_rows - 1] <= 0) {
                dprint("row_bd[%d](%d) <= 0, invalid!\n", pps->num_tile_rows - 1, pps->row_bd[pps->num_tile_rows - 1]);
                return HEVC_DEC_ERR;
            }
        }

        if (pps->num_tile_columns != 1 || pps->num_tile_rows != 1) {
            pps->loop_filter_across_tiles_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                  "loop_filter_across_tiles_enabled_flag");
        }
    } else {
        pps->column_bd[0] = sps->ctb_num_width;
        pps->row_bd[0] = sps->ctb_num_height;
    }

    for (i = 0; i < pps->num_tile_columns; i++) {
        if (pps->column_bd[i] * sps->max_cu_width <= 64 && sps->pic_width_in_luma_samples > 64) {
            dprint("PPS tile width(%d) is too small.(logic unsupport)\n", pps->column_bd[i] * sps->max_cu_width);
            return HEVC_DEC_ERR;
        }
    }
    for (i = 0; i < pps->num_tile_rows; i++) {
        if (pps->row_bd[i] * sps->max_cu_width < 64) {
            dprint("PPS tile height(%d) is too small.(logic unsupport)\n", pps->row_bd[i] * sps->max_cu_width);
            return HEVC_DEC_ERR;
        }
    }

    pps->loop_filter_across_slices_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1, "loop_filter_across_slices_enabled_flag");
    pps->deblocking_filter_control_present_flag = hevc_u_v(hevc_ctx->p_bs, 1, "deblocking_filter_control_present_flag");
    if (pps->deblocking_filter_control_present_flag) {
        pps->deblocking_filter_override_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                "deblocking_filter_override_enabled_flag");
        pps->pic_disable_deblocking_filter_flag = hevc_u_v(hevc_ctx->p_bs, 1, "pps_disable_deblocking_filter_flag");
        if (!pps->pic_disable_deblocking_filter_flag) {
            pps->pps_beta_offset_div2 = hevc_se_v(hevc_ctx->p_bs, "pps_beta_offset_div2");
            if (pps->pps_beta_offset_div2 < -6 || /* pps_beta_offset_div2 range is  -6 to 6 */
                pps->pps_beta_offset_div2 > 6) { /* pps_beta_offset_div2 range is -6 to 6 */
                dprint("pps_beta_offset_div2(%d) out of range(-6,6).\n", pps->pps_beta_offset_div2);
                pps->pps_beta_offset_div2 = 0;
            }
            pps->pps_tc_offset_div2   = hevc_se_v(hevc_ctx->p_bs, "pps_tc_offset_div2");
            if (pps->pps_tc_offset_div2 < -6 || pps->pps_tc_offset_div2 > 6) { /* pps_tc_offse_div2 range is -6 to  6 */
                dprint("pps_tc_offset_div2(%d) out of range(-6,6).\n", pps->pps_tc_offset_div2);
                pps->pps_tc_offset_div2 = 0;
            }
        }
    }

    pps->pic_scaling_list_data_present_flag = hevc_u_v(hevc_ctx->p_bs, 1, "pps_scaling_list_data_present_flag");
    if (pps->pic_scaling_list_data_present_flag) {
        ret = hevc_dec_scaling_list_data(hevc_ctx, &(pps->scaling_list));
        if (ret != HEVC_DEC_NORMAL) {
            dprint("PPS hevc_dec_scaling_list_data error.\n");
            return HEVC_DEC_ERR;
        }
    }
    pps->lists_modification_present_flag = hevc_u_v(hevc_ctx->p_bs, 1, "lists_modification_present_flag");
    pps->log2_parallel_merge_level = hevc_ue_v(hevc_ctx->p_bs, "log2_parallel_merge_level_minus2") + 0x2;
    if (pps->log2_parallel_merge_level < 0x2 || pps->log2_parallel_merge_level > sps->log2_ctb_size_y) {
        dprint("log2_parallel_merge_level(%d) out of range(2,log2_ctb_size_y).\n", pps->log2_parallel_merge_level);
        return HEVC_DEC_ERR;
    }
    pps->slice_segment_header_extension_present_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                "slice_segment_header_extension_present_flag");
    pps->pps_extension_flag        = hevc_u_v(hevc_ctx->p_bs, 1, "pps_extension_flag");

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_pps(hevc_ctx_info *hevc_ctx)
{
    hi_u32 pic_parameter_set_id;
    hevc_pic_param_set *pps_tmp = NULL;

    pos();

    pps_tmp = &hevc_ctx->tmp_param.pps_tmp;
    memset(pps_tmp, 0, sizeof(hevc_pic_param_set));

    pic_parameter_set_id = hevc_ue_v(hevc_ctx->p_bs, "pps_pic_parameter_set_id");
    if ((pic_parameter_set_id >= (hevc_ctx->max_pps_num)) ||
            (pic_parameter_set_id >= sizeof(hevc_ctx->p_pps) / sizeof(hevc_ctx->p_pps[0]))) {
        dprint("pic_parameter_set_id(%d) out of range(0,63).\n", pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }
    if (hevc_ctx->p_pps[pic_parameter_set_id].valid) { // 曾经解对过该id对应的pps
        pps_tmp->pic_parameter_set_id = pic_parameter_set_id;
        if (hevc_process_pps(hevc_ctx, pps_tmp) != HEVC_DEC_NORMAL) {
            dprint("PPS[%d] decode error.\n", pic_parameter_set_id);
            return HEVC_DEC_ERR;
        }

        /*
         * If you do not make an equal judgment, as long as it can be decoded correctly, then copy it,
         * because the equal judgment is also complicated.
         */
        pps_tmp->is_refresh = 1;
        pps_tmp->valid = 1;
        memmove(&(hevc_ctx->p_pps[pic_parameter_set_id]), pps_tmp, sizeof(hevc_pic_param_set));
    } else {
        hevc_ctx->p_pps[pic_parameter_set_id].pic_parameter_set_id = pic_parameter_set_id;
        if (hevc_process_pps(hevc_ctx, &(hevc_ctx->p_pps[pic_parameter_set_id])) != HEVC_DEC_NORMAL) {
            dprint("PPS[%d] decode error.\n", pic_parameter_set_id);
            hevc_ctx->p_pps[pic_parameter_set_id].is_refresh = 1;
            hevc_ctx->p_pps[pic_parameter_set_id].valid = 0;
            return HEVC_DEC_ERR;
        }
        hevc_ctx->p_pps[pic_parameter_set_id].is_refresh = 1;
        hevc_ctx->p_pps[pic_parameter_set_id].valid = 1;
    }

    return HEVC_DEC_NORMAL;
}


static hi_void hevc_applay_reference_picture_set(hevc_ctx_info *hevc_ctx)
{
    hi_s32 i, j;
    hi_s32 cur_poc;
    hi_s32 ref_poc;
    hi_s32 is_reference;
    hi_s32 poc_cycle;
    hevc_short_term_rpset *temp_rps = NULL;
    hevc_pic_param_set    *pps = NULL;
    hevc_seq_param_set    *sps = NULL;

    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];
    poc_cycle = 1 << sps->max_pic_order_cnt_lsb;

    temp_rps = &(hevc_ctx->curr_slice.short_term_ref_pic_set);
    for (i = 0; i < hevc_ctx->dpb.used_size; i++) {
        is_reference = 0;

        // long term reference
        for (j = temp_rps->num_negative_pics + temp_rps->num_positive_pics; j < temp_rps->num_of_pics; j++) {
            if (hevc_ctx->curr_slice.check_ltmsb[j]) {
                if ((hevc_ctx->dpb.fs[i]->frame.is_long_term) &&
                        (hevc_ctx->dpb.fs[i]->poc == temp_rps->poc[j])) {
                    is_reference = 1;
                    hevc_ctx->dpb.fs[i]->is_reference = 1;
                    hevc_ctx->dpb.fs[i]->frame_store_state = FS_IN_DPB;
                }
            } else if ((hevc_ctx->dpb.fs[i]->frame.is_long_term)
                       && ((hevc_ctx->dpb.fs[i]->poc % poc_cycle) == (temp_rps->poc[j] % poc_cycle))) {
                is_reference = 1;
                hevc_ctx->dpb.fs[i]->is_reference = 1;
                hevc_ctx->dpb.fs[i]->frame_store_state = FS_IN_DPB;
            }

            if (0 == is_reference) {
                cur_poc = hevc_ctx->dpb.fs[i]->poc;
                ref_poc = temp_rps->poc[j];

                if (!hevc_ctx->curr_slice.check_ltmsb[j]) {
                    cur_poc = cur_poc % poc_cycle;
                    ref_poc = ref_poc % poc_cycle;
                }

                if (hevc_ctx->dpb.fs[i]->is_reference && cur_poc == ref_poc) {
                    is_reference = 1;
                    hevc_ctx->dpb.fs[i]->is_reference = 1;
                    hevc_ctx->dpb.fs[i]->frame.is_long_term = 1;
                    hevc_ctx->dpb.fs[i]->frame_store_state = FS_IN_DPB;
                }
            }
        }

        // short term reference
        for (j = 0; j < (temp_rps->num_negative_pics + temp_rps->num_positive_pics); j++) {
            if ((!hevc_ctx->dpb.fs[i]->frame.is_long_term) &&
                    (hevc_ctx->dpb.fs[i]->poc == (hevc_ctx->curr_slice.poc + temp_rps->delta_poc[j]))) {
                is_reference = 1;
                hevc_ctx->dpb.fs[i]->is_reference = 1;
                hevc_ctx->dpb.fs[i]->frame_store_state = FS_IN_DPB;
            }
        }

        // mark the picture as "unused for reference" if it is not in
        // the reference picture set
        if ((hevc_ctx->dpb.fs[i]->poc != hevc_ctx->curr_slice.poc) && (0 == is_reference)) {
            hevc_ctx->dpb.fs[i]->is_reference = 0;
            hevc_ctx->dpb.fs[i]->frame.is_long_term = 0;
        }
    }

    return;
}

static hi_void hevc_update_ltreflist(hevc_ctx_info *hevc_ctx)
{
    hi_s32 i = 0;
    hi_s32 j = 0;
    hi_s32 k = 0;
    hi_s32 rps_poc;
    hi_s32 cur_poc;
    hi_u8  delta_poc_msb_cycle_lt_flag;
    hi_s32 poc_cycle;
    hevc_short_term_rpset *rps = NULL;
    hevc_pic_param_set    *pps = NULL;
    hevc_seq_param_set    *sps = NULL;

    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];
    poc_cycle = 1 << sps->max_pic_order_cnt_lsb;

    rps = &(hevc_ctx->curr_slice.short_term_ref_pic_set);
    for (i = (hi_s32)rps->num_of_pics - 1; i > (hi_s32)(rps->num_negative_pics + rps->num_positive_pics - 1); i--) {
        if (rps->used_flag[i]) {
            rps_poc = rps->poc[i];
            delta_poc_msb_cycle_lt_flag = hevc_ctx->curr_slice.check_ltmsb[i];
            rps_poc = delta_poc_msb_cycle_lt_flag ? rps_poc : rps_poc & (poc_cycle - 1);

            for (j = 0; j < (hi_s32)hevc_ctx->dpb.used_size; j++) {
                cur_poc = hevc_ctx->dpb.fs[j]->poc;
                cur_poc = delta_poc_msb_cycle_lt_flag ? cur_poc : cur_poc & (poc_cycle - 1);
                if (cur_poc == rps_poc) {
                    hevc_ctx->dpb.fs[j]->is_reference = 1;

                    hevc_ctx->dpb.fs[j]->frame.is_short_term = 0;
                    hevc_ctx->dpb.fs[j]->frame.is_long_term  = 1;
                    hevc_ctx->dpb.fs_ltref[k++] = hevc_ctx->dpb.fs[j];
                    break;
                }
            }
        }
    }
    hevc_ctx->dpb.ltref_frames_in_buffer = k;
    while (k < (hi_s32)hevc_ctx->dpb.size) {
        hevc_ctx->dpb.fs_ltref[k++] = (hevc_framestore *)NULL;
    }

    return;
}


/************************************************************************
*      函数原型 :  hi_void HEVC_UpdateReflist()
*      功能描述 :  更新DPB中短期参考列表
*      参数说明 :  无
*      返回值   ： 无
************************************************************************/
static hi_void hevc_update_reflist(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i = 0, j = 0, k = 0;
    hevc_short_term_rpset *rps = NULL;
    hi_s32 poc;
    hi_s32 val;

    rps = &(hevc_ctx->curr_slice.short_term_ref_pic_set);
    for (i = 0; i < rps->num_negative_pics; i++) {
        if (rps->used_flag[i]) {
            poc = hevc_ctx->curr_slice.poc + rps->delta_poc[i];
            for (j = 0; j < hevc_ctx->dpb.used_size; j++) {
                if (poc == hevc_ctx->dpb.fs[j]->poc) {
                    hevc_ctx->dpb.fs[j]->is_reference = 1;
                    hevc_ctx->dpb.fs[j]->frame.is_short_term = 1;
                    hevc_ctx->dpb.fs[j]->frame.is_long_term  = 0;
                    hevc_ctx->dpb.fs_negative_ref[k++] = hevc_ctx->dpb.fs[j];
                    break;
                }
            }
        }
    }
    hevc_ctx->dpb.negative_ref_frames_in_buffer = k;
    while (k < (hi_s32)hevc_ctx->dpb.size) {
        hevc_ctx->dpb.fs_negative_ref[k++] = (hevc_framestore *)NULL;
    }

    k = 0;
    val = rps->num_negative_pics + rps->num_positive_pics;
    for (; i < val; i++) {
        if (rps->used_flag[i]) {
            poc = hevc_ctx->curr_slice.poc + rps->delta_poc[i];
            for (j = 0; j < hevc_ctx->dpb.used_size; j++) {
                if (poc == hevc_ctx->dpb.fs[j]->poc) {
                    hevc_ctx->dpb.fs[j]->is_reference = 1;
                    hevc_ctx->dpb.fs[j]->frame.is_short_term = 1;
                    hevc_ctx->dpb.fs[j]->frame.is_long_term  = 0;
                    hevc_ctx->dpb.fs_positive_ref[k++] = hevc_ctx->dpb.fs[j];
                    break;
                }
            }
        }
    }
    hevc_ctx->dpb.positive_ref_frames_in_buffer = k;

    while (k < hevc_ctx->dpb.size) {
        hevc_ctx->dpb.fs_positive_ref[k++] = (hevc_framestore *)NULL;
    }

    return;
}

static hi_s32 hevc_check_that_all_ref_pics_are_available(hevc_ctx_info *hevc_ctx, hi_s32 *pi_lost_poc)
{
    hi_s32 i, j, is_available;
    hi_s32 at_least_one_lost = 0;
    hi_s32 at_least_one_removed = 0;
    hi_s32 i_poc_lost = 0;
    hi_s32 poc_cycle;
    hevc_short_term_rpset *temp_rps = NULL;
    hevc_pic_param_set    *pps = NULL;
    hevc_seq_param_set    *sps = NULL;

    if ((hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA)
            || (hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)
            || (hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL)) {
        return HEVC_DEC_NORMAL;
    }

    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];
    poc_cycle = 1 << sps->max_pic_order_cnt_lsb;

    temp_rps = &(hevc_ctx->curr_slice.short_term_ref_pic_set);

    if (temp_rps->num_negative_pics + temp_rps->num_positive_pics >= HEVC_MAX_NUM_REF_PICS) {
        HI_ERR_DEMUX("num_negative_pics(%d) or num_positive_pics(%d) invalid!\n", temp_rps->num_negative_pics,
                     temp_rps->num_positive_pics);
        return HEVC_DEC_ERR;
    }

    // loop through all long-term pictures in the reference picture set
    // to see if the picture should be kept as reference picture
    for (i = temp_rps->num_negative_pics + temp_rps->num_positive_pics; i < temp_rps->num_of_pics; i++) {
        is_available = 0;
        // loop through all pictures in the reference picture buffer
        for (j = 0; j < hevc_ctx->dpb.used_size; j++) {
            if (hevc_ctx->curr_slice.check_ltmsb[i]) {
                if (hevc_ctx->dpb.fs[j]->frame.is_long_term && hevc_ctx->dpb.fs[j]->poc == temp_rps->poc[i] &&
                        hevc_ctx->dpb.fs[j]->is_reference) {
                    is_available = 1;
                }
            } else {
                if (hevc_ctx->dpb.fs[j]->frame.is_long_term &&
                    ((hevc_ctx->dpb.fs[j]->poc % poc_cycle) == (temp_rps->poc[i] % poc_cycle)) &&
                    hevc_ctx->dpb.fs[j]->is_reference) {
                    is_available = 1;
                }
            }
        }

        // report that a picture is lost if it is in the reference picture set
        // but not available as reference picture
        if (is_available == 0) {
            if (hevc_ctx->curr_slice.poc + temp_rps->delta_poc[i] >= hevc_ctx->poc_random_access) {
                if (!temp_rps->used_flag[i]) {
                    at_least_one_removed = 1;
                } else {
                    at_least_one_lost = 1;
                    i_poc_lost = hevc_ctx->curr_slice.poc + temp_rps->delta_poc[i];
                }
            }
        }
    }

    // loop through all short-term pictures in the reference picture set
    // to see if the picture should be kept as reference picture
    for (i = 0; i < temp_rps->num_negative_pics + temp_rps->num_positive_pics; i++) {
        is_available = 0;
        // loop through all pictures in the reference picture buffer
        for (j = 0; j < hevc_ctx->dpb.used_size; j++) {
            if (!hevc_ctx->dpb.fs[j]->frame.is_long_term &&
                hevc_ctx->dpb.fs[j]->poc == hevc_ctx->curr_slice.poc + temp_rps->delta_poc[i] &&
                hevc_ctx->dpb.fs[j]->is_reference) {
                is_available = 1;
            }
        }
        // report that a picture is lost if it is in the reference picture set
        // but not available as reference picture
        if (is_available == 0) {
            if (hevc_ctx->curr_slice.poc + temp_rps->delta_poc[i] >= hevc_ctx->poc_random_access) {
                if (!temp_rps->used_flag[i]) {
                    at_least_one_removed = 1;
                } else {
                    at_least_one_lost = 1;
                    i_poc_lost = hevc_ctx->curr_slice.poc + temp_rps->delta_poc[i];
                }
            }
        }
    }

    if (at_least_one_lost) {
        *pi_lost_poc = i_poc_lost;
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

static hi_void hevc_remove_frame_store_out_dpb(hevc_ctx_info *hevc_ctx, hi_u32 pos)
{
    hi_u32 i;

    hevc_ctx->dpb.fs[pos]->frame_store_state = FS_NOT_USED;
    hevc_ctx->dpb.fs[pos]->is_reference = 0;
    hevc_ctx->dpb.fs[pos] = (hevc_framestore *)NULL;

    /* frame sort */
    for (i = pos; i <= (hevc_ctx->dpb.used_size - 1); i++) {
        hevc_ctx->dpb.fs[i] = hevc_ctx->dpb.fs[i + 1];
    }
    hevc_ctx->dpb.fs[hevc_ctx->dpb.used_size - 1] = (hevc_framestore *)NULL;
    hevc_ctx->dpb.used_size--;

    return;
}

static hi_s32 hevc_remove_un_used_frame_store(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i, j, record_used_size;
    hi_s32 is_removed = HEVC_FALSE;

    // check for frames that were already output and no longer used for reference
    /* DPB.used_size不能直接作为如下循环的控制变量，这是因为Remove操作本身会使used_size减1，
       若用它作循环控制变量，会使得DPB中的某些帧实际上并未遍历到，从而导致本该从DPB删除的帧依然留在DPB里.
       这样，DPB和FSP的信息是冲突的，可以引发很多问题.
       xx卡顿码流即由此引起，原因是fsp和dpb冲突造成get_apc()失败，进而清除了DPB，导致丢帧. */
    record_used_size = hevc_ctx->dpb.used_size;
    for (i = 0, j = 0; i < record_used_size; i++) {
        if (hevc_ctx->dpb.fs[j]->is_reference == 0) {
            hevc_remove_frame_store_out_dpb(hevc_ctx, j);
            is_removed = HEVC_TRUE;
        } else {
            j++;
        }
    }

    return is_removed;
}

static hi_s32 hevc_ref_pic_process(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;
    hi_s32 i_lost_poc;

    hevc_applay_reference_picture_set(hevc_ctx);
    ret = hevc_remove_un_used_frame_store(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("%s call hevc_remove_un_used_frame_store failed!\n", __func__);
        return HEVC_DEC_ERR;
    }

    ret = hevc_check_that_all_ref_pics_are_available(hevc_ctx, &i_lost_poc);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("%s call hevc_check_that_all_ref_pics_are_available failed!\n", __func__);
        return HEVC_DEC_ERR;
    }

    hevc_update_reflist(hevc_ctx);
    hevc_update_ltreflist(hevc_ctx);

    return HEVC_DEC_NORMAL;
}

static hi_void hevc_pic_type_statistic(hevc_ctx_info *hevc_ctx)
{
    if (hevc_is_idrunit(hevc_ctx->curr_slice.nal_unit_type) == 1) {
        hevc_ctx->curr_pic.pic_type = HEVC_IDR_FRAME;
    } else if (hevc_is_blaunit(hevc_ctx->curr_slice.nal_unit_type) == 1) {
        hevc_ctx->curr_pic.pic_type = HEVC_BLA_FRAME;
    } else if (hevc_is_craunit(hevc_ctx->curr_slice.nal_unit_type) == 1) {
        hevc_ctx->curr_pic.pic_type = HEVC_CRA_FRAME;
    } else {
        switch (hevc_ctx->curr_slice.slice_type) {
            case HEVC_B_SLICE:
                dprint("b_slice\n");
                hevc_ctx->curr_pic.pic_type = HEVC_B_FRAME;
                break;
            case HEVC_P_SLICE:
                dprint("p_slice\n");
                if (hevc_ctx->curr_pic.pic_type != HEVC_B_FRAME) {
                    hevc_ctx->curr_pic.pic_type = HEVC_P_FRAME;
                }
                break;
            case HEVC_I_SLICE:
                dprint("i_slice\n");
                if (hevc_ctx->curr_pic.pic_type != HEVC_B_FRAME
                        && hevc_ctx->curr_pic.pic_type != HEVC_P_FRAME) {
                    hevc_ctx->curr_pic.pic_type = HEVC_I_FRAME;
                }
                break;
            default:
                hevc_ctx->curr_pic.pic_type = HEVC_ERR_FRAME;
                break;
        }
    }

    return;
}

static hi_s32 hevc_alloc_frame_store(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i = 0;

    hevc_ctx->curr_pic.state = HEVC_PIC_EMPTY;

    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        if (hevc_ctx->frame_store[i].frame_store_state == FS_NOT_USED) {
            hevc_ctx->curr_pic.state                            = HEVC_PIC_DECODING;
            hevc_ctx->curr_pic.frame_store                      = &hevc_ctx->frame_store[i];
            hevc_ctx->curr_pic.frame_store->non_existing        = 0;
            hevc_ctx->curr_pic.frame_store->frame.is_long_term  = 0;
            hevc_ctx->curr_pic.frame_store->frame.is_short_term = 0;
            break;
        }
    }
    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_init_pic(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;
    hi_s32 prev_sps_id;
    hi_s32 pic_width, pic_height, log2_ctb_size_y;
    hevc_video_param_set *vps      = NULL;
    hevc_seq_param_set   *sps      = NULL;
    hevc_pic_param_set   *pps      = NULL;

    prev_sps_id = hevc_ctx->curr_sps.seq_parameter_set_id;
    pic_width = hevc_ctx->curr_sps.pic_width_in_luma_samples;
    pic_height = hevc_ctx->curr_sps.pic_height_in_luma_samples;
    log2_ctb_size_y = hevc_ctx->curr_sps.log2_ctb_size_y;

    if (hevc_ctx->curr_slice.pic_parameter_set_id < 0 ||
        hevc_ctx->curr_slice.pic_parameter_set_id >= (hevc_ctx->max_pps_num)) {
        dprint("curr_slice.pic_parameter_set_id = %d\n", hevc_ctx->curr_slice.pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];
    vps = &hevc_ctx->p_vps[sps->video_parameter_set_id];

    if ((hevc_ctx->curr_vps.video_parameter_set_id != vps->video_parameter_set_id)
            || (vps->is_refresh) || (!hevc_ctx->allow_start_dec)) {
        vps->is_refresh = 0;
        memmove(&hevc_ctx->curr_vps, vps, sizeof(hevc_video_param_set));
    }

    if ((hevc_ctx->curr_sps.seq_parameter_set_id != sps->seq_parameter_set_id)
            || (sps->is_refresh) || (!hevc_ctx->allow_start_dec)
            || pic_width != sps->pic_width_in_luma_samples || pic_height != sps->pic_height_in_luma_samples
            || log2_ctb_size_y != sps->log2_ctb_size_y) {
        sps->is_refresh = 0;
        memmove(&hevc_ctx->curr_sps, sps, sizeof(hevc_seq_param_set));
    }

    if ((hevc_ctx->curr_pps.pic_parameter_set_id != pps->pic_parameter_set_id)
            || (pps->is_refresh) || (!hevc_ctx->allow_start_dec)
            || pic_width != sps->pic_width_in_luma_samples || pic_height != sps->pic_height_in_luma_samples
            || log2_ctb_size_y != sps->log2_ctb_size_y) {
        pps->is_refresh = 0;
        memmove(&hevc_ctx->curr_pps, pps, sizeof(hevc_pic_param_set));
    }

    ret = hevc_alloc_frame_store(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("hevc_alloc_frame_store error\n");
        return HEVC_DEC_ERR;
    }

    hevc_ctx->curr_pic.state = HEVC_PIC_DECODING;
    hevc_ctx->curr_pic.thispoc = hevc_ctx->curr_slice.poc;
    hevc_ctx->curr_pic.frame_store->poc = hevc_ctx->curr_slice.poc;
    hevc_ctx->curr_pic.pic_type = HEVC_I_FRAME;
    hevc_ctx->total_slice_num = 0;

    return HEVC_DEC_NORMAL;
}

static hi_void hevc_init_dec_buffers(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i, j;

    pos();

    /* init DPB */
    hevc_ctx->dpb.size = HEVC_MAX_DPB_NUM;
    hevc_ctx->dpb.used_size                     = 0;
    hevc_ctx->dpb.negative_ref_frames_in_buffer = 0;
    hevc_ctx->dpb.positive_ref_frames_in_buffer = 0;
    hevc_ctx->dpb.ltref_frames_in_buffer        = 0;
    hevc_ctx->dpb.max_long_term_pic_idx         = 0;

    for (i = 0; i < HEVC_MAX_DPB_NUM; i++) {
        hevc_ctx->dpb.fs_ltref[i]        = NULL;
        hevc_ctx->dpb.fs_negative_ref[i] = NULL;
        hevc_ctx->dpb.fs_positive_ref[i] = NULL;
        hevc_ctx->dpb.fs[i]              = NULL;
    }

    /* init frame store */
    for (i = 0; i < HEVC_MAX_FRAME_STORE; i++) {
        hevc_ctx->frame_store[i].non_existing      = 0;
        hevc_ctx->frame_store[i].frame_store_state  = FS_NOT_USED;
        hevc_ctx->frame_store[i].is_reference      = 0;
        hevc_ctx->frame_store[i].poc               = 0;
        hevc_ctx->frame_store[i].frame.frame_store = &hevc_ctx->frame_store[i];
    }

    /* init list_x */
    for (i = 0; i < 0x2; i++) {
        for (j = 0; j < HEVC_MAX_LIST_SIZE; j++) {
            hevc_ctx->p_list_x[i][j] = NULL;
        }
        hevc_ctx->curr_slice.list_xsize[i] = 0;
    }

    return;
}

static hi_s32 compare_pic_by_poc_asc(hi_void *arg1, hi_void *arg2)
{
    if ((*(hevc_framestore **)arg1)->poc < (*(hevc_framestore **)arg2)->poc) {
        return -1;
    }

    if ((*(hevc_framestore **)arg1)->poc > (*(hevc_framestore **)arg2)->poc) {
        return 1;
    } else {
        return 0;
    }
}

#define STKSIZ (0x8 * sizeof(void*) - 0x2)

#define CUTOFF 8            /* testing shows that this is good value */

static hi_void swap_kn(
    char *a,
    char *b,
    size_t width)
{
    char tmp;

    if (a != b) {
        while (width--) {
            tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
    }
}

static hi_void shortsort(
    char *lo,
    char *hi,
    size_t width,
    int (*comp)(hi_void *, hi_void *))
{
    char *p, *max;

    while (hi > lo) {
        max = lo;
        for (p = lo + width; p <= hi; p += width) {
            if (comp(p, max) > 0) {
                max = p;
            }
        }
        swap_kn(max, hi, width);
        hi -= width;
    }
}

static hi_void hevc_qsort(
    hi_void *base,
    size_t   num,
    size_t   width,
    int (*comp)(hi_void *, hi_void *))
{
    char *lo, *hi;
    char *mid;
    char *loguy, *higuy;
    size_t size;
    char *lostk[STKSIZ], *histk[STKSIZ];
    int stkptr;

    if (num < 0x2 || width == 0) {
        return;
    }

    stkptr = 0;

    lo = base;
    hi = (char *)base + width * (num - 1);

recurse:

    size = (hi - lo) / width + 1;

    if (size <= CUTOFF) {
        shortsort(lo, hi, width, comp);
    } else {
        mid = lo + (size / 0x2) * width;

        if (comp(lo, mid) > 0) {
            swap_kn(lo, mid, width);
        }
        if (comp(lo, hi) > 0) {
            swap_kn(lo, hi, width);
        }
        if (comp(mid, hi) > 0) {
            swap_kn(mid, hi, width);
        }

        loguy = lo;
        higuy = hi;

        for (;;) {
            if (mid > loguy) {
                do  {
                    loguy += width;
                } while (loguy < mid && comp(loguy, mid) <= 0);
            }

            if (mid <= loguy) {
                do  {
                    loguy += width;
                } while (loguy <= hi && comp(loguy, mid) <= 0);
            }

            do  {
                higuy -= width;
            } while (higuy > mid && comp(higuy, mid) > 0);

            if (higuy < loguy) {
                break;
            }

            swap_kn(loguy, higuy, width);

            if (mid == higuy) {
                mid = loguy;
            }
        }

        higuy += width;
        if (mid < higuy) {
            do  {
                higuy -= width;
            } while (higuy > mid && comp(higuy, mid) == 0);
        }
        if (mid >= higuy) {
            do  {
                higuy -= width;
            } while (higuy > lo && comp(higuy, mid) == 0);
        }

        if (higuy - lo >= hi - loguy) {
            if (lo < higuy) {
                lostk[stkptr] = lo;
                histk[stkptr] = higuy;
                ++stkptr;
            }

            if (loguy < hi) {
                lo = loguy;
                goto recurse;
            }
        } else {
            if (loguy < hi) {
                lostk[stkptr] = loguy;
                histk[stkptr] = hi;
                ++stkptr;
            }

            if (lo < higuy) {
                hi = higuy;
                goto recurse;
            }
        }
    }

    --stkptr;
    if (stkptr >= 0) {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;
    } else {
        return;
    }
}

static hi_s32 hevc_insert_frm_in_dpb(hevc_ctx_info *hevc_ctx, hi_u32 pos, hevc_currpic *curr_pic)
{
    if (pos >= HEVC_MAX_DPB_NUM) {
        dprint("%s with invalid pos(%d)\n", __func__, pos);
        return HEVC_DEC_ERR;
    }

    hevc_ctx->dpb.fs[pos] = curr_pic->frame_store;
    hevc_ctx->dpb.fs[pos]->frame_store_state = FS_IN_DPB;
    curr_pic->is_ref_idc = 1;
    hevc_ctx->dpb.fs[pos]->is_reference = (curr_pic->is_ref_idc == 0) ? 0 : 1;
    hevc_ctx->dpb.fs[pos]->frame.frame_store = curr_pic->frame_store;
    hevc_ctx->dpb.fs[pos]->pic_type = hevc_ctx->dpb.fs[pos]->frame.pic_type = curr_pic->pic_type;
    hevc_ctx->dpb.fs[pos]->poc = curr_pic->thispoc;
    hevc_ctx->dpb.fs[pos]->frame.poc = curr_pic->thispoc;

    // dpb按照poc递增排序
    hevc_qsort((hi_void *)hevc_ctx->dpb.fs, (pos + 1), sizeof(hevc_framestore *), compare_pic_by_poc_asc);

    hevc_ctx->dpb.used_size++;

    curr_pic->state = HEVC_PIC_EMPTY;

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_order_process(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;

    ret = hevc_remove_un_used_frame_store(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("%s call hevc_remove_un_used_frame_store failed!\n", __func__);
        return HEVC_DEC_ERR;
    }

    ret = hevc_insert_frm_in_dpb(hevc_ctx, hevc_ctx->dpb.used_size, &hevc_ctx->curr_pic);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("%s call hevc_insert_frm_in_dpb failed.\n", __func__);
        return HEVC_DEC_ERR;
    }

    return ret;
}

static hi_s32 hevc_store_pic_in_dpb(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;

    ret = hevc_dec_order_process(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("%s call hevc_dec_order_process failed\n", __func__);
    }

    return ret;
}

static hi_s32 hevc_write_pic_msg(hevc_ctx_info *hevc_ctx)
{
    hi_s32 cnt;
    hevc_ctx->hevc_frm_poc = hevc_ctx->curr_pic.thispoc;
    hevc_ctx->hevc_frm_type = HEVC_ERR_FRAME;
    hevc_ctx->hevc_ref_num = 0;
    for (cnt = 0; cnt < 16; cnt++) { /* 16 loops */
        hevc_ctx->hevc_ref_poc[cnt] = 0;
    }
    dprint("\n\ng_hevc_frm_poc = %d\n", hevc_ctx->hevc_frm_poc);

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_check_list_x(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i;
    hi_u32 list0size, list1size;
    hevc_slice_segment_header  *slice;

    if (!hevc_ctx->curr_slice.dependent_slice_segment_flag) {
        slice = &hevc_ctx->curr_slice;
    } else {
        dprint("dependent_slice_segment_flag[%d] invalid\n", hevc_ctx->curr_slice.dependent_slice_segment_flag);
        return HEVC_DEC_ERR;
    }

    list0size = slice->list_xsize[0];
    list1size = slice->list_xsize[1];

    if (list0size >= HEVC_MAX_LIST_SIZE) {
        dprint("list_x ERROR: list0size %d >= %d\n", list0size, HEVC_MAX_LIST_SIZE);
        return HEVC_DEC_ERR;
    }

    if (list1size >= HEVC_MAX_LIST_SIZE) {
        dprint("list_x ERROR: list1size %d >= %d\n", list1size, HEVC_MAX_LIST_SIZE);
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < list0size; i++) {
        if (hevc_ctx->p_list_x[0][i] == NULL) {
            dprint("p_list_x[0][%d] = null\n", i);
            return HEVC_DEC_ERR;
        }

        if (hevc_ctx->p_list_x[0][i]->frame_store == NULL) {
            dprint("p_list_x[0][%d]->frame_store = null\n", i);
            return HEVC_DEC_ERR;
        }
    }

    for (i = 0; i < list1size; i++) {
        if (hevc_ctx->p_list_x[1][i] == NULL) {
            dprint("p_list_x[1][%d] = null\n", i);
            return HEVC_DEC_ERR;
        }

        if (hevc_ctx->p_list_x[1][i]->frame_store == NULL) {
            dprint("p_list_x[1][%d]->frame_store = null\n", i);
            return HEVC_DEC_ERR;
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_write_slice_msg(hevc_ctx_info *hevc_ctx)
{
    hi_s32 i, m;
    hi_s32 ref_pic_ok;

    if (hevc_check_list_x(hevc_ctx) != HEVC_DEC_NORMAL) {
        dprint("hevc_check_list_x failed\n");
        return HEVC_DEC_ERR;
    }

    hevc_ctx->total_slice_num++;

    if (hevc_ctx->curr_slice.slice_type == HEVC_I_SLICE) {
        dprint("hevc_i_slice\n");
    } else if (hevc_ctx->curr_slice.slice_type == HEVC_P_SLICE) {
        dprint("hevc_p_slice\n");
    } else if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
        dprint("hevc_b_slice\n");
    } else {
        dprint("hevc_non_slice\n");
    }

    dprint("list_xsize[0] = %d\n", hevc_ctx->curr_slice.list_xsize[0]);
    for (i = 0; i < hevc_ctx->curr_slice.list_xsize[0]; i++) {
        dprint("[%d]%d, ", i, hevc_ctx->p_list_x[0][i]->poc);

        ref_pic_ok = 0;
        for (m = 0; m < hevc_ctx->hevc_ref_num; m++) {
            if (hevc_ctx->hevc_ref_poc[m] == hevc_ctx->p_list_x[0][i]->poc) {
                ref_pic_ok = 1;
                break;
            }
        }
        if (ref_pic_ok == 0) {
            hevc_ctx->hevc_ref_poc[hevc_ctx->hevc_ref_num++] = hevc_ctx->p_list_x[0][i]->poc;
            if (hevc_ctx->hevc_ref_num >= 16) { /* hevc_ref_num less than 16 */
                HI_ERR_DEMUX("p_hevc_ctx->hevc_ref_num(%d) invalid!\n", hevc_ctx->hevc_ref_num);
                return HEVC_DEC_ERR;
            }
        }
    }
    if (hevc_ctx->curr_slice.list_xsize[0] > 0) {
        dprint("\n");
    }

    dprint("list_xsize[1] = %d\n", hevc_ctx->curr_slice.list_xsize[1]);
    for (i = 0; i < hevc_ctx->curr_slice.list_xsize[1]; i++) {
        dprint("[%d]%d, ", i, hevc_ctx->p_list_x[1][i]->poc);

        ref_pic_ok = 0;
        for (m = 0; m < hevc_ctx->hevc_ref_num; m++) {
            if (hevc_ctx->hevc_ref_poc[m] == hevc_ctx->p_list_x[1][i]->poc) {
                ref_pic_ok = 1;
                break;
            }
        }
        if (ref_pic_ok == 0) {
            hevc_ctx->hevc_ref_poc[hevc_ctx->hevc_ref_num++] = hevc_ctx->p_list_x[1][i]->poc;
            if (hevc_ctx->hevc_ref_num >= 16) { /* hevc_ref_num less than 16 */
                HI_ERR_DEMUX("p_hevc_ctx->hevc_ref_num(%d) invalid!\n", hevc_ctx->hevc_ref_num);
                return HEVC_DEC_ERR;
            }
        }
    }
    if (hevc_ctx->curr_slice.list_xsize[1] > 0) {
        dprint("\n");
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_init_list_x(hevc_ctx_info *hevc_ctx)
{
    hevc_storablepic *temp_list_x[HEVC_LIST_X_SIZE][HEVC_MAX_LIST_SIZE];
    hevc_ref_pic_lists_moddification *rplmodify;
    hi_u32 i, idx, ref_frame_num;
    hi_u32 neg_ref_frame_num;
    hi_u32 pos_ref_frame_num;
    hi_u32 ltref_frame_num;

    if (hevc_ctx == NULL) {
        return HEVC_DEC_ERR;
    }

    memset(temp_list_x, 0, sizeof(temp_list_x));

    idx = 0;
    if ((hevc_ctx->dpb.negative_ref_frames_in_buffer >= HEVC_MAX_DPB_NUM)
            || (hevc_ctx->dpb.positive_ref_frames_in_buffer >= HEVC_MAX_DPB_NUM)) {
        return HEVC_DEC_ERR;
    }

    for (i = 0; i < hevc_ctx->dpb.negative_ref_frames_in_buffer; i++) {
        if ((hevc_ctx->dpb.fs_negative_ref[i]->is_reference == 1) &&
                (hevc_ctx->dpb.fs_negative_ref[i]->frame.is_long_term == 0) &&
                (hevc_ctx->dpb.fs_negative_ref[i]->frame.is_short_term == 1)) {
            temp_list_x[0][idx++] = &hevc_ctx->dpb.fs_negative_ref[i]->frame;
        }
    }
    neg_ref_frame_num = idx;

    for (i = 0; i < hevc_ctx->dpb.positive_ref_frames_in_buffer; i++) {
        if ((hevc_ctx->dpb.fs_positive_ref[i]->is_reference == 1) &&
                (hevc_ctx->dpb.fs_positive_ref[i]->frame.is_long_term == 0) &&
                (hevc_ctx->dpb.fs_positive_ref[i]->frame.is_short_term == 1)) {
            temp_list_x[0][idx++] = &hevc_ctx->dpb.fs_positive_ref[i]->frame;
        }
    }

    if (idx >= HEVC_MAX_LIST_SIZE) {
        return HEVC_DEC_ERR;
    }

    pos_ref_frame_num = idx - neg_ref_frame_num;

    // long term handling
    for (i = 0; i < hevc_ctx->dpb.ltref_frames_in_buffer; i++) {
        if ((hevc_ctx->dpb.fs_ltref[i]->is_reference == 1) &&
                (hevc_ctx->dpb.fs_ltref[i]->frame.is_long_term == 1) &&
                (hevc_ctx->dpb.fs_ltref[i]->frame.is_short_term == 0)) {
            temp_list_x[0][idx++] = &hevc_ctx->dpb.fs_ltref[i]->frame;
        }
    }
    ltref_frame_num = idx - neg_ref_frame_num - pos_ref_frame_num;
    ref_frame_num = idx;

    if (ref_frame_num == 0) {
        hevc_ctx->curr_slice.list_xsize[0] = 0;
        hevc_ctx->curr_slice.list_xsize[1] = 0;
        memset(hevc_ctx->p_list_x, 0, sizeof(hevc_ctx->p_list_x));

        return HEVC_DEC_NORMAL;
    }

    if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
        idx = 0;
        for (i = 0; i < pos_ref_frame_num; i++) {
            temp_list_x[1][idx++] = temp_list_x[0][neg_ref_frame_num + i];
        }
        for (i = 0; i < neg_ref_frame_num; i++) {
            temp_list_x[1][idx++] = temp_list_x[0][i];
        }
        for (i = 0; i < ltref_frame_num; i++) {
            temp_list_x[1][idx++] = temp_list_x[0][neg_ref_frame_num + pos_ref_frame_num + i];
        }
    }

    // set max size
    hevc_ctx->curr_slice.list_xsize[0] =  hevc_ctx->curr_slice.num_ref_idx[0];
    hevc_ctx->curr_slice.list_xsize[1] =  hevc_ctx->curr_slice.num_ref_idx[1];

    idx = 0;
    rplmodify = &(hevc_ctx->curr_slice.ref_pic_lists_modification);
    for (i = 0; i < hevc_ctx->curr_slice.list_xsize[0]; i++) {
        idx = rplmodify->ref_pic_list_modification_flag_l0 ? rplmodify->list_entry_l0[i] : i % ref_frame_num;
        hevc_ctx->p_list_x[0][i] = temp_list_x[0][idx];
    }

    if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
        for (i = 0; i < hevc_ctx->curr_slice.list_xsize[1]; i++) {
            idx = rplmodify->ref_pic_list_modification_flag_l1 ? rplmodify->list_entry_l1[i] : i % ref_frame_num;
            hevc_ctx->p_list_x[1][i] = temp_list_x[1][idx];
        }

        /* for generalized b slice */
        if (hevc_ctx->curr_slice.list_xsize[1] == 0) {
            memcpy(hevc_ctx->p_list_x[1], hevc_ctx->p_list_x[0], sizeof(hevc_ctx->p_list_x[0]));
            hevc_ctx->curr_slice.list_xsize[1] = hevc_ctx->curr_slice.list_xsize[0];
        }
    }

    // set the unused list entries to a valid one
    for (i = hevc_ctx->curr_slice.list_xsize[0]; i < (HEVC_MAX_LIST_SIZE); i++) {
        hevc_ctx->p_list_x[0][i] = hevc_ctx->p_list_x[0][0];
    }
    for (i = hevc_ctx->curr_slice.list_xsize[1]; i < (HEVC_MAX_LIST_SIZE); i++) {
        hevc_ctx->p_list_x[1][i] = hevc_ctx->p_list_x[1][0];
    }

    // initlist_x over
    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_repair_list(hevc_ctx_info *hevc_ctx)
{
    hi_s8  found = -1;
    hi_u32 i, i_list, i_list_cnt;
    hevc_storablepic *repair_pic = (hevc_storablepic *)NULL;

    switch (hevc_ctx->curr_slice.slice_type) {
        case HEVC_P_SLICE:
            i_list_cnt = 1;
            break;
        case HEVC_B_SLICE:
            i_list_cnt = 0x2;
            break;
        case HEVC_I_SLICE:
            i_list_cnt = 0;
            break;
        default:
            dprint("%s unkown slice type(%d).\n", __func__, hevc_ctx->curr_slice.slice_type);
            return HEVC_DEC_ERR;
    }

    for (i_list = 0; i_list < i_list_cnt; i_list++) {
        if (0 < hevc_ctx->curr_slice.list_xsize[i_list]) {
            found = -1;
            for (i = 0; i < hevc_ctx->curr_slice.list_xsize[i_list]; i++) {
                if (hevc_ctx->p_list_x[i_list][i] != (hevc_storablepic *)NULL) {
                    found = i;
                    repair_pic = hevc_ctx->p_list_x[i_list][i];
                    break;
                }
            }

            for (i = 0; i < hevc_ctx->curr_slice.list_xsize[i_list]; i++) {
                if (hevc_ctx->p_list_x[i_list][i] == (hevc_storablepic *)NULL) {
                    dprint("%s: list_x[%d][%d] = NULL, repair it with pos(%d).\n", __func__, i_list, i, found);
                    hevc_ctx->p_list_x[i_list][i] = repair_pic;
                }
            }
        } else {
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_list(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;

    if (hevc_ctx->curr_slice.slice_type == HEVC_I_SLICE) {
        hevc_ctx->curr_slice.list_xsize[0] = 0;
        hevc_ctx->curr_slice.list_xsize[1] = 0;
        return HEVC_DEC_NORMAL;
    }

    ret = hevc_init_list_x(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("hevc_init_list_x error.\n");
        return HEVC_DEC_ERR;
    }

    ret = hevc_repair_list(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("hevc_repair_list error.\n");
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_is_reference_nalu(hevc_ctx_info *hevc_ctx)
{
    return ((hevc_ctx->curr_slice.nal_unit_type <= NAL_UNIT_RESERVED_VCL_R15) &&
            (hevc_ctx->curr_slice.nal_unit_type % 0x2 != 0))
           || ((hevc_ctx->curr_slice.nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA_W_LP) &&
               (hevc_ctx->curr_slice.nal_unit_type <= NAL_UNIT_RESERVED_IRAP_VCL23));
}

static hi_s32 hevc_dec_pred_weight_table(hevc_ctx_info *hevc_ctx)
{
    hi_u32 i, j;
    hevc_seq_param_set    *sps = NULL;
    hevc_pic_param_set    *pps = NULL;
    hevc_pred_weight_table  *pwt = NULL;
    hevc_pred_weight_table  pwt_info;

    pwt = &pwt_info;
    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];

    if (((hevc_ctx->curr_slice.slice_type == HEVC_P_SLICE) && (pps->weighted_pred_flag)) ||
            ((hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) && (pps->weighted_bipred_flag))) {
        if (hevc_ctx->curr_slice.num_ref_idx[0] >= HEVC_MAX_NUM_REF_PICS) {
            dprint("p_hevc_ctx->curr_slice.num_ref_idx[0](%d) invalid.\n", hevc_ctx->curr_slice.num_ref_idx[0]);
            return HEVC_DEC_ERR;
        }

        hevc_ue_v(hevc_ctx->p_bs, "luma_log2_weight_denom");

        if (sps->chroma_format_idc != 0) {
            hevc_se_v(hevc_ctx->p_bs, "delta_chroma_log2_weight_denom");
        }

        for (i = 0; i < hevc_ctx->curr_slice.num_ref_idx[0]; i++) {
            pwt->luma_weight_l0_flag[i] = hevc_u_v(hevc_ctx->p_bs, 1, "luma_weight_l0_flag");
        }
        if (sps->chroma_format_idc != 0) {
            for (i = 0; i < hevc_ctx->curr_slice.num_ref_idx[0]; i++) {
                pwt->chroma_weight_l0_flag[i] = hevc_u_v(hevc_ctx->p_bs, 1, "chroma_weight_l0_flag");
            }
        }

        for (i = 0; i < hevc_ctx->curr_slice.num_ref_idx[0]; i++) {
            if (pwt->luma_weight_l0_flag[i]) {
                hevc_se_v(hevc_ctx->p_bs, "delta_luma_weight_l0");
                hevc_se_v(hevc_ctx->p_bs, "luma_offset_l0");
            }
            if (sps->chroma_format_idc != 0) {
                if (pwt->chroma_weight_l0_flag[i]) {
                    for (j = 0; j < 0x2; j++) {
                        hevc_se_v(hevc_ctx->p_bs, "delta_chroma_weight_l0");
                        hevc_se_v(hevc_ctx->p_bs, "delta_chroma_offset_l0");
                    }
                }
            }
        }

        if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
            if (hevc_ctx->curr_slice.num_ref_idx[1] >= HEVC_MAX_NUM_REF_PICS) {
                dprint("p_hevc_ctx->curr_slice.num_ref_idx[1](%d) invalid.\n", hevc_ctx->curr_slice.num_ref_idx[0]);
                return HEVC_DEC_ERR;
            }

            for (i = 0; i < hevc_ctx->curr_slice.num_ref_idx[1]; i++) {
                pwt->luma_weight_l1_flag[i] = hevc_u_v(hevc_ctx->p_bs, 1, "luma_weight_l1_flag");
            }
            if (sps->chroma_format_idc != 0) {
                for (i = 0; i < hevc_ctx->curr_slice.num_ref_idx[1]; i++) {
                    pwt->chroma_weight_l1_flag[i] = hevc_u_v(hevc_ctx->p_bs, 1, "chroma_weight_l1_flag");
                }
            }
            for (i = 0; i < hevc_ctx->curr_slice.num_ref_idx[1]; i++) {
                if (pwt->luma_weight_l1_flag[i]) {
                    hevc_se_v(hevc_ctx->p_bs, "delta_luma_weight_l1");
                    hevc_se_v(hevc_ctx->p_bs, "luma_offset_l1");
                }
                if (sps->chroma_format_idc != 0) {
                    if (pwt->chroma_weight_l1_flag[i]) {
                        for (j = 0; j < 0x2; j++) {
                            hevc_se_v(hevc_ctx->p_bs, "delta_chroma_weight_l1");
                            hevc_se_v(hevc_ctx->p_bs, "delta_chroma_offset_l1");
                        }
                    }
                }
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_vps_sps_pps_check(hevc_ctx_info *hevc_ctx)
{
    hevc_video_param_set *vps = NULL;
    hevc_seq_param_set   *sps = NULL;
    hevc_pic_param_set   *pps = NULL;

    /* check pps is get or not */
    if (hevc_ctx->curr_slice.pic_parameter_set_id < 0 ||
        hevc_ctx->curr_slice.pic_parameter_set_id >= (hevc_ctx->max_pps_num)) {
        dprint("hevc_vps_sps_pps_check pic_parameter_set_id(%d) out of range\n",
               hevc_ctx->curr_slice.pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    if (!pps->valid) {
        dprint("pps with this pic_parameter_set_id = %d havn't be decoded\n",
               hevc_ctx->curr_slice.pic_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    /* check sps is get or not */
    if (pps->seq_parameter_set_id < 0 || pps->seq_parameter_set_id >= (hevc_ctx->max_sps_num)) {
        dprint("hevc_vps_sps_pps_check seq_parameter_set_id(%d) out of range\n", pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];
    if (!sps->valid) {
        dprint("sps with this seq_parameter_set_id = %d havn't be decoded\n", pps->seq_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    /* check vps is get or not */
    if (sps->video_parameter_set_id >= (hevc_ctx->max_vps_num)) {
        dprint("hevc_vps_sps_pps_check video_parameter_set_id out of range\n");
        return HEVC_DEC_ERR;
    }

    vps = &hevc_ctx->p_vps[sps->video_parameter_set_id];
    if (!vps->valid) {
        dprint("vps with this video_parameter_set_id = %d havn't be decoded\n", sps->video_parameter_set_id);
        return HEVC_DEC_ERR;
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_is_new_pic(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret = 0;

    if (hevc_ctx->poc_random_access == HEVC_MAX_INT) {
        if (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_CRA        ||
                hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP   ||
                hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP   ||
                hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL ||
                (((hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_N)
                  || (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_TRAIL_R))
                 && (hevc_ctx->curr_slice.slice_type == HEVC_I_SLICE))) {
            hevc_ctx->poc_random_access = hevc_ctx->curr_slice.poc;
        } else if ((hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL) ||
                   (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP)) {
            hevc_ctx->poc_random_access = -HEVC_MAX_INT;
        } else {
            return IS_SKIP_PIC;
        }
    } else if ((hevc_ctx->curr_slice.poc < hevc_ctx->poc_random_access) &&
               (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R ||
                hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N)) {
    } else if ((hevc_ctx->b_no_rasl_output_flag) &&
               (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R ||
                hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N)) {
    }

    /* check if picture should be skipped because of association with a previous BLA picture */
    if (hevc_ctx->prev_rapis_bla && hevc_ctx->curr_slice.poc < hevc_ctx->poc_cra &&
            (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_R ||
             hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_RASL_N)) {
    }

    if (!hevc_ctx->curr_slice.dependent_slice_segment_flag) {
        if (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL
                || hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP) {
            hevc_ctx->poc_cra = hevc_ctx->curr_slice.poc;
            hevc_ctx->prev_rapis_bla = HEVC_FALSE;
        } else if (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) {
            hevc_ctx->poc_cra = hevc_ctx->curr_slice.poc;
            hevc_ctx->prev_rapis_bla = HEVC_FALSE;
        } else if (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP
                   || hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL
                   || hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP) {
            hevc_ctx->poc_cra = hevc_ctx->curr_slice.poc;
            hevc_ctx->prev_rapis_bla = HEVC_TRUE;
        }
    }

    ret |= hevc_ctx->curr_slice.first_slice_segment_in_pic_flag;

    return ret;
}

static hi_s32 hevc_get_rap_pic_flag(hevc_ctx_info *hevc_ctx)
{
    return hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL
           || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP
           || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP
           || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL
           || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP
           || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA;
}

static hi_s32 hevc_get_idr_pic_flag(hevc_ctx_info *hevc_ctx)
{
    return hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_W_RADL
           || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP;
}

static hi_s32 hevc_dec_ref_pic_lists_modification(hevc_ctx_info *hevc_ctx, hevc_slice_segment_header *slice,
    hevc_ref_pic_lists_moddification *rp_list_modification, hi_s32 num_rps_curr_temp_list)
{
    hi_u32 i, length, temp;

    temp = num_rps_curr_temp_list;
    length = 1;
    num_rps_curr_temp_list--;
    while (num_rps_curr_temp_list >>= 1) {
        length++;
    }

    rp_list_modification->ref_pic_list_modification_flag_l0 = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                       "ref_pic_list_modification_flag_l0");
    if (rp_list_modification->ref_pic_list_modification_flag_l0 && (temp > 1)) {
        if (slice->num_ref_idx[0] >= HEVC_MAX_NUM_REF_PICS) {
            dprint("p_slice->num_ref_idx[0](%d) invalid.\n", slice->num_ref_idx[0]);
            return HEVC_DEC_ERR;
        }

        for (i = 0; i < slice->num_ref_idx[0]; i++) {
            rp_list_modification->list_entry_l0[i] = hevc_u_v(hevc_ctx->p_bs, length, "list_entry_l0");
        }
    }

    if (slice->slice_type == HEVC_B_SLICE) {
        rp_list_modification->ref_pic_list_modification_flag_l1 = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                           "ref_pic_list_modification_flag_l1");
        if (rp_list_modification->ref_pic_list_modification_flag_l1 && (temp > 1)) {
            if (slice->num_ref_idx[1] >= HEVC_MAX_NUM_REF_PICS) {
                dprint("p_slice->num_ref_idx[0](%d) invalid.\n", slice->num_ref_idx[1]);
                return HEVC_DEC_ERR;
            }

            for (i = 0; i < slice->num_ref_idx[1]; i++) {
                rp_list_modification->list_entry_l1[i] = hevc_u_v(hevc_ctx->p_bs, length, "list_entry_l1");
            }
        }
    }

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_slice_segment_header(hevc_ctx_info *hevc_ctx, hi_u32 is_dec_slice)
{
    hi_s32 rappic_flag, idrpic_flag, num_cus, max_parts;
    hi_u32 req_bits_outer = 0;
    hi_u32 req_bits_inner = 0;
    hi_u32 lcu_address = 0;
    hi_u32 inner_address = 0;
    hi_u32 slice_address = 0;
    hi_u32 bits_for_long_term_pic_in_sps = 0;
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_u32 is_saoenabled = 0;
    hi_u32 is_dbfenabled = 0;
    hi_s32 num_rps_curr_temp_list = 0;
    hi_s32 offset = 0;
    hi_s32 poc_lsb_lt = 0;
    hi_s32 lt_idx_sps = 0;
    hi_s32 prev_delta_msb = 0;
    hi_s32 delta_poc_msbcycle_lt = 0;
    hi_u32 max_poc_lsb = 0;
    hi_u32 poc_lsb = 0;
    hi_u32 poc_msb = 0;
    hi_u32 pre_poc_lsb = 0;
    hi_u32 pre_poc_msb = 0;
    hi_s32 poc_ltcurr = 0;
    hi_u32 max_collocated_ref_idx = 0;
    hi_u32 max_num_entry_point_offsets = 0;
    hi_u32 num_bits = 0;
    hi_u32 ue = 0;
    hi_s32 ret = 0;
    hi_s32 bits_left = 0;

    hevc_video_param_set  *vps = NULL;
    hevc_seq_param_set    *sps = NULL;
    hevc_pic_param_set    *pps = NULL;
    hevc_short_term_rpset *rps = NULL;

    pos();

    hevc_ctx->curr_slice.slice_type = HEVC_ERR_SLICE;

    hevc_ctx->curr_slice.first_slice_segment_in_pic_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                    "first_slice_segment_in_pic_flag");

    rappic_flag = hevc_get_rap_pic_flag(hevc_ctx);
    idrpic_flag = hevc_get_idr_pic_flag(hevc_ctx);

    if (rappic_flag) {
        hevc_ctx->curr_slice.no_output_of_prior_pics_flag = hevc_u_v(hevc_ctx->p_bs, 1, "no_output_of_prior_pics_flag");
    }

    hevc_ctx->curr_slice.pic_parameter_set_id = hevc_ue_v(hevc_ctx->p_bs, "slice_pic_parameter_set_id");
    if (hevc_ctx->curr_slice.pic_parameter_set_id < 0 ||
        hevc_ctx->curr_slice.pic_parameter_set_id >= (hevc_ctx->max_pps_num)) {
        dprint("%s: pic_parameter_set_id(%d) out of range(0 %d)!\n", __func__,
               hevc_ctx->curr_slice.pic_parameter_set_id, hevc_ctx->max_pps_num);
        return HEVC_DEC_ERR;
    }

    if (hevc_vps_sps_pps_check(hevc_ctx) != HEVC_DEC_NORMAL) {
        if (hevc_ctx->total_slice_num > 0) {
            hevc_ctx->curr_slice.poc = hevc_ctx->last_slice_poc;
        } else {
            hevc_ctx->curr_slice.poc = 0;
        }

        hevc_ctx->curr_slice.new_pic_type = hevc_is_new_pic(hevc_ctx);
        dprint("hevc_vps_sps_pps_check != hevc_dec_normal\n");
        return HEVC_DEC_ERR;
    }

    pps = &hevc_ctx->p_pps[hevc_ctx->curr_slice.pic_parameter_set_id];
    sps = &hevc_ctx->p_sps[pps->seq_parameter_set_id];
    vps = &hevc_ctx->p_vps[sps->video_parameter_set_id];

    if ((pps->dependent_slice_segments_enabled_flag) && (!hevc_ctx->curr_slice.first_slice_segment_in_pic_flag)) {
        hevc_ctx->curr_slice.dependent_slice_segment_flag = hevc_u_v(hevc_ctx->p_bs, 1, "dependent_slice_segment_flag");
    } else {
        hevc_ctx->curr_slice.dependent_slice_segment_flag = HEVC_FALSE;
    }

    if (sps->max_cu_width == 0 || sps->max_cu_height == 0) {
        dprint("max_cu_width(%d)/max_cu_height(%d) = 0\n", sps->max_cu_width, sps->max_cu_height);
        return HEVC_DEC_ERR;
    }

    num_cus = ((sps->pic_width_in_luma_samples + sps->max_cu_width - 1) / sps->max_cu_width) *
               ((sps->pic_height_in_luma_samples + sps->max_cu_height - 1) / sps->max_cu_height);
    max_parts = (1 << (sps->max_cu_depth << 1));

    while (num_cus > (1 << req_bits_outer)) {
        req_bits_outer++;
    }

#if 0
    while (num_parts > (1 << req_bits_inner)) {
        req_bits_inner++;
    }
#endif

    if (!hevc_ctx->curr_slice.first_slice_segment_in_pic_flag) {
        hevc_ctx->curr_slice.slice_segment_address = hevc_u_v(hevc_ctx->p_bs, (hi_s32)(req_bits_inner + req_bits_outer),
                                                              "slice_segment_address");
        if (hevc_ctx->curr_slice.slice_segment_address < 0 ||
                hevc_ctx->curr_slice.slice_segment_address >= (sps->ctb_num_width) * (sps->ctb_num_height)) {
            dprint("slice_segment_address(%d) out of range(0,%d).\n", hevc_ctx->curr_slice.slice_segment_address,
                   (sps->ctb_num_width) * (sps->ctb_num_height));
            return HEVC_DEC_ERR;
        }
        lcu_address   = hevc_ctx->curr_slice.slice_segment_address >> req_bits_inner;
        inner_address = hevc_ctx->curr_slice.slice_segment_address - (lcu_address << req_bits_inner);
    }
    slice_address = max_parts * lcu_address + inner_address;
    hevc_ctx->curr_slice.dependent_slice_curstart_cuaddr = slice_address;
    hevc_ctx->curr_slice.dependent_slice_curend_cuaddr   = num_cus * max_parts;
    hevc_ctx->curr_slice.poc = hevc_ctx->last_slice_poc;

    hevc_ctx->curr_slice.slice_type = HEVC_I_SLICE;
    if (!hevc_ctx->curr_slice.dependent_slice_segment_flag) {
        if ((hevc_ctx->p_bs->total_pos + pps->num_extra_slice_header_bits) > (hevc_ctx->p_bs->bs_len * 0x8)) {
            dprint("%s, %d, num_extra_slice_header_bits out of range, \
                total_pos = %d, num_extra_slice_header_bits = %d, bs_len = %d\n",
                   __func__, __LINE__, hevc_ctx->p_bs->total_pos,
                   pps->num_extra_slice_header_bits, hevc_ctx->p_bs->bs_len * 0x8);

            return HEVC_DEC_ERR;
        }

        for (i = 0; i < pps->num_extra_slice_header_bits; i++) {
            ue = hevc_u_v(hevc_ctx->p_bs, 1, "slice_reserved_flag");
        }

        hevc_ctx->curr_slice.slice_type = hevc_ue_v(hevc_ctx->p_bs, "slice_type");
        if (hevc_ctx->curr_slice.slice_type < 0 || hevc_ctx->curr_slice.slice_type > 0x2) {
            dprint("slice_type(%d) out of range(0,2].\n", hevc_ctx->curr_slice.slice_type);
            return HEVC_DEC_ERR;
        }

        if (pps->output_flag_present_flag) {
            hevc_ctx->curr_slice.pic_output_flag = hevc_u_v(hevc_ctx->p_bs, 1, "pic_output_flag");
        } else {
            hevc_ctx->curr_slice.pic_output_flag = HEVC_TRUE;
        }

        if (!idrpic_flag) {
            hevc_ctx->curr_slice.pic_order_cnt_lsb = hevc_u_v(hevc_ctx->p_bs, sps->bits_for_poc, "pic_order_cnt_lsb");

            /* calc poc of current slice */
            poc_lsb = hevc_ctx->curr_slice.pic_order_cnt_lsb;
            max_poc_lsb = 1 << sps->max_pic_order_cnt_lsb;
            pre_poc_lsb = hevc_ctx->prev_slice_poc % max_poc_lsb;
            pre_poc_msb = hevc_ctx->prev_slice_poc - pre_poc_lsb;

            if ((poc_lsb < pre_poc_lsb) && ((pre_poc_lsb - poc_lsb) >= (max_poc_lsb / 0x2))) {
                poc_msb = pre_poc_msb + max_poc_lsb;
            } else if ((poc_lsb > pre_poc_lsb) && ((poc_lsb - pre_poc_lsb) > (max_poc_lsb / 0x2))) {
                poc_msb = pre_poc_msb - max_poc_lsb;
            } else {
                poc_msb = pre_poc_msb;
            }

            if (hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_LP
                    || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_W_RADL
                    || hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP) {
                poc_msb = 0;
            }
            hevc_ctx->curr_slice.poc = poc_lsb + poc_msb;
            hevc_ctx->last_slice_poc = hevc_ctx->curr_slice.poc;

            if ((hevc_ctx->curr_slice.nuh_temporal_id == 0) && (hevc_is_reference_nalu(hevc_ctx) &&
                (hevc_ctx->curr_slice.nal_unit_type != NAL_UNIT_CODED_SLICE_RASL_R) &&
                (hevc_ctx->curr_slice.nal_unit_type != NAL_UNIT_CODED_SLICE_RADL_R))) {
                hevc_ctx->curr_slice.prev_poc = hevc_ctx->curr_slice.poc;
                hevc_ctx->prev_slice_poc = hevc_ctx->curr_slice.poc;
            }

            hevc_ctx->curr_slice.short_term_ref_pic_set_sps_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                                                                            "short_term_ref_pic_set_sps_flag");

            if (!hevc_ctx->curr_slice.short_term_ref_pic_set_sps_flag) {
                ret = hevc_dec_short_term_ref_pic_set(hevc_ctx, sps, &(hevc_ctx->curr_slice.short_term_ref_pic_set),
                                                      sps->num_short_term_ref_pic_sets);
                if (ret != HEVC_DEC_NORMAL) {
                    dprint("SH hevc_dec_short_term_ref_pic_set error.\n");
                    return HEVC_DEC_ERR;
                }
            } else {
                while ((1u << num_bits) < sps->num_short_term_ref_pic_sets) {
                    num_bits++;
                }
                if (num_bits > 0) {
                    hevc_ctx->curr_slice.short_term_ref_pic_set_idx = hevc_u_v(hevc_ctx->p_bs, num_bits,
                                                                               "short_term_ref_pic_set_idx");
                } else {
                    hevc_ctx->curr_slice.short_term_ref_pic_set_idx = 0;
                }
                if (hevc_ctx->curr_slice.short_term_ref_pic_set_idx < 0 ||
                        hevc_ctx->curr_slice.short_term_ref_pic_set_idx > sps->num_short_term_ref_pic_sets - 1) {
                    dprint("short_term_ref_pic_set_idx(%d) out of range[0,%d].\n",
                           hevc_ctx->curr_slice.short_term_ref_pic_set_idx,
                           sps->num_short_term_ref_pic_sets - 1);
                    return HEVC_DEC_ERR;
                }
                hevc_ctx->curr_slice.short_term_ref_pic_set =
                    sps->short_term_ref_pic_set[hevc_ctx->curr_slice.short_term_ref_pic_set_idx];
            }
            rps = &(hevc_ctx->curr_slice.short_term_ref_pic_set);

            if (sps->long_term_ref_pics_present_flag) {
                offset = rps->num_negative_pics + rps->num_positive_pics;
                if (sps->num_long_term_ref_pic_sps > 0) {
                    hevc_ctx->curr_slice.num_long_term_sps = hevc_ue_v(hevc_ctx->p_bs, "num_long_term_sps");
                    if (hevc_ctx->curr_slice.num_long_term_sps > sps->num_long_term_ref_pic_sps) {
                        dprint("num_long_term_sps(%d) out of range[0,%d].\n", hevc_ctx->curr_slice.num_long_term_sps,
                               sps->num_long_term_ref_pic_sps);
                        return HEVC_DEC_ERR;
                    }
                }
                bits_for_long_term_pic_in_sps = 0;
                while (sps->num_long_term_ref_pic_sps > ((hi_u32)(1 << bits_for_long_term_pic_in_sps))) {
                    bits_for_long_term_pic_in_sps++;
                }
                hevc_ctx->curr_slice.num_long_term_pics = hevc_ue_v(hevc_ctx->p_bs, "num_long_term_pics");
                rps->num_of_longterm_pics = hevc_ctx->curr_slice.num_long_term_sps +
                    hevc_ctx->curr_slice.num_long_term_pics;

                if ((rps->num_negative_pics + rps->num_positive_pics + rps->num_of_longterm_pics) >
                    HEVC_MAX_NUM_REF_PICS) {
                    dprint("SH ERROR:(num_negative_pics(%d) + num_positive_pics(%d) + num_of_longterm_pics(%d)) > %d\n",
                           rps->num_negative_pics, rps->num_positive_pics, rps->num_of_longterm_pics,
                           HEVC_MAX_NUM_REF_PICS);
                    return HEVC_DEC_ERR;
                }

                for (i = 0, j = offset + rps->num_of_longterm_pics - 1; i < rps->num_of_longterm_pics; j--, i++) {
                    if (i < hevc_ctx->curr_slice.num_long_term_sps) {
                        lt_idx_sps = 0;
                        if (bits_for_long_term_pic_in_sps) {
                            hevc_ctx->curr_slice.lt_idx_sps[i] = hevc_u_v(hevc_ctx->p_bs, bits_for_long_term_pic_in_sps,
                                                                          "lt_idx_sps");
                            lt_idx_sps = hevc_ctx->curr_slice.lt_idx_sps[i];
                            if (lt_idx_sps < 0 || lt_idx_sps > sps->num_long_term_ref_pic_sps - 1) {
                                dprint("lt_idx_sps[%d] = %d, but out of range[0,%d].\n", i, lt_idx_sps,
                                       sps->num_long_term_ref_pic_sps - 1);
                                return HEVC_DEC_ERR;
                            }
                        }

                        poc_lsb_lt = sps->lt_ref_pic_poc_lsb_sps[lt_idx_sps];
                        rps->used_flag[j] = sps->used_by_curr_pic_lt_sps_flag[lt_idx_sps];
                    } else {
                        hevc_ctx->curr_slice.poc_lsb_lt[i] = hevc_u_v(hevc_ctx->p_bs, sps->bits_for_poc, "poc_lsb_lt");
                        poc_lsb_lt = hevc_ctx->curr_slice.poc_lsb_lt[i];
                        hevc_ctx->curr_slice.used_by_curr_pic_lt_flag[i] =
                            hevc_u_v(hevc_ctx->p_bs, 1, "used_by_curr_pic_lt_flag");
                        rps->used_flag[j] = hevc_ctx->curr_slice.used_by_curr_pic_lt_flag[i];
                    }

                    hevc_ctx->curr_slice.poc_lsb_lt[i] = poc_lsb_lt;
                    hevc_ctx->curr_slice.delta_poc_msb_present_flag[i] =
                        hevc_u_v(hevc_ctx->p_bs, 1, "delta_poc_msb_present_flag");
                    if (hevc_ctx->curr_slice.delta_poc_msb_present_flag[i]) {
                        hevc_ctx->curr_slice.delta_poc_msb_cycle_lt[i] =
                            hevc_ue_v(hevc_ctx->p_bs, "delta_poc_msb_cycle_lt");
                        if (i == 0 || i == hevc_ctx->curr_slice.num_long_term_sps) {
                            delta_poc_msbcycle_lt = hevc_ctx->curr_slice.delta_poc_msb_cycle_lt[i];
                        } else {
                            delta_poc_msbcycle_lt = hevc_ctx->curr_slice.delta_poc_msb_cycle_lt[i] + prev_delta_msb;
                        }
                        hevc_ctx->curr_slice.delta_poc_msbcycle_lt[j] = delta_poc_msbcycle_lt;

                        hevc_ctx->curr_slice.check_ltmsb[j] = 1;
                    } else {
                        delta_poc_msbcycle_lt = (i == 0 ||
                            i == hevc_ctx->curr_slice.num_long_term_sps) ? 0 : prev_delta_msb;
                        hevc_ctx->curr_slice.check_ltmsb[j] = 0;
                    }
                    prev_delta_msb = delta_poc_msbcycle_lt;
                }

                for (i = 0, j = offset + rps->num_of_longterm_pics - 1; i < rps->num_of_longterm_pics; j--, i++) {
                    if (hevc_ctx->curr_slice.delta_poc_msb_present_flag[i]) {
                        poc_ltcurr = hevc_ctx->curr_slice.poc -
                            hevc_ctx->curr_slice.delta_poc_msbcycle_lt[j] * max_poc_lsb -
                            poc_lsb + hevc_ctx->curr_slice.poc_lsb_lt[i];
                        rps->poc[j] = poc_ltcurr;
                        rps->delta_poc[j] = poc_ltcurr - hevc_ctx->curr_slice.poc;
                    } else {
                        rps->poc[j] = hevc_ctx->curr_slice.poc_lsb_lt[i];
                        rps->delta_poc[j] = hevc_ctx->curr_slice.poc_lsb_lt[i] - hevc_ctx->curr_slice.poc;
                    }
                }

                rps->num_of_pics = offset + rps->num_of_longterm_pics;
                if (rps->num_of_pics > HEVC_MAX_NUM_REF_PICS) {
                    dprint("ERROR: rps->num_of_pics(%d) > %d.\n", rps->num_of_pics, HEVC_MAX_NUM_REF_PICS);
                    return HEVC_DEC_ERR;
                }
            }
            if (sps->sps_temporal_mvp_enable_flag) {
                hevc_ctx->curr_slice.slice_temporal_mvp_enable_flag =
                    hevc_u_v(hevc_ctx->p_bs, 1, "slice_temporal_mvp_enable_flag");
            } else {
                hevc_ctx->curr_slice.slice_temporal_mvp_enable_flag = HEVC_FALSE;
            }
        } else {
            hevc_ctx->curr_slice.poc = 0;
            hevc_ctx->last_slice_poc = hevc_ctx->curr_slice.poc;
            if (0 == hevc_ctx->curr_slice.nuh_temporal_id) {
                hevc_ctx->curr_slice.prev_poc = 0;
                hevc_ctx->prev_slice_poc = 0;
            }
        }

        if (sps->sample_adaptive_offset_enabled_flag) {
            hevc_ctx->curr_slice.slice_sao_luma_flag = hevc_u_v(hevc_ctx->p_bs, 1, "slice_sao_luma_flag");
            hevc_ctx->curr_slice.slice_sao_chroma_flag = hevc_u_v(hevc_ctx->p_bs, 1, "slice_sao_chroma_flag");
        }

        if (idrpic_flag) {
            hevc_ctx->curr_slice.slice_temporal_mvp_enable_flag = HEVC_FALSE;
        }

        if (hevc_ctx->curr_slice.slice_type != HEVC_I_SLICE) {
            hevc_ctx->curr_slice.num_ref_idx_active_override_flag =
                hevc_u_v(hevc_ctx->p_bs, 1, "num_ref_idx_active_override_flag");
            if (hevc_ctx->curr_slice.num_ref_idx_active_override_flag) {
                hevc_ctx->curr_slice.num_ref_idx_l0_active =
                    hevc_ue_v(hevc_ctx->p_bs, "num_ref_idx_l0_active_minus1") + 1;
                if (hevc_ctx->curr_slice.num_ref_idx_l0_active > HEVC_MAX_NUM_REF_PICS) {
                    dprint("num_ref_idx_l0_active(%d) out of range(0,%d)\n", hevc_ctx->curr_slice.num_ref_idx_l0_active,
                           HEVC_MAX_NUM_REF_PICS);
                    return HEVC_DEC_ERR;
                }

                hevc_ctx->curr_slice.num_ref_idx[0] = hevc_ctx->curr_slice.num_ref_idx_l0_active;

                if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
                    hevc_ctx->curr_slice.num_ref_idx_l1_active =
                        hevc_ue_v(hevc_ctx->p_bs, "num_ref_idx_l1_active_minus1") + 1;
                    if (hevc_ctx->curr_slice.num_ref_idx_l1_active > HEVC_MAX_NUM_REF_PICS) {
                        dprint("num_ref_idx_l1_active(%d) out of range(0,%d).\n",
                               hevc_ctx->curr_slice.num_ref_idx_l1_active, HEVC_MAX_NUM_REF_PICS);
                        return HEVC_DEC_ERR;
                    }
                    hevc_ctx->curr_slice.num_ref_idx[1] = hevc_ctx->curr_slice.num_ref_idx_l1_active;
                } else {
                    hevc_ctx->curr_slice.num_ref_idx_l1_active = 0;
                    hevc_ctx->curr_slice.num_ref_idx[1] = 0;
                }
            } else {
                hevc_ctx->curr_slice.num_ref_idx[0] = pps->num_ref_idx_l0_default_active;
                if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
                    hevc_ctx->curr_slice.num_ref_idx[1] = pps->num_ref_idx_l1_default_active;
                } else {
                    hevc_ctx->curr_slice.num_ref_idx[1] = 0;
                }
            }
        }

        if (hevc_ctx->curr_slice.slice_type == HEVC_I_SLICE) {
            num_rps_curr_temp_list = 0;
        } else {
            if (rps == NULL) {
                dprint("p_rps is null.\n");
                return HEVC_DEC_ERR;
            }

            if (rps->num_of_pics > HEVC_MAX_NUM_REF_PICS) {
                dprint("p_rps->num_of_pics out(%d) of range(0,%d).\n", rps->num_of_pics, HEVC_MAX_NUM_REF_PICS);
                return HEVC_DEC_ERR;
            }

            for (i = 0; i < rps->num_of_pics; i++) {
                if (rps->used_flag[i]) {
                    num_rps_curr_temp_list++;
                }
            }
        }

        if (hevc_ctx->curr_slice.slice_type != HEVC_I_SLICE) {
            if (pps->lists_modification_present_flag && num_rps_curr_temp_list > 1) {
                ret = hevc_dec_ref_pic_lists_modification(hevc_ctx, &hevc_ctx->curr_slice,
                                                          &(hevc_ctx->curr_slice.ref_pic_lists_modification),
                                                          num_rps_curr_temp_list);
                if (ret != HEVC_DEC_NORMAL) {
                    dprint("SH hevc_dec_ref_pic_lists_modification error.\n");
                    return HEVC_DEC_ERR;
                }
            }
        }
        if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
            hevc_ctx->curr_slice.mvd_l1_zero_flag = hevc_u_v(hevc_ctx->p_bs, 1, "mvd_l1_zero_flag");
        }
        if ((HEVC_I_SLICE != hevc_ctx->curr_slice.slice_type) && (pps->cabac_init_present_flag)) {
            hevc_ctx->curr_slice.cabac_init_flag = hevc_u_v(hevc_ctx->p_bs, 1, "cabac_init_flag");
        }
        if (hevc_ctx->curr_slice.slice_temporal_mvp_enable_flag) {
            if (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE) {
                hevc_ctx->curr_slice.collocated_from_l0_flag = hevc_u_v(hevc_ctx->p_bs, 1, "collocated_from_l0_flag");
            } else {
                hevc_ctx->curr_slice.collocated_from_l0_flag = 1;
            }
            if ((hevc_ctx->curr_slice.slice_type != HEVC_I_SLICE) &&
                    ((hevc_ctx->curr_slice.collocated_from_l0_flag && (hevc_ctx->curr_slice.num_ref_idx[0] > 1)) ||
                     (!hevc_ctx->curr_slice.collocated_from_l0_flag && (hevc_ctx->curr_slice.num_ref_idx[1] > 1)))) {
                hevc_ctx->curr_slice.collocated_ref_idx = hevc_ue_v(hevc_ctx->p_bs, "collocated_ref_idx");

                if (hevc_ctx->curr_slice.collocated_from_l0_flag) {
                    max_collocated_ref_idx = hevc_ctx->curr_slice.num_ref_idx_l0_active - 1;
                } else {
                    max_collocated_ref_idx = hevc_ctx->curr_slice.num_ref_idx_l1_active - 1;
                }
                if (hevc_ctx->curr_slice.collocated_ref_idx < 0 ||
                    hevc_ctx->curr_slice.collocated_ref_idx > max_collocated_ref_idx) {
                    dprint("collocated_from_l0_flag=%d, collocated_ref_idx(%d) out of range(0,%d).\n",
                           hevc_ctx->curr_slice.collocated_from_l0_flag, hevc_ctx->curr_slice.collocated_ref_idx,
                           max_collocated_ref_idx);
                    return HEVC_DEC_ERR;
                }
            }
        }
        if ((pps->weighted_pred_flag && (hevc_ctx->curr_slice.slice_type == HEVC_P_SLICE)) ||
                (pps->weighted_bipred_flag && (hevc_ctx->curr_slice.slice_type == HEVC_B_SLICE))) {
            ret = hevc_dec_pred_weight_table(hevc_ctx);
            if (ret != HEVC_DEC_NORMAL) {
                dprint("SH hevc_dec_pred_weight_table error.\n");
                return HEVC_DEC_ERR;
            }
        }

        if (hevc_ctx->curr_slice.slice_type != HEVC_I_SLICE) {
            hevc_ctx->curr_slice.max_num_merge_cand = MRG_MAX_NUM_CANDS -
                hevc_ue_v(hevc_ctx->p_bs, "five_minus_max_num_merge_cand");
            if (hevc_ctx->curr_slice.max_num_merge_cand < 1 ||
                hevc_ctx->curr_slice.max_num_merge_cand > 5) { /* max_num_merge_cand range is 1 to 5 */
                dprint("SH max_num_merge_cand(%d) out of range(1,5).\n", hevc_ctx->curr_slice.max_num_merge_cand);
                return HEVC_DEC_ERR;
            }
        }
        hevc_ctx->curr_slice.slice_qp_delta = hevc_se_v(hevc_ctx->p_bs, "slice_qp_delta");
        hevc_ctx->curr_slice.slice_qp = hevc_ctx->curr_slice.slice_qp_delta + pps->pic_init_qp;
        if (hevc_ctx->curr_slice.slice_qp < (hi_s32)(-sps->qp_bd_offset_y) ||
            hevc_ctx->curr_slice.slice_qp > 51) { /* slice_qp range is -hevc_ctx->curr_slice.slice_qp to 51 */
            dprint("SH slice_qp(%d) out of range [-%d,51].\n", hevc_ctx->curr_slice.slice_qp, sps->qp_bd_offset_y);
            return HEVC_DEC_ERR;
        }

        if (pps->pic_slice_chroma_qp_offsets_present_flag) {
            hevc_ctx->curr_slice.slice_cb_qp_offset = hevc_se_v(hevc_ctx->p_bs, "slice_cb_qp_offset");

            if (hevc_ctx->curr_slice.slice_cb_qp_offset < -12 || /* slice_cb_qp_offset range is -12 to 12 */
                hevc_ctx->curr_slice.slice_cb_qp_offset > 12) { /* slice_cb_qp_offset range is -12 to 12 */
                dprint("SH slice_cb_qp_offset(%d) out of range[-12,12].\n", hevc_ctx->curr_slice.slice_cb_qp_offset);
                return HEVC_DEC_ERR;
            }

            if ((hevc_ctx->curr_slice.slice_cb_qp_offset + pps->pic_cb_qp_offset) < -12 || /* range is -12 to 12 */
                    (hevc_ctx->curr_slice.slice_cb_qp_offset + pps->pic_cb_qp_offset) > 12) { /* range is -12 to 12 */
                dprint("SH slice_cb_qp_offset(%d)+pic_cb_qp_offset(%d) out of range[-12,12].\n",
                       hevc_ctx->curr_slice.slice_cb_qp_offset,
                       pps->pic_cb_qp_offset);
                return HEVC_DEC_ERR;
            }

            hevc_ctx->curr_slice.slice_cr_qp_offset = hevc_se_v(hevc_ctx->p_bs, "slice_cr_qp_offset");

            if (hevc_ctx->curr_slice.slice_cr_qp_offset < -12 || /* slice_cr_qp_offset range is -12 to 12 */
                hevc_ctx->curr_slice.slice_cr_qp_offset > 12) { /* slice_cr_qp_offset range is -12 to 12 */
                dprint("SH slice_cr_qp_offset(%d) out of range[-12,12].\n", hevc_ctx->curr_slice.slice_cr_qp_offset);
                return HEVC_DEC_ERR;
            }

            if ((hevc_ctx->curr_slice.slice_cr_qp_offset + pps->pic_cr_qp_offset) < -12 || /* range is -12 to 12 */
                    (hevc_ctx->curr_slice.slice_cr_qp_offset + pps->pic_cr_qp_offset) > 12) { /* range is -12 to 12 */
                dprint("SH slice_cr_qp_offset+p_pps->pic_cr_qp_offset(%d) out of range[-12,12].\n",
                       (hevc_ctx->curr_slice.slice_cr_qp_offset + pps->pic_cr_qp_offset));
                return HEVC_DEC_ERR;
            }
        }

        if (pps->deblocking_filter_control_present_flag) {
            if (pps->deblocking_filter_override_enabled_flag) {
                hevc_ctx->curr_slice.deblocking_filter_override_flag =
                    hevc_u_v(hevc_ctx->p_bs, 1, "deblocking_filter_override_flag");
            } else {
                hevc_ctx->curr_slice.deblocking_filter_override_flag = 0;
            }
            if (hevc_ctx->curr_slice.deblocking_filter_override_flag) {
                hevc_ctx->curr_slice.slice_disable_deblocking_filter_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                    "slice_disable_deblocking_filter_flag");
                if (!hevc_ctx->curr_slice.slice_disable_deblocking_filter_flag) {
                    hevc_ctx->curr_slice.slice_beta_offset_div2 = hevc_se_v(hevc_ctx->p_bs, "slice_beta_offset_div2");
                    if (hevc_ctx->curr_slice.slice_beta_offset_div2 < -6 || /* slice_beta_offse_div2 range is -6 to 6 */
                        hevc_ctx->curr_slice.slice_beta_offset_div2 > 6) { /* slice_beta_offset_div2 range is -6 to 6 */
                        dprint("SH slice_beta_offset_div2(%d) out of range(-6,6).\n",
                               hevc_ctx->curr_slice.slice_beta_offset_div2);
                        hevc_ctx->curr_slice.slice_beta_offset_div2 = pps->pps_beta_offset_div2;
                    }
                    hevc_ctx->curr_slice.slice_tc_offset_div2 = hevc_se_v(hevc_ctx->p_bs, "slice_tc_offset_div2");
                    if (hevc_ctx->curr_slice.slice_tc_offset_div2  < -6 || /* slice_tc_offset_div2 range is -6 to 6 */
                        hevc_ctx->curr_slice.slice_tc_offset_div2  > 6) { /* slice_tc_offset_div2 range is -6 to 6 */
                        dprint("SH slice_tc_offset_div2(%d) out of range(-6,6).\n",
                               hevc_ctx->curr_slice.slice_tc_offset_div2);
                        hevc_ctx->curr_slice.slice_tc_offset_div2 = pps->pps_tc_offset_div2;
                    }
                }
            } else {
                hevc_ctx->curr_slice.slice_disable_deblocking_filter_flag = pps->pic_disable_deblocking_filter_flag;
                hevc_ctx->curr_slice.slice_beta_offset_div2 = pps->pps_beta_offset_div2;
                hevc_ctx->curr_slice.slice_tc_offset_div2   = pps->pps_tc_offset_div2;
            }
        }

        is_saoenabled = (!sps->sample_adaptive_offset_enabled_flag) ? (HEVC_FALSE) :
            (hevc_ctx->curr_slice.slice_sao_luma_flag || hevc_ctx->curr_slice.slice_sao_chroma_flag);
        is_dbfenabled = !(hevc_ctx->curr_slice.slice_disable_deblocking_filter_flag);
        if (pps->loop_filter_across_slices_enabled_flag && (is_saoenabled || is_dbfenabled)) {
            hevc_ctx->curr_slice.slice_loop_filter_across_slices_enabled_flag = hevc_u_v(hevc_ctx->p_bs, 1,
                "slice_loop_filter_across_slices_enabled_flag");
        } else {
            hevc_ctx->curr_slice.slice_loop_filter_across_slices_enabled_flag =
                pps->loop_filter_across_slices_enabled_flag;
        }
    }

    if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag) {
        hevc_ctx->curr_slice.num_entry_point_offsets = hevc_ue_v(hevc_ctx->p_bs, "num_entry_point_offsets");

        if (pps->tiles_enabled_flag == 0 && pps->entropy_coding_sync_enabled_flag == 1) {
            max_num_entry_point_offsets = sps->ctb_num_height - 1;
        } else if (pps->tiles_enabled_flag == 1 && pps->entropy_coding_sync_enabled_flag == 0) {
            max_num_entry_point_offsets = pps->num_tile_columns * pps->num_tile_rows - 1;
        } else {
            max_num_entry_point_offsets = pps->num_tile_columns * sps->ctb_num_height - 1;
        }

        if (hevc_ctx->curr_slice.num_entry_point_offsets >
            MIN(max_num_entry_point_offsets, 255)) { /* The result of a compared with 255 is smaller */
            dprint("curr_slice.num_entry_point_offsets(%d) out of range[0,%d].\n",
                   hevc_ctx->curr_slice.num_entry_point_offsets,
                   MIN(max_num_entry_point_offsets, 255)); /* The result of a compared with 255 is smaller */
            return HEVC_DEC_ERR;
        }

        if (hevc_ctx->curr_slice.num_entry_point_offsets > 0) {
            hevc_ctx->curr_slice.offset_len = 1 + hevc_ue_v(hevc_ctx->p_bs, "offset_len_minus1");
            if (hevc_ctx->curr_slice.offset_len < 1 || hevc_ctx->curr_slice.offset_len > 32) { /* range is -1 to 32 */
                dprint("curr_slice.offset_len(%d) out of range[1,32].\n", hevc_ctx->curr_slice.offset_len);
                return HEVC_DEC_ERR;
            }
        }

        for (i = 0; i < hevc_ctx->curr_slice.num_entry_point_offsets; i++) {
            hevc_ctx->curr_slice.entry_point_offset[i] = hevc_u_v(hevc_ctx->p_bs, hevc_ctx->curr_slice.offset_len,
                                                                  "entry_point_offset_minus1");
        }
    } else {
        hevc_ctx->curr_slice.num_entry_point_offsets = 0;
    }

    if (pps->slice_segment_header_extension_present_flag) {
        hevc_ctx->curr_slice.slice_segment_header_extension_length = hevc_ue_v(hevc_ctx->p_bs,
            "slice_segment_header_extension_length");
        if (hevc_ctx->curr_slice.slice_segment_header_extension_length > 256) { /* header_extension_length less 256 */
            dprint("slice_segment_header_extension_length(%d) out of range [0,256]\n",
                   hevc_ctx->curr_slice.slice_segment_header_extension_length);
            return HEVC_DEC_ERR;
        }

        bits_left = bs_resid_bits(hevc_ctx->p_bs);
        if (bits_left < hevc_ctx->curr_slice.slice_segment_header_extension_length) {
            dprint("bits_left(%d) < slice_segment_header_extension_length(%d)\n", bits_left,
                   hevc_ctx->curr_slice.slice_segment_header_extension_length);
            return HEVC_DEC_ERR;
        }

        for (i = 0; i < hevc_ctx->curr_slice.slice_segment_header_extension_length; i++) {
            hevc_ctx->curr_slice.slice_segment_header_extension_data_byte = hevc_u_v(hevc_ctx->p_bs, 0x8,
                "slice_segment_header_extension_data_byte");
        }
    }

    if (hevc_ctx->curr_slice.dependent_slice_curstart_cuaddr == 0) {
        if (hevc_ctx->b_new_sequence) {
            if (rappic_flag) {
                if (is_dec_slice) {
                    hevc_ctx->b_new_sequence = HEVC_FALSE;
                }
                hevc_ctx->b_no_rasl_output_flag = HEVC_TRUE;
            }
        } else {
            if (rappic_flag) {
                hevc_ctx->b_no_rasl_output_flag = HEVC_FALSE;
            }
        }
    }

    if (hevc_ctx->b_no_rasl_output_flag && hevc_ctx->curr_slice.poc != 0 && rappic_flag) {
        if (hevc_ctx->curr_slice.nal_unit_type == NAL_UNIT_CODED_SLICE_CRA) {
            hevc_ctx->b_no_output_of_prior_pics_flag = HEVC_TRUE;
        } else {
            hevc_ctx->b_no_output_of_prior_pics_flag = HEVC_FALSE;
        }
    } else {
        hevc_ctx->b_no_output_of_prior_pics_flag = HEVC_FALSE;
    }

    hevc_ctx->curr_slice.new_pic_type = hevc_is_new_pic(hevc_ctx);

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_count_ref(hevc_ctx_info *hevc_ctx)
{
    hi_s32 x, y;
    hi_s32 min;
    char *str_type = HI_NULL;

    hevc_ctx->hevc_frm_type = hevc_ctx->curr_pic.pic_type;
    if (hevc_ctx->hevc_frm_type == HEVC_IDR_FRAME) {
        str_type = "IDR";
    } else if (hevc_ctx->hevc_frm_type == HEVC_BLA_FRAME) {
        str_type = "BLA";
    } else if (hevc_ctx->hevc_frm_type == HEVC_CRA_FRAME) {
        str_type = "CRA";
    } else if (hevc_ctx->hevc_frm_type == HEVC_I_FRAME) {
        str_type = "I";
    } else if (hevc_ctx->hevc_frm_type == HEVC_P_FRAME) {
        str_type = "P";
    } else if (hevc_ctx->hevc_frm_type == HEVC_B_FRAME) {
        str_type = "B";
    } else {
        str_type = "N";
    }

    dprint("F(%d): poc=%d, type=%s, ref=%d\n", hevc_ctx->hevc_frm_cnt, hevc_ctx->hevc_frm_poc, str_type,
           hevc_ctx->hevc_ref_num);
#ifdef HEVC_REF_FILE
    fprintf(g_fp_ref, "F(%d): poc=%d, type=%s, ref=%d\n", hevc_ctx->hevc_frm_cnt, hevc_ctx->hevc_frm_poc, str_type,
            hevc_ctx->hevc_ref_num);
#endif

    hevc_ctx->hevc_frm_cnt++;
    if (hevc_ctx->hevc_ref_num > 0) {
        for (x = 0; x < hevc_ctx->hevc_ref_num; x++) {
            min = hevc_ctx->hevc_ref_poc[x];
            for (y = x; y < hevc_ctx->hevc_ref_num; y++) {
                if (min > hevc_ctx->hevc_ref_poc[y]) {
                    /* instruction sync barrier for hw_csi_c_intel_pattern_match_26 issue */
                    osal_isb();

                    min = hevc_ctx->hevc_ref_poc[y];
                    hevc_ctx->hevc_ref_poc[y] = hevc_ctx->hevc_ref_poc[x];
                    hevc_ctx->hevc_ref_poc[x] = min;
                }
            }
        }

        for (x = 0; x < hevc_ctx->hevc_ref_num; x++) {
            dprint(" %d, ", hevc_ctx->hevc_ref_poc[x]);
#ifdef HEVC_REF_FILE
            fprintf(g_fp_ref, " %d, ", hevc_ctx->hevc_ref_poc[x]);
#endif
        }
#ifdef HEVC_REF_FILE
        fprintf(g_fp_ref, "\n");
#endif
        dprint("\n");
    }
#ifdef HEVC_REF_FILE
    fprintf(g_fp_ref, "\n");
    fflush(g_fp_ref);
#endif

    hevc_ctx->dmx_hevc_frm_cnt = hevc_ctx->hevc_frm_cnt;
    hevc_ctx->dmx_hevc_frm_poc = hevc_ctx->hevc_frm_poc;
    hevc_ctx->dmx_hevc_frm_type = hevc_ctx->hevc_frm_type;
    hevc_ctx->dmx_hevc_ref_num = hevc_ctx->hevc_ref_num;
    memcpy(hevc_ctx->dmx_hevc_ref_poc, hevc_ctx->hevc_ref_poc, HEVC_MAX_REF_POC * 0x4);

    return HEVC_DEC_NORMAL;
}

static hi_s32 hevc_dec_slice(hevc_ctx_info *hevc_ctx)
{
    hi_s32 ret;
    pos();

    if (hevc_ctx == NULL) {
        dprint("p_hevc_ctx is null.\n");
        return HEVC_DEC_ERR;
    }

    memset(&hevc_ctx->curr_slice, 0, sizeof(hevc_slice_segment_header));
    hevc_ctx->curr_slice.nal_unit_type   = hevc_ctx->p_curr_nal->nal_unit_type;

    ret = hevc_dec_slice_segment_header(hevc_ctx, 1);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("hevc_dec_slice dec err\n");
        return HEVC_DEC_ERR;
    }

    if (hevc_is_flush_unit(hevc_ctx->curr_slice.nal_unit_type)  ||
        hevc_ctx->b_no_output_of_prior_pics_flag == HEVC_TRUE) {
        hevc_init_dec_buffers(hevc_ctx);
    }

    dprint("new_pic_type=%d, total_slice_num=%d\n", hevc_ctx->curr_slice.new_pic_type, hevc_ctx->total_slice_num);

    if (hevc_ctx->curr_slice.new_pic_type == IS_NEW_PIC) {
        if (hevc_ctx->curr_pic.state == HEVC_PIC_DECODING) {
            hevc_count_ref(hevc_ctx);
            ret = hevc_store_pic_in_dpb(hevc_ctx);
            if (ret != HEVC_DEC_NORMAL) {
                dprint("hevc_store_pic_in_dpb failed, ret = %d\n", ret);
                return HEVC_DEC_ERR;
            }
        }

        ret = hevc_ref_pic_process(hevc_ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint("hevc_ref_pic_process failed, ret = %d\n", ret);
            return HEVC_DEC_ERR;
        }

        ret = hevc_init_pic(hevc_ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint("hevc_init_pic err, ret = %d\n", ret);
            return HEVC_DEC_ERR;
        }

        ret = hevc_write_pic_msg(hevc_ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint("hevc_write_pic_msg failed!\n");
            return HEVC_DEC_ERR;
        }

        /* robustness: drop all the slice that if this frame does'nt go with init_pic to alloc resource */
        if (hevc_ctx->curr_pic.state == HEVC_PIC_EMPTY) {
            dprint("cur pic not init yet, discard this slice!\n");
            return HEVC_DEC_ERR;
        }
    }

    hevc_pic_type_statistic(hevc_ctx);

    if (!hevc_ctx->curr_slice.dependent_slice_segment_flag) {
        // one slice one ref_list
        // do the poc management in store_pic_in_dpb
        // flag the dpb.fs_negative_ref  dpb.fs_positive_ref
        ret = hevc_dec_list(hevc_ctx);
        if (ret != HEVC_DEC_NORMAL) {
            dprint("dec list error, ret=%d\n", ret);
            return HEVC_DEC_ERR;
        }
    }

    ret = hevc_write_slice_msg(hevc_ctx);
    if (ret != HEVC_DEC_NORMAL) {
        dprint("hevc_write_slice_msg err.\n");
        return HEVC_DEC_ERR;
    }

    hevc_ctx->prev_pic_parameter_set_id = hevc_ctx->curr_slice.pic_parameter_set_id;
    hevc_ctx->prev_poc = hevc_ctx->curr_slice.poc;
    hevc_ctx->allow_start_dec = 1;

    return HEVC_DEC_NORMAL;
}

hi_s32 dmx_hevc_init(hevc_ctx_info *hevc_ctx)
{
    memset(hevc_ctx, 0, sizeof(hevc_ctx_info));

    hevc_ctx->max_vps_num   = HEVC_MAX_VIDEO_PARAM_SET_ID;
    hevc_ctx->max_sps_num   = HEVC_MAX_SEQ_PARAM_SET_ID;
    hevc_ctx->max_pps_num   = HEVC_MAX_PIC_PARAM_SET_ID;

    memset(hevc_ctx->p_vps, 0, (hevc_ctx->max_vps_num) * sizeof(hevc_video_param_set));
    memset(hevc_ctx->p_sps, 0, (hevc_ctx->max_sps_num) * sizeof(hevc_seq_param_set));
    memset(hevc_ctx->p_pps, 0, (hevc_ctx->max_pps_num) * sizeof(hevc_pic_param_set));

    hevc_ctx->curr_vps.video_parameter_set_id = hevc_ctx->max_vps_num ;
    hevc_ctx->curr_pps.pic_parameter_set_id   = hevc_ctx->max_sps_num ;
    hevc_ctx->curr_sps.seq_parameter_set_id   = hevc_ctx->max_pps_num ;

    hevc_init_scaling_order_table(hevc_ctx);

    hevc_ctx->last_display_poc = -HEVC_MAX_INT;

    hevc_init_dec_para(hevc_ctx);

    hevc_ctx->prev_pic_parameter_set_id = hevc_ctx->max_pps_num;
    hevc_ctx->prev_poc = 9999;

    /* Frame 1 invalid data report error frame */
    hevc_ctx->hevc_frm_poc = 0;
    hevc_ctx->hevc_frm_type = HEVC_ERR_FRAME;
    hevc_ctx->hevc_ref_num = 0;

    hevc_ctx->dmx_hevc_frm_poc = 0;
    hevc_ctx->dmx_hevc_frm_type = HEVC_ERR_FRAME;
    hevc_ctx->dmx_hevc_ref_num = 0;

    return HEVC_DEC_NORMAL;
}

hi_s32 hevc_get_first_nal(hi_s32 inst_idx)
{
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    sc_info  *this_sc = &ctx->this_sc;

    ctx->first_nal_offset = this_sc->global_offset;

    if (ctx->last_sei_offset < ctx->first_nal_offset && ctx->last_sei_offset >= 0) {
        ctx->first_nal_offset = ctx->last_sei_offset;
    }
    if (ctx->last_pps_offset < ctx->first_nal_offset && ctx->last_pps_offset >= 0) {
        ctx->first_nal_offset = ctx->last_pps_offset;
    }
    if (ctx->last_sps_offset < ctx->first_nal_offset && ctx->last_sps_offset >= 0) {
        ctx->first_nal_offset = ctx->last_sps_offset;
    }
    if (ctx->last_vps_offset < ctx->first_nal_offset && ctx->last_vps_offset >= 0) {
        ctx->first_nal_offset = ctx->last_vps_offset;
    }
    ctx->last_sei_offset = -1;
    ctx->last_pps_offset = -1;
    ctx->last_sps_offset = -1;
    ctx->last_vps_offset = -1;
    return 0;
}

hi_s32 hevc_make_frame(hi_s32 inst_idx)
{
    hi_s32 ret = FIDX_OK;
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    frame_pos *frm_pos = &ctx->new_frame_pos;
    hevc_ctx_info *hevc_ctx = ctx->hevc_ctx;

    frm_pos->frame_size = ctx->first_nal_offset - ctx->new_frm_offset;
    frm_pos->global_offset = ctx->new_frm_offset;
    ctx->new_frm_offset = ctx->first_nal_offset;
    if (frm_pos->frame_size <= 0) {
        return FIDX_ERR;
    }
    HI_INFO_DEMUX(" hevc_make_frame frame_size = %d glboffset %llx nfmoffset %llx\n", frm_pos->frame_size,
                  frm_pos->global_offset, ctx->new_frm_offset);

    frm_pos->cur_poc = hevc_ctx->dmx_hevc_frm_poc;
    frm_pos->ref_num = hevc_ctx->dmx_hevc_ref_num;
    memcpy(frm_pos->ref_poc, hevc_ctx->dmx_hevc_ref_poc, HEVC_MAX_REF_POC * 0x4);

    if (hevc_ctx->dmx_hevc_frm_type == HEVC_IDR_FRAME) {
        frm_pos->frame_type = FIDX_FRAME_TYPE_IDR;
    } else if (hevc_ctx->dmx_hevc_frm_type == HEVC_BLA_FRAME) {
        frm_pos->frame_type = FIDX_FRAME_TYPE_BLA;
    } else if (hevc_ctx->dmx_hevc_frm_type == HEVC_CRA_FRAME) {
        frm_pos->frame_type = FIDX_FRAME_TYPE_CRA;
    } else if (hevc_ctx->dmx_hevc_frm_type == HEVC_I_FRAME) {
        frm_pos->frame_type = FIDX_FRAME_TYPE_I;
    } else if (hevc_ctx->dmx_hevc_frm_type == HEVC_P_FRAME) {
        frm_pos->frame_type = FIDX_FRAME_TYPE_P;
    } else if (hevc_ctx->dmx_hevc_frm_type == HEVC_B_FRAME) {
        frm_pos->frame_type = FIDX_FRAME_TYPE_B;
    } else {
        frm_pos->frame_type = FIDX_FRAME_TYPE_UNKNOWN;
    }

    if (frm_pos->frame_type == FIDX_FRAME_TYPE_IDR
            || frm_pos->frame_type == FIDX_FRAME_TYPE_BLA
            || frm_pos->frame_type == FIDX_FRAME_TYPE_CRA
            || frm_pos->frame_type == FIDX_FRAME_TYPE_I) {
        frm_pos->pts = ctx->next_pts;
        ctx->next_pts = ctx->pts;
        ctx->pts  = DMX_INVALID_PTS;
    } else {
#ifndef PUT_PTS_ON_I_FRAME_ONLY
        frm_pos->pts  = ctx->next_pts;
#endif
        ctx->next_pts = ctx->pts;
#ifndef PUT_PTS_ON_I_FRAME_ONLY
        ctx->pts  = DMX_INVALID_PTS;
#endif
    }

    return ret;
}

hi_s32 process_sc_hevc(hi_s32 inst_idx)
{
    hi_s32 ret = FIDX_OK;
    fidx_ctx *ctx = &g_fidx_iis[inst_idx];
    sc_info  *this_sc = &ctx->this_sc;
    hevc_ctx_info *hevc_ctx = ctx->hevc_ctx;

    FIDX_ASSERT_RET(0 != ctx->this_scvalid, "this_sc is not valid\n");
    FIDX_ASSERT_RET(!IS_SC_WRONG(), "not enough data for this_sc\n");

    pos();
    dprint("pst_ctx->data_len = 0x%x\n", ctx->this_scdata_len);
    dprint("data: %08x, %08x\n", *(hi_u32 *)(ctx->hevc_scdata + 0), *(hi_u32 *)(ctx->hevc_scdata + 0x4));

    if (ctx->new_frm_offset < 0) {
        ctx->new_frm_offset = this_sc->global_offset;
    }

    bs_init(hevc_ctx->p_bs, (hi_u8 *)(&ctx->hevc_scdata[0]), ctx->this_scdata_len);

    hevc_ctx->p_curr_nal->forbidden_zero_bit      = bs_get(hevc_ctx->p_bs, 1);
    hevc_ctx->p_curr_nal->nal_unit_type           = bs_get(hevc_ctx->p_bs, 0x6);
    hevc_ctx->p_curr_nal->nuh_reserved_zero_6bits = bs_get(hevc_ctx->p_bs, 0x6);
    hevc_ctx->p_curr_nal->nuh_temporal_id         = bs_get(hevc_ctx->p_bs, 0x3);
    if (hevc_ctx->p_curr_nal->nuh_temporal_id == 0x0) {
        dprint("nuh_temporal_id_plus1 shall not be equal to 0.\n");
        return HEVC_DEC_ERR;
    }
    hevc_ctx->p_curr_nal->nuh_temporal_id -= 1;

    dprint("nal_unit_type = %x\n", hevc_ctx->p_curr_nal->nal_unit_type);

    if (hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_EOS || hevc_ctx->p_curr_nal->nal_unit_type == NAL_UNIT_EOB) {
        hevc_ctx->b_new_sequence = HEVC_TRUE;
    }

    dprint("entry proc hevc globaloffset %llx\n", this_sc->global_offset);
    switch (hevc_ctx->p_curr_nal->nal_unit_type) {
        case NAL_UNIT_VPS:
            dprint("nal = vps\n");
            ret = hevc_dec_vps(hevc_ctx);
            ctx->last_vps_offset = this_sc->global_offset;
            break;
        case NAL_UNIT_SPS:
            dprint("nal = sps\n");
            ret = hevc_dec_sps(hevc_ctx);
            ctx->last_sps_offset = this_sc->global_offset;
            break;
        case NAL_UNIT_PPS:
            dprint("nal = pps\n");
            ret = hevc_dec_pps(hevc_ctx);
            ctx->last_pps_offset = this_sc->global_offset;
            break;
        case NAL_UNIT_PREFIX_SEI:
            ctx->last_sei_offset = this_sc->global_offset;
            break;
        case NAL_UNIT_SUFFIX_SEI:
            dprint("nal = sei\n");
            break;
        case NAL_UNIT_CODED_SLICE_TRAIL_R:
        case NAL_UNIT_CODED_SLICE_TRAIL_N:
        case NAL_UNIT_CODED_SLICE_TLA_R:
        case NAL_UNIT_CODED_SLICE_TSA_N:
        case NAL_UNIT_CODED_SLICE_STSA_R:
        case NAL_UNIT_CODED_SLICE_STSA_N:
        case NAL_UNIT_CODED_SLICE_BLA_W_LP:
        case NAL_UNIT_CODED_SLICE_BLA_W_RADL:
        case NAL_UNIT_CODED_SLICE_BLA_N_LP:
        case NAL_UNIT_CODED_SLICE_IDR_W_RADL:
        case NAL_UNIT_CODED_SLICE_IDR_N_LP:
        case NAL_UNIT_CODED_SLICE_CRA:
        case NAL_UNIT_CODED_SLICE_RADL_N:
        case NAL_UNIT_CODED_SLICE_RADL_R:
        case NAL_UNIT_CODED_SLICE_RASL_N:
        case NAL_UNIT_CODED_SLICE_RASL_R:
            dprint("nal = slice\n");
            ret = hevc_dec_slice(hevc_ctx);

            if (hevc_ctx->curr_slice.new_pic_type == IS_NEW_PIC) {
                hevc_get_first_nal(inst_idx);

                if (hevc_make_frame(inst_idx) == FIDX_OK) {
                    dmx_rec_update_frame_info(ctx->param, &ctx->new_frame_pos);
                    memset(&ctx->new_frame_pos, 0, sizeof(frame_pos));
                    ctx->new_frame_pos.frame_type = FIDX_FRAME_TYPE_UNKNOWN;
                }
            }
            break;
        default:
            dprint("nal = non\n");
            break;
    }

    return ret;
}

/*!***********************************************************************
@brief
    feed start code to FIDX.
    there are 2 method to feed necessary information to FIDX:
    1. feed stream directly. Call FIDX_MakeFrameIndex()
    2. feed start code. In this method, the start code must be scanned outside,
       This call this function to create index.
 ************************************************************************/
hi_void  fidx_feed_start_code(hi_s32 inst_idx, const findex_scd *sc)
{
    fidx_ctx *ctx;
    frame_pos pes_frame;

    ctx = &g_fidx_iis[inst_idx];

    /* get SC info */
    ctx->this_sc.sc_id = sc->start_code;
    ctx->this_sc.global_offset = sc->global_offset;
    ctx->this_sc.offset_in_packet = 0;
    ctx->this_sc.packet_count = 0;

    /* fill SC data */
    if (ctx->video_standard == VIDSTD_HEVC) {
        /* adapt this_scdata for general checking. */
        ctx->this_scdata[0] = sc->start_code;
        memcpy(ctx->this_scdata + 1, sc->extra_scdata + 0x4, 0x8); /* +4 for skip 00 00 01 0x. */

        ctx->hevc_scdata = sc->extra_scdata + 0x3; /* +3 for skip 00 00 01 */
        ctx->this_scdata_len = sc->extra_real_scdata_size - 0x3;
    } else {
        ctx->this_scdata[0] = sc->start_code;
        memcpy(ctx->this_scdata + 1, sc->data_after_sc, 0x8);
        ctx->this_scdata_len = 1 + 0x8;
    }
    ctx->this_scvalid = 1;

    /*! if this SC is a PES SC, output it here simplay, otherwise process it according to the video standard */
    if (is_pes_sc(sc->start_code, ctx->video_standard) == 1) {
        /*! the PTS after PES SC is valid, record it */
        ctx->pts  = sc->pts_us;

        /*! report the PES position */
        memset(&pes_frame, 0, sizeof(frame_pos));
        pes_frame.frame_type = FIDX_FRAME_TYPE_PESH;
        pes_frame.global_offset = ctx->this_sc.global_offset;
        pes_frame.offset_in_packet = ctx->this_sc.offset_in_packet;
        pes_frame.packet_count = ctx->this_sc.packet_count;
        pes_frame.pts = sc->pts_us;

        dmx_rec_update_frame_info(ctx->param, &pes_frame);

        /* record the PTS of first frame from record beginning */
        if (unlikely(ctx->next_pts == 0)) {
            ctx->next_pts = sc->pts_us;
        }

        ctx->this_scvalid = 0;
        ctx->this_scdata_len = 0;
    } else {
        process_this_sc(inst_idx);
        ctx->this_scvalid = 0;
        ctx->this_scdata_len = 0;
    }

    return;
}


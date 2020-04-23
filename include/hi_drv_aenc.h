/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: implement of aenc driver
 * Author: audio
 * Create: 2019-06-21
 * Notes:  NA
 * History: 2019-06-21 Initial version for Hi3796CV300
 */

#ifndef __HI_DRV_AENC_H__
#define __HI_DRV_AENC_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AENC_INSTANCE_MAXNUM 3     /* max encoder instance */

#define AENC_MIN_INPUT_BUFFER_SIZE (1024 * 256)
#define AENC_MAX_INPUT_BUFFER_SIZE (1024 * 512 * 4)
#define AENC_DEFAULT_INPUT_BUFFER_SIZE (1024 * 512)
#define AENC_DEFAULT_OUTBUF_NUM 32

#define AENC_MAX_SRC_FRAC (48000 / 8000)
#define AENC_MAX_CHANNELS 2
#define ANEC_MAX_SMAPLEPERFRAME 2048
#define AENC_MAX_POSTPROCESS_FRAME (ANEC_MAX_SMAPLEPERFRAME * AENC_MAX_CHANNELS * AENC_MAX_SRC_FRAC)
#define AENC_WORK_BUFFER_NUM 2

#define AENC_PATH_MAX 512

typedef enum {
    ANEC_SOURCE_AI = 0,
    ANEC_SOURCE_CAST,
    ANEC_SOURCE_VIRTRACK,
    ANEC_SOURCE_MAX
} aenc_source_type;

typedef struct {
    aenc_source_type type;
    hi_handle src;
} aenc_info_attach;

typedef enum {
    AENC_CMD_CTRL_START = 0,
    AENC_CMD_CTRL_STOP,
    AENC_CMD_CTRL_MAX
} aenc_cmd_ctrl;

typedef enum {
    AENC_CMD_PROC_SAVE_PCM = 0,
    AENC_CMD_PROC_SAVE_ES,
    AENC_CMD_PROC_MAX
} aenc_cmd_save;

typedef struct {
    aenc_info_attach attach;
    hi_bool aenc_work_enable;
    hi_u32 codec_id;
    hi_char codec_type[32]; // 32 is a number
    hi_u32 sample_rate;
    hi_u32 bit_width;
    hi_u32 channels;

    hi_bool auto_src;
    hi_u32 enc_frame;
    hi_u32 err_frame;

    hi_u32 in_buf_size;
    hi_u64 in_buf_read;
    hi_u64 in_buf_write;

    hi_u32 out_frame_num;
    hi_u32 out_frame_r_idx;
    hi_u32 out_frame_w_idx;
    hi_u32 dbg_send_buf_count_try;
    hi_u32 dbg_send_buf_count;
    hi_u32 dbg_receive_stream_count_try;
    hi_u32 dbg_receive_stream_count;
    hi_u32 dbg_release_stream_count_try;
    hi_u32 dbg_release_stream_count;
    hi_u32 dbg_try_encode_count;

    aenc_cmd_ctrl pcm_ctrl_state;
    aenc_cmd_ctrl es_ctrl_state;
    hi_u32 save_pcm_cnt;
    hi_u32 save_es_cnt;
    hi_char file_path[AENC_PATH_MAX];
} aenc_proc_item;

/* use macro to check parameter */
#define HI_MPI_AENC_RET_USER_ERR(drv_err_code, h_aenc) do {     \
    hi_s32 retvalerr;                                           \
    if (HI_SUCCESS != drv_err_code) {                           \
        switch (drv_err_code) {                                 \
            case HI_ERR_AENC_IN_BUF_FULL:                       \
            case HI_ERR_AENC_DEV_NOT_OPEN:                      \
            case HI_ERR_AENC_NULL_PTR:                          \
            case HI_ERR_AENC_INVALID_PARA:                      \
            case HI_ERR_AENC_OUT_BUF_FULL:                      \
            case HI_ERR_AENC_INVALID_OUTFRAME:                  \
            case HI_ERR_AENC_DATASIZE_EXCEED:                   \
            case HI_ERR_AENC_OUT_BUF_EMPTY:                     \
                retvalerr = drv_err_code;                       \
                break;                                          \
            default:                                            \
                retvalerr = HI_FAILURE;                         \
                break;                                          \
        }                                                       \
        if (HI_ERR_AENC_IN_BUF_FULL == retvalerr) {             \
            HI_INFO_AENC(" DriverErrorCode =0x%x\n",retvalerr); \
        } else if (HI_ERR_AENC_OUT_BUF_EMPTY == retvalerr) {    \
            HI_INFO_AENC(" DriverErrorCode =0x%x\n",retvalerr); \
        } else {                                                \
            HI_ERR_AENC(" DriverErrorCode =0x%x\n",retvalerr);  \
        }                                                       \
        AENC_CHAN_UNLOCK(h_aenc);                               \
        return retvalerr;                                       \
    }                                                           \
} while (0)

#define CHECK_AENC_HANDLE(h_aenc) do {                       \
    if ((hi_s32)(h_aenc) >= AENC_INSTANCE_MAXNUM) {          \
        HI_ERR_AENC("invalid AENC handle =0x%x!\n", h_aenc); \
        return HI_FAILURE;                                   \
    }                                                        \
} while (0)

#define CHECK_AENC_CHN_OPEN_RET(h_aenc) do {                            \
    if (0 == g_aenc_init_cnt) {                                         \
        AENC_CHAN_UNLOCK(h_aenc);                                       \
        HI_ERR_AENC("AENC device state err: please init aenc first\n"); \
        return HI_FAILURE;                                              \
    }                                                                   \
    if (HI_NULL == g_aenc_chan[h_aenc]) {                               \
        AENC_CHAN_UNLOCK(h_aenc);                                       \
        return HI_FAILURE;                                              \
    }                                                                   \
    if (HI_FALSE == g_aenc_chan[h_aenc]->assign) {                      \
        AENC_CHAN_UNLOCK(h_aenc);                                       \
        HI_ERR_AENC("AENC device not open!\n");                         \
        return HI_ERR_AENC_DEV_NOT_OPEN;                                \
    }                                                                   \
} while (0)


#define CHECK_AENC_CH_CREATE(h_aenc) do {                               \
    if (!g_aenc_init_cnt) {                                             \
        HI_ERR_AENC("AENC device state err: please init aenc first\n"); \
        return HI_FAILURE;                                              \
    }                                                                   \
    if (h_aenc >= AENC_INSTANCE_MAXNUM) {                               \
        HI_ERR_AENC("Invalid Aenc handle(0x%x)\n", h_aenc);             \
        return HI_ERR_AENC_DEV_NOT_OPEN;                                \
    }                                                                   \
    if (HI_FALSE == g_aenc_chan[h_aenc]->assign) {                      \
        HI_ERR_AENC("AENC device not open!\n");                         \
        return HI_ERR_AENC_DEV_NOT_OPEN;                                \
    }                                                                   \
} while (0)

#define CHECK_AENC_NULL_PTR(ptr) do {         \
    if (NULL == ptr) {                        \
        HI_ERR_AENC("invalid NULL poiner!\n");\
        return HI_ERR_AENC_NULL_PTR;          \
    }                                         \
} while (0)

#define CHECK_AENC_OPEN_FORMAT(rate, ch, width, interleaved) do {                              \
    if (rate < HI_SAMPLE_RATE_8K || rate > HI_SAMPLE_RATE_48K) {                       \
        HI_ERR_AENC("invalid  Pcm Format: HA Encoder only support 8K~48K samplerate\n");       \
        return HI_ERR_AENC_INVALID_PARA;                                                       \
    }                                                                                          \
    if (HI_FALSE == interleaved) {                                                             \
        HI_ERR_AENC("invalid  Pcm Format: HA Encoder only support 16bit-Interleaved format\n");\
        return HI_ERR_AENC_INVALID_PARA;                                                       \
    }                                                                                          \
    if (16 != width) {                                                                         \
        HI_ERR_AENC("invalid  Pcm Format: HA Encoder only support 16bit-Interleaved format\n");\
        return HI_ERR_AENC_INVALID_PARA;                                                       \
    }                                                                                          \
    if (2 != ch && 1 != ch) {                                                                  \
        HI_ERR_AENC("invalid Pcm Format: HA Encoder only support 1 and 2 channel\n");          \
        return HI_ERR_AENC_INVALID_PARA;                                                       \
    }                                                                                          \
} while (0)

#define CHECK_AENC_PCM_SAMPLESIZE(pcm_samples_per_frame) do { \
    if (pcm_samples_per_frame > ANEC_MAX_SMAPLEPERFRAME) { \
        HI_ERR_AENC("invalid  AO Pcm Format: Pcm SamplesPerFrame = %d\n", pcm_samples_per_frame); \
        return HI_ERR_AENC_INVALID_PARA; \
    } \
} while (0)

#define CHECK_AENC_PCM_CHANNEL(ch) do { \
    if (ch > 2) { \
        HI_ERR_AENC("invalid  pcm channel(%d): must sure channel <= 2\n", ch); \
        return HI_ERR_AENC_INVALID_PARA; \
    } \
} while (0)

#define CHECK_AENC_PCM_SAMPLERATE(rate) do {              \
    switch (rate) {                                       \
        case HI_SAMPLE_RATE_8K:                       \
        case HI_SAMPLE_RATE_11K:                      \
        case HI_SAMPLE_RATE_12K:                      \
        case HI_SAMPLE_RATE_16K:                      \
        case HI_SAMPLE_RATE_22K:                      \
        case HI_SAMPLE_RATE_24K:                      \
        case HI_SAMPLE_RATE_32K:                      \
        case HI_SAMPLE_RATE_44K:                      \
        case HI_SAMPLE_RATE_48K:                      \
        case HI_SAMPLE_RATE_88K:                      \
        case HI_SAMPLE_RATE_96K:                      \
        case HI_SAMPLE_RATE_176K:                     \
        case HI_SAMPLE_RATE_192K:                     \
            break;                                        \
        default:                                          \
            HI_WARN_AO("invalid samplerate(%d)\n", rate); \
            return HI_ERR_AO_INVALID_PARA;                \
    }                                                     \
} while (0)

#define CHECK_AENC_PCM_BITWIDTH(bitwidth) do {                                          \
    if (16 != bitwidth && 24 != bitwidth) {                                             \
        HI_ERR_AENC("invalid  pcm Bitwidth(%d), must sure 16bit or 24bit\n", bitwidth); \
        return HI_ERR_AENC_INVALID_PARA;                                                \
    }                                                                                   \
} while (0)

/* Define Debug Level For HI_ID_AENC */
#define HI_FATAL_AENC(fmt...)  HI_FATAL_PRINT(HI_ID_AENC, fmt)
#define HI_ERR_AENC(fmt...)    HI_ERR_PRINT(HI_ID_AENC, fmt)
#define HI_WARN_AENC(fmt...)   HI_WARN_PRINT(HI_ID_AENC, fmt)
#define HI_INFO_AENC(fmt...)   HI_INFO_PRINT(HI_ID_AENC, fmt)

#define DRV_AENC_DEVICE_NAME "hi_aenc"

typedef struct {
    hi_u32 id;
    hi_s32 map_fd;
    hi_u64 proc_phys_addr;
} aenc_proc_param;

/* 'IOC_TYPE_AENC' means AENC magic macro */
#define     DRV_AENC_PROC_INIT _IOR(HI_ID_AENC, 0, aenc_proc_param)
#define     DRV_AENC_PROC_EXIT _IO(HI_ID_AENC, 1)

#ifdef __cplusplus
}
#endif

#endif /* __HI_DRV_AENC_H__ */

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adec driver header file
 * Author: audio
 * Create: 2019-05-26
 * Notes: NA
 * History: 2019-05-26 Initial version for Hi3796CV300
 */

#ifndef __HI_DRV_ADEC_H__
#define __HI_DRV_ADEC_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ADEC_INSTANCE_MAXNUM    8

#define ADEC_MAX_INPUT_BLOCK_SIZE         0x10000   /* max input data size of decoder */
#define ADEC_MAX_VOLUME                   100       /* 32 */
#define ADEC_MAX_CHANNLES                 8

#define ADEC_CODEC_NAME_LENGTH            64

/* Max frame number can be used in ADEC */
#define ADEC_MAX_WORK_BUFFER_NUMBER       40
#define ADEC_DEFAULT_WORKINGBUF_NUM       30

#define ADEC_MAX_INPUT_BUFFER_SIZE        (16 * 1024 * 1024)
#define ADEC_MIN_INPUT_BUFFER_SIZE        (2 * 1024)
#define ADEC_DEFAULT_INPUT_BUFFER_SIZE    (128 * 1024)
#define ADEC_DEFAULT_INPUT_AD_BUFFER_SIZE (128 * 1024)

#define ADEC_MAX_STORED_PTS_NUM           2048
#define ADEC_PATH_MAX                     512

/* note: ADEC_MAX_STORED_PACKET_NUM is not less than ADEC_MAX_STORED_PTS_NUM */
#define ADEC_MAX_STORED_PACKET_NUM       (ADEC_MAX_STORED_PTS_NUM)

/* max adec consume bytes for one frame */
#define ADEC_MAX_FRAME_CONSUME_THD       (1024 * 128)

#define ADEC_DEFAULT_OUTPUT_FRAME_MS_LIMIT 20

/* speed play */
#define ADEC_SPEED_INT_SLOW     0
#define ADEC_SPEED_INT_NORMAL   1
#define ADEC_SPEED_INT_FAST     2

#define CHECK_ADEC_HANDLE(h_adec)                       \
    do {                                               \
        if ((hi_s32)(h_adec) >= ADEC_INSTANCE_MAXNUM) { \
            HI_ERR_ADEC("invalid adec handle =0x%x!\n", h_adec); \
            return HI_ERR_ADEC_INVALID_PARA;           \
        }                                              \
    } while (0)

#define CHECK_ADEC_NULL_PTR(ptr)                  \
    do {                                          \
        if (ptr == HI_NULL) {                     \
            HI_ERR_ADEC("invalid NULL poiner!\n"); \
            return HI_ERR_ADEC_NULL_PTR;          \
        }                                         \
    } while (0)

#define CHECK_NULL_PTR_NORET(ptr)                 \
    do {                                          \
        if (ptr == HI_NULL) {                     \
            HI_ERR_ADEC("invalid NULL poiner!\n"); \
            return;                               \
        }                                         \
    } while (0)

#define CHECK_ADEC_STATE_NOMUTE(enable)         \
    do {                                         \
        if (enable == HI_FALSE) {               \
            HI_ERR_ADEC(" adec state invalid\n"); \
            return HI_FAILURE;                   \
        }                                        \
    } while (0)

#define CHECK_ADEC_STATE(enable, adec_mutex)    \
    do {                                        \
        if (enable == HI_FALSE) {              \
            HI_ERR_ADEC("adec state invalid\n"); \
            ADEC_UNLOCK(adec_mutex);             \
            return HI_FAILURE;                  \
        }                                       \
    } while (0)

#define CHECK_ADEC_STATE_WARNING(enable, adec_mutex) \
    do {                                            \
        if (enable == HI_FALSE) {                   \
            HI_WARN_ADEC("adec state invalid\n");   \
            ADEC_UNLOCK(adec_mutex);                \
            return HI_FAILURE;                      \
        }                                           \
    } while (0)

#define CHECK_ADEC_STATEARG2(enable, adec_mutex1, adec_mutex2) \
    do {                                                      \
        if (enable == HI_FALSE) {                            \
            HI_ERR_ADEC("adec state invalid\n");          \
            ADEC_UNLOCK(adec_mutex1);                          \
            ADEC_UNLOCK(adec_mutex2);                          \
            return HI_FAILURE;                                \
        }                                                     \
    } while (0)

#define CHECK_ADEC_STATEARG3(enable, adec_mutex1, adec_mutex2, adec_mutex3) \
    do {                                                                  \
        if (enable == HI_FALSE) {                                        \
            HI_ERR_ADEC("adec state invalid\n");          \
            ADEC_UNLOCK(adec_mutex1);                                      \
            ADEC_UNLOCK(adec_mutex2);                                      \
            ADEC_UNLOCK(adec_mutex3);                                      \
            return HI_FAILURE;                                            \
        }                                                                 \
    } while (0)

#define CHECK_ADEC_OUTBUF_NUMBER(number)             \
    do {                                             \
        if (number >= ADEC_MAX_WORK_BUFFER_NUMBER) { \
            HI_ERR_ADEC("invalid output no.(%d)!\n",number);          \
            return HI_FAILURE;                       \
        }                                            \
    } while (0)

#define CHECK_ADEC_OUTBUF_NUMBER_NORET(number)       \
    do {                                             \
        if (number >= ADEC_MAX_WORK_BUFFER_NUMBER) { \
            HI_ERR_ADEC("invalid output no.(%d)!\n",number);         \
            return;                                  \
        }                                            \
    } while (0)

#define HI_MPI_ADEC_RET_USER_ERR_ARG2(drv_err_code, adec_mutex1, adec_mutex2) \
    do {                                                               \
        if (drv_err_code != HI_SUCCESS) {                                \
            HI_ERR_ADEC("err_code =0x%x\n",drv_err_code); \
            ADEC_UNLOCK(adec_mutex1);                                   \
            ADEC_UNLOCK(adec_mutex2);                                   \
            return drv_err_code;                                         \
        }                                                              \
    } while (0)

#define HI_MPI_ADEC_RET_USER_ERR2_ARG2(drv_err_code, adec_mutex1, adec_mutex2) \
    do {                                                                \
        HI_ERR_ADEC("err_code =0x%x\n",drv_err_code); \
        ADEC_UNLOCK(adec_mutex1);                                        \
        ADEC_UNLOCK(adec_mutex2);                                        \
        return drv_err_code;                                              \
    } while (0)

#define CHECK_ADEC_NULL_PTR_IN_LOCK(ptr, mutex)   \
    do {                                          \
        if (ptr == HI_NULL) {                     \
            HI_ERR_ADEC("invalid NULL poiner!\n"); \
            ADEC_UNLOCK(mutex);                   \
            return HI_FAILURE;                    \
        }                                         \
    } while (0)

#define CHECK_ADEC_CHN_NULL(adec_chan, mutex)      \
    do {                                           \
        if (adec_chan == HI_NULL) {                \
            HI_WARN_ADEC("adec channel is NULL\n"); \
            ADEC_UNLOCK(mutex);                    \
            return HI_FAILURE;                     \
        }                                          \
    } while (0)

#define CHECK_ADEC_SPEED(speedint) \
    do {                                             \
        if ((speedint != ADEC_SPEED_INT_SLOW)   &&   \
            (speedint != ADEC_SPEED_INT_NORMAL) &&   \
            (speedint != ADEC_SPEED_INT_FAST)) {    \
            HI_WARN_ADEC("invalid adec speed int(%d)\n", speedint); \
            return HI_FAILURE;                       \
        }                                            \
    }while (0)

typedef enum {
    ADEC_CMD_CTRL_START = 0,
    ADEC_CMD_CTRL_STOP,
    ADEC_CMD_CTRL_MAX
} adec_cmd_ctrl;

typedef enum {
    ADEC_CMD_PROC_SAVE_PCM = 0,
    ADEC_CMD_PROC_SAVE_ES,
    ADEC_CMD_PROC_MAX
} adec_cmd_save;

typedef enum {
    /* this buffer is empty or do not has enough data, you can not read data from this buffer */
    ADEC_OUTPUT_FILL_THIS_BUF = 0,

    /* this buffer has enough data and ready for output */
    ADEC_OUTPUT_EMPTY_THIS_BUF,

    ADEC_OUTPUT_BUF_STATUS_MAX
} adec_output_buf_status;

typedef struct {
    hi_bool flag; /* buffer flag */
    hi_u32 pts; /* play time stamp may be interpolated */
    hi_u32 org_pts; /* original play time stamp */
    hi_u32 *out_buf; /* output buffer */
    hi_s32 *pcm_out_buf;
    hi_s32 *bits_out_buf; /* NULL, if decoder dont suppourt iec 61937 */
    hi_u32 pcm_out_samples_per_frame;
    hi_u32 bits_out_bytes_per_frame;
    hi_u32 out_channels;
    hi_u32 out_sample_rate;
    hi_bool interleaved;
    hi_u32 bit_per_sample; /* bit per sample */
    hi_u32 frame_index;
    hi_u32 buf_status;
} adec_outputbuf;

typedef struct {
    hi_u32 pts; /* play time stamp */
    hi_u32 beg_pos; /* stream offset address of PTS */
    hi_u32 end_pos; /* stream offset end address of PTS */
} adec_pts;

typedef struct {
    hi_bool packet_eos_flag; /* patket EOS */
    hi_s32 beg_pos; /* stream offset address of patket */
    hi_u32 end_pos; /* stream offset end address of patket */
} adec_packet;

typedef struct {
    hi_u8 *data; /* buffer addr */
    hi_u32 buf_size; /* buffer length */
    hi_s32 buf_read_pos; /* buffer buf_read_pos ptr */
    hi_u32 buf_write_pos; /* buffer buf_write_pos ptr */
    hi_u32 buf_free; /* buffer buf_free length */

    hi_u32 buf_padding_size;

#ifdef ADEC_MMZ_INBUF_SUPPORT
    hi_mmz_buf adec_in_m_mz_buf;
#endif
    hi_u32 stream_write_pos;
    hi_u32 boundary; /* pts read pointers wrap point */
} adec_stream_buffer;

typedef struct {
    hi_bool adec_work_enable;
    hi_char codec_type[ADEC_CODEC_NAME_LENGTH];
    hi_char codec_description[ADEC_CODEC_NAME_LENGTH];
    hi_u32 codec_id;
    hi_bool fmt;
    hi_sample_rate sample_rate;
    hi_bit_depth bit_width;
    hi_u32 pcm_samples_per_frame;
    hi_u32 framn_nm;
    hi_u32 err_frame_num;

    hi_u32 buf_size;
    hi_s32 buf_read;
    hi_u32 buf_write;
    hi_u32 frame_size;
    hi_u32 frame_read;
    hi_u32 frame_write;
    hi_u32 frame_read_wrap;
    hi_u32 frame_write_wrap;

    hi_u32 pts_lost;
    hi_u32 volume;
    hi_u32 out_channels;
    hi_u32 org_channels; /* output, number of stream original channels. */
    hi_u32 bits_out_bytes_per_frame;
    hi_u32 dbg_get_buf_count_try;
    hi_u32 dbg_get_buf_count;
    hi_u32 dbg_put_buf_count_try;
    hi_u32 dbg_put_buf_count;
    hi_u32 dbg_receive_frame_count_try;
    hi_u32 dbg_receive_frame_count;
    hi_u32 dbg_send_straem_count_try;
    hi_u32 dbg_send_straem_count;
    hi_u32 dbg_try_decode_count;

    hi_u32 frame_consumed_bytes;
    hi_u32 last_correct_frame_num;
    hi_u32 thread_id;

    adec_cmd_ctrl pcm_ctrl_state;
    adec_cmd_ctrl es_ctrl_state;
    hi_u32 save_pcm_cnt;
    hi_u32 save_es_cnt;
    hi_char es_file_path[ADEC_PATH_MAX];
    hi_char pcm_file_path[ADEC_PATH_MAX];

    hi_u32 thread_begin_time;
    hi_u32 thread_end_time;
    hi_u32 thread_sche_time_out_cnt;
    hi_u32 thread_exe_time_out_cnt;

    hi_u32 adec_system_sleep_time;
    hi_u32 codec_unsupport_num;
    hi_u32 stream_corrupt_num;
    hi_u32 adec_delay_ms;
    hi_u32 bit_rate;
    hi_u32 speed_int;
    hi_u32 speed_deci;
    hi_u32 speed_error_num;
} adec_proc_item;

typedef struct
{
    hi_u32 id;
    hi_s32 map_fd;
    hi_u64 proc_phys_addr;
} adec_proc_param;

#define DRV_ADEC_DEVICE_NAME "hi_adec"
#define ADEC_DEVICE_NAME "/dev/" DRV_ADEC_DEVICE_NAME

/* 'IOC_TYPE_ADEC' means ADEC magic macro */
#define DRV_ADEC_PROC_INIT _IOR(HI_ID_ADEC, 0, adec_proc_param)
#define DRV_ADEC_PROC_EXIT _IO(HI_ID_ADEC, 1)

/* define debug level for HI_ID_ADEC */
#define HI_FATAL_ADEC(fmt...) \
    HI_FATAL_PRINT(HI_ID_ADEC, fmt)

#define HI_ERR_ADEC(fmt...) \
    HI_ERR_PRINT(HI_ID_ADEC, fmt)

#define HI_WARN_ADEC(fmt...) \
    HI_WARN_PRINT(HI_ID_ADEC, fmt)

#define HI_INFO_ADEC(fmt...) \
    HI_INFO_PRINT(HI_ID_ADEC, fmt)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __HI_DRV_ADEC_H__ */

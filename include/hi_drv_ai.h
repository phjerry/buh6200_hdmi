/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ai driver head file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_DRV_AI_H__
#define __HI_DRV_AI_H__

#include "hi_ai_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AI_MAX_TOTAL_NUM 4
#define AI_MAX_HANDLE_ID ((HI_ID_AI << 16) | AI_MAX_TOTAL_NUM)
#define AI_MIN_HANDLE_ID (HI_ID_AI << 16)
#define AI_CHNID_MASK    0xffff

/* define debug level for HI_ID_AI */
#define HI_FATAL_AI(fmt...) \
    HI_FATAL_PRINT(HI_ID_AI, fmt)

#define HI_ERR_AI(fmt...) \
    HI_ERR_PRINT(HI_ID_AI, fmt)

#define HI_WARN_AI(fmt...) \
    HI_WARN_PRINT(HI_ID_AI, fmt)

#define HI_INFO_AI(fmt...) \
    HI_INFO_PRINT(HI_ID_AI, fmt)

#define CHECK_AI_NULL_PTR(p) do {     \
    if (HI_NULL == p) {               \
        HI_ERR_AI("NULL pointer!\n"); \
        return HI_ERR_AI_NULL_PTR;    \
    }                                 \
} while (0)

#define CHECK_AI_ID(handle) do {                                       \
    if ((AI_MAX_HANDLE_ID <= handle) || (AI_MIN_HANDLE_ID > handle)) { \
        HI_ERR_AI("invalid ai id 0x%x\n", handle);                     \
        return HI_ERR_AI_INVALID_ID;                                   \
    }                                                                  \
} while (0)

#define CHECK_AI_CHN_STATE(ai, state) do {                       \
    if (HI_NULL == state) {                                      \
        HI_ERR_AI("AI chn(%d) not open!\n", ai & AI_CHNID_MASK); \
        return HI_ERR_AI_INVALID_PARA;                           \
    }                                                            \
} while (0)

#define CHECK_AI_SAMPLERATE(sample_rate) do {                       \
    switch (sample_rate) {                                          \
        case HI_SAMPLE_RATE_8K:                                     \
        case HI_SAMPLE_RATE_11K:                                    \
        case HI_SAMPLE_RATE_12K:                                    \
        case HI_SAMPLE_RATE_16K:                                    \
        case HI_SAMPLE_RATE_22K:                                    \
        case HI_SAMPLE_RATE_24K:                                    \
        case HI_SAMPLE_RATE_32K:                                    \
        case HI_SAMPLE_RATE_44K:                                    \
        case HI_SAMPLE_RATE_48K:                                    \
        case HI_SAMPLE_RATE_88K:                                    \
        case HI_SAMPLE_RATE_96K:                                    \
        case HI_SAMPLE_RATE_176K:                                   \
        case HI_SAMPLE_RATE_192K:                                   \
            break;                                                  \
        default:                                                    \
            HI_WARN_AI("invalid out sample rate %d\n", sample_rate);\
            return HI_ERR_AI_INVALID_PARA;                          \
    }                                                               \
} while (0)

#define CHECK_AI_BCLKDIV(bclk_div) do {                    \
    switch (bclk_div) {                                    \
        case HI_I2S_BCLK_1_DIV:                            \
        case HI_I2S_BCLK_2_DIV:                            \
        case HI_I2S_BCLK_3_DIV:                            \
        case HI_I2S_BCLK_4_DIV:                            \
        case HI_I2S_BCLK_6_DIV:                            \
        case HI_I2S_BCLK_8_DIV:                            \
        case HI_I2S_BCLK_12_DIV:                           \
        case HI_I2S_BCLK_24_DIV:                           \
        case HI_I2S_BCLK_32_DIV:                           \
        case HI_I2S_BCLK_48_DIV:                           \
        case HI_I2S_BCLK_64_DIV:                           \
            break;                                         \
        default:                                           \
            HI_WARN_AI("invalid bclk_div %d\n", bclk_div); \
            return HI_ERR_AI_INVALID_PARA;                 \
    }                                                      \
} while (0)

#define CHECK_AI_MCLKDIV(mclk_sel) do {                   \
    switch (mclk_sel) {                                   \
        case HI_I2S_MCLK_128_FS:                          \
        case HI_I2S_MCLK_256_FS:                          \
        case HI_I2S_MCLK_384_FS:                          \
        case HI_I2S_MCLK_512_FS:                          \
        case HI_I2S_MCLK_768_FS:                          \
        case HI_I2S_MCLK_1024_FS:                         \
            break;                                        \
        default:                                          \
            HI_WARN_AI("invalid mclk sel %d\n", mclk_sel);\
            return HI_ERR_AI_INVALID_PARA;                \
    }                                                     \
} while (0)

#define CHECK_AI_CHN(chn) do {                   \
    switch (chn) {                               \
        case HI_I2S_CH_1:                        \
        case HI_I2S_CH_2:                        \
        case HI_I2S_CH_8:                        \
            break;                               \
        default:                                 \
            HI_WARN_AI("invalid chn %d\n", chn); \
            return HI_ERR_AI_INVALID_PARA;       \
    }                                            \
} while (0)

#define CHECK_AI_BITDEPTH(bit_depth) do {                    \
    switch (bit_depth) {                                     \
        case HI_I2S_BIT_DEPTH_16:                            \
        case HI_I2S_BIT_DEPTH_24:                            \
            break;                                           \
        default:                                             \
            HI_WARN_AI("invalid bit_depth %d\n", bit_depth); \
            return HI_ERR_AI_INVALID_PARA;                   \
    }                                                        \
} while (0)

#define CHECK_AI_PCMDELAY(pcm_delay_cycle) do {                          \
    switch (pcm_delay_cycle) {                                           \
        case HI_I2S_PCM_0_DELAY:                                         \
        case HI_I2S_PCM_1_DELAY:                                         \
        case HI_I2S_PCM_8_DELAY:                                         \
        case HI_I2S_PCM_16_DELAY:                                        \
        case HI_I2S_PCM_32_DELAY:                                        \
            break;                                                       \
        default:                                                         \
            HI_WARN_AI("invalid pcm_delay_cycle %d\n", pcm_delay_cycle); \
            return HI_ERR_AI_INVALID_PARA;                               \
    }                                                                    \
} while (0)

#define check_ai_hdmi_data_format(hdmi_data_format) do {                  \
    switch (hdmi_data_format) {                                           \
        case HI_AI_HDMI_FORMAT_LPCM:                                      \
        case HI_AI_HDMI_FORMAT_LBR:                                       \
        case HI_AI_HDMI_FORMAT_HBR:                                       \
            break;                                                        \
        default:                                                          \
            HI_ERR_AI("invalid hdmi data_format %d\n", hdmi_data_format); \
            return HI_ERR_AI_INVALID_PARA;                                \
    }                                                                     \
} while (0)

typedef struct {
    hi_u32 read;
    hi_u32 write;
    hi_u32 size;
    hi_s32 map_fd;
    hi_u64 phy_base_addr;
    hi_u64 user_vir_addr;
    hi_u64 kernel_vir_addr;
} ai_buf_attr;

/* for alsa to create AI */
typedef struct {
    hi_handle ai;
    hi_ai_port ai_port;
    hi_ai_attr attr;

    hi_bool alsa;
    hi_void *alsa_para;
} ai_drv_create_param;

typedef struct {
    hi_u32 aqc_try_cnt;
    hi_u32 aqc_cnt;
    hi_u32 rel_try_cnt;
    hi_u32 rel_cnt;
    hi_u32 data_type;
} ai_proc_info;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


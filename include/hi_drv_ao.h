/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao driver head file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_DRV_AO_H__
#define __HI_DRV_AO_H__

#include "hi_ao_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AO_MAX_VIRTUAL_TRACK_NUM 6
#define AO_MAX_REAL_TRACK_NUM 8
#define AO_MAX_TOTAL_TRACK_NUM AO_MAX_REAL_TRACK_NUM

#define AO_MAX_CAST_NUM 4

#define AO_MIN_LINEARVOLUME 0
#define AO_MAX_LINEARVOLUME 100
#define AO_MAX_ABSOLUTEVOLUME 0                 /* max 0 dB */
#define AO_MIN_ABSOLUTEVOLUME (-70)             /* min -70 dB */
#define AO_MAX_ABSOLUTEVOLUMEEXT 18             /* max 18 dB for S5 */
#define AO_MIN_ABSOLUTEVOLUMEEXT (-81)          /* min -81 dB for S5 */
#define AO_MIN_DECIMALVOLUME_AMPLIFICATION 1000 /* 0.125dB is 125 */
#define AO_MIN_BALANCE (-50)
#define AO_MAX_BALANCE 50
#define AO_MAX_ADJUSTSPEED 100
#define AO_MAX_DELAYMS 250

#define AO_PCM_DF_UNSTALL_THD_FRAMENUM 3
#define AO_PCM_MAX_UNSTALL_THD_FRAMENUM 10

#define HI_ID_MASTER_SLAVE_TRACK 0x00
#define HI_ID_LOWLATENCY_TRACK 0x01
#define HI_ID_RENDER_TRACK 0x02
#define HI_ID_CAST 0x01
#define HI_ID_AEF 0x02
#define AO_TRACK_CHNID_MASK 0xff
#define AO_CAST_CHNID_MASK 0xff
#define AO_AEF_CHNID_MASK 0xff
#define AO_COMM_PATH_MAX_LENGTH 1024

#define AO_TRACK_AIP_START_LATENCYMS 50
#define PEQ_BANDNUM_MAX 10
#define PEQ_BANDNUM_MIN 1
#define PEQ_FREQ_HP_MIN 20
#define PEQ_FREQ_HP_MAX 4000
#define PEQ_FREQ_LP_MIN 50
#define PEQ_FREQ_LP_MAX 22000
#define PEQ_FREQ_LS_MIN 20
#define PEQ_FREQ_LS_MAX 4000
#define PEQ_FREQ_HS_MIN 4000
#define PEQ_FREQ_HS_MAX 22000
#define PEQ_FREQ_PK_MIN 20
#define PEQ_FREQ_PK_MAX 22000

#define PEQ_Q_LP_MIN 7
#define PEQ_Q_HP_MIN 7
#define PEQ_Q_LP_MAX 7
#define PEQ_Q_HP_MAX 7
#define PEQ_Q_LS_MIN 7
#define PEQ_Q_LS_MAX 10
#define PEQ_Q_HS_MIN 7
#define PEQ_Q_HS_MAX 10
#define PEQ_Q_PK_MIN 5
#define PEQ_Q_PK_MAX 100

#define PEQ_GAIN_MIN (-15000)
#define PEQ_GAIN_MAX 15000

#define AO_CHECK_PEQQ(q, qmin, qmax)              \
    do {                                          \
        if (q < qmin || q > qmax) {               \
            HI_ERR_AO("invalid Q value %d\n", q); \
            return HI_ERR_AO_INVALID_PARA;        \
        }                                         \
    } while (0)

#define AO_CHECK_PEQFREQ(freq, freqmin, freqmax)            \
    do {                                                    \
        if (freq < freqmin || freq > freqmax) {             \
            HI_ERR_AO("invalid peq freq value %d\n", freq); \
            return HI_ERR_AO_INVALID_PARA;                  \
        }                                                   \
    } while (0)

#define AO_CHECK_PEQGAIN(gain, gainmin, gainmax)              \
    do {                                                      \
        if (gain < gainmin || gain > gainmax || gain % 125) { \
            HI_ERR_AO("invalid peq gain value %d\n", gain);   \
            return HI_ERR_AO_INVALID_PARA;                    \
        }                                                     \
    } while (0)

#define CHECK_AO_SNDCARD_OPEN(sound)                          \
    do {                                                      \
        ao_mgmt *_mgmt = ao_get_mgmt(); \
        CHECK_AO_SNDCARD(sound);                              \
        if (HI_NULL == _mgmt->snd_entity[sound].card) { \
            HI_WARN_AO(" invalid snd id %d\n", sound);        \
            return HI_ERR_AO_SOUND_NOT_OPEN;                  \
        }                                                     \
    } while (0)

#define CHECK_AO_TRACK_ID(track)                                     \
    do {                                                             \
        if ((track & 0xffff0000) != (HI_ID_AO << 16)) {              \
            HI_ERR_AO("track(0x%x) is not ao handle!\n", track);     \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
        if (((track & 0xff00) != (HI_ID_MASTER_SLAVE_TRACK << 8)) && \
            ((track & 0xf000) != (HI_ID_RENDER_TRACK << 12)) &&      \
            ((track & 0xff00) != (HI_ID_LOWLATENCY_TRACK << 8))) {   \
            HI_ERR_AO("track(0x%x) is not track handle!\n", track);  \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
    } while (0)

#define CHECK_AO_MASTER_SLAVE_TRACK_ID(track)                        \
    do {                                                             \
        if ((track & 0xffff0000) != (HI_ID_AO << 16)) {              \
            HI_ERR_AO("track(0x%x) is not ao handle!\n", track);     \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
        if (((track & 0xff00) != (HI_ID_MASTER_SLAVE_TRACK << 8))) { \
            HI_ERR_AO("track(0x%x) is not track handle!\n", track);  \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
    } while (0)

#define CHECK_AO_TRACK_OPEN(track)                                                                   \
    do {                                                                                             \
        ao_mgmt *_mgmt = ao_get_mgmt(); \
        CHECK_AO_TRACK_ID(track);                                                                    \
        if (0 == osal_atomic_read(&_mgmt->track_entity[track & AO_TRACK_CHNID_MASK].atm_use_cnt)) { \
            HI_WARN_AO(" invalid track id 0x%x\n", track);                                           \
            return HI_ERR_AO_INVALID_PARA;                                                           \
        }                                                                                            \
    } while (0)

#define CHECK_AO_AEF_HANDLE(h_aef)                              \
    do {                                                        \
        if ((h_aef & 0xffff0000) != (HI_ID_AO << 16)) {         \
            HI_ERR_AO("aef(0x%x) is not ao handle!\n", h_aef);  \
            return HI_ERR_AO_INVALID_PARA;                      \
        }                                                       \
        if ((h_aef & 0xff00) != (HI_ID_AEF << 8)) {             \
            HI_ERR_AO("aef(0x%x) is not aef handle!\n", h_aef); \
            return HI_ERR_AO_INVALID_PARA;                      \
        }                                                       \
    } while (0)

#define CHECK_AO_NULL_PTR(p)              \
    do {                                  \
        if (HI_NULL == p) {               \
            HI_ERR_AO("NULL pointer\n"); \
            return HI_ERR_AO_NULL_PTR;    \
        }                                 \
    } while (0)

#define CHECK_AO_CREATE(state)                    \
    do {                                          \
        if (0 > state) {                          \
            HI_WARN_AO("AO device not open!\n"); \
            return HI_ERR_AO_DEV_NOT_OPEN;        \
        }                                         \
    } while (0)

#define CHECK_AO_SNDCARD(card)                        \
    do {                                              \
        if (AO_SND_MAX <= card) {                \
            HI_WARN_AO(" invalid snd id %d\n", card); \
            return HI_ERR_AO_INVALID_ID;              \
        }                                             \
    } while (0)
/* master & slave only */
#define CHECK_AO_TRACK(track)                                          \
    do {                                                               \
        if (AO_MAX_TOTAL_TRACK_NUM <= (track & AO_TRACK_CHNID_MASK)) { \
            HI_WARN_AO(" invalid snd track 0x%x\n", track);            \
            return HI_ERR_AO_INVALID_PARA;                             \
        }                                                              \
    } while (0)

#define CHECK_AO_CAST(cast)                                   \
    do {                                                      \
        if (AO_MAX_CAST_NUM <= (cast & AO_CAST_CHNID_MASK)) { \
            HI_WARN_AO(" invalid snd cast 0x%x\n", cast);     \
            return HI_ERR_AO_INVALID_PARA;                    \
        }                                                     \
    } while (0)

#define CHECK_AO_CAST_HANDLE(h_cast)                                 \
    do {                                                             \
        if ((h_cast & 0xffff0000) != (HI_ID_AO << 16)) {             \
            HI_ERR_AO("h_cast(0x%x) is not ao handle!\n", h_cast);   \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
        if ((h_cast & 0xff00) != (HI_ID_CAST << 8)) {                \
            HI_ERR_AO("h_cast(0x%x) is not cast handle!\n", h_cast); \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
        if (AO_MAX_CAST_NUM <= (h_cast & AO_CAST_CHNID_MASK)) {      \
            HI_ERR_AO("invalid snd cast handle(0x%x)\n", h_cast);    \
            return HI_ERR_AO_INVALID_PARA;                           \
        }                                                            \
    } while (0)

#define CHECK_AO_PORTNUM(num)                                \
    do {                                                     \
        if ((num) > HI_AO_PORT_MAX || (num) <= 0) {               \
            HI_WARN_AO(" invalid outport number %d\n", (num)); \
            return HI_ERR_AO_INVALID_PARA;                   \
        }                                                    \
    } while (0)

#define CHECK_AO_OUTPORT(port) \
    do { \
        if (((port) > HI_AO_PORT_ARC1) && ((port) != HI_AO_PORT_ALL)) { \
            HI_WARN_AO("invalid outport 0x%x\n", (port)); \
            return HI_ERR_AO_INVALID_PARA; \
        } \
    } while (0)

#define CHECK_AO_OUTPUT_MODE(mode)                         \
    do {                                                \
        if (mode >= HI_AO_OUTPUT_MODE_MAX) {        \
            HI_WARN_AO("invalid output mode %d\n", mode); \
            return HI_ERR_AO_INVALID_PARA;              \
        }                                               \
    } while (0)

#define CHECK_AO_TRACKMODE(mode)                         \
    do {                                                 \
        if (mode >= HI_TRACK_MODE_MAX) {            \
            HI_WARN_AO(" invalid track mode %d\n", mode); \
            return HI_ERR_AO_INVALID_PARA;               \
        }                                                \
    } while (0)

#define CHECK_AO_HDMI_MODE(mode)                         \
    do {                                                \
        if (mode >= HI_AO_OUTPUT_MODE_MAX) {        \
            HI_WARN_AO("invalid hdmi mode %d\n", mode); \
            return HI_ERR_AO_INVALID_PARA;              \
        }                                               \
    } while (0)

#define CHECK_AO_SPDIF_MODE(mode)                         \
    do {                                                 \
        if (mode != HI_AO_OUTPUT_MODE_LPCM && \
            mode != HI_AO_OUTPUT_MODE_RAW) {   \
            HI_WARN_AO(" invalid spdif mode %d\n", mode); \
            return HI_ERR_AO_INVALID_PARA;               \
        }                                                \
    } while (0)

#ifdef __DPT__
#define CHECK_AO_ARCMODE(mode)                         \
    do {                                               \
        if (mode >= HI_AO_OUTPUT_MODE_MAX) {  \
            HI_WARN_AO("invalid arc mode %d\n", mode); \
            return HI_ERR_AO_INVALID_PARA;             \
        }                                              \
    } while (0)
#endif

#define CHECK_AO_SPDIFSCMSMODE(mode)                         \
    do {                                                         \
        if (mode >= HI_AO_SPDIF_SCMS_MODE_MAX) {        \
            HI_WARN_AO(" invalid spdif scms mode %d\n", mode); \
            return HI_ERR_AO_INVALID_PARA;                       \
        }                                                        \
    } while (0)

#define CHECK_AO_CATEGORYCODE(categorycode)                               \
    do {                                                                  \
        switch (categorycode) {                                           \
            case HI_AO_SPDIF_CATEGORY_GENERAL:                       \
            case HI_AO_SPDIF_CATEGORY_BROADCAST_JP:                  \
            case HI_AO_SPDIF_CATEGORY_BROADCAST_USA:                 \
            case HI_AO_SPDIF_CATEGORY_BROADCAST_EU:                  \
            case HI_AO_SPDIF_CATEGORY_PCM_CODEC:                     \
            case HI_AO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER:            \
            case HI_AO_SPDIF_CATEGORY_DIGITAL_MIXER:                 \
            case HI_AO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR:          \
            case HI_AO_SPDIF_CATEGORY_SRC:                           \
            case HI_AO_SPDIF_CATEGORY_MD:                            \
            case HI_AO_SPDIF_CATEGORY_DVD:                           \
            case HI_AO_SPDIF_CATEGORY_SYNTHESISER:                   \
            case HI_AO_SPDIF_CATEGORY_MIC:                           \
            case HI_AO_SPDIF_CATEGORY_DAT:                           \
            case HI_AO_SPDIF_CATEGORY_DCC:                           \
            case HI_AO_SPDIF_CATEGORY_VCR:                           \
                break;                                                    \
            default:                                                      \
                HI_WARN_AO("invalid category code 0x%x\n", categorycode); \
                return HI_ERR_AO_INVALID_PARA;                            \
        }                                                                 \
    } while (0)

#define CHECK_AO_FRAME_NOSTANDART_SAMPLERATE(inrate)                              \
    do {                                                                          \
        if (inrate > HI_SAMPLE_RATE_192K || inrate < HI_SAMPLE_RATE_8K) { \
            HI_INFO_AO("don't support this insamplerate(%d)\n", inrate);          \
            return HI_SUCCESS;                                                    \
        }                                                                         \
    } while (0)

#define CHECK_AO_FRAME_SAMPLERATE(inrate)                                    \
    do {                                                                     \
        switch (inrate) {                                                    \
            case HI_SAMPLE_RATE_8K:                                      \
            case HI_SAMPLE_RATE_11K:                                     \
            case HI_SAMPLE_RATE_12K:                                     \
            case HI_SAMPLE_RATE_16K:                                     \
            case HI_SAMPLE_RATE_22K:                                     \
            case HI_SAMPLE_RATE_24K:                                     \
            case HI_SAMPLE_RATE_32K:                                     \
            case HI_SAMPLE_RATE_44K:                                     \
            case HI_SAMPLE_RATE_48K:                                     \
            case HI_SAMPLE_RATE_88K:                                     \
            case HI_SAMPLE_RATE_96K:                                     \
            case HI_SAMPLE_RATE_176K:                                    \
            case HI_SAMPLE_RATE_192K:                                    \
                break;                                                       \
            default:                                                         \
                HI_INFO_AO("don't support this insamplerate(%d)\n", inrate); \
                return HI_SUCCESS;                                           \
        }                                                                    \
    } while (0)

#define CHECK_AO_SAMPLERATE(outrate)                                 \
    do {                                                             \
        switch (outrate) {                                           \
            case HI_SAMPLE_RATE_8K:                              \
            case HI_SAMPLE_RATE_11K:                             \
            case HI_SAMPLE_RATE_12K:                             \
            case HI_SAMPLE_RATE_16K:                             \
            case HI_SAMPLE_RATE_22K:                             \
            case HI_SAMPLE_RATE_24K:                             \
            case HI_SAMPLE_RATE_32K:                             \
            case HI_SAMPLE_RATE_44K:                             \
            case HI_SAMPLE_RATE_48K:                             \
            case HI_SAMPLE_RATE_88K:                             \
            case HI_SAMPLE_RATE_96K:                             \
            case HI_SAMPLE_RATE_176K:                            \
            case HI_SAMPLE_RATE_192K:                            \
                break;                                               \
            default:                                                 \
                HI_WARN_AO("invalid sample out rate %d\n", outrate); \
                return HI_ERR_AO_INVALID_PARA;                       \
        }                                                            \
    } while (0)

#define CHECK_AO_FRAME_BITDEPTH(inbitdepth)                                   \
    do {                                                                      \
        switch (inbitdepth) {                                                 \
            case HI_BIT_DEPTH_24:                                         \
            case HI_BIT_DEPTH_16:                                         \
            case HI_BIT_DEPTH_8:                                          \
                break;                                                        \
            default:                                                          \
                HI_INFO_AO("don't support this bit depth(%d)\n", inbitdepth); \
                return HI_SUCCESS;                                            \
        }                                                                     \
    } while (0)

#define CHECK_AO_LINEARVOLUME(linvolume)                                                                                     \
    do {                                                                                                                     \
        if ((linvolume < AO_MIN_LINEARVOLUME) || (linvolume > AO_MAX_LINEARVOLUME)) {                                        \
            HI_WARN_AO("invalid linear_volume(%d), min(%d) max(%d)\n", linvolume, AO_MIN_LINEARVOLUME, AO_MAX_LINEARVOLUME); \
            return HI_ERR_AO_INVALID_PARA;                                                                                   \
        }                                                                                                                    \
    } while (0)

#define CHECK_AO_ABSLUTEVOLUME(absvolume)                                                                                            \
    do {                                                                                                                             \
        if ((absvolume < AO_MIN_ABSOLUTEVOLUME) || (absvolume > AO_MAX_ABSOLUTEVOLUME)) {                                            \
            HI_WARN_AO("invalid absoulute_volume(%d), min(%d), max(%d)\n", absvolume, AO_MIN_ABSOLUTEVOLUME, AO_MAX_ABSOLUTEVOLUME); \
            return HI_ERR_AO_INVALID_PARA;                                                                                           \
        }                                                                                                                            \
    } while (0)

#define CHECK_AO_ABSLUTEPRECIVOLUME(absintvolume, absdecvolume)                                                                                                                            \
    do {                                                                                                                                                                                   \
        if ((absintvolume > 0 && absdecvolume < 0) || (absintvolume < 0 && absdecvolume > 0)) {                                                                                            \
            HI_WARN_AO("invalid precision volume decimal\n");                                                                                                                              \
            return HI_ERR_AO_INVALID_PARA;                                                                                                                                                 \
        }                                                                                                                                                                                  \
        if ((absdecvolume < -AO_MIN_DECIMALVOLUME_AMPLIFICATION) || (absdecvolume > AO_MIN_DECIMALVOLUME_AMPLIFICATION) || (0 != absdecvolume % 125)) {                                    \
            HI_WARN_AO("invalid precision volume decimal part(%d), min(%d), max(%d), step(125)\n", absdecvolume, -AO_MIN_DECIMALVOLUME_AMPLIFICATION, AO_MIN_DECIMALVOLUME_AMPLIFICATION); \
            return HI_ERR_AO_INVALID_PARA;                                                                                                                                                 \
        }                                                                                                                                                                                  \
        if ((absintvolume < AO_MIN_ABSOLUTEVOLUMEEXT) || (absintvolume > AO_MAX_ABSOLUTEVOLUMEEXT)) {                                                                                      \
            HI_WARN_AO("invalid precision volume, min(%d), max(%d)\n", AO_MIN_ABSOLUTEVOLUMEEXT, AO_MAX_ABSOLUTEVOLUMEEXT);                                                                \
            return HI_ERR_AO_INVALID_PARA;                                                                                                                                                 \
        }                                                                                                                                                                                  \
        if (absdecvolume < 0) {                                                                                                                                                            \
            if (absintvolume - 1 < AO_MIN_ABSOLUTEVOLUMEEXT) {                                                                                                                             \
                HI_WARN_AO("invalid precision volume, min(%d)\n", AO_MIN_ABSOLUTEVOLUMEEXT);                                                                                               \
                return HI_ERR_AO_INVALID_PARA;                                                                                                                                             \
            }                                                                                                                                                                              \
        }                                                                                                                                                                                  \
        if (absdecvolume > 0) {                                                                                                                                                            \
            if (absintvolume + 1 > AO_MAX_ABSOLUTEVOLUMEEXT) {                                                                                                                             \
                HI_WARN_AO("invalid precision volume, max(%d)\n", AO_MAX_ABSOLUTEVOLUMEEXT);                                                                                               \
                return HI_ERR_AO_INVALID_PARA;                                                                                                                                             \
            }                                                                                                                                                                              \
        }                                                                                                                                                                                  \
    } while (0)

#define CHECK_AO_ABSLUTEVOLUMEEXT(absvolume)                                                                                               \
    do {                                                                                                                                   \
        if ((absvolume < AO_MIN_ABSOLUTEVOLUMEEXT) || (absvolume > AO_MAX_ABSOLUTEVOLUMEEXT)) {                                            \
            HI_WARN_AO("invalid absoulute_volume(%d), min(%d), max(%d)\n", absvolume, AO_MIN_ABSOLUTEVOLUMEEXT, AO_MAX_ABSOLUTEVOLUMEEXT); \
            return HI_ERR_AO_INVALID_PARA;                                                                                                 \
        }                                                                                                                                  \
    } while (0)

#define CHECK_AO_BALANCE(balance)                                                                           \
    do {                                                                                                    \
        if ((balance < AO_MIN_BALANCE) || (balance > AO_MAX_BALANCE)) {                                     \
            HI_WARN_AO("invalid balance(%d), min(%d), max(%d)\n", balance, AO_MIN_BALANCE, AO_MAX_BALANCE); \
            return HI_ERR_AO_INVALID_PARA;                                                                  \
        }                                                                                                   \
    } while (0)

#define CHECK_AO_SPEEDADJUST(speed)                                                                                        \
    do {                                                                                                                   \
        if ((-AO_MAX_ADJUSTSPEED > speed)               \
            || (speed > AO_MAX_ADJUSTSPEED)) {                                                                      \
            HI_WARN_AO("invalid AO speed_adjust(%d) min(%d), max(%d)!\n", speed, -AO_MAX_ADJUSTSPEED, AO_MAX_ADJUSTSPEED); \
            return HI_ERR_AO_INVALID_PARA;                                                                                 \
        }                                                                                                                  \
    } while (0)

#define CHECK_AO_DRCATTR(drc_attr)                                                                             \
    do {                                                                                                       \
        if ((drc_attr->attack_time < 20) || (drc_attr->attack_time > 2000)) {                                  \
            HI_ERR_AO("invalid drc attack_time! attack_time(%d)!\n", drc_attr->attack_time);                   \
            return HI_ERR_AO_INVALID_PARA;                                                                     \
        }                                                                                                      \
        if ((drc_attr->release_time < 20) || (drc_attr->release_time > 2000)) {                                \
            HI_ERR_AO("invalid drc release_time! release_time(%d)!\n", drc_attr->release_time);                \
            return HI_ERR_AO_INVALID_PARA;                                                                     \
        }                                                                                                      \
        if (drc_attr->threshold >= drc_attr->limit) {                                                              \
            HI_ERR_AO("thrhd must less than limit! thrhd(%d),limit(%d)!\n", drc_attr->threshold, drc_attr->limit); \
            return HI_ERR_AO_INVALID_PARA;                                                                     \
        }                                                                                                      \
        if ((drc_attr->threshold < -80) || (drc_attr->threshold > -2)) {                                               \
            HI_ERR_AO("invalid drc thrhd! thrhd(%d)!\n", drc_attr->threshold);                                     \
            return HI_ERR_AO_INVALID_PARA;                                                                     \
        }                                                                                                      \
        if ((drc_attr->limit < -79) || (drc_attr->limit > -1)) {                                               \
            HI_ERR_AO("invalid drc limit! limit(%d)!\n", drc_attr->limit);                                     \
            return HI_ERR_AO_INVALID_PARA;                                                                     \
        }                                                                                                      \
    } while (0)

#define HI_FATAL_AO(fmt...) HI_FATAL_PRINT(HI_ID_AO, fmt)

#define HI_ERR_AO(fmt...) HI_ERR_PRINT(HI_ID_AO, fmt)

#define HI_WARN_AO(fmt...) HI_WARN_PRINT(HI_ID_AO, fmt)

#define HI_INFO_AO(fmt...) HI_INFO_PRINT(HI_ID_AO, fmt)

#define HI_FATAL_AIAO(fmt...) HI_FATAL_PRINT(HI_ID_AIAO, fmt)
#define HI_ERR_AIAO(fmt...) HI_ERR_PRINT(HI_ID_AIAO, fmt)
#define HI_WARN_AIAO(fmt...) HI_WARN_PRINT(HI_ID_AIAO, fmt)
#define HI_INFO_AIAO(fmt...) HI_INFO_PRINT(HI_ID_AIAO, fmt)

typedef struct {
    hi_u32 id;
    hi_ao_aef_type type;
    hi_char name[32];
    hi_char description[32];
    hi_bool enable;
} ao_aef_proc_item;

/* the type of debug audio */
typedef enum {
    AO_SND_DEBUG_TYPE_AEF,
    AO_SND_DEBUG_TYPE_BUTT
} ao_snd_debug_type;

typedef struct {
    hi_u64 phy_setting_addr;
    hi_u32 setting_size;
} aef_debug_attr;

typedef struct {
    ao_snd_debug_type debug_type;

    union {
        aef_debug_attr debug_attr;
    } un_debug_attr;
} ao_debug_attr;

typedef struct {
    hi_u64 phy_in_buf_addr;
    hi_u64 phy_out_buf_addr;
    hi_u32 in_buf_size;
    hi_u32 out_buf_size;
    hi_u32 in_buf_frame_size;
    hi_u32 out_buf_frame_size;
    hi_u32 in_buf_frame_num;  /* 2/4/8 */
    hi_u32 out_buf_frame_num; /* 2/4/8 */
    hi_u64 phy_engine_addr;
    aef_debug_attr aef_debug_addr;
} ao_aef_buf_attr;

typedef struct {
    hi_bool support;
    hi_u32 capability;
    hi_bool passthru_bypass;
    hi_char a_comm_path[AO_COMM_PATH_MAX_LENGTH];
    hi_bool continue_status;
    hi_ao_output_latency output_mode;
    struct file *con_output_fp;
} ao_render_attr;

/* audio output attribute */
/* CNcomment:音频淡入淡出控制 */
typedef struct {
    hi_bool enable;
    hi_u32 fade_in; /* fade in time(unit:ms) */   /* CNcomment:淡入时间(单位: ms) */
    hi_u32 fade_out; /* fade out time(unit:ms) */ /* CNcomment:淡出时间(单位: ms) */
} ao_track_fade;

typedef struct {
    hi_u32 frame_delay;   /* input:  the frame delay of aef */
    hi_bool mc_supported; /* input:  indicate whether support mc process of aef */
} ao_aef_attr;

typedef struct {
    hi_u32 buf_phy_addr;
    hi_u64 buf_vir_addr;
    hi_u32 buf_size;
} ao_buf_attr;

typedef struct {
    /* bit_depth: (PCM) data depth, and format of storing the output data
     * if the data depth is 16 bits, 16-bit word memory is used.
     * if the data depth is greater than 16 bits, 32-bit word memory is used,
     * and data is stored as left-aligned data. that is, the valid data is at upper bits.
     */
    /* CNcomment: bit_depth: (PCM) 数据位宽设置. 输出存放格式
     * 等于16bit: 占用16bit word内存
     * 大于16bit: 占用32bit word内存, 数据左对齐方式存放(有效数据在高位)
     */
    hi_s32 bit_depth;

    /* whether the data is interleaved */
    /* CNcomment: 数据是否交织 */
    hi_bool interleaved;

    /* sampling rate */
    /* CNcomment: 采样率 */
    hi_u32 sample_rate;

    /* number of audio channels */
    /* CNcomment: 声道 */
    hi_u32 channels;

    /* presentation time stamp (PTS, unit: us) */
    /* CNcomment: 时间戳(单位：微秒) */
    hi_s64 pts;

    /* pointer to the buffer for storing the pulse code modulation (PCM) data */
    /* CNcomment: PCM数据缓冲指针       */
    hi_u64 pcm_buffer;

    /* pointer to the buffer for storing the stream data */
    /* CNcomment: 透传数据缓冲指针 */
    hi_u64 bits_buffer;

    /* number of sampling points of the PCM data */
    /* CNcomment: PCM数据采样点数 */
    hi_u32 pcm_samples;

    /* IEC61937 data size */
    /* CNcomment: IEC61937数据长度 */
    hi_u32 bits_bytes;

    /* frame ID */
    /* CNcomment: 帧序号 */
    hi_u32 frame_index;

    /* IEC61937 data type */
    /* CNcomment: IEC61937数据类型标识，低8bit为IEC数据类型 */
    hi_u32 iec_data_type;
} ao_frame;

typedef struct {
    hi_ao_port out_port;
    hi_u64 phy_dma;
    hi_u64 phy_wptr;
    hi_u64 phy_rptr;
    hi_u32 size;
} snd_port_kernel_attr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

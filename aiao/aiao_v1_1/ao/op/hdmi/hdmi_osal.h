/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port hdmi include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HDMI_OSAL_H__
#define __HDMI_OSAL_H__

#include "audio_util.h"
#include "drv_hdmi_module_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum ao_hdmi_event {
    AO_HDMI_BEFORE_OE_DISABLE = 1,
    AO_HDMI_AFTER_OE_DISABLE,
    AO_HDMI_OE_ENABLE,
    AO_HDMI_HOT_PLUG_IN,
    AO_HDMI_HOT_PLUG_OUT,
    AO_HDMI_EVENT_NULL,
};

#define AO_HDMI_HPD_PLUGIN  1

#define MAX_SAD_AUDIO_CNT    (0x9 + 0xe)

/* Short Audio Descriptors.see CTA-861-G 7.5.2 Audio Data Block */
struct sad_fmt_audio {
    hi_u8   fmt_code;       /* Audio Format Code */
    hi_u8   ext_code;       /* Audio Coding Extension Type Code */

    /* Max Number of channels.Audio Format Code 1 to 14;
     * Audio Coding Extension Type Codes 4 to 6,8 to 10;
     * Audio Coding Extension Type 0x0D (L-PCM 3D Audio), bits MC4:MC0
     */
    hi_u32  max_channel;

    hi_bool samp_32k;       /* 32   kHz */
    hi_bool samp_44p1k;     /* 44.1 kHz */
    hi_bool samp_48k;       /* 48   kHz */
    hi_bool samp_88p2k;     /* 88.2 kHz */
    hi_bool samp_96k;       /* 96   kHz */
    hi_bool samp_176p4k;    /* 176.4kHz */
    hi_bool samp_192k;      /* 192 kHz */

    /* 16bit.Only to Audio Format Code = 1 (L-PCM) & Audio Extension Type Code 13 (L-PCM 3D Audio) */
    hi_bool width_16;

    /* 20bit.Only to Audio Format Code = 1 (L-PCM) & Audio Extension Type Code 13 (L-PCM 3D Audio) */
    hi_bool width_20;

    /* 24bit.Only to Audio Format Code = 1 (L-PCM) & Audio Extension Type Code 13 (L-PCM 3D Audio) */
    hi_bool width_24;

    hi_u32  max_bit_rate;   /* Maximum bit rate in Hz.Only to Audio Format Codes 2 to 8 */
    hi_u8   dependent;      /* Audio Format Code dependent value.Only to Audio Format Codes 9 to 13 */
    hi_u8   profile;        /* Profile.Only to Audio Format Codes 14 (WMA pro) */
    hi_bool len_1024_tl;    /* 1024_TL.AAC audio frame lengths 1024_TL.Only to extension Type Codes 4 to 6 */
    hi_bool len_960_tl;     /* 960_TL. AAC audio frame lengths 960_TL.Only to extension Type Codes 4 to 6 */
    hi_bool mps_l;          /* MPS_L. Only to Extension Type Codes 8 to 10 */
};

struct audio_property {
    /*
     * @basic :basic audio support.
     * Basic Audio¡ª Uncompressed, two channel, digital audio.
     * e.g., 2 channel IEC 60958-3 [12] L-PCM, 32, 44.1, and 48 kHz sampling rates, 16 bits/sample.
     */
    hi_bool basic;

    /*
     * @sad_count:Short Audio Descriptors audio format support total number.
     */
    hi_u32  sad_count;

    /*
     * @sad :see Short Audio Descriptors.
     */
    struct sad_fmt_audio sad[MAX_SAD_AUDIO_CNT];
};

typedef struct {
    hi_u32 hdmi_id;
    hi_bool hdmi_plug_in;

    struct hi_ao_intf_module_ops *hdmi_func;

    osal_mutex lock;
    struct notifier_block nb;

    hdmi_ao_attr ao_attr;
    struct audio_property eld;
} hdmi_osal_context;

hi_s32  hdmi_osal_get_eld(hdmi_osal_context *ctx);
hi_void hdmi_osal_set_mute(hdmi_osal_context *ctx);
hi_void hdmi_osal_set_unmute(hdmi_osal_context *ctx);
hi_s32  hdmi_osal_set_sample_rate(hdmi_osal_context *ctx, hi_u32 sample_rate);
hi_bool hdmi_osal_check_audio_support(hdmi_osal_context *ctx, hi_u32 format);
hi_s32  hdmi_osal_hw_params_validate(hdmi_osal_context *ctx, hdmi_ao_attr *attr);
hi_s32  hdmi_osal_set_hw_params(hdmi_osal_context *ctx, hdmi_ao_attr *attr);
hi_s32  hdmi_osal_init(hi_ao_port hdmi_port, hdmi_osal_context *ctx);
hi_void hdmi_osal_deinit(hdmi_osal_context *ctx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HDMI_OSAL_H__ */


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao hdmi header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "audio_util.h"
#include "hdmi_ch_status.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_void set_mode(hdmi_ch_status *ch_status, hi_u32 mode)
{
    if (mode == LBR || mode == HBR) {
        /* passthrough: audio sample word used for other purpose */
        ch_status->ch_status_0.bits.use_b = 1;
    } else {
        /* PCM: audio sample word is linear pcm sample */
        ch_status->ch_status_0.bits.use_b = 0;
    }
}

static hi_void set_category_code(hdmi_ch_status *ch_status,
    hi_ao_spdif_category_code category)
{
    hi_u32 id;

    struct _config_table {
        hi_u32 category;
        hi_u32 code;
    } config_table[] = {
        {HI_AO_SPDIF_CATEGORY_GENERAL, 0x00},
        {HI_AO_SPDIF_CATEGORY_BROADCAST_JP, 0x04},
        {HI_AO_SPDIF_CATEGORY_BROADCAST_USA, 0x64},
        {HI_AO_SPDIF_CATEGORY_BROADCAST_EU, 0x0C},
        {HI_AO_SPDIF_CATEGORY_PCM_CODEC, 0x02},
        {HI_AO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER, 0x22},
        {HI_AO_SPDIF_CATEGORY_DIGITAL_MIXER, 0x12},
        {HI_AO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR, 0x2A},
        {HI_AO_SPDIF_CATEGORY_SRC, 0x1A},
        {HI_AO_SPDIF_CATEGORY_MD, 0x49},
        {HI_AO_SPDIF_CATEGORY_DVD, 0x19},
        {HI_AO_SPDIF_CATEGORY_SYNTHESISER, 0x05},
        {HI_AO_SPDIF_CATEGORY_MIC, 0x0D},
        {HI_AO_SPDIF_CATEGORY_DAT, 0x03},
        {HI_AO_SPDIF_CATEGORY_DCC, 0x43},
        {HI_AO_SPDIF_CATEGORY_VCR, 0x0B},
    };

    for (id = 0; id < ARRAY_SIZE(config_table); id++) {
        if (config_table[id].category == category) {
            ch_status->ch_status_1.bits.category_code = config_table[id].code;
        }
    }
}

static hi_void get_category_code_type(hi_ao_spdif_category_code category, hi_bool *lbit_flag)
{
    switch (category) {
        case HI_AO_SPDIF_CATEGORY_GENERAL:
        case HI_AO_SPDIF_CATEGORY_PCM_CODEC:
        case HI_AO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER:
        case HI_AO_SPDIF_CATEGORY_DIGITAL_MIXER:
        case HI_AO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR:
        case HI_AO_SPDIF_CATEGORY_SRC:
        case HI_AO_SPDIF_CATEGORY_SYNTHESISER:
        case HI_AO_SPDIF_CATEGORY_MIC:
        case HI_AO_SPDIF_CATEGORY_DAT:
        case HI_AO_SPDIF_CATEGORY_DCC:
        case HI_AO_SPDIF_CATEGORY_VCR:
            *lbit_flag = HI_FALSE;
            break;
        case HI_AO_SPDIF_CATEGORY_BROADCAST_JP:
        case HI_AO_SPDIF_CATEGORY_BROADCAST_USA:
        case HI_AO_SPDIF_CATEGORY_BROADCAST_EU:
        case HI_AO_SPDIF_CATEGORY_MD:
        case HI_AO_SPDIF_CATEGORY_DVD:
            *lbit_flag = HI_TRUE;
            break;
        default:
            break;
    }
}

static hi_void set_category_scms(hdmi_ch_status *ch_status,
    hi_ao_spdif_scms_mode scms_mode, hi_ao_spdif_category_code category_code)
{
    hi_bool lbit_flag = HI_FALSE;

    set_category_code(ch_status, category_code);
    get_category_code_type(category_code, &lbit_flag);

    switch (scms_mode) {
        case HI_AO_SPDIF_SCMS_MODE_COPY_ALLOW:
            ch_status->ch_status_1.u32 &= 0x7f;
            ch_status->ch_status_0.bits.use_c = 1;
            break;

        case HI_AO_SPDIF_SCMS_MODE_COPY_ONCE:
            if (lbit_flag == HI_FALSE) {
                ch_status->ch_status_1.u32 |= ~0x7f;
            } else {
                ch_status->ch_status_1.u32 &= 0x7f;
            }
            ch_status->ch_status_0.bits.use_c = 0;
            break;

        case HI_AO_SPDIF_SCMS_MODE_COPY_NOMORE:
        case HI_AO_SPDIF_SCMS_MODE_COPY_PROHIBITED:
            if (lbit_flag == HI_FALSE) {
                ch_status->ch_status_1.u32 &= 0x7f;
            } else {
                ch_status->ch_status_1.u32 |= ~0x7f;
            }
            ch_status->ch_status_0.bits.use_c = 0;
            break;

        default:
            break;
    }
}

static hi_void set_channel_number(hdmi_ch_status *ch_status, hi_u32 channels, hi_u32 org_channels)
{
    ch_status->ch_status_2.bits.src_nb = channels;
    ch_status->ch_status_2.bits.ch_nb = org_channels;
}

static hi_void set_bit_width(hdmi_ch_status *ch_status, hi_bit_depth bit_depth)
{
    hi_u32 max_sample_bits;
    hi_u32 cur_sample_bits;

    switch (bit_depth) {
        case HI_BIT_DEPTH_16:
            cur_sample_bits = 0x1;
            max_sample_bits = 0;
            break;
        case HI_BIT_DEPTH_24:
            cur_sample_bits = 0x5;
            max_sample_bits = 1;
            break;
        default:
            cur_sample_bits = 0x1;
            max_sample_bits = 0;
            break;
    }

    ch_status->ch_status_4.bits.bit_max = max_sample_bits;
    ch_status->ch_status_4.bits.bit_width = cur_sample_bits;
}

static hi_void set_sample_rate(hdmi_ch_status *ch_status, hi_sample_rate sample_rate)
{
    hi_u32 id;

    struct _config_table {
        hi_sample_rate sample_rate;
        hi_u32 freq;
        hi_u32 org_freq;
    } config_table[] = {
        { HI_SAMPLE_RATE_32K,  0x3, 0xc },
        { HI_SAMPLE_RATE_44K,  0x0, 0xf },
        { HI_SAMPLE_RATE_48K,  0x2, 0xd },
        { HI_SAMPLE_RATE_88K,  0x8, 0x7 },
        { HI_SAMPLE_RATE_96K,  0xa, 0x5 },
        { HI_SAMPLE_RATE_176K, 0xc, 0xf },
        { HI_SAMPLE_RATE_192K, 0xe, 0xd },
    };

    ch_status->ch_status_3.bits.freq = 0x2;
    ch_status->ch_status_4.bits.org_freq = 0xd;

    for (id = 0; id < ARRAY_SIZE(config_table); id++) {
        if (config_table[id].sample_rate == sample_rate) {
            ch_status->ch_status_3.bits.freq = config_table[id].freq;
            ch_status->ch_status_4.bits.org_freq = config_table[id].org_freq;
        }
    }
}

hi_void hdmi_ch_status_init(hdmi_ao_attr *ao_attr, hdmi_ch_status *ch_status)
{
    hi_s32 ret;

    ret = memset_s(ch_status, sizeof(hdmi_ch_status), 0, sizeof(hdmi_ch_status));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return;
    }

    if (ao_attr->audio_format == HI_AUDIO_FORMAT_PCM) {
        set_mode(ch_status, PCM);
    } else {
        set_mode(ch_status, HBR);
    }

    set_category_scms(ch_status,
        HI_AO_SPDIF_SCMS_MODE_COPY_PROHIBITED, HI_AO_SPDIF_CATEGORY_GENERAL);

    set_channel_number(ch_status, ao_attr->channels, ao_attr->org_channels);

    set_sample_rate(ch_status, ao_attr->sample_rate);
    set_bit_width(ch_status, ao_attr->bit_depth);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio util function
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_drv_sys.h"
#include "hi_drv_audio.h"
#include "audio_util.h"

/*
 * HDMI 1.3 above supports high bit rate audio (HBR) stream which rate is more than 6.144_mbps for dolby MAT and
 * DTS-HD master audio. they are shared with 4 I2S inputs.
 */
static hi_u32 g_iec61937_hbr_data_type[] = {
    IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS,

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
    IEC61937_DATATYPE_DTS_TYPE_IV,

    IEC61937_DATATYPE_DOLBY_TRUE_HD,
#endif
};

hi_u32 autil_iec61937_data_type(hi_u16 *iec_data, hi_u32 iec_data_size)
{
    hi_u32 iec61937_data_type; /* value of PC bit 0-6 */
    hi_u16 header[8];

    iec61937_data_type = IEC61937_DATATYPE_NULL;
    if ((iec_data != HI_NULL) && (iec_data_size > 8 * sizeof(hi_u16))) {
        hi_u16 *data = HI_NULL;
        if (osal_copy_from_user(header, iec_data, 8 * sizeof(hi_u16)) != 0) {
            return IEC61937_DATATYPE_NULL;
        }
        data = header;

        if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) && (data[3] == 0)) {
            data += 4; /* 4 word16 burst spacing */
        }

        if (data[0] == IEC_61937_SYNC1) {  /* && (IEC_61937_SYNC2 == data[1]) */
            iec61937_data_type = data[2] & 0x3f;
        } else {
            iec61937_data_type = IEC61937_DATATYPE_DTSCD;
        }
    } else {
        iec61937_data_type = IEC61937_DATATYPE_NULL;
    }

    return iec61937_data_type;
}

hi_u32 autil_get_passth_format(hi_u32 format)
{
    return format & PASSTHROUGH_FORMAT_MASK;
}

hi_bool autil_is_lbr_format(hi_u32 lbr_format)
{
    return INT_TO_BOOL(lbr_format & PASSTHROUGH_FORMAT_MASK);
}

hi_bool autil_is_hbr_format(hi_u32 hbr_format)
{
    return INT_TO_BOOL(hbr_format & PASSTHROUGH_FORMAT_MASK);
}

#if defined(HI_SND_ARC_SUPPORT)
hi_bool autil_is_arc_support_hbr(hi_u32 hbr_format)
{
    return ((hbr_format & PASSTHROUGH_FORMAT_MASK) == IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS);
}
#endif

static hi_bool is_iec61937_hbr(hi_u32 iec61937_data_type, hi_u32 sample_rate)
{
    hi_s32 n;
    hi_bool hbr = HI_FALSE;

    /* check whether or not IEC61937 HBR data */
    for (n = 0; n < (hi_s32)(sizeof(g_iec61937_hbr_data_type) / sizeof(hi_s32)); n++) {
        if (g_iec61937_hbr_data_type[n] == iec61937_data_type) {
            hbr = HI_TRUE;
            break;
        }
    }

    if (iec61937_data_type == IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS) {
        /* discard ddp 32k_hz */
        if (!((sample_rate == HI_SAMPLE_RATE_48K) || (sample_rate == HI_SAMPLE_RATE_44K))) {
            hbr = HI_FALSE;
        }
    }

    return hbr;
}

hi_bool autil_is_iec61937_lbr(hi_u32 iec61937_data_type, hi_u32 sample_rate)
{
    if (iec61937_data_type == IEC61937_DATATYPE_NULL) {
        return HI_FALSE;
    }

    return !is_iec61937_hbr(iec61937_data_type, sample_rate);
}

hi_bool autil_is_iec61937_hbr(hi_u32 iec61937_data_type, hi_u32 sample_rate)
{
    if (iec61937_data_type == IEC61937_DATATYPE_NULL) {
        return HI_FALSE;
    }

    return is_iec61937_hbr(iec61937_data_type, sample_rate);
}

static hi_bool get_compress_flag(hi_u32 bits_bytes_per_frame)
{
    return INT_TO_BOOL(bits_bytes_per_frame & AOFRAME_PASSTHROUGH_COMPRESS_MASK);
}

static hi_u32 get_compress_iec_data_type(hi_u32 iec_data_type)
{
    return (iec_data_type | TRACKATTR_PASSTHROUGH_COMPRESS_MASK);
}

hi_u32 autil_get_iec_data_type(hi_u32 iec_data_type, hi_u32 bits_bytes_per_frame)
{
    hi_bool compressed = get_compress_flag(bits_bytes_per_frame);

    if (iec_data_type == IEC61937_DATATYPE_DOLBY_TRUE_HD) {
        return iec_data_type;
    }

    if (compressed == HI_TRUE) {
        iec_data_type = get_compress_iec_data_type(iec_data_type);
    }

    return iec_data_type;
}

hi_bool autil_is_truehd(hi_u32 iec_data_type)
{
    return INT_TO_BOOL(iec_data_type & TRUEHD_DATATYPE_MASK);
}

hi_u32 autil_get_truehd_data_type(hi_u32 iec_data_type)
{
    return (iec_data_type & TRUEHD_DATATYPE_MASK);
}

hi_u32 autil_calc_frame_size(hi_u32 ch, hi_u32 bit_depth)
{
    hi_u32 frame_size = 0;

    switch (bit_depth) {
        case HI_BIT_DEPTH_16:
            frame_size = ch * sizeof(hi_u16);
            break;
        case HI_BIT_DEPTH_24:
            frame_size = ch * sizeof(hi_u32);
            break;
    }

    return frame_size;
}

hi_u32 autil_latency_ms_to_byte_size(hi_u32 latency_ms, hi_u32 frame_size, hi_u32 sample_rate)
{
    return (sample_rate * latency_ms / 1000 * frame_size);
}

hi_u32 autil_byte_size_to_latency_ms(hi_u32 data_bytes, hi_u32 frame_size, hi_u32 sample_rate)
{
    if (!frame_size || !sample_rate) {
        return 0;
    } else {
        return (data_bytes * 1000) / (frame_size * sample_rate);
    }
}

/* table of normalised fixed point common logarithms */
static hi_u16 f_xlog10_table[] = {
    0x0000,
    0x00DD,
    0x01B9,
    0x0293,
    0x036B,
    0x0442,
    0x0517,
    0x05EB,
    0x06BD,
    0x078E,
    0x085D,
    0x092A,
    0x09F6,
    0x0AC1,
    0x0B8A,
    0x0C51,
    0x0D18,
    0x0DDD,
    0x0EA0,
    0x0F63,
    0x1024,
    0x10E3,
    0x11A2,
    0x125F,
    0x131B,
    0x13D5,
    0x148F,
    0x1547,
    0x15FE,
    0x16B4,
    0x1769,
    0x181C,
    0x18CF,
    0x1980,
    0x1A30,
    0x1ADF,
    0x1B8D,
    0x1C3A,
    0x1CE6,
    0x1D91,
    0x1E3B,
    0x1EE4,
    0x1F8C,
    0x2033,
    0x20D9,
    0x217E,
    0x2222,
    0x22C5,
    0x2367,
    0x2409,
    0x24A9,
    0x2548,
    0x25E7,
    0x2685,
    0x2721,
    0x27BD,
    0x2858,
    0x28F3,
    0x298C,
    0x2A25,
    0x2ABD,
    0x2B54,
    0x2BEA,
    0x2C7F,
    0x2D14,
    0x2DA8,
    0x2E3B,
    0x2ECD,
    0x2F5F,
    0x2FF0,
    0x3080,
    0x310F,
    0x319E,
    0x322C,
    0x32B9,
    0x3345,
    0x33D1,
    0x345C,
    0x34E7,
    0x3571,
    0x35FA,
    0x3682,
    0x370A,
    0x3792,
    0x3818,
    0x389E,
    0x3923,
    0x39A8,
    0x3A2C,
    0x3AB0,
    0x3B32,
    0x3BB5,
    0x3C36,
    0x3CB7,
    0x3D38,
    0x3DB8,
    0x3E37,
    0x3EB6,
    0x3F34,
    0x3FB2,
    0x402F,
    0x40AC,
    0x4128,
    0x41A3,
    0x421E,
    0x4298,
    0x4312,
    0x438C,
    0x4405,
    0x447D,
    0x44F5,
    0x456C,
    0x45E3,
    0x4659,
    0x46CF,
    0x4744,
    0x47B9,
    0x482E,
    0x48A2,
    0x4915,
    0x4988,
    0x49FB,
    0x4A6D,
    0x4ADE,
    0x4B50,
    0x4BC0,
    0x4C31,
    0x4CA0,
    0x4D10,
};

/* table of fixed point common logarithms for the powers of 2 */
static hi_u32 f_xlog2_table[] = {
    0x00000000L,
    0x00004D10L,
    0x00009A20L,
    0x0000E730L,
    0x00013441L,
    0x00018151L,
    0x0001CE61L,
    0x00021B72L,
    0x00026882L,
    0x0002B592L,
    0x000302A3L,
    0x00034FB3L,
    0x00039CC3L,
    0x0003E9D3L,
    0x000436E4L,
    0x000483F4L,
    0x0004d104L,
};

typedef hi_u32 fx_fixed; /* 16.16 format */

/****************************************************************************
*
* function:     fhll_mul
* parameters:   f   - fx_fixed point mutiplicand
*               g   - fx_fixed point number to multiply by
* returns:      result of the multiplication.
*
* description:  multiplies two fixed point number in 16.16 format together
*               and returns the result. we cannot simply multiply the
*               two 32 bit numbers together since we need to shift the
*               64 bit result right 16 bits, but the result of a fx_fixed
*               multiply is only ever 32 bits! thus we must resort to
*               computing it from first principles (this is slow and
*               should ideally be re-coded in assembler for the target
*               machine).
*
*               we can visualise the fixed point number as having two
*               parts, a whole part and a fractional part:
*
*               @formula fx_fixed = (whole + frac * 2^-16)
*
*               thus if we multiply two of these numbers together we
*               get a 64 bit result:
*
*               (a_whole + a_frac * 2^-16) * (whole + frac * 2^-16)
*
*                 = (a_whole * whole) +
*                   (a_whole * frac)*2^-16 +
*                   (whole * a_frac)*2^-16 +
*                   (a_frac * frac)*2^-32
*
*               to convert this back to a 64 bit fixed point number to 32
*               bit format we simply shift it right by 16 bits (we can round
*               it by adding 2^-17 before doing this shift). the formula
*               with the shift integrated is what is used below. natrually
*               you can alleviate most of this if the target machine can
*               perform a native 32 by 32 bit multiplication (since it
*               will produce a 64 bit result).
*
****************************************************************************/
static fx_fixed fhll_mul(fx_fixed f, fx_fixed g)
{
    fx_fixed a_whole, whole;
    fx_fixed a_frac, frac;

    /* extract the whole and fractional parts of the numbers. we strip the */
    /* sign bit from the fractional parts but leave it intact for the */
    /* whole parts. this ensures that the sign of the result will be correct. */
    a_frac = f & 0x0000FFFF;
    a_whole = f >> 16;
    frac = g & 0x0000FFFF;
    whole = g >> 16;

    /* we round the result by adding 2^(-17) before we shift the */
    /* fractional part of the result left 16 bits. */
    return ((a_whole * whole) << 16) +
           (a_whole * frac) +
           (a_frac * whole) +
           ((a_frac * frac + 0x8000) >> 16);
}

#define f_xmul(f, g) fhll_mul(f, g)

/****************************************************************************
*
* function:     fhll_log10
* parameters:   f   - number to take the square root of
* returns:      approximate square root of the number f
*
* description:  caculates the common logarithm of a fixed point number
*               using table lookup and linear interpolation.
*
*               first we isolate the first 8 bits of the mantissa in our
*               fixed point number. we do this by scanning along until we
*               find the first 1 bit in the number, and shift it all right
*               until this is in bit position 7. since IEEE floating point
*               numbers have an implied 1 bit in the mantissa, we mask this
*               bit out and use the 7 bits as an index into the table. we
*               then look up this value, and add in the appropriate logarithm
*               for the power for two represented by the numbers exponent.
*
*               because of the linear interpolation, this routine will
*               provide a common logarithm of any 16.16 fixed point
*               number that is as good as you can get given the precision
*               of fixed point (approx 1e-4 deviation).
*
****************************************************************************/
static fx_fixed fhll_log10(fx_fixed f)
{
    hi_s16 e, eindex;  /* exponent and index into table */
    fx_fixed r, diff, interpolant;

    if (f <= 0) {
        /* check for -ve and zero */
        return 0;
    }

    /* search for the index of the first 1 bit in the number (start of
     * the mantissa. note that we are only working with positive numbers
     * here, so we ignore the sign bit (bit 31).
     * exponent for number with 1 in bit
     */
    e = 14;

    /* position 30 */
    while ((f & 0x40000000) == 0) {
        /* isolate first bit */
        f <<= 1; /* shift all left 1 */
        e--; /* decrement exponent for number */
    }

    /* at this stage our number is in the following format:
     * bits 23-30        15-22       0-14
     * +-------------+-------------+---------+
     * |.table index.|.interpolant.|.ignored.|
     * +-------------+-------------+---------+
     * we compute the index into the table by shifting the mantissa
     * so that the first 1 bit ends up in bit position 7, and mask it
     * out. the interpolant factor that we use is the bottom 16
     * bits left in the original number after the index is extracted out,
     * and is used to linearly interpolate the results between the two
     * consecutive entries in the table.
     */
    eindex = (hi_s16)(f >> 23) & 0x7F;
    interpolant = (f >> 7) & 0xFFFF;

    /* lookup the values for the 7 bits of mantissa in the table, and
     * linearly interpolate between the two entries.
     */
    diff = f_xlog10_table[eindex + 1] - (r = f_xlog10_table[eindex]);
    r += f_xmul(diff, interpolant);

    /* now find the appropriate power of 2 logarithm to add to the final result. */
    if (e < 0) {
        r -= f_xlog2_table[-e];
    } else {
        r += f_xlog2_table[e];
    }

    return r;
}

hi_u32 autil_volume_linear_to_reg_db(hi_u32 linear)
{
    hi_u32 reg_db = VOLUME_0DB;

    if (linear > AO_MAX_LINEARVOLUME) {
        return HI_ERR_AO_INVALID_PARA;
    }

    /* if mute(volume=0), avoid tolerance in calculate fhll_log10 */
    if (linear == 0) {
        reg_db = VOLUME_INFDB;
    } else {
        /* 20 * log(alpha1 / 100.0) + VOLUME_0DB; */
        reg_db = (((hi_u32)((20 * fhll_log10(linear << 16)) + 0x8000) >> 16) - 40) + VOLUME_0DB;
    }

    return reg_db;
}

hi_u32 autil_volume_db_to_reg_db(hi_s32 volume_db)
{
    return (hi_u32)(volume_db + VOLUME_0DB);
}

hi_s32 autil_decimal_volume_db_to_reg_db(hi_s32 dec_volume_db)
{
    return dec_volume_db * VOLUME_PRECISION_VALUE / 1000;
}

hi_s32 autil_set_bit_zero_or_one(hi_u32 *val, hi_u32 bit, hi_u32 zero_or_one)
{
    if (zero_or_one == 0) {
        *val &= ~((hi_u32)1L << bit);
    } else if (zero_or_one == 1) {
        *val |= (hi_u32)1L << bit;
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

const aiao_track_mode autil_track_mode_transform(hi_track_mode mode)
{
    switch (mode) {
        case HI_TRACK_MODE_STEREO:
            return AIAO_TRACK_MODE_STEREO;

        case HI_TRACK_MODE_DOUBLE_MONO:
            return AIAO_TRACK_MODE_DOUBLE_MONO;

        case HI_TRACK_MODE_DOUBLE_LEFT:
            return AIAO_TRACK_MODE_DOUBLE_LEFT;

        case HI_TRACK_MODE_DOUBLE_RIGHT:
            return AIAO_TRACK_MODE_DOUBLE_RIGHT;

        case HI_TRACK_MODE_EXCHANGE:
            return AIAO_TRACK_MODE_EXCHANGE;

        case HI_TRACK_MODE_ONLY_RIGHT:
            return AIAO_TRACK_MODE_ONLY_RIGHT;

        case HI_TRACK_MODE_ONLY_LEFT:
            return AIAO_TRACK_MODE_ONLY_LEFT;

        case HI_TRACK_MODE_MUTED:
            return AIAO_TRACK_MODE_MUTED;

        default:
            return (aiao_track_mode)mode;
    }
}

#ifdef HI_PROC_SUPPORT
static const struct __ao_port_name {
    hi_ao_port port;
    hi_char *name;
} g_ao_port_name[] = {
    { HI_AO_PORT_DAC0, "DAC0" },
    { HI_AO_PORT_I2S0, "I2S0" },
    { HI_AO_PORT_I2S1, "I2S1" },
    { HI_AO_PORT_SPDIF0, "SPDIF0" },
    { HI_AO_PORT_HDMI0, "HDMI0" },
    { HI_AO_PORT_HDMI1, "HDMI1" },
    { HI_AO_PORT_ARC0, "ARC0" },
    { HI_AO_PORT_DAC1, "DAC1" },
    { HI_AO_PORT_DAC2, "DAC2" },
    { HI_AO_PORT_DAC3, "DAC3" },
    { HI_AO_PORT_ALL, "ALLPORT" },
};

const hi_char *autil_port_to_name(hi_ao_port port)
{
    hi_u32 i;

    for (i = 0; i < ARRAY_SIZE(g_ao_port_name); i++) {
        if (port == g_ao_port_name[i].port) {
            return g_ao_port_name[i].name;
        }
    }

    return "unknown_port";
}

const hi_ao_port autil_port_name_to_port(hi_char *name)
{
    hi_u32 i;

    for (i = 0; i < ARRAY_SIZE(g_ao_port_name); i++) {
        if (!strncmp(name, g_ao_port_name[i].name, strlen(g_ao_port_name[i].name))) {
            return g_ao_port_name[i].port;
        }
    }

    return HI_AO_PORT_MAX;
}

const hi_char *autil_ai_port_to_name(hi_ai_port ai_port)
{
    hi_u32 i;
    const struct __ai_port_name {
        hi_ai_port port;
        hi_char *name;
    } ai_port_name[] = {
        { HI_AI_I2S0, "I2S0" },
        { HI_AI_I2S1, "I2S1" },
        { HI_AI_ADC0, "ADC0" },
        { HI_AI_ADC1, "ADC1" },
        { HI_AI_ADC2, "ADC2" },
        { HI_AI_ADC3, "ADC3" },
        { HI_AI_ADC4, "ADC4" },
        { HI_AI_SIF0, "SIF0" },
        { HI_AI_HDMI0, "HDMI0" },
        { HI_AI_HDMI1, "HDMI1" },
        { HI_AI_HDMI2, "HDMI2" },
        { HI_AI_HDMI3, "HDMI3" },
    };

    for (i = 0; i < ARRAY_SIZE(ai_port_name); i++) {
        if (ai_port == ai_port_name[i].port) {
            return ai_port_name[i].name;
        }
    }

    return "unknown_port";
}

const hi_char *autil_track_mode_to_name(hi_track_mode mode)
{
    const hi_char *name[HI_TRACK_MODE_MAX] = {
        "STEREO",
        "DOULBE MONO",
        "DOULBE LEFT",
        "DOULBE RIGHT",
        "EXCHANGE",
        "ONLY RIGHT",
        "ONLY LEFT",
        "MUTE",
    };

    if (mode < HI_TRACK_MODE_MAX) {
        return name[mode];
    }

    return "unknown";
}

#ifdef HI_SND_SPDIF_SUPPORT
const hi_char *autil_category_code_to_name(hi_ao_spdif_category_code category)
{
    hi_u32 i;
    const struct __category_code_name {
        hi_ao_spdif_category_code category;
        hi_char *name;
    } category_code_name[] = {
        { HI_AO_SPDIF_CATEGORY_GENERAL, "general"},
        { HI_AO_SPDIF_CATEGORY_BROADCAST_JP, "broadcast_japan"},
        { HI_AO_SPDIF_CATEGORY_BROADCAST_USA, "broadcast_usa"},
        { HI_AO_SPDIF_CATEGORY_BROADCAST_EU, "broadcast_europe"},
        { HI_AO_SPDIF_CATEGORY_PCM_CODEC, "pcm_enc_dec"},
        { HI_AO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER, "sound_sampler"},
        { HI_AO_SPDIF_CATEGORY_DIGITAL_MIXER, "signal_mixer"},
        { HI_AO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR, "sound_processor"},
        { HI_AO_SPDIF_CATEGORY_SRC, "SRC"},
        { HI_AO_SPDIF_CATEGORY_MD, "mini_disc"},
        { HI_AO_SPDIF_CATEGORY_DVD, "DVD"},
        { HI_AO_SPDIF_CATEGORY_SYNTHESISER, "synthesiser"},
        { HI_AO_SPDIF_CATEGORY_MIC, "microphone"},
        { HI_AO_SPDIF_CATEGORY_DAT, "DAT"},
        { HI_AO_SPDIF_CATEGORY_DCC, "DCC"},
        { HI_AO_SPDIF_CATEGORY_VCR, "VCR"},
    };

    for (i = 0; i < ARRAY_SIZE(category_code_name); i++) {
        if (category == category_code_name[i].category) {
            return category_code_name[i].name;
        }
    }

    return "unknown_category";
}

const hi_char *autil_scms_mode_to_name(hi_ao_spdif_scms_mode scms)
{
    hi_u32 i;

    const struct __scms_mode_name {
        hi_ao_spdif_scms_mode scms;
        hi_char *name;
    } scms_mode_name[] = {
        { HI_AO_SPDIF_SCMS_MODE_COPY_ALLOW, "copy_allow"},
        { HI_AO_SPDIF_SCMS_MODE_COPY_ONCE, "copy_once"},
        { HI_AO_SPDIF_SCMS_MODE_COPY_NOMORE, "copy_no_more"},
        { HI_AO_SPDIF_SCMS_MODE_COPY_PROHIBITED, "copy_defy"},
    };

    for (i = 0; i < ARRAY_SIZE(scms_mode_name); i++) {
        if (scms == scms_mode_name[i].scms) {
            return scms_mode_name[i].name;
        }
    }

    return "unknown_scms";
}
#endif

const hi_char *autil_ouput_mode_to_name(hi_ao_ouput_mode mode)
{
    const hi_char *name[] = {
        "PCM",
        "RAW",
        "HBR2LBR",
        "AUTO",
    };

    if (mode < HI_AO_OUTPUT_MODE_MAX) {
        return name[mode];
    }

    return "unknown";
}

const hi_char *autil_track_to_name(hi_ao_track_type track_type)
{
    const hi_char *track_name[] = {
        "master",
        "slave",
        "virtual",
        "low_latency",
    };

    if (track_type < HI_AO_TRACK_TYPE_MAX) {
        return track_name[track_type];
    }

    return "unknown";
}

const hi_char *autil_engine_to_name(hi_u32 engine)
{
    const hi_char *name[] = {
        [PCM] = "PCM",
        [LBR] = "LBR",
        [HBR] = "HBR",
    };

    if (engine < TYPE_MAX) {
        return name[engine];
    }

    return "unknown";
}

const hi_char *autil_aip_to_name(hi_u32 aip)
{
    const hi_char *name[] = {
        "PCM",
        "LBR",
        "HBR",
    };

    if (aip < TYPE_MAX) {
        return name[aip];
    }

    return "unknown";
}

const hi_char *autil_format_to_name(hi_u32 format)
{
    switch (format & 0xff) {
        case IEC61937_DATATYPE_NULL:
            return "PCM";

        case IEC61937_DATATYPE_DOLBY_DIGITAL:
            return (format & 0xff00) ? "DD*" : "DD";

        case IEC61937_DATATYPE_DTS_TYPE_I:
        case IEC61937_DATATYPE_DTS_TYPE_II:
        case IEC61937_DATATYPE_DTS_TYPE_III:
            return (format & 0xff00) ? "DTS*" : "DTS";
        case IEC61937_DATATYPE_DTSCD:
            return "DTSCD";

        case IEC61937_DATATYPE_DTS_TYPE_IV:
            return (format & 0xff00) ? "DTSHD*" : "DTSHD";

        case IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS:
            return (format & 0xff00) ? "DDP*" : "DDP";

        case IEC61937_DATATYPE_DOLBY_TRUE_HD:
            return "TRUEHD";

        case IEC61937_DATATYPE_71_LPCM:
            return "7.1PCM";

        case IEC61937_DATATYPE_20_LPCM:
            return "2.0PCM";

        default:
            return "unknown";
    }
}

#ifdef HI_SND_PEQ_SUPPORT
static const struct __peq_filter_type_name {
    hi_ao_peq_filter_type filter_type;
    hi_char *name;
} g_peq_filter_type_name[] = {
    { HI_AO_PEQ_FILTER_TYPE_HP, "HP", },
    { HI_AO_PEQ_FILTER_TYPE_LS, "LS", },
    { HI_AO_PEQ_FILTER_TYPE_PK, "PK", },
    { HI_AO_PEQ_FILTER_TYPE_HS, "HS", },
    { HI_AO_PEQ_FILTER_TYPE_LP, "LP", },
};

const hi_char *autil_peq_type_to_name(hi_ao_peq_filter_type flt_type)
{
    if (flt_type < HI_AO_PEQ_FILTER_TYPE_MAX) {
        return g_peq_filter_type_name[flt_type].name;
    }

    return "NO";
}

const hi_ao_peq_filter_type autil_peq_type_name_to_type(hi_char *name)
{
    hi_u32 i;

    for (i = 0; i < ARRAY_SIZE(g_peq_filter_type_name); i++) {
        if (!strncmp(name, g_peq_filter_type_name[i].name, strlen(g_peq_filter_type_name[i].name))) {
            return g_peq_filter_type_name[i].filter_type;
        }
    }

    return HI_AO_PEQ_FILTER_TYPE_MAX;
}
#endif

const hi_char *autil_amp_type_to_name(hi_void)
{
    const hi_char *apc_name[] = {
#if defined(HI_AUDIO_AMP_AUTO_SELECT)
        [AUDIO_AMP_TYPE_EXTERNAL] = "EXTERNAL(auto_mode)",
        [AUDIO_AMP_TYPE_INTERNAL] = "INTERNAL(auto_mode)",
#else
        [AUDIO_AMP_TYPE_EXTERNAL] = "EXTERNAL",
        [AUDIO_AMP_TYPE_INTERNAL] = "INTERNAL",
#endif
    };

    return apc_name[autil_get_amp_type()];
}
#endif

hi_u32 autil_bclk_fclk_div(hi_i2s_mclk mclk_sel, hi_i2s_bclk bclk_sel)
{
    hi_u32 mclk_div;

    switch (mclk_sel) {
        case HI_I2S_MCLK_128_FS:
            mclk_div = 128;
            break;
        case HI_I2S_MCLK_256_FS:
            mclk_div = 256;
            break;
        case HI_I2S_MCLK_384_FS:
            mclk_div = 384;
            break;
        case HI_I2S_MCLK_512_FS:
            mclk_div = 512;
            break;
        case HI_I2S_MCLK_768_FS:
            mclk_div = 768;
            break;
        case HI_I2S_MCLK_1024_FS:
            mclk_div = 1024;
            break;
        default:
            return 0;
    }

    if (!(mclk_div % bclk_sel)) {
        return mclk_div / bclk_sel;
    }

    return 0;
}

hi_u32 autil_mclk_fclk_div(hi_i2s_mclk mclk_sel)
{
    switch (mclk_sel) {
        case HI_I2S_MCLK_128_FS:
            return 128;
        case HI_I2S_MCLK_256_FS:
            return 256;
        case HI_I2S_MCLK_384_FS:
            return 384;
        case HI_I2S_MCLK_512_FS:
            return 512;
        case HI_I2S_MCLK_768_FS:
            return 768;
        case HI_I2S_MCLK_1024_FS:
            return 1024;
        default:
            return 0;
    }
}

aiao_i2s_chnum autil_ch_num_unf_to_aiao(hi_i2s_channel channel)
{
    switch (channel) {
        case HI_I2S_CH_1:
            return AIAO_I2S_CHNUM_1;
        case HI_I2S_CH_2:
            return AIAO_I2S_CHNUM_2;
        case HI_I2S_CH_8:
            return AIAO_I2S_CHNUM_8;
        default:
            return AIAO_I2S_CHNUM_2;
    }
}

aiao_bit_depth autil_bit_depth_unf_to_aiao(hi_bit_depth bit_depth)
{
    switch (bit_depth) {
        case HI_BIT_DEPTH_16:
            return AIAO_BIT_DEPTH_16;
        case HI_BIT_DEPTH_24:
            return AIAO_BIT_DEPTH_24;
        default:
            return AIAO_BIT_DEPTH_16;
    }
}

aiao_i2s_mode autil_i2s_mode_unf_to_aiao(hi_i2s_mode i2s_mode)
{
    switch (i2s_mode) {
        case HI_I2S_STD_MODE:
            return AIAO_MODE_I2S;
        case HI_I2S_PCM_MODE:
            return AIAO_MODE_PCM;
        default:
            return AIAO_MODE_MAX;
    }
}

autil_chip_platform autil_get_chip_platform(hi_void)
{
    autil_chip_platform chip_platform;

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    chip_platform = AUTIL_CHIP_TYPE_PLATFORM_S28;
#else
    chip_platform = AUTIL_CHIP_TYPE_PLATFORM_S40;
#endif

    return chip_platform;
}

hi_bool autil_is_chip_mv300(hi_void)
{
    hi_chip_type chip_type = CHIP_TYPE_BUTT;
    hi_chip_version chip_version = CHIP_VERSION_BUTT;

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);

    return ((chip_type == CHIP_TYPE_HI3798M) &&
            (chip_version == CHIP_VERSION_V300));
}

hi_bool autil_is_chip_mv310(hi_void)
{
    hi_chip_type chip_type = CHIP_TYPE_BUTT;
    hi_chip_version chip_version = CHIP_VERSION_BUTT;

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);

    return (((chip_type == CHIP_TYPE_HI3798M) && (chip_version == CHIP_VERSION_V310)) ||
            ((chip_type == CHIP_TYPE_HI3798M_H) && (chip_version == CHIP_VERSION_V200)) ||
            ((chip_type == CHIP_TYPE_HI3798M_H) && (chip_version == CHIP_VERSION_V300)) ||
            ((chip_type == CHIP_TYPE_HI3716D) && (chip_version == CHIP_VERSION_V110)) ||
            ((chip_type == CHIP_TYPE_HI3716M) && (chip_version == CHIP_VERSION_V430)));
}

hi_bool autil_check_sys_mode(hi_void)
{
    /*
     * 1. hi3798mv200 need to get this flag from common
     * 2. do not add other chip here
     */
#if defined(CHIP_TYPE_HI3798MV200)
    hi_s32 ret;
    hi_u32 value = 0;

    ret = hi_drv_sys_get_flag(&value);

    return ((ret == HI_SUCCESS) && (value != 0));
#else
    return HI_FALSE;
#endif
}

/**
 * api for sync
 * read EPLL frac and int register cfg
 *
 * EPLL frac cfg register: 0xf8a22030
 * EPLL int  cfg register: 0xf8a22034
 *
 * warning:
 * 1. always add a branch to this function if you have a new cfg of EPLL,
 * 2. always check this fuction when you have a new chip because it usually has a different EPLL cfg
 *
 * 注意: 请在这里记录EPLL校准前的理论值
 */
hi_s32 autil_get_pll_cfg(hi_u32 *frac_cfg, hi_u32 *int_cfg)
{
    *frac_cfg = 0xffffffff;
    *int_cfg = 0xffffffff;

    return HI_FAILURE;
}

audio_amp_type autil_get_amp_type(hi_void)
{
#if defined(HI_AUDIO_EXTERNAL_AMP)
    return AUDIO_AMP_TYPE_EXTERNAL;
#elif defined(HI_AUDIO_INTERNAL_AMP)
    return AUDIO_AMP_TYPE_INTERNAL;
#elif defined(HI_AUDIO_AMP_AUTO_SELECT)
    hi_s32 ret;
    hi_u32 amp_type = 0;

    ret = hi_drv_sys_get_audio_op_mode(&amp_type);
    if (ret != HI_SUCCESS) {
        /* for chip(s) do not support hi_drv_sys_get_audio_op_mode API */
        return AUDIO_AMP_TYPE_EXTERNAL;
    }

    if (amp_type == 0) {
        return AUDIO_AMP_TYPE_EXTERNAL;
    } else {
        return AUDIO_AMP_TYPE_INTERNAL;
    }
#endif
}


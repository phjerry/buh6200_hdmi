/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio util function header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __AUDIO_UTIL_H__
#define __AUDIO_UTIL_H__

#include "hi_drv_ao.h"
#include "hi_drv_ai.h"
#include "drv_ao_private.h"

#include "hal_aiao_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VOLUME_6DB    0x7f
#define VOLUME_0DB    0x79
#define VOLUME_INFDB  0x28
#define VOLUME_MAX_DB AOE_AIP_VOL_6DB
#define VOLUME_MIN_DB AOE_AIP_VOL_INFDB

#define VOLUME_PRECISION_VALUE 8  /* 1/8 = 0.125 */

#define IEC_61937_SYNC1 0xF872
#define IEC_61937_SYNC2 0x4E1F

#define IEC61937_DATATYPE_NULL               0
#define IEC61937_DATATYPE_PAUSE              3
#define IEC61937_DATATYPE_DOLBY_DIGITAL      1  /* AC3 */
#define IEC61937_DATATYPE_DTS_TYPE_I         11 /* DTS type 1 */
#define IEC61937_DATATYPE_DTS_TYPE_II        12 /* DTS type 2 */
#define IEC61937_DATATYPE_DTS_TYPE_III       13 /* DTS type 3 */
#define IEC61937_DATATYPE_DTS_TYPE_IV        17 /* DTS type 4 */
#define IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS 21 /* EAC3 */
#define IEC61937_DATATYPE_DOLBY_TRUE_HD      22 /* TRUE-HD */

#define IEC61937_DATATYPE_71_LPCM 0xf0
#define IEC61937_DATATYPE_20_LPCM 0xf1

#define IEC61937_DATATYPE_DTSCD             0xff /* DTSCD */
#define IEC61937_DATATYPE_DOLBY_SIMUL       0xfe
#define AOFRAME_PASSTHROUGH_COMPRESS_MASK   0x8000
#define TRACKATTR_PASSTHROUGH_COMPRESS_MASK 0xff00
#define TRUEHD_DATATYPE_MASK                0xff
#define PASSTHROUGH_FORMAT_MASK             0xff

#define INT_TO_BOOL(i) (!!(i))
#define TRUE_ON_FALSE_OFF(value) (((value) == HI_TRUE) ? "on" : "off")

/* define IC PLATFORM */
typedef enum {
    AUTIL_CHIP_TYPE_PLATFORM_S40,
    AUTIL_CHIP_TYPE_PLATFORM_S5,
    AUTIL_CHIP_TYPE_PLATFORM_S28,
    AUTIL_CHIP_TYPE_PLATFORM_MAX,
} autil_chip_platform;

typedef enum {
    AUDIO_AMP_TYPE_EXTERNAL,
    AUDIO_AMP_TYPE_INTERNAL,
} audio_amp_type;

enum {
    HDMI_AO_INTERFACE_I2S,
    HDMI_AO_INTERFACE_SPDIF,
    HDMI_AO_INTERFACE_HBR,
    HDMI_AO_INTERFACE_MAX
};

typedef struct {
    hi_u32 audio_format;
    hi_u32 interface;  /* I2S SPDIF HBR */
    hi_u32 channels;
    hi_u32 org_channels;
    hi_u32 sample_rate;
    hi_u32 org_sample_rate;
    hi_u32 bit_depth;
} hdmi_ao_attr;

static inline hi_u32 saturate_mult(hi_u32 x, hi_u32 saturation)
{
    hi_u32 y = 1;

    while ((x << y) <= saturation) {
        y++;
    }

    return (x << (y - 1));
}

#define AUDIO_ALIGN_SIZE_4       0x4
#define AUDIO_ALIGN_SIZE_MASK_4  0x3

static inline hi_u32 audio_util_align_4(hi_u32 x)
{
    if (x & AUDIO_ALIGN_SIZE_MASK_4) {
        x = x + AUDIO_ALIGN_SIZE_4 - (x & AUDIO_ALIGN_SIZE_MASK_4);
    }

    return x;
}

hi_u32 autil_get_iec_data_type(hi_u32 iec_data_type, hi_u32 bits_bytes_per_frame);
hi_u32 autil_get_truehd_data_type(hi_u32 iec_data_type);
hi_bool autil_is_truehd(hi_u32 iec_data_type);
hi_bool autil_is_iec61937_lbr(hi_u32 iec61937_data_type, hi_u32 sample_rate);
hi_u32 autil_get_lbr_data_type(hi_u32 iec_data_type, hi_u32 bits_bytes_per_frame);
hi_u32 autil_iec61937_data_type(hi_u16 *iec_data, hi_u32 iec_data_size);
hi_bool autil_is_iec61937_hbr(hi_u32 iec61937_data_type, hi_u32 sample_rate);
hi_u32 autil_calc_frame_size(hi_u32 ch, hi_u32 bit_depth);
hi_u32 autil_latency_ms_to_byte_size(hi_u32 latency_ms, hi_u32 frame_size, hi_u32 sample_rate);
hi_u32 autil_byte_size_to_latency_ms(hi_u32 data_bytes, hi_u32 frame_size, hi_u32 sample_rate);
hi_u32 autil_volume_linear_to_reg_db(hi_u32 linear);
hi_u32 autil_volume_db_to_reg_db(hi_s32 volume_db);
hi_s32 autil_decimal_volume_db_to_reg_db(hi_s32 dec_volume_db);
hi_s32 autil_set_bit_zero_or_one(hi_u32 *val, hi_u32 bit, hi_u32 zero_or_one);
hi_u32 autil_bclk_fclk_div(hi_i2s_mclk mclk_sel, hi_i2s_bclk bclk_sel);
hi_u32 autil_mclk_fclk_div(hi_i2s_mclk mclk_sel);

const aiao_track_mode autil_track_mode_transform(hi_track_mode mode);

#ifdef HI_PROC_SUPPORT
const hi_char *autil_port_to_name(hi_ao_port port);
const hi_ao_port autil_port_name_to_port(hi_char *pc_name);
const hi_char *autil_ai_port_to_name(hi_ai_port ai_port);
const hi_char *autil_track_mode_to_name(hi_track_mode mode);
const hi_char *autil_ouput_mode_to_name(hi_ao_ouput_mode mode);
const hi_char *autil_track_to_name(hi_ao_track_type track_type);
const hi_char *autil_engine_to_name(hi_u32 engine);
const hi_char *autil_aip_to_name(hi_u32 aip);
const hi_char *autil_format_to_name(hi_u32 format);
const hi_char *autil_category_code_to_name(hi_ao_spdif_category_code category);
const hi_char *autil_scms_mode_to_name(hi_ao_spdif_scms_mode scms);
const hi_char *autil_arc_mode_to_name(hi_ao_ouput_mode mode);
const hi_char *autil_peq_type_to_name(hi_ao_peq_filter_type flt_type);
const hi_ao_peq_filter_type autil_peq_type_name_to_type(hi_char *pc_name);
const hi_char *autil_amp_type_to_name(hi_void);
#endif

aiao_i2s_chnum autil_ch_num_unf_to_aiao(hi_i2s_channel channel);
aiao_bit_depth autil_bit_depth_unf_to_aiao(hi_bit_depth bit_depth);
aiao_i2s_mode autil_i2s_mode_unf_to_aiao(hi_i2s_mode i2s_mode);
autil_chip_platform autil_get_chip_platform(hi_void);
hi_u32 autil_get_passth_format(hi_u32 format);
hi_bool autil_is_lbr_format(hi_u32 lbr_format);
hi_bool autil_is_hbr_format(hi_u32 hbr_format);
hi_bool autil_is_arc_support_hbr(hi_u32 hbr_format);

hi_bool autil_is_chip_mv300(hi_void);
hi_bool autil_check_sys_mode(hi_void);
hi_s32 autil_get_pll_cfg(hi_u32 *frac_cfg, hi_u32 *int_cfg);
hi_bool autil_is_chip_mv310(hi_void);
audio_amp_type autil_get_amp_type(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

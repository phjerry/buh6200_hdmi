/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio common codec layer.
 * Author: audio
 * Create: 2019-05-26
 * Notes:
 * History: 2019-05-26 audio common for Hi3796CV300
 */

#ifndef __HI_AO_TYPE_H__
#define __HI_AO_TYPE_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/********************************macro definition*****************************/
/* maximum sound outputport */
/* CNcomment: 最大sound输出端口 */
#define HI_AO_OUTPUT_PORT_MAX (16)
#define HI_AO_EQ_BAND_NUM_MAX (10)

/*************************** structure definition ****************************/
/* defines the ID of the audio output (AO) device. */
/* CNcomment: 定义音频输出设备号 */
typedef enum {
    /* AO device 0 */ /* CNcomment: 音频输出设备0. */
    AO_SND_0,

    /* AO device 1 */ /* CNcomment: 音频输出设备1. */
    AO_SND_1,

    /* AO device 2 */ /* CNcomment: 音频输出设备2. */
    AO_SND_2,

    AO_SND_MAX
} ao_snd_id;

/* audio volume attribute */
/* CNcomment: 音频音量属性 */
typedef struct {
    hi_bool linear_mode; /* gain type of volume */ /* CNcomment: 音量模式 */
    hi_s32 gain;         /* linear gain(linear_mode is HI_TRUE), ranging from 0 to 100 */
                         /* CNcomment: 线性音量: 0~100 */
                         /* decibel gain(linear_mode is HI_FALSE), ranging from -70db to 0db */
                         /* CNcomment: db音量:-70~0 */
} hi_ao_gain;

/* audio L/R channel volume attribute */
/* CNcomment: 音频左右声道音量属性 */
typedef struct {
    hi_bool linear_mode; /* gain type */ /* CNcomment:音量模式 */
    hi_s32 gain_l;       /* linear left ch gain(linear_mode is HI_TRUE), ranging from 0 to 100 */
                         /* CNcomment: 左声道线性音量: 0~100 */
                         /* decibel left ch gain(linear_mode is HI_FALSE), ranging from -81db to 18db */
                         /* CNcomment: 左声道d_b音量:-81~+18 */
    hi_s32 gain_r;       /* linear right ch gain(linear_mode is HI_TRUE), ranging from 0 to 100 */
                         /* CNcomment: 右声道线性音量: 0~100 */
                         /* decibel right ch gain(linear_mode is HI_FALSE), ranging from -81db to 18db */
                         /* CNcomment: 右声道db音量:-81~+18 */
} hi_ao_abs_gain;

/* high precision gain, ranging from -81db to 18db, step 0.125db */
/* CNcomment:高精度增益，范围从-81db到18db，步长为0.125db */
typedef struct {
    /* interger part of high precision gain */ /* CNcomment: 高精度增益的整数部分 */
    hi_s32 integer;

    /* decimal part of high precision gain, if -0.125db, value is -125 */
    /* CNcomment: 高精度增益的小数部分 如0.125，则值为125 */
    hi_s32 decimal;
} hi_ao_preci_gain;

/* audio track type: master, slave, virtual channel,low_latency channel */
/* CNcomment: 音频track类型: 主通道 辅通道 虚拟通道 低延时通道 */
/* low_latency channel not support attach avplay and the data only support 48K,stereo,16_bit.
 * only support one low latency track in a sound
 */
/* CNcomment: 低延时通道不支持AVPLAY绑定，此通道只支持采样率为48K,双声道,16位位宽的音频数据,仅支持创建一个低延时通道 */
typedef enum {
    HI_AO_TRACK_TYPE_MASTER = 0,
    HI_AO_TRACK_TYPE_SLAVE,
    HI_AO_TRACK_TYPE_VIRTUAL,
    HI_AO_TRACK_TYPE_LOW_LATENCY,
    HI_AO_TRACK_TYPE_OTT_SOUND,    /* ott sound(PCM) MS12 ONLY. such as app ui audio */
    HI_AO_TRACK_TYPE_SYSTEM_AUDIO, /* system sound(PCM) MS12 ONLY. */
    HI_AO_TRACK_TYPE_APP_AUDIO,    /* app audio(PCM) MS12 ONLY. such as tts audio */
    HI_AO_TRACK_TYPE_MAX
} hi_ao_track_type;

/* audio output attribute */
/* CNcomment:音频输出属性 */
typedef struct {
    /* track type */ /* CNcomment: track类型 */
    hi_ao_track_type track_type;

    /* fade in time(unit:ms) */ /* CNcomment: 淡入时间(单位: ms) */
    hi_u32 fade_in;

    /* fade out time(unit:ms) */ /* CNcomment: 淡出时间(单位: ms) */
    hi_u32 fade_out;

    /* track output buffer size */ /* CNcomment: track输出缓存大小 */
    hi_u32 output_buf_size;

    /* output buffer data size control(ms),default 400ms */
    /* CNcomment: 输出缓存中数据量控制(ms),默认400ms */
    hi_u32 buf_level;

    /* track start threshold (ms), default value is 50ms,
       max value is equal to buf_level_ms, only valid for master track */
    /* CNcomment: track启动门限，默认值是50ms，最大值为buf_level_ms，此属性只对master track有效 */
    hi_u32 start_threshold;
} hi_ao_track_attr;

/* audiotrack config parameter */
/* CNcomment: 音频 track配置参数 */
typedef struct {
    hi_u32  type; /* track config type */ /* CNcomment: track配置类型 */
    hi_void *data; /* track config data */ /* CNcomment: track配置数据 */
} hi_ao_track_config;

/* audio outputport: DAC0,I2S0,SPDIF0,HDMI0,ARC0 */
/* CNcomment: 音频输出端口:DAC0,I2S0,SPDIF0,HDMI0,ARC0 */
typedef enum {
    HI_AO_PORT_DAC0 = 0,
    HI_AO_PORT_DAC1,
    HI_AO_PORT_DAC2,
    HI_AO_PORT_DAC3,

    HI_AO_PORT_I2S0 = 0x10,
    HI_AO_PORT_I2S1,
    HI_AO_PORT_I2S2,
    HI_AO_PORT_I2S3,
    HI_AO_PORT_I2S4,

    HI_AO_PORT_SPDIF0 = 0x20,

    HI_AO_PORT_HDMI0 = 0x30,
    HI_AO_PORT_HDMI1,

    HI_AO_PORT_ARC0 = 0x40,
    HI_AO_PORT_ARC1,

    HI_AO_PORT_ALL = 0x7fff,

    HI_AO_PORT_MAX,
} hi_ao_port;

/* defines internal audio DAC outport attribute */
/* CNcomment: 定义内置音频DAC输出端口属性 */
typedef struct {
    hi_u32 reserved;
} hi_ao_dac_attr;

/* defines  audio I2S outport attribute */
/* CNcomment: 定义音频I2S输出端口属性 */
typedef struct {
    hi_i2s_attr attr;
} hi_ao_i2s_attr;

/* defines  S/PDIF outport attribute */
/* CNcomment: 定义S/PDIF输出端口属性 */
typedef struct {
    hi_u32 reserved;
} hi_ao_spdif_attr;

/* defines  HDMI audio outport attribute */
/* CNcomment: 定义HDMI音频输出端口属性 */
typedef struct {
    hi_u32 reserved;
} hi_ao_hdmi_attr;

/* defines  HDMI ARC outport attribute */
/* CNcomment: 定义HDMI音频回传通道端口属性 */
typedef struct {
    hi_u32 reserved;
} hi_ao_arc_attr;

/* defines ARC audio capbilities */
/* CNcomment: ARC音频能力 */
typedef struct {
    /* < Audio capability,reference EIA-CEA-861-D,table 37,HI_TRUE:support this Audio type;
    HI_FALSE,nonsupport this Audio type */
    /* < CNcomment: 音频能力集, 请参考EIA-CEA-861-D 表37;HI_TRUE表示支持这种显示格式，HI_FALSE表示不支持 */
    hi_bool audio_fmt_supported[HI_AUDIO_FORMAT_CXT];
} hi_ao_arc_audio_cap;

/* defines  audio outport attribute */
/* CNcomment: 定义音频输出端口属性 */
typedef struct {
    hi_ao_port port;
    union {
        hi_ao_dac_attr   dac_attr;
        hi_ao_i2s_attr   i2s_attr;
        hi_ao_spdif_attr spdif_attr;
        hi_ao_hdmi_attr  hdmi_attr;
        hi_ao_arc_attr   arc_attr;
    } un_attr;
} hi_ao_port_attr;

/* defines audio sound device attribute */
/* CNcomment: 定义sound设备属性 */
typedef struct {
    /* outport number attached sound */ /* CNcomment: 绑定到sound设备的输出端口数 */
    hi_u32 port_num;

    /* outports attached sound */ /* CNcomment: 绑定到sound设备的输出端口 */
    hi_ao_port_attr outport[HI_AO_OUTPUT_PORT_MAX];

    /* sound samplerate */ /* CNcomment: sound设备输出采样率 */
    hi_sample_rate sample_rate;

    /* sound master channel buffer size */ /* CNcomment: sound设备主输出通道缓存大小 */
    hi_u32 master_output_buf_size;

    /* sound slave channel buffer size */ /* CNcomment: sound设备从输出通道缓存大小 */
    hi_u32 slave_output_buf_size;
} hi_ao_attr;

/* defines  render attribute */
/* CNcomment: 定义render引擎属性 */
typedef struct {
    hi_char *comm_path;
} hi_ao_render_attr;

/* define SND CAST config  struct */
/* CNcomment: 定义声音共享配置结构体 */
typedef struct {
    /* Max frame of the PCM data at cast buffer */
    /* CNcomment: 最大可缓存帧数 */
    hi_u32 pcm_frame_max_num;

    /* Number of sample of the PCM data */
    /* CNcomment: PCM数据采样点数量 */
    hi_u32 pcm_samples;

    /* Cast Output samplerate */
    /* CNcomment:Cast输出采样率 */
    hi_sample_rate sample_rate;

    /* Cast Output bitdepth */
    /* CNcomment:Cast输出位宽 */
    hi_bit_depth bit_depth;

    /* Add mute frame in cast or not */
    /* CNcomment: cast是否插入静音帧 */
    hi_bool add_mute;

    /* Cast Output channels */
    /* CNcomment:Cast输出声道数 */
    hi_u32 channels;
} hi_ao_cast_attr;

/* HDMI MODE: AUTO,LPCM,RAW,HBR2LBR */
/* CNcomment: HDMI 模式:AUTO,LPCM,RAW,HBR2LBR */
typedef enum {
    /* LCPM2.0 */ /* CNcomment: 立体声pcm */
    HI_AO_OUTPUT_MODE_LPCM = 0,

    /* pass-through. */ /* CNcomment: 透传 */
    HI_AO_OUTPUT_MODE_RAW,

    /* pass-through force high-bitrate to low-bitrate. */ /* CNcomment: 蓝光次世代音频降规格输出 */
    HI_AO_OUTPUT_MODE_HBR2LBR,

    /* automatically match according to the EDID of HDMI */ /* CNcomment: 根据HDMI EDID能力自动匹配 */
    HI_AO_OUTPUT_MODE_AUTO,

    HI_AO_OUTPUT_MODE_MAX
} hi_ao_ouput_mode;

/* SPDIF category code setting */
/* CNcomment: 设置SPDIF category code类型 */
typedef enum {
    /* broadcast reception of digitally encoded audio
       with/without video signals */
    /* general */ /* CNcomment: 通用 */
    HI_AO_SPDIF_CATEGORY_GENERAL = 0x00,

    /* japan */ /* CNcomment:日本 */
    HI_AO_SPDIF_CATEGORY_BROADCAST_JP = 0x10,

    /* united states */ /* CNcomment:美国 */
    HI_AO_SPDIF_CATEGORY_BROADCAST_USA,

    /* digital converter & signal-processing products */
    /* europe */ /* CNcomment:欧洲 */
    HI_AO_SPDIF_CATEGORY_BROADCAST_EU,

    /* PCM encoder/decoder */ /* CNcomment: PCM编解码 */
    HI_AO_SPDIF_CATEGORY_PCM_CODEC = 0x20,

    /* digital sound sampler */ /* CNcomment: 数字音频采样器 */
    HI_AO_SPDIF_CATEGORY_DIGITAL_SNDSAMPLER,

    /* digital signal mixer */ /* CNcomment: 数字信号混音器 */
    HI_AO_SPDIF_CATEGORY_DIGITAL_MIXER,

    /* digital sound processor */ /* CNcomment: 数字音频处理器 */
    HI_AO_SPDIF_CATEGORY_DIGITAL_SNDPROCESSOR,

    /* laser optical products */
    /* sample rate converter */ /* CNcomment: 采样率转换器 */
    HI_AO_SPDIF_CATEGORY_SRC,

    /* mini_disc */ /* CNcomment: 迷你磁光盘 */
    HI_AO_SPDIF_CATEGORY_MD = 0x30,

    /* musical instruments, microphones and other sources
       that create original sound */
    /* digital versatile disc */ /* CNcomment: 数字多功能光盘 */
    HI_AO_SPDIF_CATEGORY_DVD,

    /* synthesiser */ /* CNcomment: 合成器 */
    HI_AO_SPDIF_CATEGORY_SYNTHESISER = 0x40,

    /* magnetic tape or magnetic disc based products */
    /* microphone */ /* CNcomment: 麦克风 */
    HI_AO_SPDIF_CATEGORY_MIC,

    /* digital audio tape */ /* CNcomment: 数字录音带 */
    HI_AO_SPDIF_CATEGORY_DAT = 0x50,

    /* digital compact cassette */ /* CNcomment: 数字盒式磁带录音机 */
    HI_AO_SPDIF_CATEGORY_DCC,

    /* video cassette recorder */ /* CNcomment: 盒式磁带录像机 */
    HI_AO_SPDIF_CATEGORY_VCR,

    HI_AO_SPDIF_CATEGORY_MAX
} hi_ao_spdif_category_code;

/* SPDIF SCMS mode setting */
/* CNcomment: 设置SPDIF SCMS模式 */
typedef enum {
    /* copy allow */ /* CNcomment: 允许复制 */
    HI_AO_SPDIF_SCMS_MODE_COPY_ALLOW,

    /* copy once */ /* CNcomment: 可复制一次 */
    HI_AO_SPDIF_SCMS_MODE_COPY_ONCE,

    /* copy no_more */ /* CNcomment: 不可复制 */
    HI_AO_SPDIF_SCMS_MODE_COPY_NOMORE,

    /* copy prohibited */ /* CNcomment: 禁止复制 */
    HI_AO_SPDIF_SCMS_MODE_COPY_PROHIBITED,

    HI_AO_SPDIF_SCMS_MODE_MAX
} hi_ao_spdif_scms_mode;

/* audio effect type, ha_audio_effect_type keep consistent with effect type of ARM/DSP */
/* CNcomment: 音效类型，ha_audio_effect_type 必须 ARM/DSP 保持唯一值 */
typedef enum {
    /* dolby audio effect */ /* CNcomment: dolby音效 */
    HI_AO_AEF_TYPE_DOLBY = 0x000,

    /* SRS audio effect */ /* CNcomment: SRS音效 */
    HI_AO_AEF_TYPE_SRS3D = 0x010,

    /* base audio effect */ /* CNcomment: 自研音效 */
    HI_AO_AEF_TYPE_BASE = 0x080,
} hi_ao_aef_type;

typedef enum {
    /* high-pass filter */ /* CNcomment: 高通滤波器 */
    HI_AO_PEQ_FILTER_TYPE_HP = 0,

    /* low-shelving filte r */ /* CNcomment: 低频搁架滤波器 */
    HI_AO_PEQ_FILTER_TYPE_LS = 1,

    /* peaking filter */ /* CNcomment: 峰值滤波器 */
    HI_AO_PEQ_FILTER_TYPE_PK = 2,

    /* high-shelving filter */ /* CNcomment: 高频搁架滤波器 */
    HI_AO_PEQ_FILTER_TYPE_HS = 3,

    /* low-pass filter */ /* CNcomment: 低通滤波器 */
    HI_AO_PEQ_FILTER_TYPE_LP = 4,

    HI_AO_PEQ_FILTER_TYPE_MAX
} hi_ao_peq_filter_type;

/* defines PEQ band attribute */
/* CNcomment: PEQ 频带属性 */
typedef struct {
    /* filter type of the band */ /* CNcomment: 某一频带的滤波器类型 */
    hi_ao_peq_filter_type type;

    /* center frequency of the band, HP and LS[20, 4000], PK[20, 22000], HS[4000, 22000], LP[50, 22000] */
    /* CNcomment: 某一频带的中心频率,范围: HP and LS[20, 4000], PK[20, 22000], HS[4000, 22000], LP[50, 22000] */
    hi_u32 freq;

    /* Q value of the band, if 2.5, value is 25, range:HS and LS[0.7, 1], PK[0.5, 10],HP and LP is fix to be 0.7 */
    /* CNcomment: 某一频带的q值,如值为2.5,设置值为25,范围: HS and LS[0.7, 1], PK[0.5, 10], 高低通内部固定为0.7 */
    hi_u32 q;

    /* gain of the band, if -2.125, value is -2125,gain ranging from -15 to 15 step by 0.125db */
    /* CNcomment: 某一频带的增益,如值为-2.125,设置值为-2125,范围:[-15, 15],精度0.125 */
    hi_s32 gain;
} hi_ao_peq_band_attr;

/* defines PEQ attribute */
/* CNcomment: PEQ属性 */
typedef struct {
    /* the PEQ band number,band_num ranging from 5 to 10 */
    /* CNcomment: PEQ频带数量,范围:[5,10] */
    hi_u32 band_num;

    /* PEQ band attribute */ /* CNcomment: PEQ频带属性 */
    hi_ao_peq_band_attr param[HI_AO_EQ_BAND_NUM_MAX];
} hi_ao_peq_attr;

/* defines GEQ band attribute */
/* CNcomment: GEQ 频带属性 */
typedef struct {
    /* < center frequency of the band */ /* < CNcomment: 某一频带的中心频率 */
    hi_u32 freq;

    /* < gain of the band, if -2.125, value is -2125,Gain ranging from -15 to 15 step by 0.125dB */
    /* < CNcomment: 某一频带的增益,如值为-2.125,设置值为-2125, 范围为-15至15 */
    hi_u32 gain;
} hi_ao_geq_band_attr;

/* defines GEQ attribute */
/* CNcomment: GEQ属性 */
typedef struct {
    /* < the GEQ band number, ranging from 5 to 10 */ /* < CNcomment: GEQ频带数量,范围为5至10 */
    hi_u32 band_num;

    /* < GEQ band attribute */ /* < CNcomment: GEQ频带属性 */
    hi_ao_geq_band_attr param[HI_AO_EQ_BAND_NUM_MAX];
} hi_ao_geq_attr;

/* defines DRC attribute */
/* CNcomment: DRC属性 */
typedef struct {
    /* the attack time(unit:ms),ranging from 20 to 2000 */ /* CNcomment: 开始生效时间(单位:ms),范围:[20, 2000] */
    hi_u32 attack_time;

    /* the release time(unit:ms),ranging from 20 to 2000 */ /* CNcomment: 开始失效时间(单位:ms),范围:[20, 2000] */
    hi_u32 release_time;

    /* the threshold value, if -2.125, value is -2125, threshold ranging from -80db to -1db step by 0.125db */
    /* CNcomment: 开始限幅幅值,如值为-2.125,设置值为-2125,步长为0.125db,范围:[-80db, -1db) */
    hi_s32 threshold;

    /* the maximum value, if -2.125, value is -2125, threshold ranging from -80db to -1db step by 0.125db */
    /* CNcomment: 最大限幅幅值,如值为-2.125,设置值为-2125,步长为0.125db,范围:[-80db, -1db) */
    hi_s32 limit;

    /* refmode(0: process both channel together mode, 1: process each channel independently mode) */
    /* CNcomment: 0, 原始版本效果; 1：单声道处理; 其余配置无效 */
    hi_u32 ref_mode;

    /* speedctrlmode(0:normal mode, 1:fast mode) */ /* CNcomment: 0, 原始版本效果; 1, 较快速控制; 其余配置无效 */
    hi_u32 speed_ctrl_mode;
} hi_ao_drc_attr;

/* defines  AVC attribute */
/* CNcomment: AVC属性 */
typedef struct {
    /* the threshold value, if -32.125, value is -32125, ranging from -40db to -16db step by 0.125db */
    /* CNcomment: 达到最大抬升增益的转折电平,如值为-32.125,设置值为-32125,步长为0.125db,范围(-40db, -16db) */
    hi_s32 threshold;

    /* the gain value, if 2.125, value is 2125,ranging from 0db to 8db step by 0.125db */
    /* CNcomment: 抬升增益,如值为2.125,设置值为2125,步长为0.125db,范围:[0db, 8db] */
    hi_s32 gain;

    /* the target level value, if -2.125, value is -2125, ranging from -40db to 0db step by 0.125db */
    /* CNcomment:目标值,如值为-2.125,设置值为-2125,步长为0.125db,范围:(-40db, 0db] */
    hi_s32 limit;

    /* the attack time(unit:ms),ranging from 20 to 2000,default value 50(recommended) */
    /* CNcomment: 开始生效时间(单位:ms),范围:[20, 2000]，默认值50，不建议修改 */
    hi_u32 attack_time;

    /* the release time(unit:ms),ranging from 20 to 2000,default value 100(recommended) */
    /* CNcomment: 开始失效时间(单位:ms),范围:[20, 2000]，默认值100，不建议修改 */
    hi_u32 release_time;
} hi_ao_avc_attr;

/* audio output latency mode */
/* CNcomment: 音频输出时延模式 */
typedef enum {
    /* normal latency MS12 ONLY. */ /* CNcomment: 通用时延 */
    HI_AO_OUTPUT_LATENCY_NORMAL = 0,

    /* low latency MS12 ONLY */ /* CNcomment: 低时延 */
    HI_AO_OUTPUT_LATENCY_LOW,

    HI_AO_OUTPUT_LATENCY_MAX,
} hi_ao_output_latency;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_AO_TYPE_H__ */


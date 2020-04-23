/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao device ioctl header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_IOCTL_H__
#define __DRV_AO_IOCTL_H__

#include "hi_drv_dev.h"

#include "hi_ao_type.h"
#include "hi_drv_ao.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    ao_snd_id sound;
    hi_ao_attr attr;
} ao_snd_open_default_param, *ao_snd_open_default_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_attr attr;
} ao_snd_open_param, *ao_snd_open_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool mute;
} ao_snd_mute_param, *ao_snd_mute_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port ao_port;
    hi_ao_ouput_mode mode;
} ao_snd_output_mode_param, *ao_snd_output_mode_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_spdif_scms_mode scms_mode;
} ao_snd_spdif_scms_mode_param, *ao_snd_spdif_scms_mode_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_spdif_category_code category_code;
} ao_snd_spdif_category_code_param, *ao_snd_spdif_category_code_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_gain gain;
} ao_snd_volume_param, *ao_snd_volume_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_sample_rate sample_rate;
} ao_snd_sample_rate_param, *ao_snd_sample_rate_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_track_mode mode;
} ao_snd_track_mode_param, *ao_snd_track_mode_param_p;

typedef struct {
    ao_snd_id sound;
    hi_bool mute;
} ao_snd_all_track_mute_param, *ao_snd_all_track_mute_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_preci_gain all_track_prescale;
} ao_snd_all_track_prescale_param, *ao_snd_all_track_prescale_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool smart_volume;
} ao_snd_smart_volume_param, *ao_snd_smart_volume_param_p;

typedef struct {
    ao_snd_id sound;
    hi_bool enable;
} ao_snd_adac_enable_param, *ao_snd_adac_enable_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_preci_gain preci_gain;
} ao_snd_preci_volume_param, *ao_snd_preci_volume_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_s32 balance;
} ao_snd_balance_param, *ao_snd_balance_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool enable;
} ao_snd_arc_enable_param, *ao_snd_arc_enable_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_ouput_mode mode;
} ao_snd_arc_mode_param, *ao_snd_arc_mode_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_arc_audio_cap cap;
} ao_snd_arc_cap_param, *ao_snd_arc_cap_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_geq_attr eq_attr;
} ao_snd_geq_param, *ao_snd_geq_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_peq_attr eq_attr;
} ao_snd_peq_param, *ao_snd_peq_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool eq_enable;
} ao_snd_eq_enable, *ao_snd_eq_enable_p;

typedef struct {
    ao_snd_id sound;
    hi_u32 band;
    hi_s32 gain;
} ao_snd_geq_gain, *ao_snd_geq_gain_p;

typedef struct {
    ao_snd_id sound;
    hi_bool avc_enable;
} ao_snd_avc_enable, *ao_snd_avc_enable_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_avc_attr avc_attr;
} ao_snd_avc_param, *ao_snd_avc_param_p;

typedef struct {
    ao_snd_id sound;
    hi_bool con_output_enable;
} ao_snd_con_output_enable, *ao_snd_con_output_enable_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_output_latency output_mode;
} ao_snd_output_latency_mode, *ao_snd_output_latency_mode_p;

typedef struct {
    ao_snd_id sound;
    hi_bool output_atmos_enable;
} ao_snd_output_atmos_enable, *ao_snd_output_atmos_enable_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool drc_enable;
} ao_snd_drc_enable, *ao_snd_drc_enable_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_ao_drc_attr drc_attr;
} ao_snd_drc_param, *ao_snd_drc_param_p;

typedef struct {
    ao_snd_id sound;
    hi_u32 type;
    hi_u32 buf_phy_addr;
    hi_u32 buf_size;
} ao_snd_isb_param, *ao_snd_isb_param_p;

typedef struct {
    ao_snd_id sound;
    ao_render_attr render_attr;
} ao_snd_render_param, *ao_snd_render_param_p;

#ifdef __DPT__
typedef struct {
    ao_snd_id sound;
    hi_unf_snd_track_info track_info;
} ao_snd_track_info_param, *ao_snd_track_info_param_p;
#endif

typedef struct {
    ao_snd_id sound;
    hi_ao_track_attr attr;
    hi_bool alsa_track;
    ao_buf_attr buf;
    hi_handle h_track;
} ao_track_create_param, *ao_track_create_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool ad_output_enable;
} ao_snd_ad_output_enable, *ao_snd_ad_output_enable_p;

typedef struct {
    hi_handle h_track;
    hi_ao_track_attr attr;
} ao_track_attr_param, *ao_track_attr_param_p;

typedef struct {
    hi_handle h_track;
    ao_track_fade fade;
} ao_track_fade_param, *ao_track_fade_param_p;

typedef struct {
    hi_handle h_track;
    hi_ao_gain track_gain;
} ao_track_weight_param, *ao_track_weight_param_p;

typedef struct {
    hi_handle h_track;
    hi_ao_abs_gain track_abs_gain;
} ao_track_abs_gain_param, *ao_track_abs_gain_param_p;

typedef struct {
    hi_handle h_track;
    hi_u32 latency_ms;
} ao_track_fifo_latency, *ao_track_fifo_latency_p;

typedef struct {
    hi_handle h_track;
    hi_ao_preci_gain preci_gain;
} ao_track_prescale_param, *ao_track_prescale_param_p;

typedef struct {
    hi_handle h_track;
    hi_bool mute;
} ao_track_mute_param, *ao_track_mute_param_p;

typedef struct {
    hi_handle h_track;
    hi_track_mode mode;
} ao_track_channel_mode_param, *ao_track_channel_mode_param_p;

typedef struct {
    hi_handle h_track;
    ao_frame ao_frame;
} ao_track_send_data_param, *ao_track_send_data_param_p;

typedef struct {
    hi_handle h_track;
    hi_u32 aip_reg_addr;
    hi_u32 bit_per_sample;
    hi_u32 sample_rate;
    hi_u32 channels;
} ao_track_mmap_param, *ao_track_mmap_param_p;

typedef struct {
    hi_handle h_track;
    hi_ao_speed speed;
} ao_track_speed_adjust_param, *ao_track_speed_adjust_param_p;

typedef struct {
    hi_handle h_track;
    hi_u32 delay_ms;
} ao_track_delay_ms_param, *ao_track_delay_ms_param_p;

typedef struct {
    hi_handle h_track;
    hi_bool empty;
} ao_track_buf_empty_param, *ao_track_buf_empty_param_p;

typedef struct {
    hi_handle h_track;
    hi_bool eos_flag;
} ao_track_eos_flag_param, *ao_track_eos_flag_param_p;

typedef struct {
    hi_handle h_track;
    hi_handle h_ai;
} ao_track_att_ai_param, *ao_track_att_ai_param_p;

typedef struct {
    hi_handle h_track;
    hi_u32 threshold_ms;
} ao_track_resume_threshold, *ao_track_resume_threshold_p;

typedef struct {
    ao_snd_id sound;
    hi_bool mute;
} ao_snd_all_cast_mute_param, *ao_snd_all_cast_mute_param_p;

typedef struct {
    hi_handle h_cast;
    hi_bool mute;
} ao_cast_mute_param, *ao_cast_mute_param_p;

typedef struct {
    hi_handle h_cast;
    hi_ao_abs_gain cast_abs_gain;
} ao_cast_abs_gain_param, *ao_cast_abs_gain_param_p;

typedef struct {
    ao_snd_id sound;
    hi_handle h_cast;
    hi_u32 req_size;
    hi_ao_cast_attr cast_attr;
} ao_cast_create_param, *ao_cast_create_param_p;

typedef struct {
    hi_handle h_cast;
    hi_u64 user_virt_addr;
    hi_u64 kernel_virt_addr;
    hi_u64 phy_addr;
    hi_u32 buf_size;
    hi_s32 map_fd;

    hi_u32 frame_bytes;
    hi_u32 frame_samples;
    hi_u32 channels;
    hi_s32 bit_per_sample;
} ao_cast_info_param, *ao_cast_info_param_p;

typedef struct {
    hi_handle h_cast;
    hi_bool cast_enable;
} ao_cast_enable_param, *ao_cast_enable_param_p;

typedef struct {
    hi_handle h_cast;
    hi_u32 frame_bytes;
    hi_u32 sample_bytes;

    hi_u32 data_offset;
    ao_frame ao_frame;
} ao_cast_data_param, *ao_cast_data_param_p;

typedef struct {
    ao_snd_id sound;
    hi_u32 aef_id;
    hi_u32 aef_proc_addr;
    ao_aef_attr aef_attr;
} ao_snd_att_aef_param, *ao_snd_att_aef_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_bool bypass;
} ao_snd_aef_bypass_param, *ao_snd_aef_bypass_param_p;

typedef struct {
    ao_snd_id sound;
    hi_u32 count;
} ao_snd_get_xrun_param, *ao_snd_get_xrun_param_p;

typedef struct {
    ao_snd_id sound;
    ao_aef_buf_attr aef_buf;
} ao_snd_aef_buf_param, *ao_snd_aef_buf_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_u32 latency_ms;
} ao_snd_set_low_latency_param, *ao_snd_set_low_latency_param_p;

typedef struct {
    hi_handle h_track;
    hi_bool enable;
} ao_track_priority_param, *ao_track_priority_param_p;

typedef struct {
    hi_handle h_track;
    hi_bool enable;
} ao_track_fifo_bypass_param, *ao_track_fifo_bypass_param_p;

typedef struct {
    hi_handle h_track;
    hi_u64 user_virt_addr;
    hi_u64 phy_addr;
} ao_track_info_param, *ao_track_info_param_p;

typedef struct {
    ao_snd_id sound;
    ao_debug_attr debug_attr;
} ao_snd_debug_param, *ao_snd_debug_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_handle h_track;
    hi_bool enable;
} ao_snd_track_duplicate_param, *ao_snd_track_duplicate_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_preci_gain preci_gain;
} ao_snd_set_alsa_prescale, *ao_snd_set_alsa_prescale_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_port out_port;
    hi_u32 delay_ms;
} ao_snd_op_delay_param, *ao_snd_op_delay_param_p;

#ifdef __DPT__
typedef struct {
    hi_handle h_track;
    hi_unf_snd_source source;
} ao_track_source_param, *ao_track_source_param_p;

typedef struct {
    ao_snd_id sound;
    hi_ao_track_config track_config;
} ao_snd_track_config_param, *ao_snd_track_config_param_p;
#endif

typedef struct {
    ao_snd_id sound;
    hi_u32 delay_ms;
} ao_snd_set_ext_delay_ms_param, *ao_snd_set_ext_delay_ms_param_p;

typedef struct {
    ao_snd_id sound;
    snd_port_kernel_attr port_k_attr[HI_AO_OUTPUT_PORT_MAX];
} ao_snd_dma_create_param, *ao_snd_dma_create_param_p;

typedef struct {
    ao_snd_id sound;
} ao_snd_dma_destory_param, *ao_snd_dma_destory_param_p;

typedef struct {
    ao_snd_id sound;
    hi_sample_rate sample_rate;
} ao_snd_dma_set_sample_rate_param, *ao_snd_dma_set_sample_rate_param_p;

enum {
    CMD_ID_AO_SND_GETDEFOPENATTR,
    CMD_ID_AO_SND_OPEN,
    CMD_ID_AO_SND_CLOSE,

    CMD_ID_AO_SND_SETMUTE,
    CMD_ID_AO_SND_GETMUTE,
    CMD_ID_AO_SND_SETVOLUME,
    CMD_ID_AO_SND_GETVOLUME,

    CMD_ID_AO_SND_SETOUTPUTMODE,
    CMD_ID_AO_SND_GETOUTPUTMODE,

    CMD_ID_AO_SND_SETSAMPLERATE,
    CMD_ID_AO_SND_GETSAMPLERATE,

    CMD_ID_AO_SND_SETTRACKMODE,
    CMD_ID_AO_SND_GETTRACKMODE,

    CMD_ID_AO_SND_SETSMARTVOLUME,
    CMD_ID_AO_SND_GETSMARTVOLUME,

    CMD_ID_AO_SND_SETSPDIFSCMSMODE,
    CMD_ID_AO_SND_GETSPDIFSCMSMODE,
    CMD_ID_AO_SND_SETSPDIFCATEGORYCODE,
    CMD_ID_AO_SND_GETSPDIFCATEGORYCODE,

    CMD_ID_AO_SND_SETALLTRACKMUTE,
    CMD_ID_AO_SND_GETALLTRACKMUTE,

    CMD_ID_AO_SND_SETADACENABLE,
    CMD_ID_AO_SND_GETADACENABLE,

    CMD_ID_AO_SND_ATTACHTRACK,
    CMD_ID_AO_SND_DETACHTRACK,

    CMD_ID_AO_SND_SETPRECIVOL,
    CMD_ID_AO_SND_GETPRECIVOL,

#ifdef __DPT__
    CMD_ID_AO_SND_SETALSAPRESCALE,
#endif

    CMD_ID_AO_SND_SETBALANCE,
    CMD_ID_AO_SND_GETBALANCE,

    CMD_ID_AO_SND_SETARCENABLE,
    CMD_ID_AO_SND_GETARCENABLE,
    CMD_ID_AO_SND_SETARCMODE,
    CMD_ID_AO_SND_GETARCMODE,
    CMD_ID_AO_SND_SETARCCAP,
    CMD_ID_AO_SND_GETARCCAP,

    CMD_ID_AO_SND_SETAVCATTR,
    CMD_ID_AO_SND_GETAVCATTR,
    CMD_ID_AO_SND_SETAVCENABLE,
    CMD_ID_AO_SND_GETAVCENABLE,

    CMD_ID_AO_SND_SETGEQATTR,
    CMD_ID_AO_SND_GETGEQATTR,
    CMD_ID_AO_SND_SETGEQENABLE,
    CMD_ID_AO_SND_GETGEQENABLE,
    CMD_ID_AO_SND_SETGEQGAIN,
    CMD_ID_AO_SND_GETGEQGAIN,

    CMD_ID_AO_SND_SETDRCENABLE,
    CMD_ID_AO_SND_GETDRCENABLE,
    CMD_ID_AO_SND_SETDRCATTR,
    CMD_ID_AO_SND_GETDRCATTR,

    CMD_ID_AO_SND_SETPEQATTR,
    CMD_ID_AO_SND_GETPEQATTR,
    CMD_ID_AO_SND_SETPEQENABLE,
    CMD_ID_AO_SND_GETPEQENABLE,

    CMD_ID_AO_SND_ATTACHAEF,
    CMD_ID_AO_SND_DETACHAEF,
    CMD_ID_AO_SND_SETAEFBYPASS,
    CMD_ID_AO_SND_GETAEFBYPASS,
    CMD_ID_AO_SND_GETAEFBUFATTR,
    CMD_ID_AO_SND_GETDEBUGPARAM,

#ifdef __DPT__
    CMD_ID_AO_SND_GETTRACKINFO,
    CMD_ID_AO_SND_DUPLICATETRACK,
    CMD_ID_AO_SND_TRACKCONFIGINIT,
#endif

    CMD_ID_AO_SND_SETADOUTPUTENABLE,
    CMD_ID_AO_SND_GETADOUTPUTENABLE,

    CMD_ID_AO_SND_GETXRUNCOUNT,

    CMD_ID_AO_SND_SETSOUNDDELAY,
    CMD_ID_AO_SND_GETSOUNDDELAY,

    CMD_ID_AO_SND_SETLOWLATENCY,
    CMD_ID_AO_SND_GETLOWLATENCY,

    CMD_ID_AO_SND_SETEXTDELAYMS,

    CMD_ID_AO_SND_DMACREATE,
    CMD_ID_AO_SND_DMADESTORY,
    CMD_ID_AO_SND_DMASETSAMPLERATE,

    CMD_ID_AO_SND_SETCONOUTPUTENABLE,
    CMD_ID_AO_SND_GETCONOUTPUTENABLE,

    CMD_ID_AO_SND_CREATESB,
    CMD_ID_AO_SND_DESTROYSB,

    CMD_ID_AO_SND_SETRENDERPARAM,
    CMD_ID_AO_SND_GETRENDERPARAM,

    CMD_ID_AO_SND_SETOUTPUTATMOSENABLE,
    CMD_ID_AO_SND_GETOUTPUTATMOSENABLE,

    CMD_ID_AO_SND_SETCONOUTPUTSTATUS,
    CMD_ID_AO_SND_GETCONOUTPUTSTATUS,

    CMD_ID_AO_SND_SETOUTPUTLATENCYMODE,
    CMD_ID_AO_SND_GETOUTPUTLATENCYMODE,

    CMD_ID_AO_SND_SETALLTRACKPRESCALE,
    CMD_ID_AO_SND_GETALLTRACKPRESCALE,

    /* AO track ioctl */
    CMD_ID_AO_TRACK_GETDEFATTR,
    CMD_ID_AO_TRACK_CREATE,
    CMD_ID_AO_TRACK_DESTROY,

    CMD_ID_AO_TRACK_START,
    CMD_ID_AO_TRACK_STOP,
    CMD_ID_AO_TRACK_PAUSE,
    CMD_ID_AO_TRACK_FLUSH,

    CMD_ID_AO_TRACK_SENDDATA,
    CMD_ID_AO_TRACK_SETWEITHT,
    CMD_ID_AO_TRACK_GETWEITHT,
    CMD_ID_AO_TRACK_SETSPEEDADJUST,
    CMD_ID_AO_TRACK_GETDELAYMS,
    CMD_ID_AO_TRACK_ISBUFEMPTY,
    CMD_ID_AO_TRACK_SETEOSFLAG,

    CMD_ID_AO_TRACK_GETATTR,
    CMD_ID_AO_TRACK_SETATTR,

    CMD_ID_AO_TRACK_ATTACHAI,
    CMD_ID_AO_TRACK_DETACHAI,

    CMD_ID_AO_TRACK_SETABSGAIN,
    CMD_ID_AO_TRACK_GETABSGAIN,

    CMD_ID_AO_TRACK_SETMUTE,
    CMD_ID_AO_TRACK_GETMUTE,
    CMD_ID_AO_TRACK_SETCHANNELMODE,
    CMD_ID_AO_TRACK_GETCHANNELMODE,
    CMD_ID_AO_TRACK_SETOTHERMUTE,

    CMD_ID_AO_TRACK_SETPRESCALE,
    CMD_ID_AO_TRACK_GETPRESCALE,

#ifdef __DPT__
    CMD_ID_AO_TRACK_SETSOURCE,
#endif

    CMD_ID_AO_TRACK_SETFIFOLATENCY,
    CMD_ID_AO_TRACK_GETFIFOLATENCY,

    CMD_ID_AO_TRACK_SETFIFOBYPASS,
    CMD_ID_AO_TRACK_MMAP,
    CMD_ID_AO_TRACK_SETPRIORITY,
    CMD_ID_AO_TRACK_GETPRIORITY,

    CMD_ID_AO_TRACK_SETRESUMETHRESHOLD,
    CMD_ID_AO_TRACK_GETRESUMETHRESHOLD,

    CMD_ID_AO_TRACK_SETFADEATTR,
    CMD_ID_AO_TRACK_SETINFO,
    CMD_ID_AO_TRACK_GETINFO,

    CMD_ID_AO_CAST_GETDEFATTR,
    CMD_ID_AO_CAST_CREATE,
    CMD_ID_AO_CAST_DESTROY,
    CMD_ID_AO_CAST_SETENABLE,
    CMD_ID_AO_CAST_GETENABLE,
    CMD_ID_AO_CAST_GETINFO,
    CMD_ID_AO_CAST_SETINFO,

    CMD_ID_AO_CAST_ACQUIREFRAME,
    CMD_ID_AO_CAST_RELEASEFRAME,

    CMD_ID_AO_CAST_SETABSGAIN,
    CMD_ID_AO_CAST_GETABSGAIN,

    CMD_ID_AO_CAST_SETMUTE,
    CMD_ID_AO_CAST_GETMUTE,
    CMD_ID_AO_SND_SETALLCASTMUTE,
    CMD_ID_AO_SND_GETALLCASTMUTE,

    CMD_ID_AO_MAX,
};

/*
 * AO command code definition
 */
/* AO SND CMD */
#define CMD_AO_GETSNDDEFOPENATTR \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETDEFOPENATTR, ao_snd_open_default_param)
#define CMD_AO_SND_OPEN _IOWR(HI_ID_AO, CMD_ID_AO_SND_OPEN, ao_snd_open_param)
#define CMD_AO_SND_CLOSE _IOW(HI_ID_AO, CMD_ID_AO_SND_CLOSE, ao_snd_id)
#define CMD_AO_SND_SETMUTE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETMUTE, ao_snd_mute_param)
#define CMD_AO_SND_GETMUTE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETMUTE, ao_snd_mute_param)
#define CMD_AO_SND_SETVOLUME _IOW(HI_ID_AO, CMD_ID_AO_SND_SETVOLUME, ao_snd_volume_param)
#define CMD_AO_SND_GETVOLUME _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETVOLUME, ao_snd_volume_param)

#define CMD_AO_SND_SETOUTPUTMODE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETOUTPUTMODE, ao_snd_output_mode_param)
#define CMD_AO_SND_GETOUTPUTMODE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETOUTPUTMODE, ao_snd_output_mode_param)

#define CMD_AO_SND_SETSAMPLERATE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETSAMPLERATE, ao_snd_sample_rate_param)
#define CMD_AO_SND_GETSAMPLERATE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETSAMPLERATE, ao_snd_sample_rate_param)
#define CMD_AO_SND_SETTRACKMODE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETTRACKMODE, ao_snd_track_mode_param)
#define CMD_AO_SND_GETTRACKMODE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETTRACKMODE, ao_snd_track_mode_param)

#define CMD_AO_SND_SETSMARTVOLUME _IOW(HI_ID_AO, CMD_ID_AO_SND_SETSMARTVOLUME, ao_snd_smart_volume_param)
#define CMD_AO_SND_GETSMARTVOLUME _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETSMARTVOLUME, ao_snd_smart_volume_param)

#define CMD_AO_SND_SETSPDIFSCMSMODE \
    _IOW(HI_ID_AO, CMD_ID_AO_SND_SETSPDIFSCMSMODE, ao_snd_spdif_scms_mode_param)
#define CMD_AO_SND_GETSPDIFSCMSMODE \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETSPDIFSCMSMODE, ao_snd_spdif_scms_mode_param)
#define CMD_AO_SND_SETSPDIFCATEGORYCODE \
    _IOW(HI_ID_AO, CMD_ID_AO_SND_SETSPDIFCATEGORYCODE, ao_snd_spdif_category_code_param)
#define CMD_AO_SND_GETSPDIFCATEGORYCODE \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETSPDIFCATEGORYCODE, ao_snd_spdif_category_code_param)

#define CMD_AO_SND_SETALLTRACKMUTE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETALLTRACKMUTE, ao_snd_all_track_mute_param)
#define CMD_AO_SND_GETALLTRACKMUTE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETALLTRACKMUTE, ao_snd_all_track_mute_param)

#define CMD_AO_SND_SETADACENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_SETADACENABLE, ao_snd_adac_enable_param)
#define CMD_AO_SND_GETADACENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETADACENABLE, ao_snd_adac_enable_param)

#define CMD_AO_SND_ATTACHTRACK _IOW(HI_ID_AO, CMD_ID_AO_SND_ATTACHTRACK, hi_handle)
#define CMD_AO_SND_DETACHTRACK _IOW(HI_ID_AO, CMD_ID_AO_SND_DETACHTRACK, hi_handle)

#define CMD_AO_SND_SETPRECIVOL _IOW(HI_ID_AO, CMD_ID_AO_SND_SETPRECIVOL, ao_snd_preci_volume_param)
#define CMD_AO_SND_GETPRECIVOL _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETPRECIVOL, ao_snd_preci_volume_param)

#define CMD_AO_SND_SETALSAPRESCALE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETALSAPRESCALE, ao_snd_set_alsa_prescale)

#define CMD_AO_SND_SETBALANCE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETBALANCE, ao_snd_balance_param)
#define CMD_AO_SND_GETBALANCE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETBALANCE, ao_snd_balance_param)

/* ARC */
#define CMD_AO_SND_SETARCENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETARCENABLE, ao_snd_arc_enable_param)
#define CMD_AO_SND_GETARCENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETARCENABLE, ao_snd_arc_enable_param)
#define CMD_AO_SND_SETARCMODE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETARCMODE, ao_snd_arc_mode_param)
#define CMD_AO_SND_GETARCMODE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETARCMODE, ao_snd_arc_mode_param)
#define CMD_AO_SND_SETARCCAP _IOW(HI_ID_AO, CMD_ID_AO_SND_SETARCCAP, ao_snd_arc_cap_param)
#define CMD_AO_SND_GETARCCAP _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETARCCAP, ao_snd_arc_cap_param)

#define CMD_AO_SND_SETAVCATTR _IOW(HI_ID_AO, CMD_ID_AO_SND_SETAVCATTR, ao_snd_avc_param)
#define CMD_AO_SND_GETAVCATTR _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETAVCATTR, ao_snd_avc_param)
#define CMD_AO_SND_SETAVCENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETAVCENABLE, ao_snd_avc_enable)
#define CMD_AO_SND_GETAVCENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETAVCENABLE, ao_snd_avc_enable)

#define CMD_AO_SND_SETGEQATTR _IOW(HI_ID_AO, CMD_ID_AO_SND_SETGEQATTR, ao_snd_geq_param)
#define CMD_AO_SND_GETGEQATTR _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETGEQATTR, ao_snd_geq_param)
#define CMD_AO_SND_SETGEQENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETGEQENABLE, ao_snd_eq_enable)
#define CMD_AO_SND_GETGEQENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETGEQENABLE, ao_snd_eq_enable)
#define CMD_AO_SND_SETGEQGAIN _IOW(HI_ID_AO, CMD_ID_AO_SND_SETGEQGAIN, ao_snd_geq_gain)
#define CMD_AO_SND_GETGEQGAIN _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETGEQGAIN, ao_snd_geq_gain)

#define CMD_AO_SND_SETDRCENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETDRCENABLE, ao_snd_drc_enable)
#define CMD_AO_SND_GETDRCENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETDRCENABLE, ao_snd_drc_enable)
#define CMD_AO_SND_SETDRCATTR _IOW(HI_ID_AO, CMD_ID_AO_SND_SETDRCATTR, ao_snd_drc_param)
#define CMD_AO_SND_GETDRCATTR _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETDRCATTR, ao_snd_drc_param)

#define CMD_AO_SND_SETPEQATTR _IOW(HI_ID_AO, CMD_ID_AO_SND_SETPEQATTR, ao_snd_peq_param)
#define CMD_AO_SND_GETPEQATTR _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETPEQATTR, ao_snd_peq_param)
#define CMD_AO_SND_SETPEQENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETPEQENABLE, ao_snd_eq_enable)
#define CMD_AO_SND_GETPEQENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETPEQENABLE, ao_snd_eq_enable)

#define CMD_AO_SND_ATTACHAEF _IOWR(HI_ID_AO, CMD_ID_AO_SND_ATTACHAEF, ao_snd_att_aef_param)
#define CMD_AO_SND_DETACHAEF _IOW(HI_ID_AO, CMD_ID_AO_SND_DETACHAEF, ao_snd_att_aef_param)
#define CMD_AO_SND_SETAEFBYPASS _IOW(HI_ID_AO, CMD_ID_AO_SND_SETAEFBYPASS, ao_snd_aef_bypass_param)
#define CMD_AO_SND_GETAEFBYPASS _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETAEFBYPASS, ao_snd_aef_bypass_param)
#define CMD_AO_SND_GETAEFBUFATTR _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETAEFBUFATTR, ao_snd_aef_buf_param)
#define CMD_AO_SND_GETTRACKINFO _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETTRACKINFO, ao_snd_track_info_param)
/* debug */
#define CMD_AO_SND_GETDEBUGPARAM _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETDEBUGPARAM, ao_snd_debug_param)

#define CMD_AO_SND_SETADOUTPUTENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETADOUTPUTENABLE, ao_snd_ad_output_enable)
#define CMD_AO_SND_GETADOUTPUTENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETADOUTPUTENABLE, ao_snd_ad_output_enable)

#define CMD_AO_SND_DUPLICATETRACK _IOWR(HI_ID_AO, CMD_ID_AO_SND_DUPLICATETRACK, ao_snd_track_duplicate_param)

#define CMD_AO_SND_SETALLCASTMUTE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETALLCASTMUTE, ao_snd_all_cast_mute_param)
#define CMD_AO_SND_GETALLCASTMUTE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETALLCASTMUTE, ao_snd_all_cast_mute_param)

#define CMD_AO_SND_TRACKCONFIGINIT _IOW(HI_ID_AO, CMD_ID_AO_SND_TRACKCONFIGINIT, ao_snd_track_config_param)

#define CMD_AO_SND_GETXRUNCOUNT _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETXRUNCOUNT, ao_snd_get_xrun_param)

#define CMD_AO_SND_SETSOUNDDELAY _IOW(HI_ID_AO, CMD_ID_AO_SND_SETSOUNDDELAY, ao_snd_op_delay_param)
#define CMD_AO_SND_GETSOUNDDELAY _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETSOUNDDELAY, ao_snd_op_delay_param)

#define CMD_AO_SND_SETLOWLATENCY _IOW(HI_ID_AO, CMD_ID_AO_SND_SETLOWLATENCY, ao_snd_set_low_latency_param)
#define CMD_AO_SND_GETLOWLATENCY _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETLOWLATENCY, ao_snd_set_low_latency_param)

#define CMD_AO_SND_SETEXTDELAYMS _IOWR(HI_ID_AO, CMD_ID_AO_SND_SETEXTDELAYMS, ao_snd_set_ext_delay_ms_param)
#define CMD_AO_SND_DMACREATE _IOWR(HI_ID_AO, CMD_ID_AO_SND_DMACREATE, ao_snd_dma_create_param)
#define CMD_AO_SND_DMADESTORY _IOWR(HI_ID_AO, CMD_ID_AO_SND_DMADESTORY, ao_snd_dma_destory_param)
#define CMD_AO_SND_DMASETSAMPLERATE _IOWR(HI_ID_AO, CMD_ID_AO_SND_DMASETSAMPLERATE, ao_snd_dma_set_sample_rate_param)

#define CMD_AO_SND_SETCONOUTPUTENABLE _IOW(HI_ID_AO, CMD_ID_AO_SND_SETCONOUTPUTENABLE, ao_snd_con_output_enable)
#define CMD_AO_SND_GETCONOUTPUTENABLE _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETCONOUTPUTENABLE, ao_snd_con_output_enable)

#define CMD_AO_SND_CREATESB _IOWR(HI_ID_AO, CMD_ID_AO_SND_CREATESB, ao_snd_isb_param)
#define CMD_AO_SND_DESTROYSB _IOW(HI_ID_AO, CMD_ID_AO_SND_DESTROYSB, ao_snd_isb_param)

#define CMD_AO_SND_SETRENDERPARAM _IOW(HI_ID_AO, CMD_ID_AO_SND_SETRENDERPARAM, ao_snd_render_param)
#define CMD_AO_SND_GETRENDERPARAM _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETRENDERPARAM, ao_snd_render_param)

#define CMD_AO_SND_SETOUTPUTATMOSENABLE \
    _IOW(HI_ID_AO, CMD_ID_AO_SND_SETOUTPUTATMOSENABLE, ao_snd_output_atmos_enable)
#define CMD_AO_SND_GETOUTPUTATMOSENABLE \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETOUTPUTATMOSENABLE, ao_snd_output_atmos_enable)

#define CMD_AO_SND_SETCONOUTPUTSTATUS \
    _IOW(HI_ID_AO, CMD_ID_AO_SND_SETCONOUTPUTSTATUS, ao_snd_con_output_enable)
#define CMD_AO_SND_GETCONOUTPUTSTATUS \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETCONOUTPUTSTATUS, ao_snd_con_output_enable)

#define CMD_AO_SND_SETOUTPUTLATENCYMODE \
    _IOW(HI_ID_AO, CMD_ID_AO_SND_SETOUTPUTLATENCYMODE, ao_snd_output_latency_mode)
#define CMD_AO_SND_GETOUTPUTLATENCYMODE \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETOUTPUTLATENCYMODE, ao_snd_output_latency_mode)

#define CMD_AO_SND_SETALLTRACKPRESCALE \
    _IOW(HI_ID_AO, CMD_ID_AO_SND_SETALLTRACKPRESCALE, ao_snd_all_track_prescale_param)
#define CMD_AO_SND_GETALLTRACKPRESCALE \
    _IOWR(HI_ID_AO, CMD_ID_AO_SND_GETALLTRACKPRESCALE, ao_snd_all_track_prescale_param)

/* AO track CMD */
#define CMD_AO_TRACK_GETDEFATTR _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETDEFATTR, hi_ao_track_attr)
#define CMD_AO_TRACK_CREATE _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_CREATE, ao_track_create_param)
#define CMD_AO_TRACK_DESTROY _IOW(HI_ID_AO, CMD_ID_AO_TRACK_DESTROY, hi_handle)

#define CMD_AO_TRACK_START _IOW(HI_ID_AO, CMD_ID_AO_TRACK_START, hi_handle)
#define CMD_AO_TRACK_STOP _IOW(HI_ID_AO, CMD_ID_AO_TRACK_STOP, hi_handle)
#define CMD_AO_TRACK_PAUSE _IOW(HI_ID_AO, CMD_ID_AO_TRACK_PAUSE, hi_handle)
#define CMD_AO_TRACK_FLUSH _IOW(HI_ID_AO, CMD_ID_AO_TRACK_FLUSH, hi_handle)

#define CMD_AO_TRACK_SENDDATA _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SENDDATA, ao_track_send_data_param)
#define CMD_AO_TRACK_SETWEITHT _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETWEITHT, ao_track_weight_param)
#define CMD_AO_TRACK_GETWEITHT _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETWEITHT, ao_track_weight_param)
#define CMD_AO_TRACK_SETSPEEDADJUST _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETSPEEDADJUST, ao_track_speed_adjust_param)
#define CMD_AO_TRACK_GETDELAYMS _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETDELAYMS, ao_track_delay_ms_param)
#define CMD_AO_TRACK_ISBUFEMPTY _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_ISBUFEMPTY, ao_track_buf_empty_param)
#define CMD_AO_TRACK_SETEOSFLAG _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETEOSFLAG, ao_track_eos_flag_param)
#define CMD_AO_TRACK_GETATTR _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETATTR, ao_track_attr_param)
#define CMD_AO_TRACK_SETATTR _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETATTR, ao_track_attr_param)

#define CMD_AO_TRACK_ATTACHAI _IOW(HI_ID_AO, CMD_ID_AO_TRACK_ATTACHAI, ao_track_att_ai_param)
#define CMD_AO_TRACK_DETACHAI _IOW(HI_ID_AO, CMD_ID_AO_TRACK_DETACHAI, ao_track_att_ai_param)
#define CMD_AO_TRACK_SETABSGAIN _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETABSGAIN, ao_track_abs_gain_param)
#define CMD_AO_TRACK_GETABSGAIN _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETABSGAIN, ao_track_abs_gain_param)

#define CMD_AO_TRACK_SETMUTE _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETMUTE, ao_track_mute_param)
#define CMD_AO_TRACK_GETMUTE _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETMUTE, ao_track_mute_param)
#define CMD_AO_TRACK_SETCHANNELMODE _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETCHANNELMODE, ao_track_channel_mode_param)
#define CMD_AO_TRACK_GETCHANNELMODE _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETCHANNELMODE, ao_track_channel_mode_param)

#define CMD_AO_TRACK_SETPRESCALE _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETPRESCALE, ao_track_prescale_param)
#define CMD_AO_TRACK_GETPRESCALE _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETPRESCALE, ao_track_prescale_param)

#ifdef __DPT__
#define CMD_AO_TRACK_SETSOURCE _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETSOURCE, ao_track_source_param)
#endif

#define CMD_AO_TRACK_SETFIFOLATENCY _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETFIFOLATENCY, ao_track_fifo_latency)
#define CMD_AO_TRACK_GETFIFOLATENCY _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETFIFOLATENCY, ao_track_fifo_latency)

/* karaoke */
#define CMD_AO_TRACK_SETFIFOBYPASS _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETFIFOBYPASS, ao_track_fifo_bypass_param)
#define CMD_AO_TRACK_MMAP _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_MMAP, ao_track_mmap_param)
#define CMD_AO_TRACK_SETPRIORITY _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETPRIORITY, ao_track_priority_param)
#define CMD_AO_TRACK_GETPRIORITY _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETPRIORITY, ao_track_priority_param)

#define CMD_AO_TRACK_SETRESUMETHRESHOLD _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETRESUMETHRESHOLD, ao_track_resume_threshold)
#define CMD_AO_TRACK_GETRESUMETHRESHOLD _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETRESUMETHRESHOLD, ao_track_resume_threshold)

#define CMD_AO_TRACK_SETFADEATTR _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_SETFADEATTR, ao_track_fade_param)
#define CMD_AO_TRACK_SETINFO _IOW(HI_ID_AO, CMD_ID_AO_TRACK_SETINFO, ao_track_info_param)
#define CMD_AO_TRACK_GETINFO _IOWR(HI_ID_AO, CMD_ID_AO_TRACK_GETINFO, ao_track_info_param)

/* AO cast CMD */
#define CMD_AO_CAST_GETDEFATTR _IOWR(HI_ID_AO, CMD_ID_AO_CAST_GETDEFATTR, hi_ao_cast_attr)
#define CMD_AO_CAST_CREATE _IOWR(HI_ID_AO, CMD_ID_AO_CAST_CREATE, ao_cast_create_param)
#define CMD_AO_CAST_DESTROY _IOW(HI_ID_AO, CMD_ID_AO_CAST_DESTROY, hi_handle)
#define CMD_AO_CAST_SETENABLE _IOW(HI_ID_AO, CMD_ID_AO_CAST_SETENABLE, ao_cast_enable_param)
#define CMD_AO_CAST_GETENABLE _IOWR(HI_ID_AO, CMD_ID_AO_CAST_GETENABLE, ao_cast_enable_param)
#define CMD_AO_CAST_GETINFO _IOWR(HI_ID_AO, CMD_ID_AO_CAST_GETINFO, ao_cast_info_param)
#define CMD_AO_CAST_SETINFO _IOW(HI_ID_AO, CMD_ID_AO_CAST_SETINFO, ao_cast_info_param)
#define CMD_AO_CAST_ACQUIREFRAME _IOWR(HI_ID_AO, CMD_ID_AO_CAST_ACQUIREFRAME, ao_cast_data_param)
#define CMD_AO_CAST_RELEASEFRAME _IOW(HI_ID_AO, CMD_ID_AO_CAST_RELEASEFRAME, ao_cast_data_param)
#define CMD_AO_CAST_SETABSGAIN _IOW(HI_ID_AO, CMD_ID_AO_CAST_SETABSGAIN, ao_cast_abs_gain_param)
#define CMD_AO_CAST_GETABSGAIN _IOWR(HI_ID_AO, CMD_ID_AO_CAST_GETABSGAIN, ao_cast_abs_gain_param)
#define CMD_AO_CAST_SETMUTE _IOW(HI_ID_AO, CMD_ID_AO_CAST_SETMUTE, ao_cast_mute_param)
#define CMD_AO_CAST_GETMUTE _IOWR(HI_ID_AO, CMD_ID_AO_CAST_GETMUTE, ao_cast_mute_param)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

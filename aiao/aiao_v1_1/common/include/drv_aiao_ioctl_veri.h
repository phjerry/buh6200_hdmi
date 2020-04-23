/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao cbb verification driver head file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AIAO_IOCTL_VERI_H__
#define __DRV_AIAO_IOCTL_VERI_H__

#include "hal_aiao_common.h"

#define AIAO_VERI_PROC_NAME "aiao_veri"

typedef struct {
    hi_u32 crg_skip;
} aiao_sys_crg;

typedef struct {
    hi_u32 hdmi;

    /* the origin of sound, suggestion set HI_UNF_SND_INTERFACE_I2S, the parameter need consistent with ao input */
    /* CNcomment: HDMI音频来源, 建议HI_UNF_SND_INTERFACE_I2S, 此参数需要与AO输入保持一致 */
    hi_u32 sound_intf;

    hi_u32 channels;

    /* the samplerate of audio,this parameter consistent with AO config */
    /* CNcomment: PCM音频采样率,此参数需要与AO的配置保持一致 */
    hi_sample_rate sample_rate;

    /* the audio bit depth, defualt 16, this parameter consistent with AO config */
    /* CNcomment: 音频位宽，默认为16，此参数需要与AO的配置保持一致 */
    hi_bit_depth bit_depth;

    hi_u32 audio_format;
} aiao_hdmi_audio_atrr;

enum {
    CMD_ID_AIAO_GET_CAPABILITY,
    CMD_ID_AIAO_SET_SYSCRG,
    CMD_ID_AIAO_RW_REGISTER,
    CMD_ID_AIAO_PORT_OPEN,
    CMD_ID_AIAO_PORT_CLOSE,
    CMD_ID_AIAO_PORT_START,
    CMD_ID_AIAO_PORT_STOP,
    CMD_ID_AIAO_PORT_MUTE,
    CMD_ID_AIAO_PORT_TRACKMODE,
    CMD_ID_AIAO_PORT_VOLUME,
    CMD_ID_AIAO_PORT_GET_USERCONFIG,
    CMD_ID_AIAO_PORT_GET_STATUS,
    CMD_ID_AIAO_PORT_SEL_SPDIFOUTSOURCE,
    CMD_ID_AIAO_PORT_SET_SPDIFOUTPORT,
    CMD_ID_AIAO_PORT_SET_I2SDATASEL,

    CMD_ID_AIAO_PORT_READ_DATA,
    CMD_ID_AIAO_PORT_WRITE_DATA,
    CMD_ID_AIAO_PORT_PREPARE_DATA,
    CMD_ID_AIAO_PORT_QUERY_BUF,
    CMD_ID_AIAO_PORT_UPDATE_RPTR,
    CMD_ID_AIAO_PORT_UPDATE_WPTR,

    CMD_ID_AIAO_HDMI_SETAUDIO_ATTR,

#ifdef HI_AIAO_TIMER_SUPPORT
    CMD_ID_AIAO_TIMER_CREATE,
    CMD_ID_AIAO_TIMER_DESTROY,
    CMD_ID_AIAO_TIMER_SETATTR,
    CMD_ID_AIAO_TIMER_SETENABLE,
    CMD_ID_AIAO_TIMER_GETSTATUS,
#endif

    CMD_ID_AIAO_MAX
};

/* AIAO command code definition */
/* global */
#define CMD_AIAO_GET_CAPABILITY _IOWR(HI_ID_AIAO, CMD_ID_AIAO_GET_CAPABILITY, aiao_port_get_capability)
#define CMD_AIAO_SET_SYSCRG     _IOWR(HI_ID_AIAO, CMD_ID_AIAO_SET_SYSCRG, aiao_sys_crg)

/* global debug */
#define CMD_AIAO_RW_REGISTER _IOWR(HI_ID_AIAO, CMD_ID_AIAO_RW_REGISTER, aiao_dbg_reg)

/* aiao port */
#define CMD_AIAO_PORT_OPEN          _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_OPEN, aiao_port_open)
#define CMD_AIAO_PORT_CLOSE         _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_CLOSE, aiao_port_close)
#define CMD_AIAO_PORT_START         _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_START, aiao_port_start)
#define CMD_AIAO_PORT_STOP          _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_STOP, aiao_port_stop)
#define CMD_AIAO_PORT_MUTE          _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_MUTE, aiao_port_mute)
#define CMD_AIAO_PORT_TRACKMODE     _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_TRACKMODE, aiao_port_track_mode)
#define CMD_AIAO_PORT_VOLUME        _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_VOLUME, aiao_port_volume)

#define CMD_AIAO_PORT_GET_USERCONFIG \
    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_GET_USERCONFIG, aiao_port_get_user_config)
#define CMD_AIAO_PORT_GET_STATUS \
    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_GET_STATUS, aiao_port_get_status)
#define CMD_AIAO_PORT_SEL_SPDIFOUTSOURCE \
    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_SEL_SPDIFOUTSOURCE, aiao_port_select_spdif_source)
#define CMD_AIAO_PORT_SET_SPDIFOUTPORT \
    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_SET_SPDIFOUTPORT, aiao_port_set_spdif_out_port)
#define CMD_AIAO_PORT_SET_I2SDATASEL \
    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_SET_I2SDATASEL, aiao_port_i2s_data_sel)

#define CMD_AIAO_PORT_READ_DATA    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_READ_DATA, aiao_port_read_data)
#define CMD_AIAO_PORT_WRITE_DATA   _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_WRITE_DATA, aiao_port_write_data)
#define CMD_AIAO_PORT_PREPARE_DATA _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_PREPARE_DATA, aiao_port_write_data)
#define CMD_AIAO_PORT_QUERY_BUF    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_QUERY_BUF, aiao_port_buf_status)

#define CMD_AIAO_PORT_UPDATE_RPTR _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_UPDATE_RPTR, aiao_port_read_data)
#define CMD_AIAO_PORT_UPDATE_WPTR _IOWR(HI_ID_AIAO, CMD_ID_AIAO_PORT_UPDATE_WPTR, aiao_port_write_data)

/* hdmi tx */
#define CMD_AIAO_HDMI_SETAUDIO_ATTR _IOWR(HI_ID_AIAO, CMD_ID_AIAO_HDMI_SETAUDIO_ATTR, aiao_hdmi_audio_atrr)

#ifdef HI_AIAO_TIMER_SUPPORT
#define CMD_AIAO_TIMER_CREATE    _IOWR(HI_ID_AIAO, CMD_ID_AIAO_TIMER_CREATE, aiao_timer_create)
#define CMD_AIAO_TIMER_DESTROY   _IOWR(HI_ID_AIAO, CMD_ID_AIAO_TIMER_DESTROY, aiao_timer_destroy)
#define CMD_AIAO_TIMER_SETATTR   _IOW(HI_ID_AIAO, CMD_ID_AIAO_TIMER_SETATTR, aiao_timer_attr)
#define CMD_AIAO_TIMER_SETENABLE _IOW(HI_ID_AIAO, CMD_ID_AIAO_TIMER_SETENABLE, aiao_timer_enable)
#define CMD_AIAO_TIMER_GETSTATUS _IOWR(HI_ID_AIAO, CMD_ID_AIAO_TIMER_GETSTATUS, aiao_timer_status)
#endif

#endif  /* __DRV_AIAO_IOCTL_VERI_H__ */

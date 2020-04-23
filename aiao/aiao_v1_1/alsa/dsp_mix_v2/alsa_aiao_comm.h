/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao alsa driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __ALSA_AIAO_COMM_H__
#define __ALSA_AIAO_COMM_H__

#include <sound/soc.h>
#include <sound/pcm.h>
#ifdef HI_ALSA_AI_SUPPORT
#include "hal_aiao_common.h"
#endif
#include "drv_ao_func.h"

#include "hi_osal.h"

#define CONFIG_AIAO_ALSA_PROC_SUPPORT
#define CONFIG_ALSA_VOLUMN_SUPPORT
#define MUTE_FRAME_OUTPUT  /* with mute frame trigger */

/* for virtual AI */
/* #define MUTE_FRAME_INPUT */
#define AIAO_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 |  \
                    SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |  \
                    SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)
#define AIAO_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |  \
                      SNDRV_PCM_FMTBIT_S24_LE)
#define CMD_START 0xa
#define CMD_STOP  0xb
#define AI_RATES  SNDRV_PCM_RATE_48000

struct aiao_hwparams {
    unsigned int channels;   /* channels */
    unsigned int rate;       /* rate in hz */
    snd_pcm_format_t format; /* SNDRV_PCM_FORMAT_**/
    unsigned int frame_size;
    unsigned int buffer_bytes;
    snd_pcm_uframes_t period_size; /* period size */
    unsigned int period_bytes;
    unsigned int periods;          /* periods */
    snd_pcm_uframes_t buffer_size; /* buffer size */
};

/* private audio data */
struct hiaudio_data {
    hi_void *file;
    struct aiao_hwparams ao_hw_param;
    hi_void *vir_base_addr;  /* alsa dma buf */
    hi_void *phy_base_addr;  /* for debug */

    hi_handle h_timer_handle;  /* interrupt handle */
    hi_u32 snd_open;

    hi_handle track_id;
    osal_atomic atm_track_state;

    hi_u32 send_try_cnt;
    hi_u32 had_sent;
    hi_u32 discard_sent;
    hi_u32 send_fail_cnt;
    hi_u32 hw_pointer;  /* port read pointer */

    ao_frame ao_frame;
    hi_u32 poll_time_ns;
    hi_bool trigger_start_ok;
#ifdef HI_ALSA_HRTIMER_SUPPORT
    ktime_t hrtperoid;
    struct hrtimer hrt;
    struct snd_pcm_substream *substream;
#else
    hi_u32 isr_config;
    hi_bool timer_enable;
#endif

    struct workqueue_struct *workq;
    struct work_struct work;
    hi_u32 cmd;

    hi_u32 mute_frame_time;
    hi_u32 port_delay_ms;

#ifdef HI_ALSA_AI_SUPPORT
    struct aiao_hwparams sthwparam_ai;  /* ai hw params */
    int ai_handle;
    hi_void *cfile;
    unsigned int ack_c_cnt;
    unsigned int ai_writepos;
    unsigned int ai_readpos;
    unsigned int last_c_pos;
    unsigned int current_c_pos;
    aiao_isr_func *isr_proc;  /* ISR func for alsa */
    unsigned int isr_total_cnt_c;
#ifdef MUTE_FRAME_INPUT
    struct snd_pcm_substream *aisubstream;
    osal_atomic atm_record_state;
    hi_u32 ai_poll_time_ns;
    ktime_t aihrtperoid;
    struct hrtimer aihrt;
    struct tasklet_struct aitasklet;
#endif  /* MUTE_FRAME_INPUT */
#endif  /* HI_ALSA_AI_SUPPORT */

#ifdef CONFIG_AIAO_ALSA_PROC_SUPPORT
    struct snd_info_entry *entry;
#endif

#ifdef CONFIG_PM
    hi_u32 suspend_state;
#endif
};

#ifdef CONFIG_ALSA_VOLUMN_SUPPORT
struct hiaudio_sw_volume {
    signed int v_all;
    signed int v_hdmi;
    signed int v_spdif;
    signed int v_adac;
    signed int v_i2s;
    signed int v_capture;
    signed int v_mute;
};
#endif

#endif

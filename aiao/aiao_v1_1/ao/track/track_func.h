/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao track func header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __TRACK_FUNC_H__
#define __TRACK_FUNC_H__

#include "drv_ao_op.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AO_TRACK_LATENCYMS_DEFAULT 256

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
#define AO_TRACK_PCM_BUFSIZE_MS_MAX 512
#else
#define AO_TRACK_PCM_BUFSIZE_MS_MAX 256
#endif

#define AO_TRACK_LBR_BUFSIZE_MS_MAX     512
#define AO_TRACK_HBR_BUFSIZE_MS_MAX     512
#define AO_TRACK_BUF_EMPTY_THRESHOLD_MS 10  /* aip 5ms + engine 5ms */

#define AO_TRACK_PCM_BUFSIZE_BYTE_MAX (((192000 * 2 * sizeof(hi_u32)) / 1000) * AO_TRACK_PCM_BUFSIZE_MS_MAX)
#define AO_TRACK_LBR_BUFSIZE_BYTE_MAX (((192000 * 2 * sizeof(hi_u32)) / 1000) * AO_TRACK_PCM_BUFSIZE_MS_MAX)

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
#define AO_TRACK_HBR_BUFSIZE_BYTE_MAX (((192000 * 8 * sizeof(hi_u32)) / 1000) * AO_TRACK_PCM_BUFSIZE_MS_MAX)
#else
#define AO_TRACK_HBR_BUFSIZE_BYTE_MAX (((192000 * 2 * sizeof(hi_u32)) / 1000) * AO_TRACK_PCM_BUFSIZE_MS_MAX)
#endif

#define AO_TRACK_NORMAL_CHANNELNUM           2
#define AO_TRACK_MAX_CHANNELNUM              8
#define AO_TRACK_MUTILPCM_CHANNELNUM         8
#define AO_TRACK_MC_CHANNELNUM               8  /* max chn num is 8, add addtional 2ch data size */
#define AO_TRACK_BITDEPTH_LOW                16
#define AO_TRACK_BITDEPTH_HIGH               24
#define AO_TRACK_DEFATTR_BUFSIZE             1024
#define AO_TRACK_MASTER_DEFATTR_BUFLEVELMS   400
#define AO_TRACK_MASTER_MIN_BUFLEVELMS       30  /* 8k,aac max_pcm_out_sample_size:2048, 2048/8000 = 0.258s = 258ms */
#define AO_TRACK_MASTER_MAX_BUFLEVELMS       800
#define AO_TRACK_MASTER_DEFATTR_FADEINMS     16
#define AO_TRACK_MASTER_DEFATTR_FADEOUTMS    8
#define AO_TRACK_MASTER_DEFATTR_BUFSIZE      (4 * AO_TRACK_DEFATTR_BUFSIZE)
#define AO_TRACK_SLAVE_DEFATTR_BUFLEVELMS    400
#define AO_TRACK_SLAVE_DEFATTR_FADEINMS      1
#define AO_TRACK_SLAVE_DEFATTR_FADEOUTMS     1
#define AO_TRACK_SLAVE_DEFATTR_BUFSIZE       (4 * AO_TRACK_DEFATTR_BUFSIZE)
#define AO_TRACK_VIRTUAL_DEFATTR_BUFSIZE     (256 * AO_TRACK_DEFATTR_BUFSIZE)
#define AO_TRACK_DEFATTR_START_THRESHOLDMS   50
#define AO_TRACK_DEFATTR_RESUME_THRESHOLDMS  50
#define AO_TRACK_DUPLICATE_DELAY_THRESHOLDMS 70
#define AO_TRACK_DUPLICATE_BUF_RESERVEMS     60

#define AO_TRACK_PATH_NAME_MAXLEN 256
#define AO_TRACK_FILE_NAME_MAXLEN 256

#define AO_TRACK_NEED_ADD_MUTE_AIP_FRAMES_LIMITS 2
#define AO_TRACK_RAW_OUTPUT_ADD_MUTE_FRAMES      2

#define AO_TRACK_MAX_PCM_DATA_SIZE 0xffff
#define AO_TRACK_MAX_RAW_DATA_SIZE 0xffff

typedef enum {
    STREAM_CHANGE_NONE = (TYPE_MAX << 8) | TYPE_MAX,
    STREAM_CHANGE_PCM2PCM = (PCM << 8) | PCM,
    STREAM_CHANGE_PCM2LBR = (PCM << 8) | LBR,
    STREAM_CHANGE_PCM2HBR = (PCM << 8) | HBR,
    STREAM_CHANGE_LBR2PCM = (LBR << 8) | PCM,
    STREAM_CHANGE_LBR2LBR = (LBR << 8) | LBR,
    STREAM_CHANGE_LBR2HBR = (LBR << 8) | HBR,
    STREAM_CHANGE_HBR2PCM = (HBR << 8) | PCM,
    STREAM_CHANGE_HBR2LBR = (HBR << 8) | LBR,
    STREAM_CHANGE_HBR2HBR = (HBR << 8) | HBR,
} stream_mode_change;

#define STREAM_CHANGE_FROM(mode) ((mode & 0xff00) >> 8)
#define STREAM_CHANGE_TO(mode)   (mode & 0x00ff)

/* used to make track stream mode change check more simpler */
#define STREAM_CHANGE_FROM_PCM(mode) (STREAM_CHANGE_FROM(mode) == PCM)
#define STREAM_CHANGE_FROM_LBR(mode) (STREAM_CHANGE_FROM(mode) == LBR)
#define STREAM_CHANGE_FROM_HBR(mode) (STREAM_CHANGE_FROM(mode) == HBR)

#define STREAM_CHANGE_TO_PCM(mode) (STREAM_CHANGE_TO(mode) == PCM)
#define STREAM_CHANGE_TO_LBR(mode) (STREAM_CHANGE_TO(mode) == LBR)
#define STREAM_CHANGE_TO_HBR(mode) (STREAM_CHANGE_TO(mode) == HBR)

/* audio output attribute */
typedef struct {
    hi_u32 pcm_sample_rate;
    hi_u32 lbr_sample_rate;
    hi_u32 hbr_sample_rate;

    hi_u32 pcm_channels;
    hi_u32 lbr_channels;           /* 2 */
    hi_u32 hbr_channels;           /* 2(DDP) or 8 */
    hi_u32 org_multi_pcm_channels; /* 6 or 8 */

    hi_u32 pcm_bit_depth; /* 16 or 24 */
    hi_u32 lbr_bit_depth; /* 16 */
    hi_u32 hbr_bit_depth; /* 16(iec61937) or 24(blue-ray LPCM) */

    hi_u32 pcm_samples;
    hi_u32 pcm_bytes;
    hi_u32 lbr_bytes;
    hi_u32 hbr_bytes;

    hi_u32 lbr_format; /* DD/DTS */
    hi_u32 hbr_format; /* DDP/DTSHD/TRUEHD or 8ch-LPCM */

    hi_void *pcm_data_buf; /* pointer to the decoded PCM data */
    hi_void *lbr_data_buf; /* pointer to the LBR data */
    hi_void *hbr_data_buf; /* pointer to the HBR data */
} snd_track_stream_attr;

typedef struct {
    hi_u32 bit_depth;
    hi_u32 channels;
    hi_u32 org_channels;
    hi_u32 sample_rate;
    hi_u32 org_sample_rate;
    hi_u32 format;
    hi_u32 samples;
    hi_u32 size;
    hi_void *data;
} track_frame;

typedef struct {
    track_frame pcm;
    track_frame lbr;
    track_frame hbr;
} snd_track_frame;

typedef struct {
    stream_mode_change pcm_change;
    stream_mode_change spdif_change;
    stream_mode_change hdmi_change;
} stream_mode_change_attr;

typedef struct {
    osal_task *thread;
    hi_audio_buffer rbf_mmz;
    circ_buf cb;
    hi_u32 buf_wptr;
    hi_u32 buf_rptr;
} track_save_alsa_attr;

typedef struct {
    hi_ao_track_attr user_track_attr;
    hi_ao_abs_gain track_abs_gain;
    hi_ao_preci_gain user_prescale;
    hi_bool mute;
    hi_track_mode channel_mode;
    hi_ao_speed speed;
    hi_u32 add_mute_frame_num;
    hi_bool eos_flag;

    /* internal state */
    hi_u32 track_id;
    hi_handle h_track;
    hi_bool alsa_track;
    snd_track_status curn_status;
    snd_track_stream_attr stream_attr;
#ifdef HI_AUDIO_AI_SUPPORT
    hi_bool att_ai;
    hi_handle h_ai;
#endif
    hi_u32 pause_delay_ms;

#ifdef HI_PROC_SUPPORT
    /* save pcm */
    snd_debug_cmd_ctrl save_state;
    hi_void *file_handle;
    hi_void *tmp_buf;  /* used for copy_from_user when saving track pcm data */
#endif

    /* track send statistics */
    hi_u32 send_try_cnt;
    hi_u32 send_cnt;

    aoe_aip_id aip[TYPE_MAX];
    hi_audio_buffer aip_rbf_mmz[TYPE_MAX];
    hi_bool aip_rbf_ext_dma_mem[TYPE_MAX];
    hi_bool fifo_bypass;
    hi_bool aip_priority;
    hi_bool data_from_kernel;

#ifdef HI_PROC_SUPPORT
    track_save_alsa_attr save_alsa;
#endif
    hi_u64 user_delay_virt_addr;
    hi_u64 phy_delay_addr;
    struct osal_list_head node;
} snd_track_state;

snd_track_state *track_find_by_handle(snd_card_state *card, hi_handle h_track);
snd_card_state *track_card_get_card(hi_u32 id);
hi_s32 track_create(snd_card_state *card, hi_ao_track_attr *attr,
    hi_bool alsa_track, hi_u32 track_id);

hi_s32 track_destroy(snd_card_state *card, hi_u32 track_id);
hi_s32 track_start(snd_card_state *card, hi_u32 track_id);
hi_s32 track_stop(snd_card_state *card, hi_u32 track_id);
hi_s32 track_pause(snd_card_state *card, hi_u32 track_id);
hi_s32 track_flush(snd_card_state *card, hi_u32 track_id);
hi_s32 track_set_aip_fifo_bypass(snd_card_state *card, hi_u32 track_id, hi_bool enable);
hi_s32 track_check_attr(hi_ao_track_attr *attr);
hi_s32 track_set_attr(snd_card_state *card, hi_u32 track_id, hi_ao_track_attr *attr);
hi_s32 track_get_attr(snd_card_state *card, hi_u32 track_id, hi_ao_track_attr *attr);
hi_s32 track_send_data(snd_card_state *card, hi_u32 track_id, ao_frame *frame);
hi_s32 track_atomic_send_data(snd_card_state *card, hi_u32 track_id, ao_frame *frame);
hi_s32 track_mmap(snd_card_state *card, hi_u32 track_id, ao_track_mmap_param *param);
hi_s32 track_set_weight(snd_card_state *card, hi_u32 track_id, hi_ao_gain *track_gain);
hi_s32 track_get_weight(snd_card_state *card, hi_u32 track_id, hi_ao_gain *track_gain);
hi_s32 track_set_speed_adjust(snd_card_state *card, hi_u32 track_id, hi_ao_speed *speed);
hi_s32 track_get_delay_ms(snd_card_state *card, hi_u32 track_id, hi_u32 *delay_ms);
hi_u32 track_get_adac_delay_ms(snd_card_state *card, snd_track_state *track);
hi_u32 track_get_spdif_delay_ms(snd_card_state *card, snd_track_state *track);
hi_u32 track_get_hdmi_delay_ms(snd_card_state *card, snd_track_state *track);

hi_s32 track_is_buf_empty(snd_card_state *card, hi_u32 track_id, hi_bool *empty);
hi_s32 track_set_eos(snd_card_state *card, hi_u32 track_id, hi_bool eos);
hi_s32 track_get_def_attr(hi_ao_track_attr *def_attr);

#ifdef HI_PROC_SUPPORT
hi_s32 track_read_proc(hi_void *p, snd_card_state *card);
hi_s32 ao_write_proc_set_track(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
hi_s32 track_write_proc_save_data(snd_card_state *card, hi_u32 track_id, snd_debug_cmd_ctrl cmd);
#endif

hi_s32 track_detect_attr(snd_card_state *card, hi_u32 track_id,
    hi_ao_track_attr *track_attr, snd_track_attr_setting *attr_setting);
hi_u32 track_get_master_id(snd_card_state *card);

#ifdef HI_AUDIO_AI_SUPPORT
hi_s32 track_set_pcm_attr(snd_card_state *card, hi_u32 track_id, hi_handle h_ai);
hi_s32 track_attach_ai(snd_card_state *card, hi_u32 track_id, hi_handle h_ai);
hi_s32 track_detach_ai(snd_card_state *card, hi_u32 track_id);
#endif

hi_s32 track_get_setting(snd_card_state *card, hi_u32 track_id, snd_track_settings *snd_settings);
hi_s32 track_restore_setting(snd_card_state *card, hi_u32 track_id, snd_track_settings *snd_settings);
hi_s32 track_set_abs_gain(snd_card_state *card, hi_u32 track_id, hi_ao_abs_gain *track_abs_gain);
hi_s32 track_get_abs_gain(snd_card_state *card, hi_u32 track_id, hi_ao_abs_gain *track_abs_gain);
hi_s32 track_set_prescale(snd_card_state *card, hi_u32 track_id, hi_ao_preci_gain *preci_gain);
hi_s32 track_get_prescale(snd_card_state *card, hi_u32 track_id, hi_ao_preci_gain *preci_gain);
hi_s32 track_set_mute(snd_card_state *card, hi_u32 track_id, hi_bool mute);
hi_s32 track_get_mute(snd_card_state *card, hi_u32 track_id, hi_bool *mute);
hi_s32 track_set_all_mute(snd_card_state *card, hi_bool mute);
hi_s32 track_set_channel_mode(snd_card_state *card, hi_u32 track_id, hi_track_mode mode);
hi_s32 track_get_channel_mode(snd_card_state *card, hi_u32 track_id, hi_track_mode *mode);
hi_s32 track_set_fifo_latency(snd_card_state *card, hi_u32 track_id, hi_u32 latency_ms);
hi_s32 track_get_fifo_latency(snd_card_state *card, hi_u32 track_id, hi_u32 *latency_ms);
hi_s32 track_set_fade_attr(snd_card_state *card, hi_u32 track_id, ao_track_fade *track_fade);

hi_s32 track_set_priority(snd_card_state *card, hi_u32 track_id, hi_bool enable);
hi_s32 track_get_priority(snd_card_state *card, hi_u32 track_id, hi_bool *enable);

hi_s32 track_force_all_to_pcm(snd_card_state *card);
hi_s32 track_set_used_by_kernel(snd_card_state *card, hi_u32 track_id);
hi_s32 track_set_resume_threshold_ms(snd_card_state *card, hi_u32 track_id, hi_u32 threshold_ms);
hi_s32 track_get_resume_threshold_ms(snd_card_state *card, hi_u32 track_id, hi_u32 *threshold_ms);
hi_s32 track_get_info(snd_card_state *card, hi_u32 track_id, hi_u64 *virt_addr, hi_u64 *phys_addr);
hi_s32 track_set_info(snd_card_state *card, hi_u32 track_id, hi_u64 virt_addr);

hi_s32 track_set_all_prescale(snd_card_state *card, hi_ao_preci_gain *prescale);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_TRACK_H__ */

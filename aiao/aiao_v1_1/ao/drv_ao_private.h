/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao private header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_PRIVATE_H__
#define __DRV_AO_PRIVATE_H__

#include "hi_osal.h"

#include "hi_drv_dev.h"
#include "drv_adsp_ext.h"
#include "drv_ao_ext.h"

#include "drv_ao_ioctl.h"
#include "hal_aoe_common.h"

#include "audio_mem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AO_MAX_TOTAL_SND_NUM AO_SND_MAX

#define AO_SND_FILE_NOUSE_FLAG    0xFFFFFFFF
#define AO_RENDER_TRACK_NUM       6
#define AO_SOUND_PATH_NAME_MAXLEN 256
#define AO_SOUND_FILE_NAME_MAXLEN 256

#define SND_MAX_OPEN_NUM 32

#define AO_SND_MUTE_DISABLE_TIMEMS_RESUME 500

#define AO_STRING_SKIP_BLANK(str) \
    while ((str)[0] == ' ') {     \
        (str)++;                  \
    }

typedef enum {
    SND_DEBUG_CMD_CTRL_START = 0,
    SND_DEBUG_CMD_CTRL_STOP,
    SND_DEBUG_CMD_CTRL_MAX
} snd_debug_cmd_ctrl;

typedef enum {
    SND_DEBUG_CMD_PROC_SAVE_TRACK = 0,
    SND_DEBUG_CMD_PROC_SAVE_SOUND,
    SND_DEBUG_CMD_PROC_MAX
} snd_debug_cmd_proc;

enum {
    SND_DATA_TYPE_PCM = 0,
    SND_DATA_TYPE_LBR = 1,
    SND_DATA_TYPE_HBR = 2,
    SND_DATA_TYPE_MAX = 3,

    PCM = SND_DATA_TYPE_PCM,
    LBR = SND_DATA_TYPE_LBR,
    HBR = SND_DATA_TYPE_HBR,
    TYPE_MAX = SND_DATA_TYPE_MAX,
};

typedef enum {
    SND_TRACK_STATUS_STOP = 0,
    SND_TRACK_STATUS_START,
    SND_TRACK_STATUS_PAUSE,
    SND_TRACK_STATUS_MAX,
} snd_track_status;

typedef enum {
    SND_TRACK_ATTR_RETAIN = 0,
    SND_TRACK_ATTR_MODIFY,
    SND_TRACK_ATTR_MASTER2SLAVE,  /* RECREATE */
    SND_TRACK_ATTR_SLAVE2MASTER,  /* RECREATE */
    SND_TRACK_ATTR_MAX,
} snd_track_attr_setting;

typedef enum {
    SND_OP_STATUS_STOP = 0,
    SND_OP_STATUS_START,
    SND_OP_STATUS_CAST_MAX,
} snd_op_status;

/* engine资源node描述 */
typedef struct {
    struct osal_list_head node;

    aoe_engine_id id;
    hi_u32 type;

    /* engine aef */
    hi_bool avc_enable;
    hi_ao_avc_attr avc_attr;

    hi_bool geq_enable;
} engine;

/* engine资源head描述 */
typedef struct {
    struct osal_list_head list; /* list of registered engine */
} snd_engine;

/* track资源head描述 */
typedef struct {
    struct osal_list_head list; /* list of registered track */

    hi_u32 snd_track_init_flag;
    hi_bool all_track_mute;  /* exclude alsa track */
    hi_u32  ext_delay_ms;
    hi_ao_preci_gain all_track_preci_gain;
} snd_track;

/* op资源head描述 */
typedef struct {
    struct osal_list_head list;  /* list of registered out port */

    hi_bool adac_enable;
    hi_bool hdmi_debug;
} snd_op;

/* ar资源head描述 */
typedef struct {
    hi_bool con_output_enable;
    hi_bool output_atmos_enable;
    ao_render_attr render_attr;
} snd_ar;

/* cast资源head描述 */
typedef struct {
    struct osal_list_head list; /* list of registered cast */
    hi_u32  snd_cast_init_flag;
} snd_cast;

typedef struct {
    /* save pcm */
    snd_debug_cmd_ctrl save_state;
    hi_void *file_handle;
} snd_proc_debug;

/* track aoe engine state */
typedef struct {
    struct osal_list_head node;

    aoe_engine_id engine;
    hi_u32 engine_type;
} snd_engine_state;

typedef struct {
    osal_task *thread;
    snd_debug_cmd_ctrl save_state;
    hi_void *file;  /* file handle used to create cast */
    hi_handle h_cast;
    hi_bool aef_bypass;

    hi_void *write_file; /* file handle used to save cast file */
} snd_proc_cast_attr;

typedef struct {
    ao_snd_id sound;
    hi_ao_attr user_open_param;
    hi_sample_rate user_sample_rate;
    hi_ao_ouput_mode user_hdmi_mode;
    hi_ao_ouput_mode user_spdif_mode;

#ifdef HI_SND_AVC_SUPPORT
    hi_bool avc_enable;
    hi_ao_avc_attr avc_attr;
#endif

#ifdef HI_SND_GEQ_SUPPORT
    hi_bool geq_enable;
    hi_ao_geq_attr geq_attr;
#endif

#ifdef HI_SND_ARC_SUPPORT
    hi_bool user_arc_enable;
    hi_ao_ouput_mode user_arc_mode;
    hi_ao_arc_audio_cap user_arc_cap;
#endif

    hi_u32 hdmi_passthrough;  /* 0(no hdmi),  1(pcm), 2(lbr), 3(hbr/7.1 lpcm) */
    hi_u32 spdif_passthrough; /* 0(no spdif),  1(pcm), 2(lbr) */
    hi_u32 arc_passthrough;   /* 0(no hdmi),  1(pcm), 2(lbr), 3(hbr/7.1 lpcm) */
    hi_u32 hdmi_data_format;
    hi_u32 spdif_data_format;

    hi_ao_port hdmi_port; /* hdmi port id */

    hi_bool lbr_device;
    hi_bool hbr_device;

    hi_u32 snd_track_init_flag;
    struct osal_list_head op;
    struct osal_list_head track;
    hi_bool all_track_mute;  /* exclude alsa track */
    struct osal_list_head engine;

    hi_bool hdmi_debug;
    hi_bool adac_enable;
    hi_u32  snd_cast_init_flag;
    struct osal_list_head cast;
    hi_u32 ext_delay_ms;

#ifdef HI_PROC_SUPPORT
    /* save pcm */
    snd_debug_cmd_ctrl save_state;
    snd_proc_cast_attr snd_proc_cast;
#endif

    hi_bool all_cast_mute;
    hi_bool cast_simulate_op;
    hi_ao_port cast_simulate_port;

    hi_bool dma_mode;
    hi_bool other_track_mute;

#if defined(HI_SND_AR_SUPPORT)
    hi_bool con_output_enable;
    hi_bool output_atmos_enable;
    ao_render_attr render_attr;
#endif

    hi_ao_preci_gain all_track_preci_gain;
} snd_card_state;

typedef struct {
    hi_ao_attr ao_attr;

    /* 全局属性 */
    hi_bool dma_mode;

    /* 声卡中的资源 */
    snd_engine engine;
    snd_track  track;
    snd_op     op;
    snd_cast   cast;
    snd_ar     *ar;

#ifdef HI_PROC_SUPPORT
    /* save pcm */
    snd_debug_cmd_ctrl save_state;
    hi_void *file_handle;
#endif
} hi_snd_card;

typedef struct {
    hi_ao_attr ao_attr;

    /* 全局属性 */
    hi_bool dma_mode;

    /* 声卡中的资源 */
    struct osal_list_head engine; /* list of registered engine */

    struct osal_list_head track;  /* list of registered track */
    hi_u32  snd_track_init_flag;
    hi_bool all_track_mute;  /* exclude alsa track */
    hi_u32  ext_delay_ms;
    hi_ao_preci_gain all_track_preci_gain;

    struct osal_list_head op;  /* list of registered out port */
    hi_bool adac_enable;
    hi_bool hdmi_debug;

    struct osal_list_head cast; /* list of registered cast */
    hi_u32  snd_cast_init_flag;

    snd_ar     *ar;

#ifdef HI_PROC_SUPPORT
    /* save pcm */
    snd_debug_cmd_ctrl save_state;
    hi_void *file_handle;
#endif
} hi_snd_card2;

typedef struct {
    hi_char *cmd;
    hi_s32 (*func)(ao_snd_id sound, snd_card_state *card, hi_char *pc_buf);
} snd_write_proc_fun;

typedef struct {
    ao_snd_id sound;
    hi_ao_track_attr track_attr;
    hi_bool alsa_track;
    hi_ao_abs_gain track_abs_gain;
    hi_ao_preci_gain prescale;
    hi_bool mute;
    hi_bool priority;
    hi_bool bypass;  /* AIP fifo bypass */
    hi_track_mode channel_mode;
    snd_track_status curn_status;
    hi_ao_speed speed;

#ifdef HI_AUDIO_AI_SUPPORT
    hi_bool att_ai;
    hi_handle h_ai;
#endif

    hi_bool data_from_kernel;
} snd_track_settings;

typedef struct {
    hi_ao_abs_gain cast_abs_gain;
    hi_bool mute;
    ao_snd_id sound;
    hi_bool user_enable_setting;
    hi_u64 user_virt_addr;
    hi_ao_cast_attr cast_attr;
} snd_cast_settings;

typedef struct {
    hi_ao_gain user_gain;
    hi_ao_preci_gain user_preci_gain;
    hi_s32 user_balance;

    hi_track_mode user_track_mode;
    hi_u32 user_mute;
    hi_bool bypass;
    hi_bool latency_ms;  /* aop buffer threshold */
    hi_u32 delay_ms;
    snd_op_status curn_status;

#ifdef HI_SND_DRC_SUPPORT
    hi_bool drc_enable;
    hi_ao_drc_attr drc_attr;
#endif
} snd_outport_attr;

typedef struct {
    hi_ao_attr user_open_param;
    hi_ao_ouput_mode user_hdmi_mode;
    hi_ao_ouput_mode user_spdif_mode;
    snd_outport_attr port_attr[HI_AO_OUTPUT_PORT_MAX];
    hi_bool all_track_mute;
    hi_bool con_output_enable;
    hi_bool output_atmos_enable;
    hi_bool adac_enable;
    hi_bool user_set_hdmi_mute;

#ifdef HI_SND_ARC_SUPPORT
    hi_bool user_arc_enable;
    hi_ao_ouput_mode user_arc_mode;
    hi_ao_arc_audio_cap user_arc_cap;
#endif

    hi_bool all_cast_mute;

#ifdef HI_SND_AR_SUPPORT
    ao_render_attr render_attr;
#endif

    hi_ao_preci_gain all_track_preci_gain;
} snd_card_settings;

#ifdef HI_PROC_SUPPORT
/* echo proc save sound pcm */
typedef struct {
    ao_snd_id sound;
    hi_bool aef_bypass;
    hi_void *file_handle;
    hi_void *devfile_handle;
} snd_pcm_save_attr;
#endif

/* cast entity */
typedef struct {
    hi_audio_buffer rbf_mmz; /* mmz dont release at suspend */
    snd_cast_settings suspend_attr;
    hi_void *file;    /* file handle */
    osal_atomic atm_use_cnt; /* cast use count, only support single user */
} ao_cast_entity;

/* track entity */
typedef struct {
    snd_track_settings suspend_attr;
    hi_void *file;    /* file handle */
    osal_atomic atm_use_cnt; /* track use count, only support single user */
} ao_track_entity;

/* snd DMA entity */
typedef struct {
    hi_void *file;    /* file handle */
    osal_atomic atm_use_cnt; /* DMA use count, only support single user */
} ao_snd_dma_entity;

/* snd entity */
typedef struct {
    snd_card_state *card;                       /* snd structure pointer */

#if defined(HI_SND_AR_SUPPORT)
    hi_audio_buffer isb_mmz[AO_RENDER_TRACK_NUM]; /* mmz do not release at suspend */
#endif
    snd_card_settings suspend_attr;
    hi_void *file[SND_MAX_OPEN_NUM]; /* file handle */
    osal_atomic atm_use_total_cnt;          /* snd use count, support multi user */
} ao_snd_entity;

/* ao global management */
typedef struct {
#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
    ao_cast_entity cast_entity[AO_MAX_CAST_NUM];
#endif
    hi_u32 track_num; /* record AO track num */
    hi_bool low_latency_created;
    ao_track_entity track_entity[AO_MAX_TOTAL_TRACK_NUM]; /* track parameter */

    ao_snd_entity snd_entity[AO_MAX_TOTAL_SND_NUM];     /* snd parameter */

    ao_snd_dma_entity snd_dma_entity[AO_MAX_TOTAL_SND_NUM];

    osal_atomic atm_open_cnt; /* open times */
    hi_bool ready;         /* init flag */
} ao_mgmt;

typedef struct {
    osal_atomic atm_user_open_cnt[AO_MAX_TOTAL_SND_NUM]; /* user snd open times */
    hi_s32 file_id[AO_MAX_TOTAL_SND_NUM];
} drv_ao_state;

hi_s32 ao_drv_init(hi_void);
hi_void ao_drv_exit(hi_void);
hi_s32 ao_drv_open(hi_void *private_data);
hi_s32 ao_drv_release(hi_void *private_data);

ao_mgmt *ao_get_mgmt(hi_void);
snd_card_state *snd_card_get_card(ao_snd_id sound);
ao_snd_id snd_card_get_snd(snd_card_state *card);
hi_void ao_lock(hi_void);
hi_void ao_unlock(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


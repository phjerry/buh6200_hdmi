/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ai driver private head file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AI_PRIVATE_H__
#define __DRV_AI_PRIVATE_H__

#include "circ_buf.h"
#include "hal_aiao_common.h"
#include "drv_ai_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AI_NAME "HI_AI"

#define AI_LATENCYMS_PERFRAME_DF (20)
#define AI_SAMPLE_PERFRAME_DF    (48000 / 1000 * AI_LATENCYMS_PERFRAME_DF)
#define AI_BUFF_FRAME_NUM_DF     (16)

enum {
    AI_I2S0_MSK = 0,
    AI_I2S1_MSK,
    AI_ADAC_MSK,
    AI_HDMI_MSK,
};

#define AI_OPEN_CNT_MAX 2

#ifdef HI_PROC_SUPPORT
#define AI_PATH_NAME_MAXLEN 256
#define AI_FILE_NAME_MAXLEN 256

typedef enum {
    AI_CMD_CTRL_STOP = 0,
    AI_CMD_CTRL_START,
    AI_CMD_CTRL_MAX
} ai_cmd_ctrl;

typedef enum {
    AI_CMD_PROC_SAVE_AI = 0,
    AI_CMD_PROC_NR_SWITCH,
    AI_CMD_PROC_SET_DELAY,
    AI_CMD_PROC_SET_NRLEVEL,
    AI_CMD_PROC_MAX
} ai_cmd_proc;
#endif

typedef struct {
    hi_u64 buf_phy_addr;
    hi_u64 buf_vir_addr;
    hi_u32 buf_size;
    hi_u32 period_byte_size;
    hi_u32 periods;
} ai_alsa_buf_attr;

typedef struct {
    ai_alsa_buf_attr buf;  /* for alsa mmap dma buffer */
    aiao_isr_func *isr_func;
    hi_void *substream;  /* for alsa ISR func params */
} ai_alsa_param;

typedef enum {
    AI_CHN_STATUS_STOP = 0,
    AI_CHN_STATUS_START,
    AI_CHN_STATUS_MAX,
} ai_chn_status;

typedef struct {
    hi_bool used;
    hi_audio_buffer ai_proc_mmz;
    ai_proc_info *proc_info;
} ai_proc_attr;

typedef struct {
    hi_handle h_ai;
    hi_ai_attr snd_port_attr;
    hi_u32 channels;
    hi_u32 bit_per_sample;
    hi_ai_port ai_port;
    aiao_port_id port;
    ai_chn_status curn_status;

    hi_audio_buffer rbf_mmz;    /* port mmz buf */
    hi_audio_buffer frame_mmz;  /* ai mmz buf */
    ai_buf_attr ai_buf;       /* the same as ai_rbf_mmz in physics */
    hi_void *file;

    hi_bool attach;
    hi_handle h_track;
    hi_bool alsa;
    hi_void *alsa_para;
    hi_ai_delay delay_comps;

    ai_proc_attr proc_attr;

#ifdef HI_PROC_SUPPORT
    /* save pcm */
    ai_cmd_ctrl save_state;
    hi_u32 save_cnt;
    hi_void *file_handle;
    hi_void *save_buf;
    hi_u32 save_read_pos;
    hi_bool save_thread_run;
    osal_task *save_thread;
#endif
} ai_chn_state;

typedef struct {
    hi_ai_port ai_port_id;
    hi_audio_buffer rbf_mmz;
} ai_resource;

/* AI GLOABL RESOURCE */
typedef struct {
    hi_u32 ai_use_flag; /* resource usage such as  (1 << I2S | 1  << HDMI RX | 1 <<  ...) */
    ai_chn_state *ap_ai_state[AI_MAX_TOTAL_NUM];
    ai_export_func ext_func; /* AI provide extenal functions */
    osal_atomic atm_open_cnt; /* open times */
} ai_global_param;

hi_s32 ai_drv_open(hi_void *file);
hi_s32 ai_drv_ioctl(hi_u32 cmd, hi_void *arg, hi_void *file);
hi_s32 ai_drv_release(hi_void *file);
hi_s32 ai_drv_init(hi_void);
hi_void ai_drv_exit(hi_void);

hi_s32 ai_drv_suspend(hi_void *private_data);
hi_s32 ai_drv_resume(hi_void *private_data);
hi_s32 ai_get_port_buf(hi_handle h_ai, aiao_rbuf_attr *aiao_buf);
hi_s32 ai_get_port_attr(hi_handle h_ai, aiao_port_attr *port_attr);

hi_s32 ai_get_enable(hi_handle h_ai, hi_bool *enable);
hi_s32 ai_set_enable(hi_handle h_ai, hi_bool enable, hi_bool track_resume);
hi_s32 ai_get_delay_ms(hi_handle h_ai, hi_u32 *delay_ms);
hi_bool ai_check_is_hdmi_port(hi_handle h_ai);

hi_s32 ai_attach_track(hi_handle h_ai, hi_handle h_track);
hi_s32 ai_detach_track(hi_handle h_ai, hi_handle h_track);

#ifdef HI_ALSA_AI_SUPPORT
hi_s32 ai_get_isr_func(aiao_isr_func **func);
hi_s32 ai_get_aiao_port_id(hi_handle h_ai, aiao_port_id *port);
#endif

hi_void ai_osal_init(hi_void);
hi_void ai_osal_deint(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

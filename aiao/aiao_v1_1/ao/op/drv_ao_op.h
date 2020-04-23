/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao port function
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_OP_H__
#define __DRV_AO_OP_H__

#include "hi_osal.h"

#include "hi_drv_ao.h"
#include "hal_aoe.h"
#include "hal_cast.h"
#include "hal_aiao.h"
#include "drv_ao_private.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AO_SNDOP_PERIODBUFSIZE 4096
#define AO_SNDOP_LATENCY_THDMS 64

#define AO_SNDOP_GLOBAL_MUTE_BIT 1
#define AO_SNDOP_LOCAL_MUTE_BIT  0

#define AO_SNDOP_MAX_AOP_NUM 2  /* max aop number per op */

#define AO_SNDOP_MIN_AOP_PERIOD_SIZE 1024

typedef struct {
    hi_u32 bit_per_sample;
    hi_u32 channels;
    hi_u32 sample_rate;
    hi_u32 data_format;

    hi_u32 data_type;

    hi_u32 latency_thd_ms;

    union {
        hi_ao_dac_attr dac_attr;
        hi_ao_i2s_attr i2s_attr;
        hi_ao_spdif_attr spdif_attr;
        hi_ao_hdmi_attr hdmi_attr;
        hi_ao_arc_attr arc_attr;
    } un_attr;
} snd_op_attr;

typedef enum {
    SND_AOP_TYPE_I2S = 0, /* hbr or 2.0 pcm or 7.1 lpcm */
    SND_AOP_TYPE_SPDIF,   /* lbr or hbr(ddp) */
    SND_AOP_TYPE_CAST,    /* 2.0 16bit pcm only */

    SND_AOP_TYPE_MAX
} snd_aop_type;

typedef enum {
    SND_OUTPUT_TYPE_DAC,

    SND_OUTPUT_TYPE_I2S,

    SND_OUTPUT_TYPE_SPDIF,

    SND_OUTPUT_TYPE_HDMI,

    SND_OUTPUT_TYPE_CAST,

    SND_OUTPUT_TYPE_ARC,

    SND_OUTPUT_TYPE_MAX,
} snd_output_type;

struct snd_op_driver;

typedef struct {
    struct osal_list_head node;
    snd_op_attr snd_port_attr;

    hi_ao_gain user_gain;

    hi_ao_preci_gain user_preci_gain;
    hi_s32 user_balance;

    hi_track_mode user_track_mode;
    hi_u32 user_mute;   /* bit[1]:global mute; bit[0]:local mute. if user_mute=0, real mute;else real unmute */
    hi_u32 latency_ms;  /* AOP buffer threshold setting */
    hi_bool bypass;
    hi_sample_rate sample_rate;

    hi_bool drc_enable;
    hi_ao_drc_attr drc_attr;

    hi_bool peq_enable;
    hi_ao_peq_attr peq_attr;

    hi_bool ad_output_enable;

    /* internal state */
    snd_op_status status;
    snd_output_type out_type;
    hi_ao_port out_port;
    hi_u32 active_id; /* 0 or 1 */
    hi_u32 op_mask;   /* bit0(0/1) and bit1(0/1) */
    aiao_port_id port_id[AO_SNDOP_MAX_AOP_NUM];
    aoe_aop_id aop[AO_SNDOP_MAX_AOP_NUM];
    hi_u32 engine_type[AO_SNDOP_MAX_AOP_NUM];
    hi_audio_buffer rbf_mmz[AO_SNDOP_MAX_AOP_NUM];
    aiao_cast_id cast_id;
    hi_u32 delay_ms;
} snd_op_state;

typedef struct {
    hi_ao_port   ao_port;
    snd_aop_type aop_type;
    hi_u32       i2s_id;
    hi_char      *buf_name;
    hi_u32       buf_size;

    aiao_port_id aiao_port;
    aiao_port_user_cfg *port_cfg;
} snd_op_create_param;

typedef struct snd_op_driver {
    struct osal_list_head node;

    hi_bool (*match)(hi_ao_port out_port);
    hi_s32  (*create)(snd_card_state *card, hi_ao_port_attr *attr, hi_bool resume);
    hi_void (*destroy)(snd_op_state *snd_op, hi_bool suspend);
    hi_s32 (*get_hw_param)(hi_ao_port_attr *attr, snd_aop_type aop_type, snd_op_create_param *hw_param);

    hi_s32 (*set_output_mode)(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode mode);
    hi_s32 (*get_output_mode)(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode *mode);
} snd_output_port_driver;

typedef struct {
    hi_char *cmd;
    hi_s32 (*func)(snd_card_state *card, hi_ao_port out_port, hi_char *buf);
} snd_op_write_proc;

hi_void snd_op_register_driver(hi_void);
hi_void snd_op_init(snd_op_state *snd_op);

hi_void snd_op_add_mute(snd_op_state *snd_op, hi_u32 mute_ms);
hi_void snd_op_get_delay_by_output_mode(snd_card_state *card, hi_u32 output_mode, hi_u32 *delay);
hi_void snd_get_delay_ms(snd_card_state *card, hi_u32 *delay);

hi_void ao_snd_destroy_op(snd_card_state *card, hi_bool suspend);
hi_s32  ao_snd_create_op(snd_card_state *card, hi_ao_attr *attr, hi_bool resume);
hi_bool ao_snd_check_output(snd_card_state *card, hi_u32 output_mode);

hi_s32 snd_op_create_aop(snd_op_state *state, hi_ao_port_attr *attr,
                         snd_aop_type type, hi_sample_rate sample_rate);
hi_s32 snd_set_op_mute(snd_card_state *card, hi_ao_port out_port, hi_bool mute);
hi_s32 snd_get_op_mute(snd_card_state *card, hi_ao_port out_port, hi_bool *mute);
hi_s32 snd_op_set_mute(snd_op_state *snd_op, hi_u32 mute);

hi_s32 snd_set_op_volume(snd_card_state *card, hi_ao_port out_port, hi_ao_gain *gain);
hi_s32 snd_get_op_volume(snd_card_state *card, hi_ao_port out_port, hi_ao_gain *gain);
hi_s32 snd_op_set_volume(snd_op_state *state, hi_ao_gain *gain);

hi_s32 snd_set_op_track_mode(snd_card_state *card, hi_ao_port out_port, hi_track_mode mode);
hi_s32 snd_get_op_track_mode(snd_card_state *card, hi_ao_port out_port, hi_track_mode *mode);
hi_s32 snd_op_set_track_mode(snd_op_state *state, hi_track_mode mode);

hi_s32 snd_set_op_preci_vol(snd_card_state *card, hi_ao_port out_port, hi_ao_preci_gain *preci_gain);
hi_s32 snd_get_op_preci_vol(snd_card_state *card, hi_ao_port out_port, hi_ao_preci_gain *preci_gain);
hi_s32 snd_op_set_preci_vol(snd_op_state *state, hi_ao_preci_gain *preci_gain);

hi_s32 snd_set_op_balance(snd_card_state *card, hi_ao_port out_port, hi_s32 balance);
hi_s32 snd_get_op_balance(snd_card_state *card, hi_ao_port out_port, hi_s32 *balance);

hi_s32 snd_op_set_attr(snd_op_state *state, snd_op_attr *snd_port_attr);
hi_s32 snd_op_get_attr(snd_op_state *state, snd_op_attr *snd_port_attr);

hi_s32 snd_op_get_output_mode(snd_card_state *card, hi_ao_port out_port, hi_ao_ouput_mode *mode);
hi_s32 snd_op_set_output_mode(snd_card_state *card, hi_ao_port out_port, hi_ao_ouput_mode mode);

hi_s32 snd_set_low_latency(snd_card_state *card, hi_ao_port out_port, hi_u32 latency_ms);
hi_u32 snd_get_low_latency(snd_card_state *card, hi_ao_port out_port, hi_u32 *latency_ms);

hi_s32 snd_set_port_sample_rate(snd_card_state *card, hi_ao_port out_port, hi_u32 sample_rate);
hi_s32 snd_get_port_info(snd_card_state *card, hi_ao_port out_port, snd_port_kernel_attr *port_k_attr);

hi_s32 snd_set_op_ad_output_enable(snd_card_state *card, hi_ao_port out_port, hi_bool enable);
hi_s32 snd_get_op_ad_output_enable(snd_card_state *card, hi_ao_port out_port, hi_bool *enable);

snd_op_state *snd_get_op_handle_by_out_port(snd_card_state *card, hi_ao_port out_port);
aoe_aop_id snd_get_op_aop_id(snd_op_state *state);

hi_s32 snd_get_op_setting(snd_card_state *card, snd_card_settings *snd_settings);
hi_s32 snd_restore_op_setting(snd_card_state *card, snd_card_settings *snd_settings);
hi_s32 snd_op_restore_setting(snd_card_state *card, hi_ao_port port,
                              snd_outport_attr *port_attr);

hi_s32 snd_op_stop(snd_op_state *op);
hi_s32 snd_op_start(snd_op_state *op);
hi_void snd_op_destroy(snd_op_state *op, hi_bool suspend);

#ifdef HI_PROC_SUPPORT
hi_s32 snd_read_op_proc(hi_void *p, snd_card_state *card, hi_ao_port port);
hi_s32 snd_write_op_proc(snd_card_state *card, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX]);
#endif

hi_void snd_get_x_run_count(snd_card_state *card, hi_u32 *count);
hi_s32 snd_set_delay_compensation(snd_card_state *card, hi_ao_port out_port, hi_u32 delay_ms);
hi_s32 snd_get_delay_compensation(snd_card_state *card, hi_ao_port out_port, hi_u32 *delay_ms);
hi_s32 snd_op_set_low_latency(snd_op_state *state, hi_u32 latency_ms);
hi_s32 snd_op_set_balance(snd_op_state *state, hi_s32 balance);

hi_s32 cast_op_create(snd_card_state *card, hi_ao_cast_attr *ao_cast_attr,
    hi_audio_buffer *mmz, snd_op_state *state);
hi_void cast_op_destroy(snd_op_state *state, hi_bool suspend);
hi_s32 cast_op_start(snd_op_state *state);
hi_s32 cast_op_stop(snd_op_state *state);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_OP_H__ */


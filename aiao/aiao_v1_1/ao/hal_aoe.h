/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aoe hal
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HI_HAL_AOE_H__
#define __HI_HAL_AOE_H__

#include "hi_audsp_common.h"
#include "hal_aoe_common.h"
#include "hi_ao_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AOE_AIP_BUFF_LATENCYMS_MIN 10
#define AOE_AIP_FIFO_LATENCYMS_MIN 6
#define AOE_AOP_BUFF_LATENCYMS_MIN 10

#define AOE_AOP_BUFF_LATENCYMS_DF   (AOE_AOP_BUFF_LATENCYMS_MIN * 4)
#define AOE_AOP_BUFF_LATENCYMS_MAX  (AOE_AOP_BUFF_LATENCYMS_MIN * 10)
#define AOE_CAST_BUFF_LATENCYMS_MAX 512

#ifdef HI_SOUND_PORT_DELAY_SUPPORT
#define AOE_AOP_BUFF_LATENCYMS_WITH_DELAY 330
#define PERIOND_NUM_WITH_DELAY            30
#else
#define AOE_AOP_BUFF_LATENCYMS_WITH_DELAY (AOE_AOP_BUFF_LATENCYMS_MAX)
#define PERIOND_NUM_WITH_DELAY            (PERIOND_NUM)
#endif

#if defined(CHIP_TYPE_HI3716MV430)
#define AO_DAC_MMZSIZE_MAX   ((48000 * 2 * sizeof(hi_u16) / 1000) * AOE_AOP_BUFF_LATENCYMS_MAX)
#define AO_I2S_MMZSIZE_MAX   ((48000 * 2 * sizeof(hi_u16) / 1000) * AOE_AOP_BUFF_LATENCYMS_MAX)
#define AO_SPDIF_MMZSIZE_MAX ((48000 * 2 * sizeof(hi_u16) / 1000) * AOE_AOP_BUFF_LATENCYMS_WITH_DELAY)
#else
#define AO_DAC_MMZSIZE_MAX   ((48000 * 2 * sizeof(hi_u32) / 1000) * AOE_AOP_BUFF_LATENCYMS_MAX)
#define AO_I2S_MMZSIZE_MAX   ((48000 * 2 * sizeof(hi_u32) / 1000) * AOE_AOP_BUFF_LATENCYMS_MAX)
#define AO_SPDIF_MMZSIZE_MAX ((192000 * 2 * sizeof(hi_u16) / 1000) * AOE_AOP_BUFF_LATENCYMS_WITH_DELAY)
#endif

#ifdef HI_SND_HBRA_PASSTHROUGH_SUPPORT
#define AO_HDMI_MMZSIZE_MAX ((192000 * 8 * sizeof(hi_u32) / 1000) * AOE_AOP_BUFF_LATENCYMS_WITH_DELAY)
#else
#ifdef HI_SOUND_PORT_DELAY_SUPPORT
#define AO_HDMI_MMZSIZE_MAX ((192000 * 2 * sizeof(hi_u32) / 1000) * AOE_AOP_BUFF_LATENCYMS_WITH_DELAY)
#else
#define AO_HDMI_MMZSIZE_MAX ((48000 * 2 * sizeof(hi_u32) / 1000) * AOE_AOP_BUFF_LATENCYMS_WITH_DELAY)
#endif
#endif

#define AO_CAST_MMZSIZE_MAX ((48000 * 2 * sizeof(hi_u16) / 1000) * AOE_CAST_BUFF_LATENCYMS_MAX)

static inline hi_u32 calc_latency_ms(hi_u32 rate, hi_u32 frame_size, hi_u32 byte)
{
    if (rate && frame_size) {
        return (1000 * byte) / (rate * frame_size);
    } else {
        return 0;
    }
}

/* global function */
hi_s32 hal_aoe_init(hi_void);
hi_void hal_aoe_deinit(hi_void);

/* AIP function */
hi_s32 hal_aoe_aip_create(aoe_aip_id *aip, aoe_aip_chn_attr *attr);
hi_void hal_aoe_aip_destroy(aoe_aip_id aip);
hi_s32 hal_aoe_aip_set_attr(aoe_aip_id aip, aoe_aip_chn_attr *attr);
hi_s32 hal_aoe_aip_get_attr(aoe_aip_id aip, aoe_aip_chn_attr *attr);
hi_u64 hal_aoe_aip_get_reg_addr(aoe_aip_id aip);
hi_void hal_aoe_aip_get_status_phy(hi_u32 aip, hi_u64 *phy_delay_addr);
hi_s32 hal_aoe_aip_start(aoe_aip_id aip);
hi_s32 hal_aoe_aip_group_start(hi_u32 start_mask);
hi_s32 hal_aoe_aip_stop(aoe_aip_id aip);
hi_s32 hal_aoe_aip_group_stop(hi_u32 stop_mask);
hi_s32 hal_aoe_aip_pause(aoe_aip_id aip);
hi_s32 hal_aoe_aip_group_pause(hi_u32 pause_mask);
hi_s32 hal_aoe_aip_flush(aoe_aip_id aip);
hi_s32 hal_aoe_aip_set_adjust_speed(aoe_aip_id aip, hi_s32 adj_speed);
hi_s32 hal_aoe_aip_set_speed(aoe_aip_id aip, hi_ao_speed *speed);
hi_u32 hal_aoe_aip_write_buf_data(aoe_aip_id aip, hi_u8 *src, hi_u32 size);
hi_u32 hal_aoe_aip_query_buf_data(aoe_aip_id aip);
hi_u32 hal_aoe_aip_query_buf_free(aoe_aip_id aip);
hi_void hal_aoe_aip_get_buf_delay_ms(aoe_aip_id aip, hi_u32 *delay_ms);
hi_void hal_aoe_aip_get_fifo_delay_ms(aoe_aip_id aip, hi_u32 *delay_ms);
hi_void hal_aoe_aip_get_status(aoe_aip_id aip, aoe_aip_status *status);
hi_s32 hal_aoe_aip_set_volume(aoe_aip_id aip, hi_u32 l_volume_db, hi_u32 r_volume_db);
hi_s32 hal_aoe_aip_set_prescale(aoe_aip_id aip, hi_u32 int_db, hi_s32 dec_db);
hi_s32 hal_aoe_aip_set_mute(aoe_aip_id aip, hi_bool mute);
hi_s32 hal_aoe_aip_set_channel_mode(aoe_aip_id aip, hi_u32 channel_mode);
hi_s32 hal_aoe_aip_set_fifo_bypass(aoe_aip_id aip, hi_bool enable);
hi_s32 hal_aoe_aip_get_fifo_bypass(aoe_aip_id aip, hi_bool *enable);
hi_s32 hal_aoe_aip_get_mix_priority(aoe_aip_id aip, hi_bool *priority);
hi_s32 hal_aoe_aip_set_mix_priority(aoe_aip_id aip, hi_bool priority);
hi_s32 hal_aoe_aip_attach_aop(aoe_aip_id aip, aoe_aop_id aop, hi_u32 skip_ms);
hi_s32 hal_aoe_aip_detach_aop(aoe_aip_id aip, aoe_aop_id aop);
hi_void hal_aoe_aip_set_data_source(aoe_aip_id aip, hi_bool data_from_kernel);
hi_s32 hal_aoe_aip_set_fifo_latency(aoe_aip_id aip, hi_u32 latency_ms);
hi_s32 hal_aoe_aip_get_fifo_latency(aoe_aip_id aip, hi_u32 *latency_ms);
hi_void hal_aoe_aip_set_eos(aoe_aip_id aip, hi_bool eos);
hi_void hal_aoe_aip_add_mute_at_read(aoe_aip_id aip, hi_u32 mute_ms);
hi_s32 hal_aoe_aip_set_resume_threshold_ms(aoe_aip_id aip, hi_u32 threshold_ms);
hi_s32 hal_aoe_aip_get_resume_threshold_ms(aoe_aip_id aip, hi_u32 *threshold_ms);
hi_s32 hal_aoe_aip_set_fade_attr(aoe_aip_id aip, hi_bool enable, hi_u32 fade_in_ms, hi_u32 fade_out_ms);
hi_s32 hal_aoe_aip_set_add_mute_ms(aoe_aip_id aip, hi_u32 mute_ms);

/* AOP function */
hi_s32 hal_aoe_aop_set_mute(aoe_aop_id aop, hi_bool mute);
hi_s32 hal_aoe_aop_set_volume(aoe_aop_id aop, hi_u32 l_volume_db, hi_u32 r_volume_db);
hi_s32 hal_aoe_aop_set_preci_vol(aoe_aop_id aop, hi_u32 int_db, hi_s32 dec_db);
hi_s32 hal_aoe_aop_set_balance(aoe_aop_id aop, hi_s32 balance);
hi_s32 hal_aoe_aop_get_rptr_and_wptr_reg_addr(aoe_aop_id aop, hi_void **wptr_reg, hi_void **rptr_reg);
hi_s32 hal_aoe_aop_create(aoe_aop_id *aop, aoe_aop_chn_attr *attr);
hi_void hal_aoe_aop_destroy(aoe_aop_id aop);
hi_s32 hal_aoe_aop_set_attr(aoe_aop_id aop, aoe_aop_chn_attr *attr);
hi_s32 hal_aoe_aop_get_attr(aoe_aop_id aop, aoe_aop_chn_attr *attr);
hi_s32 hal_aoe_aop_start(aoe_aop_id aop);
hi_s32 hal_aoe_aop_stop(aoe_aop_id aop);

hi_s32 hal_aoe_aop_set_drc_enable(aoe_aop_id aop, hi_bool enable);
hi_s32 hal_aoe_aop_set_drc_attr(aoe_aop_id aop, hi_ao_drc_attr *drc_attr);
hi_s32 hal_aoe_aop_set_peq_attr(aoe_aop_id aop, hi_ao_peq_attr *peq_attr);
hi_s32 hal_aoe_aop_set_peq_enable(aoe_aop_id aop, hi_bool enable);
hi_s32 hal_aoe_aop_set_ad_output_enable(aoe_aop_id aop, hi_bool enable);

hi_s32 hal_aoe_aop_set_aef_bypass(aoe_aop_id aop, hi_bool bypass);
hi_s32 hal_aoe_aop_set_low_latency(aoe_aop_id aop, hi_u32 latency);
hi_s32 hal_aoe_aop_get_low_latency(aoe_aop_id aop, hi_u32 *latency);

hi_s32 hal_aoe_aop_get_delay(aoe_aop_id aop, hi_u32 *real_delay_ms);
hi_s32 hal_aoe_aop_set_delay(aoe_aop_id aop, hi_u32 real_delay_ms);

/* ENGINE function */
hi_s32 hal_aoe_engine_create(aoe_engine_id *engine, aoe_engine_attr *attr);
hi_void hal_aoe_engine_destroy(aoe_engine_id engine);
hi_s32 hal_aoe_engine_set_attr(aoe_engine_id engine, aoe_engine_attr *attr);
hi_s32 hal_aoe_engine_get_attr(aoe_engine_id engine, aoe_engine_attr *attr);

hi_s32 hal_aoe_engine_start(aoe_engine_id engine);
hi_s32 hal_aoe_engine_stop(aoe_engine_id engine);
hi_void hal_aoe_engine_attach_aip(aoe_engine_id engine, aoe_aip_id aip);
hi_void hal_aoe_engine_detach_aip(aoe_engine_id engine, aoe_aip_id aip);
hi_u32 hal_aoe_engine_get_aop_attach_mask(aoe_engine_id engine);
hi_void hal_aoe_engine_attach_aop(aoe_engine_id engine, aoe_aop_id aop);
hi_void hal_aoe_engine_detach_aop(aoe_engine_id engine, aoe_aop_id aop);

hi_s32 hal_aoe_engine_set_avc_attr(aoe_engine_id engine, hi_ao_avc_attr *avc_attr);
hi_s32 hal_aoe_engine_set_avc_enable(aoe_engine_id engine, hi_bool enable);

hi_s32 hal_aoe_engine_set_geq_attr(aoe_engine_id engine, hi_ao_geq_attr *geq_attr);
hi_s32 hal_aoe_engine_set_geq_enable(aoe_engine_id engine, hi_bool enable);
hi_s32 hal_aoe_engine_set_geq_gain(aoe_engine_id engine, hi_u32 band, hi_s32 gain);

hi_s32 hal_aoe_engine_attach_aef(aoe_engine_id engine, hi_u32 aef_id);
hi_s32 hal_aoe_engine_detach_aef(aoe_engine_id engine, hi_u32 aef_id);

hi_s32 hal_aoe_engine_set_con_output_enable(aoe_engine_id engine, hi_bool enable);
hi_s32 hal_aoe_engine_set_output_atmos_enable(aoe_engine_id engine, hi_bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HAL_AOE_H__ */

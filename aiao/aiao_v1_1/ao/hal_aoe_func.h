/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aoe hal function layer header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __HAL_AOE_FUNC_H__
#define __HAL_AOE_FUNC_H__

#include "hal_aoe_common.h"
#include "hi_ao_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static inline hi_u32 aoe_reg_mix_route(hi_u32 id)
{
    return (id >= 16) ? 0 : (1 << id);
}

/* global function */
hi_s32  i_hal_aoe_init(hi_void);
hi_void i_hal_aoe_deinit(hi_void);

/* AIP function */
aip_regs_type *i_hal_aoe_aip_map_reg(aoe_aip_id aip);
hi_void i_hal_aoe_aip_set_attr(aip_regs_type *aip_reg, aoe_aip_chn_attr *attr);
hi_void i_hal_aoe_aip_set_adjust_speed(aip_regs_type *aip_reg, hi_s32 adj_speed);
hi_s32  i_hal_aoe_proc_cmd(adsp_cmd_arg *cmd);

hi_void i_hal_aoe_aip_set_prescale(aip_regs_type *aip_reg, hi_u32 int_db, hi_s32 dec_db);
hi_u64  i_hal_aoe_aip_get_reg_addr(aoe_aip_id aip);
hi_void i_hal_aoe_aip_set_add_mute_ms(aip_regs_type *aip_reg, hi_u32 mute_ms);
hi_void i_hal_aoe_aip_get_status_phy(aoe_aip_id aip, hi_u64 *phy_delay_addr);

static inline hi_s32 i_hal_aoe_aip_get_cmd_ack(aip_regs_type *aip_reg)
{
    return aip_reg->AIP_CTRL.bits.cmd_return_value;
}

static inline hi_bool i_hal_aoe_aip_get_cmd_done(aip_regs_type *aip_reg)
{
    return (aip_reg->AIP_CTRL.bits.cmd_done == 1);
}

static inline hi_void i_hal_aoe_aip_set_cmd(aip_regs_type *aip_reg, aoe_aip_cmd cmd)
{
    aip_reg->AIP_CTRL.bits.cmd = cmd;
    aip_reg->AIP_CTRL.bits.cmd_done = 0;
}

static inline hi_void i_hal_aoe_aip_set_volume(aip_regs_type *aip_reg, hi_u32 l_volume_db, hi_u32 r_volume_db)
{
    aip_reg->AIP_EXT_CTRL.bits.volume_l = l_volume_db;
    aip_reg->AIP_EXT_CTRL.bits.volume_r = r_volume_db;
}

static inline hi_void i_hal_aoe_aip_set_mute(aip_regs_type *aip_reg, hi_bool mute)
{
    aip_reg->AIP_EXT_CTRL.bits.mute = !!mute;
}

static inline hi_void i_hal_aoe_aip_set_channel_mode(aip_regs_type *aip_reg, hi_u32 channel_mode)
{
    aip_reg->AIP_EXT_CTRL.bits.channel_mode = channel_mode;
}

static inline hi_u32 i_hal_aoe_aip_get_fifo_delay_ms(aip_regs_type *aip_reg)
{
    return aip_reg->AIP_FIFO_ATTR.bits.fifo_latency_real;
}

static inline hi_void i_hal_aoe_aip_get_rptr_and_wptr_reg_addr(aip_regs_type *aip_reg,
                                                               hi_u32 **wptr_reg,
                                                               hi_u32 **rptr_reg)
{
    *wptr_reg = (hi_u32 *)(&(aip_reg->AIP_BUF_WPTR.ADDR_LOW));
    *rptr_reg = (hi_u32 *)(&(aip_reg->AIP_BUF_RPTR.ADDR_LOW));
}

static inline hi_void i_hal_aoe_aip_set_eos(aip_regs_type *aip_reg, hi_bool eos)
{
    aip_reg->AIP_EXT3_CTRL.bits.eos = !!eos;
}

#ifdef HI_SND_ADVANCED_SUPPORT
static inline hi_void i_hal_aoe_aip_set_fifo_bypass(aip_regs_type *aip_reg, hi_bool enable)
{
    aip_reg->AIP_FIFO_ATTR.bits.fifo_bypass = !!enable;
}

static inline hi_void i_hal_aoe_aip_set_mix_priority(aip_regs_type *aip_reg, hi_bool mix_priority)
{
    aip_reg->AIP_BUFF_ATTR.bits.aip_priority = !!mix_priority;
}

static inline hi_void i_hal_aoe_aip_set_fifo_latency(aip_regs_type *aip_reg, hi_u32 latency_ms)
{
    aip_reg->AIP_FIFO_ATTR.bits.fifo_latency = latency_ms;
}

static inline hi_void i_hal_aoe_aip_set_fade(aip_regs_type *aip_reg, hi_bool enable, hi_u32 fadein_ms,
                                             hi_u32 fade_out_ms)
{
    aip_reg->AIP_CTRL.bits.fade_en = enable;
}

static inline hi_void i_hal_aoe_aip_set_resume_threshold_ms(aip_regs_type *aip_reg, hi_u32 threshold_ms)
{
    aip_reg->AIP_EXT3_CTRL.bits.resume_threshold = threshold_ms;
}
#endif

/* AOP function */
aop_regs_type *i_hal_aoe_aop_map_reg(aoe_aop_id aop);

static inline hi_s32 i_hal_aoe_aop_get_cmd_ack(aop_regs_type *aop_reg)
{
    return aop_reg->AOP_CTRL.bits.cmd_return_value;
}

static inline hi_bool i_hal_aoe_aop_get_cmd_done(aop_regs_type *aop_reg)
{
    return (aop_reg->AOP_CTRL.bits.cmd_done == 1);
}

static inline hi_void i_hal_aoe_aop_set_cmd(aop_regs_type *aop_reg, aoe_aop_cmd cmd)
{
    aop_reg->AOP_CTRL.bits.cmd = cmd;
    aop_reg->AOP_CTRL.bits.cmd_done = 0;
}

static inline hi_void i_hal_aoe_aop_set_volume(aop_regs_type *aop_reg, hi_u32 l_volume_db, hi_u32 r_volume_db)
{
    aop_reg->AOP_EXT_CTRL.bits.volume_l = l_volume_db;
    aop_reg->AOP_EXT_CTRL.bits.volume_r = r_volume_db;
}

static inline hi_void i_hal_aoe_aop_set_mute(aop_regs_type *aop_reg, hi_bool mute)
{
    aop_reg->AOP_EXT_CTRL.bits.mute = !!mute;
}

hi_void i_hal_aoe_aop_set_preci_vol(aop_regs_type *aop_reg, hi_u32 int_db, hi_s32 dec_db);
hi_void i_hal_aoe_aop_set_attr(aop_regs_type *aop_reg, aoe_aop_chn_attr *attr);

#ifdef HI_SND_ADVANCED_SUPPORT
static inline hi_void i_hal_aoe_aop_set_balance(aop_regs_type *aop_reg, hi_s32 balance)
{
    if (balance >= 0) {
        aop_reg->AOP_EXT_CTRL.bits.balance_dir = 0;
        aop_reg->AOP_EXT_CTRL.bits.balance_val = balance;
    } else {
        aop_reg->AOP_EXT_CTRL.bits.balance_dir = 1;
        aop_reg->AOP_EXT_CTRL.bits.balance_val = -balance;
    }
}
#endif

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
static inline hi_void i_hal_aoe_aop_get_rptr_and_wptr_reg_addr(aop_regs_type *aop_reg,
                                                               hi_void **wptr_reg,
                                                               hi_void **rptr_reg)
{
    *wptr_reg = (hi_void *)(&(aop_reg->AOP_BUF_WPTR));
    *rptr_reg = (hi_void *)(&(aop_reg->AOP_BUF_RPTR));
}
#endif

static inline hi_void i_hal_aoe_aop_set_aef_bypass(aop_regs_type *aop_reg, hi_bool bypass)
{
    aop_reg->AOP_CTRL.bits.ena_aef_bypass = !!bypass;
}

#ifdef HI_SND_DRC_SUPPORT
static inline hi_void i_hal_aoe_aop_set_drc_enable(aop_regs_type *aop_reg, hi_bool enable)
{
    aop_reg->AOP_CTRL.bits.ena_drc = !!enable;
}

static inline hi_void i_hal_aoe_aop_set_drc_change_attr(aop_regs_type *aop_reg, hi_bool enable)
{
    aop_reg->AOP_CTRL.bits.drc_change_attr = !!enable;
}

static inline hi_void i_hal_aoe_aop_set_drc_attr(aop_regs_type *aop_reg, hi_ao_drc_attr *drc_attr)
{
    U_AOP_DRC_TIME_ATTR *drc_time_reg = (U_AOP_DRC_TIME_ATTR *)(&aop_reg->AOP_DRC_TIME_ATTR);
    U_AOP_DRC_LEVEL_ATTR *drc_levele_reg = (U_AOP_DRC_LEVEL_ATTR *)(&aop_reg->AOP_DRC_LEVEL_ATTR);

    drc_time_reg->bits.attack_time = drc_attr->attack_time;
    drc_time_reg->bits.release_time = drc_attr->release_time;
    drc_levele_reg->bits.start_level = -(drc_attr->threshold);
    drc_levele_reg->bits.targert_level = -(drc_attr->limit);
}
#endif

#ifdef HI_SND_PEQ_SUPPORT
hi_void i_hal_aoe_aop_set_peq_attr(aop_regs_type *aop_reg, hi_ao_peq_attr *peq_attr);
static inline hi_void i_hal_aoe_aop_set_peq_enable(aop_regs_type *aop_reg, hi_bool enable)
{
    aop_reg->AOP_CTRL.bits.ena_peq = !!enable;
}

static inline hi_void i_hal_aoe_aop_set_peq_change_attr(aop_regs_type *aop_reg, hi_bool enable)
{
    aop_reg->AOP_CTRL.bits.peq_change_attr = !!enable;
}
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
static inline hi_void i_hal_aoe_aop_set_latency(aop_regs_type *aop_reg, hi_u32 latency)
{
    aop_reg->AOP_BUFF_ATTR.bits.buf_latency = latency;
}

static inline hi_void i_hal_aoe_aop_get_latency(aop_regs_type *aop_reg, hi_u32 *latency)
{
    *latency = aop_reg->AOP_BUFF_ATTR.bits.buf_latency;
}

static inline hi_void i_hal_aoe_aop_set_ad_output_enable(aop_regs_type *aop_reg, hi_bool enable)
{
    aop_reg->AOP_CTRL.bits.ena_ad = !!enable;
}
#endif

#if defined (HI_SOUND_PORT_DELAY_SUPPORT)
static inline hi_void i_hal_aoe_aop_set_delay(aop_regs_type *aop_reg, hi_u32 real_delay_ms)
{
    aop_reg->AOP_CTRL.bits.realdelayms = real_delay_ms;
}

static inline hi_void i_hal_aoe_aop_get_delay(aop_regs_type *aop_reg, hi_u32 *real_delay_ms)
{
    *real_delay_ms = aop_reg->AOP_CTRL.bits.realdelayms;
}
#endif

/* ENGINE function */
engine_regs_type *i_hal_aoe_engine_map_reg(aoe_engine_id engine);
hi_void i_hal_aoe_engine_set_attr(engine_regs_type *engine_reg, aoe_engine_attr *attr);

static inline hi_s32 i_hal_aoe_engine_get_cmd_ack(engine_regs_type *engine_reg)
{
    return engine_reg->ENGINE_CTRL.bits.cmd_return_value;
}

static inline hi_bool i_hal_aoe_engine_get_cmd_done(engine_regs_type *engine_reg)
{
    return (engine_reg->ENGINE_CTRL.bits.cmd_done == 1);
}

static inline hi_void i_hal_aoe_engine_set_cmd(engine_regs_type *engine_reg, aoe_engine_cmd cmd)
{
    engine_reg->ENGINE_CTRL.bits.cmd = cmd;
    engine_reg->ENGINE_CTRL.bits.cmd_done = 0;
}

#ifdef HI_SND_AVC_SUPPORT
static inline hi_void i_hal_aoe_engine_set_avc_attr(engine_regs_type *engine_reg, hi_ao_avc_attr *avc_attr)
{
    U_ENGINE_AVC_TIME_ATTR *avc_time_reg = (U_ENGINE_AVC_TIME_ATTR *)(&engine_reg->ENGINE_AVC_TIME_ATTR);
    U_ENGINE_AVC_LEVEL_ATTR *avc_level_reg = (U_ENGINE_AVC_LEVEL_ATTR *)(&engine_reg->ENGINE_AVC_LEVEL_ATTR);

    avc_time_reg->bits.attack_time = avc_attr->attack_time;
    avc_time_reg->bits.release_time = avc_attr->release_time;
    avc_level_reg->bits.limiter_level = -(avc_attr->limit);
    avc_level_reg->bits.threshold_level = -(avc_attr->threshold);
    avc_level_reg->bits.gain = avc_attr->gain;
}

static inline hi_void i_hal_aoe_engine_set_avc_enable(engine_regs_type *engine_reg, hi_bool enable)
{
    engine_reg->ENGINE_CTRL.bits.ena_avc = !!enable;
}

static inline hi_void i_hal_aoe_engine_set_avc_change_attr(engine_regs_type *engine_reg, hi_bool enable)
{
    engine_reg->ENGINE_CTRL.bits.avc_change_attr = !!enable;
}
#endif

#ifdef HI_SND_GEQ_SUPPORT
hi_void i_hal_aoe_engine_set_geq_attr(engine_regs_type *engine_reg, hi_ao_geq_attr *geq_attr);
hi_void i_hal_aoe_engine_set_geq_gain(engine_regs_type *engine_reg, hi_u32 band, hi_s32 gain);

static inline hi_void i_hal_aoe_engine_set_geq_enable(engine_regs_type *engine_reg, hi_bool enable)
{
    engine_reg->ENGINE_CTRL.bits.ena_geq = !!enable;
}

static inline hi_void i_hal_aoe_aop_set_geq_change_attr(engine_regs_type *engine_reg, hi_bool enable)
{
    engine_reg->ENGINE_CTRL.bits.geq_change_attr = !!enable;
}
#endif

static inline hi_void i_hal_aoe_engine_attach_aip(engine_regs_type *engine_reg, aoe_aip_id aip)
{
    engine_reg->ENGINE_MIX_SRC.bits.aip_fifo_ena |= aoe_reg_mix_route(aip);
}

static inline hi_void i_hal_aoe_engine_detach_aip(engine_regs_type *engine_reg, aoe_aip_id aip)
{
    engine_reg->ENGINE_MIX_SRC.bits.aip_fifo_ena &= ~(aoe_reg_mix_route(aip));
}

static inline hi_u32 i_hal_aoe_engine_get_aop_attach_mask(engine_regs_type *engine_reg)
{
    return engine_reg->ENGINE_ROU_DST.bits.aop_buf_ena;
}

static inline hi_void i_hal_aoe_engine_attach_aop(engine_regs_type *engine_reg, aoe_aop_id aop)
{
    engine_reg->ENGINE_ROU_DST.bits.aop_buf_ena |= aoe_reg_mix_route(aop);
}

static inline hi_void i_hal_aoe_engine_detach_aop(engine_regs_type *engine_reg, aoe_aop_id aop)
{
    engine_reg->ENGINE_ROU_DST.bits.aop_buf_ena &= ~(aoe_reg_mix_route(aop));
}

#ifdef HI_SND_AEF_SUPPORT
static inline hi_void i_hal_aoe_engine_attach_aef(engine_regs_type *engine_reg, hi_u32 aef_id)
{
    engine_reg->ENGINE_ATT_AEF.bits.aef_att_ena |= aoe_reg_mix_route(aef_id);
}

static inline hi_void i_hal_aoe_engine_detach_aef(engine_regs_type *engine_reg, hi_u32 aef_id)
{
    engine_reg->ENGINE_ATT_AEF.bits.aef_att_ena &= ~(aoe_reg_mix_route(aef_id));
}
#endif

#ifdef HI_SND_AR_SUPPORT
static inline hi_void i_hal_aoe_engine_set_con_output_enable(engine_regs_type *engine_reg, hi_bool enable)
{
    engine_reg->ENGINE_CTRL.bits.ena_continue_output = !!enable;
}

static inline hi_void i_hal_aoe_engine_set_output_atmos_enable(engine_regs_type *engine_reg, hi_bool enable)
{
    engine_reg->ENGINE_CTRL.bits.ena_output_atmos = !!enable;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HAL_AOE_FUNC_H__ */

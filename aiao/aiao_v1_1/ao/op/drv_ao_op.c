/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao port function
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"
#include "hi_reg_common.h"

#include "drv_ao_op.h"
#include "audio_util.h"

#include "i2s_func.h"
#include "arc_func.h"
#include "hdmi_func.h"
#include "spdif_func.h"
#include "adac_func.h"
#include "aef_func.h"
#include "drc_func.h"
#include "peq_func.h"

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
static hi_s32 snd_op_get_delay_compensation(snd_op_state *snd_op, hi_u32 *real_delay_ms);
static hi_s32 snd_op_set_delay_compensation(snd_op_state *snd_op, hi_u32 real_delay_ms);
#endif

static OSAL_LIST_HEAD(g_op_driver_list);

hi_void snd_op_init(snd_op_state *snd_op)
{
    hi_u32 i;

    for (i = 0; i < AO_SNDOP_MAX_AOP_NUM; i++) {
        snd_op->port_id[i] = AIAO_PORT_MAX;
        snd_op->aop[i] = AOE_AOP_MAX;
        snd_op->engine_type[i] = TYPE_MAX;
    }

    OSAL_INIT_LIST_HEAD(&snd_op->node);
}

hi_void snd_op_register_driver(hi_void)
{
    struct osal_list_head *list = &g_op_driver_list;

    OSAL_INIT_LIST_HEAD(list);

    adac_op_register_driver(list);
    arc_op_register_driver(list);
    hdmi_op_register_driver(list);
    spdif_op_register_driver(list);
    i2s_op_register_driver(list);
}

static snd_output_port_driver *snd_op_get_driver(hi_ao_port out_port)
{
    snd_output_port_driver *driver = HI_NULL;

    osal_list_for_each_entry(driver, &g_op_driver_list, node) {
        if (driver->match == HI_NULL) {
            continue;
        }

        if (driver->match(out_port) == HI_TRUE) {
            return driver;
        }
    }

    return HI_NULL;
}

snd_op_state *snd_get_op_handle_by_out_port(snd_card_state *card, hi_ao_port out_port)
{
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if (snd_op->out_port == out_port) {
            return snd_op;
        }
    }

    return HI_NULL;
}

hi_s32 snd_op_start(snd_op_state *snd_op)
{
    hi_s32 ret;
    aiao_port_id port = snd_op->port_id[snd_op->active_id];
    aoe_aop_id aop = snd_op->aop[snd_op->active_id];

    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST) {
        return HI_ERR_AO_INVALID_PARA;
    }

    if (snd_op->status == SND_OP_STATUS_START) {
        return HI_SUCCESS;
    }

    ret = hal_aiao_start(port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_start, ret);
        return HI_FAILURE;
    }

    ret = hal_aoe_aop_start(aop);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_start, ret);
        return HI_FAILURE;
    }

    snd_op->status = SND_OP_STATUS_START;
    return HI_SUCCESS;
}

hi_s32 snd_op_stop(snd_op_state *snd_op)
{
    hi_s32 ret;
    aiao_port_id port = snd_op->port_id[snd_op->active_id];
    aoe_aop_id aop = snd_op->aop[snd_op->active_id];

    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST) {
        return HI_ERR_AO_INVALID_PARA;
    }

    if (snd_op->status == SND_OP_STATUS_STOP) {
        return HI_SUCCESS;
    }

    ret = hal_aiao_stop(port, AIAO_STOP_IMMEDIATE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_stop, ret);
        return HI_FAILURE;
    }

    ret = hal_aoe_aop_stop(aop);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_stop, ret);
        return HI_FAILURE;
    }

    snd_op->status = SND_OP_STATUS_STOP;
    return HI_SUCCESS;
}

static hi_void snd_op_destroy_aop(snd_op_state *snd_op, snd_aop_type type)
{
    hal_aoe_aop_destroy(snd_op->aop[type]);
    hal_aiao_close(snd_op->port_id[type]);

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
    /*
     * in HDMI_I2S_SPDIF_MUX mode, spdif port of HDMI0 use the same mmz of i2s port
     * here we only need to free mmz at i2s port
     */
    if ((snd_op->out_port == HI_AO_PORT_HDMI0 || snd_op->out_port == HI_AO_PORT_HDMI1) &&
        (type == SND_AOP_TYPE_SPDIF)) {
        return;
    }
#endif

    hi_drv_audio_mmz_release(&snd_op->rbf_mmz[type]);
}

hi_void snd_op_destroy(snd_op_state *snd_op, hi_bool suspend)
{
    hi_s32 ret;
    snd_aop_type type;

    if (snd_op->op_mask == 0) {
        return;
    }

    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST) {
        return;
    }

    ret = snd_op_stop(snd_op);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_stop, ret);
        return;
    }

    for (type = SND_AOP_TYPE_I2S; type < SND_AOP_TYPE_CAST; type++) {
        if (snd_op->op_mask & (1 << type)) {
            snd_op_destroy_aop(snd_op, type);
        }
    }

    osal_list_del(&snd_op->node);
}

hi_void ao_snd_destroy_op(snd_card_state *card, hi_bool suspend)
{
    snd_op_state *snd_op = HI_NULL;
    snd_op_state *tmp = HI_NULL;

    osal_list_for_each_entry_safe(snd_op, tmp, &card->op, node) {
        snd_output_port_driver *driver = snd_op_get_driver(snd_op->out_port);
        if (driver == HI_NULL || driver->destroy == HI_NULL) {
            continue;
        }

        driver->destroy(snd_op, suspend);
    }
}

hi_s32 ao_snd_create_op(snd_card_state *card, hi_ao_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    hi_u32 port_num;

    for (port_num = 0; port_num < attr->port_num; port_num++) {
        snd_output_port_driver *driver = snd_op_get_driver(attr->outport[port_num].port);
        if (driver == HI_NULL || driver->create == HI_NULL) {
            continue;
        }

        ret = driver->create(card, &attr->outport[port_num], resume);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(driver->create, ret);
            goto out;
        }
    }

    ret = memcpy_s(&card->user_open_param, sizeof(hi_ao_attr), attr, sizeof(hi_ao_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        goto out;
    }

    return ret;

out:
    ao_snd_destroy_op(card, HI_FALSE);
    return ret;
}

hi_bool ao_snd_check_output(snd_card_state *card, hi_u32 data_type)
{
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if (snd_op->snd_port_attr.data_type == data_type) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

hi_s32 snd_op_get_setting(snd_card_state *card, hi_ao_port port, snd_outport_attr *port_attr)
{
    hi_s32 ret;
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    port_attr->curn_status = snd_op->status;
    port_attr->user_mute = snd_op->user_mute;
    port_attr->user_track_mode = snd_op->user_track_mode;
    port_attr->delay_ms = snd_op->delay_ms;
    port_attr->latency_ms = snd_op->latency_ms;
    port_attr->bypass = snd_op->bypass;

    ret = memcpy_s(&port_attr->user_gain, sizeof(hi_ao_gain), &snd_op->user_gain, sizeof(hi_ao_gain));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    port_attr->user_balance = snd_op->user_balance;
    ret = memcpy_s(&port_attr->user_preci_gain, sizeof(hi_ao_preci_gain), &snd_op->user_preci_gain,
        sizeof(hi_ao_preci_gain));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

#if defined(HI_SND_DRC_SUPPORT)
    ret = memcpy_s(&port_attr->drc_attr, sizeof(hi_ao_drc_attr), &snd_op->drc_attr, sizeof(hi_ao_drc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    port_attr->drc_enable = snd_op->drc_enable;
#endif

    return HI_SUCCESS;
}

hi_s32 snd_get_op_setting(snd_card_state *card, snd_card_settings *snd_settings)
{
    hi_s32 ret;
    hi_u32 port;

    snd_settings->user_hdmi_mode = card->user_hdmi_mode;
    snd_settings->user_spdif_mode = card->user_spdif_mode;
    snd_settings->adac_enable = card->adac_enable;

#if defined(HI_SND_ARC_SUPPORT)
    snd_settings->user_arc_enable = card->user_arc_enable;
    snd_settings->user_arc_mode = card->user_arc_mode;
    ret = memcpy_s(&snd_settings->user_arc_cap, sizeof(hi_ao_arc_audio_cap), &card->user_arc_cap,
        sizeof(hi_ao_arc_audio_cap));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }
#endif

#if defined(HI_SND_AR_SUPPORT)
    snd_settings->con_output_enable = card->con_output_enable;
    snd_settings->output_atmos_enable = card->output_atmos_enable;
#endif

    ret = memcpy_s(&snd_settings->user_open_param, sizeof(hi_ao_attr), &card->user_open_param, sizeof(hi_ao_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    for (port = 0; port < card->user_open_param.port_num; port++) {
        ret = snd_op_get_setting(card, card->user_open_param.outport[port].port, &snd_settings->port_attr[port]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_get_setting, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 snd_op_restore_setting(snd_card_state *card, hi_ao_port port, snd_outport_attr *port_attr)
{
    hi_s32 ret;
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    if (port_attr->curn_status == SND_OP_STATUS_START) {
        ret = snd_op_start(snd_op);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_start, ret);
            return ret;
        }
    } else if (port_attr->curn_status == SND_OP_STATUS_STOP) {
        ret = snd_op_stop(snd_op);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_stop, ret);
            return ret;
        }
    }

    ret = snd_op_set_mute(snd_op, port_attr->user_mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_mute, ret);
        return ret;
    }

    ret = snd_op_set_track_mode(snd_op, port_attr->user_track_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_track_mode, ret);
        return ret;
    }

    ret = snd_op_set_volume(snd_op, &port_attr->user_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_volume, ret);
        return ret;
    }

#ifdef HI_SND_AEF_SUPPORT
    ret = snd_op_set_aef_bypass(snd_op, port_attr->bypass);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_aef_bypass, ret);
        return ret;
    }
#endif

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
    ret = snd_op_set_delay_compensation(snd_op, port_attr->delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_delay_compensation, ret);
        return ret;
    }
#endif

    ret = snd_op_set_preci_vol(snd_op, &port_attr->user_preci_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_preci_vol, ret);
        return ret;
    }

#ifdef HI_SND_ADVANCED_SUPPORT
    ret = snd_op_set_low_latency(snd_op, port_attr->latency_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_low_latency, ret);
        return ret;
    }

    ret = snd_op_set_balance(snd_op, port_attr->user_balance);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_balance, ret);
        return ret;
    }
#endif

#ifdef HI_SND_DRC_SUPPORT
    ret = snd_op_set_drc_enable(snd_op, port_attr->drc_enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_drc_enable, ret);
        return ret;
    }

    ret = snd_op_set_drc_attr(snd_op, &port_attr->drc_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_drc_attr, ret);
        return ret;
    }
#endif

    return ret;
}

static inline hi_void aiao_rbuf_to_aoe_rbuf(aiao_rbuf_attr *aiao_rbf, aoe_rbuf_attr *aoe_rbf)
{
    aoe_rbf->buf_phy_addr = aiao_rbf->buf_phy_addr;
    aoe_rbf->buf_vir_addr = aiao_rbf->buf_vir_addr;
    aoe_rbf->buf_phy_wptr = aiao_rbf->buf_phy_wptr;
    aoe_rbf->buf_vir_wptr = aiao_rbf->buf_vir_wptr;
    aoe_rbf->buf_phy_rptr = aiao_rbf->buf_phy_rptr;
    aoe_rbf->buf_vir_rptr = aiao_rbf->buf_vir_rptr;
    aoe_rbf->buf_size = aiao_rbf->buf_size;
}

static hi_void cal_buffer_attr(hi_audio_buffer *rbf_mmz, aiao_buf_attr *buf_attr, aiao_mem_attr *port_mem)
{
    hi_u32 x = buf_attr->period_buf_size;
    hi_u32 y = 1;

    while ((x << y) <= rbf_mmz->size) {
        y++;
    }

    y--;

    port_mem->buf_phy_addr = rbf_mmz->phys_addr;
    port_mem->buf_vir_addr = rbf_mmz->virt_addr - (hi_u8 *)HI_NULL;
    port_mem->buf_size = (x << y);

    buf_attr->period_number = (1 << y);
}

static hi_s32 snd_op_get_create_hw_attr(snd_op_state *snd_op, hi_ao_port_attr *attr, snd_aop_type aop_type,
    aiao_port_user_cfg *hw_port_attr)
{
    hi_s32 ret;
    hi_audio_buffer rbf_mmz;

    snd_op_create_param hw_param = {
        .ao_port = attr->port,
        .aop_type = aop_type,
        .buf_name = HI_NULL,
        .aiao_port = AIAO_PORT_MAX,
        .port_cfg = hw_port_attr,
    };

    /* step1: get port driver */
    snd_output_port_driver *driver = snd_op_get_driver(attr->port);

    if (driver == HI_NULL || driver->get_hw_param == HI_NULL) {
        HI_LOG_ERR("call snd_op_get_driver failed\n");
        return HI_FAILURE;
    }

    /* step2: get port hw_param by driver */
    ret = driver->get_hw_param(attr, aop_type, &hw_param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(driver->get_hw_param, ret);
        return ret;
    }

    if ((attr->port == HI_AO_PORT_HDMI0 || attr->port == HI_AO_PORT_HDMI1) && aop_type == SND_AOP_TYPE_SPDIF) {
#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
        aiao_rbuf_attr rbf_attr = { 0 };
        ret = hal_aiao_get_rbf_attr(snd_op->port_id[0], &rbf_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_get_rbf_attr, ret);
            return HI_FAILURE;
        }

        rbf_mmz.phys_addr = rbf_attr.buf_phy_addr;
        rbf_mmz.virt_addr = (hi_u8 *)HI_NULL + rbf_attr.buf_vir_addr;
        rbf_mmz.size = AO_HDMI_MMZSIZE_MAX;
#endif
    } else {
        /* alloc audio mmz buf to port */
        ret = hi_drv_audio_mmz_alloc(hw_param.buf_name, hw_param.buf_size, HI_FALSE, &rbf_mmz);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hi_drv_audio_mmz_alloc, ret);
            return HI_FAILURE;
        }
    }

    /* save port hw hw_param */
    hw_port_attr->ext_dma_mem = HI_TRUE;
    cal_buffer_attr(&rbf_mmz, &hw_port_attr->buf_config, &hw_port_attr->ext_mem);

    snd_op->port_id[aop_type] = hw_param.aiao_port;
    snd_op->rbf_mmz[aop_type] = rbf_mmz;

    return HI_SUCCESS;
}

hi_s32 snd_op_get_attr(snd_op_state *snd_op, snd_op_attr *snd_port_attr)
{
    hi_s32 ret;
    if (snd_op == HI_NULL || snd_port_attr == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    ret = memcpy_s(snd_port_attr, sizeof(snd_op_attr), &snd_op->snd_port_attr, sizeof(snd_op_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
    }

    return ret;
}

static hi_s32 snd_op_set_aiao_attr(snd_op_state *snd_op, snd_op_attr *snd_port_attr)
{
    hi_s32 ret;
    aiao_port_attr aiao_attr;
    aiao_port_id port_id;

    port_id = snd_op->port_id[snd_op->active_id];

    ret = hal_aiao_get_attr(port_id, &aiao_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_get_attr, ret);
        return ret;
    }

    aiao_attr.if_attr.bit_depth = (aiao_bit_depth)snd_port_attr->bit_per_sample;
    aiao_attr.if_attr.ch_num = (aiao_i2s_chnum)snd_port_attr->channels;
    aiao_attr.if_attr.rate = (aiao_sample_rate)snd_port_attr->sample_rate;
    if (snd_port_attr->data_format & 0xff) {
        hal_aiao_set_bypass(port_id, HI_TRUE);
        if ((snd_port_attr->data_format & 0xff) == IEC61937_DATATYPE_DTSCD) {
            hal_aiao_set_mode(port_id, HI_FALSE);
        } else {
            hal_aiao_set_mode(port_id, HI_TRUE);
        }
    } else {
        hal_aiao_set_bypass(port_id, HI_FALSE);
        hal_aiao_set_mode(port_id, HI_FALSE);
    }

    snd_op->engine_type[snd_op->active_id] = snd_port_attr->data_type;

    ret = hal_aiao_set_attr(port_id, &aiao_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_set_attr, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 snd_op_set_aop_attr(snd_op_state *snd_op, snd_op_attr *snd_port_attr)
{
    hi_s32 ret;
    aoe_aop_chn_attr aop_attr;
    aiao_rbuf_attr rbf_attr;
    aoe_aop_id aop;
    aiao_port_id port_id;

    aop = snd_op->aop[snd_op->active_id];
    port_id = snd_op->port_id[snd_op->active_id];

    ret = memset_s(&aop_attr, sizeof(aoe_aop_chn_attr), 0, sizeof(aoe_aop_chn_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    ret = hal_aoe_aop_get_attr(aop, &aop_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_get_attr, ret);
        return ret;
    }

    ret = memset_s(&rbf_attr, sizeof(aiao_rbuf_attr), 0, sizeof(aiao_rbuf_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    ret = hal_aiao_get_rbf_attr(port_id, &rbf_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_get_rbf_attr, ret);
        return ret;
    }

    aiao_rbuf_to_aoe_rbuf(&rbf_attr, &aop_attr.rbf_out_attr.rbf_attr);
    aop_attr.rbf_out_attr.buf_latency_thd_ms = snd_op->latency_ms;
    aop_attr.rbf_out_attr.buf_bit_per_sample = snd_port_attr->bit_per_sample;
    aop_attr.rbf_out_attr.buf_channels = snd_port_attr->channels;
    aop_attr.rbf_out_attr.buf_sample_rate = snd_port_attr->sample_rate;
    aop_attr.rbf_out_attr.buf_data_format = snd_port_attr->data_format;
    ret = hal_aoe_aop_set_attr(aop, &aop_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_attr, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 hdmi_op_set_attr(snd_op_state *snd_op, snd_op_attr *snd_port_attr)
{
#ifdef HI_SND_HDMI_PASSTHROUGH_USE_I2S_ONLY
    snd_op->active_id = SND_AOP_TYPE_I2S;
#else

    aiao_port_id port_id;

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
    hi_s32 ret;
    aiao_op_type op_type = SND_OP_TYPE_I2S;
#endif

    if ((snd_port_attr->data_format & 0xff) == 0) {
        snd_op->active_id = SND_AOP_TYPE_I2S; /* 2.0 pcm */
    } else if (autil_is_iec61937_hbr(snd_port_attr->data_format & 0xff, snd_port_attr->sample_rate)) {
        snd_op->active_id = SND_AOP_TYPE_I2S;
        if ((snd_port_attr->data_format & 0xff) == IEC61937_DATATYPE_DOLBY_DIGITAL_PLUS) {
            snd_op->active_id = SND_AOP_TYPE_SPDIF; /* lbr or hbr(ddp) */
#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
            op_type = SND_OP_TYPE_SPDIF;
#endif
        }
    } else if (((snd_port_attr->data_format & 0xff) == IEC61937_DATATYPE_71_LPCM) ||
        ((snd_port_attr->data_format & 0xff) == IEC61937_DATATYPE_20_LPCM)) {
        snd_op->active_id = SND_AOP_TYPE_I2S; /* 7.1 lpcm */
    } else {
        snd_op->active_id = SND_AOP_TYPE_SPDIF; /* lbr or hbr(ddp) */
#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
        op_type = SND_OP_TYPE_SPDIF;
#endif
    }

    port_id = snd_op->port_id[snd_op->active_id];

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
    ret = hal_aiao_set_op_type(port_id, op_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_set_op_type, ret);
        return HI_FAILURE;
    }
#endif

#endif /* HI_SND_HDMI_PASSTHROUGH_USE_I2S_ONLY */

    return HI_SUCCESS;
}

hi_s32 snd_op_set_attr(snd_op_state *snd_op, snd_op_attr *snd_port_attr)
{
    hi_s32 ret;

    if (snd_op->status != SND_OP_STATUS_STOP) {
        return HI_FAILURE;
    }

    /* note: only spdif & hdmi support set attr as pass-through switch */
    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST || snd_op->out_type == SND_OUTPUT_TYPE_DAC ||
        snd_op->out_type == SND_OUTPUT_TYPE_I2S) {
        return HI_FAILURE;
    }

    if (snd_op->out_type == SND_OUTPUT_TYPE_HDMI) {
        ret = hdmi_op_set_attr(snd_op, snd_port_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hdmi_op_set_attr, ret);
            return ret;
        }
    }

    ret = snd_op_set_aiao_attr(snd_op, snd_port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_aiao_attr, ret);
        return ret;
    }

    ret = snd_op_set_aop_attr(snd_op, snd_port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_aop_attr, ret);
        return ret;
    }

    return memcpy_s(&snd_op->snd_port_attr, sizeof(snd_op_attr), snd_port_attr, sizeof(snd_op_attr));
}

static hi_s32 snd_op_open_aop(aiao_port_id port, aiao_port_user_cfg *hw_port_attr, aoe_aop_id *aop)
{
    hi_s32 ret;
    aoe_aop_chn_attr aop_attr;
    aiao_rbuf_attr rbf_attr = { 0 };

    ret = hal_aiao_get_rbf_attr(port, &rbf_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_get_rbf_attr, ret);
        return ret;
    }

    aiao_rbuf_to_aoe_rbuf(&rbf_attr, &aop_attr.rbf_out_attr.rbf_attr);
    aop_attr.rbf_out_attr.rbf_attr.buf_wptr_rptr_flag = 1;
    aop_attr.rbf_out_attr.buf_bit_per_sample = hw_port_attr->if_attr.bit_depth;
    aop_attr.rbf_out_attr.buf_channels = hw_port_attr->if_attr.ch_num;
    aop_attr.rbf_out_attr.buf_sample_rate = hw_port_attr->if_attr.rate;
    aop_attr.rbf_out_attr.buf_data_format = 0;
    aop_attr.rbf_out_attr.buf_latency_thd_ms = AOE_AOP_BUFF_LATENCYMS_DF;
    aop_attr.rbf_hw_priority = HI_TRUE;
    aop_attr.is_cast = HI_FALSE;
    aop_attr.add_mute = HI_FALSE;

    ret = hal_aoe_aop_create(aop, &aop_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_create, ret);
    }

    return ret;
}

static inline hi_void snd_op_set_def_attr(snd_op_state *snd_op, aiao_port_user_cfg *hw_port_attr)
{
    snd_op->snd_port_attr.channels = hw_port_attr->if_attr.ch_num;
    snd_op->snd_port_attr.sample_rate = hw_port_attr->if_attr.rate;
    snd_op->snd_port_attr.bit_per_sample = hw_port_attr->if_attr.bit_depth;
    snd_op->snd_port_attr.data_format = 0;
    snd_op->snd_port_attr.data_type = PCM;
    snd_op->snd_port_attr.latency_thd_ms = AOE_AOP_BUFF_LATENCYMS_DF;
}

hi_s32 snd_op_create_aop(snd_op_state *snd_op, hi_ao_port_attr *attr, snd_aop_type aop_type, hi_sample_rate sample_rate)
{
    hi_s32 ret;
    aiao_port_user_cfg hw_port_attr;
    aoe_aop_id aop = AOE_AOP_MAX;
    aiao_port_id port;
    hi_audio_buffer rbf_mmz = { 0 };

    if (aop_type >= SND_AOP_TYPE_CAST) {
        HI_LOG_ERR("Invalid aop type\n");
        HI_ERR_PRINT_H32(aop_type);
        return HI_ERR_AO_INVALID_PARA;
    }

    ret = snd_op_get_create_hw_attr(snd_op, attr, aop_type, &hw_port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_get_create_hw_attr, ret);
        return ret;
    }

    /* use hw port_id and rbf_mmz */
    port = snd_op->port_id[aop_type];
    rbf_mmz = snd_op->rbf_mmz[aop_type];

    hw_port_attr.if_attr.rate = (hi_u32)sample_rate;

    ret = hal_aiao_open(port, &hw_port_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_open, ret);
        goto snd_mmz_release_err_exit;
    }

    ret = snd_op_open_aop(port, &hw_port_attr, &aop);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_open_aop, ret);
        goto snd_close_port_err_exit;
    }

    snd_op_set_def_attr(snd_op, &hw_port_attr);
    snd_op_set_def_aef_attr(snd_op, aop);

    snd_op->op_mask |= (1 << aop_type);
    snd_op->aop[aop_type] = aop;
    snd_op->engine_type[aop_type] = PCM;

    /* spdif interface of hdmi don't need to start aop */
    if (((attr->port != HI_AO_PORT_HDMI0) && (attr->port != HI_AO_PORT_HDMI1)) || (aop_type != SND_AOP_TYPE_SPDIF)) {
        /* as default, spdif interface of hdmi is not active */
        snd_op->active_id = aop_type;
        ret = snd_op_start(snd_op);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_start, ret);
            goto snd_destroy_aop_err_exit;
        }
    }

    snd_op->out_port = attr->port;
    snd_op->status = SND_OP_STATUS_START;
    snd_op->user_mute = 0;
    snd_op->latency_ms = AOE_AOP_BUFF_LATENCYMS_DF;
    snd_op->user_track_mode = HI_TRACK_MODE_STEREO;
    snd_op->user_gain.linear_mode = HI_FALSE;
    snd_op->user_gain.gain = 0;

    return HI_SUCCESS;

snd_destroy_aop_err_exit:
    hal_aoe_aop_destroy(aop);
snd_close_port_err_exit:
    hal_aiao_close(port);
snd_mmz_release_err_exit:
    hi_drv_audio_mmz_release(&rbf_mmz);

    return ret;
}

hi_s32 snd_restore_op_setting(snd_card_state *card, snd_card_settings *snd_settings)
{
    hi_s32 ret;
    hi_u32 port;

    card->user_hdmi_mode = snd_settings->user_hdmi_mode;
    card->user_spdif_mode = snd_settings->user_spdif_mode;
    card->adac_enable = snd_settings->adac_enable;

#ifdef HI_SND_ARC_SUPPORT
    card->user_arc_enable = snd_settings->user_arc_enable;
    card->user_arc_mode = snd_settings->user_arc_mode;
    ret = memcpy_s(&card->user_arc_cap, sizeof(hi_ao_arc_audio_cap), &snd_settings->user_arc_cap,
        sizeof(hi_ao_arc_audio_cap));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }
#endif

    ret = memcpy_s(&card->user_open_param, sizeof(hi_ao_attr), &snd_settings->user_open_param, sizeof(hi_ao_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    for (port = 0; port < snd_settings->user_open_param.port_num; port++) {
        ret = snd_op_restore_setting(card, snd_settings->user_open_param.outport[port].port,
            &snd_settings->port_attr[port]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_restore_setting, ret);
            return ret;
        }
    }

#ifdef HI_SND_ADAC_SUPPORT
    if (card->adac_enable == HI_FALSE) {
        snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, HI_AO_PORT_DAC0);
        if (snd_op != HI_NULL) {
            snd_set_adac_enable(snd_op, card->adac_enable);
        }
    }
#endif

    return HI_SUCCESS;
}

hi_s32 snd_op_get_status(snd_op_state *snd_op, aiao_port_stauts *port_status)
{
    hi_s32 ret;
    aiao_port_id port = snd_op->port_id[snd_op->active_id];

    ret = hal_aiao_get_status(port, port_status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_get_status, ret);
        HI_ERR_PRINT_H32(port);
        return ret;
    }

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
    if (port == AIAO_PORT_SPDIF_TX0) {
        aiao_port_stauts port_proc_status;
        port = snd_op->port_id[0];

        ret = hal_aiao_get_status(port, &port_proc_status);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_get_status, ret);
            HI_ERR_PRINT_H32(port);
            return ret;
        }

        ret = memcpy_s(&port_status->proc_status, sizeof(aiao_proc_stauts), &port_proc_status.proc_status,
            sizeof(aiao_proc_stauts));
        if (ret != EOK) {
            HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
            return ret;
        }
    }
#endif

    return HI_SUCCESS;
}

aoe_aop_id snd_get_op_aop_id(snd_op_state *snd_op)
{
    return snd_op->aop[snd_op->active_id];
}

hi_void snd_op_add_mute(snd_op_state *snd_op, hi_u32 mute_ms)
{
    hi_u32 frame_size;

    if (snd_op == HI_NULL || mute_ms == 0) {
        return;
    }

    frame_size = autil_calc_frame_size(snd_op->snd_port_attr.channels, snd_op->snd_port_attr.bit_per_sample);
    frame_size = autil_latency_ms_to_byte_size(mute_ms, frame_size, snd_op->snd_port_attr.sample_rate);

    hal_aiao_write_data(snd_op->port_id[snd_op->active_id], HI_NULL, frame_size);
}

static hi_void snd_op_get_delay(snd_op_state *snd_op, hi_u32 *delay)
{
#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
    hi_s32 ret;
    hi_u32 port_delay = 0;
#endif

    hal_aiao_get_delay_ms(snd_op->port_id[snd_op->active_id], delay);

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
    ret = snd_op_get_delay_compensation(snd_op, &port_delay);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_get_delay_compensation, ret);
        return;
    }

    if (*delay > port_delay) {
        *delay -= port_delay;
    } else {
        *delay = 0;
    }
#endif
}

hi_void snd_op_get_delay_by_output_mode(snd_card_state *card, hi_u32 output_mode, hi_u32 *delay)
{
    snd_op_state *snd_op = HI_NULL;

    *delay = 0;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if (snd_op->snd_port_attr.data_type == output_mode) {
            return snd_op_get_delay(snd_op, delay);
        }
    }
}

hi_void snd_get_delay_ms(snd_card_state *card, hi_u32 *delay)
{
    snd_op_state *snd_op = HI_NULL;

    *delay = 0;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        /* return delay of the first valid snd_op */
        return snd_op_get_delay(snd_op, delay);
    }
}

hi_s32 snd_op_set_volume(snd_op_state *snd_op, hi_ao_gain *gain)
{
    hi_u32 idx;
    aiao_port_id port;
    hi_s32 ret;
    hi_u32 reg_db;

    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST) {
        return HI_SUCCESS;
    }

    if (gain->linear_mode == HI_TRUE) {
        reg_db = autil_volume_linear_to_reg_db((hi_u32)gain->gain);
    } else {
        reg_db = autil_volume_db_to_reg_db(gain->gain);
    }

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        port = snd_op->port_id[idx];
        if (port < AIAO_PORT_MAX) {
            ret = hal_aiao_set_volume(port, reg_db);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aiao_set_volume, ret);
                HI_ERR_PRINT_S32(gain->gain);
                return ret;
            }
        }
    }

    snd_op->user_gain.linear_mode = gain->linear_mode;
    snd_op->user_gain.gain = gain->gain;

    return HI_SUCCESS;
}

hi_s32 snd_op_set_track_mode(snd_op_state *snd_op, hi_track_mode mode)
{
    hi_u32 idx;
    aiao_port_id port;
    hi_s32 ret;

    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST) {
        return HI_SUCCESS;
    }

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        port = snd_op->port_id[idx];
        if (port < AIAO_PORT_MAX) {
            ret = hal_aiao_set_track_mode(port, autil_track_mode_transform(mode));
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aiao_set_track_mode, ret);
                return ret;
            }
        }
    }

    snd_op->user_track_mode = mode;

    return ret;
}

hi_s32 snd_op_set_preci_vol(snd_op_state *snd_op, hi_ao_preci_gain *preci_gain)
{
    hi_u32 idx;
    aoe_aop_id aop;
    hi_s32 ret;
    hi_u32 int_db;
    hi_s32 dec_db;

    int_db = autil_volume_db_to_reg_db(preci_gain->integer);
    dec_db = autil_decimal_volume_db_to_reg_db(preci_gain->decimal);

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_preci_vol(aop, int_db, dec_db);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_preci_vol, ret);
                return HI_FAILURE;
            }
        }
    }

    snd_op->user_preci_gain = *preci_gain;

    return HI_SUCCESS;
}

hi_s32 snd_op_set_mute(snd_op_state *snd_op, hi_u32 mute)
{
    hi_u32 idx;
    aiao_port_id port;
    hi_s32 ret;

    if (snd_op->out_type == SND_OUTPUT_TYPE_CAST) {
        return HI_SUCCESS;
    }

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        port = snd_op->port_id[idx];
        if (port < AIAO_PORT_MAX) {
            ret = hal_aiao_mute(port, !!mute);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aiao_mute, ret);
                return ret;
            }
        }
    }

    snd_op->user_mute = mute;

    return HI_SUCCESS;
}

hi_s32 snd_set_op_mute(snd_card_state *card, hi_ao_port out_port, hi_bool b_mute)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    hi_u32 mute;
    hi_u32 mute_mask;
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        mute_mask = AO_SNDOP_GLOBAL_MUTE_BIT;
    } else {
        mute_mask = AO_SNDOP_LOCAL_MUTE_BIT;
    }

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            mute = snd_op->user_mute;
            mute &= (~(1L << mute_mask));
            mute |= (hi_u32)b_mute << mute_mask;
            ret = snd_op_set_mute(snd_op, mute);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_mute, ret);
                HI_ERR_PRINT_H32(snd_op->out_port);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_mute(snd_card_state *card, hi_ao_port out_port, hi_bool *mute)
{
    hi_u32 mute_bit;
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        out_port = card->user_open_param.outport[0].port;
        mute_bit = AO_SNDOP_GLOBAL_MUTE_BIT;
    } else {
        mute_bit = AO_SNDOP_LOCAL_MUTE_BIT;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *mute = !!((snd_op->user_mute >> mute_bit) & 1);
    return HI_SUCCESS;
}

hi_s32 snd_set_op_volume(snd_card_state *card, hi_ao_port out_port, hi_ao_gain *gain)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_volume(snd_op, gain);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_volume, ret);
                HI_ERR_PRINT_H32(snd_op->out_port);
                return ret;
            }
        }
    }

    /*
     * if out_port is not in card op list,
     * HI_ERR_AO_OUTPORT_NOT_ATTATCH will be returned
     */
    return ret;
}

hi_s32 snd_get_op_volume(snd_card_state *card, hi_ao_port out_port, hi_ao_gain *gain)
{
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, out_port);

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get volume of allport!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    gain->linear_mode = snd_op->user_gain.linear_mode;
    gain->gain = snd_op->user_gain.gain;

    return HI_SUCCESS;
}

hi_s32 snd_set_op_track_mode(snd_card_state *card, hi_ao_port out_port, hi_track_mode mode)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_track_mode(snd_op, mode);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_track_mode, ret);
                HI_ERR_PRINT_H32(snd_op->out_port);
                return ret;
            }
        }
    }

    /*
     * if out_port is not in card op list,
     * HI_ERR_AO_OUTPORT_NOT_ATTATCH will be returned
     */
    return ret;
}

hi_s32 snd_get_op_track_mode(snd_card_state *card, hi_ao_port out_port, hi_track_mode *mode)
{
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get track mode of all port!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *mode = snd_op->user_track_mode;
    return HI_SUCCESS;
}

hi_s32 snd_set_op_preci_vol(snd_card_state *card, hi_ao_port out_port, hi_ao_preci_gain *preci_gain)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_preci_vol(snd_op, preci_gain);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_preci_vol, ret);
                HI_ERR_PRINT_H32(snd_op->out_port);
                return ret;
            }
        }
    }

    /*
     * if out_port is not in card op list,
     * HI_ERR_AO_OUTPORT_NOT_ATTATCH will be returned
     */
    return ret;
}

hi_s32 snd_get_op_preci_vol(snd_card_state *card, hi_ao_port out_port, hi_ao_preci_gain *preci_gain)
{
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get precision volume of all port!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return memcpy_s(preci_gain, sizeof(hi_ao_preci_gain), &snd_op->user_preci_gain, sizeof(hi_ao_preci_gain));
}

hi_s32 snd_op_get_output_mode(snd_card_state *card, hi_ao_port out_port, hi_ao_ouput_mode *mode)
{
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, out_port);
    snd_output_port_driver *driver = snd_op_get_driver(out_port);

    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    if (driver == HI_NULL || driver->get_output_mode == HI_NULL) {
        return HI_ERR_AO_NOTSUPPORT;
    }

    return driver->get_output_mode(card, snd_op, mode);
}

hi_s32 snd_op_set_output_mode(snd_card_state *card, hi_ao_port out_port, hi_ao_ouput_mode mode)
{
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, out_port);
    snd_output_port_driver *driver = snd_op_get_driver(out_port);

    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    if (driver == HI_NULL || driver->set_output_mode == HI_NULL) {
        return HI_ERR_AO_NOTSUPPORT;
    }

    return driver->set_output_mode(card, snd_op, mode);
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 snd_op_get_low_latency(snd_op_state *snd_op, hi_u32 *latency_ms)
{
    hi_s32 ret;
    aoe_aop_id aop = snd_op->aop[snd_op->active_id];

    ret = hal_aoe_aop_get_low_latency(aop, latency_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_get_low_latency, ret);
        HI_ERR_PRINT_H32(aop);
    }

    return ret;
}

hi_s32 snd_op_set_low_latency(snd_op_state *snd_op, hi_u32 latency_ms)
{
    hi_s32 ret;
    hi_u32 idx;
    aoe_aop_id aop;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_low_latency(aop, latency_ms);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_low_latency, ret);
                return ret;
            }
        }
    }

    snd_op->latency_ms = latency_ms;
    return ret;
}

hi_s32 snd_op_set_balance(snd_op_state *snd_op, hi_s32 balance)
{
    hi_u32 idx;
    aoe_aop_id aop;
    hi_s32 ret;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_balance(aop, balance);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_balance, ret);
                return HI_FAILURE;
            }
        }
    }

    snd_op->user_balance = balance;

    return HI_SUCCESS;
}

hi_s32 snd_op_set_ad_output_enable(snd_op_state *snd_op, hi_bool enable)
{
    hi_u32 idx;
    aoe_aop_id aop;
    hi_s32 ret;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_ad_output_enable(aop, enable);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_ad_output_enable, ret);
                return HI_FAILURE;
            }
        }
    }

    snd_op->ad_output_enable = enable;

    return HI_SUCCESS;
}

hi_s32 snd_set_op_ad_output_enable(snd_card_state *card, hi_ao_port out_port, hi_bool enable)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_ad_output_enable(snd_op, enable);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_ad_output_enable, ret);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_ad_output_enable(snd_card_state *card, hi_ao_port out_port, hi_bool *enable)
{
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get ad_output_enable of allport!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *enable = snd_op->ad_output_enable;
    return HI_SUCCESS;
}

hi_s32 snd_set_low_latency(snd_card_state *card, hi_ao_port out_port, hi_u32 latency_ms)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_low_latency(snd_op, latency_ms);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_low_latency, ret);
                return ret;
            }
        }
    }

    return ret;
}

hi_u32 snd_get_low_latency(snd_card_state *card, hi_ao_port out_port, hi_u32 *latency_ms)
{
    snd_op_state *snd_op = HI_NULL;

    /* if out_port is HI_AO_PORT_ALL, return low_latency of the first op in the list */
    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            return snd_op_get_low_latency(snd_op, latency_ms);
        }
    }

    return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
}

hi_s32 snd_set_port_sample_rate(snd_card_state *card, hi_ao_port out_port, hi_u32 sample_rate)
{
    hi_s32 ret;
    aiao_port_attr aiao_attr;
    snd_op_state *snd_op = HI_NULL;
    aiao_port_id port_id;

    /* out_port is HI_AO_PORT_ALL */
    osal_list_for_each_entry(snd_op, &card->op, node) {
        port_id = snd_op->port_id[snd_op->active_id];

        ret = hal_aiao_stop(port_id, AIAO_STOP_IMMEDIATE);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_stop, ret);
            HI_ERR_PRINT_H32(port_id);
            return ret;
        }

        ret = hal_aiao_get_attr(port_id, &aiao_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_get_attr, ret);
            HI_ERR_PRINT_H32(port_id);
            return ret;
        }
        aiao_attr.if_attr.rate = (aiao_sample_rate)sample_rate;

        ret = hal_aiao_set_attr(port_id, &aiao_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_set_attr, ret);
            HI_ERR_PRINT_H32(port_id);
            return ret;
        }

        ret = hal_aiao_start(port_id);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_start, ret);
            HI_ERR_PRINT_H32(port_id);
            return ret;
        }

#if defined(HI_SND_HDMI_SUPPORT)
        if ((snd_op->out_port == HI_AO_PORT_HDMI0) || (snd_op->out_port == HI_AO_PORT_HDMI1)) {
            ret = hdmi_op_set_sample_rate(snd_op, sample_rate);
        }
#endif
    }

    return HI_SUCCESS;
}

hi_s32 snd_get_port_info(snd_card_state *card, hi_ao_port out_port, snd_port_kernel_attr *p_port_k_attr)
{
    hi_s32 ret;
    snd_op_state *snd_op = HI_NULL;
    aiao_rbuf_attr rbf_attr = { 0 };
    aiao_port_id port_id;
    snd_port_kernel_attr port_k_attr[HI_AO_OUTPUT_PORT_MAX];
    hi_u32 port_num;

    ret = memset_s(port_k_attr, sizeof(snd_port_kernel_attr) * HI_AO_OUTPUT_PORT_MAX, 0,
        sizeof(snd_port_kernel_attr) * HI_AO_OUTPUT_PORT_MAX);
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    /* out_port is HI_AO_PORT_ALL */
    for (port_num = 0; port_num < HI_AO_OUTPUT_PORT_MAX; port_num++) {
        port_k_attr[port_num].out_port = HI_AO_PORT_MAX;
    }

    port_num = 0;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        port_k_attr[port_num].out_port = snd_op->out_port;

        port_id = snd_op->port_id[snd_op->active_id];
        ret = hal_aiao_get_rbf_attr(port_id, &rbf_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_get_rbf_attr, ret);
            return ret;
        }

        port_k_attr[port_num].phy_dma = rbf_attr.buf_phy_addr;
        port_k_attr[port_num].phy_wptr = rbf_attr.buf_phy_wptr;
        port_k_attr[port_num].phy_rptr = rbf_attr.buf_phy_rptr;
        port_k_attr[port_num].size = rbf_attr.buf_size;
        port_num++;
    }

    return memcpy_s(p_port_k_attr, sizeof(snd_port_kernel_attr) * HI_AO_OUTPUT_PORT_MAX, port_k_attr,
        sizeof(snd_port_kernel_attr) * HI_AO_OUTPUT_PORT_MAX);
}

hi_s32 snd_set_op_balance(snd_card_state *card, hi_ao_port out_port, hi_s32 balance)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_balance(snd_op, balance);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_balance, ret);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_balance(snd_card_state *card, hi_ao_port out_port, hi_s32 *balance)
{
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get balance of allport!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *balance = snd_op->user_balance;
    return HI_SUCCESS;
}

hi_void snd_get_x_run_count(snd_card_state *card, hi_u32 *count)
{
    aiao_port_stauts status;
    snd_op_state *snd_op = HI_NULL;
    hi_u32 empty_cnt = 0;
    hi_s32 ret;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        ret = snd_op_get_status(snd_op, &status);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_get_status, ret);
        }
        empty_cnt += status.proc_status.inf_fifo_empty_cnt;
    }

    *count = empty_cnt;
}
#endif

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
hi_s32 cast_op_create(snd_card_state *card, hi_ao_cast_attr *ao_cast_attr, hi_audio_buffer *mmz, snd_op_state *snd_op)
{
    hi_s32 ret;
    aoe_aop_id aop;
    aoe_aop_chn_attr aop_attr;
    hi_u32 buf_size, frame_size;
    aiao_cast_attr cast_attr;
    aiao_cast_id cast_id;
    hi_void *wptr_addr = HI_NULL;
    hi_void *rptr_addr = HI_NULL;

    cast_attr.buf_channels = HI_AUDIO_CH_STEREO;
    cast_attr.buf_bit_per_sample = HI_BIT_DEPTH_16;
    cast_attr.buf_sample_rate = HI_SAMPLE_RATE_48K;
    cast_attr.buf_data_format = 0;

    frame_size = autil_calc_frame_size(cast_attr.buf_channels, cast_attr.buf_bit_per_sample);
    buf_size = ao_cast_attr->pcm_frame_max_num * ao_cast_attr->pcm_samples * frame_size;
    cast_attr.buf_latency_thd_ms = autil_byte_size_to_latency_ms(buf_size, frame_size, cast_attr.buf_sample_rate);

    HI_ASSERT((buf_size) < AO_CAST_MMZSIZE_MAX);

    aop_attr.rbf_out_attr.rbf_attr.buf_phy_addr = mmz->phys_addr;
    aop_attr.rbf_out_attr.rbf_attr.buf_vir_addr = mmz->virt_addr - (hi_u8 *)HI_NULL;
    aop_attr.rbf_out_attr.rbf_attr.buf_size = buf_size;

    /* cast use aop wptr&rptr avoid dsp cache problem */
    aop_attr.rbf_out_attr.rbf_attr.buf_wptr_rptr_flag = 0;

    aop_attr.rbf_out_attr.buf_bit_per_sample = cast_attr.buf_bit_per_sample;
    aop_attr.rbf_out_attr.buf_channels = cast_attr.buf_channels;
    aop_attr.rbf_out_attr.buf_sample_rate = cast_attr.buf_sample_rate;
    aop_attr.rbf_out_attr.buf_data_format = 0;
    aop_attr.rbf_hw_priority = HI_FALSE;
    aop_attr.is_cast = HI_TRUE;
    aop_attr.add_mute = ao_cast_attr->add_mute;
    aop_attr.rbf_out_attr.buf_latency_thd_ms = AOE_AOP_BUFF_LATENCYMS_DF;
    ret = hal_aoe_aop_create(&aop, &aop_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_create, ret);
        return HI_NULL;
    }

    snd_op_set_def_aef_attr(snd_op, aop);

#ifndef CASTSIMULATEOP_DEBUG
    (hi_void)hal_aoe_aop_set_aef_bypass(aop, HI_TRUE);
#endif

    cast_attr.ext_dma_mem.buf_phy_addr = mmz->phys_addr;
    cast_attr.ext_dma_mem.buf_vir_addr = mmz->virt_addr - (hi_u8 *)HI_NULL;
    cast_attr.ext_dma_mem.buf_size = buf_size;
    cast_attr.add_mute = ao_cast_attr->add_mute;
    hal_aoe_aop_get_rptr_and_wptr_reg_addr(aop, &wptr_addr, &rptr_addr);
    cast_attr.ext_dma_mem.wptr_addr = (hi_u8 *)wptr_addr - (hi_u8 *)HI_NULL;
    cast_attr.ext_dma_mem.rptr_addr = (hi_u8 *)rptr_addr - (hi_u8 *)HI_NULL;
    ret = hal_cast_create(&cast_id, &cast_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_cast_create, ret);
        hal_aoe_aop_destroy(aop);
        return HI_NULL;
    }

    snd_op->op_mask |= (1 << SND_AOP_TYPE_CAST);
    snd_op->active_id = 0;
    snd_op->aop[0] = aop;

    snd_op->cast_id = cast_id;
    snd_op->engine_type[0] = PCM;
    snd_op->status = SND_OP_STATUS_STOP;
    snd_op->out_type = SND_OUTPUT_TYPE_CAST;
    snd_op->out_port = HI_AO_PORT_MAX;

    /* put cast aop to the end */
    osal_list_add_tail(&snd_op->node, &card->op);

    return HI_SUCCESS;
}

hi_void cast_op_destroy(snd_op_state *snd_op, hi_bool suspend)
{
    hi_s32 ret;

    if (snd_op->op_mask == 0) {
        return;
    }

    if (snd_op->out_type != SND_OUTPUT_TYPE_CAST) {
        return;
    }

    ret = cast_op_stop(snd_op);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(cast_op_stop, ret);
        return;
    }

    hal_aoe_aop_destroy(snd_op->aop[0]);
    hal_cast_destroy(snd_op->cast_id);

    osal_list_del(&snd_op->node);
}

hi_s32 cast_op_start(snd_op_state *snd_op)
{
    hi_s32 ret;
    aoe_aop_id aop = snd_op->aop[snd_op->active_id];

    if (snd_op->out_type != SND_OUTPUT_TYPE_CAST) {
        return HI_ERR_AO_INVALID_PARA;
    }

    if (snd_op->status == SND_OP_STATUS_START) {
        return HI_SUCCESS;
    }

    ret = hal_cast_start(snd_op->cast_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_cast_start, ret);
        return ret;
    }

    ret = hal_aoe_aop_start(aop);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_start, ret);
        return ret;
    }

    snd_op->status = SND_OP_STATUS_START;

    return HI_SUCCESS;
}

hi_s32 cast_op_stop(snd_op_state *snd_op)
{
    hi_s32 ret;
    aoe_aop_id aop = snd_op->aop[snd_op->active_id];

    if (snd_op->out_type != SND_OUTPUT_TYPE_CAST) {
        return HI_ERR_AO_INVALID_PARA;
    }

    if (snd_op->status == SND_OP_STATUS_STOP) {
        return HI_SUCCESS;
    }

    ret = hal_cast_stop(snd_op->cast_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_cast_stop, ret);
        return ret;
    }

    ret = hal_aoe_aop_stop(aop);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_stop, ret);
        return ret;
    }

    snd_op->status = SND_OP_STATUS_STOP;
    return HI_SUCCESS;
}
#endif

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
static hi_s32 snd_op_get_delay_compensation(snd_op_state *snd_op, hi_u32 *real_delay_ms)
{
    hi_s32 ret;
    hi_u32 idx;
    aoe_aop_id aop;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];

        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_get_delay(aop, real_delay_ms);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_get_delay, ret);
                HI_ERR_PRINT_U32(aop);
                return ret;
            }
        }
    }
    return HI_SUCCESS;
}

static hi_s32 snd_op_set_delay_compensation(snd_op_state *snd_op, hi_u32 delay)
{
    hi_s32 ret;
    hi_u32 idx;
    aoe_aop_id aop;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];

        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_delay(aop, delay);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_delay, ret);
                HI_ERR_PRINT_U32(aop);
                HI_ERR_PRINT_U32(delay);
                return ret;
            }
        }
    }

    snd_op->delay_ms = delay;

    return HI_SUCCESS;
}

hi_s32 snd_set_delay_compensation(snd_card_state *card, hi_ao_port out_port, hi_u32 delay)
{
    hi_s32 ret;
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = snd_op_set_low_latency(snd_op, delay + AOE_AOP_BUFF_LATENCYMS_DF);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_low_latency, ret);
        return ret;
    }

    ret = snd_op_set_delay_compensation(snd_op, delay);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_set_delay_compensation, ret);
        return ret;
    }

    HI_LOG_INFO("set audio delay success\n");
    HI_INFO_PRINT_U32(delay);

    return ret;
}

hi_s32 snd_get_delay_compensation(snd_card_state *card, hi_ao_port out_port, hi_u32 *delay_ms)
{
    hi_s32 ret;
    snd_op_state *snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = snd_op_get_delay_compensation(snd_op, delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_get_delay_compensation, ret);
    }

    return ret;
}
#endif

#ifdef HI_PROC_SUPPORT
hi_s32 snd_read_op_proc(hi_void *p, snd_card_state *card, hi_ao_port port)
{
    hi_s32 ret;
    aiao_port_stauts status;
    snd_op_state *snd_op = HI_NULL;

    snd_op = snd_get_op_handle_by_out_port(card, port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = memset_s(&status, sizeof(aiao_port_stauts), 0, sizeof(aiao_port_stauts));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    ret = snd_op_get_status(snd_op, &status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_get_status, ret);
        return ret;
    }

    osal_proc_print(p,
        "%s: status(%s), mute(%s), vol(%d%s), track_mode(%s), preci_vol(%s%d.%ddB), "
        "balance(%d), aef_bypass(%s), delay_comps(%dms)\n",
        autil_port_to_name(port),
        (hi_char *)((AIAO_PORT_STATUS_START == status.status) ?
        "start" : ((AIAO_PORT_STATUS_STOP == status.status) ? "stop" : "stopping")),
        (snd_op->user_mute == 0) ?
        "off" : "on", snd_op->user_gain.gain, (snd_op->user_gain.linear_mode == HI_TRUE) ? "" : "dB",
        autil_track_mode_to_name(snd_op->user_track_mode),
        ((snd_op->user_preci_gain.integer == 0) && (snd_op->user_preci_gain.decimal < 0)) ?
        "-" : "", snd_op->user_preci_gain.integer,
        (snd_op->user_preci_gain.decimal < 0) ? (-snd_op->user_preci_gain.decimal) : snd_op->user_preci_gain.decimal,
        snd_op->user_balance, TRUE_ON_FALSE_OFF(snd_op->bypass), snd_op->delay_ms);

#ifdef HI_SND_DRC_SUPPORT
    if (snd_op->drc_enable == HI_TRUE) {
        osal_proc_print(p, "      DRC(attack_time %dms, release_time %dms, threshold %ddB, limit %ddB)\n\n",
            snd_op->drc_attr.attack_time, snd_op->drc_attr.release_time, snd_op->drc_attr.threshold,
            snd_op->drc_attr.limit);
    }
#endif

#ifdef HI_SND_PEQ_SUPPORT
    if (snd_op->peq_enable == HI_TRUE) {
        hi_u32 i = 0;
        osal_proc_print(p, "      PEQ attr:");
        for (i = 0; i < snd_op->peq_attr.band_num; i++) {
            osal_proc_print(p, "band%d(%s,%dHz,%d.%.3ddB,%d.%.1dQ) ", i,
                autil_peq_type_to_name(snd_op->peq_attr.param[i].type), snd_op->peq_attr.param[i].freq,
                snd_op->peq_attr.param[i].gain / 1000, snd_op->peq_attr.param[i].gain % 1000,
                snd_op->peq_attr.param[i].q / 10, snd_op->peq_attr.param[i].q % 10);

            if ((((i + 1) % 3) == 0) && (snd_op->peq_attr.band_num != (i + 1))) { /* a line 3 band */
                osal_proc_print(p, "\n      ");
            }
        }
        osal_proc_print(p, "\n");
    }
#endif

#ifdef HI_SND_SPDIF_SUPPORT
    if (port == HI_AO_PORT_SPDIF0) {
        spdif_op_read_proc(p, snd_op);
    }
#endif

    osal_proc_print(p,
        "      sample_rate(%.6d), channel(%.2d), bit_width(%2d), *engine(%s),*AOP(0x%x),"
        " *port_id(0x%x), ad_output(%s)\n",
        status.user_config.if_attr.rate, status.user_config.if_attr.ch_num, status.user_config.if_attr.bit_depth,
        autil_engine_to_name(snd_op->engine_type[snd_op->active_id]), (hi_u32)snd_op->aop[snd_op->active_id],
        (hi_u32)snd_op->port_id[snd_op->active_id], TRUE_ON_FALSE_OFF(snd_op->ad_output_enable));

    osal_proc_print(p, "      dma_cnt(%.6u), buf_empty_cnt(%.6u), fifo_empty_cnt(%.6u)\n\n", status.proc_status.dma_cnt,
        status.proc_status.buf_empty_cnt, status.proc_status.inf_fifo_empty_cnt);

    return HI_SUCCESS;
}

static hi_s32 snd_write_op_proc_mute(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    const hi_char *pc_on_cmd = "on";
    const hi_char *pc_off_cmd = "off";
    const hi_char *pc_one_cmd = "1";
    const hi_char *pc_zero_cmd = "0";
    hi_bool set_port_mute = HI_FALSE;
    hi_bool get_port_mute = HI_FALSE;
    hi_s32 ret;

    if ((pc_buf == strstr(pc_buf, pc_on_cmd)) || (pc_buf == strstr(pc_buf, pc_one_cmd))) {
        set_port_mute = HI_TRUE;
    } else if ((pc_buf == strstr(pc_buf, pc_off_cmd)) || (pc_buf == strstr(pc_buf, pc_zero_cmd))) {
        set_port_mute = HI_FALSE;
    } else {
        return HI_FAILURE;
    }

    ret = snd_get_op_mute(card, out_port, &get_port_mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_op_mute, ret);
        return ret;
    }

    ret = snd_set_op_mute(card, out_port, set_port_mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_set_op_mute, ret);
    } else {
        if (set_port_mute) {
            osal_printk("set %s mute ON success:%d(%s) -> 1(ON)\n", autil_port_to_name(out_port), (hi_u32)get_port_mute,
                (get_port_mute ? "ON" : "OFF"));
        } else {
            osal_printk("set %s mute OFF success:%d(%s) -> 0(OFF)\n",
                autil_port_to_name(out_port), (hi_u32)get_port_mute,
                (get_port_mute ? "ON" : "OFF"));
        }
    }

    return ret;
}

#ifdef HI_SND_ADVANCED_SUPPORT
static hi_s32 snd_write_op_proc_balance(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    hi_s32 set_balance;
    hi_s32 get_balance = 0;
    hi_s32 ret;

    set_balance = simple_strtol(pc_buf, &pc_buf, 10);

    CHECK_AO_BALANCE(set_balance);

    ret = snd_get_op_balance(card, out_port, &get_balance);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_op_balance, ret);
        return ret;
    }

    ret = snd_set_op_balance(card, out_port, set_balance);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_set_op_balance, ret);
    } else {
        ret = snd_get_op_balance(card, out_port, &set_balance);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_get_op_balance, ret);
            return ret;
        }
        osal_printk("set %s balance success:%d -> %d\n", autil_port_to_name(out_port), get_balance, set_balance);
    }
    return ret;
}
#endif

static hi_s32 snd_write_op_proc_volume(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    const hi_char *pcd_unit = "dB";
    hi_s32 vol = 0;
    hi_s32 ret;

    if (strstr(pc_buf, pcd_unit)) {
        hi_ao_gain set_gain;
        hi_ao_gain get_gain = { HI_TRUE, 0 };
        vol = simple_strtol(pc_buf, &pc_buf, 10);
        set_gain.linear_mode = HI_FALSE;
        set_gain.gain = vol;

        CHECK_AO_ABSLUTEVOLUME(set_gain.gain);
        if (out_port != HI_AO_PORT_ALL) {
            ret = snd_get_op_volume(card, out_port, &get_gain);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_get_op_volume, ret);
                return ret;
            }
        }
        ret = snd_set_op_volume(card, out_port, &set_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_set_op_volume, ret);
        } else {
            osal_printk("set %s volume success:%d -> %d\n", autil_port_to_name(out_port), get_gain.gain, set_gain.gain);
        }

        return ret;
    } else {
        osal_printk("invalid value! please add unit, example: 0dB\n");
        return HI_FAILURE;
    }
}

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
static hi_s32 snd_write_op_proc_delay(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    hi_u32 set_delay;
    hi_u32 get_delay = 0;
    hi_s32 ret;

    set_delay = simple_strtoul(pc_buf, &pc_buf, 10);

    ret = snd_get_delay_compensation(card, out_port, &get_delay);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_delay_compensation, ret);
        return ret;
    }

    ret = snd_set_delay_compensation(card, out_port, set_delay);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_set_delay_compensation, ret);
    } else {
        ret = snd_get_delay_compensation(card, out_port, &set_delay);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_get_delay_compensation, ret);
            return ret;
        }
        osal_printk("set %s delay compensation success:%d -> %d\n", autil_port_to_name(out_port), get_delay, set_delay);
    }
    return ret;
}
#endif

static hi_s32 snd_write_op_proc_preci_volume(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    /* hi_char *pcd_unit = "dB"; */
    hi_bool nagetive = HI_FALSE;
    hi_u32 val1;
    hi_u32 val2 = 0;
    hi_s32 ret;
#if 0
    if (strstr(pc_buf, pcd_unit)) {
#endif
    hi_ao_preci_gain set_preci_gain;
    hi_ao_preci_gain get_preci_gain;

    if (pc_buf[0] == '-') {
        nagetive = HI_TRUE;
        pc_buf++;
    }
    val1 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
    if (pc_buf[0] == '.') {
        pc_buf++;
        val2 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
        if (val2 % 125) {
            osal_printk("invalid value! step by 0.125dB. \n");
            return HI_FAILURE;
        }
    } else {
        val2 = 0;
    }
#if 0
        if (nagetive == HI_TRUE) {
            if (((set_preci_gain.integer)*1000 + (set_preci_gain.decimal)) > 81000) {
                osal_printk("invalid value! track prescale value range:-81dB~18dB\n");
                return HI_FAILURE;
            }
        } else {
            if (((set_preci_gain.integer)*1000 + (set_preci_gain.decimal)) > 18000) {
                osal_printk("invalid value! track prescale value range:-81dB~18dB\n");
                return HI_FAILURE;
            }
        }
#endif
    set_preci_gain.integer = (nagetive == HI_TRUE) ? (-val1) : val1;
    set_preci_gain.decimal = (nagetive == HI_TRUE) ? (-val2) : val2;

    CHECK_AO_ABSLUTEPRECIVOLUME(set_preci_gain.integer, set_preci_gain.decimal);

    ret = snd_get_op_preci_vol(card, out_port, &get_preci_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_op_preci_vol, ret);
        return ret;
    } else {
        if (get_preci_gain.decimal < 0) {
            get_preci_gain.decimal = -get_preci_gain.decimal;
        }
    }

    ret = snd_set_op_preci_vol(card, out_port, &set_preci_gain);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_set_op_preci_vol, ret);
    } else {
        osal_printk("set %s prescale success:%d.%d -> %d.%d\n", autil_port_to_name(out_port), get_preci_gain.integer,
            get_preci_gain.decimal, set_preci_gain.integer, val2);
    }

    return ret;
#if 0
    } else {
        osal_printk("invalid value! please add unit, example: 0dB\n");
        return HI_FAILURE;
    }
#endif
}

hi_s32 snd_write_op_proc(snd_card_state *card, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_u32 cmd_id;
    hi_ao_port out_port;

    snd_op_write_proc op_write_poc[] = {
#ifdef HI_SND_AEF_SUPPORT
        { "aefbypass", snd_write_op_proc_aef_bypass },
#endif

#ifdef HI_SND_DRC_SUPPORT
        { "drc", snd_write_op_proc_drc },
#endif

#ifdef HI_SND_PEQ_SUPPORT
        { "peq", snd_write_op_proc_peq },
#endif

        { "mute", snd_write_op_proc_mute },

#ifdef HI_SND_ADVANCED_SUPPORT
        { "balance", snd_write_op_proc_balance },
#endif

        { "volume", snd_write_op_proc_volume },
        { "precivolume", snd_write_op_proc_preci_volume },

#if defined (HI_SOUND_PORT_DELAY_SUPPORT)
        { "delaycomps", snd_write_op_proc_delay }
#endif
    };

    out_port = autil_port_name_to_port(argv[1]);

    for (cmd_id = 0; cmd_id < sizeof(op_write_poc) / sizeof(op_write_poc[0]); cmd_id++) {
        if (op_write_poc[cmd_id].func == HI_NULL) {
            continue;
        }

        if (argv[2] == strstr(argv[2], op_write_poc[cmd_id].cmd)) {
            return op_write_poc[cmd_id].func(card, out_port, argv[3]);
        }
    }

    return HI_FAILURE;
}
#endif

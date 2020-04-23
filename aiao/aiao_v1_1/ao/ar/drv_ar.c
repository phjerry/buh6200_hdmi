/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao ar implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_aiao_log.h"

#include "drv_ar.h"
#include "drv_ao_ioctl.h"
#include "drv_ao_private.h"
#include "hal_aoe.h"

hi_s32 ao_snd_set_con_output_enable(ao_snd_id sound, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(card);

    osal_list_for_each_entry(engine, &card->engine, node) {
        hi_s32 ret = hal_aoe_engine_set_con_output_enable(engine->engine, enable);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_con_output_enable, ret);
            return ret;
        }
    }

    card->con_output_enable = enable;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_con_output_enable(ao_snd_id sound, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    *enable = card->con_output_enable;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_create_isb(ao_snd_isb_param *isb)
{
    hi_s32 ret;
    hi_u32 i = isb->sound;
    hi_u32 malloc_size;
    hi_char *mmz_name[] = { "ao_main_isb", "ao_assoc_isb", "ao_ext_isb", "ao_ott_isb", "ao_sys_isb", "ao_tts_isb" };
    ao_mgmt *mgmt = ao_get_mgmt();

    if (isb->type >= AO_RENDER_TRACK_NUM) {
        HI_LOG_ERR("Invalid isb type\n");
        HI_ERR_PRINT_H32(isb->type);
        return HI_ERR_AO_INVALID_PARA;
    }

    if (mgmt->snd_entity[i].isb_mmz[isb->type].phys_addr != 0) {
        HI_LOG_WARN("This type of ISB buffer had been created\n");
        HI_WARN_PRINT_H32(isb->type);
        return HI_SUCCESS;
    }

    /* malloc mmz buffer */
    ret = hi_drv_audio_mmz_alloc(mmz_name[isb->type],
        isb->buf_size, HI_FALSE, &mgmt->snd_entity[i].isb_mmz[isb->type]);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_audio_mmz_alloc, ret);
        HI_ERR_PRINT_H32(isb->type);
        return HI_ERR_AO_MALLOC_FAILED;
    }

    malloc_size = mgmt->snd_entity[i].isb_mmz[isb->type].size;
    isb->buf_phy_addr = mgmt->snd_entity[i].isb_mmz[isb->type].phys_addr;

    HI_WARN_PRINT_H32(isb->type);
    HI_WARN_PRINT_H64(mgmt->snd_entity[i].isb_mmz[isb->type].phys_addr);
    HI_WARN_PRINT_H32(mgmt->snd_entity[i].isb_mmz[isb->type].size);

    ret = memset_s(mgmt->snd_entity[i].isb_mmz[isb->type].virt_addr, malloc_size, 0, malloc_size);
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        hi_drv_audio_mmz_release(&mgmt->snd_entity[i].isb_mmz[isb->type]);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_snd_destroy_isb(ao_snd_isb_param *isb)
{
    hi_u32 i = isb->sound;
    ao_mgmt *mgmt = ao_get_mgmt();

    if (isb->type >= AO_RENDER_TRACK_NUM) {
        HI_LOG_ERR("Invalid isb type\n");
        HI_ERR_PRINT_H32(isb->type);
        return HI_ERR_AO_INVALID_PARA;
    }

    HI_WARN_PRINT_H32(isb->type);
    HI_WARN_PRINT_H64(mgmt->snd_entity[i].isb_mmz[isb->type].phys_addr);
    HI_WARN_PRINT_H32(mgmt->snd_entity[i].isb_mmz[isb->type].size);

    if (mgmt->snd_entity[i].isb_mmz[isb->type].phys_addr) {
        hi_drv_audio_mmz_release(&mgmt->snd_entity[i].isb_mmz[isb->type]);
        mgmt->snd_entity[i].isb_mmz[isb->type].phys_addr = 0;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_snd_set_render_param(ao_snd_id sound, ao_render_attr *render_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    ret = memcpy_s(&card->render_attr, sizeof(card->render_attr),
        render_attr, sizeof(*render_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_render_param(ao_snd_id sound, ao_render_attr *render_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    ret = memcpy_s(render_attr, sizeof(*render_attr),
        &card->render_attr, sizeof(card->render_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_set_output_atmos_enable(ao_snd_id sound, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_engine_state *engine = HI_NULL;

    CHECK_AO_NULL_PTR(card);

    osal_list_for_each_entry(engine, &card->engine, node) {
        hi_s32 ret;
        ret = hal_aoe_engine_set_output_atmos_enable(engine->engine, enable);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_output_atmos_enable, ret);
            return ret;
        }
    }

    card->output_atmos_enable = enable;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_output_atmos_enable(ao_snd_id sound, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    *enable = card->output_atmos_enable;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_set_con_output_status(hi_void *file, ao_snd_id sound, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    card->render_attr.continue_status = enable;
    card->render_attr.con_output_fp = file;

    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_con_output_status(ao_snd_id sound, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    *enable = card->render_attr.continue_status;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_set_output_latency_mode(hi_void *file, ao_snd_id sound, hi_ao_output_latency output_mode)
{
    snd_card_state *card = snd_card_get_card(sound);

    card->render_attr.output_mode = output_mode;
    card->render_attr.con_output_fp = file;
    return HI_SUCCESS;
}

static hi_s32 ao_snd_get_output_latency_mode(ao_snd_id sound, hi_ao_output_latency *output_mode)
{
    snd_card_state *card = snd_card_get_card(sound);

    *output_mode = card->render_attr.output_mode;
    return HI_SUCCESS;
}

hi_void ao_snd_update_render_raw_info(ao_snd_id sound, hi_ao_ouput_mode mode)
{
    snd_card_state *card = snd_card_get_card(sound);

    card->render_attr.passthru_bypass = (mode == HI_AO_OUTPUT_MODE_RAW);
}

hi_s32 ao_ioctl_snd_create_sb(hi_void *file, hi_void *arg)
{
    ao_snd_isb_param_p param = (ao_snd_isb_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_create_isb(param);
}

hi_s32 ao_ioctl_snd_destroy_sb(hi_void *file, hi_void *arg)
{
    ao_snd_isb_param_p param = (ao_snd_isb_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_destroy_isb(param);
}

hi_s32 ao_ioctl_snd_set_render_param(hi_void *file, hi_void *arg)
{
    ao_snd_render_param_p param = (ao_snd_render_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_set_render_param(param->sound, &param->render_attr);
}

hi_s32 ao_ioctl_snd_get_render_param(hi_void *file, hi_void *arg)
{
    ao_snd_render_param_p param = (ao_snd_render_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_get_render_param(param->sound, &param->render_attr);
}

hi_s32 ao_ioctl_snd_set_output_atmos_enable(hi_void *file, hi_void *arg)
{
    ao_snd_output_atmos_enable_p param = (ao_snd_output_atmos_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_set_output_atmos_enable(param->sound, param->output_atmos_enable);
}

hi_s32 ao_ioctl_snd_get_output_atmos_enable(hi_void *file, hi_void *arg)
{
    ao_snd_output_atmos_enable_p param = (ao_snd_output_atmos_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_get_output_atmos_enable(param->sound, &param->output_atmos_enable);
}

hi_s32 ao_ioctl_set_con_output_status(hi_void *file, hi_void *arg)
{
    ao_snd_con_output_enable_p con_output = (ao_snd_con_output_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(con_output->sound);
    return ao_snd_set_con_output_status(file, con_output->sound, con_output->con_output_enable);
}

hi_s32 ao_ioctl_get_con_output_status(hi_void *file, hi_void *arg)
{
    ao_snd_con_output_enable_p con_output = (ao_snd_con_output_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(con_output->sound);
    return ao_snd_get_con_output_status(con_output->sound, &con_output->con_output_enable);
}

hi_s32 ao_ioctl_set_output_latency_mode(hi_void *file, hi_void *arg)
{
    ao_snd_output_latency_mode_p output = (ao_snd_output_latency_mode_p)arg;
    CHECK_AO_SNDCARD_OPEN(output->sound);
    return ao_snd_set_output_latency_mode(file, output->sound, output->output_mode);
}

hi_s32 ao_ioctl_get_output_latency_mode(hi_void *file, hi_void *arg)
{
    ao_snd_output_latency_mode_p output = (ao_snd_output_latency_mode_p)arg;
    CHECK_AO_SNDCARD_OPEN(output->sound);
    return ao_snd_get_output_latency_mode(output->sound, &output->output_mode);
}

hi_s32 ao_ioctl_snd_set_continue_output(hi_void *file, hi_void *arg)
{
    ao_snd_con_output_enable_p param = (ao_snd_con_output_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_set_con_output_enable(param->sound, param->con_output_enable);
}

hi_s32 ao_ioctl_snd_get_continue_output(hi_void *file, hi_void *arg)
{
    ao_snd_con_output_enable_p param = (ao_snd_con_output_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_get_con_output_enable(param->sound, &param->con_output_enable);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao external reference function
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"
#include "hi_ao_type.h"

#include "drv_pdm_ext.h"

static hi_s32 get_param_from_pdm(ao_snd_id sound, hi_drv_pdm_sound_param *pdm_param)
{
    hi_s32 ret;
    hi_pdm_export_func *pdm_func = HI_NULL;

    /* get pdm function */
    ret = osal_exportfunc_get(HI_ID_PDM, (hi_void **)&pdm_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_get, ret);
        return HI_FAILURE;
    }

    if ((pdm_func == HI_NULL) || (pdm_func->pdm_get_sound_param == HI_NULL)) {
        HI_LOG_ERR("PDM function is invalid\n");
        return HI_FAILURE;
    }

    ret = (pdm_func->pdm_get_sound_param)(sound, pdm_param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(pdm_get_sound_param, ret);
        return HI_FAILURE;
    }

    if ((pdm_param->port_num > HI_AO_OUTPUT_PORT_MAX) || (pdm_param->port_num == 0)) {
        HI_LOG_ERR("PDM return invalid port num\n");
        HI_ERR_PRINT_H32(pdm_param->port_num);
        return HI_ERR_AO_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 ao_get_open_attr_from_pdm(ao_snd_id sound, hi_ao_attr *attr)
{
    hi_s32 ret;
    hi_u32 i;
    hi_drv_pdm_sound_param pdm_param;

    ret = get_param_from_pdm(sound, &pdm_param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(get_param_from_pdm, ret);
        return ret;
    }

    attr->port_num = pdm_param.port_num;
    attr->sample_rate = HI_SAMPLE_RATE_48K;

    for (i = 0; i < pdm_param.port_num; i++) {
        ret = memcpy_s(&attr->outport[i], sizeof(hi_ao_port_attr),
            &pdm_param.out_port[i], sizeof(hi_ao_port_attr));
        if (ret != EOK) {
            HI_ERR_PRINT_FUNC_RES(get_param_from_pdm, ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*
 * use bootargs to control audio output ports:
 * 1. example: "audio=noadac,nospdif", then adac and spdif will be closed
 * 2. if audio is not set in bootargs, then use default setting
 * 3. audio_options_string is defined in kernel
 */
extern hi_char *audio_options_string;

hi_s32 ao_get_open_attr_from_bootargs(ao_snd_id sound, hi_ao_attr *attr)
{
    if (audio_options_string == HI_NULL) {
        return HI_FAILURE;
    }

    if (sound != AO_SND_0) {
        return HI_ERR_AO_NOTSUPPORT;
    }

    attr->port_num = 0;

    if (strstr(audio_options_string, "noadac") == HI_NULL) {
        attr->outport[attr->port_num].port = HI_AO_PORT_DAC0;
        attr->port_num++;
    }

    if (strstr(audio_options_string, "nospdif") == HI_NULL) {
        attr->outport[attr->port_num].port = HI_AO_PORT_SPDIF0;
        attr->port_num++;
    }

    if (strstr(audio_options_string, "nohdmi") == HI_NULL) {
        attr->outport[attr->port_num].port = HI_AO_PORT_HDMI0;
        attr->port_num++;
    }

    if (attr->port_num == 0) {
        HI_LOG_ERR("invalid audio close port bootargs setting!\n");
        return HI_FAILURE;
    }

    attr->sample_rate = HI_SAMPLE_RATE_48K;

    return HI_SUCCESS;
}


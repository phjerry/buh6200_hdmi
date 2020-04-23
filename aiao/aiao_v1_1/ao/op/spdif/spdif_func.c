/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port spdif function.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"
#include "audio_util.h"

#include "spdif_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    hi_ao_ouput_mode user_spdif_mode;
    hi_u32 spdif_passthrough; /* 0(no spdif), 1(pcm), 2(lbr) */
    hi_u32 spdif_data_format;

    hi_ao_spdif_scms_mode user_spdif_scms_mode;
    hi_ao_spdif_category_code user_spdif_category_code;

    snd_op_state op;
} spdif_device;

static inline spdif_device *get_spdif_device(snd_op_state *snd_op)
{
    return osal_container_of(snd_op, spdif_device, op);
}

static hi_void spdif_op_init(spdif_device *device)
{
    device->spdif_data_format = 0;
    device->spdif_passthrough = PCM;
    device->user_spdif_mode = HI_AO_OUTPUT_MODE_LPCM;

    device->user_spdif_scms_mode = HI_AO_SPDIF_SCMS_MODE_COPY_PROHIBITED;
    device->user_spdif_category_code = HI_AO_SPDIF_CATEGORY_GENERAL;

    device->op.out_type = SND_OUTPUT_TYPE_SPDIF;
    snd_op_init(&device->op);
}

static hi_void spdif_op_destroy(snd_op_state *snd_op, hi_bool suspend)
{
    spdif_device *device = get_spdif_device(snd_op);

    snd_op_destroy(snd_op, suspend);
    osal_kfree(HI_ID_AO, device);
}

static hi_s32 spdif_op_create(snd_card_state *card, hi_ao_port_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    spdif_device *device = HI_NULL;

    device = (spdif_device *)osal_kmalloc(HI_ID_AO, sizeof(spdif_device), OSAL_GFP_KERNEL);
    if (device == HI_NULL) {
        HI_LOG_ERR("MALLOC spdif_device failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(device, sizeof(*device), 0, sizeof(*device));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out0;
    }

    spdif_op_init(device);

    ret = snd_op_create_aop(&device->op, attr, SND_AOP_TYPE_SPDIF, card->user_sample_rate);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_create_aop, ret);
        goto out0;
    }

    osal_list_add_tail(&device->op.node, &card->op);

    ret = spdif_op_set_scms_mode(&device->op, device->user_spdif_scms_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(spdif_op_set_scms_mode, ret);
        goto out1;
    }

    ret = spdif_op_set_category_code(&device->op, device->user_spdif_category_code);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(spdif_op_set_category_code, ret);
        goto out1;
    }

    card->spdif_passthrough = PCM;
    card->user_spdif_mode = HI_AO_OUTPUT_MODE_LPCM;
    card->spdif_data_format = 0;
    card->lbr_device = HI_TRUE;

    return ret;

out1:
    spdif_op_destroy(&device->op, HI_FALSE);

out0:
    osal_kfree(HI_ID_AO, device);
    return ret;
}

hi_s32 spdif_op_set_category_code(snd_op_state *snd_op, hi_ao_spdif_category_code category_code)
{
    hi_s32 ret;
    aiao_port_id port;
    spdif_device *device = get_spdif_device(snd_op);

    port = snd_op->port_id[snd_op->active_id];
    ret = hal_aiao_set_spdif_category_code(port, (hi_u32)category_code);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_set_spdif_category_code, ret);
        return ret;
    }

    device->user_spdif_category_code = category_code;
    return ret;
}

hi_s32 spdif_op_get_category_code(snd_op_state *snd_op, hi_ao_spdif_category_code *category_code)
{
    spdif_device *device = get_spdif_device(snd_op);
    *category_code = device->user_spdif_category_code;
    return HI_SUCCESS;
}

hi_s32 spdif_op_set_scms_mode(snd_op_state *snd_op, hi_ao_spdif_scms_mode scms_mode)
{
    hi_s32 ret;
    aiao_port_id port;
    spdif_device *device = get_spdif_device(snd_op);

    port = snd_op->port_id[snd_op->active_id];
    ret = hal_aiao_set_spdif_scms_mode(port, (hi_u32)scms_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_set_spdif_scms_mode, ret);
        return ret;
    }

    device->user_spdif_scms_mode = scms_mode;
    return ret;
}

hi_s32 spdif_op_get_scms_mode(snd_op_state *snd_op, hi_ao_spdif_scms_mode *scms_mode)
{
    spdif_device *device = get_spdif_device(snd_op);
    *scms_mode = device->user_spdif_scms_mode;
    return HI_SUCCESS;
}

hi_s32 spdif_op_set_output_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode mode)
{
    spdif_device *device = get_spdif_device(snd_op);
    CHECK_AO_SPDIF_MODE(mode);

    card->user_spdif_mode = mode;
    device->user_spdif_mode = mode;
    return HI_SUCCESS;
}

hi_s32 spdif_op_get_output_mode(snd_card_state *card, snd_op_state *snd_op, hi_ao_ouput_mode *mode)
{
    spdif_device *device = get_spdif_device(snd_op);
    *mode = device->user_spdif_mode;
    return HI_SUCCESS;
}

hi_bool spdif_op_match(hi_ao_port ao_port)
{
    hi_u32 i;
    const hi_ao_port spdif_op_device[] = {
        HI_AO_PORT_SPDIF0,
    };

    for (i = 0; i < ARRAY_SIZE(spdif_op_device); i++) {
        if (spdif_op_device[i] == ao_port) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static const struct {
    hi_char *buf_name;
    hi_u32 buf_size;
} g_spdif_hw_param[] = {
    { "ao_spidf", AO_SPDIF_MMZSIZE_MAX },
};

static hi_s32 spdif_op_get_hw_param(hi_ao_port_attr *attr, snd_aop_type aop_type, snd_op_create_param *hw_param)
{
    /* 1.spdif port id */
    hw_param->aiao_port = AIAO_PORT_SPDIF_TX1;

    /* 2.spdif op buffer */
    hw_param->buf_name = g_spdif_hw_param[0].buf_name;
    hw_param->buf_size = g_spdif_hw_param[0].buf_size;

    /* 3.spdif hw cfg */
    hal_aiao_get_tx_spd_df_attr(hw_param->aiao_port, hw_param->port_cfg);

    return HI_SUCCESS;
}

static snd_output_port_driver g_spdif_op_driver = {
    .match = spdif_op_match,
    .create = spdif_op_create,
    .destroy = spdif_op_destroy,
    .set_output_mode = spdif_op_set_output_mode,
    .get_output_mode = spdif_op_get_output_mode,
    .get_hw_param = spdif_op_get_hw_param,
};

hi_void spdif_op_register_driver(struct osal_list_head *head)
{
    osal_list_add_tail(&g_spdif_op_driver.node, head);
}

#ifdef HI_PROC_SUPPORT
hi_void spdif_op_read_proc(hi_void *file, snd_op_state *snd_op)
{
    spdif_device *device = get_spdif_device(snd_op);

    osal_proc_print(file, "      category_code(%s), scms_mode(%s)\n",
        autil_category_code_to_name(device->user_spdif_category_code),
        autil_scms_mode_to_name(device->user_spdif_scms_mode));
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

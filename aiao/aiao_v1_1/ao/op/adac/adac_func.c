/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao adac op reference.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "adac_func.h"
#include "audio_util.h"
#include "hal_tianlai_adac.h"

#ifdef HI_SND_MUTECTL_SUPPORT
#include "drv_gpio_ext.h"
#endif

#define UDELAY_US_10  10
#define UDELAY_US_100 100

#define TIANLAI_SV100_DEFAULT_PRESCALE_INT 0
#define TIANLAI_SV100_DEFAULT_PRESCALE_DEC (-875)

/* aiao adac op */
typedef struct {
    hi_bool adac_enable;
    snd_op_state op;

#ifdef HI_SND_MUTECTL_SUPPORT
    gpio_ext_func *gpio_func;
    osal_timer mute_disable_timer;
#endif
} adac_device;

static hi_void adac_op_init(adac_device *device)
{
    device->adac_enable = HI_TRUE;
    device->op.out_type = SND_OUTPUT_TYPE_DAC;
    snd_op_init(&device->op);
}

static inline adac_device *get_adac_device(snd_op_state *snd_op)
{
    return osal_container_of(snd_op, adac_device, op);
}

static hi_void snd_op_enable_mute_ctrl(adac_device *device)
{
    if (autil_get_amp_type() != AUDIO_AMP_TYPE_EXTERNAL) {
        return;
    }

#if defined(CHIP_TYPE_HI3716MV430) && (0x2c == HI_SND_MUTECTL_GPIO)
    /* check MUTECTL is GPIO5_4 and set multiplex to GPIO5_4 */
    g_reg_io->ioshare_45.bits.ioshare_45 = 0x1;
    osal_udelay(UDELAY_US_10);
#endif

    if ((device->gpio_func != HI_NULL) && (device->gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) {
        (device->gpio_func->pfn_gpio_direction_set_bit)(HI_SND_MUTECTL_GPIO, 0);
    }
    if ((device->gpio_func != HI_NULL) && (device->gpio_func->pfn_gpio_write_bit != HI_NULL)) {
        (device->gpio_func->pfn_gpio_write_bit)(HI_SND_MUTECTL_GPIO, ((0 == HI_SND_MUTECTL_LEVEL) ? 0 : 1));
    }

    /* enable fast power up */
    adac_tianlai_fast_power_enable(HI_TRUE);
}

#ifdef HI_SND_MUTECTL_SUPPORT
static hi_void snd_op_disable_mute_ctrl(hi_ulong data)
{
    adac_device *device = (adac_device *)((hi_u8 *)HI_NULL + data);

    if (autil_get_amp_type() != AUDIO_AMP_TYPE_EXTERNAL) {
        return;
    }

    /* diable fast power up */
    adac_tianlai_fast_power_enable(HI_FALSE);

    if ((device->gpio_func != HI_NULL) && (device->gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) {
        (device->gpio_func->pfn_gpio_direction_set_bit)(HI_SND_MUTECTL_GPIO, 0);
    }
    if ((device->gpio_func != HI_NULL) && (device->gpio_func->pfn_gpio_write_bit != HI_NULL)) {
        (device->gpio_func->pfn_gpio_write_bit)(HI_SND_MUTECTL_GPIO, ((0 == HI_SND_MUTECTL_LEVEL) ? 1 : 0));
    }
}

static hi_s32 snd_mute_ctrl_init(adac_device *device, hi_bool resume)
{
    hi_s32 ret;

    if (autil_get_amp_type() != AUDIO_AMP_TYPE_EXTERNAL) {
        return HI_SUCCESS;
    }

    ret = osal_exportfunc_get(HI_ID_GPIO, (hi_void **)&device->gpio_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_get, ret);
        return ret;
    }

    osal_timer_init(&device->mute_disable_timer);
    device->mute_disable_timer.handler = snd_op_disable_mute_ctrl;
    device->mute_disable_timer.data = (hi_u8 *)device - (hi_u8 *)(HI_NULL);
    osal_timer_set(&device->mute_disable_timer, AO_SND_MUTE_DISABLE_TIMEMS_RESUME);

    return HI_SUCCESS;
}

static hi_void snd_mute_ctrl_deinit(adac_device *device, hi_bool suspend)
{
    if (autil_get_amp_type() != AUDIO_AMP_TYPE_EXTERNAL) {
        return;
    }

    snd_op_enable_mute_ctrl(device);
    osal_timer_destory(&device->mute_disable_timer);
}
#endif

hi_void snd_set_adac_enable(snd_op_state *snd_op, hi_bool enable)
{
    adac_device *device = get_adac_device(snd_op);

    device->adac_enable = enable;

    if (enable == HI_TRUE) {
#ifdef HI_SND_MUTECTL_SUPPORT
        snd_op_enable_mute_ctrl(device);
        adac_tianlai_init(device->op.snd_port_attr.sample_rate, HI_FALSE);

        if (autil_get_amp_type() == AUDIO_AMP_TYPE_EXTERNAL) {
            osal_timer_set(&device->mute_disable_timer, AO_SND_MUTE_DISABLE_TIMEMS_RESUME);
        }
#else
        adac_tianlai_init(device->op.snd_port_attr.sample_rate, HI_FALSE);
#endif
    } else {
#ifdef HI_SND_MUTECTL_SUPPORT
        snd_op_enable_mute_ctrl(device);
#endif
        adac_tianlai_deinit(HI_FALSE);
    }
}

hi_s32 snd_get_adac_enable(snd_op_state *snd_op, hi_bool *enable)
{
    adac_device *device = get_adac_device(snd_op);

    CHECK_AO_NULL_PTR(enable);
    *enable = device->adac_enable;

    return HI_SUCCESS;
}

static hi_void adac_op_codec_init(adac_device *device, hi_sample_rate sample_rate, hi_bool resume)
{
#if defined(HI_TIANLAI_V500) || defined(HI_TIANLAI_V600) || defined(HI_TIANLAI_V700) || \
    defined(HI_TIANLAI_V730) || defined(HI_TIANLAI_SV100)
    adac_tianlai_init(sample_rate, resume);
#endif

#ifdef HI_SND_MUTECTL_SUPPORT
    if (snd_mute_ctrl_init(device, resume) != HI_SUCCESS) {
        HI_LOG_ERR("call snd_mute_ctrl_init failed!\n");
        return;
    }
#endif

#if defined(HI_TIANLAI_SV100)
    {
        hi_s32 ret;
        /* DAC output decrease -0.875dB for TIANLAI_SV100 */
        hi_ao_preci_gain preci_gain = {
            .integer = TIANLAI_SV100_DEFAULT_PRESCALE_INT,
            .decimal = TIANLAI_SV100_DEFAULT_PRESCALE_DEC,
        };

        ret = snd_op_set_preci_vol(&device->op, &preci_gain);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(snd_op_set_preci_vol, ret);
            return;
        }
    }
#endif
}

static hi_void adac_op_destroy(snd_op_state *snd_op, hi_bool suspend)
{
    adac_device *device = get_adac_device(snd_op);

#ifdef HI_SND_MUTECTL_SUPPORT
    snd_mute_ctrl_deinit(device, suspend);
#endif
    adac_tianlai_deinit(suspend);

    snd_op_destroy(snd_op, suspend);
    osal_kfree(HI_ID_AO, device);
}

static hi_s32 adac_op_create(snd_card_state *card, hi_ao_port_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    adac_device *device = HI_NULL;

    device = (adac_device *)osal_kmalloc(HI_ID_AO, sizeof(adac_device), OSAL_GFP_KERNEL);
    if (device == HI_NULL) {
        HI_LOG_ERR("osal_kmalloc adac_device failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(device, sizeof(*device), 0, sizeof(*device));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out;
    }

    adac_op_init(device);

    ret = snd_op_create_aop(&device->op, attr, SND_AOP_TYPE_I2S, card->user_sample_rate);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_create_aop, ret);
        goto out;
    }

    adac_op_codec_init(device, card->user_sample_rate, resume);

    card->adac_enable = HI_TRUE;
    osal_list_add_tail(&device->op.node, &card->op);

    return ret;

out:
    osal_kfree(HI_ID_AO, device);
    return ret;
}

hi_bool adac_op_match(hi_ao_port ao_port)
{
    hi_u32 i;

    const hi_ao_port adac_op_device[] = {
        HI_AO_PORT_DAC0, HI_AO_PORT_DAC1, HI_AO_PORT_DAC2, HI_AO_PORT_DAC3,
    };

    for (i = 0; i < ARRAY_SIZE(adac_op_device); i++) {
        if (adac_op_device[i] == ao_port) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static const struct {
    hi_ao_port   ao_port;
    hi_char      *buf_name;
    hi_u32       buf_size;
} g_adac_hw_param[] = {
    { HI_AO_PORT_DAC0, "ao_adac0", AO_DAC_MMZSIZE_MAX},
    { HI_AO_PORT_DAC1, "ao_adac1", AO_DAC_MMZSIZE_MAX},
    { HI_AO_PORT_DAC2, "ao_adac2", AO_DAC_MMZSIZE_MAX},
    { HI_AO_PORT_DAC3, "ao_adac3", AO_DAC_MMZSIZE_MAX},
};

static aiao_port_id adac_op_get_port(hi_ao_port out_port)
{
    aiao_port_id port_id;

    switch (out_port) {
        case HI_AO_PORT_DAC0:
            if (autil_get_chip_platform() == AUTIL_CHIP_TYPE_PLATFORM_S28) {
                port_id = AIAO_PORT_TX1;
            } else {
                port_id = AIAO_PORT_TX2;
            }
            break;

        case HI_AO_PORT_DAC1:
            port_id = AIAO_PORT_TX3;
            break;

        case HI_AO_PORT_DAC2:
            port_id = AIAO_PORT_TX4;
            break;

        case HI_AO_PORT_DAC3:
            port_id = AIAO_PORT_TX5;
            break;

        default:
            HI_LOG_ERR("invalid outport\n");
            HI_ERR_PRINT_H32(out_port);
            port_id = AIAO_PORT_MAX;
    }

    return port_id;
}

static hi_s32 adac_op_get_hw_param(hi_ao_port_attr *attr, snd_aop_type aop_type,
    snd_op_create_param *hw_param)
{
    hi_u32 i;

    /* 1.adac get port id */
    hw_param->aiao_port = adac_op_get_port(attr->port);
    if (hw_param->aiao_port == AIAO_PORT_MAX) {
        HI_LOG_ERR("adac_op_get_port failed\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    /* 2.find adac hw_param and save */
    for (i = 0; i < ARRAY_SIZE(g_adac_hw_param); i++) {
        if (g_adac_hw_param[i].ao_port == attr->port) {
            hw_param->buf_name = g_adac_hw_param[i].buf_name;
            hw_param->buf_size = g_adac_hw_param[i].buf_size;
            break;
        }
    }

    /* 3. adac get hw cfg */
    hal_aiao_get_tx_i2s_df_attr(hw_param->aiao_port, hw_param->port_cfg);

    return HI_SUCCESS;
}

static snd_output_port_driver g_adac_op_driver = {
    .match   = adac_op_match,
    .create  = adac_op_create,
    .destroy = adac_op_destroy,
    .get_hw_param = adac_op_get_hw_param,
};

hi_void adac_op_register_driver(struct osal_list_head *head)
{
    osal_list_add_tail(&g_adac_op_driver.node, head);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


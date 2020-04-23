/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv port i2s function.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"
#include "i2s_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_void i2s_op_destroy(snd_op_state *state, hi_bool suspend)
{
    snd_op_destroy(state, suspend);
    osal_kfree(HI_ID_AO, state);
}

static hi_s32 i2s_op_create(snd_card_state *card, hi_ao_port_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    snd_op_state *state = HI_NULL;

    state = (snd_op_state *)osal_kmalloc(HI_ID_AO, sizeof(snd_op_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_FATAL_AO("osal_kmalloc snd_op_state failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(state, sizeof(snd_op_state), 0, sizeof(snd_op_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out0;
    }

    snd_op_init(state);

    ret = snd_op_create_aop(state, attr, SND_AOP_TYPE_I2S, card->user_sample_rate);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_op_create_aop, ret);
        goto out0;
    }

    ret = memcpy_s(&state->snd_port_attr.un_attr,
        sizeof(hi_ao_i2s_attr), &attr->un_attr, sizeof(hi_ao_i2s_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        goto out1;
    }

    state->out_type = SND_OUTPUT_TYPE_I2S;

    osal_list_add_tail(&state->node, &card->op);

    return HI_SUCCESS;

out1:
    snd_op_destroy(state, HI_FALSE);

out0:
    osal_kfree(HI_ID_AO, state);
    return ret;
}

static hi_bool i2s_op_match(hi_ao_port ao_port)
{
    hi_u32 i;
    const hi_ao_port i2s_op_device[] = {
        HI_AO_PORT_I2S0, HI_AO_PORT_I2S1, HI_AO_PORT_I2S2, HI_AO_PORT_I2S3, HI_AO_PORT_I2S4,
    };

    for (i = 0; i < ARRAY_SIZE(i2s_op_device); i++) {
        if (i2s_op_device[i] == ao_port) {
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static const struct {
    hi_ao_port   ao_port;
    snd_aop_type aop_type;
    hi_u32       i2s_id;
    hi_char      *buf_name;
    hi_u32       buf_size;
} g_i2s_hw_param[] = {
    { HI_AO_PORT_I2S0, SND_AOP_TYPE_I2S, 0, "ao_i2s0", AO_I2S_MMZSIZE_MAX},
    { HI_AO_PORT_I2S1, SND_AOP_TYPE_I2S, 1, "ao_i2s1", AO_I2S_MMZSIZE_MAX},
    { HI_AO_PORT_I2S2, SND_AOP_TYPE_I2S, 2, "ao_i2s2", AO_I2S_MMZSIZE_MAX},
    { HI_AO_PORT_I2S3, SND_AOP_TYPE_I2S, 3, "ao_i2s3", AO_I2S_MMZSIZE_MAX},
    { HI_AO_PORT_I2S4, SND_AOP_TYPE_I2S, 4, "ao_i2s4", AO_I2S_MMZSIZE_MAX},
};

static hi_s32 i2s_op_get_hw_param(hi_ao_port_attr *attr, snd_aop_type aop_type,
    snd_op_create_param *hw_param)
{
    hi_u32 i;

    /* 1.i2s get port id */
    hw_param->aiao_port = AIAO_PORT_MAX;

    /* 2.find i2s hw hw_param and save */
    for (i = 0; i < ARRAY_SIZE(g_i2s_hw_param); i++) {
        if (g_i2s_hw_param[i].ao_port == attr->port) {
            hw_param->buf_name = g_i2s_hw_param[i].buf_name;
            hw_param->buf_size = g_i2s_hw_param[i].buf_size;
            hw_param->i2s_id = g_i2s_hw_param[i].i2s_id;
            break;
        }
    }

    /* 3. i2s get hw cfg */
    hal_aiao_get_board_tx_i2s_df_attr(hw_param->i2s_id, &attr->un_attr.i2s_attr.attr,
                                      &hw_param->aiao_port, hw_param->port_cfg);

    return HI_SUCCESS;
}

static snd_output_port_driver g_i2s_op_driver = {
    .match   = i2s_op_match,
    .create  = i2s_op_create,
    .destroy = i2s_op_destroy,
    .get_hw_param = i2s_op_get_hw_param,
};

hi_void i2s_op_register_driver(struct osal_list_head *head)
{
    osal_list_add_tail(&g_i2s_op_driver.node, head);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


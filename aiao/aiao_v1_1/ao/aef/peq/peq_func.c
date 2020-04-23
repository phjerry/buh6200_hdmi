/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv drc function implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "peq_func.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void snd_op_get_def_peq_attr(snd_op_state *state)
{
    hi_s32 ret;
    hi_ao_peq_attr df_peq_attr = {
        5,
        {
            { 2, 120, 20, 0 },
            { 2, 500, 20, 0 },
            { 2, 1500, 20, 0 },
            { 2, 5000, 20, 0 },
            { 2, 10000, 20, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }
    };

    ret = memcpy_s(&state->peq_attr, sizeof(state->peq_attr),
        &df_peq_attr, sizeof(df_peq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return;
    }
}

hi_s32 snd_op_set_peq_enable(snd_op_state *snd_op, hi_bool enable)
{
    hi_s32 ret;
    hi_u32 idx;
    aoe_aop_id aop;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_peq_enable(aop, enable);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_peq_enable, ret);
                HI_ERR_PRINT_U32(aop);
                return ret;
            }
        }
    }

    snd_op->peq_enable = enable;

    return HI_SUCCESS;
}

hi_s32 snd_op_set_peq_attr(snd_op_state *snd_op, hi_ao_peq_attr *peq_attr)
{
    hi_s32 ret;
    hi_u32 idx;
    aoe_aop_id aop;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_peq_attr(aop, peq_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_peq_attr, ret);
                HI_ERR_PRINT_U32(aop);
                return ret;
            }
        }
    }

    ret = memcpy_s(&snd_op->peq_attr, sizeof(snd_op->peq_attr), peq_attr, sizeof(*peq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 snd_set_op_peq_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_peq_attr *peq_attr)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_peq_attr(snd_op, peq_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_peq_attr, ret);
                HI_ERR_PRINT_H32(snd_op->out_port);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_peq_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_peq_attr *peq_attr)
{
    hi_s32 ret;
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get peq attr of all port!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = memcpy_s(peq_attr, sizeof(*peq_attr), &snd_op->peq_attr, sizeof(snd_op->peq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 snd_set_op_peq_enable(snd_card_state *card, hi_ao_port out_port, hi_bool enable)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_peq_enable(snd_op, enable);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_peq_enable, ret);
                HI_ERR_PRINT_H32(snd_op->out_port);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_peq_enable(snd_card_state *card, hi_ao_port out_port, hi_bool *enable)
{
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get peq enable of all port\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *enable = snd_op->peq_enable;

    return HI_SUCCESS;
}

#ifdef HI_PROC_SUPPORT
hi_s32 snd_write_op_proc_peq(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    const hi_char *pc_on_cmd = "on";
    const hi_char *pc_off_cmd = "off";
    const hi_char *pc_one_cmd = "1";
    const hi_char *pc_zero_cmd = "0";
    const hi_char *pc_band = "band";
    const hi_char *pc_freq = "freq";
    const hi_char *pc_gain = "gain";
    const hi_char *pc_type = "type";
    const hi_char *pc_q = "Q";
    hi_u32 band_id;
    hi_u32 val1, val2;
    hi_s32 ret;
    hi_ao_peq_filter_type type;
    hi_bool nagetive = HI_FALSE;
    hi_bool peq_enable = HI_FALSE;
    hi_ao_peq_attr peq_attr = { 0 };

    if ((pc_buf == strstr(pc_buf, pc_on_cmd)) || (pc_buf == strstr(pc_buf, pc_one_cmd))) {
        ret = snd_get_op_peq_enable(card, out_port, &peq_enable);
        if (ret != HI_SUCCESS) {
            osal_printk("get PEQ enable failed!\n");
        }

        ret = snd_set_op_peq_enable(card, out_port, HI_TRUE);
        if (ret != HI_SUCCESS) {
            HI_LOG_ERR("set PEQ enable failed!\n");
        } else {
            osal_printk("set %s PEQ success: %s -> 1(ON)\n", autil_port_to_name(out_port),
                peq_enable ? "1(ON)" : "0(OFF)");
        }

        return ret;
    } else if ((pc_buf == strstr(pc_buf, pc_off_cmd)) || (pc_buf == strstr(pc_buf, pc_zero_cmd))) {
        ret = snd_get_op_peq_enable(card, out_port, &peq_enable);
        if (ret != HI_SUCCESS) {
            osal_printk("get PEQ enable failed!\n");
        }

        ret = snd_set_op_peq_enable(card, out_port, HI_FALSE);
        if (ret != HI_SUCCESS) {
            HI_LOG_ERR("set PEQ enable failed!\n");
        } else {
            osal_printk("set %s PEQ success: %s -> 0(OFF)\n",
                autil_port_to_name(out_port), peq_enable ? "1(ON)" : "0(OFF)");
        }

        return ret;
    } else if (pc_buf == strstr(pc_buf, pc_band)) {
        snd_get_op_peq_attr(card, out_port, &peq_attr);

        pc_buf += strlen(pc_band);
        band_id = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
        if (band_id >= peq_attr.band_num) {
            return HI_FAILURE;
        }
        AO_STRING_SKIP_BLANK(pc_buf);

        if (pc_buf == strstr(pc_buf, pc_freq)) {
            pc_buf += strlen(pc_freq);
            AO_STRING_SKIP_BLANK(pc_buf);
            val1 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            peq_attr.param[band_id].freq = val1;
        } else if (pc_buf == strstr(pc_buf, pc_gain)) {
            pc_buf += strlen(pc_gain);
            AO_STRING_SKIP_BLANK(pc_buf);
            if (pc_buf[0] == '-') {
                nagetive = HI_TRUE;
                pc_buf++;
            }
            val1 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            if (pc_buf[0] == '.') {
                pc_buf++;
                val2 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            } else {
                val2 = 0;
            }
            peq_attr.param[band_id].gain = (nagetive == HI_TRUE) ? (-(val1 * 1000 + val2)) : (val1 * 1000 + val2);
        } else if (pc_buf == strstr(pc_buf, pc_type)) {
            pc_buf += strlen(pc_type);
            AO_STRING_SKIP_BLANK(pc_buf);
            type = autil_peq_type_name_to_type(pc_buf);
            if (type == HI_AO_PEQ_FILTER_TYPE_MAX) {
                return HI_FAILURE;
            }
            peq_attr.param[band_id].type = type;
        } else if (pc_buf == strstr(pc_buf, pc_q)) {
            pc_buf += strlen(pc_q);
            AO_STRING_SKIP_BLANK(pc_buf);
            val1 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            if (pc_buf[0] == '.') {
                pc_buf++;
                val2 = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            } else {
                val2 = 0;
            }
            peq_attr.param[band_id].q = val1 * 10 + val2;
        } else {
            return HI_FAILURE;
        }
        return snd_set_op_peq_attr(card, out_port, &peq_attr);
    } else {
        return HI_FAILURE;
    }
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


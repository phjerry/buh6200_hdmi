/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drc function implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "drc_func.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void snd_op_get_def_drc_attr(snd_op_state *state)
{
    state->drc_enable = HI_FALSE;
    state->drc_attr.attack_time = 50;
    state->drc_attr.release_time = 100;
    state->drc_attr.threshold = -40;
    state->drc_attr.limit = -10;
}

hi_s32 snd_op_set_drc_enable(snd_op_state *snd_op, hi_bool enable)
{
    hi_s32 ret;
    hi_u32 idx;
    aoe_aop_id aop;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_drc_enable(aop, enable);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_drc_enable, ret);
                HI_ERR_PRINT_U32(aop);
                return ret;
            }
        }
    }

    snd_op->drc_enable = enable;

    return HI_SUCCESS;
}

hi_s32 snd_op_set_drc_attr(snd_op_state *snd_op, hi_ao_drc_attr *drc_attr)
{
    hi_u32 idx;
    aoe_aop_id aop;
    hi_s32 ret;

    for (idx = 0; idx < AO_SNDOP_MAX_AOP_NUM; idx++) {
        aop = snd_op->aop[idx];
        if (aop < AOE_AOP_MAX) {
            ret = hal_aoe_aop_set_drc_attr(aop, drc_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_drc_attr, ret);
                HI_ERR_PRINT_U32(aop);
                return ret;
            }
        }
    }

    ret = memcpy_s(&snd_op->drc_attr, sizeof(snd_op->drc_attr),
        drc_attr, sizeof(*drc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 snd_set_op_drc_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_drc_attr *drc_attr)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_drc_attr(snd_op, drc_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_drc_attr, ret);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_drc_attr(snd_card_state *card, hi_ao_port out_port, hi_ao_drc_attr *drc_attr)
{
    hi_s32 ret;
    snd_op_state *op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get drc_attr of all port!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    op = snd_get_op_handle_by_out_port(card, out_port);
    if (op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = memcpy_s(drc_attr, sizeof(*drc_attr), &op->drc_attr, sizeof(op->drc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 snd_set_op_drc_enable(snd_card_state *card, hi_ao_port out_port, hi_bool enable)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_drc_enable(snd_op, enable);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_drc_enable, ret);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_drc_enable(snd_card_state *card, hi_ao_port out_port, hi_bool *enable)
{
    snd_op_state *op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_ERR_AO("don't support get drc_enable of allport!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    op = snd_get_op_handle_by_out_port(card, out_port);
    if (op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *enable = op->drc_enable;
    return HI_SUCCESS;
}

#ifdef HI_PROC_SUPPORT
hi_s32 snd_write_op_proc_drc(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    const hi_char *pc_on_cmd = "on";
    const hi_char *pc_off_cmd = "off";
    const hi_char *pc_one_cmd = "1";
    const hi_char *pc_zero_cmd = "0";
    const hi_char *pc_attack_time = "attacktime";
    const hi_char *pc_release_time = "releasetime";
    const hi_char *pc_thrhd = "thrhd";
    const hi_char *pc_limit = "limit";
    hi_bool get_drc_en = HI_FALSE;
    hi_s32 ret;

    if ((pc_buf == strstr(pc_buf, pc_on_cmd)) || (pc_buf == strstr(pc_buf, pc_one_cmd))) {
        ret = snd_get_op_drc_enable(card, out_port, &get_drc_en);
        if (ret != HI_SUCCESS) {
            osal_printk("snd_get_op_drc_enable failed!\n");
            return ret;
        } else {
            ret = snd_set_op_drc_enable(card, out_port, HI_TRUE);
            if (ret != HI_SUCCESS) {
                osal_printk("snd_get_op_drc_enable failed!\n");
                return ret;
            } else {
                osal_printk("set %s drc ON success: %s -> 1(ON)\n", autil_port_to_name(out_port),
                    (hi_u32)get_drc_en, (get_drc_en ? "1(ON)" : "0(OFF)"));
                return ret;
            }
        }
    } else if ((pc_buf == strstr(pc_buf, pc_off_cmd)) || (pc_buf == strstr(pc_buf, pc_zero_cmd))) {
        ret = snd_get_op_drc_enable(card, out_port, &get_drc_en);
        if (ret != HI_SUCCESS) {
            osal_printk("snd_get_op_drc_enable failed!\n");
            return ret;
        } else {
            ret = snd_set_op_drc_enable(card, out_port, HI_FALSE);
            if (ret != HI_SUCCESS) {
                osal_printk("snd_get_op_drc_enable failed!\n");
                return ret;
            } else {
                osal_printk("set %s drc OFF success: %s -> 0(OFF)\n", autil_port_to_name(out_port),
                    (hi_u32)get_drc_en, (get_drc_en ? "1(ON)" : "0(OFF)"));
                return ret;
            }
        }
    } else {
        hi_ao_drc_attr set_drc_attr = { 0 };
        hi_ao_drc_attr get_drc_attr = { 0 };
        hi_ao_drc_attr *drc_attr = &set_drc_attr;
        snd_get_op_drc_attr(card, out_port, &set_drc_attr);
        snd_get_op_drc_attr(card, out_port, &get_drc_attr);

        if (pc_buf == strstr(pc_buf, pc_attack_time)) {
            pc_buf += strlen(pc_attack_time);
            AO_STRING_SKIP_BLANK(pc_buf);
            set_drc_attr.attack_time = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            if ((set_drc_attr.attack_time < 20) || (set_drc_attr.attack_time > 2000)) {
                osal_printk("invalid attacktime value! attacktime value range: 20ms~2000ms\n");
                return HI_FAILURE;
            }

            CHECK_AO_DRCATTR(drc_attr);
            ret = snd_set_op_drc_attr(card, out_port, &set_drc_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("snd_set_op_drc_attr failed!\n");
                return ret;
            } else {
                osal_printk("set %s drc attacktime success: %dms -> %dms\n", autil_port_to_name(out_port),
                    get_drc_attr.attack_time, set_drc_attr.attack_time);
                return ret;
            }
        } else if (pc_buf == strstr(pc_buf, pc_release_time)) {
            pc_buf += strlen(pc_release_time);
            AO_STRING_SKIP_BLANK(pc_buf);
            set_drc_attr.release_time = (hi_u32)simple_strtoul(pc_buf, &pc_buf, 10);
            if ((set_drc_attr.release_time < 20) || (set_drc_attr.release_time > 2000)) {
                osal_printk("invalid releasetime value! releasetime value range: 20ms~2000ms\n");
                return HI_FAILURE;
            }

            CHECK_AO_DRCATTR(drc_attr);
            ret = snd_set_op_drc_attr(card, out_port, &set_drc_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("snd_set_op_drc_attr failed!\n");
                return ret;
            } else {
                osal_printk("set %s drc releasetime success: %dms -> %dms\n",
                    autil_port_to_name(out_port), get_drc_attr.release_time, set_drc_attr.release_time);
                return ret;
            }
        } else if (pc_buf == strstr(pc_buf, pc_thrhd)) {
            pc_buf += strlen(pc_thrhd);
            AO_STRING_SKIP_BLANK(pc_buf);
            set_drc_attr.threshold = simple_strtol(pc_buf, &pc_buf, 10);
            if ((set_drc_attr.threshold < -80) || (set_drc_attr.threshold > -1)) {
                osal_printk("invalid threshold value! threshold value range: -80dB~-1dB\n");
                return HI_FAILURE;
            }

            CHECK_AO_DRCATTR(drc_attr);
            ret = snd_set_op_drc_attr(card, out_port, &set_drc_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("snd_set_op_drc_attr failed!\n");
                return ret;
            } else {
                osal_printk("set %s drc threshold value success: %ddB -> %ddB\n",
                    autil_port_to_name(out_port), get_drc_attr.threshold, set_drc_attr.threshold);
                return ret;
            }
        } else if (pc_buf == strstr(pc_buf, pc_limit)) {
            pc_buf += strlen(pc_limit);
            AO_STRING_SKIP_BLANK(pc_buf);
            set_drc_attr.limit = simple_strtol(pc_buf, &pc_buf, 10);
            if ((set_drc_attr.limit < -80) || (set_drc_attr.limit > -1)) {
                osal_printk("invalid limit value! limit value range: -80dB~-1dB\n");
                return HI_FAILURE;
            }

            CHECK_AO_DRCATTR(drc_attr);
            ret = snd_set_op_drc_attr(card, out_port, &set_drc_attr);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("snd_set_op_drc_attr failed!\n");
            } else {
                osal_printk("set %s drc limit value success: %d -> %d\n", autil_port_to_name(out_port),
                    get_drc_attr.limit, set_drc_attr.limit);
            }

            return ret;
        } else {
            return HI_FAILURE;
        }
    }
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end__cplusplus */

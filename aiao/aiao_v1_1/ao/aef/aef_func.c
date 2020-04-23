/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv aef function implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"

#include "aef_func.h"
#include "drc_func.h"
#include "peq_func.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 aef_attach_snd(snd_card_state *card, hi_u32 aef_id, ao_aef_attr *aef_attr, hi_u32 *aef_proc_addr)
{
    return HI_SUCCESS;
}

hi_s32 aef_detach_snd(snd_card_state *card, hi_u32 aef_id, ao_aef_attr *aef_attr)
{
    return HI_SUCCESS;
}

hi_s32 aef_get_buf_attr(snd_card_state *card, ao_aef_buf_attr *aef_buf)
{
    return HI_SUCCESS;
}

hi_s32 aef_get_debug_addr(snd_card_state *card, aef_debug_attr *debug_attr)
{
    return HI_SUCCESS;
}

hi_s32 snd_set_op_aef_bypass(snd_card_state *card, hi_ao_port out_port, hi_bool bypass)
{
    hi_s32 ret = HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    snd_op_state *snd_op = HI_NULL;

    osal_list_for_each_entry(snd_op, &card->op, node) {
        if ((out_port == HI_AO_PORT_ALL) || (out_port == snd_op->out_port)) {
            ret = snd_op_set_aef_bypass(snd_op, bypass);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(snd_op_set_aef_bypass, ret);
                return ret;
            }
        }
    }

    return ret;
}

hi_s32 snd_get_op_aef_bypass(snd_card_state *card, hi_ao_port out_port, hi_bool *bypass)
{
    snd_op_state *snd_op = HI_NULL;

    if (out_port == HI_AO_PORT_ALL) {
        HI_LOG_ERR("don't support get aef bypass status of all port!\n");
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    *bypass = snd_op->bypass;
    return HI_SUCCESS;
}

hi_s32 snd_op_set_aef_bypass(snd_op_state *snd_op, hi_bool bypass)
{
    hi_s32 ret;
    aoe_aop_id aop;

    aop = snd_op->aop[snd_op->active_id];
    ret = hal_aoe_aop_set_aef_bypass(aop, bypass);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_aef_bypass, ret);
        return ret;
    }

    snd_op->bypass = bypass;

    return ret;
}

hi_void snd_op_set_def_aef_attr(snd_op_state *snd_op, aoe_aop_id aop)
{
#ifdef HI_SND_DRC_SUPPORT
    snd_op_get_def_drc_attr(snd_op);
    hal_aoe_aop_set_drc_attr(aop, &snd_op->drc_attr);
#endif

#ifdef HI_SND_PEQ_SUPPORT
    snd_op_get_def_peq_attr(snd_op);
    hal_aoe_aop_set_peq_attr(aop, &snd_op->peq_attr);
#endif
}

#ifdef HI_PROC_SUPPORT
hi_s32 snd_write_op_proc_aef_bypass(snd_card_state *card, hi_ao_port out_port, hi_char *pc_buf)
{
    hi_s32 ret;
    hi_u32 i;
    hi_bool set_aef_bypass = HI_FALSE;
    hi_bool get_aef_bypass = HI_FALSE;

    struct {
        const hi_char *cmd;
        hi_bool value;
    } cmd_map[] = {
        { "on",  HI_TRUE },
        { "off", HI_FALSE },
        { "1",   HI_TRUE },
        { "0",   HI_FALSE },
    };

    for (i = 0; i < ARRAY_SIZE(cmd_map); i++) {
        if (pc_buf == strstr(pc_buf, cmd_map[i].cmd)) {
            set_aef_bypass = cmd_map[i].value;
            break;
        }
    }

    if (i >= ARRAY_SIZE(cmd_map)) {
        return HI_FAILURE;
    }

    ret = snd_get_op_aef_bypass(card, out_port, &get_aef_bypass);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_op_aef_bypass, ret);
        return ret;
    }

    ret = snd_set_op_aef_bypass(card, out_port, set_aef_bypass);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_set_op_aef_bypass, ret);
        return ret;
    }

    osal_printk("set %s aef bypass %s success:%d(%s) -> 1(%s)\n",
        autil_port_to_name(out_port),
        TRUE_ON_FALSE_OFF(set_aef_bypass),
        get_aef_bypass,
        TRUE_ON_FALSE_OFF(get_aef_bypass),
        TRUE_ON_FALSE_OFF(set_aef_bypass));

    return ret;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


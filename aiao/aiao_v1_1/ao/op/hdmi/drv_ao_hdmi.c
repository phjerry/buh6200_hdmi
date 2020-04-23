/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv hdmi implement.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_aiao_log.h"

#include "hdmi_func.h"
#include "track_func.h"
#include "drv_ar.h"
#include "drv_ao_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_bool ao_snd_hdmi_check_format_support(snd_card_state *card, hi_ao_port out_port, hi_u32 format)
{
    snd_op_state *snd_op = HI_NULL;

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_FALSE;
    }

    return hdmi_op_check_format_support(snd_op, format);
}

hi_void ao_snd_hdmi_set_mute(snd_card_state *card, hi_ao_port out_port, hi_bool mute)
{
    snd_op_state *snd_op = HI_NULL;

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        HI_LOG_ERR("snd_get_op_handle_by_out_port failed\n");
        return;
    }

    if (mute == HI_TRUE) {
        return hdmi_op_set_mute(snd_op);
    } else {
        return hdmi_op_set_unmute(snd_op);
    }
}

hi_void ao_snd_hdmi_set_ao_attr(snd_card_state *card, hi_ao_port out_port, hdmi_ao_attr *ao_attr)
{
    snd_op_state *snd_op = HI_NULL;

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        HI_LOG_ERR("snd_get_op_handle_by_out_port failed\n");
        return;
    }

    return hdmi_op_set_ao_attr(snd_op, ao_attr);
}

#ifdef HI_PROC_SUPPORT
static hi_s32 ao_snd_set_hdmi_mode(ao_snd_id sound, hi_ao_port out_port, hi_ao_ouput_mode mode)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_HDMI_MODE(mode);
    CHECK_AO_NULL_PTR(card);

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    ret = hdmi_op_set_mode(card, snd_op, mode);
#if defined(HI_SND_AR_SUPPORT)
    if (ret == HI_SUCCESS) {
        ao_snd_update_render_raw_info(sound, mode);
    }
#endif

    return ret;
}

hi_s32 ao_proc_set_hdmi_debug(ao_snd_id sound, snd_card_state *card)
{
    card->hdmi_debug = !(card->hdmi_debug);
    return HI_SUCCESS;
}

hi_s32 ao_write_proc_set_hdmi(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    const hi_char *debug_cmd = "debug";
    const hi_char *pcm_cmd = "pcm";
    const hi_char *raw_cmd = "raw";
    const hi_char *hbr2lbr_cmd = "hbr2lbr";
    const hi_char *auto_cmd = "auto";
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (argv[1] == strstr(argv[1], debug_cmd)) {
        return ao_proc_set_hdmi_debug(card->sound, card);
    } else if (argv[1] == strstr(argv[1], pcm_cmd)) {
        return ao_snd_set_hdmi_mode(card->sound, card->hdmi_port, HI_AO_OUTPUT_MODE_LPCM);
    } else if (argv[1] == strstr(argv[1], raw_cmd)) {
        return ao_snd_set_hdmi_mode(card->sound, card->hdmi_port, HI_AO_OUTPUT_MODE_RAW);
    } else if (argv[1] == strstr(argv[1], hbr2lbr_cmd)) {
        return ao_snd_set_hdmi_mode(card->sound, card->hdmi_port, HI_AO_OUTPUT_MODE_HBR2LBR);
    } else if (argv[1] == strstr(argv[1], auto_cmd)) {
        return ao_snd_set_hdmi_mode(card->sound, card->hdmi_port, HI_AO_OUTPUT_MODE_AUTO);
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

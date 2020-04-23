/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: spdif implement of ao driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "drv_ao_spdif.h"
#include "spdif_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_s32 ao_snd_set_spdif_category_code(ao_snd_id sound, hi_ao_port out_port,
    hi_ao_spdif_category_code category_code)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_CATEGORYCODE(category_code);

    if (spdif_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return spdif_op_set_category_code(snd_op, category_code);
}

static hi_s32 ao_snd_get_spdif_category_code(ao_snd_id sound, hi_ao_port out_port,
    hi_ao_spdif_category_code *category_code)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(category_code);

    if (spdif_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return spdif_op_get_category_code(snd_op, category_code);
}

static hi_s32 ao_snd_set_spdif_scms_mode(ao_snd_id sound, hi_ao_port out_port,
    hi_ao_spdif_scms_mode scms_mode)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_SPDIFSCMSMODE(scms_mode);

    if (spdif_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return spdif_op_set_scms_mode(snd_op, scms_mode);
}

static hi_s32 ao_snd_get_spdif_scms_mode(ao_snd_id sound, hi_ao_port out_port,
    hi_ao_spdif_scms_mode *scms_mode)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(scms_mode);

    if (spdif_op_match(out_port) != HI_TRUE) {
        return HI_ERR_AO_INVALID_PARA;
    }

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return spdif_op_get_scms_mode(snd_op, scms_mode);
}

hi_s32 ao_ioctl_snd_set_spdif_scms_mode(hi_void *file, hi_void *arg)
{
    ao_snd_spdif_scms_mode_param_p scms_mode = (ao_snd_spdif_scms_mode_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(scms_mode->sound);
    return ao_snd_set_spdif_scms_mode(scms_mode->sound, scms_mode->out_port, scms_mode->scms_mode);
}

hi_s32 ao_ioctl_snd_get_spdif_scms_mode(hi_void *file, hi_void *arg)
{
    ao_snd_spdif_scms_mode_param_p scms_mode = (ao_snd_spdif_scms_mode_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(scms_mode->sound);
    return ao_snd_get_spdif_scms_mode(scms_mode->sound, scms_mode->out_port, &scms_mode->scms_mode);
}

hi_s32 ao_ioctl_snd_set_spdif_category_code(hi_void *file, hi_void *arg)
{
    ao_snd_spdif_category_code_param_p category_code = (ao_snd_spdif_category_code_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(category_code->sound);
    return ao_snd_set_spdif_category_code(category_code->sound, category_code->out_port, category_code->category_code);
}

hi_s32 ao_ioctl_snd_get_spdif_category_code(hi_void *file, hi_void *arg)
{
    ao_snd_spdif_category_code_param_p category_code = (ao_snd_spdif_category_code_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(category_code->sound);
    return ao_snd_get_spdif_category_code(category_code->sound, category_code->out_port, &category_code->category_code);
}

#ifdef HI_PROC_SUPPORT
static hi_s32 ao_snd_set_spdif_mode(ao_snd_id sound, hi_ao_port out_port, hi_ao_ouput_mode mode)
{
    snd_card_state *card = snd_card_get_card(sound);
    snd_op_state *snd_op = HI_NULL;

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_SPDIF_MODE(mode);

    snd_op = snd_get_op_handle_by_out_port(card, out_port);
    if (snd_op == HI_NULL) {
        return HI_ERR_AO_OUTPORT_NOT_ATTATCH;
    }

    return spdif_op_set_output_mode(card, snd_op, mode);
}

hi_s32 ao_write_proc_set_spdif(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    const hi_char *pcm_cmd = "pcm";
    const hi_char *raw_cmd = "raw";
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (argv[1] == strstr(argv[1], pcm_cmd)) {
        return ao_snd_set_spdif_mode(card->sound, HI_AO_PORT_SPDIF0, HI_AO_OUTPUT_MODE_LPCM);
    } else if (argv[1] == strstr(argv[1], raw_cmd)) {
        return ao_snd_set_spdif_mode(card->sound, HI_AO_PORT_SPDIF0, HI_AO_OUTPUT_MODE_RAW);
    } else {
        return HI_ERR_AO_INVALID_PARA;
    }
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */


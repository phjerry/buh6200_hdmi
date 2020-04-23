/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao alsa driver first layer
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_ALSA_FUNC_H__
#define __DRV_AO_ALSA_FUNC_H__

#include "hi_ao_type.h"
#include "hi_drv_ao.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int aoe_set_volume(ao_snd_id sound, hi_ao_port out_port, hi_ao_gain *gain);
int aoe_set_mute(ao_snd_id sound, hi_ao_port out_port, hi_bool mute);
int aoe_get_mute(ao_snd_id sound, hi_ao_port out_port, hi_bool *mute);
int aoe_get_delay(ao_snd_id sound, hi_u32 *delay_ms);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

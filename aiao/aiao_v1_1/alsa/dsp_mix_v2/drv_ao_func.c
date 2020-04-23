/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: ao alsa driver first layer
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "drv_ao_func.h"
#include "hi_drv_ao.h"
#include "drv_ao_ioctl.h"
#include "drv_ao_private.h"
#include "drv_ao_ext.h"

int aoe_set_volume(ao_snd_id sound, hi_ao_port out_port, hi_ao_gain *gain)
{
    return hi_drv_ao_snd_set_volume(sound, out_port, gain);
}

int aoe_set_mute(ao_snd_id sound, hi_ao_port out_port, hi_bool mute)
{
    return hi_drv_ao_snd_set_mute(sound, out_port, mute);
}

int aoe_get_mute(ao_snd_id sound, hi_ao_port out_port, hi_bool *mute)
{
    return hi_drv_ao_snd_get_mute(sound, out_port, mute);
}

int aoe_get_delay(ao_snd_id sound, hi_u32 *delay)
{
    return hi_drv_ao_snd_get_alsa_track_delay(sound, delay);
}

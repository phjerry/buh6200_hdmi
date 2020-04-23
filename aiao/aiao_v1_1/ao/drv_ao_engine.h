/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao engine api
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_ENGINE_H__
#define __DRV_AO_ENGINE_H__

#include "hi_drv_audio.h"
#include "drv_ao_private.h"

aoe_engine_id ao_engine_get_id_by_type(snd_card_state *card, hi_u32 data_type);

hi_void ao_engine_deinit(snd_card_state *card);
hi_s32 ao_engine_init(snd_card_state *card);

#endif  /* __DRV_AO_ENGINE_H__ */

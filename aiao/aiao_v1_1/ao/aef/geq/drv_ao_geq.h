/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv geq include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_GEQ_H__
#define __DRV_AO_GEQ_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* aiao geq base functions */
hi_s32 ao_get_def_geq_attr(hi_ao_geq_attr *geq_attr);
hi_s32 ao_check_geq_attr(hi_ao_geq_attr *geq_attr);
hi_s32 ao_snd_set_geq_attr(ao_snd_id sound, hi_ao_geq_attr *geq_attr);
hi_s32 ao_snd_get_geq_attr(ao_snd_id sound, hi_ao_geq_attr *geq_attr);
hi_s32 ao_snd_set_geq_enable(ao_snd_id sound, hi_bool enable);
hi_s32 ao_snd_get_geq_enable(ao_snd_id sound, hi_bool *enable);
hi_s32 ao_snd_set_geq_gain(ao_snd_id sound, hi_u32 band, hi_s32 gain);
hi_s32 ao_snd_get_geq_gain(ao_snd_id sound, hi_u32 band, hi_s32 *gain);

/* aiao geq ioctl */
hi_s32 ao_ioctl_snd_set_geq_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_geq_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_geq_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_geq_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_geq_gain(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_geq_gain(hi_void *file, hi_void *arg);

/* aiao geq proc functions */
#ifdef HI_PROC_SUPPORT
hi_s32 ao_write_proc_geq(ao_snd_id sound, snd_card_state *card, hi_char *pc_buf);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_GEQ_H__ */


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv peq include files.
 * Author: sdk
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_PEQ_H__
#define __DRV_AO_PEQ_H__

#include "drv_ao_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* aiao peq base functions */
hi_s32 ao_check_peq_band_num(hi_u32 band_num);
hi_s32 ao_check_peq_band_attr(hi_ao_peq_band_attr *eq_param);
hi_s32 ao_check_peq_attr(hi_ao_peq_attr *peq_attr);
hi_s32 ao_snd_set_peq_attr(ao_snd_id sound, hi_ao_port out_port, hi_ao_peq_attr *peq_attr);
hi_s32 ao_snd_get_peq_attr(ao_snd_id sound, hi_ao_port out_port, hi_ao_peq_attr *peq_attr);
hi_s32 ao_snd_set_peq_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable);
hi_s32 ao_snd_get_peq_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable);

/* aiao peq ioctl */
hi_s32 ao_ioctl_snd_set_peq_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_peq_attr(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_set_peq_enable(hi_void *file, hi_void *arg);
hi_s32 ao_ioctl_snd_get_peq_enable(hi_void *file, hi_void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_AO_PEQ_H__ */


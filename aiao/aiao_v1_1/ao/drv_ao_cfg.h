/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv common header config
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_AO_CFG_H__
#define __DRV_AO_CFG_H__

#if defined(HI_SND_AEF_SUPPORT)
#include "drv_ao_aef.h"
#if defined(HI_SND_AVC_SUPPORT)
#include "drv_ao_avc.h"
#endif

#if defined(HI_SND_DRC_SUPPORT)
#include "drv_ao_drc.h"
#endif

#if defined(HI_SND_GEQ_SUPPORT)
#include "drv_ao_geq.h"
#endif

#if defined(HI_SND_PEQ_SUPPORT)
#include "drv_ao_peq.h"
#endif
#endif /* HI_SND_AEF_SUPPORT */

#if defined(HI_SND_CAST_SUPPORT)
#include "drv_ao_cast.h"
#endif

#if defined(HI_SND_HDMI_SUPPORT)
#include "drv_ao_hdmi.h"
#endif

#if defined(HI_SND_SPDIF_SUPPORT)
#include "drv_ao_spdif.h"
#endif

#if defined(HI_SND_ADAC_SUPPORT)
#include "drv_ao_adac.h"
#endif

#if defined(HI_SND_I2S_SUPPORT)
#include "drv_ao_i2s.h"
#endif

#if defined(HI_SND_AR_SUPPORT)
#include "drv_ar.h"
#endif

#if defined(HI_SND_ARC_SUPPORT)
#include "drv_ao_arc.h"
#endif

#if defined(HI_PROC_SUPPORT)
#include "drv_ao_proc.h"
#endif

#include "drv_ao_track.h"

#endif  /* __DRV_AO_CFG_H__ */


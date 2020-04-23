/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of drv_dvfs_update
 */

#ifndef __DRV_DVFS_UPDATE_H__
#define __DRV_DVFS_UPDATE_H__

#include "hi_type.h"
#include "drv_dvfs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 dvfs_update(struct hisi_dvfs_info *info, hi_u32 cur_freq, hi_u32 new_freq);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __DRV_DVFS_UPDATE_H__ */

/******************************************************************************
  * Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
  * Description   :
  * Author        : Hisilicon multimedia software group
  * Create        : 2009/12/21
  * History       :
 *******************************************************************************/
#ifndef __DRV_AVPLAY_H__
#define __DRV_AVPLAY_H__

#include "hi_osal.h"
#include "drv_avplay_ioctl.h"
#include "drv_avplay_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define AVPLAY_MAX_CNT                  16

hi_s32 avplay_init(void);
hi_s32 avplay_deinit(void);
hi_s32 avplay_open(void *private_data);
hi_s32 avplay_release(void *private_data);
hi_slong avplay_ioctl(void *file, hi_u32 cmd, hi_void *arg);

hi_s32 hi_drv_avplay_create(hi_handle *avplay, const void *unique);
hi_s32 hi_drv_avplay_destroy(hi_handle avplay);
hi_s32 hi_drv_avplay_reset(hi_handle avplay, avplay_channel_type chn);
hi_s32 hi_drv_avplay_set_resource(hi_handle avplay, hi_handle handle,
    avplay_resource_type type, hi_u32 param);

hi_s32 hi_drv_avplay_wakeup(hi_handle avplay);
hi_s32 hi_drv_avplay_wait_event(hi_handle avplay, hi_u64 event_mask,
    avplay_event_type *event, hi_u64 *param, hi_s64 timeout);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

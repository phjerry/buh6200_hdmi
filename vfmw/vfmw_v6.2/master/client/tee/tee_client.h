/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __TEE_CLIENT_H__
#define __TEE_CLIENT_H__

#include "vfmw.h"

hi_s32 tee_client_init(hi_void *args, hi_u32 len);
hi_s32 tee_client_get_image(hi_s32 chan_id, vfmw_image *image);
hi_s32 tee_client_release_image(hi_s32 chan_id, const vfmw_image *image);
hi_s32 tee_client_suspend(hi_void *args);
hi_s32 tee_client_resume(hi_void *args);
hi_s32 tee_client_control(hi_s32 chan_id, vfmw_cid eCmdID, hi_void *args, hi_u32 len);
hi_s32 tee_client_exit(hi_void *args);
hi_void tee_client_open(hi_void);
hi_void tee_client_close(hi_void);

#endif

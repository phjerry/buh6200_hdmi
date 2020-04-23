/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver proc header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMITX_PROC_H__
#define __DRV_HDMITX_PROC_H__

#include "hi_type.h"

struct hisilicon_hdmi;

hi_s32 drv_hdmitx_proc_init(struct hisilicon_hdmi *hdmi);
hi_void drv_hdmitx_proc_deinit(struct hisilicon_hdmi *hdmi);
#endif

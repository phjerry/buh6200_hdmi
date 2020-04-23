/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_INTF_H__
#define __VFMW_INTF_H__

#include "vfmw_ext.h"

extern vfmw_export_fun g_vfmw_export_funcs;

hi_s32 vfmw_init(hi_void *args);
hi_s32 vfmw_exit(hi_void *args);
hi_s32 vfmw_suspend(hi_void *args);
hi_s32 vfmw_resume(hi_void *args);
hi_s32 vfmw_get_image(hi_s32 chan_id, hi_void *frame, hi_void *ext_frm_info);
hi_s32 vfmw_release_image(hi_s32 chan_id, const hi_void *frame);
hi_s32 vfmw_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 len);
hi_s32 vfmw_mod_init(hi_void);
hi_void vfmw_mod_exit(hi_void);

#endif

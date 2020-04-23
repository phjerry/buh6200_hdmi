/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __PTS_INFO_H__
#define __PTS_INFO_H__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_s32 pts_info_init(hi_void);
hi_s32 pts_info_exit(hi_void);
hi_s32 pts_info_alloc(hi_s32);
hi_s32 pts_info_free(hi_s32);
hi_s32 pts_info_reset(hi_s32);
hi_s32 pts_info_set_frm_rate(hi_s32, vfmw_pts_frmrate *);
hi_s32 pts_info_get_frm_rate(hi_s32, vfmw_pts_frmrate *);
hi_s32 pts_info_convert(hi_s32, vfmw_image *);
hi_s32 pts_info_enable_sample_frd(hi_s32);
hi_s32 pts_info_disable_sample_frd(hi_s32);
hi_s32 pts_info_read(hi_void *, hi_s32);
hi_s32 pts_info_write(cmd_str_ptr, hi_u32);

#endif



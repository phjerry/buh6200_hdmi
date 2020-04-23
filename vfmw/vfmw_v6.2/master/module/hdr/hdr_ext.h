/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __HDR_INFO_H__
#define __HDR_INFO_H__

#include "vfmw.h"
#include "vfmw_proc.h"

hi_void hdr_info_init(hi_void);
hi_void hdr_info_exit(hi_void);
hi_s32 hdr_info_convert(hi_s32, vfmw_image *);
hi_s32 hdr_info_read(hi_void *, hi_s32);
hi_s32 hdr_info_write(cmd_str_ptr, hi_u32);

#endif

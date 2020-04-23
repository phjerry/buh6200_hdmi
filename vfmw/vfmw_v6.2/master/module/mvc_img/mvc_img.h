/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#ifndef __MVC_IMG_H__
#define __MVC_IMG_H__

#include "vfmw.h"

typedef enum {
    MVC_IMG_NULL = 0,
    MVC_IMG_DISCARD,
    MVC_IMG_REQUE,
    MVC_IMG_MAX
} mvc_img_state;

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void mvc_img_set_view0_last_frame(vfmw_image *img0);
hi_s32  mvc_img_patch_two_view(vfmw_image *img0, vfmw_image *img1, mvc_img_state *img1_state);

#endif



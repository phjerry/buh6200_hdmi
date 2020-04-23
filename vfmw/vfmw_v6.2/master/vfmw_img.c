/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "dbg.h"
#include "pts_ext.h"
#ifdef VFMW_HDR_SUPPORT
#include "hdr_ext.h"
#endif

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void img_info_convert(hi_s32 chan_id, vfmw_image *image)
{
    hi_s32 ret;

#ifdef VFMW_HDR_SUPPORT
    ret = hdr_info_convert(chan_id, image);
    if (ret != HI_SUCCESS) {
        dprint(PRN_DBG, "%s convert hdr info failed!\n", __func__);
    }
#endif

    ret = pts_info_convert(chan_id, image);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "%s convert pts info failed!\n", __func__);
    }

    return;
}

hi_void img_info_init(hi_void)
{
#ifdef VFMW_HDR_SUPPORT
    hdr_info_init();
#endif
    pts_info_init();

    return;
}

hi_void img_info_exit(hi_void)
{
#ifdef VFMW_HDR_SUPPORT
    hdr_info_exit();
#endif
    pts_info_exit();

    return;
}

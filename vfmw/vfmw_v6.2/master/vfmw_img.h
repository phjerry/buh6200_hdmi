/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __IMG_INFO_H__
#define __IMG_INFO_H__

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void img_info_init(hi_void);
hi_void img_info_exit(hi_void);
hi_void img_info_convert(hi_s32 chan_id, vfmw_image *pstImage);

#endif

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __CRC_H__
#define __CRC_H__

#include "vfmw.h"
#include "vfmw_osal.h"
#include "vfmw_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CRC_OK    0
#define CRC_ERR   -1
#define CRC_TRUE  1
#define CRC_FLASE 0

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void crc_init(hi_void);
hi_void crc_exit(hi_void);
hi_s32 crc_init_calc_env(hi_void);
hi_s32 crc_exit_calc_env(hi_void);
hi_s32 crc_create_instance(hi_s32 instance_id);
hi_s32 crc_destroy_instance(hi_s32 instance_id);
hi_s32 crc_set_config(hi_s32 instance_id, vfmw_chan_cfg *config);
hi_s32 crc_put_image(hi_s32 instance_id, vfmw_image *image);
hi_s32 crc_calc_image_crc(vfmw_image *image);

#ifdef __cplusplus
}
#endif

#endif





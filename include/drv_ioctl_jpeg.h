/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg ioctl define
 */

#ifndef __DRV_IOCTL_JPEG_H__
#define __DRV_IOCTL_JPEG_H__

#include "hi_drv_module.h"
#include "drv_jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define JPGE_HARD_DECODE_START_MARK 0x0123456789ABCDEF

#define DRV_JPEG_CMD_DECODE              _IOWR(HI_ID_JPGDEC, 0, hi_jpeg_reg_info)
#define DRV_JPEG_HDEC_CMD_ALLOC_MEM      _IOWR(HI_ID_JPGDEC, 1, jpeg_hdec_mem_info)
#define DRV_JPEG_HDEC_CMD_DECODE         _IOWR(HI_ID_JPGDEC, 2, jpeg_hdec_reg_info)
#define DRV_JPEG_HDEC_CMD_FREE_MEM       _IOWR(HI_ID_JPGDEC, 3, jpeg_hdec_mem_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

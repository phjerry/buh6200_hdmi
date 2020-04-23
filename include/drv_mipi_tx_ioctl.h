/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: mipi kernel export interface definition.
* Author: Hisilicon hisecurity group
* Create: 2019-07-26
 */
#ifndef __DRV_MIPI_TX_IOCTL__
#define __DRV_MIPI_TX_IOCTL__

#include "hi_type.h"
#include "hi_drv_mipi.h"

typedef struct {
    hi_u32 cmd;
    hi_s32(*func)(hi_void *arg);
} mipi_tx_ioctl_pair;

typedef union {
    hi_mipi_output_mode output_mode;
    hi_mipi_video_mode video_mode;
    hi_u32  back_light_percent; /* need provide a commmon way to adjustment */
    hi_mipi_dsc_status dsc_status;
} mipi_tx_attr;

typedef enum  {
    OUTPUT_MODE = 0,
    VIDEO_MODE,
    BACK_LIGHT,
    DSC_STATUS,
    ATTR_TYPE_MAX
} mipi_attr_type;

typedef struct { /* need resolution or not */
    hi_drv_mipi_id    id;
    mipi_attr_type    attr_type;
    mipi_tx_attr      attr;
} mipi_attr_pair;

typedef struct {
    hi_drv_mipi_id  id;
    hi_u16          data_type;
    hi_u16          in_param_size;
    hi_u8           *in_params;
    hi_u16          out_param_size;
    hi_u8           *out_params;
} mipi_cmd_info;

#define HI_MIPI_TX_IOC_MAGIC   't'

#define HI_MIPI_TX_GET_ATTR      _IOWR(HI_MIPI_TX_IOC_MAGIC, 0x01, mipi_attr_pair)
#define HI_MIPI_TX_SET_ATTR      _IOW(HI_MIPI_TX_IOC_MAGIC, 0x02, mipi_attr_pair)
#define HI_MIPI_TX_SET_CMD       _IOWR(HI_MIPI_TX_IOC_MAGIC, 0x03, mipi_cmd_info)

hi_s32 mipi_tx_exec_cmd(hi_void *arg);
#endif

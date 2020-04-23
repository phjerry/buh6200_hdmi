/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: drv_lsadc_ioctl define
 * Author: HiSilicon multimedia software group
 * Created Date: 2014-02-08
 */

#ifndef __DRV_LSADC_IOCTL_H__
#define __DRV_LSADC_IOCTL_H__

#include "hi_type.h"


typedef struct {
    unsigned int channel_mask;          /* channel mask (bit0:channel-A, bit1:channele-B 1:enable 0:disable) */
    unsigned int active_bit;            /* active bit */
    unsigned int data_delta;            /* data delta */
    unsigned int deglitch_bypass;       /* deglitch bypass */
    unsigned int lsadc_reset;           /* lsadc reset */
    unsigned int power_down_mod;         /* power down model */
    unsigned int model_sel;             /* model sel */
    unsigned int lsadc_zero;            /* lsadc zero */
    unsigned int glitch_sample;         /* glitch sample */
    unsigned int time_scan;             /* time scan */
} hi_lsadc_param;


/********************************* Ioctl definitions ************/
#define CMD_LSADC_SET_CONFIG    _IOW(HI_ID_LSADC, 0x1, hi_lsadc_param)
#define CMD_LSADC_GET_CONFIG    _IOWR(HI_ID_LSADC, 0x2, hi_lsadc_param)
#define CMD_LSADC_GET_VALUE     _IOWR(HI_ID_LSADC, 0x3, unsigned int)


#endif /* __DRV_IR_IOCTL_H__ */

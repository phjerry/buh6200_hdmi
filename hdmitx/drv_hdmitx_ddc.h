/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver ddc helper header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMITX_DDC_H__
#define __DRV_HDMITX_DDC_H__

#include "hi_type.h"
#include "hal_hdmitx_ddc.h"

#define DDC_WRITE_MAX_BYTE_NUM 32

/* I2C Slave Address */
#define EDID_I2C_SALVE_ADDR 0xA0
#define HDCP_I2C_SALVE_ADDR 0x74
#define SCDC_I2C_SALVE_ADDR 0xA8

/* Status and Control Data Channel Structure */
#define SCDC_SINK_VERSION   0x01
#define SCDC_SOURCE_VERSION 0x02
#define SCDC_UPDATE_FLAG1   0x10
#define SCDC_UPDATE_FLAG2   0x11

#define SCDC_TMDS_CFG                   0x20
#define SCDC_TMDS_BIT_CLOCK_RATIO_BY_40 (1 << 1)
#define SCDC_TMDS_BIT_CLOCK_RATIO_BY_10 (0 << 1)
#define SCDC_SCRAMBLING_ENABLE          (1 << 0)

#define SCDC_TMDS_SRM_CFG      0x21
#define SCDC_SCRAMBLING_STATUS (1 << 0)

#define SCDC_SINK_CFG1    0x30
#define SCDC_SINK_CFG2    0x31
#define SCDC_SOU_TEST_CFG 0x35
#define SCDC_STATUS_FLAG1 0x40
#define SCDC_STATUS_FLAG2 0x41
#define SCDC_STATUS_FLAG3 0x42

hi_s32 hi_hdmi_edid_read(struct hdmi_ddc *ddc, hi_u8 *buffer, hi_u16 block, hi_u16 size);
hi_s32 hi_hdmi_scdc_read(struct hdmi_ddc *ddc, hi_u8 offset, hi_u8 *buffer, hi_u16 size);
hi_s32 hi_hdmi_scdc_write(struct hdmi_ddc *ddc, hi_u8 offset, const void *buffer, hi_u16 size);
hi_s32 hi_hdmi_hdcp_read(struct hdmi_ddc *ddc, hi_u8 offset, hi_u8 *buffer, hi_u16 size);
hi_s32 hi_hdmi_hdcp_write(struct hdmi_ddc *ddc, hi_u8 offset, hi_u8 *buffer, hi_u16 size);

static inline hi_s32 hi_hdmi_scdc_readb(struct hdmi_ddc *ddc, hi_u8 offset, hi_u8 *value)
{
    return hi_hdmi_scdc_read(ddc, offset, value, sizeof(*value));
}

static inline hi_s32 hi_hdmi_scdc_writeb(struct hdmi_ddc *ddc, hi_u8 offset, hi_u8 value)
{
    return hi_hdmi_scdc_write(ddc, offset, &value, sizeof(value));
}

hi_bool hi_hdmi_scdc_get_scrambling_status(struct hdmi_ddc *ddc);
hi_bool hi_hdmi_scdc_set_scrambling(struct hdmi_ddc *ddc, hi_bool enable);
hi_bool hi_hdmi_scdc_set_high_tmds_clock_ratio(struct hdmi_ddc *ddc, hi_bool set);
#endif


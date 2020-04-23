/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:  hdmi driver ddc module main source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>

#include "hi_osal.h"
#include "drv_hdmitx_ddc.h"
#include "drv_hdmitx_edid.h"
#include "drv_hdmitx.h"
#include "hal_hdmitx_ddc.h"

/*
 * read a block of data from EDID,starting at a given offset.
 * @ddc: ddc operation struct
 * @buffer: return location for the block to read
 * @block: block num of edid
 * @size: size of the block to read
 */
hi_s32 hi_hdmi_edid_read(struct hdmi_ddc *ddc, hi_u8 *buffer,
                         hi_u16 block, hi_u16 size)
{
    hi_u8 start = block * EDID_LENGTH;
    hi_u8 segment = block >> 1;
    hi_u8 xfers = segment ? 3 : 2; /* 3,2: msg num */
    hi_s32 ret;
    hi_s32 retry = 5; /* 5: msg transfer retry times */
    struct ddc_msg msgs[] = {
        {
            .addr = DDC_SEGMENT_ADDR,
            .flags = 0,
            .len = 1,
            .buf = &segment,
        }, {
            .addr = EDID_I2C_SALVE_ADDR,
            .flags = 0,
            .len = 1,
            .buf = &start,
        }, {
            .addr = EDID_I2C_SALVE_ADDR,
            .flags = DDC_M_RD,
            .len = size,
            .buf = buffer,
        }
    };

    if (ddc == HI_NULL || buffer == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FAILURE;
    }

    /* Avoid sending the segment addr to not upset non-compliant DDC monitors. */
    ret = hal_ddc_transfer(ddc, &msgs[3 - xfers], xfers, retry); /* 3: msg num */
    HDMI_DBG("ret========%d", ret);

    return ret;
}

/*
 * read a block of data from SCDC, starting at a given offset.
 * @ddc: ddc operation struct
 * @offset: start offset of block to read
 * @buffer: return location for the block to read
 * @size: size of the block to read
 */
hi_s32 hi_hdmi_scdc_read(struct hdmi_ddc *ddc, hi_u8 offset,
                         hi_u8 *buffer, hi_u16 size)
{
    hi_s32 ret;
    hi_s32 retry = 5; /* 5: msg transfer retry times */

    struct ddc_msg msgs[2] = { /* 2: msg num */
        {
            .addr = SCDC_I2C_SALVE_ADDR,
            .flags = 0,
            .len = 1,
            .buf = &offset,
        }, {
            .addr = SCDC_I2C_SALVE_ADDR,
            .flags = DDC_M_RD,
            .len = size,
            .buf = buffer,
        }
    };

    if (ddc == HI_NULL || buffer == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FAILURE;
    }

    ret = hal_ddc_transfer(ddc, msgs, ARRAY_SIZE(msgs), retry);

    return ret;
}

/*
 * drm_scdc_write - write a block of data to SCDC, starting at a given offset.
 * @ddc: ddc operation struct
 * @offset: start offset of block to write
 * @buffer: block of data to write
 * @size: size of the block to write
 */
hi_s32 hi_hdmi_scdc_write(struct hdmi_ddc *ddc, hi_u8 offset,
                          const void *buffer, hi_u16 size)
{
    struct ddc_msg msg = {
        .addr = SCDC_I2C_SALVE_ADDR,
        .flags = 0,
        .len = 1 + size,
        .buf = NULL,
    };
    hi_u8 data[DDC_WRITE_MAX_BYTE_NUM] = {0};
    hi_s32 retry = 5; /* 5: msg transfer retry times */
    hi_s32 ret;

    if (ddc == HI_NULL || buffer == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FAILURE;
    }

    HDMI_INFO("start scdc write\n");
    if (size > (DDC_WRITE_MAX_BYTE_NUM - 1)) {
        HDMI_INFO("%s: write size too big\n", __func__);
        return HI_FAILURE;
    }

    data[0] = offset;
    if (memcpy_s(&data[1], size, buffer, size)) {
        HDMI_ERR("memcpy_s fail.\n");
        return HI_FAILURE;
    }
    msg.buf = data;
    msg.len = size + 1;

    ret = hal_ddc_transfer(ddc, &msg, 1, retry);

    return ret;
}

/*
 * hi_hdmi_hdcp_read - read a block of data from HDCP, starting at a given offset.
 * @ddc: ddc operation struct
 * @offset: start offset of block to read
 * @buffer: return location for the block to read
 * @size: size of the block to read
 */
hi_s32 hi_hdmi_hdcp_read(struct hdmi_ddc *ddc, hi_u8 offset,
                         hi_u8 *buffer, hi_u16 size)
{
    hi_s32 ret;
    hi_s32 retry = 5;  /* 5: hdcp read retry times */

    struct ddc_msg msgs[] = {
        {
            .addr = HDCP_I2C_SALVE_ADDR,
            .flags = 0,
            .len = 1,
            .buf = &offset,
        }, {
            .addr = HDCP_I2C_SALVE_ADDR,
            .flags = DDC_M_RD,
            .len = size,
            .buf = buffer,
        }
    };

    if (ddc == HI_NULL || buffer == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FAILURE;
    }

    HDMI_DBG("start hdcp read\n");

    ret = hal_ddc_transfer(ddc, msgs, 2, retry); /* 2: msg num */

    HDMI_DBG("end hdcp read %d\n", ret);

    return ret;
}

/*
 * hi_hdmi_hdcp_write - write a block of data to HDCP, starting at a given offset.
 * @ddc: ddc operation struct
 * @offset: start offset of block to write
 * @buffer: block of data to write
 * @size: size of the block to write
 */
hi_s32 hi_hdmi_hdcp_write(struct hdmi_ddc *ddc, hi_u8 offset,
                          hi_u8 *buffer, hi_u16 size)
{
    hi_s32 ret;
    hi_s32 retry = 10; /* 10: hdcp write retry times */
    hi_u8 buf[DDC_WRITE_MAX_BYTE_NUM] = {0};

    struct ddc_msg msgs[] = {
        {
            .addr = HDCP_I2C_SALVE_ADDR,
            .flags = 0,
            .len = 1,
        }
    };

    if (ddc == HI_NULL || buffer == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FAILURE;
    }

    HDMI_DBG("start hdcp write\n");
    if (size > (DDC_WRITE_MAX_BYTE_NUM - 1)) {
        HDMI_INFO("%s: write size too big\n", __func__);
        return HI_FAILURE;
    }

    buf[0] = offset;
    if (memcpy_s(&buf[1], size, buffer, size)) {
        HDMI_ERR("memcpy_s fail.\n");
        return HI_FAILURE;
    }
    msgs[0].buf = buf;
    msgs[0].len = size + 1;

    ret = hal_ddc_transfer(ddc, msgs, 1, retry);
    HDMI_DBG("end hdcp write %d\n", ret);

    return ret;
}

hi_bool hi_hdmi_scdc_get_scrambling_status(struct hdmi_ddc *ddc)
{
    hi_u8 status;
    hi_s32 ret;

    if (ddc == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FALSE;
    }

    ret = hi_hdmi_scdc_readb(ddc, SCDC_TMDS_SRM_CFG, &status);
    if (ret < 0) {
        HDMI_ERR("Failed to read scrambling status: %d\n", ret);
        return HI_FALSE;
    }

    return status & SCDC_SCRAMBLING_STATUS;
}

hi_bool hi_hdmi_scdc_set_scrambling(struct hdmi_ddc *ddc, hi_bool enable)
{
    hi_u8 config;
    hi_s32 ret;

    if (ddc == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FALSE;
    }

    ret = hi_hdmi_scdc_readb(ddc, SCDC_TMDS_CFG, &config);
    if (ret < 0) {
        HDMI_ERR("Failed to read TMDS config: %d\n", ret);
        return HI_FALSE;
    }

    if (enable) {
        config |= SCDC_SCRAMBLING_ENABLE;
    } else {
        config &= ~SCDC_SCRAMBLING_ENABLE;
    }

    ret = hi_hdmi_scdc_writeb(ddc, SCDC_TMDS_CFG, config);
    if (ret < 0) {
        HDMI_ERR("Failed to enable scrambling: %d\n", ret);
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_bool hi_hdmi_scdc_set_high_tmds_clock_ratio(struct hdmi_ddc *ddc, hi_bool set)
{
    hi_u8 config;
    hi_s32 ret;

    if (ddc == HI_NULL) {
        HDMI_ERR("null ptr.\n");
        return HI_FALSE;
    }

    ret = hi_hdmi_scdc_readb(ddc, SCDC_TMDS_CFG, &config);
    if (ret < 0) {
        HDMI_ERR("Failed to read TMDS config: %d\n", ret);
        return HI_FALSE;
    }

    if (set) {
        config |= SCDC_TMDS_BIT_CLOCK_RATIO_BY_40;
    } else {
        config &= ~SCDC_TMDS_BIT_CLOCK_RATIO_BY_40;
    }

    ret = hi_hdmi_scdc_writeb(ddc, SCDC_TMDS_CFG, config);
    if (ret < 0) {
        HDMI_ERR("Failed to set TMDS clock ratio: %d\n", ret);
        return HI_FALSE;
    }

    /*
     * According to the hdmi spec, a source should wait minimum 1ms and maximum
     * 100ms after writing the TMDS config for clock ratio. Lets allow a
     * wait of upto 2ms here.
     */
    osal_msleep(1); /* need sleep 1ms. */
    return HI_TRUE;
}


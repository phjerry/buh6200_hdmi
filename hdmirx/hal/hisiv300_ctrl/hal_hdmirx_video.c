/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of video functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/3
 */
#include "hal_hdmirx_video.h"
#include "hal_hdmirx_video_reg.h"
#include "hal_hdmirx_decoder_reg.h"
#include "hal_hdmirx_depack_reg.h"
#include "hal_hdmirx_pwd_reg.h"

hi_u32 hal_video_get_pixel_repetition(hi_drv_hdmirx_port port)
{
    hi_u32 avi_data[8] = {0}; /* 8: avi infoframe length */
    hi_u32 replication;

    hdmirx_hal_reg_read_block(port, REG_AVIRX_WORD0, avi_data, 8); /* 8: avi infoframe length */
    replication = avi_data[2] & BIT3_0; /* 2: repetition in avi infoframe */

    return replication;
}

hi_u32 hal_video_get_pix_clk(hi_drv_hdmirx_port port)
{
    hi_u32 freq, pixe_clk;

    freq = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET3_STAT, DET_CLK3_FREQ);
    if (freq == 0) {
        pixe_clk = 0;
    } else {
        pixe_clk = 720000000 / freq; /* 720000000: original pixe clk=24*75000*4/freq (10kHz) */
    }

    return pixe_clk;
}

hdmirx_input_width hal_video_get_deep_color(hi_drv_hdmirx_port port)
{
    hi_u32 cd_value, bit_width;

    cd_value = hdmirx_hal_reg_read_fld_align(port, REG_DEEPCOLOR_INFO, TMDS_MODE);
    switch (cd_value) {
        case 4: /* cd value = 4 for 24 bits per pixel */
            bit_width = HDMIRX_INPUT_WIDTH_24;
            break;
        case 5: /* cd value = 5 for 30 bits per pixel */
            bit_width = HDMIRX_INPUT_WIDTH_30;
            break;
        case 6: /* cd value = 6 for 36 bits per pixel */
            bit_width = HDMIRX_INPUT_WIDTH_36;
            break;
        case 7: /* cd value = 7 for 48 bits per pixel */
            bit_width = HDMIRX_INPUT_WIDTH_48;
            break;
        default:
            bit_width = HDMIRX_INPUT_WIDTH_24;
            break;
    }
    return bit_width;
}

hi_bool hal_video_get_hdmi_mode(hi_drv_hdmirx_port port)
{
    hi_u32 mode;

    mode = hdmirx_hal_reg_read_fld_align(port, REG_HDMI_MODE_HW, CFG_HDMI_MODE_ST);
    if (mode) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_u32 hal_video_get_htotal(hi_drv_hdmirx_port port, hi_bool from_dsc)
{
    hi_u32 htotal = 0;

    if (from_dsc == HI_TRUE) {
        /* hdmi2.1 dsc use, complete later */
    } else {
        htotal = hdmirx_hal_reg_read_fld_align(port, REG_CFG_HTOTAL_COUNT, CFG_HTOTAL_COUNT);
    }

    return htotal;
}

hi_u32 hal_video_get_vtotal(hi_drv_hdmirx_port port, hi_bool from_dsc)
{
    hi_u32 vtotal = 0;

    if (from_dsc == HI_TRUE) {
         /* hdmi2.1 dsc use, complete later */
    } else {
        vtotal = hdmirx_hal_reg_read_fld_align(port, REG_CFG_VTOTAL_EVEN_COUNT, CFG_VTOTAL_COUNT_EVEN);
    }

    return vtotal;
}

hi_u32 hal_video_get_vtotal4_odd(hi_drv_hdmirx_port port, hi_bool from_dsc)
{
    hi_u32 vtotal_odd = 0;

    if (from_dsc == HI_TRUE) {
         /* hdmi2.1 dsc use, complete later */
    } else {
        vtotal_odd = hdmirx_hal_reg_read_fld_align(port, REG_CFG_VTOTAL_ODD_COUNT, CFG_VTOTAL_COUNT_ODD);
    }

    return vtotal_odd;
}

hi_u32 hal_video_get_vactive(hi_drv_hdmirx_port port, hi_bool from_dsc)
{
    hi_u32 vactive = 0;

    if (from_dsc == HI_TRUE) {
        /* hdmi2.1 dsc use, complete later */
    } else {
        vactive = hdmirx_hal_reg_read_fld_align(port, REG_CFG_VACTIVE_COUNT, CFG_VACTIVE_COUNT);
    }

    return vactive;
}

hi_u32 hal_video_get_hactive(hi_drv_hdmirx_port port, hi_bool from_dsc)
{
    hi_u32 vactive = 0;

    if (from_dsc == HI_TRUE) {
       /* hdmi2.1 dsc use, complete later */
    } else {
        vactive = hdmirx_hal_reg_read_fld_align(port, REG_CFG_HACTIVE_COUNT, CFG_HACTIVE_COUNT);
    }

    return vactive;
}

hi_bool hal_video_get_interlance(hi_drv_hdmirx_port port)
{
    hi_u32 interlance;

    interlance = hdmirx_hal_reg_read_fld_align(port, REG_CFG_FDET_STATUS, CFG_STATUS_INTERLACE);
    if (interlance) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_bool hal_video_get_hpol(hi_drv_hdmirx_port port)
{
    hi_u32 hpol;

    hpol = hdmirx_hal_reg_read_fld_align(port, REG_CFG_FDET_STATUS, CFG_STATUS_HSYNC);
    if (hpol) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_bool hal_video_get_vpol(hi_drv_hdmirx_port port)
{
    hi_u32 vpol;

    vpol = hdmirx_hal_reg_read_fld_align(port, REG_CFG_FDET_STATUS, CFG_STATUS_VSYNC);
    if (vpol) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_void hal_video_set_hpol(hi_drv_hdmirx_port port, hi_bool hpol)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_SYNC_POLARITY_FORCE, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_HSYNC_POLARITY_VALUE, hpol);
}

hi_void hal_video_set_vpol(hi_drv_hdmirx_port port, hi_bool vpol)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_SYNC_POLARITY_FORCE, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_VSYNC_POLARITY_VALUE, vpol);
}

hi_void hal_video_clr_pol(hi_drv_hdmirx_port port, hi_bool vpol)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_SYNC_POLARITY_FORCE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_HSYNC_POLARITY_VALUE, 0);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_SYNC_POLARITY_FORCE, HI_FALSE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_SYNC_PROCESS, CFG_VSYNC_POLARITY_VALUE, 0);
}

hi_void hal_video_set_channel_map(hi_drv_hdmirx_port port, hdmirx_color_space color_space)
{
    const hi_u32 y_channel = 0;  /* 0 : y channel */
    const hi_u32 cb_channel = 1; /* 1 : cb channel */
    const hi_u32 cr_channel = 2; /* 2 : cr channel */

    switch (color_space) {
        case HDMIRX_COLOR_SPACE_RGB:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_Y_SEL, y_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CB_SEL, cb_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CR_SEL, cr_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_422_EN, CFG_MUTE_CR, HI_FALSE);
            break;
        case HDMIRX_COLOR_SPACE_YCBCR422:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_Y_SEL, cr_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CB_SEL, cb_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CR_SEL, y_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_422_EN, CFG_MUTE_CR, HI_TRUE);
            break;
        case HDMIRX_COLOR_SPACE_YCBCR444:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_Y_SEL, cb_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CB_SEL, cr_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CR_SEL, y_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_422_EN, CFG_MUTE_CR, HI_FALSE);
            break;
        case HDMIRX_COLOR_SPACE_YCBCR420:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_Y_SEL, y_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CB_SEL, cb_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_OUTPUT_MUXING0, CFG_OUTPUT_CR_SEL, cr_channel);
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_422_EN, CFG_MUTE_CR, HI_FALSE);
            break;
        default:
            break;
    }
}

hi_void hal_video_set_mute_value(hi_drv_hdmirx_port port, hdmirx_color_space color_space)
{
    switch (color_space) {
        case HDMIRX_COLOR_SPACE_RGB:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_Y, CFG_MUTE_Y, 0x0);   /* RGB black 0,0,0 */
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CB, CFG_MUTE_CB, 0x0); /* RGB black 0,0,0 */
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CR, CFG_MUTE_CR, 0x0); /* RGB black 0,0,0 */
            break;
        case HDMIRX_COLOR_SPACE_YCBCR422:
        case HDMIRX_COLOR_SPACE_YCBCR444:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_Y, CFG_MUTE_Y, 0x0);     /* YCbCr black 0,0x800,0x800 */
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CB, CFG_MUTE_CB, 0x800); /* ycbcr black 0,0x800,0x800 */
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CR, CFG_MUTE_CR, 0x800); /* ycbcr black 0,0x800,0x800 */
            break;
        case HDMIRX_COLOR_SPACE_YCBCR420:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_Y, CFG_MUTE_Y, 0x0);     /* ycbcr420 black 0,0x800,0 */
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CB, CFG_MUTE_CB, 0x800); /* ycbcr420 black 0,0x800,0 */
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CR, CFG_MUTE_CR, 0x0);   /* ycbcr420 black 0,0x800,0 */
            break;
        default:
            break;
    }
}

hi_void hal_video_set_crc_enable(hi_drv_hdmirx_port port, hi_bool enable)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CRC_EN, CFG_CRC_EN, enable);
}

hi_u32 hal_video_get_crc_y_value(hi_drv_hdmirx_port port, hi_u32 *value, hi_u32 len)
{
    hi_u32 result_len = 4; /* 4 : total length of crc y1 result */
    hi_u32 y_result[4] = {0}; /* 4 : total length of crc y1 result */
    hi_u32 num;

    y_result[0] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_Y01, CFG_CRC_RESULT_Y0); /* 0:1st element */
    y_result[1] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_Y01, CFG_CRC_RESULT_Y1); /* 1:2st element */
    y_result[2] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_Y23, CFG_CRC_RESULT_Y2); /* 2:3st element */
    y_result[3] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_Y23, CFG_CRC_RESULT_Y3); /* 3:4st element */

    if (result_len >= len) {
        result_len = len;
    }

    for (num = 0; num < result_len; num++) {
        *value = y_result[num];
        value++;
    }

    return num;
}

hi_u32 hal_video_get_crc_cb_value(hi_drv_hdmirx_port port, hi_u32 *value, hi_u32 len)
{
    hi_u32 cb_result[4] = {0}; /* 4 : total length of crc cb result */
    hi_u32 result_len = 4; /* 4 : total length of crc cb result */
    hi_u32 num;

    cb_result[0] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CB01, CFG_CRC_RESULT_CB0); /* 0:1st element */
    cb_result[1] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CB01, CFG_CRC_RESULT_CB1); /* 1:2st element */
    cb_result[2] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CB23, CFG_CRC_RESULT_CB2); /* 2:3st element */
    cb_result[3] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CB23, CFG_CRC_RESULT_CB3); /* 3:4st element */

    if (result_len >= len) {
        result_len = len;
    }

    for (num = 0; num < result_len; num++) {
        *value = cb_result[num];
        value++;
    }

    return num;
}

hi_u32 hal_video_get_crc_cr_value(hi_drv_hdmirx_port port, hi_u32 *value, hi_u32 len)
{
    hi_u32 cr_result[4] = {0}; /* 4 : total length of crc cr result */
    hi_u32 result_len = 4;     /* 4 : total length of crc cr result */
    hi_u32 num;

    cr_result[0] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CR01, CFG_CRC_RESULT_CR0); /* 0:1st element */
    cr_result[1] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CR01, CFG_CRC_RESULT_CR1); /* 1:2st element */
    cr_result[2] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CR23, CFG_CRC_RESULT_CR2); /* 2:3st element */
    cr_result[3] = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CRC_RESULT_CR23, CFG_CRC_RESULT_CR3); /* 3:4st element */

    if (result_len >= len) {
        result_len = len;
    }

    for (num = 0; num < result_len; num++) {
        *value = cr_result[num];
        value++;
    }

    return num;
}

hi_void hal_video_set_mute_type_en(hi_drv_hdmirx_port port, hdmirx_mute_type type, hi_bool en)
{
    switch (type) {
        case HDMIRX_MUTE_TYPE_VS:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_VS, en);
            break;
        case HDMIRX_MUTE_TYPE_HS:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_HS, en);
            break;
        case HDMIRX_MUTE_TYPE_DE:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_DE, en);
            break;
        case HDMIRX_MUTE_TYPE_DATA:
            hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_EN, en);
            break;
        default:
            break;
    }
}

hi_void hal_video_set_mute_mode_en(hi_drv_hdmirx_port port, hdmirx_mute_mode mode)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_MUTE_CTRL, CFG_MUTE_MODE, mode);
}

hi_void hal_video_clr_frame_count(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_FRAME_COUNT, CFG_FRAME_COUNT, HI_TRUE);
}

hi_u32 hal_video_get_frame_count(hi_drv_hdmirx_port port)
{
    hi_u32 frame_count;

    frame_count = hdmirx_hal_reg_read_fld_align(port, REG_CFG_FRAME_COUNT, CFG_FRAME_COUNT);

    return frame_count;
}

hi_u32 hal_video_get_frame_rate(hi_drv_hdmirx_port port)
{
    hi_u32 frame_count, frame_rate;
    /* 24000000000 : Crystal clock (hz*1000) */
    const hi_u64 f_clk_txco = 24000000000;

    frame_count = hdmirx_hal_reg_read_fld_align(port, REG_CFG_FRAME_RATE_COUNT, CFG_FRAME_RATE);
    if (frame_count == 0) {
        frame_rate = 0;
    } else {
        frame_rate = f_clk_txco / frame_count;
    }

    return frame_rate;
}

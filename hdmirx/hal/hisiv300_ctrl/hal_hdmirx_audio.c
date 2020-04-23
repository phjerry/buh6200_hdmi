/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of audio functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#include "hal_hdmirx_audio.h"
#include "hal_hdmirx_audio_reg.h"
#include "hal_hdmirx_depack_reg.h"
#include "hal_hdmirx_pwd_reg.h"
#include "hal_hdmirx_comm.h"

hi_void hal_audio_clear_audio_int(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write(port, REG_CFG_AUD_INT, BIT31_0);
    return;
}

/* enable the aec caused by H/V resolution changed, Registers need confirmation */
hi_void hal_audio_set_res_change_aec_en(hi_drv_hdmirx_port port, hi_bool en)
{
}

hi_u32 hal_audio_get_audp_state(hi_drv_hdmirx_port port)
{
    hi_u32 temp;
    temp = hdmirx_hal_reg_read(port, REG_RX_AUDP_STAT);
    return temp;
}

hi_u32 hal_audio_get_cts(hi_drv_hdmirx_port port)
{
    hi_u32 temp;
    temp = hdmirx_hal_reg_read_fld_align(port, REG_CFG_CTS_HVAL, CTS_VAL_HW);
    return temp;
}

hi_u32 hal_audio_get_n(hi_drv_hdmirx_port port)
{
    hi_u32 temp;
    temp = hdmirx_hal_reg_read_fld_align(port, REG_CFG_N_HVAL, N_VAL_HW);
    return temp;
}

/* need confirmation */
static hi_u32 hal_audio_get_hw_fs(hi_drv_hdmirx_port port)
{
    hi_u32 freq, fs, mclk_sel, mclk;

    freq = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_CLK_DET_AUD_DACR_STAT, DET_CLK2_FREQ);
    if (freq == 0) {
        mclk = 0;
    } else {
        mclk = 120000000000 / freq; /* 120000000000 : original mclk 24*5000*1000*1000/freq(Hz) */
    }

    mclk_sel = hdmirx_hal_reg_read_fld_align(port, REG_CFG_MCLK_SEL, REG_MCLK4HBRA);
    if (mclk_sel == 0) {
        fs = mclk / 128; /* 128: mclk for 128*Fs */
    } else if (mclk_sel == 1) {
        fs = mclk / 256; /* 256: mclk for 256*Fs */
    } else {
        fs = 0;
    }

    return fs;
}

hi_u32 hal_audio_get_hw_sample_rate(hi_drv_hdmirx_port port)
{
    hi_u32 sample_rate;

    sample_rate = hal_audio_get_hw_fs(port);
    sample_rate = (sample_rate + 500) / 1000; /* 500: Rounded in k; 1000: sample rate freq(kHz) */

    return sample_rate;
}

hi_u32 hal_audio_get_channel_state(hi_drv_hdmirx_port port, hi_u32 *data, hi_u32 len)
{
    hi_u32 num;
    const hi_u8 max = 7; /* 7 : 7 array element maximum */
    const hi_u8 byte = 8; /* 8 : 8 bits for 1 array element */
    const hi_u8 reg = 4; /* 4 : 4 elements 1 register */

    if (len > max) {
        len = max;
    }

    for (num = 0; num < len; num++) {
        if (num < reg) {
            data[num] = hdmirx_hal_reg_read_fld_align(port, REG_RX_CHST1, BIT7_0 << (num * byte));
        } else {
            data[num] = hdmirx_hal_reg_read_fld_align(port, REG_RX_CHST2, BIT7_0 << ((num - reg) * byte));
        }
    }

    return num;
}

hi_u32 hal_audio_get_layout(hi_drv_hdmirx_port port)
{
    hi_u32 temp;
    temp = hdmirx_hal_reg_read_fld_align(port, REG_RX_AUDP_STAT, HDMI_LAYOUT);
    return temp;
}

hi_u32 hal_audio_get_intr(hi_drv_hdmirx_port port)
{
    hi_u32 temp;
    temp = hdmirx_hal_reg_read(port, REG_CFG_AUD_INT);
    return temp;
}

hi_void hal_audio_set_intr(hi_drv_hdmirx_port port, hi_u32 value)
{
    hdmirx_hal_reg_write(port, REG_CFG_AUD_INT, value);
    return;
}

hi_bool hal_audio_get_hbra_intr(hi_drv_hdmirx_port port)
{
    hi_bool flag = HI_FALSE;

    if (hdmirx_hal_reg_read_fld_align(port, REG_RX_AUDP_STAT, HDMI_HBRA_ON)) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_void hal_audio_clear_hbra_intr(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_set_bits(port, REG_DEPACK_INTR5, DEPACK_INTR5_STAT4, 1);
}

hi_bool hal_audio_is_audio_packet_got(hi_drv_hdmirx_port port)
{
    hi_bool flag = HI_FALSE;

    if (hdmirx_hal_reg_read_fld_align(port, REG_DEPACK_INTR5, DEPACK_INTR5_STAT1)) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_void hal_audio_clear_audio_packet(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_set_bits(port, REG_DEPACK_INTR5, DEPACK_INTR5_STAT1, 1);
}

hi_bool hal_audio_is_cts_got(hi_drv_hdmirx_port port)
{
    hi_bool flag = HI_FALSE;

    if (hdmirx_hal_reg_read_fld_align(port, REG_DEPACK_INTR5, DEPACK_INTR5_STAT0)) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_bool hal_audio_is_cts_stable(hi_drv_hdmirx_port port)
{
    hi_bool flag = HI_FALSE;

    if (hdmirx_hal_reg_read_fld_align(port, REG_CFG_AUD_INT, INTR_CTS_DROPPED_ERR) == HI_FALSE) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_bool hal_audio_is_fifo_stable(hi_drv_hdmirx_port port)
{
    hi_bool flag = HI_FALSE;

    if (hdmirx_hal_reg_read_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_OVERUN_AF) ||
        hdmirx_hal_reg_read_fld_align(port, REG_CFG_AUD_INT, INTR_FIFO_UNDERUN_AF) == HI_FALSE) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_void hal_audio_set_aac_mute_en(hi_drv_hdmirx_port port, hi_bool en)
{
    /*
     * clear AAC done interrupt which might be raised by previous events.
     * if the conditions for AAC done still exist, the interrupt bit
     * will be set when the exceptions are turned on.
     */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_AAC_AUDIO_MUTE, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_INT, INTR_AAC_AUDIO_UNMUTE, HI_TRUE);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_AAC_AUTO_MUTE_EN, en);
}

hi_void hal_audio_set_aac_unmute(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_AUDIO_AAC_UNMUTE, 1);
}

hi_bool hal_audio_get_aac_mute_en(hi_drv_hdmirx_port port)
{
    hi_bool flag = HI_FALSE;

    if (hdmirx_hal_reg_read_fld_align(port, REG_AAC_STATE, AAC_MUTE_STAT) == HI_TRUE) {
        flag = HI_TRUE;
    }

    return flag;
}

hi_void hal_audio_set_mclk(hi_drv_hdmirx_port port, hi_u32 code)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_MCLK_SEL, REG_MCLK4HBRA, REG_MCLK4HBRA & code);
    /* 2 : right shift 2 bits */
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_MCLK_SEL, REG_MCLK4DAC, (REG_MCLK4DAC & code) >> 2);
}

hi_void hal_audio_set_sw_cts(hi_drv_hdmirx_port port, hi_u32 value)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CTS_SVAL, REG_CTS_HW_SW_SEL, 1);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CTS_SVAL, REG_CTS_VAL_SW, value);
}

hi_void hal_audio_set_sw_n(hi_drv_hdmirx_port port, hi_u32 value)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_N_SVAL, REG_N_HW_SW_SEL, 1);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_N_SVAL, REG_N_VAL_SW, value);
}

hi_void hal_audio_clr_sw_acr(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_CTS_SVAL, REG_CTS_HW_SW_SEL, 0);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_N_SVAL, REG_N_HW_SW_SEL, 0);
}

hi_void hal_audio_set_i2s_out_cfg(hi_drv_hdmirx_port port, hi_u32 cfg)
{
    hdmirx_hal_reg_set_bits(port, REG_CFG_AUD_EN, REG_SD0_EN, cfg & REG_SD0_EN);
    hdmirx_hal_reg_set_bits(port, REG_CFG_AUD_EN, REG_SD1_EN, cfg & REG_SD1_EN);
    hdmirx_hal_reg_set_bits(port, REG_CFG_AUD_EN, REG_SD2_EN, cfg & REG_SD2_EN);
    hdmirx_hal_reg_set_bits(port, REG_CFG_AUD_EN, REG_SD3_EN, cfg & REG_SD3_EN);
}

hi_void hal_audio_set_i2s_out_dft(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AUD_EN, REG_I2S_MODE, 1);
}

hi_void hal_audio_set_acr_start(hi_drv_hdmirx_port port)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_ACR_CTRL, REG_ACR_INIT, 1);
}

hi_u32 hal_audio_get_audio_fifo_diff(hi_drv_hdmirx_port port)
{
    hi_u32 temp;
    temp = hdmirx_hal_reg_read_fld_align(port, REG_REG_FIFO_CNT, HDMI_FIFO_DIFF);
    return temp;
}

hi_void hal_audio_set_sys_mute_en(hi_drv_hdmirx_port port, hi_bool en)
{
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_AIAO_MUTESTAT, en);
    hdmirx_hal_reg_write_fld_align(port, REG_CFG_AAC, CFG_AIAO_MUTESTAT_EN, en);
}

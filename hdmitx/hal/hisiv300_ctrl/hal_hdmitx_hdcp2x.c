/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi hal level hdcp2x source file.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-15
 */

#include "drv_hdmitx.h"
#include "drv_hdmitx_ddc.h"
#include "hal_hdmitx_io.h"
#include "hal_hdmitx_hdcp2x.h"
#include "hal_hdmitx_hdcp2x_mcu.h"
#include "hal_hdmitx_ctrl_reg.h"
#include "hal_hdmitx_mcu_reg.h"
#include "hal_hdmitx_hdcp_reg.h"

#define HDCP2X_BASE_ADDR_MCU_PRAM 0x10000
#define HDCP2X_BASE_ADDR_MCU_DRAM 0x18000
#define HDCP2X_MCU_DRAM_SIZE        (4 * 1024)
#define HDCP2X_RECVID_VILIDNUM_ADDR 0xA80
#define HDCP2X_RECVID_LIST_ADDR     0xA81
#define HDCP2X_RECVID_ADDR          0xB20
#define HDCP2X_OFFSET_HDCP2XVERSION 0x50
#define DRAM_ADDR_RECVID_VALIDNUM   0xA80
#define DRAM_ADDR_RECVID_LIST       (DRAM_ADDR_RECVID_VALIDNUM + 0x1)
#define RETRY_CNT                   5
#define HDCP2X_HW_VERION            0x100
#define hdcp2x_dram_read(hdmi_reg, offset) hdmi_readl(hdmi_reg, HDCP2X_BASE_ADDR_MCU_DRAM + (offset) * 4)

hi_s32 hdcp2x_load_mcu_code(struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i, data, max_write_times;
    hi_u32 *code = g_8051_code;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return -EINVAL;
    }

    for (i = 0; i < sizeof(g_8051_code) / sizeof(g_8051_code[0]); i++) {
        for (max_write_times = RETRY_CNT, data = code[i] + RETRY_CNT;
             (data != code[i]) && max_write_times;
             max_write_times--) {
            data = code[i];
            hdmi_writel(hdmi->hdmi_regs, HDCP2X_BASE_ADDR_MCU_PRAM + i * 4, data); /* 4 is an addr len */
            data = hdmi_readl(hdmi->hdmi_regs, HDCP2X_BASE_ADDR_MCU_PRAM + i * 4); /* 4 is an addr len */
        }

        if (!max_write_times) {
            HDMI_ERR("data: %08x, g_8051_code[%d]: %08x is not equal!\n", data, i, *(code + i));
            ret = HI_FAILURE;
            break;
        }
    }

    return ret;
}

void hdcp2x_set_2x_srst(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_MCU_SRST_REQ_M,
        reg_tx_mcu_srst_req(!!enable));
}

void hdcp2x_set_mode(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP_FUN_SEL, REG_HDCP_FUN_SEL_M,
        reg_hdcp_fun_sel(1));
}

void hdcp2x_set_mcu_start(struct hisilicon_hdmi *hdmi, hi_bool start)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_AUTH_CTRL,
        REG_MCU_AUTH_STOP_M | REG_MCU_AUTH_START_M,
        reg_mcu_auth_stop(!start) | reg_mcu_auth_start(!!start));
}

void hdcp2x_set_mcu_srst(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL,
        REG_TX_MCU_SRST_REQ_M,
        reg_tx_mcu_srst_req(!!enable));
}

void hdcp2x_clear_irq(struct hisilicon_hdmi *hdmi, hi_u32 data)
{
    hi_u32 value;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    value = data;
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B0, ((value >> 0) & 0xff)); /* 0 is shift, 0xff is mask */
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B1, ((value >> 8) & 0xff)); /* 8 is shift, 0xff is mask */
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B2, ((value >> 16) & 0xff)); /* 16 is shift, 0xff is mask */
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B3, ((value >> 24) & 0xff)); /* 24 is shift, 0xff is mask */
}

void hdcp2x_set_sync_mode(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_CIPHER_CTRL,
        REG_CFG_AUTH_DONE_SYN_EN_M | REG_CFG_HDCP_MODE_SYN_EN_M,
        reg_cfg_hdcp_mode_syn_en(enable) | reg_cfg_auth_done_syn_en(enable));
}

void hdcp2x_set_encryption(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_ENC_CTRL,
        REG_HDCP2X_ENC_EN_M, reg_hdcp2x_enc_en(enable));
}

void hdcp2x_set_irq(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    hi_u32 value;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_writel(hdmi->hdmi_regs, REG_PWD_SUB_INTR_MASK, reg_hdcp2x_intr_mask(!!enable));

    value = enable ? 0xdf : 0x0; /* 0xdf is mask , 0 is enable */
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_MASK_B0, value);
    value = enable ? 0xff : 0x0; /* 0xff enable mask every bit; 0x0 disable */
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_MASK_B1, value);
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_MASK_B2, value);
    hdmi_writel(hdmi->hdmi_regs, REG_HDCP2X_MCU_MASK_B3, value);
}

void hdcp2x_get_irq_status(struct hisilicon_hdmi *hdmi, hi_u32 *data)
{
    hi_u32 value;
    union hdcp2x_irq irq_status;

    if (hdmi == NULL || data == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    value = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B0);
    irq_status.u32_data = value;
    value = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B1);
    irq_status.u32_data |= value << 8; /* 8 is shift */
    value = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B2);
    irq_status.u32_data |= value << 16; /* 16 is shift */
    value = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_MCU_INTR_B3);
    irq_status.u32_data |= value << 24; /* 24 is shift */
    *data = irq_status.u32_data;
}

void hdcp2x_set_stream_msg(struct hisilicon_hdmi *hdmi,
    struct hdcp2x_stream_manage *str_msg)
{
    if (hdmi == NULL || str_msg == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_M0, REG_HDCP2X_RPT_M0_M, reg_hdcp2x_rpt_m0(str_msg->seq_num_m0));
    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_M1, REG_HDCP2X_RPT_M1_M, reg_hdcp2x_rpt_m1(str_msg->seq_num_m1));
    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_M2, REG_HDCP2X_RPT_M2_M, reg_hdcp2x_rpt_m2(str_msg->seq_num_m2));
    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_K0, REG_HDCP2X_RPT_K0_M, reg_hdcp2x_rpt_k0(str_msg->k0));
    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_K1, REG_HDCP2X_RPT_K1_M, reg_hdcp2x_rpt_k0(str_msg->k1));
    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_STRM_ID, REG_HDCP2X_RPT_STRM_ID_M,
        reg_hdcp2x_rpt_strm_id(str_msg->stream_id));
    hdmi_clrset(hdmi->hdmi_regs, REG_HDCP2X_RPT_STRM_TPYE,
        REG_HDCP2X_RPT_STRM_TYPE_M, reg_hdcp2x_rpt_strm_type(str_msg->stream_type));
}

void hdcp2x_set_hw_version(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    HDMI_INFO("no support now\n");
}

void hdcp2x_get_sw_version(struct hisilicon_hdmi *hdmi, hi_u8 *version, hi_u32 size)
{
    if (hdmi == NULL || size != HDCP2X_SIZE_SW_VERSION) {  /*  */
        HDMI_ERR("null ptr\n");
        return ;
    }

    *version++ = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_SW_VER_B3);
    *version++ = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_SW_VER_B2);
    *version++ = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_SW_VER_B1);
    *version = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_SW_VER_B0);
}

void hdcp2x_get_rx_info(struct hisilicon_hdmi *hdmi, struct hdcp2x_rx_info *rx_info)
{
    hi_u32 value;
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    value = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_RPT_DEV);
    rx_info->depth = (value & REG_HDCP2X_RPT_DEP_CNT_M) >> 5; /* mask start in bit 5 */
    rx_info->dev_cnt = value & REG_HDCP2X_RPT_DEV_CNT_M;

    value = hdmi_readl(hdmi->hdmi_regs, REG_HDCP2X_RPT_INFO);
    rx_info->max_devs_exceeded = (value & REG_HDCP2X_RPT_DEV_EXC_M) >> 3; /* mask start in bit 3 */
    rx_info->max_cascade_exceeded = (value & REG_HDCP2X_RPT_CAS_EXC_M) >> 2; /* mask start in bit 2 */
    rx_info->max_hdcp20_down_stream = (value & REG_HDCP20_RPT_ON_M) >> 1; /* mask start in bit 1 */
    rx_info->max_hdcp1x_down_stream = value & REG_HDCP1X_DEV_ON_M;
}

void hdcp2x_get_recv_id(struct hisilicon_hdmi *hdmi, hi_u8 *data, hi_u32 size)
{
    hi_u32 i;

    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    if (size < HDCP2X_RECVID_SIZE) {
        HDMI_ERR("size=%d(<%d)\n", size, HDCP2X_RECVID_SIZE);
        return ;
    }

    for (i = 0; i < HDCP2X_RECVID_SIZE; i++) {
        data[i] = hdcp2x_dram_read(hdmi->hdmi_regs, HDCP2X_RECVID_ADDR + i);
    }
}

hi_s32 hdcp2x_get_recv_id_list(struct hisilicon_hdmi *hdmi, hi_u8 *data, hi_u32 size)
{
    hi_u32 i;
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return -1;
    }

    if (size > HDCP2X_RECVID_LIST_MAX_SIZE) {
        HDMI_ERR("size=%d(>%d)\n", size, HDCP2X_RECVID_LIST_MAX_SIZE);
        return -1;
    }

    for (i = 0; i < size; i++) {
        data[i] = hdcp2x_dram_read(hdmi->hdmi_regs, HDCP2X_RECVID_LIST_ADDR + i);
    }

    return 0;
}

void hdcp2x_get_mcu_cap(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
}

void hdcp2x_get_auth_follow(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
}

void hdcp2x_get_sink_capability(struct hisilicon_hdmi *hdmi, struct hdcp_cap *cap)
{
    hi_s32 ret;
    hi_u8 byte = 0;

    if (hdmi == NULL || cap == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    ret = hi_hdmi_hdcp_read(hdmi->ddc, 0x50, &byte, sizeof(byte));
    HDMI_INFO("ret, byte=%d,%d\n", ret, byte);
    cap->support_22 = !!byte;
}

hi_bool hdcp1x_is_ddc_idle(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return HI_FALSE;
    }
    return HI_TRUE;
}

struct hdcp2x_hal_ops g_hal_2x_ops = {
    .load_mcu_code = hdcp2x_load_mcu_code,
    .set_2x_srst = hdcp2x_set_2x_srst,
    .set_mode = hdcp2x_set_mode,
    .set_mcu_start = hdcp2x_set_mcu_start,
    .set_mcu_srst = hdcp2x_set_mcu_srst,
    .clear_irq = hdcp2x_clear_irq,
    .set_irq = hdcp2x_set_irq,
    .get_irq_status = hdcp2x_get_irq_status,
    .set_stream_msg = hdcp2x_set_stream_msg,
    .set_hw_version = hdcp2x_set_hw_version,
    .get_sw_version = hdcp2x_get_sw_version,
    .get_rx_info = hdcp2x_get_rx_info,
    .get_recv_id = hdcp2x_get_recv_id,
    .get_recv_id_list = hdcp2x_get_recv_id_list,
    .get_mcu_cap = hdcp2x_get_mcu_cap,
    .get_auth_follow = hdcp2x_get_auth_follow,
    .get_sink_capability = hdcp2x_get_sink_capability,
    .set_encryption = hdcp2x_set_encryption,
    .set_sync_mode = hdcp2x_set_sync_mode,
    .is_ddc_idle = hdcp1x_is_ddc_idle,
};

struct hdcp2x_hal_ops *hal_hdmitx_hdcp2x_get_ops(hi_void)
{
    return &g_hal_2x_ops;
}


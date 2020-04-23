/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: CEC hal.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-10-29
 */
#include "hal_hdmitx_cec.h"
#include "hi_osal.h"
#include "hal_hdmitx_cec_reg.h"
#include "hal_hdmitx_io.h"

#define HAL_CEC_TX_MSG_HEAD_OPCODE_LEN 2
#define HAL_CEC_RX_MSG_HEAD_LEN 1

static hi_void hw_enable(struct cec_hal *cec, hi_bool enable)
{
    hi_u32 reg_val;

    /* Use query method, so disable all IRQ and clear all pending IRQ */
    reg_val = hdmi_readl(cec->regs_base, REG_CEC_IRQ_SRC_PND);
    hdmi_writel(cec->regs_base, REG_CEC_IRQ_EN_SET, 0);
    hdmi_writel(cec->regs_base, REG_CEC_IRQ_SRC_PND, reg_val);

    /* Clean all transmit and receive buffer */
    hdmi_writel(cec->regs_base, REG_CEC_PULSE, REG_RX_FRAME_CLR_ALL | REG_TX1_BUFFER_FLUSH | REG_TX0_BUFFER_FLUSH);

    /* Clean all monitor error */
    reg_val = hdmi_readl(cec->regs_base, REG_CEC_UMONITOR1);
    hdmi_writel(cec->regs_base, REG_CEC_UMONITOR1, reg_val);

    /*
     * Don't capture any directed messages include message from 0xf,
     * So CEC adapter can only receive broadcast messages.
     */
    hdmi_clr(cec->regs_base, REG_CEC_CAPTURE_ID_FLAG, REG_CAPTURE_ID_FLAG_M);
    hdmi_clr(cec->regs_base, REG_CEC_CTRL, REG_BRCST_INIT_RCV_EN);

    if (enable) {
        hdmi_set(cec->regs_base, REG_CEC_CTRL, REG_CEC_TRAN_EN | REG_CEC_RCV_EN);
    } else {
        hdmi_clr(cec->regs_base, REG_CEC_CTRL, REG_CEC_TRAN_EN | REG_CEC_RCV_EN);
    }
}

static hi_void set_osd_name(struct cec_hal *cec, const hi_u8 *osd_name, hi_u32 len)
{
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_5,
        REG_CEC_SWITCH_INFO_OSD_NAME0_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4,
        REG_CEC_SWITCH_INFO_OSD_NAME1_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4,
        REG_CEC_SWITCH_INFO_OSD_NAME2_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4,
        REG_CEC_SWITCH_INFO_OSD_NAME3_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4,
        REG_CEC_SWITCH_INFO_OSD_NAME4_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3,
        REG_CEC_SWITCH_INFO_OSD_NAME5_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3,
        REG_CEC_SWITCH_INFO_OSD_NAME6_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3,
        REG_CEC_SWITCH_INFO_OSD_NAME7_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3,
        REG_CEC_SWITCH_INFO_OSD_NAME8_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2,
        REG_CEC_SWITCH_INFO_OSD_NAME9_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2,
        REG_CEC_SWITCH_INFO_OSD_NAME10_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2,
        REG_CEC_SWITCH_INFO_OSD_NAME11_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2,
        REG_CEC_SWITCH_INFO_OSD_NAME12_M, *osd_name++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1,
        REG_CEC_SWITCH_INFO_OSD_NAME13_M, *osd_name++);
}

hi_void hal_cec_reset(struct cec_hal *cec)
{
    if (cec == HI_NULL) {
        return;
    }

    hdmi_set(cec->crg_regs_base, REG_HDMITX_CRG_CLK_SRST, REG_HDMITX_CTRL_CEC_SRST_REQ);
    /* Wait at least 1ms before clear reset */
    osal_msleep(1);
    hdmi_clr(cec->crg_regs_base, REG_HDMITX_CRG_CLK_SRST, REG_HDMITX_CTRL_CEC_SRST_REQ);
}

hi_void hal_cec_enable(struct cec_hal *cec)
{
    if (cec == HI_NULL) {
        return;
    }

    hw_enable(cec, HI_TRUE);
}

hi_void hal_cec_disable(struct cec_hal *cec)
{
    if (cec == HI_NULL) {
        return;
    }

    hw_enable(cec, HI_FALSE);
}

hi_void hal_cec_set_logic_addr(struct cec_hal *cec, hi_u8 logic_addr)
{
    if (cec == HI_NULL) {
        return;
    }

    if (logic_addr == CEC_INVALID_LOGICAL_ADDR) {
        /*
         * Don't capture any directed messages include message from 0xf,
         * So CEC adapter can only receive broadcast messages.
         */
        hdmi_clr(cec->regs_base, REG_CEC_CTRL, REG_BRCST_INIT_RCV_EN);
        hdmi_clr(cec->regs_base, REG_CEC_CAPTURE_ID_FLAG, REG_CAPTURE_ID_FLAG_M);
    } else {
        /* Capture messages from logic_addr and UNREGISTERED */
        hdmi_set(cec->regs_base, REG_CEC_CTRL, REG_BRCST_INIT_RCV_EN);
        hdmi_write_bits(cec->regs_base, REG_CEC_CAPTURE_ID_FLAG, REG_CAPTURE_ID_FLAG_M, 1 << logic_addr);
    }
}

hi_void hal_cec_transmit(struct cec_hal *cec, const cec_msg *msg, hi_u32 retry_times)
{
    hi_u32 reg_val;
    hi_u8 *ptr = HI_NULL;

    if ((cec == HI_NULL) || (msg == HI_NULL)) {
        return;
    }

    /* Clean transmit buffer */
    hdmi_writel(cec->regs_base, REG_CEC_PULSE, REG_TX0_BUFFER_FLUSH);

    /* Clear transmit pending IRQ */
    reg_val = hdmi_readl(cec->regs_base, REG_CEC_IRQ_SRC_PND);
    hdmi_writel(cec->regs_base, REG_CEC_IRQ_SRC_PND, reg_val & (REG_TX0_FRAME_RETRY_OUT_SRC | REG_TX0_FRAME_RDY_SRC));

    /* Clean all monitor error */
    reg_val = hdmi_readl(cec->regs_base, REG_CEC_UMONITOR1);
    hdmi_writel(cec->regs_base, REG_CEC_UMONITOR1, reg_val);

    ptr = (hi_u8 *)&msg->data[0];
    hdmi_writel(cec->regs_base, REG_CEC_TX0_HEAD_ADDR, *ptr++);
    /* Note: Polling messages and other messages is sending by a different way */
    if (msg->len == 1) {
        /* Send polling message */
        hdmi_write_bits(cec->regs_base, REG_CEC_RETRY_NUM, REG_PING_RETRY_NUM_M, retry_times);
        hdmi_set(cec->regs_base, REG_CEC_PULSE, REG_TX0_POLL_MESSAGE_EN);
    } else {
        /* Send messages except poll message.
         * Note: For convenience, all 15 operands are written to the register,but the redundant operands
         * will ignores according to the message length.
         */
        hdmi_write_bits(cec->regs_base, REG_CEC_RETRY_NUM, REG_RETRY_NUM_M, retry_times);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_LENGTH,
            REG_TX0_FRAME_LENGTH_M, msg->len - HAL_CEC_TX_MSG_HEAD_OPCODE_LEN);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA0, REG_TX0_FRAME_DATA_BYTE0_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA0, REG_TX0_FRAME_DATA_BYTE1_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA0, REG_TX0_FRAME_DATA_BYTE2_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA0, REG_TX0_FRAME_DATA_BYTE3_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA1, REG_TX0_FRAME_DATA_BYTE4_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA1, REG_TX0_FRAME_DATA_BYTE5_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA1, REG_TX0_FRAME_DATA_BYTE6_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA1, REG_TX0_FRAME_DATA_BYTE7_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA2, REG_TX0_FRAME_DATA_BYTE8_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA2, REG_TX0_FRAME_DATA_BYTE9_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA2, REG_TX0_FRAME_DATA_BYTE10_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA2, REG_TX0_FRAME_DATA_BYTE11_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA3, REG_TX0_FRAME_DATA_BYTE12_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA3, REG_TX0_FRAME_DATA_BYTE13_M, *ptr++);
        hdmi_write_bits(cec->regs_base, REG_CEC_TX0_FRAME_DATA3, REG_TX0_FRAME_DATA_BYTE14_M, *ptr++);
        hdmi_set(cec->regs_base, REG_CEC_PULSE, REG_TX0_FRAME_EN);
    }
}

hi_s32 hal_cec_receive(struct cec_hal *cec, cec_msg *msg)
{
    hi_u32 len, i;
    hi_u32 fifo_cnt;

    if ((cec == HI_NULL) || (msg == HI_NULL)) {
        return HI_FAILURE;
    }

    fifo_cnt = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_CNT, REG_RX_FRAME_CNT_M);
    if (fifo_cnt == 0) {
        return HI_FAILURE;
    }

    len = HAL_CEC_RX_MSG_HEAD_LEN + hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_LENGTH, REG_RX_FRAME_LENGTH_M);
    if (len > CEC_MAX_MSG_SIZE) {
        len = CEC_MAX_MSG_SIZE;
    }
    msg->len = len;

    i = 0;
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_HEAD_ADDR, REG_RX_INIT_ADDR_M | REG_RX_DEST_ADDR_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA0, REG_RX_FRAME_DATA_BYTE0_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA0, REG_RX_FRAME_DATA_BYTE1_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA0, REG_RX_FRAME_DATA_BYTE2_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA0, REG_RX_FRAME_DATA_BYTE3_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA1, REG_RX_FRAME_DATA_BYTE4_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA1, REG_RX_FRAME_DATA_BYTE5_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA1, REG_RX_FRAME_DATA_BYTE6_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA1, REG_RX_FRAME_DATA_BYTE7_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA2, REG_RX_FRAME_DATA_BYTE8_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA2, REG_RX_FRAME_DATA_BYTE9_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA2, REG_RX_FRAME_DATA_BYTE10_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA2, REG_RX_FRAME_DATA_BYTE11_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA3, REG_RX_FRAME_DATA_BYTE12_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA3, REG_RX_FRAME_DATA_BYTE13_M);
    msg->data[i++] = hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_DATA3, REG_RX_FRAME_DATA_BYTE14_M);
    hdmi_set(cec->regs_base, REG_CEC_PULSE, REG_RX_FRAME_CLR);

    for (i = len; i < CEC_MAX_MSG_SIZE; i++) {
        msg->data[i] = 0;
    }

    return HI_SUCCESS;
}

hi_s32 hal_cec_get_transmit_result(struct cec_hal *cec, cec_transmit_result *result)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 reg_irq;
    hi_u32 reg_monitor = 0;

    if ((cec == HI_NULL) || (result == HI_NULL)) {
        return HI_FAILURE;
    }

    *result = CEC_TX_RESULT_MAX;

    reg_irq = hdmi_readl(cec->regs_base, REG_CEC_IRQ_SRC_PND);
    if (reg_irq & (REG_TX0_FRAME_RETRY_OUT_SRC | REG_TX0_FRAME_RDY_SRC)) {
        /* Clean transmit buffer */
        hdmi_writel(cec->regs_base, REG_CEC_PULSE, REG_TX0_BUFFER_FLUSH);
        /* Clear transmit pending IRQ */
        hdmi_writel(cec->regs_base, REG_CEC_IRQ_SRC_PND,
            reg_irq & (REG_TX0_FRAME_RETRY_OUT_SRC | REG_TX0_FRAME_RDY_SRC));
        /* Clean all monitor error */
        reg_monitor = hdmi_readl(cec->regs_base, REG_CEC_UMONITOR1);
        hdmi_writel(cec->regs_base, REG_CEC_UMONITOR1, reg_monitor);
    }

    if (reg_irq & REG_TX0_FRAME_RETRY_OUT_SRC) {
        if (reg_monitor & (REG_TX0_POLL_NO_ACK | REG_TX0_NO_ACK)) {
            *result = CEC_TX_RESULT_NACK;
            ret = HI_SUCCESS;
        } else {
            *result = CEC_TX_RESULT_FAILED;
            ret = HI_SUCCESS;
        }
    } else if (reg_irq & REG_TX0_FRAME_RDY_SRC) {
        *result = CEC_TX_RESULT_ACK;
        ret =  HI_SUCCESS;
    }

    return ret;
}

hi_void hal_cec_stop_transmit(struct cec_hal *cec)
{
    hi_u32 reg_val;

    if (cec == HI_NULL) {
        return;
    }

    /* Clean transmit buffer */
    hdmi_writel(cec->regs_base, REG_CEC_PULSE, REG_TX0_BUFFER_FLUSH);

    /* Clear transmit pending IRQ */
    reg_val = hdmi_readl(cec->regs_base, REG_CEC_IRQ_SRC_PND);
    hdmi_writel(cec->regs_base, REG_CEC_IRQ_SRC_PND, reg_val & (REG_TX0_FRAME_RETRY_OUT_SRC | REG_TX0_FRAME_RDY_SRC));

    /* Clean monitor error */
    reg_val = hdmi_readl(cec->regs_base, REG_CEC_UMONITOR1);
    hdmi_writel(cec->regs_base, REG_CEC_UMONITOR1, reg_val);
}

hi_void hal_cec_set_switch_info(struct cec_hal *cec, struct cec_switch_info *info)
{
    hi_u8 *vendor_id = HI_NULL;

    if ((cec == HI_NULL) || (info == HI_NULL)) {
        return;
    }

    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0,
        REG_CEC_SWITCH_INFO_ENABLE_M, info->enable);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0,
        REG_CEC_SWITCH_INFO_WAKEUP_ADUIO_SYSTEM_M, info->wake_audio_system);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0,
        REG_CEC_SWITCH_INFO_DEVICE_TYPE_M, info->device_type);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0,
        REG_CEC_SWITCH_INFO_LA_M, info->logical_addr);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0,
        REG_CEC_SWITCH_INFO_PA_AB_M, cec_physic_addr_ab(info->physical_addr));
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0,
        REG_CEC_SWITCH_INFO_PA_CD_M, cec_physic_addr_cd(info->physical_addr));

    vendor_id = info->vendor_id;
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1,
        REG_CEC_SWITCH_INFO_VENDOR_ID0_M, *vendor_id++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1,
        REG_CEC_SWITCH_INFO_VENDOR_ID1_M, *vendor_id++);
    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1,
        REG_CEC_SWITCH_INFO_VENDOR_ID2_M, *vendor_id++);

    set_osd_name(cec, info->osd_name, sizeof(info->osd_name));
}

hi_void hal_cec_backup_logic_addr(struct cec_hal *cec, hi_u8 logic_addr)
{
    if (cec == HI_NULL) {
        return;
    }

    hdmi_write_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_BACKUP_LA_M, logic_addr);
}

hi_void hal_cec_get_backup_logic_addr(struct cec_hal *cec, hi_u8 *logic_addr)
{
    if ((cec == HI_NULL) || (logic_addr == HI_NULL)) {
        return;
    }

    *logic_addr = hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_BACKUP_LA_M);
}


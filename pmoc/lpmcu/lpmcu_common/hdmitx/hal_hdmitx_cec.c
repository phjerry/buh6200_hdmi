/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmitx cec hal layer.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-21
 */

#include "REG51.h"
#include "base.h"
#include "hal_hdmitx_cec_reg.h"
#include "hal_hdmitx_cec.h"

#define MCU_CEC_HAL_TX_MSG_HEAD_OPCODE_LEN 2
#define MCU_CEC_HAL_RX_MSG_HEAD_LEN 1
#define SHIFT_MAX 16

static hi_u32 get_right_shift_cnt(hi_u32_data bit_mask)
{
    hi_u32 i;
    hi_u8 all_low_16bit_is_zero = 1;

    if (bit_mask.val16[0]) {
        all_low_16bit_is_zero = 0;
    }

    for (i = 0; i < SHIFT_MAX; i++) {
        if (bit_mask.val16[all_low_16bit_is_zero] & (1 << i)) {
            break;
        }
    }

    i = all_low_16bit_is_zero ? SHIFT_MAX + i : i;

    return i;
}

static inline hi_u32 hdmi_readl(hi_u32 base, hi_u32 offset)
{
    return reg_get(base + offset);
}

static hi_u32 hdmi_read_bits(hi_u32 base, hi_u32 offset, hi_u32 bit_mask)
{
    hi_u32 reg_val;
    hi_u32 right_shift_cnt;
    hi_u32_data tmp_mask;

    tmp_mask.val32 = bit_mask;
    right_shift_cnt = get_right_shift_cnt(tmp_mask);
    reg_val = hdmi_readl(base, offset);
    return (reg_val & bit_mask) >> right_shift_cnt;
}

static inline hi_void hdmi_writel(hi_u32 base, hi_u32 offset, hi_u32 value)
{
    reg_set(base + offset, value);
}

#define hdmi_clr(s, r, v)       hdmi_writel((s), (r), hdmi_readl((s), (r)) & ~(v))
#define hdmi_set(s, r, v)       hdmi_writel((s), (r), hdmi_readl((s), (r)) | (v))

static hi_void hdmi_write_bits(hi_u32 base, hi_u32 offset, hi_u32 bit_mask, hi_u32 val)
{
    hi_u32 shift_cnt;
    hi_u32_data reg_val;
    hi_u32_data tmp_mask;

    tmp_mask.val32 = bit_mask;
    shift_cnt = get_right_shift_cnt(tmp_mask);
    reg_val.val32 = hdmi_readl(base, offset);
    reg_val.val32 &= ~bit_mask;
    if (shift_cnt >= SHIFT_MAX) {
        reg_val.val16[1] |= (val << (shift_cnt - SHIFT_MAX)) & tmp_mask.val16[1];
    } else {
        reg_val.val32 |= (val << shift_cnt) & tmp_mask.val32;
    }
    hdmi_writel(base, offset, reg_val.val32);
}

static hi_void get_osd_name(struct mcu_hdmitx_cec_hal *cec, hi_u8 *osd_name, hi_u32 len)
{
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_5, REG_CEC_SWITCH_INFO_OSD_NAME0_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4, REG_CEC_SWITCH_INFO_OSD_NAME1_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4, REG_CEC_SWITCH_INFO_OSD_NAME2_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4, REG_CEC_SWITCH_INFO_OSD_NAME3_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_4, REG_CEC_SWITCH_INFO_OSD_NAME4_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3, REG_CEC_SWITCH_INFO_OSD_NAME5_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3, REG_CEC_SWITCH_INFO_OSD_NAME6_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3, REG_CEC_SWITCH_INFO_OSD_NAME7_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_3, REG_CEC_SWITCH_INFO_OSD_NAME8_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2, REG_CEC_SWITCH_INFO_OSD_NAME9_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2, REG_CEC_SWITCH_INFO_OSD_NAME10_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2, REG_CEC_SWITCH_INFO_OSD_NAME11_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_2, REG_CEC_SWITCH_INFO_OSD_NAME12_M);
    *osd_name++ =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1, REG_CEC_SWITCH_INFO_OSD_NAME13_M);
}

hi_void hal_cec_enable_irq(struct mcu_hdmitx_cec_hal *cec, enum mcu_hdmitx_cec_irq irq, hi_bool enable)
{
    hi_u8 reg_val8;

    if (cec == HI_NULL) {
        return;
    }

    if (irq == MCU_HDMITX_CEC_IRQ_MAIN) {
        reg_val8  = INT_MASK_3;
        if (enable) {
            reg_val8 |= MCU_HDMITX_CEC_IRQ_MASK;
        } else {
            reg_val8 &= ~MCU_HDMITX_CEC_IRQ_MASK;
        }
        INT_MASK_3 = reg_val8;
    } else {
        if (enable) {
            hdmi_set(cec->regs_base, REG_CEC_IRQ_EN_SET, REG_RX_BUFFER_OV_SRC_MASK | REG_RX_FRAME_RDY_SRC_MASK);
        } else {
            hdmi_clr(cec->regs_base, REG_CEC_IRQ_EN_SET, REG_RX_BUFFER_OV_SRC_MASK | REG_RX_FRAME_RDY_SRC_MASK);
        }
    }
}

hi_bool hal_cec_get_and_clear_irq(struct mcu_hdmitx_cec_hal *cec, enum mcu_hdmitx_cec_irq irq)
{
    hi_u32 reg_val;
    hi_bool interrupt = HI_FALSE;

    if (cec == HI_NULL) {
        return HI_FALSE;
    }

    if (irq == MCU_HDMITX_CEC_IRQ_MAIN) {
        interrupt = (INT_STATUS_IP3 & MCU_HDMITX_CEC_IRQ_MASK) ? HI_TRUE : HI_FALSE;
    } else {
        reg_val = hdmi_readl(cec->regs_base, REG_CEC_IRQ_SRC_PND);
        /* Clear receive pending IRQ */
        hdmi_writel(cec->regs_base, REG_CEC_IRQ_SRC_PND,
            reg_val & (REG_RX_BUFFER_OV_SRC_MASK | REG_RX_FRAME_RDY_SRC_MASK));
        interrupt = (reg_val & (REG_RX_BUFFER_OV_SRC_MASK | REG_RX_FRAME_RDY_SRC_MASK)) ? HI_TRUE : HI_FALSE;
    }

    return interrupt;
}

hi_void hal_cec_send_msg(struct mcu_hdmitx_cec_hal *cec, const struct mcu_hdmitx_cec_msg *msg,
    hi_u32 retry_times)
{
    hi_u32 reg_val;
    hi_u8 *ptr = HI_NULL;

    if ((cec == HI_NULL) || (msg == HI_NULL)) {
        return;
    }

    /* Clean transmit buffer */
    hdmi_writel(cec->regs_base, REG_CEC_PULSE, REG_TX0_BUFFER_FLUSH);

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
            REG_TX0_FRAME_LENGTH_M, msg->len - MCU_CEC_HAL_TX_MSG_HEAD_OPCODE_LEN);
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

hi_s32 hal_cec_receive_msg(struct mcu_hdmitx_cec_hal *cec, struct mcu_hdmitx_cec_msg *msg)
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

    len = MCU_CEC_HAL_RX_MSG_HEAD_LEN + hdmi_read_bits(cec->regs_base, REG_CEC_RX_FRAME_LENGTH, REG_RX_FRAME_LENGTH_M);
    if (len > MCU_HDMITX_CEC_MAX_MSG_SIZE) {
        len = MCU_HDMITX_CEC_MAX_MSG_SIZE;
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

    for (i = len; i < MCU_HDMITX_CEC_MAX_MSG_SIZE; i++) {
        msg->data[i] = 0;
    }

    return HI_SUCCESS;
}

hi_void hal_cec_get_tx_result(struct mcu_hdmitx_cec_hal *cec, enum mcu_hdmitx_cec_tx_result *result)
{
    hi_u32 reg_irq, reg_monitor;

    if ((cec == HI_NULL) || (result == HI_NULL)) {
        return;
    }

    *result = MCU_HDMITX_CEC_TX_RESULT_NONE;

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
            *result = MCU_HDMITX_CEC_TX_RESULT_NACK;
        } else {
            *result = MCU_HDMITX_CEC_TX_RESULT_FAILED;
        }
    } else if (reg_irq & REG_TX0_FRAME_RDY_SRC) {
        *result = MCU_HDMITX_CEC_TX_RESULT_ACK;
    }

    return;
}

hi_void hal_cec_get_switch_info(struct mcu_hdmitx_cec_hal *cec, struct mcu_hdmitx_cec_switch_info *info)
{
    hi_u8 ab, cd;

    if ((cec == HI_NULL) || (info == HI_NULL)) {
        return;
    }

    info->enable =
        (hi_bool)hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_SWITCH_INFO_ENABLE);
    info->wake_audio_system = (hi_bool)hdmi_read_bits(cec->sysctrl_regs_base,
        REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_SWITCH_INFO_WAKEUP_ADUIO_SYSTEM);
    info->device_type =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_SWITCH_INFO_DEVICE_TYPE_M);
    info->logical_addr =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_SWITCH_INFO_LA_M);
    ab = hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_SWITCH_INFO_PA_AB_M);
    cd = hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_0, REG_CEC_SWITCH_INFO_PA_CD_M);
    info->physical_addr = (ab << 8) | cd;

    info->vendor_id[0] =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1, REG_CEC_SWITCH_INFO_VENDOR_ID0_M);
    info->vendor_id[1] =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1, REG_CEC_SWITCH_INFO_VENDOR_ID1_M);
    info->vendor_id[2] =
        hdmi_read_bits(cec->sysctrl_regs_base, REG_SC_HDMITX_CPU_CEC_INFO_1, REG_CEC_SWITCH_INFO_VENDOR_ID2_M);

    get_osd_name(cec, info->osd_name, sizeof(info->osd_name));
}


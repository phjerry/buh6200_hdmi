/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmitx mcu cec application.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-21
 */
#include <string.h>
#include "base.h"
#include "mcu_hdmitx_cec.h"

#define MCU_CEC_RXFIFO_MAX_SZ 3
#define HEX_LEN 2

#if HI_LOG_ENABLE
static hi_void hex_to_str(hi_u32 hex, hi_char *str, hi_u8 str_size)
{
    hi_u32 i;
    hi_u8 tmp;

    if (str_size < HEX_LEN) {
        return;
    }

    for (i = 0; i < HEX_LEN; i++) {
        tmp = (hex >> (4 - (i * 4))) & 0x0F; /* distinguish the high 4 bit and low 4 bit */
        if (tmp < 10) { /* lower than 10 is a number, otherwise is a letter */
            str[i] = (tmp + 0x30);
        } else {
            str[i] = (tmp + 0x37);
        }
    }
}

hi_void cec_printf_hex(hi_u32 hex)
{
    hi_char str[HEX_LEN + 1] = {0};
    hex_to_str(hex, str, HEX_LEN);
    printf_str(str);
}
#else
#define hex_to_str(hex, str, str_size)
#define cec_printf_hex(hex)
#endif

#define mcu_cec_printf_id(id) printf_str("cec"); cec_printf_hex(id)

static hi_void show_switch_info(hi_u8 id, const struct mcu_hdmitx_cec_switch_info *info)
{
    printf_str("enable: ");
    cec_printf_hex(info->enable);
    printf_str("\r\nwake_audio_system: ");
    cec_printf_hex(info->wake_audio_system);
    printf_str("\r\ndevice_type: ");
    cec_printf_hex(info->device_type);
    printf_str("\r\nlogic_addr : ");
    cec_printf_hex(info->logical_addr);
    printf_str("\r\nphysic_addr: ");
    cec_printf_hex(mcu_hdmitx_cec_physic_addr_ab(info->physical_addr));
    cec_printf_hex(mcu_hdmitx_cec_physic_addr_cd(info->physical_addr));
    printf_str("\r\nvendor_id  : ");
    cec_printf_hex(info->vendor_id[0]);
    cec_printf_hex(info->vendor_id[1]);
    cec_printf_hex(info->vendor_id[2]);
    printf_str("\r\nosd_name   : ");
    printf_str((hi_char *)info->osd_name);
    printf_str("\r\n");
}

static hi_void show_msg(hi_u8 id, hi_bool tx, const struct mcu_hdmitx_cec_msg *msg,
    enum mcu_hdmitx_cec_tx_result result)
{
    hi_u8 i;

    mcu_cec_printf_id(id);
    if (tx) {
        printf_str(" T:");
    } else {
        printf_str(" R:");
    }

    for (i = 0; i < msg->len; i++) {
        cec_printf_hex(msg->data[i]);
        printf_char(' ');
    }

    if (tx) {
        switch (result) {
            case MCU_HDMITX_CEC_TX_RESULT_ACK:
                printf_str("(ack)\r\n");
                break;
            case MCU_HDMITX_CEC_TX_RESULT_NACK:
                printf_str("(nack)\r\n");
                break;
            case MCU_HDMITX_CEC_TX_RESULT_FAILED:
                printf_str("(failed)\r\n");
                break;
            default:
                printf_str("(sending)\r\n");
                break;
        }
    } else {
        printf_str("\r\n");
    }
}

static hi_u8 get_osd_name_len(const hi_uchar *osd_name)
{
    hi_u8 len;

    for (len = 0; len < MCU_HDMITX_CEC_OSD_NAME_SIZE; len++) {
        if (osd_name[len] == 0) {
            break;
        }
    }

    return len;
}

static hi_void queue_clear(struct mcu_hdmitx_cec_queue *queue)
{
    memset(queue, 0, sizeof(*queue));
}

static hi_void queue_init(struct mcu_hdmitx_cec_queue *queue)
{
    queue_clear(queue);
}

static hi_s32 queue_read(struct mcu_hdmitx_cec_queue *queue, struct mcu_hdmitx_cec_msg *msg,
    hi_u8 *hw_retry_times)
{
    if (queue->cnt == 0) {
        return HI_FAILURE;
    }
    memcpy(msg, &queue->msg[queue->r_ptr], sizeof(*msg));
    *hw_retry_times = queue->hw_retry_times[queue->r_ptr];
    queue->r_ptr = (queue->r_ptr + 1) % MCU_HDMITX_CEC_MAX_TX_QUEUE_SZ;
    queue->cnt--;
    return HI_SUCCESS;
}

static hi_void queue_write(struct mcu_hdmitx_cec_queue *queue, const struct mcu_hdmitx_cec_msg *msg,
    hi_u8 hw_retry_times)
{
    if (queue->cnt >= MCU_HDMITX_CEC_MAX_TX_QUEUE_SZ) {
        printf_str("CEC Tx queue full\r\n");
        return;
    }
    memcpy(&queue->msg[queue->w_ptr], msg, sizeof(*msg));
    queue->hw_retry_times[queue->w_ptr] = hw_retry_times;
    queue->w_ptr = (queue->w_ptr + 1) % MCU_HDMITX_CEC_MAX_TX_QUEUE_SZ;
    queue->cnt++;
}

static hi_void transmit_init(struct mcu_hdmitx_cec_tx_manage *tx_manage)
{
    queue_init(&tx_manage->queue);
    tx_manage->sending = HI_FALSE;
    tx_manage->sending_time = 0;
    tx_manage->hw_retry_times = MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES;
    tx_manage->app_retry_times = 0;
}

static hi_void transmit_msg(struct mcu_hdmitx_cec_tx_manage *tx_manage,
    const struct mcu_hdmitx_cec_msg *msg, hi_u8 hw_retry_times)
{
    queue_write(&tx_manage->queue, msg, hw_retry_times);
}

static hi_void transmit_cancel(struct mcu_hdmitx_cec_tx_manage *tx_manage)
{
    queue_clear(&tx_manage->queue);
    tx_manage->sending = HI_FALSE;
    tx_manage->app_retry_times = 0;
}

static hi_void rxmsg_handle(struct mcu_hdmitx_cec *cec, const struct mcu_hdmitx_cec_msg *msg)
{
    hi_u8 physic_addr_ab;
    hi_u8 physic_addr_cd;
    hi_bool need_wakeup = HI_FALSE;
    hi_bool need_response = HI_FALSE;
    struct mcu_hdmitx_cec_msg response_msg = {0};

    physic_addr_ab = mcu_hdmitx_cec_physic_addr_ab(cec->switch_info.physical_addr);
    physic_addr_cd = mcu_hdmitx_cec_physic_addr_cd(cec->switch_info.physical_addr);
    response_msg.data[0] = mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, mcu_hdmitx_cec_initiator(msg));

    switch (msg->data[1]) {
        /* opcode wakeup */
        case MCU_HDMITX_CEC_OPCODE_IMAGE_VIEW_ON:
        case MCU_HDMITX_CEC_OPCODE_TEXT_VIEW_ON:
        case MCU_HDMITX_CEC_OPCODE_PLAY:
        case MCU_HDMITX_CEC_OPCODE_DECK_CONTROL:
            need_wakeup = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_ROUTING_CHANGE:
            if ((msg->data[4] & 0xf0) == (physic_addr_ab & 0xf0)) { /* bit[7:4] of data4 is the physical address A */
                need_wakeup = HI_TRUE;
            }
            break;
        case MCU_HDMITX_CEC_OPCODE_ROUTING_INFORMATION:
        case MCU_HDMITX_CEC_OPCODE_ACTIVE_SOURCE:
        case MCU_HDMITX_CEC_OPCODE_SET_STREAM_PATH:
            if ((msg->data[2] & 0xf0) == (physic_addr_ab & 0xf0)) { /* bit[7:4] of data2 is the physical address A */
                need_wakeup = HI_TRUE;
            }
            break;
        case MCU_HDMITX_CEC_OPCODE_USER_CONTROL_PRESSED:
            if ((msg->data[2] == MCU_HDMITX_CEC_UICMD_POWER) ||
                (msg->data[2] == MCU_HDMITX_CEC_UICMD_POWER_ON_FUNCTION)) {
                need_wakeup = HI_TRUE;
            }
            break;
        /* other opcode handle */
        case MCU_HDMITX_CEC_OPCODE_GIVE_OSD_NAME:
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_SET_OSD_NAME;
            /* the length of header and opcode is 2 */
            response_msg.len = 2 + get_osd_name_len(cec->switch_info.osd_name);
            /* minus the the length of header and opcode(2) */
            memcpy(&response_msg.data[2], cec->switch_info.osd_name, response_msg.len - 2);
            need_response = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
            response_msg.data[0] =
                mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, MCU_HDMITX_CEC_LOGIC_ADDR_BROADCAST);
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_REPORT_PHYADDR;
            response_msg.data[2] = physic_addr_ab;
            response_msg.data[3] = physic_addr_cd;
            response_msg.data[4] = cec->switch_info.device_type;
            response_msg.len = 5; /* the lenght of <Report Physical Address> is 5 */
            need_response = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID:
            response_msg.data[0] =
                mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, MCU_HDMITX_CEC_LOGIC_ADDR_BROADCAST);
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_DEVICE_VENDOR_ID;
            memcpy(&response_msg.data[2], cec->switch_info.vendor_id, MCU_HDMITX_CEC_VENDOR_ID_SIZE);
            response_msg.len = 5; /* the lenght of <Device Vendor ID> is 5 */
            need_response = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_MENU_REQUEST:
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_MENU_STATUS;
            /* 0x00:active 0x01:deactive 0x02:query */
            response_msg.data[2] = 0;
            response_msg.len = 3; /* the lenght of <Menu Status> is 3 */
            need_response = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS:
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_REPORT_POWER_STATUS;
            /* 0:on 1:standby 2:standby2on 3:on2standby */
            response_msg.data[2] = 0x01;
            response_msg.len = 3; /* the lenght of <Report Power Status> is 3 */
            need_response = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_GET_MENU_LANGUAGE:
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_SET_MENU_LANGUAGE;
            response_msg.data[2] = 'c';
            response_msg.data[3] = 'h';
            response_msg.data[4] = 'i';
            response_msg.len = 5; /* the lenght of <Set Menu Language> is 5 */
            need_response = HI_TRUE;
            break;
        case MCU_HDMITX_CEC_OPCODE_GET_CEC_VERSION:
            response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_CEC_VERSION;
            response_msg.data[2] = MCU_HDMITX_CEC_VERSION_14B;
            response_msg.len = 3; /* the lenght of <CEC Version> is 3 */
            need_response = HI_TRUE;
            break;
        default:
            if ((msg->data[0] & 0xf) != MCU_HDMITX_CEC_LOGIC_ADDR_BROADCAST) {
                response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_FEATURE_ABORT;
                response_msg.data[2] = msg->data[1];
                response_msg.data[3] = 0;
                response_msg.len = 4; /* the lenght of <Feature Abort> is 4 */
                need_response = HI_TRUE;
            }
            break;
    }

    if (need_response) {
        transmit_msg(&cec->tx_manage, &response_msg, MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES);
    }

    /* if allow TV wake STB, then wakeup */
    if (need_wakeup &&
        ((cec->wakeup_mode == MCU_HDMITX_CEC_WAKEUP_BILATERAL) ||
        (cec->wakeup_mode == MCU_HDMITX_CEC_WAKEUP_TV_TO_STB))) {
        transmit_cancel(&cec->tx_manage);
        response_msg.data[0] =
            mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, MCU_HDMITX_CEC_LOGIC_ADDR_BROADCAST);
        response_msg.data[1] = MCU_HDMITX_CEC_OPCODE_ACTIVE_SOURCE;
        response_msg.data[2] = physic_addr_ab;
        response_msg.data[3] = physic_addr_cd;
        response_msg.len = 4; /* the length of <Active Source> and <System Audio Mode Request> is 4 */
        transmit_msg(&cec->tx_manage, &response_msg, MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES);
        cec->waking_up = HI_TRUE;
    }
}

static hi_void txmsg_handle(struct mcu_hdmitx_cec *cec)
{
    hi_s32 ret;
    hi_bool need_send = HI_TRUE;
    enum mcu_hdmitx_cec_tx_result result = MCU_HDMITX_CEC_TX_RESULT_NONE;

    cec->tx_manage.sending_time++;
    /* wait last sending finish */
    if (cec->tx_manage.sending) {
        hal_cec_get_tx_result(&cec->hal, &result);
        if ((result == MCU_HDMITX_CEC_TX_RESULT_NONE) &&
            (cec->tx_manage.sending_time > mcu_hdmitx_cec_retry_times_to_time(cec->tx_manage.hw_retry_times + 1))) {
            result = MCU_HDMITX_CEC_TX_RESULT_FAILED;
        }
        /* still sending, wait finish */
        if (result == MCU_HDMITX_CEC_TX_RESULT_NONE) {
            return;
        }
        /* send message finsih */
        cec->tx_manage.sending = HI_FALSE;
        show_msg(cec->id, HI_TRUE, &cec->tx_manage.sending_msg, result);
    }

    /* need retry at application layer */
    if ((cec->tx_manage.app_retry_times > 0) &&
        (result == MCU_HDMITX_CEC_TX_RESULT_FAILED)) {
        cec->tx_manage.app_retry_times--;
    } else {
        ret = queue_read(&cec->tx_manage.queue, &cec->tx_manage.sending_msg, &cec->tx_manage.hw_retry_times);
        if (ret != HI_SUCCESS) {
            /* tx queue empty, no message need to send */
            need_send = HI_FALSE;
        }

        /* if is wakeup command, retry one more times when send message failed */
        if (cec->waking_up) {
            cec->tx_manage.app_retry_times = 1; /* retry 1 times of the wakeup command */
        } else {
            cec->tx_manage.app_retry_times = 0;
        }
    }

    if (need_send) {
        hal_cec_send_msg(&cec->hal, &cec->tx_manage.sending_msg, cec->tx_manage.hw_retry_times);
        cec->tx_manage.sending = HI_TRUE;
        cec->tx_manage.sending_time = 0;
    } else {
        /* when waking up, if no message need to send that means wakeup finish */
        if (cec->waking_up) {
            cec->wakeup_finish = HI_TRUE;
        }
    }
}

hi_void mcu_hdmitx_cec_init(struct mcu_hdmitx_cec *cec, const struct mcu_hdmitx_cec_resource *resource)
{
    cec->id = resource->id;
    cec->hal.id = resource->id;
    cec->hal.regs_base = resource->regs_base;
    cec->hal.crg_regs_base = resource->crg_regs_base;
    cec->hal.sysctrl_regs_base = resource->sysctrl_regs_base;
    cec->wakeup_mode = resource->wakeup_mode;

    printf_str("------");
    mcu_cec_printf_id(cec->id);
    printf_str("------\r\n");
    printf_str("wake up mode: ");
    cec_printf_hex(resource->wakeup_mode);
    printf_str("\r\n");

    hal_cec_get_switch_info(&cec->hal, &cec->switch_info);
    show_switch_info(cec->id, &cec->switch_info);

    transmit_init(&cec->tx_manage);
    cec->waking_up = HI_FALSE;
    cec->wakeup_finish = HI_FALSE;

    if (!cec->switch_info.enable) {
        return;
    }

    hal_cec_enable_irq(&cec->hal, MCU_HDMITX_CEC_IRQ_RX, HI_TRUE);
    hal_cec_enable_irq(&cec->hal, MCU_HDMITX_CEC_IRQ_MAIN, HI_TRUE);
}

hi_void mcu_hdmitx_cec_deinit(struct mcu_hdmitx_cec *cec)
{
    if (!cec->switch_info.enable) {
        return;
    }

    hal_cec_enable_irq(&cec->hal, MCU_HDMITX_CEC_IRQ_MAIN, HI_FALSE);
    hal_cec_enable_irq(&cec->hal, MCU_HDMITX_CEC_IRQ_RX, HI_FALSE);
}

hi_void mcu_hdmitx_cec_irq_handle(struct mcu_hdmitx_cec *cec)
{
    hi_s32 i;
    hi_bool irq;
    hi_s32 ret;
    struct mcu_hdmitx_cec_msg msg = {0};

    if (!cec->switch_info.enable) {
        return;
    }

    irq = hal_cec_get_and_clear_irq(&cec->hal, MCU_HDMITX_CEC_IRQ_RX);
    if (irq) {
        for (i = 0; i <= MCU_CEC_RXFIFO_MAX_SZ; i++) {
            ret = hal_cec_receive_msg(&cec->hal, &msg);
            if (ret != HI_SUCCESS) {
                break;
            }
            show_msg(cec->id, HI_FALSE, &msg, MCU_HDMITX_CEC_TX_RESULT_NONE);
            rxmsg_handle(cec, &msg);
        }
    }
}

hi_void mcu_hdmitx_cec_task_handle(struct mcu_hdmitx_cec *cec, hi_bool wakeup_by_other,
    hi_bool *wakeup_finish)
{
    struct mcu_hdmitx_cec_msg msg = {0};

    if (!cec->switch_info.enable) {
        *wakeup_finish = wakeup_by_other;
        return;
    }

    /* wakeup by non-cec */
    if (wakeup_by_other) {
        transmit_cancel(&cec->tx_manage);
        /* if allow STB wakeup TV, then wakeup the TV */
        if ((cec->wakeup_mode == MCU_HDMITX_CEC_WAKEUP_BILATERAL) ||
            (cec->wakeup_mode == MCU_HDMITX_CEC_WAKEUP_STB_TO_TV)) {
            msg.data[0] = mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, MCU_HDMITX_CEC_LOGIC_ADDR_TV);
            msg.data[1] = MCU_HDMITX_CEC_OPCODE_IMAGE_VIEW_ON;
            msg.len = 2; /* the length of <Image View On> is 2 */
            transmit_msg(&cec->tx_manage, &msg, MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES);

            msg.data[1] = MCU_HDMITX_CEC_OPCODE_TEXT_VIEW_ON;
            transmit_msg(&cec->tx_manage, &msg, MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES);

            /* become a active source when STB wakeup */
            msg.data[0] = mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, MCU_HDMITX_CEC_LOGIC_ADDR_BROADCAST);
            msg.data[1] = MCU_HDMITX_CEC_OPCODE_ACTIVE_SOURCE;
            msg.data[2] = mcu_hdmitx_cec_physic_addr_ab(cec->switch_info.physical_addr);
            msg.data[3] =  mcu_hdmitx_cec_physic_addr_cd(cec->switch_info.physical_addr);
            msg.len = 4; /* the length of <Active Source> and <System Audio Mode Request> is 4 */
            transmit_msg(&cec->tx_manage, &msg, MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES);

            if (cec->switch_info.wake_audio_system) {
                /* need to wakeup Audio System when STB wakeup */
                msg.data[0] =
                    mcu_hdmitx_cec_msg_header(cec->switch_info.logical_addr, MCU_HDMITX_CEC_LOGIC_ADDR_AUDIOSYSTEM);
                msg.data[1] = MCU_HDMITX_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST;
                transmit_msg(&cec->tx_manage, &msg, MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES);
            }

            cec->waking_up = HI_TRUE;
        } else {
            /* don't need to wakeup TV when STB wakeup */
            cec->wakeup_finish = HI_TRUE;
        }
    }

    if (!cec->wakeup_finish) {
        txmsg_handle(cec);
    }
    *wakeup_finish = cec->wakeup_finish;
}


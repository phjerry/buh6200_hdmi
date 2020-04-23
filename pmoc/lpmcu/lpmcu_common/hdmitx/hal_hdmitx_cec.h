/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header of hdmitx cec hal layer.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-21
 */

#ifndef __HAL_HDMITX_CEC_H__
#define __HAL_HDMITX_CEC_H__

#include "hi_type.h"

#define MCU_HDMITX_CEC_IRQ_MASK       0x20  /* hdmitx_cec irq 29 */

#define MCU_HDMITX_CEC_MAX_MSG_SIZE   16
#define MCU_HDMITX_CEC_VENDOR_ID_SIZE 3
#define MCU_HDMITX_CEC_OSD_NAME_SIZE  14
#define MCU_HDMITX_CEC_DEFAULT_RETRY_TIMES 4
#define MCU_HDMITX_CEC_POLLING_RETRY_TIMES 1
/*
 * The maximum length of a message is 16 bytes, so the maximum duration of a single message is :
 * 4.5ms + 2.4*160ms = 388.5ms, which is rounded to 400ms. And an extra 100ms is for reserved.
 */
#define mcu_hdmitx_cec_retry_times_to_time(times) (10*(400*(times) + 100)) /* 0.1ms */

#define mcu_hdmitx_cec_initiator(msg)   ((msg)->data[0] >> 4)
#define mcu_hdmitx_cec_destination(msg) ((msg)->data[0] & 0xf)
#define mcu_hdmitx_cec_msg_header(initiator, destination) (((initiator) << 4) | (destination))
#define mcu_hdmitx_cec_physic_addr_ab(physic_addr) ((physic_addr) >> 8)
#define mcu_hdmitx_cec_physic_addr_cd(physic_addr) ((physic_addr) & 0xff)

enum {
    MCU_HDMITX_CEC_LOGIC_ADDR_TV          = 0x0,
    MCU_HDMITX_CEC_LOGIC_ADDR_RECORD_1    = 0x1,
    MCU_HDMITX_CEC_LOGIC_ADDR_RECORD_2    = 0x2,
    MCU_HDMITX_CEC_LOGIC_ADDR_TUNER_1     = 0x3,
    MCU_HDMITX_CEC_LOGIC_ADDR_PLAYBACK_1  = 0x4,
    MCU_HDMITX_CEC_LOGIC_ADDR_AUDIOSYSTEM = 0x5,
    MCU_HDMITX_CEC_LOGIC_ADDR_TUNER_2     = 0x6,
    MCU_HDMITX_CEC_LOGIC_ADDR_TUNER_3     = 0x7,
    MCU_HDMITX_CEC_LOGIC_ADDR_PLAYBACK_2  = 0x8,
    MCU_HDMITX_CEC_LOGIC_ADDR_RECORD_3    = 0x9,
    MCU_HDMITX_CEC_LOGIC_ADDR_TUNER_4     = 0xa,
    MCU_HDMITX_CEC_LOGIC_ADDR_PLAYBACK_3  = 0xb,
    MCU_HDMITX_CEC_LOGIC_ADDR_RESERVED_1  = 0xc,
    MCU_HDMITX_CEC_LOGIC_ADDR_RESERVED_2  = 0xd,
    MCU_HDMITX_CEC_LOGIC_ADDR_SPECIFIC    = 0xe,
    MCU_HDMITX_CEC_LOGIC_ADDR_BROADCAST   = 0xf /* as destination address */
};

enum {
    MCU_HDMITX_CEC_OPCODE_FEATURE_ABORT         = 0x00,
    MCU_HDMITX_CEC_OPCODE_IMAGE_VIEW_ON         = 0x04,
    MCU_HDMITX_CEC_OPCODE_TEXT_VIEW_ON          = 0x0d,
    MCU_HDMITX_CEC_OPCODE_SET_MENU_LANGUAGE     = 0x32,
    MCU_HDMITX_CEC_OPCODE_STANDBY               = 0x36,
    MCU_HDMITX_CEC_OPCODE_PLAY                  = 0x41,
    MCU_HDMITX_CEC_OPCODE_DECK_CONTROL          = 0x42,
    MCU_HDMITX_CEC_OPCODE_USER_CONTROL_PRESSED  = 0x44,
    MCU_HDMITX_CEC_OPCODE_GIVE_OSD_NAME         = 0x46,
    MCU_HDMITX_CEC_OPCODE_SET_OSD_NAME          = 0x47,
    MCU_HDMITX_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST = 0x70,
    MCU_HDMITX_CEC_OPCODE_ROUTING_CHANGE        = 0x80,
    MCU_HDMITX_CEC_OPCODE_ROUTING_INFORMATION   = 0x81,
    MCU_HDMITX_CEC_OPCODE_ACTIVE_SOURCE         = 0x82,
    MCU_HDMITX_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS = 0x83,
    MCU_HDMITX_CEC_OPCODE_REPORT_PHYADDR        = 0x84,
    MCU_HDMITX_CEC_OPCODE_REQUEST_ACTIVE_SOURCE = 0x85,
    MCU_HDMITX_CEC_OPCODE_SET_STREAM_PATH       = 0x86,
    MCU_HDMITX_CEC_OPCODE_DEVICE_VENDOR_ID      = 0x87,
    MCU_HDMITX_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID = 0x8c,
    MCU_HDMITX_CEC_OPCODE_MENU_REQUEST          = 0x8d,
    MCU_HDMITX_CEC_OPCODE_MENU_STATUS           = 0x8e,
    MCU_HDMITX_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS = 0x8f,
    MCU_HDMITX_CEC_OPCODE_REPORT_POWER_STATUS   = 0x90,
    MCU_HDMITX_CEC_OPCODE_GET_MENU_LANGUAGE     = 0x91,
    MCU_HDMITX_CEC_OPCODE_CEC_VERSION           = 0x9e,
    MCU_HDMITX_CEC_OPCODE_GET_CEC_VERSION       = 0x9f,
    MCU_HDMITX_CEC_OPCODE_ABORT_MESSAGE         = 0xff
};

enum {
    MCU_HDMITX_CEC_UICMD_POWER              = 0x40,
    MCU_HDMITX_CEC_UICMD_POWER_OFF_FUNCTION = 0x6c,
    MCU_HDMITX_CEC_UICMD_POWER_ON_FUNCTION  = 0x6d
};

enum mcu_hdmitx_cec_tx_result {
    MCU_HDMITX_CEC_TX_RESULT_ACK,
    MCU_HDMITX_CEC_TX_RESULT_NACK,
    MCU_HDMITX_CEC_TX_RESULT_FAILED,
    MCU_HDMITX_CEC_TX_RESULT_NONE
};

enum mcu_hdmitx_cec_irq {
    MCU_HDMITX_CEC_IRQ_MAIN,
    MCU_HDMITX_CEC_IRQ_RX,
};

struct mcu_hdmitx_cec_msg {
    hi_u8 data[MCU_HDMITX_CEC_MAX_MSG_SIZE];
    hi_u8 len;
};

struct mcu_hdmitx_cec_switch_info {
    hi_bool enable;
    hi_bool wake_audio_system;
    hi_u8   device_type;
    hi_u8   logical_addr;
    hi_u16  physical_addr;
    hi_u8   vendor_id[MCU_HDMITX_CEC_VENDOR_ID_SIZE];
    hi_u8   osd_name[MCU_HDMITX_CEC_OSD_NAME_SIZE];
};

struct mcu_hdmitx_cec_hal {
    hi_u32 regs_base;
    hi_u32 crg_regs_base;
    hi_u32 sysctrl_regs_base;
    hi_u32 id;
};

hi_void hal_cec_enable_irq(struct mcu_hdmitx_cec_hal *cec, enum mcu_hdmitx_cec_irq irq, hi_bool enable);
hi_bool hal_cec_get_and_clear_irq(struct mcu_hdmitx_cec_hal *cec, enum mcu_hdmitx_cec_irq irq);
hi_void hal_cec_send_msg(struct mcu_hdmitx_cec_hal *cec, const struct mcu_hdmitx_cec_msg *msg,
    hi_u32 retry_times);
hi_s32  hal_cec_receive_msg(struct mcu_hdmitx_cec_hal *cec, struct mcu_hdmitx_cec_msg *msg);
hi_void hal_cec_get_tx_result(struct mcu_hdmitx_cec_hal *cec, enum mcu_hdmitx_cec_tx_result *result);
hi_void hal_cec_get_switch_info(struct mcu_hdmitx_cec_hal *cec, struct mcu_hdmitx_cec_switch_info *info);

#endif


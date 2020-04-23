/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of CEC hal.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-10-29
 */

#ifndef __HAL_HDMITX_CEC_H__
#define __HAL_HDMITX_CEC_H__

#include "drv_hdmitx_ioctl.h"

#define CEC_VENDOR_ID_SIZE 3
#define CEC_OSD_NAME_SIZE  14

#define cec_initiator(msg)   ((msg)->data[0] >> 4) /* high 4bit is initiator */
#define cec_destination(msg) ((msg)->data[0] & 0xf) /* low 4bit is destination */
#define cec_msg_header(initiator, destination) (((initiator) << 4) | (destination))
#define cec_physic_addr_ab(physic_addr) ((physic_addr) >> 8)
#define cec_physic_addr_cd(physic_addr) ((physic_addr) & 0xff)

struct cec_switch_info {
    hi_bool enable;
    hi_bool wake_audio_system;
    hi_u8   device_type;
    hi_u8   logical_addr;
    hi_u16  physical_addr;
    hi_u8   vendor_id[CEC_VENDOR_ID_SIZE];
    hi_u8   osd_name[CEC_OSD_NAME_SIZE];
};

struct cec_hal {
    void __iomem *regs_base;
    void __iomem *crg_regs_base;
    void __iomem *sysctrl_regs_base;
    hi_u32 id;
};

hi_void hal_cec_reset(struct cec_hal *cec);
hi_void hal_cec_enable(struct cec_hal *cec);
hi_void hal_cec_disable(struct cec_hal *cec);
hi_void hal_cec_set_logic_addr(struct cec_hal *cec, hi_u8 logic_addr);
hi_void hal_cec_transmit(struct cec_hal *cec, const cec_msg *msg, hi_u32 retry_times);
hi_s32 hal_cec_receive(struct cec_hal *cec, cec_msg *msg);
hi_s32 hal_cec_get_transmit_result(struct cec_hal *cec, cec_transmit_result *result);
hi_void hal_cec_stop_transmit(struct cec_hal *cec);
hi_void hal_cec_set_switch_info(struct cec_hal *cec, struct cec_switch_info *info);
hi_void hal_cec_backup_logic_addr(struct cec_hal *cec, hi_u8 logic_addr);
hi_void hal_cec_get_backup_logic_addr(struct cec_hal *cec, hi_u8 *logic_addr);

#endif


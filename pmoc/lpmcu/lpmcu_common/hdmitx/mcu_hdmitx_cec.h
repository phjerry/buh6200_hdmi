/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header of hdmitx mcu cec application.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-21
 */

#ifndef __MCU_HDMITX_CEC_H__
#define __MCU_HDMITX_CEC_H__

#include "hal_hdmitx_cec.h"

#define MCU_HDMITX_CEC_VERSION_14B 5
#define MCU_HDMITX_CEC_MAX_TX_QUEUE_SZ 10

enum mcu_hdmitx_cec_wakeup_mode {
    MCU_HDMITX_CEC_WAKEUP_BILATERAL,
    MCU_HDMITX_CEC_WAKEUP_TV_TO_STB,
    MCU_HDMITX_CEC_WAKEUP_STB_TO_TV
};

struct mcu_hdmitx_cec_queue {
    hi_u8 w_ptr;
    hi_u8 r_ptr;
    hi_u8 cnt;
    struct mcu_hdmitx_cec_msg msg[MCU_HDMITX_CEC_MAX_TX_QUEUE_SZ];
    hi_u8 hw_retry_times[MCU_HDMITX_CEC_MAX_TX_QUEUE_SZ];
};

struct mcu_hdmitx_cec_tx_manage {
    struct mcu_hdmitx_cec_queue queue;
    struct mcu_hdmitx_cec_msg sending_msg;
    hi_bool sending;
    hi_u32  sending_time; /* 0.1ms */
    hi_u8   hw_retry_times;
    hi_u8   app_retry_times;
};

struct mcu_hdmitx_cec {
    hi_u32 id;
    struct mcu_hdmitx_cec_hal hal;
    struct mcu_hdmitx_cec_switch_info switch_info;
    struct mcu_hdmitx_cec_tx_manage tx_manage;
    enum mcu_hdmitx_cec_wakeup_mode wakeup_mode;
    hi_bool waking_up;
    hi_bool wakeup_finish;
};

struct mcu_hdmitx_cec_resource {
    hi_u32 regs_base;
    hi_u32 crg_regs_base;
    hi_u32 sysctrl_regs_base;
    hi_u32 id;
    enum mcu_hdmitx_cec_wakeup_mode wakeup_mode;
};

hi_void mcu_hdmitx_cec_init(struct mcu_hdmitx_cec *cec, const struct mcu_hdmitx_cec_resource *resource);
hi_void mcu_hdmitx_cec_deinit(struct mcu_hdmitx_cec *cec);
hi_void mcu_hdmitx_cec_irq_handle(struct mcu_hdmitx_cec *cec);
hi_void mcu_hdmitx_cec_task_handle(struct mcu_hdmitx_cec *cec, hi_bool wakeup_by_other,
    hi_bool *wakeup_finish);

#endif



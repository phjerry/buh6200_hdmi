/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio config definition.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_TSIO_CONFIG_H__
#define __DRV_TSIO_CONFIG_H__

#define TSIO_REGS_BASE           0x016b0000
/* 64k for REG_TSIO,4k for REG_TSIO_PHY */
#define TSIO_REGS_SIZE           0x11000
#define TSIO_IRQ_NUM             (205 + 32)

#define TSIO_PHY_IRQ_NUM         (206 + 32)

#define TSIO_TSIPORT_CNT         8
#define TSIO_RAMPORT_CNT         16
#define TSIO_PID_CHANNEL_CNT     384
#define TSIO_RAW_CHANNEL_CNT     (64 - 2)     /* TSIO_SE_CNT - ivrestart_reserved_sid - stuff_sid */
#define TSIO_SP_CHANNEL_CNT      (64 - 2)     /* TSIO_SE_CNT - ivrestart_reserved_sid - stuff_sid */
#define TSIO_IVR_CHANNEL_CNT     16           /* same as TSIO_RAMPORT_CNT */
#define TSIO_SE_CNT              64           /* abstract resource of SID */

#define TSIO_MAX_SLOT_CNT        (TSIO_TSIPORT_CNT + TSIO_RAMPORT_CNT + TSIO_PID_CHANNEL_CNT + \
                                                    TSIO_RAW_CHANNEL_CNT + TSIO_SP_CHANNEL_CNT + \
                                                    TSIO_IVR_CHANNEL_CNT + TSIO_SE_CNT + 1)

#endif  // __DRV_TSIO_CONFIG_H__

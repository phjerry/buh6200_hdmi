/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher config define.
 */

#ifndef __DRV_TSR2RCIPHER_CONFIG_H__
#define __DRV_TSR2RCIPHER_CONFIG_H__

#define TSR2RCIPHER_REGS_BASE 0x00BE0000
#define TSC_REGS_RX_BASE      0x0
#define TSC_REGS_TX_BASE      0x5000
#define TSC_REGS_TOP_BASE     0xA000
#define TSC_REGS_CIPHER_BASE  0xB000

#define DMX_TSC_REGS_MDSC_BASE 0xC000

#define TSR2RCIPHER_IRQ_NUM  (177 + 32)

#define TSR2RCIPHER_CH_CNT   32

#define TSR2RCIPHER_MAX_SLOT_CNT (TSR2RCIPHER_CH_CNT + 1)

#endif  /* __DRV_TSR2RCIPHER_CONFIG_H__ */

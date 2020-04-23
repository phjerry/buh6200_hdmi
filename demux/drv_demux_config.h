/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: demxu configuration file for HiPVRV200
 * Author: sdk
 * Create: 2009-09-27
 */

#ifndef __DRV_DEMUX_CONFIG_H__
#define __DRV_DEMUX_CONFIG_H__

#define DMX_DEFAULT_CLOCK_MHZ   324

#define DMX_REGS_BASE           0x00C00000
#define DMX_REGS_INF_BASE       0x00000
#define DMX_REGS_SWH_BASE       0x04000
#define DMX_REGS_FLT_BASE       0x0A000
#define DMX_REGS_SCD_BASE       0x0C000
#define DMX_REGS_CFG_BASE       0x0E000
#define DMX_REGS_PAR_BASE       0x10000
#define DMX_REGS_DAV_BASE       0x20000

#define DMX_REGS_MDSC_BASE      0x00BEC000

#define DMX_IRQ_NUM            (174 + 32)

/* interral resource */
#define DMX_AVR_CNT                      64
#define DMX_WHOLE_TS_CHAN_CNT           128
#define DMX_PES_SEC_CHAN_CNT            256
#define DMX_SCD_CHAN_CNT                128
#define DMX_PIDCOPY_CHAN_CNT             32
#define DMX_BUF_CNT                     512
#define DMX_PLAY_FCT_BUF_NUM             64
#define DMX_MAX_PLAY_INDEX_NUM           64
#define DMX_MAX_REC_INDEX_NUM           256
#define DMX_FLT_CNT_PER_PLAYFCT          32

#define DMX_TAG_DUAL_CNT                2
#define DMX_TAG_OUTPUT_CNT              8
/* static resource */
#define DMX_IF_PORT_CNT                 0
#define DMX_TSI_PORT_CNT               16
#define DMX_TSO_PORT_CNT                2
#define DMX_TAG_PORT_CNT                (DMX_TAG_OUTPUT_CNT * DMX_TAG_DUAL_CNT)
#define DMX_RMX_PORT_CNT                1
#define DMX_TSIO_PORT_CNT               8

/* object */
#define DMX_RAM_PORT_CNT               16
#define DMX_RMXPUMP_CNT                64
#define DMX_RMXFCT_CNT                 DMX_RMX_PORT_CNT
#define DMX_RMXPUMP_TOTALCNT           (DMX_RMXPUMP_CNT * DMX_RMXFCT_CNT)

#define DMX_PID_CHANNEL_CNT          1024
#define DMX_PLAY_CNT                  128
#define DMX_REC_CNT                    16
#define DMX_DSC_CNT                   256
#define DMX_PCR_CNT                    32
/* Logic supports 32 bands, but only 16 bands are open to users */
#define DMX_BAND_CNT                   16
#define DMX_BAND_TOTAL_CNT             32
#define DMX_FLT_CNT                   512

#define DMX_AVPES_CHAN_CNT             (DMX_AVR_CNT - DMX_REC_CNT)

#define DMX_MAX_SLOT_CNT        (DMX_RAM_PORT_CNT + DMX_RMXFCT_CNT + DMX_RMXPUMP_TOTALCNT + DMX_PID_CHANNEL_CNT \
                               + DMX_PLAY_CNT + DMX_REC_CNT + DMX_DSC_CNT + DMX_PCR_CNT \
                               + DMX_BAND_TOTAL_CNT + DMX_FLT_CNT + 1)

/* control */
#define DMX_REC_BUF_GAP_EXIST           1  /* recording buffer gap exists only in 96cv300 es slice */

#endif  // __DRV_DEMUX_CONFIG_H__

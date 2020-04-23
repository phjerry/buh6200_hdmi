/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description:lsadc_priv define
 * Author: DPT_BSP
 * Create: 2011-11-29
 */

#ifndef _LSADC_PRIV_H_
#define _LSADC_PRIV_H_

#include "hi_osal.h"
#include "hi_debug.h"

#define LSADC_REG_BASE  0x00855000
#define LSADC_CTRL0     0x00
#define LSADC_CTRL1     0x04
#define LSADC_CTRL2     0x08
#define LSADC_CTRL3     0x0c
#define LSADC_CTRL4     0x10
#define LSADC_CTRL5     0x14
#define LSADC_CTRL6     0x18
#define LSADC_CTRL7     0x1c
#define LSADC_CTRL8     0x20
#define LSADC_IRQ_NUM   (54 + 32)
#define ACTIVE_6BIT     (0xFC << 24)
#define ACTIVE_5BIT     (0xF8 << 24)
#define CLEAR_INT_FLAG  0xF
#define INT_ENABLE      0x1
#define LSADC_START     0x1
#define LSADC_STOP      0x1
#define INT_FLAG_IN_CHA 0x1
/* --FIXME-- default setting */
#define LSADC_CONFIG_DEFAULT         (0xf061 << 8)
#define LSADC_ONESHOT_CONFIG_DEFAULT (0xf041 << 8)
#define DEFAULT_GLITCH_SAMPLE        0x30
#define DEFAULT_TIME_SCAN            0x2000
#define TEST_TIME_SCAN               0xbb8
#define LSADC_DEVICE_NAME_SIZE       64
#define UMAP_DEVNAME_LSADC           "hi_lsadc"
#define UMAP_MIN_MINOR_LSADC         0
#define LSADC_DEVICE_NAME            "lsadc"

#define HI_LSADC_SPACE_SIZE 0x30

struct lsadc_priv {
    void* base;
    int irq;
    char dev_name[LSADC_DEVICE_NAME_SIZE];
    osal_semaphore sem;
};

#define HI_FATAL_LSADC(fmt...)   HI_FATAL_PRINT(HI_ID_LSADC, fmt)
#define HI_ERR_LSADC(fmt...)     HI_ERR_PRINT(HI_ID_LSADC, fmt)
#define HI_WARN_LSADC(fmt...)    HI_WARN_PRINT(HI_ID_LSADC, fmt)
#define HI_INFO_LSADC(fmt...)    HI_INFO_PRINT(HI_ID_LSADC, fmt)

#endif

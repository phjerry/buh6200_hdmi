/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver ddc header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_DDC_H__
#define __HAL_HDMITX_DDC_H__
#include "hi_osal.h"

struct hisilicon_hdmi;

#define DDC_MAX_FIFO_SIZE 16
#define DDC_SEGMENT_ADDR  0x30
#define DDC_M_RD 0x0001  /* read data, from slave to master */

/* DDC default timeout */
#define DDC_DEFAULT_TIMEOUT_ACCESS  100
#define DDC_DEFAULT_TIMEOUT_HPD     100
#define DDC_DEFAULT_TIMEOUT_IN_PROG 20
#define DDC_DEFAULT_TIMEOUT_SCL     1
#define DDC_DEFAULT_TIMEOUT_SDA     30
#define DDC_DEFAULT_TIMEOUT_ISSUE   20

enum ddc_master_access {
    DDC_MASTER_DISABLE = 0,
    DDC_MASTER_ENABLE = 1
};

enum ddc_issue_cmd {
    CMD_READ_SINGLE_NO_ACK = 0x00,  /* 0'b0000: */
    CMD_READ_SINGLE_ACK = 0x01,     /* 0'b0001: */
    CMD_READ_MUTI_NO_ACK = 0x02,    /* 4'b0010: */
    CMD_READ_MUTI_ACK = 0x03,       /* 4'b0011: */
    CMD_READ_SEGMENT_NO_ACK = 0x04, /* 4'b0100: */
    CMD_READ_SEGMENT_ACK = 0x05,    /* 4'b0101: */
    CMD_WRITE_MUTI_NO_ACK = 0x06,   /* 4'b0110: */
    CMD_WRITE_MUTI_ACK = 0x07,      /* 4'b0111: */
    CMD_FIFO_CLR = 0x09,            /* 4'b1001: */
    CMD_SCL_DRV = 0x0a,             /* 4'b1010: */
    CMD_MASTER_ABORT = 0x0f         /* 4'b1111: */
};

enum ddc_issue_mode {
    MODE_READ_SINGLE_NO_ACK,  /* 0b0000 */
    MODE_READ_SINGLE_ACK,     /* 0b0001 */
    MODE_READ_MUTIL_NO_ACK,   /* 0b0010 */
    MODE_READ_MUTIL_ACK,      /* 0b0011 */
    MODE_READ_SEGMENT_NO_ACK, /* 0b0100 */
    MODE_READ_SEGMENT_ACK,    /* 0b0101 */
    MODE_WRITE_MUTIL_NO_ACK,  /* 0b0110 */
    MODE_WRITE_MUTIL_ACK,     /* 0b0111 */
    MODE_MAX
};

struct ddc_timeout {
    hi_u32 access_timeout;  /* access_timeout */
    hi_u32 hpd_timeout;     /* hpd_timeout */
    hi_u32 in_prog_timeout; /* in_prog_timeout */
    hi_u32 scl_timeout;     /* scl_timeout */
    hi_u32 sda_timeout;     /* sda_timeout */
    hi_u32 issue_timeout;   /* issue_timeout */
};

struct hdmi_ddc {
    struct hisilicon_hdmi *parent;
    hi_u8 slave_reg;            /* slave register offset */
    hi_u8 slave_addr;           /* ddc slave address */
    hi_u8 reg_addr;             /* ddc register address */
    hi_bool is_segment;         /* is segment read */
    hi_bool is_regaddr;         /* is register address */
    hi_u8 xfer_mode;            /* work mode specified for hisilicon ddc */
    osal_mutex lock;            /* For ddc operation. */
    struct ddc_timeout timeout; /* for save ddc timeout */
};

struct ddc_msg {
    hi_u16 addr;  /* slave address */
    hi_u16 flags; /* operation mask */
    hi_u16 len;   /* msg length */
    hi_u8 *buf;   /* pointer to msg data */
};

hi_s32 hal_ddc_init(struct hisilicon_hdmi *hdmi);
void hal_ddc_deinit(struct hisilicon_hdmi *hdmi);
hi_s32 hal_ddc_transfer(struct hdmi_ddc *ddc, struct ddc_msg *msgs, hi_u8 num, hi_u8 retry);
#endif


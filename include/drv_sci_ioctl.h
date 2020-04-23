/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: The header of sci ioctl driver
 */

#ifndef __DRV_SCI_IOCTL_H__
#define __DRV_SCI_IOCTL_H__

#include "hi_drv_sci.h"

typedef enum {
    SCI_SECURE_MODE_NONE = 0,        /* no security protection */         /* CNcomment:无安全保护 */
    SCI_SECURE_MODE_TEE,             /* trustedzone security protection */ /* CNcomment:trustedzone安全保护 */
    SCI_SECURE_MODE_MAX
} sci_security;

typedef struct {
    hi_sci_port        sci_port;
    hi_sci_protocol    sci_protocol;
    hi_u32             frequency;
} sci_open_para;

typedef struct {
    hi_sci_port        sci_port;
    sci_security       security_mode;
}sci_security_mode;

typedef struct {
    hi_sci_port          sci_port;
    hi_bool              warm_reset;
} sci_reset_para;

typedef struct {
    hi_sci_port         sci_port;
    hi_u8               *patr_buf;
    hi_u32              buf_size;
    hi_u8               data_len;
} sci_atr;

typedef struct {
    hi_sci_port         sci_port;
    hi_u32              patr_buf;
    hi_u32              buf_size;
    hi_u8               data_len;
} sci_atr_compat;

typedef struct {
    hi_sci_port           sci_port;
    hi_sci_status         sci_status;
} sci_status;

typedef struct {
    hi_sci_port           sci_port;
    hi_u8                 *pdata_buf;
    hi_u32                buf_size;
    hi_u32                data_len;
    hi_u32                timeout_ms;
} sci_data;

typedef struct {
    hi_sci_port           sci_port;
    hi_u32                pdata_buf;
    hi_u32                buf_size;
    hi_u32                data_len;
    hi_u32                timeout_ms;
} sci_data_compat;

typedef struct {
    hi_sci_port           sci_port;
    hi_sci_level          sci_level;
} sci_level;

typedef enum {
    SCI_IO_CLK,
    SCI_IO_RESET,
    SCI_IO_VCC_EN,
    SCI_IO_MAX
} sci_io;

typedef struct {
    hi_sci_port           sci_port;
    sci_io                en_io;
    hi_sci_mode           output_type;
} sci_io_output_type;

typedef struct {
    hi_bool sci[HI_SCI_PORT_MAX];
} sci_dev_state;

typedef struct {
    hi_sci_port       sci_port;
    hi_u32            clk_rate;
    hi_u32            bit_rate;
} sci_ext_baud;

typedef struct {
    hi_sci_port       sci_port;
    hi_u32            add_char_guard;
} sci_add_guard;

typedef struct {
    hi_sci_port       sci_port;
    hi_u8             send[SCI_PPS_SEND_RECV_BUF_LEN];
    hi_u8             receive[SCI_PPS_SEND_RECV_BUF_LEN];
    hi_u32            send_len;
    hi_u32            receive_len;
    hi_u32            rec_timeouts;
} sci_pps;

typedef struct {
    hi_sci_port           sci_port;
    hi_sci_protocol       sci_protocol;
    hi_u32                char_timeouts;
} sci_char_timeout;

typedef struct {
    hi_sci_port       sci_port;
    hi_u32            block_timeouts;
} sci_block_timeout;

typedef struct {
    hi_sci_port       sci_port;
    hi_u32            tx_retry_times;
} sci_txretry;

#define CMD_SCI_OPEN _IOW(HI_ID_SCI, 0x1, sci_open_para)
#define CMD_SCI_CLOSE _IOW(HI_ID_SCI, 0x2, hi_sci_port)
#define CMD_SCI_RESET _IOW(HI_ID_SCI, 0x3, sci_reset_para)
#define CMD_SCI_DEACTIVE _IOW(HI_ID_SCI, 0x4, hi_sci_port)
#define CMD_SCI_GET_ATR _IOWR(HI_ID_SCI, 0x5, sci_atr)
#define CMD_SCI_COMPAT_GET_ATR _IOWR(HI_ID_SCI, 0x5, sci_atr_compat)
#define CMD_SCI_GET_STATUS _IOWR(HI_ID_SCI, 0x6, sci_status)
#define CMD_SCI_CONF_VCC _IOW(HI_ID_SCI, 0x7, sci_level)
#define CMD_SCI_CONF_DETECT _IOW(HI_ID_SCI, 0x8, sci_level)
#define CMD_SCI_CONF_MODE _IOW(HI_ID_SCI, 0x9, sci_io_output_type)
#define CMD_SCI_SEND_DATA _IOWR(HI_ID_SCI, 0xa, sci_data)
#define CMD_SCI_COMPAT_SEND_DATA _IOWR(HI_ID_SCI, 0xa, sci_data_compat)
#define CMD_SCI_RECEIVE_DATA _IOWR(HI_ID_SCI, 0xb, sci_data)
#define CMD_SCI_COMPAT_RECEIVE_DATA _IOWR(HI_ID_SCI, 0xb, sci_data_compat)
#define CMD_SCI_SWITCH _IOW(HI_ID_SCI, 0xc, sci_open_para)
#define CMD_SCI_SET_BAUD _IOW(HI_ID_SCI, 0xd, sci_ext_baud)
#define CMD_SCI_SET_CHGUARD _IOW(HI_ID_SCI, 0xe, sci_add_guard)
#define CMD_SCI_SEND_PPS_DATA _IOW(HI_ID_SCI, 0xF, sci_pps)
#define CMD_SCI_GET_PPS_DATA _IOWR(HI_ID_SCI, 0x10, sci_pps)
#define CMD_SCI_GET_PARAM _IOWR(HI_ID_SCI, 0x11, hi_unf_sci_params)
#define CMD_SCI_SET_CHARTIMEOUT _IOW(HI_ID_SCI, 0x12, sci_char_timeout)
#define CMD_SCI_SET_BLOCKTIMEOUT _IOW(HI_ID_SCI, 0x13, sci_block_timeout)
#define CMD_SCI_SET_TXRETRY _IOW(HI_ID_SCI, 0x14, sci_txretry)
#define CMD_SCI_SET_SECURITY_MODE _IOWR(HI_ID_SCI, 0x15, sci_security_mode)

#endif /*End of #ifndef __DRV_SCI_IOCTL_H__*/

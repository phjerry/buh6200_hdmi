/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: The header of sci driver
 */

#ifndef __HI_DRV_SCI_H__
#define __HI_DRV_SCI_H__

#include "hi_unf_sci.h"

#include "hi_debug.h"
#include "hi_module.h"

#define HI_FATAL_SCI(fmt...)    HI_FATAL_PRINT(HI_ID_SCI, fmt)
#define HI_ERR_SCI(fmt...)      HI_ERR_PRINT(HI_ID_SCI, fmt)
#define HI_WARN_SCI(fmt...)     HI_WARN_PRINT(HI_ID_SCI, fmt)
#define HI_INFO_SCI(fmt...)     HI_INFO_PRINT(HI_ID_SCI, fmt)

#define HI_SCI_PORT_NUM     (2)

#define SCI_ATR_BUF_LEN           256
#define SCI_PPS_SEND_RECV_BUF_LEN 6

typedef struct {
    hi_u8    data_buf[SCI_ATR_BUF_LEN];
    hi_u32   data_len;
} hi_drv_atr_buf;

typedef enum {
    HI_SCI_PORT0,
    HI_SCI_PORT1,
    HI_SCI_PORT_MAX
} hi_sci_port;

typedef enum {
    HI_SCI_PROTOCOL_T0,
    HI_SCI_PROTOCOL_T1,
    HI_SCI_PROTOCOL_T14,
    HI_SCI_PROTOCOL_MAX
} hi_sci_protocol;

typedef enum {
    HI_SCI_LEVEL_LOW,
    HI_SCI_LEVEL_HIGH,
    HI_SCI_LEVEL_MAX
} hi_sci_level;

typedef enum {
    HI_SCI_MODE_CMOS = 0,
    HI_SCI_MODE_OD,
    HI_SCI_MODE_MAX
} hi_sci_mode;

typedef enum {
    HI_SCI_STATUS_UNINIT = 0,   /* The SCI card is not initialized.(Reserved status) */
    HI_SCI_STATUS_FIRSTINIT,    /* The SCI card is being initialized.(Reserved status) */
    HI_SCI_STATUS_NOCARD,       /* There is no SCI card. */
    HI_SCI_STATUS_INACTIVECARD, /* The SCI card is not activated (unavailable). */
    HI_SCI_STATUS_WAITATR,      /* The SCI card is waiting for the ATR data. */
    HI_SCI_STATUS_READATR,      /* The SCI card is receiving the ATR data. */
    HI_SCI_STATUS_ATR_ERROR,    /* The ATR is wrong. */
    HI_SCI_STATUS_READY,        /* The SCI card is available (activated). */
    HI_SCI_STATUS_RX,           /* The SCI card is busy receiving data. */
    HI_SCI_STATUS_TX            /* The SCI card is busy transmitting data. */
} hi_sci_status;

typedef struct {
    hi_sci_port     sci_port;        /* SCI port ID */
    hi_sci_protocol protocol_type;   /* Used protocol type */
    hi_u32          actal_clk_rate;  /* Actual clock rate conversion factor F */
    hi_u32          actal_bit_rate;  /* Actual bit rate conversion factor D */
    hi_u32          fi;              /* Clk factor returned by answer to reset (ATR) */
    hi_u32          di;              /* Bit rate factor returned by the ATR */
    hi_u32          guard_delay;     /* Extra guard time N */
    hi_u32          char_timeouts;   /* Character timeout of T0 or T1 */
    hi_u32          block_timeouts;  /* Block timeout of T1 */
    hi_u32          tx_retries;      /* Number of transmission retries */
} hi_sci_params, *hi_sci_params_ptr;

hi_s32 hi_drv_sci_reset(hi_sci_port sci_port, hi_bool warm_reset);

hi_s32 hi_drv_sci_send_data(hi_sci_port sci_port, hi_u8 *pdata_buf,
                            hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms);

hi_s32 hi_drv_sci_receive_data(hi_sci_port sci_port, hi_u8 *pdata_buf,
                               hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms);

hi_s32 hi_drv_sci_config_vcc(hi_sci_port sci_port, hi_sci_level sci_level);

hi_s32 hi_drv_sci_config_detect(hi_sci_port sci_port, hi_sci_level sci_level);

hi_s32 hi_drv_sci_config_clk_mode(hi_sci_port sci_port, hi_sci_mode clk_mode);

hi_s32 hi_drv_sci_get_sci_para(hi_sci_port sci_port, hi_sci_params_ptr psci_para);

hi_s32 hi_drv_sci_get_atr_buffer(hi_sci_port sci_port, hi_drv_atr_buf  *psci_atr_buf);

hi_s32 hi_drv_sci_deactive(hi_sci_port sci_port);

hi_s32 hi_drv_sci_get_status(hi_sci_port sci_port, hi_sci_status *pstate);

hi_s32 hi_drv_sci_open(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency);

hi_s32 hi_drv_sci_close(hi_sci_port sci_port);

hi_void hi_drv_sci_init(hi_void);

#endif

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description: the header for sci driver
 */

#ifndef __SCI_H__
#define __SCI_H__

#include <linux/interrupt.h>
#include <hi_errno.h>
#include "hal_sci.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SCI_DFT_STABLE_TIME     136
#define SCI_DFT_ACT_TIME        42500
#define SCI_DFT_DEACT_TIME      200
#if defined(HI_ADVCA_TYPE_CONAX)
#define SCI_DFT_ATRS_TIME       40200
#else
#define SCI_DFT_ATRS_TIME       40400
#endif
#define SCI_DFT_ATRD_TIME       40000
#define SCI_DFT_STOP_TIME       1860
#define SCI_DFT_START_TIME      700

#if defined(HI_ADVCA_TYPE_NAGRA)
#define SCI_DFT_CHAR_TIME       9650
#elif defined(HI_ADVCA_TYPE_CONAX)
#define SCI_DFT_CHAR_TIME       9700
#else
#define SCI_DFT_CHAR_TIME       65535
#endif

#define SCI_DFT_BLOCK_TIME      9600
#define SCI_DFT_BLKGUARD_TIME   22
#define SCI_DFT_RX_TIME         25000

#define SCI_DFT_WAIT_TIME      9600

#define SCI_ATR_BUF_LEN         256
#define SCI_READ_BUF_LEN        512

#define SCI_ATR_GROUP_START     127
#define SCI_ATR_GROUP_DONE      128

#define SCI_ATR_CONV_NORM       0x3b
#define SCI_ATR_CONV_NORM_INV   0x3f
#define SCI_ATR_CONV_INV        0x03

#if defined(CHIP_TYPE_HI3716MV430)
#define SCI_DFT_I_TIME          42600
#define SCI_DFT_C_TIME          42500
#endif

#define SCI_CHECK_POINTER(p)\
    do {\
        if ((p) == HI_NULL) {\
            HI_ERR_SCI("Pointer is NULL !\n");\
            return HI_ERR_SCI_NULL_PTR; \
        }\
    } while (0)

extern volatile sci_reg               *g_psci_reg[HI_SCI_PORT_MAX];
extern volatile sci_tide_reg          *g_psci_tide_reg[HI_SCI_PORT_MAX];


/* { 372, 372, 558, 744, 1116, 1488, 1860, 0, 0, 512, 768, 1024, 1536, 2048, 0, 0 } */
extern hi_u32 g_sci_clk_rate[];
/* { 0, 1, 2, 4, 8, 16, 32, 0, 12, 20, 0, 0, 0, 0, 0, 0 } */
extern hi_u32 g_sci_bit_rate[];

typedef struct {
    hi_sci_protocol    sci_protocol;
    hi_u32             frequency;
    hi_u32             rx_timeout;
    hi_u32             tx_timeout;
    hi_sci_level       sci_vcc;
    hi_sci_level       sci_detect;
    hi_sci_mode        clk_mode;
    hi_sci_mode        reset_mode;
    hi_sci_mode        vcc_en_mode;
    hi_sci_mode        data_mode;
} sci_attr;

typedef struct {
    hi_bool         rx_parity;
    hi_bool         tx_parity;
    sci_parity      rxparity;
    sci_parity      txparity;

    hi_u32          clock_icc;
    hi_u32          etu_value;
    hi_u32          baud_value;
    hi_u32          rx_tide;
    hi_u32          tx_tide;
    hi_u32          stable_time;
    hi_u32          active_time;
    hi_u32          deact_time;
    hi_u32          atr_start_time;
    hi_u32          atr_duration;
    hi_u32          stop_time;
    hi_u32          star_time;
    hi_u32          rx_retries;
    hi_u32          tx_retries;

    hi_u32          char_timeout;
    hi_u32          block_timeout;
    hi_u32          char_guard;
    hi_u32          block_guard;
    hi_u32          rx_timeout;

    hi_u32          clk_rate;
    hi_u32          bit_rate;
} sci_setup_para;

typedef enum {
    D_RFU0     = 0x0,      /* Reserved */
    D_1        = 0x1,      /* D=1 */
    D_2        = 0x2,      /* D=2 */
    D_4        = 0x3,      /* D=4 */
    D_8        = 0x4,      /* D=8 */
    D_16       = 0x5,      /* D=16 */
    D_32       = 0x6,      /* D=32 */
    D_RFU7     = 0x7,      /* Reserved */
    D_12       = 0x8,      /* D=12 */
    D_20       = 0x9,      /* D=20 */
    D_RFUA     = 0xa,      /* Reserved */
    D_RFUB     = 0xb,      /* Reserved */
    D_RFUC     = 0xc,      /* Reserved */
    D_RFUD     = 0xd,      /* Reserved */
    D_RFUE     = 0xe,      /* Reserved */
    D_RFUF     = 0xf       /* Reserved */
} sci_bit_rate_e;

typedef enum {
    F_RFU0     = 0x0,      /* Reserved */
    F_372      = 0x1,      /* F=372 */
    F_558      = 0x2,      /* F=558 */
    F_744      = 0x3,      /* F=744 */
    F_1116     = 0x4,      /* F=1116 */
    F_1488     = 0x5,      /* F=1488 */
    F_1860     = 0x6,      /* F=1860 */
    F_RFU7     = 0x7,      /* Reserved */
    F_RFU8     = 0x8,      /* Reserved */
    F_512      = 0x9,      /* F=512 */
    F_768      = 0xa,      /* F=768 */
    F_1024     = 0xb,      /* F=1024 */
    F_1536     = 0xc,      /* F=1536 */
    F_2048     = 0xd,      /* F=2048 */
    F_RFUE     = 0xe,      /* Reserved */
    F_RFUF     = 0xf       /* Reserved */
} sci_clock_rate;


typedef struct {
    sci_bit_rate_e       bit_rate;          /* bit rate adjustment */
    sci_clock_rate       clk_rate;          /* clock rate conversion */
    hi_u32               protocol_type;     /* first offered protocol type */
    hi_u32               alt_protocol_type; /* alternative offered protocol type */
    hi_u32               ch_guard;          /* char-char guard time */
    hi_u32               character_time;    /* Character to character timeout (etus) (default 9600) */
    hi_u32               block_time;        /* Timeout between blocks (etus) (default xxxx) */
    hi_u32               block_guard;       /* Minimum time between characters in oposite directions (default 16) */
    hi_u32               data_inform_size;  /* show card capabilitily receive data informatioan field size of T1 card */
    hi_bool              checksum_type;     /* 0:LRC  ;1: CRC  (default 0)   may be changed by TC3 */
    hi_bool              intf_byte_qual;    /* T = 15 has been received indicating global interface byte qualifier */
    hi_bool              has_checksum;      /* are we expecting a checksum? */
    hi_u32               check_sum;         /* maintains a checksum */
    hi_u32               char_present;      /* mask of characters for this group */
    hi_u32               grouping;          /* grouping being processed */
    hi_u32               current_ch;        /* character being processed */
    hi_u32               num_historical;    /* number of chars of historical data to be received */
    hi_u32               atr_mask;          /* show which ATR byte have received */
} sci_atr_para;


typedef struct {
    hi_sci_status       sci_crt_state;      /* current state of the smartcard */
    hi_sci_protocol     sci_protocol;       /* block or character transfer */
    hi_bool             sci_data_reversal;  /* Data convention */
    sci_setup_para      sci_setup_para;     /* specified setup parameters */
    sci_atr_para        sci_atr_para;       /* data regarding ATR setup */
    hi_bool             force_flag;
    hi_bool             set_extbaud_flag;   /* set extern baud flag */
} sci_state;

typedef struct {
    hi_bool                  sci_enable;
    sci_attr                 sci_attr;
    sci_state                sci_state;
    hi_u32                   err_type;
    hi_bool                  data_enough;
    hi_u32                   sys_clk;
    struct semaphore         sci_sem;
    struct tasklet_struct    sci_read_tasklet;
    wait_queue_head_t        sci_rec_waitqueue;
} sci_para;

typedef struct {
    hi_u8    data_buf[SCI_ATR_BUF_LEN];
    hi_u32   data_len;
} atr_buffer;

typedef struct {
    hi_u32    write;
    hi_u32    read;
    hi_u32    data_len;
    hi_u8     data_buf[SCI_READ_BUF_LEN + 1];
} sci_buffer;

typedef struct {
    hi_u32    crg_addr_offset;
    hi_u32    irq_number;
    hi_u32    reg_base_addr;

} sci_port_attr;

hi_void sci_init(hi_void);
hi_void sci_deinit(hi_void);
hi_s32 sci_open(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency);
hi_s32 sci_close(hi_sci_port sci_port);
hi_s32 sci_reset(hi_sci_port sci_port, hi_bool warm_reset);
hi_s32 sci_deactive(hi_sci_port sci_port);
hi_s32 sci_get_atr(hi_sci_port sci_port, hi_u8 *patr_buf, hi_u32 atr_buf_size, hi_u8 *patr_data_len);
hi_s32 sci_get_status(hi_sci_port sci_port, hi_sci_status *pstate);
hi_s32 sci_get_sci_para(hi_sci_port sci_port, sci_para *psci_para);
hi_s32 sci_get_atr_buffer(hi_sci_port sci_port, atr_buffer  *psci_atr_buf);
hi_s32 sci_pps_get_rec_buffer(hi_sci_port sci_port, hi_u8 *prec_buf, hi_u32 *pdata_len);
hi_s32 sci_output_param(hi_sci_port sci_port, hi_sci_params_ptr psci_out_param);
hi_s32 sci_config_vcc(hi_sci_port sci_port, hi_sci_level sci_level);
hi_s32 sci_config_detect(hi_sci_port sci_port, hi_sci_level sci_level);
hi_s32 sci_config_clk_mode(hi_sci_port sciPort, hi_sci_mode clk_mode);
hi_s32 sci_config_reset_mode(hi_sci_port sci_port, hi_sci_mode reset_mode);
hi_s32 sci_config_vcc_en_mode(hi_sci_port sci_port, hi_sci_mode vcc_en_mode);
hi_void sci_reset_action(hi_sci_port sci_port, hi_bool warm_reset);
hi_s32 sci_send_pps_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms);
hi_s32 sci_send_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms);
hi_s32 sci_receive_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms);
hi_s32 sci_receive_pps_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size,
                            hi_u32 *pdata_len, hi_u32 timeout_ms);
hi_s32 sci_pps_negotiation(hi_sci_port sci_port, hi_u8 *psend_buf, hi_u32 pps_len,  hi_u32 rec_timeouts);
hi_s32 sci_switch(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency);
hi_s32 sci_ext_set_baud(hi_sci_port sci_port, hi_u32 clk_rate, hi_u32 bit_rate);
hi_s32 sci_set_char_guard(hi_sci_port sci_port, hi_u32 ext_char_guard);
hi_s32 sci_set_char_timeout(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 char_timeouts);
hi_s32 sci_set_block_timeout(hi_sci_port sci_port, hi_u32 block_timeouts);
hi_s32 sci_set_tx_retry(hi_sci_port sci_port, hi_u32 tx_retry_times);
hi_s32 sci_suspend(hi_void);
hi_s32 sci_resume(hi_void);
hi_s32 sci_get_port_attr(hi_sci_port sci_port, sci_port_attr *port_attr);
hi_s32 sci_open_ree(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency);
hi_s32 sci_close_ree(hi_sci_port sci_port);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

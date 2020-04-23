/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: the header of hal for sci
 */

#ifndef __SCI_HAL_H__
#define __SCI_HAL_H__

#include "drv_sci_reg.h"
#include "drv_sci_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define IRQ_SCI0    (60 + 32)
#define IRQ_SCI1    (61 + 32)

#define SCI0_CRG_OFFSET   0x90
#define SCI1_CRG_OFFSET   0x90
#define SCI0_PHY_ADDR     0xA70000
#define SCI1_PHY_ADDR     0xA71000
#define SCI_TIDE_OFFSET_ADDR 0xF0


#define SCI_DFT_REF_CLOCK  60000UL

#define SCI_FIFO_SIZE      264

typedef enum {
    SCI_INT_CARDIN    = 0x1,
    SCI_INT_CARDOUT   = 0x2,
    SCI_INT_CARDUP    = 0x4,
    SCI_INT_CARDDOWN  = 0x8,
    SCI_INT_TXERR     = 0x10,
    SCI_INT_ATRSTOUT  = 0x20,
    SCI_INT_ATRDTOUT  = 0x40,
    SCI_INT_BLKTOUT   = 0x80,
    SCI_INT_CHTOUT    = 0x100,
    SCI_INT_RTOUT     = 0x200,
    SCI_INT_RORI      = 0x400,
    SCI_INT_CLKSTPI   = 0x800,
    SCI_INT_CLKACTI   = 0x1000,
    SCI_INT_RXTIDE    = 0x2000,
    SCI_INT_TXTIDE    = 0x4000,
} sci_int_e;

#if defined(HI_ADVCA_TYPE_NAGRA)
#define SCI_DFT_INT     ((SCI_INT_CARDIN) | (SCI_INT_CARDOUT)\
                        |(SCI_INT_CARDUP) | (SCI_INT_CARDDOWN)\
                        |(SCI_INT_RXTIDE) | (SCI_INT_ATRSTOUT)\
                        |(SCI_INT_ATRDTOUT) | (SCI_INT_RORI)\
                        |(SCI_INT_BLKTOUT) | (SCI_INT_CHTOUT))
#else
#define SCI_DFT_INT     ((SCI_INT_CARDIN) | (SCI_INT_CARDOUT)\
                        |(SCI_INT_CARDUP) | (SCI_INT_CARDDOWN)\
                        |(SCI_INT_RXTIDE) | (SCI_INT_ATRSTOUT)\
                        |(SCI_INT_ATRDTOUT) | (SCI_INT_RORI)\
                        |(SCI_INT_BLKTOUT))
#endif

typedef enum {
    SCI_PARITY_EVEN,
    SCI_PARITY_ODD
} sci_parity;

typedef enum {
    SCI_DIRECTION_RX,
    SCI_DIRECTION_TX
} sci_direction;


hi_void sci_hal_init(hi_sci_port sci_port, sci_reg *psci_reg);
hi_void sci_hal_tide_init(hi_sci_port sci_port, sci_tide_reg *psci_reg);
hi_void sci_hal_set_sci_clock(hi_sci_port sci_port, hi_bool enable);
hi_void sci_hal_set_sci_reset(hi_sci_port sci_port);
hi_void sci_hal_clear_sci_reset(hi_sci_port sci_port);

hi_u32 sci_hal_read_data(hi_sci_port sci_port);
hi_void sci_hal_write_data(hi_sci_port sci_port, hi_u8 sci_data);

hi_bool sci_hal_get_card_present(hi_sci_port sci_port);

hi_u32 sci_hal_get_cr0(hi_sci_port sci_port);
hi_void sci_hal_set_vcc(hi_sci_port sci_port, hi_sci_level sci_vcc);
hi_void sci_hal_set_detect(hi_sci_port sci_port, hi_sci_level sci_detect);
hi_void sci_hal_set_data_reversal(hi_sci_port sci_port, hi_bool data_reversal);
hi_void sci_hal_set_tx_parity_mode(hi_sci_port sci_port, hi_bool enable, sci_parity parity);
hi_void sci_hal_set_rx_parity_mode(hi_sci_port sci_port, hi_bool enable, sci_parity parity);
hi_void sci_hal_set_parity_enable(hi_sci_port sci_port, hi_bool enable);
hi_bool sci_hal_get_parity_enable(hi_sci_port sci_port);
hi_void sci_hal_set_clock(hi_sci_port sci_port, hi_bool enable, hi_u32 value);
hi_void sci_hal_set_reset_mode(hi_sci_port sci_port, hi_sci_mode reset_mode);
hi_void sci_hal_set_vccen_mode(hi_sci_port sci_port, hi_sci_mode vccen_mode);
hi_void sci_hal_set_data_mode(hi_sci_port sci_port, hi_sci_mode data_mode);

hi_void sci_hal_set_atr_timeout_enable(hi_sci_port sci_port, hi_bool enable);
hi_void sci_hal_set_block_timeout_enable(hi_sci_port sci_port, hi_bool enable);
hi_void sci_hal_set_clk_mode(hi_sci_port sci_port, hi_sci_mode clk_mode);
hi_void sci_hal_set_direction(hi_sci_port sci_port, sci_direction direction);
hi_void sci_hal_set_block_protect(hi_sci_port sci_port, hi_bool enable);
hi_void sci_hal_select_counter(hi_sci_port sci_port, hi_u32 value);
hi_void sci_hal_set_sync(hi_sci_port sci_port, hi_bool enable);

hi_void sci_hal_start_up(hi_sci_port sci_port);
hi_void sci_hal_finish(hi_sci_port sci_port);
hi_void sci_hal_reset(hi_sci_port sci_port);

hi_void sci_hal_set_clk_icc(hi_sci_port sci_port, hi_u32 value);
hi_void sci_hal_set_etu(hi_sci_port sci_port, hi_u32 value);
hi_void sci_hal_set_baud(hi_sci_port sci_port, hi_u32 value);
hi_void sci_hal_set_tide(hi_sci_port sci_port, hi_u32 rx_tide, hi_u32 tx_tide);
hi_void sci_hal_set_stable_time(hi_sci_port sci_port, hi_u32 stable_time);
hi_void sci_hal_set_active_time(hi_sci_port sci_port, hi_u32 active_time);
hi_void sci_hal_set_deact_time(hi_sci_port sci_port, hi_u32 deact_time);
hi_void sci_hal_set_atr_start_time(hi_sci_port sci_port, hi_u32 atr_start_time);
hi_void sci_hal_set_atr_duration_time(hi_sci_port sci_port, hi_u32 atr_duration_time);
hi_void sci_hal_set_stop_time(hi_sci_port sci_port, hi_u32 stop_time);
hi_void sci_hal_set_start_time(hi_sci_port sci_port, hi_u32 start_time);
hi_void sci_hal_set_retry(hi_sci_port sci_port, hi_u32 rx_retry, hi_u32 tx_retry);
hi_void sci_hal_set_char_timeout(hi_sci_port sci_port, hi_u32 char_timeout);
hi_void sci_hal_set_block_timeout(hi_sci_port sci_port, hi_u32 block_timeout);
hi_void sci_hal_set_char_guard(hi_sci_port sci_port, hi_u32 char_guard);
hi_void sci_hal_set_block_guard(hi_sci_port sci_port, hi_u32 block_guard);
hi_void sci_hal_set_rx_time(hi_sci_port sci_port, hi_u32 rx_time);
hi_u32  sci_hal_get_tx_fifo(hi_sci_port sci_port);
hi_void sci_hal_clear_tx_fifo(hi_sci_port sci_port);
hi_u32  sci_hal_get_rx_fifo(hi_sci_port sci_port);
hi_void sci_hal_clear_rx_fifo(hi_sci_port sci_port);

hi_void sci_hal_disable_all_int(hi_sci_port sci_port);
hi_void sci_hal_enable_int(hi_sci_port sci_port, sci_int_e sci_int_type);
hi_void sci_hal_disable_int(hi_sci_port sci_port, sci_int_e sci_int_type);
hi_u32  sci_hal_get_int_state(hi_sci_port sci_port);
hi_void sci_hal_clear_all_int(hi_sci_port sci_port);
hi_void sci_hal_clear_int(hi_sci_port sci_port, sci_int_e sci_int_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
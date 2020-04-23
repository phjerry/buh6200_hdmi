/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: hal for sci
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>

#include "hal_sci.h"
#include "hi_drv_sys.h"
#include "drv_sci.h"
#include "hi_reg_common.h"

volatile sci_reg                 *g_psci_reg[HI_SCI_PORT_MAX];
volatile sci_tide_reg            *g_psci_tide_reg[HI_SCI_PORT_MAX];

hi_void sci_hal_init(hi_sci_port sci_port, sci_reg *psci_reg)
{
    g_psci_reg[sci_port] = psci_reg;

    return;
}

hi_void sci_hal_tide_init(hi_sci_port sci_port, sci_tide_reg *psci_reg)
{
    g_psci_tide_reg[sci_port] = psci_reg;

    return;
}

hi_s32 sci_enable_clock(hi_sci_port sci_port, hi_bool enable)
{
    hi_u32 time_out = 0;
    U_PERI_CRG347 tmp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    if (sci_port == HI_SCI_PORT0) {
        tmp_value.u32  = reg_crg->PERI_CRG347.u32;
        tmp_value.bits.sci0_cken = enable;
        reg_crg->PERI_CRG347.u32 = tmp_value.u32;
        while (time_out < 100) { /* 100 times */
            tmp_value.u32  = reg_crg->PERI_CRG347.u32;
            if (enable != tmp_value.bits.sci0_cken) {
                time_out++;
                udelay(1);
            } else {
                break;
            }
        }
        if (time_out >= 100) { /* 100 times */
            HI_ERR_SCI("SCI0 set clock timeout!enable:%d!\n", enable);
            return HI_FAILURE;
        }
    }

    if (sci_port == HI_SCI_PORT1) {
        tmp_value.u32  = reg_crg->PERI_CRG347.u32;
        tmp_value.bits.sci1_cken = enable;
        reg_crg->PERI_CRG347.u32 = tmp_value.u32;
        while (time_out < 100) { /* 100 times */
            tmp_value.u32  = reg_crg->PERI_CRG347.u32;
            if (enable != tmp_value.bits.sci1_cken) {
                time_out++;
                udelay(1);
            } else {
                break;
            }
        }
        if (time_out >= 100) { /* 100 times */
            HI_ERR_SCI("SCI1 set clock timeout!enable:%d!\n", enable);
            return HI_FAILURE;
        }

    }
    mb();
    return HI_SUCCESS;
}

hi_s32 sci_reset_clock(hi_sci_port sci_port, hi_bool reset)
{
    hi_u32 time_out = 0;
    U_PERI_CRG347 tmp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    if (sci_port > HI_SCI_PORT1) {
        HI_ERR_SCI("Err param1!!\n");
        return HI_FAILURE;
    }

    if (sci_port == HI_SCI_PORT0) {
        tmp_value.u32  = reg_crg->PERI_CRG347.u32;
        tmp_value.bits.sci0_srst_req = reset;
        reg_crg->PERI_CRG347.u32 = tmp_value.u32;
        while (time_out < 100) { /* 100 times */
            tmp_value.u32  = reg_crg->PERI_CRG347.u32;
            if (tmp_value.bits.sci0_srst_req != reset) {
                time_out++;
                udelay(1);
            } else {
                break;
            }
        }
        if (time_out >= 100) { /* 100 times */
            HI_ERR_SCI("SCI0 reset clock timeout!reset:%d!\n", reset);
            return HI_FAILURE;
        }
    }

    if (sci_port == HI_SCI_PORT1) {
        tmp_value.u32  = reg_crg->PERI_CRG347.u32;
        tmp_value.bits.sci1_srst_req = reset;
        reg_crg->PERI_CRG347.u32 = tmp_value.u32;
        while (time_out < 100) { /* 100 times */
            tmp_value.u32  = reg_crg->PERI_CRG347.u32;
            if (tmp_value.bits.sci1_srst_req != reset) {
                time_out++;
                udelay(1);
            } else {
                break;
            }
        }
        if (time_out >= 100) { /* 100 times */
            HI_ERR_SCI("SCI1 reset clock timeout!bReset:%d!\n", reset);
            return HI_FAILURE;
        }

    }
    mb();
    return HI_SUCCESS;
}


/* set sci clk module */
hi_void sci_hal_set_sci_clock(hi_sci_port sci_port, hi_bool enable)
{
    if (sci_port > HI_SCI_PORT1) {
        HI_ERR_SCI("Err param1!!\n");
        return;
    }

    if (enable == HI_TRUE) {
        if (sci_port == HI_SCI_PORT0) {
            if (sci_enable_clock(HI_SCI_PORT0, HI_TRUE)) {
                return;
            }
        } else {
            if (sci_enable_clock(HI_SCI_PORT1, HI_TRUE)) {
                return;
            }
        }
    } else {
        if (sci_port == HI_SCI_PORT0) {
            if (sci_enable_clock(HI_SCI_PORT0, HI_FALSE)) {
                return;
            }
        } else {
            if (sci_enable_clock(HI_SCI_PORT1, HI_FALSE)) {
                return;
            }
        }
    }

    return;
}

/* sci module reset */
hi_void sci_hal_set_sci_reset(hi_sci_port sci_port)
{
    if (sci_port == HI_SCI_PORT0) {
        if (sci_reset_clock(HI_SCI_PORT0, HI_TRUE)) {
            return;
        }
    } else {
        if (sci_reset_clock(HI_SCI_PORT1, HI_TRUE)) {
            return;
        }
    }

    return;
}

/* remove sci module reset */
hi_void sci_hal_clear_sci_reset(hi_sci_port sci_port)
{
    if (sci_port == HI_SCI_PORT0) {
        if (sci_reset_clock(HI_SCI_PORT0, HI_FALSE)) {
            return;
        }
    } else {
        if (sci_reset_clock(HI_SCI_PORT1, HI_FALSE)) {
            return;
        }
    }

    return;
}

hi_u32 sci_hal_read_data(hi_sci_port sci_port)
{
    return g_psci_reg[sci_port]->sci_data.value;
}

hi_void sci_hal_write_data(hi_sci_port sci_port, hi_u8 sci_data)
{
    g_psci_reg[sci_port]->sci_data.value = sci_data;
    return;
}

/*      CR0         */
hi_bool sci_hal_get_card_present(hi_sci_port sci_port)
{
    sci_syncact_reg sci_syncact;

    sci_syncact.value = g_psci_reg[sci_port]->sci_syncact.value;

    if (sci_syncact.bits.cardpresent) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_u32 sci_hal_get_cr0(hi_sci_port sci_port)
{
    return g_psci_reg[sci_port]->sci_cr0.value;
}

hi_void sci_hal_set_vcc(hi_sci_port sci_port, hi_sci_level sci_vcc)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;
    if (sci_vcc == HI_SCI_LEVEL_LOW) {
        sci_cr0.bits.vccen_inv = 1;
    } else {
        sci_cr0.bits.vccen_inv = 0;
    }

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

hi_void sci_hal_set_detect(hi_sci_port sci_port, hi_sci_level sci_detect)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    if (sci_detect == HI_SCI_LEVEL_LOW) {
        sci_cr0.bits.detect_inv = 1;
    } else {
        sci_cr0.bits.detect_inv = 0;
    }

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

hi_void sci_hal_set_data_reversal(hi_sci_port sci_port, hi_bool data_reversal)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    sci_cr0.bits.sense = data_reversal;
    sci_cr0.bits.order = data_reversal;

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

hi_void sci_hal_set_tx_parity_mode(hi_sci_port sci_port, hi_bool enable, sci_parity parity)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    sci_cr0.bits.txparity = parity;
    sci_cr0.bits.txnak = enable;

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

hi_void sci_hal_set_rx_parity_mode(hi_sci_port sci_port, hi_bool enable, sci_parity parity)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    sci_cr0.bits.rxparity = parity;
    sci_cr0.bits.rxnak = enable;

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

hi_void sci_hal_set_parity_enable(hi_sci_port sci_port, hi_bool enable)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    sci_cr0.bits.paritybit = enable;

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

hi_bool sci_hal_get_parity_enable(hi_sci_port sci_port)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    return sci_cr0.bits.paritybit;
}

hi_void sci_hal_set_clock(hi_sci_port sci_port, hi_bool enable, hi_u32 value)
{
    sci_scr0_reg sci_cr0;

    sci_cr0.value = g_psci_reg[sci_port]->sci_cr0.value;

    sci_cr0.bits.clkdis = enable;
    sci_cr0.bits.clkval = value;

    g_psci_reg[sci_port]->sci_cr0.value = sci_cr0.value;

    return;
}

/* CR1 */
hi_void sci_hal_set_atr_timeout_enable(hi_sci_port sci_port, hi_bool enable)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.atrden = enable;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_block_timeout_enable(hi_sci_port sci_port, hi_bool enable)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.blken = enable;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_direction(hi_sci_port sci_port, sci_direction direction)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.mode = direction;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_clk_mode(hi_sci_port sci_port, hi_sci_mode clk_mode)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.clkzl = clk_mode;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_reset_mode(hi_sci_port sci_port, hi_sci_mode reset_mode)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.rstoutctl = reset_mode;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_vccen_mode(hi_sci_port sci_port, hi_sci_mode vccen_mode)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.vccenoutctl = vccen_mode;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_data_mode(hi_sci_port sci_port, hi_sci_mode data_mode)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;
    if (data_mode == HI_SCI_MODE_CMOS) {
        sci_cr1.bits.dataoutctl = 1;
    } else {
        sci_cr1.bits.dataoutctl = 0;
    }

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_block_protect(hi_sci_port sci_port, hi_bool enable)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.bgten = enable;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_select_counter(hi_sci_port sci_port, hi_u32 value)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.exdbnce = value;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

hi_void sci_hal_set_sync(hi_sci_port sci_port, hi_bool enable)
{
    sci_scr1_reg sci_cr1;

    sci_cr1.value = g_psci_reg[sci_port]->sci_cr1.value;

    sci_cr1.bits.synccard = enable;

    g_psci_reg[sci_port]->sci_cr1.value = sci_cr1.value;

    return;
}

/*      CR2         */
hi_void sci_hal_start_up(hi_sci_port sci_port)
{
    sci_scr2_reg sci_cr2;

    sci_cr2.value = g_psci_reg[sci_port]->sci_cr2.value;

    sci_cr2.bits.startup = 1;

    g_psci_reg[sci_port]->sci_cr2.value = sci_cr2.value;

    return;
}

hi_void sci_hal_finish(hi_sci_port sci_port)
{
    sci_scr2_reg sci_cr2;

    sci_cr2.value = g_psci_reg[sci_port]->sci_cr2.value;

    sci_cr2.bits.finish = 1;

    g_psci_reg[sci_port]->sci_cr2.value = sci_cr2.value;

    return;
}

hi_void sci_hal_reset(hi_sci_port sci_port)
{
    sci_scr2_reg sci_cr2;

    sci_cr2.value = g_psci_reg[sci_port]->sci_cr2.value;

    sci_cr2.bits.wreset = 1;

    g_psci_reg[sci_port]->sci_cr2.value = sci_cr2.value;

    return;
}

hi_void sci_hal_set_clk_icc(hi_sci_port sci_port, hi_u32 value)
{
    g_psci_reg[sci_port]->sci_clk_icc.value = value;

    return;
}

hi_void sci_hal_set_etu(hi_sci_port sci_port, hi_u32 value)
{
    g_psci_reg[sci_port]->sci_value.value = value;

    return;
}

hi_void sci_hal_set_baud(hi_sci_port sci_port, hi_u32 value)
{
    g_psci_reg[sci_port]->sci_baud.value = value;

    return;
}

hi_void sci_hal_set_tide(hi_sci_port sci_port, hi_u32 rx_tide, hi_u32 tx_tide)
{
    sci_rxtide_reg sci_rxtide;
    sci_txtide_reg sci_txtide;

    sci_rxtide.value = g_psci_tide_reg[sci_port]->sci_rxtide.value;
    sci_rxtide.bits.rxtide = rx_tide;

    sci_txtide.value = g_psci_tide_reg[sci_port]->sci_txtide.value;
    sci_txtide.bits.txtide = tx_tide;

    g_psci_tide_reg[sci_port]->sci_rxtide.value = sci_rxtide.value;
    g_psci_tide_reg[sci_port]->sci_txtide.value = sci_txtide.value;

    return;
}

hi_void sci_hal_set_stable_time(hi_sci_port sci_port, hi_u32 stable_time)
{
    g_psci_reg[sci_port]->sci_stable.value = stable_time;

    return;
}

hi_void sci_hal_set_active_time(hi_sci_port sci_port, hi_u32 active_time)
{
    g_psci_reg[sci_port]->sci_atime.value = active_time;

    return;
}

hi_void sci_hal_set_deact_time(hi_sci_port sci_port, hi_u32 deact_time)
{
    g_psci_reg[sci_port]->sci_dtime.value = deact_time;

    return;
}

hi_void sci_hal_set_atr_start_time(hi_sci_port sci_port, hi_u32 atr_start_time)
{
    g_psci_reg[sci_port]->sci_atr_stime.value = atr_start_time;

    return;
}

hi_void sci_hal_set_atr_duration_time(hi_sci_port sci_port, hi_u32 atr_duration_time)
{
    g_psci_reg[sci_port]->sci_atr_dtime.value = atr_duration_time;

    return;
}

hi_void sci_hal_set_stop_time(hi_sci_port sci_port, hi_u32 stop_time)
{
    g_psci_reg[sci_port]->sci_stop_time.value = stop_time;

    return;
}

hi_void sci_hal_set_start_time(hi_sci_port sci_port, hi_u32 start_time)
{
    g_psci_reg[sci_port]->sci_start_time.value = start_time;

    return;
}

hi_void sci_hal_set_retry(hi_sci_port sci_port, hi_u32 rx_retry, hi_u32 tx_retry)
{
    sci_retry_reg sci_retry;

    sci_retry.value = g_psci_reg[sci_port]->sci_retry.value;

    sci_retry.bits.txretry = tx_retry;
    sci_retry.bits.rxretry = rx_retry;

    g_psci_reg[sci_port]->sci_retry.value = sci_retry.value;

    return;
}

/* must set high 16 bit of sci_chtimels register at first,then set low 16bit of register */
hi_void sci_hal_set_char_timeout(hi_sci_port sci_port, hi_u32 char_timeout)
{
    g_psci_reg[sci_port]->sci_chtimels.value = (char_timeout & 0xffff0000) >> 16; /* right shift 16 bit */
    g_psci_reg[sci_port]->sci_chtimels.value = char_timeout & 0xffff;

    return;
}

hi_void sci_hal_set_block_timeout(hi_sci_port sci_port, hi_u32 block_timeout)
{
    g_psci_reg[sci_port]->sci_blktimems.value = (block_timeout & 0xffff0000) >> 16; /* right shift 16 bit */
    g_psci_reg[sci_port]->sci_blktimels.value = block_timeout & 0xffff;

    return;
}

hi_void sci_hal_set_char_guard(hi_sci_port sci_port, hi_u32 char_guard)
{
    g_psci_reg[sci_port]->sci_chguard.value = char_guard;
    return;
}

hi_void sci_hal_set_block_guard(hi_sci_port sci_port, hi_u32 block_guard)
{
    g_psci_reg[sci_port]->sci_blkguard.value = block_guard;

    return;
}

hi_void sci_hal_set_rx_time(hi_sci_port sci_port, hi_u32 rx_time)
{
    g_psci_reg[sci_port]->sci_rxtime.value = rx_time;

    return;
}

hi_u32 sci_hal_get_tx_fifo(hi_sci_port sci_port)
{
    return g_psci_reg[sci_port]->sci_txcount.value;
}

hi_void sci_hal_clear_tx_fifo(hi_sci_port sci_port)
{
    g_psci_reg[sci_port]->sci_txcount.value = 0xffffffff;

    return;
}

hi_u32 sci_hal_get_rx_fifo(hi_sci_port sci_port)
{
    return g_psci_reg[sci_port]->sci_rxcount.value;
}

hi_void sci_hal_clear_rx_fifo(hi_sci_port sci_port)
{
    g_psci_reg[sci_port]->sci_rxcount.value = 0xffffffff;

    return;
}

hi_void sci_hal_disable_all_int(hi_sci_port sci_port)
{
    g_psci_reg[sci_port]->sci_imsc.value = 0;

    return;
}

hi_void sci_hal_enable_int(hi_sci_port sci_port, sci_int_e sci_int_type)
{
    sci_imsc_reg sci_imsc;

    sci_imsc.value = g_psci_reg[sci_port]->sci_imsc.value;

    sci_imsc.value |= (hi_u32)sci_int_type;

    g_psci_reg[sci_port]->sci_imsc.value = sci_imsc.value;

    return;
}

hi_void sci_hal_disable_int(hi_sci_port sci_port, sci_int_e sci_int_type)
{
    sci_imsc_reg sci_imsc;

    sci_imsc.value = g_psci_reg[sci_port]->sci_imsc.value;

    sci_imsc.value = sci_imsc.value & (~(hi_u32)sci_int_type);

    g_psci_reg[sci_port]->sci_imsc.value = sci_imsc.value;

    return;
}

hi_u32 sci_hal_get_int_state(hi_sci_port sci_port)
{
    return g_psci_reg[sci_port]->sci_mis.value;
}

hi_void sci_hal_clear_all_int(hi_sci_port sci_port)
{
    g_psci_reg[sci_port]->sci_icr.value = 0xffffffff;
    return;
}

hi_void sci_hal_clear_int(hi_sci_port sci_port, sci_int_e sci_int_type)
{
    g_psci_reg[sci_port]->sci_icr.value = sci_int_type;

    return;
}

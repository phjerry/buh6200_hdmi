/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description: sci driver
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/wait.h>

#include "hi_drv_sys.h"
#include "drv_sci.h"
#include "hi_drv_osal.h"
#include "hi_reg_common.h"
#include "linux/hisilicon/securec.h"

#define SCI_ATR_ITEM(_grp, _cur) ((unsigned short)((((_cur) + 9) << 4) + (_grp)))


#define CHECK_PORTID(sci_port) do { \
        if ((sci_port) >= HI_SCI_PORT_NUM) { \
            HI_ERR_SCI("Port is invalid.\n"); \
            return HI_ERR_SCI_INVALID_PARA; \
        } \
    } while (0)

#define ATR_CHAR_TIMEOUT_ERR   0x99
#define GLOBAL_BUF_LENGTH    2
#define MIN_INTERVAL_BETWEEN_CHARACTERS 12

irqreturn_t      sci_isr(hi_s32 irq, hi_void *dev_id);
hi_void          sci_read_tasklet(hi_sci_port sci_port);
hi_s32           sci_atr_process_end(hi_sci_port sci_port, hi_u8 atr_data);

hi_u32           g_sci_clk_rate[] = { 372, 372, 558, 744, 1116, 1488, 1860, 0, 0, 512, 768, 1024, 1536, 2048, 0, 0 };
hi_u32           g_sci_bit_rate[] = { 0, 1, 2, 4, 8, 16, 32, 0, 12, 20, 0, 0, 0, 0, 0, 0 };
hi_u32           g_suspend_clk_rate[GLOBAL_BUF_LENGTH];
hi_u32           g_suspend_bit_rate[GLOBAL_BUF_LENGTH];
hi_bool          g_suspend_force_flag[GLOBAL_BUF_LENGTH];
hi_bool          g_suspend_set_ext_baud_flag[GLOBAL_BUF_LENGTH];
hi_sci_protocol  g_suspend_sci_protocol[GLOBAL_BUF_LENGTH];
hi_sci_status    g_suspend_sci_state[GLOBAL_BUF_LENGTH];

sci_para         g_sci_para[HI_SCI_PORT_MAX];
atr_buffer       g_sci_atr_buf[HI_SCI_PORT_MAX];
sci_buffer       g_sci_data_buf[HI_SCI_PORT_MAX];
sci_buffer       g_sci_send_data_buf[HI_SCI_PORT_MAX];
sci_pps          g_sci_pps[HI_SCI_PORT_MAX];

static DEFINE_SPINLOCK(sci_lock);
static hi_u32    g_hsci_irq[HI_SCI_PORT_MAX];
static sci_port_attr g_sci_port_attr[HI_SCI_PORT_MAX] = {
    { SCI0_CRG_OFFSET, IRQ_SCI0, SCI0_PHY_ADDR },
    { SCI1_CRG_OFFSET, IRQ_SCI1, SCI1_PHY_ADDR }
};

hi_s32 sci_get_port_attr(hi_sci_port sci_port, sci_port_attr *pport_attr)
{
    SCI_CHECK_POINTER(pport_attr);
    if (sci_port >= HI_SCI_PORT_NUM) {
        return HI_FAILURE;
    }

    if (memcpy_s(pport_attr, sizeof(sci_port_attr), &g_sci_port_attr[sci_port], sizeof(sci_port_attr))) {
        HI_ERR_SCI("memcpy_s failed! \n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void sci_clean_buf(hi_sci_port sci_port)
{
    g_sci_data_buf[sci_port].write = 0;
    g_sci_data_buf[sci_port].read = 0;
    g_sci_data_buf[sci_port].data_len = 0;

    sci_hal_clear_tx_fifo(sci_port);
    sci_hal_clear_rx_fifo(sci_port);

    return;
}

hi_void sci_set_state(hi_sci_port sci_port, hi_sci_status state)
{
    g_sci_para[sci_port].sci_state.sci_crt_state = state;
    return;
}

hi_s32 sci_init_setup_para(hi_sci_port sci_port)
{
    g_sci_para[sci_port].sci_state.sci_protocol = HI_SCI_PROTOCOL_T0;
    g_sci_para[sci_port].sci_state.sci_data_reversal = HI_FALSE;
    g_sci_para[sci_port].sci_state.sci_setup_para.rx_parity  = HI_FALSE;
    g_sci_para[sci_port].sci_state.sci_setup_para.tx_parity  = HI_FALSE;
#if  defined(HI_ADVCA_TYPE_NAGRA)
    g_sci_para[sci_port].sci_state.sci_setup_para.rxparity = SCI_PARITY_ODD;
    g_sci_para[sci_port].sci_state.sci_setup_para.txparity = SCI_PARITY_ODD;
#else
    g_sci_para[sci_port].sci_state.sci_setup_para.rxparity = SCI_PARITY_EVEN;
    g_sci_para[sci_port].sci_state.sci_setup_para.txparity = SCI_PARITY_EVEN;
#endif

    if (g_sci_para[sci_port].sci_attr.frequency == 0) {
        HI_ERR_SCI("Freq is 0 as divisor! \n");
        return HI_FAILURE;
    }
    g_sci_para[sci_port].sci_state.sci_setup_para.clock_icc = g_sci_para[sci_port].sys_clk
            / g_sci_para[sci_port].sci_attr.frequency
            / 2 - 1; /* 2: clock = sys_clk/freq/2 -1 */
    g_sci_para[sci_port].sci_state.sci_setup_para.stable_time = SCI_DFT_STABLE_TIME * g_sci_para[sci_port].sys_clk
            / 65535; /* 65535 : calculation formula parameter */

    g_sci_para[sci_port].sci_state.sci_setup_para.rx_tide = 1;
    g_sci_para[sci_port].sci_state.sci_setup_para.tx_tide = 1;
    g_sci_para[sci_port].sci_state.sci_setup_para.active_time = SCI_DFT_ACT_TIME;
    g_sci_para[sci_port].sci_state.sci_setup_para.deact_time = SCI_DFT_DEACT_TIME;
    g_sci_para[sci_port].sci_state.sci_setup_para.atr_start_time = SCI_DFT_ATRS_TIME;
    g_sci_para[sci_port].sci_state.sci_setup_para.atr_duration = SCI_DFT_ATRD_TIME;
    g_sci_para[sci_port].sci_state.sci_setup_para.stop_time     = SCI_DFT_STOP_TIME;
    g_sci_para[sci_port].sci_state.sci_setup_para.tx_retries    = SCI_DFT_START_TIME;
    g_sci_para[sci_port].sci_state.sci_setup_para.rx_retries    = 3; /* 3:Rx retry times */
    g_sci_para[sci_port].sci_state.sci_setup_para.tx_retries    = 3; /* 3:Tx retry times */
    g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout  = \
        SCI_DFT_CHAR_TIME - MIN_INTERVAL_BETWEEN_CHARACTERS;
    g_sci_para[sci_port].sci_state.sci_setup_para.char_guard = \
        SCI_DFT_BLOCK_TIME - MIN_INTERVAL_BETWEEN_CHARACTERS;
    g_sci_para[sci_port].sci_state.sci_setup_para.char_guard    = 0;
    g_sci_para[sci_port].sci_state.sci_setup_para.block_guard   = \
        SCI_DFT_BLKGUARD_TIME - MIN_INTERVAL_BETWEEN_CHARACTERS;
    g_sci_para[sci_port].sci_state.sci_setup_para.rx_timeout    = SCI_DFT_RX_TIME;

    g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate = 372; /* 372:default clk rate */
    g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate = 1;

    return HI_SUCCESS;
}

hi_s32 sci_default_config(hi_sci_port sci_port)
{
    hi_s32 ret;
    sci_setup_para      *psci_setup_para = NULL;

    ret = sci_init_setup_para(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("sci_init_setup_para failed! \n");
        return HI_FAILURE;
    }

    psci_setup_para = &g_sci_para[sci_port].sci_state.sci_setup_para;
    /*
     * CR0 register default config
     * set default data sequence
     */
    sci_hal_set_data_reversal(sci_port, g_sci_para[sci_port].sci_state.sci_data_reversal);

    /* set send data parity check */
    sci_hal_set_tx_parity_mode(sci_port, psci_setup_para->tx_parity, psci_setup_para->txparity);

    /* set receive data parity check */
    sci_hal_set_rx_parity_mode(sci_port, psci_setup_para->rx_parity, psci_setup_para->rxparity);

    /* enable clk,the clk pin is low when disable clk */
    sci_hal_set_clock(sci_port, HI_FALSE, 0);

    /*
     * CR1 register default config
     * set receive ATR timeout count
     */
    sci_hal_set_atr_timeout_enable(sci_port, HI_TRUE);

    /* set receive block data timeout count */
    sci_hal_set_block_timeout_enable(sci_port, HI_TRUE);

    /* set default receice direction */
    sci_hal_set_direction(sci_port, SCI_DIRECTION_RX);

    /* set block guard time count */
    sci_hal_set_block_protect(sci_port, HI_TRUE);

    /* set card present check  counter */
    sci_hal_select_counter(sci_port, 0);

    /* set  asynchronism order */
    sci_hal_set_sync(sci_port, 0);

    sci_hal_set_clk_icc(sci_port, psci_setup_para->clock_icc);

    sci_hal_set_tide(sci_port, psci_setup_para->rx_tide, psci_setup_para->tx_tide);

    sci_hal_set_stable_time(sci_port, psci_setup_para->stable_time);

    sci_hal_set_active_time(sci_port, psci_setup_para->active_time);

    sci_hal_set_deact_time(sci_port, psci_setup_para->deact_time);

    sci_hal_set_atr_start_time(sci_port, psci_setup_para->atr_start_time);

    sci_hal_set_atr_duration_time(sci_port, psci_setup_para->atr_duration);

    sci_hal_set_char_timeout(sci_port, psci_setup_para->char_timeout);

    sci_hal_set_stop_time(sci_port, psci_setup_para->stop_time);

    sci_hal_set_start_time(sci_port, psci_setup_para->tx_retries);

    sci_hal_set_retry(sci_port, psci_setup_para->rx_retries, psci_setup_para->tx_retries);

    sci_hal_set_rx_time(sci_port, psci_setup_para->rx_timeout);

    return HI_SUCCESS;
}

hi_void sci_calc_etu_baud(hi_sci_port sci_port, hi_u32 clk_rate, hi_u32 bit_rate)
{
    hi_u32 baud_total, etu_value, baud_value;
    hi_u32 actual_sci_clk;

    if (bit_rate == 0) {
        HI_ERR_SCI("bit_rate is 0 as divisor! \n");
        return;
    }

    actual_sci_clk = g_sci_para[sci_port].sys_clk /
                     (g_sci_para[sci_port].sci_state.sci_setup_para.clock_icc + 1) / 2; /* 2 : calc formula para */
    if (actual_sci_clk == 0) {
        HI_ERR_SCI("actual_sci_clk is 0 as divisor! \n");
        return;
    }
    baud_total = (clk_rate / bit_rate) * (g_sci_para[sci_port].sys_clk / actual_sci_clk);

    etu_value = 5; /* set etu_value 5 */
    while (etu_value < 255) { /* max etu value 255 */
        if ((baud_total % etu_value == 0) && ((baud_total / etu_value - 1) < 0xffff)) {
            break;
        }

        etu_value++;
    }

    baud_value = baud_total / etu_value - 1;

    g_sci_para[sci_port].sci_state.sci_setup_para.etu_value  = etu_value;
    g_sci_para[sci_port].sci_state.sci_setup_para.baud_value = baud_value;

    sci_hal_set_etu(sci_port, etu_value);
    sci_hal_set_baud(sci_port, baud_value);

    return;
}

hi_void sci_cfg_t0_para(hi_sci_port sci_port)
{
    sci_setup_para      *psci_setup_para = NULL;

    psci_setup_para = &g_sci_para[sci_port].sci_state.sci_setup_para;

    sci_hal_set_char_timeout(sci_port, psci_setup_para->char_timeout);

    sci_hal_set_block_timeout(sci_port, psci_setup_para->char_guard);

    return;
}

hi_void sci_cfg_t1_para(hi_sci_port sci_port)
{
    sci_setup_para      *psci_setup_para = NULL;

    psci_setup_para = &g_sci_para[sci_port].sci_state.sci_setup_para;

    sci_hal_set_char_timeout(sci_port, psci_setup_para->char_timeout);

    sci_hal_set_block_timeout(sci_port, psci_setup_para->char_guard);

    sci_hal_set_block_guard(sci_port, psci_setup_para->block_guard);

    return;
}

hi_s32 sci_config_t0(hi_sci_port sci_port)
{
    hi_s32 ret;

    ret = sci_default_config(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("sci_default_config failed! \n");
        return HI_FAILURE;
    }

    sci_cfg_t0_para(sci_port);

    sci_calc_etu_baud(sci_port, g_sci_clk_rate[F_372], g_sci_bit_rate[D_1]);

    sci_hal_set_parity_enable(sci_port, HI_TRUE);

    /* if T0 support block_guard, it can be set here */
    return HI_SUCCESS;
}

hi_s32 sci_config_t1(hi_sci_port sci_port)
{
    hi_s32 ret;

    ret = sci_default_config(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("sci_default_config failed! \n");
        return HI_FAILURE;
    }

    sci_cfg_t1_para(sci_port);

    sci_calc_etu_baud(sci_port, g_sci_clk_rate[F_372], g_sci_bit_rate[D_1]);

    sci_hal_set_parity_enable(sci_port, HI_TRUE);

    return HI_SUCCESS;
}

/* T14 don't need parity check,set it default F factor equel 620 and D factor equal 1 */
hi_s32 sci_config_t14(hi_sci_port sci_port)
{
    hi_s32 ret;

    ret = sci_default_config(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("sci_default_config failed! \n");
        return HI_FAILURE;
    }

    g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate = 620; /* default F factor equel 620 */
    sci_calc_etu_baud(sci_port, 620, g_sci_bit_rate[D_1]); /* default F factor equel 620 */

    sci_hal_set_parity_enable(sci_port, HI_FALSE);

    /* if T14 support block_guard, it can be set here */
    return HI_SUCCESS;
}

hi_s32 sci_config_protocol(hi_sci_port sci_port)
{
    hi_s32 ret;
    hi_sci_protocol sci_protocol;

    sci_protocol = g_sci_para[sci_port].sci_attr.sci_protocol;

    if (sci_protocol == HI_SCI_PROTOCOL_T14) {
        ret = sci_config_t14(sci_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_SCI("sci_config_t14 failed! \n");
            return HI_FAILURE;
        }
    } else if (sci_protocol == HI_SCI_PROTOCOL_T0) {
        ret = sci_config_t0(sci_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_SCI("sci_config_t0 failed! \n");
            return HI_FAILURE;
        }
    } else if (sci_protocol == HI_SCI_PROTOCOL_T1) {
        ret = sci_config_t1(sci_port);
        if (ret != HI_SUCCESS) {
            HI_ERR_SCI("sci_config_t1 failed! \n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_void sci_card_in(hi_sci_port sci_port)
{
    if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_NOCARD) {
        sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
    }

    return;
}

hi_void sci_card_out(hi_sci_port sci_port)
{
    g_sci_para[sci_port].sci_state.force_flag = HI_FALSE;
    g_sci_para[sci_port].sci_state.set_extbaud_flag = HI_FALSE;

    sci_set_state(sci_port, HI_SCI_STATUS_NOCARD);

    wake_up_interruptible(&g_sci_para[sci_port].sci_rec_waitqueue);

    return;
}

hi_void sci_card_down(hi_sci_port sci_port)
{
    if (g_sci_para[sci_port].sci_state.sci_crt_state > HI_SCI_STATUS_INACTIVECARD) {
        sci_hal_disable_all_int(sci_port);
        sci_hal_clear_all_int(sci_port);

        (hi_void)sci_config_protocol(sci_port);
        g_sci_para[sci_port].err_type = 0;
        g_sci_para[sci_port].sci_state.force_flag = HI_FALSE;
        g_sci_para[sci_port].sci_state.set_extbaud_flag = HI_FALSE;

        sci_clean_buf(sci_port);

        g_sci_atr_buf[sci_port].data_len = 0;
        g_sci_para[sci_port].sci_state.sci_atr_para.grouping = SCI_ATR_GROUP_START;
        g_sci_para[sci_port].sci_state.sci_atr_para.check_sum = 0;
        g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask = 0;

        if (sci_hal_get_card_present(sci_port)) {
            sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
        } else {
            sci_set_state(sci_port, HI_SCI_STATUS_NOCARD);
        }

        sci_hal_enable_int(sci_port, SCI_DFT_INT);
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state != HI_SCI_STATUS_NOCARD) {
        sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
    }

    return;
}

hi_void sci_rx_tide(hi_sci_port sci_port)
{
    hi_u32 reg_data;
    hi_u32 data_len;

    if (((g_sci_data_buf[sci_port].write + 1) % SCI_READ_BUF_LEN) == g_sci_data_buf[sci_port].read) {
        sci_clean_buf(sci_port);
        return;
    }

    reg_data = sci_hal_read_data(sci_port);

    if (sci_hal_get_parity_enable(sci_port) && (reg_data & 0x100)) {
        HI_ERR_SCI("SCI parity failed.reg_data:0x%x\n", reg_data);
        if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_READATR) {
            HI_ERR_SCI("SCI Parity failed!\n");
#if defined(HI_ADVCA_TYPE_NAGRA)
            sci_set_state(sci_port, HI_SCI_STATUS_ATR_ERROR);
            return;
#endif
        }
    }

    g_sci_data_buf[sci_port].data_buf[g_sci_data_buf[sci_port].write] = (hi_u8)(reg_data);

    g_sci_data_buf[sci_port].write = (g_sci_data_buf[sci_port].write + 1) % SCI_READ_BUF_LEN;

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        tasklet_schedule(&g_sci_para[sci_port].sci_read_tasklet);
    } else {
        if (!g_sci_data_buf[sci_port].data_len) {
            return;
        }

        if (g_sci_data_buf[sci_port].write >= g_sci_data_buf[sci_port].read) {
            data_len = g_sci_data_buf[sci_port].write - g_sci_data_buf[sci_port].read;
        } else {
            data_len = SCI_READ_BUF_LEN - g_sci_data_buf[sci_port].read + g_sci_data_buf[sci_port].write;
        }

        if (data_len >= g_sci_data_buf[sci_port].data_len) {
            g_sci_para[sci_port].data_enough = HI_TRUE;
            wake_up_interruptible(&g_sci_para[sci_port].sci_rec_waitqueue);
        }
    }

    return;
}

hi_void sci_push_data(hi_sci_port sci_port)
{
    hi_u32 fifo_len = 0;
    hi_u8 *ptmp_buf = HI_NULL;
    hi_u32 count = 0;

    if (g_sci_send_data_buf[sci_port].read < g_sci_send_data_buf[sci_port].data_len) {
        sci_hal_disable_int(sci_port, SCI_INT_TXTIDE);
        fifo_len = sci_hal_get_tx_fifo(sci_port);

        ptmp_buf = &(g_sci_send_data_buf[sci_port].data_buf[g_sci_send_data_buf[sci_port].read]);
        for (count = fifo_len; ((count < SCI_FIFO_SIZE) &&
                                (g_sci_send_data_buf[sci_port].read < g_sci_send_data_buf[sci_port].data_len));
                count++) {
            sci_hal_write_data(sci_port, *ptmp_buf);
            ptmp_buf++;
            g_sci_send_data_buf[sci_port].read++;
        }

        if (g_sci_send_data_buf[sci_port].read == g_sci_send_data_buf[sci_port].data_len) {
            while (1) {
                fifo_len = sci_hal_get_tx_fifo(sci_port);
                if (fifo_len == 0) {
                    break;
                } else {
                    msleep(1);
                }
            }
            sci_hal_set_direction(sci_port, SCI_DIRECTION_RX);
            wake_up_interruptible(&g_sci_para[sci_port].sci_rec_waitqueue);
        } else {
            sci_hal_enable_int(sci_port, SCI_INT_TXTIDE);
        }
    } else {
        while (1) {
            fifo_len = sci_hal_get_tx_fifo(sci_port);
            if (fifo_len == 0) {
                break;
            } else {
                msleep(1);
            }
        }
        sci_hal_set_direction(sci_port, SCI_DIRECTION_RX);
        sci_hal_disable_int(sci_port, SCI_INT_TXTIDE);
    }

    return;
}

hi_void sci_char_reversal(hi_u8 *patr_data)
{
    hi_u32 tmp_value = 0;
    hi_s32 i;

    for (i = 8; i > 0; i--) { /* 8bit */
        if (*patr_data & 0x1) {
            tmp_value |= 0x1;
        }

        tmp_value  = tmp_value << 1;
        *patr_data = *patr_data >> 1;
    }

    *patr_data = 0xFF & ~(tmp_value >> 1);
}

hi_void sci_pps_clear_data_buf(hi_sci_port sci_port)
{
    hi_u32 i;

    for (i = 0; i < SCI_PPS_SEND_RECV_BUF_LEN; i++) {
        g_sci_pps[sci_port].send[i] = 0;
        g_sci_pps[sci_port].receive[i] = 0;
    }

    g_sci_pps[sci_port].send_len = 0;
    g_sci_pps[sci_port].receive_len = 0;
}

hi_s32 sci_pps_send_package(hi_sci_port sci_port, hi_u8 *psend_buf, hi_u32 pps_len)
{
    hi_s32 send_no = 0;
    hi_u32 tmp;
    hi_u32 send_check_sum = 0;
    hi_u32 pps_format = 0;

    SCI_CHECK_POINTER(psend_buf);
    /* need clear PPS data buffer before send package */
    sci_pps_clear_data_buf(sci_port);

    /* It will  negotiate PPS with ATR Protocol and TA1 value return from card  when pps_len is 0 */
    if (pps_len == 0) {
        g_sci_pps[sci_port].send[send_no] = 0xff; /* PPS */
        send_no++;
        if ((g_sci_para[sci_port].sci_attr.sci_protocol != HI_SCI_PROTOCOL_T0) &&
                (g_sci_para[sci_port].sci_attr.sci_protocol != HI_SCI_PROTOCOL_T1) &&
                (g_sci_para[sci_port].sci_attr.sci_protocol != HI_SCI_PROTOCOL_T14)) {
            HI_ERR_SCI("Not support PPS protocol type !\n");
            return HI_ERR_SCI_PPS_PTYPE_ERR;
        }

        if (g_sci_para[sci_port].sci_attr.sci_protocol == HI_SCI_PROTOCOL_T14) {
            pps_format |= (((hi_u8)g_sci_para[sci_port].sci_attr.sci_protocol + 12) & 0x0F); /* 12:calc formula para */
        } else {
            pps_format |= ((hi_u8)g_sci_para[sci_port].sci_attr.sci_protocol & 0x0F);
        }

        g_sci_pps[sci_port].send[send_no] = pps_format; /* PPS0 */

        if ((g_sci_atr_buf[sci_port].data_buf[1] & 0x10)) { /* Atr TA1 present,will send PPS1 */
            pps_format |= (0x01 << 4); /* right shift 4 bit */
            g_sci_pps[sci_port].send[send_no] = pps_format; /* PPS0 */

            send_no++;
            g_sci_pps[sci_port].send[send_no] = g_sci_atr_buf[sci_port].data_buf[2]; /* PPS1  2: data_buf length */
        } else {
            HI_INFO_SCI("No PPS factor negotiation !\n");
        }

        send_no++;
        tmp = send_no;
        while (tmp > 0) {
            send_check_sum = send_check_sum ^ g_sci_pps[sci_port].send[--tmp];
            HI_INFO_SCI(" g_sci_pps.Send[%d]: %X  \n", tmp, g_sci_pps[sci_port].send[tmp]);
        }

        g_sci_pps[sci_port].send[send_no] = send_check_sum; /* PCK */
        g_sci_pps[sci_port].send_len = send_no + 1;
        HI_INFO_SCI(" g_sci_pps.Send[%d]: %X  \n", send_no, g_sci_pps[sci_port].send[send_no]);
    }
    /* It will negotiate PPS with psend_buf value from user */
    else {
        if (memcpy_s(g_sci_pps[sci_port].send, SCI_PPS_SEND_RECV_BUF_LEN,  psend_buf, pps_len)) {
            HI_ERR_SCI("memcpy_s failed! \n");
            return HI_FAILURE;
        }
        if (g_sci_pps[sci_port].send[0] != 0xff) {
            HI_ERR_SCI("Not PPS Command !\n");
            return HI_ERR_SCI_PPS_NOTSUPPORT_ERR;
        }

        if (HI_SCI_PROTOCOL_T0 != (g_sci_pps[sci_port].send[1] & 0x0f) &&
                (HI_SCI_PROTOCOL_T1 != (g_sci_pps[sci_port].send[1] & 0x0f)) &&
                ((g_sci_pps[sci_port].send[1] & 0x0f) != 14)) { /* T14 protocal type */
            HI_ERR_SCI("Not support PPS protocol type !\n");
            return HI_ERR_SCI_PPS_PTYPE_ERR;
        }

        if (!(g_sci_pps[sci_port].send[1] & 0x10)) {
            HI_INFO_SCI("No PPS factor negotiation !\n");
        }

        for (tmp = 0; tmp < pps_len; tmp++) {
            HI_INFO_SCI(" g_sci_pps.Send[%d]: %X  \n", tmp, g_sci_pps[sci_port].send[tmp]);
            send_check_sum ^= g_sci_pps[sci_port].send[tmp];
        }

        g_sci_pps[sci_port].send[tmp] = send_check_sum;
        HI_INFO_SCI(" g_sci_pps.Send[%d]: %X  \n", tmp, g_sci_pps[sci_port].send[tmp]);

        g_sci_pps[sci_port].send_len = pps_len + 1;
    }

    return HI_SUCCESS;
}

hi_s32 sci_pps_receive(hi_sci_port sci_port, hi_u32 rec_timeouts)
{
    hi_s32 i;
    hi_s32 ret;
    hi_u32 format;
    hi_u32 times;
    hi_s32 tmp = 0;
    hi_u32 rel_reclen = 0;
    hi_s32 receive_no = 0;
    hi_bool flag = HI_FALSE;
    hi_s32 pps_flag = HI_SUCCESS;
    hi_bool ch_protocol = HI_FALSE;

    CHECK_PORTID(sci_port);

    g_sci_pps[sci_port].receive_len = 0;
    ret = sci_receive_pps_data(sci_port, &g_sci_pps[sci_port].receive[receive_no], 1, &rel_reclen, rec_timeouts);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("receive PPSS  fail,expect receive %d,but noly receive %d \n", 1, rel_reclen);
        return ret;
    }

    if (g_sci_pps[sci_port].receive[receive_no] != 0xff) { /* if isn't card  PPS response will warm reset and return */
        HI_ERR_SCI("receive data isn't PPS  response\n");
        return HI_ERR_SCI_PPS_NOTSUPPORT_ERR;
    }

    receive_no++;
    ret = sci_receive_pps_data(sci_port, &g_sci_pps[sci_port].receive[receive_no], 1, &rel_reclen, rec_timeouts);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("receive PPS0  fail,expect receive %d,but noly receive %d \n", 1, rel_reclen);
        return ret;
    }

    format = g_sci_pps[sci_port].receive[receive_no];
    if ((format & 0x0f) == (g_sci_pps[sci_port].send[1] & 0X0F)) {
        /* if PPS negotiation protocol don't equal original set protocal will reconfig protocal */
        if ((format & 0x0f) != g_sci_para[sci_port].sci_attr.sci_protocol) {
            ch_protocol = HI_TRUE;
        }

        g_sci_para[sci_port].sci_state.sci_protocol = format & 0x0f;
        g_sci_para[sci_port].sci_attr.sci_protocol = format & 0x0f;
        format = format >> 3; /* right shift 3 bit */
    } else {
        pps_flag = HI_ERR_SCI_PPS_PTYPE_ERR;
    }

    for (i = 0; i < 4; i++) { /* for 4 times */
        tmp++; /* receive counter */
        format = format >> 1; /* format indicate which PPSx (1~3) will receive */

        if ((format & 0x01) || (tmp == 4)) { /* PCK  tmp is 4 */
            receive_no++;

            ret = sci_receive_pps_data(sci_port, &g_sci_pps[sci_port].receive[receive_no],
                                       1, &rel_reclen, rec_timeouts);
            if (ret != HI_SUCCESS) {
                HI_ERR_SCI("receive PPSx pakeage fail,expect receive %d,but noly receive %d \n", 1, rel_reclen);
                pps_flag = HI_ERR_SCI_RECEIVE_ERR;
                break;
            }
        }

        switch (tmp) {
            case 1: { /* PPS1 */
                if (!(format & 0x01)) { /* negotiation success */
                    /* if PPS0 bit = 0, PPS1 is absent, set negotiation Fd/Dd */
                    g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate = 372; /* default clk rate 372 */
                    g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate = 1;
                    break;
                }
                /* PPS0 bit5 =1 and PPS1 response == PPS1 requeset, negotiation success,set negotiation Fn, Dn */
                if (g_sci_pps[sci_port].receive[receive_no] == g_sci_pps[sci_port].send[2]) { /* 2:send buf len */
                g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate =
                    g_sci_clk_rate[(g_sci_pps[sci_port].receive[receive_no] & 0xf0) >> 4]; /* right 4 bit */
                g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate =
                    g_sci_bit_rate[(g_sci_pps[sci_port].receive[receive_no] & 0x0f)];
                } else {
                    pps_flag = HI_ERR_SCI_PPS_FACTOR_ERR;
                }

                break;
            }

            case 2: /* PPS2 */
                if (format & 0x01) {
                    HI_INFO_SCI(" PPS2.\n");
                }

                break;

            case 3: /* PPS3 */

                break;

            case 4: /* case 4 PCK */

                break;

            default:
                pps_flag = HI_ERR_SCI_PPS_NOTSUPPORT_ERR;
                break;
        }
    }

    g_sci_pps[sci_port].receive_len = receive_no + 1;

    while (receive_no >= 0) {
        HI_INFO_SCI("g_sci_pps[sci_port].Receive[%d] = %x \n", receive_no, g_sci_pps[sci_port].receive[receive_no]);
        receive_no--;
    }

    udelay(100); /* delay 100us */

    if (pps_flag == HI_SUCCESS) {
        hi_s32 ret;

        ret = down_interruptible(&g_sci_para[sci_port].sci_sem);

        if (!g_sci_para[sci_port].sci_enable) {
            HI_ERR_SCI("PPS set:SCI %d is not opened.\n", sci_port);
            up(&g_sci_para[sci_port].sci_sem);
            return HI_ERR_SCI_INVALID_OPT;
        }

        if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
            HI_ERR_SCI("PPS set:SCI%d state is not ready\n", sci_port);
            up(&g_sci_para[sci_port].sci_sem);
            return HI_ERR_SCI_RECEIVE_ERR;
        }

        /* have change protocol,will reconfig */
        if (ch_protocol == HI_TRUE) {
            HI_FATAL_SCI("PPS negotiation change protocol \n");
            ret = sci_config_protocol(sci_port);
            if (ret != HI_SUCCESS) {
                HI_ERR_SCI("PPS set:SCI %d is not opened.\n", sci_port);
                up(&g_sci_para[sci_port].sci_sem);
                return HI_FAILURE;
            }
        }

        sci_calc_etu_baud(sci_port, g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate,
                          g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate);
        up(&g_sci_para[sci_port].sci_sem);
        HI_INFO_SCI("PPS negotiation success \n");
    } else {
        HI_ERR_SCI("SCI%d PPS negotiatioan fail,will warm reset... \n", sci_port);
        ret = sci_reset(sci_port, HI_TRUE);
        if (ret) {
            return ret;
        }

        for (times = 0; times < 60; times++) { /* check status 60 times */
            if (g_sci_para[sci_port].sci_state.sci_crt_state >= HI_SCI_STATUS_READY) {
                flag = HI_TRUE;
                break;
            }

            mdelay(50); /* delay 50ms */

            HI_FATAL_SCI("SCI PPS reset count %d!\n", times);
        }

        if (flag) {
            HI_FATAL_SCI("SCI  PPS warm Reset OK!\n");
            return HI_SUCCESS;
        } else {
            HI_ERR_SCI("SCI  PPS warm Reset Fail!\n ");
        }
    }

    return pps_flag;
}

hi_s32 sci_pps_negotiation(hi_sci_port sci_port, hi_u8 *psend_buf, hi_u32 pps_len, hi_u32 rec_timeouts)
{

    hi_s32 ret = HI_SUCCESS;
    hi_u32 send_len;
    hi_u32 rel_send_len = 0;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(psend_buf);

    if (pps_len > SCI_PPS_SEND_RECV_BUF_LEN) {
        return HI_ERR_SCI_INVALID_PARA;
    }
    /* TA2 absent is Negotiation mode */
    if (g_sci_para[sci_port].sci_state.force_flag == HI_FALSE) {
        /* Package PPS command */
        ret = sci_pps_send_package(sci_port, psend_buf, pps_len);
        if (ret != HI_SUCCESS) {
            HI_ERR_SCI("SCI%d  sci_pps_send_package fail \n", sci_port);
            return ret;
        }
        send_len = g_sci_pps[sci_port].send_len;
    }
    /* TA2 present and bit5=0, will direct set TA1 */
    else {
        HI_ERR_SCI(" SCI%d direct set TA1, not need PPS  Negotiation .\n", sci_port);
        return HI_ERR_SCI_PPS_NOTSUPPORT_ERR;
    }

    ret = sci_send_pps_data(sci_port, g_sci_pps[sci_port].send,
                            send_len, &rel_send_len, 5000); /* Send PPS command to card, 5000:send pps data timeout */
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("send PPS pakeage fail,expect send %d,but noly send %d \n", send_len, rel_send_len);
        return ret;
    }

    /* Receive PPS response from card */
    ret = sci_pps_receive(sci_port, rec_timeouts);

    return ret;
}

hi_s32 sci_set_atr_byte(hi_sci_port sci_port, hi_u32 atr_mask)
{

    CHECK_PORTID(sci_port);

    /* TA1 present && TA2 present and bit5=0, will direct set TA1 */
    if ((g_sci_atr_buf[sci_port].data_buf[1] & 0x10) &&
            g_sci_para[sci_port].sci_state.force_flag) {
        HI_FATAL_SCI(" SCI%d driect set TA1.\n", sci_port);
#if defined(HI_ADVCA_TYPE_NAGRA)
        if ((g_sci_clk_rate[g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate] == 0) ||
                (g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate] == 0)) {
            HI_FATAL_SCI("Can not support this TA1 CLK %d Bit %d\n",
                         g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate,
                         g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate);
            return HI_FAILURE;
        } else if ((g_sci_clk_rate[g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate] == 372) && /* 372:clk_rate */
                   (g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate] == 1)) {
            HI_ERR_SCI("Set block 11\n");
            sci_hal_set_block_guard(sci_port, SCI_DFT_BLKGUARD_TIME - 11); /* 11: default guard time 11etu */
        }
#endif
        g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate =
            g_sci_clk_rate[g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate];
        g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate =
            g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate];
        sci_calc_etu_baud(sci_port, g_sci_clk_rate[g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate],
                          g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate]);
        sci_hal_set_char_timeout(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.character_time);
        sci_hal_set_block_timeout(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.block_time);
    }

    /* TC1, set char guard time in opposite direction */
    if (atr_mask & (0x1 << 4)) { /* shift left 4 bit */
        if (g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard < 255) { /* max char guard time 255 */
            if ((HI_SCI_PROTOCOL_T0 == g_sci_para[sci_port].sci_state.sci_atr_para.protocol_type) ||
                    (HI_SCI_PROTOCOL_T14 == g_sci_para[sci_port].sci_state.sci_atr_para.protocol_type)) {
                sci_hal_set_char_guard(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard);
#if defined(HI_ADVCA_TYPE_CONAX)
                sci_hal_set_block_guard(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard);
#endif
            }
        }
    }

    /* set TC2 */
    if (atr_mask & (0x1 << 8)) { /* shift left 8 bit */
        sci_hal_set_char_timeout(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.character_time);
    }

    /* set TB3 */
    if (atr_mask & (0x1 << 11)) { /* shift left 11 bit */
        sci_hal_set_char_timeout(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.character_time);
        sci_hal_set_block_timeout(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.block_time);
    }

    return HI_SUCCESS;
}

hi_s32 sci_atr_process_ts(hi_sci_port sci_port, hi_u8 atr_data)
{
    if (atr_data == SCI_ATR_CONV_NORM) {
        g_sci_para[sci_port].sci_state.sci_data_reversal = HI_FALSE;
    } else if (atr_data == SCI_ATR_CONV_NORM_INV) {
        g_sci_para[sci_port].sci_state.sci_data_reversal = HI_TRUE;
    } else if (atr_data == SCI_ATR_CONV_INV) {
        g_sci_para[sci_port].sci_state.sci_data_reversal = HI_TRUE;
        g_sci_atr_buf[sci_port].data_buf[0] = SCI_ATR_CONV_NORM_INV;
    } else {
        HI_ERR_SCI("sci_atr_process_ts error.\n");
        return HI_FAILURE;
    }

    sci_set_state(sci_port, HI_SCI_STATUS_READATR);

    g_sci_para[sci_port].sci_state.sci_atr_para.grouping = 0;         /* format character */
    g_sci_para[sci_port].sci_state.sci_atr_para.current_ch  = 0;      /* format character */
    g_sci_para[sci_port].sci_state.sci_atr_para.check_sum = 0;        /* initialise the checksum */
    g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask   = 0;
    g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard   = 0;
    g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate = F_372;     /* clock rate conversion */
    g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate = D_1;       /* bit rate adjustment */

    return HI_SUCCESS;
}

hi_void sci_atr_format_ti(hi_sci_port sci_port, hi_u8 atr_data)
{
    hi_u8 tils;

    tils = atr_data & 0xf;

    /*
     * mask of expected characters A-D.
     * Bit 0 = A, Bit 1 = B, Bit 2 = C, Bit 3 = D
     */
    g_sci_para[sci_port].sci_state.sci_atr_para.char_present = (atr_data & 0xf0) >> 4; /* Shift right 4 bit */

    if (g_sci_para[sci_port].sci_state.sci_atr_para.grouping == 0) {
        /* check if it's the T0 character */
        g_sci_para[sci_port].sci_state.sci_atr_para.num_historical = tils; /* number of historical chars */
        g_sci_para[sci_port].sci_state.sci_atr_para.protocol_type  = 0; /* first block always protocol 0 */
        g_sci_para[sci_port].sci_state.sci_atr_para.intf_byte_qual = HI_FALSE;
        g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum = HI_FALSE;
    } else if (g_sci_para[sci_port].sci_state.sci_atr_para.grouping == 1) {
        g_sci_para[sci_port].sci_state.sci_atr_para.protocol_type = tils; /* first offered protocol */
    } else if ((g_sci_para[sci_port].sci_state.sci_atr_para.grouping > 1) && (tils == 15)) { /* 15: historical char */
        g_sci_para[sci_port].sci_state.sci_atr_para.intf_byte_qual = HI_TRUE;
        /* T = 15 indicates gobal interface byte */
        g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum = HI_TRUE; /* we now expect a checksum */
    }

    /* check if this block specifies a non-zero protocol */
    if (g_sci_para[sci_port].sci_state.sci_atr_para.protocol_type == 1) {
        g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum = HI_TRUE; /* we now expect a checksum */
        g_sci_para[sci_port].sci_state.sci_protocol = HI_SCI_PROTOCOL_T1;
        sci_hal_set_block_guard(sci_port, SCI_DFT_BLKGUARD_TIME - MIN_INTERVAL_BETWEEN_CHARACTERS);
    } else if (g_sci_para[sci_port].sci_state.sci_atr_para.protocol_type == 14) { /* protocal type T14 */
        g_sci_para[sci_port].sci_state.sci_protocol = HI_SCI_PROTOCOL_T14;
        g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum = HI_TRUE; /* we now expect a checksum */
    }

    if (!g_sci_para[sci_port].sci_state.sci_atr_para.char_present) {
        /* stop if no chars in next block */
        g_sci_para[sci_port].sci_state.sci_atr_para.grouping = SCI_ATR_GROUP_DONE;
        return;
    }

    g_sci_para[sci_port].sci_state.sci_atr_para.grouping++;          /* next group (i) of characters */
    g_sci_para[sci_port].sci_state.sci_atr_para.current_ch = 0;      /* Character TAi is next */

    return;
}

hi_void sci_atr_param_set(hi_sci_port sci_port, hi_u8 atr_data)
{
    /* Create pointers to the instance data block and the register base */
    hi_u32 atr_grouping, atr_current;
    hi_s32 atr_item;
    hi_u32 calc_para = 960;

    atr_grouping = g_sci_para[sci_port].sci_state.sci_atr_para.grouping;
    atr_current = g_sci_para[sci_port].sci_state.sci_atr_para.current_ch;

    /*
     * The ATR grouping and the current_ch item within the grouping determine
     * how characters are interpreted.  The PL131_ATR_ITEM macro allows us to use a
     * single switch statement
     */
    atr_item = SCI_ATR_ITEM(atr_grouping, atr_current);

    switch (atr_item) {
        /* specify the clock rate conversion and bit rate adjustment factor */
        case 0xA1: { /* character A1 */
            hi_u32 value;
            value = (atr_data & 0xf0) >> 4; /* shift right 4 bit */
            g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate = (atr_data & 0xf0) >> 4; /* shift right 4 bit */
            /* clock rate conversion */
            g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate = atr_data & 0xf;
            /* bit rate adjustment */
            if (g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate > 0) {
                hi_u32 Di;
                hi_u32 Fi;
                Fi = g_sci_clk_rate[g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate];
                Di = g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate];
                if ((Fi > 0) && (Di > 0)) {
                    g_sci_para[sci_port].sci_state.sci_atr_para.character_time =
                        SCI_DFT_CHAR_TIME * Di * 372 / Fi; /* calculation factor 372 */
                    g_sci_para[sci_port].sci_state.sci_atr_para.block_time =
                        SCI_DFT_BLOCK_TIME * Di * 372 / Fi; /* calculation factor 372 */
                } else {
                    g_sci_para[sci_port].sci_state.sci_atr_para.character_time = SCI_DFT_CHAR_TIME;
                    g_sci_para[sci_port].sci_state.sci_atr_para.block_time = SCI_DFT_BLOCK_TIME;
                }
                g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout =
                    g_sci_para[sci_port].sci_state.sci_atr_para.character_time;
                g_sci_para[sci_port].sci_state.sci_setup_para.char_guard =
                    g_sci_para[sci_port].sci_state.sci_atr_para.block_time;
            }
            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 2); /* shift left 2 bit */

#if defined(HI_ADVCA_TYPE_NAGRA)
            if (atr_data == 0x11) {
                sci_hal_set_block_guard(sci_port, SCI_DFT_BLKGUARD_TIME - 11); /* default min blkguard time 11 */
                HI_ERR_SCI("Change blockguard.\n");
            }
#endif
            break;
        }

        /* programa valtage and current_ch ,chip not support */
        case 0xB1: { /* character B1 */
            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 3); /* shift left 3 bit */
            break;
        }

        /* specify the character-character guard time */
        case 0xC1: { /* character C1 */
            g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard = (atr_data & 0xff);

            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 4); /* shift left 4 bit */
            break;
        }

        /* TA2 present is specify mode */
        case 0xA2: {
            if (!(atr_data & 0x10)) {
                g_sci_para[sci_port].sci_state.force_flag = HI_TRUE;
            }

            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 6); /* shift left 6 bit */
            break;
        }

        /* programa valtage use replace TB1 ,chip not support */
        case 0xB2: { /* character B2 */
            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 7); /* shift left 7 bit */
            break;
        }

        /* tow  sequence Char timeout, noly for T0 */
        case 0xC2: { /* character C2 */
            hi_u32 Di;
            Di = g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate];

            if (Di > 0) {
                g_sci_para[sci_port].sci_state.sci_atr_para.character_time = (atr_data & 0xff) * calc_para * Di;
                g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout = (atr_data & 0xff) * calc_para * Di;
            } else {
                g_sci_para[sci_port].sci_state.sci_atr_para.character_time = (atr_data & 0xff) * calc_para;
                g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout = (atr_data & 0xff) * calc_para;
            }

            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 8); /* shift left 8 bit */
            break;
        }

        /* T1 card information area size */
        case 0xA3: { /* character A3 */
            g_sci_para[sci_port].sci_state.sci_atr_para.data_inform_size = (atr_data & 0xff);
            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 10); /* shift left 10 bit */
            break;
        }

        /* T1  CWT and BWT */
        case 0xB3: { /* character B3 */
            if (g_sci_para[sci_port].sci_state.sci_protocol == HI_SCI_PROTOCOL_T1) {
                hi_u32 value;

                value = (atr_data & 0xf0) >> 4; /* shift right 4 bit */

                if (g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate > 0) {
                    hi_u32 Di;
                    hi_u32 Fi;

                    Fi = g_sci_clk_rate[g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate];
                    Di = g_sci_bit_rate[g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate];

#if defined(HI_ADVCA_TYPE_NAGRA)
                        g_sci_para[sci_port].sci_state.sci_atr_para.block_time = ((Fi > 0) && (Di > 0)) ?
                            ((1 << value) * calc_para * 372 * Di / Fi + 12) : /* 372:min block time 12etu */
                            ((1 << value) * calc_para + 12); /* 12:min 12etu */
#else
                        g_sci_para[sci_port].sci_state.sci_atr_para.block_time = ((Fi > 0) && (Di > 0)) ?
                            ((1 << value) * calc_para * 372 * Di / Fi + 11) : /* 372:min block time 11etu */
                            ((1 << value) * calc_para + 11); /* 11:min 11etu */
#endif
                    g_sci_para[sci_port].sci_state.sci_setup_para.char_guard =
                        g_sci_para[sci_port].sci_state.sci_atr_para.block_time;
                }

                value = atr_data & 0xf;
#if defined(HI_ADVCA_TYPE_NAGRA)
                g_sci_para[sci_port].sci_state.sci_atr_para.character_time = (1 << value) + 12; /* 12:default 12etu */
                g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout = (1 << value) + 12; /* 12:default 12etu */
#else
                g_sci_para[sci_port].sci_state.sci_atr_para.character_time = (1 << value) + 11; /* 11:default 11etu */
                g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout = (1 << value) + 11; /* 11:default 11etu */
#endif
                g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 11); /* shift left 11 bit */
            }
            break;
        }

        /* T1 checksum type */
        case 0xC3: { /* character C3 */
            g_sci_para[sci_port].sci_state.sci_atr_para.checksum_type = (atr_data & 0x01);
            g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask |= (0X01 << 12); /* shift left 12 bit */
            break;
        }

        /* notify the user of an ATR item */
        default: {
            break;
        }
    }

    return;
}

hi_s32 sci_atr_process_format(hi_sci_port sci_port, hi_u8 atr_data)
{
    if ((g_sci_para[sci_port].sci_state.sci_atr_para.current_ch == 0) ||
            (g_sci_para[sci_port].sci_state.sci_atr_para.current_ch) == 4) { /* 4: call sci_atr_format_ti function */
        sci_atr_format_ti(sci_port, atr_data);
    }
    /* other character of ATR (TAi, TBi, TCi) */
    else {
        sci_atr_param_set(sci_port, atr_data);
    }

    /*
     * next character of ATR (TAi, TBi, TCi, TDi)
     * find which character (A-D) is expected next
     */
    do {
        g_sci_para[sci_port].sci_state.sci_atr_para.current_ch++;
    } while ((g_sci_para[sci_port].sci_state.sci_atr_para.current_ch < 5) && /* current_ch small than 5 */
             (!(g_sci_para[sci_port].sci_state.sci_atr_para.char_present &
                (1UL << (g_sci_para[sci_port].sci_state.sci_atr_para.current_ch - 1)))));

    if (g_sci_para[sci_port].sci_state.sci_atr_para.current_ch == 5) { /* 5: this block has finished */
        g_sci_para[sci_port].sci_state.sci_atr_para.grouping = SCI_ATR_GROUP_DONE;
        sci_hal_set_tide(sci_port, 1, 1);
        if ((g_sci_para[sci_port].sci_state.sci_atr_para.num_historical == 0) &&
                (g_sci_para[sci_port].sci_state.sci_protocol == HI_SCI_PROTOCOL_T0)) {
            HI_INFO_SCI("SCI_AtrProcessFormat_End.\n");
            sci_atr_process_end(sci_port, atr_data);
        }
    }

    return HI_SUCCESS;
}

hi_s32 sci_atr_process_end(hi_sci_port sci_port, hi_u8 atr_data)
{
    hi_u32 char_guard = 0;
    hi_s32 ret;

    /* --------TODO need wait up getart cmd-------- */
    sci_hal_disable_int(sci_port, SCI_INT_ATRDTOUT);

    /* Guard time */
    if (g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard == 255) { /* ch_guard is 255 */
        if (HI_SCI_PROTOCOL_T0 == g_sci_para[sci_port].sci_state.sci_protocol) {
            char_guard = 0;
        } else {
#if defined(HI_ADVCA_TYPE_NAGRA)
            char_guard = 1;
#endif
        }
    } else if (g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard < 255) { /* ch_guard less than 255 */
        if (HI_SCI_PROTOCOL_T1 == g_sci_para[sci_port].sci_state.sci_protocol) {
            char_guard = g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard + 1;
        } else {
            char_guard = g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard;
        }
    }

    sci_hal_set_char_guard(sci_port, char_guard);

    /* set handshake settings for T0 mode */
    if (g_sci_para[sci_port].sci_state.sci_protocol == HI_SCI_PROTOCOL_T0) {
        sci_hal_set_tx_parity_mode(sci_port, HI_TRUE, SCI_PARITY_EVEN);
        sci_hal_set_rx_parity_mode(sci_port, HI_TRUE, SCI_PARITY_EVEN);
    } else {
        sci_hal_set_tx_parity_mode(sci_port, HI_FALSE, SCI_PARITY_EVEN);
        sci_hal_set_rx_parity_mode(sci_port, HI_FALSE, SCI_PARITY_EVEN);
    }

    /* set data convention for remaining data */
    sci_hal_set_data_reversal(sci_port, g_sci_para[sci_port].sci_state.sci_data_reversal);
    sci_hal_set_atr_timeout_enable(sci_port, HI_FALSE);

    ret = sci_set_atr_byte(sci_port, g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask);
    if (ret == HI_SUCCESS) {
        sci_set_state(sci_port, HI_SCI_STATUS_READY);
    } else {
        sci_set_state(sci_port, HI_SCI_STATUS_ATR_ERROR);
    }

    return HI_SUCCESS;
}

hi_s32 sci_atr_process_history(hi_sci_port sci_port, hi_u8 atr_data)
{
    /* character after historical is the checksum - should be zero */
    if (g_sci_para[sci_port].sci_state.sci_atr_para.num_historical == 0) {
        if (g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum) {
            hi_u32 tmp = g_sci_atr_buf[sci_port].data_len;
            while (tmp-- > 1) {
                g_sci_para[sci_port].sci_state.sci_atr_para.check_sum =
                    (hi_u8)(g_sci_para[sci_port].sci_state.sci_atr_para.check_sum ^
                            g_sci_atr_buf[sci_port].data_buf[tmp]);
            }

            if (g_sci_para[sci_port].sci_state.sci_atr_para.check_sum) {
                /* NOTIFICATION - ATR error */
                HI_ERR_SCI("ATR check sum  %d error.\n", g_sci_para[sci_port].sci_state.sci_atr_para.check_sum);
                sci_set_state(sci_port, HI_SCI_STATUS_ATR_ERROR);
                return HI_FAILURE;
            }
        }
    }

    /*
     * ATR completed - set up parameters
     * finished if reached 0 (or 1 and no checksum), num_historical == 0 have received TCK ,
     * num_historical == 1 not have TCK use forT0
     */
    if (((g_sci_para[sci_port].sci_state.sci_atr_para.num_historical == 0) &&
            g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum) ||
            ((g_sci_para[sci_port].sci_state.sci_atr_para.num_historical == 1) &&
             (!g_sci_para[sci_port].sci_state.sci_atr_para.has_checksum))) {
        sci_atr_process_end(sci_port, atr_data);
    }

    if (g_sci_para[sci_port].sci_state.sci_atr_para.num_historical != 0) {
        g_sci_para[sci_port].sci_state.sci_atr_para.num_historical--; /* next historical character */
    }

    return HI_SUCCESS;
}

hi_s32 sci_atr_process(hi_sci_port sci_port, hi_u8 atr_data)
{
    hi_s32 ret;

    if (g_sci_para[sci_port].sci_state.sci_data_reversal) {
        sci_char_reversal(&atr_data);
    }
#if defined(HI_ADVCA_TYPE_NAGRA)
    if ((g_sci_para[sci_port].sci_state.sci_crt_state  ==  HI_SCI_STATUS_READY) &&
            (g_sci_para[sci_port].sci_state.sci_atr_para.num_historical == 0)) {
        HI_ERR_SCI("error ATR historybyte.atr_data:%#x,drop it!\n", atr_data);
        return 0;
    }

    if ((g_sci_para[sci_port].sci_state.sci_crt_state  ==  HI_SCI_STATUS_ATR_ERROR)) {
        HI_ERR_SCI("Atr is wriong ,drop data 0x%x!\n", atr_data);
        return 0;
    }
#endif
    g_sci_atr_buf[sci_port].data_buf[g_sci_atr_buf[sci_port].data_len] = atr_data;
    g_sci_atr_buf[sci_port].data_len++;

    /* first character of ATR (TS: convention) */
    if (g_sci_para[sci_port].sci_state.sci_atr_para.grouping == SCI_ATR_GROUP_START) {
        ret = sci_atr_process_ts(sci_port, atr_data);
        return ret;
    }
    /* format character for next block (T0 or TDi) */
    else if (g_sci_para[sci_port].sci_state.sci_atr_para.grouping != SCI_ATR_GROUP_DONE) {
        ret = sci_atr_process_format(sci_port, atr_data);
        return ret;
    }
    /* historical data */
    else {
        ret = sci_atr_process_history(sci_port, atr_data);
        return ret;
    }
}

static hi_void sci_init_config_default_para(hi_u32 sci_port)
{
    g_sci_para[sci_port].sci_enable = HI_FALSE;
    g_sci_para[sci_port].sci_attr.sci_protocol = HI_SCI_PROTOCOL_T0;
    g_sci_para[sci_port].sci_attr.frequency = 1000; /* default freq 1000 */
    g_sci_para[sci_port].sci_attr.rx_timeout = 1000; /* default rx_timeout 1000 */
    g_sci_para[sci_port].sci_attr.tx_timeout = 1000; /* default tx_timeout 1000 */
    g_sci_para[sci_port].sci_attr.sci_vcc = HI_SCI_LEVEL_HIGH;
    g_sci_para[sci_port].sci_attr.sci_detect = HI_SCI_LEVEL_HIGH;
    g_sci_para[sci_port].sci_attr.clk_mode = HI_SCI_MODE_OD;
    g_sci_para[sci_port].sci_attr.reset_mode = HI_SCI_MODE_OD;
    g_sci_para[sci_port].sci_attr.vcc_en_mode = HI_SCI_MODE_OD;
    g_sci_para[sci_port].sci_attr.data_mode = HI_SCI_MODE_CMOS;

    g_sci_para[sci_port].err_type = 0;
    g_sci_para[sci_port].sys_clk = SCI_DFT_REF_CLOCK;

    return;
}

hi_void sci_init(hi_void)
{
    hi_u32 i = 0;
    sci_port_attr port_attr = {0};
    sci_reg *sci_ker_addr = HI_NULL;
    sci_tide_reg *sci_tide_addr = HI_NULL;


    for (i = HI_SCI_PORT0; i < HI_SCI_PORT_NUM; i++) {
        sci_init_config_default_para(i);

        HI_OSAL_INIT_MUTEX(&g_sci_para[i].sci_sem);

        init_waitqueue_head(&g_sci_para[i].sci_rec_waitqueue);

        tasklet_init(&g_sci_para[i].sci_read_tasklet, (hi_void *)sci_read_tasklet, i);

        if (sci_get_port_attr(i, &port_attr) != HI_SUCCESS) {
            return;
        }
        sci_ker_addr = (sci_reg *)ioremap_nocache(port_attr.reg_base_addr, sizeof(sci_reg));
        if (sci_ker_addr == HI_NULL) {
            HI_ERR_SCI("sci ioremap_nocache err!\n");
            return ;
        }
        sci_tide_addr = (sci_tide_reg *)ioremap_nocache(port_attr.reg_base_addr + 
                         SCI_TIDE_OFFSET_ADDR, sizeof(sci_tide_reg));
        sci_hal_init(i, sci_ker_addr);
        sci_hal_tide_init(i, sci_tide_addr);
    }

    return;
}

hi_void sci_deinit(hi_void)
{
    hi_u32 i = 0;

    for (i = HI_SCI_PORT0; i < HI_SCI_PORT_NUM; i++) {
        iounmap(g_psci_reg[i]);
        iounmap(g_psci_tide_reg[i]);
    }

    return;
}


hi_s32 sci_open(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency)
{

    hi_s32 ret = HI_FAILURE;
    sci_port_attr port_attr = {0};
    hi_char irq_name[16] = "sci"; /* irq_name buffer length 16 */

    irq_name[3] = '0' + (hi_u32)sci_port % 10; /* irq_name buffer 3 element id%10 */

    g_hsci_irq[sci_port] = (hi_u32)sci_port;

    g_sci_para[sci_port].sci_attr.sci_protocol = sci_protocol;
    g_sci_para[sci_port].sci_attr.frequency = frequency;

    g_sci_para[sci_port].sci_state.sci_protocol = sci_protocol;

    /* enable clk module */
    sci_hal_set_sci_clock(sci_port, HI_TRUE);
    sci_hal_clear_sci_reset(sci_port);

    sci_hal_set_clk_mode(sci_port, g_sci_para[sci_port].sci_attr.clk_mode);
    sci_hal_set_reset_mode(sci_port, g_sci_para[sci_port].sci_attr.reset_mode);
    sci_hal_set_vccen_mode(sci_port, g_sci_para[sci_port].sci_attr.vcc_en_mode);
    sci_hal_set_data_mode(sci_port, g_sci_para[sci_port].sci_attr.data_mode);

    sci_hal_set_vcc(sci_port, g_sci_para[sci_port].sci_attr.sci_vcc);
    sci_hal_set_detect(sci_port, g_sci_para[sci_port].sci_attr.sci_detect);
    sci_hal_disable_all_int(sci_port);
    sci_hal_clear_all_int(sci_port);

    if (sci_hal_get_card_present(sci_port)) {
        sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
    } else {
        sci_set_state(sci_port, HI_SCI_STATUS_NOCARD);
    }

    ret = sci_get_port_attr(sci_port, &port_attr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = hi_drv_osal_request_irq(port_attr.irq_number, sci_isr, IRQF_SHARED,
                                  irq_name, (hi_void *)&g_hsci_irq[sci_port]);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SCI("register SCI%d Isr failed 0x%x.\n", sci_port, ret);
        return HI_FAILURE;
    }

    ret = hi_drv_sys_set_irq_affinity(HI_ID_SCI, port_attr.irq_number, irq_name);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SCI("hi_drv_sys_set_irq_affinity failed.\n");
    }

    sci_hal_enable_int(sci_port, (SCI_INT_CARDIN) | (SCI_INT_CARDOUT));

    g_sci_para[sci_port].sci_enable = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 sci_close(hi_sci_port port_id)
{
    sci_port_attr port_attr = {0};
    hi_char irq_name[16] = "sci"; /* irq_name buffer length 16 */

    irq_name[3] = '0' + (hi_u32)port_id % 10; /* irq_name buffer 3 nd element id %10 */

    g_sci_para[port_id].sci_enable = HI_FALSE;

    sci_hal_disable_all_int(port_id);

    if (sci_get_port_attr(port_id, &port_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    hi_drv_osal_free_irq(port_attr.irq_number, irq_name, (hi_void *)&g_hsci_irq[port_id]);

    /* close clk */
    sci_hal_set_sci_reset(port_id);
    sci_hal_set_sci_clock(port_id, HI_FALSE);

    return HI_SUCCESS;
}

hi_s32 sci_close_ree(hi_sci_port port_id)
{
    sci_port_attr port_attr = {0};
    hi_char irq_name[16] = "sci"; /* irq_name buffer length 16 */

    irq_name[3] = '0' + (hi_u32)port_id % 10; /* irq_name buffer 3 nd element id %10 */

    g_sci_para[port_id].sci_enable = HI_FALSE;

    sci_hal_disable_all_int(port_id);

    if (HI_SUCCESS != sci_get_port_attr(port_id, &port_attr)) {
        return HI_FAILURE;
    }

    hi_drv_osal_free_irq(port_attr.irq_number, irq_name, (hi_void *)&g_hsci_irq[port_id]);

    /* close clk */
    sci_hal_set_sci_reset(port_id);
    sci_hal_set_sci_clock(port_id, HI_FALSE);

    return HI_SUCCESS;
}

hi_void sci_reset_action(hi_sci_port sci_port, hi_bool warm_reset)
{
    if (!warm_reset) {
        sci_hal_finish(sci_port);
        mdelay(10); /* delay 10 ms */

        sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
        sci_hal_start_up(sci_port);
    } else {
        if (g_sci_para[sci_port].sci_state.sci_crt_state > HI_SCI_STATUS_INACTIVECARD) {
            sci_hal_reset(sci_port);
        } else {
            sci_hal_start_up(sci_port);
        }
    }

    sci_set_state(sci_port, HI_SCI_STATUS_WAITATR);
}

hi_s32 sci_reset(hi_sci_port sci_port, hi_bool warm_reset)
{
    hi_s32 ret;

    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    sci_hal_disable_all_int(sci_port);
    sci_hal_clear_all_int(sci_port);

    ret = sci_config_protocol(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_FAILURE;
    }

    g_sci_para[sci_port].err_type = 0;
    g_sci_para[sci_port].sci_state.force_flag = HI_FALSE;
    g_sci_para[sci_port].sci_state.set_extbaud_flag = HI_FALSE;

    sci_clean_buf(sci_port);

    g_sci_atr_buf[sci_port].data_len = 0;
    g_sci_para[sci_port].sci_state.sci_atr_para.grouping = SCI_ATR_GROUP_START;
    g_sci_para[sci_port].sci_state.sci_atr_para.check_sum = 0;
    g_sci_para[sci_port].sci_state.sci_atr_para.atr_mask = 0;
    g_sci_para[sci_port].sci_state.sci_atr_para.character_time = SCI_DFT_WAIT_TIME;
    g_sci_para[sci_port].sci_state.sci_atr_para.block_time = \
        SCI_DFT_BLOCK_TIME - MIN_INTERVAL_BETWEEN_CHARACTERS;

    if (sci_hal_get_card_present(sci_port)) {
        if (!warm_reset) {
            sci_hal_finish(sci_port);
            mdelay(10); /* delay 10 ms */

            sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
            sci_hal_start_up(sci_port);
        } else {
            if (g_sci_para[sci_port].sci_state.sci_crt_state > HI_SCI_STATUS_INACTIVECARD) {
                sci_hal_reset(sci_port);
            } else {
                sci_hal_start_up(sci_port);
            }

            sci_set_state(sci_port, HI_SCI_STATUS_WAITATR);
        }
    } else {
        sci_hal_enable_int(sci_port, (SCI_INT_CARDIN) | (SCI_INT_CARDOUT));
        sci_set_state(sci_port, HI_SCI_STATUS_NOCARD);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_FAILURE;
    }

    sci_hal_enable_int(sci_port, SCI_DFT_INT);
    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_deactive(hi_sci_port sci_port)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (sci_hal_get_card_present(sci_port)) {
        sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
        sci_hal_finish(sci_port);
        mdelay(10); /* delay 10 ms */
    }

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_get_atr(hi_sci_port sci_port, hi_u8 *patr_buf, hi_u32 atr_buf_size, hi_u8 *patr_data_len)
{
    hi_s32 ret;
    hi_u8 atr_tmp_data[SCI_ATR_BUF_LEN] = {0};

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(patr_buf);
    SCI_CHECK_POINTER(patr_data_len);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        *patr_data_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI has not atr.\n");
        *patr_data_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_NO_ATR;
    }

    if (g_sci_atr_buf[sci_port].data_len > atr_buf_size) {
        *patr_data_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_PARA;
    }

    *patr_data_len = g_sci_atr_buf[sci_port].data_len;

    if (memcpy_s((hi_void *)atr_tmp_data, sizeof(atr_tmp_data), (hi_void *)g_sci_atr_buf[sci_port].data_buf,
                 g_sci_atr_buf[sci_port].data_len)) {
        HI_ERR_SCI("memcpy_s failed! \n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_FAILURE;
    }

    if (copy_to_user(patr_buf, atr_tmp_data, *patr_data_len)) {
        HI_ERR_SCI("copy atr data failed.\n");
        *patr_data_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_get_status(hi_sci_port sci_port, hi_sci_status *pstate)
{
    hi_s32 ret;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(pstate);

#if defined(HI_ADVCA_TYPE_CONAX)
    if (g_sci_para[sci_port].err_type == ATR_CHAR_TIMEOUT_ERR) {
        HI_ERR_SCI("atr char timeout,reset!\n");
        g_sci_para[sci_port].err_type = 0;
        sci_reset(sci_port, HI_FALSE);
    }
#endif

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    *pstate = g_sci_para[sci_port].sci_state.sci_crt_state;

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_get_sci_para(hi_sci_port sci_port, sci_para  *psci_para)
{
    hi_u32 ret;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(psci_para);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_FIRSTINIT) {
        HI_ERR_SCI("SCI parameter invalid.\n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_PARA;
    }

    if (memcpy_s(psci_para, sizeof(sci_para), &g_sci_para[sci_port], sizeof(sci_para))) {
        HI_ERR_SCI("memcpy_s failed! \n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_FAILURE;
    }
    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_get_atr_buffer(hi_sci_port sci_port, atr_buffer  *psci_atr_buf)
{
    hi_u32 ret;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(psci_atr_buf);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI ATR has not ready.\n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_NO_ATR;
    }

    if (memcpy_s(psci_atr_buf, sizeof(atr_buffer), &g_sci_atr_buf[sci_port], sizeof(atr_buffer))) {
        HI_ERR_SCI("memcpy_s failed! \n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_FAILURE;
    }
    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_pps_get_rec_buffer(hi_sci_port sci_port, hi_u8 *prec_buf, hi_u32 *pdata_len)
{
    hi_u32 ret;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(prec_buf);
    SCI_CHECK_POINTER(pdata_len);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d has not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI state has not ready.\n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_PPS_NOTSUPPORT_ERR;
    }

    if (memcpy_s(prec_buf, g_sci_pps[sci_port].receive_len, g_sci_pps[sci_port].receive,
                 g_sci_pps[sci_port].receive_len)) {
        HI_ERR_SCI("memccpy_s failed! \n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_FAILURE;
    }
    *pdata_len = g_sci_pps[sci_port].receive_len;

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_output_param(hi_sci_port sci_port, hi_sci_params_ptr psci_out_param)
{
    hi_u32 ret;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }
    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d has not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_WAITATR) {
        HI_ERR_SCI("SCI has not finished init.\n");
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_NOT_INIT;
    }

    psci_out_param->fi = g_sci_para[sci_port].sci_state.sci_atr_para.clk_rate;
    psci_out_param->di = g_sci_para[sci_port].sci_state.sci_atr_para.bit_rate;
    psci_out_param->actal_clk_rate = g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate;
    psci_out_param->actal_bit_rate = g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate;
    psci_out_param->guard_delay = g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard;
    psci_out_param->protocol_type = g_sci_para[sci_port].sci_state.sci_protocol;
    psci_out_param->tx_retries = g_sci_para[sci_port].sci_state.sci_setup_para.tx_retries;

    if (g_sci_para[sci_port].sci_state.sci_protocol == HI_SCI_PROTOCOL_T1) {
        psci_out_param->char_timeouts  = g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout;
        psci_out_param->block_timeouts = g_sci_para[sci_port].sci_state.sci_setup_para.char_guard;
    } else {
        psci_out_param->char_timeouts  = g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout;
        psci_out_param->block_timeouts = 0;
    }

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_config_vcc(hi_sci_port sci_port, hi_sci_level sci_level)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    if (sci_level >= HI_SCI_LEVEL_MAX) {
        HI_ERR_SCI("sci_level(%d) is invalid para\n", sci_level);
        return HI_ERR_SCI_INVALID_PARA;
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    g_sci_para[sci_port].sci_attr.sci_vcc = sci_level;

    sci_hal_set_vcc(sci_port, g_sci_para[sci_port].sci_attr.sci_vcc);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_config_detect(hi_sci_port sci_port, hi_sci_level sci_level)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    if (sci_level >= HI_SCI_LEVEL_MAX) {
        HI_ERR_SCI("sci_level(%d) is invalid para\n", sci_level);
        return HI_ERR_SCI_INVALID_PARA;
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    g_sci_para[sci_port].sci_attr.sci_detect = sci_level;

    sci_hal_set_detect(sci_port, g_sci_para[sci_port].sci_attr.sci_detect);

    msleep(150); /* sleep 150 ms */
    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_config_clk_mode(hi_sci_port sci_port, hi_sci_mode clk_mode)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    if (clk_mode >= HI_SCI_MODE_MAX) {
        HI_ERR_SCI("clk_mode(%d) is invalid para\n", clk_mode);
        return HI_ERR_SCI_INVALID_PARA;
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    g_sci_para[sci_port].sci_attr.clk_mode = clk_mode;

    sci_hal_set_clk_mode(sci_port, g_sci_para[sci_port].sci_attr.clk_mode);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_config_reset_mode(hi_sci_port sci_port, hi_sci_mode reset_mode)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (reset_mode >= HI_SCI_MODE_MAX) {
        HI_ERR_SCI("Invalid param, port:%d, mode:%d\n", sci_port, reset_mode);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_PARA;
    }

    g_sci_para[sci_port].sci_attr.reset_mode = reset_mode;

    sci_hal_set_reset_mode(sci_port, reset_mode);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_config_vcc_en_mode(hi_sci_port sci_port, hi_sci_mode vcc_en_mode)
{
    hi_s32 ret;

    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (vcc_en_mode >= HI_SCI_MODE_MAX) {
        HI_ERR_SCI("Invalid param, port:%d, mode:%d\n", sci_port, vcc_en_mode);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_PARA;
    }

    g_sci_para[sci_port].sci_attr.vcc_en_mode = vcc_en_mode;

    sci_hal_set_vccen_mode(sci_port, vcc_en_mode);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_send_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms)
{
    hi_s32 ret;
    hi_u32 count = 0;
    hi_u32 fifo_len = 0;
    hi_u8 *ptmp_buf = HI_NULL;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(pdata_buf);
    SCI_CHECK_POINTER(pdata_len);

    if (buf_size > SCI_READ_BUF_LEN) {
        HI_ERR_SCI("buf_size is too large\n");
        return HI_ERR_SCI_INVALID_PARA;
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    sci_clean_buf(sci_port);

    g_sci_para[sci_port].sci_attr.tx_timeout = timeout_ms;

    if (copy_from_user((hi_void *)g_sci_send_data_buf[sci_port].data_buf, (hi_void *)pdata_buf, buf_size)) {
        HI_ERR_SCI("copy_from_user err.\n");
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    g_sci_send_data_buf[sci_port].data_len = buf_size;
    g_sci_send_data_buf[sci_port].read = 0;

    sci_hal_set_direction(sci_port, SCI_DIRECTION_TX);
    sci_set_state(sci_port, HI_SCI_STATUS_TX);
    sci_hal_set_tide(sci_port, 1, 0);

    ptmp_buf = g_sci_send_data_buf[sci_port].data_buf;
    for (count = 0; (count < buf_size) && (count < SCI_FIFO_SIZE); count++) {
        sci_hal_write_data(sci_port, *ptmp_buf);
        ptmp_buf++;
        g_sci_send_data_buf[sci_port].read++;
    }
    sci_hal_clear_int(sci_port, SCI_INT_TXTIDE);
    sci_hal_enable_int(sci_port, SCI_INT_TXTIDE);

    if (g_sci_send_data_buf[sci_port].read < g_sci_send_data_buf[sci_port].data_len) {
        isb();
        ret = wait_event_interruptible_timeout(g_sci_para[sci_port].sci_rec_waitqueue,
                                               ((g_sci_send_data_buf[sci_port].read ==
                                                       g_sci_send_data_buf[sci_port].data_len) ||
                                                (g_sci_para[sci_port].sci_state.sci_crt_state != HI_SCI_STATUS_TX)),
                                               g_sci_para[sci_port].sci_attr.tx_timeout * HZ / 1000); /* 1000:factor */
    }

    if ((g_sci_send_data_buf[sci_port].read == g_sci_send_data_buf[sci_port].data_len)
            && (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_TX)) {
        ret = HI_SUCCESS;
    } else {
        ret = HI_ERR_SCI_SEND_ERR;
    }

    sci_hal_disable_int(sci_port, SCI_INT_TXTIDE);
    while (1) {
        fifo_len = sci_hal_get_tx_fifo(sci_port);
        if (fifo_len == 0) {
            break;
        } else {
            msleep(1);
        }
    }
    sci_hal_set_direction(sci_port, SCI_DIRECTION_RX);

    /* Set ready state only if card exist and can work correctly. */
    if (g_sci_para[sci_port].sci_state.sci_crt_state >= HI_SCI_STATUS_READY) {
        sci_set_state(sci_port, HI_SCI_STATUS_READY);
    }

    *pdata_len = g_sci_send_data_buf[sci_port].read;

    up(&g_sci_para[sci_port].sci_sem);
    return ret;
}

hi_s32 sci_send_pps_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms)
{
    hi_s32 ret;
    hi_u32 tmp_len;
    hi_u32 fifo_len;
    hi_u32 sleep_cnt = 0;
    hi_u8 *ptmp_buf = HI_NULL;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(pdata_buf);
    SCI_CHECK_POINTER(pdata_len);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    sci_clean_buf(sci_port);

    g_sci_para[sci_port].sci_attr.tx_timeout = timeout_ms;

    sci_hal_set_direction(sci_port, SCI_DIRECTION_TX);
    sci_set_state(sci_port, HI_SCI_STATUS_TX);

    ptmp_buf = pdata_buf;
    tmp_len = buf_size;

    ret = HI_SUCCESS;

    while (tmp_len) {
        /*
         * Check card exist or not every time.
         * If card cannot work now or card out, return immediately.
         */
        if (g_sci_para[sci_port].sci_state.sci_crt_state != HI_SCI_STATUS_TX) {
            *pdata_len = 0;
            up(&g_sci_para[sci_port].sci_sem);
            return HI_ERR_SCI_SEND_ERR;
        }

        fifo_len = sci_hal_get_tx_fifo(sci_port);

        while ((fifo_len < SCI_FIFO_SIZE) && tmp_len) {
            if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
                *pdata_len = 0;
                up(&g_sci_para[sci_port].sci_sem);
                return HI_ERR_SCI_SEND_ERR;
            }

            sci_hal_write_data(sci_port, *ptmp_buf);

            tmp_len--;
            ptmp_buf++;
            fifo_len = sci_hal_get_tx_fifo(sci_port);
        }

        /* For some error(some comunication error or card out), try timeout.. */
        if (fifo_len == SCI_FIFO_SIZE) {
            continue;
        }

        if (tmp_len) {
            if (sleep_cnt < (g_sci_para[sci_port].sci_attr.tx_timeout / 10)) { /* 10 ms */
                msleep(10); /* sleep 10 ms */
                sleep_cnt++;
            } else {
                HI_ERR_SCI("send timeout\n");
                ret = HI_ERR_SCI_SEND_ERR;
                break;
            }
        }
    }

    while (1) {
        if (fifo_len == 0) {
            break;
        } else {
            msleep(1);
        }
    }
    sci_hal_set_direction(sci_port, SCI_DIRECTION_RX);

    /* Set ready state only if card exist and can work correctly. */
    if (g_sci_para[sci_port].sci_state.sci_crt_state >= HI_SCI_STATUS_READY) {
        sci_set_state(sci_port, HI_SCI_STATUS_READY);
    }

    *pdata_len = buf_size - tmp_len;

    up(&g_sci_para[sci_port].sci_sem);
    return ret;
}

hi_s32 sci_receive_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len, hi_u32 timeout_ms)
{
    hi_s32 ret;
    hi_size_t irq_flags;
    hi_u32 data_len;
    hi_u8 *ptmp_buf = HI_NULL;
    hi_u8 error = 0;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(pdata_buf);
    SCI_CHECK_POINTER(pdata_len);

    if (buf_size > SCI_READ_BUF_LEN) {
        HI_ERR_SCI("buf_size if too long\n");
        return HI_ERR_SCI_INVALID_PARA;
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI %d stat is not ready.\n", sci_port);
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    g_sci_para[sci_port].sci_attr.rx_timeout = timeout_ms;

    sci_set_state(sci_port, HI_SCI_STATUS_RX);

    g_sci_data_buf[sci_port].data_len = buf_size;

    spin_lock_irqsave(&sci_lock, irq_flags);

    if (g_sci_data_buf[sci_port].write >= g_sci_data_buf[sci_port].read) {
        data_len = g_sci_data_buf[sci_port].write - g_sci_data_buf[sci_port].read;
    } else {
        data_len = SCI_READ_BUF_LEN - g_sci_data_buf[sci_port].read + g_sci_data_buf[sci_port].write;
    }

    spin_unlock_irqrestore(&sci_lock, irq_flags);

    if (data_len < buf_size) {
        if (!g_sci_para[sci_port].sci_attr.rx_timeout) {
            error = 1;
            goto rec_out;
        }

        g_sci_para[sci_port].data_enough = HI_FALSE;

        ret = wait_event_interruptible_timeout(g_sci_para[sci_port].sci_rec_waitqueue,
                                               (g_sci_para[sci_port].data_enough
                                                || (g_sci_para[sci_port].sci_state.sci_crt_state
                                                        == HI_SCI_STATUS_NOCARD)
                                                || (g_sci_para[sci_port].err_type != 0)),
                                               g_sci_para[sci_port].sci_attr.rx_timeout * HZ / 1000); /* 1000:factor */

        if (!ret) {
            error = 2; /* error number 2 */
            HI_ERR_SCI("read timeout\n");
            goto rec_out;
        }

        if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_NOCARD) {
            error = 3; /* error number 3 */
            goto rec_out;
        }

        if (g_sci_para[sci_port].err_type != 0) {
            error = 4; /* error number 4 */
            HI_ERR_SCI("error irq.\n");
#if defined(HI_ADVCA_TYPE_NAGRA)
            if (g_sci_para[sci_port].err_type == SCI_INT_BLKTOUT) {
                HI_ERR_SCI("Rcv failed!return!\n");
                g_sci_para[sci_port].err_type = 0;
                up(&g_sci_para[sci_port].sci_sem);
                return HI_FAILURE;
            }
#endif
            g_sci_para[sci_port].err_type = 0;
            goto rec_out;
        }
    }

rec_out:

    spin_lock_irqsave(&sci_lock, irq_flags);

    if (g_sci_data_buf[sci_port].write >= g_sci_data_buf[sci_port].read) {
        data_len = g_sci_data_buf[sci_port].write - g_sci_data_buf[sci_port].read;
    } else {
        data_len = SCI_READ_BUF_LEN - g_sci_data_buf[sci_port].read + g_sci_data_buf[sci_port].write;
    }

    spin_unlock_irqrestore(&sci_lock, irq_flags);

    ptmp_buf = &(g_sci_data_buf[sci_port].data_buf[g_sci_data_buf[sci_port].read]);

    if (data_len < buf_size) {
        *pdata_len = data_len;

        if ((error == 2) || (error == 4)) { /* error number is 2 or 4 */
            ret = HI_ERR_SCI_RECEIVE_ERR;
        } else {
            ret = HI_ERR_SCI_INVALID_OPT;
        }

        HI_ERR_SCI("data %d is less than need %d.\n", data_len, buf_size);
    } else {
        *pdata_len = buf_size;
        ret = HI_SUCCESS;
    }

    if (data_len) {
        if (copy_to_user(pdata_buf, ptmp_buf, *pdata_len)) {
            HI_ERR_SCI("copy_to_user err.\n");
            *pdata_len = 0;
            up(&g_sci_para[sci_port].sci_sem);
            return HI_ERR_SCI_INVALID_OPT;
        }

        spin_lock_irqsave(&sci_lock, irq_flags);

        g_sci_data_buf[sci_port].read = (g_sci_data_buf[sci_port].read + *pdata_len) % SCI_READ_BUF_LEN;

        g_sci_data_buf[sci_port].data_len = 0;

        spin_unlock_irqrestore(&sci_lock, irq_flags);
    }

    up(&g_sci_para[sci_port].sci_sem);
    return ret;
}

hi_s32 sci_receive_pps_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size, hi_u32 *pdata_len,
                            hi_u32 timeout_ms)
{
    hi_s32 ret;
    hi_u32 data_len;
    hi_size_t irq_flags;
    hi_u8 *ptmp_buf = HI_NULL;

    CHECK_PORTID(sci_port);
    SCI_CHECK_POINTER(pdata_buf);
    SCI_CHECK_POINTER(pdata_len);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI %d stat is not ready.\n", sci_port);
        *pdata_len = 0;
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_RECEIVE_ERR;
    }

    g_sci_para[sci_port].sci_attr.rx_timeout = timeout_ms;

    sci_set_state(sci_port, HI_SCI_STATUS_RX);

    g_sci_data_buf[sci_port].data_len = buf_size;

    spin_lock_irqsave(&sci_lock, irq_flags);

    if (g_sci_data_buf[sci_port].write >= g_sci_data_buf[sci_port].read) {
        data_len = g_sci_data_buf[sci_port].write - g_sci_data_buf[sci_port].read;
    } else {
        data_len = SCI_READ_BUF_LEN - g_sci_data_buf[sci_port].read + g_sci_data_buf[sci_port].write;
    }

    spin_unlock_irqrestore(&sci_lock, irq_flags);

    if (data_len < buf_size) {
        if (!g_sci_para[sci_port].sci_attr.rx_timeout) {
            goto rec_out;
        }

        g_sci_para[sci_port].data_enough = HI_FALSE;

        ret = wait_event_interruptible_timeout(g_sci_para[sci_port].sci_rec_waitqueue,
                                               (g_sci_para[sci_port].data_enough
                                                || (g_sci_para[sci_port].sci_state.sci_crt_state
                                                        == HI_SCI_STATUS_NOCARD)
                                                || (g_sci_para[sci_port].err_type != 0)),
                                               g_sci_para[sci_port].sci_attr.rx_timeout * HZ / 1000); /* 1000:factor */

        if (!ret) {
            HI_ERR_SCI("read timeout\n");
            goto rec_out;
        }

        if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_NOCARD) {
            goto rec_out;
        }

        if (g_sci_para[sci_port].err_type != 0) {
            HI_ERR_SCI("error irq.\n");
            g_sci_para[sci_port].err_type = 0;
            goto rec_out;
        }
    }

rec_out:

    spin_lock_irqsave(&sci_lock, irq_flags);

    if (g_sci_data_buf[sci_port].write >= g_sci_data_buf[sci_port].read) {
        data_len = g_sci_data_buf[sci_port].write - g_sci_data_buf[sci_port].read;
    } else {
        data_len = SCI_READ_BUF_LEN - g_sci_data_buf[sci_port].read + g_sci_data_buf[sci_port].write;
    }

    spin_unlock_irqrestore(&sci_lock, irq_flags);

    ptmp_buf = &(g_sci_data_buf[sci_port].data_buf[g_sci_data_buf[sci_port].read]);

    if (data_len < buf_size) {
        *pdata_len = data_len;
        ret = HI_ERR_SCI_RECEIVE_ERR;
        HI_ERR_SCI("data %d is less than need %d.\n", data_len, buf_size);
    } else {
        *pdata_len = buf_size;
        ret = HI_SUCCESS;
    }

    if (data_len) {
        if (memcpy_s(pdata_buf, buf_size, ptmp_buf, *pdata_len)) {
            HI_ERR_SCI("memcpy_s failed! \n");
            ret = HI_FAILURE;
        }
        spin_lock_irqsave(&sci_lock, irq_flags);

        g_sci_data_buf[sci_port].read = (g_sci_data_buf[sci_port].read + *pdata_len) % SCI_READ_BUF_LEN;

        g_sci_data_buf[sci_port].data_len = 0;

        spin_unlock_irqrestore(&sci_lock, irq_flags);
    }

    up(&g_sci_para[sci_port].sci_sem);
    return ret;
}

hi_s32 sci_switch(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    if (sci_protocol >= HI_SCI_PROTOCOL_MAX) {
        HI_ERR_SCI("sci_protocol(%d) is invalid para\n", sci_protocol);
        return HI_ERR_SCI_INVALID_PARA;
    }

    if (sci_protocol == HI_SCI_PROTOCOL_T14) {
        if ((frequency < 1000) || (frequency > 6000)) { /* 1000: T14 protocal type freq min; 6000: T14 freq max */
            HI_ERR_SCI("para u32Frequency is invalid.\n");
            return HI_ERR_SCI_INVALID_PARA;
        }
    } else {
        if ((frequency < 1000) || (frequency > 5000)) { /* 1000: T0/T1 protocal type freq min; 5000: T0/T1 freq max */
            HI_ERR_SCI("para u32Frequency is invalid.\n");
            return HI_ERR_SCI_INVALID_PARA;
        }
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    sci_hal_set_sci_reset(sci_port);
    sci_hal_clear_sci_reset(sci_port);

    g_sci_para[sci_port].sci_attr.sci_protocol = sci_protocol;
    g_sci_para[sci_port].sci_attr.frequency = frequency;

    g_sci_para[sci_port].sci_state.sci_protocol = sci_protocol;

    up(&g_sci_para[sci_port].sci_sem);

    ret = sci_reset(sci_port, HI_FALSE);

    return ret;
}

hi_s32 sci_ext_set_baud(hi_sci_port sci_port, hi_u32 clk_rate, hi_u32 bit_rate)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    if ((clk_rate < 372) || (clk_rate > 2048)) { /* 372: clk_rate min; 2048: clk_rate max */
        HI_ERR_SCI("para u32ClkRate is invalid.\n");
        return HI_ERR_SCI_INVALID_PARA;
    }

    if ((bit_rate < 1) || (bit_rate > 32) || /* 32: max baud factor */
         ((bit_rate != 1) && (bit_rate % 2) != 0)) { /* 2:baud factor is 2 multiple */
        HI_ERR_SCI("para u32BitRate is invalid.\n");
        return HI_ERR_SCI_INVALID_PARA;
    }

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI %d is not opened.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI%d stat is not ready\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_RECEIVE_ERR;
    }

    g_sci_para[sci_port].sci_state.sci_setup_para.clk_rate = clk_rate;
    g_sci_para[sci_port].sci_state.sci_setup_para.bit_rate = bit_rate;
    sci_calc_etu_baud(sci_port, clk_rate, bit_rate);
    g_sci_para[sci_port].sci_state.set_extbaud_flag = HI_TRUE;

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_set_char_guard(hi_sci_port sci_port, hi_u32 ext_char_guard)
{
    hi_s32 ret;
    hi_u32 char_guard = 0;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI%d is not open.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI%d is not ready\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_RECEIVE_ERR;
    }

    if (ext_char_guard == 255) { /* max ext_char_guard 255 etu */
        char_guard = 0;
    } else if (ext_char_guard < 255) { /* max ext_char_guard 255 etu */
        if (g_sci_para[sci_port].sci_state.sci_protocol == HI_SCI_PROTOCOL_T1) {
            char_guard = ext_char_guard + 1;
        } else {
            char_guard = ext_char_guard;
        }
    }

    g_sci_para[sci_port].sci_state.sci_atr_para.ch_guard = char_guard;
    sci_hal_set_char_guard(sci_port, char_guard);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_set_char_timeout(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 char_timeouts)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI%d has not open.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI%d has not ready\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_NO_ATR;
    }

    if (sci_protocol != g_sci_para[sci_port].sci_state.sci_protocol) {
        HI_ERR_SCI("SCI%d :Expect according to protocol set char_timeouts \
                   conflict with current_ch card protocol\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_PARA;
    }

    g_sci_para[sci_port].sci_state.sci_setup_para.char_timeout = char_timeouts;
    sci_hal_set_char_timeout(sci_port, char_timeouts);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_set_block_timeout(hi_sci_port sci_port, hi_u32 block_timeouts)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI%d has not open.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI%d has not ready\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_NO_ATR;
    }

    if (g_sci_para[sci_port].sci_state.sci_protocol != HI_SCI_PROTOCOL_T1) {
        HI_ERR_SCI("SCI%d : set char_guard noly for T1 protocol\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_PARA;
    }

    g_sci_para[sci_port].sci_state.sci_setup_para.char_guard = block_timeouts;
    sci_hal_set_block_timeout(sci_port, block_timeouts);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_s32 sci_set_tx_retry(hi_sci_port sci_port, hi_u32 tx_retry_times)
{
    hi_s32 ret;
    CHECK_PORTID(sci_port);

    ret = down_interruptible(&g_sci_para[sci_port].sci_sem);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    if (!g_sci_para[sci_port].sci_enable) {
        HI_ERR_SCI("SCI%d has not open.\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_INVALID_OPT;
    }

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        HI_ERR_SCI("SCI%d has not ready\n", sci_port);
        up(&g_sci_para[sci_port].sci_sem);
        return HI_ERR_SCI_NO_ATR;
    }

    g_sci_para[sci_port].sci_state.sci_setup_para.tx_retries = tx_retry_times;
    sci_hal_set_retry(sci_port, g_sci_para[sci_port].sci_state.sci_setup_para.rx_retries, tx_retry_times);

    up(&g_sci_para[sci_port].sci_sem);
    return HI_SUCCESS;
}

hi_void sci_read_tasklet(hi_sci_port sci_port)
{
    hi_u8 sci_data;
    unsigned long flags;

    if (g_sci_para[sci_port].sci_state.sci_crt_state < HI_SCI_STATUS_READY) {
        do {
            spin_lock_irqsave(&sci_lock, flags);
            if (g_sci_data_buf[sci_port].write == g_sci_data_buf[sci_port].read) {
                spin_unlock_irqrestore(&sci_lock, flags);
                return;
            }

            spin_unlock_irqrestore(&sci_lock, flags);
            sci_data = g_sci_data_buf[sci_port].data_buf[g_sci_data_buf[sci_port].read];

            if (sci_data == 0x3b) {
                HI_INFO_SCI("recv atr data 0x3b\n");
            }
            g_sci_data_buf[sci_port].read = (g_sci_data_buf[sci_port].read + 1) % SCI_READ_BUF_LEN;
        } while (sci_atr_process(sci_port, sci_data) == 0);

        if (sci_hal_get_card_present(sci_port)) {
            sci_hal_finish(sci_port);
            sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
        }
    }

    return;
}

irqreturn_t sci_isr(hi_s32 irq, hi_void *dev_id)
{
    hi_u8 ii = 0;
    hi_u32 int_state;
    sci_port_attr port_attr = {0};
    hi_sci_port sci_port;

    SCI_CHECK_POINTER(dev_id);
    sci_port = *(hi_sci_port *)dev_id;


    for (ii = HI_SCI_PORT0; ii < HI_SCI_PORT_NUM; ii++) {
        if (sci_get_port_attr(ii, &port_attr) != HI_SUCCESS) {
            return IRQ_HANDLED;
        }

        if (ii == sci_port) {
            break;
        }
    }

    int_state = sci_hal_get_int_state(sci_port);

    while (int_state) {
        if (int_state & SCI_INT_CARDIN) {
            HI_INFO_SCI("sci%d card in\n", sci_port);
            sci_hal_clear_int(sci_port, SCI_INT_CARDIN);
            sci_card_in(sci_port);
        }

        if (int_state & SCI_INT_CARDOUT) {
            HI_INFO_SCI("sci%d card out\n", sci_port);
            sci_hal_clear_int(sci_port, SCI_INT_CARDOUT);
            sci_card_out(sci_port);
        }

        if (int_state & SCI_INT_CARDDOWN) {
            HI_INFO_SCI("sci%d card down\n", sci_port);
            sci_hal_clear_int(sci_port, SCI_INT_CARDDOWN);
            sci_card_down(sci_port);
        }

        if (int_state & SCI_INT_CARDUP) {
            HI_INFO_SCI("sci%d card up\n", sci_port);
            sci_hal_clear_int(sci_port, SCI_INT_CARDUP);
            sci_set_state(sci_port, HI_SCI_STATUS_WAITATR);
        }

        if (int_state & SCI_INT_RXTIDE) {
            sci_hal_clear_int(sci_port, SCI_INT_RXTIDE);
            sci_rx_tide(sci_port);
        }

        if (int_state & SCI_INT_ATRSTOUT) {
#if defined(HI_ADVCA_TYPE_CONAX)|| defined (HI_ADVCA_TYPE_NAGRA)
            sci_hal_finish(sci_port);
#endif
            sci_hal_clear_int(sci_port, SCI_INT_ATRSTOUT);
            HI_WARN_SCI("sci%d ATR start timeout.\n", sci_port);
            sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
        }

        if (int_state & SCI_INT_ATRDTOUT) {
            sci_hal_clear_int(sci_port, SCI_INT_ATRDTOUT);
            HI_ERR_SCI("sci%d ATR duration timeout.\n", sci_port);

        }

        if (int_state & SCI_INT_BLKTOUT) {
            sci_hal_clear_int(sci_port, SCI_INT_BLKTOUT);
#if defined(HI_ADVCA_TYPE_NAGRA)
            g_sci_para[sci_port].err_type = SCI_INT_BLKTOUT;
#endif
            HI_WARN_SCI("sci%d block transfer timeout.\n", sci_port);
        }

        if (int_state & SCI_INT_CHTOUT) {
#if defined(HI_ADVCA_TYPE_NAGRA)
            if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_READATR) {
                sci_hal_finish(sci_port);
            }
#endif
            sci_hal_clear_int(sci_port, SCI_INT_CHTOUT);
            HI_WARN_SCI("sci%d character transfer timeout.\n", sci_port);
#if defined(HI_ADVCA_TYPE_NAGRA)
            if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_READATR) {
                sci_set_state(sci_port, HI_SCI_STATUS_INACTIVECARD);
            }
#endif

#if defined(HI_ADVCA_TYPE_CONAX)
            if (g_sci_para[sci_port].sci_state.sci_crt_state == HI_SCI_STATUS_READATR) {
                g_sci_para[sci_port].err_type = ATR_CHAR_TIMEOUT_ERR;
            }
#endif
        }

        if (int_state & SCI_INT_RORI) {
            sci_hal_clear_int(sci_port, SCI_INT_RORI);
            HI_ERR_SCI("sci%d receive fifo overflow\n", sci_port);
            g_sci_para[sci_port].err_type = SCI_INT_RORI;
            wake_up_interruptible(&g_sci_para[sci_port].sci_rec_waitqueue);
        }

        if (int_state & SCI_INT_TXTIDE) {
            HI_INFO_SCI("sci%d fifo overflow\n", sci_port);
            sci_push_data(sci_port);
            sci_hal_clear_int(sci_port, SCI_INT_TXTIDE);
        }

        int_state = sci_hal_get_int_state(sci_port);
    }

    return IRQ_HANDLED;
}

hi_s32 sci_suspend(hi_void)
{
    hi_u32 index;

    for (index = HI_SCI_PORT0; index < HI_SCI_PORT_NUM; index++) {
        if (g_sci_para[index].sci_enable) {
            g_suspend_force_flag[index] = g_sci_para[index].sci_state.force_flag;
            g_suspend_set_ext_baud_flag[index] = g_sci_para[index].sci_state.set_extbaud_flag;
            g_suspend_sci_protocol[index] = g_sci_para[index].sci_attr.sci_protocol;
            g_suspend_clk_rate[index]  = g_sci_para[index].sci_state.sci_setup_para.clk_rate;
            g_suspend_bit_rate[index]  = g_sci_para[index].sci_state.sci_setup_para.bit_rate;
            g_suspend_sci_state[index] = g_sci_para[index].sci_state.sci_crt_state;

            sci_hal_disable_all_int(index);
            sci_hal_finish(index);
            sci_set_state(index, HI_SCI_STATUS_INACTIVECARD);
        }
    }

    return HI_SUCCESS;
}

hi_s32 sci_resume(hi_void)
{
    hi_u32 index;
    hi_u32 times;
    hi_bool flag = HI_FALSE;
    hi_s32 ret;

    for (index = HI_SCI_PORT0; index < HI_SCI_PORT_NUM; index++) {
        if (g_sci_para[index].sci_enable) {
            sci_hal_set_sci_clock(index, HI_TRUE);
            sci_hal_clear_sci_reset(index);

            sci_config_clk_mode(index, g_sci_para[index].sci_attr.clk_mode);
            sci_config_reset_mode(index, g_sci_para[index].sci_attr.reset_mode);
            sci_config_vcc_en_mode(index, g_sci_para[index].sci_attr.vcc_en_mode);

            sci_config_vcc(index, g_sci_para[index].sci_attr.sci_vcc);
            sci_config_detect(index, g_sci_para[index].sci_attr.sci_detect);

            g_sci_para[index].sci_state.sci_crt_state = g_suspend_sci_state[index];
            g_sci_para[index].sci_attr.sci_protocol = g_suspend_sci_protocol[index];

            ret = sci_get_status(index, &g_sci_para[index].sci_state.sci_crt_state);
            if (ret != HI_SUCCESS) {
                HI_ERR_SCI("Resume sci_get_status Fail !\n");
                return HI_FAILURE;
            }

            HI_FATAL_SCI("SCI%d  state before Resume reset is %d !\n",
                         index, g_sci_para[index].sci_state.sci_crt_state);

            if (g_sci_para[index].sci_state.sci_crt_state <= HI_SCI_STATUS_NOCARD) {
                sci_hal_disable_all_int(index);
                sci_hal_clear_all_int(index);
                sci_hal_enable_int(index, SCI_DFT_INT);
                HI_FATAL_SCI("Not inset Card, don't  Resume sci!\n");
                return HI_FAILURE;
            }

            ret = sci_reset(index, HI_FALSE);
            if (ret != HI_SUCCESS) {
                HI_ERR_SCI("Resume sci_reset Fail !\n");
                return HI_FAILURE;
            }

            for (times = 0; times < 60; times++) { /* 60 times */
                if (g_sci_para[index].sci_state.sci_crt_state >= HI_SCI_STATUS_READY) {
                    flag = HI_TRUE;
                    break;
                }

                mdelay(50); /* delay 50ms */

                HI_FATAL_SCI("SCI Resume reset count %d!\n", times);
            }
            if (flag) {
                HI_FATAL_SCI("SCI Resume cold Reset OK!\n");

                g_sci_para[index].sci_state.force_flag = g_suspend_force_flag[index];
                g_sci_para[index].sci_state.set_extbaud_flag = g_suspend_set_ext_baud_flag[index];
                g_sci_para[index].sci_state.sci_setup_para.clk_rate = g_suspend_clk_rate[index];
                g_sci_para[index].sci_state.sci_setup_para.bit_rate = g_suspend_bit_rate[index];
                sci_calc_etu_baud(index, g_sci_para[index].sci_state.sci_setup_para.clk_rate,
                                  g_sci_para[index].sci_state.sci_setup_para.bit_rate);
            } else {
                HI_FATAL_SCI("SCI Resume cold Reset Fail!\n");
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_sci_reset(hi_sci_port sci_port, hi_bool warm_reset)
{
    hi_s32 ret;

    ret = sci_reset(sci_port, warm_reset);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_reset failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_send_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size,
                            hi_u32 *pdata_len, hi_u32 timeout_ms)
{
    hi_s32 ret;

    SCI_CHECK_POINTER(pdata_buf);
    SCI_CHECK_POINTER(pdata_len);

    ret = sci_send_data(sci_port, pdata_buf, buf_size, pdata_len, timeout_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_send_data failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_receive_data(hi_sci_port sci_port, hi_u8 *pdata_buf, hi_u32 buf_size,
                               hi_u32 *pdata_len, hi_u32 timeout_ms)
{
    hi_s32 ret;

    SCI_CHECK_POINTER(pdata_buf);
    SCI_CHECK_POINTER(pdata_len);

    ret = sci_receive_data(sci_port, pdata_buf, buf_size, pdata_len, timeout_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_receive_data failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_config_vcc(hi_sci_port sci_port, hi_sci_level sci_level)
{
    hi_s32 ret;

    ret = sci_config_vcc(sci_port, sci_level);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_config_vcc failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_config_detect(hi_sci_port sci_port, hi_sci_level sci_level)
{
    hi_s32 ret;

    ret =  sci_config_detect(sci_port, sci_level);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_config_detect failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_config_clk_mode(hi_sci_port sci_port, hi_sci_mode clk_mode)
{
    hi_s32 ret;

    ret = sci_config_clk_mode(sci_port, clk_mode);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_config_clk_mode failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_get_sci_para(hi_sci_port sci_port, hi_sci_params_ptr psci_para)
{
    hi_s32 ret;

    SCI_CHECK_POINTER(psci_para);

    ret = sci_output_param(sci_port, psci_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_get_sci_para failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_get_atr_buffer(hi_sci_port sci_port, hi_drv_atr_buf  *psci_atr_buf)
{
    hi_s32 ret;

    SCI_CHECK_POINTER(psci_atr_buf);

    ret = sci_get_atr_buffer(sci_port, (atr_buffer *)psci_atr_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_deactive failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_deactive(hi_sci_port sci_port)
{
    hi_s32 ret;

    ret = sci_deactive(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_deactive failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_get_status(hi_sci_port sci_port, hi_sci_status *pstate)
{
    hi_s32 ret;

    SCI_CHECK_POINTER(pstate);

    ret = sci_get_status(sci_port, pstate);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_get_status failed! \n");
    }
    return ret;
}

hi_s32 hi_drv_sci_open(hi_sci_port sci_port, hi_sci_protocol sci_protocol, hi_u32 frequency)
{
    hi_s32 ret;
    ret = sci_open(sci_port, sci_protocol, frequency);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_open failed! \n");
    }

    return ret;
}

hi_s32 hi_drv_sci_close(hi_sci_port sci_port)
{
    hi_s32 ret;

    ret = sci_close(sci_port);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("hi_drv_sci_close failed! \n");
    }

    return ret;
}

hi_void hi_drv_sci_init(hi_void)
{
    sci_init();
    return;
}

EXPORT_SYMBOL(hi_drv_sci_reset);
EXPORT_SYMBOL(hi_drv_sci_send_data);
EXPORT_SYMBOL(hi_drv_sci_receive_data);
EXPORT_SYMBOL(hi_drv_sci_config_vcc);
EXPORT_SYMBOL(hi_drv_sci_config_detect);
EXPORT_SYMBOL(hi_drv_sci_config_clk_mode);
EXPORT_SYMBOL(hi_drv_sci_get_sci_para);
EXPORT_SYMBOL(hi_drv_sci_get_atr_buffer);
EXPORT_SYMBOL(hi_drv_sci_deactive);
EXPORT_SYMBOL(hi_drv_sci_get_status);
EXPORT_SYMBOL(hi_drv_sci_open);
EXPORT_SYMBOL(hi_drv_sci_close);
EXPORT_SYMBOL(hi_drv_sci_init);

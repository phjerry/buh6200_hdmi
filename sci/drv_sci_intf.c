/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description: the interface of sci driver
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/compat.h>

#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "drv_sci.h"

#include "hi_module.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "drv_sci_ext.h"
#include "hi_drv_osal.h"
#include "hi_drv_sys.h"
#include "linux/hisilicon/securec.h"

#define PROC_NAME_LEN 8

static atomic_t g_sci_count = ATOMIC_INIT(0);
HI_OSAL_DECLARE_MUTEX(g_sci_mutex);

static hi_u32 g_sci_open_num[HI_SCI_PORT_MAX];

#ifdef HI_PROC_SUPPORT

static hi_char g_card_state_string[][20] = { "UNINIT", /* 20: array g_card_state_string member max length */
                                             "FIRSTINIT",
                                             "NOCARD",
                                             "INACTIVECARD",
                                             "WAITATR",
                                             "READATR",
                                             "READY",
                                             "RX",
                                             "TX"
};

static hi_char g_protocol_string[][5] = { "T=0", "T=1", "T=14" }; /* 5: array g_protocol_string member max length */
static hi_char g_clock_mode_string[][5] = { "CMOS", "OD" }; /* 5: array g_clock_mode_string member max length */

hi_void sci_dsp_proc(struct seq_file *p, sci_para *psci_para, atr_buffer *psci_atr_buf, hi_u32 get_atr_ret)
{
    hi_u8 atr_mask = 0x0;
    hi_u8 atr_fi = 0x1;
    hi_u8 atr_di = 0x1;
    hi_u32 atr_num = 0;
    hi_u32 actual_sci_clk;

    if (p == HI_NULL || psci_para == HI_NULL || psci_atr_buf == HI_NULL) {
        HI_ERR_SCI("pointer is NULL ! \n");
        return;
    }

    HI_PROC_PRINT(p, "%-20s:", "ATR");
    for (atr_num = 0; atr_num <= psci_atr_buf->data_len - 1; atr_num++) {
        HI_PROC_PRINT(p, "0x%x ", psci_atr_buf->data_buf[atr_num]);
    }

    atr_num = 0;
    HI_PROC_PRINT(p, "\n%-20s:0x%x\n", "TS", psci_atr_buf->data_buf[atr_num++]);
    HI_PROC_PRINT(p, "%-20s:0x%x\n", "T0", psci_atr_buf->data_buf[atr_num]);

    /* interface byte */
    atr_mask = (psci_atr_buf->data_buf[atr_num] & 0xf0) >> 3; /* shift right 3 bit */
    if ((atr_mask >> 1) & 0x01) {
        HI_PROC_PRINT(p, "%-20s:0x%x", "TA1", psci_atr_buf->data_buf[++atr_num]);
        atr_di = psci_atr_buf->data_buf[atr_num] & 0x0f;
        atr_fi = (psci_atr_buf->data_buf[atr_num] & 0xf0) >> 4; /* shift right 4 bit */
        HI_PROC_PRINT(p, " (FI = %u,DI = %u)\n", g_sci_clk_rate[atr_fi], g_sci_bit_rate[atr_di]);
    }

    if ((atr_mask >> 2) & 0x01) { /* shift right 2 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x\n", "TB1", psci_atr_buf->data_buf[++atr_num]);
    }

    if ((atr_mask >> 3) & 0x01) { /* shift right 3 bit */
        ++atr_num;
        HI_PROC_PRINT(p, "%-20s:0x%x (N = %d)\n", "TC1", psci_atr_buf->data_buf[atr_num],
                      psci_atr_buf->data_buf[atr_num]);
    }

    if ((atr_mask >> 4) & 0x01) { /* shift right 4 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x\n", "TD1", psci_atr_buf->data_buf[++atr_num]);
        atr_mask = (psci_atr_buf->data_buf[atr_num] & 0xf0) >> 3; /* shift right 3 bit */
    } else {
        atr_mask = 0x0;
    }

    if ((atr_mask >> 1) & 0x01) {
        HI_PROC_PRINT(p, "%-20s:0x%x (present indicate is specific mode)\n", "TA2", psci_atr_buf->data_buf[++atr_num]);
    }

    if ((atr_mask >> 2) & 0x01) { /* shift right 2 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x\n", "TB2", psci_atr_buf->data_buf[++atr_num]);
    }

    if ((atr_mask >> 3) & 0x01) { /* shift right 3 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x (noly for T0 show IC card max char timeout)\n", "TC2",
                      psci_atr_buf->data_buf[++atr_num]);
    }

    if ((atr_mask >> 4) & 0x01) { /* shift right 4 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x\n", "TD2", psci_atr_buf->data_buf[++atr_num]);
        atr_mask = (psci_atr_buf->data_buf[atr_num] & 0xf0) >> 3; /* shift right 3 bit */
    } else {
        atr_mask = 0x0;
    }

    if ((atr_mask >> 1) & 0x01) {
        HI_PROC_PRINT(p, "%-20s:0x%x (INF length)\n", "TA3", psci_atr_buf->data_buf[++atr_num]);
    }

    if ((atr_mask >> 2) & 0x01) { /* shift right 2 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x (b5~b8:BWI  b1~b4:CWI)\n", "TB3", psci_atr_buf->data_buf[++atr_num]);
    }

    if ((atr_mask >> 3) & 0x01) { /* shift right 3 bit */
        HI_PROC_PRINT(p, "%-20s:0x%x\n", "TC3", psci_atr_buf->data_buf[++atr_num]);
    }

    /* History Byte */
    ++atr_num;
    if (psci_para->sci_attr.sci_protocol == HI_SCI_PROTOCOL_T0) {
        HI_PROC_PRINT(p, "%-20s:", "HistoryByte");
        while (atr_num <= psci_atr_buf->data_len - 1) {
            HI_PROC_PRINT(p, "0x%x ", psci_atr_buf->data_buf[atr_num++]);
        }

        HI_PROC_PRINT(p, "\n");
    } else {
        HI_PROC_PRINT(p, "%-20s:", "HistoryByte");
        while (atr_num <= psci_atr_buf->data_len - 2) { /* when atr_num smller than psci_atr_buf->data_len - 2 */
            HI_PROC_PRINT(p, " 0x%x", psci_atr_buf->data_buf[atr_num++]);
        }

        HI_PROC_PRINT(p, "\n%-20s:0x%x\n", "TCK", psci_atr_buf->data_buf[atr_num]);
    }

    actual_sci_clk = psci_para->sys_clk / (psci_para->sci_state.sci_setup_para.clock_icc + 1) / 2; /* 2: calc factor */

    HI_PROC_PRINT(p, "%-20s:%u\n", "actual_sci_clk", actual_sci_clk);
    HI_PROC_PRINT(p, "%-20s:%u\n", "ExpectBaudRate", psci_para->sci_state.sci_setup_para.bit_rate
                  * psci_para->sci_attr.frequency * 1000 /* 1000: calculation factor */
                  / psci_para->sci_state.sci_setup_para.clk_rate);
    HI_PROC_PRINT(p, "%-20s:%u \n", "CalcBaudFlag", psci_para->sci_state.force_flag);
    HI_PROC_PRINT(p, "%-20s:%u \n", "set_extbaud_flag", psci_para->sci_state.set_extbaud_flag);
    HI_PROC_PRINT(p, "%-20s:%u\n", "clk_rate(F)", psci_para->sci_state.sci_setup_para.clk_rate);
    HI_PROC_PRINT(p, "%-20s:%u\n", "bit_rate(D)", psci_para->sci_state.sci_setup_para.bit_rate);
    HI_PROC_PRINT(p, "%-20s:%u\n", "BaudRate", psci_para->sci_state.sci_setup_para.bit_rate *
                  actual_sci_clk * 1000 / psci_para->sci_state.sci_setup_para.clk_rate); /* 1000: calculation factor */
    HI_PROC_PRINT(p, "%-20s:%u etu\n", "add_char_guard", psci_para->sci_state.sci_atr_para.ch_guard);
    HI_PROC_PRINT(p, "%-20s:%u etu\n", "block_guard", psci_para->sci_state.sci_setup_para.block_guard);
    HI_PROC_PRINT(p, "%-20s:%u \n", "Value", psci_para->sci_state.sci_setup_para.etu_value);
    HI_PROC_PRINT(p, "%-20s:%u \n", "Baud", psci_para->sci_state.sci_setup_para.baud_value);
    if (psci_para->sci_attr.sci_protocol == HI_SCI_PROTOCOL_T1) {
        HI_PROC_PRINT(p, "%-20s:%u \n", "char_guard", psci_para->sci_state.sci_setup_para.char_guard);
        HI_PROC_PRINT(p, "%-20s:%u \n", "block_timeout", psci_para->sci_state.sci_setup_para.block_timeout);
    } else {
        HI_PROC_PRINT(p, "%-20s:%u \n", "char_guard", psci_para->sci_state.sci_setup_para.char_guard);
    }

    return;
}

hi_s32 sci_proc_read(struct seq_file *p, hi_void *v)
{
    hi_proc_item    *proc_item = HI_NULL;
    hi_sci_port sci_port = HI_SCI_PORT0;
    hi_s32 ret;
    sci_para sci_para;
    atr_buffer sci_atr_buf = {{0}};
    hi_u8 i = 0;

    SCI_CHECK_POINTER(p);

    proc_item = p->private;

    for (i = HI_SCI_PORT0; i < HI_SCI_PORT_NUM; i++) {
        hi_char name[PROC_NAME_LEN] = "sci";

        name[3] = '0' + i % 10; /* buffer name 3 nd element i%10 */

        if (strncmp(proc_item->entry_name, name, PROC_NAME_LEN) == 0) {
            sci_port = i;
            break;
        }
    }

    HI_PROC_PRINT(p, "---------Hisilicon SCI%d Info---------\n", sci_port);

    if (g_sci_open_num[sci_port] != 0) {
        ret = sci_get_sci_para(sci_port, &sci_para);
        if (ret != HI_SUCCESS) {
            HI_FATAL_SCI("sci_proc_read SCI%u sci_get_sci_para failed.\n", sci_port);
            return ret;
        }

        HI_PROC_PRINT(p, "%-20s:%s\n", "Sci State", g_card_state_string[sci_para.sci_state.sci_crt_state]);
        HI_PROC_PRINT(p, "%-20s:%d\n", "SetFrequency", sci_para.sci_attr.frequency);
        HI_PROC_PRINT(p, "%-20s:%s\n", "Protocol", g_protocol_string[sci_para.sci_attr.sci_protocol]);
        HI_PROC_PRINT(p, "%-20s:%d\n", "VccEnLevel", sci_para.sci_attr.sci_vcc);
        HI_PROC_PRINT(p, "%-20s:%d\n", "DetectLevel", sci_para.sci_attr.sci_detect);
        HI_PROC_PRINT(p, "%-20s:%s\n", "ClockMode", g_clock_mode_string[sci_para.sci_attr.clk_mode]);
        HI_PROC_PRINT(p, "%-20s:%s\n", "ResetMode", g_clock_mode_string[sci_para.sci_attr.reset_mode]);
        HI_PROC_PRINT(p, "%-20s:%s\n", "VccMode", g_clock_mode_string[sci_para.sci_attr.vcc_en_mode]);

        if (sci_para.sci_state.sci_crt_state >= HI_SCI_STATUS_READATR) {
            ret = sci_get_atr_buffer(sci_port, &sci_atr_buf);
            if (ret != HI_SUCCESS) {
                HI_FATAL_SCI("sci_proc_read SCI%u sci_get_atr_buffer failed.\n", sci_port);
                return ret;
            }

            sci_dsp_proc(p, &sci_para, &sci_atr_buf, ret);
        }

        return HI_SUCCESS;
    } else {
        HI_PROC_PRINT(p, "SCI%d is not open\n", sci_port);
        return HI_SUCCESS;
    }
}

#endif


hi_s32 sci_ext_open(sci_open_para *psci_open, sci_dev_state *psci_dev_state)
{
    hi_s32 ret = HI_SUCCESS;

    SCI_CHECK_POINTER(psci_open);
    SCI_CHECK_POINTER(psci_dev_state);

    if (psci_open->sci_port >= HI_SCI_PORT_NUM) {
        return HI_FAILURE;
    }
    /* first open sci0 will execute sci_open_para initialize sci0 config */
    if (!psci_dev_state->sci[psci_open->sci_port] && !g_sci_open_num[psci_open->sci_port]) {
            ret = sci_open(psci_open->sci_port, psci_open->sci_protocol, psci_open->frequency);
            if (ret != HI_SUCCESS) {
                HI_FATAL_SCI("call sci_open failed.\n");
                return ret;
            }

            psci_dev_state->sci[psci_open->sci_port] = HI_TRUE;
            g_sci_open_num[psci_open->sci_port]++;
    }

    return ret;
}

hi_s32 sci_ext_close(hi_sci_port sci_port, sci_dev_state *psci_dev_state)
{
    hi_s32 ret = HI_SUCCESS;

    SCI_CHECK_POINTER(psci_dev_state);
    /* close SCI0 */
    if (sci_port < HI_SCI_PORT_NUM) {
        psci_dev_state->sci[sci_port] = HI_FALSE;
        g_sci_open_num[sci_port]--;

        if (!g_sci_open_num[sci_port]) {
            ret = sci_close(sci_port);
            if (ret != HI_SUCCESS) {
                HI_FATAL_SCI("call sci_close failed.\n");
            }
        }
    }

    return ret;
}

hi_s32 sci_check_state(hi_sci_port sci_port, sci_dev_state *psci_dev_state)
{
    SCI_CHECK_POINTER(psci_dev_state);

    if ((sci_port < HI_SCI_PORT_NUM) && psci_dev_state->sci[sci_port]) {
        return HI_SUCCESS;
    }

    return HI_ERR_SCI_INVALID_PARA;
}

hi_slong sci_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    sci_dev_state *psci_dev_state = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    SCI_CHECK_POINTER(file);
    SCI_CHECK_POINTER(arg);

    psci_dev_state = file->private_data;

    if (down_interruptible(&g_sci_mutex)) {
        return -EAGAIN;
    }

    switch (cmd) {
#ifdef HI_TEE_SCI_SUPPORT
        case CMD_SCI_SET_SECURITY_MODE: {
            sci_security_mode *psci_security_mode;

            psci_security_mode = (sci_security_mode *)arg;

            ret = sci_set_security_mode(psci_security_mode->sci_port, psci_security_mode->security_mode);
            break;
        }
#endif
        case CMD_SCI_OPEN: {
            sci_open_para     *psci_open;

            psci_open = (sci_open_para *)arg;

            ret = sci_ext_open(psci_open, psci_dev_state);

            break;
        }

        case CMD_SCI_CLOSE: {
            ret = sci_check_state(*((hi_sci_port *)arg), psci_dev_state);
            if (ret == HI_SUCCESS) {
                ret = sci_ext_close(*((hi_sci_port *)arg), psci_dev_state);
            } else {
                ret = HI_SUCCESS;
            }

            break;
        }

        default:
            break;
    }

    up(&g_sci_mutex);

    if (ret) {
        return  HI_FAILURE;
    }

    switch (cmd) {
        case CMD_SCI_RESET: {
            sci_reset_para  *psci_reset;

            psci_reset = (sci_reset_para *)arg;
            ret = sci_reset(psci_reset->sci_port, psci_reset->warm_reset);

            break;
        }

        case CMD_SCI_DEACTIVE: {
            ret = sci_deactive(*((hi_sci_port *)arg));

            break;
        }

        case CMD_SCI_GET_ATR: {
            sci_atr  *psci_atr;

            psci_atr = (sci_atr *)arg;
            ret = sci_get_atr(psci_atr->sci_port, psci_atr->patr_buf, psci_atr->buf_size, &psci_atr->data_len);

            break;
        }

        case CMD_SCI_GET_STATUS: {
            sci_status   *psci_status;

            psci_status = (sci_status *)arg;
            ret = sci_get_status(psci_status->sci_port, &psci_status->sci_status);

            break;
        }

        case CMD_SCI_CONF_VCC: {
            sci_level   *psci_level;

            psci_level = (sci_level *)arg;
            ret = sci_config_vcc(psci_level->sci_port, psci_level->sci_level);

            break;
        }

        case CMD_SCI_CONF_DETECT: {
            sci_level   *psci_level;

            psci_level = (sci_level *)arg;
            ret = sci_config_detect(psci_level->sci_port, psci_level->sci_level);

            break;
        }

        case CMD_SCI_CONF_MODE: {
            sci_io_output_type   *psci_io;

            psci_io = (sci_io_output_type *)arg;

            if (psci_io->en_io == SCI_IO_CLK) {
                ret = sci_config_clk_mode(psci_io->sci_port, psci_io->output_type);
            } else if (psci_io->en_io == SCI_IO_RESET) {
                ret = sci_config_reset_mode(psci_io->sci_port, psci_io->output_type);
            } else if (psci_io->en_io == SCI_IO_VCC_EN) {
                ret = sci_config_vcc_en_mode(psci_io->sci_port, psci_io->output_type);
            } else {
                ret = HI_ERR_SCI_NOTSUPPORT;
            }

            break;
        }

        case CMD_SCI_SEND_DATA: {
            sci_data    *psci_data;

            psci_data = (sci_data *)arg;
            ret = sci_send_data(psci_data->sci_port, psci_data->pdata_buf, psci_data->buf_size, &psci_data->data_len,
                                psci_data->timeout_ms);

            break;
        }

        case CMD_SCI_RECEIVE_DATA: {
            sci_data    *psci_data;

            psci_data = (sci_data *)arg;
            ret = sci_receive_data(psci_data->sci_port, psci_data->pdata_buf, psci_data->buf_size, &psci_data->data_len,
                                   psci_data->timeout_ms);

            if (ret == HI_ERR_SCI_RECEIVE_ERR) {
                ret = HI_SUCCESS;
            }

            break;
        }

        case CMD_SCI_SWITCH: {
            sci_open_para     *psci_open;

            psci_open = (sci_open_para *)arg;
            ret = sci_switch(psci_open->sci_port, psci_open->sci_protocol, psci_open->frequency);

            break;
        }

        case CMD_SCI_SET_BAUD: {
            sci_ext_baud     *psci_ext_baud;

            psci_ext_baud = (sci_ext_baud *)arg;
            ret = sci_ext_set_baud(psci_ext_baud->sci_port, psci_ext_baud->clk_rate, psci_ext_baud->bit_rate);

            break;
        }

        case CMD_SCI_SET_CHGUARD: {
            sci_add_guard  *psci_add_guard;

            psci_add_guard = (sci_add_guard *)arg;
            ret = sci_set_char_guard(psci_add_guard->sci_port, psci_add_guard->add_char_guard);
            break;
        }

        case CMD_SCI_SEND_PPS_DATA: {
            sci_pps    *psci_send_pps;

            psci_send_pps = (sci_pps *)arg;
            ret = sci_pps_negotiation(psci_send_pps->sci_port, psci_send_pps->send, psci_send_pps->send_len,
                                      psci_send_pps->rec_timeouts);

            break;
        }

        case CMD_SCI_GET_PPS_DATA: {
            sci_pps    *psci_rec_pps;

            psci_rec_pps = (sci_pps *)arg;
            ret = sci_pps_get_rec_buffer(psci_rec_pps->sci_port, psci_rec_pps->receive, &psci_rec_pps->receive_len);

            break;
        }

        case CMD_SCI_GET_PARAM: {
            hi_sci_params_ptr psci_out_param;

            psci_out_param = (hi_sci_params_ptr)arg;
            ret = sci_output_param(psci_out_param->sci_port, psci_out_param);

            break;
        }

        case CMD_SCI_SET_CHARTIMEOUT: {
            sci_char_timeout *ch_timeout;

            ch_timeout = (sci_char_timeout *)arg;
            ret = sci_set_char_timeout(ch_timeout->sci_port, ch_timeout->sci_protocol, ch_timeout->char_timeouts);
            break;
        }

        case CMD_SCI_SET_BLOCKTIMEOUT: {
            sci_block_timeout *blk_timeout;

            blk_timeout = (sci_block_timeout *)arg;
            ret = sci_set_block_timeout(blk_timeout->sci_port, blk_timeout->block_timeouts);

            break;
        }

        case CMD_SCI_SET_TXRETRY: {
            sci_txretry *tx_retry;

            tx_retry = (sci_txretry *)arg;
            ret = sci_set_tx_retry(tx_retry->sci_port, tx_retry->tx_retry_times);

            break;
        }

        default:
            ret = HI_SUCCESS;
            break;
    }

    return ret;
}

#ifdef CONFIG_COMPAT
hi_slong sci_compat_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    sci_dev_state *psci_dev_state = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    SCI_CHECK_POINTER(file);
    SCI_CHECK_POINTER(arg);

    psci_dev_state = file->private_data;

    if (down_interruptible(&g_sci_mutex)) {
        return -EAGAIN;
    }

    switch (cmd) {
        case CMD_SCI_OPEN: {
            sci_open_para     *psci_open;

            psci_open = (sci_open_para *)arg;
            ret = sci_ext_open(psci_open, psci_dev_state);

            break;
        }

        case CMD_SCI_CLOSE: {
            ret = sci_check_state(*((hi_sci_port *)arg), psci_dev_state);
            if (ret == HI_SUCCESS) {
                ret = sci_ext_close(*((hi_sci_port *)arg), psci_dev_state);
            }

            break;
        }

        default:
            break;
    }

    up(&g_sci_mutex);

    if (ret) {
        return HI_FAILURE;
    }

    switch (cmd) {
        case CMD_SCI_RESET: {
            sci_reset_para  *psci_reset;

            psci_reset = (sci_reset_para *)arg;
            ret = sci_reset(psci_reset->sci_port, psci_reset->warm_reset);

            break;
        }

        case CMD_SCI_DEACTIVE: {
            ret = sci_deactive(*((hi_sci_port *)arg));

            break;
        }

        case CMD_SCI_COMPAT_GET_ATR: {
            sci_atr_compat  *psci_atr;

            psci_atr = (sci_atr_compat *)arg;
#ifdef CONFIG_COMPAT
            ret = sci_get_atr(psci_atr->sci_port, (hi_u8 *)compat_ptr(psci_atr->patr_buf),
                              psci_atr->buf_size, &psci_atr->data_len);
#endif
            break;
        }

        case CMD_SCI_GET_STATUS: {
            sci_status   *psci_status;

            psci_status = (sci_status *)arg;
            ret = sci_get_status(psci_status->sci_port, &psci_status->sci_status);

            break;
        }

        case CMD_SCI_CONF_VCC: {
            sci_level   *psci_level;

            psci_level = (sci_level *)arg;
            ret = sci_config_vcc(psci_level->sci_port, psci_level->sci_level);

            break;
        }

        case CMD_SCI_CONF_DETECT: {
            sci_level   *psci_level;

            psci_level = (sci_level *)arg;
            ret = sci_config_detect(psci_level->sci_port, psci_level->sci_level);

            break;
        }

        case CMD_SCI_CONF_MODE: {
            sci_io_output_type   *psci_io;

            psci_io = (sci_io_output_type *)arg;

            if (psci_io->en_io == SCI_IO_CLK) {
                ret = sci_config_clk_mode(psci_io->sci_port, psci_io->output_type);
            } else if (psci_io->en_io == SCI_IO_RESET) {
                ret = sci_config_reset_mode(psci_io->sci_port, psci_io->output_type);
            } else if (psci_io->en_io == SCI_IO_VCC_EN) {
                ret = sci_config_vcc_en_mode(psci_io->sci_port, psci_io->output_type);
            } else {
                ret = HI_ERR_SCI_NOTSUPPORT;
            }

            break;
        }

        case CMD_SCI_COMPAT_SEND_DATA: {
            sci_data_compat    *psci_data;

            psci_data = (sci_data_compat *)arg;
#ifdef CONFIG_COMPAT
            ret = sci_send_data(psci_data->sci_port, (hi_u8 *)compat_ptr(psci_data->pdata_buf),
                                psci_data->buf_size, &psci_data->data_len, psci_data->timeout_ms);
#endif
            break;
        }

        case CMD_SCI_COMPAT_RECEIVE_DATA: {
            sci_data_compat    *psci_data;

            psci_data = (sci_data_compat *)arg;
#ifdef CONFIG_COMPAT
            ret = sci_receive_data(psci_data->sci_port, (hi_u8 *)compat_ptr(psci_data->pdata_buf),
                                   psci_data->buf_size, &psci_data->data_len, psci_data->timeout_ms);
#endif
            if (ret == HI_ERR_SCI_RECEIVE_ERR) {
                ret = HI_SUCCESS;
            }

            break;
        }

        case CMD_SCI_SWITCH: {
            sci_open_para     *psci_open;

            psci_open = (sci_open_para *)arg;
            ret = sci_switch(psci_open->sci_port, psci_open->sci_protocol, psci_open->frequency);

            break;
        }

        case CMD_SCI_SET_BAUD: {
            sci_ext_baud     *psci_ext_baud;

            psci_ext_baud = (sci_ext_baud *)arg;
            ret = sci_ext_set_baud(psci_ext_baud->sci_port, psci_ext_baud->clk_rate, psci_ext_baud->bit_rate);

            break;
        }

        case CMD_SCI_SET_CHGUARD: {
            sci_add_guard  *psci_add_guard;

            psci_add_guard = (sci_add_guard *)arg;
            ret = sci_set_char_guard(psci_add_guard->sci_port, psci_add_guard->add_char_guard);
            break;
        }

        case CMD_SCI_SEND_PPS_DATA: {
            sci_pps    *psci_send_pps;

            psci_send_pps = (sci_pps *)arg;
            ret = sci_pps_negotiation(psci_send_pps->sci_port, psci_send_pps->send, psci_send_pps->send_len,
                                      psci_send_pps->rec_timeouts);

            break;
        }

        case CMD_SCI_GET_PPS_DATA: {
            sci_pps    *psci_rec_pps;

            psci_rec_pps = (sci_pps *)arg;
            ret = sci_pps_get_rec_buffer(psci_rec_pps->sci_port, psci_rec_pps->receive, &psci_rec_pps->receive_len);

            break;
        }

        case CMD_SCI_GET_PARAM: {
            hi_sci_params_ptr psci_out_param;

            psci_out_param = (hi_sci_params_ptr)arg;
            ret = sci_output_param(psci_out_param->sci_port, psci_out_param);

            break;
        }

        case CMD_SCI_SET_CHARTIMEOUT: {
            sci_char_timeout *ch_timeout;

            ch_timeout = (sci_char_timeout *)arg;
            ret = sci_set_char_timeout(ch_timeout->sci_port, ch_timeout->sci_protocol, ch_timeout->char_timeouts);
            break;
        }

        case CMD_SCI_SET_BLOCKTIMEOUT: {
            sci_block_timeout *blk_timeout;

            blk_timeout = (sci_block_timeout *)arg;
            ret = sci_set_block_timeout(blk_timeout->sci_port, blk_timeout->block_timeouts);

            break;
        }

        case CMD_SCI_SET_TXRETRY: {
            sci_txretry *tx_retry;

            tx_retry = (sci_txretry *)arg;
            ret = sci_set_tx_retry(tx_retry->sci_port, tx_retry->tx_retry_times);

            break;
        }

        default:
            ret = HI_SUCCESS;
            break;
    }

    return ret;
}
#endif

static hi_s32 sci_drv_open(struct inode *inode, struct file *filp)
{
    hi_s32 ret;
    sci_dev_state    *psci_dev_state = HI_NULL;
    hi_u8 ii = 0;

    SCI_CHECK_POINTER(filp);

    ret = down_interruptible(&g_sci_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    psci_dev_state = HI_KMALLOC(HI_ID_SCI, sizeof(sci_dev_state), GFP_KERNEL);
    if (!psci_dev_state) {
        HI_FATAL_SCI("malloc psci_dev_state failed.\n");
        up(&g_sci_mutex);
        return HI_FAILURE;
    }
    if (memset_s(psci_dev_state, sizeof(sci_dev_state), 0, sizeof(sci_dev_state))) {
        HI_ERR_SCI("memset_s failed! \n");
        up(&g_sci_mutex);
        return HI_FAILURE;
    }

    if (atomic_inc_return(&g_sci_count) == 1) {
        sci_init();
    }

    for (ii = 0; ii < HI_SCI_PORT_NUM; ii++) {
        psci_dev_state->sci[ii] = HI_FALSE;
    }

    filp->private_data = psci_dev_state;

    up(&g_sci_mutex);
    return HI_SUCCESS;
}

static hi_s32 sci_drv_close(struct inode *inode, struct file *filp)
{
    hi_s32 ret;
    hi_u8 ii = 0;
    sci_dev_state *psci_dev_state = HI_NULL;

    SCI_CHECK_POINTER(filp);

    ret = down_interruptible(&g_sci_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_SCI("%s:down_interruptible err! \n", __func__);
        return ret;
    }

    psci_dev_state = filp->private_data;

    for (ii = HI_SCI_PORT0; ii < HI_SCI_PORT_NUM; ii++) {
        if (psci_dev_state->sci[ii]) {
            ret = sci_ext_close(ii, psci_dev_state);
            if (ret != HI_SUCCESS) {
                up(&g_sci_mutex);
                return HI_FAILURE;
            }
        }
    }

    sci_deinit();
    if (!atomic_dec_and_test(&g_sci_count)) {
        HI_ERR_SCI("stomic_dec_and_test operating failed! \n");
    }

    HI_KFREE(HI_ID_SCI, filp->private_data);

    up(&g_sci_mutex);

    return HI_SUCCESS;
}

static hi_slong sci_drv_ioctl(struct file *ffile, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret;

    ret = (hi_slong)hi_drv_user_copy(ffile, cmd, arg, sci_ioctl);

    return ret;
}

#ifdef CONFIG_COMPAT
static hi_slong sci_drv_compat_ioctl(struct file *ffile, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret;

    ret = (hi_slong)hi_drv_user_copy(ffile, cmd, arg, sci_compat_ioctl);

    return ret;
}
#endif

static hi_s32 sci_pm_suspend(struct device *dev)
{
    hi_s32 ret = HI_SUCCESS;

    ret = down_trylock(&g_sci_mutex);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SCI("sci_pm_suspend lock err!\n");
        return HI_FAILURE;
    }

    ret = sci_suspend();
    if (ret != HI_SUCCESS) {
        up(&g_sci_mutex);
        return HI_FAILURE;
    }

    up(&g_sci_mutex);
    HI_PRINT("SCI suspend OK\n");
    return HI_SUCCESS;
}

static hi_s32 sci_pm_resume(struct device *dev)
{
    hi_s32 ret = HI_SUCCESS;

    ret = down_trylock(&g_sci_mutex);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SCI("sci_pm_resume  lock err!\n");
        return HI_FAILURE;
    }

    /* resume sci module will reopen and active smart card */
    ret = sci_resume();
    if (ret != HI_SUCCESS) {
        up(&g_sci_mutex);
        return HI_FAILURE;
    }

    up(&g_sci_mutex);
    HI_PRINT("SCI resume OK\n");
    return HI_SUCCESS;
}

static struct dev_pm_ops g_sci_pm_ops = {
    .suspend        = sci_pm_suspend,
    .suspend_late   = NULL,
    .resume_early   = NULL,
    .resume         = sci_pm_resume,
};

static struct file_operations g_sci_fops = {
owner:
    THIS_MODULE,
open:
    sci_drv_open,
unlocked_ioctl:
    sci_drv_ioctl,
#ifdef CONFIG_COMPAT
compat_ioctl:
    sci_drv_compat_ioctl,
#endif
release:
    sci_drv_close,
};

static struct class *g_sci_class = HI_NULL;
static dev_t g_sci_devno;
static struct cdev *g_sci_cdev = HI_NULL;
static struct device *g_sci_dev = HI_NULL;

static hi_s32 sci_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_sci_devno, 0, 1, "sci");
    if (ret != HI_SUCCESS) {
        HI_FATAL_SCI("sci alloc chrdev region failed\n");

        return HI_FAILURE;
    }

    g_sci_cdev = cdev_alloc();
    if (IS_ERR(g_sci_cdev)) {
        HI_FATAL_SCI("sci alloc cdev failed! \n");

        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_sci_cdev, &g_sci_fops);
    g_sci_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_sci_cdev, g_sci_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SCI("sci add cdev failed, ret(%d).\n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_sci_class = class_create(THIS_MODULE, "sci_class");
    if (IS_ERR(g_sci_class)) {
        HI_FATAL_SCI("sci create dev class failed! \n");

        ret = HI_FAILURE;
        goto out2;
    }

    g_sci_class->pm = &g_sci_pm_ops;
    g_sci_dev = device_create(g_sci_class, HI_NULL, g_sci_devno, HI_NULL, HI_DEV_SCI_NAME);
    if (IS_ERR(g_sci_dev)) {
        HI_FATAL_SCI("sci create dev failed! \n");
        ret = HI_FAILURE;

        goto out3;
    }

    return HI_SUCCESS;

out3:
    class_destroy(g_sci_class);
    g_sci_class = HI_NULL;
out2:
    cdev_del(g_sci_cdev);
out1:
    kfree(g_sci_cdev);
    g_sci_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_sci_devno, 1);

    return ret;
}

static hi_void sci_unregister_dev(hi_void)
{
    device_destroy(g_sci_class, g_sci_devno);
    g_sci_dev = HI_NULL;
    class_destroy(g_sci_class);
    g_sci_class = HI_NULL;
    cdev_del(g_sci_cdev);
    kfree(g_sci_cdev);
    g_sci_cdev = HI_NULL;
    unregister_chrdev_region(g_sci_devno, 1);

    return;
}

hi_s32 sci_drv_module_init(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_u8 i = 0;
    hi_proc_item *proc_item = HI_NULL;
#endif
    /* SCI device regeister */
    if (sci_register_dev() < 0) {
        HI_FATAL_SCI("register SCI failed.\n");
        return HI_FAILURE;
    }

#ifdef HI_PROC_SUPPORT
    /* register SCI proc function */
    for (i = 0; i < HI_SCI_PORT_NUM; i++) {
        hi_char name[PROC_NAME_LEN] = "sci";

        name[3] = '0' + i % 10; /* buffer name 3 nd element i%10 */

        proc_item = hi_drv_proc_add_module(name, HI_NULL, HI_NULL);
        if (proc_item == HI_NULL) {
            HI_FATAL_SCI("add sci%d proc failed.\n", i);
            sci_unregister_dev();
            return HI_FAILURE;
        }

        proc_item->read  = sci_proc_read;
    }
#endif

#ifdef MODULE
    HI_PRINT("Load hi_sci.ko success.   \t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
}

hi_void sci_drv_module_exit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_drv_proc_remove_module("sci0");

    if (HI_SCI_PORT_NUM > 1) {
        hi_drv_proc_remove_module("sci1");
    }
#endif
    sci_unregister_dev();
    return;
}

#ifdef MODULE
module_init(sci_drv_module_init);
module_exit(sci_drv_module_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");

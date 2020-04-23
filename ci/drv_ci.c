/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of drv_ci
 * Author: z00268517
 * Create: 2014-08-02
 */
/**
 * \file
 * \brief Provide the realization of the CI driver interface
 */

/* ----------------------------- INCLUDE FILES ------------------------------------------ */

#include "hi_debug.h"
#include "drv_ci_ext.h"
#include "hal_ci.h"
#include "hi_drv_ci.h"
#include "hi_osal.h"
#include "linux/hisilicon/securec.h"

#undef  LOG_D_MODULE_ID
#define LOG_D_MODULE_ID HI_ID_CI

/* allow modules to define internel error code, from 0x1000. */
#define HI_KMALLOC_CI(size, flags)      osal_kmalloc(HI_ID_CI, size, flags)
#define HI_KFREE_CI(addr)               osal_kfree(HI_ID_CI, addr)
#define HI_VMALLOC_CI(size)             osal_vmalloc(HI_ID_CI, size)
#define HI_VFREE_CI(addr)               osal_vfree(HI_ID_CI, addr)

/* --------------------------- MACRO DECLARATIONS --------------------------------------- */
#define MAX_TUPPLE_BUFFER 0x100 /* maximum tupple buffer size */

#ifndef CI_PEACH_TEST
#define CI_PCCD_IORESET_TIMEOUT 300   /* 3s */
#define CI_PCCD_RW_TIMEOUT 5          /* 50ms */
#define CI_PCCD_NEGBUFFER_TIMEOUT 300 /* 300ms */

#define CI_TIME_10MS 10           /* 10ms */
#define CI_TIME_1MS 1             /* 1ms */
#define CI_DELAY_AFTER_WRITE_US 5 /* us */
#else
#define CI_PCCD_IORESET_TIMEOUT 0   /* 3s */
#define CI_PCCD_RW_TIMEOUT 0       /* 5ms */
#define CI_PCCD_NEGBUFFER_TIMEOUT 0 /* 300ms */

#define CI_TIME_10MS 0           /* 10ms */
#define CI_TIME_1MS 0             /* 1ms */
#define CI_DELAY_AFTER_WRITE_US 0 /* us */
#endif

#define DATA_REG 0
#define COM_STAT_REG 1
#define SIZE_L_REG 2
#define SIZE_H_REG 3

#define CI_PCCD_MIN_BUFFERSIZE 16
#define CI_PCCD_MAX_BUFFERSIZE 1024

/* status register bits */
#define PCCD_RE 0x01 /* read error */
#define PCCD_WE 0x02 /* write error */
#define PCCD_FR 0x40 /* free */
#define PCCD_DA 0x80 /* Data available */

/* command register bits */
#define PCCD_SETHC 0x01 /* host control bit set */
#define PCCD_SETSW 0x02 /* size write set */
#define PCCD_SETSR 0x04 /* size read set  */
#define PCCD_SETRS 0x08 /* interface reset */
#define PCCD_CLEAR 0x00 /* clear command register */

#define BITS_IN_1BYTE     8
#define BITS_IN_2BYTE     16
#define BITS_IN_3BYTE     24
#define CIS_EVEN_ADDR     2
#define IO_DATA_LEN       2

/* EN50221 specific constants */
#define STCE_EV1                    "DVB_HOST"
#define STCE_PD1                    "DVB_CI_MODULE"
#define STCE_EV2                    "DTV_HOST"
#define STCE_PD2                    "DTV_CI_MODULE"
#define STCF_LIB                    "DVB_CI_V1.00"
#define STCF_LIB_LEN                12
#define TPCE_IO                     0x22
#define TPCE_IF                     0x04
#define COR_BASE_ADDRESS_UPPERLIMIT 0xFFE
#define STCI_IFN                    0x41
#define STCI_IFN_1                  0x02
#define TPLLV1_MINOR                0x00
#define TPLLV1_MAJOR                0x05
#define TPLLMANFID_SIZE             0x04
#define TPCC_RFSZ_MASK              0xC0
#define TPCC_RMSZ_MASK              0x3C
#define TPCC_RASZ_MASK              0x03
#define TPCE_INDX_MASK              0x3F
#define RW_SCALE_MASK               0x1F
#define IO_ADDLINES_MASK            0x1F
#define DTYPE_NULL                  0x00
#define DTYPE_RESERVED              0x0F
#define DSPEED_MASK                 0x07
#define TPCE_TD_WAIT_SCALE_MASK     0x03
#define TPCE_TD_READY_SCALE_MASK    0x1C
#define TPCE_IO_RD_LENSIZE_MASK     0xC0
#define TPCE_IO_RD_ADDRSIZE_MASK    0x30
#define TPCE_IO_RD_ARNUM_MASK       0x0F
#define TPCE_MS_WINDOW_MASK         0x07
#define TPCE_MS_LENSIZE_MASK        0x18
#define TPCE_MS_CARDADDRSIZE_MASK   0x60
#define STCE_EV_TAG                 0xC0
#define STCE_PD_TAG                 0xC1

#define NOT_READY_WAIT 0x1F
#define WAIT_SCALE_0   0x1C
#define WAIT_SCALE_1   0x1D
#define WAIT_SCALE_2   0x1E
#define READY_SCALE_0  0x03
#define READY_SCALE_1  0x07
#define READY_SCALE_2  0x0B
#define READY_SCALE_3  0x0F
#define READY_SCALE_4  0x13
#define READY_SCALE_5  0x17
#define READY_SCALE_6  0x1b

/* tupple tag of CIS */
#define CISTPL_DEVICE        0x01
#define CISTPL_CHECKSUM      0x10
#define CISTPL_LINKTARGET    0x13
#define CISTPL_NO_LINK       0x14
#define CISTPL_VERS_1        0x15
#define CISTPL_ALTSTR        0x16
#define CISTPL_DEVICE_A      0x17
#define CISTPL_CONFIG        0x1a
#define CISTPL_CFTABLE_ENTRY 0x1b
#define CISTPL_DEVICE_OC     0x1c
#define CISTPL_DEVICE_OA     0x1d
#define CISTPL_MANFID        0x20
#define CISTPL_FUNCID        0x21
#define CISTPL_END           0xff

/* masking bit */
#define MASK_BIT_0 0x01
#define MASK_BIT_1 0x02
#define MASK_BIT_2 0x04
#define MASK_BIT_3 0x08
#define MASK_BIT_4 0x10
#define MASK_BIT_5 0x20
#define MASK_BIT_6 0x40
#define MASK_BIT_7 0x80

#define TUPPLE_DATA_MIN_LEN 6

#define INVALID_DATA_SIZE  0xFFFF
#define DBG_DUMP_LINE_SIZE 16

#define CHECK_CI_OPENED(port) do {                \
        if (!g_ci_drv_param[port].open) {           \
            HI_LOG_ERR("CI port didn't init.\n"); \
            hi_err_print_u32(port);                \
            return HI_ERR_CI_NOT_INIT;            \
        }                                         \
    } while (0)

#define CHECK_CI_PCCD_OPENED(port, card) do {     \
        if ((g_ci_drv_param[port].open != HI_TRUE)  \
            || (g_ci_drv_param[port].card_param[card].card_open != HI_TRUE)) { \
            HI_LOG_ERR("CI card didn't init.\n");                         \
            hi_err_print_u32(port);                                        \
            hi_err_print_u32(card);                                        \
            return HI_ERR_CI_NOT_INIT;                                    \
        }                                                                 \
    } while (0)

/* -------------------- STATIC STRUCTURE DECLARATIONS ----------------------------------- */
/* private Data structures***********************************************/
/* CI PC CARD configure and management structure */
typedef struct hi_ci_drv_pccd_parameter {
    hi_bool card_open;
    hi_bool print_io_data; /* print IO data */
    ci_pccd_attr attr;     /* voltage, speed, ... */
    hi_u32 tpcc_radr;      /* TPCC_RADR, will be evaluated when check CIS */
    hi_u8 tpce_indx;       /* TPCE_INDX, will be evaluated when check CIS */
    hi_u8 reserve;
    hi_u16 buffer_size; /* save buffer size, prevent read ro write overflow */
    hi_u8 *buffer;      /* buffer use to save IO data */
    hi_u32 io_cnt;
} ci_drv_pccd_parameter;

/* CI driver configure structure */
typedef struct hi_ci_drv_parameter {
    hi_bool open;
    ci_drv_pccd_parameter card_param[HI_CI_PCCD_MAX];
    hi_ci_attr attr;
} ci_drv_parameter;

/* tupple Data structure */
typedef struct hi_ci_pccd_tupple {
    hi_u8 tupple_tag;
    hi_u8 tupple_link;
    hi_u8 tupple_data[MAX_TUPPLE_BUFFER];
} ci_pccd_tupple, *ci_pccd_tupple_ptr;

/* ------------------------- STATIC DECLARATIONS ---------------------------------------- */
static ci_drv_parameter g_ci_drv_param[HI_CI_PORT_MAX];

hi_s32 drv_ci_init(hi_void)
{
    hi_s32 ret;
    hi_ci_port port;

    hi_dbg_func_enter();

    CHECK_FUNC_RET(memset_s(g_ci_drv_param, sizeof(g_ci_drv_param), 0, sizeof(g_ci_drv_param)));

    for (port = HI_CI_PORT_0; port < HI_CI_PORT_MAX; port++) {
        g_ci_drv_param[port].attr.dev_type = HI_CI_DEV_SEL_HICI;
        g_ci_drv_param[port].attr.dev_attr.hici.is_power_ctrl_gpio_used = HI_FALSE;
    }

    ret = hal_ci_init();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_init, ret);
        return ret;
    }

    hi_dbg_func_exit();

    return HI_SUCCESS;
}

hi_void drv_ci_de_init(hi_void)
{
    hi_dbg_func_enter();

    hi_dbg_func_exit();
}

/* added begin 2012-04-24 : support various CI device */
hi_s32 drv_ci_set_attr(hi_ci_port port, hi_ci_attr attr)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(attr.dev_type != HI_CI_DEV_SEL_HICI);

    g_ci_drv_param[port].attr = attr;

    /*
     * normally, user should call this function after open CI device
     * once they open device earlier, close and open the device again.
     */
    ret = hal_ci_set_attr(port, attr);
    if (ret != HI_SUCCESS) {
        hi_err_print_u32(port);
        hi_err_print_call_fun_err(hal_ci_set_attr, ret);
        return ret;
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_get_attr(hi_ci_port port, hi_ci_attr *attr)
{
    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(attr == HI_NULL);

    *attr = g_ci_drv_param[port].attr;

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_open(hi_ci_port port)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);

    ret = hal_ci_device_open(port);
    if (ret == HI_SUCCESS) {
        /* init parameters */
        g_ci_drv_param[port].open = HI_TRUE;
    } else {
        HI_LOG_ERR("CI port open failed.\n", port, ret);
        hi_err_print_u32(port);
        hi_err_print_call_fun_err(hal_ci_device_open, ret);
        return ret;
    }

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_close(hi_ci_port port)
{
    hi_s32 ret;
    hi_ci_pccd card;

    hi_dbg_func_enter();

    if (port >= HI_CI_PORT_1) {
        return HI_FAILURE;
    }

    /* close all cards of this group. */
    for (card = HI_CI_PCCD_A; card < HI_CI_PCCD_B; card++) {
        hal_ci_pccd_close(port, card);
        g_ci_drv_param[port].card_param[card].card_open = HI_FALSE;
        hi_err_print_u32(port);
        hi_err_print_u32(card);
    }

    ret = hal_ci_device_close(port);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_device_close, ret);
        g_ci_drv_param[port].open = HI_FALSE;
        return ret;
    }

    g_ci_drv_param[port].open = HI_FALSE;

    hi_dbg_func_exit();

    return HI_SUCCESS;
}

hi_s32 drv_ci_standby(hi_ci_port port)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);

    ret = hal_ci_standby(port);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_standby, ret);
        hi_err_print_u32(port);
        return ret;
    }

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_resume(hi_ci_port port)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);

    ret = hal_ci_resume(port);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_resume, ret);
        hi_err_print_u32(port);
        return ret;
    }

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_pccd_open(hi_ci_port port, hi_ci_pccd card)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_OPENED(port);

    /* make sure open once */
    if (g_ci_drv_param[port].open == HI_FALSE) {
        HI_LOG_ERR("CI PCCD open failed for CI_OPEN failed.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return HI_FAILURE;
    }

    ret = hal_ci_pccd_open(port, card);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("CI port PCCD open failed.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_call_fun_err(hal_ci_pccd_open, ret);
        return ret;
    }

    /* init buffer */
    g_ci_drv_param[port].card_param[card].buffer = HI_VMALLOC_CI(CI_PCCD_MAX_BUFFERSIZE);
    if (g_ci_drv_param[port].card_param[card].buffer == HI_NULL) {
        HI_LOG_FATAL("CI port PCCD alloc memory failed.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_call_fun_err(HI_VMALLOC_CI, HI_ERR_CI_NO_MEMORY);
        return HI_ERR_CI_NO_MEMORY;
    }

    g_ci_drv_param[port].card_param[card].buffer_size = CI_PCCD_MAX_BUFFERSIZE;

    /* init parameters */
    g_ci_drv_param[port].card_param[card].card_open = HI_TRUE;
    HI_LOG_INFO("CI port PCCD open success\n");

    hi_dbg_func_exit();
    return ret;
}

hi_void drv_ci_pccd_close(hi_ci_port port, hi_ci_pccd card)
{
    hi_dbg_func_enter();

    if ((port >= HI_CI_PORT_1) || (card >= HI_CI_PCCD_B)) {
        HI_LOG_ERR("invalid CI port or card id.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return;
    }

    if (g_ci_drv_param[port].open == HI_FALSE) {
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return;
    }

    /* free buffer */
    if (g_ci_drv_param[port].card_param[card].buffer != HI_NULL) {
        HI_VFREE_CI(g_ci_drv_param[port].card_param[card].buffer);
        g_ci_drv_param[port].card_param[card].buffer = HI_NULL;
        g_ci_drv_param[port].card_param[card].buffer_size = 0;
    }

    /* hardware close */
    hal_ci_pccd_close(port, card);

    g_ci_drv_param[port].card_param[card].card_open = HI_FALSE;

    hi_dbg_func_exit();
}

hi_s32 drv_ci_pccd_ctrl_power(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ctrl_power ctrl_power)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_PCCD_OPENED(port, card);

    ret = hal_ci_pccd_ctrl_power(port, card, ctrl_power);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_pccd_ctrl_power, ret);
        hi_err_print_u32(port);
        return ret;
    }
    g_ci_drv_param[port].attr.dev_attr.hici.is_power_ctrl_gpio_used = (ctrl_power == HI_CI_PCCD_CTRLPOWER_ON) ? HI_FALSE : HI_TRUE;

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_pccd_reset(hi_ci_port port, hi_ci_pccd card)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_PCCD_OPENED(port, card);

    ret = hal_ci_pccd_reset(port, card);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("CI PCCD reset failed.\n");
        hi_err_print_call_fun_err(ci_pccd_reset, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    HI_LOG_INFO("CI PCCD reset successfully\n");

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_pccd_is_ready(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ready_ptr ready)
{
    hi_s32 ret;

    hi_dbg_func_enter();
    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(ready == HI_NULL);

    CHECK_CI_PCCD_OPENED(port, card);

    ret = hal_ci_pccd_ready_or_busy(port, card, ready);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("CI PCCD get ready status failed.\n");
        hi_err_print_call_fun_err(hal_ci_pccd_ready_or_busy, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_detect(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_status_ptr status)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(status == HI_NULL);

    CHECK_CI_PCCD_OPENED(port, card);

    ret = hal_ci_pccd_detect(port, card, status);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("CI PCCD detect failed.\n");
        hi_err_print_call_fun_err(ci_pccd_detect, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_pccd_get_status(hi_ci_port port, hi_ci_pccd card, hi_u8 *value)
{
    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(value == HI_NULL);

    CHECK_CI_PCCD_OPENED(port, card);

    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, value));

    hi_dbg_func_exit();

    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_io_read(hi_ci_port port, hi_ci_pccd card, hi_u8 *buffer, hi_u32 buffer_len, hi_u32 *read_len)
{
    hi_u16 io_len_to_read;
    hi_u32 counter;
    hi_u16 buffer_size;
    hi_u8 status;
    hi_u8 data_low;
    hi_u8 data_high;
    hi_u8 *recv = HI_NULL;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(buffer == HI_NULL);
    HI_LOG_CHECK_PARAM(read_len == HI_NULL);

    CHECK_CI_PCCD_OPENED(port, card);

    /* Module to Host Transfers
     * Periodically the host tests the DA bit in the Status Register. If DA is '1'
     * then the host reads the Size Register to find out how much data is to be transferred.
     * It then reads that number of data bytes from the Data register.
     * This multiple read shall not be interrupted by any other operations on the interface
     * except for reads of the Status Register. When the first byte is read the module
     * sets RE to '1' and sets DA to '0'. During the transfer the RE
     * bit remains at '1' until the last byte is read, at which point it is set to '0'.
     * If any further bytes are read then the RE bit is set to '1'.
     */
    if (g_ci_drv_param[port].card_param[card].buffer == HI_NULL) {
        hi_err_print_err_code(HI_ERR_CI_IO_READ_ERR);
        return HI_ERR_CI_IO_READ_ERR;
    } else {
        recv = g_ci_drv_param[port].card_param[card].buffer;
    }

    /* DA is set ? */
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
    if ((status & PCCD_DA) != PCCD_DA) {
        hi_err_print_err_code(HI_ERR_CI_IO_READ_ERR);
        hi_err_print_u32(status);
        return HI_ERR_CI_IO_READ_ERR;
    }

    /* read size */
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, SIZE_L_REG, &data_low));
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, SIZE_H_REG, &data_high));
    io_len_to_read = data_low | (data_high << BITS_IN_1BYTE);

     /* read len must be a safe length. */
     /* please invoke ci_pccd_neg_buffer_size() to negotiate buffer size. */
    buffer_size = g_ci_drv_param[port].card_param[card].buffer_size;
    if (io_len_to_read > buffer_size) {
        hi_err_print_err_code(HI_ERR_CI_IO_READ_ERR);
        hi_err_print_u32(io_len_to_read);
        return HI_ERR_CI_IO_READ_ERR;
    }

    /* read io data */
    for (counter = 0; counter < io_len_to_read; counter++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, DATA_REG, recv + counter));
    }

    /* before read over, read error bit should be 1. */
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
    if ((status & PCCD_RE) != 0x00) {
        HI_LOG_ERR("Io read error, the RE is '1'.\n");
        return HI_FAILURE;
    }

    if (g_ci_drv_param[port].card_param[card].print_io_data == HI_TRUE) {
        hi_log_print_block(recv, counter);
    }

    if (buffer_len < counter) {
        HI_LOG_ERR("the dst is lower than src, copy temp buffer to user failed.\n");
        return HI_FAILURE;
    }

    /* copy data to user */
    if (osal_copy_to_user(buffer, recv, counter)) {
        HI_LOG_ERR("copy temp buffer to user failed.\n");
        return HI_FAILURE;
    }

    if (counter > 0) {
        g_ci_drv_param[port].card_param[card].io_cnt++;
    }

    *read_len = counter;

    hi_dbg_func_exit();
    return HI_SUCCESS;
} /* ci_pccd_read */

hi_s32 drv_ci_pccd_io_write(hi_ci_port port, hi_ci_pccd card, const hi_u8 *buffer,
    hi_u32 write_len, hi_u32 *write_ok_len)
{
    hi_u32 counter;
    hi_u32 retry_times;
    hi_u16 buffer_size;
    hi_u8 status;
    hi_u8 *send = HI_NULL;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(buffer == HI_NULL);
    HI_LOG_CHECK_PARAM(write_ok_len == HI_NULL);

    /* no need to write, do nothing. */
    if (write_len == 0) {
        return HI_SUCCESS;
    }

    CHECK_CI_PCCD_OPENED(port, card);

    *write_ok_len = 0x00;

    /* check buffer: if null, return. */
    send = g_ci_drv_param[port].card_param[card].buffer;
    if (send == HI_NULL) {
        HI_LOG_ERR("invalid buffer,point is null.\n");
        hi_err_print_err_code(HI_ERR_CI_NO_MEMORY);
        return HI_ERR_CI_NO_MEMORY;
    }

    /* write len must be a safe length. */
    /* please invoke ci_pccd_neg_buffer_size() to negotiate buffer size. */
    buffer_size = g_ci_drv_param[port].card_param[card].buffer_size;
    if (write_len > buffer_size) {
        hi_err_print_err_code(HI_ERR_CI_IO_READ_ERR);
        hi_err_print_u32(write_len);
        hi_err_print_u32(buffer_size);
        return HI_ERR_CI_INVALID_PARA;
    }

    /* copy user data */
    if (osal_copy_from_user(send, buffer, write_len)) {
        HI_LOG_ERR("copy buffer from user failed.\n");
        return HI_FAILURE;
    }

    /* Host to Module Transfers
     * The host sets the HC bit and then tests the FR bit. If FR is '0' then
     * the interface is busy and the host must reset HC and wait a period before
     * repeating the test. If FR is '1' then the host writes the number of bytes
     * it wishes to send to the module into the Size register and then writes that
     * number of data bytes to the Data register. This multiple write shall not be
     * interrupted by any other operations on the interface except for reads of the Status
     * Register. When the first byte is written the module sets WE to '1' and sets FR to '0'.
     * During the transfer the WE bit remains at '1' until the last byte is written,
     * at which point it is set to '0'. If any further bytes are writte then the WE bit
     * is set to '1'. At the end of the transfer the host shall reset the HC bit by writing '0'
     * to it. The host must test the DA bit before initiating the host-to-module cycle above
     * in order to avoid deadlock in the case of a single buffer implementation in the module.
     */
    /* if the module has data avalaible, you should read first */
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
    if ((status & PCCD_DA) == PCCD_DA) {
        HI_LOG_ERR("CI card write error,Data is available!\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return HI_ERR_CI_PCCD_DEVICE_BUSY;
    }

    /* if the module is free, set host control and wait for free again */
    for (retry_times = 0; retry_times < CI_PCCD_RW_TIMEOUT; retry_times++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_SETHC));

        osal_msleep(1);
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
        if ((status & PCCD_FR) == PCCD_FR) {
            break;
        }

        CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_CLEAR));
        osal_msleep(CI_TIME_10MS);
    }

    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
    if ((status & PCCD_FR) != PCCD_FR) {
        HI_LOG_ERR("ci_pccd_write() error: device is busy!\n");
        return HI_ERR_CI_PCCD_DEVICE_BUSY;
    }

    /* the host writes the number of bytes he wants to send to the module */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, SIZE_L_REG, (hi_u8)(write_len & 0xff)));
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, SIZE_H_REG, (hi_u8)(write_len >> BITS_IN_1BYTE)));

    for (counter = 0; counter < write_len; counter++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, DATA_REG, send[counter]));

        /* When the first byte is written the module sets WE to '1'and sets FR to '0'.
         * During the transfer the WE bit remains at '1'until the last byte is written,
         * at which point it is set to '0'.
         */
    }

    /* after write over, write error bit should be 0. */
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
    if ((status & PCCD_WE) == PCCD_WE) {
        hi_err_print_err_code(HI_ERR_CI_IO_WRITE_ERR);
        return HI_ERR_CI_IO_WRITE_ERR;
    }

    /* return control to the module */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_CLEAR));

    if (g_ci_drv_param[port].card_param[card].print_io_data == HI_TRUE) {
        hi_log_print_block(send, counter);
    }

    *write_ok_len = counter;

    hi_dbg_func_exit();
    return HI_SUCCESS;
} /* ci_pccd_write */

static hi_s32 drv_ci_pccd_read_block(hi_ci_port port, hi_ci_pccd card, hi_u8 cis[MAX_CIS_SIZE],
    hi_u32 cibuf_len, hi_u32 *len)
{
    hi_s32 ret;
    hi_u32 retry_times;
    hi_ci_pccd_ready ready;
    hi_u32 tupple_offset;
    hi_u16 offset = 0;
    hi_u8 num;

    hi_dbg_func_enter();

    if (cibuf_len > MAX_CIS_SIZE) {
        HI_LOG_ERR("CIS buffer len err.\n");
        return HI_FAILURE;
    }

    /* wait PCCARD ready */
    for (retry_times = 0; retry_times < CI_PCCD_IORESET_TIMEOUT; retry_times++) {
        ret = hal_ci_pccd_ready_or_busy(port, card, &ready);
        if (ret != HI_SUCCESS) {
            hi_err_print_call_fun_err(hal_ci_pccd_ready_or_busy, ret);
        }
        if (ready == HI_CI_PCCD_READY) {
            break;
        }
        osal_msleep(CI_TIME_10MS);
    }

    if (retry_times == CI_PCCD_IORESET_TIMEOUT) {
        HI_LOG_ERR("card busy.\n");
        hi_err_print_err_code(HI_ERR_CI_PCCD_DEVICE_BUSY);
        return HI_ERR_CI_PCCD_DEVICE_BUSY;
    }

    while (1) {
        tupple_offset = offset * CIS_EVEN_ADDR; /* CIS is valid in the even byte, so offset*2 */

        if (offset >= MAX_CIS_SIZE - 1) {
            HI_LOG_ERR("CIS size overflow.\n");
            hi_err_print_h32(offset);
            hi_err_print_err_code(HI_ERR_CI_PCCD_CIS_READ);
            return HI_ERR_CI_PCCD_CIS_READ;
        }

        /* read tupple tag, 1 byte. */
        hal_ci_pccd_mem_read_byte(port, card, tupple_offset, &(cis[offset]));
        hi_dbg_print_h32(offset);
        hi_dbg_print_h32(cis[offset]);

        /* check tupple tag */
        switch (cis[offset]) {
            case CISTPL_DEVICE:
            case CISTPL_CHECKSUM:
            case CISTPL_LINKTARGET:
            case CISTPL_NO_LINK:
            case CISTPL_VERS_1:
            case CISTPL_ALTSTR:
            case CISTPL_DEVICE_A:
            case CISTPL_CONFIG:
            case CISTPL_CFTABLE_ENTRY:
            case CISTPL_DEVICE_OC:
            case CISTPL_DEVICE_OA:
            case CISTPL_MANFID:
            case CISTPL_FUNCID:
            case CISTPL_END:
                break;
            default:
                hi_err_print_h32(cis[offset]);
                return HI_ERR_CI_PCCD_CIS_READ;
        } /* end switch */

        hal_ci_pccd_mem_read_byte(port, card, tupple_offset + CIS_EVEN_ADDR,  /* 2: even address */
            &(cis[offset + 1]));  /* read tupple link, 1 byte. */

        /* if CISTPL_END, break; */
        if (cis[offset] == CISTPL_END) {
            offset += 1;
            break;
        }

        /* prevent cis arrays from crossing the boundary */
        if (cis[offset + 1] - 1 + CIS_EVEN_ADDR + offset >= MAX_CIS_SIZE) {
            return HI_ERR_CI_PCCD_CIS_READ;
        }

        /* reading the tupple data */
        for (num = 0; num < cis[offset + 1]; num++) {
            hal_ci_pccd_mem_read_byte(port, card, tupple_offset + 4 + num * 2,  /* tag and link 2 bytes, so add 4 */
                &(cis[offset + CIS_EVEN_ADDR + num]));
        }

        /* next tupple */
        offset += CIS_EVEN_ADDR + cis[offset + 1];
    }

    *len = offset;
    hi_dbg_func_exit();
    return HI_SUCCESS;
}

static hi_s32 check_cioa_oc(ci_pccd_tupple_ptr tupple, ci_pccd_volt *volt, ci_pccd_speed *speed)
{
    hi_u8 bit1;
    hi_u8 bit2;
    hi_u8 device_type_code;
    hi_u8 i;

    hi_dbg_func_enter();

    /* other conditions info */
    /* ext voltage, unsupport now */
    if (MASK_BIT_7 & tupple->tupple_data[0]) {
        *volt = CI_PCCD_VOLT_5V;
    } else {
        bit1 = tupple->tupple_data[0] & MASK_BIT_1;
        bit2 = tupple->tupple_data[0] & MASK_BIT_2;

        /* 00b, 5V */
        if ((!bit2) && (!bit1)) {
            *volt = CI_PCCD_VOLT_5V;
        } else if ((!bit2) && (bit1)) { /* 01b, 3V3 */
            *volt = CI_PCCD_VOLT_3V3;
        } else if ((bit2) && (!bit1)) { /* 10b, X.X volt VCC, unsupport now */
            *volt = CI_PCCD_VOLT_5V;
        } else { /* 11b, card_bus PC card Y.Y volt VCC, unsupport now */
            *volt = CI_PCCD_VOLT_5V;
        }

        HI_LOG_INFO("CIS_OA_OC get voltage.\n");
    }

    /* device info, from byte 1 */
    for (i = 1; i < tupple->tupple_link; i++) {
        device_type_code = tupple->tupple_data[i] >> 4; /* type code: 4bit */
        HI_LOG_INFO("CIS_OA_OC get device_type_code.\n");

        if ((device_type_code == DTYPE_NULL) || (device_type_code == DTYPE_RESERVED)) {
            continue;
        }

        /* don't support DSPEED_EXT */
        *speed = tupple->tupple_data[i] & DSPEED_MASK;
        HI_LOG_INFO("CIS_OA_OC: speed.\n");
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

static hi_u8 char_lower_case(const hi_u8 ch)
{
    hi_u8 temp;

    hi_dbg_func_enter();
    if (ch >= 'A' && ch <= 'Z') {
        temp = ch + ('a' - 'A');
    } else {
        temp = ch;
    }

    hi_dbg_func_exit();
    return temp;
}

static const hi_u8 *string_searach(const hi_u8 *str, const hi_u8 *sudtr, hi_bool icase_sensitive)
{
    hi_s32 i;
    hi_u32 str_len;
    hi_u32 sustr_len;
    hi_u32 index;
    const hi_u8 *target = HI_NULL_PTR;

    hi_dbg_func_enter();

    str_len = strlen(str);
    sustr_len = strlen(sudtr);
    if (str_len == 0 || sustr_len == 0 || str_len < sustr_len) {
        return HI_NULL_PTR;
    }
    index = 0;
    target = &str[0];
    if (icase_sensitive == HI_TRUE) { /* case insensitive */
        for (i = 0; i < sustr_len; i++, index++) {
            if (char_lower_case(str[index]) != char_lower_case(sudtr[i])) {
                break;
            }
        }
    } else { /* case sensitive */
        for (i = 0; i <= sustr_len; i++, index++) {
            if (str[index] != sudtr[i]) {
                break;
            }
        }
    }
    if (i == sustr_len - 1) {
        ;
    } else {
        target = HI_NULL_PTR;
        return target;
    }
    hi_dbg_func_exit();
    return target;
}

static hi_bool check_ciplus(hi_u8 *buf, hi_u32 len, hi_u32 *ciprof)
{
    const hi_u8 *target = HI_NULL_PTR;
    *ciprof = 0;

    hi_dbg_func_enter();

    target = string_searach(buf, "$compatible[ciplus", HI_FALSE);
    if (target == HI_NULL_PTR) {
        return HI_FALSE;
    }
    HI_LOG_INFO("\"$compatible[ciplus\" appear in the additional"
                " product information string\n");
    target = string_searach(target, "ciprof=1", HI_FALSE);
    if (target != HI_NULL_PTR) {
        HI_LOG_INFO("\"ciplus\" appear in the additional product information string\n");
        *ciprof = 1;
    }

    hi_dbg_func_exit();
    return HI_TRUE;
}

/* TPLLV1_MAJOR: 0x05  TPLLV1_MINOR: 0x00 */
static hi_s32 check_civers1(ci_pccd_tupple_ptr tupple, hi_bool *ciplus, hi_u32 *ciprof)
{
    hi_dbg_func_enter();

    if ((tupple->tupple_data[0] != TPLLV1_MAJOR)
        || (tupple->tupple_data[1] != TPLLV1_MINOR)) {
        HI_LOG_ERR("invalid CISTPL_VERS_1\n");
        return HI_FAILURE;
    }
    if (check_ciplus(tupple->tupple_data, MAX_TUPPLE_BUFFER, ciprof) == HI_TRUE) {
        HI_LOG_INFO("check CIS, is a CI+ CAM.\n");
        *ciplus = HI_TRUE;
    } else {
        HI_LOG_INFO("check CIS, is a CI CAM.\n");
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

/* must has this tupple, just check length */
static hi_s32 check_cimanf_id(ci_pccd_tupple_ptr tupple)
{
    hi_dbg_func_enter();
    if ((tupple->tupple_link < TPLLMANFID_SIZE)) {
        HI_LOG_ERR("invalid CISTPL_MANFID\n");
        return HI_FAILURE;
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

static hi_u32 cis_read_tpcc_addr(hi_u32 *tpcc_radr_data, hi_u8 *tupple_data, hi_u32 len, hi_u8 tpcc_rasz)
{
    hi_u32 tpcc_radr = 0;
    hi_u32 index = 0;

    index++; /* head */
    index++; /* head */

    if (len < TUPPLE_DATA_MIN_LEN) {
        HI_LOG_ERR("tupple_data len err!\n");
        return HI_FAILURE;
    }

    switch (tpcc_rasz) {
        case 0: {  /* 0 means 1 byte */
            tpcc_radr = tupple_data[index];
            break;
        }
        case 1: { /* 1 means 2 byte */
            tpcc_radr  = tupple_data[index++];
            tpcc_radr |= (hi_u32)(tupple_data[index++]) << BITS_IN_1BYTE;
            break;
        }
        case 2: { /* 2 means 3 byte */
            tpcc_radr  = tupple_data[index++];
            tpcc_radr |= (hi_u32)(tupple_data[index++]) << BITS_IN_1BYTE;
            tpcc_radr |= (hi_u32)(tupple_data[index++]) << BITS_IN_2BYTE;
            break;
        }
        case 3: { /* 3 means 4 byte */
            tpcc_radr  = tupple_data[index++];
            tpcc_radr |= (hi_u32)(tupple_data[index++]) << BITS_IN_1BYTE;
            tpcc_radr |= (hi_u32)(tupple_data[index++]) << BITS_IN_2BYTE;
            tpcc_radr |= (hi_u32)(tupple_data[index++]) << BITS_IN_3BYTE;
            break;
        }
    }

    *tpcc_radr_data = tpcc_radr;

    return HI_SUCCESS;
}

static hi_s32 check_ciconfig(ci_pccd_tupple_ptr tupple, hi_u32 *tpcc_radr)
{
    hi_u8 tpcc_sz;
    hi_u8 tpcc_rmsk;
    hi_u8 tpcc_rfsz;
    hi_u8 tpcc_rmsz;
    hi_u8 tpcc_rasz;
    hi_u32 local_tpcc_radr;
    hi_u8 subt_tupple_tag;
    hi_u8 subt_tupple_len;
    hi_u8 *psubt_tupple_value = HI_NULL;
    hi_u8 ret;

    hi_dbg_func_enter();

    if (tupple->tupple_link < 5) {  /* 5: max len of link */
        HI_LOG_ERR("invalid CISTPL_CONFIG\n");
        return HI_FAILURE;
    }

    tpcc_sz = tupple->tupple_data[0];

    /* don't care TPCC_LAST here: tupple->tupple_data[1]. */
    tpcc_rfsz = tpcc_sz & TPCC_RFSZ_MASK;
    tpcc_rmsz = tpcc_sz & TPCC_RMSZ_MASK;
    tpcc_rasz = tpcc_sz & TPCC_RASZ_MASK;

    ret = cis_read_tpcc_addr(&local_tpcc_radr, tupple->tupple_data, MAX_TUPPLE_BUFFER, tpcc_rasz);
    if (ret == HI_FAILURE) {
        HI_LOG_ERR("cis_read_tpcc_addr err.\n");
        return HI_FAILURE;
    }

    /* tpcc_radr can't larger than 0xffe */
    if (local_tpcc_radr > COR_BASE_ADDRESS_UPPERLIMIT) {
        HI_LOG_ERR("invalid tpcc_radr.\n");
        hi_err_print_u32(local_tpcc_radr);
        return HI_FAILURE;
    }

    /* tpcc_rmsz should be 0, don't care tpcc_rmsk */
    if (tpcc_rmsz != 0) {
        HI_LOG_ERR("more than 1 configuration register\n");
        hi_err_print_h32(tpcc_rmsz);
        return HI_FAILURE;
    }

    tpcc_rmsk = tupple->tupple_data[tpcc_rasz + 3]; /* 3: index of mask */

    /* sutupple */
    subt_tupple_tag = tupple->tupple_data[tpcc_rasz + 4]; /* 4: index of tag */
    subt_tupple_len = tupple->tupple_data[tpcc_rasz + 5]; /* 5: index of len */
    psubt_tupple_value = tupple->tupple_data + tpcc_rasz + 6; /* 6: index of value */

    /* DVB_CI_V1.00, 0x0241 */
    if (!(((osal_memncmp(STCF_LIB, STCF_LIB_LEN, (hi_char *)psubt_tupple_value + CIS_EVEN_ADDR, STCF_LIB_LEN)) == 0)
          && (psubt_tupple_value[1] == STCI_IFN_1) && (psubt_tupple_value[0] == STCI_IFN))) {
        HI_LOG_ERR("check STCF_LIB fail.\n");
        return HI_FAILURE;
    }

    *tpcc_radr = local_tpcc_radr;

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

static hi_void check_citiming(hi_u8 tpcefs, hi_u8 **ptpce_io, hi_u8 **ptpce_td)
{
    /* timing */
    if (tpcefs & MASK_BIT_2) {
        switch ((**ptpce_td) & RW_SCALE_MASK) {
            case NOT_READY_WAIT:
                *ptpce_io = (*ptpce_td) + 1;
                break;
            case WAIT_SCALE_0:
            case WAIT_SCALE_1:
            case WAIT_SCALE_2:
                *ptpce_io = (*ptpce_td) + CIS_EVEN_ADDR;
                break;
            case READY_SCALE_0:
            case READY_SCALE_1:
            case READY_SCALE_2:
            case READY_SCALE_3:
            case READY_SCALE_4:
            case READY_SCALE_5:
            case READY_SCALE_6:
                *ptpce_io = (*ptpce_td) + CIS_EVEN_ADDR;
                break;
            default:
                *ptpce_io = (*ptpce_td) + 1 + CIS_EVEN_ADDR;
                break;
        }
    } else {
        *ptpce_io = *ptpce_td;
        HI_LOG_INFO("no timing descriptor.\n");
    }

    HI_LOG_INFO("no timing descriptor.\n");
}

static hi_s32 check_cicf_table_entry(ci_pccd_tupple_ptr tupple, hi_u8 *pcor_value)
{
    hi_u8 tpce_if;
    hi_u8 tpcefs;
    hi_u8 power_description_bits;
    hi_u8 tpce_indx;
    hi_u8 tpce_intface;
    hi_u8 tpce_default;
    hi_u8 tpce_pd_spsb; /* structure parameter selection byte */
    hi_u8 tpce_pd_spd;  /* structure parameter definition */
    hi_u8 tpce_pd_spdx; /* extension structure parameter definition */
    hi_u8 tpce_io_range;
    hi_u8 tpce_io_bm;  /* I/O bus mapping */
    hi_u8 tpce_io_al;  /* I/O address line */
    hi_u8 tpce_io_rdb; /* I/O range descriptor byte */
    hi_u8 tpce_io_sol; /* size of Length I/O address range */
    hi_u8 tpce_io_soa; /* size of address I/O address range */
    hi_u8 tpce_io_nar; /* number of I:O address range */
    hi_u8 *ptpce_sutupple = HI_NULL;
    hi_u8 *ptpce_td = HI_NULL;
    hi_u8 *ptpce_io_rdffci = HI_NULL;
    hi_u8 *ptpce_io = HI_NULL;
    hi_u8 *ptpce_ir = HI_NULL;
    hi_u8 *ptpce_ms = HI_NULL;
    hi_u32 tpce_io_block_start;
    hi_u32 tpce_io_block_size;
    hi_u8 *ptpce_pd = HI_NULL;
    hi_s32 tpce_loop;
    hi_bool haio_space = HI_TRUE;

    hi_dbg_func_enter();

    tpce_indx = tupple->tupple_data[0];
    tpce_intface = tpce_indx & MASK_BIT_7;
    tpce_default = tpce_indx & MASK_BIT_6;
    tpce_indx = tpce_indx & TPCE_INDX_MASK;

    /* TPCE_INDX has both bits 6 (default) and 7 (intface) set. */
    if (!(tpce_intface && tpce_default)) {
        HI_LOG_ERR("CIS check error, the default bit or "
                   "intface bit in TPCE_INDX is not set.\n");
        return HI_FAILURE;
    }

    tpce_if = tupple->tupple_data[1]; /* 1: index of tpce_if */
    tpcefs = tupple->tupple_data[2];  /* 2: index of tpcefs */
    ptpce_pd = tupple->tupple_data + 3; /* 3: index of ptpce_pd */

    power_description_bits = tpcefs & (MASK_BIT_1 | MASK_BIT_0);

    /* power configuration */
    while (power_description_bits > 0) {
        for (tpce_pd_spsb = *ptpce_pd++; tpce_pd_spsb > 0; tpce_pd_spsb >>= 1) {
            if (tpce_pd_spsb & MASK_BIT_0) {
                tpce_pd_spd = *ptpce_pd++;

                while (*ptpce_pd & MASK_BIT_7) {
                    tpce_pd_spdx = *ptpce_pd++;
                }
            }
        }

        power_description_bits--;
        hi_dbg_print_h32(power_description_bits);
    }
    ptpce_td = ptpce_pd;
    ptpce_pd = HI_NULL;

    check_citiming(tpcefs, &ptpce_io, &ptpce_td);

    /* I/O space */
    if (tpcefs & MASK_BIT_3) {
        ptpce_ir = ptpce_io + 1;
        tpce_io_range = (*ptpce_io) & (MASK_BIT_7);
        tpce_io_bm = (((*ptpce_io) & (MASK_BIT_5)) | (MASK_BIT_6)) >> 5; /* 5-bits offset of tpce_io_bm */
        tpce_io_al = (*ptpce_io) & IO_ADDLINES_MASK;

        if (tpce_io_range) {
            /* I/O range descriptor byte */
            tpce_io_rdb = ptpce_io[1];
            /* size of Length I/O address range */
            tpce_io_sol = (tpce_io_rdb & (MASK_BIT_7 | MASK_BIT_6)) >> 6; /* 6-bits offset of tpce_io_sol */
            /* size of address I/O address range */
            tpce_io_soa = (tpce_io_rdb & (MASK_BIT_5 | MASK_BIT_4)) >> 4; /* 4-bits offset of tpce_io_soa */
            /* number of I:O address range */
            tpce_io_nar = (tpce_io_rdb & (MASK_BIT_3 | MASK_BIT_2 | MASK_BIT_1 | MASK_BIT_0)) + 1;

            ptpce_io_rdffci = ptpce_io + CIS_EVEN_ADDR;
            for (tpce_loop = 0; tpce_loop < tpce_io_nar; tpce_loop++) {
                tpce_io_block_start = ptpce_io_rdffci[(tpce_io_sol + tpce_io_soa) * tpce_loop];
                if (tpce_io_soa > 1) {
                    tpce_io_block_start |= ((hi_u32)ptpce_io_rdffci[(tpce_io_sol
                        + tpce_io_soa) * tpce_loop + 1]) << BITS_IN_1BYTE;
                }
                tpce_io_block_size = ptpce_io_rdffci[(tpce_io_sol + tpce_io_soa) * tpce_loop + tpce_io_soa] + 1;
            }

            ptpce_ir = ptpce_io_rdffci + (tpce_io_sol + tpce_io_soa) * tpce_io_nar;
        } else {
            ptpce_ir = ptpce_io + 1;
        }
    } else {
        ptpce_ir = ptpce_io;
        haio_space = HI_FALSE;
        HI_LOG_ERR("CIS check error,no I/O space descriptor.");
        hi_err_print_h32(tpcefs);
    }

    /* IRQ */
    if (tpcefs & MASK_BIT_4) {
        ptpce_ms = ptpce_ir + 1;
    } else {
        ptpce_ms = ptpce_ir;
    }

    /* memory space */
    switch ((tpcefs & (MASK_BIT_6 | MASK_BIT_6)) >> 5) { /* 5-bits offset of memory space */
        case 0: /* find ptpce_sutupple at index 0 */
            ptpce_sutupple = ptpce_ms;
            ptpce_ms = HI_NULL;
            break;
        case 1: /* find ptpce_sutupple at index 1 */
            ptpce_sutupple = ptpce_ms + 1;
            break;
        case 2: /* find ptpce_sutupple at index 2 */
            ptpce_sutupple = ptpce_ms + CIS_EVEN_ADDR;
            break;
        case 3: /* find ptpce_sutupple at index 3 */
            ptpce_sutupple = ptpce_ms + 1 + CIS_EVEN_ADDR;
            break;
        default:
            break;
    }

    if (haio_space) {
        /* TPCE_IF = 04h - indicating custom interface 0. en50221 */
        /* TPCE_IO is a 1-byte field with the value 22h. en50221 */
        if (!((tpce_if == TPCE_IF) && (*ptpce_io == TPCE_IO))) {
            return HI_FAILURE;
        }
    }

    *pcor_value = tpce_indx;

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_check_cis(hi_ci_port port, hi_ci_pccd card)
{
    hi_s32 ret;
    ci_pccd_tupple current_tupple;
    hi_u16 offset = 0;
    hi_bool checked_oaoc = HI_FALSE;
    hi_bool checked_vers1 = HI_FALSE;
    hi_bool checked_manf_id = HI_FALSE;
    hi_bool checked_config = HI_FALSE;
    hi_bool checked_cf_table_entry = HI_FALSE;
    hi_bool checked_all = HI_FALSE;
    hi_u32 tpcc_radr;
    hi_u8 cor_value;
    ci_pccd_volt volt = CI_PCCD_VOLT_5V;
    ci_pccd_speed speed = CI_PCCD_SPEED_150NS;
    hi_bool iciplus = HI_FALSE;
    hi_u32 ciprof;
    hi_u8 acis[MAX_CIS_SIZE] = {0};
    hi_u32 cilen = sizeof(acis);

    hi_dbg_func_enter();

    /* valid CI and card and make sure they are opened. */
    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_1);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_PCCD_OPENED(port, card);

    ret = drv_ci_pccd_read_block(port, card, acis, sizeof(acis), &cilen);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("read CIS block failed.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    ciprof    = 0;
    cor_value = 0;
    tpcc_radr = 0;

    while (!checked_all) {
        if (offset >= MAX_CIS_SIZE - 1) {
            HI_LOG_ERR("Offset overflowed !\n");
            return HI_FAILURE;
        }

        current_tupple.tupple_tag = acis[offset];
        current_tupple.tupple_link = acis[offset + 1];

        if (((offset + CIS_EVEN_ADDR + current_tupple.tupple_link) >= MAX_CIS_SIZE) ||
           (current_tupple.tupple_link >= MAX_TUPPLE_BUFFER)) {
            HI_LOG_ERR("Offset overflowed !\n");
            return HI_FAILURE;
        }

        CHECK_FUNC_RET(memcpy_s(current_tupple.tupple_data, MAX_TUPPLE_BUFFER,
                                &acis[offset + CIS_EVEN_ADDR], current_tupple.tupple_link));
        current_tupple.tupple_data[current_tupple.tupple_link] = '\0';

        if (current_tupple.tupple_tag == CISTPL_END) {
            hi_dbg_func_trace();
            break;
        }

        if (current_tupple.tupple_link == 0) {
            hi_dbg_func_trace();
            break;
        }

        switch (current_tupple.tupple_tag) { /* check for the tupple tag */
            case CISTPL_DEVICE_OA:  /* fall-through */
            case CISTPL_DEVICE_OC:
                ret = check_cioa_oc(&current_tupple, &volt, &speed);
                if (ret == HI_SUCCESS) {
                    checked_oaoc = HI_TRUE;
                    hi_dbg_func_trace();
                }
                break;

            case CISTPL_VERS_1:
                ret = check_civers1(&current_tupple, &iciplus, &ciprof);
                if (ret == HI_SUCCESS) {
                    checked_vers1 = HI_TRUE;
                    hi_dbg_func_trace();
                }
                break;

            case CISTPL_MANFID:
                ret = check_cimanf_id(&current_tupple);
                if (ret == HI_SUCCESS) {
                    checked_manf_id = HI_TRUE;
                    hi_dbg_func_trace();
                }
                break;

            case CISTPL_CONFIG:
                ret = check_ciconfig(&current_tupple, &tpcc_radr);
                if (ret == HI_SUCCESS) {
                    checked_config = HI_TRUE;
                    hi_dbg_func_trace();
                }
                break;

            case CISTPL_CFTABLE_ENTRY:
                ret = check_cicf_table_entry(&current_tupple, &cor_value);
                if (ret == HI_SUCCESS) {
                    checked_cf_table_entry = HI_TRUE;
                    hi_dbg_func_trace();
                }
                break;

            default:
                hi_dbg_func_trace();
                break;
        } /* end switch */

        if (checked_oaoc && checked_vers1 && checked_manf_id && checked_config && checked_cf_table_entry) {
            checked_all = HI_TRUE;
        }

        offset += CIS_EVEN_ADDR + current_tupple.tupple_link;
    } /* end while */
    if (!checked_all) {
        HI_LOG_ERR("check_cis fail.\n");
        g_ci_drv_param[port].card_param[card].attr.volt = CI_PCCD_VOLT_5V;
        g_ci_drv_param[port].card_param[card].attr.speed = CI_PCCD_SPEED_150NS;
        g_ci_drv_param[port].card_param[card].tpcc_radr = 0;
        g_ci_drv_param[port].card_param[card].tpce_indx = 0;
        hi_err_print_err_code(HI_ERR_CI_PCCD_CIS_READ);
        return ret;
    } else {
        g_ci_drv_param[port].card_param[card].attr.volt = volt;
        if ((speed >= CI_PCCD_SPEED_MAX) || (speed < CI_PCCD_SPEED_250NS)) {
            speed = CI_PCCD_SPEED_600NS;
        }

        g_ci_drv_param[port].card_param[card].attr.speed = speed;
        g_ci_drv_param[port].card_param[card].attr.is_ciplus = iciplus;
        g_ci_drv_param[port].card_param[card].attr.ciprof = ciprof;
        g_ci_drv_param[port].card_param[card].tpcc_radr = tpcc_radr;
        g_ci_drv_param[port].card_param[card].tpce_indx = cor_value;

        ret = HI_SUCCESS;
    }

    hi_dbg_func_exit();
    return ret;
} /* ci_pccd_check_cis */

/***********************************************************************************
 * function:      ci_pccd_write_cor
 * Description:   write TPCE_INDX to TPCC_RADR
 *                please refer to EN50221 specification.
 *********************************************************************************/
hi_s32 drv_ci_pccd_write_cor(hi_ci_port port, hi_ci_pccd card)
{
    hi_u16 addr;
    hi_u8  val;

    hi_dbg_func_enter();

    /* valid CI and card and make sure they are opened. */
    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_PCCD_OPENED(port, card);

    addr = g_ci_drv_param[port].card_param[card].tpcc_radr;
    val  = g_ci_drv_param[port].card_param[card].tpce_indx;

    if ((addr == 0x00) || (addr > COR_BASE_ADDRESS_UPPERLIMIT)) {
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        HI_LOG_ERR("invalid COR address.\n");
        hi_err_print_h32(addr);
        return HI_FAILURE;
    }

    /* if registe io_write function, use it to write directly */
    hal_ci_pccd_mem_write_byte(port, card, addr, val);

    /* Check write OK ? */
    hal_ci_pccd_mem_read_byte(port, card, addr, &val);
    if (val != g_ci_drv_param[port].card_param[card].tpce_indx) {
        HI_LOG_ERR("write_cor failed.\n");
        hi_err_print_err_code(HI_ERR_CI_ATTR_WRITE_ERR);
        return HI_ERR_CI_ATTR_WRITE_ERR;
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
} /* ci_pccd_write_cor */

/***********************************************************************************
 * function:      ci_pccd_io_reset
 * Description:   reset the IO interface
 *********************************************************************************/
hi_s32 drv_ci_pccd_io_reset(hi_ci_port port, hi_ci_pccd card)
{
    hi_u16 retry_times = 0;
    hi_u8 status;

    hi_dbg_func_enter();

    /* valid CI and card and make sure they are opened. */
    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_PCCD_OPENED(port, card);

    /* reset the command register */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_SETRS));
    osal_msleep(CI_TIME_10MS);

    /* clear the reset bit */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_CLEAR));

    /* wait for free */
    for (retry_times = 0; retry_times < CI_PCCD_IORESET_TIMEOUT; retry_times++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
        if (status & PCCD_FR) {
            break;
        }
        osal_msleep(CI_TIME_10MS);
    }

    if (!(status & PCCD_FR)) {
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        HI_LOG_ERR("CI PCCD card ioreset timeout.\n");
        hi_err_print_err_code(HI_ERR_CI_TIMEOUT);
        return HI_ERR_CI_TIMEOUT;
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
} /* ci_pccd_io_reset */

/***********************************************************************************
 * function:      ci_pccd_neg_buffer_size
 * Description:   negotiate about buffer size
 *********************************************************************************/
hi_s32 drv_ci_pccd_neg_buffer_size(hi_ci_port port, hi_ci_pccd card, hi_u16 *buffer_size)
{
    hi_u8 size[IO_DATA_LEN] = { 0, 0 };
    hi_u16 retry_cnt;
    hi_u16 data_size;
    hi_u16 temp_size;
    hi_u16 hosize;
    hi_u8 buffer_count;
    hi_u8 data_low;
    hi_u8 data_high;
    hi_u8 status = 0;

    hi_dbg_func_enter();

    /* valid CI and card and make sure they are opened. */
    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(buffer_size == HI_NULL);

    /* get host buffer size */
    hosize = *buffer_size;

    CHECK_CI_PCCD_OPENED(port, card);

    /* set size read bit */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_SETSR));

    /* wait for Data available */
    for (retry_cnt = 0; retry_cnt < CI_PCCD_NEGBUFFER_TIMEOUT; retry_cnt++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
        if (status & PCCD_DA) {
            break;
        }
        osal_msleep(CI_TIME_1MS);
    }

    if (!(status & PCCD_DA)) {
        HI_LOG_ERR("CI PCCD card, set SR bit time out\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_err_code(HI_ERR_CI_TIMEOUT);
        return HI_ERR_CI_TIMEOUT;
    }

    /* read buffer size's size */
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, SIZE_L_REG, &data_low));
    CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, SIZE_H_REG, &data_high));
    data_size = data_low | (data_high << BITS_IN_1BYTE);

    if ((data_size == 0) || (data_size > IO_DATA_LEN)) {
        HI_LOG_ERR("CI PCCD card, buffer size's size is error.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_u32(data_size);
        hi_err_print_err_code(HI_ERR_CI_IO_READ_ERR);
        return HI_ERR_CI_IO_READ_ERR;
    }

    /* read buffer size */
    temp_size = 0;
    for (buffer_count = 0; buffer_count < data_size; buffer_count++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, DATA_REG, &size[buffer_count]));
        temp_size = (temp_size << BITS_IN_1BYTE) | size[buffer_count];
    }

    /* clear command register */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_CLEAR));

    if (hosize != 0) {
        if (temp_size > hosize) {
            temp_size = hosize;
            size[0] = (hi_u8)(temp_size >> BITS_IN_1BYTE);
            size[1] = (hi_u8)(temp_size & 0x00ff);
            data_size = IO_DATA_LEN;
        }
    }

    if (temp_size < CI_PCCD_MIN_BUFFERSIZE) {
        HI_LOG_ERR("CI PCCD card, buffer size too small.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_u32(temp_size);
        hi_err_print_err_code(HI_ERR_CI_IO_READ_ERR);
        return HI_ERR_CI_IO_READ_ERR;
    }

    osal_msleep(CI_TIME_1MS);

    /* set size write bit */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_SETSW));

    /* wait for free */
    for (retry_cnt = 0; retry_cnt < CI_PCCD_NEGBUFFER_TIMEOUT; retry_cnt++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
        if (status & PCCD_FR) {
            break;
        }
        osal_msleep(CI_TIME_1MS);
    }

    if (!(status & PCCD_FR)) {
        HI_LOG_ERR("CI PCCD card, set SW bit time out.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_err_code(HI_ERR_CI_TIMEOUT);
        return HI_ERR_CI_TIMEOUT;
    }

    /* set host control */
    for (retry_cnt = 0; retry_cnt < CI_PCCD_NEGBUFFER_TIMEOUT; retry_cnt++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_SETHC));
        osal_msleep(CI_TIME_1MS);
        status = 0;
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
        if (status & PCCD_FR) {
            break;
        }

        CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_CLEAR));
        osal_msleep(CI_TIME_1MS);
    }

    if (!(status & PCCD_FR)) {
        HI_LOG_ERR("CI PCCD card, set HC bit time out\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_err_code(HI_ERR_CI_TIMEOUT);
        return HI_ERR_CI_TIMEOUT;
    }

    /* write buffer size's size */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, SIZE_L_REG, (hi_u8)(data_size & 0xff)));
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, SIZE_H_REG, (hi_u8)(data_size >> BITS_IN_1BYTE)));

    /* write buffer size */
    for (buffer_count = 0; buffer_count < data_size; buffer_count++) {
        CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, DATA_REG, size[buffer_count]));
        status = 0;
        CHECK_FUNC_RET(hal_ci_pccd_io_read_byte(port, card, COM_STAT_REG, &status));
        if (!(status & PCCD_WE)) {
            break;
        }
    }

    /* clear command register and wait f_ree */
    CHECK_FUNC_RET(hal_ci_pccd_io_write_byte(port, card, COM_STAT_REG, PCCD_CLEAR));
    *buffer_size = temp_size;

    /* malloc buffer: if buffer had been allocated, free it and allocate it using new buffer size. */
    if (g_ci_drv_param[port].card_param[card].buffer != HI_NULL) {
        HI_VFREE_CI(g_ci_drv_param[port].card_param[card].buffer);
        g_ci_drv_param[port].card_param[card].buffer = HI_NULL;
    }

    g_ci_drv_param[port].card_param[card].buffer = HI_VMALLOC_CI(temp_size);
    if (g_ci_drv_param[port].card_param[card].buffer != HI_NULL) {
        g_ci_drv_param[port].card_param[card].buffer_size = temp_size;
    } else {
        HI_LOG_ERR("CI PCCD card alloc memory fail.\n");
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return HI_FAILURE;
    }

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_ts_ctrl(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_tsctrl cmd, hi_ci_pccd_tsctrl_param *param)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(cmd != HI_CI_PCCD_TSCTRL_BYPASS);
    HI_LOG_CHECK_PARAM(param == HI_NULL);

    CHECK_CI_PCCD_OPENED(port, card);

    ret = hal_ci_pccd_ts_by_pass(port, card, param->bypass.ts_bypass);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_pccd_ts_by_pass, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    hi_dbg_func_exit();
    return ret;
}

hi_s32 drv_ci_pccd_dbg_io_print_ctrl(hi_ci_port port, hi_ci_pccd card, hi_bool print)
{
    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    g_ci_drv_param[port].card_param[card].print_io_data = print;
    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_get_debug_info(hi_ci_port port, hi_ci_pccd card, ci_pccd_debuginfo_ptr debug_info)
{
    hi_s32 ret;

    hi_dbg_func_enter();

    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);
    HI_LOG_CHECK_PARAM(debug_info == NULL);

    /* detect card */
    ret = hal_ci_pccd_detect(port, card, &(debug_info->status));
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_pccd_detect, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    /* ready or busy */
    ret = hal_ci_pccd_ready_or_busy(port, card, &(debug_info->ready));
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_pccd_ready_or_busy, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    /* get TS mode */
    ret = hal_ci_pccd_get_bypass_mode(port, card, &(debug_info->by_pass));
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_pccd_get_bypass_mode, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        return ret;
    }

    debug_info->is_ciplus = g_ci_drv_param[port].card_param[card].attr.is_ciplus;
    debug_info->buffer_size = g_ci_drv_param[port].card_param[card].buffer_size;
    debug_info->attr = g_ci_drv_param[port].attr;
    debug_info->io_cnt = g_ci_drv_param[port].card_param[card].io_cnt;

    hi_dbg_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_write_cor_ex(hi_ci_port port, hi_ci_pccd card, hi_u16 addr, hi_u8 data)
{
    hi_u8 cor_index;

    hi_dbg_func_enter();

    /* valid CI and card and make sure they are opened. */
    HI_LOG_CHECK_PARAM(port >= HI_CI_PORT_MAX);
    HI_LOG_CHECK_PARAM(card >= HI_CI_PCCD_B);

    CHECK_CI_PCCD_OPENED(port, card);

    if (addr > COR_BASE_ADDRESS_UPPERLIMIT) {
        HI_LOG_ERR("invalid COR_RADR\n");
        hi_err_print_h32(addr);
        return HI_FAILURE;
    }

    /* writing the COR index to the COR address */
    hal_ci_pccd_mem_write_byte(port, card, addr, data);
    HI_LOG_INFO("card write data to addr.\n");

    cor_index = 0;
    hal_ci_pccd_mem_read_byte(port, card, addr, &cor_index);
    if (cor_index != data) {
        HI_LOG_ERR("card write data to addr failed,read cor index.\n");
        hi_err_print_u32(port);
        hi_err_print_h32(data);
        hi_err_print_h32(addr);
        hi_err_print_h32(cor_index);
        return HI_ERR_CI_ATTR_WRITE_ERR;
    }

    return HI_SUCCESS;
}

hi_s32 drv_ci_pccd_get_cis(hi_ci_port port, hi_ci_pccd card, hi_u8 *cis, hi_u32 cis_len, hi_u32 *cilen)
{
    hi_s32 ret;
    hi_u32 len;
    hi_u8 acis[MAX_CIS_SIZE];

    CHECK_CIPORT_VALID(port);
    CHECK_PCCD_VALID(card);

    CHECK_CI_PCCD_OPENED(port, card);

    if (cis == HI_NULL) {
        HI_LOG_ERR("invalid para.\n");
        return HI_ERR_CI_INVALID_PARA;
    }

    ret = drv_ci_pccd_read_block(port, card, acis, sizeof(acis), &len);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("drv_ci_pccd_read_block fail.\n");
        hi_err_print_call_fun_err(drv_ci_pccd_read_block, ret);
        hi_err_print_u32(port);
        hi_err_print_u32(card);
        hi_err_print_h32(len);
        return ret;
    }

    if ((cis_len < len) || (len > MAX_CIS_SIZE)) {
        HI_LOG_ERR("drv_ci_pccd_read_block fail.\n");
        return HI_FAILURE;
    }

    ret = osal_copy_to_user(cis, acis, len);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(osal_copy_to_user, ret);
        return HI_FAILURE;
    }
    *cilen = len;

    return HI_SUCCESS;
}


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher hw hal function impl.
 */

#include "hi_type.h"
#include "hi_osal.h"

#include "hi_drv_sys.h"
#include "hi_reg_common.h"

#include "hal_tsr2rcipher.h"
#include "drv_tsr2rcipher_define.h"
#include "drv_tsr2rcipher_func.h"
#include "drv_tsr2rcipher_reg.h"

#ifndef HI_REG_READ
#define HI_REG_READ(addr,result)  ((result) = *(volatile unsigned int *)(addr))
#endif

#ifndef HI_REG_WRITE
#define HI_REG_WRITE(addr,result)  (*(volatile unsigned int *)(addr) = (result))
#endif

/* Tsr2rcipher Rx Config */
hi_void tsc_hal_rx_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    /* enable rx interrupt */
    tsc_hal_rx_set_int(mgmt, id, HI_TRUE, HI_FALSE);

    /* rx ctrl */
    tsc_hal_rx_set_ctrl(mgmt, id, buf_type, 0);
}

/* Tsr2rcipher Rx DeConfig */
hi_void tsc_hal_rx_de_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    /* disable rx interrupt */
    tsc_hal_rx_set_int(mgmt, id, HI_FALSE, HI_FALSE);
}

/* Tsr2rcipher Rx Set Buf */
hi_void tsc_hal_rx_set_buf(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 src_buf_addr, hi_u32 src_buf_len)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    /* rx buffer address */
    tsc_hal_rx_set_buf_addr(mgmt, id, src_buf_addr);

    /* rx buffer len */
    tsc_hal_rx_set_buf_len(mgmt, id, src_buf_len);

    /* tsc_hal_rx_add_dsc(mgmt, id); */
}

/* Get DspTor status, HI_TURE: description queue not full, can configure new description */
hi_bool tsc_hal_rx_get_dsp_tor_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_RX_DSPTOR_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(id >= mgmt->ch_cnt, HI_FALSE);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, RX_DSPTOR_CTRL(id));
    return (reg.bits.rx_dsptor_full == 0x0);
}

/* Set the buffer start address, HI_TURE: description queue not full, can configure new description */
hi_void tsc_hal_rx_set_buf_addr(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 src_addr)
{
    U_RX_DSPTOR_START_ADDR reg_addr;
    U_TSC_RX_CTRL reg_ctrl;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg_addr.u32 = TSC_READ_REG_RX(mgmt->io_base, RX_DSPTOR_START_ADDR(id));
    reg_ctrl.u32 = TSC_READ_REG_RX(mgmt->io_base, TSC_RX_CTRL(id));

    /* risk here */
    reg_addr.bits.rx_dsptor_start_addr = src_addr & 0x00000000FFFFFFFF; /* 4K align, and 16 bytes align next version */
    reg_ctrl.bits.rx_session_id        = (src_addr & 0x0000000F00000000) >> 32;

    TSC_WRITE_REG_RX(mgmt->io_base, RX_DSPTOR_START_ADDR(id), reg_addr.u32);
    TSC_WRITE_REG_RX(mgmt->io_base, TSC_RX_CTRL(id), reg_ctrl.u32);
}

/* Set the buffer Len */
hi_void tsc_hal_rx_set_buf_len(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u32 src_len)
{
    U_RX_DSPTOR_LENGTH reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, RX_DSPTOR_LENGTH(id));
    reg.bits.rx_dsptor_length = src_len / TSR2RCIPHER_TS_PACKAGE_LEN; /* ts package number */
    reg.bits.rx_dsptor_cfg = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, RX_DSPTOR_LENGTH(id), reg.u32);
}

/* Add the Rx Dsc */
hi_void tsc_hal_rx_add_dsc(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_RX_DSPTOR_LENGTH reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, RX_DSPTOR_LENGTH(id));
    reg.bits.rx_dsptor_cfg = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, RX_DSPTOR_LENGTH(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_RX(mgmt->io_base, RX_DSPTOR_LENGTH(id)));
}

/* Set Rx Ctrl */
hi_void tsc_hal_rx_set_ctrl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type, hi_u32 rx_pkg_th)
{
    U_TSC_RX_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, TSC_RX_CTRL(id));
    reg.bits.rx_buf_type      = buf_type;
    reg.bits.rx_pkt_int_level = rx_pkg_th;

    TSC_WRITE_REG_RX(mgmt->io_base, TSC_RX_CTRL(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_RX(mgmt->io_base, TSC_RX_CTRL(id)));
}

/* Set Rx Ctrl */
hi_void tsc_hal_rx_set_int(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_bool dsc_rd, hi_bool rx_pkt)
{
    U_IENA_RX reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, IENA_RX(id));
    reg.bits.iena_rx_dsptor_done  = (HI_TRUE == dsc_rd ? 1 : 0);
    reg.bits.iena_rx_pkt_cnt      = (HI_TRUE == rx_pkt ? 1 : 0);

    TSC_WRITE_REG_RX(mgmt->io_base, IENA_RX(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_RX(mgmt->io_base, IENA_RX(id)));
}

/* Get Rx Dsc Rd Total Int status */
hi_u32 tsc_hal_rx_get_dsc_rd_total_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_RX_INT_TYPE reg;

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, RX_INT_TYPE);
    return reg.bits.rx_dsptor_done_int;
}

/* Get Rx Pkt cnt Total Int status */
hi_u32 tsc_hal_rx_get_pkt_cnt_total_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_RX_INT_TYPE reg;

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, RX_INT_TYPE);
    return reg.bits.rx_pkt_cnt_int;
}

/* Get the Dsc Rd interrupt status */
hi_u32 tsc_hal_rx_get_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg_index = 0;
    hi_u32 bit_offset = 0;
    hi_u32 reg = 0;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(id >= mgmt->ch_cnt, 0);

    reg_index  = id / INT_FLAG_REG_NUM;
    bit_offset = id % INT_FLAG_REG_NUM;

    reg = TSC_READ_REG_RX(mgmt->io_base, ISTA_RX_DSPTOR0(reg_index));

    return (reg & (1U << bit_offset));
}

/* Cls Rx Dsc Rd Int status */
hi_void tsc_hal_rx_cls_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_IRAW_RX reg;
    U_IRAW_RX reg_cls;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.iraw_rx_dsptor_done = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, IRAW_RX(id), reg.u32);
}

/* Get the Rx Pkt cnt interrupt status */
hi_u32 tsc_hal_rx_get_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg_index = 0;
    hi_u32 bit_offset = 0;
    hi_u32 reg = 0;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(id >= mgmt->ch_cnt, 0);

    reg_index  = id / INT_FLAG_REG_NUM;
    bit_offset = id % INT_FLAG_REG_NUM;

    reg = TSC_READ_REG_RX(mgmt->io_base, ISTA_RX_PKT_CNT0(reg_index));

    return (reg & (1U << bit_offset));
}

/* Cls Rx Pkt cnt Int status */
hi_void tsc_hal_rx_cls_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_IRAW_RX reg;
    U_IRAW_RX reg_cls;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.iraw_rx_pkt_cnt = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, IRAW_RX(id), reg.u32);
}

/* Get the Rx Total Dsc Rd interrupt Register value */
hi_void tsc_hal_rx_get_dsc_rd_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array)
{
    hi_s32 i;
    hi_s32 array_cnt = 0;
    array_cnt = (mgmt->ch_cnt - 1) / INT_FLAG_REG_NUM;

    for (i = 0; i < array_cnt; i++) {
        value_array[i] = TSC_READ_REG_RX(mgmt->io_base, ISTA_RX_DSPTOR0(i));
    }
}

/* Get the Total Pkt cnt interrupt regitster value */
hi_void tsc_hal_rx_get_pkt_cnt_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array)
{
    hi_s32 i;
    hi_s32 array_cnt = 0;
    array_cnt = (mgmt->ch_cnt - 1) / INT_FLAG_REG_NUM;

    for (i = 0; i < array_cnt; i++) {
        value_array[i] = TSC_READ_REG_RX(mgmt->io_base, ISTA_RX_PKT_CNT0(i));
    }
}

/* Set Rx Mode Ctrl */
hi_void tsc_hal_set_mode_ctl(struct tsr2rcipher_mgmt *mgmt, struct tsr2rcipher_r_ch *rch,
                             enum tsc_crypt_type crypt_type)
{
    U_TSC_MODE_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(rch->base.id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, TSC_MODE_CTRL(rch->base.id));
    reg.bits.key_id     = (rch->ks_handle & 0x000000ff);
    reg.bits.dsc_type   = crypt_type;
    reg.bits.core_sel   = rch->core_type;
    reg.bits.pl_raw_sel = rch->mode;

    if (rch->is_odd_key == HI_TRUE) {
        reg.bits.odd_even_sel = 1;
    } else {
        reg.bits.odd_even_sel = 0;
    }

    if (rch->is_crc_check == HI_TRUE) {
        reg.bits.tsc_crc_en = 1;
    } else {
        reg.bits.tsc_crc_en = 0;
    }

    TSC_WRITE_REG_RX(mgmt->io_base, TSC_MODE_CTRL(rch->base.id), reg.u32);

    reg.bits.tsc_rx_sec_attr = 1;
    reg.bits.tsc_tx_sec_attr = 1;

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_RX(mgmt->io_base, TSC_MODE_CTRL(rch->base.id)));
}

/* Set Rx Mode Ctrl */
hi_void tsc_hal_en_mode_ctl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_TSC_MODE_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, TSC_MODE_CTRL(id));
    reg.bits.tsc_chn_en = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, TSC_MODE_CTRL(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_RX(mgmt->io_base, TSC_MODE_CTRL(id)));
}

/* Set Rx Mode Ctrl */
hi_void tsc_hal_dis_mode_ctl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_TSC_MODE_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_RX(mgmt->io_base, TSC_MODE_CTRL(id));
    reg.bits.tsc_chn_en = 0;

    TSC_WRITE_REG_RX(mgmt->io_base, TSC_MODE_CTRL(id), reg.u32);
}

/* Tsr2rcipher Tx Config */
hi_void tsc_hal_tx_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    /* enable tx interrupt */
    tsc_hal_tx_set_int(mgmt, id, HI_TRUE, HI_FALSE);

    /* tx ctrl */
    tsc_hal_tx_set_ctrl(mgmt, id, buf_type, 0);
}

/* Tsr2rcipher Tx DeConfig */
hi_void tsc_hal_tx_de_config(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    /* disable rx interrupt */
    tsc_hal_tx_set_int(mgmt, id, HI_FALSE, HI_FALSE);
}

/* Tsr2rcipher Tx Set Buffer */
hi_void tsc_hal_tx_set_buf(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 dst_buf_addr, hi_u32 dst_buf_len)
{
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    /* tx buffer address */
    tsc_hal_tx_set_buf_addr(mgmt, id, dst_buf_addr);

    /* tx buffer len */
    tsc_hal_tx_set_buf_len(mgmt, id, dst_buf_len);

    /* tsc_hal_tx_add_dsc(mgmt, id); */
}

/* Get DspTor status, HI_TURE: description queue not full, can configure new description */
hi_bool tsc_hal_tx_get_dsp_tor_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_TX_DSPTOR_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(id >= mgmt->ch_cnt, HI_FALSE);

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, TX_DSPTOR_CTRL(id));
    return (reg.bits.tx_dsptor_full == 0x0);
}

/* Set the buffer start address */
hi_void tsc_hal_tx_set_buf_addr(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u64 dst_addr)
{
    U_TX_DSPTOR_START_ADDR reg_addr;
    U_TSC_TX_CTRL          reg_ctrl;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg_addr.u32 = TSC_READ_REG_TX(mgmt->io_base, TX_DSPTOR_START_ADDR(id));
    reg_ctrl.u32 = TSC_READ_REG_TX(mgmt->io_base, TSC_TX_CTRL(id));

    /* risk here */
    reg_addr.bits.tx_dsptor_start_addr = dst_addr & 0x00000000FFFFFFFF; /* 4K align, and 16 bytes align next version */
    reg_ctrl.bits.tx_session_id        = (dst_addr & 0x0000000F00000000) >> 32;

    TSC_WRITE_REG_TX(mgmt->io_base, TX_DSPTOR_START_ADDR(id), reg_addr.u32);
    TSC_WRITE_REG_TX(mgmt->io_base, TSC_TX_CTRL(id), reg_ctrl.u32);
}

/* Set the buffer Len */
hi_void tsc_hal_tx_set_buf_len(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_u32 dst_len)
{
    U_TX_DSPTOR_LENGTH reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, TX_DSPTOR_LENGTH(id));
    reg.bits.tx_dsptor_length = dst_len / TSR2RCIPHER_TS_PACKAGE_LEN; /* ts package number */
    reg.bits.tx_dsptor_cfg = 1;

    TSC_WRITE_REG_TX(mgmt->io_base, TX_DSPTOR_LENGTH(id), reg.u32);
}

/* Add the Rx Dsc */
hi_void tsc_hal_tx_add_dsc(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_TX_DSPTOR_LENGTH reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, TX_DSPTOR_LENGTH(id));
    reg.bits.tx_dsptor_cfg = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, TX_DSPTOR_LENGTH(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_TX(mgmt->io_base, TX_DSPTOR_LENGTH(id)));
}

/* Set Tx Ctrl */
hi_void tsc_hal_tx_set_ctrl(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, enum tsc_buf_type buf_type, hi_u32 tx_pkg_th)
{
    U_TSC_TX_CTRL reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, TSC_TX_CTRL(id));
    reg.bits.tx_buf_type      = buf_type;
    reg.bits.tx_pkt_int_level = tx_pkg_th;
    reg.bits.tx_press_dis     = 0x1;  /* disable press */

    TSC_WRITE_REG_TX(mgmt->io_base, TSC_TX_CTRL(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_TX(mgmt->io_base, TSC_TX_CTRL(id)));
}

/* Set Tx interrupt */
hi_void tsc_hal_tx_set_int(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_bool dsc_rd, hi_bool tx_pkt)
{
    U_IENA_TX reg;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, IENA_TX(id));
    reg.bits.iena_tx_dsptor_done  = (dsc_rd == HI_TRUE ? 1 : 0);
    reg.bits.iena_tx_pkt_cnt      = (tx_pkt == HI_TRUE ? 1 : 0);

    TSC_WRITE_REG_TX(mgmt->io_base, IENA_TX(id), reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_TX(mgmt->io_base, IENA_TX(id)));
}

/* Get Tx Dsc Rd Total Int status */
hi_u32 tsc_hal_tx_get_dsc_rd_total_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TX_INT_TYPE reg;

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, TX_INT_TYPE);
    return reg.bits.tx_dsptor_done_int;
}

/* Get Tx Pkt cnt Total Int status */
hi_u32 tsc_hal_tx_get_pkt_cnt_total_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TX_INT_TYPE reg;

    reg.u32 = TSC_READ_REG_TX(mgmt->io_base, TX_INT_TYPE);
    return reg.bits.tx_pkt_cnt_int;
}

/* Get the Tx Dsc Rd interrupt status */
hi_u32 tsc_hal_tx_get_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg_index = 0;
    hi_u32 bit_offset = 0;
    hi_u32 reg = 0;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(id >= mgmt->ch_cnt, 0);

    reg_index  = id / INT_FLAG_REG_NUM;
    bit_offset = id % INT_FLAG_REG_NUM;

    reg = TSC_READ_REG_TX(mgmt->io_base, ISTA_TX_DSPTOR0(reg_index));

    return (reg & (1U << bit_offset));
}

/* Cls Tx Dsc Rd Int status */
hi_void tsc_hal_tx_cls_dsc_rd_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_IRAW_TX reg;
    U_IRAW_TX reg_cls;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.iraw_tx_dsptor_done = 1;

    TSC_WRITE_REG_TX(mgmt->io_base, IRAW_TX(id), reg.u32);
}

/* Get the Tx Pkt cnt interrupt status */
hi_u32 tsc_hal_tx_get_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg_index = 0;
    hi_u32 bit_offset = 0;
    hi_u32 reg = 0;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(id >= mgmt->ch_cnt, 0);

    reg_index  = id / INT_FLAG_REG_NUM;
    bit_offset = id % INT_FLAG_REG_NUM;

    reg = TSC_READ_REG_TX(mgmt->io_base, ISTA_TX_PKT_CNT0(reg_index));

    return (reg & (1U << bit_offset));
}

/* Cls Tx Pkt Cnt Int status */
hi_void tsc_hal_tx_cls_pkt_cnt_int_status(struct tsr2rcipher_mgmt *mgmt, hi_u32 id)
{
    U_IRAW_TX reg;
    U_IRAW_TX reg_cls;
    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.iraw_tx_pkt_cnt = 1;

    TSC_WRITE_REG_RX(mgmt->io_base, IRAW_TX(id), reg.u32);

    TSC_COM_EQUAL(reg_cls.u32, TSC_READ_REG_TX(mgmt->io_base, IRAW_TX(id)));
}

/* Get the Total Dsc Rd interrupt Register value */
hi_void tsc_hal_tx_get_dsc_rd_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array)
{
    hi_s32 i;
    hi_s32 array_cnt = 0;
    array_cnt = (mgmt->ch_cnt - 1) / INT_FLAG_REG_NUM;

    for (i = 0; i < array_cnt; i++) {
        value_array[i] = TSC_READ_REG_TX(mgmt->io_base, ISTA_TX_DSPTOR0(i));
    }
}

/* Get the Total Pkt interrupt regitster value */
hi_void tsc_hal_tx_get_pkt_cnt_total_int_value(struct tsr2rcipher_mgmt *mgmt, hi_u32 *value_array)
{
    hi_s32 i;
    hi_s32 array_cnt = 0;
    array_cnt = (mgmt->ch_cnt - 1) / INT_FLAG_REG_NUM;

    for (i = 0; i < array_cnt; i++) {
        value_array[i] = TSC_READ_REG_TX(mgmt->io_base, ISTA_TX_PKT_CNT0(i));
    }
}

/* Set Top interrupt */
hi_void tsc_hal_top_set_int(struct tsr2rcipher_mgmt *mgmt, hi_bool rx_int, hi_bool tx_int, hi_bool cipher_int)
{
    U_TSC_IENA reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_IENA);
    reg.bits.rx2cpu_iena      = (rx_int == HI_TRUE ? 1 : 0);
    reg.bits.tx2cpu_iena      = (tx_int == HI_TRUE ? 1 : 0);
    reg.bits.cipher2cpu_iena  = (cipher_int == HI_TRUE ? 1 : 0);

    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_IENA, reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_TOP(mgmt->io_base, TSC_IENA));
}

/* Get Top Rx interrupt status */
hi_u32 tsc_hal_top_get_rx_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_ISTA reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_ISTA);

    return reg.bits.rx2cpu_ista;
}

/* Get Top Rx Raw interrupt status */
hi_u32 tsc_hal_top_get_rx_raw_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_IRAW reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_IRAW);

    return reg.bits.rx2cpu_iraw;
}

/* Cls Top Rx interrupt status */
hi_void tsc_hal_top_cls_rx_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_IRAW  reg;
    U_TSC_IRAW  reg_cls;

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.rx2cpu_iraw = 1;

    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_IRAW, reg.u32);
}

/* Get Top Tx interrupt status */
hi_u32 tsc_hal_top_get_tx_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_ISTA reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_ISTA);

    return reg.bits.tx2cpu_ista;
}

/* Get Top Tx Raw interrupt status */
hi_u32 tsc_hal_top_get_tx_raw_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_IRAW reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_IRAW);

    return reg.bits.tx2cpu_iraw;
}

/* Cls Top Tx interrupt status */
hi_void tsc_hal_top_cls_tx_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_IRAW  reg;
    U_TSC_IRAW  reg_cls;

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.tx2cpu_iraw = 1;

    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_IRAW, reg.u32);
}

/* Get Top Cipher interrupt status */
hi_u32 tsc_hal_top_get_cipher_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_ISTA reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_ISTA);

    return reg.bits.cipher2cpu_ista;
}

/* Get Top Cipher interrupt status */
hi_u32 tsc_hal_top_get_cipher_raw_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_IRAW reg;

    reg.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_IRAW);

    return reg.bits.cipher2cpu_iraw;
}

/* Cls Top Rx interrupt status */
hi_void tsc_hal_top_cls_cipher_int_status(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_IRAW  reg;
    U_TSC_IRAW  reg_cls;

    reg.u32 = 0;
    reg_cls.u32 = 0;
    reg.bits.cipher2cpu_iraw = 1;

    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_IRAW, reg.u32);
}

hi_void tsc_hal_init_hw(struct tsr2rcipher_mgmt *mgmt)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    if (reg_crg == HI_NULL) {
        HI_ERR_TSR2RCIPHER("reg_crg is NULL!\n");
        return;
    }

    reg_crg->PERI_CRG185.bits.tscipher_srst_req = 1;
    mb();

    reg_crg->PERI_CRG185.bits.tscipher_cken = 1;
    reg_crg->PERI_CRG185.bits.tscipher_srst_req = 0;

    mb();

    return;
}

hi_void tsc_hal_de_init_hw(struct tsr2rcipher_mgmt *mgmt)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    if (reg_crg == HI_NULL) {
        HI_ERR_TSR2RCIPHER("reg_crg is NULL!\n");
        return;
    }

    reg_crg->PERI_CRG185.bits.tscipher_srst_req = 1;
    mb();

    reg_crg->PERI_CRG185.bits.tscipher_cken = 0;
    reg_crg->PERI_CRG185.bits.tscipher_srst_req = 1;

    mb();

    return;
}

#ifdef HI_SMMU_SUPPORT
hi_void tsc_hal_en_mmu(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_MMU_NOSEC_EN mmu_nosec_en;
    hi_ulong cb_ttbr = 0;
    hi_ulong err_rd_addr = 0;
    hi_ulong err_wr_addr = 0;
    hi_u32 i;

    osal_mem_get_nssmmu_pgtinfo(&cb_ttbr, &err_rd_addr, &err_wr_addr);
    mgmt->cb_ttbr = cb_ttbr;

    /* hw restrict cb_ttbr must be 16bytes align. */
    WARN_ON(cb_ttbr % 16);

    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_TLB, cb_ttbr >> 4); /* must 16 byte align */

    mmu_nosec_en.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_EN);
    mmu_nosec_en.bits.sw_nosec_mmu_en = 1;
    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_EN, mmu_nosec_en.u32);

    TSC_COM_EQUAL(mmu_nosec_en.u32, TSC_READ_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_EN));

    for (i = 0; i < 8; i++) {
        TSC_WRITE_REG_TOP(mgmt->io_base, TSC_MMU_RX_CLR(i), 0xFFFFFFFF);
    }

    for (i = 0; i < 8; i++) {
        TSC_WRITE_REG_TOP(mgmt->io_base, TSC_MMU_TX_CLR(i), 0xFFFFFFFF);
    }
}

hi_void tsc_hal_dis_mmu(struct tsr2rcipher_mgmt *mgmt)
{
    U_TSC_MMU_NOSEC_EN mmu_nosec_en;

    mmu_nosec_en.u32 = TSC_READ_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_EN);
    mmu_nosec_en.bits.sw_nosec_mmu_en = 0;

    TSC_WRITE_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_EN, mmu_nosec_en.u32);

    TSC_COM_EQUAL(mmu_nosec_en.u32, TSC_READ_REG_TOP(mgmt->io_base, TSC_MMU_NOSEC_EN));
}
#endif

/* Set the Mdsc Enable register */
hi_void tsc_hal_mdscset_en(struct tsr2rcipher_mgmt *mgmt, hi_u32 id, hi_bool ca_en, hi_bool ca_chan_scr_en)
{
    U_MDSC_EN reg;

    TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(id >= mgmt->ch_cnt);

    reg.u32 = TSC_READ_REG_MDSC(mgmt->io_base, MDSC_EN);

    reg.bits.ca_en                  = ca_en  ? 1 : 0;
    reg.bits.ts_ctrl_dsc_change_en  = ca_chan_scr_en  ? 1 : 0;

    TSC_WRITE_REG_MDSC(mgmt->io_base, MDSC_EN, reg.u32);

    TSC_COM_EQUAL(reg.u32, TSC_READ_REG_MDSC(mgmt->io_base, MDSC_EN));
}


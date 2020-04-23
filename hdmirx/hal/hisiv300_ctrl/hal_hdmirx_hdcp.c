/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Implementation of hdcp functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-3
 */
#include "hal_hdmirx_hdcp.h"
#include <linux/delay.h>
#include "hal_hdmirx_hdcp_reg.h"
#include "hal_hdmirx_hdcp2x_cpu_reg.h"
#include "hal_hdmirx_depack_reg.h"
#include "hal_hdmirx_pwd_reg.h"
#include "hal_hdmirx_mcu_cpu_reg.h"
#include "hal_hdmirx_hdcp2x_det_reg.h"

hi_void hal_hdcp_set_crc_en(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type, hi_bool en)
{
    if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_22) {
        hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_KEY_BIST_CTRL, CPU_KEY_RAM_CRC_EN, en);
    } else {
        hdmirx_hal_hdcp1x_reg_write_fld_align(port, REG_CFG_BIST_EN, CFG_BIST_EN, en);
    }
}

hi_bool hal_hdcp_get_crc_result(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type)
{
    hi_u32 result = HI_FALSE;

    if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_22) {
        result = hdmirx_hal_hdcp2x_reg_read_fld_align(port, REG_HDCP2X_KEY_BIST_STA, KEY_RAM_CRC_PASS);
    } else {
        result = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_BIST_STATE, HDCP_CFG_BIST_DONE);
    }

    if (result) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_bool hal_hdcp_get_done(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type)
{
    hi_u32 auth_done, decrypt_on;
    hi_bool hdcp_done;

    if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_22) {
        auth_done = hdmirx_hal_hdcp2x_reg_read_fld_align(port, REG_HDCP2X_GEN_OUT2, HDCP2X_GEN_OUT2);
        auth_done = auth_done & BIT4;
        decrypt_on = hdmirx_hal_reg_read_fld_align(port, REG_HDCP_DET_HW, HDCP2X_DECRYPT_ST);
    } else {
        auth_done = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_AUTH_STATE, CFG_AUTH_DONE);
        decrypt_on = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_AUTH_STATE, CFG_DECRYPTING_ON);
    }

    if ((auth_done == 0) || (decrypt_on == 0)) {
            hdcp_done = HI_FALSE;
        } else {
            hdcp_done = HI_TRUE;
    }

    return hdcp_done;
}

hdmirx_hdcp_err_cnt hal_hdcp_get_bch_err_cnt(hi_drv_hdmirx_port port)
{
    hdmirx_hdcp_err_cnt hdcp_err;

    hdcp_err.bch_err_cnt = hdmirx_hal_reg_read_fld_align(port, REG_RX_BCH_ERR, BCH_PACKET_ERR_COUNT);
    hdcp_err.pkt_cnt = hdmirx_hal_reg_read_fld_align(port, REG_RX_PKT_CNT, PACKET_RECEIVED_COUNT);
    hdcp_err.t4_err_cnt = hdmirx_hal_reg_read_fld_align(port, REG_RX_T4_ERR, T4_PACKET_ERR_COUNT);

    return hdcp_err;
}

hi_drv_hdmirx_hdcp_type hal_hdcp_check_type(hi_drv_hdmirx_port port)
{
    hi_u32 type;

    type = hdmirx_hal_reg_read_fld_align(port, REG_RX_PWD_STATUS, PWD_STAT_HDCP_DET_STAT);

    return type;
}

hi_u32 hal_hdcp_get_bksv(hi_drv_hdmirx_port port, hi_drv_hdmirx_hdcp_type hdcp_type, hi_u32 *bksv, hi_u32 max_len)
{
    hi_u32 num = 0;
    hi_u32 len = 5; /* 5: aksv length */
    hi_u32 mask;
    hi_u32 bits = 8; /* 8: move right */

    if (len > max_len) {
        len = max_len;
    }
    if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_14) {
        for (num = 0; num < len; num++) {
            if (num <= 2) { /* 2: LSB */
                mask = BIT7_0 << (num * bits);
                *bksv = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_BKSV_0, mask);
            } else {
                mask = BIT7_0 << ((num - 3) * bits); /* 3: MSB */
                *bksv = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_BKSV_1, mask);
            }
            bksv++;
        }
    } else if (hdcp_type == HI_DRV_HDMIRX_HDCPTYPE_22) {
        for (num = 0; num < len; num++) {
            if (num <= 3) { /* 3: MSB */
                mask = BIT7_0 << (num * bits);
                *bksv = hdmirx_hal_hdcp2x_reg_read_fld_align(port, REG_HDCP2X_RCV_ID_LSB, mask);
            } else {
                mask = BIT7_0 << ((num - 4) * bits); /* 4: MSB */
                *bksv = hdmirx_hal_hdcp2x_reg_read_fld_align(port, REG_HDCP2X_RCV_ID_MSB, mask);
            }
            bksv++;
        }
    }

    return num;
}

hi_u32 hal_hdcp_get_aksv(hi_drv_hdmirx_port port, hi_u32 *aksv, hi_u32 max_len)
{
    hi_u32 num;
    hi_u32 len = 5; /* 5: aksv length */
    hi_u32 mask;
    hi_u32 bits = 8; /* 8: move right */

    if (len > max_len) {
        len = max_len;
    }

    for (num = 0; num < len; num++) {
        if (num <= 2) { /* 2: LSB */
            mask = BIT7_0 << (num * bits);
            *aksv = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_AKSV_0, mask);
        } else {
            mask = BIT7_0 << ((num - 3) * bits); /* 3: MSB */
            *aksv = hdmirx_hal_hdcp1x_reg_read_fld_align(port, REG_CFG_AKSV_1, mask);
        }
        aksv++;
    }

    return num;
}

hi_u32 hal_hdcp_get_enc_en_cnt(hi_drv_hdmirx_port port)
{
    hi_u32 encry_cnt;

    encry_cnt = hdmirx_hal_reg_read_fld_align(port, REG_CFG_ENCRY_FRM_CNT, CFG_ENCRY_FRM_CNT);

    return encry_cnt;
}

hi_s32 hal_hdcp2x_load_mcu_pram(hi_drv_hdmirx_port port, hi_u32 *pram, hi_u32 max_len)
{
    hi_u32 len = 0x10000; /* 0x10000:pram total length */
    hi_u32 num, addr;

    if (len >= max_len) {
        len = max_len;
    }

    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, CFG_HDCP2X_MCU_SRST_REQ, HI_TRUE);
    for (num = 0; num < len; num++) {
        addr = num * 4; /* 4 : 1 reg = 4 byte */
        hdmirx_hal_hdcp_ram_reg_write(port, addr, pram[num]);
    }
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, CFG_HDCP2X_MCU_SRST_REQ, HI_FALSE);

    return HI_SUCCESS;
}

hi_void hal_hdcp2x_start_mcu(hi_drv_hdmirx_port port, hi_bool start)
{
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, CFG_HDCP2X_MCU_SRST_REQ, HI_TRUE);
    udelay(1); /* delay 1us */
    hdmirx_hal_reg_write_fld_align(port, REG_RX_PWD_SRST, CFG_HDCP2X_MCU_SRST_REQ, HI_FALSE);
}

hi_void hal_hdcp2x_set_reauth(hi_drv_hdmirx_port port)
{
    hdmirx_hal_hdcp2x_reg_write_fld_align(port, REG_HDCP2X_REAUTH_CTRL, CPU_REAUTH_REQ_SET, HI_TRUE);
}

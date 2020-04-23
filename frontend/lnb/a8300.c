/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: A8300 LNB driver
 * Author: SDK
 * Create: 2017/10/23
 */
#include <linux/delay.h>
#include "drv_lnbctrl.h"

#include "hi_drv_i2c.h"

#define A8300_ADDR 0x10
#define A8300_CTRL_REG0_ADDR 0
#define A8300_CTRL_REG1_ADDR 1

#define A8300_STATUS_REG0_ADDR 0

#define A8300_I2C_WRITE_TIMES 1


static hi_bool g_a8300_inited = HI_FALSE;
static hi_u8 g_reg1_data = 0;
static hi_u8 g_reg2_data = 0;
static hi_u32 g_a8300_port = 0;

static hi_s32 a8300_read_byte(lnbctrl_dev_param* param, hi_u8 reg_addr, hi_u8 *reg_val)
{
    hi_s32 ret;

    if (reg_val == HI_NULL) {
        LOG_ERR_PRINT("pointer is null\n");
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    ret = hi_drv_i2c_read_2stop(param->i2c_num, param->dev_addr, reg_addr, 1, reg_val, 1);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("A8300 read i2c 0x%02x failed.\n", *reg_val);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 a8300_write_byte(lnbctrl_dev_param* param, hi_u8 reg_addr, hi_u8 reg_val)
{
    hi_s32 ret;

    ret = hi_drv_i2c_write(param->i2c_num, param->dev_addr, reg_addr, 1, &reg_val, 1);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("A8300 write i2c failed, %x\n", ret);
        return ret;
    }

    LOG_INFO_PRINT("A8300 write i2c 0x%02x ok.\n", reg_val);
    return HI_SUCCESS;
}

hi_s32 a8300_init(hi_u32 port, hi_u8 i2c_channel, hi_u8 dev_addr)
{
    lnbctrl_dev_param* param = HI_NULL;
    hi_s32 ret = HI_FAILURE;
    hi_u8 reg_val = 0x80;

    if (g_a8300_inited) {
        return HI_SUCCESS;
    }

    g_reg1_data = 0;
    g_reg2_data = 0;

    /* 添加一个node,所有port共用一个 pstParam */
    g_a8300_port = port;
    param = lnbctrl_queue_get(g_a8300_port);
    if (param == HI_NULL) {
        param = lnbctrl_queue_insert(g_a8300_port, i2c_channel, dev_addr);
        if (param == HI_NULL) {
            LOG_ERR_PRINT("lnbctrl_queue_insert failed.\n");
            return HI_FAILURE;
        }
    }

    if (!param->inited) {
        /* 初始化,寄存器写0。如果不先读一下寄存器的话，首次写入会失效，原因待查。 */
        ret = a8300_read_byte(param, A8300_STATUS_REG0_ADDR, &reg_val);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("a8300_read_byte 0x%x failed.\n", reg_val);
            return ret;
        }
        ret = a8300_write_byte(param, A8300_STATUS_REG0_ADDR, 0);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("a8300_write_byte failed.\n");
            return ret;
        }

        reg_val = g_reg1_data;
        reg_val |= 0x20;      /* 22k波形由外部产生 */
        ret = a8300_write_byte(param, A8300_CTRL_REG0_ADDR, reg_val);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("a8300_write_byte 0x%x failed.\n", reg_val);
            return ret;
        }
        g_reg1_data = reg_val;

        param->inited = HI_TRUE;
    }

    g_a8300_inited = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 a8300_standby(hi_u32 tuner_port, hi_u32 standby)
{
    return HI_SUCCESS;
}

hi_s32 a8300_lnbctrl_power(hi_u32 tuner_port, hi_u8 power)
{
    hi_s32 ret;
    hi_u8 reg_val;
    lnbctrl_dev_param* param;

    /* Find node */
    param = lnbctrl_queue_get(g_a8300_port);
    if (param == HI_NULL) {
        LOG_ERR_PRINT("lnbctrl_queue_get failed.\n");
        return HI_FAILURE;
    }

    reg_val = g_reg1_data;
    if (power == 0) {
        reg_val &= 0xe0;
    } else {
        reg_val |= 0x10;
    }

    ret = a8300_write_byte(param, A8300_CTRL_REG0_ADDR, reg_val);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("a8300_write_byte failed.\n");
        return ret;
    }

    g_reg1_data = reg_val;

    return HI_SUCCESS;
}

hi_s32 a8300_set_lnb_out(hi_u32 tuner_port, frontend_lnb_out_level out)
{
    hi_s32 ret;
    lnbctrl_dev_param* param;
    hi_u8 reg;

    param = lnbctrl_queue_get(g_a8300_port);
    if (param == HI_NULL) {
        LOG_ERR_PRINT("lnbctrl_queue_get failed.\n");
        return HI_FAILURE;
    }

    if (!param->inited) {
        LOG_ERR_PRINT("pstParam uninited.\n");
        return HI_FAILURE;
    }

    reg = g_reg1_data;
    switch (out) {
        case FRONTEND_LNB_OUT_0V:
        case FRONTEND_LNB_OUT_BUTT:            reg &= 0xe0;
            break;
        case FRONTEND_LNB_OUT_13V:            reg &= 0xe0;
            reg |= 0x12;
            break;
        case FRONTEND_LNB_OUT_14V:            reg &= 0xe0;
            reg |= 0x13;
            break;
        case FRONTEND_LNB_OUT_18V:            reg &= 0xe0;
            reg |= 0x1b;
            break;
        case FRONTEND_LNB_OUT_19V:            reg &= 0xe0;
            reg |= 0x1c;
            break;
        default:            LOG_ERR_PRINT("Unkown enout: %d.\n", out);
            return HI_FAILURE;
    }

    ret = a8300_write_byte(param, A8300_CTRL_REG0_ADDR, reg);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("a8300_write_byte 0x%x failed.\n", reg);
        return ret;
    }

    g_reg1_data = reg;

    return HI_SUCCESS;
}

hi_s32 a8300_deinit(hi_u32 tuner_port)
{
    lnbctrl_dev_param* param;
    hi_s32 ret;

    /* Find node */
    param = lnbctrl_queue_get(g_a8300_port);
    if (param == HI_NULL) {
        LOG_INFO_PRINT("lnbctrl_queue_get failed.\n");
        return HI_SUCCESS;
    }

    /* A8300 power off */
    ret = a8300_lnbctrl_power(g_a8300_port, 0);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("a8300_lnbctrl_power0 failed.\n");
        return HI_FAILURE;
    }

    lnbctrl_queue_remove(g_a8300_port);
    g_a8300_inited = HI_FALSE;
    g_reg1_data = 0;
    g_reg2_data = 0;
    g_a8300_port = 0;

    return HI_SUCCESS;
}

hi_s32 drv_fe_adp_a8300_regist_func(drv_fe_lnb_ops *lnb_ops)
{
    drv_fe_check_pointer(lnb_ops);

    lnb_ops->init = a8300_init;
    lnb_ops->deinit = a8300_deinit;
    lnb_ops->standby = a8300_standby;
    lnb_ops->set_lnb_out = a8300_set_lnb_out;
    return HI_SUCCESS;
}


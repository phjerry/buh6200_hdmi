/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: LNBH30 driver
 * Author: SDK
 * Created: 2019-10-11
 */
#include "lnbh30.h"
#include "drv_lnbctrl.h"

static hi_s32 lnbh30_init(hi_u32 port, hi_u8 i2c_channel, hi_u8 dev_addr)
{
    lnbctrl_dev_param *param = NULL;
    hi_s32 ret;

    LOG_DBG_PRINT("port %d, I2C %d, addr 0x%x.\n", port, i2c_channel, dev_addr);

    param = lnbctrl_queue_get(port);
    if (param == NULL) {
        param = lnbctrl_queue_insert(port, i2c_channel, dev_addr);
        if (param == NULL) {
            return HI_ERR_FRONTEND_FAILED_ALLOC_MEM;
        }
    }

    if (!param->inited) {
        /* 12V */
        ret = lnb_write_byte(port, 0x01, 0x02);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("LNBH30 %d init failed\n", port);
            return ret;
        }

        param->lnb_out = FRONTEND_LNB_OUT_13V;
        param->inited = HI_TRUE;
    }

    return HI_SUCCESS;
}

static hi_s32 lnbh30_deinit(hi_u32 port)
{
    lnbctrl_dev_param *param = NULL;

    LOG_DBG_PRINT("LNBH30 %d deinit\n", port);

    param = lnbctrl_queue_get(port);
    if (param == NULL) {
        return HI_SUCCESS;
    }

    lnb_write_byte(port, 0x01, 0);

    if (param->inited) {
        param->inited = HI_FALSE;
    }
    lnbctrl_queue_remove(port);

    return HI_SUCCESS;
}

static hi_u8 lnbh30_get_vsel(frontend_lnb_out_level out)
{
    hi_u8 vsel = 0;
    switch (out) {
        case FRONTEND_LNB_OUT_13V:
        case FRONTEND_LNB_OUT_14V:
            vsel = 0x2;
            break;
        case FRONTEND_LNB_OUT_18V:
        case FRONTEND_LNB_OUT_19V:
            vsel = 0x5;
            break;
        case FRONTEND_LNB_OUT_0V:
        default:
            vsel = 0x0;
            break;
    }
    return vsel;
}

static hi_s32 lnbh30_set_lnb_out(hi_u32 port, frontend_lnb_out_level out)
{
    lnbctrl_dev_param *param = NULL;
    hi_s32 ret;
    hi_u8 reg_value = 0;
    hi_u8 vsel;

    HI_DBG_PRINT_U32(out);

    param = lnbctrl_queue_get(port);
    if (param == HI_NULL) {
        return HI_FAILURE;
    }

    if (!param->inited) {
        LOG_ERR_PRINT("lnbh30 %d not init.\n", port);
        return HI_FAILURE;
    }
    vsel = lnbh30_get_vsel(out);
    ret = lnb_read_byte(port, 0x01, &reg_value);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(lnb_read_byte, ret);
        return ret;
    }
    reg_value &= 0xf8;
    reg_value |= vsel;
    ret = lnb_write_byte(port, 0x01, reg_value);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(lnb_write_byte, ret);
        return ret;
    }

    param->lnb_out = out;
    return HI_SUCCESS;
}

static hi_s32 lnbh30_standby(hi_u32 port, hi_u32 standby)
{
    /* If standby, power off */
    if (standby == 1) {
        return lnbh30_set_lnb_out(port, FRONTEND_LNB_OUT_0V);
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_adp_lnbh30_regist_func(drv_fe_lnb_ops *lnb_ops)
{
    drv_fe_check_pointer(lnb_ops);

    lnb_ops->init = lnbh30_init;
    lnb_ops->deinit = lnbh30_deinit;
    lnb_ops->standby = lnbh30_standby;
    lnb_ops->set_lnb_out = lnbh30_set_lnb_out;
    return HI_SUCCESS;
}


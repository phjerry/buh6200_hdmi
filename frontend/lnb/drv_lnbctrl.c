/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: LNB base function
 * Author: SDK
 * Created: 2017-06-30
 */

#include "drv_frontend.h"
#include "drv_fe_i2c.h"
#include "drv_lnbctrl.h"
#include "hi_drv_mem.h"

static lnbctrl_dev_param* g_lnb_ctrl_dev_head = HI_NULL;

/* Malloc and insert a node */
lnbctrl_dev_param* lnbctrl_queue_insert(hi_u32 tuner_port, hi_u32 i2c_num, hi_u8 dev_addr)
{
    lnbctrl_dev_param* new_code = HI_NULL;
    lnbctrl_dev_param* tail = g_lnb_ctrl_dev_head;

    new_code = HI_VMALLOC(HI_ID_TUNER, sizeof(lnbctrl_dev_param));
    if (new_code == HI_NULL) {
        LOG_ERR_PRINT("LNB ctrl dev vmalloc fail.\n");
        return HI_NULL;
    }

    new_code->tuner_port = tuner_port;
    new_code->i2c_num = i2c_num;
    new_code->dev_addr = dev_addr;
    new_code->inited  = HI_FALSE;
    new_code->lnb_out = FRONTEND_LNB_OUT_0V;
    new_code->next = HI_NULL;

    if (tail == HI_NULL) {
        g_lnb_ctrl_dev_head = new_code;
    } else {
        while (tail->next != HI_NULL) {
            tail = tail->next;
        }

        tail->next = new_code;
    }

    return new_code;
}

/* Find and return a node */
lnbctrl_dev_param* lnbctrl_queue_get(hi_u32 tuner_port)
{
    lnbctrl_dev_param* temp = g_lnb_ctrl_dev_head;

    while (temp != HI_NULL) {
        if (tuner_port == temp->tuner_port) {
            break;
        }

        temp = temp->next;
    }

    return temp;
}

/* Remove a node */
hi_void lnbctrl_queue_remove(hi_u32 tuner_port)
{
    lnbctrl_dev_param* temp = g_lnb_ctrl_dev_head;
    lnbctrl_dev_param* last = g_lnb_ctrl_dev_head;

    while (temp != HI_NULL) {
        if (tuner_port == temp->tuner_port) {
            if (temp == g_lnb_ctrl_dev_head) {
                g_lnb_ctrl_dev_head = temp->next;
            }

            last->next = temp->next;
            HI_VFREE(HI_ID_TUNER, temp);
            break;
        }

        last = temp;
        temp = temp->next;
    }
}

hi_s32 lnb_read_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 *reg_val)
{
    lnbctrl_dev_param* param = lnbctrl_queue_get(port_id);
    drv_fe_check_pointer(param);
    return drv_fe_i2c_read(param->i2c_num, param->dev_addr, reg_addr, 1, reg_val, 1);
}

hi_s32 lnb_write_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 reg_val)
{
    lnbctrl_dev_param* param = lnbctrl_queue_get(port_id);
    drv_fe_check_pointer(param);
    return drv_fe_i2c_write(param->i2c_num, param->dev_addr, reg_addr, 1, &reg_val, 1);
}

static hi_s32 lnb_none_init(hi_u32 fe_port, hi_u8 i2c_channel, hi_u8 dev_addr)
{
    return HI_SUCCESS;
}

static hi_s32 lnb_none_deinit(hi_u32 fe_port)
{
    return HI_SUCCESS;
}

static hi_s32 lnb_none_standby(hi_u32 fe_port, hi_u32 standby)
{
    return HI_SUCCESS;
}

static hi_s32 lnb_none_set_lnb_out(hi_u32 fe_port, frontend_lnb_out_level lnb_out_level)
{
    return HI_SUCCESS;
}

hi_s32 drv_fe_adp_lnb_none_regist_func(drv_fe_lnb_ops *lnb_ops)
{
    drv_fe_check_pointer(lnb_ops);

    lnb_ops->init = lnb_none_init;
    lnb_ops->deinit = lnb_none_deinit;
    lnb_ops->standby = lnb_none_standby;
    lnb_ops->set_lnb_out = lnb_none_set_lnb_out;
    return HI_SUCCESS;
}


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend i2c driver
 * Author: SDK
 * Created: 2017-06-30
 */
#include "drv_frontend_ctrl.h"
#include "hi_drv_i2c.h"


hi_s32 drv_fe_i2c_read(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                       hi_u32 buf_len)
{
    drv_fe_ctrl_func *ctrl_fun = drv_frontend_get_ctrl_func();
    if (channel < HI_STD_I2C_NUM) {
        return ctrl_fun->i2c_func->pfn_i2c_read(channel, dev_addr, reg_addr, reg_addr_offset, buf, buf_len);
    } else {
        return ctrl_fun->gpio_i2c_func->pfn_gpio_i2c_read_ext(channel, dev_addr, reg_addr, reg_addr_offset, buf, buf_len);
    }
}

hi_s32 drv_fe_i2c_write(hi_u32 channel, hi_u8 dev_addr, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf,
                        hi_u32 buf_len)
{
    drv_fe_ctrl_func *ctrl_fun = drv_frontend_get_ctrl_func();
    if (channel < HI_STD_I2C_NUM) {
        return ctrl_fun->i2c_func->pfn_i2c_write(channel, dev_addr, reg_addr, reg_addr_offset, buf, buf_len);
    } else {
        return ctrl_fun->gpio_i2c_func->pfn_gpio_i2c_write_ext(channel, dev_addr, reg_addr, reg_addr_offset, buf, buf_len);
    }
}

hi_s32 demod_i2c_read(hi_u32 port_id, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_check_pointer(frontend_context);
    return drv_fe_i2c_read(frontend_context->attr.demod_i2c_channel,
                           frontend_context->attr.demod_addr,
                           reg_addr,
                           reg_addr_offset,
                           buf,
                           buf_len);
}

hi_s32 demod_i2c_write(hi_u32 port_id, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_check_pointer(frontend_context);
    return drv_fe_i2c_write(frontend_context->attr.demod_i2c_channel,
                            frontend_context->attr.demod_addr,
                            reg_addr,
                            reg_addr_offset,
                            buf,
                            buf_len);
}

hi_s32 demod_read_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 *reg_val)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_check_pointer(frontend_context);
    return drv_fe_i2c_read(frontend_context->attr.demod_i2c_channel,
                           frontend_context->attr.demod_addr,
                           reg_addr,
                           1, /* reg_addr_offset */
                           reg_val,
                           1); /* size of reg_val */
}

hi_s32 demod_write_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 reg_val)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_check_pointer(frontend_context);
    return drv_fe_i2c_write(frontend_context->attr.demod_i2c_channel,
                            frontend_context->attr.demod_addr,
                            reg_addr,
                            1, /* reg_addr_offset */
                            &reg_val,
                            1); /* size of reg_val */
}

hi_s32 demod_read_bit(hi_u32 port_id, hi_u8 reg_addr, hi_u8 bit_shift, hi_u8 *bit_val)
{
    hi_s32 ret;
    hi_u8 reg_val = 0;
    ret = demod_read_byte(port_id, reg_addr, &reg_val);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    *bit_val = (reg_val & (1 << bit_shift)) ? 1 : 0;
    return HI_SUCCESS;
}

hi_s32 demod_write_bit(hi_u32 port_id, hi_u8 reg_addr, hi_u8 bit_shift, hi_u8 bit_val)
{
    hi_s32 ret;
    hi_u8 reg_val = 0;
    ret = demod_read_byte(port_id, reg_addr, &reg_val);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (bit_val == 0) {
        reg_val &= ~(1 << bit_shift);
    } else {
        reg_val |= 1 << bit_shift;
    }

    return demod_write_byte(port_id, reg_addr, reg_val);
}

hi_s32 tuner_i2c_read(hi_u32 port_id, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_check_pointer(frontend_context);
    demod_ops = &frontend_context->demod_ops;
    if (demod_ops->i2c_bypass != NULL) {
        ret = demod_ops->i2c_bypass(port_id, HI_TRUE);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return drv_fe_i2c_read(frontend_context->attr.tuner_i2c_channel,
                           frontend_context->attr.tuner_addr,
                           reg_addr,
                           reg_addr_offset,
                           buf,
                           buf_len);
}

hi_s32 tuner_i2c_write(hi_u32 port_id, hi_u32 reg_addr, hi_u32 reg_addr_offset, hi_u8 *buf, hi_u32 buf_len)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(frontend_context);
    demod_ops = &frontend_context->demod_ops;
    if (demod_ops->i2c_bypass != NULL) {
        ret = demod_ops->i2c_bypass(port_id, HI_TRUE);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return drv_fe_i2c_write(frontend_context->attr.tuner_i2c_channel,
                            frontend_context->attr.tuner_addr,
                            reg_addr,
                            reg_addr_offset,
                            buf,
                            buf_len);
}

hi_s32 tuner_read_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 *reg_val)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(frontend_context);
    demod_ops = &frontend_context->demod_ops;
    if (demod_ops->i2c_bypass != NULL) {
        ret = demod_ops->i2c_bypass(port_id, HI_TRUE);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return drv_fe_i2c_read(frontend_context->attr.tuner_i2c_channel,
                           frontend_context->attr.tuner_addr,
                           reg_addr,
                           1, /* reg_addr_offset */
                           reg_val,
                           1); /* size of reg_val */
}

hi_s32 tuner_write_byte(hi_u32 port_id, hi_u8 reg_addr, hi_u8 reg_val)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(port_id);
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_check_pointer(frontend_context);
    demod_ops = &frontend_context->demod_ops;
    if (demod_ops->i2c_bypass != NULL) {
        ret = demod_ops->i2c_bypass(port_id, HI_TRUE);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return drv_fe_i2c_write(frontend_context->attr.tuner_i2c_channel,
                            frontend_context->attr.tuner_addr,
                            reg_addr,
                            1, /* reg_addr_offset */
                            &reg_val,
                            1); /* size of reg_val */
}


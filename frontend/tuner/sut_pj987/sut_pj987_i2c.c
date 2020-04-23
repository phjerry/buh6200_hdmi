/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: SUT-PJ987 i2c driver
 * Author: SDK
 * Create: 2019-9-18
 */
#include "drv_frontend_ctrl.h"
#include "hi_drv_i2c.h"

hi_u8 sut_pj987_reg_write(hi_u16 id, hi_u8 subadr, hi_u8 *data, hi_u8 len)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(id);
    if (frontend_context == NULL) {
        return 0;
    }
    if (frontend_context->demod_ops.i2c_bypass != NULL) {
        frontend_context->demod_ops.i2c_bypass(id, HI_TRUE);
    }
    hi_drv_i2c_write_sony(frontend_context->attr.tuner_i2c_channel,
        frontend_context->attr.tuner_addr, subadr, 1, data, len, 0x03); /* 0x03 start and stop enable */
    if (frontend_context->demod_ops.i2c_bypass != NULL) {
        frontend_context->demod_ops.i2c_bypass(id, HI_FALSE);
    }
    return 0;
}

hi_u8 sut_pj987_reg_read(hi_u16 id, hi_u8 subadr, hi_u8 *data, uint8_t len)
{
    drv_fe_ctrl_ctx *frontend_context = drv_frontend_get_context(id);
    if (frontend_context == NULL) {
        return 0;
    }
    if (frontend_context->demod_ops.i2c_bypass != NULL) {
        frontend_context->demod_ops.i2c_bypass(id, HI_TRUE);
    }
    hi_drv_i2c_read_sony(frontend_context->attr.tuner_i2c_channel,
        frontend_context->attr.tuner_addr, subadr, 1, data, len, 0x03); /* 0x03 start and stop enable */
    if (frontend_context->demod_ops.i2c_bypass != NULL) {
        frontend_context->demod_ops.i2c_bypass(id, HI_FALSE);
    }
    return 0;
}


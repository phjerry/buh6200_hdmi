/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description:
 * Author:
 * Create: 2019-08-06
 */

#include "hi_sony_i2c.h"
#include "hi_drv_i2c.h"
#include "hi_errno.h"

sony_result_t hi_sony_i2c_read(sony_i2c_t* pi2c, uint8_t dev_addr, uint8_t* data, uint32_t size, uint8_t mode)
{
    hi_s32 ret;
    hi_s32 i2c_chnl;

    SONY_TRACE_I2C_ENTER("hi_sony_i2c_read");

    if (pi2c == NULL) {
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (pi2c->user == NULL) {
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }

    i2c_chnl = *(hi_u16*)(pi2c->user);
    ret = hi_drv_i2c_read(i2c_chnl, dev_addr, 0, 0, data, size);
    switch (ret) {
        case HI_SUCCESS:
            SONY_TRACE_I2C_RETURN(SONY_RESULT_OK);
        case HI_ERR_I2C_READ_TIMEOUT:
            SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_I2C);
        default:
            SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_OTHER);
    }
}

sony_result_t hi_sony_i2c_write(sony_i2c_t* pi2c, uint8_t dev_addr, const uint8_t *data, uint32_t size, uint8_t mode)
{
    hi_s32 ret;
    hi_s32 i2c_chnl, i;
    hi_u8 buf[100];

    SONY_TRACE_I2C_ENTER("hi_sony_i2c_write");

    if (pi2c == NULL) {
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (pi2c->user == NULL) {
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (size > sizeof(buf)) {
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }

    for (i = 0; i < size && i < sizeof(buf); i++) {
        buf[i] = data[i];
    }
    i2c_chnl = *(hi_u16*)(pi2c->user);
    ret = hi_drv_i2c_write_sony(i2c_chnl, dev_addr, 0, 0, buf, size, mode);

    switch (ret) {
        case HI_SUCCESS:
            SONY_TRACE_I2C_RETURN(SONY_RESULT_OK);
        case HI_ERR_I2C_WRITE_TIMEOUT:
            SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_I2C);
        default:
            SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_OTHER);
    }
}

/* I2c object creation */
hi_s32 hi_sony_create_i2c(sony_i2c_t* pi2c, hi_u16 *pi2c_chnl)
{
    SONY_TRACE_I2C_ENTER("hi_sony_create_i2c");

    if (pi2c == NULL) {
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    pi2c->Read = hi_sony_i2c_read;
    pi2c->Write = hi_sony_i2c_write;
    pi2c->ReadRegister = sony_i2c_CommonReadRegister;
    pi2c->WriteRegister = sony_i2c_CommonWriteRegister;
    pi2c->WriteOneRegister = sony_i2c_CommonWriteOneRegister;
    pi2c->gwAddress = 0;
    pi2c->gwSub = 0;
    pi2c->user = pi2c_chnl; /* Store driver object to user pointer */

    return HI_SUCCESS;
}


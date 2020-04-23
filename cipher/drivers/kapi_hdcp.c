/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_hdcp
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_hdcp.h"

hi_s32 kapi_hdcp_encrypt(hi_cipher_hdcp_attr *attr, const hi_u8 *in, hi_u8 *out, hi_u32 len, hi_u32 operation)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_param(in == HI_NULL);
    hi_log_check_param(attr == HI_NULL);

    ret =  cryp_hdcp_crypto(attr, in, out, len, operation);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_hdcp_crypto, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}


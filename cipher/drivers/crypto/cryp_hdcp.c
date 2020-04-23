/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_hdcp
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "drv_hdcp.h"
#include "cryp_hdcp.h"

/*********************** Internal Structure Definition ***********************/
/** \addtogroup      hdcp */
/** @{ */ /** <!-- [hdcp] */

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      hdcp drivers */
/** @{ */ /** <!-- [hdcp] */

hi_s32 cryp_hdcp_crypto(hi_cipher_hdcp_attr *attr, const hi_u8 *in, hi_u8 *out, hi_u32 len, hi_u32 decrypt)
{
#ifdef CHIP_HDCP_SUPPORT
    return drv_hdcp_encrypt(attr, in, out, len, decrypt);
#else
    return HI_ERR_CIPHER_UNSUPPORTED;
#endif
}

/** @} */ /** <!-- ==== API Code end ==== */

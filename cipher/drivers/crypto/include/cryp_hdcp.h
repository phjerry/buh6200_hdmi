/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_hdcp
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __CRYP_TRNG_H_
#define __CRYP_TRNG_H_

#include "drv_osal_lib.h"
#include "drv_hdcp.h"

/******************************* API Declaration *****************************/
/** \addtogroup      hdcp */
/** @{ */ /** <!--[hdcp] */

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      hdcp drivers */
/** @{ */ /** <!-- [hdcp] */

/**
\brief  HDCP encrypt/decrypt use AES-CBC, IV is all zero.
\param[in]  keysel Hdcp root key sel.
\param[in]  ramsel hdmi ram sel.
\param[in]  in The input data.
\param[out] out The input data.
\param[out] len The input data length, must align with 16.
\param[in]  operation encrypt or decrypt.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 cryp_hdcp_crypto(hi_cipher_hdcp_attr *attr, const hi_u8 *in, hi_u8 *out, hi_u32 len, hi_u32 operation);

/** @} */ /** <!-- ==== API declaration end ==== */
#endif

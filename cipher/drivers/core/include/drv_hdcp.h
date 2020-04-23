/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_hdcp
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __DRV_HDCP_H__
#define __DRV_HDCP_H__

/*************************** Structure Definition ****************************/
/** \addtogroup     hdcp */
/** @{ */ /** <!-- [hdcp] */

/* ! \hdcp capacity, 0-nonsupport, 1-support */
typedef struct {
    hi_u32 hdcp : 1; /* !<  Support HDCP */
} hdcp_capacity;

typedef struct {
    hi_u8 rx_read; /* rx read enable or not */
    hi_u8 tx_read; /* tx read enable or not */
    hi_u8 key_sel; /* otp key, hisi key or cpu */
    hi_u8 crc4;    /* crc4 of KSV[1~4] */
} hdcp_module_status;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      hdcp */
/** @{ */ /** <!--[hdcp] */

/**
\brief  HDCP encrypt/decrypt use AES-CBC, IV is all zero.
\param[in]  keysel Hdcp root key sel.
\param[in]  ramsel hdmi ram sel.
\param[in]  in The input data.
\param[out] out The input data.
\param[out] len The input data length, must align with 16.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_hdcp_encrypt(hi_cipher_hdcp_attr *attr, const hi_u8 *in, hi_u8 *out, hi_u32 len, hi_u32 decrypt);

/**
\brief  get the hdcp status.
\param[out] module The hdcp status.
\retval     NA.
 */
void drv_hdcp_proc_status(hdcp_module_status *module);

/** @} */ /** <!-- ==== API declaration end ==== */

#endif

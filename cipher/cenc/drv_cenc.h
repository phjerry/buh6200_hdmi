/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_cenc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __TEE_CENC_H__
#define __TEE_CENC_H__

#include "hi_drv_cipher.h"

#define CENC_ODDKEY_SUPPORT 0x80000000
#define CENC_KEY_SIZE       16
#define CENC_IV_SIZE        16

hi_s32 cenc_decrypt(hi_u32 id,
                    hi_cipher_cenc *cenc,
                    compat_addr inputphy,
                    compat_addr outputphy,
                    hi_u32 bytelength);

#endif /* __DRV_CIPHER_INTF_H__ */

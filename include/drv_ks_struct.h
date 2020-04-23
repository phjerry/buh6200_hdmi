/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Key slot drvier level head file.
 * Author: linux SDK team
 * Create: 2019/09/27
 */

#ifndef __DRV_KS_STRUCT_H__
#define __DRV_KS_STRUCT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Define the maximum number of TScipher keyslot. */
#define HI_KEYSLOT_TSCIPHER_MAX   256

/* Define the maximum number of Mcipher keyslot. */
#define HI_KEYSLOT_MCIPHER_MAX    15

/* Define the maximum number of HMAC keyslot. */
#define HI_KEYSLOT_HMAC_MAX       1

typedef enum {
    HI_KEYSLOT_TYPE_TSCIPHER = 0x00,
    HI_KEYSLOT_TYPE_MCIPHER,
    HI_KEYSLOT_TYPE_HMAC,
    HI_KEYSLOT_TYPE_MAX
} hi_keyslot_type;

typedef union {
    struct {
        hi_u8 type     : 7; /* come from enum hi_keyslot_type */
        hi_u8 secure   : 1; /* 1 means TEE, 0 means REE */
    } bits;
    hi_u8 u8;
} hi_keyslot_priv_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_KS_STRUCT_H__ */


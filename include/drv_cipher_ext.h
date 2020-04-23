/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_cipher_ext
 * Author: zhaoguihong
 * Create: 2019-06-18
 */

#ifndef __DRV_CIPHER_EXT_H__
#define __DRV_CIPHER_EXT_H__

/* add include here */
#include "hi_drv_cipher.h"
#include "drv_cipher_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef hi_void (*hi_cipher_call_back)(hi_u32);

typedef hi_s32  (*fn_cipher_create_handle)(hi_handle *handle, hi_cipher_attr *cipher_attr);
typedef hi_s32  (*fn_cipher_config_chn)(hi_handle handle, hi_cipher_ctrl *config);
typedef hi_s32  (*fn_cipher_get_key_slot_handle)(hi_handle cipher, hi_handle *key_slot);
typedef hi_s32  (*fn_cipher_destroy_handle)(hi_handle handle);
typedef hi_s32  (*fn_cipher_encrypt)(hi_handle handle, hi_u64 src_phy_addr, hi_u64 dest_phy_addr, hi_u32 data_length);
typedef hi_s32  (*fn_cipher_decrypt)(hi_handle handle, hi_u64 src_phy_addr, hi_u64 dest_phy_addr, hi_u32 data_length);
typedef hi_s32  (*fn_cipher_get_random_number)(hi_u32 *randnum, hi_u32 timeout);
typedef hi_s32  (*fn_cipher_load_hdcp_key)(hi_cipher_hdcp_attr *attr, const hi_u8 *key, hi_u32 keylen);
typedef hi_s32  (*fn_cipher_soft_reset)(hi_void);
typedef hi_s32  (*fn_cipher_resume)(hi_void);
typedef hi_void (*fn_cipher_suspend)(hi_void);
typedef hi_s32  (*fn_cipher_set_iv)(hi_handle handle, hi_u8 *iv, hi_u32 iv_len);
typedef hi_s32  (*fn_cipher_hash_start)(hi_handle *handle, hi_cipher_hash_type type, hi_u8 *key, hi_u32 keylen);
typedef hi_s32  (*fn_cipher_hash_update)(hi_handle handle, hi_u8 *input, hi_u32 length);
typedef hi_s32  (*fn_cipher_hash_finish)(hi_handle handle, hi_u8 *hash, hi_u32 *hashlen);

typedef struct {
    fn_cipher_create_handle      hi_cipher_create_handle;
    fn_cipher_config_chn         hi_cipher_config_chn;
    fn_cipher_get_key_slot_handle hi_cipher_get_key_slot_handle;
    fn_cipher_destroy_handle     hi_cipher_destroy_handle;
    fn_cipher_encrypt            hi_cipher_encrypt;
    fn_cipher_decrypt            hi_cipher_decrypt;
    fn_cipher_get_random_number  hi_cipher_get_random_number;
    fn_cipher_load_hdcp_key      hi_cipher_load_hdcp_key;
    fn_cipher_soft_reset         hi_cipher_soft_reset;
    fn_cipher_resume             hi_cipher_resume;
    fn_cipher_suspend            hi_cipher_suspend;
    fn_cipher_set_iv             hi_cipher_set_iv;
    fn_cipher_hash_start         hi_cipher_hash_start;
    fn_cipher_hash_update        hi_cipher_hash_update;
    fn_cipher_hash_finish        hi_cipher_hash_finish;
} hi_cipher_export_fun;

hi_s32  cipher_drv_mod_init(hi_void);
hi_void cipher_drv_mod_exit(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* __DRV_CIPHER_EXT_H__ */


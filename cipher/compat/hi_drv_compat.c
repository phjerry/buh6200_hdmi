/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of hi_drv_compat
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "drv_symc.h"
#include "hi_drv_cipher.h"

#define HDCP_KEY_FLAG_MASK 0xC0
#define HDCP_KEY_FLAG_HISI 0x80
#define CRC32_P32          0xEDB88320L
#define BYTE_BITS          8
#define BYTE_MASK          0xFF

hi_s32 hi_drv_cipher_create_handle(hi_handle *handle, hi_cipher_attr *cipher_attr)
{
    return kapi_symc_create(handle, cipher_attr->cipher_type);
}

hi_s32 hi_drv_cipher_config_chn(hi_handle handle, hi_cipher_ctrl *ctrl)
{
    hi_log_check_param(ctrl == HI_NULL);
    return kapi_symc_config(handle,
                            ctrl->alg,
                            ctrl->work_mode, ctrl->bit_width, ctrl->key_len,
                            (hi_u8 *)ctrl->iv, AES_IV_SIZE,
                            ctrl->change_flags.bit1_iv, ADDR_NULL, 0, 0);
}

hi_s32 hi_drv_cipher_get_key_slot_handle(hi_handle cipher, hi_handle *key_slot)
{
    return kapi_symc_get_keyslot(cipher, key_slot);
}

hi_s32 hi_drv_cipher_destroy_handle(hi_handle handle)
{
    return kapi_symc_destroy(handle);
}

hi_s32 hi_drv_cipher_encrypt(hi_handle handle, hi_u64 src_phy_addr, hi_u64 dest_phy_addr, hi_u32 data_length)
{
    compat_addr input;
    compat_addr output;

    ADDR_U64(input)  = src_phy_addr;
    ADDR_U64(output) = dest_phy_addr;

    return kapi_symc_crypto(handle, input, output, data_length,
        SYMC_OPERATION_ENCRYPT, HI_FALSE, HI_FALSE);
}

hi_s32 hi_drv_cipher_decrypt(hi_handle handle, hi_u64 src_phy_addr, hi_u64 dest_phy_addr, hi_u32 data_length)
{
    compat_addr input;
    compat_addr output;

    ADDR_U64(input)  = src_phy_addr;
    ADDR_U64(output) = dest_phy_addr;

    return kapi_symc_crypto(handle, input, output, data_length,
        SYMC_OPERATION_ENCRYPT, HI_TRUE, HI_FALSE);
}

hi_s32 hi_drv_cipher_get_random_number(hi_u32 *randnum, hi_u32 timeout)
{
    return kapi_trng_get_random(randnum, timeout);;
}

hi_s32 hi_drv_cipher_soft_reset(hi_void)
{
    hi_s32 ret;
    drv_symc_suspend();
    ret = drv_symc_resume();
    if (ret != HI_SUCCESS) {
        HI_ERR_CIPHER("drv symc resume failed!\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_cipher_load_hdcp_key(hi_cipher_hdcp_attr *attr, const hi_u8 *key, hi_u32 keylen)
{
    return kapi_hdcp_encrypt(attr, key, HI_NULL, keylen, SYMC_OPERATION_DECRYPT);
}

/* adapt for opentv */
hi_s32 hi_drv_cipher_config_chn_ex(hi_cipher_config_ctrl_ex *ctrl)
{
    compat_addr a_phy_addr;

    hi_log_check_param(ctrl == HI_NULL);

    ADDR_U64(a_phy_addr) = ctrl->a_phy_addr;

    return kapi_symc_config(ctrl->handle, /* handle or softid */
                            ctrl->alg,
                            ctrl->work_mode,
                            ctrl->bit_width,
                            ctrl->key_len,
                            (hi_u8 *)ctrl->iv, ctrl->iv_len, ctrl->change_flags.bit1_iv,
                            a_phy_addr, ctrl->a_len, ctrl->tag_len);
}

/* adapt for pm */
hi_s32 hi_drv_cipher_hash_start(hi_handle *handle, hi_cipher_hash_type type,
                                hi_u8 *key, hi_u32 keylen)
{
    return kapi_hash_start(handle, type, key, keylen);
}

hi_s32 hi_drv_cipher_hash_update(hi_handle handle, hi_u8 *input, hi_u32 length)
{
    return kapi_hash_update(handle, input, length, HASH_CHUNCK_SRC_LOCAL);
}

hi_s32 hi_drv_cipher_hash_finish(hi_handle handle, hi_u8 *hash, hi_u32 *hash_len)
{
    return kapi_hash_finish(handle, hash, HASH_RESULT_MAX_SIZE_IN_WORD * WORD_WIDTH, hash_len);
}

hi_s32 hi_drv_cipher_rsa_sign(hi_cipher_rsa_sign *rsa_sign,
                              hi_u8 *hash_data,
                              hi_u32 hash_len,
                              hi_u8 *out_sign,
                              hi_u32 *out_sign_len)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    cryp_rsa_key key;
    hi_s32 ret;

    hi_log_check_param(rsa_sign == HI_NULL);
    hi_log_check_param(rsa_sign->pri_key.n_len != rsa_sign->pri_key.d_len);

    ret = memset_s(&key, sizeof(key), 0, sizeof(key));
    if (ret != HI_SUCCESS) {
        HI_ERR_CIPHER("Memset_s failed!");
        return ret;
    }
    key.public = HI_FALSE;
    key.klen = rsa_sign->pri_key.n_len;
    key.n = rsa_sign->pri_key.n;
    key.d = rsa_sign->pri_key.d;

    return kapi_rsa_sign_hash(&key, rsa_sign->scheme, hash_data, hash_len,
                              out_sign, out_sign_len);
#else
    hi_log_error("Unsupport RSA\n");
    return HI_FAILURE;
#endif
}

hi_s32 hi_drv_cipher_rsa_verify(hi_cipher_rsa_verify *rsa_verify,
                                hi_u8 *hash_data,
                                hi_u32 hash_len,
                                hi_u8 *in_sign,
                                hi_u32 in_sign_len)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    cryp_rsa_key key;
    hi_u8 *buf = HI_NULL;
    hi_u8 *pub = HI_NULL;
    hi_u32 len;
    hi_u32 i;
    hi_s32 ret;

    hi_log_check_param(rsa_verify == HI_NULL);
    ret = memset_s(&key, sizeof(key), 0, sizeof(key));
    if (ret != HI_SUCCESS) {
        HI_ERR_CIPHER("Memset_s failed!");
        return ret;
    }
    key.public = HI_TRUE;
    key.klen = rsa_verify->pub_key.n_len;
    key.n = rsa_verify->pub_key.n;
    buf = rsa_verify->pub_key.e;
    pub = (hi_u8 *)&key.e;
    len = rsa_verify->pub_key.e_len;

    for (i = 0; i < cipher_min(WORD_WIDTH, len); i++) {
        pub[WORD_WIDTH - i - 1] = buf[len - i - 1];
    }

    return kapi_rsa_verify_hash(&key, rsa_verify->scheme, hash_data, hash_len,
                                in_sign, in_sign_len);
#else
    hi_log_error("Unsupport RSA\n");
    return HI_FAILURE;
#endif
}

EXPORT_SYMBOL(hi_drv_cipher_config_chn_ex);
EXPORT_SYMBOL(hi_drv_cipher_config_chn);
EXPORT_SYMBOL(hi_drv_cipher_destroy_handle);
EXPORT_SYMBOL(hi_drv_cipher_encrypt);
EXPORT_SYMBOL(hi_drv_cipher_create_handle);
EXPORT_SYMBOL(hi_drv_cipher_decrypt);
EXPORT_SYMBOL(hi_drv_cipher_hash_start);
EXPORT_SYMBOL(hi_drv_cipher_hash_update);
EXPORT_SYMBOL(hi_drv_cipher_hash_finish);
EXPORT_SYMBOL(hi_drv_cipher_rsa_sign);
EXPORT_SYMBOL(hi_drv_cipher_rsa_verify);


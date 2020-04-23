/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_rsa
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_rsa.h"

#ifdef HI_PRODUCT_RSA_SUPPORT

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      rsa */
/** @{ */ /** <!-- [kapi] */

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 kapi_rsa_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_init, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 kapi_rsa_deinit(void)
{
    hi_log_func_enter();

    cryp_rsa_deinit();

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_encrypt(cryp_rsa_key *key,
                        hi_cipher_rsa_enc_scheme scheme,
                        hi_u8 *in, hi_u32 inlen,
                        hi_u8 *out, hi_u32 *outlen)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_encrypt(key, scheme, in, inlen, out, outlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_encrypt, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_decrypt(cryp_rsa_key *key,
                        hi_cipher_rsa_enc_scheme scheme,
                        hi_u8 *in, hi_u32 inlen,
                        hi_u8 *out, hi_u32 *outlen)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_decrypt(key, scheme, in, inlen, out, outlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_decrypt, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_sign_hash(cryp_rsa_key *key,
                          hi_cipher_rsa_sign_scheme scheme,
                          hi_u8 *hash, hi_u32 hlen,
                          hi_u8 *sign, hi_u32 *signlen)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_sign_hash(key, scheme, hash, hlen, sign, signlen, hlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_sign_hash, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_verify_hash(cryp_rsa_key *key,
                            hi_cipher_rsa_sign_scheme scheme,
                            hi_u8 *hash, hi_u32 hlen,
                            hi_u8 *sign, hi_u32 signlen)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_rsa_verify_hash(key, scheme, hash, hlen, sign, signlen, hlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_verify_hash, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_rsa_gen_key(hi_u32 numbits, hi_u32 exponent, cryp_rsa_key *key)
{
    hi_s32 ret;

    hi_log_func_enter();

    hi_log_check_param(key == HI_NULL);
    hi_log_check_param(key->p == HI_NULL);
    hi_log_check_param(key->q == HI_NULL);
    hi_log_check_param(key->dp == HI_NULL);
    hi_log_check_param(key->dq == HI_NULL);
    hi_log_check_param(key->qp == HI_NULL);
    hi_log_check_param(RSA_KEY_BITWIDTH_4096 * BITS_IN_BYTE < numbits);

    if ((exponent != RSA_KEY_EXPONENT_VALUE1) && (exponent != RSA_KEY_EXPONENT_VALUE2)) {
        hi_log_error("RSA compute crt params, e error\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    ret = cryp_rsa_gen_key(numbits, exponent, key);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_rsa_gen_key, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

/** @} */ /** <!-- ==== Structure Definition end ==== */

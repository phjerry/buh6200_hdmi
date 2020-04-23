/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_ecc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_ecc.h"

#ifdef HI_PRODUCT_ECC_SUPPORT
#define BYTE_BITS               8
#define ECC_MAX_SIZE            72

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecc_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_ecc_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_ecc_init, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecc_deinit(void)
{
    hi_log_func_enter();

    cryp_ecc_deinit();

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_ecc_gen_key(ecc_param_t *ecc, hi_u8 *inkey, hi_u8 *outkey, hi_u8 *px, hi_u8 *py)
{
    hi_s32 ret;
    ecc_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(ecc == HI_NULL);
    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);

    hi_log_check_param(ecc->p == HI_NULL);
    hi_log_check_param(ecc->a == HI_NULL);
    hi_log_check_param(ecc->b == HI_NULL);
    hi_log_check_param(ecc->gx == HI_NULL);
    hi_log_check_param(ecc->gy == HI_NULL);
    hi_log_check_param(ecc->n == HI_NULL);
    hi_log_check_param(ecc->ksize > ECC_MAX_SIZE);
    hi_log_check_param(ecc->ksize == 0);

    func = cryp_get_ecc_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->genkey == HI_NULL);

    ret = func->genkey(ecc, inkey, outkey, px, py);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(func->genkey, ret);
        return ret;
    }
    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_ecdh_compute_key(ecc_param_t *ecc, hi_u8 *d, hi_u8 *px, hi_u8 *py, hi_u8 *sharekey)
{
    hi_s32 ret;
    ecc_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(ecc == HI_NULL);
    hi_log_check_param(d == HI_NULL);
    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);
    hi_log_check_param(sharekey == HI_NULL);

    hi_log_check_param(ecc->p == HI_NULL);
    hi_log_check_param(ecc->a == HI_NULL);
    hi_log_check_param(ecc->b == HI_NULL);
    hi_log_check_param(ecc->gx == HI_NULL);
    hi_log_check_param(ecc->gy == HI_NULL);
    hi_log_check_param(ecc->n == HI_NULL);
    hi_log_check_param(ecc->ksize > ECC_MAX_SIZE);
    hi_log_check_param(ecc->ksize == 0);

    func = cryp_get_ecc_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->ecdh == HI_NULL);

    ret = func->ecdh(ecc, d, px, py, sharekey);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(func->ecdh, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_ecdsa_sign_hash(ecc_param_t *ecc, hi_u8 *d, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s)
{
    hi_s32 ret;
    ecc_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(ecc == HI_NULL);
    hi_log_check_param(d == HI_NULL);
    hi_log_check_param(hash == HI_NULL);
    hi_log_check_param(r == HI_NULL);
    hi_log_check_param(s == HI_NULL);

    hi_log_check_param(ecc->p == HI_NULL);
    hi_log_check_param(ecc->a == HI_NULL);
    hi_log_check_param(ecc->b == HI_NULL);
    hi_log_check_param(ecc->gx == HI_NULL);
    hi_log_check_param(ecc->gy == HI_NULL);
    hi_log_check_param(ecc->n == HI_NULL);
    hi_log_check_param(ecc->ksize > ECC_MAX_SIZE);
    hi_log_check_param(ecc->ksize == 0);

    func = cryp_get_ecc_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->sign == HI_NULL);

    ret = func->sign(ecc, d, hash, hlen, r, s);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(func->sign, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 kapi_ecdsa_verify_hash(ecc_param_t *ecc, hi_u8 *px, hi_u8 *py, hi_u8 *hash,
                              hi_u32 hlen, hi_u8 *r, hi_u8 *s)
{
    hi_s32 ret;
    ecc_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(ecc == HI_NULL);
    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);
    hi_log_check_param(hash == HI_NULL);
    hi_log_check_param(r == HI_NULL);
    hi_log_check_param(s == HI_NULL);

    hi_log_check_param(ecc->p == HI_NULL);
    hi_log_check_param(ecc->a == HI_NULL);
    hi_log_check_param(ecc->b == HI_NULL);
    hi_log_check_param(ecc->gx == HI_NULL);
    hi_log_check_param(ecc->gy == HI_NULL);
    hi_log_check_param(ecc->n == HI_NULL);
    hi_log_check_param(ecc->ksize > ECC_MAX_SIZE);
    hi_log_check_param(ecc->ksize == 0);

    func = cryp_get_ecc_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->verify == HI_NULL);

    ret = func->verify(ecc, px, py, hash, hlen, r, s);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(func->verify, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

}

#endif


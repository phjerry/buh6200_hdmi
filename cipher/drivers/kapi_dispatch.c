/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_dispatch
 * Author: zhaoguihong
 * Create: 2019-06-18
 */

#include "drv_osal_lib.h"
#include "cryp_symc.h"
#include "ext_alg.h"
#include "drv_cenc.h"

/* ! \max pakage numher of symc mutli encrypt */
#define SYMC_MULTI_MAX_PKG      0x1000
#define RSA_PUBLIC_BUFFER_NUM   0x03
#define RSA_PRIVATE_BUFFER_NUM  0x07
#define ECC_PARAM_CNT           0x06
#define ECC_KLEN_ALIGN          0x08
#define ECC_KLEN_MIN            0x20

#define SM2_ENCRYPT_PAD_LEN  (SM2_LEN_IN_BYTE * 3)
#define MAX_MALLOC_BUF_SIZE  0x10000
#define MAX_CMAC_BUF_SIZE    0x20000000  /* 512M */
#define MAX_CENC_SUB_SAMPLE  100

typedef hi_s32 (*hi_drv_func)(hi_void *param);

typedef struct {
    const char *name;
    hi_drv_func func;
    hi_u32 cmd;
} crypto_dispatch_func;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      link */
/** @{ */ /** <!-- [link] */

static hi_s32 dispatch_symc_create_handle(hi_void *argp)
{
    hi_s32 ret;
    symc_create_t *symc_create = argp;

    hi_log_func_enter();

    /* allocate a aes channel */
    ret = kapi_symc_create(&symc_create->id, symc_create->type);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_create, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_symc_destroy_handle(hi_void *argp)
{
    hi_s32 ret;
    symc_destroy_t *destroy = argp;

    hi_log_func_enter();

    ret = kapi_symc_destroy(destroy->id);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_destroy, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_symc_config(hi_void *argp)
{
    hi_s32 ret;
    symc_config_t *config = argp;
    compat_addr in;

    hi_log_func_enter();
    ADDR_U64(in) = crypto_bufhandle_to_phys(config->aad);
    ret = kapi_symc_config(config->id, config->alg, config->mode, config->width, config->klen,
        (hi_u8 *)config->iv, config->ivlen, config->iv_usage, in, config->alen, config->tlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_config, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_symc_encrypt(hi_void *argp)
{
    hi_s32 ret;
    symc_encrypt_t *encrypt = argp;
    compat_addr in;
    compat_addr out;

    hi_log_func_enter();
    ADDR_U64(in) = crypto_bufhandle_to_phys(encrypt->input);
    ADDR_U64(out) = crypto_bufhandle_to_phys(encrypt->output);
    ret = kapi_symc_crypto(encrypt->id, in, out, encrypt->length,
        encrypt->operation, HI_FALSE, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_crypto, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_symc_encrypt_multi(hi_void *argp)
{
#ifdef HI_PRODUCT_MULTI_CIPHER_SUPPORT
    hi_s32 ret;
    symc_encrypt_multi_t *encrypt_mutli = argp;

    hi_log_func_enter();

    hi_log_debug("operation %d\n", encrypt_mutli->operation);
    ret = kapi_symc_crypto_multi(encrypt_mutli->id, (const void *)(ADDR_VIA(encrypt_mutli->pkg)),
        encrypt_mutli->pkg_num, encrypt_mutli->operation, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_crypto_multi, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport mutli encrypt\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_symc_get_tag(hi_void *argp)
{
#ifdef HI_PRODUCT_AEAD_SUPPORT
    hi_s32 ret;
    aead_tag_t *aead_tag = argp;

    hi_log_func_enter();

    ret = kapi_aead_get_tag(aead_tag->id, aead_tag->tag, &aead_tag->taglen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_aead_get_tag, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport get tag\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_symc_get_config(hi_void *argp)
{
    hi_s32 ret;
    symc_get_config_t *get_config = argp;

    hi_log_func_enter();

    ret = kapi_symc_get_config(get_config->id, &get_config->ctrl);

    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_get_config, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_symc_get_keyslot(hi_void *argp)
{
    hi_s32 ret;
    symc_keyslot_t *get_keyslot = argp;

    hi_log_func_enter();

    ret = kapi_symc_get_keyslot(get_keyslot->cipher, &get_keyslot->keyslot);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_get_keyslot, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_symc_camc(hi_void *argp)
{
#ifdef HI_PRODUCT_CBC_MAC_SUPPORT
    hi_s32 ret;
    symc_cmac_t *cmac = argp;

    hi_log_func_enter();

    if (cmac->last == HI_TRUE) {
        check_exit(ext_aes_cmac_finish(cmac->id, ADDR_VIA(cmac->in), cmac->inlen, HI_TRUE, cmac->mac));
    } else {
        check_exit(ext_aes_cmac_update(cmac->id, ADDR_VIA(cmac->in), cmac->inlen, HI_TRUE));
    }

exit__:
    if (ret != HI_SUCCESS) {
        hi_log_print_err_code(ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport cmac\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_hash_start(hi_void *argp)
{
    hi_s32 ret;
    hash_start_t *start = argp;
    hi_u8 *key = HI_NULL;

    hi_log_func_enter();

    if (HI_CIPHER_HASH_TYPE_HMAC_SHA1 == start->type
        || HI_CIPHER_HASH_TYPE_HMAC_SHA224 == start->type
        || HI_CIPHER_HASH_TYPE_HMAC_SHA256 == start->type
#ifdef HI_PRODUCT_SHA512_SUPPORT
        || HI_CIPHER_HASH_TYPE_HMAC_SHA384 == start->type
        || HI_CIPHER_HASH_TYPE_HMAC_SHA512 == start->type
#endif
) {

        hi_log_check_param(start->keylen > MAX_MALLOC_BUF_SIZE);
        hi_log_check_param(ADDR_VIA(start->key) == HI_NULL);

        key = (hi_u8 *)crypto_calloc(1, start->keylen);
        if (key == HI_NULL) {
            hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
            return HI_ERR_CIPHER_FAILED_MEM;
        }

        check_exit(crypto_copy_from_user(key, start->keylen, ADDR_VIA(start->key), start->keylen));
    }

    check_exit(kapi_hash_start(&start->id, start->type, key, start->keylen));

    if (key != HI_NULL) {
        crypto_free(key);
        key = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (key != HI_NULL) {
        crypto_free(key);
        key = HI_NULL;
    }

    return ret;
}

static hi_s32 dispatch_hash_update(hi_void *argp)
{
    hi_s32 ret;
    hash_update_t *update = argp;

    hi_log_func_enter();

    hi_log_check_param(ADDR_VIA(update->input) == HI_NULL);

    update->src = HASH_CHUNCK_SRC_USER;
    ret = kapi_hash_update(update->id,
                           ADDR_VIA(update->input),
                           update->length,
                           update->src);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_hash_update, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 dispatch_hash_finish(hi_void *argp)
{
    hi_s32 ret;
    hash_finish_t *finish = argp;

    hi_log_func_enter();

    ret = kapi_hash_finish(finish->id, (hi_u8 *)finish->hash, sizeof(finish->hash), &finish->hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_hash_finish, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

#ifdef HI_PRODUCT_RSA_SUPPORT
static hi_s32 rsa_alloc_buffer(cryp_rsa_key *key, rsa_info_t *rsa_info,
                               hi_u8 **in, hi_u8 **out)
{
    hi_u32 size = 0;
    hi_u32 klen = 0;
    hi_s32 ret;
    hi_u8 *buf = HI_NULL;

    hi_log_func_enter();

    if (rsa_info->public == HI_FALSE) {
        hi_log_check_param((ADDR_VIA(rsa_info->d) == HI_NULL)
                           && ((ADDR_VIA(rsa_info->p) == HI_NULL)
                                     || (ADDR_VIA(rsa_info->q) == HI_NULL)
                                     || (ADDR_VIA(rsa_info->dp) == HI_NULL)
                                     || (ADDR_VIA(rsa_info->dq) == HI_NULL)
                                     || (ADDR_VIA(rsa_info->qp) == HI_NULL)));
    }

    hi_log_check_param(rsa_info->inlen > rsa_info->klen);
    hi_log_check_param(rsa_info->outlen > rsa_info->klen);
    hi_log_check_param(rsa_info->klen < RSA_MIN_KEY_LEN);
    hi_log_check_param(rsa_info->klen > RSA_MAX_KEY_LEN);

    ret = memset_s(key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    key->klen = klen = rsa_info->klen;
    key->public = rsa_info->public;

    if (rsa_info->public) {
        /* buffer size of key, input and output */
        size = rsa_info->klen * RSA_PUBLIC_BUFFER_NUM;

        buf = crypto_calloc(1, size);
        if (buf == HI_NULL) {
            hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
            hi_log_print_func_err(crypto_calloc, ret);
            return HI_ERR_CIPHER_FAILED_MEM;
        }

        key->n = buf;
        buf += klen;
        *in = buf;
        buf += klen;
        *out = buf;
        buf += klen;

        check_exit(crypto_copy_from_user(key->n, klen, ADDR_VIA(rsa_info->n), klen));
        check_exit(crypto_copy_from_user(*in, klen, ADDR_VIA(rsa_info->in), klen));
        key->e = rsa_info->e;
    } else {
        /* n + d or n + p + q + dP + dQ + qp
         * the length of n/d is klen,
         * the length of p/q/dP/dQ/qp is klen/2,
         * the length of input is klen
         * the length of output is klen
 */
        size = klen * RSA_PRIVATE_BUFFER_NUM;

        buf = crypto_calloc(1, size);
        hi_log_check_param(buf == HI_NULL);

        key->n = buf;
        buf += klen;
        key->d = buf;
        buf += klen;
        key->p = buf;
        buf += klen >> 1;
        key->q = buf;
        buf += klen >> 1;
        key->dp = buf;
        buf += klen >> 1;
        key->dq = buf;
        buf += klen >> 1;
        key->qp = buf;
        buf += klen >> 1;
        key->e = rsa_info->e;

        if (ADDR_VIA(rsa_info->n) != HI_NULL) {
            check_exit(crypto_copy_from_user(key->n, klen, ADDR_VIA(rsa_info->n), klen));
        }

        if (ADDR_VIA(rsa_info->d) != HI_NULL) {
            check_exit(crypto_copy_from_user(key->d, klen, ADDR_VIA(rsa_info->d), klen));
        } else {
            check_exit(crypto_copy_from_user(key->p, klen >> 1, ADDR_VIA(rsa_info->p), klen >> 1));
            check_exit(crypto_copy_from_user(key->q, klen >> 1, ADDR_VIA(rsa_info->q), klen >> 1));
            check_exit(crypto_copy_from_user(key->dp, klen >> 1, ADDR_VIA(rsa_info->dp), klen >> 1));
            check_exit(crypto_copy_from_user(key->dq, klen >> 1, ADDR_VIA(rsa_info->dq), klen >> 1));
            check_exit(crypto_copy_from_user(key->qp, klen >> 1, ADDR_VIA(rsa_info->qp), klen >> 1));
            key->d = HI_NULL;
        }

        *in = buf;
        buf += klen;
        *out = buf;
        buf += klen;

        if (ADDR_VIA(rsa_info->in) != HI_NULL) {
            check_exit(crypto_copy_from_user(*in, klen, ADDR_VIA(rsa_info->in), rsa_info->inlen));
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (key->n != HI_NULL) {
        crypto_free(key->n);
        key->n = HI_NULL;
    }

    hi_log_error("error, copy rsa key from user failed\n");
    hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);

    return HI_ERR_CIPHER_FAILED_MEM;
}

static hi_void rsa_free_buffer(cryp_rsa_key *key)
{
    hi_log_func_enter();

    if (key->n != HI_NULL) {
        crypto_free(key->n);
        key->n = HI_NULL;
    }

    hi_log_func_exit();
    return;
}
#endif

static hi_s32 dispatch_rsa_encrypt(hi_void *argp)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    hi_s32 ret;
    rsa_info_t *rsa_info = argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    cryp_rsa_key key = { 0 };

    hi_log_func_enter();

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa_alloc_key failed\n");
        hi_log_print_func_err(rsa_alloc_buffer, ret);
        return ret;
    }

    ret = kapi_rsa_encrypt(&key, rsa_info->enc_scheme, in, rsa_info->inlen,
                           out, &rsa_info->outlen);
    if (ret != HI_SUCCESS) {
        rsa_free_buffer(&key);
        hi_log_print_func_err(kapi_rsa_encrypt, ret);
        return ret;
    }

    ret = crypto_copy_to_user(ADDR_VIA(rsa_info->out), rsa_info->outlen, out, rsa_info->outlen);
    if (ret != HI_SUCCESS) {
        rsa_free_buffer(&key);
        hi_log_print_func_err(crypto_copy_to_user, ret);
        return ret;
    }

    rsa_free_buffer(&key);

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport rsa encrypt\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_rsa_decrypt(hi_void *argp)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    hi_s32 ret;
    rsa_info_t *rsa_info = argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    cryp_rsa_key key = { 0 };

    hi_log_func_enter();

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa_alloc_key failed\n");
        hi_log_print_func_err(rsa_alloc_buffer, ret);
        return ret;
    }

    ret = kapi_rsa_decrypt(&key, rsa_info->enc_scheme,
                           in, rsa_info->inlen, out, &rsa_info->outlen);
    if (ret != HI_SUCCESS) {
        rsa_free_buffer(&key);
        hi_log_print_func_err(kapi_rsa_decrypt, ret);
        return ret;
    }

    ret = crypto_copy_to_user(ADDR_VIA(rsa_info->out), rsa_info->outlen, out, rsa_info->outlen);
    if (ret != HI_SUCCESS) {
        rsa_free_buffer(&key);
        hi_log_print_func_err(crypto_copy_to_user, ret);
        return ret;
    }

    rsa_free_buffer(&key);

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport rsa decrypt\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_rsa_sign_hash(hi_void *argp)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    hi_s32 ret;
    rsa_info_t *rsa_info = argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    cryp_rsa_key key = { 0 };

    hi_log_func_enter();

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa alloc key buffer failed\n");
        hi_log_print_func_err(rsa_alloc_buffer, ret);
        return ret;
    }

    ret = kapi_rsa_sign_hash(&key, rsa_info->sign_scheme, in,
                             rsa_info->inlen, out, &rsa_info->outlen);
    if (ret != HI_SUCCESS) {
        rsa_free_buffer(&key);
        hi_log_print_func_err(kapi_rsa_sign_hash, ret);
        return ret;
    }

    ret = crypto_copy_to_user(ADDR_VIA(rsa_info->out), rsa_info->outlen, out, rsa_info->outlen);
    if (ret != HI_SUCCESS) {
        rsa_free_buffer(&key);
        hi_log_print_func_err(crypto_copy_to_user, ret);
        return ret;
    }

    rsa_free_buffer(&key);

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport rsa sign\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_rsa_verify_hash(hi_void *argp)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    hi_s32 ret;
    rsa_info_t *rsa_info = argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    cryp_rsa_key key = { 0 };

    hi_log_func_enter();

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa_alloc_key failed\n");
        hi_log_print_func_err(rsa_alloc_buffer, ret);
        return ret;
    }

    /* copy hash value from user */
    check_exit(crypto_copy_from_user(out, key.klen, ADDR_VIA(rsa_info->out), rsa_info->outlen));
    check_exit(kapi_rsa_verify_hash(&key,
                                    rsa_info->sign_scheme,
                                    out,
                                    rsa_info->outlen,
                                    in,
                                    rsa_info->inlen));
    rsa_free_buffer(&key);
    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    rsa_free_buffer(&key);

    return ret;
#else
    hi_log_error("Unsupport rsa verify\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_rsa_compute_crt(hi_void *argp)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    hi_s32 ret;
    rsa_info_t *rsa_info = argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    cryp_rsa_key key = { 0 };

    hi_log_func_enter();

    ret = memset_s(&key, sizeof(key), 0, sizeof(key));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    rsa_info->public = HI_FALSE;
    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa_alloc_key failed\n");
        hi_log_print_func_err(rsa_alloc_buffer, ret);
        return ret;
    }

    check_exit(ext_rsa_compute_crt(&key));

    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->dp), key.klen >> 1, key.dp, key.klen >> 1));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->dq), key.klen >> 1, key.dq, key.klen >> 1));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->qp), key.klen >> 1, key.qp, key.klen >> 1));

    rsa_free_buffer(&key);
    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    rsa_free_buffer(&key);
    return ret;

#else
    hi_log_error("Unsupport rsa compute crt\n");
    return HI_FAILURE;
#endif /* end of HI_PRODUCT_RSA_SUPPORT */
}

static hi_s32 dispatch_rsa_gen_key(hi_void *argp)
{
#ifdef HI_PRODUCT_RSA_SUPPORT
    hi_s32 ret;
    rsa_info_t *rsa_info = argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    cryp_rsa_key key = { 0 };

    hi_log_func_enter();

    hi_log_check_param(rsa_info->klen > RSA_KEY_BITWIDTH_4096);

    rsa_info->public = HI_FALSE;
    ret = memset_s(&key, sizeof(key), 0, sizeof(key));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa_alloc_key failed\n");
        hi_log_print_func_err(rsa_alloc_buffer, ret);
        return ret;
    }

    check_exit(kapi_rsa_gen_key(key.klen * BITS_IN_BYTE, key.e, &key));

    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->n), key.klen, key.n, key.klen));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->d), key.klen, key.d, key.klen));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->p), key.klen >> 1, key.p, key.klen >> 1));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->q), key.klen >> 1, key.q, key.klen >> 1));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->dp), key.klen >> 1, key.dp, key.klen >> 1));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->dq), key.klen >> 1, key.dq, key.klen >> 1));
    check_exit(crypto_copy_to_user(ADDR_VIA(rsa_info->qp), key.klen >> 1, key.qp, key.klen >> 1));

    rsa_free_buffer(&key);
    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    rsa_free_buffer(&key);
    return ret;
#else
    hi_log_error("Unsupport rsa compute crt\n");
    return HI_ERR_CIPHER_UNSUPPORTED;
#endif
}

static hi_s32 dispatch_hdcp_encrypt(hi_void *argp)
{
    hi_s32 ret = HI_FAILURE;
    hdcp_key_t *hdcp_key = (hdcp_key_t *)argp;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    hi_u32 decrypt = SYMC_OPERATION_DECRYPT;

    hi_log_func_enter();

    hi_log_check_param(hdcp_key->attr.key_sel >= HDCP_KEY_SEL_COUNT);
    hi_log_check_param(hdcp_key->attr.ram_sel >= HDMI_RAM_SEL_COUNT);
    hi_log_check_param((hdcp_key->decrypt != HDCP_OPERATION_ENCRYPT)
                    && (hdcp_key->decrypt != HDCP_OPERATION_DECRYPT));
    hi_log_check_param(hdcp_key->attr.alg  != HI_CIPHER_ALG_AES);
    hi_log_check_param(hdcp_key->attr.mode != HI_CIPHER_WORK_MODE_GCM);
    hi_log_check_param(hdcp_key->len % AES_BLOCK_SIZE_IN_BYTE != 0x00);

    if (hdcp_key->decrypt == HDCP_OPERATION_ENCRYPT) {
        decrypt = SYMC_OPERATION_ENCRYPT;
    }

    in = crypto_calloc(1, hdcp_key->len);
    if (HI_NULL == in) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        hi_log_print_func_err(crypto_calloc, ret);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    check_exit(crypto_copy_from_user(in, hdcp_key->len, ADDR_VIA(hdcp_key->in), hdcp_key->len));

    if (HI_NULL != ADDR_VIA(hdcp_key->out)) {
        out = crypto_calloc(1, hdcp_key->len);
        if (HI_NULL == out) {
            hi_log_error("error, malloc buffer for hdcp key output failed\n");
            ret = HI_ERR_CIPHER_FAILED_MEM;
            goto exit__;
        }
    }

    check_exit(kapi_hdcp_encrypt(&hdcp_key->attr, in, out, hdcp_key->len, decrypt));
    if (HI_NULL != out) {
        check_exit(crypto_copy_to_user(ADDR_VIA(hdcp_key->out), hdcp_key->len, out, hdcp_key->len));
    }

    if (HI_NULL != in) {
        crypto_free(in);
        in = HI_NULL;
    }
    if (HI_NULL != out) {
        crypto_free(out);
        out = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (HI_NULL != in) {
        crypto_free(in);
        in = HI_NULL;
    }
    if (HI_NULL != out) {
        crypto_free(out);
        out = HI_NULL;
    }

    return ret;
}

static hi_s32 dispatch_trng_get_random(hi_void *argp)
{
    trng_t *trng = argp;
    hi_s32 ret;
    hi_u8 *randbyte = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(trng->size > MAX_MALLOC_BUF_SIZE);

    randbyte = crypto_malloc(trng->size);
    if (randbyte == HI_NULL) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    ret = kapi_trng_get_rand_byte(randbyte, trng->size, trng->timeout);
    if (ret != HI_SUCCESS) {
        goto exit__;
    }

    ret = crypto_copy_to_user(ADDR_VIA(trng->randnum), trng->size, randbyte, trng->size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_copy_to_user, ret);
        goto exit__;
    }

exit__:
    if (randbyte != HI_NULL) {
        crypto_free(randbyte);
        randbyte = HI_NULL;
    }

    hi_log_func_exit();
    return ret;
}

static hi_s32 dispatch_sm2_sign(hi_void *argp)
{
#ifdef HI_PRODUCT_SM2_SUPPORT

    hi_s32 ret;
    sm2_sign_t *sign = (sm2_sign_t *)argp;
    hi_u8 *user_id = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(sign->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param((sign->idlen + 1 > SM2_ID_MAX_LEN));

    user_id = crypto_calloc(1, sign->idlen + 1);
    if (user_id == HI_NULL) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    check_exit(crypto_copy_from_user(user_id, sign->idlen + 1, ADDR_VIA(sign->id), sign->idlen));
    sign->src = HASH_CHUNCK_SRC_USER;
    check_exit(kapi_sm2_sign(sign->d,
                             sign->px,
                             sign->py,
                             user_id,
                             sign->idlen,
                             ADDR_VIA(sign->msg),
                             sign->msglen,
                             sign->src,
                             sign->r,
                             sign->s));
    if (user_id != HI_NULL) {
        crypto_free(user_id);
        user_id = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (user_id != HI_NULL) {
        crypto_free(user_id);
        user_id = HI_NULL;
    }

    return ret;
#else
    hi_log_error("Unsupport SM2\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_sm2_verify(hi_void *argp)
{
#ifdef HI_PRODUCT_SM2_SUPPORT
    hi_s32 ret;
    sm2_verify_t *verify = (sm2_verify_t *)argp;
    hi_u8 *user_id = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(verify->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param((verify->idlen + 1 > SM2_ID_MAX_LEN));

    user_id = crypto_calloc(1, (verify->idlen + 1));
    if (user_id == HI_NULL) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    check_exit(crypto_copy_from_user(user_id, verify->idlen + 1, ADDR_VIA(verify->id), verify->idlen));
    verify->src = HASH_CHUNCK_SRC_USER;
    check_exit(kapi_sm2_verify(verify->px,
                               verify->py,
                               user_id,
                               verify->idlen,
                               ADDR_VIA(verify->msg),
                               verify->msglen,
                               verify->src,
                               verify->r,
                               verify->s));
    if (user_id != HI_NULL) {
        crypto_free(user_id);
        user_id = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (user_id != HI_NULL) {
        crypto_free(user_id);
        user_id = HI_NULL;
    }

    return ret;
#else
    hi_log_error("Unsupport SM2\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_sm2_encrypt(hi_void *argp)
{
#ifdef HI_PRODUCT_SM2_SUPPORT
    hi_s32 ret;
    sm2_encrypt_t *encrypt = (sm2_encrypt_t *)argp;
    hi_u8 *msg = HI_NULL;
    hi_u8 *enc = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(encrypt->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param(encrypt->keylen != SM2_LEN_IN_WROD);
    hi_log_check_param(encrypt->msglen > MAX_MALLOC_BUF_SIZE);

    msg = crypto_calloc(1, encrypt->msglen);
    if (msg == HI_NULL) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    enc = crypto_calloc(1, encrypt->msglen + SM2_ENCRYPT_PAD_LEN);
    if (enc == HI_NULL) {
        crypto_free(msg);
        msg = HI_NULL;
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    check_exit(crypto_copy_from_user(msg, encrypt->msglen, ADDR_VIA(encrypt->msg), encrypt->msglen));
    check_exit(kapi_sm2_encrypt(encrypt->px, encrypt->py, msg, encrypt->msglen, enc,
                                &encrypt->enclen));
    check_exit(crypto_copy_to_user(ADDR_VIA(encrypt->enc), encrypt->enclen, enc, encrypt->enclen));

    if (msg != HI_NULL) {
        crypto_free(msg);
        msg = HI_NULL;
    }
    if (enc != HI_NULL) {
        crypto_free(enc);
        enc = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (msg != HI_NULL) {
        crypto_free(msg);
        msg = HI_NULL;
    }
    if (enc != HI_NULL) {
        crypto_free(enc);
        enc = HI_NULL;
    }

    return ret;
#else
    hi_log_error("Unsupport SM2\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_sm2_decrypt(hi_void *argp)
{
#ifdef HI_PRODUCT_SM2_SUPPORT
    hi_s32 ret;
    sm2_decrypt_t *decrypt = (sm2_decrypt_t *)argp;
    hi_u8 *msg = HI_NULL;
    hi_u8 *enc = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(decrypt->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param(decrypt->keylen != SM2_LEN_IN_WROD);
    hi_log_check_param(ADDR_VIA(decrypt->msg) == HI_NULL);
    hi_log_check_param(ADDR_VIA(decrypt->enc) == HI_NULL);
    hi_log_check_param(decrypt->enclen < SM2_ENCRYPT_PAD_LEN);
    hi_log_check_param(decrypt->enclen > MAX_MALLOC_BUF_SIZE);

    msg = crypto_calloc(1, decrypt->enclen - SM2_ENCRYPT_PAD_LEN);
    if (msg == HI_NULL) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    enc = crypto_calloc(1, decrypt->enclen);
    if (enc == HI_NULL) {
        crypto_free(msg);
        msg = HI_NULL;
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    check_exit(crypto_copy_from_user(enc, decrypt->enclen, ADDR_VIA(decrypt->enc), decrypt->enclen));
    check_exit(kapi_sm2_decrypt(decrypt->d, enc, decrypt->enclen, msg, &decrypt->msglen));
    check_exit(crypto_copy_to_user(ADDR_VIA(decrypt->msg), decrypt->msglen, msg, decrypt->msglen));

    if (msg != HI_NULL) {
        crypto_free(msg);
        msg = HI_NULL;
    }
    if (enc != HI_NULL) {
        crypto_free(enc);
        enc = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    if (msg != HI_NULL) {
        crypto_free(msg);
        msg = HI_NULL;
    }
    if (enc != HI_NULL) {
        crypto_free(enc);
        enc = HI_NULL;
    }

    return ret;
#else
    hi_log_error("Unsupport SM2\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_sm2_gen_key(hi_void *argp)
{
#ifdef HI_PRODUCT_SM2_SUPPORT
    hi_s32 ret;
    sm2_key_t *key = (sm2_key_t *)argp;

    hi_log_func_enter();
    hi_log_check_param(key->keylen != SM2_LEN_IN_WROD);
    hi_log_check_param(key->magic_num != CRYPTO_MAGIC_NUM);

    ret = kapi_sm2_gen_key(key->d, key->px, key->py);

    hi_log_func_exit();

    return ret;
#else
    hi_log_error("Unsupport SM2\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_cenc_decrypt(hi_void *argp)
{
#if defined(HI_PRODUCT_CENC_SUPPORT)
    hi_s32 ret;
    cenc_info_t *cenc = argp;
    hi_cipher_cenc cipher_cenc;
    compat_addr in;
    compat_addr out;

    hi_log_func_enter();

    ADDR_U64(in) = crypto_bufhandle_to_phys(cenc->inphy);
    ADDR_U64(out) = crypto_bufhandle_to_phys(cenc->outphy);
    cipher_cenc.first_offset = cenc->firstoffset;
    cipher_cenc.odd_key = cenc->oddkey;
    cipher_cenc.subsample = (hi_cipher_subsample *)ADDR_VIA(cenc->subsample);
    cipher_cenc.subsample_num = cenc->subsample_num;
    ret = kapi_symc_cenc_decrypt(cenc->id, &cipher_cenc, in, out, cenc->length);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_cenc_decrypt, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
#else
    hi_log_error("Unsupport CENC\n");
    return HI_FAILURE;
#endif
}

static hi_s32 dispatch_ecc_alloc_param(ecc_info_t *info, ecc_param_t *ecc)
{
    hi_s32 ret;
    hi_u32 offset;

    hi_log_check_param(info->ksize > MAX_MALLOC_BUF_SIZE);

    hi_log_debug("ecc->ksize 0x%x\n", info->ksize);

    ecc->ksize = ALIGN(info->ksize, ECC_KLEN_ALIGN);
    ecc->ksize = cipher_max(ecc->ksize, ECC_KLEN_MIN);

    ecc->p = crypto_calloc(1, ecc->ksize * ECC_PARAM_CNT);
    if (ecc->p == HI_NULL) {
        hi_log_error("error, malloc for ecc->p failed\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    ecc->b = ecc->p + ecc->ksize;
    ecc->a = ecc->b + ecc->ksize;
    ecc->gx = ecc->a + ecc->ksize;
    ecc->gy = ecc->gx + ecc->ksize;
    ecc->n = ecc->gy + ecc->ksize;
    ecc->h = info->h;
    offset = ecc->ksize - info->ksize;

    check_exit(crypto_copy_from_user(ecc->p + offset, ecc->ksize, ADDR_VIA(info->p), info->ksize));
    check_exit(crypto_copy_from_user(ecc->b + offset, ecc->ksize, ADDR_VIA(info->b), info->ksize));
    check_exit(crypto_copy_from_user(ecc->a + offset, ecc->ksize, ADDR_VIA(info->a), info->ksize));
    check_exit(crypto_copy_from_user(ecc->gx + offset, ecc->ksize, ADDR_VIA(info->gx), info->ksize));
    check_exit(crypto_copy_from_user(ecc->gy + offset, ecc->ksize, ADDR_VIA(info->gy), info->ksize));
    check_exit(crypto_copy_from_user(ecc->n + offset, ecc->ksize, ADDR_VIA(info->n), info->ksize));

    return HI_SUCCESS;

exit__:
    if (ecc->p != HI_NULL) {
        crypto_free(ecc->p);
        ecc->p = HI_NULL;
    }

    return ret;
}

static hi_void dispatch_ecc_free_param(ecc_param_t *ecc)
{
    if (ecc->p != HI_NULL) {
        crypto_free(ecc->p);
    }

    ecc->p = HI_NULL;
    ecc->a = HI_NULL;
    ecc->b = HI_NULL;
    ecc->gx = HI_NULL;
    ecc->gy = HI_NULL;
    ecc->n = HI_NULL;
    return;
}

static hi_s32 dispatch_ecdh_compute_key(hi_void *argp)
{
    hi_s32 ret;
    ecc_info_t *info = argp;
    ecc_param_t ecc = { 0 };
    hi_u8 *buf = HI_NULL;
    hi_u8 *d = HI_NULL;
    hi_u8 *px = HI_NULL;
    hi_u8 *py = HI_NULL;
    hi_u8 *sharekey = HI_NULL;
    hi_u32 offset;

    hi_log_func_enter();
    hi_log_check_param(info->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param(info->mlen < info->ksize);

    ret = dispatch_ecc_alloc_param(info, &ecc);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, dispatch_ecc_alloc_param failed\n");
        hi_log_print_func_err(dispatch_ecc_alloc_param, ret);
        return ret;
    }

    buf = crypto_calloc(1, ecc.ksize * 4);     /* alloc 4 buffer for p, px, py, sharekey */
    if (buf == HI_NULL) {
        hi_log_error("error, malloc for key failed\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        goto exit__;
    }
    d = buf;
    px = d + ecc.ksize;
    py = px + ecc.ksize;
    sharekey = py + ecc.ksize;
    offset = ecc.ksize - info->ksize;

    check_exit(crypto_copy_from_user(d + offset, ecc.ksize, ADDR_VIA(info->d), info->ksize));
    check_exit(crypto_copy_from_user(px + offset, ecc.ksize, ADDR_VIA(info->px), info->ksize));
    check_exit(crypto_copy_from_user(py + offset, ecc.ksize, ADDR_VIA(info->py), info->ksize));
    check_exit(kapi_ecdh_compute_key(&ecc, d, px, py, sharekey));
    check_exit(crypto_copy_to_user(ADDR_VIA(info->msg), ecc.ksize, sharekey + offset, info->ksize));

    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }
    return ret;
}

static hi_s32 dispatch_ecc_gen_key(hi_void *argp)
{
    hi_s32 ret;
    ecc_info_t *info = argp;
    ecc_param_t ecc = { 0 };
    hi_u8 *buf = HI_NULL;
    hi_u8 *inkey = HI_NULL;
    hi_u8 *outkey = HI_NULL;
    hi_u8 *px = HI_NULL;
    hi_u8 *py = HI_NULL;
    hi_u32 offset;

    hi_log_func_enter();
    hi_log_check_param(info->magic_num != CRYPTO_MAGIC_NUM);

    ret = dispatch_ecc_alloc_param(info, &ecc);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, dispatch_ecc_alloc_param failed\n");
        hi_log_print_func_err(dispatch_ecc_alloc_param, ret);
        return ret;
    }

    buf = crypto_calloc(1, ecc.ksize * 4); /* alloc 4 buffer for px, py, inkey, outkey */
    if (buf == HI_NULL) {
        hi_log_error("error, malloc for key failed\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        goto exit__;
    }
    px = buf;
    py = px + ecc.ksize;
    inkey = py + ecc.ksize;
    outkey = inkey + ecc.ksize;
    offset = ecc.ksize - info->ksize;

    if (ADDR_VIA(info->msg) != HI_NULL) {
        check_exit(crypto_copy_from_user(inkey + offset, ecc.ksize, ADDR_VIA(info->msg), info->ksize));
    } else {
        inkey = HI_NULL;
    }

    check_exit(kapi_ecc_gen_key(&ecc, inkey, outkey, px, py));

    check_exit(crypto_copy_to_user(ADDR_VIA(info->d), ecc.ksize, outkey + offset, info->ksize));
    check_exit(crypto_copy_to_user(ADDR_VIA(info->px), ecc.ksize, px + offset, info->ksize));
    check_exit(crypto_copy_to_user(ADDR_VIA(info->py), ecc.ksize, py + offset, info->ksize));

    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }
    return ret;
}

static hi_s32 dispatch_ecdsa_sign_hash(hi_void *argp)
{
    hi_s32 ret;
    ecc_info_t *info = argp;
    ecc_param_t ecc = { 0 };
    hi_u8 *buf = HI_NULL;
    hi_u8 *d = HI_NULL;
    hi_u8 *hash = HI_NULL;
    hi_u8 *r = HI_NULL;
    hi_u8 *s = HI_NULL;
    hi_u32 offset;

    hi_log_func_enter();
    hi_log_check_param(info->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param(info->mlen > HASH_RESULT_MAX_SIZE);

    ret = dispatch_ecc_alloc_param(info, &ecc);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, dispatch_ecc_alloc_param failed\n");
        hi_log_print_func_err(dispatch_ecc_alloc_param, ret);
        return ret;
    }

    buf = crypto_calloc(1, ecc.ksize * 3 + info->mlen);   /* alloc 3 buffer for d, hash, r, s */
    if (buf == HI_NULL) {
        hi_log_error("error, malloc for buf failed\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        goto exit__;
    }
    d = buf;
    r = d + ecc.ksize;
    s = r + ecc.ksize;
    hash = s + ecc.ksize;
    offset = ecc.ksize - info->ksize;

    check_exit(crypto_copy_from_user(d + offset, ecc.ksize, ADDR_VIA(info->d), info->ksize));
    check_exit(crypto_copy_from_user(hash, info->mlen, ADDR_VIA(info->msg), info->mlen));

    check_exit(kapi_ecdsa_sign_hash(&ecc, d, hash, info->mlen, r, s));

    check_exit(crypto_copy_to_user(ADDR_VIA(info->r), ecc.ksize, r + offset, info->ksize));
    check_exit(crypto_copy_to_user(ADDR_VIA(info->s), ecc.ksize, s + offset, info->ksize));

    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }
    return ret;
}

static hi_s32 dispatch_ecdsa_verify_hash(hi_void *argp)
{
    hi_s32 ret;
    ecc_info_t *info = argp;
    ecc_param_t ecc = { 0 };
    hi_u8 *buf = HI_NULL;
    hi_u8 *px = HI_NULL;
    hi_u8 *py = HI_NULL;
    hi_u8 *hash = HI_NULL;
    hi_u8 *r = HI_NULL;
    hi_u8 *s = HI_NULL;
    hi_u32 offset;

    hi_log_func_enter();
    hi_log_check_param(info->magic_num != CRYPTO_MAGIC_NUM);
    hi_log_check_param(info->mlen > HASH_RESULT_MAX_SIZE);

    ret = dispatch_ecc_alloc_param(info, &ecc);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, dispatch_ecc_alloc_param failed\n");
        hi_log_print_func_err(dispatch_ecc_alloc_param, ret);
        return ret;
    }

    buf = crypto_calloc(1, ecc.ksize * 4 + info->mlen); /* alloc 4 buffer for px, py, hash, r, s */
    if (buf == HI_NULL) {
        hi_log_error("error, malloc for key failed\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        goto exit__;
    }
    px = buf;
    py = px + ecc.ksize;
    r = py + ecc.ksize;
    s = r + ecc.ksize;
    hash = s + ecc.ksize;
    offset = ecc.ksize - info->ksize;

    check_exit(crypto_copy_from_user(px + offset, ecc.ksize, ADDR_VIA(info->px), info->ksize));
    check_exit(crypto_copy_from_user(py + offset, ecc.ksize, ADDR_VIA(info->py), info->ksize));
    check_exit(crypto_copy_from_user(hash, info->mlen, ADDR_VIA(info->msg), info->mlen));
    check_exit(crypto_copy_from_user(r + offset, ecc.ksize, ADDR_VIA(info->r), info->ksize));
    check_exit(crypto_copy_from_user(s + offset, ecc.ksize, ADDR_VIA(info->s), info->ksize));

    check_exit(kapi_ecdsa_verify_hash(&ecc, px, py, hash, info->mlen, r, s));
    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:
    dispatch_ecc_free_param(&ecc);
    if (buf != HI_NULL) {
        crypto_free(buf);
        buf = HI_NULL;
    }
    return ret;
}

static crypto_dispatch_func dispatch_func[CRYPTO_CMD_COUNT] = {
    { "CreateHandle", dispatch_symc_create_handle,   CRYPTO_CMD_SYMC_CREATEHANDLE },
    { "DestroyHandle", dispatch_symc_destroy_handle, CRYPTO_CMD_SYMC_DESTROYHANDLE },
    { "ConfigChn", dispatch_symc_config,         CRYPTO_CMD_SYMC_CONFIGHANDLE },
    { "Encrypt", dispatch_symc_encrypt,          CRYPTO_CMD_SYMC_ENCRYPT },
    { "EncryptMulti", dispatch_symc_encrypt_multi,  CRYPTO_CMD_SYMC_ENCRYPTMULTI },
    { "CMAC", dispatch_symc_camc,                CRYPTO_CMD_SYMC_CMAC },
    { "GetTag", dispatch_symc_get_tag,           CRYPTO_CMD_SYMC_GETTAG },
    { "CencDerypt", dispatch_cenc_decrypt,       CRYPTO_CMD_CENC },
    { "HashStart", dispatch_hash_start,          CRYPTO_CMD_HASH_START },
    { "HashUpdate", dispatch_hash_update,        CRYPTO_CMD_HASH_UPDATE },
    { "HashFinish", dispatch_hash_finish,        CRYPTO_CMD_HASH_FINISH },
    { "RsaEncrypt", dispatch_rsa_encrypt,        CRYPTO_CMD_RSA_ENC },
    { "RsaDecrypt", dispatch_rsa_decrypt,        CRYPTO_CMD_RSA_DEC },
    { "RsaSign", dispatch_rsa_sign_hash,         CRYPTO_CMD_RSA_SIGN },
    { "RsaVerify", dispatch_rsa_verify_hash,     CRYPTO_CMD_RSA_VERIFY },
    { "RsaComputeCrt", dispatch_rsa_compute_crt, CRYPTO_CMD_RSA_COMPUTE_CRT },
    { "RsaGenKey", dispatch_rsa_gen_key,         CRYPTO_CMD_RSA_GEN_KEY },
    { "TRNG", dispatch_trng_get_random,          CRYPTO_CMD_TRNG },
    { "Sm2Sign", dispatch_sm2_sign,              CRYPTO_CMD_SM2_SIGN },
    { "Sm2Verify", dispatch_sm2_verify,          CRYPTO_CMD_SM2_VERIFY },
    { "Sm2Encrypt", dispatch_sm2_encrypt,        CRYPTO_CMD_SM2_ENCRYPT },
    { "Sm2Decrypt", dispatch_sm2_decrypt,        CRYPTO_CMD_SM2_DECRYPT },
    { "Sm2GenKey", dispatch_sm2_gen_key,         CRYPTO_CMD_SM2_GEN_KEY },
    { "EcdhComputeKey", dispatch_ecdh_compute_key, CRYPTO_CMD_ECDH_COMPUTE_KEY },
    { "EcdhGenKey", dispatch_ecc_gen_key,          CRYPTO_CMD_ECC_GEN_KEY },
    { "EcdsaSign", dispatch_ecdsa_sign_hash,       CRYPTO_CMD_ECDSA_SIGN },
    { "EcdsaVerify", dispatch_ecdsa_verify_hash,   CRYPTO_CMD_ECDSA_VERIFY },
    { "GetSymcConfig", dispatch_symc_get_config,   CRYPTO_CMD_SYMC_GET_CONFIG },
    { "GetKeySlot", dispatch_symc_get_keyslot,    CRYPTO_CMD_SYMC_GET_KEYSLOT},
    { "HdcpEncrypt", dispatch_hdcp_encrypt,       CRYPTO_CMD_HDCP_ENCRYPT},
};

hi_s32 crypto_ioctl(hi_u32 cmd, hi_void *argp)
{
    hi_u32 nr;
    hi_s32 ret;

    hi_log_func_enter();

    nr = CRYPTO_IOC_NR(cmd);

    hi_log_check_param(argp == HI_NULL);
    hi_log_check_param(nr >= CRYPTO_CMD_COUNT);
    hi_log_check_param(dispatch_func[nr].cmd != cmd);
    hi_log_check_param(dispatch_func[nr].func == HI_NULL);

    hi_log_debug("cmd 0x%x, nr %d, size %d, local cmd 0x%x\n",
                 cmd, nr, CRYPTO_IOC_SIZE(cmd), dispatch_func[nr].cmd);

    hi_log_debug("Link Func NR %d, Name:  %s\n", nr, dispatch_func[nr].name);

    ret = dispatch_func[nr].func(argp);
    if (ret != HI_SUCCESS) {
        /* TRNG may be empty in FIFO, don't report error, try to read it again */
        if (cmd != CRYPTO_CMD_TRNG) {
            hi_log_error("error, call dispatch_fun fun failed!\n");
            hi_log_print_func_err(crypto_dispatch_func, ret);
        }
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 crypto_entry(hi_void)
{
    hi_s32 ret;

    hi_log_func_enter();

    crypto_mem_init();

    ret = module_addr_map();
    if (ret != HI_SUCCESS) {
        hi_log_error("module addr map failed\n");
        hi_log_print_func_err(module_addr_map, ret);
        return ret;
    }

    ret = kapi_trng_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("kapi trng init failed\n");
        hi_log_print_func_err(kapi_trng_init, ret);
        goto error;
    }

    ret = kapi_symc_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("kapi symc init failed\n");
        hi_log_print_func_err(kapi_symc_init, ret);
        goto error2;
    }

    ret = kapi_hash_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("kapi hash init failed\n");
        hi_log_print_func_err(kapi_hash_init, ret);
        goto error1;
    }

#ifdef HI_PRODUCT_RSA_SUPPORT
    ret = kapi_rsa_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("kapi rsa init failed\n");
        hi_log_print_func_err(kapi_hash_init, ret);
        kapi_hash_deinit();
        goto error1;
    }
#endif

#ifdef HI_PRODUCT_SM2_SUPPORT
    ret = kapi_sm2_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("kapi sm2 init failed\n");
        hi_log_print_func_err(kapi_sm2_init, ret);
        kapi_rsa_deinit();
        kapi_hash_deinit();
        goto error1;
    }
#endif

#ifdef HI_PRODUCT_ECC_SUPPORT
    ret = kapi_ecc_init();
    if (ret != HI_SUCCESS) {
        hi_log_error("kapi sm2 init failed\n");
        hi_log_print_func_err(kapi_ecc_init, ret);
        kapi_rsa_deinit();
        kapi_hash_deinit();
        kapi_sm2_deinit();
        goto error1;
    }
#endif

    hi_log_func_exit();
    return HI_SUCCESS;

error1:
    kapi_symc_deinit();
error2:
    kapi_trng_deinit();
error:
    module_addr_unmap();

    return ret;
}

hi_s32 crypto_exit(hi_void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = kapi_symc_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_deinit, ret);
        return ret;
    }

    ret = kapi_hash_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_hash_deinit, ret);
        return ret;
    }

#ifdef HI_PRODUCT_RSA_SUPPORT
    ret = kapi_rsa_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_rsa_deinit, ret);
        return ret;
    }
#endif

#ifdef HI_PRODUCT_SM2_SUPPORT
    ret = kapi_sm2_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_sm2_deinit, ret);
        return ret;
    }
#endif

#ifdef HI_PRODUCT_ECC_SUPPORT
    ret = kapi_ecc_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_ecc_deinit, ret);
        return ret;
    }
#endif

    ret = kapi_trng_deinit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_trng_deinit, ret);
        return ret;
    }

    ret = module_addr_unmap();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(module_addr_unmap, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 crypto_release(hi_void)
{
    hi_s32 ret;

    ret = kapi_symc_release();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_symc_release, ret);
        return ret;
    }

    ret = kapi_hash_release();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(kapi_hash_release, ret);
        return ret;
    }

    return HI_SUCCESS;
}

/** @} */ /** <!-- ==== Structure Definition end ==== */


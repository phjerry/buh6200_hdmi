/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_rsa
 * Author: zhaoguihong
 * Create: 2019-06-18
 */

#ifdef HI_PRODUCT_RSA_SUPPORT

#include "drv_osal_lib.h"
#include "drv_pke.h"
#include "cryp_rsa.h"
#include "cryp_trng.h"
#include "ext_alg.h"
#include "mbedtls/rsa.h"
#include "mbedtls/rsa_internal.h"

/********************** Internal Structure Definition ************************/
/** \addtogroup      rsa */
/** @{ */ /** <!-- [rsa] */

#define RSA_PKCS1_TYPE_MIN_PAD_LEN (11)
#define RSA_BITS_1024              1024
#define RSA_BITS_2048              2048
#define RSA_BITS_3072              3072
#define RSA_BITS_4096              4096
#define BYTE_BITS                  (8)

/* ! rsa mutex */
static crypto_mutex g_rsa_mutex;

#define KAPI_RSA_LOCK()                  \
    do { \
        ret = crypto_mutex_lock(&g_rsa_mutex); \
        if (ret != HI_SUCCESS) {             \
            hi_log_error("error, rsa lock failed\n");    \
            hi_log_func_exit();               \
        } \
    } while (0)

#define KAPI_RSA_UNLOCK() crypto_mutex_unlock(&g_rsa_mutex)

/* ! \rsa rsa soft function */
int mbedtls_mpi_exp_mod_sw(mbedtls_mpi *X, const mbedtls_mpi *A,
                           const mbedtls_mpi *E, const mbedtls_mpi *N,
                           mbedtls_mpi *_RR);

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      rsa drivers */
/** @{ */ /** <!-- [rsa] */

void mbedtls_mpi_print(const mbedtls_mpi *X, const char *name)
{
#ifdef HI_CIPHER_DEBUG
    int ret;
    size_t n;
    hi_u8 buf[512] = { 0 }; /* 512 max buf */

    n = mbedtls_mpi_size(X);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(X, buf, n));
    hi_print_hex(name, (hi_u8 *)buf, n);

cleanup:
    return;
#endif
}

#ifdef CHIP_RSA_SUPPORT
static hi_s32 rsa_get_klen(unsigned long module_len, hi_u32 *keylen, rsa_key_width *width)
{
    if (module_len <= RSA_KEY_LEN_1024) {
        *keylen = RSA_KEY_LEN_1024;
        *width = RSA_KEY_WIDTH_1024;
    } else if (module_len <= RSA_KEY_LEN_2048) {
        *keylen = RSA_KEY_LEN_2048;
        *width = RSA_KEY_WIDTH_2048;
    } else if (module_len <= RSA_KEY_LEN_4096) {
        *keylen = RSA_KEY_LEN_4096;
        *width = RSA_KEY_WIDTH_4096;
    } else {
        hi_log_error("error, invalid key len %ld\n", module_len);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_s32 cryp_check_data(hi_u8 *N, hi_u8 *E, hi_u8 *MC, hi_u32 len)
{
    hi_u32 i;

    /* MC > 0 */
    for (i = 0; i < len; i++) {
        if (MC[i] > 0) {
            break;
        }
    }
    if (i >= len) {
        hi_log_error("RSA M/C is zero, error!\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* MC < N */
    for (i = 0; i < len; i++) {
        if (MC[i] < N[i]) {
            break;
        }
    }
    if (i >= len) {
        hi_log_error("RSA M/C is larger than N, error!\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* E >= 1 */
    for (i = 0; i < len; i++) {
        if (E[i] > 0) {
            break;
        }
    }
    if (i >= len) {
        hi_log_error("RSA D/E is zero, error!\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* N is Odd ? */
    if (((N[len - 1] & 0x01) == 0x00)) {
        hi_log_error("RSA N is odd, error!\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static int cryp_ifep_rsa_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A,
                                 const mbedtls_mpi *E, const mbedtls_mpi *N,
                                 mbedtls_mpi *_RR)
{
    hi_u32 module_len;
    hi_u8 *buf = HI_NULL;
    hi_u8 *n = HI_NULL;
    hi_u8 *k = HI_NULL;
    hi_u8 *in = HI_NULL;
    hi_u8 *out = HI_NULL;
    hi_u32 keylen = 0;
    rsa_key_width width = 0;
    mbedtls_mpi _A;
    hi_s32 ret;

    hi_log_func_enter();

    /* computes valid bits of N */
    module_len = mbedtls_mpi_size(N);

    ret = rsa_get_klen(module_len, &keylen, &width);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(rsa_get_klen, ret);
        return ret;
    }

    buf = crypto_malloc(keylen * 4); /* mallc 4 buf to store n || k(e or d) || in || out */
    if (buf == HI_NULL) {
        hi_log_print_func_err(crypto_malloc, HI_ERR_CIPHER_FAILED_MEM);
        return HI_ERR_CIPHER_FAILED_MEM;
    }

    /* zero */
    ret = memset_s(buf, keylen * 4, 0, keylen * 4);  /* clear 4 buf */
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        crypto_free(buf);
        buf = HI_NULL;
        return ret;
    }

    n = buf;
    k = n + keylen;
    in = k + keylen;
    out = in + keylen;

    mbedtls_mpi_init(&_A);
    check_exit(mbedtls_mpi_mod_mpi(&_A, A, N));

    /* read A, E, N */
    check_exit(mbedtls_mpi_write_binary(&_A, in, keylen));
    check_exit(mbedtls_mpi_write_binary(E, k, keylen));
    check_exit(mbedtls_mpi_write_binary(N, n, keylen));

    /* key and data valid ? */
    check_exit(cryp_check_data(n, k, in, keylen));

    /* out = in ^ k mod n */
    ret = drv_ifep_rsa_exp_mod(n, k, in, out, width);
    if (ret == HI_SUCCESS) {
        /* write d */
        mbedtls_mpi_read_binary(X, out, keylen);
    }
exit__:

    mbedtls_mpi_free(&_A);
    crypto_free(buf);
    buf = HI_NULL;

    hi_log_func_exit();

    return ret;
}
#endif

int mbedtls_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A,
                        const mbedtls_mpi *E,
                        const mbedtls_mpi *N,
                        mbedtls_mpi *_RR)
{
    hi_s32 ret;

    hi_log_func_enter();

    KAPI_RSA_LOCK();

    hi_log_check_param(X == HI_NULL);
    hi_log_check_param(A == HI_NULL);
    hi_log_check_param(E == HI_NULL);
    hi_log_check_param(N == HI_NULL);

    mbedtls_mpi_print(A, "M");
    mbedtls_mpi_print(E, "E");
    mbedtls_mpi_print(N, "N");

#if defined(CHIP_RSA_SUPPORT)
    ret = cryp_ifep_rsa_exp_mod(X, A, E, N, _RR);
#else
    ret = mbedtls_mpi_exp_mod_sw(X, A, E, N, _RR);
#endif
    mbedtls_mpi_print(X, "X");

    if (ret != HI_SUCCESS) {
        hi_log_error("rsa mpi_exp_mod failed, ret = 0x%x\n", ret);
    }

    KAPI_RSA_UNLOCK();

    hi_log_func_exit();

    return ret;
}

int cryp_rsa_init(void)
{
    hi_log_func_enter();

    crypto_mutex_init(&g_rsa_mutex);

#if defined(CHIP_IFEP_RSA_VER_V100)
    {
        hi_s32 ret;

        ret = drv_rsa_init();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(drv_rsa_init, ret);
            return ret;
        }
    }
#endif

    hi_log_func_exit();
    return HI_SUCCESS;
}

void cryp_rsa_deinit(void)
{
#if defined(CHIP_IFEP_RSA_VER_V100)
    rsa_capacity capacity;

    drv_ifep_rsa_get_capacity(&capacity);

    /* recovery the rsa function of mbedtls */
    if (capacity.rsa) {
        drv_rsa_deinit();
    }
#endif

    crypto_mutex_destroy(&g_rsa_mutex);
}

int mbedtls_get_random(void *param, hi_u8 *rand, size_t size)
{
    hi_u32 i;
    hi_u32 randnum = 0;

    for (i = 0; i < size;) {
        cryp_trng_get_random(&randnum, -1);
        rand[i++] = (hi_u8)(randnum) & 0xFF;
        rand[i++] = (hi_u8)(randnum >> 8) & 0xFF;  /* bit 8..0 */
        rand[i++] = (hi_u8)(randnum >> 16) & 0xFF; /* bit 16..15 */
        rand[i++] = (hi_u8)(randnum >> 24) & 0xFF; /* bit 31..24 */
    }

    return HI_SUCCESS;
}

static hi_s32 cryp_rsa_init_key(cryp_rsa_key *key, hi_u32 *mode, mbedtls_rsa_context *rsa)
{
    hi_s32 ret;

    hi_log_func_enter();

    mbedtls_mpi_init(&rsa->N);
    mbedtls_mpi_init(&rsa->E);
    mbedtls_mpi_init(&rsa->D);
    mbedtls_mpi_init(&rsa->P);
    mbedtls_mpi_init(&rsa->Q);
    mbedtls_mpi_init(&rsa->DP);
    mbedtls_mpi_init(&rsa->DQ);
    mbedtls_mpi_init(&rsa->QP);

    check_exit(mbedtls_mpi_read_binary(&rsa->N, key->n, key->klen));
    rsa->len = key->klen;
    if ((rsa->len < RSA_MIN_KEY_LEN) || (rsa->len > RSA_MAX_KEY_LEN)) {
        hi_log_error("RSA invalid keylen: 0x%x!\n", rsa->len);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        ret = HI_ERR_CIPHER_INVALID_PARA;
        goto exit__;
    }

    if (key->public) {
        check_exit(mbedtls_mpi_read_binary(&rsa->E, (hi_u8 *)&key->e, WORD_WIDTH));
        *mode = MBEDTLS_RSA_PUBLIC;
    } else {
        if (key->d != HI_NULL) { /* Non CRT */
            check_exit(mbedtls_mpi_read_binary(&rsa->D, key->d, key->klen));
            *mode = MBEDTLS_RSA_PRIVATE;
        } else { /* CRT */
            check_exit(mbedtls_mpi_read_binary(&rsa->P, key->p, key->klen >> 1));
            check_exit(mbedtls_mpi_read_binary(&rsa->Q, key->q, key->klen >> 1));
            check_exit(mbedtls_mpi_read_binary(&rsa->DP, key->dp, key->klen >> 1));
            check_exit(mbedtls_mpi_read_binary(&rsa->DQ, key->dq, key->klen >> 1));
            check_exit(mbedtls_mpi_read_binary(&rsa->QP, key->qp, key->klen >> 1));
            *mode = MBEDTLS_RSA_PRIVATE;
        }
    }

    hi_log_debug("mode %d, e 0x%x\n", *mode, key->e);

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:

    mbedtls_mpi_free(&rsa->N);
    mbedtls_mpi_free(&rsa->E);
    mbedtls_mpi_free(&rsa->D);
    mbedtls_mpi_free(&rsa->P);
    mbedtls_mpi_free(&rsa->Q);
    mbedtls_mpi_free(&rsa->DP);
    mbedtls_mpi_free(&rsa->DQ);
    mbedtls_mpi_free(&rsa->QP);

    hi_log_func_exit();

    return ret;
}

static void cryp_rsa_deinit_key(mbedtls_rsa_context *rsa)
{
    hi_log_func_enter();

    mbedtls_mpi_free(&rsa->N);
    mbedtls_mpi_free(&rsa->E);
    mbedtls_mpi_free(&rsa->D);
    mbedtls_mpi_free(&rsa->P);
    mbedtls_mpi_free(&rsa->Q);
    mbedtls_mpi_free(&rsa->DP);
    mbedtls_mpi_free(&rsa->DQ);
    mbedtls_mpi_free(&rsa->QP);

    hi_log_func_exit();
}

static hi_s32 cryp_rsa_enc_get_alg(hi_u32 scheme, int *padding, int *hash_id, int *hashlen)
{
    /* RSA padding mode for enc scheme */
    switch (scheme) {
        case HI_CIPHER_RSA_ENC_SCHEME_NO_PADDING:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2: {
            *padding = 0x00;
            *hash_id = 0;
            *hashlen = 0;
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5: {
            *padding = MBEDTLS_RSA_PKCS_V15;
            *hash_id = 0;
            *hashlen = 0;
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA1;
            *hashlen = SHA1_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA224: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA224;
            *hashlen = SHA224_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA256;
            *hashlen = SHA256_RESULT_SIZE;
            break;
        }
#ifdef HI_PRODUCT_SHA512_SUPPORT
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA384: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA384;
            *hashlen = SHA384_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA512: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA512;
            *hashlen = SHA512_RESULT_SIZE;
            break;
        }
#endif
        default: {
            hi_log_error("RSA padding mode for enc is error, mode = 0x%x.\n", scheme);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    hi_log_debug("padding %d, hash_id %d, hashlen %d\n", *padding, *hash_id, *hashlen);
    return HI_SUCCESS;
}

static hi_s32 cryp_rsa_sign_get_alg(hi_u32 scheme, int *padding, int *hash_id, int *hashlen)
{
    /* RSA padding mode for sign scheme */
    switch (scheme) {
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1: {
            *padding = MBEDTLS_RSA_PKCS_V15;
            *hash_id = MBEDTLS_MD_SHA1;
            *hashlen = SHA1_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224: {
            *padding = MBEDTLS_RSA_PKCS_V15;
            *hash_id = MBEDTLS_MD_SHA224;
            *hashlen = SHA224_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256: {
            *padding = MBEDTLS_RSA_PKCS_V15;
            *hash_id = MBEDTLS_MD_SHA256;
            *hashlen = SHA256_RESULT_SIZE;
            break;
        }
#ifdef HI_PRODUCT_SHA512_SUPPORT
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384: {
            *padding = MBEDTLS_RSA_PKCS_V15;
            *hash_id = MBEDTLS_MD_SHA384;
            *hashlen = SHA384_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512: {
            *padding = MBEDTLS_RSA_PKCS_V15;
            *hash_id = MBEDTLS_MD_SHA512;
            *hashlen = SHA512_RESULT_SIZE;
            break;
        }
#endif
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA1;
            *hashlen = SHA1_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA224;
            *hashlen = SHA224_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA256;
            *hashlen = SHA256_RESULT_SIZE;
            break;
        }
#ifdef HI_PRODUCT_SHA512_SUPPORT
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA384;
            *hashlen = SHA384_RESULT_SIZE;
            break;
        }
        case HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512: {
            *padding = MBEDTLS_RSA_PKCS_V21;
            *hash_id = MBEDTLS_MD_SHA512;
            *hashlen = SHA512_RESULT_SIZE;
            break;
        }
#endif
        default: {
            hi_log_error("RSA padding mode error, mode = 0x%x.\n", scheme);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    hi_log_debug("padding %d, hash_id %d, hashlen %d\n", *padding, *hash_id, *hashlen);

    return HI_SUCCESS;
}

/* PKCS #1: block type 0,1,2 message padding */
static hi_s32 rsa_padding_add_pkcs1_type(mbedtls_rsa_context *rsa, hi_u32 mode, hi_u32 klen,
                                         hi_u8 bt, hi_u8 *in, hi_u32 inlen, hi_u8 *out)
{
    hi_s32 ret;
    hi_u32 plen;
    hi_u8 *peb = HI_NULL;
    hi_u32 i = 0;

    hi_log_func_enter();

    if (inlen > klen - RSA_PKCS1_TYPE_MIN_PAD_LEN) {
        hi_log_error("klen is invalid.\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    peb = out;

    /* first byte is 0x00 */
    *(peb++) = 0;

    /* Private Key BT (Block Type) */
    *(peb++) = bt;

    /* The padding string PS shall consist of k-3-||D|| octets */
    plen = klen - 3 - inlen; /* skip 3 byte */
    switch (bt) {
        case 0x00: {
            /* For block type 00, the octets shall have value 00 */
            ret = memset_s(peb, plen, 0x00, plen);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(memset_s, ret);
                return ret;
            }
            break;
        }
        case 0x01: {
            /* for block type 01, they shall have value FF */
            ret = memset_s(peb, plen, 0xFF, plen);
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(memset_s, ret);
                return ret;
            }
            break;
        }
        case 0x02: {
            /* for block type 02, they shall be pseudorandomly generated and nonzero. */
            (hi_void)mbedtls_get_random(HI_NULL, peb, plen);

            /* make sure nonzero */
            for (i = 0; i < plen; i++) {
                if (peb[i] == 0x00) {
                    peb[i] = 0x01;
                }
            }
            break;
        }
        default: {
            hi_log_error("BT(0x%x) is invalid.\n", plen);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    /* skip the padding string */
    peb += plen;

    /* set 0x00 follow PS */
    *(peb++) = 0x00;

    /* input data */
    ret = memcpy_s(peb, inlen, in, inlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    if (mode == MBEDTLS_RSA_PUBLIC) {
        ret = mbedtls_rsa_public(rsa, out, out);
        if (ret != HI_SUCCESS) {
            hi_log_error("rsa public failed.\n");
            hi_log_print_func_err(mbedtls_rsa_public, ret);
            return ret;
        }
    } else {
        ret = mbedtls_rsa_private(rsa, HI_NULL, 0, out, out);
        if (ret != HI_SUCCESS) {
            hi_log_error("rsa private failed.\n");
            hi_log_print_func_err(mbedtls_rsa_private, ret);
            return ret;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* PKCS #1: block type 0,1,2 message padding */
static hi_s32 rsa_padding_check_pkcs1_type(mbedtls_rsa_context *rsa, hi_u32 klen, hi_u32 mode,
                                           hi_u8 bt, hi_u8 *in, hi_u32 inlen,
                                           hi_u8 *out, hi_u32 *outlen)
{
    hi_s32 ret;
    hi_u8 *peb = HI_NULL;

    hi_log_func_enter();

    if (mode == MBEDTLS_RSA_PUBLIC) {
        ret = mbedtls_rsa_public(rsa, in, in);
        if (ret != HI_SUCCESS) {
            hi_log_error("rsa public failed.\n");
            hi_log_print_func_err(mbedtls_rsa_public, ret);
            return ret;
        }
    } else {
        ret = mbedtls_rsa_private(rsa, HI_NULL, 0, in, in);
        if (ret != HI_SUCCESS) {
            hi_log_error("rsa private failed.\n");
            hi_log_print_func_err(mbedtls_rsa_private, ret);
            return ret;
        }
    }

    *outlen = 0x00;
    peb = in;

    /* first byte must be 0x00 */
    if (*peb != 0x00) {
        hi_log_error("EB[0] != 0x00.\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    peb++;

    /* Private Key BT (Block Type) */
    if (*peb != bt) {
        hi_log_error("EB[1] != BT(0x%x).\n", bt);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    peb++;

    switch (bt) {
        case 0x00:
            /* For block type 00, the octets shall have value 00 */
            for (; peb < in + inlen - 1; peb++) {
                if ((*peb == 0x00) && (*(peb + 1) != 0)) {
                    break;
                }
            }
            break;
        case 0x01:
            /* For block type 01, the octets shall have value FF */
            for (; peb < in + inlen - 1; peb++) {
                if (*peb == 0xFF) {
                    continue;
                } else if (*peb == 0x00) {
                    break;
                } else {
                    peb = in + inlen - 1;
                    break;
                }
            }
            break;
        case 0x02:
            /* for block type 02, they shall be pseudorandomly generated and nonzero. */
            for (; peb < in + inlen - 1; peb++) {
                if (*peb == 0x00) {
                    break;
                }
            }
            break;
        default:
            hi_log_error("BT(0x%x) is invalid.\n", bt);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            return HI_ERR_CIPHER_INVALID_PARA;
    }

    if (peb >= (in + inlen - 1)) {
        hi_log_error("PS Error.\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* skip 0x00 after PS */
    peb++;

    /* get payload data */
    *outlen = in + klen - peb;
    ret = memcpy_s(out, klen, peb, *outlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 rsa_no_padding(mbedtls_rsa_context *rsa, hi_u32 klen, hi_u32 mode,
                             hi_u8 *in, hi_u32 inlen, hi_u8 *out)
{
    hi_s32 ret;
    hi_u8 data[RSA_MAX_KEY_LEN] = { 0 };

    hi_log_func_enter();

    if (inlen > klen) {
        hi_log_error("input length %d invalid.\n", inlen);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    ret = memcpy_s(data + klen - inlen, RSA_MAX_KEY_LEN, in, inlen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    if (mode == MBEDTLS_RSA_PUBLIC) {
        ret = mbedtls_rsa_public(rsa, data, out);
        if (ret != HI_SUCCESS) {
            hi_log_error("rsa public failed.\n");
            hi_log_print_func_err(mbedtls_rsa_public, ret);
            return ret;
        }
    } else {
        ret = mbedtls_rsa_private(rsa, HI_NULL, 0, data, out);
        if (ret != HI_SUCCESS) {
            hi_log_error("rsa private failed, ret = %d.\n", ret);
            hi_log_print_func_err(mbedtls_rsa_private, ret);
            return HI_ERR_CIPHER_INVALID_PARA;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 cryp_rsa_encrypt(cryp_rsa_key *key, hi_cipher_rsa_enc_scheme scheme,
                        hi_u8 *in, hi_u32 inlen, hi_u8 *out, hi_u32 *outlen)
{
    hi_s32 ret;
    hi_u32 mode = 0;
    int padding = 0;
    int hash_id = 0;
    int hashlen = 0;
    hi_u32 bt = 0;
    mbedtls_rsa_context rsa;

    hi_log_func_enter();

    hi_log_check_param(key == HI_NULL);
    hi_log_check_param(in == HI_NULL);
    hi_log_check_param(out == HI_NULL);
    hi_log_check_param(outlen == HI_NULL);
    hi_log_check_param(key->klen > RSA_KEY_LEN_4096);
    hi_log_check_param(inlen > key->klen);

    ret = cryp_rsa_enc_get_alg(scheme, &padding, &hash_id, &hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_enc_get_alg failed.\n");
        hi_log_print_err_code(ret);
        return ret;
    }

    mbedtls_rsa_init(&rsa, padding, hash_id);

    ret = cryp_rsa_init_key(key, &mode, &rsa);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_init_key failed.\n");
        hi_log_print_err_code(ret);
        return ret;
    }

    switch (scheme) {
        case HI_CIPHER_RSA_ENC_SCHEME_NO_PADDING: {
            ret = rsa_no_padding(&rsa, key->klen, mode, in, inlen, out);
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2: {
            bt = scheme - HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0;
            ret = rsa_padding_add_pkcs1_type(&rsa, mode, key->klen, bt, in, inlen, out);
            if (ret != HI_SUCCESS) {
                hi_log_error("error, rsa add pkcs1_type failed, ret = %d.\n", ret);
                hi_log_print_func_err(rsa_padding_add_pkcs1_type, ret);
                ret = HI_ERR_CIPHER_FAILED_ENCRYPT;
            }
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1:
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA224:
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256:
#ifdef HI_PRODUCT_SHA512_SUPPORT
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA384:
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA512:
#endif
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5: {
            ret = mbedtls_rsa_pkcs1_encrypt(&rsa, mbedtls_get_random,
                                            HI_NULL, mode, inlen, in, out);
            if (ret != HI_SUCCESS) {
                hi_log_error("error, rsa pkcs1 encrypt failed, ret = %d", ret);
                hi_log_print_func_err(rsa_padding_add_pkcs1_type, ret);
                ret = HI_ERR_CIPHER_FAILED_ENCRYPT;
            }
            break;
        }
        default: {
            hi_log_error("RSA padding mode error, mode = 0x%x.\n", scheme);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            ret = HI_ERR_CIPHER_INVALID_PARA;
            break;
        }
    }

    if (ret == HI_SUCCESS) {
        *outlen = key->klen;
    }

    cryp_rsa_deinit_key(&rsa);

    if (ret != HI_SUCCESS) {
        hi_log_error("rsa encrypt failed, scheme %d, ret = 0x%x\n", scheme, ret);
    }

    hi_log_func_exit();

    return ret;
}

hi_s32 cryp_rsa_decrypt(cryp_rsa_key *key, hi_cipher_rsa_enc_scheme scheme,
                        hi_u8 *in, hi_u32 inlen, hi_u8 *out, hi_u32 *outlen)
{
    hi_s32 ret;
    hi_u32 mode = 0;
    int padding = 0;
    int hash_id = 0;
    int hashlen = 0;
    hi_u32 bt = 0;
    size_t outsize = 0;
    mbedtls_rsa_context rsa;

    hi_log_func_enter();

    hi_log_check_param(key == HI_NULL);
    hi_log_check_param(in == HI_NULL);
    hi_log_check_param(out == HI_NULL);
    hi_log_check_param(outlen == HI_NULL);
    hi_log_check_param(key->klen > RSA_KEY_LEN_4096);
    hi_log_check_param(inlen != key->klen);

    hi_log_info("[RSA] %s decrypt, scheme %d\n", key->public ? "public" : "private", scheme);
    ret = cryp_rsa_enc_get_alg(scheme, &padding, &hash_id, &hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_enc_get_alg failed");
        hi_log_print_func_err(cryp_rsa_enc_get_alg, ret);
        return ret;
    }

    mbedtls_rsa_init(&rsa, padding, hash_id);

    ret = cryp_rsa_init_key(key, &mode, &rsa);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_init_key failed");
        hi_log_print_func_err(cryp_rsa_init_key, ret);
        return ret;
    }

    switch (scheme) {
        case HI_CIPHER_RSA_ENC_SCHEME_NO_PADDING: {
            ret = rsa_no_padding(&rsa, key->klen, mode, in, inlen, out);
            *outlen = key->klen;
            if (ret != HI_SUCCESS) {
                hi_log_print_func_err(rsa_no_padding, ret);
            }
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1:
        case HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2: {
            bt = scheme - HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0;
            ret = rsa_padding_check_pkcs1_type(&rsa, key->klen, mode, bt, in, inlen, out, outlen);
            if (ret != HI_SUCCESS) {
                hi_log_error("error, rsa check pkcs1 type failed, ret = %d", ret);
                hi_log_print_func_err(rsa_padding_check_pkcs1_type, ret);
                ret = HI_ERR_CIPHER_FAILED_DECRYPT;
            }
            break;
        }
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1:
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA224:
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256:
#ifdef HI_PRODUCT_SHA512_SUPPORT
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA384:
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA512:
#endif
        case HI_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5: {
            ret = mbedtls_rsa_pkcs1_decrypt(&rsa, mbedtls_get_random,
                                            HI_NULL, mode, &outsize, in, out, key->klen);
            *outlen = (hi_u32)outsize;
            if (ret != HI_SUCCESS) {
                hi_log_error("error, rsa pkcs1 decrypt failed, ret = %d", ret);
                hi_log_print_func_err(mbedtls_rsa_pkcs1_decrypt, ret);
                ret = HI_ERR_CIPHER_FAILED_DECRYPT;
            }
            break;
        }
        default: {
            hi_log_error("RSA padding mode error, mode = 0x%x.\n", scheme);
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
            ret = HI_ERR_CIPHER_INVALID_PARA;
            break;
        }
    }

    cryp_rsa_deinit_key(&rsa);

    if (ret != HI_SUCCESS) {
        hi_log_error("rsa decrypt failed, scheme %d, ret = 0x%x\n", scheme, ret);
    }

    hi_log_func_exit();

    return ret;
}

hi_s32 cryp_rsa_sign_hash(cryp_rsa_key *key, hi_cipher_rsa_sign_scheme scheme,
                          hi_u8 *in, hi_u32 inlen, hi_u8 *out, hi_u32 *outlen, hi_u32 saltlen)
{
    hi_s32 ret;
    hi_u32 mode = 0;
    int padding = 0;
    int hash_id = 0;
    int hashlen = 0;
    mbedtls_rsa_context rsa;

    hi_log_func_enter();

    hi_log_check_param(key == HI_NULL);
    hi_log_check_param(in == HI_NULL);
    hi_log_check_param(out == HI_NULL);
    hi_log_check_param(outlen == HI_NULL);
    hi_log_check_param(key->klen > RSA_KEY_LEN_4096);
    hi_log_check_param(inlen > key->klen);

    hi_log_info("[RSA] sign hash, scheme %d\n", scheme);

    ret = cryp_rsa_sign_get_alg(scheme, &padding, &hash_id, &hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_sign_get_alg failed");
        hi_log_print_func_err(cryp_rsa_sign_get_alg, ret);
        return ret;
    }

    mbedtls_rsa_init(&rsa, padding, hash_id);

    ret = cryp_rsa_init_key(key, &mode, &rsa);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_init_key failed");
        hi_log_print_func_err(cryp_rsa_init_key, ret);
        return ret;
    }

    ret = mbedtls_rsa_pkcs1_sign(&rsa, mbedtls_get_random, HI_NULL,
                                 mode, hash_id, hashlen, in, out);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, rsa_pkcs1 sign failed, ret = %d\n", ret);
        hi_log_print_func_err(mbedtls_rsa_pkcs1_sign, ret);
        cryp_rsa_deinit_key(&rsa);
        ret = HI_ERR_CIPHER_RSA_SIGN;
        hi_log_error("rsa sign hash failed, scheme %d, ret = 0x%x\n", scheme, ret);
        return ret;
    }

    *outlen = key->klen;
    cryp_rsa_deinit_key(&rsa);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 cryp_rsa_verify_hash(cryp_rsa_key *key, hi_cipher_rsa_sign_scheme scheme,
                            hi_u8 *hash, hi_u32 hlen, hi_u8 *sign, hi_u32 signlen, hi_u32 saltlen)
{
    hi_s32 ret;
    int padding = 0;
    int hash_id = 0;
    int hashlen = 0;
    hi_u32 mode = 0;
    mbedtls_rsa_context rsa;

    hi_log_func_enter();

    hi_log_check_param(key == HI_NULL);
    hi_log_check_param(hash == HI_NULL);
    hi_log_check_param(sign == HI_NULL);
    hi_log_check_param(key->klen > RSA_KEY_LEN_4096);
    hi_log_check_param(signlen > key->klen);

    hi_log_info("[RSA] verify hash, scheme %d\n", scheme);

    ret = cryp_rsa_sign_get_alg(scheme, &padding, &hash_id, &hashlen);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_sign_get_alg failed");
        hi_log_print_func_err(cryp_rsa_sign_get_alg, ret);
        return ret;
    }

    mbedtls_rsa_init(&rsa, padding, hash_id);

    ret = cryp_rsa_init_key(key, &mode, &rsa);
    if (ret != HI_SUCCESS) {
        hi_log_error("error, cryp_rsa_init_key failed, ret = %d\n", ret);
        hi_log_print_func_err(cryp_rsa_init_key, ret);
        return ret;
    }

    ret = mbedtls_rsa_pkcs1_verify(&rsa, mbedtls_get_random, HI_NULL,
                                   mode, hash_id, hashlen, hash, sign);
    if (ret != HI_SUCCESS) {
        hi_log_error("error,  rsa pkcs1 verify failed, ret = %d\n", ret);
        hi_log_print_func_err(mbedtls_rsa_pkcs1_verify, ret);
        cryp_rsa_deinit_key(&rsa);
        ret = HI_ERR_CIPHER_RSA_VERIFY;
        hi_log_error("rsa verify hash failed, scheme %d, ret = 0x%x\n", scheme, ret);
        return ret;
    }

    cryp_rsa_deinit_key(&rsa);

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 cryp_rsa_gen_key(hi_u32 numbits, hi_u32 exponent, cryp_rsa_key *key)
{
    hi_s32 ret;
    mbedtls_rsa_context rsa;

    hi_log_func_enter();

    mbedtls_rsa_init(&rsa, 0, 0);

    key->klen = numbits / BYTE_BITS;
    key->e = exponent;

    check_exit(mbedtls_rsa_gen_key(&rsa, mbedtls_get_random, 0, numbits, exponent));
    check_exit(mbedtls_rsa_deduce_crt(&rsa.P, &rsa.Q, &rsa.D, &rsa.DP, &rsa.DQ, &rsa.QP));

    check_exit(mbedtls_mpi_write_binary(&rsa.N, key->n, key->klen));
    check_exit(mbedtls_mpi_write_binary(&rsa.D, key->d, key->klen));
    check_exit(mbedtls_mpi_write_binary(&rsa.P, key->p, key->klen >> 1));
    check_exit(mbedtls_mpi_write_binary(&rsa.Q, key->q, key->klen >> 1));
    check_exit(mbedtls_mpi_write_binary(&rsa.DP, key->dp, key->klen >> 1));
    check_exit(mbedtls_mpi_write_binary(&rsa.DQ, key->dq, key->klen >> 1));
    check_exit(mbedtls_mpi_write_binary(&rsa.QP, key->qp, key->klen >> 1));

    mbedtls_rsa_free(&rsa);
    hi_log_func_exit();
    return HI_SUCCESS;

exit__:

    hi_log_error("rsa gen key failed, ret = 0x%x\n", ret);
    mbedtls_rsa_free(&rsa);
    return ret;
}
#endif

hi_s32 cryp_rsa_compute_crt(cryp_rsa_key *key)
{
#ifdef HI_CIPHER_DEBUG
    hi_s32 ret;
    hi_u32 i;
    hi_u8 *buffer = HI_NULL;
    hi_u8 *p = HI_NULL;
    hi_u8 *q = HI_NULL;
    hi_u8 *n = HI_NULL;
    hi_u8 *d = HI_NULL;

    hi_log_func_enter();

    ret = drv_pke_resume();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_resume, ret);
        return ret;
    }

    buffer = crypto_calloc(1, key->klen * 4); /* malloc 4 buffer for p,q,n,d */
    if (buffer == HI_NULL) {
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_MEM);
        hi_log_print_func_err(crypto_calloc, ret);
        drv_pke_suspend();
        return HI_ERR_CIPHER_FAILED_MEM;
    }
    p = buffer;
    q = p + key->klen;
    n = q + key->klen;
    d = n + key->klen;

    /* 1. D  = E^-1 mod ((P-1)*(Q-1))
     * 2. DP = D mod (P - 1)
     * 3. DQ = D mod (Q - 1)
     * 4. QP = Q^-1 mod P
     * 5. N  = P * Q
     */
    memcpy((p + (key->klen)) >> 1, key->p, (key->klen) >> 1);
    memcpy((q + (key->klen)) >> 1, key->q, (key->klen) >> 1);

    /* 1. D  = E^-1 mod ((P-1)*(Q-1)) */
    ret = drv_rsa_compute_crt(p, q, key->e, key->klen, n, d);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_rsa_compute_crt, ret);
        goto _error;
    }
    hi_print_hex("D", d, key->klen);
    hi_print_hex("N", n, key->klen);

    /* 2.1 P = P - 1 */
    for (i = key->klen - 1; i > 0; i--) {
        p[i] -= 1;
        if (p[i] != 0xFF) { /* not borrow */
            break;
        }
    }
    hi_print_hex("p", p, key->klen);

    /* 2.2 P = D mod (P - 1) */
    ret = drv_pke_mod(d, p, p, key->klen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_mod, ret);
        goto _error;
    }

    /* 2.3 DP = P */
    memcpy(key->dp, (p + (key->klen)) >> 1, (key->klen) >> 1);
    hi_print_hex("dp", key->dp, key->klen >> 1);

    /* 3.1 Q = Q - 1 */
    for (i = key->klen - 1; i > 0; i--) {
        q[i] -= 1;
        if (q[i] != 0xFF) { /* not borrow */
            break;
        }
    }
    hi_print_hex("q", q, key->klen);

    /* 3.3 Q = D mod (Q - 1) */
    ret = drv_pke_mod(d, q, q, key->klen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_mod, ret);
        goto _error;
    }

    /* 3.3 DQ = Q */
    memcpy(key->dq, (q + (key->klen)) >> 1, (key->klen) >> 1);
    hi_print_hex("dq", key->dq, key->klen >> 1);

    /* QP = Q^-1 mod P */  // only support width of 192/224/384/256/521
    memset(q, 0, key->klen);
    memset(p, 0, key->klen);
    memcpy((p + (key->klen)) >> 1, key->p, (key->klen) >> 1);
    memcpy((q + (key->klen)) >> 1, key->q, (key->klen) >> 1);
    hi_print_hex("p", p, key->klen);
    hi_print_hex("q", q, key->klen);
    ret = drv_pke_inv_mod(q, p, d, key->klen);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_inv_mod, ret);
        goto _error;
    }
    hi_print_hex("d", d, key->klen);
    memcpy(key->qp, (d + (key->klen)) >> 1, (key->klen) >> 1);
    hi_print_hex("qp", key->qp, key->klen >> 1);

    memset(buffer, 0, key->klen * 4); /* clear 4 buffer for p,q,n,d */
    crypto_free(buffer);
    buffer = HI_NULL;

    hi_log_func_exit();
    return HI_SUCCESS;

_error:
    drv_pke_suspend();
    memset(buffer, 0, key->klen * 4); /* free 4 buffer for p,q,n,d */
    crypto_free(buffer);
    buffer = HI_NULL;
    return ret;
#else
    hi_s32 ret;

    hi_log_func_enter();

    ret = ext_rsa_compute_crt(key);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(ext_rsa_compute_crt, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
#endif
}
/** @} */ /** <!-- ==== API Code end ==== */

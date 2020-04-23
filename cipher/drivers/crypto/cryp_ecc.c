/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_ecc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "drv_pke.h"
#include "cryp_ecc.h"
#include "cryp_trng.h"
#include "ext_alg.h"
#include "mbedtls/bignum.h"

#ifdef HI_PRODUCT_ECC_SUPPORT

/********************** Internal Structure Definition ************************/
/** \addtogroup      ecc */
/** @{ */ /** <!-- [ecc] */

#define HI_LOG_DEBUG_MSG(a, b, c)
#define ECC_SIGN_BUF_CNT          (15)
#define ECC_VERIFY_BUF_CNT        (20)
#define ECC_GET_RANDNUM_WAITDONE  (0x1000)

/* ecc function list */
ecc_func g_ecc_descriptor;

/* ! rsa mutex */
static crypto_mutex g_ecc_mutex;

#define KAPI_ECC_LOCK()                  \
    do { \
        ret = crypto_mutex_lock(&g_ecc_mutex); \
        if (ret != HI_SUCCESS) {             \
            hi_log_error("error, ecc lock failed\n");    \
            hi_log_func_exit();               \
        }  \
    } while (0)

#define KAPI_ECC_UNLOCK() crypto_mutex_unlock(&g_ecc_mutex)

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      ecc drivers */
/** @{ */ /** <!-- [ecc] */

#ifdef CHIP_PKE_VER_V200
hi_s32 cryp_ecc_gen_key(ecc_param_t *ecc, hi_u8 *inkey, hi_u8 *outkey, hi_u8 *px, hi_u8 *py)
{
    hi_s32 ret = HI_FAILURE;
    hi_u8 *d = HI_NULL;
    hi_u32 cnt = 0;

    hi_log_func_enter();

    KAPI_ECC_LOCK();

    check_exit(drv_pke_resume());

    for (cnt = 0; cnt < ECC_TRY_CNT; cnt++) {
        hi_log_info("Step 1. generate randnum d, 1<=k<=n-2\n");
        if (inkey != HI_NULL) {
            d = inkey;
        } else {
            cryp_ecc_get_randnum(outkey, ecc->n, ecc->ksize);
            d = outkey;
        }

        hi_log_info("Step 2. PA = dA * G\n");
        check_exit(drv_pke_mul_dot(d, ecc->gx, ecc->gy, px, py, ecc->ksize, HI_TRUE, ecc));

        /* 0 < d < n ? */
        if (cryp_ecc_rang_check(d, ecc->n, ecc->ksize) != HI_SUCCESS) {
            continue;
        }
        break;
    }

    if (cnt >= ECC_TRY_CNT) {
        hi_log_error("Error! gen ecc key failed!\n");
        ret = HI_FAILURE;
        goto exit__;
    }

exit__:

    drv_pke_clean_ram();
    drv_pke_suspend();

    KAPI_ECC_UNLOCK();

    hi_log_func_exit();

    return ret;
}

/* *** z = d * p(x,y), sharekey = z(x) *** */
hi_s32 cryp_ecdh_compute_key(ecc_param_t *ecc, hi_u8 *d, hi_u8 *px, hi_u8 *py, hi_u8 *sharekey)
{
    hi_s32 ret;

    hi_log_func_enter();

    KAPI_ECC_LOCK();

    ret = drv_pke_resume();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_pke_resume, ret);
        KAPI_ECC_UNLOCK();
        return ret;
    }

    ret = drv_pke_mul_dot(d, px, py, sharekey, HI_NULL, ecc->ksize, HI_FALSE, ecc);
    if (ret != HI_SUCCESS) {
        drv_pke_suspend();
        hi_log_print_func_err(drv_pke_mul_dot, ret);
        KAPI_ECC_UNLOCK();
        return ret;
    }

    drv_pke_clean_ram();
    drv_pke_suspend();

    KAPI_ECC_UNLOCK();
    hi_log_func_exit();
    return HI_SUCCESS;
}

static int cryp_ecdsa_derive_mpi(ecc_param_t *ecc, hi_u8 *hash, hi_u32 hlen, hi_u8 *e)
{
    int ret;
    mbedtls_mpi x;
    mbedtls_mpi n;
    hi_u32 nbits;
    hi_u32 use_size;

    mbedtls_mpi_init(&x);
    mbedtls_mpi_init(&n);

    use_size = cipher_min(hlen, ecc->ksize);

    check_exit(mbedtls_mpi_read_binary(&x, hash, use_size));
    check_exit(mbedtls_mpi_read_binary(&n, ecc->n, ecc->ksize));
    nbits = mbedtls_mpi_bitlen(&n);

    if (use_size * BITS_IN_BYTE > nbits) {
        check_exit(mbedtls_mpi_shift_r(&x, use_size * BITS_IN_BYTE - nbits));
    }

    /* While at it, reduce modulo N */
    if (mbedtls_mpi_cmp_mpi(&x, &n) >= 0) {
        check_exit(mbedtls_mpi_sub_mpi(&x, &x, &n));
    }

    check_exit(mbedtls_mpi_write_binary(&x, e, ecc->ksize));

exit__:

    mbedtls_mpi_free(&x);
    mbedtls_mpi_free(&n);

    return (ret);
}

hi_s32 cryp_ecdsa_sign(ecc_param_t *ecc, hi_u8 *d, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 *buf = HI_NULL;
    hi_u8 *k = HI_NULL;
    hi_u8 *Rx = HI_NULL;
    hi_u8 *Ry = HI_NULL;
    hi_u8 *e = HI_NULL;
    hi_u8 *N = HI_NULL;
    hi_u8 *E = HI_NULL;
    hi_u8 *mE = HI_NULL;
    hi_u8 *mr = HI_NULL;
    hi_u8 *md = HI_NULL;
    hi_u8 *mrd = HI_NULL;
    hi_u8 *y = HI_NULL;
    hi_u8 *mk = HI_NULL;
    hi_u8 *mkni = HI_NULL;
    hi_u8 *ms = HI_NULL;
    hi_u32 retry = 0;
    hi_u32 id = 0;
    pke_block block;

    hi_log_func_enter();

    /* d < n ? */
    if (cryp_ecc_cmp(d, ecc->n, ecc->ksize) >= 0) {
        hi_log_error("Error! d must less than n!\n");
        hi_log_print_func_err(cryp_ecc_cmp, HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    KAPI_ECC_LOCK();

    buf = crypto_calloc(1, ecc->ksize * ECC_SIGN_BUF_CNT);
    if (buf == HI_NULL) {
        hi_log_error("Error! Malloc memory failed!\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        KAPI_ECC_UNLOCK();
        return HI_FAILURE;
    }

    check_exit(drv_pke_resume());

    k = buf + ecc->ksize * id++;
    Rx = buf + ecc->ksize * id++;
    Ry = buf + ecc->ksize * id++;
    e = buf + ecc->ksize * id++;
    N = buf + ecc->ksize * id++;
    E = buf + ecc->ksize * id++;
    mE = buf + ecc->ksize * id++;
    mr = buf + ecc->ksize * id++;
    md = buf + ecc->ksize * id++;
    mrd = buf + ecc->ksize * id++;
    y = buf + ecc->ksize * id++;
    mk = buf + ecc->ksize * id++;
    mkni = buf + ecc->ksize * id++;
    ms = buf + ecc->ksize * id++;

    check_exit(cryp_ecdsa_derive_mpi(ecc, hash, hlen, e));

    while (1) {
        if (retry++ > ECC_TRY_CNT) {
            hi_log_error("Error! K is Invalid!\n");
            ret = HI_FAILURE;
            goto exit__;
        }

        /************Step 1  ******************/
        hi_log_info("1. generate randnum k\n");
        cryp_ecc_get_randnum(k, ecc->n, ecc->ksize);
        HI_LOG_DEBUG_MSG("k", k, ecc->ksize);

        /************Step 2 - 7 **************/
        hi_log_info("2. R = k*G\n");
        check_exit(drv_pke_mul_dot(k, ecc->gx, ecc->gy, Rx, Ry, ecc->ksize, HI_FALSE, ecc));
        HI_LOG_DEBUG_MSG("Rx", Rx, ecc->ksize);
        HI_LOG_DEBUG_MSG("Ry", Ry, ecc->ksize);

        hi_log_info("3. r= (xR + 0) mod n\n");
        check_exit(drv_pke_add_mod(Rx, ms, ecc->n, r, ecc->ksize));
        HI_LOG_DEBUG_MSG("r", r, ecc->ksize);

        hi_log_info("5. r ?=0\n");
        if (cryp_ecc_is_zero(r, ecc->ksize)) {
            continue;
        }

        hi_log_info("4. N =2^2len_n mod n\n");
        block.word[0] = 0x00;
        block.word[1] = 0x01;
        check_exit(drv_pke_mod_block(&block, 2 * ecc->ksize, ecc->n, ecc->ksize, N)); /* 2 ksize */
        HI_LOG_DEBUG_MSG("N", N, ecc->ksize);

        hi_log_info("5. E =e+0 mod n\n");
        check_exit(drv_pke_add_mod(e, ms, ecc->n, E, ecc->ksize));
        HI_LOG_DEBUG_MSG("E", E, ecc->ksize);

        hi_log_info("6. mE = E * N mod n\n");
        check_exit(drv_pke_mul_mod(E, N, ecc->n, mE, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mE", mE, ecc->ksize);

        hi_log_info("7. mr = r * N mod n\n");
        check_exit(drv_pke_mul_mod(r, N, ecc->n, mr, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mr", mr, ecc->ksize);

        hi_log_info("8. md = d * N mod n\n");
        check_exit(drv_pke_mul_mod(d, N, ecc->n, md, ecc->ksize, HI_TRUE));
        HI_LOG_DEBUG_MSG("md", md, ecc->ksize);

        hi_log_info("9. mrd = md * mr mod n\n");
        check_exit(drv_pke_mul_mod(md, mr, ecc->n, mrd, ecc->ksize, HI_TRUE));
        HI_LOG_DEBUG_MSG("mrd", mrd, ecc->ksize);

        hi_log_info("10. y = mE + mrd mod n\n");
        check_exit(drv_pke_add_mod(mE, mrd, ecc->n, y, ecc->ksize));
        HI_LOG_DEBUG_MSG("y", y, ecc->ksize);

        hi_log_info("11. mk = k * N mod n\n");
        check_exit(drv_pke_mul_mod(k, N, ecc->n, mk, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mk", mk, ecc->ksize);

        hi_log_info("12. mkni = mk ^ -1 mod n\n");
        check_exit(drv_pke_inv_mod(mk, ecc->n, mkni, ecc->ksize));
        HI_LOG_DEBUG_MSG("mkni", mkni, ecc->ksize);

        hi_log_info("13. ms = mkni * y mod n\n");
        check_exit(drv_pke_mul_mod(mkni, y, ecc->n, ms, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("ms", ms, ecc->ksize);

        hi_log_info("14. s = ms * 1 mod n\n");
        memset(k, 0, ecc->ksize);
        k[ecc->ksize - 1] = 0x01;
        check_exit(drv_pke_mul_mod(ms, k, ecc->n, s, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("s", s, ecc->ksize);
        break;
    }

exit__:
    drv_pke_clean_ram();
    drv_pke_suspend();
    (void)memset_s(buf, ecc->ksize * ECC_SIGN_BUF_CNT, 0, ecc->ksize * ECC_SIGN_BUF_CNT);

    crypto_free(buf);
    buf = HI_NULL;

    KAPI_ECC_UNLOCK();
    hi_log_func_exit();

    return ret;
}

hi_s32 cryp_ecdsa_verify(ecc_param_t *ecc, hi_u8 *px, hi_u8 *py, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 *buf = HI_NULL;
    hi_u8 *ms = HI_NULL;
    hi_u8 *msni = HI_NULL;
    hi_u8 *e = HI_NULL;
    hi_u8 *N = HI_NULL;
    hi_u8 *E = HI_NULL;
    hi_u8 *mE = HI_NULL;
    hi_u8 *mu1 = HI_NULL;
    hi_u8 *u1 = HI_NULL;
    hi_u8 *mr = HI_NULL;
    hi_u8 *mu2 = HI_NULL;
    hi_u8 *u2 = HI_NULL;
    hi_u8 *u1Gx = HI_NULL;
    hi_u8 *u1Gy = HI_NULL;
    hi_u8 *u2Qx = HI_NULL;
    hi_u8 *u2Qy = HI_NULL;
    hi_u8 *Rx = HI_NULL;
    hi_u8 *Ry = HI_NULL;
    hi_u8 *v = HI_NULL;
    hi_u32 retry = 0;
    hi_u32 id = 0;
    pke_block block;

    hi_log_func_enter();

    /* 0 < r < n ? */
    if (cryp_ecc_is_zero(r, ecc->ksize) == HI_TRUE) {
        hi_log_error("Error! r must large than 0!\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    if (cryp_ecc_cmp(r, ecc->n, ecc->ksize) >= 0) {
        hi_log_error("Error! r must less than n!\n");
        hi_log_print_func_err(cryp_ecc_cmp, HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    /* 0< s < n ? */
    if (cryp_ecc_is_zero(s, ecc->ksize) == HI_TRUE) {
        hi_log_error("Error! s must large than 0!\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    if (cryp_ecc_cmp(s, ecc->n, ecc->ksize) >= 0) {
        hi_log_error("Error! s must less than n!\n");
        hi_log_print_func_err(cryp_ecc_cmp, HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    KAPI_ECC_LOCK();

    buf = crypto_calloc(1, ecc->ksize * ECC_VERIFY_BUF_CNT);
    if (buf == HI_NULL) {
        hi_log_error("Error! Malloc memory failed!\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        KAPI_ECC_UNLOCK();
        return HI_FAILURE;
    }

    check_exit(drv_pke_resume());

    ms = buf + ecc->ksize * id++;
    msni = buf + ecc->ksize * id++;
    e = buf + ecc->ksize * id++;
    N = buf + ecc->ksize * id++;
    E = buf + ecc->ksize * id++;
    mE = buf + ecc->ksize * id++;
    mu1 = buf + ecc->ksize * id++;
    u1 = buf + ecc->ksize * id++;
    mr = buf + ecc->ksize * id++;
    mu2 = buf + ecc->ksize * id++;
    u2 = buf + ecc->ksize * id++;
    u1Gx = buf + ecc->ksize * id++;
    u1Gy = buf + ecc->ksize * id++;
    u2Qx = buf + ecc->ksize * id++;
    u2Qy = buf + ecc->ksize * id++;
    Rx = buf + ecc->ksize * id++;
    Ry = buf + ecc->ksize * id++;
    v = buf + ecc->ksize * id++;

    check_exit(cryp_ecdsa_derive_mpi(ecc, hash, hlen, e));

    HI_LOG_DEBUG_MSG("e", e, ecc->ksize);
    HI_LOG_DEBUG_MSG("r", r, ecc->ksize);
    HI_LOG_DEBUG_MSG("s", s, ecc->ksize);

    while (1) {
        if (retry++ > ECC_TRY_CNT) {
            hi_log_error("Error! K is Invalid!\n");
            ret = HI_FAILURE;
            goto exit__;
        }

        /************Step 1  ******************/
        hi_log_info("1. N =2^2len_n mod n\n");
        block.word[0] = 0x00;
        block.word[1] = 0x01;
        check_exit(drv_pke_mod_block(&block, 2 * ecc->ksize, ecc->n, ecc->ksize, N)); /* 2 ksize */
        HI_LOG_DEBUG_MSG("N", N, ecc->ksize);

        hi_log_info("2. ms = s * N mod n\n");
        HI_LOG_DEBUG_MSG("s", s, ecc->ksize);
        check_exit(drv_pke_mul_mod(s, N, ecc->n, ms, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("ms", ms, ecc->ksize);

        hi_log_info("3. msni = ms ^ -1 mod n\n");
        check_exit(drv_pke_inv_mod(ms, ecc->n, msni, ecc->ksize));
        HI_LOG_DEBUG_MSG("msni", msni, ecc->ksize);

        hi_log_info("4. E =e+0 mod n\n");
        check_exit(drv_pke_add_mod(e, v, ecc->n, E, ecc->ksize));
        HI_LOG_DEBUG_MSG("E", E, ecc->ksize);

        hi_log_info("5. mE = E * N mod n\n");
        check_exit(drv_pke_mul_mod(E, N, ecc->n, mE, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mE", mE, ecc->ksize);

        hi_log_info("6. mu1 = mE * msni mod n\n");
        check_exit(drv_pke_mul_mod(mE, msni, ecc->n, mu1, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mu1", mu1, ecc->ksize);

        hi_log_info("7. u1 = mu1 * 1 mod n\n");
        memset(ms, 0, ecc->ksize);
        ms[ecc->ksize - 1] = 0x01;
        check_exit(drv_pke_mul_mod(mu1, ms, ecc->n, u1, ecc->ksize, HI_TRUE));
        HI_LOG_DEBUG_MSG("u1", u1, ecc->ksize);

        hi_log_info("8. mr = r * N mod n\n");
        check_exit(drv_pke_mul_mod(r, N, ecc->n, mr, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mr", mr, ecc->ksize);

        hi_log_info("9. mu2 = mr * msni mod n\n");
        check_exit(drv_pke_mul_mod(mr, msni, ecc->n, mu2, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("mu2", mu2, ecc->ksize);

        hi_log_info("10. u2 = mu2 * 1 mod n\n");
        memset(ms, 0, ecc->ksize);
        ms[ecc->ksize - 1] = 0x01;
        check_exit(drv_pke_mul_mod(mu2, ms, ecc->n, u2, ecc->ksize, HI_FALSE));
        HI_LOG_DEBUG_MSG("u2", u2, ecc->ksize);

        hi_log_info("11. u1G=u1*G\n");
        check_exit(drv_pke_mul_dot(u1, ecc->gx, ecc->gy, u1Gx, u1Gy, ecc->ksize, HI_FALSE, ecc));
        HI_LOG_DEBUG_MSG("u1Gx", u1Gx, ecc->ksize);
        HI_LOG_DEBUG_MSG("u1Gy", u1Gy, ecc->ksize);

        hi_log_info("12. u2Q=u2*Qu\n");
        check_exit(drv_pke_mul_dot(u2, px, py, u2Qx, u2Qy, ecc->ksize, HI_FALSE, ecc));
        HI_LOG_DEBUG_MSG("u2Qx", u2Qx, ecc->ksize);
        HI_LOG_DEBUG_MSG("u2Qy", u2Qy, ecc->ksize);

        hi_log_info("13. R=u1G+u2Q\n");
        check_exit(drv_pke_add_dot(u1Gx, u1Gy, u2Qx, u2Qy, Rx, Ry, ecc->ksize, ecc));
        HI_LOG_DEBUG_MSG("Rx", Rx, ecc->ksize);
        HI_LOG_DEBUG_MSG("Ry", Ry, ecc->ksize);

        hi_log_info("14. v= (xR+0) mod n\n");
        memset(ms, 0, ecc->ksize);
        check_exit(drv_pke_add_mod(Rx, ms, ecc->n, v, ecc->ksize));
        HI_LOG_DEBUG_MSG("v", v, ecc->ksize);

        hi_log_info("v = r ?\n");
        if (cryp_ecc_cmp(r, v, ecc->ksize) == 0) {
            ret = HI_SUCCESS;
        } else {
            hi_log_error("Error! r != v!\n");
            hi_log_print_err_code(HI_FAILURE);
            ret = HI_FAILURE;
        }
        break;
    }

exit__:

    drv_pke_clean_ram();
    drv_pke_suspend();
    (void)memset_s(buf, ecc->ksize * ECC_SIGN_BUF_CNT, 0, ecc->ksize * ECC_SIGN_BUF_CNT);
    crypto_free(buf);
    buf = HI_NULL;

    KAPI_ECC_UNLOCK();
    hi_log_func_exit();

    return ret;
}
#endif

int cryp_ecc_init(void)
{
    hi_s32 ret;
    pke_capacity capacity;

    hi_log_func_enter();

    crypto_mutex_init(&g_ecc_mutex);

    ret = memset_s(&g_ecc_descriptor, sizeof(g_ecc_descriptor), 0, sizeof(g_ecc_descriptor));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }
    ret = memset_s(&capacity, sizeof(capacity), 0, sizeof(pke_capacity));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

#ifdef CHIP_PKE_VER_V200
    drv_pke_get_capacity(&capacity);

    /* replace the ecc function of tomcrypt */
    if (capacity.ecc) {
        if (drv_pke_init() != HI_SUCCESS) {
            return HI_FAILURE;
        }
        g_ecc_descriptor.sign = cryp_ecdsa_sign;
        g_ecc_descriptor.verify = cryp_ecdsa_verify;
        g_ecc_descriptor.ecdh = cryp_ecdh_compute_key;
        g_ecc_descriptor.genkey = cryp_ecc_gen_key;
    } else
#endif
#ifdef SOFT_ECC_SUPPORT
    {
        g_ecc_descriptor.sign = ext_ecdsa_sign_hash;
        g_ecc_descriptor.verify = ext_ecdsa_verify_hash;
        g_ecc_descriptor.ecdh = ext_ecdh_compute_key;
        g_ecc_descriptor.genkey = ext_ecc_gen_key;
    }
#endif

    hi_log_func_exit();
    return HI_SUCCESS;
}

void cryp_ecc_deinit(void)
{
#ifdef CHIP_PKE_VER_V200
    pke_capacity capacity;

    drv_pke_get_capacity(&capacity);

    /* recovery the ecc function of mbedtls */
    if (capacity.ecc) {
        drv_pke_deinit();
    }
#endif
    crypto_mutex_destroy(&g_ecc_mutex);
}

ecc_func *cryp_get_ecc_op(void)
{
    return &g_ecc_descriptor;
}

#endif

#ifdef CHIP_PKE_SUPPORT
void cryp_ecc_get_randnum(hi_u8 *randnum, const hi_u8 *max, hi_u32 klen)
{
    hi_u32 i = 0;
    hi_u32 val = 0;

    for (i = 0; i < klen; i += WORD_WIDTH) {
        cryp_trng_get_random(&val, -1);
        memcpy(&randnum[i], &val, WORD_WIDTH);
    }

    /* make sure randnum <= max */
    for (i = 0; i < klen; i++) {
        if (randnum[i] < max[i]) {
            break;
        }
        randnum[i] = max[i];
    }

    return;
}

/* check val whether zero or not */
hi_u32 cryp_ecc_is_zero(const void *val, hi_u32 klen)
{
    hi_u32 i = 0;
    const hi_u8 *p = val;

    for (i = 0; i < klen; i++) {
        if (p[i] != 0x00) {
            return HI_FALSE;
        }
    }
    return HI_TRUE;
}

/* compare 2 val */
hi_s32 cryp_ecc_cmp(const void *val1, const void *val2, hi_u32 klen)
{
    return osal_memncmp(val1, klen, val2, klen);
}

/* check val whether less than max or not */
hi_s32 cryp_ecc_rang_check(const hi_u8 *val, const hi_u8 *max, hi_u32 klen)
{
    hi_u32 i = 0;

    if (cryp_ecc_is_zero(val, klen)) {
        return HI_ERR_CIPHER_ILLEGAL_DATA;
    }

    for (i = 0; i < klen; i++) {
        if (val[i] < max[i]) {
            return HI_SUCCESS;
        } else if (val[i] > max[i]) {
            return HI_ERR_CIPHER_ILLEGAL_DATA;
        }
    }

    return HI_SUCCESS;
}
#endif

/** @} */ /** <!-- ==== API Code end ==== */

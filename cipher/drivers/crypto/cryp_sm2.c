/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_sm2
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "drv_pke.h"
#include "cryp_sm2.h"
#include "cryp_ecc.h"
#include "cryp_trng.h"
#include "ext_alg.h"

#ifdef HI_PRODUCT_SM2_SUPPORT

#define HI_LOG_DEBUG_MSG(a, b, c)
#define SM2_SIGN_BUF_CNT          (18)
#define SM2_GET_RANDNUM_WAITDONE  (0x1000)
#define SM2_TRY_CNT               8

#ifdef CHIP_PKE_SUPPORT
/* ! Define the constant value */
static const hi_u8 g_sm2n1[SM2_LEN_IN_BYTE] = {
    0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x72, 0x03, 0xDF, 0x6B, 0x21, 0xC6, 0x05, 0x2B, 0x53, 0xBB, 0xF4, 0x09, 0x39, 0xD5, 0x41, 0x22
};
static const hi_u8 g_sm2n2[SM2_LEN_IN_BYTE] = {
    0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x72, 0x03, 0xDF, 0x6B, 0x21, 0xC6, 0x05, 0x2B, 0x53, 0xBB, 0xF4, 0x09, 0x39, 0xD5, 0x41, 0x21
};
static const hi_u8 g_sm2P[SM2_LEN_IN_BYTE] = {
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03
};
static const hi_u8 g_sm2N[SM2_LEN_IN_BYTE] = {
    0x1E, 0xB5, 0xE4, 0x12, 0xA2, 0x2B, 0x3D, 0x3B, 0x62, 0x0F, 0xC8, 0x4C, 0x3A, 0xFF, 0xE0, 0xD4,
    0x34, 0x64, 0x50, 0x4A, 0xDE, 0x6F, 0xA2, 0xFA, 0x90, 0x11, 0x92, 0xAF, 0x7C, 0x11, 0x4F, 0x20
};
static const hi_u8 g_sm21[SM2_LEN_IN_BYTE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
};
static const hi_u8 g_sm20[SM2_LEN_IN_BYTE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const hi_u8 g_sm2ma[SM2_LEN_IN_BYTE] = {
    0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC
};
static const hi_u8 g_sm2mb[SM2_LEN_IN_BYTE] = {
    0x24, 0x0F, 0xE1, 0x88, 0xBA, 0x20, 0xE2, 0xC8, 0x52, 0x79, 0x81, 0x50, 0x5E, 0xA5, 0x1C, 0x3C,
    0x71, 0xCF, 0x37, 0x9A, 0xE9, 0xB5, 0x37, 0xAB, 0x90, 0xD2, 0x30, 0x63, 0x2B, 0xC0, 0xDD, 0x42
};

#define drv_sm2_add_mod(a, b, p, out) \
    drv_pke_add_mod((hi_u8 *)a, (hi_u8 *)b, (hi_u8 *)p, (hi_u8 *)out, SM2_LEN_IN_BYTE)
#define drv_sm2_sub_mod(a, b, p, out) \
    drv_pke_sub_mod((hi_u8 *)a, (hi_u8 *)b, (hi_u8 *)p, (hi_u8 *)out, SM2_LEN_IN_BYTE)
#define drv_sm2_mul_modp(a, b, c, private) \
    drv_pke_mul_mod((hi_u8 *)a, (hi_u8 *)b, (hi_u8 *)sm2_id->p, (hi_u8 *)c, SM2_LEN_IN_BYTE, private)
#define drv_sm2_mul_modn(a, b, c, private) \
    drv_pke_mul_mod((hi_u8 *)a, (hi_u8 *)b, (hi_u8 *)sm2_id->n, (hi_u8 *)c, SM2_LEN_IN_BYTE, private)
#define drv_sm2_inv_mod(a, p, c) \
    drv_pke_inv_mod((hi_u8 *)a, (hi_u8 *)p, (hi_u8 *)c, SM2_LEN_IN_BYTE)
#define drv_sm2_mod(a, p, c) \
    drv_pke_mod((hi_u8 *)a, (hi_u8 *)p, (hi_u8 *)c, SM2_LEN_IN_BYTE)
#define drv_sm2_mul(a, b, c) \
    drv_pke_mul((hi_u8 *)a, (hi_u8 *)b, (hi_u8 *)c, SM2_LEN_IN_BYTE)
#define drv_sm2_mul_dot(k, px, py, rx, ry, private) \
    drv_pke_mul_dot((hi_u8 *)k, (hi_u8 *)px, (hi_u8 *)py, (hi_u8 *)rx, (hi_u8 *)ry, SM2_LEN_IN_BYTE, private, sm2_id)
#define drv_sm2_add_dot(sx, sy, rx, ry, cx, cy) \
    drv_pke_add_dot((hi_u8 *)sx, (hi_u8 *)sy, (hi_u8 *)rx, (hi_u8 *)ry, \
    (hi_u8 *)cx, (hi_u8 *)cy, SM2_LEN_IN_BYTE, sm2_id)

#define cryp_sm2_get_randnum(a, b) cryp_ecc_get_randnum(a, b, SM2_LEN_IN_BYTE)
#define cryp_sm2_is_zero(a)        cryp_ecc_is_zero(a, SM2_LEN_IN_BYTE)
#define cryp_sm2_cmp(a, b)         cryp_ecc_cmp(a, b, SM2_LEN_IN_BYTE)
#define cryp_sm2_rang_check(a)     cryp_ecc_rang_check(a, g_sm2n1, SM2_LEN_IN_BYTE)
#endif

/* sm2 function list */
sm2_func g_sm2_descriptor;

/* ! rsa mutex */
static crypto_mutex g_sm2_mutex;

#define KAPI_SM2_LOCK()                  \
    do { \
        ret = crypto_mutex_lock(&g_sm2_mutex); \
        if (ret != HI_SUCCESS) {             \
            hi_log_error("error, sm2 lock failed\n");    \
            hi_log_func_exit();               \
        } \
    } while (0)

#define KAPI_SM2_UNLOCK() crypto_mutex_unlock(&g_sm2_mutex)

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      sm2 drivers */
/** @{ */ /** <!-- [sm2] */

#ifdef CHIP_PKE_SUPPORT

/***************************************
  1. generate a randnum k, k[1,n-1],
  2. (x1,y1)=[k]G,
  3. r=(e+x1)mod n, if r=0 or r+k=n, return step 1
  4. s=((1+dA)^-1 (k-rdA))mod n
****************************************/
hi_s32 cryp_sm2_sign(hi_u32 e[SM2_LEN_IN_WROD], hi_u32 d[SM2_LEN_IN_WROD],
                     hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD],
                     hi_u32 length, ecc_param_t *sm2_id)
{
    hi_s32 ret;
    hi_s32 ret_exit;
    hi_u8 *buf = HI_NULL;
    hi_u8 *k = HI_NULL;
    hi_u8 *Rx = HI_NULL;
    hi_u8 *Ry = HI_NULL;
    hi_u8 *ee = HI_NULL;
    hi_u8 *rk = HI_NULL;
    hi_u8 *my = HI_NULL;
    hi_u8 *mr = HI_NULL;
    hi_u8 *mk = HI_NULL;
    hi_u8 *mrk = HI_NULL;
    hi_u8 *mrky = HI_NULL;
    hi_u8 *TA = HI_NULL;
    hi_u8 *mTA = HI_NULL;
    hi_u8 *mTy = HI_NULL;
    hi_u8 *minv = HI_NULL;
    hi_u8 *mrk1 = HI_NULL;
    hi_u8 *mrky1 = HI_NULL;
    hi_u8 *mTz = HI_NULL;
    hi_u8 *ms = HI_NULL;
    hi_u32 re_try = 0;
    hi_u32 id = 0;
    hi_u32 i = 0;
    hi_u32 j = 0;

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    /* d < n ? */
    if (cryp_sm2_cmp(d, sm2_id->n) >= 0) {
        hi_log_error("Error! d must less than n!\n");
        hi_log_print_func_err(cryp_sm2_cmp, HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    KAPI_SM2_LOCK();

    buf = (hi_u8 *)crypto_calloc(1, SM2_LEN_IN_BYTE * SM2_SIGN_BUF_CNT);
    if (buf == HI_NULL) {
        hi_log_error("Error! Malloc memory failed!\n");
        hi_log_print_func_err(crypto_calloc, HI_ERR_CIPHER_FAILED_MEM);
        KAPI_SM2_UNLOCK();
        return HI_FAILURE;
    }

    check_exit(drv_pke_resume());

    Rx = buf + SM2_LEN_IN_BYTE * id++;
    Ry = buf + SM2_LEN_IN_BYTE * id++;
    ee = buf + SM2_LEN_IN_BYTE * id++;
    rk = buf + SM2_LEN_IN_BYTE * id++;
    my = buf + SM2_LEN_IN_BYTE * id++;
    mr = buf + SM2_LEN_IN_BYTE * id++;
    mk = buf + SM2_LEN_IN_BYTE * id++;
    mrk = buf + SM2_LEN_IN_BYTE * id++;
    mrky = buf + SM2_LEN_IN_BYTE * id++;
    TA = buf + SM2_LEN_IN_BYTE * id++;
    mTA = buf + SM2_LEN_IN_BYTE * id++;
    mTy = buf + SM2_LEN_IN_BYTE * id++;
    minv = buf + SM2_LEN_IN_BYTE * id++;
    mrk1 = buf + SM2_LEN_IN_BYTE * id++;
    mrky1 = buf + SM2_LEN_IN_BYTE * id++;
    mTz = buf + SM2_LEN_IN_BYTE * id++;
    ms = buf + SM2_LEN_IN_BYTE * id++;
    k = buf + SM2_LEN_IN_BYTE * id++;

    while (1) {
        if (re_try++ > SM2_TRY_CNT) {
            hi_log_error("Error! K is Invalid!\n");
            ret = HI_FAILURE;
            goto exit__;
        }

        /************Step 1  ******************/
        hi_log_info("1. generate randnum k\n");
        for (i = 0; i < SM2_TRY_CNT; i++) {
            cryp_sm2_get_randnum(buf + SM2_LEN_IN_WROD * i, g_sm2n1);
        }
        check_exit(cryp_trng_get_random(&j, -1));
        j %= SM2_TRY_CNT;
        check_exit(memcpy_s(k, SM2_LEN_IN_BYTE, buf + SM2_LEN_IN_WROD * j, SM2_LEN_IN_BYTE));
        HI_LOG_DEBUG_MSG("k", k, SM2_LEN_IN_BYTE);

        /************Step 2 - 7 **************/
        hi_log_info("2. R=k*G\n");
        check_exit(drv_sm2_mul_dot(k, sm2_id->gx, sm2_id->gy, Rx, Ry, HI_TRUE));
        HI_LOG_DEBUG_MSG("Rx", Rx, SM2_LEN_IN_BYTE);
        HI_LOG_DEBUG_MSG("Ry", Ry, SM2_LEN_IN_BYTE);

        hi_log_info("3. e`=e+0 mod n\n");
        check_exit(drv_sm2_add_mod(e, g_sm20, sm2_id->n, ee));
        HI_LOG_DEBUG_MSG("e`", ee, SM2_LEN_IN_BYTE);

        hi_log_info("4. r=e`+Rx mod n\n");
        check_exit(drv_sm2_add_mod(ee, Rx, sm2_id->n, r));
        HI_LOG_DEBUG_MSG("r", r, SM2_LEN_IN_BYTE);

        hi_log_info("5. r ?=0\n");
        if (cryp_sm2_is_zero(r)) {
            continue;
        }
        hi_log_info("6. rk= r+k mod n\n");
        check_exit(drv_sm2_add_mod(r, k, sm2_id->n, rk));
        HI_LOG_DEBUG_MSG("rk", rk, SM2_LEN_IN_BYTE);
        hi_log_info("7. rk ?=0\n");
        if (cryp_sm2_is_zero(rk)) {
            continue;
        }

        /************Step 8 - 13 **************/
        hi_log_info("8. generate randnum my\n");
        cryp_sm2_get_randnum(my, g_sm2n1);
        HI_LOG_DEBUG_MSG("my", my, SM2_LEN_IN_BYTE);

        hi_log_info("9. mr=r*N mod n\n");
        check_exit(drv_sm2_mul_modn(r, g_sm2N, mr, HI_FALSE));
        HI_LOG_DEBUG_MSG("mr", mr, SM2_LEN_IN_BYTE);

        hi_log_info("10. mk=k*N mod n\n");
        check_exit(drv_sm2_mul_modn(k, g_sm2N, mk, HI_FALSE));
        HI_LOG_DEBUG_MSG("mk", mk, SM2_LEN_IN_BYTE);

        hi_log_info("11. mrk = (mr + mk) mod n\n");
        check_exit(drv_sm2_add_mod(mr, mk, sm2_id->n, mrk));
        HI_LOG_DEBUG_MSG("mrk", mrk, SM2_LEN_IN_BYTE);

        hi_log_info("12. mrky = (mrk * my) mod n\n");
        check_exit(drv_sm2_mul_modn(mrk, my, mrky, HI_FALSE));
        HI_LOG_DEBUG_MSG("mrky", mrky, SM2_LEN_IN_BYTE);

        hi_log_info("13. TA= dA*my mod n\n");
        check_exit(drv_sm2_mul_modn(my, d, TA, HI_TRUE));
        HI_LOG_DEBUG_MSG("TA", TA, SM2_LEN_IN_BYTE);

        /************Step 14 - 19 **************/
        hi_log_info("14. mTA = TA*N mod n\n");
        check_exit(drv_sm2_mul_modn(TA, g_sm2N, mTA, HI_FALSE));
        HI_LOG_DEBUG_MSG("mTA", mTA, SM2_LEN_IN_BYTE);

        hi_log_info("15. mTy = (mTA + my) mod n\n");
        check_exit(drv_sm2_add_mod(mTA, my, sm2_id->n, mTy));
        HI_LOG_DEBUG_MSG("mTy", mTy, SM2_LEN_IN_BYTE);

        hi_log_info("16. minv = mTy^-1 mod n\n");
        check_exit(drv_sm2_inv_mod(mTy, sm2_id->n, minv));
        HI_LOG_DEBUG_MSG("minv", minv, SM2_LEN_IN_BYTE);

        hi_log_info("17. mrk1 = (mr + mk) mod n\n");
        check_exit(drv_sm2_add_mod(mr, mk, sm2_id->n, mrk1));
        HI_LOG_DEBUG_MSG("mrk1", mrk1, SM2_LEN_IN_BYTE);

        hi_log_info("18. mrky1 = (mrk1 * my) mod n\n");
        check_exit(drv_sm2_mul_modn(mrk1, my, mrky1, HI_FALSE));
        HI_LOG_DEBUG_MSG("mrky1", mrky1, SM2_LEN_IN_BYTE);

        hi_log_info("19. mrky1 ?= mrky\n");
        if (cryp_sm2_cmp(mrky1, mrky) != 0) {
            hi_log_error("Error! mrky1 != mrky!\n");
            ret = HI_FAILURE;
            goto exit__;
        }

        /************Step 20 - 25 **************/
        hi_log_info("20. mTz = (mrky * minv) mod n\n");
        check_exit(drv_sm2_mul_modn(mrky, minv, mTz, HI_FALSE));
        HI_LOG_DEBUG_MSG("mTz", mTz, SM2_LEN_IN_BYTE);

        hi_log_info("21. ms = (mTz - mr) mod n\n");
        check_exit(drv_sm2_sub_mod(mTz, mr, sm2_id->n, ms));
        HI_LOG_DEBUG_MSG("ms", ms, SM2_LEN_IN_BYTE);

        hi_log_info("22. ms ?=0\n");
        if (cryp_sm2_is_zero(ms)) {
            continue;
        }
        hi_log_info("23. s=ms*1 mod n\n");
        check_exit(drv_sm2_mul_modn(ms, g_sm21, s, HI_FALSE));
        HI_LOG_DEBUG_MSG("s", s, SM2_LEN_IN_BYTE);

        hi_log_info("24. rk= r+k mod n\n");
        check_exit(drv_sm2_add_mod(r, k, sm2_id->n, rk));
        HI_LOG_DEBUG_MSG("rk", rk, SM2_LEN_IN_BYTE);

        hi_log_info("25. rk ?=0\n");
        if (cryp_sm2_is_zero(rk)) {
            continue;
        }
        break;
    }

exit__:
    drv_pke_clean_ram();
    drv_pke_suspend();
    ret_exit = memset_s(buf, SM2_LEN_IN_BYTE * SM2_SIGN_BUF_CNT, 0, SM2_LEN_IN_BYTE * SM2_SIGN_BUF_CNT);
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret_exit);
    }

    KAPI_SM2_UNLOCK();

    crypto_free(buf);
    buf = HI_NULL;

    hi_log_func_exit();

    return ret;
}

/***************************************
  1. t=(r+s)mod n, if t==0, return fail
  2. (x1,y1)=[s]G+tP,
  3. r=(e+x1)mod n, if r==R, return pass
****************************************/
hi_s32 cryp_sm2_verify(hi_u32 e[SM2_LEN_IN_WROD],
                       hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                       hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD],
                       hi_u32 length, ecc_param_t *sm2_id)
{
    hi_s32 ret = HI_FAILURE;
    hi_u8 t[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 sgx[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 sgy[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 tpax[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 tpay[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 rx[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 ry[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 ee[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 v[SM2_LEN_IN_BYTE] = { 0 };

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    KAPI_SM2_LOCK();

    check_exit(drv_pke_resume());

    /* t = r + s mod n */
    check_exit(cryp_sm2_rang_check((hi_u8 *)r)); /* *** 1<=r<=n-1 ** */
    check_exit(cryp_sm2_rang_check((hi_u8 *)s)); /* *** 1<=s<=n-1 ** */
    check_exit(drv_sm2_add_mod(r, s, sm2_id->n, t));

    /* t = 0 ? */
    if (cryp_sm2_is_zero(t)) {
        hi_log_error("Error! t is zero!\n");
        ret = HI_FAILURE;
        hi_log_print_func_err(cryp_sm2_is_zero, HI_TRUE);
        goto exit__;
    }

    hi_log_info("sG = s * G\n");
    check_exit(drv_sm2_mul_dot(s, sm2_id->gx, sm2_id->gy, sgx, sgy, HI_FALSE));
    HI_LOG_DEBUG_MSG("sGx", sgx, SM2_LEN_IN_BYTE);
    HI_LOG_DEBUG_MSG("sGy", sgy, SM2_LEN_IN_BYTE);

    hi_log_info("tPA = t * PA\n");
    check_exit(drv_sm2_mul_dot(t, px, py, tpax, tpay, HI_FALSE));
    HI_LOG_DEBUG_MSG("tPAx", tpax, SM2_LEN_IN_BYTE);
    HI_LOG_DEBUG_MSG("tPAy", tpay, SM2_LEN_IN_BYTE);

    hi_log_info("R = sG + tPA\n");
    check_exit(drv_sm2_add_dot(sgx, sgy, tpax, tpay, rx, ry));
    HI_LOG_DEBUG_MSG("Rx", rx, SM2_LEN_IN_BYTE);
    HI_LOG_DEBUG_MSG("Ry", ry, SM2_LEN_IN_BYTE);

    hi_log_info("e` = e + 0 mod n\n");
    check_exit(drv_sm2_add_mod(e, g_sm20, sm2_id->n, ee));
    HI_LOG_DEBUG_MSG("e", e, SM2_LEN_IN_BYTE);
    HI_LOG_DEBUG_MSG("e`", ee, SM2_LEN_IN_BYTE);

    hi_log_info("v = e` + Rx mod n\n");
    check_exit(drv_sm2_add_mod(ee, rx, sm2_id->n, v));

    HI_LOG_DEBUG_MSG("r", r, SM2_LEN_IN_BYTE);
    HI_LOG_DEBUG_MSG("v", v, SM2_LEN_IN_BYTE);

    hi_log_info("v = r ?\n");
    if (cryp_sm2_cmp(r, v) == 0) {
        ret = HI_SUCCESS;
    } else {
        hi_log_error("Error! r != v!\n");
        hi_log_print_err_code(HI_FAILURE);
        ret = HI_FAILURE;
    }

exit__:
    drv_pke_clean_ram();
    drv_pke_suspend();

    KAPI_SM2_UNLOCK();

    hi_log_func_exit();

    return ret;
}

/* Compute C1(x, y) = k * G(x, y), XY = k * P(x, y) */
hi_s32 cryp_sm2_encrypt(hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                        hi_u32 c1x[SM2_LEN_IN_WROD], hi_u32 c1y[SM2_LEN_IN_WROD],
                        hi_u32 x2[SM2_LEN_IN_WROD], hi_u32 y2[SM2_LEN_IN_WROD],
                        hi_u32 length, ecc_param_t *sm2_id)
{
    hi_s32 ret = HI_FAILURE;
    hi_u8 k[SM2_LEN_IN_BYTE] = { 0 };
    hi_u32 retry = 0;

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    /* t = 0 ? */
    if (cryp_sm2_is_zero(px) || cryp_sm2_is_zero(py)) {
        hi_log_error("Error! PB is zero!\n");
        hi_log_print_func_err(cryp_sm2_is_zero, HI_TRUE);
        return HI_FAILURE;
    }

    KAPI_SM2_LOCK();

    check_exit(drv_pke_resume());

    while (1) {
        if (retry++ > SM2_TRY_CNT) {
            hi_log_error("Error! K is Invalid!\n");
            ret = HI_FAILURE;
            goto exit__;
        }

        /* generate randnum k, 1<=k<=n-1 */
        cryp_sm2_get_randnum(k, g_sm2n1);

        /* *** C1=k*G *** */
        check_exit(drv_sm2_mul_dot(k, sm2_id->gx, sm2_id->gy, c1x, c1y, HI_FALSE));

        /* *** c1 = 0 ? *** */
        if (cryp_sm2_is_zero(c1x) || cryp_sm2_is_zero(c1y)) {
            continue;
        }

        /* *** kPB = k * PB *** */
        check_exit(drv_sm2_mul_dot(k, px, py, x2, y2, HI_FALSE));

        /* *** xy = 0 ? *** */
        if (cryp_sm2_is_zero(x2) || cryp_sm2_is_zero(y2)) {
            continue;
        }
        break;
    }

exit__:

    drv_pke_clean_ram();
    drv_pke_suspend();

    KAPI_SM2_UNLOCK();

    hi_log_func_exit();

    return ret;
}

/* check y^2=x^3+ax+b ? */
static hi_s32 cryp_sm2_dot_check(hi_u32 x[SM2_LEN_IN_WROD], hi_u32 y[SM2_LEN_IN_WROD],
                                 hi_u32 check, ecc_param_t *sm2_id)
{
    hi_s32 ret = HI_FAILURE;
    hi_u8 mx[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 my[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 mx2[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 mx3[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 max[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 mt[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 ms[SM2_LEN_IN_BYTE] = { 0 };
    hi_u8 my2[SM2_LEN_IN_BYTE] = { 0 };

    hi_log_func_enter();

    /* xy = 0 ? */
    if (cryp_sm2_is_zero(x) || cryp_sm2_is_zero(y)) {
        hi_log_error("Error! X or Y is zero!\n");
        hi_log_print_func_err(cryp_sm2_is_zero, HI_TRUE);
        return HI_FAILURE;
    }

    hi_log_info("1. ma = a*P mod p\n");
    hi_log_info("2. mb = b*P mod p\n");
    hi_log_info("3. mx=C1x*P mod p\n");
    check_exit(drv_sm2_mul_modp(x, g_sm2P, mx, HI_FALSE));
    hi_log_info("4. my=C1y*P mod p\n");
    check_exit(drv_sm2_mul_modp(y, g_sm2P, my, HI_FALSE));
    hi_log_info("5. mx2=mx* mx mod p\n");
    check_exit(drv_sm2_mul_modp(mx, mx, mx2, HI_FALSE));
    hi_log_info("6. mx3=mx2* mx mod p\n");
    check_exit(drv_sm2_mul_modp(mx2, mx, mx3, HI_FALSE));
    hi_log_info("7. max=ma*mx mod p\n");
    check_exit(drv_sm2_mul_modp(g_sm2ma, mx, max, HI_FALSE));
    hi_log_info("8. mt= mx3+max mod p\n");
    check_exit(drv_sm2_add_mod(mx3, max, sm2_id->p, mt));
    hi_log_info("9. ms= mt+mb mod p\n");
    check_exit(drv_sm2_add_mod(mt, g_sm2mb, sm2_id->p, ms));
    hi_log_info("10. my2=my*my mod p\n");
    check_exit(drv_sm2_mul_modp(my, my, my2, HI_FALSE));
    hi_log_info("11. my2 ?= ms\n");
    if (cryp_sm2_cmp(ms, my2) != 0) {
        hi_log_error("Error! my2 != s!\n");
        ret = HI_FAILURE;
        goto exit__;
    }

    if (check) {
        hi_log_info("12. S= h * C1\n");
        check_exit(drv_sm2_mul_dot(g_sm21, x, y, mx, my, HI_FALSE));
    }
exit__:

    drv_pke_clean_ram();

    hi_log_func_exit();

    return ret;
}

/* Compute XY(x, y) = C1(x, y) * d */
hi_s32 cryp_sm2_decrypt(hi_u32 d[SM2_LEN_IN_WROD],
                        hi_u32 c1x[SM2_LEN_IN_WROD], hi_u32 c1y[SM2_LEN_IN_WROD],
                        hi_u32 x2[SM2_LEN_IN_WROD], hi_u32 y2[SM2_LEN_IN_WROD],
                        hi_u32 length, ecc_param_t *sm2_id)
{
    hi_s32 ret;
    hi_s32 ret_exit;
    hi_u32 mx[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 my[SM2_LEN_IN_WROD] = { 0 };

    hi_log_func_enter();

    if (length != SM2_LEN_IN_BYTE) {
        hi_log_error("Error! invalid sm2 length(%u)!\n", length);
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_LENGTH;
    }

    /* c1 = 0 ? */
    if (cryp_sm2_is_zero(c1x) || cryp_sm2_is_zero(c1y)) {
        hi_log_error("Error! C1 is zero!\n");
        hi_log_print_func_err(cryp_sm2_is_zero, HI_TRUE);
        return HI_FAILURE;
    }

    /* d < n ? */
    if (cryp_sm2_cmp(d, sm2_id->n) >= 0) {
        hi_log_error("Error! d must less than n!\n");
        hi_log_print_func_err(cryp_sm2_cmp, HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    KAPI_SM2_LOCK();

    check_exit(drv_pke_resume());

    /* check C1y^2=C1x^3+aC1x+b ? */
    check_exit(cryp_sm2_dot_check(c1x, c1y, HI_TRUE, sm2_id));

    /* *** Compute M(x,y) *** */
    check_exit(drv_sm2_mul_dot(d, c1x, c1y, mx, my, HI_TRUE));

    /* check C1y^2=C1x^3+aC1x+b ? */
    check_exit(cryp_sm2_dot_check(c1x, c1y, HI_TRUE, sm2_id));

    /* check My^2=Mx^3+aMx+b ? */
    check_exit(cryp_sm2_dot_check(mx, my, HI_FALSE, sm2_id));

    check_exit(memcpy_s(x2, length, mx, sizeof(mx)));
    check_exit(memcpy_s(y2, length, my, sizeof(my)));

exit__:

    ret_exit = memset_s(mx, sizeof(mx), 0, length);
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret_exit);
    }

    ret_exit = memset_s(my, sizeof(my), 0, length);
    if (ret_exit != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret_exit);
    }

    drv_pke_clean_ram();
    drv_pke_suspend();

    KAPI_SM2_UNLOCK();

    if (ret != HI_SUCCESS) {
        hi_log_print_err_code(ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* Compute P(x,y) = d * G(x,y) */
hi_s32 cryp_sm2_gen_key(hi_u32 d[SM2_LEN_IN_WROD],
                        hi_u32 px[SM2_LEN_IN_WROD],
                        hi_u32 py[SM2_LEN_IN_WROD],
                        hi_u32 length, ecc_param_t *sm2_id)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_u32 cnt = 0;
    hi_u8 randnum[SM2_TRY_CNT][SM2_LEN_IN_BYTE];

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    KAPI_SM2_LOCK();

    check_exit(drv_pke_resume());

    check_exit(memset_s(randnum, sizeof(randnum), 0, sizeof(randnum)));

    for (cnt = 0; cnt < SM2_TRY_CNT; cnt++) {
        hi_log_info("Step 1. generate randnum d, 1<=k<=n-2");
        for (i = 0; i < SM2_TRY_CNT; i++) {
            cryp_sm2_get_randnum(randnum[i], g_sm2n1);
        }
        cryp_trng_get_random(&j, -1);
        j %= SM2_TRY_CNT;
        check_exit(memcpy_s(d, SM2_LEN_IN_BYTE, randnum[j], SM2_LEN_IN_BYTE));

        hi_log_info("Step 2. PA=dA*G");

        /* *** P = d * G *** */
        check_exit(drv_sm2_mul_dot(d, sm2_id->gx, sm2_id->gy, px, py, HI_TRUE));

        /* *** 0 < d < n ? *** */
        if (cryp_sm2_is_zero(d) == HI_TRUE) {
            continue;
        }
        if (cryp_sm2_cmp(d, sm2_id->n) >= 0) {
            continue;
        }
        break;
    }

    if (cnt >= SM2_TRY_CNT) {
        hi_log_error("Error! gen ecc key failed!\n");
        ret = HI_FAILURE;
        goto exit__;
    }

exit__:

    drv_pke_clean_ram();
    drv_pke_suspend();

    KAPI_SM2_UNLOCK();

    hi_log_func_exit();

    return ret;
}
#endif

int cryp_sm2_init(void)
{
    hi_s32 ret;
    pke_capacity capacity;

    hi_log_func_enter();

    crypto_mutex_init(&g_sm2_mutex);

    ret = memset_s(&g_sm2_descriptor, sizeof(g_sm2_descriptor), 0, sizeof(g_sm2_descriptor));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    ret = memset_s(&capacity, sizeof(capacity), 0, sizeof(capacity));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

#ifdef CHIP_PKE_SUPPORT
    drv_pke_get_capacity(&capacity);

    /* replace the sm2 function of tomcrypt */
    if (capacity.sm2) {
        if (drv_pke_init() != HI_SUCCESS) {
            return HI_FAILURE;
        }
        g_sm2_descriptor.sign = cryp_sm2_sign;
        g_sm2_descriptor.verify = cryp_sm2_verify;
        g_sm2_descriptor.encrypt = cryp_sm2_encrypt;
        g_sm2_descriptor.decrypt = cryp_sm2_decrypt;
        g_sm2_descriptor.genkey = cryp_sm2_gen_key;
    }
#endif
#ifdef SOFT_SM2_SUPPORT
    else {
        g_sm2_descriptor.sign = mbedtls_sm2_sign;
        g_sm2_descriptor.verify = mbedtls_sm2_verify;
        g_sm2_descriptor.encrypt = mbedtls_sm2_encrypt;
        g_sm2_descriptor.decrypt = mbedtls_sm2_decrypt;
        g_sm2_descriptor.genkey = mbedtls_sm2_gen_key;
    }
#endif

    hi_log_func_exit();
    return HI_SUCCESS;
}

void cryp_sm2_deinit(void)
{
#ifdef CHIP_PKE_SUPPORT
    pke_capacity capacity;

    drv_pke_get_capacity(&capacity);

    /* recovery the sm2 function of mbedtls */
    if (capacity.sm2) {
        drv_pke_deinit();
    }
#endif
    crypto_mutex_destroy(&g_sm2_mutex);
}

sm2_func *cryp_get_sm2_op(void)
{
    return &g_sm2_descriptor;
}

#endif


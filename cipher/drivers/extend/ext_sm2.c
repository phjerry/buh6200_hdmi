/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of ext_sm2
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_trng.h"
#include "cryp_sm2.h"
#include "cryp_rsa.h"
#include "mbedtls/ecp.h"

#ifdef SOFT_SM2_SUPPORT

/************************* Internal Structure Definition ********************/
/** \addtogroup      sm2 */
/** @{ */ /** <!-- [sm2] */

#define SM256R1_P \
    "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define SM256R1_A \
    "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC"
#define SM256R1_B \
    "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
#define SM256R1_GX \
    "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"
#define SM256R1_GY \
    "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0"
#define SM256R1_N \
    "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"

#define RADIX_HEX      16
#define KEY_MAX_TRIES  10

/**
* \brief          print a bignum string of  mbedtls*
 */
void mbedtls_mpi_print(const mbedtls_mpi *X, const char *name);

#define MPI_PRINT(x) mbedtls_mpi_print(x, #x)

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      sm2 drivers */
/** @{ */ /** <!-- [sm2] */

/*
 * Make group available from embedded constants
 */
static int ecp_group_load_sm2(mbedtls_ecp_group *grp)
{
    int ret;

    check_exit(mbedtls_mpi_read_string(&grp->P, RADIX_HEX, SM256R1_P));
    check_exit(mbedtls_mpi_read_string(&grp->A, RADIX_HEX, SM256R1_A));
    check_exit(mbedtls_mpi_read_string(&grp->B, RADIX_HEX, SM256R1_B));
    check_exit(mbedtls_mpi_read_string(&grp->N, RADIX_HEX, SM256R1_N));
    check_exit(mbedtls_mpi_read_string(&grp->G.X, RADIX_HEX, SM256R1_GX));
    check_exit(mbedtls_mpi_read_string(&grp->G.Y, RADIX_HEX, SM256R1_GY));
    check_exit(mbedtls_mpi_lset(&grp->G.Z, 1));

    grp->pbits = mbedtls_mpi_bitlen(&grp->P);
    grp->nbits = mbedtls_mpi_bitlen(&grp->N);

    grp->h = 0;

exit__:
    return (ret);
}

/*
 * Derive a suitable integer for group grp from a buffer of length len
 * SEC1 4.1.3 step 5 aka SEC1 4.1.4 step 3
 */
static int derive_mpi(const mbedtls_ecp_group *grp, mbedtls_mpi *x,
                      const unsigned char *buf, size_t blen)
{
    int ret;
    size_t n_size = (grp->nbits + BITS_IN_BYTE - 1) / BITS_IN_BYTE;
    size_t use_size = blen > n_size ? n_size : blen;

    check_exit(mbedtls_mpi_read_binary(x, buf, use_size));
    if (use_size * BITS_IN_BYTE > grp->nbits) {
        check_exit(mbedtls_mpi_shift_r(x, use_size * BITS_IN_BYTE - grp->nbits));
    }

    /* While at it, reduce modulo N */
    if (mbedtls_mpi_cmp_mpi(x, &grp->N) >= 0) {
        check_exit(mbedtls_mpi_sub_mpi(x, x, &grp->N));
    }

exit__:
    return (ret);
}

int sm2_sign(mbedtls_mpi *r, mbedtls_mpi *s, mbedtls_mpi *d, /* Private Key */
             unsigned char *buf, size_t blen,
             int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = HI_FAILURE;
    int key_tries = 0;
    int sign_tries = 0;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R;
    mbedtls_mpi k;
    mbedtls_mpi e;
    mbedtls_mpi rk;
    mbedtls_mpi mdA;
    mbedtls_mpi inv;

    hi_log_func_enter();

    if ((buf == NULL) || (blen == 0)) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    if (mbedtls_mpi_cmp_int(d, 0) == 0) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    mbedtls_mpi_init(&k);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&rk);
    mbedtls_mpi_init(&mdA);
    mbedtls_mpi_init(&inv);
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&R);

    check_exit(ecp_group_load_sm2(&grp));

    do {
        /* Steps 1: generate a randnum k, 1<=k<=n-1 */
        check_exit(mbedtls_mpi_fill_random(&k, mbedtls_mpi_size(&grp.N), f_rng, p_rng));

        /* Step 2: compute R = [k]G */
        check_exit(mbedtls_ecp_mul(&grp, &R, &k, &grp.G, NULL, NULL));

        /* Step 3: derive MPI from hashed message */
        check_exit(derive_mpi(&grp, &e, buf, blen));

        /* Step 4: compute r = e + Rx mod n */
        check_exit(mbedtls_mpi_add_mpi(r, &e, &R.X));
        check_exit(mbedtls_mpi_mod_mpi(r, r, &grp.N));

        if (key_tries++ > KEY_MAX_TRIES) {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto exit__;
        }

        if (mbedtls_mpi_cmp_int(r, 0) == 0) {
            continue;
        }

        /* Step 5: compute rk = r + k mod n */
        check_exit(mbedtls_mpi_add_mpi(&rk, r, &k));

        if (mbedtls_mpi_cmp_int(&rk, 0) == 0) {
            continue;
        }

        /* Step 6: compute mdA = k - r.dA */
        check_exit(mbedtls_mpi_mul_mpi(&mdA, r, d));
        check_exit(mbedtls_mpi_sub_mpi(&mdA, &k, &mdA));

        /* Step 7: compute inv = (1 + dA)^-1 mod n */
        check_exit(mbedtls_mpi_add_int(&inv, d, 1));
        check_exit(mbedtls_mpi_inv_mod(&inv, &inv, &grp.N));

        /* Step 8: compute s = inv * mdA mod n */
        check_exit(mbedtls_mpi_mul_mpi(s, &inv, &mdA));
        check_exit(mbedtls_mpi_mod_mpi(s, s, &grp.N));

        if (sign_tries++ > KEY_MAX_TRIES) {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto exit__;
        }
    } while (mbedtls_mpi_cmp_int(s, 0) == 0);

exit__:
    mbedtls_mpi_free(&k);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&rk);
    mbedtls_mpi_free(&mdA);
    mbedtls_mpi_free(&inv);
    mbedtls_ecp_point_free(&R);
    mbedtls_ecp_group_free(&grp);

    hi_log_func_exit();

    return (ret);
}

int sm2_verify(unsigned char *buf, size_t blen, mbedtls_ecp_point *Q, /* Public key */
               mbedtls_mpi *r, mbedtls_mpi *s)
{
    int ret;
    mbedtls_mpi e, t, R;
    mbedtls_ecp_point P;
    mbedtls_ecp_group grp;

    hi_log_func_enter();

    if (mbedtls_ecp_is_zero(Q)) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    if (mbedtls_mpi_cmp_int(r, 0) == 0) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    if (mbedtls_mpi_cmp_int(s, 0) == 0) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&t);
    mbedtls_mpi_init(&R);
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&P);

    check_exit(ecp_group_load_sm2(&grp));
    check_exit(mbedtls_mpi_read_binary(&e, buf, blen));

    /*
     * Step 1: make sure r and s are in range 1..n-1
     */
    if (mbedtls_mpi_cmp_int(r, 1) < 0 || mbedtls_mpi_cmp_mpi(r, &grp.N) >= 0 ||
        mbedtls_mpi_cmp_int(s, 1) < 0 || mbedtls_mpi_cmp_mpi(s, &grp.N) >= 0) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto exit__;
    }

    /*
     * Additional precaution: make sure Q is valid
     */
    check_exit(mbedtls_ecp_check_pubkey(&grp, Q));

    /*
     * Step 3: derive MPI from hashed message
     */
    check_exit(derive_mpi(&grp, &e, buf, blen));

    /*
     * Step 4: t = (r+s) mod n
     */
    check_exit(mbedtls_mpi_add_mpi(&t, r, s));
    check_exit(mbedtls_mpi_mod_mpi(&t, &t, &grp.N));

    /*
     * Step 5: P = [s]G + [t]Q
     *
     * Since we're not using any secret data, no need to pass a RNG to
     * mbedtls_ecp_mul() for countermesures.
     */
    check_exit(mbedtls_ecp_muladd(&grp, &P, s, &grp.G, &t, Q));

    if (mbedtls_ecp_is_zero(&P)) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto exit__;
    }

    /*
     * Step 6: convert xR to an integer (no-op)
     * Step 7: R = (e+Px) mod n
     */
    check_exit(mbedtls_mpi_add_mpi(&R, &e, &P.X));
    check_exit(mbedtls_mpi_mod_mpi(&R, &R, &grp.N));

    /*
     * Step 8: check if v (that is, R.X) is equal to r
     */
    if (mbedtls_mpi_cmp_mpi(&R, r) != 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto exit__;
    }

exit__:

    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&t);
    mbedtls_mpi_free(&R);
    mbedtls_ecp_point_free(&P);
    mbedtls_ecp_group_free(&grp);

    hi_log_func_exit();

    return (ret);
}

/* k[1,n-1], R = [k]G, Z = [k]Q */
int sm2_encrypt(mbedtls_ecp_point *Z, mbedtls_ecp_point *Q, /* Public key */ mbedtls_ecp_point *R,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    int key_tries = 0;
    mbedtls_mpi k;
    mbedtls_ecp_group grp;

    hi_log_func_enter();

    if (mbedtls_ecp_is_zero(Q)) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    /*
        mbedtls_mpi_print (&Q->X, "Q->X");
        mbedtls_mpi_print (&Q->Y, "Q->Y");
        mbedtls_mpi_print (&Q->Z, "Q->Z");
 */
    mbedtls_mpi_init(&k);
    mbedtls_ecp_group_init(&grp);

    check_exit(ecp_group_load_sm2(&grp));

    do {
        if (key_tries++ > SM2_TRY_CNT) {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto exit__;
        }

        /*
         * Steps 1: generate a randnum k, 1<=k<=n-1
         */
        check_exit(mbedtls_mpi_fill_random(&k, mbedtls_mpi_size(&grp.N), f_rng, p_rng));

        /*
         * Step 2: compute R = [k]G
         */
        check_exit(mbedtls_ecp_mul(&grp, R, &k, &grp.G, NULL, NULL));
        if (mbedtls_ecp_is_zero(R)) {
            continue;
        }

        /*
         * Additional precaution: make sure Q is valid
         */
        check_exit(mbedtls_ecp_check_pubkey(&grp, Q));

        /*
         * Step 3: compute Z = [k]Q
         */
        check_exit(mbedtls_ecp_mul(&grp, Z, &k, Q, NULL, NULL));

    } while (mbedtls_ecp_is_zero(Z));

exit__:

    mbedtls_mpi_free(&k);
    mbedtls_ecp_group_free(&grp);

    hi_log_func_exit();

    return (ret);
}

/* Z = [d]R */
int sm2_decrypt(mbedtls_ecp_point *Z, mbedtls_mpi *d, /* Private key */ mbedtls_ecp_point *R)
{
    int ret;
    mbedtls_mpi u1, u2;
    mbedtls_ecp_group grp;

    hi_log_func_enter();

    if (mbedtls_mpi_cmp_int(d, 0) == 0) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    if (mbedtls_ecp_is_zero(R)) {
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    mbedtls_mpi_init(&u1);
    mbedtls_mpi_init(&u2);
    mbedtls_ecp_group_init(&grp);

    check_exit(ecp_group_load_sm2(&grp));

    check_exit(mbedtls_mpi_mul_mpi(&u1, &R->X, &R->X));   /* *** u1 = Rx*Rx *** */
    check_exit(mbedtls_mpi_mul_mpi(&u1, &u1, &R->X));     /* *** u1 = u1 * Rx *** */
    check_exit(mbedtls_mpi_mul_mpi(&u2, &grp.A, &R->X));  /* *** u2 = aRx *** */
    check_exit(mbedtls_mpi_add_mpi(&u1, &u1, &u2));       /* *** u1 = u1 + u2 *** */
    check_exit(mbedtls_mpi_add_mpi(&u1, &u1, &grp.B));    /* *** u1 = u1 + b *** */
    check_exit(mbedtls_mpi_mul_mpi(&u2, &R->Y, &R->Y));   /* *** u2 = Ry*Ry *** */
    check_exit(mbedtls_mpi_mod_mpi(&u1, &u1, &grp.P));    /* *** u1 = u1 mod p *** */
    check_exit(mbedtls_mpi_mod_mpi(&u2, &u2, &grp.P));    /* *** u2 = u2 mod p *** */
    if (mbedtls_mpi_cmp_mpi(&u1, &u2) != 0) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto exit__;
    }

    /* Step 2: compute Z = [d]R */
    check_exit(mbedtls_ecp_mul(&grp, Z, d, R, NULL, NULL));
    if (mbedtls_ecp_is_zero(Z)) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto exit__;
    }

exit__:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&u1);
    mbedtls_mpi_free(&u2);

    hi_log_func_exit();

    return (ret);
}

hi_s32 mbedtls_sm2_sign(hi_u32 e[SM2_LEN_IN_WROD], hi_u32 d[SM2_LEN_IN_WROD],
                        hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD], hi_u32 length)
{
    int ret = HI_SUCCESS;
    mbedtls_mpi md;
    mbedtls_mpi mr;
    mbedtls_mpi ms;

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    mbedtls_mpi_init(&md);
    mbedtls_mpi_init(&mr);
    mbedtls_mpi_init(&ms);

    check_exit(mbedtls_mpi_read_binary(&md, (HI_U8 *)d, SM2_LEN_IN_BYTE));
    check_exit(sm2_sign(&mr, &ms, &md, (hi_u8 *)e, SM2_LEN_IN_BYTE,
                        mbedtls_get_random, HI_NULL));

    check_exit(mbedtls_mpi_write_binary(&mr, (hi_u8 *)r, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_write_binary(&ms, (hi_u8 *)s, SM2_LEN_IN_BYTE));

exit__:
    mbedtls_mpi_free(&md);
    mbedtls_mpi_free(&mr);
    mbedtls_mpi_free(&ms);

    hi_log_func_exit();

    return ret;
}

/***************************************
  1. t=(r+s)mod n, if t==0, return fail
  2. (x1,y1)=[s]G+tP,
  3. r=(e+x1)mod n, if r==R, return pass
****************************************/
hi_s32 mbedtls_sm2_verify(hi_u32 e[SM2_LEN_IN_WROD],
                          hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                          hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD], hi_u32 length)
{
    int ret = HI_FAILURE;
    mbedtls_mpi mr;
    mbedtls_mpi ms;
    mbedtls_ecp_point Q;

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    mbedtls_mpi_init(&mr);
    mbedtls_mpi_init(&ms);
    mbedtls_ecp_point_init(&Q);

    check_exit(mbedtls_mpi_read_binary(&mr, (hi_u8 *)r, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_read_binary(&ms, (hi_u8 *)s, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_read_binary(&Q.X, (HI_U8 *)px, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_read_binary(&Q.Y, (HI_U8 *)py, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_lset(&Q.Z, 1));
    check_exit(sm2_verify((hi_u8 *)e, SM2_LEN_IN_BYTE, &Q, &mr, &ms));

exit__:
    mbedtls_mpi_free(&mr);
    mbedtls_mpi_free(&ms);
    mbedtls_ecp_point_free(&Q);

    hi_log_func_exit();

    return ret;
}

/* Compute C1(x, y) = k * G(x, y), XY = k * P(x, y) */
hi_s32 mbedtls_sm2_encrypt(hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                           hi_u32 c1x[SM2_LEN_IN_WROD], hi_u32 c1y[SM2_LEN_IN_WROD],
                           hi_u32 x2[SM2_LEN_IN_WROD], hi_u32 y2[SM2_LEN_IN_WROD], hi_u32 length)
{
    int ret = HI_SUCCESS;
    mbedtls_ecp_point Q;
    mbedtls_ecp_point R;
    mbedtls_ecp_point Z;

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    mbedtls_ecp_point_init(&Q);
    mbedtls_ecp_point_init(&Z);
    mbedtls_ecp_point_init(&R);

    check_exit(mbedtls_mpi_read_binary(&Q.X, (HI_U8 *)px, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_read_binary(&Q.Y, (HI_U8 *)py, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_lset(&Q.Z, 1));

    check_exit(sm2_encrypt(&Z, &Q, &R, mbedtls_get_random, HI_NULL));

    check_exit(mbedtls_mpi_write_binary(&R.X, (HI_U8 *)c1x, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_write_binary(&R.Y, (HI_U8 *)c1y, SM2_LEN_IN_BYTE));

    check_exit(mbedtls_mpi_write_binary(&Z.X, (HI_U8 *)x2, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_write_binary(&Z.Y, (HI_U8 *)y2, SM2_LEN_IN_BYTE));

exit__:
    mbedtls_ecp_point_free(&Q);
    mbedtls_ecp_point_free(&R);
    mbedtls_ecp_point_free(&Z);

    hi_log_func_exit();

    return ret;
}

/* Compute M(x, y) = C1(x, y) * d */
hi_s32 mbedtls_sm2_decrypt(hi_u32 d[SM2_LEN_IN_WROD],
                           hi_u32 c1x[SM2_LEN_IN_WROD], hi_u32 c1y[SM2_LEN_IN_WROD],
                           hi_u32 x2[SM2_LEN_IN_WROD], hi_u32 y2[SM2_LEN_IN_WROD], hi_u32 length)
{
    hi_s32 ret = HI_SUCCESS;
    mbedtls_mpi md;
    mbedtls_ecp_point R;
    mbedtls_ecp_point Z;

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    mbedtls_mpi_init(&md);
    mbedtls_ecp_point_init(&Z);
    mbedtls_ecp_point_init(&R);

    check_exit(mbedtls_mpi_read_binary(&md, (HI_U8 *)d, SM2_LEN_IN_BYTE));

    /* Import hi_u8 array C1 ro point R */
    check_exit(mbedtls_mpi_read_binary(&R.X, (HI_U8 *)c1x, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_read_binary(&R.Y, (HI_U8 *)c1y, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_lset(&R.Z, 1));

    /* compute Z = [d]R */
    check_exit(sm2_decrypt(&Z, &md, &R));

    /* Export R to hi_u8 array C1(x2,y2) */
    check_exit(mbedtls_mpi_write_binary(&Z.X, (HI_U8 *)x2, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_write_binary(&Z.Y, (HI_U8 *)y2, SM2_LEN_IN_BYTE));

exit__:
    mbedtls_mpi_free(&md);
    mbedtls_ecp_point_free(&R);
    mbedtls_ecp_point_free(&Z);

    hi_log_func_exit();

    return ret;
}

/* Compute P(x,y) = d * G(x,y) */
hi_s32 mbedtls_sm2_gen_key(hi_u32 d[SM2_LEN_IN_WROD],
                           hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD], hi_u32 length)
{
    hi_s32 ret = HI_SUCCESS;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi m;
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_u32 random[SM2_TRY_CNT][SM2_LEN_IN_WROD];

    hi_log_func_enter();

    hi_log_check_length(length != SM2_LEN_IN_BYTE);

    mbedtls_mpi_init(&m);
    mbedtls_ecp_point_init(&Q);
    mbedtls_ecp_group_init(&grp);

    check_exit(ecp_group_load_sm2(&grp));

    hi_log_info("Step 1. generate randnum d, 1<=k<=n-2");
    for (i = 0; i < SM2_TRY_CNT; i++) {
        check_exit(cryp_sm2_get_randnum(random[i], sm2n1, sizeof(sm2n1)));
    }
    cryp_trng_get_random(&j, -1);
    j %= SM2_TRY_CNT;
    check_exit(memcpy_s(d, SM2_LEN_IN_BYTE, random[j], SM2_LEN_IN_BYTE));

    hi_log_info("Step 2. PA=dA*G");

    /*
     * Step 2: P = d * G
 */
    check_exit(mbedtls_mpi_read_binary(&m, (HI_U8 *)d, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_ecp_mul(&grp, &Q, &m, &grp.G, NULL, NULL));
    if (mbedtls_ecp_is_zero(&Q)) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto exit__;
    }

    check_exit(mbedtls_mpi_write_binary(&Q.X, (HI_U8 *)px, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_write_binary(&Q.Y, (HI_U8 *)py, SM2_LEN_IN_BYTE));
    check_exit(mbedtls_mpi_lset(&Q.Z, 1));

exit__:
    mbedtls_mpi_free(&m);
    mbedtls_ecp_point_free(&Q);
    mbedtls_ecp_group_free(&grp);

    hi_log_func_exit();

    return ret;
}

#endif
/** @} */ /** <!-- ==== API Code end ==== */

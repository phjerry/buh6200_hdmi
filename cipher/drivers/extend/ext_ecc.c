/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of ext_ecc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/rsa.h"

#define RSA_KEY_EXPONENT_VALUE1 (0X3)
#define RSA_KEY_EXPONENT_VALUE2 (0X10001)

/**
* \brief          print a bignum string of  mbedtls*
*/
void mbedtls_mpi_print(const mbedtls_mpi *X, const char *name);

int mbedtls_get_random(void *param, hi_u8 *rand, size_t size);

#define MPI_PRINT(x) mbedtls_mpi_print(x, #x)

#ifdef HI_PRODUCT_RSA_SUPPORT
hi_s32 ext_rsa_compute_crt(cryp_rsa_key *key)
{
    hi_s32 ret = HI_FAILURE;
    mbedtls_mpi E, D, P, Q, DP, DQ, QP, P1, Q1, H;

    hi_log_func_enter();

    hi_log_check_param(key->p == HI_NULL);
    hi_log_check_param(key->q == HI_NULL);
    hi_log_check_param(key->dp == HI_NULL);
    hi_log_check_param(key->dq == HI_NULL);
    hi_log_check_param(key->qp == HI_NULL);
    hi_log_check_param(key->klen == 0);
    hi_log_check_param(key->e == 0);

    if ((key->klen != RSA_KEY_BITWIDTH_1024) && (key->klen != RSA_KEY_BITWIDTH_2048)
        && (key->klen != RSA_KEY_BITWIDTH_3072) && (key->klen != RSA_KEY_BITWIDTH_4096)) {
        hi_log_error("RSA compute crt params, bitnum error\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    if ((key->e != RSA_KEY_EXPONENT_VALUE1) && (key->e != RSA_KEY_EXPONENT_VALUE2)) {
        hi_log_error("RSA compute crt params, e error\n");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    mbedtls_mpi_init(&E);
    mbedtls_mpi_init(&D);
    mbedtls_mpi_init(&P);
    mbedtls_mpi_init(&Q);
    mbedtls_mpi_init(&DP);
    mbedtls_mpi_init(&DQ);
    mbedtls_mpi_init(&QP);
    mbedtls_mpi_init(&P1);
    mbedtls_mpi_init(&Q1);
    mbedtls_mpi_init(&H);

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&E, key->e));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&P, key->p, key->klen >> 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q, key->q, key->klen >> 1));

    /* D  = E^-1 mod ((P-1)*(Q-1))
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&P1, &P, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&Q1, &Q, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&H, &P1, &Q1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&D, &E, &H));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&DP, &D, &P1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&DQ, &D, &Q1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&QP, &Q, &P));

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&DP, key->dp, key->klen >> 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&DQ, key->dq, key->klen >> 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&QP, key->qp, key->klen >> 1));

cleanup:

    mbedtls_mpi_free(&E);
    mbedtls_mpi_free(&D);
    mbedtls_mpi_free(&P);
    mbedtls_mpi_free(&Q);
    mbedtls_mpi_free(&DP);
    mbedtls_mpi_free(&DQ);
    mbedtls_mpi_free(&QP);
    mbedtls_mpi_free(&P1);
    mbedtls_mpi_free(&Q1);
    mbedtls_mpi_free(&H);

    hi_log_func_exit();

    return ret;

}
#endif

#ifdef SOFT_ECC_SUPPORT
hi_s32 ext_ecdh_compute_key(ecc_param_t *ecc, hi_u8 *d, hi_u8 *px, hi_u8 *py, hi_u8 *sharekey)
{
    hi_s32 ret = HI_FAILURE;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi z;
    mbedtls_mpi md;

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

    hi_log_check_param(ecc->ksize == 0);

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&grp.P);
    mbedtls_mpi_init(&grp.A);
    mbedtls_mpi_init(&grp.B);
    mbedtls_mpi_init(&grp.N);
    mbedtls_ecp_point_init(&grp.G);
    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&z);
    mbedtls_mpi_init(&md);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.P, ecc->p, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.A, ecc->a, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.B, ecc->b, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.N, ecc->n, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.X, ecc->gx, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.Y, ecc->gy, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp.G.Z, 1));
    grp.pbits = mbedtls_mpi_bitlen(&grp.P);
    grp.nbits = mbedtls_mpi_bitlen(&grp.N);
    grp.h = ecc->h;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q.X, px, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q.Y, py, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&Q.Z, 1));

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&md, d, ecc->ksize));

    MBEDTLS_MPI_CHK(mbedtls_ecdh_compute_shared(&grp, &z, &Q, &md, HI_NULL, 0));

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&z, sharekey, ecc->ksize));
cleanup:
    mbedtls_mpi_free(&grp.P);
    mbedtls_mpi_free(&grp.A);
    mbedtls_mpi_free(&grp.B);
    mbedtls_mpi_free(&grp.N);
    mbedtls_ecp_point_free(&grp.G);
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&z);
    mbedtls_mpi_free(&md);

    hi_log_func_exit();

    return (ret);
}

hi_s32 ext_ecc_gen_key(ecc_param_t *ecc, hi_u8 *inkey, hi_u8 *outkey, hi_u8 *px, hi_u8 *py)
{
    hi_s32 ret = HI_FAILURE;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi d;

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

    hi_log_check_param(ecc->ksize == 0);

    if ((inkey == HI_NULL) && (outkey == HI_NULL)) {
        hi_log_error("Invalid param!");
        hi_log_print_err_code(HI_ERR_CIPHER_INVALID_PARA);
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&grp.P);
    mbedtls_mpi_init(&grp.A);
    mbedtls_mpi_init(&grp.B);
    mbedtls_mpi_init(&grp.N);
    mbedtls_ecp_point_init(&grp.G);
    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&d);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.P, ecc->p, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.A, ecc->a, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.B, ecc->b, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.N, ecc->n, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.X, ecc->gx, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.Y, ecc->gy, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp.G.Z, 1));
    grp.pbits = mbedtls_mpi_bitlen(&grp.P);
    grp.nbits = mbedtls_mpi_bitlen(&grp.N);
    grp.h = ecc->h;

    if (inkey == HI_NULL) {
        MBEDTLS_MPI_CHK(mbedtls_ecdh_gen_public(&grp, &d, &Q, mbedtls_get_random, 0));
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&d, outkey, ecc->ksize));
    } else {
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&d, inkey, ecc->ksize));
        if ((mbedtls_mpi_cmp_int(&d, 1) < 0) || (mbedtls_mpi_cmp_mpi(&d, &grp.N) >= 0)) {
            hi_log_error("Invalid private key !!!\n");
            goto cleanup;
        }
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&grp, &Q, &d, &grp.G, HI_NULL, 0));
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&Q.X, px, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&Q.Y, py, ecc->ksize));

cleanup:
    mbedtls_mpi_free(&grp.P);
    mbedtls_mpi_free(&grp.A);
    mbedtls_mpi_free(&grp.B);
    mbedtls_mpi_free(&grp.N);
    mbedtls_ecp_point_free(&grp.G);
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&d);

    hi_log_func_exit();

    return (ret);
}

hi_s32 ext_ecdsa_sign_hash(ecc_param_t *ecc, hi_u8 *d, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s)
{
    hi_s32 ret = HI_FAILURE;
    mbedtls_ecp_group grp;
    mbedtls_mpi mr;
    mbedtls_mpi ms;
    mbedtls_mpi md;

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

    hi_log_check_param(ecc->ksize == 0);

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&grp.P);
    mbedtls_mpi_init(&grp.A);
    mbedtls_mpi_init(&grp.B);
    mbedtls_mpi_init(&grp.N);
    mbedtls_ecp_point_init(&grp.G);
    mbedtls_mpi_init(&mr);
    mbedtls_mpi_init(&ms);
    mbedtls_mpi_init(&md);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.P, ecc->p, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.A, ecc->a, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.B, ecc->b, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.N, ecc->n, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.X, ecc->gx, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.Y, ecc->gy, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp.G.Z, 1));
    grp.pbits = mbedtls_mpi_bitlen(&grp.P);
    grp.nbits = mbedtls_mpi_bitlen(&grp.N);
    grp.h = ecc->h;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&md, d, ecc->ksize));

    MBEDTLS_MPI_CHK(mbedtls_ecdsa_sign(&grp, &mr, &ms, &md, hash, hlen, mbedtls_get_random, 0));

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&mr, r, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ms, s, ecc->ksize));

cleanup:
    mbedtls_mpi_free(&grp.P);
    mbedtls_mpi_free(&grp.A);
    mbedtls_mpi_free(&grp.B);
    mbedtls_mpi_free(&grp.N);
    mbedtls_ecp_point_free(&grp.G);
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&mr);
    mbedtls_mpi_free(&ms);
    mbedtls_mpi_free(&md);

    hi_log_func_exit();

    return (ret);
}

hi_s32 ext_ecdsa_verify_hash(ecc_param_t *ecc, hi_u8 *px, hi_u8 *py, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s)
{
    hi_s32 ret = HI_FAILURE;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi mr;
    mbedtls_mpi ms;

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

    hi_log_check_param(ecc->ksize == 0);

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&grp.P);
    mbedtls_mpi_init(&grp.A);
    mbedtls_mpi_init(&grp.B);
    mbedtls_mpi_init(&grp.N);
    mbedtls_ecp_point_init(&grp.G);
    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&mr);
    mbedtls_mpi_init(&ms);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.P, ecc->p, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.A, ecc->a, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.B, ecc->b, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.N, ecc->n, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.X, ecc->gx, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&grp.G.Y, ecc->gy, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&grp.G.Z, 1));
    grp.pbits = mbedtls_mpi_bitlen(&grp.P);
    grp.nbits = mbedtls_mpi_bitlen(&grp.N);
    grp.h = ecc->h;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q.X, px, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&Q.Y, py, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&Q.Z, 1));

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&mr, r, ecc->ksize));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ms, s, ecc->ksize));

    MBEDTLS_MPI_CHK(mbedtls_ecdsa_verify(&grp, hash, hlen, &Q, &mr, &ms));

cleanup:
    mbedtls_mpi_free(&grp.P);
    mbedtls_mpi_free(&grp.A);
    mbedtls_mpi_free(&grp.B);
    mbedtls_mpi_free(&grp.N);
    mbedtls_ecp_point_free(&grp.G);
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&mr);
    mbedtls_mpi_free(&ms);

    hi_log_func_exit();

    return(ret);
}

#endif


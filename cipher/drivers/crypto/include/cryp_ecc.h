/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of cryp_ecc
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __CRYP_ECC_H_
#define __CRYP_ECC_H_

#include "drv_osal_lib.h"
#include "drv_pke.h"

/******************************* API Declaration *****************************/
/** \addtogroup      ecc */
/** @{ */ /** <!--[ecc] */

#define ECC_TRY_CNT 8

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/**
* \brief          Initialize crypto of ecc *
 */
hi_s32 cryp_ecc_init(void);

/**
* \brief          Deinitialize crypto of ecc *
 */
void cryp_ecc_deinit(void);

/**
\brief Generate a ECC key pair.
 */
typedef hi_s32 (*func_ecc_gen_ke)(ecc_param_t *ecc, hi_u8 *inkey, hi_u8 *outkey, hi_u8 *px, hi_u8 *py);

/**
\brief Compute ECDH shared secret key. This key corresponds to the X coordinates of the computed P point..
\      Compute sharekey = d * P(x, y)
 */
typedef hi_s32 (*func_ecdh_compute_key)(ecc_param_t *ecc, hi_u8 *d, hi_u8 *px, hi_u8 *py, hi_u8 *sharekey);

/**
\brief ECC sign a hash value with a ECC private key.
\  1. generate a randnum k, k within [1,n-1],
\  2. (x1,y1)=[k]G, e = MSB(H)
\  3. r= x1 mod n, if r=0 or r+k=n, return step 1
\  4. s=(k^-1 * (e + r * d))mod n
 */
typedef hi_s32 (*func_ecdsa_sign_hash)(ecc_param_t *ecc, hi_u8 *d, hi_u8 *hash, hi_u32 hlen, hi_u8 *r,
               hi_u8 *s);

/**
* \brief          get randnum for sm2 *
 */
void cryp_ecc_get_randnum(hi_u8 *randnum, const hi_u8 *max, hi_u32 klen);

/**
* \brief check val whether zero or not
 */
hi_u32 cryp_ecc_is_zero(const void *val, hi_u32 klen);

/**
* \brief compare 2 val
 */
hi_s32 cryp_ecc_cmp(const void *val1, const void *val2, hi_u32 klen);

/**
* \brief  check val whether less than max or not
 */
hi_s32 cryp_ecc_rang_check(const hi_u8 *val, const hi_u8 *max, hi_u32 klen);

/**
\brief ECC verify a sm3 value with a ECC public key.
\  1. t=(r+s)mod n, if t==0, return fail
\  2. (x1,y1)=[s]G+tP,
\  3. r=(e+x1)mod n, if r==R, return pass
 */
typedef hi_s32 (*func_ecdsa_verify_hash)(ecc_param_t *ecc, hi_u8 *px, hi_u8 *py, hi_u8 *hash, hi_u32 hlen,
               hi_u8 *r, hi_u8 *s);

/* ! \struct of ECC function template */
typedef struct {
    func_ecdh_compute_key ecdh;
    func_ecdsa_sign_hash sign;
    func_ecdsa_verify_hash verify;
    func_ecc_gen_ke genkey;
} ecc_func;

/**
\brief  Clone the function from template of ecc engine.
\retval     On success, func is returned.  On error, HI_NULL is returned.
 */
ecc_func *cryp_get_ecc_op(void);

/** @} */ /** <!-- ==== API declaration end ==== */
#endif

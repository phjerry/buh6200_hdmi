/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_pke
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __DRV_PKE_H__
#define __DRV_PKE_H__

/*************************** Structure Definition ****************************/
/** \addtogroup     pke */
/** @{ */ /** <!-- [pke] */

/* ! Define RSA 1024 key length */
#define RSA_KEY_LEN_1024 128

/* ! Define RSA 1024 key length */
#define RSA_KEY_LEN_2048 256

/* ! Define RSA 1024 key length */
#define RSA_KEY_LEN_3072 384

/* ! Define RSA 1024 key length */
#define RSA_KEY_LEN_4096 512

/* ! \rsa key width */
typedef enum {
    RSA_KEY_WIDTH_1024 = 0x00, /* !<  RSA 1024 */
    RSA_KEY_WIDTH_2048,        /* !<  RSA 2048 */
    RSA_KEY_WIDTH_4096,        /* !<  RSA 4096 */
    RSA_KEY_WIDTH_3072,        /* !<  RSA 3072 */
    RSA_KEY_WIDTH_COUNT,
} rsa_key_width;

/* ! \pke capacity, 0-nonsupport, 1-support */
typedef struct {
    hi_u32 sm2 : 1; /* !<  Support SM2 */
    hi_u32 ecc : 1; /* !<  Support ECC */
    hi_u32 rsa : 1; /* !<  Support RSA */
} pke_capacity;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      pke */
/** @{ */ /** <!--[pke] */

/* ! Define the length of zero padding for mul-dot */
#define PKE_LEN_BLOCK_IN_WOED 0x02
#define PKE_LEN_BLOCK_IN_BYTE 0x08
#define PKE_LEN_BLOCK_MASK    0x07

/* Define the union for sm2 block */
typedef union {
    hi_u8 byte[PKE_LEN_BLOCK_IN_BYTE];
    hi_u32 word[PKE_LEN_BLOCK_IN_WOED];
} pke_block;

/**
\brief  Initialize the pke module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_pke_init(void);

/**
\brief  Deinitialize the pke module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_pke_deinit(void);

/**
\brief  enable the pke module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_pke_resume(void);

/**
\brief  disable the pke module.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
void drv_pke_suspend(void);

/* c = a + b mod p */
hi_s32 drv_pke_add_mod(const hi_u8 *a, const hi_u8 *b, const hi_u8 *p, hi_u8 *c, hi_u32 size);

/* c = a - b mod p */
hi_s32 drv_pke_sub_mod(const hi_u8 *a, const hi_u8 *b, const hi_u8 *p, hi_u8 *c, hi_u32 size);

/* c = a * b mod p */
hi_s32 drv_pke_mul_mod(const hi_u8 *a, const hi_u8 *b, const hi_u8 *p, hi_u8 *c, hi_u32 size, hi_u32 private);

/* c = a^-1 mod p */
hi_s32 drv_pke_inv_mod(const hi_u8 *a, const hi_u8 *p, hi_u8 *c, hi_u32 size);

/* c = a mod p */
hi_s32 drv_pke_mod(const hi_u8 *a, const hi_u8 *p, hi_u8 *c, hi_u32 size);

/* c = a mod p */
hi_s32 drv_pke_mod_block(const pke_block *a, hi_u32 power, const hi_u8 *p, hi_u32 plen, hi_u8 *c);

/* rsa key generation with public key */
hi_s32 drv_rsa_pub_key(hi_u32 e, hi_u32 klen, cryp_rsa_key *key);

/* c = a * b */
hi_s32 drv_pke_mul(const hi_u8 *a, const hi_u8 *b, hi_u8 *c, hi_u32 size);

/* R = k . P(x,y) */
hi_s32 drv_pke_mul_dot(const hi_u8 *k,
                       const hi_u8 *px,
                       const hi_u8 *py,
                       hi_u8 *rx,
                       hi_u8 *ry,
                       hi_u32 size,
                       hi_u32 private,
                       ecc_param_t *ecp_id);

/* C = S(x,y) + R(x,y) */
hi_s32 drv_pke_add_dot(const hi_u8 *sx,
                       const hi_u8 *sy,
                       const hi_u8 *rx,
                       const hi_u8 *ry,
                       hi_u8 *cx,
                       hi_u8 *cy,
                       hi_u32 size,
                       ecc_param_t *ecp_id);

/**
\brief  clean pke ram .
 */
hi_s32 drv_pke_clean_ram(void);

/**
\brief  c=m^e mod n.
 */
hi_s32 drv_ifep_rsa_exp_mod(hi_u8 *n, hi_u8 *k, hi_u8 *in, hi_u8 *out, rsa_key_width width);

/**
\brief  get the pke capacity.
\param[out] capacity The hash capacity.
\retval     NA.
 */
void drv_pke_get_capacity(pke_capacity *capacity);

/** @} */ /** <!-- ==== API declaration end ==== */

#endif

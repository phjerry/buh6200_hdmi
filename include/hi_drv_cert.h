/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: cert driver layer general declaration
 * Author: Hisilicon hisecurity team
 * Create: 2016-12-08
 */

#ifndef __HI_DRV_CERT_H__
#define __HI_DRV_CERT_H__

#include "hi_type.h"
#include "hi_unf_cert.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/* Define the algorithm of crypto engine. */
typedef enum {
    HI_CRYPTO_ENGINE_ALG_CSA2 = 0,            /* CSA2.0 */
    HI_CRYPTO_ENGINE_ALG_CSA3,                /* CSA3.0 */
    HI_CRYPTO_ENGINE_ALG_ASA,                 /* ASA 64/128 Algorithm */
    HI_CRYPTO_ENGINE_ALG_ASA_LIGHT,           /* ASA light Algorithm */

    HI_CRYPTO_ENGINE_ALG_AES_ECB_T = 0x10,    /* SPE AES ECB, the clear stream left in the tail */
    HI_CRYPTO_ENGINE_ALG_AES_ECB_L,           /* AES_ECB_L the clear stream left in the leading */

    HI_CRYPTO_ENGINE_ALG_AES_CBC_T,           /* AES CBC, the clear stream left in the tail */
    HI_CRYPTO_ENGINE_ALG_AES_CISSA,           /* Common IPTV Software-oriented Scrambling Algorithm(CISSA), golbal IV */
    HI_CRYPTO_ENGINE_ALG_AES_CBC_L,           /* AES_CBC_L the clear stream left in the leading */

    HI_CRYPTO_ENGINE_ALG_AES_CBC_IDSA,        /* AES128 CBC Payload, ATIS IIF Default Scrambling Algorithm (IDSA),
                                                 the difference between AES_CBC_IDSA and AES_IPTV is AES_CBC_IDSA
                                                 only support 0 IV */
    HI_CRYPTO_ENGINE_ALG_AES_IPTV,            /* AES IPTV of SPE */
    HI_CRYPTO_ENGINE_ALG_AES_CTR,             /* AES CTR */

    HI_CRYPTO_ENGINE_ALG_DES_CI = 0x20,       /* DES CBC */
    HI_CRYPTO_ENGINE_ALG_DES_CBC,             /* DES CBC */
    HI_CRYPTO_ENGINE_ALG_DES_CBC_IDSA,        /* DES CBC Payload, ATIS IIF Default Scrambling Algorithm(IDSA),
                                                 Not support set IV */

    HI_CRYPTO_ENGINE_ALG_SMS4_ECB = 0x30,     /* SMS4 ECB */
    HI_CRYPTO_ENGINE_ALG_SMS4_CBC,            /* SMS4 CBC */
    HI_CRYPTO_ENGINE_ALG_SMS4_CBC_IDSA,       /* SMS4 CBC Payload, ATIS IIF Default Scrambling Algorithm (IDSA)
                                                 Not support set IV */

    HI_CRYPTO_ENGINE_ALG_TDES_ECB = 0x40,     /* TDES ECB */
    HI_CRYPTO_ENGINE_ALG_TDES_CBC,            /* TDES CBC */
    HI_CRYPTO_ENGINE_ALG_TDES_CBC_IDSA,       /* TDES CBC Payload, ATIS IIF Default Scrambling Algorithm (IDSA)
                                                 Not support set IV */

    HI_CRYPTO_ENGINE_ALG_MULTI2_ECB = 0x50,   /* MULTI2 ECB */
    HI_CRYPTO_ENGINE_ALG_MULTI2_CBC,          /* MULTI2 CBC */
    HI_CRYPTO_ENGINE_ALG_MULTI2_CBC_IDSA,     /* MULTI2 CBC Payload, ATIS IIF Default Scrambling Algorithm (IDSA)
                                                 Not support set IV */

    HI_CRYPTO_ENGINE_ALG_RAW_AES = 0x4000,
    HI_CRYPTO_ENGINE_ALG_RAW_DES,
    HI_CRYPTO_ENGINE_ALG_RAW_SM4,
    HI_CRYPTO_ENGINE_ALG_RAW_TDES,
    HI_CRYPTO_ENGINE_ALG_RAW_HMAC_SHA1,
    HI_CRYPTO_ENGINE_ALG_RAW_HMAC_SHA2,
    HI_CRYPTO_ENGINE_ALG_RAW_HMAC_SM3,
    HI_CRYPTO_ENGINE_ALG_RAW_HDCP,

    HI_CRYPTO_ENGINE_ALG_MAX
} hi_crypto_engine_alg;

/* Input and Output data Num */

#define DATA_NUM                      8
#define DATA_LEN                      4

/* Engine type */
typedef enum {
    HI_CERT_KEY_PORT_TSCIPHER     = 0x0,   /* TS Cipher */
    HI_CERT_KEY_PORT_MCIPHER      = 0x1,   /* MCipher */
    HI_CERT_KEY_PORT_MAX          = 0x2,
} hi_cert_key_port_sel;

/* Timeout type */
typedef enum {
    HI_CERT_TIMEOUT_DEFAULT, /* Default timeout. Indicates that the associated command does not write in OTP. */
    HI_CERT_TIMEOUT_OTP,     /* The associated command is writing in OTP memory. */
    HI_CERT_LAST_TIMEOUT,    /* Other use. */
} hi_cert_timeout;

/* Structure of the CERT exchange information */
typedef struct {
    hi_u8 input_data[DATA_LEN * DATA_NUM];
    hi_u8 output_data[DATA_LEN * DATA_NUM];
    hi_u8 status[DATA_LEN];
    hi_u8 opcodes[DATA_LEN];
    hi_cert_timeout timeout;
} hi_cert_command;

/* Structure of the CERT CTRL information */
typedef struct {
    hi_handle handle;                        /* Target module handle, address information included */
    hi_bool is_even;                         /* Type of key odd or even */
    hi_crypto_engine_alg engine;             /* Target crypto engine */
    hi_cert_key_port_sel port_sel;           /* Port select */
} hi_cert_key_data;

/* Structure of CERT resource */
typedef struct {
    hi_handle res_handle;  /* cert resource handle */
} hi_cert_res_handle;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* End of #ifndef __HI_DRV_CERT_H__*/


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Keyladder drvier level head file.
 * Author: linux SDK team
 * Create: 2019/09/23
 */

#ifndef __DRV_KLAD_STRUCT_H__
#define __DRV_KLAD_STRUCT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*************************** structure definition ****************************/
/** \addtogroup      KLAD */
/** @{ */  /** <!-- [KLAD] */

/*
 * CA vendor id list
 */
#define HI_CA_ID_BASIC               0x0

/*
 * Rootkey type list
 */
typedef enum {
    HI_ROOTKEY_NULL  = 0x0,
    HI_ROOTKEY_CSA2  = 0x1,
    HI_ROOTKEY_CSA3  = 0x2,
    HI_ROOTKEY_AES   = 0x3,
    HI_ROOTKEY_TDES  = 0x4,
    HI_ROOTKEY_SM4   = 0x5,
    HI_ROOTKEY_MISC  = 0x6,
    HI_ROOTKEY_R2R   = 0x7,
    HI_ROOTKEY_HDCP  = 0x8,
    HI_ROOTKEY_DCAS  = 0x9,
    HI_ROOTKEY_DYM   = 0xFF,
} hi_rootkey_type;

/** CA VendorID */
typedef enum {
    HI_CA_VENDORID_COMMON     = 0x00,        /* No-Advcance CA chipset, Marked with 0 */
    HI_CA_VENDORID_NAGRA      = 0x01,        /* NAGRA  Chipse, Marked with R */
    HI_CA_VENDORID_IRDETO     = 0x02,        /* IRDETO Chipset, Marked with I */
    HI_CA_VENDORID_CONAX      = 0x03,        /* CONAX Chipset, Marked with C */
    HI_CA_VENDORID_NDS        = 0x04,        /* NDS Chipset */
    HI_CA_VENDORID_SUMA       = 0x05,        /* SUMA Chipset, Marked with S */
    HI_CA_VENDORID_NOVEL      = 0x06,        /* NOVEL Chipset, Marked with Y */
    HI_CA_VENDORID_VERIMATRIX = 0x07,        /* VERIMATRIX Chipset, Marked with M */
    HI_CA_VENDORID_CTI        = 0x08,        /* CTI Chipset, Marked with T */
    HI_CA_VENDORID_SAFEVIEW   = 0x09,        /* SAFEVIEW CA Chipset */
    HI_CA_VENDORID_LATENSE    = 0x0a,        /* LATENSE CA Chipset */
    HI_CA_VENDORID_SH_TELECOM = 0x0b,        /* SH_TELECOM CA Chipset */
    HI_CA_VENDORID_DCAS       = 0x0c,        /* DCAS CA Chipset */
    HI_CA_VENDORID_VIACCESS   = 0x0d,        /* VIACCESS CA Chipset */
    HI_CA_VENDORID_PANACCESS  = 0x0e,        /* Panaccess CA Chipset */
    HI_CA_VENDORID_ABV        = 0x0f,        /* ABV CA Chipset */
    HI_CA_VENDORID_MAX
} hi_ca_vendor_id;

/*
 * Keyladder type list
 */
typedef enum {
    HI_KLAD_COM   = 0x10,
    HI_KLAD_TA    = 0x11,
    HI_KLAD_FP    = 0x12,
    HI_KLAD_NONCE = 0x13,
    HI_KLAD_CLR   = 0x14,
} hi_klad_type;

#define HI_KLAD_ID(TYPE)             ((TYPE) & 0xff)
#define HI_KLAD_TYPE(TYPE)           (((TYPE) >> 8) & 0xff)
#define HI_RK_TYPE(TYPE)             (((TYPE) >> 16) & 0xff)
#define HI_KLAD_CA(TYPE)             (((TYPE) >> 24) & 0xff)

#define HI_KLAD_INSTANCE(CA, RK, KLAD, ID) \
    ((((CA) << 24) & 0xFF000000) + (((RK) << 16) & 0xFF0000) + (((KLAD) << 8) & 0xFF00) + (ID))

/*
 * Clear route keyladder
 */
#define HI_KLAD_TYPE_CLEARCW     HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_NULL, HI_KLAD_CLR, 0x01)

/*
 * Dynamic keyladder, it can be customized
 */
#define HI_KLAD_TYPE_DYNAMIC     HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_DYM, HI_KLAD_COM, 0x01)

/*
 * OEM Software keyladder
 * 1 stage keyladder
 * Keyladder algorithm use AES, target engine is MCipher and target engine algorithm is AES.
 */
#define HI_KLAD_TYPE_OEM_SW      HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_R2R, HI_KLAD_COM, 0x01)

/*
 * OEM R2R keyladder
 * 1 stage keyladder
 * Keyladder algorithm use AES, target engine is MCipher and target engine algorithm is AES.
 */
#define HI_KLAD_TYPE_OEM_R2R     HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_R2R, HI_KLAD_COM, 0x02)

/*
 * OEM HDCP keyladder
 * 1 stage keyladder
 * Keyladder algorithm use AES, target engine is MCipher and target engine algorithm is AES.
 */
#define HI_KLAD_TYPE_OEM_HDCP    HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_HDCP, HI_KLAD_COM, 0x01)

/*
 * CSA2 keyladder
 * 2 stage keyladder
 * Keyladder algorithm use AES/TDES/SM4, target engine is TSCIPHER and target engine algorithm is CSA2.
 */
#define HI_KLAD_TYPE_CSA2   HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_CSA2, HI_KLAD_COM, 0x01)

/*
 * CSA3 keyladder
 * 2 stage keyladder
 * Keyladder algorithm use AES/TDES/SM4, target engine is TSCIPHER and target engine algorithm is CSA3.
 */
#define HI_KLAD_TYPE_CSA3   HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_CSA3, HI_KLAD_COM, 0x01)

/*
 * R2R keyladder
 * 2 stage keyladder
 * Keyladder algorithm use AES/TDES/SM4, target engine is MCipher and target engine algorithm is AES/TDES/SM4.
 */
#define HI_KLAD_TYPE_R2R   HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_R2R, HI_KLAD_COM, 0x03)

/*
 * SP keyladder
 * 2 stage keyladder
 * Keyladder algorithm use AES/TDES/SM4, target engine is TSCIPHER and target engine algorithm is AES.
 */
#define HI_KLAD_TYPE_SP   HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_AES, HI_KLAD_COM, 0x01)

/*
 * MISC keyladder
 * 2 stage keyladder
 * Keyladder algorithm use AES/TDES/SM4, target engine is TSCIPHER and target engine algorithm is CSA2/CSA3/AES/TDES.
 */
#define HI_KLAD_TYPE_MISC   HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_MISC, HI_KLAD_COM, 0x01)

/* define the maximum session key level */
#define HI_SESSION_KEY_MAX_LEVEL 0x04

/* define the maximum key length. */
#define HI_KLAD_MAX_KEY_LEN      32

/* define the maximum session key length. */
#define HI_KLAD_MAX_SESSION_KEY_LEN      16

#define HI_KLAD_MAX_IV_LEN       16

/* Define the algorithm of crypto engine. */
typedef enum {
    HI_CRYPTO_ENGINE_ALG_CSA2 = 0,            /* CSA2.0 */
    HI_CRYPTO_ENGINE_ALG_CSA3,                /* CSA3.0 */
    HI_CRYPTO_ENGINE_ALG_ASA,                 /* ASA 64/128 Algorithm */

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

/**< Define the key security attribute. */
typedef enum {
    HI_KLAD_SEC_ENABLE = 0,
    HI_KLAD_SEC_DISABLE,
    HI_KLAD_SEC_MAX
} hi_klad_sec;

/* define the keyladder algrithm. */
typedef enum {
    HI_KLAD_ALG_TYPE_DEFAULT   = 0, /* default value */
    HI_KLAD_ALG_TYPE_TDES      = 1,
    HI_KLAD_ALG_TYPE_AES,
    HI_KLAD_ALG_TYPE_SM4,
    HI_KLAD_ALG_TYPE_MAX
} hi_klad_alg_type;

/* define the keyladder level. */
typedef enum {
    HI_KLAD_LEVEL1 = 0,
    HI_KLAD_LEVEL2,
    HI_KLAD_LEVEL3,
    HI_KLAD_LEVEL4,
    HI_KLAD_LEVEL5,
    HI_KLAD_LEVEL6,
    HI_KLAD_LEVEL_MAX
} hi_klad_level;
/* define the structure of keyladder configuration. */
typedef struct {
    hi_u32 owner_id;   /* keyladder owner ID. different keyladder have different ID. */
    hi_u32 klad_type;  /* keyladder type. */
} hi_klad_config;

/* define the structure of content key configurations. */
typedef struct {
    hi_bool decrypt_support;     /* the content key can be used for decrypting. */
    hi_bool encrypt_support;     /* the content key can be used for encrypting. */
    hi_crypto_engine_alg engine; /* the content key can be used for which algorithm of the crypto engine. */
} hi_klad_key_config;

/* define the structure of content key security configurations. */
typedef struct {
    hi_bool dest_buf_sec_support;     /* the destination buffer of target engine can be secure. */
    hi_bool dest_buf_non_sec_support; /* the destination buffer of target engine can be non-secure. */
    hi_bool src_buf_sec_support;      /* the source buffer of target engine can be secure. */
    hi_bool src_buf_non_sec_support;  /* the source buffer of target engine can be non-secure. */
} hi_klad_key_secure_config;

/* structure of keyladder extend attributes. */
typedef struct {
    hi_klad_config klad_cfg;               /* the keyladder configuration. */
    hi_klad_key_config key_cfg;            /* the content key configuration. */
} hi_klad_attr;

/* structure of setting session key. */
typedef struct {
    hi_klad_level level;            /* the level of session key. */
    hi_klad_alg_type alg;           /* the algorithm used to decrypt session key. */
    hi_u32 key_size;                /* the size of session key. */
    hi_u8 key[HI_KLAD_MAX_KEY_LEN]; /* the session key. */
} hi_klad_session_key;

/* structure of setting content key. */
typedef struct {
    hi_bool odd;                    /* odd or even key flag. */
    hi_klad_alg_type alg;           /* the algorithm of the content key. */
    hi_u32 key_size;                /* the size of content key. */
    hi_u8 key[HI_KLAD_MAX_KEY_LEN]; /* the content key. */
} hi_klad_content_key;

/* structure of sending clear key. */
typedef struct {
    hi_bool odd;                    /* odd or even key flag. */
    hi_u32 key_size;                /* the size of content key. */
    hi_u8 key[HI_KLAD_MAX_KEY_LEN]; /* the content key. */
} hi_klad_clear_key;

/* structure of generating keyladder key. */
typedef struct {
    hi_klad_alg_type alg;             /* the algorithm of the content key. */
    hi_u32 key_size;                  /* the size of content key. */
    hi_u8 key[HI_KLAD_MAX_KEY_LEN];
    hi_u32 gen_key_size;              /* the size of generated key. */
    hi_u8 gen_key[HI_KLAD_MAX_KEY_LEN];
} hi_klad_gen_key;

/* structure of setting nonce keyladder key. */
typedef struct {
    hi_klad_alg_type alg;             /* the algorithm of the content key. */
    hi_u32 key_size;                  /* the size of content key. */
    hi_u8 key[HI_KLAD_MAX_KEY_LEN];
    hi_u32 nonce_size;                /* the size of nonce key. */
    hi_u8 nonce[HI_KLAD_MAX_KEY_LEN]; /* the size of nonce key. */
} hi_klad_nonce_key;

typedef enum {
    HI_KLAD_FP_OPT_ENCRYPT      = 0, /* Encrypt operation. */
    HI_KLAD_FP_OPT_DECRYPT,          /* Decrypt operation. */
    HI_KLAD_FP_OPT_ROUTE,            /* Send key to ctypto engine. */
    HI_KLAD_FP_OPT_MAX
} hi_klad_fp_operation;

/* Structure of setting FP keyladder key. */
typedef struct {
    hi_klad_fp_operation operation; /* The operation of flash protection keyladder. */
    hi_klad_alg_type alg; /* The algorithm of the flash protection key, effective when enOperation set xxx_OPT_ROUTE */
    hi_u32 key_size; /* The size of flash protection key, effective when enOperation set xxx_OPT_ROUTE. */
    hi_u8 key[HI_KLAD_MAX_KEY_LEN];
} hi_klad_fp_key;

typedef struct {
    hi_u32 key_size;
    hi_u8 key[HI_KLAD_MAX_KEY_LEN];
} hi_klad_ta_key;

typedef struct {
    hi_u32 data_size;
    hi_u8 *data;
} hi_klad_trans_data;


/* Rootkey slot. */
typedef enum {
    HI_BOOT_ROOTKEY_SLOT   = 0x0,
    HI_HISI_ROOTKEY_SLOT   = 0x1,
    HI_OEM_ROOTKEY_SLOT    = 0x2,
    HI_CAS_ROOTKEY_SLOT0   = 0x10,
    HI_CAS_ROOTKEY_SLOT1   = 0x11,
    HI_CAS_ROOTKEY_SLOT2   = 0x12,
    HI_CAS_ROOTKEY_SLOT3   = 0x13,
    HI_CAS_ROOTKEY_SLOT4   = 0x14,
    HI_CAS_ROOTKEY_SLOT5   = 0x15,
    HI_CAS_ROOTKEY_SLOT6   = 0x16,
    HI_CAS_ROOTKEY_SLOT7   = 0x17,
    HI_ROOTKEY_SLOT_MAX
} hi_rootkey_select;

/* configure crypto engine type. */
typedef struct {
    hi_bool mcipher_support;  /* support send key to mcipher or not. */
    hi_bool tscipher_support; /* support send key to t_scipher(tsr2_r_cipher and demux) or not. */
} hi_rootkey_target;

/* configure crypto engine algorithm. */
typedef struct {
    hi_bool sm4_support;      /* target engine support SM4 algorithm or not. */
    hi_bool tdes_support;     /* target engine support TDES algorithm or not. */
    hi_bool aes_support;      /* target engine support AES algorithm or not. */

    hi_bool csa2_support;     /* target engine support CSA2 algorithm or not. */
    hi_bool csa3_support;     /* target engine support CSA3 algorithm or not. */
    hi_bool hmac_sha_support; /* target engine support HMAC SHA or not. */
    hi_bool hmac_sm3_support; /* target engine support HMAC SM3 or not. */
} hi_rootkey_target_alg;

/* configure target engine features. */
typedef struct {
    hi_bool encrypt_support;  /* target engine support encrypt or not. */
    hi_bool decrypt_support;  /* target engine support decrypt or not. */
} hi_rootkey_target_feature;

/* configure keyladder algithm. */
typedef struct {
    hi_bool sm4_support;     /* keyladder support SM4 algorithm or not. */
    hi_bool tdes_support;    /* keyladder support TDES algorithm or not. */
    hi_bool aes_support;     /* keyladder support AES algorithm or not. */
} hi_rootkey_alg;

/* configure keyladder stage. */
typedef enum {
    HI_ROOTKEY_LEVEL1 = 0,  /* keyladder support 1 stage. */
    HI_ROOTKEY_LEVEL2,      /* keyladder support 2 stage. */
    HI_ROOTKEY_LEVEL3,      /* keyladder support 3 stage. */
    HI_ROOTKEY_LEVEL4,      /* keyladder support 4 stage. */
    HI_ROOTKEY_LEVEL5,      /* keyladder support 5 stage. */
    HI_ROOTKEY_LEVEL_MAX
} hi_rootkey_level;

/* structure of rootkey attributes. */
typedef struct {
    hi_rootkey_select  rootkey_sel;                   /* Rootkey slot select. */
    hi_rootkey_target target_support;                 /* crypto engine select. */
    hi_rootkey_target_alg target_alg_support;         /* crypto engine algorithm. */
    hi_rootkey_target_feature target_feature_support;
    hi_rootkey_level level;                           /* keyladder stage. */
    hi_rootkey_alg alg_support;                       /* keyladder algorithm. */
} hi_rootkey_attr;

/*
* hi_klad_target_engine_type : Define the target crypto engine type
*/
typedef enum {
    HI_KLAD_TARGET_ENGINE_TSCIPHER = 0x00,
    HI_KLAD_TARGET_ENGINE_MCIPHER,
    HI_KLAD_TARGET_ENGINE_MAX
} hi_klad_target_engine_type;

/*
\brief Declare keyladder callback function interface
\param[in] err_code     Return error code.
\param[in] args         Receive buffer.
\param[in] size         The length of cArgs.
\param[in] user_data    User private data.
*/
typedef hi_s32(*callback_func)(hi_s32 err_code, hi_char *args, hi_u32 size,
                               hi_void *user_data, hi_u32 user_data_len);

/* Define cb descriptor */
typedef struct {
    callback_func call_back_func; /* keyladder callback function interface */
    hi_void *user_data;         /* user private data */
    hi_u32  user_data_len;      /* User private data length */
} klad_callback;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_KLAD_STRUCT_H__ */


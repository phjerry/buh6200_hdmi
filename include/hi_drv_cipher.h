/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of hi_drv_cipher
 * Author: zhaoguihong
 * Create: 2019-06-18
 */

#ifndef __HI_DRV_CIPHER_H__
#define __HI_DRV_CIPHER_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define HDCP_KEY_RAM_SIZE                        320
#define HDCP_KEY_PRIME_SIZE                      320
#define HDCP_KEY_TOOL_FILE_SIZE                  384
#define HDCP_KEY_CHIP_FILE_SIZE                  (HDCP_KEY_RAM_SIZE + 12)
#define HDMIRX20_HDCP22_KEY_DCP_VALID_LEN        862
#define HDMIRX20_HDCP22_KEY_BASEDATA_LEN         864
#define HDMIRX20_HDCP22_KEY_PACKAGE_VALID_LEN    1024
#define HDMIRX20_HDCP22_KEY_PACKAGE_LEN          1028

#define HI_FATAL_CIPHER(fmt...)             HI_FATAL_PRINT(HI_ID_CIPHER, fmt)
#define HI_ERR_CIPHER(fmt...)               HI_ERR_PRINT(HI_ID_CIPHER, fmt)
#define HI_WARN_CIPHER(fmt...)              HI_WARN_PRINT(HI_ID_CIPHER, fmt)
#define HI_INFO_CIPHER(fmt...)              HI_INFO_PRINT(HI_ID_CIPHER, fmt)
#define HI_DEBUG_CIPHER(fmt...)             HI_DBG_PRINT(HI_ID_CIPHER, fmt)

/** max length of SM2, unit: word */
#define SM2_LEN_IN_WROD                   (8)

/** max length of SM2, unit: byte */
#define SM2_LEN_IN_BYTE                   (SM2_LEN_IN_WROD * 4)

/** CIPHER only update key, don't set the IV */
#define CIPHER_IV_CHANGE_KEY_ONLY         (0)

/** CIPHER set key and IV for first package */
#define CIPHER_IV_CHANGE_ONE_PKG          (1)

/** CIPHER set key and IV for all package */
#define CIPHER_IV_CHANGE_ALL_PKG          (2)

/** CIPHER only update IV for first package, don't update the key*/
#define CIPHER_IV_CHANGE_ONE_PKG_IV_ONLY  (3)

/** AES IV length in word */
#define AES_IV_LEN_IN_WORD             (4)

/** SM4 IV length in word */
#define SM4_IV_LEN_IN_WORD             (4)

/** TDES IV length in word */
#define TDES_IV_LEN_IN_WORD            (2)

#define HDCP_AES_GCM_IV_LEN            (16)
#define HDCP_AES_GCM_TAG_LEN           (16)
#define HDCP_AES_GCM_AAD_LEN           (16)

/* enum typedef */
/* Cipher work mode */
typedef enum
{
    HI_CIPHER_WORK_MODE_ECB,            /* Electronic codebook (ECB) mode, ECB has been considered insecure and it is recommended not to use it.*/
    HI_CIPHER_WORK_MODE_CBC,            /* Cipher block chaining (CBC) mode*/
    HI_CIPHER_WORK_MODE_CFB,            /* Cipher feedback (CFB) mode*/
    HI_CIPHER_WORK_MODE_OFB,            /* Output feedback (OFB) mode*/
    HI_CIPHER_WORK_MODE_CTR,            /* Counter (CTR) mode*/
    HI_CIPHER_WORK_MODE_CCM,            /* Counter (CCM) mode*/
    HI_CIPHER_WORK_MODE_GCM,            /* Counter (GCM) mode*/
    HI_CIPHER_WORK_MODE_BUTT
}hi_cipher_work_mode;

/* Cipher algorithm */
typedef enum
{
    HI_CIPHER_ALG_3DES          = 0x0,  /* 3DES algorithm */
    HI_CIPHER_ALG_AES           = 0x1,  /* Advanced encryption standard (AES) algorithm */
    HI_CIPHER_ALG_SM4           = 0x2,  /* SM4 algorithm*/
    HI_CIPHER_ALG_DMA           = 0x3,  /* DMA copy*/
    HI_CIPHER_ALG_BUTT          = 0x4
}hi_cipher_alg;

/* Key length */
typedef enum
{
    HI_CIPHER_KEY_AES_128BIT    = 0x0,  /* 128-bit key for the AES algorithm */
    HI_CIPHER_KEY_AES_192BIT    = 0x1,  /* 192-bit key for the AES algorithm */
    HI_CIPHER_KEY_AES_256BIT    = 0x2,  /* 256-bit key for the AES algorithm */
    HI_CIPHER_KEY_DES_3KEY      = 0x2,  /* Three keys for the DES algorithm */
    HI_CIPHER_KEY_DES_2KEY      = 0x3,  /* Two keys for the DES algorithm */
    HI_CIPHER_KEY_DEFAULT       = 0x0,  /* Default key length, DES-8, SM1-48, SM4-16 */
}hi_cipher_key_length;

/*Cipher mem_handle*/
typedef struct
{
    hi_s64 cipher_mem_handle;
    hi_u64 cipher_addr_offset;
} hi_cipher_mem_handle;

/* Cipher bit width */
typedef enum
{
    HI_CIPHER_BIT_WIDTH_1BIT    = 0x0,  /* 1-bit width */
    HI_CIPHER_BIT_WIDTH_8BIT    = 0x1,  /* 8-bit width */
    HI_CIPHER_BIT_WIDTH_64BIT   = 0x2,  /* 64-bit width */
    HI_CIPHER_BIT_WIDTH_128BIT  = 0x3,  /* 128-bit width */
}hi_cipher_bit_width;

/* Encryption/Decryption type selecting */
typedef enum
{
    HI_CIPHER_TYPE_NORMAL       = 0x00, /* Create normal channel */
    HI_CIPHER_TYPE_BUTT,
}hi_cipher_type;

/*! encrypt operation */
#define SYMC_OPERATION_ENCRYPT         (0)

/*! encrypt operation */
#define SYMC_OPERATION_DECRYPT         (1)

/* struct define */
/* Cipher control parameters */
typedef struct
{
    hi_u32 bit1_iv:4;             /* Initial Vector change flag, 0-don't set, 1-set IV for first package, 2-set IV for each package  */
    hi_u32 bits_reserve:28;       /* Reserved */
}hi_cipher_ctrl_change_flag;

/* Structure of the cipher type */
typedef struct
{
    hi_cipher_type cipher_type;
}hi_cipher_attr;

/* Structure of the cipher control information */
typedef struct
{
    hi_u32 iv[4];                                   /* Initialization vector (IV) */
    hi_cipher_alg alg;                              /* Cipher algorithm */
    hi_cipher_bit_width bit_width;                  /* Bit width for encryption or decryption */
    hi_cipher_work_mode work_mode;                  /* Operating mode */
    hi_cipher_key_length key_len;                   /* Key length */
    hi_cipher_ctrl_change_flag change_flags;        /* control information exchange choices, we default all woulde be change except they have been in the choices */
} hi_cipher_ctrl;

/* Structure of the cipher AES control information */
typedef struct
{
    hi_u32 iv[4];                                   /* Initialization vector (IV) */
    hi_cipher_bit_width bit_width;                  /* Bit width for encryption or decryption */
    hi_cipher_key_length key_len;                   /* Key length */
    hi_cipher_ctrl_change_flag change_flags;        /* control information exchange choices, we default all woulde be change except they have been in the choices */
} hi_cipher_ctrl_aes;

/* Structure of the cipher AES CCM/GCM control information */
typedef struct
{
    hi_u32 iv[4];                                   /* Initialization vector (IV) */
    hi_cipher_key_length key_len;                   /* Key length */
    hi_u32 iv_len;                                  /* IV lenght for CCM/GCM, which is an element of {7, 8, 9, 10, 11, 12, 13} for CCM, and is an element of [1-16] for GCM*/
    hi_u32 tag_len;                                 /* Tag lenght for CCM which is an element of {4,6,8,10,12,14,16}*/
    hi_u32 a_len;                                   /* Associated data for CCM and GCM*/
    hi_cipher_mem_handle a_phy_addr;                              /* Physical address of Associated data for CCM and GCM*/
} hi_cipher_ctrl_aes_ccm_gcm;

/* Structure of the cipher 3DES control information */
typedef struct
{
    hi_u32 iv[2];                                   /* Initialization vector (IV) */
    hi_cipher_bit_width bit_width;                  /* Bit width for encryption or decryption */
    hi_cipher_key_length key_len;                   /* Key length */
    hi_cipher_ctrl_change_flag change_flags;        /* control information exchange choices, we default all woulde be change except they have been in the choices */
} hi_cipher_ctrl_3des;

/* Structure of the cipher SM4 control information */
typedef struct
{
    hi_u32 iv[4];                                   /* Initialization vector (IV) */
    hi_cipher_ctrl_change_flag change_flags;        /* control information exchange choices, we default all woulde be change except they have been in the choices */
} hi_cipher_ctrl_sm4;

/* Expand Structure of the cipher control information */
typedef struct
{
    hi_cipher_alg alg;                              /* Cipher algorithm */
    hi_cipher_work_mode work_mode;                  /* Operating mode */
    /*  Parameter for special algorithm
        for AES, the pointer should point to hi_cipher_ctrl_aes;
        for AES_CCM or AES_GCM, the pointer should point to hi_cipher_ctrl_aes_ccm_gcm;
        for DES, the pointer should point to hi_cipher_ctrl_des;
        for 3DES, the pointer should point to hi_cipher_ctrl_3des;
        for SM1, the pointer should point to hi_cipher_ctrl_sm1;
        for SM4, the pointer should point to hi_cipher_ctrl_sm4;
    */
    hi_void *param;
    hi_u32  param_size;
} hi_cipher_ctrl_ex;

/* Cipher data */
typedef struct
{
    hi_cipher_mem_handle src_phy_addr;                             /* phy address of the original data */
    hi_cipher_mem_handle dest_phy_addr;                            /* phy address of the purpose data */
    hi_u32 byte_length;                              /* Cigher data length*/
    hi_bool odd_key;                                 /* Use odd key or even key*/
} hi_cipher_data;

/* Hash algrithm type */
typedef enum
{
    HI_CIPHER_HASH_TYPE_SHA1         = 0x00,
    HI_CIPHER_HASH_TYPE_SHA224,
    HI_CIPHER_HASH_TYPE_SHA256,
    HI_CIPHER_HASH_TYPE_SHA384,
    HI_CIPHER_HASH_TYPE_SHA512,
    HI_CIPHER_HASH_TYPE_SM3          =  0x10,
    HI_CIPHER_HASH_TYPE_HMAC_SHA1    =  0x20,
    HI_CIPHER_HASH_TYPE_HMAC_SHA224,
    HI_CIPHER_HASH_TYPE_HMAC_SHA256,
    HI_CIPHER_HASH_TYPE_HMAC_SHA384,
    HI_CIPHER_HASH_TYPE_HMAC_SHA512,
    HI_CIPHER_HASH_TYPE_HMAC_SM3     =  0x30,
    HI_CIPHER_HASH_TYPE_BUTT,
}hi_cipher_hash_type;

/* Hash init struct input */
typedef struct
{
    hi_u8 *hmac_key;
    hi_u32 hmac_key_len;
    hi_cipher_hash_type sha_type;
}hi_cipher_hash_attr;


typedef enum
{
    HI_CIPHER_RSA_ENC_SCHEME_NO_PADDING,                     /* without padding */
    HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0,                   /* PKCS#1 block type 0 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1,                   /* PKCS#1 block type 1 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2,                   /* PKCS#1 block type 2 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1,                /* PKCS#1 RSAES-OAEP-SHA1 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA224,              /* PKCS#1 RSAES-OAEP-SHA224 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256,              /* PKCS#1 RSAES-OAEP-SHA256 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA384,              /* PKCS#1 RSAES-OAEP-SHA384 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA512,              /* PKCS#1 RSAES-OAEP-SHA512 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5,               /* PKCS#1 RSAES-PKCS1_V1_5 padding*/
    HI_CIPHER_RSA_ENC_SCHEME_BUTT,
}hi_cipher_rsa_enc_scheme;

typedef enum
{
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1 = 0x100, /* PKCS#1 RSASSA_PKCS1_V15_SHA1 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224,       /* PKCS#1 RSASSA_PKCS1_V15_SHA224 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256,       /* PKCS#1 RSASSA_PKCS1_V15_SHA256 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384,       /* PKCS#1 RSASSA_PKCS1_V15_SHA384 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512,       /* PKCS#1 RSASSA_PKCS1_V15_SHA512 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1,         /* PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224,       /* PKCS#1 RSASSA_PKCS1_PSS_SHA224 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256,       /* PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384,       /* PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512,       /* PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/
    HI_CIPHER_RSA_SIGN_SCHEME_BUTT,
}hi_cipher_rsa_sign_scheme;

/* RSA public key struct */
typedef struct
{
    hi_u8  *n;                          /* Point to public modulus  */
    hi_u8  *e;                          /* Point to public exponent */
    hi_u16 n_len;                       /* Length of public modulus, max value is 512Byte*/
    hi_u16 e_len;                       /* Length of public exponent, max value is 512Byte*/
}hi_cipher_rsa_pub_key;

/* RSA private key struct */
typedef struct
{
    hi_u8 *n;                           /* Public modulus    */
    hi_u8 *e;                           /* Public exponent   */
    hi_u8 *d;                           /* Private exponent  */
    hi_u8 *p;                           /* 1st prime factor  */
    hi_u8 *q;                           /* 2nd prime factor  */
    hi_u8 *dp;                          /* d% (p - 1) */
    hi_u8 *dq;                          /* d % (q - 1) */
    hi_u8 *qp;                          /* 1 / (q % p) */
    hi_u16 n_len;                       /* Length of public modulus */
    hi_u16 e_len;                       /* Length of public exponent */
    hi_u16 d_len;                       /* Length of private exponent */
    hi_u16 p_len;                       /* Length of 1st prime factor,should be half of u16NLen */
    hi_u16 q_len;                       /* Length of 2nd prime factor,should be half of u16NLen */
    hi_u16 dp_len;                      /* Length of D % (P - 1),should be half of u16NLen */
    hi_u16 dq_len;                      /* Length of D % (Q - 1),should be half of u16NLen */
    hi_u16 qp_len;                      /* Length of 1 / (Q % P),should be half of u16NLen */
}hi_cipher_rsa_pri_key;

/* RSA public key encryption struct input */
typedef struct
{
    hi_cipher_rsa_enc_scheme scheme;    /* RSA encryption scheme */
    hi_cipher_rsa_pub_key pub_key;      /* RSA private key struct */
}hi_cipher_rsa_pub_enc;

/* RSA private key decryption struct input */
typedef struct
{
    hi_cipher_rsa_enc_scheme scheme;    /* RSA encryption scheme */
    hi_cipher_rsa_pri_key pri_key;      /* RSA private key struct */
}hi_cipher_rsa_pri_enc;

/* RSA signature struct input */
typedef struct
{
    hi_cipher_rsa_sign_scheme scheme;   /* RSA signature scheme*/
    hi_cipher_rsa_pri_key pri_key;      /* RSA private key struct */
 }hi_cipher_rsa_sign;

/* RSA signature verify struct input */
typedef struct
{
    hi_cipher_rsa_sign_scheme scheme;   /* RSA signature scheme*/
    hi_cipher_rsa_pub_key pub_key;      /* RSA public key struct */
 }hi_cipher_rsa_verify;

/* SM2 signature struct input */
 typedef struct
{
    hi_u32 d[SM2_LEN_IN_WROD];
    hi_u32 px[SM2_LEN_IN_WROD];
    hi_u32 py[SM2_LEN_IN_WROD];
    hi_u8 *id;
    hi_u16 id_len;
}hi_cipher_sm2_sign;

/* SM2 signature verify struct input */
typedef struct
{
    hi_u32 px[SM2_LEN_IN_WROD];
    hi_u32 py[SM2_LEN_IN_WROD];
    hi_u8 *id;
    hi_u16 id_len;
}hi_cipher_sm2_verify;

/* SM2 publuc key encryption struct input */
typedef struct
{
    hi_u32 px[SM2_LEN_IN_WROD];
    hi_u32 py[SM2_LEN_IN_WROD];
}hi_cipher_sm2_enc;

/* SM2 private key decryption struct input */
typedef struct
{
    hi_u32 d[SM2_LEN_IN_WROD];
}hi_cipher_sm2_dec;

/* SM2 key generate struct input */
typedef struct
{
    hi_u32 d[SM2_LEN_IN_WROD];
    hi_u32 px[SM2_LEN_IN_WROD];
    hi_u32 py[SM2_LEN_IN_WROD];
}hi_cipher_sm2_key;

/* Elliptic curve domain parameters. */
typedef struct
{
  hi_u8 *p;        /* Finite field: equal to p in case of prime field curves or equal to 2^n in case of binary field curves.*/
  hi_u8 *a;        /* Curve parameter a (q-3 in Suite B).*/
  hi_u8 *b;        /* Curve parameter b*/
  hi_u8 *gx;       /* X coordinates of G which is a base point on the curve.*/
  hi_u8 *gy;       /* Y coordinates of G which is a base point on the curve.*/
  hi_u8 *n;        /* Prime which is the order of G point.*/
  hi_u32 h;        /* Cofactor, which is the order of the elliptic curve divided by the order of the point G. For the Suite B curves, h = 1.*/
  hi_u32 key_size; /* Key size in bytes. It corresponds to the size in bytes of the prime phi_u8n*/
} hi_cipher_ecc_param;

/* CENC subsample struct input */
typedef struct
{
    hi_u32 clear_header_len;
    hi_u32 payload_len;
    hi_u32 payload_pattern_encrypt_len;
    hi_u32 payload_pattern_clear_len;
    hi_u32 payload_pattern_offset_len;
    hi_u32 iv_change;
    hi_u32 iv[4];
}hi_cipher_subsample;

typedef struct
{
    hi_bool  odd_key;
    hi_u32   first_offset;
    hi_cipher_subsample *subsample;
    hi_u32 subsample_num;
}hi_cipher_cenc;

/*! \hdmi ram sel */
typedef enum {
    HDMI_RAM_SEL_NONE = 0x00,    /*!<  don't write data to hdmi ram  */
    HDMI_RAM_SEL_RX_14,          /*!<  hmdi rx 1.4  */
    HDMI_RAM_SEL_TX_14,          /*!<  hmdi tx 1.4  */
    HDMI_RAM_SEL_RX_22,          /*!<  hmdi rx 2.2  */
    HDMI_RAM_SEL_TX_22,          /*!<  hmdi tx 2.2  */
    HDMI_RAM_SEL_COUNT,
} hdmi_ram_sel;

typedef enum {
    HDCP_KEY_SEL_KLAD = 0x00,   /*!<  OTP HDCP Root Key  */
    HDCP_KEY_SEL_HISI,         /*!<  Hisilicon Fixed Key */
    HDCP_KEY_SEL_COUNT,
} hdcp_rootkey_sel;

typedef struct
{
    hdmi_ram_sel     ram_sel;
    hi_u32           ram_num;
    hdcp_rootkey_sel key_sel;
    hi_u32           key_slot;
    hi_cipher_alg       alg;
    hi_cipher_work_mode mode;
    hi_u8  aad[HDCP_AES_GCM_AAD_LEN];
    hi_u8  iv[HDCP_AES_GCM_IV_LEN];
    hi_u32 tag[HDCP_AES_GCM_TAG_LEN];
}hi_cipher_hdcp_attr;

typedef struct
{
    hi_handle handle;
    hi_cipher_attr cipher_attr;
}hi_cipher_handle;

typedef struct
{
    hi_handle handle;
    hi_u64 src_phy_addr;
    hi_u64 dest_phy_addr;
    hi_u32 data_length;
}hi_cipher_data_ex;

typedef struct
{
    hi_handle handle;
    hi_u32 key[12];                               /* Key input, EK||AK||SK for SM1 */ /**< CNcomment:Կ, SM1δAK||EK||SK*/
    hi_u32 odd_key[8];                            /* Key input, Old Key */            /**< CNcomment:Կ, Կ*/
    hi_u32 iv[4];                                 /* Initialization vector (IV) */                                                                                    /**< CNcomment:ʼ */
    hi_cipher_alg alg;                            /* Cipher algorithm */                                                                                              /**< CNcomment:㷨 */
    hi_cipher_bit_width bit_width;                /* Bit width for encryption or decryption */                                                                        /**< CNcomment:ܻܵλ */
    hi_cipher_work_mode work_mode;                /* Operating mode */                                                                                                /**< CNcomment:ģʽ */
    hi_cipher_key_length key_len;                 /* Key length */                                                                                                    /**< CNcomment:Կ */
    hi_cipher_ctrl_change_flag change_flags;      /* control information exchange choices, we default all woulde be change except they have been in the choices */    /**< CNcomment:ϢѡѡûбʶĬȫ */
    hi_u32 iv_len;                                /* IV lenght for CCM/GCM, which is an element of {4,6,8,10,12,14,16} for CCM, and is an element of [1-16] for GCM*/  /**< CNcomment: CCM/GCMIVȣCCMȡֵΧ{7, 8, 9, 10, 11, 12, 13} GCMȡֵΧ[1-16]*/
    hi_u32 tag_len;                               /* Tag lenght for CCM which is an element of {4,6,8,10,12,14,16}*/          /**< CNcomment: CCMTAGȣȡֵΧ{4,6,8,10,12,14,16}*/
    hi_u32 a_len;                                 /* Associated data for CCM and GCM*/                                        /**< CNcomment: CCM/GCMĹݳ*/
    hi_u32 a_phy_addr;                            /* Physical address of Associated data for CCM and GCM*/                                        /**< CNcomment: CCM/GCMĹݳ*/
}hi_cipher_config_ctrl_ex;

typedef struct
{
    hi_u32 time_out_us;
    hi_u32 rng;
}hi_cipher_rng;

hi_s32 hi_drv_cipher_create_handle(hi_handle *handle, hi_cipher_attr *cipher_attr);
hi_s32 hi_drv_cipher_config_chn(hi_handle handle, hi_cipher_ctrl *ctrl);
hi_s32 hi_drv_cipher_config_chn_ex(hi_cipher_config_ctrl_ex *ctrl);
hi_s32 hi_drv_cipher_get_key_slot_handle(hi_handle cipher, hi_handle *key_slot);
hi_s32 hi_drv_cipher_destroy_handle(hi_handle cipher_chn);
hi_s32 hi_drv_cipher_encrypt(hi_handle handle, hi_u64 src_phy_addr, hi_u64 dest_phy_addr, hi_u32 data_length);
hi_s32 hi_drv_cipher_decrypt(hi_handle handle, hi_u64 src_phy_addr, hi_u64 dest_phy_addr, hi_u32 data_length);
hi_s32 hi_drv_cipher_get_random_number(hi_u32 *randnum, hi_u32 timeout);
hi_s32 hi_drv_cipher_soft_reset(hi_void);
hi_s32 hi_drv_cipher_hash_start(hi_handle *handle, hi_cipher_hash_type type, hi_u8 *key, hi_u32 keylen);
hi_s32 hi_drv_cipher_hash_update(hi_handle handle, hi_u8 *input, hi_u32 length);
hi_s32 hi_drv_cipher_hash_finish(hi_handle handle, hi_u8 *hash, hi_u32 *hashlen);
hi_void hi_drv_cipher_suspend(hi_void);
hi_s32 hi_drv_cipher_resume(hi_void);

hi_s32 hi_drv_cipher_rsa_sign(hi_cipher_rsa_sign *rsa_sign,
                             hi_u8 *hash_data,
                             hi_u32 hash_len,
                             hi_u8 *out_sign,
                             hi_u32 *out_sign_len);

hi_s32 hi_drv_cipher_rsa_verify(hi_cipher_rsa_verify *rsa_verify,
                               hi_u8 *hash_data,
                               hi_u32 hash_len,
                               hi_u8 *in_sign,
                               hi_u32 in_sign_len);
hi_s32 hi_drv_cipher_load_hdcp_key(hi_cipher_hdcp_attr *attr, const hi_u8 *key, hi_u32 keylen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* End of #ifndef __HI_DRV_CIPHER_H__*/


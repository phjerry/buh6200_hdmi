/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_cipher_ioctl
 * Author: zhaoguihong
 * Create: 2019-06-18
 */

#ifndef __DRV_CIPHER_IOCTL_H__
#define __DRV_CIPHER_IOCTL_H__

#include "hi_type.h"
#include "hi_drv_cipher.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! \the source of hash message */
typedef enum {
    HASH_CHUNCK_SRC_LOCAL, /*!<  Local buffer, e.g. Kernel  */
    HASH_CHUNCK_SRC_USER,  /*!<  User buffer, use copy_from_user to read data */
} hash_chunk_src;

/*! \union of compat addr*/
typedef union {
    void *p;                /*!<  virtual address */
    unsigned long long phy; /*!<  physical address */
    unsigned int word[2];   /*!<  double word of address */
} compat_addr;

extern compat_addr compat_addr_zero;
#define ADDR_H32(addr)   addr.word[1]  /*!<  High 32 bit of hi_u64 */
#define ADDR_L32(addr)   addr.word[0]  /*!<  Low 32 bit of hi_u64 */
#define ADDR_U64(addr)   addr.phy      /*!<  64 bit of hi_u64 */
#define ADDR_VIA(addr)   addr.p          /*!<  buffer point */
#define ADDR_NULL        compat_addr_zero  /*!<  buffer point */

#define ADDR_P_H32(addr) addr->word[1]  /*!<  High 32 bit of hi_u64 */
#define ADDR_P_L32(addr) addr->word[0]  /*!<  Low 32 bit of hi_u64 */
#define ADDR_P_U64(addr) addr->phy      /*!<  64 bit of hi_u64 */
#define ADDR_P_VIA(addr) addr->p        /*!<  buffer point */

#define CRYPTO_MAGIC_NUM   (0xc0704d19)

/*! \AES KEY size */
#define SYMC_KEY_SIZE       (32)

/*! \SM1 SK size */
#define SYMC_SM1_SK_SIZE    (16)

/*! \AES IV size */
#define AES_IV_SIZE         (16)

/*! \AES BLOCK size */
#define AES_BLOCK_SIZE_IN_BYTE (16)
#define AES_BLOCK_SIZE_IN_WORD (4)

/*! \DES IV size */
#define DES_IV_SIZE         (8)

/*width of word*/
#define WORD_WIDTH                      (4)
#define WORD_BIT_WIDTH                  (32)
#define U32_MAX_SIZE                    (0xFFFFFFFF)

/*width of double word*/
#define DOUBLE_WORD_WIDTH               (8)
#define DOUBLE_WORD                     (2)

#ifndef SM2_LEN_IN_WROD
#define SM2_LEN_IN_WROD                 (8)
#define SM2_LEN_IN_BYTE                 (32)
#endif

/*! \aead tag length */
#define AEAD_TAG_SIZE                  (16)
#define AEAD_TAG_SIZE_IN_WORD          (4)

/*! \bits in a byte */
#define BITS_IN_BYTE                   (8)

/*! \hash result max size */
#define HASH_RESULT_MAX_SIZE           (64)

/*! \hash result max size in word */
#define HASH_RESULT_MAX_SIZE_IN_WORD   (16)

/*! \hdcp root key size in word */
#define HDCP_ROOT_KEY_SIZE_IN_WORD     (4)

/*! encrypt operation */
#define SYMC_OPERATION_ENCRYPT         (0)

/*! encrypt operation */
#define SYMC_OPERATION_DECRYPT         (1)

/*! encrypt operation */
#define HDCP_OPERATION_ENCRYPT         (0x3CA5965A)

/*! encrypt operation */
#define HDCP_OPERATION_DECRYPT         (0xC35A69A5)

/*! capacity upport */
#define CRYPTO_CAPACITY_SUPPORT        (1)
#define CRYPTO_CAPACITY_NONSUPPORT     (0)

/* max length of SM2 ID */
#define SM2_ID_MAX_LEN          0x1FFF

/*! Define the time out */
#define CRYPTO_TIME_OUT         6000
#define MS_TO_US                1000

/* result size */
#define SHA1_RESULT_SIZE           (20)  /* SHA1 */
#define SHA224_RESULT_SIZE         (28)  /* SHA224 */
#define SHA256_RESULT_SIZE         (32)  /* SHA256 */
#define SHA384_RESULT_SIZE         (48)  /* SHA384 */
#define SHA512_RESULT_SIZE         (64)  /* SHA512 */
#define SM3_RESULT_SIZE            (32)  /* SM3 */

/* rsa key length */
#define RSA_MIN_KEY_LEN     (32)
#define RSA_MAX_KEY_LEN     (512)
#define RSA_KEY_BITWIDTH_1024     (128)
#define RSA_KEY_BITWIDTH_2048     (256)
#define RSA_KEY_BITWIDTH_3072     (384)
#define RSA_KEY_BITWIDTH_4096     (512)
#define RSA_KEY_EXPONENT_VALUE1   (0X3)
#define RSA_KEY_EXPONENT_VALUE2   (0X10001)

/*! \struct of Symmetric cipher create */
typedef struct {
    hi_u32 id;                    /*!< to store the id of soft channel */
    hi_cipher_type type;   /*!<  symc channel type */
} symc_create_t;

/*! \struct of Symmetric cipher destroy */
typedef struct {
    hi_u32 id;             /*!< id of soft channel */
    hi_u32 reserve;        /*!<  reserve to make align at 64bit */
} symc_destroy_t;

/*! \struct of Symmetric cipher get key slot handle */
typedef struct {
    hi_u32 cipher;             /*!< cipher channel */
    hi_u32 keyslot;            /*!< keyslot channel */
} symc_keyslot_t;

/*! \struct of Symmetric cipher configure infomation */
typedef struct {
    hi_u32 id;                               /*!<  Id of soft channel */
    hi_cipher_alg alg;                       /*!<  Symmetric cipher algorithm */
    hi_cipher_work_mode mode;                /*!<  Symmetric cipher algorithm */
    hi_cipher_bit_width width;               /*!<  Symmetric cipher bit width */
    hi_cipher_key_length klen;               /*!<  Symmetric cipher key length */
    hi_u8 iv[AES_IV_SIZE];                   /*!<  IV buffer */
    hi_u32 ivlen;                            /*!<  IV length */
    hi_u32 iv_usage;                         /*!<  Usage of IV */
    hi_u32 reserve;        /*!<  reserve to make align at 64bit */
    hi_cipher_mem_handle aad;       /*!<  Associated Data */
    hi_u32  alen;          /*!<  Associated Data Length */
    hi_u32  tlen;          /*!<  Tag length */
} symc_config_t;

/*! \struct of Symmetric cipher encrypt/decrypt */
typedef struct {
    hi_u32 id;              /*!<  Id of soft channel */
    hi_u32 length;          /*!<  Length of the encrypted data */
    hi_u32 operation;       /*!<  Decrypt or encrypt */
    hi_u32 last;            /*!<  last or not */
    hi_cipher_mem_handle input;      /*!<  Physical address of the input data */
    hi_cipher_mem_handle output;     /*!<  Physical address of the output data */
} symc_encrypt_t;

/*! \struct of Symmetric cipher multiple encrypt/decrypt */
typedef struct {
    hi_u32 id;             /*!<  Id of soft channel */
    compat_addr pkg;       /*!<  Buffer of package infomation */
    hi_u32 pkg_num;        /*!<  Number of package infomation */
    hi_u32 operation;      /*!<  Decrypt or encrypt */
} symc_encrypt_multi_t;

/*! \struct of Symmetric cipher wait done */
typedef struct {
    hi_u32 id;             /*!<  Id of soft channel */
    hi_u32 timetout;       /*!< timeout */
} symc_waitdone_t;

/*! \struct of Symmetric cipher cmac encrypt/decrypt */
typedef struct {
    hi_u32 id;             /*!<  Id of soft channel */
    hi_u32 inlen;          /*!<  length of input */
    compat_addr in;        /*!<  Buffer of input */
    hi_u8 mac[AES_BLOCK_SIZE_IN_BYTE];    /*!<  CMAC */
    hi_u32 last;           /*!<  last or not */
    hi_u32 reserve;        /*!<  reserve for align at 64bit */
} symc_cmac_t;

/*! \struct of Symmetric cipher get tag */
typedef struct {
    hi_u32 id;                            /*!<  Id of soft channel */
    hi_u32 tag[AEAD_TAG_SIZE_IN_WORD];    /*!<  Buffer of tag */
    hi_u32 taglen ;                       /*!<  Length of tag */
} aead_tag_t;

/*! \struct of Symmetric cipher get ctrl */
typedef struct {
    hi_u32 id;                            /*!<  Id of soft channel */
    hi_cipher_ctrl  ctrl;                 /*!<  control infomation */
} symc_get_config_t;

/*! \struct of Hash start */
typedef struct {
    hi_u32 id;                            /*!<  Id of soft channel */
    hi_cipher_hash_type  type;            /*!<  HASH type */
    compat_addr key;                      /*!<  HMAC key */
    hi_u32 keylen;                        /*!<  HMAC key */
    hi_u32 reserve;                       /*!<  reserve for align at 64bit */
} hash_start_t;

/*! \struct of Hash update */
typedef struct {
    hi_u32 id;             /*!<  Id of soft channel */
    hi_u32 length;         /*!<  Length of the message */
    compat_addr input;     /*!<  Message data buffer */
    hash_chunk_src src;    /*!<  source of hash message */
    hi_u32 reserve;        /*!<  reserve for align at 64bit */
} hash_update_t;

/*! \struct of Hash update */
typedef struct {
    hi_u32 id;             /*!<  Id of soft channel */
    hi_u32 hash[HASH_RESULT_MAX_SIZE_IN_WORD]; /*!<  buffer holding the hash data */
    hi_u32 hashlen;        /*!<  length of the hash data */
    hi_u32 reserve;        /*!<  reserve for align at 64bit */
} hash_finish_t;

/*! \struct of rsa encrypt/decrypt */
typedef struct {
    hi_cipher_rsa_enc_scheme enc_scheme;    /*!<  RSA encryption scheme */
    hi_cipher_rsa_sign_scheme sign_scheme;  /*!<  RSA sign scheme */
    hi_u32 public;             /** Type of key, true-public or false-private */
    hi_u32 klen;               /*!<  length of rsa key */
    hi_u32 e;                  /*!<  The public exponent */
    compat_addr d;          /*!<  The private exponent */
    compat_addr n;          /*!<  The modulus */
    compat_addr p;          /*!<  The p factor of N */
    compat_addr q;          /*!<  The q factor of N */
    compat_addr qp;         /*!<  The 1/q mod p CRT param */
    compat_addr dp;         /*!<  The d mod (p - 1) CRT param */
    compat_addr dq;         /*!<  The d mod (q - 1) CRT param */
    compat_addr in;         /*!<  input data to be encryption */
    compat_addr out;        /*!<  output data of encryption */
    hi_u32 inlen;              /*!<  length of input data to be encryption */
    hi_u32 outlen;             /*!<  length of output data */
} rsa_info_t;

/*! \struct of ecc  */
typedef struct {
    compat_addr p;      /*!<  Finite field: equal to p in case of prime field curves or equal to 2^n in case of binary field curves.*/
    compat_addr a;      /*!<  Curve parameter a (q-3 in Suite B).*/
    compat_addr b;      /*!<  Curve parameter b*/
    compat_addr gx;     /*!<  X coordinates of G which is a base point on the curve.*/
    compat_addr gy;     /*!<  Y coordinates of G which is a base point on the curve.*/
    compat_addr n;      /*!<  Prime which is the order of G point.*/
    hi_u32 h;           /*!<  Cofactor, which is the order of the elliptic curve divided by the order of the point G. For the Suite B curves, h = 1.*/
    hi_u32 ksize;       /*!<  Key size in bytes. It corresponds to the size in bytes of the prime phi_u8n*/
    compat_addr d;      /*!<  privtate key*/
    compat_addr px;     /*!<  X coordinates of public key.*/
    compat_addr py;     /*!<  Y coordinates of public key.*/
    compat_addr msg;    /*!<  intput or output message */
    hi_u32 mlen;        /*!<  length of intput or output message */
    hi_u32 magic_num;   /*!<  magic num for security */
    compat_addr r;      /*!<  Buffer where to write the uncompressed r component of the resulting signature.*/
    compat_addr s;      /*!<  Buffer where to write the uncompressed s component of the resulting signature.*/
} ecc_info_t;

/*! \struct of pbkdf  */
typedef struct {
    compat_addr password; /*!<  Buffer of password */
    compat_addr salt;     /*!<  Buffer of salt */
    compat_addr output;   /*!<  Buffer of ouput */
    hi_u32 plen;              /*!<  Length of password */
    hi_u32 slen;              /*!<  Length of salt */
    hi_u32 iteration_count;   /*!<  iteration count */
    hi_u32 outlen;            /*!<  ouput key length */
} pbkdf_hmac256_t;

/*! \struct of cenc  */
typedef struct {
    hi_u32 id;
    hi_handle key_slot;
    hi_u32 oddkey;
    hi_u32 length;
    hi_u32 firstoffset;
    hi_u32 subsample_num;
    compat_addr subsample;
    hi_cipher_mem_handle inphy;
    hi_cipher_mem_handle outphy;
} cenc_info_t;

/*! \struct of ecc  */
typedef struct {
    hi_u8 *p; /*!<  Finite field: equal to p in case of prime field curves or equal to 2^n in case of binary field curves.*/
    hi_u8 *a; /*!<  Curve parameter a (q-3 in Suite B).*/
    hi_u8 *b; /*!<  Curve parameter b*/
    hi_u8 *gx;/*!<  X coordinates of G which is a base point on the curve.*/
    hi_u8 *gy;/*!<  Y coordinates of G which is a base point on the curve.*/
    hi_u8 *n; /*!<  Prime which is the order of G point.*/
    hi_u32 h;         /*!<  Cofactor, which is the order of the elliptic curve divided by the order of the point G. For the Suite B curves, h = 1.*/
    hi_u32 ksize;     /*!<  Key size in bytes. It corresponds to the size in bytes of the prime phi_u8n*/
} ecc_param_t;

/** RSA PKCS style key */
typedef struct {
    /** Type of key, true-public or false-private */
    hi_u16 public;
    /** The key length */
    hi_u16 klen;
    /** The public exponent */
    hi_u32 e;
    /** The private exponent */
    hi_u8 *d;
    /** The modulus */
    hi_u8 *n;
    /** The p factor of N */
    hi_u8 *p;
    /** The q factor of N */
    hi_u8 *q;
    /** The 1/q mod p CRT param */
    hi_u8 *qp;
    /** The d mod (p - 1) CRT param */
    hi_u8 *dp;
    /** The d mod (q - 1) CRT param */
    hi_u8 *dq;
} cryp_rsa_key;

/*! \struct of trng */
typedef struct {
    compat_addr randnum;         /*!<  randnum rand number  */
    hi_u32 size;
    hi_u32 timeout;              /*!<  time out  */
} trng_t;

/*! \struct of sm2 sign */
typedef struct {
    hi_u32 d[SM2_LEN_IN_WROD];    /*!<  sm2 private key */
    hi_u32 px[SM2_LEN_IN_WROD];   /*!<  sm2 x public key */
    hi_u32 py[SM2_LEN_IN_WROD];   /*!<  sm2 y public key */
    hi_u32 idlen;                 /*!<  length of sm2 user id */
    hi_u32 magic_num;             /*!<  magic num for secure */
    compat_addr id;               /*!<  sm2 user id */
    compat_addr msg;              /*!<  mesaage to be sign */
    hi_u32 msglen;                /*!<  length of mesaage to be sign */
    hash_chunk_src src;           /*!<  source of hash message */
    hi_u32 r[SM2_LEN_IN_WROD];    /*!<  sm2 sign result of r */
    hi_u32 s[SM2_LEN_IN_WROD];    /*!<  sm2 sign result of s */
} sm2_sign_t;

/*! \struct of sm2 verify */
typedef struct {
    hi_u32 px[SM2_LEN_IN_WROD];   /*!<  sm2 x public key */
    hi_u32 py[SM2_LEN_IN_WROD];   /*!<  sm2 y public key */
    compat_addr id;               /*!<  sm2 user id */
    compat_addr msg;              /*!<  mesaage to be sign */
    hi_u32 idlen;                 /*!<  length of sm2 user id */
    hi_u32 msglen;                /*!<  length of mesaage to be sign */
    hash_chunk_src src;           /*!<  source of hash message */
    hi_u32 magic_num;             /*!<  magic num for secure */
    hi_u32 r[SM2_LEN_IN_WROD];    /*!<  sm2 sign result of r */
    hi_u32 s[SM2_LEN_IN_WROD];    /*!<  sm2 sign result of s */
} sm2_verify_t;

/*! \struct of sm2 encrypt */
typedef struct {
    hi_u32 px[SM2_LEN_IN_WROD];   /*!<  sm2 x public key */
    hi_u32 py[SM2_LEN_IN_WROD];   /*!<  sm2 y public key */
    compat_addr msg;              /*!<  mesaage to be encrypt */
    compat_addr enc;              /*!<  encrypt mesaage */
    hi_u32 enclen;                /*!<  length of encrypt mesaage */
    hi_u32 msglen;                /*!<  length of mesaage to be encrypt */
    hi_u32 keylen;                /*!<  lenth of sm2 key in word */
    hi_u32 magic_num;             /*!<  magic num for secure */
} sm2_encrypt_t;

/*! \struct of sm2 decrypt */
typedef struct {
    hi_u32 d[SM2_LEN_IN_WROD];    /*!<  sm2 private key */
    compat_addr enc;              /*!<  encrypt mesaage */
    compat_addr msg;              /*!<  decrypt mesaage */
    hi_u32 msglen;                /*!<  length of decrypt mesaage */
    hi_u32 enclen;                /*!<  length of encrypt mesaage */
    hi_u32 keylen;                /*!<  lenth of sm2 key in word */
    hi_u32 magic_num;             /*!<  magic num for secure */
} sm2_decrypt_t;

/*! \struct of sm2 key */
typedef struct {
    hi_u32 d[SM2_LEN_IN_WROD];    /*!<  sm2 private key */
    hi_u32 px[SM2_LEN_IN_WROD];   /*!<  sm2 x public key */
    hi_u32 py[SM2_LEN_IN_WROD];   /*!<  sm2 y public key */
    hi_u32 keylen;
    hi_u32 magic_num;
} sm2_key_t;

/*! \struct of hdcp key */
typedef struct {
    hi_cipher_hdcp_attr attr;
    compat_addr in;
    compat_addr out;
    hi_u32 len;
    hi_u32 decrypt;
} hdcp_key_t;

#define HASH_BLOCK_SIZE            (64)

#define CRYPTO_IOC_NA           0U
#define CRYPTO_IOC_W            1U
#define CRYPTO_IOC_R            2U
#define CRYPTO_IOC_RW           3U

/* Ioctl definitions */
#define CIPHER_NAME "HI_CIPHER"

#define CRYPTO_IOC(dir,type,nr,size) (((dir) << 30)|((size) << 16)|((type) << 8)|((nr) << 0))
#define CRYPTO_IOR(nr,size)    CRYPTO_IOC(CRYPTO_IOC_R, HI_ID_CIPHER,(nr), size)
#define CRYPTO_IOW(nr,size)    CRYPTO_IOC(CRYPTO_IOC_W, HI_ID_CIPHER,(nr), size)
#define CRYPTO_IOWR(nr,size)   CRYPTO_IOC(CRYPTO_IOC_RW, HI_ID_CIPHER,(nr),size)

#define CRYPTO_IOC_DIR(cmd)       (((cmd) >> 30) & 0x03)
#define CRYPTO_IOC_TYPE(cmd)      (((cmd) >> 8) & 0xFF)
#define CRYPTO_IOC_NR(cmd)        (((cmd) >> 0) & 0xFF)
#define CRYPTO_IOC_SIZE(cmd)      (((cmd) >> 16) & 0x3FFF)

#define CRYPTO_CMD_SYMC_CREATEHANDLE      CRYPTO_IOWR(0x00, sizeof(symc_create_t))
#define CRYPTO_CMD_SYMC_DESTROYHANDLE     CRYPTO_IOW (0x01, sizeof(symc_destroy_t))
#define CRYPTO_CMD_SYMC_CONFIGHANDLE      CRYPTO_IOW (0x02, sizeof(symc_config_t))
#define CRYPTO_CMD_SYMC_ENCRYPT           CRYPTO_IOW (0x03, sizeof(symc_encrypt_t))
#define CRYPTO_CMD_SYMC_ENCRYPTMULTI      CRYPTO_IOW (0x04, sizeof(symc_encrypt_multi_t))
#define CRYPTO_CMD_SYMC_CMAC              CRYPTO_IOWR(0x05, sizeof(symc_cmac_t))
#define CRYPTO_CMD_SYMC_GETTAG            CRYPTO_IOWR(0x06, sizeof(aead_tag_t))
#define CRYPTO_CMD_CENC                   CRYPTO_IOW (0x07, sizeof(cenc_info_t))
#define CRYPTO_CMD_HASH_START             CRYPTO_IOWR(0x08, sizeof(hash_start_t))
#define CRYPTO_CMD_HASH_UPDATE            CRYPTO_IOW (0x09, sizeof(hash_update_t))
#define CRYPTO_CMD_HASH_FINISH            CRYPTO_IOWR(0x0a, sizeof(hash_finish_t))
#define CRYPTO_CMD_RSA_ENC                CRYPTO_IOWR(0x0b, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_DEC                CRYPTO_IOWR(0x0c, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_SIGN               CRYPTO_IOWR(0x0d, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_VERIFY             CRYPTO_IOWR(0x0e, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_COMPUTE_CRT        CRYPTO_IOWR(0x0f, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_GEN_KEY            CRYPTO_IOWR(0x10, sizeof(rsa_info_t))
#define CRYPTO_CMD_TRNG                   CRYPTO_IOW (0x11, sizeof(trng_t))
#define CRYPTO_CMD_SM2_SIGN               CRYPTO_IOWR(0x12, sizeof(sm2_sign_t))
#define CRYPTO_CMD_SM2_VERIFY             CRYPTO_IOWR(0x13, sizeof(sm2_verify_t))
#define CRYPTO_CMD_SM2_ENCRYPT            CRYPTO_IOWR(0x14, sizeof(sm2_encrypt_t))
#define CRYPTO_CMD_SM2_DECRYPT            CRYPTO_IOWR(0x15, sizeof(sm2_decrypt_t))
#define CRYPTO_CMD_SM2_GEN_KEY            CRYPTO_IOWR(0x16, sizeof(sm2_key_t))
#define CRYPTO_CMD_ECDH_COMPUTE_KEY       CRYPTO_IOWR(0x17, sizeof(ecc_info_t))
#define CRYPTO_CMD_ECC_GEN_KEY            CRYPTO_IOWR(0x18, sizeof(ecc_info_t))
#define CRYPTO_CMD_ECDSA_SIGN             CRYPTO_IOW (0x19, sizeof(ecc_info_t))
#define CRYPTO_CMD_ECDSA_VERIFY           CRYPTO_IOW (0x1a, sizeof(ecc_info_t))
#define CRYPTO_CMD_SYMC_GET_CONFIG        CRYPTO_IOWR(0x1b, sizeof(symc_get_config_t))
#define CRYPTO_CMD_SYMC_GET_KEYSLOT       CRYPTO_IOWR(0x1c, sizeof(symc_keyslot_t))
#define CRYPTO_CMD_HDCP_ENCRYPT           CRYPTO_IOWR(0x1d, sizeof(hdcp_key_t))
#define CRYPTO_CMD_COUNT                  0x1E

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __DRV_CIPHER_IOCTL_H__*/

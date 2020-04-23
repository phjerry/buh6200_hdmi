/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_cipher_kapi
 * Author: zhaoguihong
 * Create: 2019-06-18
 */

#ifndef __TEE_DRV_CIPHER_KAPI_H__
#define __TEE_DRV_CIPHER_KAPI_H__

#include "hi_drv_cipher.h"
#include "drv_cipher_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

/******************************* API Declaration *****************************/
/** \addtogroup      crypto */
/** @{ */  /** <!--[link]*/

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      crypto */
/** @{ */  /** <!-- [link] */

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_init(hi_void);

/**
\brief   Kapi Deinit.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_deinit(hi_void);

/**
\brief   Kapi release.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_release(hi_void);

/**
\brief   Create symc handle.
\param[in]  id The channel number.
\param[in]  uuid The user identification.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_create(hi_u32 *id, hi_cipher_type type);

/**
\brief   Destroy symc handle.
\param[in]  id The channel number.
\param[in]  uuid The user identification.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_destroy(hi_u32 id);
hi_s32 kapi_symc_get_keyslot(hi_handle cipher,  hi_handle *keyslot);

/**
\brief  set work params.
* \param[in]  id The channel number.
* \param[in]  hard_key whether use the hard key or not.
* \param[in]  alg The symmetric cipher algorithm.
* \param[in]  mode The symmetric cipher mode.
* \param[in]  sm1_round_num The round number of sm1.
* \param[in]  fkey first  key buffer, defualt
* \param[in]  skey second key buffer, expand
* \param[in]  klen The key length.
* \param[in]  aad      Associated Data
* \param[in]  alen     Associated Data Length
* \param[in]  tlen     Tag length
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_config(hi_u32 id,
                     hi_cipher_alg alg,
                     hi_cipher_work_mode work_mode,
                     hi_cipher_bit_width bit_width,
                     hi_cipher_key_length key_len,
                     hi_u8 *iv, hi_u32 ivlen, hi_u32 iv_usage,
                     compat_addr aad, hi_u32 alen, hi_u32 tlen);

/**
\brief  get work params.
* \param[in]  id The channel number.
* \param[out] ctrl infomation.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_get_config(hi_u32 id, hi_cipher_ctrl *ctrl);

/**
\brief  get work params without lock.
* \param[in]  id The channel number.
* \param[out] ctrl infomation.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_symc_unlock_get_config(hi_u32 id, hi_cipher_ctrl *ctrl);

/**
 * \brief          SYMC  buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param[in]  id The channel number.
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \param length   length of the input data
 * \param operation  decrypt or encrypt
 * \param oddkey     odd key or not
 *
 * \return         0 if successful
 */
hi_s32 kapi_symc_crypto(hi_u32 id, compat_addr input,
                     compat_addr output, hi_u32 length,
                     hi_u32 operation, hi_u32 oddkey, hi_u32 last);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param[in]  id The channel number.
 * \param pkg       Buffer of package infomation
 * \param pkg_num   Number of package infomation
 * \param operation  decrypt or encrypt
 * \param last       last or not
 *
 * \return         0 if successful
 */
hi_s32 kapi_symc_crypto_multi(hi_u32 id, const hi_void *pkg,
                           hi_u32 pkg_num, hi_u32 operation, hi_u32 last);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param[in]  id The channel number.
 * \param pkg       Buffer of package infomation
 * \param pkg_num   Number of package infomation
 * \param operation  decrypt or encrypt
 * \param last       last or not
 *
 * \return         0 if successful
 */
hi_s32 kapi_symc_crypto_multi_asyn(hi_u32 id, const hi_void *pkg,
                                hi_u32 pkg_num, hi_u32 operation, hi_u32 last);

/**
 * \brief          SYMC wait done.
 *
 * \param[in]  id The channel number.
 *
 * \return         0 if successful
 */
hi_s32 kapi_symc_crypto_waitdone(hi_u32 id, hi_u32 timeout);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 * \param[in]  id The channel number.
 * \param[in]  tag tag data of CCM/GCM
 *
 * \return         0 if successful
 */
hi_s32 kapi_aead_get_tag(hi_u32 id, hi_u32 tag[AEAD_TAG_SIZE_IN_WORD], hi_u32 *taglen);

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_hash_init(hi_void);

/**
\brief   Kapi Deinit.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_hash_deinit(hi_void);

/**
 * \brief          HASH context setup.
 *
 *
 * \param[out] id The channel number.
 * \param[in] type    Hash type
 * \param[in] key     hmac key
 * \param[in] keylen  hmac key length
 *
 * \return         0 if successful
 */
hi_s32 kapi_hash_start(hi_u32 *id, hi_cipher_hash_type type,
                    hi_u8 *key, hi_u32 keylen);

/**
 * \brief          HASH process buffer.
 *
 * \param[in]  id The channel number.
 * \param[in] input    buffer holding the input data
 * \param[in] length   length of the input data
 * \param[in] src      source of hash message
 *
 * \return         0 if successful
 */
hi_s32 kapi_hash_update(hi_u32 id, hi_u8 *input, hi_u32 length,
                     hash_chunk_src src);

/**
 * \brief          HASH final digest.
 *
 * \param[in]  id The channel number.
 * \param[out] hash    buffer holding the hash data
 * \param[in]  hash_buf_len    the length of hash buffer
 * \param[out] hashlen length of the hash data
 *
 * \return         0 if successful
 */
hi_s32 kapi_hash_finish(hi_u32 id, hi_u8 *hash, hi_u32 hash_buf_len, hi_u32 *hashlen);

/**
\brief   hash release.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_hash_release(hi_void);

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_rsa_init(hi_void);

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_rsa_deinit(hi_void);

/**
* \brief RSA encryption a plaintext with a RSA private key.
*
* \param[in] key:       rsa key struct.
* \param[in] scheme:  rsa encrypt/decrypt scheme.
* \param[in] in   input data to be encryption
* \param[out] inlen:  length of input data to be encryption
* \param[out] out   output data of encryption
* \param[out] outlen: length of output data to be encryption
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_rsa_encrypt(cryp_rsa_key *key,
                     hi_cipher_rsa_enc_scheme scheme,
                     hi_u8 *in, hi_u32 inlen,
                     hi_u8 *out, hi_u32 *outlen);

/**
* \brief RSA decryption a ciphertext with a RSA public key.
*
* \param[in] key:       rsa key struct.
* \param[in] scheme:  rsa encrypt/decrypt scheme.
* \param[in] in   input data to be encryption
* \param[in] inlen:  length of input data to be encryption
* \param[out] out   output data to be encryption
* \param[out] outlen: length of output data to be encryption
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_rsa_decrypt(cryp_rsa_key *key,
                     hi_cipher_rsa_enc_scheme scheme,
                     hi_u8 *in, hi_u32 inlen,
                     hi_u8 *out, hi_u32 *outlen);

/**
* \brief RSA signature a context with appendix, where a signers RSA private key is used.
*
* \param[in] key:       rsa key struct.
* \param[in] scheme:  rsa signature/verify scheme.
* \param[in] in    input data to be encryption
* \param[in] inlen:  length of input data to be encryption
* \param[in] hash:   hash value of context,if NULL, let hash = Hash(context) automatically
* \param[out] out   output data to be encryption
* \param[out] outlen: length of output data to be encryption
* \param[in]  src      source of hash message
* \param[in]  uuid uuid The user identification.
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_rsa_sign_hash(cryp_rsa_key *key,
                       hi_cipher_rsa_sign_scheme scheme,
                       hi_u8 *hash, hi_u32 hlen,
                       hi_u8 *sign, hi_u32 *signlen);

/**
* \brief RSA verify a ciphertext with a RSA public key.
*
* \param[in]  key_info:   encryption struct.
* \param[in]  in   input data to be encryption
* \param[out] inlen:  length of input data to be encryption
* \param[in]  hash:   hash value of context,if NULL, let hash = Hash(context) automatically
* \param[out] out   output data to be encryption
* \param[out] outlen: length of output data to be encryption
* \param[in]  src      source of hash message
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_rsa_verify_hash(cryp_rsa_key *key,
                         hi_cipher_rsa_sign_scheme scheme,
                         hi_u8 *hash, hi_u32 hlen,
                         hi_u8 *sign, hi_u32 signlen);

/**
* \brief RSA gen key.
*
* \param[in]  numbits   bits num of N
* \param[in]  exponent  e
* \param[in]  key_info:   encryption struct.
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_rsa_gen_key(hi_u32 numbits, hi_u32 exponent, cryp_rsa_key *key);

/**
* \brief set iv.
*
* \param[in]  handle:   handle.
* \param[in]  iv   iv data
* \param[in]  inlen:  length of input data to be encryption
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_symc_setiv(hi_u32 handle, hi_u8 *iv, hi_u32 ivlen);

/**
* \brief get iv.
*
* \param[in]  handle:   handle.
* \param[in]  iv   iv data
* \param[in]  inlen:  length of input data to be encryption
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_s32 kapi_symc_getiv(hi_u32 handle, hi_u8 *iv, hi_u32 iv_buf_len, hi_u32 *ivlen);

/**
* \brief get mode.
*
* \param[in]  handle:   handle.
* \retval ::HI_SUCCESS  Call this API successful
* \retval ::HI_FAILURE  Call this API fails.
*/
hi_cipher_work_mode kapi_symc_getmode(hi_u32 handle);

/**
* \brief  HDCP encrypt/decrypt use AES-CBC, IV is all zero.
* \param[in]  keysel Hdcp root key sel.
* \param[in]  ramsel hdmi ram sel.
* \param[in]  in The input data.
* \param[out] out The input data.
* \param[out] len The input data length, must align with 16.
* \retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_hdcp_encrypt(hi_cipher_hdcp_attr *attr, const hi_u8 *in, hi_u8 *out, hi_u32 len, hi_u32 decrypt);

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_trng_init(hi_void);

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_trng_deinit(hi_void);

/**
\brief get rand number.
\param[out]  randnum rand number.
\param[in]   timeout time out.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_trng_get_random( hi_u32 *randnum, hi_u32 timeout);

/**
\brief get rand bytes.
\param[out]  randnum rand bytes.
\param[in]   size size of rand byte.
\param[in]   timeout time out.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_trng_get_rand_byte(hi_u8 *randbyte, hi_u32 size, hi_u32 timeout);

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_init(hi_void);

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_deinit(hi_void);

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecc_init(hi_void);

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecc_deinit(hi_void);

/**
\brief SM2 signature a context with appendix, where a signers SM2 private key is used.
\param d[in]     sm2 private key
\param px[in]    sm2 x public key
\param py[in]    sm2 y public key
\param id[in]    sm2 user id
\param idlen[in] length of sm2 user id
\param msg[in]   mesaage to be sign
\param msglen[in] length of mesaage to be sign
\param src[in]    source of hash message
\param r[out]      sm2 sign result of r
\param s[out]      sm2 sign result of s
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_sign(hi_u32 d[SM2_LEN_IN_WROD],
                  hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                  hi_u8 *id, hi_u16 idlen,
                  hi_u8 *msg, hi_u32 msglen, hash_chunk_src src,
                  hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD]);
/**
\brief SM2 signature verification a context with appendix, where a signers SM2 public key is used.
\param px[in]    sm2 x public key
\param py[in]    sm2 y public key
\param id[in]    sm2 user id
\param idlen[in] length of sm2 user id
\param msg[in]   mesaage to be sign
\param msglen[in] length of mesaage to be sign
\param src[in]    source of hash message
\param r[in]      sm2 sign result of r
\param s[in]      sm2 sign result of s
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_verify(hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                    hi_u8 *id, hi_u16 idlen, hi_u8 *msg, hi_u32 msglen, hash_chunk_src src,
                    hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD]);

/**
\brief SM2 encryption a plaintext with a RSA public key.
\param px[in]    sm2 x public key
\param py[in]    sm2 y public key
\param msg[in]   mesaage to be encryption
\param msglen[in] length of mesaage to be encryption
\param enc[out]    encryption mesaage
\param enclen[out] length of encryption mesaage
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_encrypt(hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                     hi_u8 *msg, hi_u32 msglen, hi_u8 *enc, hi_u32 *enclen);
/**
\brief SM2 decryption a plaintext with a RSA public key.
\param d[in]     sm2 private key
\param enc[out]    mesaage to be decryption
\param enclen[out] length of mesaage to be decryption
\param msg[in]     decryption mesaage
\param msglen[in]  length of decryption mesaage
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_decrypt(hi_u32 d[SM2_LEN_IN_WROD], hi_u8 *enc,
                     hi_u32 enclen, hi_u8 *msg, hi_u32 *msglen);

/**
\brief Generate a SM2 key pair..
\param d[in]     sm2 private key
\param px[in]    sm2 x public key
\param py[in]    sm2 y public key
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_sm2_gen_key(hi_u32 d[SM2_LEN_IN_WROD],
                     hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD]);


/**
\brief CENC decryption a ciphertext.
\param id[in] Cipher handle
\param key[in] for cipher decryption
\param iv[in] for cipher decryption
\param inphy[in] non-secure Physical address of the source data
\param outphy[in] secure Physical address of the target data
\param length[in] Length of the decrypted data
\param firstoffset[in] offset of the first encrypt block data
\retval HI_SUCCESS  Call this API succussful.
\retval HI_FAILURE  Call this API fails.
\see \n
N/A
*/
hi_s32 kapi_symc_cenc_decrypt(hi_u32 id,
                             hi_cipher_cenc *cenc,
                             compat_addr inputphy,
                             compat_addr outputphy,
                             hi_u32 bytelength);

/**
\brief   Get user uuid.
\param[in]  id The channel number.
\param[out] the point to user uuid.
\retval     NA.
*/
hi_void kapi_symc_get_user_uuid(hi_u32 id, pid_t *uuid);

/**
* \brief Compute ECDH shared secret key. This key corresponds to the X coordinates of the computed P point.
* \param d         ecc private key
* \param px        ecc x public key
* \param py        ecc y public key
* \param sharekey  ecdh share key
* \retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecdh_compute_key(ecc_param_t *ecc, hi_u8 *d, hi_u8 *px, hi_u8 *py, hi_u8 *sharekey);

/**
* \brief Generate a ECC key pair.
* \param inkey    input private key
* \param outkey   input private key
* \param px       ECC x public key
* \param py       ECC y public key
* \retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecc_gen_key(ecc_param_t *ecc, hi_u8 *inkey, hi_u8 *outkey, hi_u8 *px, hi_u8 *py);

/**
* \brief ECDSA signature a context with appendix, where a signers ECC private key is used.
* \param d         ECC private key
* \param hash      hash to be sign
* \param hlen      length of hash to be sign
* \param r         ECC sign result of r
* \param s         ECC sign result of s
* \retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecdsa_sign_hash(ecc_param_t *ecc, hi_u8 *d, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s);

/**
* \brief ECDSA signature verification a context with appendix, where a signers ECC public key is used.
* \param px       ECC x public key
* \param py       ECC y public key
* \param hash     hash to be verification
* \param hlen     length of hash to be verification
* \param r        ECDSA sign result of r
* \param s        ECDSA sign result of s
* \retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 kapi_ecdsa_verify_hash(ecc_param_t *ecc, hi_u8 *px, hi_u8 *py, hi_u8 *hash, hi_u32 hlen, hi_u8 *r, hi_u8 *s);
/** @}*/  /** <!-- ==== API Code end ====*/

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* End of #ifndef __HI_DRV_CIPHER_H__*/

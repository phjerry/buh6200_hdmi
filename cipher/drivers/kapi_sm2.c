/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of kapi_sm2
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_sm2.h"

#ifdef HI_PRODUCT_SM2_SUPPORT
/************************* Internal Structure Definition *********************/
/** \addtogroup      sm2 */
/** @{ */ /** <!-- [sm2] */

/* max length of SM2 ID */
#define SM2_ID_MAX_LEN 0x1FFF

/* SM2 PC of uncompress */
#define SM2_PC_UNCOMPRESS 0x04

/* SM2 encrypt try count */
#define SM2_ENCRYPT_TRY_CNT 10

#define BYTE_BITS       8
#define SM2_ENC_PAD_LEN (SM2_LEN_IN_BYTE * 3)

static ecc_param_t sm2_id = {
    .p = (hi_u8*) "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    .a = (hi_u8*) "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC",
    .b = (hi_u8*) "\x28\xE9\xFA\x9E\x9D\x9F\x5E\x34\x4D\x5A\x9E\x4B\xCF\x65\x09\xA7"
                  "\xF3\x97\x89\xF5\x15\xAB\x8F\x92\xDD\xBC\xBD\x41\x4D\x94\x0E\x93",
    .n = (hi_u8*) "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\x72\x03\xDF\x6B\x21\xC6\x05\x2B\x53\xBB\xF4\x09\x39\xD5\x41\x23",
    .gx = (hi_u8*)"\x32\xC4\xAE\x2C\x1F\x19\x81\x19\x5F\x99\x04\x46\x6A\x39\xC9\x94"
                  "\x8F\xE3\x0B\xBF\xF2\x66\x0B\xE1\x71\x5A\x45\x89\x33\x4C\x74\xC7",
    .gy = (hi_u8*)"\xBC\x37\x36\xA2\xF4\xF6\x77\x9C\x59\xBD\xCE\xE3\x6B\x69\x21\x53"
                  "\xD0\xA9\x87\x7C\xC6\x2A\x47\x40\x02\xDF\x32\xE5\x21\x39\xF0\xA0"
};

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      sm2 */
/** @{ */ /** <!-- [kapi] */

/**
\brief   Kapi Init.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 kapi_sm2_init(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = cryp_sm2_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(cryp_sm2_init, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief   Kapi Deinitialize.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 kapi_sm2_deinit(void)
{
    hi_log_func_enter();

    cryp_sm2_deinit();

    hi_log_func_exit();
    return HI_SUCCESS;
}

static hi_s32 sm2_kdf(hi_u32 *x, hi_u32 *y, hi_u32 xy_len, hi_u8 *k, hi_u32 klen)
{
    hi_s32 ret;
    hi_u32 sm3id = 0;
    hi_u32 hi_u32ct = 0;
    hi_u8 h[SM2_LEN_IN_BYTE] = { 0 };
    hi_u32 i;
    hi_u32 block;
    hi_u32 hashlen;

    hi_log_func_enter();

    if ((xy_len / sizeof(hi_u32)) < SM2_LEN_IN_WROD) {
        hi_log_error("Invalid xy_len.\n");
        return HI_ERR_CIPHER_INVALID_PARA;
    }
    if (klen == 0) {
        return HI_SUCCESS;
    }

    block = (klen + SM2_LEN_IN_BYTE - 1) / SM2_LEN_IN_BYTE;

    for (i = 0; i < block; i++) {
        hi_u32ct = cipher_cpu_to_be32(i + 1);

        /* *** H = SM3(X || Y || CT) *** */
        check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)x, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)y, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)&hi_u32ct, sizeof(hi_u32ct), HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_finish(sm3id, h, sizeof(h), &hashlen));
        if (i == (block - 1)) {
            check_exit(memcpy_s(k + i * SM2_LEN_IN_BYTE, SM2_LEN_IN_BYTE,
                                h, klen - i * SM2_LEN_IN_BYTE));
        } else {
            check_exit(memcpy_s(k + i * SM2_LEN_IN_BYTE, SM2_LEN_IN_BYTE, h, sizeof(h)));
        }
    }

    /* check k != 0 */
    for (i = 0; i < klen; i++) {
        if (k[i] != 0) {
            break;
        }
    }
    if (i >= klen) {
        hi_log_error("Invalid key.\n");
        return HI_ERR_CIPHER_ILLEGAL_KEY;
    }

    hi_log_func_exit();
    return HI_SUCCESS;

exit__:

    return ret;
}

hi_s32 kapi_sm2_sign(hi_u32 d[SM2_LEN_IN_WROD],
                     hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                     hi_u8 *id, hi_u16 idlen,
                     hi_u8 *msg, hi_u32 msglen, hash_chunk_src src,
                     hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD])
{
    hi_s32 ret;
    hi_u32 sm3id = 0;
    hi_u8 entla[2] = { 0 }; /* 2 byte entla */
    hi_u8 ZA[SM2_LEN_IN_BYTE] = { 0 };
    hi_u32 e[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 hashlen = 0;
    sm2_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(idlen > SM2_ID_MAX_LEN);
    hi_log_check_param(d == HI_NULL);
    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);
    hi_log_check_param(id == HI_NULL);
    hi_log_check_param(r == HI_NULL);
    hi_log_check_param(s == HI_NULL);
    if (msglen > 0) {
        hi_log_check_param(msg == HI_NULL);
    }

    func = cryp_get_sm2_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->sign == HI_NULL);

    /* msg is the result of sm3 ? */
    if ((idlen == SM3_RESULT_SIZE) && (msglen == 0x00)) {
        /* id is the sm3 result when msglen = 0 and idlen = 32 */
        check_exit(memcpy_s(e, sizeof(e), id, SM3_RESULT_SIZE));
    } else {
        /* compute e */
        entla[0] = (hi_u8)((idlen * BYTE_BITS) >> BYTE_BITS);
        entla[1] = (hi_u8)((idlen * BYTE_BITS));

        /* *** ZA=H256(ENTLA||IDA||a||b||xG||yG||xA||yA) *** */
        /* SM2 and sm3 use two different lock, so have to use SM3 Instead of MPI */
        check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
        check_exit(kapi_hash_update(sm3id, entla, sizeof(entla), HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, id, idlen, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.a, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.b, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.gx, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.gy, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)px, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)py, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_finish(sm3id, ZA, sizeof(ZA), &hashlen));

        /* *** e=H256(ZA||M) *** */
        check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
        check_exit(kapi_hash_update(sm3id, ZA, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, msg, msglen, src));
        check_exit(kapi_hash_finish(sm3id, (u8 *)e, sizeof(e), &hashlen));

        hi_log_debug("msg 0x%p, msglen %d\n", msg, msglen);
    }

    /***************************************
      1. generate a randnum k, k?[1,n-1],
      2. (x1,y1)=[k]G,
      3. r=(e+x1)mod n, if r=0 or r+k=n, return step 1
      4. s=((1+dA)^-1 ?(k-r?dA))mod n
    ****************************************/
    ret = func->sign(e, d, r, s, SM2_LEN_IN_BYTE, &sm2_id);
    if (ret != HI_SUCCESS) {
        crypto_zeroize(e, sizeof(e));
        hi_log_print_func_err(func->sign, ret);
        return ret;
    }

exit__:
    crypto_zeroize(e, sizeof(e));
    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_sm2_verify(hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                       hi_u8 *id, hi_u16 idlen, hi_u8 *msg, hi_u32 msglen, hash_chunk_src src,
                       hi_u32 r[SM2_LEN_IN_WROD], hi_u32 s[SM2_LEN_IN_WROD])
{
    hi_s32 ret;
    hi_u32 sm3id;
    hi_u8 entla[2] = { 0 };  /* 2 byte entla */
    hi_u8 ZA[SM2_LEN_IN_BYTE] = { 0 };
    hi_u32 e[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 hashlen = 0;
    sm2_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(idlen > SM2_ID_MAX_LEN);
    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);
    hi_log_check_param(id == HI_NULL);
    hi_log_check_param(r == HI_NULL);
    hi_log_check_param(s == HI_NULL);
    if (msglen > 0) {
        hi_log_check_param(msg == HI_NULL);
    }

    func = cryp_get_sm2_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->verify == HI_NULL);

    /* msg is the result of sm3 ? */
    if ((idlen == SM3_RESULT_SIZE) && (msglen == 0x00)) {
        /* id is the sm3 result when msglen = 0 and idlen = 32 */
        check_exit(memcpy_s(e, sizeof(e), id, idlen));
    } else {
        /* compute e */
        entla[0] = (hi_u8)((idlen * BYTE_BITS) >> BYTE_BITS);
        entla[1] = (hi_u8)((idlen * BYTE_BITS));

        /* *** ZA=H256(ENTLA||IDA||a||b||xG||yG||xA||yA) *** */
        check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
        check_exit(kapi_hash_update(sm3id, entla, sizeof(entla), HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, id, idlen, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.a, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.b, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.gx, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)sm2_id.gy, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)px, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, (hi_u8 *)py, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_finish(sm3id, ZA, sizeof(ZA), &hashlen));

        /* *** e=H256(ZA||M) *** */
        check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
        check_exit(kapi_hash_update(sm3id, ZA, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
        check_exit(kapi_hash_update(sm3id, msg, msglen, src));
        check_exit(kapi_hash_finish(sm3id, (u8 *)e, sizeof(e), &hashlen));

        hi_log_debug("msg 0x%p, msglen %d\n", msg, msglen);
    }

    /***************************************
      1. t=(r+s)mod n, if t==0, return fail
      2. (x1,y1)=[s]G+tP,
      3. r=(e+x1)mod n, if r==R, return pass
    ****************************************/
    ret = func->verify(e, px, py, r, s, SM2_LEN_IN_BYTE, &sm2_id);
    if (ret != HI_SUCCESS) {
        crypto_zeroize(e, sizeof(e));
        hi_log_print_func_err(func->verify, ret);
        return ret;
    }

exit__:
    crypto_zeroize(e, sizeof(e));
    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_sm2_encrypt(hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD],
                        hi_u8 *msg, hi_u32 msglen, hi_u8 *enc, hi_u32 *enclen)
{
    hi_s32 ret;
    hi_u32 sm3id = 0;
    hi_u32 c1_index;
    hi_u32 c2_index;
    hi_u32 c3_index;
    hi_u32 i;
    hi_u32 c1x[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 c1y[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 x2[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 y2[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 hashlen = 0;
    hi_u32 trycnt = 0;
    sm2_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);
    hi_log_check_param(msg == HI_NULL);
    hi_log_check_param(enc == HI_NULL);
    hi_log_check_param(enclen == HI_NULL);

    func = cryp_get_sm2_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->encrypt == HI_NULL);

    c1_index = 0;
    c3_index = c1_index + SM2_LEN_IN_BYTE + SM2_LEN_IN_BYTE;
    c2_index = c3_index + SM2_LEN_IN_BYTE;

    /* *** C1 = k * G *** */
    while (trycnt++ < SM2_ENCRYPT_TRY_CNT) {
        /* *** C1(x, y) = k * G(x, y), XY = k * P(x, y) *** */
        ret = func->encrypt(px, py, c1x, c1y, x2, y2, SM2_LEN_IN_BYTE, &sm2_id);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(func->encrypt, ret);
            return ret;
        }

        /* the key consist of PC||x1||y1, PC = 4 when uncompress */
        ret = memcpy_s(&enc[c1_index], SM2_LEN_IN_BYTE, c1x, SM2_LEN_IN_BYTE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
        c1_index += SM2_LEN_IN_BYTE;
        ret = memcpy_s(&enc[c1_index], SM2_LEN_IN_BYTE, c1y, SM2_LEN_IN_BYTE);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }

        /* gen mask for msg encrypt */
        ret = sm2_kdf(x2, y2, sizeof(x2), &enc[c2_index], msglen);
        if (ret == HI_SUCCESS) {
            break;
        }
    }

    if (ret != HI_SUCCESS) {
        hi_log_error("sm2 encrypt failed\n");
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_ENCRYPT);
        ret = HI_ERR_CIPHER_FAILED_ENCRYPT;
        goto exit__;
    }

    /* *** C2=M ^ k *** */
    for (i = 0; i < msglen; i++) {
        enc[c2_index++] ^= msg[i];
    }

    /* *** C3 =Hash(x2||M||y2) *** */
    check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
    check_exit(kapi_hash_update(sm3id, (hi_u8 *)x2, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
    check_exit(kapi_hash_update(sm3id, msg, msglen, HASH_CHUNCK_SRC_LOCAL));
    check_exit(kapi_hash_update(sm3id, (hi_u8 *)y2, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
    check_exit(kapi_hash_finish(sm3id, &enc[c3_index], SM2_LEN_IN_BYTE, &hashlen));

    *enclen = SM2_ENC_PAD_LEN + msglen;

exit__:
    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_sm2_decrypt(hi_u32 d[SM2_LEN_IN_WROD], hi_u8 *enc,
                        hi_u32 enclen, hi_u8 *msg, hi_u32 *msglen)
{
    hi_s32 ret;
    hi_u8 h[SM2_LEN_IN_BYTE] = { 0 };
    hi_u32 sm3id = 0;
    hi_u32 c1_index;
    hi_u32 c2_index;
    hi_u32 c3_index;
    hi_u32 m_len;
    hi_u32 c1x[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 c1y[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 x2[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 y2[SM2_LEN_IN_WROD] = { 0 };
    hi_u32 hashlen = 0;
    hi_u32 i;
    sm2_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(d == HI_NULL);
    hi_log_check_param(enc == HI_NULL);
    hi_log_check_param(msg == HI_NULL);
    hi_log_check_param(msglen == HI_NULL);
    hi_log_check_param(enclen < SM2_ENC_PAD_LEN);

    func = cryp_get_sm2_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->decrypt == HI_NULL);

    c1_index = 0;
    c3_index = c1_index + SM2_LEN_IN_BYTE + SM2_LEN_IN_BYTE;
    c2_index = c3_index + SM2_LEN_IN_BYTE;
    m_len = enclen - SM2_ENC_PAD_LEN;  /* *** C2 = C - C1 -C3 *** */

    /* C1 */
    ret = memcpy_s(c1x, sizeof(c1x), &enc[c1_index], SM2_LEN_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }
    c1_index += SM2_LEN_IN_BYTE;
    ret = memcpy_s(c1y, sizeof(c1y), &enc[c1_index], SM2_LEN_IN_BYTE);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    /* Compute M(x, y) = C1(x, y) * d */
    ret = func->decrypt(d, c1x, c1y, x2, y2, SM2_LEN_IN_BYTE, &sm2_id);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(func->decrypt, ret);
        return ret;
    }

    /* Compute mask for msg */
    ret = sm2_kdf(x2, y2, sizeof(x2), msg, m_len);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(sm2_kdf, ret);
        return ret;
    }

    /* Get clear msg from C2 */
    for (i = 0; i < m_len; i++) {
        msg[i] ^= enc[c2_index++];
    }

    /* Check C3 */
    check_exit(kapi_hash_start(&sm3id, HI_CIPHER_HASH_TYPE_SM3, HI_NULL, 0));
    check_exit(kapi_hash_update(sm3id, (hi_u8 *)x2, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
    check_exit(kapi_hash_update(sm3id, msg, m_len, HASH_CHUNCK_SRC_LOCAL));
    check_exit(kapi_hash_update(sm3id, (hi_u8 *)y2, SM2_LEN_IN_BYTE, HASH_CHUNCK_SRC_LOCAL));
    check_exit(kapi_hash_finish(sm3id, h, sizeof(h), &hashlen));

    if (osal_memncmp(&enc[c3_index], SM2_LEN_IN_BYTE,  h, SM2_LEN_IN_BYTE) != 0) {
        hi_log_error("Err, SM2 decrypt failed, invalid C3\n");
        hi_log_print_err_code(HI_ERR_CIPHER_FAILED_DECRYPT);
        ret = HI_ERR_CIPHER_FAILED_DECRYPT;
        goto exit__;
    }

    *msglen = m_len;

exit__:
    hi_log_func_exit();

    return ret;
}

hi_s32 kapi_sm2_gen_key(hi_u32 d[SM2_LEN_IN_WROD], hi_u32 px[SM2_LEN_IN_WROD], hi_u32 py[SM2_LEN_IN_WROD])
{
    hi_s32 ret;
    sm2_func *func = HI_NULL;

    hi_log_func_enter();

    hi_log_check_param(d == HI_NULL);
    hi_log_check_param(px == HI_NULL);
    hi_log_check_param(py == HI_NULL);

    func = cryp_get_sm2_op();
    hi_log_check_param(func == HI_NULL);
    hi_log_check_param(func->genkey == HI_NULL);

    /* d is randnum, P(x,y) =d * G(x,y) */
    ret = func->genkey(d, px, py, SM2_LEN_IN_BYTE, &sm2_id);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(func->genkey, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}
#endif

/** @} */ /** <!-- ==== Structure Definition end ==== */

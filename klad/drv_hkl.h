/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hisilicon keyladder driver.
* Author : Hisilicon security team
* Created : 2019-06-125
*/

#ifndef __DRV_HKL_H__
#define __DRV_HKL_H__

#include "drv_klad_hw_define.h"
#include "drv_ioctl_klad.h"
#include "hal_klad.h"
#include "drv_rkp.h"
#include "hal_rkp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


enum hkl_mgmt_state {
    HKL_MGMT_CLOSED = 0x0,
    HKL_MGMT_OPENED,
};

struct core_wait {
    osal_mutex              lock;
    osal_wait               wait_queue;
    hi_u32                  wait_cond;
};

struct hkl_wait {
    osal_mutex              lock;
    osal_wait               wait_queue;
    hi_u32                  wait_cond;
    hi_u32                  lock_info;
};

struct hkl_mgmt {
    osal_mutex                  lock;
    enum hkl_mgmt_state         state;

    void                        *io_base ;
    struct core_wait            kl_alg_ip;
    struct hkl_wait             kl_csgk2_lock;
    struct hkl_wait             kl_ta_lock;
    struct hkl_wait             kl_fp_lock;
    struct hkl_wait             kl_clr_lock;
    struct hkl_wait             kl_nonce_lock;
    struct hkl_wait             kl_com_lock;

    struct time_ns              time_ctl_b;
    struct time_ns              time_ctl_e;
    struct time_ns              time_int_b;
    struct time_ns              time_int_e;
};

typedef enum {
    KLAD_ALG_TDES         = 0x00,
    KLAD_ALG_AES          = 0x01,
    KLAD_ALG_SM4          = 0x02,
    KLAD_ALG_RSD          = 0x03,
} klad_alg_sel;

typedef enum {
    PORT_SEL_TSCIPHER         = 0x00,
    PORT_SEL_MCIPHER          = 0x01,
    PORT_SEL_RSD              = 0x02,
    PORT_SEL_SCIPHER          = 0x03,
} klad_port_sel;

typedef struct {
    hi_u32 key_dec_support;
    hi_u32 key_enc_support;
    unsigned int dsc_mode;
    klad_port_sel port_sel;
} target_cfg;

typedef struct {
    hi_u32 hpp_only;
    hi_u32 dest_sec;
    hi_u32 dest_nsec;
    hi_u32 src_sec;
    hi_u32 src_nsec;
    hi_u32 key_sec;
} target_sec_cfg;

typedef struct {
    hi_u32 is_odd;
    unsigned int key_slot_num;
    target_cfg tgt_cfg;
    target_sec_cfg tgt_sec_cfg;
} klad_cfg;

typedef struct {
    klad_alg_sel klad_alg[HKL_LEVEL];
    hi_u8 session_key[HKL_LEVEL][HKL_KEY_LEN];
    unsigned int key_abort;
    klad_cfg cfg;
} common_klad;

typedef struct {
    hi_u32 is_odd;
    unsigned int key_slot_num;
    klad_port_sel port_sel;
    hi_u8 iv[HKL_KEY_LEN];
} clear_iv;

typedef struct {
    hi_u8 is_dec;
    hi_u8 session_key[HKL_KEY_LEN];
} fp_ext_klad;

typedef struct {
    common_klad com_klad;
    fp_ext_klad fpk_ext_klad;
} fp_klad;

typedef struct {
    unsigned int level;
    hi_u8 key_crc[HKL_LEVEL];
} common_klad_crc;

typedef struct {
    common_klad_crc com_klad_crc;
    hi_u8 fpk_crc;
} fp_klad_crc;

typedef struct {
    hi_u32 clear_key[0x4];
} fp_klad_reg;

typedef enum {
    KLAD_CLR_64           = 0x00,
    KLAD_CLR_128          = 0x01,
    KLAD_CLR_192          = 0x02,
    KLAD_CLR_256          = 0x03,
    KLAD_CLR_MAX          = 0x04,
} klad_clr_key;

/* entry API */
hi_s32 hi_klad_clr_entry_start(struct klad_r_clr_route *clr_route, const hi_klad_clr_entry *instance);
hi_s32 hi_klad_rkp_entry_start(struct klad_r_com_hkl *com_hkl);
hi_s32 hi_klad_com_entry_start(struct klad_r_com_hkl *com_hkl, const hi_klad_com_entry *instance);

/* API */
hi_s32 hi_klad_com_start(rkp_deob_kdf *deob_kdf, common_klad *com_klad, hi_u32 com_kl_num,
                         common_klad_crc *com_klad_crc, rkp_deob_kdf_rst *deob_kdf_rst);
hi_s32 hi_klad_com_process(rkp_deob_kdf *deob_kdf, common_klad *com_klad, common_klad_crc *com_klad_crc,
                           rkp_deob_kdf_rst *deob_kdf_rst);
hi_s32 hi_klad_fp_start(rkp_deob_kdf *deob_kdf, fp_klad *fpk_klad, hi_u32 com_kl_num,
                        fp_klad_crc *fpk_klad_crc, rkp_deob_kdf_rst *deob_kdf_rst);
hi_s32 hi_klad_fp_process(rkp_deob_kdf *deob_kdf, fp_klad *fpk_klad,
                          fp_klad_crc *fpk_klad_crc, rkp_deob_kdf_rst *deob_kdf_rst);
hi_s32 hi_klad_fp_generate(fp_klad_reg *klad_clear);
hi_s32 hi_klad_fp_send(void);
hi_s32 hi_klad_fp_rd_disable(void);
hi_s32 hi_klad_clrcw_process(common_klad *com_klad, klad_clr_key key_len);

hi_s32 hi_klad_clriv_process(clear_iv *clr_iv);

hi_s32 hi_klad_clrcw_lock(void);
hi_s32 hi_klad_clrcw_unlock(void);
hi_s32 hi_klad_com_lock(hi_u32 *com_kl_num);
hi_s32 hi_klad_com_unlock(hi_u32 com_kl_num);
hi_s32 hi_klad_ta_lock(void);
hi_s32 hi_klad_ta_unlock(void);
hi_s32 hi_klad_fp_lock(void);
hi_s32 hi_klad_fp_unlock(void);
hi_s32 hi_klad_nonce_lock(void);
hi_s32 hi_klad_nonce_unlock(void);
hi_s32 hi_klad_csgk2_lock(void);
hi_s32 hi_klad_csgk2_unlock(void);

#if HI_INT_SUPPORT
hi_void hkl_int_deinit(hi_void);
hi_void hkl_int_init(hi_void);
#endif
#ifndef HI_TEE_SUPPORT
hi_void hkl_perm_multi2_cfg(hi_void);
#endif
struct hkl_mgmt *__get_hkl_mgmt(hi_void);

hi_s32 hkl_mgmt_init(hi_void);
hi_void hkl_mgmt_exit(hi_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif //__DRV_HKL_H__


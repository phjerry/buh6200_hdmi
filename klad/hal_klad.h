/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KLAD driver in register level.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __HAL_KLAD_H__
#define __HAL_KLAD_H__

#include "drv_klad_hw_define.h"
#include "drv_klad_reg.h"

#define HKL_LEVEL 8
#define HKL_KEY_LEN 16

/* Note: The enumeration value is fixed, same as register 'KL_LOCK_CTRL.kl_lock_num. */
typedef enum {
    HKL_LOCK_KLAD_COM            = 0,
    HKL_LOCK_KLAD_NONCE          = 1,
    HKL_LOCK_KLAD_CLR            = 2,
    HKL_LOCK_KLAD_FP             = 3,
    HKL_LOCK_KLAD_TA             = 4,
    HKL_LOCK_KLAD_CSGK2          = 5,
    HKL_LOCK_KLAD_MAX
} hkl_lock_klad;

#define CHAR_2_WORD(a, b, c, d) \
    (((hi_u32)(a) << 24) | ((hi_u32)(b) << 16) | ((hi_u32)(c) << 8) | (d))

hi_s32 hkl_com_lock(hi_u32 *com_kl_num);
hi_s32 hkl_com_unlock(hi_u32 com_kl_num);
hi_u32 hkl_is_idle(void);
hi_s32 hkl_wait_idle(void);
void hkl_int_enable(hi_u32 int_enable);
void hkl_set_addr(hi_u16 addr, hi_u32 is_odd);
void hkl_set_cfg(hi_u32 dec_support, hi_u32 enc_support, hi_u8 port, hi_u8 dsc_code);
void hkl_set_sec_cfg(hi_u32 dest_sec, hi_u32 dest_nsec,
                     hi_u32 src_sec, hi_u32 src_nsec, hi_u32 key_sec);
hi_s32 hkl_set_data_in(hi_u8 *data, hi_u32 len);
void hkl_set_com_ctrl(hi_u8 com_kl_num, hi_u8 level, hi_u8 alg, hi_u32 to_ta_kl, hi_u32 key_abort);
hi_u8 hkl_get_crc(void);
void hkl_dump_crc(void);
void hkl_dump_dfx(void);
void hkl_dump_com_tag(hi_u8 com_kl_num);
hi_s32 hkl_check_err(void);
hi_s32 hkl_wait_finish(void);
hi_s32 hkl_fp_lock(void);
hi_s32 hkl_fp_unlock(void);
hi_s32 hkl_ta_lock(void);
hi_s32 hkl_ta_unlock(void);
hi_s32 hkl_nonce_lock(void);
hi_s32 hkl_nonce_unlock(void);
void hkl_set_fp_lev1(hi_u8 is_dec, hi_u32 com_kl_num);
void hkl_set_fp_lev2(void);
hi_s32 hkl_set_fp_rd_disable(void);
hi_s32 hkl_generate_fp_dec_key(hi_u32 *rst0, hi_u32 *rst1, hi_u32 *rst2, hi_u32 *rst3);
hi_s32 hkl_set_clr_ctrl(hi_s32 key_size, hi_bool is_iv);
hi_s32 hkl_clrcw_lock(void);
hi_s32 hkl_clrcw_unlock(void);
hi_s32 hkl_set_csgk2_ctrl(void);
hi_s32 hkl_csgk2_lock(void);
hi_s32 hkl_csgk2_unlock(void);
hi_s32 hkl_set_csgk2_disable(void);
void hkl_get_cipher_key(void);
hi_s32 hal_klad_init(hi_void);
hi_void hal_klad_exit(hi_void);

hi_s32 hkl_check_fp_lock_stat(void);
hi_s32 hkl_check_ta_lock_stat(void);
hi_s32 hkl_check_nonce_lock_stat(void);
hi_s32 hkl_check_clrcw_lock_stat(void);
hi_s32 hkl_check_csgk2_lock_stat(void);
hi_s32 hkl_check_com_lock_stat(hi_u32 kl_index);

#ifdef HI_INT_SUPPORT
hi_void hkl_int_write_reg(const hi_u32 addr, const hi_u32 val);
hi_u32 hkl_int_read_reg(const hi_u32 addr);
hi_void hal_klad_lock_int_en(hi_void);
hi_void hal_klad_lock_int_dis(hi_void);
hi_void hal_klad_csgk2_lock_int_clr(hi_void);
hi_void hal_klad_ta_lock_int_clr(hi_void);
hi_void hal_klad_fp_lock_int_clr(hi_void);
hi_void hal_klad_clr_lock_int_clr(hi_void);
hi_void hal_klad_nonce_lock_int_clr(hi_void);
hi_void hal_klad_com_lock_int_clr(hi_void);
hi_void hal_klad_lock_int_clr(hi_void);

hi_void hal_klad_int_en(hi_void);
hi_void hal_klad_int_dis(hi_void);
hi_void hal_klad_int_clr(hi_void);
hi_u32 hal_klad_get_int_stat(hi_void);

#endif
hi_u32 hkl_read_reg(const hi_u32 addr);

struct hkl_lock_stat {
    hi_u8 com_lock_stat[KLAD_COM_CNT];
    hi_u8 ta_lock_stat;
    hi_u8 fp_lock_stat;
    hi_u8 nonce_lock_stat;
    hi_u8 clr_lock_stat;
};

void hkl_get_klad_lock_info(struct hkl_lock_stat *stat);

#endif /* __HAL_KLAD_H__ */

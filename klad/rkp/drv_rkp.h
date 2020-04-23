/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :hpp rom keyladder.
* Author : Hisilicon security team
* Created : 2019-03-13
*/
#ifndef __DRV_RKP_H__
#define __DRV_RKP_H__

#include "hal_rkp.h"
#include "drv_rkp_dbg.h"
#include "hi_drv_klad.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define LEN_8  8
#define LEN_16 16

enum rkp_mgmt_state {
    RKP_MGMT_CLOSED = 0x0,
    RKP_MGMT_OPENED,
};

/* define the union rkp_rk_cas_flag */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    bb_owner_id           : 7   ; /* [6..0]  */
        hi_u32    reserved_0            : 1   ; /* [7]  */
        hi_u32    ca_owner_id           : 8   ; /* [15..8]  */
        hi_u32    secret_route_enable   : 1   ; /* [16]  */
        hi_u32    csa2_check            : 1   ; /* [17]  */
        hi_u32    chipid_sel            : 2   ; /* [19..18]  */
        hi_u32    key_id_0              : 1   ; /* [20]  */
        hi_u32    key_id_1              : 1   ; /* [21]  */
        hi_u32    reserved_1            : 2   ; /* [32..22]  */
        hi_u32    deobf_select          : 1   ; /* [24]  */
        hi_u32    rk_disable            : 1   ; /* [25]  */
        hi_u32    reserved_2            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_rk_cas_flag;

/* define the union rkp_rk_boot_flag */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    bb_owner_id           : 7   ; /* [6..0]  */
        hi_u32    reserved_0            : 1   ; /* [7]  */
        hi_u32    ca_owner_id           : 8   ; /* [15..8]  */
        hi_u32    reserved_1            : 1   ; /* [16]  */
        hi_u32    csa2_check            : 1   ; /* [17]  */
        hi_u32    reserved_2            : 6   ; /* [23..18]  */
        hi_u32    deobf_select          : 1   ; /* [24]  */
        hi_u32    rk_disable            : 1   ; /* [25]  */
        hi_u32    reserved_3            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_rk_boot_flag;

/* define the union rkp_rk_boot_flag */
typedef union {
    /* define the struct bits */
    struct {
        hi_u32    reserved_0            : 25   ; /* [24..0]  */
        hi_u32    rk_disable            : 1   ; /* [25]  */
        hi_u32    reserved_1            : 6   ; /* [31..26]  */
    } bits;

    /* define an unsigned member */
    hi_u32    u32;
} rkp_rk_stbm_flag;

struct rkp_rk_cas_slot {
    rkp_rk_cas_flag flag;
    hi_bool rk_ready;
};

struct rkp_rk_boot_slot {
    rkp_rk_boot_flag flag;
    hi_bool rk_ready;
};

struct rkp_rk_stbm_slot {
    rkp_rk_stbm_flag flag;
    hi_bool rk_ready;
};

struct rkp_rk_otp_flag {
    struct rkp_rk_cas_slot  cas_slot[0x08]; /* 8 cas slot */
    struct rkp_rk_boot_slot boot_slot;
    struct rkp_rk_stbm_slot stbm_slot;
};

struct rkp_mgmt {
    osal_mutex                  lock;
    enum rkp_mgmt_state         state;

    void                        *io_base;

    struct rkp_rk_otp_flag      rk_flag;

    osal_mutex                  rkp_lock;
    osal_wait                   rkp_wait_queue;
    hi_u32                      rkp_wait_cond;

    struct time_ns              time_ctl_b;
    struct time_ns              time_ctl_e;
    struct time_ns              time_int_b;
    struct time_ns              time_int_e;
};

/*
 * RKP deob definition
 */
typedef struct {
    hi_rkp_slot_choose rootkey_slot;
    /* otp */
    unsigned int deob_key_sel;
    hi_u8 bb_owner_id;
    hi_alg_sel vmask_alg_sel;
    hi_alg_sel clear_rk_alg_sel;
} rkp_deob;

int rkp_deob_start(rkp_deob *deob);

hi_s32 rkp_deob_process(rkp_deob *deob);
int rkp_deob_otp_verify(rkp_deob *deob_data);

/*
 * RKP inte definition
 */
typedef struct {
    hi_rkp_slot_choose rootkey_slot;
    unsigned int chip_index;
    hi_u8 chip_id[LEN_8];
    hi_alg_sel inte_alg_sel;
} rkp_inte;

typedef struct {
    unsigned int check_sum;
} rkp_inte_rst;

int rkp_inte_start(rkp_inte *inte, rkp_inte_rst *inte_rst);
int rkp_inte_otp_verify(rkp_inte *inte);

/*
 * RKP deob+inte definition
 */
typedef struct {
    rkp_deob deob;
    rkp_inte  inte;
} rkp_deob_inte;

typedef struct {
    rkp_deob_rst     deob_rst;
    rkp_inte_rst      inte_rst;
} rkp_deob_inte_rst;

int rkp_deob_inte_start(rkp_deob_inte *deob_inte, rkp_deob_inte_rst *deob_inte_rst);
hi_s32 rkp_deob_inte_verify(rkp_deob_inte *deob_inte);

/*
 * RKP kdf definition
 */
typedef struct {
    hi_rkp_slot_choose rootkey_slot; /* delete in hpp rom but hrf hsl */
    hi_u32 vendor_id;
    hi_u32 cas_kdf_static; /* delete in hpp rom , fixed set xx in hrf hsl */
    hi_u32 unique_type; /* fixed set 0 */
    rkp_module_id_0 module_id0;
    rkp_module_id_1 module_id1;
    rkp_module_id_2 module_id2;
    rkp_module_id_3 module_id3;
    /* otp */
    unsigned short kdf_segment_id;
    unsigned int ca_owner_id;
    unsigned int test_mode;
    hi_u8 die_id[LEN_16]; /* only used header 14 byte */
} rkp_kdf;

typedef struct {
    hi_rkp_klad_sel klad_sel;
    hi_rkp_klad_type_sel klad_type_sel;
} klad_sel;

int rkp_kdf_otp_verify(rkp_kdf *kdf);

/*
 * RKP fixed rk definition
 */
typedef struct {
    hi_rkp_slot_choose rootkey_slot;
    hi_u32 fix_rk_key_sel; /* 0~3:FIXED_RK_KEY_STATIC0~3 */
    hi_u16 fix_rk_key;
    hi_u16 fix_rk_data;
    /* otp */
    hi_u8 bb_owner_id;
    hi_alg_sel clear_rk_alg_sel;
} rkp_fix_rk;

int rkp_fix_otp_verify(rkp_fix_rk *fix_rk);

/*
 * RKP fixed rk + kdf definition
 */
typedef struct {
    rkp_fix_rk fix_rk;
    rkp_kdf kdf;
    klad_sel klad;
} rkp_fix_rk_kdf;

int rkp_fix_rk_otp_verify(rkp_fix_rk_kdf *fix_rk_kdf);
int rkp_fix_eff_rk_start(rkp_fix_rk_kdf *fix_rk_kdf);
hi_s32 rkp_fix_eff_rk_process(rkp_fix_rk_kdf *fix_rk_kdf, hi_u32 com_kl_num);

/*
 * RKP deob + kdf definition
 */
typedef struct {
    rkp_deob deob;
    rkp_kdf  kdf;
    klad_sel klad;
} rkp_deob_kdf;

int rkp_eff_rk_start(rkp_deob_kdf *deob_kdf);
hi_s32 rkp_eff_rk_otp_verify(rkp_deob_kdf *deob_kdf);
hi_s32 rkp_eff_rk_process(rkp_deob_kdf *deob_kdf, hi_rkp_klad_sel klad_sel);
hi_s32 rkp_eff_rk_jtag_process(rkp_deob_kdf *deob_kdf);
#if HI_INT_SUPPORT
hi_void rkp_int_deinit(hi_void);
hi_void rkp_int_init(hi_void);
#endif
struct rkp_mgmt *__get_rkp_mgmt(hi_void);
hi_s32 rkp_mgmt_init(hi_void);
hi_void rkp_mgmt_exit(hi_void);

hi_s32 rkp_cas_slot_find(hi_rootkey_select *root_slot);
hi_u32 otp_shadow_read(hi_u32 addr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_RKP_H__


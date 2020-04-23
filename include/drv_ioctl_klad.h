/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KLAD ioctl defination.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KLAD_IOCTL_H__
#define __DRV_KLAD_IOCTL_H__

#include "hi_drv_module.h"
#include "drv_klad_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    hi_handle create_handle;  /* handle created by driver */
} hi_klad_ctl_handle;

typedef struct {
    hi_handle handle;
    hi_handle target;
} hi_klad_ctl_target;

typedef struct {
    hi_handle handle;
    hi_klad_attr attr;
} hi_klad_ctl_attr;

typedef struct {
    hi_handle handle;
    hi_rootkey_attr rk_attr;
} hi_rk_ctl_attr;

typedef struct {
    hi_handle handle;
    hi_klad_session_key session_key;
} hi_klad_ctl_session_key;

typedef struct {
    hi_handle handle;
    hi_klad_content_key content_key;
} hi_klad_ctl_content_key;

typedef struct {
    hi_handle handle;
    hi_klad_clear_key clear_key;
} hi_klad_ctl_clear_key;

typedef struct {
    hi_handle handle;
    hi_klad_nonce_key nonce_key;
} hi_klad_ctl_nonce_key;

typedef struct {
    hi_handle handle;
} hi_klad_ctl_fp_key;

typedef struct {
    hi_handle handle;
    hi_klad_gen_key gen_key;
} hi_klad_ctl_gen_key;


typedef struct {
    hi_rootkey_attr rk_attr;
    hi_klad_attr attr;
    hi_handle hw_handle;
} hi_klad_create_attr;

typedef struct {
    hi_s64 target_cnt;
    hi_handle target_handle;

    hi_s64 rk_attr_cnt;
    hi_rootkey_attr rk_attr;

    hi_s64 attr_cnt;
    hi_klad_attr attr;

    hi_s64 session_cnt[HI_KLAD_LEVEL_MAX];
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX];

    hi_s64 content_cnt;
    hi_klad_content_key content_key;

    hi_handle hw_handle;
} hi_klad_com_entry;

typedef struct {
    hi_s64 target_cnt;
    hi_handle target_handle;

    hi_s64 attr_cnt;
    hi_klad_attr attr;

    hi_s64 session_ta_cnt;
    hi_klad_ta_key session_ta_key;

    hi_s64 trans_cnt;
    hi_klad_trans_data trans_data;

    hi_s64 content_ta_cnt;
    hi_klad_ta_key content_ta_key;

    hi_handle hw_handle;
} hi_klad_ta_entry;

typedef struct {
    hi_s64 target_cnt;
    hi_handle target_handle;

    hi_s64 attr_cnt;
    hi_klad_attr attr;

    hi_s64 session_cnt[HI_KLAD_LEVEL_MAX];
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX];

    hi_s64 nonce_cnt;
    hi_klad_nonce_key nonce_key;

    hi_handle hw_handle;
} hi_klad_nonce_entry;

typedef struct {
    hi_s64 target_cnt;
    hi_handle target_handle;

    hi_s64 attr_cnt;
    hi_klad_attr attr;

    hi_s64 session_cnt[HI_KLAD_LEVEL_MAX];
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX];

    hi_s64 fp_cnt;
    hi_klad_fp_key fp_key;

    hi_handle hw_handle;
} hi_klad_fp_entry;

typedef struct {
    hi_s64 target_cnt;
    hi_handle target_handle;

    hi_s64 attr_cnt;
    hi_klad_attr attr;

    hi_s64 clr_cnt;
    hi_klad_clear_key clr_key;

    hi_handle hw_handle;
} hi_klad_clr_entry;

#define CMD_KLAD_CREATE                     _IOWR(HI_ID_KLAD, 0x1, hi_klad_ctl_handle)
#define CMD_KLAD_DESTROY                    _IOW (HI_ID_KLAD, 0x2, hi_handle)
#define CMD_KLAD_ATTACH                     _IOW (HI_ID_KLAD, 0x3, hi_klad_ctl_target)
#define CMD_KLAD_DETACH                     _IOW (HI_ID_KLAD, 0x4, hi_klad_ctl_target)
#define CMD_KLAD_GET_ATTR                   _IOWR(HI_ID_KLAD, 0x5, hi_klad_ctl_attr)
#define CMD_KLAD_SET_ATTR                   _IOW (HI_ID_KLAD, 0x6, hi_klad_ctl_attr)
#define CMD_RK_GET_ATTR                     _IOWR(HI_ID_KLAD, 0x25, hi_rk_ctl_attr)
#define CMD_RK_SET_ATTR                     _IOW (HI_ID_KLAD, 0x26, hi_rk_ctl_attr)
#define CMD_KLAD_SET_SESSION_KEY            _IOW (HI_ID_KLAD, 0x7, hi_klad_ctl_session_key)
#define CMD_KLAD_SET_TA_SESSION_KEY         _IOW (HI_ID_KLAD, 0x8, hi_klad_ctl_session_key)
#define CMD_KLAD_SET_CONTENT_KEY            _IOW (HI_ID_KLAD, 0x9, hi_klad_ctl_content_key)
#define CMD_KLAD_SET_CLEAR_KEY              _IOW (HI_ID_KLAD, 0xa, hi_klad_ctl_clear_key)
#define CMD_KLAD_GET_NONCE_KEY              _IOWR(HI_ID_KLAD, 0xb, hi_klad_ctl_nonce_key)
#define CMD_KLAD_FP_KEY                     _IOWR(HI_ID_KLAD, 0xc, hi_klad_ctl_fp_key)
#define CMD_KLAD_GENERATE_KEY               _IOWR(HI_ID_KLAD, 0xd, hi_klad_ctl_gen_key)

#define CMD_KLAD_COM_CREATE                 _IOWR(HI_ID_KLAD, 0x40, hi_klad_create_attr)
#define CMD_KLAD_TA_CREATE                  _IOWR(HI_ID_KLAD, 0x41, hi_klad_create_attr)
#define CMD_KLAD_FP_CREATE                  _IOWR(HI_ID_KLAD, 0x42, hi_klad_create_attr)
#define CMD_KLAD_NONCE_CREATE               _IOWR(HI_ID_KLAD, 0x43, hi_klad_create_attr)

#define CMD_KLAD_COM_STARTUP                _IOWR(HI_ID_KLAD, 0x20, hi_klad_com_entry)
#define CMD_KLAD_TA_STARTUP                 _IOWR(HI_ID_KLAD, 0x21, hi_klad_ta_entry)
#define CMD_KLAD_FP_STARTUP                 _IOWR(HI_ID_KLAD, 0x22, hi_klad_fp_entry)
#define CMD_KLAD_NONCE_STARTUP              _IOWR(HI_ID_KLAD, 0x23, hi_klad_nonce_entry)
/* Need not destory seperately. clear keyladder have not RKP process. */
#define CMD_KLAD_CLR_PROCESS                _IOW(HI_ID_KLAD, 0x24, hi_klad_clr_entry)

#define CMD_KLAD_COM_DESTORY                _IOW(HI_ID_KLAD, 0x30, hi_handle)
#define CMD_KLAD_TA_DESTORY                 _IOW(HI_ID_KLAD, 0x31, hi_handle)
#define CMD_KLAD_FP_DESTORY                 _IOW(HI_ID_KLAD, 0x32, hi_handle)
#define CMD_KLAD_NONCE_DESTORY              _IOW(HI_ID_KLAD, 0x33, hi_handle)

#define CMD_KLAD_MAX                        0xFFFFFFFF


#ifdef __cplusplus
}
#endif
#endif /* __DRV_KLAD_IOCTL_H__ */

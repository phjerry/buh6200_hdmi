/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher private struct and function define.
 */

#ifndef __DRV_TSR2RCIPHER_DEFINE_H__
#define __DRV_TSR2RCIPHER_DEFINE_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_osal.h"

#include "hi_drv_module.h"

#include "drv_tsr2rcipher_ioctl.h"
#include "drv_tsr2rcipher_config.h"
#include "drv_tsr2rcipher_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TSR2RCIPHER_NAME    "HI_TSR2RCIPHER"
#define TSR2RCIPHER_STR_LEN 16

/* general struct */
enum tsc_buf_type {
    TSC_BUF_TYPE_LINK = 0,
    TSC_BUF_TYPE_CYCLE = 1,
    TSC_BUF_TYPE_MAX
};

enum tsc_core_type {
    TSC_CORE_TYPE_AES  = 0,
    TSC_CORE_TYPE_SMS4 = 1,
    TSC_CORE_TYPE_MAX
};

enum tsc_crypt_type {
    TSC_CRYPT_TYPE_EN = 0,
    TSC_CRYPT_TYPE_DE = 1,
    TSC_CRYPT_TYPE_MAX
};

enum tsr2rcipher_mgmt_state {
    TSR2RCIPHER_MGMT_CLOSED = 0x0,
    TSR2RCIPHER_MGMT_OPENED,
};

struct tsr2rcipher_mgmt {
    osal_mutex                     lock;
    enum  tsr2rcipher_mgmt_state   state;
    osal_atomic                    ref_count;
    struct tsr2rcipher_mgmt_ops   *ops;

    struct workqueue_struct       *tsr2rcipher_queue;

    hi_void                       *io_base;
    hi_ulong                       cb_ttbr;

    hi_u32                         ch_cnt;
    osal_mutex                     ch_list_lock;
    osal_spinlock                  ch_list_lock2;
    struct list_head               ch_head;
    DECLARE_BITMAP(ch_bitmap, TSR2RCIPHER_CH_CNT);

    osal_task                     *monitor;

    hi_bool                        rx_int;
    hi_bool                        tx_int;
    hi_bool                        cipher_int;
};

struct tsr2rcipher_r_ch {
    struct tsr2rcipher_r_base    base;         /* !!! it must be first entry. */
    struct tsr2rcipher_r_ch_ops *ops;
    osal_mutex                   lock;
    struct list_head             node;

    hi_bool                      staled;
    hi_handle                    ch_handle;

    tsr2rcipher_alg              alg;
    tsr2rcipher_mode             mode;         /* RAW or PLAYLOAD */
    hi_bool                      is_crc_check; /* CRC or not */

    hi_bool                      is_create_ks;
    hi_handle                    ks_handle;

    hi_bool                      is_odd_key;

    hi_u32                       alg_code;
    enum tsc_core_type           core_type;   /* AES or SMS4 */
    tsr2rcipher_iv_type          iv_type;     /* even or odd */
    hi_u8                        even_iv[TSR2RCIPHER_MAX_IV_LEN];
    hi_u8                        odd_iv[TSR2RCIPHER_MAX_IV_LEN];

    wait_queue_head_t            rx_wait_queue;
    hi_u32                       rx_wait_dsc_rd;
    hi_u32                       rx_wait_pkt_cnt;

    wait_queue_head_t            tx_wait_queue;
    hi_u32                       tx_wait_dsc_rd;
    hi_u32                       tx_wait_pkt_cnt;
    hi_u32                       tx_wait_afull;
};

struct tsr2rcipher_mgmt_ops {
    hi_s32(*init)(struct tsr2rcipher_mgmt *mgmt);
    hi_s32(*exit)(struct tsr2rcipher_mgmt *mgmt);

    hi_s32(*get_cap)(struct tsr2rcipher_mgmt *mgmt, tsr2rcipher_capability *cap);

    hi_s32(*create_ch)(struct tsr2rcipher_mgmt *mgmt, const tsr2rcipher_attr *attr, struct tsr2rcipher_r_ch **rch);
    hi_s32(*destroy_ch)(struct tsr2rcipher_mgmt *mgmt, struct tsr2rcipher_r_ch *rch);

    hi_s32(*suspend)(struct tsr2rcipher_mgmt *mgmt);
    hi_s32(*resume)(struct tsr2rcipher_mgmt *mgmt);

    /* debug helper */
    hi_void(*show_info)(struct tsr2rcipher_mgmt *mgmt);
};

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_TSR2RCIPHER_DEFINE_H__ */

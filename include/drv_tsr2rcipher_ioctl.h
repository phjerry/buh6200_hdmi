/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher ioctl definition.
 */

#ifndef __DRV_TSR2RCIPHER_IOCTL_H__
#define __DRV_TSR2RCIPHER_IOCTL_H__

#include "hi_type.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TSR2RCIPHER_MIN_IV_LEN        (8)
#define TSR2RCIPHER_MAX_IV_LEN        (16)
#define TSR2RCIPHER_MAX_SIZE_PRE_DESC (64 * 1024 * 188)
#define TSR2RCIPHER_TS_PACKAGE_LEN    (188)
#define TSR2RCIPHER_ADDR_ALIGN        (4)

#define TSR2RCIPHER_CMD_MASK (0xF0)
#define TSR2RCIPHER_GBL_CMD  (0x00)
#define TSR2RCIPHER_CH_CMD   (0x10)

/*
 * defines the capability of the TSR2RCIPHER module
 * CNcomment: 定义tsr2rcipher模块业务功能结构体
 */
typedef struct {
    hi_u32 ts_chan_cnt; /* number of channel */ /* CNcomment: tsr2rcipher channel数目 */
} tsr2rcipher_capability;

typedef enum {
    TSR2RCIPHER_IV_EVEN = 0,
    TSR2RCIPHER_IV_ODD  = 1,
    TSR2RCIPHER_IV_MAX,
} tsr2rcipher_iv_type;

/*
 * tsr2rcipher mode
 * CNcomment: tsr2rcipher工作模式
 */
typedef enum {
    TSR2RCIPHER_MODE_PAYLOAD = 0x0, /* payload mode */ /* CNcomment: payload模式 */
    TSR2RCIPHER_MODE_RAW     = 0x1, /* raw mode */ /* CNcomment: raw模式 */
    TSR2RCIPHER_MODE_MAX
} tsr2rcipher_mode;

/*
 * tsr2rcipher algorithm
 * CNcomment: tsr2rcipher加密算法
 */
typedef enum {
    TSR2RCIPHER_ALG_AES_ECB   = 0x10,
    TSR2RCIPHER_ALG_AES_CBC   = 0x13,
    TSR2RCIPHER_ALG_AES_IPTV  = 0x16,
    TSR2RCIPHER_ALG_AES_CTR   = 0x17,
    TSR2RCIPHER_ALG_SMS4_ECB  = 0x30,
    TSR2RCIPHER_ALG_SMS4_CBC  = 0x31,
    TSR2RCIPHER_ALG_SMS4_IPTV = 0x32,
    TSR2RCIPHER_ALG_MAX
} tsr2rcipher_alg;

/*
 * structure of the tsr2rcipher control information
 * CNcomment: 加密控制信息结构
 */
typedef struct {
    tsr2rcipher_alg  alg;
    tsr2rcipher_mode mode;
    hi_bool          is_crc_check;
    hi_bool          is_create_ks;
    hi_bool          is_odd_key;
} tsr2rcipher_attr;

typedef struct {
    hi_mem_handle_t mem_handle;
    hi_mem_size_t   addr_offset;
} tsr2rcipher_mem_handle;

typedef struct {
    tsr2rcipher_attr tsc_attr; /* [in] */
    hi_handle        handle;    /* [out] */
} tsr2rcipher_create_info;

typedef struct {
    hi_handle        handle;    /* [in] */
    tsr2rcipher_attr tsc_attr; /* [out] */
} tsr2rcipher_get_attr_info;

typedef struct {
    hi_handle        handle;    /* [in] */
    tsr2rcipher_attr tsc_attr; /* [in] */
} tsr2rcipher_set_attr_info;

typedef struct {
    hi_handle tsc_handle; /* [in] */
    hi_handle ks_handle;  /* [out] */
} tsr2rcipher_get_ks_handle;

typedef struct {
    hi_handle tsc_handle; /* [in] */
    hi_handle ks_handle;  /* [in] */
} tsr2rcipher_attach_ks;

typedef struct {
    hi_handle tsc_handle; /* [in] */
    hi_handle ks_handle;  /* [in] */
} tsr2rcipher_detach_ks;

typedef struct {
    hi_handle            handle;                     /* [in] */
    tsr2rcipher_iv_type  type;                       /* [in] */
    hi_u32               len;                        /* [in] */
    hi_u8                iv[TSR2RCIPHER_MAX_IV_LEN]; /* [in] */
} tsr2rcipher_set_iv_info;

typedef struct {
    hi_handle handle;
    tsr2rcipher_mem_handle src_mem_handle;
    tsr2rcipher_mem_handle dst_mem_handle;
    hi_u32 data_len;
} tsr2rcipher_deal_data_info;

#define TSR2RCIPHER_IOCTL_GETCAP   _IOWR(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_GBL_CMD+0x0), tsr2rcipher_capability)
#define TSR2RCIPHER_IOCTL_CREATE   _IOWR(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x0), tsr2rcipher_create_info)
#define TSR2RCIPHER_IOCTL_DESTROY  _IOW(HI_ID_TSR2RCIPHER,  (TSR2RCIPHER_CH_CMD+0x1), hi_handle)
#define TSR2RCIPHER_IOCTL_GETATTR  _IOWR(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x2), tsr2rcipher_get_attr_info)
#define TSR2RCIPHER_IOCTL_SETATTR  _IOW(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x3), tsr2rcipher_set_attr_info)
#define TSR2RCIPHER_IOCTL_SETIV    _IOW(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x4), tsr2rcipher_set_iv_info)
#define TSR2RCIPHER_IOCTL_ENCRYPT  _IOW(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x5), tsr2rcipher_deal_data_info)
#define TSR2RCIPHER_IOCTL_DECRYPT  _IOW(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x6), tsr2rcipher_deal_data_info)
#define TSR2RCIPHER_IOCTL_GETKS    _IOWR(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x7), tsr2rcipher_get_ks_handle)
#define TSR2RCIPHER_IOCTL_ATTACHKS _IOW(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x8), tsr2rcipher_attach_ks)
#define TSR2RCIPHER_IOCTL_DETACHKS _IOW(HI_ID_TSR2RCIPHER, (TSR2RCIPHER_CH_CMD+0x9), tsr2rcipher_detach_ks)

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_TSR2RCIPHER_IOCTL_H__ */

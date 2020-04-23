/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:KS ioctl defination.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KS_IOCTL_H__
#define __DRV_KS_IOCTL_H__

#include "drv_ks_struct.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/* structure for ioctl */
typedef struct {
    hi_handle ks_handle;
    hi_keyslot_type ks_type; /* only for command: CMD_KS_CREATE */
} ks_entry;

#define CMD_KS_CREATE                  _IOWR(HI_ID_KEYSLOT,  0x1, ks_entry)
#define CMD_KS_DESTORY                 _IOW(HI_ID_KEYSLOT,  0x2, ks_entry)
#define CMD_KS_MAX                     0xffffffff

#ifdef __cplusplus
}
#endif
#endif /* __DRV_KS_IOCTL_H__ */

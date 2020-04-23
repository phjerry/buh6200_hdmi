/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-12-13. All rights reserved.
 * Description: XXX
 * Author: Wan
 * Create: 2019-12-13
 */

#ifndef __DRV_PDM_H__
#define __DRV_PDM_H__

#include <linux/semaphore.h>
#include "drv_pdm_ext.h"
#include "hi_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PDM_MAX_BUF_NUM     16

typedef struct {
    hi_char             buf_name[PDM_MAX_BUF_NUM];
    hi_bool             release;
    hi_u32              lenth;
    hi_u8               *phy_addr;
    hi_u8               *vir_addr;
} pdm_buf_info;

typedef struct {
    hi_disp_param       disp_param;
    pdm_buf_info        buf_info[PDM_MAX_BUF_NUM];
    hi_u32              buf_num;
    osal_semaphore     pdm_mutex;
} pdm_global;

typedef struct {
    hi_ao_port ao_port;
    hi_char *name;
    hi_char *key_name;
    hi_char *key_volume;
} ao_port_group;

extern pdm_global g_pdm_global;

hi_s32 drv_pdm_get_disp_param(hi_drv_display disp, hi_disp_param *disp_param);
hi_s32 drv_pdm_release_reserve_mem(const hi_char *buf_name);
hi_s32 drv_pdm_get_data(const hi_char *buf_name, hi_u8 **data_addr, hi_u32 *data_len);
hi_s32 drv_pdm_get_sound_param(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param);

#ifdef __cplusplus
}
#endif

#endif

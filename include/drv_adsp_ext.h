/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio adsp driver ext header file
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __DRV_ADSP_EXT_H__
#define __DRV_ADSP_EXT_H__

#include "hi_audsp_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    hi_s32 (*adsp_load_firmware)(adsp_code_id);
    hi_s32 (*adsp_unload_firmware)(adsp_code_id);
    hi_s32 (*adsp_get_aoe_fw_info)(adsp_code_id, aoe_fw_info *);
    hi_s32 (*adsp_write_cmd)(adsp_cmd_arg *);
} adsp_export_func;

hi_s32  adsp_drv_mod_init(hi_void);
hi_void adsp_drv_mod_exit(hi_void);

hi_s32  hi_drv_adsp_init(hi_void);
hi_void hi_drv_adsp_deinit(hi_void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_ADSP_EXT_H__ */

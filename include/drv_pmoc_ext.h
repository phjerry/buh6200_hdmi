/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: functions for external using
 */

#ifndef __DRV_PMOC_EXT_H__
#define __DRV_PMOC_EXT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef hi_s32 (*fn_get_chip_temperature)(hi_s16 *temperature);

typedef struct {
    fn_get_chip_temperature get_chip_temperature;
} pmoc_export_func;

hi_s32  pmoc_drv_mod_init(hi_void);
hi_void pmoc_drv_mod_exit(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* __DRV_PMOC_EXT_H__ */


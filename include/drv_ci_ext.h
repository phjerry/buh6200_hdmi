/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of drv_osal_sys
 * Author: ÕÔ¹ðºé z00268517
 * Create: 2014-08-02
 */
#ifndef __DRV_CI_H__
#define __DRV_CI_H__

#include "hi_mpi_ci.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEF_CIMAX_SMI_BITWIDTH        (8) /* CIMAX SMI bit width */
#define DEF_CIMAX_SMC_BANK0_BASE_ADDR (0x30000000)
#define DEF_CIMAX_I2CNum              (3)    /* CIMaX I2C group */
#define DEF_CIMAX_ADDR                (0x80) /* CIMaX I2C address */

#define MAX_CIS_SIZE (512)

/* CI driver configure structure */
typedef struct hi_ci_pccd_debuginfo {
    hi_ci_pccd_status status;
    hi_ci_pccd_ready ready;
    hi_bool by_pass;
    hi_u16 buffer_size;
    hi_bool is_ciplus;
    hi_ci_attr attr;
    hi_u32 io_cnt;
} ci_pccd_debuginfo, *ci_pccd_debuginfo_ptr;

hi_void drv_ci_init_mutex(hi_void);
hi_void drv_ci_de_init_mutex(hi_void);
hi_s32 drv_ci_init(hi_void);
hi_void drv_ci_de_init(hi_void);
hi_s32 drv_ci_open(hi_ci_port port);
hi_s32 drv_ci_close(hi_ci_port port);
hi_s32 drv_ci_standby(hi_ci_port port);
hi_s32 drv_ci_resume(hi_ci_port port);

hi_s32 drv_ci_pccd_open(hi_ci_port port, hi_ci_pccd card);
hi_void drv_ci_pccd_close(hi_ci_port port, hi_ci_pccd card);

hi_s32 drv_ci_pccd_ctrl_power(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ctrl_power ctrl_power);
hi_s32 drv_ci_pccd_reset(hi_ci_port port, hi_ci_pccd card);
hi_s32 drv_ci_pccd_is_ready(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ready_ptr ready);
hi_s32 drv_ci_pccd_detect(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_status_ptr status);
hi_s32 drv_ci_pccd_get_status(hi_ci_port port, hi_ci_pccd card, hi_u8 *value);
hi_s32 drv_ci_pccd_io_read(hi_ci_port port, hi_ci_pccd card, hi_u8 *buffer, hi_u32 buffer_len, hi_u32 *read_len);
hi_s32 drv_ci_pccd_io_write(hi_ci_port port, hi_ci_pccd card,
    const hi_u8 *buffer, hi_u32 write_len, hi_u32 *write_ok_len);

hi_s32 drv_ci_pccd_check_cis(hi_ci_port port, hi_ci_pccd card);
hi_s32 drv_ci_pccd_write_cor(hi_ci_port port, hi_ci_pccd card);
hi_s32 drv_ci_pccd_io_reset(hi_ci_port port, hi_ci_pccd card);
hi_s32 drv_ci_pccd_neg_buffer_size(hi_ci_port port, hi_ci_pccd card, hi_u16 *buffer_size);

hi_s32 drv_ci_pccd_ts_ctrl(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_tsctrl cmd, hi_ci_pccd_tsctrl_param *param);
hi_s32 drv_ci_pccd_get_debug_info(hi_ci_port port, hi_ci_pccd card, ci_pccd_debuginfo_ptr debug_info);
hi_s32 drv_ci_pccd_dbg_io_print_ctrl(hi_ci_port port, hi_ci_pccd card, hi_bool print);
hi_s32 drv_ci_pccd_write_cor_ex(hi_ci_port port, hi_ci_pccd card, hi_u16 addr, hi_u8 u8_data);
hi_s32 drv_ci_pccd_get_cis(hi_ci_port port, hi_ci_pccd card, hi_u8 *cis, hi_u32 cis_len, hi_u32 *ci_len);

hi_s32 drv_ci_set_attr(hi_ci_port port, hi_ci_attr ci_attr);
hi_s32 drv_ci_get_attr(hi_ci_port port, hi_ci_attr *ci_attr);

#ifndef MODULE
hi_s32 hi_ci_mod_init(hi_void);
hi_void hi_ci_mod_exit(hi_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

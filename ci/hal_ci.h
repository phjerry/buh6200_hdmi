/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of drv_osal_sys
 * Author: ÕÔ¹ðºé z00268517
 * Create: 2014-08-02
 */
#ifndef __HAL_CI_H__
#define __HAL_CI_H__

#include "drv_ci_ioctl.h"

#define hi_dbg_func_enter()         HI_FUNC_ENTER()
#define hi_dbg_func_exit()          HI_FUNC_EXIT()
#define hi_dbg_print_h32(value)     HI_DBG_PRINT_H32(value)
#define hi_dbg_func_trace()

#define HI_LOG_NOTICE(fmt...)       HI_INFO_PRINT(HI_ID_CI, fmt)
#define HI_LOG_INFO(fmt...)         HI_INFO_PRINT(HI_ID_CI, fmt)
#define HI_LOG_ERR(fmt...)          HI_ERR_PRINT(HI_ID_CI, fmt)
#define HI_LOG_FATAL(fmt...)        HI_FATAL_PRINT(HI_ID_CI, fmt)

#define hi_err_print_u32(val)       HI_ERR_PRINT_U32(val)
#define hi_err_print_h32(val)       HI_ERR_PRINT_H32(val)
#define hi_err_print_err_code(val)  HI_ERR_PRINT_U32(val)
#define hi_err_print_call_fun_err(func, err_code)   HI_ERR_PRINT_FUNC_RES(func, err_code)

#define hi_log_print_block(block, size) HI_DBG_PRINT_BLOCK(block, size)


hi_s32 hal_ci_init(hi_void);
hi_void hal_ci_deinit(hi_void);
hi_s32 hal_ci_device_open(hi_ci_port port);
hi_s32 hal_ci_device_close(hi_ci_port port);
hi_s32 hal_ci_set_attr(hi_ci_port port, hi_ci_attr attr);
hi_s32 hal_ci_pccd_open(hi_ci_port port, hi_ci_pccd card);
hi_void hal_ci_pccd_close(hi_ci_port port, hi_ci_pccd card);
hi_s32 hal_ci_pccd_io_read_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 *value);
hi_s32 hal_ci_pccd_io_write_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 value);
hi_s32 hal_ci_pccd_mem_read_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 *value);
hi_s32 hal_ci_pccd_mem_write_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 value);
hi_s32 hal_ci_pccd_detect(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_status_ptr status);
hi_s32 hal_ci_pccd_ready_or_busy(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ready_ptr ready);
hi_s32 hal_ci_pccd_reset(hi_ci_port port, hi_ci_pccd card);
hi_s32 hal_ci_pccd_ctrl_power(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ctrl_power ctrl_power);
hi_s32 hal_ci_pccd_ts_by_pass(hi_ci_port port, hi_ci_pccd card, hi_bool bypass);
hi_s32 hal_ci_pccd_get_bypass_mode(hi_ci_port port, hi_ci_pccd card, hi_bool *bypass);
hi_s32 hal_ci_standby(hi_ci_port port);
hi_s32 hal_ci_resume(hi_ci_port port);

#endif


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009-12-17
 */

#ifndef __HI_DRV_LOG_H__
#define __HI_DRV_LOG_H__

#include "hi_type.h"
#include "hi_debug.h"

typedef enum {
    LOG_OUTPUT_SERIAL,
    LOG_OUTPUT_NETWORK,
    LOG_OUTPUT_UDISK,
    LOG_OUTPUT_MAX
} log_output_pos;

hi_s32  hi_drv_log_read_buf(hi_u8 *buf, hi_u32 buf_len, hi_u32 *copy_len, hi_bool is_kernel_copy);
hi_s32  hi_drv_log_write_buf(hi_u8 *buf, hi_u32 msg_len, hi_u32 msg_from_pos);
hi_s32  hi_drv_log_set_path(hi_char *path, hi_u32 path_len);
hi_s32  hi_drv_log_get_path(hi_s8 *buf, hi_u32 buf_len);
hi_s32  hi_drv_log_set_store_path(hi_char *path, hi_u32 path_len);
hi_s32  hi_drv_log_get_store_path(hi_s8 *buf, hi_u32 buf_len);
hi_s32  hi_drv_log_set_size(hi_u32 size);
hi_s32  hi_drv_log_get_level(hi_u32 module_id, hi_log_level *log_level);

#endif /* End of __HI_DRV_LOG_H__ */


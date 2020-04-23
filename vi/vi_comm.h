/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi comm
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_COMM_H__
#define __VI_COMM_H__

#include "linux/hisilicon/securec.h"

#include "hi_type.h"
#include "hi_osal.h"
#include "hi_errno.h"
#include "hi_debug.h"

#include "hi_drv_video.h"
#include "hi_drv_pq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define VI_STATIC

#define P_I_MULTIPLE 2
#define FRAME_PRECISION 1000

#define VI_MAX_ATV_HEIGHT 660

#define VI_MAX_OUTPUT_FRAMERATE 60

#define VI_PAL_WIDTH 720
#define VI_PAL_HEIGHT 576

#define VI_NTSC_WIDTH 720
#define VI_NTSC_HEIGHT 480

#define VI_FHD_WIDTH 1920
#define VI_FHD_HEIGHT 1080

#define VI_MAX_FHD_WIDTH 1920
#define VI_MAX_FHD_HEIGHT 1200

#define VI_MIN_UHD_WIDTH 3840
#define VI_MIN_UHD_HEIGHT 2160

#define VI_MAX_UHD_WIDTH 4096
#define VI_MAX_UHD_HEIGHT 2160

#define VI_MAX_UHD_WIDTH_8K 8192
#define VI_MAX_UHD_HEIGHT_8K 4320

#define VI_I_MIN_BUF_NUM 8
#define VI_I_MAX_BUF_NUM 15
#define VI_P_MIN_BUF_NUM 6
#define VI_P_MAX_BUF_NUM 15

#define vi_drv_log_err(fmt...) HI_ERR_PRINT(HI_ID_VI, fmt)
#define vi_drv_log_info(fmt...) HI_INFO_PRINT(HI_ID_VI, fmt)
#define vi_drv_log_dbg(fmt...) HI_DBG_PRINT(HI_ID_VI, fmt)
#define vi_drv_log_fatal(fmt...) HI_FATAL_PRINT(HI_ID_VI, fmt)
#define vi_drv_log_warn(fmt...) HI_WARN_PRINT(HI_ID_VI, fmt)
#define vi_drv_log_notice(fmt...) HI_ERR_PRINT(HI_ID_VI, fmt)

#define vi_drv_func_enter() HI_ERR_PRINT(HI_ID_VI, " ===>[Enter]\n")
#define vi_drv_func_exit() HI_ERR_PRINT(HI_ID_VI, " <===[Exit]\n")

#define vi_drv_is_odd_num(a) ((a)&0x1)
#define vi_drv_get_vi_id(handle_handle) ((handle_handle)&0xff)
#define vi_drv_get_mod_id(handle_handle) (((handle_handle) >> 16) & 0xff)
#define vi_drv_generate_handle(vi_id) ((HI_ID_VI << 24) | (HI_ID_VI << 16) | (HI_ID_VI << 8) | (vi_id))

#define vi_ceiling(x, a) (((x) + (a)-1) / (a))
#define vi_get_stride(w) \
    ((((w) % 256) == 0) ? (w) : (((((w) / 256) % 2) == 0) ? ((((w) / 256) + 1) * 256) : ((((w) / 256) + 2) * 256)))

#define vi_align_stride_y_8bit(w) vi_get_stride((w))
#define vi_align_stride_y_10bit(w) vi_get_stride(vi_ceiling((w)*10, 8)) /* l0bit Compact Model */
#define vi_align_stride_y_12bit(w) vi_get_stride(vi_ceiling((w)*12, 8)) /* l2bit Compact Model */

typedef enum {
    VI_BUFFER_ALLOC_TYPE_AUTO = 0,
    VI_BUFFER_ALLOC_TYPE_MMZ,
    VI_BUFFER_ALLOC_TYPE_MMU,

    VI_BUFFER_ALLOC_TYPE_MAX,
} vi_buffer_alloc_type;

typedef struct {
    hi_void *buf_obj;
    hi_mem_handle_t buf_handle;
    hi_u8 *start_vir_addr_p;
    hi_u64 start_phy_addr;
    hi_u32 size;
    vi_buffer_alloc_type alloc_type;
} vi_buffer_addr_info;

hi_s32 vi_comm_pq_get_csc_coef(hi_drv_pq_csc_info csc_info, hi_drv_pq_csc_coef *csc_coef_p);

hi_s32 vi_comm_get_y_stride(hi_u32 width, hi_drv_pixel_bitwidth bit_width, hi_u32 *stride_p);
hi_s32 vi_comm_get_c_stride(hi_u32 y_stride, hi_drv_pixel_format pix_fmt, hi_u32 *cstride_p);

hi_s32 vi_comm_alloc(const char *buf_name, vi_buffer_addr_info *buf_p);
hi_void vi_comm_release(vi_buffer_addr_info *buf_p);
hi_s32 vi_comm_mmap(vi_buffer_addr_info *buf_p);
hi_void vi_comm_unmap(vi_buffer_addr_info *buf_p);
hi_s32 vi_comm_query_phy_addr(hi_mem_handle_t phy_addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

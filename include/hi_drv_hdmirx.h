/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description : hdmirx drv-part
* Author : Hisilicon multimedia interface software group
* Create : 2019-12-6
*/
#ifndef __HI_DRV_HDMIRX_H__
#define __HI_DRV_HDMIRX_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_video.h"
#include "drv_hdmirx_struct.h"
#include "hi_drv_color.h"

#define DOLBY_INFO_LENGTH      31
#define HDR10_PLUS_INFO_LENGTH 31

#define hi_fatal_hdmirx(fmt...)       HI_FATAL_PRINT(HI_ID_HDMIRX, fmt)
#define hi_err_hdmirx(fmt...)         HI_ERR_PRINT(HI_ID_HDMIRX, fmt)
#define hi_warn_hdmirx(fmt...)        HI_WARN_PRINT(HI_ID_HDMIRX, fmt)
#define hi_info_hdmirx(fmt...)        HI_INFO_PRINT(HI_ID_HDMIRX, fmt)
#define hi_dbg_hdmirx(fmt...)         HI_DBG_PRINT(HI_ID_HDMIRX, fmt)
#define hi_trace_hdmirx(fmt...)       HI_TRACE_PRINT(HI_ID_HDMIRX, fmt)

#define hi_err_hdmirx_print_u32(val)  HI_ERR_PRINT_U32(val)
#define hi_warn_hdmirx_print_u32(val) HI_WARN_PRINT_U32(val)
#define hi_warn_hdmirx_print_str(val) HI_WARN_PRINT_STR(val)
#define hi_dbg_hdmirx_print_u32(val)  HI_DBG_PRINT_U32(val)
#define hi_info_hdmirx_print_u32(val) HI_INFO_PRINT_U32(val)
#define hi_info_hdmirx_func_enter()   HI_INFO_PRINT(HI_ID_HDMIRX, " ===>[Enter]\n")
#define hi_info_hdmirx_func_exit()    HI_INFO_PRINT(HI_ID_HDMIRX, " <===[Exit]\n")
#define hi_dbg_hdmirx_func_enter()    HI_DBG_PRINT(HI_ID_HDMIRX, " ===>[Enter]\n")
#define hi_dbg_hdmirx_func_exit()     HI_DBG_PRINT(HI_ID_HDMIRX, " <===[Exit]\n")
#define hi_warn_hdmirx_func_enter()   HI_WARN_PRINT(HI_ID_HDMIRX, " ===>[Enter]\n")
#define hi_warn_hdmirx_func_exit()    HI_WARN_PRINT(HI_ID_HDMIRX, " <===[Exit]\n")

typedef enum {
    HI_DRV_HDMIRX_VISTATUS_CREATE,
    HI_DRV_HDMIRX_VISTATUS_DESTORY,
    HI_DRV_HDMIRX_VISTATUS_MAX
} hi_drv_hdmirx_vistatus;

typedef struct {
    hi_u8 dolby_info[DOLBY_INFO_LENGTH];
} hi_hdmirx_dolby_vision_info;

typedef struct {
    hi_u8 hdr10_plus_info[HDR10_PLUS_INFO_LENGTH];
} hi_hdmirx_hdr10_plus_info;

typedef enum {
    HI_HDMIRX_SOURCE_INPUT_TYPE_SDR,
    HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10,
    HI_HDMIRX_SOURCE_INPUT_TYPE_HLG,
    HI_HDMIRX_SOURCE_INPUT_TYPE_DOLBY,
    HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN,
    HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10PLUS,
    HI_HDMIRX_SOURCE_INPUT_TYPE_MAX
} hi_hdmirx_source_input_type;

typedef struct {
    hi_hdmirx_source_input_type   src_type;
    hi_drv_pixel_format           pix_fmt;
    hi_drv_pixel_bitwidth         bit_width;
    hi_drv_color_space            color_space;
    hi_drv_hdr_hdr10plus_metadata metadata;
    hi_drv_hdr_hlg_metadata       hlg_info;
    hi_drv_color_descript         color_desp_info;
    hi_hdmirx_dolby_vision_info   dolby_info;
    hi_hdmirx_hdr10_plus_info     hdr10_plus_info;
} hi_hdmirx_dynamic_source_data;

/* interface with VICAP when HDR10 */
typedef struct {
    hi_u32 (*hdmirx_get_data_route)(hi_void);
    hi_s32 (*hdmirx_drv_intf_set_status)(hi_u32 vicap_id, hi_drv_hdmirx_vistatus status);
    hi_s32 (*hdmirx_drv_intf_get_hdr_attr)(hi_hdmirx_dynamic_source_data *hdr_attr);
} hdmirx_export_func;

#endif

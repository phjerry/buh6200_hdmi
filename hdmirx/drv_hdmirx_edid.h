/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of edid functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_EDID_H__
#define __DRV_HDMIRX_EDID_H__

#include "drv_hdmirx_common.h"

#define b_pcm_support    0x1
#define b_ac3_support    0x2
#define b_mpeg1_support  0x4
#define b_mp3_support    0x8
#define b_mpeg2_support  0x10
#define b_aac_support    0x20
#define b_dts_support    0x40
#define b_atrac_support  0x80
#define b_onebit_support 0x100
#define b_e_ac3_support  0x200
#define b_dts_hd_support 0x400
#define b_mat_support    0x800
#define b_dst_support    0x1000
#define b_wma_support    0x2000

#define PRODUCT_INFO_LEN    10
#define MONITORNAME_LEN     13
#define DESCRIPTOR_PER_LEN  18
#define DESCRIPTOR_LEN      72
#define MONITORHEAD_LEN     5
#define MANUFAC_NAME        3
#define PRODUCT_INFO_OFFSET 8
#define DESCRIPTOR_OFFSET   54
#define MANUFAC_NAME        3
#define SPD_VENDOR_NAME_LEN 8
#define SPD_PRODUCT_DES_LEN 16
#define EDID_LENGTH         256
#define EDID_HEAD_LENGTH    8

typedef enum {
    hdmirx_edid_cea_tag_res1 = 0,
    HDMIRX_EDID_CEA_TAG_AUDIO,
    HDMIRX_EDID_CEA_TAG_VIDEO,
    HDMIRX_EDID_CEA_TAG_VSDB,
    HDMIRX_EDID_CEA_TAG_SPEAK,
    HDMIRX_EDID_CEA_TAG_VESA,
    hdmirx_edid_cea_tag_res2,
    HDMIRX_EDID_CEA_TAG_EXT,
    HDMIRX_EDID_CEA_TAG_TOTAL
} hdmirx_edid_cea_tag;

typedef struct {
    hi_bool active;
    hi_u8 manufac_name[MANUFAC_NAME + 1];
    hi_u8 monitor_name[MONITORNAME_LEN + 1];
    hi_u32 audio_data;
} hdmirx_edid_context;

hi_s32 hdmirx_drv_edid_register(hdmirx_edid_type edid_type, hi_u8 *edid_data, hi_u32 edid_length);
hi_s32 hdmirx_drv_edid_log(hdmirx_edid_type edid_type);
hi_s32 hdmirx_drv_edid_get_edid_type(hi_u8 *edid_data, hdmirx_edid_type *edid_type);

#endif



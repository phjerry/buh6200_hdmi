/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include "vfmw_osal.h"
#include "dbg.h"

typedef enum {
    ATTR_GET_BEGIN = 0,
    ATTR_GET_CAP,
    ATTR_GET_OUT_FMT,
    ATTR_GET_SEG_BUF_SIZE,
    ATTR_GET_LICENSE,
    ATTR_GET_END,

    ATTR_SET_BEGIN = 100,
    ATTR_SET_CLK,
    ATTR_RESET_VDH,
    ATTR_SET_END,
} attr_id;

typedef enum {
    ATTACH_NONE,
    ATTACH_DEC,
    ATTACH_DISP,
    ATTACH_BUTT,
} mem_attach;

typedef struct {
    hi_u32 mem_format; /* 0:linear */
    hi_u32 bit_depth;
    hi_u32 image_width;
    hi_u32 image_height;
    hi_u32 image_stride;
    hi_u32 image_stride_2bit;

    UADDR luma_phy_addr;
    UADDR chrom_phy_addr;
    UADDR luma_2bit_phy_addr;
    UADDR chrom_2bit_phy_addr;

    hi_u8 *luma_vir_addr;
    hi_u8 *chrom_vir_addr;
    hi_u8 *luma_2bit_vir_addr;
    hi_u8 *chrom_2bit_vir_addr;
} pdt_yuv;

typedef struct {
    hi_u32 is_disp_need;
    hi_u8 compress_en;
    hi_u8 bit_depth;
    hi_u32 disp_num;
    hi_u32 disp_size;
    hi_u32 disp_width;
    hi_u32 disp_height;
    hi_u32 disp_stride;
    hi_u32 head_stride;
    hi_u32 head_size;
} mem_disp_info;

typedef struct {
    hi_u8 bit_depth;
    hi_u32 dec_num;
    hi_u32 dec_size;
    hi_u32 dec_width;
    hi_u32 dec_height;
    hi_u32 dec_stride;
    hi_u32 compress_en;
    hi_u32 head_stride;
    hi_u32 head_size;
} mem_dec_info;

typedef struct {
    hi_u32 pmv_num;
    hi_u32 pmv_size;
    hi_u32 line_num_size;

    mem_attach attach;
    hi_u32 nor_size;
    hi_u32 sec_size;

    mem_dec_info dec_info;
    mem_disp_info disp_info;
    hi_u32 min_parallel_num;
} mem_need_info;

typedef struct {
    hi_s32 chan_id;
    hi_u8 is_omx;
    hi_u8 is_hdr;
    hi_u8 alloc_type;
    vfmw_cmp cmp_mode;
    vfmw_bit_depth_attr disp_bit_depth_mode;
    hi_u32 extra_fs_num;
    hi_bool add_extra;
    vfmw_vid_std vid_std;
    hi_u8 bit_depth;
    hi_u8 linear_en;
    hi_u8 need_mtdt;

    hi_u32 dec_width;
    hi_u32 dec_height;
    hi_u32 disp_width;
    hi_u32 disp_height;
    hi_u32 need_dec_num;
    hi_u32 need_pmv_num;
    hi_u32 need_disp_num;

    hi_u32 slot_width;
    hi_u32 slot_height;
    hi_u32 disp_slot_width;
    hi_u32 disp_slot_height;
} mem_req_info;

typedef struct {
    hi_u32 valid;
    hi_u32 av1_en;
    hi_u32 avs3_en;
    hi_u32 core_num;
} pdt_license;

hi_s32 pdt_init(hi_void);
hi_s32 pdt_exit(hi_void);
hi_s32 pdt_suspend(hi_void *);
hi_s32 pdt_resume(hi_void *);
hi_s32 pdt_get_attr(attr_id, hi_void *);
hi_s32 pdt_set_attr(attr_id, hi_void *);
hi_s32 pdt_get_mem_req(mem_req_info *, mem_need_info *);
hi_s32 pdt_get_dps_num(vfmw_vid_std, hi_s32 *);

hi_u32 pdt_get_y8b_data(pdt_yuv *, hi_s32, hi_s32);
hi_u32 pdt_get_c8b_data(pdt_yuv *, hi_s32, hi_s32);
hi_u32 pdt_get_ynb_data(pdt_yuv *, hi_s32, hi_s32);
hi_u32 pdt_get_cnb_data(pdt_yuv *, hi_s32, hi_s32);
hi_u32 pdt_gety_data(pdt_yuv *, hi_s32, hi_s32);
hi_u32 pdt_getu_data(pdt_yuv *, hi_s32, hi_s32);
hi_u32 pdt_getv_data(pdt_yuv *, hi_s32, hi_s32);
hi_s32 pdt_yuv_2d(vfmw_image *, hi_u8 *);

#endif

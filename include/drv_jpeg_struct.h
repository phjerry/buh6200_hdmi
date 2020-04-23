/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hi jpeg type define
 */

#ifndef __DRV_JPEG_STRUCT_H__
#define __DRV_JPEG_STRUCT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define JPEG_MAX_COMPONENT 3
#define JPEG_MAX_COMPONENTS 4
#define JPEG_DC_TABLE_SIZE 12
#define JPEG_DCT_SIZE 64
#define JPEG_AC_SYMBOL_TABLE_SIZE 256

#define JPEG_MCU_8ALIGN 8
#define JPEG_MCU_16ALIGN 16
#define JPEG_REG_CFG_FOUR_BYTES 4
#define JPEG_REG_CFG_FOUR_BITS 4
#define JPEG_REG_CFG_EIGHT_BITS 8
#define JPEG_REG_CFG_SIXTEEN_BITS 16
#define JPEG_FACTOR_COLUMN_NUM 2

#define JPEG_HDEC_MAX_COMPONENT_NUM 3
#define JPEG_INPUT_DATA_BUFFER_NUM 2
#define JPEG_INPUT_DATA_BUFFER_RESERVE_SIZE 128
#define JPEG_INPUT_DATA_BUFFER_SIZE (512 * 1024)

typedef enum {
    JPEG_FMT_ARGB_8888 = 0, /* 0 */
    JPEG_FMT_BGRA_8888,     /* 1 */
    JPEG_FMT_ABGR_8888,     /* 2 */
    JPEG_FMT_RGBA_8888,     /* 3 */
    JPEG_FMT_ARGB_1555,     /* 4 */
    JPEG_FMT_BGRA_5551,     /* 5 */
    JPEG_FMT_ABGR_1555,     /* 6 */
    JPEG_FMT_RGBA_5551,     /* 7 */
    JPEG_FMT_RGB_888,       /* 8 */
    JPEG_FMT_BGR_888,       /* 9 */
    JPEG_FMT_RGB_565,       /* 10 */
    JPEG_FMT_BGR_565,       /* 11 */
    JPEG_FMT_YCCK,          /* 12 */
    JPEG_FMT_CMYK,          /* 13 */
    JPEG_FMT_YUV_PACK,      /* 14 */
    JPEG_FMT_YVU_PACK,      /* 15 */
    JPEG_FMT_YUV400,        /* 16 */
    JPEG_FMT_YUV420,        /* 17 */
    JPEG_FMT_YUV422_21,     /* 18 */
    JPEG_FMT_YUV422_12,     /* 19 */
    JPEG_FMT_YUV444,        /* 20 */
    JPEG_FMT_YVU400,        /* 21 */
    JPEG_FMT_YVU420,        /* 22 */
    JPEG_FMT_YVU422_21,     /* 23 */
    JPEG_FMT_YVU422_12,     /* 24 */
    JPEG_FMT_YVU444,        /* 25 */
    JPEG_FMT_MAX
} jpeg_hdec_image_fmt;

typedef enum {
    JPEG_INT_TYPE_NONE = 0,
    JPEG_INT_TYPE_CONTINUE,
    JPEG_INT_TYPE_LOWDEALY,
    JPEG_INT_TYPE_FINISH,
    JPEG_INT_TYPE_ERROR,
    JPEG_INT_TYPE_STREAM_ERROR,
    JPEG_INT_TYPE_CSC_ERROR,
    JPEG_INT_TYPE_MAX
} hi_jpeg_dec_state;

typedef struct {
    hi_u32 x, y;
    hi_u32 w, h;
} jpeg_hdec_crop_info;

typedef struct {
    hi_u32 fd;
    hi_u32 buf_size;
} jpeg_hdec_assign_mem;

typedef struct {
    jpeg_hdec_assign_mem stream_mem;
    jpeg_hdec_assign_mem yuv_mem;
    jpeg_hdec_assign_mem xrgb_mem;
} jpeg_hdec_assign_mem_info;

typedef struct {
    hi_u32 buf_size;
} jpeg_hdec_stream_mem_info;

typedef struct {
    hi_u32 y_size;
    hi_u32 uv_size;
} jpeg_hdec_yuv_mem_info;

typedef struct {
    hi_u32 output_stride;
    hi_u32 output_height;
} jpeg_hdec_xrgb_mem_info;

typedef struct {
    jpeg_hdec_stream_mem_info stream_mem;
    jpeg_hdec_yuv_mem_info yuv_mem;
    jpeg_hdec_xrgb_mem_info xrgb_mem;
} jpeg_hdec_input_mem_info;

typedef struct {
    hi_bool is_low_delay;
    hi_bool is_out_usr_buf;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle_t low_delay_handle;
    hi_mem_size_t low_delay_offset;
    hi_mem_size_t low_delay_size;
    hi_mem_handle_t src_buf_handle;
    hi_mem_size_t src_buf_offset;
    hi_mem_size_t src_buf_size;
    hi_mem_handle_t dst_buf_handle;
    hi_mem_size_t dst_buf_y_offset;
    hi_mem_size_t dst_buf_uv_offset;
    hi_mem_size_t dst_buf_size;
#else
    hi_u64 low_delay_offset;
    hi_u64 low_delay_size;
    hi_u64 src_buf_size;
    hi_u64 dst_buf_y_offset;
    hi_u64 dst_buf_uv_offset;
    hi_u64 dst_buf_size;
#endif
    hi_u32 resume_value;
    hi_u64 low_delay_phy_buf;
    hi_u64 src_phy_buf;
    hi_u64 dst_phy_buf;
    hi_u64 start_save_stream_phy_buf;
    hi_u64 end_save_stream_phy_buf;
    hi_u64 y_dst_phy_addr;
    hi_u64 uv_dst_phy_addr;
    hi_u32 cur_pos_in_buffer;
    hi_u32 dst_buf_stride[JPEG_MAX_COMPONENT];
    hi_u32 scale;
    hi_u32 image_mcu_width;
    hi_u32 image_mcu_height;
    hi_u32 output_height;
    hi_u32 output_stride;
    hi_u32 restart_interval;
    hi_s32 reg_quant_table[JPEG_DCT_SIZE];
    hi_u32 reg_dc_table[JPEG_DC_TABLE_SIZE];
    hi_u32 reg_ac_min_table[JPEG_DCT_SIZE];
    hi_u32 reg_ac_base_table[JPEG_DCT_SIZE];
    hi_u32 reg_ac_symbol_table[JPEG_AC_SYMBOL_TABLE_SIZE];
    hi_u8 fac[JPEG_MAX_COMPONENT][JPEG_FACTOR_COLUMN_NUM];
    hi_u32 jpeg_color_space;
    hi_u32 output_color_space;
    hi_jpeg_dec_state interrupt_state;
} hi_jpeg_reg_info;

typedef struct {
    hi_bool is_eof;
    hi_u32 buf_size;
    hi_u64 phy_buf;
    hi_u64 vir_buf;
} jpeg_hdec_save_buf;

typedef struct {
    hi_u32 read_buf_index;
    hi_u32 hdec_buf_index;
    jpeg_hdec_save_buf save_buf[JPEG_INPUT_DATA_BUFFER_NUM];
} jpeg_hdec_buf_info;

typedef struct {
    hi_u64 phy_buf[JPEG_HDEC_MAX_COMPONENT_NUM];
    hi_u64 vir_buf[JPEG_HDEC_MAX_COMPONENT_NUM];
    jpeg_hdec_crop_info crop_rect;
} jpeg_hdec_surface_info;

typedef struct {
    jpeg_hdec_input_mem_info input_mem_info;
    jpeg_hdec_assign_mem_info output_mem_info;
    jpeg_hdec_surface_info yuv_surface;
    jpeg_hdec_surface_info xrgb_surface;
    jpeg_hdec_buf_info buf_info;
} jpeg_hdec_mem_info;

typedef struct {
    hi_bool is_use_standard_lib_dec;
    hi_bool is_low_delay_support;
    hi_bool is_lack_of_valid_data;
    hi_bool is_dri_support;
    hi_bool is_huf_num_support;
    hi_bool is_scale_support;
    hi_bool is_sample_factor_support;
    hi_bool is_input_img_size_support;
    hi_bool is_out_color_space_support;
    hi_bool is_open_dev_success;
    hi_bool is_alloc_mem_success;
    hi_bool is_get_dev_success;
    hi_bool is_open_csc_dev_success;
    hi_bool is_open_ion_dev_success;
    hi_bool is_support_seek_to_soft;
    hi_u32 hard_dec_times;
    hi_u32 soft_dect_times;
    hi_u32 dec_cost_times;
} jpeg_proc_info;

typedef enum {
    JPEG_EXIT_MODULE_REGISTER = 0x1,
    JPEG_EXIT_PM_REGISTER = 0x2,
    JPEG_EXIT_REG_MAP = 0x4,
    JPEG_EXIT_HAL_INIT = 0x8,
    JPEG_EXIT_IRQ_REGISTER = 0x10,
    JPEG_EXIT_DECODE_INIT = 0x20,
    JPEG_EXIT_SMMU_MAP = 0x40,
    JPEG_EXIT_SMMU_INIT = 0x80,
    JPEG_EXIT_ALL = 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40 | 0x80,
    JPEG_EXIT_IRQ_MAX
} jpg_exit_state;

typedef struct {
    hi_ulong handle;
    hi_u32 width;
    hi_u32 height;
    hi_u64 vir_buf;
    hi_u32 phy_buf;
    hi_u32 stride;
    hi_u32 buf_size;
} hi_jpeg_exif_surface;

typedef enum {
    JPEG_STREAM_MEM_MMU_TYPE = 0X1,
    JPEG_YOUTPUT_MEM_MMU_TYPE = 0X2,
    JPEG_UVOUTPUT_MEM_MMU_TYPE = 0X4,
    JPEG_XRGBSAMPLE0_READ_MEM_MMU_TYPE = 0X8,
    JPEG_XRGBSAMPLE1_READ_MEM_MMU_TYPE = 0X10,
    JPEG_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE = 0X20,
    JPEG_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE = 0X40,
    JPEG_XRGBOUTPUT_MEM_MMU_TYPE = 0X80,
    JPEG_LINENUM_MEM_MMU_TYPE = 0X100,
    JPEG_MEMTYPE_BUTT
} drv_mem_type;

typedef struct {
    hi_u64 phy_addr;
    jpeg_hdec_image_fmt jpeg_color_space;
    hi_u32 y_height;
    hi_u32 y_width;
    hi_u32 y_stride;
    hi_u64 clut_phy_addr;
    hi_u64 cbcr_phy_addr;
    hi_bool is_ycbcr_clut;
    hi_bool alpha_max_is_255;
    hi_bool support_alpha_ext_1555;
    hi_u8 alpha0;
    hi_u8 alpha1;
    hi_u32 cbcr_stride;
} jpeg_hdec_dec_in;

typedef struct {
    hi_u64 phy_addr;
    hi_u32 out_stride;
    hi_u32 cbcr_stride;
    hi_u32 width;
    hi_u32 height;
    hi_u64 clut_phy_addr;
    hi_u64 cbcr_phy_addr;
    hi_bool is_ycbcr_clut;
    hi_bool alpha_max_is_255;
    hi_bool support_alpha_ext_1555;
    hi_u8 alpha0;
    hi_u8 alpha1;
    jpeg_hdec_crop_info crop_rect;
    jpeg_hdec_image_fmt output_color_space;
} jpeg_hdec_dec_out;

typedef struct {
    hi_bool is_csc_finished;
    jpeg_hdec_dec_in dec_in;
    jpeg_hdec_dec_out dec_out;
} jpeg_hdec_csc_surface;

typedef struct {
    hi_jpeg_reg_info reg_info;
    jpeg_hdec_csc_surface csc_surface;
} jpeg_hdec_reg_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

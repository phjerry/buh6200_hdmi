/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "crc_ext.h"
#include "vfmw_pdt.h"

#define NEW_CRC

typedef struct {
    /* yuv virAddr */
    hi_u8 *y_addr_8bit;
    hi_u8 *y_addr_nbit;
    hi_u8 *c_addr_8bit;
    hi_u8 *c_addr_nbit;
} yuv_addr;

typedef struct {
    /* tile virAddr */
    hi_u8 *y_address_8bit;
    hi_u8 *y_address_nbit;
    hi_u8 *c_address_8bit;
    hi_u8 *c_address_nbit;

    /* bit depth */
    hi_s32 real_bit_depth;
    hi_s32 save_bit_depth;

    hi_u32 is_1d;

    hi_u32 stride;
    hi_u32 stride_nbit;
} yuv_param;

/* --------------------------- INTERNAL VALUE -------------------------- */
#define MAX_CRC_YUV_SIZE (4096 * 2304 * 3 / 2)
#define CALC_BUF_SIZE    (MAX_CRC_YUV_SIZE * 2)

static mem_record g_calc_memory;
static hi_s8 g_is_crc_calc_init = CRC_FLASE;

extern hi_s32 g_MulticrcNum[VFMW_CHAN_NUM];
extern hi_u8 g_MulticrcStrm[VFMW_CHAN_NUM][256]; /* 256 :a number */
extern hi_s32 g_crc_inst_num;

typedef struct {
    hi_u8 *crc_y_addr_8bit;
    hi_u8 *crc_y_addr_nbit;
    hi_u8 *crc_c_addr_8bit;
    hi_u8 *crc_c_addr_nbit;
    hi_u32 crc_width;
    hi_u32 crc_height;
    hi_u32 crc_stride;

    hi_u8 *y_addr;
    hi_u8 *c_addr;
    hi_u8 *y_addr_nbit;
    hi_u8 *c_addr_nbit;
    hi_u32 top_offset;
    hi_u32 left_offset;
    hi_u32 stride;
    hi_u8  is_1d;
    hi_u8  is_uv12;
} crc_calc_param;

extern hi_u32 g_row_map_table_y[2][4][16]; /* 2 :a number 4 :a number 16 :a number */
extern hi_u32 g_row_map_table_uv[2][4][8]; /* 2 :a number 4 :a number 8 :a number */

hi_u32 g_row_map_table_y_2b[4][16] = { /* 4 :a number 16 :a number */
    { 0, 1, 2,  3,  4,  5,  6,  7,  8, 9, 10, 11, 12, 13, 14, 15 },
    { 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2,  3,  4,  5,  6,  7 },
    { 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2,  3,  4,  5,  6,  7 },
    { 0, 1, 2,  3,  4,  5,  6,  7,  8, 9, 10, 11, 12, 13, 14, 15 },
};

hi_u32 g_row_map_table_uv_2b[8] = { /* 8 :a number */
    0, 1, 2, 3, 4, 5, 6, 7
};

/* ------------------------- INTERNAL FUNCTION ------------------------- */
static hi_void crc_get_vir_addr(vfmw_image *image, UADDR luma_phy, UADDR chroma_phy,
                                hi_u8 **luma_vir, hi_u8 **chroma_vir,
                                hi_u8 **luma_vir_2bit, hi_u8 **chroma_vir_2bit,
                                hi_u32 is_view_1)
{
    mem_record mem_record = { 0 };

    mem_record.mode = MEM_MMU_MMU;
    mem_record.phy_addr = luma_phy;
    /* 3 :a number 2 :a number 8 :a number */
    mem_record.length = (chroma_phy - luma_phy) * 3 / 2 * (image->bit_depth) / 8;
    mem_record.dma_buf = is_view_1 == 0 ? image->disp_info.frm_dma_buf : image->disp_info.frm_dma_buf_1;
    *luma_vir = OS_MMAP(&mem_record);

    *chroma_vir = *luma_vir + (chroma_phy - luma_phy);

    if (image->bit_depth > 8) { /* 8 :a number */
        *luma_vir_2bit = *luma_vir + (chroma_phy - luma_phy) * 3 / 2; /* 3 :a number 2 :a number */
        /* 8 :a number 8 :a number */
        *chroma_vir_2bit = *luma_vir_2bit + (chroma_phy - luma_phy) * (image->bit_depth - 8) / 8;
    }

    return;
}

static hi_void crc_release_vir_addr(vfmw_image *image, UADDR luma_phy, UADDR chroma_phy,
                                    hi_u8 *luma_vir, hi_u8 *chroma_vir, hi_u32 is_view_1)
{
    mem_record mem = { 0 };

    mem.vir_addr = luma_vir;
    mem.dma_buf = is_view_1 == 0 ? image->disp_info.frm_dma_buf : image->disp_info.frm_dma_buf_1;
    OS_UNMAP(&mem);

    return;
}

static hi_u32 crc_calc_field_crc_8bit(crc_calc_param *param,
                                      hi_u32 width, hi_u32 height, hi_u32 stride, hi_u8 btm_field)
{
    hi_u32 i, j;
    hi_u32 frame_crc = 0;
    hi_u32 chrom_width = width / 2; /* 2 :a number */
    hi_u32 chrom_height = height / 2; /* 2 :a number */
    hi_u32 y_width_distance = width / 8; /* 8 :a number */
    hi_u32 y_height_distance = height / 4; /* 4 :a number */
    hi_u32 chrom_width_distance;
    hi_u32 chrom_height_distance;
    hi_u32 w, h;
    hi_u32 top_offset;
    hi_u32 data1, data2, data4;
    pdt_yuv yuv_info = { 0 };

    while ((y_width_distance % 8) != 0) { /* 8 :a number */
        y_width_distance--;
    }

    chrom_width_distance = y_width_distance / 2; /* 2 :a number */
    chrom_height_distance = y_height_distance / 2; /* 2 :a number */

    y_width_distance = (y_width_distance < 4) ? 4 : y_width_distance; /* 4 :a number 4 :a number */
    chrom_width_distance = (chrom_width_distance < 4) ? 4 : chrom_width_distance; /* 4 :a number 4 :a number */

    stride = param->stride;

    if (param->is_1d == 1) {
        yuv_info.mem_format = 1;
    } else {
        yuv_info.mem_format = 0;
    }

    yuv_info.image_stride = stride;
    yuv_info.luma_vir_addr = param->y_addr;
    yuv_info.chrom_vir_addr = param->c_addr;

    top_offset = param->top_offset + btm_field;

    for (i = 0; i < height; i += y_height_distance) {
        for (j = 0; j < width - 4; j += y_width_distance) { /* 4 :a number */
            h = 2 * i + top_offset; /* 2 :a number */
            w = j + param->left_offset;

            data1 = pdt_get_y8b_data(&yuv_info, h, w);
            data4 = data1;

            data1 = pdt_get_y8b_data(&yuv_info, h, w + 1);
            data4 += data1 << 8; /* 8 :a number */

            data1 = pdt_get_y8b_data(&yuv_info, h, w + 2); /* 2 :a number */
            data4 += data1 << 16; /* 16 :a number */

            data1 = pdt_get_y8b_data(&yuv_info, h, w + 3); /* 3 :a number */
            data4 += data1 << 24; /* 24 :a number */

            frame_crc += data4;
        }
    }

    if (yuv_info.chrom_vir_addr != HI_NULL) {
        top_offset = param->top_offset / 2 + btm_field; /* 2 :a number */

        for (i = 0; i < chrom_height; i += chrom_height_distance) {
            for (j = 0; j < 2 * chrom_width - 4; j += chrom_width_distance) { /* 2 :a number 4 :a number */
                h = 2 * i + top_offset; /* 2 :a number */
                w = j + param->left_offset;

                data1 = pdt_get_c8b_data(&yuv_info, h, w / 2); /* 2 :a number */
                data2 = pdt_get_c8b_data(&yuv_info, h, w / 2 + 1); /* 2 :a number */

                if (param->is_uv12 == 1) {
                    data4 = ((data1 & 0xff00) >> 8) + ((data1 & 0xff) << 8) + /* 8 :a number 8 :a number */
                            ((data2 & 0xff00) << 8) + ((data2 & 0xff) << 24); /* 8 :a number 24 :a number */
                } else {
                    data4 = data1 + (data2 << 16); /* 16 :a number */
                }

                frame_crc += data4;
            }
        }
    }

    return frame_crc;
}

static hi_u32 crc_calc_field_crc_nbit(crc_calc_param *param,
                                      hi_u32 width, hi_u32 height, hi_u32 stride, hi_u8 btm_field)
{
    hi_u32 i, j, k;
    hi_u32 frame_crc = 0;
    hi_u32 chrom_width = width / 2; /* 2 :a number */
    hi_u32 chrom_height = height / 2; /* 2 :a number */
    hi_u32 y_width_distance = width / 8; /* 8 :a number */
    hi_u32 y_height_distance = height / 4; /* 4 :a number */
    hi_u32 chrom_width_distance;
    hi_u32 chrom_height_distance;
    hi_u32 w, h;
    hi_u32 top_offset;
    hi_u32 data1, data4;
    pdt_yuv yuv_info;

    while ((y_width_distance % 8) != 0) { /* 8 :a number */
        y_width_distance--;
    }

    chrom_width_distance = y_width_distance / 2; /* 2 :a number */
    chrom_height_distance = y_height_distance / 2; /* 2 :a number */

    y_width_distance = (y_width_distance < 4) ? 4 : y_width_distance; /* 4 :a number 4 :a number */
    chrom_width_distance = (chrom_width_distance < 4) ? 4 : chrom_width_distance; /* 4 :a number 4 :a number */

    stride = param->stride;

    if (param->is_1d == 1) {
        yuv_info.mem_format = 1;
    } else {
        yuv_info.mem_format = 0;
    }

    yuv_info.image_stride_2bit = stride / 4; /* 4 :a number */
    yuv_info.luma_2bit_vir_addr = param->y_addr_nbit;
    yuv_info.chrom_2bit_vir_addr = param->c_addr_nbit;

    top_offset = param->top_offset + btm_field;

    for (i = 0; i < height; i += y_height_distance) {
        for (j = 0; j < width - 4; j += y_width_distance) { /* 4 :a number */
            h = 2 * i + top_offset; /* 2 :a number */
            w = j + param->top_offset;

            data4 = 0;
            for (k = 0; k < 16; k++) { /* 16 :a number */
                data4 += pdt_get_ynb_data(&yuv_info, h, w * 4 + k) << (k << 1); /* 4 :a number */
            }

            frame_crc += data4;
        }
    }

    if (yuv_info.chrom_2bit_vir_addr != HI_NULL) {
        top_offset = param->top_offset / 2 + btm_field; /* 2 :a number */

        for (i = 0; i < chrom_height; i += chrom_height_distance) {
            for (j = 0; j < 2 * chrom_width - 4; j += chrom_width_distance) { /* 2 :a number 4 :a number */
                h = 2 * i + top_offset; /* 2 :a number */
                w = j + param->top_offset;

                if (param->is_uv12 == 1) {
                    data4 = 0;
                    for (k = 0; k < 16; k += 2) { /* 16 :a number 2 :a number */
                        /* 4 :a number 2 :a number */
                        data4 += pdt_get_cnb_data(&yuv_info, h, (w * 4 + k) / 2) << (k << 1);
                    }
                } else {
                    data4 = 0;
                    for (k = 0; k < 16; k += 2) { /* 16 :a number 2 :a number */
                        data1 = pdt_get_cnb_data(&yuv_info, h, (w * 4 + k) / 2); /* 4 :a number 2 :a number */
                        data1 = ((data1 & 0x3) << 2) + (data1 >> 2); /* 2 :a number 2 :a number */
                        data4 += data1 << (k << 1);
                    }
                }

                frame_crc += data4;
            }
        }
    }

    return frame_crc;
}

static hi_void crc_get_yuv_param(yuv_param *yuv_param, hi_u8 *y_addr, hi_u8 *c_addr,
                                 hi_u8 *y_addr_2bit, hi_u8 *c_addr_2bit, vfmw_image *image, yuv_addr *yuv_addr)
{
    yuv_addr->y_addr_8bit = g_calc_memory.vir_addr;
    yuv_addr->y_addr_nbit = yuv_addr->y_addr_8bit + (CALC_BUF_SIZE / 2); /* 2 :a number */

    if (image->format.sample_type == 0) {
        yuv_addr->c_addr_8bit = yuv_addr->y_addr_8bit + (MAX_CRC_YUV_SIZE * 2 / 3); /* 2 :a number 3 :a number */
        yuv_addr->c_addr_nbit = yuv_addr->y_addr_nbit + (MAX_CRC_YUV_SIZE * 2 / 3); /* 2 :a number 3 :a number */
    } else {
        yuv_addr->c_addr_8bit = HI_NULL;
        yuv_addr->c_addr_nbit = HI_NULL;
    }

    yuv_param->y_address_8bit = y_addr;
    yuv_param->y_address_nbit = y_addr_2bit;
    if (image->format.sample_type == 0) {
        yuv_param->c_address_8bit = c_addr;
        yuv_param->c_address_nbit = c_addr_2bit;
    } else {
        yuv_param->c_address_8bit = HI_NULL;
        yuv_param->c_address_nbit = HI_NULL;
    }
    yuv_param->stride = image->disp_info.y_stride;
    yuv_param->stride_nbit = image->disp_info.stride_2bit;
    yuv_param->real_bit_depth = (image->bit_depth != 0) ? image->bit_depth : 8; /* 8 :a number */
    yuv_param->is_1d = (image->disp_info.linear_en) ? 0 : 1;

    return;
}

static hi_void crc_get_calc_param(hi_u8 *in_y_addr, hi_u8 *in_c_addr, hi_u32 width, hi_u32 height,
                                  crc_calc_param *calc_param, yuv_addr *yuv_addr)
{
    calc_param->crc_y_addr_8bit = yuv_addr->y_addr_8bit;
    calc_param->crc_y_addr_nbit = yuv_addr->y_addr_nbit;
    calc_param->crc_c_addr_8bit = yuv_addr->c_addr_8bit;
    calc_param->crc_c_addr_nbit = yuv_addr->c_addr_nbit;
    calc_param->crc_width = width;
    calc_param->crc_height = height;
    calc_param->crc_stride = width;

    return;
}

static hi_void crc_adjust_param(vfmw_image *image, crc_calc_param *calc_param)
{
    hi_u32 decode_width, decode_height, display_width, display_height, offset_y, offset_c;

    decode_width = ((image->image_width + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */
    decode_height = ((image->image_height + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */
    offset_y = image->top_offset * decode_width + image->left_offset;
    offset_c = (image->top_offset >> 1) * decode_width + image->left_offset;

    display_width = ((image->disp_info.disp_width + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */
    display_height = ((image->disp_info.disp_height + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */

    calc_param->crc_y_addr_8bit = calc_param->crc_y_addr_8bit + offset_y;
    calc_param->crc_c_addr_8bit = calc_param->crc_c_addr_8bit + offset_c;

    calc_param->crc_width = display_width;
    calc_param->crc_height = display_height;
    calc_param->crc_stride = decode_width;

    return;
}

static hi_void crc_calc_yuv_crc_2bit(crc_calc_param *param, hi_u32 *act_crc)
{
#ifdef NEW_CRC
    /* 4 :a number 2 :a number 4 :a number */
    act_crc[0] = crc_calc_field_crc_nbit(param, param->crc_width / 4, param->crc_height / 2, param->crc_stride / 4, 0);
    /* 4 :a number 2 :a number 4 :a number */
    act_crc[1] = crc_calc_field_crc_nbit(param, param->crc_width / 4, param->crc_height / 2, param->crc_stride / 4, 1);
#endif

    return;
}

static hi_void crc_calc_yuv_crc_8bit(crc_calc_param *param, hi_u32 *act_crc)
{
    /* 2 :a number */
    act_crc[0] = crc_calc_field_crc_8bit(param, param->crc_width, param->crc_height / 2, param->crc_stride, 0);
    /* 2 :a number */
    act_crc[1] = crc_calc_field_crc_8bit(param, param->crc_width, param->crc_height / 2, param->crc_stride, 1);

    return;
}

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
static hi_void crc_calc_actual_crc(vfmw_image *image, hi_u8 *y_addr, hi_u8 *c_addr,
                                   hi_u8 *y_addr_2bit, hi_u8 *c_addr_2bit, hi_u32 *act_crc_8bit, hi_u32 *act_crc_2bit)
{
    hi_u32 width, height;
    crc_calc_param calc_param = { 0 };
    yuv_addr yuv_addr = { 0 };
    yuv_param yuv_param = { 0 };

    if (image->bit_depth > 8) { /* 8 :a number */
        /* 2 :a number 15 :a number 16 :a number 16 :a number 2 :a number */
        width = ((image->image_width * 2 + 15) / 16) * 16 / 2;
        height = ((image->image_height + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */
    } else {
        width = ((image->image_width + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */
        height = ((image->image_height + 15) / 16) * 16; /* 15 :a number 16 :a number 16 :a number */
    }

    crc_get_yuv_param(&yuv_param, y_addr, c_addr, y_addr_2bit, c_addr_2bit, image, &yuv_addr);

    crc_get_calc_param(y_addr, c_addr, width, height, &calc_param, &yuv_addr);

    if (image->vid_std == VFMW_HEVC) {
        /* 2 HEVC : According to display width and height
            OTHER : According to decode  width and height */
        crc_adjust_param(image, &calc_param);
    }

    calc_param.y_addr = yuv_param.y_address_8bit;
    calc_param.c_addr = yuv_param.c_address_8bit;
    calc_param.y_addr_nbit = yuv_param.y_address_nbit;
    calc_param.c_addr_nbit = yuv_param.c_address_nbit;
    calc_param.left_offset = image->left_offset;
    calc_param.top_offset = image->top_offset;
    calc_param.stride = image->disp_info.y_stride;
    calc_param.is_1d = yuv_param.is_1d;
    calc_param.is_uv12 = 1;
    if (image->vid_std == VFMW_HEVC) {
        calc_param.is_uv12 = 0;
    }

    crc_calc_yuv_crc_8bit(&calc_param, act_crc_8bit);

    if (image->bit_depth > 8) { /* 8 :a number */
        crc_calc_yuv_crc_2bit(&calc_param, act_crc_2bit);
    }

    return;
}

hi_s32 crc_calc_view_crc(vfmw_image *image, UADDR luma_phy, UADDR chroma_phy,
    UADDR luma_phy_2bit, UADDR chroma_phy_2bit, hi_u32 *act_crc_8bit, hi_u32 *act_crc_2bit, hi_u32 is_view_1)
{
    hi_u8 *y_addr = HI_NULL;
    hi_u8 *c_addr = HI_NULL;
    hi_u8 *y_addr_2bit = HI_NULL;
    hi_u8 *c_addr_2bit = HI_NULL;

    crc_get_vir_addr(image, luma_phy, chroma_phy, &y_addr, &c_addr, &y_addr_2bit, &c_addr_2bit, is_view_1);
    if (y_addr == HI_NULL || c_addr == HI_NULL) {
        OS_PRINT("%s get viraddr failed YPhy = 0x%x, CPhy = 0x%x\n", __func__, luma_phy, chroma_phy);
        return CRC_ERR;
    }

    crc_calc_actual_crc(image, y_addr, c_addr, y_addr_2bit, c_addr_2bit, act_crc_8bit, act_crc_2bit);

    crc_release_vir_addr(image, luma_phy, chroma_phy, y_addr, c_addr, is_view_1);

    return CRC_OK;
}

/* ------------------------- EXTERNAL FUNCTION ------------------------- */
hi_s32 crc_init_calc_env(hi_void)
{
    VFMW_ASSERT_RET(g_is_crc_calc_init == CRC_FLASE, CRC_ERR);

    VFMW_CHECK_SEC_FUNC(memset_s(&g_calc_memory, sizeof(g_calc_memory), 0, sizeof(g_calc_memory)));

#ifndef NEW_CRC
    g_calc_memory.vir_addr = OS_ALLOC_VIR("CRC_CALC", CALC_BUF_SIZE);
    if (g_calc_memory.vir_addr == HI_NULL) {
        OS_PRINT("%s alloc calc mem size %d failed!\n", __func__, CALC_BUF_SIZE);
        return CRC_ERR;
    }
#endif

    g_is_crc_calc_init = CRC_TRUE;

    return CRC_OK;
}

hi_s32 crc_exit_calc_env(hi_void)
{
    VFMW_ASSERT_RET(g_is_crc_calc_init == CRC_TRUE, CRC_ERR);

#ifndef NEW_CRC
    if (g_calc_memory.vir_addr != 0) {
        OS_FREE_VIR(g_calc_memory.vir_addr);
    }
#endif

    VFMW_CHECK_SEC_FUNC(memset_s(g_MulticrcStrm, sizeof(g_MulticrcStrm), 0, sizeof(g_MulticrcStrm)));
    VFMW_CHECK_SEC_FUNC(memset_s(g_MulticrcNum, sizeof(g_MulticrcNum), 0, sizeof(g_MulticrcNum)));
    g_crc_inst_num = 0;

    g_is_crc_calc_init = CRC_FLASE;

    return CRC_OK;
}

hi_s32 crc_calc_image_crc(vfmw_image *image)
{
    hi_s32 ret;

    VFMW_ASSERT_RET(g_is_crc_calc_init == CRC_TRUE, CRC_ERR);
    VFMW_ASSERT_RET(image != HI_NULL, CRC_ERR);

    ret = crc_calc_view_crc(image, image->disp_info.luma_phy_addr, image->disp_info.chrom_phy_addr,
                            image->disp_info.luma_phy_addr_2bit, image->disp_info.chrom_phy_addr_2bit,
                            image->crc_info.actual_crc_8bit, image->crc_info.actual_crc_2bit, 0);
    if (ret != CRC_OK) {
        return CRC_ERR;
    }

    if (image->is_3d == 1) {
        ret = crc_calc_view_crc(image, image->disp_info.luma_phy_addr_1, image->disp_info.chrom_phy_addr_1,
                                0, 0, image->crc_info.actual_crc_8bit_1, HI_NULL, 1);
        if (ret != CRC_OK) {
            return CRC_ERR;
        }
    }

    return CRC_OK;
}

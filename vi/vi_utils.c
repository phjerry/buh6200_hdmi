/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi utils
 * Author: sdk
 * Create: 2019-12-14
 */

#include "vi_comm.h"
#include "vi_type.h"

#include "vi_utils.h"

typedef enum {
    VICAP_DRV_FRMTYPE_EYE_LEFT = 0,
    VICAP_DRV_FRMTYPE_EYE_RIGHT,

    VICAP_DRV_FRMTYPE_EYE_MAX,
} vicap_drv_frmtype;

#pragma pack(1)
typedef struct {
    unsigned short a0 : 10;
    unsigned short b0 : 10;
    unsigned short a1 : 10;
    unsigned short b1 : 10;
} alignment_10bits;
#pragma pack()

typedef struct {
    hi_u32 uv_height;
    hi_u32 uv_width;
    hi_u32 uv_src_width;
    hi_u32 uv_buf_size;
    hi_u32 y_buf_size;
    hi_u32 sum_buf_size;
} vicap_drv_utils;

typedef struct {
    vicap_drv_utils ut_info;
    vi_buffer_addr_info buf;

    hi_u32 offset;

    hi_void *fp;
    hi_u8 *ptr_base;

    hi_u8 *ydata_p;
    hi_u8 *udata_p;
    hi_u8 *vdata_p;
} write_yuv_param;

typedef struct {
    hi_u32 offset;
    hi_u8 *ptr_from_p;

    hi_u8 *udata_to_p;
    hi_u8 *vdata_to_p;

    hi_u32 strip;
} write_uv_param;

VI_STATIC hi_void *vi_drv_utils_fopen(const char *filename, int flags, int mode)
{
    hi_void *filp = HI_NULL;

    filp = osal_klib_fopen(filename, flags, mode);

    return filp;
}

VI_STATIC hi_void vi_drv_utils_fclose(hi_void *filp)
{
    if (filp != HI_NULL) {
        osal_klib_fclose(filp);
    }
}

VI_STATIC int vi_drv_utils_fwrite(const char *buf, int len, hi_void *filp)
{
    int writelen;

    writelen = osal_klib_fwrite(buf, len, filp);
    vi_drv_log_dbg("write:%d\n", writelen);

    return writelen;
}

VI_STATIC hi_s32 vi_drv_utils_get_pixel_format(hi_drv_video_frame *frame_p, hi_char **format_pp, hi_char **color_pp,
                                               hi_char **range_pp, hi_char **space_pp)
{
    hi_char *temp_format = HI_NULL;
    hi_char *temp_color = HI_NULL;
    hi_char *temp_range = HI_NULL;
    hi_char *temp_space = HI_NULL;

    if (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV42_RGB) {
        temp_format = "prgb444";
    } else if (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV42) {
        temp_format = "pyuv444";
    } else if (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1) {
        temp_format = "pyuv422";
    } else {
        temp_format = "pyuv420";
    }

    switch (frame_p->color_space.color_primary) {
        case HI_DRV_COLOR_PRIMARY_BT601_525:
            temp_color = "601525";
            break;
        case HI_DRV_COLOR_PRIMARY_BT601_625:
            temp_color = "601625";
            break;
        case HI_DRV_COLOR_PRIMARY_BT709:
            temp_color = "709";
            break;
        case HI_DRV_COLOR_PRIMARY_BT2020:
            temp_color = "2020";
            break;
        default:
            vi_drv_log_err("not support this color_space[%d]\n", frame_p->color_space);
            return HI_FAILURE;
            break;
    }

    if (frame_p->color_space.color_space == HI_DRV_COLOR_CS_YUV) {
        temp_space = "yuv";
    } else {
        temp_space = "rgb";
    }

    if (frame_p->color_space.quantify_range == HI_DRV_COLOR_LIMITED_RANGE) {
        temp_range = "l";
    } else {
        temp_range = "f";
    }

    *format_pp = temp_format;
    *color_pp = temp_color;
    *range_pp = temp_range;
    *space_pp = temp_space;

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_utils_get_offset(hi_drv_pixel_bitwidth bit_width, hi_u32 *offset_p,
                                         vi_saveframe_bitwidth save_frame_bit)
{
    if (bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
        *offset_p = 1;
        /* not support 8 bit save as 10 bit */
        if (save_frame_bit == VI_SAVEFRAME_BITWIDTH_10BIT) {
            vi_drv_log_err("For 8bit data, not support save using 10bit.\n");
            return HI_FAILURE;
        }

        return HI_SUCCESS;
    } else if (bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        /* save 10bit as 8 bit */
        if (save_frame_bit == VI_SAVEFRAME_BITWIDTH_8BIT) {
            *offset_p = 5; /* 5: step is 5 byte */
        } else {
            /* save 10bit as 10 bit */
            *offset_p = 10; /* 10: step is 10 byte */
        }

        return HI_SUCCESS;
    } else {
        vi_drv_log_err("not support this bitwidth\n");
    }

    return HI_FAILURE;
}

VI_STATIC hi_s32 vi_drv_utils_get_rect_info(hi_drv_video_frame *frame_p, vicap_drv_utils *ut_info_p,
                                            vi_saveframe_bitwidth save_frame_bit)
{
    if ((frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV21) || (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV12)) {
        /* YUV420 */
        ut_info_p->uv_height = frame_p->height / 2; /* yuv420 need / 2 */
        ut_info_p->uv_width = frame_p->width / 2;   /* yuv420 need / 2 */
        ut_info_p->uv_src_width = frame_p->width;
        ut_info_p->sum_buf_size =
            frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y * frame_p->height * 2; /* yuv420 bufsize is y size * 2 */
    } else if ((frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1) ||
               (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV16_2X1)) {
        /* YUV422 */
        ut_info_p->uv_height = frame_p->height;
        ut_info_p->uv_width = frame_p->width / 2; /* yuv422 need / 2 */
        ut_info_p->uv_src_width = frame_p->width;
        ut_info_p->sum_buf_size =
            frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y * frame_p->height * 2; /* yuv422 bufsize is y size * 2 */
    } else if ((frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV42) || (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV24) ||
               (frame_p->pixel_format == HI_DRV_PIXEL_FMT_NV42_RGB)) {
        /* YUV444 */
        ut_info_p->uv_height = frame_p->height;
        ut_info_p->uv_width = frame_p->width;
        ut_info_p->uv_src_width = frame_p->width * 2; /* yuv444 need * 2 */
        ut_info_p->sum_buf_size =
            frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y * frame_p->height * 3; /* yuv444 bufsize is y size * 3 */
    } else {
        vi_drv_log_err("not support this type(%d)\n", frame_p->pixel_format);
        return HI_FAILURE;
    }

    ut_info_p->y_buf_size = frame_p->height * frame_p->width;
    ut_info_p->uv_buf_size = ut_info_p->uv_height * ut_info_p->uv_width;

    vi_drv_log_dbg("bit_width=%d, save_frame_bit=%d.\n", frame_p->bit_width, save_frame_bit);
    if (frame_p->bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        /* 10bit source data, not convert to 8bit, save as 10bit */
        if (save_frame_bit == VI_SAVEFRAME_BITWIDTH_10BIT) {
            /* 10bit: pstUtInfo->u32YBufSize: size of Y buffer, pstFrame->u32Width: size of Y pixel */
            /* pstUtInfo->u32UvBufSize: size of U/v buffer, pstUtInfo->u32UvWidth size of U/V pixel */
            ut_info_p->y_buf_size = frame_p->height * (frame_p->width * 10 / 8);            /* 10: 10bit, 8: 8bit */
            ut_info_p->uv_buf_size = ut_info_p->uv_height * (ut_info_p->uv_width * 10 / 8); /* 10: 10bit, 8: 8bit */
            vi_drv_log_dbg("save as 10 bit, y_size=%d, uv_size=%d.\n", ut_info_p->y_buf_size, ut_info_p->uv_buf_size);
        }
    }

    vi_drv_log_notice("uv_height=%d, uv_width=%d, uv_src_width=%d, sum_buf_size=%d, y_buf_size=%d, uv_buf_size=%d.\n",
                      ut_info_p->uv_height, ut_info_p->uv_width, ut_info_p->uv_src_width, ut_info_p->sum_buf_size,
                      ut_info_p->y_buf_size, ut_info_p->uv_buf_size);

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_utils_trans10to8bit(hi_u8 *ptr_in, hi_u32 in_len, hi_u8 *ptr_out, hi_u32 out_len)
{
    if (in_len < 5 || out_len < 4) { /* 4 5 is min */
        vi_drv_log_err("in out erro\n");
        return HI_FAILURE;
    }

    /* 10bit convert to 8bit, 5 * 8bit data == 4 * 10bit data */
    ptr_out[0] = ((ptr_in[1] & 0x03) << 6) | ((ptr_in[0] & 0xfc) >> 2); /* index 0 and 1, 6 and 2 is shift bit num */
    ptr_out[1] = ((ptr_in[2] & 0x0f) << 4) | ((ptr_in[1] & 0xf0) >> 4); /* index 1 and 2, 6 and 4 is shift bit num */
    ptr_out[2] = ((ptr_in[3] & 0x3f) << 2) | ((ptr_in[2] & 0xc0) >> 6); /* index 2 and 3, 6 and 6 is shift bit num */
    ptr_out[3] = (ptr_in[4]);                                           /* index 3 and 4 */

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_utils_write_one_line_y(hi_u8 *ptr_y, hi_u8 *ptr, hi_u32 offset, hi_u32 len)
{
    hi_u32 i = 0;
    hi_u32 step = 0;
    hi_u8 au8_tmp8bit[4] = {0}; /* len is 4 */
    errno_t err;

    HI_UNUSED(step);

    if (offset == 1) {
        /* 8bit no need convert */
        err = memcpy_s(ptr_y, sizeof(hi_u8) * len, ptr, sizeof(hi_u8) * len);
        if (err != EOK) {
        }
    } else if (offset == 5) { /* 5 is convert offset */
        /* 10bit need convert, use hight 8bit */
        step = len * 10 / 8 / offset; /* 10 is in, 8 is out */

        for (i = 0; i < step; i++) {
            /* trans 10bit to 8bit */
            err = memset_s(au8_tmp8bit, sizeof(hi_u8) * 4, 0, sizeof(hi_u8) * 4); /* 4 is step */
            if (err != EOK) {
            }
            vi_drv_utils_trans10to8bit(&ptr[offset * i], sizeof(hi_u8) * 5,                   /* 5 is in byte */
                                       au8_tmp8bit, sizeof(hi_u8) * 4);                       /* 4 is out byte */
            err = memcpy_s(&ptr_y[i * 4], sizeof(hi_u8) * 4, au8_tmp8bit, sizeof(hi_u8) * 4); /* 4 is step */
            if (err != EOK) {
            }
        }
    } else if (offset == 10) {                                      /* 10: step is 10 byte */
        err = memcpy_s(ptr_y, (len * 10 / 8), ptr, (len * 10 / 8)); /* (len*10/8) total length */
        if (err != EOK) {
        }
    }

    return HI_SUCCESS;
}

VI_STATIC hi_void vi_drv_utils_write_one_line_uv_for_10bit(hi_u8 *ptr_u, hi_u8 *ptr_v, hi_u8 *ptr, hi_u32 len,
                                                           hi_drv_pixel_format pix_format)
{
    hi_u32 i = 0;
    hi_u32 step;
    hi_u32 offset = 10;                  /* 10: 10bit */
    alignment_10bits tmp10bit_before[2]; /* 2: 2 * 5 byte data */
    alignment_10bits tmp10bit_after[2];  /* 2: 2 * 5 byte data */

    step = len * 10 / 8 / offset; /* (len * 10 / 8)total lenth, step is 10 byte */
    for (i = 0; i < step; i++) {
        if (EOK != memset_s(tmp10bit_before, sizeof(tmp10bit_before), 0, sizeof(tmp10bit_before))) {
        }
        if (EOK != memset_s(tmp10bit_after, sizeof(tmp10bit_after), 0, sizeof(tmp10bit_after))) {
        }
        /* ...UVUVUV... U/V unit use 10bit, save as 10Bytes(80bit), each ptr_u/ptr_v save 5Bytes. */
        if (EOK != memcpy_s(tmp10bit_before, sizeof(tmp10bit_before), ptr + (offset * i), offset)) {
        }
        /* get U/V data */
        tmp10bit_after[0].a0 = tmp10bit_before[0].a0;
        tmp10bit_after[0].b0 = tmp10bit_before[0].a1;
        tmp10bit_after[0].a1 = tmp10bit_before[1].a0;
        tmp10bit_after[0].b1 = tmp10bit_before[1].a1;

        tmp10bit_after[1].a0 = tmp10bit_before[0].b0;
        tmp10bit_after[1].b0 = tmp10bit_before[0].b1;
        tmp10bit_after[1].a1 = tmp10bit_before[1].b0;
        tmp10bit_after[1].b1 = tmp10bit_before[1].b1;

        if ((HI_DRV_PIXEL_FMT_NV21 == pix_format) || (HI_DRV_PIXEL_FMT_NV61_2X1 == pix_format) ||
            (HI_DRV_PIXEL_FMT_NV42 == pix_format) || (HI_DRV_PIXEL_FMT_NV42_RGB == pix_format)) {
            if (EOK != memcpy_s(&ptr_v[i * 5], sizeof(alignment_10bits), /* 5: 5 byte */
                                &tmp10bit_after[0], sizeof(alignment_10bits))) {
            }
            if (EOK != memcpy_s(&ptr_u[i * 5], sizeof(alignment_10bits), /* 5: 5 byte */
                                &tmp10bit_after[1], sizeof(alignment_10bits))) {
            }
        } else {
            if (EOK != memcpy_s(&ptr_u[i * 5], sizeof(alignment_10bits), /* 5: 5 byte */
                                &tmp10bit_after[0], sizeof(alignment_10bits))) {
            }
            if (EOK != memcpy_s(&ptr_v[i * 5], sizeof(alignment_10bits), /* 5: 5 byte */
                                &tmp10bit_after[1], sizeof(alignment_10bits))) {
            }
        }
    }
}

VI_STATIC hi_s32 vi_drv_utils_write_one_line_uv(write_uv_param *uv_param_p, hi_drv_pixel_format pix_format)
{
    hi_u32 i;
    hi_u32 step;
    hi_u8 au8_tmp8bit[4] = {0}; /* len is 4 */
    hi_u8 *ptr_u = uv_param_p->udata_to_p;
    hi_u8 *ptr_v = uv_param_p->vdata_to_p;
    hi_u8 *ptr = uv_param_p->ptr_from_p;
    hi_u32 offset = uv_param_p->offset;
    hi_u32 len = uv_param_p->strip;

    if (offset == 1) {
        /* src input 8bit */
        for (i = 0; i < len / 2; i++) { /* 10bit need / 2  */
            if ((HI_DRV_PIXEL_FMT_NV21 == pix_format) || (HI_DRV_PIXEL_FMT_NV61_2X1 == pix_format) ||
                (HI_DRV_PIXEL_FMT_NV42 == pix_format) || (HI_DRV_PIXEL_FMT_NV42_RGB == pix_format)) {
                ptr_v[i] = ptr[2 * i];     /* index 2 * i */
                ptr_u[i] = ptr[2 * i + 1]; /* index 2 * i + 1 */
            } else {
                ptr_u[i] = ptr[2 * i];     /* index 2 * I */
                ptr_v[i] = ptr[2 * i + 1]; /* index 2 * i + 1 */
            }
        }
    } else if (offset == 5) { /* 5 is 10bit to 8bit offset */
        /* src input 10bit, trans 10bit to 8bit */
        step = len * 10 / 8 / offset; /* 10 and 8 are used to calculate step */
        for (i = 0; i < step; i++) {
            if (EOK != memset_s(au8_tmp8bit, sizeof(hi_u8) * 4, 0, sizeof(hi_u8) * 4)) { /* 4 is step  */
            }
            vi_drv_utils_trans10to8bit(&ptr[offset * i], sizeof(hi_u8) * 5, /* 5 is in */
                                       au8_tmp8bit, sizeof(hi_u8) * 4);     /* 4 is out */

            if ((HI_DRV_PIXEL_FMT_NV21 == pix_format) || (HI_DRV_PIXEL_FMT_NV61_2X1 == pix_format) ||
                (HI_DRV_PIXEL_FMT_NV42 == pix_format) || (HI_DRV_PIXEL_FMT_NV42_RGB == pix_format)) {
                ptr_v[i * 2] = au8_tmp8bit[0];     /* index i * 2 */
                ptr_v[i * 2 + 1] = au8_tmp8bit[2]; /* index i * 2 + 1 */
                ptr_u[i * 2] = au8_tmp8bit[1];     /* index i * 2 */
                ptr_u[i * 2 + 1] = au8_tmp8bit[3]; /* index 2 * 2 + 1, index 3 */
            } else {
                ptr_u[i * 2] = au8_tmp8bit[0];     /* index i * 2 */
                ptr_u[i * 2 + 1] = au8_tmp8bit[2]; /* index i * 2 + 1 */
                ptr_v[i * 2] = au8_tmp8bit[1];     /* index i * 2 */
                ptr_v[i * 2 + 1] = au8_tmp8bit[3]; /* index 2 * 2 + 1, index 3 */
            }
        }
    } else if (offset == 10) { /* 10 is 10bit */
        vi_drv_utils_write_one_line_uv_for_10bit(ptr_u, ptr_v, ptr, len, pix_format);
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_utils_write_yuv_alloc_resource(hi_drv_video_frame *frame_p, hi_char *file_p,
                                                       write_yuv_param *param_p)
{
    hi_s32 ret;

    param_p->ydata_p = osal_vmalloc(HI_ID_VI, param_p->ut_info.y_buf_size);
    param_p->udata_p = osal_vmalloc(HI_ID_VI, param_p->ut_info.uv_buf_size);
    param_p->vdata_p = osal_vmalloc(HI_ID_VI, param_p->ut_info.uv_buf_size);
    if ((param_p->ydata_p == HI_NULL) || (param_p->udata_p == HI_NULL) || (param_p->vdata_p == HI_NULL)) {
        vi_drv_log_err("alloc fail!\n");
        return HI_FAILURE;
    }

    ret = vi_comm_mmap(&param_p->buf);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("mmap fail, [0x%x, %x]\n", param_p->buf.start_phy_addr, param_p->buf.size);
        return ret;
    }

    param_p->fp = vi_drv_utils_fopen(file_p, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, 0777); /* 0777 is authority */
    if (param_p->fp == HI_NULL) {
        vi_drv_log_err("open file '%s' fail!\n", file_p);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_void vi_drv_utils_write_yuv_release_resource(write_yuv_param *param_p)
{
    if (param_p->fp != HI_NULL) {
        vi_drv_utils_fclose(param_p->fp);
    }

    vi_comm_unmap(&param_p->buf);

    if (param_p->vdata_p != HI_NULL) {
        osal_vfree(HI_ID_VI, param_p->vdata_p);
    }

    if (param_p->udata_p != HI_NULL) {
        osal_vfree(HI_ID_VI, param_p->udata_p);
    }

    if (param_p->ydata_p != HI_NULL) {
        osal_vfree(HI_ID_VI, param_p->ydata_p);
    }
}

VI_STATIC hi_s32 vi_drv_utils_write_yuv_to_file(write_yuv_param *param_p)
{
    hi_s32 len;
    vicap_drv_utils *ut_info_p = &param_p->ut_info;

    len = vi_drv_utils_fwrite(param_p->ydata_p, ut_info_p->y_buf_size, param_p->fp);
    if (len < ut_info_p->y_buf_size) {
        return HI_FAILURE;
    }

    len = vi_drv_utils_fwrite(param_p->udata_p, ut_info_p->uv_buf_size, param_p->fp);
    if (len < ut_info_p->uv_buf_size) {
        return HI_FAILURE;
    }

    len = vi_drv_utils_fwrite(param_p->vdata_p, ut_info_p->uv_buf_size, param_p->fp);
    if (len < ut_info_p->uv_buf_size) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_utils_write_yuv_to_memory(hi_drv_video_frame *frame_p, vi_saveframe_bitwidth save_frame_bit,
                                                  write_yuv_param *param_p)
{
    hi_s32 ret;
    hi_u32 i;
    vicap_drv_utils *ut_info_p = &param_p->ut_info;

    hi_u8 *ptr = HI_NULL;
    hi_u8 *ptr_base = param_p->buf.start_vir_addr_p; /* point to video frame buffer */

    hi_u8 *ydata_p = param_p->ydata_p;
    hi_u8 *udata_p = param_p->udata_p;
    hi_u8 *vdata_p = param_p->vdata_p;
    hi_u32 offset = param_p->offset;

    /* default use 8bit */
    hi_s32 offset_wide_y = frame_p->width;
    hi_s32 offset_wide_uv = ut_info_p->uv_width;

    write_uv_param uv_param = {0};

    /* source data is 10bit, judgement save as original format or converted to 8 bit */
    if (frame_p->bit_width == HI_DRV_PIXEL_BITWIDTH_10BIT) {
        if (save_frame_bit == VI_SAVEFRAME_BITWIDTH_10BIT) {
            vi_drv_log_dbg("the data is 10bit, and save as 10bit.\n");
            /* total bytes is width * 10 / 8 */
            offset_wide_y = frame_p->width * 10 / 8;       /* 10 is 10 bit, 8 is 8 bit */
            offset_wide_uv = ut_info_p->uv_width * 10 / 8; /* 10 is 10 bit, 8 is 8 bit */
        }
    }

    /* Y */
    vi_drv_log_notice("write y... w=%d, h=%d, ystride=%d, y_buf_size=%d.\n", offset_wide_y, frame_p->height,
                      frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y, ut_info_p->y_buf_size);

    ptr = ptr_base;
    for (i = 0; i < frame_p->height; i++) {
        ret = vi_drv_utils_write_one_line_y(ydata_p + i * offset_wide_y, ptr, offset, frame_p->width);
        if (ret != HI_SUCCESS) {
            return ret;
        }
        ptr += frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_y;
    }

    /* UV */
    vi_drv_log_notice("write c... srcw=%d, w=%d, h=%d, cstride=%d, uv_buf_size=%d.\n", ut_info_p->uv_src_width,
                      offset_wide_uv, ut_info_p->uv_height, frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_c,
                      ut_info_p->uv_buf_size);

    ptr = ptr_base + frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].c_offset;
    for (i = 0; i < ut_info_p->uv_height; i++) {
        uv_param.offset = offset;
        uv_param.ptr_from_p = ptr;
        uv_param.udata_to_p = udata_p + i * offset_wide_uv;
        uv_param.vdata_to_p = vdata_p + i * offset_wide_uv;
        uv_param.strip = ut_info_p->uv_src_width;
        ret = vi_drv_utils_write_one_line_uv(&uv_param, frame_p->pixel_format);
        if (ret != HI_SUCCESS) {
            return ret;
        }
        ptr += frame_p->buf_addr[HI_DRV_3D_EYE_LEFT].stride_c;
    }

    return HI_SUCCESS;
}

/* The frame memory data in 8 bit or 10 bit format written to the file */
VI_STATIC hi_s32 vi_drv_utils_write_yuv_by_format(vi_buf_node *node_p, hi_char *file_p,
                                                  vi_saveframe_bitwidth save_frame_bit)
{
    hi_s32 ret;
    write_yuv_param param = {0};
    hi_drv_video_frame *frame_p = &node_p->frame_info;

    vi_drv_log_notice("write yuv..., save_frame_bit = %d.\n", save_frame_bit);

    param.buf = node_p->frame_addr.video_buf_addr;

    ret = vi_drv_utils_get_rect_info(frame_p, &param.ut_info, save_frame_bit);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = vi_drv_utils_get_offset(frame_p->bit_width, &param.offset, save_frame_bit);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_log_notice("offset=%d.\n", param.offset);

    ret = vi_drv_utils_write_yuv_alloc_resource(frame_p, file_p, &param);
    if (ret != HI_SUCCESS) {
        goto fail;
    }

    ret = vi_drv_utils_write_yuv_to_memory(frame_p, save_frame_bit, &param);
    if (ret != HI_SUCCESS) {
        goto fail;
    }

    ret = vi_drv_utils_write_yuv_to_file(&param);
    if (ret != HI_SUCCESS) {
        goto fail;
    }

    vi_drv_utils_write_yuv_release_resource(&param);

    HI_PRINT("image saved to '%s' WH(%d*%d)index(%d)\n", file_p, frame_p->width, frame_p->height, frame_p->frame_index);
    return HI_SUCCESS;

fail:
    vi_drv_utils_write_yuv_release_resource(&param);
    return HI_FAILURE;
}

hi_s32 vi_utils_write_frame(hi_char *file_p, vi_buf_node *node_p, vi_saveframe_bitwidth bit_width)
{
    hi_s32 ret;
    hi_u32 len;
    hi_s8 str[VI_DUMP_NAME_LEN] = {"/mnt"};

    len = strlen(str);
    ret = snprintf_s(str + len, VI_DUMP_NAME_LEN - len, VI_DUMP_NAME_LEN - len - 1, "/%s", file_p);
    if (ret < 0) {
        return HI_FAILURE;
    }

    vi_drv_log_notice("path:%s\n", str);

    ret = vi_drv_utils_write_yuv_by_format(node_p, str, bit_width);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 vi_utils_generate_frame_name(hi_drv_video_frame *frame_p, hi_s8 *file_name_p, hi_u32 size)
{
    hi_s32 ret;
    hi_u32 len;
    hi_char *format = HI_NULL;
    hi_char *color = HI_NULL;
    hi_char *range = HI_NULL;
    hi_char *space = HI_NULL;

    if (size < VI_DUMP_NAME_LEN) {
        vi_drv_log_err("name array too small!\n");
        return HI_FAILURE;
    }

    ret = vi_drv_utils_get_pixel_format(frame_p, &format, &color, &range, &space);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    ret = snprintf_s(file_name_p, VI_DUMP_NAME_LEN, VI_DUMP_NAME_LEN - 1, "vi_sdr2d_%s_8b_%s_%s_%s", format, color,
                     space, range);
    if (ret < 0) {
        vi_drv_log_err("snprintf_s failed\n");
        return HI_FAILURE;
    }

    len = strlen(file_name_p);
    ret = snprintf_s(file_name_p + len, VI_DUMP_NAME_LEN - len, VI_DUMP_NAME_LEN - len - 1, "_%dx%d%s%dhz.yuv",
                     frame_p->width, frame_p->height, frame_p->sample_type == HI_DRV_SAMPLE_TYPE_INTERLACE ? "i" : "p",
                     frame_p->frame_rate / FRAME_PRECISION);
    if (ret < 0) {
        vi_drv_log_err("snprintf_s failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

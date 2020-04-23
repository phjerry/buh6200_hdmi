/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include "drv_venc_efl.h"
#include "drv_omxvenc_efl.h"
#include "hi_drv_mem.h"
#include "drv_venc.h"

#include "hi_drv_log.h"

#include "hi_reg_common.h"

#include "hi_drv_stat.h"
#include "hi_drv_sys.h"
#include "hi_osal.h"
#include "linux/hisilicon/securec.h"

#ifdef VENC_SUPPORT_JPGE
#include "drv_jpge_ext.h"
#endif

#ifdef VENC_DPT_ONLY
#include "hi_math.h"
#endif

#include "venc_hal_ext.h"
#include "public.h"
#include "sysconfig.h"
#include "product.h"

#include "drv_venc_h265e_dpb.h"
#include "drv_venc_h265e_ref.h"
#include "drv_venc_h264e_dpb.h"
#include "drv_venc_h264e_ref.h"
#include "drv_venc_ratecontrol_client.h"

venc_hal_fun_ptr g_venc_hal_fun_ptr;
extern venc_osal_func_ptr g_venc_osal_func_ptr;

/* #define __VENC_FREQ_DYNAMIC__ */
extern hi_void venc_drv_board_init(hi_void);
extern hi_void venc_drv_board_deinit(hi_void);
extern osal_semaphore g_venc_mutex;
extern osal_spinlock *g_rc_lock;
extern vedu_osal_event g_rc_event;
#ifndef HI_ADVCA_FUNCTION_RELEASE
#define SMOOTH_PRINK printk
#else
#define SMOOTH_PRINK(format, arg...)
#endif

enum {
    VENC_PACKAGE_UY0VY1 = 0b10001101,
    VENC_PACKAGE_Y0UY1V = 0b11011000,
    VENC_PACKAGE_Y0VY1U = 0b01111000
};

enum {
    VENC_SEMIPLANNAR_420_UV = 0,
    VENC_SEMIPLANNAR_420_VU = 1,
    VENC_PLANNAR_420 = 2,
    VENC_PLANNAR_422 = 3,
    VENC_PACKAGE_422_YUYV = 4,
    VENC_PACKAGE_422_UYVY = 5,
    VENC_PACKAGE_422_YVYU = 6,
    VENC_SEMIPLANNAR_422 = 7,
    VENC_UNKNOW = 8
};

enum {
    JPGE_STORE_SEMIPLANNAR = 0,
    JPGE_STORE_PLANNAR = 1,
    JPGE_STORE_PACKAGE = 2
};

enum {
    VEDU_CAP_LEVEL_QCIF = 0,  /* the resolution of the picture to be decoded is less than or equal to 176x144. */
                              /* CNcomment: 解码的图像大小不超过176*144 */
    VEDU_CAP_LEVEL_CIF = 1,   /* the resolution of the picture to be decoded less than or equal to 352x288. */
                              /* CNcomment: 解码的图像大小不超过352*288 */
    VEDU_CAP_LEVEL_D1 = 2,    /* the resolution of the picture to be decoded less than or equal to 720x576. */
                              /* CNcomment: 解码的图像大小不超过720*576 */
    VEDU_CAP_LEVEL_720P = 3,  /* the resolution of the picture to be decoded is less than or equal to 1280x720. */
                              /* CNcomment: 解码的图像大小不超过1280*720 */
    VEDU_CAP_LEVEL_1080P = 4, /* the resolution of the picture to be decoded is less than or equal to 1920x1080. */
                              /* CNcomment: 解码的图像大小不超过1920*1080 */
	VEDU_CAP_LEVEL_4K = 5,    /* the resolution of the picture to be decoded is less than or equal to 1920x1080. */
                              /* CNcomment: 解码的图像大小不超过1920*1080 */
    VEDU_CAP_LEVEL_JPEG_EXT = 6,
    VEDU_CAP_LEVEL_BUTT
} ;

#ifdef __VENC_DRV_DBG__

hi_u32 g_tmp_time_dbg[100][7] = {{0, 0, 0, 0, 0, 0, 0}}; /* 100, 7: array size */
hi_u32 g_isr_id = 0;
hi_u32 g_put_msg_ebd_id = 0;
hi_u32 g_put_msg_fbd_id = 0;
hi_u32 g_get_msg_ebd_id = 0;
hi_u32 g_get_msg_fbd_id = 0;
hi_bool g_flage_dbg = 1;

#endif

#ifdef VENC_SUPPORT_JPGE
static const hi_u8 g_zig_zag_scan[64] = {   /* 64: array size */
    0,  1,  5,  6, 14, 15, 27, 28,
    2,  4,  7, 13, 16, 26, 29, 42,
    3,  8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

static const hi_u8 g_jpge_yqt[64] = {       /* 64: array size */
    16, 11, 10, 16,  24,  40,  51,  61,
    12, 12, 14, 19,  26,  58,  60,  55,
    14, 13, 16, 24,  40,  57,  69,  56,
    14, 17, 22, 29,  51,  87,  80,  62,
    18, 22, 37, 56,  68, 109, 103,  77,
    24, 35, 55, 64,  81, 104, 113,  92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103,  99
};
static const hi_u8 g_jpge_cqt[64] = {       /* 64: array size */
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

/* 2(SOI)+18(APP0)+207(DQT)+19(SOF)+432(DHT)+6(DRI)+14(SOS) */
static const hi_u8 g_jpge_jfif_hdr[698] = { /* 698: array size */
    0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x00, 0xff, 0xdb, 0x00, 0x43, 0x00, 0x10, 0x0b, 0x0c, 0x0e, 0x0c, 0x0a, 0x10, 0x0e, 0x0d, 0x0e, 0x12, 0x11, 0x10,
    0x13, 0x18, 0x28, 0x1a, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25, 0x1d, 0x28, 0x3a, 0x33, 0x3d, 0x3c, 0x39, 0x33,
    0x38, 0x37, 0x40, 0x48, 0x5c, 0x4e, 0x40, 0x44, 0x57, 0x45, 0x37, 0x38, 0x50, 0x6d, 0x51, 0x57, 0x5f, 0x62, 0x67,
    0x68, 0x67, 0x3e, 0x4d, 0x71, 0x79, 0x70, 0x64, 0x78, 0x5c, 0x65, 0x67, 0x63, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x11,
    0x12, 0x12, 0x18, 0x15, 0x18, 0x2f, 0x1a, 0x1a, 0x2f, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xff, 0xdb, 0x00, 0x43, 0x02, 0x11, 0x12, 0x12, 0x18, 0x15, 0x18, 0x2f, 0x1a,
    0x1a, 0x2f, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xff,
    0xc0, 0x00, 0x11, 0x08, 0x01, 0x20, 0x01, 0x60, 0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x02, 0xff,
    0xc4, 0x00, 0x1f, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0xc4, 0x00, 0x1f, 0x01, 0x00,
    0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0xc4, 0x00, 0xb5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02,
    0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21,
    0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1,
    0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53,
    0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9,
    0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0xff, 0xc4, 0x00, 0xb5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00,
    0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72,
    0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4,
    0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6,
    0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xdd, 0x00, 0x04, 0x00, 0x64,
    0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00
};
#endif

/*******************************************************************/
vedu_efl_chn_ctx g_vedu_chn_ctx[HI_VENC_MAX_CHN];

vedu_efl_ip_ctx g_vedu_ip_ctx;

extern optm_venc_chn g_venc_chn[HI_VENC_MAX_CHN];

extern vedu_osal_event g_venc_wait_stream[HI_VENC_MAX_CHN];

extern osal_spinlock g_send_frame_lock[HI_VENC_MAX_CHN];     /* lock the destroy and send frame */
vedu_osal_event g_venc_event;
vedu_osal_event g_venc_event_stream;

static hi_void venc_drv_get_last_frm_info_omx(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in);

#ifdef VENC_SUPPORT_JPGE
extern hi_jpge_export_func *g_jpge_func;
#endif

extern hi_s8 g_priority_tab[2][16];     /* 2, 16: array size */

#define D_VENC_GET_CHN(ve_chn, venc_chn) do { \
        if ((venc_chn) == NULL) { \
            (ve_chn) = HI_VENC_MAX_CHN; \
            break; \
        } \
        for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
            if (g_venc_chn[ve_chn].venc_handle == (venc_chn)) { break; } \
        } \
    } while (0)

#define D_VENC_GET_CHN_BY_UHND(ve_chn, venc_usr_chn) \
    do { \
        if ((venc_usr_chn) == HI_INVALID_HANDLE) { \
            (ve_chn) = HI_VENC_MAX_CHN; \
            break; \
        } \
        for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
            if (g_venc_chn[ve_chn].user_handle == (venc_usr_chn)) { break; } \
        } \
    } while (0)

#define D_VENC_GET_CHN_FROM_TAB(chn_id, tab_no)\
    do {  \
        (chn_id) = g_priority_tab[0][(tab_no)];\
    } while (0)

#define D_VENC_RC_ABS(x, y) (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

#define D_VENC_RC_UPDATE(data, curdata) \
    do {\
        *((data) + 5) = *((data) + 4);\
        *((data) + 4) = *((data) + 3);\
        *((data) + 3) = *((data) + 2);\
        *((data) + 2) = *((data) + 1);\
        *((data) + 1) = *(data);\
        *(data) = (curdata);\
    } while (0)

#define D_VENC_RC_MAX(x, y) ((x) > (y) ? (x) : (y))
#define D_VENC_RC_MIN(x, y) ((x) > (y) ? (y) : (x))

#define D_VENC_RC_MIN3(x, y, z) (((x) < (y)) ? D_VENC_RC_MIN(x, z) : D_VENC_RC_MIN(y, z))
#define D_VENC_RC_MAX3(x, y, z) (((x) > (y)) ? D_VENC_RC_MAX(x, z) : D_VENC_RC_MAX(y, z))
#define D_VENC_RC_MEDIAN(x, y, z) (((x) + (y) + (z) - D_VENC_RC_MAX3(x, y, z)) - D_VENC_RC_MIN3(x, y, z))

#define D_VENC_CALC_PME_SIZE(protocol, pic_width, pic_height) \
        ((D_VENC_ALIGN_UP(pic_width, 64) / 4) * (D_VENC_ALIGN_UP(pic_height, 64) / 4))

#define D_VENC_CALC_PMEINFO_SIZE(protocol, pic_width, pic_height) \
        (((protocol) == VEDU_H265) ? \
        ((((pic_width) + 1023) / 1024) * D_VENC_ALIGN_UP(pic_height, 64) / 64 * 32) : \
        ((((pic_width) + 4095) / 4096) * D_VENC_ALIGN_UP(pic_height, 16) / 16 * 32))

#define D_VENC_CALC_MV_SIZE(protocol, pic_width, pic_height) \
        (((protocol) == VEDU_H265) ? \
        (D_VENC_ALIGN_UP(pic_width, 64) / 64 * D_VENC_ALIGN_UP(pic_height, 64) / 64 * 256) : \
        (D_VENC_ALIGN_UP(pic_width, 16) / 16 * D_VENC_ALIGN_UP(pic_height, 16) / 16 * 32))


hi_bool is_chip_id_v500_r001(hi_chip_type chip_type, hi_chip_version chip_version)
{
    if (((chip_type == CHIP_TYPE_HI3798M) && (chip_version == CHIP_VERSION_V300)) ||
        ((chip_type == CHIP_TYPE_HI3796M) && (chip_version == CHIP_VERSION_V200)) ||
        ((chip_type == CHIP_TYPE_HI3798M) && (chip_version == CHIP_VERSION_V310)) ||
        ((chip_type == CHIP_TYPE_HI3716M) && (chip_version == CHIP_VERSION_V450)) ||
        ((chip_type == CHIP_TYPE_HI3716D) && (chip_version == CHIP_VERSION_V110)) ||
        ((chip_type == CHIP_TYPE_HI3798M_H) && (chip_version == CHIP_VERSION_V200)) ||
        ((chip_type == CHIP_TYPE_HI3798M_H) && (chip_version == CHIP_VERSION_V300)) ||
        ((chip_type == CHIP_TYPE_HI3798C) && (chip_version == CHIP_VERSION_V300)) ||
        ((chip_type == CHIP_TYPE_HI3796C) && (chip_version == CHIP_VERSION_V300))) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_void h264e_put_trailing_bits(tbit_stream *bs)
{
    venc_drv_bs_put_bits31(bs, 1, 1);

    if (bs->total_bits & 7) {       /* 7: bit mask */
        venc_drv_bs_put_bits31(bs, 0, 8 - (bs->total_bits & 7));    /* 8, 7: bit mask */
    }

    if (bs->buff != NULL) {
        *bs->buff++ = (bs->big_endian ? bs->tu32_b : REV32(bs->tu32_b));
    }
}

static hi_u32 get_level_idc_from_mb(hi_u32 total_mb)
{
    hi_u32 level_idc;

    if (total_mb <= 99) {               /* 99: total_mb */
        level_idc = 10;                 /* 10: level_idc */
    } else if (total_mb <= 396) {       /* 396: total_mb */
        level_idc = 20;                 /* 20: level_idc */
    } else if (total_mb <= 792) {       /* 792: total_mb */
        level_idc = 21;                 /* 21: level_idc */
    } else if (total_mb <= 1620) {      /* 1620: total_mb */
        level_idc = 30;                 /* 30: level_idc */
    } else if (total_mb <= 3600) {      /* 3600: total_mb */
        level_idc = 31;                 /* 31: level_idc */
    } else if (total_mb <= 5120) {      /* 5120: total_mb */
        level_idc = 32;                 /* 32: level_idc */
    } else if (total_mb <= 8192) {      /* 8192: total_mb */
        level_idc = 41;                 /* 41: level_idc */
    } else if (total_mb <= 8704) {      /* 8704: total_mb */
        level_idc = 42;                 /* 42: level_idc */
    } else if (total_mb <= 22080) {     /* 22080: total_mb */
        level_idc = 50;                 /* 50: level_idc */
    } else {
        level_idc = 51;                 /* 51: level_idc */
    }

    return level_idc;
}

static void set_bs_profile_and_level(tbit_stream *bs, hi_u32 profile, hi_u32 level)
{
    venc_drv_bs_put_bits32(bs, 1, 32); /* 1, 32: code, bits */

    venc_drv_bs_put_bits31(bs, 0, 1); /* 0, 1: code, bits. forbidden_zero_bit */
    venc_drv_bs_put_bits31(bs, 3, 2); /* 3, 2: code, bits. nal_ref_idc */
    venc_drv_bs_put_bits31(bs, 7, 5); /* 7, 5: code, bits. nal_unit_type */

    venc_drv_bs_put_bits31(bs, profile, 8);    /* 8: bits */
    venc_drv_bs_put_bits31(bs, 0x00, 8);           /* 0x00, 8: code, bits */
    venc_drv_bs_put_bits31(bs, level, 8);      /* 8: bits */

    venc_drv_bs_put_bits31(bs, 1, 1); /* 0, 1: code, bits. ue, sps_id = 0 */

    if (profile == 100) {  /* 100: pro_file_idc. just for high profile */
        venc_drv_bs_put_bits31(bs, 0x2, 3);        /* 0x2, 3: code, bits */
        venc_drv_bs_put_bits31(bs, 0xC, 4);        /* 0xC, 4: code, bits */
    }

    venc_drv_bs_put_bits31(bs, 1, 1); /* 1, 1: code, bits. ue, log2_max_frame_num_minus4 = 0 */

}

static hi_u32 h264e_make_sps(hi_u8 *sps_buf, hi_u32 len, const vedu_efl_h264e_sps *sps)
{
    hi_u32 code, total_mb, profile_idc, level_idc, direct_8x8_interence_flag;
    int bits;

    tbit_stream bs;
    memset_s(&bs, sizeof(tbit_stream), 0, sizeof(tbit_stream));
    venc_drv_bs_open_bit_stream(&bs, (hi_u32 *)sps_buf, len);

    total_mb = sps->frame_width_in_mb * sps->frame_height_in_mb;

    level_idc = get_level_idc_from_mb(total_mb);

    if (total_mb < 1620) {              /* 1620: total_mb */
        direct_8x8_interence_flag = 0;
    } else {
        direct_8x8_interence_flag = 1;
    }

    profile_idc = sps->pro_file_idc;

    set_bs_profile_and_level(&bs, profile_idc, level_idc);

    UE_VLC(bits, code, sps->pic_order_cnt_type);
    venc_drv_bs_put_bits31(&bs, code, bits);
    if (sps->pic_order_cnt_type == 0) {
        UE_VLC(bits, code, 12);         /* 12: bits */
        venc_drv_bs_put_bits31(&bs, code, bits);
    }

    venc_drv_bs_put_bits31(&bs, 3, 3); /* 3, 3: code, bits. ue, num_ref_frames = 1 (or 2) */
    venc_drv_bs_put_bits31(&bs, 0, 1); /* 0, 1: code, bits. u1, gaps_in_frame_num_value_allowed_flag */

    UE_VLC(bits, code, (sps->frame_width_in_mb - 1));
    venc_drv_bs_put_bits31(&bs, code, bits);
    UE_VLC(bits, code, (sps->frame_height_in_mb - 1));
    venc_drv_bs_put_bits31(&bs, code, bits);

    venc_drv_bs_put_bits31(&bs, 1, 1); /* u1, frame_mbs_only_flag = 1 (or 0) */

    venc_drv_bs_put_bits31(&bs, direct_8x8_interence_flag, 1); /* direct_8x8_inference_flag */

    {
        int frame_cropping = ((sps->frame_crop_left | sps->frame_crop_right |
                               sps->frame_crop_top | sps->frame_crop_bottom) != 0);

        venc_drv_bs_put_bits31(&bs, frame_cropping, 1);

        if (frame_cropping) {
            UE_VLC(bits, code, sps->frame_crop_left);
            venc_drv_bs_put_bits31(&bs, code, bits);
            UE_VLC(bits, code, sps->frame_crop_right);
            venc_drv_bs_put_bits31(&bs, code, bits);
            UE_VLC(bits, code, sps->frame_crop_top);
            venc_drv_bs_put_bits31(&bs, code, bits);
            UE_VLC(bits, code, sps->frame_crop_bottom);
            venc_drv_bs_put_bits31(&bs, code, bits);
        }
    }

    venc_drv_bs_put_bits31(&bs, 0, 1); /* vui_parameters_present_flag */
    h264e_put_trailing_bits(&bs);

    HI_INFO_VENC("VENC h264 profile_id:%d,level_id:%d,num_ref_frames: %d\n", profile_idc, level_idc, 1);
    return (hi_u32)bs.total_bits;
}

static hi_u32 h264e_make_pps(hi_u8 *pps_buf, hi_u32 len, const vedu_efl_h264e_pps *pps)
{
    hi_u32 code;
    int bits;
    hi_u32 b = pps->h264_cabac_en ? 1 : 0;
    tbit_stream bs;
    memset_s(&bs, sizeof(tbit_stream), 0, sizeof(tbit_stream));
    venc_drv_bs_open_bit_stream(&bs, (hi_u32 *)pps_buf, len);

    venc_drv_bs_put_bits32(&bs, 1, 32); /* 1, 32: code, bits. */

    venc_drv_bs_put_bits31(&bs, 0, 1); /* 0, 1: code, bits. forbidden_zero_bit */
    venc_drv_bs_put_bits31(&bs, 3, 2); /* 3, 2: code, bits. nal_ref_idc */
    venc_drv_bs_put_bits31(&bs, 8, 5); /* 8, 5: code, bits. nal_unit_type */

    venc_drv_bs_put_bits31(&bs, 1, 1);
    venc_drv_bs_put_bits31(&bs, 1, 1);

    venc_drv_bs_put_bits31(&bs, b, 1); /* 1: bits. entropy_coding_mode_flag = 0 */
    venc_drv_bs_put_bits31(&bs, 0, 1); /* 0, 1: code, bits. pic_order_present_flag */
    venc_drv_bs_put_bits31(&bs, 1, 1); /* 1, 1: code, bits. num_slice_groups_minus1 */
    /* venc_drv_bs_put_bits31(&bs, 1, 1); */ /* num_ref_idx_l0_active_minus1 = 0 (or 1) */
    venc_drv_bs_put_bits31(&bs, 2, 3);  /* 2, 3: code, bits.  */
    venc_drv_bs_put_bits31(&bs, 1, 1); /* 1: num_ref_idx_l1_active_minus1 = 0 */
    venc_drv_bs_put_bits31(&bs, 0, 3); /* 3: bits. weighted_pred_flag & weighted_bipred_idc */
    venc_drv_bs_put_bits31(&bs, 3, 2); /* 3, 2: code, bits. pic_init_qp_minus26 & pic_init_qs_minus26 */

    SE_VLC(bits, code, pps->chr_qp_offset); /* chroma_qp_index_offset */
    venc_drv_bs_put_bits31(&bs, code, bits);

    venc_drv_bs_put_bits31(&bs, 1, 1);                /* deblocking_filter_control_present_flag */
    venc_drv_bs_put_bits31(&bs, pps->const_intra, 1); /* constrained_intra_pred_flag */

    venc_drv_bs_put_bits31(&bs, 0, 1);                /* redundant_pic_cnt_present_flag */

    if (pps->h264_hp_en) {
        venc_drv_bs_put_bits31(&bs, 1, 1); /* transform_8x8_mode_flag */
        venc_drv_bs_put_bits31(&bs, 0, 1); /* pic_scaling_matrix_present_flag */
        SE_VLC(bits, code, pps->chr_qp_offset);
        venc_drv_bs_put_bits31(&bs, code, bits);
    }

    h264e_put_trailing_bits(&bs);
    return (hi_u32)bs.total_bits;
}

static void memory_management_control_operation(tbit_stream *bs, const vedu_efl_h264e_slc_hdr *slc_hdr)
{
    hi_u32 code = 0;
    hi_s32 bits;
    hi_u32 i;

    /* adaptive_ref_pic_marking_mode_flag */
    venc_drv_bs_put_bits31(bs, slc_hdr->mark.adaptive_ref_pic_marking_mode_flag, 1);
    if (slc_hdr->mark.adaptive_ref_pic_marking_mode_flag) {
        i = 0;
        do {
            UE_VLC(bits, code, slc_hdr->mark.memory_management_control_operation[i]);
            venc_drv_bs_put_bits31(bs, code, bits);
            if (slc_hdr->mark.memory_management_control_operation[i] != 0 &&
                slc_hdr->mark.memory_management_control_operation[i] != 5) { /* 5 operateion */
                UE_VLC(bits, code, slc_hdr->mark.marking_operation[i]);
                venc_drv_bs_put_bits31(bs, code, bits);
            }
        } while (slc_hdr->mark.memory_management_control_operation[i++] != 0);
    }
}

static void h264e_set_bs1_from_slc_hdr(tbit_stream *bs1, const vedu_efl_h264e_slc_hdr *slc_hdr,
                                       vedu_efl_enc_para *enc_para)
{
    hi_u32 code = 0;
    hi_s32 bits;

    UE_VLC(bits, code, slc_hdr->slice_type);
    venc_drv_bs_put_bits31(bs1, code, bits);                /* slice_type, 0(P) or 2(I) */

    venc_drv_bs_put_bits31(bs1, 1, 1);                      /* pic_parameter_set_id */
    venc_drv_bs_put_bits31(bs1, slc_hdr->frame_num & 0xF, 4); /* 0xF, 4: bit mask, bits frame number */

    if (slc_hdr->slice_type == 2) { /* 2: slice_type. all I picture be IDR */
        UE_VLC(bits, code, enc_para->idr_pic_id & 0xF);         /* 0xF: bit mask */
        venc_drv_bs_put_bits31(bs1, code, bits);
        enc_para->idr_pic_id++;
    }
    /* num_ref_idx_active_override_flag */
    if (slc_hdr->pic_order_cnt_type == 0) {
        venc_drv_bs_put_bits31(bs1, slc_hdr->pic_order_cnt_lsb & 0xFFFF, 16);      /* 0xFFFF, 16: bit mask, bits */
    }
    if (slc_hdr->slice_type == 1) {
        venc_drv_bs_put_bits31(bs1,  slc_hdr->direct_spatial_mv_pred_flag, 1);
    }

    if (slc_hdr->slice_type != 2) {         /* 2: slice_type */
        venc_drv_bs_put_bits31(bs1, 1, 1);  /* num_ref_idx_active_override_flag */
        UE_VLC(bits, code, slc_hdr->num_ref_idx_l0_active_minus1);
        venc_drv_bs_put_bits31(bs1, code, bits);
        if (slc_hdr->slice_type == 1) {
            UE_VLC(bits, code, slc_hdr->num_ref_idx_l1_active_minus1);
            venc_drv_bs_put_bits31(bs1, code, bits);
        }
    }
}

static void h264e_set_bs_buf(hi_u32 *hal_buf, hi_s32 bits, hi_u32 big_endian, hi_u32 *buf, hi_u32 len)
{
    hi_s32 i;

    for (i = 0; bits > 0; i++, bits -= 32) {    /* 32: bits */
        hal_buf[i] = big_endian ? buf[i] : REV32(buf[i]);
        if (bits < 32) {                        /* 32: bits */
            hal_buf[i] >>= (hi_u32)(32 - bits);     /* 32: bits */
        }
    }
}

static hi_u32 h264e_make_slc_hdr(vedu_efl_enc_para *enc_para, const vedu_efl_h264e_slc_hdr *slc_hdr)
{
    hi_u32 buf[8] = {0};    /* 8: array size */
    hi_u32 bit_para;
    hi_u32 *hdr_buf = enc_para->hal.slc_hdr_stream;
    hi_u32 *reorder_buf = enc_para->hal.reorder_stream;
    hi_u32 *mark_buf = enc_para->hal.marking_stream;
    tbit_stream bs1;
    tbit_stream bs2;
    tbit_stream bs3;

    memset_s(&bs1, sizeof(tbit_stream), 0, sizeof(tbit_stream));
    memset_s(&bs2, sizeof(tbit_stream), 0, sizeof(tbit_stream));
    memset_s(&bs3, sizeof(tbit_stream), 0, sizeof(tbit_stream));

    venc_drv_bs_open_bit_stream(&bs1, buf, 8);  /* 8: array size */

    h264e_set_bs1_from_slc_hdr(&bs1, slc_hdr, enc_para);

    if (bs1.buff != NULL) {
        *bs1.buff++ = (bs1.big_endian ? bs1.tu32_b : REV32(bs1.tu32_b));
    }

    h264e_set_bs_buf(hdr_buf, bs1.total_bits, bs1.big_endian, buf, 8);  /* 8: array size */

    bit_para = bs1.total_bits;

    /* ***** ref_pic_list_reordering() *********************************** */
    venc_drv_bs_open_bit_stream(&bs2, buf, 8);  /* 8: array size */

    /* ref_pic_list_reordering_flag_l0 : 0 */
    venc_drv_bs_put_bits31(&bs2, slc_hdr->ref_pic_list_reordering_flag_l0, 1);
    if (slc_hdr->slice_type == 1) {
        venc_drv_bs_put_bits31(&bs2, slc_hdr->ref_pic_list_reordering_flag_l1, 1);
    }
    if (bs2.buff != NULL) {
        *bs2.buff++ = (bs2.big_endian ? bs2.tu32_b : REV32(bs2.tu32_b));
    }

    h264e_set_bs_buf(reorder_buf, bs2.total_bits, bs2.big_endian, buf, 8);  /* 8: array size */

    bit_para |= bs2.total_bits << 8;            /* 8: shift left */

    /****** dec_ref_pic_marking() *****************************************/
    venc_drv_bs_open_bit_stream(&bs3, buf, 8);  /* 8: array size */

    if (slc_hdr->slice_type == 2) { /* 2: I frame */
        venc_drv_bs_put_bits31(&bs3, slc_hdr->mark.no_output_of_prior_pics_flag, 1); /* no_output_of_prior_pics_flag */
        venc_drv_bs_put_bits31(&bs3, slc_hdr->mark.long_term_reference_flag, 1); /* long_term_reference_flag     */
    } else {
        memory_management_control_operation(&bs3, slc_hdr);
    }

    if (bs3.buff != NULL) {
        *bs3.buff++ = (bs3.big_endian ? bs3.tu32_b : REV32(bs3.tu32_b));
    }

    h264e_set_bs_buf(mark_buf, bs3.total_bits, bs3.big_endian, buf, 8);    /* 8: array size */

    bit_para |= bs3.total_bits << 16; /* 16: shift left */
    return (hi_u32)bit_para;
}

static void h265e_vps_set_bs(tbit_stream *bs, const vedu_efl_h265e_vps *vps)
{
    int i = 0;

    /* profile_tier_level */
    venc_drv_bs_put_bits31(bs, 0, 2);       /* general_profile_space=0  u(2) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_tier_flag=0  u(1) */
    venc_drv_bs_put_bits31(bs, 1, 5);       /* general_profile_idc =1   u(5) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_profile_compatibility_flag[0]  u(1) */
    venc_drv_bs_put_bits31(bs, 1, 1);       /* general_profile_compatibility_flag[1]  u(1) */
    venc_drv_bs_put_bits31(bs, 1, 1);       /* general_profile_compatibility_flag[2]  u(1) */

    for (i = 3; i < 32;i++) {                /* 32: bits */
        venc_drv_bs_put_bits31(bs, 0, 1);   /* general_profile_compatibility_flag[][j]  u(1) */
    }

    /* general_progressive_source_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, vps->general_progressive_source_flag, 1);
    /* general_interlaced_source_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, vps->general_interlaced_source_flag, 1);
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_non_packed_constraint_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_frame_only_constraint_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, 0, 16);      /* general_reserved_zero_44bits[0..15] = 0  u(16) */
    venc_drv_bs_put_bits31(bs, 0, 16);      /* general_reserved_zero_44bits[16..31] = 0  u(16) */
    venc_drv_bs_put_bits31(bs, 0, 12);      /* general_reserved_zero_44bits[32..43] = 0  u(12) */
    venc_drv_bs_put_bits31(bs, 0, 8);       /* general_level_idc=0    u(8) */
}

/* 获取VPS语法信息 */
static hi_u32 h265e_make_vps(hi_u8 *vps_buf, hi_u32 len, const vedu_efl_h265e_vps *vps)
{
    hi_u32 code;
    int    bits;

    int i = 0;
    tbit_stream bs;
    memset_s(&bs, sizeof(tbit_stream), 0, sizeof(tbit_stream));

    venc_drv_bs_open_bit_stream(&bs, (hi_u32 *)vps_buf, len);

    venc_drv_bs_put_bits32(&bs, 1, 32);      /* 32: bits */
    venc_drv_bs_put_bits31(&bs, 0x4001, 16); /* 0x4001, 16: code, bits */

    venc_drv_bs_put_bits31(&bs, 0, 4);       /* 4: bits. video_parameter_set_id = 0  u(4) */
    venc_drv_bs_put_bits31(&bs, 3, 2);       /* 3, 2: code ,bits. vps_reserved_three_2bits = 3  u(2) */
    venc_drv_bs_put_bits31(&bs, 0, 6);       /* 6: bits. vps_max_layers_minus1 = 0   u(6) */
    venc_drv_bs_put_bits31(&bs, 0, 3);       /* 3: bits. */
                                             /* vps_max_sub_layers_minus1 = 0或1 u(3) 开大小P，取值为1；否则取值为0 */
    venc_drv_bs_put_bits31(&bs, 1, 1);       /* vps_temporal_id_nesting_flag = 1 u(1) */
    venc_drv_bs_put_bits31(&bs, 0xffff, 16); /* 0xffff, 16: bits mask, bits. vps_reserved_ffff_16bits = 0xffff u(16) */

    h265e_vps_set_bs(&bs, vps);

    venc_drv_bs_put_bits31(&bs, 1, 1);           /* vps_sub_layer_ordering_info_present_flag = 1  u(1) */

    for (i = 0; i <= 0; i++) {
        UE_VLC(bits, code, vps->vps_max_dec_pic_buffering_minus1[0]);
        venc_drv_bs_put_bits31(&bs, code, bits); /* vps_max_dec_pic_buffering_minus1[ j ]  ue(v), 1 */

        UE_VLC(bits, code, vps->vps_max_num_reorder_pics[0]);
        venc_drv_bs_put_bits31(&bs, code, bits); /* vps_max_num_reorder_pics[ j ]  ue(v), 0 */

        UE_VLC(bits, code, vps->vps_max_latency_increase_plus1[0]);
        venc_drv_bs_put_bits31(&bs, code, bits); /* vps_max_latency_increase_plus1[ j ]  ue(v), 0 */
    }


    venc_drv_bs_put_bits31(&bs, 0, 6);           /* vps_max_layer_id=0  u(6) */

    UE_VLC(bits, code, 0);
    venc_drv_bs_put_bits31(&bs, code, bits);     /* vps_num_layer_sets_minus1=0  ue(v) */

    venc_drv_bs_put_bits31(&bs, 0, 1);           /* vps_timing_info_present_flag = 0  u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* vps_extension_flag=0    u(1) */

    h264e_put_trailing_bits(&bs);

    return (hi_u32)bs.total_bits;
}

static void h265e_sps_set_bs_1(tbit_stream *bs, const vedu_efl_h265e_sps *sps)
{
    hi_u32 code;
    int    bits, i;

    /* profile_tier_level */
    venc_drv_bs_put_bits31(bs, 0, 2);       /* general_profile_space  u(2) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_tier_flag=0  u(1) */
    venc_drv_bs_put_bits31(bs, 1, 5);       /* general_profile_idc=1   u(5) */

    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_profile_compatibility_flag[][0] =1u(1) */
    venc_drv_bs_put_bits31(bs, 1, 1);       /* general_profile_compatibility_flag[][1] =1u(1) */
    venc_drv_bs_put_bits31(bs, 1, 1);       /* general_profile_compatibility_flag[][2] =1u(1) */
    for (i = 3; i < 32; i++) {               /* 32: bits */
        venc_drv_bs_put_bits31(bs, 0, 1);   /* general_profile_compatibility_flag[][j]=0  u(1) */
    }
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_progressive_source_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_interlaced_source_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_non_packed_constraint_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, 0, 1);       /* general_frame_only_constraint_flag = 0  u(1) */
    venc_drv_bs_put_bits31(bs, 0, 16);      /* general_reserved_zero_44bits[0..15] = 0  u(16) */
    venc_drv_bs_put_bits31(bs, 0, 16);      /* general_reserved_zero_44bits[16..31] = 0  u(16) */
    venc_drv_bs_put_bits31(bs, 0, 12);      /* general_reserved_zero_44bits[32..43] = 0  u(12) */
    venc_drv_bs_put_bits31(bs, 0, 8);       /* general_level_idc    u(8) */


    venc_drv_bs_put_bits31(bs, 1, 1);           /* seq_parameter_set_id   ue(v) */

    UE_VLC(bits, code, 1);
    venc_drv_bs_put_bits31(bs, code, bits);     /* chroma_format_idc = 1  ue(v) */

    UE_VLC(bits, code, sps->pic_width_in_luma_samples);
    venc_drv_bs_put_bits31(bs, code, bits);     /* pic_width_in_luma_samples   ue(v) */

    UE_VLC(bits, code, sps->pic_height_in_luma_samples);
    venc_drv_bs_put_bits31(bs, code, bits);     /* pic_height_in_luma_samples  ue(v) */

    venc_drv_bs_put_bits31(bs, sps->pic_cropping_flag, 1);

    if (sps->pic_cropping_flag == 1) {
        UE_VLC(bits, code, sps->pic_crop_left_offset);
        venc_drv_bs_put_bits31(bs, code, bits);

        UE_VLC(bits, code, sps->pic_crop_right_offset);
        venc_drv_bs_put_bits31(bs, code, bits);

        UE_VLC(bits, code, sps->pic_crop_top_offset);
        venc_drv_bs_put_bits31(bs, code, bits);

        UE_VLC(bits, code, sps->pic_crop_bottom_offset);
        venc_drv_bs_put_bits31(bs, code, bits);
    }

    venc_drv_bs_put_bits31(bs, 1, 1);           /* bit_depth_luma_minus8 = 0    ue(v) */
    venc_drv_bs_put_bits31(bs, 1, 1);           /* bit_depth_chroma_minus8 = 0   ue(v) */

    UE_VLC(bits, code, 12);                      /* 12: val */
    venc_drv_bs_put_bits31(bs, code, bits);     /* log2_max_pic_order_cnt_lsb_minus4 =12   ue(v) */

    venc_drv_bs_put_bits31(bs, 1, 1);           /* sps_sub_layer_ordering_info_present_flag = 1   u(1) */
}

static void h265e_sps_set_bs_2(tbit_stream *bs, const vedu_efl_h265e_sps *sps)
{
    hi_u32 code;
    int    bits, i;

    for (i = 0; i <= sps->sps_max_sub_layers_minus1; i++) {
        /* sps_max_dec_pic_buffering_minus1[ i ] = 1    ue(v) */
        UE_VLC(bits, code,  sps->sps_max_dec_pic_buffering_minus1);
        venc_drv_bs_put_bits31(bs, code, bits);
        /* sps_max_num_reorder_pics[ i ]=0      ue(v), */
        UE_VLC(bits, code,  sps->sps_max_num_reorder_pics);
        venc_drv_bs_put_bits31(bs, code, bits);
        /* sps_max_latency_increase_plus1[ i ]=0     ue(v) */
        venc_drv_bs_put_bits31(bs, 1, 1);
    }
    venc_drv_bs_put_bits31(bs, 1, 1);           /* log2_min_luma_coding_block_size_minus3=0     ue(v) */

    UE_VLC(bits, code, 3);                       /* 3: val */
    venc_drv_bs_put_bits31(bs, code, bits);     /* log2_diff_max_min_luma_coding_block_size = 3   ue(v) */

    venc_drv_bs_put_bits31(bs, 1, 1);           /* log2_min_transform_block_size_minus2 =0    ue(v) */

    UE_VLC(bits, code, 3);                       /* 3: val */
    venc_drv_bs_put_bits31(bs, code, bits);     /* log2_diff_max_min_transform_block_size=2   ue(v) */

    /* venc_drv_bs_put_bits31(bs, 1, 1); */     /* max_transform_hierarchy_depth_inter =0   ue(v) */
    UE_VLC(bits, code, 1);
    venc_drv_bs_put_bits31(bs, code, bits);
    venc_drv_bs_put_bits31(bs, 1, 1);           /* max_transform_hierarchy_depth_intra =0   ue(v) */
    venc_drv_bs_put_bits31(bs, 0, 1);           /* scaling_list_enable_flag=0         u(1) */

    venc_drv_bs_put_bits31(bs, 0, 1);           /* amp_enabled_flag =0        u(1) */

    venc_drv_bs_put_bits31(bs, 1, 1);           /* sample_adaptive_offset_enabled_flag =0   u(1) */

    venc_drv_bs_put_bits31(bs, sps->pcm_enable, 1);
    if (sps->pcm_enable == 1) {
        venc_drv_bs_put_bits31(bs, 7, 4);       /* pcm_sample_bit_depth_luma_minus1=7    u(4) */
        venc_drv_bs_put_bits31(bs, 7, 4);       /* pcm_sample_bit_depth_chroma_minus1=7  u(4) */

        UE_VLC(bits, code, (sps->ipcm_log2_min_size - 3));      /* 3: ipcm_log2_min_size */
        venc_drv_bs_put_bits31(bs, code, bits); /* log2_min_pcm_luma_coding_block_size_minus3    ue(v) */

        UE_VLC(bits, code, (sps->ipcm_log2_max_size - sps->ipcm_log2_min_size));
        venc_drv_bs_put_bits31(bs, code, bits); /* log2_diff_max_min_pcm_luma_coding_block_size    ue(v) */

        venc_drv_bs_put_bits31(bs, 0, 1);       /* pcm_loop_filter_disabled_flag    u(1) */
    }
}

/* 获取SPS语法信息 */
static hi_u32 h265e_make_sps(hi_u8 *sps_buf, hi_u32 len, const vedu_efl_h265e_sps *sps)
{
    hi_u32 code;
    int    bits, i, j;

    tbit_stream bs;
    memset_s(&bs, sizeof(tbit_stream), 0, sizeof(tbit_stream));

    venc_drv_bs_open_bit_stream(&bs, (hi_u32 *)sps_buf, len);

    venc_drv_bs_put_bits32(&bs, 1, 32);          /* 32: bits */
    venc_drv_bs_put_bits31(&bs, 0x4201, 16);     /* 0x4201, 16: code, bits */

    venc_drv_bs_put_bits31(&bs, 0, 4);           /* video_parameter_set_id  u(4) */
    venc_drv_bs_put_bits31(&bs, sps->sps_max_sub_layers_minus1, 3);           /* sps_max_sub_layers_minus1=0  u(3) */
    venc_drv_bs_put_bits31(&bs, 1, 1);           /* sps_temporal_id_nesting_flag = 1  u(1) */

    h265e_sps_set_bs_1(&bs, sps);
    h265e_sps_set_bs_2(&bs, sps);

    UE_VLC(bits, code, sps->num_short_term_ref_pic_sets);
    venc_drv_bs_put_bits31(&bs, code, bits);

    for (j = 0; j < sps->num_short_term_ref_pic_sets; j++) {
        if (j > 0) {
            /* inter_ref_pic_set_prediction_flag            u(1) */
            venc_drv_bs_put_bits31(&bs, 0, 1);
        }
        /* num_negative_pics : 0 */
        UE_VLC(bits, code, sps->num_negative_pics[j]);
        venc_drv_bs_put_bits31(&bs, code, bits);

        /* num_positive_pics = 0            ue(v) */
        UE_VLC(bits, code, sps->num_positive_pics[j]);
        venc_drv_bs_put_bits31(&bs, code, bits);
        for (i = 0; i < sps->num_negative_pics[j];i++) {
            /* delta_poc_s0_minus1            ue(v) */
            UE_VLC(bits, code,  sps->delta_poc_s0_minus1[j][i]);
            venc_drv_bs_put_bits31(&bs, code, bits);
            /* used_by_curr_pic_s0_flag            u(1) */
            venc_drv_bs_put_bits31(&bs, sps->used_by_curr_pic_s0_flag[j][i], 1);
        }
        for (i = 0; i < sps->num_positive_pics[j]; i++) {
            /* delta_poc_s1_minus1            ue(v) */
            UE_VLC(bits, code,  sps->delta_poc_s1_minus1[j][i]);
            venc_drv_bs_put_bits31(&bs, code, bits);
            /* used_by_curr_pic_s1_flag            u(1) */
            venc_drv_bs_put_bits31(&bs, sps->used_by_curr_pic_s1_flag[j][i], 1);
        }
    }

    /* long_term_ref_pics_present_flag */
    venc_drv_bs_put_bits31(&bs, sps->long_term_ref_pic_present_flag, 1);

    if (sps->long_term_ref_pic_present_flag == 1) {
        UE_VLC(code, bits, sps->num_long_term_ref_pics_sps);
        venc_drv_bs_put_bits31(&bs, code, bits);
    }

    /* sps_temporal_mvp_enable_flag =1           u(1) */
    venc_drv_bs_put_bits31(&bs, sps->sps_temporal_mvp_enable_flag, 1);
    /* strong_intra_smoothing_enable_flag=1      u(1) */
    venc_drv_bs_put_bits31(&bs, sps->strong_intra_smoothing_flag, 1);

    venc_drv_bs_put_bits31(&bs, 0, 1);           /* vui_parameters_present_flag =0        u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* sps_extension_flag = 0                u(1) */

    h264e_put_trailing_bits(&bs);

    return (hi_u32)bs.total_bits;
}

static void h265e_pps_set_bs(tbit_stream *bs, const vedu_efl_h265e_pps *pps)
{
    hi_u32 code;
    int    bits;

    venc_drv_bs_put_bits31(bs, 1, 1);           /* pic_parameter_set_id=0   ue(v) */
    venc_drv_bs_put_bits31(bs, 1, 1);           /* seq_parameter_set_id=0   ue(v) */
    venc_drv_bs_put_bits31(bs, 0, 1);           /* dependent_slice_segments_enabled_flag=0        u(1) */
    venc_drv_bs_put_bits31(bs, 0, 1);           /* output_flag_present_flag=0        u(1) */
    venc_drv_bs_put_bits31(bs, 0, 3);           /* num_extra_slice_header_bits = 0     u(3) */
    venc_drv_bs_put_bits31(bs, 0, 1);           /* sign_data_hiding_flag = 0   u(1) */
    venc_drv_bs_put_bits31(bs, 1, 1);           /* cabac_init_present_flag=0    u(1) */
    venc_drv_bs_put_bits31(bs, 1, 1);           /* num_ref_idx_l0_default_active_minus1 =0     ue(v) */
    venc_drv_bs_put_bits31(bs, 1, 1);           /* num_ref_idx_l1_default_active_minus1=0      ue(v) */

    SE_VLC(bits, code, 0);
    venc_drv_bs_put_bits31(bs, code, bits);     /* pic_init_qp_minus26=0                       se(v) */

    venc_drv_bs_put_bits31(bs, 0, 1);           /* constrained_intra_pred_flag =0              u(1) */
    venc_drv_bs_put_bits31(bs, 0, 1);           /* transform_skip_enabled_flag=0               u(1) */

    venc_drv_bs_put_bits31(bs, pps->cu_qp_delta_enable, bits);    /* cu_qp_delta_enabled_flag               u(1) */
    if (pps->cu_qp_delta_enable == 1) {
        UE_VLC(bits, code, 2);                   /* 2: bits */
        venc_drv_bs_put_bits31(bs, code, bits); /* diff_cu_qp_delta_depth                ue(v) */
    }
}

/* Get PPS syntax information */
/* CNcomment: 获取PPS语法信息 */
static hi_u32 h265e_make_pps(hi_u8 *pps_buf, hi_u32 len, const vedu_efl_h265e_pps *pps)
{
    hi_u32 code;
    int    bits;

    tbit_stream bs;
    memset_s(&bs, sizeof(tbit_stream), 0, sizeof(tbit_stream));

    venc_drv_bs_open_bit_stream(&bs, (hi_u32 *)pps_buf, len);

    venc_drv_bs_put_bits32(&bs, 1, 32);          /* 32: bits */
    venc_drv_bs_put_bits31(&bs, 0x4401, 16);     /* 0x4401, 16: code, bits */

    h265e_pps_set_bs(&bs, pps);

    SE_VLC(bits, code, pps->i_cb_qp_offset);
    venc_drv_bs_put_bits31(&bs, code, bits);     /* pic_cb_qp_offset                    se(v) */

    SE_VLC(bits, code, pps->i_cr_qp_offset);
    venc_drv_bs_put_bits31(&bs, code, bits);     /* pic_cr_qp_offset                    se(v) */

    venc_drv_bs_put_bits31(&bs, 0, 1);           /* pic_slice_chroma_qp_offsets_present_flag = 0   u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* weighted_pred_flag = 0        u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* weighted_bipred_flag = 0        u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* transquant_bypass_enable_flag   u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* tiles_enabled_flag = 0             u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* entropy_coding_sync_enabled_flag    u(1) */
    venc_drv_bs_put_bits31(&bs, 1, 1);           /* loop_filter_across_slices_enabled_flag    u(1) */
    venc_drv_bs_put_bits31(&bs, 1, 1);           /* deblocking_filter_control_present_flag = 1   u(1) */
    venc_drv_bs_put_bits31(&bs, 1, 1);           /* deblocking_filter_override_enabled_flag   u(1) */
    venc_drv_bs_put_bits31(&bs, pps->pic_disable_db_filter, 1);        /* pic_disable_deblocking_filter_flag   u(1) */
    if (pps->pic_disable_db_filter == 0) {
        SE_VLC(bits, code, 0);
        venc_drv_bs_put_bits31(&bs, code, bits); /* pps_beta_offset_div2    se(v) */

        SE_VLC(bits, code, 0);
        venc_drv_bs_put_bits31(&bs, code, bits); /* pps_tc_offset_div2       se(v) */
    }

    venc_drv_bs_put_bits31(&bs, 0, 1);           /* pic_scaling_list_data_present_flag    u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* lists_modification_present_flag = 0   u(1) */
    venc_drv_bs_put_bits31(&bs, 1, 1);           /* log2_parallel_merge_level_minus2=0    ue(v) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* slice_segment_header_extension_present_flag=0     u(1) */
    venc_drv_bs_put_bits31(&bs, 0, 1);           /* pps_extension_flag    u(1) */

    h264e_put_trailing_bits(&bs);

    return (hi_u32)bs.total_bits;
}

static hi_void h265e_make_slice_head_dbsao(vedu_efl_enc_para *enc_para, const vedu_efl_h265e_slc_hdr *slc_hdr,
                                           tbit_stream *bs)
{
    hi_u32 code, sao_enabled, dbf_enabled;
    int    bits;

    if (slc_hdr->db_filter_ctl_present == 1) {
        if (slc_hdr->db_filter_override_enabled == 1) {
            /* deblocking_filter_override_flag        u(1) */
            venc_drv_bs_put_bits31(bs, slc_hdr->df_override_flag, 1);
        }
        if (slc_hdr->df_override_flag == 1) {
            /* slice_disable_deblocking_filter_flag     u(1) */
            venc_drv_bs_put_bits31(bs, slc_hdr->slice_header_disable_df, 1);
            if (slc_hdr->slice_header_disable_df == 0) {
                SE_VLC(bits, code, 0);
                venc_drv_bs_put_bits31(bs, code, bits);                     /* beta_offset_div2        se(v) */

                SE_VLC(bits, code, 0);
                venc_drv_bs_put_bits31(bs, code, bits);                     /* tc_offset_div2          se(v) */
            }
        }
    }
    sao_enabled = (slc_hdr->sao_enabled_flag) == 0 ? 0 : (slc_hdr->slice_sao_luma || slc_hdr->slice_sao_chroma);
    dbf_enabled = (slc_hdr->slice_header_disable_df == 0) ? 1 : 0;

    if (sao_enabled == 1 || dbf_enabled == 1 || slc_hdr->slice_header_disable_df == 0) {
        /* slice_loop_filter_across_slices_enabled_flag     u(1) */
        venc_drv_bs_put_bits31(bs, slc_hdr->slice_lf_across_slice, 1);
    }

    return;
}

static void h265e_slice_set_bs2_ref(tbit_stream *bs2, const vedu_efl_h265e_slc_hdr *slc_hdr,
                                    vedu_efl_enc_para *enc_para)
{
    hi_u32 code;
    int    bits, i;

    if (enc_para->num_short_term_ref_pic_sets > 1) {
        /* i=enc_para->ui_num_short_term_ref_pic_sets>2?2:1; */
        i = enc_para->num_short_term_ref_pic_sets > 2 ?     /* 2: num_short_term_ref_pic_sets */
            (enc_para->num_short_term_ref_pic_sets > 4 ? 3 : 2) : 1;    /* 4, 3, 2: num_short_term_ref_pic_sets */
        venc_drv_bs_put_bits31(bs2, slc_hdr->short_term_ref_pic_set_idx, i);
    }

    if (enc_para->long_term_ref_pic_present_flag == 1) {
        UE_VLC(bits, code, slc_hdr->num_long_term_pics);
        venc_drv_bs_put_bits31(bs2, code, bits);

        for (i = 0; i < slc_hdr->num_long_term_pics; i++) {
            venc_drv_bs_put_bits31(bs2, slc_hdr->poc_lsb_lt[i], slc_hdr->slice_poc_lsb_bits);
            venc_drv_bs_put_bits31(bs2, slc_hdr->used_by_curr_pic_lt_flag[i], 1);

            venc_drv_bs_put_bits31(bs2, slc_hdr->delta_poc_msb_present_flag[i], 1);
            if (slc_hdr->delta_poc_msb_present_flag[i]) {
                UE_VLC(bits, code, slc_hdr->delta_poc_msb_cycle_lt[i]);
                venc_drv_bs_put_bits31(bs2, code, bits);
            }
        }
    }
}

static void h265e_slice_set_bs2(tbit_stream *bs2, const vedu_efl_h265e_slc_hdr *slc_hdr, vedu_efl_enc_para *enc_para)
{
    hi_u32 code;
    int    bits;

    UE_VLC(bits, code, slc_hdr->slice_type);
    venc_drv_bs_put_bits31(bs2, code, bits);                     /* slice_type    ue(v) */

    if (slc_hdr->slice_type != 2) {         /* 2: slice_type */
        venc_drv_bs_put_bits31(bs2, slc_hdr->poc_lsb, 16);      /* pic_order_cnt_lsb         u(16) */
        venc_drv_bs_put_bits31(bs2, 1, 1);                       /* short_term_ref_pic_set_sps_flag=1     u(1) */

        h265e_slice_set_bs2_ref(bs2, slc_hdr, enc_para);

        if (slc_hdr->sps_temporal_mvp_enable_flag) {
            venc_drv_bs_put_bits31(bs2, 1, 1);                   /* enable_temporal_mvp_flag=1        u(1) */
        }
    }

    venc_drv_bs_put_bits31(bs2, slc_hdr->slice_sao_luma, 1);      /* slice_sao_luma_flag                  u(1) */
    venc_drv_bs_put_bits31(bs2, slc_hdr->slice_sao_chroma, 1);    /* slice_sao_chroma_flag                u(1) */

    if (slc_hdr->slice_type != 2) {     /* 2: slice_type */
        venc_drv_bs_put_bits31(bs2, 1, 1);                       /* num_ref_idx_active_override_flag = 1    u(1) */

        UE_VLC(bits, code,  slc_hdr->num_ref_idx_l0_active_minus1);
        venc_drv_bs_put_bits31(bs2, code, bits);                 /* num_ref_idx_l0_active_minus1 */

        if (slc_hdr->slice_type == 0) {
            UE_VLC(bits, code, slc_hdr->num_ref_idx_l1_active_minus1);
            venc_drv_bs_put_bits31(bs2, code, bits);             /* num_ref_idx_l1_active_minus1 */
        }

        if (slc_hdr->slice_type == 0) {
            /* mvd_l1_zero_flag */
            venc_drv_bs_put_bits31(bs2, slc_hdr->mvd_l1_zero_flag, 1);
        }

        venc_drv_bs_put_bits31(bs2, 0, 1);                       /* cabac_init_flag */

        if (slc_hdr->sps_temporal_mvp_enable_flag) {
            if (slc_hdr->slice_type == 0) {
                venc_drv_bs_put_bits31(bs2, slc_hdr->collocated_from_l0_flag, 1);
            }

            if (((slc_hdr->collocated_from_l0_flag == 1) && (slc_hdr->num_ref_idx_l0_active_minus1 > 0))
            || ((slc_hdr->collocated_from_l0_flag == 0)  && (slc_hdr->num_ref_idx_l1_active_minus1 > 0))) {
                UE_VLC(bits, code, slc_hdr->collocated_ref_idx);
                venc_drv_bs_put_bits31(bs2, code, bits);
            }
        }

        UE_VLC(bits, code, slc_hdr->five_minus_max_num_merge_cand);
        venc_drv_bs_put_bits31(bs2, code, bits);          /* five_minus_max_num_merge_cand=3         ue(v) */

    }

    SE_VLC(bits, code, slc_hdr->i_slice_qp_delta);
    venc_drv_bs_put_bits31(bs2, code, bits);              /* slice_qp_delta     se(v) */
}

static hi_u32 h265e_make_slice_head(vedu_efl_enc_para *enc_para, const vedu_efl_h265e_slc_hdr *slc_hdr)
{
    hi_u32 buf[8] = {0};        /* 8: array size */
    int    bits, i;

    tbit_stream bs1 ;
    tbit_stream bs2 ;

    memset_s(buf, sizeof(buf), 0, sizeof(hi_u32) * 8);      /* 8: buf size */
    memset_s(&bs1, sizeof(tbit_stream), 0, sizeof(tbit_stream));
    memset_s(&bs2, sizeof(tbit_stream), 0, sizeof(tbit_stream));

    /* part1 */
    venc_drv_bs_open_bit_stream(&bs1, buf, 8);  /* 8: array size */

    if (slc_hdr->slice_type == 2) { /* H265E_I_SLICE */
        venc_drv_bs_put_bits31(&bs1, 0, 1);       /* no_output_of_prior_pics_flag = 0    u(1) */
    }

    venc_drv_bs_put_bits31(&bs1, 1, 1);           /* slice_pic_parameter_set_id=0         ue(v) */

    if (bs1.buff != NULL) {
        *bs1.buff++ = (bs1.big_endian ? bs1.tu32_b : REV32(bs1.tu32_b));
    }

    bits = bs1.total_bits;

    for (i = 0; bits > 0; i++, bits -= 32) {        /* 32: bits */
        enc_para->hal.slc_hdr_part1 = bs1.big_endian ? buf[i] : REV32(buf[i]);
        if (bits < 32) { enc_para->hal.slc_hdr_part1 >>= (hi_u32)(32 - bits); }     /* 32: bits */
    }
    enc_para->hal.slc_hdr_size_part1 = bs1.total_bits;

    /* part2 */
    venc_drv_bs_open_bit_stream(&bs2, buf, 8);  /* 8: array size */

    h265e_slice_set_bs2(&bs2, slc_hdr, enc_para);

    h265e_make_slice_head_dbsao(enc_para, slc_hdr, &bs2);

    if (bs2.buff != NULL) {
        *bs2.buff++ = (bs2.big_endian ? bs2.tu32_b : REV32(bs2.tu32_b));
    }

    bits = bs2.total_bits;

    for (i = 0; bits > 0; i++, bits -= 32) {        /* 32: bits */
        enc_para->hal.cabac_slc_hdr_part2_seg[i] = bs2.big_endian ? buf[i] : REV32(buf[i]);
        if (bits < 32) {                /* 32: bits */
            enc_para->hal.cabac_slc_hdr_part2_seg[i] >>= (32 - (hi_u32)bits); /* 32: bits */
        }
    }
    enc_para->hal.slc_hdr_size_part2 = bs2.total_bits;

    return (hi_u32)bs2.total_bits;
}


static hi_u32 check_prevention_three_byte(hi_u8 *pc_stream, hi_u32 lenth_of_pc, hi_u32 i_num,
    hi_u8 *temp_stream, hi_u32 lenth_of_temp)
{
    hi_u32 i_insert_bytes = 0;
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_u32 inum = i_num / 8;    /* 8: i_num */

    if (inum < 5) {             /* 5: i_num */
        return i_insert_bytes;
       /* return; */
    }
    for (i = 0; i < 6 && j < lenth_of_temp && i < lenth_of_pc; i++, j++) {  /* 6: array size */
        temp_stream[j] = pc_stream[i];

    }
    for (i = 6, j = 6;      /* 6: array size */
         i <= (inum - 3) && i < (lenth_of_pc - 3) && j < (lenth_of_temp - 4); i++) { /* 3, 4: inum, lenth_of_temp */
        temp_stream[j] = pc_stream[i];
        temp_stream[j + 1] = pc_stream[i + 1];

        if ((pc_stream[i] != 0) || (pc_stream[i + 1] != 0)) {
            temp_stream[j + 2] = pc_stream[i + 2];      /* 2: pc_stream index */
            j++;
            continue;
        }
        if ((pc_stream[i + 2] == 0) || (pc_stream[i + 2] == 1) ||     /* 2: pc_stream index */
            (pc_stream[i + 2] == 2) || (pc_stream[i + 2] == 3)) {     /* 2, 3: pc_stream */
            temp_stream[j + 2] = 0x03;                      /* 2, 0x03: temp_stream */
            i_insert_bytes++;


            if ((i == (inum - 3)) || (i == (inum - 4))) {   /* 3, 4: inum */
                temp_stream[j + 3] = pc_stream[i + 2];      /* 3, 2: pc_stream */
                temp_stream[j + 4] = pc_stream[i + 3];      /* 4, 3: pc_stream */
                i += 1;
            } else {
                i += 1;
                j += 3;                 /* 3: index */
            }
        } else {
            temp_stream[j + 2] = pc_stream[i + 2];  /* 2: pc_stream index */
            j++;
        }
    }

    return i_insert_bytes;
}

hi_void set_stream_info(vedu_efl_enc_para *enc_para, strm_mng_stream_info *stream_info)
{
    memset_s(stream_info, sizeof(strm_mng_stream_info), 0, sizeof(strm_mng_stream_info));

    stream_info->para_set_len = enc_para->hal.para_set_len;
    stream_info->para_set_real_len = enc_para->hal.para_set_real_len;
    stream_info->para_set_array = enc_para->hal.para_set_array;

    stream_info->strm_buf_rp_vir_addr = enc_para->strm_buf_rp_vir_addr;
    stream_info->strm_buf_wp_vir_addr = enc_para->strm_buf_wp_vir_addr;;

    stream_info->stream_buf_addr_array = enc_para->hal.vedu_strm_addr;
    stream_info->stream_buf_len_array = enc_para->hal.vedu_strm_buf_len;

    stream_info->strm_buf_size = enc_para->hal.strm_buf_size;
    stream_info->cur_strm_buf_size = enc_para->cur_strm_buf_size;
    stream_info->stream_total_byte = enc_para->stat.stream_total_byte;

    stream_info->stream_mmz_buf = enc_para->stream_mmz_buf;
    stream_info->slice_buf = enc_para->hal.slice_buf;
    stream_info->buf = enc_para->buf;

    stream_info->slice_idx = enc_para->hal.slice_idx;
    stream_info->too_few_buffer_skip = enc_para->stat.too_few_buffer_skip;
    stream_info->first_slc_one_frm = enc_para->hal.first_slc_one_frm;
    stream_info->protocol = enc_para->protocol;
#ifdef VENC_SUPPORT_JPGE
    stream_info->jpeg_mmz_buf = &enc_para->jpg_mmz_buf;
#endif
    stream_info->i_frm_insert_by_save_stream = enc_para->i_frm_insert_by_save_strm;
}

static hi_void set_strm_mng_input_info(vedu_efl_enc_para *enc_para, strm_mng_input_info *input_info)
{
    hi_u32 i = 0;

    memset_s(input_info, sizeof(strm_mng_input_info), 0, sizeof(strm_mng_input_info));

    input_info->prepend_sps_pps_enable = enc_para->prepend_sps_pps_enable;
    input_info->vps_bits = enc_para->vps_bits;
    input_info->pps_bits = enc_para->pps_bits;
    input_info->sps_bits = enc_para->sps_bits;

    input_info->vps_stream = enc_para->vps_stream;
    input_info->sps_stream = enc_para->sps_stream;
    input_info->pps_stream = enc_para->pps_stream;

    for (i = 0; i < 16; i++) {      /* 16: index */
        input_info->slice_length[i] = enc_para->hal.slice_length[i];
        input_info->slice_is_end[i] = enc_para->hal.slice_is_end[i];
    }

    input_info->slc_split_en = enc_para->hal.slc_split_en;
    input_info->protocol = enc_para->protocol;
    input_info->is_intra_pic = enc_para->rc.is_intra_pic;
    input_info->pts0 = enc_para->hal.pts0;
    input_info->pts1 = enc_para->hal.pts1;
    input_info->ext_flag = enc_para->hal.ext_flag;
    input_info->ext_fill_len = enc_para->hal.ext_fill_len;
    input_info->target_bits = enc_para->rc.target_bits;
    input_info->get_frame_num_ok = enc_para->stat.get_frame_num_ok;
    input_info->venc_buf_full = enc_para->rc.venc_buf_full;
    input_info->venc_pbit_overflow = enc_para->rc.venc_p_bit_overflow;
    input_info->low_dly_mod = enc_para->rc.low_delay_mode;
    input_info->recode_cnt = enc_para->rc.recode_cnt;
    input_info->time_out = enc_para->time_out;
#ifdef VENC_SUPPORT_JPGE
    input_info->jfif_hdr = enc_para->jfif_hdr;
#endif
}

static hi_void get_stream_info(vedu_efl_enc_para *enc_para, strm_mng_stream_info *stream_info)
{
    enc_para->hal.para_set_len = stream_info->para_set_len;
    enc_para->hal.para_set_real_len = stream_info->para_set_real_len;

    enc_para->strm_buf_rp_vir_addr = stream_info->strm_buf_rp_vir_addr;
    enc_para->strm_buf_wp_vir_addr = stream_info->strm_buf_wp_vir_addr;

    enc_para->cur_strm_buf_size = stream_info->cur_strm_buf_size;
    enc_para->stat.stream_total_byte = stream_info->stream_total_byte;

    enc_para->hal.slice_idx = stream_info->slice_idx;

    enc_para->stat.too_few_buffer_skip = stream_info->too_few_buffer_skip;
    enc_para->hal.first_slc_one_frm = stream_info->first_slc_one_frm;

    enc_para->i_frm_insert_by_save_strm = stream_info->i_frm_insert_by_save_stream;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
#ifdef VENC_SUPPORT_JPGE
static hi_void jpge_make_q_table(hi_s32 q_level, hi_u8 *luma_qt, hi_u32 yqt_len, hi_u8 *chroma_qt, hi_u32 cqt_len)
{
    hi_s32 i = 0;
    hi_s32 lq = 0;
    hi_s32 cq = 0;
    hi_s32 factor = q_level;
    hi_s32 q = q_level;

    if (q < 1) {
        factor = 1;
    }
    if (q > 99) {       /* 99: factor */
        factor = 99;    /* 99: factor */
    }

    if (q < 50) {       /* 50: factor */
        q = 5000 / factor;  /* 5000: factor */
    } else {
        q = 200 - factor * 2;   /* 200, 2: factor */
    }

    /* calculate the new quantizer */
    for (i = 0; i < 64 && i < yqt_len && i < cqt_len; i++) {    /* 64: index */
        lq = (g_jpge_yqt[i] * q + 50) / 100;            /* 50, 100: lq */
        cq = (g_jpge_cqt[i] * q + 50) / 100;            /* 50, 100: lq */

        /* limit the quantizers to 1 <= q <= 255 */
        if (lq < 1) {
            lq = 1;
        } else if (lq > 255) {      /* 255: lq */
            lq = 255;               /* 255: lq */
        }
        luma_qt[i] = lq;

        if (cq < 1) {
            cq = 1;
        } else if (cq > 255) {      /* 255: cq */
            cq = 255;               /* 255: cq */
        }
        chroma_qt[i] = cq;
    }
}

static hi_void jpge_make_jfif(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;
    /* make JFIF header bitstream */
    hi_u32 w = enc_para->rc.pic_width;
    hi_u32 h = enc_para->rc.pic_height;
    hi_u32 t;
    hi_u32 i;
    strm_mng_input_info input_info;
    strm_mng_stream_info stream_info;

    hi_u8  dri[] = {0xff, 0xdd, 0x00, 0x04, 0x00, 0x64};
    hi_u8  sos[] = {0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00};

    for (i = 0; i < 698; i++) { enc_para->jfif_hdr[i] = g_jpge_jfif_hdr[i]; }   /* 698: array size */

    /* 420 422 or 444 */
    if     (enc_para->yuv_sample_type == JPGE_YUV420) { enc_para->jfif_hdr[238] = 0x22; }   /* 238: index */
    else if (enc_para->yuv_sample_type == JPGE_YUV422) { enc_para->jfif_hdr[238] = 0x21; }  /* 238: index */
    else if (enc_para->yuv_sample_type == JPGE_YUV444) { enc_para->jfif_hdr[238] = 0x11; }  /* 238: index */

    if (enc_para->rotation_angle == JPGE_ROTATION_90 ||
         enc_para->rotation_angle == JPGE_ROTATION_270) {
        if (enc_para->yuv_sample_type == JPGE_YUV422) /* 90 or 270 @ 422 */
        { enc_para->jfif_hdr[238] = 0x22; }       /* 238, 0x22: index, jfif_hdr. change 422 to 420 */
        t = w;
        w = h;
        h = t;
    }

    /* img size */
    enc_para->jfif_hdr[232] = h >> 8;       /* 232, 8: index, shift left */
    enc_para->jfif_hdr[233] = h & 0xFF;     /* 233, 0xFF: index, bit mask */
    enc_para->jfif_hdr[234] = w >> 8;       /* 234, 8: index, shift left */
    enc_para->jfif_hdr[235] = w & 0xFF;     /* 235, 0xFF: index, bit mask */

    /* DRI & SOS */
    t = 678;        /* 678: index */

    if (enc_para->hal.slc_split_en) {
        dri[4] = enc_para->hal.split_size >> 8;     /* 4, 8: index, shift left */
        dri[5] = enc_para->hal.split_size & 0xFF;   /* 5, 0xFF: index, bit mask */

        for (i = 0; i < 6; i++, t++) { enc_para->jfif_hdr[t] = dri[i]; }    /* 6: array size */
    }

    for (i = 0; i < 14; i++, t++) { enc_para->jfif_hdr[t] = sos[i]; }       /* 14: array size */

    /* DQT */
    jpge_make_q_table(enc_para->q_level, enc_para->jpge_yqt,
                      sizeof(enc_para->jpge_yqt) / sizeof(enc_para->jpge_yqt[0]),
                      enc_para->jpge_cqt, sizeof(enc_para->jpge_cqt) / sizeof(enc_para->jpge_cqt[0]));

    for (i = 0; i < 64; i++) {      /* 64: array size */
        t = g_zig_zag_scan[i];
        enc_para->jfif_hdr[t +  25] = enc_para->jpge_yqt[i];        /* 25: index */
        enc_para->jfif_hdr[t +  94] = enc_para->jpge_cqt[i];        /* 94: index */
        enc_para->jfif_hdr[t + 163] = enc_para->jpge_cqt[i];        /* 163: index */
    }

    set_stream_info(enc_para, &stream_info);
    set_strm_mng_input_info(enc_para, &input_info);
    if (strm_jpge_write_header(enc_para->strm_mng, &input_info, &stream_info) != HI_SUCCESS) {
        HI_ERR_VENC("JPGE write header failed!");
    }

    return;
}

static hi_void jpge_remove_jfif(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;
    strm_mng_input_info input_info;
    strm_mng_stream_info stream_info;

    set_stream_info(enc_para, &stream_info);
    set_strm_mng_input_info(enc_para, &input_info);

    if (strm_jpge_remove_header(enc_para->strm_mng, &input_info, &stream_info) != HI_SUCCESS) {
        HI_WARN_VENC("JPGE remove header failed!");
    }

    return;
}
#endif

static hi_void convert_pix_format_yuv_store_type_1(hi_drv_pixel_format old_format, hi_u32 *store_type, hi_u32 *find)
{
    switch (old_format) {
        case HI_DRV_PIXEL_FMT_NV61_2X1:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_NV21:
        case HI_DRV_PIXEL_FMT_NV12:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_NV80:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_NV12_411:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_NV61:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_NV42:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_UYVY:
            *store_type = VENC_STORE_PACKAGE;
            break;

        case HI_DRV_PIXEL_FMT_YUYV:
            *store_type = VENC_STORE_PACKAGE;
            break;

        default:
            *find = 0;
            break;
    }
}

static hi_void convert_pix_format_yuv_store_type_2(hi_drv_pixel_format old_format, hi_u32 *store_type)
{
    switch (old_format) {
        case HI_DRV_PIXEL_FMT_YVYU:
            *store_type = VENC_STORE_PACKAGE;
            break;

        case HI_DRV_PIXEL_FMT_YUV400:
            *store_type = VENC_STORE_PLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_YUV411:
            *store_type = VENC_STORE_PLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_YUV420P:
            *store_type = VENC_STORE_PLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_1X2:
            *store_type = VENC_STORE_PLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_2X1:
            *store_type = VENC_STORE_PLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_YUV_444:
            *store_type = VENC_STORE_PLANNAR;
            break;

        case HI_DRV_PIXEL_FMT_YUV410P:
            *store_type = VENC_STORE_PLANNAR;
            break;

        default:
            *store_type = VENC_STORE_SEMIPLANNAR;
            break;
    }
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     : flag :0 -> yuv_store_type; 1 -> yuv_sample_type; 2 -> package_sel
******************************************************************************/
static hi_u32 convert_pix_format_yuv_store_type(hi_drv_pixel_format old_format)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u32 find = 1;

    convert_pix_format_yuv_store_type_1(old_format, &ret, &find);

    if (find == 0) {
        convert_pix_format_yuv_store_type_2(old_format, &ret);
    }

    return ret;
}

static hi_void convert_pix_format_yuv_sample_type_1(hi_drv_pixel_format old_format, hi_u32 *store_type, hi_u32 *find)
{
    switch (old_format) {
        case HI_DRV_PIXEL_FMT_NV61_2X1:
            *store_type = VENC_YUV_422;
            break;

        case HI_DRV_PIXEL_FMT_NV21:
        case HI_DRV_PIXEL_FMT_NV12:
            *store_type = VENC_YUV_420;
            break;

        case HI_DRV_PIXEL_FMT_NV80:     /* 400 */
            *store_type = VENC_YUV_NONE;
            break;

        case HI_DRV_PIXEL_FMT_NV12_411:
            *store_type = VENC_YUV_NONE;
            break;

        case HI_DRV_PIXEL_FMT_NV61:
        case HI_DRV_PIXEL_FMT_NV16:
            *store_type = VENC_YUV_422;
            break;

        case HI_DRV_PIXEL_FMT_NV42:
            *store_type = VENC_YUV_444;
            break;

        case HI_DRV_PIXEL_FMT_UYVY:
        case HI_DRV_PIXEL_FMT_YUYV:
        case HI_DRV_PIXEL_FMT_YVYU:
            *store_type = VENC_YUV_422;
            break;

        default:
            *find = 0;
            break;
    }
}

static hi_void convert_pix_format_yuv_sample_type_2(hi_drv_pixel_format old_format, hi_u32 *store_type)
{
    switch (old_format) {
        case HI_DRV_PIXEL_FMT_YUV400:
            *store_type = VENC_YUV_NONE;
            break;

        case HI_DRV_PIXEL_FMT_YUV411:
            *store_type = VENC_YUV_NONE;
            break;

        case HI_DRV_PIXEL_FMT_YUV420P:
            *store_type = VENC_YUV_420;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_1X2:
            *store_type = VENC_YUV_422;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_2X1:
            *store_type = VENC_YUV_422;
            break;

        case HI_DRV_PIXEL_FMT_YUV_444:
            *store_type = VENC_YUV_444;
            break;

        case HI_DRV_PIXEL_FMT_YUV410P:
            *store_type = VENC_YUV_NONE;
            break;

        default:
            *store_type = VENC_YUV_NONE;
            break;
    }
}

static hi_u32 convert_pix_format_yuv_sample_type(hi_drv_pixel_format old_format)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u32 find = 1;

    convert_pix_format_yuv_sample_type_1(old_format, &ret, &find);

    if (find == 0) {
        convert_pix_format_yuv_sample_type_2(old_format, &ret);
    }

    return ret;
}

static hi_void convert_pix_format_package_sel_1(hi_drv_pixel_format old_format, hi_u32 *store_type, hi_u32 *find)
{
    switch (old_format) {
        case HI_DRV_PIXEL_FMT_NV61_2X1:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_NV21:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_NV12:
            *store_type = VENC_U_V;
            break;

        case HI_DRV_PIXEL_FMT_NV80:     /* 400 */
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_NV12_411:
            *store_type = VENC_U_V;
            break;

        case HI_DRV_PIXEL_FMT_NV61:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_NV16:
            *store_type = VENC_U_V;
            break;

        case HI_DRV_PIXEL_FMT_NV42:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_UYVY:
            *store_type = VENC_PACKAGE_UY0VY1;
            break;

        case HI_DRV_PIXEL_FMT_YUYV:
            *store_type = VENC_PACKAGE_Y0UY1V;
            break;

        default:
            *find = 0;
            break;
    }
}

static hi_void convert_pix_format_package_sel_2(hi_drv_pixel_format old_format, hi_u32 *store_type)
{
    switch (old_format) {
        case HI_DRV_PIXEL_FMT_YVYU:
            *store_type = VENC_PACKAGE_Y0VY1U;
            break;

        case HI_DRV_PIXEL_FMT_YUV400:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_YUV411:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_YUV420P:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_1X2:
            *store_type = VENC_U_V;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_2X1:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_YUV_444:
            *store_type = VENC_V_U;
            break;

        case HI_DRV_PIXEL_FMT_YUV410P:
            *store_type = VENC_V_U;
            break;

        default:
            *store_type = VENC_YUV_NONE;
            break;
    }
}

static hi_u32 convert_pix_format_package_sel(hi_drv_pixel_format old_format)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u32 find = 1;

    convert_pix_format_package_sel_1(old_format, &ret, &find);

    if (find == 0) {
        convert_pix_format_package_sel_2(old_format, &ret);
    }

    return ret;
}

static hi_u32 convert_pix_format_frame_info(hi_drv_pixel_format old_format)
{
    hi_u32 ret = HI_SUCCESS;

    switch (old_format) {
        case HI_DRV_PIXEL_FMT_NV21:
            ret = VENC_SEMIPLANNAR_420_VU;
            break;

        case HI_DRV_PIXEL_FMT_NV12:
            ret = VENC_SEMIPLANNAR_420_UV;
            break;

        case HI_DRV_PIXEL_FMT_UYVY:
            ret = VENC_PACKAGE_422_UYVY;
            break;

        case HI_DRV_PIXEL_FMT_YUYV:
            ret = VENC_PACKAGE_422_YUYV;
            break;

        case HI_DRV_PIXEL_FMT_YVYU:
            ret = VENC_PACKAGE_422_YVYU;
            break;

        case HI_DRV_PIXEL_FMT_YUV420P:
            ret = VENC_PLANNAR_420;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_1X2:
            ret = VENC_PLANNAR_422;
            break;

        case HI_DRV_PIXEL_FMT_YUV422_2X1:
            ret = VENC_PLANNAR_422;
            break;

        default:
            ret = VENC_UNKNOW;
            break;
    }

    return ret;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     : flag :0 -> yuv_store_type; 1 -> yuv_sample_type; 2 -> package_sel
******************************************************************************/
static hi_u32 convert_pix_format(hi_drv_pixel_format old_format, hi_u32 flag)
{
    hi_u32 ret = HI_SUCCESS;
    if (flag == 0) { /* yuv_store_type */
        ret = convert_pix_format_yuv_store_type(old_format);
    } else if (flag == 1) { /* yuv_sample_type */
        ret = convert_pix_format_yuv_sample_type(old_format);
    } else if (flag == 2) { /* 2: flag, package_sel */
        ret = convert_pix_format_package_sel(old_format);
    } else if (flag == 3) { /* 3: flag, for proc frame_info */
        ret = convert_pix_format_frame_info(old_format);
    } else {
        ret = HI_FALSE;
    }

    return ret;
}

/* 成功取帧返回 HI_SUCCESS,连一次都取不成功返回HI_FAILURE */
static hi_s32 quick_encode_process(vedu_efl_enc_para *enc_handle, hi_handle get_img_hhd)
{
    hi_bool last_frame = HI_FALSE;
    hi_drv_video_frame image_temp;
    vedu_efl_enc_para *enc_para = HI_NULL;
    hi_u32 ve_chn;
    hi_u32 skip_cnt = 0;

    enc_para = enc_handle;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);

    enc_para->stat.get_frame_num_try++;

    if ((enc_para->src_info.get_image)(get_img_hhd, &enc_para->image) == HI_SUCCESS) {
        enc_para->stat.get_frame_num_ok++;
        while ((!last_frame)  && (skip_cnt < 100)) {        /* 100: skip_cnt */
            enc_para->stat.get_frame_num_try++;
            skip_cnt++;
            if ((enc_para->src_info.get_image)(get_img_hhd, &image_temp) == HI_SUCCESS) {
                enc_para->stat.get_frame_num_ok++;

                enc_para->stat.put_frame_num_try++;
                (*enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
                enc_para->stat.put_frame_num_ok++;
                enc_para->stat.quick_encode_skip++;
                enc_para->image = image_temp;
            } else {
                last_frame = HI_TRUE;
            }
        }

        if (skip_cnt >= 100) {      /* 100: skip_cnt */
            HI_WARN_VENC("skip frame too much! force to stop!\n");
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void set_default_input_frm_rate_type(vedu_efl_enc_para *enc_handle)
{
    hi_u32 ve_chn = HI_VENC_MAX_CHN;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    if (ve_chn >= HI_VENC_MAX_CHN) {
        HI_ERR_VENC("handle %p not match with g_venc_chn[x].venc_handle.\n", enc_handle);
        return;
    }

    if (HI_ID_MAX != g_venc_chn[ve_chn].src_mod_id) {
        /* attach mode default use auto */
        enc_handle->input_frm_rate_type_default = HI_VENC_FRMRATE_AUTO;
    } else {
        /* no attach mode default use user config */
        enc_handle->input_frm_rate_type_default = HI_VENC_FRMRATE_USER;
    }

    return;
}

hi_void venc_drv_efl_wake_up_thread(hi_void)
{
    venc_drv_osal_give_event(&g_venc_event);

    return ;
}

hi_void venc_drv_efl_sort_priority(hi_void)
{
    hi_u32 i, j;
    for (i = 0; i < HI_VENC_MAX_CHN - 1; i++) {
        for (j =  HI_VENC_MAX_CHN - 1; j > i; j--) {
            if (g_priority_tab[1][j] > g_priority_tab[1][j - 1]) {
                hi_u32 temp0 = g_priority_tab[0][j];
                hi_u32 temp1 = g_priority_tab[1][j];
                g_priority_tab[0][j] = g_priority_tab[0][j - 1];
                g_priority_tab[1][j] = g_priority_tab[1][j - 1];
                g_priority_tab[0][j - 1] = temp0;
                g_priority_tab[1][j - 1] = temp1;
            }
        }
    }
}

hi_s32 venc_drv_efl_sort_priority_2(hi_s8 priority)
{
    hi_u32 i;
    hi_u32 cnt = 0;
    hi_u32 id = HI_VENC_MAX_CHN - 1;
    hi_bool find = 0;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((priority == g_priority_tab[1][i]) && (g_priority_tab[0][i] != INVAILD_CHN_FLAG)) {
            if (!find) {
                id = i;
                find = 1;
            }
            cnt++;
        }
    }

    if (!find) {
        HI_WARN_VENC("can't fine the channel match with priority(%d)\n", priority);
        return HI_FAILURE;
    }

    if (cnt == 1 || (id >= HI_VENC_MAX_CHN - 1)) {
        return HI_SUCCESS;
    }

    for (i = 0; (i < (cnt - 1)) && (id < (HI_VENC_MAX_CHN - 1)); i++, id++) {
        hi_u32 temp0 = g_priority_tab[0][id];
        hi_u32 temp1 = g_priority_tab[1][id];
        g_priority_tab[0][id] = g_priority_tab[0][id + 1];
        g_priority_tab[1][id] = g_priority_tab[1][id + 1];
        g_priority_tab[0][id + 1] = temp0;
        g_priority_tab[1][id + 1] = temp1;
    }

    return HI_SUCCESS;
}

#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
hi_void update_rcn_ref_share_buf_param(vedu_efl_rcn_ref_share_buf *rcn_ref_share_buf,
    vedu_efl_enc_para *enc_para, hi_u32 y_stride)
{
    hi_u32 curr_height, next_height, offset_addr_y;
    hi_u32 active_idx = rcn_ref_share_buf->active_rcn_idx;

    curr_height = rcn_ref_share_buf->rcn_next_height_y[active_idx];
    next_height = curr_height + enc_para->pic_height_y;

    offset_addr_y = curr_height * y_stride;
    rcn_ref_share_buf->rcn_ref_y_len[active_idx] = enc_para->all_height_y - curr_height;
    rcn_ref_share_buf->rcn_ref_c_len[active_idx] = rcn_ref_share_buf->rcn_ref_y_len[active_idx] / 2;    /* 2: half */
    rcn_ref_share_buf->rcn_ref_y_addr[active_idx] = rcn_ref_share_buf->rcn_ref_y_base[active_idx] + offset_addr_y;
    rcn_ref_share_buf->rcn_ref_c_addr[active_idx] = rcn_ref_share_buf->rcn_ref_c_base[active_idx] +
                                                    offset_addr_y / 2;      /* 2: half */

    if (next_height > enc_para->all_height_y) {
        rcn_ref_share_buf->rcn_next_height_y[active_idx] = next_height - enc_para->all_height_y;
    } else {
        rcn_ref_share_buf->rcn_next_height_y[active_idx] = next_height;
    }

    return;
}
#endif

static hi_void venc_drv_h265_set_reg_no_share_rcn(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    UINT32 ref_num = reg_cfg->buf_cfg.ref_num;

    reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr0_l =  enc_para->h265e_dpb.rcn_frame->phy_addr[0];
    reg_cfg->buf_cfg.vedu_vcpi_rec_caddr0_l = enc_para->h265e_dpb.rcn_frame->phy_addr[1];

    reg_cfg->buf_cfg.vedu_vcpi_nbi_mvst_addr_l = enc_para->h265e_dpb.rcn_frame->tmv_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_st_addr_l = enc_para->h265e_dpb.rcn_frame->pme_info_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmest_addr_l = enc_para->h265e_dpb.rcn_frame->pme_addr;

    reg_cfg->buf_cfg.vcpi_recst_ystride = enc_para->h265e_dpb.rcn_frame->y_stride;
    reg_cfg->buf_cfg.vcpi_recst_cstride = enc_para->h265e_dpb.rcn_frame->c_stride;
    if (ref_num > 0) {
        reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr0_l = enc_para->h265e_dpb.ref_frame[0]->phy_addr[0];
        reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr0_l = enc_para->h265e_dpb.ref_frame[0]->phy_addr[1];

        reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l = enc_para->h265e_dpb.ref_frame[0]->tmv_addr;
        reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_ld0_addr_l = enc_para->h265e_dpb.ref_frame[0]->pme_info_addr;
        reg_cfg->buf_cfg.vedu_vcpi_pmeld_l0_addr_l = enc_para->h265e_dpb.ref_frame[0]->pme_addr;

        reg_cfg->buf_cfg.vcpi_refy_l0_stride = enc_para->h265e_dpb.ref_frame[0]->y_stride;
        reg_cfg->buf_cfg.vcpi_refc_l0_stride = enc_para->h265e_dpb.ref_frame[0]->c_stride;
    }

    reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = 0;

    if (ref_num > 1) {
        reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l = enc_para->h265e_dpb.ref_frame[1]->phy_addr[0];
        reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l = enc_para->h265e_dpb.ref_frame[1]->phy_addr[1];
        reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = enc_para->h265e_dpb.ref_frame[1]->pme_addr;

        reg_cfg->buf_cfg.vcpi_refy_l1_stride = enc_para->h265e_dpb.ref_frame[1]->y_stride;
        reg_cfg->buf_cfg.vcpi_refc_l1_stride = enc_para->h265e_dpb.ref_frame[1]->c_stride;
    }

}

static hi_void venc_drv_h265_set_ref0_reg(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_addr[enc_para->rcn_ref_share_buf.active_ref0_idx];
    reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_base[enc_para->rcn_ref_share_buf.active_ref0_idx];

    reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_addr[enc_para->rcn_ref_share_buf.active_ref0_idx];
    reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_base[enc_para->rcn_ref_share_buf.active_ref0_idx];

    reg_cfg->buf_cfg.vcpi_ref0_luma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_y_len[enc_para->rcn_ref_share_buf.active_ref0_idx];
    reg_cfg->buf_cfg.vcpi_ref0_chroma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_c_len[enc_para->rcn_ref_share_buf.active_ref0_idx];

    reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l = enc_para->h265e_dpb.ref_frame[0]->tmv_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_ld0_addr_l = enc_para->h265e_dpb.ref_frame[0]->pme_info_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeld_l0_addr_l = enc_para->h265e_dpb.ref_frame[0]->pme_addr;

    reg_cfg->buf_cfg.vcpi_refy_l0_stride = enc_para->h265e_dpb.ref_frame[0]->y_stride;
    reg_cfg->buf_cfg.vcpi_refc_l0_stride = enc_para->h265e_dpb.ref_frame[0]->c_stride;
}

static hi_void venc_drv_h265_set_reg_share_rcn(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    UINT32 ref_num = reg_cfg->buf_cfg.ref_num;

    if (ref_num > 0) {
        venc_drv_h265_set_ref0_reg(reg_cfg, enc_para);
    }

    reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = 0;

    if (ref_num > 1) {
        reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l =
                enc_para->rcn_ref_share_buf.rcn_ref_y_addr[enc_para->rcn_ref_share_buf.active_ref1_idx];
        reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr1_l =
                enc_para->rcn_ref_share_buf.rcn_ref_y_base[enc_para->rcn_ref_share_buf.active_ref1_idx];

        reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l =
                enc_para->rcn_ref_share_buf.rcn_ref_c_addr[enc_para->rcn_ref_share_buf.active_ref1_idx];
        reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr1_l =
                enc_para->rcn_ref_share_buf.rcn_ref_c_base[enc_para->rcn_ref_share_buf.active_ref1_idx];

        reg_cfg->buf_cfg.vcpi_ref1_luma_length =
                enc_para->rcn_ref_share_buf.rcn_ref_y_len[enc_para->rcn_ref_share_buf.active_ref1_idx];
        reg_cfg->buf_cfg.vcpi_ref1_chroma_length =
                enc_para->rcn_ref_share_buf.rcn_ref_c_len[enc_para->rcn_ref_share_buf.active_ref1_idx];

        reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = enc_para->h265e_dpb.ref_frame[1]->pme_addr;

        reg_cfg->buf_cfg.vcpi_refy_l1_stride = enc_para->h265e_dpb.ref_frame[1]->y_stride;
        reg_cfg->buf_cfg.vcpi_refc_l1_stride = enc_para->h265e_dpb.ref_frame[1]->c_stride;
    }
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    update_rcn_ref_share_buf_param(&enc_para->rcn_ref_share_buf, enc_para, enc_para->h265e_dpb.rcn_frame->y_stride);
#endif

    reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_addr[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_base[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vedu_vcpi_rec_caddr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_addr[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vedu_vcpi_rec_caddr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_base[enc_para->rcn_ref_share_buf.active_rcn_idx];

    reg_cfg->buf_cfg.vcpi_rec_luma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_y_len[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vcpi_rec_chroma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_c_len[enc_para->rcn_ref_share_buf.active_rcn_idx];

    reg_cfg->buf_cfg.vedu_vcpi_nbi_mvst_addr_l = enc_para->h265e_dpb.rcn_frame->tmv_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_st_addr_l = enc_para->h265e_dpb.rcn_frame->pme_info_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmest_addr_l = enc_para->h265e_dpb.rcn_frame->pme_addr;

    reg_cfg->buf_cfg.vcpi_recst_ystride = enc_para->h265e_dpb.rcn_frame->y_stride;
    reg_cfg->buf_cfg.vcpi_recst_cstride = enc_para->h265e_dpb.rcn_frame->c_stride;

    return;
}

static hi_void venc_drv_h264_set_reg_no_share_rcn(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr0_l =  enc_para->h264_rcn_pic->phy_addr[0];
    reg_cfg->buf_cfg.vedu_vcpi_rec_caddr0_l = enc_para->h264_rcn_pic->phy_addr[1];

    reg_cfg->buf_cfg.vedu_vcpi_nbi_mvst_addr_l = enc_para->h264_rcn_pic->tmv_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_st_addr_l = enc_para->h264_rcn_pic->pme_info_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmest_addr_l = enc_para->h264_rcn_pic->pme_addr;

    reg_cfg->buf_cfg.vcpi_recst_ystride = enc_para->h264_rcn_pic->y_stride;
    reg_cfg->buf_cfg.vcpi_recst_cstride = enc_para->h264_rcn_pic->c_stride;
    if (enc_para->h264_reorder_ref[0] != HI_NULL) {
        reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr0_l = enc_para->h264_reorder_ref[0]->phy_addr[0];
        reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr0_l = enc_para->h264_reorder_ref[0]->phy_addr[1];

        reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l = enc_para->h264_reorder_ref[0]->tmv_addr;
        reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_ld0_addr_l = enc_para->h264_reorder_ref[0]->pme_info_addr;
        reg_cfg->buf_cfg.vedu_vcpi_pmeld_l0_addr_l = enc_para->h264_reorder_ref[0]->pme_addr;

        reg_cfg->buf_cfg.vcpi_refy_l0_stride = enc_para->h264_reorder_ref[0]->y_stride;
        reg_cfg->buf_cfg.vcpi_refc_l0_stride = enc_para->h264_reorder_ref[0]->c_stride;
    }

    reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = 0;

    if (enc_para->h264_reorder_ref[1] != HI_NULL) {
        reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l = enc_para->h264_reorder_ref[1]->phy_addr[0];
        reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l = enc_para->h264_reorder_ref[1]->phy_addr[1];
        reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = enc_para->h264_reorder_ref[1]->pme_addr;
        reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l = enc_para->h264_reorder_ref[1]->tmv_addr;

        reg_cfg->buf_cfg.vcpi_refy_l1_stride = enc_para->h264_reorder_ref[1]->y_stride;
        reg_cfg->buf_cfg.vcpi_refc_l1_stride = enc_para->h264_reorder_ref[1]->c_stride;
    }

    return;
}

static hi_void venc_drv_h264_set_ref0_reg(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_addr[enc_para->rcn_ref_share_buf.active_ref0_idx];
    reg_cfg->buf_cfg.vedu_vcpi_refy_l0_addr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_base[enc_para->rcn_ref_share_buf.active_ref0_idx];

    reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_addr[enc_para->rcn_ref_share_buf.active_ref0_idx];
    reg_cfg->buf_cfg.vedu_vcpi_refc_l0_addr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_base[enc_para->rcn_ref_share_buf.active_ref0_idx];

    reg_cfg->buf_cfg.vcpi_ref0_luma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_y_len[enc_para->rcn_ref_share_buf.active_ref0_idx];
    reg_cfg->buf_cfg.vcpi_ref0_chroma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_c_len[enc_para->rcn_ref_share_buf.active_ref0_idx];

    reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l = enc_para->h264_reorder_ref[0]->tmv_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_ld0_addr_l = enc_para->h264_reorder_ref[0]->pme_info_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeld_l0_addr_l = enc_para->h264_reorder_ref[0]->pme_addr;

    reg_cfg->buf_cfg.vcpi_refy_l0_stride = enc_para->h264_reorder_ref[0]->y_stride;
    reg_cfg->buf_cfg.vcpi_refc_l0_stride = enc_para->h264_reorder_ref[0]->c_stride;
}

static hi_void venc_drv_h264_set_reg_share_rcn(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    if (enc_para->h264_reorder_ref[0] != HI_NULL) {
        venc_drv_h264_set_ref0_reg(reg_cfg, enc_para);
    }

    reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l = 0;
    reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = 0;

    if (enc_para->h264_reorder_ref[1] != HI_NULL) {
        reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr0_l =
                enc_para->rcn_ref_share_buf.rcn_ref_y_addr[enc_para->rcn_ref_share_buf.active_ref1_idx];
        reg_cfg->buf_cfg.vedu_vcpi_refy_l1_addr1_l =
                enc_para->rcn_ref_share_buf.rcn_ref_y_base[enc_para->rcn_ref_share_buf.active_ref1_idx];

        reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr0_l =
                enc_para->rcn_ref_share_buf.rcn_ref_c_addr[enc_para->rcn_ref_share_buf.active_ref1_idx];
        reg_cfg->buf_cfg.vedu_vcpi_refc_l1_addr1_l =
                enc_para->rcn_ref_share_buf.rcn_ref_c_base[enc_para->rcn_ref_share_buf.active_ref1_idx];

        reg_cfg->buf_cfg.vcpi_ref1_luma_length =
                enc_para->rcn_ref_share_buf.rcn_ref_y_len[enc_para->rcn_ref_share_buf.active_ref1_idx];
        reg_cfg->buf_cfg.vcpi_ref1_chroma_length =
                enc_para->rcn_ref_share_buf.rcn_ref_c_len[enc_para->rcn_ref_share_buf.active_ref1_idx];

        reg_cfg->buf_cfg.vedu_vcpi_pmeld_l1_addr_l = enc_para->h264_reorder_ref[1]->pme_addr;
        reg_cfg->buf_cfg.vedu_vcpi_nbi_mvld_addr_l = enc_para->h264_reorder_ref[1]->tmv_addr;

        reg_cfg->buf_cfg.vcpi_refy_l1_stride = enc_para->h264_reorder_ref[1]->y_stride;
        reg_cfg->buf_cfg.vcpi_refc_l1_stride = enc_para->h264_reorder_ref[1]->c_stride;
    }
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    update_rcn_ref_share_buf_param(&enc_para->rcn_ref_share_buf, enc_para, enc_para->h264_rcn_pic->y_stride);
#endif
    reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_addr[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vedu_vcpi_rec_yaddr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_y_base[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vedu_vcpi_rec_caddr0_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_addr[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vedu_vcpi_rec_caddr1_l =
            enc_para->rcn_ref_share_buf.rcn_ref_c_base[enc_para->rcn_ref_share_buf.active_rcn_idx];

    reg_cfg->buf_cfg.vcpi_rec_luma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_y_len[enc_para->rcn_ref_share_buf.active_rcn_idx];
    reg_cfg->buf_cfg.vcpi_rec_chroma_length =
            enc_para->rcn_ref_share_buf.rcn_ref_c_len[enc_para->rcn_ref_share_buf.active_rcn_idx];

    reg_cfg->buf_cfg.vedu_vcpi_nbi_mvst_addr_l = enc_para->h264_rcn_pic->tmv_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmeinfo_st_addr_l = enc_para->h264_rcn_pic->pme_info_addr;
    reg_cfg->buf_cfg.vedu_vcpi_pmest_addr_l = enc_para->h264_rcn_pic->pme_addr;

    reg_cfg->buf_cfg.vcpi_recst_ystride = enc_para->h264_rcn_pic->y_stride;
    reg_cfg->buf_cfg.vcpi_recst_cstride = enc_para->h264_rcn_pic->c_stride;

    return;
}

static hi_void venc_drv_efl_cfg_reg_part1(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    /* get ctrl_cfg default info */
    reg_cfg->ctrl_cfg.protocol = enc_para->protocol;
    reg_cfg->ctrl_cfg.profile = enc_para->h264_hp_en;
    reg_cfg->ctrl_cfg.enc_width = enc_para->rc.pic_width;
    reg_cfg->ctrl_cfg.enc_height = enc_para->rc.pic_height;
    reg_cfg->ctrl_cfg.slc_split_en = enc_para->hal.slc_split_en;   /* 0: not enable 1: enable */
    reg_cfg->ctrl_cfg.split_size = enc_para->hal.split_size;
    reg_cfg->ctrl_cfg.csc_mode = 0;
    reg_cfg->ctrl_cfg.secure_en = 0;
    reg_cfg->ctrl_cfg.low_dly_mode = enc_para->rc.low_delay_mode;
    reg_cfg->ctrl_cfg.time_out_en = 2;          /* 2: time_out_en */
    reg_cfg->ctrl_cfg.enable_rcn_ref_share_buf = enc_para->enable_rcn_ref_share_buf;

    reg_cfg->ctrl_cfg.lcu_performance_en = 1;
    if (reg_cfg->ctrl_cfg.protocol == VEDU_H265) {
        reg_cfg->ctrl_cfg.lcu_performance_baseline = 3200;      /* 3200: lcu_performance_baseline */
    } else {
        reg_cfg->ctrl_cfg.lcu_performance_baseline = 800;       /* 800: lcu_performance_baseline */
    }

    reg_cfg->ctrl_cfg.reg_config_mode = VENC_REG_CONFIG_MODE; /* 0: cfg mode 1: ddr mode */

    if (enc_para->rc.is_intra_pic) {
        if ((enc_para->prepend_sps_pps_enable == HI_TRUE) || (enc_para->stat.get_frame_num_ok == 1)) {
            reg_cfg->ctrl_cfg.parameter_set_en = 1;
        } else {
            reg_cfg->ctrl_cfg.parameter_set_en = 0;
        }
    }

    /* get buf_cfg default info */
    reg_cfg->buf_cfg.store_fmt = enc_para->hal.yuv_store_type;           /* 0, semiplannar; 1, package; 2,planer */
    reg_cfg->buf_cfg.package_sel = enc_para->hal.package_sel;
    reg_cfg->buf_cfg.src_y_addr = enc_para->hal.src_y_addr;
    reg_cfg->buf_cfg.src_c_addr = enc_para->hal.src_c_addr;
    reg_cfg->buf_cfg.src_v_addr = enc_para->hal.src_v_addr;                  /* just for input of planner */
    reg_cfg->buf_cfg.s_stride_y = enc_para->hal.s_stride_y;
    reg_cfg->buf_cfg.s_stride_c = enc_para->hal.s_stride_c;


    reg_cfg->buf_cfg.rcn_idx = enc_para->rc.rcn_idx;             /* 0 or 1, idx for rcn, !idx for ref */
    reg_cfg->buf_cfg.pme_stride = (enc_para->rc.pic_width + 255) / 256 * 256 / 4;   /* 255, 256, 4: pme_stride */
}

static hi_void venc_drv_efl_cfg_reg_part2(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    hi_u32 i = 0;

    reg_cfg->buf_cfg.tunl_cell_addr = enc_para->hal.tunl_cell_addr;

    if (g_vedu_ip_ctx.is_chip_id_v500_r001 == HI_TRUE) {
        for (i = 0; i < 16; i++) {          /* 16: index */
            reg_cfg->buf_cfg.strm_buf_addr[i] = enc_para->hal.vedu_strm_addr[i]; /* 96mv200 */
            reg_cfg->buf_cfg.strm_buf_size[i] = enc_para->hal.vedu_strm_buf_len[i];
        }

        for (i = 0; i < 48; i++) {          /* 48: index */
            reg_cfg->buf_cfg.para_set_reg[i] = enc_para->hal.para_set_array[i];
        }

        reg_cfg->buf_cfg.para_set_addr = enc_para->hal.para_set_phy_addr;
        reg_cfg->buf_cfg.para_set_buf_size = enc_para->hal.para_set_len;

        reg_cfg->buf_cfg.stream_flag = STREAM_LIST;
    } else {
        reg_cfg->buf_cfg.strm_buf_addr[0] = enc_para->hal.strm_buf_addr;
        reg_cfg->buf_cfg.strm_buf_size[0] = enc_para->hal.strm_buf_size;
        reg_cfg->buf_cfg.stream_flag = STREAM_RECYCLE;
    }

    reg_cfg->buf_cfg.strm_buf_rp_addr = enc_para->hal.strm_buf_rp_addr; /* phy addr for hardware */
    reg_cfg->buf_cfg.strm_buf_wp_addr = enc_para->hal.strm_buf_wp_addr;
   /* get rc_cfg default info */
    reg_cfg->rc_cfg.intra_pic = enc_para->rc.is_intra_pic;
    reg_cfg->rc_cfg.target_bits = enc_para->rc.target_bits;         /* target_bits of each frame */
    reg_cfg->rc_cfg.start_qp = enc_para->rc.start_qp;
    reg_cfg->rc_cfg.min_qp = enc_para->rc.min_qp;
    reg_cfg->rc_cfg.max_qp = enc_para->rc.max_qp;
    reg_cfg->rc_cfg.i_qp_delta = enc_para->rc.i_qp_delta;
    reg_cfg->rc_cfg.p_qp_delta = enc_para->rc.p_qp_delta;
    reg_cfg->rc_cfg.avbr = enc_para->rc.is_avbr;
    reg_cfg->rc_cfg.pic_type = enc_para->rc.pic_type;

   /* get smmu_cfg default info */
#ifdef HI_SMMU_SUPPORT
    reg_cfg->smmu_cfg.smmu_ns_page_base_addr = enc_para->hal.smmu_page_base_addr;
    reg_cfg->smmu_cfg.smmu_ns_err_write_addr = enc_para->hal.smmu_err_write_addr;
    reg_cfg->smmu_cfg.smmu_ns_err_read_addr = enc_para->hal.smmu_err_read_addr;
#endif
   /* get stream_cfg info */
    reg_cfg->stream_cfg.slc_hdr_stream[0] = enc_para->hal.slc_hdr_stream[0];
    reg_cfg->stream_cfg.slc_hdr_stream[1] = enc_para->hal.slc_hdr_stream[1];
    reg_cfg->stream_cfg.slc_hdr_stream[2] = enc_para->hal.slc_hdr_stream[2];    /* 2: index */
    reg_cfg->stream_cfg.slc_hdr_stream[3] = enc_para->hal.slc_hdr_stream[3];    /* 3: index */

    reg_cfg->stream_cfg.reorder_stream[0] = enc_para->hal.reorder_stream[0];
    reg_cfg->stream_cfg.reorder_stream[1] = enc_para->hal.reorder_stream[1];
    reg_cfg->stream_cfg.marking_stream[0] = enc_para->hal.marking_stream[0];
    reg_cfg->stream_cfg.marking_stream[1] = enc_para->hal.marking_stream[1];
}

static hi_void venc_drv_efl_cfg_rcn_reg_h265(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    if (enc_para->rc.pic_type != PIC_INTRA) {
            reg_cfg->buf_cfg.ref_num = enc_para->h265e_dpb.rcn_frame->ref_num;
        } else {
            reg_cfg->buf_cfg.ref_num = 0;
        }

        if (enc_para->enable_rcn_ref_share_buf) {
            venc_drv_h265_set_reg_share_rcn(reg_cfg, enc_para);
        } else {
            venc_drv_h265_set_reg_no_share_rcn(reg_cfg, enc_para);
        }

        if (reg_cfg->buf_cfg.ref_num > 0) {
            reg_cfg->buf_cfg.vcpi_curr_ref_long_flag = enc_para->h265e_dpb.rcn_frame->ref_info[0].ref_long;
            reg_cfg->buf_cfg.long_term_refpic = enc_para->h265e_dpb.rcn_frame->ref_info[0].ref_long;
        }

        if (reg_cfg->buf_cfg.ref_num > 1) {
            reg_cfg->buf_cfg.vcpi_curr_ref_long_flag |= enc_para->h265e_dpb.rcn_frame->ref_info[1].ref_long << 1;
            reg_cfg->buf_cfg.long_term_refpic |= enc_para->h265e_dpb.rcn_frame->ref_info[1].ref_long;
        }

        reg_cfg->buf_cfg.pmv_poc[0] = enc_para->hal.base_cfg.pmv_poc[0];
        reg_cfg->buf_cfg.pmv_poc[1] = enc_para->hal.base_cfg.pmv_poc[1];
        reg_cfg->buf_cfg.pmv_poc[2] = enc_para->hal.base_cfg.pmv_poc[2];        /* 2: index */
        reg_cfg->buf_cfg.pmv_poc[3] = enc_para->hal.base_cfg.pmv_poc[3];        /* 3: index */
        reg_cfg->buf_cfg.pmv_poc[4] = enc_para->hal.base_cfg.pmv_poc[4];        /* 4: index */
        reg_cfg->buf_cfg.pmv_poc[5] = enc_para->hal.base_cfg.pmv_poc[5];        /* 5: index */
}

static hi_void venc_drv_efl_cfg_rcn_reg_h264(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    if (enc_para->rc.pic_type != PIC_INTRA) {
        reg_cfg->buf_cfg.ref_num = enc_para->h264_dpb.ref_num;
    } else {
        reg_cfg->buf_cfg.ref_num = 0;
    }
    if (enc_para->enable_rcn_ref_share_buf) {
        venc_drv_h264_set_reg_share_rcn(reg_cfg, enc_para);
    } else {
        venc_drv_h264_set_reg_no_share_rcn(reg_cfg, enc_para);
    }

    reg_cfg->buf_cfg.long_term_refpic =
            ((HI_NULL != enc_para->h264_reorder_ref[0] && enc_para->h264_reorder_ref[0]->ref == REF_LONG) ||
            (HI_NULL != enc_para->h264_reorder_ref[1] && enc_para->h264_reorder_ref[1]->ref == REF_LONG));

    reg_cfg->buf_cfg.pmv_poc[0] = enc_para->h264_poc.poc;

    if (enc_para->h264_reorder_ref[0] != HI_NULL) {
        reg_cfg->buf_cfg.pmv_poc[1] = enc_para->h264_reorder_ref[0]->poc;
    }
    if (enc_para->h264_reorder_ref[1] != HI_NULL) {
        reg_cfg->buf_cfg.pmv_poc[2] = enc_para->h264_reorder_ref[1]->poc;   /* 2: index */
    }
    reg_cfg->buf_cfg.pmv_poc[3] = 0;        /* 3: index */
    reg_cfg->buf_cfg.pmv_poc[4] = 0;        /* 4: index */
    reg_cfg->buf_cfg.pmv_poc[5] = 0;        /* 5: index */
}

static hi_void venc_drv_efl_set_reg_cfg_data(venc_hal_cfg *reg_cfg, vedu_efl_enc_para *enc_para)
{
    hi_u32 i = 0;

    venc_drv_efl_cfg_reg_part1(reg_cfg, enc_para);

    if (enc_para->protocol == VEDU_H265) {
        venc_drv_efl_cfg_rcn_reg_h265(reg_cfg, enc_para);
    } else if (enc_para->protocol == VEDU_H264) {
        venc_drv_efl_cfg_rcn_reg_h264(reg_cfg, enc_para);
    }

    venc_drv_efl_cfg_reg_part2(reg_cfg, enc_para);

    if (reg_cfg->ctrl_cfg.protocol == VEDU_H264) {
        reg_cfg->stream_cfg.slc_hdr_bits[0] = enc_para->hal.slc_hdr_bits;       /* 8bit_0 | mark | reorder | slchdr */
    } else {
        reg_cfg->stream_cfg.slc_hdr_bits[0] = enc_para->hal.slc_hdr_size_part1; /* 8bit_0 | mark | reorder | slchdr */
        reg_cfg->stream_cfg.slc_hdr_bits[1] = enc_para->hal.slc_hdr_size_part2; /* 8bit_0 | mark | reorder | slchdr */
        reg_cfg->stream_cfg.slc_hdr_part1 = enc_para->hal.slc_hdr_part1;
        for (i = 0; i < 8; i++) {       /* 8: index */
            reg_cfg->stream_cfg.cabac_slc_hdr_part2_seg[i] = enc_para->hal.cabac_slc_hdr_part2_seg[i];
        }
    }
    reg_cfg->ext_cfg.pts0 = enc_para->hal.pts0;
    reg_cfg->ext_cfg.pts1 = enc_para->hal.pts1;
    reg_cfg->ext_cfg.ext_flag = enc_para->hal.ext_flag;
    reg_cfg->ext_cfg.ext_fill_len = enc_para->hal.ext_fill_len;
    reg_cfg->ext_cfg.ddr_reg_cfg_phy_addr = enc_para->hal.ddr_reg_cfg_buf.start_phy_addr;
    reg_cfg->ext_cfg.ddr_reg_cfg_vir_addr = (VIRT_ADDR)enc_para->hal.ddr_reg_cfg_buf.start_vir_addr;
}

static hi_s32 venc_drv_efl_cfg_reg_venc(vedu_efl_enc_para *enc_handle)
{
    venc_hal_cfg reg_cfg;
    vedu_efl_enc_para *enc_para = enc_handle;

    memset_s(&reg_cfg, sizeof(venc_hal_cfg), 0, sizeof(venc_hal_cfg));
    venc_drv_board_init();
    venc_drv_efl_set_reg_cfg_data(&reg_cfg, enc_para);
    VENC_HAL_CFG_REG(&reg_cfg);

    return HI_SUCCESS;
}

static hi_void venc_drv_efl_get_read_back_data(venc_hal_read *read_back, vedu_hal *hal, vedu_rc *rc)
{
    hi_u32 i = 0;

    hal->venc_end_of_pic = read_back->venc_end_of_pic;
    rc->venc_buf_full = read_back->venc_buf_full;
    rc->venc_p_bit_overflow = read_back->venc_pbit_overflow;
    hal->mean_qp = read_back->mean_qp;
    rc->pic_bits = read_back->pic_bits;
    rc->mean_qp = read_back->mean_qp;
    rc->num_i_mb_current_frm = read_back->num_i_mb_cur_frm;

    rc->mhb_bits = read_back->mhb_bits;
    rc->txt_bits = read_back->txt_bits;
    rc->madi_val = read_back->madi_val;
    rc->madp_val = read_back->madp_val;

    for (i = 0; i < 16; i++) {      /* 16: index */
        hal->slice_length[i] = read_back->slice_length[i];
        hal->slice_is_end[i] = read_back->slice_is_end[i];
    }
}

static hi_void make_slice_header_set_h264(vedu_efl_enc_para *enc_para)
{
    vedu_efl_h264e_slc_hdr slc_hdr;

    if (enc_para->rc.pic_type == PIC_INTRA) {
        enc_para->frame_num = 0;
    }
    memcpy_s(&slc_hdr, sizeof(vedu_efl_h264e_slc_hdr), &enc_para->h264_slc_hdr, sizeof(vedu_efl_h264e_slc_hdr));
    slc_hdr.frame_num = enc_para->frame_num;
    enc_para->h264_slc_hdr.frame_num = slc_hdr.frame_num;
    /* HI_ERR_VENC("frame_num = %d \n", enc_para->frame_num); */
    slc_hdr.pic_order_cnt_type = enc_para->h264_poc.pic_order_cnt_type;
    slc_hdr.pic_order_cnt_lsb = enc_para->h264_poc.pic_order_cnt_lsb;
    slc_hdr.slice_type = enc_para->rc.is_intra_pic ?
                            2 : (enc_para->rc.pic_type == PIC_BIINTER ? 1 : 0);    /* 2: slice_type */

    enc_para->h264_slc_hdr.num_ref_idx_active_override_flag = 1;
    if (enc_para->rc.pic_type == PIC_BIINTER) {
        slc_hdr.num_ref_idx_l0_active_minus1 = 0;
    } else {
        slc_hdr.num_ref_idx_l0_active_minus1 = enc_para->h264_reorder_ref[1] != HI_NULL;
    }
    slc_hdr.num_ref_idx_l1_active_minus1 = 0;

    slc_hdr.direct_spatial_mv_pred_flag = 1;
    slc_hdr.ref_pic_list_reordering_flag_l0 = enc_para->h264_slc_hdr.reorder.ref_pic_list_reordering_flag_l0;
    slc_hdr.ref_pic_list_reordering_flag_l1 = enc_para->h264_slc_hdr.reorder.ref_pic_list_reordering_flag_l1;

    slc_hdr.long_term_reference_flag = 0;
    slc_hdr.adaptive_ref_pic_marking_mode_flag = enc_para->h264_slc_hdr.mark.adaptive_ref_pic_marking_mode_flag;

    /* HI_ERR_VENC("adaptive_ref_pic_marking_mode_flag = %d\n", slc_hdr.mark.adaptive_ref_pic_marking_mode_flag); */
    slc_hdr.num_ref_index = enc_para->num_ref_index;

    memset_s(&enc_para->hal.slc_hdr_stream, sizeof(enc_para->hal.slc_hdr_stream),
             0, sizeof(hi_u32) * 4);  /* 4: length */
    memset_s(&enc_para->hal.reorder_stream, sizeof(enc_para->hal.reorder_stream),
             0, sizeof(hi_u32) * 2);  /* 2: length */
    memset_s(&enc_para->hal.marking_stream, sizeof(enc_para->hal.marking_stream),
             0, sizeof(hi_u32) * 2);  /* 2: length */

    enc_para->hal.slc_hdr_bits = h264e_make_slc_hdr(enc_para, &slc_hdr);
}

static hi_void make_slice_header(vedu_efl_enc_para *enc_para)
{
    if (enc_para->protocol == VEDU_H264) {
        make_slice_header_set_h264(enc_para);
    } else if (enc_para->protocol == VEDU_H265) {
        vedu_efl_h265e_slc_hdr  slc_hdr;

        memcpy_s(&slc_hdr, sizeof(vedu_efl_h265e_slc_hdr), &enc_para->h265e_slc_hdr, sizeof(vedu_efl_h265e_slc_hdr));

        enc_para->rc.h264_frm_num = enc_para->rc.is_intra_pic ? 0 : enc_para->rc.h264_frm_num  + 1;
        slc_hdr.slice_type = enc_para->rc.is_intra_pic ?
                             2 : (enc_para->rc.pic_type == PIC_BIINTER ? 0 : 1);    /* 2: is_intra_pic */

        slc_hdr.poc_lsb = enc_para->h265_poc.pic_order_cnt_lsb;
        slc_hdr.sps_temporal_mvp_enable_flag = enc_para->hal.base_cfg.tmv_en;

        slc_hdr.sao_enabled_flag = enc_para->hal.base_cfg.vcpi_sao_luma ||
                                                enc_para->hal.base_cfg.vcpi_sao_chroma;
        slc_hdr.slice_sao_luma = enc_para->hal.base_cfg.vcpi_sao_luma;
        slc_hdr.slice_sao_chroma = enc_para->hal.base_cfg.vcpi_sao_chroma;
        slc_hdr.cabac_init_flag = enc_para->hal.base_cfg.vcpi_cabac_init_idc;

        if (enc_para->rc.is_intra_pic && enc_para->rc.is_avbr) {
            enc_para->rc.start_qp = enc_para->rc.start_qp - 2;      /* 2: start_qp */
        }

        slc_hdr.i_slice_qp_delta = enc_para->rc.start_qp - 26;      /* 26: start_qp */
        slc_hdr.db_filter_ctl_present = 1;
        slc_hdr.db_filter_override_enabled = 1;

        slc_hdr.first_slice_in_pic = 1;

        slc_hdr.slice_header_disable_df = 0;
        slc_hdr.df_override_flag = 1;

        slc_hdr.slice_lf_across_slice = 1;

        slc_hdr.five_minus_max_num_merge_cand = 5 - enc_para->hal.base_cfg.max_num_mergecand; /* 5: max_num_mergecand */
        slc_hdr.num_ref_idx_l0_active_minus1 = enc_para->h265e_slc_hdr.num_ref_idx_l0_active_minus1;
        slc_hdr.num_ref_idx_l1_active_minus1 = enc_para->h265e_slc_hdr.num_ref_idx_l1_active_minus1;

        slc_hdr.mvd_l1_zero_flag = 0;
        slc_hdr.collocated_from_l0_flag = 1;
        if ((slc_hdr.collocated_from_l0_flag && slc_hdr.num_ref_idx_l0_active_minus1 > 0) ||
        (!slc_hdr.collocated_from_l0_flag && slc_hdr.num_ref_idx_l1_active_minus1 > 0)) {
            slc_hdr.collocated_ref_idx = 0;
        }

        slc_hdr.short_term_ref_pic_set_idx = enc_para->h265e_slc_hdr.short_term_ref_pic_set_idx;
        slc_hdr.slice_poc_lsb_bits = enc_para->h265_poc.log2_max_poc_lsb;

        enc_para->hal.slc_hdr_bits = h265e_make_slice_head(enc_para, &slc_hdr);
    }
}

static hi_void get_hal_col_ref_cnt(vedu_efl_enc_para *enc_para, hi_s32 ref_cnt)
{
    hi_s32 col_ref_cnt;
    /* Forward reference frame of the current frame */
    /* CNcomment: 当前帧前向参考帧 */
    enc_para->hal.base_cfg.pmv_poc[1] = enc_para->h265e_dpb.ref_frame[0]->poc;
    if (ref_cnt > 1) {
        /* Backward reference frame of the current frame */
        /* CNcomment: 当前帧后向参考帧 */
        enc_para->hal.base_cfg.pmv_poc[2] = enc_para->h265e_dpb.ref_frame[1]->poc;  /* 2: index */
    }

    if (enc_para->rc.pic_type == PIC_BIINTER) {
        /* The col frame of the b frame is the backward reference frame */
        /* CNcomment: b帧的col帧是后向参考帧 */
        col_ref_cnt = enc_para->h265e_dpb.ref_frame[1]->ref_num;

        /* Current frame col pic reference frame */
        /* CNcomment: 当前帧col pic参考帧 */
        enc_para->hal.base_cfg.pmv_poc[3] = enc_para->h265e_dpb.ref_frame[1]->poc;  /* 3: index */
        if (col_ref_cnt > 0) {
            /* Current frame col pic forward reference frame */
            /* CNcomment: 当前帧col pic前向参考帧 */
            enc_para->hal.base_cfg.pmv_poc[4] = enc_para->h265e_dpb.ref_frame[1]->ref_info[0].poc; /* 4: index */
            if (ref_cnt > 1) {
                /* Current frame col pic backward reference frame */
                /* CNcomment: 当前帧col pic后向参考帧 */
                enc_para->hal.base_cfg.pmv_poc[5] = enc_para->h265e_dpb.ref_frame[1]->ref_info[1].poc;  /* 5: index */
            }
        }
    } else {
        col_ref_cnt = enc_para->h265e_dpb.ref_frame[0]->ref_num;

        /* Current frame col pic reference frame */
        /* CNcomment: 当前帧col pic参考帧 */
        enc_para->hal.base_cfg.pmv_poc[3] = enc_para->h265e_dpb.ref_frame[0]->poc;  /* 3: index */
        if (col_ref_cnt > 0) {
            /* Current frame col pic forward reference frame */
            /* CNcomment: 当前帧col pic前向参考帧 */
            enc_para->hal.base_cfg.pmv_poc[4] = enc_para->h265e_dpb.ref_frame[0]->ref_info[0].poc;  /* 4: index */

            if (ref_cnt > 1) {
                /* Current frame col pic backward reference frame */
                /* CNcomment: 当前帧col pic后向参考帧 */
                enc_para->hal.base_cfg.pmv_poc[5] = enc_para->h265e_dpb.ref_frame[0]->ref_info[1].poc; /* 5: index */
            }
        }
    }
}

static hi_void get_hal_info(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_s32 ref_cnt;

    enc_para->hal.src_y_addr = enc_in->bus_vi_y;
    enc_para->hal.src_c_addr = enc_in->bus_vi_c;
    enc_para->hal.src_v_addr = enc_in->bus_vi_v;
    enc_para->hal.s_stride_y = enc_in->vi_y_stride;
    enc_para->hal.s_stride_c = enc_in->vi_c_stride;

    if (enc_para->rc.low_delay_mode) {
        enc_para->hal.tunl_cell_addr = enc_in->tunl_cell_addr;
    }

    enc_para->hal.pts0 = enc_in->pts0;
    enc_para->hal.pts1 = enc_in->pts1;

    /* just for omx channel */
    enc_para->hal.ext_flag = enc_para->image_omx.flags;
    enc_para->hal.ext_fill_len = enc_para->image_omx.data_len;

    if (enc_para->protocol == VEDU_H265) {
        /* pmv_poc */
        enc_para->hal.base_cfg.pmv_poc[0] = enc_para->h265_poc.pic_order_cnt;  /* 当前帧 */
        /* Number of reference frames of the current frame */
        /* CNcomment: 当前帧参考帧的数目 */
        if (enc_para->rc.pic_type != PIC_INTRA) {
            ref_cnt = enc_para->h265e_dpb.rcn_frame->ref_num;
        } else {
            ref_cnt = 0;
        }

        if (ref_cnt > 0) {
            get_hal_col_ref_cnt(enc_para, ref_cnt);
        }
    }

    enc_para->rc.pme_info_idx = (enc_para->rc.pme_info_idx + 1) % 3;    /* 3: pme_info_idx */
    enc_para->rc.rcn_idx = !enc_para->rc.rcn_idx;
}

/* calculating the ratio between instantaneous rate and bit_rate, to know */
/* whether the current rate of the output is under controlled */
/* CNcomment: 计算瞬时码率与目标码率之间的比例关系，输出当前的状态是否为可控状态 */
hi_s32 venc_drv_calculate_rc_ratio(hi_u32 current_inst_bits, hi_u32 bit_rate)
{
    hi_s32 inst_vs_bit_rate_ratio = 0;

    if (current_inst_bits > bit_rate) {
        inst_vs_bit_rate_ratio = (current_inst_bits - bit_rate) * 100 / bit_rate;   /* 100: inst_vs_bit_rate_ratio */
    } else {
        inst_vs_bit_rate_ratio = (bit_rate - current_inst_bits) * 100 / bit_rate;   /* 100: inst_vs_bit_rate_ratio */
        inst_vs_bit_rate_ratio = inst_vs_bit_rate_ratio * (-1);
    }

    return inst_vs_bit_rate_ratio;
}

static hi_s32 get_frame_write_buf(vedu_efl_enc_para *enc_para)
{
    hi_u32 ret;
    strm_mng_input_info input_info;
    strm_mng_stream_info stream_info;

    set_stream_info(enc_para, &stream_info);
    set_strm_mng_input_info(enc_para, &input_info);

    ret = strm_get_write_buffer(enc_para->strm_mng, &input_info, &stream_info);
    if (ret != HI_SUCCESS) {
        enc_para->cur_strm_buf_size = stream_info.cur_strm_buf_size;
        return ret;
    }

    get_stream_info(enc_para, &stream_info);

    return HI_SUCCESS;
}

#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
static hi_u32 get_ref_rcn_share_buffer_size_ex(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg)
{
    hi_u32 height_aline16 = D_VENC_ALIGN_UP(enc_cfg->frame_height, 16);     /* 16: bits */
    hi_u32 width_aline64 = D_VENC_ALIGN_UP(enc_cfg->frame_width, 64);       /* 64: bits */
    hi_u32 protocol = enc_cfg->protocol;
    hi_u32 sw_height = 0 ;
    hi_u32 block_height_y = 0 ;
    hi_u32 block_height_c = 0 ;
    hi_u32 ctb_size = 0 ;

    if (protocol == VEDU_H265) {
        sw_height = 32 ; ctb_size = 64 ;        /* 32, 64: sw_height, ctb_size */
        block_height_y = (((sw_height * 4 + 16) + ctb_size) + 15) / 16 * 16;    /* 4, 16, 15: block_height_y */
        block_height_c = block_height_y / 2;        /* 2: block_height_c */
    } else {
        sw_height = 16 ; ctb_size = 16 ;        /* 16, 16: sw_height, ctb_size */
        block_height_y = (((sw_height * 4 + 16) + ctb_size) + 15) / 16 * 16 ;   /* 4, 16, 15: block_height_y */
        block_height_c = block_height_y / 2;        /* 2: block_height_c */
    }

    enc_para->rcn_block_size = (block_height_y + block_height_c) * width_aline64;

    enc_para->y_size = width_aline64 * (block_height_y + height_aline16);
    enc_para->c_size = width_aline64 * (block_height_c + height_aline16 / 2);   /* 2: c_size */
    enc_para->all_height_y = height_aline16 + block_height_y;
    enc_para->pic_height_y = height_aline16;
    enc_para->blk_height_y = block_height_y;

    return enc_para->rcn_block_size;
}

hi_bool is_support_ref_rcn_share_buf(hi_void)
{
    return HI_TRUE;
}

static hi_s32 get_rcn_ref_share_buf(vedu_efl_enc_para *enc_para, hi_s32 num_ref_frames)
{
    hi_s32 index;
    pic_pool_buffer *pic = enc_para->pic_pool;

    for (index = 0; index < num_ref_frames; index++) {
        enc_para->rcn_ref_share_buf.rcn_ref_y_base[index] = pic[index].pic_addr;
        enc_para->rcn_ref_share_buf.rcn_ref_c_base[index] = pic[index].pic_addr + enc_para->y_size;
    }
    return HI_SUCCESS;
}

static hi_s32 init_rcn_ref_share_buff_param(vedu_efl_enc_para *enc_para)
{
    hi_s32 index;
    vedu_efl_rcn_ref_share_buf *share_buf = &enc_para->rcn_ref_share_buf;

    share_buf->active_rcn_idx = 0;
    share_buf->active_ref0_idx = 0;
    share_buf->active_ref1_idx = 0;

    for (index = 0; index < share_buf->active_num_ref_frames; index++) {
        share_buf->rcn_next_height_y[index] = 0;
        share_buf->rcn_next_height_c[index] = 0;
        share_buf->rcn_ref_y_len[index] = enc_para->all_height_y;
        share_buf->rcn_ref_y_len1[index] = 0;
        share_buf->rcn_ref_c_len[index] = share_buf->rcn_ref_y_len[index] / 2;  /* 2: rcn_ref_c_len */
        share_buf->rcn_ref_c_len1[index] = 0;
        share_buf->rcn_ref_y_addr[index] = share_buf->rcn_ref_y_base[index];
        share_buf->rcn_ref_c_addr[index] = share_buf->rcn_ref_c_base[index];
    }

    return HI_SUCCESS;
}

hi_s32 put_rcn_ref_share_buf(vedu_efl_enc_para *enc_para, hi_s32 num_ref_frames)
{
    return HI_SUCCESS;
}
#endif

static hi_void venc_drv_update_enc_info(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;
    vedu_rc *rc = &enc_para->rc;

    if (rc->venc_buf_full || rc->venc_p_bit_overflow) { /* buffer full -> LOST */
        if (!rc->rc_start) {
            venc_drv_fifo_write(&enc_para->valg_bits_fifo, 0);
        }
    } else {
        rc->rc_start = 0;
        enc_para->tr_count -= rc->vi_frm_rate;
        enc_para->inter_frm_cnt = rc->is_intra_pic ? 0 : (enc_para->inter_frm_cnt + 1);

        venc_drv_fifo_write(&enc_para->valg_bits_fifo, rc->pic_bits);

        if (rc->is_intra_pic) {
            rc->i_pre_qp = rc->current_qp;
        } else {
            rc->p_pre_qp = rc->current_qp;
        }

    }

    return;
}

static hi_s32 process_pic_h265(hi_u32 pic_type, vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_s32 ret = HI_FAILURE;

    switch (pic_type) {
        case PIC_INTRA:
            enc_para->ref_gap_cnt = 0;
            h265e_encode_poc(enc_para, pic_type);
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
            if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
                init_rcn_ref_share_buff_param(enc_para);
            }
#endif
            h265e_empty_pool(enc_para);
            h265e_dpb_flush(&enc_para->h265e_dpb);
            h265e_ref_init(enc_para, &enc_para->h265_ref);
            ret = h265e_dpb_get_new_frame(&enc_para->h265e_dpb, enc_para);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h265e_dpb_get_new_frame fail ! pic_type = %d \n", pic_type);
                return ret;
            }

            break;

        case PIC_INTER:
        case PIC_INTERSP:
        case PIC_INTERBP:
        case PIC_BIINTER:
            h265e_encode_poc(enc_para, pic_type);
            h265e_set_slc_hdr_ref_param(&enc_para->h265_ref, enc_para, &enc_para->h265e_slc_hdr);
            h265e_dpb_generate_ref_list(&enc_para->h265e_dpb, &enc_para->h265e_slc_hdr, enc_para);
            ret = h265e_dpb_get_new_frame(&enc_para->h265e_dpb, enc_para);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h265e_dpb_get_new_frame fail ! pic_type = %d \n", pic_type);
                return ret;
            }

            break;
        default:
            break;
    }

    return ret;
}

static hi_s32 process_pic_h264_intra(hi_u32 pic_type, vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_s32 ret;

    ret = h264e_init_ref(&enc_para->h264_ref);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h264e_init_ref fail \n");
        return ret;
    }
    ret = h264e_encode_poc(enc_para, pic_type);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h264e_encode_poc fail \n");
        return ret;
    }
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        init_rcn_ref_share_buff_param(enc_para);
    }
#endif
    ret = h264e_dpb_flush(enc_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h264e_dpb_flush fail \n");
        return ret;
    }
    ret = h264e_dpb_get_new_frame(enc_para, enc_in);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h264e_dpb_get_new_frame fail ! pic_type = %d \n", pic_type);
        return ret;
    }
    ret = h264e_slc_hdr_mark_reorder(enc_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("h264e_slc_hdr_mark_reorder fail \n");
        return ret;
    }

    return ret;
}

static hi_s32 process_pic_h264(hi_u32 pic_type, vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_s32 ret = HI_FAILURE;

    switch (pic_type) {
        case PIC_INTRA:
            ret = process_pic_h264_intra(pic_type, enc_para, enc_in);
            break;

        case PIC_INTER:
        case PIC_INTERSP:
        case PIC_INTERBP:
        case PIC_BIINTER:
            ret = h264e_encode_poc(enc_para, pic_type);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h264e_encode_poc fail ! pic_type = %d \n", pic_type);
                return ret;
            }
            ret = h264e_dpb_init_ref_list(enc_para, pic_type);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h264e_dpb_init_ref_list fail ! pic_type = %d \n", pic_type);
                return ret;
            }
            ret = h264e_dpb_get_new_frame(enc_para, enc_in);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h264e_dpb_get_new_frame fail ! pic_type = %d \n", pic_type);
                return ret;
            }
            ret = h264e_slc_hdr_mark_reorder(enc_para);
            if (ret != HI_SUCCESS) {
                HI_ERR_VENC("h264e_slc_hdr_mark_reorder fail ! pic_type = %d \n", pic_type);
                return ret;
            }
            break;
        default:
            break;
    }

    return ret;
}

static hi_s32 venc_drv_process_pic(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 pic_type = enc_para->rc.pic_type;

    if (enc_para->protocol == VEDU_H265) {
        ret = process_pic_h265(pic_type, enc_para, enc_in);
    } else if (enc_para->protocol == VEDU_H264) {
        ret = process_pic_h264(pic_type, enc_para, enc_in);
    }

    return ret;
}

static hi_s32 venc_drv_efl_start_one_frame_venc(vedu_efl_enc_para *enc_handle, vedu_efl_enc_in *enc_in)
{
    hi_u32 ret = HI_SUCCESS;
    vedu_efl_enc_para *enc_para = enc_handle;

    if (enc_para->not_enough_buf != HI_TRUE) {
        if (rc_client_task_start(&(enc_para->rc)) == HI_SUCCESS) {
            goto SKIP;
        }

        enc_para->stat.put_frame_num_try++;
        if (enc_para->omx_chn) {
            if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE,
                                         HI_SUCCESS, &enc_para->image_omx) != HI_SUCCESS) {
                HI_ERR_VENC("put msg omx err!\n");
            }
            enc_para->image_valid = HI_FALSE;
            enc_para->stat.ebd_cnt++;
            enc_para->stat.msg_queue_num++;
        } else {
            (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
            enc_para->image_valid =  HI_FALSE;
        }

        enc_para->stat.put_frame_num_ok++;

        HI_WARN_VENC("start RC task error!\n");
        enc_para->stat.rc_calculate_fail++;
        return HI_FAILURE;

SKIP:
        ret = venc_drv_process_pic(enc_para, enc_in);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("process pic fail!\n");
            return ret;
        }

        make_slice_header(enc_para);
        get_hal_info(enc_para, enc_in);
    }

    enc_para->stat.get_stream_buf_try++;
    ret = get_frame_write_buf(enc_para);
    if (ret == HI_SUCCESS) {
        enc_para->stat.get_stream_buf_ok++;
    }

    enc_para->not_enough_buf = (ret == HI_SUCCESS) ? HI_FALSE : HI_TRUE;

    if (ret != HI_SUCCESS) {
        enc_para->stat.start_one_frame_fail++;
    } else {
        enc_para->first_slc_for_low_dly = HI_TRUE;
    }

    return ret;
}

static hi_void put_frame_write_buffer(vedu_efl_enc_para *enc_para)
{
    strm_mng_input_info input_info;
    strm_mng_stream_info stream_info;

    set_strm_mng_input_info(enc_para, &input_info);
    set_stream_info(enc_para, &stream_info);

    strm_put_write_buffer(enc_para->strm_mng, &input_info, &stream_info);

    HI_INFO_VENC("current buffer size= %d output pic_size = %d  too_few_buffer_skip = %d, pts = 0x%x\n",
        stream_info.buf[0].size, input_info.slice_length[0],
        stream_info.too_few_buffer_skip, enc_para->image_omx.time_stamp0);

    enc_para->stat.too_few_buffer_skip = stream_info.too_few_buffer_skip;
    enc_para->hal.slice_idx = stream_info.slice_idx;
    enc_para->hal.first_slc_one_frm = stream_info.first_slc_one_frm;
}

static hi_void venc_drv_efl_end_one_frame_venc(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    put_frame_write_buffer(enc_para);

    enc_para->hal.slice_idx = 0;

    venc_drv_update_enc_info(enc_handle);

    return;
}

#ifdef VENC_SUPPORT_JPGE
static hi_void put_jpge_write_buffer(vedu_efl_enc_para *enc_para)
{
    strm_mng_input_info input_info;
    strm_mng_stream_info stream_info;

    set_strm_mng_input_info(enc_para, &input_info);
    set_stream_info(enc_para, &stream_info);

    if (strm_put_jpge_write_buffer(enc_para->strm_mng, &input_info, &stream_info) != HI_SUCCESS) {
        HI_ERR_VENC("put JPGE write buffer failed!");
    }

    enc_para->stat.too_few_buffer_skip = stream_info.too_few_buffer_skip;
}

static hi_s32 venc_drv_efl_end_one_frame_jpge(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    put_jpge_write_buffer(enc_para);

    return HI_SUCCESS;
}
#endif

static hi_void venc_drv_efl_end_one_slice_venc(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para* enc_para = enc_handle;
    put_frame_write_buffer(enc_para);
}

static hi_void bipredb_3_b_frame(vedu_efl_h265e_sps *sps)
{
    hi_u32 i;

    sps->num_short_term_ref_pic_sets = 5;            /* 5: num_short_term_ref_pic_sets */

    /* first p after IDR */
    sps->num_negative_pics[0] = 1;
    sps->num_positive_pics[0] = 0;
    sps->delta_poc_s0_minus1[0][0] = 3;          /* 3: delta_poc_s0_minus1 */
    sps->used_by_curr_pic_s0_flag[0][0] = 1;

    /* dual p */
    sps->num_negative_pics[1] = 2;       /* 2: num_negative_pics */
    sps->num_positive_pics[1] = 0;

    for (i = 0; i < sps->num_negative_pics[1]; i++) {
        sps->delta_poc_s0_minus1[1][i] = 3;      /* 3: delta_poc_s0_minus1 */
        sps->used_by_curr_pic_s0_flag[1][i] = 1;
    }

    /* B1 */
    sps->num_negative_pics[2] = 1;       /* 2: index */
    sps->num_positive_pics[2] = 1;       /* 2: index */

    for (i = 0; i < sps->num_negative_pics[2]; i++) {    /* 2: index */
        sps->delta_poc_s0_minus1[2][i] = 0;              /* 2: index */
        sps->used_by_curr_pic_s0_flag[2][i] = 1;         /* 2: index */
    }

    for (i = 0; i < sps->num_positive_pics[2]; i++) {    /* 2: index */
        sps->delta_poc_s1_minus1[2][i] = 2;              /* 2: index */
        sps->used_by_curr_pic_s1_flag[2][i] = 1;         /* 2: index */
    }

    /* B2 */
    sps->num_negative_pics[3] = 1;               /* 3: index */
    sps->num_positive_pics[3] = 1;               /* 3: index */

    for (i = 0; i < sps->num_negative_pics[3]; i++) {    /* 3: index */
        sps->delta_poc_s0_minus1[3][i] = 1;              /* 3: index */
        sps->used_by_curr_pic_s0_flag[3][i] = 1;         /* 3: index */
    }

    for (i = 0; i < sps->num_positive_pics[3]; i++) {    /* 3: index */
        sps->delta_poc_s1_minus1[3][i] = 1;              /* 3: index */
        sps->used_by_curr_pic_s1_flag[3][i] = 1;         /* 3: index */
    }

    /* B3 */
    sps->num_negative_pics[4] = 1;       /* 4: index */
    sps->num_positive_pics[4] = 1;       /* 4: index */

    for (i = 0; i < sps->num_negative_pics[4]; i++) {    /* 4: index */
        sps->delta_poc_s0_minus1[4][i] = 2;              /* 2, 4: delta_poc_s0_minus1, index */
        sps->used_by_curr_pic_s0_flag[4][i] = 1;         /* 4: index */
    }

    for (i = 0; i < sps->num_positive_pics[4]; i++) {    /* 4: index */
        sps->delta_poc_s1_minus1[4][i] = 0;              /* 4: index */
        sps->used_by_curr_pic_s1_flag[4][i] = 1;         /* 4: index */
    }
}

static hi_void bipredb_2_b_frame(vedu_efl_h265e_sps *sps)
{
    hi_u32 i;

    sps->num_short_term_ref_pic_sets = 4;        /* 4: num_short_term_ref_pic_sets */

        /* first p after IDR */
    sps->num_negative_pics[0] = 1;
    sps->num_positive_pics[0] = 0;
    sps->delta_poc_s0_minus1[0][0] = 2;          /* 2: delta_poc_s0_minus1 */
    sps->used_by_curr_pic_s0_flag[0][0] = 1;

    /* dual p */
    sps->num_negative_pics[1] = 2;               /* 2: num_negative_pics */
    sps->num_positive_pics[1] = 0;

    for (i = 0; i < sps->num_negative_pics[1]; i++) {
        sps->delta_poc_s0_minus1[1][i] = 2;      /* 2: delta_poc_s0_minus1 */
        sps->used_by_curr_pic_s0_flag[1][i] = 1;
    }

    /* B1 */
    sps->num_negative_pics[2] = 1;           /* 2: index */
    sps->num_positive_pics[2] = 1;           /* 2: index */

    for (i = 0; i < sps->num_negative_pics[2]; i++) {    /* 2: index */
        sps->delta_poc_s0_minus1[2][i] = 0;  /* 2: index */
        sps->used_by_curr_pic_s0_flag[2][i] = 1;     /* 2: index */
    }

    for (i = 0; i < sps->num_positive_pics[2]; i++) {    /* 2: index */
        sps->delta_poc_s1_minus1[2][i] = 1;              /* 2: index */
        sps->used_by_curr_pic_s1_flag[2][i] = 1;         /* 2: index */
    }

    /* B2 */
    sps->num_negative_pics[3] = 1;               /* 3: index */
    sps->num_positive_pics[3] = 1;               /* 3: index */

    for (i = 0; i < sps->num_negative_pics[3]; i++) {    /* 3: index */
        sps->delta_poc_s0_minus1[3][i] = 1;              /* 3: index */
        sps->used_by_curr_pic_s0_flag[3][i] = 1;         /* 3: index */
    }

    for (i = 0; i < sps->num_positive_pics[3]; i++) {    /* 3: index */
        sps->delta_poc_s1_minus1[3][i] = 0;              /* 3: index */
        sps->used_by_curr_pic_s1_flag[3][i] = 1;         /* 3: index */
    }
}

static hi_void flush_strm_header_h265_bipredb(vedu_efl_h265e_sps *sps, hi_u32 b_frm_num)
{
    hi_u32 i;

    switch (b_frm_num) {
        case 1:     /* 1: b_frm_num */
            sps->num_short_term_ref_pic_sets = 3;    /* 3: num_short_term_ref_pic_sets */

            /* first p after IDR */
            sps->num_negative_pics[0] = 1;
            sps->num_positive_pics[0] = 0;
            sps->delta_poc_s0_minus1[0][0] = 1;
            sps->used_by_curr_pic_s0_flag[0][0] = 1;

            /* dual p */
            sps->num_negative_pics[1] = 2;       /* 2: num_negative_pics */
            sps->num_positive_pics[1] = 0;

            for (i = 0; i < sps->num_negative_pics[1]; i++) {
                sps->delta_poc_s0_minus1[1][i] = 1;
                sps->used_by_curr_pic_s0_flag[1][i] = 1;
            }

            /* B */
            sps->num_negative_pics[2] = 1;       /* 2: index */
            sps->num_positive_pics[2] = 1;       /* 2: index */

            for (i = 0; i < sps->num_negative_pics[2]; i++) {    /* 2: index */
                sps->delta_poc_s0_minus1[2][i] = 0;              /* 2: index */
                sps->used_by_curr_pic_s0_flag[2][i] = 1;         /* 2: index */
            }

            for (i = 0; i < sps->num_positive_pics[2]; i++) {    /* 2: index */
                sps->delta_poc_s1_minus1[2][i] = 0;              /* 2: index */
                sps->used_by_curr_pic_s1_flag[2][i] = 1;         /* 2: index */
            }

            break;
        case 2:     /* 2: b_frm_num */
            bipredb_2_b_frame(sps);
            break;
        case 3:     /* 3: b_frm_num */
            bipredb_3_b_frame(sps);
            break;
        default:
            HI_ERR_VENC("INVALID B FRAME NUM!\n");
            break;
    }
}

static hi_void flush_strm_header_h265_gop_type(vedu_efl_h265e_sps *sps, vedu_efl_enc_para *enc_para)
{
    hi_u32 i;

    switch (enc_para->rc.gop_type) {
        case HI_VENC_GOP_MODE_NORMALP:
            sps->num_short_term_ref_pic_sets = 1;
            sps->num_negative_pics[0] = 1;
            sps->num_positive_pics[0] = 0;

            for (i = 0; i < sps->num_negative_pics[0]; i++) {
                sps->delta_poc_s0_minus1[0][i] = 0;
                sps->used_by_curr_pic_s0_flag[0][i] = 1;
            }

            sps->long_term_ref_pic_present_flag = 0;
            break;

        case HI_VENC_GOP_MODE_BIPREDB:
            flush_strm_header_h265_bipredb(sps, enc_para->rc.b_frm_num);
            sps->long_term_ref_pic_present_flag = 0;
            break;

        case HI_VENC_GOP_MODE_DUALP:
            sps->num_short_term_ref_pic_sets = 2;            /* 2: num_short_term_ref_pic_sets */

            /* dual p */
            sps->num_negative_pics[0] = 2;           /* 2: num_negative_pics */
            sps->num_positive_pics[0] = 0;
            sps->delta_poc_s0_minus1[0][0] = 0;
            sps->used_by_curr_pic_s0_flag[0][0] = 1;
            sps->delta_poc_s0_minus1[0][1] = 0;
            sps->used_by_curr_pic_s0_flag[0][1] = 1;

            /* for the first p after IDR */
            sps->num_negative_pics[1] = 1;
            sps->num_positive_pics[1] = 0;
            sps->delta_poc_s0_minus1[1][0] = 0;
            sps->used_by_curr_pic_s0_flag[1][0] = 1;
            if (enc_para->gop_attr.sp_interval > 0) {
                sps->long_term_ref_pic_present_flag = 1;
                sps->num_long_term_ref_pics_sps = 0;
            } else {
                sps->long_term_ref_pic_present_flag = 0;
            }

            break;
        default:
            HI_ERR_VENC("unknown gop!\n");
            break;
    }
}

static hi_void flush_strm_header_h265_set_pps(vedu_efl_h265e_pps *pps)
{
    pps->cu_qp_delta_enable = 1;
    pps->i_cb_qp_offset = 0;
    pps->i_cr_qp_offset = 0;

    pps->pic_disable_db_filter = 1;

    pps->lists_modification_present_flag = 0;
}

static hi_void flush_strm_header_h265_set_sps(vedu_efl_h265e_sps *sps, vedu_efl_enc_para *enc_para)
{
    hi_u32 i;

    sps->pic_width_in_luma_samples = ((enc_para->rc.pic_width  + 7) >> 3) << 3;      /* 7, 3: pic_width, bits */
    sps->pic_height_in_luma_samples = ((enc_para->rc.pic_height + 7) >> 3) << 3;     /* 7, 3: pic_width, bits */
    sps->pic_crop_left_offset = 0 ;
    sps->pic_crop_right_offset = (sps->pic_width_in_luma_samples  - enc_para->rc.pic_width) >> 1;
    sps->pic_crop_top_offset = 0 ;
    sps->pic_crop_bottom_offset = (sps->pic_height_in_luma_samples - enc_para->rc.pic_height) >> 1;
    sps->pic_cropping_flag = sps->pic_crop_left_offset ||
        (sps->pic_crop_top_offset | sps->pic_crop_right_offset) || sps->pic_crop_bottom_offset;

    sps->sao_enabled_flag = 0;
    sps->sps_temporal_mvp_enable_flag = enc_para->hal.base_cfg.tmv_en;
    sps->pcm_enable = enc_para->hal.base_cfg.vcpi_ipcm_en;
    sps->ipcm_log2_max_size = 3 ;        /* 3: ipcm_log2_max_size */
    sps->ipcm_log2_min_size = 3 ;        /* 3: ipcm_log2_min_size */

    sps->strong_intra_smoothing_flag = 1;
    sps->profile_idc = 100;              /* 100: profile_idc */

    flush_strm_header_h265_gop_type(sps, enc_para);

    enc_para->long_term_ref_pic_present_flag = sps->long_term_ref_pic_present_flag;
    enc_para->num_short_term_ref_pic_sets = sps->num_short_term_ref_pic_sets;
    sps->sps_max_sub_layers_minus1 = 0;

    for (i = 0; i <= sps->sps_max_sub_layers_minus1; i++) {
        sps->sps_max_dec_pic_buffering_minus1 = enc_para->rcn_num - 1;

        if (enc_para->rc.gop_type == HI_VENC_GOP_MODE_BIPREDB) {
            sps->sps_max_num_reorder_pics = 1;
        } else {
            sps->sps_max_num_reorder_pics = 0;
        }

        sps->sps_max_latency_increase_plus1 = 0;
    }
}

static hi_void flush_strm_header_h265_set_para(vedu_efl_enc_para *enc_para)
{
    hi_u32 i;

    vedu_efl_h265e_vps vps = {0};
    vedu_efl_h265e_sps sps = {0};
    vedu_efl_h265e_pps pps = {0};

    vps.vps_max_sub_layers_minus1 = 0;
    for (i = 0; i <= vps.vps_max_sub_layers_minus1; i++) {
        vps.vps_max_dec_pic_buffering_minus1[i] = enc_para->rcn_num - 1;

        if (enc_para->rc.gop_type == HI_VENC_GOP_MODE_BIPREDB) {
            vps.vps_max_num_reorder_pics[i] = 1;
        } else {
            vps.vps_max_num_reorder_pics[i] = 0;
        }

        vps.vps_max_latency_increase_plus1[i] = 0;
    }

    vps.general_interlaced_source_flag = !enc_para->is_progressive;
    vps.general_progressive_source_flag = enc_para->is_progressive;

    flush_strm_header_h265_set_sps(&sps, enc_para);

    flush_strm_header_h265_set_pps(&pps);

    enc_para->vps_bits = h265e_make_vps(enc_para->vps_stream, 64, &vps);    /* 64: size of vps */
    enc_para->sps_bits = h265e_make_sps(enc_para->sps_stream, 64, &sps);    /* 64: size of sps */
    enc_para->pps_bits = h265e_make_pps(enc_para->pps_stream, 320, &pps);   /* 320: size of pps */
}

static hi_void flush_strm_header_h264(vedu_efl_enc_para *enc_para)
{
    vedu_efl_h264e_sps sps;
    vedu_efl_h264e_pps pps;

    switch (enc_para->h264_hp_en) {
        case VEDU_H264_BASELINE_PROFILE:
            sps.pro_file_idc = 66;      /* 66: pro_file_idc */
            break;
        case VEDU_H264_MAIN_PROFILE:
            sps.pro_file_idc = 77;      /* 77: pro_file_idc */
            break;
        case VEDU_H264_HIGH_PROFILE:
            sps.pro_file_idc = 100;     /* 100: pro_file_idc */
            break;
        default:
            sps.pro_file_idc = 100;     /* 100: pro_file_idc */
            break;
    }

    sps.frame_width_in_mb = (enc_para->rc.pic_width + 15) >> 4;     /* 15, 4: pic_width, bits */
    sps.frame_height_in_mb = (enc_para->rc.pic_height + 15) >> 4;   /* 15, 4: pic_width, bits */
    sps.frame_crop_left = 0;
    sps.frame_crop_top = 0;
    sps.frame_crop_right = (sps.frame_width_in_mb * 16 - enc_para->rc.pic_width) >> 1;  /* 16: frame_width_in_mb */
    sps.frame_crop_bottom = (sps.frame_height_in_mb * 16 - enc_para->rc.pic_height) >> 1; /* 16: frame_height_in_mb */
    sps.pic_order_cnt_type = enc_para->h264_poc.pic_order_cnt_type;
    sps.log2_max_frame_num_minus4 = 4;      /* 4: log2_max_frame_num_minus4 */
    enc_para->h264_sps.log2_max_frame_num_minus4 = sps.log2_max_frame_num_minus4;


    pps.chr_qp_offset = enc_para->chr_qp_offset;
    pps.const_intra = enc_para->const_intra;
    pps.h264_hp_en = (enc_para->h264_hp_en == VEDU_H264_HIGH_PROFILE) ? 1 : 0;
    pps.h264_cabac_en = enc_para->h264_cabac_en;
    pps.scale_8x8 = enc_para->scale_8x8;

    enc_para->sps_bits = h264e_make_sps(enc_para->sps_stream, 64, &sps);    /* 64: size of vps */
    enc_para->pps_bits = h264e_make_pps(enc_para->pps_stream, 320, &pps);   /* 320: size of pps */
}

hi_void venc_drv_efl_flush_strm_header(vedu_efl_enc_para *enc_para)
{
    if (enc_para == NULL) {
        HI_ERR_VENC("the ptr is NULL\n");
        return;
    }
    /* make sps & pps & VOL stream */
    if (enc_para->protocol == VEDU_H264) {
        flush_strm_header_h264(enc_para);
    } else if (enc_para->protocol == VEDU_H265) {
        hi_u32 insert;

        flush_strm_header_h265_set_para(enc_para);

        memcpy_s(enc_para->temp_vps_stream, sizeof(enc_para->temp_vps_stream),
                 enc_para->vps_stream, sizeof(enc_para->temp_vps_stream));
        memcpy_s(enc_para->temp_sps_stream, sizeof(enc_para->temp_sps_stream),
                 enc_para->sps_stream, sizeof(enc_para->temp_sps_stream));
        memcpy_s(enc_para->temp_pps_stream, sizeof(enc_para->temp_pps_stream),
                 enc_para->pps_stream, sizeof(enc_para->temp_pps_stream));
        memset_s(enc_para->vps_stream, sizeof(enc_para->vps_stream), 0, sizeof(enc_para->vps_stream));
        memset_s(enc_para->sps_stream, sizeof(enc_para->sps_stream), 0, sizeof(enc_para->sps_stream));
        memset_s(enc_para->pps_stream, sizeof(enc_para->pps_stream), 0, sizeof(enc_para->pps_stream));

        insert = check_prevention_three_byte(enc_para->temp_vps_stream,
                                             sizeof(enc_para->temp_vps_stream) / sizeof(enc_para->temp_vps_stream[0]),
                                             enc_para->vps_bits, enc_para->vps_stream,
                                             sizeof(enc_para->vps_stream) / sizeof(enc_para->vps_stream[0]));
        enc_para->vps_bits = enc_para->vps_bits + insert * 8;       /* 8: insert */
        insert = check_prevention_three_byte(enc_para->temp_sps_stream,
                                             sizeof(enc_para->temp_sps_stream) / sizeof(enc_para->temp_sps_stream[0]),
                                             enc_para->sps_bits, enc_para->sps_stream,
                                             sizeof(enc_para->sps_stream) / sizeof(enc_para->sps_stream[0]));
        enc_para->sps_bits = enc_para->sps_bits + insert * 8;       /* 8: insert */
        insert = check_prevention_three_byte(enc_para->temp_pps_stream,
                                             sizeof(enc_para->temp_pps_stream) / sizeof(enc_para->temp_pps_stream[0]),
                                             enc_para->pps_bits, enc_para->pps_stream,
                                             sizeof(enc_para->pps_stream) / sizeof(enc_para->pps_stream[0]));
        enc_para->pps_bits = enc_para->pps_bits + insert * 8;       /* 8: insert */
    }

    return;
}

static hi_void init_cfg_set_chan_para(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg)
{
    /* get channel para */
    enc_para->hal.strm_buf_addr = enc_para->stream_mmz_buf.start_phy_addr  + VEDU_MMZ_ALIGN;
    enc_para->hal.strm_buf_fd = enc_para->stream_mmz_buf.fd;
    enc_para->hal.strm_buf_size = enc_cfg->stream_buf_size - VEDU_MMZ_ALIGN;

    enc_para->vir_2_bus_offset = enc_para->stream_mmz_buf.size; /* for jpeg */

    enc_para->protocol = enc_cfg->protocol;
    enc_para->is_progressive = 1;
    enc_para->rc.pic_width = enc_cfg->frame_width;
    enc_para->rc.pic_height = enc_cfg->frame_height;

    enc_para->rotation_angle = VEDU_ROTATION_0;

    enc_para->hal.slc_split_en = enc_cfg->slc_split_en;
    enc_para->quick_encode = enc_cfg->quick_encode;
#ifdef VENC_SUPPORT_JPGE
    enc_para->q_level = enc_cfg->q_level;
    enc_para->waiting_isr_jpge = 0;
#endif
    enc_para->priority = enc_cfg->priority;
    enc_para->rc.gop = enc_cfg->gop;
    enc_para->waiting_isr = 0;
    enc_para->omx_chn = enc_cfg->omx_chn;
    enc_para->i_catch_enable = enc_cfg->auto_request_ifrm;
    enc_para->prepend_sps_pps_enable = enc_cfg->prepend_sps_pps;
    enc_para->auto_skip_frame_en = enc_cfg->rc_skip_frm_en;
    enc_para->drift_rc_skip_thr = enc_cfg->drift_rc_thr;

    /* gop */
    enc_para->gop_attr.sp_interval = enc_cfg->sp_interval;

    /* other */
    enc_para->never_enc = HI_TRUE;
    enc_para->hal.slc_split_mod = 1; /* just choose the mb line mode */
    enc_para->num_ref_index = 0;

    if (enc_para->protocol == VEDU_H264) {
        enc_para->h264_hp_en = enc_cfg->profile;
        enc_para->h264_cabac_en = (enc_cfg->profile == VEDU_H264_BASELINE_PROFILE) ? 0 : 1;
    }

    enc_para->hal.base_cfg.vcpi_ipcm_en = 0;

    enc_para->hal.base_cfg.vcpi_sao_chroma = 1 ;
    enc_para->hal.base_cfg.vcpi_sao_luma = 1 ;

    enc_para->hal.base_cfg.max_num_mergecand = 2 ;      /* 2: max_num_mergecand */
    enc_para->hal.base_cfg.tmv_en = 1;

    enc_para->hal.base_cfg.vcpi_cabac_init_idc = 0;
}

static hi_void venc_drv_efl_init_cfg(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg)
{
    init_cfg_set_chan_para(enc_para, enc_cfg);

    /* make sps & pps & VOL stream */
    venc_drv_efl_flush_strm_header(enc_para);

    /* init RC para */
    enc_para->rc.is_avbr = enc_cfg->is_avbr;
    enc_para->rc.is_intra_pic = 1;

    enc_para->rc.min_time_of_p = 3;     /* 3: min_time_of_p */
    enc_para->rc.max_time_of_p = (enc_para->rc.is_avbr) ? 200 : 6;  /* 200, 6: max_time_of_p */
    enc_para->rc.delta_time_of_p = 0;
    enc_para->rc.i_qp_delta = 2;        /* 2: i_qp_delta */
    enc_para->rc.p_qp_delta = 2;        /* 2: p_qp_delta */

    /* init stat info */
    enc_para->stat.get_stream_buf_try = 0;
    enc_para->stat.get_stream_buf_ok = 0;
    enc_para->stat.get_frame_num_try = 0;
    enc_para->stat.put_frame_num_try = 0;
    enc_para->stat.get_stream_num_try = 0;
    enc_para->stat.put_stream_num_try = 0;
    enc_para->stat.get_frame_num_ok = 0;
    enc_para->stat.put_frame_num_ok = 0;
    enc_para->stat.get_stream_num_ok = 0;
    enc_para->stat.put_stream_num_ok = 0;
    enc_para->stat.buf_full_num = 0;
    enc_para->stat.quick_encode_skip = 0;
    enc_para->stat.frm_rc_ctrl_skip = 0;
    enc_para->stat.too_few_buffer_skip = 0;
    enc_para->stat.too_many_bits_skip = 0;
    enc_para->stat.err_cfg_skip = 0;
    enc_para->stat.same_pts_skip = 0;
    enc_para->stat.stream_total_byte = 0;

    enc_para->stat.real_send_input_frm_rate = 0;
    enc_para->stat.real_send_output_frm_rate = 0;

    enc_para->stat.queue_num = 0;
    enc_para->stat.dequeue_num = 0;
    enc_para->stat.stream_queue_num = 0;
    enc_para->stat.msg_queue_num = 0;
    enc_para->stat.used_stream_buf = 0;

    enc_para->stat.total_encode_num = 0;
    enc_para->stat.total_pic_bits = 0;

    enc_para->stat.etb_cnt = 0;
    enc_para->stat.ebd_cnt = 0;
    enc_para->stat.ftb_cnt = 0;
    enc_para->stat.fbd_cnt = 0;

    /* init src info */
    enc_para->src_info.get_image = venc_drv_efl_get_image;
    enc_para->src_info.put_image = venc_drv_efl_put_image;

    enc_para->src_info.handle = HI_INVALID_HANDLE;

    enc_para->input_frm_rate_type_default = HI_VENC_FRMRATE_BUTT;
    enc_para->input_frm_rate_type_config = HI_VENC_FRMRATE_BUTT;

    enc_para->time_out = 0;
}

static hi_s32 get_write_read_ptr_buf_addr(vedu_efl_enc_para *enc_para)
{
    if (strm_alloc_write_read_ptr_buf(enc_para->strm_mng, &enc_para->wp_rp_mmz_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    memset_s(enc_para->wp_rp_mmz_buf.start_vir_addr, WRP_BUF_SIZE, 0, WRP_BUF_SIZE);
    enc_para->hal.strm_buf_rp_addr = enc_para->wp_rp_mmz_buf.start_phy_addr;
    enc_para->hal.strm_buf_wp_addr = enc_para->wp_rp_mmz_buf.start_phy_addr + 16;   /* 16: start_phy_addr */
    enc_para->strm_buf_rp_vir_addr = (hi_u32 *)enc_para->wp_rp_mmz_buf.start_vir_addr;
    enc_para->strm_buf_wp_vir_addr = (hi_u32 *)(enc_para->wp_rp_mmz_buf.start_vir_addr + 16);   /* 16: start_vir_addr */

    return HI_SUCCESS;
}

static hi_s32 get_ddr_cfg_mode_buf_addr(vedu_hal *hal, hi_u32 protocol)
{
    hi_s32 ret;
    if ((hal->reg_config_mode) && (protocol != VEDU_JPGE)) {
        hi_u32 ddr_reg_cfg_buf_size = 4096;     /* 4096: ddr_reg_cfg_buf_size */

        ret = hi_drv_venc_mmz_alloc_and_map("venc_ddr_reg_cfg_buf", 0, ddr_reg_cfg_buf_size,
                                            64, &hal->ddr_reg_cfg_buf); /* 64: align */

        if (ret != HI_SUCCESS) {
            return HI_FAILURE;
        }

        memset_s(hal->ddr_reg_cfg_buf.start_vir_addr, ddr_reg_cfg_buf_size, 0, ddr_reg_cfg_buf_size);
    }

    return HI_SUCCESS;
}

static hi_void get_rcn_and_mv_addr(vedu_hal *hal, hi_u32 luma_size, hi_u32 bus_rcn_buf,
                                   vedu_efl_enc_cfg *enc_cfg, vedu_efl_enc_para *enc_para)
{
    hi_u32 protocol = enc_cfg->protocol;
    pic_pool_buffer *pic = enc_para->pic_pool;
    pic_info_pool_buffer *pic_info = enc_para->pic_info_pool;
    hi_u32 i = 0;
    hi_u32 pic_info_addr;
    hi_u32 pic_num = enc_para->pic_num;
    hi_u32 pic_info_num = enc_para->pic_info_num;

    /* pic encode support turn 90 degrees, cap_level:1080P not only support 1920x1088 but also 1088x1920,
      so set pic_height as same as pic_width here! */
    enc_para->tmv_size = D_VENC_CALC_MV_SIZE(protocol, enc_cfg->max_width, enc_cfg->max_height);
    enc_para->pme_info_size = D_VENC_CALC_PMEINFO_SIZE(protocol, enc_cfg->max_width, enc_cfg->max_height);
    enc_para->pme_size = D_VENC_CALC_PME_SIZE(protocol, enc_cfg->max_width, enc_cfg->max_height);
    enc_para->luma_size = luma_size;


    for (i = 0; i < pic_num; i++) {
        pic[i].pic_addr = bus_rcn_buf + i * (luma_size * 3 / 2 + enc_para->rcn_block_size); /* 3, 2: pic_addr */
        pic[i].used = HI_FALSE;
        pic[i].pic_id = i;
    }

    pic_info_addr = bus_rcn_buf + (luma_size * 3 / 2 + enc_para->rcn_block_size) * i;   /* 3, 2: pic_info_addr */

    for (i = 0; i < pic_info_num; i++) {
        pic_info[i].pic_info_addr = pic_info_addr +
                            (enc_para->tmv_size + enc_para->pme_info_size + enc_para->pme_size) * i;
        pic_info[i].used = HI_FALSE;
        pic_info[i].pic_info_id = i;
    }

    hal->rcn_y_addr[0] = bus_rcn_buf;
    hal->rcn_c_addr[0] = bus_rcn_buf + luma_size;
    hal->rcn_y_addr[1] = bus_rcn_buf + luma_size * 3 / 2;   /* 3, 2: calculate rcn_y_addr */
    hal->rcn_c_addr[1] = bus_rcn_buf + luma_size * 5 / 2;   /* 5, 2: calculate rcn_c_addr */
}

static hi_u32 calc_me_mv_buffer_size(hi_u32 max_width, hi_u32 max_height, hi_u32 protocol, hi_u32 gop_type)
{
    hi_u32 pme_buf_size;
    hi_u32 pme_info_buf_size;
    hi_u32 mv_buf_size;
    hi_u32 total_size = 0;

    /* pic encode support turn 90 degrees, cap_level:1080P not only support 1920x1088 but also 1088x1920,
      so set pic_height as same as pic_width here! */
    pme_buf_size = D_VENC_CALC_PME_SIZE(protocol, max_width, max_height);
    mv_buf_size = D_VENC_CALC_MV_SIZE(protocol, max_width, max_height);
    pme_info_buf_size = D_VENC_CALC_PMEINFO_SIZE(protocol, max_width, max_height);

    if (gop_type ==  HI_VENC_GOP_MODE_NORMALP) {
        total_size = (pme_buf_size + mv_buf_size + pme_info_buf_size) * 2;      /* 2: total_size */
    } else if (gop_type == HI_VENC_GOP_MODE_DUALP || gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        total_size = (pme_buf_size + mv_buf_size + pme_info_buf_size) * 3;      /* 3: total_size */
    }

    return total_size;
}

static hi_void cfg_b_frm_cnt(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg)
{
    if (enc_para->rc.gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        if (enc_cfg->protocol == VEDU_H265) {
            enc_para->h265_ref.b_frm_cnt = enc_cfg->b_frm_num;
            enc_para->rc.b_frm_num = enc_cfg->b_frm_num;
        } else if (enc_cfg->protocol == VEDU_H264) {
            enc_para->h264_ref.b_frm_cnt = enc_cfg->b_frm_num;
            enc_para->rc.b_frm_num = enc_cfg->b_frm_num;
        }
    }
}

static hi_void cfg_gop_info(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg, vedu_hal *hal)
{
    h264e_pic *h264_pic = HI_NULL;
    hi_u32 i;

    if (g_vedu_ip_ctx.is_chip_id_v500_r001 == HI_TRUE) {
        enc_para->stream_flag = STREAM_LIST;
    } else {
        enc_para->stream_flag = STREAM_RECYCLE;
    }

    enc_para->strm_mng = (enc_para->strm_mng & 0xffff0000) | (enc_cfg->priv_handle & 0xffff);
    /* get smmu addr when SMMU support */
#ifdef HI_SMMU_SUPPORT
    hi_drv_venc_get_smmu_addr(&hal->smmu_page_base_addr, &hal->smmu_err_read_addr, &hal->smmu_err_write_addr);
#endif

    enc_para->enable_rcn_ref_share_buf = HI_FALSE;
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    if (is_support_ref_rcn_share_buf() == HI_TRUE) {
        enc_para->enable_rcn_ref_share_buf = enc_cfg->rcn_ref_share_buf;
    }
#endif

    enc_para->rc.gop_type = enc_cfg->gop_type;
    cfg_b_frm_cnt(enc_para, enc_cfg);

    if (enc_cfg->protocol == VEDU_H265) {
        enc_para->rcn_num = h265e_get_num_ref_frames(enc_cfg->gop_type);
        enc_para->pic_num = enc_para->rcn_num;
        enc_para->pic_info_num = enc_para->rcn_num;
        h265e_dpb_init(&enc_para->h265e_dpb, enc_para->rcn_num);
    } else if (enc_cfg->protocol == VEDU_H264) {
        enc_para->rcn_num = h264e_get_num_ref_frames(enc_cfg->gop_type);
        enc_para->pic_num = enc_para->rcn_num;
        enc_para->pic_info_num = enc_para->rcn_num;
        h264e_dpb_init(&enc_para->h264_dpb, enc_para->rcn_num);
        h264e_init_poc(enc_para);

        for (i = 0; i < H264E_MAX_FRM_SIZE; i++) {
            h264_pic = &enc_para->h264_pic[i];
            h264_pic->used = 0;
            h264_pic->frame_num = 0;
            h264_pic->ref = REF_NONE;
            h264_pic->pic_num = 0;
            h264_pic->pool_pic_id = HI_INVALID_HANDLE;
            h264_pic->pool_pic_info_id = HI_INVALID_HANDLE;
        }
    }
}

static hi_bool alloc_stream_and_rcn_buf(vedu_efl_enc_para *enc_para, hi_u32 *bit_buf_size, hi_u32 *strm_buf_ext_len,
                                            vedu_efl_enc_cfg *enc_cfg, hi_u32 luma_size)
{
    hi_u32 me_mv_buf_size;
    strm_mng_alloc_info buffer_alloc_info;
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    hi_u32 rcn_block_size = 0;
#endif

    /* get bit stream buffer size */
    me_mv_buf_size = calc_me_mv_buffer_size(enc_cfg->max_width, enc_cfg->max_height, enc_cfg->protocol, enc_cfg->gop_type);

    *bit_buf_size = strm_get_bit_buf_size(enc_para->strm_mng, &enc_cfg->stream_buf_size,
                                          enc_cfg->slc_split_en, luma_size, strm_buf_ext_len);
    if (*bit_buf_size == 0) {
        return HI_FAILURE;
    }
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        rcn_block_size = get_ref_rcn_share_buffer_size_ex(enc_para, enc_cfg);
        if (rcn_block_size == 0) {
            return HI_FAILURE;
        }

        enc_para->pic_num--;
    }
#endif

    /* malloc stream buffer */
    memset_s(&buffer_alloc_info, sizeof(strm_mng_alloc_info), 0, sizeof(strm_mng_alloc_info));
    buffer_alloc_info.protocol = enc_cfg->protocol;
    buffer_alloc_info.bit_buf_size = *bit_buf_size;
    buffer_alloc_info.luma_size = luma_size;
    buffer_alloc_info.me_mv_size = me_mv_buf_size;
    buffer_alloc_info.rcn_size = luma_size * 3 / 2 * enc_para->pic_num; /* 3, 2: rcn_size */
#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        buffer_alloc_info.rcn_block_size = rcn_block_size * enc_para->pic_num;
    }
#endif
    if (strm_alloc_stream_buf(enc_para->strm_mng, &enc_para->stream_mmz_buf,
                              &buffer_alloc_info, enc_para->enable_rcn_ref_share_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

enum {
    CREATE_ENV_ERROR_1 = -1,
    CREATE_ENV_ERROR_2 = -2,
    CREATE_ENV_ERROR_3 = -3,
    CREATE_ENV_ERROR_4 = -4,
    CREATE_ENV_ERROR_5 = -5,
    CREATE_ENV_ERROR_6 = -6,
    CREATE_ENV_ERROR_7 = -7,
    CREATE_ENV_ERROR_8 = -8
};

static hi_s32 mng_queue_init(hi_bool omx_chn, vedu_efl_enc_para *enc_para)
{
    if (omx_chn) {
        enc_para->frame_queue_omx = venc_drv_mng_queue_init(MAX_VEDU_QUEUE_NUM, HI_TRUE, HI_FALSE);

        if (enc_para->frame_queue_omx == HI_NULL) {
            HI_ERR_VENC("failed to init frame_queue, size = %d\n", sizeof(queue_info));
            return CREATE_ENV_ERROR_5;
        }

        enc_para->stream_queue_omx = venc_drv_mng_queue_init(MAX_VEDU_STRM_QUEUE_NUM, HI_TRUE, HI_FALSE);

        if (enc_para->stream_queue_omx == HI_NULL) {
            HI_ERR_VENC("failed to init stream_queue, size = %d\n", sizeof(queue_info));
            return CREATE_ENV_ERROR_6;
        }

        enc_para->msg_queue_omx = venc_drv_mng_queue_init(MSG_QUEUE_NUM, HI_TRUE, HI_TRUE);

        if (enc_para->msg_queue_omx == HI_NULL) {
            HI_ERR_VENC("failed to init msg_queue, size = %d\n", sizeof(queue_info));
            return CREATE_ENV_ERROR_7;
        }
    } else {
        enc_para->frame_queue = venc_drv_mng_queue_init(MAX_VEDU_QUEUE_NUM, HI_FALSE, HI_FALSE);

        if (enc_para->frame_queue == HI_NULL) {
            HI_ERR_VENC("failed to init frame_queue, size = %d\n", sizeof(queue_info));
            return CREATE_ENV_ERROR_5;
        }

        enc_para->frame_dequeue = venc_drv_mng_queue_init(MAX_VEDU_QUEUE_NUM, HI_FALSE, HI_FALSE);

        if (enc_para->frame_dequeue == HI_NULL) {
            HI_ERR_VENC("failed to init frame_dequeue, size = %d\n", sizeof(queue_info));
            return CREATE_ENV_ERROR_6;
        }
    }

    return HI_SUCCESS;
}

static hi_void mng_queue_deinit(hi_bool omx_chn, queue_info *omx_queue, queue_info *queue)
{
    if (omx_chn) {
        if (omx_queue == HI_NULL) {
            return;
        }
        if (venc_drv_mng_queue_deinit(omx_queue) != HI_SUCCESS) {
            HI_ERR_VENC("HI_VFREE failed to free queue_omx, size = %d\n", sizeof(queue_info));
        }
    } else {
        if (queue == HI_NULL) {
            return;
        }
        if (venc_drv_mng_queue_deinit(queue) != HI_SUCCESS) {
            HI_ERR_VENC("HI_VFREE failed to free queue, size = %d\n", sizeof(queue_info));
        }
    }
}

static hi_s32 alloc_strm_and_para_buf(vedu_efl_enc_para *enc_para, hi_u32 *strm_buf_ext_len, vedu_efl_enc_cfg *enc_cfg,
                                       hi_u32 luma_size)
{
    hi_u32 bit_buf_size;
    hi_u32 bus_rcn_buf; /* 16 aligned, = 2.0 frame mb-yuv */
    vedu_hal *hal = &enc_para->hal;

    /* get stream manange handle */
    if (strm_get_match_mode(enc_cfg->protocol, &enc_para->strm_mng, g_vedu_ip_ctx.is_chip_id_v500_r001) != HI_SUCCESS) {
        HI_ERR_VENC("get STRM match mode failed\n");
        return CREATE_ENV_ERROR_1;
    }

    cfg_gop_info(enc_para, enc_cfg, hal);

    if (alloc_stream_and_rcn_buf(enc_para, &bit_buf_size, strm_buf_ext_len, enc_cfg, luma_size) != HI_SUCCESS) {
        return CREATE_ENV_ERROR_2;
    }

    /* get write/read ptr buffer */
    if (get_write_read_ptr_buf_addr(enc_para) != HI_SUCCESS) {
        return CREATE_ENV_ERROR_2;
    }

    /* get parameter set buffer */
    if (strm_alloc_para_set_buf_addr(enc_para->strm_mng, &hal->slice_buf, &hal->para_set_phy_addr) != HI_SUCCESS) {
        return CREATE_ENV_ERROR_3;
    }

    /* get DDR cfg mode buffer addr */
    hal->reg_config_mode = VENC_REG_CONFIG_MODE;

    if (get_ddr_cfg_mode_buf_addr(hal, enc_cfg->protocol) != HI_SUCCESS) {
        return CREATE_ENV_ERROR_4;
    }

    /* arrange_chn_buf -> rcn & bits */
    bus_rcn_buf = enc_para->stream_mmz_buf.start_phy_addr + bit_buf_size;
    get_rcn_and_mv_addr(hal, luma_size, bus_rcn_buf, enc_cfg, enc_para);

#ifdef CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT
    if (enc_para->enable_rcn_ref_share_buf == HI_TRUE) {
        get_rcn_ref_share_buf(enc_para, enc_para->rcn_num - 1);
        enc_para->rcn_ref_share_buf.active_num_ref_frames = enc_para->rcn_num - 1;
        init_rcn_ref_share_buff_param(enc_para);
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 create_strm_inst(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg,
                                           strm_mng_stream_info *stream_info, hi_u32 strm_buf_ext_len)
{
    /* give cfg info to encpara */
    venc_drv_efl_init_cfg(enc_para, enc_cfg);

    /* create inst, get handle num */
    set_stream_info(enc_para, stream_info);
    stream_info->strm_buf_ext_len = strm_buf_ext_len;
    if (strm_create(enc_para->strm_mng, stream_info) != HI_SUCCESS) {
        return CREATE_ENV_ERROR_8;
    }

    return HI_SUCCESS;
}

static vedu_efl_enc_para *alloc_venc_instance(vedu_efl_enc_cfg *enc_cfg)
{
    vedu_efl_enc_para *enc_para = HI_NULL;

    /* malloc encoder parameter */
    enc_para = (vedu_efl_enc_para *)HI_VMALLOC(HI_ID_VENC, sizeof(vedu_efl_enc_para));
    if (enc_para == HI_NULL) {
        HI_ERR_VENC("HI_VMALLOC failed, size = %d\n", sizeof(vedu_efl_enc_para));
        return HI_NULL;
    }
    memset_s(enc_para, sizeof(vedu_efl_enc_para), 0, sizeof(vedu_efl_enc_para));

    return enc_para;
}

static hi_s32 cfg_venc_strm_and_queue(vedu_efl_enc_para *enc_para, vedu_efl_enc_cfg *enc_cfg,
                                             hi_u32 luma_size, strm_mng_stream_info *stream_info)
{
    hi_s32 ret;
    hi_u32 strm_buf_ext_len;

    ret = alloc_strm_and_para_buf(enc_para, &strm_buf_ext_len, enc_cfg, luma_size);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /************************************* add ******************************************/
    ret = mng_queue_init(enc_cfg->omx_chn, enc_para);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = create_strm_inst(enc_para, enc_cfg, stream_info, strm_buf_ext_len);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

#define GOTO_LABLE(ret) do {        \
        if ((ret) == CREATE_ENV_ERROR_1) goto error_1;        \
        else if ((ret) == CREATE_ENV_ERROR_2) goto error_2;   \
        else if ((ret) == CREATE_ENV_ERROR_3) goto error_3;   \
        else if ((ret) == CREATE_ENV_ERROR_4) goto error_4;   \
        else if ((ret) == CREATE_ENV_ERROR_5) goto error_5;   \
        else if ((ret) == CREATE_ENV_ERROR_6) goto error_6;   \
        else if ((ret) == CREATE_ENV_ERROR_7) goto error_7;   \
        else if ((ret) == CREATE_ENV_ERROR_8) goto error_8;   \
    } while (0)


hi_s32 venc_drv_efl_create_chan(vedu_efl_enc_para **enc_handle, vedu_efl_enc_cfg *enc_cfg)
{
    vedu_efl_enc_para *enc_para = HI_NULL;
    hi_u32 luma_size;
    vedu_hal *hal = HI_NULL;
    hi_s32 ret;

    strm_mng_stream_info stream_info;

    D_VENC_CHECK_PTR(enc_handle);
    D_VENC_CHECK_PTR(enc_cfg);

    /* get config info */
    memset_s(&stream_info, sizeof(strm_mng_stream_info), 0, sizeof(strm_mng_stream_info));

    /* get current luma size, Note: max_height must be 16 aligned, otherwise purple mosaic may appear */
    luma_size = D_VENC_ALIGN_UP(enc_cfg->max_width, VEDU_MMZ_ALIGN) *
        D_VENC_ALIGN_UP(enc_cfg->max_height, LUMA_HEIGHT_ALIGH);

    enc_para = alloc_venc_instance(enc_cfg);
    if (enc_para == HI_NULL) {
        goto error_0;
    }

    ret = cfg_venc_strm_and_queue(enc_para, enc_cfg, luma_size, &stream_info);
    GOTO_LABLE(ret);

    /* get return val */
    *enc_handle = enc_para;

    return HI_SUCCESS;

error_8:
    mng_queue_deinit(enc_cfg->omx_chn, enc_para->msg_queue_omx, enc_para->frame_dequeue);

error_7:
    mng_queue_deinit(enc_cfg->omx_chn, enc_para->stream_queue_omx, HI_NULL);

error_6:
    mng_queue_deinit(enc_cfg->omx_chn, enc_para->frame_queue_omx, enc_para->frame_queue);

error_5:
    hi_drv_venc_mmz_unmap_and_release(&hal->ddr_reg_cfg_buf);
error_4:
    if (strm_rls_para_set_buf_addr(enc_para->strm_mng, &hal->slice_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }
error_3:
    if (strm_rls_write_read_ptr_buf(enc_para->strm_mng, &enc_para->wp_rp_mmz_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }
error_2:
    if (strm_rls_stream_buf(enc_para->strm_mng, &enc_para->stream_mmz_buf, enc_cfg->protocol) != HI_SUCCESS) {
        return HI_FAILURE;
    }
error_1:
    HI_VFREE(HI_ID_VENC, enc_para);
error_0:
    return HI_FAILURE;
}

static hi_bool destroy_venc_mng_queue_deinit(vedu_efl_enc_para *enc_para)
{
    venc_buffer m_buf_venc_temp = {0};
    hi_u32 i;

    if (enc_para->omx_chn) {
        for (i = 0; i < OMX_OUTPUT_PORT_BUFFER_MAX;i++) {
            if (enc_para->omx_output_buf[i].start_vir_addr != 0) {
                m_buf_venc_temp.start_vir_addr = enc_para->omx_output_buf[i].start_vir_addr;
                m_buf_venc_temp.dma_buf = enc_para->omx_output_buf[i].dma_buf;
                hi_drv_venc_unmap(&m_buf_venc_temp);
            }
        }

        if (venc_drv_mng_queue_deinit(enc_para->stream_queue_omx)) {
            HI_ERR_VENC("HI_VFREE failed to free stream_queue_omx, size = %d\n", sizeof(queue_info));
            return HI_FAILURE;
        }

        if (venc_drv_mng_queue_deinit(enc_para->frame_queue_omx)) {
            HI_ERR_VENC("HI_VFREE failed to free frame_queue_omx, size = %d\n", sizeof(queue_info));
            return HI_FAILURE;
        }

        if (venc_drv_mng_queue_deinit(enc_para->msg_queue_omx)) {
            HI_ERR_VENC("HI_VFREE failed to free msg_queue_omx, size = %d\n", sizeof(queue_info));
            return HI_FAILURE;
        }

    } else {
        if (venc_drv_mng_queue_deinit(enc_para->frame_queue)) {
            HI_ERR_VENC("HI_VFREE failed to free frame_queue, size = %d\n", sizeof(queue_info));
            return HI_FAILURE;
        }
        if (venc_drv_mng_queue_deinit(enc_para->frame_dequeue)) {
            HI_ERR_VENC("HI_VFREE failed to free frame_dequeue, size = %d\n", sizeof(queue_info));
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_efl_destroy_venc(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    if (enc_para == NULL) {
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    /* Needn't to close fd when abnormal exit */
    if (enc_para->abnormal_exit == HI_TRUE) {
        enc_para->stream_mmz_buf.skip_close_fd = HI_TRUE;
        enc_para->wp_rp_mmz_buf.skip_close_fd = HI_TRUE;
        enc_para->extra_rcn_buf.skip_close_fd = HI_TRUE;
        enc_para->hal.ddr_reg_cfg_buf.skip_close_fd = HI_TRUE;
        enc_para->hal.slice_buf.skip_close_fd = HI_TRUE;
    }

    if (strm_rls_stream_buf(enc_para->strm_mng, &enc_para->stream_mmz_buf, enc_para->protocol) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (strm_rls_write_read_ptr_buf(enc_para->strm_mng, &enc_para->wp_rp_mmz_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (enc_para->extra_rcn_flag) {
        hi_drv_venc_mmz_unmap_and_release(&enc_para->extra_rcn_buf);
    }

    if (enc_para->hal.reg_config_mode) {
        hi_drv_venc_mmz_unmap_and_release(&enc_para->hal.ddr_reg_cfg_buf);
    }

    if (strm_destroy(enc_para->strm_mng) != HI_SUCCESS) {
        HI_ERR_VENC("STRMMNG_Destroy failed!");
    }

    if (strm_rls_para_set_buf_addr(enc_para->strm_mng, &enc_para->hal.slice_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (destroy_venc_mng_queue_deinit(enc_para)) {
        return HI_FAILURE;
    }

    HI_VFREE(HI_ID_VENC, enc_para);

    return HI_SUCCESS;
}

hi_s32 venc_drv_efl_attach_input(vedu_efl_enc_para *enc_handle, vedu_efl_src_info *src_info)
{
    hi_u32 chn_id = 0;
    vedu_efl_enc_para  *enc_para = HI_NULL;

    D_VENC_GET_CHN(chn_id, enc_handle);
    D_VENC_CHECK_CHN(chn_id);
    D_VENC_CHECK_PTR(src_info);

    enc_para = enc_handle;

    if (enc_para->omx_chn) {
        if (!src_info->get_image_omx) {
            return HI_FAILURE;
        }
    } else {
        if ((!src_info->get_image) || (!src_info->put_image) || (!src_info->change_info)) {
            return HI_FAILURE;
        }
    }

    enc_para->src_info = *src_info;

    return HI_SUCCESS;
}

hi_s32 venc_drv_efl_detach_input(vedu_efl_enc_para *enc_handle, vedu_efl_src_info *src_info)
{
    hi_u32 chn_id = 0;
    vedu_efl_enc_para  *enc_para = HI_NULL;
    D_VENC_GET_CHN(chn_id, enc_handle);
    D_VENC_CHECK_CHN(chn_id);
    D_VENC_CHECK_PTR(src_info);

    enc_para = enc_handle;
    enc_para->src_info = *src_info;

    enc_para->input_frm_rate_type_default = HI_VENC_FRMRATE_USER;

    return HI_SUCCESS;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_void venc_drv_efl_request_i_frame(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    if (enc_para == HI_NULL) {
        HI_ERR_VENC("enc_para = NULL\n");
        return;
    }
    enc_para->inter_frm_cnt = enc_para->rc.gop - 1;

    enc_para->rc.i_frm_insert_flag = 1;
    enc_para->rc.rc_start = 1;

    return;
}

static hi_s32 venc_drv_efl_rc_get_attr(hi_venc_chan_info *chn_user_cfg, vedu_efl_enc_para *enc_handle,
                                       vedu_efl_rc_attr *rc_attr)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    rc_attr->bit_rate = enc_para->rc.bit_rate;
    rc_attr->out_frm_rate = enc_para->rc.vo_frm_rate;
    rc_attr->in_frm_rate = chn_user_cfg->config.input_frame_rate;
    rc_attr->max_qp = enc_para->rc.max_qp;
    rc_attr->min_qp = enc_para->rc.min_qp;

    return HI_SUCCESS;
}

#ifdef VENC_SUPPORT_JPGE
hi_s32 venc_drv_efl_alloc_buf_2_jpge(vedu_efl_enc_para *enc_handle, hi_u32 max_width, hi_u32 max_height)
{
    hi_s32 ret;
    hi_u32 size;
    vedu_efl_enc_para  *enc_para = enc_handle;
    if (enc_para == NULL) {
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    size = ALIGN_UP(max_width * max_height * 3 / 2, 64);  /* 64: stream buffer align size 3/2: 1.5 times */

    ret = hi_drv_venc_alloc_and_map("venc_jpeg_out_buf", 0, size, 64, &enc_para->jpg_mmz_buf);  /* 64: align */
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("hi_drv_venc_alloc_and_map failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void venc_drv_efl_free_buf_2_jpge(vedu_efl_enc_para *enc_handle)
{
    vedu_efl_enc_para *enc_para = enc_handle;
    if (enc_para == NULL) {
        HI_ERR_VENC("the ptr is NULL\n");
        return;
    }

    hi_drv_venc_unmap_and_release(&enc_para->jpg_mmz_buf);

    return;
}
#endif

hi_void rc_attr_init_set_enc_para(vedu_efl_enc_para *enc_para, vedu_efl_rc_attr *rc_attr)
{
    enc_para->rc.bit_rate = rc_attr->bit_rate;
    enc_para->rc.vo_frm_rate = rc_attr->out_frm_rate;
    enc_para->rc.vi_frm_rate = rc_attr->in_frm_rate;
    enc_para->rc.min_still_percent = 5;     /* 5: min_still_percent */
    enc_para->rc.max_still_qp = 30;         /* 30: max_still_qp */
    if (enc_para->rc.is_avbr) {
        enc_para->rc.max_qp = 48;           /* 48: max_qp */
        enc_para->rc.min_qp = 24;           /* 24: min_qp */

        if (enc_para->rc.pic_width <= 720 && enc_para->rc.pic_height <= 576) {  /* 720, 576: pic_width, pic_height */
            enc_para->rc.max_qp = 48;       /* 48: max_qp */
            enc_para->rc.min_qp = 20;       /* 20: min_qp */
        }
    } else {
        enc_para->rc.max_qp = rc_attr->max_qp;
        enc_para->rc.min_qp = rc_attr->min_qp;
    }
}

hi_s32 venc_drv_efl_rc_attr_init(vedu_efl_enc_para *enc_handle, vedu_efl_rc_attr *rc_attr)
{
    vedu_efl_enc_para *enc_para = NULL;
    hi_u32 i;
    hi_u32 tr_count_temp = 0;
    hi_u32 *tr_count = NULL;

    D_VENC_CHECK_PTR(enc_handle);
    D_VENC_CHECK_PTR(rc_attr);

    enc_para = enc_handle;

    if ((rc_attr->in_frm_rate > HI_VENC_MAX_fps) || (rc_attr->in_frm_rate < rc_attr->out_frm_rate)) {
        return HI_FAILURE;
    }

    if (rc_attr->out_frm_rate == 0) {
        return HI_FAILURE;
    }

    if (enc_para->rc.vo_frm_rate != rc_attr->out_frm_rate || enc_para->rc.vi_frm_rate != rc_attr->in_frm_rate) {
        tr_count = &enc_para->tr_count;
    } else {
        tr_count = &tr_count_temp;
    }

    rc_attr_init_set_enc_para(enc_para, rc_attr);

    /* initialize frame rate control parameter */
    venc_drv_fifo_info(&enc_para->valg_bits_fifo, enc_para->bits_fifo,
                       sizeof(enc_para->bits_fifo) / sizeof(enc_para->bits_fifo[0]),
                       enc_para->rc.vi_frm_rate, enc_para->rc.bit_rate / enc_para->rc.vi_frm_rate);

    enc_para->mean_bit = enc_para->rc.bit_rate / enc_para->rc.vo_frm_rate;
    *tr_count = enc_para->rc.vi_frm_rate;

    for (i = 0; i < enc_para->rc.vi_frm_rate; i++) {
        *tr_count += enc_para->rc.vo_frm_rate;

        if (*tr_count > enc_para->rc.vi_frm_rate) {
            *tr_count -= enc_para->rc.vi_frm_rate;
            venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, enc_para->mean_bit);
        } else {
            venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, 0);
        }
    }

    /* initialize re-start parameter */
    enc_para->rc.rc_start = 1;

    return HI_SUCCESS;
}

static hi_s32 frm_rate_ctrl_skip_frm(vedu_efl_enc_para *enc_para, hi_u32 tr)
{
    hi_u32 diff_tr = (tr - enc_para->last_tr);
    hi_u32 i;

    /* don't run too many loop */
    if (diff_tr < VEDU_TR_STEP) {
        return HI_FAILURE;
    } else if (diff_tr > 0x1f) {
        diff_tr = 0x1f;
    }

    /* LOST frames into fifo */
    for (i = 0; i < diff_tr - VEDU_TR_STEP; i += VEDU_TR_STEP) {
        enc_para->tr_count += enc_para->rc.vo_frm_rate;
        venc_drv_fifo_write(&enc_para->valg_bits_fifo, 0);
    }

    /* this frame */
    enc_para->tr_count += enc_para->rc.vo_frm_rate;
    enc_para->last_tr = tr;

    /* don't care too many lost frames */
    if (enc_para->tr_count > enc_para->rc.vo_frm_rate + enc_para->rc.vi_frm_rate) {
        enc_para->tr_count = enc_para->rc.vo_frm_rate + enc_para->rc.vi_frm_rate;
    }

    /* skip this frame */ /* too many bits */ /* rc_threshold = 20% */
    if ((enc_para->tr_count <= enc_para->rc.vi_frm_rate) ||                         /* time to skip */
        (enc_para->valg_bits_fifo.sum >
            (enc_para->rc.bit_rate / 100 * (enc_para->drift_rc_skip_thr + 100)))) { /* 100: bit_rate */
        if (enc_para->tr_count <= enc_para->rc.vi_frm_rate) {
            enc_para->stat.frm_rc_ctrl_skip++;
        } else {
            if ((!enc_para->omx_chn) && (enc_para->auto_skip_frame_en)) {
                enc_para->stat.too_many_bits_skip++;
            } else {
                return HI_SUCCESS;
            }

        }
        venc_drv_fifo_write(&enc_para->valg_bits_fifo, 0);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void frm_rate_ctrl_get_pic_type(vedu_efl_enc_para *enc_para)
{
    if (enc_para->rc.gop_type == HI_VENC_GOP_MODE_DUALP) {
        if (enc_para->gop_attr.sp_interval == 0) {
            if (enc_para->inter_frm_cnt == 0) {
                enc_para->rc.pic_type = PIC_INTER;
            } else {
                enc_para->rc.pic_type = PIC_INTERSP;
            }
        } else {
            if ((enc_para->inter_frm_cnt % enc_para->gop_attr.sp_interval) ==
                (enc_para->gop_attr.sp_interval - 1)) {
                enc_para->rc.pic_type = PIC_INTERBP;
            } else if (enc_para->inter_frm_cnt % enc_para->gop_attr.sp_interval == 0) {
                enc_para->rc.pic_type = PIC_INTER;
            } else {
                enc_para->rc.pic_type = PIC_INTERSP;
            }
        }
    } else if (enc_para->rc.gop_type == HI_VENC_GOP_MODE_BIPREDB) {
        if ((enc_para->inter_frm_cnt % (enc_para->rc.b_frm_num + 1)) != 0) {
            enc_para->rc.pic_type = PIC_BIINTER;
        } else {
            enc_para->rc.pic_type = PIC_INTERSP;
        }
    } else if (enc_para->rc.gop_type == HI_VENC_GOP_MODE_NORMALP) {
        enc_para->rc.pic_type = PIC_INTER;
    }
    enc_para->rc.is_intra_pic = 0;
    enc_para->rc.frm_num_in_gop = enc_para->inter_frm_cnt + 1;
}

static hi_s32 venc_drv_efl_rc_frm_rate_ctrl(vedu_efl_enc_para *enc_handle, hi_u32 tr)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    if (enc_para->rc.rc_start) {
        enc_para->tr_count = enc_para->rc.vi_frm_rate;
        enc_para->tr_count += enc_para->rc.vo_frm_rate;
        enc_para->last_tr = tr;
        enc_para->rc.is_intra_pic = 1;
        enc_para->rc.pic_type = PIC_INTRA;
        enc_para->frm_cnt = 0;
    } else {
        if (frm_rate_ctrl_skip_frm(enc_para, tr) != HI_SUCCESS) {
            return HI_FAILURE;
        }

        /* intra or inter based gop */
        if (enc_para->inter_frm_cnt >= enc_para->rc.gop - 1) {
            enc_para->rc.is_intra_pic = 1;
            enc_para->rc.pic_type = PIC_INTRA;
            enc_para->frm_cnt = 0;
            enc_para->rc.frm_num_in_gop = 0;
        } else {
            frm_rate_ctrl_get_pic_type(enc_para);
        }
    }

    return HI_SUCCESS;
}

hi_s32 venc_drv_efl_rc_set_attr(vedu_efl_enc_para *enc_handle, vedu_efl_rc_attr *rc_attr)
{
    vedu_efl_enc_para *enc_para = NULL;
    hi_u32 i;
    hi_u32 tr_count_temp = 0;
    hi_u32 *tr_count = NULL;

    D_VENC_CHECK_PTR(enc_handle);
    D_VENC_CHECK_PTR(rc_attr);

    enc_para = enc_handle;

    if ((rc_attr->in_frm_rate > HI_VENC_MAX_fps) || (rc_attr->in_frm_rate < rc_attr->out_frm_rate)) {
        return HI_FAILURE;
    }

    if (rc_attr->out_frm_rate == 0) {
        return HI_FAILURE;
    }

    if (enc_para->rc.vo_frm_rate != rc_attr->out_frm_rate || enc_para->rc.vi_frm_rate != rc_attr->in_frm_rate) {
        tr_count = &enc_para->tr_count;
    } else {
        tr_count = &tr_count_temp;
    }

    /* updata the RC structure */
    enc_para->rc.average_frame_bits = rc_attr->bit_rate / rc_attr->out_frm_rate;
    enc_para->rc.inst_bits_len = rc_attr->out_frm_rate;

    if (rc_attr->gop <= (enc_para->rc.frm_num_in_gop + 1)) {
        venc_drv_efl_request_i_frame(enc_para);
        goto init_process;
    }

init_process:

    /* initialize  parameter */
    enc_para->rc.bit_rate = rc_attr->bit_rate;
    enc_para->rc.vo_frm_rate = rc_attr->out_frm_rate;
    enc_para->rc.vi_frm_rate = rc_attr->in_frm_rate;
    enc_para->rc.gop = rc_attr->gop;
    *tr_count = enc_para->rc.vi_frm_rate;

    /* updata the RC strm_buffer */
    venc_drv_fifo_info(&enc_para->valg_bits_fifo, enc_para->bits_fifo,
                       sizeof(enc_para->bits_fifo) / sizeof(enc_para->bits_fifo[0]),
                       enc_para->rc.vi_frm_rate, enc_para->rc.bit_rate / enc_para->rc.vi_frm_rate);

    enc_para->mean_bit = enc_para->rc.bit_rate / enc_para->rc.vo_frm_rate;

    for (i = 0; i < enc_para->rc.vi_frm_rate; i++) {
        *tr_count += enc_para->rc.vo_frm_rate;

        if (*tr_count > enc_para->rc.vi_frm_rate) {
            *tr_count -= enc_para->rc.vi_frm_rate;
            venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, enc_para->mean_bit);
        } else {
            venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, 0);
        }
    }

    return HI_SUCCESS;
}

#if 0
static hi_void venc_drv_efl_intra_frm_rate_change(vedu_efl_enc_para *enc_para, hi_u32 new_vi_frm_rate,
                                                  hi_venc_chan_info src_user_cfg)
{
    hi_u32 i;
    enc_para->vi_frm_rate = new_vi_frm_rate;
    if (src_user_cfg.target_frame_rate > enc_para->vi_frm_rate) {
        enc_para->vo_frm_rate = enc_para->vi_frm_rate;

    } else {
        enc_para->vo_frm_rate = src_user_cfg.target_frame_rate;
    }

    /* updata the RC structure */
    enc_para->rc.average_frame_bits = enc_para->bit_rate / enc_para->vo_frm_rate;

    enc_para->rc.gop_bits_left = enc_para->rc.average_frame_bits * (enc_para->gop - enc_para->rc.b_frm_num_in_gop - 1);

    enc_para->rc.total_bits_left += enc_para->rc.gop_bits_left;

    /* initialize  parameter */
    enc_para->tr_count = enc_para->vi_frm_rate;

    /* updata the RC strm_buffer */
    venc_drv_fifo_info(&enc_para->valg_bits_fifo, enc_para->bits_fifo,
                       sizeof(enc_para->bits_fifo) / sizeof(enc_para->bits_fifo[0]),
                       enc_para->vi_frm_rate, enc_para->bit_rate / enc_para->vi_frm_rate);

    enc_para->mean_bit = enc_para->bit_rate / enc_para->vo_frm_rate;

    for (i = 0; i < enc_para->vi_frm_rate; i++) {
        enc_para->tr_count += enc_para->vo_frm_rate;

        if (enc_para->tr_count > enc_para->vi_frm_rate) {
            enc_para->tr_count -= enc_para->vi_frm_rate;
            venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, enc_para->mean_bit);
        } else {
            venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, 0);
        }
    }

    return;
}
#endif

/******************************************************************************
function   :
description: 获取尽量可靠的输入帧率 与对应的输出帧率 并做相应的处理
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
static hi_u32 get_frm_rate_auto_detection(vedu_efl_enc_para *enc_para)
{
    hi_u32 diff1 = 0;
    hi_u32 new_vi_frm_rate = 0;
    hi_u32 max1, max2;

    enc_para->rc.last_sec_frame_cnt++;

    /* If the first 6 frames are not yet full, the frame rate that has been believed to be the largest */
    /* CNcomment: 前6帧还没有满，一直相信最大的那个帧率 */
    if (enc_para->last_frm_rate[5] == 0) {  /* 5: index */
        max1 = D_VENC_RC_MAX3(enc_para->last_frm_rate[0], enc_para->last_frm_rate[1],
                              enc_para->last_frm_rate[2]);  /* 2: index */
        max2 = D_VENC_RC_MAX3(enc_para->last_frm_rate[3], enc_para->last_frm_rate[4],
                              enc_para->last_frm_rate[5]);  /* 4, 5: index */
        new_vi_frm_rate = D_VENC_RC_MAX(max1, max2);
    } else {
        if (enc_para->rc.frm_rate_sata_error_flag) {
            new_vi_frm_rate = enc_para->rc.vi_frm_rate;
            /* The first 2 second frame rate is counted. */
            /* CNcomment: 前2s帧率都统计出来了 */
        } else if (enc_para->last_sec_input_frm_rate[1] != 0) {
            diff1 = D_VENC_RC_ABS(enc_para->last_sec_input_frm_rate[0], enc_para->last_sec_input_frm_rate[1]);
            if (diff1 <= 3) {   /* 3: diff1 */
                new_vi_frm_rate = D_VENC_RC_MAX(enc_para->last_sec_input_frm_rate[0],
                                                enc_para->last_sec_input_frm_rate[1]);
            } else {
                new_vi_frm_rate = enc_para->last_sec_input_frm_rate[0];
            }
            /* The first 1 second frame rate is counted. */
            /* CNcomment: 前1s帧率都统计出来了 */
        } else if (enc_para->last_sec_input_frm_rate[0] != 0) {
            new_vi_frm_rate = enc_para->last_sec_input_frm_rate[0];
            /* The frame rate of the first second has not been counted yet. */
            /* CNcomment: 第一s的帧率还没有统计出来 */
        } else {
            max1 = D_VENC_RC_MAX3(enc_para->last_frm_rate[0], enc_para->last_frm_rate[1],
                                  enc_para->last_frm_rate[2]);  /* 2: index */
            max2 = D_VENC_RC_MAX3(enc_para->last_frm_rate[3], enc_para->last_frm_rate[4],
                                  enc_para->last_frm_rate[5]);  /* 3, 4, 5: index */
            new_vi_frm_rate = D_VENC_RC_MAX(max1, max2);
        }
    }

    if (enc_para->rc.last_sec_frame_cnt >= (1 + new_vi_frm_rate)) {
        if (enc_para->last_frm_rate[0] > new_vi_frm_rate) {
            new_vi_frm_rate = enc_para->last_frm_rate[0] +
                (enc_para->last_frm_rate[0] - new_vi_frm_rate) + 6;  /* 6: new_vi_frm_rate */
            enc_para->rc.frm_rate_sata_error_flag = 1;
        } else if (new_vi_frm_rate < enc_para->rc.last_sec_frame_cnt) {
            new_vi_frm_rate = enc_para->rc.last_sec_frame_cnt + 15;     /* 15: new_vi_frm_rate */
            enc_para->rc.frm_rate_sata_error_flag = 1;
        }
    }

    return new_vi_frm_rate;
}

static hi_u32 get_frm_rate_user_config(vedu_efl_enc_para *enc_para, hi_u32 chn_id)
{
    hi_u32 new_vi_frm_rate;

    new_vi_frm_rate = g_venc_chn[chn_id].chn_user_cfg.config.input_frame_rate;

    enc_para->rc.last_sec_frame_cnt_for_usr = enc_para->rc.rc_start ? 0 : enc_para->rc.last_sec_frame_cnt_for_usr + 1;

    if (enc_para->rc.last_sec_frame_cnt_for_usr >= new_vi_frm_rate) {
        enc_para->rc.last_sec_frame_cnt_for_usr = 0;
    }

    return new_vi_frm_rate;
}

static hi_u32 get_frm_rate_from_stream(vedu_efl_enc_para *enc_para)
{
    hi_u32 new_vi_frm_rate;

    new_vi_frm_rate = enc_para->last_frm_rate[0];

    return new_vi_frm_rate;
}

static hi_void update_input_frm_rate(vedu_efl_enc_para *enc_para, hi_u32 new_vi_frm_rate,
                                     hi_venc_chan_info *src_user_cfg)
{
    hi_u32 i = 0;

    if ((new_vi_frm_rate != enc_para->rc.vi_frm_rate) && (new_vi_frm_rate <= FRM_RATE_CNT_NUM)) {
        if (new_vi_frm_rate >= FRM_RATE_CNT_NUM) {
            new_vi_frm_rate = FRM_RATE_CNT_NUM;   /* fifo buffer range */
        }

        enc_para->rc.vi_frm_rate = new_vi_frm_rate;

        if (src_user_cfg->config.target_frame_rate > enc_para->rc.vi_frm_rate) {
            enc_para->rc.vo_frm_rate = enc_para->rc.vi_frm_rate;
        } else {
            enc_para->rc.vo_frm_rate = src_user_cfg->config.target_frame_rate;
        }

        /* updata the RC structure */
        if (enc_para->rc.frm_rate_sata_error_flag) {   /* error! */
            enc_para->rc.average_frame_bits = enc_para->rc.bit_rate / \
                (enc_para->rc.vo_frm_rate + 8); /* 8: vo_frm_rate */
        } else {
            enc_para->rc.average_frame_bits = enc_para->rc.bit_rate / enc_para->rc.vo_frm_rate;
        }

        /* initialize  parameter */
        enc_para->tr_count = enc_para->rc.vi_frm_rate;

        /* updata the RC strm_buffer */
        venc_drv_fifo_info(&enc_para->valg_bits_fifo, enc_para->bits_fifo,
                           sizeof(enc_para->bits_fifo) / sizeof(enc_para->bits_fifo[0]),
                           enc_para->rc.vi_frm_rate, enc_para->rc.bit_rate / enc_para->rc.vi_frm_rate);

        enc_para->mean_bit = enc_para->rc.bit_rate / enc_para->rc.vo_frm_rate;

        for (i = 0; i < enc_para->rc.vi_frm_rate; i++) {
            enc_para->tr_count += enc_para->rc.vo_frm_rate;

            if (enc_para->tr_count > enc_para->rc.vi_frm_rate) {
                enc_para->tr_count -= enc_para->rc.vi_frm_rate;
                venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, enc_para->mean_bit);
            } else {
                venc_drv_fifo_write_init(&enc_para->valg_bits_fifo, 0);
            }
        }
    }

    return;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
static hi_s32 venc_drv_efl_set_resolution(vedu_efl_enc_para *enc_handle, hi_u32 frame_width,
                                          hi_u32 frame_height, hi_u32 chn_id)
{
    vedu_efl_enc_para *enc_para = enc_handle;
    vedu_efl_rc_attr rc_attr;
    hi_u32 width_in_mb = (frame_width + 15) >> 4;       /* 15, 4: frame_width, bits */
    hi_u32 height_in_mb = (frame_height + 15) >> 4;     /* 15, 4: frame_width, bits */
    hi_u32 luma_size = (width_in_mb * height_in_mb) << 8;   /* 8: bits */
    hi_s32 ret;

    /* check config */
    if (luma_size > enc_para->hal.rcn_c_addr[0] - enc_para->hal.rcn_y_addr[0]) {
        HI_ERR_VENC("Wrong configuration of width(%d) and height(%d)\n", frame_width, frame_height);
        return HI_FAILURE;
    }

    ret = venc_drv_efl_rc_get_attr(&(g_venc_chn[chn_id].chn_user_cfg), enc_para, &rc_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("venc_drv_efl_rc_get_attr fail\n");
        return HI_FAILURE;
    }
    if (HI_SUCCESS != venc_drv_efl_rc_attr_init(enc_handle, &rc_attr)) {
        HI_ERR_VENC("config venc rate control attribute err!.\n");
        return HI_FAILURE;
    }

    enc_para->hal.slc_split_mod = 1;
    enc_para->num_ref_index = 0;
    enc_para->rc.pic_width = frame_width;
    enc_para->rc.pic_height = frame_height;

    venc_drv_efl_flush_strm_header(enc_para);

    return HI_SUCCESS;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32 venc_drv_efl_start_venc(vedu_efl_enc_para *enc_handle)
{
    hi_u32 i, j;
    vedu_lock_flag flag;
    vedu_efl_enc_para *enc_para = NULL;

    D_VENC_CHECK_PTR(enc_handle);

    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);
    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if (g_vedu_chn_ctx[i].enc_handle == NULL) {
            g_vedu_chn_ctx[i].enc_handle = enc_handle;
            enc_para = enc_handle;
            break;
        }
    }
    if ((i >= HI_VENC_MAX_CHN) || (enc_para == NULL)) {
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }
    for (j = 0;j < HI_VENC_MAX_CHN; j++) {
        if (g_priority_tab[0][j] == INVAILD_CHN_FLAG) {
            g_priority_tab[0][j] = i;
            g_priority_tab[1][j] = enc_para->priority;
            venc_drv_efl_sort_priority();
            break;
        }
    }
    set_default_input_frm_rate_type(enc_para);

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);

    enc_para->never_enc = HI_TRUE;
    enc_para->first_nal_2_send = HI_TRUE;

    return HI_SUCCESS;
}

static hi_void venc_drv_efl_init_last_frm_rate(hi_u32 *last_frm_rate, hi_u32 *last_sec_input_frm_rate,
                                               hi_u32 last_frm_rate_lenth, hi_u32 last_sec_input_frmrate_lenth)
{
    hi_u32 i;

    for (i = 0; i < last_frm_rate_lenth; i++) {
        last_frm_rate[i] = 0;
    }
    for (i = 0; i < last_sec_input_frmrate_lenth; i++) {
        last_sec_input_frm_rate[i] = 0;
    }

    return;
}

static hi_s32 set_chan_handle_invalid(vedu_efl_enc_para *enc_handle)
{
    hi_u32 i, j;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if (g_vedu_chn_ctx[i].enc_handle == enc_handle) {
            g_vedu_chn_ctx[i].enc_handle = NULL;
            break;
        }
    }
    for (j = 0; (i < HI_VENC_MAX_CHN) && (j < HI_VENC_MAX_CHN); j++) {
        if (i == g_priority_tab[0][j]) {
            g_priority_tab[0][j] = INVAILD_CHN_FLAG;
            g_priority_tab[1][j] = 0;
            venc_drv_efl_sort_priority();
            break;
        }
    }

    if (i == HI_VENC_MAX_CHN) {
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    return HI_SUCCESS;
}

static hi_void stop_venc_omx_flush_chn(vedu_efl_enc_para *enc_para, hi_u32 ve_chn)
{
    if (!enc_para->omx_chn) {  /* for venc */
        if (enc_para->image_valid) {
            if (venc_drv_efl_put_image(ve_chn, &enc_para->image) != HI_SUCCESS) {
                HI_WARN_VENC("put_image failed!\n");
            }
        }
        venc_drv_mng_move_queue_to_dequeue(enc_para->frame_queue, enc_para->frame_dequeue);
    } else {                   /* for omxvenc */
        venc_drv_efl_flush_port_omx(enc_para, ALL_PORT, HI_TRUE);
    }
}

static hi_void stop_venc_flush_queue(vedu_efl_enc_para *enc_handle, hi_u32 ve_chn)
{
    vedu_efl_enc_para *enc_para = NULL;

    /* rfresh the queue of the img */
    enc_para = enc_handle;
    if (g_venc_chn[ve_chn].src_mod_id >= HI_ID_MAX) {
        stop_venc_omx_flush_chn(enc_para, ve_chn);
    } else {
        venc_drv_efl_rls_all_frame(enc_handle);
    }

    enc_para->rc.frm_rate_sata_error_flag = 0;
    venc_drv_efl_init_last_frm_rate(enc_para->last_frm_rate, enc_para->last_sec_input_frm_rate,
                                    sizeof(enc_para->last_frm_rate) / sizeof(hi_u32),
                                    sizeof(enc_para->last_sec_input_frm_rate) / sizeof(hi_u32));

    strm_reset_buffer(enc_para->strm_mng, enc_para->protocol);

    if (g_venc_chn[ve_chn].omx_chn) {
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx,
                                     HI_VENC_MSG_RESP_STOP_DONE, HI_SUCCESS, NULL) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->stat.msg_queue_num++;
    }
}

hi_s32 venc_drv_efl_stop_venc(vedu_efl_enc_para *enc_handle)
{
    hi_u32 time_cnt = 0;
    hi_s32 ret;

    vedu_lock_flag flag;
    hi_u32 ve_chn;

    D_VENC_GET_CHN(ve_chn, enc_handle);
    D_VENC_CHECK_CHN(ve_chn);
    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);

    ret = set_chan_handle_invalid(enc_handle);

    if (ret != HI_SUCCESS) {
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
        HI_ERR_VENC("set_chan_handle_invalid fail\n");
        return ret;
    }

    /* wait finish last frame */
    while ((((vedu_efl_enc_para *)enc_handle)->waiting_isr) && (time_cnt < 400)) { /* 400: time_cnt */
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
        venc_drv_osal_give_event(&g_rc_event);
        osal_msleep_uninterruptible(5); /* 5 ms */
        time_cnt++;
        venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);
    }

    if (time_cnt == 400) {     /* 400: time_cnt */
        HI_ERR_VENC("wait the VEDU isr time out!!force to stop the channel\n");
        VENC_PDT_RESET_HARDWARE();
        VENC_PDT_OPEN_HARDWARE();
        g_vedu_ip_ctx.ip_free = 1;
    }
#ifdef VENC_SUPPORT_JPGE
    /* wait finish last frame */
    time_cnt = 0;
    while ((((vedu_efl_enc_para *)enc_handle)->waiting_isr_jpge)  && (time_cnt < 1000)) {   /* 1000: time_cnt */
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
        osal_msleep_uninterruptible(1);
        time_cnt++;
        venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);
    }

    if (time_cnt == 1000) {     /* 1000: time_cnt */
        HI_ERR_VENC("wait the JPEG isr time out!!force to stop the channel\n");
    }
#endif

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);

    venc_drv_osal_give_event(&g_venc_wait_stream[ve_chn]);

    stop_venc_flush_queue(enc_handle, ve_chn);

    return HI_SUCCESS;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
static hi_void venc_drv_dbg_lowdelay_event(hi_s32 chan_id, hi_stat_ld_event event_id,
                                           hi_u32 frame_id, hi_handle src_hd)
{
    hi_stat_ld_event_info evt;
    hi_u32 tmp_time;

    tmp_time = hi_drv_sys_get_time_stamp_ms();

    evt.ld_event = event_id;
    evt.frame = frame_id;
    evt.handle = src_hd;
    evt.time = tmp_time;
    hi_drv_stat_ld_notify_event(&evt);
}

static hi_s32 venc_drv_efl_check_img_cfg(const hi_u32 protocol, const hi_drv_video_frame *pre_image,
    hi_u32 yuv_store_type)
{
    hi_u8 flag = 0;
    hi_u32 max_width, max_height, min_width, min_height;
    hi_u64 phy_addr_base, phy_addr_y, phy_addr_c, phy_addr_cr;

    if (protocol == VEDU_JPGE) {
        max_width = HI_JPEG_MAX_WIDTH;
        min_width = HI_JPEG_MIN_WIDTH;
        max_height = HI_JPEG_MAX_HEIGTH;
        min_height = HI_JPEG_MIN_HEIGTH;
    } else {
        max_width = HI_VENC_MAX_WIDTH;
        min_width = HI_VENC_MIN_WIDTH;
        max_height = HI_VENC_MAX_HEIGTH;
        min_height = HI_VENC_MIN_HEIGTH;
    }

    phy_addr_base = pre_image->video_private.venc_private.data;
    phy_addr_y = phy_addr_base + pre_image->buf_addr[0].y_offset;
    phy_addr_c = phy_addr_base + pre_image->buf_addr[0].c_offset;
    phy_addr_cr = phy_addr_base + pre_image->buf_addr[0].cr_offset;

    flag |= (hi_u8)((pre_image->width > max_width) || (pre_image->width < min_width));
    flag |= (hi_u8)((pre_image->height > max_height) || (pre_image->height < min_height));
    flag |= (hi_u8)((!phy_addr_y) || (phy_addr_y % 16));  /* 16: bits */
    flag |= (hi_u8)((!pre_image->buf_addr[0].stride_y) || (pre_image->buf_addr[0].stride_y % 16));  /* 16: bits */
    flag |= (hi_u8)(pre_image->buf_addr[0].stride_y > FRMBUF_STRIDE_LIMIT);

    if (yuv_store_type == VENC_STORE_PLANNAR) {
        flag |= (hi_u8)((!phy_addr_c) || (phy_addr_c % 16)); /* 16: bits */
        flag |= (hi_u8)((!phy_addr_cr) || (phy_addr_cr % 16));    /* 16: bits */
        flag |= (hi_u8)((!pre_image->buf_addr[0].stride_c) ||
                (pre_image->buf_addr[0].stride_c % 16));  /* 16: bits */
        flag |= (hi_u8)((!pre_image->buf_addr[0].stride_cr) ||
                (pre_image->buf_addr[0].stride_cr % 16));    /* 16: bits */
        flag |= (hi_u8)(pre_image->buf_addr[0].stride_c > FRMBUF_STRIDE_LIMIT);
        flag |= (hi_u8)(pre_image->buf_addr[0].stride_cr > FRMBUF_STRIDE_LIMIT);
        flag |= (hi_u8)(pre_image->buf_addr[0].stride_cr != pre_image->buf_addr[0].stride_c);
    } else if (yuv_store_type == VENC_STORE_SEMIPLANNAR) {
        flag |= (hi_u8)((!phy_addr_c) || (phy_addr_c % 16));  /* 16: bits */
        flag |= (hi_u8)((!pre_image->buf_addr[0].stride_c) || (pre_image->buf_addr[0].stride_c % 16));  /* 16: bits */
        flag |= (hi_u8)(pre_image->buf_addr[0].stride_c > FRMBUF_STRIDE_LIMIT);
    }
    if (flag == HI_TRUE) {
        return HI_FAILURE;
    } else {
        return HI_SUCCESS;
    }
}

static hi_s32 venc_drv_efl_check_omx_img_cfg(const hi_venc_user_buf *pre_image, hi_u32 yuv_store_type)
{
    hi_u8 flag = 0;

    flag |= (hi_u8)((pre_image->pic_width > HI_VENC_MAX_WIDTH) || (pre_image->pic_width < HI_OMXVENC_MIN_WIDTH));
    flag |= (hi_u8)((pre_image->pic_height > HI_VENC_MAX_HEIGTH) || (pre_image->pic_height < HI_OMXVENC_MIN_HEIGTH));
    flag |= (hi_u8)((!pre_image->buffer_addr_phy) || (pre_image->buffer_addr_phy % 16));    /* 16: bits */
    flag |= (hi_u8)((!pre_image->stride_y) || (pre_image->stride_y  % 16)); /* 16: bits */
    flag |= (hi_u8)(pre_image->stride_y > FRMBUF_STRIDE_LIMIT);

    if (yuv_store_type == VENC_STORE_PLANNAR) {
        flag |= (hi_u8)((!pre_image->offset_y_c)  || (pre_image->offset_y_c % 16)); /* 16: bits */
        flag |= (hi_u8)((!pre_image->offset_y_cr) || (pre_image->offset_y_cr % 16));    /* 16: bits */
        flag |= (hi_u8)((!pre_image->stride_c)  || (pre_image->stride_c % 16));  /* 16: bits */
        flag |= (hi_u8)(pre_image->stride_c > FRMBUF_STRIDE_LIMIT);
    } else if (yuv_store_type == VENC_STORE_SEMIPLANNAR) {
        flag |= (hi_u8)((!pre_image->offset_y_c)  || (pre_image->offset_y_c % 16)); /* 16: bits */
        flag |= (hi_u8)((!pre_image->stride_c)  || (pre_image->stride_c  % 16));    /* 16: bits */
        flag |= (hi_u8)(pre_image->stride_c > FRMBUF_STRIDE_LIMIT);
    }

    if (flag == HI_TRUE) {
        return HI_FAILURE;
    } else {
        return HI_SUCCESS;
    }
}

static hi_bool query_chn_set_resolution(vedu_efl_enc_para *enc_para, hi_u32 chn_id)
{
    if (g_venc_chn[chn_id].jpge == HI_INVALID_HANDLE) {  /* h264 */
        hi_s32 ret = HI_FAILURE;
        venc_drv_efl_request_i_frame(enc_para);
        ret = venc_drv_efl_set_resolution(enc_para, enc_para->image.width, enc_para->image.height, chn_id);

        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("vedu_efl_set_resolution err:%#x.\n", ret);
            enc_para->stat.err_cfg_skip++;
            enc_para->stat.put_frame_num_try++;
            (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
            enc_para->image_valid = HI_FALSE;
            enc_para->stat.put_frame_num_ok++;
            enc_para->hal.pts0 = enc_para->image.pts;

            return HI_FAILURE;
        }
    } else {  /* JPEG */
#ifdef VENC_SUPPORT_JPGE
        enc_para->rc.pic_width = enc_para->image.width;
        enc_para->rc.pic_height = enc_para->image.height;
#endif
    }

    return HI_SUCCESS;
}

static hi_s32 venc_drv_efl_query_chn_x_resize(hi_u32 chn_id, vedu_efl_enc_para *enc_para)
{
    /* configured the resolving power dynamically */
    if ((enc_para->image.width != enc_para->rc.pic_width) || (enc_para->image.height != enc_para->rc.pic_height)) {
        if ((enc_para->image.width == g_venc_chn[chn_id].chn_user_cfg.config.width)
            && (enc_para->image.height == g_venc_chn[chn_id].chn_user_cfg.config.height)) {
            if (query_chn_set_resolution(enc_para, chn_id) != HI_SUCCESS) {
                return HI_FAILURE;
            }
        } else {
            HI_ERR_VENC("ERR:different resolution between the frame info and the encoder cfg!encode: "
                        "%d_x%d, frame_info: %d_x%d\n", g_venc_chn[chn_id].chn_user_cfg.config.width,
                        g_venc_chn[chn_id].chn_user_cfg.config.height, enc_para->image.width, enc_para->image.height);
            enc_para->stat.err_cfg_skip++;
            enc_para->stat.put_frame_num_try++;
            (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
            enc_para->image_valid = HI_FALSE;
            enc_para->stat.put_frame_num_ok++;

            enc_para->hal.pts0 = enc_para->image.pts;
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void venc_drv_efl_query_chn_x_stride(vedu_efl_enc_in *enc_in, vedu_efl_enc_para *enc_para)
{
    /* Forcing SEMIPLANAR_422 to be encoded as semiplaner 420 */
    /* CNcomment: 强制把SEMIPLANAR_422 当semiplaner 420编码 */
    if ((enc_para->hal.yuv_store_type == VENC_STORE_SEMIPLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
        enc_in->vi_c_stride = enc_para->image.buf_addr[0].stride_c * 2;     /* 2: vi_c_stride */
    } else if ((enc_para->hal.yuv_store_type == VENC_STORE_PACKAGE) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
    } else if ((enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_420)) {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
        enc_in->vi_c_stride = enc_para->image.buf_addr[0].stride_c;
    } else {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
        enc_in->vi_c_stride = enc_para->image.buf_addr[0].stride_c;
    }
}

static hi_void venc_drv_efl_update_scan_type(vedu_efl_enc_para *enc_para)
{
    if ((enc_para->protocol == VEDU_H265) &&
        ((hi_u32)enc_para->is_progressive != (hi_u32)enc_para->image.sample_type)) {
        vedu_efl_h265e_vps vps;
        enc_para->is_progressive = (enc_para->image.sample_type) ? HI_TRUE : HI_FALSE;
        vps.general_interlaced_source_flag = !enc_para->is_progressive;
        vps.general_progressive_source_flag =  enc_para->is_progressive;
        enc_para->vps_bits = h265e_make_vps(enc_para->vps_stream, 64, &vps);    /* 64: size of vps */
    }
}

static hi_s32 get_channel_image(vedu_efl_enc_para *enc_para, hi_handle hd)
{
    enc_para->stat.get_frame_num_try++;

    if (!enc_para->src_info.get_image) {
        HI_ERR_VENC(" get_image is NULL!!\n");
        return HI_FAILURE;
    }

    if ((enc_para->src_info.get_image)(hd, &(enc_para->image)) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    enc_para->stat.get_frame_num_ok++;

    return HI_SUCCESS;
}

static hi_void put_channel_image_for_error(vedu_efl_enc_para *enc_para)
{
    enc_para->stat.put_frame_num_try++;
    (enc_para->src_info.put_image)(enc_para->src_info.handle, &(enc_para->image));
    enc_para->image_valid = HI_FALSE;
    enc_para->stat.put_frame_num_ok++;
}

static hi_void get_channel_frame_rate(vedu_efl_enc_para *enc_para, hi_u32 chn_id)
{
    hi_u32 new_frm_rate;
    hi_venc_frm_rate_type input_frm_rate_type;

    input_frm_rate_type = (enc_para->input_frm_rate_type_config != HI_VENC_FRMRATE_BUTT) ?
        enc_para->input_frm_rate_type_config :
        enc_para->input_frm_rate_type_default;

    D_VENC_RC_UPDATE(enc_para->last_frm_rate, enc_para->image.frame_rate / 1000);

    if (input_frm_rate_type == HI_VENC_FRMRATE_AUTO) {
        new_frm_rate = get_frm_rate_auto_detection(enc_para);
    } else if (input_frm_rate_type == HI_VENC_FRMRATE_STREAM) {
        new_frm_rate = get_frm_rate_from_stream(enc_para);
    } else {
        new_frm_rate = get_frm_rate_user_config(enc_para, chn_id);
    }

    if (new_frm_rate == 0) {
        new_frm_rate = get_frm_rate_user_config(enc_para, chn_id);
        HI_ERR_VENC("new_frm_rate = 0 not support, force to use user_config input frame_rate(%d).\n", new_frm_rate);
    }

    update_input_frm_rate(enc_para, new_frm_rate, &g_venc_chn[chn_id].chn_user_cfg);

    enc_para->stat.real_send_input_frm_rate = enc_para->rc.vi_frm_rate;
    enc_para->stat.real_send_output_frm_rate = enc_para->rc.vo_frm_rate;
}

static hi_void get_channel_pixel_format(vedu_efl_enc_para *enc_para)
{
    enc_para->hal.yuv_store_type = convert_pix_format(enc_para->image.pixel_format, 0);
    enc_para->yuv_sample_type = convert_pix_format(enc_para->image.pixel_format, 1);
    enc_para->hal.package_sel = convert_pix_format(enc_para->image.pixel_format, 2);    /* 2: flag */
    enc_para->stat.frame_type = convert_pix_format(enc_para->image.pixel_format, 3);    /* 3: flag */
}

static hi_s32 get_local_channel_info_x(vedu_efl_enc_para *enc_para, hi_u32 chn_id)
{
    hi_handle hd;

    hd = g_venc_chn[chn_id].user_handle;

    if (enc_para->quick_encode) {
        if (quick_encode_process(g_venc_chn[chn_id].venc_handle, hd) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    } else {
        if (get_channel_image(enc_para, hd) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    /* don't re-get */
    if (enc_para->hal.pts0 == enc_para->image.pts) {
        put_channel_image_for_error(enc_para);
        enc_para->stat.same_pts_skip++;

        return HI_FAILURE;
    }

    /* video encoder does frame rate control by two value: input frame rate and target frame rate */
    /* input frame rate is calculated by timer mechanism accurately */
    /* target frame rate is input by user and can be changed dynamiclly */
    get_channel_frame_rate(enc_para, chn_id);

    venc_drv_efl_update_scan_type(enc_para);

    /* configured the resolving power dynamically */
    if (venc_drv_efl_query_chn_x_resize(chn_id, enc_para) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* skip - frame rate ctrl */
    if (venc_drv_efl_rc_frm_rate_ctrl(enc_para, enc_para->stat.put_frame_num_ok) != HI_SUCCESS) {
        put_channel_image_for_error(enc_para);
        enc_para->hal.pts0 = enc_para->image.pts;

        return HI_FAILURE;
    }

    get_channel_pixel_format(enc_para);

    /* check the picture resolving power ,stride ,addr info first */
    if (venc_drv_efl_check_img_cfg(enc_para->protocol, &enc_para->image, enc_para->hal.yuv_store_type) != HI_SUCCESS) {
        HI_ERR_VENC("img cfg erro!!\n");
        put_channel_image_for_error(enc_para);
        enc_para->stat.err_cfg_skip++;
        enc_para->hal.pts0 = enc_para->image.pts;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 get_local_channel_check_size(vedu_efl_enc_para *enc_para)
{
    if ((enc_para->image_omx.pic_width != enc_para->rc.pic_width) ||
        (enc_para->image_omx.pic_height != enc_para->rc.pic_height)) {
        HI_ERR_VENC("not support resolution change!!! %dx%d ==> %dx%d\n",
                    enc_para->rc.pic_width, enc_para->rc.pic_height,
                    enc_para->image_omx.pic_width, enc_para->image_omx.pic_height);
        enc_para->stat.put_frame_num_try++;
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE, HI_SUCCESS,
                                     &(enc_para->image_omx)) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->image_valid = HI_FALSE;
        enc_para->stat.ebd_cnt++;
        enc_para->stat.msg_queue_num++;
        enc_para->stat.put_frame_num_ok++;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 check_omx_image_config_info(vedu_efl_enc_para *enc_para)
{
    if (venc_drv_efl_check_omx_img_cfg(&enc_para->image_omx, enc_para->hal.yuv_store_type) != HI_SUCCESS) {
        HI_ERR_VENC("img cfg erro!!\n");
        enc_para->stat.put_frame_num_try++;
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE,
                                     HI_SUCCESS, &(enc_para->image_omx)) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->image_valid = HI_FALSE;
        enc_para->stat.ebd_cnt++;
        enc_para->stat.msg_queue_num++;
        enc_para->stat.put_frame_num_ok++;
        enc_para->stat.err_cfg_skip++;
        enc_para->hal.pts0 = enc_para->image_omx.time_stamp0;
        enc_para->hal.pts1 = enc_para->image_omx.time_stamp1;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 get_local_channel_info_y(vedu_efl_enc_para *enc_para, hi_u32 chn_id)
{
    enc_para->stat.get_frame_num_try++;
    if (!enc_para->src_info.get_image_omx) {
        HI_ERR_VENC("enc_para->src_info.get_image_omx is null!\n");
        return HI_FAILURE;
    }
    if ((enc_para->src_info.get_image_omx)(enc_para->src_info.handle, &(enc_para->image_omx)) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    enc_para->stat.get_frame_num_ok++;

    enc_para->stat.real_send_input_frm_rate = enc_para->rc.vi_frm_rate;
    enc_para->stat.real_send_output_frm_rate = enc_para->rc.vo_frm_rate;

    if (get_local_channel_check_size(enc_para) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* video encoder does frame rate control by two value: input frame rate and target frame rate */
    /* input frame rate is calculated by timer mechanism accurately */
    /* target frame rate is input by user and can be changed dynamiclly */
    /* skip - frame rate ctrl */
    if (venc_drv_efl_rc_frm_rate_ctrl(g_venc_chn[chn_id].venc_handle, enc_para->stat.put_frame_num_ok) != HI_SUCCESS) {
        enc_para->stat.put_frame_num_try++;
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE, HI_SUCCESS,
                                     &(enc_para->image_omx)) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->image_valid = HI_FALSE;
        enc_para->stat.ebd_cnt++;
        enc_para->stat.msg_queue_num++;
        enc_para->stat.put_frame_num_ok++;

        enc_para->hal.pts0 = enc_para->image_omx.time_stamp0;
        enc_para->hal.pts1 = enc_para->image_omx.time_stamp1;
        return HI_FAILURE;
    }

    enc_para->hal.yuv_store_type = enc_para->image_omx.store_type; /* VENC_STORE_SEMIPLANNAR; */
    enc_para->yuv_sample_type = enc_para->image_omx.sample_type; /* VENC_YUV_420; */
    enc_para->hal.package_sel = enc_para->image_omx.package_sel; /* VENC_V_U; */

    /* check the picture resolving power ,stride ,addr info first */
    return check_omx_image_config_info(enc_para);
}

static hi_void get_global_channel_info_x(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_u64 phy_addr_base;

    /****************get addr information**************/
    phy_addr_base = enc_para->image.video_private.venc_private.data;
    enc_in->bus_vi_y = phy_addr_base + enc_para->image.buf_addr[0].y_offset;
    enc_in->bus_vi_c = phy_addr_base + enc_para->image.buf_addr[0].c_offset;

    if (enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) {
        enc_in->bus_vi_v = phy_addr_base + enc_para->image.buf_addr[0].cr_offset;
    }

    /****************get tunnel addr information**************/
    if (enc_para->image.tunnel_phy_addr) {
        enc_para->rc.low_delay_mode = HI_TRUE;
        enc_in->tunl_cell_addr = enc_para->image.tunnel_phy_addr;
    } else {
        enc_para->rc.low_delay_mode = HI_FALSE;
        enc_in->tunl_cell_addr = 0;
    }

    /****************get stride information**************/
    venc_drv_efl_query_chn_x_stride(enc_in, enc_para);

    /****************get pts information**************/
    enc_in->pts0 = enc_para->image.pts;
    enc_in->pts1 = 0;
}

static hi_s32 venc_drv_efl_query_chn_x(hi_u32 chn_id, vedu_efl_enc_in *enc_in)
{
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)g_venc_chn[chn_id].venc_handle;
    D_VENC_CHECK_PTR(enc_para);
    D_VENC_CHECK_PTR(enc_in);

    if (enc_para->not_enough_buf != HI_TRUE) {
        if (get_local_channel_info_x(enc_para, chn_id) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    get_global_channel_info_x(enc_para, enc_in);

    return HI_SUCCESS;
}

static hi_s32 venc_drv_efl_query_chn_y(hi_u32 chn_id, vedu_efl_enc_in *enc_in)
{
    /* hi_handle hd; */
    vedu_efl_enc_para *enc_para = (vedu_efl_enc_para *)g_venc_chn[chn_id].venc_handle;

    if (enc_para->not_enough_buf != HI_TRUE) {
        if (get_local_channel_info_y(enc_para, chn_id) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    venc_drv_get_last_frm_info_omx(enc_para, enc_in);

    return HI_SUCCESS;
}

static hi_s32 query_channel_info(hi_u32 chn_id, vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    if (g_venc_chn[chn_id].omx_chn) {
        if (venc_drv_mng_queue_empty(enc_para->stream_queue_omx)) {
            return HI_FAILURE;
        }
        if (venc_drv_efl_query_chn_y(chn_id, enc_in) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    } else {
        if (venc_drv_efl_query_chn_x(chn_id, enc_in) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    venc_drv_dbg_lowdelay_event(chn_id, HI_STAT_LD_EVENT_VENC_FRM_IN, enc_para->image.frame_index,
        enc_para->image.tunnel_handle);
    return HI_SUCCESS;
}

static vedu_efl_enc_para *query_chn_h264_find_chn(hi_u32 *start_num, hi_u32 *id, hi_s32 *start_id,
                                                  vedu_efl_enc_in *enc_in, hi_bool is_isr)
{
    hi_u32 start_query_no;
    hi_u32 chn_id = 0;
    hi_s32 start_chn_id;
    vedu_efl_enc_para *enc_para = HI_NULL;
    hi_s32 ret = HI_SUCCESS;

    for (start_query_no = 0; start_query_no < HI_VENC_MAX_CHN; start_query_no++) {
        D_VENC_GET_CHN_FROM_TAB(start_chn_id, start_query_no);

        if (start_chn_id == INVAILD_CHN_FLAG) {
            continue;
        }

        D_VENC_GET_CHN(chn_id, g_vedu_chn_ctx[start_chn_id].enc_handle);

        if (chn_id >= HI_VENC_MAX_CHN) {
            return HI_NULL;
        }

        enc_para = (vedu_efl_enc_para *) g_vedu_chn_ctx[start_chn_id].enc_handle;

        if ((is_isr == HI_TRUE) && (g_venc_chn[chn_id].proc_write.save_yuv_file_run)) {
            continue;
        }

        if (enc_para->src_info.handle == HI_INVALID_HANDLE) {
            enc_para->src_info.handle = g_venc_chn[chn_id].user_handle;
        }

        ret = strm_check_buffer_size(enc_para->strm_mng, enc_para->hal.strm_buf_size, enc_para->cur_strm_buf_size);
        if (ret != HI_SUCCESS) {
            enc_para->stat.check_buf_size_num_fail++;
            continue;
        }

       /* this query_chn just for H264 */
#ifdef VENC_SUPPORT_JPGE
        if (HI_INVALID_HANDLE != g_venc_chn[chn_id].jpge) {
            continue;
        }
#endif
        if (query_channel_info(chn_id, enc_para, enc_in) != HI_SUCCESS) {
            continue;
        }

        enc_para->waiting_isr = 1;
        break; /* find channel:start_chn_id  to enc */
    }

    *start_num = start_query_no;
    *id = chn_id;
    *start_id = start_chn_id;

    return enc_para;
}

static vedu_efl_enc_para *venc_drv_efl_query_chn_h264(vedu_efl_enc_in *enc_in, hi_bool is_isr)
{
    hi_u32 start_query_no = 0;
    hi_s32 start_chn_id = 0;     /* this ID correspond to g_vedu_chn_ctx(class:vedu_efl_chn_ctx) */
    hi_u32 chn_id = 0;            /* this ID correspond to g_venc_chn(class:optm_venc_chn) */
    vedu_efl_enc_para *enc_para = HI_NULL;
    vedu_lock_flag flag;
    vedu_efl_enc_para *hd_ret = HI_NULL;

    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);

    enc_para = query_chn_h264_find_chn(&start_query_no, &chn_id, &start_chn_id, enc_in, is_isr);
    if (enc_para == HI_NULL) {
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
        return HI_NULL;
    }

    if (start_query_no != HI_VENC_MAX_CHN) {
        venc_drv_efl_sort_priority_2(g_priority_tab[1][start_query_no]);
    }

    if ((start_chn_id >= 0) && (start_chn_id < HI_VENC_MAX_CHN)) {
        hd_ret = g_venc_chn[chn_id].venc_handle;
    }

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);

    if (start_query_no == HI_VENC_MAX_CHN) {
        return HI_NULL;
    } else {
        if (g_venc_chn[chn_id].proc_write.save_yuv_file_run && enc_para) {
            if (enc_para->omx_chn) {
                venc_drv_dbg_write_yuv_omx(&enc_para->image_omx, g_venc_chn[chn_id].proc_write.yuv_file_name,
                    sizeof(g_venc_chn[chn_id].proc_write.yuv_file_name) / sizeof(hi_char));
            } else {
                venc_drv_dbg_write_yuv(&enc_para->image, g_venc_chn[chn_id].proc_write.yuv_file_name,
                    sizeof(g_venc_chn[chn_id].proc_write.yuv_file_name) / sizeof(hi_char));
            }
        }

        return hd_ret;
    }
}

#ifdef VENC_SUPPORT_JPGE

static vedu_efl_enc_para *query_chn_jpeg_find_chn(hi_u32 *start_num, hi_s32 *start_id, hi_u32 *id,
                                                  vedu_efl_enc_in *enc_in)
{
    hi_u32 start_query_no;
    hi_u32 chn_id;
    hi_s32 start_chn_id;
    vedu_efl_enc_para *enc_para = HI_NULL;

    for (start_query_no = 0; start_query_no < HI_VENC_MAX_CHN; start_query_no++) {
        D_VENC_GET_CHN_FROM_TAB(start_chn_id, start_query_no);
        if (start_chn_id == INVAILD_CHN_FLAG) {
            continue;
        }
        D_VENC_GET_CHN(chn_id, g_vedu_chn_ctx[start_chn_id].enc_handle);
        if (chn_id >= HI_VENC_MAX_CHN)
            return HI_NULL;

        enc_para = (vedu_efl_enc_para *) g_vedu_chn_ctx[start_chn_id].enc_handle;
        if (enc_para->src_info.handle == HI_INVALID_HANDLE) {
            enc_para->src_info.handle = g_venc_chn[chn_id].user_handle;
        }

        /* this query_chn just for jpeg */
        if (g_venc_chn[chn_id].jpge == HI_INVALID_HANDLE) {
            continue;
        }

        if (HI_SUCCESS != venc_drv_efl_query_chn_x(chn_id, enc_in)) { continue; }

        venc_drv_dbg_lowdelay_event(chn_id, HI_STAT_LD_EVENT_VENC_FRM_IN,
                                    enc_para->image.frame_index, enc_para->image.tunnel_handle);

        enc_para->waiting_isr_jpge = 1;
        break; /* find channel:start_chn_id  to enc */
    }

    *start_num = start_query_no;
    *start_id = start_chn_id;
    *id = chn_id;

    return enc_para;
}

static vedu_efl_enc_para *venc_drv_efl_query_chn_jpeg(vedu_efl_enc_in *enc_in)
{
    hi_u32 start_query_no = 0;
    hi_s32 start_chn_id = 0;     /* this ID correspond to g_vedu_chn_ctx(class:vedu_efl_chn_ctx) */
    hi_u32 chn_id = 0;            /* this ID correspond to g_venc_chn(class:optm_venc_chn) */
    vedu_efl_enc_para *enc_para = HI_NULL;
    vedu_lock_flag flag;
    vedu_efl_enc_para *hd_ret = HI_NULL;

    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);

    enc_para = query_chn_jpeg_find_chn(&start_query_no, &start_chn_id, &chn_id, enc_in);
    if (enc_para == HI_NULL) {
        venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
        return HI_NULL;
    }

    if (start_query_no != HI_VENC_MAX_CHN) {
        venc_drv_efl_sort_priority_2(g_priority_tab[1][start_query_no]);
    }

    if ((start_chn_id >= 0) && (start_chn_id < HI_VENC_MAX_CHN)) {
        hd_ret = g_vedu_chn_ctx[start_chn_id].enc_handle;
    }

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);

    if (start_query_no == HI_VENC_MAX_CHN) {
        return HI_NULL;
    } else {
        if (g_venc_chn[chn_id].proc_write.save_yuv_file_run && enc_para &&
           (g_venc_chn[chn_id].src_mod_id < HI_ID_MAX)) {
            venc_drv_dbg_write_yuv(&enc_para->image, g_venc_chn[chn_id].proc_write.yuv_file_name,
                sizeof(g_venc_chn[chn_id].proc_write.yuv_file_name) / sizeof(hi_char));
        }

        if (enc_in->tunl_cell_addr != 0) {
            HI_ERR_VENC("warning!!!!! JPEG encoder not support lowdelay!!!the output maybe error!\n");
        }

        return hd_ret;
    }
}
#endif

#ifdef __VENC_FREQ_DYNAMIC__
static hi_s32 venc_drv_efl_current_cap(hi_s32 *clk_sel)
{
    hi_u32 chn_id = 0;
    hi_u32 total_mb = 0;
    hi_u32 mb_num = 0;
    hi_u32 width_in_mb = 0;
    hi_u32 height_in_mb = 0;
    for (chn_id = 0; chn_id < HI_VENC_MAX_CHN; chn_id++) {
        if (g_venc_chn[chn_id].enable) {
            width_in_mb = (g_venc_chn[chn_id].chn_user_cfg.width + 15) >> 4;    /* 15, 4: width, bits */
            height_in_mb = (g_venc_chn[chn_id].chn_user_cfg.height + 15) >> 4;  /* 15, 4: width, bits */
            mb_num = width_in_mb * height_in_mb * g_venc_chn[chn_id].chn_user_cfg.target_frame_rate;
            total_mb += mb_num;
        }
    }

    if (total_mb < 125000) { /* 125000: MB, 0.5s = 400*(MB)*(1/100MHZ)   MB=125000 */
        *clk_sel = VENC_FREQUENCY_100M;
    } else if (total_mb < 187500) {         /* 187500: MB */
        *clk_sel = VENC_FREQUENCY_150M;
    } else {
        *clk_sel = VENC_FREQUENCY_200M;
    }
    return HI_SUCCESS;
}
#endif

static hi_void venc_smmu_fault_check(VOID)
{
#ifdef HI_SMMU_SUPPORT
    venc_hal_read_smmu read_back = {0};

    memset_s(&read_back, sizeof(venc_hal_read_smmu), 0, sizeof(venc_hal_read_smmu));

    VENC_HAL_READ_REG_SMMU(&read_back);

    if (read_back.tlb_miss_stat_ns) {
        HI_ERR_VENC("SMMU table miss error!\n");
    }

    if (read_back.ptw_trans_stat_ns) {
        HI_ERR_VENC("SMMU table pre get error!\n");
    }

    /* for not safe mode */
    if (read_back.tlb_invalid_stat_ns_rd) {
        HI_ERR_VENC("SMMU_READ_ERR MODULE : %s NSSTATE : %#x rd_addr:%#x\n", "HI_MOD_VENC",
                    read_back.tlb_invalid_stat_ns_rd, read_back.fault_addr_rd_ns);
    }

    if (read_back.tlb_invalid_stat_ns_wr) {

        HI_ERR_VENC("SMMU_WRITE_ERR MODULE : %s NSSTATE : %#x wr_addr:%#x\n", "HI_MOD_VENC",
                    read_back.tlb_invalid_stat_ns_wr, read_back.fault_addr_wr_ns);
    }
#endif
    return;
}

#ifdef AUTO_CATCH_I_FRAME
static hi_void i_frm_insert_rc_mark(vedu_efl_enc_para *enc_para)
{
    hi_u32 i;

    enc_para->inter_frm_cnt = enc_para->rc.gop - 1;
    enc_para->rc.i_frm_insert_flag = 1;
    enc_para->rc.rc_start = 1;

    for (i = 0; i < 6; i++) {   /* 6: sizes */
        enc_para->rc.i_mb_ratio_current_scence[i] = 0;
    }
}
#endif

static hi_void venc_drv_rc_i_frm_insert_for_large_imb(vedu_efl_enc_para *enc_para)
{
    hi_u32 imb_ratio = 0;
    hi_u32 mbtotal;

    mbtotal =  enc_para->rc.pic_width * enc_para->rc.pic_height / (16 * 16);    /* 16: mbtotal */

    if (mbtotal != 0) {
        imb_ratio = enc_para->rc.num_i_mb_current_frm * 1000 / mbtotal;     /* 1000: imb_ratio */
    }

#ifdef AUTO_CATCH_I_FRAME
    if (enc_para->i_catch_enable) {
        enc_para->frm_cnt++;

        if (enc_para->frm_cnt > 30) {       /* 30: frm_cnt */
            hi_u32 imb_ratio_avg = 0;
            hi_u32 diff = 0;
            hi_u32 i = 0;
            imb_ratio_avg =  venc_drv_rc_average(enc_para->rc.i_mb_ratio_current_scence, 6);    /* 6: n */
            imb_ratio_avg = D_VENC_RC_MAX(imb_ratio_avg, 1);

            diff = D_VENC_RC_ABS(imb_ratio_avg, imb_ratio);

            if ((imb_ratio > imb_ratio_avg) &&
                ((diff * 100 / imb_ratio_avg) > 200) && (imb_ratio > 500)) { /* 100, 200, 500: diff, imb_ratio */
                i_frm_insert_rc_mark(enc_para);
            } else {
                D_VENC_RC_UPDATE(enc_para->rc.i_mb_ratio_current_scence, imb_ratio);
            }
        } else {
            if (enc_para->frm_cnt != 1) {  /* if it is I frame not update i_mb_ratio_current_scence. */
                D_VENC_RC_UPDATE(enc_para->rc.i_mb_ratio_current_scence, imb_ratio);
            }
        }
    }
#endif

    return;
}

static hi_void venc_drv_get_last_frm_info(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_u64 phy_addr_base;

    /* Need to be identified */
    /* CNcomment: 待明确!! */
    if (enc_para->image.tunnel_phy_addr) {
        enc_para->rc.low_delay_mode = HI_TRUE;
        enc_in->tunl_cell_addr = enc_para->image.tunnel_phy_addr;
    } else {
        enc_para->rc.low_delay_mode = HI_FALSE;
        enc_in->tunl_cell_addr = 0;
    }

    phy_addr_base = enc_para->image.video_private.venc_private.data;
    enc_in->bus_vi_y = phy_addr_base + enc_para->image.buf_addr[0].y_offset;
    enc_in->bus_vi_c = phy_addr_base + enc_para->image.buf_addr[0].c_offset;

     /* enc_para->image.c_addr; */ /* 目前帧信息结构体缺少该结构,planer格式时需要； */
    if (enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) {
        enc_in->bus_vi_v = phy_addr_base + enc_para->image.buf_addr[0].cr_offset;
    }

    /* Forcing SEMIPLANAR_422 to be encoded as semiplaner 420 */
    /* CNcomment: 强制把SEMIPLANAR_422 当semiplaner 420编码 */
    if ((enc_para->hal.yuv_store_type == VENC_STORE_SEMIPLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
        enc_in->vi_c_stride = enc_para->image.buf_addr[0].stride_c * 2;     /* 2: stride_c */
    } else if ((enc_para->hal.yuv_store_type == VENC_STORE_PACKAGE) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
    } else if ((enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_420)) {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
        enc_in->vi_c_stride = enc_para->image.buf_addr[0].stride_c;
    } else {
        enc_in->vi_y_stride = enc_para->image.buf_addr[0].stride_y;
        enc_in->vi_c_stride = enc_para->image.buf_addr[0].stride_c;
    }

    enc_in->pts0 = enc_para->image.pts;
    enc_in->pts1 = 0;

    return;
}

static hi_void get_last_frm_set_type(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    /* Forcing SEMIPLANAR_422 to be encoded as semiplaner 420 */
    /* CNcomment: 强制把SEMIPLANAR_422 当semiplaner 420编码 */
    if ((enc_para->hal.yuv_store_type == VENC_STORE_SEMIPLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
        enc_in->vi_y_stride = enc_para->image_omx.stride_y;
        enc_in->vi_c_stride = enc_para->image_omx.stride_c * 2;     /* 2: stride_c */
        enc_para->stat.frame_type = VENC_SEMIPLANNAR_422;
    } else if ((enc_para->hal.yuv_store_type == VENC_STORE_PACKAGE) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
        enc_in->vi_y_stride = enc_para->image_omx.stride_y;

        switch (enc_para->hal.package_sel) {
            case VENC_PACKAGE_UY0VY1:
                enc_para->stat.frame_type = VENC_PACKAGE_422_UYVY;
                break;

            case VENC_PACKAGE_Y0UY1V:
                enc_para->stat.frame_type = VENC_PACKAGE_422_YUYV;
                break;

            case VENC_PACKAGE_Y0VY1U:
                enc_para->stat.frame_type = VENC_PACKAGE_422_YVYU;
                break;

            default:
                enc_para->stat.frame_type = VENC_UNKNOW;
                break;
        }
    } else if ((enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_420)) {
        enc_in->vi_y_stride = enc_para->image_omx.stride_y;
        enc_in->vi_c_stride = enc_para->image_omx.stride_c;
        enc_para->stat.frame_type = VENC_PLANNAR_420;
    } else {
        enc_in->vi_y_stride = enc_para->image_omx.stride_y;
        enc_in->vi_c_stride = enc_para->image_omx.stride_c;

        if ((enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) && (enc_para->yuv_sample_type == VENC_YUV_422)) {
            enc_para->stat.frame_type = VENC_PLANNAR_422;
        } else if ((enc_para->hal.yuv_store_type == VENC_STORE_SEMIPLANNAR) &&
                  (enc_para->yuv_sample_type == VENC_YUV_420)) {
            if (enc_para->hal.package_sel == 0) {
                enc_para->stat.frame_type = VENC_SEMIPLANNAR_420_VU;
            } else {
                enc_para->stat.frame_type = VENC_SEMIPLANNAR_420_UV;
            }
        }
    }
}

static hi_void venc_drv_get_last_frm_info_omx(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    enc_para->rc.low_delay_mode = HI_FALSE;
    enc_in->tunl_cell_addr = 0;

    if (enc_para->image_omx.meta_data_flag) {  /* store_meta_data case */
        enc_in->bus_vi_y = enc_para->image_omx.meta_data.buffer_addr_phy;
    } else {
        enc_in->bus_vi_y = enc_para->image_omx.buffer_addr_phy + enc_para->image_omx.offset;
    }

    enc_in->bus_vi_c = enc_in->bus_vi_y + enc_para->image_omx.offset_y_c; /* enc_para->image.buf_addr[0].phy_addr_c; */

    if (enc_para->hal.yuv_store_type == VENC_STORE_PLANNAR) {
        /* enc_para->image_omx.stride_c * enc_para->image_omx.pic_height / 2; */
        /* enc_para->image.c_addr; */       /* The current frame information structure lacks this structure.
                                                If it is a planer format, it is required. */
                                            /* 目前帧信息结构体缺少该结构,planer格式时需要； */
        enc_in->bus_vi_v = enc_in->bus_vi_y + enc_para->image_omx.offset_y_cr;
    }

    get_last_frm_set_type(enc_para, enc_in);

    enc_in->pts0 = enc_para->image_omx.time_stamp0;
    enc_in->pts1 = enc_para->image_omx.time_stamp1;

    return;
}

hi_s32 venc_drv_get_encode_para(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    if (enc_para->omx_chn) {
        venc_drv_get_last_frm_info_omx(enc_para, enc_in);
    } else {
        venc_drv_get_last_frm_info(enc_para, enc_in);
    }

    return HI_SUCCESS;
}

#if RC_RECODE_USE
static hi_s32 vedu_drv_rc_process_of_recoding(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    strm_mng_input_info input_info;
    strm_mng_stream_info stream_info;

    if ((venc_drv_rc_is_need_recoding(&enc_para->rc) != HI_SUCCESS)
        || (venc_drv_rc_start_qp_change(&enc_para->rc) != HI_SUCCESS)
        || (venc_drv_get_encode_para(enc_para, enc_in) != HI_SUCCESS)) {
        enc_para->rc.recode_cnt = 0;

        return HI_FAILURE;
    } else {
        HI_ERR_VENC("%s rcn_idx = %d pme_info_idx = %d \n", __func__, enc_para->rc.rcn_idx, enc_para->rc.pme_info_idx);
        /* enc_para->frame_num--; */
        enc_para->rc.rcn_idx = !enc_para->rc.rcn_idx;
        enc_para->rc.pme_info_idx = enc_para->rc.pme_info_idx == 0 ?
                                    2 : enc_para->rc.pme_info_idx - 1;      /* 2: pme_info_idx */
        enc_para->rc.recode_flag = 1;

        if (enc_para->protocol == VEDU_H264) {
            enc_para->h264_rcn_pic->used = 0;
            enc_para->h264_rcn_pic->ref = REF_NONE;
            h264e_set_rcn_out_pic(enc_para);
            h264e_back_up_ref(&enc_para->h264_ref);
            h264e_back_up_poc(enc_para);
        } else if (enc_para->protocol == VEDU_H265) {

        }


        set_stream_info(enc_para, &stream_info);
        set_strm_mng_input_info(enc_para, &input_info);
        strm_put_buffer_for_recode(enc_para->strm_mng, &input_info, &stream_info);

        if (venc_drv_efl_start_one_frame_venc(enc_para, enc_in) == HI_SUCCESS) {
            g_vedu_ip_ctx.ip_free = 0;
            enc_para->stat.recode_num++;
            venc_drv_efl_cfg_reg_venc(enc_para);

            return HI_SUCCESS;
        } else {
            enc_para->frame_num++;
            enc_para->rc.rcn_idx = !enc_para->rc.rcn_idx;
            enc_para->rc.pme_info_idx = enc_para->rc.pme_info_idx == 2 ?    /* 2: pme_info_idx */
                                        0 : enc_para->rc.pme_info_idx + 1;
            enc_para->rc.recode_cnt = 0;
            enc_para->rc.recode_flag = 0;
            HI_ERR_VENC("RECODING START FRAME FAILURE!");

            return HI_FAILURE;
        }
    }
}
#endif

static hi_void process_time_out_interupt(vedu_efl_enc_para *enc_para, hi_u32 ve_chn)
{
    HI_ERR_VENC("VENC timeout interrupt!! lowdelay= %d\n", enc_para->rc.low_delay_mode);

    VENC_HAL_CLEAR_INT(INTERRUPT_TYPE_TIMEOUT);
    VENC_PDT_RESET_HARDWARE();
    VENC_PDT_OPEN_HARDWARE();

    enc_para->waiting_isr = 0;
    g_vedu_ip_ctx.ip_free = 1;

    /* release image encoded */
    enc_para->stat.put_frame_num_try++;

    if (enc_para->omx_chn) {
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE,
                                     HI_SUCCESS, &enc_para->image_omx) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->image_valid = HI_FALSE;
        enc_para->stat.ebd_cnt++;
        enc_para->stat.msg_queue_num++;
    } else {
        (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
        enc_para->image_valid =  HI_FALSE;
    }

    enc_para->stat.put_frame_num_ok++;

    if ((!enc_para->rc.low_delay_mode) || (!enc_para->hal.slc_split_en)) {
        venc_drv_dbg_lowdelay_event(ve_chn, HI_STAT_LD_EVENT_VENC_FRM_OUT,
                                    enc_para->image.frame_index, enc_para->image.tunnel_handle);
    }

    enc_para->first_slc_for_low_dly = HI_TRUE;
    enc_para->time_out = 1;
    put_frame_write_buffer(enc_para);
    enc_para->time_out = 0;
}

static hi_void process_slice_interrupt(venc_hal_read *read_back, vedu_efl_enc_para *enc_para, hi_u32 ve_chn)
{
    venc_drv_efl_get_read_back_data(read_back, &enc_para->hal, &enc_para->rc);

    if (enc_para->first_slc_for_low_dly) {
        venc_drv_dbg_lowdelay_event(ve_chn, HI_STAT_LD_EVENT_VENC_FRM_OUT,
                                    enc_para->image.frame_index, enc_para->image.tunnel_handle);
        enc_para->first_slc_for_low_dly = HI_FALSE;
    }

    venc_drv_efl_end_one_slice_venc(g_vedu_ip_ctx.curr_handle);

    VENC_HAL_CLEAR_INT(INTERRUPT_TYPE_SLICE_END);
}

static hi_void put_frame_to_queue(vedu_efl_enc_para *enc_para)
{
    enc_para->stat.put_frame_num_try++;

    if (enc_para->omx_chn) {
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx, HI_VENC_MSG_RESP_INPUT_DONE, HI_SUCCESS,
            &enc_para->image_omx) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }

        enc_para->image_valid = HI_FALSE;
        enc_para->stat.ebd_cnt++;
        enc_para->stat.msg_queue_num++;
#ifdef __VENC_DRV_DBG__
        if (g_put_msg_ebd_id < 100) {       /* 100: g_put_msg_ebd_id */
            g_tmp_time_dbg[g_put_msg_ebd_id][3] = hi_drv_sys_get_time_stamp_ms();   /* 3: index */

            g_put_msg_ebd_id++;
        }
#endif
    } else {
        (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
        enc_para->image_valid =  HI_FALSE;
    }

    enc_para->stat.put_frame_num_ok++;
}

static hi_void process_end_of_pic_interupt(vedu_efl_enc_para *enc_para, hi_u32 ve_chn, venc_hal_read *read_back)
{
    venc_drv_efl_get_read_back_data(read_back, &enc_para->hal, &enc_para->rc);
    VENC_HAL_CLEAR_INT(INTERRUPT_TYPE_ALL);

    /* add for oversized frame recoding */
    if (enc_para->rc.pic_bits != 0 || read_back->venc_buf_full || read_back->venc_pbit_overflow) {
        enc_para->stat.rate_control_ratio = venc_drv_calculate_rc_ratio(enc_para->rc.inst_bits_sum,
            enc_para->rc.bit_rate);
#if RC_RECODE_USE
        if (((read_back->venc_buf_full == 1) ||
            (read_back->venc_pbit_overflow == 1)) && enc_para->rc.recode_cnt)
            HI_ERR_VENC("recode time = %d still buffer_full(%d) or venc_pbit_overflow(%d)!\n",
                        enc_para->rc.recode_cnt, read_back->venc_buf_full, read_back->venc_pbit_overflow);

        enc_para->rc.check_buf_size = enc_para->cur_strm_buf_size;
        if (vedu_drv_rc_process_of_recoding(enc_para, &enc_in) == HI_SUCCESS)
            return;
#endif
    }

    if (enc_para->protocol == VEDU_H265) {
        h265e_dpb_insert_frame(&enc_para->h265e_dpb, enc_para->h265_poc.pic_order_cnt);
        h265e_dpb_unmark(enc_para, &enc_para->h265e_slc_hdr);
        enc_para->ref_gap_cnt++;
    } else if (enc_para->protocol == VEDU_H264) {
        h264e_update_dpb_frames(enc_para);
        h264e_update_frame_num(enc_para);
        h264e_set_rcn_out_pic(enc_para);
    }

    put_frame_to_queue(enc_para);

    if ((!enc_para->rc.low_delay_mode) || (!enc_para->hal.slc_split_en)) {
        venc_drv_dbg_lowdelay_event(ve_chn, HI_STAT_LD_EVENT_VENC_FRM_OUT, enc_para->image.frame_index,
            enc_para->image.tunnel_handle);
    }

    enc_para->first_slc_for_low_dly = HI_TRUE;
}

static hi_void venc_isr(hi_void)
{
#if RC_RECODE_USE
    vedu_efl_enc_in enc_in;
#endif
    hi_u32 ve_chn;
    vedu_efl_enc_para *enc_para = HI_NULL;
    venc_hal_read read_back = {0};

    D_VENC_GET_CHN(ve_chn, g_vedu_ip_ctx.curr_handle);
    if (ve_chn >= HI_VENC_MAX_CHN) {
        return;
    }

    enc_para = (vedu_efl_enc_para *) g_vedu_ip_ctx.curr_handle;

    venc_smmu_fault_check();

    memset_s(&read_back, sizeof(read_back), 0, sizeof(read_back));
    VENC_HAL_READ_REG(&read_back);

    if (read_back.venc_time_out) {
        process_time_out_interupt(enc_para, ve_chn);
    }

    if (read_back.venc_slice_int) {
        process_slice_interrupt(&read_back, enc_para, ve_chn);
    }

    if (read_back.venc_end_of_pic) {
#ifdef __VENC_DRV_DBG__
        if (g_isr_id < 100) {       /* 100: g_isr_id */
            g_tmp_time_dbg[g_isr_id][2] = hi_drv_sys_get_time_stamp_ms();   /* 2: index */

            g_isr_id++;
        }
#endif
        process_end_of_pic_interupt(enc_para, ve_chn, &read_back);

        venc_drv_efl_end_one_frame_venc(g_vedu_ip_ctx.curr_handle);

        if (enc_para->omx_chn && enc_para->prepend_sps_pps_enable &&
            ((g_venc_chn[ve_chn].proc_write.time_mode_run == HI_TRUE) ||
             (g_venc_chn[ve_chn].proc_write.frame_mode_run == HI_TRUE))) {
            enc_para->prepend_sps_pps_enable = HI_FALSE;
        }

        venc_drv_rc_i_frm_insert_for_large_imb(enc_para);

        enc_para->waiting_isr = 0;
        g_vedu_ip_ctx.ip_free = 1;
        venc_drv_efl_wake_up_thread();

        venc_drv_osal_give_event(&g_venc_wait_stream[ve_chn]);

        if (g_venc_chn[ve_chn].omx_chn) {
            venc_drv_osal_give_event(&g_venc_event_stream);
        }
    }
}

static hi_bool task_cfg_reg_venc(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_u32 chn_id = 0;

    if (enc_para != NULL) {
        D_VENC_GET_CHN(chn_id, enc_para);


        if (chn_id >= HI_VENC_MAX_CHN) {
            enc_para->waiting_isr = 0;
            return HI_FAILURE;
        }

        if (g_venc_chn[chn_id].jpge == HI_INVALID_HANDLE) {
            if (venc_drv_efl_start_one_frame_venc(enc_para, enc_in) != HI_SUCCESS) {
                enc_para->waiting_isr = 0;
                return HI_FAILURE;
            }

            g_vedu_ip_ctx.curr_handle = enc_para;
            g_vedu_ip_ctx.ip_free = 0;

            venc_drv_efl_cfg_reg_venc(enc_para);
        }
    }

    return HI_SUCCESS;
}
#ifdef VENC_SUPPORT_JPGE
static hi_void task_jpge_set_enc_in(hi_jpge_venc_input_info *jpge_enc_in,
                                    vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    jpge_enc_in->y_vir_buf = enc_in->bus_vi_y;
    jpge_enc_in->cb_vir_buf = enc_in->bus_vi_c;
    jpge_enc_in->cr_vir_buf = enc_in->bus_vi_v;
    jpge_enc_in->y_stride = enc_in->vi_y_stride;
    jpge_enc_in->cbcr_stride = enc_in->vi_c_stride;

    jpge_enc_in->dst_phy_buf = enc_para->hal.strm_buf_addr;
    jpge_enc_in->dst_rp_buf = enc_para->hal.strm_buf_rp_addr;
    jpge_enc_in->dst_wp_buf = enc_para->hal.strm_buf_wp_addr;
    jpge_enc_in->dst_buf_size = enc_para->hal.strm_buf_size;
    jpge_enc_in->vir_to_phy_offset = enc_para->vir_2_bus_offset;
    jpge_enc_in->lum_qt = enc_para->jpge_yqt;
    jpge_enc_in->chm_qt = enc_para->jpge_cqt;
    jpge_enc_in->frame_width = enc_para->rc.pic_width;
    jpge_enc_in->frame_height = enc_para->rc.pic_height;
    jpge_enc_in->yuv_sample_type = enc_para->yuv_sample_type;
    jpge_enc_in->pts0 = enc_in->pts0;

    switch (enc_para->hal.yuv_store_type) {
        case VENC_STORE_SEMIPLANNAR:
            jpge_enc_in->yuv_store_type = JPGE_STORE_SEMIPLANNAR;
            break;

        case VENC_STORE_PACKAGE:
            jpge_enc_in->yuv_store_type = JPGE_STORE_PACKAGE;
            break;

        case VENC_STORE_PLANNAR:
            jpge_enc_in->yuv_store_type = JPGE_STORE_PLANNAR;
            break;

        default:
            jpge_enc_in->yuv_store_type = JPGE_STORE_SEMIPLANNAR;
            break;
    }

    jpge_enc_in->package_sel = enc_para->hal.package_sel;
    jpge_enc_in->rotation_angle = enc_para->rotation_angle;
}

static hi_s32 task_jpge_venc(vedu_efl_enc_para *enc_para, vedu_efl_enc_in *enc_in)
{
    hi_u32 chn_id = 0;
    hi_jpge_venc_input_info jpge_enc_in;
    hi_bool jpge_buf_full = HI_FALSE;
    hi_s32 ret;

    enc_para = venc_drv_efl_query_chn_jpeg(enc_in);

    if (enc_para != NULL) {
        D_VENC_GET_CHN(chn_id, enc_para);

        if (chn_id >= HI_VENC_MAX_CHN) {
            return HI_FAILURE;
        }

        if (HI_INVALID_HANDLE != g_venc_chn[chn_id].jpge) {  /* JPGE CHANNEL */
            enc_para->hal.pts0 = enc_in->pts0;
            enc_para->hal.pts1 = enc_in->pts1;

            jpge_make_jfif(enc_para);

            task_jpge_set_enc_in(&jpge_enc_in, enc_para, enc_in);



            HI_DBG_VENC("prepare to encode!wp = %x, rp = %x\n",
                        *enc_para->strm_buf_wp_vir_addr, *enc_para->strm_buf_rp_vir_addr);
            ret = g_jpge_func->drv_jpge_module_encode(g_venc_chn[chn_id].jpge, &jpge_enc_in, &jpge_buf_full);

            if (ret != HI_SUCCESS) {
                jpge_remove_jfif(enc_para);
                enc_para->stat.err_cfg_skip++;
            } else if (jpge_buf_full) {
                /* remove the heard of JFIF before */
                jpge_remove_jfif(enc_para);
                enc_para->stat.too_few_buffer_skip++;
            } else {
                HI_DBG_VENC("encode ==>success!!!!wp = %x, rp = %x, buffer_len = %x\n",
                            *enc_para->strm_buf_wp_vir_addr, *enc_para->strm_buf_rp_vir_addr,
                            enc_para->hal.strm_buf_size);
                venc_drv_efl_end_one_frame_jpge(enc_para);
            }

            /* release the img */
            enc_para->stat.put_frame_num_try++;
            (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
            enc_para->image_valid = HI_FALSE;
            enc_para->rc.rc_start = 0;
            enc_para->tr_count -= enc_para->rc.vi_frm_rate;
            enc_para->stat.put_frame_num_ok++;

            /* g_vedu_ip_ctx.ip_free = 1; */  /* NEXT */

            enc_para->waiting_isr_jpge = 0;
        }
    }

    return HI_SUCCESS;
}
#endif

static hi_void venc_drv_efl_task(hi_void)
{
    hi_s32 ret = 0;
    vedu_efl_enc_in enc_in;

    vedu_efl_enc_para *enc_para = HI_NULL;
    hi_u32 i = 0;
    hi_bool tmp_value = HI_FALSE;
    g_vedu_ip_ctx.task_running = 1;

    /* Initialize the wait queue header */
    /* CNcomment: 初始化等待队列头 */
    venc_drv_osal_init_event(&g_venc_event, 0);

    /* wait for venc start */
    while (!g_vedu_ip_ctx.stop_task) {
        for (i = 0; i < HI_VENC_MAX_CHN; i++) {
            tmp_value = tmp_value || g_venc_chn[i].enable;
        }

        if (tmp_value == HI_FALSE) {
            osal_msleep_uninterruptible(10);         /* 10: ms */
        } else {
            break;
        }
    }

    /* find valid venc handle */
    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        enc_para = (vedu_efl_enc_para *)g_venc_chn[i].venc_handle;
        if (enc_para == HI_NULL) {
            continue;
        }
    }

    while (!g_vedu_ip_ctx.stop_task) {
        /* vedu_ip for h264 */
        if (g_vedu_ip_ctx.ip_free) {
            /* if ipfree, don't irqlock */
            memset_s(&enc_in, sizeof(vedu_efl_enc_in), 0, sizeof(vedu_efl_enc_in));
            enc_para = venc_drv_efl_query_chn_h264(&enc_in, HI_FALSE);

            if (task_cfg_reg_venc(enc_para, &enc_in) != HI_SUCCESS) {
                continue;
            }
        }

#ifdef VENC_SUPPORT_JPGE
        ret = task_jpge_venc(enc_para, &enc_in);
        if (ret != HI_SUCCESS) {
            continue;
        }
#endif
        /* if ipfree, sleep */
        ret = venc_drv_osal_wait_event(&g_venc_event, 15);  /* 15: timeout */

        if (ret != 0) {
            HI_INFO_VENC("wait time out!\n");
        }

        /* osal_msleep_uninterruptible(10*1000); */
    }

    g_vedu_ip_ctx.task_running = 0;
    return;
}

static vedu_efl_enc_para *venc_drv_efl_query_chn_stream(hi_venc_msg_info *msg_info)
{
    hi_u32 start_query_no = 0;
    hi_s32 start_chn_id = 0;     /* this ID correspond to g_vedu_chn_ctx(class:vedu_efl_chn_ctx) */
    hi_u32 chn_id = 0;             /* this ID correspond to g_venc_chn(class:optm_venc_chn) */
    vedu_lock_flag flag;
    vedu_efl_enc_para *enc_para = NULL;
    vedu_efl_enc_para *venc_handle = HI_NULL;

    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);
    for (start_query_no = 0; start_query_no < HI_VENC_MAX_CHN; start_query_no++) {
        D_VENC_GET_CHN_FROM_TAB(start_chn_id, start_query_no);
        if (start_chn_id == INVAILD_CHN_FLAG) { continue; }
        D_VENC_GET_CHN(chn_id, g_vedu_chn_ctx[start_chn_id].enc_handle);
        if (chn_id >= HI_VENC_MAX_CHN) {
            venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
            return HI_NULL;
        }

        if (!g_venc_chn[chn_id].omx_chn) { continue; }

        venc_handle = g_vedu_chn_ctx[start_chn_id].enc_handle;
        enc_para = venc_handle;

        enc_para->stat.get_stream_num_try++;

        if (strm_get_written_length(enc_para->strm_mng) > 0) {
            if (venc_drv_mng_queue_empty(enc_para->stream_queue_omx)) { continue; }

            venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
            if (venc_drv_mng_dequeue(enc_para->stream_queue_omx, msg_info, sizeof(hi_venc_msg_info))) {
                venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);
                continue;
            }
            venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);

            enc_para->stream_queue_omx->stm_lock = 1;   /* mutex with the stop processing */
            enc_para->stat.stream_queue_num--;

            enc_para->stat.get_stream_num_ok++;

            break; /* find channel:start_chn_id  have buffer to fill */
        }
    }

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);

    if (start_query_no == HI_VENC_MAX_CHN) {
        return HI_NULL;
    } else {
        return venc_handle;
    }
}

static hi_void h264_get_strm_buf_flag(hi_u32 nalu_type, hi_bool *first_nal2_send, hi_venc_user_buf *stream_buf)
{
    switch (nalu_type) {
        case H264_NALU_TYPE_IDR:
            {
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_SYNCFRAME;
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_ENDOFFRAME;
            }
            break;

        case H264_NALU_TYPE_P:
            stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_ENDOFFRAME;
            break;

        case H264_NALU_TYPE_SPS:
        case H264_NALU_TYPE_PPS:
            if (*first_nal2_send) {
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_STARTTIME;
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_CODECCONFIG;
                *first_nal2_send = HI_FALSE;
            } else {
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_CODECCONFIG;
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_EXTRADATA;
            }
            break;

        default:
            HI_ERR_VENC("nalu.nalu_type(%d) is invalid\n", nalu_type);
            break;
    }
}

static hi_void h265_get_strm_buf_flag(hi_u32 nalu_type, hi_bool *first_nal2_send, hi_venc_user_buf *stream_buf)
{
    switch (nalu_type) {
        case H265_NALU_TYPE_IDR:
            {
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_SYNCFRAME;
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_ENDOFFRAME;
            }
            break;

        case H265_NALU_TYPE_P:
            stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_ENDOFFRAME;
            break;

        case H265_NALU_TYPE_SPS:
        case H265_NALU_TYPE_PPS:
            if (*first_nal2_send) {
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_STARTTIME;
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_CODECCONFIG;
                *first_nal2_send = HI_FALSE;
            } else {
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_CODECCONFIG;
                stream_buf->flags |=  HI_VENC_OMX_BUFFERFLAG_EXTRADATA;
            }
            break;

        default:
            HI_ERR_VENC("nalu.nalu_type(%d) is invalid\n", nalu_type);
            break;
    }
}

static hi_void venc_drv_omx_get_strm_buf_flag(hi_u32 protocol, hi_u32 nalu_type,
                                              hi_bool *first_nal2_send, hi_venc_user_buf *stream_buf)
{
    if (protocol == VEDU_H264) {
        h264_get_strm_buf_flag(nalu_type, first_nal2_send, stream_buf);
    } else {
        h265_get_strm_buf_flag(nalu_type, first_nal2_send, stream_buf);
    }
}

static hi_void efl_task_stream_list_buf(vedu_efl_enc_para *enc_para,
                                        hi_venc_user_buf *stream_buf, hi_void *venc_stream_addr)
{
    vedu_efl_nalu nalu = {{NULL, NULL}};
    hi_s32 ret = 0;

    if (strm_get_written_length(enc_para->strm_mng) > 0) {
        ret = venc_drv_efl_acq_read_buffer(enc_para, &nalu);
        if ((ret == HI_SUCCESS) && (nalu.slc_len[0] > 0) && (nalu.slc_len[0] <= SLICE_SIZE_LIMIT) &&
            (stream_buf->buffer_size >= nalu.slc_len[0])) {
            memcpy_s(venc_stream_addr, nalu.slc_len[0], nalu.virt_addr[0], nalu.slc_len[0]);
            stream_buf->data_len = nalu.slc_len[0];
        } else {
            HI_ERR_VENC("stream_buf.buffer_size(%d) smaller than slice_len(%d).\n",
                        stream_buf->buffer_size, nalu.slc_len[0]);
            if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx,
                                         HI_VENC_MSG_RESP_FLUSH_OUTPUT_DONE, ret, stream_buf) != HI_SUCCESS) {
                HI_ERR_VENC("put msg omx err!\n");
            }
            enc_para->stat.fbd_cnt++;
            return;
        }

        stream_buf->time_stamp0 = nalu.pts0;
        stream_buf->time_stamp1 = nalu.pts1;

        if (venc_drv_efl_rls_read_buffer(enc_para, &nalu) != HI_SUCCESS) {
            HI_ERR_VENC("omx release buffer failed!");
        }

        venc_drv_omx_get_strm_buf_flag(enc_para->protocol, nalu.nalu_type, &enc_para->first_nal_2_send, stream_buf);

        if (nalu.ext_flag & HI_VENC_OMX_BUFFERFLAG_EOS) {
            stream_buf->flags |= HI_VENC_OMX_BUFFERFLAG_EOS;
            if (nalu.ext_fill_len == 0) {
                stream_buf->data_len = 0;
            }
        }

        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx,
                                     HI_VENC_MSG_RESP_OUTPUT_DONE, ret, stream_buf) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->stat.fbd_cnt++;
    } else {
        HI_INFO_VENC("get stream failed!\n");
    }
}

static hi_s32 venc_drv_omx_output_check(hi_u32 protocol, hi_u32 nalu_type, hi_u32 package_flag)
{
    hi_s32 ret = HI_FAILURE;

    switch (package_flag) {
        case OMX_PACKAGE_ONEINONEOUT:
            if (protocol == VEDU_H265) {
                if ((nalu_type != H265_NALU_TYPE_VPS)
                    && (nalu_type != H265_NALU_TYPE_SPS)
                    && (nalu_type != H265_NALU_TYPE_PPS)) {
                    ret = HI_SUCCESS;
                }
            } else {
                /* H264 */
                if ((nalu_type != H264_NALU_TYPE_SPS) && (nalu_type != H264_NALU_TYPE_PPS)) {
                    ret = HI_SUCCESS;
                }
            }
            break;

        case OMX_PACKAGE_SPLIT:
            if (protocol == VEDU_H265) {
                if ((nalu_type != H265_NALU_TYPE_VPS)
                    && (nalu_type != H265_NALU_TYPE_SPS)) {
                    ret = HI_SUCCESS;
                }
            } else {
                /* H264 */
                if ((nalu_type != H264_NALU_TYPE_SPS)) {
                    ret = HI_SUCCESS;
                }
            }
            break;

        case OMX_PACKAGE_ALONE:
            ret = HI_SUCCESS;
            break;

        default:
            HI_ERR_VENC("not support this package_flag = %d\n", package_flag);
            ret = HI_SUCCESS;
            break;
    }

    return ret;
}

static hi_bool check_stream_buf_size(vedu_efl_nalu *nalu, hi_u32 buffer_size)
{
    if (buffer_size >= (nalu->slc_len[0] + nalu->slc_len[1])) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_void task_stream_loop_buf_cpy_nalu(hi_void *venc_stream_addr, vedu_efl_nalu *nalu,
                                             hi_venc_user_buf *stream_buf)
{
    if (nalu->slc_len[0] > 0) {
        memcpy_s(venc_stream_addr, nalu->slc_len[0], nalu->virt_addr[0], nalu->slc_len[0]);
        stream_buf->data_len += nalu->slc_len[0];
        venc_stream_addr += nalu->slc_len[0];
    }

    if (nalu->slc_len[1] > 0) {
        memcpy_s(venc_stream_addr, nalu->slc_len[1], nalu->virt_addr[1], nalu->slc_len[1]);
        stream_buf->data_len += nalu->slc_len[1];
        venc_stream_addr += nalu->slc_len[1];
    }
}

static hi_void efl_task_stream_loop_buf(vedu_efl_enc_para *enc_para, hi_venc_user_buf *stream_buf,
                                        hi_void *venc_stream_addr)
{
    hi_s32 ret = 0;
    hi_s32 ret2 = 0;
    vedu_efl_nalu nalu = {{NULL, NULL}};

    do {
        if (strm_get_written_length(enc_para->strm_mng) > 0) {
            memset_s(&nalu, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu));

            ret = venc_drv_efl_acq_read_buffer(enc_para, &nalu);
            if (check_stream_buf_size(&nalu, stream_buf->buffer_size) == HI_TRUE) {
                task_stream_loop_buf_cpy_nalu(venc_stream_addr, &nalu, stream_buf);
            } else {
                HI_ERR_VENC("stream_buf.buffer_size(%d) smaller than slice_len(%d).\n",
                            stream_buf->buffer_size, (nalu.slc_len[0] + nalu.slc_len[1]));
            }

            stream_buf->time_stamp0 =  nalu.pts0;
            stream_buf->time_stamp1 =  nalu.pts1;

            if (venc_drv_efl_rls_read_buffer(enc_para, &nalu) != HI_SUCCESS) {
                HI_ERR_VENC("omx release buffer failed!");
            }
        } else {
            ret2 = venc_drv_osal_wait_event(&g_venc_event_stream, 10);  /* 10: timeout */

            if (ret2 != 0) {
                HI_INFO_VENC("wait stream time out!\n");
            }
        }

#ifdef __OMXVENC_ONE_IN_ONE_OUT__
    }while ((!ret) && (venc_drv_omx_output_check(enc_para->protocol, nalu.nalu_type,
                                                 OMX_PACKAGE_ONEINONEOUT) == HI_FAILURE));
#else
    }while ((!ret) && (venc_drv_omx_output_check(enc_para->protocol, nalu.nalu_type, OMX_PACKAGE_SPLIT) == HI_FAILURE));
#endif

        venc_drv_omx_get_strm_buf_flag(enc_para->protocol, nalu.nalu_type, &enc_para->first_nal_2_send, stream_buf);
        if (nalu.ext_flag & HI_VENC_OMX_BUFFERFLAG_EOS) {
            stream_buf->flags |= HI_VENC_OMX_BUFFERFLAG_EOS;
            if (nalu.ext_fill_len == 0) {
                stream_buf->data_len = 0;
            }
        }

        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx,
                            HI_VENC_MSG_RESP_OUTPUT_DONE, ret, stream_buf) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->stat.fbd_cnt++;
}

static hi_void efl_task_stream_recv_strm(vedu_efl_enc_para *enc_para,
    hi_venc_user_buf *stream_buf, hi_void *venc_stream_addr)
{
    if (g_vedu_ip_ctx.is_chip_id_v500_r001 == HI_TRUE) {
            efl_task_stream_list_buf(enc_para, stream_buf, venc_stream_addr);
        } else {
            efl_task_stream_loop_buf(enc_para, stream_buf, venc_stream_addr);
        }

        enc_para->stream_queue_omx->stm_lock = 0;
        enc_para->stat.msg_queue_num++;

#ifdef __VENC_DRV_DBG__
        if (put_msg_fbd_id < 100) {     /* 100: put_msg_fbd_id */
            g_tmp_time_dbg[put_msg_fbd_id][4] = hi_drv_sys_get_time_stamp_ms(); /* 4: index */

            put_msg_fbd_id++;
        }
#endif
}

static hi_void task_stream_recv_strm(vedu_efl_enc_para *enc_para, hi_venc_msg_info *msg_info)
{
    hi_venc_user_buf stream_buf = {0};
    hi_void *venc_stream_addr = NULL;
    hi_u32 find_buf;
    hi_bool mutex_flag;
    hi_s32 ret;
    hi_u32 i;

    memcpy_s(&stream_buf, sizeof(hi_venc_user_buf), &(msg_info->buf), sizeof(hi_venc_user_buf));
    venc_stream_addr = (hi_void *)(uintptr_t)(stream_buf.buffer_addr_phy + stream_buf.virt_to_phy_offset);
    mutex_flag = HI_TRUE;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        mutex_flag = HI_FALSE;
        goto put_msg;
    }

    find_buf = 0;
    for (i = 0; i < OMX_OUTPUT_PORT_BUFFER_MAX; i++) {
        if (enc_para->omx_output_buf[i].start_phy_addr == stream_buf.buffer_addr_phy) {
            find_buf = 1;
            break;
        }
    }

    if (find_buf) {
        efl_task_stream_recv_strm(enc_para, &stream_buf, venc_stream_addr);
    } else {
put_msg:
        ret = 0;
        if (venc_drv_efl_put_msg_omx(enc_para->msg_queue_omx,
                                     HI_VENC_MSG_RESP_OUTPUT_DONE, ret, &stream_buf) != HI_SUCCESS) {
            HI_ERR_VENC("put msg omx err!\n");
        }
        enc_para->stat.fbd_cnt++;

    }
    if (mutex_flag != HI_FALSE) {
        osal_sem_up(&g_venc_mutex);
    }
}

static hi_void venc_drv_efl_task_stream(hi_void)
{
    vedu_efl_enc_para *enc_handle = HI_NULL;
    hi_u32 i = 0;
    hi_s32 ret = 0;
    hi_bool tmp_value = HI_FALSE;
    hi_venc_msg_info msg_info;

    g_vedu_ip_ctx.task_strm_running = 1;

    /* wait for venc start */
    while (!g_vedu_ip_ctx.stop_task) {
        for (i = 0; i < HI_VENC_MAX_CHN; i++) {
            tmp_value = tmp_value || (g_venc_chn[i].enable && g_venc_chn[i].omx_chn);
        }

        if (tmp_value == HI_FALSE) {
            osal_msleep_uninterruptible(10); /* 10: timeout */
        } else {
            break;
        }
    }

    while (!g_vedu_ip_ctx.stop_task) {
        /* if ipfree, don't irqlock */
        enc_handle = venc_drv_efl_query_chn_stream(&msg_info);
        if (enc_handle != NULL) {
            task_stream_recv_strm(enc_handle, &msg_info);
        } else {
            /* if channel not ready, sleep */
            /* osal_msleep_uninterruptible(10); */
            ret = venc_drv_osal_wait_event(&g_venc_event_stream, 10); /* 10: timeout */
            if (ret != 0) {
                HI_INFO_VENC("wait stream time out!\n");
            }
        }
    }

    g_vedu_ip_ctx.task_strm_running = 0;

    return;
}

hi_s32 venc_drv_efl_acq_read_buffer(vedu_efl_enc_para *venc_chn, vedu_efl_nalu *vedu_packet)
{
    hi_s32 ret;
    strm_mng_stream_info stream_info;

    D_VENC_CHECK_PTR(venc_chn);

    set_stream_info(venc_chn, &stream_info);
    ret = strm_acq_read_buffer(venc_chn->strm_mng, &stream_info, vedu_packet);

    venc_chn->stat.stream_total_byte = stream_info.stream_total_byte;

    if (ret == HI_SUCCESS) {
        if (venc_drv_dbg_save_stream(vedu_packet, venc_chn) != HI_SUCCESS) {
            HI_ERR_VENC("channel save stream failed\n");
        }
    }

    return ret;
}

hi_s32 venc_drv_efl_rls_read_buffer(vedu_efl_enc_para *venc_chn, vedu_efl_nalu *vedu_packet)
{
    hi_s32 ret;
    strm_mng_stream_info stream_info;

    set_stream_info(venc_chn, &stream_info);
    ret = strm_rls_read_buffer(venc_chn->strm_mng, &stream_info, vedu_packet);

    return ret;
}

/******************************************************************************
function   :
description: IP-VEDU & IP-JPGE open & close
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32 venc_drv_efl_open_vedu(hi_void)
{
    hi_u32 i;
    hi_s32 ret;
    hi_chip_type chip_type = 0;
    hi_chip_version chip_version = 0;

    /* create channel control mutex */
    if (venc_drv_osal_lock_create(&g_vedu_ip_ctx.chn_lock) == HI_FAILURE) {
        return HI_FAILURE;
    }

    if (venc_drv_osal_lock_create(&g_rc_lock) == HI_FAILURE) {
        return HI_FAILURE;
    }

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);

    /* initial func */
    if (is_chip_id_v500_r001(chip_type, chip_version)) {
        g_vedu_ip_ctx.is_chip_id_v500_r001 = HI_TRUE;
        drv_venc_pdt_v500_r001_init();
    } else {
        g_vedu_ip_ctx.is_chip_id_v500_r001 = HI_FALSE;
        drv_venc_pdt_v500_r001_init();
    }

    if (VENC_HAL_INIT() != HI_SUCCESS) {
        HI_ERR_VENC("hal init failure!\n");

        venc_drv_osal_lock_destory(g_vedu_ip_ctx.chn_lock);

        return HI_FAILURE;
    }

    if (strm_init() != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* set ip free status */
    g_vedu_ip_ctx.ip_free = 1;

    /* clear channel status */
    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        g_vedu_chn_ctx[i].enc_handle = NULL;
    }

    /* init IRQ */
    VENC_HAL_DISABLE_INT(INTERRUPT_TYPE_ALL);
    VENC_HAL_CLEAR_INT(INTERRUPT_TYPE_ALL);

    ret = venc_drv_osal_irq_int(VEDU_IRQ_ID, venc_isr);
    if (ret < 0) {
        HI_ERR_VENC("request irq failed! return %d\n", ret);
        return HI_FAILURE;
    }

    /* creat thread to manage channel */
    g_vedu_ip_ctx.stop_task = 0;
    g_vedu_ip_ctx.task_running = 0;
    g_vedu_ip_ctx.task_strm_running = 0;

    rc_client_init_task_state();

    venc_drv_osal_create_task(&g_vedu_ip_ctx.task_frame, "hi_venc_frame_task", venc_drv_efl_task);
    osal_msleep_uninterruptible(1);
    venc_drv_osal_create_task(&g_vedu_ip_ctx.task_stream, "hi_venc_stream_task", venc_drv_efl_task_stream);

    return HI_SUCCESS;
}

hi_void venc_drv_efl_close_vedu(hi_void)
{
    hi_u32 recycle_cnt = 0;

    g_vedu_ip_ctx.stop_task = 1;

    while ((g_vedu_ip_ctx.task_running) && (recycle_cnt < 50)) {        /* 50: recycle_cnt */
        recycle_cnt++;

        osal_msleep_uninterruptible(1);
    }

    if (recycle_cnt == 50) {        /* 50: recycle_cnt */
        HI_INFO_VENC("vedu close timeout! force to close vedu\n");
    }

    recycle_cnt = 0;
    while ((g_vedu_ip_ctx.task_strm_running) && (recycle_cnt < 50)) {    /* 50: recycle_cnt */
        recycle_cnt++;

        osal_msleep_uninterruptible(1);
    }

    if (recycle_cnt == 50) {        /* 50: recycle_cnt */
        HI_INFO_VENC("vedu close timeout! force to close vedu[task_strm_running]\n");
    }

    rc_client_deinit_task_state();

    venc_drv_osal_delete_task(g_vedu_ip_ctx.task_frame);
    venc_drv_osal_delete_task(g_vedu_ip_ctx.task_stream);

    VENC_HAL_DISABLE_INT(INTERRUPT_TYPE_ALL);
    VENC_HAL_CLEAR_INT(INTERRUPT_TYPE_ALL);

    venc_drv_osal_irq_free(VEDU_IRQ_ID);

    VENC_HAL_DEINIT();

    strm_deinit();

    venc_drv_osal_lock_destory(g_vedu_ip_ctx.chn_lock);
    venc_drv_osal_lock_destory(g_rc_lock);

    return;
}

hi_void venc_drv_efl_suspend_vedu(hi_void)
{
    hi_u32 recycle_cnt = 0;

    g_vedu_ip_ctx.stop_task = 1;

    while ((g_vedu_ip_ctx.task_running) && (recycle_cnt < 50)) {        /* 50: recycle_cnt */
        recycle_cnt++;

        osal_msleep_uninterruptible(1);
    }

    if (recycle_cnt == 50) {        /* 50: recycle_cnt */
        HI_INFO_VENC("vedu close timeout! force to close vedu\n");
    }

    recycle_cnt = 0;
    while ((g_vedu_ip_ctx.task_strm_running) && (recycle_cnt < 50)) {   /* 50: recycle_cnt */
        recycle_cnt++;

        osal_msleep_uninterruptible(1);
    }

    if (recycle_cnt == 50) {        /* 50: recycle_cnt */
        HI_INFO_VENC("vedu close timeout! force to close vedu[task_strm_running]\n");
    }

    rc_client_deinit_task_state();

    venc_drv_osal_delete_task(g_vedu_ip_ctx.task_frame);
    venc_drv_osal_delete_task(g_vedu_ip_ctx.task_stream);

    VENC_HAL_DISABLE_INT(INTERRUPT_TYPE_ALL);
    VENC_HAL_CLEAR_INT  (INTERRUPT_TYPE_ALL);

    venc_drv_osal_irq_free(VEDU_IRQ_ID);

    return ;
}

/*****************************************************************************
 prototype    : venc_drv_resume
 description  : VENC resume function
 input        : none
 output       : none
 return value : none
 others       : delete initialization of global value compared with vedu_efl_open_vedu
*****************************************************************************/
hi_s32 venc_drv_efl_resume_vedu(hi_void)
{
    /* init IRQ */
    if (venc_drv_osal_irq_int(VEDU_IRQ_ID, venc_isr) == HI_FAILURE) {
        return HI_FAILURE;
    }

    /* creat thread to manage channel */
    g_vedu_ip_ctx.stop_task = 0;
    rc_client_init_task_state();
    venc_drv_osal_create_task(&g_vedu_ip_ctx.task_frame, "hi_venc_frame_task", venc_drv_efl_task);
    osal_msleep_uninterruptible(1);
    venc_drv_osal_create_task(&g_vedu_ip_ctx.task_stream, "hi_venc_stream_task", venc_drv_efl_task_stream);

    return HI_SUCCESS;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32 venc_drv_efl_query_stat_info(vedu_efl_enc_para *enc_handle, vedu_state_info *stat_info)
{
    vedu_efl_enc_para *enc_para = enc_handle;

    if ((enc_para == NULL) || (stat_info == NULL)) {
        return HI_FAILURE;
    }

    enc_para->stat.used_stream_buf = strm_get_written_length(enc_para->strm_mng);
    *stat_info = enc_para->stat;

    return HI_SUCCESS;
}

static hi_s32 get_frame_phy_addr_base(hi_drv_video_frame *frame)
{
    hi_s32 ret;
    venc_buffer venc_buf;

    venc_buf.fd = frame->buf_addr[0].start_addr;
    ret = hi_drv_venc_handle_map(&venc_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("Map frame physical address failed.\n");
        return ret;
    }

    frame->buf_addr[0].dma_handle = venc_buf.dma_buf;
    frame->buf_addr[0].vir_addr_y = (hi_u64)venc_buf.start_vir_addr + frame->buf_addr[0].y_offset;
    frame->buf_addr[0].vir_addr_c = (hi_u64)venc_buf.start_vir_addr + frame->buf_addr[0].c_offset;

    frame->video_private.venc_private.data = venc_buf.start_phy_addr;

    return HI_SUCCESS;
}


/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32 venc_drv_efl_queue_frame(vedu_efl_enc_para *enc_handle, hi_drv_video_frame *frame)
{
    vedu_efl_enc_para *enc_para = NULL;
    hi_s32 ret;
    hi_u32 i;

    D_VENC_CHECK_PTR(frame);

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((g_venc_chn[i].venc_handle == enc_handle) && (g_venc_chn[i].venc_handle != NULL)) {
            enc_para = enc_handle;
            break;
        }
    }
    if ((i == HI_VENC_MAX_CHN) || (enc_para == NULL)) {
        HI_ERR_VENC(" bad handle!!\n");
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (get_frame_phy_addr_base(frame) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = venc_drv_mng_queue(enc_para->frame_queue, 0, frame, 0, 0, sizeof(hi_drv_video_frame));
    if (ret == HI_SUCCESS) {
        enc_para->stat.queue_num++;
        venc_drv_efl_wake_up_thread();
    }

    return ret;
}

hi_s32 venc_drv_efl_q_frame_by_attach(hi_u32 enc_usr_handle, hi_drv_video_frame *frame)
{
    hi_s32 ch_indx = 0;
    hi_s32 ret;
    unsigned long flags;

    D_VENC_CHECK_PTR(frame);

    while (ch_indx < HI_VENC_MAX_CHN) {
        if (g_venc_chn[ch_indx].user_handle == enc_usr_handle) {
            break;
        }

        ch_indx++;
    }

    if (ch_indx >= HI_VENC_MAX_CHN) {
        HI_ERR_VENC(" bad handle!!\n");

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (g_venc_chn[ch_indx].venc_handle == NULL) {
        return HI_ERR_VENC_CHN_NO_ATTACH;
    }

    if (HI_VENC_MAX_CHN  == ch_indx) {
        HI_ERR_VENC(" bad handle!!\n");

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (g_venc_chn[ch_indx].src_mod_id >= HI_ID_MAX) {
        return HI_ERR_VENC_CHN_NO_ATTACH;
    }

    osal_spin_lock_irqsave(&g_send_frame_lock[ch_indx], &flags);
    ret = venc_drv_efl_queue_frame(g_venc_chn[ch_indx].venc_handle, frame);
    osal_spin_unlock_irqrestore(&g_send_frame_lock[ch_indx], &flags);

    return ret;
}

static hi_void clean_frame_phy_addr_info(hi_drv_video_frame *frame)
{
    venc_buffer venc_buf;

    venc_buf.dma_buf = frame->buf_addr[0].dma_handle;
    hi_drv_venc_handle_unmap(&venc_buf);

    frame->buf_addr[0].dma_handle = 0;
    frame->buf_addr[0].vir_addr_y = 0;
    frame->buf_addr[0].vir_addr_c = 0;
    frame->video_private.venc_private.data = 0;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32  venc_drv_efl_dequeue_frame(vedu_efl_enc_para *enc_handle, hi_drv_video_frame *frame)
{
    vedu_efl_enc_para  *enc_para = NULL;
    hi_s32 ret;
    hi_u32 i;

    D_VENC_CHECK_PTR(frame);

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((g_venc_chn[i].venc_handle == enc_handle) && (g_venc_chn[i].venc_handle != NULL)) {
            enc_para = enc_handle;
            break;
        }
    }

    if ((i == HI_VENC_MAX_CHN) || (enc_para == NULL)) {
        HI_ERR_VENC(" bad handle!!\n");
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_mng_dequeue(enc_para->frame_dequeue, frame, sizeof(hi_drv_video_frame));
    if (ret == HI_SUCCESS) {
        clean_frame_phy_addr_info(frame);
        enc_para->stat.dequeue_num--;
    }

    return ret;
}


hi_void venc_drv_efl_rls_all_frame(vedu_efl_enc_para *enc_handle)
{
    vedu_lock_flag flag;
    vedu_efl_enc_para  *enc_para = NULL;
    hi_s32 ret = 0;
    hi_u32 i;
    hi_u32 cnt = 0;
    hi_drv_video_frame frame_info;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((g_venc_chn[i].venc_handle == enc_handle) && (g_venc_chn[i].venc_handle != NULL)) {
            enc_para = enc_handle;
            break;
        }
    }

    if ((i == HI_VENC_MAX_CHN) || ((enc_para == NULL))) {
        HI_ERR_VENC(" bad handle!!\n");
        return;
    }

    venc_drv_osal_lock(g_vedu_ip_ctx.chn_lock, &flag);

    if (enc_para->image_valid) {
        (enc_para->src_info.put_image)(enc_para->src_info.handle, &enc_para->image);
        enc_para->image_valid = HI_FALSE;
    }

    while ((!venc_drv_mng_queue_empty(enc_para->frame_queue)) && (!ret)  && (cnt < 100)) {      /* 100: cnt */
        cnt++;
        ret = venc_drv_mng_dequeue(enc_para->frame_queue, &frame_info, sizeof(hi_drv_video_frame));
        if (!ret) {
            (enc_para->src_info.put_image)(enc_para->src_info.handle, &frame_info);
            if (g_venc_chn[i].src_mod_id < HI_ID_MAX) {
                enc_para->stat.queue_num--;
            }
        }
    }

    venc_drv_osal_unlock(g_vedu_ip_ctx.chn_lock, &flag);
    return;
}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32 venc_drv_efl_get_image(hi_s32 enc_usr_handle, hi_drv_video_frame *frame)
{
    hi_u32 chn_id;
    hi_s32 ret;
    vedu_efl_enc_para *enc_para = HI_NULL;

    D_VENC_GET_CHN_BY_UHND(chn_id, enc_usr_handle);
    D_VENC_CHECK_CHN(chn_id);

    if (frame == NULL) {
        HI_ERR_VENC(" frame info is null!!\n");
        return HI_ERR_VENC_INVALID_PARA;
    }

    enc_para = g_venc_chn[chn_id].venc_handle;
    if (enc_para == NULL) {
        HI_ERR_VENC(" enc_para is null!!\n");
        return HI_ERR_VENC_INVALID_CHNID;
    }

    ret = venc_drv_mng_dequeue(enc_para->frame_queue, frame, sizeof(hi_drv_video_frame));
    if (ret) {
        HI_INFO_VENC("NOT FOR OMX FREAM!!\n");
        return ret;
    }

    enc_para->stat.queue_num--;
    enc_para->image_valid = HI_TRUE;

    return ret;
}

hi_s32 venc_drv_efl_get_image_omx(hi_s32 enc_usr_handle, hi_venc_user_buf *frame)
{
    hi_u32 chn_id;
    hi_s32 ret;
    vedu_efl_enc_para  *enc_para = NULL;
    hi_venc_msg_info  queue_data;

    D_VENC_GET_CHN_BY_UHND(chn_id, enc_usr_handle);
    D_VENC_CHECK_CHN(chn_id);

    if (frame == NULL) {
        HI_ERR_VENC(" frame info is null!!\n");
        return HI_ERR_VENC_INVALID_PARA;
    }

    enc_para = g_venc_chn[chn_id].venc_handle;
    if (enc_para == NULL) {
        HI_ERR_VENC(" enc_para is null!!\n");
        return HI_ERR_VENC_INVALID_CHNID;
    }

    if (venc_drv_mng_queue_empty(enc_para->frame_queue_omx)) {
        return HI_FAILURE;
    }

    ret = venc_drv_mng_dequeue(enc_para->frame_queue_omx, &queue_data, sizeof(hi_venc_msg_info));
    if (ret) {
        HI_INFO_VENC("err:not match for OMX!! ret = %d\n", ret);
        return ret;
    } else {
        memcpy_s(frame, sizeof(hi_venc_user_buf), &(queue_data.buf), sizeof(hi_venc_user_buf));
    }

    enc_para->stat.queue_num--;
    enc_para->image_valid = HI_TRUE;

    return HI_SUCCESS;

}

/******************************************************************************
function   :
description:
calls      :
input      :
output     :
return     :
others     :
******************************************************************************/
hi_s32 venc_drv_efl_put_image(hi_s32 enc_usr_handle, hi_drv_video_frame *frame)
{
    hi_u32 chn_id;
    hi_s32 ret;
    vedu_efl_enc_para *enc_para = HI_NULL;

    D_VENC_GET_CHN_BY_UHND(chn_id, enc_usr_handle);
    D_VENC_CHECK_CHN(chn_id);

    if (frame == NULL) {
        HI_ERR_VENC(" frame info is null!!\n");
        return HI_ERR_VENC_INVALID_PARA;
    }

    enc_para = g_venc_chn[chn_id].venc_handle;
    if (enc_para == NULL) {
        HI_ERR_VENC(" enc_para is null!!\n");
        return HI_ERR_VENC_INVALID_CHNID;
    }

    ret = venc_drv_mng_queue(enc_para->frame_dequeue, 0, frame, 0, 0, sizeof(hi_drv_video_frame));
    if (!ret) {
        enc_para->stat.dequeue_num++;
        enc_para->image_valid = HI_FALSE;
    }

    return ret;
}

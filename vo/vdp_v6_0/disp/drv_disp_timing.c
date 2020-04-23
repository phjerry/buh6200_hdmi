/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp timing
* Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_drv_disp.h"
#include "drv_disp_timing.h"
#include "drv_disp_interface.h"
#include "linux/hisilicon/securec.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/******************************************************************************
    local function and macro
******************************************************************************/

#define disp_check_null_pointer(ptr)                                              \
    do {                                                                          \
        if (ptr == HI_NULL) {                                                     \
            hi_err_disp("DISP ERROR! Input null pointer in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_NULL_PTR;                                          \
        }                                                                         \
    } while(0)

static hi_u32 g_disp_fmt_array[][2] = {
    { HI_DRV_DISP_FMT_NTSC,         1 },
    { HI_DRV_DISP_FMT_NTSC_J,       1 },
    { HI_DRV_DISP_FMT_NTSC_443,     1 },
    { HI_DRV_DISP_FMT_PAL_M,        1 },
    { HI_DRV_DISP_FMT_PAL_60,       1 },
    { HI_DRV_DISP_FMT_1440x480I_60, 2 },

    { HI_DRV_DISP_FMT_SECAM_SIN,    3 },
    { HI_DRV_DISP_FMT_SECAM_COS,    3 },
    { HI_DRV_DISP_FMT_SECAM_L,      3 },
    { HI_DRV_DISP_FMT_SECAM_B,      3 },
    { HI_DRV_DISP_FMT_SECAM_G,      3 },
    { HI_DRV_DISP_FMT_SECAM_D,      3 },
    { HI_DRV_DISP_FMT_SECAM_K,      3 },
    { HI_DRV_DISP_FMT_SECAM_H,      3 },
    { HI_DRV_DISP_FMT_PAL,          3 },
    { HI_DRV_DISP_FMT_PAL_B,        3 },
    { HI_DRV_DISP_FMT_PAL_B1,       3 },
    { HI_DRV_DISP_FMT_PAL_D,        3 },
    { HI_DRV_DISP_FMT_PAL_D1,       3 },
    { HI_DRV_DISP_FMT_PAL_G,        3 },
    { HI_DRV_DISP_FMT_PAL_H,        3 },
    { HI_DRV_DISP_FMT_PAL_K,        3 },
    { HI_DRV_DISP_FMT_PAL_I,        3 },
    { HI_DRV_DISP_FMT_PAL_N,        3 },
    { HI_DRV_DISP_FMT_PAL_NC,       3 },
    { HI_DRV_DISP_FMT_1440x576I_50, 4 },

    { HI_DRV_DISP_FMT_480P_60,    5 },
    { HI_DRV_DISP_FMT_576P_50,    6 },
    { HI_DRV_DISP_FMT_720P_50,    7 },
    { HI_DRV_DISP_FMT_720P_59_94, 8 },
    { HI_DRV_DISP_FMT_720P_60,    9 },

    { HI_DRV_DISP_FMT_1080I_50,    10 },
    { HI_DRV_DISP_FMT_1080I_59_94, 11 },
    { HI_DRV_DISP_FMT_1080I_60,    12 },

    { HI_DRV_DISP_FMT_1080P_23_976, 13 },
    { HI_DRV_DISP_FMT_1080P_24,     14 },
    { HI_DRV_DISP_FMT_1080P_25,     15 },
    { HI_DRV_DISP_FMT_1080P_29_97,  16 },
    { HI_DRV_DISP_FMT_1080P_30,     17 },
    { HI_DRV_DISP_FMT_1080P_50,     18 },
    { HI_DRV_DISP_FMT_1080P_59_94,  19 },
    { HI_DRV_DISP_FMT_1080P_60,     20 },
    { HI_DRV_DISP_FMT_1080P_100,    21 },
    { HI_DRV_DISP_FMT_1080P_119_88, 22 },
    { HI_DRV_DISP_FMT_1080P_120,    23 },

    { HI_DRV_DISP_FMT_3840X2160_23_976, 24 },
    { HI_DRV_DISP_FMT_3840X2160_24,     25 },
    { HI_DRV_DISP_FMT_3840X2160_25,     26 },
    { HI_DRV_DISP_FMT_3840X2160_29_97,  27 },
    { HI_DRV_DISP_FMT_3840X2160_30,     28 },
    { HI_DRV_DISP_FMT_3840X2160_50,     29 },
    { HI_DRV_DISP_FMT_3840X2160_60,     30 },
    { HI_DRV_DISP_FMT_3840X2160_100,    31 },
    { HI_DRV_DISP_FMT_3840X2160_119_88, 32 },
    { HI_DRV_DISP_FMT_3840X2160_120,    33 },

    { HI_DRV_DISP_FMT_4096X2160_24,     34 },
    { HI_DRV_DISP_FMT_4096X2160_25,     35 },
    { HI_DRV_DISP_FMT_4096X2160_30,     36 },
    { HI_DRV_DISP_FMT_4096X2160_50,     37 },
    { HI_DRV_DISP_FMT_4096X2160_60,     38 },
    { HI_DRV_DISP_FMT_4096X2160_100,    39 },
    { HI_DRV_DISP_FMT_4096X2160_119_88, 40 },
    { HI_DRV_DISP_FMT_4096X2160_120,    41 },

    { HI_DRV_DISP_FMT_7680X4320_23_976, 42 },
    { HI_DRV_DISP_FMT_7680X4320_24,     43 },
    { HI_DRV_DISP_FMT_7680X4320_25,     44 },
    { HI_DRV_DISP_FMT_7680X4320_29_97,  45 },
    { HI_DRV_DISP_FMT_7680X4320_30,     46 },
    { HI_DRV_DISP_FMT_7680X4320_50,     47 },
    { HI_DRV_DISP_FMT_7680X4320_59_94,  48 },
    { HI_DRV_DISP_FMT_7680X4320_60,     49 },
    { HI_DRV_DISP_FMT_7680X4320_100,    50 },
    { HI_DRV_DISP_FMT_7680X4320_119_88, 51 },
    { HI_DRV_DISP_FMT_7680X4320_120,    52 },

    { HI_DRV_DISP_FMT_1080P_24_FP, 53 },
    { HI_DRV_DISP_FMT_720P_60_FP,  54 },
    { HI_DRV_DISP_FMT_720P_50_FP,  55 },

    { HI_DRV_DISP_FMT_861D_640X480_60,      56 },
    { HI_DRV_DISP_FMT_VESA_800X600_60,      57 },
    { HI_DRV_DISP_FMT_VESA_1024X768_60,     58 },
    { HI_DRV_DISP_FMT_VESA_1280X720_60,     59 },
    { HI_DRV_DISP_FMT_VESA_1280X800_60,     60 },
    { HI_DRV_DISP_FMT_VESA_1280X1024_60,    61 },
    { HI_DRV_DISP_FMT_VESA_1360X768_60,     62 },
    { HI_DRV_DISP_FMT_VESA_1366X768_60,     63 },
    { HI_DRV_DISP_FMT_VESA_1400X1050_60,    64 },
    { HI_DRV_DISP_FMT_VESA_1440X900_60,     65 },
    { HI_DRV_DISP_FMT_VESA_1440X900_60_RB,  66 },
    { HI_DRV_DISP_FMT_VESA_1600X900_60_RB,  67 },
    { HI_DRV_DISP_FMT_VESA_1600X1200_60,    68 },
    { HI_DRV_DISP_FMT_VESA_1680X1050_60,    69 },
    { HI_DRV_DISP_FMT_VESA_1680X1050_60_RB, 70 },
    { HI_DRV_DISP_FMT_VESA_1920X1080_60,    71 },
    { HI_DRV_DISP_FMT_VESA_1920X1200_60,    72 },
    { HI_DRV_DISP_FMT_VESA_1920X1440_60,    73 },
    { HI_DRV_DISP_FMT_VESA_2048X1152_60,    74 },
    { HI_DRV_DISP_FMT_VESA_2560X1440_60_RB, 75 },
    { HI_DRV_DISP_FMT_VESA_2560X1600_60_RB, 76 },

    { HI_DRV_DISP_FMT_CUSTOM, 0 },
};

static disp_timing_info g_disp_format_param[] = {
    /* |--INTFACE---||---TOP---||---HORIZON----||--BOTTOM---||-PULSE-||-INVERSE-| */
    /* Iop    Vact Vbb Vfb Hact Hbb Hfb Bvact Bvbb Bvfb Hpw Vpw Hmid bIdv bIhs bIvs */
    // 0 HI_UNF_ENC_FMT_1080P_60,
    {
        HI_DRV_DISP_FMT_1080P_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        148500,
        16,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 576I: HDMI输出要求hmid=300, 而YPbPr要求hmid=0,
    // 考虑一般用户不会使用HDMI输出576I，所以不支持HDMI_567I输出，选择hmid=0
    // 1 HI_UNF_ENC_FMT_NTSC(4:3)
    {
        HI_DRV_DISP_FMT_NTSC,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT601_525, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT601_525
        },
        HI_TRUE,
        5994,
        13500,
        6,
        { 4, 3 },
        { 0, 240, 18, 4, 720, 119, 19, 240, 19, 4, 62, 3, 310, 0, 1, 1 },
    },

    // 576I: HDMI输出要求hmid=300, 而YPbPr要求hmid=0,
    // 考虑一般用户不会使用HDMI输出576I，所以不支持HDMI_567I输出，选择hmid=0
    // 2 HI_UNF_ENC_FMT_NTSC(4:3)
    {
        HI_DRV_DISP_FMT_NTSC,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT601_525, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT601_525
        },
        HI_TRUE,
        5994,
        13500,
        6,
        { 4, 3 },
        { 0, 240, 18, 4, 720, 119, 19, 240, 19, 4, 62, 3, 310, 0, 1, 1 },
    },

    // 3 HI_UNF_ENC_FMT_PAL(4:3)
    {
        HI_DRV_DISP_FMT_PAL,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT601_525, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT601_525
        },
        HI_TRUE,
        5000,
        13500,
        21,
        { 4, 3 },
        { 0, 288, 22, 2, 720, 132, 12, 288, 23, 2, 63, 3, 300, 0, 1, 1 },
    },

    // 4  HI_UNF_ENC_FMT_PAL(4:3)
    {
        HI_DRV_DISP_FMT_PAL,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT601_525, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT601_525
        },
        HI_TRUE,
        5000,
        13500,
        21,
        { 4, 3 },
        { 0, 288, 22, 2, 720, 132, 12, 288, 23, 2, 63, 3, 300, 0, 1, 1 },
    },

    /* |--INTFACE---||----TOP----||---HORIZON---||--BOTTOM----||-PULSE-||-INVERSE-| */
    /* Synm Iop Itf Vact Vbb Vfb Hact Hbb Hfb Bvact Bvbb Bvfb  Hpw Vpw Hmid bIdv bIhs bIvs */
    // 5 HI_UNF_ENC_FMT_480P_60,(4:3)
    {
        HI_DRV_DISP_FMT_480P_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT601_525, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT601_525
        },
        HI_FALSE,
        6000,
        27000,
        2,
        { 4, 3 },
        { 1, 480, 36, 9, 720, 122, 16, 1, 1, 1, 62, 6, 1, 0, 1, 1 },
    },

    /* Synm Iop  Itf  Vact Vbb Vfb Hact Hbb Hfb Bvact Bvbb Bvfb Hpw Vpw Hmid bIdv bIhs bIvs */
    // 6 HI_UNF_ENC_FMT_576P_50,  (4:3)
    {
        HI_DRV_DISP_FMT_576P_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT601_525, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT601_525
        },
        HI_FALSE,
        5000,
        27000,
        17,
        { 4, 3 },
        { 1, 576, 44, 5, 720, 132, 12, 1, 1, 1, 64, 5, 1, 0, 1, 1 },
    },

    // 7 HI_UNF_ENC_FMT_720P_50
    {
        HI_DRV_DISP_FMT_720P_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5000,
        74250,
        19,
        { 16, 9 },
        { 1, 720, 25, 5, 1280, 260, 440, 1, 1, 1, 40, 5, 1, 0, 0, 0 },
    },

    // 8 720P  59.94
    {
        HI_DRV_DISP_FMT_720P_59_94,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5994,
        74176,
        4,
        { 16, 9 },
        { 1, 720, 25, 5, 1280, 260, 110, 1, 1, 1, 40, 5, 1, 0, 0, 0 },
    },

    // 9 HI_UNF_ENC_FMT_720P_60
    {
        HI_DRV_DISP_FMT_720P_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        74250,
        4,
        { 16, 9 },
        { 1, 720, 25, 5, 1280, 260, 110, 1, 1, 1, 40, 5, 1, 0, 0, 0 },
    },

    // 10 HI_UNF_ENC_FMT_1080i_50
    {
        HI_DRV_DISP_FMT_1080I_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_TRUE,
        5000,
        74250,
        20,
        { 16, 9 },
        { 0, 540, 20, 2, 1920, 192, 528, 540, 21, 2, 44, 5, 1128, 0, 0, 0 },
    },

    // 11  1080i 59.94
    {
        HI_DRV_DISP_FMT_1080I_59_94,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_TRUE,
        5994,
        74176,
        5,
        { 16, 9 },
        { 0, 540, 20, 2, 1920, 192, 88, 540, 21, 2, 44, 5, 908, 0, 0, 0 },
    },

    // 12 HI_UNF_ENC_FMT_1080i_60
    {
        HI_DRV_DISP_FMT_1080I_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_TRUE,
        6000,
        74250,
        5,
        { 16, 9 },
        { 0, 540, 20, 2, 1920, 192, 88, 540, 21, 2, 44, 5, 908, 0, 0, 0 },
    },

    // 13 1080P 23.976,
    {
        HI_DRV_DISP_FMT_1080P_23_976,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2397,
        74176,
        32,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 638, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 14 HI_UNF_ENC_FMT_1080P_24 @74.25MHz,
    {
        HI_DRV_DISP_FMT_1080P_24,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2400,
        74250,
        32,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 638, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 15 HI_UNF_ENC_FMT_1080P_25,
    {
        HI_DRV_DISP_FMT_1080P_25,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2500,
        74250,
        33,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 528, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 16 1080P 29.97
    {
        HI_DRV_DISP_FMT_1080P_29_97,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2997,
        74176,
        34,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 17 HI_UNF_ENC_FMT_1080P_30
    {
        HI_DRV_DISP_FMT_1080P_30,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        3000,
        74250,
        34,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 18 HI_UNF_ENC_FMT_1080P_50,
    {
        HI_DRV_DISP_FMT_1080P_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5000,
        148500,
        31,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 528, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 19 1080p 59.94,
    {
        HI_DRV_DISP_FMT_1080P_59_94,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5994,
        148352,
        16,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 20 HI_UNF_ENC_FMT_1080P_60,
    {
        HI_DRV_DISP_FMT_1080P_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        148500,
        16,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 21 HI_DRV_DISP_FMT_1080P_100
    {
        HI_DRV_DISP_FMT_1080P_100,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        10000,
        297000,
        64,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 528, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 22 HI_DRV_DISP_FMT_1080P_119_88
    {
        HI_DRV_DISP_FMT_1080P_119_88,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        11988,
        296703,
        63,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 638, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    // 23 HI_DRV_DISP_FMT_1080P_120
    {
        HI_DRV_DISP_FMT_1080P_120,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        12000,
        297000,
        63,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },

    /* 24, 3840X2160 23.976 */
    {
        HI_DRV_DISP_FMT_3840X2160_23_976,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2397,
        296703,
        103,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 1276, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 25  4k*2k 24hz,
    {
        HI_DRV_DISP_FMT_3840X2160_24,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2400,
        297000,
        93,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 1276, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 26 4k*2k 25hz,
    {
        HI_DRV_DISP_FMT_3840X2160_25,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2500,
        297000,
        94,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 1056, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 27 4k*2k 29.97hz,
    {
        HI_DRV_DISP_FMT_3840X2160_29_97,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2997,
        296703,
        105,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 176, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 28 4k*2k 30hz,
    {
        HI_DRV_DISP_FMT_3840X2160_30,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        3000,
        297000,
        95,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 176, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 29 3840x2160@50Hz,
    {
        HI_DRV_DISP_FMT_3840X2160_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5000,
        594000,
        96,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 1056, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 30 3840x2160@60Hz,
    {
        HI_DRV_DISP_FMT_3840X2160_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        594000,
        97,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 176, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 31 3840x2160@100Hz,
    {
        HI_DRV_DISP_FMT_3840X2160_100,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        10000,
        1188000,
        117,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 1056, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 32 3840x2160@119.88Hz,
    {
        HI_DRV_DISP_FMT_3840X2160_119_88,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        11988,
        1186812,
        118,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 176, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 33 3840x2160@120Hz,
    {
        HI_DRV_DISP_FMT_3840X2160_120,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        12000,
        1188000,
        118,
        { 16, 9 },
        { 1, 2160, 82, 8, 3840, 384, 176, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 34 4096x2160@24Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_24,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2400,
        297000,
        98,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 384, 1020, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 35 4096x2160@25Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_25,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2500,
        297000,
        99,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 216, 968, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 36 4096x2160@30Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_30,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        3000,
        297000,
        100,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 216, 88, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 37 4096x2160@50Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5000,
        594000,
        101,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 216, 968, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 38 4096x2160@60Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        594000,
        102,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 216, 88, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 39 4096x2160@100Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_100,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        10000,
        1188000,
        218,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 384, 800, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 40 4096x2160@119.88Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_119_88,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        11988,
        1186812,
        219,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 216, 88, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 41 4096x2160@120Hz,
    {
        HI_DRV_DISP_FMT_4096X2160_120,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        12000,
        1188000,
        219,
        { 16, 9 },
        { 1, 2160, 82, 8, 4096, 216, 88, 1, 1, 1, 88, 10, 1, 0, 0, 0 },
    },

    // 42  7680X4320 23.976
    {
        HI_DRV_DISP_FMT_7680X4320_23_976,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2397,
        1186812,
        194,
        { 16, 9 },
        { 1, 4320, 164, 16, 7680, 768, 2552, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 43  7680X4320 24
    {
        HI_DRV_DISP_FMT_7680X4320_24,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2400,
        1188000,
        194,
        { 16, 9 },
        { 1, 4320, 164, 16, 7680, 768, 2552, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 44  7680X4320 25
    {
        HI_DRV_DISP_FMT_7680X4320_25,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2500,
        1188000,
        195,
        { 16, 9 },
        { 1, 4320, 64, 16, 7680, 768, 2352, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 45  7680X4320 29.97
    {
        HI_DRV_DISP_FMT_7680X4320_29_97,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2997,
        1186812,
        196,
        { 16, 9 },
        { 1, 4320, 64, 16, 7680, 768, 552, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 46  7680X4320 30
    {
        HI_DRV_DISP_FMT_7680X4320_30,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        3000,
        1188000,
        196,
        { 4, 3 },
        { 1, 4320, 64, 16, 7680, 768, 552, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 47  7680X4320 50
    {
        HI_DRV_DISP_FMT_7680X4320_50,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5000,
        2376000,
        198,
        { 16, 9 },
        { 1, 4320, 64, 16, 7680, 768, 2352, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 48  7680X4320 59.94
    {
        HI_DRV_DISP_FMT_7680X4320_59_94,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5994,
        2373624,
        199,
        { 16, 9 },
        { 1, 4320, 64, 16, 7680, 768, 552, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 49  7680X4320 60
    {
        HI_DRV_DISP_FMT_7680X4320_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        2376000,
        199,
        { 16, 9 },
        { 1, 4320, 64, 16, 7680, 768, 552, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 50  7680X4320 100
    {
        HI_DRV_DISP_FMT_7680X4320_100,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        10000,
        4752000,
        200,
        { 16, 9 },
        { 1, 4320, 164, 16, 7680, 768, 2112, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 51  7680X4320 119.88
    {
        HI_DRV_DISP_FMT_7680X4320_119_88,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        11988,
        4747248,
        201,
        { 16, 9 },
        { 1, 4320, 164, 16, 7680, 768, 352, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    // 52  7680X4320 120
    {
        HI_DRV_DISP_FMT_7680X4320_120,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        12000,
        4752000,
        201,
        { 16, 9 },
        { 1, 4320, 164, 16, 7680, 768, 352, 1, 1, 1, 176, 20, 1, 0, 0, 0 },
    },

    /* Synm Iop  Itf  Vact Vbb Vfb Hact  Hbb Hfb Bvact Bvbb Bvfb Hpw Vpw Hmid bIdv bIhs bIvs */
    // 53 HI_UNF_ENC_FMT_1080P_24_FP @74.25MHz,
    {
        HI_DRV_DISP_FMT_1080P_24_FP,
        HI_DRV_DISP_STEREO_FRAME_PACKING,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        2400,
        148500,
        32,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 638, 1080, 41, 4, 44, 5, 1, 0, 0, 0 },
    },

    // 54 HI_UNF_ENC_FMT_720P_60_FP
    {
        HI_DRV_DISP_FMT_720P_60_FP,
        HI_DRV_DISP_STEREO_FRAME_PACKING,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        148500,
        69,
        { 16, 9 },
        { 1, 720, 25, 5, 1280, 260, 110, 720, 25, 5, 40, 5, 1, 0, 0, 0 },
    },

    // 55 HI_UNF_ENC_FMT_720P_50_FP
    /* Synm Iop Itf Vact Vbb Vfb Hact Hbb Hfb Bvact Bvbb Bvfb Hpw Vpw Hmid bIdv bIhs bIvs */
    {
        HI_DRV_DISP_FMT_720P_50_FP,
        HI_DRV_DISP_STEREO_FRAME_PACKING,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_YUV, HI_DRV_COLOR_LIMITED_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        5000,
        148500,
        68,
        { 16, 9 },
        { 1, 720, 25, 5, 1280, 260, 440, 720, 25, 5, 40, 5, 1, 0, 0, 0 },
    },

    // 480I: HDMI输出要求hmid=310, 而YPbPr要求hmid=0,
    // 考虑一般用户不会使用HDMI输出480I，所以不支持HDMI_480I输出，选择hmid=0

    /* ============================================= */
    // 56, LCD
    {
        HI_DRV_DISP_FMT_861D_640X480_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        25175,
        1,
        { 16, 9 },
        { 1, 480, 35, 10, 640, 144, 16, 1, 1, 1, 96, 2, 1, 0, 1, 1 },
    },

    // 57
    {
        HI_DRV_DISP_FMT_VESA_800X600_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        40000,
        257,
        { 16, 9 },
        { 1, 600, 27, 1, 800, 216, 40, 1, 1, 1, 128, 4, 1, 0, 0, 0 },
    },
    // 58
    {
        HI_DRV_DISP_FMT_VESA_1024X768_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        48400,
        268,
        { 16, 9 },
        { 1, 768, 35, 3, 1024, 296, 24, 1, 1, 1, 136, 6, 1, 0, 1, 1 },
    },
    // 59
    {
        HI_DRV_DISP_FMT_VESA_1280X720_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        74250,
        286,
        { 16, 9 },
        { 1, 720, 25, 5, 1280, 260, 110, 1, 1, 1, 40, 5, 1, 0, 0, 0 },
    },
    // 60
    {
        HI_DRV_DISP_FMT_VESA_1280X800_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        83500,
        291,
        { 16, 9 },
        { 1, 800, 28, 3, 1280, 328, 72, 1, 1, 1, 128, 6, 1, 0, 0, 0 },
    },
    // 61
    {
        HI_DRV_DISP_FMT_VESA_1280X1024_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        108000,
        298,
        { 16, 9 },
        { 1, 1024, 41, 1, 1280, 360, 48, 1, 1, 1, 112, 3, 1, 0, 0, 0 },
    },
    // 62
    {
        HI_DRV_DISP_FMT_VESA_1360X768_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        85500,
        301,
        { 16, 9 },
        { 1, 768, 24, 3, 1360, 368, 64, 1, 1, 1, 112, 6, 1, 0, 0, 0 },
    },
    // 63
    {
        HI_DRV_DISP_FMT_VESA_1366X768_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        85500,
        303,
        { 16, 9 },
        { 1, 768, 27, 3, 1366, 356, 70, 1, 1, 1, 143, 3, 1, 0, 0, 0 },
    },
    // 64
    {
        HI_DRV_DISP_FMT_VESA_1400X1050_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        121750,
        304,
        { 16, 9 },
        { 1, 1050, 36, 3, 1400, 376, 88, 1, 1, 1, 144, 4, 1, 0, 0, 0 },
    },
    // 65
    {
        HI_DRV_DISP_FMT_VESA_1440X900_60_RB,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        88750,
        308,
        { 16, 9 },
        { 1, 900, 31, 3, 1440, 384, 80, 1, 1, 1, 152, 6, 1, 0, 0, 0 },
    },
    // 66
    {
        HI_DRV_DISP_FMT_VESA_1440X900_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        106500,
        308,
        { 16, 9 },
        { 1, 900, 23, 3, 1440, 112, 48, 1, 1, 1, 32, 6, 1, 0, 0, 0 },
    },
    // 67
    {
        HI_DRV_DISP_FMT_VESA_1600X900_60_RB,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        97750,
        312,
        { 16, 9 },
        { 1, 900, 99, 1, 1600, 176, 24, 1, 1, 1, 80, 3, 1, 0, 0, 0 },
    },
    // 68
    {
        HI_DRV_DISP_FMT_VESA_1600X1200_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        162000,
        313,
        { 16, 9 },
        { 1, 1200, 49, 1, 1600, 496, 64, 1, 1, 1, 192, 3, 1, 0, 0, 0 },
    },
    // 69
    {
        HI_DRV_DISP_FMT_VESA_1680X1050_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        146250,
        319,
        { 16, 9 },
        { 1, 1050, 36, 3, 1680, 456, 104, 1, 1, 1, 176, 6, 1, 0, 0, 0 },
    },
    // 70
    {
        HI_DRV_DISP_FMT_VESA_1680X1050_60_RB,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        119000,
        343,
        { 16, 9 },
        { 1, 1050, 36, 3, 1680, 456, 104, 1, 1, 1, 176, 6, 1, 0, 0, 0 },
    },

    /* |--INTFACE---||-----TOP-----||---HORIZON----||---BOTTOM----||-PULSE-||-INVERSE-| */
    /* Synm Iop  Itf  Vact Vbb Vfb Hact Hbb Hfb Bvact Bvbb Bvfb Hpw Vpw Hmid bIdv bIhs bIvs */
    // 11 HI_UNF_ENC_FMT_480P_60,
    // 71
    {
        HI_DRV_DISP_FMT_VESA_1920X1080_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        148500,
        329,
        { 16, 9 },
        { 1, 1080, 41, 4, 1920, 192, 88, 1, 1, 1, 44, 5, 1, 0, 0, 0 },
    },
    // 72
    {
        HI_DRV_DISP_FMT_VESA_1920X1200_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        193250,
        330,
        { 16, 9 },
        { 1, 1200, 32, 3, 1920, 112, 48, 1, 1, 1, 32, 6, 1, 0, 0, 0 },
    },
    /* Synm Iop Itf Vact Vbb Vfb Hact Hbb Hfb Bvact Bvbb Bvfb Hpw Vpw Hmid bIdv bIhs bIvs */
    // 73
    {
        HI_DRV_DISP_FMT_VESA_1920X1440_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        234000,
        334,
        { 16, 9 },
        { 1, 1440, 59, 1, 1920, 552, 128, 1, 1, 1, 208, 3, 1, 0, 0, 0 },
    },
    // 74
    {
        HI_DRV_DISP_FMT_VESA_2048X1152_60,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        198000,
        337,
        { 16, 9 },
        { 1, 1152, 30, 3, 2048, 112, 48, 1, 1, 1, 32, 5, 1, 0, 0, 0 },
    },
    // 75       /*not support*/
    {
        HI_DRV_DISP_FMT_VESA_2560X1440_60_RB,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        238750,
        342,
        { 16, 9 },
        { 1, 1440, 39, 2, 2560, 112, 48, 1, 1, 1, 32, 5, 1, 0, 0, 0 },
    },

    // 76       /*not support*/
    {
        HI_DRV_DISP_FMT_VESA_2560X1600_60_RB,
        HI_DRV_DISP_STEREO_NONE,
        {
            HI_DRV_COLOR_PRIMARY_BT709, HI_DRV_COLOR_CS_RGB, HI_DRV_COLOR_FULL_RANGE,
            HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR, HI_DRV_COLOR_MATRIX_COEFFS_BT709
        },
        HI_FALSE,
        6000,
        268500,
        338,
        { 16, 9 },
        { 1, 1600, 43, 3, 2560, 112, 48, 1, 1, 1, 32, 6, 1, 0, 0, 0 },
    },
};

hi_s32 get_fmt_index(hi_drv_disp_fmt format, hi_u32 *index)
{
    hi_u32 fmt_index = 0;

    *index = -1;

    disp_check_null_pointer(index);

    if (format > HI_DRV_DISP_FMT_CUSTOM) {
        hi_err_disp("eFmt out of range.\n");
        return HI_FAILURE;
    }

    for (fmt_index = 0; fmt_index < sizeof(g_disp_fmt_array) / sizeof(g_disp_fmt_array[0]); fmt_index++) {
        if (format == g_disp_fmt_array[fmt_index][0]) {
            *index = g_disp_fmt_array[fmt_index][1];
            break;
        }
    }

    if (fmt_index > sizeof(g_disp_fmt_array) / sizeof(g_disp_fmt_array[0])) {
        hi_err_disp("Failed to search efmt:%d, Array size:%d, %d!\n", format, sizeof(g_disp_fmt_array),
                    sizeof(g_disp_fmt_array[0]));
        return HI_ERR_DISP_NOT_SUPPORT_FMT;
    }

    if (*index == -1) {
        hi_err_disp("No support Fmt");
        return HI_ERR_DISP_NOT_SUPPORT_FMT;
    }

    return HI_SUCCESS;
}

hi_s32 get_format_cfg_para(hi_u32 index, disp_timing_info *disp_format_param)
{
    if (index >= sizeof(g_disp_format_param) / sizeof(disp_timing_info)) {
        return HI_FAILURE;
    }

    *disp_format_param = g_disp_format_param[index];
    return HI_SUCCESS;
}

hi_s32 drv_disp_timing_get_timing_info(hi_drv_disp_fmt format,
                                       hi_drv_disp_static_timing *disp_timing,
                                       disp_timing_info *timing_info)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 index = 0;

    if (format >= HI_DRV_DISP_FMT_BUTT) {
        hi_err_disp("format(%d) is invalid.\n", format);
        return HI_FAILURE;
    }

    disp_check_null_pointer(disp_timing);
    disp_check_null_pointer(timing_info);

    ret = get_fmt_index(format, &index);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = get_format_cfg_para(index, timing_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    disp_timing->disp_3d_mode = timing_info->disp_3d_mode;
    disp_timing->vic_num = timing_info->vic_num;
    disp_timing->disp_fmt = timing_info->disp_fmt;
    disp_timing->interlace = timing_info->interlace;

    if ((timing_info->disp_fmt >= HI_DRV_DISP_FMT_NTSC)
        && (timing_info->disp_fmt <= HI_DRV_DISP_FMT_1440x576I_50)) {
        disp_timing->pix_repeat = 2;
    } else {
        disp_timing->pix_repeat = 1;
    }

    disp_timing->timing.idv = timing_info->sync_info.idv;
    disp_timing->timing.ihs = timing_info->sync_info.ihs;
    disp_timing->timing.ivs = timing_info->sync_info.ivs;

    disp_timing->timing.clkreversal = timing_info->sync_info.iop;
    disp_timing->timing.pix_freq = timing_info->pixel_clk;
    disp_timing->timing.refresh_rate = timing_info->refresh_rate;
    disp_timing->timing.vfb = timing_info->sync_info.vfb;
    disp_timing->timing.vbb = timing_info->sync_info.vbb;
    disp_timing->timing.vact = timing_info->sync_info.vact;

    disp_timing->timing.hfb = timing_info->sync_info.hfb;
    disp_timing->timing.hbb = timing_info->sync_info.hbb;
    disp_timing->timing.hact = timing_info->sync_info.hact;

    disp_timing->timing.vpw = timing_info->sync_info.vpw;
    disp_timing->timing.hpw = timing_info->sync_info.hpw;

    disp_timing->aspect_ratio.aspect_ratio_h = timing_info->aspect_ratio.aspect_ratio_h;
    disp_timing->aspect_ratio.aspect_ratio_w = timing_info->aspect_ratio.aspect_ratio_w;

    return HI_SUCCESS;
}

hi_s32 drv_disp_timing_config(hi_drv_display disp, disp_timing_info *timing_info)
{
    hi_s32 ret = 0;
    hi_u32 intf_num;
    disp_get_intf intf_status;
    hi_drv_disp_intf intf_attr;

    ret = drv_disp_intf_get_status(disp, &intf_status);
    if (ret != HI_SUCCESS) {
        hi_err_disp("drv_disp_intf_get_status is error\n");
    }

    intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;

    for (intf_num = 0; intf_num < intf_status.intf_num; intf_num++) {
        if ((intf_status.intf[intf_num].intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) &&
            (intf_status.intf[intf_num].un_intf.hdmi == HI_DRV_HDMI_ID_0)) {
            intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf_attr.un_intf.hdmi = HI_DRV_HDMI_ID_0;
            break;
        } else {
            memcpy_s(&intf_attr,
                     sizeof(hi_drv_disp_intf),
                     &(intf_status.intf[intf_num]),
                     sizeof(hi_drv_disp_intf));
        }
    }

    if ((intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) ||
        (intf_attr.intf_type == HI_DRV_DISP_INTF_TYPE_MIPI)) {
        if (timing_info->interlace == HI_TRUE) {
            hi_err_disp("hdmi and mipi not support interlace fmt(%d)\n", timing_info->disp_fmt);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    ret = hal_disp_set_timing(disp, &intf_attr, timing_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("set timing is error\n");
    }

    return ret;
}

hi_s32 drv_disp_timing_get_fmt_from_vic(hi_u32 vic, hi_drv_disp_fmt *format)
{
    hi_u32 fmt_index = 0;

    disp_check_null_pointer(format);

    for (fmt_index = 0; fmt_index < (sizeof(g_disp_format_param) / sizeof(disp_timing_info)); fmt_index++) {
        if (vic == g_disp_format_param[fmt_index].vic_num) {
            *format = g_disp_format_param[fmt_index].disp_fmt;
            break;
        }
    }

    if (fmt_index ==  (sizeof(g_disp_format_param) / sizeof(disp_timing_info))) {
        hi_warn_disp("vic(%d) is err!\n", vic);
        return HI_ERR_DISP_INVALID_PARA;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

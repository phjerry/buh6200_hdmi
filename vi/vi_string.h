/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vi string
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __VI_STRING_H__
#define __VI_STRING_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

VI_STATIC hi_char *g_vi_instance_str[] = {"DEINIT", "INIT", "CREATE", "ATTACH", "START", "MAX"};

VI_STATIC hi_char *g_vi_type[] = {"MAIN", "SUB", "MAX"};

VI_STATIC hi_char *g_vi_access[] = {"UNKNOW", "TVD", "HDDEC", "HDMIRX0", "HDMIRX1", "HDMIRX2", "HDMIRX3", "MAX"};

VI_STATIC hi_char *g_vi_over_smp[] = {"1X", "2X", "4X", "MAX"};

VI_STATIC hi_char *g_vi_3d_fmt[] = {"2D", "SBS", "TAB", "FS", "FPK", "MAX"};

VI_STATIC hi_char *g_vi_picture_mode[] = {"VIDEO_MODE", "GAME_MODE", "PC_MODE", "MAX"};

VI_STATIC hi_char *g_vi_intf[] = {"UNKNOW", "FVHDE", "BT1120", "MAX"};

VI_STATIC hi_char *g_vi_source_type[] = {"DTV", "USB", "ATV", "SCART", "SVIDEO", "CVBS", "VGA", "YPBPR", "HDMI", "MAX"};

VI_STATIC hi_char *g_vi_bit_width[] = {"8BIT", "10BIT", "12BIT", "16BIT", "MAX"};

VI_STATIC hi_char *g_vi_field_mode[] = {"TOP", "BOTTOM", "ALL", "MAX"};

VI_STATIC hi_char *g_vi_color_sys[] = {"AUTO",  "PAL",    "NTSC",    "SECAM",   "PAL_M",
                                       "PAL_N", "PAL_60", "NTSC443", "NTSC_50", "MAX"};

VI_STATIC hi_char *g_vi_hdr_type[] = {"SDR", "DOLBY_BL", "DOLBY_EL", "HDR10", "HLG", "SLF", "SL_HDR", "HDR10+", "MAX"};

VI_STATIC hi_char *g_vi_quantization_range[] = {"AUTO", "LIMIT", "FULL", "MAX"};

VI_STATIC hi_char *g_vi_pix_fmt[] = {
    "RGB332",   "RGB444",   "RGB555",     "RGB565",     "BGR565",    "RGB555X",   "RGB565X",   "BGR666",   "BGR24",
    "RGB24",    "BGR32",    "RGB32",      "CLUT_1BPP",  "CLUT_2BPP", "CLUT_4BPP", "CLUT_8BPP", "ACLUT_44", "ACLUT_88",
    "ARGB4444", "ABGR4444", "RGBA4444",   "ARGB1555",   "ABGR1555",  "RGBA5551",  "ARGB8565",  "ABGR8565", "RGBA5658",
    "ARGB6666", "RGBA6666", "ARGB8888",   "ABGR8888",   "RGBA8888",  "AYUV8888",  "YUVA8888",  "GREY",     "Y4",
    "Y6",       "Y10",      "Y12",        "Y16",        "Y10BPACK",  "PAL8",      "YVU410",    "YVU420",   "YUYV",
    "YYUV",     "YVYU",     "UYVY",       "VYUY",       "YUV422P",   "YUV411P",   "Y41P",      "YUV444",   "YUV555",
    "YUV565",   "YUV32",    "YUV410",     "YUV420",     "HI240",     "HM12",      "M420",      "NV08",     "NV80",
    "NV12",     "NV21",     "NV12_411",   "NV16",       "NV61",      "NV16_2X1",  "YUV422",    "NV24",     "YUV444",
    "NV42_RGB", "NV12M",    "NV12MT",     "YUV420M",    "SBGGR8",    "SGBRG8",    "SGRBG8",    "SRGGB8",   "SBGGR10",
    "SGBRG10",  "SGRBG10",  "SRGGB10",    "SBGGR12",    "SGBRG12",   "SGRBG12",   "SRGGB12",   "YUV400 ",  "yuv410p",
    "yuv420p",  "YUV411",   "YUV422_1X2", "YUV422_2X1", "YUV_444",   "MAX"};

VI_STATIC hi_char *g_vi_color_space[] = {"YUV", "RGB", "MAX"};

VI_STATIC hi_char *g_vi_color_quantify_range[] = {"LIMIT", "FULL", "MAX"};

VI_STATIC hi_char *g_vi_color_primary[] = {"UNSPEC",   "BT601_525", "BT601_625", "BT709",   "BT2020",
                                           "1931_XYZ", "470_SYSM",  "240M",      "GENERIC", "RP431",
                                           "EG432_1",  "3213E",     "MAX"};

VI_STATIC hi_char *g_vi_color_transfer_curve[] = {"GAMMA_SDR", "GAMMA_HDR", "PQ", "HLG", "XVYCC", "MAX"};

VI_STATIC hi_char *g_vi_color_matrix_coeffs[] = {"IDENTITY", "UNSPEC",   "BT601_525", "BT601_625", "BT709",
                                                 "BT2020_N", "BT2020_C", "BT2100_I",  "USFCC",     "240M",
                                                 "YCGCO",    "ST2085",   "CHROMAT_N", "CHROMAT_C", "MAX"};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

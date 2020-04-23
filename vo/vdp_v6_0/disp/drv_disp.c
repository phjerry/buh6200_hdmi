/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: vdp
* Create: 2019-04-12
 */
#include "hi_drv_pdm.h"

/* common headers */
#include "hi_drv_module.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_drv_osal.h"
#include "hi_drv_mem.h"

#include "hi_drv_disp.h"
#include "drv_disp.h"
#include "drv_disp_ioctl.h"
#include "drv_display.h"

#include "drv_disp_ext.h"
#include "drv_win_ext.h"
#include "drv_disp_isr.h"
#include "drv_xdp_osal.h"


#include "hi_drv_stat.h"
#include "drv_disp_interface.h"
#include "xdp_ctrl.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VERSION_TIME "2019-12-23 10:31"

drv_disp_state g_disp_mod_state;
drv_disp_global_state g_disp_user_count_state;
drv_disp_global_state g_disp_kernel_count_state;
drv_disp_global_state g_disp_all_count_state;
hi_s32 g_disp_attach_count = 0;
hi_s32 g_drx_force_format_enable = HI_FALSE;

#define DISP_PROC_BASE            0
#define DISP_PROC_EXTREN_XDR_INFO 1
hi_u32 g_disp_proc_level = DISP_PROC_BASE;
#define DISP_PROC_NAME_LENGTH 25
#define DISP_PARA_LENGTH 10

typedef struct tag_disp_proc_command {
    const hi_char *command_name;
    hi_s32 (*pf_debug_fun)(hi_drv_display disp, hi_char *pArg2);
} disp_proc_command;

osal_atomic g_disp_atomic_cnt;
osal_semaphore g_disp_semaphore;

hi_s32 drv_disp_process_cmd(unsigned int cmd, hi_void *arg, drv_disp_state *disp_state, hi_bool user);
hi_s32 drv_disp_compat_process_cmd(unsigned int cmd, hi_void *arg, drv_disp_state *disp_state, hi_bool user);
hi_s32 disp_ext_open(hi_drv_display disp, drv_disp_state *disp_state, hi_bool user);
hi_s32 disp_ext_close(hi_drv_display disp, drv_disp_state *disp_state, hi_bool user);

#define DISP_DEBUG_CALLER()                                                              \
    do {                                                                                 \
        hi_warn_disp("called by id %d: %s\n", get_current()->tgid, get_current()->comm); \
    } while(0)
/* ================================================ */
#define CHECK_ARRAY_RANGE(dst, max) \
    do {                            \
        if (dst >= max) {           \
            dst = max - 1;          \
        }                           \
    } while(0)

#define DEF_DRV_DISP_PROC_FUNCTION_START_FROM_HERE

#define DISP_HDR_CFG_EOTF_TYPE_BUTT 3
hi_u8 *g_disp_hdr_cfg_eotf_type[DISP_HDR_CFG_EOTF_TYPE_BUTT] = {
    "EotfBT1886",
    "EotfPQ",
    "BUTT",
};

#define DISP_HDR_CFG_CHRM_FORMAT_BUTT 4
hi_u8 *g_disp_hdr_cfg_chrm_format[DISP_HDR_CFG_CHRM_FORMAT_BUTT] = {
    "420",
    "422",
    "444",
    "BUTT",
};

hi_u8 *g_disp_xdr_src_frm_type[HI_DRV_HDR_TYPE_MAX + 1] = {
    "SDR Frame",
    "HDR10 Frame",
    "HLG Frame",
    "CUVA Frame",
    "JTP_SL_HDR Frame",
    "HDR10PLUS Frame",
    "DOLBYVISION Frame",
    "BUTT",
};

hi_u8 *g_disp_xdr_engine_type_string[DRV_XDR_ENGINE_BUTT + 1] = {
    "AUTO",
    "SDR",
    "HDR10",
    "HLG",
    "SLF",
    "DOLBY",
    "JTP",
    "BUTT",
};

hi_u8 *g_disp_proc_hdr_priority_mode_string[HI_DRV_DISP_HDR_PRIORITY_MAX + 1] = {
    "PRIORITY VIDEO",
    "PRIORITY GRAPHIC",
    "AUTO",
    "BUTT",
};

#define DISP_HDR_CFG_CS_STRING_BUTT 4
hi_u8 *g_disp_hdr_cfg_cs_string[DISP_HDR_CFG_CS_STRING_BUTT] = {
    "SIGNAL_CS_YCBCR", /* YCbCr */
    "SIGNAL_CS_RGB",   /* RGB */
    "SIGNAL_CS_IPT",   /* IPT */
    "BUTT",
};

#define DISP_HDR_CFG_CLR_STRING_BUTT 5
hi_u8 *g_disp_hdr_cfg_clr_string[DISP_HDR_CFG_CLR_STRING_BUTT] = {
    "KClrYuv",
    "KClrRgb",
    "KClrRgba",
    "KClrIpt",
    "BUTT",
};

#define DISP_YN_STRING_BUTT 3
hi_u8 *g_disp_yn_string[DISP_YN_STRING_BUTT] = {
    "N",
    "Y",
    "BUTT"
};

hi_u8 *g_disp_layer_string[HI_DRV_DISP_LAYER_BUTT + 1] = {
    "NONE",
    "VIDEO",
    "GFX",
    "BUTT",
};

static const drv_disp_fmt_string g_fmt_string_map[] = {
    { "ntsc", HI_DRV_DISP_FMT_NTSC },
    { "NTSC_J", HI_DRV_DISP_FMT_NTSC_J },
    { "NTSC_443", HI_DRV_DISP_FMT_NTSC_443 },
    { "PAL_Nc", HI_DRV_DISP_FMT_PAL_M },
    { "PAL_60", HI_DRV_DISP_FMT_PAL_60 },
    { "1440x480i", HI_DRV_DISP_FMT_1440x480I_60 },
    { "SECAM_SIN", HI_DRV_DISP_FMT_SECAM_SIN },
    { "SECAM_COS", HI_DRV_DISP_FMT_SECAM_COS },
    { "SECAM_L", HI_DRV_DISP_FMT_SECAM_L },
    { "SECAM_B", HI_DRV_DISP_FMT_SECAM_B },
    { "SECAM_G", HI_DRV_DISP_FMT_SECAM_G },
    { "SECAM_D", HI_DRV_DISP_FMT_SECAM_D },
    { "SECAM_K", HI_DRV_DISP_FMT_SECAM_K },
    { "SECAM_H", HI_DRV_DISP_FMT_SECAM_H },

    { "pal", HI_DRV_DISP_FMT_PAL },
    { "PAL_B", HI_DRV_DISP_FMT_PAL_B },
    { "PAL_B1", HI_DRV_DISP_FMT_PAL_B1 },
    { "PAL_D", HI_DRV_DISP_FMT_PAL_D },
    { "PAL_D1", HI_DRV_DISP_FMT_PAL_D1 },
    { "PAL_G", HI_DRV_DISP_FMT_PAL_G },
    { "PAL_H", HI_DRV_DISP_FMT_PAL_H },
    { "PAL_K", HI_DRV_DISP_FMT_PAL_K },
    { "PAL_I", HI_DRV_DISP_FMT_PAL_I },
    { "PAL_M", HI_DRV_DISP_FMT_PAL_N },
    { "PAL_N", HI_DRV_DISP_FMT_PAL_NC },
    { "1440x576i", HI_DRV_DISP_FMT_1440x576I_50 },

    { "480p60",     HI_DRV_DISP_FMT_480P_60 },
    { "576p50",     HI_DRV_DISP_FMT_576P_50 },
    { "720p50",     HI_DRV_DISP_FMT_720P_50 },
    { "1280x720_59_94",   HI_DRV_DISP_FMT_720P_59_94 },
    { "720p60",     HI_DRV_DISP_FMT_720P_60 },
    { "1080i50",     HI_DRV_DISP_FMT_1080I_50 },
    { "1920x1080i_59_94", HI_DRV_DISP_FMT_1080I_59_94 },
    { "1080i60",     HI_DRV_DISP_FMT_1080I_60 },

    { "1920x1080_23_976", HI_DRV_DISP_FMT_1080P_23_976 },
    { "1080p24",     HI_DRV_DISP_FMT_1080P_24 },
    { "1080p25",     HI_DRV_DISP_FMT_1080P_25 },
    { "1920x1080_29_97",  HI_DRV_DISP_FMT_1080P_29_97 },
    { "1080p30",     HI_DRV_DISP_FMT_1080P_30 },
    { "1080p50",     HI_DRV_DISP_FMT_1080P_50 },
    { "1920x1080_59_94",  HI_DRV_DISP_FMT_1080P_59_94 },
    { "1080p60",     HI_DRV_DISP_FMT_1080P_60 },
    { "1080p100",     HI_DRV_DISP_FMT_1080P_100 },
    { "1080p119_88",     HI_DRV_DISP_FMT_1080P_119_88 },
    { "1080p120",     HI_DRV_DISP_FMT_1080P_120 },

    { "3840x2160_23_976", HI_DRV_DISP_FMT_3840X2160_23_976 },
    { "3840x2160_24",     HI_DRV_DISP_FMT_3840X2160_24 },
    { "3840x2160_25",     HI_DRV_DISP_FMT_3840X2160_25 },
    { "3840x2160_29_97",  HI_DRV_DISP_FMT_3840X2160_29_97 },
    { "3840x2160_30",     HI_DRV_DISP_FMT_3840X2160_30 },
    { "3840x2160_50",     HI_DRV_DISP_FMT_3840X2160_50 },
    { "3840x2160_60",     HI_DRV_DISP_FMT_3840X2160_60 },
    { "3840X2160_100",    HI_DRV_DISP_FMT_3840X2160_100 },
    { "3840X2160_119_88", HI_DRV_DISP_FMT_3840X2160_119_88 },
    { "3840X2160_120",    HI_DRV_DISP_FMT_3840X2160_120 },

    { "4096x2160_24",     HI_DRV_DISP_FMT_4096X2160_24 },
    { "4096x2160_25",     HI_DRV_DISP_FMT_4096X2160_25 },
    { "4096x2160_30",     HI_DRV_DISP_FMT_4096X2160_30 },
    { "4096x2160_50",     HI_DRV_DISP_FMT_4096X2160_50 },
    { "4096x2160_60",     HI_DRV_DISP_FMT_4096X2160_60 },
    { "4096X2160_100",    HI_DRV_DISP_FMT_4096X2160_100 },
    { "4096X2160_119_88", HI_DRV_DISP_FMT_4096X2160_119_88 },
    { "4096X2160_120",    HI_DRV_DISP_FMT_4096X2160_120 },

    { "7680x4320_23_976", HI_DRV_DISP_FMT_7680X4320_23_976 },
    { "7680x4320_24",     HI_DRV_DISP_FMT_7680X4320_24 },
    { "7680x4320_25",     HI_DRV_DISP_FMT_7680X4320_25 },
    { "7680x4320_29_976", HI_DRV_DISP_FMT_7680X4320_29_97 },
    { "7680x4320_30",     HI_DRV_DISP_FMT_7680X4320_30 },
    { "7680x4320_50",     HI_DRV_DISP_FMT_7680X4320_50 },
    { "7680x4320_59_94",  HI_DRV_DISP_FMT_7680X4320_59_94 },
    { "7680x4320_60",     HI_DRV_DISP_FMT_7680X4320_60 },
    { "7680x4320_100",    HI_DRV_DISP_FMT_7680X4320_100 },
    { "7680x4320_119_88", HI_DRV_DISP_FMT_7680X4320_119_88 },
    { "7680x4320_120",    HI_DRV_DISP_FMT_7680X4320_120 },

    { "1080p24fp", HI_DRV_DISP_FMT_1080P_24_FP },
    { "720p60fp", HI_DRV_DISP_FMT_720P_60_FP },
    { "720p50fp", HI_DRV_DISP_FMT_720P_50_FP },

    { "640x480", HI_DRV_DISP_FMT_861D_640X480_60 },
    { "800x600", HI_DRV_DISP_FMT_VESA_800X600_60 },
    { "1024x768", HI_DRV_DISP_FMT_VESA_1024X768_60 },
    { "vesa", HI_DRV_DISP_FMT_VESA_1280X720_60 },
    { "1280x800", HI_DRV_DISP_FMT_VESA_1280X800_60 },
    { "1280x1024", HI_DRV_DISP_FMT_VESA_1280X1024_60 },
    { "1360x768", HI_DRV_DISP_FMT_VESA_1360X768_60 },
    { "1366x768", HI_DRV_DISP_FMT_VESA_1366X768_60 },
    { "1400x1050", HI_DRV_DISP_FMT_VESA_1400X1050_60 },
    { "1440x900", HI_DRV_DISP_FMT_VESA_1440X900_60 },
    { "1440x900_RB", HI_DRV_DISP_FMT_VESA_1440X900_60_RB },
    { "1600x900_RB", HI_DRV_DISP_FMT_VESA_1600X900_60_RB },
    { "1600x1200", HI_DRV_DISP_FMT_VESA_1600X1200_60 },
    { "1680x1050", HI_DRV_DISP_FMT_VESA_1680X1050_60 },
    { "1680x1050_RB", HI_DRV_DISP_FMT_VESA_1680X1050_60_RB },
    { "1920x1080", HI_DRV_DISP_FMT_VESA_1920X1080_60 },
    { "1920x1200", HI_DRV_DISP_FMT_VESA_1920X1200_60 },
    { "1920x1440", HI_DRV_DISP_FMT_VESA_1920X1440_60 },
    { "2048x1152", HI_DRV_DISP_FMT_VESA_2048X1152_60 },
    { "2560x1440_RB", HI_DRV_DISP_FMT_VESA_2560X1440_60_RB },
    { "2560x1600_RB", HI_DRV_DISP_FMT_VESA_2560X1600_60_RB },

    { "CustomerTiming", HI_DRV_DISP_FMT_CUSTOM },
    { "BUTT", HI_DRV_DISP_FMT_BUTT },
};

hi_u8 *g_vdp_disp_mode_string[HI_DRV_DISP_STEREO_MODE_BUTT + 1] = {
    "2D",
    "FPK",
    "SBS_HALF",
    "TAB",
    "FILED_ALTE",
    "LINE_ALTE",
    "SBS_FULL",
    "L_DEPTH",
    "L_DEPTH_G_DEPTH",
    "BUTT",
};
#if 1

hi_u8 *g_vdpc_interface_string[HI_DRV_DISP_INTF_ID_MAX + 1] = {
    "YPbPr0",
    "RGB0",
    "S_VIDEO0",
    "CVBS0",
    "VGA0",
    "HDMI0",
    "HDMI1",
    "MIPI0",
    "MIPI1",
    "BT656_0",
    "BT1120_0",
    "LCD_0",
    "BUTT",
};

static const drv_disp_bit_width_string g_bit_widht_string_map[HI_DRV_PIXEL_BITWIDTH_MAX + 1] = {
    { "8bit", HI_DRV_PIXEL_BITWIDTH_8BIT },
    { "10bit", HI_DRV_PIXEL_BITWIDTH_10BIT },
    { "12bit", HI_DRV_PIXEL_BITWIDTH_12BIT },
    { "max", HI_DRV_PIXEL_BITWIDTH_MAX },
};

static const drv_disp_pixel_format_string g_pixel_format_string_map[HI_DRV_DISP_PIXEL_MAX + 1] = {
    { "rgb", HI_DRV_DISP_PIXEL_RGB },
    { "422", HI_DRV_DISP_PIXEL_YUV422 },
    { "444", HI_DRV_DISP_PIXEL_YUV444 },
    { "420", HI_DRV_DISP_PIXEL_YUV420 },
    { "max", HI_DRV_DISP_PIXEL_MAX },
};

#define DISPLAY_INVALID_ID 0xFFFFFFFFul
static hi_u32 g_disp_proc_id[HI_DRV_DISPLAY_BUTT] = { DISPLAY_INVALID_ID };

static hi_s32 drv_disp_proc_init(hi_void)
{
    hi_u32 u;

    for (u = 0; u < (hi_u32)HI_DRV_DISPLAY_BUTT; u++) {
        g_disp_proc_id[u] = DISPLAY_INVALID_ID;
    }

    return HI_SUCCESS;
}

#define PROC_NAME_LENGTH 12
static hi_void drv_disp_proc_de_init(hi_void)
{
    hi_char proc_name[PROC_NAME_LENGTH] = { 0 };
    hi_u32 u;

    for (u = 0; u < (hi_u32)HI_DRV_DISPLAY_BUTT; u++) {
        if (g_disp_proc_id[u] != DISPLAY_INVALID_ID) {
            snprintf(proc_name, PROC_NAME_LENGTH, "%s%d", "disp", u);
            proc_name[PROC_NAME_LENGTH - 1] = '\0';
            hi_drv_proc_remove_module(proc_name);
        }
    }

    return;
}

hi_char *g_vdp_disp_name[HI_DRV_DISPLAY_BUTT + 1] = { "display0", "display1",     "display2", "invalid  display" };
hi_char *g_vdp_disp_state[2] = { "Close", "Open" };
hi_char *g_vdp_disp_state_1[2] = { "Disable", "Enable" };
hi_char *g_vdp_disp_ar_mode[2] = { "Auto", "Custmer Setting" };
hi_char *g_vdp_disp_cast_alloc_mode[2] = { "DispAllocate", "UserAllocate" };
hi_u32 g_vdp_disp_cast_buffer_state[5] = { 1,              4,                  2,              3,                   4 };
hi_char *g_vdp_disp_isr_type[HI_DRV_DISP_C_TYPE_BUTT] = { "0_percent", "90_percent", "100_percent", "DHD0_WBC", "VID_WBC",
                                                          "GFX_WBC", "REG_UP", "SMMU", "RETURN_FRAME_ISR", "LOWBANDWIDTH",
                                                          "FRAME_FINISH", "FRAME_TIMEOUT0", "FRAME_TIMEOUT1", "FRAME_RXERR"
                                                        };



hi_u8 *g_disp_transfer_curve_string[HI_DRV_COLOR_TRANSFER_TYPE_MAX + 1] = {
    "GAMMA_SDR",
    "GAMMA_HDR",
    "PQ",
    "HLG",
    "XVYCC",
    "MAX",
};

hi_u8 *g_disp_color_primary_string[HI_DRV_COLOR_PRIMARY_COLOR_MAX + 1] = {
    "UNSPECIFY",
    "BT601_525",
    "BT601_625",
    "BT709",
    "BT2020",
    "CIE1931_XYZ",
    "BT470_SYSM",
    "SMPTE_240M",
    "GENERIC_FILM",
    "SMPTE_RP431",
    "SMPTE_EG432_1",
    "EBU_TECH_3213E",
    "COLOR_MAX",
};

hi_u8 *g_disp_color_matrix_coeffs_string[HI_DRV_COLOR_MATRIX_COEFFS_MAX + 1] = {
    "IDENTITY",
    "UNSPECIFY",
    "BT601_525",
    "BT601_625",
    "BT709",
    "BT2020_NON_CONSTANT",
    "BT2020_CONSTANT",
    "BT2100_ICTCP",
    "USFCC",
    "SMPTE_240M",
    "YCGCO",
    "ST2085",
    "CHROMAT_NON_CONSTANT",
    "CHROMAT_CONSTANT",
    "COEFFS_MAX",
};

hi_u8 *g_disp_color_space_string[HI_DRV_COLOR_CS_MAX + 1] = {
    "YUV",
    "RGB",
    "MAX",
};

hi_u8 *g_disp_color_crange_string[HI_DRV_COLOR_RANGE_MAX + 1] = {
    "LIMITED",
    "FULL",
    "RANGE_MAX",
};

hi_char *g_vdp_disp_hdr_type[HI_DRV_DISP_TYPE_BUTT + 1] = {
    "SDR",
    "HDR10",
    "HDR10PLUS",
    "HLG",
    "DOLBY",
    "DOLBY_LL",
    "SDR CERT",
    "HDR10 CERT",
    "TECHNICOLOR",
    "BUTT",
};

hi_char *g_vdp_disp_output_color_space[HI_DRV_DISP_COLOR_SPACE_BUTT + 1] = {
    "BT709", "BT601", "BT2020", "BUTT"
};

hi_char *g_vdp_disp_match_content_mode[HI_DRV_DISP_MATCH_CONTENT_MODE_MAX + 1] = {
    "NONE", "DEFAULT", "MAX"
};

hi_char *g_vdp_disp_aspect_mode[HI_DRV_DISP_ASPECT_RATIO_MODE_MAX + 1] = {
    "Auto", "4TO3", "16TO9", "221TO100", "CUSTOM", "MAX"
};

static disp_proc_info g_disp_attr;

hi_void disp_proc_show_timing_info(hi_void *p)
{
    hi_u32 i;

    for (i = 0; i < sizeof(g_fmt_string_map) / sizeof(drv_disp_fmt_string); i++) {
        if (g_disp_attr.fmt == g_fmt_string_map[i].fmt) {
            osal_proc_print(p, "%-20s:%s/%s/%d\n", "Formt/DispMode/pix_clk", g_fmt_string_map[i].pfmt_string,
                            g_vdp_disp_mode_string[g_disp_attr.stereo], g_disp_attr.timing.pix_freq);
            break;
        }
    }

    osal_proc_print(p, "%-20s:%s/%s\n", "debug_bit_width/debug_pixel_format",
                    g_bit_widht_string_map[g_disp_attr.data_width].pbit_width_string,
                    g_pixel_format_string_map[g_disp_attr.pixel_format].ppixel_format_string);

    osal_proc_print(p, "%-20s:\n", "------Custom Timing Para List------");
    osal_proc_print(p, "       %-20s: %d/%d/%d\n", "HACT/VACT/VFreq", g_disp_attr.timing.hact, g_disp_attr.timing.vact,
                    g_disp_attr.timing.refresh_rate);
    osal_proc_print(p, "       %-20s: %d/%d/%d\n", "HBB/HFB/HPW ", g_disp_attr.timing.hbb, g_disp_attr.timing.hfb,
                    g_disp_attr.timing.hpw);
    osal_proc_print(p, "       %-20s: %d/%d/%d\n", "VBB/VFB/VPW", g_disp_attr.timing.vbb, g_disp_attr.timing.vfb,
                    g_disp_attr.timing.vpw);
    osal_proc_print(p, "       %-20s: %d/%d/%d\n", "IDV/IHS/IVS", g_disp_attr.timing.idv, g_disp_attr.timing.ihs,
                    g_disp_attr.timing.ivs);
    return ;
}

hi_void disp_proc_show_setting_info(hi_void *p)
{
    osal_proc_print(p, "%-20s:%s\n", "RightEyeFirst", g_vdp_disp_state_1[g_disp_attr.right_eye_first]);
    osal_proc_print(p, "%-20s:%d/%d\n", "VirtualScreen", g_disp_attr.virtaul_screen.width,
                    g_disp_attr.virtaul_screen.height);
    osal_proc_print(p, "%-20s:%d/%d/%d/%d\n", "Offset(L/T/R/B)", g_disp_attr.offset_info.left,
                    g_disp_attr.offset_info.top, g_disp_attr.offset_info.right, g_disp_attr.offset_info.bottom);
    if (g_disp_proc_level > DISP_PROC_BASE) {
        osal_proc_print(p, "%-20s:%d/%d/%d/%d\n", "ActualOffset(L/T/R/B)", g_disp_attr.actual_offset_info.left,
                        g_disp_attr.actual_offset_info.top, g_disp_attr.actual_offset_info.right,
                        g_disp_attr.actual_offset_info.bottom);
    }

    osal_proc_print(p, "%-20s:%s\n", "AspectRatioMode", g_vdp_disp_ar_mode[(hi_u32)g_disp_attr.cust_aspect_ratio]);
    osal_proc_print(p, "%-20s:%d:%d\n", "AspectRatio", g_disp_attr.ar_w, g_disp_attr.ar_h);
    osal_proc_print(p, "%-20s:%s\n", "UserAspectRatioMode",  g_vdp_disp_aspect_mode[g_disp_attr.aspect_mode]);

    osal_proc_print(p, "%-20s:%d\n", "Alpha", g_disp_attr.alpha);
    osal_proc_print(p, "%-20s:0x%x/0x%x/0x%x\n", "Background (R/G/B)", g_disp_attr.bg_color.red,
                    g_disp_attr.bg_color.green,
                    g_disp_attr.bg_color.blue);
    osal_proc_print(p, "%-20s:%s->%s\n", "Zorder(Bot->Top)", g_disp_layer_string[g_disp_attr.layer[0]],
                    g_disp_layer_string[g_disp_attr.layer[1]]);

    if (g_disp_attr.master == HI_TRUE) {
        osal_proc_print(p, "%-20s:%s\n", "AttachRole", "source");
    }

    if (g_disp_attr.slave == HI_TRUE) {
        osal_proc_print(p, "%-20s:%s\n", "AttachRole", "destination");
    }

    if ((g_disp_attr.master != HI_TRUE) && (g_disp_attr.slave != HI_TRUE)) {
        osal_proc_print(p, "%-20s:%s\n", "AttachRole", "single running.");
    }

    osal_proc_print(p, "%-20s:%s\n", "AttachDisp", g_vdp_disp_name[g_disp_attr.attached_disp]);
    osal_proc_print(p, "%-20s:%s\n", "MatchMode", g_vdp_disp_match_content_mode[g_disp_attr.match_mode]);
    osal_proc_print(p, "%-20s:%s\n", "DispType", g_vdp_disp_hdr_type[g_disp_attr.disp_type]);
    osal_proc_print(p, "%-20s:%s\n", "ColorSpace",
                    g_vdp_disp_output_color_space[g_disp_attr.color_space_mode]);
    osal_proc_print(p, "%-20s:%s\n", "Actual DispType", g_vdp_disp_hdr_type[g_disp_attr.actual_output_type]);
    osal_proc_print(p, "%-20s:%s\n", "Actual ColorSpace",
                    g_vdp_disp_output_color_space[g_disp_attr.actual_output_colorspace]);
    osal_proc_print(p, "%-20s:%s\n", "ColorSpace",
                    g_disp_color_space_string[g_disp_attr.color_space.color_space]);
    osal_proc_print(p, "%-19s:%-20s\n", "range",
                    g_disp_color_crange_string[g_disp_attr.color_space.quantify_range]);
    osal_proc_print(p, "%-19s:%-20s\n", "matrix_coeffs",
                    g_disp_color_matrix_coeffs_string[g_disp_attr.color_space.matrix_coef]);
    osal_proc_print(p, "%-19s:%-20s\n", "color_primary",
                    g_disp_color_primary_string[g_disp_attr.color_space.color_primary]);
    osal_proc_print(p, "%-19s:%-20s\n", "transfer_curve",
                    g_disp_transfer_curve_string[g_disp_attr.color_space.transfer_type]);
}

hi_void disp_proc_show_isr_info(hi_void *p, disp_isr_debug *isr_debug)
{
    hi_drv_disp_callback_type isr_type = 0;

    for (isr_type = HI_DRV_DISP_C_INTPOS_0_PERCENT; isr_type < HI_DRV_DISP_C_TYPE_BUTT; isr_type++) {
        if (isr_debug->isr_state_info[isr_type].isr_type != HI_DRV_DISP_C_TYPE_NONE) {
            osal_proc_print(p, "%-10s: IsrType[%s], IsrCnt[%d], IsrExeTime[%llu], PeriodTime[%llu]", "IsrDebug",
                            g_vdp_disp_isr_type[isr_debug->isr_state_info[isr_type].isr_type - 1],
                            isr_debug->isr_state_info[isr_type].isr_cnt,
                            isr_debug->isr_state_info[isr_type].isr_time_us,
                            isr_debug->isr_state_info[isr_type].isr_period_time);

            osal_proc_print(p, "%s IsrTimeOutCnt[%d], IsrLostCnt[%d], IsrTwichEnterCnt[%d]\n", " ",
                            isr_debug->isr_state_info[isr_type].time_out,
                            isr_debug->isr_state_info[isr_type].lost_interrupt,
                            isr_debug->isr_state_info[isr_type].into_isr_twice_cnt);
        }
    }

    osal_proc_print(p, "%-10s:LowbandCount[%d], SmmuCount[%d],smmu_err_status[0x%x],reg_addr[0x%x], \
                    DeCompErr[%d], BusErr[%d]\n", "isr_debug",
                    g_disp_attr.underflow, g_disp_attr.smmu_err_cnt,
                    isr_debug->smmu_err_status, isr_debug->smmu_err_addr,
                    isr_debug->decmp_err, isr_debug->bus_err);

    return;
}

hi_s32 disp_proc_read(hi_void *p, hi_void *v)
{
    hi_drv_display disp;
    hi_s32 ret;
    disp_isr_debug isr_debug;

    if (p == HI_NULL) {
        hi_err_disp("p is null\n");
        return HI_FAILURE;
    }

    if (v == HI_NULL) {
        hi_err_disp("v is null\n");
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)v;

    memset(&isr_debug, 0, sizeof(disp_isr_debug));

    /* Disp PROC */
    ret = drv_disp_get_proc_into(disp, &g_disp_attr);
    if (ret) {
        osal_proc_print(p, "---------Get Hisilicon DISP %d Out Info Failed!---------\n", disp);
        return HI_SUCCESS;
    }

    osal_proc_print(p, "---------Hisilicon DISP %d State---------" VERSION_TIME "\n", disp);
    osal_proc_print(p, "%-20s:%s\n", "State", g_vdp_disp_state[g_disp_attr.enable]);
    osal_proc_print(p, "%-20s:%s\n", "Licensed", g_vdp_disp_state_1[g_disp_attr.licensed]);

    disp_proc_show_timing_info(p);

    disp_proc_show_setting_info(p);

    osal_proc_print(p, "%-20s:%s/%s\n", "Cvbs/Ypbpr BootClose", g_vdp_disp_state_1[g_disp_attr.boot_args_shutdown_cvbs],
                    g_vdp_disp_state_1[g_disp_attr.boot_args_shutdown_ypbpr]);

    osal_proc_print(p, "%-20s:%d\n", "InitCount", osal_atomic_read(&g_disp_atomic_cnt));
    osal_proc_print(p, "%-20s:%d/%d\n", "OpenCnt[User/Kernel]", g_disp_user_count_state.disp_open_num[disp],
                    g_disp_kernel_count_state.disp_open_num[disp]);

    (hi_void) disp_isr_get_isr_state(disp, &(g_disp_attr.underflow),
                                     &g_disp_attr.smmu_err_cnt, &isr_debug);

    disp_proc_show_isr_info(p, &isr_debug);
    (hi_void) drv_disp_intf_get_proc_info(p, disp);
    drv_xdp_ctrl_proc_info(p, disp);

    return HI_SUCCESS;
}

hi_drv_disp_fmt disp_get_fmtby_string(hi_char *fmt_string)
{
    hi_s32 index = 0;
    hi_drv_disp_fmt fmt = HI_DRV_DISP_FMT_BUTT;

    for (index = 0; index < sizeof(g_fmt_string_map) / sizeof(drv_disp_fmt_string); index++) {
        if (osal_strncmp(fmt_string, strlen(fmt_string),
            g_fmt_string_map[index].pfmt_string, strlen(fmt_string)) == 0) {
            fmt = g_fmt_string_map[index].fmt;
            break;
        }
    }

    return fmt;
}

hi_void disp_proc_print_help(hi_void)
{
    osal_printk("echo help                            > /proc/msp/dispX\n");
    osal_printk("echo fmt 1080i50/720p50/pal/ntsc/... > /proc/msp/dispX\n");
    osal_printk("echo 3d fp/sbs_hf/tab                > /proc/msp/dispX\n");
    osal_printk("echo rf on/off                       > /proc/msp/dispX\n");
    osal_printk("echo left     X                      > /proc/msp/dispX\n");
    osal_printk("echo top      X                      > /proc/msp/dispX\n");
    osal_printk("echo right    X                      > /proc/msp/dispX\n");
    osal_printk("echo bottom   X                      > /proc/msp/dispX\n");
    osal_printk("echo video up/down                   > /proc/msp/dispX\n");
    osal_printk("echo reset                           > /proc/msp/dispX\n");
    osal_printk("echo cat AbsolutePath                > /proc/msp/dispX\n");
    osal_printk("echo bkg red/green/blue/black/white  > /proc/msp/dispX\n");
    osal_printk("echo cs  bt709/bt2020/bt601          > /proc/msp/dispX\n");
    osal_printk("echo sinkcap  0~7  (dolby hdr10 hlg) > /proc/msp/dispX\n");
    osal_printk("echo bt2020  on/off/                 > /proc/msp/dispX\n");
    osal_printk("echo engine  sdr/hdr10/dolby/hlg     > /proc/msp/dispX\n");
    osal_printk("echo priority  video/gfx/auto        > /proc/msp/dispX\n");
    osal_printk("echo colorbar on/off                 > /proc/msp/dispX\n");
    osal_printk("echo cvbs on/off                     > /proc/msp/dispX\n");
    osal_printk("echo hdmi2_0 on/off                  > /proc/msp/dispX\n");
    osal_printk("echo hdmi2_1 on/off                  > /proc/msp/dispX\n");
    osal_printk("echo mipi0    on_single/on_single_dsc/on_double/off  > /proc/msp/dispX\n");
    osal_printk("echo mipi1    on_single/on_single_dsc/on_double/off  > /proc/msp/dispX\n");
    osal_printk("------------------------------------------------------\n");
    osal_printk("echo bitwidth 8/10/12       > /proc/msp/dispX\n");
    osal_printk("echo pixelfmt 420/422/444         > /proc/msp/dispX\n");
    osal_printk("echo output sdr/cert_sdr/dolby/hdr10/cert_hdr10/hlg/auto/follow_first_frm/dolby_ll > \
                /proc/msp/disp1\n");
#ifdef VDP_SDK_HAL_TEST_SUPPORT
    osal_printk("echo hal_test  on/off               > /proc/msp/dispX\n");
#endif
}

hi_s32 disp_proc_help(hi_drv_display disp, hi_char *arg2)
{
    disp_proc_print_help();
    return HI_SUCCESS;
}

hi_s32 disp_proc_set_format(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 idx;
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_fmt fmt;

    fmt = disp_get_fmtby_string(arg2);

    if (fmt == HI_DRV_DISP_FMT_BUTT) {
        osal_printk("\nPlease choose a right format:\n");
        for (idx = 0; idx < (sizeof(g_fmt_string_map) / sizeof(drv_disp_fmt_string)); idx++) {
            osal_printk("   %s\n", g_fmt_string_map[idx].pfmt_string);
        }
        osal_printk("\n");

        return HI_ERR_DISP_NOT_SUPPORT_FMT;
    } else if ((fmt >= HI_DRV_DISP_FMT_1080P_24_FP) && (fmt <= HI_DRV_DISP_FMT_720P_50_FP)) {
        ret = drv_disp_set_format(disp, HI_DRV_DISP_STEREO_FRAME_PACKING, fmt);
    } else {
        hi_drv_disp_fmt fmtold;
        hi_drv_disp_stereo_mode stereoold;

        ret = drv_disp_get_format(disp, &stereoold, &fmtold);
        if (ret == HI_SUCCESS) {
            ret = drv_disp_set_format(disp, stereoold, fmt);
        }
        if ((g_drx_force_format_enable == HI_TRUE) && (ret != HI_SUCCESS)) {
            osal_printk("dfx force set format %d\n", fmt);
            drv_disp_dfx_force_format(disp, stereoold, fmt);
            ret = HI_SUCCESS;
        }
    }

    return ret;
}

hi_s32 disp_proc_set_bit_width(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 idx;
    hi_s32 ret = HI_SUCCESS;

    if (osal_strncmp(arg2, strlen(arg2), "8", strlen(arg2)) == 0) {
        ret = drv_disp_set_bit_width(disp, HI_DRV_PIXEL_BITWIDTH_8BIT);
    } else if(osal_strncmp(arg2, strlen(arg2), "10", strlen(arg2)) == 0) {
        ret = drv_disp_set_bit_width(disp, HI_DRV_PIXEL_BITWIDTH_10BIT);
    } else if(osal_strncmp(arg2, strlen(arg2), "12", strlen(arg2)) == 0) {
        ret = drv_disp_set_bit_width(disp, HI_DRV_PIXEL_BITWIDTH_12BIT);
    } else if (osal_strncmp(arg2, strlen(arg2), "off", strlen(arg2)) == 0) {
        ret = drv_disp_set_bit_width(disp, HI_DRV_PIXEL_BITWIDTH_MAX);
    } else {
        osal_printk("\nPlease choose a right format:\n");
        for (idx = 0; idx < (sizeof(g_bit_widht_string_map) / sizeof(drv_disp_bit_width_string)); idx++) {
            osal_printk("   %s\n", g_bit_widht_string_map[idx].pbit_width_string);
        }
        osal_printk("\n");

        return HI_ERR_DISP_NOT_SUPPORT_FMT;
    }

    return ret;
}

hi_s32 disp_proc_set_pixel_format(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 idx;
    hi_s32 ret = HI_SUCCESS;

    if (osal_strncmp(arg2, strlen(arg2) - 1, "420", strlen(arg2) - 1) == 0) {
        ret = drv_disp_set_pixel_format(disp, HI_DRV_DISP_PIXEL_YUV420);
    } else if (osal_strncmp(arg2, strlen(arg2) - 1, "422", strlen(arg2) - 1) == 0) {
        ret = drv_disp_set_pixel_format(disp, HI_DRV_DISP_PIXEL_YUV422);
    } else if (osal_strncmp(arg2, strlen(arg2) - 1, "444", strlen(arg2) - 1) == 0) {
        ret = drv_disp_set_pixel_format(disp, HI_DRV_DISP_PIXEL_YUV444);
    } else if (osal_strncmp(arg2, strlen(arg2) - 1, "rgb", strlen(arg2) - 1) == 0) {
        ret = drv_disp_set_pixel_format(disp, HI_DRV_DISP_PIXEL_RGB);
    } else if (osal_strncmp(arg2, strlen(arg2) - 1, "off", strlen(arg2) - 1) == 0) {
        ret = drv_disp_set_pixel_format(disp, HI_DRV_DISP_PIXEL_MAX);
    } else {
        osal_printk("\nPlease choose a right format:\n");
        for (idx = 0; idx < (sizeof(g_pixel_format_string_map) / sizeof(drv_disp_pixel_format_string)); idx++) {
            osal_printk("   %s\n", g_pixel_format_string_map[idx].ppixel_format_string);
        }
        osal_printk("\n");

        return HI_ERR_DISP_NOT_SUPPORT_FMT;
    }

    return ret;
}

hi_s32 disp_proc_set3_dmode(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_fmt fmtold;
    hi_drv_disp_stereo_mode stereoold;

    ret = drv_disp_get_format(disp, &stereoold, &fmtold);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (osal_strncmp(arg2, strlen("2d"), "2d", strlen("2d")) == 0) {
        ret = drv_disp_set_format(disp, HI_DRV_DISP_STEREO_NONE, fmtold);
    } else if (osal_strncmp(arg2, strlen("sbs_hf"), "sbs_hf", strlen("sbs_hf")) == 0) {
        ret = drv_disp_set_format(disp, HI_DRV_DISP_STEREO_SBS_HALF, fmtold);
    } else if (osal_strncmp(arg2, strlen("tab"), "tab", strlen("tab")) == 0) {
        ret = drv_disp_set_format(disp, HI_DRV_DISP_STEREO_TAB, fmtold);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_set_right_eye_first(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        ret = drv_disp_set_right_eye_first(disp, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        ret = drv_disp_set_right_eye_first(disp, HI_FALSE);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_set_left(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_offset screenoffset;

    ret = drv_disp_get_screen_offset(disp, &screenoffset);
    if (ret == HI_SUCCESS) {
        screenoffset.left = (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH);
        ret = drv_disp_set_screen_offset(disp, &screenoffset);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_set_right(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_offset screenoffset;

    ret = drv_disp_get_screen_offset(disp, &screenoffset);
    if (ret == HI_SUCCESS) {
        screenoffset.right = (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH);
        ret = drv_disp_set_screen_offset(disp, &screenoffset);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_set_bottom(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_offset screenoffset;

    ret = drv_disp_get_screen_offset(disp, &screenoffset);
    if (ret == HI_SUCCESS) {
        screenoffset.bottom = (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH);
        ret = drv_disp_set_screen_offset(disp, &screenoffset);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_set_top(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_offset screenoffset;

    ret = drv_disp_get_screen_offset(disp, &screenoffset);
    if (ret == HI_SUCCESS) {
        screenoffset.top = (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH);
        ret = drv_disp_set_screen_offset(disp, &screenoffset);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_reset(hi_drv_display disp, hi_char *arg2)
{
    hi_err_disp("Not support set rotation now\n");
    return HI_ERR_DISP_NOT_SUPPORT;
}

hi_s32 disp_proc_cat(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_char *char_code = arg2;
    hi_drv_video_frame *cur_frame = HI_NULL;
    hi_u32 pathlength = 0;
    hi_void *h_common_handle = HI_NULL;

    cur_frame = (hi_drv_video_frame *)osal_vmalloc(HI_ID_DISP, sizeof(hi_drv_video_frame));
    if (!cur_frame) {
        hi_err_disp("alloc frame info memory failed\n");
        return ret;
    }

    /* get currently displayed frame */
    ret = drv_disp_acquire_snapshot(disp, cur_frame, &h_common_handle);
    if (ret != HI_SUCCESS) {
        hi_err_disp("catpure screen failed\n");
        osal_vfree(HI_ID_DISP, cur_frame);
        return ret;
    }

    /* calculate char nubmer of path string */
    while ((*char_code != ' ') && (*char_code != '\0')) {
        char_code++;
        pathlength++;
    }
    pathlength++;

    /* save yuv frame */
    // ret = vdp_debug_save_yuvimg(cur_frame, arg2, pathlength);

    drv_disp_release_snapshot(disp, cur_frame, h_common_handle);

    osal_vfree(HI_ID_DISP, cur_frame);

    return ret;
}

hi_s32 disp_proc_set_low_delay(hi_drv_display disp, hi_char *arg2)
{
    hi_handle hcast;
    hi_void *hcast_ptr = HI_NULL;

    if (disp != HI_DRV_DISPLAY_0) {
        return HI_FAILURE;
    }

    (hi_void) drv_disp_get_cast_handle(disp, &hcast, &hcast_ptr);
    if (osal_strncmp(arg2, strlen("start"), "start", strlen("start")) == 0) {
        if ((hcast) && (hcast_ptr)) {
            hi_drv_stat_ld_start(HI_STAT_LD_SCENES_VID_CAST, hcast);
        }
    } else if (osal_strncmp(arg2, strlen("start"), "stop", strlen("stop")) == 0) {
        hi_drv_stat_ld_stop();
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 disp_proc_set_output_type(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    if (osal_strncmp(arg2, strlen("sdr"), "sdr", strlen("sdr")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_NORMAL);
    } else if (osal_strncmp(arg2, strlen("cert_sdr"), "cert_sdr", strlen("cert_sdr")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_SDR_CERT);
    } else if (osal_strncmp(arg2, strlen("dolby_ll"), "dolby_ll", strlen("dolby_ll")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_DOLBY_LL);
    } else if (osal_strncmp(arg2, strlen("dolby"), "dolby", strlen("dolby")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_DOLBY);
    } else if (osal_strncmp(arg2, strlen("hdr10"), "hdr10", strlen("hdr10")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_HDR10);
    } else if (osal_strncmp(arg2, strlen("cert_hdr10"), "cert_hdr10", strlen("cert_hdr10")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_HDR10_CERT);
    } else if (osal_strncmp(arg2, strlen("hlg"), "hlg", strlen("hlg")) == 0) {
        ret = drv_disp_set_output(disp, HI_DRV_DISP_TYPE_HLG);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_set_xdr_engine_type(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    if (osal_strncmp(arg2, strlen("sdr"), "sdr", strlen("sdr")) == 0) {
        ret = drv_disp_set_xdr_engine_type(disp, DRV_XDR_ENGINE_SDR);
    } else if (osal_strncmp(arg2, strlen("dolby"), "dolby", strlen("dolby")) == 0) {
        ret = drv_disp_set_xdr_engine_type(disp, DRV_XDR_ENGINE_DOLBY);
    } else if (osal_strncmp(arg2, strlen("hdr10"), "hdr10", strlen("hdr10")) == 0) {
        ret = drv_disp_set_xdr_engine_type(disp, DRV_XDR_ENGINE_HDR10);
    } else if (osal_strncmp(arg2, strlen("hlg"), "hlg", strlen("hlg")) == 0) {
        ret = drv_disp_set_xdr_engine_type(disp, DRV_XDR_ENGINE_HLG);
    } else if (osal_strncmp(arg2, strlen("auto"), "auto", strlen("auto")) == 0) {
        ret = drv_disp_set_xdr_engine_type(disp, DRV_XDR_ENGINE_AUTO);
    } else {
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 disp_proc_check_reg_config(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    if (osal_strncmp(arg2, strlen("video"), "video", strlen("video")) == 0) {
        // ret = disp_check_reg_config("video");
    } else if (osal_strncmp(arg2, strlen("wbc_dhd"), "wbc_dhd", strlen("wbc_dhd")) == 0) {
        // ret = disp_check_reg_config("wbc_dhd");
    }

    return ret;
}

hi_s32 disp_proc_keep_scene_lowband(hi_drv_display disp, hi_char *arg2)
{
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        osal_printk("keep scene lowband --on --\n");
        disp_isr_set_isr_control_flag(ISRFUNC_CTRL_FLAG_STOP_WHEN_LOWBAND);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
    osal_printk("keep scene lowband --off --\n");
        disp_isr_clear_isr_control_flag(ISRFUNC_CTRL_FLAG_STOP_WHEN_LOWBAND);
    }

    return HI_SUCCESS;
}

hi_s32 disp_proc_keep_scene_smmu_err(hi_drv_display disp, hi_char *arg2)
{
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        osal_printk("keep scene smmu err --on --\n");
        disp_isr_set_isr_control_flag(ISRFUNC_CTRL_FLAG_STOP_WHEN_SMMU_ERR);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
    osal_printk("keep scene smmu err --off --\n");
        disp_isr_clear_isr_control_flag(ISRFUNC_CTRL_FLAG_STOP_WHEN_SMMU_ERR);
    }

    return HI_SUCCESS;
}

hi_s32 disp_proc_set_apha(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;

    drv_disp_set_alpha(disp, (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH));

    return ret;
}

hi_s32 disp_proc_stop_send_hdmiifo(hi_drv_display disp, hi_char *arg2)
{
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        drv_disp_set_stop_hdmiinfo(disp, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        drv_disp_set_stop_hdmiinfo(disp, HI_FALSE);
    }

    return HI_SUCCESS;
}

hi_s32 disp_proc_set_match_mode(hi_drv_display disp, hi_char *arg2)
{
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        drv_disp_set_match_content_mode(disp, HI_DRV_DISP_MATCH_CONTENT_MODE_DEFAULT);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        drv_disp_set_match_content_mode(disp, HI_DRV_DISP_MATCH_CONTENT_MODE_NONE);
    }

    return HI_SUCCESS;
}

hi_s32 disp_proc_set_isrdelay_time_ms(hi_drv_display disp, hi_char *arg2)
{
    disp_isr_check_delay_time_ms(disp, (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH));

    return HI_SUCCESS;
}

hi_s32 disp_proc_set_bkg_color(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_color bgcolor = { 0 };

    if (osal_strncmp(arg2, strlen("red"), "red", strlen("red")) == 0) {
        bgcolor.red = 0xff;
        bgcolor.green = 0;
        bgcolor.blue = 0;
        ret = drv_disp_set_bgcolor(disp, &bgcolor);
    } else if (osal_strncmp(arg2, strlen("green"), "green", strlen("green")) == 0) {
        bgcolor.red = 0;
        bgcolor.green = 0xff;
        bgcolor.blue = 0;
        ret = drv_disp_set_bgcolor(disp, &bgcolor);
    } else if (osal_strncmp(arg2, strlen("blue"), "blue", strlen("blue")) == 0) {
        bgcolor.red = 0;
        bgcolor.green = 0;
        bgcolor.blue = 0xff;
        ret = drv_disp_set_bgcolor(disp, &bgcolor);
    } else if (osal_strncmp(arg2, strlen("black"), "black", strlen("black")) == 0) {
        bgcolor.red = 0;
        bgcolor.green = 0;
        bgcolor.blue = 0;
        ret = drv_disp_set_bgcolor(disp, &bgcolor);
    } else if (osal_strncmp(arg2, strlen("white"), "white", strlen("white")) == 0) {
        bgcolor.red = 0xff;
        bgcolor.green = 0xff;
        bgcolor.blue = 0xff;
        ret = drv_disp_set_bgcolor(disp, &bgcolor);
    } else {
        hi_err_disp("Unsupport type color, will set black color!\n");
        bgcolor.red = 0;
        bgcolor.green = 0;
        bgcolor.blue = 0;
        ret = drv_disp_set_bgcolor(disp, &bgcolor);
    }

    return ret;
}

hi_s32 disp_proc_set_output_color_space(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret;
    hi_drv_disp_output_color_space color_space = HI_DRV_DISP_COLOR_SPACE_BUTT;

    if (osal_strncmp(arg2, strlen("bt709"), "bt709", strlen("bt709")) == 0) {
        color_space = HI_DRV_DISP_COLOR_SPACE_BT709;
    } else if (osal_strncmp(arg2, strlen("bt2020"), "bt2020", strlen("bt2020")) == 0) {
        color_space = HI_DRV_DISP_COLOR_SPACE_BT2020;
    } else if (osal_strncmp(arg2, strlen("bt601"), "bt601", strlen("bt601")) == 0) {
        color_space = HI_DRV_DISP_COLOR_SPACE_BT601;
    } else {
        hi_err_disp("Unsupport type color space, %s\n", arg2);
        return HI_FAILURE;
    }

    ret = drv_disp_set_output_color_space(disp, color_space);

    return ret;
}

#ifdef VDP_SDK_HAL_TEST_SUPPORT
hi_s32 disp_proc_set_test_mode(hi_drv_display disp, hi_char *arg2)
{
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        g_drx_force_format_enable = HI_TRUE;
    } else {
        g_drx_force_format_enable = HI_FALSE;
    }
    return HI_SUCCESS;
}

hi_s32 disp_proc_set_hal_test_support(hi_drv_display disp, hi_char *arg2)
{
    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        disp_isr_free_irq(HI_TRUE);
    } else {
        disp_isr_free_irq(HI_FALSE);
    }
    return HI_SUCCESS;
}
#endif

hi_s32 disp_proc_set_priority_mode(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_hdr_priority_mode priority_mode = HI_DRV_DISP_HDR_PRIORITY_MAX;

    if (osal_strncmp(arg2, strlen("video"), "video", strlen("video")) == 0) {
        priority_mode = HI_DRV_DISP_HDR_PRIORITY_VIDEO;
    } else if (osal_strncmp(arg2, strlen("gfx"), "gfx", strlen("gfx")) == 0) {
        priority_mode = HI_DRV_DISP_HDR_PRIORITY_GRAPHIC;
    } else if (osal_strncmp(arg2, strlen("auto"), "auto", strlen("auto")) == 0) {
        priority_mode = HI_DRV_DISP_HDR_PRIORITY_AUTO;
    } else {
        hi_err_disp("invalid pArg2!\n");
        priority_mode = HI_DRV_DISP_HDR_PRIORITY_VIDEO;
    }

    // ret = disp_set_hdr_priority_mode(disp, priority_mode);

    return ret;
}

hi_s32 disp_proc_set_color_bar_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;

    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        drv_disp_set_color_bar(disp, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        drv_disp_set_color_bar(disp, HI_FALSE);
    } else {
        hi_err_disp("invalid arg!\n");
    }

    return ret;
}


hi_s32 disp_proc_set_suspend_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_SUCCESS;

    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        drv_disp_suspend();
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        drv_disp_resume();
    } else {
        hi_err_disp("invalid arg!\n");
    }

    return ret;
}


hi_s32 disp_proc_set_cvbs_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_FAILURE;
    disp_intf intf_attr;

    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        intf_attr.id = HI_DRV_DISP_INTF_CVBS0;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        intf_attr.id = HI_DRV_DISP_INTF_CVBS0;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_FALSE);
    } else {
        hi_err_disp("invalid arg!\n");
    }

    return ret;
}

hi_s32 disp_proc_set_hdmi2_0_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_FAILURE;
    disp_intf intf_attr;

    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_HDMI0;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_HDMI0;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_FALSE);
    } else {
        hi_err_disp("invalid arg!\n");
    }

    return ret;
}

hi_s32 disp_proc_set_hdmi2_1_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_FAILURE;
    disp_intf intf_attr;

    if (osal_strncmp(arg2, strlen("on"), "on", strlen("on")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_HDMI1;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_HDMI1;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_FALSE);
    } else {
        hi_err_disp("invalid arg!\n");
    }

    return ret;
}

hi_s32 disp_proc_set_mipi0_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_FAILURE;
    disp_intf intf_attr;

    if (osal_strncmp(arg2, strlen("on_single"), "on_single", strlen("on_single")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI0;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_SINGLE;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("on_single_dsc"), "on_single_dsc", strlen("on_single_dsc")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI0;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_SINGLE_DSC;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("on_double"), "on_double", strlen("on_double")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI0;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_DOUBLE;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI0;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_DOUBLE;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_FALSE);
    } else {
        hi_err_disp("invalid arg\n");
    }

    return ret;
}

hi_s32 disp_proc_set_mipi1_enable(hi_drv_display disp, hi_char *arg2)
{
    hi_s32 ret = HI_FAILURE;
    disp_intf intf_attr;

    if (osal_strncmp(arg2, strlen("on_single"), "on_single", strlen("on_single")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI1;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_SINGLE;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("on_single_dsc"), "on_single_dsc", strlen("on_single_dsc")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI1;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_SINGLE_DSC;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("on_double"), "on_double", strlen("on_double")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI1;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_DOUBLE;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_TRUE);
    } else if (osal_strncmp(arg2, strlen("off"), "off", strlen("off")) == 0) {
        intf_attr.id= HI_DRV_DISP_INTF_MIPI1;
        intf_attr.mipi_mode = HI_DRV_MIPI_MODE_DOUBLE;
        ret = drv_disp_proc_set_intf_enable(disp, &intf_attr, HI_FALSE);
    } else {
        hi_err_disp("invalid arg\n");
    }

    return ret;
}

hi_s32 disp_proc_set_proc_level(hi_handle h_win, hi_char *arg2)
{
    g_disp_proc_level = (hi_u32)osal_strtol(arg2, NULL, DISP_PARA_LENGTH);

    return HI_SUCCESS;
}

hi_s32 osal_cmd_check(hi_u32 in_argc, hi_u32 aspect_argc, hi_void *arg, hi_void *private)
{
    if (in_argc < aspect_argc) {
        hi_err_disp("in_argc is %d, aspect_argc is %d\n", in_argc, aspect_argc);
        return HI_FAILURE;
    }

    if (private == HI_NULL) {
        hi_err_disp("private is null\n");
        return HI_FAILURE;
    }

    if (arg == HI_NULL) {
        hi_err_disp("arg is null\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_disp_proc_get_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 1, private, private) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_help(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_format(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_format(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_bit_width(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_bit_width(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_pixel_format(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_pixel_format(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set3_dmode(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set3_dmode(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_right_eye_first(hi_u32 argc,
                                                char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_right_eye_first(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_left(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_left(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_right(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_right(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_bottom(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_bottom(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_top(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_top(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_cat(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_cat(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_low_delay(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_low_delay(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_output_type(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_output_type(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_check_reg_config(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_check_reg_config(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_bkg_color(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_bkg_color(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_keep_scene_lowband(hi_u32 argc,
                                               char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                               hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_keep_scene_lowband(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_output_color_space(hi_u32 argc,
                                                   char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                   hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_output_color_space(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_proc_level(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_proc_level(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_apha(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_apha(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_hdr_sink_support(hi_u32 argc,
                                                 char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                 hi_void *private)
{

    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_sink_capability sink_cap = { 0 };
    hi_s32 enable;
    hi_drv_display disp;

    if (osal_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is the number of entrys */
        return HI_FAILURE;
    }
    disp = *(hi_drv_display *)private;

    ret = drv_disp_get_sink_capability(disp, &sink_cap);
    if (ret != HI_SUCCESS) {
        hi_err_disp("DISP_GetSinkCapability err 0x%x!\n", ret);
        return ret;
    }

    enable = (hi_bool)osal_strtol(argv[2], NULL, DISP_PARA_LENGTH);

    if (0 != strstr(argv[1], "sdr")) {
        sink_cap.support_sdr = enable;
    }
    if (0 != strstr(argv[1], "hdr10")) {
        sink_cap.support_hdr10 = enable;
    }
    if (0 != strstr(argv[1], "hdr10")) {
        sink_cap.support_hdr10 = enable;
    }
    if (0 != strstr(argv[1], "hdr10plus")) {
        sink_cap.support_hdr10plus = enable;
    }
    if (0 != strstr(argv[1], "hlg")) {
        sink_cap.support_hlg = enable;
    }
    if (0 != strstr(argv[1], "bt601")) {
        sink_cap.support_bt601 = enable;
    }
    if (0 != strstr(argv[1], "bt709")) {
        sink_cap.support_bt709 = enable;
    }
    if (0 != strstr(argv[1], "bt2020")) {
        sink_cap.support_bt2020 = enable;
    }
    ret = drv_disp_set_sink_capability(disp, &sink_cap);

    return ret;
}

static hi_s32 drv_disp_proc_set_xdr_engine_type(hi_u32 argc,
                                                char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_xdr_engine_type(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_stop_send_hdmiifo(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_stop_send_hdmiifo(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_isrdelay_time_ms(hi_u32 argc,
                                                 char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                 hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_isrdelay_time_ms(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_priority_mode(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_priority_mode(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_color_bar_enable(hi_u32 argc,
                                                 char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                 hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_color_bar_enable(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_cvbs_enable(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_cvbs_enable(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_hdmi2_0_enable(hi_u32 argc,
                                               char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                               hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_hdmi2_0_enable(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_hdmi2_1_enable(hi_u32 argc,
                                               char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                               hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_hdmi2_1_enable(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_mipi0_enable(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_mipi0_enable(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_mipi1_enable(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_mipi1_enable(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_debug_level(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 enable;

    if (osal_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is the number of entrys */
        return HI_FAILURE;
    }

    enable = (hi_bool)osal_strtol(argv[2], NULL, DISP_PARA_LENGTH);

    if (0 != strstr(argv[1], "all")) {
        disp_set_debug_flag(0xFFFFFFFF, enable);
    }
    if (0 != strstr(argv[1], "sync")) {
        disp_set_debug_flag(AVSYNC_INFOR, enable);
    }
    if (0 != strstr(argv[1], "buf")) {
        disp_set_debug_flag(BUFFER_INFOR, enable);
    }
    if (0 != strstr(argv[1], "vpss")) {
        disp_set_debug_flag(VPSS_INFOR, enable);
    }
    if (0 != strstr(argv[1], "attr")) {
        disp_set_debug_flag(WINATTR_INFOR, enable);
    }
    if (0 != strstr(argv[1], "ioctl")) {
        disp_set_debug_flag(IOCTL_INFOR, enable);
    }
    if (strstr(argv[1], "overlay") != 0) {
        disp_set_debug_flag(ANDROID_FENCE_INFOR, enable);
    }
    if (strstr(argv[1], "fence") != 0) {
        disp_set_debug_flag(BUFFER_FENCE_INFOR, enable);
    }
    if (0 != strstr(argv[1], "ai")) {
        disp_set_debug_flag(AI_WIN_INFOR, enable);
    }
    if (0 != strstr(argv[1], "hdr")) {
        disp_set_debug_flag(HDR_INFOR, enable);
    }
    if (0 != strstr(argv[1], "check_sum")) {
        disp_set_debug_flag(CHECK_SUM, enable);
    }
    return HI_SUCCESS;
}

static hi_s32 drv_disp_proc_set_match_mode(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_match_mode(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_keep_scene_smmu_err(hi_u32 argc,
                                                char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_keep_scene_smmu_err(disp, argv[1]);
    return ret;
}

#ifdef VDP_SDK_HAL_TEST_SUPPORT
static hi_s32 drv_disp_proc_set_test_mode(hi_u32 argc,
                                          char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                          hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_test_mode(disp, argv[1]);
    return ret;
}

static hi_s32 drv_disp_proc_set_hal_test_support(hi_u32 argc,
                                                 char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                                 hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_hal_test_support(disp, argv[1]);
    return ret;
}
#endif

static hi_s32 drv_disp_proc_set_suspend_enable(hi_u32 argc,
                                               char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                               hi_void *private)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is the number of entrys */
        return HI_FAILURE;
    }

    disp = *(hi_drv_display *)private;
    ret = disp_proc_set_suspend_enable(disp, argv[1]);
    return ret;
}

static osal_proc_cmd g_disp_proc_info[] = {
    { "help",           drv_disp_proc_get_help },
    { "fmt",   drv_disp_proc_set_format },
    { "bitwidth",   drv_disp_proc_set_bit_width },
    { "pixelfmt",   drv_disp_proc_set_pixel_format },
    { "3d",   drv_disp_proc_set3_dmode },
    { "rf",   drv_disp_proc_set_right_eye_first },
    { "left",   drv_disp_proc_set_left },
    { "right",   drv_disp_proc_set_right },
    { "bottom",   drv_disp_proc_set_bottom },
    { "top",   drv_disp_proc_set_top },
    { "cat",   drv_disp_proc_cat },
    { "low_delay_stat", drv_disp_proc_set_low_delay },
    { "output",   drv_disp_proc_set_output_type },
    { "config_check",   drv_disp_proc_check_reg_config },
    { "bkg",   drv_disp_proc_set_bkg_color },
    { "lowband",   drv_disp_proc_keep_scene_lowband },
    { "cs",   drv_disp_proc_set_output_color_space },
    { "proc",   drv_disp_proc_set_proc_level },
    { "alpha",   drv_disp_proc_set_apha },
    { "sinkcap",   drv_disp_proc_set_hdr_sink_support },
    { "engine",   drv_disp_proc_set_xdr_engine_type },
    { "stopsendhdmi",   drv_disp_proc_stop_send_hdmiifo },
    { "isr_delaytime",  drv_disp_proc_set_isrdelay_time_ms },
    { "priority",   drv_disp_proc_set_priority_mode },
    { "colorbar",   drv_disp_proc_set_color_bar_enable },
    { "cvbs",    drv_disp_proc_set_cvbs_enable },
    { "hdmi2_0",    drv_disp_proc_set_hdmi2_0_enable },
    { "hdmi2_1",    drv_disp_proc_set_hdmi2_1_enable },
    { "mipi0",    drv_disp_proc_set_mipi0_enable },
    { "mipi1",    drv_disp_proc_set_mipi1_enable },
    { "debug_ctrl",    drv_disp_proc_set_debug_level },
    { "match",    drv_disp_proc_set_match_mode },
    { "smmu",    drv_disp_proc_keep_scene_smmu_err },

#ifdef VDP_SDK_HAL_TEST_SUPPORT
    { "test_mode",   drv_disp_proc_set_test_mode },
    { "hal_test",   drv_disp_proc_set_hal_test_support },
#endif
    { "suspend",   drv_disp_proc_set_suspend_enable },
};

static hi_s32 drv_disp_proc_add(hi_drv_display disp)
{
    hi_char proc_name[DISP_PROC_NAME_LENGTH];
    osal_proc_entry *proc_item = HI_NULL;
    snprintf(proc_name, PROC_NAME_LENGTH, "%s%d", "disp", disp);
    proc_item = osal_proc_add(proc_name, strlen(proc_name));
    if (proc_item == HI_NULL) {
        hi_fatal_disp("add %s proc failed.\n", proc_name);
        return HI_ERR_DISP_CREATE_ERR;
    }

    proc_item->private = (hi_void *)drv_disp_get_channel_id(disp);
    proc_item->read = disp_proc_read;
    proc_item->cmd_list = g_disp_proc_info;
    proc_item->cmd_cnt = sizeof(g_disp_proc_info) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

static hi_s32 drv_disp_proc_del(hi_drv_display disp)
{
    hi_char proc_name[PROC_NAME_LENGTH];

    /* register HD-display PROC */
    snprintf(proc_name, PROC_NAME_LENGTH, "%s%d", "disp", disp);
    proc_name[PROC_NAME_LENGTH - 1] = '\0';

    osal_proc_remove(proc_name, strlen(proc_name));
    return HI_SUCCESS;
}
#endif

/***************************************************************/
#define DEF_DRV_DISP_FILE_FUNCTION_START_FROM_HERE

hi_s32 disp_check_para(hi_drv_display disp, drv_disp_state *disp_state)
{
    if ((disp < HI_DRV_DISPLAY_BUTT) && disp_state->disp_open[disp]) {
        return HI_SUCCESS;
    }

    return HI_ERR_DISP_INVALID_PARA;
}

hi_s32 disp_file_open(struct inode *finode, struct file *ffile)
{
    drv_disp_state *disp_state = HI_NULL;
    hi_drv_display u;

    osal_sem_down(&g_disp_semaphore);

    disp_state = osal_kmalloc(HI_ID_DISP, sizeof(drv_disp_state), OSAL_GFP_KERNEL);
    if (disp_state == HI_NULL) {
        osal_sem_up(&g_disp_semaphore);
        hi_fatal_disp("malloc pDispState failed.\n");
        return HI_ERR_DISP_MALLOC_FAILED;
    }
    memset(disp_state, 0, sizeof(drv_disp_state));

    for (u = 0; u < HI_DRV_DISPLAY_BUTT; u++) {
        disp_state->disp_open[u] = HI_FALSE;
        disp_state->hcast_handle[u] = HI_NULL;
        disp_state->h_snapshot[u] = HI_NULL;
        disp_state->hvbi[u] = HI_FALSE;
    }

    ffile->private_data = disp_state;

    osal_sem_up(&g_disp_semaphore);
    return HI_SUCCESS;
}

hi_s32 disp_file_close(struct inode *finode, struct file *ffile)
{
    drv_disp_state *disp_state;
    hi_drv_display u;
    hi_s32 ret = HI_FAILURE;

    osal_sem_down(&g_disp_semaphore);

    disp_state = ffile->private_data;

    for (u = 0; u < HI_DRV_DISPLAY_BUTT; u++) {
        /* to close cast service, no matter disp open or not. */
        if (disp_state->hcast_handle[u]) {
            ret = drv_disp_destroy_cast(disp_state->hcast_handle[u]);
            if (ret != HI_SUCCESS) {
                hi_err_disp("destroy cast  %d  failed!\n", u);
                break;
            }
        }

        /* to close snapshot service, for ctrl+c condition. */
        if (disp_state->h_snapshot[u]) {
            ret = drv_disp_destroy_snapshot(disp_state->h_snapshot[u]);
            if (ret != HI_SUCCESS) {
                hi_err_disp("destroy snapshot  %d  failed!\n", u);
                break;
            }
        }

        if (disp_state->hvbi[u]) {
            // ret = drv_vbi_de_init();
            if (ret != HI_SUCCESS) {
                hi_err_disp("destroy vbi  %d  failed!\n", u);
                break;
            }
        }

        if (disp_state->disp_open[u]) {
            ret = disp_ext_close(u, disp_state, HI_TRUE);
            if (ret != HI_SUCCESS) {
                hi_err_disp("Display %d close failed!\n", u);
                break;
            }
        }
    }

    if (ffile->private_data != HI_NULL) {
        osal_kfree(HI_ID_DISP, ffile->private_data);
        ffile->private_data = HI_NULL;
    }

    osal_sem_up(&g_disp_semaphore);
    return ret;
}

hi_slong drv_disp_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    drv_disp_state *disp_state;
    hi_s32 ret = HI_SUCCESS;

    osal_sem_down(&g_disp_semaphore);

    disp_state = file->private_data;

    ret = drv_disp_process_cmd(cmd, arg, disp_state, HI_TRUE);

    osal_sem_up(&g_disp_semaphore);
    return ret;
}
hi_slong drv_disp_compat_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    drv_disp_state *disp_state;
    hi_s32 ret = HI_SUCCESS;

    osal_sem_down(&g_disp_semaphore);

    disp_state = file->private_data;

    ret = drv_disp_compat_process_cmd(cmd, arg, disp_state, HI_TRUE);

    osal_sem_up(&g_disp_semaphore);
    return ret;
}

/***************************************************************/
#define DEF_DRV_DISP_DRV_FUNCTION_START_FROM_HERE

hi_s32 hi_drv_disp_process(hi_u32 cmd, hi_void *arg)
{
    drv_disp_state *disp_state;
    hi_s32 ret;

    osal_sem_down(&g_disp_semaphore);

    disp_state = &g_disp_mod_state;

    ret = drv_disp_process_cmd(cmd, arg, disp_state, HI_FALSE);

    osal_sem_up(&g_disp_semaphore);

    return ret;
}

hi_s32 hi_drv_disp_process_intr(hi_u32 cmd, hi_void *arg)
{
    drv_disp_state *disp_state;
    hi_s32 ret;
    disp_state = &g_disp_mod_state;
    ret = drv_disp_process_cmd(cmd, arg, disp_state, HI_FALSE);
    return ret;
}
hi_u32 disp_reset_count_status(void)
{
    hi_drv_display u;

    for (u = 0; u < HI_DRV_DISPLAY_BUTT; u++) {
        g_disp_all_count_state.disp_open_num[u] = 0;
        g_disp_user_count_state.disp_open_num[u] = 0;
        g_disp_kernel_count_state.disp_open_num[u] = 0;
        g_disp_mod_state.disp_open[u] = HI_FALSE;
        g_disp_mod_state.hcast_handle[u] = HI_NULL;
        g_disp_mod_state.h_snapshot[u] = HI_NULL;
    }

    g_disp_attach_count = 0;

    return HI_SUCCESS;
}

hi_s32 disp_ext_open(hi_drv_display disp, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    hi_bool licensed;

    if (disp >= HI_DRV_DISPLAY_2) {
        hi_err_disp("disp out of range\n");
        return HI_FAILURE;
    }

    licensed = drv_disp_is_licensed(disp);
    if (licensed == HI_FALSE) {
        hi_err_disp("disp%d is not licensed\n", disp);
        return HI_ERR_DISP_INVALID_LICENSE;
    }

    /* create DISP for the first time */
    if (disp_state->disp_open[disp] != HI_TRUE) {
        /* call basic interface for the first time creating DISP globally */
        ret = drv_disp_open(disp);
        if (ret != HI_SUCCESS) {
            hi_err_disp(" Error number is: %x.\n", ret);
            return ret;
        }

        disp_state->disp_open[disp] = HI_TRUE;

        g_disp_all_count_state.disp_open_num[disp]++;

        if (user == HI_TRUE) {
            g_disp_user_count_state.disp_open_num[disp]++;
        } else {
            g_disp_kernel_count_state.disp_open_num[disp]++;
        }
    }

    return HI_SUCCESS;
}

hi_s32 disp_ext_close(hi_drv_display disp, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;

    if (disp >= HI_DRV_DISPLAY_2) {
        hi_err_disp("disp out of range\n");
        return HI_FAILURE;
    }

    /* destroy DISP */
    if (disp_state->disp_open[disp] == HI_TRUE) {
        if (user == HI_TRUE) {
            if (g_disp_user_count_state.disp_open_num[disp] == 0) {
                hi_warn_disp("Already Close User display%d =0\n", disp);
                return HI_SUCCESS;
            }

            g_disp_user_count_state.disp_open_num[disp]--; /* User count -- */
        } else {
            if (g_disp_kernel_count_state.disp_open_num[disp] == 0) {
                hi_warn_disp("Already Close kernel display%d =0\n", disp);
                return HI_SUCCESS;
            }
            g_disp_kernel_count_state.disp_open_num[disp]--;
        }

        g_disp_all_count_state.disp_open_num[disp]--; /* Global count -- */

        if (!g_disp_all_count_state.disp_open_num[disp]) {
            ret = drv_disp_close(disp);
            if (ret != HI_SUCCESS) {
                hi_fatal_disp("call DISP_Close failed.\n");
            }

            g_disp_attach_count = 0;
        }

        disp_state->disp_open[disp] = HI_FALSE;
    }

    return ret;
}

hi_s32 disp_ext_attach_src(hi_drv_display master, hi_drv_display slave)
{
    hi_s32 ret = HI_SUCCESS;

    if ((master != HI_DRV_DISPLAY_1) || (slave != HI_DRV_DISPLAY_0)) {
        hi_fatal_disp("Attach parameters invalid.\n");
        return HI_ERR_DISP_INVALID_OPT;
    }

    /*
    ret = disp_attach(master, slave);
 */
    return ret;
}

hi_s32 disp_ext_detach_src(hi_drv_display master, hi_drv_display slave)
{
    hi_s32 ret = HI_SUCCESS;

    if ((master != HI_DRV_DISPLAY_1) || (slave != HI_DRV_DISPLAY_0)) {
        hi_fatal_disp("Attach parameters invalid.\n");
        return HI_ERR_DISP_INVALID_OPT;
    }

    // ret = disp_detach(master, slave);

    return ret;
}

hi_s32 hi_drv_disp_attach(hi_drv_display master, hi_drv_display slave)
{
    hi_s32 ret = HI_SUCCESS;
    disp_attach attach_cfg;
    DISP_CHECK_ID(master);
    DISP_CHECK_ID(slave);

    attach_cfg.master = master;
    attach_cfg.slave = slave;
    ret = hi_drv_disp_process(CMD_DISP_ATTACH, &attach_cfg);
    return ret;
}

hi_s32 hi_drv_disp_detach(hi_drv_display master, hi_drv_display slave)
{
    hi_s32 ret = HI_SUCCESS;
    disp_attach attach_cfg;
    DISP_CHECK_ID(master);
    DISP_CHECK_ID(slave);

    attach_cfg.master = master;
    attach_cfg.slave = slave;
    ret = hi_drv_disp_process(CMD_DISP_DETACH, &attach_cfg);
    return ret;
}

hi_s32 hi_drv_disp_set_format(hi_drv_display disp, hi_drv_disp_fmt format)
{
    hi_s32 ret = HI_SUCCESS;
    disp_format format_cfg;
    DISP_CHECK_ID(disp);
    if (((format >= HI_DRV_DISP_FMT_1080P_24_FP)
         && (format <= HI_DRV_DISP_FMT_720P_50_FP))
        || (format >= HI_DRV_DISP_FMT_BUTT)) {
        hi_err_disp("Invalid format.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    memset(&format_cfg, 0, sizeof(disp_format));
    format_cfg.disp = disp;
    format_cfg.format = format;
    format_cfg.stereo = HI_DRV_DISP_STEREO_NONE;
    ret = hi_drv_disp_process(CMD_DISP_SET_FORMAT, &format_cfg);
    return ret;
}

hi_s32 hi_drv_disp_get_format(hi_drv_display disp, hi_drv_disp_fmt *format)
{
    hi_s32 ret = HI_SUCCESS;
    disp_format format_cfg = { 0 };
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(format);

    memset(&format_cfg, 0, sizeof(disp_format));
    format_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_GET_FORMAT, &format_cfg);
    if (!ret) {
        *format = format_cfg.format;
    }
    return ret;
}

hi_s32 hi_drv_disp_set_custom_timing(hi_drv_display disp, hi_drv_disp_timing *timing)
{
    hi_s32 ret = HI_SUCCESS;
    disp_timing timing_cfg = { 0 };
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(timing);

    memset(&timing_cfg, 0, sizeof(disp_timing));
    timing_cfg.disp = disp;
    memcpy(&timing_cfg.timing_para, timing, sizeof(hi_drv_disp_timing));
    ret = hi_drv_disp_process(CMD_DISP_SET_TIMING, &timing_cfg);
    return ret;
}

hi_s32 hi_drv_disp_get_custom_timing(hi_drv_display disp, hi_drv_disp_timing *timing)
{
    hi_s32 ret = HI_SUCCESS;
    disp_timing timing_cfg = { 0 };
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(timing);
    memset(&timing_cfg, 0, sizeof(disp_timing));
    timing_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_GET_TIMING, &timing_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    memcpy(timing, &timing_cfg.timing_para, sizeof(disp_timing));
    return ret;
}

hi_s32 hi_drv_disp_add_intf(hi_drv_display disp, hi_drv_disp_intf *intf)
{
    hi_s32 ret;
    disp_set_intf drv_disp_intf = { 0 };

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(intf);
    drv_disp_intf.disp = disp;

    memcpy(&drv_disp_intf.intf, intf, sizeof(hi_drv_disp_intf));

    ret = hi_drv_disp_process(CMD_DISP_ADD_INTF, &drv_disp_intf);
    return ret;
}

hi_s32 hi_drv_disp_del_intf(hi_drv_display disp, hi_drv_disp_intf *intf)
{
    hi_s32 ret;
    disp_set_intf disp_intf = { 0 };

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(intf);
    disp_intf.disp = disp;

    memcpy(&disp_intf.intf, intf, sizeof(hi_drv_disp_intf));

    ret = hi_drv_disp_process(CMD_DISP_DEL_INTF, &disp_intf);
    return ret;
}

hi_s32 hi_drv_disp_open(hi_drv_display disp)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    ret = hi_drv_disp_process(CMD_DISP_OPEN, &disp);
    return ret;
}

hi_s32 hi_drv_disp_close(hi_drv_display disp)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    ret = hi_drv_disp_process(CMD_DISP_CLOSE, &disp);
    return ret;
}

hi_s32 hi_drv_disp_set_alpha(hi_drv_display disp, hi_u32 alpha)
{
    hi_s32 ret = HI_SUCCESS;
    disp_alpha alpha_cfg = { 0 };

    DISP_CHECK_ID(disp);
    alpha_cfg.alpha = alpha;
    alpha_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_SET_ALPHA, &alpha_cfg);
    return ret;
}

hi_s32 hi_drv_disp_get_alpha(hi_drv_display disp, hi_u32 *alpha)
{
    hi_s32 ret = HI_SUCCESS;
    disp_alpha alpha_cfg = { 0 };

    DISP_CHECK_ID(disp);
    if (alpha == NULL) {
        hi_err_disp(" null pointer  !\n");
        return HI_FAILURE;
    }
    alpha_cfg.disp = disp;

    ret = hi_drv_disp_process(CMD_DISP_GET_ALPHA, &alpha_cfg);

    *alpha = alpha_cfg.alpha;

    return ret;
}

hi_s32 hi_drv_disp_set_enable(hi_drv_display disp, hi_bool enable)
{
    hi_s32 ret = HI_SUCCESS;
    disp_enable enable_cfg = { 0 };

    DISP_CHECK_ID(disp);
    memset(&enable_cfg, 0, sizeof(disp_enable));
    enable_cfg.enable = enable;
    enable_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_SET_ENABLE, &enable_cfg);
    return ret;
}

hi_s32 hi_drv_disp_get_enable(hi_drv_display disp, hi_bool *enable)
{
    hi_s32 ret = HI_SUCCESS;
    disp_enable enable_cfg = { 0 };
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(enable);

    memset(&enable_cfg, 0, sizeof(disp_enable));
    enable_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_GET_ENABLE, &enable_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    *enable = enable_cfg.enable;
    return ret;
}

hi_s32 hi_drv_disp_set_right_eye_first(hi_drv_display disp, hi_bool enable)
{
    hi_s32 ret = HI_SUCCESS;
    disp_r_eye_first re_first = { 0 };

    DISP_CHECK_ID(disp);
    re_first.refirst = enable;
    re_first.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_SET_R_E_FIRST, &re_first);
    return ret;
}

hi_s32 hi_drv_disp_set_bgcolor(hi_drv_display disp, hi_drv_disp_color *bgcolor)
{
    hi_s32 ret = HI_SUCCESS;
    disp_bgc bgc_cfg = { 0 };

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(bgcolor);
    memset(&bgc_cfg, 0, sizeof(disp_bgc));
    bgc_cfg.bgcolor = *bgcolor;
    bgc_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_SET_BGC, &bgc_cfg);
    return ret;
}

hi_s32 hi_drv_disp_get_bgcolor(hi_drv_display disp, hi_drv_disp_color *bgcolor)
{
    hi_s32 ret = HI_SUCCESS;
    disp_bgc bgc_cfg = { 0 };
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(bgcolor);

    memset(&bgc_cfg, 0, sizeof(disp_bgc));
    bgc_cfg.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_GET_BGC, &bgc_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    *bgcolor = bgc_cfg.bgcolor;
    return ret;
}

hi_s32 hi_drv_disp_set_aspect_ratio(hi_drv_display disp, hi_u32 ratio_h, hi_u32 ratio_v)
{
    hi_s32 ret = HI_SUCCESS;
    disp_aspect_ratio disp_ratio = { 0 };

    DISP_CHECK_ID(disp);
    memset(&disp_ratio, 0, sizeof(disp_aspect_ratio));
    disp_ratio.disp = disp;
    disp_ratio.w = ratio_h;
    disp_ratio.h = ratio_v;
    ret = hi_drv_disp_process(CMD_DISP_SET_DEV_RATIO, &disp_ratio);
    return ret;
}

hi_s32 hi_drv_disp_get_aspect_ratio(hi_drv_display disp, hi_u32 *ratio_h, hi_u32 *ratio_v)
{
    hi_s32 ret = HI_SUCCESS;
    disp_aspect_ratio disp_ratio = { 0 };

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(ratio_h);
    DISP_CHECK_NULL_POINTER(ratio_v);

    memset(&disp_ratio, 0, sizeof(disp_aspect_ratio));
    disp_ratio.disp = disp;
    ret = hi_drv_disp_process(CMD_DISP_GET_DEV_RATIO, &disp_ratio);
    if (ret == HI_SUCCESS) {
        *ratio_h = disp_ratio.w;
        *ratio_v = disp_ratio.h;
    }
    return ret;
}

hi_s32 hi_drv_disp_set_layer_zorder(hi_drv_display disp, hi_drv_disp_layer layer, hi_drv_disp_zorder z_flag)
{
    hi_s32 ret = HI_SUCCESS;
    disp_zorder zorder_cfg = { 0 };

    if ((layer >= HI_DRV_DISP_LAYER_BUTT) || (z_flag >= HI_DRV_DISP_ZORDER_BUTT)) {
        hi_err_disp("Invalid layer or zorder flag.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    DISP_CHECK_ID(disp);
    memset(&zorder_cfg, 0, sizeof(disp_zorder));
    zorder_cfg.disp = disp;
    zorder_cfg.layer = layer;
    zorder_cfg.zflag = z_flag;
    ret = hi_drv_disp_process(CMD_DISP_SET_ZORDER, &zorder_cfg);
    return ret;
}

hi_s32 hi_drv_disp_get_layer_zorder(hi_drv_display disp, hi_drv_disp_layer layer, hi_u32 *zorder)
{
    hi_s32 ret = HI_SUCCESS;
    disp_zorder zorder_cfg = { 0 };
    if (layer >= HI_DRV_DISP_LAYER_BUTT) {
        hi_err_disp("Invalid layer or zorder flag.\n");
        return HI_ERR_DISP_INVALID_PARA;
    }
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(zorder);
    memset(&zorder_cfg, 0, sizeof(disp_zorder));
    zorder_cfg.disp = disp;
    zorder_cfg.layer = layer;
    ret = hi_drv_disp_process(CMD_DISP_SET_ZORDER, &zorder_cfg);
    *zorder = zorder_cfg.zflag;
    return ret;
}

hi_s32 hi_drv_disp_create_cast(hi_drv_display disp, hi_drv_disp_cast_cfg *cfg, hi_handle *phcast)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_create cast_create = { 0 };

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(cfg);
    DISP_CHECK_NULL_POINTER(phcast);
    cast_create.disp = disp;
    cast_create.hcast = *phcast;
    cast_create.cfg = *cfg;
    ret = hi_drv_disp_process(CMD_DISP_CREATE_CAST, &cast_create);
    return ret;
}

hi_s32 hi_drv_disp_destroy_cast(hi_handle hcast)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_destroy cast_destroy = { 0 };

    cast_destroy.hcast = hcast;
    ret = hi_drv_disp_process(CMD_DISP_DESTROY_CAST, &cast_destroy);
    return ret;
}

hi_s32 hi_drv_disp_set_cast_enable(hi_handle hcast, hi_bool enable)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_enable cast_enable = { 0 };
    memset(&cast_enable, 0, sizeof(disp_cast_enable));
    cast_enable.hcast = hcast;
    cast_enable.enable = enable;
    ret = hi_drv_disp_process(CMD_DISP_SET_CAST_ENABLE, &cast_enable);
    return ret;
}

hi_s32 hi_drv_disp_get_cast_enable(hi_handle hcast, hi_bool *enable)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_enable cast_enable = { 0 };

    DISP_CHECK_NULL_POINTER(enable);
    memset(&cast_enable, 0, sizeof(disp_cast_enable));
    cast_enable.hcast = hcast;
    ret = hi_drv_disp_process(CMD_DISP_GET_CAST_ENABLE, &cast_enable);
    if (!ret) {
        *enable = cast_enable.enable;
    }
    return ret;
}

static disp_cast_frame g_cast_frame = { 0 };

hi_s32 hi_drv_disp_acquire_cast_frame(hi_handle hcast, hi_drv_video_frame *cast_frame)
{
    hi_s32 ret = HI_SUCCESS;
    // disp_cast_frame stCastFrame = {0};

    DISP_CHECK_NULL_POINTER(cast_frame);

    memset(&g_cast_frame, 0, sizeof(disp_cast_frame));
    g_cast_frame.hcast = hcast;
    ret = hi_drv_disp_process(CMD_DISP_ACQUIRE_CAST_FRAME, &g_cast_frame);
    if (!ret) {
        *cast_frame = g_cast_frame.frame;
    }
    return ret;
}

hi_s32 hi_drv_disp_release_cast_frame(hi_handle hcast, hi_drv_video_frame *cast_frame)
{
    hi_s32 ret = HI_SUCCESS;
    // disp_cast_frame stCastFrame = {0};

    DISP_CHECK_NULL_POINTER(cast_frame);
    memset(&g_cast_frame, 0, sizeof(disp_cast_frame));
    g_cast_frame.hcast = hcast;
    g_cast_frame.frame = *cast_frame;

    ret = hi_drv_disp_process_intr(CMD_DISP_RELEASE_CAST_FRAME, &g_cast_frame);

    return ret;
}

hi_s32 hi_drv_disp_externl_attach(hi_handle hcast, hi_handle h_sink)
{
    hi_s32 ret = HI_SUCCESS;
    disp_ext_attach disp_attach_info = { 0 };

    disp_attach_info.hcast = hcast;
    disp_attach_info.hmutual = h_sink;
    disp_attach_info.type = EXT_ATTACH_TYPE_SINK;

    ret = hi_drv_disp_process(CMD_DISP_EXT_ATTACH, &disp_attach_info);

    return ret;
}

hi_s32 hi_drv_disp_set_cast_attr(hi_handle hcast,
                                 hi_u32 width,
                                 hi_u32 height,
                                 hi_u32 frm_rate)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_ext_attr disp_cast_attr = { 0 };
    memset((void *)&disp_cast_attr, 0, sizeof(disp_cast_ext_attr));
    disp_cast_attr.hcast = hcast;
    disp_cast_attr.cast_attr.width = width;
    disp_cast_attr.cast_attr.height = height;

    ret = hi_drv_disp_process(CMD_DISP_SET_CASTATTR, &disp_cast_attr);
    return ret;
}

hi_s32 hi_drv_disp_get_cast_attr(hi_handle hcast, hi_drv_disp_cast_attr *cast_attr)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_ext_attr disp_cast_attr = { 0 };
    DISP_CHECK_NULL_POINTER(cast_attr);
    memset((void *)&disp_cast_attr, 0, sizeof(disp_cast_ext_attr));

    disp_cast_attr.hcast = hcast;

    ret = hi_drv_disp_process(CMD_DISP_GET_CASTATTR, &disp_cast_attr);
    if (ret == HI_FAILURE) {
        return ret;
    }

    *cast_attr = disp_cast_attr.cast_attr;
    return ret;
}

hi_s32 hi_drv_disp_externl_detach(hi_handle hcast, hi_handle h_sink)
{
    hi_s32 ret = HI_SUCCESS;
    disp_ext_attach disp_attach_info;

    disp_attach_info.hcast = hcast;
    disp_attach_info.hmutual = h_sink;
    disp_attach_info.type = EXT_ATTACH_TYPE_SINK;

    ret = hi_drv_disp_process(CMD_DISP_EXT_DEATTACH, &disp_attach_info);

    return ret;
}

hi_s32 hi_drv_disp_get_slave(hi_drv_display disp, hi_drv_display *pen_slave)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(pen_slave);
    osal_sem_down(&g_disp_semaphore);

    // ret = disp_get_slave(disp, pen_slave);

    osal_sem_up(&g_disp_semaphore);

    return ret;
}

hi_s32 hi_drv_disp_get_master(hi_drv_display disp, hi_drv_display *pen_master)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(pen_master);
    osal_sem_down(&g_disp_semaphore);

    // ret = disp_get_master(disp, pen_master);

    osal_sem_up(&g_disp_semaphore);

    return ret;
}

hi_s32 hi_drv_disp_get_display_info(hi_drv_display disp, hi_disp_display_info *info)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(info);

    ret = drv_disp_get_display_info(disp, info);
    return ret;
}

hi_s32 hi_drv_disp_get_current_timing_status(hi_drv_display disp, hi_drv_disp_timing_status *current_timing_status)
{
    return drv_disp_get_current_timing_status(disp,current_timing_status);
}

hi_s32 hi_drv_disp_reg_callback(hi_drv_display disp, hi_drv_disp_callback_type type,
                                hi_drv_disp_callback *callback)
{
    hi_s32 ret = HI_SUCCESS;
    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(callback);
    if (type >= HI_DRV_DISP_C_TYPE_BUTT) {
        hi_err_disp("Invalid display callback type!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    osal_sem_down(&g_disp_semaphore);

    callback->callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    ret = disp_isr_reg_callback(disp, type, callback);
    osal_sem_up(&g_disp_semaphore);
    return ret;
}

hi_s32 hi_drv_disp_un_reg_callback(hi_drv_display disp, hi_drv_disp_callback_type type,
                                   hi_drv_disp_callback *callback)
{
    hi_s32 ret = HI_SUCCESS;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(callback);
    if (type >= HI_DRV_DISP_C_TYPE_BUTT) {
        hi_err_disp("Invalid display callback type!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }
    osal_sem_down(&g_disp_semaphore);

    callback->callback_prior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    ret = disp_isr_unreg_callback(disp, type, callback);
    osal_sem_up(&g_disp_semaphore);
    return ret;
}

typedef struct disp_processCmdFunc {
    unsigned int cmd;
    hi_s32 (*disp_process_cmd_func)(hi_void *arg, drv_disp_state *pDispState, hi_bool bUser);
} stdisp_processCmdFunc;

hi_s32 drv_disp_compat_process_cmd(unsigned int cmd, hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_FAILURE;

    if ((arg == HI_NULL) || (disp_state == HI_NULL)) {
        hi_fatal_disp("pass null ptr.\n");
        ret = HI_ERR_DISP_NULL_PTR;
        return ret;
    }

    switch (cmd) {
        case CMD_DISP_COMPAT_SEND_VBI: {
            disp_compat_vbi *disp_vbi;
            hi_drv_disp_vbi_data vbi_data;

            disp_vbi = (disp_compat_vbi *)arg;

            vbi_data.type = disp_vbi->vbi_data.type;
            vbi_data.data_addr = (hi_u8 *)((uintptr_t)disp_vbi->vbi_data.data_addr);
            vbi_data.data_len = disp_vbi->vbi_data.data_len;

            ret = drv_disp_send_vbi_data(disp_vbi->hvbi, &vbi_data);
            break;
        }
        default: {
            ret = drv_disp_process_cmd(cmd, arg, disp_state, user);
            break;
        }
    }

    return ret;
}

static hi_s32 disp_process_cmd_attach(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_attach *attach_cfg;

    attach_cfg = (disp_attach *)arg;

    ret = disp_ext_attach_src(attach_cfg->master, attach_cfg->slave);

    return ret;
}

static hi_s32 disp_process_cmd_detach(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_attach *attach_cfg;

    attach_cfg = (disp_attach *)arg;

    ret = disp_ext_detach_src(attach_cfg->master, attach_cfg->slave);

    return ret;
}

static hi_s32 disp_process_cmd_open(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;

    ret = disp_ext_open(*((hi_drv_display *)arg), disp_state, user);

    return ret;
}

static hi_s32 disp_process_cmd_close(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;

    ret = disp_check_para(*((hi_drv_display *)arg), disp_state);

    if (ret == HI_SUCCESS) {
        ret = disp_ext_close(*((hi_drv_display *)arg), disp_state, user);
    } else {
        ret = HI_SUCCESS;
    }

    return ret;
}

static hi_s32 disp_process_cmd_set_enable(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_enable *enable_cfg;

    enable_cfg = (disp_enable *)arg;

    ret = disp_check_para(enable_cfg->disp, disp_state);

    if (ret == HI_SUCCESS) {
        ret = drv_disp_set_enable(enable_cfg->disp,
                                  (enable_cfg->enable >= HI_TRUE) ? HI_TRUE : HI_FALSE);
    }

    return ret;
}

static hi_s32 disp_process_cmd_get_enable(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_enable *enable_cfg;

    enable_cfg = (disp_enable *)arg;

    ret = drv_disp_get_enable(enable_cfg->disp, &enable_cfg->enable);

    return ret;
}

hi_void update_mipi_mode_for_unf_attach(hi_drv_display disp, hi_drv_disp_intf *intf)
{
    if (intf->intf_type == HI_DRV_DISP_INTF_TYPE_MIPI) {
        disp_get_intf intf_state;
        drv_disp_intf_get_status(disp, &intf_state); /* alreay update by pdm mipi value */
        intf->un_intf.mipi.mipi_mode = intf_state.intf[HI_DRV_DISP_INTF_MIPI0].un_intf.mipi.mipi_mode;
    }
}

static hi_s32 disp_process_cmd_add_intf(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_set_intf *disp_intf;
    DISP_DEBUG_CALLER();

    disp_intf = (disp_set_intf *)arg;

    /* if set by unf ,update mipi mode by pdm already */
    update_mipi_mode_for_unf_attach(disp_intf->disp, &disp_intf->intf);

    ret = drv_disp_add_intf(disp_intf->disp, disp_intf->intf);
    hi_warn_disp("disp%d ,add ID-%d,ret -0x%x\n",
                 disp_intf->disp,
                 disp_intf->intf.intf_type,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_del_intf(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_set_intf *disp_intf;
    DISP_DEBUG_CALLER();

    disp_intf = (disp_set_intf *)arg;
    ret = drv_disp_del_intf(disp_intf->disp, disp_intf->intf);
    hi_warn_disp("disp%d ,add ID-%d,ret -0x%x\n",
                 disp_intf->disp,
                 disp_intf->intf.intf_type,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_intf_status(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_get_intf *disp_intf;
    DISP_DEBUG_CALLER();

    disp_intf = (disp_get_intf *)arg;
    ret = drv_disp_get_intf_status(disp_intf->disp, disp_intf);

    return ret;
}

static hi_s32 disp_process_cmd_set_intf_enable(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_set_intf_enable *disp_intf_enable;
    DISP_DEBUG_CALLER();

    disp_intf_enable = (disp_set_intf_enable *)arg;

    ret = drv_disp_set_intf_enable(disp_intf_enable->disp, disp_intf_enable->intf, disp_intf_enable->enable);
    hi_warn_disp("disp%d ,set intf ID-%d,enable(%d), ret -0x%x\n",
                 disp_intf_enable->disp,
                 disp_intf_enable->intf.intf_type,
                 disp_intf_enable->enable,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_intf_enable(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_set_intf_enable *disp_intf_enable;
    DISP_DEBUG_CALLER();

    disp_intf_enable = (disp_set_intf_enable *)arg;

    ret = drv_disp_get_intf_enable(disp_intf_enable->disp, disp_intf_enable->intf, &(disp_intf_enable->enable));
    hi_warn_disp("disp%d ,get intf ID-%d, enable(%d),ret -0x%x\n",
                 disp_intf_enable->disp,
                 disp_intf_enable->intf.intf_type,
                 disp_intf_enable->enable,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_set_format(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_format *format_cfg;
    DISP_DEBUG_CALLER();

    format_cfg = (disp_format *)arg;

    ret = drv_disp_set_format(format_cfg->disp, format_cfg->stereo, format_cfg->format);
    hi_warn_disp("disp%d ,stero-%d,format-%d-ret -0x%x\n", format_cfg->disp, format_cfg->stereo, format_cfg->format,
                 ret);
    /* use for set format in testcase */
    if ((g_drx_force_format_enable == HI_TRUE) && (ret != HI_SUCCESS)) {
        hi_warn_disp("dfx force set format %d\n", format_cfg->format);
        drv_disp_dfx_force_format(format_cfg->disp, format_cfg->stereo, format_cfg->format);
        ret = HI_SUCCESS;
    }
    return ret;
}

static hi_s32 disp_process_cmd_get_format(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_format *format_cfg;

    format_cfg = (disp_format *)arg;

    ret = drv_disp_get_format(format_cfg->disp, &format_cfg->stereo, &format_cfg->format);

    return ret;
}

static hi_s32 disp_process_cmd_set_refirst(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_r_eye_first *re_first;

    re_first = (disp_r_eye_first *)arg;

    ret = drv_disp_set_right_eye_first(re_first->disp,
                                       (re_first->refirst >= HI_TRUE) ? HI_TRUE : HI_FALSE);

    return ret;
}

static hi_s32 disp_process_cmd_set_virt_screen(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_vir_screen virt_screen;
    virt_screen = *((disp_vir_screen *)arg);

    ret = drv_disp_set_virt_screen(virt_screen.disp, &(virt_screen.vir_screen));

    return ret;
}

static hi_s32 disp_process_cmd_get_virt_screen(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_vir_screen *virt_screen;
    virt_screen = (disp_vir_screen *)arg;

    ret = drv_disp_get_virt_screen(virt_screen->disp, &virt_screen->vir_screen);

    return ret;
}

static hi_s32 disp_process_cmd_set_screen_offset(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_screen_offset *offset;
    offset = (disp_screen_offset *)arg;
    DISP_DEBUG_CALLER();

    ret = drv_disp_set_screen_offset(offset->disp, &offset->screen_offset);
    hi_warn_disp("disp%d ,l,r,t,b,(%d,%d,%d,%d)-ret -0x%x\n",
                 offset->disp,
                 offset->screen_offset.left,
                 offset->screen_offset.right,
                 offset->screen_offset.top,
                 offset->screen_offset.bottom,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_screen_offset(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_screen_offset *offset;
    offset = (disp_screen_offset *)arg;

    ret = drv_disp_get_screen_offset(offset->disp, &offset->screen_offset);

    return ret;
}

static hi_s32 disp_process_cmd_set_custom_timing(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_timing *chan_mode;

    chan_mode = (disp_set_timing *)arg;
    DISP_DEBUG_CALLER();

    ret = drv_disp_set_custom_timing(chan_mode->disp, &(chan_mode->format_param));

    return ret;
}


static hi_s32 disp_process_cmd_set_timing(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 number;
    disp_set_multi_channel_timing *chan_mode;

    chan_mode = (disp_set_multi_channel_timing *)arg;
    DISP_DEBUG_CALLER();

    for (number = 0; number < DISP_CHANNEL_MAX_NUMBER; number++) {
        if (chan_mode->disp_timing[number].format_param.number != 0) {
            ret = drv_disp_check_intf_validate(chan_mode->disp_timing[number].disp, &(chan_mode->disp_timing[number].format_param));
            if (ret != HI_SUCCESS) {
                hi_err_disp("disp(%d)check timing is error\n", chan_mode->disp_timing[number].disp);
                return HI_ERR_DISP_INVALID_PARA;
            }
        }
    }

    for (number = 0; number < DISP_CHANNEL_MAX_NUMBER; number++) {
        if (chan_mode->disp_timing[number].format_param.number != 0) {
            ret = drv_disp_set_timing(chan_mode->disp_timing[number].disp, &(chan_mode->disp_timing[number].format_param));
            if (ret != HI_SUCCESS) {
                hi_err_disp("disp(%d) set timing is error\n", chan_mode->disp_timing[number].disp);
                break;
            }
        }
    }

    return ret;
}

static hi_s32 disp_process_cmd_get_timing(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_timing *timing_cfg;

    timing_cfg = (disp_set_timing *)arg;
    ret = drv_disp_get_timing(timing_cfg->disp, &(timing_cfg->format_param));

    return ret;
}

static hi_s32 disp_process_cmd_set_zorder(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_zorder *zorder_cfg;

    zorder_cfg = (disp_zorder *)arg;

    return ret;
}

static hi_s32 disp_process_cmd_get_order(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_zorder *zorder_cfg;

    zorder_cfg = (disp_zorder *)arg;

    // ret = disp_get_layer_zorder(zorder_cfg->disp, zorder_cfg->layer, &zorder_cfg->zflag);

    return ret;
}

static hi_s32 disp_process_cmd_set_dev_ratio(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_aspect_ratio *aspect_ratio;
    DISP_DEBUG_CALLER();

    aspect_ratio = (disp_aspect_ratio *)arg;

    ret = drv_disp_set_aspect_ratio(aspect_ratio->disp, aspect_ratio->aspect_mode, aspect_ratio->w, aspect_ratio->h);
    hi_warn_disp("set asp disp%d ,H-(%d),V-(%d),-ret -0x%x, apsect mode-(%d)\n",
                 aspect_ratio->disp,
                 aspect_ratio->w,
                 aspect_ratio->h,
                 aspect_ratio->aspect_mode,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_dev_ratio(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_aspect_ratio *aspect_ratio;

    aspect_ratio = (disp_aspect_ratio *)arg;

    ret = drv_disp_get_aspect_ratio(aspect_ratio->disp,
                                    &aspect_ratio->aspect_mode,
                                    &aspect_ratio->w,
                                    &aspect_ratio->h);
    hi_warn_disp("get asp disp%d ,H-(%d),V-(%d),-ret -0x%x, apsect mode-(%d)\n",
                 aspect_ratio->disp,
                 aspect_ratio->w,
                 aspect_ratio->h,
                 aspect_ratio->aspect_mode,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_set_bgc(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_bgc *bgc_cfg;

    bgc_cfg = (disp_bgc *)arg;

    ret = drv_disp_set_bgcolor(bgc_cfg->disp, &bgc_cfg->bgcolor);

    return ret;
}

static hi_s32 disp_process_cmd_get_bgc(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_bgc *bgc_cfg;

    bgc_cfg = (disp_bgc *)arg;

    ret = drv_disp_get_bgcolor(bgc_cfg->disp, &bgc_cfg->bgcolor);

    return ret;
}

static hi_s32 disp_process_cmd_create_vbichannel(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_vbi_create_channel *vbi_crt_chanl;

    vbi_crt_chanl = (disp_vbi_create_channel *)arg;

    /* if vbi already open , return failed */
    if (disp_state->hvbi[HI_DRV_DISPLAY_1] == HI_TRUE) {
        return HI_ERR_DISP_INVALID_OPT;
    }

    ret = drv_disp_create_vbichannel(vbi_crt_chanl->disp, &vbi_crt_chanl->cfg, &vbi_crt_chanl->hvbi);

    if (ret == HI_SUCCESS) {
        disp_state->hvbi[HI_DRV_DISPLAY_1] = HI_TRUE;
    }

    return ret;
}

static hi_s32 disp_process_cmd_destroy_vbichannel(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    hi_handle *hvbi;

    hvbi = (hi_handle *)arg;
    if (disp_state->hvbi[HI_DRV_DISPLAY_1] == HI_FALSE) {
        return HI_ERR_DISP_INVALID_OPT;
    }

    ret = drv_disp_destroy_vbichannel(*hvbi);
    if (ret == HI_SUCCESS) {
        disp_state->hvbi[HI_DRV_DISPLAY_1] = HI_FALSE;
    }

    return ret;
}

static hi_s32 disp_process_cmd_send_vbi(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_vbi *vbi_cfg;

    vbi_cfg = (disp_vbi *)arg;

    ret = drv_disp_send_vbi_data(vbi_cfg->hvbi, &vbi_cfg->vbi_data);

    return ret;
}

static hi_s32 disp_process_cmd_set_wss(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_wss *wss_cfg;
    DISP_DEBUG_CALLER();

    wss_cfg = (disp_wss *)arg;

    ret = drv_disp_set_wss(wss_cfg->disp, &wss_cfg->wss_data);
    hi_warn_disp("disp%d set WSS,enable-(%d),data-(%d),-ret -0x%x\n",
                 wss_cfg->disp,
                 wss_cfg->wss_data.enable,
                 wss_cfg->wss_data.data,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_set_mcrvsn(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_mcrvsn *mcrvsn;
    DISP_DEBUG_CALLER();

    mcrvsn = (disp_mcrvsn *)arg;

    ret = drv_disp_set_macrovision(mcrvsn->disp, mcrvsn->mcrvsn);
    hi_warn_disp("disp%d ,set MCRVSN ,mode-(%d),-ret -0x%x\n",
                 mcrvsn->disp,
                 mcrvsn->mcrvsn,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_mcrvsn(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_mcrvsn *mcrvsn;

    mcrvsn = (disp_mcrvsn *)arg;

    ret = drv_disp_get_macrovision(mcrvsn->disp, &mcrvsn->mcrvsn);

    return ret;
}

static hi_s32 disp_process_cmd_set_cgms(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cgms *cgms_cfg;
    DISP_DEBUG_CALLER();

    cgms_cfg = (disp_cgms *)arg;

    ret = drv_disp_set_cgms(cgms_cfg->disp, &cgms_cfg->cgms_cfg);
    hi_warn_disp("disp%d ,set CGMS enable(%d) ,type-(%d),,mode-(%d),-ret -0x%x\n",
                 cgms_cfg->disp,
                 cgms_cfg->cgms_cfg.enable,
                 cgms_cfg->cgms_cfg.type,
                 cgms_cfg->cgms_cfg.mode,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_create_cast(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_create *c = (disp_cast_create *)arg;

    DISP_CHECK_ID(c->disp);
    if (disp_state->hcast_handle[c->disp] != HI_NULL) {
        return HI_ERR_DISP_INVALID_OPT;
    }

    ret = drv_disp_create_cast(c->disp, &c->cfg, &c->hcast);
    if (ret == HI_SUCCESS) {
        disp_state->hcast_handle[c->disp] = c->hcast;
    }

    return ret;
}

static hi_s32 disp_process_cmd_destroy_cast(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_destroy *c = (disp_cast_destroy *)arg;
    hi_s32 i;

    for (i = 0; i < HI_DRV_DISPLAY_BUTT; i++) {
        if (disp_state->hcast_handle[i] == c->hcast) {
            ret = drv_disp_destroy_cast(c->hcast);
            disp_state->hcast_handle[i] = HI_NULL;
        }
    }

    return ret;
}

static hi_s32 disp_process_cmd_set_cast_enable(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_enable *c = (disp_cast_enable *)arg;

    ret = drv_disp_set_cast_enable(c->hcast, c->enable);

    return ret;
}

static hi_s32 disp_process_cmd_get_cast_enable(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_enable *c = (disp_cast_enable *)arg;

    /* To do */
    ret = drv_disp_get_cast_enable(c->hcast, &c->enable);

    return ret;
}

static hi_s32 disp_process_cmd_acquire_cast_frame(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_frame *c = (disp_cast_frame *)arg;

    /* To do */
    ret = drv_disp_acquire_cast_frame(c->hcast, &c->frame);

    return ret;
}

static hi_s32 disp_process_cmd_release_cast_frame(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_frame *c = (disp_cast_frame *)arg;

    /* To do */
    ret = drv_disp_release_cast_frame(c->hcast, &c->frame);

    return ret;
}

static hi_s32 disp_process_cmd_acquire_snapshot(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_snapshot_frame *frame = (disp_snapshot_frame *)arg;
    hi_void *snapshot_handle_out = 0;

    DISP_CHECK_ID(frame->disp_layer);

    /* does not support continuous snapshot. */
    if (disp_state->h_snapshot[frame->disp_layer] != HI_NULL) {
        return HI_FAILURE;
    }

    ret = drv_disp_acquire_snapshot(frame->disp_layer,
                                    &frame->frame,
                                    &snapshot_handle_out);

    if (ret == HI_SUCCESS) {
        disp_state->h_snapshot[frame->disp_layer] = snapshot_handle_out;
    } else {
        disp_state->h_snapshot[frame->disp_layer] = HI_NULL;
    }

    return ret;
}

static hi_s32 disp_process_cmd_release_snapshot(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_snapshot_frame *frame = (disp_snapshot_frame *)arg;

    /* for released snapshot, just break. */
    DISP_CHECK_ID(frame->disp_layer);

    if (disp_state->h_snapshot[frame->disp_layer] == HI_NULL) {
        return HI_FAILURE;
    }

    ret = drv_disp_release_snapshot(frame->disp_layer, &frame->frame, disp_state->h_snapshot[frame->disp_layer]);
    disp_state->h_snapshot[frame->disp_layer] = 0;

    return ret;
}

static hi_s32 disp_process_cmd_ext_attach(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_ext_attach *c = (disp_ext_attach *)arg;

    /* To do */
    ret = drv_disp_external_attach(c->hcast, c->hmutual);

    return ret;
}

static hi_s32 disp_process_cmd_ext_deattach(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_ext_attach *c = (disp_ext_attach *)arg;

    ret = drv_disp_external_de_attach(c->hcast, c->hmutual);

    return ret;
}

static hi_s32 disp_process_cmd_set_cast_attr(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_ext_attr *c = (disp_cast_ext_attr *)arg;

    /* To do */
    ret = drv_disp_set_cast_attr(c->hcast, &c->cast_attr);

    return ret;
}

static hi_s32 disp_process_cmd_get_cast_attr(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_SUCCESS;
    disp_cast_ext_attr *c = (disp_cast_ext_attr *)arg;

    /* To do */
    ret = drv_disp_get_cast_attr(c->hcast, &c->cast_attr);

    return ret;
}

static hi_s32 disp_process_cmd_suspend(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    return drv_disp_suspend();
}

static hi_s32 disp_process_cmd_resume(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    return drv_disp_resume();
}

static hi_s32 disp_process_cmd_set_output_type(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_output *disp_output = (disp_set_output *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_output(disp_output->disp, disp_output->disp_type);
    hi_warn_disp("disp%d ,set Output Type(%d)-ret -0x%x\n",
                 disp_output->disp,
                 disp_output->disp_type,
                 ret);
    return ret;
}

static hi_s32 disp_process_cmd_get_output_type(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_output *disp_output = (disp_set_output *)arg;
    ret = drv_disp_get_output(disp_output->disp, &disp_output->disp_type);

    return ret;
}

static hi_s32 disp_process_cmd_set_output_color_space(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_colorspace *color_space_cfg = (disp_colorspace *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_output_color_space(color_space_cfg->disp, color_space_cfg->color_space);
    hi_warn_disp("disp%d ,set Color Space (%d)-ret -0x%x\n",
                 color_space_cfg->disp,
                 color_space_cfg->color_space,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_output_color_space(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_colorspace *color_space_cfg = (disp_colorspace *)arg;
    ret = drv_disp_get_output_color_space(color_space_cfg->disp, &color_space_cfg->color_space);

    return ret;
}

static hi_s32 disp_process_cmd_set_xdr_engine_type(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_xdr_engine *xdr_engine = (disp_xdr_engine *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_xdr_engine_type(xdr_engine->disp, xdr_engine->xdr_engine);
    hi_warn_disp("disp%d ,set XdrEngineType (%d)-ret -0x%x\n",
                 xdr_engine->disp,
                 xdr_engine->xdr_engine,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_xdr_engine_type(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_xdr_engine *xdr_engine = (disp_xdr_engine *)arg;
    ret = drv_disp_get_xdr_engine_type(xdr_engine->disp, &xdr_engine->xdr_engine);

    return ret;
}

static hi_s32 disp_process_cmd_set_alpha(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_alpha *alpha_cfg = (disp_alpha *)arg;
    DISP_DEBUG_CALLER();

    ret = drv_disp_set_alpha(alpha_cfg->disp, alpha_cfg->alpha);
    hi_warn_disp("disp%d ,set Alpha (%d)-ret -0x%x\n",
                 alpha_cfg->disp,
                 alpha_cfg->alpha,
                 ret);

    return ret;
}

static hi_s32 disp_process_cmd_get_alpha(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_alpha *alpha_cfg = (disp_alpha *)arg;

    ret = drv_disp_get_alpha(alpha_cfg->disp, &alpha_cfg->alpha);

    return ret;
}

static hi_s32 disp_process_cmd_set_sink_capability(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_sink_capability *disp_sink_cap = (disp_sink_capability *)arg;

    ret = drv_disp_set_sink_capability(disp_sink_cap->disp, &disp_sink_cap->sink_capability);
    return ret;
}

static hi_s32 disp_process_cmd_get_capability(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_get_capability *disp_get_cap = (disp_get_capability *)arg;

    ret = drv_disp_get_capability(disp_get_cap->disp, &disp_get_cap->disp_capability);
    return ret;
}


static hi_s32 disp_process_cmd_set_match_content_mode(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_match_mode *match_mode = (disp_match_mode *)arg;

    ret = drv_disp_set_match_content_mode(match_mode->disp, match_mode->match_content_mode);
    return ret;
}

static hi_s32 disp_process_cmd_get_match_content_mode(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_match_mode *match_mode = (disp_match_mode *)arg;

    ret = drv_disp_get_match_content_mode(match_mode->disp, &(match_mode->match_content_mode));
    return ret;
}

static hi_s32 disp_process_cmd_get_output_status(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_output_status *output_status = (disp_output_status *)arg;

    ret = drv_disp_get_output_status(output_status->disp, &output_status->output_status);
    return ret;
}

static hi_s32 disp_process_cmd_set_techni_bright(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_FAILURE;
    disp_techni_bright *techni_bright = HI_NULL;
    techni_bright = (disp_techni_bright *)arg;
    DISP_DEBUG_CALLER();

    // ret = drv_disp_set_techni_disp_bright(techni_bright->disp, techni_bright->techni_bright);
    hi_warn_disp("disp%d ,Techni Bright(%d)-ret -0x%x\n",
                 techni_bright->disp,
                 techni_bright->techni_bright,
                 ret);
    return ret;
}

static hi_s32 disp_process_cmd_get_techni_bright(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_FAILURE;

    return ret;
}

static hi_s32 disp_process_cmd_set_hdr_priority_mode(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_FAILURE;

    return ret;
}

static hi_s32 disp_process_cmd_get_hdr_priority_mode(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = HI_FAILURE;

    return ret;
}

static hi_s32 disp_process_cmd_get_fmt_from_vic(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_get_fmt_from_vic *fmt_from_vic;
    fmt_from_vic = (disp_get_fmt_from_vic *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_get_fmt_from_vic(fmt_from_vic->vic, &(fmt_from_vic->fmt));

    return ret;
}

static hi_s32 disp_process_cmd_set_vrr(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_vrr *drv_disp_vrr;
    drv_disp_vrr = (disp_set_vrr *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_vrr(drv_disp_vrr->disp , &(drv_disp_vrr->disp_vrr));

    return ret;
}

static hi_s32 disp_process_cmd_get_vrr(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_vrr *drv_disp_vrr;
    drv_disp_vrr = (disp_set_vrr *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_get_vrr(drv_disp_vrr->disp , &(drv_disp_vrr->disp_vrr));

    return ret;
}

static hi_s32 disp_process_cmd_set_qms(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_qms *drv_disp_qms;
    drv_disp_qms = (disp_set_qms *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_qms(drv_disp_qms->disp , drv_disp_qms->enable);

    return ret;
}

static hi_s32 disp_process_cmd_get_qms(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_qms *drv_disp_qms;
    drv_disp_qms = (disp_set_qms *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_get_qms(drv_disp_qms->disp , &(drv_disp_qms->enable));

    return ret;
}

static hi_s32 disp_process_cmd_set_allm(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_allm *drv_disp_allm;
    drv_disp_allm = (disp_set_allm *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_allm(drv_disp_allm->disp , drv_disp_allm->enable);

    return ret;
}


static hi_s32 disp_process_cmd_get_allm(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_allm *drv_disp_allm;
    drv_disp_allm = (disp_set_allm *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_get_allm(drv_disp_allm->disp , &(drv_disp_allm->enable));

    return ret;
}

static hi_s32 disp_process_cmd_set_qft(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_qft *drv_disp_qft;
    drv_disp_qft = (disp_set_qft *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_set_qft(drv_disp_qft->disp , drv_disp_qft->factor);

    return ret;
}

static hi_s32 disp_process_cmd_get_qft(hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret;
    disp_set_qft *drv_disp_qft;
    drv_disp_qft = (disp_set_qft *)arg;
    DISP_DEBUG_CALLER();
    ret = drv_disp_get_qft(drv_disp_qft->disp , &(drv_disp_qft->factor));

    return ret;
}

static const stdisp_processCmdFunc g_disp_cmd_func_tab[] = {
    { CMD_DISP_ATTACH,                 disp_process_cmd_attach },
    { CMD_DISP_DETACH,                 disp_process_cmd_detach },
    { CMD_DISP_OPEN,                   disp_process_cmd_open },
    { CMD_DISP_CLOSE,                  disp_process_cmd_close },
    { CMD_DISP_SET_ENABLE,             disp_process_cmd_set_enable },
    { CMD_DISP_GET_ENABLE,             disp_process_cmd_get_enable },
    { CMD_DISP_ADD_INTF,               disp_process_cmd_add_intf },
    { CMD_DISP_DEL_INTF,               disp_process_cmd_del_intf },
    { CMD_DISP_GET_INTF_STATUS,        disp_process_cmd_get_intf_status },
    { CMD_DISP_SET_FORMAT,             disp_process_cmd_set_format },
    { CMD_DISP_GET_FORMAT,             disp_process_cmd_get_format },
    { CMD_DISP_SET_R_E_FIRST,          disp_process_cmd_set_refirst },
    { CMD_DISP_SET_VIRTSCREEN,         disp_process_cmd_set_virt_screen },
    { CMD_DISP_GET_VIRTSCREEN,         disp_process_cmd_get_virt_screen },
    { CMD_DISP_SET_SCREENOFFSET,       disp_process_cmd_set_screen_offset },
    { CMD_DISP_GET_SCREENOFFSET,       disp_process_cmd_get_screen_offset },
    { CMD_DISP_SET_TIMING,             disp_process_cmd_set_timing },
    { CMD_DISP_GET_TIMING,             disp_process_cmd_get_timing },
    { CMD_DISP_SET_CUSTOM_TIMING,      disp_process_cmd_set_custom_timing },
    { CMD_DISP_SET_ZORDER,             disp_process_cmd_set_zorder },
    { CMD_DISP_GET_ORDER,              disp_process_cmd_get_order },
    { CMD_DISP_SET_DEV_RATIO,          disp_process_cmd_set_dev_ratio },
    { CMD_DISP_GET_DEV_RATIO,          disp_process_cmd_get_dev_ratio },
    { CMD_DISP_SET_BGC,                disp_process_cmd_set_bgc },
    { CMD_DISP_GET_BGC,                disp_process_cmd_get_bgc },
    { CMD_DISP_CREATE_VBI_CHANNEL,     disp_process_cmd_create_vbichannel },
    { CMD_DISP_DESTROY_VBI_CHANNEL,    disp_process_cmd_destroy_vbichannel },
    { CMD_DISP_SEND_VBI,               disp_process_cmd_send_vbi },
    { CMD_DISP_SET_WSS,                disp_process_cmd_set_wss },
    { CMD_DISP_SET_MCRVSN,             disp_process_cmd_set_mcrvsn },
    { CMD_DISP_GET_MCRVSN,             disp_process_cmd_get_mcrvsn },
    { CMD_DISP_SET_CGMS,               disp_process_cmd_set_cgms },
    { CMD_DISP_CREATE_CAST,            disp_process_cmd_create_cast },
    { CMD_DISP_DESTROY_CAST,           disp_process_cmd_destroy_cast },
    { CMD_DISP_SET_CAST_ENABLE,        disp_process_cmd_set_cast_enable },
    { CMD_DISP_GET_CAST_ENABLE,        disp_process_cmd_get_cast_enable },
    { CMD_DISP_ACQUIRE_CAST_FRAME,     disp_process_cmd_acquire_cast_frame },
    { CMD_DISP_RELEASE_CAST_FRAME,     disp_process_cmd_release_cast_frame },
    { CMD_DISP_ACQUIRE_SNAPSHOT,       disp_process_cmd_acquire_snapshot },
    { CMD_DISP_RELEASE_SNAPSHOT,       disp_process_cmd_release_snapshot },
    { CMD_DISP_EXT_ATTACH,             disp_process_cmd_ext_attach },
    { CMD_DISP_EXT_DEATTACH,           disp_process_cmd_ext_deattach },
    { CMD_DISP_SET_CASTATTR,           disp_process_cmd_set_cast_attr },
    { CMD_DISP_GET_CASTATTR,           disp_process_cmd_get_cast_attr },
    { CMD_DISP_SUSPEND,                disp_process_cmd_suspend },
    { CMD_DISP_RESUME,                 disp_process_cmd_resume },
    { CMD_DISP_SET_OUTPUT_TYPE,        disp_process_cmd_set_output_type },
    { CMD_DISP_GET_OUTPUT_TYPE,        disp_process_cmd_get_output_type },
    { CMD_DISP_SET_OUTPUT_COLOR_SPACE, disp_process_cmd_set_output_color_space },
    { CMD_DISP_GET_OUTPUT_COLOR_SPACE, disp_process_cmd_get_output_color_space },
    { CMD_DISP_SET_XDR_ENGINE,         disp_process_cmd_set_xdr_engine_type },
    { CMD_DISP_GET_XDR_ENGINE,         disp_process_cmd_get_xdr_engine_type },
    { CMD_DISP_SET_ALPHA,              disp_process_cmd_set_alpha },
    { CMD_DISP_GET_ALPHA,              disp_process_cmd_get_alpha },
    { CMD_DISP_SET_SINK_CAPABILITY,    disp_process_cmd_set_sink_capability },
    { CMD_DISP_GET_CAPABILITY,         disp_process_cmd_get_capability },
    { CMD_DISP_SET_MATCH_CONTENT_MODE, disp_process_cmd_set_match_content_mode },
    { CMD_DISP_GET_MATCH_CONTENT_MODE, disp_process_cmd_get_match_content_mode },
    { CMD_DISP_GET_OUTPUT_STATUS,      disp_process_cmd_get_output_status },
    { CMD_DISP_SET_TECHNI_BRIGHT,      disp_process_cmd_set_techni_bright },
    { CMD_DISP_GET_TECHNI_BRIGHT,      disp_process_cmd_get_techni_bright },
    { CMD_DISP_SET_HDR_PRIORITY,       disp_process_cmd_set_hdr_priority_mode },
    { CMD_DISP_GET_HDR_PRIORITY,       disp_process_cmd_get_hdr_priority_mode },
    { CMD_DISP_GET_FMT_FROM_VIC,       disp_process_cmd_get_fmt_from_vic },
    { CMD_DISP_SET_INTF_ENABLE,        disp_process_cmd_set_intf_enable},
    { CMD_DISP_GET_INTF_ENABLE,        disp_process_cmd_get_intf_enable },
    { CMD_DISP_SET_VRR,                disp_process_cmd_set_vrr },
    { CMD_DISP_GET_VRR,                disp_process_cmd_get_vrr },
    { CMD_DISP_SET_QMS,                disp_process_cmd_set_qms },
    { CMD_DISP_GET_QMS,                disp_process_cmd_get_qms },
    { CMD_DISP_SET_ALLM,               disp_process_cmd_set_allm },
    { CMD_DISP_GET_ALLM,               disp_process_cmd_get_allm },
    { CMD_DISP_SET_QFT,                disp_process_cmd_set_qft },
    { CMD_DISP_GET_QFT,                disp_process_cmd_get_qft },
};

hi_s32 drv_disp_process_cmd(unsigned int cmd, hi_void *arg, drv_disp_state *disp_state, hi_bool user)
{
    hi_s32 ret = -ENOIOCTLCMD;
    hi_u32 index = 0;

    if ((arg == HI_NULL) || (disp_state == HI_NULL)) {
        hi_fatal_disp("pass null ptr.\n");
        ret = HI_ERR_DISP_NULL_PTR;
        return ret;
    }

    for (index = 0; (index < sizeof(g_disp_cmd_func_tab) / sizeof(stdisp_processCmdFunc)); index++) {
        if ((g_disp_cmd_func_tab[index].cmd == cmd)
            && (g_disp_cmd_func_tab[index].disp_process_cmd_func != HI_NULL)) {
            ret = g_disp_cmd_func_tab[index].disp_process_cmd_func(arg, disp_state, user);
            break;
        }
    }

    return ret;
}

// may be delete
hi_s32 hi_drv_disp_init(hi_void)
{
    hi_s32 ret;

    osal_sem_down(&g_disp_semaphore);

    if (1 == osal_atomic_inc_return(&g_disp_atomic_cnt)) {
        /* for configuration such as start clock, re-use pins, etc */
        ret = drv_disp_init();
        if (ret != HI_SUCCESS) {
            hi_fatal_disp("call DISP_Init failed.\n");
            (hi_void)osal_atomic_dec_return(&g_disp_atomic_cnt);
            osal_sem_up(&g_disp_semaphore);
            return HI_FAILURE;
        }
    }

    osal_sem_up(&g_disp_semaphore);
    ret = drv_disp_register();
    return ret;
}

hi_s32 hi_drv_disp_de_init(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_display u;

    osal_sem_down(&g_disp_semaphore);

    for (u = 0; u < HI_DRV_DISPLAY_BUTT; u++) {
        if (g_disp_mod_state.disp_open[u]) {
            hi_info_disp("DISP_MOD_ExtClose HD0\n");
            ret = disp_ext_close(u, &g_disp_mod_state, HI_FALSE);
            if (ret != HI_SUCCESS) {
                hi_fatal_disp("DISP_MOD_ExtClose Display %d failed!\n", u);
            }
        }
    }

    hi_info_disp("HI_DRV_DISP_Deinit:atomic g_DispCount:%d\n", osal_atomic_read(&g_disp_atomic_cnt));

    if (osal_atomic_dec_return(&g_disp_atomic_cnt) == 0) {
        hi_info_disp("close clock\n");

        /* closing clock */
        drv_disp_de_init();
    }

    osal_sem_up(&g_disp_semaphore);
    return 0;
}

static disp_export_func g_disp_export_funcs = {

    .disp_init = hi_drv_disp_init,
    .disp_deinit = hi_drv_disp_de_init,
    .disp_open = hi_drv_disp_open,
    .disp_close = hi_drv_disp_close,
#if 0
    .pfndispattach           = hi_drv_disp_attach,
    .pfndispdetach           = hi_drv_disp_detach,
    .pfndispsetformat        = hi_drv_disp_set_format,
    .pfndispgetformat        = hi_drv_disp_get_format,
    .pfndispsetcustomtiming  = hi_drv_disp_set_custom_timing,
    .pfndispgetcustomtiming  = hi_drv_disp_get_custom_timing,
    .pfndispaddintf          = hi_drv_disp_add_intf,
    .pfndispdeintf           = hi_drv_disp_del_intf,


    .pfndispsetenable        = hi_drv_disp_set_enable,
    .pfndispgetenable        = hi_drv_disp_get_enable,
    .pfndispsetrighteyefirst = hi_drv_disp_set_right_eye_first,
    .pfndispsetbgcolor       = hi_drv_disp_set_bgcolor,
    .pfndispgetbgcolor       = hi_drv_disp_get_bgcolor,
    .pfndispsetaspectratio   = hi_drv_disp_set_aspect_ratio,
    .pfndispgetaspectratio   = hi_drv_disp_get_aspect_ratio,
    .pfndispsetlayerzorder   = hi_drv_disp_set_layer_zorder,
    .pfndispgetlayerzorder   = hi_drv_disp_get_layer_zorder,

    .pfndispcreatcast        = hi_drv_disp_create_cast,
    .pfndispdestorycast      = hi_drv_disp_destroy_cast,
    .pfndispsetcastenable    = hi_drv_disp_set_cast_enable,
    .pfndispgetcastenable    = hi_drv_disp_get_cast_enable,
    .pfndispacquirecastfrm   = hi_drv_disp_acquire_cast_frame,
    .pfndisprlscastfrm       = hi_drv_disp_release_cast_frame,

    .pfndispextattach        = hi_drv_disp_externl_attach,
    .pfndispextdeattach      = hi_drv_disp_externl_detach,
    .pfndispsetcastattr      = hi_drv_disp_set_cast_attr,
    .pfndispgetcastattr      = hi_drv_disp_get_cast_attr,
    .disp_suspend              = drv_disp_suspend,
    .disp_resume               = drv_disp_resume,

#endif

    .disp_get_displayinfo = hi_drv_disp_get_display_info,
    .disp_register_call_back = hi_drv_disp_reg_callback,
    .disp_unregister_call_back = hi_drv_disp_un_reg_callback,

};

hi_s32 drv_disp_start_up(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_init(&g_disp_semaphore, 1);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = osal_atomic_init(&g_disp_atomic_cnt);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    drv_disp_proc_init();
    drv_disp_proc_add(HI_DRV_DISPLAY_0);
    drv_disp_proc_add(HI_DRV_DISPLAY_1);

    if (drv_disp_init() == HI_SUCCESS) {
        (hi_void)drv_disp_intf_init();
        hi_drv_disp_open(HI_DRV_DISPLAY_0);
#ifndef VDP_SUPPORT_PANEL /* vbo only need open disp0 */
        hi_drv_disp_open(HI_DRV_DISPLAY_1);
#endif
    }
    return HI_SUCCESS;
}

hi_s32 drv_disp_shut_down(hi_void)
{
    drv_disp_proc_del(HI_DRV_DISPLAY_1);
    drv_disp_proc_del(HI_DRV_DISPLAY_0);
    drv_disp_proc_de_init();

    hi_drv_disp_close(HI_DRV_DISPLAY_0);
    hi_drv_disp_close(HI_DRV_DISPLAY_1);
    drv_disp_de_init();
    (hi_void)drv_disp_intf_deinit();

    osal_atomic_set(&g_disp_atomic_cnt, 0);
    if (g_disp_atomic_cnt.atomic != HI_NULL) {
        osal_atomic_destory(&g_disp_atomic_cnt);
    }

    if (g_disp_semaphore.sem != HI_NULL) {
        osal_sem_destory(&g_disp_semaphore);
    }
    return HI_SUCCESS;
}

hi_s32 drv_disp_register(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    // add for multiple process
    disp_reset_count_status();

    ret = hi_drv_module_register((hi_u32)HI_ID_DISP, "HI_DISP", (hi_void *)(&g_disp_export_funcs),
                                 HI_NULL);
    if (ret != HI_SUCCESS) {
        hi_fatal_disp("HI_DRV_MODULE_Register DISP failed\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_void drv_disp_un_register(hi_void)
{
    hi_drv_module_unregister((hi_u32)HI_ID_DISP);

    return;
}

hi_s32 drv_disp_resume(hi_void)
{
    return drv_display_resume();
}

hi_s32 drv_disp_suspend(hi_void)
{
    return drv_display_suspend();
}

#ifdef VDP_SDK_HAL_TEST_SUPPORT
EXPORT_SYMBOL(hi_drv_disp_set_format);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

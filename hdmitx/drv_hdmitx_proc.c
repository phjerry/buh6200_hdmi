/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver proc source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>
#include "hi_drv_proc.h"
#include "drv_hdmitx.h"
#include "drv_hdmitx_proc.h"
#include "drv_hdmitx_debug.h"
#include "drv_hdmitx_edid.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx_infoframe.h"
#include "drv_hdmitx_cec.h"
#include "drv_hdmitx_hdcp.h"
#include "hal_hdmitx_ctrl.h"
#include "hal_hdmitx_dsc.h"
#include "hal_hdmitx_frl.h"
#include "hi_osal.h"

#define PROC_FEILD_RATE_DEVIATION 50

#define STR_YES   "yes"
#define STR_NO    "no"
#define STR_NA    "n/a"
#define STR_ERROR "error"
#define STR_RUN   "run"
#define STR_STOP  "stop"
#define STR_ON    "on"
#define STR_OFF   "off"

#define HDCP_ERR_CODE_CATEGORY_NUM 3
#define EACH_CATEGORY_TYPE_NUM     0x10

struct proc_colorimetry {
    hi_u32 num;
    char *name;
};

const static hi_char *g_frl_event[] = {
    "TRAIN_DISABLE",
    "TRAIN_SUCCESS",
    "TRAIN_SINK_NO_SCDC",
    "TRAIN_READY_TIMEOUT",
    "TRAIN_TFLT_TIMEOUT",
    "TRAIN_LAST_RATE",
    "TRAIN_LTSP_TIMEOUT",
    "TRAIN_DDC_ERR",
    "UNKNOWN",
};

const static hi_char *g_frl_state[] = {
    "LTS_L",
    "LTS_1",
    "LTS_2",
    "LTS_3",
    "LTS_4",
    "LTS_P",
};

const static hi_char *g_3d_structure[] = {
    "FRAME_PACKING",
    "FIELD_ALTERNATIVE",
    "LINE_ALTERNATIVE",
    "SIDE_BY_SIDE_FULL",
    "L_DEPTH",
    "L_DEPTH_GFX_GFX_DEPTH",
    "TOP_AND_BOTTOM",
    "SIDE_BY_SIDE_HALF",
    "INVALID",
};

const static hi_char *g_audio_input_type[] = {
    "unknown",
    "i2s",
    "spdif",
    "hbr",
    "unknown",
};

const static hi_char *g_audio_rate[] = {
    "8K",
    "11K",
    "12K",
    "16K",
    "22K",
    "24K",
    "32K",
    "44K",
    "48K",
    "88K",
    "96K",
    "176K",
    "192K",
    "768K",
    "unknown",
};

const static hi_char *g_audio_chn_num[] = {
    "unknown",
    "unknown",
    "2CH",
    "3CH",
    "4CH",
    "5CH",
    "6CH",
    "7CH",
    "8CH",
    "unknown",
};

const static hi_char *g_color_depth[] = {
    "24bit",
    "30bit",
    "36bit",
    "error",
};

const static hi_char *g_color_format[] = {
    "rgb444",
    "ycbcr422",
    "ycbcr444",
    "ycbcr420",
    "error",
};

struct proc_colorimetry g_colorimetry[] = {
    { 0x00, "no_data" },
    { 0x01, "itu601" },
    { 0x02, "itu709" },
    { 0x03, "xvyCC601" },
    { 0x13, "xvyCC709" },
    { 0x23, "sycc601" },
    { 0x33, "ad_ycc_601" },
    { 0x43, "ad_rgb" },
    { 0x53, "2020_con_s" },
    { 0x63, "2020_non_c" },
    { 0x73, "extension" },
    { 0x83, "error" },
};

const static hi_char *g_3d_mode[] = {
    "frame_pack",
    "field_alter",
    "line_alter",
    "sbs(full)",
    "l+depth",
    "l+depth+gd",
    "top&bottom",
    "reserved",
    "sbs(half)",
    "none(2d)",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "not_in_use",
    "error",
};

const static hi_char *g_pic_aspect_ratio[] = {
    "no_data",
    "4:3",
    "16:9",
    "64:27",
    "256:135",
    "error",
};

const static hi_char *g_act_aspect_ratio[] = {
    "reserved",
    "reserved",
    "box_16:9",
    "box_14:9",
    "box_16:9_ctr",
    "reserved",
    "reserved",
    "reserved",
    "same_pic",
    "ctr_4:3",
    "ctr_16:9",
    "ctr_14:9",
    "reserved",
    "4:3_ctr_14:9",
    "16:9_ctr_14:9",
    "16:9_ctr_4:3",
    "error",
};

const static hi_char *g_pixel_repeat[] = {
    "no",
    "2_times",
    "3_times",
    "4_times",
    "5_times",
    "6_times",
    "7_times",
    "8_times",
    "9_times",
    "10_times",
    "error",
};

const static hi_char *g_rgb_quantizeion[] = {
    "default",
    "limited",
    "full",
    "reserved",
    "error",
};

const static hi_char *g_ycc_quantizeion[] = {
    "limited",
    "full",
    "reserved",
    "reserved",
    "error",
};

const static hi_char *g_scan_info[] = {
    "no_data",
    "overs",
    "under",
    "reserved",
    "error",
};

const static hi_char *g_pic_scaling[] = {
    "unknown",
    "horiz",
    "vert",
    "horiz&vert",
    "error",
};

const static hi_char *g_bar_data_present[] = {
    "no_prst",
    "vert. Bar",
    "horiz. Bar",
    "vert&horiz",
    "error",
};

/* EIA-CEA-861-F Table 14 */
const static hi_char *g_it_content_type[] = {
    "graphics",
    "photo",
    "cinema",
    "game",
    "error",
};

const static hi_char *g_hpd_status_str[] = {
    "detecting",
    "in", /* HPD_PLUGIN 1 */
    "out", /* HPD_PLUGOUT 2 */
    "det_fail", /* HPD_DET_FAIL 3 */
    "error",
};

const static hi_char *g_audio_sample_rate_str[] = {
    "stream",
    "32k",
    "44.1k",
    "48k",
    "88.2k",
    "96k",
    "176.4k",
    "192k",
    "error",
};

const static hi_char *g_audio_sample_bit_str[] = {
    "stream",
    "16bit",
    "20bit",
    "24bit",
    "error",
};

/* EIA-CEA-861-F Table 29 */
const static hi_char *g_audio_ext_code_type_str[] = {
    "refer",
    "not_in_use",
    "not_in_use",
    "not_in_use",
    "4_he_aac", /* MPEG4 */
    "4_he_aacv2",
    "4_aac_lc",
    "dra",
    "4_he_aac+",
    "mpeg_h_3d",
    "ac-4",
    "l_pcm_3d",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
};

const static hi_char *g_sound_intf_Str[] = {
    "error",
    "i2s",
    "spdif",
    "hbra",
    "error",
};

/* EIA-CEA-861-F Table 27 */
const static hi_char *g_audio_code_type_str[] = {
    "stream",
    "l_pcm",
    "ac_3",
    "mpeg_1",
    "mp3",
    "mpeg2",
    "aac_lc",
    "dts",
    "atrac",
    "one_bit_ao",
    "eac_3",
    "dts_hd",
    "mat",
    "dst",
    "wma_pro",
    "error",
};

/* EIA-CEA-861-F Table 27 */
const static hi_char *g_audio_channel_str[] = {
    "error",
    "error",
    "2_ch",
    "3_ch",
    "4_ch",
    "5_ch",
    "6_ch",
    "7_ch",
    "8_ch",
    "error",
};

const static hi_char *g_infoframe_type_str[] = {
    "vendor",
    "avi",
    "spd",
    "audio",
    "drm",
    "error",
};

const static hi_char *g_hdmi_id_str[] = {
    "HDMI0",
    "HDMI1",
    "UNKNOWN",
};

static const hi_char *g_logic_addr_status_name[] = {
    "valid",
    "invalid",
    "not config",
    "configuring",
};

static const hi_char *g_device_type_name[] = {
    "tv",
    "record",
    "reserve",
    "tuner",
    "playback",
    "audio system",
    "switch",
    "processor"
};

static const hi_char *g_hdcp_mode[] = {
    "auto",
    "1.X",
    "2.2",
    "2.3",
    "unknown",
};

static const hi_char *g_hdcp_ver[] = {
    "ver_none",
    "ver_hdcp1X",
    "ver_hdcp22",
    "ver_hdcp23",
    "unknown",
};

static const hi_char *g_hdcp_err_code[HDCP_ERR_CODE_CATEGORY_NUM][EACH_CATEGORY_TYPE_NUM] = {
    {
        "err_undo",
        "err_none",
        "err_plug_out",
        "err_no_sig",
        "err_no_key",
        "err_invalid_key",
        "err_ddc",
        "err_on_srm",
        "err_no_cap",
    }, {
        "1x_no_cap",
        "1x_bcap_fail",
        "1x_bskv_fail"
        "1x_integrity_fail_r0",
        "1x_wdt_timeout",
        "1x_vi_check_fail",
        "1x_exceed_topology",
        "1x_integrity_fail_ri",
    }, {
        "2x_no_cap",
        "2x_signature_fail",
        "2x_mismatch_h",
        "2x_ake_timeout",
        "2x_locality_fail",
        "2x_reauth_reQ",
        "2x_wdt_timeout",
        "2x_v_mismatch",
        "2x_roll_over",
        "2x_exceed_topology",
        "unknown",
    }
};

const static hi_char *g_proc_name_str[] = {
    "soft_status --", /* hdmitx */
    "hw_status ----",
    "hw_cfg -------",
    "detail_timing ",
    "frl_info -----",
    "cec_info -----",
    "hdcp_info ----",
    "video_attr ---", /* hdmitx_aovo */
    "video_avi ----",
    "video_path ---",
    "video_vsif ---",
    "audio_attr ---",
    "audio_path ---",
    "audio_infofrm-",
    "dsc_cfg ------", /* hdmitx_dsc */
    "para_input ---",
    "dsc_timing ---",
    "dsc_caps -----",
    "dsc_status ---",
    "em_data_pack -",
    "pps_raw_data -",
    "edid_raw_data ", /* hdmitx_sink */
    "display_info -",
    "vrr_info -----",
    "hdr_info -----",
    "dolby_info ---",
    "dsc_info -----",
    "scdc_info ----",
    "latency_info -",
    "audio_info ---",
    "base_info ----",
    "color_info ---",
    "timing_info --",
    "unknown ------",
};

enum proc_info_name {
    SOFT_STATUS, /* hdmitx */
    HW_STATUS,
    HW_CFG,
    DETAIL_TIMING,
    FRL_INFO,
    CEC_INFO,
    HDCP_INFO,
    VIDEO_ATTR, /* hdmitx_aovo */
    VIDEO_AVI,
    VIDEO_PATH,
    VIDEO_VSIF,
    AUDIO_ATTR,
    AUDIO_PATH,
    AUDIO_INFOFRAME,
    DSC_CFG, /* hdmitx_dsc */
    PARA_INPUT,
    DSC_TIMING,
    DSC_CAPS,
    DSC_STATUS,
    EM_DATA_PACK,
    PPS_RAW_DATA,
    EDID_RAW_DATA, /* hdmitx_sink */
    DISPLAY_INFO,
    VRR_INFO,
    HDR_INFO,
    DOLBY_INFO,
    DSC_INFO,
    SCDC_INFO,
    LATENCY_INFO,
    AUDIO_INFO,
    BASE_INFO,
    COLOR_INFO,
    TIMING_INFO,
};

enum print_part {
    MIN,
    OFFSET,
    MAX,
};

static hi_u32 proc_check_max(hi_u32 in, hi_u32 max)
{
    hi_u32 out;

    if (!max) {
        HDMI_ERR("array max size error!\n");
        return 0;
    }

    if (in < 0) {
        in = max - 1;
    }

    out = ((in) >= ((max) - 1)) ? ((max) - 1) : (in);

    return out;
}

static hi_u32 proc_colorimetry2num(hi_u32 colorimetry)
{
    hi_u32 i;

    for (i = 0; i < ARRAY_SIZE(g_colorimetry); i++) {
        if (g_colorimetry[i].num == colorimetry) {
            return i;
        }
    }
    return i;
}

static void proc_print_name(struct seq_file *file, struct hisilicon_hdmi *hdmi, enum proc_info_name name)
{
    hi_u32 temp;

    temp = proc_check_max(name, ARRAY_SIZE(g_proc_name_str));
    HI_PROC_PRINT(file, "-------------------------------");
    HI_PROC_PRINT(file, "----------------- %-14s---------------------------", g_proc_name_str[temp]);
    HI_PROC_PRINT(file, "-------------------------------\n");
}

static hi_void proc_sink_timing_ycc420(struct seq_file *file, struct hdmi_connector *connector)
{
    hi_s32 ret;
    hi_u32 i = 0;
    struct osal_list_head *n = NULL;
    struct osal_list_head *p = NULL;
    struct hdmi_display_mode *display_mode = NULL;
    hi_char timing_info[25]; /* timing format buff size 25 */

    HI_PROC_PRINT(file, "\n%s:\n", "YCbCr420[Only]");
    osal_list_for_each_safe(p, n, &connector->probed_modes) {
        display_mode = osal_list_entry(p, struct hdmi_display_mode, head);
        if (display_mode->parse_type & MODE_TYPE_Y420VDB) {
            if (memset_s(timing_info, sizeof(timing_info), 0, sizeof(timing_info))) {
                HDMI_ERR("memset_s err\n");
                return;
            }
            ret = snprintf_s(timing_info, sizeof(timing_info), sizeof(timing_info) - 1, "%uX%u%s_%u",
                             display_mode->detail.h_active,
                             display_mode->detail.v_active,
                             display_mode->detail.progressive ? "p" : "i",
                             display_mode->detail.field_rate);
            if (ret < 0) {
                HDMI_ERR("snprintf_s err\n");
                return;
            }

            HI_PROC_PRINT(file, "[%03u][%s]%-15s ", display_mode->vic,
                          display_mode->native_mode ? "y" : "n", timing_info);

            i++;
            if ((i % 4) == 0) { /* 4 per each wrap */
                HI_PROC_PRINT(file, "\n");
            }
        }
    }
    i = 0;
    HI_PROC_PRINT(file, "\n%s:\n", "YCbCr420[Also]");
    osal_list_for_each_safe(p, n, &connector->probed_modes) {
        display_mode = osal_list_entry(p, struct hdmi_display_mode, head);
        if (display_mode->parse_type & MODE_TYPE_VDB_Y420CMDB) {
            if (memset_s(timing_info, sizeof(timing_info), 0, sizeof(timing_info))) {
                HDMI_ERR("memset_s err\n");
                return;
            }
            ret = snprintf_s(timing_info, sizeof(timing_info), sizeof(timing_info) - 1, "%uX%u%s_%u",
                             display_mode->detail.h_active,
                             display_mode->detail.v_active,
                             display_mode->detail.progressive ? "p" : "i",
                             display_mode->detail.field_rate);
            if (ret < 0) {
                HDMI_ERR("snprintf_s err\n");
                return;
            }

            HI_PROC_PRINT(file, "[%03u][%s]%-15s ", display_mode->vic,
                          display_mode->native_mode ? "y" : "n", timing_info);

            i++;
            if ((i % 4) == 0) { /* 4 per each wrap */
                HI_PROC_PRINT(file, "\n");
            }
        }
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_void proc_sink_timing_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_u32 i = 0;
    hi_char timing_info[25]; /* timing format buff size 25 */
    struct hdmi_connector *connector = hdmi->connector;
    struct osal_list_head *n = NULL;
    struct osal_list_head *p = NULL;
    struct hdmi_display_mode *display_mode = NULL;

    proc_print_name(file, hdmi, TIMING_INFO);

    if (osal_list_empty(&connector->probed_modes)) {
        return;
    }

    osal_list_for_each_safe(p, n, &connector->probed_modes) {
        display_mode = osal_list_entry(p, struct hdmi_display_mode, head);
        if (memset_s(timing_info, sizeof(timing_info), 0, sizeof(timing_info))) {
            HDMI_ERR("memset_s err\n");
            return;
        }
        ret = snprintf_s(timing_info, sizeof(timing_info), sizeof(timing_info) - 1, "%uX%u%s_%u",
                         display_mode->detail.h_active,
                         display_mode->detail.v_active,
                         display_mode->detail.progressive ? "p" : "i",
                         display_mode->detail.field_rate);
        if (ret < 0) {
            HDMI_ERR("snprintf_s err\n");
            return;
        }

        HI_PROC_PRINT(file, "[%03u][%s]%-15s ", display_mode->vic, display_mode->native_mode ? "y" : "n", timing_info);
        i++;
        if ((i % 4) == 0) { /* 4 per each wrap */
            HI_PROC_PRINT(file, "\n");
        }
    }

    proc_sink_timing_ycc420(file, connector);
}

static hi_void proc_sink_color_colorimetry(struct seq_file *file, struct colorimetry colorimetry_info)
{
    /* line 5 */
    HI_PROC_PRINT(file, "%-17s:", "colorimetry");
    if (colorimetry_info.xvycc601 == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "xv_ycc601");
    }
    if (colorimetry_info.xvycc709 == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "xv_ycc709");
    }
    if (colorimetry_info.sycc601 == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "s_ycc601");
    }
    if (colorimetry_info.adobe_ycc601 == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "adoble_ycc601");
    }
    if (colorimetry_info.adobe_rgb == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "adoble_rgb");
    }
    if (colorimetry_info.bt2020_cycc == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "bt2020c_ycc");
    }
    if (colorimetry_info.bt2020_ycc == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "bt2020_ycc");
    }
    if (colorimetry_info.bt2020_rgb == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "bt2020_rgb");
    }
    if (colorimetry_info.dci_p3 == HI_TRUE) {
        HI_PROC_PRINT(file, " %-s", "dpi-p3");
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_void proc_sink_color_dpt(struct seq_file *file, struct color_depth color_dpt)
{
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s:", "rgb_color_dpt");
    if (color_dpt.rgb_30) {
        HI_PROC_PRINT(file, " %-s", "30");
    }
    if (color_dpt.rgb_36) {
        HI_PROC_PRINT(file, " %-s", "36");
    }
    if (color_dpt.rgb_48) {
        HI_PROC_PRINT(file, " %-s", "48");
    }
    HI_PROC_PRINT(file, "\n");

    /* line 3 */
    HI_PROC_PRINT(file, "%-17s:", "ycc444_color_dpt");
    if (color_dpt.y444_30) {
        HI_PROC_PRINT(file, " %-s", "ycc444_30");
    }
    if (color_dpt.y444_36) {
        HI_PROC_PRINT(file, " %-s", "ycc444_36");
    }
    if (color_dpt.y444_48) {
        HI_PROC_PRINT(file, " %-s", "ycc444_48");
    }
    HI_PROC_PRINT(file, "\n");

    /* line 4 */
    HI_PROC_PRINT(file, "%-17s:", "ycc420_color_dpt");
    if (color_dpt.y420_30) {
        HI_PROC_PRINT(file, " %-s", "30");
    }
    if (color_dpt.y420_36) {
        HI_PROC_PRINT(file, " %-s", "36");
    }
    if (color_dpt.y420_48) {
        HI_PROC_PRINT(file, " %-s", "48");
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_void proc_sink_color_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct color_property *color = &connector->color;
    hdmi_avail_mode tmp_mode;
    struct hdmi_timing_mode *timing = NULL;

    proc_print_name(file, hdmi, COLOR_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s:", "color_fmt");
    if (color->format.rgb) {
        HI_PROC_PRINT(file, " %-s", "rgb444");
    }
    if (color->format.ycbcr444) {
        HI_PROC_PRINT(file, " %-s", "ycc444");
    }
    if (color->format.ycbcr422) {
        HI_PROC_PRINT(file, " %-s", "ycc422");
    }
    if (color->format.ycbcr420) {
        HI_PROC_PRINT(file, " %-s", "ycc420");
    }
    HI_PROC_PRINT(file, "\n");

    proc_sink_color_dpt(file, color->depth);
    proc_sink_color_colorimetry(file, color->colorimetry);

    /* line 6 */
    HI_PROC_PRINT(file, "%-17s: %-2u bit     |", "max_bpc", color->depth.bpc);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "rgb_quan_select", color->quantization.rgb_qs_selecable ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "ycc_quan_sel", color->quantization.ycc_qy_selecable ? STR_YES : STR_NO);
    hdmi_connector_get_native_mode(connector, &tmp_mode);
    timing = vic2timing_mode(tmp_mode.vic);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "native_fmt", timing ? timing->name : "error");
    hdmi_connector_get_max_mode(connector, &tmp_mode);
    timing = vic2timing_mode(tmp_mode.vic);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "max_fmt", timing ? timing->name : "error");
}

static hi_void proc_sink_base_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_char mfc_name_buf[25]; /* buff size 25 */
    struct hdmi_connector *connector = hdmi->connector;
    struct base_property *base = &connector->base;

    if (memset_s(mfc_name_buf, sizeof(mfc_name_buf), '\0', sizeof(mfc_name_buf))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    ret = strncpy_s(mfc_name_buf, sizeof(mfc_name_buf), base->vendor.mfc_name, 4); /* mfc name buff size 4 */
    if (ret < 0) {
        HDMI_ERR("strncpy_s err\n");
        return ;
    }
    proc_print_name(file, hdmi, BASE_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %d.%-8d |", "ver", base->version, base->revision);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "manufc_name", mfc_name_buf);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "red_x", base->chromaticity.red_x);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "green_x", base->chromaticity.green_x);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "edid_src", connector->edid_src_type);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "pd_code", base->vendor.product_code);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "red_y", base->chromaticity.red_y);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "green_y", base->chromaticity.green_y);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "manufc_year", base->vendor.mfc_year);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "serial_number", base->vendor.serial_num);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "blue_x", base->chromaticity.blue_x);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "white_x", base->chromaticity.white_x);
    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "manufc_week", base->vendor.mfc_week);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "ext_block_num", base->ext_block_num);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "blue_y", base->chromaticity.blue_y);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "white_y", base->chromaticity.white_y);
}

static hi_void proc_sink_audio_dpt(struct seq_file *file, struct sad_fmt_audio *sad)
{
    HI_PROC_PRINT(file, "%-17s:", "bit_depth");
    if (sad->width_16 == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "16bit");
    }
    if (sad->width_20 == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "20bit");
    }
    if (sad->width_24 == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "24bit");
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_void proc_sink_audio_rate(struct seq_file *file, struct sad_fmt_audio *sad)
{
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s:", "rate(Hz)");
    if (sad->samp_32k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "32k");
    }
    if (sad->samp_44p1k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "44.1k");
    }
    if (sad->samp_48k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "48k");
    }
    if (sad->samp_88p2k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "88.2k");
    }
    if (sad->samp_96k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "96k");
    }
    if (sad->samp_176p4k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "176.4k");
    }
    if (sad->samp_192k == HI_TRUE) {
        HI_PROC_PRINT(file, " %s", "192k");
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_void proc_sink_audio_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 i, index;
    struct hdmi_connector *connector = hdmi->connector;
    struct audio_property *audio = &connector->audio;

    proc_print_name(file, hdmi, AUDIO_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "bas_sup", audio->basic ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "bas_cnt", audio->sad_count);

    for (i = 0; i < audio->sad_count; i++) {
        /* line 2 */
        HI_PROC_PRINT(file, "%-17s: No.%-d       |", "sad_num", i);
        index = proc_check_max(audio->sad[i].fmt_code, ARRAY_SIZE(g_audio_code_type_str));
        HI_PROC_PRINT(file, "%-17s: %-10s |", "code_type", g_audio_code_type_str[index]);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "max_chn_num", audio->sad[i].max_channel);

        index = proc_check_max(audio->sad[i].ext_code, ARRAY_SIZE(g_audio_ext_code_type_str));
        HI_PROC_PRINT(file, "%-17s:%-10s\n", "ext_code_type", g_audio_ext_code_type_str[index]);

        proc_sink_audio_rate(file, &audio->sad[i]);

        /* line 4 */
        if (audio->sad[i].fmt_code <= 1) {
            proc_sink_audio_dpt(file, &audio->sad[i]);
        } else if (audio->sad[i].fmt_code <= 8) { /* Audio Format Codes 8 */
            HI_PROC_PRINT(file, "%-17s: %-10u\n", "max_bit_rate(khz)", audio->sad[i].max_bit_rate);
        } else if (audio->sad[i].fmt_code <= 13) { /* Audio Format Codes 13 */
            if (audio->sad[i].fmt_code == 10) /* Audio Format Codes 10 */
                HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_atmos",
                              (audio->sad[i].dependent & 0x1) ? STR_YES : STR_NO);
            HI_PROC_PRINT(file, "%-17s: %-10d\n", "depend", audio->sad[i].dependent);
        } else if (audio->sad[i].fmt_code == 14) { /* Audio Format Codes 14 */
            HI_PROC_PRINT(file, "%-17s: %-10d\n", "profile", audio->sad[i].profile);
        } else if (audio->sad[i].fmt_code == 15) { /* Audio Format Codes 15 */
            if (audio->sad[i].ext_code >= 4 &&     /* Audio Coding Extension Type Codes 4 */
                audio->sad[i].ext_code <= 6) {     /* Audio Coding Extension Type Codes 6 */
                HI_PROC_PRINT(file, "%-17s: %-10s ", "960_tl", audio->sad[i].len_960_tl ? STR_YES : STR_NO);
                HI_PROC_PRINT(file, "%-17s: %-10s\n", "1024_tl", audio->sad[i].len_1024_tl ? STR_YES : STR_NO);
            } else if (audio->sad[i].ext_code >= 8 &&  /* Audio Coding Extension Type Codes 8 */
                       audio->sad[i].ext_code <= 10) { /* Audio Coding Extension Type Codes 10 */
                HI_PROC_PRINT(file, "%-17s: %-10s\n", "mps_l", audio->sad[i].mps_l ? STR_YES : STR_NO);
                HI_PROC_PRINT(file, "%-17s: %-10s ", "960_tl", audio->sad[i].len_960_tl ? STR_YES : STR_NO);
                HI_PROC_PRINT(file, "%-17s: %-10s\n", "1024_tl", audio->sad[i].len_1024_tl ? STR_YES : STR_NO);
            }
        }
    }
}

static hi_void proc_sink_latency_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct latency_property *latency = &connector->latency;

    proc_print_name(file, hdmi, LATENCY_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "p_video(ms)", latency->p_video);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "p_audio(ms)", latency->p_audio);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "i_video(ms)", latency->i_video);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "i_audio(ms)", latency->i_audio);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "latency_prst", latency->latency_present ? STR_YES : STR_NO);
}

static hi_void proc_sink_scdc_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct scdc_property *scdc = &connector->scdc;

    proc_print_name(file, hdmi, SCDC_INFO);

    HI_PROC_PRINT(file, "%-17s: %-10s |", "Ver", scdc->version ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "scdc_sup", scdc->present ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "lte_340mcsc", scdc->lte_340mcsc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "rr_capable", scdc->rr_capable ? STR_YES : STR_NO);
}

static hi_void proc_sink_dsc_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct dsc_property *dsc = &connector->dsc;

    proc_print_name(file, hdmi, DSC_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_1p2", dsc->dsc_1p2 ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "16bpc", dsc->dsc_16bpc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "10bpc", dsc->dsc_10bpc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "max_slice", dsc->max_slice);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "native_420", dsc->y420 ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "12bpc", dsc->dsc_12bpc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "max_frl_rate", dsc->dsc_max_rate);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "tot_chunk_byte", dsc->total_chunk_bytes);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "aLL_bpp", dsc->all_bpp ? STR_YES : STR_NO);
}

static hi_void proc_sink_dolby_vision_v0(struct seq_file *file, struct dolby_v0 dolby)
{
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_Ver", 0);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_min_pq", dolby.target_min_pq);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_White_X", dolby.white_x);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_green_X", dolby.green_x);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_Sup_Y422", dolby.y422_36bit ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_max_pq", dolby.target_max_pq);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_white_Y", dolby.white_y);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_green_Y", dolby.green_y);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_sup_2160P60", dolby.is_2160p60 ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dm_minor_ver", dolby.dm_minor_ver);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_red_X", dolby.red_x);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_blue_X", dolby.blue_x);
    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "glbl_dimming", dolby.global_dimming ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dm_major_Ver", dolby.dm_major_ver);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_red_Y", dolby.red_y);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_blue_Y", dolby.blue_y);
}

static hi_void proc_sink_dolby_vision_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct dolby_property *dolby = &connector->dolby;

    proc_print_name(file, hdmi, DOLBY_INFO);

    if (dolby->support_v0 == HI_TRUE) {
        proc_sink_dolby_vision_v0(file, dolby->v0);
    } else if (dolby->support_v1 == HI_TRUE) {
        /* line 1 */
        HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_ver", "1");
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_green_X", dolby->v1.green_x);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_red_X", dolby->v1.red_x);
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_blue_X", dolby->v1.blue_x);

        /* line 2 */
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "dm_Ver", dolby->v1.dm_version);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_green_y", dolby->v1.green_y);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_red_y", dolby->v1.red_y);
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_blue_y", dolby->v1.blue_y);

        /* line 3 */
        HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_sup_y422", dolby->v1.y422_36bit ? STR_YES : STR_NO);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_min_lum", dolby->v1.target_min_lum);
        HI_PROC_PRINT(file, "%-17s: %-10s |", "colorimetry", dolby->v1.colorimetry ? STR_YES : STR_NO);
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "low_Latency", dolby->v1.low_latency);

        /* line 4 */
        HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_sup_2160P60", dolby->v1.is_2160p60 ? STR_YES : STR_NO);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_max_lum", dolby->v1.target_max_lum);
    } else if (dolby->support_v2 == HI_TRUE) {
        /* line 1 */
        HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_ver", "2");
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_green_x", dolby->v2.green_x);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_red_x", dolby->v2.red_x);
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_blue_x", dolby->v2.blue_x);
        /* line 2 */
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "dm_ver", dolby->v2.dm_version);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_red_y", dolby->v2.red_y);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_green_y", dolby->v2.green_y);
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "dolby_blue_y", dolby->v2.blue_y);
        /* line 3 */
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_min_pq_v2", dolby->v2.target_min_pq_v2);
        HI_PROC_PRINT(file, "%-17s: %-10s |", "dolby_Sup_y422", dolby->v2.y422_36bit ? STR_YES : STR_NO);
        HI_PROC_PRINT(file, "%-17s: %-10s |", "back_light_cntl", dolby->v2.back_light_ctrl ? STR_YES : STR_NO);
        HI_PROC_PRINT(file, "%-17s: %-10d\n", "rgb/y444_10/12b", dolby->v2.y444_rgb_30b36b);
        /* line 4 */
        HI_PROC_PRINT(file, "%-17s: %-10d |", "dolby_max_pq_v2", dolby->v2.target_max_pq_v2);
        HI_PROC_PRINT(file, "%-17s: %-10s |", "glbl_dimming", dolby->v0.global_dimming ? STR_YES : STR_NO);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "back_lt_min_luma", dolby->v2.back_lt_min_lum);
        HI_PROC_PRINT(file, "%-17s: %-10d |", "intfc", dolby->v2.interface);
    } else {
        HI_PROC_PRINT(file, "%-17s: %-10s\n", "dolby_vision_ver", STR_NA);
    }
}

static hi_void proc_sink_hdr_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct hdr_property *hdr = &connector->hdr;

    proc_print_name(file, hdmi, HDR_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "tradition_sdr", hdr->eotf.traditional_sdr ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "s_type1", hdr->st_metadata.s_type1 ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dynamic_type1", hdr->dy_metadata.d_type1_support ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "dynamic_type3", hdr->dy_metadata.d_type3_support ? STR_YES : STR_NO);

    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "tradition_hdr", hdr->eotf.traditional_hdr ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "max_lum", hdr->st_metadata.max_lum_cv);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "ver_type1", hdr->dy_metadata.d_type1_version);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "dynamic_type4", hdr->dy_metadata.d_type4_support ? STR_YES : STR_NO);

    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "smpte_st2084", hdr->eotf.st2084_hdr ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "avg_lum", hdr->st_metadata.aver_lum_cv);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dynamic_type2", hdr->dy_metadata.d_type2_support ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "ver_type4", hdr->dy_metadata.d_type4_version);

    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "hybrid_long_gama", hdr->eotf.hlg ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "min_lum", hdr->st_metadata.min_lum_cv);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "Ver_type2", hdr->dy_metadata.d_type2_version);
}

static hi_void proc_sink_vrr_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct vrr_property *vrr = &connector->vrr;

    proc_print_name(file, hdmi, VRR_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "fva_sup", vrr->fva ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "m_delta_sup", vrr->m_delta ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "cnm_vrr_sup", vrr->cnm_vrr ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "cinema_vrr_sup", vrr->cinema_vrr ? STR_YES : STR_NO);

    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "fapa_start_locat", vrr->fapa_start_locat ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "allm_sup", vrr->allm ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "vrr_max", vrr->vrr_max);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "vrr_min", vrr->vrr_min);
}

static hi_void proc_sink_disp_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_connector *connector = hdmi->connector;
    struct hdmi_display_info *display_info = &connector->display_info;

    proc_print_name(file, hdmi, DISPLAY_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "cea_ver", display_info->cea_rev);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "force_max_tmds", display_info->force_max_tmds ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "hdmi_sup", display_info->has_hdmi_infoframe ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "ffe_lvl", display_info->ffe_level);

    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "max_wid(cm)", display_info->width_cm);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "max_tmds_clk(kHz)", display_info->max_tmds_clock);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dvi_dual_sup", display_info->dvi_dual ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "max_frl_rt", display_info->max_frl_rate);

    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "max_height(cm)", display_info->height_cm);
}

static hi_void proc_edid_raw_data(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 i;
    struct hdmi_connector *connector = hdmi->connector;
    hi_u32 len = connector->edid_size;
    hi_u32 max_edid_len = 4 * EDID_LENGTH; /* 4:edid max segment num */
    hi_u8 *data = (hi_u8 *)connector->edid_raw;

    proc_print_name(file, hdmi, EDID_RAW_DATA);

    len = (len < max_edid_len) ? len : max_edid_len;
    for (i = 0; i < len; i++) {
        if (i == 0) {
            HI_PROC_PRINT(file, "/*%02xH:*/ ", i);
        }

        HI_PROC_PRINT(file, "0x%02x,", data[i]);

        if (((i + 1) % 4 == 0) && ((i + 1) < len)) { /* 4:print format */
            HI_PROC_PRINT(file, " ");
        }

        if (((i + 1) % 16 == 0) && ((i + 1) < len)) { /* 16:print format */
            HI_PROC_PRINT(file, "\n/*%02xH:*/ ", i);
        }
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_void proc_dsc_em_data_pps_info(struct seq_file *file, hi_u32 i)
{
    switch (i) {
        case 0: /* 0: pkg 1 */
            HI_PROC_PRINT(file, "pps[0:20]<-->pkg1[10:30]\n");
            break;
        case 1: /* 1: pkg 2 */
            HI_PROC_PRINT(file, "pps[21:48]<-->pkg2[3:30]\n");
            break;
        case 2: /* 2: pkg 3 */
            HI_PROC_PRINT(file, "pps[49:76]<-->pkg3[3:30]\n");
            break;
        case 3: /* 3: pkg 4 */
            HI_PROC_PRINT(file, "pps[77:104]<-->pkg4[3:30]\n");
            break;
        case 4: /* 4: pkg 5 */
            HI_PROC_PRINT(file, "pps[105:127]<-->pkg5[3:25]\n");
            break;
        default :
            HI_PROC_PRINT(file, "\n");
            break;
    }
}

static hi_void proc_dsc_em_data_array_print(struct seq_file *file, struct rc_parameter_set *rc_param,
                                            enum print_part bit)
{
    hi_u32 i;

    if ((bit < MIN) || (bit > MAX)) {
        HI_PROC_PRINT(file, "print dsc em data  err.");
        return;
    }

    for (i = 0; i < NUM_BUF_RANGES; i++) {
        if (bit == MIN) {
            HI_PROC_PRINT(file, "%02u ", rc_param[i].range_min_qp);
        } else if (bit == OFFSET) {
            HI_PROC_PRINT(file, "%02u ", rc_param[i].range_bpg_offset);
        } else {
            HI_PROC_PRINT(file, "%02u ", rc_param[i].range_max_qp);
        }
        if ((i + 1) % 5 == 0) { /* 5:print format */
            HI_PROC_PRINT(file, " ");
        }
    }
}

static hi_void proc_dsc_em_data_info_third_part(struct seq_file *file, struct hisilicon_hdmi *hdmi, em_data_info data)
{
    hi_u32 i, j;
    hi_u32 pkg_base;

    HI_PROC_PRINT(file, "%s: ", "rc_buf_thresh[0:13]");
    for (i = 0; i < NUM_BUF_RANGES - 1; i++) {
        HI_PROC_PRINT(file, "%02u ", data.em_data.rc_buf_thresh[i]);
        if ((i + 1) % 5 == 0) { /* 5:print format */
            HI_PROC_PRINT(file, " ");
        }
    }
    HI_PROC_PRINT(file, "\n");
    HI_PROC_PRINT(file, "%-19s: ", "rng_bpg_ofs[0:14]");
    proc_dsc_em_data_array_print(file, data.em_data.rc_parameters, OFFSET);
    HI_PROC_PRINT(file, "\n");
    HI_PROC_PRINT(file, "%-19s: ", "rng_min_qp[0:14]");
    proc_dsc_em_data_array_print(file, data.em_data.rc_parameters, MIN);
    HI_PROC_PRINT(file, "\n");
    HI_PROC_PRINT(file, "%-19s: ", "rng_max_qp[0:14]");
    proc_dsc_em_data_array_print(file, data.em_data.rc_parameters, MAX);
    HI_PROC_PRINT(file, "\n");

    proc_print_name(file, hdmi, PPS_RAW_DATA);

    for (j = 0; j < 6; j++) { /* 6: emp pkg num */
        pkg_base = j * 31; /* 31: pkg len */
        HI_PROC_PRINT(file, "em_data_pkg%u:", j + 1);
        proc_dsc_em_data_pps_info(file, j);
        for (i = pkg_base; i < pkg_base + 31; i++) { /* 31: pkg len */
            HI_PROC_PRINT(file, "0x%02x,", data.raw_data[i]);
            if ((i - pkg_base + 1) % 5 == 0) { /* 5 :print format */
                HI_PROC_PRINT(file, " ");
            }
            if ((i - pkg_base + 1) % 20 == 0) { /* 20 :print format */
                HI_PROC_PRINT(file, "\n");
            }
        }
        HI_PROC_PRINT(file, "\n");
    }
}

static hi_void proc_dsc_em_data_info_sec_part(struct seq_file *file, em_data_info data)
{
    /* line 8 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "vfr", data.em_data.vfr);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "bits_per_pixel", data.em_data.bits_per_pixel);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "init_ofs", data.em_data.inital_offset);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "sec_line_ofs_adj", data.em_data.second_line_offset_adj);
    /* line 9 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "sync", data.em_data.sync);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "pic_height", data.em_data.pic_height);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "final_ofs", data.em_data.final_offset);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "hfront", data.em_data.hfront);
    /* line 10 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "org_id", data.em_data.organization_id);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "pic_width", data.em_data.pic_width);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "flatness_min_qp", data.em_data.flatness_min_qp);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "hsync", data.em_data.hsync);
    /* line 11 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "set_tag", data.em_data.data_set_tag);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "slice_height", data.em_data.slice_height);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "flatness_max_qp", data.em_data.flatness_max_qp);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "hback", data.em_data.hback);
    /* line 12 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "set_len", data.em_data.data_set_length);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "slice_width", data.em_data.slice_width);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "rc_model_sz", data.em_data.rc_model_size);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "hcact", data.em_data.hcactive);
    /* line 13 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "dsc_ver_ma", data.em_data.dsc_version_major);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "chunk_sz", data.em_data.chunk_size);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "rc_edge_factor", data.em_data.rc_edge_factor);
    /* line 14 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "dsc_ver_mi", data.em_data.dsc_version_minor);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "init_xmit_delay", data.em_data.initial_xmit_delay);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "rc_incr_lmt0", data.em_data.rc_quant_incr_limit0);
}

static hi_void proc_dsc_em_data_info_first_part(struct seq_file *file, em_data_info data)
{
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "first", data.em_data.first);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "pps_identi", data.em_data.pps_identifier);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "init_dec_delay", data.em_data.initial_dec_delay);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "rc_inc_lmt1", data.em_data.rc_quant_incr_limit1);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "last", data.em_data.last);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "bits_per_cmpnt", data.em_data.bits_per_component);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "init_scale_val", data.em_data.initial_scale_value);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "rc_tgt_ofs_hi", data.em_data.rc_tgt_offset_hi);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "sequan_index", data.em_data.sequence_index);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "linebuf_dpt", data.em_data.linebuf_depth);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "scale_inc", data.em_data.scale_increment_interval);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "rc_tgt_off_lo", data.em_data.rc_tgt_offset_lo);
    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "pkt_new", data.em_data.pkt_new);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "blk_pred_en", data.em_data.block_pred_enable);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "scale_dec", data.em_data.scale_decrement_interval);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "native_420", data.em_data.native_420);
    /* line 5 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "pkt_end", data.em_data.pkt_end);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "conv_rgb", data.em_data.convert_rgb);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "1st_line_bpg_off", data.em_data.first_line_bpg_offset);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "native_422", data.em_data.native_422);
    /* line 6 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "ds_type", data.em_data.ds_type);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "simple_422", data.em_data.simple_422);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "nfl_bpg_off", data.em_data.nfl_bpg_offset);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "sec_line_bpg_off", data.em_data.second_line_bpg_offset);
    /* line 7 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "afr", data.em_data.afr);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "vbr_en", data.em_data.vbr_enable);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "slice_bpg_off", data.em_data.slice_bpg_offset);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "nsl_bpg_off", data.em_data.nsl_bpg_offset);
}

static hi_void proc_dsc_em_data_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    em_data_info data;
    hi_s32 ret;

    if (memset_s(&data, sizeof(em_data_info), 0, sizeof(em_data_info))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    ret = hal_ctrl_get_dsc_emp(hdmi, &data);
    if (ret < 0) {
        HI_PROC_PRINT(file, "get video path info err.\n");
        return;
    }

    proc_print_name(file, hdmi, EM_DATA_PACK);
    proc_dsc_em_data_info_first_part(file, data);
    proc_dsc_em_data_info_sec_part(file, data);
    proc_dsc_em_data_info_third_part(file, hdmi, data);

    HI_PROC_PRINT(file, "\n");
}

static hi_s32 proc_dsc_status_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct dsc_ctrl *dsc = ctrl->dsc;
    proc_print_name(file, hdmi, DSC_STATUS);

    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_en", dsc->status.enable ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "o_intf_low_bw", dsc->status.o_intf_low_bw ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "isync_fifo_full", dsc->status.isync_fifo_full ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "rc_err_vld", dsc->status.rc_error_vld);

    return 0;
}

static hi_s32 proc_dsc_caps_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct dsc_ctrl *dsc = ctrl->dsc;

    proc_print_name(file, hdmi, DSC_CAPS);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_1p2", dsc->caps.dsc_1p2 ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_12bpc", dsc->caps.dsc_12bpc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "max_frl_rt", dsc->caps.dsc_max_frl_rate);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "max_silce_cnt", dsc->caps.max_silce_cout);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_native420", dsc->caps.dsc_native420 ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_16bpc", dsc->caps.dsc_16bpc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "max_clk_per_slc", dsc->caps.max_pixel_clk_per_slice);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "max_Chunk_byte", dsc->caps.max_chunk_byte);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "dsc_10bpc", dsc->caps.dsc_10bpc ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "dsc_all_bpp", dsc->caps.dsc_all_bpp ? STR_YES : STR_NO);

    return 0;
}

static hi_s32 proc_dsc_timing_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct dsc_ctrl *dsc = ctrl->dsc;

    proc_print_name(file, hdmi, DSC_TIMING);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "hc_act", dsc->timing_info.hcactive);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "h_back", dsc->timing_info.hback);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "v_sync", dsc->timing_info.vsync);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "de_polar", dsc->timing_info.de_polarity);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "hc_blank", dsc->timing_info.hcblank);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "h_blank", dsc->timing_info.hblank);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "v_back", dsc->timing_info.vback);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "h_polar", dsc->timing_info.h_polarity);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "h_front", dsc->timing_info.hfront);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "v_front", dsc->timing_info.vfront);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "v_act", dsc->timing_info.vactive);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "v_polar", dsc->timing_info.v_polarity);

    return 0;
}

static hi_s32 proc_dsc_para_input_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct dsc_ctrl *dsc = ctrl->dsc;

    proc_print_name(file, hdmi, PARA_INPUT);
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "is_encode", dsc->inpare.is_encoder);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "native_422", dsc->inpare.native_422);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "pic_wdth", dsc->inpare.pic_width);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "slice_wdth", dsc->inpare.slice_width);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "cnv_rgb", dsc->inpare.convert_rgb);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "bit_per_cmpt", dsc->inpare.bits_per_component);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "pic_height", dsc->inpare.pic_height);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "slice_height", dsc->inpare.slice_height);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "native_420", dsc->inpare.native_420);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "bit_per_pixel", dsc->inpare.bits_per_pixel);

    return 0;
}

static hi_void proc_dsc_cfg_info_sec_part(struct seq_file *file, struct dsc_ctrl *dsc)
{
    /* line 7 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "simple_422", dsc->cfg_s.simple_422);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "init_xmt_delay", dsc->cfg_s.initial_xmit_delay);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "flatness_min_qp", dsc->cfg_s.flatness_min_qp);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "xstart", dsc->cfg_s.xstart);
    /* line 8 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "native_422", dsc->cfg_s.native_422);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "init_dec_delay", dsc->cfg_s.initial_dec_delay);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "nsl_bpg_ofs", dsc->cfg_s.nsl_bpg_offset);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "flat_qp_delta", dsc->cfg_s.somewhat_flat_qp_delta);
    /* line 9 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "native_420", dsc->cfg_s.native_420);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "block_pred_en", dsc->cfg_s.block_pred_enable);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "flatness_det_thre", dsc->cfg_s.flatness_det_thresh);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "pps_id", dsc->cfg_s.pps_identifier);
    /* line 10 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "native_444", dsc->cfg_s.native_444);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "first_ln_bpg_ofs", dsc->cfg_s.first_line_bpg_ofs);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "flatness_mod", dsc->cfg_s.flatness_mod);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "very_flat_qp", dsc->cfg_s.very_flat_qp);
    /* line 11 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "pic_wid", dsc->cfg_s.pic_width);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "scnd_ln_bpg_ofs", dsc->cfg_s.second_line_bpg_ofs);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "init_scale_val", dsc->cfg_s.initial_scale_value);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "scnd_ln_ofs_adj", dsc->cfg_s.second_line_ofs_adj);
    /* line 12 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "pic_height", dsc->cfg_s.pic_height);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "init_ofs", dsc->cfg_s.initial_offset);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "scal_decr", dsc->cfg_s.scale_decrement_interval);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "flat_qp_thresh", dsc->cfg_s.somewhat_flat_qp_thresh);
}

static hi_void proc_dsc_cfg_info_first_part(struct seq_file *file, struct dsc_ctrl *dsc)
{
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "ln_buf_dpt", dsc->cfg_s.linebuf_depth);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rc_tgt_ofs_hi", dsc->cfg_s.rc_tgt_offset_hi);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "nfl_bpg_ofs", dsc->cfg_s.nfl_bpg_offset);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "full_ich_err_prec", dsc->cfg_s.full_ich_err_precision);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rcb_bits", dsc->cfg_s.rcb_bits);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rc_tgt_ofs_lo", dsc->cfg_s.rc_tgt_offset_lo);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "slice_bpg_ofs", dsc->cfg_s.slice_bpg_offset);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "dsc_ver_minor", dsc->cfg_s.dsc_version_minor);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "bit_per_cmpt", dsc->cfg_s.bits_per_component);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "bits_per_pixel", dsc->cfg_s.bits_per_pixel);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "ystart", dsc->cfg_s.ystart);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "final_ofs", dsc->cfg_s.final_offset);
    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "cnv_rgb", dsc->cfg_s.convert_rgb);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rc_edge_factor", dsc->cfg_s.rc_edge_factor);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rc_model_sz", dsc->cfg_s.rc_model_size);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "vbr_en", dsc->cfg_s.vbr_enable);
    /* line 5 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "slice_wdth", dsc->cfg_s.slice_width);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rc_q_incr_lim1", dsc->cfg_s.rc_quant_incr_limit1);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "mux_word_sz", dsc->cfg_s.mux_word_size);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "scal_incr", dsc->cfg_s.scale_increment_interval);
    /* line 6 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "slice_height", dsc->cfg_s.slice_height);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "rc_q_incr_lim0", dsc->cfg_s.rc_quant_incr_limit0);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "flatness_max_qp", dsc->cfg_s.flatness_max_qp);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "chunk_sz", dsc->cfg_s.chunk_size);
}

static hi_s32 proc_dsc_cfg_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct dsc_ctrl *dsc = ctrl->dsc;
    hi_u32 i = 0;

    proc_print_name(file, hdmi, DSC_CFG);
    proc_dsc_cfg_info_first_part(file, dsc);
    proc_dsc_cfg_info_sec_part(file, dsc);
    /* line 13 */
    HI_PROC_PRINT(file, "%s: ", "rc_buf_thresh[0:13]");
    for (i = 0; i < NUM_BUF_RANGES - 1; i++) {
        HI_PROC_PRINT(file, "%02u ", dsc->cfg_s.rc_buf_thresh[i]);
        if ((i + 1) % 5 == 0) { /* 5:print format */
            HI_PROC_PRINT(file, " ");
        }
    }
    HI_PROC_PRINT(file, "\n");
    /* line 14 */
    HI_PROC_PRINT(file, "%-19s: ", "rng_bpg_ofs[0:14]");
    for (i = 0; i < NUM_BUF_RANGES; i++) {
        HI_PROC_PRINT(file, "%02u ", dsc->cfg_s.rc_range_parameters[i].range_bpg_offset);
        if ((i + 1) % 5 == 0) { /* 5:print format */
            HI_PROC_PRINT(file, " ");
        }
    }
    HI_PROC_PRINT(file, "\n");
    /* line 15 */
    HI_PROC_PRINT(file, "%-19s: ", "rng_min_qp[0:14]");
    for (i = 0; i < NUM_BUF_RANGES; i++) {
        HI_PROC_PRINT(file, "%02u ", dsc->cfg_s.rc_range_parameters[i].range_min_qp);
        if ((i + 1) % 5 == 0) { /* 5:print format */
            HI_PROC_PRINT(file, " ");
        }
    }
    HI_PROC_PRINT(file, "\n");
    /* line 16 */
    HI_PROC_PRINT(file, "%-19s: ", "rng_max_qp[0:14]");
    for (i = 0; i < NUM_BUF_RANGES; i++) {
        HI_PROC_PRINT(file, "%02u ", dsc->cfg_s.rc_range_parameters[i].range_max_qp);
        if ((i + 1) % 5 == 0) { /* 5:print format */
            HI_PROC_PRINT(file, " ");
        }
    }
    HI_PROC_PRINT(file, "\n");

    return 0;
}

static hi_s32 proc_frl_all_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct frl *frl = ctrl->frl;
    hi_u8 index;

    proc_print_name(file, hdmi, FRL_INFO);

    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-9s |", "max_rate_prior", frl->config.max_rate_proir ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "sink_ver", frl->config.sink_version);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "ln_1_0_ltp_req", frl->scdc.ln_1_0_ltp_req);
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "phy_output", frl->stat.phy_output ? STR_YES : STR_NO);

    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "rdy_timeout", frl->config.ready_timeout);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "scdc_present", frl->config.scdc_present ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "ln_3_2_ltp_req", frl->scdc.ln_3_2_ltp_req);
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "video_transifer", frl->stat.video_transifer ? STR_YES : STR_NO);

    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "tflt_margin", frl->config.tflt_margin);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "src_ffe_levels", frl->config.src_ffe_levels);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "frl_max", frl->scdc.frl_max ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "frl_start", frl->stat.frl_start ? STR_YES : STR_NO);

    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "updt_flag_magin", frl->config.update_flag_magin);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "frl_rate", frl->scdc.frl_rate);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "flt_no_timeout", frl->scdc.flt_no_timeout ? STR_YES : STR_NO);
    index = proc_check_max(frl->stat.frl_state, ARRAY_SIZE(g_frl_state));
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "frl_state", g_frl_state[index]);

    /* line 5 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "ltsp_poll_inter", frl->config.ltsp_poll_interval);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "ffe_levels", frl->scdc.ffe_levels);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "ready_timeout", frl->stat.ready_timeout ? STR_YES : STR_NO);
    index = proc_check_max(frl->stat.event, ARRAY_SIZE(g_frl_event));
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "event", g_frl_event[index]);

    /* line 6 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "frl_max_rate", frl->config.frl_max_rate);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "flt_update", frl->scdc.flt_update ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "tflt_timeout", frl->stat.tflt_timeout ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-19s: %-9d\n", "ffe_levels", frl->stat.ffe_levels[0]);

    /* line 7 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "frl_min_rate", frl->config.frl_min_rate);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "flt_start", frl->scdc.flt_start ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9s\n", "ltsp_timeout", frl->stat.ltsp_timeout ? STR_YES : STR_NO);

    /* line 8 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "dsc_frl_min_rate", frl->config.dsc_frl_min_rate);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "flt_ready", frl->scdc.flt_ready ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9s\n", "ltsp_poll", frl->stat.ltsp_poll ? STR_YES : STR_NO);

    return 0;
}

static hi_s32 proc_detail_timing_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hi_display_mode *mode = &ctrl->mode;

    proc_print_name(file, hdmi, DETAIL_TIMING);
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "h_total", mode->timing_data.in.detail.htotal);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "v_total", mode->timing_data.in.detail.vtotal);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "h_front", mode->timing_data.in.detail.hfront);
    HI_PROC_PRINT(file, "%-19s: %-9u\n", "v_front", mode->timing_data.in.detail.vfront);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "h_back", mode->timing_data.in.detail.hback);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "v_back", mode->timing_data.in.detail.vback);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "h_act", mode->timing_data.in.detail.hactive);
    HI_PROC_PRINT(file, "%-19s: %-9u\n", "v_act", mode->timing_data.in.detail.vactive);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "h_sync", mode->timing_data.in.detail.hsync);
    HI_PROC_PRINT(file, "%-17s: %-9u\n", "v_sync", mode->timing_data.in.detail.vsync);

    return 0;
}

static hi_s32 proc_hw_cfg_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hdmi_hw_config *pre_hw_config = &ctrl->pre_hw_config;
    struct hdmi_hw_config *cur_hw_config = &ctrl->cur_hw_config;

    proc_print_name(file, hdmi, HW_CFG);
    /* line1 */
    HI_PROC_PRINT(file, "%-17s: %-9s |", "work_mode", cur_hw_config->work_mode ? "frl" : "tmds");
    HI_PROC_PRINT(file, "%-17s: %-9s |", "pre_work_mode", pre_hw_config->work_mode ? "frl" : "tmds");
    HI_PROC_PRINT(file, "%-17s: %-9s |", "tmds_scr_en", cur_hw_config->tmds_scr_en ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-19s: %-9s \n", "pre_tmds_scr_en", pre_hw_config->tmds_scr_en ? STR_YES : STR_NO);
    /* line2 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "tmds_clock", cur_hw_config->tmds_clock);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "pre_tmds_clock", pre_hw_config->tmds_clock);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "dvi_mode", cur_hw_config->dvi_mode ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "pre_dvi_mode", pre_hw_config->dvi_mode ? STR_YES : STR_NO);
    /* line3 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "frl_rt", cur_hw_config->cur_frl_rate);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "pre_frl_rt", pre_hw_config->cur_frl_rate);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "min_frl_rt", cur_hw_config->min_frl_rate);
    HI_PROC_PRINT(file, "%-19s: %-9u\n", "pre_min_frl_rt", pre_hw_config->min_frl_rate);
    /* line4 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "max_frl_rt", cur_hw_config->max_frl_rate);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "pre_max_frl_rt", pre_hw_config->max_frl_rate);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "min_dsc_frl_rt", cur_hw_config->min_dsc_frl_rate);
    HI_PROC_PRINT(file, "%-19s: %-9u\n", "pre_min_dsc_frl_rt", pre_hw_config->min_dsc_frl_rate);
    /* line5 */
    HI_PROC_PRINT(file, "%-17s: %-9s |", "dsc_enable", cur_hw_config->dsc_enable ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "pre_dsc_enable", pre_hw_config->dsc_enable ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "bpp_target", cur_hw_config->bpp_target);
    HI_PROC_PRINT(file, "%-19s: %-9u\n", "pre_bpp_target", pre_hw_config->bpp_target);
    /* line6 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "slice_width", cur_hw_config->slice_width);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "pre_slice_width", pre_hw_config->slice_width);
    HI_PROC_PRINT(file, "%-17s: %-9u |", "hc_act", cur_hw_config->hcactive);
    HI_PROC_PRINT(file, "%-19s: %-9u\n", "pre_hc_act", pre_hw_config->hcactive);
    /* line7 */
    HI_PROC_PRINT(file, "%-17s: %-9u |", "hc_blk", cur_hw_config->hcblank);
    HI_PROC_PRINT(file, "%-17s: %-9u\n", "pre_hc_blk", pre_hw_config->hcblank);

    return 0;
}

static hi_s32 proc_hw_status_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    enum hdmi_connector_status status;

    proc_print_name(file, hdmi, HW_STATUS);
    status = proc_check_max(hdmi->connector->status, ARRAY_SIZE(g_hpd_status_str));
    HI_PROC_PRINT(file, "%-17s: %-9s\n", "hpd_status", g_hpd_status_str[status]);

    return 0;
}

static hi_s32 proc_soft_status_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_controller *ctrl = hdmi->ctrl;

    proc_print_name(file, hdmi, SOFT_STATUS);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "tpll_enable", ctrl->tpll_enable ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "ppll_enable", ctrl->ppll_enable ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "min_rt_prior", ctrl->min_rate_prior ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-19s: %-9s\n", "tmds_prior", ctrl->tmds_prior ? STR_YES : STR_NO);

    return 0;
}

static hi_s32 proc_port_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 id = hdmi->id;
    hi_u32 index;

    index = proc_check_max(id, ARRAY_SIZE(g_hdmi_id_str));
    HI_PROC_PRINT(file, "HDMI ID: %s\n\n", g_hdmi_id_str[index]);

    return 0;
}

static hi_void proc_ver_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    HI_PROC_PRINT(file, "%s\n", VERSION_STRING);
    HI_PROC_PRINT(file, "HDMI VERSION: %u\n", hdmi->version);
}

static hi_s32 proc_sink_read(hi_void *file, hi_void *private)
{
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    proc_ver_info(file, hdmi);
    proc_port_info(file, hdmi);
    proc_edid_raw_data(file, hdmi);
    proc_sink_disp_info(file, hdmi);
    proc_sink_vrr_info(file, hdmi);
    proc_sink_hdr_info(file, hdmi);
    proc_sink_dolby_vision_info(file, hdmi);
    proc_sink_scdc_info(file, hdmi);
    proc_sink_dsc_info(file, hdmi);
    proc_sink_latency_info(file, hdmi);
    proc_sink_audio_info(file, hdmi);
    proc_sink_base_info(file, hdmi);
    proc_sink_color_info(file, hdmi);
    proc_sink_timing_info(file, hdmi);

    return 0;
}

static hi_void proc_audio_infoframe(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 i;
    hi_u32 index;
    hi_char chn_sts[25]; /* channel status buff size 25 */
    hi_u8 info[HDMI_AUDIO_INFOFRAME_SIZE] = {0};
    struct hdmi_audio_infoframe audio;

    if (memset_s(&audio, sizeof(struct hdmi_audio_infoframe), 0, sizeof(struct hdmi_audio_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    if (memset_s(chn_sts, sizeof(chn_sts), '\0', sizeof(chn_sts))) {
        HDMI_ERR("memset_s err\n");
        return;
    }

    proc_print_name(file, hdmi, AUDIO_INFO);
    /* line 1 */
    index = proc_check_max(audio.type, ARRAY_SIZE(g_infoframe_type_str));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "infofrm_type", g_infoframe_type_str[index]);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "ver", audio.version);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "len", audio.length);
    index = proc_check_max(audio.coding_type, ARRAY_SIZE(g_audio_code_type_str));
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "code_type", g_audio_code_type_str[index]);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10d |", "chn_cnt", audio.channels);
    index = proc_check_max(audio.sample_frequency, ARRAY_SIZE(g_audio_sample_rate_str));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "freq", g_audio_sample_rate_str[index]);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "depth", STR_NO);
    index = proc_check_max(audio.sample_size, ARRAY_SIZE(g_audio_sample_bit_str));
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "size", g_audio_sample_bit_str[index]);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "down_mix_inhibit", audio.downmix_inhibit ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "lvl_shft_val", audio.level_shift_value);
    HI_PROC_PRINT(file, "%-17s: 0x%02x (%d)\n", "chn_alloc", audio.channel_allocation, audio.channel_allocation);
    /* line 4 */
    HI_PROC_PRINT(file, "%-17s:\n", "info_raw_data");
    for (i = 1; i < HDMI_AUDIO_INFOFRAME_SIZE; i++) {
        if (i % 4 == 0) { /* 4:print format */
            HI_PROC_PRINT(file, " ");
        }
        HI_PROC_PRINT(file, "%02x ", info[i]);
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_s32 proc_audio_path_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 index;
    hi_s32 ret;
    audio_path audio_path_info;

    ret = hal_ctrl_get_audio_path(hdmi, &audio_path_info);
    if (ret < 0) {
        HI_PROC_PRINT(file, "get audio path info err.\n");
        return ret;
    }

    proc_print_name(file, hdmi, AUDIO_PATH);
    /* line 1 */
    index = proc_check_max(audio_path_info.chn_num, ARRAY_SIZE(g_audio_chn_num));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "chn_num", g_audio_chn_num[index]);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "reg_n", audio_path_info.reg_n);
    index = proc_check_max(audio_path_info.input_type, ARRAY_SIZE(g_audio_input_type));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "input_type", g_audio_input_type[index]);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "compressed", audio_path_info.is_pcm_compress ? STR_YES : STR_NO);
    /* line 2 */
    index = proc_check_max(audio_path_info.sample_rate_num, ARRAY_SIZE(g_audio_rate));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "rate", g_audio_rate[index]);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "reg_cts", audio_path_info.reg_cts);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "input_len", audio_path_info.input_len);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "out_bit_len", audio_path_info.out_bit_len);

    return 0;
}

static hi_void proc_audio_attr_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_u32 index;
    hi_char chn_sts[25]; /* channel status buff size 25 */
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct ao_attr *attr = &ctrl->attr;
    struct hdmi_audio_infoframe audio;

    if (memset_s(&audio, sizeof(struct hdmi_audio_infoframe), 0, sizeof(struct hdmi_audio_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    if (memset_s(chn_sts, sizeof(chn_sts), '\0', sizeof(chn_sts))) {
        HDMI_ERR("memset_s err\n");
        return;
    }

    proc_print_name(file, hdmi, AUDIO_ATTR);
    /* line 1 */
    index = proc_check_max(attr->aud_input_type, ARRAY_SIZE(g_sound_intf_Str));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "input_type", g_sound_intf_Str[index]);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "depth", attr->aud_sample_size);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "rate", attr->aud_sample_rate);
    ret = strncpy_s(chn_sts, sizeof(chn_sts), attr->channel_status, 7); /* size 7 */
    if (ret < 0) {
        HDMI_ERR("strncpy_s err\n");
        return;
    }
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "chn_sts", chn_sts);
    /* line 2 */
    index = proc_check_max(attr->aud_channels, ARRAY_SIZE(g_audio_channel_str));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "chn_num", g_audio_channel_str[index]);
    index = proc_check_max(attr->aud_codec, ARRAY_SIZE(g_audio_code_type_str));
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "code_type", g_audio_code_type_str[index]);
}

static hi_s32 proc_video_vsif_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u8 buffer[HDMI_INFOFRAME_BUF_SIZE] = {0};
    union hdmi_infoframe frame;
    hi_u32 i;
    hi_u8 index;

    hal_ctrl_hw_get_info_frame(hdmi, HDMI_INFOFRAME_TYPE_VENDOR, buffer, sizeof(buffer));
    hdmi_infoframe_unpack(&frame, buffer, sizeof(buffer));

    proc_print_name(file, hdmi, VIDEO_VSIF);
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "ver", frame.vendor.hdmi.version);
    HI_PROC_PRINT(file, "%-17s: %-10u |", "length", frame.vendor.hdmi.length);
    HI_PROC_PRINT(file, "%-17s: %-10x |", "oui", frame.vendor.hdmi.oui);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "vic", frame.vendor.hdmi.vic);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "s3d_ext_data", frame.vendor.hdmi.s3d_ext_data);
    index = proc_check_max(frame.vendor.hdmi.s3d_struct, ARRAY_SIZE(g_3d_structure));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "s3d_struct", g_3d_structure[index]);
    /* line 3 */
    HI_PROC_PRINT(file, "vsi_raw_data:\n");
    for (i = 0; i < HDMI_INFOFRAME_BUF_SIZE; i++) {
        HI_PROC_PRINT(file, "0x%02x,", buffer[i]);
        if (((i + 1) % 4 == 0) && ((i + 1) < HDMI_INFOFRAME_BUF_SIZE)) { /* 4:print format */
            HI_PROC_PRINT(file, " ");
        }

        if (((i + 1) % 16 == 0) && ((i + 1) < HDMI_INFOFRAME_BUF_SIZE)) { /* 16:print format */
            HI_PROC_PRINT(file, "\n");
        }
    }
    HI_PROC_PRINT(file, "\n");

    return 0;
}

static hi_s32 proc_video_path_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 index;
    hi_s32 ret;
    video_path video_path_info;

    ret = hal_ctrl_get_video_path(hdmi, &video_path_info);
    if (ret < 0) {
        HI_PROC_PRINT(file, "get video path info err.\n");
        return ret;
    }

    proc_print_name(file, hdmi, VIDEO_PATH);
    /* line 1 */
    index = proc_check_max(video_path_info.out_color_fmt, ARRAY_SIZE(g_color_format));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "out_color_fmt", g_color_format[index]);
    index = proc_check_max(video_path_info.in_color_depth, ARRAY_SIZE(g_color_depth));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "in_colof_dpt", g_color_depth[index]);
    index = proc_check_max(video_path_info.out_color_depth, ARRAY_SIZE(g_color_depth));
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "out_color_dpt", g_color_depth[index]);

    return 0;
}

static hi_s32 proc_video_avi_infoframe(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_u32 index;
    hi_u32 i = 1;
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hi_display_mode *mode = &ctrl->mode;
    hi_u8 buffer[HDMI_INFOFRAME_BUF_SIZE] = {0};
    hi_u32 len;

    hal_ctrl_hw_get_info_frame(hdmi, HDMI_INFOFRAME_TYPE_AVI, buffer, sizeof(buffer));

    proc_print_name(file, hdmi, VIDEO_AVI);
    /* line 1 */
    HI_PROC_PRINT(file, "%-17s: %-10u |", "vic", mode->timing_data.in.vic);
    index = proc_check_max(mode->timing_data.in.picture_aspect_ratio, ARRAY_SIZE(g_pic_aspect_ratio));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "pic_aspect_ratio", g_pic_aspect_ratio[index]);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "it_content_valid",
                  mode->timing_data.in.it_content_valid ? STR_YES : STR_NO);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "bottom_bar", mode->timing_data.in.bottom_bar);
    /* line 2 */
    index = proc_check_max(mode->timing_data.in.scan_info, ARRAY_SIZE(g_scan_info));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "Scan Info", g_scan_info[index]);
    index = proc_check_max(mode->timing_data.in.active_aspect_ratio, ARRAY_SIZE(g_act_aspect_ratio));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "act_aspect_ratio", g_act_aspect_ratio[index]);
    index = proc_check_max(mode->timing_data.in.pixel_repeat, ARRAY_SIZE(g_pixel_repeat));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "pixel_repeat", g_pixel_repeat[index]);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "left_bar", mode->timing_data.in.left_bar);
    /* line 3 */
    HI_PROC_PRINT(file, "%-17s: %-10s |", "aspect_present",
                  mode->timing_data.in.active_aspect_present ? STR_YES : STR_NO);
    index = proc_check_max(mode->timing_data.in.picture_scal, ARRAY_SIZE(g_pic_scaling));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "pic_scal", g_pic_scaling[index]);
    HI_PROC_PRINT(file, "%-17s: %-10d |", "top_bar", mode->timing_data.in.top_bar);
    HI_PROC_PRINT(file, "%-17s: %-10d\n", "right_bar", mode->timing_data.in.right_bar);
    /* line 4 */
    index = proc_check_max(mode->timing_data.in.bar_present, ARRAY_SIZE(g_bar_data_present));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "bar_Present", g_bar_data_present[index]);
    index = proc_check_max(mode->timing_data.in.it_content_type, ARRAY_SIZE(g_it_content_type));
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "it_content_type", g_it_content_type[index]);
    /* line 5 */
    HI_PROC_PRINT(file, "%-17s:\n", "avi_raw_data");

    len = buffer[2] + 4; /* 2: byte of pkg len, 4: offset */
    len = (len > HDMI_INFOFRAME_BUF_SIZE) ? HDMI_INFOFRAME_BUF_SIZE : len;
    for (index = 3; index < len; index++) { /* 3£ºpkg body start address */
        HI_PROC_PRINT(file, "%02x ", buffer[index]);
        if (i % 8 == 0) { /* 8:print format */
            HI_PROC_PRINT(file, "\n");
        } else if (i % 4 == 0) { /* 4:print format */
            HI_PROC_PRINT(file, " ");
        }
        i++;
    }
    HI_PROC_PRINT(file, "\n");

    return 0;
}

static hi_s32 proc_video_attr_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_u32 temp_index;
    hi_u32 index;
    hi_char tmp_str[25] = {0}; /* video timing name max size 25 */
    struct hdmi_controller *ctrl = hdmi->ctrl;
    struct hi_display_mode *mode = &ctrl->mode;

    proc_print_name(file, hdmi, VIDEO_ATTR);
    /* line 1 */
    index = proc_check_max(mode->timing_data.in.picture_aspect_ratio, ARRAY_SIZE(g_pic_aspect_ratio));
    ret = snprintf_s(tmp_str, sizeof(tmp_str), sizeof(tmp_str), "%uX%u@%u %s",
                     mode->timing_data.in.detail.hactive,
                     mode->timing_data.in.detail.vactive,
                     mode->timing_data.in.detail.refresh_rate / 100, /* 100: The unit from Khz to Mhz */
                     g_pic_aspect_ratio[index]);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return ret;
    }
    HI_PROC_PRINT(file, "%-17s: [%03u]%-25s\n", "video_timing", mode->timing_data.in.vic, tmp_str);
    /* line 2 */
    /* color depth */
    index = proc_check_max(mode->timing_data.in.color_depth, ARRAY_SIZE(g_color_depth));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "in_color_dpt", g_color_depth[index]);
    index = proc_check_max(mode->timing_data.out.color_depth, ARRAY_SIZE(g_color_depth));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "out_color_dpt", g_color_depth[index]);
    HI_PROC_PRINT(file, "%-17s: %-10u\n", "pixel_clk", mode->timing_data.in.pixel_clock);
    /* line 3 */
    /* color space */
    index = proc_check_max(mode->timing_data.in.color.color_format, ARRAY_SIZE(g_color_format));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "in_color_space", g_color_format[index]);
    index = proc_check_max(mode->timing_data.out.color.color_format, ARRAY_SIZE(g_color_format));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "out_color_space", g_color_format[index]);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "vSync_pol", mode->timing_data.in.v_sync_pol ? STR_YES : STR_NO);
    /* line 4 */
    /* color colorimetry */
    temp_index = proc_colorimetry2num(mode->timing_data.in.color.colorimetry);
    index = proc_check_max(temp_index, ARRAY_SIZE(g_colorimetry));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "in_colorimetry", g_colorimetry[index].name);
    HI_PROC_PRINT(file, "%-17s: %-10s |", "out_colorimetry", g_colorimetry[index].name);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "hsync_pol", mode->timing_data.in.h_sync_pol ? STR_YES : STR_NO);
    /* line 5 */
    /* YCC and RGB quantizeion */
    index = proc_check_max(mode->timing_data.in.color.ycc_quantization, ARRAY_SIZE(g_ycc_quantizeion));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "in_ycc_quan", g_ycc_quantizeion[index]);
    index = proc_check_max(mode->timing_data.out.color.ycc_quantization, ARRAY_SIZE(g_ycc_quantizeion));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "out_ycc_quan", g_ycc_quantizeion[index]);
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "de_pol", mode->timing_data.in.de_pol ? STR_YES : STR_NO);
    /* line 6 */
    index = proc_check_max(mode->timing_data.in.color.rgb_quantization, ARRAY_SIZE(g_rgb_quantizeion));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "in_rgb_quan", g_rgb_quantizeion[index]);
    index = proc_check_max(mode->timing_data.out.color.rgb_quantization, ARRAY_SIZE(g_rgb_quantizeion));
    HI_PROC_PRINT(file, "%-17s: %-10s |", "out_rgb_quan", g_rgb_quantizeion[index]);
    /* 3d mode */
    index = proc_check_max(mode->timing_data.in.mode_3d, ARRAY_SIZE(g_3d_mode));
    HI_PROC_PRINT(file, "%-17s: %-10s\n", "3d_mode", g_3d_mode[index]);

    return 0;
}

static hi_void proc_cec_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    proc_print_name(file, hdmi, CEC_INFO);

    HI_PROC_PRINT(file, "%-17s: %-9s |", "open", hdmi->cec->open ? "yes" : "no");
    HI_PROC_PRINT(file, "%-17s: %-9X |", "logic_addr", hdmi->cec->status.logical_addr);
    HI_PROC_PRINT(file, "%-17s: %-9X |", "physic_addr", hdmi->cec->status.physical_addr);
    HI_PROC_PRINT(file, "%-19s: %-9s \n", "devive type",
        hdmi->cec->device_type >= CEC_DEVICE_TYPE_MAX ? "error" : g_device_type_name[hdmi->cec->device_type]);

    HI_PROC_PRINT(file, "%-17s: %-9d |", "tgid", hdmi->cec->tgid);
    HI_PROC_PRINT(file, "%-17s: %-9s \n", "addr_status",
        hdmi->cec->status.logic_addr_status > CEC_LOGIC_ADDR_CONFIGURING ?
        "error" : g_logic_addr_status_name[hdmi->cec->status.logic_addr_status]);
}

static hi_void proc_hdcp_info(struct seq_file *file, struct hisilicon_hdmi *hdmi)
{
    struct hdmi_hdcp *hdcp = hdmi->hdcp;
    struct hdcp2x_sink_status hdcp2x_sink = hdcp->sink_st_2x;
    struct hdcp_usr_status status = hdcp->status;
    hi_u16 index;

    if (hdcp == HI_NULL) {
        HDMI_ERR("null ptr err.\n");
        return;
    }

    proc_print_name(file, hdmi, HDCP_INFO);
    /* line 1 */
    index = proc_check_max(hdcp->usr_mode, ARRAY_SIZE(g_hdcp_mode));
    HI_PROC_PRINT(file, "%-17s: %-9s |", "usr_mode", g_hdcp_mode[index]);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "load_mcu_code2x", hdcp->load_mcu_code2x ? "true" : "false");
    HI_PROC_PRINT(file, "%-17s: %-9u |", "dev_cnt", hdcp2x_sink.rx_info.dev_cnt);
    HI_PROC_PRINT(file, "%-17s: %-9u\n", "depth", hdcp2x_sink.rx_info.depth);
    /* line 2 */
    HI_PROC_PRINT(file, "%-17s: %-9s |", "usr_start", hdcp->usr_start ? "true" : "false");
    HI_PROC_PRINT(file, "%-17s: %-9s |", "auth_start", status.auth_start ? "true" : "false");
    HI_PROC_PRINT(file, "%-17s: %-9s |", "devs_exceed", hdcp2x_sink.rx_info.max_devs_exceeded ? "true" : "false");
    index = proc_check_max(status.work_version, ARRAY_SIZE(g_hdcp_ver));
    HI_PROC_PRINT(file, "%-17s: %-9s\n", "work_version", g_hdcp_ver[index]);
    /* line 3 */
    if (hdcp->usr_reauth_times == HDCP_DEFAULT_REAUTH_TIMES) {
        HI_PROC_PRINT(file, "%-17s: %-9s |", "usr_reauth_times", "unlimited");
    } else {
        HI_PROC_PRINT(file, "%-17s: %-9u |", "usr_reauth_times", hdcp->usr_reauth_times);
    }
    HI_PROC_PRINT(file, "%-17s: %-9s |", "auth_success", status.auth_success ? "true" : "false");
    HI_PROC_PRINT(file, "%-17s: %-9s |", "cascade", hdcp2x_sink.rx_info.max_cascade_exceeded ? "true" : "false");
    HI_PROC_PRINT(file, "%-17s: %-9d\n", "cur_reauth_times", status.cur_reauth_times);
    /* line 4 */
    HI_PROC_PRINT(file, "%-17s: %-9d |", "start_delay_time", hdcp->start_delay_time);
    HI_PROC_PRINT(file, "%-17s: %-9d |", "cur_delay_time", hdcp->cur_delay_time);
    HI_PROC_PRINT(file, "%-17s: %-9s |", "max_20_dn_stream",
                  hdcp2x_sink.rx_info.max_hdcp20_down_stream ? "true" : "false");
    index = proc_check_max(status.err_code, HDCP_ERR_CODE_CATEGORY_NUM * EACH_CATEGORY_TYPE_NUM);
    HI_PROC_PRINT(file, "%-17s: %-9s\n", "err_code",
                  g_hdcp_err_code[index / EACH_CATEGORY_TYPE_NUM][index % EACH_CATEGORY_TYPE_NUM]);
    /* line 5 */
    HI_PROC_PRINT(file, "%-17s:", "recv_id");
    for (index = 0; index < HDCP2X_RECVID_SIZE; index++) {
        HI_PROC_PRINT(file, " %u", hdcp2x_sink.recv_id[index]);
    }
    HI_PROC_PRINT(file, "\n");
}

static hi_s32 proc_aovo_read(hi_void *file, hi_void *private)
{
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    proc_ver_info(file, hdmi);
    proc_port_info(file, hdmi);
    proc_video_attr_info(file, hdmi); /* video attr */
    proc_video_avi_infoframe(file, hdmi); /* avi infoframe */
    proc_video_path_info(file, hdmi); /* video path */
    proc_video_vsif_info(file, hdmi); /* vsif */
    proc_audio_attr_info(file, hdmi); /* audio attr */
    proc_audio_path_info(file, hdmi); /* audio path */
    proc_audio_infoframe(file, hdmi); /* audio infoframe */

    return 0;
}

static hi_s32 proc_hdmitx_read(hi_void *file, hi_void *private)
{
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    proc_ver_info(file, hdmi);
    proc_port_info(file, hdmi);
    proc_soft_status_info(file, hdmi);
    proc_hw_status_info(file, hdmi);
    proc_hw_cfg_info(file, hdmi);
    proc_detail_timing_info(file, hdmi);
    proc_frl_all_info(file, hdmi);
    proc_cec_info(file, hdmi);
    proc_hdcp_info(file, hdmi);

    return 0;
}

static hi_s32 proc_dsc_read(hi_void *file, hi_void *private)
{
    struct hisilicon_hdmi *hdmi = (struct hisilicon_hdmi *)private;

    proc_ver_info(file, hdmi);
    proc_port_info(file, hdmi);
    proc_dsc_cfg_info(file, hdmi);
    proc_dsc_para_input_info(file, hdmi);
    proc_dsc_timing_info(file, hdmi);
    proc_dsc_caps_info(file, hdmi);
    proc_dsc_status_info(file, hdmi);
    proc_dsc_em_data_info(file, hdmi);

    return 0;
}

hi_s32 drv_hdmitx_proc_init(struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_char buf[16]; /* proc file name buff size 16 */
    osal_proc_entry *item = HI_NULL;
    cmd_msg *debug_cmd_msg = drv_hdmitx_debug_get_cmd_list();

    if (hdmi == HI_NULL) {
        HDMI_ERR("input params is null!\n");
        return -EINVAL;
    }

    /* create hdmi proc */
    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return ret;
    }

    item = osal_proc_add(buf, sizeof(buf));
    if (item == HI_NULL) {
        HDMI_ERR("create hdmitx%d proc entry fail!\n", hdmi->id);
        return -ENOMEM;
    }

    /* set functions */
    item->read = proc_hdmitx_read;
    item->cmd_list = debug_cmd_msg->cmd_list;
    item->cmd_cnt = debug_cmd_msg->cmd_cnt;
    item->private = (hi_void *)hdmi;

    /* create hdmi_aovo proc */
    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d_aovo", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return ret;
    }

    item = osal_proc_add(buf, sizeof(buf));
    if (item == HI_NULL) {
        HDMI_ERR("create hdmitx%d_aovo proc entry fail!\n", hdmi->id);
        return -ENOMEM;
    }

    /* set functions */
    item->read = proc_aovo_read;
    item->cmd_list = HI_NULL;
    item->private = (hi_void *)hdmi;

    /* create hdmi_dsc proc */
    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d_dsc", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return ret;
    }

    item = osal_proc_add(buf, sizeof(buf));
    if (item == HI_NULL) {
        HDMI_ERR("create hdmi%d_dsc proc entry fail!\n", hdmi->id);
        return -ENOMEM;
    }

    /* set functions */
    item->read = proc_dsc_read;
    item->cmd_list = HI_NULL;
    item->private = (hi_void *)hdmi;

    /* create hdmi_sink proc */
    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d_sink", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return ret;
    }

    item = osal_proc_add(buf, sizeof(buf));
    if (item == HI_NULL) {
        HDMI_ERR("create hdmitx%d_sink proc entry fail!\n", hdmi->id);
        return -ENOMEM;
    }

    /* set functions */
    item->read = proc_sink_read;
    item->cmd_list = HI_NULL;
    item->private = (hi_void *)hdmi;

    return 0;
}

hi_void drv_hdmitx_proc_deinit(struct hisilicon_hdmi *hdmi)
{
    hi_s32 ret;
    hi_char buf[16]; /* proc file name buff size 16 */

    if (hdmi == HI_NULL) {
        HDMI_ERR("input params is null!\n");
        return;
    }

    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return;
    }
    osal_proc_remove(buf, sizeof(buf));

    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d_aovo", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return;
    }
    osal_proc_remove(buf, sizeof(buf));

    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d_dsc", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return;
    }
    osal_proc_remove(buf, sizeof(buf));

    if (memset_s(buf, sizeof(buf), 0, sizeof(buf))) {
        HDMI_ERR("memset_s err\n");
        return;
    }
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "hdmitx%d_sink", hdmi->id);
    if (ret < 0) {
        HDMI_ERR("snprintf_s err\n");
        return;
    }
    osal_proc_remove(buf, sizeof(buf));
}


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq common api for other module
 * Author: shijiaoling
 * Create: 2019-7-1
 */

#include <linux/uaccess.h>
#include "drv_pq.h"
#include "hi_osal.h"
#include "drv_pq_vpss_thread.h"

#define PQ_MODIFY_TIME "(20191116)"
#define PQ_ZME_DRAW_MODE_MAX 10
#define PQ_ARG_LEN_MAX       8
#define PQ_ARG_FMT_10        10
#define PQ_ARG_FMT_16        16
#define PQ_ZME_FILE_TYPE_32  32
#define PQ_ZME_FILE_TYPE_22  22
#define PQ_OFFSET_MAX        512
#define PQ_OFFSET_MIN        (-512)
#define PQ_PROC_NAME_LENGTH 16

static pq_proc_print_level g_en_pq_proc_print_level = PQ_PROC_PRINT_LEVEL_ORI;

static hi_s32 drv_pq_proc_print_help(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_brightness(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_contrast(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_hue(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_saturation(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_demo_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_color_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_flesh_tone(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_dei_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_fod_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_print_bin(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_print_alg_bin(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_bin(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_hdrcfg(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_print_type(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_scene_change(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_cfg_default(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_mc_only(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_hdcp(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_image_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_color_temp(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_hdr_offset(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_gfx_hdr_offset(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_hdr_tm(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_gfx_hdr_tm(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_zme_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_gfxzme_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_gfxzme_offset(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_alg_ctrl(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_vpmode(hi_drv_pq_vp_type vp_type, hi_char *arg2);
static hi_s32 drv_pq_proc_set_vptype_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_proc_level_ctrl(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_vdp_zme_strategy(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_4ksr_scale_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);
static hi_s32 drv_pq_proc_set_8ksr_scale_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private);

static osal_proc_cmd g_pq_proc_info[] = {
    { "help", drv_pq_proc_print_help },
    { "bright", drv_pq_proc_set_brightness },
    { "contrast", drv_pq_proc_set_contrast },
    { "hue", drv_pq_proc_set_hue },
    { "satu", drv_pq_proc_set_saturation },
    { "demomode", drv_pq_proc_set_demo_mode },
    { "colormode", drv_pq_proc_color_mode },
    { "fleshtone", drv_pq_proc_set_flesh_tone },
    { "deimode", drv_pq_proc_set_dei_mode },
    { "fodmode", drv_pq_proc_set_fod_mode },
    { "printbin", drv_pq_proc_set_print_bin },
    { "printalgbin", drv_pq_proc_set_print_alg_bin },
    { "setbin", drv_pq_proc_set_bin },
    { "sethdrcfg", drv_pq_proc_set_hdrcfg },
    { "printtype", drv_pq_proc_set_print_type },
    { "scenechange", drv_pq_proc_set_scene_change },
    { "scd", drv_pq_proc_set_scene_change },
    { "cfgdefault", drv_pq_proc_set_cfg_default },
    { "mc_only", drv_pq_proc_set_mc_only },
    { "hdcp", drv_pq_proc_set_hdcp },
    { "imagemode", drv_pq_proc_set_image_mode },
    { "preview", drv_pq_proc_set_vptype_mode },
    { "remote", drv_pq_proc_set_vptype_mode },
    { "colortemp", drv_pq_proc_set_color_temp },
    { "hdroffset", drv_pq_proc_set_hdr_offset },
    { "gfxhdroffset", drv_pq_proc_set_gfx_hdr_offset },
    { "hdrtm", drv_pq_proc_set_hdr_tm },
    { "gfxhdrtm", drv_pq_proc_set_gfx_hdr_tm },
    { "zme", drv_pq_proc_set_zme_mode },
    { "gfxzme", drv_pq_proc_set_gfxzme_mode },
    { "gfxzmeoffset", drv_pq_proc_set_gfxzme_offset },
    { "fmd", drv_pq_proc_set_alg_ctrl },
    { "tnr", drv_pq_proc_set_alg_ctrl },
    { "snr", drv_pq_proc_set_alg_ctrl },
    { "db", drv_pq_proc_set_alg_ctrl },
    { "vdp4ksnr", drv_pq_proc_set_alg_ctrl },
    { "vpsshdr", drv_pq_proc_set_alg_ctrl },
    { "dr", drv_pq_proc_set_alg_ctrl },
    { "dm", drv_pq_proc_set_alg_ctrl },
    { "sharp", drv_pq_proc_set_alg_ctrl },
    { "dci", drv_pq_proc_set_alg_ctrl },
    { "sr", drv_pq_proc_set_alg_ctrl },
    { "acm", drv_pq_proc_set_alg_ctrl },
    { "csc", drv_pq_proc_set_alg_ctrl },
    { "dei", drv_pq_proc_set_alg_ctrl },
    { "ds", drv_pq_proc_set_alg_ctrl },
    { "artds", drv_pq_proc_set_alg_ctrl },
    { "gfxcsc", drv_pq_proc_set_alg_ctrl },
    { "gfxzme", drv_pq_proc_set_alg_ctrl },
    { "cle", drv_pq_proc_set_alg_ctrl },
    { "all", drv_pq_proc_set_alg_ctrl },
    { "proc", drv_pq_proc_set_proc_level_ctrl },

    { "vdpzmestrategy", drv_pq_proc_set_vdp_zme_strategy },

    { "4ksr_scale_mode", drv_pq_proc_set_4ksr_scale_mode },
    { "8ksr_scale_mode", drv_pq_proc_set_8ksr_scale_mode },
};

typedef enum {
    PRN_TABLE_ALL = 0,
    PRN_TABLE_MULTI = 1,
    PRN_TABLE_SINGLE = 2,
    PRN_TABLE_ALG = 3,
    PRN_TABLE_SET_DEFAULT = 4,

    PRN_TABLE_MAX,
} pq_prn_table_type;

hi_char *g_pq_intf_str[PQ_INTF_MODE_MAX] = {
    "PQ_UPDATE_VDP_ALG",
    "PQ_UPDATE_VDP_STT",
    "PQ_SET_VDP_HDR",
    "PQ_GET_VDP_ZME",
    "PQ_GET_VDP_CSC",

    "PQ_INIT_VPSS_ALG",
    "PQ_UPDATE_VPSS_STT",
    "PQ_UPDATE_VPSS_ALG",
    "PQ_GET_VPSS_ZME",
    "PQ_SET_VPSS_HDR",

    "PQ_GET_GFX_CSC",
    "PQ_GET_GFX_ZME",
    "PQ_SET_GFX_HDR",
    "PQ_GET_HWC_HDR",

    "PQ_UPDATE_VDP_AI",
    "PQ_UPDATE_VPSS_AI",

    "PQ_UPDATE_NR_STT",
    "PQ_UPDATE_DEI_STT",
    "PQ_UPDATE_DB_STT",
    "PQ_UPDATE_DM_STT",

    "PQ_UPDATE_NR_ALG",
    "PQ_UPDATE_DEI_ALG",
    "PQ_UPDATE_DB_ALG",
    "PQ_UPDATE_DM_ALG",
};

static hi_s32 pq_osal_cmd_check(hi_u32 in_argc, hi_u32 aspect_argc, hi_void *arg, hi_void *private)
{
    if (in_argc != aspect_argc) {
        osal_printk("in_argc is %d, aspect_argc is %d\n", in_argc, aspect_argc);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef PQ_PROC_CTRL_SUPPORT
static hi_s32 drv_pq_proc_read_acm(struct seq_file *s)
{
#ifdef PQ_ALG_ACM
    hi_pq_color_spec_mode proc_color_spec_mode = HI_PQ_COLOR_MODE_RECOMMEND;
    hi_u32 flesh_str = 0;
    hi_pq_six_base_color six_base_color;

    hi_u8 *proc_enhance_mode[HI_PQ_COLOR_MODE_MAX] = {
        "optimal",
        "blue",
        "green",
        "bg",
        "original"
    };

    hi_u8 *proc_flesh_tone[HI_PQ_FLESHTONE_GAIN_MAX] = {
        "off",
        "low",
        "middle",
        "high"
    };

    drv_pq_get_color_enhance_mode(&proc_color_spec_mode);
    drv_pq_get_flesh_tone_level(&flesh_str);
    drv_pq_get_six_base_color(&six_base_color);

    PQ_CHECK_OVER_RANGE_RE_FAIL(proc_color_spec_mode, HI_PQ_COLOR_MODE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(flesh_str, HI_PQ_FLESHTONE_GAIN_MAX);
    if (six_base_color.red > PQ_ALG_MAX_VALUE || six_base_color.green > PQ_ALG_MAX_VALUE ||
       six_base_color.blue > PQ_ALG_MAX_VALUE || six_base_color.cyan > PQ_ALG_MAX_VALUE ||
       six_base_color.magenta > PQ_ALG_MAX_VALUE || six_base_color.yellow > PQ_ALG_MAX_VALUE) {
        HI_ERR_PQ("six base color over range!\n");
        return HI_FAILURE;
    }

    osal_proc_print(s, "%-20s: %-20s", "enhance mode", proc_enhance_mode[proc_color_spec_mode]);
    osal_proc_print(s, "%-20s: %s\n", "flesh tone level", proc_flesh_tone[flesh_str]);
    osal_proc_print(s, "%-20s: %d %d %d %d %d %d\n", "six base color",
        six_base_color.red, six_base_color.green, six_base_color.blue,
        six_base_color.cyan, six_base_color.magenta, six_base_color.yellow);

#endif

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_read_csc(struct seq_file *s)
{
    return HI_SUCCESS;
}

static pq_gfx_csc_proc_info g_gfx_csc = {0};
static hi_s32 drv_pq_proc_read_gfxcsc(struct seq_file *s)
{
#ifdef PQ_ALG_GFXCSC

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_info)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_info(&g_gfx_csc);
    }

    osal_proc_print(s, "%-10s: %-40s\n", "gfxcsc layer",
        (g_gfx_csc.gfx_layer == HI_DRV_PQ_GFX_LAYER_GP0) ? "GP0" : "G3");
    osal_proc_print(s, "%-10s: %-20s", "gfxcsc enable", g_gfx_csc.csc_en ? "on" : "off");
    osal_proc_print(s, "%-10s: %u\n", "gfxcsc input mode", g_gfx_csc.csc_in);
    osal_proc_print(s, "%-10s: %u\n", "gfxcsc output mode", g_gfx_csc.csc_out);
#endif

    return HI_SUCCESS;
}

hi_char *g_zme_node_str[HI_DRV_PQ_NODE_MAX] = {
    "V0_4KZME",
    "V0_4KSR",
    "V0_8KSR",
    "V0_8KZME",
};

static hi_s32 drv_pq_proc_read_zme_base_info(struct seq_file *s, hi_pq_proc_get_vdpzme_strategy proc_vdp_zme_strategy)
{
    hi_drv_pq_hd_zme_strategy_in hd_zme_in = { 0 };
    hi_drv_pq_hd_zme_strategy_out hd_zme_out = { 0 };
    pq_common_status pq_status = { 0 };

    drv_pq_get_pq_status(&pq_status);
    hd_zme_in = proc_vdp_zme_strategy.zme_in.hd_zme_strategy;
    hd_zme_out = proc_vdp_zme_strategy.zme_out.hd_zme_strategy;

    osal_proc_print(s, "%-20s %-20d\n", "zme layer :", proc_vdp_zme_strategy.layer_id);
    osal_proc_print(s, "%-20s %-20d", "w_in :", hd_zme_in.zme_common_info.zme_w_in);
    osal_proc_print(s, "%-20s %-20d \n", "h_in :", hd_zme_in.zme_common_info.zme_h_in);
    osal_proc_print(s, "%-20s %-20d", "w_out :", hd_zme_in.zme_common_info.zme_w_out);
    osal_proc_print(s, "%-20s  %-20d \n", "h_out :", hd_zme_in.zme_common_info.zme_h_out);

    osal_proc_print(s, "%-20s  %-20s \n", "equal ratio :", proc_vdp_zme_strategy.is_equal_ratio ? "yes" : "no");

    osal_proc_print(s, "intf_h / video_oh : %-8d / %-8d \n",
        pq_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_h,
        hd_zme_in.zme_common_info.zme_h_out);
    osal_proc_print(s, "video_ih / video_oh : %-8d / %-8d \n",
        hd_zme_in.zme_common_info.zme_h_in, hd_zme_in.zme_common_info.zme_h_out);

    osal_proc_print(s, "intf_w / video_ow : %-8d / %-8d \n",
        pq_status.timing_info[HI_PQ_DISPLAY_1].fmt_rect.rect_w,
        hd_zme_in.zme_common_info.zme_w_out);
    osal_proc_print(s, "max(video_iw / video_ow, video_ih / video_oh) : max(%d / %d,%d / %d) \n",
        hd_zme_in.zme_common_info.zme_w_in, hd_zme_in.zme_common_info.zme_w_out,
        hd_zme_in.zme_common_info.zme_h_in, hd_zme_in.zme_common_info.zme_h_out);

    osal_proc_print(s, "%-20s %-20d \n", "horizontal draw mode :", hd_zme_out.hor_draw_mul);
    osal_proc_print(s, "%-20s %-20d \n", "vertical   draw mode :", hd_zme_out.ver_draw_mul);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_read_all_zme_info(struct seq_file *s, hi_pq_proc_get_vdpzme_strategy proc_vdp_zme_strategy)
{
    hi_drv_pq_v0_zme_node zme_node = 0;
    hi_drv_pq_hd_zme_strategy_in hd_zme_in = { 0 };
    hi_drv_pq_hd_zme_strategy_out hd_zme_out = { 0 };

    hd_zme_in = proc_vdp_zme_strategy.zme_in.hd_zme_strategy;
    hd_zme_out = proc_vdp_zme_strategy.zme_out.hd_zme_strategy;

    for (zme_node = HI_DRV_PQ_4KZME; zme_node < HI_DRV_PQ_NODE_MAX; zme_node++) {
        osal_proc_print(s, "----------------- %s ------------------\n", g_zme_node_str[zme_node]);
        osal_proc_print(s, "%-20s  %-20d\n", "zme_enable :", hd_zme_out.zme_fmt[zme_node].zme_enable);

        if (zme_node == HI_DRV_PQ_8KZME) {
            osal_proc_print(s, "%-20s  %-20d\n", "8kzme band pos :", hd_zme_out.zme_fmt[zme_node].zme_point);
        }

        osal_proc_print(s, "%-20s  %-20d", "zme_w_in :", hd_zme_out.zme_fmt[zme_node].zme_w_in);
        osal_proc_print(s, "%-20s  %-20d\n", "zme_h_in :", hd_zme_out.zme_fmt[zme_node].zme_h_in);
        osal_proc_print(s, "%-20s  %-20d", "zme_w_out :", hd_zme_out.zme_fmt[zme_node].zme_w_out);
        osal_proc_print(s, "%-20s  %-20d\n", "zme_h_out :", hd_zme_out.zme_fmt[zme_node].zme_h_out);

        osal_proc_print(s, "%-20s  %-20s", "zme_fmt_in :",
            (hd_zme_out.zme_fmt[zme_node].zme_fmt_in == HI_DRV_PQ_ZME_FMT_420) ? "420" :
            ((hd_zme_out.zme_fmt[zme_node].zme_fmt_in == HI_DRV_PQ_ZME_FMT_422) ? "422" : "444"));
        osal_proc_print(s, "%-20s  %-20s\n", "zme_fmt_out :",
            (hd_zme_out.zme_fmt[zme_node].zme_fmt_out == HI_DRV_PQ_ZME_FMT_420) ? "420" :
            ((hd_zme_out.zme_fmt[zme_node].zme_fmt_in == HI_DRV_PQ_ZME_FMT_422) ? "422" : "444"));
        osal_proc_print(s, "%-20s  %-20s", "frame_fmt_in :",
            (hd_zme_out.zme_fmt[zme_node].frame_fmt_in == HI_DRV_PQ_FRM_FRAME) ? "frame" : "field");
        osal_proc_print(s, "%-20s  %-20s\n", "frame_fmt_out :",
            (hd_zme_out.zme_fmt[zme_node].frame_fmt_out == HI_DRV_PQ_FRM_FRAME) ? "frame" : "field");

        osal_proc_print(s, "%-20s  %-20d", "zme_fir_hl :", hd_zme_out.zme_fmt[zme_node].zme_fir_mode.zme_fir_hl);
        osal_proc_print(s, "%-20s  %-20d\n", "zme_fir_hc :", hd_zme_out.zme_fmt[zme_node].zme_fir_mode.zme_fir_hc);
        osal_proc_print(s, "%-20s  %-20d", "zme_fir_vl :", hd_zme_out.zme_fmt[zme_node].zme_fir_mode.zme_fir_vl);
        osal_proc_print(s, "%-20s  %-20d\n", "zme_fir_vc :", hd_zme_out.zme_fmt[zme_node].zme_fir_mode.zme_fir_vc);

        osal_proc_print(s, "%-20s %-20d", "zme_offset_hl :", hd_zme_out.zme_fmt[zme_node].zme_offset.zme_offset_hl);
        osal_proc_print(s, "%-20s %-20d\n", "zme_offset_hc :", hd_zme_out.zme_fmt[zme_node].zme_offset.zme_offset_hc);
        osal_proc_print(s, "%-20s %-20d", "zme_offset_vl :", hd_zme_out.zme_fmt[zme_node].zme_offset.zme_offset_vl);
        osal_proc_print(s, "%-20s %-20d\n", "zme_offset_vc :", hd_zme_out.zme_fmt[zme_node].zme_offset.zme_offset_vc);
        osal_proc_print(s, "%-20s %-20d", "zme_offset_vc_btm :",
            hd_zme_out.zme_fmt[zme_node].zme_offset.zme_offset_vc_btm);
        osal_proc_print(s, "%-20s %-20d\n", "zme_offset_vl_btm :",
            hd_zme_out.zme_fmt[zme_node].zme_offset.zme_offset_vl_btm);
    }

    return HI_SUCCESS;
}

hi_pq_proc_get_vdpzme_strategy g_proc_vdp_zme_strategy = {0};
static hi_s32 drv_pq_proc_read_zme(struct seq_file *s)
{
#ifdef PQ_ALG_ZME

    osal_proc_print(s, "--------------------------- %s ----------------------\n", "vdp v0 zme reso");

    if (GET_ALG_FUN(HI_PQ_MODULE_ZME)->get_proc_vdp_zme_strategy) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->get_proc_vdp_zme_strategy(&g_proc_vdp_zme_strategy);
        drv_pq_proc_read_zme_base_info(s, g_proc_vdp_zme_strategy);
        drv_pq_proc_read_all_zme_info(s, g_proc_vdp_zme_strategy);
    }
#endif

    return HI_SUCCESS;
}
static gfx_zme_strategy_proc g_proc_gfxzme_para = {0};
static hi_s32 drv_pq_proc_read_gfxzme(struct seq_file *s)
{
#ifdef PQ_ALG_GFXZME
    hi_s32 ret = HI_SUCCESS;

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, get_strategy_by_proc)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->get_strategy_by_proc(&g_proc_gfxzme_para);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    osal_proc_print(s, "------------------------------------ %s ------------------------------------\n",
        "gfxzme");

    osal_proc_print(s, "%-20s: %-20s", "gfxzme layer",
        (g_proc_gfxzme_para.gfx_layer == HI_DRV_PQ_GFX_LAYER_GP0) ? "GP0" : "G3");
    osal_proc_print(s, "%-20s: %-20s\n", "gfxzme mode",
        (g_proc_gfxzme_para.gfx_zme_out.zme_fir_mode.zme_fir_h == HI_DRV_PQ_ZME_FIR) ? "fir" : "copy");
    osal_proc_print(s, "%-20s: %u * %-15u", "gfxzme input",
        g_proc_gfxzme_para.gfx_zme_out.zme_width_in, g_proc_gfxzme_para.gfx_zme_out.zme_height_in);
    osal_proc_print(s, "%-20s:  %u * %-10u\n", "gfxzme output",
        g_proc_gfxzme_para.gfx_zme_out.zme_width_out, g_proc_gfxzme_para.gfx_zme_out.zme_height_out);
    osal_proc_print(s, "%-20s: %-20d", "gfxzme hl offset", g_proc_gfxzme_para.gfx_zme_out.zme_offset.zme_offset_hl);
    osal_proc_print(s, "%-20s: %-20d\n", "gfxzme hc offset", g_proc_gfxzme_para.gfx_zme_out.zme_offset.zme_offset_hc);
    osal_proc_print(s, "%-20s: %-20d", "gfxzme vtp offset", g_proc_gfxzme_para.gfx_zme_out.zme_offset.zme_offset_v_tp);
    osal_proc_print(s, "%-20s: %-20d\n", "gfxzme vbtm offset",
        g_proc_gfxzme_para.gfx_zme_out.zme_offset.zme_offset_v_btm);
    osal_proc_print(s, "%-20s: %-20d", "gfxzme ratio_h",
        (1 << 20) / g_proc_gfxzme_para.gfx_zme_out.zme_ratio.zme_ratio_h); /* 20: precision */
    osal_proc_print(s, "%-20s: %-20d\n", "gfxzme ratio_v",
        (1 << 12) / g_proc_gfxzme_para.gfx_zme_out.zme_ratio.zme_ratio_v); /* 12 precision */

#endif

    return ret;
}

static hi_s32 drv_pq_proc_read_fmd(struct seq_file *s)
{
#ifdef PQ_ALG_FMD
    hi_u32 field_order = 0;

    if (GET_ALG_FUN(HI_PQ_MODULE_FMD)->get_fod_dect_info) {
        GET_ALG_FUN(HI_PQ_MODULE_FMD)->get_fod_dect_info(&field_order);
        if (field_order == 0) {
            osal_proc_print(s, "%-20s: %-20s", "real field order", "top first");
        } else if (field_order == 1) {
            osal_proc_print(s, "%-20s: %-20s", "real field order", "bottom first");
        } else {
            osal_proc_print(s, "%-20s: %-20s", "real field order", "unkown");
        }
    }

    if (GET_ALG_FUN(HI_PQ_MODULE_FMD)->get_force_fod_mode) {
        GET_ALG_FUN(HI_PQ_MODULE_FMD)->get_force_fod_mode(&field_order);
        if (field_order == 0) {
            osal_proc_print(s, "%-20s: %-20s\n", "force field order", "top first");
        } else if (field_order == 1) {
            osal_proc_print(s, "%-20s: %-20s\n", "force field order", "bottom first");
        } else {
            osal_proc_print(s, "%-20s: %-20s\n", "force field order", "unkown");
        }
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_read_dei(struct seq_file *s)
{
    hi_bool mc_only_en = 0;
    hi_bool ma_only_en = 0;

    if (GET_ALG_FUN(HI_PQ_MODULE_DEI)->get_mc_only_enable) {
        GET_ALG_FUN(HI_PQ_MODULE_DEI)->get_mc_only_enable(&mc_only_en);
        if (mc_only_en == HI_TRUE) {
            osal_proc_print(s, "%-20s: %-20s", "mc_only", "on");
        } else {
            osal_proc_print(s, "%-20s: %-20s", "mc_only", "off");
        }
    }

    if (GET_ALG_FUN(HI_PQ_MODULE_DEI)->get_ma_only_enable) {
        GET_ALG_FUN(HI_PQ_MODULE_DEI)->get_ma_only_enable(&ma_only_en);
        if (ma_only_en == HI_TRUE) {
            osal_proc_print(s, "%-20s: %-20s\n", "ma_only", "on");
        } else {
            osal_proc_print(s, "%-20s: %-20s\n", "ma_only", "off");
        }
    }

    return HI_SUCCESS;
}

const char *g_hdr_type_str[HI_DRV_HDR_TYPE_MAX] = {
    "SDR",
    "HDR10",
    "HLG",
    "CUVA",
    "JTP_SL_HDR",
    "HDR10PLUS",
    "DOLBYVISION",
};

const char *g_csc_primary_str[HI_DRV_CS_MAX] = {
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
    "MAX",
};

const char *g_csc_space_str[HI_DRV_CS_MAX] = {
    "YUV",
    "RGB",
    "MAX",
};

const char *g_csc_range_str[HI_DRV_CS_MAX] = {
    "LIMITED",
    "FULL",
    "MAX",
};

const char *g_csc_matrix_str[HI_DRV_CS_MAX] = {
    "IDENTITY = 0",
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
    "MAX",
};

const char *g_csc_type_str[HI_DRV_CS_MAX] = {
    "HI_DRV_CS_UNKNOWN",
    "HI_DRV_CS_DEFAULT",
    "HI_DRV_CS_BT601_YUV_LIMITED",
    "HI_DRV_CS_BT601_YUV_FULL",
    "HI_DRV_CS_BT601_RGB_LIMITED",
    "HI_DRV_CS_BT601_RGB_FULL",
    "HI_DRV_CS_NTSC1953",
    "HI_DRV_CS_BT470_SYSTEM_M",
    "HI_DRV_CS_BT470_SYSTEM_BG",
    "HI_DRV_CS_BT709_YUV_LIMITED",
    "HI_DRV_CS_BT709_YUV_FULL",
    "HI_DRV_CS_BT709_RGB_LIMITED",
    "HI_DRV_CS_BT709_RGB_FULL",
    "HI_DRV_CS_BT2020_YUV_LIMITED",
    "HI_DRV_CS_BT2020_YUV_FULL",
    "HI_DRV_CS_BT2020_RGB_LIMITED",
    "HI_DRV_CS_BT2020_RGB_FULL",
    "HI_DRV_CS_REC709",
    "HI_DRV_CS_SMPT170M",
    "HI_DRV_CS_SMPT240M",
    "HI_DRV_CS_BT878",
    "HI_DRV_CS_XVYCC",
    "HI_DRV_CS_JPEG",
};

static hi_s32 drv_pq_proc_read_hdr(struct seq_file *s)
{
#ifdef PQ_ALG_HDR
    pq_hdr_cfg_proc hdr_cfg = {0};
    hi_s32 ret;

    if (GET_ALG_FUN(HI_PQ_MODULE_HDR)->get_hdr_cfg_by_proc) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->get_hdr_cfg_by_proc(&hdr_cfg);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    osal_proc_print(s, "%-20s : %-20s\n", "layer id", (hdr_cfg.layer_id == HI_DRV_PQ_XDR_LAYER_ID_0) ? "V0" : "V1");
    osal_proc_print(s, "%-20s : %-10s %-10s %-10s %-10s %-10s\n", "src", g_hdr_type_str[hdr_cfg.src_hdr_type],
                    g_csc_primary_str[hdr_cfg.color_space_in.color_primary],
                    g_csc_space_str[hdr_cfg.color_space_in.color_space],
                    g_csc_range_str[hdr_cfg.color_space_in.quantify_range],
                    g_csc_matrix_str[hdr_cfg.color_space_in.matrix_coef]);
    osal_proc_print(s, "%-20s : %-10s %-10s %-10s %-10s %-10s\n", "disp", g_hdr_type_str[hdr_cfg.disp_hdr_type],
                    g_csc_primary_str[hdr_cfg.color_space_out.color_primary],
                    g_csc_space_str[hdr_cfg.color_space_out.color_space],
                    g_csc_range_str[hdr_cfg.color_space_out.quantify_range],
                    g_csc_matrix_str[hdr_cfg.color_space_out.matrix_coef]);
    osal_proc_print(s, "%-20s : %-20s %-20s : %-20s\n",
        "hdrv1_cm", (hdr_cfg.hdrv1_cm_en == HI_TRUE) ? "on" : "off",
        "imap", (hdr_cfg.imap_en == HI_TRUE) ? "on" : "off");
    osal_proc_print(s, "%-20s : %-20s %-20s : %-20s\n",
        "tmapv2", (hdr_cfg.tmapv2_en == HI_TRUE) ? "on" : "off",
        "omap", (hdr_cfg.omap_en == HI_TRUE) ? "on" : "off");
#endif

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_read_cle(struct seq_file *s)
{
#ifdef PQ_ALG_CLE
    pq_cle_cfg_proc cle_proc = { 0 };
    hi_s32 ret;

    if (GET_ALG_FUN(HI_PQ_MODULE_CLE)->get_cle_cfg_by_proc) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->get_cle_cfg_by_proc(&cle_proc);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }
    osal_proc_print(s, "4K-CLE video iw / ih : %-10d / %-10d \n",
        cle_proc.cle_cfg_proc[CLE_ID_V0_4K].video_in_width,
        cle_proc.cle_cfg_proc[CLE_ID_V0_4K].video_in_height);
    osal_proc_print(s, "4K-CLE video ow / oh : %-10d / %-10d \n",
        cle_proc.cle_cfg_proc[CLE_ID_V0_4K].video_out_width,
        cle_proc.cle_cfg_proc[CLE_ID_V0_4K].video_out_height);
    osal_proc_print(s, "%-20s : %-20d\n", "4K-CLE MIN scale", cle_proc.cle_min_scale[CLE_ID_V0_4K]);

    osal_proc_print(s, "4K-CLE cle in w,h    : %-10d,%-10d \n",
        cle_proc.cle_cfg_proc[CLE_ID_V0_4K].cle_in_width, cle_proc.cle_cfg_proc[CLE_ID_V0_4K].cle_in_height);
    osal_proc_print(s, "%-20s : %-20s \n",
        "4K CLE DFX enable", (cle_proc.cle_enable[CLE_ID_V0_4K] == HI_TRUE) ? "on" : "off");
    osal_proc_print(s, "%-20s : %-20s %-20s : %-20s \n",
        "4K CLE", (cle_proc.cle_cfg_proc[CLE_ID_V0_4K].cle_enable == HI_TRUE) ? "on" : "off",
        "4K CLM", (cle_proc.cle_cfg_proc[CLE_ID_V0_4K].clm_enable == HI_TRUE) ? "on" : "off");

    osal_proc_print(s, "8K-CLE video iw / ih : %-10d / %-10d \n",
        cle_proc.cle_cfg_proc[CLE_ID_V0_8K].video_in_width,
        cle_proc.cle_cfg_proc[CLE_ID_V0_8K].video_in_height);
    osal_proc_print(s, "8K-CLE video ow / oh : %-10d / %-10d \n",
        cle_proc.cle_cfg_proc[CLE_ID_V0_8K].video_out_width,
        cle_proc.cle_cfg_proc[CLE_ID_V0_8K].video_out_height);
    osal_proc_print(s, "%-20s : %-20d\n", "8K-CLE MIN scale", cle_proc.cle_min_scale[CLE_ID_V0_8K]);

    osal_proc_print(s, "8K-CLE cle in w,h    : %-10d , %-10d \n",
        cle_proc.cle_cfg_proc[CLE_ID_V0_8K].cle_in_width, cle_proc.cle_cfg_proc[CLE_ID_V0_8K].cle_in_height);
    osal_proc_print(s, "%-20s : %-20s \n",
        "8K CLE DFX enable", (cle_proc.cle_enable[CLE_ID_V0_8K] == HI_TRUE) ? "on" : "off");
    osal_proc_print(s, "%-20s : %-20s %-20s : %-20s \n",
        "8K CLE", (cle_proc.cle_cfg_proc[CLE_ID_V0_8K].cle_enable == HI_TRUE) ? "on" : "off",
        "8K CLM", (cle_proc.cle_cfg_proc[CLE_ID_V0_8K].clm_enable == HI_TRUE) ? "on" : "off");

    return HI_SUCCESS;
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_read_sharpen(struct seq_file *s)
{
#ifdef PQ_ALG_SHARPEN
    pq_sharp_cfg_proc sharp_proc = { 0 };
    hi_s32 ret;

    if (GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->get_sharp_proc_info) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->get_sharp_proc_info(&sharp_proc);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }
    osal_proc_print(s, "sharpen video iw / ih : %-10d / %-10d \n",
        sharp_proc.sharp_reso.video_in_width, sharp_proc.sharp_reso.video_in_height);
    osal_proc_print(s, "sharpen video ow / oh : %-10d / %-10d \n",
        sharp_proc.sharp_reso.video_out_width, sharp_proc.sharp_reso.video_out_height);
    osal_proc_print(s, "%-20s  : %-20d\n", "sharpen MIN scale", sharp_proc.sharp_min_scale_ratio);

    osal_proc_print(s, "sharpen module in w,h : %-10d,%-10d \n",
        sharp_proc.sharp_reso.sharpen_in_width, sharp_proc.sharp_reso.sharpen_in_height);
    osal_proc_print(s, "%-20s : %-20s \n", "sharpen enable", (sharp_proc.sharp_enable == HI_TRUE) ? "on" : "off");
#endif
    return HI_SUCCESS;
}

#endif

static hi_s32 drv_pq_proc_read_vpsshdr(struct seq_file *s)
{
#ifdef PQ_ALG_VPSSHDR
    pq_vpsshdr_cfg_proc vpsshdr_proc = { 0 };
    hi_u32 handle;
    hi_s32 ret;

    for (handle = 0; handle < VPSS_HANDLE_NUM; handle++) {
        if (GET_ALG_FUN(HI_PQ_MODULE_VPSSHDR)->get_vpsshdr_cfg_by_proc) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_VPSSHDR)->get_vpsshdr_cfg_by_proc(handle, &vpsshdr_proc);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }

        if (vpsshdr_proc.enable == HI_TRUE) {
            osal_proc_print(s, "%-20s : %-20d\n", "handle id", handle);
            osal_proc_print(s, "%-20s : %-20s %-20s\n", "src",
                            g_hdr_type_str[vpsshdr_proc.src_hdr_type], g_csc_type_str[vpsshdr_proc.color_space_in]);
            osal_proc_print(s, "%-20s : %-20s %-20s\n", "disp",
                            g_hdr_type_str[vpsshdr_proc.disp_hdr_type], g_csc_type_str[vpsshdr_proc.color_space_out]);
            osal_proc_print(s, "%-20s : %-20s %-20s : %-20s\n",
                            "hdrv1_cm", (vpsshdr_proc.hdrv1_cm_en == HI_TRUE) ? "on" : "off",
                            "imap", (vpsshdr_proc.imap_en == HI_TRUE) ? "on" : "off");
            osal_proc_print(s, "%-20s : %-20s\n",
                            "tmapv2", (vpsshdr_proc.tmapv2_en == HI_TRUE) ? "on" : "off");
        }
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_read_dci(struct seq_file *s)
{
#ifdef PQ_ALG_DCI
    pq_dci_cfg_proc dci_proc = { 0 };
    hi_s32 ret;

    if (GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_dci_proc_info) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_dci_proc_info(&dci_proc);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    osal_proc_print(s, "%-20s: %-20s \n", "dci link type",
        (dci_proc.link_type == DCI_LINK_BEFORE_SR) ? "before 4K SR" : "after 8K SR");
    osal_proc_print(s, "4K SR : %-10d          8K SR: %-10d \n",
        dci_proc.dci_reso.sr_4k_en, dci_proc.dci_reso.sr_8k_en);
    osal_proc_print(s, "4K SR IN   w / h : %-10d / %-10d \n",
        dci_proc.dci_reso.sr_4k_in_width, dci_proc.dci_reso.sr_4k_in_height);
    osal_proc_print(s, "8K SR OUT  w / h : %-10d / %-10d \n",
        dci_proc.dci_reso.sr_8k_out_width, dci_proc.dci_reso.sr_8k_out_height);
    osal_proc_print(s, "%-20s : %-20s \n", "dci enable", (dci_proc.dci_enable == HI_TRUE) ? "on" : "off");
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_hdcpen(hi_bool on_off)
{
    if (PQ_FUNC_CALL(HI_PQ_MODULE_ZME, set_vdp_hdcp_en)) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_hdcp_en(on_off);
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

static hi_void drv_pq_proc_print_table(pq_prn_table_type type, hi_u64 pri_addr)
{
#ifdef PQ_PROC_CTRL_SUPPORT
    hi_u32 i;
    hi_u64 addr;
    hi_u32 value, module;
    hi_u8 lsb, msb, source_mode, output_mode;
    hi_u32 phy_list_num = pq_table_get_phy_list_num();
    pq_bin_phy_reg *phy_reg = pq_table_get_phy_reg();

    PQ_CHECK_OVER_RANGE_RE_NULL(type, PRN_TABLE_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(phy_list_num, PHY_REG_MAX);
    PQ_CHECK_NULL_PTR_RE_NULL(phy_reg);

    if (type == PRN_TABLE_SET_DEFAULT) {
        pq_table_set_table_reg_default();
        osal_printk("set pqbin default from code success!\n");
        return;
    }

    osal_printk("\n");
    osal_printk("list  addr  lsb  msb  source_mode  output_mode  module  value\n");

    for (i = 0; i < phy_list_num; i++) {
        addr = phy_reg[i].reg_addr;
        value = phy_reg[i].value;
        module = phy_reg[i].module;
        lsb = phy_reg[i].lsb;
        msb = phy_reg[i].msb;
        source_mode = phy_reg[i].source_mode;
        output_mode = phy_reg[i].output_mode;

        if ((type == PRN_TABLE_SINGLE) && (pri_addr != addr)) {
            continue;
        } else if ((type == PRN_TABLE_MULTI) && (pq_comm_get_alg_adape_type(module) != PQ_BIN_ADAPT_MULTIPLE)) {
            continue;
        } else if ((type == PRN_TABLE_ALG) && (pri_addr != module)) {
            continue;
        }

        osal_printk("\n");
        osal_printk("[%d]   0x%x   %d   %d %d  %d  0x%x %d\n",
            i, addr, lsb, msb, source_mode, output_mode, module, value);
    }

    osal_printk("\n\n");

#endif
}

hi_void drv_pq_proc_set_table(hi_u32 list, hi_u32 value)
{
#ifdef PQ_PROC_CTRL_SUPPORT
    hi_u64 addr;
    hi_u32 old_value, module;
    hi_u8 lsb, msb, source_mode, output_mode;
    pq_bin_phy_reg *phy_reg = pq_table_get_phy_reg();
    hi_u32 phy_list_num = pq_table_get_phy_list_num();
    if ((phy_reg == HI_NULL) || (phy_list_num > PHY_REG_MAX)) {
        return;
    }

    if (list >= phy_list_num) {
        return;
    }

    osal_printk("\n");
    osal_printk("list\t");
    osal_printk("addr\t");
    osal_printk("lsb\t");
    osal_printk("msb\t");
    osal_printk("source_mode\t");
    osal_printk("output_mode\t");
    osal_printk("module\t");
    osal_printk("value\t");

    addr = phy_reg[list].reg_addr;
    old_value = phy_reg[list].value;
    module = phy_reg[list].module;
    lsb = phy_reg[list].lsb;
    msb = phy_reg[list].msb;
    source_mode = phy_reg[list].source_mode;
    output_mode = phy_reg[list].output_mode;

    osal_printk("\n");
    osal_printk("[%d]\t", list);
    osal_printk("0x%x\t", addr);
    osal_printk("%d\t", lsb);
    osal_printk("%d\t", msb);
    osal_printk("%d\t\t", source_mode);
    osal_printk("%d\t\t", output_mode);
    osal_printk("0x%x\t", module);
    osal_printk("%d to %d\n", old_value, value);

    phy_reg[list].value = value;
#endif

    return;
}

#ifdef PQ_PROC_CTRL_SUPPORT
drv_pq_vpss_thread_proc g_vpss_thread_proc[VPSS_HANDLE_NUM] = {0};
static hi_void drv_pq_proc_read_vpss_thread(struct seq_file *s)
{
    hi_u32 handle;

    for (handle = 0; handle < VPSS_HANDLE_NUM; handle++) {
        drv_pq_proc_get_vpss_thread_info(handle, &g_vpss_thread_proc[handle]);
        if (g_vpss_thread_proc[handle].handle_enable == HI_TRUE) {
            osal_proc_print(s, "%-20s: %-20d\n", "handle id", handle);
            osal_proc_print(s, "%-20s: %-20d\n", "in handle enable", g_vpss_thread_proc[handle].handle_enable);
            osal_proc_print(s, "%-20s: %-20d", "in buf0 exist", g_vpss_thread_proc[handle].is_in_exist[0]);
            osal_proc_print(s, "%-20s: %-20d\n", "in buf1 exist", g_vpss_thread_proc[handle].is_in_exist[1]);
            osal_proc_print(s, "%-20s: %-20d", "out buf0 exist", g_vpss_thread_proc[handle].is_out_exist[0]);
            osal_proc_print(s, "%-20s: %-20d\n", "out buf1 exist", g_vpss_thread_proc[handle].is_out_exist[1]);
            osal_proc_print(s, "%-20s: %-20d", "input buf alloc", g_vpss_thread_proc[handle].is_input_alloc);
            osal_proc_print(s, "%-20s: %-20d\n", "output buf alloc", g_vpss_thread_proc[handle].is_output_alloc);
            osal_proc_print(s, "%-20s: %-20d", "input ready", g_vpss_thread_proc[handle].is_input_ready);
            osal_proc_print(s, "%-20s: %-20d\n", "output ready", g_vpss_thread_proc[handle].is_output_ready);
            osal_proc_print(s, "%-20s: %-20d", "input index", g_vpss_thread_proc[handle].input_index);
            osal_proc_print(s, "%-20s: %-20d\n", "output index", g_vpss_thread_proc[handle].output_index);

            osal_proc_print(s, "%-20s: %-20d", "nr input ready",
                g_vpss_thread_proc[handle].alg_in_ready[PQ_VPSS_ALG_NR]);
            osal_proc_print(s, "%-20s: %-20d\n", "nr output ready",
                g_vpss_thread_proc[handle].alg_out_ready[PQ_VPSS_ALG_NR]);

            osal_proc_print(s, "%-20s: %-20d", "dei input ready",
                g_vpss_thread_proc[handle].alg_in_ready[PQ_VPSS_ALG_DEI]);
            osal_proc_print(s, "%-20s: %-20d\n", "dei output ready",
                g_vpss_thread_proc[handle].alg_out_ready[PQ_VPSS_ALG_DEI]);

            osal_proc_print(s, "%-20s: %-20d", "db input ready",
                g_vpss_thread_proc[handle].alg_in_ready[PQ_VPSS_ALG_DB]);
            osal_proc_print(s, "%-20s: %-20d\n", "db output ready",
                g_vpss_thread_proc[handle].alg_out_ready[PQ_VPSS_ALG_DB]);

            osal_proc_print(s, "%-20s: %-20d", "dm input ready",
                g_vpss_thread_proc[handle].alg_in_ready[PQ_VPSS_ALG_DM]);
            osal_proc_print(s, "%-20s: %-20d\n", "dm output ready",
                g_vpss_thread_proc[handle].alg_out_ready[PQ_VPSS_ALG_DM]);
        }
    }

    return;
}

static hi_void drv_pq_proc_read_intf_time(struct seq_file *s)
{
    pq_intf_mode mode;
    for (mode = 0; mode < PQ_INTF_MODE_MAX; mode++) {
        if ((mode + 1) % 2 == 0) { /* 2:num */
            osal_proc_print(s, "\n");
        }

        osal_proc_print(s, "%-20s:    %-10lu (us)       ", g_pq_intf_str[mode], g_proc_intf_time_cnt[mode]);
    }
    osal_proc_print(s, "\n");

    return;
}
#endif

static pq_common_status g_proc_status = {0};
static pq_bin_param g_proc_bin_param = {0};
static hi_bool g_proc_bin_status = HI_FALSE;
static hi_drv_pq_param g_proc_param = {0};
static pq_demo_mode g_proc_demo_mode = PQ_DEMO_ENABLE_L;

static hi_drv_pq_vpss_timming g_proc_pq_vpss_timming = {0};
static hi_drv_pq_vdp_channel_timing g_proc_hd_timing_info = {0};
static hi_drv_pq_vdp_channel_timing g_proc_sd_timing_info = {0};

static hi_u8 *g_proc_source_type[PQ_SOURCE_MODE_MAX] = {
    "unknown",
    "SD",
    "FHD",
    "4K",
    "8K"
};

static hi_u8 *g_output_mode_tmp[PQ_OUTPUT_MODE_MAX] = {
    "unknown",
    "SD",
    "FHD",
    "4K",
    "8K60",
    "8K120"
};

hi_void drv_pq_proc_module_ctrl(struct seq_file *s)
{
    hi_bool eable_en = HI_FALSE;
    hi_bool demo_en = HI_FALSE;
    hi_bool strength = 0;
    hi_u32 alg_module = 0;

    osal_proc_print(s, "==========================algorithm ctrl information ========================\n");
    for (alg_module = 0; alg_module < HI_PQ_MODULE_MAX; alg_module++) {
        if (HI_NULL == GET_ALG(alg_module)) {
            continue;
        }

        osal_proc_print(s, "------------------------------------ %s ------------------------------------\n",
            pq_comm_get_alg_name(alg_module));
        if (pq_comm_get_alg_type_id(alg_module) == REG_TYPE_VPSS) {
            osal_proc_print(s, "%-20s: %-20s", "bind to", "vpss");
        } else if (pq_comm_get_alg_type_id(alg_module) == REG_TYPE_VDP) {
            osal_proc_print(s, "%-20s: %-20s", "bind to", "vdp");
        }

        if (GET_ALG_FUN(alg_module)->get_enable) {
            GET_ALG_FUN(alg_module)->get_enable(&eable_en);
            osal_proc_print(s, "%-20s: %-20s\n", "module", eable_en ? "on" : "off");
        }

        if (GET_ALG_FUN(alg_module)->get_strength) {
            GET_ALG_FUN(alg_module)->get_strength(&strength);
            osal_proc_print(s, "%-20s: %-20d", "strength", strength);
        }

        if (GET_ALG_FUN(alg_module)->get_demo) {
            GET_ALG_FUN(alg_module)->get_demo(&demo_en);
            osal_proc_print(s, "%-20s: %-20s\n", "demo", demo_en ? "on" : "off");
            if (demo_en && GET_ALG_FUN(alg_module)->get_demo_mode) {
                GET_ALG_FUN(alg_module)->get_demo_mode(&g_proc_demo_mode);
            }
        } else if (pq_comm_get_alg_type_id(alg_module) != REG_TYPE_ALL
                   || GET_ALG_FUN(alg_module)->get_enable
                   || GET_ALG_FUN(alg_module)->get_strength) {
            osal_proc_print(s, "\n");
        }
    }

    osal_proc_print(s, "================================================================================\n");
}

hi_void drv_pq_proc_module_info(struct seq_file *s)
{
    hi_u32 alg_module = 0;

    osal_proc_print(s, "==========================algorithm detail information ========================\n");
    for (alg_module = 0; alg_module < HI_PQ_MODULE_MAX; alg_module++) {
        if (HI_NULL == GET_ALG(alg_module)) {
            continue;
        }

        osal_proc_print(s, "------------------------------------ %s ------------------------------------\n",
            pq_comm_get_alg_name(alg_module));

        switch (alg_module) {
            case HI_PQ_MODULE_ACM:
                drv_pq_proc_read_acm(s);
                break;
            case HI_PQ_MODULE_CSC:
                drv_pq_proc_read_csc(s);
                break;
            case HI_PQ_MODULE_GFXCSC:
                drv_pq_proc_read_gfxcsc(s);
                break;
            case HI_PQ_MODULE_ZME:
                drv_pq_proc_read_zme(s);
                break;
            case HI_PQ_MODULE_GFXZME:
                drv_pq_proc_read_gfxzme(s);
                break;
            case HI_PQ_MODULE_DEI:
                drv_pq_proc_read_dei(s);
                break;
            case HI_PQ_MODULE_FMD:
                drv_pq_proc_read_fmd(s);
                break;
            case HI_PQ_MODULE_HDR:
                drv_pq_proc_read_hdr(s);
                break;
            case HI_PQ_MODULE_CLE:
                drv_pq_proc_read_cle(s);
                break;
            case HI_PQ_MODULE_SHARPNESS:
                drv_pq_proc_read_sharpen(s);
                break;
            case HI_PQ_MODULE_DCI:
                drv_pq_proc_read_dci(s);
                break;
            case HI_PQ_MODULE_VPSSHDR:
                drv_pq_proc_read_vpsshdr(s);
                break;
            default:
                break;
        }
    }
}

hi_void drv_pq_proc_read_pqbin(hi_void *seqfile)
{
    hi_u32 pq_param_size = sizeof(pq_bin_param);

    osal_proc_print(seqfile, "================================ PQ bin information =================================\n");
    osal_proc_print(seqfile, "%-20s: %s %s\n", "driver version", "PQ_V4_0" PQ_MODIFY_TIME,
                  "[build time:"__DATE__", "__TIME__"]");
    osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin version", PQ_VERSION);
    osal_proc_print(seqfile, "%-20s: %d\n", "PQ bin size", pq_param_size);

    if (g_proc_bin_status == HI_FALSE) {
        osal_proc_print(seqfile, "%-20s: failure\n", "PQ bin load");
    } else {
        osal_proc_print(seqfile, "%-20s: success\n", "PQ bin load");
        osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin version", g_proc_bin_param.pq_file_header.version);
        osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin chipname", g_proc_bin_param.pq_file_header.chip_name);
        osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin SDK version", g_proc_bin_param.pq_file_header.sdk_version);
        osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin author", g_proc_bin_param.pq_file_header.author);
        osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin describe", g_proc_bin_param.pq_file_header.desc);
        osal_proc_print(seqfile, "%-20s: %s\n", "PQ bin time", g_proc_bin_param.pq_file_header.time);
    }

    return;
}

hi_void drv_pq_proc_read_picture_param(hi_void *seqfile)
{
    hi_bool default_code = HI_FALSE;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();

    PQ_CHECK_OVER_RANGE_RE_NULL(source_mode, PQ_SOURCE_MODE_MAX);
    PQ_CHECK_OVER_RANGE_RE_NULL(output_mode, PQ_OUTPUT_MODE_MAX);
    drv_pq_get_default_param(&default_code);
    osal_proc_print(seqfile, "================== video information ================\n");
    osal_proc_print(seqfile, "%-20s: %-20d", "HD brightness", NUM2LEVEL(g_proc_param.hd_video_setting.brightness));
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD brightness", NUM2LEVEL(g_proc_param.sd_video_setting.brightness));
    osal_proc_print(seqfile, "%-20s: %-20d", "HD contrast", NUM2LEVEL(g_proc_param.hd_video_setting.contrast));
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD contrast", NUM2LEVEL(g_proc_param.sd_video_setting.contrast));
    osal_proc_print(seqfile, "%-20s: %-20d", "HD hue", NUM2LEVEL(g_proc_param.hd_video_setting.hue));
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD hue", NUM2LEVEL(g_proc_param.sd_video_setting.hue));
    osal_proc_print(seqfile, "%-20s: %-20d", "HD saturation", NUM2LEVEL(g_proc_param.hd_video_setting.saturation));
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD saturation", NUM2LEVEL(g_proc_param.sd_video_setting.saturation));
    osal_proc_print(seqfile, "%-20s: %-20d", "HD wcg_temperature",
        NUM2LEVEL(g_proc_param.hd_video_setting.wcg_temperature));
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD wcg_temperature",
        NUM2LEVEL(g_proc_param.sd_video_setting.wcg_temperature));
    osal_proc_print(seqfile, "%-20s: %-20s\n", "default param", default_code ? "yes" : "no");
    osal_proc_print(seqfile, "%-20s: %-20s\n", "3D type", g_proc_status.b3d_type ? "yes" : "no");
    osal_proc_print(seqfile, "%-20s: %-20s", "out mode", g_output_mode_tmp[output_mode]);
    osal_proc_print(seqfile, "%-20s: %s(%4d*%4d)\n", "source", g_proc_source_type[source_mode],
        g_proc_pq_vpss_timming.width, g_proc_pq_vpss_timming.height);
    osal_proc_print(seqfile, "%-20s: %4d*%4d, progressive:%d, refresh rate:%d\n", "HD output",
        g_proc_hd_timing_info.fmt_rect.rect_w, g_proc_hd_timing_info.fmt_rect.rect_h,
        g_proc_hd_timing_info.frame_fmt, g_proc_hd_timing_info.refresh_rate);
    osal_proc_print(seqfile, "%-20s: %4d*%4d, progressive:%d, refresh rate:%d\n", "SD output",
        g_proc_sd_timing_info.fmt_rect.rect_w, g_proc_sd_timing_info.fmt_rect.rect_h,
        g_proc_sd_timing_info.frame_fmt, g_proc_sd_timing_info.refresh_rate);

    osal_proc_print(seqfile, "=============== picture information ================\n");
    osal_proc_print(seqfile, "%-20s: %-20d", "HD brightness", g_proc_param.hd_picture_setting.brightness);
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD brightness", g_proc_param.sd_picture_setting.brightness);
    osal_proc_print(seqfile, "%-20s: %-20d", "HD contrast", g_proc_param.hd_picture_setting.contrast);
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD contrast", g_proc_param.sd_picture_setting.contrast);
    osal_proc_print(seqfile, "%-20s: %-20d", "HD hue", g_proc_param.hd_picture_setting.hue);
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD hue", g_proc_param.sd_picture_setting.hue);
    osal_proc_print(seqfile, "%-20s: %-20d", "HD saturation", g_proc_param.hd_picture_setting.saturation);
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD saturation", g_proc_param.sd_picture_setting.saturation);
    osal_proc_print(seqfile, "%-20s: %-20d", "HD wcg_temperature", g_proc_param.hd_picture_setting.wcg_temperature);
    osal_proc_print(seqfile, "%-20s: %-20d\n", "SD wcg_temperature", g_proc_param.sd_picture_setting.wcg_temperature);

    return;
}

hi_s32 drv_pq_proc_read(hi_void *seqfile, hi_void *private)
{
#ifdef PQ_PROC_CTRL_SUPPORT

    drv_pq_get_pq_status(&g_proc_status);
    drv_pq_get_pq_bin_param(&g_proc_bin_param);
    drv_pq_get_pq_bin_status(&g_proc_bin_status);
    drv_pq_get_pq_param(&g_proc_param);
    drv_pq_get_vpss_timming(&g_proc_pq_vpss_timming);

    g_proc_hd_timing_info = g_proc_status.timing_info[HI_PQ_DISPLAY_1];
    g_proc_sd_timing_info = g_proc_status.timing_info[HI_PQ_DISPLAY_0];

    drv_pq_proc_read_pqbin(seqfile);
    drv_pq_proc_read_picture_param(seqfile);
    osal_proc_print(seqfile, "=================== vpss thread info======================\n");
    drv_pq_proc_read_vpss_thread(seqfile);
    osal_proc_print(seqfile, "================== intf time info=====================\n");
    drv_pq_proc_read_intf_time(seqfile);
    drv_pq_proc_module_ctrl(seqfile);
    drv_pq_proc_module_info(seqfile);

#endif
    return HI_SUCCESS;
}

hi_s32 drv_pq_create_proc(hi_void)
{
    hi_char proc_name[PQ_PROC_NAME_LENGTH];
    osal_proc_entry *proc_item = NULL;

    snprintf(proc_name, PQ_PROC_NAME_LENGTH, "pq");
    proc_item = osal_proc_add(proc_name, strlen(proc_name));
    if (proc_item == NULL) {
        HI_ERR_PQ("PQ proc register fail!\n");
        return HI_FAILURE;
    }

    proc_item->private = HI_NULL;
    proc_item->read = drv_pq_proc_read;
    proc_item->cmd_list = g_pq_proc_info;
    proc_item->cmd_cnt = sizeof(g_pq_proc_info) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void drv_pq_remove_proc(hi_void)
{
    hi_char proc_name[PQ_PROC_NAME_LENGTH];

    snprintf(proc_name, PQ_PROC_NAME_LENGTH, "pq");
    proc_name[PQ_PROC_NAME_LENGTH - 1] = '\0';

    osal_proc_remove(proc_name, strlen(proc_name));
}

hi_void drv_pq_proc_print_base(hi_void)
{
    osal_printk("echo help                        > /proc/msp/pq\n");
    osal_printk("echo bright          <0~100>     > /proc/msp/pq\n");
    osal_printk("echo contrast        <0~100>     > /proc/msp/pq\n");
    osal_printk("echo hue             <0~100>     > /proc/msp/pq\n");
    osal_printk("echo satu            <0~100>     > /proc/msp/pq\n");
    osal_printk("echo sharp/dci/acm/tnr/snr/db/dm/all <0~100>         > /proc/msp/pq\n");
    osal_printk("echo sharp/dci/acm/tnr/snr/dei/db/dm/dr/ds/cle/sr/all enable/disable  > /proc/msp/pq\n");
    osal_printk("echo sharp/dci/acm/tnr/snr/dei/db/vpscene/vdp4ksnr/vpsshdr/all \
                            demoon/demooff  > /proc/msp/pq\n");
    osal_printk("echo dei/fmd/fod/db/all debug_on/debug_off  > /proc/msp/pq\n");
    osal_printk("echo 4ksr_scale_mode/8ksr_scale_mode 0~8  > /proc/msp/pq\n");
    osal_printk("echo mode            debug/normal     > /proc/msp/pq\n");
    osal_printk("echo scd/scenechange on/off           > /proc/msp/pq\n");
    osal_printk("echo cfgdefault      on/off           > /proc/msp/pq\n");
    osal_printk("echo mc_only         on/off           > /proc/msp/pq\n");
    osal_printk("echo hdcp            on/off           > /proc/msp/pq\n");
    osal_printk("echo colormode       blue/green/bg/optimal/off  > /proc/msp/pq\n");
    osal_printk("echo deimode         auto/fir/copy              > /proc/msp/pq\n");
    osal_printk("echo fodmode         auto/top1st/bottom1st/unkown      > /proc/msp/pq\n");
    osal_printk("echo deshoot         flat/medfir                > /proc/msp/pq\n");
    osal_printk("echo demomode        fixr/fixl/scrollr/scrolll  > /proc/msp/pq\n");
    osal_printk("echo zme             vdpfir/vdpcopy/vdpmedon/vdpmedoff         > /proc/msp/pq\n");
    osal_printk("echo zme             vpssfir/vpsscopy/vpssmedon/vpssmedoff      > /proc/msp/pq\n");
    osal_printk("echo gfxzme         gfxfir/gfxcopy/gfxmedon/gfxmedoff         > /proc/msp/pq\n");
    osal_printk("echo gfxzmeoffset  hl(-512~512) hc(-512~512) vtp(-512~512) vbtm(-512~512) \
        > /proc/msp/pq\n");
    osal_printk("echo fleshtone       off/low/mid/high           > /proc/msp/pq\n");
    return;
}

hi_void drv_pq_proc_print_special(hi_void)
{
    if (g_en_pq_proc_print_level == PQ_PROC_PRINT_LEVEL3) {
        osal_printk("echo vdpzmestrategy  zme_num(0:no set,1:just use zme1) w_draw(width draw,0:no set,1/2/4/8:draw) \
            h_draw(height draw, 0:no set, 1/2/4/8:draw) > /proc/msp/pq\n");
    }
    osal_printk("echo printbin        <hex>/all/multi  > /proc/msp/pq\n");
    osal_printk("echo printalgbin     <0~20>           > /proc/msp/pq\n");
    osal_printk("echo printhdrtm      0(ori)/1(b100)/2(b0)/3(d100)/4(d0)/5(BP)/6(DP) > /proc/msp/pq\n");
    osal_printk("echo printtype       <hex>            > /proc/msp/pq\n");
    osal_printk("echo printalgbin     <0~20>           > /proc/msp/pq\n");
    osal_printk("echo setbin          default          > /proc/msp/pq\n");
    osal_printk("echo setbin<list>    value            > /proc/msp/pq\n");
    osal_printk("echo sethdrcfg       default/debug    > /proc/msp/pq\n");
    osal_printk("echo imagemode       normal/videophone/gallery    > /proc/msp/pq\n");
    osal_printk("echo preview/remote  recommed/black/color/bt/warm/cool/old/user > /proc/msp/pq\n");
    osal_printk("echo colortemp       rr gg bb           > /proc/msp/pq\n");
    osal_printk("echo hdroffset       hdr_scene(00:hdr2_sdr,10:sdr2_hdr) bb cc ss hh rr gg bb   > /proc/msp/pq\n");
    osal_printk("echo hdrtm           hdr_scene(00:hdr2_sdr,10:sdr2_hdr) M(0-1) \
                            DD(dci_d:0-99) BB(dci_b:0-99) dd(0-99) bb(0-99) > /proc/msp/pq\n");
    osal_printk("echo gfxhdroffset    gfx_hdr_scene(10:sdr2_hdr,11:sdr2_hlg) bb cc ss hh rr gg bb \
                            > /proc/msp/pq\n");
    osal_printk("echo gfxhdrtm        gfx_hdr_scene(10:sdr2_hdr,11:sdr2_hlg) \
                            M(0-1) dd(0-99) bb(0-99) > /proc/msp/pq\n");

    osal_printk("echo frost           off/low/mid/high           > /proc/msp/pq\n");
    return;
}

static hi_s32 drv_pq_proc_print_help(unsigned int argc,
                                     char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    drv_pq_proc_print_base();
    drv_pq_proc_print_special();
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_4ksr_scale_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 mode;
    hi_pq_module_type module = HI_PQ_MODULE_SR;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    mode = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    if (HI_NULL == PQ_FUNC_CALL(module, set_sr_scale_mode)) {
        HI_ERR_PQ("get set_sr_scale_mode function failed!\n");
        return HI_FAILURE;
    }

    if (mode > 8) { /* max scale mode is 8 */
        HI_ERR_PQ("scale mode->%d is invalid!\n", mode);
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    ret = GET_ALG_FUN(module)->set_sr_scale_mode(SR_ID_4K, mode);

    return ret;
}

static hi_s32 drv_pq_proc_set_8ksr_scale_mode(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 mode;
    hi_pq_module_type module = HI_PQ_MODULE_SR;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    mode = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    if (HI_NULL == PQ_FUNC_CALL(module, set_sr_scale_mode)) {
        HI_ERR_PQ("get set_sr_scale_mode function failed!\n");
        return HI_FAILURE;
    }

    if (mode > 8) { /* max scale mode is 8 */
        HI_ERR_PQ("scale mode->%d is invalid!\n", mode);
        return HI_FAILURE;
    }

    ret = GET_ALG_FUN(module)->set_sr_scale_mode(SR_ID_8K, mode);

    return ret;
}

static hi_s32 drv_pq_proc_set_brightness(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    drv_pq_set_brightness(HI_PQ_DISPLAY_0, data);
    drv_pq_set_brightness(HI_PQ_DISPLAY_1, data);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_contrast(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    drv_pq_set_contrast(HI_PQ_DISPLAY_0, data);
    drv_pq_set_contrast(HI_PQ_DISPLAY_1, data);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_hue(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    drv_pq_set_hue(HI_PQ_DISPLAY_0, data);
    drv_pq_set_hue(HI_PQ_DISPLAY_1, data);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_saturation(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    drv_pq_set_saturation(HI_PQ_DISPLAY_0, data);
    drv_pq_set_saturation(HI_PQ_DISPLAY_1, data);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_demo_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_demo_mode demo_mode = HI_PQ_DEMO_MODE_MAX;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("fixr"), "fixr", strlen("fixr")) == 0) {
        demo_mode = HI_PQ_DEMO_MODE_FIXED_R;
    } else if (osal_strncmp(argv[1], strlen("fixl"), "fixl", strlen("fixl")) == 0) {
        demo_mode = HI_PQ_DEMO_MODE_FIXED_L;
    } else if (osal_strncmp(argv[1], strlen("scrollr"), "scrollr", strlen("scrollr")) == 0) {
        demo_mode = HI_PQ_DEMO_MODE_SCROLL_R;
    } else if (osal_strncmp(argv[1], strlen("scrolll"), "scrolll", strlen("scrolll")) == 0) {
        demo_mode = HI_PQ_DEMO_MODE_SCROLL_L;
    } else if ((0 <= osal_strtol(argv[2], NULL, PQ_ARG_FMT_10)) && /* 2: num */
               (osal_strtol(argv[2], NULL, PQ_ARG_FMT_10) <= PQ_ALG_MAX_VALUE)) { /* 2: num */
        drv_pq_set_demo_coordinate(REG_TYPE_VPSS,
            osal_strtol(argv[2], NULL, PQ_ARG_FMT_10));  /* 2: num */
        drv_pq_set_demo_coordinate(REG_TYPE_VDP,
            osal_strtol(argv[2], NULL, PQ_ARG_FMT_10)); /* 2: num */
        return HI_SUCCESS;
    } else {
        return HI_FAILURE;
    }

    drv_pq_set_demo_disp_mode(REG_TYPE_VPSS, demo_mode);
    drv_pq_set_demo_disp_mode(REG_TYPE_VDP, demo_mode);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_color_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_color_spec_mode enhance_mode;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("blue"), "blue", strlen("blue")) == 0) {
        enhance_mode = HI_PQ_COLOR_MODE_BLUE;
    } else if (osal_strncmp(argv[1], strlen("green"), "green", strlen("green")) == 0) {
        enhance_mode = HI_PQ_COLOR_MODE_GREEN;
    } else if (osal_strncmp(argv[1], strlen("bg"), "bg", strlen("bg")) == 0) {
        enhance_mode = HI_PQ_COLOR_MODE_BG;
    } else if (osal_strncmp(argv[1], strlen("optimal"), "optimal", strlen("optimal")) == 0) {
        enhance_mode = HI_PQ_COLOR_MODE_RECOMMEND;
    } else if (osal_strncmp(argv[1], strlen("off"), "off", strlen("off")) == 0) {
        enhance_mode = HI_PQ_COLOR_MODE_ORIGINAL;
    } else {
        return HI_FAILURE;
    }

    return drv_pq_set_color_enhance_mode(enhance_mode);
}

static hi_s32 drv_pq_proc_set_flesh_tone(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_fleshtone flesh_tone_level;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("off"), "off", strlen("off")) == 0) {
        flesh_tone_level = HI_PQ_FLESHTONE_GAIN_OFF;
    } else if (osal_strncmp(argv[1], strlen("low"), "low", strlen("low")) == 0) {
        flesh_tone_level = HI_PQ_FLESHTONE_GAIN_LOW;
    } else if (osal_strncmp(argv[1], strlen("mid"), "mid", strlen("mid")) == 0) {
        flesh_tone_level = HI_PQ_FLESHTONE_GAIN_MID;
    } else if (osal_strncmp(argv[1], strlen("high"), "high", strlen("high")) == 0) {
        flesh_tone_level = HI_PQ_FLESHTONE_GAIN_HIGH;
    } else {
        return HI_FAILURE;
    }

    return drv_pq_set_flesh_tone_level(flesh_tone_level);
}

static hi_s32 drv_pq_proc_set_vdp_zme_strategy(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_proc_vdpzme_strategy proc_vdp_zme_strategy = { 0 };
    hi_u32 proc_zme_num;
    hi_u32 proc_w_draw;
    hi_u32 proc_h_draw;

    if (pq_osal_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    /* set vdp zme num :    1 : just use zme1;    0 :  use normal zme number */
    proc_zme_num = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);

    /* set vdp hor draw mode : 0/1/2/4/8;  0:not support proc setting */
    proc_w_draw = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */

    /* set vdp ver draw mode : 0/1/2/4/8;  0:not support proc setting */
    proc_h_draw = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */
    if (((proc_zme_num == HI_PQ_PROC_ZME_NUM_ORI) || (proc_zme_num == HI_PQ_PROC_ZME_NUM_ONLY_ZME1))
        && ((proc_w_draw == HI_PQ_PROC_PREZME_HOR_ORI) || (proc_w_draw == HI_PQ_PROC_PREZME_HOR_1X)
            || (proc_w_draw == HI_PQ_PROC_PREZME_HOR_2X) || (proc_w_draw == HI_PQ_PROC_PREZME_HOR_4X)
            || (proc_w_draw == HI_PQ_PROC_PREZME_HOR_8X))
        && ((proc_h_draw == HI_PQ_PROC_PREZME_VER_ORI) || (proc_h_draw == HI_PQ_PROC_PREZME_VER_1X)
            || (proc_h_draw == HI_PQ_PROC_PREZME_VER_2X) || (proc_h_draw == HI_PQ_PROC_PREZME_VER_4X)
            || (proc_h_draw == HI_PQ_PROC_PREZME_VER_8X))) {
        proc_vdp_zme_strategy.proc_vdp_zme_num = (hi_pq_proc_zme_num)proc_zme_num;
        proc_vdp_zme_strategy.proc_vdp_width_draw_mode = (hi_pq_proc_prezme_hor_mul)proc_w_draw;
        proc_vdp_zme_strategy.proc_vdp_height_draw_mode = (hi_pq_proc_prezme_ver_mul)proc_h_draw;

        if (PQ_FUNC_CALL(HI_PQ_MODULE_ZME, set_proc_vdp_zme_strategy)) {
            GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_proc_vdp_zme_strategy(proc_vdp_zme_strategy);
        }

        return HI_SUCCESS;
    } else {
#ifdef PQ_PROC_CTRL_SUPPORT
        osal_printk("vdp zme strategy: zme num=%d, width_draw=%d, height_draw=%d\n",
            proc_zme_num, proc_w_draw, proc_h_draw);
#endif
        return HI_FAILURE;
    }
}

static hi_s32 drv_pq_proc_set_dei_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_fod_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;
    hi_pq_module_type module = HI_PQ_MODULE_FMD;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (HI_NULL == PQ_FUNC_CALL(module, set_force_fod_mode)) {
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("auto"), "auto", strlen("auto")) == 0) {
        ret = GET_ALG_FUN(module)->set_force_fod_mode(PQ_FOD_ENABLE_AUTO);
    } else if (osal_strncmp(argv[1], strlen("top1st"), "top1st", strlen("top1st")) == 0) {
        ret = GET_ALG_FUN(module)->set_force_fod_mode(PQ_FOD_TOP_FIRST);
    } else if (osal_strncmp(argv[1], strlen("bottom1st"), "bottom1st", strlen("bottom1st")) == 0) {
        ret = GET_ALG_FUN(module)->set_force_fod_mode(PQ_FOD_BOTTOM_FIRST);
    } else if (osal_strncmp(argv[1], strlen("unkown"), "unkown", strlen("unkown")) == 0) {
        ret = GET_ALG_FUN(module)->set_force_fod_mode(PQ_FOD_UNKOWN);
    } else {
        osal_printk("fod mode: %s\n", argv[1]);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_print_bin(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data = 0;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("all"), "all", strlen("all")) == 0) {
        drv_pq_proc_print_table(PRN_TABLE_ALL, data);
    } else if (osal_strncmp(argv[1], strlen("multi"), "multi", strlen("multi")) == 0) {
        drv_pq_proc_print_table(PRN_TABLE_MULTI, data);
    } else {
        data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_16);
        drv_pq_proc_print_table(PRN_TABLE_SINGLE, data);
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_print_alg_bin(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    drv_pq_proc_print_table(PRN_TABLE_ALG, data);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_bin(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_hdrcfg(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("default"), "default", strlen("default")) == 0) {
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_default_cfg)) {
            GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_default_cfg(HI_TRUE);
        }

        return HI_SUCCESS;
    } else if (osal_strncmp(argv[1], strlen("debug"), "debug", strlen("debug")) == 0) {
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_default_cfg)) {
            GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_default_cfg(HI_FALSE);
        }

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

static hi_s32 drv_pq_proc_set_print_type(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 data;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    data = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_16);
    pq_hal_set_print_type(data);

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_scene_change(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("on"), "on", strlen("on")) == 0) {
        if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, set_dci_scd)) {
            GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_dci_scd(HI_TRUE);
        }
    } else if (osal_strncmp(argv[1], strlen("off"), "off", strlen("off")) == 0) {
        if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, set_dci_scd)) {
            GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_dci_scd(HI_FALSE);
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_cfg_default(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("on"), "on", strlen("on")) == 0) {
        hi_drv_pq_set_default_param(HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("off"), "off", strlen("off")) == 0) {
        hi_drv_pq_set_default_param(HI_FALSE);
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_mc_only(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_bool enable_mc_only = HI_TRUE;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("on"), "on", strlen("on")) == 0) {
        enable_mc_only = HI_TRUE;
    } else if (osal_strncmp(argv[1], strlen("off"), "off", strlen("off")) == 0) {
        enable_mc_only = HI_FALSE;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DEI, set_mc_only_enable)) {
        GET_ALG_FUN(HI_PQ_MODULE_DEI)->set_mc_only_enable(enable_mc_only);
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_hdcp(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_bool hdcp_en = HI_FALSE;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("on"), "on", strlen("on")) == 0) {
        hdcp_en = HI_TRUE;
    } else if (osal_strncmp(argv[1], strlen("off"), "off", strlen("off")) == 0) {
        hdcp_en = HI_FALSE;
    }

    return drv_pq_proc_set_hdcpen(hdcp_en);
}

static hi_s32 drv_pq_proc_set_image_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[0], strlen("imagemode"), "imagemode", strlen("imagemode")) == 0) {
        if (osal_strncmp(argv[1], strlen("normal"), "normal", strlen("normal")) == 0) {
            drv_pq_set_image_mode(HI_PQ_IMAGE_MODE_NORMAL);
        } else if (osal_strncmp(argv[1], strlen("videophone"), "videophone", strlen("videophone")) == 0) {
            drv_pq_set_image_mode(HI_PQ_IMAGE_MODE_VIDEOPHONE);
        } else if (osal_strncmp(argv[1], strlen("gallery"), "gallery", strlen("gallery")) == 0) {
            drv_pq_set_image_mode(HI_PQ_IMAGE_MODE_GALLERY);
        }
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
static hi_s32 drv_pq_proc_set_color_temp(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 date_r, date_g, date_b;
    hi_pq_color_temperature temperature = { 0 };

    if (pq_osal_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    date_r = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    date_g = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */
    date_b = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */

    temperature.red_gain = date_r;
    temperature.green_gain = date_g;
    temperature.blue_gain = date_b;

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_color_temp)) {
        GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_color_temp(HI_PQ_DISPLAY_1, &temperature);
        GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_color_temp(HI_PQ_DISPLAY_0, &temperature);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_color_temp)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_color_temp(HI_PQ_DISPLAY_1, &temperature);
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_color_temp(HI_PQ_DISPLAY_0, &temperature);
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_hdr_offset(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_hdr_offset hdr_offset = {0};
    hi_u32 date;

    if (pq_osal_cmd_check(argc, 9, private, private) != HI_SUCCESS) { /* 9 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    date = osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    PQ_CHECK_OVER_RANGE_RE_FAIL(date, HI_PQ_HDR_MODE_MAX);
    hdr_offset.hdr_process_scene = (hi_pq_hdr_mode)date;
    hdr_offset.bright = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */
    hdr_offset.contrast = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */
    hdr_offset.satu = (hi_u32)osal_strtol(argv[4], NULL, PQ_ARG_FMT_10); /* 4: index */
    hdr_offset.hue = (hi_u32)osal_strtol(argv[5], NULL, PQ_ARG_FMT_10); /* 5: index */
    hdr_offset.r = (hi_u32)osal_strtol(argv[6], NULL, PQ_ARG_FMT_10); /* 6: index */
    hdr_offset.g = (hi_u32)osal_strtol(argv[7], NULL, PQ_ARG_FMT_10); /* 7: index */
    hdr_offset.b = (hi_u32)osal_strtol(argv[8], NULL, PQ_ARG_FMT_10); /* 8: index */

    hdr_offset.bright = LEVEL2NUM(hdr_offset.bright);
    hdr_offset.contrast = LEVEL2NUM(hdr_offset.contrast);
    hdr_offset.satu = LEVEL2NUM(hdr_offset.satu);
    hdr_offset.hue = LEVEL2NUM(hdr_offset.hue);

    hdr_offset.r = LEVEL2NUM(hdr_offset.r);
    hdr_offset.g = LEVEL2NUM(hdr_offset.g);
    hdr_offset.b = LEVEL2NUM(hdr_offset.b);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_offset)) {
        GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_offset(&hdr_offset);
    }
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_gfx_hdr_offset(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_hdr_offset hdr_offset = {0};
    hi_u32 date;

    if (pq_osal_cmd_check(argc, 9, private, private) != HI_SUCCESS) { /* 9 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    date = osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    PQ_CHECK_OVER_RANGE_RE_FAIL(date, HI_PQ_HDR_MODE_MAX);
    hdr_offset.hdr_process_scene = (hi_pq_hdr_mode)date;
    hdr_offset.bright = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */
    hdr_offset.contrast = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */
    hdr_offset.satu = (hi_u32)osal_strtol(argv[4], NULL, PQ_ARG_FMT_10); /* 4: index */
    hdr_offset.hue = (hi_u32)osal_strtol(argv[5], NULL, PQ_ARG_FMT_10); /* 5: index */
    hdr_offset.r = (hi_u32)osal_strtol(argv[6], NULL, PQ_ARG_FMT_10); /* 6: index */
    hdr_offset.g = (hi_u32)osal_strtol(argv[7], NULL, PQ_ARG_FMT_10); /* 7: index */
    hdr_offset.b = (hi_u32)osal_strtol(argv[8], NULL, PQ_ARG_FMT_10); /* 8: index */

    hdr_offset.bright = LEVEL2NUM(hdr_offset.bright);
    hdr_offset.contrast = LEVEL2NUM(hdr_offset.contrast);
    hdr_offset.satu = LEVEL2NUM(hdr_offset.satu);
    hdr_offset.hue = LEVEL2NUM(hdr_offset.hue);

    hdr_offset.r = LEVEL2NUM(hdr_offset.r);
    hdr_offset.g = LEVEL2NUM(hdr_offset.g);
    hdr_offset.b = LEVEL2NUM(hdr_offset.b);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_offset)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_offset(&hdr_offset);
    }
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_hdr_tm(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_hdr_offset hdr_offset = {0};
    hi_u32 date;

    if (pq_osal_cmd_check(argc, 7, private, private) != HI_SUCCESS) { /* 7 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    date = osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    PQ_CHECK_OVER_RANGE_RE_FAIL(date, HI_PQ_HDR_MODE_MAX);
    hdr_offset.hdr_process_scene = (hi_pq_hdr_mode)date;
    hdr_offset.hdr_mode = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */
    hdr_offset.ac_cdark = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */
    hdr_offset.ac_cbrigt = (hi_u32)osal_strtol(argv[4], NULL, PQ_ARG_FMT_10); /* 4: index */
    hdr_offset.dark_cv = (hi_u32)osal_strtol(argv[5], NULL, PQ_ARG_FMT_10); /* 5: index */
    hdr_offset.bright_cv = (hi_u32)osal_strtol(argv[6], NULL, PQ_ARG_FMT_10); /* 6: index */

    if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_tm_curve)) {
        GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_tm_curve(&hdr_offset);
    }
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_gfx_hdr_tm(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_pq_hdr_offset hdr_offset = {0};
    hi_u32 date;

    if (pq_osal_cmd_check(argc, 5, private, private) != HI_SUCCESS) { /* 5 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    date = osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    PQ_CHECK_OVER_RANGE_RE_FAIL(date, HI_PQ_HDR_MODE_MAX);
    hdr_offset.hdr_process_scene = (hi_pq_hdr_mode)date;
    hdr_offset.hdr_mode = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */
    hdr_offset.dark_cv = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */
    hdr_offset.bright_cv = (hi_u32)osal_strtol(argv[4], NULL, PQ_ARG_FMT_10); /* 4: index */

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_tm_curve)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_tm_curve(&hdr_offset);
    }
    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_gfxzme_offset(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    gfx_zme_strategy_proc para;
    hi_s32 offset_hl;
    hi_s32 offset_hc;
    hi_s32 offset_v_tp;
    hi_s32 offset_v_btm;
    hi_s32 ret = HI_SUCCESS;

    if (pq_osal_cmd_check(argc, 5, private, private) != HI_SUCCESS) { /* 5 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    offset_hl = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
    offset_hc = (hi_u32)osal_strtol(argv[2], NULL, PQ_ARG_FMT_10); /* 2: index */
    offset_v_tp = (hi_u32)osal_strtol(argv[3], NULL, PQ_ARG_FMT_10); /* 3: index */
    offset_v_btm = (hi_u32)osal_strtol(argv[4], NULL, PQ_ARG_FMT_10); /* 4: index */

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, get_strategy_by_proc)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->get_strategy_by_proc(&para);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    if ((offset_hl >= PQ_OFFSET_MIN) && (offset_hl <= PQ_OFFSET_MAX) &&
        (offset_hc >= PQ_OFFSET_MIN) && (offset_hc <= PQ_OFFSET_MAX) &&
        (offset_v_tp >= PQ_OFFSET_MIN) && (offset_v_tp <= PQ_OFFSET_MAX) &&
        (offset_v_btm >= PQ_OFFSET_MIN) && (offset_v_btm <= PQ_OFFSET_MAX)) {
        para.gfx_zme_offset_update =  HI_TRUE;
        para.gfx_zme_out.zme_offset.zme_offset_hl = offset_hl;
        para.gfx_zme_out.zme_offset.zme_offset_hc = offset_hc;
        para.gfx_zme_out.zme_offset.zme_offset_v_tp = offset_v_tp;
        para.gfx_zme_out.zme_offset.zme_offset_v_btm = offset_v_btm;
    } else {
#ifdef PQ_PROC_CTRL_SUPPORT
        osal_printk("gfxzme offset: offset_hl=%d, offset_hc=%d, \
            offset_v_tp=%d, offset_v_btm=%d\n",
            offset_hl, offset_hc, offset_v_tp, offset_v_btm);
#endif
        return HI_FAILURE;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, set_strategy_by_proc)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->set_strategy_by_proc(para);
    }

    return ret;
}

static hi_s32 drv_pq_proc_set_gfxzme_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    gfx_zme_strategy_proc para;
    hi_s32 ret = HI_SUCCESS;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, get_strategy_by_proc)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->get_strategy_by_proc(&para);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    if (osal_strncmp(argv[1], strlen("gfxfir"), "gfxfir", strlen("gfxfir")) == 0) {
        para.gfx_zme_fir_update = HI_TRUE;
        para.gfx_zme_fir_mode = HI_DRV_PQ_ZME_FIR;
    } else if (osal_strncmp(argv[1], strlen("gfxcopy"), "gfxcopy", strlen("gfxcopy")) == 0) {
        para.gfx_zme_fir_update = HI_TRUE;
        para.gfx_zme_fir_mode = HI_DRV_PQ_ZME_COPY;
    } else if (osal_strncmp(argv[1], strlen("gfxmedon"), "gfxmedon", strlen("gfxmedon")) == 0) {
        para.gfx_zme_fir_update = HI_TRUE;
        para.gfx_zme_med_en = HI_TRUE;
    } else if (osal_strncmp(argv[1], strlen("gfxmedoff"), "gfxmedoff", strlen("gfxmedoff")) == 0) {
        para.gfx_zme_fir_update = HI_TRUE;
        para.gfx_zme_med_en = HI_FALSE;
    } else {
#ifdef PQ_PROC_CTRL_SUPPORT
        osal_printk("gfxzme mode: %s\n", argv[1]);
#endif
        return HI_FAILURE;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, set_strategy_by_proc)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->set_strategy_by_proc(para);
    }

    return ret;
}

static hi_s32 drv_pq_proc_set_zme_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (HI_NULL == PQ_FUNC_CALL(HI_PQ_MODULE_ZME, set_vdp_zme_mode)) {
        return HI_FAILURE;
    }

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[1], strlen("vdpfir"), "vdpfir", strlen("vdpfir")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VDP_FIR, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("vdpcopy"), "vdpcopy", strlen("vdpcopy")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VDP_FIR, HI_FALSE);
    } else if (osal_strncmp(argv[1], strlen("vdpmedon"), "vdpmedon", strlen("vdpmedon")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VDP_MED, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("vdpmedoff"), "vdpmedoff", strlen("vdpmedoff")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VDP_MED, HI_FALSE);
    } else if (osal_strncmp(argv[1], strlen("vpssfir"), "vpssfir", strlen("vpssfir")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VPSS_FIR, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("vercopy"), "vpsscopy", strlen("vercopy")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VPSS_FIR, HI_FALSE);
    } else if (osal_strncmp(argv[1], strlen("vpssmedon"), "vpssmedon", strlen("vpssmedon")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VPSS_MED, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("vpssmedoff"), "vpssmedoff", strlen("vpssmedoff")) == 0) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_mode(PQ_ZME_MODE_VPSS_MED, HI_FALSE);
    } else {
#ifdef PQ_PROC_CTRL_SUPPORT
        osal_printk("zme mode: %s\n", argv[1]);
#endif
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_alg_ctrl_arg1(hi_char *arg1, hi_pq_module_type *pen_module)
{
    hi_pq_module_type module = HI_PQ_MODULE_MAX;

    if (osal_strncmp(arg1, strlen("fmd"), "fmd", strlen("fmd")) == 0) {
        module = HI_PQ_MODULE_FMD;
    } else if (osal_strncmp(arg1, strlen("tnr"), "tnr", strlen("tnr")) == 0) {
        module = HI_PQ_MODULE_TNR;
    } else if (osal_strncmp(arg1, strlen("snr"), "snr", strlen("snr")) == 0) {
        module = HI_PQ_MODULE_SNR;
    } else if (osal_strncmp(arg1, strlen("db"), "db", strlen("db")) == 0) {
        module = HI_PQ_MODULE_DB;
    } else if (osal_strncmp(arg1, strlen("dr"), "dr", strlen("dr")) == 0) {
        module = HI_PQ_MODULE_DR;
    } else if (osal_strncmp(arg1, strlen("dm"), "dm", strlen("dm")) == 0) {
        module = HI_PQ_MODULE_DM;
    } else if (osal_strncmp(arg1, strlen("sr"), "sr", strlen("sr")) == 0) {
        module = HI_PQ_MODULE_SR;
    } else if (osal_strncmp(arg1, strlen("sharp"), "sharp", strlen("sharp")) == 0) {
        module = HI_PQ_MODULE_SHARPNESS;
    } else if (osal_strncmp(arg1, strlen("dci"), "dci", strlen("dci")) == 0) {
        module = HI_PQ_MODULE_DCI;
    } else if (osal_strncmp(arg1, strlen("acm"), "acm", strlen("acm")) == 0) {
        module = HI_PQ_MODULE_ACM;
    } else if (osal_strncmp(arg1, strlen("csc"), "csc", strlen("csc")) == 0) {
        module = HI_PQ_MODULE_CSC;
    } else if (osal_strncmp(arg1, strlen("dei"), "dei", strlen("dei")) == 0) {
        module = HI_PQ_MODULE_DEI;
    } else if (osal_strncmp(arg1, strlen("ds"), "ds", strlen("ds")) == 0) {
        module = HI_PQ_MODULE_DS;
    } else if (osal_strncmp(arg1, strlen("gfxcsc"), "gfxcsc", strlen("gfxcsc")) == 0) {
        module = HI_PQ_MODULE_GFXCSC;
    } else if (osal_strncmp(arg1, strlen("gfxzme"), "gfxzme", strlen("gfxzme")) == 0) {
        module = HI_PQ_MODULE_GFXZME;
    } else if (osal_strncmp(arg1, strlen("vpscene"), "vpscene", strlen("vpscene")) == 0) {
        module = HI_PQ_MODULE_VPSSCSC;
    } else if (osal_strncmp(arg1, strlen("vpsshdr"), "vpsshdr", strlen("vpsshdr")) == 0) {
        module = HI_PQ_MODULE_VPSSHDR;
    } else if (osal_strncmp(arg1, strlen("cle"), "cle", strlen("cle")) == 0) {
        module = HI_PQ_MODULE_CLE;
    } else if (osal_strncmp(arg1, strlen("all"), "all", strlen("all")) == 0) {
        module = HI_PQ_MODULE_ALL;
    }

    *pen_module = module;

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_alg_ctrl_arg2(hi_pq_module_type module, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_u32 strength = 0;
    hi_drv_pq_param param = {0};

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (osal_strncmp(argv[1], strlen("enable"), "enable", strlen("enable")) == 0) {
        drv_pq_set_pq_module(module, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("disable"), "disable", strlen("disable")) == 0) {
        drv_pq_set_pq_module(module, HI_FALSE);
    } else if (osal_strncmp(argv[1], strlen("demoon"), "demoon", strlen("demoon")) == 0) {
        drv_pq_set_demo_en(module, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("demooff"), "demooff", strlen("demooff")) == 0) {
        drv_pq_set_demo_en(module, HI_FALSE);
    } else if (osal_strncmp(argv[1], strlen("debug_on"), "debug_on", strlen("debug_on")) == 0) {
        drv_pq_set_debug_en(module, HI_TRUE);
    } else if (osal_strncmp(argv[1], strlen("debug_off"), "debug_off", strlen("debug_off")) == 0) {
        drv_pq_set_debug_en(module, HI_FALSE);
    } else {
        strength = osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);
        if (strength > PQ_ALG_MAX_VALUE) {
            return HI_FAILURE;
        }

        drv_pq_get_pq_param(&param);
        if (PQ_FUNC_CALL(module, set_strength)) {
            param.strength[module] = strength;
            drv_pq_set_pq_param(&param);
            GET_ALG_FUN(module)->set_strength(strength);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_vptype(hi_char *arg1, hi_drv_pq_vp_type *en_vp_type)
{
    if (osal_strncmp(arg1, strlen("preview"), "preview", strlen("preview")) == 0) {
        *en_vp_type = HI_DRV_PQ_VP_TYPE_PREVIEW;
    } else if (osal_strncmp(arg1, strlen("remote"), "remote", strlen("remote")) == 0) {
        *en_vp_type = HI_DRV_PQ_VP_TYPE_REMOTE;
    } else {
        HI_ERR_PQ("drv_pq_proc_set_vp_type type not match!\n");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_vpmode(hi_drv_pq_vp_type vp_type, hi_char *arg2)
{
    hi_drv_pq_vp_mode vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_MAX;

    if (osal_strncmp(arg2, strlen("recommed"), "recommed", strlen("recommed")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_RECOMMEND;
    } else if (osal_strncmp(arg2, strlen("black"), "black", strlen("black")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_BLACK;
    } else if (osal_strncmp(arg2, strlen("color"), "color", strlen("color")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_COLORFUL;
    } else if (osal_strncmp(arg2, strlen("bt"), "bt", strlen("bt")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_BRIGHT;
    } else if (osal_strncmp(arg2, strlen("warm"), "warm", strlen("warm")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_WARM;
    } else if (osal_strncmp(arg2, strlen("cool"), "cool", strlen("cool")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_COOL;
    } else if (osal_strncmp(arg2, strlen("old"), "old", strlen("old")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_OLD;
    } else if (osal_strncmp(arg2, strlen("user"), "user", strlen("user")) == 0) {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_USER;
    } else {
        vp_scene_mode = HI_DRV_PQ_VIDEOPHONE_MODE_MAX;
    }

    if (vp_type == HI_DRV_PQ_VP_TYPE_PREVIEW) {
        drv_pq_set_vppreview_mode(vp_scene_mode);
    } else if (vp_type == HI_DRV_PQ_VP_TYPE_REMOTE) {
        drv_pq_set_vpremote_mode(vp_scene_mode);
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pq_proc_set_alg_ctrl(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_pq_module_type module = HI_PQ_MODULE_MAX;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    ret = drv_pq_proc_set_alg_ctrl_arg1(argv[0], &module);
    PQ_CHECK_RETURN_SUCCESS(ret);
    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    return drv_pq_proc_set_alg_ctrl_arg2(module, argv);
}

static hi_s32 drv_pq_proc_set_proc_level_ctrl(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 proc_level = 0;
    hi_bool proc_print_level_en = HI_FALSE;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    if (osal_strncmp(argv[0], PQ_PROC_CMD_NAMELENGTH, "proc", PQ_PROC_CMD_NAMELENGTH) == 0) {
        proc_level = (hi_u32)osal_strtol(argv[1], NULL, PQ_ARG_FMT_10);

        PQ_CHECK_OVER_RANGE_RE_FAIL(proc_level, PPQ_ROC_PRINT_MAX);

        g_en_pq_proc_print_level = (pq_proc_print_level)proc_level;
    }

    if (PQ_PROC_PRINT_LEVEL3 == g_en_pq_proc_print_level) {
        proc_print_level_en = HI_TRUE;
    }

    return ret;
}

static hi_s32 drv_pq_proc_set_vptype_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_drv_pq_vp_type vp_type = HI_DRV_PQ_VP_TYPE_MAX;

    if (pq_osal_cmd_check(argc, 2, private, private) != HI_SUCCESS) { /* 2 is para index */
        drv_pq_proc_print_help(argc, argv, private);
        return HI_FAILURE;
    }

    ret = drv_pq_proc_set_vptype(argv[0], &vp_type);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return drv_pq_proc_set_vpmode(vp_type, argv[1]);
}

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: baseparam database operation function
 * Author: wan
 * Create: 2019-12-6
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/memory.h>
#include <linux/bootmem.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/memblock.h>
#include <linux/hikapi.h>

#include "hi_drv_pdm.h"
#include "hi_db.h"
#include "drv_pdm_ext.h"
#include "drv_pdm.h"
#include "hi_drv_sys.h"
#include "linux/hisilicon/securec.h"
#include "hi_osal.h"
#include "linux/hisilicon/hi_drv_mbx.h"
#include "linux/dma-buf.h"
#include "hi_osal.h"

typedef enum {
    MBX_READ_TRNG = 0x4B3C2D1E,
    MBX_LOAD_TEE = 0x4B3C2D2D,
    MBX_LOAD_HRF = 0x4B3C2D3C,
    MBX_LOAD_TPP = 0x4B3C2D4B,
    MBX_LOAD_ADSP = 0x4B3C2D5A,
    MBX_UNLOAD_ADSP = 0x4B3C2D6B,
    MBX_SET_BL33 = 0x4B352D6B,
    MBX_LOAD_LICENSE = 0x4B3C2D69,
    MBX_ACK_MSG = 0x4B3CD22F,
} mailbox_cmd;

typedef enum {
    MBX_ACK_SUCCESS = 0x4B3CD296,
    MBX_CRC_FAILURE = 0xB4C32D5A,
    MBX_ACK_FAILURE = 0xB4C37D5A,
} mailbox_ret;

typedef struct {
    hi_u32 cmd;
    hi_u32 len; /* length of struct */
    hi_u32 low_addr;
    hi_u32 high_addr;
    hi_u32 size; /* length of mmz */
    hi_u32 crc;
} mbx_params;

typedef struct {
    hi_u32 cmd;
    hi_u32 result;
    hi_u32 crc;
} mbx_ack;


#define PDM_DISP_BRIGHTNESS_DEFAULT    50
#define PDM_DISP_CONTRAST_DEFAULT      50
#define PDM_DISP_SATURATION_DEFAULT    50
#define PDM_DISP_HUE_PLUS_DEFAULT      50
#define PDM_DISP_SCREEN_WIDTH_DEFAULT  1280
#define PDM_DISP_SCREEN_HEIGHT_DEFAULT 720
#define PDM_DISP_SCREEN_MAX            3840
#define PDM_DISP_SCREEN_MIN            480
#define PDM_DISP_OFFSET_VALUE          200
#define PDM_VOLUME_MAX_VALUES          100
#define PDM_VOLUME_DEFAULT_VALUES      50
#define PDM_TAG_MAX_LEN                512
#define PDM_MCE_BASE_KEYNAME_LEN       32
#define PDM_INVALID_INTERFACE          0xffffffff
#define PDM_AUD_SINGLE_PORT_MAX_NUM    4
#define LOW_16_BIT_MASK                0xffff
#define LOW_32_BIT_MASK                0xffffffff
#define PDM_MBX_TIMEOUT                100000
#define PDM_FHD_WIDTH                  1920
#define PDM_FHD_HEIGHT                 1080

#if defined(CHIP_TYPE_HI3798MV310)
#define PDM_DISP0_UNSUPPORT
#endif

#define DRV_PDM_LOCK(mutex) \
    do { \
        if (osal_sem_down_interruptible(mutex)) \
        {       \
            HI_ERR_PDM("ERR: pdm lock error!\n"); \
        } \
    } while (0)

#define DRV_PDM_UNLOCK(mutex) \
    do { \
        osal_sem_up(mutex); \
    } while (0)


#define PDM_KEYNAME_LEN    32

static const hi_u32 g_polynomial = 0xEDB88320;
static hi_bool g_volume_exist_in_base = HI_FALSE;
static const ao_port_group g_ao_port_group[] = {
    { HI_AO_PORT_DAC0, "DAC0", "BASE_KEY_SND_DAC", "BASE_KEY_SND_DAC_VOLUME" },

    { HI_AO_PORT_I2S0, "I2S0", "BASE_KEY_SND_I2S", "BASE_KEY_SND_I2S_VOLUME" },
    { HI_AO_PORT_I2S1, "I2S1", "BASE_KEY_SND_I2S", "BASE_KEY_SND_I2S_VOLUME" },
    { HI_AO_PORT_I2S2, "I2S2", "BASE_KEY_SND_I2S", "BASE_KEY_SND_I2S_VOLUME" },
    { HI_AO_PORT_I2S3, "I2S3", "BASE_KEY_SND_I2S", "BASE_KEY_SND_I2S_VOLUME" },

    { HI_AO_PORT_SPDIF0, "SPDIF0", "BASE_KEY_SND_SPDIF", "BASE_KEY_SND_SPDIF_VOLUME" },

    { HI_AO_PORT_HDMI0, "HDMI0", "BASE_KEY_SND_HDMI", "BASE_KEY_SND_HDMI_VOLUME" },
    { HI_AO_PORT_HDMI1, "HDMI1", "BASE_KEY_SND_HDMI", "BASE_KEY_SND_HDMI_VOLUME" },

    { HI_AO_PORT_ARC0, "ARC0", "BASE_KEY_SND_ARC", "BASE_KEY_SND_ARC_VOLUME" },
    { HI_AO_PORT_ARC1, "ARC1", "BASE_KEY_SND_ARC", "BASE_KEY_SND_ARC_VOLUME" },
};

hi_void pdm_get_def_disp_param(hi_drv_display disp, hi_disp_param *disp_param)
{
    hi_s32 i = 0;
    hi_s32 ret = HI_FAILURE;

    if (disp == HI_DRV_DISPLAY_1) {
        disp_param->src_disp           = HI_DRV_DISPLAY_0;
        disp_param->format             = HI_DRV_DISP_FMT_NTSC;
        disp_param->brightness         = PDM_DISP_BRIGHTNESS_DEFAULT;
        disp_param->contrast           = PDM_DISP_CONTRAST_DEFAULT;
        disp_param->saturation         = PDM_DISP_SATURATION_DEFAULT;
        disp_param->hue_plus           = PDM_DISP_HUE_PLUS_DEFAULT;
        disp_param->gamma_enable       = HI_FALSE;
        disp_param->virt_screen_width  = PDM_DISP_SCREEN_WIDTH_DEFAULT;
        disp_param->virt_screen_height = PDM_DISP_SCREEN_HEIGHT_DEFAULT;
        disp_param->disp_enable        = 0;

        ret = memset_s(&(disp_param->offset_info), sizeof(hi_drv_disp_offset), 0x0, sizeof(hi_drv_disp_offset));
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memset_s failed !\n");
            return;
        }

        ret = memset_s(&(disp_param->bg_color), sizeof(hi_drv_disp_color), 0x0, sizeof(hi_drv_disp_color));
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memset_s failed !\n");
            return;
        }

        disp_param->pixel_format = HI_PF_ARG8888;

        for (i = 0; i < HI_DRV_DISP_INTF_TYPE_MAX; i++) {
            disp_param->intf[i].intf_type = PDM_INVALID_INTERFACE;

#ifndef PDM_DISP0_UNSUPPORT
#ifdef HI_DAC_CVBS
            if (i == HI_DRV_DISP_INTF_TYPE_CVBS) {
                disp_param->intf[i].intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;
                disp_param->intf[i].un_intf.stCVBS.u8Dac = HI_DAC_CVBS;
            }
#endif
#endif
        }
    }

    if (disp == HI_DRV_DISPLAY_0) {
        disp_param->src_disp           = HI_DRV_DISPLAY_0;
        disp_param->format             = HI_DRV_DISP_FMT_720P_60;
        disp_param->brightness         = PDM_DISP_BRIGHTNESS_DEFAULT;
        disp_param->contrast           = PDM_DISP_CONTRAST_DEFAULT;
        disp_param->saturation         = PDM_DISP_SATURATION_DEFAULT;
        disp_param->hue_plus           = PDM_DISP_HUE_PLUS_DEFAULT;
        disp_param->gamma_enable       = HI_FALSE;
        disp_param->virt_screen_width  = PDM_DISP_SCREEN_WIDTH_DEFAULT;
        disp_param->virt_screen_height = PDM_DISP_SCREEN_HEIGHT_DEFAULT;
        disp_param->deep_color_mode    = HI_DRV_PIXEL_BITWIDTH_12BIT;
        disp_param->disp_enable        = HI_TRUE;

        ret = memset_s(&(disp_param->offset_info), sizeof(hi_drv_disp_offset), 0x0, sizeof(hi_drv_disp_offset));
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memset_s failed !\n");
            return;
        }

        ret = memset_s(&(disp_param->bg_color), sizeof(hi_drv_disp_color), 0x0, sizeof(hi_drv_disp_color));
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memset_s failed !\n");
            return;
        }

        disp_param->pixel_format = HI_PF_ARG8888;

        for (i = 0; i < HI_DRV_DISP_INTF_TYPE_MAX; i++) {
            switch (i) {
                case HI_DRV_DISP_INTF_TYPE_HDMI:
                    disp_param->intf[i].intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
                    disp_param->intf[i].un_intf.hdmi = HI_DRV_HDMI_ID_0;
                    break;

#if defined(HI_DAC_YPBPR_Y) && defined(HI_DAC_YPBPR_PB) && defined(HI_DAC_YPBPR_PR)
                case HI_DRV_DISP_INTF_TYPE_YPBPR:
                    disp_param->intf[i].intf_type = HI_DRV_DISP_INTF_TYPE_YPBPR;
                    disp_param->intf[i].un_intf.ypbpr.dac_y   = HI_DAC_YPBPR_Y;
                    disp_param->intf[i].un_intf.ypbpr.dac_pb  = HI_DAC_YPBPR_PB;
                    disp_param->intf[i].un_intf.ypbpr.dac_pr  = HI_DAC_YPBPR_PR;
                    break;
#endif

#ifdef PDM_DISP0_UNSUPPORT
#ifdef HI_DAC_CVBS
                case HI_DRV_DISP_INTF_TYPE_CVBS:
                    disp_param->intf[i].intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;
                    disp_param->intf[i].un_intf.cvbs.dac_cvbs = HI_DAC_CVBS;
                    disp_param->format = HI_DRV_DISP_FMT_NTSC;
                    break;
#endif
#endif

                default :
                    disp_param->intf[i].intf_type = PDM_INVALID_INTERFACE;
                    break;
            }
        }
    }
    return;
}

hi_s32 pdm_get_buf_by_name(const hi_char *buf_name, hi_u8 **base_phy_addr, hi_u32 *len)
{
    hi_s32 i;

    for (i = 0; i < g_pdm_global.buf_num; i++) {
        if (0 == osal_strncmp(g_pdm_global.buf_info[i].buf_name, strlen(buf_name), buf_name, strlen(buf_name))) {
            break;
        }
    }

    if (i >= g_pdm_global.buf_num) {
        return HI_FAILURE;
    }

    *base_phy_addr = g_pdm_global.buf_info[i].phy_addr;

    *len = g_pdm_global.buf_info[i].lenth;

    return HI_SUCCESS;
}

hi_s32 pdm_find_buffer_name(const hi_char *buf_name, hi_u32 *index)
{
    hi_u32 i = 0;

    for (i = 0; i < g_pdm_global.buf_num; i++) {
        if (osal_strncmp(g_pdm_global.buf_info[i].buf_name, strlen(buf_name), buf_name, strlen(buf_name)) == 0) {
            break;
        }
    }

    if (i >= g_pdm_global.buf_num) {
        HI_INFO_PDM("can not find buffer:%s\n", buf_name);
        return HI_FAILURE;
    }

    *index = i;
    return HI_SUCCESS;
}

hi_s32 pdm_set_vir_addr_by_name(const hi_char *buf_name, hi_u8 *vir_addr)
{
    hi_u32 i = 0;
    hi_s32 ret;

    DRV_PDM_LOCK(&g_pdm_global.pdm_mutex);

    ret = pdm_find_buffer_name(buf_name, &i);
    if (ret != HI_SUCCESS) {
        DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
        return ret;
    }

    g_pdm_global.buf_info[i].vir_addr = vir_addr;
    DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
    return HI_SUCCESS;
}

hi_s32 pdm_get_vir_addr_by_name(const hi_char *buf_name, hi_u8 **vir_addr, hi_u32 *len)
{
    hi_u32 i = 0;
    hi_s32 ret;

    DRV_PDM_LOCK(&g_pdm_global.pdm_mutex);

    ret = pdm_find_buffer_name(buf_name, &i);
    if (ret != HI_SUCCESS) {
        DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
        return ret;
    }

    *vir_addr = g_pdm_global.buf_info[i].vir_addr;
    *len = g_pdm_global.buf_info[i].lenth;
    DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
    return HI_SUCCESS;
}

hi_s32 pdm_trans_tv_fomat(hi_drv_disp_fmt src_fmt, hi_drv_disp_fmt *hd_fmt, hi_drv_disp_fmt *sd_fmt)
{
    switch (src_fmt) {
        /* bellow are tv display formats */
        case HI_DRV_DISP_FMT_1080P_60: {
            *hd_fmt = HI_DRV_DISP_FMT_1080P_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_1080P_50: {
            *hd_fmt = HI_DRV_DISP_FMT_1080P_50;
            *sd_fmt = HI_DRV_DISP_FMT_PAL;
            break;
        }
        case HI_DRV_DISP_FMT_1080I_60: {
            *hd_fmt = HI_DRV_DISP_FMT_1080I_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_1080I_50: {
            *hd_fmt = HI_DRV_DISP_FMT_1080I_50;
            *sd_fmt = HI_DRV_DISP_FMT_PAL;
            break;
        }
        case HI_DRV_DISP_FMT_720P_60: {
            *hd_fmt = HI_DRV_DISP_FMT_720P_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_720P_50: {
            *hd_fmt = HI_DRV_DISP_FMT_720P_50;
            *sd_fmt = HI_DRV_DISP_FMT_PAL;
            break;
        }
        case HI_DRV_DISP_FMT_576P_50: {
            *hd_fmt = HI_DRV_DISP_FMT_576P_50;
            *sd_fmt = HI_DRV_DISP_FMT_PAL;
            break;
        }
        case HI_DRV_DISP_FMT_480P_60: {
            *hd_fmt = HI_DRV_DISP_FMT_480P_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_PAL: {
            *hd_fmt = HI_DRV_DISP_FMT_PAL;
            *sd_fmt = HI_DRV_DISP_FMT_PAL;
            break;
        }
        case HI_DRV_DISP_FMT_NTSC: {
            *hd_fmt = HI_DRV_DISP_FMT_NTSC;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 pdm_trans_vga_fomat(hi_drv_disp_fmt src_fmt, hi_drv_disp_fmt *hd_fmt, hi_drv_disp_fmt *sd_fmt)
{
    switch (src_fmt) {
        /* bellow are vga display formats */
        case HI_DRV_DISP_FMT_861D_640X480_60: {
            *hd_fmt = HI_DRV_DISP_FMT_861D_640X480_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_800X600_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_800X600_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1024X768_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1024X768_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1280X720_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1280X720_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1280X800_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1280X800_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1280X1024_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1280X1024_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1360X768_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1360X768_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1366X768_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1366X768_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1400X1050_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1400X1050_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1440X900_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1440X900_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1440X900_60_RB: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1440X900_60_RB;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1600X900_60_RB: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1600X900_60_RB;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1600X1200_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1600X1200_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1680X1050_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1680X1050_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }

        case HI_DRV_DISP_FMT_VESA_1920X1080_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1920X1080_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_1920X1200_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_1920X1200_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        case HI_DRV_DISP_FMT_VESA_2048X1152_60: {
            *hd_fmt = HI_DRV_DISP_FMT_VESA_2048X1152_60;
            *sd_fmt = HI_DRV_DISP_FMT_NTSC;
            break;
        }
        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void pdm_trans_fomat(hi_drv_disp_fmt src_fmt, hi_drv_disp_fmt *hd_fmt, hi_drv_disp_fmt *sd_fmt)
{
    hi_s32 ret;

    ret = pdm_trans_tv_fomat(src_fmt, hd_fmt, sd_fmt);
    if (ret == HI_SUCCESS) {
        return;
    }

    ret = pdm_trans_vga_fomat(src_fmt, hd_fmt, sd_fmt);
    if (ret == HI_SUCCESS) {
        return;
    }

    *hd_fmt = HI_DRV_DISP_FMT_1080I_50;
    *sd_fmt = HI_DRV_DISP_FMT_PAL;
    return;
}

hi_s32 pdm_match_format(hi_drv_disp_fmt src_fmt, hi_drv_disp_fmt *dst_fmt)
{
/*
 * old version fmt,  HI_UNF_ENC_FMT_3840X2160_24 = 0x100
 * new version fmt, HI_UNF_ENC_FMT_3840X2160_24 = 0x40
 * need to be adapted to the latest version
 */
    switch ((hi_u32)src_fmt) {
        case PDM_ENC_ORG_FMT_3840X2160_24: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_24;
            break;
        }
        case PDM_ENC_ORG_FMT_3840X2160_25: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_25;
            break;
        }
        case PDM_ENC_ORG_FMT_3840X2160_30: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_30;
            break;
        }
        case PDM_ENC_ORG_FMT_3840X2160_50: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_50;
            break;
        }
        case PDM_ENC_ORG_FMT_3840X2160_60: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_60;
            break;
        }
        case PDM_ENC_ORG_FMT_4096X2160_24: {
            *dst_fmt = HI_DRV_DISP_FMT_4096X2160_24;
            break;
        }
        case PDM_ENC_ORG_FMT_4096X2160_25: {
            *dst_fmt = HI_DRV_DISP_FMT_4096X2160_25;
            break;
        }
        case PDM_ENC_ORG_FMT_4096X2160_30: {
            *dst_fmt = HI_DRV_DISP_FMT_4096X2160_30;
            break;
        }
        case PDM_ENC_ORG_FMT_4096X2160_50: {
            *dst_fmt = HI_DRV_DISP_FMT_4096X2160_50;
            break;
        }
        case PDM_ENC_ORG_FMT_4096X2160_60: {
            *dst_fmt = HI_DRV_DISP_FMT_4096X2160_60;
            break;
        }
        case PDM_ENC_ORG_FMT_3840X2160_23_976: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_23_976;
            break;
        }
        case PDM_ENC_ORG_FMT_3840X2160_29_97: {
            *dst_fmt = HI_DRV_DISP_FMT_3840X2160_29_97;
            break;
        }
        case PDM_ENC_ORG_FMT_720P_59_94: {
            *dst_fmt = HI_DRV_DISP_FMT_720P_59_94;
            break;
        }
        case PDM_ENC_ORG_FMT_1080P_59_94: {
            *dst_fmt = HI_DRV_DISP_FMT_1080P_59_94;
            break;
        }
        case PDM_ENC_ORG_FMT_1080P_29_97: {
            *dst_fmt = HI_DRV_DISP_FMT_1080P_29_97;
            break;
        }
        case PDM_ENC_ORG_FMT_1080P_23_976: {
            *dst_fmt = HI_DRV_DISP_FMT_1080P_23_976;
            break;
        }
        case PDM_ENC_ORG_FMT_1080i_59_94: {
            *dst_fmt = HI_DRV_DISP_FMT_1080I_59_94;
            break;
        }
        default: {
            *dst_fmt = src_fmt;
            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 pdm_read_version_info(hi_db *db, hi_u32 *version)
{
    hi_s32          ret;
    hi_db_table     table = {0};
    hi_db_key       key = {{0}};

    ret = hi_db_get_table_by_name(db, MCE_BASE_TABLENAME_VERSION, &table);
    if (ret != HI_SUCCESS) {
        HI_INFO_PDM("table: MCE_BASE_TABLENAME_VERSION is not found\n");
        return HI_FAILURE;
    }

    ret = hi_db_get_key_by_name(&table, MCE_BASE_KEYNAME_UNF_VERSION, &key);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    *version = *(hi_u32 *)key.value;

    return HI_SUCCESS;
}

static hi_s32 drv_pdm_get_custom_timing(hi_db_table *table, hi_drv_disp_timing *timing)
{
    hi_s32    ret;
    hi_db_key key = {{0}};

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_VFB, &key);
    if (ret == HI_SUCCESS) {
        timing->vfb = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_VBB, &key);
    if (ret == HI_SUCCESS) {
        timing->vbb = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_VACT, &key);
    if (ret == HI_SUCCESS) {
        timing->vact = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_HFB, &key);
    if (ret == HI_SUCCESS) {
        timing->hfb = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_HBB, &key);
    if (ret == HI_SUCCESS) {
        timing->hbb = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_HACT, &key);
    if (ret == HI_SUCCESS) {
        timing->hact = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_VPW, &key);
    if (ret == HI_SUCCESS) {
        timing->vpw = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_HPW, &key);
    if (ret == HI_SUCCESS) {
        timing->hpw = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_IDV, &key);
    if (ret == HI_SUCCESS) {
        timing->idv = *(hi_bool *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_IHS, &key);
    if (ret == HI_SUCCESS) {
        timing->ihs = *(hi_bool *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_IVS, &key);
    if (ret == HI_SUCCESS) {
        timing->ivs = *(hi_bool *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_CLKR, &key);
    if (ret == HI_SUCCESS) {
        timing->clkreversal = *(hi_bool *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_FIXF, &key);
    if (ret == HI_SUCCESS) {
        timing->pix_freq = *(hi_u32 *)(key.value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_TIMING_VERTF, &key);
    if (ret == HI_SUCCESS) {
        timing->refresh_rate = *(hi_u32 *)(key.value);
    }

    return HI_SUCCESS;
}

hi_void pdm_get_disp_intf_param(hi_disp_param *disp_param, hi_db_key *key, hi_db_table *table)
{
    hi_s32 ret;

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_HDMI, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_HDMI] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_YPBPR, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_YPBPR] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_CVBS, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_CVBS] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_RGB, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_RGB] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_SVIDEO, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_SVIDEO] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_BT1120, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_BT1120] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_BT656, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_BT656] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_LCD, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_LCD] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_VGA, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_VGA] = *(hi_drv_disp_intf *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_INTF_MIPI, key);
    if (ret == HI_SUCCESS) {
        disp_param->intf[HI_DRV_DISP_INTF_TYPE_MIPI] = *(hi_drv_disp_intf *)(key->value);
    }

    return;
}

hi_void pdm_get_disp_color_and_blight_param(hi_disp_param *disp_param,
                                            hi_db_key *key,
                                            hi_db_table *table)
{
    hi_s32 ret;

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_HULEP, key);
    if (ret == HI_SUCCESS) {
        disp_param->hue_plus = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_SATU, key);
    if (ret == HI_SUCCESS) {
        disp_param->saturation = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_CONTR, key);
    if (ret == HI_SUCCESS) {
        disp_param->contrast = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_BRIG, key);
    if (ret == HI_SUCCESS) {
        disp_param->brightness = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_BGCOLOR, key);
    if (ret == HI_SUCCESS) {
        disp_param->bg_color = *(hi_drv_disp_color *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_GAMA, key);
    if (ret == HI_SUCCESS) {
        disp_param->gamma_enable = *(hi_bool *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_ASPECT, key);
    if (ret == HI_SUCCESS) {
        disp_param->aspect_ratio = *(hi_drv_aspect_ratio *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_SRC_DISP, key);
    if (ret == HI_SUCCESS) {
        disp_param->src_disp = *(hi_drv_display*)(key->value);
    }

    return;
}

hi_void pdm_get_disp_rect_param(hi_disp_param *disp_param, hi_db_key *key, hi_db_table *table)
{
    hi_s32 ret ;

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_VIRSCW, key);
    if (ret == HI_SUCCESS) {
        disp_param->virt_screen_width = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_VIRSCH, key);
    if (ret == HI_SUCCESS) {
        disp_param->virt_screen_height = *(hi_u32 *)(key->value);
    }

    if ((disp_param->virt_screen_width > PDM_DISP_SCREEN_MAX) ||
        (disp_param->virt_screen_width < PDM_DISP_SCREEN_MIN) ||
        (disp_param->virt_screen_height > PDM_DISP_SCREEN_MAX) ||
        (disp_param->virt_screen_height < PDM_DISP_SCREEN_MIN)) {
        HI_INFO_PDM("invalid vir screen:W %d, H %d\n", disp_param->virt_screen_width, disp_param->virt_screen_height);
        disp_param->virt_screen_width = PDM_DISP_SCREEN_WIDTH_DEFAULT;
        disp_param->virt_screen_height = PDM_DISP_SCREEN_HEIGHT_DEFAULT;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_DISP_L, key);
    if (ret == HI_SUCCESS) {
        disp_param->offset_info.left = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_DISP_T, key);
    if (ret == HI_SUCCESS) {
        disp_param->offset_info.top = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_DISP_R, key);
    if (ret == HI_SUCCESS) {
        disp_param->offset_info.right = *(hi_u32 *)(key->value);
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_DISP_B, key);
    if (ret == HI_SUCCESS) {
        disp_param->offset_info.bottom = *(hi_u32 *)(key->value);
    }

    if ((disp_param->offset_info.left > PDM_DISP_OFFSET_VALUE) ||
        (disp_param->offset_info.top > PDM_DISP_OFFSET_VALUE) ||
        (disp_param->offset_info.right > PDM_DISP_OFFSET_VALUE) ||
        (disp_param->offset_info.bottom > PDM_DISP_OFFSET_VALUE)) {
        HI_INFO_PDM("invalid offset:T %d, B %d, L %d, R %d\n", disp_param->offset_info.top,
                    disp_param->offset_info.bottom, disp_param->offset_info.left,
                    disp_param->offset_info.right);

        disp_param->offset_info.left = 0;
        disp_param->offset_info.top = 0;
        disp_param->offset_info.right = 0;
        disp_param->offset_info.bottom = 0;
    }

    return;
}

hi_void pdm_get_mipi_param(hi_disp_param *disp_param, hi_db_key *key, hi_db_table *table)
{
    hi_s32 ret;
    hi_u32 i;
    hi_char mipi_buf[PDM_MCE_BASE_KEYNAME_LEN] = {0};
    hi_u64 mipi_lane_id_buf;

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_LANEID, key);
    if (ret == HI_SUCCESS) {
        mipi_lane_id_buf = *(hi_u64 *)(key->value);
        disp_param->mipi_pdm_attr.lane_id[3] = (mipi_lane_id_buf >> 48) & LOW_16_BIT_MASK; /* shift 48 for lane 3 */
        disp_param->mipi_pdm_attr.lane_id[2] = (mipi_lane_id_buf >> 32) & LOW_16_BIT_MASK; /* shift 32 for lane 2 */
        disp_param->mipi_pdm_attr.lane_id[1] = (mipi_lane_id_buf >> 16) & LOW_16_BIT_MASK; /* shift 16 for lane 1 */
        disp_param->mipi_pdm_attr.lane_id[0] = (mipi_lane_id_buf >> 0) & LOW_16_BIT_MASK;
    } else {
        mipi_lane_id_buf = 0;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_OUT_MODE, key);
    if (ret == HI_SUCCESS) {
        disp_param->mipi_pdm_attr.output_mode = *(hi_mipi_output_mode *)(key->value);
    } else {
        disp_param->mipi_pdm_attr.output_mode = OUTPUT_MODE_CSI;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_VID_MODE, key);
    if (ret == HI_SUCCESS) {
        disp_param->mipi_pdm_attr.video_mode = *(hi_mipi_video_mode *)(key->value);
    } else {
        disp_param->mipi_pdm_attr.video_mode = BURST_MODE;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_MODE, key);
    if (ret == HI_SUCCESS) {
        disp_param->mipi_pdm_attr.mipi_mode = *(hi_drv_mipi_mode *)(key->value);
    } else {
        disp_param->mipi_pdm_attr.mipi_mode = HI_DRV_MIPI_MODE_SINGLE;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_BPP, key);
    if (ret == HI_SUCCESS) {
        disp_param->mipi_pdm_attr.bpp = *(hi_u32 *)(key->value);
    } else {
        disp_param->mipi_pdm_attr.bpp = 0;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_BRI, key);
    if (ret == HI_SUCCESS) {
        disp_param->mipi_pdm_attr.bri_percent = *(hi_u32 *)(key->value);
    } else {
        disp_param->mipi_pdm_attr.bri_percent = 0;
    }

    ret = hi_db_get_key_by_name(table, MCE_BASE_KEYNAME_MIPI_CMD_CNT, key);
    if (ret == HI_SUCCESS) {
        disp_param->mipi_pdm_attr.cmd_count = *(hi_u32 *)(key->value);
    } else {
        disp_param->mipi_pdm_attr.cmd_count = 0;
    }

    ret = memset_s(mipi_buf, PDM_MCE_BASE_KEYNAME_LEN, 0, PDM_MCE_BASE_KEYNAME_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_PDM("init mipi buf fail\n");
    }

    for (i = 0; i < disp_param->mipi_pdm_attr.cmd_count; i++) {
        ret = snprintf(mipi_buf, PDM_MCE_BASE_KEYNAME_LEN, "%s%03d", MCE_BASE_KEYNAME_MIPI_PANEL_CMD, i);
        if (ret < 0) {
            HI_ERR_PDM("snprintf mipi buf fail\n");
        }

        ret = hi_db_get_key_by_name(table, mipi_buf, key);
        if (ret == HI_SUCCESS) {
            disp_param->mipi_pdm_attr.cmds[i] = *(panel_cmd *)(key->value);
        }
    }

    return;
}

hi_s32 drv_pdm_get_disp_param(hi_drv_display disp, hi_disp_param *disp_param)
{
    hi_s32                ret;
    hi_db                 base_db = {0};
    hi_db_table           table = {0};
    hi_db_key             key = {{0}};
    hi_u8                 *base_phy_addr = HI_NULL;
    hi_u8                 *base_vir_addr = HI_NULL;
    hi_u32                base_len;
    hi_u32                version;
    hi_drv_disp_fmt      fmt;

    if ((pdm_get_vir_addr_by_name(PDM_BASEPARAM_BUFNAME, &base_vir_addr, &base_len) != HI_SUCCESS) ||
        (base_vir_addr == HI_NULL)) {
        ret = pdm_get_buf_by_name(PDM_BASEPARAM_BUFNAME, &base_phy_addr, &base_len);
        if (ret != HI_SUCCESS) {
            /*  if there is no baseparam, use default */
            HI_WARN_PDM("ERR: hi_db_get_db_from_mem, use default baseparam!\n");

            pdm_get_def_disp_param(disp, disp_param);
            return HI_SUCCESS;
        }

        base_vir_addr = osal_blockmem_vmap((hi_u64)base_phy_addr, base_len);
        pdm_set_vir_addr_by_name(PDM_BASEPARAM_BUFNAME, base_vir_addr);
    }

    pdm_get_def_disp_param(disp, disp_param);

    ret = hi_db_get_db_from_mem((hi_u8 *)base_vir_addr, &base_db);
    if (ret != HI_SUCCESS) {
        HI_WARN_PDM("ERR: hi_db_get_db_from_mem, use default baseparam!\n");

        pdm_get_def_disp_param(disp, disp_param);
        return HI_SUCCESS;
    }

    if (disp == HI_DRV_DISPLAY_0) {
        ret = hi_db_get_table_by_name(&base_db, MCE_BASE_TABLENAME_DISP0, &table);
    } else {
        ret = hi_db_get_table_by_name(&base_db, MCE_BASE_TABLENAME_DISP1, &table);
    }

    if (ret != HI_SUCCESS) {
        HI_INFO_PDM("ERR: hi_db_get_table_by_name, use default baseparam!\n");
        return HI_SUCCESS;
    }

    ret = hi_db_get_key_by_name(&table, MCE_BASE_KEYNAME_FMT, &key);
    if (ret == HI_SUCCESS) {
        disp_param->format = *(hi_drv_disp_fmt *)(key.value);
        if (disp_param->format == HI_DRV_DISP_FMT_CUSTOM) {
            ret = drv_pdm_get_custom_timing(&table, &(disp_param->disp_timing));
        }

        if (disp == HI_DRV_DISPLAY_0) {
           /*
            * if there is not a UNF version number on the baseparam image
            * convert format, HI_UNF_ENC_FMT_3840X2160_24...hi_unf_enc_fmt_1080i_59_94
            */
            if ((pdm_read_version_info(&base_db, &version) == HI_FAILURE) &&
                (pdm_match_format(disp_param->format, &fmt) == HI_SUCCESS)) {
                    disp_param->format = fmt;
            }
        }
    }

    ret = hi_db_get_key_by_name(&table, MCE_BASE_KEYNAME_ENABLE, &key);
    if (ret == HI_SUCCESS) {
        disp_param->disp_enable = *(hi_u32 *)(key.value);
    } else {
        disp_param->disp_enable = 0;
    }

    pdm_get_disp_intf_param(disp_param, &key, &table);

    pdm_get_disp_color_and_blight_param(disp_param, &key, &table);

    pdm_get_disp_rect_param(disp_param, &key, &table);

    if ((disp == HI_DRV_DISPLAY_1) && ((disp_param->virt_screen_width > PDM_FHD_WIDTH) ||
        (disp_param->virt_screen_height > PDM_FHD_HEIGHT))) {
        HI_ERR_PDM("invalid vir screen:W %d, H %d\n", disp_param->virt_screen_width, disp_param->virt_screen_height);
        disp_param->virt_screen_width = PDM_FHD_WIDTH;
        disp_param->virt_screen_height = PDM_FHD_HEIGHT;
    }

    pdm_get_mipi_param(disp_param, &key, &table);

    ret = hi_db_get_key_by_name(&table, MCE_BASE_KEYNAME_PF, &key);
    if (ret == HI_SUCCESS) {
        disp_param->pixel_format = *(hi_pixel_format *)(key.value);
        if ((disp_param->pixel_format < HI_PF_ARG8888) || (disp_param->pixel_format >= HI_PF_MAX)) {
            HI_INFO_PDM("invalid pixel_format:%d\n", disp_param->pixel_format);

            disp_param->pixel_format = HI_PF_ARG8888;
        }
    } else {
        disp_param->pixel_format = HI_PF_ARG8888;
    }

    ret = hi_db_get_key_by_name(&table, MCE_BASE_KEYNAME_HDMI_VIDEOMODE, &key);
    if (ret == HI_SUCCESS) {
        disp_param->vid_out_mode = *(hi_drv_disp_pix_format *)(key.value);
    }
    ret = hi_db_get_key_by_name(&table, MCE_BASE_KEYNAME_HDMI_DEEPCOLOR, &key);
    if (ret == HI_SUCCESS) {
        disp_param->deep_color_mode = *(hi_drv_pixel_bitwidth *)(key.value);
    }
    return HI_SUCCESS;
}

hi_s32 drv_pdm_get_data(const hi_char *buf_name, hi_u8 **data_addr, hi_u32 *data_len)
{
    hi_s32      ret = HI_SUCCESS;
    hi_u8       *phy_addr = HI_NULL;

    if ((buf_name != HI_NULL) &&
        (data_addr != HI_NULL) &&
        (data_len != HI_NULL)) {
        if ((pdm_get_vir_addr_by_name(buf_name, data_addr, data_len) != HI_SUCCESS) ||
                                      (*data_addr == HI_NULL)) {
            ret = pdm_get_buf_by_name(buf_name, &phy_addr, data_len);
            if (ret != HI_SUCCESS) {
                return ret;
            }

            *data_addr = osal_blockmem_vmap((hi_u64)phy_addr, *data_len);

            return pdm_set_vir_addr_by_name(buf_name, *data_addr);
        } else {
            return HI_SUCCESS;
        }
    } else {
        return HI_FAILURE;
    }
}

hi_s32 pdm_get_key_name_by_snd_output_port(hi_ao_port snd_output_port,
                                           hi_char *name, hi_u32 name_len,
                                           hi_char *name_vol, hi_u32 name_vol_len)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 i;

    for (i = 0; i < sizeof(g_ao_port_group) / sizeof(g_ao_port_group[0]); i++) {
        if (g_ao_port_group[i].ao_port == snd_output_port) {
            ret = memcpy_s(name, name_len, g_ao_port_group[i].key_name, strlen(g_ao_port_group[i].key_name));
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("memcpy get %s key fail!\n", g_ao_port_group[i].key_name);
                return HI_FAILURE;
            }

            ret = memcpy_s(name_vol, name_vol_len, g_ao_port_group[i].key_volume,
                strlen(g_ao_port_group[i].key_volume));
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("memcpy get %s key fail!\n", g_ao_port_group[i].key_volume);
                return HI_FAILURE;
            }

            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

hi_s32 pdm_get_sound(hi_db_table *table, hi_ao_port sound_output_port,
                     hi_db_key *key, hi_drv_pdm_sound_param *sound_param, hi_u32 *volume)
{
    hi_s32  ret;
    hi_char key_name[PDM_KEYNAME_LEN] = {0};
    hi_char key_name_vol[PDM_KEYNAME_LEN] = {0};
    hi_char key_name_joint[PDM_KEYNAME_LEN] = {0};
    hi_u32 i;

    ret = pdm_get_key_name_by_snd_output_port(sound_output_port, key_name, PDM_KEYNAME_LEN,
        key_name_vol, PDM_KEYNAME_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_PDM("cannot get name by snd port\n");
        return ret;
    }

    for (i = 0; i < PDM_AUD_SINGLE_PORT_MAX_NUM; i++) {
        ret = snprintf_s(key_name_joint, PDM_KEYNAME_LEN, PDM_KEYNAME_LEN - 1, "%s%d", key_name, i);
        if (ret < 0) {
            HI_ERR_PDM("cannot generate joint base key name:%x\n", ret);
            return ret;
        }

        ret = hi_db_get_key_by_name(table, key_name_joint, key);
        if ((ret == HI_SUCCESS) && (*(hi_ao_port *)(key->value) == sound_output_port)) {
            sound_param->out_port[sound_param->port_num].port = *(hi_ao_port *)(key->value);
            if ((sound_output_port == HI_AO_PORT_I2S0) ||
                (sound_output_port == HI_AO_PORT_I2S1) ||
                (sound_output_port == HI_AO_PORT_I2S2) ||
                (sound_output_port == HI_AO_PORT_I2S3)) {
                sound_param->out_port[sound_param->port_num].un_attr.i2s_attr =
                    *(hi_ao_i2s_attr *)(key->value + sizeof(hi_ao_port));
            }

            memset_s(key_name_joint, sizeof(key_name_joint), 0, sizeof(key_name_joint));
            ret = snprintf_s(key_name_joint, PDM_KEYNAME_LEN, PDM_KEYNAME_LEN - 1, "%s%d", key_name_vol, i);
            if (ret < 0) {
                HI_ERR_PDM("cannot generate joint base key name:%x\n", ret);
                return ret;
            }
            ret = hi_db_get_key_by_name(table, key_name_joint, key);
            if (ret == HI_SUCCESS) {
                *volume = *(hi_u32 *)(key->value);
                if (*volume > PDM_VOLUME_MAX_VALUES) {
                    *volume = PDM_VOLUME_DEFAULT_VALUES;
                }
                g_volume_exist_in_base = HI_TRUE;
            } else {
                *volume = PDM_VOLUME_DEFAULT_VALUES;
                g_volume_exist_in_base = HI_FALSE;
            }

            sound_param->audio_volume[sound_param->port_num] = *volume;
            sound_param->port_num++;

            break;
        }
    }

    if (i >= PDM_AUD_SINGLE_PORT_MAX_NUM) {
        HI_INFO_PDM("cannot find key corresponded to a certain port,port:%x\n", sound_output_port);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void pdm_get_sound_param(hi_drv_pdm_sound_param *sound_param,
                                   hi_db_table *table, hi_db_key *key, hi_u32 *volume)
{
    hi_s32 ret;
    hi_u32 i;

    for (i = 0; i < (sizeof(g_ao_port_group) / sizeof(g_ao_port_group[0])); i++) {
        ret = pdm_get_sound(table, g_ao_port_group[i].ao_port, key, sound_param, volume);
        if (ret != HI_SUCCESS) {
            HI_INFO_PDM("get sound port %s failed! ret = 0x%x\n", g_ao_port_group[i].name, ret);
        }
    }
}

hi_s32 drv_pdm_get_sound_param(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param)
{
    hi_s32          ret;
    hi_db           base_db = {0};
    hi_db_table     table = {0};
    hi_db_key       key = {{0}};
    hi_u8           *base_phy_addr = HI_NULL;
    hi_u8           *base_vir_addr = HI_NULL;
    hi_u32          base_len;
    hi_u32          volume;
    hi_char         *table_name = HI_NULL;

    if ((sound >= AO_SND_MAX) || (sound_param == HI_NULL)) {
        return HI_FAILURE;
    }

    if ((pdm_get_vir_addr_by_name(PDM_BASEPARAM_BUFNAME, &base_vir_addr, &base_len) != HI_SUCCESS) ||
                                  (base_vir_addr == HI_NULL)) {
        ret = pdm_get_buf_by_name(PDM_BASEPARAM_BUFNAME, &base_phy_addr, &base_len);
        if (ret != HI_SUCCESS) {
            return ret;
        }

        base_vir_addr = osal_blockmem_vmap((hi_u64)base_phy_addr, base_len);
        pdm_set_vir_addr_by_name(PDM_BASEPARAM_BUFNAME, base_vir_addr);
    }

    ret = hi_db_get_db_from_mem((hi_u8 *)base_vir_addr, &base_db);
    if (ret != HI_SUCCESS) {
        HI_INFO_PDM("ERR: hi_db_get_db_from_mem, use default baseparam!\n");
        return HI_FAILURE;
    }

    if (sound == AO_SND_0) {
        table_name = MCE_BASE_TABLENAME_SOUND0;
    } else if (sound == AO_SND_1) {
        table_name = MCE_BASE_TABLENAME_SOUND1;
    } else {
        table_name = MCE_BASE_TABLENAME_SOUND2;
    }

    ret = hi_db_get_table_by_name(&base_db, table_name, &table);
    if (ret != HI_SUCCESS) {
        HI_INFO_PDM("ERR: hi_db_get_table_by_name, use default baseparam!\n");
        return HI_FAILURE;
    }

    sound_param->port_num = 0;

    pdm_get_sound_param(sound_param, &table, &key, &volume);

    return HI_SUCCESS;
}

static hi_s32 drv_pdm_tag_remove_buff(const hi_char *buf_name)
{
    hi_char pdm_tag[PDM_TAG_MAX_LEN] = {0};
    hi_u32  pdm_len;
    hi_char *p = HI_NULL;
    hi_char *q = HI_NULL;
    hi_u32  ii = 0;
    hi_u32  len = 0;

    pdm_len = get_param_data("pdm_tag", pdm_tag, PDM_TAG_MAX_LEN);
    if (pdm_len >= PDM_TAG_MAX_LEN) {
        return HI_FAILURE;
    }

    pdm_tag[PDM_TAG_MAX_LEN - 1] = '\0';
    p = strstr(pdm_tag, buf_name);

    if (p == HI_NULL) {
        return HI_FAILURE;
    }

    q = strstr(p, " ");
    if (q == HI_NULL) {
        *p = '\0';
    } else {
        q++;
        len = strlen(pdm_tag);
        for (ii = 0; ii < (len - (q - pdm_tag)); ii++) {
            p[ii] = q[ii];
        }

        p[ii] = '\0';
    }

    set_param_data("pdm_tag", pdm_tag, strlen(pdm_tag) + 1);

    return HI_SUCCESS;
}

hi_s32 drv_pdm_release_reserve_mem(const hi_char *buf_name)
{
    hi_u32 i = 0;
    hi_s32 ret;

    DRV_PDM_LOCK(&g_pdm_global.pdm_mutex);

    ret = pdm_find_buffer_name(buf_name, &i);
    if (ret != HI_SUCCESS) {
        DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
        return ret;
    }

    if (g_pdm_global.buf_info[i].release) {
        DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
        return HI_SUCCESS;
    }

    g_pdm_global.buf_info[i].release = HI_TRUE;

    if (g_pdm_global.buf_info[i].vir_addr != HI_NULL) {
        osal_blockmem_vunmap((hi_u8 *)(g_pdm_global.buf_info[i].vir_addr));

        g_pdm_global.buf_info[i].vir_addr = HI_NULL;
    }

    DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);

    /*
     * this function maybe block, can not lock
     * for 64bit system, function first para must be merge to 64 bit, cant hi_u32 type
     */
    pdm_free_reserve_mem((phys_addr_t)(uintptr_t)g_pdm_global.buf_info[i].phy_addr,
                         g_pdm_global.buf_info[i].lenth);

    ret = drv_pdm_tag_remove_buff(buf_name);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_pdm_get_reserve_mem(const hi_char *buf_name, hi_u32 *resever_addr,
                                      hi_u32 *resever_buf_size)
{
    hi_u32 buf_index = 0;
    hi_s32 ret;

    DRV_PDM_LOCK(&g_pdm_global.pdm_mutex);

    ret = pdm_find_buffer_name(buf_name, &buf_index);
    if (ret != HI_SUCCESS) {
        DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
        return ret;
    }

    if (g_pdm_global.buf_info[buf_index].release) {
        DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);
        return HI_SUCCESS;
    }

    DRV_PDM_UNLOCK(&g_pdm_global.pdm_mutex);

    *resever_addr = (phys_addr_t)(uintptr_t)g_pdm_global.buf_info[buf_index].phy_addr;
    *resever_buf_size = g_pdm_global.buf_info[buf_index].lenth;

    return HI_SUCCESS;
}

hi_u32 drv_pdm_crc32_bitwise(const void *input_data, hi_u32 length, hi_u32 previous_crc32)
{
    hi_u32 crc = ~previous_crc32;
    hi_u8 *current_data = (hi_u8 *) input_data;
    hi_u32 j;

    while (length--) {
        crc ^= *current_data++;
        for (j = 0; j < 8; j++) { /* for an 8 bits char, do crc calculate every bit */
            crc = (crc >> 1) ^ (-(hi_s32)(crc & 1) & g_polynomial);
        }
    }
    return ~crc; /* same as crc ^ 0xFFFFFFFF */
}

hi_s32 drv_pdm_set_license_data(hi_u64 lic_mmz_handle, hi_u32 lic_len)
{
    hi_u8 *lic_dma_addr = HI_NULL;
    hi_u64 lic_phy_addr;
    hi_u32 mbx_handle;
    hi_s32 ret;
    mbx_params send_param = {0};
    hi_u32 crc_result;
    hi_u32 tx_len = 0;
    hi_u32 rx_len = 0;

    hi_u8  rx_buf[20] = {0}; /* 20 bytes makes room enough */
    mbx_ack rcv_mbx = {0};

    lic_dma_addr = osal_mem_handle_get(lic_mmz_handle, HI_ID_PDM);
    if (lic_dma_addr == HI_NULL) {
        HI_ERR_PDM("cannot get mmz dma addr\n");
        return HI_FAILURE;
    }

    lic_phy_addr = osal_mem_phys(lic_dma_addr);
    if (lic_phy_addr == 0) {
        HI_ERR_PDM("cannot get mmz phy addr\n");
        return HI_FAILURE;
    }

    mbx_handle = hi_drv_mbx_open(HI_MBX_ACPU2HRF_BOOT);
    if (mbx_handle < 0) {
        HI_ERR_PDM("cannot open mbx\n");
        return HI_FAILURE;
    }

    send_param.cmd = MBX_LOAD_LICENSE;
    send_param.high_addr = ((lic_phy_addr >> 32) & LOW_32_BIT_MASK); /* R shift 32bits to get high addr */
    send_param.low_addr = (lic_phy_addr & LOW_32_BIT_MASK);
    send_param.size = lic_len;
    send_param.len = sizeof(mbx_params);

    crc_result = drv_pdm_crc32_bitwise(&send_param, sizeof(send_param) - sizeof(hi_u32), 0);
    send_param.crc = crc_result;

    ret = hi_drv_mbx_tx(mbx_handle, (hi_void *)&send_param, sizeof(send_param), &tx_len, PDM_MBX_TIMEOUT);
    if (ret != HI_SUCCESS) {
        HI_ERR_PDM("cannot send mbx : %x\n", ret);
        return ret;
    }

    ret = hi_drv_mbx_rx(mbx_handle, rx_buf, sizeof(mbx_ack), &rx_len, PDM_MBX_TIMEOUT);
    if (ret != HI_SUCCESS) {
        HI_ERR_PDM("cannot recive mbx : %x\n", ret);
        return ret;
    }

    ret = hi_drv_mbx_close(mbx_handle);
    if (ret < 0) {
        HI_ERR_PDM("cannot close mbx:%x\n", ret);
        return ret;
    }

    ret = memcpy_s(&rcv_mbx, sizeof(rcv_mbx), rx_buf, sizeof(rcv_mbx));
    if (ret != HI_SUCCESS) {
        HI_ERR_PDM("cannot cpy mbx data: %x\n", ret);
        return ret;
    }

    if (rcv_mbx.crc != drv_pdm_crc32_bitwise(&rcv_mbx, sizeof(rcv_mbx) - sizeof(hi_u32), 0)) {
        HI_ERR_PDM("recive mbx data has beem tainted!\n", ret);
        return ret;
    }

    if (rcv_mbx.cmd != MBX_ACK_MSG) {
        HI_ERR_PDM("recive mbx data invalid :%x\n", rcv_mbx.cmd);
        return ret;
    }

    if (rcv_mbx.result == MBX_ACK_SUCCESS) {
        return HI_SUCCESS;
    } else {
        return rcv_mbx.result;
    }
}

hi_s32 hi_drv_pdm_get_disp_param(hi_drv_display disp, hi_disp_param *disp_param)
{
    if (disp_param == HI_NULL) {
        HI_ERR_PDM("disp_param is null!\n");
        return HI_ERR_PDM_PTR_NULL;
    }

    return drv_pdm_get_disp_param(disp, disp_param);
}

hi_s32 hi_drv_pdm_get_sound_param(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param)
{
    if (sound_param == HI_NULL) {
        HI_ERR_PDM("sound_param is null!\n");
        return HI_ERR_PDM_PTR_NULL;
    }

    return drv_pdm_get_sound_param(sound, sound_param);
}

hi_s32 hi_drv_pdm_get_sound_param_ex(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param,
                                     hi_bool *volume_exist_in_base)
{
    hi_s32 ret;

    if ((volume_exist_in_base == HI_NULL) || (sound_param == HI_NULL)) {
        HI_ERR_PDM("ptr is null!\n");
        return HI_ERR_PDM_PTR_NULL;
    }

    ret = drv_pdm_get_sound_param(sound, sound_param);

    *volume_exist_in_base = g_volume_exist_in_base;

    return ret;
}

hi_s32 hi_drv_pdm_release_reserve_mem(const hi_char *buf_name)
{
    if (buf_name == HI_NULL) {
        HI_ERR_PDM("buf_name is null!\n");
        return HI_ERR_PDM_PTR_NULL;
    }

    return drv_pdm_release_reserve_mem(buf_name);
}

hi_s32 hi_drv_pdm_get_reserve_mem(const hi_char *buf_name, hi_u32 *resever_addr, hi_u32 *resever_buf_size)
{
    if ((buf_name == NULL) || (resever_addr == NULL) || (resever_buf_size == NULL)) {
        HI_ERR_PDM("ptr is null!\n");
        return HI_FAILURE;
    }

    return drv_pdm_get_reserve_mem(buf_name, resever_addr, resever_buf_size);
}

hi_s32 hi_drv_pdm_get_data(const hi_char *buf_name, hi_u8 **data_addr, hi_u32 *data_len)
{
    if ((data_addr == HI_NULL) || (buf_name == HI_NULL) || (data_len == HI_NULL)) {
        HI_ERR_PDM("ptr is null!\n");
        return HI_FAILURE;
    }

    return drv_pdm_get_data(buf_name, data_addr, data_len);
}

hi_s32 hi_drv_pdm_set_license_data(hi_u64 lic_handle, hi_u32 lic_len)
{
    if (lic_handle == HI_INVALID_HANDLE || lic_len == 0) {
        HI_ERR_PDM("input param is invalid!\n");
        return HI_ERR_PDM_PARAM_INVALID;
    }

    return drv_pdm_set_license_data(lic_handle, lic_len);
}


EXPORT_SYMBOL(hi_drv_pdm_get_disp_param);
EXPORT_SYMBOL(hi_drv_pdm_get_sound_param);

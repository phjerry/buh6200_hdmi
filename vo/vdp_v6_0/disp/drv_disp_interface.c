/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: vdp
 * Create: 2019-04-12
 */

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"

#include "drv_display.h"
#include "drv_disp_interface.h"
#include "hi_drv_disp.h"
#include "drv_disp_ext.h"
#include "drv_hdmi_module_ext.h"
#include "drv_mipi_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* local function and macro */
typedef struct {
    hi_bool attached;
    hi_bool enable;
    hi_bool matching;
    hi_bool is_support;
    disp_intf_func func_intf_ctrl;
    disp_intf intf_attr;
    hi_drv_pixel_bitwidth bit_width;
    hi_drv_disp_pix_format pixel_fmt;
} disp_intf_status;

typedef struct {
    disp_intf_status intf_status[HI_DRV_DISP_INTF_ID_MAX];
    hi_drv_disp_fmt disp_fmt;
} disp_intf_channel;

typedef struct {
    disp_intf_channel disp_intf_chn[HI_DRV_DISPLAY_BUTT];
    hi_bool is_vdac_attached[HI_DISP_VDAC_MAX_NUMBER];
    hi_bool is_intf_attached[HI_DRV_DISP_INTF_ID_MAX];
} disp_inttf_mng;

typedef enum {
    DISP_INTF_SET = 0,
    DISP_INTF_CLEAR = 1,
} disp_intf_mode;

#define DISP_INTF_FORMAT_HFB 8
#define DISP_HTOTAL_VTOTAL_MAX 0x1FFFFFFF

disp_inttf_mng g_disp_intf_mng;

#define GET_INTF_CHANNEL_BY_ID(id, ptr)                                        \
    do {                                                                       \
        if (id >= HI_DRV_DISPLAY_BUTT) {                                       \
            hi_err_disp("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_INVALID_PARA;                                   \
        }                                                                      \
        (ptr) = &g_disp_intf_mng.disp_intf_chn[(id) - HI_DRV_DISPLAY_0];       \
    } while(0)

#define GET_INTF_CHANNEL_DIRECTLY_BY_ID(id, ptr)                       \
    do {                                                               \
        (ptr) = &g_disp_intf_mng.disp_intf_chn[(id) - HI_DRV_DISPLAY_0]; \
    } while (0)

hi_s32 intf_type_hal_to_drv(hi_drv_disp_intf_id intf_id, hi_drv_disp_intf *intf)
{
    hi_s32 ret = HI_SUCCESS;
    switch (intf_id) {
        case HI_DRV_DISP_INTF_YPBPR0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_YPBPR;
            break;
        case HI_DRV_DISP_INTF_RGB0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_RGB;
            break;
        case HI_DRV_DISP_INTF_SVIDEO0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_SVIDEO;
            break;
        case HI_DRV_DISP_INTF_CVBS0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;
            break;
        case HI_DRV_DISP_INTF_VGA0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_VGA;
            break;
        case HI_DRV_DISP_INTF_HDMI0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf->un_intf.hdmi = HI_DRV_HDMI_ID_0;
            break;
        case HI_DRV_DISP_INTF_HDMI1:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf->un_intf.hdmi = HI_DRV_HDMI_ID_1;
            break;
        case HI_DRV_DISP_INTF_MIPI0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
            intf->un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_0;
            intf->un_intf.mipi.mipi_mode = HI_DRV_MIPI_MODE_SINGLE;
            break;
        case HI_DRV_DISP_INTF_MIPI1:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
            intf->un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_0;
            intf->un_intf.mipi.mipi_mode = HI_DRV_MIPI_MODE_SINGLE;
            break;
        case HI_DRV_DISP_INTF_BT656_0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_BT656;
            intf->un_intf.bt656 = HI_DRV_DISP_BT656_0;
            break;
        case HI_DRV_DISP_INTF_BT1120_0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_BT1120;
            intf->un_intf.bt1120 = HI_DRV_DISP_BT1120_0;
            break;
        case HI_DRV_DISP_INTF_LCD0:
            intf->intf_type = HI_DRV_DISP_INTF_TYPE_LCD;
            intf->un_intf.lcd = HI_DRV_DISP_LCD_0;
            break;
        default:
            ret = HI_ERR_DISP_INVALID_PARA;
            break;
    }

    return ret;
}

hi_s32 intf_type_drv_to_hal_info(hi_drv_display disp,
                                 hi_drv_disp_fmt disp_fmt,
                                 hi_drv_disp_intf_id intf_id,
                                 disp_attach_intf_info *intf)
{
    hi_s32 ret = HI_SUCCESS;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf->intf_attached = HI_TRUE;
    intf->link_venc = HI_FALSE;
    intf->intf_venc = DISP_VENC_BUTT;

    switch (intf_id) {
        case HI_DRV_DISP_INTF_YPBPR0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_YPBPR;
            intf->link_venc = HI_TRUE;

            if ((disp_fmt >= HI_DRV_DISP_FMT_NTSC) &&
                (disp_fmt <= HI_DRV_DISP_FMT_1440x576I_50)) {
                intf->intf_venc = DISP_VENC_SDATE0;
            } else {
                intf->intf_venc = DISP_VENC_HDATE0;
            }

            intf->intf_attr.un_intf.ypbpr.dac_y = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
            intf->intf_attr.un_intf.ypbpr.dac_pb = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
            intf->intf_attr.un_intf.ypbpr.dac_pr = disp_chn->intf_status[intf_id].intf_attr.dac_pr_r;

            break;
        case HI_DRV_DISP_INTF_RGB0:
            intf->link_venc = HI_TRUE;
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_RGB;

            if ((disp_fmt >= HI_DRV_DISP_FMT_NTSC) &&
                (disp_fmt <= HI_DRV_DISP_FMT_1440x576I_50)) {
                intf->intf_venc = DISP_VENC_SDATE0;
            } else {
                intf->intf_venc = DISP_VENC_HDATE0;
            }

            intf->intf_attr.un_intf.rgb.dac_g = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
            intf->intf_attr.un_intf.rgb.dac_b = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
            intf->intf_attr.un_intf.rgb.dac_r = disp_chn->intf_status[intf_id].intf_attr.dac_pr_r;

            break;
        case HI_DRV_DISP_INTF_SVIDEO0:
            intf->link_venc = HI_TRUE;
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_SVIDEO;
            intf->intf_venc = DISP_VENC_SDATE0;

            intf->intf_attr.un_intf.svideo.dac_y = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
            intf->intf_attr.un_intf.svideo.dac_c = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;

            break;
        case HI_DRV_DISP_INTF_CVBS0:
            intf->link_venc = HI_TRUE;
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;
            intf->intf_venc = DISP_VENC_SDATE0;

            intf->intf_attr.un_intf.cvbs.dac_cvbs = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
            break;
        case HI_DRV_DISP_INTF_VGA0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_VGA;
            intf->intf_venc = DISP_VENC_VGA0;

            intf->intf_attr.un_intf.vga.dac_g = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
            intf->intf_attr.un_intf.vga.dac_b = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
            intf->intf_attr.un_intf.vga.dac_r = disp_chn->intf_status[intf_id].intf_attr.dac_pr_r;
            break;
        case HI_DRV_DISP_INTF_HDMI0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf->intf_attr.un_intf.hdmi = HI_DRV_HDMI_ID_0;
            break;
        case HI_DRV_DISP_INTF_HDMI1:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
            intf->intf_attr.un_intf.hdmi = HI_DRV_HDMI_ID_1;
            break;
        case HI_DRV_DISP_INTF_MIPI0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
            intf->intf_attr.un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_0;
            intf->intf_attr.un_intf.mipi.mipi_mode = disp_chn->intf_status[intf_id].intf_attr.mipi_mode;
            break;
        case HI_DRV_DISP_INTF_MIPI1:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
            intf->intf_attr.un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_1;
            intf->intf_attr.un_intf.mipi.mipi_mode = disp_chn->intf_status[intf_id].intf_attr.mipi_mode;
            break;
        case HI_DRV_DISP_INTF_BT656_0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_BT656;
            intf->intf_attr.un_intf.bt656 = HI_DRV_DISP_BT656_0;
            break;
        case HI_DRV_DISP_INTF_BT1120_0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_BT1120;
            intf->intf_attr.un_intf.bt1120 = HI_DRV_DISP_BT1120_0;
            break;
        case HI_DRV_DISP_INTF_LCD0:
            intf->intf_attr.intf_type = HI_DRV_DISP_INTF_TYPE_LCD;
            intf->intf_attr.un_intf.lcd = HI_DRV_DISP_LCD_0;
            break;
        default:
            ret = HI_ERR_DISP_INVALID_PARA;
            break;
    }

    return ret;
}

hi_drv_disp_intf_id intf_type_drv_to_hal(hi_drv_disp_intf intf)
{
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;
    switch (intf.intf_type) {
        case HI_DRV_DISP_INTF_TYPE_HDMI:
            if (intf.un_intf.hdmi == HI_DRV_HDMI_ID_0) {
                intf_id = HI_DRV_DISP_INTF_HDMI0;
            } else if (intf.un_intf.hdmi == HI_DRV_HDMI_ID_1) {
                intf_id = HI_DRV_DISP_INTF_HDMI1;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_LCD:
            if (intf.un_intf.lcd == HI_DRV_DISP_LCD_0) {
                intf_id = HI_DRV_DISP_INTF_LCD0;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_BT1120:
            if (intf.un_intf.bt1120 == HI_DRV_DISP_BT1120_0) {
                intf_id = HI_DRV_DISP_INTF_BT1120_0;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_BT656:
            if (intf.un_intf.bt656 == HI_DRV_DISP_BT656_0) {
                intf_id = HI_DRV_DISP_INTF_BT656_0;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_YPBPR:
            intf_id = HI_DRV_DISP_INTF_YPBPR0;
            break;
        case HI_DRV_DISP_INTF_TYPE_RGB:
            intf_id = HI_DRV_DISP_INTF_RGB0;
            break;
        case HI_DRV_DISP_INTF_TYPE_CVBS:
            intf_id = HI_DRV_DISP_INTF_CVBS0;
            break;
        case HI_DRV_DISP_INTF_TYPE_SVIDEO:
            intf_id = HI_DRV_DISP_INTF_SVIDEO0;
            break;
        case HI_DRV_DISP_INTF_TYPE_VGA:
            intf_id = HI_DRV_DISP_INTF_VGA0;
            break;
        case HI_DRV_DISP_INTF_TYPE_MIPI:
            if (intf.un_intf.mipi.mipi_id == HI_DRV_MIPI_ID_0) {
                intf_id = HI_DRV_DISP_INTF_MIPI0;
            } else if (intf.un_intf.mipi.mipi_id == HI_DRV_MIPI_ID_1) {
                intf_id = HI_DRV_DISP_INTF_MIPI1;
            }
            break;
        default:
            break;
    }

    return intf_id;
}

hi_s32 drv_disp_intf_update_vdac_state(hi_drv_display disp, hi_drv_disp_intf intf, disp_intf_mode set_mode)
{
    hi_s32 ret = HI_FAILURE;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    if (intf_id >= HI_DRV_DISP_INTF_ID_MAX) {
        return HI_ERR_DISP_INVALID_PARA;
    }

    switch (intf.intf_type) {
        case HI_DRV_DISP_INTF_TYPE_YPBPR:
            if (set_mode == DISP_INTF_SET) {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_y] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pb] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pr] = HI_TRUE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pr_r = intf.un_intf.ypbpr.dac_pr;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = intf.un_intf.ypbpr.dac_y;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = intf.un_intf.ypbpr.dac_pb;
                disp_chn->intf_status[intf_id].intf_attr.dac_sync = HI_FALSE;
            } else {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_y] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pb] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pr] = HI_FALSE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_sync = HI_FALSE;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_RGB:
            if (set_mode == DISP_INTF_SET) {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_r] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_g] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_b] = HI_TRUE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pr_r = intf.un_intf.rgb.dac_r;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = intf.un_intf.rgb.dac_g;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = intf.un_intf.rgb.dac_b;
                disp_chn->intf_status[intf_id].intf_attr.dac_sync = HI_FALSE;
            } else {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_r] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_g] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_b] = HI_FALSE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_sync = HI_FALSE;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_CVBS:
            if (set_mode == DISP_INTF_SET) {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.cvbs.dac_cvbs] = HI_TRUE;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = intf.un_intf.cvbs.dac_cvbs;
            } else {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.cvbs.dac_cvbs] = HI_FALSE;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_SVIDEO:
            if (set_mode == DISP_INTF_SET) {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_y] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_c] = HI_TRUE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = intf.un_intf.svideo.dac_c;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = intf.un_intf.svideo.dac_y;
            } else {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_y] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_c] = HI_FALSE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_VGA:
            if (set_mode == DISP_INTF_SET) {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_r] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_g] = HI_TRUE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_b] = HI_TRUE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pr_r = intf.un_intf.vga.dac_r;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = intf.un_intf.vga.dac_g;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = intf.un_intf.vga.dac_b;
                disp_chn->intf_status[intf_id].intf_attr.dac_sync = HI_FALSE;
            } else {
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_r] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_g] = HI_FALSE;
                g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_b] = HI_FALSE;
                disp_chn->intf_status[intf_id].intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
                disp_chn->intf_status[intf_id].intf_attr.dac_sync = HI_FALSE;
            }
            break;

        default:
            ret = HI_ERR_DISP_INVALID_PARA;
            break;
    }

    return HI_SUCCESS;
}

hi_s32 clean_disp_intf(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_s32 ret;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_intf_status *intf_status = HI_NULL;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    intf_status = &disp_chn->intf_status[intf_id];

    ret = drv_disp_intf_update_vdac_state(disp, intf, DISP_INTF_CLEAR);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    disp_chn->intf_status[intf_id].attached = HI_FALSE;
    disp_chn->intf_status[intf_id].enable = HI_FALSE;
    disp_chn->intf_status[intf_id].matching = HI_FALSE;
    disp_chn->intf_status[intf_id].intf_attr.id = HI_DRV_DISP_INTF_ID_MAX;
    disp_chn->intf_status[intf_id].intf_attr.mipi_mode = HI_DRV_MIPI_MODE_MAX;

    g_disp_intf_mng.is_intf_attached[intf_id] = HI_FALSE;

    return ret;
}

hi_s32 check_vdac_rgb_params(disp_intf_type intf_type, hi_u32 dac_y, hi_u32 dac_pb, hi_u32 dac_pr)
{
    if ((intf_type == HI_DRV_DISP_INTF_TYPE_YPBPR) || (intf_type == HI_DRV_DISP_INTF_TYPE_VGA) ||
        (intf_type == HI_DRV_DISP_INTF_TYPE_RGB)) {
        if ((dac_y >= HI_DISP_VDAC_MAX_NUMBER) || (dac_pb >= HI_DISP_VDAC_MAX_NUMBER) ||
            (dac_pr >= HI_DISP_VDAC_MAX_NUMBER) || (dac_y == dac_pb) || (dac_y == dac_pr) || (dac_pb == dac_pr)) {
            hi_err_disp("type %d:y(%d),pb(%d),pr(%d)", intf_type, dac_y, dac_pb, dac_pr);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    if (intf_type == HI_DRV_DISP_INTF_TYPE_SVIDEO) {
        if ((dac_y >= HI_DISP_VDAC_MAX_NUMBER) || (dac_pb >= HI_DISP_VDAC_MAX_NUMBER) ||
            (dac_pr != HI_DISP_VDAC_INVALID_ID) || (dac_y == dac_pb)) {
            hi_err_disp("type %d:y(%d),pb(%d),pr(%d)", intf_type, dac_y, dac_pb, dac_pr);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    if (intf_type == HI_DRV_DISP_INTF_TYPE_CVBS) {
        if ((dac_y >= HI_DISP_VDAC_MAX_NUMBER) || (dac_pb != HI_DISP_VDAC_INVALID_ID) ||
            (dac_pr != HI_DISP_VDAC_INVALID_ID)) {
            hi_err_disp("type %d:y(%d),pb(%d),pr(%d)", intf_type, dac_y, dac_pb, dac_pr);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}

hi_s32 is_intf_vdac_busy(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    if (intf_id >= HI_DRV_DISP_INTF_ID_MAX) {
        hi_err_disp("intf_id(%d) is err\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    switch (intf.intf_type) {
        case HI_DRV_DISP_INTF_TYPE_YPBPR:
            if ((g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_y]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pb]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pr])) {
                hi_err_disp("ypbpr vdac is attached: y_id(%d)[%d], pb_id(%d)[%d], pr_id(%d)[%d]\n",
                    intf.un_intf.ypbpr.dac_y,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_y], intf.un_intf.ypbpr.dac_pb,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pb], intf.un_intf.ypbpr.dac_pr,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.ypbpr.dac_pr]);
                return HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_RGB:
            if ((g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_r]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_g]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_b])) {
                hi_err_disp("rgb vdac is attached: r_id(%d)[%d], g_id(%d)[%d], b_id(%d)[%d]\n", intf.un_intf.rgb.dac_r,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_r], intf.un_intf.rgb.dac_g,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_g], intf.un_intf.rgb.dac_b,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.rgb.dac_b]);
                return HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_CVBS:
            if (g_disp_intf_mng.is_vdac_attached[intf.un_intf.cvbs.dac_cvbs]) {
                hi_err_disp("cvbs vdac is attached: cvbs_id(%d)[%d]\n", intf.un_intf.cvbs.dac_cvbs,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.cvbs.dac_cvbs]);
                return HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_SVIDEO:
            if ((g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_y]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_c])) {
                hi_err_disp("svideo vdac is attached: y_id(%d)[%d], c_id(%d)[%d]\n", intf.un_intf.svideo.dac_y,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_y], intf.un_intf.svideo.dac_c,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.svideo.dac_c]);
                return HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_VGA:
            if ((g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_r]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_g]) ||
                (g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_b])) {
                hi_err_disp("vga vdac is attached: r_id(%d)[%d], g_id(%d)[%d], b_id(%d)[%d]\n", intf.un_intf.vga.dac_r,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_r], intf.un_intf.vga.dac_g,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_g], intf.un_intf.vga.dac_b,
                    g_disp_intf_mng.is_vdac_attached[intf.un_intf.vga.dac_b]);
                return HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_HDMI:
        case HI_DRV_DISP_INTF_TYPE_LCD:
        case HI_DRV_DISP_INTF_TYPE_BT1120:
        case HI_DRV_DISP_INTF_TYPE_BT656:
        case HI_DRV_DISP_INTF_TYPE_MIPI:
            break;

        default:
            return HI_ERR_DISP_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 is_intf_params_valid(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    if (intf_id >= HI_DRV_DISP_INTF_ID_MAX) {
        hi_err_disp("intf_id(%d) is err\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    switch (intf.intf_type) {
        case HI_DRV_DISP_INTF_TYPE_YPBPR:
            ret = check_vdac_rgb_params(intf.intf_type, intf.un_intf.ypbpr.dac_y, intf.un_intf.ypbpr.dac_pb,
                intf.un_intf.ypbpr.dac_pr);
            break;
        case HI_DRV_DISP_INTF_TYPE_RGB:
            ret = check_vdac_rgb_params(intf.intf_type, intf.un_intf.rgb.dac_r, intf.un_intf.rgb.dac_g,
                intf.un_intf.rgb.dac_b);
            break;
        case HI_DRV_DISP_INTF_TYPE_CVBS:
            ret = check_vdac_rgb_params(intf.intf_type, intf.un_intf.cvbs.dac_cvbs, HI_DISP_VDAC_INVALID_ID,
                HI_DISP_VDAC_INVALID_ID);
            break;
        case HI_DRV_DISP_INTF_TYPE_SVIDEO:
            ret = check_vdac_rgb_params(intf.intf_type, intf.un_intf.svideo.dac_y, intf.un_intf.svideo.dac_c,
                HI_DISP_VDAC_INVALID_ID);
            break;
        case HI_DRV_DISP_INTF_TYPE_VGA:
            ret = check_vdac_rgb_params(intf.intf_type, intf.un_intf.vga.dac_r, intf.un_intf.vga.dac_g,
                intf.un_intf.vga.dac_b);
            break;
        case HI_DRV_DISP_INTF_TYPE_HDMI:
            if ((intf.un_intf.hdmi != HI_DRV_HDMI_ID_0) && (intf.un_intf.hdmi != HI_DRV_HDMI_ID_1)) {
                ret = HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_LCD:
            if (intf.un_intf.lcd != HI_DRV_DISP_LCD_0) {
                ret = HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_BT1120:
            if (intf.un_intf.bt1120 != HI_DRV_DISP_BT1120_0) {
                ret = HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_BT656:
            if (intf.un_intf.bt656 != HI_DRV_DISP_BT656_0) {
                ret = HI_ERR_DISP_INVALID_PARA;
            }
            break;
        case HI_DRV_DISP_INTF_TYPE_MIPI:
            if ((intf.un_intf.mipi.mipi_id != HI_DRV_MIPI_ID_0) &&
                (intf.un_intf.mipi.mipi_mode != HI_DRV_MIPI_MODE_SINGLE) &&
                (intf.un_intf.mipi.mipi_mode != HI_DRV_MIPI_MODE_SINGLE_DSC) &&
                (intf.un_intf.mipi.mipi_mode != HI_DRV_MIPI_MODE_DOUBLE)) {
                ret = HI_ERR_DISP_INVALID_PARA;
            }
            break;
        default:
            ret = HI_ERR_DISP_INVALID_PARA;
            break;
    }

    return ret;
}

hi_bool is_disp_intf_existed(hi_drv_display disp_id, hi_drv_disp_intf intf)
{
    disp_intf_channel *disp_chn = HI_NULL;
    hi_drv_disp_intf_id intf_id;

    GET_INTF_CHANNEL_DIRECTLY_BY_ID(disp_id, disp_chn);
    intf_id = intf_type_drv_to_hal(intf);
    if ((disp_chn->intf_status[intf_id].attached == HI_TRUE) &&
        (intf_id == disp_chn->intf_status[intf_id].intf_attr.id)) {
        return HI_TRUE;
    }

    hi_warn_disp("intf(%d) is not existed\n", intf.intf_type);
    return HI_FALSE;
}

hi_void attach_to_disp(disp_intf_status *intf_status, hi_drv_disp_intf intf)
{
    intf_status->intf_attr.id = intf_type_drv_to_hal(intf);

    switch (intf.intf_type) {
        case HI_DRV_DISP_INTF_TYPE_YPBPR:

            intf_status->intf_attr.dac_y_g = intf.un_intf.ypbpr.dac_y;
            intf_status->intf_attr.dac_pb_b = intf.un_intf.ypbpr.dac_pb;
            intf_status->intf_attr.dac_pr_r = intf.un_intf.ypbpr.dac_pr;
            intf_status->intf_attr.dac_sync = HI_FALSE;
            break;
        case HI_DRV_DISP_INTF_TYPE_VGA:
            intf_status->intf_attr.dac_y_g = intf.un_intf.vga.dac_g;
            intf_status->intf_attr.dac_pb_b = intf.un_intf.vga.dac_b;
            intf_status->intf_attr.dac_pr_r = intf.un_intf.vga.dac_r;
            intf_status->intf_attr.dac_sync = HI_FALSE;
            break;
        case HI_DRV_DISP_INTF_TYPE_RGB:
            intf_status->intf_attr.dac_y_g = intf.un_intf.rgb.dac_g;
            intf_status->intf_attr.dac_pb_b = intf.un_intf.rgb.dac_b;
            intf_status->intf_attr.dac_pr_r = intf.un_intf.rgb.dac_r;
            intf_status->intf_attr.dac_sync = HI_FALSE;
            break;

        case HI_DRV_DISP_INTF_TYPE_SVIDEO:
            intf_status->intf_attr.dac_y_g = intf.un_intf.svideo.dac_y;
            intf_status->intf_attr.dac_pb_b = intf.un_intf.svideo.dac_c;
            intf_status->intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
            intf_status->intf_attr.dac_sync = HI_FALSE;
            break;

        case HI_DRV_DISP_INTF_TYPE_CVBS:

            intf_status->intf_attr.dac_y_g = intf.un_intf.cvbs.dac_cvbs;
            intf_status->intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
            intf_status->intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
            intf_status->intf_attr.dac_sync = HI_FALSE;

        default:

            intf_status->intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
            intf_status->intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
            intf_status->intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
            intf_status->intf_attr.dac_sync = HI_FALSE;
            break;
    }
}

hi_s32 add_disp_intf(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_s32 ret;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_intf_status *intf_status = HI_NULL;
    disp_attach_intf_info attached_intf_info;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    intf_status = &disp_chn->intf_status[intf_id];

    ret = intf_type_drv_to_hal_info(disp, disp_chn->disp_fmt, intf_id, &attached_intf_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("intf type(%d) intf_type_drv_to_hal_infois error\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    ret = hal_disp_attach_intf(disp, &attached_intf_info);
    if (ret) {
        hi_err_disp("DISP %d acquire  (%d) failed\n", disp, intf_status->intf_attr.id);
        return ret;
    }

    ret = drv_disp_intf_update_vdac_state(disp, intf, DISP_INTF_SET);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    intf_status->attached = HI_TRUE;
    disp_chn->intf_status[intf_id].intf_attr.id = intf_id;

    if ((intf_id == HI_DRV_DISP_INTF_MIPI0) || (intf_id == HI_DRV_DISP_INTF_MIPI1)) {
        intf_status->intf_attr.mipi_mode = intf.un_intf.mipi.mipi_mode;
    }

    g_disp_intf_mng.is_intf_attached[intf_id] = HI_TRUE;

    return ret;
}

hi_s32 del_and_unattach_intf(hi_drv_display disp_id, hi_drv_disp_intf intf)
{
    return HI_SUCCESS;
}

hi_s32 disp_suspend(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_set_intf_output_format(hi_drv_display disp, hi_drv_disp_format_param *format_param,
    disp_timing_info *timing_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 number;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_attach_intf_info intf_attached_info;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        if (disp_chn->intf_status[intf_id].attached) {
            ret = intf_type_drv_to_hal_info(disp, timing_info->disp_fmt, intf_id, &intf_attached_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("intf type(%d) intf_type_drv_to_hal_infois error\n", intf_id);
                return HI_ERR_DISP_INVALID_PARA;
            }

            ret = hal_disp_set_intf_format(disp, &intf_attached_info, timing_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("set intf format is error\n");
                return HI_ERR_DISP_INVALID_PARA;
            }
        }
    }

    return ret;
}

hi_s32 disp_intf_check_fmt(hi_drv_disp_intf intf, hi_drv_disp_fmt format)
{
    hi_s32 ret = 0;
    if (format >= HI_DRV_DISP_FMT_BUTT) {
        hi_err_disp("fmt(%d) is err!\n", format);
        return HI_ERR_DISP_INVALID_PARA;
    }

    switch (intf.intf_type) {
        case HI_DRV_DISP_INTF_TYPE_HDMI:
        case HI_DRV_DISP_INTF_TYPE_LCD:
        case HI_DRV_DISP_INTF_TYPE_BT1120:
        case HI_DRV_DISP_INTF_TYPE_BT656:
        case HI_DRV_DISP_INTF_TYPE_YPBPR:
        case HI_DRV_DISP_INTF_TYPE_RGB:
        case HI_DRV_DISP_INTF_TYPE_VGA:
        case HI_DRV_DISP_INTF_TYPE_MIPI:
            ret = HI_SUCCESS;
            break;

        case HI_DRV_DISP_INTF_TYPE_CVBS:
        case HI_DRV_DISP_INTF_TYPE_SVIDEO:
            if ((format >= HI_DRV_DISP_FMT_PAL) && (format <= HI_DRV_DISP_FMT_1440x480I_60)) {
                ret = HI_SUCCESS;
            } else {
                ret = HI_ERR_DISP_INVALID_PARA;
            }
            break;

        default:
            break;
    }

    return ret;
}

hi_void update_intf_matching_status(hi_drv_display disp, hi_drv_disp_format_param *format_param,
    hi_bool *matching_state)
{
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;
    GET_INTF_CHANNEL_DIRECTLY_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        if (matching_state[intf_id] == HI_TRUE) {
            disp_chn->intf_status[intf_id].matching = HI_TRUE;
        } else {
            disp_chn->intf_status[intf_id].matching = HI_FALSE;
        }
    }
}

hi_s32 disp_check_channel_fmt_validate(hi_drv_disp_timing_cfg *disp_timing_cfg, disp_intf_type intf_type)
{
    hi_u32 htotal;
    hi_u32 vtotal;

    htotal = disp_timing_cfg->static_timing.timing.hfb + disp_timing_cfg->static_timing.timing.hact +
        disp_timing_cfg->static_timing.timing.hbb;
    vtotal = disp_timing_cfg->static_timing.timing.vfb + disp_timing_cfg->static_timing.timing.vact +
        disp_timing_cfg->static_timing.timing.vbb;

    if ((vtotal == 0) || (htotal == 0) || (htotal > DISP_HTOTAL_VTOTAL_MAX) || (vtotal > DISP_HTOTAL_VTOTAL_MAX) ||
        ((DISP_HTOTAL_VTOTAL_MAX / vtotal) < htotal)) {
        hi_err_disp("vtotal(%d)*htotal(%d) is error\n", vtotal, htotal);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ((intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) || (intf_type == HI_DRV_DISP_INTF_TYPE_MIPI)) {
        if (disp_timing_cfg->static_timing.interlace == HI_TRUE) {
            hi_err_disp("hdmi and mipi not support interlace fmt(%d)\n", disp_timing_cfg->static_timing.disp_fmt);
            return HI_ERR_DISP_NOT_SUPPORT_FMT;
        }

        if ((disp_timing_cfg->static_timing.disp_fmt != HI_DRV_DISP_FMT_CUSTOM) &&
            (((htotal * vtotal) % DISP_INTF_FORMAT_HFB) != 0)) {
            hi_err_disp("hdmi and mipi not support fmt(%d) of htotal*vtotal(%d)\n",
                disp_timing_cfg->static_timing.disp_fmt, htotal * vtotal);
            return HI_ERR_DISP_NOT_SUPPORT_FMT;
        }

        if ((disp_timing_cfg->static_timing.disp_fmt >= HI_DRV_DISP_FMT_1080P_24_FP) &&
            (disp_timing_cfg->static_timing.disp_fmt <= HI_DRV_DISP_FMT_VESA_2560X1600_60_RB)) {
            hi_err_disp("hdmi and mipi not support VESA and 3d fmt(%d)\n", disp_timing_cfg->static_timing.disp_fmt,
                disp_timing_cfg->static_timing.timing.hfb);
            return HI_ERR_DISP_NOT_SUPPORT_FMT;
        }
    }

    if (intf_type == HI_DRV_DISP_INTF_TYPE_CVBS) {
        if (disp_timing_cfg->static_timing.disp_fmt > HI_DRV_DISP_FMT_1440x576I_50) {
            hi_err_disp("cvbs not support FHD above fmt(%d)\n", disp_timing_cfg->static_timing.disp_fmt);
            return HI_ERR_DISP_NOT_SUPPORT_FMT;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_check_output_validate(hi_drv_display disp, hi_drv_disp_intf_info *intf_info,
    hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_intf_status *intf_status = HI_NULL;
    hi_bool matching_state[HI_DRV_DISP_INTF_ID_MAX] = { HI_FALSE };

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        intf_status = &disp_chn->intf_status[intf_id];
        if (intf_status->attached == HI_TRUE) {
            DISP_CHECK_NULL_POINTER(intf_status->func_intf_ctrl.intf_check_validate);

            intf_info->in_info.data_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
            intf_info->in_info.pixel_format = HI_DRV_DISP_PIXEL_YUV444;
            intf_info->out_info.data_width = format_param->intf_output[number].bit_width;
            intf_info->out_info.pixel_format = format_param->intf_output[number].pixel_fmt;

            ret = intf_status->func_intf_ctrl.intf_check_validate(format_param->intf_output[number].intf,
                MODE_STATIC_TIMING, intf_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("disp%d intf type (%d) check validate failed \n", disp,
                    format_param->intf_output[number].intf.intf_type);
                matching_state[intf_id] = HI_FALSE;
                return HI_ERR_DISP_INVALID_PARA;
            }

            matching_state[intf_id] = HI_TRUE;

            ret = disp_check_channel_fmt_validate(&(intf_info->disp_timing),
                format_param->intf_output[number].intf.intf_type);
            if (ret != HI_SUCCESS) {
                return ret;
            }
        } else {
            hi_err_disp("intf type(%d) is not attached \n", intf_id);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    update_intf_matching_status(disp, format_param, matching_state);
    disp_chn->disp_fmt = intf_info->disp_timing.static_timing.disp_fmt;

    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_output_prepare(hi_drv_display disp, hi_drv_disp_intf_info *intf_info,
    hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_intf_status *intf_status = HI_NULL;
    disp_timing_info timing_info;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(intf_info);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        intf_status = &disp_chn->intf_status[intf_id];
        if ((intf_status->attached == HI_TRUE) && (intf_status->enable == HI_TRUE)) {
            DISP_CHECK_NULL_POINTER(intf_status->func_intf_ctrl.intf_prepare);
            intf_info->in_info.data_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
            intf_info->in_info.pixel_format = HI_DRV_DISP_PIXEL_YUV444;
            intf_info->out_info.data_width = format_param->intf_output[number].bit_width;
            intf_info->out_info.pixel_format = format_param->intf_output[number].pixel_fmt;

            ret = intf_status->func_intf_ctrl.intf_prepare(format_param->intf_output[number].intf, MODE_STATIC_TIMING,
                intf_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("intf(%d) prepare is err\n", intf_id);
                return ret;
            }
        } else {
            continue;
        }
    }

    timing_info.disp_fmt = intf_info->disp_timing.static_timing.disp_fmt;
    ret = drv_disp_set_intf_output_format(disp, format_param, &timing_info);
    if (ret != HI_SUCCESS) {
        hi_err_disp("drv_disp_set_intf_format is error\n");
    }

    return ret;
}

hi_s32 drv_disp_intf_output_config(hi_drv_display disp, hi_drv_disp_intf_info *intf_info,
    hi_drv_disp_format_param *format_param)
{
    hi_s32 ret;
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_intf_status *intf_status = HI_NULL;

    DISP_CHECK_ID(disp);
    DISP_CHECK_NULL_POINTER(intf_info);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        intf_status = &disp_chn->intf_status[intf_id];
        if ((intf_status->attached == HI_TRUE) && (intf_status->enable == HI_TRUE)) {
            DISP_CHECK_NULL_POINTER(intf_status->func_intf_ctrl.intf_config);

            intf_info->in_info.data_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
            intf_info->in_info.pixel_format = HI_DRV_DISP_PIXEL_YUV444;
            intf_info->out_info.data_width = format_param->intf_output[number].bit_width;
            intf_info->out_info.pixel_format = format_param->intf_output[number].pixel_fmt;

            ret = intf_status->func_intf_ctrl.intf_config(format_param->intf_output[number].intf, MODE_STATIC_TIMING,
                intf_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("intf(%d) config is err\n", intf_id);
                return ret;
            }
        } else {
            continue;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_output_enable(hi_drv_display disp, hi_drv_disp_format_param *format_param, hi_bool enable)
{
    hi_s32 ret;
    hi_u32 intf_number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_status *intf_status = HI_NULL;
    disp_intf_channel *disp_chn = HI_NULL;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (intf_number = 0; intf_number < format_param->number; intf_number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[intf_number].intf);
        intf_status = &(disp_chn->intf_status[intf_id]);
        if (disp_chn->intf_status[intf_id].attached == HI_TRUE) {
            DISP_CHECK_NULL_POINTER(intf_status->func_intf_ctrl.intf_enable);

            ret = intf_status->func_intf_ctrl.intf_enable(format_param->intf_output[intf_number].intf, enable);
            if (ret != HI_SUCCESS) {
                hi_err_disp("set intf(%d) enable failed.\n", intf_id);
                return ret;
            }

            intf_status->enable = enable;
        }
    }

    return HI_SUCCESS;
}

hi_s32 disp_intf_check_validate(hi_drv_disp_intf intf, hi_u32 set_mode, hi_drv_disp_intf_info *intf_info)
{
    return HI_SUCCESS;
}

hi_s32 disp_intf_config(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    return HI_SUCCESS;
}

hi_s32 disp_intf_detach(hi_drv_disp_intf intf, void *data)
{
    return HI_SUCCESS;
}

hi_s32 disp_intf_enable(hi_drv_disp_intf intf, hi_bool enable)
{
    return HI_SUCCESS;
}

hi_s32 disp_intf_prepare(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    return HI_SUCCESS;
}

hi_s32 disp_intf_attach(hi_drv_disp_intf intf, void *data)
{
    return HI_SUCCESS;
}

hi_s32 disp_intf_atomic_config(hi_drv_disp_intf intf, hi_u32 mode, hi_drv_disp_intf_info *intf_info)
{
    return HI_SUCCESS;
}

hi_s32 disp_resume(hi_drv_disp_intf intf, hi_void *pdev, hi_void *state)
{
    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_get_status(hi_drv_display disp, disp_get_intf *intf_state)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;
    disp_intf_channel *disp_chn = HI_NULL;
    hi_drv_disp_intf *tmp_intf = HI_NULL;
    hi_s32 intf_num = 0;
    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);
    for (intf_id = HI_DRV_DISP_INTF_YPBPR0; intf_id < HI_DRV_DISP_INTF_ID_MAX; intf_id++) {
        if (disp_chn->intf_status[intf_id].attached == HI_TRUE) {
            tmp_intf = &(intf_state->intf[intf_num]);
            switch (intf_id) {
                case HI_DRV_DISP_INTF_YPBPR0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_YPBPR;
                    tmp_intf->un_intf.ypbpr.dac_pr = disp_chn->intf_status[intf_id].intf_attr.dac_pr_r;
                    tmp_intf->un_intf.ypbpr.dac_y = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
                    tmp_intf->un_intf.ypbpr.dac_pb = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
                    break;
                case HI_DRV_DISP_INTF_RGB0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_RGB;
                    tmp_intf->un_intf.rgb.dac_r = disp_chn->intf_status[intf_id].intf_attr.dac_pr_r;
                    tmp_intf->un_intf.rgb.dac_g = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
                    tmp_intf->un_intf.rgb.dac_b = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
                    break;
                case HI_DRV_DISP_INTF_CVBS0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_CVBS;
                    tmp_intf->un_intf.cvbs.dac_cvbs = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
                    break;
                case HI_DRV_DISP_INTF_SVIDEO0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_SVIDEO;
                    tmp_intf->un_intf.svideo.dac_c = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
                    tmp_intf->un_intf.svideo.dac_y = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
                    break;
                case HI_DRV_DISP_INTF_VGA0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_VGA;
                    tmp_intf->un_intf.vga.dac_r = disp_chn->intf_status[intf_id].intf_attr.dac_pr_r;
                    tmp_intf->un_intf.vga.dac_g = disp_chn->intf_status[intf_id].intf_attr.dac_y_g;
                    tmp_intf->un_intf.vga.dac_b = disp_chn->intf_status[intf_id].intf_attr.dac_pb_b;
                    break;
                case HI_DRV_DISP_INTF_HDMI0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
                    tmp_intf->un_intf.hdmi = HI_DRV_HDMI_ID_0;
                    break;
                case HI_DRV_DISP_INTF_HDMI1:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_HDMI;
                    tmp_intf->un_intf.hdmi = HI_DRV_HDMI_ID_1;
                    break;
                case HI_DRV_DISP_INTF_LCD0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_LCD;
                    tmp_intf->un_intf.lcd = HI_DRV_DISP_LCD_0;
                    break;
                case HI_DRV_DISP_INTF_BT1120_0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_BT1120;
                    tmp_intf->un_intf.bt1120 = HI_DRV_DISP_BT1120_0;
                    break;
                case HI_DRV_DISP_INTF_BT656_0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_BT656;
                    tmp_intf->un_intf.bt656 = HI_DRV_DISP_BT656_0;
                    break;
                case HI_DRV_DISP_INTF_MIPI0:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
                    tmp_intf->un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_0;
                    tmp_intf->un_intf.mipi.mipi_mode = disp_chn->intf_status[intf_id].intf_attr.mipi_mode;
                    break;
                case HI_DRV_DISP_INTF_MIPI1:
                    tmp_intf->intf_type = HI_DRV_DISP_INTF_TYPE_MIPI;
                    tmp_intf->un_intf.mipi.mipi_id = HI_DRV_MIPI_ID_1;
                    tmp_intf->un_intf.mipi.mipi_mode = disp_chn->intf_status[intf_id].intf_attr.mipi_mode;
                    break;
                default:
                    ret = HI_ERR_DISP_INVALID_PARA;
                    break;
            }
            intf_num++;
        }
    }
    intf_state->disp = disp;
    intf_state->intf_num = intf_num;
    return ret;
}

hi_s32 drv_disp_intf_get_enable(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool *enable)
{
    disp_intf_channel *disp_chn = HI_NULL;
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);
    intf_id = intf_type_drv_to_hal(drv_intf);
    if (intf_id > HI_DRV_DISP_INTF_ID_MAX) {
        hi_err_disp("intf(%d) id invalid.\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (disp_chn->intf_status[intf_id].attached == HI_TRUE) {
        *enable = disp_chn->intf_status[intf_id].enable;
    } else {
        hi_err_disp("intf(%d) is not attached.\n", intf_id);
        ret = HI_ERR_DISP_INVALID_PARA;
    }

    return ret;
}

hi_s32 drv_disp_intf_get_capability(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool *is_support)
{
    disp_intf_channel *disp_chn = HI_NULL;
    hi_drv_disp_intf_id intf_id;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);
    intf_id = intf_type_drv_to_hal(drv_intf);
    if (intf_id > HI_DRV_DISP_INTF_ID_MAX) {
        hi_err_disp("intf(%d) id invalid.\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    *is_support = disp_chn->intf_status[intf_id].is_support;
    return HI_SUCCESS;
}

char *g_disp_intf_type[HI_DRV_DISP_INTF_ID_MAX] = {
    "ypbpr",
    "rgb",
    "svideo",
    "cvbs",
    "vga",
    "hdmi0",
    "hdmi1",
    "mipi0",
    "mipi1",
    "bt656",
    "bt1120",
    "lcd",
};

char *g_disp_intf_bit_width[HI_DRV_PIXEL_BITWIDTH_MAX + 1] = {
    "8bit",
    "10bit",
    "12bit",
    "16bit",
    "null",
};

char *g_disp_intf_pixel_fmt[HI_DRV_DISP_PIXEL_MAX + 1] = {
    "rgb",
    "422",
    "444",
    "420",
    "null",
};

char *g_disp_mipi_intf_mode[HI_DRV_MIPI_MODE_MAX + 1] = {
    "single",
    "single_dsc",
    "double",
    "max",
};

hi_s32 drv_disp_intf_get_proc_info(struct seq_file *p, hi_drv_display disp)
{
    int i = 0;
    disp_intf_channel *disp_chn = HI_NULL;
    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    HI_PROC_PRINT(p, "%-20s: \n", "attached intf");

    for (i = HI_DRV_DISP_INTF_YPBPR0; i < HI_DRV_DISP_INTF_ID_MAX; i++) {
        if (disp_chn->intf_status[i].attached == HI_TRUE) {
            HI_PROC_PRINT(p, "%s", g_disp_intf_type[i]);

            if (disp_chn->intf_status[i].enable == HI_TRUE) {
                HI_PROC_PRINT(p, "     [enable=YES]");
            } else {
                HI_PROC_PRINT(p, "     [enable=NO]");
            }

            if (disp_chn->intf_status[i].matching == HI_TRUE) {
                HI_PROC_PRINT(p, "[matching=YES]");
            } else {
                HI_PROC_PRINT(p, "[matching=NO]");
            }

            HI_PROC_PRINT(p, "[bitwidth=%s]", g_disp_intf_bit_width[disp_chn->intf_status[i].bit_width]);
            HI_PROC_PRINT(p, "[pixelfmt=%s]", g_disp_intf_pixel_fmt[disp_chn->intf_status[i].pixel_fmt]);

            if (i == HI_DRV_DISP_INTF_MIPI0 || i == HI_DRV_DISP_INTF_MIPI1) {
                HI_PROC_PRINT(p, "[mode=%s]", g_disp_mipi_intf_mode[disp_chn->intf_status[i].intf_attr.mipi_mode]);
            }

            HI_PROC_PRINT(p, "\n");
        }
    }

    HI_PROC_PRINT(p, "\n");
    HI_PROC_PRINT(p, "%-20s: ", "attached vdac");

    for (i = 0; i < HI_DISP_VDAC_MAX_NUMBER; i++) {
        if (g_disp_intf_mng.is_vdac_attached[i] == HI_TRUE) {
            HI_PROC_PRINT(p, "[%d]", i);
        }
    }

    HI_PROC_PRINT(p, "\n");
    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_attach(hi_drv_display disp, hi_drv_disp_intf intf)
{
    disp_intf_channel *disp_chn = HI_NULL;
    hi_s32 ret;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    if (intf_id >= HI_DRV_DISP_INTF_ID_MAX) {
        hi_err_disp("intf(%d) id is err\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (is_disp_intf_existed(disp, intf) == HI_TRUE) {
        hi_warn_disp("intf(%d) id is attached\n", intf_id);
        return HI_SUCCESS;
    }

    if (g_disp_intf_mng.is_intf_attached[intf_id] == HI_TRUE) {
        hi_err_disp("intf(%d) is attached to other disp(%d)\n", intf_id, disp);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (is_intf_params_valid(disp, intf) != HI_SUCCESS) {
        hi_err_disp("intf params is invalid\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (is_intf_vdac_busy(disp, intf) != HI_SUCCESS) {
        hi_err_disp("vdac is busy\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    DISP_CHECK_NULL_POINTER(disp_chn->intf_status[intf_id].func_intf_ctrl.intf_attach);

    ret = disp_chn->intf_status[intf_id].func_intf_ctrl.intf_attach(intf, HI_NULL);
    if (ret != HI_SUCCESS) {
        hi_err_disp("intf_attach failed\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    ret = add_disp_intf(disp, intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("add_disp_intf failed\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    return ret;
}

hi_s32 drv_disp_intf_detach(hi_drv_display disp, hi_drv_disp_intf intf)
{
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    intf_id = intf_type_drv_to_hal(intf);
    if (intf_id >= HI_DRV_DISP_INTF_ID_MAX) {
        hi_err_disp("intf(%d) id is err\n", intf_id);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (is_disp_intf_existed(disp, intf) == HI_TRUE) {
        del_and_unattach_intf(disp, intf);
        (hi_void)clean_disp_intf(disp, intf);

        return HI_SUCCESS;
    }

    hi_warn_disp("disp%d del intf %d (%d,%d,%d)\n", disp, intf.intf_type, intf.un_intf.ypbpr.dac_y,
        intf.un_intf.ypbpr.dac_pb, intf.un_intf.ypbpr.dac_pr);
    return HI_ERR_DISP_INVALID_PARA;
}

hi_s32 drv_disp_set_intf_format(hi_drv_display disp, disp_timing_info *timing_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_drv_disp_intf_id intf_id = HI_DRV_DISP_INTF_ID_MAX;
    disp_intf_channel *disp_chn = HI_NULL;
    disp_attach_intf_info intf_attached_info;

    DISP_CHECK_ID(disp);
    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (intf_id = HI_DRV_DISP_INTF_YPBPR0; intf_id < HI_DRV_DISP_INTF_ID_MAX; intf_id++) {
        if (disp_chn->intf_status[intf_id].attached) {
            ret = intf_type_drv_to_hal_info(disp, timing_info->disp_fmt, intf_id, &intf_attached_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("intf type(%d) intf_type_drv_to_hal_infois error\n", intf_id);
                return HI_ERR_DISP_INVALID_PARA;
            }

            ret = hal_disp_set_intf_format(disp, &intf_attached_info, timing_info);
            if (ret != HI_SUCCESS) {
                hi_err_disp("set intf format is error\n");
                return HI_ERR_DISP_INVALID_PARA;
            }
        }
    }

    return ret;
}

hi_s32 drv_disp_intf_set_attr(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        if (disp_chn->intf_status[intf_id].attached == HI_TRUE) {
            disp_chn->intf_status[intf_id].bit_width = format_param->intf_output[number].bit_width;
            if (disp_chn->intf_status[intf_id].bit_width == HI_DRV_PIXEL_BITWIDTH_DEFAULT) {
                disp_chn->intf_status[intf_id].bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
            }
            disp_chn->intf_status[intf_id].pixel_fmt = format_param->intf_output[number].pixel_fmt;
        } else {
            hi_err_disp("set intf(%d) is not attached\n", intf_id);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}


hi_s32 drv_disp_intf_set_enable_status(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        if (disp_chn->intf_status[intf_id].attached == HI_TRUE) {
            disp_chn->intf_status[intf_id].enable = format_param->intf_output[number].enable;
        } else {
            hi_err_disp("set intf(%d) is not attached\n", intf_id);
            return HI_ERR_DISP_INVALID_PARA;
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_get_attr(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_u32 number;
    hi_drv_disp_intf_id intf_id;
    disp_intf_channel *disp_chn = HI_NULL;

    GET_INTF_CHANNEL_BY_ID(disp, disp_chn);

    for (number = 0; number < format_param->number; number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[number].intf);
        format_param->intf_output[number].bit_width = disp_chn->intf_status[intf_id].bit_width;
        format_param->intf_output[number].pixel_fmt = disp_chn->intf_status[intf_id].pixel_fmt;
    }

    return HI_SUCCESS;
}

hi_bool drv_disp_intf_check_existed_status(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_u32 intf_number;
    hi_bool attached_status;
    for (intf_number = 0; intf_number < format_param->number; intf_number++) {
        attached_status = is_disp_intf_existed(disp, format_param->intf_output[intf_number].intf);
        if (attached_status != HI_TRUE) {
            return HI_FALSE;
        }
    }
    return HI_TRUE;
}

hi_bool drv_disp_intf_check_same_param(hi_drv_display disp, hi_drv_disp_format_param *format_param)
{
    hi_u32 intf_number;
    disp_intf_channel *disp_chn = HI_NULL;
    hi_drv_disp_intf_id intf_id;

    GET_INTF_CHANNEL_DIRECTLY_BY_ID(disp, disp_chn);

    for (intf_number = 0; intf_number < format_param->number; intf_number++) {
        intf_id = intf_type_drv_to_hal(format_param->intf_output[intf_number].intf);
        if ((format_param->intf_output[intf_number].bit_width != disp_chn->intf_status[intf_id].bit_width) ||
            (format_param->intf_output[intf_number].pixel_fmt != disp_chn->intf_status[intf_id].pixel_fmt)) {
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

hi_s32 drv_disp_intf_suspend(hi_drv_display disp)
{
    hi_u32 intf_id = 0;
    hi_s32 ret = HI_SUCCESS;
    disp_intf_status *intf_status = HI_NULL;
    hi_drv_disp_intf intf;

    for (intf_id = HI_DRV_DISP_INTF_YPBPR0; intf_id < HI_DRV_DISP_INTF_ID_MAX; intf_id++) {
        intf_status = &g_disp_intf_mng.disp_intf_chn[disp].intf_status[intf_id];
        if ((intf_status->attached == HI_TRUE) && (intf_status->func_intf_ctrl.intf_suspend != HI_NULL)) {
            intf_type_hal_to_drv(intf_id, &intf);
            ret = intf_status->func_intf_ctrl.intf_suspend(intf, HI_NULL, HI_NULL);
            if (ret != HI_SUCCESS) {
                hi_err_disp("intf_suspend %d err(%x)\n", intf_id, ret);
                continue;
            }
        }
    }
    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_resume(hi_drv_display disp)
{
    hi_u32 intf_id = 0;
    hi_s32 ret = HI_SUCCESS;
    disp_intf_status *intf_status = HI_NULL;
    hi_drv_disp_intf intf;

    for (intf_id = HI_DRV_DISP_INTF_YPBPR0; intf_id < HI_DRV_DISP_INTF_ID_MAX; intf_id++) {
        intf_status = &g_disp_intf_mng.disp_intf_chn[disp].intf_status[intf_id];
        if ((intf_status->attached == HI_TRUE) && (intf_status->func_intf_ctrl.intf_resume != HI_NULL)) {
            intf_type_hal_to_drv(intf_id, &intf);
            ret = intf_status->func_intf_ctrl.intf_resume(intf, HI_NULL, HI_NULL);
            if (ret != HI_SUCCESS) {
                hi_err_disp("intf_resume %d err(%x)\n", intf_id, ret);
                continue;
            }
        }
    }

    return HI_SUCCESS;
}

hi_void disp_intf_attr_init(hi_drv_disp_intf_id intf_id, disp_intf_status *intf_status)
{
    intf_status->attached = HI_FALSE;
    intf_status->matching = HI_FALSE;
    intf_status->enable = HI_FALSE;
    intf_status->bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    intf_status->pixel_fmt = HI_DRV_DISP_PIXEL_YUV444;

    intf_status->intf_attr.id = HI_DRV_DISP_INTF_ID_MAX;
    intf_status->intf_attr.dac_y_g = HI_DISP_VDAC_INVALID_ID;
    intf_status->intf_attr.dac_pb_b = HI_DISP_VDAC_INVALID_ID;
    intf_status->intf_attr.dac_pr_r = HI_DISP_VDAC_INVALID_ID;
    intf_status->intf_attr.dac_sync = HI_FALSE;
    intf_status->is_support = HI_FALSE;

    if ((intf_id == HI_DRV_DISP_INTF_HDMI0) ||
        (intf_id == HI_DRV_DISP_INTF_HDMI1)) {
        intf_status->is_support = HI_TRUE;
    }

#ifdef HI_MIPI_SUPPORT
    if ((intf_id == HI_DRV_DISP_INTF_MIPI0) ||
        (intf_id == HI_DRV_DISP_INTF_MIPI1)) {
        intf_status->is_support = HI_TRUE;
    }
#endif

    if (intf_id == HI_DRV_DISP_INTF_CVBS0) {
        intf_status->is_support = HI_TRUE;
    }
}

hi_void disp_intf_get_hdmi_func(disp_intf_status *intf_status)
{
    hi_s32 ret;
    struct hi_hdmi_ext_intf *hdmi_intf = HI_NULL;
    disp_intf_func *hdmi_ext_func = HI_NULL;

    ret = hi_drv_module_get_func(HI_ID_HDMITX, (hi_void **)&hdmi_intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get hdmi func is err(%x)\n", ret);
        return;
    }
    hdmi_ext_func = (disp_intf_func *)(hdmi_intf->vo_data);

    intf_status->func_intf_ctrl.intf_suspend = hdmi_ext_func->intf_suspend;
    intf_status->func_intf_ctrl.intf_resume = hdmi_ext_func->intf_resume;
    intf_status->func_intf_ctrl.intf_check_validate = hdmi_ext_func->intf_check_validate;
    intf_status->func_intf_ctrl.intf_config = hdmi_ext_func->intf_config;
    intf_status->func_intf_ctrl.intf_attach = hdmi_ext_func->intf_attach;
    intf_status->func_intf_ctrl.intf_detach = hdmi_ext_func->intf_detach;
    intf_status->func_intf_ctrl.intf_atomic_config = hdmi_ext_func->intf_atomic_config;
    intf_status->func_intf_ctrl.intf_enable = hdmi_ext_func->intf_enable;
    intf_status->func_intf_ctrl.intf_prepare = hdmi_ext_func->intf_prepare;
}

#ifdef HI_MIPI_SUPPORT
hi_void disp_intf_get_mipi_func(disp_intf_status *intf_status)
{
    hi_s32 ret;
    mipi_func_export *mipi_intf = HI_NULL;
    ret = hi_drv_module_get_func(HI_ID_MIPI, (hi_void **)&mipi_intf);
    if (ret != HI_SUCCESS) {
        hi_err_disp("get mipi func is err(%x)\n", ret);
        return;
    }

    intf_status->func_intf_ctrl.intf_suspend = mipi_intf->intf_suspend;
    intf_status->func_intf_ctrl.intf_resume = mipi_intf->intf_resume;
    intf_status->func_intf_ctrl.intf_check_validate = mipi_intf->intf_check_validate;
    intf_status->func_intf_ctrl.intf_config = mipi_intf->intf_config;
    intf_status->func_intf_ctrl.intf_attach = mipi_intf->intf_attach;
    intf_status->func_intf_ctrl.intf_detach = mipi_intf->intf_detach;
    intf_status->func_intf_ctrl.intf_atomic_config = mipi_intf->intf_atomic_config;
    intf_status->func_intf_ctrl.intf_enable = mipi_intf->intf_enable;
    intf_status->func_intf_ctrl.intf_prepare = mipi_intf->intf_prepare;
}
#endif

hi_s32 disp_intf_get_module_func(hi_drv_disp_intf_id intf_id, disp_intf_status *intf_status)
{
    intf_status->func_intf_ctrl.intf_suspend = disp_suspend;
    intf_status->func_intf_ctrl.intf_resume = disp_resume;
    intf_status->func_intf_ctrl.intf_check_validate = disp_intf_check_validate;
    intf_status->func_intf_ctrl.intf_config = disp_intf_config;
    intf_status->func_intf_ctrl.intf_attach = disp_intf_attach;
    intf_status->func_intf_ctrl.intf_detach = disp_intf_detach;
    intf_status->func_intf_ctrl.intf_atomic_config = disp_intf_atomic_config;
    intf_status->func_intf_ctrl.intf_enable = disp_intf_enable;
    intf_status->func_intf_ctrl.intf_prepare = disp_intf_prepare;

    if ((intf_id == HI_DRV_DISP_INTF_HDMI0) || (intf_id == HI_DRV_DISP_INTF_HDMI1)) {
        disp_intf_get_hdmi_func(intf_status);
    }

#ifdef HI_MIPI_SUPPORT
    if ((intf_id == HI_DRV_DISP_INTF_MIPI0) || (intf_id == HI_DRV_DISP_INTF_MIPI1)) {
        disp_intf_get_mipi_func(intf_status);
    }
#endif

    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_init(hi_void)
{
    hi_u32 disp = 0;
    hi_u32 intf_id = 0;
    hi_s32 ret = 0;
    disp_intf_status *intf_status = HI_NULL;

    memset(&g_disp_intf_mng, 0, sizeof(disp_inttf_mng));

    for (disp = 0; disp < HI_DRV_DISPLAY_BUTT; disp++) {
        for (intf_id = HI_DRV_DISP_INTF_YPBPR0; intf_id < HI_DRV_DISP_INTF_ID_MAX; intf_id++) {
            intf_status = &g_disp_intf_mng.disp_intf_chn[disp].intf_status[intf_id];
            disp_intf_attr_init(intf_id, intf_status);
            ret = disp_intf_get_module_func(intf_id, intf_status);
            if (ret != HI_SUCCESS) {
                hi_err_disp("get intf(%d) module func is err\n", intf_id);
                return ret;
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_disp_intf_deinit(hi_void)
{
    memset(&g_disp_intf_mng, 0, sizeof(disp_inttf_mng));
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

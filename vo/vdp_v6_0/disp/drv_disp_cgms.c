
/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp cgms
* Create: 2016-04-12
 */

#include "drv_cgms.h"
#include "drv_mcvn.h"
#include "vdp_hal_comm.h"
#include "vdp_software_selfdefine.h"
//#include "hal_reg_cfg_manager.h"

s_vdp_com_regs_type *g_vdp_com_reg = HI_NULL;
s_vdp_monitor_regs_type *g_vdp_monitor_reg= HI_NULL;


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef __DISP_PLATFORM_BOOT__

static const  disp_cgms_fmt_cfg   g_cgms_cfg_table[] =
{
    {
        HI_DRV_DISP_FMT_1080i_50,
        {0x88003, 0x28083, 0xd8043, 0x780c3}
    },

    {
        HI_DRV_DISP_FMT_1080i_60,
        { 0x88003, 0x28083, 0xd8043, 0x780c3}
    },

    {
        HI_DRV_DISP_FMT_720P_50,
        {0x88003, 0x28083, 0xd8043, 0x780c3}
    },

    {
        HI_DRV_DISP_FMT_720P_60,
        { 0x88003, 0x28083, 0xd8043, 0x780c3}
    },

    {
        HI_DRV_DISP_FMT_480P_60,
        {0x88003, 0x28083,  0xd8043, 0x780c3}
    },

    {
        HI_DRV_DISP_FMT_576P_50,
        {0x0008, 0x2008, 0x1008, 0x3008}
    },

    {
        HI_DRV_DISP_FMT_NTSC,
        {0x18000, 0xb8080, 0x50040, 0xe80c0}
    },

    {
        HI_DRV_DISP_FMT_NTSC_J,
        {0x18000,  0xb8080, 0x50040, 0xe80c0}
    },

    {
        HI_DRV_DISP_FMT_PAL,
        {0x0008,  0x2008,  0x1008,  0x3008}
    },

    {
        HI_DRV_DISP_FMT_PAL_N,
        { 0x0008, 0x2008, 0x1008, 0x3008}
    },

    {
        HI_DRV_DISP_FMT_PAL_Nc,
        {0x0008, 0x2008, 0x1008, 0x3008}
    }

};

hi_s32 drv_cgms_init(HI_DRV_DISPLAY_E disp, HI_DRV_DISP_FMT_E fmt, disp_cgms_info* cgms)
{
    CGMS_CHECK_NULL_POINTER(cgms);

    cgms->enable = HI_FALSE;

    cgms->attached_disp = disp;

    cgms->type = HI_DRV_DISP_CGMSA_A;

    cgms->mode = HI_DRV_DISP_CGMSA_COPY_FREELY;

    cgms->cur_format = fmt;

    return HI_SUCCESS;
}

hi_s32 drv_cgms_de_init(disp_cgms_info *cgms)
{
    return HI_SUCCESS;
}

hi_s32 disp_cgms_set_enable(HI_DRV_DISP_FMT_E format, const HI_DRV_DISP_CGMSA_CFG_S *cgms_cfg)
{
    /* Type A of Sdate with PAL format depend on WSS setting */
    if (HI_DRV_DISP_CGMSA_A == cgms_cfg->type)
    {
        if (format == HI_DRV_DISP_FMT_PAL
            || format == HI_DRV_DISP_FMT_PAL_N
            || format == HI_DRV_DISP_FMT_PAL_Nc)
        {
            sdate_hdate_vbi_set_wss_en(DISP_VENC_SDATE0, cgms_cfg->enable);
        }
        else
        {
            sdate_hdate_vbi_set_wss_en(DISP_VENC_SDATE0, HI_FALSE);
            sdate_hdate_vbi_set_cgms_type_aen(DISP_VENC_HDATE0, cgms_cfg->enable);
            sdate_hdate_vbi_set_cgms_type_aen(DISP_VENC_SDATE0, cgms_cfg->enable);
        }
    }
    else if (HI_DRV_DISP_CGMSA_B == cgms_cfg->type)
    {
        vdp_vbi_set_cgms_type_ben(DISP_VENC_HDATE0, cgms_cfg->enable);
    }
    else
    {
        HI_ERR_DISP("DISP does not support the macrovision type:%d!\n", cgms_cfg->type);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 disp_cgms_set_type_a(disp_venc channel, HI_DRV_DISP_FMT_E format, const HI_DRV_DISP_CGMSA_CFG_S *cgms_cfg)
{
    hi_u32 data = 0;
    hi_u32 index = 0;
    hi_bool find_fmt_flag = HI_FALSE;

    for (; index < sizeof(g_cgms_cfg_table) / sizeof(disp_cgms_fmt_cfg); index++)
    {
        if (g_cgms_cfg_table[index].fmt == format)
        {
            find_fmt_flag = HI_TRUE;
            data = g_cgms_cfg_table[index].au32_cfg_data[cgms_cfg->mode ];
            break;
        }
    }

    if (HI_TRUE != find_fmt_flag)
    {
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (format == HI_DRV_DISP_FMT_PAL_N
        || format == HI_DRV_DISP_FMT_PAL_Nc
        || format == HI_DRV_DISP_FMT_PAL)
    {
        sdate_hdate_vbi_set_wss_data(channel, data);
    }
    else
    {
        sdate_hdate_vbi_set_cgms_type_a(channel, data);
    }

    return HI_SUCCESS;

}

hi_s32 disp_cgms_set_type_b(disp_cgms_info *cgms,HI_DRV_DISP_FMT_E format,HI_DRV_DISP_CGMSA_CFG_S cgms_cfg)
{
    HI_ERR_DISP("CGMS_B is unsupported\n",format);

    return HI_ERR_DISP_INVALID_OPT;
}

static hi_void disp_set_cgms_cfg_to_com_reg(const HI_DRV_DISP_CGMSA_CFG_S *cgms_cfg)
{
    u_analog_ctrl analog_ctrl;

    analog_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32));
    analog_ctrl.bits.cgms_en    = cgms_cfg->enable;
    analog_ctrl.bits.cgms_type  = cgms_cfg->type;
    analog_ctrl.bits.cgms_mode  = cgms_cfg->mode;
    vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);

    return;
}

static hi_void disp_get_cgms_cfg_from_com_reg(HI_DRV_DISP_CGMSA_CFG_S *cgms_cfg)
{
    u_analog_ctrl analog_ctrl;

    analog_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32));
    cgms_cfg->enable = analog_ctrl.bits.cgms_en;
    cgms_cfg->type  = analog_ctrl.bits.cgms_type;
    cgms_cfg->mode  = analog_ctrl.bits.cgms_mode;

    return;
}

static hi_s32 disp_get_date_attach_status_from_com_reg(disp_venc date_type, HI_DRV_DISPLAY_E *pen_disp)
{
    u_analog_ctrl analog_ctrl;
    unsigned int date_attach;

    analog_ctrl.u32 = vdp_reg_read(((uintptr_t)&(g_vdp_com_reg->analog_ctrl.u32)));

    if (DISP_VENC_SDATE0 == date_type)
    {
        date_attach = analog_ctrl.bits.sdate_attach;
    }
    else if (DISP_VENC_HDATE0 == date_type)
    {
        date_attach = analog_ctrl.bits.hdate_attach;
    }
    else
    {
        HI_ERR_DISP("Unsupport date type :%d\n", date_type);
        return HI_FAILURE;
    }

    /* Accroding to common register define: 0-no_attaching, 0x1-DISP0, 0x2-DISP1 */
    switch (date_attach)
    {
        case 0x1 :
            *pen_disp = HI_DRV_DISPLAY_1;
            break;
        case 0x2 :
            *pen_disp = HI_DRV_DISPLAY_0;
            break;
        case 0x0 :
            *pen_disp = HI_DRV_DISPLAY_BUTT;
        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 disp_get_disp_fmt_from_com_reg(HI_DRV_DISPLAY_E disp, HI_DRV_DISP_FMT_E *fmt)
{
    u_analog_ctrl analog_ctrl;

    /* Pick REE format from register */
    analog_ctrl.u32 = vdp_reg_read(((uintptr_t)&(g_vdp_com_reg->analog_ctrl.u32)));

    if (HI_DRV_DISPLAY_1 == disp)
    {
        if (analog_ctrl.bits.disp0_fmt >= HI_DRV_DISP_FMT_BUTT)
        {
            HI_ERR_DISP("disp0_fmt=%d is out of range !", analog_ctrl.bits.disp0_fmt);
            return HI_FAILURE;
        }
        *fmt = analog_ctrl.bits.disp0_fmt;
    }
    else if (HI_DRV_DISPLAY_0 == disp)
    {
        if (analog_ctrl.bits.disp1_fmt >= HI_DRV_DISP_FMT_BUTT)
        {
            HI_ERR_DISP("disp1_fmt=%d is out of range !", analog_ctrl.bits.disp1_fmt);
            return HI_FAILURE;
        }
        *fmt = analog_ctrl.bits.disp1_fmt;
    }
    else
    {
        HI_ERR_DISP("DISP enum out of range : DISP=%d.", disp);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


hi_s32 disp_get_xdate_info_from_com_reg(disp_venc date, disp_x_date_com_reg_info* disp_x_date_info)
{
    HI_DRV_DISPLAY_E disp;

    if (HI_SUCCESS != disp_get_date_attach_status_from_com_reg(date, &disp))
    {
        return HI_FAILURE;
    }
    else
    {
        disp_x_date_info->disp = disp;
    }

    if (HI_SUCCESS != disp_get_disp_fmt_from_com_reg(disp, &disp_x_date_info->format))
    {
        HI_ERR_DISP(" Call DISP_GetDispFmt function failed, Disp=%d, DateFmt=%d !\n", disp, disp_x_date_info->format);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_cgms_set(HI_DRV_DISP_FMT_E sdate_fmt, HI_DRV_DISP_FMT_E hdate_fmt, const HI_DRV_DISP_CGMSA_CFG_S *cgms_cfg)
{
    if (sdate_fmt >= HI_DRV_DISP_FMT_BUTT)
    {
        HI_ERR_DISP("Invalid format %d\n", sdate_fmt);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (hdate_fmt >= HI_DRV_DISP_FMT_BUTT)
    {
        HI_ERR_DISP("Invalid format %d\n", hdate_fmt);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (cgms_cfg->type >= HI_DRV_DISP_CGMSA_TYPE_BUTT)
    {
        HI_ERR_DISP("Invalid Type %d\n", cgms_cfg->type);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (cgms_cfg->mode >= HI_DRV_DISP_CGMSA_MODE_BUTT)
    {
        HI_ERR_DISP("Invalid Mode %d\n", cgms_cfg->mode);
        return HI_ERR_DISP_INVALID_PARA;
    }

    disp_cgms_set_enable(sdate_fmt, cgms_cfg);

    disp_set_cgms_cfg_to_com_reg(cgms_cfg);

    if (cgms_cfg->enable == HI_FALSE)
    {
        return HI_SUCCESS;
    }

    if (cgms_cfg->type == HI_DRV_DISP_CGMSA_A)
    {
        (hi_void)disp_cgms_set_type_a(DISP_VENC_SDATE0, sdate_fmt, cgms_cfg);
        (hi_void)disp_cgms_set_type_a(DISP_VENC_HDATE0, hdate_fmt, cgms_cfg);
    }
    else
    {
        HI_ERR_DISP("Type %d can't be supported\n", cgms_cfg->type);

        return HI_ERR_DISP_INVALID_OPT;
    }

    return HI_SUCCESS;
}

hi_s32 drv_cgms_get(disp_cgms_state *cur_state)
{
    HI_DRV_DISP_CGMSA_CFG_S cgms_cfg = {0};

    disp_get_cgms_cfg_from_com_reg(&cgms_cfg);

    if (cgms_cfg.mode >= HI_DRV_DISP_CGMSA_MODE_BUTT
        || cgms_cfg.type >= HI_DRV_DISP_CGMSA_TYPE_BUTT)
    {
        HI_ERR_DISP("Mode=%d or Type=%d is out of range !\n", cgms_cfg.mode, cgms_cfg.type);
        return HI_FAILURE;
    }

    cur_state->enable = cgms_cfg.enable;
    cur_state->mode  = cgms_cfg.mode;
    cur_state->type  = cgms_cfg.type;

    return HI_SUCCESS;
}


#else
hi_s32 drv_cgms_init(HI_DRV_DISPLAY_E disp,HI_DRV_DISP_FMT_E fmt,disp_cgms_info *cgms)
{
    return HI_SUCCESS;
}

hi_s32 drv_cgms_de_init(disp_cgms_info *cgms)
{
    return HI_SUCCESS;
}

hi_s32 drv_cgms_set(HI_DRV_DISP_FMT_E sdate_fmt, HI_DRV_DISP_FMT_E hdate_fmt, HI_DRV_DISP_CGMSA_CFG_S cgms_cfg)
{
    return HI_SUCCESS;
}

hi_s32 drv_cgms_get(disp_cgms_state *cur_state)
{
    return HI_SUCCESS;
}
#endif

hi_void drv_set_date_attach_to_com_reg(disp_venc date, HI_DRV_DISPLAY_E disp)
{
    u_analog_ctrl analog_ctrl;
    unsigned int date_attach;

    analog_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32));

    /* Accroding to common register define: 0-no_attaching, 0x1-DISP0, 0x2-DISP1 */
    switch (disp)
    {
        case HI_DRV_DISPLAY_1 :
            date_attach = 0x1;
            break;
        case HI_DRV_DISPLAY_0 :
            date_attach = 0x2;
            break;
        case HI_DRV_DISPLAY_BUTT :
            date_attach = 0x0;
            break;
        default:
            return;
    }

    if (DISP_VENC_SDATE0 == date)
    {
        analog_ctrl.bits.sdate_attach = date_attach;
        vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);
    }
    else if (DISP_VENC_HDATE0 == date)
    {
        analog_ctrl.bits.hdate_attach = date_attach;
        vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);
    }
    else
    {
        return;
    }

    return;
}

hi_void drv_set_disp_fmt_to_com_reg(HI_DRV_DISPLAY_E disp, HI_DRV_DISP_FMT_E fmt)
{
    u_analog_ctrl analog_ctrl;

    analog_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32));

    if (HI_DRV_DISPLAY_1 == disp)
    {
        analog_ctrl.bits.disp0_fmt = fmt;
        vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);
    }
    else if (HI_DRV_DISPLAY_0 == disp)
    {
        analog_ctrl.bits.disp1_fmt = fmt;
        vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);
    }
    else
    {
        HI_INFO_DISP("DISP enum out of range : DISP=%d.", disp);
    }

    return;
}

hi_void drv_init_com_reg(hi_void)
{
    u_analog_ctrl analog_ctrl;

    analog_ctrl.u32 = 0x0;
    vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);
}

hi_void drv_get_tee_video_monitor_ctrl(hi_u16 *pu16_down_scale_w,hi_u16 *pu16_down_scale_h)
{
#if defined(CHIP_TYPE_hi3798mv200) || defined(CHIP_TYPE_hi3796mv200)
    u_monitor_ctrl  monitor_ctrl;

    monitor_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_monitor_reg->monitor_ctrl.u32));
    *pu16_down_scale_w = monitor_ctrl.bits.downscale_w ;
    *pu16_down_scale_h = monitor_ctrl.bits.downscale_h ;
#else
    *pu16_down_scale_w = 0;
    *pu16_down_scale_h = 0;
#endif

    return ;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

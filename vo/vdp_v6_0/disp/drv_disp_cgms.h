
/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp cgms
* Create: 2016-04-12
 */
#ifndef __DRV_CGMS_H__
#define __DRV_CGMS_H__


#include "hi_type.h"

#ifndef __DISP_PLATFORM_BOOT__
#include "drv_disp_isr.h"
#endif

#include "drv_disp_hal.h"
#include "hi_drv_disp.h"

#define CGMS_CHECK_NULL_POINTER(ptr) \
    {                                \
        if (ptr == HI_NULL)                    \
        {                            \
            DISP_ERROR("DISP ERROR! Input null pointer in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_NULL_PTR;  \
        }                             \
    }

typedef struct hi_disp_cgms_info_
{
#ifndef __DISP_PLATFORM_BOOT__
    hi_bool enable;
    HI_DRV_DISPLAY_E attached_disp;
    HI_DRV_DISP_CGMSA_TYPE_E  type;
    HI_DRV_DISP_CGMSA_MODE_E  mode;
    HI_DRV_DISP_FMT_E cur_format;
#endif
}disp_cgms_info;


typedef struct hi_disp_cgms_state_
{
#ifndef __DISP_PLATFORM_BOOT__
    hi_bool enable;
    HI_DRV_DISPLAY_E attached_disp;
    HI_DRV_DISP_CGMSA_TYPE_E  type;
    HI_DRV_DISP_CGMSA_MODE_E  mode;
#endif
}disp_cgms_state;

typedef struct hi_disp_cgms_fmt_cfg_
{
    HI_DRV_DISP_FMT_E fmt;
    hi_u32  au32_cfg_data[HI_DRV_DISP_CGMSA_MODE_BUTT];
} disp_cgms_fmt_cfg;

typedef struct tag_disp_x_date_com_reg_info
{
    HI_DRV_DISPLAY_E disp;
    HI_DRV_DISP_FMT_E format;
}disp_x_date_com_reg_info;

// Define the union U_ANALOG_CTRL, Get REE config from this REG
#ifdef VDP_TEE_OLD_ANALOG_CTRL_REG_SUPPORT
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cgms_en             : 1   ; // [0]
        unsigned int    cgms_type           : 3   ; // [3..1]
        unsigned int    cgms_mode           : 3   ; // [6..4]
        unsigned int    mcvn_en             : 1   ; // [7]
        unsigned int    mcvn_mode           : 4   ; // [11..8]
        unsigned int    disp0_fmt           : 6   ; // [17..12]
        unsigned int    disp1_fmt           : 6   ; // [23..18]
        unsigned int    sdate_attach        : 2   ; // [25..24]     0-no_attaching, 0x1-DISP0, 0x2-DISP1
        unsigned int    hdate_attach        : 2   ; // [27..26]     0-no_attaching, 0x1-DISP0, 0x2-DISP1
        unsigned int    tee_hd_video_mute   : 1   ; // [28]     tee hd video mute status
        unsigned int    ree_cfg_downscale   : 1   ; // [29]     ree hd video mute status
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} u_analog_ctrl;
#else
typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    cgms_en             : 1   ; // [0]
        unsigned int    cgms_type           : 3   ; // [3..1]
        unsigned int    cgms_mode           : 3   ; // [6..4]
        unsigned int    mcvn_en             : 1   ; // [7]
        unsigned int    mcvn_mode           : 4   ; // [11..8]
        unsigned int    disp0_fmt           : 7   ; // [18..12]
        unsigned int    disp1_fmt           : 7   ; // [25..19]
        unsigned int    sdate_attach        : 2   ; // [27..26]     0-no_attaching, 0x1-DISP0, 0x2-DISP1
        unsigned int    hdate_attach        : 2   ; // [29..28]     0-no_attaching, 0x1-DISP0, 0x2-DISP1
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} u_analog_ctrl;
#endif

//==============================================================================
/* Define the global struct */
typedef struct
{
    u_analog_ctrl analog_ctrl;
} s_vdp_com_regs_type;

extern s_vdp_com_regs_type *g_vdp_com_reg;

typedef union
{
    // Define the struct bits
    struct
    {
        unsigned int    downscale_w         : 16   ; // [0:15]
        unsigned int    downscale_h         : 16   ; // [16:31]
    } bits;

    // Define an unsigned member
    unsigned int    u32;

} u_monitor_ctrl;

//==============================================================================
/* Define the global struct */
typedef struct
{
    u_monitor_ctrl monitor_ctrl;
} s_vdp_monitor_regs_type;

extern s_vdp_monitor_regs_type *g_vdp_monitor_reg;

#ifndef __DISP_PLATFORM_BOOT__

hi_s32 drv_cgms_init(HI_DRV_DISPLAY_E disp, HI_DRV_DISP_FMT_E fmt, disp_cgms_info* cgms);

hi_s32 drv_cgms_de_init(disp_cgms_info *cgms);

hi_s32 drv_cgms_set(HI_DRV_DISP_FMT_E sdate_fmt, HI_DRV_DISP_FMT_E hdate_fmt, const HI_DRV_DISP_CGMSA_CFG_S *cgms_cfg);

hi_s32 drv_cgms_get(disp_cgms_state *cur_state);

hi_s32 disp_get_xdate_info_from_com_reg(disp_venc date, disp_x_date_com_reg_info* disp_x_date_info);

#endif

hi_void drv_set_date_attach_to_com_reg(disp_venc date, HI_DRV_DISPLAY_E disp);

hi_void drv_set_disp_fmt_to_com_reg(HI_DRV_DISPLAY_E disp, HI_DRV_DISP_FMT_E fmt);

hi_void drv_init_com_reg(hi_void);

hi_void drv_get_tee_video_monitor_ctrl(hi_u16 *pu16_down_scale_w,hi_u16 *pu16_down_scale_h);

#endif

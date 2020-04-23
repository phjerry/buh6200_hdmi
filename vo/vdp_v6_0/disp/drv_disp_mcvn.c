
/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp macrvision
* Create: 2016-04-12
 */
#include "drv_mcvn.h"
#include "drv_cgms.h"
#include "vdp_hal_comm.h"
#include "vdp_software_selfdefine.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
hi_s32 drv_mcvn_set_date_mcvn(HI_DRV_DISP_FMT_E fmt, HI_DRV_DISP_MACROVISION_E mcvn);


static hi_u32 g_macrovision_coef[6][4][5]=
{
#if 0
    {
        {0x0,       0x0,      0x0,     0x0,       0x0},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10607efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10607efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10607efe,0x800aaf40}
    },    /*PAL  PAL_N  PAL_Nc */
#else
    // 2011.9.17 ange for zg by caobin, according to new macrovision standard
    {
        {0x0,       0x0,      0x0,     0x0,       0x0},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10407efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10607efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10507efe,0x800aaf40}
    },    /*PAL  PAL_N  PAL_Nc */

#endif

#if 0
    {
        {0x0,       0x0,      0x0,  0x0,       0x0},
        {0x46545d36,0x246db39,0x7f8,0x15600f0f,0x801ffc50},
        {0x46545d3e,0x246db39,0x7f8,0x15600f0f,0x801ffc50},
        {0x5615573e,0x246dbad,0x7f8,0x15500f0f,0x801ffc50}
    },    /*NTSC NTSC_J */
#else
    // 2011.9.17 ange for zg by caobin, according to new macrovision standard
    {
        {0x0,       0x0,      0x0,  0x0,       0x0},
        {0x46545d36,0x246db39,0x7f8,0x15400f0f,0x801ffc50},
        {0x46545d3e,0x246db39,0x7f8,0x15600f0f,0x801ffc50},
        {0x5615573e,0x246dbad,0x7f8,0x15500f0f,0x801ffc50}
    },    /*NTSC NTSC_J */

#endif

    {
        {0x0,       0x0,      0x0,  0x0,       0x0},
        {0x56155736,0x246dbad,0x7f8,0x15600f0f,0x801ffc50},
        {0x46545d36,0x246db39,0x7f8,0x15600f0f,0x801ffc50},
        {0x56155736,0x246dbad,0x7f8,0x15500f0f,0x801ffc50}
    },    /* NTSC_PAL_M*/

#if 0
    {
        {0x0,       0x0,      0x0,     0x0,   0x0       },
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10507efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10507efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10507efe,0x800aaf40}
    },    /*SECAM */
#else
    // 2011.9.17 ange for zg by caobin, according to new macrovision standard
    {
        {0x0,       0x0,      0x0,     0x0,   0x0       },
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10407efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10607efe,0x800aaf40},
        {0x8aa89a36,0x14f5c15,0xaa03fe,0x10407efe,0x800aaf40}
    },    /*SECAM SIN/COS */
#endif

    {
        {0x0,       0x0,      0x0,  0x0,       0x0     },
        {0x56155736,0x246dbad,0x7f8,0x15601e1e,0x1ffc50},
        {0x56155736,0x246dbad,0x7f8,0x15601e1e,0x1ffc50},
        {0x56155736,0x246dbad,0x7f8,0x15601e1e,0x1ffc50}
    },  /*PAL_60*/

    {
        {0x0,       0x0,      0x0,  0x0,       0x0     },
        {0x56155736,0x246dbad,0x7f8,0x15500f0f,0x1ffc50},
        {0x56155736,0x246dbad,0x7f8,0x15500f0f,0x1ffc50},
        {0x56155736,0x246dbad,0x7f8,0x15500f0f,0x1ffc50}
    },  /*NTSC_443*/


};

hi_s32 drv_mcvn_init(HI_DRV_DISPLAY_E disp,HI_DRV_DISP_FMT_E fmt)
{
    vdp_mvcn_set_mcvn_enable(HI_FALSE);

    drv_mcvn_set_date_mcvn(fmt, HI_DRV_DISP_MACROVISION_TYPE0);

    return HI_SUCCESS;
}

hi_s32 drv_mcvn_de_init(HI_DRV_DISPLAY_E disp)
{
    return HI_SUCCESS;
}

static hi_void disp_set_mcvn_cfg_to_com_reg(HI_DRV_DISP_MACROVISION_E mode)
{
    u_analog_ctrl analog_ctrl;

    analog_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32));
    analog_ctrl.bits.mcvn_mode  = mode;
    vdp_reg_write(((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32)), analog_ctrl.u32);

    return;
}

static hi_void disp_get_mcvn_cfg_from_com_reg(HI_DRV_DISP_MACROVISION_E *pen_mode)
{
    u_analog_ctrl analog_ctrl;

    analog_ctrl.u32 = vdp_reg_read((uintptr_t) & (g_vdp_com_reg->analog_ctrl.u32));
    *pen_mode = analog_ctrl.bits.mcvn_mode;

    return;
}

hi_s32 drv_mcvn_get_date_mcvn(HI_DRV_DISP_MACROVISION_E* pen_mcvn)
{
    disp_get_mcvn_cfg_from_com_reg(pen_mcvn);

    return HI_SUCCESS;
}

hi_s32 drv_mcvn_set_date_mcvn(HI_DRV_DISP_FMT_E fmt, HI_DRV_DISP_MACROVISION_E mcvn)
{
    hi_u32 *mcvn_date = HI_NULL;

    switch (fmt)
    {
        case HI_DRV_DISP_FMT_SECAM_COS:
        case HI_DRV_DISP_FMT_SECAM_SIN:

            mcvn_date = g_macrovision_coef[3][mcvn];
            break;

        case HI_DRV_DISP_FMT_PAL_M:

            mcvn_date = g_macrovision_coef[2][mcvn];
            break;

        case HI_DRV_DISP_FMT_NTSC:
        case HI_DRV_DISP_FMT_NTSC_J:

            mcvn_date = g_macrovision_coef[1][mcvn];
            break;

        case HI_DRV_DISP_FMT_PAL:
        case HI_DRV_DISP_FMT_PAL_N:
        case HI_DRV_DISP_FMT_PAL_Nc:

            mcvn_date = g_macrovision_coef[0][mcvn];
            break;

        default:
            mcvn_date = g_macrovision_coef[0][0];
            break;
    }

    sdate_mvcn_date_set_mcvn(mcvn_date);

    return HI_SUCCESS;
}

hi_s32 drv_mcvn_set_macrovision(HI_DRV_DISP_MACROVISION_E mode, HI_DRV_DISP_FMT_E fmt)
{
    if (HI_DRV_DISP_MACROVISION_TYPE3  < mode)
    {
        HI_ERR_DISP("DISP does not support the macrovision type!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    /* Set Hdate macrovision */
    if(mode == HI_DRV_DISP_MACROVISION_TYPE0)
    {
        vdp_mvcn_set_mcvn_enable(HI_FALSE);
    }
    else if(  (mode >= HI_DRV_DISP_MACROVISION_TYPE1)
            &&(mode <= HI_DRV_DISP_MACROVISION_TYPE3))
    {
        vdp_mvcn_set_mcvn_enable(HI_TRUE);
    }
    else
    {
        HI_ERR_DISP("HD DISP does not support the macrovision type!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    /* Set Sdate macrovision */
    drv_mcvn_set_date_mcvn(fmt, mode);

    disp_set_mcvn_cfg_to_com_reg(mode);

    return HI_SUCCESS;
}

hi_s32 drv_mcvn_get_macrovision(HI_DRV_DISP_MACROVISION_E *pen_mode)
{
    /* Get macrovision infomation from COM-Reg */
    drv_mcvn_get_date_mcvn(pen_mode);

    if (*pen_mode >= HI_DRV_DISP_MACROVISION_BUTT)
    {
        HI_ERR_DISP("Mode=%d is out of range !\n", *pen_mode);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


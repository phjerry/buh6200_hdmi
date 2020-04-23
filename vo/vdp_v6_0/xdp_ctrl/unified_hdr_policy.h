/*
* Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: ion  adapter code
* Author: image
* Create: 2019-03-28
 */

#ifndef WIN_LAYER_MAPPING_H
#define WIN_LAYER_MAPPING_H
#include "hi_type.h"
#include "hi_drv_disp.h"

typedef enum {
    HDR_OUTPUTMAPPING_V0 = 0x0,
    HDR_OUTPUTMAPPING_VP,
    HDR_OUTPUTMAPPING_CBM,
    DRV_XDR_ENGINE_BUTT
} hdr_outputmapping_location;

typedef struct hihdr_policy_input_info {
    hi_u32 win_num;                                /* do not support dymamic-mentadata,
        not support hdr10plus, not support dolby output */
    hi_bool is_program_change;                     /* need do hdr-auto adapter again */
    hi_u32 is_littlewin_exist;                     /* do not support hdr-auto adapter again. */
    hi_u32 is_hdr_automode;                        /* user setting: whether do hdr-auto adjust. */
    hi_u32 usersetting_disp_out_hdrtype;           /* user setting disp output hdr. */
    hi_u32 current_dolby_win_num;                  /* according the frame infor. */
    hi_u32 is_hdr10_plus_exist;                    /* according the frame info */
    HI_DRV_DISP_SINK_CAPABILITY_S hdmi_capability; /* when force or auto, we give a optimize choice to the user. */
    HI_DRV_DISP_XDR_ENGINE_E hdr_engine;           /* in different engine, different result will occure. */
    hi_u32 main_win_previous_frame_hdrtype;
    hi_u32 main_win_current_frame_hdrtype;
    HI_DRV_COLOR_SPACE_E csc_type;
    hi_u32 last_judgement_hdr_result /* one play, up not down,only when changed */,
           hi_bool is_second_display_exist; /* do not support dolby; */
    hi_bool is_main_window_has_frame;    /* when auto-mode, maybe no frame exist in main window. */
    hi_bool is_hifb_single_buf;          /* when only one fb buf, just output the sdr for hwc and gpu */

    hi_bool user_force_setting;
} hdr_policy_input_info;

typedef struct hihdr_policy_result_info {
    hi_drv_disp_out_type disp_output_hdr_type;
    hi_drv_disp_colorpace disp_output_csc_type;
    hi_bool is_dynamic_mentata;
    hdr_outputmapping_location hdr_outputmapping_loacation;
} hdr_policy_result_info;

hi_s32 xdpctrl_get_hdr_engine(hi_drv_disp_xdr_engine *hdr_engine);

/* hi_bool is_program_change;  for dvb tv, change the tv, do auto again. */
hi_s32 xdpctrl_get_hdr_type(hdr_policy_input_info *hdr_policy_refer_info,
                            hdr_policy_result_info *hdr_policy_result);


#endif

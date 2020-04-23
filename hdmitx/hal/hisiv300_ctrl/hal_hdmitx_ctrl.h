/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver hdmi controller aon reg header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMI_CTRL_H__
#define __HAL_HDMI_CTRL_H__

#include "drv_hdmitx_modes.h"
#include "drv_hdmi_module_ext.h"
#include "drv_hdmitx_infoframe.h"
#include "drv_hdmitx_debug.h"

struct hisilicon_hdmi;
struct dsc_ctrl;
struct train;

/* audio_mute_type define */
#define HDMI_AUDIO_MUTE_TYPE_IN   0
#define HDMI_AUDIO_MUTE_TYPE_DATA 1
#define HDMI_AUDIO_MUTE_TYPE_SF   2

/* work_mode define */
#define HDMI_WORK_MODE_TMDS    0
#define HDMI_WORK_MODE_FRL     1

/* work_mode_prior define */
#define HDMI_PRIOR_AUTIO       0
#define HDMI_PRIOR_FORCE_TMDS  1
#define HDMI_PRIOR_FORCE_FRL   2

/* inforame buffer size define */
#define HDMI_INFOFRAME_BUF_SIZE 32

#define DSC_EM_PKG_LEN 186

typedef struct {
    hi_u8 raw_data[DSC_EM_PKG_LEN];
    struct hdmi_hdmi_dsc_ext_metadata em_data;
} em_data_info;

typedef struct {
    hi_u32 input_type;
    bool is_pcm_compress;
    hi_u32 input_len;
    hi_u32 out_bit_len;
    hi_u32 chn_num;
    hi_u32 sample_rate_num;
    hi_u32 reg_n;
    hi_u32 reg_cts;
} audio_path;

typedef struct {
    hi_u32 out_color_fmt;
    hi_u32 in_color_depth;
    hi_u32 out_color_depth;
} video_path;

typedef struct {
    hi_u32 n_value;
    hi_u32 cts_value;
} ncts_val;

struct hw_timing {
    hi_bool de_polarity;
    hi_bool vsync_polarity;
    hi_bool hsync_polarity;
    hi_bool interlaced;
    hi_u32 hsync_total;
    hi_u32 hsync_active;
    hi_u32 vsync_total;
    hi_u32 vsync_active;
};

struct hdmi_hw_config {
    hi_u32 work_mode;

    hi_bool tmds_scr_en;
    hi_u32 tmds_clock;
    hi_bool dvi_mode;

    hi_u32 cur_frl_rate;

    hi_u32 min_frl_rate;
    hi_u32 max_frl_rate;
    hi_u32 min_dsc_frl_rate;

    hi_bool dsc_enable;
    hi_u32 bpp_target;
    hi_u32 slice_width;
    hi_u32 hcactive;
    hi_u32 hcblank;
    hi_bool frl_train_success;
};

struct hdmi_controller {
    hi_s32 version;
    hi_u32 audio_mute_type;
    /*
     * Only for debug,work mode prior in both capale frl & tmds.
     * see macro HDMI_PRIOR_XXX.
     * 0--auto select prior strategy: [108,255] prior frl; or prior tmds
     * 1--force tmds prior
     * 2--force frl prior
     */
    hi_u32 work_mode_prior;
    /*
     * @tmds_prior: tmds work mode prior.
     */
    hi_bool tmds_prior;
    hi_bool min_rate_prior;
    hi_bool tpll_enable;
    hi_bool ppll_enable;
    hi_bool dsc_force;
    hi_bool force_output;
    hi_u32 force_frl_rate;
    struct hisilicon_hdmi *parent;
    struct hi_display_mode mode;
    struct ao_attr attr;
    struct dsc_ctrl *dsc;
    struct frl *frl;
    struct hdmi_hw_config cur_hw_config;
    struct hdmi_hw_config pre_hw_config;
};

/*
 * hdmi_controller_init - create an controller structure based on the hdmi and init it
 * @hdmi: The base hdmi structure
 * Based on the hdmi structure, after device parsing, we get the configuration
 * for the controller, controller version, controller feature, and etc. then we
 * can create a new controller or just reference a controller, after that, init the controller
 * hardware if nessary.
 * Return: The targe controller or null if the required controller is not specified.
 */
struct hdmi_controller *hal_ctrl_controller_init(struct hisilicon_hdmi *hdmi);
void hal_ctrl_set_video_black(struct hdmi_controller *ctrl, hi_bool enable);
hi_s32 hal_ctrl_set_video_path(struct hdmi_controller *ctrl);
hi_s32 hal_ctrl_disable_deepcolor_for_dsc(struct hdmi_controller *ctrl);
hi_s32 hal_ctrl_set_audio_path(struct hdmi_controller *ctrl);
hi_s32 hal_ctrl_set_audio_ncts(struct hdmi_controller *ctrl);
void hal_ctrl_audio_enable(struct hdmi_controller *ctrl, hi_bool enable);
hi_s32 hal_ctrl_set_infoframe(struct hdmi_controller *ctrl);
hi_s32 hal_ctrl_set_mode(struct hdmi_controller *ctrl);
hi_s32 hal_ctrl_set_dsc_emp(struct hdmi_controller *ctrl, hi_bool enable);
hi_void hal_ctrl_set_avmute(struct hdmi_controller *ctrl, hi_bool enable);
hi_void hal_ctrl_pwd_soft_reset(struct hdmi_controller *ctrl);
hi_void hal_ctrl_fast_reset(struct hdmi_controller *ctrl);
hi_s32 hal_ctrl_tmds_set_scramble(struct hdmi_controller *ctrl);
hi_bool hal_ctrl_tmds_clk_is_stable(struct hdmi_controller *ctrl);
hi_void hal_ctrl_controller_deinit(struct hdmi_controller *ctrl);
hi_void hal_ctrl_hw_get_display_mode(struct hdmi_controller *ctrl);
hi_bool hal_ctrl_hw_get_source_scramble(struct hisilicon_hdmi *hdmi);
hi_u32 hal_ctrl_hw_get_vic(struct hisilicon_hdmi *hdmi);
hi_u32 hal_ctrl_hw_get_color_format(struct hisilicon_hdmi *hdmi);
hi_bool hal_ctrl_hw_get_hdmi_mode(struct hisilicon_hdmi *hdmi);
hi_void hal_ctrl_hw_get_info_frame(struct hisilicon_hdmi *hdmi, hi_s32 type, hi_u8 *buffer, hi_u8 buffer_len);
hi_void hal_ctrl_hw_get_detect_timing(struct hisilicon_hdmi *hdmi, struct hw_timing *timing);
hi_void hal_ctrl_hw_send_info_frame(struct hisilicon_hdmi *hdmi, hi_u8 *buffer, hi_u8 len);
hi_s32 hal_ctrl_get_audio_path(struct hisilicon_hdmi *hdmi, audio_path *audio_path_info);
hi_s32 hal_ctrl_get_video_path(struct hisilicon_hdmi *hdmi, video_path *video_path_info);
hi_s32 hal_ctrl_get_dsc_emp(struct hisilicon_hdmi *hdmi, em_data_info *em);
hi_s32 hal_ctrl_debug(struct hisilicon_hdmi *hdmi, enum debug_ext_cmd_list cmd, struct hdmitx_debug_msg msg);

#endif

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of controller functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_CTRL_H__
#define __DRV_HDMIRX_CTRL_H__

#include "drv_hdmirx_struct.h"
#include "hi_drv_hdmirx.h"
#include "hal_hdmirx_comm.h"
#include "drv_hdmirx_common.h"
#include "hi_osal.h"

#define HDMIRX_PWD_INTR_MAX       1
#define HDMIRX_WAIT_TIMEOUT       250
#define HDMIRX_NO_SIGNAL_THR      40
#define HDMIRX_HDCP_STABLE_THR    20
#define NO_SIGNAL_THR_IN_VIDEO_ON 4
#define HDMIRX_SCRAMBLE_PIX_CLOCK 34000
#define NO_SIGNAL_STABLE_CNT      4
#define HDMIRX_HDCP_BCHERR_CNT    0x400
#define HDMRIX_EDID_TASK_NUM      5
#define HDMIRX_CHECK_AVI_VALID_THR 50
#define HDMIRX_CHECK_STABLE_THR   25
#define HDMIRX_CHECK_DPHY_ALIGN   5
#define HDMIRX_SPD_VENDOR_NAME_LENGTH 17
#define HDMIRX_HDCP_1P4_KEY_LENGTH 320
#define HDMIRX_HDCP_2P2_KEY_LENGTH 864

typedef enum {
    HDMIRX_STATE_VIDEO_OFF,
    HDMIRX_STATE_VIDEO_ON,
    HDMIRX_STATE_WAIT,
    HDMIRX_STATE_BUTT
} hdmirx_ctrl_state;

typedef enum {
    HDMIRX_VIDPATH_NORMAL,
    HDMIRX_VIDPATH_BYPASS,
    HDMIRX_VIDPATH_BUTT
} hdmirx_ctrl_vidpath;

typedef enum {
    hdmirx_coreiso_normal,
    hdmirx_coreiso_bypass,
    hdmirx_coreiso_butt
} hdmirx_ctrl_coreiso;

typedef enum {
    HDMIRX_MODECHG_TYPE_NONE,
    HDMIRX_MODECHG_TYPE_HDCPERR,
    HDMIRX_MODECHG_TYPE_WAITOUT,
    HDMIRX_MODECHG_TYPE_CKDT,
    HDMIRX_MODECHG_TYPE_NOPOW,
    HDMIRX_MODECHG_TYPE_NOSCDT,
    HDMIRX_MODECHG_TYPE_TIMINGCHG,
    HDMIRX_MODECHG_TYPE_NOAVI,
    HDMIRX_MODECHG_TYPE_AVICHG,
    HDMIRX_MODECHG_TYPE_VSIFCHG,
    HDMIRX_MODECHG_TYPE_BUTT
} hdmirx_modechg_type;

typedef enum {
    HDMIRX_RST_NONE = 0,
    HDMIRX_RST_HDMI,
    HDMIRX_RST_MHL
} hdmirx_rst_type;

typedef struct {
    hi_u32 hdcp_stable_cnt;
    hdmirx_ctrl_state state;
    hdmirx_ctrl_state last_state;
    hi_u32 wait_time_out;
    hi_u32 wait_cnt;
} hdmirx_ctrl_port_ctx;

typedef struct {
    hi_u32 edid_data[HDMIRX_EDID_MAX][256]; /* 256: array colunm size */
    hi_u32 cec_addr;
} hdmirx_ctrl_edid_init;

typedef struct {
    hi_bool is_free;
    hi_bool can_free;
    osal_task *task;
} hdmirx_ctrl_edid_fresh_task;

typedef struct {
    hi_u32 no_sync_cnt; /* the counter of no sync */
    hi_u32 hdcp_err_cnt;
    hi_u32 power;
    hi_u32 unstable_cnt;
    hi_u8 pwr5v; /* 4: array size */
    hi_u8 pwr5v_cnt;
    hi_u8 dphy_inter_align_cnt;
    hi_u8 no_avi_cnt;
    hi_bool need_sub_port;
    hi_bool hdcp_check_en;
    hi_bool hdcp_edid_ready;
    hi_bool pwrstatus;
    hi_bool con_state;
    hi_bool mode_change;
    hi_bool no_avi; /* the flag of mode change */
    hi_bool hpd_low_ctrl;
    hi_bool hpd_high_ctrl;
    osal_timeval low_start_time;
    osal_timeval high_start_time;
    hi_drv_hdmirx_port port;
    hdmirx_ctrl_port_ctx port_status; /* the status of port */
    hdmirx_edid_type cur_edid_type;
    hdmirx_input_type input_type;
    hi_drv_hdmirx_edid_mode edid_mode;
    hdmirx_ctrl_edid_init edid_info;
} hdmirx_ctrl_context;

typedef struct {
    hi_u32 dev_id;                 /* device id of ip */
    hi_u32 devic_rev;              /* revision of  device */
    hi_drv_hdmirx_port cur_port;   /* current port */
    hi_drv_hdmirx_port start_port; /* current used port */
    hi_drv_hdmirx_port end_port;   /* the start port to loop check */
    hi_drv_hdmirx_port loop_port;  /* loop count of port */
    hi_bool run;                   /* the switch of the thread running */
    hdmirx_ctrl_context ctrl_ctx[HI_DRV_HDMIRX_PORT_MAX];
    hdmirx_ctrl_edid_fresh_task edid_fresh_task[HDMRIX_EDID_TASK_NUM];
    osal_task *resume_thread_task;
} hdmirx_ctrl_context_info;

typedef struct {
    hi_u8  edid_data[256]; /* 256: array size */
    hdmirx_edid_type edid_type;
    hi_u32 cec_addr;
} hdmirx_ctrl_edid;

extern hdmirx_ctrl_context_info g_hdmirx_ctrl_ctx_info;
extern hi_drv_sig_status g_over_vide_sig_sta[HI_DRV_HDMIRX_PORT_MAX];
extern hdmirx_ctrl_edid g_edid[HI_DRV_HDMIRX_PORT_MAX];
extern const hi_char *g_state_string[];

#endif

#ifndef __DRV_HDMIRX_CTRL2_H__
#define __DRV_HDMIRX_CTRL2_H__

#define HDMIRXV2_CTRL_GET_CTX_INFO() (&g_hdmirx_ctrl_ctx_info)
#define HDMIRXV2_CTRL_GET_CTX(port) (&(g_hdmirx_ctrl_ctx_info.ctrl_ctx[port]))

hi_s32 hdmirxv2_drv_ctrl_init(hi_void);
hi_s32 hdmirxv2_drv_ctrl_resume(hi_void);
hi_s32 hdmirxv2_drv_ctrl_suspend(hi_void);
hi_void hdmirxv2_drv_ctrl_de_init(hi_void);
hi_s32 hdmirxv2_ctrl_main_task(hi_void);

hi_s32 hdmirxv2_drv_ctrl_connect(hi_drv_hdmirx_port port);
hi_s32 hdmirxv2_drv_ctrl_disconnect(hi_drv_hdmirx_port port);
/* hi_s32 hdmirxv2_drv_ctrl_get_hdr_data(hi_hdmirx_dhdr_data *data); */
hi_s32 hdmirxv2_drv_ctrl_get_sig_status(hi_drv_sig_info *sig_info);
hi_s32 hdmirxv2_drv_ctrl_get_audio_status(hi_drv_sig_info *sig_info);
hi_s32 hdmirxv2_drv_ctrl_get_timing(hi_drv_hdmirx_timing *timing);
hi_bool hdmirxv2_ctrl_is_need2be_done(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_ctrl_is_video_on_state(hi_drv_hdmirx_port port);
hi_u32 hdmirxv2_ctrl_get_shadow_int_mask(hi_u32 idx);
hi_void hdmirxv2_ctrl_set_shadow_int_mask(hi_u32 idx, hi_u32 mask, hi_bool en);
hi_void hdmirxv2_ctrl_mode_change(hi_drv_hdmirx_port port);
hi_void hdmirxv2_ctrl_change_state(hi_drv_hdmirx_port port, hdmirx_ctrl_state e_new_state);
hi_s32 hdmirxv2_drv_ctrl_get_audio_info(hi_drv_hdmirx_aud_info *audio_info);
hi_s32 hdmirxv2_drv_ctrl_get_offline_det_status(hi_drv_hdmirx_offline_status *offline_stat);
hi_s32 hdmirxv2_drv_ctrl_load_hdcp(hi_drv_hdmirx_hdcp_info *hdcp_key);
hi_s32 hdmirxv2_drv_ctrl_update_edid(hi_drv_hdmirx_edid_info *edid);
hi_s32 hdmirxv2_drv_ctrl_edid_fresh(hi_drv_hdmirx_port port);
hi_s32 hdmirxv2_drv_ctrl_set_hpd_value(hi_drv_hdmirx_hpd *hpd);
hdmirx_ctrl_state hdmirxv2_drv_ctrl_get_state(hi_drv_hdmirx_port port);
hi_drv_sig_status hdmirxv2_ctrl_get_port_status(hi_drv_hdmirx_port port);
hi_drv_hdmirx_port hdmirxv2_ctrl_get_sub_port(hi_void);
hi_s32 hdmirxv2_drv_ctrl_cec_enable(hi_bool *enable);
hi_s32 hdmirxv2_drv_ctrl_cec_set_command(hi_drv_hdmirx_cec_cmd *cec_cmd);
hi_s32 hdmirxv2_drv_ctrl_cec_get_command(hi_drv_hdmirx_cec_cmd *cec_cmd);
hi_s32 hdmirxv2_drv_ctrl_cec_get_cur_state(hi_drv_hdmirx_cec_cmd_state_data *cmd_state);
hi_s32 hdmirxv2_drv_ctrl_cec_standby_enable(hi_bool enable);
hi_s32 hdmirxv2_drv_ctrl_cec_get_msg_cnt(hi_u8 *cmd_cnt);
/* hi_s32 hdmirxv2_drv_ctrl_send_hdr_data(hi_void); */
hi_void hdmirxv2_ctrl_set_input_type(hi_drv_hdmirx_port port, hdmirx_input_type input_type);
hi_s32 hdmirxv2_ctrl_cec_main_task(hi_void);
hi_s32 hdmirxv2_drv_ctrl_init_edid(hi_drv_hdmirx_edid_init_info *init_edid);
hi_s32 hdmirxv2_drv_ctrl_set_edid_mode(hi_drv_hdmirx_edid_mode_info *edid_mode);

#endif

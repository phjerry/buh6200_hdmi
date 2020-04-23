/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of proc functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_PROC_H__
#define __DRV_HDMIRX_PROC_H__

#include "drv_hdmirx_common.h"

#define HDMIRX_EDID_PRINT_SIZE 7

hi_s32 hdmirxv2_drv_proc_add(hi_drv_hdmirx_port port);
hi_void hdmirxv2_drv_proc_del(hi_void);

hi_s32 hdmirxv2_drv_proc_cmd_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_thread_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_video_bypass(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_edid_info(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_update_edid(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_select_port(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_ddc_en(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_hpd(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_soft_ware_rst(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_rst_audio_fifo(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_trem_mode(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_audio_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_video_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_over_write_v_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_get_edid(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_hdcp_err_chk(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_mute_state(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_phy_power(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_phy_init(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_eq_code(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_auto_eq_en(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_lane0_eq(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_lane1_eq(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);
hi_s32 hdmirxv2_drv_proc_set_lane2_eq(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private);

#endif


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver vdp header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMI_VIDEO_PROC_REG_H__
#define __HAL_HDMI_VIDEO_PROC_REG_H__

#define VDP_DITHER_SEED_NUM 8

/* VDP module field info */
#define REG_VDP_VDP2HDMI_CTRL  0xa000
#define reg_sync_delay_time(x) (((x) & 0xffff) << 16)
#define REG_SYNC_DELAY_TIME_M  (0xffff << 16)
#define reg_sync_delay_sel(x)  (((x) & 1) << 1)
#define REG_SYNC_DELAY_SEL_M   (1 << 1)
#define reg_ck_gt_en(x)        (((x) & 1) << 0)
#define REG_CK_GT_EN_M         (1 << 0)

#define REG_VDP_VDP2HDMI_RESO 0xa004
#define reg_frm_height(x)     (((x) & 0xffff) << 16)
#define REG_FRM_HEIGHT_M      (0xffff << 16)
#define reg_frm_width(x)      (((x) & 0xffff) << 0)
#define REG_FRM_WIDTH_M       (0xffff << 0)

#define REG_VDP_VDP2HDMI_CMCMODEH 0xa008

#define REG_VDP_VDP2HDMI_CMCMODEL 0xa00C

#define REG_VDP_VDP2HDMI_HFIRCOEF01 0xa010
#define reg_hfir_coef1(x)           (((x) & 0x3ff) << 16)
#define REG_HFIR_COEF1_M            (0x3ff << 16)
#define reg_hfir_coef0(x)           (((x) & 0x3ff) << 0)
#define REG_HFIR_COEF0_M            (0x3ff << 0)

#define REG_VDP_VDP2HDMI_HFIRCOEF23 0xa014
#define reg_hfir_coef3(x)           (((x) & 0x3ff) << 16)
#define REG_HFIR_COEF3_M            (0x3ff << 16)
#define reg_hfir_coef2(x)           (((x) & 0x3ff) << 0)
#define REG_HFIR_COEF2_M            (0x3ff << 0)

#define REG_VDP_VDP2HDMI_HFIRCOEF45 0xa018
#define reg_hfir_coef5(x)           (((x) & 0x3ff) << 16)
#define REG_HFIR_COEF5_M            (0x3ff << 16)
#define reg_hfir_coef4(x)           (((x) & 0x3ff) << 0)
#define REG_HFIR_COEF4_M            (0x3ff << 0)

#define REG_VDP_VDP2HDMI_HFIRCOEF6 0xa01C
#define reg_hfir_coef6(x)          (((x) & 0x3ff) << 0)
#define REG_HFIR_COEF6_M           (0x3ff << 0)

#define REG_VDP_HIPP_CSC_CTRL    0xa100
#define reg_hipp_csc_ck_gt_en(x) (((x) & 1) << 2)
#define REG_HIPP_CSC_CK_GT_EN_M  (1 << 2)
#define reg_hipp_csc_demo_en(x)  (((x) & 1) << 1)
#define REG_HIPP_CSC_DEMO_EN_M   (1 << 1)
#define reg_hipp_csc_en(x)       (((x) & 1) << 0)
#define REG_HIPP_CSC_EN_M        (1 << 0)

#define REG_VDP_HIPP_CSC_COEF00 0xa104
#define REG_VDP_HIPP_CSC_COEF01 0xa108
#define REG_VDP_HIPP_CSC_COEF02 0xa10C
#define REG_VDP_HIPP_CSC_COEF10 0xa110
#define REG_VDP_HIPP_CSC_COEF11 0xa114
#define REG_VDP_HIPP_CSC_COEF12 0xa118
#define REG_VDP_HIPP_CSC_COEF20 0xa11C
#define REG_VDP_HIPP_CSC_COEF21 0xa120
#define REG_VDP_HIPP_CSC_COEF22 0xa124
#define REG_VDP_HIPP_CSC_SCALE  0xa128
#define REG_VDP_HIPP_CSC_IDC0   0xa12C
#define REG_VDP_HIPP_CSC_IDC1   0xa130
#define REG_VDP_HIPP_CSC_IDC2   0xa134
#define REG_VDP_HIPP_CSC_ODC0   0xa138
#define REG_VDP_HIPP_CSC_ODC1   0xa13C
#define REG_VDP_HIPP_CSC_ODC2   0xa140
#define REG_VDP_HIPP_CSC_MIN_Y  0xa144
#define REG_VDP_HIPP_CSC_MIN_C  0xa148
#define REG_VDP_HIPP_CSC_MAX_Y  0xa14C
#define REG_VDP_HIPP_CSC_MAX_C  0xa150

#define reg_hipp_csc_coef(x)  (((x) & 0xffff) << 0)
#define REG_HIPP_CSC_COEF_M   (0xffff << 0)
#define reg_hipp_csc_scale(x) (((x) & 0xf) << 0)
#define REG_HIPP_CSC_SCALE_M  (0xf << 0)
#define reg_hipp_csc_idc(x)   (((x) & 0x1fff) << 0)
#define REG_HIPP_CSC_IDC_M    (0x1fff << 0)
#define reg_hipp_csc_odc(x)   (((x) & 0x1fff) << 0)
#define REG_HIPP_CSC_ODC_M    (0x1fff << 0)
#define reg_hipp_csc_min_y(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC_MIN_Y_M  (0xfff << 0)
#define reg_hipp_csc_min_c(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC_MIN_C_M  (0xfff << 0)
#define reg_hipp_csc_max_y(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC_MAX_Y_M  (0xfff << 0)
#define reg_hipp_csc_max_c(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC_MAX_C_M  (0xfff << 0)

#define REG_VDP_HIPP_CSC2_COEF00 0xa154
#define REG_VDP_HIPP_CSC2_COEF01 0xa158
#define REG_VDP_HIPP_CSC2_COEF02 0xa15C
#define REG_VDP_HIPP_CSC2_COEF10 0xa160
#define REG_VDP_HIPP_CSC2_COEF11 0xa164
#define REG_VDP_HIPP_CSC2_COEF12 0xa168
#define REG_VDP_HIPP_CSC2_COEF20 0xa16C
#define REG_VDP_HIPP_CSC2_COEF21 0xa170
#define REG_VDP_HIPP_CSC2_COEF22 0xa174
#define REG_VDP_HIPP_CSC2_SCALE  0xa178
#define REG_VDP_HIPP_CSC2_IDC0   0xa17C
#define REG_VDP_HIPP_CSC2_IDC1   0xa180
#define REG_VDP_HIPP_CSC2_IDC2   0xa184
#define REG_VDP_HIPP_CSC2_ODC0   0xa188
#define REG_VDP_HIPP_CSC2_ODC1   0xa18C
#define REG_VDP_HIPP_CSC2_ODC2   0xa190
#define REG_VDP_HIPP_CSC2_MIN_Y  0xa194
#define REG_VDP_HIPP_CSC2_MIN_C  0xa198
#define REG_VDP_HIPP_CSC2_MAX_Y  0xa19C
#define REG_VDP_HIPP_CSC2_MAX_C  0xa1A0

#define reg_hipp_csc2_coef(x)  (((x) & 0xffff) << 0)
#define REG_HIPP_CSC2_COEF_M   (0xffff << 0)
#define reg_hipp_csc2_scale(x) (((x) & 0xf) << 0)
#define REG_HIPP_CSC2_SCALE_M  (0xf << 0)
#define reg_hipp_csc2_idc(x)   (((x) & 0x1fff) << 0)
#define REG_HIPP_CSC2_IDC_M    (0x1fff << 0)
#define reg_hipp_csc2_odc(x)   (((x) & 0x1fff) << 0)
#define REG_HIPP_CSC2_ODC_M    (0x1fff << 0)
#define reg_hipp_csc2_min_y(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC2_MIN_Y_M  (0xfff << 0)
#define reg_hipp_csc2_min_c(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC2_MIN_C_M  (0xfff << 0)
#define reg_hipp_csc2_max_y(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC2_MAX_Y_M  (0xfff << 0)
#define reg_hipp_csc2_max_c(x) (((x) & 0xfff) << 0)
#define REG_HIPP_CSC2_MAX_C_M  (0xfff << 0)

#define REG_VDP_HIPP_CSC_INK_CTRL 0xa1F0
#define reg_color_mode(x)         (((x) & 0x3) << 4)
#define REG_COLOR_MODE_M          (0x3 << 4)
#define reg_cross_enable(x)       (((x) & 1) << 3)
#define REG_CROSS_ENABLE_M        (1 << 3)
#define reg_data_fmt(x)           (((x) & 1) << 2)
#define REG_DATA_FMT_M            (1 << 2)
#define reg_ink_sel(x)            (((x) & 1) << 1)
#define REG_INK_SEL_M             (1 << 1)
#define reg_ink_en(x)             (((x) & 1) << 0)
#define REG_INK_EN_M              (1 << 0)

#define REG_VDP_HIPP_CSC_INK_POS 0xa1F4
#define reg_y_pos(x)             (((x) & 0xffff) << 0)
#define REG_Y_POS_M              (0xffff << 0)
#define reg_x_pos(x)             (((x) & 0xffff) << 0)
#define REG_X_POS_M              (0xffff << 0)

#define REG_VDP_DITHER_CTRL      0xa200
#define reg_dither_ck_gt_en(x)   (((x) & 1) << 19)
#define REG_DITHER_CK_GT_EN_M    (1 << 19)
#define reg_bitdepthout(x)       (((x) & 0xf) << 15)
#define REG_BITDEPTHOUT_M        (0xf << 15)
#define reg_bitdepthtemp(x)      (((x) & 0xf) << 11)
#define REG_BITDEPTHTEMP_M       (0xf << 11)
#define reg_bitdepthin(x)        (((x) & 0xf) << 7)
#define REG_BITDEPTHIN_M         (0xf << 7)
#define reg_dither_reset(x)      (((x) & 0x7) << 4)
#define REG_DITHER_RESET_M       (0x7 << 4)
#define reg_dither_mode(x)       (((x) & 1) << 3)
#define REG_DITHER_MODE_M        (1 << 3)
#define reg_dither_roundlimit(x) (((x) & 1) << 2)
#define REG_DITHER_ROUNDLIMIT_M  (1 << 2)
#define reg_dither_round(x)      (((x) & 1) << 1)
#define REG_DITHER_ROUND_M       (1 << 1)
#define reg_dither_enable(x)     (((x) & 1) << 0)
#define REG_DITHER_ENABLE_M      (1 << 0)

#define REG_VDP_DITHER_DEMO_CTRL 0xa204
#define reg_dither_demo_pos(x)   (((x) & 0xffff) << 16)
#define REG_DITHER_DEMO_POS_M    (0xffff << 16)
#define reg_dither_demo_mode(x)  (((x) & 1) << 1)
#define REG_DITHER_DEMO_MODE_M   (1 << 1)
#define reg_dither_demo_en(x)    (((x) & 1) << 0)
#define REG_DITHER_DEMO_EN_M     (1 << 0)

#define REG_VDP_DITHER_THR   0xa208
#define reg_dither_thrmax(x) (((x) & 0xffff) << 16)
#define REG_DITHER_THRMAX_M  (0xffff << 16)
#define reg_dither_thrmin(x) (((x) & 0xffff) << 0)
#define REG_DITHER_THRMIN_M  (0xffff << 0)

#define REG_VDP_DITHER_TAPEMODE 0xa20C
#define reg_dither_tapmode7(x)  (((x) & 0x3) << 28)
#define REG_DITHER_TAPMODE7_M   (0x3 << 28)
#define reg_dither_tapmode6(x)  (((x) & 0x3) << 24)
#define REG_DITHER_TAPMODE6_M   (0x3 << 24)
#define reg_dither_tapmode5(x)  (((x) & 0x3) << 20)
#define REG_DITHER_TAPMODE5_M   (0x3 << 20)
#define reg_dither_tapmode4(x)  (((x) & 0x3) << 16)
#define REG_DITHER_TAPMODE4_M   (0x3 << 16)
#define reg_dither_tapmode3(x)  (((x) & 0x3) << 12)
#define REG_DITHER_TAPMODE3_M   (0x3 << 12)
#define reg_dither_tapmode2(x)  (((x) & 0x3) << 8)
#define REG_DITHER_TAPMODE2_M   (0x3 << 8)
#define reg_dither_tapmode1(x)  (((x) & 0x3) << 4)
#define REG_DITHER_TAPMODE1_M   (0x3 << 4)
#define reg_dither_tapmode0(x)  (((x) & 0x3) << 0)
#define REG_DITHER_TAPMODE0_M   (0x3 << 0)

#define reg_vdp_dither_seed_y(n) (0xa210 + n * 0x10)
#define reg_vdp_dither_seed_u(n) (0xa214 + n * 0x10)
#define reg_vdp_dither_seed_v(n) (0xa218 + n * 0x10)
#define reg_vdp_dither_seed_w(n) (0xa21c + n * 0x10)

#define REG_VDP_DITHER_INK_CTRL    0xa2F0
#define reg_dither_color_mode(x)   (((x) & 0x3) << 4)
#define REG_DITHER_COLOR_MODE_M    (0x3 << 4)
#define reg_dither_cross_enable(x) (((x) & 1) << 3)
#define REG_DITHER_CROSS_ENABLE_M  (1 << 3)
#define reg_dither_data_fmt(x)     (((x) & 1) << 2)
#define REG_DITHER_DATA_FMT_M      (1 << 2)
#define reg_dither_ink_sel(x)      (((x) & 1) << 1)
#define REG_DITHER_INK_SEL_M       (1 << 1)
#define reg_dither_ink_en(x)       (((x) & 1) << 0)
#define REG_DITHER_INK_EN_M        (1 << 0)

#define REG_VDP_DITHER_INK_POS 0xa2F4
#define reg_dither_y_pos(x)    (((x) & 0xffff) << 16)
#define REG_DITHER_Y_POS_M     (0xffff << 16)
#define reg_dither_x_pos(x)    (((x) & 0xffff) << 0)
#define REG_DITHER_X_POS_M     (0xffff << 0)

#define REG_VDP_VDP2HDMI_DEUBG 0xa300

#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_reg.h hander file
 * Author: zhangjunyu
 * Create: 2016/07/02
 */

#ifndef __HAL_VPSS_REG_H__
#define __HAL_VPSS_REG_H__

#include "hi_type.h"
#include "vpss_define.h"
#include "hi_reg_vpss.h"
#include "hi_reg_vpss_stt.h"
#include "hal_vpss_reg_struct.h"
#include "hi_drv_vpss.h"
#include "hal_vpss_ice_define.h"
#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VPSS_MAC_RCHN_CF = 0,
    VPSS_MAC_RCHN_DI_CF,
    VPSS_MAC_RCHN_DI_P1,
    VPSS_MAC_RCHN_DI_P2,
    VPSS_MAC_RCHN_DI_P3,
    VPSS_MAC_RCHN_DI_P3I,
    VPSS_MAC_RCHN_CC_PR0,
    VPSS_MAC_RCHN_CC_PR4,
    VPSS_MAC_RCHN_CC_PR8,
    VPSS_MAC_RCHN_NR_REF,
    VPSS_MAC_RCHN_DI_RSADY,  // 10
    VPSS_MAC_RCHN_DI_RSADC,
    VPSS_MAC_RCHN_DI_RHISM,
    VPSS_MAC_RCHN_DI_P1MV,
    VPSS_MAC_RCHN_DI_P2MV,
    VPSS_MAC_RCHN_DI_P3MV,
    VPSS_MAC_RCHN_CC_RCCNT,
    VPSS_MAC_RCHN_CC_RYCNT,
    VPSS_MAC_RCHN_NR_RMAD,
    VPSS_MAC_RCHN_SNR_RMAD,
    VPSS_MAC_RCHN_NR_CFMV,  // 20
    VPSS_MAC_RCHN_NR_P1RGMV,
    VPSS_MAC_RCHN_NR_P2RGMV,
    VPSS_MAC_RCHN_NR_RCNT,
    VPSS_MAC_RCHN_RG_RPRJH,
    VPSS_MAC_RCHN_RG_RPRJV,
    VPSS_MAC_RCHN_RG_P1RGMV,
    VPSS_MAC_RCHN_RG_P2RGMV,
    VPSS_MAC_RCHN_DMCNT, /* dm430 */
    VPSS_MAC_RCHN_ME_CF,
    VPSS_MAC_RCHN_ME_REF,  // 30
    VPSS_MAC_RCHN_ME_P1MV,
    VPSS_MAC_RCHN_ME_PRMV,
    VPSS_MAC_RCHN_ME_P1GMV,
    VPSS_MAC_RCHN_ME_PRGMV,
    VPSS_MAC_RCHN_ME_P1RGMV,  // 35
    VPSS_MAC_RCHN_ME_CFRGMV,

    VPSS_MAC_RCHN_MAX
} vpss_mac_rchn;

typedef enum {
    VPSS_MAC_WCHN_OUT0 = 0,
#ifdef VPSS_96CV300_CS_SUPPORT
    VPSS_MAC_WCHN_OUT1, /* new add todo */
    VPSS_MAC_WCHN_OUT2, /* new add todo */
#endif
    VPSS_MAC_WCHN_OUT3,
    VPSS_MAC_WCHN_NR_RFR,
    VPSS_MAC_WCHN_NR_RFRH,
    VPSS_MAC_WCHN_NR_RFRHV,
    VPSS_MAC_WCHN_CC_RFR,
    VPSS_MAC_WCHN_CC_RFR1,
    VPSS_MAC_WCHN_DI_RFR,
    VPSS_MAC_WCHN_NR_WMAD,
    VPSS_MAC_WCHN_NR_WCNT,
    VPSS_MAC_WCHN_CC_WCCNT,
    VPSS_MAC_WCHN_CC_WYCNT,
    VPSS_MAC_WCHN_DI_WSADY,
    VPSS_MAC_WCHN_DI_WSADC,
    VPSS_MAC_WCHN_DI_WHISM,
    VPSS_MAC_WCHN_RG_WPRJH,
    VPSS_MAC_WCHN_RG_WPRJV,
    VPSS_MAC_WCHN_RG_CFRGMV,
    VPSS_MAC_WCHN_DMCNT, /* dm430 */
    VPSS_MAC_WCHN_ME_CFMV,
    VPSS_MAC_WCHN_ME_CFGMV,

    VPSS_MAC_WCHN_MAX
} vpss_mac_wchn;

typedef enum {
    XDP_DATA_TYPE_SP_LINEAR = 0x0,
    XDP_DATA_TYPE_SP_TILE = 0x1,
    XDP_DATA_TYPE_PACKAGE = 0x2,
    XDP_DATA_TYPE_PLANAR = 0x3,
    XDP_DATA_TYPE_MAX
} xdp_data_type;

typedef enum {
    XDP_PROC_FMT_SP_420 = 0x0,
    XDP_PROC_FMT_SP_422 = 0x1,
    XDP_PROC_FMT_SP_444 = 0x2,   // plannar,in YUV color domain
    XDP_PROC_FMT_SP_400 = 0x3,   // plannar,in YUV color domain
    XDP_PROC_FMT_RGB_888 = 0x3,  // package,in RGB color domain
    XDP_PROC_FMT_RGB_444 = 0x4,  // plannar,in RGB color domain

    XDP_PROC_FMT_MAX
} xdp_proc_fmt;

typedef enum {
    XDP_PKG_FMT_YUYV = 0x0,
    XDP_PKG_FMT_YVYU = 0x1,
    XDP_PKG_FMT_UYVY = 0x2,
    XDP_PKG_FMT_VYUY = 0x3,
    XDP_PKG_FMT_YYUV = 0x4,
    XDP_PKG_FMT_YYVU = 0x5,
    XDP_PKG_FMT_UVYY = 0x6,
    XDP_PKG_FMT_VUYY = 0x7,
    XDP_PKG_FMT_MAX
} xdp_pkg_fmt;

typedef enum {
    XDP_RMODE_INTERFACE = 0,
    XDP_RMODE_INTERLACE = 0,
    XDP_RMODE_PROGRESSIVE = 1,
    XDP_RMODE_TOP = 2,
    XDP_RMODE_BOTTOM = 3,
    XDP_RMODE_PRO_TOP = 4,
    XDP_RMODE_PRO_BOTTOM = 5,
    XDP_RMODE_MAX
} xdp_data_rmode;

typedef enum {
    XDP_DATA_WTH_8 = 0,
    XDP_DATA_WTH_10 = 1,
    XDP_DATA_WTH_12 = 2,
    XDP_DATA_WTH_MAX
} xdp_data_wth;

typedef struct {
    hi_u32 x;
    hi_u32 y;

    hi_u32 wth;
    hi_u32 hgt;

    hi_u32 src_w;
    hi_u32 src_h;
} xdp_rect;

typedef struct {
    hi_bool mute_en;
    hi_u32 mute_y;
    hi_u32 mute_c;
} vpss_mute_cfg;

typedef enum {
    VPSS_RCHN_ADDR_DATA = 0,  // 2d left eye data
    VPSS_RCHN_ADDR_HEAD_TOP,  // dcmp head data top
    VPSS_RCHN_ADDR_HEAD_BOT,  // dcmp head data bottom
    VPSS_RCHN_ADDR_2B,        // 2bit
    VPSS_RCHN_ADDR_MAX
} vpss_rchn_addr;

typedef struct {
    hi_u64 addr_y;
    hi_u64 addr_u;
    hi_u64 addr_v;
    hi_u32 str_y;
    hi_u32 str_c;

    hi_u64 y2_b_addr;
    hi_u64 u2_b_addr;
    hi_u64 v2_b_addr;
    hi_u32 y2_b_str;
    hi_u32 c2_b_str;

    hi_u64 y_head_addr;
    hi_u64 c_head_addr;
    hi_u32 y_head_str;
    hi_u32 c_head_str;
} xdp_trio_addr;

typedef enum {
    XDP_CMP_TYPE_OFF = 0,
    XDP_CMP_TYPE_SEG,
    XDP_CMP_TYPE_LINE,
    XDP_CMP_TYPE_FRM,
    XDP_CMP_TYPE_MAX
} xdp_cmp_type;

typedef enum {
    VPSS_DITHER_MODE_RAND = 0,
    VPSS_DITHER_MODE_ROUND,
    VPSS_DITHER_MODE_MAX
} vpss_dither_mode;

typedef struct {
    hi_bool en;
    vpss_dither_mode mode;
} vpss_dither_cfg;

typedef struct {
    hi_u32 frm_cnt;
    hi_u32 node_cnt;
    xdp_cmp_type cmp_type;
    hi_bool is_raw_en;
    hi_bool is_lossy_y;
    hi_bool is_lossy_c;
    hi_u32 cmp_ratio_y;
    hi_u32 cmp_ratio_c;
    ice_reg_cfg_mode cmp_cfg_mode;
} vpss_dcmp_cfg;

typedef struct {
    hi_bool en;
    xdp_data_type data_type;
    xdp_proc_fmt data_fmt;
    xdp_pkg_fmt pkg_fmt;
    xdp_data_rmode rd_mode;
    xdp_data_wth data_width;
    xdp_rect in_rect;
    vpss_mute_cfg mute_cfg;
    hi_bool flip_en;
    hi_bool mirror_en;
    hi_bool uv_inv_en;
    hi_bool mmu_bypass;
    hi_bool tunl_en;
    vpss_dcmp_cfg dcmp_cfg;
    xdp_trio_addr addr[VPSS_RCHN_ADDR_MAX];
    hi_bool set_flag;
} vpss_mac_rchn_cfg;

typedef enum {
    VPSS_WCHN_ADDR_DATA = 0,  // data
    VPSS_WCHN_ADDR_HEAD,      // cmp head data
    VPSS_WCHN_ADDR_MAX
} vpss_wchn_addr;

typedef struct {
    hi_u32 frm_cnt;
    hi_u32 node_cnt;
    xdp_cmp_type cmp_type;
    hi_bool is_raw_en;
    hi_bool is_lossy_y;
    hi_bool is_lossy_c;
    hi_u32 cmp_ratio_y;
    hi_u32 cmp_ratio_c;
    ice_reg_cfg_mode cmp_cfg_mode;
    ice_frm_part_mode part_mode_cfg;
} vpss_cmp_cfg;

typedef struct {
    // cfg
    hi_bool en;
    xdp_data_type data_type;
    xdp_data_wth data_width;
    hi_bool flip_en;
    hi_bool mirror_en;
    hi_bool uv_inv_en;
    vpss_dither_cfg dither_cfg;
    hi_bool mmu_bypass;
    hi_bool tunl_en;
    // cmp
    vpss_cmp_cfg cmp_cfg;
    // addr
    xdp_trio_addr addr[VPSS_WCHN_ADDR_MAX];
    // env
    hi_bool set_flag;
    xdp_proc_fmt data_fmt;
    xdp_data_rmode rd_mode;
    xdp_rect out_rect;  // offset_x/y have real regs
    hi_u64 finfo;
    hi_u32 size;
    hi_u8 *vir_addr_y;
    hi_u8 *vir_addr_c;
} vpss_mac_wchn_cfg;

typedef enum {
    HAL_IFIR_MODE_DISEN = 0,
    HAL_IFIR_MODE_COPY,
    HAL_IFIR_MODE_DOUBLE,
    HAL_IFIR_MODE_FILT,

    HAL_IFIR_MODE_MAX
} hal_ifir_mode;

hi_void vpss_reg_write(volatile hi_u32 *a, hi_u32 b);
hi_u32 vpss_reg_read(volatile hi_u32 *a);

hi_void vpss_stt_get_lbd_space(vpss_stt_reg_type *vpss_stt_regs, hi_u32 *top, hi_u32 *bottom, hi_u32 *left,
                               hi_u32 *right);
hi_void vpss_stt_get_lbd_shift(vpss_stt_reg_type *vpss_stt_regs, hi_u32 *top, hi_u32 *bottom, hi_u32 *left,
                               hi_u32 *right);

hi_void vpss_sys_set_int_mask(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask);
hi_u32 vpss_sys_get_int_state(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_mask_int_state(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_void vpss_sys_set_int_clr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 data1);

hi_u32 vpss_sys_get_node_cnt(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_scan0_cnt(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_scan1_cnt(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_scan2_cnt(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_scan3_cnt(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_scan_cnt(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_scan_num(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_node_num(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_mac_debug0(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_mac_debug1(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_mac_debug2(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_u32 vpss_sys_get_mac_debug3(vpss_reg_type *vpss_regs, hi_u32 addr_offset);

hi_void vpss_sys_set_ck_gt_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 ck_gt_en);

hi_void vpss_sys_set_node_rst_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 node_rst_en);
hi_void vpss_sys_set_scan_rst_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_rst_en);

hi_void vpss_sys_set_vpss_start(vpss_reg_type *vpss_regs, hi_u32 task_id, hi_u32 start);

// -------------------------------------------------
// vpss hal system AXI
// -------------------------------------------------
hi_void vpss_sys_set_pnext(vpss_reg_type *vpss_regs, hi_u32 task_id, hi_u32 pnext_h, hi_u32 pnext_l);

hi_void vpss_sys_set_snr_mad_disable(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 snr_mad_disable);
hi_void vpss_sys_set_hfr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hfr_en);
hi_void vpss_sys_set_lbd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lbd_en);
hi_void vpss_sys_set_tnr_rec_8bit_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 tnr_rec_8bit_en);
hi_void vpss_sys_set_tnr_mad_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 tnr_mad_mode);
hi_void vpss_sys_set_hcti_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hcti_en);
hi_void vpss_sys_set_scd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scd_en);
hi_void vpss_sys_set_vc1_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_en);
hi_void vpss_sys_setHspEn(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hsp_en);
hi_void vpss_sys_set_rotate_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rotate_en);
hi_void vpss_sys_set_rotate_angle(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rotate_angle);
hi_void vpss_sys_set_snr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 snr_en);
hi_void vpss_sys_set_cccl_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 cccl_en);
hi_void vpss_sys_get_rgmv_addr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u64 *rgmv_addr);

hi_bool vpss_sys_get_cccl_en(vpss_reg_type *vpss_regs);
hi_bool vpss_sys_get_vc1_en(vpss_reg_type *vpss_regs);

hi_void vpss_sys_set_meds_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 meds_en);
hi_void vpss_sys_set_tnr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 tnr_en);
hi_void vpss_sys_set_mcnr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mcnr_en);
hi_void vpss_sys_set_dei_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 dei_en);
hi_void vpss_sys_set_mcdi_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mcdi_en);
hi_void vpss_sys_set_ifmd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 ifmd_en);
hi_void vpss_sys_set_igbm_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 igbm_en);
hi_void vpss_sys_set_rgme_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rgme_en);
hi_void vpss_sys_set_me_version(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_version);
hi_void vpss_sys_set_ma_mac_sel(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 ma_mac_sel);
hi_void vpss_sys_set_vpss3drs_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vpss_3drs_en);

hi_void vpss_sys_set_slow(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 slow);
hi_void vpss_sys_set_mask0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask0);
hi_void vpss_sys_set_mask1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask1);
hi_void vpss_sys_set_mask2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask2);
hi_void vpss_sys_set_mask3(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask3);
hi_void vpss_sys_set_mask4(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask4);
hi_void vpss_sys_set_mask5(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask5);
hi_void vpss_sys_set_mask6(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask6);
hi_void vpss_sys_set_mask7(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 mask7);

hi_void vpss_sys_set_stt_raddr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 stt_addr_h, hi_u32 stt_addr_l);
hi_void vpss_sys_set_stt_waddr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 stt_addr_h, hi_u32 stt_addr_l);

hi_void vpss_sys_setVpssOkNum(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vpss_ok_num);
hi_void vpss_sys_setVpssHoldEn(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vpss_hold_en);
hi_void vpss_sys_setScanOkNum(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_ok_num);
hi_void vpss_sys_setScanHoldEn(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_hold_en);

hi_void vpss_sys_set_rupd_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 rupd_en);

/* Scan ctrl */
hi_void vpss_scan_set_scanTimeout(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_timeout);
hi_void vpss_scan_set_scanRstEn(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_rst_en);
hi_void vpss_scan_set_scanInitTimer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_init_timer);
hi_void vpss_scan_set_scan_start_addr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_start_addr_h,
                                      hi_u32 scan_start_addr_l);
hi_void vpss_scan_set_scan_pnext_addr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 scan_pnext_addr_h,
                                      hi_u32 scan_pnext_addr_l);
hi_void vpss_scan_set_me_cf_online_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_cf_online_en);
hi_void vpss_scan_set_me_vc1_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_vc1_en);
hi_void vpss_scan_set_me_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_en);
hi_void vpss_scan_set_me_mv_upsmp_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_mv_upsmp_en);
hi_void vpss_scan_set_me_mv_dnsmp_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_mv_dnsmp_en);
hi_void vpss_scan_set_me_layer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_layer);
hi_void vpss_scan_set_me_scan(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 me_scan);
hi_void vpss_scan_set_me_vid_default_cfg(vpss_reg_type *vpss_regs, hi_u32 addr_offset);
hi_void vpss_rchn_set_cf_vid_default_cfg(vpss_reg_type *reg);

/* MAC ctrl */
hi_void vpss_mac_set_cf_cconvert(vpss_reg_type *vdp_reg, hi_u32 cf_c_convert);
hi_void vpss_mac_get_cf_cconvert(vpss_reg_type *vdp_reg, hi_u32 *cf_c_convert);
hi_void vpss_mac_set_img_pro_mode(vpss_reg_type *vdp_reg, hi_u32 img_pro_mode);
hi_void vpss_mac_set_prot(vpss_reg_type *vdp_reg, hi_u32 prot);
hi_void vpss_mac_get_prot(vpss_reg_type *vdp_reg, hi_u32 *prot);
hi_void vpss_mac_set_cf_rtunl_en(vpss_reg_type *vdp_reg, hi_u32 cf_rtunl_en);
hi_void vpss_mac_set_cf_rtunl_addr_h(vpss_reg_type *vdp_reg, hi_u32 cf_rtunl_addr_h);
hi_void vpss_mac_set_cf_rtunl_addr_l(vpss_reg_type *vdp_reg, hi_u32 cf_rtunl_addr_l);
hi_void vpss_mac_set_me_stt_waddr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 me_stt_w_addr_h);
hi_void vpss_mac_set_me_stt_waddr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 me_stt_w_addr_l);
/* WCHN */
hi_void vpss_wchn_set_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 en);
hi_void vpss_wchn_set_tunl_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 tunl_en);
hi_void vpss_wchn_set_dbypass(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bypass);
hi_void vpss_wchn_set_hbypass(vpss_reg_type *vpss_reg, hi_u32 offset, hi_u32 hypass);

hi_void vpss_wchn_set_dither_mode(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 dither_mode);
hi_void vpss_wchn_set_dither_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 dither_en);
hi_void vpss_wchn_set_flip(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 flip);
hi_void vpss_wchn_set_mirror(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 mirror);
hi_void vpss_wchn_set_uv_invert(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 uv_invert);
hi_void vpss_wchn_set_cmp_mode(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 cmp_mode);
hi_void vpss_wchn_set_bitw(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bitw);
hi_void vpss_wchn_set_type(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 type);
hi_void vpss_wchn_set_ver_offset(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 ver_offset);
hi_void vpss_wchn_set_hor_offset(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 hor_offset);
hi_void vpss_wchn_set_yaddr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 y_addr_l);
hi_void vpss_wchn_set_yaddr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 y_addr_h);
hi_void vpss_wchn_set_caddr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_l);
hi_void vpss_wchn_set_caddr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_h);
hi_void vpss_wchn_set_cstride(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 stride);
hi_void vpss_wchn_set_ystride(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 stride_y);
hi_void vpss_wchn_env_set_height(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 height);
hi_void vpss_wchn_env_set_width(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 width);
hi_void vpss_wchn_env_set_finfo_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 finfo_h);
hi_void vpss_wchn_env_set_finfo_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 finfo_l);

// -------------------------------------------------
// vpss mac wchn hal solo
// -------------------------------------------------
hi_void vpss_wchn_solo_set_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 en);
hi_void vpss_wchn_solo_set_tunl_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 tunl_en);
hi_void vpss_wchn_solo_set_dbypass(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bypass);
hi_void vpss_wchn_solo_set_flip(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 flip);
hi_void vpss_wchn_solo_set_mirror(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 mirror);
hi_void vpss_wchn_solo_set_addr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_h);
hi_void vpss_wchn_solo_set_addr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_l);
hi_void vpss_wchn_solo_set_stride(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 stride);
hi_void vpss_wchn_solo_env_set_height(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 height);
hi_void vpss_wchn_solo_env_set_width(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 width);
/* RCHN */
hi_void vpss_rchn_set_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 en);
hi_void vpss_rchn_set_mute_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 test_en);
hi_void vpss_rchn_set_dbypass(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bypass);
hi_void vpss_rchn_set_hbypass(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 h_bypass);
hi_void vpss_rchn_set2b_bypass(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bypass);
hi_void vpss_rchn_set_lm_rmode(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 lm_rmode);
hi_void vpss_rchn_set_flip(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 flip);
hi_void vpss_rchn_set_uv_invert(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 uv_invert);
hi_void vpss_rchn_set_dcmp_mode(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 dcmp_mode);
hi_void vpss_rchn_set_bitw(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bitw);
hi_void vpss_rchn_set_order(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 order);
hi_void vpss_rchn_set_type(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 type);
hi_void vpss_rchn_set_format(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 format);
hi_void vpss_rchn_set_height(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 height);
hi_void vpss_rchn_set_width(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 width);
hi_void vpss_rchn_set_ver_offset(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 ver_offset);
hi_void vpss_rchn_set_hor_offset(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 hor_offset);
hi_void vpss_rchn_set_yaddr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 y_addr_l);
hi_void vpss_rchn_set_yaddr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 y_addr_h);
hi_void vpss_rchn_set_caddr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_l);
hi_void vpss_rchn_set_caddr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_h);
hi_void vpss_rchn_set_cr_addr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 cr_addr_h);
hi_void vpss_rchn_set_cr_addr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 cr_addr_l);
hi_void vpss_rchn_set_cstride(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 stride);
hi_void vpss_rchn_set_ystride(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 stride_y);

hi_void vpss_set_hvhdaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdaddr_h);
hi_void vpss_set_hvhdaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdaddr_l);
hi_void vpss_set_hvhdcaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdcaddr_h);
hi_void vpss_set_hvhdcaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 h_vhdcaddr_l);
hi_void vpss_set_bvhdaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdaddr_h);
hi_void vpss_set_bvhdaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdaddr_l);
hi_void vpss_set_bvhdcaddr_h(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdcaddr_h);
hi_void vpss_set_bvhdcaddr_l(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhdcaddr_l);
hi_void vpss_set_chm_tile_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 chm_tile_stride);
hi_void vpss_set_lm_tile_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lm_tile_stride);
hi_void vpss_set_chm_head_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 chm_head_stride);
hi_void vpss_set_lm_head_stride(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lm_head_stride);

hi_void vpss_rchn_set_cmute_val(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 mute_val);
hi_void vpss_rchn_set_ymute_val(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 y_mute_val);

// -------------------------------------------------
// vpss mac rchn hal solo
// -------------------------------------------------
hi_void vpss_rchn_solo_set_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 en);
hi_void vpss_rchn_solo_set_tunle_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 tunl_en);
hi_void vpss_rchn_solo_set_mute_en(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 test_en);
hi_void vpss_rchn_solo_set_dbypass(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 bypass);
hi_void vpss_rchn_solo_set_flip(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 flip);
hi_void vpss_rchn_solo_set_mirror(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 mirror);
hi_void vpss_rchn_solo_set_height(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 height);
hi_void vpss_rchn_solo_set_width(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 width);
hi_void vpss_rchn_solo_set_addr_h(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_h);
hi_void vpss_rchn_solo_set_addr_l(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 addr_l);
hi_void vpss_rchn_solo_set_stride(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 stride);
hi_void vpss_rchn_solo_set_mute_val0(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 mute_val0);
hi_void vpss_rchn_solo_set_mute_val1(vpss_reg_type *vdp_reg, hi_u32 offset, hi_u32 mute_val1);

/* VZME */
hi_void vpss_vzme_set_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 ck_gt_en);
hi_void vpss_vzme_set_out_fmt(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 out_fmt);
hi_void vpss_vzme_set_out_height(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 out_height);
hi_void vpss_vzme_set_lvfir_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 lvfir_en);
hi_void vpss_vzme_set_cvfir_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 cvfir_en);
hi_void vpss_vzme_set_lvmid_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 lvmid_en);
hi_void vpss_vzme_set_cvmid_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 cvmid_en);
hi_void vpss_vzme_set_lvfir_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 lvfir_mode);
hi_void vpss_vzme_set_cvfir_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 cvfir_mode);
hi_void vpss_vzme_set_vratio(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vratio);
hi_void vpss_vzme_set_vluma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vluma_offset);
hi_void vpss_vzme_set_vchroma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vchroma_offset);
hi_void vpss_vzme_set_vbluma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vbluma_offset);
hi_void vpss_vzme_set_vbchroma_offset(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vbchroma_offset);
hi_void vpss_vzme_set_vl_shootctrl_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_shootctrl_en);
hi_void vpss_vzme_set_vl_shootctrl_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_shootctrl_mode);
hi_void vpss_vzme_set_vl_flatdect_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_flatdect_mode);
hi_void vpss_vzme_set_vl_coringadj_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_coringadj_en);
hi_void vpss_vzme_set_vl_gain(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_gain);
hi_void vpss_vzme_set_vl_coring(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vl_coring);
hi_void vpss_vzme_set_vc_shootctrl_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_shootctrl_en);
hi_void vpss_vzme_set_vc_shootctrl_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_shootctrl_mode);
hi_void vpss_vzme_set_vc_flatdect_mode(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_flatdect_mode);
hi_void vpss_vzme_set_vc_coringadj_en(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_coringadj_en);
hi_void vpss_vzme_set_vc_gain(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_gain);
hi_void vpss_vzme_set_vc_coring(vpss_reg_type *vpss_reg, hi_u32 offset_addr, hi_u32 vc_coring);
/* HZME */
hi_void vpss_hzme_set_out_fmt(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 out_fmt);
hi_void vpss_hzme_set_ck_gt_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 ck_gt_en);
hi_void vpss_hzme_set_out_width(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 out_width);
hi_void vpss_hzme_set_lhfir_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhfir_en);
hi_void vpss_hzme_set_chfir_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chfir_en);
hi_void vpss_hzme_set_lhmid_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhmid_en);
hi_void vpss_hzme_set_chmid_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chmid_en);
hi_void vpss_hzme_set_non_lnr_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 non_lnr_en);
hi_void vpss_hzme_set_lhfir_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhfir_mode);
hi_void vpss_hzme_set_chfir_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chfir_mode);
hi_void vpss_hzme_set_hfir_order(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hfir_order);
hi_void vpss_hzme_set_hratio(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hratio);
hi_void vpss_hzme_set_lhfir_offset(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 lhfir_offset);
hi_void vpss_hzme_set_chfir_offset(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 chfir_offset);
hi_void vpss_hzme_set_zone0_delta(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone0_delta);
hi_void vpss_hzme_set_zone2_delta(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone2_delta);
hi_void vpss_hzme_set_zone1_end(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone1_end);
hi_void vpss_hzme_set_zone0_end(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 zone0_end);
hi_void vpss_hzme_set_hl_shootctrl_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_shootctrl_en);
hi_void vpss_hzme_set_hl_shootctrl_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_shootctrl_mode);
hi_void vpss_hzme_set_hl_flatdect_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_flatdect_mode);
hi_void vpss_hzme_set_hl_coringadj_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_coringadj_en);
hi_void vpss_hzme_set_hl_gain(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_gain);
hi_void vpss_hzme_set_hl_coring(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hl_coring);
hi_void vpss_hzme_set_hc_shootctrl_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_shootctrl_en);
hi_void vpss_hzme_set_hc_shootctrl_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_shootctrl_mode);
hi_void vpss_hzme_set_hc_flatdect_mode(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_flatdect_mode);
hi_void vpss_hzme_set_hc_coringadj_en(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_coringadj_en);
hi_void vpss_hzme_set_hc_gain(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_gain);
hi_void vpss_hzme_set_hc_coring(vpss_reg_type *vpss_regs, hi_u32 offset_addr, hi_u32 hc_coring);

/* Pzme */
hi_void vpss_pzme_set_pzme_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_en);
hi_void vpss_pzme_set_pzme_out_img_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset,
                                          hi_u32 pzme_out_img_height);
hi_void vpss_pzme_set_pzme_out_img_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_out_img_width);
hi_void vpss_pzme_set_pzme_hstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_h_step);
hi_void vpss_pzme_set_pzme_wstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_w_step);
/* Out0Pzme */
hi_void vpss_pzme_set_out0_pzme_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_en);
hi_void vpss_pzme_set_out0_pzme_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_mode);
hi_void vpss_pzme_set_out0_pzme_out_img_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset,
                                               hi_u32 pzme_out_img_height);
hi_void vpss_pzme_set_out0_pzme_out_img_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset,
                                              hi_u32 pzme_out_img_width);
hi_void vpss_pzme_set_out0_pzme_hstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_h_step);
hi_void vpss_pzme_set_out0_pzme_wstep(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 pzme_w_step);
/* MASTER */
hi_void vpss_master_set_mstr0_woutstanding(vpss_reg_type *reg, hi_u32 mstr0_woutstanding);
hi_void vpss_master_set_mstr0_routstanding(vpss_reg_type *reg, hi_u32 mstr0_routstanding);
hi_void vpss_mac_set_nr_refy_bd_det_en(vpss_reg_type *reg, hi_u32 nr_refy_bd_det_en);
hi_void vpss_mac_set_nr_refy_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_refy_frame_bitdepth);
hi_void vpss_mac_set_nr_refy_pixel_format(vpss_reg_type *reg, hi_u32 nr_refy_pixel_format);
hi_void vpss_mac_set_nr_refy_cmp_mode(vpss_reg_type *reg, hi_u32 nr_refy_cmp_mode);
hi_void vpss_mac_set_nr_refy_is_lossless(vpss_reg_type *reg, hi_u32 nr_refy_is_lossless);
hi_void vpss_mac_set_nr_refy_dcmp_en(vpss_reg_type *reg, hi_u32 nr_refy_dcmp_en);
hi_void vpss_mac_set_nr_refy_frame_height(vpss_reg_type *reg, hi_u32 nr_refy_frame_height);
hi_void vpss_mac_set_nr_refy_frame_width(vpss_reg_type *reg, hi_u32 nr_refy_frame_width);
hi_void vpss_mac_set_nr_refy_buffer_fullness_thr(vpss_reg_type *reg, hi_u32 nr_refy_buffer_fullness_thr);
hi_void vpss_mac_set_nr_refy_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_refy_buffer_init_bits);
hi_void vpss_mac_set_nr_refy_budget_mb_bits(vpss_reg_type *reg, hi_u32 nr_refy_budget_mb_bits);
hi_void vpss_mac_set_nr_refc_bd_det_en(vpss_reg_type *reg, hi_u32 nr_refc_bd_det_en);
hi_void vpss_mac_set_nr_refc_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_refc_frame_bitdepth);
hi_void vpss_mac_set_nr_refc_pixel_format(vpss_reg_type *reg, hi_u32 nr_refc_pixel_format);
hi_void vpss_mac_set_nr_refc_cmp_mode(vpss_reg_type *reg, hi_u32 nr_refc_cmp_mode);
hi_void vpss_mac_set_nr_refc_is_lossless(vpss_reg_type *reg, hi_u32 nr_refc_is_lossless);
hi_void vpss_mac_set_nr_refc_dcmp_en(vpss_reg_type *reg, hi_u32 nr_refc_dcmp_en);
hi_void vpss_mac_set_nr_refc_frame_height(vpss_reg_type *reg, hi_u32 nr_refc_frame_height);
hi_void vpss_mac_set_nr_refc_frame_width(vpss_reg_type *reg, hi_u32 nr_refc_frame_width);
hi_void vpss_mac_set_nr_refc_buffer_fullness_thr(vpss_reg_type *reg, hi_u32 nr_refc_buffer_fullness_thr);
hi_void vpss_mac_set_nr_refc_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_refc_buffer_init_bits);
hi_void vpss_mac_set_nr_refc_budget_mb_bits(vpss_reg_type *reg, hi_u32 nr_refc_budget_mb_bits);
hi_void vpss_mac_set_nr_rfry_qp_force_en(vpss_reg_type *reg, hi_u32 nr_rfry_qp_force_en);
hi_void vpss_mac_set_nr_rfry_grph_en(vpss_reg_type *reg, hi_u32 nr_rfry_grph_en);
hi_void vpss_mac_set_nr_rfry_part_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfry_part_cmp_en);
hi_void vpss_mac_set_nr_rfry_pixel_format(vpss_reg_type *reg, hi_u32 nr_rfry_pixel_format);
hi_void vpss_mac_set_nr_rfry_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_rfry_frame_bitdepth);
hi_void vpss_mac_set_nr_rfry_cmp_mode(vpss_reg_type *reg, hi_u32 nr_rfry_cmp_mode);
hi_void vpss_mac_set_nr_rfry_is_lossless(vpss_reg_type *reg, hi_u32 nr_rfry_is_lossless);
hi_void vpss_mac_set_nr_rfry_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfry_cmp_en);
hi_void vpss_mac_set_nr_rfry_frame_height(vpss_reg_type *reg, hi_u32 nr_rfry_frame_height);
hi_void vpss_mac_set_nr_rfry_frame_width(vpss_reg_type *reg, hi_u32 nr_rfry_frame_width);
hi_void vpss_mac_set_nr_rfry_pcmp_end_hpos(vpss_reg_type *reg, hi_u32 nr_rfry_pcmp_end_hpos);
hi_void vpss_mac_set_nr_rfry_pcmp_start_hpos(vpss_reg_type *reg, hi_u32 nr_rfry_pcmp_start_hpos);
hi_void vpss_mac_set_nr_rfry_min_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfry_min_mb_bits);
hi_void vpss_mac_set_nr_rfry_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfry_mb_bits);
hi_void vpss_mac_set_nr_rfry_first_col_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfry_first_col_adj_bits);
hi_void vpss_mac_set_nr_rfry_first_row_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfry_first_row_adj_bits);
hi_void vpss_mac_set_nr_rfry_smooth_status_thr(vpss_reg_type *reg, hi_u32 nr_rfry_smooth_status_thr);
hi_void vpss_mac_set_nr_rfry_first_mb_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfry_first_mb_adj_bits);
hi_void vpss_mac_set_nr_rfry_diff_thr(vpss_reg_type *reg, hi_u32 nr_rfry_diff_thr);
hi_void vpss_mac_set_nr_rfry_big_grad_thr(vpss_reg_type *reg, hi_u32 nr_rfry_big_grad_thr);
hi_void vpss_mac_set_nr_rfry_still_thr(vpss_reg_type *reg, hi_u32 nr_rfry_still_thr);
hi_void vpss_mac_set_nr_rfry_smth_thr(vpss_reg_type *reg, hi_u32 nr_rfry_smth_thr);
hi_void vpss_mac_set_nr_rfry_noise_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfry_noise_pix_num_thr);
hi_void vpss_mac_set_nr_rfry_still_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfry_still_pix_num_thr);
hi_void vpss_mac_set_nr_rfry_smth_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfry_smth_pix_num_thr);
hi_void vpss_mac_set_nr_rfry_qp_dec2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_dec2_bits_thr);
hi_void vpss_mac_set_nr_rfry_qp_dec1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_dec1_bits_thr);
hi_void vpss_mac_set_nr_rfry_qp_inc2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_inc2_bits_thr);
hi_void vpss_mac_set_nr_rfry_qp_inc1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfry_qp_inc1_bits_thr);
hi_void vpss_mac_set_nr_rfry_grph_bits_penalty(vpss_reg_type *reg, hi_u32 nr_rfry_grph_bits_penalty);
hi_void vpss_mac_set_nr_rfry_buf_fullness_thr_reg0(vpss_reg_type *reg, hi_u32 nr_rfry_buf_fullness_thr_reg0);
hi_void vpss_mac_set_nr_rfry_buf_fullness_thr_reg1(vpss_reg_type *reg, hi_u32 nr_rfry_buf_fullness_thr_reg1);
hi_void vpss_mac_set_nr_rfry_buf_fullness_thr_reg2(vpss_reg_type *reg, hi_u32 nr_rfry_buf_fullness_thr_reg2);
hi_void vpss_mac_set_nr_rfry_qp_rge_reg0(vpss_reg_type *reg, hi_u32 nr_rfry_qp_rge_reg0);
hi_void vpss_mac_set_nr_rfry_qp_rge_reg1(vpss_reg_type *reg, hi_u32 nr_rfry_qp_rge_reg1);
hi_void vpss_mac_set_nr_rfry_qp_rge_reg2(vpss_reg_type *reg, hi_u32 nr_rfry_qp_rge_reg2);
hi_void vpss_mac_set_nr_rfry_bits_offset_reg0(vpss_reg_type *reg, hi_u32 nr_rfry_bits_offset_reg0);
hi_void vpss_mac_set_nr_rfry_bits_offset_reg1(vpss_reg_type *reg, hi_u32 nr_rfry_bits_offset_reg1);
hi_void vpss_mac_set_nr_rfry_bits_offset_reg2(vpss_reg_type *reg, hi_u32 nr_rfry_bits_offset_reg2);
hi_void vpss_mac_set_nr_rfry_grph_ideal_bits(vpss_reg_type *reg, hi_u32 nr_rfry_grph_ideal_bits);
hi_void vpss_mac_set_nr_rfry_est_err_gain_map(vpss_reg_type *reg, hi_u32 nr_rfry_est_err_gain_map);
hi_void vpss_mac_set_nr_rfry_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_rfry_buffer_init_bits);
hi_void vpss_mac_set_nr_rfrc_qp_force_en(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_force_en);
hi_void vpss_mac_set_nr_rfrc_grph_en(vpss_reg_type *reg, hi_u32 nr_rfrc_grph_en);
hi_void vpss_mac_set_nr_rfrc_part_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfrc_part_cmp_en);
hi_void vpss_mac_set_nr_rfrc_pixel_format(vpss_reg_type *reg, hi_u32 nr_rfrc_pixel_format);
hi_void vpss_mac_set_nr_rfrc_frame_bitdepth(vpss_reg_type *reg, hi_u32 nr_rfrc_frame_bitdepth);
hi_void vpss_mac_set_nr_rfrc_cmp_mode(vpss_reg_type *reg, hi_u32 nr_rfrc_cmp_mode);
hi_void vpss_mac_set_nr_rfrc_is_lossless(vpss_reg_type *reg, hi_u32 nr_rfrc_is_lossless);
hi_void vpss_mac_set_nr_rfrc_cmp_en(vpss_reg_type *reg, hi_u32 nr_rfrc_cmp_en);
hi_void vpss_mac_set_nr_rfrc_frame_height(vpss_reg_type *reg, hi_u32 nr_rfrc_frame_height);
hi_void vpss_mac_set_nr_rfrc_frame_width(vpss_reg_type *reg, hi_u32 nr_rfrc_frame_width);
hi_void vpss_mac_set_nr_rfrc_pcmp_end_hpos(vpss_reg_type *reg, hi_u32 nr_rfrc_pcmp_end_hpos);
hi_void vpss_mac_set_nr_rfrc_pcmp_start_hpos(vpss_reg_type *reg, hi_u32 nr_rfrc_pcmp_start_hpos);
hi_void vpss_mac_set_nr_rfrc_min_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_min_mb_bits);
hi_void vpss_mac_set_nr_rfrc_mb_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_mb_bits);
hi_void vpss_mac_set_nr_rfrc_first_col_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_first_col_adj_bits);
hi_void vpss_mac_set_nr_rfrc_first_row_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_first_row_adj_bits);
hi_void vpss_mac_set_nr_rfrc_smooth_status_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_smooth_status_thr);
hi_void vpss_mac_set_nr_rfrc_first_mb_adj_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_first_mb_adj_bits);
hi_void vpss_mac_set_nr_rfrc_diff_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_diff_thr);
hi_void vpss_mac_set_nr_rfrc_big_grad_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_big_grad_thr);
hi_void vpss_mac_set_nr_rfrc_still_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_still_thr);
hi_void vpss_mac_set_nr_rfrc_smth_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_smth_thr);
hi_void vpss_mac_set_nr_rfrc_noise_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_noise_pix_num_thr);
hi_void vpss_mac_set_nr_rfrc_still_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_still_pix_num_thr);
hi_void vpss_mac_set_nr_rfrc_smth_pix_num_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_smth_pix_num_thr);
hi_void vpss_mac_set_nr_rfrc_qp_dec2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_dec2_bits_thr);
hi_void vpss_mac_set_nr_rfrc_qp_dec1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_dec1_bits_thr);
hi_void vpss_mac_set_nr_rfrc_qp_inc2_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_inc2_bits_thr);
hi_void vpss_mac_set_nr_rfrc_qp_inc1_bits_thr(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_inc1_bits_thr);
hi_void vpss_mac_set_nr_rfrc_grph_bits_penalty(vpss_reg_type *reg, hi_u32 nr_rfrc_grph_bits_penalty);
hi_void vpss_mac_set_nr_rfrc_buf_fullness_thr_reg0(vpss_reg_type *reg, hi_u32 nr_rfrc_buf_fullness_thr_reg0);
hi_void vpss_mac_set_nr_rfrc_buf_fullness_thr_reg1(vpss_reg_type *reg, hi_u32 nr_rfrc_buf_fullness_thr_reg1);
hi_void vpss_mac_set_nr_rfrc_buf_fullness_thr_reg2(vpss_reg_type *reg, hi_u32 nr_rfrc_buf_fullness_thr_reg2);
hi_void vpss_mac_set_nr_rfrc_qp_rge_reg0(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_rge_reg0);
hi_void vpss_mac_set_nr_rfrc_qp_rge_reg1(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_rge_reg1);
hi_void vpss_mac_set_nr_rfrc_qp_rge_reg2(vpss_reg_type *reg, hi_u32 nr_rfrc_qp_rge_reg2);
hi_void vpss_mac_set_nr_rfrc_bits_offset_reg0(vpss_reg_type *reg, hi_u32 nr_rfrc_bits_offset_reg0);
hi_void vpss_mac_set_nr_rfrc_bits_offset_reg1(vpss_reg_type *reg, hi_u32 nr_rfrc_bits_offset_reg1);
hi_void vpss_mac_set_nr_rfrc_bits_offset_reg2(vpss_reg_type *reg, hi_u32 nr_rfrc_bits_offset_reg2);
hi_void vpss_mac_set_nr_rfrc_grph_ideal_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_grph_ideal_bits);
hi_void vpss_mac_set_nr_rfrc_est_err_gain_map(vpss_reg_type *reg, hi_u32 nr_rfrc_est_err_gain_map);
hi_void vpss_mac_set_nr_rfrc_buffer_init_bits(vpss_reg_type *reg, hi_u32 nr_rfrc_buffer_init_bits);
/************/
hi_void vpss_mmu_set_glb_bypass(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 glb_bypass);
hi_void vpss_mmu_set_auto_clk_gt_en(vpss_reg_type *vdp_reg, hi_u32 layer, hi_u32 auto_clk_gt_en);
hi_void vpss_mmu_set_int_en(vpss_reg_type *vdp_reg, hi_u32 layer, hi_u32 int_en);
hi_void vpss_mmu_set_ptw_pf(vpss_reg_type *vdp_reg, hi_u32 layer, hi_u32 ptw_pf);
hi_void vpss_mmu_set_scb_ttbr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 scb_ttbr);
hi_void vpss_mmu_set_scb_ttbr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 scb_ttbr_h);
hi_void vpss_mmu_set_err_srd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_rd_addr);
hi_void vpss_mmu_set_err_srd_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_rd_addr_h);
hi_void vpss_mmu_set_err_swr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_wr_addr);
hi_void vpss_mmu_set_err_swr_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_s_wr_addr_h);
hi_void vpss_mmu_get_err_srd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_s_rd_addr);
hi_void vpss_mmu_get_err_swr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_s_wr_addr);
hi_void vpss_mmu_set_sintmask(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 mask);
hi_void vpss_mmu_set_nsintmask(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 mask);

hi_void vpss_mmu_set_cb_ttbr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 cb_ttbr);
hi_void vpss_mmu_set_cb_ttbr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 cb_ttbr_h);
hi_void vpss_mmu_set_err_ns_rd_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_rd_addr_h);
hi_void vpss_mmu_set_err_ns_rd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_rd_addr);
hi_void vpss_mmu_set_err_ns_wr_addr_h(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_wr_addr_h);
hi_void vpss_mmu_set_err_ns_wr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 err_ns_wr_addr);
hi_void vpss_mmu_get_err_ns_wr_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_ns_wr_addr);
hi_void vpss_mmu_get_err_ns_rd_addr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 *err_ns_rd_addr);

hi_u32 vpss_mmu_get_ints_stat(vpss_reg_type *vpss_reg, hi_u32 layer);
hi_void vpss_mmu_set_ints_clr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 ints_clr);
hi_u32 vpss_mmu_get_intns_stat(vpss_reg_type *vpss_reg, hi_u32 layer);
hi_void vpss_mmu_set_intns_clr(vpss_reg_type *vpss_reg, hi_u32 layer, hi_u32 intns_clr);
hi_void vpss_mmu_set_intns_tlbinvalid_rd_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_tlbinvalid_rd_msk);
hi_void vpss_mmu_set_intns_tlbinvalid_wr_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_tlbinvalid_wr_msk);
hi_void vpss_mmu_set_intns_ptw_trans_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_ptw_trans_msk);
hi_void vpss_mmu_set_intns_tlbmiss_msk(vpss_reg_type *reg, hi_u32 layer, hi_u32 intns_tlbmiss_msk);
hi_void vpss_sys_set_four_pixel_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 four_pixel_en);
hi_void vpss_sys_setdbm_430_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 dbm_430_en);
hi_void vpss_sys_sethdr_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 hdr_en);
hi_void vpss_vc1_set_vc1_mapc(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapc);
hi_void vpss_vc1_set_vc1_mapy(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapy);
hi_void vpss_vc1_set_vc1_mapcflg(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapcflg);
hi_void vpss_vc1_set_vc1_mapyflg(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_mapyflg);
hi_void vpss_vc1_set_vc1_rangedfrm(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_rangedfrm);
hi_void vpss_vc1_set_vc1_profile(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vc1_profile);

hi_s32 vpss_mac_init_rchn_cfg(vpss_mac_rchn_cfg *rchn_cfg);
hi_void vpss_mac_set_rchn_cfg(vpss_reg_type *reg, vpss_mac_rchn layer, vpss_mac_rchn_cfg *rchn_cfg);
hi_void vpss_mac_set_rchn_cf_cfg(vpss_reg_type *reg, vpss_mac_rchn layer, vpss_mac_rchn_cfg *rchn_cfg);

hi_s32 vpss_mac_init_wchn_cfg(vpss_mac_wchn_cfg *wchn_cfg);
hi_s32 vpss_mac_set_wchn_cfg(vpss_reg_type *reg, vpss_mac_wchn layer, vpss_mac_wchn_cfg *wchn_cfg);
hi_s32 vpss_reg_set_rotate_cfg(vpss_reg_type *app_addr, hi_drv_vpss_rotation angle, hi_bool rotate_y);

hi_void vpss_mac_set_reg_load(vpss_reg_type *reg);
hi_bool vpss_get_wchn_class_type(vpss_mac_wchn layer);
hi_s32 vpss_crg_set_clock_en(hi_u32 vpss_ip, hi_bool clk_en);

hi_void vpss_soft_reset(vpss_ip ip, vpss_reg_type *reg, hi_bool rst_en);
hi_void vpss_master_set_split_mode(vpss_reg_type *reg, hi_u32 split_mode);

hi_void vpss_set_out0y_bit_depth(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_bit_depth);
hi_void vpss_set_out0y_esl_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_esl_qp);
hi_void vpss_set_out0y_chroma_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_chroma_en);
hi_void vpss_set_out0y_is_lossless(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_is_lossless);
hi_void vpss_set_out0y_cmp_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_cmp_mode);
hi_void vpss_set_out0y_ice_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_ice_en);
hi_void vpss_set_out0y_frame_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_frame_height);
hi_void vpss_set_out0y_frame_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_frame_width);
hi_void vpss_set_out0y_min_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_min_mb_bits);
hi_void vpss_set_out0y_budget_mb_bits_last(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_budget_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_max_mb_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_noise_pix_num_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_smooth_status_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_diff_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_diff_thr);
hi_void vpss_set_out0y_big_grad_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_dec2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_dec1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_inc2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_inc1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_buf_fullness_thr_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_buf_fullness_thr_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_buf_fullness_thr_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_rge_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_rge_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_qp_rge_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_bits_offset_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_bits_offset_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_bits_offset_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_est_err_gain_map(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_buffer_size(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_buffer_size);
hi_void vpss_set_out0y_buffer_init_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_reserve_para0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_reserve_para1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_adpqp_thr0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_adpqp_thr0);
hi_void vpss_set_out0y_adpqp_thr1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_adpqp_thr1);
hi_void vpss_set_out0y_smooth_deltabits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_max_left_bits_buffer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0y_debug_info(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0y_debug_info);
hi_void vpss_set_out0c_bit_depth(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_bit_depth);
hi_void vpss_set_out0c_esl_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_esl_qp);
hi_void vpss_set_out0c_chroma_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_chroma_en);
hi_void vpss_set_out0c_is_lossless(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_is_lossless);
hi_void vpss_set_out0c_cmp_mode(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_cmp_mode);
hi_void vpss_set_out0c_ice_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_ice_en);
hi_void vpss_set_out0c_frame_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_frame_height);
hi_void vpss_set_out0c_frame_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_frame_width);
hi_void vpss_set_out0c_min_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_min_mb_bits);
hi_void vpss_set_out0c_budget_mb_bits_last(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_budget_mb_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_max_mb_qp(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_max_mb_qp);
hi_void vpss_set_out0c_noise_pix_num_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_smooth_status_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_diff_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_diff_thr);
hi_void vpss_set_out0c_big_grad_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 out0c_big_grad_thr);
hi_void vpss_set_out0c_qp_dec2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_qp_dec1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_qp_inc2_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_qp_inc1_bits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_buf_fullness_thr_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_buf_fullness_thr_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_buf_fullness_thr_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_qp_rge_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_qp_rge_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_qp_rge_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_bits_offset_reg0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_bits_offset_reg1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_bits_offset_reg2(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_est_err_gain_map(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_buffer_size(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_buffer_init_bits(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_reserve_para0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_reserve_para1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_adpqp_thr0(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_adpqp_thr1(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_smooth_deltabits_thr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_max_left_bits_buffer(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_set_out0c_debug_info(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 para);
hi_void vpss_sys_set_vhd0_crop_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 crop_en);
hi_void vpss_sys_set_vhd0_crop_x(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_x);
hi_void vpss_sys_set_vhd0_crop_y(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_y);
hi_void vpss_sys_set_vhd0_crop_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_width);
hi_void vpss_sys_set_vhd0_crop_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_crop_height);
hi_void vpss_sys_set_vhd0_vbk_y(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_vbk_y);
hi_void vpss_sys_set_vhd0_vbk_cb(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_vbk_cb);
hi_void vpss_sys_set_vhd0_vbk_cr(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_vbk_cr);
hi_void vpss_sys_set_vhd0_lba_xfpos(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_xfpos);
hi_void vpss_sys_set_vhd0_lba_yfpos(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_yfpos);
hi_void vpss_sys_set_vhd0_lba_width(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_width);
hi_void vpss_sys_set_vhd0_lba_height(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 vhd0_lba_height);
hi_void vpss_sys_set_vhd0_lba_en(vpss_reg_type *vpss_regs, hi_u32 addr_offset, hi_u32 lbox_en);
hi_void vpss_sys_set_allip_ck_gt_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 ck_gt_en);

hi_void vpss_ifir_set_mode(vpss_reg_type *vdp_reg, hi_u32 data, hal_ifir_mode mode);
hi_void vpss_ifir_set_mid_en(vpss_reg_type *vdp_reg, hi_u32 data, hi_u32 en);
hi_void vpss_ifir_set_ck_gt_en(vpss_reg_type *vdp_reg, hi_u32 data, hi_u32 en);
hi_void vpss_ifir_set_coef(vpss_reg_type *vdp_reg, hi_u32 data, hi_s32 *coef, hi_u32 coef_num);

hi_void vpss_dfir_set422_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set420_en(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set420_mode(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef0(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef1(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef2(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef3(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef4(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef5(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);
hi_void vpss_dfir_set_hfir_coef6(vpss_reg_type *vpss_reg, hi_u32 addr_offset, hi_u32 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

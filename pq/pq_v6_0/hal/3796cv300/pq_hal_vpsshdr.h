/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef _PQ_HAL_VPSSHDR_H_
#define _PQ_HAL_VPSSHDR_H_

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "drv_pq_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PQ_VPSSHDR_CM_8            8
#define VPSSHDR_IMAP_CM_LUT_9      9
#define VPSSHDR_OMAP_CM_LUT_9      9

typedef enum {
    VPSS_HDR_COEF_BUF_CH_VZME = 0,
    VPSS_HDR_COEF_BUF_HDR = 1,
    VPSS_HDR_COEF_BUF_HDR1 = 2,
    VPSS_HDR_COEF_BUF_HDR2 = 3,
    VPSS_HDR_COEF_BUF_HDR3 = 4,
    VPSS_HDR_COEF_BUF_HDR4 = 5,
    VPSS_HDR_COEF_BUF_HDR5 = 6,
    VPSS_HDR_COEF_BUF_ALL = 12,
    VPSS_HDR_COEF_BUF_BUTT = 13
} pq_vpss_coef_buf;

#define VPSS_COEF_SIZE_CH_VZME   (4096 * 128 / 8)
#define VPSS_COEF_SIZE_HDR       (4096 * 128 / 8)
#define VPSS_COEF_SIZE_HDR1      (4096 * 128 / 8)
#define VPSS_COEF_SIZE_HDR2      (4096 * 128 / 8)
#define VPSS_COEF_SIZE_HDR3      (4096 * 128 / 8)
#define VPSS_COEF_SIZE_HDR4      (4096 * 128 / 8)
#define VPSS_COEF_SIZE_HDR5      (4096 * 128 / 8)

#define VDP_VPSSHDR_COEF_SIZE (VPSS_COEF_SIZE_CH_VZME + VPSS_COEF_SIZE_HDR +        \
                               VPSS_COEF_SIZE_HDR1 + VPSS_COEF_SIZE_HDR2 +          \
                               VPSS_COEF_SIZE_HDR3 + VPSS_COEF_SIZE_HDR4 + VPSS_COEF_SIZE_HDR5)

typedef struct {
    hi_bool enable;
    hi_bool rshift_round_en;
    hi_bool sclut_rd_en;
    hi_bool sslut_rd_en;
    hi_bool tslut_rd_en;
    hi_bool silut_rd_en;
    hi_bool tilut_rd_en;
    hi_bool smc_enable;
    hi_bool rshift_en;
    hi_bool s2u_en;

    hi_u32 c1_expan;
    hi_u32 para_rdata;
    hi_u32 rshift_bit;
} pq_vpsshdr_tmap_cfg;

typedef enum {
    PQ_VPSSHDR_CM_POS0 = 0,
    PQ_VPSSHDR_CM_POS1,

    PQ_VPSSHDR_CM_POS_MAX
} pq_vpsshdr_cm_pos;

typedef enum {
    PQ_VPSSHDR_TM_POS0 = 0,
    PQ_VPSSHDR_TM_POS1,

    PQ_VPSSHDR_TM_POS_MAX
} pq_vpsshdr_tm_pos;

typedef struct {
    hi_u32 u32size;
    hi_u8 *vir_addr[VPSS_HDR_COEF_BUF_BUTT + 1];
    hi_u64 phy_addr[VPSS_HDR_COEF_BUF_BUTT + 1];
} pq_vpsshdr_coef_addr;

typedef struct {
    hi_bool enable;
    hi_u32 clip_c_max;
    hi_u32 clip_c_min;
    hi_u32 scale2p;
    hi_u32 step[PQ_VPSSHDR_CM_8]; /* 8 is number */
    hi_u32 pos[PQ_VPSSHDR_CM_8];  /* 8 is number */
    hi_u32 num[PQ_VPSSHDR_CM_8];  /* 8 is number */
} pq_vpsshdrv1_cm_cfg;

typedef struct {
    hi_bool enable;
    hi_bool rshift_round_en;
    hi_bool sclut_rd_en;
    hi_bool sslut_rd_en;
    hi_bool tslut_rd_en;
    hi_bool silut_rd_en;
    hi_bool tilut_rd_en;
    hi_bool smc_enable;
    hi_bool rshift_en;
    hi_bool s2u_en;

    hi_u32 c1_expan;
    hi_u32 para_rdata;
    hi_u32 rshift_bit;
} pq_vpss_tmap_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 min;
    hi_u32 max;
    hi_u32 scale2p;
    hi_s32 out_dc[3];  /* 3 is number */
    hi_s32 in_dc[3];   /* 3 is number */
    hi_s16 coef[3][3]; /* 3 is number */
} pq_vpss_ipt2lms_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 min;
    hi_u32 max;
    hi_u32 scale2p;
    hi_s16 coef[3][3]; /* 3 is number */
} pq_vpss_lms2rgb_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 cm_pos;
    hi_u32 demo_pos;
    hi_u32 demo_mode;
    hi_bool demo_en;
    hi_u32 bitdepth_out_mode;
    hi_u32 bitdepth_in_mode;
} pq_vpss_cacm_cfg;

typedef struct {
    hi_bool enable;
    hi_s16 coef[3][3]; /* 3 is number */
    hi_u32 scale2p;
    hi_s32 out_dc[3]; /* 3 is number */
    hi_s32 in_dc[3];  /* 3 is number */
    hi_u32 min;
    hi_u32 max;
} pq_vpss_rgb2yuv_cfg;

typedef struct {
    hi_bool enable;

    hi_bool ipt_out_en; /* OmapIptOutEn */
    hi_bool u2s_enable;
    hi_bool lshift_en;
    hi_bool degamma_en;
    hi_bool gamma_en;
    hi_bool denorm_en;
    hi_bool cvm_en;

    hi_u32 out_bits;
    hi_u32 out_color;
    hi_u32 ipt_scale;
    hi_u32 oetf;
    hi_u32 range_min;
    hi_u32 range_over;
    hi_s32 ipt_off[3]; /* 3 is number */

    pq_vpss_ipt2lms_cfg ipt2lms;
    pq_vpss_lms2rgb_cfg lms2rgb;
    pq_vpss_cacm_cfg cm_cfg;
    pq_vpss_rgb2yuv_cfg rgb2yuv;
} pq_vpss_omap_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 clip_c_max;
    hi_u32 clip_c_min;
    hi_u32 scale2p;
    hi_u32 step[PQ_VPSSHDR_CM_8]; /* 8 is number */
    hi_u32 pos[PQ_VPSSHDR_CM_8]; /* 8 is number */
    hi_u32 num[PQ_VPSSHDR_CM_8]; /* 8 is number */
} pq_vpsshdr_v1_cm_cfg;

typedef struct {
    pq_vpsshdr_v1_cm_cfg cm_cfg;
    hi_drv_pq_imap_cfg imap_cfg;
    pq_vpsshdr_tmap_cfg tmap_cfg;
    hi_drv_pq_hdr_coef_addr coef_addr;
} pq_hal_vpsshdr_cfg;

extern pq_vpsshdr_coef_addr g_pq_vdp_vpss_hdr_coef_buf[VPSS_HANDLE_NUM];

hi_s32 pq_hal_get_vpsshdr_cfg(drv_pq_internal_xdr_frame_info *xdr_frame_info, pq_hal_vpsshdr_cfg *hdr_cfg);
hi_s32 pq_hal_set_vpsshdr_cfg(hi_drv_pq_vpss_layer layer_id, pq_hal_vpsshdr_cfg *hdr_cfg,
                              vpss_reg_type *vpss_reg);

hi_s32 pq_hal_set_vpsshdr_offset(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 pq_hal_set_vpsshdr_csc_setting(hi_pq_image_param *pic_setting);

hi_s32 pq_hal_set_vpsshdr_tm_curve(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 pq_hal_init_vpsshdr(pq_bin_param *pq_param, hi_bool default_code);

hi_s32 pq_hal_deinit_vpsshdr(hi_void);
hi_s32 pq_hal_get_vpsshdr_cfg_proc(hi_bool *cm_enable, hi_bool *imap_enable, hi_bool *tmap_enable);
hi_s32 pq_hal_set_vpsshdr_coef_addr(hi_drv_pq_vpss_layer layer_id, pq_hal_vpsshdr_cfg *hdr_cfg,
                                    vpss_reg_type *vpss_reg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

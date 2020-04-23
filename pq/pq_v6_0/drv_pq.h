/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq common define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __DRV_PQ_H__
#define __DRV_PQ_H__

#include "hi_debug.h"
#include "hi_type.h"

#include "hi_drv_pq.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_module.h"
#include "pq_hal_comm.h"
#include "drv_pq_table.h"

#ifdef PQ_ALG_TOOLS
#include "drv_pq_tools.h"
#endif

#ifdef PQ_ALG_ZME
#include "pq_mng_zme.h"
#endif

#ifdef PQ_ALG_VPSSZME
#include "pq_mng_vpsszme.h"
#endif

#ifdef PQ_ALG_SR
#include "pq_mng_sr.h"
#endif

#ifdef PQ_ALG_CSC
#include "pq_mng_csc.h"
#endif

#ifdef PQ_ALG_DEI
#include "pq_mng_dei.h"
#endif

#ifdef PQ_ALG_DB
#include "pq_mng_db.h"
#endif

#ifdef PQ_ALG_DM
#include "pq_mng_dm.h"
#endif

#ifdef PQ_ALG_DM_MZ
#include "pq_mng_dm_mz.h"
#endif

#ifdef PQ_ALG_TNR
#include "pq_mng_tnr.h"
#endif

#ifdef PQ_ALG_SNR
#include "pq_mng_snr.h"
#endif

#ifdef PQ_ALG_DERING
#include "pq_mng_dering.h"
#endif

#ifdef PQ_ALG_DESHOOT
#include "pq_mng_deshoot.h"
#endif

#ifdef PQ_ALG_GFXCSC
#include "pq_mng_gfxcsc.h"
#endif

#ifdef PQ_ALG_GFXZME
#include "pq_mng_gfxzme.h"
#endif

#ifdef PQ_ALG_GFXHDR
#include "pq_mng_gfxhdr.h"
#endif

#ifdef PQ_ALG_FMD
#include "pq_mng_ifmd.h"
#endif

#ifdef PQ_ALG_SHARPEN
#include "pq_mng_sharpen.h"
#endif

#ifdef PQ_ALG_CLE
#include "pq_mng_cle.h"
#endif

#ifdef PQ_ALG_ACM
#include "pq_mng_acm.h"
#endif

#ifdef PQ_ALG_DCI
#include "pq_mng_dci.h"
#endif

#ifdef PQ_ALG_DECONTOUR
#include "pq_mng_decontour.h"
#endif

#ifdef PQ_ALG_HDR
#include "pq_mng_hdr.h"
#endif

#ifdef PQ_ALG_VPSSCSC
#include "pq_mng_vpsscsc.h"
#endif

#ifdef PQ_ALG_VPSSZME
#include "pq_mng_vpsszme.h"
#endif

#ifdef PQ_ALG_VPSSHDR
#include "pq_mng_vpsshdr.h"
#endif

#ifdef PQ_ALG_COCO
#include "pq_mng_colorcoring.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PQ_NAME "HI_PQ"

/* 用户PQ 数据结构 */
typedef struct {
    hi_pq_image_param sd_picture_setting;                        /* graph SD setting */
    hi_pq_image_param hd_picture_setting;                        /* graph HD setting */
    hi_pq_image_param sd_video_setting;                          /* video SD setting */
    hi_pq_image_param hd_video_setting;                          /* video HD setting */
    hi_pq_image_param vpss_video_setting[HI_DRV_PQ_VP_TYPE_MAX]; /* vpss video setting */
    hi_u32 strength[HI_PQ_MODULE_MAX];
    hi_bool demo_on_off[HI_PQ_MODULE_MAX];
    hi_bool debug_on_off[HI_PQ_MODULE_MAX];
    hi_bool module_on_off[HI_PQ_MODULE_MAX];
    hi_pq_color_enhance color_enhance;
    hi_pq_demo_mode demo_mode;
} hi_drv_pq_param;

/* *  VPSS info  */ /* *CNcomment: VPSS timing  级更新系数 */
typedef struct {
    hi_drv_source input_src;
    hi_u32 width;
    hi_u32 height;
    hi_u32 frame_rate;
    hi_bool interlace;
    hi_drv_color_sys color_sys;
    hi_bool sp422;
} hi_drv_pq_vpss_timming;

typedef enum {
    PQ_UPDATE_INFT = 0,
    PQ_UPDATE_PROC,
    PQ_UPDATE_DRIVER,

    PQ_PQ_UPDATE_MAX
} pq_update_module;

typedef enum {
    PQ_UPDATE_VDP_ALG = 0,
    PQ_UPDATE_VDP_STT,
    PQ_SET_VDP_HDR,
    PQ_GET_VDP_ZME,
    PQ_GET_VDP_CSC,

    PQ_INIT_VPSS_ALG,
    PQ_UPDATE_VPSS_STT,
    PQ_UPDATE_VPSS_ALG,
    PQ_GET_VPSS_ZME,
    PQ_SET_VPSS_HDR,

    PQ_GET_GFX_CSC,
    PQ_GET_GFX_ZME,
    PQ_SET_GFX_HDR,
    PQ_GET_HWC_HDR,

    PQ_UPDATE_VDP_AI,
    PQ_UPDATE_VPSS_AI,

    PQ_UPDATE_NR_STT,
    PQ_UPDATE_DEI_STT,
    PQ_UPDATE_DB_STT,
    PQ_UPDATE_DM_STT,

    PQ_UPDATE_NR_ALG,
    PQ_UPDATE_DEI_ALG,
    PQ_UPDATE_DB_ALG,
    PQ_UPDATE_DM_ALG,

    PQ_INTF_MODE_MAX
} pq_intf_mode;

typedef struct {
    hi_bool is_pq_module_init;
    hi_bool is_pq_register;
    hi_bool is_spinlock_init;
    hi_bool is_pqbin_init;
    hi_bool is_vdp_alg_register;
    hi_bool is_vpss_alg_register;
    hi_bool is_comm_init;
    hi_bool is_table_init;
    hi_bool is_vdp_alg_stt_init;
    hi_bool is_alg_init;
    hi_bool is_vdp_thread_init;
    hi_bool is_vpss_thread_init;
} pq_init_module;

extern hi_ulong g_proc_intf_time_cnt[PQ_INTF_MODE_MAX];

hi_s32 drv_pq_create_proc(hi_void);
hi_void drv_pq_remove_proc(hi_void);
hi_s32 drv_pq_get_brightness(hi_pq_display pq_display, hi_u32 *brightness);

hi_s32 drv_pq_set_brightness(hi_pq_display pq_display, hi_u32 brightness);

hi_s32 drv_pq_get_contrast(hi_pq_display pq_display, hi_u32 *contrast);

hi_s32 drv_pq_set_contrast(hi_pq_display pq_display, hi_u32 contrast);

hi_s32 drv_pq_get_hue(hi_pq_display pq_display, hi_u32 *hue);

hi_s32 drv_pq_set_hue(hi_pq_display pq_display, hi_u32 hue);

hi_s32 drv_pq_get_saturation(hi_pq_display pq_display, hi_u32 *saturation);

hi_s32 drv_pq_set_saturation(hi_pq_display pq_display, hi_u32 saturation);

/* vpss csc */
hi_s32 drv_pq_get_vpbrightness(hi_drv_pq_vp_type vp_mode, hi_u32 *brightness);

hi_s32 drv_pq_get_vpcontrast(hi_drv_pq_vp_type vp_mode, hi_u32 *contrast);

hi_s32 drv_pq_get_vphue(hi_drv_pq_vp_type vp_mode, hi_u32 *hue);

hi_s32 drv_pq_get_vpsaturation(hi_drv_pq_vp_type vp_mode, hi_u32 *saturation);

hi_s32 drv_pq_set_vpbrightness(hi_drv_pq_vp_type vp_mode, hi_u32 brightness);

hi_s32 drv_pq_set_vpcontrast(hi_drv_pq_vp_type vp_mode, hi_u32 contrast);

hi_s32 drv_pq_set_vphue(hi_drv_pq_vp_type vp_mode, hi_u32 hue);

hi_s32 drv_pq_set_vpsaturation(hi_drv_pq_vp_type vp_mode, hi_u32 saturation);

hi_s32 drv_pq_get_vpvideo_setting(hi_drv_pq_vp_type vp_mode, hi_pq_image_param *setting);

hi_s32 drv_pq_set_vpvideo_setting(hi_drv_pq_vp_type vp_mode, hi_pq_image_param *setting);

hi_s32 drv_pq_get_sharpness(hi_u32 *sharpness);

hi_s32 drv_pq_set_sharpness(hi_u32 sharpness);

hi_s32 drv_pq_get_tnr_level(hi_u32 *nr_level);

hi_s32 drv_pq_get_snr_level(hi_u32 *nr_level);

hi_s32 drv_pq_set_tnr_level(hi_u32 tnr_level);

hi_s32 drv_pq_set_snr_level(hi_u32 tnr_level);

hi_s32 drv_pq_get_color_ehance(hi_u32 *color_gain_level);

hi_s32 drv_pq_set_color_ehance(hi_u32 color_gain_level);

hi_s32 drv_pq_get_flesh_tone_level(hi_u32 *flesh_tone_level);

hi_s32 drv_pq_set_flesh_tone_level(hi_pq_fleshtone flesh_tone_level);

hi_s32 drv_pq_set_dci_level(hi_u32 dci_level);

hi_s32 drv_pq_get_dci_level(hi_u32 *dci_level);

hi_s32 drv_pq_set_demo_en(hi_pq_module_type module, hi_bool on_off);

hi_s32 drv_pq_get_demo_en(hi_pq_module_type module, hi_bool *enable);

hi_s32 drv_pq_set_debug_en(hi_pq_module_type module, hi_bool on_off);

hi_s32 drv_pq_get_pq_module(hi_pq_module_type module, hi_u32 *on_off);

hi_s32 drv_pq_set_pq_module(hi_pq_module_type module, hi_u32 on_off);

hi_s32 drv_pq_get_bin_phy_addr(hi_u64 *addr);

hi_s32 drv_pq_set_six_base_color(hi_pq_six_base_color *color_offset);

hi_s32 drv_pq_get_six_base_color(hi_pq_six_base_color *color_offset);

hi_s32 drv_pq_set_color_enhance_mode(hi_pq_color_spec_mode color_spec_mode);

hi_s32 drv_pq_get_color_enhance_mode(hi_pq_color_spec_mode *color_spec_mode);

hi_s32 drv_pq_set_demo_disp_mode(pq_reg_type flag, hi_pq_demo_mode demo_mode);

hi_s32 drv_pq_get_demo_disp_mode(hi_pq_demo_mode *pen_demo_mode);

hi_s32 drv_pq_get_default_param(hi_bool *default_code);

hi_s32 drv_pq_set_demo_coordinate(pq_reg_type type, hi_u32 x);

hi_s32 drv_pq_get_demo_coordinate(hi_u32 *x);

hi_s32 drv_pq_get_db_level(hi_u32 *db_level);

hi_s32 drv_pq_set_db_level(hi_u32 db_level);

hi_s32 drv_pq_get_dm_level(hi_u32 *dm_level);

hi_s32 drv_pq_set_dm_level(hi_u32 dm_level);

hi_s32 drv_pq_set_dc_level(hi_u32 dc_level);

hi_s32 drv_pq_get_dc_level(hi_u32 *dc_level);

hi_s32 drv_pq_get_coco_level(hi_u32 *coco_level);

hi_s32 drv_pq_set_coco_level(hi_u32 coco_level);

hi_s32 drv_pq_get_acm_level(hi_u32 *acm_level);

hi_s32 drv_pq_set_acm_level(hi_u32 acm_level);

hi_s32 drv_pq_set_dr_level(hi_u32 dr_level);

hi_s32 drv_pq_get_dr_level(hi_u32 *dr_level);

hi_s32 drv_pq_set_ds_level(hi_u32 ds_level);

hi_s32 drv_pq_get_ds_level(hi_u32 *ds_level);

hi_s32 drv_pq_set_image_mode(hi_pq_image_mode image_mode);

hi_s32 drv_pq_get_image_mode(hi_pq_image_mode *pen_image_mode);

hi_s32 drv_pq_set_temperature(hi_pq_display disp, hi_pq_color_temperature *temperature);

hi_s32 drv_pq_get_temperature(hi_pq_display disp, hi_pq_color_temperature *temperature);

hi_s32 drv_pq_set_vppreview_mode(hi_drv_pq_vp_mode vp_scene_mode);

hi_s32 drv_pq_get_vppreview_mode(hi_drv_pq_vp_mode *pen_vp_scene_mode);

hi_s32 drv_pq_set_vpremote_mode(hi_drv_pq_vp_mode vp_scene_mode);

hi_s32 drv_pq_get_vpremote_mode(hi_drv_pq_vp_mode *pen_vp_scene_mode);

hi_s32 drv_pq_set_hdr_offset_para(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 drv_pq_set_gfx_hdr_offset_para(hi_pq_hdr_offset *hdr_offset_para);

hi_s32 drv_pq_suspend(hi_void *pdev);

hi_s32 drv_pq_resume(hi_void *pdev);

hi_s32 drv_pq_set_sd_picture_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_set_hd_picture_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_set_sd_video_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_set_hd_video_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_get_sd_video_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_get_hd_video_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_get_sd_picture_setting(hi_pq_image_param *setting);
hi_s32 drv_pq_get_hd_picture_setting(hi_pq_image_param *setting);

hi_s32 drv_pq_set_pq_status(pq_common_status *pq_status);
hi_s32 drv_pq_get_pq_status(pq_common_status *pq_status);
hi_s32 drv_pq_get_pq_bin_param(pq_bin_param *param);
hi_s32 drv_pq_set_pq_bin_param(pq_bin_param *param);
hi_s32 drv_pq_get_pq_param(hi_drv_pq_param *param);
hi_s32 drv_pq_set_pq_param(hi_drv_pq_param *param);
hi_s32 drv_pq_get_pq_bin_status(hi_bool *bin_status);
hi_s32 drv_pq_set_vpss_timming(hi_drv_pq_vpss_timming vpss_timming);
hi_s32 drv_pq_get_vpss_timming(hi_drv_pq_vpss_timming *vpss_timming);
hi_s32 drv_pq_get_init_flag(hi_bool *flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* end of #ifndef __DRV_PQ_H__ */

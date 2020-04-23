/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq key api
 * Author: pq
 * Create: 2016-01-1
 */

#include "drv_pq.h"
#ifdef PQ_PDM_SUPPORT
#include "drv_pdm_ext.h"
#endif
#include "drv_pq_ext.h"
#include "hi_drv_dev.h"
#include "drv_pq_vdp_thread.h"
#include "drv_pq_vpss_thread.h"

#define PQ_THREAD_INDEX_MAX 30

osal_spinlock g_pq_status_lock;
hi_u32 g_pq_vpss_alg_index[VPSS_HANDLE_NUM] = { 0 };
pq_bin_param *g_pq_bin_param = HI_NULL;
drv_pq_mem_info g_pq_bin_buf = { 0 };
hi_bool g_load_pq_bin = HI_FALSE;
pq_init_module g_pq_init_flag = {0};
hi_u32 g_pq_init_count = 0;
hi_bool g_sd_update_flag[HI_PQ_MODULE_MAX][PQ_PQ_UPDATE_MAX] = { { HI_TRUE, HI_TRUE } };
hi_bool g_hd_update_flag[HI_PQ_MODULE_MAX][PQ_PQ_UPDATE_MAX] = { { HI_TRUE, HI_TRUE } };

hi_u32 g_image_width = PQ_WIDTH_1280;
hi_u32 g_image_height = PQ_WIDTH_720;
hi_u32 g_out_width = PQ_WIDTH_1280;
hi_u32 g_out_height = PQ_WIDTH_720;

hi_drv_pq_param g_pq_param = { { 0 } };
hi_drv_pq_vpss_timming g_pq_vpss_timming =  {0};
pq_common_status g_pq_status = { 0 };

static hi_u32 g_demo_coordinate = PQ_ALG_DEFAULT_VALUE; /* range: 0~100 */
static hi_bool g_is_fast_boot = HI_FALSE;
static hi_bool g_is_default_param = HI_FALSE; /* rwzb flag */

#ifdef PQ_PDM_SUPPORT
hi_disp_param g_sd_disp_param = { 0 };
hi_disp_param g_hd_disp_param = { 0 };
#endif

hi_drv_pq_vp_mode g_vp_mode_en[HI_DRV_PQ_VP_TYPE_MAX] = {
    HI_DRV_PQ_VIDEOPHONE_MODE_RECOMMEND,
    HI_DRV_PQ_VIDEOPHONE_MODE_RECOMMEND,
    HI_DRV_PQ_VIDEOPHONE_MODE_RECOMMEND,
};
/* handle num limit value , pq module can both be the limit value */
hi_ulong g_proc_intf_time_cnt[PQ_INTF_MODE_MAX] = {0};

#ifdef PQ_PDM_SUPPORT
static hi_s32 drv_pq_get_flash_pq_bin(pq_bin_param *pq_param);
static hi_s32 drv_pq_get_pic_setting(hi_void);
#endif

pq_export_func g_pq_export_funcs = {
    .pq_init = hi_drv_pq_init,
    .pq_deinit = hi_drv_pq_deinit,
    .pq_suspend = hi_drv_pq_suspend,
    .pq_resume = hi_drv_pq_resume,
    .pq_update_vdp_stt_info = hi_drv_pq_update_vdp_stt_info,
    .pq_update_vdp_alg = hi_drv_pq_update_vdp_alg,
    .pq_get_vdp_csc_coef = hi_drv_pq_get_vdp_csc_coef,
    .pq_get_vdp_zme_strategy = hi_drv_pq_get_vdp_zme_strategy,
    .pq_get_vdp_zme_coef = hi_drv_pq_get_vdp_zme_coef,
    .pq_set_vdp_hdr_cfg = hi_drv_pq_set_vdp_hdr_cfg,
    .pq_set_default_param = hi_drv_pq_set_default_param,
    .pq_update_vdp_ai_result = hi_drv_pq_update_vdp_ai_result,
    .pq_init_vpss_alg = hi_drv_pq_init_vpss_alg,
    .pq_update_vpss_stt_info = hi_drv_pq_update_vpss_stt_info,
    .pq_update_vpss_alg = hi_drv_pq_update_vpss_alg,
    .pq_get_vpss_zme_coef = hi_drv_pq_get_vpsszme_coef,
    .pq_set_vpss_hdr_cfg = hi_drv_pq_set_vpss_hdr_cfg,
    .pq_update_vpss_ai_result = hi_drv_pq_update_vpss_ai_result,
    .pq_get_gfx_csc_coef = hi_drv_pq_get_gfx_csc_coef,
    .pq_get_gfx_zme_strategy = hi_drv_pq_get_gfx_zme_strategy,
    .pq_set_gfx_hdr_cfg = hi_drv_pq_set_gfx_hdr_cfg,
    .pq_get_hwc_hdr_cfg = hi_drv_pq_get_hwc_hdr_cfg,
};

#define DRV_PQ_LOCK(irq_flag)                              \
    do { \
        osal_spin_lock_irqsave(&g_pq_status_lock, irq_flag); \
    } while (0)

#define DRV_PQ_UNLOCK(irq_flag)  \
    do {                 \
        osal_spin_unlock_irqrestore(&g_pq_status_lock, irq_flag);\
    } while (0)

static hi_u64 hi_pq_sys_get_time_stamp_us(hi_void)
{
    hi_u64 time_now;

    time_now = osal_sched_clock();

    time_now = osal_div_u64(time_now, 1000); /* 1000: div */

    return time_now;
}

hi_void drv_pq_unreister_vdp_alg(hi_void)
{
#ifdef PQ_ALG_ZME
    pq_mng_unregister_zme();
#endif

#ifdef PQ_ALG_SR
    pq_mng_unregister_sr();
#endif

#ifdef PQ_ALG_CSC
    pq_mng_unregister_csc();
#endif

#ifdef PQ_ALG_GFXCSC
    pq_mng_unregister_gfx_csc();
#endif

#ifdef PQ_ALG_GFXZME
    pq_mng_unregister_gfx_zme();
#endif

#ifdef PQ_ALG_GFXHDR
    pq_mng_unregister_gfx_hdr();
#endif

#ifdef PQ_ALG_SHARPEN
    pq_mng_unregister_sharp();
#endif

#ifdef PQ_ALG_CLE
    pq_mng_unregister_cle();
#endif

#ifdef PQ_ALG_ACM
    pq_mng_unregister_acm();
#endif

#ifdef PQ_ALG_DCI
    pq_mng_unregister_dci();
#endif

#ifdef PQ_ALG_DECONTOUR
    pq_mng_unregister_decontour();
#endif

#ifdef PQ_ALG_HDR
    pq_mng_unregister_hdr();
#endif
}

hi_void drv_pq_unreister_vpss_alg(hi_void)
{
#ifdef PQ_ALG_DEI
    pq_mng_unregister_dei();
#endif

#ifdef PQ_ALG_DB
    pq_mng_unregister_db();
#endif

#ifdef PQ_ALG_DM
    pq_mng_unregister_dm();
#endif

#ifdef PQ_ALG_DM_MZ
    pq_mng_unregister_dm_mz();
#endif

#ifdef PQ_ALG_DERING
    pq_mng_unregister_dr();
#endif

#ifdef PQ_ALG_DESHOOT
    pq_mng_unregister_ds();
#endif

#ifdef PQ_ALG_TNR
    pq_mng_unregister_tnr();
#endif

#ifdef PQ_ALG_SNR
    pq_mng_unregister_snr();
#endif

#ifdef PQ_ALG_FMD
    pq_mng_unregister_fmd();
#endif

#ifdef PQ_ALG_VPSSCSC
    pq_mng_unregister_vpss_csc();
#endif

#ifdef PQ_ALG_VPSSZME
    pq_mng_unregister_vpsszme();
#endif

#ifdef PQ_ALG_VPSSHDR
    pq_mng_unregister_vpss_hdr();
#endif

#ifdef PQ_ALG_COCO
    pq_mng_unregister_coco();
#endif
}

hi_void drv_pq_reister_vdp_alg(hi_void)
{
#ifdef PQ_ALG_ZME
    pq_mng_register_zme(REG_TYPE_VDP);
#endif

#ifdef PQ_ALG_SR
    pq_mng_register_sr();
#endif

#ifdef PQ_ALG_CSC
    pq_mng_register_csc(REG_TYPE_VDP);
#endif

#ifdef PQ_ALG_GFXCSC
    pq_mng_register_gfx_csc();
#endif

#ifdef PQ_ALG_GFXZME
    pq_mng_register_gfx_zme();
#endif

#ifdef PQ_ALG_GFXHDR
    pq_mng_register_gfx_hdr(REG_TYPE_VDP);
#endif

#ifdef PQ_ALG_SHARPEN
    pq_mng_register_sharp(REG_TYPE_VDP);
#endif

#ifdef PQ_ALG_CLE
    pq_mng_register_cle();
#endif

#ifdef PQ_ALG_ACM
    pq_mng_register_acm(REG_TYPE_VDP);
#endif

#ifdef PQ_ALG_DCI
    pq_mng_register_dci();
#endif

#ifdef PQ_ALG_DECONTOUR
    pq_mng_register_decontour();
#endif

#ifdef PQ_ALG_HDR
    pq_mng_register_hdr(REG_TYPE_VDP);
#endif
}

hi_void drv_pq_reister_vpss_alg(hi_void)
{
#ifdef PQ_ALG_DEI
    pq_mng_register_dei(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_DB
    pq_mng_register_db();
#endif

#ifdef PQ_ALG_DM
    pq_mng_register_dm(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_DM_MZ
    pq_mng_register_dm_mz();
#endif

#ifdef PQ_ALG_DERING
    pq_mng_register_dr(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_DESHOOT
    pq_mng_register_ds(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_TNR
    pq_mng_register_tnr(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_SNR
    pq_mng_register_snr(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_FMD
    pq_mng_register_fmd(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_VPSSCSC
    pq_mng_register_vpss_csc();
#endif

#ifdef PQ_ALG_VPSSZME
    pq_mng_register_vpsszme();
#endif

#ifdef PQ_ALG_VPSSHDR
    pq_mng_register_vpss_hdr(REG_TYPE_VPSS);
#endif

#ifdef PQ_ALG_COCO
    pq_mng_register_coco();
#endif
}

hi_s32 drv_pq_alloc_bin_buff(drv_pq_mem_info *mem_info)
{
    drv_pq_mem_attr attr = { 0 };
    hi_s32 ret;

    attr.name = "PQ_FLASH_BIN";
    attr.size = sizeof(pq_bin_param);
    attr.mode = OSAL_NSSMMU_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, mem_info);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_comm_mem_flush(mem_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_comm_mem_flush error!\n");
        if (mem_info->vir_addr != HI_NULL) {
            pq_comm_mem_free(mem_info);
            mem_info->vir_addr = HI_NULL;
            mem_info->phy_addr = 0;
        }
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void drv_pq_set_video_default(hi_bool default_code)
{
    hi_pq_image_param sd_video_setting = {0};
    hi_pq_image_param hd_video_setting = {0};

    if (default_code == HI_TRUE) {
        sd_video_setting.brightness = PQ_CSC_VIDEO_DEFAULT;
        sd_video_setting.contrast = PQ_CSC_VIDEO_DEFAULT;
        sd_video_setting.hue = PQ_CSC_VIDEO_DEFAULT;
        sd_video_setting.wcg_temperature = PQ_CSC_VIDEO_DEFAULT;
        sd_video_setting.saturation = PQ_CSC_VIDEO_DEFAULT;
        hd_video_setting.brightness = PQ_CSC_VIDEO_DEFAULT;
        hd_video_setting.contrast = PQ_CSC_VIDEO_DEFAULT;
        hd_video_setting.hue = PQ_CSC_VIDEO_DEFAULT;
        hd_video_setting.wcg_temperature = PQ_CSC_VIDEO_DEFAULT;
        hd_video_setting.saturation = PQ_CSC_VIDEO_DEFAULT;
    } else {
        sd_video_setting.brightness = g_pq_param.sd_video_setting.brightness;
        sd_video_setting.contrast = g_pq_param.sd_video_setting.contrast;
        sd_video_setting.hue = g_pq_param.sd_video_setting.hue;
        sd_video_setting.wcg_temperature = g_pq_param.sd_video_setting.hue;
        sd_video_setting.saturation = g_pq_param.sd_video_setting.saturation;
        hd_video_setting.brightness = g_pq_param.hd_video_setting.brightness;
        hd_video_setting.contrast = g_pq_param.hd_video_setting.contrast;
        hd_video_setting.hue = g_pq_param.hd_video_setting.hue;
        hd_video_setting.wcg_temperature = g_pq_param.hd_video_setting.hue;
        hd_video_setting.saturation = g_pq_param.hd_video_setting.saturation;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(HI_PQ_DISPLAY_0, &sd_video_setting);
        GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(HI_PQ_DISPLAY_1, &hd_video_setting);
    }
}

static hi_void drv_pq_set_picture_default(hi_bool default_code)
{
    hi_pq_image_param sd_picture_setting = {0};
    hi_pq_image_param hd_picture_setting = {0};

    if (default_code == HI_TRUE) {
        sd_picture_setting.brightness = PQ_CSC_PICTURE_DEFAULT;
        sd_picture_setting.contrast = PQ_CSC_PICTURE_DEFAULT;
        sd_picture_setting.hue = PQ_CSC_PICTURE_DEFAULT;
        sd_picture_setting.wcg_temperature = PQ_CSC_PICTURE_DEFAULT;
        sd_picture_setting.saturation = PQ_CSC_PICTURE_DEFAULT;
        hd_picture_setting.brightness = PQ_CSC_PICTURE_DEFAULT;
        hd_picture_setting.contrast = PQ_CSC_PICTURE_DEFAULT;
        hd_picture_setting.hue = PQ_CSC_PICTURE_DEFAULT;
        hd_picture_setting.wcg_temperature = PQ_CSC_PICTURE_DEFAULT;
        hd_picture_setting.saturation = PQ_CSC_PICTURE_DEFAULT;
    } else {
        sd_picture_setting.brightness = g_pq_param.sd_picture_setting.brightness;
        sd_picture_setting.contrast = g_pq_param.sd_picture_setting.contrast;
        sd_picture_setting.hue = g_pq_param.sd_picture_setting.hue;
        sd_picture_setting.wcg_temperature = g_pq_param.sd_picture_setting.hue;
        sd_picture_setting.saturation = g_pq_param.sd_picture_setting.saturation;
        hd_picture_setting.brightness = g_pq_param.hd_picture_setting.brightness;
        hd_picture_setting.contrast = g_pq_param.hd_picture_setting.contrast;
        hd_picture_setting.hue = g_pq_param.hd_picture_setting.hue;
        hd_picture_setting.wcg_temperature = g_pq_param.hd_picture_setting.hue;
        hd_picture_setting.saturation = g_pq_param.hd_picture_setting.saturation;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_0, &sd_picture_setting);
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_1, &hd_picture_setting);
    }
}

hi_void hi_drv_pq_special_deinit(hi_void)
{
    hi_u32 i;

#ifdef PQ_SOFTWARE_ALG_SUPPORT
    if (g_pq_init_flag.is_vpss_thread_init == HI_TRUE) {
        drv_pq_deinit_vpss_thread();
        g_pq_init_flag.is_vpss_thread_init = HI_FALSE;
    }

    if (g_pq_init_flag.is_vdp_thread_init == HI_TRUE) {
        drv_pq_deinit_vdp_thread();
        g_pq_init_flag.is_vdp_thread_init = HI_FALSE;
    }
#endif

    if (g_pq_init_flag.is_alg_init == HI_TRUE) {
        for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
            if (PQ_FUNC_CALL(i, deinit)) {
                GET_ALG_FUN(i)->deinit();
            }
        }
        g_pq_init_flag.is_alg_init = HI_FALSE;
    }

    if (g_pq_init_flag.is_vdp_alg_stt_init == HI_TRUE) {
        pq_comm_vdp_stt_deinit();
        g_pq_init_flag.is_vdp_alg_stt_init = HI_FALSE;
    }

    if (g_pq_init_flag.is_table_init == HI_TRUE) {
        pq_table_deinit_pqtable();
        g_pq_init_flag.is_table_init = HI_FALSE;
    }

    return;
}

hi_void hi_drv_pq_base_deinit(hi_void)
{
    if (g_pq_init_flag.is_comm_init == HI_TRUE) {
        pq_comm_deinit();
        g_pq_init_flag.is_comm_init = HI_FALSE;
    }

    if (g_pq_init_flag.is_vpss_alg_register == HI_TRUE) {
        drv_pq_unreister_vpss_alg();
        g_pq_init_flag.is_vpss_alg_register = HI_FALSE;
    }

    if (g_pq_init_flag.is_vdp_alg_register == HI_TRUE) {
        drv_pq_unreister_vdp_alg();
        g_pq_init_flag.is_vdp_alg_register = HI_FALSE;
    }

    if (g_pq_init_flag.is_pqbin_init == HI_TRUE) {
        if (g_pq_bin_buf.vir_addr != HI_NULL) {
            pq_comm_mem_free(&g_pq_bin_buf);
            g_pq_bin_buf.vir_addr = HI_NULL;
        }
        g_pq_init_flag.is_pqbin_init = HI_FALSE;
    }

    if (g_pq_init_flag.is_spinlock_init == HI_TRUE) {
        osal_spin_lock_destory(&g_pq_status_lock);
        g_pq_init_flag.is_spinlock_init = HI_FALSE;
    }

    if (g_pq_init_flag.is_pq_register == HI_TRUE) {
        osal_exportfunc_unregister(HI_ID_PQ);
        g_pq_init_flag.is_pq_register = HI_FALSE;
    }

    return;
}

hi_s32 hi_drv_pq_deinit(hi_void)
{
    /* attion return value */
    if ((g_pq_init_flag.is_pq_module_init == HI_FALSE) || (g_pq_init_count > 1)) {
        g_pq_init_count--;
        return HI_SUCCESS;
    }

    hi_drv_pq_special_deinit();
    hi_drv_pq_base_deinit();

    g_load_pq_bin = HI_FALSE;
    g_pq_bin_param = HI_NULL;
    g_pq_init_flag.is_pq_module_init = HI_FALSE;
    return HI_SUCCESS;
}

hi_s32 hi_drv_pq_init(hi_void)
{
    hi_s32 ret;
    hi_u32 i = 0;
    g_load_pq_bin = HI_FALSE;
    g_pq_bin_param = HI_NULL;

    if (g_pq_init_flag.is_pq_module_init == HI_TRUE) {
        HI_WARN_PQ("PQ module is already inited!\n");
        g_pq_init_count++;
        return HI_SUCCESS;
    }

    ret = osal_exportfunc_register(HI_ID_PQ, PQ_NAME, &g_pq_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("ERR: hi_drv_module_register!\n");
        g_load_pq_bin = HI_FALSE;
        g_pq_bin_param = HI_NULL;
        g_pq_init_flag.is_pq_module_init = HI_FALSE;
        return ret;
    }
    g_pq_init_flag.is_pq_register = HI_TRUE;

    ret = osal_spin_lock_init(&g_pq_status_lock);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("init spin lock error!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_spinlock_init = HI_TRUE;

    /* boot use reserve mem, boot to kernel , pq need copy this bin to use; release after copy */
    ret = drv_pq_alloc_bin_buff(&g_pq_bin_buf);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("alloc pq bin buff error!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_pqbin_init = HI_TRUE;
    g_pq_bin_param = (pq_bin_param *)g_pq_bin_buf.vir_addr;

#ifdef PQ_PDM_SUPPORT
    ret = drv_pq_get_flash_pq_bin(g_pq_bin_param);
    if (ret != HI_SUCCESS) {
        g_load_pq_bin = HI_FALSE;
    } else {
        g_load_pq_bin = HI_TRUE;
    }
#endif

    drv_pq_reister_vdp_alg();
    g_pq_init_flag.is_vdp_alg_register = HI_TRUE;
    drv_pq_reister_vpss_alg();
    g_pq_init_flag.is_vpss_alg_register = HI_TRUE;

#ifdef PQ_PDM_SUPPORT
    ret = drv_pq_get_pic_setting();
    if (ret != HI_SUCCESS) {
        HI_WARN_PQ("ERR: drv_pq_get_pic_setting failed!\n");
    }

    drv_pq_set_video_default(HI_FALSE);
    drv_pq_set_picture_default(HI_FALSE);
#else
    g_pq_param.sd_picture_setting.brightness = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.contrast = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.hue = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.wcg_temperature = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.saturation = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.brightness = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.contrast = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.hue = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.wcg_temperature = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.saturation = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_video_setting.brightness = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.contrast = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.hue = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.wcg_temperature = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.saturation = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.brightness = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.contrast = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.hue = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.wcg_temperature = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.saturation = PQ_CSC_VIDEO_DEFAULT;

    drv_pq_set_video_default(HI_TRUE);
    drv_pq_set_picture_default(HI_TRUE);
#endif

    ret = pq_comm_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("comm init error!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_comm_init = HI_TRUE;

    /* init vdp stt function. */
    ret = pq_comm_vdp_stt_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("pq_comm_vdp_stt_init error!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_vdp_alg_stt_init = HI_TRUE;

    ret = pq_table_init_pqtable(g_pq_bin_param, !g_load_pq_bin);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("init PQ table failure!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_table_init = HI_TRUE;

    for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
        if ((GET_ALG(i) == HI_NULL) || (GET_ALG_FUN(i) == HI_NULL)) {
            continue;
        }

        if (GET_ALG_FUN(i)->init) {
            ret = GET_ALG_FUN(i)->init(g_pq_bin_param, !g_load_pq_bin);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("alg ID:%d not init\n", i);
            }
        }

        g_pq_param.demo_on_off[i] = HI_FALSE;
        g_pq_param.module_on_off[i] = HI_TRUE;
        g_pq_param.strength[i] = PQ_ALG_DEFAULT_VALUE;
    }
    g_pq_init_flag.is_alg_init = HI_TRUE;

    g_pq_param.color_enhance.color_enhance_type = HI_PQ_COLOR_ENHANCE_SPEC_COLOR_MODE;
    g_pq_param.color_enhance.color_gain.color_space_mode = HI_PQ_COLOR_MODE_RECOMMEND;
    g_pq_param.demo_mode = HI_PQ_DEMO_MODE_FIXED_L; /* default left enable */
    g_pq_status.tun_mode = PQ_TUN_NORMAL;
    g_pq_status.image_mode = PQ_IMAGE_MODE_NORMAL;

    /* set gfx  brightness/contrast/hue/saturation */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_0,
                &(g_pq_param.sd_picture_setting));
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_1,
                &(g_pq_param.hd_picture_setting));
    }

#ifdef PQ_SOFTWARE_ALG_SUPPORT
    ret = drv_pq_init_vdp_thread();
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("drv_pq_init_vdp_thread failure!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_vdp_thread_init = HI_TRUE;

    ret = drv_pq_init_vpss_thread();
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("drv_pq_init_vpss_thread failure!\n");
        hi_drv_pq_deinit();
        return HI_FAILURE;
    }
    g_pq_init_flag.is_vpss_thread_init = HI_TRUE;
#endif

    g_pq_init_flag.is_pq_module_init = HI_TRUE;
    g_pq_init_count++;
    return HI_SUCCESS;
}

hi_s32 hi_drv_pq_suspend(hi_void *pdev)
{
    HI_PRINT("pq suspend ok \n");

    return HI_SUCCESS;
}

hi_s32 hi_drv_pq_resume(hi_void *pdev)
{
    hi_s32 ret = HI_SUCCESS;

    if (g_is_fast_boot == HI_TRUE) {
#ifdef PQ_PDM_SUPPORT
        ret = drv_pq_get_pic_setting();
        if (ret != HI_SUCCESS) {
            HI_WARN_PQ("ERR: drv_pq_get_pic_setting failed!\n");
        }
#else
    g_pq_param.sd_picture_setting.brightness = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.contrast = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.hue = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.wcg_temperature = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_picture_setting.saturation = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.brightness = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.contrast = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.hue = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.wcg_temperature = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.hd_picture_setting.saturation = PQ_CSC_PICTURE_DEFAULT;
    g_pq_param.sd_video_setting.brightness = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.contrast = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.hue = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.wcg_temperature = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.sd_video_setting.saturation = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.brightness = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.contrast = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.hue = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.wcg_temperature = PQ_CSC_VIDEO_DEFAULT;
    g_pq_param.hd_video_setting.saturation = PQ_CSC_VIDEO_DEFAULT;
#endif
    }

    /* set gfx  brightness/contrast/hue/saturation */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_0,
                &(g_pq_param.sd_picture_setting));
        GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_1,
                &(g_pq_param.hd_picture_setting));
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, set_resume)) {
        GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->set_resume(!g_load_pq_bin);
    }

    HI_PRINT("pq resume ok \n");
    return ret;
}

hi_void drv_pq_check_video_update(hi_pq_display pq_display, hi_pq_image_param video_setting)
{
    if (pq_display == HI_PQ_DISPLAY_0) {
        if ((g_pq_param.sd_video_setting.brightness != video_setting.brightness) ||
            (g_pq_param.sd_video_setting.contrast != video_setting.contrast) ||
            (g_pq_param.sd_video_setting.hue != video_setting.hue) ||
            (g_pq_param.sd_video_setting.saturation != video_setting.saturation) ||
            (g_pq_param.sd_video_setting.wcg_temperature != video_setting.wcg_temperature)) {
            g_sd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_TRUE;
            g_sd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_TRUE;
        } else {
            g_sd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_FALSE;
            g_sd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_FALSE;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        if ((g_pq_param.hd_video_setting.brightness != video_setting.brightness) ||
            (g_pq_param.hd_video_setting.contrast != video_setting.contrast) ||
            (g_pq_param.hd_video_setting.hue != video_setting.hue) ||
            (g_pq_param.hd_video_setting.saturation != video_setting.saturation) ||
            (g_pq_param.hd_video_setting.wcg_temperature != video_setting.wcg_temperature)) {
            g_hd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_TRUE;
            g_hd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_TRUE;
        } else {
            g_hd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_FALSE;
            g_hd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_FALSE;
        }
    }
}

hi_void drv_pq_check_picture_update(hi_pq_display pq_display, hi_pq_image_param picture_setting)
{
    if (pq_display == HI_PQ_DISPLAY_0) {
        if ((g_pq_param.sd_picture_setting.brightness != picture_setting.brightness) ||
            (g_pq_param.sd_picture_setting.contrast != picture_setting.contrast) ||
            (g_pq_param.sd_picture_setting.hue != picture_setting.hue) ||
            (g_pq_param.sd_picture_setting.saturation != picture_setting.saturation) ||
            (g_pq_param.sd_picture_setting.wcg_temperature != picture_setting.wcg_temperature)) {
            g_sd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_TRUE;
            g_sd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_TRUE;
        } else {
            g_sd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_FALSE;
            g_sd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_FALSE;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        if ((g_pq_param.hd_picture_setting.brightness != picture_setting.brightness) ||
            (g_pq_param.hd_picture_setting.contrast != picture_setting.contrast) ||
            (g_pq_param.hd_picture_setting.hue != picture_setting.hue) ||
            (g_pq_param.hd_picture_setting.saturation != picture_setting.saturation) ||
            (g_pq_param.hd_picture_setting.wcg_temperature != picture_setting.wcg_temperature)) {
            g_hd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_TRUE;
            g_hd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_TRUE;
        } else {
            g_hd_update_flag[HI_PQ_MODULE_GFXCSC][PQ_UPDATE_INFT] = HI_FALSE;
            g_hd_update_flag[HI_PQ_MODULE_GFXHDR][PQ_UPDATE_INFT] = HI_FALSE;
        }
    }

    return;
}

hi_s32 drv_pq_get_brightness(hi_pq_display pq_display, hi_u32 *brightness)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(brightness);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &picture_setting);
        if (ret == HI_SUCCESS) {
            *brightness = NUM2LEVEL(picture_setting.brightness);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }
    PQPRINT(PQ_PRN_CSC, "get type %d brightness: %d\n", pq_display, *brightness);
    return ret;
}

hi_s32 drv_pq_set_video_brightness(hi_pq_display pq_display, hi_u32 brightness)
{
    hi_pq_image_param video_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(brightness, PQ_ALG_MAX_VALUE);

    /* video */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    video_setting.brightness = LEVEL2NUM(brightness);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_video_update(pq_display, video_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_video_setting.brightness = LEVEL2NUM(brightness);
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_video_setting.brightness = LEVEL2NUM(brightness);
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* HDR CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_csc_video_setting(&video_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_picture_brightness(hi_pq_display pq_display, hi_u32 brightness)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(brightness, PQ_ALG_MAX_VALUE);

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    picture_setting.brightness = brightness;
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_picture_update(pq_display, picture_setting);

        if (pq_display == HI_PQ_DISPLAY_0) {
                g_pq_param.sd_picture_setting.brightness = (hi_u16)brightness;
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
                g_pq_param.hd_picture_setting.brightness = (hi_u16)brightness;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* gfx_hdr CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_csc_video_setting(&picture_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_brightness(hi_pq_display pq_display, hi_u32 brightness)
{
    hi_s32 ret;

    ret = drv_pq_set_video_brightness(pq_display, brightness);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_picture_brightness(pq_display, brightness);

    PQPRINT(PQ_PRN_CSC, "set type %d brightness: %d\n", pq_display, brightness);

    return ret;
}

hi_s32 drv_pq_get_contrast(hi_pq_display pq_display, hi_u32 *contrast)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(contrast);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &picture_setting);
        if (ret == HI_SUCCESS) {
            *contrast = NUM2LEVEL(picture_setting.contrast);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }
    PQPRINT(PQ_PRN_CSC, "get type %d contrast: %d\n", pq_display, *contrast);

    return ret;
}

hi_s32 drv_pq_set_video_contrast(hi_pq_display pq_display, hi_u32 contrast)
{
    hi_pq_image_param video_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(contrast, PQ_ALG_MAX_VALUE);

    /* video */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    video_setting.contrast = LEVEL2NUM(contrast);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_video_update(pq_display, video_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_video_setting.contrast = LEVEL2NUM(contrast);
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_video_setting.contrast = LEVEL2NUM(contrast);
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* HDR CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_csc_video_setting(&video_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_picture_contrast(hi_pq_display pq_display, hi_u32 contrast)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(contrast, PQ_ALG_MAX_VALUE);

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    picture_setting.contrast = contrast;

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_picture_update(pq_display, picture_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_picture_setting.contrast = (hi_u16)contrast;
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_picture_setting.contrast = (hi_u16)contrast;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* gfx_hdr CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_csc_video_setting(&picture_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_contrast(hi_pq_display pq_display, hi_u32 contrast)
{
    hi_s32 ret;

    ret = drv_pq_set_video_contrast(pq_display, contrast);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_picture_contrast(pq_display, contrast);

    PQPRINT(PQ_PRN_CSC, "set type %d contrast: %d\n", pq_display, contrast);

    return ret;
}

hi_s32 drv_pq_get_hue(hi_pq_display pq_display, hi_u32 *hue)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(hue);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &picture_setting);
        if (ret == HI_SUCCESS) {
            *hue = NUM2LEVEL(picture_setting.hue);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }

    PQPRINT(PQ_PRN_CSC, "get type %d hue: %d\n", pq_display, *hue);

    return ret;
}

hi_s32 drv_pq_set_video_hue(hi_pq_display pq_display, hi_u32 hue)
{
    hi_pq_image_param video_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hue, PQ_ALG_MAX_VALUE);

    /* video CSC */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    video_setting.hue = LEVEL2NUM(hue);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_video_update(pq_display, video_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_video_setting.hue = LEVEL2NUM(hue);
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_video_setting.hue = LEVEL2NUM(hue);
        }
    }
    if (pq_display == HI_PQ_DISPLAY_1) {
        /* HDR CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_csc_video_setting(&video_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}
hi_s32 drv_pq_set_picture_hue(hi_pq_display pq_display, hi_u32 hue)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hue, PQ_ALG_MAX_VALUE);

   /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    picture_setting.hue = hue;
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_picture_update(pq_display, picture_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_picture_setting.hue = (hi_u16)hue;
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_picture_setting.hue = (hi_u16)hue;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* gfx_hdr CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_csc_video_setting(&picture_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_hue(hi_pq_display pq_display, hi_u32 hue)
{
    hi_s32 ret;

    ret = drv_pq_set_video_hue(pq_display, hue);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_picture_hue(pq_display, hue);

    PQPRINT(PQ_PRN_CSC, "set type %d hue: %d\n", pq_display, hue);

    return ret;
}

hi_s32 drv_pq_get_wcg_temperature(hi_pq_display pq_display, hi_u32 *wcg_temperature)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(wcg_temperature);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &picture_setting);
        if (ret == HI_SUCCESS) {
            *wcg_temperature = NUM2LEVEL(picture_setting.wcg_temperature);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }

    PQPRINT(PQ_PRN_CSC, "get type %d wcg_temperature: %d\n", pq_display, *wcg_temperature);

    return ret;
}

hi_s32 drv_pq_set_video_wcg_temp(hi_pq_display pq_display, hi_u32 wcg_temperature)
{
    hi_pq_image_param video_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(wcg_temperature, PQ_ALG_MAX_VALUE);

    /* video CSC */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    video_setting.wcg_temperature = LEVEL2NUM(wcg_temperature);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_video_update(pq_display, video_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_video_setting.wcg_temperature = LEVEL2NUM(wcg_temperature);
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_video_setting.wcg_temperature = LEVEL2NUM(wcg_temperature);
        }
    }
    if (pq_display == HI_PQ_DISPLAY_1) {
        /* HDR CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_csc_video_setting(&video_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_picture_wcg_temp(hi_pq_display pq_display, hi_u32 wcg_temperature)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(wcg_temperature, PQ_ALG_MAX_VALUE);

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    picture_setting.wcg_temperature = wcg_temperature;
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_picture_setting.wcg_temperature = (hi_u16)wcg_temperature;
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_picture_setting.wcg_temperature = (hi_u16)wcg_temperature;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* gfx_hdr CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_csc_video_setting(&picture_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_wcg_temperature(hi_pq_display pq_display, hi_u32 wcg_temperature)
{
    hi_s32 ret;

    ret = drv_pq_set_video_wcg_temp(pq_display, wcg_temperature);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_picture_wcg_temp(pq_display, wcg_temperature);

    PQPRINT(PQ_PRN_CSC, "set type %d wcg_temperature: %d\n", pq_display, wcg_temperature);

    return ret;
}

hi_s32 drv_pq_get_saturation(hi_pq_display pq_display, hi_u32 *saturation)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(saturation);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &picture_setting);
        if (ret == HI_SUCCESS) {
            *saturation = NUM2LEVEL(picture_setting.saturation);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }
    PQPRINT(PQ_PRN_CSC, "get type %d saturation: %d\n", pq_display, *saturation);

    return ret;
}

hi_s32 drv_pq_set_video_saturation(hi_pq_display pq_display, hi_u32 saturation)
{
    hi_pq_image_param video_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(saturation, PQ_ALG_MAX_VALUE);

    /* video CSC */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    video_setting.saturation = LEVEL2NUM(saturation);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(pq_display, &video_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_video_update(pq_display, video_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_video_setting.saturation = LEVEL2NUM(saturation);
        }

        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_video_setting.saturation = LEVEL2NUM(saturation);
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* HDR CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_csc_video_setting(&video_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_picture_saturation(hi_pq_display pq_display, hi_u32 saturation)
{
    hi_pq_image_param picture_setting = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_display, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(saturation, PQ_ALG_MAX_VALUE);

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    picture_setting.saturation = saturation;
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(pq_display, &picture_setting);
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_check_picture_update(pq_display, picture_setting);
        if (pq_display == HI_PQ_DISPLAY_0) {
            g_pq_param.sd_picture_setting.saturation = (hi_u16)saturation;
        }
        if (pq_display == HI_PQ_DISPLAY_1) {
            g_pq_param.hd_picture_setting.saturation = (hi_u16)saturation;
        }
    }

    if (pq_display == HI_PQ_DISPLAY_1) {
        /* gfx_hdr CSC */
        if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_csc_video_setting)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_csc_video_setting(&picture_setting);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    return ret;
}

hi_s32 drv_pq_set_saturation(hi_pq_display pq_display, hi_u32 saturation)
{
    hi_s32 ret;

    ret = drv_pq_set_video_saturation(pq_display, saturation);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_picture_saturation(pq_display, saturation);

    PQPRINT(PQ_PRN_CSC, "set type %d saturation: %d\n", pq_display, saturation);

    return ret;
}

/* get brightness, contrast, hue, saturation  of HD video */
hi_s32 drv_pq_get_hd_video_setting(hi_pq_image_param *setting)
{
    hi_pq_image_param setting_tmp = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(setting);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(HI_PQ_DISPLAY_1, &setting_tmp);
        if (ret == HI_SUCCESS) {
            setting->brightness = NUM2LEVEL(setting_tmp.brightness);
            setting->contrast = NUM2LEVEL(setting_tmp.contrast);
            setting->hue = NUM2LEVEL(setting_tmp.hue);
            setting->saturation = NUM2LEVEL(setting_tmp.saturation);
            setting->wcg_temperature = NUM2LEVEL(setting_tmp.wcg_temperature);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }

    return HI_SUCCESS;
}

/* set brightness, contrast, hue, saturation  of HD video */
hi_s32 drv_pq_set_hd_video_setting(hi_pq_image_param *setting)
{
    hi_pq_image_param setting_tmp = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(setting);

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->brightness, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->contrast, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->saturation, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->hue, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->wcg_temperature, PQ_ALG_MAX_VALUE);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    PQPRINT(PQ_PRN_CSC, "set brightness:%d, contrast:%d, hue:%d, saturation:%d\n",
            setting->brightness, setting->contrast, setting->hue, setting->saturation);

    /* 0~100 to 0~255 */
    setting_tmp.brightness = LEVEL2NUM(setting->brightness);
    setting_tmp.contrast = LEVEL2NUM(setting->contrast);
    setting_tmp.hue = LEVEL2NUM(setting->hue);
    setting_tmp.saturation = LEVEL2NUM(setting->saturation);
    setting_tmp.wcg_temperature = LEVEL2NUM(setting->wcg_temperature);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(HI_PQ_DISPLAY_1, &setting_tmp);
    }

    /* up drv layer; we use 0~100 level value */
    g_pq_param.hd_video_setting.brightness = LEVEL2NUM(setting->brightness);
    g_pq_param.hd_video_setting.contrast = LEVEL2NUM(setting->contrast);
    g_pq_param.hd_video_setting.hue = LEVEL2NUM(setting->hue);
    g_pq_param.hd_video_setting.saturation = LEVEL2NUM(setting->saturation);
    g_pq_param.hd_video_setting.wcg_temperature = LEVEL2NUM(setting->wcg_temperature);

    return ret;
}

/* get brightness, contrast, hue, saturation  of SD video */
hi_s32 drv_pq_get_sd_video_setting(hi_pq_image_param *setting)
{
    hi_pq_image_param setting_tmp = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(setting);

    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_video_setting(HI_PQ_DISPLAY_0, &setting_tmp);
        if (ret == HI_SUCCESS) {
            setting->brightness = NUM2LEVEL(setting_tmp.brightness);
            setting->contrast = NUM2LEVEL(setting_tmp.contrast);
            setting->hue = NUM2LEVEL(setting_tmp.hue);
            setting->saturation = NUM2LEVEL(setting_tmp.saturation);
            setting->wcg_temperature = NUM2LEVEL(setting_tmp.wcg_temperature);
        } else {
            HI_ERR_PQ("get csc error\n");
        }
    }

    return ret;
}

/* set brightness, contrast, hue, saturation  of SD video */
hi_s32 drv_pq_set_sd_video_setting(hi_pq_image_param *setting)
{
    hi_pq_image_param setting_tmp = {0};
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(setting);

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->brightness, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->contrast, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->saturation, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->hue, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->wcg_temperature, PQ_ALG_MAX_VALUE);

    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    PQPRINT(PQ_PRN_CSC, "set brightness:%d, contrast:%d, hue:%d, saturation:%d\n",
            setting->brightness, setting->contrast, setting->hue, setting->saturation);

    setting_tmp.brightness = LEVEL2NUM(setting->brightness);
    setting_tmp.contrast = LEVEL2NUM(setting->contrast);
    setting_tmp.hue = LEVEL2NUM(setting->hue);
    setting_tmp.saturation = LEVEL2NUM(setting->saturation);
    setting_tmp.wcg_temperature = LEVEL2NUM(setting->wcg_temperature);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(HI_PQ_DISPLAY_0, &setting_tmp);
    }

    g_pq_param.sd_video_setting.brightness = LEVEL2NUM(setting->brightness);
    g_pq_param.sd_video_setting.contrast = LEVEL2NUM(setting->contrast);
    g_pq_param.sd_video_setting.hue = LEVEL2NUM(setting->hue);
    g_pq_param.sd_video_setting.saturation = LEVEL2NUM(setting->saturation);
    g_pq_param.sd_video_setting.wcg_temperature = LEVEL2NUM(setting->wcg_temperature);

    return ret;
}

/* get brightness, contrast, hue, saturation  of HD gfx */
hi_s32 drv_pq_get_hd_picture_setting(hi_pq_image_param *setting)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(setting);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    setting->brightness = g_pq_param.hd_picture_setting.brightness;
    setting->contrast = g_pq_param.hd_picture_setting.contrast;
    setting->hue = g_pq_param.hd_picture_setting.hue;
    setting->saturation = g_pq_param.hd_picture_setting.saturation;
    setting->wcg_temperature = g_pq_param.hd_picture_setting.wcg_temperature;

    return HI_SUCCESS;
}

/* get brightness, contrast, hue, saturation  of SD gfx */
hi_s32 drv_pq_get_sd_picture_setting(hi_pq_image_param *setting)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(setting);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    setting->brightness = g_pq_param.sd_picture_setting.brightness;
    setting->contrast = g_pq_param.sd_picture_setting.contrast;
    setting->hue = g_pq_param.sd_picture_setting.hue;
    setting->saturation = g_pq_param.sd_picture_setting.saturation;
    setting->wcg_temperature = g_pq_param.sd_picture_setting.wcg_temperature;

    return HI_SUCCESS;
}

/* set brightness, contrast, hue, saturation  of HD gfx */
hi_s32 drv_pq_set_hd_picture_setting(hi_pq_image_param *setting)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(setting);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->brightness, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->contrast, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->saturation, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->hue, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->wcg_temperature, PQ_ALG_MAX_VALUE);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_1, setting);
    }

    g_pq_param.hd_picture_setting.brightness = setting->brightness;
    g_pq_param.hd_picture_setting.contrast = setting->contrast;
    g_pq_param.hd_picture_setting.hue = setting->hue;
    g_pq_param.hd_picture_setting.saturation = setting->saturation;
    g_pq_param.hd_picture_setting.wcg_temperature = setting->wcg_temperature;

    HI_DEBUG_PQ("set_hd_picture brightness:%u, contrast:%u, hue:%u, saturation:%u, wcg_tempe: %u \n",
                setting->brightness, setting->contrast, setting->hue, setting->saturation, setting->wcg_temperature);

    return ret;
}

hi_s32 drv_pq_set_hdr_offset_para(hi_pq_hdr_offset *hdr_offset_para)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    PQ_CHECK_OVER_RANGE_RE_FAIL(hdr_offset_para->hdr_process_scene, HI_PQ_HDR_MODE_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->hdr_mode, 1);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->bright, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->contrast, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->satu, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->hue, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->r, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->g, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->b, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->dark_cv, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->bright_cv, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->ac_cdark, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->ac_cbrigt, PQ_ALG_MAX_VALUE);

    hdr_offset_para->bright = LEVEL2NUM(hdr_offset_para->bright);
    hdr_offset_para->contrast = LEVEL2NUM(hdr_offset_para->contrast);
    hdr_offset_para->satu = LEVEL2NUM(hdr_offset_para->satu);
    hdr_offset_para->hue = LEVEL2NUM(hdr_offset_para->hue);

    hdr_offset_para->r = LEVEL2NUM(hdr_offset_para->r);
    hdr_offset_para->g = LEVEL2NUM(hdr_offset_para->g);
    hdr_offset_para->b = LEVEL2NUM(hdr_offset_para->b);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_offset)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_offset(hdr_offset_para);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_hdr_tm_curve)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_hdr_tm_curve(hdr_offset_para);
    }

    return ret;
}

hi_s32 drv_pq_set_gfx_hdr_offset_para(hi_pq_hdr_offset *hdr_offset_para)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    PQ_CHECK_OVER_RANGE_RE_FAIL(hdr_offset_para->hdr_process_scene, HI_PQ_HDR_MODE_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->hdr_mode, 1);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->bright, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->contrast, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->satu, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->hue, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->r, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->g, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->b, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->dark_cv, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->bright_cv, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->ac_cdark, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(hdr_offset_para->ac_cbrigt, PQ_ALG_MAX_VALUE);

    hdr_offset_para->bright = LEVEL2NUM(hdr_offset_para->bright);
    hdr_offset_para->contrast = LEVEL2NUM(hdr_offset_para->contrast);
    hdr_offset_para->satu = LEVEL2NUM(hdr_offset_para->satu);
    hdr_offset_para->hue = LEVEL2NUM(hdr_offset_para->hue);

    hdr_offset_para->r = LEVEL2NUM(hdr_offset_para->r);
    hdr_offset_para->g = LEVEL2NUM(hdr_offset_para->g);
    hdr_offset_para->b = LEVEL2NUM(hdr_offset_para->b);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_offset)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_offset(hdr_offset_para);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_hdr_tm_curve)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_hdr_tm_curve(hdr_offset_para);
    }

    return ret;
}

/* get brightness, contrast, hue, saturation  of SD gfx */
hi_s32 drv_pq_set_sd_picture_setting(hi_pq_image_param *setting)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(setting);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->brightness, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->contrast, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->saturation, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->hue, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(setting->wcg_temperature, PQ_ALG_MAX_VALUE);

    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_gfx_csc_picture_setting)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_gfx_csc_picture_setting(HI_PQ_DISPLAY_0, setting);
    }

    g_pq_param.sd_picture_setting.brightness = setting->brightness;
    g_pq_param.sd_picture_setting.contrast = setting->contrast;
    g_pq_param.sd_picture_setting.hue = setting->hue;
    g_pq_param.sd_picture_setting.saturation = setting->saturation;
    g_pq_param.sd_picture_setting.wcg_temperature = setting->wcg_temperature;

    return ret;
}

hi_s32 drv_pq_set_temperature(hi_pq_display disp, hi_pq_color_temperature *temperature)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(temperature);
    PQ_CHECK_OVER_RANGE_RE_FAIL(disp, HI_PQ_DISPLAY_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(temperature->red_gain, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(temperature->green_gain, PQ_ALG_MAX_VALUE);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(temperature->blue_gain, PQ_ALG_MAX_VALUE);

    /* video */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_color_temp)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_color_temp(disp, temperature);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* gfx */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, set_color_temp)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->set_color_temp(disp, temperature);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    return ret;
}

hi_s32 drv_pq_get_temperature(hi_pq_display disp, hi_pq_color_temperature *temperature)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(temperature);
    PQ_CHECK_OVER_RANGE_RE_FAIL(disp, HI_PQ_DISPLAY_MAX);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_color_temp)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_color_temp(disp, temperature);
    }

    return ret;
}

hi_s32 drv_pq_get_sharpness(hi_u32 *sharpness)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(sharpness);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_SHARPNESS, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->get_strength(sharpness);
        PQPRINT(PQ_PRN_SHARPEN, "get sharpen level: %d\n", *sharpness);
    }

    return ret;
}

hi_s32 drv_pq_set_sharpness(hi_u32 sharpness)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(sharpness, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_SHARPNESS, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->set_strength(sharpness);
        g_pq_param.strength[HI_PQ_MODULE_SHARPNESS] = sharpness;
        PQPRINT(PQ_PRN_SHARPEN, "set sharpen level: %d\n", sharpness);
    }

    /* set cle strength. */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CLE, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->set_strength(sharpness);
        g_pq_param.strength[HI_PQ_MODULE_CLE] = sharpness;
        PQPRINT(HI_PQ_MODULE_CLE, "set cle level: %d\n", sharpness);
    }

    return ret;
}

hi_s32 drv_pq_get_color_ehance(hi_u32 *color_gain_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(color_gain_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->get_strength(color_gain_level);
        PQPRINT(PQ_PRN_ACM, "get color_gain_level: %d\n", *color_gain_level);
    }

    return ret;
}

hi_s32 drv_pq_set_color_ehance(hi_u32 color_gain_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(color_gain_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->set_strength(color_gain_level);
        g_pq_param.strength[HI_PQ_MODULE_ACM] = color_gain_level;
        PQPRINT(PQ_PRN_ACM, "set color_gain_level: %d\n", color_gain_level);
    }

    return ret;
}

hi_s32 drv_pq_get_flesh_tone_level(hi_u32 *flesh_tone_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(flesh_tone_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, get_flesh_tone_level)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->get_flesh_tone_level(flesh_tone_level);
        PQPRINT(PQ_PRN_ACM, "get flesh_tone_level: %d\n", *flesh_tone_level);
    }

    return ret;
}

hi_s32 drv_pq_set_flesh_tone_level(hi_pq_fleshtone flesh_tone_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(flesh_tone_level, HI_PQ_FLESHTONE_GAIN_MAX);

    g_pq_param.color_enhance.color_enhance_type = HI_PQ_COLOR_ENHANCE_FLESHTONE;
    g_pq_param.color_enhance.color_gain.fleshtone_gain = flesh_tone_level;

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, set_flesh_tone_level)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->set_flesh_tone_level(flesh_tone_level);
        PQPRINT(PQ_PRN_ACM, "set flesh_tone_level: %d\n", flesh_tone_level);
    }

    return ret;
}

hi_s32 drv_pq_set_six_base_color(hi_pq_six_base_color *color_offset)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(color_offset);

    g_pq_param.color_enhance.color_enhance_type = HI_PQ_COLOR_ENHANCE_SIX_BASE;
    g_pq_param.color_enhance.color_gain.six_base_color = *color_offset;

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, set_six_base_color)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->set_six_base_color(color_offset);
        PQPRINT(PQ_PRN_ACM, "set six_base_color red:%d, green: %d, blue: %d, cyan: %d, magenta: %d, yellow: %d\n",
                color_offset->red, color_offset->green,
                color_offset->blue, color_offset->cyan,
                color_offset->magenta, color_offset->yellow);
    }

    return ret;
}

hi_s32 drv_pq_get_six_base_color(hi_pq_six_base_color *color_offset)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(color_offset);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, get_six_base_color)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->get_six_base_color(color_offset);
        PQPRINT(PQ_PRN_ACM, "get six_base_color red:%d, green: %d, blue: %d, cyan: %d, magenta: %d, yellow: %d\n",
                color_offset->red, color_offset->green,
                color_offset->blue, color_offset->cyan,
                color_offset->magenta, color_offset->yellow);
    }

    return ret;
}

hi_s32 drv_pq_set_color_enhance_mode(hi_pq_color_spec_mode color_spec_mode)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(color_spec_mode, HI_PQ_COLOR_MODE_MAX);

    g_pq_param.color_enhance.color_enhance_type = HI_PQ_COLOR_ENHANCE_SPEC_COLOR_MODE;
    g_pq_param.color_enhance.color_gain.color_space_mode = color_spec_mode;

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, set_color_enhance_mode)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->set_color_enhance_mode(color_spec_mode);
        PQPRINT(PQ_PRN_ACM, "set color_enhance_mode: %d\n", color_spec_mode);
    }

    return ret;
}

hi_s32 drv_pq_get_color_enhance_mode(hi_pq_color_spec_mode *color_spec_mode)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(color_spec_mode);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, get_color_enhance_mode)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->get_color_enhance_mode(color_spec_mode);
        PQPRINT(PQ_PRN_ACM, "get color_enhance_mode: %d\n", *color_spec_mode);
    }

    return ret;
}

hi_s32 drv_pq_set_dci_level(hi_u32 dci_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(dci_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_strength(dci_level);
        g_pq_param.strength[HI_PQ_MODULE_DCI] = dci_level;
        PQPRINT(PQ_PRN_DCI, "set dci_level_gain: %d\n", dci_level);
    }

    return ret;
}

hi_s32 drv_pq_get_dci_level(hi_u32 *dci_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(dci_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_strength(dci_level);
        PQPRINT(PQ_PRN_DCI, "get DCI level: %d\n", *dci_level);
    }

    return ret;
}

hi_s32 drv_pq_get_tnr_level(hi_u32 *tnr_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(tnr_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_TNR, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_TNR)->get_strength(tnr_level);
        PQPRINT(PQ_PRN_TNR, "get TNR level: %d\n", *tnr_level);
    }

    return ret;
}

hi_s32 drv_pq_set_tnr_level(hi_u32 tnr_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(tnr_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_TNR, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_TNR)->set_strength(tnr_level);
        g_pq_param.strength[HI_PQ_MODULE_TNR] = tnr_level;
        PQPRINT(PQ_PRN_TNR, "set TNR level: %d\n", tnr_level);
    }

    return ret;
}

hi_s32 drv_pq_get_snr_level(hi_u32 *snr_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(snr_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_SNR, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SNR)->get_strength(snr_level);
        PQPRINT(PQ_PRN_SNR, "get SNR level: %d\n", *snr_level);
    }

    return ret;
}

hi_s32 drv_pq_set_snr_level(hi_u32 snr_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(snr_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_SNR, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SNR)->set_strength(snr_level);
        g_pq_param.strength[HI_PQ_MODULE_SNR] = snr_level;
        PQPRINT(PQ_PRN_SNR, "set SNR level: %d\n", snr_level);
    }

    return ret;
}

hi_s32 drv_pq_get_db_level(hi_u32 *db_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(db_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DB, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DB)->get_strength(db_level);
        PQPRINT(PQ_PRN_DB_READ, "get DB level: %d\n", *db_level);
    }

    return ret;
}

hi_s32 drv_pq_set_db_level(hi_u32 db_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(db_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DB, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DB)->set_strength(db_level);
        g_pq_param.strength[HI_PQ_MODULE_DB] = db_level;
        PQPRINT(PQ_PRN_DB_READ, "set DB level: %d\n", db_level);
    }

    return ret;
}

hi_s32 drv_pq_get_dm_level(hi_u32 *dm_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(dm_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DM, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DM)->get_strength(dm_level);
        PQPRINT(PQ_PRN_DM, "get DM level: %d\n", *dm_level);
    }

    return ret;
}

hi_s32 drv_pq_set_dm_level(hi_u32 dm_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(dm_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DM, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DM)->set_strength(dm_level);
        g_pq_param.strength[HI_PQ_MODULE_DM] = dm_level;
        PQPRINT(PQ_PRN_DM, "set DM level: %d\n", dm_level);
    }

    return ret;
}

hi_s32 drv_pq_get_dc_level(hi_u32 *dc_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(dc_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DC, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DC)->get_strength(dc_level);
        PQPRINT(PQ_PRN_DC, "get decontour level: %d\n", *dc_level);
    }

    return ret;
}

hi_s32 drv_pq_set_dc_level(hi_u32 dc_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(dc_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DC, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DC)->set_strength(dc_level);
        g_pq_param.strength[HI_PQ_MODULE_DC] = dc_level;
        PQPRINT(PQ_PRN_DC, "set decontour level: %d\n", dc_level);
    }

    return ret;
}

hi_s32 drv_pq_get_coco_level(hi_u32 *coco_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(coco_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_COCO, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_COCO)->get_strength(coco_level);
        PQPRINT(PQ_PRN_COCO, "get colorcoring level: %d\n", *coco_level);
    }

    return ret;
}

hi_s32 drv_pq_set_coco_level(hi_u32 coco_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(coco_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_COCO, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_COCO)->set_strength(coco_level);
        g_pq_param.strength[HI_PQ_MODULE_COCO] = coco_level;
        PQPRINT(PQ_PRN_COCO, "set colorcoring level: %d\n", coco_level);
    }

    return ret;
}

hi_s32 drv_pq_get_acm_level(hi_u32 *acm_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(acm_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->get_strength(acm_level);
        PQPRINT(PQ_PRN_ACM, "get acm level: %d\n", *acm_level);
    }

    return ret;
}

hi_s32 drv_pq_set_acm_level(hi_u32 acm_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(acm_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->set_strength(acm_level);
        g_pq_param.strength[HI_PQ_MODULE_ACM] = acm_level;
        PQPRINT(PQ_PRN_ACM, "set acm level: %d\n", acm_level);
    }

    return ret;
}

hi_s32 drv_pq_get_dr_level(hi_u32 *dr_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(dr_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DR, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DR)->get_strength(dr_level);
    }

    return ret;
}

hi_s32 drv_pq_set_dr_level(hi_u32 dr_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(dr_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DR, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DR)->set_strength(dr_level);
        g_pq_param.strength[HI_PQ_MODULE_DR] = dr_level;
    }

    return ret;
}

hi_s32 drv_pq_get_ds_level(hi_u32 *ds_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(ds_level);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DS, get_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DS)->get_strength(ds_level);
    }

    return ret;
}

hi_s32 drv_pq_set_ds_level(hi_u32 ds_level)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(ds_level, PQ_ALG_MAX_VALUE);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DS, set_strength)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DS)->set_strength(ds_level);
        g_pq_param.strength[HI_PQ_MODULE_DS] = ds_level;
    }

    return ret;
}

hi_s32 drv_pq_get_pq_module(hi_pq_module_type module, hi_u32 *on_off)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);
    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (module == HI_PQ_MODULE_ALL) {
        *on_off = g_pq_param.module_on_off[HI_PQ_MODULE_ALL];
        ret = HI_SUCCESS;
    } else if (PQ_FUNC_CALL(module, get_enable)) {
        ret = GET_ALG_FUN(module)->get_enable(on_off);
    }

    return ret;
}

hi_s32 drv_pq_set_pq_module(hi_pq_module_type module, hi_u32 on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (module == HI_PQ_MODULE_ALL) {
        g_pq_param.module_on_off[HI_PQ_MODULE_ALL] = on_off;

        for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
            if (PQ_FUNC_CALL(i, set_enable)) {
                ret = GET_ALG_FUN(i)->set_enable(on_off);
                g_pq_param.module_on_off[i] = on_off;
            }
        }
    } else if (PQ_FUNC_CALL(module, set_enable)) {
        ret = GET_ALG_FUN(module)->set_enable(on_off);
        g_pq_param.module_on_off[module] = on_off;
    }

    return ret;
}

hi_s32 drv_pq_set_demo_en(hi_pq_module_type module, hi_bool on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (module == HI_PQ_MODULE_ALL) {
        g_pq_param.demo_on_off[HI_PQ_MODULE_ALL] = on_off;

        for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
            if (PQ_FUNC_CALL(i, set_demo)) {
                ret = GET_ALG_FUN(i)->set_demo(on_off);
                g_pq_param.demo_on_off[i] = on_off;
            }
        }
    } else if (PQ_FUNC_CALL(module, set_demo)) {
        ret = GET_ALG_FUN(module)->set_demo(on_off);
        g_pq_param.demo_on_off[module] = on_off;
    }

    return ret;
}

hi_s32 drv_pq_get_demo_en(hi_pq_module_type module, hi_bool *enable)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (module == HI_PQ_MODULE_ALL) {
        *enable = g_pq_param.demo_on_off[HI_PQ_MODULE_ALL];
    } else {
        *enable = g_pq_param.demo_on_off[module];
    }

    return ret;
}

hi_s32 drv_pq_set_debug_en(hi_pq_module_type module, hi_bool on_off)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (module == HI_PQ_MODULE_ALL) {
        g_pq_param.debug_on_off[HI_PQ_MODULE_ALL] = on_off;

        for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
            if (PQ_FUNC_CALL(i, set_debug_en)) {
                ret = GET_ALG_FUN(i)->set_debug_en(on_off);
                g_pq_param.debug_on_off[i] = on_off;
            }
        }
    } else if (PQ_FUNC_CALL(module, set_debug_en)) {
        ret = GET_ALG_FUN(module)->set_debug_en(on_off);
        g_pq_param.debug_on_off[module] = on_off;
    }

    return ret;
}

/* set demo display mode */
hi_s32 drv_pq_set_demo_disp_mode(pq_reg_type type, hi_pq_demo_mode demo_mode)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i = 0;
    pq_demo_mode set_mode = PQ_DEMO_ENABLE_L;

    PQ_CHECK_OVER_RANGE_RE_FAIL(type, REG_TYPE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(demo_mode, HI_PQ_DEMO_MODE_MAX);

    if ((demo_mode == HI_PQ_DEMO_MODE_FIXED_R) || (demo_mode == HI_PQ_DEMO_MODE_SCROLL_R)) {
        set_mode = PQ_DEMO_ENABLE_R;
    } else if ((demo_mode == HI_PQ_DEMO_MODE_FIXED_L) || (demo_mode == HI_PQ_DEMO_MODE_SCROLL_L)) {
        set_mode = PQ_DEMO_ENABLE_L;
    }

    for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
        if (PQ_FUNC_CALL(i, set_demo_mode)) {
            if (type != pq_comm_get_alg_type_id(i)) {
                continue;
            }

            ret = GET_ALG_FUN(i)->set_demo_mode(set_mode);
        }
    }

    g_pq_param.demo_mode = demo_mode;
    return ret;
}

hi_s32 drv_pq_get_demo_disp_mode(hi_pq_demo_mode *pen_demo_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(pen_demo_mode);

    *pen_demo_mode = g_pq_param.demo_mode;

    return HI_SUCCESS;
}

static hi_void drv_pq_refresh_mkt_coordinate(pq_reg_type type)
{
    /* DEMO */
    hi_u32 i = 0;
    hi_u32 step = 1;
    static hi_u32 s_u32_pos = 0;

    PQ_CHECK_OVER_RANGE_RE_NULL(type, REG_TYPE_MAX);

    if ((g_pq_param.demo_mode == HI_PQ_DEMO_MODE_FIXED_R) || (g_pq_param.demo_mode == HI_PQ_DEMO_MODE_FIXED_L)) {
        drv_pq_set_demo_coordinate(type, g_demo_coordinate);
        return;
    }

    for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
        if (HI_NULL == PQ_FUNC_CALL(i, set_demo_mode_coor)) {
            continue;
        }

        if (type != pq_comm_get_alg_type_id(i)) {
            continue;
        }

        /* set demo display mode */
        if ((g_pq_param.demo_mode == HI_PQ_DEMO_MODE_SCROLL_R) || (g_pq_param.demo_mode == HI_PQ_DEMO_MODE_SCROLL_L)) {
            if (g_image_width == 0) {
                return;
            }

            if (type == REG_TYPE_VPSS) {
                GET_ALG_FUN(i)->set_demo_mode_coor(s_u32_pos);
            } else if (type == REG_TYPE_VDP) {
                GET_ALG_FUN(i)->set_demo_mode_coor(s_u32_pos * g_out_width / g_image_width);
            }
        }
    }

    s_u32_pos = s_u32_pos + step;
    if (g_image_width < s_u32_pos) {
        s_u32_pos = 0;
    }

    return;
}

hi_s32 drv_pq_set_demo_coordinate(pq_reg_type type, hi_u32 x)
{
    hi_u32 i;

    PQ_CHECK_OVER_RANGE_RE_FAIL(type, REG_TYPE_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(x, PQ_UNF_PARA_MAX);

    g_demo_coordinate = x;

    for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
        if (HI_NULL == PQ_FUNC_CALL(i, set_demo_mode_coor)) {
            continue;
        }

        if (type != pq_comm_get_alg_type_id(i)) {
            continue;
        }

        /* set demo display mode */
        if (type == REG_TYPE_VPSS) {
            GET_ALG_FUN(i)->set_demo_mode_coor(g_image_width * x / PQ_ALG_MAX_VALUE);
        } else if (type == REG_TYPE_VDP) {
            GET_ALG_FUN(i)->set_demo_mode_coor(g_out_width * x / PQ_ALG_MAX_VALUE);
        }
    }

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_demo_coordinate(hi_u32 *x)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(x);

    *x = g_demo_coordinate;

    return HI_SUCCESS;
}

static hi_void drv_pq_refresh_vdp_cfg(hi_void)
{
    drv_pq_refresh_mkt_coordinate(REG_TYPE_VDP);

    pq_table_init_soft_table(HI_TRUE);

    return;
}

static hi_s32 drv_pq_set_module_en(hi_pq_module_type mode, hi_bool module_on_off)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_OVER_RANGE_RE_FAIL(mode, HI_PQ_MODULE_MAX);

    if (HI_NULL == PQ_FUNC_CALL(mode, set_enable)) {
        return ret;
    }

    ret = GET_ALG_FUN(mode)->set_enable(module_on_off);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("alg ID:%d, set_enable error!\n", mode);
    }

    return ret;
}

static hi_s32 drv_pq_reset_pq_status(pq_reg_type type)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i = 0;
    hi_bool module_on = HI_FALSE;

    PQ_CHECK_OVER_RANGE_RE_FAIL(type, REG_TYPE_MAX);

    for (i = 0; i < HI_PQ_MODULE_MAX; i++) {
        if (HI_NULL == GET_ALG(i)) {
            continue;
        }

        if (type != pq_comm_get_alg_type_id(i)) {
            continue;
        }

        if (GET_ALG_FUN(i)->set_enable) {
            module_on = (g_is_default_param == HI_TRUE) ? HI_FALSE : g_pq_param.module_on_off[i];

            ret = GET_ALG_FUN(i)->set_enable(module_on);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("alg ID:%d, set_enable error!\n", i);
            }
        }

        if (GET_ALG_FUN(i)->set_demo) {
            ret = GET_ALG_FUN(i)->set_demo(g_pq_param.demo_on_off[i]);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("alg ID:%d, set_demo error!\n", i);
            }
        }

        if (GET_ALG_FUN(i)->set_strength) {
            ret = GET_ALG_FUN(i)->set_strength(g_pq_param.strength[i]);
            if (ret != HI_SUCCESS) {
                HI_ERR_PQ("alg ID:%d, set_strength error!\n", i);
            }
        }
    }

    /* set demo mode */
    ret = drv_pq_set_demo_disp_mode(type, g_pq_param.demo_mode);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_demo_coordinate(type, g_demo_coordinate);

    return ret;
}

static hi_s32 drv_pq_set_scene_mode(hi_drv_pq_vpss_scene_mode pq_module)
{
    pq_common_status pq_status = { 0 };

    PQ_CHECK_OVER_RANGE_RE_FAIL(pq_module.scene_mode, HI_DRV_PQ_SCENE_MODE_MAX);

    drv_pq_get_pq_status(&pq_status);

    if (pq_status.image_mode != PQ_IMAGE_MODE_NORMAL) {
        pq_comm_set_output_mode(PQ_OUTPUT_MODE_PREVIEW);
        return HI_SUCCESS;
    }

    if (pq_module.scene_mode == HI_DRV_PQ_SCENE_MODE_PREVIEW) {
        pq_comm_set_output_mode(PQ_OUTPUT_MODE_PREVIEW);
    } else if (pq_module.scene_mode == HI_DRV_PQ_SCENE_MODE_REMOTE) {
        pq_comm_set_output_mode(PQ_OUTPUT_MODE_REMOTE);
    }

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_vppreview_mode(hi_drv_pq_vp_mode vp_scene_mode)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(vp_scene_mode, HI_DRV_PQ_VIDEOPHONE_MODE_MAX);

    g_vp_mode_en[HI_DRV_PQ_VP_TYPE_PREVIEW] = vp_scene_mode;

    return HI_SUCCESS;
}

/* get VP preview mode */
hi_s32 drv_pq_get_vppreview_mode(hi_drv_pq_vp_mode *pen_vp_scene_mode)
{
    hi_drv_pq_vp_mode vp_scene_mode;
    PQ_CHECK_NULL_PTR_RE_FAIL(pen_vp_scene_mode);

    vp_scene_mode = g_vp_mode_en[HI_DRV_PQ_VP_TYPE_PREVIEW];

    *pen_vp_scene_mode = vp_scene_mode;

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_vpremote_mode(hi_drv_pq_vp_mode vp_scene_mode)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(vp_scene_mode, HI_DRV_PQ_VIDEOPHONE_MODE_MAX);

    g_vp_mode_en[HI_DRV_PQ_VP_TYPE_REMOTE] = vp_scene_mode;

    return HI_SUCCESS;
}

/* get VP remote mode */
hi_s32 drv_pq_get_vpremote_mode(hi_drv_pq_vp_mode *pen_vp_scene_mode)
{
    hi_drv_pq_vp_mode vp_scene_mode;
    PQ_CHECK_NULL_PTR_RE_FAIL(pen_vp_scene_mode);

    vp_scene_mode = g_vp_mode_en[HI_DRV_PQ_VP_TYPE_REMOTE];

    *pen_vp_scene_mode = vp_scene_mode;

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_image_mode(hi_pq_image_mode image_mode)
{
    hi_s32 ret;
    pq_common_status pq_status;

    PQ_CHECK_OVER_RANGE_RE_FAIL(image_mode, HI_PQ_IMAGE_MODE_MAX);

    drv_pq_get_pq_status(&pq_status);
    pq_status.image_mode = (pq_comm_image_mode)image_mode;

    ret = drv_pq_set_pq_status(&pq_status);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_sharpness(g_pq_param.strength[HI_PQ_MODULE_SHARPNESS]);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_set_module_en(HI_PQ_MODULE_DCI, g_pq_param.module_on_off[HI_PQ_MODULE_DCI]);

    return ret;
}

hi_s32 drv_pq_get_image_mode(hi_pq_image_mode *pen_image_mode)
{
    pq_common_status pq_status = { 0 };

    PQ_CHECK_NULL_PTR_RE_FAIL(pen_image_mode);

    drv_pq_get_pq_status(&pq_status);
    *pen_image_mode = (hi_pq_image_mode)pq_status.image_mode;

    return HI_SUCCESS;
}

/*********************************VPSS INTF**********************************************/
hi_void drv_pq_ajust_vpss_alg_enble(hi_drv_pq_vpss_info *vpss_src_info)
{
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_src_info);

    if ((vpss_src_info->hdr_type_in == HI_DRV_HDR_TYPE_DOLBYVISION) ||
        (vpss_src_info->hdr_type_out == HI_DRV_HDR_TYPE_DOLBYVISION)) {
        drv_pq_set_module_en(HI_PQ_MODULE_TNR, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_SNR, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_DB, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_DM, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_DBM, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_DR, HI_FALSE);

        drv_pq_set_module_en(HI_PQ_MODULE_DEI, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_FMD, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_FOD, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_COCO, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_VPSSCSC, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_VPSSHDR, HI_FALSE);
    } else if ((vpss_src_info->hdr_type_in == HI_DRV_HDR_TYPE_HDR10) ||
               (vpss_src_info->hdr_type_in == HI_DRV_HDR_TYPE_HLG) ||
#ifdef HI_HDR10PLUS_SUPPORT
               (vpss_src_info->hdr_type_in == HI_DRV_HDR_TYPE_HDR10PLUS) ||
               (vpss_src_info->hdr_type_out == HI_DRV_HDR_TYPE_HDR10PLUS) ||
#endif
               (vpss_src_info->hdr_type_out == HI_DRV_HDR_TYPE_HDR10) ||
               (vpss_src_info->hdr_type_out == HI_DRV_HDR_TYPE_HLG)) {
        drv_pq_set_module_en(HI_PQ_MODULE_DS, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_COCO, HI_FALSE);
    } else if ((vpss_src_info->hdr_type_in == HI_DRV_HDR_TYPE_CUVA) ||
               (vpss_src_info->hdr_type_in == HI_DRV_HDR_TYPE_JTP_SL_HDR) ||
               (vpss_src_info->hdr_type_out == HI_DRV_HDR_TYPE_CUVA) ||
               (vpss_src_info->hdr_type_out == HI_DRV_HDR_TYPE_JTP_SL_HDR)) {
        HI_ERR_PQ(" Nout support SL type!\n");
    }
}

hi_s32 hi_drv_pq_init_vpss_alg(hi_drv_pq_vpss_scene_mode scence_mode, hi_drv_pq_vpss_info *vpss_src_info,
                               hi_void *vpss_reg)
{
    hi_s32 ret;
    hi_u64 start_time;
    hi_u64 end_time;
    pq_source_mode source_mode;
    pq_output_mode output_mode;

    PQ_CHECK_OVER_RANGE_RE_FAIL(scence_mode.scene_mode, HI_DRV_PQ_SCENE_MODE_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_src_info);
    PQ_CHECK_OVER_RANGE_RE_FAIL(vpss_src_info->handle_id, VPSS_HANDLE_NUM);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();
    pq_hal_update_vpss_reg(vpss_src_info->handle_id, (vpss_reg_type *)vpss_reg);

    /* VPSS deinit send null to clear reg para */
    if ((vpss_reg == HI_NULL) || (vpss_src_info == HI_NULL)) {
        return HI_SUCCESS;
    }

    if (vpss_src_info->frame_width == 0 || vpss_src_info->frame_height == 0) {
        HI_ERR_PQ("VPSS timing W | H is zero !\n");
        return HI_SUCCESS;
    }

#ifdef PQ_SOFTWARE_ALG_SUPPORT
    drv_pq_alloc_vpss_api(vpss_src_info->handle_id);
#endif

    g_image_width = vpss_src_info->frame_width;
    g_image_height = vpss_src_info->frame_height;

    source_mode = pq_comm_check_source_mode(vpss_src_info->frame_width);
    pq_comm_set_source_mode(source_mode);

    ret = drv_pq_set_scene_mode(scence_mode);
    PQ_CHECK_RETURN_SUCCESS(ret);
    output_mode = pq_comm_get_output_mode();

    /* load VPSS physical register */
    ret = pq_table_load_phy_list(REG_TYPE_VPSS, source_mode, output_mode, vpss_src_info->handle_id);
    PQ_CHECK_RETURN_SUCCESS(ret);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_FMD, init_fod_param)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_FMD)->init_fod_param(vpss_src_info->handle_id);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_FMD, init_ifmd_param)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_FMD)->init_ifmd_param(vpss_src_info->frame_width, vpss_src_info->frame_height);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* reset vpss customer pq setting */
    ret = drv_pq_reset_pq_status(REG_TYPE_VPSS);
    PQ_CHECK_RETURN_SUCCESS(ret);

    drv_pq_ajust_vpss_alg_enble(vpss_src_info);
#ifdef PQ_SOFTWARE_ALG_SUPPORT
    g_pq_vpss_alg_index[vpss_src_info->handle_id] = 0;
    drv_pq_wakeup_thread(0);
#endif

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_INIT_VPSS_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

/* ********************************VDP INTF********************************************* */
static hi_s32 drv_pq_update_partly_vdp_pq(pq_source_mode source_mode, pq_output_mode output_mode)
{
    hi_s32 ret;

    PQ_CHECK_OVER_RANGE_RE_FAIL(source_mode, PQ_SOURCE_MODE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(output_mode, PQ_OUTPUT_MODE_MAX);

    ret = pq_table_load_multi_list(REG_TYPE_VDP, source_mode, output_mode);

    return ret;
}

static hi_s32 drv_pq_updatefull_vdp_pq(pq_source_mode source_mode, pq_output_mode output_mode)
{
    hi_s32 ret;

    PQ_CHECK_OVER_RANGE_RE_FAIL(source_mode, PQ_SOURCE_MODE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(output_mode, PQ_OUTPUT_MODE_MAX);

    ret = pq_table_load_phy_list(REG_TYPE_VDP, source_mode, output_mode, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_table_load_phy_list VDP failed!\n");
        return ret;
    }

    /* set picture setting */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, set_csc_video_setting)) {
        GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(HI_PQ_DISPLAY_0,
                (hi_pq_image_param*)&g_pq_param.sd_video_setting);
        GET_ALG_FUN(HI_PQ_MODULE_CSC)->set_csc_video_setting(HI_PQ_DISPLAY_1,
                (hi_pq_image_param*)&g_pq_param.hd_video_setting);
    }

    /* update coef */
    if ((PQ_FUNC_CALL(HI_PQ_MODULE_DCI, set_resume)) && (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, updat_dci_coef))) {
        if (GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_resume) {
            GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_resume(!g_load_pq_bin);
        }

        if (GET_ALG_FUN(HI_PQ_MODULE_DCI)->updat_dci_coef) {
            GET_ALG_FUN(HI_PQ_MODULE_DCI)->updat_dci_coef(PQ_VDP_LAYER_VID0);
        }
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, updat_acm_coef)) {
        GET_ALG_FUN(HI_PQ_MODULE_ACM)->updat_acm_coef(PQ_VDP_LAYER_VP0);
    }

    return ret;
}

hi_void drv_pq_ajust_vdp_alg_enble(hi_drv_pq_vdp_info *vdp_info)
{
    PQ_CHECK_NULL_PTR_RE_NULL(vdp_info);

    if ((vdp_info->frame_info.hdr_type_in == HI_DRV_HDR_TYPE_DOLBYVISION) ||
        (vdp_info->frame_info.hdr_type_out == HI_DRV_HDR_TYPE_DOLBYVISION)) {
        drv_pq_set_module_en(HI_PQ_MODULE_SHARPNESS, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_CLE, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_SR, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_CSC, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_HDR, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_DC, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_DCI, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_ACM, HI_FALSE);

        drv_pq_set_module_en(HI_PQ_MODULE_GFXCSC, HI_FALSE);
        drv_pq_set_module_en(HI_PQ_MODULE_GFXHDR, HI_FALSE);
    } else if ((vdp_info->frame_info.hdr_type_in == HI_DRV_HDR_TYPE_HDR10) ||
               (vdp_info->frame_info.hdr_type_in == HI_DRV_HDR_TYPE_HLG) ||
#ifdef HI_HDR10PLUS_SUPPORT
               (vdp_info->frame_info.hdr_type_in == HI_DRV_HDR_TYPE_HDR10PLUS) ||
               (vdp_info->frame_info.hdr_type_out == HI_DRV_HDR_TYPE_HDR10PLUS) ||
#endif
               (vdp_info->frame_info.hdr_type_out == HI_DRV_HDR_TYPE_HDR10) ||
               (vdp_info->frame_info.hdr_type_out == HI_DRV_HDR_TYPE_HLG)) {
    } else if ((vdp_info->frame_info.hdr_type_in == HI_DRV_HDR_TYPE_CUVA) ||
               (vdp_info->frame_info.hdr_type_in == HI_DRV_HDR_TYPE_JTP_SL_HDR) ||
               (vdp_info->frame_info.hdr_type_out == HI_DRV_HDR_TYPE_CUVA) ||
               (vdp_info->frame_info.hdr_type_out == HI_DRV_HDR_TYPE_JTP_SL_HDR)) {
        HI_ERR_PQ(" Nout support SL type!\n");
    }
}

hi_s32 hi_drv_pq_update_vdp_alg(hi_drv_pq_display_id display_id, hi_drv_pq_vdp_info *vdp_info)
{
    hi_s32 ret;
    hi_u64 start_time;
    hi_u64 end_time;
    hi_bool timing_change;
    pq_source_mode source_mode;
    pq_output_mode output_mode;
    static hi_drv_pq_vdp_info vdp_info_tmp[HI_DRV_PQ_DISPLAY_MAX] = { { 0 } };

    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_info);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);
    PQ_CHECK_OVER_RANGE_RE_FAIL(display_id, HI_DRV_PQ_DISPLAY_MAX);

    start_time = hi_pq_sys_get_time_stamp_us();
    if ((vdp_info != HI_NULL) && (vdp_info->layer_id == HI_DRV_PQ_VDP_LAYER_VID1)) {
        return HI_SUCCESS;
    }

    if (vdp_info->frame_info.frame_width == 0 || vdp_info->frame_info.frame_height == 0) {
        HI_ERR_PQ("VDP timing W | H is zero !\n");
        return HI_SUCCESS;
    }

    /* update VDP confignure every interrupt */
    drv_pq_refresh_vdp_cfg();

#if (defined(TNR_NOTEE_SUPPORT) || defined(TNR_TEE_SUPPORT))
    if (PQ_FUNC_CALL(HI_PQ_MODULE_DC, update_dc_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DC)->update_dc_cfg();
    }
#endif

    timing_change = (vdp_info_tmp[display_id].frame_info.frame_width != vdp_info->frame_info.frame_width)
                    || (vdp_info_tmp[display_id].frame_info.frame_height != vdp_info->frame_info.frame_height)
                    || (vdp_info_tmp[display_id].channel_timing[display_id].video_rect.rect_w !=
                        vdp_info->channel_timing[display_id].video_rect.rect_w)
                    || (vdp_info_tmp[display_id].channel_timing[display_id].video_rect.rect_h !=
                        vdp_info->channel_timing[display_id].video_rect.rect_h)
                    || (vdp_info_tmp[display_id].frame_info.hdr_type_in != vdp_info->frame_info.hdr_type_in)
                    || (vdp_info_tmp[display_id].frame_info.hdr_type_out != vdp_info->frame_info.hdr_type_out)
                    || (osal_memncmp(&g_pq_status.timing_info[display_id], sizeof(hi_drv_pq_vdp_channel_timing),
                                     &vdp_info->channel_timing[display_id], sizeof(hi_drv_pq_vdp_channel_timing)) != 0);
    if ((vdp_info->frame_info.is_part_update == HI_TRUE) && (timing_change == HI_FALSE)) {
        return HI_SUCCESS;
    }

    memcpy(&vdp_info_tmp[display_id], vdp_info, sizeof(hi_drv_pq_vdp_info));
    memcpy(&(g_pq_status.timing_info[display_id]), &(vdp_info->channel_timing[display_id]),
           sizeof(hi_drv_pq_vdp_channel_timing));

    /* ignored sd */
    if (display_id == HI_DRV_PQ_DISPLAY_0) {
        return HI_SUCCESS;
    }

    g_pq_status.isogeny_mode = vdp_info->frame_info.is_ogeny_mode;
    g_image_width = vdp_info->frame_info.frame_width;
    g_image_height = vdp_info->frame_info.frame_height;
    source_mode = pq_comm_check_source_mode(vdp_info->frame_info.frame_width);
    pq_comm_set_source_mode(source_mode);
    output_mode = pq_comm_get_output_mode();
    if ((output_mode != PQ_OUTPUT_MODE_PREVIEW) && (output_mode != PQ_OUTPUT_MODE_REMOTE)) {
        output_mode = pq_comm_check_output_mode(vdp_info->channel_timing[display_id].fmt_rect.rect_w,
            vdp_info->channel_timing[display_id].refresh_rate); /* 100: fresh rate multi */
        pq_comm_set_output_mode(output_mode);
    }

    if (display_id == HI_DRV_PQ_DISPLAY_1) {
        g_out_width = vdp_info->channel_timing[display_id].video_rect.rect_w;
        g_out_height = vdp_info->channel_timing[display_id].video_rect.rect_h;
    }

    /* load VDP physical register */
    if (vdp_info->frame_info.is_part_update == HI_TRUE) {
        /* set partly update */
        drv_pq_update_partly_vdp_pq(source_mode, output_mode);
    } else {
        /* set FULL update */
        ret = drv_pq_updatefull_vdp_pq(source_mode, output_mode);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* reset VDP customer pq setting */
    ret = drv_pq_reset_pq_status(REG_TYPE_VDP);
    PQ_CHECK_RETURN_SUCCESS(ret);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, set_vdp_hdr_type)) {
        GET_ALG_FUN(HI_PQ_MODULE_ACM)->set_vdp_hdr_type(vdp_info->frame_info.hdr_type_in,
                vdp_info->frame_info.hdr_type_out);
    }
    drv_pq_ajust_vdp_alg_enble(vdp_info);

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_VDP_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_update_vdp_stt_info(hi_void)
{
    hi_s32 ret;
    hi_u64 start_time;
    hi_u64 end_time;

    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);
    start_time = hi_pq_sys_get_time_stamp_us();

    ret = pq_comm_vdp_stt_update();
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("update vdp stt failed! ret->%d.\n", ret);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DC, update_dc_stt_info)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DC)->update_dc_stt_info();
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_VDP_STT] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_void drv_pq_trans_csc(hi_drv_color_descript color_descript, drv_pq_internal_color_space *internal_csc)
{
    hi_drv_color_primary    color_primary;
    hi_drv_color_space          color_space;
    hi_drv_color_quantify_range quantify_range;

    color_primary = color_descript.color_primary;
    color_space = color_descript.color_space;
    quantify_range = color_descript.quantify_range;

    if (((color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) ||
         (color_primary == HI_DRV_COLOR_PRIMARY_BT601_625)) && (color_space == HI_DRV_COLOR_CS_YUV)
        && (quantify_range == HI_DRV_COLOR_LIMITED_RANGE)) {
        *internal_csc = HI_DRV_CS_BT601_YUV_LIMITED;
    } else if (((color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) ||
                (color_primary == HI_DRV_COLOR_PRIMARY_BT601_625)) && (color_space == HI_DRV_COLOR_CS_RGB) &&
               (quantify_range == HI_DRV_COLOR_LIMITED_RANGE)) {
        *internal_csc = HI_DRV_CS_BT601_RGB_LIMITED;
    } else if (((color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) ||
                (color_primary == HI_DRV_COLOR_PRIMARY_BT601_625)) && (color_space == HI_DRV_COLOR_CS_YUV) &&
               (quantify_range == HI_DRV_COLOR_FULL_RANGE)) {
        *internal_csc = HI_DRV_CS_BT601_YUV_FULL;
    } else if (((color_primary == HI_DRV_COLOR_PRIMARY_BT601_525) ||
                (color_primary == HI_DRV_COLOR_PRIMARY_BT601_625)) && (color_space == HI_DRV_COLOR_CS_RGB) &&
               (quantify_range == HI_DRV_COLOR_FULL_RANGE)) {
        *internal_csc = HI_DRV_CS_BT601_RGB_FULL;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT709) && (color_space == HI_DRV_COLOR_CS_YUV) &&
               (quantify_range == HI_DRV_COLOR_LIMITED_RANGE)) {
        *internal_csc = HI_DRV_CS_BT709_YUV_LIMITED;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT709) && (color_space == HI_DRV_COLOR_CS_YUV) &&
               (quantify_range == HI_DRV_COLOR_FULL_RANGE)) {
        *internal_csc = HI_DRV_CS_BT709_YUV_FULL;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT709) && (color_space == HI_DRV_COLOR_CS_RGB) &&
               (quantify_range == HI_DRV_COLOR_LIMITED_RANGE)) {
        *internal_csc = HI_DRV_CS_BT709_RGB_LIMITED;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT709) && (color_space == HI_DRV_COLOR_CS_RGB) &&
               (quantify_range == HI_DRV_COLOR_FULL_RANGE)) {
        *internal_csc = HI_DRV_CS_BT709_RGB_FULL;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT2020) && (color_space == HI_DRV_COLOR_CS_YUV) &&
               (quantify_range == HI_DRV_COLOR_LIMITED_RANGE)) {
        *internal_csc = HI_DRV_CS_BT2020_YUV_LIMITED;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT2020) && (color_space == HI_DRV_COLOR_CS_YUV) &&
               (quantify_range == HI_DRV_COLOR_FULL_RANGE)) {
        *internal_csc = HI_DRV_CS_BT2020_YUV_FULL;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT2020) && (color_space == HI_DRV_COLOR_CS_RGB) &&
               (quantify_range == HI_DRV_COLOR_LIMITED_RANGE)) {
        *internal_csc = HI_DRV_CS_BT2020_RGB_LIMITED;
    } else if ((color_primary == HI_DRV_COLOR_PRIMARY_BT2020) && (color_space == HI_DRV_COLOR_CS_RGB) &&
               (quantify_range == HI_DRV_COLOR_FULL_RANGE)) {
        *internal_csc = HI_DRV_CS_BT2020_RGB_FULL;
    } else {
        HI_ERR_PQ("[%d] [%d] [%d] No Support !\n", color_primary, color_space, quantify_range);
        return;
    }

    return;
}

/* vedio CSC port must every frame */
hi_s32 hi_drv_pq_get_vdp_csc_coef(hi_drv_pq_csc_layer layer_id, hi_drv_pq_csc_info csc_info,
                                  hi_drv_pq_csc_coef *csc_coef)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;
    drv_pq_internal_csc_info internal_csc = {0};

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_CSC_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(csc_coef);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();
    internal_csc.csc_en = csc_info.csc_en;
    internal_csc.csc_pecision = csc_info.csc_pecision;
    internal_csc.csc_type = csc_info.csc_type;
    drv_pq_trans_csc(csc_info.csc_in, &(internal_csc.csc_in));
    drv_pq_trans_csc(csc_info.csc_out, &(internal_csc.csc_out));

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CSC, get_csc_coef)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CSC)->get_csc_coef(layer_id, internal_csc, csc_coef);
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_GET_VDP_CSC] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

/* ********************************GFX INTF********************************************* */
/* gfx CSC port must every frame */
hi_s32 hi_drv_pq_get_gfx_csc_coef(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_csc_info *gfx_csc_info,
                                  hi_drv_pq_gfx_csc_coef *gfx_csc_coef)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;
    hi_u64 start_time;
    hi_u64 end_time;
    drv_pq_internal_gfx_csc_info internal_gfx_csc_info = {0};

    PQ_CHECK_OVER_RANGE_RE_FAIL(gfx_layer, HI_DRV_PQ_GFX_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_csc_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_csc_coef);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();

    internal_gfx_csc_info.is_bgr_in = gfx_csc_info->is_bgr_in;
    internal_gfx_csc_info.gfx_csc_mode.csc_en = gfx_csc_info->gfx_csc_mode.csc_en;
    internal_gfx_csc_info.gfx_csc_mode.csc_pecision = gfx_csc_info->gfx_csc_mode.csc_pecision;
    internal_gfx_csc_info.gfx_csc_mode.csc_type = gfx_csc_info->gfx_csc_mode.csc_type;
    drv_pq_trans_csc(gfx_csc_info->gfx_csc_mode.csc_in, &(internal_gfx_csc_info.gfx_csc_mode.csc_in));
    drv_pq_trans_csc(gfx_csc_info->gfx_csc_mode.csc_out, &(internal_gfx_csc_info.gfx_csc_mode.csc_out));

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXCSC, get_gfx_csc_coef)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXCSC)->get_gfx_csc_coef(gfx_layer, &internal_gfx_csc_info, gfx_csc_coef);
    }

    if (gfx_layer == HI_DRV_PQ_GFX_LAYER_GP1) {
        for (i = 0; i < PQ_PQ_UPDATE_MAX; i++) {
            if (g_sd_update_flag[HI_PQ_MODULE_GFXCSC][i] == HI_TRUE) {
                gfx_csc_coef->is_update = HI_TRUE;
                break;
            }
            gfx_csc_coef->is_update = HI_FALSE;
        }
    } else if (gfx_layer == HI_DRV_PQ_GFX_LAYER_GP0) {
        for (i = 0; i < PQ_PQ_UPDATE_MAX; i++) {
            if (g_hd_update_flag[HI_PQ_MODULE_GFXCSC][i] == HI_TRUE) {
                gfx_csc_coef->is_update = HI_TRUE;
                break;
            }
            gfx_csc_coef->is_update = HI_FALSE;
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_GET_GFX_CSC] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

#ifdef PQ_ALG_CLE
static hi_s32 drv_pq_get_cle_enable(hi_drv_pq_zme_strategy_out *zme_out)
{
    pq_cle_reso cle_reso[CLE_ID_MAX] = { 0 };
    hi_s32 ret;

    if ((GET_ALG(HI_PQ_MODULE_CLE) == HI_NULL) || (GET_ALG_FUN(HI_PQ_MODULE_CLE) == HI_NULL)) {
        HI_ERR_PQ("alg ID:%d func not register\n", HI_PQ_MODULE_CLE);
        return HI_FAILURE;
    }

    if (GET_ALG_FUN(HI_PQ_MODULE_CLE)->get_cle_reso) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->get_cle_reso(CLE_ID_V0_4K, &cle_reso[CLE_ID_V0_4K]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("alg ID:%d get_cle_reso failed!\n", HI_PQ_MODULE_CLE);
            return ret;
        }

        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->get_cle_reso(CLE_ID_V0_8K, &cle_reso[CLE_ID_V0_8K]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("alg ID:%d get_cle_reso failed!\n", HI_PQ_MODULE_CLE);
            return ret;
        }
    }

    zme_out->hd_zme_strategy.cle_4k_en = cle_reso[CLE_ID_V0_4K].cle_enable;
    zme_out->hd_zme_strategy.cle_8k_en = cle_reso[CLE_ID_V0_8K].cle_enable;

    return HI_SUCCESS;
}

static hi_s32 drv_pq_set_cle_enable(hi_drv_pq_zme_strategy_in *zme_in, hi_drv_pq_zme_strategy_out *zme_out)
{
    pq_cle_reso cle_reso[CLE_ID_MAX] = {0};
    hi_s32 ret;

    cle_reso[CLE_ID_V0_4K].cle_enable = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_enable;
    cle_reso[CLE_ID_V0_4K].clm_enable = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_enable;

    cle_reso[CLE_ID_V0_4K].video_in_width = zme_in->hd_zme_strategy.zme_common_info.zme_w_in;
    cle_reso[CLE_ID_V0_4K].video_in_height = zme_in->hd_zme_strategy.zme_common_info.zme_h_in;
    cle_reso[CLE_ID_V0_4K].cle_in_width = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_w_out;
    cle_reso[CLE_ID_V0_4K].video_out_width = zme_in->hd_zme_strategy.zme_common_info.zme_w_out;
    cle_reso[CLE_ID_V0_4K].cle_in_height = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_h_out;
    cle_reso[CLE_ID_V0_4K].video_out_height = zme_in->hd_zme_strategy.zme_common_info.zme_h_out;

    cle_reso[CLE_ID_V0_8K].cle_enable = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_enable;
    cle_reso[CLE_ID_V0_8K].clm_enable = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_enable;

    cle_reso[CLE_ID_V0_8K].video_in_width = zme_in->hd_zme_strategy.zme_common_info.zme_w_in;
    cle_reso[CLE_ID_V0_8K].video_in_height = zme_in->hd_zme_strategy.zme_common_info.zme_h_in;
    cle_reso[CLE_ID_V0_8K].cle_in_width = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
    cle_reso[CLE_ID_V0_8K].video_out_width = zme_in->hd_zme_strategy.zme_common_info.zme_w_out;
    cle_reso[CLE_ID_V0_8K].cle_in_height = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;
    cle_reso[CLE_ID_V0_8K].video_out_height = zme_in->hd_zme_strategy.zme_common_info.zme_h_out;

    if ((GET_ALG(HI_PQ_MODULE_CLE) == HI_NULL) || (GET_ALG_FUN(HI_PQ_MODULE_CLE) == HI_NULL)) {
        HI_ERR_PQ("alg ID:%d func not register\n", HI_PQ_MODULE_CLE);
        return HI_FAILURE;
    }

    if (GET_ALG_FUN(HI_PQ_MODULE_CLE)->set_cle_reso) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->set_cle_reso(CLE_ID_V0_4K, &cle_reso[CLE_ID_V0_4K]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("alg ID:%d set_cle_reso failed!\n", HI_PQ_MODULE_CLE);
            return ret;
        }

        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->set_cle_reso(CLE_ID_V0_8K, &cle_reso[CLE_ID_V0_8K]);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("alg ID:%d set_cle_reso failed!\n", HI_PQ_MODULE_CLE);
            return ret;
        }
    }

    return HI_SUCCESS;
}
#endif

#ifdef PQ_ALG_SHARPEN
static hi_s32 drv_pq_set_sharp_reso(hi_drv_pq_zme_strategy_in *zme_in, hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret;
    pq_sharpen_reso sharp_reso = { 0 };

    sharp_reso.sharpen_in_width  = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KZME].zme_w_out;
    sharp_reso.sharpen_in_height = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KZME].zme_h_out;

    sharp_reso.video_in_width = zme_in->hd_zme_strategy.zme_common_info.zme_w_in;
    sharp_reso.video_in_height = zme_in->hd_zme_strategy.zme_common_info.zme_h_in;
    sharp_reso.video_out_width = zme_in->hd_zme_strategy.zme_common_info.zme_w_out;
    sharp_reso.video_out_height = zme_in->hd_zme_strategy.zme_common_info.zme_h_out;

    if ((GET_ALG(HI_PQ_MODULE_SHARPNESS) == HI_NULL) || (GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS) == HI_NULL)) {
        HI_ERR_PQ("alg ID:%d func not register\n", HI_PQ_MODULE_SHARPNESS);
        return HI_FAILURE;
    }

    if (GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->set_sharp_reso) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->set_sharp_reso(&sharp_reso);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("alg ID:%d set_sharp_reso failed!\n", HI_PQ_MODULE_SHARPNESS);
            return ret;
        }
    }

    return HI_SUCCESS;
}
#endif

#ifdef PQ_ALG_DCI
static hi_s32 pq_mng_sr_set_dci_reso(hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret;
    pq_dci_ref_reso dci_reso = { 0 };

    dci_reso.sr_4k_en = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_enable;
    dci_reso.sr_8k_en = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_enable;

    dci_reso.sr_4k_in_width  = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_w_in;
    dci_reso.sr_4k_in_height = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_4KSR].zme_h_in;

    dci_reso.sr_8k_out_width  = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_w_out;
    dci_reso.sr_8k_out_height = zme_out->hd_zme_strategy.zme_fmt[HI_DRV_PQ_8KSR].zme_h_out;

    if ((GET_ALG(HI_PQ_MODULE_DCI) == HI_NULL) || (GET_ALG_FUN(HI_PQ_MODULE_DCI) == HI_NULL)) {
        HI_ERR_PQ("alg ID:%d func not register\n", HI_PQ_MODULE_DCI);
        return HI_FAILURE;
    }

    if (GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_dci_reso) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->set_dci_reso(&dci_reso);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("alg ID:%d set_dci_reso not found.\n", HI_PQ_MODULE_DCI);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}
#endif

static hi_s32 hi_drv_pq_online_alg_update(hi_drv_pq_zme_strategy_in *zme_in, hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret;

#ifdef PQ_ALG_SHARPEN
    ret = drv_pq_set_sharp_reso(zme_in, zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_SHARPNESS, update_sharp_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->update_sharp_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }
#endif

#ifdef PQ_ALG_CLE
    ret = drv_pq_set_cle_enable(zme_in, zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = drv_pq_get_cle_enable(zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_CLE, update_cle_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->update_cle_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }
#else
    zme_out->hd_zme_strategy.cle_4k_en = HI_FALSE;
    zme_out->hd_zme_strategy.cle_8k_en = HI_FALSE;
#endif

#ifdef PQ_ALG_DCI
    ret = pq_mng_sr_set_dci_reso(zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, update_dci_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->update_dci_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }
#endif

    return HI_SUCCESS;
}

hi_s32 hi_drv_pq_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in *zme_in,
                                      hi_drv_pq_zme_strategy_out *zme_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;
    static hi_bool sr_enable = HI_TRUE;

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_DISP_LAYER_ZME_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();

    if (sr_enable != zme_in->hd_zme_strategy.hdcp_factor.sr_en) {
        sr_enable = zme_in->hd_zme_strategy.hdcp_factor.sr_en;
        if (PQ_FUNC_CALL(HI_PQ_MODULE_SR, set_enable)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_SR)->set_enable(sr_enable);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ZME, get_vdp_zme_strategy)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ZME)->get_vdp_zme_strategy(layer_id, zme_in, zme_out);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    ret = hi_drv_pq_online_alg_update(zme_in, zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    zme_out->hd_zme_strategy.is_update = HI_TRUE;
    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_GET_VDP_ZME] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

#ifdef PQ_VP_SUPPORT
hi_s32 drv_pq_switch_scene_mode(hi_drv_pq_scene_mode scene_mode, hi_drv_pq_vp_type *en_drv_pq_vp_type)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(scene_mode, HI_DRV_PQ_SCENE_MODE_MAX);

    switch (scene_mode) {
        case HI_DRV_PQ_SCENE_MODE_NORMAL: {
            *en_drv_pq_vp_type = HI_DRV_PQ_VP_TYPE_NORMAL;
            ;
        }
        break;
        case HI_DRV_PQ_SCENE_MODE_PREVIEW: {
            *en_drv_pq_vp_type = HI_DRV_PQ_VP_TYPE_PREVIEW;
        }
        break;
        case HI_DRV_PQ_SCENE_MODE_REMOTE: {
            *en_drv_pq_vp_type = HI_DRV_PQ_VP_TYPE_REMOTE;
            ;
        }
        break;
        default: {
            *en_drv_pq_vp_type = HI_DRV_PQ_VP_TYPE_NORMAL;
        }
        break;
    }

    return HI_SUCCESS;
}
#endif

hi_s32 hi_drv_pq_get_vpsszme_coef(hi_drv_pq_vpss_layer layer_id,
    hi_drv_pq_vpsszme_in *zme_in,
    hi_drv_pq_vpsszme_out *zme_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_VPSS_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();

    if (PQ_FUNC_CALL(HI_PQ_MODULE_VPSSZME, get_vpsszme_coef)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_VPSSZME)->get_vpsszme_coef(layer_id, zme_in, zme_out);
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_GET_VPSS_ZME] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_set_vpss_hdr_cfg(hi_drv_pq_vpss_layer layer_id,
                                  hi_drv_pq_xdr_frame_info *xdr_frame_info,
                                  hi_void *vpss_reg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;
    drv_pq_internal_xdr_frame_info internal_xdr = {0};

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_VPSS_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(xdr_frame_info);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();

    internal_xdr.window_num = xdr_frame_info->window_num;
    internal_xdr.src_hdr_type = xdr_frame_info->src_hdr_type;
    internal_xdr.disp_hdr_type = xdr_frame_info->disp_hdr_type;
    internal_xdr.handle = xdr_frame_info->handle;

    drv_pq_trans_csc(xdr_frame_info->color_space_in, &(internal_xdr.color_space_in));
    drv_pq_trans_csc(xdr_frame_info->color_space_out, &(internal_xdr.color_space_out));

    if (PQ_FUNC_CALL(HI_PQ_MODULE_VPSSHDR, set_vpss_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_VPSSHDR)->set_vpss_hdr_cfg(layer_id, &internal_xdr, (vpss_reg_type*)vpss_reg);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* hdr process ACM use */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_DEI, get_vpss_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DEI)->get_vpss_hdr_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* hdr process DCI use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_DB, get_vpss_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DB)->get_vpss_hdr_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* hdr process SHARPEN use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_DM, get_vpss_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DM)->get_vpss_hdr_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* hdr process CLE use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_TNR, get_vpss_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_TNR)->get_vpss_hdr_cfg();
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    /* hdr process SR use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_SNR, get_vpss_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SNR)->get_vpss_hdr_cfg();
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_SET_VPSS_HDR] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_get_vdp_zme_coef(hi_drv_pq_zme_coef_in zme_coef_in, hi_drv_pq_zme_coef_out *zme_coef_out)
{
    hi_s32 ret = HI_SUCCESS;

    PQ_CHECK_NULL_PTR_RE_FAIL(zme_coef_out);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_ZME, get_vdp_zme_coef)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ZME)->get_vdp_zme_coef(zme_coef_in, zme_coef_out);
    }

    return ret;
}

hi_s32 hi_drv_pq_update_nr_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_input_info *nr_api_input)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();

    if ((vpss_stt_info->alg_en.nr_en == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_TNR, update_tnr_stt_info)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_TNR)->update_tnr_stt_info(vpss_stt_info, nr_api_input);
        if (ret != HI_SUCCESS) {
            nr_api_input->is_nr_ready = HI_FALSE;
            HI_ERR_PQ("get tnr api input info failed!\n");
            return HI_FAILURE;
        } else {
            nr_api_input->is_nr_ready = HI_TRUE;
        }
    }

    if ((vpss_stt_info->alg_en.nr_en == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_SNR, update_snr_stt_info)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SNR)->update_snr_stt_info(vpss_stt_info, nr_api_input);
        if (ret != HI_SUCCESS) {
            nr_api_input->is_nr_ready = HI_FALSE;
            HI_ERR_PQ("get tnr api input info failed!\n");
            return HI_FAILURE;
        } else {
            nr_api_input->is_nr_ready = HI_TRUE;
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_NR_STT] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_update_dei_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, drv_pq_dei_input_info *dei_api_input)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();
    if ((vpss_stt_info->frame_fmt == HI_DRV_PQ_FRM_FRAME) ||
        (vpss_stt_info->alg_en.dei_en != HI_TRUE)) { /* progressive */
        return HI_SUCCESS;
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_FMD, update_fod_ifmd_stt)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_FMD)->update_fod_ifmd_stt(vpss_stt_info, &(dei_api_input->vpss_cfg_info));
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    if (PQ_FUNC_CALL(HI_PQ_MODULE_DEI, update_dei_stt_info)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DEI)->update_dei_stt_info(vpss_stt_info, dei_api_input);
        if (ret != HI_SUCCESS) {
            dei_api_input->is_dei_ready = HI_FALSE;
            HI_ERR_PQ("get dei api input info failed!\n");
            return HI_FAILURE;
        } else {
            dei_api_input->is_dei_ready = HI_TRUE;
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_DEI_STT] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_update_db_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_db_input_info *db_api_input_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();
    if ((vpss_stt_info->alg_en.db_en == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_DB, update_db_stt_info)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DB)->update_db_stt_info(vpss_stt_info, db_api_input_info);
        if (ret != HI_SUCCESS) {
            db_api_input_info->is_db_ready = HI_FALSE;
            HI_ERR_PQ("get db api input info failed!\n");
            return HI_FAILURE;
        } else {
            db_api_input_info->is_db_ready = HI_TRUE;
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_DB_STT] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_update_dm_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_input_info *dm_api_input)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();
    if ((vpss_stt_info->alg_en.dm_en == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_DM, update_dm_stt_info)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DM)->update_dm_stt_info(vpss_stt_info, dm_api_input);
        if (ret != HI_SUCCESS) {
            dm_api_input->is_dm_ready = HI_FALSE;
            HI_ERR_PQ("get dm api input info failed!\n");
            return HI_FAILURE;
        } else {
            dm_api_input->is_dm_ready = HI_TRUE;
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_DB_STT] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_void drv_pq_check_vpss_stt_param(hi_drv_pq_vpss_stt_info* vpss_stt_info)
{
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_stt_info);
    PQ_CHECK_ZERO_RE_NULL(vpss_stt_info->width);
    PQ_CHECK_ZERO_RE_NULL(vpss_stt_info->height);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_stt_info->vpss_stt_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_stt_info->rgmv_reg_addr);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_stt_info->nr_stt_reg);
    PQ_CHECK_NULL_PTR_RE_NULL(vpss_stt_info->scan_stt_reg);
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(vpss_stt_info->handle_id, VPSS_HANDLE_NUM);
    pq_check_init_return_null(g_pq_init_flag.is_pq_module_init);
    return;
}

hi_s32 hi_drv_pq_update_vpss_stt_info(hi_drv_pq_vpss_stt_info *vpss_stt_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;
#ifdef PQ_SOFTWARE_ALG_SUPPORT
    hi_drv_pq_vpss_api_input *vpss_api_input = HI_NULL;
#endif

    drv_pq_check_vpss_stt_param(vpss_stt_info);
    start_time = hi_pq_sys_get_time_stamp_us();

#ifdef PQ_SOFTWARE_ALG_SUPPORT
    vpss_api_input = drv_pq_vpss_request_buffer(VPSS_BUF_IN, vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_api_input);

    if ((vpss_stt_info->alg_en.dei_en) || (vpss_stt_info->alg_en.nr_en)) {
        if (PQ_FUNC_CALL(HI_PQ_MODULE_DEI, update_field_flag)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_DEI)->update_field_flag(vpss_stt_info->handle_id,
                    g_pq_vpss_alg_index[vpss_stt_info->handle_id]);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }

        if (PQ_FUNC_CALL(HI_PQ_MODULE_DEI, update_dei_rc_info)) {
            ret = GET_ALG_FUN(HI_PQ_MODULE_DEI)->update_dei_rc_info(vpss_stt_info->handle_id,
                    vpss_stt_info->width, vpss_stt_info->height);
            PQ_CHECK_RETURN_SUCCESS(ret);
        }
    }

    vpss_api_input->handle = vpss_stt_info->handle_id;
    vpss_api_input->index = ++g_pq_vpss_alg_index[vpss_stt_info->handle_id];

    if (vpss_api_input->is_ready == HI_FALSE) {
        ret = hi_drv_pq_update_nr_stt_info(vpss_stt_info, &(vpss_api_input->nr_api_input));
        PQ_CHECK_RETURN_SUCCESS(ret);

        ret = hi_drv_pq_update_dei_stt_info(vpss_stt_info, &(vpss_api_input->dei_api_input));
        PQ_CHECK_RETURN_SUCCESS(ret);

        ret = hi_drv_pq_update_db_stt_info(vpss_stt_info, &(vpss_api_input->db_api_input));
        PQ_CHECK_RETURN_SUCCESS(ret);

        ret = hi_drv_pq_update_dm_stt_info(vpss_stt_info, &(vpss_api_input->dm_api_input));
        PQ_CHECK_RETURN_SUCCESS(ret);

        drv_pq_vpss_update_buffer(VPSS_BUF_IN, vpss_stt_info->handle_id);
    }
#endif

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_VPSS_STT] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_update_nr_alg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_nr_api_output_reg *nr_api_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();

    if ((nr_api_out->is_nr_output_ready == HI_TRUE) &&
        (g_pq_param.module_on_off[HI_PQ_MODULE_TNR] == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_TNR, update_tnr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_TNR)->update_tnr_cfg(vpss_stt_info, nr_api_out);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    if ((nr_api_out->is_nr_output_ready == HI_TRUE) &&
        (g_pq_param.module_on_off[HI_PQ_MODULE_SNR] == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_SNR, update_snr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SNR)->update_snr_cfg(vpss_stt_info, nr_api_out);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_NR_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_update_dei_alg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dei_api_output_reg *dei_api_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    if ((vpss_stt_info->frame_fmt == HI_DRV_PQ_FRM_FRAME) ||
        (vpss_stt_info->alg_en.dei_en != HI_TRUE)) { /* progressive */
        return HI_SUCCESS;
    }

    start_time = hi_pq_sys_get_time_stamp_us();

    if ((dei_api_out->is_dei_output_ready == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_DEI, update_dei_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DEI)->update_dei_cfg(vpss_stt_info->handle_id, dei_api_out);
        if (ret == HI_SUCCESS) {
            dei_api_out->is_dei_output_ready = HI_FALSE;
        } else {
            HI_ERR_PQ("update_dei_cfg failed!\n");
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_DEI_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_update_db_alg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_db_output_info *db_api_alg_output)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();
    /* DB update cfg */
    if ((db_api_alg_output->is_db_output_ready == HI_TRUE) &&
        (g_pq_param.module_on_off[HI_PQ_MODULE_DB] == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_DB, update_db_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DB)->update_db_cfg(vpss_stt_info, db_api_alg_output);
        if (ret == HI_SUCCESS) {
            db_api_alg_output->is_db_output_ready = HI_FALSE;
        } else {
            HI_ERR_PQ("db update_db_cfg failed! ret->%#x\n", ret);
        }
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_DB_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_update_dm_alg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_dm_output_reg *dm_api_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();
    if ((dm_api_out->is_dm_output_ready == HI_TRUE) &&
        (g_pq_param.module_on_off[HI_PQ_MODULE_DM] == HI_TRUE) &&
        PQ_FUNC_CALL(HI_PQ_MODULE_DM, update_dm_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DM)->update_dm_cfg(vpss_stt_info, dm_api_out);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_DM_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

/* 设置算法运算完之后的寄存器 */
hi_s32 hi_drv_pq_update_vpss_alg(hi_drv_pq_vpss_stt_info *vpss_stt_info, hi_drv_pq_vpss_cfg_info *vpss_cfg_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;
#ifdef PQ_SOFTWARE_ALG_SUPPORT
    hi_drv_pq_vpss_api_output *vpss_api_output = HI_NULL;
#endif

    drv_pq_check_vpss_stt_param(vpss_stt_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_cfg_info);
    start_time = hi_pq_sys_get_time_stamp_us();
    drv_pq_refresh_mkt_coordinate(REG_TYPE_VPSS);

#ifdef PQ_SOFTWARE_ALG_SUPPORT
UPDATE_VPSS_ALG:
    vpss_api_output = drv_pq_vpss_request_buffer(VPSS_BUF_OUT, vpss_stt_info->handle_id);
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_api_output);

    memcpy(vpss_cfg_info, &(vpss_api_output->dei_api_out.vpss_cfg_info), sizeof(hi_drv_pq_vpss_cfg_info));

    if (vpss_api_output->is_ready == HI_TRUE) {
        if ((vpss_api_output->index > g_pq_vpss_alg_index[vpss_stt_info->handle_id]) ||
            (vpss_api_output->index + PQ_THREAD_INDEX_MAX < g_pq_vpss_alg_index[vpss_stt_info->handle_id])) {
            HI_ERR_PQ("input index:%d output index:%d, not match\n", g_pq_vpss_alg_index[vpss_stt_info->handle_id],
                      vpss_api_output->index);
            return HI_FAILURE;
        }

        if ((vpss_api_output->index == g_pq_vpss_alg_index[vpss_stt_info->handle_id] - 1) ||
            (vpss_api_output->index == g_pq_vpss_alg_index[vpss_stt_info->handle_id])) {
            ret = hi_drv_pq_update_nr_alg(vpss_stt_info, &(vpss_api_output->nr_api_out));
            PQ_CHECK_RETURN_SUCCESS(ret);
            if (vpss_stt_info->height != PQ_WIDTH_720) {
                ret = hi_drv_pq_update_dei_alg(vpss_stt_info, &(vpss_api_output->dei_api_out));
                PQ_CHECK_RETURN_SUCCESS(ret);
            }
            ret = hi_drv_pq_update_db_alg(vpss_stt_info, &(vpss_api_output->db_api_out));
            PQ_CHECK_RETURN_SUCCESS(ret);

            ret = hi_drv_pq_update_dm_alg(vpss_stt_info, &(vpss_api_output->dm_api_out));
            PQ_CHECK_RETURN_SUCCESS(ret);

            drv_pq_vpss_update_buffer(VPSS_BUF_OUT, vpss_stt_info->handle_id);
        } else {
            HI_WARN_PQ("input output not match,input:%d output:%d\n", g_pq_vpss_alg_index[vpss_stt_info->handle_id],
                       vpss_api_output->index);
            drv_pq_vpss_update_buffer(VPSS_BUF_OUT, vpss_stt_info->handle_id);
            goto UPDATE_VPSS_ALG;
        }
    }
#endif
    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_VPSS_ALG] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

#ifdef PQ_PDM_SUPPORT
static hi_s32 drv_pq_get_flash_pq_bin(pq_bin_param *pq_param)
{
    hi_s32 ret;
    pq_file_header *pq_file_head = HI_NULL;
    hi_u32 check_size;
    hi_u32 check_pos;
    hi_u32 check_sum = 0;
    hi_u32 pq_len = 0;
    hi_u8 *pq_addr = HI_NULL;

    hi_pdm_export_func *pdm_funcs = HI_NULL;

    pq_bin_param *pq_param_tmp = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(pq_param);

    memset(pq_param, 0x0, sizeof(pq_bin_param));

    /* get pq bin from pdm */
    ret = osal_exportfunc_get(HI_ID_PDM, (hi_void **)(&pdm_funcs));
    if (ret != HI_SUCCESS) {
        HI_FATAL_PQ("hi_drv_module_get_function failed!\n");
        return HI_FAILURE;
    }

    PQ_CHECK_NULL_PTR_RE_FAIL(pdm_funcs);
    PQ_CHECK_NULL_PTR_RE_FAIL(pdm_funcs->pdm_get_data);

    ret = pdm_funcs->pdm_get_data(PQ_DEF_NAME, &pq_addr, &pq_len);
    if (ret != HI_SUCCESS) {
        HI_WARN_PQ("pst_pdm_funcs->pdm_get_data fail, PQ bin param may not burned\r\n");
        goto PQ_RELEASE_RESERVE_MEM;
    }

    if (pq_addr == HI_NULL) {
        HI_WARN_PQ("pst_pdm_funcs->pdm_get_data get addr is null, PQ bin param may not burned\r\n");
        goto PQ_RELEASE_RESERVE_MEM;
    }

    pq_param_tmp = (pq_bin_param *)pq_addr;
    pq_file_head = &(pq_param_tmp->pq_file_header);

    check_pos = (hi_u8 *)(&(pq_param_tmp->coef)) - (hi_u8 *)pq_param_tmp;
    check_size = pq_file_head->param_size;

    if (check_size != sizeof(pq_bin_param)) {
        HI_INFO_PQ("check p_qbin size error, param size = %d,sizeof(pq_param) = %d\n",
                   check_size, sizeof(pq_bin_param));
        /* do not release in boot */
        goto PQ_RELEASE_RESERVE_MEM;
    }

    if (osal_strncmp(pq_file_head->version, sizeof(pq_file_head->version),
        PQ_VERSION, strlen(PQ_VERSION)) != 0) {
        HI_ERR_PQ("check p_qbin version[%s] error\n", PQ_VERSION);
        /* do not release in boot */
        goto PQ_RELEASE_RESERVE_MEM;
    }

    if (pq_comm_check_chip_name(pq_file_head->chip_name, sizeof(pq_file_head->chip_name)) != HI_SUCCESS) {
        HI_ERR_PQ("check p_qbin chip_name[%s] error\n", pq_file_head->chip_name);
        /* do not release in boot */
        goto PQ_RELEASE_RESERVE_MEM;
    }

    while (check_pos < check_size) {
        check_sum += *(hi_u8 *)(((hi_u8 *)pq_param_tmp) + check_pos);
        check_pos++;
    }

    if (check_sum != pq_file_head->file_check_sum) {
        HI_ERR_PQ("p_qbin checksum error,declare checksum = %u,calcsum = %u\r\n",
                  pq_file_head->file_check_sum, check_sum);
        goto PQ_RELEASE_RESERVE_MEM;
    }

    memcpy(pq_param, pq_param_tmp, sizeof(pq_bin_param));

    /* do not release in boot */
    pdm_funcs->pdm_release_reserve_mem(PQ_DEF_NAME);

    return HI_SUCCESS;

PQ_RELEASE_RESERVE_MEM:
    pdm_funcs->pdm_release_reserve_mem(PQ_DEF_NAME);
    return HI_FAILURE;
}
#endif

hi_s32 drv_pq_get_bin_phy_addr(hi_u64 *addr)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    *addr = g_pq_bin_buf.phy_addr;

    return HI_SUCCESS;
}

#ifdef PQ_PDM_SUPPORT
hi_s32 drv_pq_get_pic_setting(hi_void)
{
    hi_s32 ret;

    hi_pdm_export_func *pdm_funcs = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_PDM, (hi_void **)(&pdm_funcs));
    if (ret != HI_SUCCESS) {
        HI_WARN_PQ("hi_drv_module_get_function failed!\n");
        return HI_FAILURE;
    }

    PQ_CHECK_NULL_PTR_RE_FAIL(pdm_funcs);
    PQ_CHECK_NULL_PTR_RE_FAIL(pdm_funcs->pdm_get_disp_param);

    ret = pdm_funcs->pdm_get_disp_param(HI_DRV_DISPLAY_1, &g_sd_disp_param);
    if (ret != HI_SUCCESS) {
        HI_WARN_PQ("baseparam may not burned\r\n");
        return HI_FAILURE;
    }

    g_pq_param.sd_picture_setting.brightness = g_sd_disp_param.brightness;
    g_pq_param.sd_picture_setting.contrast = g_sd_disp_param.contrast;
    g_pq_param.sd_picture_setting.hue = g_sd_disp_param.hue_plus;
    g_pq_param.sd_picture_setting.saturation = g_sd_disp_param.saturation;
    g_pq_param.sd_picture_setting.wcg_temperature = g_sd_disp_param.wcg_temperature;

    g_pq_param.sd_video_setting.brightness = LEVEL2NUM(g_sd_disp_param.brightness);
    g_pq_param.sd_video_setting.contrast = LEVEL2NUM(g_sd_disp_param.contrast);
    g_pq_param.sd_video_setting.hue = LEVEL2NUM(g_sd_disp_param.hue_plus);
    g_pq_param.sd_video_setting.saturation = LEVEL2NUM(g_sd_disp_param.saturation);
    g_pq_param.sd_video_setting.wcg_temperature = LEVEL2NUM(g_sd_disp_param.wcg_temperature);

    ret = pdm_funcs->pdm_get_disp_param(HI_DRV_DISPLAY_0, &g_hd_disp_param);
    if (ret != HI_SUCCESS) {
        HI_WARN_PQ("baseparam may not burned\r\n");
        return HI_FAILURE;
    }

    g_pq_param.hd_picture_setting.brightness = g_hd_disp_param.brightness;
    g_pq_param.hd_picture_setting.contrast = g_hd_disp_param.contrast;
    g_pq_param.hd_picture_setting.hue = g_hd_disp_param.hue_plus;
    g_pq_param.hd_picture_setting.saturation = g_hd_disp_param.saturation;
    g_pq_param.hd_picture_setting.wcg_temperature = g_hd_disp_param.wcg_temperature;

    g_pq_param.hd_video_setting.brightness = LEVEL2NUM(g_hd_disp_param.brightness);
    g_pq_param.hd_video_setting.contrast = LEVEL2NUM(g_hd_disp_param.contrast);
    g_pq_param.hd_video_setting.hue = LEVEL2NUM(g_hd_disp_param.hue_plus);
    g_pq_param.hd_video_setting.saturation = LEVEL2NUM(g_hd_disp_param.saturation);
    g_pq_param.hd_video_setting.wcg_temperature = LEVEL2NUM(g_hd_disp_param.wcg_temperature);

    return HI_SUCCESS;
}
#endif

static hi_void drv_pq_set_default_zme_param(hi_bool default_code)
{
    if (PQ_FUNC_CALL(HI_PQ_MODULE_ZME, set_vdp_zme_default)) {
        GET_ALG_FUN(HI_PQ_MODULE_ZME)->set_vdp_zme_default(default_code);
    }

    return;
}

static hi_void drv_pq_set_default_alg_param(hi_bool default_code)
{
    hi_u32 alg = 0;
    hi_bool module_on = HI_FALSE;
    hi_s32 ret = HI_SUCCESS;

    for (alg = 0; alg < HI_PQ_MODULE_MAX; alg++) {
        module_on = (default_code == HI_TRUE) ? HI_FALSE : g_pq_param.module_on_off[alg];
        ret = drv_pq_set_module_en(alg, module_on);
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("[%u] [%u]drv_pq_set_module_en error!\n", alg, module_on);
        }
    }

    return;
}

hi_s32 hi_drv_pq_set_default_param(hi_bool default_code)
{
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    drv_pq_set_default_zme_param(default_code);

    if (g_is_default_param == default_code) {
        return HI_SUCCESS;
    }

    drv_pq_set_video_default(default_code);
    drv_pq_set_picture_default(default_code);
    drv_pq_set_default_alg_param(default_code);

    g_is_default_param = default_code;

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_default_param(hi_bool *default_code)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(default_code);

    *default_code = g_is_default_param;

    return HI_SUCCESS;
}

hi_s32 hi_drv_pq_get_gfx_zme_strategy(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_zme_in *gfx_zme_in,
                                      hi_drv_pq_gfx_zme_out *gfx_zme_out)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    start_time = hi_pq_sys_get_time_stamp_us();

    PQ_CHECK_OVER_RANGE_RE_FAIL(gfx_layer, HI_DRV_PQ_GFX_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_zme_out);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXZME, get_gfx_zme_strategy)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXZME)->get_gfx_zme_strategy(gfx_layer, gfx_zme_in, gfx_zme_out);
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_GET_GFX_ZME] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_set_vdp_hdr_cfg(hi_drv_pq_xdr_layer_id layer_id, hi_drv_pq_xdr_frame_info *xdr_frame_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_XDR_LAYER_ID_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(xdr_frame_info);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();

    if (PQ_FUNC_CALL(HI_PQ_MODULE_HDR, set_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_HDR)->set_vdp_hdr_cfg(layer_id, xdr_frame_info);
    }

    /* hdr process ACM use */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_ACM, get_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_ACM)->get_vdp_hdr_cfg();
    }

    /* hdr process DCI use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_DCI, get_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DCI)->get_vdp_hdr_cfg();
    }

    /* hdr process SHARPEN use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_SHARPNESS, get_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SHARPNESS)->get_vdp_hdr_cfg();
    }

    /* hdr process CLE use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_CLE, get_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_CLE)->get_vdp_hdr_cfg();
    }

    /* hdr process SR use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_SR, get_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SR)->get_vdp_hdr_cfg();
    }

    /* hdr process Decontour use checksum */
    if (PQ_FUNC_CALL(HI_PQ_MODULE_DC, get_vdp_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_DC)->get_vdp_hdr_cfg();
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_SET_VDP_HDR] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_set_gfx_hdr_cfg(hi_drv_pq_gfx_layer gfx_layer, hi_drv_pq_gfx_hdr_info *gfx_hdr_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;
    drv_pq_internal_gfx_hdr_info internal_gfx_hdr = {0};

    PQ_CHECK_OVER_RANGE_RE_FAIL(gfx_layer, HI_DRV_PQ_GFX_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(gfx_hdr_info);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();
 
    internal_gfx_hdr.src_hdr_type = gfx_hdr_info->src_hdr_type;
    internal_gfx_hdr.disp_hdr_type = gfx_hdr_info->disp_hdr_type;

    drv_pq_trans_csc(gfx_hdr_info->color_space_in, &(internal_gfx_hdr.color_space_in));
    drv_pq_trans_csc(gfx_hdr_info->color_space_out, &(internal_gfx_hdr.color_space_out));

    if (PQ_FUNC_CALL(HI_PQ_MODULE_GFXHDR, set_gfx_hdr_cfg)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_GFXHDR)->set_gfx_hdr_cfg(gfx_layer, &internal_gfx_hdr);
    }

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_SET_GFX_HDR] = (end_time > start_time) ? (end_time - start_time) : 0;
    return ret;
}

hi_s32 hi_drv_pq_get_hwc_hdr_cfg(hi_drv_pq_hwc_layer hwc_layer, hi_drv_pq_hwc_hdr_info* hwc_hdr_info,
                                 hi_drv_pq_hwc_hdr_cfg* hwc_hdr_cfg)

{
    hi_u64 start_time;
    hi_u64 end_time;
    PQ_CHECK_OVER_RANGE_RE_FAIL(hwc_layer, HI_DRV_PQ_HWC_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(hwc_hdr_info);
    PQ_CHECK_NULL_PTR_RE_FAIL(hwc_hdr_cfg);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);
    start_time = hi_pq_sys_get_time_stamp_us();

    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_GET_HWC_HDR] = (end_time > start_time) ? (end_time - start_time) : 0;
    return HI_SUCCESS;
}

hi_s32 hi_drv_pq_update_vdp_ai_result(hi_drv_pq_display_id display_id, hi_drv_pq_ai_result *ai_result)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    PQ_CHECK_OVER_RANGE_RE_FAIL(display_id, HI_DRV_PQ_DISPLAY_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(ai_result);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();
    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_VDP_AI] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 hi_drv_pq_update_vpss_ai_result(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_ai_result *ai_result)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64 start_time;
    hi_u64 end_time;

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_VPSS_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(ai_result);
    PQ_CHECK_INIT_RE_FAIL(g_pq_init_flag.is_pq_module_init);

    start_time = hi_pq_sys_get_time_stamp_us();
    end_time = hi_pq_sys_get_time_stamp_us();
    g_proc_intf_time_cnt[PQ_UPDATE_VPSS_AI] = (end_time > start_time) ? (end_time - start_time) : 0;

    return ret;
}

hi_s32 drv_pq_set_pq_status(pq_common_status *pq_status)
{
    unsigned long irq_flag = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(pq_status);

    DRV_PQ_LOCK(&irq_flag);
    memcpy(&g_pq_status, pq_status, sizeof(pq_common_status));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_pq_status(pq_common_status *pq_status)
{
    unsigned long irq_flag = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(pq_status);

    DRV_PQ_LOCK(&irq_flag);
    memcpy(pq_status, &g_pq_status, sizeof(pq_common_status));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_pq_bin_param(pq_bin_param *param)
{
    unsigned long irq_flag = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(param);

    DRV_PQ_LOCK(&irq_flag);
    memcpy(param, g_pq_bin_param, sizeof(pq_bin_param));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_pq_bin_param(pq_bin_param *param)
{
    unsigned long irq_flag = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(param);

    DRV_PQ_LOCK(&irq_flag);
    memcpy(g_pq_bin_param, param, sizeof(pq_bin_param));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_pq_param(hi_drv_pq_param *param)
{
    unsigned long irq_flag = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(param);

    DRV_PQ_LOCK(&irq_flag);
    memcpy(param, &g_pq_param, sizeof(hi_drv_pq_param));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_pq_param(hi_drv_pq_param *param)
{
    unsigned long irq_flag = 0;

    PQ_CHECK_NULL_PTR_RE_FAIL(param);

    DRV_PQ_LOCK(&irq_flag);
    memcpy(&g_pq_param, param, sizeof(hi_drv_pq_param));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_pq_bin_status(hi_bool *bin_status)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(bin_status);

    *bin_status = g_load_pq_bin;

    return HI_SUCCESS;
}

hi_s32 drv_pq_set_vpss_timming(hi_drv_pq_vpss_timming vpss_timming)
{
    unsigned long irq_flag = 0;

    DRV_PQ_LOCK(&irq_flag);
    memcpy(&g_pq_vpss_timming, &vpss_timming, sizeof(hi_drv_pq_vpss_timming));
    DRV_PQ_UNLOCK(&irq_flag);

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_vpss_timming(hi_drv_pq_vpss_timming *vpss_timming)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(vpss_timming);
    vpss_timming->width = g_image_width;
    vpss_timming->height = g_image_height;

    return HI_SUCCESS;
}

hi_s32 drv_pq_get_init_flag(hi_bool *flag)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(flag);

    *flag = g_pq_init_flag.is_pq_module_init;

    return HI_SUCCESS;
}

EXPORT_SYMBOL(hi_drv_pq_init);
EXPORT_SYMBOL(hi_drv_pq_deinit);
EXPORT_SYMBOL(hi_drv_pq_suspend);
EXPORT_SYMBOL(hi_drv_pq_resume);

EXPORT_SYMBOL(hi_drv_pq_update_vdp_alg);
EXPORT_SYMBOL(hi_drv_pq_update_vdp_stt_info);
EXPORT_SYMBOL(hi_drv_pq_get_vdp_csc_coef);
EXPORT_SYMBOL(hi_drv_pq_get_vdp_zme_strategy);
EXPORT_SYMBOL(hi_drv_pq_get_vdp_zme_coef);
EXPORT_SYMBOL(hi_drv_pq_set_vdp_hdr_cfg);
EXPORT_SYMBOL(hi_drv_pq_set_default_param);
EXPORT_SYMBOL(hi_drv_pq_update_vdp_ai_result);

EXPORT_SYMBOL(hi_drv_pq_init_vpss_alg);
EXPORT_SYMBOL(hi_drv_pq_update_vpss_alg);
EXPORT_SYMBOL(hi_drv_pq_update_vpss_stt_info);
EXPORT_SYMBOL(hi_drv_pq_get_vpsszme_coef);
EXPORT_SYMBOL(hi_drv_pq_set_vpss_hdr_cfg);
EXPORT_SYMBOL(hi_drv_pq_update_vpss_ai_result);

EXPORT_SYMBOL(hi_drv_pq_get_gfx_csc_coef);
EXPORT_SYMBOL(hi_drv_pq_get_gfx_zme_strategy);
EXPORT_SYMBOL(hi_drv_pq_set_gfx_hdr_cfg);

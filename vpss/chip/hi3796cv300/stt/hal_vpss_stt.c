/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_stt.c source file vpss stt
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_stt.h"
#include "hal_vpss_stt_ccclmad.h"
#include "hal_vpss_stt_diemad.h"
#include "hal_vpss_stt_diesad.h"
#include "hal_vpss_stt_global.h"
#include "hal_vpss_stt_megmv.h"
#include "hal_vpss_stt_memv.h"
#include "hal_vpss_stt_nrcnt.h"
#include "hal_vpss_stt_nrmad.h"
#include "hal_vpss_stt_dmcnt.h"
#include "hal_vpss_stt_prj.h"
#include "hal_vpss_stt_rgmv.h"
#include "vpss_policy.h"
#include "drv_vpss_instance.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool need_cccl;
    vpss_stt_global global_list;
    vpss_stt_global global_me_list;
    vpss_stt_ccclcnt cccl_list;
    vpss_stt_deimad dei_mad_list;
    vpss_stt_deisad dei_sad_list;
    vpss_stt_megmv me_gmv_list;
    vpss_stt_memv me_mv_list;
    vpss_nrcntinfo nr_cnt_list;
    vpss_nrmadinfo nr_mad_list;
    vpss_dmcnt_info dmcnt_list;
    vpss_stt_prj prj_list;
    vpss_stt_rgmv rgmv_list;
} vpss_stt_ctx;

static vpss_stt_cfg g_stt_cfg;

static hi_void vpss_stt_get_cccl_flag(vpss_stt_ctx *ctx, hi_drv_pixel_format *real_pixel_format, vpss_stt_attr *attr)
{
    hi_u32 cccl_support_w;
    hi_u32 cccl_support_h;
    hi_bool cccl_support = HI_FALSE;

    cccl_support = vpss_policy_support_cccl();
    vpss_policy_support_cccl_height_and_width(&cccl_support_w, &cccl_support_h);
    if ((cccl_support == HI_TRUE) && (attr->width <= cccl_support_w) && (attr->height <= cccl_support_h)) {
        ctx->need_cccl = HI_TRUE;

        if ((VPSS_POLICY_DATAFMT_420 == vpss_policy_get_data_fmt(attr->pixel_format))
            && VPSS_SUPPORT_CF_UPSAMP) {
            *real_pixel_format = HI_DRV_PIXEL_FMT_NV61_2X1;
        }
    } else {
        ctx->need_cccl = HI_FALSE;
    }

    return;
}

static hi_s32 vpss_stt_all_channal_init(vpss_stt_ctx *ctx, vpss_stt_comm_attr *comm_attr)
{
    hi_s32 ret, ret1, ret8, ret9, ret10, ret11;
    hi_s32 ret2 = HI_SUCCESS;
    hi_s32 ret3 = HI_SUCCESS;
    hi_s32 ret4 = HI_SUCCESS;
    hi_s32 ret5 = HI_SUCCESS;
    hi_s32 ret6 = HI_SUCCESS;
    hi_s32 ret7 = HI_SUCCESS;

    ret = vpss_stt_global_init(&ctx->global_list, STT_MODE_GLOBAL);
    ret1 = vpss_stt_global_init(&ctx->global_me_list, STT_MODE_ME);

    if (ctx->need_cccl == HI_TRUE) {
        ret2 = vpss_stt_cccl_cnt_init(&ctx->cccl_list, comm_attr);
    }

    if (comm_attr->interlace == HI_TRUE) {
        ret3 = vpss_stt_deimad_init(&ctx->dei_mad_list, comm_attr);
        ret4 = vpss_stt_deisad_init(&ctx->dei_sad_list, comm_attr);
    }

    if (comm_attr->width <= VPSS_WIDTH_FHD) {
        ret5 = vpss_stt_megmv_init(&ctx->me_gmv_list);
        ret6 = vpss_stt_memv_init(&ctx->me_mv_list, comm_attr);
        ret7 = vpss_stt_nrcnt_init(&ctx->nr_cnt_list, comm_attr);
    }

    ret8 = vpss_stt_nrmad_init(&ctx->nr_mad_list, comm_attr);
    ret9 = vpss_stt_dmcnt_init(&ctx->dmcnt_list, comm_attr);
    ret10 = vpss_stt_prj_init(&ctx->prj_list, comm_attr);
    ret11 = vpss_stt_rgmv_init(&ctx->rgmv_list, comm_attr);
    if ((ret != HI_SUCCESS) || (ret1 != HI_SUCCESS) || (ret2 != HI_SUCCESS) || (ret3 != HI_SUCCESS) ||
        (ret4 != HI_SUCCESS) || (ret5 != HI_SUCCESS) || (ret6 != HI_SUCCESS) || (ret7 != HI_SUCCESS) ||
        (ret8 != HI_SUCCESS) || (ret9 != HI_SUCCESS) || (ret10 != HI_SUCCESS) || (ret11 != HI_SUCCESS)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_stt_init(vpss_stt *stt, vpss_stt_attr *attr)
{
    hi_s32 ret;
    vpss_stt_ctx *ctx = HI_NULL;
    vpss_stt_comm_attr comm_attr;
    hi_drv_pixel_format real_pixel_format;

    if (stt->init == HI_TRUE) {
        vpss_stt_deinit(stt);
    }

    ctx = vpss_vmalloc(sizeof(vpss_stt_ctx));
    if (ctx == HI_NULL) {
        vpss_error("vpss malloc buffer failed, size = %d\n", sizeof(vpss_stt_ctx));
        return HI_FAILURE;
    }

    stt->contex = (hi_void *)ctx;
    memset(ctx, 0, sizeof(vpss_stt_ctx));

    if ((attr->width > VPSS_MAX_WIDTH_NO_PQ) || (attr->height > VPSS_MAX_HEIGHT_NO_PQ)) {
        stt->init = HI_FALSE;
        stt->complete_count = 0;
        return HI_SUCCESS;
    }

    real_pixel_format = attr->pixel_format;

    vpss_stt_get_cccl_flag(ctx, &real_pixel_format, attr);

    comm_attr.secure = attr->secure;
    comm_attr.interlace = attr->interlace;
    comm_attr.width = attr->width;
    comm_attr.height = attr->height;
    comm_attr.pixel_format = real_pixel_format;
    ret = vpss_stt_all_channal_init(ctx, &comm_attr);
    if (ret != HI_SUCCESS) {
        vpss_stt_deinit(stt);
        return HI_FAILURE;
    }

    stt->init = HI_TRUE;
    stt->complete_count = 0;
    stt->stt_attr = *attr;
    return HI_SUCCESS;
}

hi_s32 vpss_stt_deinit(vpss_stt *stt)
{
    vpss_stt_ctx *ctx = HI_NULL;

    if (stt->init == HI_FALSE) {
        /* keep free othre init success buffer */
        vpss_info("Vpss STT DeInit but not init.\n");
    }

    ctx = (vpss_stt_ctx *)stt->contex;

    if (ctx == HI_NULL) {
        vpss_info("can't run here.\n");
        return HI_FAILURE;
    }

    vpss_stt_global_deinit(&ctx->global_list);
    vpss_stt_global_deinit(&ctx->global_me_list);

    if (ctx->need_cccl == HI_TRUE) {
        vpss_stt_cccl_cnt_deinit(&ctx->cccl_list);
    }

    if (stt->stt_attr.interlace == HI_TRUE) {
        vpss_stt_deimad_deinit(&ctx->dei_mad_list);
        vpss_stt_deisad_deinit(&ctx->dei_sad_list);
    }

    if (stt->stt_attr.width <= VPSS_WIDTH_FHD) {
        vpss_stt_megmv_deinit(&ctx->me_gmv_list);
        vpss_stt_memv_deinit(&ctx->me_mv_list);
        vpss_stt_nrcnt_deinit(&ctx->nr_cnt_list);
    }

    vpss_stt_nrmad_deinit(&ctx->nr_mad_list);
    vpss_stt_dmcnt_deinit(&ctx->dmcnt_list);
    vpss_stt_prj_deinit(&ctx->prj_list);
    vpss_stt_rgmv_deinit(&ctx->rgmv_list);
    vpss_vfree(ctx);
    memset(stt, 0, sizeof(vpss_stt));
    return HI_SUCCESS;
}
hi_s32 vpss_stt_reset(vpss_stt *stt)
{
    vpss_stt_ctx *ctx;

    if (stt->init != HI_TRUE) {
        vpss_info("stt has not init.\n");
        return HI_FAILURE;
    }

    ctx = (vpss_stt_ctx *)stt->contex;
    vpss_stt_global_reset(&ctx->global_list);
    vpss_stt_global_reset(&ctx->global_me_list);

    if (ctx->need_cccl == HI_TRUE) {
        vpss_stt_cccl_cnt_reset(&ctx->cccl_list);
    }

    if (stt->stt_attr.interlace == HI_TRUE) {
        vpss_stt_deimad_reset(&ctx->dei_mad_list);
        vpss_stt_deisad_reset(&ctx->dei_sad_list);
    }

    if (stt->stt_attr.width <= VPSS_WIDTH_FHD) {
        vpss_stt_megmv_reset(&ctx->me_gmv_list);
        vpss_stt_memv_reset(&ctx->me_mv_list);
        vpss_stt_nrcnt_reset(&ctx->nr_cnt_list);
    }

    vpss_stt_nrmad_reset(&ctx->nr_mad_list);
    vpss_stt_dmcnt_reset(&ctx->dmcnt_list);
    vpss_stt_prj_reset(&ctx->prj_list);
    vpss_stt_rgmv_reset(&ctx->rgmv_list);
    return HI_SUCCESS;
}

hi_s32 vpss_stt_complete(vpss_stt *stt)
{
    vpss_stt_ctx *ctx;

    if (stt->init != HI_TRUE) {
        vpss_info("stt has not init.\n");
        return HI_SUCCESS;
    }

    ctx = (vpss_stt_ctx *)stt->contex;
    vpss_stt_global_complete(&ctx->global_list);
    vpss_stt_global_complete(&ctx->global_me_list);

    if (ctx->need_cccl == HI_TRUE) {
        vpss_stt_cccl_cnt_complete(&ctx->cccl_list);
    }

    if (stt->stt_attr.interlace == HI_TRUE) {
        vpss_stt_deimad_complete(&ctx->dei_mad_list);
        vpss_stt_deisad_complete(&ctx->dei_sad_list);
    }

    if (stt->stt_attr.width <= VPSS_WIDTH_FHD) {
        vpss_stt_megmv_complete(&ctx->me_gmv_list);
        vpss_stt_memv_complete(&ctx->me_mv_list);
        vpss_stt_nrcnt_complete(&ctx->nr_cnt_list);
    }

    vpss_stt_nrmad_complete(&ctx->nr_mad_list);
    vpss_stt_dmcnt_complete(&ctx->dmcnt_list);
    vpss_stt_prj_complete(&ctx->prj_list);
    vpss_stt_rgmv_complete(&ctx->rgmv_list);

    return HI_SUCCESS;
}

static hi_void vpss_stt_get_global_cfg(vpss_stt_ctx *stt_ctx, vpss_stt_cfg *stt_cfg)
{
    vpss_stt_global_cfg stt_global_cfg = { 0 };
    vpss_stt_global_cfg stt_global_me_cfg = { 0 };

    vpss_stt_global_get_info(&stt_ctx->global_list, &stt_global_cfg);
    vpss_stt_global_get_info(&stt_ctx->global_me_list, &stt_global_me_cfg);

    stt_cfg->global_cfg.r_stt_chn.phy_addr = stt_global_cfg.read_phy_addr;
    stt_cfg->global_cfg.r_stt_chn.vir_addr = stt_global_cfg.read_vir_addr;
    stt_cfg->global_cfg.r_stt_chn.size = stt_global_cfg.size;
    stt_cfg->global_cfg.w_stt_chn.phy_addr = stt_global_cfg.write_phy_addr;
    stt_cfg->global_cfg.w_stt_chn.vir_addr = stt_global_cfg.write_vir_addr;
    stt_cfg->global_cfg.w_stt_chn.size = stt_global_cfg.size;
    stt_cfg->global_cfg.w_me1_stt_chn.phy_addr = stt_global_me_cfg.read_phy_addr;
    stt_cfg->global_cfg.w_me1_stt_chn.vir_addr = stt_global_me_cfg.read_vir_addr;
    stt_cfg->global_cfg.w_me1_stt_chn.size = stt_global_me_cfg.size;
    stt_cfg->global_cfg.w_me2_stt_chn.phy_addr = stt_global_me_cfg.write_phy_addr;
    stt_cfg->global_cfg.w_me2_stt_chn.vir_addr = stt_global_me_cfg.write_vir_addr;
    stt_cfg->global_cfg.w_me2_stt_chn.size = stt_global_me_cfg.size;

    return;
}

static hi_void vpss_stt_get_cccl_cfg(vpss_stt_ctx *stt_ctx, vpss_stt_cfg *stt_cfg)
{
    vpss_stt_ccclcnt_cfg cccl_cnt_y_cfg = { 0 };
    vpss_stt_ccclcnt_cfg cccl_cnt_c_cfg = { 0 };

    if (stt_ctx->need_cccl == HI_TRUE) {
        vpss_stt_cccl_cnt_get_cfg(&stt_ctx->cccl_list, &cccl_cnt_y_cfg, &cccl_cnt_c_cfg);
    }

    stt_cfg->cccl_cfg.r_mad_y_chn.phy_addr = cccl_cnt_y_cfg.read_addr;
    stt_cfg->cccl_cfg.r_mad_y_chn.vir_addr = cccl_cnt_y_cfg.read_vir_addr;
    stt_cfg->cccl_cfg.r_mad_y_chn.width = cccl_cnt_y_cfg.width;
    stt_cfg->cccl_cfg.r_mad_y_chn.height = cccl_cnt_y_cfg.height;
    stt_cfg->cccl_cfg.r_mad_y_chn.stride = cccl_cnt_y_cfg.stride;
    stt_cfg->cccl_cfg.r_mad_y_chn.size = cccl_cnt_y_cfg.size;
    stt_cfg->cccl_cfg.w_mad_y_chn.phy_addr = cccl_cnt_y_cfg.write_addr;
    stt_cfg->cccl_cfg.w_mad_y_chn.vir_addr = cccl_cnt_y_cfg.write_vir_addr;
    stt_cfg->cccl_cfg.w_mad_y_chn.width = cccl_cnt_y_cfg.width;
    stt_cfg->cccl_cfg.w_mad_y_chn.height = cccl_cnt_y_cfg.height;
    stt_cfg->cccl_cfg.w_mad_y_chn.stride = cccl_cnt_y_cfg.stride;
    stt_cfg->cccl_cfg.w_mad_y_chn.size = cccl_cnt_y_cfg.size;

    stt_cfg->cccl_cfg.r_mad_c_chn.phy_addr = cccl_cnt_c_cfg.read_addr;
    stt_cfg->cccl_cfg.r_mad_c_chn.vir_addr = cccl_cnt_c_cfg.read_vir_addr;
    stt_cfg->cccl_cfg.r_mad_c_chn.width = cccl_cnt_c_cfg.width;
    stt_cfg->cccl_cfg.r_mad_c_chn.height = cccl_cnt_c_cfg.height;
    stt_cfg->cccl_cfg.r_mad_c_chn.stride = cccl_cnt_c_cfg.stride;
    stt_cfg->cccl_cfg.r_mad_c_chn.size = cccl_cnt_c_cfg.size;
    stt_cfg->cccl_cfg.w_mad_c_chn.phy_addr = cccl_cnt_c_cfg.write_addr;
    stt_cfg->cccl_cfg.w_mad_c_chn.vir_addr = cccl_cnt_c_cfg.write_vir_addr;
    stt_cfg->cccl_cfg.w_mad_c_chn.width = cccl_cnt_c_cfg.width;
    stt_cfg->cccl_cfg.w_mad_c_chn.height = cccl_cnt_c_cfg.height;
    stt_cfg->cccl_cfg.w_mad_c_chn.stride = cccl_cnt_c_cfg.stride;
    stt_cfg->cccl_cfg.w_mad_c_chn.size = cccl_cnt_c_cfg.size;

    return;
}

static hi_void vpss_stt_get_dei_cfg(vpss_stt *stt, vpss_stt_ctx *stt_ctx, vpss_stt_cfg *stt_cfg)
{
    vpss_stt_deimad_cfg dei_mad_cfg = { 0 };
    vpss_stt_deisad_cfg dei_sad_y_cfg = { 0 };
    vpss_stt_deisad_cfg dei_sad_c_cfg = { 0 };

    if (stt->stt_attr.interlace == HI_TRUE) {
        vpss_stt_deimad_get_cfg(&stt_ctx->dei_mad_list, &dei_mad_cfg);
        vpss_stt_deisad_get_cfg(&stt_ctx->dei_sad_list, &dei_sad_y_cfg, &dei_sad_c_cfg);
    }

    stt_cfg->dei_cfg.r_mad_chn.phy_addr = dei_mad_cfg.read_addr;
    stt_cfg->dei_cfg.r_mad_chn.vir_addr = dei_mad_cfg.read_vir_addr;
    stt_cfg->dei_cfg.r_mad_chn.width = dei_mad_cfg.width;
    stt_cfg->dei_cfg.r_mad_chn.height = dei_mad_cfg.height;
    stt_cfg->dei_cfg.r_mad_chn.stride = dei_mad_cfg.stride;
    stt_cfg->dei_cfg.r_mad_chn.size = dei_mad_cfg.size;
    stt_cfg->dei_cfg.w_mad_chn.phy_addr = dei_mad_cfg.write_addr;
    stt_cfg->dei_cfg.w_mad_chn.vir_addr = dei_mad_cfg.write_vir_addr;
    stt_cfg->dei_cfg.w_mad_chn.width = dei_mad_cfg.width;
    stt_cfg->dei_cfg.w_mad_chn.height = dei_mad_cfg.height;
    stt_cfg->dei_cfg.w_mad_chn.stride = dei_mad_cfg.stride;
    stt_cfg->dei_cfg.w_mad_chn.size = dei_mad_cfg.size;

    stt_cfg->dei_cfg.r_sad_y_chn.phy_addr = dei_sad_y_cfg.read_addr;
    stt_cfg->dei_cfg.r_sad_y_chn.vir_addr = dei_sad_y_cfg.read_vir_addr;
    stt_cfg->dei_cfg.r_sad_y_chn.width = dei_sad_y_cfg.width;
    stt_cfg->dei_cfg.r_sad_y_chn.height = dei_sad_y_cfg.height;
    stt_cfg->dei_cfg.r_sad_y_chn.stride = dei_sad_y_cfg.stride;
    stt_cfg->dei_cfg.r_sad_y_chn.size = dei_sad_y_cfg.size;
    stt_cfg->dei_cfg.w_sad_y_chn.phy_addr = dei_sad_y_cfg.write_addr;
    stt_cfg->dei_cfg.w_sad_y_chn.vir_addr = dei_sad_y_cfg.write_vir_addr;
    stt_cfg->dei_cfg.w_sad_y_chn.width = dei_sad_y_cfg.width;
    stt_cfg->dei_cfg.w_sad_y_chn.height = dei_sad_y_cfg.height;
    stt_cfg->dei_cfg.w_sad_y_chn.stride = dei_sad_y_cfg.stride;
    stt_cfg->dei_cfg.w_sad_y_chn.size = dei_sad_y_cfg.size;

    stt_cfg->dei_cfg.r_sad_c_chn.phy_addr = dei_sad_c_cfg.read_addr;
    stt_cfg->dei_cfg.r_sad_c_chn.vir_addr = dei_sad_c_cfg.read_vir_addr;
    stt_cfg->dei_cfg.r_sad_c_chn.width = dei_sad_c_cfg.width;
    stt_cfg->dei_cfg.r_sad_c_chn.height = dei_sad_c_cfg.height;
    stt_cfg->dei_cfg.r_sad_c_chn.stride = dei_sad_c_cfg.stride;
    stt_cfg->dei_cfg.r_sad_c_chn.size = dei_sad_c_cfg.size;
    stt_cfg->dei_cfg.w_sad_c_chn.phy_addr = dei_sad_c_cfg.write_addr;
    stt_cfg->dei_cfg.w_sad_c_chn.vir_addr = dei_sad_c_cfg.write_vir_addr;
    stt_cfg->dei_cfg.w_sad_c_chn.width = dei_sad_c_cfg.width;
    stt_cfg->dei_cfg.w_sad_c_chn.height = dei_sad_c_cfg.height;
    stt_cfg->dei_cfg.w_sad_c_chn.stride = dei_sad_c_cfg.stride;
    stt_cfg->dei_cfg.w_sad_c_chn.size = dei_sad_c_cfg.size;

    return;
}

static hi_void vpss_stt_get_dei_memv_cfg(vpss_stt_memv_cfg *memv_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->dei_cfg.r_me_mv_p1_chn.phy_addr = memv_cfg->p1_read_addr;
    stt_cfg->dei_cfg.r_me_mv_p1_chn.vir_addr = memv_cfg->p1_read_vir_addr;
    stt_cfg->dei_cfg.r_me_mv_p1_chn.width = memv_cfg->width;
    stt_cfg->dei_cfg.r_me_mv_p1_chn.height = memv_cfg->height;
    stt_cfg->dei_cfg.r_me_mv_p1_chn.stride = memv_cfg->stride;
    stt_cfg->dei_cfg.r_me_mv_p1_chn.size = memv_cfg->size;
    stt_cfg->dei_cfg.r_me_mv_p2_chn.phy_addr = memv_cfg->p2_read_addr;
    stt_cfg->dei_cfg.r_me_mv_p2_chn.vir_addr = memv_cfg->p2_read_vir_addr;
    stt_cfg->dei_cfg.r_me_mv_p2_chn.width = memv_cfg->width;
    stt_cfg->dei_cfg.r_me_mv_p2_chn.height = memv_cfg->height;
    stt_cfg->dei_cfg.r_me_mv_p2_chn.stride = memv_cfg->stride;
    stt_cfg->dei_cfg.r_me_mv_p2_chn.size = memv_cfg->size;
    stt_cfg->dei_cfg.r_me_mv_p3_chn.phy_addr = memv_cfg->p3_read_addr;
    stt_cfg->dei_cfg.r_me_mv_p3_chn.vir_addr = memv_cfg->p3_read_vir_addr;
    stt_cfg->dei_cfg.r_me_mv_p3_chn.width = memv_cfg->width;
    stt_cfg->dei_cfg.r_me_mv_p3_chn.height = memv_cfg->height;
    stt_cfg->dei_cfg.r_me_mv_p3_chn.stride = memv_cfg->stride;
    stt_cfg->dei_cfg.r_me_mv_p3_chn.size = memv_cfg->size;

    return;
}

static hi_void vpss_stt_get_me_for_nr_cfg(vpss_stt_memvfortnr_cfg *memv_for_tnr_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->me_cfg.w_me_for_nr_stt_chn.phy_addr = memv_for_tnr_cfg->write_addr;
    stt_cfg->me_cfg.w_me_for_nr_stt_chn.vir_addr = memv_for_tnr_cfg->write_vir_addr;
    stt_cfg->me_cfg.w_me_for_nr_stt_chn.width = memv_for_tnr_cfg->width;
    stt_cfg->me_cfg.w_me_for_nr_stt_chn.height = memv_for_tnr_cfg->height;
    stt_cfg->me_cfg.w_me_for_nr_stt_chn.stride = memv_for_tnr_cfg->stride;
    stt_cfg->me_cfg.w_me_for_nr_stt_chn.size = memv_for_tnr_cfg->size;
    stt_cfg->me_cfg.w_me_for_di_stt_chn.phy_addr = memv_for_tnr_cfg->read_addr;
    stt_cfg->me_cfg.w_me_for_di_stt_chn.vir_addr = memv_for_tnr_cfg->read_vir_addr;
    stt_cfg->me_cfg.w_me_for_di_stt_chn.width = memv_for_tnr_cfg->width;
    stt_cfg->me_cfg.w_me_for_di_stt_chn.height = memv_for_tnr_cfg->height;
    stt_cfg->me_cfg.w_me_for_di_stt_chn.stride = memv_for_tnr_cfg->stride;
    stt_cfg->me_cfg.w_me_for_di_stt_chn.size = memv_for_tnr_cfg->size;

    return;
}

static hi_void vpss_stt_get_me_cfg(vpss_stt_megmv_cfg *megmv_cfg, vpss_stt_memv_cfg *memv_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->me_cfg.rg_mv_p1_chn.phy_addr = megmv_cfg->read_addr;
    stt_cfg->me_cfg.rg_mv_p1_chn.vir_addr = megmv_cfg->read_vir_addr;
    stt_cfg->me_cfg.rg_mv_p1_chn.stride = megmv_cfg->stride;
    stt_cfg->me_cfg.rg_mv_p1_chn.size = megmv_cfg->size;
    stt_cfg->me_cfg.rg_mv_pr_chn.phy_addr = megmv_cfg->write_addr;
    stt_cfg->me_cfg.rg_mv_pr_chn.vir_addr = megmv_cfg->write_vir_addr;
    stt_cfg->me_cfg.rg_mv_pr_chn.stride = megmv_cfg->stride;
    stt_cfg->me_cfg.rg_mv_pr_chn.size = megmv_cfg->size;
    stt_cfg->me_cfg.wg_mv_chn.phy_addr = megmv_cfg->write_addr;
    stt_cfg->me_cfg.wg_mv_chn.vir_addr = megmv_cfg->write_vir_addr;
    stt_cfg->me_cfg.wg_mv_chn.stride = megmv_cfg->stride;
    stt_cfg->me_cfg.wg_mv_chn.size = megmv_cfg->size;
    stt_cfg->me_cfg.r_mv_p1_chn.phy_addr = memv_cfg->p1_read_addr;
    stt_cfg->me_cfg.r_mv_p1_chn.vir_addr = memv_cfg->p1_read_vir_addr;
    stt_cfg->me_cfg.r_mv_p1_chn.width = memv_cfg->width;
    stt_cfg->me_cfg.r_mv_p1_chn.height = memv_cfg->height;
    stt_cfg->me_cfg.r_mv_p1_chn.stride = memv_cfg->stride;
    stt_cfg->me_cfg.r_mv_p1_chn.size = memv_cfg->size;
    stt_cfg->me_cfg.r_mv_pr_chn.phy_addr = memv_cfg->cur_write_addr;
    stt_cfg->me_cfg.r_mv_pr_chn.vir_addr = memv_cfg->write_vir_addr;
    stt_cfg->me_cfg.r_mv_pr_chn.width = memv_cfg->width;
    stt_cfg->me_cfg.r_mv_pr_chn.height = memv_cfg->height;
    stt_cfg->me_cfg.r_mv_pr_chn.stride = memv_cfg->stride;
    stt_cfg->me_cfg.r_mv_pr_chn.size = memv_cfg->size;
    stt_cfg->me_cfg.w_mv_chn.phy_addr = memv_cfg->cur_write_addr;
    stt_cfg->me_cfg.w_mv_chn.vir_addr = memv_cfg->write_vir_addr;
    stt_cfg->me_cfg.w_mv_chn.width = memv_cfg->width;
    stt_cfg->me_cfg.w_mv_chn.height = memv_cfg->height;
    stt_cfg->me_cfg.w_mv_chn.stride = memv_cfg->stride;
    stt_cfg->me_cfg.w_mv_chn.size = memv_cfg->size;

    return;
}

static hi_void vpss_stt_get_me_rgmv_cfg(vpss_stt_rgmv_cfg *rgme_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->me_cfg.r_rgmv_cf_chn.phy_addr = rgme_cfg->cur_write_addr;
    stt_cfg->me_cfg.r_rgmv_cf_chn.vir_addr = rgme_cfg->cur_write_vir_addr;
    stt_cfg->me_cfg.r_rgmv_cf_chn.width = rgme_cfg->width;
    stt_cfg->me_cfg.r_rgmv_cf_chn.height = rgme_cfg->height;
    stt_cfg->me_cfg.r_rgmv_cf_chn.stride = rgme_cfg->stride;
    stt_cfg->me_cfg.r_rgmv_cf_chn.size = rgme_cfg->size;
    stt_cfg->me_cfg.r_rgmv_p1_chn.phy_addr = rgme_cfg->p1_read_addr;
    stt_cfg->me_cfg.r_rgmv_p1_chn.vir_addr = rgme_cfg->p1_read_vir_addr;
    stt_cfg->me_cfg.r_rgmv_p1_chn.width = rgme_cfg->width;
    stt_cfg->me_cfg.r_rgmv_p1_chn.height = rgme_cfg->height;
    stt_cfg->me_cfg.r_rgmv_p1_chn.stride = rgme_cfg->stride;
    stt_cfg->me_cfg.r_rgmv_p1_chn.size = rgme_cfg->size;
    stt_cfg->me_cfg.r_rgmv_p2_chn.phy_addr = rgme_cfg->p2_read_addr;
    stt_cfg->me_cfg.r_rgmv_p2_chn.vir_addr = rgme_cfg->p2_read_vir_addr;
    stt_cfg->me_cfg.r_rgmv_p2_chn.width = rgme_cfg->width;
    stt_cfg->me_cfg.r_rgmv_p2_chn.height = rgme_cfg->height;
    stt_cfg->me_cfg.r_rgmv_p2_chn.stride = rgme_cfg->stride;
    stt_cfg->me_cfg.r_rgmv_p2_chn.size = rgme_cfg->size;

    return;
}

static hi_void vpss_stt_get_nr_memvfortnr_cfg(vpss_stt_memvfortnr_cfg *memv_for_tnr_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->nr_cfg.r_memv_for_tnr_chn.phy_addr = memv_for_tnr_cfg->write_addr;
    stt_cfg->nr_cfg.r_memv_for_tnr_chn.vir_addr = memv_for_tnr_cfg->write_vir_addr;
    stt_cfg->nr_cfg.r_memv_for_tnr_chn.width = memv_for_tnr_cfg->width;
    stt_cfg->nr_cfg.r_memv_for_tnr_chn.height = memv_for_tnr_cfg->height;
    stt_cfg->nr_cfg.r_memv_for_tnr_chn.stride = memv_for_tnr_cfg->stride;
    stt_cfg->nr_cfg.r_memv_for_tnr_chn.size = memv_for_tnr_cfg->size;

    return;
}

static hi_void vpss_stt_get_nr_cfg(vpss_nrcntcfg *nr_cnt_cfg, vpss_nrmadcfg *nr_mad_cfg,
                                   vpss_stt_rgmv_cfg *rgme_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->nr_cfg.r_cnt_chn.phy_addr = nr_cnt_cfg->read_addr;
    stt_cfg->nr_cfg.r_cnt_chn.vir_addr = nr_cnt_cfg->read_vir_addr;
    stt_cfg->nr_cfg.r_cnt_chn.width = nr_cnt_cfg->width;
    stt_cfg->nr_cfg.r_cnt_chn.height = nr_cnt_cfg->height;
    stt_cfg->nr_cfg.r_cnt_chn.stride = nr_cnt_cfg->u32stride;
    stt_cfg->nr_cfg.r_cnt_chn.size = nr_cnt_cfg->size;
    stt_cfg->nr_cfg.w_cnt_chn.phy_addr = nr_cnt_cfg->write_addr;
    stt_cfg->nr_cfg.w_cnt_chn.vir_addr = nr_cnt_cfg->write_vir_addr;
    stt_cfg->nr_cfg.w_cnt_chn.width = nr_cnt_cfg->width;
    stt_cfg->nr_cfg.w_cnt_chn.height = nr_cnt_cfg->height;
    stt_cfg->nr_cfg.w_cnt_chn.stride = nr_cnt_cfg->u32stride;
    stt_cfg->nr_cfg.w_cnt_chn.size = nr_cnt_cfg->size;
    stt_cfg->nr_cfg.r_tnr_mad_chn.phy_addr = nr_mad_cfg->tnr_read_addr;
    stt_cfg->nr_cfg.r_tnr_mad_chn.vir_addr = nr_mad_cfg->tnr_read_vir_addr;
    stt_cfg->nr_cfg.r_tnr_mad_chn.width = nr_mad_cfg->width;
    stt_cfg->nr_cfg.r_tnr_mad_chn.height = nr_mad_cfg->height;
    stt_cfg->nr_cfg.r_tnr_mad_chn.stride = nr_mad_cfg->u32stride;
    stt_cfg->nr_cfg.r_tnr_mad_chn.size = nr_mad_cfg->size;
    stt_cfg->nr_cfg.r_snr_mad_chn.phy_addr = nr_mad_cfg->snr_read_addr;
    stt_cfg->nr_cfg.r_snr_mad_chn.vir_addr = nr_mad_cfg->snr_read_vir_addr;
    stt_cfg->nr_cfg.r_snr_mad_chn.width = nr_mad_cfg->width;
    stt_cfg->nr_cfg.r_snr_mad_chn.height = nr_mad_cfg->height;
    stt_cfg->nr_cfg.r_snr_mad_chn.stride = nr_mad_cfg->u32stride;
    stt_cfg->nr_cfg.r_snr_mad_chn.size = nr_mad_cfg->size;
    stt_cfg->nr_cfg.w_mad_chn.phy_addr = nr_mad_cfg->tnr_write_addr;
    stt_cfg->nr_cfg.w_mad_chn.vir_addr = nr_mad_cfg->write_vir_addr;
    stt_cfg->nr_cfg.w_mad_chn.width = nr_mad_cfg->width;
    stt_cfg->nr_cfg.w_mad_chn.height = nr_mad_cfg->height;
    stt_cfg->nr_cfg.w_mad_chn.stride = nr_mad_cfg->u32stride;
    stt_cfg->nr_cfg.w_mad_chn.size = nr_mad_cfg->size;
    stt_cfg->nr_cfg.r_rgmv_p1_chn.phy_addr = rgme_cfg->p1_read_addr;
    stt_cfg->nr_cfg.r_rgmv_p1_chn.vir_addr = rgme_cfg->p1_read_vir_addr;
    stt_cfg->nr_cfg.r_rgmv_p1_chn.width = rgme_cfg->width;
    stt_cfg->nr_cfg.r_rgmv_p1_chn.height = rgme_cfg->height;
    stt_cfg->nr_cfg.r_rgmv_p1_chn.stride = rgme_cfg->stride;
    stt_cfg->nr_cfg.r_rgmv_p1_chn.size = rgme_cfg->size;
    stt_cfg->nr_cfg.r_rgmv_p2_chn.phy_addr = rgme_cfg->p2_read_addr;
    stt_cfg->nr_cfg.r_rgmv_p2_chn.vir_addr = rgme_cfg->p2_read_vir_addr;
    stt_cfg->nr_cfg.r_rgmv_p2_chn.width = rgme_cfg->width;
    stt_cfg->nr_cfg.r_rgmv_p2_chn.height = rgme_cfg->height;
    stt_cfg->nr_cfg.r_rgmv_p2_chn.stride = rgme_cfg->stride;
    stt_cfg->nr_cfg.r_rgmv_p2_chn.size = rgme_cfg->size;

    return;
}

static hi_void vpss_stt_get_dmcnt_cfg(vpss_stt_ctx *stt_ctx, vpss_stt_cfg *stt_cfg)
{
    vpss_dmcnt_cfg dmcnt_cfg = { 0 };

    vpss_stt_dmcnt_get_info(&stt_ctx->dmcnt_list, &dmcnt_cfg);

    stt_cfg->dmcnt_cfg.r_cnt_chn.phy_addr = dmcnt_cfg.read_phy_addr;
    stt_cfg->dmcnt_cfg.r_cnt_chn.vir_addr = dmcnt_cfg.read_vir_addr;
    stt_cfg->dmcnt_cfg.r_cnt_chn.width = dmcnt_cfg.width;
    stt_cfg->dmcnt_cfg.r_cnt_chn.height = dmcnt_cfg.height;
    stt_cfg->dmcnt_cfg.r_cnt_chn.stride = dmcnt_cfg.stride;
    stt_cfg->dmcnt_cfg.r_cnt_chn.size = dmcnt_cfg.size;
    stt_cfg->dmcnt_cfg.w_cnt_chn.phy_addr = dmcnt_cfg.write_phy_addr;
    stt_cfg->dmcnt_cfg.w_cnt_chn.vir_addr = dmcnt_cfg.write_vir_addr;
    stt_cfg->dmcnt_cfg.w_cnt_chn.width = dmcnt_cfg.width;
    stt_cfg->dmcnt_cfg.w_cnt_chn.height = dmcnt_cfg.height;
    stt_cfg->dmcnt_cfg.w_cnt_chn.stride = dmcnt_cfg.stride;
    stt_cfg->dmcnt_cfg.w_cnt_chn.size = dmcnt_cfg.size;

    return;
}

static hi_void vpss_stt_get_rgme_cfg(vpss_stt_rgmv_cfg *rgme_cfg, vpss_stt_cfg *stt_cfg)
{
    stt_cfg->rgme_cfg.r_rgmv_p1_chn.phy_addr = rgme_cfg->p1_read_addr;
    stt_cfg->rgme_cfg.r_rgmv_p1_chn.vir_addr = rgme_cfg->p1_read_vir_addr;
    stt_cfg->rgme_cfg.r_rgmv_p1_chn.width = rgme_cfg->width;
    stt_cfg->rgme_cfg.r_rgmv_p1_chn.height = rgme_cfg->height;
    stt_cfg->rgme_cfg.r_rgmv_p1_chn.stride = rgme_cfg->stride;
    stt_cfg->rgme_cfg.r_rgmv_p1_chn.size = rgme_cfg->size;
    stt_cfg->rgme_cfg.r_rgmv_p2_chn.phy_addr = rgme_cfg->p2_read_addr;
    stt_cfg->rgme_cfg.r_rgmv_p2_chn.vir_addr = rgme_cfg->p2_read_vir_addr;
    stt_cfg->rgme_cfg.r_rgmv_p2_chn.width = rgme_cfg->width;
    stt_cfg->rgme_cfg.r_rgmv_p2_chn.height = rgme_cfg->height;
    stt_cfg->rgme_cfg.r_rgmv_p2_chn.stride = rgme_cfg->stride;
    stt_cfg->rgme_cfg.r_rgmv_p2_chn.size = rgme_cfg->size;
    stt_cfg->rgme_cfg.w_rgmv_chn.phy_addr = rgme_cfg->cur_write_addr;
    stt_cfg->rgme_cfg.w_rgmv_chn.vir_addr = rgme_cfg->cur_write_vir_addr;
    stt_cfg->rgme_cfg.w_rgmv_chn.width = rgme_cfg->width;
    stt_cfg->rgme_cfg.w_rgmv_chn.height = rgme_cfg->height;
    stt_cfg->rgme_cfg.w_rgmv_chn.stride = rgme_cfg->stride;
    stt_cfg->rgme_cfg.w_rgmv_chn.size = rgme_cfg->size;

    return;
}

static hi_void vpss_stt_get_rgme_prj_cfg(vpss_stt_ctx *stt_ctx, vpss_stt_cfg *stt_cfg)
{
    vpss_stt_prj_cfg prjh_cfg = { 0 };
    vpss_stt_prj_cfg prjv_cfg = { 0 };

    vpss_stt_prj_get_cfg(&stt_ctx->prj_list, &prjh_cfg, &prjv_cfg);

    stt_cfg->rgme_cfg.r_prjh_chn.phy_addr = prjh_cfg.read_addr;
    stt_cfg->rgme_cfg.r_prjh_chn.vir_addr = prjh_cfg.read_vir_addr;
    stt_cfg->rgme_cfg.r_prjh_chn.width = prjh_cfg.width;
    stt_cfg->rgme_cfg.r_prjh_chn.height = prjh_cfg.height;
    stt_cfg->rgme_cfg.r_prjh_chn.stride = prjh_cfg.stride;
    stt_cfg->rgme_cfg.r_prjh_chn.size = prjh_cfg.size;
    stt_cfg->rgme_cfg.w_prjh_chn.phy_addr = prjh_cfg.write_addr;
    stt_cfg->rgme_cfg.w_prjh_chn.vir_addr = prjh_cfg.write_vir_addr;
    stt_cfg->rgme_cfg.w_prjh_chn.width = prjh_cfg.width;
    stt_cfg->rgme_cfg.w_prjh_chn.height = prjh_cfg.height;
    stt_cfg->rgme_cfg.w_prjh_chn.stride = prjh_cfg.stride;
    stt_cfg->rgme_cfg.w_prjh_chn.size = prjh_cfg.size;

    stt_cfg->rgme_cfg.r_prjv_chn.phy_addr = prjv_cfg.read_addr;
    stt_cfg->rgme_cfg.r_prjv_chn.vir_addr = prjv_cfg.read_vir_addr;
    stt_cfg->rgme_cfg.r_prjv_chn.width = prjv_cfg.width;
    stt_cfg->rgme_cfg.r_prjv_chn.height = prjv_cfg.height;
    stt_cfg->rgme_cfg.r_prjv_chn.stride = prjv_cfg.stride;
    stt_cfg->rgme_cfg.r_prjv_chn.size = prjv_cfg.size;
    stt_cfg->rgme_cfg.w_prjv_chn.phy_addr = prjv_cfg.write_addr;
    stt_cfg->rgme_cfg.w_prjv_chn.vir_addr = prjv_cfg.write_vir_addr;
    stt_cfg->rgme_cfg.w_prjv_chn.width = prjv_cfg.width;
    stt_cfg->rgme_cfg.w_prjv_chn.height = prjv_cfg.height;
    stt_cfg->rgme_cfg.w_prjv_chn.stride = prjv_cfg.stride;
    stt_cfg->rgme_cfg.w_prjv_chn.size = prjv_cfg.size;

    return;
}

hi_s32 vpss_stt_get_cfg(vpss_stt *stt, vpss_stt_cfg *stt_cfg)
{
    vpss_stt_megmv_cfg megmv_cfg = { 0 };
    vpss_stt_memv_cfg memv_cfg = { 0 };
    vpss_stt_memvfortnr_cfg memv_for_tnr_cfg = { 0 };
    vpss_nrcntcfg nr_cnt_cfg = { 0 };
    vpss_nrmadcfg nr_mad_cfg = { 0 };
    vpss_stt_rgmv_cfg rgme_cfg = { 0 };
    vpss_stt_ctx *stt_ctx = HI_NULL;

    if (stt->init != HI_TRUE) {
        vpss_error("stt has not init.\n");
        return HI_FAILURE;
    }

    if (stt_cfg == HI_NULL) {
        vpss_error("para is null.\n");
        return HI_FAILURE;
    }

    memset(stt_cfg, 0, sizeof(vpss_stt_cfg));
    stt_ctx = (vpss_stt_ctx *)stt->contex;
    if (stt_ctx == HI_NULL) {
        vpss_error("para is null.\n");
        return HI_FAILURE;
    }
    /* get global  stt cfg */
    vpss_stt_get_global_cfg(stt_ctx, stt_cfg);
    /* get cccl cnt cfg */
    vpss_stt_get_cccl_cfg(stt_ctx, stt_cfg);

    if (stt->stt_attr.width <= VPSS_WIDTH_FHD) {
        vpss_stt_megmv_get_cfg(&stt_ctx->me_gmv_list, &megmv_cfg);
        vpss_stt_memv_get_cfg(&stt_ctx->me_mv_list, &memv_cfg);
        vpss_stt_memv_for_tnr_get_cfg(&stt_ctx->me_mv_list, &memv_for_tnr_cfg);
        vpss_stt_nrcnt_get_info(&stt_ctx->nr_cnt_list, &nr_cnt_cfg);
    }

    vpss_stt_nrmad_get_info(&stt_ctx->nr_mad_list, &nr_mad_cfg);
    vpss_stt_rgmv_get_cfg(&stt_ctx->rgmv_list, &rgme_cfg);
    /* get die  cfg */
    vpss_stt_get_dei_cfg(stt, stt_ctx, stt_cfg);
    /* get memv  cfg */
    vpss_stt_get_dei_memv_cfg(&memv_cfg, stt_cfg);
    /* get me cfg */
    vpss_stt_get_me_cfg(&megmv_cfg, &memv_cfg, stt_cfg);
    vpss_stt_get_me_rgmv_cfg(&rgme_cfg, stt_cfg);
    vpss_stt_get_me_for_nr_cfg(&memv_for_tnr_cfg, stt_cfg);
    /* get nr cfg */
    vpss_stt_get_nr_cfg(&nr_cnt_cfg, &nr_mad_cfg, &rgme_cfg, stt_cfg);
    vpss_stt_get_nr_memvfortnr_cfg(&memv_for_tnr_cfg, stt_cfg);
    /* dmcnt */
    vpss_stt_get_dmcnt_cfg(stt_ctx, stt_cfg);
    /* get rgme  cfg */
    vpss_stt_get_rgme_cfg(&rgme_cfg, stt_cfg);
    vpss_stt_get_rgme_prj_cfg(stt_ctx, stt_cfg);

    return HI_SUCCESS;
}

static hi_void convert_hal_stt_info(vpss_stt_channel_cfg *drv_stt, vpss_hal_stt_channel_cfg *hal_stt)
{
    hal_stt->phy_addr = drv_stt->phy_addr;
    hal_stt->vir_addr = drv_stt->vir_addr;
    hal_stt->width = drv_stt->width;
    hal_stt->height = drv_stt->height;
    hal_stt->stride = drv_stt->stride;
    hal_stt->size = drv_stt->size;

    return;
}

static hi_void vpss_stt_fill_cccl_info(vpss_instance *instance, vpss_hal_stt_cfg *hal_stt_cfg)
{
    hi_u32 cccl_support_w;
    hi_u32 cccl_support_h;
    hi_bool cccl_support = HI_FALSE;

    cccl_support = vpss_policy_support_cccl();
    vpss_policy_support_cccl_height_and_width(&cccl_support_w, &cccl_support_h);
    if ((cccl_support == HI_TRUE) && (instance->stream_info.width <= cccl_support_w) &&
        (instance->stream_info.height <= cccl_support_h)) {
        convert_hal_stt_info(&g_stt_cfg.cccl_cfg.r_mad_y_chn, &hal_stt_cfg->cccl_cfg.r_mad_y_chn);
        convert_hal_stt_info(&g_stt_cfg.cccl_cfg.r_mad_c_chn, &hal_stt_cfg->cccl_cfg.r_mad_c_chn);
        convert_hal_stt_info(&g_stt_cfg.cccl_cfg.w_mad_y_chn, &hal_stt_cfg->cccl_cfg.w_mad_y_chn);
        convert_hal_stt_info(&g_stt_cfg.cccl_cfg.w_mad_c_chn, &hal_stt_cfg->cccl_cfg.w_mad_c_chn);
    }

    return;
}

static hi_void vpss_stt_fill_dei_info(vpss_instance *instance, vpss_hal_stt_cfg *hal_stt_cfg)
{
    if (instance->stream_info.interlace == HI_FALSE) {
        return;
    }

    convert_hal_stt_info(&g_stt_cfg.dei_cfg.r_mad_chn, &hal_stt_cfg->dei_cfg.r_mad_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.r_me_mv_p1_chn, &hal_stt_cfg->dei_cfg.r_me_mv_p1_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.r_me_mv_p2_chn, &hal_stt_cfg->dei_cfg.r_me_mv_p2_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.r_me_mv_p3_chn, &hal_stt_cfg->dei_cfg.r_me_mv_p3_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.r_sad_c_chn, &hal_stt_cfg->dei_cfg.r_sad_c_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.r_sad_y_chn, &hal_stt_cfg->dei_cfg.r_sad_y_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.w_mad_chn, &hal_stt_cfg->dei_cfg.w_mad_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.w_sad_c_chn, &hal_stt_cfg->dei_cfg.w_sad_c_chn);
    convert_hal_stt_info(&g_stt_cfg.dei_cfg.w_sad_y_chn, &hal_stt_cfg->dei_cfg.w_sad_y_chn);

    return;
}

static hi_void vpss_stt_fill_me_info(vpss_hal_stt_cfg *hal_stt_cfg)
{
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_cur_chn, &hal_stt_cfg->me_cfg.r_cur_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.rg_mv_p1_chn, &hal_stt_cfg->me_cfg.rg_mv_p1_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.rg_mv_pr_chn, &hal_stt_cfg->me_cfg.rg_mv_pr_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_mv_p1_chn, &hal_stt_cfg->me_cfg.r_mv_p1_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_mv_pr_chn, &hal_stt_cfg->me_cfg.r_mv_pr_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_ref_chn, &hal_stt_cfg->me_cfg.r_ref_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_rgmv_cf_chn, &hal_stt_cfg->me_cfg.r_rgmv_cf_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_rgmv_p1_chn, &hal_stt_cfg->me_cfg.r_rgmv_p1_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.r_rgmv_p2_chn, &hal_stt_cfg->me_cfg.r_rgmv_p2_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.wg_mv_chn, &hal_stt_cfg->me_cfg.wg_mv_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.w_me_for_di_stt_chn, &hal_stt_cfg->me_cfg.w_me_for_di_stt_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.w_me_for_nr_stt_chn, &hal_stt_cfg->me_cfg.w_me_for_nr_stt_chn);
    convert_hal_stt_info(&g_stt_cfg.me_cfg.w_mv_chn, &hal_stt_cfg->me_cfg.w_mv_chn);

    return;
}

static hi_void vpss_stt_fill_nr_info(vpss_hal_stt_cfg *hal_stt_cfg)
{
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.r_cnt_chn, &hal_stt_cfg->nr_cfg.r_cnt_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.r_memv_for_tnr_chn, &hal_stt_cfg->nr_cfg.r_memv_for_tnr_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.r_rgmv_p1_chn, &hal_stt_cfg->nr_cfg.r_rgmv_p1_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.r_rgmv_p2_chn, &hal_stt_cfg->nr_cfg.r_rgmv_p2_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.r_snr_mad_chn, &hal_stt_cfg->nr_cfg.r_snr_mad_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.r_tnr_mad_chn, &hal_stt_cfg->nr_cfg.r_tnr_mad_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.w_cnt_chn, &hal_stt_cfg->nr_cfg.w_cnt_chn);
    convert_hal_stt_info(&g_stt_cfg.nr_cfg.w_mad_chn, &hal_stt_cfg->nr_cfg.w_mad_chn);

    return;
}

static hi_void vpss_stt_fill_dmcnt_info(vpss_hal_stt_cfg *hal_stt_cfg)
{
    convert_hal_stt_info(&g_stt_cfg.dmcnt_cfg.r_cnt_chn, &hal_stt_cfg->dmcnt_cfg.r_cnt_chn);
    convert_hal_stt_info(&g_stt_cfg.dmcnt_cfg.w_cnt_chn, &hal_stt_cfg->dmcnt_cfg.w_cnt_chn);

    return;
}

static hi_void vpss_stt_fill_rgme_info(vpss_hal_stt_cfg *hal_stt_cfg)
{
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.r_prjh_chn, &hal_stt_cfg->rgme_cfg.r_prjh_chn);
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.r_prjv_chn, &hal_stt_cfg->rgme_cfg.r_prjv_chn);
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.r_rgmv_p1_chn, &hal_stt_cfg->rgme_cfg.r_rgmv_p1_chn);
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.r_rgmv_p2_chn, &hal_stt_cfg->rgme_cfg.r_rgmv_p2_chn);
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.w_prjh_chn, &hal_stt_cfg->rgme_cfg.w_prjh_chn);
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.w_prjv_chn, &hal_stt_cfg->rgme_cfg.w_prjv_chn);
    convert_hal_stt_info(&g_stt_cfg.rgme_cfg.w_rgmv_chn, &hal_stt_cfg->rgme_cfg.w_rgmv_chn);

    return;
}

static hi_void vpss_stt_fill_global_info(vpss_hal_stt_cfg *hal_stt_cfg)
{
    convert_hal_stt_info(&g_stt_cfg.global_cfg.r_stt_chn, &hal_stt_cfg->global_cfg.r_stt_chn);
    convert_hal_stt_info(&g_stt_cfg.global_cfg.w_me1_stt_chn, &hal_stt_cfg->global_cfg.w_me1_stt_chn);
    convert_hal_stt_info(&g_stt_cfg.global_cfg.w_me2_stt_chn, &hal_stt_cfg->global_cfg.w_me2_stt_chn);
    convert_hal_stt_info(&g_stt_cfg.global_cfg.w_stt_chn, &hal_stt_cfg->global_cfg.w_stt_chn);

    return;
}

hi_s32 vpss_stt_fill_stt_info(hi_void *fill_instance, hi_drv_3d_eye_type type)
{
    hi_s32 ret;
    vpss_hal_info *hal_info = HI_NULL;
    vpss_stt *stt = HI_NULL;
    vpss_hal_stt_cfg *hal_stt_cfg = HI_NULL;
    vpss_instance *instance = HI_NULL;

    instance = (vpss_instance *)fill_instance;
    hal_info = &instance->hal_info;
    hal_stt_cfg = &hal_info->stt_cfg;
    stt = &instance->stt_list[type];

    ret = vpss_stt_get_cfg(stt, &g_stt_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("instance get stt cfg failed, type %d\n", type);
        return ret;
    }

    /* fill cccl cfg */
    vpss_stt_fill_cccl_info(instance, hal_stt_cfg);
    /* fill die cfg */
    vpss_stt_fill_dei_info(instance, hal_stt_cfg);
    /* fill me cfg */
    vpss_stt_fill_me_info(hal_stt_cfg);
    /* fill nr cfg */
    vpss_stt_fill_nr_info(hal_stt_cfg);
    /* fill dmcnt cfg */
    vpss_stt_fill_dmcnt_info(hal_stt_cfg);
    /* fill  rgme cfg */
    vpss_stt_fill_rgme_info(hal_stt_cfg);
    /* fill global cfg */
    vpss_stt_fill_global_info(hal_stt_cfg);

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



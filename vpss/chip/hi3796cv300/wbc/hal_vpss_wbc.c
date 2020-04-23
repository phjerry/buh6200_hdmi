/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hal_vpss_wbc.c source file vpss wbc
 * Author: zhangjunyu
 * Create: 2016/07/01
 */

#include "hal_vpss_wbc.h"
#include "hal_vpss_wbc_cccl.h"
#include "hal_vpss_wbc_die.h"
#include "hal_vpss_wbc_nrhdown.h"
#include "hal_vpss_wbc_nrvdown.h"
#include "hal_vpss_wbc_nrrfr.h"
#include "vpss_policy.h"
#include "drv_vpss_instance.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool need_cccl;
    vpss_wbc_ccclrfr cccl_list;
    vpss_wbc_deirfr dei_list;
    vpss_wbc_nrhdown nr_hdow_list;
    vpss_wbc_nrvdown nr_vdow_list;
    vpss_wbc_nrrfr nr_list;
} vpss_wbc_ctx;

hi_s32 vpss_wbc_init(vpss_wbc *wbc, vpss_wbc_attr *attr)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 cccl_support_w;
    hi_u32 cccl_support_h;
    hi_bool cccl_support = HI_FALSE;
    vpss_wbc_ctx *ctx;
    vpss_wbc_ccclrfr_attr cccl_attr;
    vpss_wbc_deirfr_attr dei_attr;
    vpss_wbc_nrhdown_attr nr_hdown_attr;
    vpss_wbc_nrvdown_attr nr_vdown_attr;
    vpss_wbc_nrrfr_attr nr_attr;
    hi_drv_pixel_format real_pixel_format;

    if (wbc->init == HI_TRUE) {
        vpss_wbc_deinit(wbc);
    }

    ctx = vpss_vmalloc(sizeof(vpss_wbc_ctx));
    if (ctx == HI_NULL) {
        vpss_error("vpss malloc buffer failed, size = %d\n", sizeof(vpss_wbc_ctx));
        return HI_FAILURE;
    }

    wbc->contex = ctx;
    memset(ctx, 0, sizeof(vpss_wbc_ctx));

    if ((attr->width > VPSS_MAX_WIDTH_NO_PQ) || (attr->height > VPSS_MAX_HEIGHT_NO_PQ)) {
        wbc->init = HI_FALSE;
        wbc->complete_count = 0;
        return HI_SUCCESS;
    }

    real_pixel_format = attr->pixel_format;
    cccl_support = vpss_policy_support_cccl();
    vpss_policy_support_cccl_height_and_width(&cccl_support_w, &cccl_support_h);
    if ((cccl_support == HI_TRUE) && (attr->width <= cccl_support_w) && (attr->height <= cccl_support_h)) {
        ctx->need_cccl = HI_TRUE;

        if ((VPSS_POLICY_DATAFMT_420 == vpss_policy_get_data_fmt(attr->pixel_format))
            && VPSS_SUPPORT_CF_UPSAMP) {
            real_pixel_format = HI_DRV_PIXEL_FMT_NV61_2X1;
        }
    } else {
        ctx->need_cccl = HI_FALSE;
    }

    cccl_attr.interlace = attr->interlace;
    cccl_attr.secure = attr->secure;
    cccl_attr.bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
    cccl_attr.pixel_format = real_pixel_format;
    cccl_attr.ref_mode = VPSS_WBC_CCCLRFR_REF_MODE_NULL;
    cccl_attr.mem_name = "vpss_cccl_buffer";
    cccl_attr.width = attr->width;
    cccl_attr.height = attr->height;
    nr_hdown_attr.interlace = attr->interlace;
    nr_hdown_attr.secure = attr->secure;
    nr_hdown_attr.bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    nr_hdown_attr.mode = (attr->interlace == HI_TRUE) ? VPSS_WBC_NRHDOWN_MODE_3FIELD : VPSS_WBC_NRHDOWN_MODE_NORMAL;
    nr_hdown_attr.pixel_format = real_pixel_format;
    nr_hdown_attr.ref_mode = VPSS_WBC_NRHDOWN_REF_MODE_NULL;
    nr_hdown_attr.mem_name = "vpss_nrhdown_buffer";
    nr_hdown_attr.width = attr->width;
    nr_hdown_attr.height = attr->height;
    nr_vdown_attr.interlace = attr->interlace;
    nr_vdown_attr.secure = attr->secure;
    nr_vdown_attr.bit_width = HI_DRV_PIXEL_BITWIDTH_8BIT;
    nr_vdown_attr.mode = (attr->interlace == HI_TRUE) ? VPSS_WBC_NRVDOWN_MODE_3FIELD : VPSS_WBC_NRVDOWN_MODE_NORMAL;
    nr_vdown_attr.pixel_format = real_pixel_format;
    nr_vdown_attr.ref_mode = VPSS_WBC_NRVDOWN_REF_MODE_NULL;
    nr_vdown_attr.mem_name = "vpss_nrvdown_buffer";
    nr_vdown_attr.width = attr->width;
    nr_vdown_attr.height = attr->height;
    nr_attr.interlace = attr->interlace;
    nr_attr.secure = attr->secure;
    nr_attr.bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
    nr_attr.mode = (attr->interlace == HI_TRUE) ? VPSS_WBC_NRRFR_MODE_4FIELD : VPSS_WBC_NRRFR_MODE_NORMAL;
    nr_attr.pixel_format = real_pixel_format;
    nr_attr.ref_mode = VPSS_WBC_NRRFR_REF_MODE_NULL;
    nr_attr.mem_name = "vpss_nr_buffer";
    nr_attr.width = attr->width;
    nr_attr.height = attr->height;
    vpss_policy_get_nr_frf_cmp_info(nr_attr.width, nr_attr.height, &nr_attr.cmp_info);

    if (ctx->need_cccl == HI_TRUE) {
        ret = vpss_wbc_ccclrfr_init(&ctx->cccl_list, &cccl_attr);
    }

    if (attr->interlace == HI_TRUE) {
        dei_attr.secure = attr->secure;
        dei_attr.bit_width = HI_DRV_PIXEL_BITWIDTH_10BIT;
        dei_attr.pixel_format = real_pixel_format;
        dei_attr.width = attr->width;
        dei_attr.height = attr->height;
        ret |= vpss_wbc_deirfr_init(&ctx->dei_list, &dei_attr);
    }

    if (attr->width <= VPSS_WIDTH_FHD) {
        ret |= vpss_wbc_nr_hdown_init(&ctx->nr_hdow_list, &nr_hdown_attr);
        ret |= vpss_wbc_nr_vdown_init(&ctx->nr_vdow_list, &nr_vdown_attr);
    }

    ret |= vpss_wbc_nrrfr_init(&ctx->nr_list, &nr_attr);
    if (ret != HI_SUCCESS) {
        vpss_wbc_deinit(wbc);
        return HI_FAILURE;
    }

    wbc->init = HI_TRUE;
    wbc->wbc_attr = *attr;
    wbc->complete_count = 0;
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_deinit(vpss_wbc *wbc)
{
    vpss_wbc_ctx *ctx = HI_NULL;

    if (wbc->init == HI_FALSE) {
        /* keep free othre init success buffer */
        vpss_info("Vpss WBC DeInit but not init.\n");
    }

    ctx = (vpss_wbc_ctx *)wbc->contex;

    if (ctx == HI_NULL) {
        vpss_info("can't run here.\n");
        return HI_FAILURE;
    }

    if (ctx->need_cccl == HI_TRUE) {
        vpss_wbc_ccclrfr_deinit(&ctx->cccl_list);
    }

    if (wbc->wbc_attr.interlace == HI_TRUE) {
        vpss_wbc_deirfr_deinit(&ctx->dei_list);
    }

    if (wbc->wbc_attr.width <= VPSS_WIDTH_FHD) {
        vpss_wbc_nr_hdown_deinit(&ctx->nr_hdow_list);
        vpss_wbc_nr_vdown_deinit(&ctx->nr_vdow_list);
    }

    vpss_wbc_nrrfr_deinit(&ctx->nr_list);
    vpss_vfree(ctx);
    memset(wbc, 0, sizeof(vpss_wbc));
    return HI_SUCCESS;
}

hi_s32 vpss_wbc_reset(vpss_wbc *wbc)
{
    vpss_wbc_ctx *ctx;

    if (wbc->init == HI_FALSE) {
        vpss_info("Vpss STT DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    ctx = (vpss_wbc_ctx *)wbc->contex;

    if (ctx == HI_NULL) {
        vpss_error("can't run here.\n");
        return HI_FAILURE;
    }

    if (ctx->need_cccl == HI_TRUE) {
        vpss_wbc_ccclrfr_reset(&ctx->cccl_list);
    }

    if (wbc->wbc_attr.interlace == HI_TRUE) {
        vpss_wbc_deirfr_reset(&ctx->dei_list);
    }

    if (wbc->wbc_attr.width <= VPSS_WIDTH_FHD) {
        vpss_wbc_nr_hdown_reset(&ctx->nr_hdow_list);
        vpss_wbc_nr_vdown_reset(&ctx->nr_vdow_list);
    }

    vpss_wbc_nrrfr_reset(&ctx->nr_list);
    wbc->complete_count = 0;
    return HI_SUCCESS;
}
hi_s32 vpss_wbc_complete(vpss_wbc *wbc)
{
    vpss_wbc_ctx *ctx;

    if (wbc->init == HI_FALSE) {
        vpss_warn("Vpss STT DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    ctx = (vpss_wbc_ctx *)wbc->contex;

    if (ctx == HI_NULL) {
        vpss_error("can't run here.\n");
        return HI_FAILURE;
    }

    if (ctx->need_cccl == HI_TRUE) {
        vpss_wbc_ccclrfr_complete(&ctx->cccl_list);
    }

    if (wbc->wbc_attr.interlace == HI_TRUE) {
        vpss_wbc_deirfr_complete_image(&ctx->dei_list);
    }

    if (wbc->wbc_attr.width <= VPSS_WIDTH_FHD) {
        vpss_wbc_nr_hdown_complete(&ctx->nr_hdow_list);
        vpss_wbc_nr_vdown_complete(&ctx->nr_vdow_list);
    }

    vpss_wbc_nrrfr_complete(&ctx->nr_list);
    wbc->complete_count++;
    return HI_SUCCESS;
}
hi_s32 vpss_wbc_get_cfg(vpss_wbc *wbc, vpss_wbc_cfg *wbc_cfg)
{
    vpss_wbc_ctx *ctx = HI_NULL;
    vpss_wbc_nr_cfg *wbc_nr_cfg = HI_NULL;
    vpss_wbc_cccl_cfg *wbc_cccl_cfg = HI_NULL;
    vpss_wbc_dei_cfg *wbc_dei_cfg = HI_NULL;
    vpss_wbc_hds_cfg *wbc_hds_cfg = HI_NULL;
    vpss_wbc_hvds_cfg *wbc_hvds_cfg = HI_NULL;
    hi_drv_vpss_video_frame *pstnr_ref[] = { HI_NULL, HI_NULL, HI_NULL };

    if (wbc->init == HI_FALSE) {
        vpss_warn("Vpss STT DeInit error(not init).\n");
        return HI_SUCCESS;
    }

    if (wbc_cfg == HI_NULL) {
        vpss_warn("Para null.\n");
        return HI_FAILURE;
    }

    ctx = (vpss_wbc_ctx *)wbc->contex;

    if (ctx == HI_NULL) {
        vpss_error("can't run here.\n");
        return HI_FAILURE;
    }

    memset(wbc_cfg, 0, sizeof(vpss_wbc_cfg));
    wbc_nr_cfg = &wbc_cfg->wbc_nr_cfg;
    wbc_cccl_cfg = &wbc_cfg->wbc_cccl_cfg;
    wbc_dei_cfg = &wbc_cfg->wbc_dei_cfg;
    wbc_hds_cfg = &wbc_cfg->wbc_hds_cfg;
    wbc_hvds_cfg = &wbc_cfg->wbc_hvds_cfg;

    if (ctx->need_cccl == HI_TRUE) {
        vpss_wbc_ccclrfr_get_wbc_info(&ctx->cccl_list, &wbc_cccl_cfg->w_frame);
        vpss_wbc_ccclrfr_get_pre_info(&ctx->cccl_list, &wbc_cccl_cfg->rp2_frame,
                                      &wbc_cccl_cfg->rp4_frame, &wbc_cccl_cfg->rp8_frame);
    }

    if (wbc->wbc_attr.width <= VPSS_WIDTH_FHD) {
        vpss_wbc_nr_hdown_get_wbc_info(&ctx->nr_hdow_list, &wbc_hds_cfg->w_frame);
        vpss_wbc_nr_hdown_get_ref_info(&ctx->nr_hdow_list, &wbc_hds_cfg->rp2_frame);
        vpss_wbc_nr_vdown_get_wbc_info(&ctx->nr_vdow_list, &wbc_hvds_cfg->w_frame);
        vpss_wbc_nr_vdown_get_ref_info(&ctx->nr_vdow_list, &wbc_hvds_cfg->rp2_frame);
    }

    vpss_wbc_nrrfr_get_wbc_info(&ctx->nr_list, &wbc_nr_cfg->w_frame);
    vpss_wbc_nrrfr_get_ref_info(&ctx->nr_list, pstnr_ref);

    if (wbc->wbc_attr.interlace == HI_TRUE) {
        vpss_wbc_deirfr_get_wbc_info(&ctx->dei_list, &wbc_dei_cfg->w_frame);
        vpss_wbc_deirfr_get_ref_info(&ctx->dei_list, &wbc_dei_cfg->r_frame);
        wbc_nr_cfg->rp1_frame = pstnr_ref[2]; /* 1,2:para */
        wbc_nr_cfg->rp2_frame = pstnr_ref[1];
        wbc_nr_cfg->rp3_frame = pstnr_ref[0];
    } else {
        wbc_nr_cfg->rp1_frame = pstnr_ref[0];
    }

    return HI_SUCCESS;
}

hi_s32 vpss_wbc_fill_wbc_frame_info(hi_void *fill_instance, hi_drv_3d_eye_type type)
{
    hi_s32 ret;
    unsigned long flags;
    hi_u32 cccl_support_w;
    hi_u32 cccl_support_h;
    hi_bool cccl_support = HI_FALSE;
    vpss_instance *instance;
    hi_drv_vpss_video_frame *cur_frame;
    vpss_wbc_cfg wbc_cfg = { 0 };
    vpss_hal_info *hal_info;
    vpss_hal_wbc_cfg *hal_wbc_cfg;
    vpss_wbc *wbc;
    instance = (vpss_instance *)fill_instance;
    hal_info = &instance->hal_info;
    hal_wbc_cfg = &hal_info->wbc_cfg;
    wbc = &instance->wbc_list[type];
    vpss_comm_down_spin(&(instance->src_spin), &flags);
    ret = vpss_src_get_process_image(&instance->src_list, &cur_frame);
    if (ret != HI_SUCCESS) {
        vpss_error("can't happen,error!\n");
        vpss_comm_up_spin(&(instance->src_spin), &flags);
        return HI_FAILURE;
    }

    vpss_comm_up_spin(&(instance->src_spin), &flags);
    ret = vpss_wbc_get_cfg(wbc, &wbc_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("inst getwbc cfg failed, eye %d\n", type);
        return ret;
    }

    cccl_support = vpss_policy_support_cccl();
    vpss_policy_support_cccl_height_and_width(&cccl_support_w, &cccl_support_h);
    /* fill cccl cfg */
    if ((cccl_support == HI_TRUE) &&
        (instance->stream_info.width <= cccl_support_w) &&
        (instance->stream_info.height <= cccl_support_h)) {
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_cccl_cfg.w_frame, &hal_wbc_cfg->wbc_cccl_cfg.w_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_cccl_cfg.rp2_frame, &hal_wbc_cfg->wbc_cccl_cfg.rp2_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_cccl_cfg.rp4_frame, &hal_wbc_cfg->wbc_cccl_cfg.rp4_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_cccl_cfg.rp8_frame, &hal_wbc_cfg->wbc_cccl_cfg.rp8_frame, type);
    }

    /* fill die cfg */
    if (instance->stream_info.interlace == HI_TRUE) {
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_dei_cfg.w_frame, &hal_wbc_cfg->wbc_dei_cfg.w_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_dei_cfg.r_frame, &hal_wbc_cfg->wbc_dei_cfg.r_frame, type);
    }

    if (instance->stream_info.width <= VPSS_WIDTH_FHD) {
        /* fill nrhdown cfg */
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_hds_cfg.w_frame, &hal_wbc_cfg->wbc_hds_cfg.w_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_hds_cfg.rp2_frame, &hal_wbc_cfg->wbc_hds_cfg.rp2_frame, type);
        /* fill nrvdown cfg */
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_hvds_cfg.w_frame, &hal_wbc_cfg->wbc_hvds_cfg.w_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_hvds_cfg.rp2_frame, &hal_wbc_cfg->wbc_hvds_cfg.rp2_frame, type);
    }

    /* fill nr cfg */
    if (instance->stream_info.interlace == HI_TRUE) {
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.w_frame, &hal_wbc_cfg->wbc_nr_cfg.w_frame, type);

        if (instance->wbc_list[0].complete_count == 1) { /* 1:first frame */
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp1_frame, &hal_wbc_cfg->wbc_nr_cfg.rp1_frame, type);
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp1_frame, &hal_wbc_cfg->wbc_nr_cfg.rp2_frame, type);
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp1_frame, &hal_wbc_cfg->wbc_nr_cfg.rp3_frame, type);
        } else if (instance->wbc_list[0].complete_count == 2) { /* 2:second frame */
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp1_frame, &hal_wbc_cfg->wbc_nr_cfg.rp1_frame, type);
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp2_frame, &hal_wbc_cfg->wbc_nr_cfg.rp2_frame, type);
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp2_frame, &hal_wbc_cfg->wbc_nr_cfg.rp3_frame, type);
        } else {
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp1_frame, &hal_wbc_cfg->wbc_nr_cfg.rp1_frame, type);
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp2_frame, &hal_wbc_cfg->wbc_nr_cfg.rp2_frame, type);
            vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp3_frame, &hal_wbc_cfg->wbc_nr_cfg.rp3_frame, type);
        }
    } else {
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.w_frame, &hal_wbc_cfg->wbc_nr_cfg.w_frame, type);
        vpss_inst_set_hal_frame_info(wbc_cfg.wbc_nr_cfg.rp1_frame, &hal_wbc_cfg->wbc_nr_cfg.rp1_frame, type);
    }

    if (wbc_cfg.wbc_nr_cfg.w_frame != HI_NULL) {
        wbc_cfg.wbc_nr_cfg.w_frame->comm_frame.frame_index = cur_frame->comm_frame.frame_index;
        wbc_cfg.wbc_nr_cfg.w_frame->comm_frame.pts = cur_frame->comm_frame.pts;
        wbc_cfg.wbc_nr_cfg.w_frame->comm_frame.repeat_first_field = cur_frame->comm_frame.repeat_first_field;
        wbc_cfg.wbc_nr_cfg.w_frame->comm_frame.field_mode = cur_frame->comm_frame.field_mode;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



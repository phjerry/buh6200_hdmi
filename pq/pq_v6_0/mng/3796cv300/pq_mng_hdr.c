/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hdr mng api
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/string.h>
#include "hi_type.h"
#include "pq_mng_hdr.h"
#include "pq_hal_comm.h"
#include "pq_hal_hdr.h"
#include "pq_hal_hdr_regset.h"
#ifdef PQ_HDR_COEF_GOLDEN
#include "pq_hal_hdr_cfg_golden.h"
#endif

static hi_bool g_sg_b_hdr_init_flag = HI_FALSE;

pq_hdr_coef_addr g_pq_vdp_hdr_coef_buf = { 0 }; /** vdp hdr coef buffer **/

drv_pq_mem_info g_pq_hdr_buf_addr;
pq_hdr_cfg_proc g_hdr_cfg_proc = { 0 };

static hi_void pq_vir_addr_distribute(pq_hdr_coef_addr *addr)
{
    addr->vir_addr[PQ_COEF_BUF_V0_HDR0] = addr->vir_addr[PQ_COEF_BUF_HDR_RESERVE] + COEF_SIZE_RESERVE;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR1] = addr->vir_addr[PQ_COEF_BUF_V0_HDR0] + COEF_SIZE_HDR0;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR2] = addr->vir_addr[PQ_COEF_BUF_V0_HDR1] + COEF_SIZE_HDR1;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR3] = addr->vir_addr[PQ_COEF_BUF_V0_HDR2] + COEF_SIZE_HDR2;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR4] = addr->vir_addr[PQ_COEF_BUF_V0_HDR3] + COEF_SIZE_HDR3;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR5] = addr->vir_addr[PQ_COEF_BUF_V0_HDR4] + COEF_SIZE_HDR4;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR6] = addr->vir_addr[PQ_COEF_BUF_V0_HDR5] + COEF_SIZE_HDR5;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR7] = addr->vir_addr[PQ_COEF_BUF_V0_HDR6] + COEF_SIZE_HDR6;
    addr->vir_addr[PQ_COEF_BUF_V0_HDR8] = addr->vir_addr[PQ_COEF_BUF_V0_HDR7] + COEF_SIZE_HDR7;
    addr->vir_addr[PQ_COEF_BUF_OM_DEGMM] = addr->vir_addr[PQ_COEF_BUF_V0_HDR8] + COEF_SIZE_HDR8;
    addr->vir_addr[PQ_COEF_BUF_OM_GMM] = addr->vir_addr[PQ_COEF_BUF_OM_DEGMM] + COEF_SIZE_OM_DEGMM;
    addr->vir_addr[PQ_COEF_BUF_OM_CM] = addr->vir_addr[PQ_COEF_BUF_OM_GMM] + COEF_SIZE_OM_GMM;
}

static hi_void pq_phy_addr_distribute(pq_hdr_coef_addr *addr)
{
    addr->phy_addr[PQ_COEF_BUF_V0_HDR0] = addr->phy_addr[PQ_COEF_BUF_HDR_RESERVE] + COEF_SIZE_RESERVE;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR1] = addr->phy_addr[PQ_COEF_BUF_V0_HDR0] + COEF_SIZE_HDR0;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR2] = addr->phy_addr[PQ_COEF_BUF_V0_HDR1] + COEF_SIZE_HDR1;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR3] = addr->phy_addr[PQ_COEF_BUF_V0_HDR2] + COEF_SIZE_HDR2;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR4] = addr->phy_addr[PQ_COEF_BUF_V0_HDR3] + COEF_SIZE_HDR3;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR5] = addr->phy_addr[PQ_COEF_BUF_V0_HDR4] + COEF_SIZE_HDR4;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR6] = addr->phy_addr[PQ_COEF_BUF_V0_HDR5] + COEF_SIZE_HDR5;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR7] = addr->phy_addr[PQ_COEF_BUF_V0_HDR6] + COEF_SIZE_HDR6;
    addr->phy_addr[PQ_COEF_BUF_V0_HDR8] = addr->phy_addr[PQ_COEF_BUF_V0_HDR7] + COEF_SIZE_HDR7;
    addr->phy_addr[PQ_COEF_BUF_OM_DEGMM] = addr->phy_addr[PQ_COEF_BUF_V0_HDR8] + COEF_SIZE_HDR8;
    addr->phy_addr[PQ_COEF_BUF_OM_GMM] = addr->phy_addr[PQ_COEF_BUF_OM_DEGMM] + COEF_SIZE_OM_DEGMM;
    addr->phy_addr[PQ_COEF_BUF_OM_CM] = addr->phy_addr[PQ_COEF_BUF_OM_GMM] + COEF_SIZE_OM_GMM;
}

hi_s32 pq_mng_init_hdr(pq_bin_param *param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (g_sg_b_hdr_init_flag == HI_TRUE) {
        return HI_SUCCESS;
    }

    attr.name = "pq_hdr_coef";
    attr.size = VDP_HDR_COEF_SIZE;
    attr.mode = OSAL_MMZ_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_pq_hdr_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_pq_hdr_buf_addr  malloc failed\n");
        g_sg_b_hdr_init_flag = HI_FALSE;

        return HI_FAILURE;
    }
    ret = pq_comm_mem_flush(&g_pq_hdr_buf_addr);
    if (ret != HI_SUCCESS) {
        goto PQ_HDR_DEINIT;
    }

    if (param == HI_NULL) {
        goto PQ_HDR_DEINIT;
    }

    ret = pq_hal_init_hdr(param, para_use_table_default);
    if (ret != HI_SUCCESS) {
        goto PQ_HDR_DEINIT;
    }

    g_pq_vdp_hdr_coef_buf.vir_addr[PQ_COEF_BUF_HDR_RESERVE] = g_pq_hdr_buf_addr.vir_addr;
    g_pq_vdp_hdr_coef_buf.phy_addr[PQ_COEF_BUF_HDR_RESERVE] = g_pq_hdr_buf_addr.phy_addr;

    pq_vir_addr_distribute(&g_pq_vdp_hdr_coef_buf);
    pq_phy_addr_distribute(&g_pq_vdp_hdr_coef_buf);
    ret = pq_cfg_distribute_addr(&g_pq_vdp_hdr_coef_buf);
    if (ret != HI_SUCCESS) {
        goto PQ_HDR_DEINIT;
    }

    g_sg_b_hdr_init_flag = HI_TRUE;

    return ret;

PQ_HDR_DEINIT : {
        if (g_pq_hdr_buf_addr.vir_addr != HI_NULL) {
            pq_comm_mem_free ((drv_pq_mem_info *)(&(g_pq_hdr_buf_addr)));
            g_pq_hdr_buf_addr.vir_addr = HI_NULL;
            g_pq_hdr_buf_addr.phy_addr = 0;
        }

        return HI_FAILURE;
    }
}

hi_s32 pq_mng_deinit_hdr(hi_void)
{
    if (g_sg_b_hdr_init_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* release hdr coefficient memory */
    if (g_pq_hdr_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free ((drv_pq_mem_info *)(&(g_pq_hdr_buf_addr)));
        g_pq_hdr_buf_addr.vir_addr = HI_NULL;
        g_pq_hdr_buf_addr.phy_addr = 0;
    }

    g_sg_b_hdr_init_flag = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_hdr_cfg(hi_drv_pq_xdr_layer_id layer_id,
                          hi_drv_pq_xdr_frame_info *xdr_frame_info)
{
    hi_s32 ret;
    pq_hal_hdr_cfg hdr_cfg = { 0 };

    if ((layer_id >= HI_DRV_PQ_XDR_LAYER_ID_MAX) || (xdr_frame_info == HI_NULL)) {
        return HI_FAILURE;
    }

    ret = pq_hal_get_hdrcfg(xdr_frame_info, &hdr_cfg);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifdef PQ_HDR_COEF_GOLDEN
    pq_hal_imap_tmap_cfg_golden(&hdr_cfg, 0);
    pq_hal_omap_cfg_golden(&hdr_cfg, 0);
    pq_hal_cfg_viraddr_context_golden(&hdr_cfg, 0);
#endif

    pq_hal_set_hdr_cfg(layer_id, &hdr_cfg);

    g_hdr_cfg_proc.layer_id = layer_id;
    g_hdr_cfg_proc.src_hdr_type = xdr_frame_info->src_hdr_type;
    g_hdr_cfg_proc.disp_hdr_type = xdr_frame_info->disp_hdr_type;
    g_hdr_cfg_proc.color_space_in = xdr_frame_info->color_space_in;
    g_hdr_cfg_proc.color_space_out = xdr_frame_info->color_space_out;
    g_hdr_cfg_proc.hdrv1_cm_en = hdr_cfg.cm_cfg.enable;
    g_hdr_cfg_proc.imap_en = hdr_cfg.imap_cfg.enable;
    g_hdr_cfg_proc.tmapv2_en = hdr_cfg.tmap_cfg.enable;
    g_hdr_cfg_proc.omap_en = hdr_cfg.omap_cfg.enable;

    return HI_SUCCESS;
}

hi_s32 pq_mng_tool_get_hdrpara_mode(hi_pq_hdr_para_mode *mode)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(mode);

    ret = pq_hal_tool_get_hdrpara_mode(mode);

    return ret;
}

hi_s32 pq_mng_tool_set_hdrpara_mode(hi_pq_hdr_para_mode *mode)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(mode);

    ret = pq_hal_tool_set_hdrpara_mode(mode);

    return ret;
}

hi_s32 pq_mng_tool_set_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_tmap);

    ret = pq_hal_tool_set_hdr_tmap(hdr_tmap);

    return ret;
}

hi_s32 pq_mng_tool_get_hdr_tmap(hi_pq_hdr_tmap *hdr_tmap)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_tmap);

    ret = pq_hal_tool_get_hdr_tmap(hdr_tmap);

    return ret;
}

hi_s32 pq_mng_tool_set_hdr_smap(hi_pq_hdr_smap *hdr_smap)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_smap);

    ret = pq_hal_tool_set_hdr_smap(hdr_smap);

    return ret;
}

hi_s32 pq_mng_tool_get_hdr_smap(hi_pq_hdr_smap *hdr_smap)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_smap);

    ret = pq_hal_tool_get_hdr_smap(hdr_smap);

    return ret;
}

static hi_s32 pq_mng_set_hdr_tm_curve(hi_pq_hdr_offset *pst_hdr_offset_para)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(pst_hdr_offset_para);

    PQ_CHECK_INIT_RE_FAIL(g_sg_b_hdr_init_flag);

    ret = pq_hal_set_hdr_tm_curve(pst_hdr_offset_para);

    return ret;
}

static hi_s32 pq_mng_set_hdr_csc_setting(hi_pq_image_param *pic_setting)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(pic_setting);

    PQ_CHECK_INIT_RE_FAIL(g_sg_b_hdr_init_flag);

    ret = pq_hal_set_hdr_csc_setting(pic_setting);

    return ret;
}

static hi_s32 pq_mng_set_hdr_offset(hi_pq_hdr_offset *hdr_offset_para)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_offset_para);

    PQ_CHECK_INIT_RE_FAIL(g_sg_b_hdr_init_flag);

    ret = pq_hal_set_hdr_offset(hdr_offset_para);

    return ret;
}

static hi_s32 pq_mng_set_hdr_dci_histgram(hi_u32 dci_out_width, hi_u32 dci_out_height,
                                          hi_pq_dci_histgram *dci_histgram)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(dci_histgram);

    PQ_CHECK_INIT_RE_FAIL(g_sg_b_hdr_init_flag);

    ret = pq_hal_set_hdrdci_histgram(dci_out_width, dci_out_height, dci_histgram);

    return ret;
}

static hi_s32 pq_mng_get_hdr_cfg_by_proc(pq_hdr_cfg_proc *hdr_cfg)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(hdr_cfg);
    memcpy(hdr_cfg, &g_hdr_cfg_proc, sizeof(pq_hdr_cfg_proc));

    return HI_SUCCESS;
}

static pq_alg_funcs g_hdr_funcs = {
    .init = pq_mng_init_hdr,
    .deinit = pq_mng_deinit_hdr,
    .set_vdp_hdr_cfg = pq_mng_set_hdr_cfg,
    .set_hdr_dci_histgram = pq_mng_set_hdr_dci_histgram,
    .set_hdr_csc_video_setting = pq_mng_set_hdr_csc_setting,
    .set_hdr_offset = pq_mng_set_hdr_offset,
    .set_hdr_tm_curve = pq_mng_set_hdr_tm_curve,
    .get_hdr_cfg_by_proc = pq_mng_get_hdr_cfg_by_proc,
};

hi_s32 pq_mng_register_hdr(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_HDR, type, PQ_BIN_ADAPT_SINGLE, "hdr", &g_hdr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_hdr(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_HDR);

    return ret;
}


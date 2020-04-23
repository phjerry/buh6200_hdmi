/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include <linux/string.h>

#include "pq_mng_vpsshdr.h"
#include "pq_hal_comm.h"
#include "drv_pq_mem.h"
#include "hi_osal.h"

static hi_bool g_sg_b_vpss_hdr_init_flag = HI_FALSE;
drv_pq_mem_info g_pq_vpss_hdr_buf_addr;
static pq_vpsshdr_cfg_proc g_pq_vpsshdr_cfg_proc[VPSS_HANDLE_NUM] = {0};

static hi_void pq_vpss_vir_addr_distribute(pq_vpsshdr_coef_addr *addr)
{
    addr->vir_addr[VPSS_HDR_COEF_BUF_HDR] = addr->vir_addr[VPSS_HDR_COEF_BUF_CH_VZME] + VPSS_COEF_SIZE_CH_VZME;
    addr->vir_addr[VPSS_HDR_COEF_BUF_HDR1] = addr->vir_addr[VPSS_HDR_COEF_BUF_HDR] + VPSS_COEF_SIZE_HDR;
    addr->vir_addr[VPSS_HDR_COEF_BUF_HDR2] = addr->vir_addr[VPSS_HDR_COEF_BUF_HDR1] + VPSS_COEF_SIZE_HDR1;
    addr->vir_addr[VPSS_HDR_COEF_BUF_HDR3] = addr->vir_addr[VPSS_HDR_COEF_BUF_HDR2] + VPSS_COEF_SIZE_HDR2;
    addr->vir_addr[VPSS_HDR_COEF_BUF_HDR4] = addr->vir_addr[VPSS_HDR_COEF_BUF_HDR3] + VPSS_COEF_SIZE_HDR3;
    addr->vir_addr[VPSS_HDR_COEF_BUF_HDR5] = addr->vir_addr[VPSS_HDR_COEF_BUF_HDR4] + VPSS_COEF_SIZE_HDR4;
}

static hi_void pq_vpss_phy_addr_distribute(pq_vpsshdr_coef_addr *addr)
{
    addr->phy_addr[VPSS_HDR_COEF_BUF_HDR] = addr->phy_addr[VPSS_HDR_COEF_BUF_CH_VZME] + VPSS_COEF_SIZE_CH_VZME;
    addr->phy_addr[VPSS_HDR_COEF_BUF_HDR1] = addr->phy_addr[VPSS_HDR_COEF_BUF_HDR] + VPSS_COEF_SIZE_HDR;
    addr->phy_addr[VPSS_HDR_COEF_BUF_HDR2] = addr->phy_addr[VPSS_HDR_COEF_BUF_HDR1] + VPSS_COEF_SIZE_HDR1;
    addr->phy_addr[VPSS_HDR_COEF_BUF_HDR3] = addr->phy_addr[VPSS_HDR_COEF_BUF_HDR2] + VPSS_COEF_SIZE_HDR2;
    addr->phy_addr[VPSS_HDR_COEF_BUF_HDR4] = addr->phy_addr[VPSS_HDR_COEF_BUF_HDR3] + VPSS_COEF_SIZE_HDR3;
    addr->phy_addr[VPSS_HDR_COEF_BUF_HDR5] = addr->phy_addr[VPSS_HDR_COEF_BUF_HDR4] + VPSS_COEF_SIZE_HDR4;
}

static hi_void pq_mng_vpsshdr_distribute_buff(hi_void)
{
    hi_u32 handle;

    for (handle = 0; handle < VPSS_HANDLE_NUM; handle++) {
        g_pq_vdp_vpss_hdr_coef_buf[handle].vir_addr[VPSS_HDR_COEF_BUF_CH_VZME] =
            g_pq_vpss_hdr_buf_addr.vir_addr + handle * VDP_VPSSHDR_COEF_SIZE;
        g_pq_vdp_vpss_hdr_coef_buf[handle].phy_addr[VPSS_HDR_COEF_BUF_CH_VZME] =
            g_pq_vpss_hdr_buf_addr.phy_addr + handle * VDP_VPSSHDR_COEF_SIZE;
        pq_vpss_vir_addr_distribute(&g_pq_vdp_vpss_hdr_coef_buf[handle]);
        pq_vpss_phy_addr_distribute(&g_pq_vdp_vpss_hdr_coef_buf[handle]);
    }
}

hi_s32 pq_mng_init_vpss_hdr(pq_bin_param *pq_param, hi_bool default_code)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (g_sg_b_vpss_hdr_init_flag == HI_TRUE) {
        return HI_SUCCESS;
    }

    attr.name = "pq_vpsshdr_coef";
    attr.size = VDP_VPSSHDR_COEF_SIZE * VPSS_HANDLE_NUM;
    attr.mode = OSAL_MMZ_TYPE; /* just smmu support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_pq_vpss_hdr_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_pq_vpss_hdr_buf_addr  malloc failed\n");
        g_sg_b_vpss_hdr_init_flag = HI_FALSE;

        return HI_FAILURE;
    }
    ret = pq_comm_mem_flush(&g_pq_vpss_hdr_buf_addr);
    if (ret != HI_SUCCESS) {
        goto PQ_HDR_DEINIT;
    }

    if (pq_param == HI_NULL) {
        goto PQ_HDR_DEINIT;
    }

    ret = pq_hal_init_vpsshdr(pq_param, default_code);
    if (ret != HI_SUCCESS) {
        goto PQ_HDR_DEINIT;
    }

    pq_mng_vpsshdr_distribute_buff();

    g_sg_b_vpss_hdr_init_flag = HI_TRUE;

    return ret;

PQ_HDR_DEINIT : {
        if (g_pq_vpss_hdr_buf_addr.vir_addr != HI_NULL) {
            pq_comm_mem_free ((drv_pq_mem_info *)(&(g_pq_vpss_hdr_buf_addr)));
            g_pq_vpss_hdr_buf_addr.vir_addr = HI_NULL;
            g_pq_vpss_hdr_buf_addr.phy_addr = 0;
        }

        return HI_FAILURE;
    }
}

hi_s32 pq_mng_deinit_vpss_hdr(hi_void)
{
    if (g_sg_b_vpss_hdr_init_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* release hdr coefficient memory */
    if (g_pq_vpss_hdr_buf_addr.vir_addr != HI_NULL) {
        pq_comm_mem_free ((drv_pq_mem_info *)(&(g_pq_vpss_hdr_buf_addr)));
        g_pq_vpss_hdr_buf_addr.vir_addr = HI_NULL;
        g_pq_vpss_hdr_buf_addr.phy_addr = 0;
    }

    g_sg_b_vpss_hdr_init_flag = HI_FALSE;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_vpsshdr_cfg(hi_drv_pq_vpss_layer layer_id, drv_pq_internal_xdr_frame_info *xdr_frame_info,
                              vpss_reg_type *vpss_reg)
{
    hi_s32 ret;
    pq_hal_vpsshdr_cfg hdr_cfg = { 0 };

    if ((layer_id >= HI_DRV_PQ_VPSS_LAYER_MAX) || (xdr_frame_info == HI_NULL)) {
        return HI_FAILURE;
    }

    ret = pq_hal_get_vpsshdr_cfg(xdr_frame_info, &hdr_cfg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get vpsshdr cfg is error!\n");
        return HI_FAILURE;
    }

    ret = pq_hal_set_vpsshdr_coef_addr(layer_id, &hdr_cfg, vpss_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set vpsshdr coef addr is error!\n");
        return ret;
    }

    ret = pq_hal_set_vpsshdr_cfg(layer_id, &hdr_cfg, vpss_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("set vpsshdr cfg is error!\n");
    }

    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].enable = HI_TRUE;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].src_hdr_type = xdr_frame_info->src_hdr_type;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].disp_hdr_type = xdr_frame_info->disp_hdr_type;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].color_space_in = xdr_frame_info->color_space_in;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].color_space_out = xdr_frame_info->color_space_out;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].hdrv1_cm_en = hdr_cfg.cm_cfg.enable;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].imap_en = hdr_cfg.imap_cfg.enable;
    g_pq_vpsshdr_cfg_proc[xdr_frame_info->handle].tmapv2_en = hdr_cfg.tmap_cfg.enable;

    return ret;
}

static hi_s32 pq_mng_get_vpsshdr_cfg_by_proc(hi_u32 handle, pq_vpsshdr_cfg_proc *vpsshdr_cfg)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(vpsshdr_cfg);
    memcpy(vpsshdr_cfg, &(g_pq_vpsshdr_cfg_proc[handle]), sizeof(pq_vpsshdr_cfg_proc));

    return HI_SUCCESS;
}

static pq_alg_funcs g_vpss_hdr_funcs = {
    .init = pq_mng_init_vpss_hdr,
    .deinit = pq_mng_deinit_vpss_hdr,
    .set_vpss_hdr_cfg = pq_mng_set_vpsshdr_cfg,
    .get_vpsshdr_cfg_by_proc = pq_mng_get_vpsshdr_cfg_by_proc,
};

hi_s32 pq_mng_register_vpss_hdr(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_VPSSHDR, type, PQ_BIN_ADAPT_SINGLE, "vpsshdr", &g_vpss_hdr_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_vpss_hdr(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_VPSSHDR);

    return ret;
}


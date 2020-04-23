/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq mng vpsszme
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_hal_comm.h"
#include "pq_hal_vpsszme.h"
#include "pq_mng_vpsszme.h"
#include "pq_mng_vpsszme_coef.h"

static pq_vpss_zme g_pq_vpsszme_info = { 0 };
drv_pq_mem_info g_pq_vpsszme_buf_addr;
pq_vpsszme_coef_addr g_pq_vpsszme_coef_buf[PQ_HAL_VPSSZME_COEF_MAX] = { 0 };

/* commonly used */
static hi_u64 pq_get_hlfir_coef_addr(pq_vpsszme_ratio *ratio)
{
    hi_u32 tmp_ratio;
    pq_hal_vpsszme_coef_ratio coef_ratio = PQ_HAL_VPSSZME_COEF_1;

    if (ratio->ratio_h == 0) {
        HI_ERR_PQ("ratio equal 0, error\n");
        ratio->ratio_h = 1;
    }

    tmp_ratio = PQ_VPSSZME_WIDTH_4096 * PQ_VPSSZME_WIDTH_4096 / ratio->ratio_h;

    if (tmp_ratio > PQ_VPSSZME_RATIO_4096) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_1;
    } else if (tmp_ratio == PQ_VPSSZME_RATIO_4096) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_E1;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_3072) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_075;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_2048) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_05;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_1365) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_033;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_1024) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_025;
    } else {
        coef_ratio = PQ_HAL_VPSSZME_COEF_1;
    }

    return g_pq_vpsszme_coef_buf[coef_ratio].phy_addr[PQ_VPSSZME_COEF_BUF_HZME];
}

static hi_u64 pq_get_vlfir_coef_addr(pq_vpsszme_ratio *ratio)
{
    hi_u32 tmp_ratio;
    pq_hal_vpsszme_coef_ratio coef_ratio = PQ_HAL_VPSSZME_COEF_1;

    if (ratio->ratio_v == 0) {
        HI_ERR_PQ("ratio equal 0, error\n");
        ratio->ratio_v = 1;
    }

    tmp_ratio = PQ_VPSSZME_WIDTH_4096 * PQ_VPSSZME_WIDTH_4096 / ratio->ratio_v;

    if (tmp_ratio > PQ_VPSSZME_RATIO_4096) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_1;
    } else if (tmp_ratio == PQ_VPSSZME_RATIO_4096) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_E1;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_3072) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_075;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_2048) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_05;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_1365) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_033;
    } else if (tmp_ratio >= PQ_VPSSZME_RATIO_1024) {
        coef_ratio = PQ_HAL_VPSSZME_COEF_025;
    } else {
        coef_ratio = PQ_HAL_VPSSZME_COEF_1;
    }

    return g_pq_vpsszme_coef_buf[coef_ratio].phy_addr[PQ_VPSSZME_COEF_BUF_VZME];
}

hi_s32 get_vpsszme_ratio(hi_drv_pq_vpss_layer layer_id, hi_drv_pq_vpsszme_in *zme_in, pq_vpsszme_ratio *ratio)
{
    PQ_CHECK_ZERO_RE_FAIL(zme_in->comm_info.zme_w_out);
    PQ_CHECK_ZERO_RE_FAIL(zme_in->comm_info.zme_h_out);

    if ((zme_in->comm_info.zme_w_in / zme_in->comm_info.zme_w_out >= PQ_HZME_MAX_RATIO) ||
        (zme_in->comm_info.zme_w_out / zme_in->comm_info.zme_w_in >= PQ_HZME_MAX_RATIO)) {
        HI_ERR_PQ("[vpss zme_w in:%d, out:%d] than 16 !\n",
                  zme_in->comm_info.zme_w_in, zme_in->comm_info.zme_w_out);
    }

    if ((zme_in->comm_info.zme_h_in / zme_in->comm_info.zme_h_out >= PQ_VZME_MAX_RATIO) ||
        (zme_in->comm_info.zme_h_out / zme_in->comm_info.zme_h_in >= PQ_VZME_MAX_RATIO)) {
        HI_ERR_PQ("[vpss zme_h in:%d, out:%d] than 16 !\n",
                  zme_in->comm_info.zme_h_in, zme_in->comm_info.zme_h_out);
    }

    /* when wide > 4096, overflow will occurs */
    ratio->ratio_h = ((zme_in->comm_info.zme_w_in / 2 * PQ_HZME_PRECISION) / /* 2 : times */
                      zme_in->comm_info.zme_w_out * 2) >> 8; /* 2 : times; 8 : ratio shift */
    ratio->ratio_v = zme_in->comm_info.zme_h_in * PQ_VZME_PRECISION / zme_in->comm_info.zme_h_out;

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_vpsszme_coef(hi_drv_pq_vpss_layer layer_id,
                               hi_drv_pq_vpsszme_in *zme_in,
                               hi_drv_pq_vpsszme_out *zme_out)
{
    hi_s32 ret;
    pq_vpsszme_ratio ratio = { 0 };

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_VPSS_LAYER_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);
    PQ_CHECK_INIT_RE_FAIL(g_pq_vpsszme_info.zme_init_flag);

    /* fit refer to input H and V */
    ret = get_vpsszme_ratio(layer_id, zme_in, &ratio);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get zme ratio failed \n");
        return HI_FAILURE;
    }
    /* hor address */
    zme_out->zme_coef_addr.zme_coef_hl_addr = pq_get_hlfir_coef_addr(&ratio);

    /* vert address */
    zme_out->zme_coef_addr.zme_coef_vl_addr = pq_get_vlfir_coef_addr(&ratio);

    return HI_SUCCESS;
}

static hi_void pq_vpsszme_hzme_addr_distribute(hi_void)
{
    pq_hal_vpsszme_coef_ratio ratio;

    for (ratio = PQ_HAL_VPSSZME_COEF_1; ratio < PQ_HAL_VPSSZME_COEF_MAX; ratio++) {
        g_pq_vpsszme_coef_buf[ratio].vir_addr[PQ_VPSSZME_COEF_BUF_HZME] = g_pq_vpsszme_buf_addr.vir_addr + ratio * VPSS_COEF_SIZE_HZME;
        g_pq_vpsszme_coef_buf[ratio].phy_addr[PQ_VPSSZME_COEF_BUF_HZME] = g_pq_vpsszme_buf_addr.phy_addr + ratio * VPSS_COEF_SIZE_HZME;
    }

    return;
}

static hi_void pq_vpsszme_vzme_addr_distribute(hi_void)
{
    pq_hal_vpsszme_coef_ratio ratio;
    hi_u32 vir_addr_offset = PQ_HAL_VPSSZME_COEF_MAX * VPSS_COEF_SIZE_HZME;
    hi_u64 phy_addr_offset = PQ_HAL_VPSSZME_COEF_MAX * VPSS_COEF_SIZE_HZME;

    for (ratio = PQ_HAL_VPSSZME_COEF_1; ratio < PQ_HAL_VPSSZME_COEF_MAX; ratio++) {
        g_pq_vpsszme_coef_buf[ratio].vir_addr[PQ_VPSSZME_COEF_BUF_VZME] = g_pq_vpsszme_buf_addr.vir_addr +
            vir_addr_offset + ratio * VPSS_COEF_SIZE_VZME;
        g_pq_vpsszme_coef_buf[ratio].phy_addr[PQ_VPSSZME_COEF_BUF_VZME] = g_pq_vpsszme_buf_addr.phy_addr +
            phy_addr_offset + ratio * VPSS_COEF_SIZE_VZME;
    }

    return;
}

static hi_void pq_mng_vpsszme_comn_init(hi_void)
{
    /* load zme coefficient into the memory */
    pq_hal_vpsszme_coef_ratio ratio;
    hi_u8 *coef_addr_h = HI_NULL;
    hi_u8 *coef_addr_v = HI_NULL;

    pq_vpsszme_hzme_addr_distribute();
    pq_vpsszme_vzme_addr_distribute();

    for (ratio = PQ_HAL_VPSSZME_COEF_1; ratio < PQ_HAL_VPSSZME_COEF_MAX; ratio++) {
        memcpy(g_vpsszme_hl_coef, g_pq_vpss_zme_coef[ratio][VPSSZME_COEF_8T17P_LH], sizeof(g_vpsszme_hl_coef));
        memcpy(g_vpsszme_hc_coef, g_pq_vpss_zme_coef[ratio][VPSSZME_COEF_8T17P_CH], sizeof(g_vpsszme_hc_coef));
        memcpy(g_vpsszme_vl_coef, g_pq_vpss_zme_coef[ratio][VPSSZME_COEF_6T17P_LV], sizeof(g_vpsszme_vl_coef));
        memcpy(g_vpsszme_vc_coef, g_pq_vpss_zme_coef[ratio][VPSSZME_COEF_6T17P_CV], sizeof(g_vpsszme_vc_coef));
        coef_addr_h = g_pq_vpsszme_coef_buf[ratio].vir_addr[PQ_VPSSZME_COEF_BUF_HZME];
        coef_addr_v = g_pq_vpsszme_coef_buf[ratio].vir_addr[PQ_VPSSZME_COEF_BUF_VZME];
        pq_hal_get_vpsszme_coef(0, coef_addr_h, coef_addr_v);
    }

    return;
}

hi_s32 pq_mng_init_vpss_zme(pq_bin_param *pq_param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (g_pq_vpsszme_info.zme_init_flag) {
        return HI_SUCCESS;
    }

    attr.name = "pq_vpsszme_coef";
    attr.size = VPSS_ZME_COEF_SIZE * PQ_HAL_VPSSZME_COEF_MAX;
    attr.mode =  OSAL_MMZ_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;

    ret = pq_comm_mem_alloc(&attr,  &g_pq_vpsszme_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get pq_vpsszme failed\n");
        g_pq_vpsszme_info.zme_init_flag = HI_FALSE;

        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_pq_vpsszme_buf_addr);
    if (ret != HI_SUCCESS) {
        goto PQ_VPSSZME_DEINIT;
    }

    /* load vpss zme coef; init vpss zme coef */
    pq_mng_vpsszme_comn_init();

    g_pq_vpsszme_info.zme_init_flag = HI_TRUE;

    return HI_SUCCESS;

PQ_VPSSZME_DEINIT :
    if (g_pq_vpsszme_buf_addr.vir_addr != HI_NULL) {
        ret = pq_comm_mem_free ((drv_pq_mem_info *)(&(g_pq_vpsszme_buf_addr)));
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("pq_comm_mem_free failed\n");
        }

        g_pq_vpsszme_buf_addr.vir_addr = HI_NULL;
        g_pq_vpsszme_buf_addr.phy_addr = 0;
    }

    g_pq_vpsszme_info.zme_init_flag = HI_FALSE;

    return HI_FAILURE;
}

hi_s32 pq_mng_deinit_vpss_zme(hi_void)
{
    hi_s32 ret;

    if (g_pq_vpsszme_info.zme_init_flag == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* 这里看下是否把buf的free和虚拟地址的单独来看，
    否则如果一个buf申请没有map虚拟地址的话,
    可能会导致少free一次，引用计数少一次-1 */
    if (g_pq_vpsszme_buf_addr.vir_addr != HI_NULL) {
        ret = pq_comm_mem_free ((drv_pq_mem_info *)(&(g_pq_vpsszme_buf_addr)));
        if (ret != HI_SUCCESS) {
            HI_ERR_PQ("pq_comm_mem_free failed\n");
        }

        g_pq_vpsszme_buf_addr.vir_addr = HI_NULL;
        g_pq_vpsszme_buf_addr.phy_addr = 0;
    }

    g_pq_vpsszme_info.zme_init_flag = HI_FALSE;

    return HI_SUCCESS;
}

static pq_alg_funcs g_vpsszme_funcs = {
    .init = pq_mng_init_vpss_zme,
    .deinit = pq_mng_deinit_vpss_zme,
    .get_vpsszme_coef = pq_mng_get_vpsszme_coef,
};

hi_s32 pq_mng_register_vpsszme(hi_void)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_VPSSZME, REG_TYPE_VPSS, PQ_BIN_ADAPT_SINGLE, "vpsszme", &g_vpsszme_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_vpsszme()
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_VPSSZME);

    return ret;
}


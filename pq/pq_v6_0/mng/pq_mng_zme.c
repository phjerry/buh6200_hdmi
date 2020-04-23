/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq mng zme
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_mng_zme.h"
#include "pq_hal_comm.h"
#include "pq_hal_zme.h"
#include "pq_hal_zme_set_coef.h"
#include "pq_mng_zme_coef.h"

extern hi_s16 g_zme_4k_hl_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_zme_4k_hc_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_zme_4k_vl_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_zme_4k_vc_coef[17][8]; /* 17:phase size, 8: tap size */

extern hi_s16 g_zme_8k_hl_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_zme_8k_hc_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_zme_8k_vl_coef[17][8]; /* 17:phase size, 8: tap size */
extern hi_s16 g_zme_8k_vc_coef[17][8]; /* 17:phase size, 8: tap size */

extern hi_s32 g_sr_lsth_coef_horz[1560]; /* 1560:SR coef size */
extern hi_s32 g_sr_lsth_coef[1560]; /* 1560:SR coef size */
extern hi_s32 g_sr_c_coef_horz[1560]; /* 1560:SR coef size */
extern hi_s32 g_sr_c_coef[1560]; /* 1560:SR coef size */
extern hi_s32 g_sr_tree_cluster[150 * 3 * 3]; /* 150 * 3 * 3:SR tree size */
extern hi_s32 g_sr_tree_reg_mtr[256 * 16 * 16]; /* 256 * 16 * 16:SR tree size */

static alg_video_zme g_sg_st_pq_video_vdp_zme_info = { 0 };

drv_pq_mem_info g_pq_zme_buf_addr;

pq_zme_coef_addr g_pq_vdp_zme_coef_buf[PQ_HAL_ZME_COEF_RATIO_MAX] = { 0 }; /** vdp hdr coef buffer **/

hi_bool g_pq_sr_enable = HI_TRUE;
static hi_u32 g_zme4k_reg = 0;
static hi_u32 g_sr4k_reg[PQ_LAYER_VID_MAX] = {
    0,
    0,
    0,
    0x1000 / 4, /* 4 is a number */
};
static hi_u32 g_sr8k_reg[PQ_LAYER_VID_MAX] = {
    0,
    0,
    0,
    0,
};

hi_s32 pq_mng_set_vdp_zme_mode(pq_zme_mode mode, hi_bool on_off)
{
    switch (mode) {
        case PQ_ZME_MODE_VDP_FIR: {
            g_sg_st_pq_video_vdp_zme_info.vdp_zme_fir_en = on_off;
            pq_hal_set_vdp_zme_fir_en(on_off);
            break;
        }
        case PQ_ZME_MODE_VDP_MED: {
            g_sg_st_pq_video_vdp_zme_info.vdp_zme_med_en = on_off;
            pq_hal_set_vdp_zme_med_en(on_off);
            break;
        }
        default:
            break;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_vdp_zme_mode(pq_zme_mode mode, hi_bool* on_off)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(on_off);

    switch (mode) {
        case PQ_ZME_MODE_VDP_FIR:
            *on_off = g_sg_st_pq_video_vdp_zme_info.vdp_zme_fir_en;
            break;
        case PQ_ZME_MODE_VDP_MED:
            *on_off = g_sg_st_pq_video_vdp_zme_info.vdp_zme_med_en;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

static hi_void pq_zme_vir_addr_distribute(pq_zme_coef_addr* addr)
{
    addr->vir_addr[PQ_ZME_COEF_BUF_8K_VZME] = addr->vir_addr[PQ_ZME_COEF_BUF_RESERVER] + COEF_SIZE_RESERVE;
    addr->vir_addr[PQ_ZME_COEF_BUF_8K_HZME] = addr->vir_addr[PQ_ZME_COEF_BUF_8K_VZME] + COEF_SIZE_8K_VZME;
    addr->vir_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_H] = addr->vir_addr[PQ_ZME_COEF_BUF_8K_HZME] + COEF_SIZE_8K_HZME;
    addr->vir_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_V] =
        addr->vir_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_H] + COEF_SIZE_XDP_2DSCALE_H;
    addr->vir_addr[PQ_ZME_COEF_BUF_4K_SR0] = addr->vir_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_V] + COEF_SIZE_XDP_2DSCALE_V;
    addr->vir_addr[PQ_ZME_COEF_BUF_8K_SR0] = addr->vir_addr[PQ_ZME_COEF_BUF_4K_SR0] + COEF_SIZE_4K_SR0;

    return;
}

static hi_void pq_zme_phy_addr_distribute(pq_zme_coef_addr* addr)
{
    addr->phy_addr[PQ_ZME_COEF_BUF_8K_VZME] = addr->phy_addr[PQ_ZME_COEF_BUF_RESERVER] + COEF_SIZE_RESERVE;
    addr->phy_addr[PQ_ZME_COEF_BUF_8K_HZME] = addr->phy_addr[PQ_ZME_COEF_BUF_8K_VZME] + COEF_SIZE_8K_VZME;
    addr->phy_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_H] = addr->phy_addr[PQ_ZME_COEF_BUF_8K_HZME] + COEF_SIZE_8K_HZME;
    addr->phy_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_V] =
        addr->phy_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_H] + COEF_SIZE_XDP_2DSCALE_H;
    addr->phy_addr[PQ_ZME_COEF_BUF_4K_SR0] = addr->phy_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_V] + COEF_SIZE_XDP_2DSCALE_V;
    addr->phy_addr[PQ_ZME_COEF_BUF_8K_SR0] = addr->phy_addr[PQ_ZME_COEF_BUF_4K_SR0] + COEF_SIZE_4K_SR0;

    return;
}

/* common zme coefficient memory intial; get a static address pointer;
   several zme modules can use one memory block to save coefficient.
 */
static hi_s32 pq_mng_vdp_zme_comn_init(hi_void)
{
    /* load zme coefficient into the memory */
    pq_hal_zme_coef_ratio ratio;
    hi_u32 offset_4kzme = g_zme4k_reg;
    hi_u32 offset_4ksr = g_sr4k_reg[PQ_LAYER_VID0];
    hi_u32 offset_8ksr = g_sr8k_reg[PQ_LAYER_VID0];
    hi_s32 ret;

    for (ratio = PQ_HAL_ZME_COEF_1; ratio < PQ_HAL_ZME_COEF_RATIO_MAX; ratio++) {
        g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_RESERVER] =
            g_pq_zme_buf_addr.vir_addr + ratio * VDP_ZME_COEF_SIZE;
        g_pq_vdp_zme_coef_buf[ratio].phy_addr[PQ_ZME_COEF_BUF_RESERVER] =
            g_pq_zme_buf_addr.phy_addr + ratio * VDP_ZME_COEF_SIZE;
        pq_zme_vir_addr_distribute(&g_pq_vdp_zme_coef_buf[ratio]);
        pq_zme_phy_addr_distribute(&g_pq_vdp_zme_coef_buf[ratio]);
    }

    for (ratio = PQ_HAL_ZME_COEF_1; ratio < PQ_HAL_ZME_COEF_RATIO_MAX; ratio++) {
        memcpy(g_zme_4k_hl_coef, g_pq_vdp_4kzme_coef[ratio][ZME_COEF_8T17P_LH], sizeof(g_zme_4k_hl_coef));
        memcpy(g_zme_4k_hc_coef, g_pq_vdp_4kzme_coef[ratio][ZME_COEF_8T17P_CH], sizeof(g_zme_4k_hc_coef));
        memcpy(g_zme_4k_vl_coef, g_pq_vdp_4kzme_coef[ratio][ZME_COEF_6T17P_LV], sizeof(g_zme_4k_vl_coef));
        memcpy(g_zme_4k_vc_coef, g_pq_vdp_4kzme_coef[ratio][ZME_COEF_6T17P_CV], sizeof(g_zme_4k_vc_coef));
        pq_hal_set_4kzme_coef(offset_4kzme, g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_H],
            g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_XDP_2DSCALE_V]);

        memcpy(g_zme_8k_vl_coef, g_pq_vdp_8kzme_coef[ratio][ZME_COEF_6T17P_LV], sizeof(g_zme_8k_vl_coef));
        memcpy(g_zme_8k_vc_coef, g_pq_vdp_8kzme_coef[ratio][ZME_COEF_6T17P_CV], sizeof(g_zme_8k_vc_coef));
        memcpy(g_zme_8k_hl_coef, g_pq_vdp_8kzme_coef[ratio][ZME_COEF_8T17P_LH], sizeof(g_zme_8k_hl_coef));
        memcpy(g_zme_8k_hc_coef, g_pq_vdp_8kzme_coef[ratio][ZME_COEF_8T17P_CH], sizeof(g_zme_8k_hc_coef));
        pq_hal_set_8kzme_coef(PQ_8KZME_TYPE_VID0, g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_8K_HZME],
            g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_8K_VZME]);

        memcpy(g_sr_lsth_coef_horz, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_lsth_coef_horz));
        memcpy(g_sr_lsth_coef, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_lsth_coef));
        memcpy(g_sr_c_coef_horz, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_c_coef_horz));
        memcpy(g_sr_c_coef, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_c_coef));
        memcpy(g_sr_tree_cluster, g_pq_hipp_tree_cluster, sizeof(g_sr_tree_cluster));
        memcpy(g_sr_tree_reg_mtr, g_pq_hipp_tree_regmtr, sizeof(g_sr_tree_reg_mtr));
        pq_hal_set_4ksr_coef(offset_4ksr, g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_4K_SR0]);

        memcpy(g_sr_lsth_coef_horz, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_lsth_coef_horz));
        memcpy(g_sr_lsth_coef, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_lsth_coef));
        memcpy(g_sr_c_coef_horz, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_c_coef_horz));
        memcpy(g_sr_c_coef, g_hipp_2dsr_filter2d_group12a_g5_rm, sizeof(g_sr_c_coef));
        memcpy(g_sr_tree_cluster, g_pq_hipp_tree_cluster, sizeof(g_sr_tree_cluster));
        memcpy(g_sr_tree_reg_mtr, g_pq_hipp_tree_regmtr, sizeof(g_sr_tree_reg_mtr));
        pq_hal_set_8ksr_coef(offset_8ksr, g_pq_vdp_zme_coef_buf[ratio].vir_addr[PQ_ZME_COEF_BUF_8K_SR0]);
    }

    /* filter mode and median filter */
    ret = pq_mng_set_vdp_zme_mode(PQ_ZME_MODE_VDP_FIR, HI_TRUE);
    PQ_CHECK_RETURN_SUCCESS(ret);
    ret = pq_mng_set_vdp_zme_mode(PQ_ZME_MODE_VDP_MED, HI_FALSE);

    return ret;
}

hi_s32 pq_mng_init_vdp_zme(pq_bin_param* pq_param, hi_bool para_use_table_default)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = { 0 };

    if (g_sg_st_pq_video_vdp_zme_info.zme_init_flag) {
        return HI_SUCCESS;
    }

    if (pq_param == HI_NULL) {
        g_sg_st_pq_video_vdp_zme_info.zme_init_flag = HI_FALSE;
        return HI_FAILURE;
    }

    attr.name = "pq_vdp_zme_coef";
    attr.size = VDP_ZME_COEF_SIZE * PQ_HAL_ZME_COEF_RATIO_MAX;
    attr.mode =  OSAL_MMZ_TYPE;
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;

    ret = pq_comm_mem_alloc(&attr,  &g_pq_zme_buf_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("get pq_zme failed\n");
        g_sg_st_pq_video_vdp_zme_info.zme_init_flag = HI_FALSE;

        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_pq_zme_buf_addr);
    if (ret != HI_SUCCESS) {
        goto PQ_ZME_DEINIT;
    }

    /* load vdp zme coef; init vdp zme coef */
    ret = pq_mng_vdp_zme_comn_init();
    if (ret != HI_SUCCESS) {
        goto PQ_ZME_DEINIT;
    }

    g_sg_st_pq_video_vdp_zme_info.zme_init_flag = HI_TRUE;
    g_sg_st_pq_video_vdp_zme_info.vdp_zme_fir_en = HI_TRUE;
    g_sg_st_pq_video_vdp_zme_info.vdp_zme_med_en = HI_FALSE;
    g_sg_st_pq_video_vdp_zme_info.zme_tun_mode = PQ_TUN_NORMAL;

    return HI_SUCCESS;

PQ_ZME_DEINIT : {
        if (g_pq_zme_buf_addr.vir_addr != HI_NULL) {
            pq_comm_mem_free ((drv_pq_mem_info*)(&(g_pq_zme_buf_addr)));
            g_pq_zme_buf_addr.vir_addr = HI_NULL;
            g_pq_zme_buf_addr.phy_addr = 0;
        }

        return HI_FAILURE;
    }
}

hi_s32 pq_mng_deinit_vdp_zme(hi_void)
{
    if (g_sg_st_pq_video_vdp_zme_info.zme_init_flag) {
        /* release vdp zme coefficient memory */
        if (g_pq_zme_buf_addr.vir_addr != HI_NULL) {
            pq_comm_mem_free(&g_pq_zme_buf_addr);
            g_pq_zme_buf_addr.vir_addr = HI_NULL;
        }

        g_sg_st_pq_video_vdp_zme_info.zme_init_flag = HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_vdp_zme_coef(hi_drv_pq_zme_coef_in zme_coef_in, hi_drv_pq_zme_coef_out* zme_coef_out)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_coef_out);

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_zme_coef_addr(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in* zme_in,
                                hi_drv_pq_zme_strategy_out* zme_out)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);

    ret = pq_hal_get_zme_coef_addr(layer_id, zme_in, zme_out);

    return ret;
}

hi_s32 pq_mng_get_vdp_zme_strategy(hi_drv_pq_zme_layer layer_id, hi_drv_pq_zme_strategy_in* zme_in,
                                   hi_drv_pq_zme_strategy_out* zme_out)
{
    hi_s32 ret;

    PQ_CHECK_OVER_RANGE_RE_FAIL(layer_id, HI_DRV_PQ_DISP_LAYER_ZME_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_in);
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_out);

    if (PQ_FUNC_CALL(HI_PQ_MODULE_SR, get_enable)) {
        ret = GET_ALG_FUN(HI_PQ_MODULE_SR)->get_enable(&g_pq_sr_enable);
        PQ_CHECK_RETURN_SUCCESS(ret);
    }

    ret = pq_hal_get_vdp_zme_strategy(layer_id, zme_in, zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_mng_get_zme_coef_addr(layer_id, zme_in, zme_out);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return ret;
}

hi_s32 pq_mng_set_zme_tun_mode(pq_tun_mode zme_tun_mode)
{
    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pq_video_vdp_zme_info.zme_init_flag);

    g_sg_st_pq_video_vdp_zme_info.zme_tun_mode = zme_tun_mode;

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_zme_tun_mode(pq_tun_mode* zme_tun_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(zme_tun_mode);
    *zme_tun_mode = g_sg_st_pq_video_vdp_zme_info.zme_tun_mode;

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_vdp_zme_default(hi_bool on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pq_video_vdp_zme_info.zme_init_flag);

    if (g_sg_st_pq_video_vdp_zme_info.zme_tun_mode == PQ_TUN_NORMAL) {
        pq_hal_set_vdp_zme_default(on_off);
    } else {
        pq_hal_set_vdp_zme_default(HI_FALSE); /* when debug rwzb no using */
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_vdp_hdcpen(hi_bool on_off)
{
    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pq_video_vdp_zme_info.zme_init_flag);

    pq_hal_set_vdp_hdcpen(on_off);

    return HI_SUCCESS;
}

hi_s32 pq_mng_set_proc_vdp_zme_strategy(hi_pq_proc_vdpzme_strategy proc_vdp_zme_strategy)
{
    PQ_CHECK_INIT_RE_FAIL(g_sg_st_pq_video_vdp_zme_info.zme_init_flag);

    pq_hal_set_proc_vdp_zme_strategy(proc_vdp_zme_strategy);

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_proc_vdp_zme_strategy(hi_pq_proc_get_vdpzme_strategy* proc_vdp_zme_strategy)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(proc_vdp_zme_strategy);

    pq_hal_get_proc_vdp_zme_strategy(proc_vdp_zme_strategy);

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_proc_vdp_zme_reso(hi_pq_proc_vdpzme_reso* proc_vdp_zme_reso)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(proc_vdp_zme_reso);

    pq_hal_get_proc_vdp_zme_reso(proc_vdp_zme_reso);

    return HI_SUCCESS;
}

static pq_alg_funcs g_zme_funcs = {
    .init = pq_mng_init_vdp_zme,
    .deinit = pq_mng_deinit_vdp_zme,
    .set_vdp_zme_mode = pq_mng_set_vdp_zme_mode,
    .get_vdp_zme_mode = pq_mng_get_vdp_zme_mode,
    .get_vdp_zme_coef = pq_mng_get_vdp_zme_coef,
    .get_vdp_zme_strategy = pq_mng_get_vdp_zme_strategy,
    .set_vdp_zme_default = pq_mng_set_vdp_zme_default,
    .set_vdp_hdcp_en = pq_mng_set_vdp_hdcpen,
    .set_proc_vdp_zme_strategy = pq_mng_set_proc_vdp_zme_strategy,
    .get_proc_vdp_zme_strategy = pq_mng_get_proc_vdp_zme_strategy,
    .get_proc_vdp_zme_reso = pq_mng_get_proc_vdp_zme_reso,
};

hi_s32 pq_mng_register_zme(pq_reg_type type)
{
    hi_s32 ret;

    ret = pq_comm_alg_register(HI_PQ_MODULE_ZME, type, PQ_BIN_ADAPT_SINGLE, "zme", &g_zme_funcs);

    return ret;
}

hi_s32 pq_mng_unregister_zme()
{
    hi_s32 ret;

    ret = pq_comm_alg_unregister(HI_PQ_MODULE_ZME);

    return ret;
}



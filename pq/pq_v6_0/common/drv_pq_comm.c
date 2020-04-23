/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq common define
 * Author: pq
 * Create: 2016-01-1
 */

#include <linux/fs.h>

#include "drv_pq_comm.h"
#include "hi_type.h"
#include "pq_hal_comm.h"

#define MIN_LEN(x, y) ((x) > (y) ? (y) : (x))

static pq_source_mode g_en_source_mode = PQ_SOURCE_MODE_NO;
static pq_output_mode g_en_output_mode = PQ_OUTPUT_MODE_NO;
static pq_alg_reg *g_alg_reg_module[HI_PQ_MODULE_MAX] = {NULL};
static hi_u8 g_moudle_ctrl[HI_PQ_MODULE_MAX][PQ_SOURCE_MODE_MAX][PQ_OUTPUT_MODE_MAX] = {0};

static drv_pq_mem_info g_pq_vdp_stt_buf = { 0 };

#ifdef PQ_PDM_SUPPORT
hi_s32 pq_comm_check_chip_name(hi_char *chip_name, hi_u32 len)
{
    hi_s32 ret;
    PQ_CHECK_NULL_PTR_RE_FAIL(chip_name);

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    ret = osal_strncmp(chip_name, len, "HI_CHIP_TYPE_HI3796C", strlen("HI_CHIP_TYPE_HI3798C"));
#else
    HI_ERR_PQ("unknown chip type \r\n");
    return HI_FAILURE;
#endif

    if (ret != 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

hi_s32 pq_comm_alg_register(hi_pq_module_type module,
                            pq_reg_type reg_type,
                            pq_bin_adapt_type adapt_type,
                            const hi_u8* alg_name,
                            const pq_alg_funcs* funcs)
{
    pq_alg_reg *alg_reg = HI_NULL;
    hi_u32 min_len;

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(reg_type, REG_TYPE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(adapt_type, PQ_BIN_ADAPT_MAX);
    PQ_CHECK_NULL_PTR_RE_FAIL(alg_name);
    PQ_CHECK_NULL_PTR_RE_FAIL(funcs);

    if (g_alg_reg_module[module] == HI_NULL) {
        alg_reg = (pq_alg_reg *)pq_kmalloc(sizeof(pq_alg_reg));
        if (alg_reg == HI_NULL) {
            HI_ERR_PQ("ID: %d, register failure!", (hi_u32)reg_type);
            return HI_FAILURE;
        }
        memset(alg_reg, 0, sizeof(pq_alg_reg));
        g_alg_reg_module[module] = alg_reg;
    } else {
        alg_reg = g_alg_reg_module[module];
    }

    alg_reg->alg_id = (hi_u32)module;
    alg_reg->type_id = (hi_u32)reg_type;
    alg_reg->adape_type = (hi_u32)adapt_type;
    alg_reg->fun = funcs;

    min_len = MIN_LEN(sizeof(alg_reg->alg_name) - 1, strlen(alg_name));
    memset(alg_reg->alg_name, 0, sizeof(alg_reg->alg_name));
    memcpy(alg_reg->alg_name, alg_name, min_len);

    return HI_SUCCESS;
}

hi_s32 pq_comm_alg_unregister(hi_pq_module_type module)
{
    pq_alg_reg *alg_reg = HI_NULL;

    PQ_CHECK_OVER_RANGE_RE_FAIL(module, HI_PQ_MODULE_MAX);

    if (g_alg_reg_module[module] == HI_NULL) {
        HI_ERR_PQ("null %d \n", module);
        return HI_SUCCESS;
    }
    alg_reg = g_alg_reg_module[module];

    alg_reg->alg_id = HI_NULL;
    alg_reg->type_id = HI_NULL;
    alg_reg->adape_type = HI_NULL;
    alg_reg->fun = HI_NULL;

    memset(alg_reg->alg_name, 0, sizeof(alg_reg->alg_name));

    pq_kfree_safe(alg_reg);

    g_alg_reg_module[module] = HI_NULL;
    return HI_SUCCESS;
}

pq_alg_reg *pq_comm_get_alg(hi_pq_module_type module)
{
    if (module >= HI_PQ_MODULE_MAX) {
        return HI_NULL;
    }

    return g_alg_reg_module[module];
}

hi_u32 pq_comm_get_alg_type_id(hi_pq_module_type module)
{
    if (module >= HI_PQ_MODULE_MAX) {
        return REG_TYPE_MAX;
    }

    if (g_alg_reg_module[module] == HI_NULL) {
        return REG_TYPE_MAX;
    }

    return g_alg_reg_module[module]->type_id;
}

hi_u32 pq_comm_get_alg_adape_type(hi_pq_module_type module)
{
    if (module >= HI_PQ_MODULE_MAX) {
        return PQ_BIN_ADAPT_MAX;
    }

    if (g_alg_reg_module[module] == HI_NULL) {
        return PQ_BIN_ADAPT_MAX;
    }

    return g_alg_reg_module[module]->adape_type;
}

hi_u8 *pq_comm_get_alg_name(hi_pq_module_type module)
{
    if (module >= HI_PQ_MODULE_MAX) {
        return HI_NULL;
    }

    if (g_alg_reg_module[module] == HI_NULL) {
        return HI_NULL;
    }

    return g_alg_reg_module[module]->alg_name;
}

/* en_module must be a  clear module , en_src_mode has been charged when set */
hi_u8 pq_comm_get_moudle_ctrl(hi_pq_module_type module, pq_source_mode src_mode,
                              pq_output_mode out_mode)
{
    return g_moudle_ctrl[module][src_mode][out_mode];
}

hi_void pq_comm_set_moudle_ctrl(hi_pq_module_type module, pq_source_mode src_mode,
                                pq_output_mode out_mode, hi_u8 on_off)
{
    PQ_CHECK_OVER_RANGE_RE_NULL(module, HI_PQ_MODULE_MAX);
    PQ_CHECK_OVER_RANGE_RE_NULL(src_mode, PQ_SOURCE_MODE_MAX);

    g_moudle_ctrl[module][src_mode][out_mode] = on_off;
    return;
}

pq_source_mode pq_comm_check_source_mode(hi_u32 width)
{
    if (width <= PQ_WIDTH_720) { /* SD source */
        return PQ_SOURCE_MODE_SD;
    } else if (width <= PQ_WIDTH_1920) { /* FHD source */
        return PQ_SOURCE_MODE_FHD;
    } else if (width <= PQ_WIDTH_4096) { /* 4K source */
        return PQ_SOURCE_MODE_4K;
    } else if (width <= PQ_WIDTH_8192) { /* 8K source */
        return PQ_SOURCE_MODE_8K;
    }

    return PQ_SOURCE_MODE_NO;
}

pq_output_mode pq_comm_check_output_mode(hi_u32 width, hi_u32 refresh_rate)
{
    if (width <= PQ_WIDTH_720) { /* SD output */
        return PQ_OUTPUT_MODE_SD;
    } else if (width <= PQ_WIDTH_1920) { /* FHD output */
        return PQ_OUTPUT_MODE_FHD;
    } else if (width <= PQ_WIDTH_4096) { /* 4K output */
        return PQ_OUTPUT_MODE_4K;
    } else if ((width <= PQ_WIDTH_7680) && (refresh_rate <= REFRESHRATE_60)) { /* 8K60 output */
        return PQ_OUTPUT_MODE_8K60;
    } else if ((width <= PQ_WIDTH_7680) && (refresh_rate <= REFRESHRATE_120)) { /* 8K60 output */
        return PQ_OUTPUT_MODE_8K120;
    }

    return PQ_OUTPUT_MODE_NO;
}

pq_source_mode pq_comm_get_source_mode(hi_void)
{
    return g_en_source_mode;
}

hi_void pq_comm_set_source_mode(pq_source_mode mode)
{
    PQ_CHECK_OVER_RANGE_RE_NULL(mode, PQ_SOURCE_MODE_MAX);

    g_en_source_mode = mode;
    return;
}

pq_output_mode pq_comm_get_output_mode(hi_void)
{
    return g_en_output_mode;
}

hi_void pq_comm_set_output_mode(pq_output_mode mode)
{
    PQ_CHECK_OVER_RANGE_RE_NULL(mode, PQ_OUTPUT_MODE_MAX);

    g_en_output_mode = mode;
    return;
}

/* mem comm */
hi_s32 pq_comm_mem_alloc(drv_pq_mem_attr *in_mem_attr, drv_pq_mem_info *out_mem_info)
{
    hi_s32 ret;
    drv_pq_mem_info mem_info = {0};

    if (HI_NULL == in_mem_attr || HI_NULL == out_mem_info) {
        HI_ERR_PQ("drv_pq_comm_mem_alloc para is null !!!!!!!!!!\n");
        return HI_FAILURE;
    }

    ret = drv_pq_mem_alloc(in_mem_attr, &mem_info);
    if (ret == HI_SUCCESS) {
        out_mem_info->mode = in_mem_attr->mode;
        out_mem_info->is_map_viraddr = in_mem_attr->is_map_viraddr;
        out_mem_info->is_cache = in_mem_attr->is_cache;

        out_mem_info->size = mem_info.size; /* after align size */
        out_mem_info->phy_addr = mem_info.phy_addr;
        out_mem_info->vir_addr = mem_info.vir_addr;
        out_mem_info->name = mem_info.name; /* no buffer name temp */
        out_mem_info->dma_buf = mem_info.dma_buf;
    }

    return ret;
}

hi_s32 pq_comm_mem_free(drv_pq_mem_info *mem_info)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(mem_info);

    ret = drv_pq_mem_free(mem_info);

    return ret;
}

hi_s32 pq_comm_mem_addr_map(hi_u64 fd, drv_pq_mem_fd_info *out_fd_info)
{
    hi_s32 ret;
    drv_pq_mem_info mem_info_tmp;

    PQ_CHECK_NULL_PTR_RE_FAIL(out_fd_info);

    ret = drv_pq_mem_map_fd((struct dma_buf *)fd, &mem_info_tmp);
    PQ_CHECK_RETURN_SUCCESS(ret);

    out_fd_info->phy_addr = mem_info_tmp.phy_addr;
    out_fd_info->vir_addr = mem_info_tmp.vir_addr;
    out_fd_info->fd = fd;

    return ret;
}

hi_s32 pq_comm_mem_unmap(drv_pq_mem_fd_info *fd_info)
{
    hi_s32 ret;
    drv_pq_mem_info mem_info_tmp;

    PQ_CHECK_NULL_PTR_RE_FAIL(fd_info);

    mem_info_tmp.phy_addr = fd_info->phy_addr;
    mem_info_tmp.vir_addr = fd_info->vir_addr;

    ret = drv_pq_mem_unmap_fd((struct dma_buf *)fd_info->fd, &mem_info_tmp);

    return ret;
}

hi_s32 pq_comm_mem_flush(drv_pq_mem_info *mem_info)
{
    hi_s32 ret;

    PQ_CHECK_NULL_PTR_RE_FAIL(mem_info);

    /* dma_buf need to mod later todo */
    ret = dma_buf_begin_cpu_access(mem_info->dma_buf, DMA_FROM_DEVICE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("ret = %d\n", ret);
    }

    return ret;
}

hi_s32 pq_comm_vdp_stt_init(hi_void)
{
    hi_s32 ret;
    drv_pq_mem_attr attr = {0};

    attr.name = "pq_vdp_stt_info";
    attr.size = PQ_VDP_STT_BUF_SIZE;
    attr.mode = OSAL_MMZ_TYPE; /* just cma support */
    attr.is_map_viraddr = HI_TRUE;
    attr.is_cache = HI_FALSE;
    ret = pq_comm_mem_alloc(&attr, &g_pq_vdp_stt_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_pq_vdp_stt_buf malloc failed\n");
        return HI_FAILURE;
    }

    ret = pq_comm_mem_flush(&g_pq_vdp_stt_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("g_pq_vdp_stt_buf flush failed\n");
        pq_comm_vdp_stt_deinit();
        return HI_FAILURE;
    }

    ret = pq_hal_comm_vdp_stt_init(g_pq_vdp_stt_buf.vir_addr, g_pq_vdp_stt_buf.phy_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_comm_vdp_stt_init failed\n");
        pq_comm_vdp_stt_deinit();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 pq_comm_vdp_stt_deinit(hi_void)
{
    /* release vdp stt buf. */
    if (g_pq_vdp_stt_buf.vir_addr != HI_NULL) {
        pq_comm_mem_free((drv_pq_mem_info *)(&(g_pq_vdp_stt_buf)));
        g_pq_vdp_stt_buf.vir_addr = HI_NULL;
        g_pq_vdp_stt_buf.phy_addr = 0;
    }

    return HI_SUCCESS;
}

hi_s32 pq_comm_vdp_stt_update(hi_void)
{
    hi_s32 ret;

    ret = pq_hal_comm_vdp_stt_update();
    return ret;
}


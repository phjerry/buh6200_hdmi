/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: create adsp thread and initialize
 * Author: Audio
 * Create: 2019-10-30
 * Notes: NA
 * History: 2019-10-30 sw adsp driver
 */

#include "hi_adsp_debug.h"

#include "hi_osal.h"
#include "hi_audsp_aoe.h"
#include "drv_ao_ext.h"
#include "drv_adsp_hal.h"
#include "drv_adsp_cmd.h"
#include "drv_adsp_private.h"

#define SHARESRAM_RESERVE_COUNT 2

static audio_export_func *adsp_get_audio_mem_func(hi_void)
{
    hi_s32 ret;
    audio_export_func *mem_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_AIAO, (hi_void **)&mem_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_get, ret);
        return HI_NULL;
    }

    if (mem_func == HI_NULL ||
        mem_func->mmz_alloc == HI_NULL ||
        mem_func->mmz_release == HI_NULL) {
        HI_LOG_FATAL("audio mem function is invalid\n");
        return HI_NULL;
    }

    return mem_func;
}

static hi_audio_buffer g_dsp_aoe_reg;

static hi_s32 alloc_aoe_reg(hi_void)
{
    hi_s32 ret;

    /* for chip(s) does not need to map aoe registers to user space,
     * we use cache MMZ as aoe registers, make read and write more efficient
     *
     * note: karaoke and MS12 render will map aoe registers to user space
     */
#if defined(CHIP_TYPE_HI3716MV430)
    const hi_bool cache = HI_TRUE;
#else
    const hi_bool cache = HI_FALSE;
#endif
    const hi_u32 size = DSP0_SHARESRAM_MAXSIZE + DSP0_SHARESRAM_RESERVESIZE * SHARESRAM_RESERVE_COUNT;
    audio_export_func *mem_func = adsp_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        return HI_FAILURE;
    }

    ret = mem_func->mmz_alloc("AOE_SHARED_RAM", size, cache, &g_dsp_aoe_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(mem_func->mmz_alloc, ret);
        return ret;
    }

    ret = memset_s(g_dsp_aoe_reg.virt_addr, g_dsp_aoe_reg.size, 0, size);
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        mem_func->mmz_release(&g_dsp_aoe_reg);
        return ret;
    }

    HI_DBG_PRINT_VOID(g_dsp_aoe_reg.virt_addr);
    HI_DBG_PRINT_H64(g_dsp_aoe_reg.phys_addr);

    return HI_SUCCESS;
}

static hi_void free_aoe_reg(hi_void)
{
    audio_export_func *mem_func = adsp_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        return;
    }

    if (g_dsp_aoe_reg.virt_addr != HI_NULL) {
        mem_func->mmz_release(&g_dsp_aoe_reg);
        g_dsp_aoe_reg.virt_addr = HI_NULL;
        g_dsp_aoe_reg.phys_addr = 0;
    }
}

static hi_void adsp_unload_fw(hi_void)
{
#ifdef HI_PROC_SUPPORT
    adsp_proc_deinit();
#endif
    free_aoe_reg();
}

static hi_s32 adsp_load_fw(hi_void)
{
    hi_s32 ret;

    ret = alloc_aoe_reg();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(alloc_aoe_reg, ret);
        return ret;
    }

#ifdef HI_PROC_SUPPORT
    ret = adsp_proc_init(g_dsp_aoe_reg.virt_addr + DSP0_SHARESRAM_RESERVESIZE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_proc_init, ret);
        free_aoe_reg();
        return ret;
    }
#endif

    return HI_SUCCESS;
}

static hi_s32 adsp_load_aoe(hi_void)
{
    hi_s32 ret;
    aoe_regs_type *aoe_reg = (aoe_regs_type *)(g_dsp_aoe_reg.virt_addr + DSP0_SHARESRAM_RESERVESIZE);

    ret = adsp_cmd_fifo_init(aoe_reg, HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_cmd_fifo_init, ret);
        return ret;
    }

    ret = aoe_sw_engine_create(aoe_reg);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(aoe_sw_engine_create, ret);
        goto out;
    }

    return HI_SUCCESS;

out:
    adsp_cmd_fifo_deinit();
    return ret;
}

static hi_s32 adsp_unload_aoe(hi_void)
{
    hi_s32 ret;

    ret = aoe_sw_engine_destory();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(aoe_sw_engine_destory, ret);
        return ret;
    }

    adsp_cmd_fifo_deinit();

    return HI_SUCCESS;
}

static hi_s32 adsp_get_aoe_info(aoe_fw_info *info)
{
    if (g_dsp_aoe_reg.virt_addr == HI_NULL) {
        return HI_FAILURE;
    }

    if (info == HI_NULL) {
        return HI_FAILURE;
    }

    info->aoe_sw_flag = HI_TRUE;
    info->aoe_reg_vir_addr = g_dsp_aoe_reg.virt_addr + DSP0_SHARESRAM_RESERVESIZE;
    info->aoe_reg_phy_addr = g_dsp_aoe_reg.phys_addr + DSP0_SHARESRAM_RESERVESIZE;

    return HI_SUCCESS;
}

static adsp_driver g_adsp_driver = {
    .load_fw      = adsp_load_fw,
    .unload_fw    = adsp_unload_fw,
    .load_aoe     = adsp_load_aoe,
    .unload_aoe   = adsp_unload_aoe,
    .get_aoe_info = adsp_get_aoe_info,
};

adsp_driver *sw_adsp_get_driver(hi_void)
{
    return &g_adsp_driver;
}


/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp command fifo implementation
 * Author: Audio
 * Create: 2019-11-11
 * Notes: NA
 * History: 2019-11-11 first version for tplay
 */

#include "hi_osal.h"

#include "hi_adsp_debug.h"
#include "drv_adsp_cmd.h"
#include "hi_kfifo.h"
#include "drv_ao_ext.h"

#define ADSP_CMD_FIFO_SIZE 0x1000 /* 4k */

static kfifo g_adsp_cmd_fifo;
static hi_audio_buffer g_adsp_cmd_fifo_buf;

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
        mem_func->mmz_release == HI_NULL ||
        mem_func->smmu_alloc == HI_NULL ||
        mem_func->smmu_release == HI_NULL) {
        return HI_NULL;
    }

    return mem_func;
}

hi_s32 adsp_cmd_fifo_init(aoe_regs_type *aoe_reg, hi_bool sw_aoe)
{
    hi_s32 ret;
    cmd_fifo_reg *fifo = &aoe_reg->aoe_cmd_fifo;

    audio_export_func *mem_func = adsp_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        return HI_FAILURE;
    }

    ret = mem_func->mmz_alloc("adsp_cmd_fifo", ADSP_CMD_FIFO_SIZE, sw_aoe, &g_adsp_cmd_fifo_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(mem_func->mmz_alloc, ret);
        return HI_ERR_ADSP_ALLOC_MEM_FAILED;
    }

    fifo->read = 0;
    fifo->write = 0;
    fifo->size = ADSP_CMD_FIFO_SIZE;

    if (sw_aoe == HI_TRUE) {
        adsp_write_addr(g_adsp_cmd_fifo_buf.virt_addr, fifo->addr);
    } else {
        adsp_write_addr(g_adsp_cmd_fifo_buf.phys_addr, fifo->addr);
    }

    ret = kfifo_init(&g_adsp_cmd_fifo, &fifo->write, &fifo->read,
        g_adsp_cmd_fifo_buf.virt_addr, ADSP_CMD_FIFO_SIZE);
    if (ret != 0) {
        HI_ERR_PRINT_FUNC_RES(kfifo_init, ret);
        adsp_cmd_fifo_deinit();
        return ret;
    }

    return HI_SUCCESS;
}

hi_void adsp_cmd_fifo_deinit(hi_void)
{
    audio_export_func *mem_func = adsp_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        return;
    }

    mem_func->mmz_release(&g_adsp_cmd_fifo_buf);
}

hi_s32 adsp_write_cmd(adsp_cmd_arg *cmd)
{
    hi_s32 len;

    if ((cmd == HI_NULL) || (cmd->argv == HI_NULL)) {
        return HI_ERR_ADSP_NULL_PTR;
    }

    if ((cmd->argc.cmd >= ADSP_CMD_MAX) || (cmd->argc.cmd_type >= ADSP_CMD_TYPE_MAX)) {
        HI_ERR_PRINT_H32(cmd->argc.cmd);
        HI_ERR_PRINT_H32(cmd->argc.cmd_type);
        return HI_ERR_ADSP_INVALID_PARA;
    }

    if (sizeof(cmd->argc) + cmd->argc.size >= ADSP_CMD_FIFO_SIZE) {
        HI_ERR_PRINT_H32(cmd->argc.size);
        return HI_ERR_ADSP_INVALID_PARA;
    }

    while (1) {
        if (kfifo_unused(&g_adsp_cmd_fifo) < sizeof(cmd->argc) + cmd->argc.size) {
            osal_msleep(1);
            continue;
        }

        len = kfifo_in(&g_adsp_cmd_fifo, cmd, sizeof(cmd->argc));
        if (len != sizeof(cmd->argc)) {
            return HI_FAILURE;
        }

        len = kfifo_in(&g_adsp_cmd_fifo, cmd->argv, cmd->argc.size);
        if (len != cmd->argc.size) {
            return HI_FAILURE;
        }

        break;
    }

    return HI_SUCCESS;
}

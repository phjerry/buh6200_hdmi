/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: load adsp firmware and initialize
 * Author: Audio
 * Create: 2019-10-30
 * Notes: NA
 * History: 2019-10-30 hw adsp driver
 */

#include "hi_adsp_debug.h"

#include "hi_reg_dsp.h"
#include "drv_ao_ext.h"
#include "drv_adsp_hal.h"
#include "drv_adsp_cmd.h"
#include "drv_adsp_private.h"
#include "linux/hisilicon/hi_drv_mbx.h"

static aoe_fw_info g_dsp_fw_info;

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

static hi_s32 adsp_map_aoe_reg(hi_void)
{
    hi_u32 i;
    hi_u8 *aoe_reg = HI_NULL;

    aoe_reg = (hi_u8 *)osal_ioremap_nocache(DSP0_SHARESRAM_BASEADDR, DSP0_SHARESRAM_MAXSIZE);
    if (aoe_reg == HI_NULL) {
        HI_LOG_ERR("adsp_ioremap aoe_reg failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < DSP0_SHARESRAM_MAXSIZE; i++) {
        *(aoe_reg + i) = 0x0;
    }

    g_dsp_fw_info.aoe_sw_flag = HI_FALSE;
    g_dsp_fw_info.aoe_reg_vir_addr = (hi_void *)aoe_reg;
    g_dsp_fw_info.aoe_reg_phy_addr = DSP0_SHARESRAM_BASEADDR;

    HI_LOG_INFO("aoe_reg = 0x%p\n", aoe_reg);

    return HI_SUCCESS;
}

static hi_void adsp_unmap_aoe_reg(hi_void)
{
    if (g_dsp_fw_info.aoe_reg_vir_addr != HI_NULL) {
        osal_iounmap(g_dsp_fw_info.aoe_reg_vir_addr);
        g_dsp_fw_info.aoe_reg_vir_addr = HI_NULL;
        g_dsp_fw_info.aoe_reg_phy_addr = 0;
    }
}

static const hi_u8 g_adsp_fw[] = {
#include "firmware/adsp_elf.dat"
};

enum mailbox_cmd {
    MBX_READ_TRNG   = 0x4B3C2D1E,
    MBX_LOAD_TEE    = 0x4B3C2D2D,
    MBX_LOAD_HRF    = 0x4B3C2D3C,
    MBX_LOAD_TPP    = 0x4B3C2D4B,
    MBX_LOAD_ADSP   = 0x4B3C2D5A,
    MBX_UNLOAD_ADSP = 0x4B3C2D6B,
    MBX_ACK_MSG     = 0x4B3CD22F,
    MBX_ACK_SUCCESS = 0x4B3CD296,
    MBX_CRC_FAILURE = 0xB4C32D5A,
};

#define TIME_OUT_SEND 0x50000 /* test time in FPGA */
#define TIME_OUT_READ 0xF0000 /* test time in FPGA */

typedef struct {
    hi_u32 cmd;
    hi_u32 len;
    hi_u32 low_addr;
    hi_u32 high_addr;
    hi_u32 size;
    hi_u32 crc;
} mbx_params;

static hi_u32 drv_mbx_crc32_bitwise(const void *data, hi_u32 length, hi_u32 previous_crc32)
{
    const hi_u32 polynomial = 0xEDB88320;
    hi_u32 crc = ~previous_crc32;
    hi_u8 *cur = (hi_u8 *)data;
    hi_u32 j;

    while (length--) {
        crc ^= *cur++;
        for (j = 0; j < 0x8; j++) {
            crc = (crc >> 1) ^ (-(hi_s32)(crc & 1) & polynomial);
        }
    }
    return ~crc; /* same as crc ^ 0xFFFFFFFF */
}

static hi_s32 adsp_load_with_mbx(hi_u32 phys_addr, hi_u32 size)
{
    hi_s32 ret;
    hi_s32 handle;
    hi_u32 msg_len = 0;

    mbx_params mbx_param = {
        .cmd = MBX_LOAD_ADSP,
        .len = 0,
        .low_addr = phys_addr,
        .high_addr = 0,
        .size = size,
        .crc = 0,
    };

    mbx_param.crc = drv_mbx_crc32_bitwise(&mbx_param, sizeof(mbx_params) - sizeof(hi_u32), 0);

    handle = hi_drv_mbx_open(HI_MBX_ACPU2HRF_BOOT);
    if (handle < 0) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_open, handle);
        return HI_FAILURE;
    }

    ret = hi_drv_mbx_tx(handle, (hi_u8 *)&mbx_param, sizeof(mbx_params), &msg_len, TIME_OUT_SEND);
    if (ret < 0 || msg_len != sizeof(mbx_params)) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_tx, ret);
    }

    ret = hi_drv_mbx_rx(handle, (hi_u8 *)&mbx_param, sizeof(mbx_params), &msg_len, TIME_OUT_READ);
    if (ret < 0 || msg_len != sizeof(mbx_params)) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_rx, ret);
    }

    ret = hi_drv_mbx_close(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_close, ret);
        return ret;
    }

    ret = (mbx_param.cmd == MBX_ACK_SUCCESS) ? HI_SUCCESS : HI_FAILURE;

    HI_FUNC_EXIT();
    return ret;
}

static hi_void adsp_unload_with_mbx(hi_void)
{
    hi_s32 ret;
    hi_s32 handle;
    hi_u32 msg_len = 0;

    mbx_params mbx_param = {
        .cmd = MBX_UNLOAD_ADSP,
        .len = 0,
        .low_addr = 0,
        .high_addr = 0,
        .size = 0,
        .crc = 0,
    };

    mbx_param.crc = drv_mbx_crc32_bitwise(&mbx_param, sizeof(mbx_params) - sizeof(hi_u32), 0);

    handle = hi_drv_mbx_open(HI_MBX_ACPU2HRF_BOOT);
    if (handle < 0) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_open, handle);
        return;
    }

    ret = hi_drv_mbx_tx(handle, (hi_u8 *)&mbx_param, sizeof(mbx_params), &msg_len, TIME_OUT_SEND);
    if (ret < 0 || msg_len != sizeof(mbx_params)) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_tx, ret);
    }

    ret = hi_drv_mbx_rx(handle, (hi_u8 *)&mbx_param, sizeof(mbx_params), &msg_len, TIME_OUT_READ);
    if (ret < 0 || msg_len != sizeof(mbx_params)) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_rx, ret);
    }

    ret = hi_drv_mbx_close(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_mbx_close, ret);
        return;
    }

    if (mbx_param.cmd != MBX_ACK_SUCCESS) {
        HI_LOG_ERR("mailbox ack failed(0x%x)\n", mbx_param.cmd);
    }

    HI_FUNC_EXIT();
}

static hi_s32 adsp_load_firmware(hi_void)
{
    hi_s32 ret;
    hi_audio_buffer cma_buf;

    audio_export_func *mem_func = adsp_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        HI_LOG_ERR("call adsp_get_audio_mem_func failed!\n");
        return HI_FAILURE;
    }

    ret = mem_func->mmz_alloc("adsp_elf", sizeof(g_adsp_fw), HI_FALSE, &cma_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(mem_func->mmz_alloc, ret);
        return ret;
    }

    ret = memcpy_s(cma_buf.virt_addr, cma_buf.size, g_adsp_fw, sizeof(g_adsp_fw));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        ret = HI_FAILURE;
        goto out;
    }

    ret = adsp_load_with_mbx((hi_u32)cma_buf.phys_addr, sizeof(g_adsp_fw));
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_load_with_mbx, ret);
    }

out:
    mem_func->mmz_release(&cma_buf);
    return ret;
}

static volatile S_DSP_UNSECUE_REGS_TYPE* g_reg_adsp_status = HI_NULL;

static hi_s32 adsp_map_ctrl_reg(hi_void)
{
    g_reg_adsp_status =
        (volatile S_DSP_UNSECUE_REGS_TYPE *)osal_ioremap_nocache(REG_ADSP_UNSECUE, sizeof(S_DSP_UNSECUE_REGS_TYPE));
    if (g_reg_adsp_status == HI_NULL) {
        HI_LOG_ERR("ioremap_nocache S_DSP_UNSECUE_REGS_TYPE failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void adsp_unmap_ctrl_reg(hi_void)
{
    if (g_reg_adsp_status != HI_NULL) {
        osal_iounmap(g_reg_adsp_status);
        g_reg_adsp_status = HI_NULL;
    }
}

static hi_s32 adsp_check_start(hi_void)
{
    volatile hi_u32 loop = 0;

    for (loop = 0; loop < DSP_ACK_TIME; loop++) {
        if ((g_reg_adsp_status->DSP_WAITI_STATUS.bits.dsp_work_on_state == 1) &&
            (g_reg_adsp_status->DSP_WAITI_STATUS.bits.waiti == 1)) {
            return HI_SUCCESS;
        }
        osal_msleep(1);
    }

    HI_LOG_ERR("adsp_check_start timeout\n");

    return HI_FAILURE;
}

static hi_void adsp_check_stop(hi_void)
{
    volatile hi_u32 loop = 0;

    /* check peri_pwaitmode_dsp0 to make sure dsp is in waiti mode */
    for (loop = 0; loop < DSP_ACK_TIME; loop++) {
        if ((g_reg_adsp_status->DSP_WAITI_STATUS.bits.dsp_work_on_state == 0) &&
            (g_reg_adsp_status->DSP_WAITI_STATUS.bits.waiti == 1)) {
            return;
        }
        osal_msleep(1);
    }

    HI_LOG_ERR("adsp_check_stop timeout\n");
}

static hi_void adsp_set_arm_ready(hi_void)
{
    hi_u32 unsecue_swi_int = HI_DSP_INTERRUPT_SWI4PERI;
    U_INTR_RAW_UNSECUE_REGS intr_unsecue_reg;
    U_INTR_MSK_UNSECUE_REGS intr_unsecue_mask;

    volatile U_ADSP_CTRL adsp_sysctrl;

    volatile adsp_chn_regs_type *adsp_sys_reg =
        (volatile adsp_chn_regs_type *)(g_dsp_fw_info.aoe_reg_vir_addr + DSP0_SHARESRAM_SYS_OFFSET);

    adsp_sysctrl.u32 = adsp_sys_reg->ADSP_SYSCTRL.u32;
    adsp_sysctrl.bits.aoe_cmd = ADSP_AOE_CMD_MAX;
    adsp_sysctrl.bits.sys_cmd = ADSP_SYSCOM_CMD_MAX;
    adsp_sysctrl.bits.aoe_cmd_done = 1;
    adsp_sysctrl.bits.sys_cmd_done = 1;
    adsp_sys_reg->ADSP_SYSCTRL.u32 = adsp_sysctrl.u32;

    intr_unsecue_reg.u32 = g_reg_adsp_status->INTR_RAW_UNSECUE_REGS.u32;
    intr_unsecue_reg.bits.unsecue_sw_com_int |=
        (1L << (unsecue_swi_int - HI_DSP_INTERRUPT_SWI_UNSEC));
    g_reg_adsp_status->INTR_RAW_UNSECUE_REGS.u32 = intr_unsecue_reg.u32;

    intr_unsecue_mask.u32 = g_reg_adsp_status->INTR_MSK_UNSECUE_REGS.u32;
    intr_unsecue_mask.bits.unsecue_sw_com_int_mask |=
        (1L << (unsecue_swi_int - HI_DSP_INTERRUPT_SWI_UNSEC));
    g_reg_adsp_status->INTR_MSK_UNSECUE_REGS.u32 = intr_unsecue_mask.u32;
}

static hi_void adsp_stop(hi_void)
{
    adsp_check_stop();
    adsp_unmap_ctrl_reg();
    adsp_unload_with_mbx();
}

static hi_s32 adsp_start(hi_void)
{
    hi_s32 ret;

    ret = adsp_load_firmware();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_load_firmware, ret);
        return ret;
    }

    ret = adsp_map_ctrl_reg();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_map_ctrl_reg, ret);
        goto out1;
    }

    ret = adsp_check_start();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_check_start, ret);
        goto out2;
    }

    return ret;

out2:
    adsp_unmap_ctrl_reg();

out1:
    adsp_unload_with_mbx();
    return ret;
}

static hi_void hal_adsp_deinit(hi_void)
{
    adsp_unmap_aoe_reg();
    adsp_stop();
}

static hi_s32 hal_adsp_init(hi_void)
{
    hi_s32 ret;

    ret = adsp_start();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_start, ret);
        return ret;
    }

    ret = adsp_map_aoe_reg();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_map_aoe_reg, ret);
        adsp_stop();
        return ret;
    }

    adsp_set_arm_ready();

    return ret;
}

static volatile adsp_chn_regs_type *g_adsp_sys_reg = HI_NULL;

static adsp_aoesys_cmd_ret adsp_aoe_ack(hi_void)
{
    volatile hi_u32 loop = 0;
    U_ADSP_CTRL adsp_sysctrl;

    for (loop = 0; loop < DSP_ACK_TIME; loop++) {
        adsp_sysctrl.u32 = g_adsp_sys_reg->ADSP_SYSCTRL.u32;
        if (adsp_sysctrl.bits.aoe_cmd_done) {
            return (adsp_aoesys_cmd_ret)adsp_sysctrl.bits.aoe_cmd_return_value;
        }
        osal_msleep(1);
    }

    return ADSP_AOE_CMD_ERR_TIMEOUT;
}

static hi_s32 adsp_aoe_set_cmd(adsp_aoesys_cmd newcmd)
{
    adsp_aoesys_cmd_ret ack;
    U_ADSP_CTRL adsp_sysctrl;
    adsp_sysctrl.u32 = g_adsp_sys_reg->ADSP_SYSCTRL.u32;

    switch (newcmd) {
        case ADSP_AOE_CMD_START:
        case ADSP_AOE_CMD_STOP:
            adsp_sysctrl.bits.aoe_cmd = newcmd;
            break;

        default:
            return HI_SUCCESS;
    }

    adsp_sysctrl.bits.aoe_cmd_done = 0;
    g_adsp_sys_reg->ADSP_SYSCTRL.u32 = adsp_sysctrl.u32;

    ack = adsp_aoe_ack();
    if (ack != ADSP_AOE_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(adsp_aoe_ack, ack);
        HI_ERR_PRINT_H32(newcmd);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static adsp_syscom_cmd_ret adsp_sys_ack(hi_void)
{
    volatile hi_u32 loop = 0;
    U_ADSP_CTRL adsp_sysctrl;

    for (loop = 0; loop < DSP_ACK_TIME; loop++) {
        adsp_sysctrl.u32 = g_adsp_sys_reg->ADSP_SYSCTRL.u32;
        if (adsp_sysctrl.bits.sys_cmd_done) {
            return (adsp_syscom_cmd_ret)adsp_sysctrl.bits.sys_cmd_return_value;
        }
        osal_msleep(1);
    }

    return ADSP_SYSCOM_CMD_ERR_TIMEOUT;
}

static hi_s32 adsp_sys_set_cmd(adsp_syscom_cmd newcmd, hi_bool block)
{
    adsp_syscom_cmd_ret ack;
    U_ADSP_CTRL adsp_sysctrl;

    adsp_sysctrl.u32 = g_adsp_sys_reg->ADSP_SYSCTRL.u32;

    switch (newcmd) {
        case ADSP_SYSCOM_CMD_START:
        case ADSP_SYSCOM_CMD_STOP:
            adsp_sysctrl.bits.sys_cmd = newcmd;
            break;

        default:
            HI_LOG_ERR("unknow cmd\n");
            HI_ERR_PRINT_H32(newcmd);
            return HI_FAILURE;
    }

    adsp_sysctrl.bits.sys_cmd_done = 0;
    g_adsp_sys_reg->ADSP_SYSCTRL.u32 = adsp_sysctrl.u32;
    if (block == HI_TRUE) {
        ack = adsp_sys_ack();
        if (ack != ADSP_SYSCOM_CMD_DONE) {
            HI_ERR_PRINT_FUNC_RES(adsp_sys_ack, ack);
            HI_ERR_PRINT_H32(newcmd);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 adsp_sys_init(hi_void)
{
    hi_s32 ret;
    volatile U_ADSP_CTRL adsp_sysctrl;

    if (g_dsp_fw_info.aoe_reg_vir_addr == HI_NULL) {
        HI_LOG_ERR("DSP share sram ptr is NULL\n");
        return HI_FAILURE;
    }

    g_adsp_sys_reg =
        (volatile adsp_chn_regs_type *)(g_dsp_fw_info.aoe_reg_vir_addr + DSP0_SHARESRAM_SYS_OFFSET);

    adsp_sysctrl.u32 = g_adsp_sys_reg->ADSP_SYSCTRL.u32;
    adsp_sysctrl.bits.aoe_cmd = ADSP_AOE_CMD_MAX;
    adsp_sysctrl.bits.sys_cmd = ADSP_SYSCOM_CMD_MAX;
    adsp_sysctrl.bits.aoe_cmd_done = 1;
    adsp_sysctrl.bits.sys_cmd_done = 1;
    g_adsp_sys_reg->ADSP_SYSCTRL.u32 = adsp_sysctrl.u32;

    ret = adsp_sys_set_cmd(ADSP_SYSCOM_CMD_START, HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_sys_set_cmd, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void adsp_sys_deinit(hi_void)
{
    hi_s32 ret;

    ret = adsp_sys_set_cmd(ADSP_SYSCOM_CMD_STOP, HI_FALSE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_sys_set_cmd, ret);
        return;
    }

    g_adsp_sys_reg = HI_NULL;
}

static hi_s32 adsp_load_fw(hi_void)
{
    hi_s32 ret;

    ret = hal_adsp_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_adsp_init, ret);
        return ret;
    }

    ret = adsp_sys_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_sys_init, ret);
        goto out1;
    }

#ifdef HI_PROC_SUPPORT
    ret = adsp_proc_init(g_dsp_fw_info.aoe_reg_vir_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_proc_init, ret);
        goto out2;
    }
#endif

    return HI_SUCCESS;

#ifdef HI_PROC_SUPPORT
out2:
    adsp_sys_deinit();
#endif

out1:
    hal_adsp_deinit();
    return ret;
}

static hi_void adsp_unload_fw(hi_void)
{
#ifdef HI_PROC_SUPPORT
    adsp_proc_deinit();
#endif

    adsp_sys_deinit();
    hal_adsp_deinit();
}

static hi_s32 adsp_load_aoe(hi_void)
{
    hi_s32 ret;

    aoe_regs_type *aoe_reg = (aoe_regs_type *)(g_dsp_fw_info.aoe_reg_vir_addr + AOE_COM_REG_OFFSET);

    ret = adsp_cmd_fifo_init(aoe_reg, HI_FALSE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_cmd_fifo_init, ret);
        return ret;
    }

    ret = adsp_aoe_set_cmd(ADSP_AOE_CMD_START);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_aoe_set_cmd, ret);
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

    ret = adsp_aoe_set_cmd(ADSP_AOE_CMD_STOP);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_aoe_set_cmd, ret);
        return ret;
    }

    adsp_cmd_fifo_deinit();

    return HI_SUCCESS;
}

static hi_s32 adsp_get_aoe_info(aoe_fw_info *info)
{
    if (g_dsp_fw_info.aoe_reg_vir_addr == HI_NULL) {
        return HI_FAILURE;
    }

    if (info == HI_NULL) {
        return HI_FAILURE;
    }

    info->aoe_sw_flag = HI_FALSE;
    info->aoe_reg_vir_addr = g_dsp_fw_info.aoe_reg_vir_addr;
    info->aoe_reg_phy_addr = g_dsp_fw_info.aoe_reg_phy_addr;

    return HI_SUCCESS;
}

static adsp_driver g_adsp_driver = {
    .load_fw      = adsp_load_fw,
    .unload_fw    = adsp_unload_fw,
    .load_aoe     = adsp_load_aoe,
    .unload_aoe   = adsp_unload_aoe,
    .get_aoe_info = adsp_get_aoe_info,
};

adsp_driver *hw_adsp_get_driver(hi_void)
{
    return &g_adsp_driver;
}


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "hal_mdc.h"
#include "dbg.h"
#include "vfmw_sys.h"
#include "vfmw_osal.h"
#include "mdc_private.h"

#ifdef VFMW_SEC_SUPPORT
/* tmp header include
   need fix by tee
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/syscalls.h>

#include "teek_client_type.h"
#include "teek_client_api.h"
#include "teek_client_id.h"

typedef struct {
    TEEC_Context tee_context;
    TEEC_Session tee_session;
    TEEC_UUID svc_id;
} mdc_tee_ctx;

#define MDC_TEE_LOAD     0
#define MDC_TEE_UNLOAD   1
#define TEE_SERVICE_VFMW \
{ \
    0x3c2bfc84, \
    0xc03c, \
    0x11e6, \
    { \
        0xa4, 0xa6, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01 \
    } \
}

STATIC mdc_tee_ctx g_mdc_tee_ctx_info = {
    .svc_id = TEE_SERVICE_VFMW,
};

#define mdc_hal_get_tee_ctx(ctx) \
do { \
    (ctx) = &g_mdc_tee_ctx_info; \
} while (0)

#endif

#define MDC_REG_MAP(phy, len)  OS_KMAP_REG(phy, len)
#define MDC_REG_UNMAP(vir)     OS_KUNMAP_REG(vir)
#define MDC_WR_REG(base, reg, dat) \
do { \
    *((volatile hi_u32 *)((hi_u8 *)(base) + (reg))) = (dat); \
} while (0)

#define MDC_RD_REG(base, reg) (*((volatile hi_u32 *)((hi_u8 *)(base) + (reg))));

STATIC const hi_u8 g_mdc_bin_array[] = {
#include MDC_BIN_PATH
};

STATIC hi_u32 mdc_hal_get_cpu_freq(hi_u32 cfg)
{
    hi_u32 bit_freq = (cfg & MDC_FREQ_MASK) >> MDC_FREQ_BIT;
    hi_u32 freq_m;

    switch (bit_freq) {
        case MDC_FREQ_24M_CFG:
            freq_m = 24;  /* 24M */
            break;
        case MDC_FREQ_710M_CFG:
            freq_m = 710; /* 710M */
            break;
        case MDC_FREQ_784M_CFG:
            freq_m = 784; /* 784M */
            break;
        case MDC_FREQ_750M_CFG:
            freq_m = 750; /* 750M */
            break;
        case MDC_FREQ_600M_CFG:
            freq_m = 600; /* 600M */
            break;
        case MDC_FREQ_500M_CFG:
            freq_m = 500; /* 500M */
            break;
        case MDC_FREQ_392M_CFG:
            freq_m = 392; /* 392M */
            break;
        default:
            dprint(PRN_ALWS, "%s wrong cfg bit_freq is %d\n", __func__, bit_freq);
            freq_m = 0;
    }

    return freq_m;
}

#ifdef VFMW_SEC_SUPPORT
hi_s32 mdc_hal_load(hi_void)
{
    TEEC_Result result;
    TEEC_Operation operation;
    hi_u32 origin;
    hi_u32 root_id = 0;
    mdc_tee_ctx *ctx;
    hi_u8 package_name[] = "task_hisi_vfmw";

    mdc_hal_get_tee_ctx(ctx);
    /* init tee environment */
    result = TEEK_InitializeContext(HI_NULL, &ctx->tee_context);
    if (result != HI_SUCCESS) {
        DPRINT_ERR("vfmw_tee InitializeContext failed, ReturnCode=0x%x\n", result);
        goto cleanup_1;
    }
    /* open session */
    VFMW_CHECK_SEC_FUNC(memset_s(&operation, sizeof(TEEC_Operation), 0, sizeof(TEEC_Operation)));
    operation.started = 1;
    operation.cancel_flag = 0;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);
    operation.params[2].tmpref.buffer = (void *)(&root_id); /* to third param 2 */
    operation.params[2].tmpref.size = sizeof(root_id); /* to third param 2 */
    operation.params[3].tmpref.buffer = (void *)(package_name); /* to forth param 3 */
    operation.params[3].tmpref.size = strlen(package_name) + 1; /* to forth param 3 srtlen plus 1 */
    result = TEEK_OpenSession(&ctx->tee_context, &ctx->tee_session, &ctx->svc_id,
                              TEEC_LOGIN_IDENTIFY, HI_NULL, &operation, HI_NULL);
    if (result != HI_SUCCESS) {
        DPRINT_ERR("vfmw_tee OpenSession failed, ReturnCode=0x%x, ReturnOrigin=0x%x\n", result, origin);
        goto cleanup_2;
    }
    /* init operation, send cmd */
    VFMW_CHECK_SEC_FUNC(memset_s(&operation, sizeof(TEEC_Operation), 0x0, sizeof(TEEC_Operation)));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.buffer = (void *)&g_mdc_bin_array;
    operation.params[0].tmpref.size = sizeof(g_mdc_bin_array);
    result = TEEK_InvokeCommand(&ctx->tee_session, MDC_TEE_LOAD, &operation, &origin);
    if (result != HI_SUCCESS) {
        DPRINT_ERR("vfmw_tee InvokeCommand failed, ReturnCode=0x%x, ReturnOrigin=0x%x ret.a =0x%x\n",
                   result, origin, operation.params[1].value.a);
        goto cleanup_3;
    }

    return HI_SUCCESS;
cleanup_3:
    TEEK_CloseSession(&ctx->tee_session);
cleanup_2:
    TEEK_FinalizeContext(&ctx->tee_context);
cleanup_1:
    return HI_FAILURE;
}

hi_s32 mdc_hal_unload(hi_void)
{
    TEEC_Result result;
    TEEC_Operation operation;
    mdc_tee_ctx *ctx;
    uint32_t origin;

    mdc_hal_get_tee_ctx(ctx);

    VFMW_CHECK_SEC_FUNC(memset_s(&operation, sizeof(TEEC_Operation), 0x0, sizeof(TEEC_Operation)));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEK_InvokeCommand(&ctx->tee_session, MDC_TEE_UNLOAD, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        DPRINT_ERR("vfmw_tee InvokeCommand failed, ReturnCode=0x%x, ReturnOrigin=0x%x ret.a =0x%x\n",
                   result, origin, operation.params[0].value.a);
    }

    TEEK_CloseSession(&ctx->tee_session);
    TEEK_FinalizeContext(&ctx->tee_context);

    return HI_SUCCESS;
}
#else
hi_s32 mdc_hal_load(hi_void)
{
    hi_u32 *code_virt_addr = HI_NULL;
    hi_u8 *mdc_reg_virt_addr = HI_NULL;
    hi_u8 *mtimer_virt_addr = HI_NULL;
    /* delay test temp data */
    hi_u32 start, end, mcu_start;
    hi_u32 mcu_bin_cpy_cost, mcu_reg_cfg_cost, mcu_boot_succ_cost, mcu_total_cost;
    hi_u8 *mdc_comm_reg_virt_addr = HI_NULL;
#ifdef VFMW_IPCM_SUPPORT
    hi_u8 *ipcm_reg_virt_addr = HI_NULL;
#endif
    mtimer_virt_addr = MDC_REG_MAP(SYS_CNT_REG_BASE, SYS_CNT_REG_LEN);
    VFMW_ASSERT_RET(mtimer_virt_addr != HI_NULL, HI_FAILURE);
    mdc_reg_virt_addr = MDC_REG_MAP(MCU_CFG_CRG_BASE, MCU_CFG_CRG_MAP_LEN);
    VFMW_ASSERT_RET(mdc_reg_virt_addr != HI_NULL, HI_FAILURE);

    MDC_WR_REG(mdc_reg_virt_addr, MCU_RST_REG_OFS, 0x1BF);

    MDC_WR_REG(mtimer_virt_addr, SYS_CNT_REG_CTRL, 1);

    start = MDC_RD_REG(mtimer_virt_addr, SYS_CNT_REG_MTIME_L);
    mcu_start = start;

    code_virt_addr = OS_PHYS_TO_VIRT(MCU_DAT_BASE_ADDR);
    VFMW_ASSERT_RET(code_virt_addr != HI_NULL, HI_FAILURE);

    VFMW_CHECK_SEC_FUNC(memcpy_s(code_virt_addr, sizeof(g_mdc_bin_array),
                                 g_mdc_bin_array, sizeof(g_mdc_bin_array)));
    OS_FLUSH_CACHE(code_virt_addr, MCU_DAT_BASE_ADDR, sizeof(g_mdc_bin_array));
    end = MDC_RD_REG(mtimer_virt_addr, SYS_CNT_REG_MTIME_L);
    mcu_bin_cpy_cost = (end - start) / SYS_CNT_TO_US;

    start = end;
    /* CFG ITCM START END ADDR */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_ITCM_START_REG_OSF, ITCM_BASE_ADDR_IN_MDC);
    MDC_WR_REG(mdc_reg_virt_addr, MCU_ITCM_END_REG_OSF, ITCM_BASE_ADDR_IN_MDC + ITCM_CODE_LEN - 1);

    /* CFG DTCM START END ADDR */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_DTCM_START_REG_OSF, DTCM_BASE_ADDR_IN_MDC);
    MDC_WR_REG(mdc_reg_virt_addr, MCU_DTCM_END_REG_OSF, DTCM_BASE_ADDR_IN_MDC + DTCM_DATA_LEN - 1);

    /* CFG AXI START END ADDR */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_AXI_START_REG_OSF, 0x10000000);
    MDC_WR_REG(mdc_reg_virt_addr, MCU_AXI_END_REG_OSF, 0xFFFFFFFF);

    /* CFG AHB START END ADDR */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_AHB_START_REG_OSF, 0x10000);
    MDC_WR_REG(mdc_reg_virt_addr, MCU_AHB_END_REG_OSF, 0x0FFFFFFF);

    /* CFG MDC START ADDR */
    MDC_WR_REG(mdc_reg_virt_addr, 0x00, MCU_DAT_BASE_ADDR);

    /* RESET DERESET MCU */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_RST_REG_OFS, 1);

#ifdef VFMW_IPCM_SUPPORT
    ipcm_reg_virt_addr = OS_KMAP_REG(IPCM_REG_BASE, IPCM_REG_MAP_LEN);
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_CLT_ADDR_OFS)) = 0x0;
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_SVR_STA_OFS)) = 0x0;
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_SVR_ADDR_OFS)) = 0x0;
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_CLT_STA_OFS)) = (1 << IPCM_REG_ALIVE_BIT);
    OS_KUNMAP_REG(ipcm_reg_virt_addr);
#endif

    /* INIT MDC COMMON STATE POS REG */
    mdc_comm_reg_virt_addr = MDC_REG_MAP(MDC_STA_REG_BASE, MDC_STA_REG_BASE_LEN);
    VFMW_ASSERT_RET(mdc_comm_reg_virt_addr != HI_NULL, HI_FAILURE);
    MDC_WR_REG(mdc_comm_reg_virt_addr, 0x00, 0);
    MDC_REG_UNMAP(mdc_comm_reg_virt_addr);

    MDC_WR_REG(mdc_reg_virt_addr, MCU_CLK_CFG, 0x1FF);

    MDC_WR_REG(mdc_reg_virt_addr, MCU_RST_REG_OFS, 0);

    end = MDC_RD_REG(mtimer_virt_addr, SYS_CNT_REG_MTIME_L);
    mcu_reg_cfg_cost = (end - start) / SYS_CNT_TO_US;
    start = end;

    /* CLEAR WAIT */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_WAIT_REG_OFS, 0);

    end = MDC_RD_REG(mtimer_virt_addr, SYS_CNT_REG_MTIME_L);
    mcu_boot_succ_cost = (end - start) / SYS_CNT_TO_US;

    mcu_total_cost = (end - mcu_start) / SYS_CNT_TO_US;

    dprint(PRN_MDC, "Read MCU Bin From MCUBIN.dat to %#x size = %d OK!\n",
           MCU_DAT_BASE_ADDR, sizeof(g_mdc_bin_array));
    dprint(PRN_MDC, "Start MDC Mtimer is 0x%x, cpy = %dus, reg_cfg = %dus, total = %dus\n",
           end, mcu_reg_cfg_cost, mcu_bin_cpy_cost, mcu_total_cost);

    MDC_REG_UNMAP(mdc_reg_virt_addr);
    MDC_REG_UNMAP(mtimer_virt_addr);

    return HI_SUCCESS;
}

hi_s32 mdc_hal_unload(hi_void)
{
    hi_u8 *mdc_reg_virt_addr = HI_NULL;
#ifdef VFMW_IPCM_SUPPORT
    hi_u8 *ipcm_reg_virt_addr = OS_KMAP_REG(IPCM_REG_BASE, IPCM_REG_MAP_LEN);
#endif

    mdc_reg_virt_addr = MDC_REG_MAP(MCU_CFG_CRG_BASE, MCU_CFG_CRG_MAP_LEN);

    /* RESET MDC */
    MDC_WR_REG(mdc_reg_virt_addr, MCU_RST_REG_OFS, 1);
    /* CLEAR SERVER STATE of IPCM */
#ifdef VFMW_IPCM_SUPPORT
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_SVR_STA_OFS)) = 0x0;
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_CLT_ADDR_OFS)) = 0x0;
    *((volatile hi_u32 *)((hi_u32)ipcm_reg_virt_addr + IPCM_REG_SVR_ADDR_OFS)) = 0x0;
#endif

    MDC_WR_REG(mdc_reg_virt_addr, MCU_RST_REG_OFS, 0x1FF); /* reset this not support ipcm */

    MDC_WR_REG(mdc_reg_virt_addr, MCU_CLK_CFG, 0x0);

#ifdef VFMW_IPCM_SUPPORT
    OS_KUNMAP_REG(ipcm_reg_virt_addr);
#endif
    MDC_REG_UNMAP(mdc_reg_virt_addr);

    dprint(PRN_ALWS, "Unload MDC OK!!\n");

    return HI_SUCCESS;
}
#endif

hi_void mdc_hal_print_mdc_state(hi_void)
{
    hi_u8 *reg_vir_addr;
    hi_u32 reg_value;

    reg_vir_addr = OS_KMAP_REG(MDC_STA_REG_BASE, MDC_STA_REG_BASE_LEN);
    VFMW_ASSERT(reg_vir_addr != HI_NULL);
    reg_value = *((volatile hi_u32 *)reg_vir_addr);
    dprint(PRN_ALWS, "%s MDC boot state reg_value is 0x%x\n", __func__, reg_value);
    OS_KUNMAP_REG(reg_vir_addr);

    reg_vir_addr = MDC_REG_MAP(MCU_CFG_CRG_BASE, MCU_CFG_CRG_MAP_LEN);
    VFMW_ASSERT(reg_vir_addr != HI_NULL);
    reg_value = *((volatile hi_u32 *)reg_vir_addr + MCU_WFI_REG_OFS);
    dprint(PRN_ALWS, "%s MDC MCU state reg_value is 0x%x\n", __func__, reg_value);
    MDC_REG_UNMAP(reg_vir_addr);

    return;
}

hi_s32 mdc_hal_is_wfi(hi_void)
{
    hi_u32 time_out = 100;
    hi_u8 *mdc_reg_base = HI_NULL;
    hi_u32 reg_value;
    hi_s32 flag;

    mdc_reg_base = MDC_REG_MAP(MCU_CFG_CRG_BASE, MCU_CFG_CRG_MAP_LEN);

    while (time_out--) {
        reg_value = MDC_RD_REG(mdc_reg_base, MCU_WFI_REG_OFS);
        flag = (((reg_value) & (1 << MCU_WFI_BIT)) != 0) ? HI_TRUE : HI_FALSE;
        if (flag == HI_TRUE) {
            MDC_REG_UNMAP(mdc_reg_base);
            return HI_SUCCESS;
        }
        OS_MSLEEP(1);
    }

    dprint(PRN_ALWS, "%s timeout !! reg_value is 0x%x\n", __func__, reg_value);

    MDC_REG_UNMAP(mdc_reg_base);

    return HI_FAILURE;
}

hi_void mdc_hal_dump_reg(hi_void *buf, hi_u32 len, hi_s32 *used)
{
    hi_u8 *mdc_reg_base = HI_NULL;
    hi_u8 *mdc_freq_reg = HI_NULL;
    hi_u32 mdc_state, mdc_cpu_cfg, mdc_freq;

    mdc_reg_base = MDC_REG_MAP(MDC0_BASE_ADDR + MCU_CFG_CRG_OFS, MCU_CFG_CRG_MAP_LEN);
    if (mdc_reg_base == HI_NULL) {
        dprint(PRN_ALWS, "%s map mdc_reg_base failed phy 0x%x\n", __func__, MDC0_BASE_ADDR + MCU_CFG_CRG_OFS);
        return;
    }
    mdc_freq_reg = MDC_REG_MAP(MDC_FREQ_BASE, MDC_FREQ_MAP_LEN);
    if (mdc_freq_reg == HI_NULL) {
        MDC_REG_UNMAP(mdc_reg_base);
        dprint(PRN_ALWS, "%s map mdc_freq_reg failed phy 0x%x\n", __func__, MDC_FREQ_BASE);
        return;
    }

    mdc_cpu_cfg = MDC_RD_REG(mdc_freq_reg, 0x00);

    mdc_freq = mdc_hal_get_cpu_freq(mdc_cpu_cfg);

    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_X, "cpu_freq", mdc_freq,
                 "pc_crg", *(volatile hi_u32 *)(mdc_reg_base + MCU_EXEC_ADDR_SRG_OFS));
    mdc_state = MDC_RD_REG(mdc_reg_base, MCU_WFI_REG_OFS);
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "wfi_state", mdc_state & 0x1,
                 "hard_fault", (mdc_state & 0x4) >> 2); /* bit 2 is hard_fault flag */
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_D_D, "iommu_idle", (mdc_state & 0x20) >> 5, /* bit 5 is iommu_idle flag */
                 "debug_mode", (mdc_state & 0x2) >> 1); /* bit 1 is debug_mode flag */
    OS_DUMP_PROC(buf, len, used, 0, LOG_FMT_X_X, "clk_crg", *(volatile hi_u32 *)(mdc_reg_base + MCU_CLK_CFG),
                 "rst_crg", *(volatile hi_u32 *)(mdc_reg_base + MCU_RST_REG_OFS));

    MDC_REG_UNMAP(mdc_reg_base);
    MDC_REG_UNMAP(mdc_freq_reg);
}

hi_void mdc_hal_get_base_time(hi_u64 *arm_us, hi_u64 *mdc_tick)
{
    hi_u64 time_us;
    hi_u64 time_tick;
    hi_u8 *vir = HI_NULL;
    volatile hi_u64 *tick_reg = HI_NULL;

    *arm_us = 0;
    *mdc_tick = 0;

    vir = OS_KMAP_REG(MDC_TICK_REG, 8); /* 8 reg bytes */
    if (vir == HI_NULL) {
        dprint(PRN_ERROR, "map mdc tick reg fail\n");
        return;
    }

    tick_reg = (volatile hi_u64 *)vir;
    time_us = OS_GET_TIME_US64();
    time_tick = *tick_reg;
    time_us += OS_GET_TIME_US64();
    time_tick += *tick_reg;
    time_us += OS_GET_TIME_US64();
    time_tick += *tick_reg;
    time_us += OS_GET_TIME_US64();
    time_tick += *tick_reg;

    OS_KUNMAP_REG(vir);
    *arm_us = OS_DO_DIV(time_us, 4); /* div 4 get avg */
    *mdc_tick = OS_DO_DIV(time_tick, 4); /* div 4 get avg */
}



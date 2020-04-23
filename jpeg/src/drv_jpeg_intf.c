/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg module init implement
 */

#include "drv_jpeg_intf.h"
#include <linux/hisilicon/securec.h>
#include "hi_osal.h"
#include "hi_gfx_comm_k.h"
#include "hi_gfx_sys_k.h"
#include "hi_drv_osal.h"
#include "hi_reg_common.h"
#include "hi_drv_sys.h"
#include "hi_debug.h"

#include "drv_jpeg_hal.h"
#include "drv_jpeg_dev.h"
#include "drv_jpeg_dec.h"
#include "drv_jpeg_reg.h"
#include "drv_jpeg_osr.h"
#include "drv_jpeg_mutex.h"
#include "drv_ioctl_jpeg.h"
#include "hi_drv_jpeg.h"
#include "drv_jpeg_csc.h"

static hi_bool g_jpeg_finish_init = HI_FALSE;
static hi_bool g_jpeg_finish_register = HI_FALSE;
static hi_bool g_smmu_init = HI_FALSE;

static hi_s32 drv_jpeg_dev_register(hi_void);
static hi_s32 jpeg_irq_func(hi_s32 irq_num, hi_void *dev_id);
static hi_s32 jpeg_get_interrupt_state(hi_jpeg_dec_state *int_state);
static hi_s32 jpeg_wait_hard_timeout(hi_jpeg_dec_state *int_state);
static hi_void jpeg_wait_soft_timeout(hi_jpeg_dec_state *int_state);
static hi_void jpeg_reset_decompress(hi_jpeg_dec_state int_state);
static hi_void jpeg_mod_exit(jpg_exit_state exit_state);
static hi_s32 jpeg_decode_init(hi_void);
static hi_void jpeg_decode_deinit(hi_void);
static hi_s32 jpeg_register_irq(hi_u32 irq_num);
static hi_void jpeg_unregister_irq(hi_u32 irq_num);

#define JPEGNAME "jpeg"
#define JPEG_SYSTEM_ABNORMAL_TIMES 100
#define JPEG_MAX_CNT 10
#define DRV_JPEG_INTTYPE_DELAY_TIME 10000
#define DRV_JPEG_INTTYPE_DELAY_TIME_UNIT 1000

typedef struct {
    volatile U_PERI_CRG332 temp_value;
    hi_u32 cur_clock_state;
    hi_u32 start_timems;
    hi_u32 end_timems;
    hi_u32 total_time;
} drv_jpeg_reg_reset_info;

static drv_jpeg_mgr *g_drv_jpeg_info = NULL;

static hi_jpeg_export_func g_jpeg_export_func = {
    .drv_jpeg_module_init = hi_drv_jpeg_module_init,
    .drv_jpeg_module_exit = hi_drv_jpeg_module_exit,
    .drv_jpeg_module_open = drv_jpeg_dev_open,
    .drv_jpeg_module_close = drv_jpeg_dev_close,
    .drv_jpeg_module_get_status = drv_jpeg_dev_get_status
};

#ifdef CONFIG_GFX_PROC_SUPPORT
static hi_void hi_jpeg_show_sdk_version(hi_bool is_load)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#if !defined(CONFIG_GFX_COMM_VERSION_DISABLE) && !defined(CONFIG_GFX_COMM_DEBUG_DISABLE) && defined(MODULE)
    if (is_load == HI_TRUE) {
        GRAPHIC_COMM_PRINT("Load hi_jpeg.ko success.\t(%s)\n", VERSION_STRING);
    } else {
        GRAPHIC_COMM_PRINT("UnLoad hi_jpeg.ko success.\t(%s)\n", VERSION_STRING);
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

hi_s32 hi_drv_jpeg_module_init(hi_void)
{
    hi_s32 ret;

    if (g_jpeg_finish_init == HI_TRUE) {
        return HI_SUCCESS;
    }

#ifndef HI_MCE_SUPPORT
    ret = drv_jpeg_dev_register();
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call drv_jpeg_dev_register failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
#endif

    ret = drv_jpeg_init_tde_dev();
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d ->failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = drv_jpeg_pm_register();
    if (ret == HI_FAILURE) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT | JPEG_EXIT_MODULE_REGISTER | JPEG_EXIT_DECODE_INIT | JPEG_EXIT_IRQ_REGISTER |
                      JPEG_EXIT_SMMU_MAP | JPEG_EXIT_SMMU_INIT);
        HI_PRINT("[module-jpeg][err] : %s %d call hi_gfx_pm_register failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

#ifdef CONFIG_GFX_PROC_SUPPORT
    hi_jpeg_show_sdk_version(HI_TRUE);
#endif
    g_jpeg_finish_init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 hi_drv_jpeg_module_register(hi_void)
{
    return drv_jpeg_dev_register();
}

hi_void drv_jpeg_cancel_reset(hi_void)
{
    hi_s32 ret;
    volatile U_PERI_CRG332 temp_value;
#ifdef CONFIG_GFX_MMU_CLOCK
    volatile U_PERI_CRG180 temp_smmu_value;
#endif
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_crg);

#ifdef CONFIG_GFX_MMU_CLOCK
    temp_smmu_value.u32 = reg_crg->PERI_CRG332.u32;
    if (temp_smmu_value.bits.jpgd_smmu_cken == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
#endif

    temp_value.u32 = reg_crg->PERI_CRG332.u32;
    if (temp_value.bits.jpgd_cken == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

#ifdef CONFIG_GFX_MMU_CLOCK
    temp_smmu_value.u32 = reg_crg->PERI_CRG180.u32;
    temp_smmu_value.bits.jpgd_smmu_srst_req = 0x0;
    reg_crg->PERI_CRG180.u32 = temp_smmu_value.u32;
    osal_mb();
#endif

    temp_value.u32 = reg_crg->PERI_CRG332.u32;
    temp_value.bits.jpgd_srst_req = 0x0;
    reg_crg->PERI_CRG332.u32 = temp_value.u32;
    osal_mb();
    osal_udelay(1);
#ifdef CONFIG_GFX_MMU_SUPPORT
    if (g_smmu_init == HI_FALSE) {
        ret = HI_GFX_InitSmmu((hi_u32)(JPGD_MMU_REG_BASEADDR));
        if (ret == HI_FAILURE) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_InitSmmu, ret);
            g_smmu_init = HI_FALSE;
        } else {
            g_smmu_init = HI_TRUE;
        }
    }
#endif
    drv_jpeg_set_int_mask(JPGD_REG_INTMASK_VALUE);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void drv_jpeg_on_clock(hi_void)
{
    hi_s32 cnt;
#ifdef CONFIG_GFX_MMU_CLOCK
    volatile U_PERI_CRG180 untemp_smmu_value;
#endif
    volatile U_PERI_CRG332 untemp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_crg);

#ifdef CONFIG_GFX_MMU_CLOCK
    untemp_smmu_value.u32 = reg_crg->PERI_CRG180.u32;
    untemp_smmu_value.bits.jpgd_smmu_cken = 0x1;
    reg_crg->PERI_CRG180.u32 = untemp_smmu_value.u32;
    osal_mb();

    cnt = 0;
    do {
        cnt++;
        untemp_value.u32 = reg_crg->PERI_CRG332.u32;
        if (untemp_value.bits.jpgd_cken == 0x1) {
            break;
        }
    } while (cnt < JPEG_MAX_CNT);
    cnt = 0;
#endif

    untemp_value.u32 = reg_crg->PERI_CRG332.u32;
    untemp_value.bits.jpgd_cken = 0x1;
    reg_crg->PERI_CRG332.u32 = untemp_value.u32;
    osal_mb();

    do {
        cnt++;
        untemp_value.u32 = reg_crg->PERI_CRG332.u32;
        if (untemp_value.bits.jpgd_cken == 0x1) {
            break;
        }
    } while (cnt < JPEG_MAX_CNT);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void drv_jpeg_off_clock(hi_void)
{
#ifdef CONFIG_GFX_SMMU_CLOSE_CLOCK
    volatile U_PERI_CRG180 untemp_smmu_value;
#endif
    volatile U_PERI_CRG332 untemp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_crg);

    untemp_value.u32 = reg_crg->PERI_CRG332.u32;
    untemp_value.bits.jpgd_cken = 0x0;
    reg_crg->PERI_CRG332.u32 = untemp_value.u32;
    osal_mb();

#ifdef CONFIG_GFX_SMMU_CLOSE_CLOCK
    untemp_smmu_value.u32 = reg_crg->PERI_CRG180.u32;
    untemp_smmu_value.bits.jpgd_smmu_cken = 0x0;
    reg_crg->PERI_CRG180.u32 = untemp_smmu_value.u32;
    osal_mb();
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void jpeg_check_and_reset_reg(drv_jpeg_reg_reset_info *reg_reset_info, volatile hi_reg_crg *reg_crg,
                                        volatile hi_u32 *busy_addr)
{
    if ((reg_reset_info == NULL) || (reg_crg == NULL) || (busy_addr == NULL)) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    reg_reset_info->temp_value.u32 = reg_crg->PERI_CRG332.u32;
    if (reg_reset_info->temp_value.bits.jpgd_cken == 0x0) {
        reg_reset_info->cur_clock_state = 0x0;
        reg_reset_info->temp_value.bits.jpgd_cken = 0x1;
        reg_crg->PERI_CRG332.u32 = reg_reset_info->temp_value.u32;
        osal_mb();
        osal_udelay(1);
    }

    /* <-- reset register > */
    reg_reset_info->temp_value.u32 = reg_crg->PERI_CRG332.u32;
    reg_reset_info->temp_value.bits.jpgd_srst_req = 0x1;
    reg_crg->PERI_CRG332.u32 = reg_reset_info->temp_value.u32;
    osal_mb();

    /* <-- check reset success > */
    HI_GFX_GetTimeStamp(&reg_reset_info->start_timems, NULL);
    while (*busy_addr & 0x2) {
        HI_GFX_GetTimeStamp(&reg_reset_info->end_timems, NULL);
        reg_reset_info->total_time = reg_reset_info->end_timems - reg_reset_info->start_timems;
        if (reg_reset_info->total_time >= 1) {
            break;
        }
        osal_udelay(1);
    }
}

static hi_void jpeg_recover_reg_clock(drv_jpeg_reg_reset_info *reg_reset_info, volatile hi_reg_crg *reg_crg)
{
    if ((reg_reset_info == NULL) || (reg_crg == NULL)) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    reg_reset_info->temp_value.u32 = reg_crg->PERI_CRG332.u32;
    reg_reset_info->temp_value.bits.jpgd_cken = reg_reset_info->cur_clock_state;
    reg_crg->PERI_CRG332.u32 = reg_reset_info->temp_value.u32;
    osal_mb();
    osal_udelay(1);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void drv_jpeg_reset_reg(hi_void)
{
#ifdef CONFIG_GFX_SMMU_RESET
    hi_u32 smmu_busy_value = 0;
    volatile U_PERI_CRG180 temp_smmu_value;
#endif
    volatile hi_u32 *busy_addr = jpeg_get_reg_base();
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    drv_jpeg_reg_reset_info reg_reset_info = {0};

    reg_reset_info.cur_clock_state = 0x1;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(busy_addr);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_crg);

    jpeg_check_and_reset_reg(&reg_reset_info, reg_crg, busy_addr);

#ifdef CONFIG_GFX_SMMU_RESET
    temp_smmu_value.u32 = reg_crg->PERI_CRG180.u32;
    temp_smmu_value.bits.jpgd_smmu_srst_req = 0x1;
    reg_crg->PERI_CRG180.u32 = temp_smmu_value.u32;
    osal_mb();

    HI_GFX_SmmuResetValue(JPGD_MMU_REG_BASEADDR, &smmu_busy_value);
    HI_GFX_GetTimeStamp(&reg_reset_info.start_timems, NULL);
    while (smmu_busy_value == 0x0) {
        HI_GFX_GetTimeStamp(&reg_reset_info.end_timems, NULL);
        reg_reset_info.total_time = reg_reset_info.end_timems - reg_reset_info.start_timems;
        if (reg_reset_info.total_time >= 1) {
            break;
        }
        HI_GFX_SmmuResetValue(JPGD_MMU_REG_BASEADDR, &smmu_busy_value);
#if 1
        osal_udelay(1);
        break;
#endif
    }
#endif

#ifdef CONFIG_GFX_SMMU_RESET
    HI_GFX_DeinitSmmu();
    osal_udelay(1);
#endif
    jpeg_recover_reg_clock(&reg_reset_info, reg_crg);

    return;
}

static hi_s32 jpeg_init_lock(hi_void)
{
    hi_s32 ret;

    ret = osal_spin_lock_init(&g_drv_jpeg_info->decode_lock);
    if (ret != 0) {
        g_drv_jpeg_info->decode_lock.lock = NULL;
        return HI_FAILURE;
    }

    ret = osal_wait_init(&g_drv_jpeg_info->wait_intrrupt);
    if (ret != 0) {
        g_drv_jpeg_info->wait_intrrupt.wait = NULL;
        return HI_FAILURE;
    }

    ret = osal_wait_init(&g_drv_jpeg_info->wait_mutex);
    if (ret != 0) {
        g_drv_jpeg_info->wait_mutex.wait = NULL;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void jpeg_deinit_lock(hi_void)
{
    if (g_drv_jpeg_info->decode_lock.lock != NULL) {
        osal_spin_lock_destory(&g_drv_jpeg_info->decode_lock);
    }

    if (g_drv_jpeg_info->wait_intrrupt.wait != NULL) {
        osal_wait_destroy(&g_drv_jpeg_info->wait_intrrupt);
    }

    if (g_drv_jpeg_info->wait_mutex.wait != NULL) {
        osal_wait_destroy(&g_drv_jpeg_info->wait_mutex);
    }
}

static hi_s32 jpeg_decode_init(hi_void)
{
    g_drv_jpeg_info = (drv_jpeg_mgr *)HI_GFX_KMALLOC(HIGFX_JPGDEC_ID, sizeof(drv_jpeg_mgr), GFP_KERNEL);
    if (g_drv_jpeg_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    memset_s(g_drv_jpeg_info, sizeof(drv_jpeg_mgr), 0x0, sizeof(drv_jpeg_mgr));

    if (jpeg_init_lock() != HI_SUCCESS) {
        jpeg_deinit_lock();
        HI_GFX_KFREE(HIGFX_JPGDEC_ID, g_drv_jpeg_info);
        return HI_FAILURE;
    }
    g_drv_jpeg_info->is_lock = HI_FALSE;
    g_drv_jpeg_info->dev_open_times = 0;
    return HI_SUCCESS;
}

static hi_void jpeg_decode_deinit(hi_void)
{
    if (g_drv_jpeg_info != NULL) {
        HI_GFX_KFREE(HIGFX_JPGDEC_ID, (hi_void *)g_drv_jpeg_info);
    }
    g_drv_jpeg_info = NULL;
}

static hi_void jpeg_unregister_irq(hi_u32 irq_num)
{
    if (g_drv_jpeg_info != NULL) {
        osal_irq_free(irq_num, (hi_void *)g_drv_jpeg_info);
    }
}

static hi_s32 jpeg_register_irq(hi_u32 irq_num)
{
    hi_s32 ret;

    ret = osal_irq_request(irq_num, (osal_irq_handler)jpeg_irq_func, NULL, JPEGNAME, g_drv_jpeg_info);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call hi_drv_osal_request_irq failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = HI_GFX_SetIrq(HIGFX_JPGDEC_ID, irq_num, JPEGNAME);
    if (ret != HI_SUCCESS) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT | JPEG_EXIT_MODULE_REGISTER | JPEG_EXIT_IRQ_REGISTER);
        HI_PRINT("[module-jpeg][err] : %s %d call HI_GFX_SetIrq failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 jpeg_irq_func(hi_s32 irq_num, hi_void *dev_id)
{
    hi_u32 reg_int_type = 0;
    hi_s32 ret;
    if (g_drv_jpeg_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    drv_jpeg_dev_get_status(&reg_int_type);
    drv_jpeg_dev_clear_status(reg_int_type);

    ret = HI_GFX_SmmuIsr("HI_MOD_JPEG");
    if (ret != HI_SUCCESS) {
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_NONE;
        HI_PRINT("[module-jpeg][err] : %s %d call HI_GFX_SmmuIsr failure\n", __FUNCTION__, __LINE__);
    }

    if (reg_int_type & 0x1) {
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_FINISH;
    } else if (reg_int_type & 0x2) {
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_ERROR;
    } else if (reg_int_type & 0x4) {
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_CONTINUE;
    } else if (reg_int_type & 0x20) {
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_LOWDEALY;
    } else if (reg_int_type & 0x40) {
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_STREAM_ERROR;
    } else {
        HI_PRINT("[module-jpeg][err] : %s %d reg_int_type:%d\n", __FUNCTION__, __LINE__, reg_int_type);
    }
    osal_wait_wakeup(&g_drv_jpeg_info->wait_intrrupt);
    return OSAL_IRQ_HANDLED;
}

hi_s32 drv_jpeg_decode(hi_void *arg)
{
    hi_s32 ret;
    hi_jpeg_reg_info *reg_info = (hi_jpeg_reg_info *)arg;
    if ((arg == NULL) || (g_drv_jpeg_info == NULL)) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    g_drv_jpeg_info->is_lock = HI_TRUE;
    drv_jpeg_start_decompress(reg_info);
    ret = jpeg_get_interrupt_state(&reg_info->interrupt_state);
    if ((ret != HI_SUCCESS) || (reg_info->interrupt_state != JPEG_INT_TYPE_FINISH)) {
        jpeg_reset_decompress(JPEG_INT_TYPE_ERROR);
        HI_PRINT("[module-jpeg][err] : %s %d interrupt_state:%d\n", __FUNCTION__, __LINE__, reg_info->interrupt_state);
        return HI_FAILURE;
    }
    jpeg_reset_decompress(JPEG_INT_TYPE_FINISH);
    return HI_SUCCESS;
}

hi_s32 drv_jpeg_hdec_decode(hi_void *arg)
{
    hi_s32 ret;
    jpeg_hdec_reg_info *hdec_reg_info = (jpeg_hdec_reg_info *)arg;
    ret = drv_jpeg_decode(&hdec_reg_info->reg_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    ret = drv_jpeg_hdec_csc_convert(&hdec_reg_info->csc_surface);
    if (ret != HI_SUCCESS) {
        hdec_reg_info->reg_info.interrupt_state = JPEG_INT_TYPE_CSC_ERROR;
        HI_PRINT("[module-jpeg][err] : %s %d csc convert failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 jpeg_get_interrupt_state(hi_jpeg_dec_state *int_state)
{
    hi_s32 ret;

    if (g_drv_jpeg_info == NULL) {
        *int_state = JPEG_INT_TYPE_ERROR;
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = jpeg_wait_hard_timeout(int_state);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    } else if (ret == -ERESTARTSYS) {
        jpeg_wait_soft_timeout(int_state);
        return HI_SUCCESS;
    } else {
        *int_state = JPEG_INT_TYPE_ERROR;
        HI_PRINT("[module-jpeg][err] : %s %d call jpeg_wait_hard_timeout failure int_state:%d\n", __FUNCTION__,
                 __LINE__, *int_state);
        return HI_FAILURE;
    }
}

static hi_s32 jpeg_check_interrupt_state(const hi_void *param)
{
    if (g_drv_jpeg_info->interrupt_state != JPEG_INT_TYPE_NONE) {
        return 1;
    }
    return 0;
}

static hi_s32 jpeg_wait_hard_timeout(hi_jpeg_dec_state *int_state)
{
    hi_s32 ret;
    hi_jpeg_dec_state interrupt_state = JPEG_INT_TYPE_NONE;

    ret = osal_wait_timeout_interruptible(&g_drv_jpeg_info->wait_intrrupt, jpeg_check_interrupt_state, NULL,
                                          DRV_JPEG_INTTYPE_DELAY_TIME * HZ / DRV_JPEG_INTTYPE_DELAY_TIME_UNIT);
    osal_irq_disable(JPGD_IRQ_NUM);
    if ((ret > 0) || (g_drv_jpeg_info->interrupt_state != JPEG_INT_TYPE_NONE)) {
        interrupt_state = g_drv_jpeg_info->interrupt_state;
        g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_NONE;
        osal_irq_enable(JPGD_IRQ_NUM);
        *int_state = interrupt_state;
        return HI_SUCCESS;
    }
    osal_irq_enable(JPGD_IRQ_NUM);
    return ret;
}

static hi_void jpeg_wait_soft_timeout(hi_jpeg_dec_state *int_state)
{
    hi_u32 start_time_ms = 0;
    hi_u32 end_time_ms = 0;
    hi_u32 run_time_ms = 0;

    HI_GFX_GetTimeStamp(&start_time_ms, NULL);
    while (1) {
        HI_GFX_GetTimeStamp(&end_time_ms, NULL);
        run_time_ms = end_time_ms - start_time_ms;
        if (run_time_ms >= DRV_JPEG_INTTYPE_DELAY_TIME) {
            break;
        }
    }

    *int_state = g_drv_jpeg_info->interrupt_state;
    g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_NONE;
    return;
}

static hi_void jpeg_reset_decompress(hi_jpeg_dec_state int_state)
{
    hi_ulong lock_flag = 0;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_drv_jpeg_info);

    osal_spin_lock_irqsave(&g_drv_jpeg_info->decode_lock, &lock_flag);
    if (g_drv_jpeg_info->is_lock == HI_FALSE) {
        osal_spin_unlock_irqrestore(&g_drv_jpeg_info->decode_lock, &lock_flag);
        return;
    }

    if ((int_state != JPEG_INT_TYPE_FINISH) && (int_state != JPEG_INT_TYPE_ERROR)) {
        osal_spin_unlock_irqrestore(&g_drv_jpeg_info->decode_lock, &lock_flag);
        HI_PRINT("[module-jpeg][err] : %s %d ->failure int_state:%d\n", __FUNCTION__, __LINE__, int_state);
        return;
    }

    drv_jpeg_reset_reg();
    drv_jpeg_off_clock();

    g_drv_jpeg_info->is_lock = HI_FALSE;
    osal_wait_wakeup(&g_drv_jpeg_info->wait_mutex);

    osal_spin_unlock_irqrestore(&g_drv_jpeg_info->decode_lock, &lock_flag);
    return;
}

static hi_s32 jpeg_check_info_lock(const hi_void *param)
{
    if (g_drv_jpeg_info->is_lock == HI_FALSE) {
        return 1;
    }
    return 0;
}

hi_s32 drv_jpeg_dev_open(struct inode *finode, struct file *ffile)
{
    hi_ulong lock_flag = 0;
    hi_s32 ret;
    if (g_drv_jpeg_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    jpeg_dec_mutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
    if (((g_drv_jpeg_info->is_lock == HI_TRUE) || (g_drv_jpeg_info->dev_open_times != 0)) &&
        (g_drv_jpeg_info->system_abnormal_times <= JPEG_SYSTEM_ABNORMAL_TIMES)) {
        g_drv_jpeg_info->system_abnormal_times++;
        jpeg_dec_unmutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
        HI_PRINT("[module-jpeg][err] : %s %d ->failure, is_lock:%d dev_open_times:%d system_abnormal_times:%d\n",
                 __FUNCTION__, __LINE__, g_drv_jpeg_info->is_lock, g_drv_jpeg_info->dev_open_times,
                 g_drv_jpeg_info->system_abnormal_times);
        return HI_FAILURE;
    }

    (g_drv_jpeg_info->dev_open_times)++;
    g_drv_jpeg_info->system_abnormal_times = 0;
    jpeg_dec_unmutex(&g_drv_jpeg_info->decode_lock, &lock_flag);

    ret = osal_wait_timeout_interruptible(&g_drv_jpeg_info->wait_mutex, jpeg_check_info_lock, NULL,
                                          (DRV_JPEG_INTTYPE_DELAY_TIME * HZ) / DRV_JPEG_INTTYPE_DELAY_TIME_UNIT);
    if (g_drv_jpeg_info->is_lock == HI_TRUE) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_drv_jpeg_info->is_lock);
    }

    if (g_drv_jpeg_info->dev_open_times > 1) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_drv_jpeg_info->dev_open_times);
    }
    if (ret <= 0) {
        drv_jpeg_reset_reg();
        drv_jpeg_cancel_reset();
        jpeg_reset_decompress(JPEG_INT_TYPE_ERROR);
        HI_PRINT("[module-jpeg][err] : %s %d ->failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    drv_jpeg_on_clock();
    drv_jpeg_reset_reg();
    drv_jpeg_cancel_reset();
    return HI_SUCCESS;
}

hi_s32 drv_jpeg_dev_close(struct inode *finode, struct file *ffile)
{
    hi_ulong lock_flag = 0;
    if (g_drv_jpeg_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    jpeg_dec_mutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
    if (g_drv_jpeg_info->dev_open_times == 0) {
        jpeg_dec_unmutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_END);
        HI_PRINT("[module-jpeg][err] : %s %d ->failure\n", __FUNCTION__, __LINE__);
        return HI_SUCCESS;
    }
    jpeg_dec_unmutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
    jpeg_reset_decompress(JPEG_INT_TYPE_ERROR);
    jpeg_dec_mutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
    (g_drv_jpeg_info->dev_open_times)--;
    jpeg_dec_unmutex(&g_drv_jpeg_info->decode_lock, &lock_flag);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_UNF_EXIT_FUNC);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_END);
    return HI_SUCCESS;
}

hi_s32 drv_jpeg_dev_suspend(struct device *dev)
{
    if (g_drv_jpeg_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    drv_jpeg_reset_reg();
    drv_jpeg_off_clock();
    g_drv_jpeg_info->interrupt_state = JPEG_INT_TYPE_FINISH;
    g_drv_jpeg_info->is_suspend = HI_TRUE;
    g_drv_jpeg_info->is_resume = HI_FALSE;

    HI_PRINT("jpeg suspend ok\n");
    return HI_SUCCESS;
}

hi_s32 drv_jpeg_dev_resume(struct device *dev)
{
    if (g_drv_jpeg_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    if (g_drv_jpeg_info->is_suspend == HI_TRUE) {
        g_drv_jpeg_info->is_suspend = HI_FALSE;
        g_drv_jpeg_info->is_resume = HI_TRUE;
    }
    drv_jpeg_on_clock();
    drv_jpeg_cancel_reset();

    HI_PRINT("jpeg resume ok\n");
    return HI_SUCCESS;
}

hi_s32 drv_jpeg_dev_resume_early(struct device *dev)
{
    return HI_SUCCESS;
}

static hi_bool jpeg_smmu_register(hi_void)
{
    hi_s32 ret;

    ret = HI_GFX_MapSmmuReg((hi_u32)(JPGD_MMU_REG_BASEADDR));
    if (ret != HI_SUCCESS) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT | JPEG_EXIT_MODULE_REGISTER | JPEG_EXIT_DECODE_INIT | JPEG_EXIT_IRQ_REGISTER);
        HI_PRINT("[module-jpeg][err] : %s %d call HI_GFX_MapSmmuReg failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
#ifdef CONFIG_GFX_MMU_SUPPORT
    ret = HI_GFX_InitSmmu((hi_u32)(JPGD_MMU_REG_BASEADDR));
    if (ret != HI_SUCCESS) {
        g_smmu_init = HI_FALSE;
    } else {
        HI_PRINT("[module-jpeg] : %s %d init smmu success\n", __FUNCTION__, __LINE__);
        g_smmu_init = HI_TRUE;
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 drv_jpeg_dev_register(hi_void)
{
    hi_s32 ret;
    volatile hi_u32 *jpeg_reg_base = NULL;

    if (g_jpeg_finish_register == HI_TRUE) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_SUCCESS;
    }
    g_jpeg_finish_register = HI_TRUE;

    jpeg_reg_base = (volatile hi_u32 *)HI_GFX_REG_MAP(JPGD_REG_BASEADDR, JPGD_REG_LENGTH);
    if (jpeg_reg_base == NULL) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT);
        HI_PRINT("[module-jpeg][err] : %s %d call HI_GFX_REG_MAP failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    jpeg_set_reg_base(jpeg_reg_base);

    ret = hi_gfx_module_register(HIGFX_JPGDEC_ID, JPEGDEVNAME, &g_jpeg_export_func);
    if (ret != HI_SUCCESS) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT);
        HI_PRINT("[module-jpeg][err] : %s %d call hi_gfx_module_register failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = jpeg_decode_init();
    if (ret != HI_SUCCESS) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT | JPEG_EXIT_MODULE_REGISTER);
        HI_PRINT("[module-jpeg][err] : %s %d call jpeg_decode_init failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = jpeg_register_irq(JPGD_IRQ_NUM);
    if (ret != HI_SUCCESS) {
        jpeg_mod_exit(JPEG_EXIT_HAL_INIT | JPEG_EXIT_MODULE_REGISTER | JPEG_EXIT_DECODE_INIT);
        HI_PRINT("[module-jpeg][err] : %s %d call jpeg_register_irq failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    return jpeg_smmu_register();
}

static hi_void jpeg_mod_exit(jpg_exit_state exit_state)
{
    volatile hi_u32 *reg_base_vir = NULL;
    if (exit_state & JPEG_EXIT_PM_REGISTER) {
        drv_jpeg_pm_unregister();
    }
    if (exit_state & JPEG_EXIT_SMMU_INIT) {
        HI_GFX_DeinitSmmu();
    }

    if (exit_state & JPEG_EXIT_SMMU_MAP) {
        HI_GFX_UnMapSmmuReg();
    }

    if (exit_state & JPEG_EXIT_IRQ_REGISTER) {
        jpeg_unregister_irq(JPGD_IRQ_NUM);
    }
    if (exit_state & JPEG_EXIT_DECODE_INIT) {
        jpeg_deinit_lock();
        jpeg_decode_deinit();
    }

    drv_jpeg_deinit_tde_dev();

    if (exit_state & JPEG_EXIT_MODULE_REGISTER) {
        hi_gfx_module_unregister(HIGFX_JPGDEC_ID);
    }
    reg_base_vir = jpeg_get_reg_base();
    if (reg_base_vir != NULL) {
        HI_GFX_REG_UNMAP((hi_void *)reg_base_vir);
        jpeg_set_reg_base(NULL);
    }
    g_jpeg_finish_register = HI_FALSE;
#ifdef CONFIG_GFX_PROC_SUPPORT
    hi_jpeg_show_sdk_version(HI_FALSE);
#endif
    return;
}

hi_void hi_drv_jpeg_module_exit(hi_void)
{
    if (g_jpeg_finish_init == HI_FALSE) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    g_jpeg_finish_init = HI_FALSE;
    jpeg_mod_exit(JPEG_EXIT_ALL);
    return;
}

#ifdef MODULE
module_init(hi_drv_jpeg_module_init);
module_exit(hi_drv_jpeg_module_exit);
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
MODULE_AUTHOR("HISILICON");
MODULE_DESCRIPTION("Hisilicon JPEG Device Driver");
MODULE_LICENSE("GPL");
#else
MODULE_AUTHOR("");
MODULE_DESCRIPTION("");
MODULE_LICENSE("GPL");
#endif

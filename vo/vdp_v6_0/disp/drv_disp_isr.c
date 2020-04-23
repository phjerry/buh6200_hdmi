/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: isr
* Create: 2019-04-12
 */
#include "linux/interrupt.h"
#include "drv_disp_isr.h"
#include "hi_drv_sys.h"
#include "hi_errno.h"
#include "hi_drv_osal.h"
#include "drv_xdp_osal.h"
#include "drv_display.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

spinlock_t g_isr_list_opt_lock;

static hi_s32 g_disp_isr_mngr_init_flag = -1;
static disp_isr_m g_disp_isr_mngr;
static hi_u32 g_isr_control_flag = 0;
#ifndef IRQ_HANDLED
#define IRQ_HANDLED 1
#endif

#define DEF_DISP_ISR_MAIN_RETURN_VALUE IRQ_HANDLED

#define DISPLAY0_BUS_UNDERFLOW_INT    0x00000008UL
#define DISPLAY1_BUS_UNDERFLOW_INT    0x00000080UL
#define DISP_INT_CNT_MAX              256
#define DEF_DEBUG_DISP_INT_MAX_NUMBER 1000

DEFINE_SPINLOCK(g_isr_list_opt_lock);

#define isr_check_null_pointer(ptr)                                               \
    do {                                                                          \
        if (ptr == HI_NULL) {                                                     \
            hi_err_disp("DISP ERROR! Input null pointer in %s!\n", __FUNCTION__); \
            return HI_ERR_DISP_NULL_PTR;                                          \
        }                                                                         \
    } while(0)

#define disp_check_id(id)                                                      \
    do {                                                                       \
        if ((id >= HI_DRV_DISPLAY_2) /* || (id < HI_DRV_DISPLAY_0) */) {                     \
            hi_err_disp("DISP ERROR! Invalid display in %s!\n", __FUNCTION__); \
            return -1;                                                         \
        }                                                                      \
    } while(0)

hi_s32 disp_isr_main(hi_s32 irq, hi_void *dev_id);

static hi_u32 *get_global_isr_control_flag(hi_void)
{
    return &g_isr_control_flag;
}

hi_s32 disp_isr_check_delay_time_ms(hi_drv_display disp, hi_u32 isr_delay_time_ms)
{
    disp_isr_chn *disp_isr_chn = HI_NULL;

    disp_isr_chn = &g_disp_isr_mngr.disp_chn[disp];

    disp_isr_chn->isr_debug.isr_delay_time_ms = isr_delay_time_ms;

    hi_warn_disp("set disp%d delay time %dms\n", disp, isr_delay_time_ms);
    return HI_SUCCESS;
}


hi_void disp_isr_set_isr_control_flag(isr_func_ctrl_flag en_isr_control_flag)
{
    hi_u32 *isr_control_flag = HI_NULL;

    isr_control_flag = get_global_isr_control_flag();

    *isr_control_flag |= (hi_u32)en_isr_control_flag;
    return;
}

hi_void disp_isr_clear_isr_control_flag(isr_func_ctrl_flag en_isr_control_flag)
{
    hi_u32 *isr_control_flag = HI_NULL;

    isr_control_flag = get_global_isr_control_flag();
    *isr_control_flag &= ~(hi_u32)en_isr_control_flag;

    return;
}

hi_s32 disp_isr_switch_intterrup(hi_drv_display disp, hi_drv_disp_callback_type isr_type, hi_bool enable)
{
    switch (isr_type) {
        case HI_DRV_DISP_C_INTPOS_0_PERCENT:
            if (HI_DRV_DISPLAY_1 == disp) {
                hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_D0_0_PERCENT, enable);
            } else if (HI_DRV_DISPLAY_0 == disp) {
                hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_D1_0_PERCENT, enable);
            }
            break;
        case HI_DRV_DISP_C_INTPOS_90_PERCENT:
            if (HI_DRV_DISPLAY_1 == disp) {
                hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_D0_90_PERCENT, enable);
            } else if (HI_DRV_DISPLAY_0 == disp) {
                hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_D1_90_PERCENT, enable);
            }
            break;
        case HI_DRV_DISP_C_INTPOS_100_PERCENT:
            if (HI_DRV_DISPLAY_1 == disp) {
                hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_D0_100_PERCENT, enable);
            } else if (HI_DRV_DISPLAY_0 == disp) {
                hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_D1_100_PERCENT, enable);
            }
            break;
        case HI_DRV_DISP_C_DHD0_WBC: {
            hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_WBCDHD_PARTFNI, enable);
        }
        break;
        case HI_DRV_DISP_C_VID_WBC: {
            hal_disp_set_interrupt_enable((hi_u32)DISP_INTERRUPT_WBC_VP, enable);
        }
        break;
        case HI_DRV_DISP_C_SMMU: {
            hal_disp_set_interrupt_enable(DISP_INTERRUPT_SMMU, enable);
        }
        break;
        default:
            break;
    }

    return HI_SUCCESS;
}

hi_s32 disp_isr_reset_chn(hi_drv_display disp)
{
    disp_isr_chn *disp_chn = HI_NULL;

    if (g_disp_isr_mngr_init_flag >= 0) {
        return HI_FAILURE;
    }

    disp_chn = &g_disp_isr_mngr.disp_chn[disp];

    memset(disp_chn, 0, sizeof(disp_isr_chn));

    disp_chn->disp = disp;
    disp_chn->cb_info.event_type = HI_DRV_DISP_C_EVET_NONE;

    disp_chn->enable = HI_FALSE;

    return HI_SUCCESS;
}

hi_u32 g_isr_handle = 0;
#define OPTM_IRQ_NUM 140

hi_s32 disp_isr_init(hi_void)
{
    hi_drv_display disp;
    hi_s32 ret;

    if (g_disp_isr_mngr_init_flag >= 0) {
        return HI_SUCCESS;
    }

    memset(&g_disp_isr_mngr, 0, sizeof(disp_isr_m));

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_BUTT; disp++) {
        disp_isr_reset_chn(disp);
    }

    disp_isr_check_delay_time_ms(HI_DRV_DISPLAY_0, DEF_DISP_ISR_DELAY_TIME_MS);
    disp_isr_check_delay_time_ms(HI_DRV_DISPLAY_1, DEF_DISP_ISR_DELAY_TIME_MS);
    ret = osal_irq_request(OPTM_IRQ_NUM, disp_isr_main, 0, "vdp", &g_isr_handle);
    if (ret != HI_SUCCESS) {
        hi_err_disp("test ko registe IRQ failed!\n");
        return HI_FAILURE;
    }

    g_disp_isr_mngr_init_flag++;

    return HI_SUCCESS;
}

hi_s32 disp_isr_deinit(hi_void)
{
    if (g_disp_isr_mngr_init_flag < 0) {
        return HI_SUCCESS;
    }

    memset(&g_disp_isr_mngr, 0, sizeof(disp_isr_m));
    osal_irq_free(OPTM_IRQ_NUM, &g_isr_handle);

    g_disp_isr_mngr_init_flag--;

    return HI_SUCCESS;
}

hi_void disp_isr_free_irq(hi_bool enable)
{
    hi_s32 ret;

    if (enable) {
        osal_irq_free(OPTM_IRQ_NUM, &g_isr_handle);
    } else {
        ret = osal_irq_request(OPTM_IRQ_NUM, disp_isr_main, 0, "vdp", &g_isr_handle);
        if (ret != HI_SUCCESS) {
            hi_err_disp("registe IRQ failed!\n");
        }
    }
}

hi_s32 disp_isr_suspend(hi_void)
{
    if (g_disp_isr_mngr_init_flag < 0) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 disp_isr_resume(hi_void)
{
    hi_drv_display disp = 0;
    hi_drv_disp_callback_type isr_type = 0;
    disp_isr_chn *disp_chn = HI_NULL;
    if (g_disp_isr_mngr_init_flag < 0) {
        return HI_FAILURE;
    }

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_BUTT; disp++) {
        disp_chn = &g_disp_isr_mngr.disp_chn[disp];
        for (isr_type = HI_DRV_DISP_C_INTPOS_0_PERCENT; isr_type < HI_DRV_DISP_C_TYPE_BUTT; isr_type++) {
            if (disp_chn->isr_list[isr_type].node_flag != 0) {
                disp_isr_switch_intterrup(disp, isr_type, HI_TRUE);
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 disp_isr_open_chn(hi_drv_display disp)
{
    if ((g_disp_isr_mngr_init_flag < 0) || (disp >= HI_DRV_DISPLAY_2)) {
        return HI_FAILURE;
    }

    if (g_disp_isr_mngr.disp_chn[disp].enable == HI_TRUE) {
        return HI_SUCCESS;
    }

    disp_isr_reset_chn(disp);

    g_disp_isr_mngr.disp_chn[disp].enable = HI_TRUE;

    g_disp_isr_mngr.chn_number++;

    return HI_SUCCESS;
}

hi_s32 disp_isr_close_chn(hi_drv_display disp)
{
    if ((g_disp_isr_mngr_init_flag < 0) || (disp >= HI_DRV_DISPLAY_2)) {
        return HI_FAILURE;
    }

    if (g_disp_isr_mngr.disp_chn[disp].enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    g_disp_isr_mngr.disp_chn[disp].enable = HI_FALSE;

    g_disp_isr_mngr.chn_number--;

    return HI_SUCCESS;
}

static hi_s32 disp_isr_search_node(disp_isr_chn *disp_chn, hi_drv_disp_callback_type isr_type,
                                   hi_drv_disp_callback *cb_info)
{
    hi_u32 u = 0;
    hi_u64 v = 0;

    v = disp_chn->isr_list[isr_type].node_flag;

    for (u = 0; u < DEF_DISP_ISR_LIST_LENGTH && v; u++) {
        if ((v & ((hi_u64)1 << (hi_u64)u))
            && (disp_chn->isr_list[isr_type].isr_node[u].pf_disp_callback == cb_info->pf_disp_callback)
            && (disp_chn->isr_list[isr_type].isr_node[u].hdst == cb_info->hdst)) {
            hi_info_disp("[search node success][%s,%d] isr_type = %d, node_index = %d!\n",
                         __FUNCTION__,
                         __LINE__,
                         isr_type,
                         u);
            return (hi_s32)u;
        }
    }

    return -1;
}

hi_s32 disp_isr_search_null_node(disp_isr_chn *disp_chn, hi_drv_disp_callback_type isr_type)
{
    hi_u32 i = 0;
    hi_u64 v = 0;

    for (i = 0; i < DEF_DISP_ISR_LIST_LENGTH; i++) {
        v = (hi_u64)1 << (hi_u64)i;
        if ((disp_chn->isr_list[isr_type].node_flag & v) == 0) {
            return i;
        }
    }

    return HI_FAILURE;
}

static hi_u32 g_disp_int_table[HI_DRV_DISPLAY_BUTT][HI_DRV_DISP_C_TYPE_BUTT] = {
    /* NONE, SHOW_MODE, INTPOS_0_PERCENT, INTPOS_90_PERCENT,       GFX_WBC, REG_UP */
    {
        0, DISP_INTERRUPT_D1_0_PERCENT, DISP_INTERRUPT_D1_90_PERCENT,
        DISP_INTERRUPT_D1_100_PERCENT, 0,
        0, 0
    },
    {
        0, DISP_INTERRUPT_D0_0_PERCENT, DISP_INTERRUPT_D0_90_PERCENT,
        DISP_INTERRUPT_D0_100_PERCENT, DISP_INTERRUPT_WBCDHD_PARTFNI,
        DISP_INTERRUPT_WBC_VP, 0
    },
    {
        0, 0, 0,
        0, 0,
        0, 0
    },
};

hi_s32 disp_isr_reg_callback(hi_drv_display disp, hi_drv_disp_callback_type isr_type,
                             hi_drv_disp_callback *isr_cb)
{
    disp_isr_chn *disp_chn = HI_NULL;
    hi_u64 node_flag_new = 0;
    hi_u64 index = 0;
    hi_size_t irq_flag = 0;
    hi_s32 ret = HI_FAILURE;

    disp_check_id(disp);
    isr_check_null_pointer(isr_cb);
    isr_check_null_pointer(isr_cb->pf_disp_callback);

    if (g_disp_isr_mngr_init_flag < 0) {
        return HI_FAILURE;
    }

    if (isr_type >= HI_DRV_DISP_C_TYPE_BUTT) {
        hi_err_disp("Para eType invalid !\n");
        return HI_FAILURE;
    }

    if (isr_cb->callback_prior >= HI_DRV_DISP_CALLBACK_PRIORTY_BUTT) {
        hi_err_disp("Para pstCB invalid:%x,%x,%x!\n", isr_cb, isr_cb->pf_disp_callback, isr_cb->callback_prior);
        return HI_FAILURE;
    }

    spin_lock_irqsave(&g_isr_list_opt_lock, irq_flag);

    disp_chn = &g_disp_isr_mngr.disp_chn[disp];
    if (disp_chn->enable != HI_TRUE) {
        hi_err_disp("DISP %d is not add to ISR manager!\n", disp);
        spin_unlock_irqrestore(&g_isr_list_opt_lock, irq_flag);
        return HI_FAILURE;
    }

    ret = disp_isr_search_null_node(disp_chn, isr_type);
    if (ret < 0) {
        hi_err_disp("DISP %d  callback reach max number!\n", disp);

        spin_unlock_irqrestore(&g_isr_list_opt_lock, irq_flag);
        return ret;
    }

    index = ret;

    /* record callback info */
    disp_chn->isr_list[isr_type].isr_node[index].hdst = isr_cb->hdst;
    disp_chn->isr_list[isr_type].isr_node[index].callback_prior = isr_cb->callback_prior;
    disp_chn->isr_list[isr_type].isr_node[index].pf_disp_callback = isr_cb->pf_disp_callback;

    /* update display channel node flag */
    node_flag_new = disp_chn->isr_list[isr_type].node_flag;
    node_flag_new = node_flag_new | ((hi_u64)1 << (hi_u64)index);
    disp_chn->isr_list[isr_type].node_flag = node_flag_new;

    /* enable interrupt */
    if (disp_chn->isr_list[isr_type].node_flag != 0) {
        disp_isr_switch_intterrup(disp, isr_type, HI_TRUE);
    }

    hi_info_disp("[isr_register success][%s,%d] disp = %d, isr_type = %d, node_index = %d\n", __FUNCTION__, __LINE__,
                 disp, isr_type, index);

    spin_unlock_irqrestore(&g_isr_list_opt_lock, irq_flag);
    return HI_SUCCESS;
}

hi_s32 disp_isr_unreg_callback(hi_drv_display disp, hi_drv_disp_callback_type isr_type,
                               hi_drv_disp_callback *isr_cb)
{
    disp_isr_chn *disp_chn = HI_NULL;
    hi_u64 node_flag_new = 0;
    hi_u64 index;
    hi_size_t irq_flag = 0;
    hi_s32 ret;

    disp_check_id(disp);
    isr_check_null_pointer(isr_cb);
    isr_check_null_pointer(isr_cb->pf_disp_callback);

    if (g_disp_isr_mngr_init_flag < 0) {
        hi_err_disp("isr isn't init !\n");
        return HI_FAILURE;
    }

    if (isr_type >= HI_DRV_DISP_C_TYPE_BUTT) {
        hi_err_disp("Para eType invalid !\n");
        return HI_FAILURE;
    }

    if (isr_cb->callback_prior >= HI_DRV_DISP_CALLBACK_PRIORTY_BUTT) {
        hi_err_disp("Para pstCB invalid:%x,%x,%x!\n", isr_cb, isr_cb->pf_disp_callback, isr_cb->callback_prior);
        return HI_FAILURE;
    }

    spin_lock_irqsave(&g_isr_list_opt_lock, irq_flag);

    disp_chn = &g_disp_isr_mngr.disp_chn[disp];

    ret = disp_isr_search_node(disp_chn, isr_type, isr_cb);
    if (ret < 0) {
        hi_err_disp("Callback is not exist!\n");
        spin_unlock_irqrestore(&g_isr_list_opt_lock, irq_flag);
        return ret;
    }

    index = ret;
    // update node flag
    node_flag_new = disp_chn->isr_list[isr_type].node_flag;
    node_flag_new = node_flag_new & (~((hi_u64)1 << (hi_u64)index));
    disp_chn->isr_list[isr_type].node_flag = node_flag_new;

    /* clear node record */
    disp_chn->isr_list[isr_type].isr_node[index].pf_disp_callback = HI_NULL;
    disp_chn->isr_list[isr_type].isr_node[index].hdst = HI_NULL;

    if (disp_chn->isr_list[isr_type].node_flag == 0) {
        disp_isr_switch_intterrup(disp, isr_type, HI_FALSE);
    }

    hi_info_disp("[isr_unregister success][%s,%d] disp = %d, isr_type = %d, node_index = %d!\n",
                 __FUNCTION__, __LINE__, disp, isr_type, index);

    spin_unlock_irqrestore(&g_isr_list_opt_lock, irq_flag);

    return HI_SUCCESS;
}

hi_s32 disp_isr_set_event(hi_drv_display disp, hi_drv_disp_callback_event event)
{
    disp_isr_chn *disp_chn = HI_NULL;

    disp_check_id(disp);

    if (event >= HI_DRV_DISP_C_EVENT_BUTT) {
        hi_err_disp("Para eEvent invalid !\n");
        return HI_FAILURE;
    }

    if (g_disp_isr_mngr_init_flag < 0) {
        return HI_FAILURE;
    }

    disp_chn = &g_disp_isr_mngr.disp_chn[disp];
    if (disp_chn->enable != HI_TRUE) {
        hi_err_disp("DISP %d is not add to ISR manager!\n", disp);
        return HI_FAILURE;
    }

    disp_chn->cb_info.event_type = event;

    return HI_SUCCESS;
}

hi_s32 disp_isr_set_disp_info(hi_drv_display disp, hi_disp_display_info *disp_info)
{
    disp_isr_chn *disp_chn = HI_NULL;
    disp_check_id(disp);
    isr_check_null_pointer(disp_info);

    if (g_disp_isr_mngr_init_flag < 0) {
        return HI_FAILURE;
    }

    disp_chn = &g_disp_isr_mngr.disp_chn[disp];
    if (disp_chn->enable != HI_TRUE) {
        hi_err_disp("DISP %d is not add to ISR manager!\n", disp);
        return HI_FAILURE;
    }

    disp_chn->cb_info.disp_info = *disp_info;

    return HI_SUCCESS;
}

hi_s32 disp_isr_get_isr_state(hi_drv_display disp, hi_u32 *under_flow,
                              hi_u32 *smmu_err_cnt, disp_isr_debug *isr_debug)
{
    disp_isr_chn *disp_chn = HI_NULL;

    isr_check_null_pointer(under_flow);
    isr_check_null_pointer(isr_debug);

    disp_chn = &g_disp_isr_mngr.disp_chn[disp];

    *smmu_err_cnt = disp_chn->smmu_err_cnt;
    *under_flow = disp_chn->under_flow;
    *isr_debug = disp_chn->isr_debug;

    return HI_SUCCESS;
}

hi_void disp_isr_execute_by_priorty(disp_isr_chn *disp_chn,
                                    hi_drv_disp_callback_type int_type)
{
    hi_u64 isr_bit_map = 0, n = 0;
    hi_drv_disp_callback_priorty isr_execute_priorty = HI_DRV_DISP_CALLBACK_PRIORTY_BUTT;

    isr_bit_map = disp_chn->isr_list[int_type].node_flag;

    for (isr_execute_priorty = HI_DRV_DISP_CALLBACK_PRIORTY_HIGH;
         isr_execute_priorty < HI_DRV_DISP_CALLBACK_PRIORTY_BUTT;
         isr_execute_priorty++) {
        for (n = 0; (n < DEF_DISP_ISR_LIST_LENGTH) && isr_bit_map; n++) {
            if (isr_bit_map & ((hi_u64)1 << (hi_u64)n)) {
                if ((disp_chn->isr_list[int_type].isr_node[n].pf_disp_callback)
                    && (disp_chn->isr_list[int_type].isr_node[n].callback_prior == isr_execute_priorty)) {
                    hi_info_disp("[isr_process][%s,%d] isr_type = %d, prior = %d\n", __FUNCTION__, __LINE__,
                                 int_type, isr_execute_priorty);
                    disp_chn->isr_list[int_type].isr_node[n].pf_disp_callback(disp_chn->isr_list[int_type].isr_node[n].hdst,
                                                                              &disp_chn->cb_info);
                    isr_bit_map = isr_bit_map - ((hi_u64)1 << (hi_u64)n);
                }

                if (!irqs_disabled()) {
                    hi_warn_disp("#######$$$$$$$$$$$............eIntType=%u, n=%d\n", int_type, n);
                }
            }
        }

        if (!isr_bit_map) {
            break;
        }
    }

    return;
}

hi_void disp_isr_process_int_state(hi_u32 *int_mask_status,hi_bool *keep_scene)
{
    hi_s32 ret;
    hi_drv_display disp;
    hi_u32 int_status;
    hi_u32 smmu_status;
    hi_u32 *isr_control_flag = get_global_isr_control_flag();
    *keep_scene = HI_FALSE;

    hal_disp_get_interrupt_state(INT_STATE_TYPE_INTF_UNMASK, &int_status);
    hal_disp_get_interrupt_state(INT_STATE_TYPE_INTF_MASK, int_mask_status);
    hal_disp_get_interrupt_state(INT_STATE_TYPE_SMMU_UNMASK, &smmu_status);

    /* clear interrupt state */
    hal_disp_clean_interrupt_state(INT_STATE_TYPE_INTF_MASK, int_status);
    hal_disp_clean_interrupt_state(INT_STATE_TYPE_SMMU_MASK, smmu_status);

    if ((g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].enable == HI_TRUE) &&
        (int_status & DISPLAY0_BUS_UNDERFLOW_INT)) {
        g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].under_flow++;
        int_status = int_status & (~(hi_u32)DISPLAY0_BUS_UNDERFLOW_INT);
    }

    if ((g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].enable == HI_TRUE) &&
        (int_status & DISPLAY0_BUS_UNDERFLOW_INT)) {
        g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].under_flow++;
        int_status = int_status & (~(hi_u32)DISPLAY0_BUS_UNDERFLOW_INT);

        if ( *isr_control_flag &  ISRFUNC_CTRL_FLAG_STOP_WHEN_LOWBAND) {
            *keep_scene = HI_TRUE;
        }
    }

    if ((g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].enable == HI_TRUE) &&
        (smmu_status != 0)) {
        g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].isr_debug.smmu_err_status = smmu_status;
        g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].isr_debug.smmu_err_addr = hal_disp_get_mmu_error_addr_ns();
        g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].smmu_err_cnt++;

        if ( *isr_control_flag &  ISRFUNC_CTRL_FLAG_STOP_WHEN_SMMU_ERR) {
            *keep_scene = HI_TRUE;
        }
    }

    for (disp = HI_DRV_DISPLAY_0; disp < HI_DRV_DISPLAY_2; disp++) {
        ret = hal_disp_get_dcmp_state(disp);
        if ((g_disp_isr_mngr.disp_chn[disp].enable == HI_TRUE) &&
            (ret != HI_SUCCESS)) {
            g_disp_isr_mngr.disp_chn[disp].isr_debug.decmp_err++;
            hal_disp_clean_dcmp_state(disp);
        }
    }

    ret = hal_disp_get_master_state();
    if (ret != HI_SUCCESS) {
        g_disp_isr_mngr.disp_chn[disp].isr_debug.bus_err++;
        hal_disp_clean_master_state();
    }

    return;
}

hi_s32 check_isr_state(hi_void)
{
    /* if display is not open, return */
    if (g_disp_isr_mngr.chn_number == 0) {
        hi_err_disp("isr isn't open!\n");
        return DEF_DISP_ISR_MAIN_RETURN_VALUE;
    }

    return HI_SUCCESS;
}

static hi_void disp_isr_debug_process(hi_drv_display disp, hi_drv_disp_callback_type int_type,
                                      hi_u64 start_systime, hi_u64 finish_systime)
{
    hi_u64 period_time;
    hi_u64 expected_period_time;
    hi_drv_disp_timing_status current_timing_status;
    disp_isr_chn *disp_chn = &g_disp_isr_mngr.disp_chn[disp];

    disp_chn->isr_debug.isr_state_info[int_type].isr_type = int_type;
    disp_chn->isr_debug.isr_state_info[int_type].isr_cnt++;
    disp_chn->isr_debug.isr_state_info[int_type].isr_time_us = finish_systime - start_systime;

    if (disp_chn->isr_debug.isr_delay_time_ms > 0) {

        (hi_void)drv_disp_get_current_timing_status(disp, &current_timing_status);

        disp_chn->cb_info.disp_info.bottom_field = current_timing_status.btm;
        disp_chn->cb_info.disp_info.vline = current_timing_status.vline;
        period_time = start_systime - disp_chn->isr_debug.isr_state_info[int_type].last_time;
        disp_chn->cb_info.disp_info.last_systime = start_systime;
        disp_chn->cb_info.disp_info.last_vline = disp_chn->cb_info.disp_info.vline;
        disp_chn->isr_debug.isr_state_info[int_type].isr_period_time = period_time;
        expected_period_time = (hi_u64)current_timing_status.circle_time_us +
            (hi_u64)disp_chn->isr_debug.isr_delay_time_ms * ISR_SYSTEM_TIME_US;
        if ((period_time > expected_period_time) ||
            ((finish_systime - start_systime) > disp_chn->isr_debug.isr_delay_time_ms * ISR_SYSTEM_TIME_US)) {
            hi_warn_disp("set-%d-isr,delay:%dms, left -%dms,use-%dms,line-(L-%d,C-%d), count:(%d)\n",
                         disp_chn->isr_debug.isr_delay_time_ms,
                         (period_time - (hi_u64)current_timing_status.circle_time_us),
                         current_timing_status.left_time,
                         (finish_systime - start_systime),
                         disp_chn->cb_info.disp_info.last_vline,
                         disp_chn->cb_info.disp_info.vline,
                         disp_chn->isr_debug.isr_state_info[int_type].isr_cnt);
            disp_chn->isr_debug.isr_state_info[int_type].time_out++;
        }

        disp_chn->isr_debug.isr_state_info[int_type].last_time = start_systime;
        disp_chn->isr_debug.isr_state_info[int_type].vtime = disp_chn->cb_info.disp_info.vline;
    }
}

hi_void get_isr_cnt(hi_drv_display disp, hi_drv_disp_callback_type int_type)
{
    hi_bool btm;
    hi_u32 vcnt;
    hi_u32 int_cnt;
    hi_u32 tmp_cnt;

    hal_disp_get_state(disp, &btm, &vcnt, &int_cnt);

    tmp_cnt = (int_cnt + DISP_INT_CNT_MAX -
               g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].current_int_cnt %
               DISP_INT_CNT_MAX) % DISP_INT_CNT_MAX;

    if (tmp_cnt > 1) {
        g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].lost_interrupt += tmp_cnt - 1;
    }

    g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].current_int_cnt += tmp_cnt;

    if ((g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].current_int_cnt >
        g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].pre_int_cnt) ||
        (g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].current_int_cnt == 0)) {
        g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].pre_int_cnt =
            g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].current_int_cnt;
    } else {
        g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].into_isr_twice_cnt++;
        g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].pre_int_cnt =
            g_disp_isr_mngr.disp_chn[disp].isr_debug.isr_state_info[int_type].current_int_cnt;
    }
}

hi_void disp_isr_process(hi_drv_display disp, hi_drv_disp_callback_type int_type)
{
    hi_u64 start_systime;
    hi_u64 finish_systime;
    disp_isr_chn *disp_chn = &g_disp_isr_mngr.disp_chn[disp];

    get_isr_cnt(disp, int_type);
    start_systime = hi_drv_sys_get_time_stamp_us();
    disp_isr_execute_by_priorty(disp_chn, int_type);
    finish_systime = hi_drv_sys_get_time_stamp_us();

    disp_isr_debug_process(disp, int_type, start_systime, finish_systime);
}

hi_void disp_isr_debug_check(hi_drv_display disp, hi_drv_disp_callback_type int_type)
{
    if (int_type == HI_DRV_DISP_C_INTPOS_0_PERCENT) {
        hi_u32 checksum[DISP_CHECKSUM_TYPE_BUTT];
        hal_disp_debug_get_checksum(disp, &checksum[DISP_CHECKSUM_TYPE_R],
            &checksum[DISP_CHECKSUM_TYPE_G], &checksum[DISP_CHECKSUM_TYPE_B]);

        if (g_disp_isr_mngr.disp_chn[disp].checksum[DISP_CHECKSUM_TYPE_R] != checksum[DISP_CHECKSUM_TYPE_R] ||
            g_disp_isr_mngr.disp_chn[disp].checksum[DISP_CHECKSUM_TYPE_G] != checksum[DISP_CHECKSUM_TYPE_G] ||
            g_disp_isr_mngr.disp_chn[disp].checksum[DISP_CHECKSUM_TYPE_B] != checksum[DISP_CHECKSUM_TYPE_B]) {

            g_disp_isr_mngr.disp_chn[disp].checksum[DISP_CHECKSUM_TYPE_R] = checksum[DISP_CHECKSUM_TYPE_R];
            g_disp_isr_mngr.disp_chn[disp].checksum[DISP_CHECKSUM_TYPE_G] = checksum[DISP_CHECKSUM_TYPE_G];
            g_disp_isr_mngr.disp_chn[disp].checksum[DISP_CHECKSUM_TYPE_B] = checksum[DISP_CHECKSUM_TYPE_B];

            COMMON_DEBUG(CHECK_SUM,"disp %d checksum chg to:R:%x G:%x B:%x\n", disp,
             checksum[DISP_CHECKSUM_TYPE_R], checksum[DISP_CHECKSUM_TYPE_G], checksum[DISP_CHECKSUM_TYPE_B]);
        }

    }

}

hi_s32 disp_isr_main(hi_s32 irq, hi_void *dev_id)
{
    hi_s32 ret = 0;
    hi_drv_display disp;
    hi_drv_disp_callback_type int_type;
    disp_isr_chn *disp_chn = HI_NULL;
    hi_u32 int_status = 0;
    hi_s32 i;
    hi_size_t irq_flag = 0;
    hi_bool keep_scene = HI_FALSE;

    disp_isr_process_int_state(&int_status, &keep_scene);
    if (keep_scene == HI_TRUE) {
        return ret;
    }

    ret = check_isr_state();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    spin_lock_irqsave(&g_isr_list_opt_lock, irq_flag);

    for (i = (hi_s32)HI_DRV_DISPLAY_0; i <= HI_DRV_DISPLAY_1; i++) {
        disp = (hi_drv_display)i;
        disp_chn = &g_disp_isr_mngr.disp_chn[disp];
        if (disp_chn->enable != HI_TRUE) {
            continue;
        }

        for (int_type = HI_DRV_DISP_C_INTPOS_0_PERCENT; int_type < HI_DRV_DISP_C_TYPE_BUTT; int_type++) {
            if ((g_disp_int_table[disp][int_type] & int_status) == 0) {
                continue;
            }

            disp_isr_process(disp, int_type);
            disp_isr_debug_check(disp, int_type);

            int_status = int_status & (~(hi_u32)g_disp_int_table[disp][int_type]);
        }
    }

    if (int_status != 0) {
        hi_fatal_disp("Unespexted interrup 0x%x happened, disp0 = %d, disp1 = %d!\n",
                      int_status,
                      g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_0].enable,
                      g_disp_isr_mngr.disp_chn[HI_DRV_DISPLAY_1].enable);
    }

    spin_unlock_irqrestore(&g_isr_list_opt_lock, irq_flag);

    return DEF_DISP_ISR_MAIN_RETURN_VALUE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */





/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"

#include "hi_drv_osal.h"

#include "hi_reg_common.h"
#include "hi_drv_ao.h"
#include "hi_drv_ai.h"
#include "hal_aiao.h"
#include "hal_aiao_priv.h"
#include "hal_aiao_func.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static osal_semaphore g_hal_aiao_mutex = { HI_NULL };
static osal_atomic g_atm_aiao_init_cnt = { HI_NULL };

static hi_s32 g_aiao_suspend_flag = 0;
static hi_s32 g_aiao_resume_flag = 0;

typedef struct {
    hi_void *port[AIAO_INT_MAX];
    aiao_isr_func *f_reg_isr[AIAO_INT_MAX];

#ifdef HI_AIAO_TIMER_SUPPORT
    aiao_timer_isr_func *f_timer_isr[AIAO_INT_MAX];
    hi_void *timersub[AIAO_INT_MAX];
#endif

    hi_void *sub[AIAO_INT_MAX]; /* only for alsa isr HI_ALSA_AI_SUPPORT */
} aiao_global_source;

/* private state */
static aiao_global_source g_aiao_rm;

#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
static aiao_i2s_board_config g_i2s_board_settings[AIAO_MAX_EXT_I2S_NUMBER];
static hi_void hal_aiao_board_i2s_reset(hi_void)
{
    aiao_i2s_board_config *i2s_borad;
    hi_s32 n;

    for (n = 0; n < AIAO_MAX_EXT_I2S_NUMBER; n++) {
        i2s_borad = &g_i2s_board_settings[n];
        i2s_borad->init_flag = 0;
        i2s_borad->tx_port_id = AIAO_PORT_MAX;
        i2s_borad->rx_port_id = AIAO_PORT_MAX;
    }

#if defined(HI_I2S0_SUPPORT)
    i2s_borad = &g_i2s_board_settings[0];

    i2s_borad->tx_port_id = AIAO_PORT_TX0;
    i2s_borad->tx_source = AIAO_TX0;
    i2s_borad->tx_crg_source = AIAO_TX_CRG0;
    i2s_borad->tx_crg_mode = AIAO_CRG_MODE_MASTER;

    i2s_borad->rx_port_id = AIAO_PORT_RX0;
    i2s_borad->rx_source = AIAO_RX0;
    /* hi3716cv200 aiao external i2s rx allways work at AIAO_CRG_MODE_DUPLICATE */
    i2s_borad->rx_crg_source = AIAO_TX_CRG0;
    i2s_borad->rx_crg_mode = AIAO_CRG_MODE_DUPLICATE;
#endif

#if defined(HI_I2S1_SUPPORT)
    i2s_borad = &g_i2s_board_settings[1];

    i2s_borad->tx_port_id = AIAO_PORT_TX1;
    i2s_borad->tx_source = AIAO_TX1;
    i2s_borad->tx_crg_source = AIAO_TX_CRG1;
    i2s_borad->tx_crg_mode = AIAO_CRG_MODE_MASTER;

#ifdef HI_FPGA
    i2s_borad->rx_port_id = AIAO_PORT_RX3;
    i2s_borad->rx_source = AIAO_RX2;
    i2s_borad->rx_crg_source = AIAO_TX_CRG2;
#else
    i2s_borad->rx_port_id = AIAO_PORT_RX1;
    i2s_borad->rx_source = AIAO_RX1;
    /* hi3716cv200 aiao external i2s rx allways work at AIAO_CRG_MODE_DUPLICATE */
    i2s_borad->rx_crg_source = AIAO_TX_CRG1;
#endif
    i2s_borad->rx_crg_mode = AIAO_CRG_MODE_DUPLICATE;
#endif
}

static hi_bool aiao_validate_board_i2s(aiao_port_id port_id)
{
    if ((port_id == AIAO_PORT_TX0) || (port_id == AIAO_PORT_TX1) || (port_id == AIAO_PORT_RX0) ||
        (port_id == AIAO_PORT_RX1)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_void aiao_set_i2s_clk(aiao_port_id port_id, aiao_if_attr *if_attr)
{
    hi_s32 n;

    for (n = 0; n < AIAO_MAX_EXT_I2S_NUMBER; n++) {
        if ((g_i2s_board_settings[n].rx_port_id == port_id) && !(g_i2s_board_settings[n].init_flag & 0x2)) {
            /* set rx port in use of tx clock */
            if (if_attr->crg_mode == AIAO_CRG_MODE_DUPLICATE) {
                (hi_void)i_hal_aiao_set_i2s_master_clk(g_i2s_board_settings[n].tx_port_id, if_attr);
            } else if (if_attr->crg_mode == AIAO_CRG_MODE_SLAVE) {
                (hi_void)i_hal_aiao_set_i2_slave_clk(g_i2s_board_settings[n].tx_port_id, if_attr);
            }
        }
    }
}
#endif

static hi_s32 aiao_check_board_i2s_open_attr(aiao_port_id port_id, const aiao_if_attr *if_attr)
{
    hi_s32 ret = HI_SUCCESS;

#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
    hi_bool valid_board_i2s = aiao_validate_board_i2s(port_id);
    if (valid_board_i2s == HI_TRUE) {
        ret = hal_aiao_check_board_i2s_open_attr(port_id, if_attr);
    }
#endif

    return ret;
}

static hi_void aiao_set_board_i2s_open_attr(aiao_port_id port_id, const aiao_if_attr *if_attr)
{
#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
    hi_bool valid_board_i2s = aiao_validate_board_i2s(port_id);
    if (valid_board_i2s == HI_TRUE) {
        hal_aiao_set_board_i2s_open_attr(port_id, if_attr);
        aiao_set_i2s_clk(port_id, (aiao_if_attr *)if_attr);
    }
#endif
}

#ifdef HI_AIAO_TIMER_SUPPORT
static hi_void aiao_timer_isr(hi_u32 id)
{
    aiao_timer_id timer_id = ID2TIMER(id);

    if (g_aiao_rm.f_timer_isr[id] != HI_NULL) {
        g_aiao_rm.f_timer_isr[id](timer_id, g_aiao_rm.timersub[id]);
    }

    i_hal_timer_clear_timer(timer_id);
}
#endif

static hi_void aiao_port_isr(hi_u32 id)
{
    hi_u32 port_int_raw_status;
    aiao_port *port = HI_NULL;
    aiao_port_id port_id;
    port_id = ID2PORT(id);
    port_int_raw_status = i_hal_aiao_get_int_status_raw(port_id);
    port = (aiao_port *)g_aiao_rm.port[id];

    if (port != HI_NULL) {
        if (g_aiao_rm.f_reg_isr[id] != HI_NULL) {
            g_aiao_rm.f_reg_isr[id](port_id, port_int_raw_status, g_aiao_rm.sub[id]);
        }
    }

    i_hal_aiao_clr_int(port_id, port_int_raw_status);
}

static irqreturn_t aiao_isr(hi_s32 irq, hi_void *dev_id)
{
    hi_u32 id;
    hi_u32 top_int_status;
    top_int_status = i_hal_aiao_get_top_int_status();

    for (id = 0; id < AIAO_INT_MAX; id++) {
        if (int_status(id, top_int_status)) {
#ifdef HI_AIAO_TIMER_SUPPORT
            if ((id >= AIAO_TIMER_INT0) && (id <= AIAO_TIMER_INT1)) {
                aiao_timer_isr(id);
            } else
#endif
            {
                aiao_port_isr(id);
            }
        }
    }

    return IRQ_HANDLED;
}

hi_void hal_aiao_power_off(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    aiao_hw_power_off();

    osal_sem_up(&g_hal_aiao_mutex);

    return;
}

hi_s32 hal_aiao_suspend(hi_void)
{
    if (!g_aiao_suspend_flag) {
        hal_aiao_free_isr();
        g_aiao_suspend_flag = 1;
    }

    hal_aiao_power_off();
    g_aiao_resume_flag = 0;

    return HI_SUCCESS;
}

hi_s32 hal_aiao_resume(hi_void)
{
    hi_s32 ret;

    if (!g_aiao_resume_flag) {
        ret = hal_aiao_power_on();
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_power_on, ret);
            return ret;
        }

        ret = hal_aiao_request_isr();
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_request_isr, ret);
            return ret;
        }

#if defined(HI_AIAO_TIMER_SUPPORT)
        /* support hard time, so can't enbale timer0(bit[27]) and timer1(bit[28]) interrupt */
        i_hal_aiao_set_top_int(0x07fff0ff); /* enable all top interrupt */
#else
        /* don't support hard time */
        i_hal_aiao_set_top_int(0xe7fff0ff); /* enable all top interrupt */
#endif
        g_aiao_resume_flag = 1;
    }

    g_aiao_suspend_flag = 0;
    return HI_SUCCESS;
}

hi_s32 hal_aiao_power_on(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
    hal_aiao_board_i2s_reset();
#endif

    ret = aiao_hw_power_on();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(aiao_hw_power_on, ret);
    }

    osal_sem_up(&g_hal_aiao_mutex);
    return ret;
}

hi_s32 hal_aiao_request_isr(hi_void)
{
    hi_s32 ret;

    ret = hi_drv_osal_request_irq(AIAO_IRQ_NUM, aiao_isr, IRQF_SHARED, "aiao", aiao_isr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_osal_request_irq, ret);
        HI_ERR_PRINT_U32(AIAO_IRQ_NUM);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void hal_aiao_free_isr(hi_void)
{
    /* free irq */
    hi_drv_osal_free_irq(AIAO_IRQ_NUM, "aiao", aiao_isr);
}

static hi_void aiao_osal_init(hi_void)
{
    if (g_hal_aiao_mutex.sem == HI_NULL) {
        osal_sem_init(&g_hal_aiao_mutex, 1);
        osal_atomic_init(&g_atm_aiao_init_cnt);
    }
}

static hi_void aiao_osal_deinit(hi_void)
{
    if (osal_atomic_read(&g_atm_aiao_init_cnt) == 0) {
        if (g_hal_aiao_mutex.sem != HI_NULL) {
            osal_atomic_destory(&g_atm_aiao_init_cnt);
            osal_sem_destory(&g_hal_aiao_mutex);
        }
    }
}

/* global function */
hi_s32 hal_aiao_init(hi_void)
{
    hi_s32 ret;
    hi_u32 id;

    aiao_osal_init();

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (osal_atomic_inc_return(&g_atm_aiao_init_cnt) == 1) {
        ret = i_hal_aiao_init();
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(i_hal_aiao_init, ret);
            goto out0;
        }

        /* init aiao state */
        for (id = 0; id < AIAO_INT_MAX; id++) {
            g_aiao_rm.port[id] = HI_NULL;
        }

        ret = hal_aiao_request_isr();
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aiao_request_isr, ret);
            goto out1;
        }

#ifdef HI_AIAO_TIMER_SUPPORT
        i_hal_aiao_set_top_int(0x07fff0ff); /* enable all top interrupt */
#else
        i_hal_aiao_set_top_int(0xe7fff0ff); /* enable all top interrupt */
#endif

#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
        hal_aiao_board_i2s_reset();
#endif
    }

    osal_sem_up(&g_hal_aiao_mutex);
    return HI_SUCCESS;

out1:
    i_hal_aiao_de_init();

out0:
    osal_atomic_dec_return(&g_atm_aiao_init_cnt);
    osal_sem_up(&g_hal_aiao_mutex);
    aiao_osal_deinit();
    return ret;
}

hi_void hal_aiao_de_init(hi_void)
{
    hi_s32 ret;
    hi_u32 id;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    if (osal_atomic_dec_return(&g_atm_aiao_init_cnt) == 0) {
        i_hal_aiao_set_top_int(0); /* disable all top interrupt */
        hal_aiao_free_isr();

        /* close port */
        for (id = 0; id < AIAO_INT_MAX; id++) {
            if (g_aiao_rm.port[id] != HI_NULL) {
                i_hal_aiao_close((aiao_port *)g_aiao_rm.port[id]);
            }

            g_aiao_rm.port[id] = HI_NULL;
            g_aiao_rm.f_reg_isr[id] = HI_NULL;
        }

        i_hal_aiao_de_init();
    }

    osal_sem_up(&g_hal_aiao_mutex);
    aiao_osal_deinit();
}

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
hi_void hal_aiao_get_hw_capability(hi_u32 *capability)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    i_hal_aiao_get_hw_capability(capability);
    osal_sem_up(&g_hal_aiao_mutex);
}

hi_void hal_aiao_get_hw_version(hi_u32 *version)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    i_hal_aiao_get_hw_version(version);
    osal_sem_up(&g_hal_aiao_mutex);
}

hi_void hal_aiao_dbg_rw_reg(aiao_dbg_reg *reg)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    i_hal_aiao_dbg_rw_reg(reg);
    osal_sem_up(&g_hal_aiao_mutex);
}

hi_void hal_aiao_set_top_int(hi_u32 multibit)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    i_hal_aiao_set_top_int(multibit);
    osal_sem_up(&g_hal_aiao_mutex);
}

hi_u32 hal_aiao_get_top_int_raw_status(hi_void)
{
    hi_s32 ret;
    hi_u32 int_raw;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return 0;
    }

    int_raw = i_hal_aiao_get_top_int_raw_status();
    osal_sem_up(&g_hal_aiao_mutex);
    return int_raw;
}

hi_u32 hal_aiao_get_top_int_status(hi_void)
{
    hi_s32 ret;
    hi_u32 int_status;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return 0;
    }

    int_status = i_hal_aiao_get_top_int_status();
    osal_sem_up(&g_hal_aiao_mutex);
    return int_status;
}
#endif

/* global port function */
hi_s32 hal_aiao_open(aiao_port_id port_id, const aiao_port_user_cfg *config)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    aiao_port *port = HI_NULL;
    aiao_isr_func *isr_func = HI_NULL;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        HI_LOG_ERR("AIAO port had been open\n");
        HI_FATAL_PRINT_U32(id);
        osal_sem_up(&g_hal_aiao_mutex);
        return HI_FAILURE;
    }

    ret = aiao_check_board_i2s_open_attr(port_id, &config->if_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(aiao_check_board_i2s_open_attr, ret);
        osal_sem_up(&g_hal_aiao_mutex);
        return ret;
    }

    ret = i_hal_aiao_open(port_id, config, &port, &isr_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aiao_open, ret);
        osal_sem_up(&g_hal_aiao_mutex);
        return ret;
    }

    g_aiao_rm.port[id] = (hi_void *)port;
    g_aiao_rm.f_reg_isr[id] = isr_func;
    g_aiao_rm.sub[id] = config->substream;

    aiao_set_board_i2s_open_attr(port_id, &config->if_attr);

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_void hal_aiao_close(aiao_port_id port_id)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_close((aiao_port *)g_aiao_rm.port[id]);
        g_aiao_rm.port[id] = HI_NULL;
#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
        hal_aiao_destroy_board_i2s_open_attr(port_id);
#endif
    }
    osal_sem_up(&g_hal_aiao_mutex);
}

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
hi_s32 hal_aiao_set_op_type(aiao_port_id port_id, aiao_op_type op_type)
{
    hi_u32 id = PORT2ID(port_id);
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_op_type(port_id, op_type);
    } else {
        HI_LOG_ERR("g_aiao_rm.h_port[id] nulll \n");
    }
    osal_sem_up(&g_hal_aiao_mutex);
    return ret;
}
#endif

hi_s32 hal_aiao_set_attr(aiao_port_id port_id, aiao_port_attr *attr)
{
    hi_u32 id = PORT2ID(port_id);
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    ret = aiao_check_board_i2s_open_attr(port_id, &attr->if_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(aiao_check_board_i2s_open_attr, ret);
        osal_sem_up(&g_hal_aiao_mutex);
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_attr((aiao_port *)g_aiao_rm.port[id], attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(i_hal_aiao_set_attr, ret);
            osal_sem_up(&g_hal_aiao_mutex);
            return ret;
        }
    }

    aiao_set_board_i2s_open_attr(port_id, &attr->if_attr);

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
static hi_s32 hal_aiao_check(const aiao_port_id port_id)
{
    hi_u32 capability;
    hi_u32 id;

    hal_aiao_get_hw_capability(&capability);

    for (id = 0; id < AIAO_INT_MAX; id++) {
        if (0x01 & (capability >> id)) {
            if (ID2PORT(id) == port_id) {
                return HI_SUCCESS;
            }
        }
    }

    return HI_FAILURE;
}

hi_s32 hal_aiao_open_veri(aiao_port_id port_id, const aiao_port_user_cfg *config)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    aiao_isr_func *isr_func = HI_NULL;
    aiao_port *port = HI_NULL;

    ret = hal_aiao_check(port_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aiao_check, ret);
        return ret;
    }

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        HI_LOG_ERR("AIAO port had been open!\n");
        HI_ERR_PRINT_H32(port_id);
        osal_sem_up(&g_hal_aiao_mutex);
        return HI_FAILURE;
    }

    ret = i_hal_aiao_open(port_id, config, &port, &isr_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aiao_open, ret);
        HI_ERR_PRINT_H32(port_id);
        osal_sem_up(&g_hal_aiao_mutex);
        return ret;
    }

    g_aiao_rm.port[id] = (hi_void *)port;
    g_aiao_rm.f_reg_isr[id] = isr_func;

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_void hal_aiao_close_veri(aiao_port_id port_id)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_close((aiao_port *)g_aiao_rm.port[id]);
        g_aiao_rm.port[id] = HI_NULL;
    }

    osal_sem_up(&g_hal_aiao_mutex);
}
#endif

hi_s32 hal_aiao_get_attr(aiao_port_id port_id, aiao_port_attr *attr)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_get_attr((aiao_port *)g_aiao_rm.port[id], attr);
    } else {
        ret = HI_FAILURE;
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

#ifdef HI_AIAO_TIMER_SUPPORT
hi_s32 hal_aiao_t_create(aiao_timer_id timer_id, const aiao_timer_create *param)
{
    hi_s32 ret;
    aiao_timer_isr_func *isr = HI_NULL;
    hi_u32 id = TIMER2ID(timer_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] == HI_NULL) {
        aiao_timer port;
        isr = param->timer_isr_func;
        ret = i_hal_aiao_t_create(ID2TIMER(id), param, &port, &isr);
        if (ret == HI_SUCCESS) {
            g_aiao_rm.port[id] = (hi_void *)port;
            g_aiao_rm.f_timer_isr[id] = isr;
            g_aiao_rm.timersub[id] = param->substream;
        }
    }

    osal_sem_up(&g_hal_aiao_mutex);

    HI_LOG_DBG("create timer success\n");
    HI_DBG_PRINT_H32(timer_id);

    return ret;
}

hi_void hal_aiao_t_destroy(aiao_timer_id timer_id)
{
    hi_s32 ret;
    hi_u32 id = TIMER2ID(timer_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_t_destroy((aiao_timer)g_aiao_rm.port[id]);
        g_aiao_rm.port[id] = HI_NULL;
    }

    osal_sem_up(&g_hal_aiao_mutex);

    HI_LOG_DBG("destroy timer success\n");
    HI_DBG_PRINT_H32(timer_id);
}

hi_s32 hal_aiao_t_set_timer_attr(aiao_timer_id timer_id, const aiao_timer_attr *attr_param)
{
    hi_s32 ret;
    hi_u32 id = TIMER2ID(timer_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_t_set_timer_attr((aiao_timer)g_aiao_rm.port[id], attr_param);
    } else {
        HI_LOG_ERR("timer not create\n");
        HI_ERR_PRINT_H32(timer_id);
        osal_sem_up(&g_hal_aiao_mutex);
        return HI_FAILURE;
    }

    osal_sem_up(&g_hal_aiao_mutex);

    HI_DBG_PRINT_H32(timer_id);
    HI_DBG_PRINT_U32(attr_param->config);
    HI_DBG_PRINT_U32(attr_param->if_attr.rate);
    HI_DBG_PRINT_U32(attr_param->if_attr.fclk_div);
    HI_DBG_PRINT_U32(attr_param->if_attr.bclk_div);

    return ret;
}

hi_s32 hal_aiao_t_set_timer_enable(aiao_timer_id timer_id, hi_bool enable)
{
    hi_s32 ret;
    hi_u32 id = TIMER2ID(timer_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_t_set_timer_enable((aiao_timer)g_aiao_rm.port[id], enable);
    } else {
        HI_LOG_ERR("timer not create\n");
        HI_ERR_PRINT_H32(timer_id);
        osal_sem_up(&g_hal_aiao_mutex);
        return HI_FAILURE;
    }

    osal_sem_up(&g_hal_aiao_mutex);

    HI_LOG_DBG("destroy timer success\n");
    HI_DBG_PRINT_H32(timer_id);

    return ret;
}

hi_s32 hal_aiao_t_get_status(aiao_timer_id timer_id, aiao_timer_status *param)
{
    hi_s32 ret;
    hi_u32 id = TIMER2ID(timer_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_t_get_status((aiao_timer)g_aiao_rm.port[id], param);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_void hal_aiao_t_timer_process(aiao_timer_id timer_id, hi_void *pst)
{
    hi_u32 id = TIMER2ID(timer_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_t_timer_process((aiao_timer)g_aiao_rm.port[id]);
    }
}
#endif

hi_s32 hal_aiao_start(aiao_port_id port_id)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_start((aiao_port *)g_aiao_rm.port[id]);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_stop(aiao_port_id port_id, aiao_port_stopmode stop_mode)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_stop((aiao_port *)g_aiao_rm.port[id], stop_mode);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_mute(aiao_port_id port_id, hi_bool mute)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_mute((aiao_port *)g_aiao_rm.port[id], mute);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_set_volume(aiao_port_id port_id, hi_u32 volume_db)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_volume((aiao_port *)g_aiao_rm.port[id], volume_db);
    }

    return ret;
}

hi_s32 hal_aiao_set_spdif_category_code(aiao_port_id port_id, aiao_spdif_categorycode category_code)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_spdif_category_code((aiao_port *)g_aiao_rm.port[id], category_code);
    }

    osal_sem_up(&g_hal_aiao_mutex);
    return ret;
}

hi_s32 hal_aiao_set_spdif_scms_mode(aiao_port_id port_id, aiao_spdif_scms_mode scms_mode)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_spdif_scms_mode((aiao_port *)g_aiao_rm.port[id], scms_mode);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_set_track_mode(aiao_port_id port_id, aiao_track_mode track_mode)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_track_mode((aiao_port *)g_aiao_rm.port[id], track_mode);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_set_bypass(aiao_port_id port_id, hi_bool by_bass)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_bypass((aiao_port *)g_aiao_rm.port[id], by_bass);
    }

    return ret;
}

hi_s32 hal_aiao_set_mode(aiao_port_id port_id, hi_bool mode)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_mode((aiao_port *)g_aiao_rm.port[id], mode);
    }

    return ret;
}

hi_s32 hal_aiao_get_user_congfig(aiao_port_id port_id, aiao_port_user_cfg *user_config)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_get_user_congfig((aiao_port *)g_aiao_rm.port[id], user_config);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_get_status(aiao_port_id port_id, aiao_port_stauts *proc_info)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_get_status((aiao_port *)g_aiao_rm.port[id], proc_info);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_s32 hal_aiao_select_spdif_source(aiao_port_id port_id, aiao_spdifport_source src_chn_id)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_select_spdif_source((aiao_port *)g_aiao_rm.port[id], src_chn_id);
    }

    return ret;
}

hi_s32 hal_aiao_set_spdif_out_port(aiao_port_id port_id, hi_s32 en)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_spdif_out_port((aiao_port *)g_aiao_rm.port[id], en);
    }

    return ret;
}

hi_s32 hal_aiao_set_i2s_sd_select(aiao_port_id port_id, aiao_i2s_data_sel *sd_sel)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);
    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_set_i2s_sd_select((aiao_port *)g_aiao_rm.port[id], sd_sel);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

#if defined(HI_AUDIO_AI_SUPPORT)
hi_u32 hal_aiao_read_data_not_up_rptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size, hi_u32 *rptr, hi_u32 *wptr)
{
    hi_u32 read_bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        read_bytes = i_hal_aiao_read_data_not_up_rptr((aiao_port *)g_aiao_rm.port[id], dest, dest_size, rptr, wptr);
    }

    return read_bytes;
}
#endif

/* port buffer function */
hi_u32 hal_aiao_read_data(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size)
{
    hi_u32 read_bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        read_bytes = i_hal_aiao_read_data((aiao_port *)g_aiao_rm.port[id], dest, dest_size);
    }

    return read_bytes;
}

hi_u32 hal_aiao_write_data(aiao_port_id port_id, hi_u8 *src, hi_u32 src_len)
{
    hi_u32 write_bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        write_bytes = i_hal_aiao_write_data((aiao_port *)g_aiao_rm.port[id], src, src_len);
    }

    return write_bytes;
}

#if defined(HI_AUDIO_AI_SUPPORT) || defined(HI_AIAO_VERIFICATION_SUPPORT)
hi_u32 hal_aiao_prepare_data(aiao_port_id port_id, hi_u8 *src, hi_u32 src_len)
{
    hi_u32 write_bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        write_bytes = i_hal_aiao_prepare_data((aiao_port *)g_aiao_rm.port[id], src, src_len);
    }

    return write_bytes;
}

hi_u32 hal_aiao_query_buf_data_provide_rptr(aiao_port_id port_id, hi_u32 *rptr)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_query_buf_data_provide_rptr((aiao_port *)g_aiao_rm.port[id], rptr);
    }

    return bytes;
}

hi_u32 hal_aiao_query_buf_data(aiao_port_id port_id)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return 0;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_query_buf_data((aiao_port *)g_aiao_rm.port[id]);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return bytes;
}

hi_u32 hal_aiao_query_buf_free(aiao_port_id port_id)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return 0;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_query_buf_free((aiao_port *)g_aiao_rm.port[id]);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return bytes;
}

hi_u32 hal_aiao_update_rptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_update_rptr((aiao_port *)g_aiao_rm.port[id], dest, dest_size);
    }

    return bytes;
}

hi_u32 hal_aiao_update_wptr(aiao_port_id port_id, hi_u8 *src, hi_u32 src_len)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_update_wptr((aiao_port *)g_aiao_rm.port[id], src, src_len);
    }

    return bytes;
}
#endif

hi_void hal_aiao_get_delay_ms(aiao_port_id port_id, hi_u32 *delayms)
{
    hi_u32 id = PORT2ID(port_id);
    *delayms = 0;

    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_get_delay_ms((aiao_port *)g_aiao_rm.port[id], delayms);
        return;
    }

    return;
}

hi_s32 hal_aiao_get_rbf_attr(aiao_port_id port_id, aiao_rbuf_attr *rbf_attr)
{
    hi_s32 ret;
    hi_u32 id = PORT2ID(port_id);

    ret = osal_sem_down_interruptible(&g_hal_aiao_mutex);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("lock g_hal_aiao_mutex failed\n");
        return ret;
    }

    if (g_aiao_rm.port[id] != HI_NULL) {
        ret = i_hal_aiao_get_rbf_attr((aiao_port *)g_aiao_rm.port[id], rbf_attr);
    }

    osal_sem_up(&g_hal_aiao_mutex);

    return ret;
}

hi_void hal_aiao_proc_statistics(aiao_port_id port_id, hi_u32 int_status, hi_void *pst)
{
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_proc_statistics((aiao_port *)g_aiao_rm.port[id], int_status);
    }
}

hi_void hal_aiao_set_spdif_port_en(aiao_port_id port_id, hi_s32 en)
{
    i_hal_aiao_set_spdif_port_en(port_id, en);
}

#ifdef HI_ALSA_AI_SUPPORT
hi_u32 hal_aiao_alsa_update_rptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_alsa_update_rptr((aiao_port *)g_aiao_rm.port[id], dest, dest_size);
    }

    return bytes;
}

hi_u32 hal_aiao_alsa_update_wptr(aiao_port_id port_id, hi_u8 *dest, hi_u32 dest_size)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_alsa_update_wptr((aiao_port *)g_aiao_rm.port[id], dest, dest_size);
    }

    return bytes;
}

hi_u32 hal_aiao_p_alsa_flush(aiao_port_id port_id)
{
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        i_hal_aiao_alsa_flash((aiao_port *)g_aiao_rm.port[id]);
    }

    return HI_SUCCESS;
}

hi_u32 hal_aiao_alsa_query_write_pos(aiao_port_id port_id)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_alsa_query_write_pos((aiao_port *)g_aiao_rm.port[id]);
    }

    return bytes;
}

hi_u32 hal_aiao_alsa_query_read_pos(aiao_port_id port_id)
{
    hi_u32 bytes = 0;
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_rm.port[id] != HI_NULL) {
        bytes = i_hal_aiao_alsa_query_read_pos((aiao_port *)g_aiao_rm.port[id]);
    }

    return bytes;
}
#endif

static aiao_port_user_cfg g_aiao_tx_i2s_default_open_attr = {
    .if_attr = {
        .crg_mode = AIAO_CRG_MODE_MASTER,
        .ch_num = AIAO_I2S_CHNUM_2,
        .bit_depth = AIAO_BIT_DEPTH_16,
        .rise_edge = AIAO_MODE_EDGE_RISE,
        .rate = AIAO_SAMPLE_RATE_48K,
        .fclk_div = 64,
        .bclk_div = 4,
        .crg_source = AIAO_TX_CRG0,
        .pcm_delay_cycles = 1,
        .i2s_mode = AIAO_MODE_I2S,
        .source = AIAO_TX0,
        .sd0 = AIAO_I2S_SD0,
        .sd1 = AIAO_I2S_SD1,
        .sd2 = AIAO_I2S_SD2,
        .sd3 = AIAO_I2S_SD3,
        .multislot = HI_FALSE,
    },
    .buf_config = {
        .period_buf_size = aiao_df_period_buf_size,
        .period_number = PERIOND_NUM,
    },
    .track_mode = AIAO_TRACK_MODE_STEREO,
    .fade_in_rate = aiao_df_fade_in_rate,
    .fade_out_rate = aiao_df_fade_out_rate,
    .mute = HI_FALSE,
    .mute_fade = HI_TRUE,
    .volume_db = 0x79,
    .by_bass = HI_FALSE,
    .isr_func = hal_aiao_proc_statistics,
};

static aiao_port_user_cfg g_aiao_tx_hdmi_hbr_default_open_attr = {
    .if_attr = {
        .crg_mode = AIAO_CRG_MODE_MASTER,
        .ch_num = AIAO_I2S_CHNUM_8,
        .bit_depth = AIAO_BIT_DEPTH_16,
        .rise_edge = AIAO_MODE_EDGE_RISE,
        .rate = AIAO_SAMPLE_RATE_192K,
        .fclk_div = 64,
        .bclk_div = 4,
        .crg_source = AIAO_TX_CRG0,
        .pcm_delay_cycles = 1,
        .i2s_mode = AIAO_MODE_I2S,
        .source = AIAO_TX0,
        .sd0 = AIAO_I2S_SD0,
        .sd1 = AIAO_I2S_SD1,
        .sd2 = AIAO_I2S_SD2,
        .sd3 = AIAO_I2S_SD3,
        .multislot = HI_FALSE,
    },
    .buf_config = {
        .period_buf_size = aiao_df_period_buf_size * 16,
        .period_number = PERIOND_NUM,
    },
    .track_mode = AIAO_TRACK_MODE_STEREO,
    .fade_in_rate = aiao_df_fade_in_rate,
    .fade_out_rate = aiao_df_fade_out_rate,
    .mute = HI_FALSE,
    .mute_fade = HI_TRUE,
    .volume_db = 0x79,
    .by_bass = HI_TRUE,
    .isr_func = hal_aiao_proc_statistics,
};

static aiao_port_user_cfg g_aiao_tx_hdmi_i2s_default_open_attr = {
    .if_attr = {
        .crg_mode = AIAO_CRG_MODE_MASTER,
        .ch_num = AIAO_I2S_CHNUM_2,
        .bit_depth = AIAO_BIT_DEPTH_16,
        .rise_edge = AIAO_MODE_EDGE_RISE,
        .rate = AIAO_SAMPLE_RATE_48K,
        .fclk_div = 64,
        .bclk_div = 4,
        .crg_source = AIAO_TX_CRG0,
        .pcm_delay_cycles = 1,
        .i2s_mode = AIAO_MODE_I2S,
        .source = AIAO_TX0,
        .sd0 = AIAO_I2S_SD0,
        .sd1 = AIAO_I2S_SD1,
        .sd2 = AIAO_I2S_SD2,
        .sd3 = AIAO_I2S_SD3,
        .multislot = HI_FALSE,
    },
    .buf_config = {
        .period_buf_size = aiao_df_period_buf_size,
        .period_number = PERIOND_NUM,
    },
    .track_mode = AIAO_TRACK_MODE_STEREO,
    .fade_in_rate = aiao_df_fade_in_rate,
    .fade_out_rate = aiao_df_fade_out_rate,
    .mute = HI_FALSE,
    .mute_fade = HI_TRUE,
    .volume_db = 0x79,
    .by_bass = HI_FALSE,
    .isr_func = hal_aiao_proc_statistics,
    .op_type = SND_OP_TYPE_I2S,
};

static aiao_port_user_cfg g_aiao_tx_spd_default_open_attr = {
    .if_attr = {
        .crg_mode = AIAO_CRG_MODE_MASTER,
        .ch_num = AIAO_I2S_CHNUM_2,
        .bit_depth = AIAO_BIT_DEPTH_16,
        .rate = AIAO_SAMPLE_RATE_48K,
        .fclk_div = 128,
        .bclk_div = 2,
    },
    .buf_config = {
        .period_buf_size = aiao_df_period_buf_size,
        .period_number = PERIOND_NUM,
    },
    .track_mode = AIAO_TRACK_MODE_STEREO,
    .fade_in_rate = aiao_df_fade_in_rate,
    .fade_out_rate = aiao_df_fade_out_rate,
    .mute = HI_FALSE,
    .mute_fade = HI_TRUE,
    .volume_db = 0x79,
    .by_bass = HI_FALSE,
    .isr_func = hal_aiao_proc_statistics,
    .op_type = SND_OP_TYPE_I2S,
};

static aiao_i2s_source g_tx_i2s_source_tab[] = {
    AIAO_TX0,
    AIAO_TX1,
    AIAO_TX2,
    AIAO_TX3,
    AIAO_TX4,
    AIAO_TX5,
    AIAO_TX6,
    AIAO_TX7,
};

static aiao_crg_source g_tx_crg_source_tab[] = {
    AIAO_TX_CRG0,
    AIAO_TX_CRG1,
    AIAO_TX_CRG2,
    AIAO_TX_CRG3,
    AIAO_TX_CRG4,
    AIAO_TX_CRG5,
    AIAO_TX_CRG6,
    AIAO_TX_CRG7,
};

#if defined(HI_AUDIO_AI_SUPPORT)
static aiao_i2s_source g_rx_i2s_source_tab[] = {
    AIAO_RX0,
    AIAO_RX1,
    AIAO_RX2,
    AIAO_RX3,
    AIAO_RX4,
    AIAO_RX5,
    AIAO_RX6,
    AIAO_RX7,
};

static aiao_crg_source g_rx_crg_source_tab[] = {
    AIAO_RX_CRG0,
    AIAO_RX_CRG1,
    AIAO_RX_CRG2,
    AIAO_RX_CRG3,
    AIAO_RX_CRG4,
    AIAO_RX_CRG5,
    AIAO_RX_CRG6,
    AIAO_RX_CRG7,
};
#endif

hi_void hal_aiao_set_tx_i2s_df_attr(aiao_port_id port_id, aiao_isr_func *isr_func)
{
    g_aiao_tx_i2s_default_open_attr.isr_func = isr_func;
}

hi_void hal_aiao_get_tx_i2s_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_i2s_default_open_attr, sizeof(aiao_port_user_cfg));
    attr->if_attr.source = g_tx_i2s_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_source = g_tx_crg_source_tab[PORT2CHID(port_id)];
}

hi_void hal_aiao_get_hdmi_hbr_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_hdmi_hbr_default_open_attr, sizeof(aiao_port_user_cfg));
    attr->if_attr.source = g_tx_i2s_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_source = g_tx_crg_source_tab[PORT2CHID(port_id)];
}

hi_void hal_aiao_get_hdmi_i2s_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_hdmi_i2s_default_open_attr, sizeof(aiao_port_user_cfg));
    attr->if_attr.source = g_tx_i2s_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_source = g_tx_crg_source_tab[PORT2CHID(port_id)];
}

hi_void hal_aiao_get_tx_spd_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_spd_default_open_attr, sizeof(aiao_port_user_cfg));
}

#if defined(HI_AUDIO_AI_SUPPORT)
hi_void hal_aiao_get_rx_adc_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_i2s_default_open_attr, sizeof(aiao_port_user_cfg));

    attr->if_attr.source = g_rx_i2s_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_source = g_tx_crg_source_tab[PORT2CHID(AIAO_PORT_TX2)];
    attr->if_attr.crg_mode = AIAO_CRG_MODE_DUPLICATE;
}

hi_void hal_aiao_get_rx_sif_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_i2s_default_open_attr, sizeof(aiao_port_user_cfg));

    attr->if_attr.source = g_rx_i2s_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_source = g_rx_crg_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_mode = AIAO_CRG_MODE_SLAVE;
}

hi_void hal_aiao_get_rx_hdmi_df_attr(aiao_port_id port_id, aiao_port_user_cfg *attr)
{
    memcpy(attr, &g_aiao_tx_i2s_default_open_attr, sizeof(aiao_port_user_cfg));
    attr->if_attr.source = g_rx_i2s_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_source = g_rx_crg_source_tab[PORT2CHID(port_id)];
    attr->if_attr.crg_mode = AIAO_CRG_MODE_SLAVE;
}
#endif

#if defined(HI_I2S0_SUPPORT) || defined(HI_I2S1_SUPPORT)
hi_void hal_aiao_get_board_tx_i2s_df_attr(hi_u32 board_i2s_num, hi_i2s_attr *i2s_attr, aiao_port_id *port_id,
    aiao_port_user_cfg *attr)
{
    aiao_i2s_board_config *i2s_borad;

    i2s_borad = &g_i2s_board_settings[board_i2s_num];

    memcpy(attr, &g_aiao_tx_i2s_default_open_attr, sizeof(aiao_port_user_cfg));

    attr->if_attr.crg_mode = (i2s_attr->master == HI_TRUE) ? AIAO_CRG_MODE_MASTER : AIAO_CRG_MODE_SLAVE;
    attr->if_attr.ch_num = autil_ch_num_unf_to_aiao(i2s_attr->channel);
    attr->if_attr.bit_depth = autil_bit_depth_unf_to_aiao((hi_u32)i2s_attr->bit_depth);
    attr->if_attr.fclk_div = autil_bclk_fclk_div(i2s_attr->mclk, i2s_attr->bclk);
    attr->if_attr.bclk_div = (hi_u32)i2s_attr->bclk;
    attr->if_attr.crg_source = i2s_borad->tx_crg_source;
    attr->if_attr.source = i2s_borad->tx_source;
    attr->if_attr.pcm_delay_cycles = 0;
    attr->if_attr.i2s_mode = autil_i2s_mode_unf_to_aiao(i2s_attr->i2s_mode);

    if (i2s_attr->i2s_mode == HI_I2S_PCM_MODE) {
        attr->if_attr.rise_edge =
            (HI_TRUE == i2s_attr->pcm_sample_rise_edge) ? AIAO_MODE_EDGE_RISE : AIAO_MODE_EDGE_FALL;
        attr->if_attr.pcm_delay_cycles = (hi_u32)i2s_attr->pcm_delay_cycle;
    }

    if (attr->if_attr.bit_depth == AIAO_BIT_DEPTH_24) {
        attr->buf_config.period_buf_size *= 2;
    }

    *port_id = i2s_borad->tx_port_id;
}

hi_void hal_aiao_get_board_rx_i2s_df_attr(hi_u32 board_i2s_num, aiao_port_id *port_id, aiao_port_user_cfg *attr)
{
    aiao_i2s_board_config *i2s_borad;

    i2s_borad = &g_i2s_board_settings[board_i2s_num];

    memcpy(attr, &g_aiao_tx_i2s_default_open_attr, sizeof(aiao_port_user_cfg));

    attr->if_attr.crg_mode = i2s_borad->rx_crg_mode;
    attr->if_attr.crg_source = i2s_borad->rx_crg_source;
    attr->if_attr.source = i2s_borad->rx_source;
    attr->if_attr.pcm_delay_cycles = 0;

    *port_id = i2s_borad->rx_port_id;
}

hi_s32 hal_aiao_check_board_i2s_open_attr(aiao_port_id port_id, const aiao_if_attr *new_if_attr)
{
    aiao_i2s_board_config *i2s_borad = HI_NULL;
    aiao_if_attr *old_if_attr = HI_NULL;
    hi_s32 n;

    for (n = 0; n < AIAO_MAX_EXT_I2S_NUMBER; n++) {
        i2s_borad = &g_i2s_board_settings[n];

        if ((port_id == i2s_borad->tx_port_id) || (port_id == i2s_borad->rx_port_id)) {
            if (!i2s_borad->init_flag) {
                return HI_SUCCESS;
            }

            if (((port_id == i2s_borad->rx_port_id) && (!(i2s_borad->init_flag & 0x2))) ||
                ((port_id == i2s_borad->tx_port_id) && (!(i2s_borad->init_flag & 0x1)))) {
                return HI_SUCCESS;
            }

            old_if_attr = &i2s_borad->if_common_attr;
            break;
        }
    }

    if (n == AIAO_MAX_EXT_I2S_NUMBER) {
        return HI_SUCCESS;
    }

    if (old_if_attr == HI_NULL) {
        return HI_FAILURE;
    }

    if (old_if_attr->bit_depth != new_if_attr->bit_depth) {
        return HI_FAILURE;
    }

    if (old_if_attr->ch_num != new_if_attr->ch_num) {
        return HI_FAILURE;
    }

    if (old_if_attr->rate != new_if_attr->rate) {
        return HI_FAILURE;
    }

    if (old_if_attr->i2s_mode != new_if_attr->i2s_mode) {
        return HI_FAILURE;
    }

    if (old_if_attr->pcm_delay_cycles != new_if_attr->pcm_delay_cycles) {
        return HI_FAILURE;
    }

    if ((old_if_attr->crg_mode == AIAO_CRG_MODE_SLAVE) || (new_if_attr->crg_mode == AIAO_CRG_MODE_SLAVE)) {
        if (old_if_attr->crg_mode != new_if_attr->crg_mode) {
            return HI_FAILURE;
        }
    }

    if ((old_if_attr->fclk_div != new_if_attr->fclk_div) || (old_if_attr->bclk_div != new_if_attr->bclk_div)) {
        return HI_FAILURE;
    }

    if (i2s_borad->init_flag) {
        return HI_SUCCESS;
    }

    return HI_SUCCESS;
}

hi_void hal_aiao_set_board_i2s_open_attr(aiao_port_id port_id, const aiao_if_attr *new_if_attr)
{
    aiao_i2s_board_config *i2s_borad = HI_NULL;
    aiao_if_attr *old_if_attr = HI_NULL;
    hi_s32 n;

    for (n = 0; n < AIAO_MAX_EXT_I2S_NUMBER; n++) {
        i2s_borad = &g_i2s_board_settings[n];

        if ((port_id == i2s_borad->tx_port_id) || (port_id == i2s_borad->rx_port_id)) {
            old_if_attr = &i2s_borad->if_common_attr;
            break;
        }
    }

    if (n == AIAO_MAX_EXT_I2S_NUMBER) {
        return;
    }

    if ((new_if_attr == HI_NULL) || (old_if_attr == HI_NULL)) {
        return;
    }

    memcpy(old_if_attr, new_if_attr, sizeof(aiao_if_attr));

    if (port_id == i2s_borad->tx_port_id) {
        i2s_borad->init_flag |= 0x2; /* set tx */
    } else if (port_id == i2s_borad->rx_port_id) {
        i2s_borad->init_flag |= 0x1; /* set rx */
    }

    return;
}

hi_void hal_aiao_destroy_board_i2s_open_attr(aiao_port_id port_id)
{
    aiao_i2s_board_config *i2s_borad;
    hi_s32 n;

    for (n = 0; n < AIAO_MAX_EXT_I2S_NUMBER; n++) {
        i2s_borad = &g_i2s_board_settings[n];

        if ((port_id == i2s_borad->tx_port_id) || (port_id == i2s_borad->rx_port_id)) {
            break;
        }
    }

    if (n == AIAO_MAX_EXT_I2S_NUMBER) {
        return;
    }

    if (port_id == i2s_borad->tx_port_id) {
        i2s_borad->init_flag &= 0x1; /* clr tx */
    } else if (port_id == i2s_borad->rx_port_id) {
        i2s_borad->init_flag &= 0x2; /* clr rx */
    }

    return;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

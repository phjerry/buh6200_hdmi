/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao hardware driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "audio_util.h"

#include "hal_aiao_priv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static hi_void aiao_mmz_name(aiao_port_id port_id, hi_char *psz_mmz_name, hi_u32 name_size)
{
    hi_u32 chn_id = PORT2CHID(port_id);

    switch (PORT2MODE(port_id)) {
        case AIAO_MODE_RXI2S:
            snprintf(psz_mmz_name, name_size, "%s%02d", "aiao_rx", chn_id);
            break;
        case AIAO_MODE_TXI2S:
            snprintf(psz_mmz_name, name_size, "%s%02d", "aiao_tx", chn_id);
            break;
        case AIAO_MODE_TXSPDIF:
            snprintf(psz_mmz_name, name_size, "%s%02d", "aiao_sp", chn_id);
            break;
        default:
            break;
    }
}

static hi_u32 aiao_calc_buffer_size(hi_u32 *p_period_buf_size, hi_u32 *p_period_number)
{
    hi_u32 period_buf_size = *p_period_buf_size;
    hi_u32 period_number = *p_period_number;

    if (period_number > AIAO_BUFFER_PERIODNUM_MAX) {
        period_number = AIAO_BUFFER_PERIODNUM_MAX;
    }

    if (period_number < AIAO_BUFFER_PERIODNUM_MIN) {
        period_number = AIAO_BUFFER_PERIODNUM_MIN;
    }

    if (period_buf_size % AIAO_BUFFER_SIZE_ALIGN) {
        period_buf_size -= (period_buf_size % AIAO_BUFFER_SIZE_ALIGN);
    }

    if (period_buf_size < AIAO_BUFFER_PERIODSIZE_MIN) {
        period_buf_size = AIAO_BUFFER_PERIODSIZE_MIN;
    }

    if (period_buf_size > AIAO_BUFFER_PERIODSIZE_MAX) {
        period_buf_size = AIAO_BUFFER_PERIODSIZE_MAX;
    }

    *p_period_buf_size = period_buf_size;
    *p_period_number = period_number;

    return period_number * period_buf_size;
}

static hi_void port_buf_flush(aiao_port *port)
{
    port->buf.buff_rptr = 0;
    port->buf.buff_wptr = 0;
    aiao_hw_set_buf(port->port_id, &port->buf);
}

static hi_s32 port_buf_init(aiao_port *port)
{
    aiao_port_user_cfg *config = &port->user_congfig;
    aiao_buf_attr *buf_config = &config->buf_config;
    hi_u32 *wptr_addr = HI_NULL;
    hi_u32 *rptr_addr = HI_NULL;
    hi_u32 buf_size = aiao_calc_buffer_size(&buf_config->period_buf_size, &buf_config->period_number);
    hi_u32 start_phy_addr;
    hi_u8 *start_vir_addr = HI_NULL;

    if (config->ext_dma_mem == HI_TRUE) {
        if (!config->ext_mem.buf_phy_addr || !config->ext_mem.buf_vir_addr) {
            HI_FATAL_AIAO("phy_addr(0x%x) vir_addr(0x%x) invalid \n",
                config->ext_mem.buf_phy_addr, config->ext_mem.buf_vir_addr);
            return HI_FAILURE;
        }
        if (config->ext_mem.buf_phy_addr % AIAO_BUFFER_ADDR_ALIGN) {
            HI_FATAL_AIAO("phy_addr(0x%x) should align to (0x%x) invalid \n", config->ext_mem.buf_phy_addr,
                AIAO_BUFFER_ADDR_ALIGN);
            return HI_FAILURE;
        }
        if (buf_size > config->ext_mem.buf_size) {
            HI_FATAL_AIAO("ext_mem(0x%x) less than(0x%x) \n", config->ext_mem.buf_size, buf_size);
            return HI_FAILURE;
        }
        start_phy_addr = config->ext_mem.buf_phy_addr;
        start_vir_addr = (hi_u8 *)HI_NULL + config->ext_mem.buf_vir_addr;
    } else {
        /* step 1.0, malloc MMZ */
        hi_s32 ret;
        hi_char mmz_name[16] = {0};
        aiao_mmz_name(port->port_id, mmz_name, sizeof(mmz_name));
        ret = hi_drv_audio_mmz_alloc(mmz_name, buf_size, HI_FALSE, &port->mmz);
        if (ret != HI_SUCCESS) {
            HI_FATAL_AIAO("hi_drv_audio_mmz_alloc %s failed\n", mmz_name);
            return ret;
        }
        start_phy_addr = port->mmz.phys_addr;
        start_vir_addr = port->mmz.virt_addr;
    }

    aiao_hw_get_rptr_and_wptr_reg_addr(port->port_id, &wptr_addr, &rptr_addr);
    if ((wptr_addr == HI_NULL) || (rptr_addr == HI_NULL)) {
        HI_FATAL_AIAO("call aiao_hw_get_rptr_and_wptr_reg_addr failed\n");
        return HI_FAILURE;
    }

    /* step 2.0, CIRC buffer */
    circ_buf_init(&port->cb, wptr_addr, rptr_addr, start_vir_addr, buf_size);

    /* step 3.0, AIAO CIRC buffer reg */
    port->buf.buff_saddr = start_phy_addr;
    port->buf.buff_wptr = 0;
    port->buf.buff_rptr = 0;
    port->buf.buff_size = buf_size;
    port->buf.period_buf_size = buf_config->period_buf_size;
    port->buf.threshold_size = buf_config->period_buf_size;

    aiao_hw_set_buf(port->port_id, &port->buf);
    return HI_SUCCESS;
}

static hi_void port_buf_de_init(aiao_port *port)
{
    if (port->user_congfig.ext_dma_mem != HI_TRUE) {
        hi_drv_audio_mmz_release(&port->mmz);
    }
}

static hi_void port_set_i2s_attr(aiao_port *port)
{
    aiao_port_user_cfg *config = &port->user_congfig;

    aiao_hw_set_if_attr(port->port_id, &config->if_attr);
}

static hi_void port_set_user_default_congfig(aiao_port *port)
{
    aiao_port_id port_id = port->port_id;
    aiao_port_user_cfg *config = &port->user_congfig;

    aiao_hw_set_track_mode(port_id, config->track_mode);
    aiao_hw_set_fade_in_rate(port_id, config->fade_in_rate);
    aiao_hw_set_fade_out_rate(port_id, config->fade_out_rate);
    aiao_hw_set_mute_fade(port_id, config->mute_fade);
    aiao_hw_set_mute(port_id, config->mute);
    aiao_hw_set_volume_db(port_id, config->volume_db);
    aiao_hw_set_bypass(port_id, config->by_bass);
}

static hi_void port_reset_proc_status(aiao_port *port)
{
    memset(&port->proc_status, 0, sizeof(aiao_proc_stauts));
}

static hi_s32 port_start(aiao_port *port)
{
    hi_s32 ret;

    if (port->port_status == AIAO_PORT_STATUS_STOP) {
        ret = aiao_hw_set_start(port->port_id, AIAO_START);
        if (ret != HI_SUCCESS) {
            HI_ERR_AIAO("AIAO_START error\n");
            return ret;
        }

        port->port_status = AIAO_PORT_STATUS_START;
    } else if (port->port_status == AIAO_PORT_STATUS_STOP_PENDDING) {
        ret = aiao_hw_set_start(port->port_id, AIAO_STOP);
        if (ret != HI_SUCCESS) {
            HI_ERR_AIAO("AIAO_STOP error\n");
            return ret;
        }

        port_buf_flush(port);

        /* restore user mute setting */
        aiao_hw_set_mute(port->port_id, port->user_congfig.mute);
        ret = aiao_hw_set_start(port->port_id, AIAO_START);
        if (ret != HI_SUCCESS) {
            HI_ERR_AIAO("AIAO_START error\n");
            return ret;
        }

        port->port_status = AIAO_PORT_STATUS_START;
    }

    return HI_SUCCESS;
}

static hi_s32 port_stop(aiao_port *port, aiao_port_stopmode stop_mode)
{
    hi_s32 ret;

    if (stop_mode == AIAO_STOP_FADEOUT) {
        stop_mode = AIAO_STOP_IMMEDIATE;
    }

    switch (port->port_status) {
        case AIAO_PORT_STATUS_START: {
            /* stop immediately */
            ret = aiao_hw_set_start(port->port_id, AIAO_STOP);
            if (ret != HI_SUCCESS) {
                HI_ERR_AIAO("AIAO_STOP error\n");
                return ret;
            }

            port_buf_flush(port);
            port->port_status = AIAO_PORT_STATUS_STOP;
        }
        break;
        case AIAO_PORT_STATUS_STOP_PENDDING:
            if (stop_mode != AIAO_STOP_FADEOUT) {
                /* stop immediately */
                ret = aiao_hw_set_start(port->port_id, AIAO_STOP);
                if (ret != HI_SUCCESS) {
                    HI_ERR_AIAO("AIAO_STOP error\n");
                    return ret;
                }

                port_buf_flush(port);

                /* restore user mute setting */
                aiao_hw_set_mute(port->port_id, port->user_congfig.mute);
                port->port_status = AIAO_PORT_STATUS_STOP;
            }
            break;
        case AIAO_PORT_STATUS_STOP:
            break;
    }

    if (port->port_status == AIAO_PORT_STATUS_STOP) {
        port_reset_proc_status(port);
    }

    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_select_spdif_source(aiao_port *port, aiao_spdifport_source src_chn_id)
{
    AIAO_ASSERT_NULL(port);

    aiao_hw_set_spdif_port_select(port->port_id, src_chn_id);
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_spdif_out_port(aiao_port *port, hi_s32 en)
{
    AIAO_ASSERT_NULL(port);

    aiao_hw_set_spdif_port_en(port->port_id, en);
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_i2s_sd_select(aiao_port *port, aiao_i2s_data_sel *sd_sel)
{
    AIAO_ASSERT_NULL(port);

    aiao_hw_set_i2s_data_select(port->port_id, AIAO_I2S_SD0, sd_sel->sd0);
    aiao_hw_set_i2s_data_select(port->port_id, AIAO_I2S_SD1, sd_sel->sd1);
    aiao_hw_set_i2s_data_select(port->port_id, AIAO_I2S_SD2, sd_sel->sd2);
    aiao_hw_set_i2s_data_select(port->port_id, AIAO_I2S_SD3, sd_sel->sd3);
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_start(aiao_port *port)
{
    AIAO_ASSERT_NULL(port);
    return port_start(port);
}

hi_s32 i_hal_aiao_stop(aiao_port *port, aiao_port_stopmode stop_mode)
{
    AIAO_ASSERT_NULL(port);
    return port_stop(port, stop_mode);
}

hi_s32 i_hal_aiao_mute(aiao_port *port, hi_bool mute)
{
    aiao_port_user_cfg *config = HI_NULL;

    AIAO_ASSERT_NULL(port);
    config = &port->user_congfig;

    aiao_hw_set_mute(port->port_id, mute);
    config->mute = mute;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_volume(aiao_port *port, hi_u32 volume_db)
{
    aiao_port_user_cfg *config = HI_NULL;

    AIAO_ASSERT_NULL(port);
    config = &port->user_congfig;

    aiao_hw_set_volume_db(port->port_id, volume_db);
    config->volume_db = volume_db;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_spdif_category_code(aiao_port *port, aiao_spdif_categorycode category_code)
{
    aiao_port_user_cfg *config = HI_NULL;
    AIAO_ASSERT_NULL(port);
    config = &port->user_congfig;
    aiao_hw_set_spdif_category_scms(port->port_id, config->spdifscms_mode, category_code);
    config->spdif_category_code = category_code;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_spdif_scms_mode(aiao_port *port, aiao_spdif_scms_mode scms_mode)
{
    aiao_port_user_cfg *config = HI_NULL;

    AIAO_ASSERT_NULL(port);
    config = &port->user_congfig;

    aiao_hw_set_spdif_category_scms(port->port_id, scms_mode, config->spdif_category_code);
    config->spdifscms_mode = scms_mode;

    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_track_mode(aiao_port *port, aiao_track_mode track_mode)
{
    aiao_port_user_cfg *config = HI_NULL;

    AIAO_ASSERT_NULL(port);
    config = &port->user_congfig;

    aiao_hw_set_track_mode(port->port_id, track_mode);
    config->track_mode = track_mode;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_bypass(aiao_port *port, hi_bool by_bass)
{
    aiao_port_user_cfg *config = HI_NULL;

    AIAO_ASSERT_NULL(port);
    config = &port->user_congfig;

    aiao_hw_set_bypass(port->port_id, by_bass);
    config->by_bass = by_bass;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_mode(aiao_port *port, hi_bool mode)
{
    AIAO_ASSERT_NULL(port);

    aiao_hw_set_mode(port->port_id, mode);

    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_set_i2_slave_clk(aiao_port_id port_id, aiao_if_attr *if_attr)
{
    aiao_hw_set_i2s_slave_clk(port_id, if_attr);
    return HI_SUCCESS;
}

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
hi_s32 i_hal_aiao_set_op_type(aiao_port_id port_id, aiao_op_type op_type)
{
    aiao_hw_set_op_type(port_id, op_type);

    return HI_SUCCESS;
}
#endif

hi_s32 i_hal_aiao_set_i2s_master_clk(aiao_port_id port_id, aiao_if_attr *if_attr)
{
    aiao_hw_set_i2s_master_clk(port_id, if_attr);
    return HI_SUCCESS;
}

hi_void i_hal_aiao_set_spdif_port_en(aiao_port_id port_id, hi_s32 en)
{
    aiao_hw_set_spdif_port_en(port_id, en);
}

hi_s32 i_hal_aiao_set_attr(aiao_port *port, aiao_port_attr *attr)
{
    AIAO_ASSERT_NULL(port);
    AIAO_ASSERT_NULL(attr);

    if (port->port_status != AIAO_PORT_STATUS_STOP) {
        HI_FATAL_AIAO("port[0x%x] status is not AIAO_PORT_STATUS_STOP\n", port->port_id);
        return HI_FAILURE;
    }

    /* free buffer */
    port_buf_de_init(port);

    memcpy(&port->user_congfig.if_attr, &attr->if_attr, sizeof(aiao_if_attr));
    memcpy(&port->user_congfig.buf_config, &attr->buf_config, sizeof(aiao_buf_attr));

    /* AIAO buf init */
    if (port_buf_init(port) == HI_FAILURE) {
        HI_FATAL_AIAO("port_buf_init failed\n");
        return HI_FAILURE;
    }

    /* AIAO CRG/I2S */
    port_set_i2s_attr(port);

    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_get_attr(aiao_port *port, aiao_port_attr *attr)
{
    AIAO_ASSERT_NULL(port);
    AIAO_ASSERT_NULL(attr);

    memcpy(&attr->if_attr, &port->user_congfig.if_attr, sizeof(aiao_if_attr));
    memcpy(&attr->buf_config, &port->user_congfig.buf_config, sizeof(aiao_buf_attr));
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_open(const aiao_port_id port_id, const aiao_port_user_cfg *config, aiao_port **p_port,
    aiao_isr_func **isr)
{
    aiao_port *port = HI_NULL;

    AIAO_ASSERT_NULL(config);
    AIAO_ASSERT_NULL(p_port);

    /* step 1 malloc aiao_port */
    port = osal_kmalloc(HI_ID_AIAO, sizeof(aiao_port), OSAL_GFP_KERNEL);
    if (port == HI_NULL) {
        HI_FATAL_AIAO("malloc aiao_port failed\n");
        return HI_FAILURE;
    }

    memset(port, 0, sizeof(aiao_port));
    memcpy(&port->user_congfig, config, sizeof(aiao_port_user_cfg));
    port->port_id = port_id;
    port->port_status = AIAO_PORT_STATUS_STOP;

    /* step 2 AIAO buf init */
    if (port_buf_init(port) == HI_FAILURE) {
        osal_kfree(HI_ID_AIAO, port);
        HI_FATAL_AIAO("port_buf_init failed\n");
        return HI_FAILURE;
    }
#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
#if !defined(HI_AIAO_VERIFICATION_SUPPORT)
    if (port_id != AIAO_PORT_SPDIF_TX0)
#endif
#endif
    {
        /* step 3 AIAO CRG/I2S */
        port_set_i2s_attr(port);

        /* step 3.1 AIAO interrupt setting */
        if (PORT2DIR(port->port_id) == AIAO_DIR_TX) {
            aiao_hw_set_int(port->port_id, (hi_u32)(1L << AIAO_TXINT0_BUF_TRANSFINISH));
        } else {
            aiao_hw_set_int(port->port_id, (hi_u32)(1L << AIAO_RXINT0_BUF_TRANSFINISH));
        }
#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
        aiao_hw_set_op_type(port->port_id, config->op_type);
#endif
        /* step 4 AIAO user default config */
        port_set_user_default_congfig(port);
    }

    /* step 5 AIAO init internal state */
    port_reset_proc_status(port);

    *p_port = (aiao_port *)port;
    *isr = config->isr_func;
    return HI_SUCCESS;
}

#ifdef HI_AIAO_TIMER_SUPPORT
hi_s32 i_hal_aiao_t_create(aiao_timer_id timer_id, const aiao_timer_create *param, aiao_timer *ppst_port,
    aiao_timer_isr_func **isr)
{
    aiao_timer port;
    AIAO_ASSERT_NULL(param);

    port = osal_kmalloc(HI_ID_AIAO, sizeof(aiao_timer_ctx), OSAL_GFP_KERNEL);
    if (port == HI_NULL) {
        HI_FATAL_AIAO("malloc aiao_timer_ctx failed\n");
        return HI_FAILURE;
    }

    memset(port, 0, sizeof(aiao_timer_ctx));
    memcpy(&port->timer_param, param, sizeof(aiao_timer_create));
    port->timer_id = timer_id;
    *ppst_port = port;
    *isr = param->timer_isr_func;

    return HI_SUCCESS;
}

hi_void i_hal_aiao_t_destroy(aiao_timer port)
{
    AIAO_ASSERT_NULL(port);

    osal_kfree(HI_ID_AIAO, port);
}

hi_s32 i_hal_aiao_t_set_timer_attr(aiao_timer port, const aiao_timer_attr *attr)
{
    AIAO_ASSERT_NULL(port);

    aiao_timer_set_attr(port->timer_id, &(attr->if_attr), attr->config);
    port->timer_attr.config = attr->config;
    memcpy(&(port->timer_attr.if_attr), &(attr->if_attr), sizeof(aiao_if_timer_attr));
    port->timer_status.rate = attr->if_attr.rate;
    port->timer_status.config = attr->config;

    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_t_set_timer_enable(aiao_timer port, hi_bool enable)
{
    AIAO_ASSERT_NULL(port);

    aiao_timer_set_enable(port->timer_id, enable);
    port->timer_status.enable = enable;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_t_get_status(aiao_timer port, aiao_timer_status *status)
{
    AIAO_ASSERT_NULL(port);

    memcpy(status, &port->timer_status, sizeof(aiao_timer_status));
    return HI_SUCCESS;
}

hi_void i_hal_aiao_t_timer_process(aiao_timer port)
{
    aiao_timer_status *timer_status;

    timer_status = &port->timer_status;
    timer_status->isr_cnt++;
}

hi_void i_hal_timer_clear_timer(aiao_timer_id timer_id)
{
    aiao_timer_clear_timer(timer_id);
}
#endif

hi_void i_hal_aiao_close(aiao_port *port)
{
    hi_s32 ret;

    AIAO_ASSERT_NULL(port);

    /* step 1 stop channel */
    ret = port_stop(port, AIAO_STOP_IMMEDIATE);
    if (ret != HI_SUCCESS) {
        HI_ERR_AIAO("port_stop error\n");
        return;
    }

    /* step 1.1 AIAO interrupt setting */
    if (PORT2DIR(port->port_id) == AIAO_DIR_TX) {
        aiao_hw_clr_int(port->port_id, (hi_u32)AIAO_TXINT_ALL);
    } else {
        aiao_hw_clr_int(port->port_id, (hi_u32)AIAO_RXINT_ALL);
    }

    /* step 2 free buffer */
    port_buf_de_init(port);

    /* step 3 free dev */
    osal_kfree(HI_ID_AIAO, port);
}

hi_s32 i_hal_aiao_get_user_congfig(aiao_port *port, aiao_port_user_cfg *user_config)
{
    AIAO_ASSERT_NULL(port);
    AIAO_ASSERT_NULL(user_config);
    memcpy(user_config, &port->user_congfig, sizeof(aiao_port_user_cfg));
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_get_status(aiao_port *port, aiao_port_stauts *status)
{
    AIAO_ASSERT_NULL(port);

    memcpy(&status->proc_status, &port->proc_status, sizeof(aiao_proc_stauts));
    memcpy(&status->user_config, &port->user_congfig, sizeof(aiao_port_user_cfg));
    memcpy(&status->buf, &port->buf, sizeof(aiao_buf_info));
    memcpy(&status->circ_buf, &port->cb, sizeof(circ_buf));
    status->status = port->port_status;
    return HI_SUCCESS;
}

hi_s32 i_hal_aiao_get_rbf_attr(aiao_port *port, aiao_rbuf_attr *rbf_attr)
{
    hi_u32 *buf_vir_wptr = HI_NULL;
    hi_u32 *buf_vir_rptr = HI_NULL;

    AIAO_ASSERT_NULL(port);
    if (port->user_congfig.ext_dma_mem == HI_TRUE) {
        rbf_attr->buf_phy_addr = port->user_congfig.ext_mem.buf_phy_addr;
        rbf_attr->buf_vir_addr = port->user_congfig.ext_mem.buf_vir_addr;
        rbf_attr->buf_size = port->buf.buff_size;
    } else {
        rbf_attr->buf_phy_addr = port->mmz.phys_addr;
        rbf_attr->buf_vir_addr = port->mmz.virt_addr - (hi_u8 *)HI_NULL;
        rbf_attr->buf_size = port->buf.buff_size;
    }

    aiao_hw_get_rptr_and_wptr_reg_addr(port->port_id, &buf_vir_wptr, &buf_vir_rptr);
    aiao_hw_get_rptr_and_wptr_reg_phy_addr(port->port_id, &rbf_attr->buf_phy_wptr, &rbf_attr->buf_phy_rptr);
    rbf_attr->buf_vir_wptr = (hi_u8 *)buf_vir_wptr - (hi_u8 *)HI_NULL;
    rbf_attr->buf_vir_rptr = (hi_u8 *)buf_vir_rptr - (hi_u8 *)HI_NULL;

    return HI_SUCCESS;
}

static hi_void aiao_hal_tx_isr_proc(aiao_proc_stauts *proc, hi_u32 int_status)
{
    if (int_status & AIAO_TXINT0_BUF_TRANSFINISH_MASK) {
        proc->dma_cnt++;
    }

    if (int_status & AIAO_TXINT1_BUF_EMPTY_MASK) {
        /* ignore the buf empty INT at start beginning */
        if (proc->bus_fifo_empty_cnt) {
            proc->buf_empty_cnt++;
        }
    }

    if (int_status & AIAO_TXINT2_BUF_AEMPTY_MASK) {
        /* ignore the buf allmost empty INT at start beginning */
        if (proc->bus_fifo_empty_cnt) {
            proc->buf_empty_warning_cnt++;
        }
    } else {
        if (int_status & AIAO_TXINT3_BUF_FIFOEMPTY_MASK) {
            proc->bus_time_out_cnt++;
        }
    }

    if (int_status & AIAO_TXINT3_BUF_FIFOEMPTY_MASK) {
        proc->bus_fifo_empty_cnt++;
    }

    if (int_status & AIAO_TXINT4_IF_FIFOEMPTY_MASK) {
        proc->inf_fifo_empty_cnt++;
    }

    if (int_status & AIAO_TXINT7_DATA_BROKEN_MASK) {
        proc->inf_empty_cnt_real++;
    }
}

static hi_void aiao_hal_rx_isr_proc(aiao_proc_stauts *proc, hi_u32 int_status)
{
    if (int_status & AIAO_RXINT0_BUF_TRANSFINISH_MASK) {
        proc->dma_cnt++;
    }

    if (int_status & AIAO_RXINT1_BUF_FULL_MASK) {
        proc->buf_full_cnt++;
    }

    if (int_status & AIAO_RXINT2_BUF_AFULL_MASK) {
        proc->buf_full_warning_cnt++;
    } else {
        if (int_status & AIAO_RXINT3_BUF_FIFOFULL_MASK) {
            proc->bus_time_out_cnt++;
        }
    }

    if (int_status & AIAO_RXINT3_BUF_FIFOFULL_MASK) {
        proc->bus_fifo_full_cnt++;
    }

    if (int_status & AIAO_RXINT4_IF_FIFOFULL_MASK) {
        proc->inf_fifo_full_cnt++;
    }
}

hi_void i_hal_aiao_proc_statistics(aiao_port *port, hi_u32 int_status)
{
    aiao_proc_stauts *proc = HI_NULL;

    AIAO_ASSERT_NULL(port);
    proc = &port->proc_status;
    if (PORT2DIR(port->port_id) == AIAO_DIR_TX) {
        aiao_hal_tx_isr_proc(proc, int_status);
    } else {
        aiao_hal_rx_isr_proc(proc, int_status);
    }
}

#if defined(HI_AUDIO_AI_SUPPORT)
hi_u32 i_hal_aiao_read_data_not_up_rptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size, hi_u32 *rptr, hi_u32 *wptr)
{
    circ_buf *cb = HI_NULL;
    hi_u32 bytes = 0;

    AIAO_ASSERT_NULL(port);
    HI_ASSERT_RET(dest != HI_NULL);

    cb = &port->cb;
    if (port->port_status == AIAO_PORT_STATUS_START) {
        if (PORT2DIR(port->port_id) == AIAO_DIR_RX) {
            bytes = circ_buf_read_with_ext_pos(cb, dest, dest_size, rptr, wptr);
        } else {
            HI_WARN_AIAO("AIAO tx buf can't been read.\n");
        }
    }

    return bytes;
}
#endif

hi_u32 i_hal_aiao_read_data(aiao_port *port, hi_u8 *dest, hi_u32 dest_size)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes = 0;

    AIAO_ASSERT_NULL(port);
    HI_ASSERT_RET(dest != HI_NULL);

    cb = &port->cb;
    proc = &port->proc_status;
    if (port->port_status == AIAO_PORT_STATUS_START) {
        if (PORT2DIR(port->port_id) == AIAO_DIR_RX) {
            proc->try_read_cnt++;
            bytes = circ_buf_read(cb, dest, dest_size);
            proc->total_byte_read += bytes;
        } else {
            HI_WARN_AIAO("AIAO tx buf can't been read.\n");
        }
    }

    return bytes;
}

hi_u32 i_hal_aiao_write_data(aiao_port *port, hi_u8 *src, hi_u32 src_len)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes;

    AIAO_ASSERT_NULL(port);
    cb = &port->cb;
    proc = &port->proc_status;

    proc->try_write_cnt++;
    bytes = circ_buf_write(cb, src, src_len, HI_TRUE); /* copy from user or src = NULL */
    proc->total_byte_write += bytes;

    return bytes;
}

#if defined(HI_AUDIO_AI_SUPPORT) || defined(HI_AIAO_VERIFICATION_SUPPORT)
hi_u32 i_hal_aiao_prepare_data(aiao_port *port, hi_u8 *src, hi_u32 src_len)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes = 0;
    hi_s32 ret;

    AIAO_ASSERT_NULL(port);
    HI_ASSERT(src != HI_NULL);

    cb = &port->cb;
    proc = &port->proc_status;

    if (PORT2DIR(port->port_id) == AIAO_DIR_TX) {
        if (port->port_status == AIAO_PORT_STATUS_STOP_PENDDING) {
            ret = aiao_hw_set_start(port->port_id, AIAO_STOP);
            if (ret != HI_SUCCESS) {
                HI_ERR_AIAO("AIAO_STOP error\n");
                return 0;
            }

            port_buf_flush(port);

            /* restore user mute setting */
            aiao_hw_set_mute(port->port_id, port->user_congfig.mute);
            port->port_status = AIAO_PORT_STATUS_STOP;
        }

        proc->try_write_cnt++;
        bytes = circ_buf_write(cb, src, src_len, HI_TRUE); /* copy from user or src = NULL */
        proc->total_byte_write += bytes;
    } else {
        HI_WARN_AIAO("AIAO rx buf can't been write.\n");
    }

    return bytes;
}

hi_u32 i_hal_aiao_query_buf_data_provide_rptr(aiao_port *port, hi_u32 *rptr)
{
    AIAO_ASSERT_NULL(port);
    return circ_buf_data_size(*port->cb.write, *rptr, port->cb.size);
}

hi_u32 i_hal_aiao_query_buf_data(aiao_port *port)
{
    AIAO_ASSERT_NULL(port);
    return circ_buf_query_busy(&port->cb);
}

hi_u32 i_hal_aiao_query_buf_free(aiao_port *port)
{
    AIAO_ASSERT_NULL(port);

    return circ_buf_query_free(&port->cb);
}

hi_u32 i_hal_aiao_update_rptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes = 0;

    AIAO_ASSERT_NULL(port);
    HI_ASSERT(dest != HI_NULL);

    cb = &port->cb;
    proc = &port->proc_status;
    if (port->port_status == AIAO_PORT_STATUS_START) {
        if (PORT2DIR(port->port_id) == AIAO_DIR_RX) {
            proc->try_read_cnt++;
            bytes = circ_buf_update_read_pos(cb, dest_size);
            proc->total_byte_read += bytes;
        } else {
            HI_WARN_AIAO("AIAO tx buf can't been read.\n");
        }
    }

    return bytes;
}

hi_u32 i_hal_aiao_update_wptr(aiao_port *port, hi_u8 *src, hi_u32 src_len)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes = 0;

    AIAO_ASSERT_NULL(port);
    HI_ASSERT(src != HI_NULL);
    cb = &port->cb;
    proc = &port->proc_status;

    if (port->port_status == AIAO_PORT_STATUS_START) {
        if (PORT2DIR(port->port_id) == AIAO_DIR_TX) {
            proc->try_write_cnt++;
            bytes = circ_buf_update_write_pos(cb, src_len);
            proc->total_byte_write += bytes;
        } else {
            HI_WARN_AIAO("AIAO rx buf can't been write.\n");
        }
    } else {
        HI_WARN_AIAO("AIAO tx should been start before write.\n");
    }

    return bytes;
}
#endif

hi_void i_hal_aiao_get_delay_ms(aiao_port *port, hi_u32 *delayms)
{
    hi_u32 bytes, frame_size;
    AIAO_ASSERT_NULL(port);

    bytes = circ_buf_query_busy(&port->cb);
    frame_size =
        autil_calc_frame_size((hi_u32)port->user_congfig.if_attr.ch_num, (hi_u32)port->user_congfig.if_attr.bit_depth);
    *delayms = autil_byte_size_to_latency_ms(bytes, frame_size, (hi_u32)port->user_congfig.if_attr.rate);
}

hi_s32 i_hal_aiao_init(hi_void)
{
    return aiao_hw_init();
}

hi_void i_hal_aiao_de_init(hi_void)
{
    aiao_hw_de_init();
}

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
hi_void i_hal_aiao_get_hw_capability(hi_u32 *capability)
{
    aiao_hw_get_hw_capability(capability);
}

hi_void i_hal_aiao_get_hw_version(hi_u32 *version)
{
    aiao_hw_get_hw_version(version);
}

hi_void i_hal_aiao_dbg_rw_reg(aiao_dbg_reg *reg)
{
    aiao_hw_dbg_rw_reg(reg);
}
#endif

hi_void i_hal_aiao_set_top_int(hi_u32 multibit)
{
    aiao_hw_set_top_int(multibit);
}

hi_u32 i_hal_aiao_get_top_int_raw_status(hi_void)
{
    return aiao_hw_get_top_int_raw_status();
}

hi_u32 i_hal_aiao_get_top_int_status(hi_void)
{
    return aiao_hw_get_top_int_status();
}

hi_void i_hal_aiao_set_int(aiao_port_id port_id, hi_u32 multibit)
{
    aiao_hw_set_int(port_id, multibit);
}

hi_void i_hal_aiao_clr_int(aiao_port_id port_id, hi_u32 multibit)
{
    aiao_hw_clr_int(port_id, multibit);
}

hi_u32 i_hal_aiao_get_int_status_raw(aiao_port_id port_id)
{
    return aiao_hw_get_int_status_raw(port_id);
}

hi_u32 i_hal_aiao_get_int_status(aiao_port_id port_id)
{
    return aiao_hw_get_int_status(port_id);
}

#ifdef HI_ALSA_AI_SUPPORT
hi_u32 i_hal_aiao_alsa_query_write_pos(aiao_port *port)
{
    hi_u32 bytes = 0;

    AIAO_ASSERT_NULL(port);
    if (port->port_status == AIAO_PORT_STATUS_START) {
        bytes = *(port->cb.write);
    }

    return bytes;
}

hi_u32 i_hal_aiao_alsa_query_read_pos(aiao_port *port)
{
    hi_u32 bytes = 0;
    AIAO_ASSERT_NULL(port);

    if (port->port_status == AIAO_PORT_STATUS_START) {
        bytes = *(port->cb.read);
    }

    return bytes;
}

hi_u32 i_hal_aiao_alsa_update_rptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes = 0;
    AIAO_ASSERT_NULL(port);
    cb = &port->cb;
    proc = &port->proc_status;
    if (port->port_status == AIAO_PORT_STATUS_START) {
        proc->try_read_cnt++;
        bytes = circ_buf_update_read_pos(cb, dest_size);
        proc->total_byte_read += bytes;
    }
    return bytes;
}

hi_u32 i_hal_aiao_alsa_update_wptr(aiao_port *port, hi_u8 *dest, hi_u32 dest_size)
{
    circ_buf *cb = HI_NULL;
    aiao_proc_stauts *proc = HI_NULL;
    hi_u32 bytes = 0;
    AIAO_ASSERT_NULL(port);
    cb = &port->cb;
    proc = &port->proc_status;
    if (port->port_status == AIAO_PORT_STATUS_START) {
        proc->try_read_cnt++;
        bytes = circ_buf_update_write_pos(cb, dest_size);
        proc->total_byte_read += bytes;
    }
    return bytes;
}

hi_u32 i_hal_aiao_alsa_flash(aiao_port *port)
{
    circ_buf *cb = HI_NULL;
    AIAO_ASSERT_NULL(port);

    cb = &port->cb;

    if (port->port_status == AIAO_PORT_STATUS_START) {
        circ_buf_flush(cb);
    }

    return HI_SUCCESS;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

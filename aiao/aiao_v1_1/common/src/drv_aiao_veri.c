/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao cbb verification code
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "drv_aiao_ioctl_veri.h"
#include "drv_ao_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* private state */
typedef struct {
    hi_u32 port[AIAO_INT_MAX];

    hi_u32 period_buf_size[AIAO_INT_MAX];
    hi_void *port_data_buf[AIAO_INT_MAX];
} aiao_state;

#define CHECK_AIAO_PORT_OPEN(id)                        \
    do {                                                  \
        if (g_aiao_state->port[(id)] == AIAO_PORT_MAX) { \
            HI_ERR_AIAO("invalid port id 0x%x\n", (id));  \
            return HI_FAILURE;                            \
        }                                                 \
    } while (0)

static aiao_state g_aiao_dev_state;
static aiao_state *g_aiao_state = &g_aiao_dev_state;

static hi_s32 aiao_veri_proc_read(hi_void *p, hi_void *v);

static hi_proc_fn_set aiao_cbb_opt = {
    .read = aiao_veri_proc_read,
};

#define PERCENT_FACTOR 100
#define AIAO_CBB_DEFAULT_PERIOND_TIME 20
#define MS_TO_S_FACTOR 1000

static hi_u32 get_df_periond_size(aiao_sample_rate rate, aiao_i2s_chnum ch, aiao_bit_depth bit_depth)
{
    hi_u32 frame_size = aiao_frame_size(ch, bit_depth);
    hi_u32 periond_size;

    periond_size = (rate * frame_size * AIAO_CBB_DEFAULT_PERIOND_TIME) / MS_TO_S_FACTOR;

    if (periond_size % frame_size) {
        periond_size -= periond_size % frame_size;
    }

    if (periond_size % AIAO_BUFFER_SIZE_ALIGN) {
        periond_size += AIAO_BUFFER_SIZE_ALIGN - periond_size % AIAO_BUFFER_SIZE_ALIGN;
    }

    return periond_size;
}

static hi_void aiao_free_port_data_buffer(aiao_port_id port_id)
{
    hi_u32 id = PORT2ID(port_id);
    if (g_aiao_state->port_data_buf[id] != HI_NULL) {
        osal_kfree(HI_ID_AIAO, g_aiao_state->port_data_buf[id]);
        g_aiao_state->port_data_buf[id] = HI_NULL;
    }
}

static hi_s32 aiao_malloc_port_data_buffer(aiao_port_id port_id, aiao_port_user_cfg *config)
{
    hi_u32 id = PORT2ID(port_id);

    g_aiao_state->period_buf_size[id] =
        get_df_periond_size(config->if_attr.rate,
                            config->if_attr.ch_num,
                            config->if_attr.bit_depth);
    if (g_aiao_state->period_buf_size[id] == 0) {
        HI_ERR_AIAO("port(0x%x) data buffer size is zero!", port_id);
        return HI_FAILURE;
    }

    g_aiao_state->port_data_buf[id] = osal_kmalloc(HI_ID_AIAO, g_aiao_state->period_buf_size[id], OSAL_GFP_KERNEL);
    if (g_aiao_state->port_data_buf[id] == HI_NULL) {
        HI_ERR_AIAO("malloc port data buffer failed!");
        g_aiao_state->period_buf_size[id] = 0;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void aiao_veri_open(hi_void)
{
    hi_s32 id;
    hi_proc_item *item = HI_NULL;

    /* init aiao state */
    for (id = 0; id < AIAO_INT_MAX; id++) {
        g_aiao_state->port[id] = AIAO_PORT_MAX;
        g_aiao_state->period_buf_size[id] = 0;
        g_aiao_state->port_data_buf[id] = HI_NULL;
    }

    item = hi_drv_proc_add_module(AIAO_VERI_PROC_NAME, &aiao_cbb_opt, HI_NULL);
    if (item == HI_NULL) {
        HI_ERR_AIAO("add proc aiao_port failed\n");
    }
}

hi_void aiao_veri_release(hi_void)
{
    hi_s32 id;

    hi_drv_proc_remove_module(AIAO_VERI_PROC_NAME);

    for (id = 0; id < AIAO_INT_MAX; id++) {
        if (g_aiao_state->port[id] != AIAO_INT_MAX) {
            hal_aiao_close_veri(ID2PORT(g_aiao_state->port[id]));
            g_aiao_state->port[id] = AIAO_PORT_MAX;
            g_aiao_state->period_buf_size[id] = 0;
            if (g_aiao_state->port_data_buf[id] != HI_NULL) {
                osal_kfree(HI_ID_AIAO, g_aiao_state->port_data_buf[id]);
                g_aiao_state->port_data_buf[id] = HI_NULL;
            }
        }
    }
}

static const char *aiao_port_status_to_name(aiao_port_status status)
{
    const char *status_tab[] = {
        [AIAO_PORT_STATUS_STOP] = "stop",
        [AIAO_PORT_STATUS_START] = "start",
        [AIAO_PORT_STATUS_STOP_PENDDING] = "stopping",
    };

    return status_tab[status];
}

hi_s32 aiao_veri_proc_read(hi_void *p, hi_void *v)
{
    hi_u32 id;
    hi_s32 ret;
    aiao_port_stauts status;

    for (id = 0; id < AIAO_INT_MAX; id++) {
        hi_u32 bytes;
        hi_u32 percent;
        aiao_port_id port_id = ID2PORT(g_aiao_state->port[id]);
        if (port_id == AIAO_PORT_MAX) {
            continue;
        }

        bytes = hal_aiao_query_buf_data(port_id);
        ret = hal_aiao_get_status(port_id, &status);
        if (ret != HI_SUCCESS) {
            return ret;
        }

#ifdef HI_SND_HDMI_I2S_SPDIF_MUX
        if (port_id == AIAO_PORT_SPDIF_TX0) {
            /* see function snd_op_get_port with args HI_AO_PORT_HDMI0 SND_AOP_TYPE_I2S */
            aiao_port_stauts port_proc_status;
            aiao_port_id tmp_port_id = AIAO_PORT_TX2;

            ret = hal_aiao_get_status(tmp_port_id, &port_proc_status);
            if (ret != HI_SUCCESS) {
                HI_ERR_AIAO("hal_aiao_get_status (port:%d) failed\n", tmp_port_id);
                return ret;
            }

            ret = memcpy_s(&status.proc_status, sizeof(aiao_proc_stauts),
                &port_proc_status.proc_status, sizeof(aiao_proc_stauts));
            if (ret != EOK) {
                HI_ERR_AIAO("memcpy_s (port:%d) failed\n", tmp_port_id);
                return ret;
            }
        }
#endif
        percent = (bytes * PERCENT_FACTOR) / status.buf.buff_size;

        if (PORT2DIR(port_id) == AIAO_DIR_TX) {
            osal_proc_print(p, "%s_%s(%.2d), ",
                (hi_char *)(AIAO_MODE_TXI2S == PORT2MODE(port_id) ? "i2s" : "spd"),
                "tx", PORT2CHID(port_id));
        } else {
            osal_proc_print(p, "%s_%s(%.2d), ",
                "i2s", "rx", PORT2CHID(port_id));
        }

        osal_proc_print(p, "%s(%s), %s(%.6d), %s(%.2d), %s(%.2d), ",
            "status", aiao_port_status_to_name(status.status),
            "fs", status.user_config.if_attr.rate,
            "ch", status.user_config.if_attr.ch_num,
            "bit", status.user_config.if_attr.bit_depth);

        osal_proc_print(p, "%s(%.6u), %s(%.6u), ",
            "dma_cnt", status.proc_status.dma_cnt,
            "dma_try", status.proc_status.try_write_cnt);

        if (PORT2DIR(port_id) == AIAO_DIR_TX) {
            osal_proc_print(p, "%s(0x%.6x), ",
                "total_byte", status.proc_status.total_byte_write);
        } else {
            osal_proc_print(p, "%s(0x%.6x), ",
                "total_byte", status.proc_status.total_byte_read);
        }

        osal_proc_print(p, "%s(%.2u%%), %s(0x%.6x), %s(%.2d)\n",
            "buf_used", percent,
            "period_size", status.user_config.buf_config.period_buf_size,
            "period_num", status.user_config.buf_config.period_number);

        osal_proc_print(p, "%-4s", " ");

        if (PORT2DIR(port_id) == AIAO_DIR_TX) {
            osal_proc_print(p, "%s(%.6u), %s(%.6u), %s(%.6u), %s(%.6u), ",
                "buf_empty_cnt", status.proc_status.buf_empty_cnt,
                "buf_empty_warning_cnt", status.proc_status.buf_empty_warning_cnt,
                "bus_fifo_empty_cnt", status.proc_status.bus_fifo_empty_cnt,
                "inf_fifo_empty_cnt", status.proc_status.inf_fifo_empty_cnt);
        } else {
            osal_proc_print(p, "%s(%.6u), %s(%.6u), %s(%.6u), %s(%.6u), ",
                "buf_full_cnt", status.proc_status.buf_full_cnt,
                "buf_full_warning_cnt", status.proc_status.buf_full_warning_cnt,
                "bus_fifo_full_cnt", status.proc_status.bus_fifo_full_cnt,
                "inf_fifo_full_cnt", status.proc_status.inf_fifo_full_cnt);
        }

        osal_proc_print(p, "%s(%.6u)\n\n",
            "bus_time_out_cnt", status.proc_status.bus_time_out_cnt);
    }

    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_get_hw_capability(hi_void *file, hi_void *arg)
{
    aiao_port_get_capability *param = (aiao_port_get_capability *)arg;
    hal_aiao_get_hw_capability(&param->capability);
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_set_sys_crg(hi_void *file, hi_void *arg)
{
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_dbg_rw_reg(hi_void *file, hi_void *arg)
{
    aiao_dbg_reg *dbg_reg = (aiao_dbg_reg *)arg;
    hal_aiao_dbg_rw_reg(dbg_reg);
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_open_veri(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    aiao_port_open *param = (aiao_port_open *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    if (g_aiao_state->port[id] != AIAO_PORT_MAX) {
        HI_ERR_AIAO("port_id(0x%x), had already open.\n", param->port_id);
        return HI_FAILURE;
    }

    param->config.isr_func = hal_aiao_proc_statistics;
    ret = hal_aiao_open_veri(param->port_id, &param->config);
    if (ret == HI_SUCCESS) {
        g_aiao_state->port[id] = id;
        aiao_malloc_port_data_buffer(param->port_id, &param->config);
        HI_INFO_AIAO("port_id(0x%x), open success.\n", param->port_id);
    } else {
        HI_ERR_AIAO("port_id(0x%x), open fail(0x%x).\n", param->port_id, ret);
    }

    return ret;
}

static hi_s32 aiao_ioctl_close_veri(hi_void *file, hi_void *arg)
{
    aiao_port_close *param = (aiao_port_close *)arg;

    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    hal_aiao_close_veri(param->port_id);
    aiao_free_port_data_buffer(param->port_id);
    g_aiao_state->port[id] = AIAO_PORT_MAX;
    HI_INFO_AIAO("port_id(0x%x), close.\n", param->port_id);

    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_start(hi_void *file, hi_void *arg)
{
    aiao_port_start *param = (aiao_port_start *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_start(param->port_id);
}

static hi_s32 aiao_ioctl_stop(hi_void *file, hi_void *arg)
{
    aiao_port_stop *param = (aiao_port_stop *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_stop(param->port_id, param->stop_mode);
}

static hi_s32 aiao_ioctl_mute(hi_void *file, hi_void *arg)
{
    aiao_port_mute *param = (aiao_port_mute *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_mute(param->port_id, param->mute);
}

static hi_s32 aiao_ioctl_set_track_mode(hi_void *file, hi_void *arg)
{
    aiao_port_track_mode *param = (aiao_port_track_mode *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_set_track_mode(param->port_id, param->track_mode);
}

static hi_s32 aiao_ioctl_set_volume(hi_void *file, hi_void *arg)
{
    aiao_port_volume *param = (aiao_port_volume *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);

    return hal_aiao_set_volume(param->port_id, param->volume_db);
}

static hi_s32 aiao_ioctl_get_user_congfig(hi_void *file, hi_void *arg)
{
    aiao_port_get_user_config *param = (aiao_port_get_user_config *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);

    return hal_aiao_get_user_congfig(param->port_id, &param->user_config);
}

static hi_s32 aiao_ioctl_get_status(hi_void *file, hi_void *arg)
{
    aiao_port_get_status *param = (aiao_port_get_status *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_get_status(param->port_id, &param->status);
}

static hi_s32 aiao_ioctl_select_spdif_source(hi_void *file, hi_void *arg)
{
    aiao_port_select_spdif_source *param = (aiao_port_select_spdif_source *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_select_spdif_source(param->port_id, param->src_chn_id);
}

static hi_s32 aiao_ioctl_set_spdif_out_port(hi_void *file, hi_void *arg)
{
    aiao_port_set_spdif_out_port *param = (aiao_port_set_spdif_out_port *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_set_spdif_out_port(param->port_id, param->en);
}

static hi_s32 aiao_ioctl_set_i2s_sd_select(hi_void *file, hi_void *arg)
{
    aiao_port_i2s_data_sel *param = (aiao_port_i2s_data_sel *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    return hal_aiao_set_i2s_sd_select(param->port_id, &param->sd_sel);
}

static hi_s32 aiao_ioctl_read_data(hi_void *file, hi_void *arg)
{
    aiao_port_read_data *param = (aiao_port_read_data *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);

    if ((g_aiao_state->period_buf_size[id] == 0) ||
        (param->dest_size > g_aiao_state->period_buf_size[id]) ||
        (g_aiao_state->port_data_buf[id] == HI_NULL)) {
        HI_ERR_AIAO("param->dest_size(%d) > g_aiao_state->period_buf_size[id](%d)",
            param->dest_size, g_aiao_state->period_buf_size[id]);
        return HI_FAILURE;
    }

    param->read_bytes = hal_aiao_read_data(param->port_id,
        g_aiao_state->port_data_buf[id], param->dest_size);

    if (copy_to_user(param->dest, g_aiao_state->port_data_buf[id], param->read_bytes)) {
        HI_ERR_AIAO("copy_to_user failed.\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_write_data(hi_void *file, hi_void *arg)
{
    aiao_port_write_data *param = (aiao_port_write_data *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);

    if ((g_aiao_state->period_buf_size[id] == 0) ||
        (param->src_len > g_aiao_state->period_buf_size[id]) ||
        (g_aiao_state->port_data_buf[id] == HI_NULL)) {
        HI_ERR_AIAO("param->src_len(%d) > g_aiao_state->period_buf_size[id](%d)",
            param->src_len, g_aiao_state->period_buf_size[id]);
        return HI_FAILURE;
    }

    if (osal_copy_from_user(g_aiao_state->port_data_buf[id], param->src, param->src_len)) {
        HI_ERR_AIAO("osal_copy_from_user failed.\n");
        return HI_FAILURE;
    }

    param->write_bytes = hal_aiao_write_data(param->port_id,
        g_aiao_state->port_data_buf[id], param->src_len);
    if (param->write_bytes != param->src_len) {
        HI_ERR_AIAO("hal_aiao_write_data failed(0x%x, 0x%x)\n", param->src_len, param->write_bytes);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_query_buf(hi_void *file, hi_void *arg)
{
    aiao_port_buf_status *param = (aiao_port_buf_status *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);

    param->data_size = hal_aiao_query_buf_data(param->port_id);
    param->free_size = hal_aiao_query_buf_free(param->port_id);
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_update_rptr(hi_void *file, hi_void *arg)
{
    aiao_port_read_data *param = (aiao_port_read_data *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);
    param->read_bytes = hal_aiao_update_rptr(param->port_id, param->dest, param->dest_size);
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_update_wptr(hi_void *file, hi_void *arg)
{
    aiao_port_write_data *param = (aiao_port_write_data *)arg;
    hi_u32 id = PORT2ID(param->port_id);
    CHECK_AIAO_PORT_OPEN(id);

    param->write_bytes = hal_aiao_update_wptr(param->port_id, param->src, param->src_len);
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_hdmi_set_audio_attr(hi_void *file, hi_void *arg)
{
    aiao_hdmi_audio_atrr *param = (aiao_hdmi_audio_atrr *)arg;

    hdmi_ao_attr audio_attr = {
        .audio_format = param->audio_format,
        .interface = param->sound_intf,
        .channels = param->channels,
        .org_channels = param->channels,
        .sample_rate = param->sample_rate,
        .org_sample_rate = param->sample_rate,
        .bit_depth = param->bit_depth,
    };

    ao_snd_hdmi_set_ao_attr(HI_NULL, 0, &audio_attr);
    return HI_SUCCESS;
}

#ifdef HI_AIAO_TIMER_SUPPORT
static hi_s32 aiao_ioctl_timer_create(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    aiao_timer_create *param = (aiao_timer_create *)arg;
    hi_u32 id = TIMER2ID(param->timer_id);
    if (g_aiao_state->port[id] != AIAO_PORT_MAX) {
        HI_ERR_AIAO("port_id(0x%x), had open\n", param->timer_id);
        return HI_FAILURE;
    }

    param->timer_isr_func = hal_aiao_t_timer_process;
    ret = hal_aiao_t_create(param->timer_id, param);
    if (ret == HI_SUCCESS) {
        g_aiao_state->port[id] = id;
    } else {
        HI_ERR_AIAO("timer_id(0x%x), create timer fail(0x%x).\n", param->timer_id, ret);
    }

    return ret;
}

static hi_s32 aiao_ioctl_timer_destory(hi_void *file, hi_void *arg)
{
    aiao_timer_destroy *param = (aiao_timer_destroy *)arg;
    hi_u32 id = TIMER2ID(param->timer_id);
    CHECK_AIAO_PORT_OPEN(id);
    hal_aiao_t_destroy(param->timer_id);
    g_aiao_state->port[id] = AIAO_PORT_MAX;
    return HI_SUCCESS;
}

static hi_s32 aiao_ioctl_timer_set_attr(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    aiao_timer_attr *param = (aiao_timer_attr *)arg;
    hi_u32 id = TIMER2ID(param->timer_id);
    CHECK_AIAO_PORT_OPEN(id);

    ret = hal_aiao_t_set_timer_attr(param->timer_id, param);
    if (ret != HI_SUCCESS) {
        HI_ERR_AIAO("port_id(0x%x), hal_aiao_t_set_timer_attr fail(0x%x).\n", param->timer_id, ret);
    }

    return ret;
}

static hi_s32 aiao_ioctl_timer_set_enable(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    aiao_timer_enable *param = (aiao_timer_enable *)arg;
    hi_u32 id = TIMER2ID(param->timer_id);
    CHECK_AIAO_PORT_OPEN(id);

    ret = hal_aiao_t_set_timer_enable(param->timer_id, param->enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_AIAO("port_id(0x%x), hal_aiao_t_set_timer_enable fail(0x%x).\n", param->timer_id, ret);
    }

    return ret;
}

static hi_s32 aiao_ioctl_timer_get_status(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    aiao_timer_status *param = (aiao_timer_status *)arg;
    hi_u32 id = TIMER2ID(param->timer_id);
    CHECK_AIAO_PORT_OPEN(id);

    ret = hal_aiao_t_get_status(param->timer_id, param);
    if (ret != HI_SUCCESS) {
        HI_ERR_AIAO("port_id(0x%x), hal_aiao_t_get_status fail(0x%x).\n", param->timer_id, ret);
    }

    return ret;
}
#endif

static const struct {
    hi_u32 cmd;
    hi_s32 (*ioctl)(hi_void *file, hi_void *arg);
} g_fn_aiao_ioctl[CMD_ID_AIAO_MAX] = {
    [CMD_ID_AIAO_GET_CAPABILITY] = { CMD_AIAO_GET_CAPABILITY,      aiao_ioctl_get_hw_capability },
    [CMD_ID_AIAO_SET_SYSCRG] = { CMD_AIAO_SET_SYSCRG,          aiao_ioctl_set_sys_crg },
    [CMD_ID_AIAO_RW_REGISTER] = { CMD_AIAO_RW_REGISTER,         aiao_ioctl_dbg_rw_reg },
    [CMD_ID_AIAO_PORT_OPEN] = { CMD_AIAO_PORT_OPEN,           aiao_ioctl_open_veri },
    [CMD_ID_AIAO_PORT_CLOSE] = { CMD_AIAO_PORT_CLOSE,          aiao_ioctl_close_veri },
    [CMD_ID_AIAO_PORT_START] = { CMD_AIAO_PORT_START,          aiao_ioctl_start },
    [CMD_ID_AIAO_PORT_STOP] = { CMD_AIAO_PORT_STOP,           aiao_ioctl_stop },
    [CMD_ID_AIAO_PORT_MUTE] = { CMD_AIAO_PORT_MUTE,           aiao_ioctl_mute },
    [CMD_ID_AIAO_PORT_TRACKMODE] = { CMD_AIAO_PORT_TRACKMODE,      aiao_ioctl_set_track_mode },
    [CMD_ID_AIAO_PORT_VOLUME] = { CMD_AIAO_PORT_VOLUME,         aiao_ioctl_set_volume },
    [CMD_ID_AIAO_PORT_GET_USERCONFIG] = { CMD_AIAO_PORT_GET_USERCONFIG, aiao_ioctl_get_user_congfig },
    [CMD_ID_AIAO_PORT_GET_STATUS] = { CMD_AIAO_PORT_GET_STATUS,     aiao_ioctl_get_status },

    [CMD_ID_AIAO_PORT_SEL_SPDIFOUTSOURCE] = { CMD_AIAO_PORT_SEL_SPDIFOUTSOURCE, aiao_ioctl_select_spdif_source },
    [CMD_ID_AIAO_PORT_SET_SPDIFOUTPORT] = { CMD_AIAO_PORT_SET_SPDIFOUTPORT,   aiao_ioctl_set_spdif_out_port },
    [CMD_ID_AIAO_PORT_SET_I2SDATASEL] = { CMD_AIAO_PORT_SET_I2SDATASEL,     aiao_ioctl_set_i2s_sd_select },

    [CMD_ID_AIAO_PORT_READ_DATA] = { CMD_AIAO_PORT_READ_DATA,    aiao_ioctl_read_data },
    [CMD_ID_AIAO_PORT_WRITE_DATA] = { CMD_AIAO_PORT_WRITE_DATA,   aiao_ioctl_write_data },
    [CMD_ID_AIAO_PORT_PREPARE_DATA] = { CMD_AIAO_PORT_PREPARE_DATA, aiao_ioctl_write_data },
    [CMD_ID_AIAO_PORT_QUERY_BUF] = { CMD_AIAO_PORT_QUERY_BUF,    aiao_ioctl_query_buf },
    [CMD_ID_AIAO_PORT_UPDATE_RPTR] = { CMD_AIAO_PORT_UPDATE_RPTR,  aiao_ioctl_update_rptr },
    [CMD_ID_AIAO_PORT_UPDATE_WPTR] = { CMD_AIAO_PORT_UPDATE_WPTR,  aiao_ioctl_update_wptr },

    [CMD_ID_AIAO_HDMI_SETAUDIO_ATTR] = { CMD_AIAO_HDMI_SETAUDIO_ATTR, aiao_ioctl_hdmi_set_audio_attr },

#ifdef HI_AIAO_TIMER_SUPPORT
    [CMD_ID_AIAO_TIMER_CREATE] = { CMD_AIAO_TIMER_CREATE,    aiao_ioctl_timer_create },
    [CMD_ID_AIAO_TIMER_DESTROY] = { CMD_AIAO_TIMER_DESTROY,   aiao_ioctl_timer_destory },
    [CMD_ID_AIAO_TIMER_SETATTR] = { CMD_AIAO_TIMER_SETATTR,   aiao_ioctl_timer_set_attr },
    [CMD_ID_AIAO_TIMER_SETENABLE] = { CMD_AIAO_TIMER_SETENABLE, aiao_ioctl_timer_set_enable },
    [CMD_ID_AIAO_TIMER_GETSTATUS] = { CMD_AIAO_TIMER_GETSTATUS, aiao_ioctl_timer_get_status },
#endif
};

hi_s32 aiao_veri_process_cmd(hi_void *file, hi_u32 cmd, hi_void *arg)
{
    hi_u32 cmd_type = _IOC_TYPE(cmd);
    hi_u32 cmd_id = _IOC_NR(cmd);

    if (cmd_type == HI_ID_AIAO) {
        if (cmd_id >= CMD_ID_AIAO_MAX) {
            HI_LOG_WARN("unknown cmd\n");
            HI_WARN_PRINT_H32(cmd);
            return HI_ERR_AO_INVALID_PARA;
        }

        if (cmd != g_fn_aiao_ioctl[cmd_id].cmd) {
            HI_LOG_WARN("unknown cmd\n");
            HI_WARN_PRINT_H32(cmd);
            return HI_ERR_AO_INVALID_PARA;
        }

        if (g_fn_aiao_ioctl[cmd_id].ioctl == HI_NULL) {
            return HI_ERR_AO_NOTSUPPORT;
        }

        return (g_fn_aiao_ioctl[cmd_id].ioctl)(file, arg);
    }

    HI_WARN_AIAO("unknown command type.\n");
    return HI_ERR_AO_INVALID_PARA;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

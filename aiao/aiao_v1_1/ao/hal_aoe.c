/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aoe hal
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_aiao_log.h"

#include "hi_drv_audio.h"

#include "hal_aoe.h"
#include "hal_aoe_func.h"

#include "circ_buf.h"
#include "audio_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

typedef struct {
    struct osal_list_head aip_list;
    struct osal_list_head aop_list;
    struct osal_list_head engine_list;
} aoe_global_source;

typedef struct {
    aoe_aip_chn_attr user_attr;

    aoe_aip_id aip;
    aip_regs_type *aip_reg;
    struct osal_list_head node;

    hi_u32 buf_frame_size;
    hi_u32 fifo_frame_size;
    hi_s32 adj_speed;
    hi_u32 l_volume_db;
    hi_u32 r_volume_db;
    hi_bool mute;
    hi_u32 channel_mode;
    aoe_aip_status status;
    circ_buf cb;
    hi_bool fifo_bypass;
    hi_bool data_from_kernel;
} aoe_aip_chn_state;

/* private state */
static aoe_global_source g_aoe_rm;

/* hal aoe aip function */
#define CHECK_AIP_OPEN(aip_id) \
    do { \
        if (_aip_find_by_id((aip_id)) == HI_NULL) { \
            HI_LOG_ERR("aip is not create.\n"); \
            HI_ERR_PRINT_H32((aip_id)); \
            return HI_FAILURE; \
        } \
    } while (0)

#define AIP_FIND_BY_ID(aip_id, aip_state)  \
    do { \
        if (((aip_state) = _aip_find_by_id((aip_id))) == HI_NULL) { \
            HI_LOG_ERR("aip is not create.\n"); \
            HI_ERR_PRINT_H32((aip_id)); \
            return HI_FAILURE; \
        } \
    } while (0)

static aoe_aip_chn_state *_aip_find_by_id(aoe_aip_id aip_id)
{
    aoe_aip_chn_state *state = HI_NULL;

    osal_list_for_each_entry(state, &g_aoe_rm.aip_list, node) {
        if (state->aip == aip_id && state->aip_reg != HI_NULL) {
            return state;
        }
    }

    return HI_NULL;
}

static hi_void aoe_aip_flush_state(aoe_aip_chn_state *state)
{
    state->adj_speed = 0;
    i_hal_aoe_aip_set_adjust_speed(state->aip_reg, state->adj_speed);
}

/* global function */
hi_s32 hal_aoe_init(hi_void)
{
    OSAL_INIT_LIST_HEAD(&g_aoe_rm.aip_list);
    OSAL_INIT_LIST_HEAD(&g_aoe_rm.aop_list);
    OSAL_INIT_LIST_HEAD(&g_aoe_rm.engine_list);

    return i_hal_aoe_init();
}

hi_void hal_aoe_deinit(hi_void)
{
    OSAL_INIT_LIST_HEAD(&g_aoe_rm.aip_list);
    OSAL_INIT_LIST_HEAD(&g_aoe_rm.aop_list);
    OSAL_INIT_LIST_HEAD(&g_aoe_rm.engine_list);

    i_hal_aoe_deinit();
}

static aoe_aip_id aoe_get_free_aip(hi_void)
{
    aoe_aip_id aip_id;

    for (aip_id = AOE_AIP0; aip_id < AOE_AIP_MAX; aip_id++) {
        if (_aip_find_by_id(aip_id) == HI_NULL) {
            return aip_id;
        }
    }

    return AOE_AIP_MAX;
}

hi_s32 hal_aoe_aip_create(aoe_aip_id *p_aip, aoe_aip_chn_attr *attr)
{
    hi_s32 ret;
    aoe_aip_id aip;
    aoe_aip_chn_state *state = HI_NULL;

    aip = aoe_get_free_aip();
    if (aip == AOE_AIP_MAX) {
        HI_LOG_ERR("call aoe_get_free_aip failed!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    state = osal_kmalloc(HI_ID_AO, sizeof(aoe_aip_chn_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_LOG_ERR("KMALLOC aoe_aip_chn_state failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(state, sizeof(aoe_aip_chn_state), 0, sizeof(aoe_aip_chn_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out0;
    }

    state->aip = aip;
    state->status = AOE_AIP_STATUS_STOP;
    osal_list_add_tail(&state->node, &g_aoe_rm.aip_list);

    state->aip_reg = i_hal_aoe_aip_map_reg(aip);
    if (state->aip_reg == HI_NULL) {
        HI_LOG_ERR("i_hal_aoe_aip_map_reg failed\n");
        goto out1;
    }

    ret = hal_aoe_aip_set_attr(aip, attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_set_attr, ret);
        goto out1;
    }

    state->l_volume_db = AOE_AIP_VOL_0DB;
    state->r_volume_db = AOE_AIP_VOL_0DB;
    i_hal_aoe_aip_set_volume(state->aip_reg, AOE_AIP_VOL_0DB, AOE_AIP_VOL_0DB);
    i_hal_aoe_aip_set_prescale(state->aip_reg, AOE_AIP_VOL_0DB, 0);
    state->channel_mode = AIP_CHANNEL_MODE_STEREO;
    i_hal_aoe_aip_set_channel_mode(state->aip_reg, AIP_CHANNEL_MODE_STEREO);

    i_hal_aoe_aip_set_mute(state->aip_reg, HI_FALSE);

    *p_aip = aip;
    return HI_SUCCESS;

out1:
    osal_list_del(&state->node);

out0:
    osal_kfree(HI_ID_AO, state);
    *p_aip = AOE_AIP_MAX;

    return HI_FAILURE;
}

hi_void hal_aoe_aip_destroy(aoe_aip_id aip)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        HI_LOG_ERR("_aip_find_by_id failed\n");
        return;
    }

    if (state->status != AOE_AIP_STATUS_STOP) {
        hal_aoe_aip_stop(aip);
    }

    osal_list_del(&state->node);
    osal_kfree(HI_ID_AO, state);
}

hi_s32 hal_aoe_aip_set_attr(aoe_aip_id aip, aoe_aip_chn_attr *attr)
{
    hi_s32 ret;
    aoe_aip_chn_state *state = HI_NULL;
    hi_u32 *wptr_addr = HI_NULL;
    hi_u32 *rptr_addr = HI_NULL;
    aoe_aip_in_buf_attr *in_attr = HI_NULL;
    aoe_aip_out_fifo_attr *out_attr = HI_NULL;

    CHECK_AO_NULL_PTR(attr);
    AIP_FIND_BY_ID(aip, state);

    in_attr = &attr->buf_in_attr;
    out_attr = &attr->fifo_out_attr;
    if (!in_attr->rbf_attr.buf_phy_addr || !in_attr->rbf_attr.buf_vir_addr) {
        HI_LOG_ERR("Invalid buffer address\n");
        HI_ERR_PRINT_H64(in_attr->rbf_attr.buf_phy_addr);
        HI_ERR_PRINT_H64(in_attr->rbf_attr.buf_vir_addr);
        return HI_FAILURE;
    }
    if (!in_attr->rbf_attr.buf_size) {
        HI_LOG_ERR("Invalid buffer size\n");
        HI_ERR_PRINT_H32(in_attr->rbf_attr.buf_size);
        return HI_FAILURE;
    }

    if (out_attr->fifo_latency_thd_ms < AOE_AIP_FIFO_LATENCYMS_MIN) {
        HI_LOG_ERR("Invalid fifo latency\n");
        HI_ERR_PRINT_U32(out_attr->fifo_latency_thd_ms);
        HI_ERR_PRINT_U32(AOE_AIP_FIFO_LATENCYMS_MIN);
        return HI_FAILURE;
    }

    if (state->status != AOE_AIP_STATUS_STOP) {
        return HI_FAILURE;
    }

    attr->fifo_bypass = state->fifo_bypass;
    i_hal_aoe_aip_set_attr(state->aip_reg, attr);

    if (!attr->buf_in_attr.rbf_attr.buf_wptr_rptr_flag) {
        i_hal_aoe_aip_get_rptr_and_wptr_reg_addr(state->aip_reg, &wptr_addr, &rptr_addr);
        circ_buf_init(&state->cb,
                      wptr_addr,
                      rptr_addr,
                      (hi_u8 *)HI_NULL + attr->buf_in_attr.rbf_attr.buf_vir_addr,
                      attr->buf_in_attr.rbf_attr.buf_size);
    }

    state->buf_frame_size = autil_calc_frame_size(attr->buf_in_attr.buf_channels,
        attr->buf_in_attr.buf_bit_per_sample);

    state->fifo_frame_size = autil_calc_frame_size(attr->fifo_out_attr.fifo_channels,
        attr->fifo_out_attr.fifo_bit_per_sample);

    state->adj_speed = 0;
    i_hal_aoe_aip_set_adjust_speed(state->aip_reg, 0);

    ret = memcpy_s(&state->user_attr, sizeof(aoe_aip_chn_attr), attr, sizeof(aoe_aip_chn_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_get_attr(aoe_aip_id aip, aoe_aip_chn_attr *attr)
{
    hi_s32 ret;
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    ret = memcpy_s(attr, sizeof(aoe_aip_chn_attr), &state->user_attr, sizeof(aoe_aip_chn_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void hal_aoe_aip_get_status_phy(hi_u32 aip, hi_u64 *phy_delay_addr)
{
    i_hal_aoe_aip_get_status_phy(aip, phy_delay_addr);
}

static hi_s32 __hal_aoe_aip_wait_cmd_done(aoe_aip_chn_state *state)
{
    volatile hi_u32 loop_outer = 0;
    volatile hi_u32 loop_inner = 0;

    for (loop_outer = 0; loop_outer < DSP_ACK_TIME; loop_outer++) {
        for (loop_inner = 0; loop_inner < 8; loop_inner++) {
            if (i_hal_aoe_aip_get_cmd_done(state->aip_reg) == HI_TRUE) {
                return AOE_AIP_CMD_DONE;
            }
            osal_udelay(10);
        }
        osal_msleep(1);
    }

    return AOE_AIP_CMD_ERR_TIMEOUT;
}

static hi_s32 hal_aoe_aip_wait_cmd_done(aoe_aip_chn_state *state)
{
    hi_s32 ret;

    ret = __hal_aoe_aip_wait_cmd_done(state);
    if (ret != AOE_AIP_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(__hal_aoe_aip_wait_cmd_done, ret);
        return ret;
    }

    ret = i_hal_aoe_aip_get_cmd_ack(state->aip_reg);
    if (ret != AOE_AIP_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aoe_aip_get_cmd_ack, ret);
        return ret;
    }

    return HI_SUCCESS;
}

/*
 * hal_aoe_aip_group_set_cmd - set command to several aip at the same time
 * @aip_mask: aip id mask, store aip(s) need to send command
 * @aip_status: change aip status to this after command processed
 * @ready_mask: if current aip status match ready mask then this aip is already ready,
 *              which means there is no need to send command again.
 * @cmd: command to send
 */
static hi_s32 hal_aoe_aip_group_set_cmd(hi_u32 aip_mask,
    aoe_aip_status aip_status, hi_u32 ready_mask, aoe_aip_cmd cmd)
{
    hi_s32 ret;
    aoe_aip_chn_state *state = HI_NULL;

    osal_list_for_each_entry(state, &g_aoe_rm.aip_list, node) {
        if (!((1 << state->aip) & aip_mask)) {
            continue;
        }

        /* if aip status is already right, then remove aip from aip mask */
        if (state->status & ready_mask) {
            aip_mask ^= (1 << state->aip);
        }
    }

    /* no aip left in aip mask need to process */
    if (aip_mask == 0) {
        return HI_SUCCESS;
    }

    osal_list_for_each_entry(state, &g_aoe_rm.aip_list, node) {
        if ((1 << state->aip) & aip_mask) {
            i_hal_aoe_aip_set_cmd(state->aip_reg, cmd);
        }
    }

    osal_list_for_each_entry(state, &g_aoe_rm.aip_list, node) {
        if (!((1 << state->aip) & aip_mask)) {
            continue;
        }

        ret = hal_aoe_aip_wait_cmd_done(state);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(hal_aoe_aip_wait_cmd_done, ret);
            HI_ERR_PRINT_H32(state->aip);
            HI_ERR_PRINT_H32(cmd);
            return ret;
        }

        /* flush aip state when execute start or stop cmd */
        if (cmd == AOE_AIP_CMD_START || cmd == AOE_AIP_CMD_STOP) {
            aoe_aip_flush_state(state);
        }

        /* do not update aip status when execute flush cmd */
        if (cmd != AOE_AIP_CMD_FLUSH) {
            state->status = aip_status;
        }
    }

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_start(aoe_aip_id aip)
{
    return hal_aoe_aip_group_start(1 << aip);
}

hi_s32 hal_aoe_aip_group_start(hi_u32 start_mask)
{
    return hal_aoe_aip_group_set_cmd(start_mask,
        AOE_AIP_STATUS_START, AOE_AIP_STATUS_START, AOE_AIP_CMD_START);
}

hi_s32 hal_aoe_aip_stop(aoe_aip_id aip)
{
    return hal_aoe_aip_group_stop(1 << aip);
}

hi_s32 hal_aoe_aip_group_stop(hi_u32 stop_mask)
{
    return hal_aoe_aip_group_set_cmd(stop_mask,
        AOE_AIP_STATUS_STOP, AOE_AIP_STATUS_STOP, AOE_AIP_CMD_STOP);
}

hi_s32 hal_aoe_aip_pause(aoe_aip_id aip)
{
    return hal_aoe_aip_group_pause(1 << aip);
}

hi_s32 hal_aoe_aip_group_pause(hi_u32 pause_mask)
{
    return hal_aoe_aip_group_set_cmd(pause_mask,
        AOE_AIP_STATUS_PAUSE,
        AOE_AIP_STATUS_PAUSE | AOE_AIP_STATUS_STOP,
        AOE_AIP_CMD_PAUSE);
}

hi_s32 hal_aoe_aip_flush(aoe_aip_id aip)
{
    return hal_aoe_aip_group_set_cmd(1 << aip,
        AIP_STATUS_MAX, /* unused */
        AOE_AIP_STATUS_STOP, AOE_AIP_CMD_FLUSH);
}

hi_s32 hal_aoe_aip_set_volume(aoe_aip_id aip, hi_u32 l_volume_db, hi_u32 r_volume_db)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_volume(state->aip_reg, l_volume_db, r_volume_db);

    state->l_volume_db = l_volume_db;
    state->r_volume_db = r_volume_db;

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_prescale(aoe_aip_id aip, hi_u32 int_db, hi_s32 dec_db)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_prescale(state->aip_reg, int_db, dec_db);
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_mute(aoe_aip_id aip, hi_bool mute)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_mute(state->aip_reg, mute);

    state->mute = mute;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_channel_mode(aoe_aip_id aip, hi_u32 channel_mode)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_channel_mode(state->aip_reg, channel_mode);

    state->channel_mode = channel_mode;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_adjust_speed(aoe_aip_id aip, hi_s32 adj_speed)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_adjust_speed(state->aip_reg, adj_speed);
    state->adj_speed = adj_speed;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_speed(aoe_aip_id aip, hi_ao_speed *speed)
{
    adsp_cmd_arg cmd;

    CHECK_AO_NULL_PTR(speed);
    CHECK_AIP_OPEN(aip);

    cmd.argc.cmd = ADSP_CMD_AIP_SET_SPEED;
    cmd.argc.id = aip;
    cmd.argc.cmd_type = ADSP_CMD_TYPE_AIP;
    cmd.argc.size = sizeof(hi_ao_speed);
    cmd.argv = (hi_char *)speed;

    return i_hal_aoe_proc_cmd(&cmd);
}

hi_u32 hal_aoe_aip_query_buf_data(aoe_aip_id aip)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        HI_LOG_WARN("aip is not create.\n");
        HI_WARN_PRINT_H32(aip);
        return 0;
    }

    if (state->status == AOE_AIP_STATUS_STOP) {
        return 0;
    }

    if (state->user_attr.buf_in_attr.rbf_attr.buf_wptr_rptr_flag) {
        HI_LOG_WARN("don't support aip_query_buf_data whent buf_wptr_rptr_flag(1)\n");
        return 0;
    }

    return circ_buf_query_busy(&state->cb);
}

hi_u32 hal_aoe_aip_query_buf_free(aoe_aip_id aip)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        HI_LOG_WARN("aip is not create.\n");
        HI_WARN_PRINT_H32(aip);
        return 0;
    }

    if (state->user_attr.buf_in_attr.rbf_attr.buf_wptr_rptr_flag) {
        HI_LOG_WARN("don't support aip_query_buf_free whent buf_wptr_rptr_flag(1)\n");
        return 0;
    }

    return circ_buf_query_free(&state->cb);
}

hi_u32 hal_aoe_aip_write_buf_data(aoe_aip_id aip, hi_u8 *src, hi_u32 size)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        HI_LOG_WARN("aip is not create.\n");
        HI_WARN_PRINT_H32(aip);
        return 0;
    }

    if (size == 0) {
        return 0;
    }

    if (state->user_attr.buf_in_attr.rbf_attr.buf_wptr_rptr_flag) {
        HI_LOG_WARN("don't support aip_write_buf_data when buf_wptr_rptr_flag(1)\n");
        return 0;
    }

    return circ_buf_write(&state->cb, src, size, state->data_from_kernel);
}

hi_void hal_aoe_aip_add_mute_at_read(aoe_aip_id aip, hi_u32 mute_ms)
{
    hi_u32 size;
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        HI_LOG_WARN("aip is not create.\n");
        HI_WARN_PRINT_H32(aip);
        return;
    }

    if (mute_ms == 0 || state->status == AOE_AIP_STATUS_START) {
        return;
    }

    size = autil_latency_ms_to_byte_size(mute_ms, state->buf_frame_size, state->user_attr.buf_in_attr.buf_sample_rate);

    circ_buf_write_mute_at_read_pos(&state->cb, audio_util_align_4(size));
}

hi_void hal_aoe_aip_get_buf_delay_ms(aoe_aip_id aip, hi_u32 *delay_ms)
{
    hi_u32 free_bytes;
    aoe_aip_chn_state *state = _aip_find_by_id(aip);

    if (state == HI_NULL) {
        *delay_ms = 0;
        return;
    }

    if (state->user_attr.buf_in_attr.rbf_attr.buf_wptr_rptr_flag) {
        HI_LOG_WARN("dont support aip_get_buf_delay_ms whent buf_wptr_rptr_flag(1)\n");
        *delay_ms = 0;
        return;
    }

    free_bytes = circ_buf_query_busy(&state->cb);
    *delay_ms = calc_latency_ms(state->user_attr.buf_in_attr.buf_sample_rate, state->buf_frame_size, free_bytes);
}

hi_void hal_aoe_aip_get_fifo_delay_ms(aoe_aip_id aip, hi_u32 *delay_ms)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);

    if (state == HI_NULL) {
        *delay_ms = 0;
        return;
    }

    if (state->status == AOE_AIP_STATUS_STOP) {
        *delay_ms = 0;
        return;
    }

    *delay_ms = i_hal_aoe_aip_get_fifo_delay_ms(state->aip_reg);
}

hi_void hal_aoe_aip_get_status(aoe_aip_id aip, aoe_aip_status *status)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        *status = AOE_AIP_STATUS_STOP;
        return;
    }

    *status = state->status;
}

hi_void hal_aoe_aip_set_data_source(aoe_aip_id aip, hi_bool data_from_kernel)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        return;
    }

    state->data_from_kernel = data_from_kernel;
}

hi_void hal_aoe_aip_set_eos(aoe_aip_id aip, hi_bool eos)
{
    aoe_aip_chn_state *state = _aip_find_by_id(aip);
    if (state == HI_NULL) {
        return;
    }

    i_hal_aoe_aip_set_eos(state->aip_reg, eos);
}

hi_s32 hal_aoe_aip_set_add_mute_ms(aoe_aip_id aip, hi_u32 mute_ms)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_add_mute_ms(state->aip_reg, mute_ms);
    return HI_SUCCESS;
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_u64 hal_aoe_aip_get_reg_addr(aoe_aip_id aip)
{
    return i_hal_aoe_aip_get_reg_addr(aip);
}

hi_s32 hal_aoe_aip_set_fifo_bypass(aoe_aip_id aip, hi_bool enable)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_fifo_bypass(state->aip_reg, enable);

    state->fifo_bypass = enable;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_get_fifo_bypass(aoe_aip_id aip, hi_bool *enable)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    *enable = state->fifo_bypass;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_get_mix_priority(aoe_aip_id aip, hi_bool *priority)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    *priority = state->user_attr.buf_in_attr.mix_priority;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_mix_priority(aoe_aip_id aip, hi_bool priority)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_mix_priority(state->aip_reg, priority);

    state->user_attr.buf_in_attr.mix_priority = priority;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_get_fifo_latency(aoe_aip_id aip, hi_u32 *latency_ms)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    *latency_ms = state->user_attr.fifo_out_attr.fifo_latency_thd_ms;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_fifo_latency(aoe_aip_id aip, hi_u32 latency_ms)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_fifo_latency(state->aip_reg, latency_ms);
    state->user_attr.fifo_out_attr.fifo_latency_thd_ms = latency_ms;

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_resume_threshold_ms(aoe_aip_id aip, hi_u32 threshold_ms)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_resume_threshold_ms(state->aip_reg, threshold_ms);

    state->user_attr.buf_in_attr.resume_threshold_ms = threshold_ms;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_get_resume_threshold_ms(aoe_aip_id aip, hi_u32 *threshold_ms)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    *threshold_ms = state->user_attr.buf_in_attr.resume_threshold_ms;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aip_set_fade_attr(aoe_aip_id aip, hi_bool enable, hi_u32 fade_in_ms, hi_u32 fade_out_ms)
{
    aoe_aip_chn_state *state = HI_NULL;

    AIP_FIND_BY_ID(aip, state);

    i_hal_aoe_aip_set_fade(state->aip_reg, enable, fade_in_ms, fade_out_ms);
    state->user_attr.buf_in_attr.fade_enable = enable;

    return HI_SUCCESS;
}
#endif

/* hal aoe aop function */
typedef struct {
    aoe_aop_chn_attr user_attr;

    aoe_aop_id aop;
    aop_regs_type *aop_reg;
    aoe_aop_status status;
    struct osal_list_head node;

    hi_bool mute;
    hi_u32 l_volume_db;
    hi_u32 r_volume_db;

    hi_bool drc_enable;
    hi_ao_drc_attr drc_attr;

    hi_bool peq_enable;
    hi_ao_peq_attr peq_attr;

    hi_bool ad_output_enable;

    hi_bool bypass;
    hi_u32 delay_ms;
} aoe_aop_chn_state;

#define CHECK_AOP_OPEN(aop_id) \
    do { \
        if (_aop_find_by_id((aop_id)) == HI_NULL) { \
            HI_LOG_ERR("aop is not create.\n"); \
            HI_ERR_PRINT_H32((aop_id)); \
            return HI_FAILURE; \
        } \
    } while (0)

#define AOP_FIND_BY_ID(aop_id, aop_state)  \
    do { \
        if (((aop_state) = _aop_find_by_id((aop_id))) == HI_NULL) { \
            HI_LOG_ERR("aop is not create.\n"); \
            HI_ERR_PRINT_H32((aop_id)); \
            return HI_FAILURE; \
        } \
    } while (0)

static aoe_aop_chn_state *_aop_find_by_id(aoe_aop_id aop_id)
{
    aoe_aop_chn_state *state = HI_NULL;

    osal_list_for_each_entry(state, &g_aoe_rm.aop_list, node) {
        if (state->aop == aop_id && state->aop_reg != HI_NULL) {
            return state;
        }
    }

    return HI_NULL;
}

static aoe_aop_id aoe_get_free_aop(hi_void)
{
    aoe_aop_id aop_id;

    for (aop_id = AOE_AOP0; aop_id < AOE_AOP_MAX; aop_id++) {
        if (_aop_find_by_id(aop_id) == HI_NULL) {
            return aop_id;
        }
    }

    return AOE_AOP_MAX;
}

#if defined (HI_SOUND_PORT_DELAY_SUPPORT)
hi_s32 hal_aoe_aop_set_delay(aoe_aop_id aop, hi_u32 real_delay_ms)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_delay(state->aop_reg, real_delay_ms);

    state->delay_ms = real_delay_ms;

    HI_INFO_PRINT_H32(aop);
    HI_INFO_PRINT_U32(real_delay_ms);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_get_delay(aoe_aop_id aop, hi_u32 *real_delay_ms)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_get_delay(state->aop_reg, real_delay_ms);

    HI_INFO_PRINT_H32(aop);
    HI_INFO_PRINT_U32(*real_delay_ms);

    return HI_SUCCESS;
}
#endif

hi_s32 hal_aoe_aop_set_mute(aoe_aop_id aop, hi_bool mute)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_mute(state->aop_reg, mute);

    state->mute = mute;

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_set_volume(aoe_aop_id aop, hi_u32 l_volume_db, hi_u32 r_volume_db)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_volume(state->aop_reg, l_volume_db, r_volume_db);

    state->l_volume_db = l_volume_db;
    state->r_volume_db = r_volume_db;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_set_preci_vol(aoe_aop_id aop, hi_u32 int_db, hi_s32 dec_db)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_preci_vol(state->aop_reg, int_db, dec_db);
    return HI_SUCCESS;
}

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
hi_s32 hal_aoe_aop_get_rptr_and_wptr_reg_addr(aoe_aop_id aop, hi_void **wptr_reg, hi_void **rptr_reg)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_get_rptr_and_wptr_reg_addr(state->aop_reg, wptr_reg, rptr_reg);
    return HI_SUCCESS;
}
#endif

hi_s32 hal_aoe_aop_set_attr(aoe_aop_id aop, aoe_aop_chn_attr *attr)
{
    hi_s32 ret;
    aoe_aop_chn_state *state = HI_NULL;
    aoe_aop_out_buf_attr *out_attr = HI_NULL;

    CHECK_AO_NULL_PTR(attr);
    AOP_FIND_BY_ID(aop, state);

    out_attr = &attr->rbf_out_attr;
    if (!out_attr->rbf_attr.buf_phy_addr || !out_attr->rbf_attr.buf_vir_addr) {
        HI_LOG_ERR("Invalid buffer address\n");
        HI_ERR_PRINT_H64(out_attr->rbf_attr.buf_phy_addr);
        HI_ERR_PRINT_H64(out_attr->rbf_attr.buf_vir_addr);
        return HI_FAILURE;
    }
    if (!out_attr->rbf_attr.buf_size) {
        HI_LOG_ERR("Invalid buffer size\n");
        HI_ERR_PRINT_H32(out_attr->rbf_attr.buf_size);
        return HI_FAILURE;
    }

    if (out_attr->buf_latency_thd_ms < AOE_AOP_BUFF_LATENCYMS_MIN) {
        HI_LOG_ERR("Invalid fifo latency\n");
        HI_ERR_PRINT_U32(out_attr->buf_latency_thd_ms);
        return HI_FAILURE;
    }

    i_hal_aoe_aop_set_attr(state->aop_reg, attr);

    ret = memcpy_s(&state->user_attr, sizeof(aoe_aop_chn_attr), attr, sizeof(aoe_aop_chn_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_create(aoe_aop_id *p_aop, aoe_aop_chn_attr *attr)
{
    hi_s32 ret;
    aoe_aop_id aop;
    aoe_aop_chn_state *state = HI_NULL;

    aop = aoe_get_free_aop();
    if (aop == AOE_AOP_MAX) {
        HI_LOG_ERR("call aoe_get_free_aop failed!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    state = osal_kmalloc(HI_ID_AO, sizeof(aoe_aop_chn_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_LOG_ERR("KMALLOC aoe_aop_chn_attr failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(state, sizeof(aoe_aop_chn_state), 0, sizeof(aoe_aop_chn_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out0;
    }

    state->aop = aop;
    state->status = AOE_AOP_STATUS_STOP;
    osal_list_add_tail(&state->node, &g_aoe_rm.aop_list);

    state->aop_reg = i_hal_aoe_aop_map_reg(aop);
    if (state->aop_reg == HI_NULL) {
        HI_LOG_ERR("i_hal_aoe_aop_map_reg failed\n");
        goto out1;
    }

    ret = hal_aoe_aop_set_attr(aop, attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_set_attr, ret);
        goto out1;
    }

    state->l_volume_db = AOE_AIP_VOL_0DB;
    state->r_volume_db = AOE_AIP_VOL_0DB;
    i_hal_aoe_aop_set_volume(state->aop_reg, AOE_AIP_VOL_0DB, AOE_AIP_VOL_0DB);
    i_hal_aoe_aop_set_mute(state->aop_reg, HI_FALSE);
#if defined (HI_SOUND_PORT_DELAY_SUPPORT)
    i_hal_aoe_aop_set_delay(state->aop_reg, 0);
#endif

    *p_aop = aop;
    return HI_SUCCESS;

out1:
    osal_list_del(&state->node);

out0:
    osal_kfree(HI_ID_AO, state);
    *p_aop = AOE_AOP_MAX;

    return HI_FAILURE;
}

hi_void hal_aoe_aop_destroy(aoe_aop_id aop)
{
    aoe_aop_chn_state *state = _aop_find_by_id(aop);
    if (state == HI_NULL) {
        return;
    }

    osal_list_del(&state->node);
    osal_kfree(HI_ID_AO, state);
}

hi_s32 hal_aoe_aop_get_attr(aoe_aop_id aop, aoe_aop_chn_attr *attr)
{
    hi_s32 ret;
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);

    ret = memcpy_s(attr, sizeof(aoe_aop_chn_attr), &state->user_attr, sizeof(aoe_aop_chn_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 __hal_aoe_aop_wait_cmd_done(aoe_aop_chn_state *state)
{
    volatile hi_u32 loop_outer = 0;
    volatile hi_u32 loop_inner = 0;

    for (loop_outer = 0; loop_outer < DSP_ACK_TIME; loop_outer++) {
        for (loop_inner = 0; loop_inner < 8; loop_inner++) {
            if (i_hal_aoe_aop_get_cmd_done(state->aop_reg) == HI_TRUE) {
                return AOE_AOP_CMD_DONE;
            }
            osal_udelay(10);
        }
        osal_msleep(1);
    }

    return AOE_AOP_CMD_ERR_TIMEOUT;
}

static hi_s32 hal_aoe_aop_wait_cmd_done(aoe_aop_chn_state *state)
{
    hi_s32 ret;

    ret = __hal_aoe_aop_wait_cmd_done(state);
    if (ret != AOE_AOP_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(__hal_aoe_aop_wait_cmd_done, ret);
        return ret;
    }

    ret = i_hal_aoe_aop_get_cmd_ack(state->aop_reg);
    if (ret != AOE_AOP_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aoe_aop_get_cmd_ack, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 hal_aoe_aop_set_cmd(aoe_aop_id aop,
    aoe_aop_status aop_status, aoe_aop_cmd cmd)
{
    hi_s32 ret;
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);

    if (state->status == aop_status) {
        return HI_SUCCESS;
    }

    i_hal_aoe_aop_set_cmd(state->aop_reg, cmd);

    ret = hal_aoe_aop_wait_cmd_done(state);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_aop_wait_cmd_done, ret);
        return ret;
    }

    state->status = aop_status;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_start(aoe_aop_id aop)
{
    return hal_aoe_aop_set_cmd(aop, AOE_AOP_STATUS_START, AOE_AOP_CMD_START);
}

hi_s32 hal_aoe_aop_stop(aoe_aop_id aop)
{
    return hal_aoe_aop_set_cmd(aop, AOE_AOP_STATUS_STOP, AOE_AOP_CMD_STOP);
}

hi_s32 hal_aoe_aop_set_aef_bypass(aoe_aop_id aop, hi_bool bypass)
{
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_aef_bypass(state->aop_reg, bypass);
    state->bypass = bypass;

    return HI_SUCCESS;
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 hal_aoe_aop_set_balance(aoe_aop_id aop, hi_s32 balance)
{
    aoe_aop_chn_state *state = HI_NULL;
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_balance(state->aop_reg, balance);
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_set_low_latency(aoe_aop_id aop, hi_u32 latency)
{
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);
    i_hal_aoe_aop_set_latency(state->aop_reg, latency);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_get_low_latency(aoe_aop_id aop, hi_u32 *latency)
{
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);
    i_hal_aoe_aop_get_latency(state->aop_reg, latency);

    return HI_SUCCESS;
}
#endif

#ifdef HI_SND_DRC_SUPPORT
hi_s32 hal_aoe_aop_set_drc_enable(aoe_aop_id aop, hi_bool enable)
{
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_drc_enable(state->aop_reg, enable);
    i_hal_aoe_aop_set_drc_change_attr(state->aop_reg, HI_TRUE);

    state->drc_enable = enable;
    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_set_drc_attr(aoe_aop_id aop, hi_ao_drc_attr *drc_attr)
{
    hi_s32 ret;
    aoe_aop_chn_state *state = HI_NULL;

    CHECK_AO_NULL_PTR(drc_attr);
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_drc_attr(state->aop_reg, drc_attr);
    i_hal_aoe_aop_set_drc_change_attr(state->aop_reg, HI_TRUE);

    ret = memcpy_s(&state->drc_attr, sizeof(hi_ao_drc_attr), drc_attr, sizeof(hi_ao_drc_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}
#endif

#ifdef HI_SND_PEQ_SUPPORT
hi_s32 hal_aoe_aop_set_peq_attr(aoe_aop_id aop, hi_ao_peq_attr *peq_attr)
{
    hi_s32 ret;
    aoe_aop_chn_state *state = HI_NULL;

    CHECK_AO_NULL_PTR(peq_attr);
    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_peq_attr(state->aop_reg, peq_attr);
    i_hal_aoe_aop_set_peq_change_attr(state->aop_reg, HI_TRUE);

    ret = memcpy_s(&state->peq_attr, sizeof(hi_ao_peq_attr), peq_attr, sizeof(hi_ao_peq_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hal_aoe_aop_set_peq_enable(aoe_aop_id aop, hi_bool enable)
{
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_peq_enable(state->aop_reg, enable);
    i_hal_aoe_aop_set_peq_change_attr(state->aop_reg, HI_TRUE);

    state->peq_enable = enable;
    return HI_SUCCESS;
}
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 hal_aoe_aop_set_ad_output_enable(aoe_aop_id aop, hi_bool enable)
{
    aoe_aop_chn_state *state = HI_NULL;

    AOP_FIND_BY_ID(aop, state);

    i_hal_aoe_aop_set_ad_output_enable(state->aop_reg, enable);

    state->ad_output_enable = enable;
    return HI_SUCCESS;
}
#endif

/* hal aoe engine function */
typedef struct {
    aoe_engine_id engine_id;
    engine_regs_type *engine_reg;
    aoe_engine_status status;
    struct osal_list_head node;

    aoe_engine_attr user_attr;
} aoe_engine_chn_state;

#define CHECK_ENGINE_OPEN(engine_id) \
    do {  \
        if (_engine_find_by_id((engine_id)) == HI_NULL) { \
            HI_LOG_ERR("engine is not create.\n"); \
            HI_ERR_PRINT_H32((engine_id)); \
            return HI_FAILURE; \
        } \
    } while (0)

#define ENGINE_FIND_BY_ID(engine_id, engine_state) \
    do { \
        if (((engine_state) = _engine_find_by_id((engine_id))) == HI_NULL) { \
            HI_LOG_ERR("engine is not create.\n"); \
            HI_ERR_PRINT_H32((engine_id)); \
            return HI_FAILURE; \
        } \
    } while (0)

static aoe_engine_chn_state *_engine_find_by_id(aoe_engine_id engine_id)
{
    aoe_engine_chn_state *state = HI_NULL;

    osal_list_for_each_entry(state, &g_aoe_rm.engine_list, node) {
        if (state->engine_id == engine_id && state->engine_reg != HI_NULL) {
            return state;
        }
    }

    return HI_NULL;
}

static inline aoe_engine_id aoe_get_free_engine(hi_void)
{
    aoe_engine_id engine_id;

    for (engine_id = AOE_ENGINE0; engine_id < AOE_ENGINE_MAX; engine_id++) {
        if (_engine_find_by_id(engine_id) == HI_NULL) {
            return engine_id;
        }
    }

    return AOE_ENGINE_MAX;
}

hi_s32 hal_aoe_engine_create(aoe_engine_id *p_engine, aoe_engine_attr *attr)
{
    hi_s32 ret;
    aoe_engine_id engine;
    aoe_engine_chn_state *state = HI_NULL;

    engine = aoe_get_free_engine();
    if (engine == AOE_ENGINE_MAX) {
        HI_LOG_ERR("call aoe_get_free_engine failed!\n");
        return HI_ERR_AO_NOTSUPPORT;
    }

    state = osal_kmalloc(HI_ID_AO, sizeof(aoe_engine_chn_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_LOG_ERR("KMALLOC aoe_engine_chn_state failed\n");
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(state, sizeof(aoe_engine_chn_state), 0, sizeof(aoe_engine_chn_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out0;
    }

    state->engine_id = engine;
    state->status = AOE_ENGINE_STATUS_STOP;
    osal_list_add_tail(&state->node, &g_aoe_rm.engine_list);

    state->engine_reg = i_hal_aoe_engine_map_reg(engine);
    if (state->engine_reg == HI_NULL) {
        HI_LOG_ERR("i_hal_aoe_engine_map_reg failed\n");
        goto out1;
    }

    ret = hal_aoe_engine_set_attr(engine, attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_set_attr, ret);
        goto out1;
    }

    *p_engine = engine;
    return HI_SUCCESS;

out1:
    osal_list_del(&state->node);

out0:
    osal_kfree(HI_ID_AO, state);
    *p_engine = AOE_ENGINE_MAX;

    return HI_FAILURE;
}

hi_void hal_aoe_engine_destroy(aoe_engine_id engine)
{
    aoe_engine_chn_state *state = _engine_find_by_id(engine);
    if (state == HI_NULL) {
        return;
    }

    osal_list_del(&state->node);
    osal_kfree(HI_ID_AO, state);
}

hi_s32 hal_aoe_engine_set_attr(aoe_engine_id engine, aoe_engine_attr *attr)
{
    hi_s32 ret;
    aoe_engine_chn_state *state = HI_NULL;

    CHECK_AO_NULL_PTR(attr);

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_attr(state->engine_reg, attr);

    ret = memcpy_s(&state->user_attr, sizeof(aoe_engine_attr), attr, sizeof(aoe_engine_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_get_attr(aoe_engine_id engine, aoe_engine_attr *attr)
{
    hi_s32 ret;
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);

    ret = memcpy_s(attr, sizeof(aoe_engine_attr), &state->user_attr, sizeof(aoe_engine_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef HI_SND_AVC_SUPPORT
hi_s32 hal_aoe_engine_set_avc_attr(aoe_engine_id engine, hi_ao_avc_attr *avc_attr)
{
    aoe_engine_chn_state *state = HI_NULL;

    CHECK_AO_NULL_PTR(avc_attr);
    ENGINE_FIND_BY_ID(engine, state);

    i_hal_aoe_engine_set_avc_attr(state->engine_reg, avc_attr);
    i_hal_aoe_engine_set_avc_change_attr(state->engine_reg, HI_TRUE);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_set_avc_enable(aoe_engine_id engine, hi_bool enable)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_avc_enable(state->engine_reg, enable);
    i_hal_aoe_engine_set_avc_change_attr(state->engine_reg, HI_TRUE);

    return HI_SUCCESS;
}
#endif

#ifdef HI_SND_AR_SUPPORT
hi_s32 hal_aoe_engine_set_con_output_enable(aoe_engine_id engine, hi_bool enable)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_con_output_enable(state->engine_reg, enable);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_set_output_atmos_enable(aoe_engine_id engine, hi_bool enable)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_output_atmos_enable(state->engine_reg, enable);

    return HI_SUCCESS;
}
#endif

#ifdef HI_SND_GEQ_SUPPORT
hi_s32 hal_aoe_engine_set_geq_attr(aoe_engine_id engine, hi_ao_geq_attr *geq_attr)
{
    aoe_engine_chn_state *state = HI_NULL;

    CHECK_AO_NULL_PTR(geq_attr);

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_geq_attr(state->engine_reg, geq_attr);
    i_hal_aoe_aop_set_geq_change_attr(state->engine_reg, HI_TRUE);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_set_geq_enable(aoe_engine_id engine, hi_bool enable)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_geq_enable(state->engine_reg, enable);
    i_hal_aoe_aop_set_geq_change_attr(state->engine_reg, HI_TRUE);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_set_geq_gain(aoe_engine_id engine, hi_u32 band, hi_s32 gain)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_set_geq_gain(state->engine_reg, band, gain);
    i_hal_aoe_aop_set_geq_change_attr(state->engine_reg, HI_TRUE);

    return HI_SUCCESS;
}
#endif

static hi_s32 __hal_aoe_engine_wait_cmd_done(aoe_engine_chn_state *state)
{
    volatile hi_u32 loop_outer = 0;
    volatile hi_u32 loop_inner = 0;

    for (loop_outer = 0; loop_outer < DSP_ACK_TIME; loop_outer++) {
        for (loop_inner = 0; loop_inner < 8; loop_inner++) {
            if (i_hal_aoe_engine_get_cmd_done(state->engine_reg) == HI_TRUE) {
                return AOE_ENGINE_CMD_DONE;
            }
            osal_udelay(10);
        }
        osal_msleep(1);
    }

    return AOE_ENGINE_CMD_ERR_TIMEOUT;
}

static hi_s32 hal_aoe_engine_wait_cmd_done(aoe_engine_chn_state *state)
{
    hi_s32 ret;

    ret = __hal_aoe_engine_wait_cmd_done(state);
    if (ret != AOE_ENGINE_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(__hal_aoe_engine_wait_cmd_done, ret);
        return ret;
    }

    ret = i_hal_aoe_engine_get_cmd_ack(state->engine_reg);
    if (ret != AOE_ENGINE_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aoe_engine_get_cmd_ack, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 hal_aoe_engine_set_cmd(aoe_engine_id engine,
    aoe_engine_status engine_status, aoe_engine_cmd cmd)
{
    hi_s32 ret;
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);

    if (state->status == engine_status) {
        return HI_SUCCESS;
    }

    i_hal_aoe_engine_set_cmd(state->engine_reg, cmd);
    ret = hal_aoe_engine_wait_cmd_done(state);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hal_aoe_engine_wait_cmd_done, ret);
        HI_ERR_PRINT_H32(cmd);
        HI_ERR_PRINT_H32(state->engine_id);
        return ret;
    }

    state->status = engine_status;

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_start(aoe_engine_id engine)
{
    return hal_aoe_engine_set_cmd(engine, AOE_ENGINE_STATUS_START, AOE_ENGINE_CMD_START);
}

hi_s32 hal_aoe_engine_stop(aoe_engine_id engine)
{
    return hal_aoe_engine_set_cmd(engine, AOE_ENGINE_STATUS_STOP, AOE_ENGINE_CMD_STOP);
}

hi_void hal_aoe_engine_attach_aip(aoe_engine_id engine, aoe_aip_id aip)
{
    aoe_engine_chn_state *state = _engine_find_by_id(engine);
    if (state == HI_NULL) {
        HI_LOG_ERR("engine is not create.\n");
        HI_ERR_PRINT_H32(engine);
        return;
    }

    if (_aip_find_by_id(aip) == HI_NULL) {
        HI_LOG_ERR("aip is not create.\n");
        HI_ERR_PRINT_H32(aip);
        return;
    }

    i_hal_aoe_engine_attach_aip(state->engine_reg, aip);
}

hi_void hal_aoe_engine_detach_aip(aoe_engine_id engine, aoe_aip_id aip)
{
    aoe_engine_chn_state *state = _engine_find_by_id(engine);
    if (state == HI_NULL) {
        HI_LOG_ERR("engine is not create.\n");
        HI_ERR_PRINT_H32(engine);
        return;
    }

    if (_aip_find_by_id(aip) == HI_NULL) {
        HI_LOG_ERR("aip is not create.\n");
        HI_ERR_PRINT_H32(aip);
        return;
    }

    i_hal_aoe_engine_detach_aip(state->engine_reg, aip);
}

hi_u32 hal_aoe_engine_get_aop_attach_mask(aoe_engine_id engine)
{
    aoe_engine_chn_state *state = _engine_find_by_id(engine);
    if (state == HI_NULL) {
        HI_LOG_ERR("engine is not create.\n");
        HI_ERR_PRINT_H32(engine);
        return 0;
    }

    return i_hal_aoe_engine_get_aop_attach_mask(state->engine_reg);
}

hi_void hal_aoe_engine_attach_aop(aoe_engine_id engine, aoe_aop_id aop)
{
    aoe_engine_chn_state *state = _engine_find_by_id(engine);
    if (state == HI_NULL) {
        HI_LOG_ERR("engine is not create.\n");
        HI_ERR_PRINT_H32(engine);
        return;
    }

    if (_aop_find_by_id(aop) == HI_NULL) {
        HI_LOG_ERR("aop is not create.\n");
        HI_ERR_PRINT_H32(aop);
        return;
    }

    i_hal_aoe_engine_attach_aop(state->engine_reg, aop);
}

hi_void hal_aoe_engine_detach_aop(aoe_engine_id engine, aoe_aop_id aop)
{
    aoe_engine_chn_state *state = _engine_find_by_id(engine);
    if (state == HI_NULL) {
        HI_LOG_ERR("engine is not create.\n");
        HI_ERR_PRINT_H32(engine);
        return;
    }

    if (_aop_find_by_id(aop) == HI_NULL) {
        HI_LOG_ERR("aop is not create.\n");
        HI_ERR_PRINT_H32(aop);
        return;
    }

    i_hal_aoe_engine_detach_aop(state->engine_reg, aop);
}

#ifdef HI_SND_AEF_SUPPORT
hi_s32 hal_aoe_engine_attach_aef(aoe_engine_id engine, hi_u32 aef_id)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_attach_aef(state->engine_reg, aef_id);

    return HI_SUCCESS;
}

hi_s32 hal_aoe_engine_detach_aef(aoe_engine_id engine, hi_u32 aef_id)
{
    aoe_engine_chn_state *state = HI_NULL;

    ENGINE_FIND_BY_ID(engine, state);
    i_hal_aoe_engine_detach_aef(state->engine_reg, aef_id);

    return HI_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

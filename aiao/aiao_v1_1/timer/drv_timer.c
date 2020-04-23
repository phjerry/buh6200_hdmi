/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao timer driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"

#include "hal_aiao.h"
#include "drv_timer_private.h"
#include "audio_util.h"

static timer_global_resource g_global_timerrs;

static hi_void timer_get_def_param(aiao_timer_attr *timer_default_attr)
{
    timer_default_attr->if_attr.rate = HI_SAMPLE_RATE_48K;
    timer_default_attr->if_attr.bclk_div = HI_I2S_BCLK_4_DIV;
    timer_default_attr->if_attr.fclk_div = HI_I2S_MCLK_256_FS;
    timer_default_attr->config = TIMER_INTERRUPT_CONFIG_DF;
}

static hi_s32 timer_alloc_handle(hi_handle *ph_handle)
{
    hi_u32 i;
    timer_channel_state *state = HI_NULL;

    if (ph_handle == HI_NULL) {
        HI_ERR_AO("bad param null pointer!\n");
        return HI_FAILURE;
    }

    /* allocate new timer channel */
    for (i = 0; i < TIMER_MAX_TOTAL_NUM; i++) {
        if (g_global_timerrs.timer_attr[i] == NULL) {
            break;
        }
    }

    if (i >= AIAO_TIMER_MAX) {
        HI_ERR_AO("too many timer channel!\n");
        return HI_FAILURE;
    }

    state = osal_kmalloc(HI_ID_AO, sizeof(timer_channel_state), OSAL_GFP_KERNEL);
    if (state == HI_NULL) {
        HI_FATAL_AO("osal_kmalloc timer_create failed\n");
        return HI_FAILURE;
    }
    memset(state, 0, sizeof(timer_channel_state));

    g_global_timerrs.timer_attr[i] = state;
    *ph_handle = i;

    return HI_SUCCESS;
}

static hi_void timer_free_handle(hi_handle h_handle)
{
    timer_channel_state *state = HI_NULL;

    state = g_global_timerrs.timer_attr[h_handle];
    if (state == HI_NULL) {
        HI_ERR_AO("timer handle is already free\n");
        return;
    }

    osal_kfree(HI_ID_AO, (hi_void *)state);
    g_global_timerrs.timer_attr[h_handle] = HI_NULL;
}

static hi_s32 timer_create(aiao_timer_create *param)
{
    hi_s32 ret;
    timer_channel_state *state = HI_NULL;
    aiao_timer_id timer_id;

    timer_id = param->timer_id;
    ret = hal_aiao_t_create(timer_id, param);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AO("hal_aiao_t_create failed 0x%x\n", ret);
        return HI_FAILURE;
    }
    state = g_global_timerrs.timer_attr[timer_id];
    state->func = param->timer_isr_func;
    state->substream = param->substream;
    state->enable = HI_FALSE;

    return HI_SUCCESS;
}

static hi_s32 timer_destroy(hi_handle handle)
{
    aiao_timer_id timer_id;

    timer_id = (aiao_timer_id)handle;

    hal_aiao_t_destroy(timer_id);

    return HI_SUCCESS;
}

static hi_s32 timer_set_attr(hi_handle handle, hi_u32 config, aiao_sample_rate rate)
{
    aiao_timer_id timer_id;
    timer_channel_state *state = HI_NULL;
    aiao_timer_attr param;
    hi_s32 ret;

    timer_id = (aiao_timer_id)handle;
    state = g_global_timerrs.timer_attr[timer_id];

    param.timer_id = timer_id;
    param.config = config;
    param.if_attr.rate = rate;
    param.if_attr.bclk_div = HI_I2S_BCLK_4_DIV;

#if 0
    param.if_attr.fclk_div = HI_I2S_MCLK_256_FS;
#else
    param.if_attr.fclk_div = autil_mclk_fclk_div(HI_I2S_MCLK_256_FS);
#endif
    ret = hal_aiao_t_set_timer_attr(timer_id, &param);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("timer ID %d set attr failed\n", (hi_u32)timer_id);
    }

    state->config = config;
    state->if_attr.rate = param.if_attr.rate;
    state->if_attr.bclk_div = param.if_attr.bclk_div;
    state->if_attr.fclk_div = param.if_attr.fclk_div;

    HI_INFO_AO("timer ID %d set attr success\n", (hi_u32)timer_id);
    return ret;
}

static hi_s32 timer_set_enable(hi_handle handle, hi_bool enable)
{
    timer_channel_state *state = HI_NULL;
    aiao_timer_id timer_id;
    hi_s32 ret;

    timer_id = (aiao_timer_id)handle;
    state = g_global_timerrs.timer_attr[timer_id];

    ret = hal_aiao_t_set_timer_enable(timer_id, enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("timer ID %d set enable %d failed\n", (hi_u32)timer_id, enable);
    }

    state->enable = enable;
    HI_INFO_AO("timer ID %d set enable %d success\n", (hi_u32)timer_id, enable);

    return ret;
}

static hi_s32 timer_get_enable(hi_handle handle, hi_bool *enable)
{
    timer_channel_state *state = HI_NULL;
    aiao_timer_id timer_id;

    timer_id = (aiao_timer_id)handle;
    state = g_global_timerrs.timer_attr[timer_id];

    if (state != HI_NULL) {
        *enable = state->enable;
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

static hi_s32 timer_drv_create(hi_handle *ph_handle, aiao_timer_isr_func *func, hi_void *substream)
{
    hi_s32 ret;
    aiao_timer_create param;
    aiao_timer_id alloc_id;

    CHECK_AO_NULL_PTR(ph_handle);

    ret = timer_alloc_handle(&alloc_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("alloc timer handle failed!");
        return ret;
    }
    param.timer_id = alloc_id;
    param.timer_isr_func = func;
    param.substream = substream;
    ret = timer_create(&param);
    if (ret != HI_SUCCESS) {
        timer_free_handle(alloc_id);
        HI_ERR_AO("timer create failed!");
        return ret;
    }
    *ph_handle = alloc_id;

    HI_INFO_AO("timer ID %d create success\n", (hi_u32)alloc_id);
    return ret;
}

static hi_s32 timer_drv_destroy(hi_handle handle)
{
    hi_s32 ret;

    ret = timer_destroy(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("timer_destroy failed %d!", ret);
        return ret;
    }

    timer_free_handle(handle);
    HI_INFO_AO("timer ID %d destroy success\n", (hi_u32)handle);

    return ret;
}

static hi_s32 timer_drv_get_settings(hi_handle handle, snd_timer_settings *timer_settings)
{
    timer_channel_state *state = HI_NULL;

    state = g_global_timerrs.timer_attr[(hi_u32)handle];

    timer_settings->enable = state->enable;
    timer_settings->config = state->config;
    timer_settings->rate = state->if_attr.rate;
    timer_settings->bclk_div = state->if_attr.bclk_div;
    timer_settings->fclk_div = state->if_attr.fclk_div;
    timer_settings->func = state->func;
    timer_settings->substream = state->substream;

    return HI_SUCCESS;
}

static hi_s32 timer_drv_restore_settings(hi_handle handle, snd_timer_settings *timer_settings)
{
    aiao_timer_create param;
    hi_s32 ret;

    param.timer_id = (aiao_timer_id)handle;
    param.timer_isr_func = timer_settings->func;
    param.substream = timer_settings->substream;
    ret = timer_create(&param);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("timer resume create failed!");
        return ret;
    }
    /* set attr */
    timer_set_attr(handle, timer_settings->config, timer_settings->rate);
    HI_INFO_AO("timer ID %d set attr success\n", (aiao_timer_id)handle);
    /* enable */
    timer_set_enable(handle, timer_settings->enable);

    return HI_SUCCESS;
}

hi_s32 hi_drv_timer_get_def_param(aiao_timer_id timer_id, aiao_timer_attr *param)
{
    param->timer_id = timer_id;
    timer_get_def_param(param);

    return HI_SUCCESS;
}

hi_s32 hi_drv_timer_create(hi_handle *ph_handle, aiao_timer_isr_func *func, hi_void *substream)
{
    return timer_drv_create(ph_handle, func, substream);
}

hi_s32 hi_drv_timer_destroy(hi_handle handle)
{
    return timer_drv_destroy(handle);
}

hi_s32 hi_drv_timer_set_attr(hi_handle handle, hi_u32 config, aiao_sample_rate rate)
{
    return timer_set_attr(handle, config, rate);
}

hi_s32 hi_drv_timer_set_enable(hi_handle handle, hi_bool enable)
{
    return timer_set_enable(handle, enable);
}

hi_s32 hi_drv_timer_get_enable(hi_handle handle, hi_bool *enable)
{
    return timer_get_enable(handle, enable);
}

hi_s32 hi_drv_timer_suspend(hi_void)
{
    hi_s32 ret;
    hi_u32 i;

    for (i = 0; i < TIMER_MAX_TOTAL_NUM; i++) {
        if (g_global_timerrs.timer_attr[i]) {
            timer_drv_get_settings(i, &g_global_timerrs.suspend_attr[i]);
            /* destory timer */
            ret = timer_destroy(i);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("timer_destroy failed!");
                return ret;
            }
        }
    }
    HI_PRINT("AO timer suspend OK.\n");
    return HI_SUCCESS;
}

hi_s32 hi_drv_timer_resume(hi_void)
{
    hi_s32 ret;
    hi_u32 i;

    for (i = 0; i < TIMER_MAX_TOTAL_NUM; i++) {
        if (g_global_timerrs.timer_attr[i]) {
            /* recreater timer */
            ret = timer_drv_restore_settings(i, &g_global_timerrs.suspend_attr[i]);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("timer_restore_settings %d failed 0x%x!", i, ret);
                return ret;
            }
        }
    }
    HI_PRINT("AO timer resume OK.\n");
    return HI_SUCCESS;
}

hi_s32 timer_drv_init(hi_void)
{
    memset(&g_global_timerrs, 0, sizeof(timer_global_resource));
    return HI_SUCCESS;
}

hi_void timer_drv_deinit(hi_void)
{
    return;
}

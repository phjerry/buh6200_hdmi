/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: sync stc interface definition
 * Author: Hisilicon multimedia software group
 * Create: 2012-12-22
 */

#include "drv_sync_stc.h"

#ifdef SYNC_NOT_SUPPORT_OSAL
#include <asm/io.h>
#include <asm/div64.h>
#include <linux/delay.h>
#endif

#include "drv_ao_ext.h"
#include "drv_sync.h"
#include "drv_sync_intf.h"
#include "drv_sync_define.h"
#include "drv_stc_hal.h"

#define PPM_AMPLIFY_FACTOR          100
#define PLL_FBDIV_AMPLIFY_FACTOR    0x1000000

static osal_spinlock g_stc_lock;
static hi_u32 g_is_hpll_conf  = HI_FALSE;

#ifdef  AIAO_SRC_PLL_HAS_CORRECTED
hi_u32 g_aud_pll_src_val_int  = 0;
hi_u32 g_aud_pll_src_val_frac = 0;

hi_void sync_set_aud_pll_src_value(sync_aud_pll_value_type type, hi_u32 value)
{
    if (type == SYNC_AUD_PLL_VALUE_TYPE_INT) {
        g_aud_pll_src_val_int = value;
    } else (type == SYNC_AUD_PLL_VALUE_TYPE_FRAC) {
        g_aud_pll_src_val_frac = value;
    }
}

hi_u32 sync_get_aud_pll_src_value(sync_aud_pll_value_type type)
{
    if (type == SYNC_AUD_PLL_VALUE_TYPE_INT) {
        return g_aud_pll_src_val_int;
    } else (type == SYNC_AUD_PLL_VALUE_TYPE_FRAC) {
        return g_aud_pll_src_val_frac;
    }
}
#endif

void sync_stc_sys_init(void)
{
    (void)osal_spin_lock_init(&g_stc_lock);
}

void sync_stc_sys_deinit(void)
{
    osal_spin_lock_destory(&g_stc_lock);
}

static void sync_stc_basic_pll_reset(void)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    stc_hal_read_basic_pll_freq_ctrl_crg(&fbdiv, &frac);
    stc_hal_tune_basic_pll_freq(fbdiv, frac);

    HI_INFO_SYNC("basic pll reset !\n");
}

static void sync_stc_aud_pll_reset(void)
{
    hi_u32 fbdiv;
    hi_u32 frac;

#ifdef AIAO_SRC_PLL_HAS_CORRECTED
    fbdiv = g_aud_pll_src_val_int & (0xfff);
    frac = g_aud_pll_src_val_frac & (0xffffff);
#else
    stc_hal_read_aud_pll_freq_ctrl_crg(&fbdiv, &frac);
#endif

    stc_hal_tune_aud_pll_freq(fbdiv, frac);
    HI_INFO_SYNC("aud pll reset !\n");
}

static void sync_stc_vid_port0_pll_reset(void)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    stc_hal_read_vid_port0_pll_freq_ctrl_crg(&fbdiv, &frac);
    stc_hal_tune_vid_port0_pll_freq(fbdiv, frac);

    HI_INFO_SYNC("vid_port0 Freq Reset !\n");
}

static void sync_stc_vid_port1_pll_reset(void)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    stc_hal_read_vid_port1_pll_freq_ctrl_crg(&fbdiv, &frac);
    stc_hal_tune_vid_port1_pll_freq(fbdiv, frac);

    HI_INFO_SYNC("vid_port1 Freq Reset !\n");
}

static hi_bool sync_check_pll_tune_freq_enable(void)
{
    if (!stc_hal_is_basic_pll_tune_freq_enable()) {
        return HI_FALSE;
    }

    if (!stc_hal_is_aud_pll_tune_freq_enable()) {
        return HI_FALSE;
    }

    if (!stc_hal_is_vid_port0_tune_freq_enable()) {
        return HI_FALSE;
    }

    if (!stc_hal_is_vid_port1_tune_freq_enable()) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_s32 sync_hpll_conf_flag_set(hi_bool is_conf)
{
    osal_spin_lock(&g_stc_lock);

    g_is_hpll_conf = is_conf;

    if (is_conf) {
        stc_hal_disable_tune_vid_port0_pll_freq();
        stc_hal_disable_tune_vid_port1_pll_freq();
        HI_INFO_SYNC("disable hpll turn freq!\n");
    }

    HI_INFO_SYNC("SetHpllConfigFlg:%d\n", is_conf);
    osal_spin_unlock(&g_stc_lock);

    return HI_SUCCESS;
}

static hi_s32 sync_pll_tune_freq_crg_init(void)
{
    hi_u32 fbdiv;
    hi_u32 frac;

    stc_hal_read_aud_pll_tune_freq_crg(&fbdiv, &frac);

    if (fbdiv == 0x0 && frac == 0x0) {
#ifdef AIAO_SRC_PLL_HAS_CORRECTED
        if ((g_aud_pll_src_val_int == 0) || (g_aud_pll_src_val_frac == 0)) {
            HI_ERR_SYNC("aud pll init fail!\n");
            return HI_FAILURE;
        }

        fbdiv = g_aud_pll_src_val_int & (0xfff);
        frac = g_aud_pll_src_val_frac & (0xffffff);
#else
        stc_hal_read_aud_pll_freq_ctrl_crg(&fbdiv, &frac);
#endif
        stc_hal_write_aud_pll_tune_freq_crg(fbdiv, frac);
        HI_INFO_SYNC("aud pll tune freq init success !\n");
    }

    stc_hal_read_basic_pll_tune_freq_crg(&fbdiv, &frac);

    if (fbdiv == 0x0 && frac == 0x0) {
        stc_hal_read_basic_pll_freq_ctrl_crg(&fbdiv, &frac);
        stc_hal_write_basic_pll_tune_freq_crg(fbdiv, frac);
        HI_INFO_SYNC("basic pll tune freq init success !\n");
    }

    stc_hal_read_vid_port0_pll_tune_freq_crg(&fbdiv, &frac);

    if (fbdiv == 0x0 && frac == 0x0) {
        stc_hal_read_vid_port0_pll_freq_ctrl_crg(&fbdiv, &frac);
        stc_hal_write_vid_port0_pll_tune_freq_crg(fbdiv, frac);
        HI_INFO_SYNC("vid port0 pll tune freq init success !\n");
    }

    stc_hal_read_vid_port1_pll_tune_freq_crg(&fbdiv, &frac);

    if (fbdiv == 0x0 && frac == 0x0) {
        stc_hal_read_vid_port1_pll_freq_ctrl_crg(&fbdiv, &frac);
        stc_hal_write_vid_port1_pll_tune_freq_crg(fbdiv, frac);
        HI_INFO_SYNC("vid port1 pll tune freq init success !\n");
    }

    return HI_SUCCESS;
}

static void sync_proc_stc_adjust_fail_evt(void)
{
    hi_u32 j = 0;
    sync_context *ctx = HI_NULL;

    for (j = 0; j < SYNC_MAX_NUM; j++) {
        ctx = sync_info_ctx_get(j);
        if (ctx != HI_NULL && ctx->attr.sync_ref == SYNC_REF_PCR &&
            ctx->pcr_sync_info.pcr_adjust_type == SYNC_PCR_ADJUST_SCR) {
            ctx->pcr_sync_info.stc_info.is_stc_adjust_fail = HI_TRUE;
        }
    }
}

static hi_bool sync_check_stc_adjust_fail_evt(void)
{
    hi_u32 i = 0;
    sync_context *ctx = HI_NULL;

    for (i = 0; i < SYNC_MAX_NUM; i++) {
        ctx = sync_info_ctx_get(i);
        if (ctx != HI_NULL && ctx->attr.sync_ref == SYNC_REF_PCR &&
            ctx->pcr_sync_info.pcr_adjust_type == SYNC_PCR_ADJUST_SCR) {
            if (ctx->pcr_sync_info.stc_info.is_stc_adjust_fail == HI_TRUE) {
                return HI_TRUE;
            }
        }
    }

    return HI_FALSE;
}

static void sync_cal_delta_frac(hi_u32 fbdiv, hi_u32 frac, hi_s32 ppm, hi_s32 *delta)
{
    hi_u64 ppm_expand;

    /*
    computational formula :
    PPM = PPM / 100
    Fout = FReff(fbdiv + frac / 2^24) / RefDiv
    DeltaFrac = (Fout / FReff * RefDiv * PPM / 1000 / 1000 ) << 24
    */
    if (ppm >= 0) {
        ppm_expand = ppm;
        ppm_expand = ((ppm_expand * fbdiv) << 24) + ppm_expand * frac;
        ppm_expand = osal_div_u64(ppm_expand, 100 * 1000000);

        *delta = (hi_s32)(ppm_expand & 0xffffffff);
    } else {
        ppm_expand = -1 * ppm;
        ppm_expand = ((ppm_expand * fbdiv) << 24) + ppm_expand * frac;
        ppm_expand = osal_div_u64(ppm_expand, 100 * 1000000);

        *delta = -1 * (hi_s32)(ppm_expand & 0xffffffff);
    }

    return;
}

static void sync_cal_target_freq(hi_u32 *fbdiv, hi_u32 *frac, hi_s32 delta)
{
    if (delta >= 0) {
        *frac = *frac + delta;

        if (*frac > 0xffffff) {
            *fbdiv = *fbdiv + 1;
            *frac  = *frac - 0x1000000;
        }
    } else {
        if (sync_abs(delta) > *frac) {
            *fbdiv = *fbdiv - 1;
            *frac  = *frac + 0x1000000 + delta;
        } else {
            *frac = *frac + delta;
        }
    }
}

static hi_s32 sync_cal_ppm(hi_u32 fbdiv, hi_u32 frac, hi_s32 delta, hi_s32 *ppm)
{
    hi_u64 delta_expand;
    hi_u64 frac_expand;

    /* PPM * 100 = (DeltaFracExpand * 100 * 1000000) / ((FracExpand << 24) + Frac) */
    frac_expand = fbdiv;
    frac_expand = (frac_expand << 24) + frac;

    /* do_div(HI_U64, hi_u32) */
    frac_expand = osal_div_u64(frac_expand, 10000);
    if (frac_expand == 0) {
        HI_ERR_SYNC("FracExpand is zero!\n");
        return HI_FAILURE;
    }

    if (delta >= 0) {
        delta_expand = delta;
        delta_expand = delta_expand * 10000;
        delta_expand = osal_div_u64(delta_expand, frac_expand);

        *ppm = (hi_s32)(delta_expand & 0xffffffff);
    } else {
        delta_expand = -1 * delta;
        delta_expand = delta_expand * 10000;
        delta_expand = osal_div_u64(delta_expand, frac_expand);

        *ppm = -1 * (hi_s32)(delta_expand & 0xffffffff);
    }

    return HI_SUCCESS;
}

static hi_s32 sync_tune_basic_pll_frq(sync_context *ctx, hi_s32 ppm)
{
    hi_s32 ret;
    hi_u32 fbdiv = 0;
    hi_u32 frac = 0;
    hi_u32 tune_fbdiv = 0;
    hi_u32 tune_frac = 0;
    hi_u32 pll_adjust;
    hi_u32 current_ppm = 0;
    hi_u32 delta_frc = 0;

    stc_hal_read_basic_pll_freq_ctrl_crg(&fbdiv, &frac);
    stc_hal_read_basic_pll_tune_freq_crg(&tune_fbdiv, &tune_frac);

    pll_adjust = (tune_fbdiv - fbdiv) * PLL_FBDIV_AMPLIFY_FACTOR + (tune_frac - frac);
    ret = sync_cal_ppm(fbdiv, frac, pll_adjust, &current_ppm);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("call sync_cal_ppm gailed\n");
        return ret;
    }

    if (sync_abs(current_ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        tune_fbdiv = fbdiv;
        tune_frac  = frac;
        current_ppm = 0;
        stc_hal_write_basic_pll_tune_freq_crg(tune_fbdiv, tune_frac);
        HI_WARN_SYNC("baisc fbdiv is changed !\n");
    }

    if (sync_abs(current_ppm + ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        sync_stc_basic_pll_reset();
        HI_ERR_SYNC("basic pll adjust too large and fail, PPM:%d \n", (current_ppm + ppm) / PPM_AMPLIFY_FACTOR);
        return HI_FAILURE;
    }

    sync_cal_delta_frac(tune_fbdiv, tune_frac, ppm, &delta_frc);
    sync_cal_target_freq(&tune_fbdiv, &tune_frac, delta_frc);

    if (stc_hal_is_basic_pll_tune_freq_busy()) {
        HI_WARN_SYNC("basic pll busy\n");
        return HI_FAILURE;
    }

    stc_hal_tune_basic_pll_freq(tune_fbdiv, tune_frac);

    HI_INFO_SYNC("basic org fbdiv:%-5d, frac:%d -> new fbdiv:%-5d, frac:%d\n", fbdiv, frac, tune_fbdiv, tune_frac);
    return HI_SUCCESS;
}

static hi_s32 sync_tune_aud_pll_frq(sync_context *ctx, hi_s32 ppm)
{
    hi_s32 ret;
    hi_u32 fbdiv;
    hi_u32 frac;
    hi_u32 tune_fbdiv = 0;
    hi_u32 tune_frac = 0;
    hi_u32 pll_adjust;
    hi_u32 current_ppm = 0;
    hi_u32 delta_frc = 0;

#ifdef AIAO_SRC_PLL_HAS_CORRECTED
    fbdiv = g_aud_pll_src_val_int & (0xfff);           /* 0xfff:int value is bite 0 ~ 11 */
    frac  = g_aud_pll_src_val_frac & (0xffffff);       /* 0xffffff:frac value is bite 0 ~ 23 */
#else
    stc_hal_read_aud_pll_freq_ctrl_crg(&fbdiv, &frac);
#endif

    stc_hal_read_aud_pll_tune_freq_crg(&tune_fbdiv, &tune_frac);

    pll_adjust = (tune_fbdiv - fbdiv) * PLL_FBDIV_AMPLIFY_FACTOR + (tune_frac - frac);
    ret = sync_cal_ppm(fbdiv, frac, pll_adjust, &current_ppm);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("call sync_cal_ppm gailed\n");
        return ret;
    }

    if (sync_abs(current_ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        tune_fbdiv = fbdiv;
        tune_frac  = frac;
        current_ppm = 0;
        stc_hal_write_aud_pll_tune_freq_crg(tune_fbdiv, tune_frac);
        HI_WARN_SYNC("aud fbdiv is changed !\n");
    }

    if (sync_abs(current_ppm + ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        sync_stc_aud_pll_reset();
        HI_ERR_SYNC("aud pll adjust too large and fail, PPM:%d \n", (current_ppm + ppm) / PPM_AMPLIFY_FACTOR);
        return HI_FAILURE;
    }

    sync_cal_delta_frac(tune_fbdiv, tune_frac, ppm, &delta_frc);
    sync_cal_target_freq(&tune_fbdiv, &tune_frac, delta_frc);

    if (stc_hal_is_aud_pll_tune_freq_busy()) {
        HI_WARN_SYNC("aud pll busy\n");
        return HI_FAILURE;
    }

    stc_hal_tune_aud_pll_freq(tune_fbdiv, tune_frac);

    HI_INFO_SYNC("aud org fbdiv:%-5d, frac:%d -> new fbdiv:%-5d, frac:%d\n", fbdiv, frac, tune_fbdiv, tune_frac);
    return HI_SUCCESS;
}

static hi_s32 sync_tune_vid_port0_pll_frq(sync_context *ctx, hi_s32 ppm)
{
    hi_s32 ret;
    hi_u32 fbdiv = 0;
    hi_u32 frac = 0;
    hi_u32 tune_fbdiv = 0;
    hi_u32 tune_frac = 0;
    hi_u32 pll_adjust;
    hi_u32 current_ppm = 0;
    hi_u32 delta_frc = 0;

    stc_hal_read_vid_port0_pll_freq_ctrl_crg(&fbdiv, &frac);
    stc_hal_read_vid_port0_pll_tune_freq_crg(&tune_fbdiv, &tune_frac);

    pll_adjust = (tune_fbdiv - fbdiv) * PLL_FBDIV_AMPLIFY_FACTOR + (tune_frac - frac);
    ret = sync_cal_ppm(fbdiv, frac, pll_adjust, &current_ppm);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("call sync_cal_ppm gailed\n");
        return ret;
    }

    if (sync_abs(current_ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        tune_fbdiv = fbdiv;
        tune_frac  = frac;
        current_ppm = 0;
        stc_hal_write_vid_port0_pll_tune_freq_crg(tune_fbdiv, tune_frac);
        HI_WARN_SYNC("vid_port0 fbdiv is changed !\n");
    }

    if (sync_abs(current_ppm + ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        sync_stc_vid_port0_pll_reset();
        HI_ERR_SYNC("vid_port0 pll adjust too large and fail, PPM:%d \n", (current_ppm + ppm) / PPM_AMPLIFY_FACTOR);
        return HI_FAILURE;
    }

    sync_cal_delta_frac(tune_fbdiv, tune_frac, ppm, &delta_frc);
    sync_cal_target_freq(&tune_fbdiv, &tune_frac, delta_frc);

    if (stc_hal_is_vid_port0_pll_tune_freq_busy()) {
        HI_WARN_SYNC("vid_port0 pll busy\n");
        return HI_FAILURE;
    }

    stc_hal_tune_vid_port0_pll_freq(tune_fbdiv, tune_frac);

    HI_INFO_SYNC("vid0 org fbdiv:%-5d, frac:%d -> new fbdiv:%-5d, frac:%d\n", fbdiv, frac, tune_fbdiv, tune_frac);
    return HI_SUCCESS;
}

static hi_s32 sync_tune_vid_port1_pll_frq(sync_context *ctx, hi_s32 ppm)
{
    hi_s32 ret;
    hi_u32 fbdiv = 0;
    hi_u32 frac = 0;
    hi_u32 tune_fbdiv = 0;
    hi_u32 tune_frac = 0;
    hi_u32 pll_adjust;
    hi_u32 current_ppm = 0;
    hi_u32 delta_frc = 0;

    stc_hal_read_vid_port1_pll_freq_ctrl_crg(&fbdiv, &frac);
    stc_hal_read_vid_port1_pll_tune_freq_crg(&tune_fbdiv, &tune_frac);

    pll_adjust = (tune_fbdiv - fbdiv) * PLL_FBDIV_AMPLIFY_FACTOR + (tune_frac - frac);
    ret = sync_cal_ppm(fbdiv, frac, pll_adjust, &current_ppm);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYNC("call sync_cal_ppm gailed\n");
        return ret;
    }

    if (sync_abs(current_ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        tune_fbdiv = fbdiv;
        tune_frac  = frac;
        current_ppm = 0;
        stc_hal_write_vid_port1_pll_tune_freq_crg(tune_fbdiv, tune_frac);
        HI_WARN_SYNC("vid_port1 fbdiv is changed !\n");
    }

    if (sync_abs(current_ppm + ppm) / PPM_AMPLIFY_FACTOR > STC_MAX_PPM) {
        sync_stc_vid_port1_pll_reset();
        HI_WARN_SYNC("vid_port0 pll adjust too large and fail, PPM:%d \n", (current_ppm + ppm) / PPM_AMPLIFY_FACTOR);
        return HI_FAILURE;
    }

    sync_cal_delta_frac(tune_fbdiv, tune_frac, ppm, &delta_frc);
    sync_cal_target_freq(&tune_fbdiv, &tune_frac, delta_frc);

    if (stc_hal_is_vid_port1_pll_tune_freq_busy()) {
        HI_WARN_SYNC("vid_port1 pll busy\n");
        return HI_FAILURE;
    }

    stc_hal_tune_vid_port1_pll_freq(tune_fbdiv, tune_frac);

    HI_INFO_SYNC("vid1 org fbdiv:%-5d, frac:%d -> new fbdiv:%-5d, frac:%d\n", fbdiv, frac, tune_fbdiv, tune_frac);
    return HI_SUCCESS;
}

static void sync_pll_tune_freq(sync_context *ctx, hi_s32 ppm)
{
    hi_s32 ret;

    if (osal_spin_trylock(&g_stc_lock) == 0) {
        return;
    }

    if (g_is_hpll_conf == HI_TRUE) {
        goto OUT;
    }

    if (sync_check_stc_adjust_fail_evt() == HI_TRUE) {
        goto OUT;
    }

    /* stc adjust prscision is 0.1(PPM) */
    if (sync_abs(ppm) < PPM_ADJUST_THRESHOLD) {
        goto OUT;
    }

    ret = sync_tune_aud_pll_frq(ctx, ppm);
    if (ret != HI_SUCCESS) {
        goto ERR;
    }

    ret = sync_tune_vid_port0_pll_frq(ctx, ppm);
    if (ret != HI_SUCCESS) {
        goto ERR;
    }

    ret = sync_tune_vid_port1_pll_frq(ctx, ppm);
    if (ret != HI_SUCCESS) {
        goto ERR;
    }

    ret = sync_tune_basic_pll_frq(ctx, ppm);
    if (ret != HI_SUCCESS) {
        goto ERR;
    }

ERR:
    if (ret != HI_SUCCESS) {
        sync_proc_stc_adjust_fail_evt();
    }

OUT:
    osal_spin_unlock(&g_stc_lock);
    return;
}

/* stc Adjust Policy */
/* select first pcr channel to adjust stc */
static void sync_stc_adjust_policy(sync_context *ctx, hi_s32 *ppm)
{
    hi_u32 i, j;
    sync_context *sync_ctx = HI_NULL;

    if (ctx->cur_status == SYNC_STATUS_STOP) {
        ctx->pcr_sync_info.is_stc_adjust_refer = HI_FALSE;

        *ppm = 0;
        return;
    }

    if (ctx->pcr_sync_info.is_stc_adjust_refer == HI_TRUE) {
        *ppm = ctx->pcr_sync_info.ppm_info.valid;
        return;
    }

    /* search refer channel */
    for (i = 0; i < SYNC_MAX_NUM; i++) {
        sync_ctx = sync_info_ctx_get(i);
        if (sync_ctx != HI_NULL && sync_ctx->attr.sync_ref == SYNC_REF_PCR &&
            sync_ctx->pcr_sync_info.pcr_adjust_type == SYNC_PCR_ADJUST_SCR &&
            sync_ctx->pcr_sync_info.is_pcr_first_come == HI_TRUE) {
            if (sync_ctx->cur_status != SYNC_STATUS_STOP) {
                break;
            }
        }
    }

    if (i >= SYNC_MAX_NUM) {
        *ppm = 0;
        return;
    }

    /* judge the current channel is refer channel or not */
    if (sync_ctx == ctx) {
        ctx->pcr_sync_info.is_stc_adjust_refer = HI_TRUE;

        for (j = 0; j < SYNC_MAX_NUM; j++) {
            sync_ctx = sync_info_ctx_get(j);
            if (sync_ctx != HI_NULL &&
                sync_ctx != ctx &&
                sync_ctx->attr.sync_ref == SYNC_REF_PCR &&
                sync_ctx->pcr_sync_info.is_stc_adjust_refer == HI_TRUE) {
                sync_ctx->pcr_sync_info.is_stc_adjust_refer = HI_FALSE;
            }
        }

        HI_INFO_SYNC("current stc adjust refer sync channel change \n");
    }

    *ppm = 0;
    return;
}

void sync_stc_adjust(sync_context *ctx, hi_u64 pcr, hi_u64 stc)
{
    hi_s32      ret;
    hi_s32      pcr_delta;
    hi_s32      stc_delta;

    if ((ctx->attr.sync_ref != SYNC_REF_PCR) ||
        (ctx->pcr_sync_info.pcr_adjust_type != SYNC_PCR_ADJUST_SCR) ||
        (ctx->pcr_sync_info.stc_info.is_stc_adjust_fail == HI_TRUE)) {
        return;
    }

    /* init stc adjust */
    if (ctx->pcr_sync_info.stc_info.is_stc_adjust_init != HI_TRUE) {
        if (sync_check_pll_tune_freq_enable() == HI_FALSE) {
            ctx->pcr_sync_info.stc_info.is_stc_adjust_fail = HI_TRUE;

            HI_WARN_SYNC("Pll Tune Freq Is disable !\n");
            return;
        }

        ret = sync_pll_tune_freq_crg_init();
        if (ret != HI_SUCCESS) {
            ctx->pcr_sync_info.stc_info.is_stc_adjust_fail = HI_TRUE;

            HI_ERR_SYNC("sync_pll_tune_freq_crg_init fail !\n");
            return;
        }

        ctx->pcr_sync_info.stc_info.stc_last_val      = stc;
        ctx->pcr_sync_info.pcr_last_val                = pcr;
        ctx->pcr_sync_info.ppm_info.num                = 0;
        ctx->pcr_sync_info.ppm_info.sum                = 0;
        ctx->pcr_sync_info.ppm_info.valid              = 0;
        ctx->pcr_sync_info.pcr_delta_num               = 0;
        ctx->pcr_sync_info.pcr_delta_sum               = 0;
        ctx->pcr_sync_info.stc_info.stc_delta_sum      = 0;
        ctx->pcr_sync_info.stc_info.is_stc_adjust_init = HI_TRUE;
        ctx->pcr_sync_info.is_stc_adjust_refer         = HI_FALSE;

        HI_INFO_SYNC("stc adjust init success\n");
        return ;
    }

    /* pcr or stc smaller than the last one */
    if ((pcr < ctx->pcr_sync_info.pcr_last_val) ||
        (stc < ctx->pcr_sync_info.stc_info.stc_last_val)) {
        ctx->pcr_sync_info.stc_info.stc_last_val = stc;
        ctx->pcr_sync_info.pcr_last_val          = pcr;

        HI_INFO_SYNC("pcr or stc smaller than the last one \n");
        return ;
    }

    /* pcr jump */
    if (pcr - ctx->pcr_sync_info.pcr_last_val > PCR_MAX_STEP) {
        ctx->pcr_sync_info.stc_info.stc_last_val = stc;
        ctx->pcr_sync_info.pcr_last_val         = pcr;

        HI_INFO_SYNC("PCR Jump !\n");
        return ;
    }

    stc_delta = stc - ctx->pcr_sync_info.stc_info.stc_last_val;
    pcr_delta = pcr - ctx->pcr_sync_info.pcr_last_val;

    ctx->pcr_sync_info.stc_info.stc_last_val = stc;
    ctx->pcr_sync_info.pcr_last_val         = pcr;

    /* refer stc_delta judge pcr_delta */
    if (sync_abs(pcr_delta - stc_delta) > PCR_STC_MAX_DELTA_DIFF) {
        HI_INFO_SYNC("PcrDelt is err, PcrDelt:%d, StcDelt:%d\n", pcr_delta, stc_delta);
        return ;
    }

    ctx->pcr_sync_info.pcr_delta_sum += pcr_delta;
    ctx->pcr_sync_info.stc_info.stc_delta_sum += stc_delta;
    ctx->pcr_sync_info.pcr_delta_num++;

    /* caculate ppm*100 */
    if ((ctx->pcr_sync_info.pcr_delta_num == PCR_DELTA_FILTER_NUMBER) &&
        (ctx->pcr_sync_info.pcr_delta_sum != 0)) {
        hi_u64 delta_expand;
        hi_s32 delta;
        hi_s32 ppm;

        delta = (ctx->pcr_sync_info.pcr_delta_sum - ctx->pcr_sync_info.stc_info.stc_delta_sum);

        if (delta > 0) {
            delta_expand = delta;
            delta_expand = delta_expand * 1000 * 1000 * 100;

            delta_expand = osal_div_u64(delta_expand, ctx->pcr_sync_info.pcr_delta_sum);
            ppm = (hi_s32)(delta_expand & 0xffffffff);
        } else {
            delta_expand = -1 * delta;
            delta_expand = delta_expand * 1000 * 1000 * 100;

            delta_expand = osal_div_u64(delta_expand, ctx->pcr_sync_info.pcr_delta_sum);
            ppm = -1 * (hi_s32)(delta_expand & 0xffffffff);
        }

        ctx->pcr_sync_info.pcr_delta_num      = 0;
        ctx->pcr_sync_info.pcr_delta_sum         = 0;
        ctx->pcr_sync_info.stc_info.stc_delta_sum = 0;

        if ((ppm / PPM_AMPLIFY_FACTOR) > STC_MAX_PPM) {
            HI_INFO_SYNC("PPM err, PPM:%d \n", (ppm / PPM_AMPLIFY_FACTOR));
            return;
        }

        ctx->pcr_sync_info.ppm_info.sum += ppm;
        ctx->pcr_sync_info.ppm_info.num++;
    }

    if (ctx->pcr_sync_info.ppm_info.num == PPM_FILTER_NUMBER) {
        hi_s32 ppm = 0;

        ctx->pcr_sync_info.ppm_info.valid = ctx->pcr_sync_info.ppm_info.sum / ctx->pcr_sync_info.ppm_info.num;
        sync_stc_adjust_policy(ctx, &ppm);
        sync_pll_tune_freq(ctx, ppm);

        ctx->pcr_sync_info.ppm_info.num = 0;
        ctx->pcr_sync_info.ppm_info.sum    = 0;

        HI_INFO_SYNC("current channel Valid PPM*100:%d, adjust STC PPM*100:%d\n",
            ctx->pcr_sync_info.ppm_info.valid, ppm);
    }
    return;
}


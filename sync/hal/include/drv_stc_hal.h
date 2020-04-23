/*
* Copyright (c) Hisilicon Tech. Co., Ltd. 20019-2019 All rights reserved.
* Description   : drv_stc_hal
* Author        : Hisilicon multimedia software group
* Create        : 2019/12/13
*/

#ifndef __DRV_STC__HAL_H__
#define __DRV_STC__HAL_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif
void stc_hal_read_basic_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_read_basic_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_write_basic_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val);
void stc_hal_tune_basic_pll_freq(hi_u32 fbdiv, hi_u32 frac);
void stc_hal_disable_tune_basic_pll_freq(void);
hi_bool stc_hal_is_basic_pll_tune_freq_enable(void);
hi_bool stc_hal_is_basic_pll_tune_freq_busy(void);

void stc_hal_read_aud_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_read_aud_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_write_aud_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val);
void stc_hal_tune_aud_pll_freq(hi_u32 fbdiv, hi_u32 frac);
void stc_hal_disable_tune_aud_pll_freq(void);
hi_bool stc_hal_is_aud_pll_tune_freq_enable(void);
hi_bool stc_hal_is_aud_pll_tune_freq_busy(void);

void stc_hal_read_vid_port0_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_read_vid_port0_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_write_vid_port0_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val);
void stc_hal_tune_vid_port0_pll_freq(hi_u32 fbdiv, hi_u32 frac);
void stc_hal_disable_tune_vid_port0_pll_freq(void);
hi_bool stc_hal_is_vid_port0_tune_freq_enable(void);
hi_bool stc_hal_is_vid_port0_pll_tune_freq_busy(void);

void stc_hal_read_vid_port1_pll_freq_ctrl_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_read_vid_port1_pll_tune_freq_crg(hi_u32 *int_val, hi_u32 *frac_val);
void stc_hal_write_vid_port1_pll_tune_freq_crg(hi_u32 int_val, hi_u32 frac_val);
void stc_hal_tune_vid_port1_pll_freq(hi_u32 fbdiv, hi_u32 frac);
void stc_hal_disable_tune_vid_port1_pll_freq(void);
hi_bool stc_hal_is_vid_port1_tune_freq_enable(void);
hi_bool stc_hal_is_vid_port1_pll_tune_freq_busy(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


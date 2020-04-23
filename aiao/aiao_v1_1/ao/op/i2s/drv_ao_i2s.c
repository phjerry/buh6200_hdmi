/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: i2s implement of ao driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "drv_ao_i2s.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_void ao_get_i2s_def_open_attr(hi_i2s_attr *attr)
{
    attr->master = HI_TRUE;
    attr->i2s_mode = HI_I2S_STD_MODE;
    attr->mclk = HI_I2S_MCLK_256_FS;
    attr->bclk = HI_I2S_BCLK_4_DIV;
    attr->channel = HI_I2S_CH_2;
    attr->bit_depth = HI_I2S_BIT_DEPTH_16;
    attr->pcm_sample_rise_edge = HI_TRUE;
    attr->pcm_delay_cycle = HI_I2S_PCM_1_DELAY;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


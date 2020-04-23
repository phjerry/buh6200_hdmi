/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio memory interface
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#ifndef __AUDIO_MEM_H__
#define __AUDIO_MEM_H__

#include "hi_drv_audio.h"

hi_s32  hi_drv_audio_mmz_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *mmz_buf);
hi_void hi_drv_audio_mmz_release(const hi_audio_buffer *mmz_buf);
hi_s32  hi_drv_audio_sec_mmz_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *mmz_buf);
hi_void hi_drv_audio_sec_mmz_release(const hi_audio_buffer *mmz_buf);

#ifndef HI_SND_DSP_SUPPORT
hi_s32  hi_drv_audio_smmu_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *smmu_buf);
hi_void hi_drv_audio_smmu_release(const hi_audio_buffer *smmu_buf);
hi_s32  hi_drv_audio_sec_smmu_alloc(const hi_char *name, hi_u32 size, hi_bool cache, hi_audio_buffer *smmu_buf);
hi_void hi_drv_audio_sec_smmu_release(const hi_audio_buffer *smmu_buf);
#else
#define hi_drv_audio_smmu_alloc hi_drv_audio_mmz_alloc
#define hi_drv_audio_smmu_release hi_drv_audio_mmz_release
#define hi_drv_audio_sec_smmu_alloc hi_drv_audio_sec_mmz_alloc
#define hi_drv_audio_sec_smmu_release hi_drv_audio_sec_mmz_release
#endif

#endif


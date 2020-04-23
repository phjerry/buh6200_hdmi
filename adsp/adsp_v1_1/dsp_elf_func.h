/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: adsp elf header file
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#ifndef __DSP_ELF_H__
#define __DSP_ELF_H__

#include "hi_drv_audio.h"

hi_s32 copy_elf_section(hi_uchar *elf_addr);
hi_s32 check_elf_paser(hi_uchar *elf_addr);
hi_s32 reset_elf_section(hi_u32 base_addr, hi_u32 size);

#endif

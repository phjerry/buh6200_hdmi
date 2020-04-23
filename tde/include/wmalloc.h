/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: mem manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_WMALLOC__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_WMALLOC__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 wmeminit(void);

hi_void wmemterm(void);

hi_u64 wgetphy(hi_void *ptr);

hi_void *wgetvrt(hi_u64 phyaddr);

hi_u32 wgetfreenum(hi_void);

hi_void wmemflush(hi_void);

#ifndef TDE_BOOT
struct seq_file *wprintinfo(struct seq_file *page);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_WMALLOC__ */

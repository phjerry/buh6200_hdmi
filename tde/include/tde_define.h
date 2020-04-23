/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: define manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_DEFINE__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_DEFINE__

#ifndef HI_BUILD_IN_BOOT
#include "hi_osal.h"
#include "hi_gfx_comm_k.h"
#else
#include "exports.h"
#include "hi_common.h"
#include "hi_gfx_mem.h"
#endif

#include "wmalloc.h"
#include "tde_adp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    TDE_COLORFMT_CATEGORY_ARGB,
    TDE_COLORFMT_CATEGORY_CLUT,
    TDE_COLORFMT_CATEGORY_An,
    TDE_COLORFMT_CATEGORY_YCbCr,
    TDE_COLORFMT_CATEGORY_BYTE,
    TDE_COLORFMT_CATEGORY_HALFWORD,
    TDE_COLORFMT_CATEGORY_MB,
    TDE_COLORFMT_CATEGORY_MAX
} tde_color_fmt_category;

/* Node submit type, Independant operation node, Operated head node, Operated middle node */
typedef enum {
    TDE_NODE_SUBM_ALONE = 0, /* current node submit as independant operated node */
    TDE_NODE_SUBM_PARENT = 1,
    /* current node submit as operated parent node */                        /* CNcomment: 分块的第一个节点 */
    TDE_NODE_SUBM_CHILD = 2 /* current node submit as operated child node */ /* CNcomment: 分块的子节点 */
} tde_node_submit_type;

/* Operation setting information node */
typedef struct {
    hi_void *vir_buf;    /* setting information node buffer */
    hi_u32 node_size;    /* current node occupied size, united in byte */
    hi_u64 up_data_flag; /* current node update flag */
    hi_u64 phy_addr;     /* current node addr in hardware list */
} tde_node_surface;

/* Notified mode  after current node's job end */
typedef enum {
    TDE_JOB_NONE_NOTIFY = 0,
    TDE_JOB_COMPL_NOTIFY,
    TDE_JOB_WAKE_NOTIFY,
    TDE_JOB_NOTIFY_MAX
} tde_notify_mode;

#define STATIC static
#define INLINE inline
#define TDE_MAX_WAIT_TIMEOUT 2000

#ifndef HI_BUILD_IN_BOOT
extern hi_u32 g_tde_tmp_buffer_size;
#endif

#ifndef HI_BUILD_IN_BOOT
#define TDE_CACH_STRATEGY
#endif

hi_void *tde_malloc(hi_u32 size);
hi_void tde_free(hi_void *ptr);

#ifndef HI_BUILD_IN_BOOT
#define tde_lock(lock, lockflags) osal_spin_lock_irqsave(lock, &lockflags)
#define tde_unlock(lock, lockflags) osal_spin_unlock_irqrestore(lock, &lockflags)
#else
#define tde_lock(lock, lockflags)
#define tde_unlock(lock, lockflags)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_DEFINE__ */

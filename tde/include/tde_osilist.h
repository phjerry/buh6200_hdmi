/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: osilist manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef __SOURCE_MSP_DRV_TDE_INCLUDE_OSILIST__
#define __SOURCE_MSP_DRV_TDE_INCLUDE_OSILIST__

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_list.h"
#endif
#include "hi_osal.h"
#include "osal_list.h"
#include "tde_define.h"
#include "drv_tde_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Data struct of software list node */
typedef struct {
#ifndef HI_BUILD_IN_BOOT
    struct osal_list_head list_head;
#else
    struct list_head list_head;
#endif
    hi_s32 handle; /* Job handle of the instruct */
    /* Instruct serial number in job, form one on start, the same number is the same instruct */
    hi_s32 index;
    tde_node_submit_type submit_type; /* current node type */
    tde_node_surface node_buf;        /* Node of operate config */
    hi_u32 phy_buf_num;               /* Number of physical buffer distributed */
} tde_sw_node;

/* Job definition */
typedef struct {
#ifndef HI_BUILD_IN_BOOT
    struct osal_list_head list_head;
#else
    struct list_head list_head;
#endif
    hi_s32 handle;                                /* Job handle */
    drv_tde_func_callback func_complete_callback; /* Pointer of callback fuction */
    hi_void *func_para;                           /* Arguments of callback function */
    tde_notify_mode notify_type;                  /* Notice type after node completed */
    hi_u32 cmd_num;                               /* Instruct number of job */
    hi_u32 node_num;                              /* Node number of job */
    tde_sw_node *first_cmd;                       /* Software node of first instruct in job */
    tde_sw_node *last_cmd;                        /* Software node of last instruct in job */
    tde_sw_node *tail_node;                       /* Last software node of job */
#ifndef HI_BUILD_IN_BOOT
    osal_wait query; /* Wait queue used in query */
#endif
    hi_bool has_submitted;     /* If have submitted */
    hi_bool aq_use_buf;        /* If using temporary buffer */
    hi_u8 wait_for_done_count; /* wait job count */
    hi_bool is_wait;
    hi_void *private_data;
} tde_sw_job;

hi_s32 tde_list_init(hi_void);

hi_void tde_list_term(hi_void);

hi_s32 tde_list_beg_job(hi_s32 *pHandle, hi_void *private_data);

hi_s32 tde_list_cancel_job(hi_s32 handle);

hi_s32 tde_list_submit_job(hi_s32 handle, hi_u32 time_out, drv_tde_func_callback func_complete_callback,
                           hi_void *func_para, tde_notify_mode notify_type);

#ifndef HI_BUILD_IN_BOOT
hi_s32 tde_list_wait_all_done(hi_void);

hi_void tde_list_reset(hi_void);

hi_s32 tde_list_wait_for_done(hi_s32 handle, hi_u32 time_out);

hi_void tde_list_comp_proc(hi_void);

#ifdef TDE_HWC_COOPERATE
hi_void tde_list_comp(hi_void);
#endif
hi_void tde_list_node_comp(hi_void);
#endif

hi_u32 tde_list_get_phy_buf(hi_u32 cbcr_offset, hi_u32 buffer_size);

hi_void tde_list_put_phy_buf(hi_u32 buf_num);

hi_void tde_list_free_serial_cmd(tde_sw_node *first_cmd, tde_sw_node *last_cmd);

hi_s32 drv_tde_lock_working_flag(hi_size_t *lock);

hi_s32 drv_tde_unlock_working_flag(hi_size_t *lock);

hi_s32 drv_tde_get_working_flag(hi_bool *bFlag);

hi_s32 drv_tde_set_working_flag(hi_bool bFlag);

#ifndef HI_BUILD_IN_BOOT
hi_void tde_list_free_pending_job(hi_void *private_data);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOURCE_MSP_DRV_TDE_INCLUDE_OSILIST__ */

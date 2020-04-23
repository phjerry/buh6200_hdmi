/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: osilist manage
 * Author: sdk
 * Create: 2019-03-18
 */

#ifndef HI_BUILD_IN_BOOT
#include "hi_osal.h"
#else
#include "exports.h"
#endif

#ifdef HI_BUILD_IN_MINI_BOOT
#include "delay.h"
#endif

#include "tde_define.h"
#include "tde_handle.h"
#include "tde_buffer.h"
#include "tde_osilist.h"
#include "tde_hal.h"
#include "wmalloc.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_comm.h"
#include "hi_gfx_debug.h"
#include "hi_errno.h"
#else
#include "tde_debug.h"
#include "hi_errno.h"
#endif
/* **************************** Macro Definition ******************************************* */

/* ************************** Enum Definition ********************************************** */

/* ************************** Structure Definition ***************************************** */

/* JOB LIST head node definition */
typedef struct {
    struct osal_list_head list_head;
    hi_s32 s32HandleLast;     /* job handle wait for last submit */
    hi_s32 s32HandleFinished; /* job handle last completed */
    hi_u32 u32JobNum;         /* job number in queue */
#ifndef HI_BUILD_IN_BOOT
    osal_spinlock lock;
#endif
    tde_sw_job *pstJobCommitted; /* last submited job node pointer, which is the first job */
    tde_sw_job *pstJobToCommit;  /* job node pointer wait for submit,which is the first job */
    hi_u64 u32JobFinished;
    tde_sw_job *pstJobLast; /* last job in the list */
} TDE_SWJOBLIST_S;

#ifndef HI_BUILD_IN_BOOT
typedef hi_void (*TDE_WQ_CB)(hi_u32);

typedef struct {
    hi_u32 Count;
    TDE_WQ_CB pWQCB;
    osal_workqueue work;
} TDEFREEWQ_S;

/****************************************************************************/
/* TDE osi list inner variable definition */
/****************************************************************************/
STATIC osal_wait s_TdeBlockJobWq; /* wait queue used to block */
#endif
STATIC TDE_SWJOBLIST_S *s_pstTDEOsiJobList; /* global job list queue */

#ifndef HI_BUILD_IN_BOOT
STATIC hi_bool gs_bWorkingFlag = 0;
#endif

#ifdef TDE_HWC_COOPERATE
osal_spinlock s_WorkingFlagLock;
#endif

/****************************************************************************/
/* TDE osi list inner interface definition */
/****************************************************************************/
#ifndef HI_BUILD_IN_BOOT
STATIC hi_void TdeOsiListDoFreePhyBuff(hi_u32 buf_num);
#endif

STATIC INLINE hi_void TdeOsiListSafeDestroyJob(tde_sw_job *pstJob);
STATIC hi_void TdeOsiListReleaseHandle(tde_handle_mgr *pstJobHeader);
STATIC INLINE hi_void TdeOsiListAddJob(tde_sw_job *pstJob);
STATIC hi_void TdeOsiListDestroyJob(tde_sw_job *pstJob);

/* ****************************** API declaration ****************************************** */

/* ****************************** API release ********************************************** */

/*****************************************************************************
 * Function:      TdeOsiListSafeDestroyJob
 * Description:   release node from FstCmd to LastCmd
 * Input:         pstJob:delete job list
 * Output:        none
 * Return:        none
 * Others:
 *****************************************************************************/
STATIC INLINE hi_void TdeOsiListSafeDestroyJob(tde_sw_job *pstJob)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_BUILD_IN_BOOT
    /* if user query this job, release job in query function */
    if (pstJob->wait_for_done_count != 0) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstJob->handle);
        pstJob->notify_type = TDE_JOB_NOTIFY_MAX;
        osal_wait_wakeup(&pstJob->query);
    } else
#endif
    {
        TdeOsiListDestroyJob(pstJob);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      TdeOsiListReleaseHandle
 * Description:   release handle manage info
 * Input:         pstJobHeader:handle manage struct
 * Output:        none
 * Return:        none
 * Others:
 *****************************************************************************/
STATIC hi_void TdeOsiListReleaseHandle(tde_handle_mgr *pstJobHeader)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (pstJobHeader != HI_NULL) {
        if (release_handle(pstJobHeader->handle)) {
            tde_free(pstJobHeader);
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      TdeOsiListDoFreePhyBuff
 * Description:    free temporary buffer
 * Input:         buf_num: the number of temporary buffer
 * Output:        none
 * Return:        none
 * Others:
 *****************************************************************************/
STATIC hi_void TdeOsiListDoFreePhyBuff(hi_u32 buf_num)
{
    hi_u32 i = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    for (i = 0; i < buf_num; i++) {
        tde_free_physic_buffer();
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_s32 tde_init_tde_lock(hi_void)
{
#ifndef HI_BUILD_IN_BOOT
    if (osal_spin_lock_init(&s_pstTDEOsiJobList->lock) != 0) {
        s_pstTDEOsiJobList->lock.lock = HI_NULL;
        return HI_FAILURE;
    }
    if (osal_spin_lock_init(&g_tde_buf_lock) != 0) {
        g_tde_buf_lock.lock = HI_NULL;
        return HI_FAILURE;
    }
#ifdef TDE_HWC_COOPERATE
    if (osal_spin_lock_init(&s_WorkingFlagLock) != 0) {
        s_WorkingFlagLock.lock = HI_NULL;
        return HI_FAILURE;
    }
#endif
#endif
    return HI_SUCCESS;
}

static hi_void tde_deinit_tde_lock(hi_void)
{
#ifndef HI_BUILD_IN_BOOT
    if (s_pstTDEOsiJobList->lock.lock != HI_NULL) {
        osal_spin_lock_destory(&s_pstTDEOsiJobList->lock);
    }
    if (g_tde_buf_lock.lock != HI_NULL) {
        osal_spin_lock_destory(&g_tde_buf_lock);
    }
#ifdef TDE_HWC_COOPERATE
    if (s_WorkingFlagLock.lock != HI_NULL) {
        osal_spin_lock_destory(&s_WorkingFlagLock);
    }
#endif
#endif
}

/*****************************************************************************
 Prototype    : tde_list_init
 Description  : initialize list manage  module
 Input        : hi_void
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
hi_s32 tde_list_init(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_BUILD_IN_BOOT
    if (osal_wait_init(&s_TdeBlockJobWq) != 0) {
        return HI_FAILURE;
    }
#endif
    if (!initial_handle()) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, initial_handle, FAILURE_TAG);
        return HI_FAILURE;
    }

    s_pstTDEOsiJobList = (TDE_SWJOBLIST_S *)tde_malloc(sizeof(TDE_SWJOBLIST_S));
    if (s_pstTDEOsiJobList == HI_NULL) {
        destroy_handle();
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_FAILURE;
    }

    OSAL_INIT_LIST_HEAD(&s_pstTDEOsiJobList->list_head);

    if (tde_init_tde_lock() != HI_SUCCESS) {
        tde_deinit_tde_lock();
        destroy_handle();
        tde_free(s_pstTDEOsiJobList);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_init_tde_lock, FAILURE_TAG);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : TdeOsiListFreeNode
 Description  : release node
 Input        : hardware_node: node pointer
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
STATIC INLINE hi_void TdeOsiListFreeNode(tde_sw_node *hardware_node)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);

    osal_list_del_init(&hardware_node->list_head);
    if (hardware_node->node_buf.vir_buf != HI_NULL) {
        tde_free(hardware_node->node_buf.vir_buf);
    }

    tde_list_put_phy_buf(hardware_node->phy_buf_num);

    tde_free(hardware_node);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      tde_list_free_serial_cmd
 * Description:   release from FstCmd to LastCmd
 * Input:         first_cmd: first node
 *                last_cmd:last node
 * Output:        none
 * Return:        none
 * Others:
 *****************************************************************************/
hi_void tde_list_free_serial_cmd(tde_sw_node *first_cmd, tde_sw_node *last_cmd)
{
    tde_sw_node *pstNextCmd = HI_NULL;
    tde_sw_node *pstCurCmd = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((first_cmd == HI_NULL) || (last_cmd == HI_NULL)) {
        return;
    }

    pstCurCmd = pstNextCmd = first_cmd;

    while (pstNextCmd != last_cmd) {
        pstNextCmd = osal_list_entry(pstCurCmd->list_head.next, tde_sw_node, list_head);

        TdeOsiListFreeNode(pstCurCmd);

        if (pstNextCmd == HI_NULL) {
            break;
        }

        pstCurCmd = pstNextCmd;
    }

    TdeOsiListFreeNode(last_cmd);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 Prototype    : tde_list_term
 Description  : deinitialization of list manager module
 Input        : hi_void
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
hi_void tde_list_term(hi_void)
{
    tde_sw_job *pstJob = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(s_pstTDEOsiJobList);

    while (!osal_list_empty(&s_pstTDEOsiJobList->list_head)) {
        pstJob = osal_list_entry(s_pstTDEOsiJobList->list_head.next, tde_sw_job, list_head);
        osal_list_del_init(&pstJob->list_head);
        TdeOsiListDestroyJob(pstJob);
    }

#ifndef HI_BUILD_IN_BOOT
    osal_spin_lock_destory(&s_pstTDEOsiJobList->lock);
    osal_spin_lock_destory(&g_tde_buf_lock);
#ifdef TDE_HWC_COOPERATE
    osal_spin_lock_destory(&s_WorkingFlagLock);
#endif
#endif
    osal_wait_destroy(&s_TdeBlockJobWq);
    tde_free(s_pstTDEOsiJobList);
    s_pstTDEOsiJobList = HI_NULL;

    destroy_handle();

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void TdeOsiListFlushNode(tde_sw_node *hardware_node)
{
    tde_node_surface *pNodeBufInfo = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);

    pNodeBufInfo = &hardware_node->node_buf;

    if (pNodeBufInfo->vir_buf != HI_NULL) {
        wmemflush();
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void TdeOsiListFlushJob(tde_sw_job *pstJob)
{
    tde_sw_node *pstNextCmd = HI_NULL;
    tde_sw_node *pstCurCmd = HI_NULL;
    tde_sw_node *first_cmd = HI_NULL;
    tde_sw_node *last_cmd = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstJob);
    first_cmd = pstJob->first_cmd;
    last_cmd = pstJob->tail_node;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(first_cmd);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(last_cmd);

    pstCurCmd = pstNextCmd = first_cmd;
    while (pstNextCmd != last_cmd) {
        pstNextCmd = osal_list_entry(pstCurCmd->list_head.next, tde_sw_node, list_head);
        TdeOsiListFlushNode(pstCurCmd);
        pstCurCmd = pstNextCmd;
    }

    TdeOsiListFlushNode(last_cmd);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

/*****************************************************************************
 * Function:      TdeOsiListAddJob
 * Description:   add task info to task list
 * Input:         pstJob: job struct
 * Output:        none
 * Return:        none
 * Others:
 *****************************************************************************/
STATIC INLINE hi_void TdeOsiListAddJob(tde_sw_job *pstJob)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    osal_list_add_tail(&pstJob->list_head, &s_pstTDEOsiJobList->list_head);
    s_pstTDEOsiJobList->u32JobNum++;
    s_pstTDEOsiJobList->s32HandleLast = pstJob->handle;
    s_pstTDEOsiJobList->pstJobLast = pstJob;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

/*****************************************************************************
 Prototype    : tde_list_beg_job
 Description  : create a job
 Input        : NONE
 Output       : pHandle: created job handle
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
hi_s32 tde_list_beg_job(hi_s32 *pHandle, hi_void *private_data)
{
    tde_handle_mgr *pHandleMgr = HI_NULL;
    tde_sw_job *pstJob = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pHandle, HI_FAILURE);

    pHandleMgr = (tde_handle_mgr *)tde_malloc(sizeof(tde_handle_mgr) + sizeof(tde_sw_job));
    if (pHandleMgr == HI_NULL) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    pstJob = (tde_sw_job *)((hi_u8 *)pHandleMgr + sizeof(tde_handle_mgr));
    pHandleMgr->res = (hi_void *)pstJob;

    get_handle(pHandleMgr, pHandle);
    if (*pHandle < 0) {
        tde_free(pHandleMgr);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, get_handle, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    OSAL_INIT_LIST_HEAD(&pstJob->list_head);

#ifndef HI_BUILD_IN_BOOT
    if (osal_wait_init(&pstJob->query) != 0) {
        return HI_ERR_TDE_NO_MEM;
    }
#endif

    pstJob->handle = *pHandle;
    if (private_data != HI_NULL) {
        pstJob->private_data = private_data;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : TdeOsiListDestroyJob
 Description  : destroy a job
 Input        : handle: job handle
 Output       : NONE
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
STATIC hi_void TdeOsiListDestroyJob(tde_sw_job *pstJob)
{
    tde_handle_mgr *pHandleMgr = HI_NULL;
#ifdef CONFIG_GFX_MMU_SUPPORT
    tde_hardware_node *hardware_node = HI_NULL;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (!query_handle(pstJob->handle, &pHandleMgr)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstJob->handle);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, query_handle, FAILURE_TAG);
        return;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    hardware_node = (tde_hardware_node *)(pstJob->first_cmd->node_buf.vir_buf + TDE_NODE_HEAD_BYTE);
    tde_hal_free_tmp_buf(hardware_node);
#endif

    tde_list_free_serial_cmd(pstJob->first_cmd, pstJob->tail_node);
    osal_wait_destroy(&pstJob->query);

    TdeOsiListReleaseHandle(pHandleMgr);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      tde_list_cancel_job
 * Description:   cancel task
 * Input:         handle:task handle
 * Output:        none
 * Return:        =0,success <0,error
 * Others:
 *****************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_s32 tde_list_cancel_job(hi_s32 handle)
{
    tde_handle_mgr *pHandleMgr = HI_NULL;
    tde_sw_job *pstJob = HI_NULL;
    hi_size_t lockflags = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
    if (!query_handle(handle, &pHandleMgr)) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
    pstJob = (tde_sw_job *)pHandleMgr->res;

    if (pstJob->has_submitted) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, handle);
        return HI_FAILURE;
    }

    tde_list_free_serial_cmd(pstJob->first_cmd, pstJob->tail_node);
    TdeOsiListReleaseHandle(pHandleMgr);
    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#else
/*****************************************************************************
 * Function:      TdeOsiListWaitTdeIdle
 * Description:   wait for completion of the task
 * Input:         ?T
 * Output:        ?T
 * Return:        True: Idle/False: Busy
 * Others:        ?T
 *****************************************************************************/
STATIC hi_void TdeOsiListWaitTdeIdle(hi_void)
{
    hi_ulong ticks_start = 0;
    hi_ulong ticks_now = 0;
    hi_u32 diff_ms = 0;
    hi_ulong time_sec = 0;
    hi_u16 time_msec = 0;
    hi_u16 time_usec = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    do {
        osal_udelay(10 * 1000); /* wait for 10 * 1000 us */

        ticks_now = do_gettime(&time_sec, &time_msec, &time_usec);
        diff_ms = (hi_u32)((ticks_now - ticks_start) / 1000); /* 1ms equals 1000 us */

        if (tde_hal_ctl_is_idle_safely()) {
            tde_hal_ctl_int_status();
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return;
        }
    } while (diff_ms < 3000); /* if less than 3000 ms */

    if (diff_ms >= 3000) { /* if more than 3000 ms */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, diff_ms);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "tde work timeout");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}
#endif

hi_s32 tde_check_notify_type(const hi_void *param)
{
    tde_sw_job *pstJob = (tde_sw_job *)param;

    if (pstJob == HI_NULL) {
        return 0;
    }
    if (pstJob->notify_type == TDE_JOB_NOTIFY_MAX) {
        return 1;
    }
    return 0;
}

/*****************************************************************************
 Prototype    : tde_list_submit_job
 Description  : when submit job handle by user, at first add job list to global list, and then handle with according by
different situation 1.when TDE is free and no command canbe added,evaluate waited node pointer,start to software list
node to hardware 2.when TDE is not free and no command canbe added, evaluate waited node pointer 3.when TDE is not free
but command canbe added,no handle Input        : handle: job handle pSwNode: node resource submit_type: node type Output
: NONE Return Value : Calls        : Called By    :
*****************************************************************************/
hi_s32 tde_list_submit_job(hi_s32 handle, hi_u32 time_out, drv_tde_func_callback func_complete_callback,
                           hi_void *func_para, tde_notify_mode notify_type)
{
    tde_sw_job *pstJob = HI_NULL;
    tde_handle_mgr *pHandleMgr = HI_NULL;
    hi_bool bValid = HI_FALSE;
    tde_sw_node *tail_node = HI_NULL;
    hi_s32 s32Ret = HI_SUCCESS;
    hi_u8 *hardware_node = HI_NULL;
#ifndef HI_BUILD_IN_BOOT
    hi_size_t lockflags = 0;
#ifdef TDE_HWC_COOPERATE
    hi_size_t lock = 0;
#endif
    hi_bool asynflag = 0;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((0 == time_out) || (time_out > 20000)) { /* if timeout more than 20000 jiffies */
        time_out = 20000;                        /* if timeout equals 20000 jiffies */
    }

    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
    bValid = query_handle(handle, &pHandleMgr);
    if (!bValid) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, handle);
        return HI_ERR_TDE_INVALID_HANDLE;
    }

    pstJob = (tde_sw_job *)pHandleMgr->res;
    if (pstJob->has_submitted) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, handle);
        return HI_ERR_TDE_INVALID_HANDLE;
    }

    if (pstJob->first_cmd == HI_NULL) {
        TdeOsiListReleaseHandle(pHandleMgr);
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    tail_node = pstJob->tail_node;
    if (tail_node == HI_NULL) {
        TdeOsiListReleaseHandle(pHandleMgr);
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    hardware_node = (hi_u8 *)tail_node->node_buf.vir_buf + TDE_NODE_HEAD_BYTE;
    tde_hal_node_enable_complete_int((hi_void *)hardware_node);

    pstJob->has_submitted = HI_TRUE;
    pstJob->notify_type = notify_type;
    pstJob->func_complete_callback = func_complete_callback;
    pstJob->func_para = func_para;

    /* If the job to commit is not null,join the current job to the tail node of the last job. */
    if (HI_NULL != s_pstTDEOsiJobList->pstJobToCommit) {
        tde_sw_node *pstTailNodeInJobList = s_pstTDEOsiJobList->pstJobLast->tail_node;
        hi_u32 *pNextNodeAddr = (hi_u32 *)pstTailNodeInJobList->node_buf.vir_buf + (TDE_NODE_HEAD_BYTE >> 2) +
                                ((pstTailNodeInJobList->node_buf.node_size) >> 2); /* rshift by 2 */

        hi_u64 *pNextNodeUpdate = (hi_u64 *)(pNextNodeAddr + 1);

        *pNextNodeAddr = pstJob->first_cmd->node_buf.phy_addr;
        *pNextNodeUpdate = pstJob->first_cmd->node_buf.up_data_flag;

        if (pstJob->aq_use_buf) {
            s_pstTDEOsiJobList->pstJobToCommit->aq_use_buf = HI_TRUE;
        }
#ifdef TDE_CACH_STRATEGY
        /* 将job中的对应的所有 hw 节点flush到内存，保证硬件能正确访问 */
        TdeOsiListFlushJob(s_pstTDEOsiJobList->pstJobLast);
#endif
    } else {
        s_pstTDEOsiJobList->pstJobToCommit = pstJob;
    }

#ifdef TDE_CACH_STRATEGY
    /* 将job中的对应的所有 hw 节点flush到内存，保证硬件能正确访问 */
    TdeOsiListFlushJob(pstJob);
#endif

    TdeOsiListAddJob(pstJob);

#ifndef HI_BUILD_IN_BOOT
    if (TDE_JOB_WAKE_NOTIFY != notify_type) {
        if ((!osal_in_interrupt()) && (wgetfreenum() < 5)) { /* if free units less than 5 */
            pstJob->notify_type = TDE_JOB_WAKE_NOTIFY;
            notify_type = TDE_JOB_WAKE_NOTIFY;
            time_out = 1000; /* timeout equals 1000 jiffies */
            asynflag = 1;    /* 由非阻塞方式转为阻塞方式标志位 */
        }
    }
#endif

#ifdef TDE_HWC_COOPERATE
    drv_tde_lock_working_flag(&lock);
    if (!gs_bWorkingFlag)
#endif
    {
        s32Ret = tde_hal_node_execute(s_pstTDEOsiJobList->pstJobToCommit->first_cmd->node_buf.phy_addr,
                                      s_pstTDEOsiJobList->pstJobToCommit->first_cmd->node_buf.up_data_flag,
                                      s_pstTDEOsiJobList->pstJobToCommit->aq_use_buf);
        if (s32Ret == HI_SUCCESS) {
            s_pstTDEOsiJobList->pstJobCommitted = s_pstTDEOsiJobList->pstJobToCommit;
            s_pstTDEOsiJobList->pstJobToCommit = HI_NULL;
            s_pstTDEOsiJobList->u32JobFinished = 0x0;
#ifdef TDE_HWC_COOPERATE
            gs_bWorkingFlag = 1;
#endif
        }
    }
#ifdef TDE_HWC_COOPERATE
    drv_tde_unlock_working_flag(&lock);
#endif

#ifndef HI_BUILD_IN_BOOT
    if (TDE_JOB_WAKE_NOTIFY == notify_type) {
        pstJob->wait_for_done_count++;
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

        s32Ret = osal_wait_timeout_interruptible(&s_TdeBlockJobWq, tde_check_notify_type, (hi_void *)pstJob, time_out);

        tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
        pstJob->wait_for_done_count--;

        if (TDE_JOB_NOTIFY_MAX == pstJob->notify_type) {
            if (pstJob->wait_for_done_count == 0) {
                TdeOsiListDestroyJob(pstJob);
            }
            tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        } else {
            pstJob->notify_type = TDE_JOB_COMPL_NOTIFY;
            tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
            if ((-ERESTARTSYS) == s32Ret) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstJob->handle);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_wait_timeout_interruptible, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "handle interrupt");
                return HI_ERR_TDE_INTERRUPT;
            }
            if (1 == asynflag) {
                /* 如果由非阻塞方式转为阻塞方式则无超时信息 */
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return HI_SUCCESS;
            }
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstJob->handle);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_wait_timeout_interruptible, FAILURE_TAG);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "handle timeout");
            return HI_ERR_TDE_JOB_TIMEOUT;
        }
    }
#else
    /* to do:等待TDE任务完成 */
    TdeOsiListWaitTdeIdle();
    /* 从链表中删除该job节点,防止再次遍历到该节点 */
    osal_list_del_init(&pstJob->list_head);
    /* 释放job */
    TdeOsiListSafeDestroyJob(pstJob);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstJob->handle);
#endif

    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 Prototype    : tde_list_wait_all_done
 Description  : wait for all TDE operate is completed,that is to say wait for the last job to be completed.
 Input        : none
 Output       : NONE
 Return Value : HI_SUCCESS,TDE operate completed
 Calls        :
 Called By    :
*****************************************************************************/
hi_s32 tde_list_wait_all_done(hi_void)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_s32 s32WaitHandle = -1;
    hi_size_t lockflags = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
    s32WaitHandle = s_pstTDEOsiJobList->s32HandleLast;
    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
    if (-1 == s32WaitHandle) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    Ret = tde_list_wait_for_done(s32WaitHandle, 10000); /* wait for 10000 jiffies */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return Ret;
}

/*****************************************************************************
 * Function:      tde_list_wait_for_done
 * Description:   block to wait for job done
 * Input:         handle: job handle
 *                time_out: timeout value
 * Output:        none
 * Return:        =0,successfully completed <0,error
 * Others:
 *****************************************************************************/
hi_s32 tde_list_wait_for_done(hi_s32 handle, hi_u32 time_out)
{
    tde_sw_job *pstJob = HI_NULL;
    tde_handle_mgr *pHandleMgr = HI_NULL;
    hi_s32 s32Ret = HI_SUCCESS;
    hi_bool bValid = HI_FALSE;
    hi_size_t lockflags = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((0 == time_out) || (time_out > 20000)) { /* if more than 20000 jiffies */
        time_out = 20000;                        /* timeout at most 20000 jiffies */
    }

    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
    bValid = query_handle(handle, &pHandleMgr);
    if (!bValid) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }
    pstJob = (tde_sw_job *)pHandleMgr->res;
    if (!pstJob->has_submitted) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, handle);
        return HI_ERR_TDE_INVALID_HANDLE;
    }
    pstJob->is_wait = HI_TRUE;
    pstJob->wait_for_done_count++;

    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

    s32Ret = osal_wait_timeout_interruptible(&(pstJob->query), tde_check_notify_type, (hi_void *)pstJob, time_out);
    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
    pstJob->wait_for_done_count--;

    if (TDE_JOB_NOTIFY_MAX != pstJob->notify_type) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        if ((-ERESTARTSYS) == s32Ret) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstJob->handle);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_wait_timeout_interruptible, FAILURE_TAG);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "handle interrupt");
            return HI_ERR_TDE_INTERRUPT;
        } else {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstJob->handle);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_wait_timeout_interruptible, FAILURE_TAG);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "handle timeout");
        }
        return HI_ERR_TDE_QUERY_TIMEOUT;
    }

    /* complete */
    if (pstJob->wait_for_done_count == 0) {
        TdeOsiListDestroyJob(pstJob);
    }

    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
 Prototype    : tde_list_comp_proc
 Description  : list complete interrupt servic, mainly complete switch on hardware lists
 Input        : hi_void
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
hi_void tde_list_comp_proc()
{
    hi_s32 s32Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (s_pstTDEOsiJobList->pstJobToCommit != HI_NULL) {
        s32Ret = tde_hal_node_execute(s_pstTDEOsiJobList->pstJobToCommit->first_cmd->node_buf.phy_addr,
                                      s_pstTDEOsiJobList->pstJobToCommit->first_cmd->node_buf.up_data_flag,
                                      s_pstTDEOsiJobList->pstJobToCommit->aq_use_buf);
        if (s32Ret == HI_SUCCESS) {
            s_pstTDEOsiJobList->pstJobCommitted = s_pstTDEOsiJobList->pstJobToCommit;
            s_pstTDEOsiJobList->pstJobToCommit = HI_NULL;
            s_pstTDEOsiJobList->u32JobFinished = 0x0;
#ifdef TDE_HWC_COOPERATE
            gs_bWorkingFlag = 1;
#endif
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

#ifdef TDE_HWC_COOPERATE
hi_void tde_list_comp()
{
    hi_size_t lockflags = 0;
    hi_size_t lock = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (s_pstTDEOsiJobList == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, s_pstTDEOsiJobList);
        return;
    }
    tde_lock(&s_pstTDEOsiJobList->lock, lock);

    drv_tde_lock_working_flag(&lockflags);
    if (!gs_bWorkingFlag) {
        if (tde_hal_ctl_is_idle_safely()) {
            tde_list_comp_proc();
        }
    }

    drv_tde_unlock_working_flag(&lockflags);

    tde_unlock(&s_pstTDEOsiJobList->lock, lock);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

/*****************************************************************************
 Function:      tde_list_node_comp
 Description:   node complete interrupt service, maily complete deleting node and resume suspending,free node
 Input:         none
 Output:        none
 Return:        create job handle
 Others:        none
 Calls        :
 Called By    :
*****************************************************************************/
hi_void tde_list_node_comp()
{
    tde_handle_mgr *pHandleMgr = HI_NULL;
    tde_sw_job *pstJob = HI_NULL;
    hi_s32 s32FinishedHandle = -1;
    tde_sw_job *pstDelJob = HI_NULL;
    hi_s32 s32Delhandle = -1;
    hi_u64 running_node;
    hi_bool bWork = HI_TRUE;
    hi_u32 *pu32FinishHandle = HI_NULL;
    hi_size_t lockflags = 0;
#ifdef TDE_HWC_COOPERATE
    hi_size_t lock = 0;
#endif
    hi_bool bNotWork = HI_FALSE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
    if (tde_hal_ctl_is_idle_safely()) {
        bWork = HI_FALSE;
    }

    if (s_pstTDEOsiJobList->pstJobCommitted == HI_NULL || s_pstTDEOsiJobList->pstJobCommitted->last_cmd == HI_NULL) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
    running_node = s_pstTDEOsiJobList->pstJobCommitted->last_cmd->node_buf.phy_addr;

    bNotWork = ((running_node == 0) || (running_node == s_pstTDEOsiJobList->u32JobFinished));
    if (bNotWork) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    pu32FinishHandle = (hi_u32 *)wgetvrt(running_node - TDE_NODE_HEAD_BYTE + 4); /* offset by 4 */
    if (HI_NULL == pu32FinishHandle) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    s32FinishedHandle = *pu32FinishHandle;

    if (!bWork) {
        s_pstTDEOsiJobList->u32JobFinished = running_node;
    } else {
        if (!query_handle(s32FinishedHandle, &pHandleMgr)) {
            tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return;
        }
        pstJob = (tde_sw_job *)pHandleMgr->res;

        if (pstJob->list_head.prev == &s_pstTDEOsiJobList->list_head) {
            tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, s32FinishedHandle);
            GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "No pre Job left");
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return;
        }

        pstDelJob = osal_list_entry(pstJob->list_head.prev, tde_sw_job, list_head);
        s32FinishedHandle = pstDelJob->handle;
        s_pstTDEOsiJobList->u32JobFinished = 0x0;
    }

    if (!query_handle(s32FinishedHandle, &pHandleMgr)) {
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, s32FinishedHandle);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "handle already delete");
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    s_pstTDEOsiJobList->s32HandleFinished = s32FinishedHandle;

    while (!osal_list_empty(&s_pstTDEOsiJobList->list_head)) {
        pstDelJob = osal_list_entry(s_pstTDEOsiJobList->list_head.next, tde_sw_job, list_head);
        s32Delhandle = pstDelJob->handle;
        s_pstTDEOsiJobList->u32JobNum--;
        if (s32Delhandle == s_pstTDEOsiJobList->s32HandleLast) {
            s_pstTDEOsiJobList->s32HandleLast = -1;
        }
        osal_list_del_init(&pstDelJob->list_head);

        if (TDE_JOB_WAKE_NOTIFY == pstDelJob->notify_type) {
            pstDelJob->notify_type = TDE_JOB_NOTIFY_MAX;
            if (pstDelJob->func_complete_callback != HI_NULL) {
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDelJob->handle);
                GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "handle has callback func");
                tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
                pstDelJob->func_complete_callback(pstDelJob->func_para, &(pstDelJob->handle));
                tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
            }
            if (pstDelJob->is_wait) {
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDelJob->handle);
                GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "query handle complete");
                osal_wait_wakeup(&pstDelJob->query);
            }
            osal_wait_wakeup(&s_TdeBlockJobWq);
        } else if (TDE_JOB_COMPL_NOTIFY == pstDelJob->notify_type) {
            if (pstDelJob->func_complete_callback != HI_NULL) {
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, pstDelJob->handle);
                GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "handle has callback func");
                tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
                pstDelJob->func_complete_callback(pstDelJob->func_para, &(pstDelJob->handle));
                tde_lock(&s_pstTDEOsiJobList->lock, lockflags);
            }
            TdeOsiListSafeDestroyJob(pstDelJob);
        } else {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Error Status!!");
        }

        if (s32Delhandle == s32FinishedHandle) {
            if (!bWork) {
                if (tde_hal_ctl_is_idle_safely()) {
                    tde_hal_set_clock(HI_FALSE);
#ifdef TDE_HWC_COOPERATE
                    drv_tde_lock_working_flag(&lock);
                    gs_bWorkingFlag = 0;
                    drv_tde_unlock_working_flag(&lock);
#else
                    tde_list_comp_proc();
#endif
                }
            }
            tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return;
        }
    }
    if (tde_hal_ctl_is_idle_safely()) {
        tde_hal_set_clock(HI_FALSE);
#ifdef TDE_HWC_COOPERATE
        drv_tde_lock_working_flag(&lock);
        gs_bWorkingFlag = 0;
        drv_tde_unlock_working_flag(&lock);
#endif
    }
    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

/*****************************************************************************
 * Function:      tde_list_get_phy_buf
 * Description:    get one physical buffer, to deflicker and zoom
 * Input:
 * Output:        none
 * Return:        created job handle
 * Others:        none
 *****************************************************************************/
hi_u32 tde_list_get_phy_buf(hi_u32 cbcr_offset, hi_u32 buffer_size)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (osal_in_interrupt()) {
        return 0;
    }

    Ret = tde_alloc_physic_buffer(cbcr_offset, buffer_size);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return Ret;
}

STATIC hi_void TdeOsiListFreevmem(osal_workqueue *work)
{
    TDEFREEWQ_S *pWQueueInfo = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(work);

    pWQueueInfo = container_of(work, TDEFREEWQ_S, work);
    if (pWQueueInfo != HI_NULL) {
        pWQueueInfo->pWQCB(pWQueueInfo->Count);
        osal_workqueue_destroy(&pWQueueInfo->work);
        tde_free(pWQueueInfo);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

hi_void TdeOsiListHsr(void *pstFunc, hi_u32 data)
{
    TDEFREEWQ_S *pstWQ = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    pstWQ = tde_malloc(sizeof(TDEFREEWQ_S));
    if (HI_NULL == pstWQ) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return;
    }

    pstWQ->Count = data;
    pstWQ->pWQCB = (TDE_WQ_CB)pstFunc;

    if (osal_workqueue_init(&pstWQ->work, TdeOsiListFreevmem) != 0) {
        tde_free((hi_void *)pstWQ);
        return;
    }
    osal_workqueue_schedule(&pstWQ->work);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}
#endif

/*****************************************************************************
 * Function:      tde_list_put_phy_buf
 * Description:   put back physical buffer
 * Input:         buf_num
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_list_put_phy_buf(hi_u32 buf_num)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (0 != buf_num) {
#ifndef HI_BUILD_IN_BOOT
        TdeOsiListHsr((hi_void *)TdeOsiListDoFreePhyBuff, buf_num);
#else
        TdeOsiListDoFreePhyBuff(buf_num);
#endif
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_s32 drv_tde_lock_working_flag(hi_size_t *lock)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifdef TDE_HWC_COOPERATE
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(lock, HI_FAILURE);
    tde_lock(&s_WorkingFlagLock, *lock);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_tde_unlock_working_flag(hi_size_t *lock)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifdef TDE_HWC_COOPERATE
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(lock, HI_FAILURE);
    tde_unlock(&s_WorkingFlagLock, *lock);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_tde_get_working_flag(hi_bool *bFlag)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(bFlag, HI_FAILURE);
    *bFlag = gs_bWorkingFlag;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_tde_set_working_flag(hi_bool bFlag)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    gs_bWorkingFlag = bFlag;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#ifndef HI_BUILD_IN_BOOT
extern tde_handle_mgr *g_pstTdeHandleList;

hi_void tde_list_free_pending_job(hi_void *private_data)
{
    tde_handle_mgr *pstHandle = HI_NULL;
    tde_handle_mgr *pstSave = HI_NULL;
    tde_sw_job *pstJob = HI_NULL;
    hi_size_t lockflags = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(g_pstTdeHandleList);

    tde_lock(&s_pstTDEOsiJobList->lock, lockflags);

    pstHandle = osal_list_entry(g_pstTdeHandleList->list_head.next, tde_handle_mgr, list_head);
    if (pstHandle == HI_NULL) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "No pending job!");
        tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
        return;
    }

    while (pstHandle != g_pstTdeHandleList) {
        pstSave = pstHandle;
        pstJob = (tde_sw_job *)pstSave->res;
        if (pstJob == HI_NULL) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "pstJob Null Pointer!");
            tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);
            return;
        }

        pstHandle = osal_list_entry(pstHandle->list_head.next, tde_handle_mgr, list_head);

        /* * free when it is not submitted * */
        if ((!pstJob->has_submitted) && (private_data == pstJob->private_data)) {
            /* * free handle resource * */
            tde_list_free_serial_cmd(pstJob->first_cmd, pstJob->tail_node);
            osal_list_del_init(&pstSave->list_head);
            tde_free(pstSave);
        }

        if (pstHandle == HI_NULL) {
            break;
        }
    }

    tde_unlock(&s_pstTDEOsiJobList->lock, lockflags);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}
#endif

#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void tde_osi_list_proc(hi_void *p)
{
    if ((p != HI_NULL) && (s_pstTDEOsiJobList != HI_NULL)) {
        osal_proc_print(p, "finish handle\t: %u\n", s_pstTDEOsiJobList->s32HandleFinished);
    }
    return;
}
#endif

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: list operation
 * Author: sdk
 * Create: 2019-05-17
 */

#include <linux/hisilicon/securec.h>
#include "hi_osal.h"

#include "hi_drv_osal.h"
#include "hi_gfx_sys_k.h"
#include "drv_gfx2d_struct.h"
#include "drv_gfx2d_errcode.h"
#include "hi_gfx_comm_k.h"
#include "drv_gfx2d_hal.h"
#include "drv_gfx2d_mem.h"
#include "drv_gfx2d_config.h"
#include "drv_gfx2d_fence.h"
#include "drv_gfx2d_list.h"
#ifdef GFX2D_SYNC_TDE
#include "hi_drv_tde.h"
#endif
#include "drv_gfx2d_debug.h"

/* **************************** Macro Definition ************************************************* */
#define GFX2D_ALIGN(value) (((value) + 0xf) & 0xfffffff0)

#define GFX2D_LIST_CHECK_DEVID(id) do { \
    if ((id) > DRV_GFX2D_DEV_ID_0) {        \
        return DRV_GFX2D_ERR_INVALID_DEVID; \
    }                                       \
} while (0)

#define GFX2D_LIST_CHECK_DEVTYPE(type) do { \
    if ((type) > GFX2D_HAL_DEV_TYPE_HWC) {  \
        return DRV_GFX2D_ERR_INVALID_DEVID; \
    }                                       \
} while (0)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
#define gfx2d_ion_get osal_mem_ref_get
#define gfx2d_ion_put osal_mem_ref_put
#else
#define gfx2d_ion_get
#define gfx2d_ion_put
#endif
/* ************************** Structure Definition *********************************************** */
typedef enum {
    GFX2D_LIST_TASKSTATE_UNFINISH = 0x0,
    GFX2D_LIST_TASKSTATE_FINISH,
} GFX2D_LIST_TASKSTATE_E;

#ifdef GFX2D_ASYNC_PTHREAD_SUPPORT
typedef struct {
    hi_mem_handle_t src_phy_addr[GFX2D_MAX_LAYERS][DRV_GFX2D_MAX_SURFACE_NUM];
    hi_void *free_mem_dma_buf[GFX2D_MAX_LAYERS][DRV_GFX2D_MAX_SURFACE_NUM];
    osal_workqueue gfx2d_free_mem_work;
} gfx2d_free_mem_work_mgr;
#endif
/* task descriptor */
typedef struct {
    /* List which links all submitted task by submit order. */
    struct osal_list_head stList;
    osal_spinlock lock;             /* Spin lock for concurrent access */
    hi_void *pNode;                 /* First node of task,which include info for hardware */
    drv_gfx2d_dev_id dev_id;        /* Dev id the task submitted to */
    GFX2D_HAL_DEV_TYPE_E enDevType; /* Dev type the task submitted to */
    GFX2D_LIST_TASKSTATE_E enState; /* Task state */
    hi_u32 u32Ref;                  /* Reference count */
    hi_bool bWait;                  /* Whether some process is waiting for the task finish */
    osal_wait stWaitQueueHead;      /* wait queue head */
    hi_bool work_sync;
    hi_s32 src_acquire_fd[GFX2D_MAX_LAYERS];
    hi_s32 dst_acquire_fd;
    hi_s32 release_fence_fd;
    drv_gfx2d_compose_ext_info ext_info;
#ifdef GFX2D_FENCE_SUPPORT
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    struct sync_fence *src_fence[GFX2D_MAX_LAYERS];
    struct sync_fence *dst_fence;
#else
    struct dma_fence *src_fence[GFX2D_MAX_LAYERS];
    struct dma_fence *dst_fence;
#endif
#endif
#ifdef GFX2D_ASYNC_PTHREAD_SUPPORT
    gfx2d_free_mem_work_mgr *free_mem_work;
#endif

} GFX2D_LIST_TASK_S;

typedef struct {
    hi_bool is_create;
    hi_bool need_wait_up;
    hi_bool to_submit_task;
    osal_spinlock submit_task_lock;
    osal_task *pthread_task;
} gfx2d_submit_work_mgr;

/* task management descriptor */
typedef struct {
    struct osal_list_head stListHead; /* List head which links all submitted task */
    osal_spinlock lock;               /* Spin lock for concurrent access */
    GFX2D_LIST_TASK_S *pstSubmitedHeadTask; /* First task submiited to hardware */           /* 正在工作的链表头节点 */
    GFX2D_LIST_TASK_S *pstSubmitedTailTask; /* Last task submiited to hardware */            /* 正在工作的链表尾节点 */
    GFX2D_LIST_TASK_S *pstToSubmitTask; /* First task waitting or submmitting to hardware */ /* 待提交的链表头节点 */
    GFX2D_HAL_NODE_OPS_S *pstNodeOps; /* functions for access node */
#ifdef GFX2D_FENCE_SUPPORT
    osal_wait gfx2d_wait_queue;
    gfx2d_submit_work_mgr submit_work_mgr;
#endif
#ifdef CONFIG_GFX_PROC_SUPPORT
    /* proc info */
    hi_u32 u32TotalTaskNum;   /* total task num submitted */
    hi_u32 u32RunTaskNum;     /* working task num */
    hi_u32 u32WaitTaskNum;    /* waiting task num */
    hi_u32 u32MaxTaskPerList; /* max task num submitted to hardware */
    hi_u32 u32LastTimeCost;   /* last task time cost */
    hi_u32 u32MaxTimeCost;    /* max time cost */
    hi_u32 u32TotalTimeCost;  /* total time cost */
    hi_u32 u32CollisionCnt;
    hi_u32 u32LastCollisionTimeCost;
    hi_u32 u32MaxCollisionTimeCost;
#endif
} GFX2D_LIST_TASK_MGR_S;

#ifdef CONFIG_GFX_PROC_SUPPORT
typedef struct {
    hi_bool bDestoryTaskTimesInIsrFunc;
    hi_u32 DestoryTaskTimesInIsrFunc;
    hi_u32 DestoryTaskTimesInSubNode;
} GFX2D_LIST_DEBUG_S;
static GFX2D_LIST_DEBUG_S g_gfx2d_list_debug = {0};
#endif

/* ****************************** API forward declarations *************************************** */
static GFX2D_LIST_TASK_MGR_S g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0 + 1][GFX2D_HAL_DEV_TYPE_HWC + 1];

hi_s32 GFX2D_LIST_Isr(hi_s32 irq, hi_void *dev_id);

static inline GFX2D_LIST_TASK_S *CreateTask(const drv_gfx2d_dev_id dev_id, hi_void *pNode,
                                            GFX2D_HAL_DEV_TYPE_E enNodeType, const hi_bool work_sync,
                                            hi_s32 release_fence_fd);

static inline hi_void DestroyTask(GFX2D_LIST_TASK_S *pstTask);

static inline hi_void InsertTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr, GFX2D_LIST_TASK_S *pstTask);

static inline hi_void DeleteTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr, GFX2D_LIST_TASK_S *pstTask);

static inline hi_void SubmitTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr);

static inline hi_void UnrefTask(GFX2D_LIST_TASK_S *pstTask);

static GFX2D_LIST_TASK_S *GetTailTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr);

static hi_s32 WaitForDone(GFX2D_LIST_TASK_S *pstTask, hi_u32 time_out);

static void Gfx2dTaskletFunc(unsigned long int_status);

#ifdef GFX2D_SYNC_TDE
static hi_u32 g_HWCIrqHandle = 0;
#endif

static osal_tasklet g_Gfx2dTasklet;

hi_s32 gfx2d_check_need_wait_flag(const void *param)
{
    GFX2D_LIST_TASK_MGR_S *task_mgr = (GFX2D_LIST_TASK_MGR_S *)param;

    if (task_mgr == HI_NULL) {
        return 0;
    }
#ifdef GFX2D_FENCE_SUPPORT
    if (task_mgr->submit_work_mgr.need_wait_up == HI_TRUE) {
        return 1;
    }
#endif
    return 0;
}

#ifdef GFX2D_FENCE_SUPPORT
static hi_s32 gfx2d_submit_work_process(hi_void *work)
{
    GFX2D_LIST_TASK_MGR_S *task_mgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
    hi_ulong lockflag = 0;
    hi_s32 ret = HI_FAILURE;
    hi_bool to_submit_ready = HI_FALSE;

    while (!osal_kthread_should_stop()) {
        osal_spin_lock_irqsave(&(task_mgr->submit_work_mgr.submit_task_lock), &lockflag);
        to_submit_ready = task_mgr->submit_work_mgr.to_submit_task;
        task_mgr->submit_work_mgr.to_submit_task = HI_FALSE;
        osal_spin_unlock_irqrestore(&(task_mgr->submit_work_mgr.submit_task_lock), &lockflag);
        if (to_submit_ready == HI_TRUE) {
            SubmitTask(task_mgr);
        }
        ret = osal_wait_timeout_interruptible(&task_mgr->gfx2d_wait_queue, gfx2d_check_need_wait_flag,
                                              (hi_void *)task_mgr, 1000);
        /* if is not wake up by event call */
        if (ret <= 0) {
            continue;
        }
        task_mgr->submit_work_mgr.need_wait_up = HI_FALSE;
    }
    return HI_SUCCESS;
}

hi_s32 gfx2d_submit_task_create_pthread(osal_task **pthread_task, hi_s32 (*osal_kthread_handler)(hi_void *data),
                                        hi_void *data)
{
    *pthread_task = osal_kthread_create(osal_kthread_handler, data, "gfx2d_submit_task_mgr", 0);
    if (IS_ERR(*pthread_task)) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_kthread_create, HI_FAILURE);
        return HI_FAILURE;
    }

    osal_kthread_set_priority(*pthread_task, OSAL_TASK_PRIORITY_HIGH);
    return HI_SUCCESS;
}
#endif

hi_s32 gfx2d_init_task_mem(hi_void)
{
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
    if (osal_spin_lock_init(&(pstTaskMgr->lock)) != 0) {
        pstTaskMgr->lock.lock = HI_NULL;
        return DRV_GFX2D_ERR_NO_MEM;
    }

#ifdef GFX2D_FENCE_SUPPORT
    if (osal_wait_init(&(pstTaskMgr->gfx2d_wait_queue)) != 0) {
        pstTaskMgr->gfx2d_wait_queue.wait = HI_NULL;
        return DRV_GFX2D_ERR_NO_MEM;
    }
    if (osal_spin_lock_init(&(pstTaskMgr->submit_work_mgr.submit_task_lock)) != 0) {
        pstTaskMgr->submit_work_mgr.submit_task_lock.lock = HI_NULL;
        return DRV_GFX2D_ERR_NO_MEM;
    }
#endif

    g_Gfx2dTasklet.handler = Gfx2dTaskletFunc;
    if (osal_tasklet_init(&g_Gfx2dTasklet) != 0) {
        g_Gfx2dTasklet.tasklet = HI_NULL;
        return DRV_GFX2D_ERR_NO_MEM;
    }
    osal_tasklet_update(&g_Gfx2dTasklet);

    return HI_SUCCESS;
}

hi_void gfx2d_deinit_task_mem(hi_void)
{
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];

    if (pstTaskMgr->lock.lock != HI_NULL) {
        osal_spin_lock_destory(&(pstTaskMgr->lock));
    }

#ifdef GFX2D_FENCE_SUPPORT
    if (pstTaskMgr->gfx2d_wait_queue.wait != HI_NULL) {
        osal_wait_destroy(&(pstTaskMgr->gfx2d_wait_queue));
    }
    if (pstTaskMgr->submit_work_mgr.submit_task_lock.lock != HI_NULL) {
        osal_spin_lock_destory(&(pstTaskMgr->submit_work_mgr.submit_task_lock));
    }
#endif

    if (g_Gfx2dTasklet.tasklet != HI_NULL) {
        osal_tasklet_kill(&g_Gfx2dTasklet);
    }
}

hi_s32 GFX2D_LIST_Init(hi_void)
{
    hi_u32 u32IntNum;
    hi_u32 u32NodeNum = 0;
    hi_u32 u32NodeSize = 0;
    hi_s32 s32Ret = HI_FAILURE;
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_u32 u32BaseAddr = 0;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    memset_s(pstTaskMgr, sizeof(GFX2D_LIST_TASK_MGR_S), 0, sizeof(GFX2D_LIST_TASK_MGR_S));
    OSAL_INIT_LIST_HEAD(&(pstTaskMgr->stListHead));

    GFX2D_HAL_GetNodeOps(GFX2D_HAL_DEV_TYPE_HWC, &(pstTaskMgr->pstNodeOps));
    if (pstTaskMgr->pstNodeOps == NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstTaskMgr->pstNodeOps);
        return DRV_GFX2D_ERR_SYS;
    }

    u32IntNum = GFX2D_HAL_GetIsrNum(DRV_GFX2D_DEV_ID_0, GFX2D_HAL_DEV_TYPE_HWC);

#ifdef GFX2D_SYNC_TDE
    s32Ret = hi_drv_osal_request_irq(u32IntNum, (irq_handler_t)GFX2D_LIST_Isr, IRQF_SHARED, "gfx2d", &g_HWCIrqHandle);
#else
    s32Ret = hi_drv_osal_request_irq(u32IntNum, (irq_handler_t)GFX2D_LIST_Isr, IRQF_PROBE_SHARED, "gfx2d", NULL);
#endif
    if (s32Ret != 0) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_irq_request, FAILURE_TAG);
        return DRV_GFX2D_ERR_SYS;
    }

    s32Ret = HI_GFX_SetIrq(HIGFX_GFX2D_ID, u32IntNum, "gfx2d");
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32IntNum);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_SetIrq, s32Ret);
        goto FREE_IRQ_EXIT;
    }

    u32NodeNum = GFX2D_CONFIG_GetNodeNum();
    u32NodeSize = GFX2D_ALIGN(sizeof(GFX2D_LIST_TASK_S));
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32NodeNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32NodeSize);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==================================================");

    s32Ret = GFX2D_MEM_Register(u32NodeSize, u32NodeNum);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_MEM_Register, s32Ret);
        goto FREE_IRQ_EXIT;
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    u32BaseAddr = GFX2D_HAL_GetBaseAddr(DRV_GFX2D_DEV_ID_0, GFX2D_HAL_DEV_TYPE_HWC);

    s32Ret = HI_GFX_MapSmmuReg(u32BaseAddr + 0xf000);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_InitSmmu, s32Ret);
        goto FREE_EIXT;
    }

    s32Ret = HI_GFX_InitSmmu(u32BaseAddr + 0xf000);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_InitSmmu, s32Ret);
        goto FREE_EIXT;
    }

#endif

    if (gfx2d_init_task_mem() != HI_SUCCESS) {
        goto FREE_EIXT;
    }

#ifdef GFX2D_FENCE_SUPPORT
    s32Ret = gfx2d_submit_task_create_pthread(&(pstTaskMgr->submit_work_mgr.pthread_task), gfx2d_submit_work_process,
                                              &(pstTaskMgr->submit_work_mgr));
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, gfx2d_submit_task_create_pthread, s32Ret);
        goto FREE_EIXT;
    }
    pstTaskMgr->submit_work_mgr.need_wait_up = HI_FALSE;
    pstTaskMgr->submit_work_mgr.is_create = HI_TRUE;
    pstTaskMgr->submit_work_mgr.to_submit_task = HI_FALSE;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;

FREE_EIXT:
    GFX2D_Mem_UnRegister(u32NodeSize);

FREE_IRQ_EXIT:
#ifdef GFX2D_SYNC_TDE
    hi_drv_osal_free_irq(u32IntNum, "gfx2d", &g_HWCIrqHandle);
#else
    hi_drv_osal_free_irq(u32IntNum, "gfx2d", NULL);
#endif
    gfx2d_deinit_task_mem();
    return HI_FAILURE;
}

hi_void GFX2D_LIST_Deinit(hi_void)
{
    hi_u32 u32IntNum;
    hi_u32 u32NodeSize;
#ifdef GFX2D_FENCE_SUPPORT
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifdef CONFIG_GFX_MMU_SUPPORT
    HI_GFX_DeinitSmmu();
    HI_GFX_UnMapSmmuReg();
#endif

    u32NodeSize = GFX2D_ALIGN(sizeof(GFX2D_LIST_TASK_S));
    GFX2D_Mem_UnRegister(u32NodeSize);

    u32IntNum = GFX2D_HAL_GetIsrNum(DRV_GFX2D_DEV_ID_0, GFX2D_HAL_DEV_TYPE_HWC);
#ifdef GFX2D_SYNC_TDE
    hi_drv_osal_free_irq(u32IntNum, "gfx2d", &g_HWCIrqHandle);
#else
    hi_drv_osal_free_irq(u32IntNum, "gfx2d", NULL);
#endif

#ifdef GFX2D_FENCE_SUPPORT
    if (pstTaskMgr->submit_work_mgr.is_create == HI_TRUE) {
        pstTaskMgr->submit_work_mgr.need_wait_up = HI_TRUE;
        osal_wait_wakeup(&(pstTaskMgr->gfx2d_wait_queue));

        if (!IS_ERR(pstTaskMgr->submit_work_mgr.pthread_task)) {
            osal_kthread_destroy(pstTaskMgr->submit_work_mgr.pthread_task, HI_TRUE);
            pstTaskMgr->submit_work_mgr.pthread_task = NULL;
        }
        pstTaskMgr->submit_work_mgr.is_create = HI_FALSE;
    }
#endif

    gfx2d_deinit_task_mem();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#ifdef GFX2D_SYNC_TDE
static hi_tde_export_func *g_gfx2d_tde_export_funcs = HI_NULL;
#endif

hi_s32 GFX2D_LIST_Open(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifdef GFX2D_SYNC_TDE
    if (g_gfx2d_tde_export_funcs != HI_NULL) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (HI_SUCCESS != osal_exportfunc_get(HI_ID_TDE, (hi_void **)&g_gfx2d_tde_export_funcs)) {
        g_gfx2d_tde_export_funcs = HI_NULL;
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, hi_drv_module_get_function, FAILURE_TAG);
        return HI_FAILURE;
    }

    if (g_gfx2d_tde_export_funcs == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_gfx2d_tde_export_funcs);
        return HI_FAILURE;
    }

    if ((g_gfx2d_tde_export_funcs->drv_tde_module_lock_working_flag == HI_NULL) ||
        (g_gfx2d_tde_export_funcs->drv_tde_module_get_working_flag == HI_NULL) ||
        (g_gfx2d_tde_export_funcs->drv_tde_module_unlock_working_flag == HI_NULL) ||
        (g_gfx2d_tde_export_funcs->drv_tde_module_set_working_flag == HI_NULL)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_gfx2d_tde_export_funcs->drv_tde_module_lock_working_flag);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_gfx2d_tde_export_funcs->drv_tde_module_get_working_flag);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_gfx2d_tde_export_funcs->drv_tde_module_unlock_working_flag);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_gfx2d_tde_export_funcs->drv_tde_module_set_working_flag);
        g_gfx2d_tde_export_funcs = HI_NULL;
        return HI_FAILURE;
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void GFX2D_LIST_Close(hi_void)
{
#ifdef GFX2D_SYNC_TDE
    g_gfx2d_tde_export_funcs = HI_NULL;
#endif
    return;
}

#if defined(GFX2D_FENCE_SUPPORT) && defined(GFX2D_ASYNC_PTHREAD_SUPPORT)
static hi_void gfx2d_free_mem_work_process(osal_workqueue *work)
{
    hi_u32 i, j;
    gfx2d_free_mem_work_mgr *free_mem_work;
    free_mem_work = (gfx2d_free_mem_work_mgr *)container_of(work, gfx2d_free_mem_work_mgr, gfx2d_free_mem_work);
    if (free_mem_work == NULL) {
        return;
    }

    for (i = 0; i < DRV_GFX2D_MAX_SURFACE_NUM; i++) {
        for (j = 0; j < GFX2D_MAX_LAYERS; j++) {
            if (free_mem_work->src_phy_addr[j][i] <= 0) {
                continue;
            }
            osal_mem_ref_put(free_mem_work->free_mem_dma_buf[j][i], ConvertID(HIGFX_GFX2D_ID));

            free_mem_work->src_phy_addr[j][i] = 0;
        }
    }
    HI_GFX_KFREE(HIGFX_GFX2D_ID, free_mem_work);
}
#endif

#ifdef GFX2D_FENCE_SUPPORT
static hi_void save_address_to_task(const GFX2D_LIST_NODE_S *list_node, hi_bool work_sync, GFX2D_LIST_TASK_S *task)
{
#if defined(GFX2D_USE_ION_MEM) && defined(GFX2D_ASYNC_PTHREAD_SUPPORT)
    hi_u32 i, j;
    gfx2d_free_mem_work_mgr *free_mem_work;
#endif

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(list_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(task);

    if ((list_node == NULL) || (task == NULL) || work_sync == HI_TRUE) {
        return;
    }

#if defined(GFX2D_USE_ION_MEM) && defined(GFX2D_ASYNC_PTHREAD_SUPPORT)
    free_mem_work = HI_GFX_KMALLOC(HIGFX_GFX2D_ID, sizeof(gfx2d_free_mem_work_mgr), GFP_KERNEL);
    if (free_mem_work == NULL) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "frem_mem_work kmalloc failure");
        return;
    }
    memset(free_mem_work, 0x0, sizeof(gfx2d_free_mem_work_mgr));
    task->free_mem_work = free_mem_work;
    if (osal_workqueue_init(&(free_mem_work->gfx2d_free_mem_work), gfx2d_free_mem_work_process) != 0) {
        HI_GFX_KFREE(HIGFX_GFX2D_ID, (hi_void *)free_mem_work);
        return;
    }

    for (i = 0; i < DRV_GFX2D_MAX_SURFACE_NUM; i++) {
        for (j = 0; j < GFX2D_MAX_LAYERS; j++) {
            if (list_node->src_phy_addr[j][i] <= 0) {
                continue;
            }
            free_mem_work->src_phy_addr[j][i] = list_node->src_phy_addr[j][i];
            free_mem_work->free_mem_dma_buf[j][i] = osal_mem_handle_get(list_node->src_phy_addr[j][i],
                                                                        ConvertID(HIGFX_GFX2D_ID));
        }
    }
#endif
    return;
}

static hi_void put_input_ddr_from_task(GFX2D_LIST_TASK_S *task)
{
#if defined(GFX2D_ASYNC_PTHREAD_SUPPORT)
    gfx2d_free_mem_work_mgr *free_mem_work = NULL;
    if (task == NULL || task->work_sync == HI_TRUE) {
        return;
    }

    free_mem_work = task->free_mem_work;
    if (free_mem_work == NULL) {
        return;
    }
    osal_workqueue_schedule(&(free_mem_work->gfx2d_free_mem_work));
#else
    // HI_UNUSED(task);
#endif
    return;
}

static hi_void save_fence_to_task(const GFX2D_LIST_NODE_S *list_node, hi_bool work_sync, GFX2D_LIST_TASK_S *task)
{
    hi_u32 i = 0;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(list_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(task);
    if (work_sync == HI_TRUE) {
        return;
    }

    for (i = 0; i < GFX2D_MAX_LAYERS; i++) {
        task->src_fence[i] = list_node->src_fence[i];
        task->src_acquire_fd[i] = list_node->src_acquire_fd[i];
    }

    return;
}
#endif

hi_s32 GFX2D_LIST_SubNode(GFX2D_LIST_NODE_S *pstNode, drv_gfx2d_compose_ext_info *ext_info)
{
    hi_s32 s32Ret = HI_SUCCESS;
    GFX2D_LIST_TASK_S *pstTask = NULL;
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = NULL;
#ifdef GFX2D_FENCE_SUPPORT
    hi_ulong lockflag;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstNode, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(ext_info, HI_FAILURE);

    GFX2D_LIST_CHECK_DEVID(pstNode->dev_id);
    GFX2D_LIST_CHECK_DEVTYPE(pstNode->enNodeType);

    pstTaskMgr = &g_gfx2d_list_task_manager[pstNode->dev_id][pstNode->enNodeType];
    pstTask = CreateTask(pstNode->dev_id, pstNode->pNode, pstNode->enNodeType, pstNode->work_sync,
                         pstNode->release_fence_fd);
    if (pstTask == HI_NULL) {
        if (pstTaskMgr->pstNodeOps && pstTaskMgr->pstNodeOps->pfnFreeNode) {
            pstTaskMgr->pstNodeOps->pfnFreeNode(pstNode->pNode);
        }
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CreateTask, FAILURE_TAG);
        return DRV_GFX2D_ERR_NO_MEM;
    }

#ifdef GFX2D_FENCE_SUPPORT
    save_fence_to_task(pstNode, pstNode->work_sync, pstTask);
    save_address_to_task(pstNode, pstNode->work_sync, pstTask);
#endif
    InsertTask(pstTaskMgr, pstTask);
#ifdef GFX2D_FENCE_SUPPORT
    if (pstNode->work_sync == HI_FALSE) {
        osal_spin_lock_irqsave(&(pstTaskMgr->submit_work_mgr.submit_task_lock), &lockflag);
        pstTaskMgr->submit_work_mgr.need_wait_up = HI_TRUE;
        pstTaskMgr->submit_work_mgr.to_submit_task = HI_TRUE;
        osal_spin_unlock_irqrestore(&(pstTaskMgr->submit_work_mgr.submit_task_lock), &lockflag);
        osal_wait_wakeup(&(pstTaskMgr->gfx2d_wait_queue));
    } else {
#endif
        SubmitTask(pstTaskMgr);
#ifdef GFX2D_FENCE_SUPPORT
    }
#endif

    if (pstNode->work_sync) {
        s32Ret = WaitForDone(pstTask, pstNode->time_out);
        if (s32Ret != HI_SUCCESS) {
            DestroyTask(pstTask);
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, WaitForDone, FAILURE_TAG);
            return s32Ret;
        }
        if (memcpy_s(ext_info, sizeof(drv_gfx2d_compose_ext_info), &(pstTask->ext_info),
                     sizeof(drv_gfx2d_compose_ext_info)) != EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "call memcpy_s failed");
        }
        DestroyTask(pstTask);
    } else {
#ifdef CONFIG_GFX_PROC_SUPPORT
        if (g_gfx2d_list_debug.DestoryTaskTimesInIsrFunc > g_gfx2d_list_debug.DestoryTaskTimesInSubNode) {
            g_gfx2d_list_debug.bDestoryTaskTimesInIsrFunc = HI_TRUE;
        }
        g_gfx2d_list_debug.DestoryTaskTimesInSubNode++;
#endif
        if (memcpy_s(ext_info, sizeof(drv_gfx2d_compose_ext_info), &(pstTask->ext_info),
                     sizeof(drv_gfx2d_compose_ext_info)) != EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "call memcpy_s failed");
        }
        DestroyTask(pstTask);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 GFX2D_LIST_WaitAllDone(const drv_gfx2d_dev_id dev_id, const hi_u32 time_out)
{
    GFX2D_LIST_TASK_S *pstTask = NULL;
    hi_s32 s32Ret = HI_FAILURE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GFX2D_LIST_CHECK_DEVID(dev_id);

    pstTask = GetTailTask(&g_gfx2d_list_task_manager[dev_id][GFX2D_HAL_DEV_TYPE_HWC]);
    if (pstTask == HI_NULL) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    s32Ret = WaitForDone(pstTask, time_out);
    if (s32Ret != HI_SUCCESS) {
        DestroyTask(pstTask);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, WaitForDone, s32Ret);
        return s32Ret;
    }

    DestroyTask(pstTask);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static GFX2D_LIST_TASK_S *CreateTask(const drv_gfx2d_dev_id dev_id, hi_void *pNode, GFX2D_HAL_DEV_TYPE_E enNodeType,
                                     const hi_bool work_sync, hi_s32 release_fence_fd)
{
    GFX2D_LIST_TASK_S *pstTask = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    pstTask = (GFX2D_LIST_TASK_S *)HI_GFX_KMALLOC(HIGFX_GFX2D_ID, sizeof(GFX2D_LIST_TASK_S), GFP_KERNEL);
    if (pstTask == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, sizeof(GFX2D_LIST_TASK_S));
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_KMALLOC, FAILURE_TAG);
        return HI_NULL;
    }

    memset_s(pstTask, sizeof(GFX2D_LIST_TASK_S), 0, sizeof(GFX2D_LIST_TASK_S));

    OSAL_INIT_LIST_HEAD(&(pstTask->stList));
    if (osal_spin_lock_init(&(pstTask->lock)) != 0) {
        HI_GFX_KFREE(HIGFX_GFX2D_ID, (hi_void *)pstTask);
        return HI_NULL;
    }
    if (osal_wait_init(&(pstTask->stWaitQueueHead)) != 0) {
        HI_GFX_KFREE(HIGFX_GFX2D_ID, (hi_void *)pstTask);
        osal_spin_lock_destory(&(pstTask->lock));
        return HI_NULL;
    }

    pstTask->u32Ref = 1;
    pstTask->pNode = pNode;
    pstTask->dev_id = dev_id;
    pstTask->enDevType = enNodeType;
    pstTask->work_sync = work_sync;

#ifdef GFX2D_FENCE_SUPPORT
    pstTask->release_fence_fd = release_fence_fd;
#endif

    memset_s(&(pstTask->ext_info.alpha_sum), sizeof(pstTask->ext_info.alpha_sum), 0xff,
             sizeof(pstTask->ext_info.alpha_sum));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return pstTask;
}

static hi_void DestroyTask(GFX2D_LIST_TASK_S *pstTask)
{
    hi_ulong lockflag = 0;
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTask);
    osal_spin_lock_irqsave(&(pstTask->lock), &lockflag);
    pstTask->u32Ref--;
    if ((pstTask->u32Ref == 0) && (GFX2D_LIST_TASKSTATE_FINISH == pstTask->enState)) {
        if (pstTaskMgr->pstNodeOps && pstTaskMgr->pstNodeOps->pfnFreeNode) {
            pstTaskMgr->pstNodeOps->pfnFreeNode(pstTask->pNode);
        }
        osal_spin_unlock_irqrestore(&(pstTask->lock), &lockflag);
        osal_spin_lock_destory(&(pstTask->lock));
        osal_wait_destroy(&(pstTask->stWaitQueueHead));
        HI_GFX_KFREE(HIGFX_GFX2D_ID, pstTask);

        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    osal_spin_unlock_irqrestore(&(pstTask->lock), &lockflag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void InsertTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr, GFX2D_LIST_TASK_S *pstTask)
{
    hi_ulong lockflag = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTaskMgr);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTask);

    osal_spin_lock_irqsave(&(pstTaskMgr->lock), &lockflag);

    osal_list_add_tail(&(pstTask->stList), &(pstTaskMgr->stListHead));
    if (pstTaskMgr->pstToSubmitTask == NULL) {
        /* * 记录待提交任务的首个任务节点的地址 * */
        pstTaskMgr->pstToSubmitTask = pstTask;
    }

#ifdef CONFIG_GFX_PROC_SUPPORT
    pstTaskMgr->u32TotalTaskNum++;
    pstTaskMgr->u32WaitTaskNum++;
#endif

    osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void DeleteTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr, GFX2D_LIST_TASK_S *pstTask)
{
    hi_ulong lockflag = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTaskMgr);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTask);

    osal_spin_lock_irqsave(&(pstTaskMgr->lock), &lockflag);

    osal_list_del_init(&(pstTask->stList));

    osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#ifdef GFX2D_FENCE_SUPPORT
#define GFX2D_NEED_WAIT_NUM 70 /* task(10) * layer(7) */
static hi_s32 wait_task_fence(GFX2D_LIST_TASK_MGR_S *task)
{
    hi_u32 i = 0;
    hi_u32 fence_num = 0;
    hi_u32 acquire_fd[GFX2D_NEED_WAIT_NUM] = {0};
    hi_ulong lockflag = 0;
    struct osal_list_head *cur_list = NULL;
    GFX2D_LIST_TASK_S *cur_task = NULL;
#ifndef GFX2D_SUBMIT_EVERY_TASK
    struct osal_list_head *next_list = NULL;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    struct sync_fence *acquire_fence[GFX2D_NEED_WAIT_NUM] = {NULL};
#else
    struct dma_fence *acquire_fence[GFX2D_NEED_WAIT_NUM] = {NULL};
#endif

    if (task->pstToSubmitTask == NULL) {
        return HI_SUCCESS;
    }

    osal_spin_lock_irqsave(&(task->lock), &lockflag);
    cur_list = &(task->pstToSubmitTask->stList);
#ifndef GFX2D_SUBMIT_EVERY_TASK
    next_list = cur_list->next;
    while (next_list != (&task->stListHead)) {
        cur_task = (GFX2D_LIST_TASK_S *)cur_list;
        for (i = 0; i < GFX2D_MAX_LAYERS; i++) {
            if (fence_num >= GFX2D_NEED_WAIT_NUM) {
                osal_spin_unlock_irqrestore(&(task->lock), &lockflag);
                return HI_FAILURE;
            }
            if (cur_task->src_fence[i] != NULL) {
                acquire_fd[fence_num] = cur_task->src_acquire_fd[i];
                acquire_fence[fence_num] = cur_task->src_fence[i];
                fence_num++;
                cur_task->src_fence[i] = NULL;
            }
        }
        if (fence_num >= GFX2D_NEED_WAIT_NUM) {
            osal_spin_unlock_irqrestore(&(task->lock), &lockflag);
            return HI_FAILURE;
        }
        cur_list = next_list;
        next_list = next_list->next;
    }
#endif
    if (cur_list != NULL) {
        cur_task = (GFX2D_LIST_TASK_S *)cur_list;
        for (i = 0; i < GFX2D_MAX_LAYERS; i++) {
            if (fence_num >= GFX2D_NEED_WAIT_NUM) {
                osal_spin_unlock_irqrestore(&(task->lock), &lockflag);
                return HI_FAILURE;
            }
            if (cur_task->src_fence[i] != NULL) {
                acquire_fd[fence_num] = cur_task->src_acquire_fd[i];
                acquire_fence[fence_num] = cur_task->src_fence[i];
                fence_num++;
                cur_task->src_fence[i] = NULL;
            }
        }
        if (fence_num >= GFX2D_NEED_WAIT_NUM) {
            osal_spin_unlock_irqrestore(&(task->lock), &lockflag);
            return HI_FAILURE;
        }
    }
    osal_spin_unlock_irqrestore(&(task->lock), &lockflag);

    for (i = 0; i < fence_num; i++) {
        if (acquire_fence[i] != NULL) {
            GFX2D_FENCE_Wait(acquire_fence[i], acquire_fd[i]);
        }
    }
    return HI_SUCCESS;
}
#endif

static hi_void SubmitTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr)
{
    hi_s32 ret = HI_FAILURE;
    hi_ulong lockflag;
    struct osal_list_head *pstCur = NULL;
    struct osal_list_head *pstNext = NULL;
#ifndef GFX2D_SUBMIT_EVERY_TASK
    GFX2D_LIST_TASK_S *pstCurTask = NULL;
    GFX2D_LIST_TASK_S *pstNextTask = NULL;
#endif
#ifdef GFX2D_SYNC_TDE
    hi_bool bDeviceBusy = HI_FALSE;
    hi_ulong deviceflag = 0;
#endif
    HI_UNUSED(ret);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTaskMgr);

#ifdef GFX2D_FENCE_SUPPORT
    ret = wait_task_fence(pstTaskMgr);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, wait_task_fence, ret);
    }
#endif

    /* If the hardware is working or no task is submited by app,return!
    Use the pstTaskMgr->pstSubmitedHeadTask to judge whether the hardware
    is working or not:when submitting task to hardware,set the flag the addr
    of submitted task.After the task is done,set the flag null to denote the
    hardware is idle! */
    if ((pstTaskMgr->pstSubmitedHeadTask != NULL) || (pstTaskMgr->pstToSubmitTask == NULL)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    osal_spin_lock_irqsave(&(pstTaskMgr->lock), &lockflag);
#ifdef GFX2D_SYNC_TDE
    g_gfx2d_tde_export_funcs->drv_tde_module_lock_working_flag(&deviceflag);

    g_gfx2d_tde_export_funcs->drv_tde_module_get_working_flag(&bDeviceBusy);

    if (bDeviceBusy) {
        g_gfx2d_tde_export_funcs->drv_tde_module_unlock_working_flag(&deviceflag);
        osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);
#ifdef CONFIG_GFX_PROC_SUPPORT
        /* the statistics time of conflict and TDE is currently allocated randomly and priority setting can be
        considered. */
        GFX2D_CONFIG_StartTime(); /* CNcomment: 统计和TDE冲突的时间，目前是随机分配，可以考虑设置优先级 */
        pstTaskMgr->u32CollisionCnt++; /* the number of TDE conflicts  */ /* CNcomment: 统计和TDE冲突的次数 */
#endif
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
#endif
    pstCur = &(pstTaskMgr->pstToSubmitTask->stList);
    pstNext = pstCur->next;

#ifdef CONFIG_GFX_PROC_SUPPORT
    pstTaskMgr->u32RunTaskNum = 1;
    pstTaskMgr->u32WaitTaskNum--;
#endif

#ifndef GFX2D_SUBMIT_EVERY_TASK
    while (pstNext != (&pstTaskMgr->stListHead)) {
        pstCurTask = (GFX2D_LIST_TASK_S *)pstCur;
        pstNextTask = (GFX2D_LIST_TASK_S *)pstNext;

        if ((pstTaskMgr->pstNodeOps == HI_NULL) || (pstTaskMgr->pstNodeOps->pfnLinkNode == HI_NULL)) {
            break;
        }

        /* Hardware can process many tasks one time,requires that the tasks
        link each other by the promissory format! */
        ret = pstTaskMgr->pstNodeOps->pfnLinkNode(pstCurTask->pNode, pstNextTask->pNode);
        if (ret < 0) {
            break;
        }
#ifdef CONFIG_GFX_PROC_SUPPORT
        pstTaskMgr->u32RunTaskNum++;
        pstTaskMgr->u32WaitTaskNum--;
#endif
        pstCur = pstNext;
        pstNext = pstNext->next;
    }
#endif

    /* Submit task to hardware! */
    pstTaskMgr->pstSubmitedHeadTask = pstTaskMgr->pstToSubmitTask;
    pstTaskMgr->pstSubmitedTailTask = (GFX2D_LIST_TASK_S *)pstCur;

    if (pstNext == &(pstTaskMgr->stListHead)) {
        pstTaskMgr->pstToSubmitTask = HI_NULL;
    } else {
        pstTaskMgr->pstToSubmitTask = (GFX2D_LIST_TASK_S *)pstNext;
    }

#ifdef CONFIG_GFX_PROC_SUPPORT
    if (pstTaskMgr->u32RunTaskNum > pstTaskMgr->u32MaxTaskPerList) {
        pstTaskMgr->u32MaxTaskPerList = pstTaskMgr->u32RunTaskNum;
    }
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
    pstTaskMgr->u32LastCollisionTimeCost = GFX2D_CONFIG_EndTime();
    if (pstTaskMgr->u32LastCollisionTimeCost > pstTaskMgr->u32MaxCollisionTimeCost) {
        pstTaskMgr->u32MaxCollisionTimeCost = pstTaskMgr->u32LastCollisionTimeCost;
    }

    GFX2D_CONFIG_StartTime();
#endif

    if (pstTaskMgr->pstNodeOps && pstTaskMgr->pstNodeOps->pfnSubNode) {
        pstTaskMgr->pstNodeOps->pfnSubNode(pstTaskMgr->pstSubmitedHeadTask->dev_id,
                                           pstTaskMgr->pstSubmitedHeadTask->pNode);
    }

#ifdef GFX2D_SYNC_TDE
    g_gfx2d_tde_export_funcs->drv_tde_module_set_working_flag(HI_TRUE);
    g_gfx2d_tde_export_funcs->drv_tde_module_unlock_working_flag(&deviceflag);
#endif
    osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static GFX2D_LIST_TASK_S *GetTailTask(GFX2D_LIST_TASK_MGR_S *pstTaskMgr)
{
    hi_ulong listlockflag = 0;
    hi_ulong tasklockflag = 0;
    GFX2D_LIST_TASK_S *pstTask = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstTaskMgr, NULL);

    osal_spin_lock_irqsave(&(pstTaskMgr->lock), &listlockflag);
    if (osal_list_empty(&(pstTaskMgr->stListHead))) {
        osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &listlockflag);
        return HI_NULL;
    }

    pstTask = (GFX2D_LIST_TASK_S *)pstTaskMgr->stListHead.prev;
    osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &listlockflag);

    /* Add task ref to avoid the task is destroyed by other process! */
    osal_spin_lock_irqsave(&(pstTask->lock), &tasklockflag);
    pstTask->u32Ref++;
    osal_spin_unlock_irqrestore(&(pstTask->lock), &tasklockflag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return pstTask;
}

static hi_void ListCompIsr(hi_void)
{
    GFX2D_LIST_TASK_S *pstTask = NULL;
    GFX2D_LIST_TASK_S *pstNextTask = NULL;
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
    hi_ulong lockflag = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifdef CONFIG_GFX_PROC_SUPPORT
    pstTaskMgr->u32LastTimeCost = GFX2D_CONFIG_EndTime();
    pstTaskMgr->u32TotalTimeCost += pstTaskMgr->u32LastTimeCost;
    if (pstTaskMgr->u32LastTimeCost > pstTaskMgr->u32MaxTimeCost) {
        pstTaskMgr->u32MaxTimeCost = pstTaskMgr->u32LastTimeCost;
    }
#endif

    pstNextTask = pstTaskMgr->pstSubmitedHeadTask;
    do {
        pstTask = pstNextTask;

        /* Get the next task before wake_up,because process waked up may
        delete the task from the list! */
        osal_spin_lock_irqsave(&(pstTaskMgr->lock), &lockflag);
        pstNextTask = (GFX2D_LIST_TASK_S *)(pstTask->stList.next);
        osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);

        if (pstTaskMgr->pstNodeOps && pstTaskMgr->pstNodeOps->pfnNodeIsr) {
            pstTaskMgr->pstNodeOps->pfnNodeIsr(pstTask->pNode);
        }

        osal_spin_lock_irqsave(&(pstTask->lock), &lockflag);
        pstTask->enState = GFX2D_LIST_TASKSTATE_FINISH;
        pstTask->u32Ref++;
#ifdef GFX2D_ALPHADETECT_SUPPORT
        if (pstTask->bWait) {
            GFX2D_HAL_GetTransparent(&(pstTask->ext_info));
        }
#endif
        osal_spin_unlock_irqrestore(&(pstTask->lock), &lockflag);

#ifdef GFX2D_FENCE_SUPPORT
        if (pstTask->release_fence_fd >= 0) {
            put_input_ddr_from_task(pstTask);
            GFX2D_FENCE_WakeUp();
        }
#endif
        DeleteTask(pstTaskMgr, pstTask);

        if (pstTask->bWait) {
            UnrefTask(pstTask);
            osal_wait_wakeup(&pstTask->stWaitQueueHead);
        } else {
#ifdef CONFIG_GFX_PROC_SUPPORT
            g_gfx2d_list_debug.DestoryTaskTimesInIsrFunc++;
#endif
            DestroyTask(pstTask);
        }
#ifdef CONFIG_GFX_PROC_SUPPORT
        osal_spin_lock_irqsave(&(pstTaskMgr->lock), &lockflag);
        pstTaskMgr->u32RunTaskNum--;
        osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);
#endif
    } while (pstTask != pstTaskMgr->pstSubmitedTailTask);

    osal_spin_lock_irqsave(&(pstTaskMgr->lock), &lockflag);
    pstTaskMgr->pstSubmitedHeadTask = HI_NULL;
    pstTaskMgr->pstSubmitedTailTask = HI_NULL;
    if (pstTaskMgr->pstNodeOps && pstTaskMgr->pstNodeOps->pfnAllNodeIsr) {
        pstTaskMgr->pstNodeOps->pfnAllNodeIsr();
    }
    osal_spin_unlock_irqrestore(&(pstTaskMgr->lock), &lockflag);

#ifdef GFX2D_SYNC_TDE
    g_gfx2d_tde_export_funcs->drv_tde_module_lock_working_flag(&lockflag);
    g_gfx2d_tde_export_funcs->drv_tde_module_set_working_flag(HI_FALSE);
    g_gfx2d_tde_export_funcs->drv_tde_module_unlock_working_flag(&lockflag);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static void Gfx2dTaskletFunc(unsigned long int_status);

static void Gfx2dTaskletFunc(unsigned long int_status)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    g_Gfx2dTasklet.data &= (~int_status);

    SubmitTask(&g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC]);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

hi_s32 GFX2D_LIST_Isr(hi_s32 irq, hi_void *dev_id)
{
    hi_u32 u32Status;
#ifdef GFX2D_FENCE_SUPPORT
    GFX2D_LIST_TASK_MGR_S *task_mgr = NULL;
    hi_ulong lockflag = 0;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifdef GFX2D_SYNC_TDE
    if (g_gfx2d_tde_export_funcs == NULL) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return OSAL_IRQ_HANDLED;
    }
#endif
#ifdef CONFIG_GFX_MMU_SUPPORT
    HI_GFX_SmmuIsr("HI_MOD_GFX2D");
#endif
    u32Status = GFX2D_HAL_GetIntStatus(GFX2D_HAL_DEV_TYPE_HWC);
    if (u32Status & 0x1) {
        ListCompIsr();
    }
#ifdef GFX2D_FENCE_SUPPORT
    task_mgr = &(g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC]);
    osal_spin_lock_irqsave(&(task_mgr->submit_work_mgr.submit_task_lock), &lockflag);
    task_mgr->submit_work_mgr.need_wait_up = HI_TRUE;
    task_mgr->submit_work_mgr.to_submit_task = HI_TRUE;
    osal_spin_unlock_irqrestore(&(task_mgr->submit_work_mgr.submit_task_lock), &lockflag);
    osal_wait_wakeup(&(task_mgr->gfx2d_wait_queue));
#else
    g_Gfx2dTasklet.data = g_Gfx2dTasklet.data | ((unsigned long)u32Status);
    osal_tasklet_update(&g_Gfx2dTasklet);
    osal_tasklet_schedule(&g_Gfx2dTasklet);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return OSAL_IRQ_HANDLED;
}

static inline hi_void UnrefTask(GFX2D_LIST_TASK_S *pstTask)
{
    hi_ulong lockflag = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstTask);

    osal_spin_lock_irqsave(&(pstTask->lock), &lockflag);
    pstTask->u32Ref--;
    osal_spin_unlock_irqrestore(&(pstTask->lock), &lockflag);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 gfx2d_check_task_state(const void *param)
{
    GFX2D_LIST_TASK_S *pstTask = (GFX2D_LIST_TASK_S *)param;

    if (pstTask == HI_NULL) {
        return 0;
    }
    if (pstTask->enState == GFX2D_LIST_TASKSTATE_FINISH) {
        return 1;
    }
    return 0;
}

static hi_s32 WaitForDone(GFX2D_LIST_TASK_S *pstTask, hi_u32 time_out)
{
    hi_ulong lockflag = 0;
    hi_s32 s32Ret = HI_FAILURE;
    hi_u32 u32Jiffies = 0;
    GFX2D_LIST_TASKSTATE_E enState;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstTask, HI_FAILURE);

    if ((time_out == 0) || (time_out > 10000)) { /* 10000 algorithm data */
        u32Jiffies = 10000 * HZ / 1000;          /* 10000,1000 algorithm data */
    } else {
        u32Jiffies = time_out * HZ / 1000; /* 1000 algorithm data */
    }
    osal_spin_lock_irqsave(&(pstTask->lock), &lockflag);
    pstTask->bWait = HI_TRUE;
    osal_spin_unlock_irqrestore(&(pstTask->lock), &lockflag);

    s32Ret = osal_wait_timeout_interruptible(&pstTask->stWaitQueueHead, gfx2d_check_task_state, pstTask, u32Jiffies);
    if (s32Ret <= 0) {
        s32Ret = DRV_GFX2D_ERR_TIMEOUT;
    }

    osal_spin_lock_irqsave(&(pstTask->lock), &lockflag);
    pstTask->bWait = HI_FALSE;
    enState = pstTask->enState;
    osal_spin_unlock_irqrestore(&(pstTask->lock), &lockflag);

    if (GFX2D_LIST_TASKSTATE_FINISH == enState) {
        s32Ret = HI_SUCCESS;
    } else {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "timeout");
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return s32Ret;
}

hi_s32 GFX2D_LIST_Resume(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_u32 u32BaseAddr = 0;

    u32BaseAddr = GFX2D_HAL_GetBaseAddr(DRV_GFX2D_DEV_ID_0, GFX2D_HAL_DEV_TYPE_HWC);
    (hi_void) HI_GFX_MapSmmuReg(u32BaseAddr + 0xf000);

    return HI_GFX_InitSmmu(u32BaseAddr + 0xf000);
#else
    return HI_SUCCESS;
#endif
}

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_LIST_ReadProc(hi_void *p, hi_void *v)
{
    GFX2D_LIST_TASK_MGR_S *pstTaskMgr = &g_gfx2d_list_task_manager[DRV_GFX2D_DEV_ID_0][GFX2D_HAL_DEV_TYPE_HWC];
    // HI_UNUSED(v);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(p);

    osal_proc_print(p, "++++++++++++++++++++++++++++ GFX2D Task Info ++++++++++++++++++++++\n");
    osal_proc_print(p, "TotalTaskNum\t:%u\n", pstTaskMgr->u32TotalTaskNum);     /* * 任务数 * */
    osal_proc_print(p, "RunTaskNum\t:%u\n", pstTaskMgr->u32RunTaskNum);         /* * 正在执行的任务数     * */
    osal_proc_print(p, "WaitTaskNum\t:%u\n", pstTaskMgr->u32WaitTaskNum);       /* * 等待执行任务数       * */
    osal_proc_print(p, "MaxTaskPerList\t:%d\n", pstTaskMgr->u32MaxTaskPerList); /* * 单次执行的最大任务数 * */
    osal_proc_print(p, "TotalTimeCost\t:%uus\n", pstTaskMgr->u32TotalTimeCost); /* * 所有任务执行完的时间 * */
    osal_proc_print(p, "MaxTimeCost\t:%uus\n", pstTaskMgr->u32MaxTimeCost);     /* * 任务的最大耗时   * */
    osal_proc_print(p, "LastTimeCost\t:%uus\n", pstTaskMgr->u32LastTimeCost);   /* * 上一次任务的耗时 * */
    osal_proc_print(p, "CollisionCnt\t:%uus\n", pstTaskMgr->u32CollisionCnt);   /* * 和TDE冲突次数    * */
    osal_proc_print(p, "LastCollisionTimeCost\t:%uus\n", pstTaskMgr->u32LastCollisionTimeCost); /* 最近一次等TDE的时间 */
    /* 提交任务还没有执行完就响应中断销毁任务, 需要注意考虑这种情况的特殊处理 */
    if (g_gfx2d_list_debug.bDestoryTaskTimesInIsrFunc == HI_TRUE) {
        g_gfx2d_list_debug.bDestoryTaskTimesInIsrFunc = HI_FALSE;
        osal_proc_print(p, "Maybe exist mem abnormal\t:Call DestoryTask in isr_func\n");
    }

    return;
}
#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: struct define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef _DRV_PQ_VPSS_THREAD_H_
#define _DRV_PQ_VPSS_THREAD_H_
#include "hi_type.h"
#include "hi_osal.h"
#include "drv_pq_ext_zme.h"
#include "drv_pq_comm.h"

#define ALG_BUFFER_NUM 2

/* 共享内存结构体 */
typedef struct {
    drv_pq_mem_info mmu_buff;
    hi_bool exist;
    hi_s32 fd;
} drv_pq_buff;

typedef enum {
    HI_DRV_PQ_VPSS_API_NR = 0,
    HI_DRV_PQ_VPSS_API_DB,
    HI_DRV_PQ_VPSS_API_DM,
    HI_DRV_PQ_VPSS_API_DEI,

    HI_DRV_PQ_VPSS_API_MAX
} drv_pq_vpss_api;

typedef struct {
    drv_pq_buff vpss_in_buff[ALG_BUFFER_NUM][VPSS_HANDLE_NUM];
    drv_pq_buff vpss_out_buff[ALG_BUFFER_NUM][VPSS_HANDLE_NUM];
    hi_bool handle_enable[VPSS_HANDLE_NUM];
    hi_drv_pq_vpss_api_input *input_buf[VPSS_HANDLE_NUM];
    hi_drv_pq_vpss_api_output *output_buf[VPSS_HANDLE_NUM];
    hi_u32 alg_lost;
    hi_u32 alg_run_count;
    hi_bool is_vpss_thread_exist;
    hi_bool wake_poll_process;
    wait_queue_head_t pq_poll_wait_queue;
} drv_pq_vpss_alg_info;

/* pq param */
typedef struct {
    drv_pq_vpss_alg_info vpss_alg_info;
    hi_bool is_stop;
} drv_pq_vpss_context;

typedef enum {
    VPSS_BUF_IN = 0,
    VPSS_BUF_OUT,
    VPSS_BUF_BUTT
} drv_pq_vpss_buf_type;

typedef enum {
    PQ_VPSS_ALG_NR = 0,
    PQ_VPSS_ALG_DEI,
    PQ_VPSS_ALG_DB,
    PQ_VPSS_ALG_DM,

    PQ_VPSS_ALG_MAX
} drv_pq_vpss_alg_type;

typedef struct {
    hi_bool handle_enable;
    hi_bool is_in_exist[ALG_BUFFER_NUM];
    hi_bool is_out_exist[ALG_BUFFER_NUM];
    hi_bool is_input_alloc;
    hi_bool is_output_alloc;
    hi_bool is_input_ready;
    hi_bool is_output_ready;
    hi_u32 input_index;
    hi_u32 output_index;
    hi_bool alg_in_ready[PQ_VPSS_ALG_MAX];
    hi_bool alg_out_ready[PQ_VPSS_ALG_MAX];
} drv_pq_vpss_thread_proc;

/* PQ初始化和去初始化时调用 */
hi_s32 drv_pq_init_vpss_thread(hi_void); /* 内核态DRV_PQ使用 */
hi_void drv_pq_deinit_vpss_thread(hi_void); /* 内核态DRV_PQ使用 */

/* init vpss 调用，场景发生变化时，申请buffer，如果对应handle已经申请，则不重复申请 */
hi_s32 drv_pq_alloc_vpss_api(hi_u32 handle); /* 内核态DRV_PQ使用 */
hi_void drv_pq_free_vpss_api(hi_void); /* 内核态DRV_PQ使用 */

hi_s32 drv_pq_wakeup_thread(hi_bool released);

hi_u32 drv_pq_vpss_poll(struct file *filp, struct poll_table_struct *wait);

hi_s32 drv_pq_get_alg_thread_status(hi_bool *pb_thread_exist);
hi_s32 drv_pq_set_alg_thread_status(hi_bool b_thread_exist);

void *drv_pq_vpss_request_buffer(drv_pq_vpss_buf_type type, hi_u32 handle);
hi_s32 drv_pq_vpss_update_buffer(drv_pq_vpss_buf_type type, hi_u32 handle);

hi_s32 drv_pq_get_vpss_thread_info(hi_drv_pq_vpss_thread_info *vpss_thread_info); /* 用户态MPI_PQ使用 */

hi_s32 drv_pq_proc_get_vpss_thread_info(hi_u32 handle, drv_pq_vpss_thread_proc *vpss_thread_proc);
#endif

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: struct define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef _DRV_PQ_VDP_THREAD_H_
#define _DRV_PQ_VDP_THREAD_H_
#include "hi_type.h"
#include "hi_osal.h"
#include "drv_pq_comm.h"

typedef enum {
    HI_DRV_PQ_VDP_API_HDR = 0,
    HI_DRV_PQ_VDP_API_DC,

    HI_DRV_PQ_VDP_API_MAX
} drv_pq_vdp_api;

/* 共享内存结构体 */
typedef struct {
    drv_pq_mem_info mmu_buff;
    hi_bool exist;
    hi_s32 fd;
} drv_pq_buff_vdp;

typedef struct {
    drv_pq_buff_vdp vdp_in_buff[VDP_ALG_BUFFER_NUM];
    drv_pq_buff_vdp vdp_out_buff[VDP_ALG_BUFFER_NUM];
    hi_drv_pq_vdp_api_input *input_buf;
    hi_drv_pq_vdp_api_output *output_buf;
    hi_bool handle_enable;
    hi_bool is_vdp_thread_exist;
    hi_bool wake_poll_out_process;
    hi_u32 alg_lost;
    hi_u32 alg_run_count;
    wait_queue_head_t pq_poll_wait_out_queue;
} drv_pq_vdp_alg_info;

/* CNcomment:PQ 参数 */
typedef struct {
    drv_pq_vdp_alg_info vdp_alg_info;
    hi_bool is_stop;
} drv_pq_vdp_context;

typedef enum {
    VDP_BUF_IN,
    VDP_BUF_OUT,
    VDP_BUF_MAX
} drv_pq_vdp_buf_type;

/* PQ初始化和去初始化时调用 */
hi_s32 drv_pq_init_vdp_thread(hi_void); /* 内核态DRV_PQ使用 */
hi_void drv_pq_deinit_vdp_thread(hi_void); /* 内核态DRV_PQ使用 */

/* init vdp 调用，场景发生变化时，申请buffer，如果对应handle已经申请，则不重复申请 */
hi_s32 drv_pq_alloc_vdp_api(hi_void); /* 内核态DRV_PQ使用 */
hi_void drv_pq_free_vdp_api(hi_void); /* 内核态DRV_PQ使用 */

hi_s32 drv_pq_vdp_wakeup_thread(hi_bool released);

hi_u32 drv_pq_vdp_poll(struct file *filp, struct poll_table_struct *wait);

hi_s32 drv_pq_get_vdp_alg_thread_status(hi_bool *pb_thread_exist);
hi_s32 drv_pq_set_vdp_alg_thread_status(hi_bool b_thread_exist);

/* 使用此接口外部要处理以下标记：
    1. timing info
    2. index号和匹配问题，见参考代码
    3. 具体算法的ready标记
    4. 版本号
    5. 各个算法的使能标记
    */
void *drv_pq_vdp_request_buffer(drv_pq_vdp_buf_type type);
hi_s32 drv_pq_vdp_update_buffer(drv_pq_vdp_buf_type type);

hi_s32 drv_pq_get_vdp_thread_info(hi_drv_pq_vdp_thread_info *vdp_thread_info); /* 用户态MPI_PQ使用 */
#endif

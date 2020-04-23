/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv_rm
 * Author: sdk_video
 * Create: 2015/11/25
 */

#include "securec.h"
#include "hi_osal.h"
#include "hi_errno.h"

#include "hi_drv_rm.h"
#include "hi_drv_win.h"
#include "drv_rm.h"
#include "drv_rm_proc.h"
#include "drv_rm_comm.h"
#include "drv_rm_define.h"

static rm_drv_golbal_ctx g_st_rm_drv_golbal_ctx;
static rm_drv_path_ctx   g_st_rm_drv_path_ctx[RM_DRV_CHN_MAX_NUM + 1];
static hi_u32            g_u32_reserve;

osal_semaphore g_rm_drv_mutex;

/* 初始化上下文 */
static hi_void rm_drv_comm_de_init_path_ctx(hi_u32 rm_path_id)
{
    osal_wait_destroy(&g_st_rm_drv_path_ctx[rm_path_id].wait_queue);
    return;
}

static hi_void rm_drv_comm_de_init_golbal_ctx(hi_void)
{
    osal_wait_destroy(&g_st_rm_drv_golbal_ctx.wait_queue);
    return;
}

static hi_void rm_drv_comm_init_path_ctx(hi_u32 rm_path_id)
{
    errno_t err;

    err = memset_s(&g_st_rm_drv_path_ctx[rm_path_id], sizeof(g_st_rm_drv_path_ctx), 0, sizeof(rm_drv_path_ctx));
    if (err != EOK) {
        HI_LOG_ERR("rm_drv_comm_init_path_ctx memset_s fail!\n");
        return;
    }

    g_st_rm_drv_path_ctx[rm_path_id].rm_handle = HI_INVALID_HANDLE;
    osal_wait_init(&g_st_rm_drv_path_ctx[rm_path_id].wait_queue);
    return;
}

static hi_void rm_drv_comm_init_golbal_ctx(hi_void)
{
    hi_s32 i = 0;

    memset_s(&g_st_rm_drv_golbal_ctx, sizeof(g_st_rm_drv_golbal_ctx), 0, sizeof(rm_drv_golbal_ctx));

    g_st_rm_drv_golbal_ctx.wake_up_cnt = 0;
    g_st_rm_drv_golbal_ctx.path_recycle_cnt = 0;
    g_st_rm_drv_golbal_ctx.normal_cnt = 0;
    g_st_rm_drv_golbal_ctx.rm_path_count = 0;
    g_st_rm_drv_golbal_ctx.rm_available_win_count = RM_DRV_WIN_MAX_COUNT;
    g_st_rm_drv_golbal_ctx.wake_up_flags = HI_FALSE;
    g_st_rm_drv_golbal_ctx.waiting_event = HI_FALSE;
    g_st_rm_drv_golbal_ctx.wind_owner = HI_DRV_WIN_OWNER_BUTT;
    g_st_rm_drv_golbal_ctx.win_owner_id = 0;

    for (i = 0; i < RM_DRV_CHN_MAX_NUM; i++) {
        g_st_rm_drv_golbal_ctx.chn_create_cnt[i] = 0;
        g_st_rm_drv_golbal_ctx.chn_destroy_cnt[i] = 0;
    }

    g_u32_reserve = 0;
    osal_wait_init(&g_st_rm_drv_golbal_ctx.wait_queue);
    return;
}


/* 获取上下文 */
hi_void rm_drv_comm_get_path_ctx(hi_u32 rm_path_id, rm_drv_path_ctx **rm_drv_path_ctx_p)
{
    *rm_drv_path_ctx_p = &g_st_rm_drv_path_ctx[rm_path_id];
    return;
}

/* 获取上下文 */
hi_void rm_drv_comm_get_golbal_ctx(rm_drv_golbal_ctx **rm_drv_golbal_ctx_p)
{
    *rm_drv_golbal_ctx_p = &g_st_rm_drv_golbal_ctx;
    return;
}

/* get current time */
hi_void rm_drv_comm_get_cur_time(hi_u32 *cur_time_us_p)
{
    osal_timeval time;
    osal_get_timeofday(&time);

    *cur_time_us_p = time.tv_sec * 1000000 + time.tv_usec; /* 1000000 uesed convert second to microsecond */
    return;
}

static hi_s32 rm_drv_ctrl_put_win_event_to_list(hi_drv_rm_win_event win_event)
{
    hi_s32 ret = HI_SUCCESS;
    hi_s32 result = HI_SUCCESS;
    hi_u32 rm_path_id = 0;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    hi_drv_rm_event rm_event;

    for (rm_path_id = 0; rm_path_id < RM_DRV_CHN_MAX_NUM; rm_path_id++) {
        rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
        if (rm_drv_path_ctx_p->rm_handle != HI_INVALID_HANDLE) {
            rm_event.win_event = win_event;
            ret = rm_drv_comm_put_event(rm_drv_path_ctx_p->rm_handle, rm_drv_path_ctx_p->event_handle_p, &rm_event);
            if (ret != HI_SUCCESS) {
                HI_LOG_DBG("put event in list failed!\n");
                hi_dbg_print_u32(win_event);
                hi_dbg_print_u32(rm_path_id);
                result = HI_FAILURE;
            }

            rm_drv_path_ctx_p->list_empty = HI_FALSE;
        }
    }

    return result;
}

/* 获取空闲RM path id */
static hi_s32 rm_drv_ctrl_get_free_path_id(hi_u32 *rm_path_id_p)
{
    hi_u32 rm_path_id;
    rm_drv_path_ctx *rm_path_ctx_p = HI_NULL;

    for (rm_path_id = 0; rm_path_id < RM_DRV_CHN_MAX_NUM; rm_path_id++) {
        rm_path_ctx_p = &g_st_rm_drv_path_ctx[rm_path_id];
        if (rm_path_ctx_p->rm_handle == HI_INVALID_HANDLE) {
            *rm_path_id_p = rm_path_id;
            return HI_SUCCESS;
        }
    }

    return HI_ERR_RM_NOT_FREE_INST;
}

static hi_s32 rm_drv_ctrl_wake_up_waiting_event(hi_void)
{
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);

    if (rm_drv_golbal_ctx_p == NULL) {
        HI_LOG_ERR("para rm_drv_golbal_ctx_p is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    rm_drv_golbal_ctx_p->wake_up_flags = HI_TRUE;
    osal_wait_wakeup(&rm_drv_golbal_ctx_p->wait_queue);
    return HI_SUCCESS;
}

static hi_s32 rm_drv_ctrl_disable(hi_handle rm_handle)
{
    hi_u32 rm_path_id = 0;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;

    if (rm_handle == HI_INVALID_HANDLE) {
        HI_LOG_ERR("rm handle is invalid.\n");
        return HI_ERR_RM_INVALID_HANDLE;
    }

    if (((rm_path_id) = (rm_handle) & 0xff) >= RM_DRV_CHN_MAX_NUM) {
        HI_LOG_ERR("rm path id is over range.rm_id = 0x%08X\n", rm_path_id);
        return HI_ERR_RM_INST_OVER_RANGE;
    }

    rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
    if (rm_handle != rm_drv_path_ctx_p->rm_handle) {
        HI_LOG_ERR("src_rm_handle is not equal dest_rm_handle.\n");
        hi_err_print_h32(rm_handle);
        hi_err_print_h32(rm_drv_path_ctx_p->rm_handle);
        return HI_ERR_RM_INVALID_HANDLE;
    }

    if (rm_drv_path_ctx_p->chn_enable != HI_FALSE) {
        rm_drv_path_ctx_p->chn_enable = HI_FALSE;
    } else {
        HI_LOG_ERR("path is already disable!\n");
        hi_err_print_u32(rm_path_id);
        return HI_ERR_RM_DISABLE_ERR;
    }

    return HI_SUCCESS;
}

static hi_s32 rm_drv_ctrl_destroy(hi_handle rm_handle)
{
    hi_s32 ret;
    hi_u32 rm_path_id = 0;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;
    hi_handle tmp_rm_handle;

    if (rm_handle == HI_INVALID_HANDLE) {
        HI_LOG_ERR("rm handle is invalid.\n");
        return HI_ERR_RM_INVALID_HANDLE;
    }

    if (((rm_path_id) = (rm_handle) & 0xff) >= RM_DRV_CHN_MAX_NUM) {
        HI_LOG_ERR("rm path id is over range.rm_id = 0x%08X\n", rm_path_id);
        return HI_ERR_RM_INST_OVER_RANGE;
    }

    rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
    if (rm_handle != rm_drv_path_ctx_p->rm_handle) {
        HI_LOG_ERR("src_rm_handle is not equal dest_rm_handle.\n");
        hi_err_print_h32(rm_handle);
        hi_err_print_h32(rm_drv_path_ctx_p->rm_handle);
        return HI_ERR_RM_INVALID_HANDLE;
    }

    tmp_rm_handle = rm_drv_path_ctx_p->rm_handle;
    rm_drv_path_ctx_p->rm_handle = HI_INVALID_HANDLE;

    ret = rm_drv_comm_de_init_event_list(rm_drv_path_ctx_p->event_handle_p);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("de_init event list failed.\n");
        rm_drv_path_ctx_p->rm_handle = tmp_rm_handle;
        return HI_ERR_RM_DESTROY_ERR;
    }

    rm_drv_path_ctx_p->chn_enable = HI_FALSE;
    rm_drv_path_ctx_p->event_handle_p = HI_NULL;
    rm_drv_path_ctx_p->queue_cnt = 1;
    rm_drv_path_ctx_p->try_queue_cnt = 0;
    rm_drv_path_ctx_p->cur_pid = 0;

    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);

    rm_drv_golbal_ctx_p->rm_path_count--;
    rm_drv_golbal_ctx_p->chn_destroy_cnt[rm_path_id]++;
    return HI_SUCCESS;
}

/* avoid service crash cause path resource leak */
static hi_void rm_drv_ctrl_recycle_path_resource(hi_void)
{
    hi_s32 ret = 0;
    hi_u32 rm_path_id = 0;
    hi_u32 available_rm_path;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);

    available_rm_path = RM_DRV_CHN_MAX_NUM - rm_drv_golbal_ctx_p->rm_path_count;
    if (available_rm_path > 1) {
        return;
    }

    HI_LOG_ERR("available rm path less than two!\n");

    for (rm_path_id = 0; rm_path_id < RM_DRV_CHN_MAX_NUM; rm_path_id++) {
        rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
        if (rm_drv_path_ctx_p->rm_handle == HI_INVALID_HANDLE) {
            continue;
        }
        if (rm_drv_path_ctx_p->used_node == RM_DRV_LIST_NODE_NUM && rm_drv_path_ctx_p->free_node == 0) {
            HI_LOG_ERR("path resource leak! recycle it \n");
            hi_err_print_u32(rm_path_id);
            hi_err_print_u32(rm_drv_path_ctx_p->cur_pid);
            ret = rm_drv_ctrl_disable(rm_drv_path_ctx_p->rm_handle);
            ret |= rm_drv_ctrl_destroy(rm_drv_path_ctx_p->rm_handle);
            if (ret != HI_SUCCESS) {
                HI_LOG_ERR("path resource leak! recycle failed\n");
                hi_err_print_u32(rm_path_id);
            } else {
                rm_drv_path_ctx_p->rm_handle = HI_INVALID_HANDLE;
                rm_drv_path_ctx_p->event_handle_p = HI_NULL;
                rm_drv_path_ctx_p->chn_enable = HI_FALSE; /* default colse the channel */
                rm_drv_path_ctx_p->queue_cnt = 1;
                rm_drv_path_ctx_p->try_queue_cnt = 0;
                rm_drv_path_ctx_p->free_node = RM_DRV_LIST_NODE_NUM;
                rm_drv_path_ctx_p->used_node = 0;
                rm_drv_path_ctx_p->cur_index = 0;
                rm_drv_path_ctx_p->cur_pid = 0;
                rm_drv_golbal_ctx_p->path_recycle_cnt++;
                HI_LOG_DBG("path resource leak! recycle succeed!\n");
                hi_dbg_print_u32(rm_path_id);
            }
        }
    }

    return;
}

/* check path is idle */
static hi_void rm_drv_ctrl_check_idle_path(hi_void)
{
    hi_u32 rm_path_id = 0;
    hi_u32 cur_sys_time = 0;
    hi_u32 time_diff = 0;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;

    rm_drv_comm_get_cur_time(&cur_sys_time);

    for (rm_path_id = 0; rm_path_id < RM_DRV_CHN_MAX_NUM; rm_path_id++) {
        rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
        if (rm_drv_path_ctx_p->rm_handle != HI_INVALID_HANDLE) {
            time_diff = cur_sys_time - rm_drv_path_ctx_p->lst_query_time;
            rm_drv_path_ctx_p->cur_query_time = cur_sys_time;

            if (rm_drv_path_ctx_p->idle == HI_TRUE) {
                continue;
            }

            if (time_diff > RM_DRV_IDLE_MAX_TIME_US && rm_drv_path_ctx_p->used_node >= RM_DRV_IDLE_MAX_USED_NODE) {
                rm_drv_path_ctx_p->idle = HI_TRUE;
                hi_warn_print_u32(rm_drv_path_ctx_p->cur_pid);
                hi_warn_print_bool(rm_drv_path_ctx_p->idle);
            }
        }
    }

    return;
}

#ifdef RM_IDLE_PATH_RECYCLE_SUPPORT
/* avoid service crash cause path resource leak */
static hi_void rm_drv_ctrl_recycle_idle_path(hi_void)
{
    hi_s32 ret = 0;
    hi_u32 rm_path_id = 0;
    hi_u32 available_rm_path = 0;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);

    for (rm_path_id = 0; rm_path_id < RM_DRV_CHN_MAX_NUM; rm_path_id++) {
        rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
        if (rm_drv_path_ctx_p->rm_handle == HI_INVALID_HANDLE) {
            continue;
        }
        if (rm_drv_path_ctx_p->idle == HI_TRUE) {
            HI_LOG_ERR("path is idle! recycle it\n");
            hi_err_print_u32(rm_path_id);
            hi_err_print_u32(rm_drv_path_ctx_p->cur_pid);

            ret = rm_drv_ctrl_disable(rm_drv_path_ctx_p->rm_handle);
            if (ret != HI_SUCCESS) {
                hi_rm_err_print_call_func(rm_drv_ctrl_disable, ret);
                hi_err_print_u32(rm_path_id);
            }

            ret = rm_drv_ctrl_destroy(rm_drv_path_ctx_p->rm_handle);
            if (ret != HI_SUCCESS) {
                hi_rm_err_print_call_func(rm_drv_ctrl_destroy, ret);
                hi_err_print_u32(rm_path_id);
            } else {
                rm_drv_path_ctx_p->rm_handle = HI_INVALID_HANDLE;
                rm_drv_path_ctx_p->event_handle_p = HI_NULL;
                rm_drv_path_ctx_p->chn_enable = HI_FALSE; /* default colse the channel */
                rm_drv_path_ctx_p->queue_cnt = 1;
                rm_drv_path_ctx_p->try_queue_cnt = 0;
                rm_drv_path_ctx_p->free_node = RM_DRV_LIST_NODE_NUM;
                rm_drv_path_ctx_p->used_node = 0;
                rm_drv_path_ctx_p->cur_index = 0;
                rm_drv_path_ctx_p->cur_pid = 0;
                rm_drv_golbal_ctx_p->path_recycle_cnt++;
                HI_LOG_DBG("path resource leak! recycle succeed!\n");
                hi_dbg_print_u32(rm_path_id);
            }
        }
    }

    return;
}
#endif

hi_s32 rm_drv_ctrl_query_event(hi_handle rm_handle, hi_drv_rm_event *rm_event_p)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 rm_path_id = 0;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    hi_drv_rm_event rm_event;

    if (rm_handle == HI_INVALID_HANDLE) {
        HI_LOG_ERR("rm handle is invalid.\n");
        return HI_ERR_RM_INVALID_HANDLE;
    }

    if (((rm_path_id) = (rm_handle) & 0xff) >= RM_DRV_CHN_MAX_NUM) {
        HI_LOG_ERR("rm path id is over range.rm_id = 0x%08X\n", rm_path_id);
        return HI_ERR_RM_INST_OVER_RANGE;
    }

    rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
    if (rm_handle != rm_drv_path_ctx_p->rm_handle) {
        HI_LOG_ERR("src_rm_handle is not equal dest_rm_handle.\n", rm_handle, rm_drv_path_ctx_p->rm_handle);
        hi_err_print_h32(rm_handle);
        hi_err_print_h32(rm_drv_path_ctx_p->rm_handle);
        return HI_ERR_RM_INVALID_HANDLE;
    }

    rm_drv_path_ctx_p->try_queue_cnt++;
    rm_drv_path_ctx_p->lst_query_time = rm_drv_path_ctx_p->cur_query_time;
    rm_drv_comm_get_cur_time(&rm_drv_path_ctx_p->cur_query_time);
    rm_drv_path_ctx_p->query_time_diff = rm_drv_path_ctx_p->cur_query_time - rm_drv_path_ctx_p->lst_query_time;
    rm_drv_path_ctx_p->idle = HI_FALSE;

    if ((rm_drv_path_ctx_p->list_empty == HI_FALSE) && (rm_drv_path_ctx_p->chn_enable == HI_TRUE)) {
        ret = rm_drv_comm_get_event(rm_drv_path_ctx_p->rm_handle, rm_drv_path_ctx_p->event_handle_p, &rm_event);
        if (ret == HI_SUCCESS) {
            rm_drv_path_ctx_p->queue_cnt++;
            memcpy_s(rm_event_p, sizeof(hi_drv_rm_event), &rm_event, sizeof(hi_drv_rm_event));
        }
    }

    return ret;
}

hi_s32 hi_drv_rm_init(hi_void)
{
    hi_u32 i;
    /* 初始化上下文 */
    for (i = 0; i < RM_DRV_CHN_MAX_NUM; i++) {
        rm_drv_comm_init_path_ctx(i);
    }

    rm_drv_comm_init_golbal_ctx();
    (hi_void)rm_drv_proc_add();

    osal_sem_init(&g_rm_drv_mutex, 1);
    return HI_SUCCESS;
}
hi_void hi_drv_rm_de_init(hi_void)
{
    hi_u32 i;
    /* 初始化上下文 */
    for (i = 0; i < RM_DRV_CHN_MAX_NUM; i++) {
        rm_drv_comm_de_init_path_ctx(i);
    }

    rm_drv_comm_de_init_golbal_ctx();
    (hi_void)rm_drv_proc_del();

    osal_sem_destory(&g_rm_drv_mutex);
    return;
}

hi_s32 hi_drv_rm_create(hi_handle *rm_handle, hi_u32 pid)
{
    hi_s32 ret;
    hi_u32 rm_path_id = 0;
    hi_void *list_handle_p = HI_NULL;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;
    rm_drv_list_attr list_attr;

    hi_info_func_enter();
    if (rm_handle == NULL) {
        HI_LOG_ERR("para rm_handle is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }
    osal_sem_down(&g_rm_drv_mutex);
    ret = rm_drv_ctrl_get_free_path_id(&rm_path_id); /* 检查是否满足创建条件 */
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("get free rm id failed\n");
        osal_sem_up(&g_rm_drv_mutex);
        return HI_ERR_RM_NOT_FREE_INST;
    }
    list_attr.list_node_num = RM_DRV_LIST_NODE_NUM;
    ret = rm_drv_comm_init_event_list(&list_attr, &list_handle_p);
    if (ret == HI_SUCCESS) { /* 返回生成的handle号 */
        *rm_handle = ((HI_ID_RM << 8) | rm_path_id); /* 8 means letf sheft 8 bit */
        rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
        rm_drv_path_ctx_p->rm_handle = *rm_handle;
        rm_drv_path_ctx_p->event_handle_p = list_handle_p;
        rm_drv_path_ctx_p->chn_enable = HI_FALSE; /* default colse the channel */
        rm_drv_path_ctx_p->queue_cnt = 1;
        rm_drv_path_ctx_p->try_queue_cnt = 0;
        rm_drv_path_ctx_p->free_node = RM_DRV_LIST_NODE_NUM;
        rm_drv_path_ctx_p->used_node = 0;
        rm_drv_path_ctx_p->cur_index = 0;
        rm_drv_path_ctx_p->cur_pid = pid;
        rm_drv_path_ctx_p->list_empty = HI_TRUE;
        rm_drv_path_ctx_p->idle = HI_FALSE;
        rm_drv_path_ctx_p->cur_query_time = 0;
        rm_drv_path_ctx_p->lst_query_time = 0;
        rm_drv_path_ctx_p->query_time_diff = 0;
        rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);
        rm_drv_golbal_ctx_p->rm_path_count++;
        rm_drv_golbal_ctx_p->chn_create_cnt[rm_path_id]++;
    }
    osal_sem_up(&g_rm_drv_mutex);
    hi_info_func_exit();

    return ret;
}

hi_s32 hi_drv_rm_destroy(hi_handle rm_handle)
{
    hi_s32 ret;

    hi_info_func_enter();
    osal_sem_down(&g_rm_drv_mutex);
    ret = rm_drv_ctrl_destroy(rm_handle);
    osal_sem_up(&g_rm_drv_mutex);
    hi_info_func_exit();

    return ret;
}

hi_s32 hi_drv_rm_enable(hi_handle rm_handle)
{
    hi_u32 rm_path_id = 0;
    hi_s32 ret = HI_SUCCESS;
    rm_drv_path_ctx *rm_drv_path_ctx_p = HI_NULL;

    hi_info_func_enter();
    if (rm_handle == HI_INVALID_HANDLE) {
        HI_LOG_ERR("rm handle is invalid.\n");
        return HI_ERR_RM_INVALID_HANDLE;
    }

    if (((rm_path_id) = (rm_handle) & 0xff) >= RM_DRV_CHN_MAX_NUM) {
        HI_LOG_ERR("rm path id is over range.rm_id = 0x%08X\n", rm_path_id);
        return HI_ERR_RM_INST_OVER_RANGE;
    }

    rm_drv_comm_get_path_ctx(rm_path_id, &rm_drv_path_ctx_p);
    if (rm_handle != rm_drv_path_ctx_p->rm_handle) {
        HI_LOG_ERR("src_rm_handle is not equal dest_rm_handle.\n", rm_handle, rm_drv_path_ctx_p->rm_handle);
        hi_err_print_h32(rm_handle);
        hi_err_print_h32(rm_drv_path_ctx_p->rm_handle);
        return HI_ERR_RM_INVALID_HANDLE;
    }

    osal_sem_down(&g_rm_drv_mutex);

    if (rm_drv_path_ctx_p->chn_enable != HI_TRUE) {
        rm_drv_path_ctx_p->chn_enable = HI_TRUE;
    } else {
        HI_LOG_ERR("path is already enable!\n");
        hi_err_print_u32(rm_path_id);
        ret = HI_ERR_RM_ENABLE_ERR;
    }

    osal_sem_up(&g_rm_drv_mutex);
    hi_info_func_exit();

    return ret;
}
hi_s32 hi_drv_rm_disable(hi_handle rm_handle)
{
    hi_s32 ret;

    hi_info_func_enter();
    osal_sem_down(&g_rm_drv_mutex);
    ret = rm_drv_ctrl_disable(rm_handle);
    osal_sem_up(&g_rm_drv_mutex);
    hi_info_func_exit();
    return ret;
}

hi_s32 hi_drv_rm_query_event(hi_handle rm_handle, hi_drv_rm_event *rm_event_p)
{
    hi_s32 ret;

    hi_dbg_func_enter();
    if (rm_event_p == NULL) {
        HI_LOG_ERR("para rm_event_p is null pointer.\n");
        return HI_ERR_RM_NULL_PTR;
    }

    osal_sem_down(&g_rm_drv_mutex);
    ret = rm_drv_ctrl_query_event(rm_handle, rm_event_p);
    osal_sem_up(&g_rm_drv_mutex);
    hi_dbg_func_exit();
    return ret;
}

static hi_s32 rm_wait_condition(const void *data)
{
    if (*(hi_bool *)data == HI_TRUE) {
        return 1;
    }

    return 0;
}

hi_s32 hi_drv_rm_acquire_window_handle(hi_u32 pid)
{
    hi_u32 remain_time = 10;
    hi_s32 ret = HI_FAILURE;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    hi_info_func_enter();
    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);
    rm_drv_ctrl_check_idle_path();

#ifdef RM_IDLE_PATH_RECYCLE_SUPPORT
    osal_sem_down(&g_rm_drv_mutex)
    rm_drv_ctrl_recycle_idle_path();
    osal_sem_up(&g_rm_drv_mutex);
#endif

    if (rm_drv_golbal_ctx_p->rm_available_win_count > 0) {
        rm_drv_golbal_ctx_p->normal_cnt++;
        rm_drv_golbal_ctx_p->win_owner_id = pid;
        hi_info_func_exit();
        return HI_SUCCESS;
    } else {
        /* set waiting event flag true */
        osal_sem_down(&g_rm_drv_mutex);
        rm_drv_golbal_ctx_p->waiting_event = HI_TRUE;
        ret = rm_drv_ctrl_put_win_event_to_list(HI_DRV_RM_EVENT_WIN_LACK);
        if (ret != HI_SUCCESS) {
            HI_LOG_DBG("put event to list error!\n");
            rm_drv_ctrl_recycle_path_resource();
        }
        osal_sem_up(&g_rm_drv_mutex);
        remain_time = osal_wait_timeout_interruptible(&rm_drv_golbal_ctx_p->wait_queue, rm_wait_condition,
            &rm_drv_golbal_ctx_p->wake_up_flags, (HZ * 5)); /* (HZ * 5) means the max sleep time */
        if (remain_time > 0) { /* 大于零表示非timeout */
            rm_drv_golbal_ctx_p->wake_up_cnt++;
            rm_drv_golbal_ctx_p->wake_up_time = remain_time;
            rm_drv_golbal_ctx_p->wake_up_flags = HI_FALSE;
            rm_drv_golbal_ctx_p->waiting_event = HI_FALSE;
            rm_drv_golbal_ctx_p->win_owner_id = pid;
            hi_info_func_exit();

            return HI_SUCCESS;
        }
    }
    HI_LOG_ERR("acquire window resource failed,waiting time out.\n");
    hi_err_print_u32(remain_time);
    hi_info_func_exit();

    return HI_ERR_RM_NOT_RESOURCE;
}

hi_s32 hi_drv_rm_notify_wind_created(hi_handle win_handle)
{
    hi_s32 ret;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    hi_info_func_enter();
    osal_sem_down(&g_rm_drv_mutex);
    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);
    rm_drv_golbal_ctx_p->rm_available_win_count--;
    hi_info_print_s32(rm_drv_golbal_ctx_p->rm_available_win_count);

    ret = rm_drv_ctrl_put_win_event_to_list(HI_DRV_RM_EVENT_WIN_CREATED);
    if (ret != HI_SUCCESS) {
        HI_LOG_DBG("put event to list error!\n");
        rm_drv_ctrl_recycle_path_resource();
    }

    osal_sem_up(&g_rm_drv_mutex);
    hi_info_func_exit();

    return ret;
}

hi_s32 hi_drv_rm_notify_wind_destroyed(hi_handle win_handle)
{
    hi_s32 ret;
    rm_drv_golbal_ctx *rm_drv_golbal_ctx_p = HI_NULL;

    hi_info_func_enter();
    osal_sem_down(&g_rm_drv_mutex);

    rm_drv_comm_get_golbal_ctx(&rm_drv_golbal_ctx_p);
    rm_drv_golbal_ctx_p->rm_available_win_count++;
    hi_info_print_s32(rm_drv_golbal_ctx_p->rm_available_win_count);
    rm_drv_golbal_ctx_p->wake_up_flags = HI_FALSE;

    if (rm_drv_golbal_ctx_p->waiting_event) {
        rm_drv_ctrl_wake_up_waiting_event();
    }

    ret = rm_drv_ctrl_put_win_event_to_list(HI_DRV_RM_EVENT_WIN_DESTROYED);
    if (ret != HI_SUCCESS) {
        HI_LOG_DBG("put event to list error!\n");
        rm_drv_ctrl_recycle_path_resource();
    }

    osal_sem_up(&g_rm_drv_mutex);
    hi_info_func_exit();

    return ret;
}


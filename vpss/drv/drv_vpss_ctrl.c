/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: drv_vpss_ctrl.c source file vpss control strategy
 * Author: zhangjunyu
 * Create: 2016/06/13
 */

#ifndef __VPSS_UT__
#include "hi_drv_osal.h"
#include "hal_vpss.h"
#endif

#include "hi_drv_proc.h"
#include "vpss_version.h"
#include "vpss_comm.h"
#include "drv_vpss_ctrl.h"
#include "drv_vpss_dbg.h"
#include "drv_vpss_instance.h"
#include "vpss_policy.h"

#ifdef DPT
#include "vpss_hal_hw_buff.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEF_DBG_ON           "1"
#define DBG_NUMBER_MAX       1000

typedef hi_s32 (*proc_func_type)(vpss_ip en_ip, hi_char (*argv)[VPSS_PROC_PARAM_MAX_LENGTH]);

typedef struct {
    hi_char *cmd_name;
    proc_func_type proc_func;
} drv_vpss_proc_func;

typedef irqreturn_t (*fn_vpss_isr_call_back)(hi_s32 irq, hi_void *dev_id);
irqreturn_t vpss0_ctrl_int_service(hi_s32 irq, hi_void *dev_id);
irqreturn_t vpss1_ctrl_int_service(hi_s32 irq, hi_void *dev_id);

typedef enum {
    VPSS_TASK_STATE_IDLE = 0,
    VPSS_TASK_STATE_PREPARE,
    VPSS_TASK_STATE_CONFIG,
    VPSS_TASK_STATE_START,
    VPSS_TASK_STATE_WAIT,
    VPSS_TASK_STATE_SUCCESS,
    VPSS_TASK_STATE_CLEAR,
    VPSS_TASK_STATE_MAX
} vpss_task_state;

typedef struct {
    vpss_task_state state;
    hi_u32 task_inst_num;
    vpss_instance *instance[VPSS_INSTANCE_MAX_NUM];
    hi_u64 check_task_time;
    hi_u64 check_task_time_end;
    hi_u64 prepare_task_time;
    hi_u64 prepare_task_time_end;
    hi_u64 config_task_time;
    hi_u64 config_task_time_end;
    hi_u64 start_task_time;
    hi_u64 start_task_time_end;
    hi_u64 update_task_time;
    hi_u64 get_isrint_time;
    hi_u64 complete_task_time;
    hi_u64 complete_task_time_end;
    hi_u64 clear_task_time;
    hi_u64 clear_task_time_end;
} vpss_task;

typedef struct {
    vpss_comm_spin list_lock;
    hi_u32 target;
    hi_u32 instance_num;
    vpss_instance *inst_pool[VPSS_INSTANCE_MAX_NUM];
    hi_u32 check_cnt;
    hi_u32 check_suc_cnt;
    hi_u32 check_rate;
    hi_u32 check_suc_rate;
    hi_u64 last_check_time;
} vpss_ctrl_inst;

typedef struct {
    hi_bool ip_vaild;
    hi_bool suspend;
    hi_bool in_mce;
    hi_bool logic_work;
    hi_bool vpss_frame_arrival;
    hi_bool vpss_frame_back;
    vpss_ip ip;
    hi_u32 vpss_irq_num;
    hi_handle h_vpss_irq;
    hi_char isr_name[VPSS_FILE_NAMELENGTH];
    fn_vpss_isr_call_back isr_process;
    hi_s32 is_vpss_open;
    vpss_comm_event task_next;
    vpss_comm_event new_task;
    vpss_comm_spin spin_lock;
    vpss_task task[VPSS_TASK_MAX_NUM];
    osal_task *h_thread;
    volatile hi_u32 thread_pos;
    volatile hi_u32 thread_sleep;
    vpss_ctrl_inst inst_ctrl_info;
    vpss_debug_info global_debug_info[VPSS_IP_MAX * VPSS_INSTANCE_MAX_NUM];
    hi_u32 clear_task_cnt;
    /* add support policy */
    hi_bool support_policy_func[VPSS_POLICY_SUPPORT_MAX];
} vpss_ctrl;

static vpss_ctrl g_vpss_ctrl[VPSS_IP_MAX] = {
    {
        .ip_vaild = HI_TRUE,
        .ip = VPSS_IP_0,
        .vpss_irq_num = VPSS0_IRQ_NUM,
        .isr_process = vpss0_ctrl_int_service,
        .isr_name = "int_vpss0",
        .is_vpss_open = 0,
        .vpss_frame_arrival = HI_FALSE,
        .vpss_frame_back = HI_FALSE,
    },

    {
        .ip_vaild = HI_FALSE,
        .ip = VPSS_IP_1,
        .vpss_irq_num = VPSS1_IRQ_NUM,
        .isr_process = vpss1_ctrl_int_service,
        .isr_name = "int_vpss1",
        .is_vpss_open = 0,
        .vpss_frame_arrival = HI_FALSE,
        .vpss_frame_back = HI_FALSE,
    }
};

#define vpss_ctrl_get_ctx(ip, ptr)                      \
    do {                                                \
        if ((ip) >= VPSS_IP_MAX) {                      \
            vpss_error("invalid vpss ip %d !\n", (ip)); \
            return HI_FAILURE;                          \
        }                                               \
        (ptr) = &g_vpss_ctrl[ip];                       \
    } while (0)

hi_void vpss_ctrl_wake_up_thread(hi_void)
{
    if (g_vpss_ctrl[VPSS_IP_0].is_vpss_open >= 1) {
        vpss_comm_give_event(&(g_vpss_ctrl[VPSS_IP_0].new_task), 1, 0);
    }

    if (g_vpss_ctrl[VPSS_IP_1].is_vpss_open >= 1) {
        vpss_comm_give_event(&(g_vpss_ctrl[VPSS_IP_1].new_task), 1, 0);
    }
}

hi_s32 vpss_ctrl_pause(vpss_ip vpss_ip)
{
    hi_u32 wait_time = 0;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(vpss_ip, vpss_ctrl);
    vpss_ctrl->thread_sleep = VPSS_THREAD_SLEEP;

    while (vpss_ctrl->thread_pos != VPSS_THREAD_POS_IDLE) {
        osal_msleep_uninterruptible(5);      /* sleep 5 ms */
        wait_time += 5; /* wait time add 5 */

        if (wait_time > VPSS_WAIT_MAX_TIME_MS) {
            vpss_error("vpss_ctrl_pause waittime ms=%d\n", wait_time);
            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_resume(vpss_ip vpss_ip)
{
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(vpss_ip, vpss_ctrl);
    vpss_ctrl->thread_sleep = VPSS_THREAD_UNSLEEP;
    vpss_comm_give_event(&vpss_ctrl->new_task, 1, 0);
    return HI_SUCCESS;
}

hi_void vpss_ctrl_suspend_inst(vpss_instance *instance)
{
    hi_u32 wait_time = 0;
    instance->suspend = HI_TRUE;

    while ((instance->state != INSTANCE_STATE_IDLE) || (instance->prepareing == HI_TRUE)) {
        osal_msleep_uninterruptible(5);      /* sleep 5 ms */
        wait_time += 5; /* wait time add 5 */

        if (wait_time > VPSS_WAIT_MAX_TIME_MS) {
            vpss_error("vpss_ctrl_wait_inst_idle ERROR state = %d,waittime ms=%d\n",
                       instance->state, wait_time);
            break;
        }
    }
}

hi_void vpss_ctrl_resume_inst(vpss_instance *instance)
{
    instance->suspend = HI_FALSE;
}

hi_s32 vpss_ctrl_update_task_success(vpss_ip ip)
{
    hi_u32 task_id;
    unsigned long flag;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);

    for (task_id = 0; task_id < VPSS_TASK_MAX_NUM; task_id++) {
        if (vpss_ctrl->task[task_id].state == VPSS_TASK_STATE_WAIT) {
            vpss_ctrl->task[task_id].state = VPSS_TASK_STATE_SUCCESS;
            vpss_ctrl->task[task_id].get_isrint_time = vpss_comm_get_sched_time();
            vpss_hal_complete_logic(ip);
            vpss_ctrl->logic_work = HI_FALSE;
            break;
        }
    }

    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);
    return HI_SUCCESS;
}
hi_s32 vpss_ctrl_process_interrupt(vpss_ip ip)
{
    hi_s32 ret;
    ret = vpss_hal_process_interrupt(ip);
    if (ret == HI_SUCCESS) {
        vpss_ctrl_update_task_success(VPSS_IP_0);
        vpss_comm_give_event(&(g_vpss_ctrl[VPSS_IP_0].task_next), EVENT_DONE, EVENT_UNDO);
    }

    return ret;
}

irqreturn_t vpss0_ctrl_int_service(hi_s32 irq, hi_void *dev_id)
{
    vpss_ctrl_process_interrupt(VPSS_IP_0);
    return IRQ_HANDLED;
}
irqreturn_t vpss1_ctrl_int_service(hi_s32 irq, hi_void *dev_id)
{
    vpss_error("vpss logic error, irq  = %x \n", irq);
    return IRQ_HANDLED;
}

hi_s32 vpss_ctrl_init_inst_list(vpss_ip ip)
{
    hi_u32 count;
    vpss_ctrl_inst *ins_list = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    ins_list = &vpss_ctrl->inst_ctrl_info;
    if (vpss_comm_init_spin(&(ins_list->list_lock)) != HI_SUCCESS) {
        vpss_error("Init list_lock spin failed.\n");
        return HI_FAILURE;
    }

    ins_list->target = 0;
    ins_list->instance_num = 0;

    for (count = 0; count < VPSS_INSTANCE_MAX_NUM; count++) {
        ins_list->inst_pool[count] = HI_NULL;
    }

    return HI_SUCCESS;
}
vpss_instance *vpss_ctrl_get_instance(hi_handle h_vpss)
{
    hi_u32 i;
    hi_u32 ip_pos;
    vpss_ctrl_inst *inst_ctrl_info = HI_NULL;
    vpss_instance *target = HI_NULL;
    unsigned long lock_flag;
    target = HI_NULL;

    if (h_vpss >= VPSS_INSTANCE_MAX_NUM * VPSS_IP_MAX) {
        vpss_error("invalid VPSS HANDLE %u.\n", h_vpss);
        return HI_NULL;
    }

    for (ip_pos = VPSS_IP_0; ip_pos < VPSS_IP_MAX; ip_pos++) {
        inst_ctrl_info = &(g_vpss_ctrl[ip_pos].inst_ctrl_info);
        if (g_vpss_ctrl[ip_pos].ip_vaild != HI_TRUE) {
            continue;
        }

        vpss_comm_down_spin(&(inst_ctrl_info->list_lock), &lock_flag);
        for (i = 0; i < VPSS_INSTANCE_MAX_NUM; i++) {
            if (inst_ctrl_info->inst_pool[i] == HI_NULL) {
                continue;
            }

            if (h_vpss != inst_ctrl_info->inst_pool[i]->id) {
                continue;
            }

            target = inst_ctrl_info->inst_pool[i];
            break;
        }
        vpss_comm_up_spin(&(inst_ctrl_info->list_lock), &lock_flag);
    }

    return target;
}

hi_s32 vpss_ctrl_regist_isr(vpss_ip ip)
{
#ifndef __VPSS_UT__
    hi_s32 ret;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    ret = osal_irq_request(vpss_ctrl->vpss_irq_num, (osal_irq_handler)vpss_ctrl->isr_process,
        HI_NULL, vpss_ctrl->isr_name, &vpss_ctrl->h_vpss_irq);
    if (ret != HI_SUCCESS) {
        vpss_error("VPSS%d registe IRQ failed!\n", (hi_u32)ip);
        return HI_FAILURE;
    } else {
        return HI_SUCCESS;
    }
#else
    return HI_SUCCESS;
#endif
}

hi_s32 vpss_ctrl_un_regist_isr(vpss_ip ip)
{
#ifndef __VPSS_UT__
    vpss_ctrl *vpss_ctrl = HI_NULL;

    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    osal_irq_free(vpss_ctrl->vpss_irq_num, &vpss_ctrl->h_vpss_irq);
    return HI_SUCCESS;
#else
    return HI_SUCCESS;
#endif
}

hi_s32 vpss_ctrl_get_distribute_ip(vpss_ip *pen_vpss_ip)
{
    hi_u32 min_inst_count = VPSS_INSTANCE_MAX_NUM;
    hi_s32 ret;
    hi_u32 i;

    for (i = 0; i < VPSS_IP_MAX; i++) {
        if ((g_vpss_ctrl[i].ip_vaild == HI_TRUE) &&
            (min_inst_count > g_vpss_ctrl[i].inst_ctrl_info.instance_num)) {
            min_inst_count = g_vpss_ctrl[i].inst_ctrl_info.instance_num;
            *pen_vpss_ip = i;
            ret = HI_SUCCESS;
        }

        vpss_dbg("g_st_vpss_ctrl[%d].st_inst_ctrl_info.instance_num=%d\n", i,
                 g_vpss_ctrl[i].inst_ctrl_info.instance_num);
    }

    return ret;
}

hi_s32 vpss_ctrl_distribution_task(vpss_ip ip, vpss_task **pre_task, vpss_task **config_task)
{
    hi_u32 task_id;
    unsigned long flag;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
    for (task_id = 0; task_id < VPSS_TASK_MAX_NUM; task_id++) {
        if ((vpss_ctrl->task[task_id].state == VPSS_TASK_STATE_WAIT) ||
            (vpss_ctrl->task[task_id].state == VPSS_TASK_STATE_SUCCESS)) {
            *pre_task = &vpss_ctrl->task[task_id];
            *config_task = &vpss_ctrl->task[!task_id];
            break;
        }
    }
    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);

    return HI_SUCCESS;
}
static hi_void vpss_ctrl_check_performance(vpss_instance *instance, vpss_ctrl_inst *inst_ctrl_info)
{
    inst_ctrl_info->check_cnt++;

    if (jiffies - inst_ctrl_info->last_check_time > HZ) {
        inst_ctrl_info->check_rate = inst_ctrl_info->check_cnt;
        inst_ctrl_info->check_suc_rate = inst_ctrl_info->check_suc_cnt;
        inst_ctrl_info->last_check_time = jiffies;
        inst_ctrl_info->check_cnt = 0;
        inst_ctrl_info->check_suc_cnt = 0;
    }

    if (jiffies - instance->perfor.check_dfx.last_check_time > HZ) {
        instance->perfor.check_dfx.try_rate = instance->perfor.check_dfx.try_cnt;
        instance->perfor.check_dfx.try_suc_rate = instance->perfor.check_dfx.try_suc_cnt;
        instance->perfor.buf_dfx.try_rate = instance->perfor.buf_dfx.try_cnt;
        instance->perfor.buf_dfx.try_suc_rate = instance->perfor.buf_dfx.try_suc_cnt;
        instance->perfor.img_dfx.get_try_rate = instance->perfor.img_dfx.get_try_cnt;
        instance->perfor.img_dfx.get_suc_rate = instance->perfor.img_dfx.get_suc_cnt;
        instance->perfor.check_dfx.last_check_time = jiffies;
        instance->perfor.check_dfx.try_cnt = 0;
        instance->perfor.check_dfx.try_suc_cnt = 0;
        instance->perfor.buf_dfx.try_cnt = 0;
        instance->perfor.buf_dfx.try_suc_cnt = 0;
        instance->perfor.img_dfx.get_try_cnt = 0;
        instance->perfor.img_dfx.get_suc_cnt = 0;
    }
}

vpss_instance *vpss_ctrl_check_find_available_instance(vpss_ip ip)
{
    hi_bool inst_available = HI_FALSE;
    hi_u32 cycle_time = 0;
    hi_u32 cur_pos;
    unsigned long lock_flag;
    vpss_ctrl_inst *inst_ctrl_info = HI_NULL;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_frame_status frame_status;
    unsigned long flag;

    /* bstart=true;state=idle;in buffer has frame;out buffer has empty frame */
    vpss_ctrl = &g_vpss_ctrl[ip];
    inst_ctrl_info = &vpss_ctrl->inst_ctrl_info;
    cur_pos = inst_ctrl_info->target;

    while (cycle_time < VPSS_INSTANCE_MAX_NUM) {
        vpss_comm_down_spin(&(inst_ctrl_info->list_lock), &lock_flag);
        instance = inst_ctrl_info->inst_pool[cur_pos];
        vpss_comm_up_spin(&(inst_ctrl_info->list_lock), &lock_flag);
        cur_pos = (cur_pos + 1) % VPSS_INSTANCE_MAX_NUM;
        cycle_time++;

        if (instance == HI_NULL) {
            continue;
        }

        vpss_ctrl_check_performance(instance, inst_ctrl_info);
        instance->perfor.check_dfx.task_time_start = vpss_comm_get_sched_time();
        if ((instance->perfor.check_dfx.task_time_start - instance->perfor.check_dfx.last_task_time)
            >= VPSS_THREAD_TIMEOUT_NS) {
            instance->perfor.cnt_dfx.time_out_count++;
        }

        instance->perfor.check_dfx.last_task_time = instance->perfor.check_dfx.task_time_start;
        instance->prepareing = HI_TRUE;
        inst_available = vpss_inst_check_inst_available(instance, &frame_status);

        vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
        vpss_ctrl->vpss_frame_arrival = frame_status.vpss_frame_arrival;
        vpss_ctrl->vpss_frame_back = frame_status.vpss_frame_back;
        vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);

        instance->perfor.check_dfx.task_time_end = vpss_comm_get_sched_time();
        if (inst_available != HI_TRUE) {
            instance->prepareing = HI_FALSE;
            continue;
        }

        inst_ctrl_info->check_suc_cnt++;
        inst_ctrl_info->target = cur_pos;
        vpss_ctrl_wake_up_thread();
        return instance;
    }

    return HI_NULL;
}

hi_u32 vpss_ctrl_check_task(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    unsigned long flag;
    hi_u32 available_num;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl_info = HI_NULL;

    task->check_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl_info);
    available_num = 0;

    for (i = 0; i < vpss_ctrl_info->inst_ctrl_info.instance_num; i++) {
        instance = vpss_ctrl_check_find_available_instance(ip);
        if (instance != HI_NULL) {
            task->instance[available_num] = instance;
            vpss_inst_update_state(instance, INSTANCE_STATE_PREPARE);
            available_num++;
        }
    }

    task->task_inst_num = available_num;
    if (task->task_inst_num != 0) {
        vpss_comm_down_spin(&vpss_ctrl_info->spin_lock, &flag);
        task->state = VPSS_TASK_STATE_PREPARE;
        vpss_comm_up_spin(&vpss_ctrl_info->spin_lock, &flag);
    }

    task->check_task_time_end = vpss_comm_get_sched_time();

    return HI_SUCCESS;
}

hi_u32 vpss_ctrl_prepare_task(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    hi_s32 ret;
    hi_bool prepare_success = HI_FALSE;
    unsigned long flag;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    task->prepare_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    prepare_success = HI_TRUE;

    for (i = 0; i < task->task_inst_num; i++) {
        instance = task->instance[i];
        if (instance == HI_NULL) {
            vpss_error("can't run here, i %d task num %d\n", i, task->task_inst_num);
            continue;
        }

        instance->perfor.prepare_dfx.task_time_start = vpss_comm_get_sched_time();
        ret = vpss_inst_prepare_task(instance);
        instance->perfor.prepare_dfx.task_time_end = vpss_comm_get_sched_time();
        if (ret != HI_SUCCESS) {
            prepare_success = HI_FALSE;
            break;
        }
    }

    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
    if (prepare_success == HI_TRUE) {
        task->state = VPSS_TASK_STATE_CONFIG;
    } else {
        task->state = VPSS_TASK_STATE_CLEAR;
    }
    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);
    task->prepare_task_time_end = vpss_comm_get_sched_time();

    return HI_SUCCESS;
}

hi_u32 vpss_ctrl_config_task(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    hi_s32 ret;
    hi_bool config_success = HI_FALSE;
    unsigned long flag;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    task->config_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    config_success = HI_TRUE;

    for (i = 0; i < task->task_inst_num; i++) {
        instance = task->instance[i];
        if (instance == HI_NULL) {
            vpss_error("can't run here, i %d task num %d\n", i, task->task_inst_num);
            continue;
        }

        instance->perfor.config_dfx.task_time_start = vpss_comm_get_sched_time();
        ret = vpss_inst_config_task(instance);
        instance->perfor.config_dfx.task_time_end = vpss_comm_get_sched_time();
        if (ret != HI_SUCCESS) {
            config_success = HI_FALSE;
            break;
        }
    }

    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
    if (config_success == HI_TRUE) {
        task->state = VPSS_TASK_STATE_START;
    } else {
        task->state = VPSS_TASK_STATE_CLEAR;
    }
    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);

    task->config_task_time_end = vpss_comm_get_sched_time();
    return HI_SUCCESS;
}
hi_u32 vpss_ctrl_start_task(vpss_ip ip, vpss_task *task)
{
    unsigned long flag;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    hi_s32 ret = HI_SUCCESS;
    task->start_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);

    if (vpss_ctrl->logic_work == HI_FALSE) {
        ret = vpss_hal_start_logic(ip);
        if (ret == HI_SUCCESS) {
            vpss_ctrl->logic_work = HI_TRUE;
        }

        task->state = VPSS_TASK_STATE_WAIT;
    }

    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);
    task->start_task_time_end = vpss_comm_get_sched_time();
#ifdef __VPSS_UT__
    task->state = VPSS_TASK_STATE_SUCCESS;
    vpss_hal_complete_logic(ip);
    vpss_ctrl->logic_work = HI_FALSE;
#endif
    return ret;
}
hi_u32 vpss_ctrl_update_task(vpss_ip ip, vpss_task *task)
{
    hi_u64 wait_time;
    unsigned long flag;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    task->update_task_time = vpss_comm_get_sched_time();
    wait_time = task->update_task_time - task->start_task_time_end;
    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);

    if (wait_time > VPSS_LOGIC_TIMEOUT_NS) {
        vpss_error("task timeout %lld, clear task\n", wait_time);
        task->state = VPSS_TASK_STATE_CLEAR;
    }

    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);
    return HI_SUCCESS;
}
hi_u32 vpss_ctrl_clear_task(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    unsigned long flag;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    task->clear_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    for (i = 0; i < task->task_inst_num; i++) {
        instance = task->instance[i];
        if (instance == HI_NULL) {
            vpss_error("can't run here, i %d task num %d\n", i, task->task_inst_num);
            continue;
        }

        if (instance->global_dbg_ctrl->dump_reg) {
            vpss_hal_dump_reg(ip);
        }

        instance->logic_work = vpss_ctrl->logic_work;
        vpss_ctrl->clear_task_cnt++;
        vpss_inst_clear_task(instance);
    }

    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
    task->state = VPSS_TASK_STATE_IDLE;
    task->task_inst_num = 0;

    for (i = 0; i < VPSS_INSTANCE_MAX_NUM; i++) {
        task->instance[i] = HI_NULL;
    }

    vpss_hal_complete_logic(ip);
    vpss_ctrl->logic_work = HI_FALSE;
    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);
    task->clear_task_time_end = vpss_comm_get_sched_time();

    return HI_SUCCESS;
}
hi_void vpss_inst_calc_run_time(vpss_instance *instance)
{
    hi_u32 i;

    instance->proc_perfor.proc_check_time = instance->perfor.check_dfx.task_time_end
        - instance->perfor.check_dfx.task_time_start;
    instance->proc_perfor.proc_prepare_time = instance->perfor.prepare_dfx.task_time_end
        - instance->perfor.prepare_dfx.task_time_start;
    instance->proc_perfor.proc_config_time = instance->perfor.config_dfx.task_time_end
        - instance->perfor.config_dfx.task_time_start;
    instance->proc_perfor.proc_start_time = instance->perfor.start_dfx.task_time_end
        - instance->perfor.start_dfx.task_time_start;

    instance->proc_perfor.proc_complete_send_time = instance->perfor.complete_dfx.send_out_time_end
        - instance->perfor.complete_dfx.send_out_time_start;

    for (i = 0; i < VPSS_PORT_MAX_NUM; i++) {
        instance->proc_perfor.proc_complete_send1_time[i] = instance->perfor.complete_dfx.send_out_end1[i]
                - instance->perfor.complete_dfx.send_out_start[i];
        instance->proc_perfor.proc_complete_send2_time[i] = instance->perfor.complete_dfx.send_out_end2[i]
                - instance->perfor.complete_dfx.send_out_end1[i];
        instance->proc_perfor.proc_complete_send3_time[i] = instance->perfor.complete_dfx.send_out_end3[i]
                - instance->perfor.complete_dfx.send_out_end2[i];
        instance->proc_perfor.proc_complete_send_q_time[i] = instance->perfor.complete_dfx.send_out_q_end4[i]
                - instance->perfor.complete_dfx.send_out_end3[i];
    }

    instance->proc_perfor.proc_complete_rel_time = instance->perfor.complete_dfx.rel_src_time_end
        - instance->perfor.complete_dfx.rel_src_time_start;
    instance->proc_perfor.proc_complete_complete_time = instance->perfor.complete_dfx.complete_time_end
        - instance->perfor.complete_dfx.complete_time_start;

    instance->proc_perfor.proc_complete_time = instance->perfor.complete_dfx.task_time_end
        - instance->perfor.complete_dfx.task_time_start;
    instance->proc_perfor.proc_total_time = instance->perfor.complete_dfx.task_time_end
        - instance->perfor.prepare_dfx.task_time_start;

    instance->proc_perfor.proc_pq_stt_time = instance->perfor.pq_dfx.stt_time_end
        - instance->perfor.pq_dfx.stt_time_start;
    instance->proc_perfor.proc_fence_time = instance->perfor.fence_dfx.time_end
        - instance->perfor.fence_dfx.time_start;
}

hi_u32 vpss_ctrl_complete_task(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    unsigned long flag;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    task->complete_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    for (i = 0; i < task->task_inst_num; i++) {
        instance = task->instance[i];
        if (instance == HI_NULL) {
            vpss_error("can't run here, i %d task num %d\n", i, task->task_inst_num);
            continue;
        }

        instance->perfor.complete_dfx.task_time_start = vpss_comm_get_sched_time();
        vpss_inst_complete_task(instance);
        instance->perfor.complete_dfx.task_time_end = vpss_comm_get_sched_time();
        vpss_inst_calc_run_time(instance);
        instance->proc_perfor.proc_logic_time = task->get_isrint_time - task->start_task_time_end;
    }

    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
    task->state = VPSS_TASK_STATE_IDLE;
    task->task_inst_num = 0;

    for (i = 0; i < VPSS_INSTANCE_MAX_NUM; i++) {
        task->instance[i] = HI_NULL;
    }

    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);
    task->complete_task_time_end = vpss_comm_get_sched_time();
    /* todo */
    vpss_hal_golden_write_chn_file();

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_advance_send_frame(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    task->complete_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    for (i = 0; i < task->task_inst_num; i++) {
        instance = task->instance[i];
        if (instance == HI_NULL) {
            vpss_error("can't run here, i %d task num %d\n", i, task->task_inst_num);
            continue;
        }

        if (instance->hal_info.out_wtunl_en == HI_TRUE) {
            vpss_inst_send_frame(instance);
        }
    }

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_advance_release_frame(vpss_ip ip, vpss_task *task)
{
    hi_u32 i;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    task->complete_task_time = vpss_comm_get_sched_time();
    vpss_ctrl_get_ctx(ip, vpss_ctrl);

    for (i = 0; i < task->task_inst_num; i++) {
        instance = task->instance[i];
        if (instance == HI_NULL) {
            vpss_error("can't run here, i %d task num %d\n", i, task->task_inst_num);
            continue;
        }

        if (instance->hal_info.in_wtunl_en == HI_TRUE) {
            vpss_inst_release_frame(instance);
        }
    }

    return HI_SUCCESS;
}

hi_void vpss_ctrl_process_task(vpss_ip ip, vpss_task *task)
{
    if (task->state == VPSS_TASK_STATE_IDLE) {
        vpss_ctrl_check_task(ip, task);
    }

    if (task->state == VPSS_TASK_STATE_PREPARE) {
        vpss_ctrl_prepare_task(ip, task);
    }

    if (task->state == VPSS_TASK_STATE_CONFIG) {
        vpss_ctrl_config_task(ip, task);
    }

    if (task->state == VPSS_TASK_STATE_START) {
        vpss_ctrl_start_task(ip, task);
#ifdef DPT
        /* in tunnel process */
        vpss_ctrl_advance_send_frame(ip, task);
        /* out tunnel process */
        vpss_ctrl_advance_release_frame(ip, task);
#endif
    }

    if (task->state == VPSS_TASK_STATE_WAIT) {
        vpss_ctrl_update_task(ip, task);
    }

    if (task->state == VPSS_TASK_STATE_SUCCESS) {
        vpss_ctrl_complete_task(ip, task);
    }

    if (task->state == VPSS_TASK_STATE_CLEAR) {
        vpss_ctrl_clear_task(ip, task);
    }
}

static hi_void vpss_ctrl_thread_process_idle(vpss_ctrl *vpss_ctrl, vpss_task *config_task, vpss_task *pre_task)
{
    hi_s32 ret = OSAL_ERR;

    vpss_ctrl->thread_pos = VPSS_THREAD_POS_IDLE;

    if ((config_task->state == VPSS_TASK_STATE_IDLE) && (pre_task->state == VPSS_TASK_STATE_IDLE)) {
        /* sleep condition:  1.No input frame 2.No output buffer 3.No instance 4.No task to start. */
        if ((vpss_ctrl->vpss_frame_arrival == HI_FALSE) || (vpss_ctrl->vpss_frame_back == HI_FALSE) ||
            (vpss_ctrl->inst_ctrl_info.instance_num == 0) || (vpss_ctrl->inst_ctrl_info.check_suc_cnt == 0)) {
            ret = vpss_comm_wait_event(&(vpss_ctrl->new_task), VPSS_THREAD_WAIT_IDLE_MS);
        }
    }

    if (((config_task->state == VPSS_TASK_STATE_START) && (pre_task->state == VPSS_TASK_STATE_WAIT)) ||
        ((pre_task->state == VPSS_TASK_STATE_START) && (config_task->state == VPSS_TASK_STATE_WAIT))) {
        ret = vpss_comm_wait_event(&(vpss_ctrl->new_task), 1); /* timeout is 1ms */
    } else {
        if ((pre_task->state == VPSS_TASK_STATE_WAIT) || (config_task->state == VPSS_TASK_STATE_WAIT)) {
            ret = vpss_comm_wait_event(&(vpss_ctrl->new_task), 1); /* timeout is 1ms */
        }
    }

    if (ret == OSAL_OK) {
        vpss_dbg("wake_up_thread success.\n");
    }

    vpss_comm_reset_event(&(vpss_ctrl->new_task), EVENT_UNDO, EVENT_UNDO);
}

static hi_s32 vpss_ctrl_init_event(vpss_ctrl *vpss_ctrl_info)
{
    if (vpss_comm_init_event(&(vpss_ctrl_info->task_next), EVENT_UNDO, EVENT_UNDO) != HI_SUCCESS) {
        vpss_error("Init event failed.\n");
        return HI_FAILURE;
    }

    if (vpss_comm_init_event(&(vpss_ctrl_info->new_task), EVENT_UNDO, EVENT_UNDO) != HI_SUCCESS) {
        vpss_comm_destory_event(&(vpss_ctrl_info->task_next));
        vpss_error("Init event failed.\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_thread_process(hi_void *arg)
{
    hi_s32 ret = OSAL_ERR;
    vpss_ip ip;
    vpss_task *config_task = HI_NULL;
    vpss_task *pre_task = HI_NULL;
    vpss_ctrl *vpss_ctrl_info = HI_NULL;
#ifndef __VPSS_UT__
    ip = *(vpss_ip *)arg;
#else
    ip = (vpss_ip)arg;
#endif
    vpss_ctrl_get_ctx(ip, vpss_ctrl_info);
    vpss_ctrl_info->thread_pos = VPSS_THREAD_POS_INIT;
    vpss_ctrl_info->logic_work = HI_FALSE;
    pre_task = &vpss_ctrl_info->task[0];
    config_task = &vpss_ctrl_info->task[1];
    if (vpss_ctrl_init_event(vpss_ctrl_info) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    while (osal_kthread_should_stop() != HI_TRUE) {
        vpss_ctrl_info->thread_pos = VPSS_THREAD_POS_WORK;

        if (vpss_ctrl_info->thread_sleep == VPSS_THREAD_SLEEP) {
            goto VPSS_THREAD_IDLE;
        }

        ret = vpss_ctrl_distribution_task(ip, &pre_task, &config_task);
        if (ret != HI_SUCCESS) {
            vpss_error("vpss distribution task failed\n");
        }

        vpss_ctrl_process_task(ip, config_task);
        vpss_ctrl_process_task(ip, pre_task);
    VPSS_THREAD_IDLE:
        vpss_ctrl_thread_process_idle(vpss_ctrl_info, config_task, pre_task);
    }

    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_notify_frame_status(hi_handle h_vpss, vpss_frame_state frame_state)
{
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    unsigned long flag;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("hVpss %d is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_ctrl_get_ctx(instance->ctrl_id, vpss_ctrl);
    vpss_comm_down_spin(&vpss_ctrl->spin_lock, &flag);
    if (frame_state == VPSS_FRAME_STATUS_BACK) {
        vpss_ctrl->vpss_frame_back = HI_TRUE;
    } else if (frame_state == VPSS_FRAME_STATUS_ARRIVAL) {
        vpss_ctrl->vpss_frame_arrival = HI_TRUE;
    } else {
        vpss_error("frame_state is %d, not correct\n", frame_state);
    }

    vpss_comm_up_spin(&vpss_ctrl->spin_lock, &flag);

    return HI_SUCCESS;
}


hi_s32 vpss_ctrl_create_thread(vpss_ip ip)
{
#ifndef __VPSS_UT__
    vpss_ctrl *vpss_ctrl = HI_NULL;

    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    vpss_ctrl->thread_sleep = VPSS_THREAD_UNSLEEP;

    /* osal create include create & wakeup */
    vpss_ctrl->h_thread = osal_kthread_create(vpss_ctrl_thread_process, &vpss_ctrl->ip, "hi_vpss_process", 0);
    if (IS_ERR(vpss_ctrl->h_thread)) {
        vpss_error("can not create thread.\n");
        return HI_FAILURE;
    }

    osal_kthread_set_priority(vpss_ctrl->h_thread, OSAL_TASK_PRIORITY_HIGH);
#else
    vpss_ctrl *vpss_ctrl = HI_NULL;
    hi_u32 err;
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    kthread_start();
    err = pthread_create(&vpss_ctrl->h_thread, HI_NULL, vpss_ctrl_thread_process, HI_NULL);
    if (err != 0) {
        vpss_error("can not create thread.\n");
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_destory_thread(vpss_ip ip)
{
#ifndef __VPSS_UT__
    vpss_ctrl *vpss_ctrl_info = HI_NULL;

    vpss_ctrl_get_ctx(ip, vpss_ctrl_info);

    if (!IS_ERR(vpss_ctrl_info->h_thread)) {
        osal_kthread_destroy(vpss_ctrl_info->h_thread, 1);
    }
#else
    vpss_ctrl *vpss_ctrl_info = HI_NULL;
    vpss_ctrl_get_ctx(ip, vpss_ctrl_info);
    osal_kthread_destroy(vpss_ctrl_info->h_thread, 1);
    pthread_join(vpss_ctrl_info->h_thread, HI_NULL);
#endif

    if (vpss_ctrl_info->task_next.queue_head.wait != HI_NULL) {
        vpss_comm_destory_event(&(vpss_ctrl_info->task_next));
    }

    if (vpss_ctrl_info->new_task.queue_head.wait != HI_NULL) {
        vpss_comm_destory_event(&(vpss_ctrl_info->new_task));
    }

    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_echo_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    if (vpss_comm_cmd_check(argc, 1, private, private) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    osal_printk("------------------------------------VPSS---------------------------------------\n"
                " echo command      inst_id   para1    para2(para3)    path                  explanation\n"
                "-------------------------------------------------------------------------\n");
    osal_printk("  vpssbypass       (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
    osal_printk("  pqbypass         (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  printinfo        (0~11)    (0~3)    num     >/proc/msp/vpss_ctrl00  para1:0~2 port 3 src\n");

    osal_printk("  set_src_reso     (0~11)     w        h      >/proc/msp/vpss_ctrl00  para1:width para2:height\n");

    osal_printk("  saveyuv          (0~11)    (0~3)    num     >/proc/msp/vpss_ctrl00  para1:0~2 port 3 src\n"
                "  set_prog         (0~11)    (0/1)    (0/1)   >/proc/msp/vpss_ctrl00 "
                " (para1:prog set en,0:off,1:on); (para2:inter set,0:prog,1:inter) \n"
                "  setpause         (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  setbuffnum       (0~11)    (0/1)    (3~20)  >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
    osal_printk("  printpqinfo      (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  printframeaddr   (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
    osal_printk("  setdumpreg       (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
    osal_printk("  intestpattern    (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  outtestpattern   (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  rotateneedalg    (0~11)     w        h      >/proc/msp/vpss_ctrl00  para1:width para2:height\n");
    osal_printk("  rotation         (0~11)    (0~2)    (0~1) (0~3) >/proc/msp/vpss_ctrl00  para1:0~2 port; "
                " para2:off/on; para3:0 off, 1 90, 2 180, 3 270\n");
    osal_printk("  printtunlinfo    (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  refcntinfo       (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n"
                "  intunlenable     (0~11)    (0/1)    (0/1)   >/proc/msp/vpss_ctrl00  para2:0 off 1 on\n"
                "  outtunlenable    (0~11)    (0/1)    (0/1)   >/proc/msp/vpss_ctrl00  para2:0 off 1 on\n");

#ifdef DPT
    osal_printk(" colsecmp          (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
    osal_printk(" printmemcinfo     (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
    osal_printk(" printvoinfo       (0~11)    (0/1)            >/proc/msp/vpss_ctrl00  para1:0 off 1 on\n");
#endif
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_bypass_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].vpss_bypass = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].vpss_bypass = HI_FALSE;
    }

    osal_printk("command vpssbypass success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_printinfo_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    hi_u32 type;
    hi_u32 num;
    hi_u32 number_of_times;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */
    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    type = (hi_u32)(osal_strtol(argv[2], HI_NULL, OSAL_BASE_DEC));  /* 2 is the index */
    if (type > 3) {  /* type cannot beyond 3 */
        osal_printk("input para2 is too big, need 0/1/2/3.\n");
        return HI_FAILURE;
    }

    num = (hi_u32)(osal_strtol(argv[3], HI_NULL, OSAL_BASE_DEC));  /* 3 is the index */
    number_of_times = (num == 0) ? 1 : num;
    if (number_of_times > DBG_NUMBER_MAX) {
        osal_printk("input number %d is too big,we support 0~1000\n", number_of_times);
        return HI_FAILURE;
    }

    if (type == 3) {  /* type is 3 means print input info */
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_src_info = HI_TRUE;
        g_vpss_ctrl[ip].global_debug_info[inst_num].src_dbg_times = number_of_times;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_out_info[type] = HI_TRUE;
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_dbg_times[type] = number_of_times;
    }

    osal_printk("command printinfo success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_save_frame_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    hi_u32 type;
    hi_u32 num;
    hi_u32 number_of_times;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    type = (hi_u32)(osal_strtol(argv[2], HI_NULL, OSAL_BASE_DEC));  /* 2 is the index */

    if (type > 3) {  /* type cannot beyond 3 */
        osal_printk("input para2 is too big, need 0/1/2/3.\n");
        return HI_FAILURE;
    }

    num = (hi_u32)(osal_strtol(argv[3], HI_NULL, OSAL_BASE_DEC));  /* 3 is the index */
    number_of_times = (num == 0) ? 1 : num;

    if (number_of_times > DBG_NUMBER_MAX) {
        osal_printk("input number %d is too big,we support 0~1000\n", number_of_times);
        return HI_FAILURE;
    }

    if (type == 3) {  /* type is 3 means save source frame */
        g_vpss_ctrl[ip].global_debug_info[inst_num].save_in_frame = HI_TRUE;
        g_vpss_ctrl[ip].global_debug_info[inst_num].src_dbg_times = number_of_times;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].save_out_frame[type] = HI_TRUE;
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_dbg_times[type] = number_of_times;
    }

    osal_printk("command save_frame success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_pqbypass_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].pq_bypass = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].pq_bypass = HI_FALSE;
    }

    osal_printk("command pqbypass success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_prog_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_prog_inter_en = HI_TRUE;

        if (!osal_strncmp(argv[3], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 3 is the index */
            g_vpss_ctrl[ip].global_debug_info[inst_num].set_interlaced = HI_TRUE;
        } else {
            g_vpss_ctrl[ip].global_debug_info[inst_num].set_interlaced = HI_FALSE;
        }
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_prog_inter_en = HI_FALSE;
    }

    osal_printk("command inter success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_pause_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_pause = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_pause = HI_FALSE;
    }

    osal_printk("command pause success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_print_pq_info_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_pq_info = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_pq_info = HI_FALSE;
    }

    osal_printk("command tran_pq_info success\n");
    return HI_SUCCESS;
}

#ifdef DPT
static hi_s32 vpss_ctrl_closecmp_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */ */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */ */
        g_vpss_ctrl[ip].global_debug_info[inst_num].close_cmp = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].close_cmp = HI_FALSE;
    }

    osal_printk("command closecmp success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_print_vo_info_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_vo_info = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_vo_info = HI_FALSE;
    }

    osal_printk("command tran_vo_info success\n");
    return HI_SUCCESS;
}
#endif

static hi_s32 vpss_ctrl_print_frame_addr_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_frame_addr = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_frame_addr = HI_FALSE;
    }

    osal_printk("command print_frame_addr success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_dump_reg_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].dump_reg = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].dump_reg = HI_FALSE;
    }

    osal_printk("command set_dump_reg success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_in_testpattern_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].cf_pattern.ptn_en = HI_TRUE;
        g_vpss_ctrl[ip].global_debug_info[inst_num].cf_pattern.line_width = 0x07;
        g_vpss_ctrl[ip].global_debug_info[inst_num].cf_pattern.tst_pat_speed = 0x1;
        g_vpss_ctrl[ip].global_debug_info[inst_num].cf_pattern.tst_pat_color = 0x7F7F;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].cf_pattern.ptn_en = HI_FALSE;
    }

    osal_printk("command testpattern success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_out_testpattern_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_pattern.ptn_en = HI_TRUE;
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_pattern.line_width = 0x17;
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_pattern.tst_pat_speed = 0x0;
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_pattern.tst_pat_color = 0xCFCF;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_pattern.ptn_en = HI_FALSE;
    }

    osal_printk("command testpattern success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_rotate_support_alg_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    hi_u32 input_width;
    hi_u32 input_height;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    input_width = (hi_u32)(osal_strtol(argv[2], HI_NULL, OSAL_BASE_DEC));  /* 2 is the index */

    if (input_width > VPSS_MAX_WIDTH_TRANS || input_width < VPSS_MIN_WIDTH_TRANS) {
        osal_printk("input para2 is too big, need 136 <= input_width <= 4096.\n");
        return HI_FAILURE;
    }

    input_height = (hi_u32)(osal_strtol(argv[3], HI_NULL, OSAL_BASE_DEC));  /* 3 is the index */

    if (input_height > VPSS_MAX_HEIGHT_TRANS || input_height <= VPSS_MIN_HEIGHT_TRANS) {
        osal_printk("input para3 is too big, need 64 <= input_height <= 1088.\n");
        return HI_FAILURE;
    }

    g_vpss_ctrl[ip].global_debug_info[inst_num].rot_alg_width = input_width;
    g_vpss_ctrl[ip].global_debug_info[inst_num].rot_alg_height = input_height;
    osal_printk("command set rotate support alg width and height instance:%d w:%d h:%d success\n",
                inst_num, input_width, input_height);

    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_src_reso(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    hi_u32 input_width;
    hi_u32 input_height;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    input_width = (hi_u32)(osal_strtol(argv[2], HI_NULL, OSAL_BASE_DEC));  /* 2 is the index */
    if ((input_width != 0) &&
        ((input_width > VPSS_MAX_PROC_SET_SRC_WIDTH) || (input_width < VPSS_MIN_PROC_SET_SRC_WIDTH))) {
        osal_printk("input para2 is too big, need 64 <= input_width <= 8192.\n");
        return HI_FAILURE;
    }

    input_height = (hi_u32)(osal_strtol(argv[3], HI_NULL, OSAL_BASE_DEC));  /* 3 is the index */
    if ((input_height != 0) &&
        ((input_height > VPSS_MAX_PROC_SET_SRC_HEIGHT) || (input_height < VPSS_MIN_PROC_SET_SRC_HEIGHT))) {
        osal_printk("input para3 is too big, need 64 <= input_height <= 4320.\n");
        return HI_FAILURE;
    }

    if ((input_width % VPSS_RESO_ALIGN != 0) || (input_height % VPSS_RESO_ALIGN != 0)) {
        osal_printk("set reso align set w:%d h:%d \n", input_width, input_height);
        return HI_FAILURE;
    }

    g_vpss_ctrl[ip].global_debug_info[inst_num].set_src_width = input_width;
    g_vpss_ctrl[ip].global_debug_info[inst_num].set_src_height = input_height;
    osal_printk("command set src reso instance:%d w:%d h:%d success\n", inst_num, input_width, input_height);

    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_rota_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num, type, num;
    hi_drv_vpss_rotation rotation;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 5, private, private) != HI_SUCCESS) { /* 5 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */
    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    type = (hi_u32)(osal_strtol(argv[2], HI_NULL, OSAL_BASE_DEC));  /* 2 is the index */
    if (type >= DEF_DBG_ROTA_MAX) {  /* only support port 0 1 2 */
        osal_printk("input para2 is too big, need 0/1/2.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[3], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 3 : third set */
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_rota_en[type] = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_rota_en[type] = HI_FALSE;
    }

    num = (hi_u32)(osal_strtol(argv[4], HI_NULL, OSAL_BASE_DEC));  /* 4 is the index */
    if (num >= HI_DRV_VPSS_ROTATION_MAX) {
        osal_printk("input number %d is too big,we support 0 1 2 3 \n", num);
        return HI_FAILURE;
    }
    switch (num) {
        case 0: /* 0:no rota */
            rotation = HI_DRV_VPSS_ROTATION_DISABLE;
            break;
        case 1: /* 1:90 rota */
            rotation = HI_DRV_VPSS_ROTATION_90;
            break;
        case 2: /* 2:180 rota */
            rotation = HI_DRV_VPSS_ROTATION_180;
            break;
        case 3: /* 3:270 rota */
            rotation = HI_DRV_VPSS_ROTATION_270;
            break;
        default:
            rotation = HI_DRV_VPSS_ROTATION_DISABLE;
            break;
    }

    g_vpss_ctrl[ip].global_debug_info[inst_num].rotation[type] = rotation;

    osal_printk("command printinfo success\n");
    return HI_SUCCESS;
}


static hi_s32 vpss_ctrl_print_tunl_info_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_tunl_info = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_tunl_info = HI_FALSE;
    }

    osal_printk("command print_tunl_info success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_print_ref_cnt_info_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 3, private, private) != HI_SUCCESS) { /* 3 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_ref_cnt_info = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].print_ref_cnt_info = HI_FALSE;
    }

    osal_printk("command print_ref_cnt_info success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_in_tunl_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].in_tunl = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].in_tunl = HI_FALSE;
    }

    if (!osal_strncmp(argv[3], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 3 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].in_wtunl_enable = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].in_wtunl_enable = HI_FALSE;
    }

    osal_printk("command tunl success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_out_tunl_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[1], HI_NULL, OSAL_BASE_DEC));  /* 1 is the index */

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 2 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_tunl = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_tunl = HI_FALSE;
    }

    if (!osal_strncmp(argv[3], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {  /* 3 is the index */
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_wtunl_enable = HI_TRUE;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].out_wtunl_enable = HI_FALSE;
    }

    osal_printk("command tunl success\n");
    return HI_SUCCESS;
}

static hi_s32 vpss_ctrl_set_buff_num_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_u32 inst_num;
    hi_u32 buff_num;
    vpss_ip ip;

    if (vpss_comm_cmd_check(argc, 4, private, private) != HI_SUCCESS) { /* 4 is para index */
        return HI_FAILURE;
    }

    ip = *(vpss_ip *)private;
    inst_num = (hi_u32)(osal_strtol(argv[0x1], HI_NULL, OSAL_BASE_DEC));

    if (inst_num > (VPSS_INSTANCE_MAX_NUM - 1)) {
        osal_printk("Input para2 is too big, inst num should 0~1.\n");
        return HI_FAILURE;
    }

    if (!osal_strncmp(argv[0x2], strlen(DEF_DBG_ON), DEF_DBG_ON, strlen(DEF_DBG_ON))) {
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_buff_num = HI_TRUE;
        buff_num = (hi_u32)(osal_strtol(argv[0x3], HI_NULL, OSAL_BASE_DEC));

        if ((buff_num < 3) || (buff_num > DEF_OUT_NODE_MAX_NUM)) {  /* buffer range[3,20] */
            osal_printk("Buff num %d too big, num should 3~6.\n", buff_num);
            return HI_FAILURE;
        }

        g_vpss_ctrl[ip].global_debug_info[inst_num].buff_num = buff_num;
    } else {
        g_vpss_ctrl[ip].global_debug_info[inst_num].set_buff_num = HI_FALSE;
    }

    osal_printk("command tunl success\n");
    return HI_SUCCESS;
}

static hi_void vpss_task_proc(hi_void *p, vpss_task *task, hi_u32 i)
{
    hi_u8 *vpss_task_string[VPSS_TASK_STATE_MAX + 1] = {
        "IDLE",
        "PREPARE",
        "CONFIG",
        "START",
        "WAIT",
        "SUCCESS",
        "CLEAR",
        "BUTT",
    };
    osal_proc_print(p, "-------------------------------task[0x%x] info---------------------------------\n", i);
    osal_proc_print(p, "%-40s:%-16s/%-16d\n", "state/instnum",
                    vpss_task_string[task->state], task->task_inst_num);
    osal_proc_print(p, "%-40s:%-16lld/%-16lld\n", "update/isr",
                    task->update_task_time, task->get_isrint_time);
    osal_proc_print(p, "%-40s:%-16lld/%-16lld\n", "check_time/prepare_time",
                    task->check_task_time_end - task->check_task_time,
                    task->prepare_task_time_end - task->prepare_task_time);
    osal_proc_print(p, "%-40s:%-16lld/%-16lld\n", "config_time/start_time",
                    task->config_task_time_end - task->config_task_time,
                    task->start_task_time_end - task->start_task_time);
    osal_proc_print(p, "%-40s:%-16lld/%-16lld\n", "logic_time/complete_time",
                    task->get_isrint_time - task->start_task_time_end,
                    task->complete_task_time_end - task->complete_task_time);
    osal_proc_print(p, "%-40s:%-8lld\n", "total_time",
                    task->complete_task_time_end - task->prepare_task_time);
}

hi_s32 vpss_ctrl_proc_read(hi_void *p, hi_void *v)
{
#ifndef __VPSS_UT__
    vpss_ip ip;
    hi_u32 i = 0;
    vpss_task *task = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    hi_u8 *vpss_true_string[] = {
        "false",
        "true ",
    };

    if (v == HI_NULL) {
        vpss_error("v is null pointer!\n");
        return -EFAULT;
    }

    ip = *(vpss_ip *)v;
    vpss_ctrl = &g_vpss_ctrl[ip];
    osal_proc_print(p, "-------------------------------VPSS CTRL--------------------------------\n");
    osal_proc_print(p, "%-40s:%-8s\n", "version", g_vpss_date);
    osal_proc_print(p, "-------------------------------ctrl info---------------------------------\n");
    osal_proc_print(p, "%-40s:%-8d/%-8d/%-8d/%-8s\n", "IP/IRQ/open_num/suspend",
                    vpss_ctrl->ip, vpss_ctrl->vpss_irq_num,
                    vpss_ctrl->is_vpss_open, vpss_true_string[vpss_ctrl->suspend]);
    osal_proc_print(p, "%-40s:%-8d/%-8d/%-8d\n", "inst_num/targetpos/clear_task_count",
                    vpss_ctrl->inst_ctrl_info.instance_num, vpss_ctrl->inst_ctrl_info.target,
                    vpss_ctrl->clear_task_cnt);
    osal_proc_print(p, "-------------------------------thread info-------------------------------\n");
    osal_proc_print(p, "%-40s:%-8s/%-8d\n", "sleep/threadpos",
                    vpss_true_string[vpss_ctrl->thread_sleep], vpss_ctrl->thread_pos);
    osal_proc_print(p, "%-40s:%-8d/%-8d\n", "process_hz(try/OK) ",
                    vpss_ctrl->inst_ctrl_info.check_rate, vpss_ctrl->inst_ctrl_info.check_suc_rate);
    osal_proc_print(p, "%-40s:%-8d/%-8d\n", "process_cnt(try/OK) ",
                    vpss_ctrl->inst_ctrl_info.check_cnt, vpss_ctrl->inst_ctrl_info.check_suc_cnt);

    for (i = 0; i < VPSS_TASK_MAX_NUM; i++) {
        task = &vpss_ctrl->task[i];
        vpss_task_proc(p, task, i);
    }

#endif
    return HI_SUCCESS;
}

static osal_proc_cmd g_vpss_proc_info[] = {
    { "help",   vpss_ctrl_echo_help },
    { "vpssbypass",   vpss_ctrl_set_bypass_proc },
    { "pqbypass",   vpss_ctrl_pqbypass_proc },
    { "printinfo",   vpss_ctrl_printinfo_proc },
    { "set_src_reso",  vpss_ctrl_set_src_reso },
    { "saveyuv",   vpss_ctrl_save_frame_proc },
    { "set_prog",   vpss_ctrl_set_prog_proc },
    { "setpause",   vpss_ctrl_set_pause_proc },
    { "setbuffnum",   vpss_ctrl_set_buff_num_proc },
    { "printpqinfo",   vpss_ctrl_print_pq_info_proc },
    { "printframeaddr", vpss_ctrl_print_frame_addr_proc },
    { "setdumpreg",   vpss_ctrl_set_dump_reg_proc },
    { "intestpattern",  vpss_ctrl_in_testpattern_proc },
    { "outtestpattern", vpss_ctrl_out_testpattern_proc },
    { "rotateneedalg",  vpss_ctrl_rotate_support_alg_proc },
    { "rotation",  vpss_ctrl_set_rota_proc },
    { "printtunlinfo",  vpss_ctrl_print_tunl_info_proc },
    { "refcntinfo",   vpss_ctrl_print_ref_cnt_info_proc },
    { "intunlenable",   vpss_ctrl_set_in_tunl_proc },
    { "outtunlenable",  vpss_ctrl_set_out_tunl_proc },
#ifdef DPT
    { "closecmp",   vpss_ctrl_closecmp_proc },
    { "printvoinfo",   vpss_ctrl_print_vo_info_proc },
#endif
};

hi_s32 vpss_ctrl_create_proc(vpss_ip ip)
{
#ifndef __VPSS_UT__
    hi_char proc_name[VPSS_FILE_NAMELENGTH];
    osal_proc_entry *proc_item = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    vpss_ctrl_get_ctx(ip, vpss_ctrl);
    snprintf(proc_name, VPSS_FILE_NAMELENGTH, "vpss_ctrl%02d", ip);

    proc_item = osal_proc_add(proc_name, strlen(proc_name));
    if (proc_item == HI_NULL) {
        vpss_error("vpss add proc failed!\n");
        return HI_FAILURE;
    }
    proc_item->private = (hi_void *)&(vpss_ctrl->ip);
    proc_item->read = vpss_ctrl_proc_read;
    proc_item->cmd_list = g_vpss_proc_info;
    proc_item->cmd_cnt = sizeof(g_vpss_proc_info) / sizeof(osal_proc_cmd);
#endif
    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_destory_proc(vpss_ip ip)
{
#ifndef __VPSS_UT__
    hi_char proc_name[VPSS_FILE_NAMELENGTH];
    snprintf(proc_name, VPSS_FILE_NAMELENGTH, "vpss_ctrl%02d", ip);
    osal_proc_remove(proc_name, strlen(proc_name));
#endif
    return HI_SUCCESS;
}

hi_u32 vpss_ctrl_malloc_instance_id(hi_void)
{
    hi_u32 i;
    hi_u32 inst_count;
    hi_bool find_vpss = HI_FALSE;
    hi_u32 vpss_id;
    vpss_ip vpss_ip;
    unsigned long lock_flag;
    vpss_ctrl_inst *inst_ctrl_info = HI_NULL;
    inst_count = 0;

    for (vpss_id = 0; vpss_id < VPSS_IP_MAX * VPSS_INSTANCE_MAX_NUM; vpss_id++) {
        find_vpss = HI_FALSE;
        for (vpss_ip = VPSS_IP_0; vpss_ip < VPSS_IP_MAX; vpss_ip++) {
            inst_ctrl_info = &(g_vpss_ctrl[vpss_ip].inst_ctrl_info);
            if (g_vpss_ctrl[vpss_ip].ip_vaild != HI_TRUE) {
                continue;
            }

            vpss_comm_down_spin(&(inst_ctrl_info->list_lock), &lock_flag);
            for (i = 0; i < VPSS_INSTANCE_MAX_NUM; i++) {
                if (inst_ctrl_info->inst_pool[i] != HI_NULL) {
                    if (vpss_id == inst_ctrl_info->inst_pool[i]->id) {
                        find_vpss = HI_TRUE;
                        inst_count++;
                        break;
                    }
                }
            }
            vpss_comm_up_spin(&(inst_ctrl_info->list_lock), &lock_flag);
        }

        if (find_vpss == HI_FALSE) {
            break;
        }
    }

    if (inst_count == 0) {
        return 0;
    } else {
        return vpss_id;
    }
}

hi_handle vpss_ctrl_add_instance(vpss_instance *instance)
{
    hi_u32 vpss_id;
    vpss_ctrl_inst *inst_ctrl_info = HI_NULL;
    hi_u32 count;
    unsigned long lock_flag;
    vpss_ip vpss_ip;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    vpss_ip = instance->ctrl_id;
    vpss_ctrl_get_ctx(vpss_ip, vpss_ctrl);

    inst_ctrl_info = &vpss_ctrl->inst_ctrl_info;
    vpss_id = vpss_ctrl_malloc_instance_id();
    vpss_comm_down_spin(&(inst_ctrl_info->list_lock), &lock_flag);
    for (count = 0; count < VPSS_INSTANCE_MAX_NUM; count++) {
        if (inst_ctrl_info->inst_pool[count] == HI_NULL) {
            inst_ctrl_info->inst_pool[count] = instance;
            break;
        }
    }
    vpss_comm_up_spin(&(inst_ctrl_info->list_lock), &lock_flag);

    if (count == VPSS_INSTANCE_MAX_NUM) {
        vpss_error("instance number is max,%d.\n", count);
        return HI_INVALID_HANDLE;
    } else {
        instance->id = vpss_id;
        inst_ctrl_info->instance_num++;
        return instance->id;
    }
}

hi_s32 vpss_ctrl_del_instance(hi_handle h_vpss)
{
    hi_u32 i;
    vpss_ip vpss_ip;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl_inst *inst_ctrl_info = HI_NULL;
    unsigned long lock_flag;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_vpss(%d) is invalid\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_ip = instance->ctrl_id;
    vpss_ctrl_get_ctx(vpss_ip, vpss_ctrl);
    inst_ctrl_info = &vpss_ctrl->inst_ctrl_info;

    vpss_comm_down_spin(&(inst_ctrl_info->list_lock), &lock_flag);
    for (i = 0; i < VPSS_INSTANCE_MAX_NUM; i++) {
        if (inst_ctrl_info->inst_pool[i] == instance) {
            inst_ctrl_info->inst_pool[i] = HI_NULL;
            break;
        }
    }
    vpss_comm_up_spin(&(inst_ctrl_info->list_lock), &lock_flag);

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_create_instance(hi_drv_vpss_instance_cfg *instance_cfg, hi_handle *instance_id)
{
    hi_s32 ret;
    hi_s32 inst_handle;
    vpss_ip vpss_ip = VPSS_IP_MAX;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;

    vpss_check_bool(instance_cfg->quick_enable);
    vpss_check_enum(instance_cfg->prog_detect, HI_DRV_VPSS_PROG_DETECT_MAX);
    vpss_check_enum(instance_cfg->work_mode, HI_DRV_VPSS_WORKMODE_MAX);
    vpss_check_enum(instance_cfg->pq_mode, HI_DRV_VPSS_PQMODE_MAX);

    ret = vpss_ctrl_get_distribute_ip(&vpss_ip);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss ctrl isn't enable\n");
        return HI_ERR_VPSS_NO_INIT;
    }

    vpss_ctrl_get_ctx(vpss_ip, vpss_ctrl);

    if (vpss_ctrl->is_vpss_open == 0) {
        vpss_error("vpss ctrl isn't opened, ip %d\n", vpss_ip);
        return HI_ERR_VPSS_NO_INIT;
    }

    instance = (vpss_instance *)vpss_vmalloc(sizeof(vpss_instance));
    if (instance == HI_NULL) {
        vpss_error("vmalloc instance node failed \n");
        return HI_ERR_VPSS_MALLOC_FAILED;
    }

    vpss_inst_init(instance, instance_cfg);
    instance->ctrl_id = vpss_ip;
    inst_handle = vpss_ctrl_add_instance(instance);
    if (inst_handle == HI_INVALID_HANDLE) {
        vpss_vfree(instance);
        vpss_error("vpss_ctrl_add_instance failed \n");
        return HI_INVALID_HANDLE;
    }
    instance->in_list.h_src_module = inst_handle;
    ret = vpss_inst_create_proc(instance);
    if (ret != HI_SUCCESS) {
        vpss_ctrl_del_instance(instance->id);
        vpss_vfree(instance);
        vpss_error("vpss_ctrl_create_inst_proc failed \n");
        return HI_INVALID_HANDLE;
    }

    instance->global_dbg_ctrl = &(vpss_ctrl->global_debug_info[inst_handle]);
    *instance_id = inst_handle;

    return HI_SUCCESS;
}

hi_void delete_inst_from_task(vpss_ctrl *vpss_ctrl, vpss_instance *instance)
{
    hi_u32 i;
    hi_u32 task_inst_num;
    task_inst_num = vpss_ctrl->task[0].task_inst_num;

    for (i = 0; i < task_inst_num; i++) {
        if (vpss_ctrl->task[0].instance[i] == instance) {
            vpss_ctrl->task[0].instance[i] = HI_NULL;
        }
    }

    task_inst_num = vpss_ctrl->task[1].task_inst_num;

    for (i = 0; i < task_inst_num; i++) {
        if (vpss_ctrl->task[1].instance[i] == instance) {
            vpss_ctrl->task[1].instance[i] = HI_NULL;
        }
    }
}

static hi_void vpss_ctrl_destory_instance_map_check(vpss_instance *instance)
{
    if (instance->perfor.buf_dfx.try_map_cnt != instance->perfor.buf_dfx.try_umap_cnt) {
        vpss_error("map umap try not match; map:%d umap:%d \n",
                   instance->perfor.buf_dfx.try_map_cnt,
                   instance->perfor.buf_dfx.try_umap_cnt);
    }

    if (instance->perfor.buf_dfx.map_suc_cnt != instance->perfor.buf_dfx.umap_suc_cnt) {
        vpss_error("map umap suc not match; map:%d umap:%d \n",
                   instance->perfor.buf_dfx.try_map_cnt,
                   instance->perfor.buf_dfx.try_umap_cnt);
    }

    return;
}

hi_s32 vpss_ctrl_destory_instance(hi_handle h_vpss)
{
    vpss_ip vpss_ip;
    vpss_instance *instance = HI_NULL;
    vpss_ctrl *vpss_ctrl = HI_NULL;
    hi_s32 ret;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_vpss(%d) is not be vaild or created\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_ip = instance->ctrl_id;
    vpss_ctrl_get_ctx(vpss_ip, vpss_ctrl);
    vpss_ctrl_suspend_inst(instance);

    ret = vpss_ctrl_pause(vpss_ip);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_ctrl_pause failed\n");
    }

    delete_inst_from_task(vpss_ctrl, instance);
    vpss_ctrl->inst_ctrl_info.instance_num--;

    ret = vpss_ctrl_del_instance(h_vpss);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_ctrl_del_instance failed\n");
    }

    vpss_inst_deinit(instance);

    ret = vpss_ctrl_resume(vpss_ip);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_ctrl_resume failed\n");
    }

    vpss_ctrl_resume_inst(instance);
    vpss_ctrl_destory_instance_map_check(instance);

    vpss_vfree(instance);
    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_set_vpss_cfg(hi_handle h_vpss, hi_drv_vpss_instance_cfg *instance_cfg)
{
    vpss_instance *instance = HI_NULL;

    vpss_check_bool(instance_cfg->quick_enable);
    vpss_check_enum(instance_cfg->prog_detect, HI_DRV_VPSS_PROG_DETECT_MAX);
    vpss_check_enum(instance_cfg->work_mode, HI_DRV_VPSS_WORKMODE_MAX);
    vpss_check_enum(instance_cfg->pq_mode, HI_DRV_VPSS_PQMODE_MAX);

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("hvpss %d is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_inst_set_inst_cfg(instance, instance_cfg);
    return HI_SUCCESS;
}
hi_s32 vpss_ctrl_get_vpss_cfg(hi_handle h_vpss, hi_drv_vpss_instance_cfg *instance_cfg)
{
    vpss_instance *instance;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("hvpss %d is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_inst_get_inst_cfg(instance, instance_cfg);
    return HI_SUCCESS;
}
hi_s32 vpss_ctrl_get_default_cfg(hi_drv_vpss_instance_cfg *instance_cfg)
{
    vpss_inst_get_def_inst_cfg(instance_cfg);
    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_create_port(hi_handle h_vpss, hi_drv_vpss_port_cfg *vpss_port_cfg, hi_handle *ph_port)
{
    vpss_ip vpss_ip;
    vpss_instance *instance;
    hi_s32 ret;
    hi_s32 ret_value;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_vpss %d is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_ip = instance->ctrl_id;
    vpss_ctrl_suspend_inst(instance);

    ret_value = vpss_ctrl_pause(vpss_ip);
    if (ret_value != HI_SUCCESS) {
        vpss_error("vpss_ctrl_pause failed\n");
    }

    ret = vpss_inst_create_port(instance, vpss_port_cfg, ph_port);

    ret_value = vpss_ctrl_resume(vpss_ip);
    if (ret_value != HI_SUCCESS) {
        vpss_error("vpss_ctrl_resume failed\n");
    }

    vpss_ctrl_resume_inst(instance);
    return ret;
}
hi_s32 vpss_ctrl_destroy_port(hi_handle h_port)
{
    vpss_ip vpss_ip;
    vpss_instance *instance;
    hi_s32 ret;
    hi_s32 ret_value;
    hi_handle h_vpss;

    h_vpss = porthandle_to_vpssid(h_port);
    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", h_port);
        return HI_FAILURE;
    }

    vpss_ctrl_suspend_inst(instance);

    vpss_ip = instance->ctrl_id;
    ret_value = vpss_ctrl_pause(vpss_ip);
    if (ret_value != HI_SUCCESS) {
        vpss_error("vpss_ctrl_pause failed\n");
    }

    ret = vpss_inst_destory_port(instance, h_port);
    ret_value = vpss_ctrl_resume(vpss_ip);
    if (ret_value != HI_SUCCESS) {
        vpss_error("vpss_ctrl_resume failed\n");
    }

    vpss_ctrl_resume_inst(instance);
    return ret;
}
hi_s32 vpss_ctrl_set_port_cfg(hi_handle h_port, hi_drv_vpss_port_cfg *vpss_port_cfg)
{
    vpss_instance *instance;
    hi_s32 ret;
    hi_handle h_vpss;

    h_vpss = porthandle_to_vpssid(h_port);
    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", h_port);
        return HI_FAILURE;
    }

    ret = vpss_inst_check_port_cfg(instance, vpss_port_cfg);
    if (ret != HI_SUCCESS) {
        vpss_error("set_port_cfg error.\n");
        return HI_FAILURE;
    }

    ret = vpss_inst_set_port_cfg(instance, h_port, vpss_port_cfg);
    return ret;
}
hi_s32 vpss_ctrl_get_port_cfg(hi_handle h_port, hi_drv_vpss_port_cfg *vpss_port_cfg)
{
    vpss_instance *instance;
    hi_s32 ret;
    hi_handle h_vpss;

    h_vpss = porthandle_to_vpssid(h_port);
    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port is invalid.\n");
        return HI_FAILURE;
    }

    ret = vpss_inst_get_port_cfg(instance, h_port, vpss_port_cfg);
    return ret;
}
hi_s32 vpss_ctrl_get_default_port_cfg(hi_drv_vpss_port_cfg *vpss_port_cfg)
{
    vpss_inst_get_def_port_cfg(vpss_port_cfg);
    return HI_SUCCESS;
}
hi_s32 vpss_ctrl_enable_port(hi_handle h_port, hi_bool enable)
{
    vpss_instance *instance = HI_NULL;
    hi_s32 ret;
    hi_handle h_vpss;
    vpss_check_bool(enable);

    h_vpss = porthandle_to_vpssid(h_port);
    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", h_port);
        return HI_FAILURE;
    }

    ret = vpss_inst_enable_port(instance, h_port, enable);
    return ret;
}
hi_s32 vpss_ctrl_send_command(hi_handle h_vpss, hi_drv_vpss_user_command command, hi_void *args)
{
    vpss_instance *instance;
    vpss_ip vpss_ip;
    hi_s32 ret;
    hi_s32 ret_value;

    vpss_check_enum(command, HI_DRV_VPSS_USER_COMMAND_MAX);

    if (command == HI_DRV_VPSS_USER_COMMAND_WAKEUP) {
        vpss_ctrl_wake_up_thread();
        ret = vpss_ctrl_notify_frame_status(h_vpss, VPSS_FRAME_STATUS_BACK);
        if (ret != HI_SUCCESS) {
            vpss_error("notify frame status failed ret %d\n", ret);
        }
        return HI_SUCCESS;
    }

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_vpss %d is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_ip = instance->ctrl_id;
    vpss_ctrl_suspend_inst(instance);
    ret_value = vpss_ctrl_pause(vpss_ip);
    if (ret_value != HI_SUCCESS) {
        vpss_error("vpss_ctrl_pause failed\n");
    }

    ret = vpss_inst_reply_user_command(instance, command, args);
    ret_value = vpss_ctrl_resume(vpss_ip);
    if (ret_value != HI_SUCCESS) {
        vpss_error("vpss_ctrl_resume failed\n");
    }

    vpss_ctrl_resume_inst(instance);
    return ret;
}

hi_s32 vpss_ctrl_queue_frame(hi_handle h_vpss, hi_drv_video_frame *frame)
{
    hi_s32 ret;
    hi_s32 ret_value;
    vpss_instance *instance;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_vpss %d is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    vpss_ctrl_wake_up_thread();

    if (vpss_inst_check_image(instance, frame) != HI_SUCCESS) {
        vpss_error("check frame para fail,index = %d\n", frame->frame_index);
        return HI_FAILURE;
    }

    ret = vpss_inst_queue_frame(instance, frame);

    ret_value = vpss_ctrl_notify_frame_status(h_vpss, VPSS_FRAME_STATUS_ARRIVAL);
    if (ret_value != HI_SUCCESS) {
        vpss_error("notify frame status failed ret %d\n", ret_value);
    }
    return ret;
}

hi_s32 vpss_ctrl_dequeue_frame(hi_handle h_vpss, hi_drv_video_frame *frame)
{
    hi_s32 ret;
    vpss_instance *instance;

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_vpss %u is invalid.\n", h_vpss);
        return HI_FAILURE;
    }

    ret = vpss_inst_dequeue_frame(instance, frame);
    return ret;
}

hi_s32 vpss_ctrl_acquire_frame(hi_handle h_port, hi_drv_video_frame *frame)
{
    vpss_instance *instance;
    hi_s32 ret;
    hi_handle h_vpss;

    h_vpss = porthandle_to_vpssid(h_port);
    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", h_port);
        return HI_FAILURE;
    }

    ret = vpss_inst_get_port_frame(instance, h_port, frame);

    return ret;
}

hi_s32 vpss_ctrl_release_frame(hi_handle h_port, hi_drv_video_frame *frame)
{
    vpss_instance *instance = HI_NULL;
    hi_s32 ret;
    hi_handle h_vpss;

    h_vpss = porthandle_to_vpssid(h_port);

    if (frame == HI_NULL) {
        vpss_error("pst_vpss_frame is NULL.\n");
        return HI_FAILURE;
    }

    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", h_port);
        return HI_FAILURE;
    }

    ret = vpss_inst_rel_port_frame(instance, h_port, frame);
    if (ret != HI_SUCCESS) {
        vpss_error("\t\n port = %d rel_port_frame %d failed", h_port, frame->frame_index);
    }

    (hi_void) vpss_ctrl_wake_up_thread();
    return ret;
}

hi_s32 vpss_ctrl_regist_hook_event(hi_handle instance_id, hi_handle dst_id, fn_vpss_event_callback event_callback)
{
    vpss_instance *instance;
    hi_s32 ret;

    instance = vpss_ctrl_get_instance(instance_id);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", instance_id);
        return HI_FAILURE;
    }

    ret = vpss_inst_regist_hook_event(instance, dst_id, event_callback);
    return ret;
}

hi_s32 vpss_ctrl_regist_callback(hi_handle h_port, hi_handle h_sink, hi_handle h_src,
    hi_drv_vpss_callback_func *fn_vpss_callback)
{
    vpss_instance *instance;
    hi_s32 ret;
    hi_handle h_vpss;

    h_vpss = porthandle_to_vpssid(h_port);
    instance = vpss_ctrl_get_instance(h_vpss);
    if (instance == HI_NULL) {
        vpss_error("h_port %d is invalid.\n", h_port);
        return HI_FAILURE;
    }

    ret = vpss_inst_regist_callback(instance, h_port, h_sink, h_src, fn_vpss_callback);
    return ret;
}

hi_void vpss_ctrl_context_init(hi_void)
{
    memset(&g_vpss_ctrl[VPSS_IP_0], 0, VPSS_IP_MAX * sizeof(vpss_ctrl));
    g_vpss_ctrl[VPSS_IP_0].ip_vaild = HI_TRUE;
    g_vpss_ctrl[VPSS_IP_0].ip = VPSS_IP_0;
    g_vpss_ctrl[VPSS_IP_0].vpss_irq_num = VPSS0_IRQ_NUM;
    g_vpss_ctrl[VPSS_IP_0].isr_process = vpss0_ctrl_int_service;
    g_vpss_ctrl[VPSS_IP_0].is_vpss_open = 0;
    memcpy(g_vpss_ctrl[VPSS_IP_0].isr_name, "int_vpss0", strlen("int_vpss0"));
    g_vpss_ctrl[VPSS_IP_1].ip_vaild = HI_FALSE;
    g_vpss_ctrl[VPSS_IP_1].ip = VPSS_IP_1;
    g_vpss_ctrl[VPSS_IP_1].vpss_irq_num = VPSS1_IRQ_NUM;
    g_vpss_ctrl[VPSS_IP_1].isr_process = vpss1_ctrl_int_service;
    g_vpss_ctrl[VPSS_IP_1].is_vpss_open = 0;
    memcpy(g_vpss_ctrl[VPSS_IP_1].isr_name, "int_vpss1", strlen("int_vpss1"));
    return;
}

hi_void vpss_ctrl_context_deinit(hi_void)
{
    memset(&g_vpss_ctrl[VPSS_IP_0], 0, VPSS_IP_MAX * sizeof(vpss_ctrl));
    g_vpss_ctrl[VPSS_IP_0].ip_vaild = HI_FALSE;
    g_vpss_ctrl[VPSS_IP_0].ip = VPSS_IP_0;
    g_vpss_ctrl[VPSS_IP_0].vpss_irq_num = VPSS0_IRQ_NUM;
    g_vpss_ctrl[VPSS_IP_0].isr_process = HI_NULL;
    g_vpss_ctrl[VPSS_IP_0].is_vpss_open = 0;
    memcpy(g_vpss_ctrl[VPSS_IP_0].isr_name, "int_vpss0", strlen("int_vpss0"));
    g_vpss_ctrl[VPSS_IP_1].ip_vaild = HI_FALSE;
    g_vpss_ctrl[VPSS_IP_1].ip = VPSS_IP_1;
    g_vpss_ctrl[VPSS_IP_1].vpss_irq_num = VPSS1_IRQ_NUM;
    g_vpss_ctrl[VPSS_IP_1].isr_process = HI_NULL;
    g_vpss_ctrl[VPSS_IP_1].is_vpss_open = 0;
    memcpy(g_vpss_ctrl[VPSS_IP_1].isr_name, "int_vpss1", strlen("int_vpss1"));
    return;
}

static hi_void vpss_ctrl_ip_deinit(vpss_ip tmp_ip, vpss_ip init_ip)
{
    for (tmp_ip = 0; tmp_ip < init_ip; tmp_ip++) {
        if (g_vpss_ctrl[tmp_ip].ip_vaild == HI_TRUE) {
            vpss_ctrl_un_regist_isr(tmp_ip);
            vpss_hal_deinit(tmp_ip);
            vpss_ctrl_destory_thread(tmp_ip);
            vpss_ctrl_destory_proc(tmp_ip);
            if (g_vpss_ctrl[init_ip].spin_lock.lock != HI_NULL) {
                vpss_comm_destory_spin(&(g_vpss_ctrl[init_ip].spin_lock));
            }
            if (g_vpss_ctrl[init_ip].inst_ctrl_info.list_lock.lock != HI_NULL) {
                vpss_comm_destory_spin(&(g_vpss_ctrl[init_ip].inst_ctrl_info.list_lock));
            }
        }
    }
}

static hi_s32 vpss_ctrl_init_spin_and_inst_list(vpss_ip init_ip)
{
    if (vpss_comm_init_spin(&(g_vpss_ctrl[init_ip].spin_lock)) != HI_SUCCESS) {
        vpss_error("Init g_vpss_ctrl spin_lock failed. init_ip:%d \n", init_ip);
        return HI_FAILURE;
    }

    if (vpss_ctrl_init_inst_list(init_ip) != HI_SUCCESS) {
        vpss_error("vpss_ctrl_init_inst_list failed.\n");
        vpss_comm_destory_spin(&(g_vpss_ctrl[init_ip].spin_lock));
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_init(hi_void)
{
    hi_s32 ret;
    vpss_ip init_ip;
    vpss_ip tmp_ip = 0;

    for (init_ip = 0; init_ip < VPSS_IP_MAX; init_ip++) {
        if (g_vpss_ctrl[init_ip].ip_vaild != HI_TRUE) {
            continue;
        }

        if (g_vpss_ctrl[init_ip].is_vpss_open == 0) {
            if (vpss_ctrl_regist_isr(init_ip) != HI_SUCCESS) {
                goto VPSS_IP_DEL_INT;
            }

            if (vpss_ctrl_init_spin_and_inst_list(init_ip) != HI_SUCCESS) {
                goto VPSS_IP_DEL_INT;
            }

            if (vpss_hal_init(init_ip) != HI_SUCCESS) {
                goto VPSS_IP_UN_REGIST_IRQ;
            }

            if (vpss_ctrl_create_thread(init_ip) != HI_SUCCESS) {
                vpss_error("vpss_ctrl_create_thread failed\n");
                goto VPSS_IP_HAL_DEL_INIT;
            }

            if (vpss_ctrl_create_proc(init_ip) != HI_SUCCESS) {
                vpss_error("vpss_ctrl_create_proc failed\n");
                goto VPSS_IP_DESTORY_THREAD;
            }
        }

        g_vpss_ctrl[init_ip].is_vpss_open++;
    }

    return HI_SUCCESS;
VPSS_IP_DESTORY_THREAD:
    ret = vpss_ctrl_destory_thread(init_ip);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_ctrl_destory_thread failed\n");
    }

VPSS_IP_HAL_DEL_INIT:
    ret = vpss_hal_deinit(init_ip);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_hal_deinit failed\n");
    }

VPSS_IP_UN_REGIST_IRQ:
    ret = vpss_ctrl_un_regist_isr(init_ip);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_ctrl_un_regist_isr failed\n");
    }

VPSS_IP_DEL_INT:
    vpss_ctrl_ip_deinit(tmp_ip, init_ip);
    return HI_FAILURE;
}

hi_s32 vpss_ctrl_deinit_last_open_time(vpss_ip init_ip, hi_u32 open_time)
{
    hi_u32 count;
    vpss_check_enum(init_ip, VPSS_IP_MAX);

    if (open_time == 1) {
        for (count = 0; count < VPSS_INSTANCE_MAX_NUM; count++) {
            if (g_vpss_ctrl[init_ip].inst_ctrl_info.inst_pool[count] != HI_NULL) {
                vpss_error("ctrl_del_init error,destroy instance first.\n");
                g_vpss_ctrl[init_ip].is_vpss_open++;
                return HI_FAILURE;
            }
        }
    }

    if (open_time == 0) {
        vpss_ctrl_un_regist_isr(init_ip);
        vpss_ctrl_destory_thread(init_ip);
        vpss_hal_deinit(init_ip);
        vpss_ctrl_destory_proc(init_ip);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_deinit(hi_void)
{
    vpss_ip init_ip;

    for (init_ip = 0; init_ip < VPSS_IP_MAX; init_ip++) {
        if (g_vpss_ctrl[init_ip].ip_vaild != HI_TRUE) {
            continue;
        }

        if (g_vpss_ctrl[init_ip].is_vpss_open < 1) {
            vpss_error("ctrl_del_init error,vpss hasn't initted,ip %d,open num %d.\n",
                       init_ip, g_vpss_ctrl[init_ip].is_vpss_open);
            return HI_FAILURE;
        }

        g_vpss_ctrl[init_ip].is_vpss_open--;
        vpss_ctrl_deinit_last_open_time(init_ip, g_vpss_ctrl[init_ip].is_vpss_open);
    }

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_dev_suspend(hi_void)
{
    vpss_ip ip;
    hi_s32 ret;

    for (ip = VPSS_IP_0; ip < VPSS_IP_MAX; ip++) {
        if (g_vpss_ctrl[ip].ip_vaild == HI_TRUE) {
            if (vpss_ctrl_pause(ip) != HI_SUCCESS) {
                vpss_error("vpss_ctrl_pause failed\n");
            }

            ret = vpss_hal_sys_init(ip, HI_FALSE);
            if (ret != HI_SUCCESS) {
                vpss_error(" set clock failed\n");
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 vpss_ctrl_dev_resume(hi_void)
{
    vpss_ip ip;
    hi_s32 ret;

    for (ip = VPSS_IP_0; ip < VPSS_IP_MAX; ip++) {
        if (g_vpss_ctrl[ip].ip_vaild == HI_TRUE) {
            ret = vpss_hal_sys_init(ip, HI_TRUE);
            if (ret != HI_SUCCESS) {
                vpss_error(" set clock failed\n");
            }

            ret = vpss_ctrl_resume(ip);
            if (ret != HI_SUCCESS) {
                vpss_error("vpss_ctrl_resume failed\n");
            }
        }
    }

    return HI_SUCCESS;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


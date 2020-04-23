/*
 * * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * * Description: win_frc Component
 * * Author: image team
 * * Create: 2019-07-2
 * *
 */

#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_osal.h"
#include "drv_win_frc.h"

#ifndef __VDP_UT__
#define HI_ERR_SOFT_FRC(fmt...)  HI_PRINT(fmt)
#else
#define HI_ERR_SOFT_FRC(fmt...)  printf(fmt)
#endif

#define SOFT_FRC_FRAME_RATE_60HZ 60000
#define MAX_FRC_NUM 64

#define ALG_FRC_PRECISION       32
#define ALG_FRC_BASE_PLAY_RATIO 256

/* maximum display rate:16 */
#define ALG_FRC_MAX_PLAY_RATIO (ALG_FRC_BASE_PLAY_RATIO * 256)

/* minimum display rate: 1/16 */
#define ALG_FRC_MIN_PLAY_RATIO 1

/* maximum display rate:16 */
#define ALG_FRC_MAX_SPEED_INTEGER 64
#define ALG_FRC_MAX_SPEED_DECIMAL 999

/* minimum display rate: 1/16 */
#define ALG_FRC_MIN_PLAY_RATIO 1

#define AVPLAY_ALG_ABS(x) (((x) < 0) ? -(x) : (x))

typedef struct {
    hi_u32 in_rate;     /* unit: frame */
    hi_u32 out_rate;    /* fresh rate */
    hi_u32 play_rate;   /* play rate */
    hi_u32 cur_id;      /* current insert or drop position in a FRC cycle */
    hi_u32 input_count; /* input counter */
} frc_soft_alg;

typedef struct {
    hi_u32 in_rate;   /* unit: frame */
    hi_u32 out_rate;  /* fresh rate */
    hi_u32 play_rate; /* play rate */
} frc_soft_cfg;

typedef struct {
    frc_soft_alg soft_alg;
    frc_soft_cfg soft_config;
    hi_u32 frm_state;
    hi_u32 speed_integer;
    hi_u32 speed_decimal;
} frc_soft_control;


typedef struct osal_list_head soft_frc_list;
typedef osal_spinlock soft_frc_spin;
typedef struct {
    soft_frc_list node;
    hi_handle frc_handle;
    frc_soft_control frc_control;
} soft_frc_node;

typedef struct {
    hi_bool b_frc_init;
    hi_u32 frc_num;
    soft_frc_spin frc_spin_lock;
    soft_frc_list frc_node_list;
} soft_frc_context;

static hi_void frc_soft_init(frc_soft_control *soft_frc_control)
{
    /* init frc */
    memset(soft_frc_control, 0, sizeof(frc_soft_control));

    soft_frc_control->frm_state = 0;
    soft_frc_control->soft_config.in_rate = SOFT_FRC_FRAME_RATE_60HZ;
    soft_frc_control->soft_config.out_rate = SOFT_FRC_FRAME_RATE_60HZ;
    soft_frc_control->soft_config.play_rate = ALG_FRC_BASE_PLAY_RATIO;
    soft_frc_control->speed_integer = 1;
    soft_frc_control->speed_decimal = 0;

    return;
}

static hi_void frc_soft_de_init(frc_soft_control *soft_frc_control)
{
    memset(&soft_frc_control->soft_alg, 0, sizeof(frc_soft_alg));
    return;
}

static hi_s32 frc_soft_reset(frc_soft_alg *soft_alg)
{
    memset(soft_alg, 0, sizeof(frc_soft_alg));

    return HI_SUCCESS;
}

static hi_void frc_soft_get_t_play_rate(frc_soft_control *soft_control)
{
    hi_u32 rate_precision = 1000;

    soft_control->soft_config.play_rate =
        (soft_control->speed_integer * rate_precision + soft_control->speed_decimal) *
        ALG_FRC_BASE_PLAY_RATIO / rate_precision;

    if ((soft_control->soft_config.play_rate > ALG_FRC_MAX_PLAY_RATIO)
        || (soft_control->soft_config.play_rate < ALG_FRC_MIN_PLAY_RATIO)) {
        HI_ERR_SOFT_FRC("vo frc invalid play rate %d\n", soft_control->soft_config.play_rate);
        soft_control->soft_config.play_rate = ALG_FRC_BASE_PLAY_RATIO;
        return;
    }

    return;
}

static hi_u64 DIV_64(hi_u64 rg_a, hi_u64 rg_b)
{
#if (!defined __DISP_PLATFORM_BOOT__)
    return osal_div64_u64(rg_a, rg_b);
#else
    hi_u64 u64tmp = rg_a / rg_b;
    return u64tmp;
#endif
}

static hi_void frc_soft_get_frc_flag(frc_soft_alg *h_frc, hi_u32 in_rate, hi_u32 quot, hi_u32 remainder,
    hi_bool *pb_flag)
{
    hi_u64 tmp = 0;
    hi_u32 cur_id; /* current insert or drop position in a FRC cycle */
    hi_u64 remainder_cycle;

    remainder_cycle = (hi_u64)(h_frc->input_count % in_rate + 1);
    cur_id = h_frc->cur_id;
    /* if remder==0 */
    *pb_flag = HI_FALSE;

    if (remainder == 0) {
        *pb_flag = HI_FALSE;
    } else {
#if (defined __VDP_UT__)
        tmp = (hi_u64)(((hi_u64)in_rate * (hi_u64)cur_id + (remainder >> 1)) / remainder);
#else
        tmp = DIV_64(((hi_u64)in_rate * (hi_u64)cur_id + (remainder >> 1)), remainder);
#endif
        if (tmp == remainder_cycle) {
            *pb_flag = HI_TRUE;
            h_frc->cur_id++;
            h_frc->cur_id = (h_frc->cur_id % remainder == 0) ? remainder : (h_frc->cur_id % remainder);
        } else {
            *pb_flag = HI_FALSE;
        }
    }

    return;
}

static hi_void frc_soft_increase_frame_rate(frc_soft_alg *h_frc, hi_u32 in_rate, hi_u32 out_rate,
    hi_s32 *ps32_frm_state)
{
    hi_u32 quot;
    hi_u32 remainder;
    hi_bool b_flag = HI_FALSE;

    quot = (out_rate) / (in_rate);
    remainder = (out_rate) % (in_rate);

    frc_soft_get_frc_flag(h_frc, in_rate, quot, remainder, &b_flag);
    if (b_flag == HI_TRUE) {
        // repeat time: (quot-1)+1;
        *ps32_frm_state = quot;
    } else {
        // repeat time: (quot-1);
        *ps32_frm_state = quot - 1;
    }

    return;
}

static hi_void frc_soft_decrease_frame_rate(frc_soft_alg *h_frc, hi_u32 in_rate, hi_u32 out_rate,
    hi_s32 *ps32_frm_state)
{
    hi_u32 quot;
    hi_u32 remainder;
    hi_bool b_flag = HI_FALSE;

    quot = (in_rate) / (out_rate);
    remainder = (in_rate) - (out_rate);
    frc_soft_get_frc_flag(h_frc, in_rate, quot, remainder, &b_flag);

    // if flag==1, need drop this frame.
    if (b_flag == HI_TRUE) {
        *ps32_frm_state = -1;
    } else {
        *ps32_frm_state = 0;
    }

    return;
}
static hi_void frc_soft_update_frc_para(frc_soft_cfg *soft_config, frc_soft_alg *h_frc)
{
    if ((h_frc->out_rate != soft_config->out_rate)
        || (h_frc->play_rate != soft_config->play_rate)
        || (h_frc->in_rate != soft_config->in_rate)) {
        // RESET
        frc_soft_reset(h_frc);
        h_frc->in_rate = soft_config->in_rate;
        h_frc->out_rate = soft_config->out_rate;
        h_frc->play_rate = soft_config->play_rate;
        h_frc->cur_id = 1; /* this should be 0 or 1 ? */
    }

    return;
}
static hi_void frc_soft_calculate(frc_soft_control *soft_frc_control)
{
    hi_u32 in_rate;
    hi_u32 out_rate;
    hi_u64 out_rate_temp;
    hi_u32 mp_frame_rate;
    hi_u32 play_rate;
    frc_soft_alg *h_frc = &soft_frc_control->soft_alg;
    frc_soft_cfg *pst_frc_cfg = &soft_frc_control->soft_config;

    mp_frame_rate = pst_frc_cfg->in_rate;
    soft_frc_control->frm_state = 0;
    /* to support stream which rate is less tha 6 frame per second/ */
    if (mp_frame_rate < 1) {
        soft_frc_control->frm_state = 0;
        return;
    }

    frc_soft_get_t_play_rate(soft_frc_control);
    frc_soft_update_frc_para(pst_frc_cfg, h_frc);

    play_rate = pst_frc_cfg->play_rate;  // 8bit fraction
    /* support  256X playrate */
    out_rate_temp = (hi_u64)h_frc->out_rate;
    out_rate_temp = out_rate_temp * ALG_FRC_PRECISION * ALG_FRC_BASE_PLAY_RATIO;

#if (defined __VDP_UT__)
    out_rate_temp = (hi_u64)(out_rate_temp / play_rate);
#else
    out_rate_temp = DIV_64(out_rate_temp, play_rate);
#endif

    out_rate = (hi_u32)out_rate_temp;
    in_rate = (h_frc->in_rate) * ALG_FRC_PRECISION;

    if ((in_rate) < (out_rate)) {
        /* lower framerate to higher framerate ---need repeat */
        frc_soft_increase_frame_rate(h_frc, in_rate, out_rate, &soft_frc_control->frm_state);
    } else if (in_rate > out_rate) {
        /* higher framerate to lower framerate ---need drop */
        frc_soft_decrease_frame_rate(h_frc, in_rate, out_rate, &soft_frc_control->frm_state);
    } else {  // don't need frame rate conversion
        soft_frc_control->frm_state = 0;
    }

    h_frc->input_count++;

    return;
}

static soft_frc_context g_soft_frc_context;

static hi_s32 soft_frc_list_deinit(soft_frc_list *frc_node_list)
{
    soft_frc_list *pos = HI_NULL;
    soft_frc_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret;
    soft_frc_node *frc_node = HI_NULL;

    ret = HI_SUCCESS;
    osal_list_for_each_safe(pos, n, frc_node_list)
    {
        frc_node = osal_list_entry(pos, soft_frc_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_FRC_NUM) {
            HI_ERR_SOFT_FRC("list error\n");
            ret = HI_FAILURE;
            break;
        }

        HI_ERR_SOFT_FRC("frc %d destroy error!!!!!\n", frc_node->frc_handle);
        osal_list_del_init(pos);
        osal_kfree(HI_ID_WIN, frc_node);
        ret = HI_FAILURE;
    }

    return ret;
}

static hi_bool check_frc_handle_valid(soft_frc_list *frc_node_list, hi_handle frc_handle)
{
    soft_frc_list *pos = HI_NULL;
    soft_frc_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_bool check_result = HI_TRUE;
    soft_frc_node *frc_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frc_node_list)
    {
        frc_node = osal_list_entry(pos, soft_frc_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_FRC_NUM) {
            HI_ERR_SOFT_FRC("list error\n");
            check_result = HI_FALSE;
            break;
        }

        if (frc_node->frc_handle == frc_handle) {
            check_result = HI_FALSE;
            break;
        }
    }

    return check_result;
}

static soft_frc_node *get_soft_frc_node(soft_frc_list *frc_node_list, hi_handle frc_handle)
{
    soft_frc_list *pos = HI_NULL;
    soft_frc_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    soft_frc_node *frc_node = HI_NULL;

    osal_list_for_each_safe(pos, n, frc_node_list)
    {
        frc_node = osal_list_entry(pos, soft_frc_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_FRC_NUM) {
            HI_ERR_SOFT_FRC("list error\n");
            frc_node = HI_NULL;
            break;
        }
        if (frc_node->frc_handle == frc_handle) {
            break;
        } else {
            frc_node = HI_NULL;
        }
    }

    return frc_node;
}

hi_s32 win_frc_init(void)
{
    hi_s32 ret;
    // 加载ko
    if (g_soft_frc_context.b_frc_init == HI_TRUE) {
        HI_ERR_SOFT_FRC("frc has been inited, init flag is %d\n", g_soft_frc_context.b_frc_init);
        return HI_FAILURE;
    }

    memset(&g_soft_frc_context, 0, sizeof(soft_frc_context));
    OSAL_INIT_LIST_HEAD(&(g_soft_frc_context.frc_node_list));
    ret = osal_spin_lock_init(&(g_soft_frc_context.frc_spin_lock));
    if (ret != HI_SUCCESS) {
        HI_ERR_SOFT_FRC("frc lock initiation failed");
        return HI_FAILURE;
    }
    g_soft_frc_context.b_frc_init = HI_TRUE;
    return HI_SUCCESS;
}

hi_s32 win_frc_deinit(void)
{
    hi_s32 ret;

    if (g_soft_frc_context.b_frc_init != HI_TRUE) {
        HI_ERR_SOFT_FRC("please init frc first, init flag is %d\n", g_soft_frc_context.b_frc_init);
        return HI_FAILURE;
    }
    // 卸载ko的时候可以不加锁保护该处
    ret = soft_frc_list_deinit(&(g_soft_frc_context.frc_node_list));
    osal_spin_lock_destory(&(g_soft_frc_context.frc_spin_lock));
    memset(&g_soft_frc_context, 0, sizeof(soft_frc_context));
    return ret;
}

hi_handle win_frc_create(void)
{
    unsigned long flags;
    soft_frc_node *frc_node = HI_NULL;
    hi_u32 try_get_count = 0;
    hi_handle frc_handle;

    if (g_soft_frc_context.b_frc_init != HI_TRUE) {
        HI_ERR_SOFT_FRC("can not create frc when not init!!!!!!\n");
        return HI_INVALID_HANDLE;
    }

    osal_spin_lock_irqsave(&g_soft_frc_context.frc_spin_lock, &flags);
    while (1) {
        g_soft_frc_context.frc_num++;
        frc_handle = g_soft_frc_context.frc_num;
        try_get_count++;
        if (check_frc_handle_valid(&(g_soft_frc_context.frc_node_list), g_soft_frc_context.frc_num) == HI_TRUE) {
            break;
        }
        if (try_get_count > MAX_FRC_NUM) {
            break;
        }

    }
    osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);

    if (try_get_count > MAX_FRC_NUM) {
        HI_ERR_SOFT_FRC("try %d times, can not find valid handle\n", try_get_count);
        return HI_INVALID_HANDLE;
    }

    frc_node = (soft_frc_node *)osal_kmalloc(HI_ID_WIN, sizeof(soft_frc_node), OSAL_GFP_KERNEL);
    if (frc_node == HI_NULL) {
        HI_ERR_SOFT_FRC("frc node malloc failed\n");
        return HI_INVALID_HANDLE;
    }

    memset(frc_node, 0, sizeof(soft_frc_node));
    osal_spin_lock_irqsave(&g_soft_frc_context.frc_spin_lock, &flags);
    osal_list_add_tail(&(frc_node->node), &(g_soft_frc_context.frc_node_list));
    frc_node->frc_handle = frc_handle;
    frc_soft_init(&frc_node->frc_control);
    osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);

    return frc_node->frc_handle;
}

hi_s32 win_frc_destory(hi_handle h_frc)
{
    unsigned long flags;
    soft_frc_list *pos = HI_NULL;
    soft_frc_list *n = HI_NULL;
    hi_u32 list_entry_times = 0;
    hi_s32 ret = HI_FAILURE;
    soft_frc_node *frc_node = HI_NULL;

    if (g_soft_frc_context.b_frc_init != HI_TRUE) {
        HI_ERR_SOFT_FRC("can not destroy frc when not init!!!!!!\n");
        return HI_FAILURE;
    }

    osal_spin_lock_irqsave(&g_soft_frc_context.frc_spin_lock, &flags);
    osal_list_for_each_safe(pos, n, &(g_soft_frc_context.frc_node_list))
    {
        frc_node = osal_list_entry(pos, soft_frc_node, node);
        list_entry_times++;
        if (list_entry_times > MAX_FRC_NUM) {
            HI_ERR_SOFT_FRC("list error\n");
            ret = HI_FAILURE;
            break;
        }
        if (frc_node->frc_handle == h_frc) {
            osal_list_del_init(pos);
            ret = HI_SUCCESS;
            break;
        }
    }
    osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);

    if (ret == HI_SUCCESS) {
        frc_soft_de_init(&frc_node->frc_control);
        osal_kfree(HI_ID_WIN, frc_node);
        return HI_SUCCESS;
    } else {
        HI_ERR_SOFT_FRC("destroy frc error!!!!!!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 win_frc_calcute(hi_handle h_frc, win_frc_input_attr *in_attr, win_frc_output_result *result)
{
    unsigned long flags;
    soft_frc_node *frc_node = HI_NULL;

    osal_spin_lock_irqsave(&g_soft_frc_context.frc_spin_lock, &flags);
    frc_node = get_soft_frc_node(&(g_soft_frc_context.frc_node_list), h_frc);
    if (frc_node == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);
        HI_ERR_SOFT_FRC("can not find frc context, h_frc is %d\n", h_frc);
        return HI_FAILURE;
    }

    frc_node->frc_control.soft_config.in_rate = in_attr->in_rate;
    frc_node->frc_control.soft_config.out_rate = in_attr->out_rate;
    frc_node->frc_control.speed_integer = in_attr->speed_integer;
    frc_node->frc_control.speed_decimal = in_attr->speed_decimal;
    frc_soft_calculate(&frc_node->frc_control);
    result->need_play_cnt = (hi_u32)(frc_node->frc_control.frm_state + 1);
    osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);
    return HI_SUCCESS;
}

hi_void win_frc_proc_info(hi_handle h_frc, struct seq_file *p)
{
    unsigned long flags;
    soft_frc_node *frc_node = HI_NULL;
    soft_frc_node temp_frc_node;

    HI_PROC_PRINT(p, "------------------------------------frc proc info-----------------------------------\n");
    osal_spin_lock_irqsave(&g_soft_frc_context.frc_spin_lock, &flags);
    frc_node = get_soft_frc_node(&(g_soft_frc_context.frc_node_list), h_frc);
    if (frc_node == HI_NULL) {
        osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);
        HI_ERR_SOFT_FRC("can not find frc context, h_frc is %d\n", h_frc);
        return;
    }

    memcpy(&temp_frc_node, frc_node, sizeof(soft_frc_node));
    osal_spin_unlock_irqrestore(&g_soft_frc_context.frc_spin_lock, &flags);

    HI_PROC_PRINT(p, "%-40s:0x%-8x/%-10d/%-10d/%-10d\n", "handle/in_rate/out_rate/play_rate",
        temp_frc_node.frc_handle,
        temp_frc_node.frc_control.soft_config.in_rate,
        temp_frc_node.frc_control.soft_config.out_rate,
        temp_frc_node.frc_control.soft_config.play_rate);
    HI_PROC_PRINT(p, "%-40s:%-10d\n", "cur_play_cnt",
        (temp_frc_node.frc_control.frm_state + 1));
    return;
}

/*************************test code end *********************************/


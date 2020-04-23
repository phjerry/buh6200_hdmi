/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: p00370051
* Create: 2019/06/01
* Notes:
* History: 2019-06-2 p00370051 CSEC
*/

#include <asm/uaccess.h>

#include "linux/hisilicon/securec.h"
#include "hi_debug.h"
#include "hi_drv_sys.h"
#include "hi_drv_video.h"
#include "drv_sys_ioctl.h"
#include "drv_sys_policy.h"
#include "hi_osal.h"

#define HI_LOG_ERR(fmt...)  HI_ERR_PRINT(HI_ID_SYS, fmt)
#define HI_LOG_INFO(fmt...)  HI_INFO_PRINT(HI_ID_SYS, fmt)
#define HI_LOG_DBG(fmt...)  HI_DBG_PRINT(HI_ID_SYS, fmt)
#define hi_dbg_print_u32(val) HI_DBG_PRINT_U32(val)
#define hi_info_print_u32(val) HI_INFO_PRINT_U32(val)
#define hi_info_func_enter() HI_INFO_PRINT(" ===>[Enter]\n")
#define hi_info_func_exit()  HI_INFO_PRINT(" <===[Exit]\n")
#define hi_dbg_func_enter() HI_DBG_PRINT(" ===>[Enter]\n")
#define hi_dbg_func_exit()  HI_DBG_PRINT(" <===[Exit]\n")


#define SYS_POLICY_INVALID_VALUE    0xffffffff

#define SYS_POLICY_PROC    "policy"
#define HDR_STRATEGY_INFO_MAX 8

#define DRV_SYS_INVALID_ID (HDR_STRATEGY_INFO_MAX + 1)

hi_comm_hdr_strategy_info g_asg_st_hdr_strategy_info[HDR_STRATEGY_INFO_MAX] = {0};

comm_sys_policy_t g_policy[HI_SYS_POLICY_TYPE_BUTT + 1] = {0};

hi_bool g_print_hdr_info = HI_FALSE;

DEFINE_SPINLOCK(g_sys_hdr_strategy_lock);
DEFINE_SPINLOCK(g_sys_policy_lock);

#define SYS_DRV_HDR_STRATEGY_LOCK(flags)   spin_lock_irqsave(&g_sys_hdr_strategy_lock, flags)
#define SYS_DRV_HDR_STRATEGY_UNLOCK(flags) spin_unlock_irqrestore(&g_sys_hdr_strategy_lock, flags)

#define SYS_DRV_POLICY_LOCK(flags)   spin_lock_irqsave(&g_sys_policy_lock, flags)
#define SYS_DRV_POLICY_UNLOCK(flags) spin_unlock_irqrestore(&g_sys_policy_lock, flags)

typedef struct hi_comm_hdr_strategy_info {
    hi_bool b_vaild;
    HI_MOD_ID_E enModId;
    hi_handle h_source;
    hi_handle h_window;
    hi_bool b_master_window;
    hi_bool b_original_video;
    hi_u32 priority; /* 当前的优先级，默认为0.数字越大，优先级越高 */
    hi_drv_hdr_strategy_info st_hdr_strategy_info;
} hi_comm_hdr_strategy_info;

hi_void drv_sys_policy_init(hi_void)
{
    hi_u32 i;

    hi_info_func_enter();

    for (i = 0; i < HI_SYS_POLICY_TYPE_BUTT; i++) {
        g_policy[i].policy_info.policy_valid = HI_FALSE;
        g_policy[i].policy_info.policy_value = SYS_POLICY_INVALID_VALUE;
        g_policy[i].policy_owner = HI_NULL;
    }

    hi_info_func_exit();

    return;
}

hi_void drv_sys_hdr_strategy_init(hi_void)
{
    hi_u32 i;
    errno_t err_ret;

    hi_info_func_enter();

    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        g_asg_st_hdr_strategy_info[i].b_master_window = HI_FALSE;
        g_asg_st_hdr_strategy_info[i].b_original_video = HI_FALSE;
        g_asg_st_hdr_strategy_info[i].b_vaild = HI_FALSE;
        g_asg_st_hdr_strategy_info[i].h_window = HI_INVALID_HANDLE;
        g_asg_st_hdr_strategy_info[i].h_source = HI_INVALID_HANDLE;
        g_asg_st_hdr_strategy_info[i].enModId = HI_ID_BUTT;
        g_asg_st_hdr_strategy_info[i].priority = 0;
        err_ret = memset_s(&g_asg_st_hdr_strategy_info[i].st_hdr_strategy_info,
            sizeof(g_asg_st_hdr_strategy_info[i].st_hdr_strategy_info),  0x0,
            sizeof(hi_drv_hdr_strategy_info));
        if (err_ret != EOK) {
            HI_LOG_ERR("secure func call error\n");
            return;
        }
    }
    hi_info_func_exit();

    return;
}

hi_void drv_sys_print_hdr_strategy(hi_void)
{
    hi_u32 i;

    if (g_print_hdr_info == HI_TRUE) {
        for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
            HI_LOG_INFO("[%d] mian=%-5s,Original=%-5s, bVaild=%-5s,  hSource=0x%-8x, "
                        "hWindow=0x%-8x, enModId=0x%-8x,u32Priority=0x%-8x, Hdr=0x%-8x\n",
                        i,
                        g_asg_st_hdr_strategy_info[i].b_master_window == HI_TRUE ? "True" : "False",
                        g_asg_st_hdr_strategy_info[i].b_original_video == HI_TRUE ? "True" : "False",
                        g_asg_st_hdr_strategy_info[i].b_vaild == HI_TRUE ? "True" : "False",
                        g_asg_st_hdr_strategy_info[i].h_source, g_asg_st_hdr_strategy_info[i].h_window,
                        g_asg_st_hdr_strategy_info[i].enModId, g_asg_st_hdr_strategy_info[i].priority,
                        g_asg_st_hdr_strategy_info[i].st_hdr_strategy_info.hdr_type);
        }
    }
    return;
}

static hi_s32 drv_sys_get_window_type(hi_handle h_window, hi_bool *pb_main)
{
    hi_u32 i;

    hi_info_func_enter();

    /* 原生通路一定是主通路 */
    *pb_main = HI_TRUE;

    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        /* 基于先验规则，当前的类型与之前的一致 */
        if (g_asg_st_hdr_strategy_info[i].h_window == h_window) {
            hi_info_print_h32(g_asg_st_hdr_strategy_info[i].h_window);
            hi_info_print_u32(i);
            *pb_main = g_asg_st_hdr_strategy_info[i].b_master_window;
            break;
        }
    }

    hi_info_print_bool(*pb_main);
    hi_info_func_exit();

    return HI_SUCCESS;
}

static hi_void drv_sys_get_priority_by_id(HI_MOD_ID_E enModId, hi_u32 *pu32_priority)
{
    hi_info_func_enter();

    switch (enModId) {
        case HI_ID_WIN: {
            *pu32_priority = 0x4;
            break;
        }

        case HI_ID_VICAP: {
            *pu32_priority = 0x3;
            break;
        }

        case HI_ID_AVPLAY: {
            *pu32_priority = 0x2;
            break;
        }

        case HI_ID_VDEC: {
            *pu32_priority = 0x1;
            break;
        }

        default:
        {
            *pu32_priority = 0x0;
            break;
        }
    }

    hi_info_print_u32(*pu32_priority);
    hi_info_func_exit();

    return;
}

static hi_s32 drv_sys_get_hdr_strategy_index(hi_u32 *pu32_vaild_index)
{
    hi_u32 i = 0;
    hi_u32 vaild_main_index = DRV_SYS_INVALID_ID;
    hi_u32 vaild_other_index = DRV_SYS_INVALID_ID;
    hi_u32 mian_priority = 0;
    hi_u32 other_priority = 0;

    hi_dbg_func_enter();

    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        /* 1、先根据通路属性找到对应的有效ID */
        if (g_asg_st_hdr_strategy_info[i].b_vaild == HI_TRUE) {
            /* 2、 优先查找主window有效的值,并根据优先级，给出优先级高的有效ID */
            if (g_asg_st_hdr_strategy_info[i].b_master_window == HI_TRUE &&
                g_asg_st_hdr_strategy_info[i].priority > mian_priority) {
                vaild_main_index = i;
                mian_priority = g_asg_st_hdr_strategy_info[i].priority;
                hi_dbg_print_u32(vaild_main_index);
                hi_dbg_print_u32(mian_priority);
            }

            /* 3、如果主window不存在，根据优先级，给出优先级高的有效ID */
            if (g_asg_st_hdr_strategy_info[i].priority > other_priority) {
                vaild_other_index = i;
                other_priority = g_asg_st_hdr_strategy_info[i].priority;
                hi_dbg_print_u32(vaild_other_index);
                hi_dbg_print_u32(other_priority);
            }
        }
    }

    if (vaild_main_index != DRV_SYS_INVALID_ID) {
        *pu32_vaild_index = vaild_main_index;
    } else if (vaild_other_index != DRV_SYS_INVALID_ID) {
        *pu32_vaild_index = vaild_other_index;
    } else {
        HI_LOG_INFO("no find.\n");
        return HI_FAILURE;
    }

    hi_dbg_func_exit();

    return HI_SUCCESS;
}

static hi_s32 drv_sys_get_valid_id(hi_handle h_handle, hi_u32 priority, hi_u32 *valid_id)
{
    hi_u32 i;
    hi_bool b_id_occupy = HI_FALSE;
    hi_bool b_win_occupy = HI_FALSE;

    /* 1、找到有绑定关系的id,退出.绑定的情况只有一种 */
    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        if (h_handle == g_asg_st_hdr_strategy_info[i].h_source) {
            *valid_id = i;
            HI_LOG_INFO("h_handle[%d]=0x%x.\n", i, h_handle);
            break;
        } 

        /* 2、没有找到绑定的源,但有window存在 */
        if (g_asg_st_hdr_strategy_info[i].h_source == HI_INVALID_HANDLE &&
            g_asg_st_hdr_strategy_info[i].h_window != HI_INVALID_HANDLE) {
            if (b_win_occupy == HI_FALSE) {
                *valid_id = i;
                b_win_occupy = HI_TRUE;
                HI_LOG_INFO("h_window[%d]=0x%x.\n", i, g_asg_st_hdr_strategy_info[i].h_window);
            }
        }

        /* 3、找到第一个没人用的数据用于记录无绑定源的情况 */
        if (g_asg_st_hdr_strategy_info[i].b_vaild == HI_FALSE &&
            g_asg_st_hdr_strategy_info[i].h_window == HI_INVALID_HANDLE) {
            /* 只记录第一个数据 */
            if (b_id_occupy == HI_FALSE && b_win_occupy == HI_FALSE) {
                *valid_id = i;
                b_id_occupy = HI_TRUE;
                hi_info_print_u32(*valid_id);
            }
        }
    }

    if (*valid_id == DRV_SYS_INVALID_ID) {
        HI_LOG_ERR("The valid hdrStrategyInfo is already more than max number!\n");
        return HI_FAILURE;
    }
    HI_LOG_INFO("====> WinType=%s, u32Priority=0x%x, u32ValidId=0x%x\n",
                g_asg_st_hdr_strategy_info[*valid_id].b_master_window == HI_TRUE ? "True" : "False",
                priority, *valid_id);

    return HI_SUCCESS;
}

static hi_void drv_sys_get_window_attach_id(hi_comm_window_attach_info *pst_win_attach_info,
    comm_window_attach_id *id_info, hi_bool *b_master_window)
{
    hi_u32 i;
    hi_bool b_id_occupy = HI_FALSE;
    hi_bool b_src_exist = HI_FALSE;
    hi_bool b_win_exist = HI_FALSE;

    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        /* 1、找到已有绑定情况的ID */
        if (g_asg_st_hdr_strategy_info[i].h_window == pst_win_attach_info->h_window &&
            g_asg_st_hdr_strategy_info[i].h_source == pst_win_attach_info->h_source) {
            id_info->match_id = i;
            hi_info_print_u32(id_info->match_id);
            break;
        }

        /* 2、找到第一个与源ID 一致的ID */
        if (g_asg_st_hdr_strategy_info[i].h_source == pst_win_attach_info->h_source &&
            g_asg_st_hdr_strategy_info[i].h_window == HI_INVALID_HANDLE) {
            /* 只记录第一个数据 */
            if (b_src_exist == HI_FALSE) {
                id_info->src_exist_id = i;
                b_src_exist = HI_TRUE;
                hi_info_print_u32(id_info->src_exist_id);
            }
        }

        /* 3、找到第一个与window 一致的ID */
        if (g_asg_st_hdr_strategy_info[i].h_window == pst_win_attach_info->h_window &&
            g_asg_st_hdr_strategy_info[i].h_source == HI_INVALID_HANDLE) {
            /* 只记录第一个数据 */
            if (b_win_exist == HI_FALSE) {
                id_info->wind_exist_id = i;
                b_win_exist = HI_TRUE;
                hi_info_print_u32(id_info->wind_exist_id);
            }
        }

        /* 4、找到第一个未被占用的ID */
        if (g_asg_st_hdr_strategy_info[i].h_window == HI_INVALID_HANDLE) {
            /* 只记录第一个数据 */
            if (b_id_occupy == HI_FALSE) {
                id_info->empty_id = i;
                b_id_occupy = HI_TRUE;
                hi_info_print_u32(id_info->empty_id);
            }
        }

        if (g_asg_st_hdr_strategy_info[i].h_window == pst_win_attach_info->h_window) {
            if (*b_master_window == HI_FALSE) {
                *b_master_window = g_asg_st_hdr_strategy_info[i].b_master_window;
            }
        }
    }
    return;
}

static hi_void drv_sys_get_hdr_strategy_id(hi_handle h_window,
    hi_u32 *wind_exist_id, hi_u32 *wind_no_exist_id)
{
    hi_u32 i;
    hi_u32 wind_exist_priority = 0;
    hi_u32 wind_no_exist_priority = 0;
    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        /* 1、根据window handle 找到匹配的handle */
        if (g_asg_st_hdr_strategy_info[i].h_window == h_window && g_asg_st_hdr_strategy_info[i].b_vaild == HI_TRUE) {
            /* 2、根据优先级，给出优先级高的有效ID */
            if (g_asg_st_hdr_strategy_info[i].priority > wind_exist_priority) {
                *wind_exist_id = i;
                wind_exist_priority = g_asg_st_hdr_strategy_info[i].priority;
                hi_dbg_print_u32(*wind_exist_id);
                hi_dbg_print_u32(wind_exist_priority);
            }
        }

        /* 无效ID */
        if (g_asg_st_hdr_strategy_info[i].h_window == HI_INVALID_HANDLE &&
            g_asg_st_hdr_strategy_info[i].b_vaild == HI_TRUE) {
            /* 2、根据优先级，给出优先级高的有效ID */
            if (g_asg_st_hdr_strategy_info[i].priority > wind_no_exist_priority) {
                *wind_no_exist_id = i;
                wind_no_exist_priority = g_asg_st_hdr_strategy_info[i].priority;
                hi_dbg_print_u32(*wind_no_exist_id);
                hi_dbg_print_u32(wind_no_exist_priority);
            }
        }
    }
    return;
}

static hi_void drv_sys_check_window_exist(hi_comm_window_info *pst_win_info, hi_bool *b_window_exist)
{
    hi_u32 i;

    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        HI_LOG_INFO("<--h_window[%d]=0x%x. h_source[%d]=0x%x-->\n", i, g_asg_st_hdr_strategy_info[i].h_window, i,
                    g_asg_st_hdr_strategy_info[i].h_source);

        /* 1、现在window已存在 */
        if (g_asg_st_hdr_strategy_info[i].h_window == pst_win_info->h_window) {
            /* 1.1、存在window且有绑定源 */
            if (g_asg_st_hdr_strategy_info[i].h_source != HI_INVALID_HANDLE) {
                g_asg_st_hdr_strategy_info[i].b_master_window = pst_win_info->b_master_window;
                g_asg_st_hdr_strategy_info[i].h_window = pst_win_info->h_window;
                *b_window_exist = HI_TRUE;
            } else { /* 1.2、存在但是没有绑定源 */
                g_asg_st_hdr_strategy_info[i].b_master_window = pst_win_info->b_master_window;
                g_asg_st_hdr_strategy_info[i].h_window = pst_win_info->h_window;
                g_asg_st_hdr_strategy_info[i].b_original_video = HI_TRUE;
                *b_window_exist = HI_TRUE;
            }
        }
    }

    return;
}

static hi_void drv_sys_set_window_info(hi_comm_window_info *pst_win_info,
    hi_bool b_window_exist, hi_bool *b_first_id)
{
    hi_u32 i;
    /* 2、不存在window */
    if (b_window_exist == HI_FALSE) {
        HI_LOG_INFO("not exist window=0x%x\n", pst_win_info->h_window);
        for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
            if (g_asg_st_hdr_strategy_info[i].h_window == HI_INVALID_HANDLE && *b_first_id == HI_TRUE) {
                /* 2.1、不存在window但是绑定源 */
                /* 2.2、不存且没有绑定源 */
                g_asg_st_hdr_strategy_info[i].b_master_window = pst_win_info->b_master_window;
                g_asg_st_hdr_strategy_info[i].h_window = pst_win_info->h_window;
                g_asg_st_hdr_strategy_info[i].b_original_video = HI_TRUE;
                *b_first_id = HI_FALSE;
            }
        }
    }
    return;
}

const HI_CHAR *drv_sys_module_id_to_name(HI_MOD_ID_E enModId)
{
    switch (enModId) {
        case HI_ID_WIN:
            return "VO";

        case HI_ID_VDEC:
            return "VDEC";

        case HI_ID_VICAP:
            return "VICAP";

        case HI_ID_AVPLAY:
            return "AVPLAY";

        default:
            return "none";
    }
}

const HI_CHAR *drv_sys_hdr_type_to_name(hi_drv_video_hdr_type en_hdr_type)
{
    switch (en_hdr_type) {
        case HI_DRV_VIDEO_HDR_TYPE_SDR:
            return "SDR";

        case HI_DRV_VIDEO_HDR_TYPE_DOLBY_BL:
            return "DOLBY_BL";

        case HI_DRV_VIDEO_HDR_TYPE_DOLBY_EL:
            return "DOLBY_EL";

        case HI_DRV_VIDEO_HDR_TYPE_HDR10:
            return "HDR10";

        case HI_DRV_VIDEO_HDR_TYPE_HLG:
            return "HLG";

        case HI_DRV_VIDEO_HDR_TYPE_SLF:
            return "SLF";

        case HI_DRV_VIDEO_HDR_TYPE_TECHNICOLOR:
            return "TECHNICOLOR";

        case HI_DRV_VIDEO_HDR_TYPE_HDR10PLUS:
            return "HDR10+";

        default:
            return "None";
    }
}

const HI_CHAR *drv_sys_policy_type_to_name(hi_sys_policy_type policy_type)
{
    switch (policy_type) {
        case HI_SYS_POLICY_TYPE_EXTRA_BUFF:
            return "EXTRA_BUFF";

        case HI_SYS_POLICY_TYPE_CTS_FLAG:
            return "CTS_FLAG";

        case HI_SYS_POLICY_TYPE_GTS_FLAG:
            return "GTS_FLAG";

        case HI_SYS_POLICY_TYPE_DISP_WIDTH:
            return "DISP_WIDTH";

        case HI_SYS_POLICY_TYPE_VCODEC_WIDTH:
            return "VCODEC_WIDTH";

        default:
            return "None";
    }
}

static hi_s32 drv_sys_policy_proc_show(hi_void *s, hi_void *private)
{
    hi_u32 i = 0;

    osal_proc_print(s,
        "----------------------------------hdr-policy-info------------------------------------------------\n");
    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        if (g_asg_st_hdr_strategy_info[i].st_hdr_strategy_info.hdr_type > HI_DRV_VIDEO_HDR_TYPE_BUTT) {
            g_asg_st_hdr_strategy_info[i].st_hdr_strategy_info.hdr_type = HI_DRV_VIDEO_HDR_TYPE_BUTT;
        }

        osal_proc_print(s,
            "[%d]-master=%-5s |original=%-5s |vaild=%-5s |h_src=0x%-8x "
            "|h_win=0x%-8x |mod_id=%-6s |priority=0x%-2x |hdr_type=%s\n",
            i,
            g_asg_st_hdr_strategy_info[i].b_master_window == HI_TRUE ? "true" : "false",
            g_asg_st_hdr_strategy_info[i].b_original_video == HI_TRUE ? "true" : "false",
            g_asg_st_hdr_strategy_info[i].b_vaild == HI_TRUE ? "true" : "false",
            g_asg_st_hdr_strategy_info[i].h_source, g_asg_st_hdr_strategy_info[i].h_window,
            drv_sys_module_id_to_name(g_asg_st_hdr_strategy_info[i].enModId),
            g_asg_st_hdr_strategy_info[i].priority,
            drv_sys_hdr_type_to_name(g_asg_st_hdr_strategy_info[i].st_hdr_strategy_info.hdr_type));
    }
    osal_proc_print(s,
        "\n----------------------------------common-policy-info-------------------------------------------\n");

    for (i = 0; i < HI_SYS_POLICY_TYPE_BUTT; i++) {
        osal_proc_print(s, "[%-12s] |vaild=%-5s |value=0x%-8x policy_owner:%p\n",
            drv_sys_policy_type_to_name(i),
            g_policy[i].policy_info.policy_valid == HI_TRUE ? "true" : "false",
            g_policy[i].policy_info.policy_value, g_policy[i].policy_owner);
    }

    osal_proc_print(s,
        "-------------------------------------------------------------------------------------------------\n\n");

    return 0;
}

static hi_s32 drv_sys_policy_osal_proc_print_help(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("\n--------------------------------- policy-----------------------------\n");
    osal_printk("echo command     para1   para2  path                  explanation\n");
    osal_printk("echo help           > /proc/msp/policy\n");
    osal_printk("echo print [0/1]    > /proc/msp/policy  print hdr info\n");
    osal_printk("-----------------------------------------------------------------------\n\n");

    return 0;
}

static hi_s32 drv_sys_policy_proc_set_hdr_printen(unsigned int argc,
    char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_ulong hdr_print;

    if (argc != 2) { /* 2 is param num */
        return -1;
    }

    hdr_print = osal_strtoul(argv[1], NULL, OSAL_BASE_DEC);
    g_print_hdr_info = (hdr_print == 0) ? HI_FALSE : HI_TRUE;

    return 0;
}


static hi_void drv_sys_policy_set_hdr_type(hdr_strategy_info_item *info_item,
    hi_drv_hdr_strategy_info *pst_hdr_strategy_info, hi_u32 valid_id, hi_u32 priority)
{
    errno_t err_ret;
    if (pst_hdr_strategy_info->vaild == HI_TRUE) {
        g_asg_st_hdr_strategy_info[valid_id].b_vaild = pst_hdr_strategy_info->vaild;
        g_asg_st_hdr_strategy_info[valid_id].priority = priority;
        g_asg_st_hdr_strategy_info[valid_id].enModId = info_item->mod_id;
        g_asg_st_hdr_strategy_info[valid_id].h_source = info_item->h_handle;
        /* 如果当前设置的是window，由于window之前没有绑定情况，在设置有效时设置默认绑定情况为自己与自己绑定 */
        if (info_item->mod_id == HI_ID_WIN) {
            drv_sys_get_window_type(info_item->h_handle, &g_asg_st_hdr_strategy_info[valid_id].b_master_window);
            g_asg_st_hdr_strategy_info[valid_id].h_window = info_item->h_handle;
        }
    } else {
        g_asg_st_hdr_strategy_info[valid_id].b_vaild = HI_FALSE;
        g_asg_st_hdr_strategy_info[valid_id].b_master_window = HI_FALSE;
        g_asg_st_hdr_strategy_info[valid_id].priority = 0;
        g_asg_st_hdr_strategy_info[valid_id].enModId = HI_ID_BUTT;

        /* 如果当前设置的是window，由于window之前没有绑定情况，在设置无效时清空window的绑定情况 */
        if (info_item->mod_id == HI_ID_WIN ||
            g_asg_st_hdr_strategy_info[valid_id].h_window == HI_INVALID_HANDLE ||
            g_asg_st_hdr_strategy_info[valid_id].b_original_video == HI_TRUE) {
            g_asg_st_hdr_strategy_info[valid_id].h_window = HI_INVALID_HANDLE;
            g_asg_st_hdr_strategy_info[valid_id].h_source = HI_INVALID_HANDLE;
        }

        g_asg_st_hdr_strategy_info[valid_id].b_original_video = HI_FALSE;
    }

    err_ret = memcpy_s(&g_asg_st_hdr_strategy_info[valid_id].st_hdr_strategy_info,
        sizeof(g_asg_st_hdr_strategy_info[valid_id].st_hdr_strategy_info),
        pst_hdr_strategy_info, sizeof(hi_drv_hdr_strategy_info));
    if (err_ret != EOK) {
        HI_LOG_ERR("secure func call error\n");
        return;
    }
    HI_LOG_INFO("StrategyInfo[%d].CurHdrType:%d, SetHdrType:%d\n", valid_id,
                g_asg_st_hdr_strategy_info[valid_id].st_hdr_strategy_info.hdr_type, pst_hdr_strategy_info->hdr_type);
}
hi_s32 hi_drv_sys_set_hdr_strategy_info(HI_MOD_ID_E enModId, hi_handle h_handle,
    hi_drv_hdr_strategy_info *pst_hdr_strategy_info)
{
    hi_s32 ret;
    hi_u32 priority = 0;
    hi_u32 valid_id = DRV_SYS_INVALID_ID;
    hdr_strategy_info_item  info_item;
    unsigned long flags;

    hi_info_func_enter();

    if (pst_hdr_strategy_info == HI_NULL || enModId >= HI_ID_BUTT) {
        HI_LOG_ERR("The param is out of range!\n");
        return HI_FAILURE;
    }

    HI_LOG_INFO("enModId=0x%x, h_handle=0x%x, b_vaild=%s.\n", enModId, h_handle,
                pst_hdr_strategy_info->vaild == HI_TRUE ? "True" : "False");

    SYS_DRV_HDR_STRATEGY_LOCK(flags);

#ifndef SYS_HDR_STRATEGY_INFO_UT
    if (enModId == HI_ID_VDEC) {
        h_handle = (HI_ID_VDEC << 16) | h_handle; /* 16 表示左移16位 */
        hi_info_print_h32(h_handle);
    }
#endif
    info_item.mod_id = enModId;
    info_item.h_handle = h_handle;

    drv_sys_print_hdr_strategy();

    drv_sys_get_priority_by_id(enModId, &priority);

    ret = drv_sys_get_valid_id(h_handle, priority, &valid_id);
    if (ret == HI_FAILURE) {
        SYS_DRV_HDR_STRATEGY_UNLOCK(flags);
        return ret;
    }

    drv_sys_policy_set_hdr_type(&info_item, pst_hdr_strategy_info, valid_id, priority);

    drv_sys_print_hdr_strategy();

    SYS_DRV_HDR_STRATEGY_UNLOCK(flags);

    hi_info_func_exit();
    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_hdr_strategy_info(hi_drv_hdr_strategy_info *pst_hdr_strategy_info)
{
    hi_s32 ret;
    hi_u32 vaild_index = 0;
    errno_t err_ret;

    hi_dbg_func_enter();

    if (pst_hdr_strategy_info == HI_NULL) {
        HI_LOG_ERR("get pst_hdr_strategy_info is null point!\n");
        return HI_FAILURE;
    }

    ret = drv_sys_get_hdr_strategy_index(&vaild_index);
    if (ret != HI_SUCCESS) {
        HI_LOG_INFO("get vaild hdr strategy info is error!\n");
        return HI_FAILURE;
    }

    err_ret =  memcpy_s(pst_hdr_strategy_info,  sizeof(hi_drv_hdr_strategy_info),
        &g_asg_st_hdr_strategy_info[vaild_index].st_hdr_strategy_info,
        sizeof(hi_drv_hdr_strategy_info));
    if (err_ret != EOK) {
        HI_LOG_ERR("secure func call error\n");
        return HI_FAILURE;
    }
    hi_dbg_func_exit();

    return HI_SUCCESS;
}


/* 添加window的绑定关系 */
hi_s32 hi_drv_sys_add_window_attach_info(hi_comm_window_attach_info *pst_win_attach_info)
{
    hi_u32 i = 0;
    unsigned long flags;
    hi_u32 valid_id = DRV_SYS_INVALID_ID;
    comm_window_attach_id id_info = {DRV_SYS_INVALID_ID, DRV_SYS_INVALID_ID,
        DRV_SYS_INVALID_ID, DRV_SYS_INVALID_ID};
    hi_bool b_master_window = HI_FALSE;

    hi_info_func_enter();

    if (pst_win_attach_info == HI_NULL) {
        HI_LOG_ERR("get pst_win_attach_info is null point!\n");
        return HI_FAILURE;
    }

    HI_LOG_INFO("h_window=0x%x, h_source=0x%x\n", pst_win_attach_info->h_window, pst_win_attach_info->h_source);

    SYS_DRV_HDR_STRATEGY_LOCK(flags);

    drv_sys_print_hdr_strategy();

    drv_sys_get_window_attach_id(pst_win_attach_info, &id_info, &b_master_window);

    if (id_info.match_id != DRV_SYS_INVALID_ID) {
        valid_id = id_info.match_id;
    } else if (id_info.src_exist_id != DRV_SYS_INVALID_ID) {
        valid_id = id_info.src_exist_id;
    } else if (id_info.wind_exist_id != DRV_SYS_INVALID_ID) {
        valid_id = id_info.wind_exist_id;
    } else if (id_info.empty_id != DRV_SYS_INVALID_ID) {
        valid_id = id_info.empty_id;
    } else {
        HI_LOG_ERR("add index is MAX, i=%u.\n", i);
        SYS_DRV_HDR_STRATEGY_UNLOCK(flags);
        return HI_FAILURE;
    }

    g_asg_st_hdr_strategy_info[valid_id].h_window = pst_win_attach_info->h_window;
    g_asg_st_hdr_strategy_info[valid_id].h_source = pst_win_attach_info->h_source;
    g_asg_st_hdr_strategy_info[valid_id].b_original_video = HI_FALSE;
    g_asg_st_hdr_strategy_info[valid_id].b_master_window = pst_win_attach_info->b_master_window;
    /* 继承主从关系 */
    if (b_master_window == HI_TRUE) {
        g_asg_st_hdr_strategy_info[valid_id].b_master_window = HI_TRUE;
    }

    HI_LOG_INFO("h_window[%d]=0x%x, h_source[%d]=0x%x\n", valid_id, g_asg_st_hdr_strategy_info[valid_id].h_window,
                valid_id, g_asg_st_hdr_strategy_info[valid_id].h_source);
    drv_sys_print_hdr_strategy();

    SYS_DRV_HDR_STRATEGY_UNLOCK(flags);
    hi_info_func_exit();

    return HI_SUCCESS;
}

/* 删除window的绑定关系 */
hi_s32 hi_drv_sys_del_window_attach_info(hi_comm_window_attach_info *pst_win_attach_info)
{
    hi_u32 i = 0;
    unsigned long flags;

    hi_info_func_enter();

    if (pst_win_attach_info == HI_NULL) {
        HI_LOG_ERR("get pst_win_attach_info is null point!\n");
        return HI_FAILURE;
    }

    SYS_DRV_HDR_STRATEGY_LOCK(flags);

    drv_sys_print_hdr_strategy();

    HI_LOG_INFO("h_window=0x%x, h_source=0x%x\n", pst_win_attach_info->h_window, pst_win_attach_info->h_source);
    for (i = 0; i < HDR_STRATEGY_INFO_MAX; i++) {
        HI_LOG_INFO("[%d] h_window=0x%x, h_source=0x%x\n", i, g_asg_st_hdr_strategy_info[i].h_window,
                    g_asg_st_hdr_strategy_info[i].h_source);
        if (g_asg_st_hdr_strategy_info[i].h_window == pst_win_attach_info->h_window &&
            g_asg_st_hdr_strategy_info[i].h_source == pst_win_attach_info->h_source) {
            break;
        }
    }

    if (i == HDR_STRATEGY_INFO_MAX) {
        HI_LOG_ERR("Del index is MAX i=%u.\n", i);
        SYS_DRV_HDR_STRATEGY_UNLOCK(flags);
        return HI_FAILURE;
    }

    g_asg_st_hdr_strategy_info[i].h_window = HI_INVALID_HANDLE;
    g_asg_st_hdr_strategy_info[i].h_source = HI_INVALID_HANDLE;
    g_asg_st_hdr_strategy_info[i].b_master_window = HI_FALSE;
    g_asg_st_hdr_strategy_info[i].b_vaild = HI_FALSE;
    g_asg_st_hdr_strategy_info[i].b_original_video = HI_FALSE;
    HI_LOG_INFO("h_window[%d]=0x%x, h_source[%d]=0x%x\n", i, g_asg_st_hdr_strategy_info[i].h_window, i,
                g_asg_st_hdr_strategy_info[i].h_source);
    drv_sys_print_hdr_strategy();

    SYS_DRV_HDR_STRATEGY_UNLOCK(flags);
    hi_info_func_exit();

    return HI_SUCCESS;
}

/* 通过window 的handle 获取到HDR信息 */
hi_s32 hi_drv_sys_get_hdr_strategy_by_window(hi_handle h_window, hi_drv_hdr_strategy_info *pst_hdr_strategy_info)
{
    errno_t err_ret;
    hi_u32 wind_exist_id = DRV_SYS_INVALID_ID;
    hi_u32 wind_no_exist_id = DRV_SYS_INVALID_ID;

    hi_dbg_func_enter();

    if (pst_hdr_strategy_info == HI_NULL) {
        HI_LOG_ERR("get pst_hdr_strategy_info is null point!\n");
        return HI_FAILURE;
    }

    hi_dbg_print_h32(h_window);

    drv_sys_get_hdr_strategy_id(h_window, &wind_exist_id, &wind_no_exist_id);

    /* 如果window有绑定关系，返回查到的信息 */
    if (wind_exist_id != DRV_SYS_INVALID_ID) {
        err_ret = memcpy_s(pst_hdr_strategy_info,  sizeof(hi_drv_hdr_strategy_info),
            &g_asg_st_hdr_strategy_info[wind_exist_id].st_hdr_strategy_info,
            sizeof(hi_drv_hdr_strategy_info));
        if (err_ret != EOK) {
            HI_LOG_ERR("secure func call error\n");
            return HI_FAILURE;
        }
        HI_LOG_DBG("index=%d, en_hdr_type=%d.\n", wind_exist_id,
                   g_asg_st_hdr_strategy_info[wind_exist_id].st_hdr_strategy_info.hdr_type);
        hi_dbg_func_enter();
        return HI_SUCCESS;
    }

    if (wind_no_exist_id != DRV_SYS_INVALID_ID) {
        err_ret = memcpy_s(pst_hdr_strategy_info,  sizeof(hi_drv_hdr_strategy_info),
            &g_asg_st_hdr_strategy_info[wind_no_exist_id].st_hdr_strategy_info,
            sizeof(hi_drv_hdr_strategy_info));
        if (err_ret != EOK) {
            HI_LOG_ERR("secure func call error\n");
            return HI_FAILURE;
        }
        HI_LOG_DBG("index=%d, en_hdr_type=%d.\n", wind_no_exist_id,
                   g_asg_st_hdr_strategy_info[wind_no_exist_id].st_hdr_strategy_info.hdr_type);
        hi_dbg_func_enter();
        return HI_SUCCESS;
    }

    HI_LOG_INFO("not find.\n");

    return HI_FAILURE;
}


/* 设置window 主从信息 */
hi_s32 hi_drv_sys_set_window_info(hi_comm_window_info *pst_win_info)
{
    hi_bool b_first_id = HI_TRUE;
    hi_bool b_window_exist = HI_FALSE;
    unsigned long flags;

    hi_info_func_enter();

    if (pst_win_info == HI_NULL) {
        HI_LOG_ERR("get pst_win_info is null point!\n");
        return HI_FAILURE;
    }

    SYS_DRV_HDR_STRATEGY_LOCK(flags);

    drv_sys_print_hdr_strategy();

    drv_sys_check_window_exist(pst_win_info, &b_window_exist);

    drv_sys_set_window_info(pst_win_info, b_window_exist, &b_first_id);

    drv_sys_print_hdr_strategy();
    SYS_DRV_HDR_STRATEGY_UNLOCK(flags);

    hi_info_func_exit();

    return HI_SUCCESS;
}

static osal_proc_cmd g_policy_proc_cmd[] = {
    { "help", drv_sys_policy_osal_proc_print_help },
    { "print", drv_sys_policy_proc_set_hdr_printen }
};

hi_s32 hi_drv_sys_policy_init(hi_void)
{
    osal_proc_entry *proc_item = NULL;

    proc_item = osal_proc_add(SYS_POLICY_PROC, strlen(SYS_POLICY_PROC));
    if (proc_item == NULL) {
        HI_LOG_ERR("add %s proc failed.\n", SYS_POLICY_PROC);
        return HI_FAILURE;
    }

    proc_item->read = drv_sys_policy_proc_show;
    proc_item->cmd_cnt = sizeof(g_policy_proc_cmd) / sizeof(osal_proc_cmd);
    proc_item->cmd_list = g_policy_proc_cmd;
    proc_item->private = NULL;

    drv_sys_policy_init();
    drv_sys_hdr_strategy_init();

    return HI_SUCCESS;
}

hi_void hi_drv_sys_policy_deinit(hi_void)
{
    osal_proc_remove(SYS_POLICY_PROC, strlen(SYS_POLICY_PROC));
}


hi_s32 hi_drv_sys_set_policy(hi_sys_policy_type policy_type, hi_sys_policy *policy)
{
    unsigned long flags;

    if (policy == HI_NULL) {
        HI_LOG_ERR("set policy is null point!\n");
        return HI_FAILURE;
    }

    if (policy_type >= HI_SYS_POLICY_TYPE_BUTT || policy_type < HI_SYS_POLICY_TYPE_EXTRA_BUFF) {
        HI_LOG_ERR("policy_type is error, type:%d!\n", policy_type);
        return HI_FAILURE;
    }

    SYS_DRV_POLICY_LOCK(flags);
    g_policy[policy_type].policy_info.policy_valid = policy->policy_valid;
    g_policy[policy_type].policy_info.policy_value = policy->policy_value;

    if (policy->policy_valid == HI_FALSE) {
        g_policy[policy_type].policy_info.policy_value = SYS_POLICY_INVALID_VALUE;
    }
    SYS_DRV_POLICY_UNLOCK(flags);

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_policy(hi_sys_policy_type policy_type, hi_sys_policy *policy)
{
    unsigned long flags;

    if (policy == HI_NULL) {
        HI_LOG_ERR("get policy is null point!\n");
        return HI_FAILURE;
    }

    if (policy_type >= HI_SYS_POLICY_TYPE_BUTT || policy_type < HI_SYS_POLICY_TYPE_EXTRA_BUFF) {
        HI_LOG_ERR("policy type is error, type:%d!\n", policy_type);
        return HI_FAILURE;
    }

    SYS_DRV_POLICY_LOCK(flags);
    policy->policy_valid = g_policy[policy_type].policy_info.policy_valid;
    policy->policy_value = g_policy[policy_type].policy_info.policy_value;
    SYS_DRV_POLICY_UNLOCK(flags);

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_set_policy_file(hi_sys_policy_type policy_type, hi_void *policy_owner)
{
    unsigned long flags;

    if (policy_owner == HI_NULL) {
        HI_LOG_ERR("set policy_owner is null point!\n");
        return HI_FAILURE;
    }

    if (policy_type >= HI_SYS_POLICY_TYPE_BUTT || policy_type < HI_SYS_POLICY_TYPE_EXTRA_BUFF) {
        HI_LOG_ERR("policy_type is error, type:%d!\n", policy_type);
        return HI_FAILURE;
    }

    SYS_DRV_POLICY_LOCK(flags);
    g_policy[policy_type].policy_owner = policy_owner;
    SYS_DRV_POLICY_UNLOCK(flags);

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_clean_policy_by_file(hi_void *policy_owner)
{
    hi_u32 i;
    unsigned long flags;

    if (policy_owner == HI_NULL) {
        HI_LOG_ERR("set policy_owner is null point!\n");
        return HI_FAILURE;
    }
    SYS_DRV_POLICY_LOCK(flags);
    for (i = 0; i < HI_SYS_POLICY_TYPE_BUTT; i++) {
        if (g_policy[i].policy_owner == policy_owner) {
            g_policy[i].policy_owner = HI_NULL;
            g_policy[i].policy_info.policy_valid = HI_FALSE;
        }
    }
    SYS_DRV_POLICY_UNLOCK(flags);

    return HI_SUCCESS;
}


EXPORT_SYMBOL(hi_drv_sys_set_hdr_strategy_info);
EXPORT_SYMBOL(hi_drv_sys_get_hdr_strategy_info);

EXPORT_SYMBOL(hi_drv_sys_add_window_attach_info);
EXPORT_SYMBOL(hi_drv_sys_del_window_attach_info);
EXPORT_SYMBOL(hi_drv_sys_set_window_info);
EXPORT_SYMBOL(hi_drv_sys_get_hdr_strategy_by_window);
EXPORT_SYMBOL(hi_drv_sys_set_policy);
EXPORT_SYMBOL(hi_drv_sys_get_policy);



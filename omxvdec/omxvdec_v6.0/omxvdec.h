/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */


#ifndef __OMXVDEC_H__
#define __OMXVDEC_H__

#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include "hi_drv_sys.h"
#include "drv_omxvdec.h"
#include "omxvdec_mem.h"
#include "hi_drv_module.h"  // hi_drv_module_get_function
#include "hi_osal.h"

#define MAX_OPEN_COUNT  32
#define MAX_CHANNEL_NUM (MAX_OPEN_COUNT)
#define OMXVDEC_MAX_EXT_BUF_NUM 32

#define OMX_ALWS    0xFFFFFFFF
#define OMX_FATAL   0
#define OMX_ERR     1
#define OMX_WARN    2
#define OMX_INFO    3
#define OMX_TRACE   4
#define OMX_INBUF   5
#define OMX_OUTBUF  6
#define OMX_VPSS    7
#define OMX_VER     8
#define OMX_PTS     9

#define OMX_LOWDELAY_REC_NODE_NUM            10
#define OMX_LOWDELAY_REC_ITEM                9
#define OMX_LOWDELAY_REC_USERTAG             0
#define OMX_LOWDELAY_REC_NODE_STATE          1
#define OMX_LOWDELAY_REC_ETB_TIME            2
#define OMX_LOWDELAY_REC_VFMW_RCV_STRM_TIME  3
#define OMX_LOWDELAY_REC_VFMW_RLS_STRM_TIME  4
#define OMX_LOWDELAY_REC_VFMW_RPO_IMG_TIME   5
#define OMX_LOWDELAY_REC_VPSS_RCV_IMG_TIME   6
#define OMX_LOWDELAY_REC_VPSS_RPO_IMG_TIME   7
#define OMX_LOWDELAY_REC_TOTAL_USED_TIME     8
#define OMX_LOWDELAY_REC_NODE_FREE           0
#define OMX_LOWDELAY_REC_NODE_WRITED         1

extern hi_u32 g_trace_option;

#if (HI_PROC_SUPPORT == 1)
#define omx_print(flag, format, arg...)                                    \
    do {                                                                  \
        if (OMX_ALWS == flag || OMX_FATAL == flag || OMX_ERR == flag ||(0 != (g_trace_option & (1 << flag))))     \
            osal_printk("<OMXVDEC:%s:%d> " format, __func__, __LINE__, ##arg); \
    } while (0)

#define proc_print(fmt...) seq_printf(fmt)
#else
#define omx_print(flag, format, arg...) ({do {}while (0);0; })
#define proc_print(fmt...)
#endif

#define TRACE()     omx_print(OMX_TRACE, "\n")
#define FUNC_IN()   omx_print(OMX_TRACE, " IN\n")
#define FUNC_EXIT() omx_print(OMX_TRACE, " EXIT\n")

#define OMXVDEC_ASSERT_RETURN(cond, else_print)            \
    do {                                                   \
        if (!(cond)) {                                     \
            omx_print(OMX_ALWS, "ASSERT:%s\n", else_print); \
            return HI_FAILURE;                             \
        }                                                  \
    } while (0)

#define OMXVDEC_ASSERT_RETURN_NULL(cond, else_print)       \
    do {                                                   \
        if (!(cond)) {                                     \
            omx_print(OMX_ALWS, "ASSERT:%s\n", else_print); \
            return;                                        \
        }                                                  \
    } while (0)

#define HI_CHECK_SEC_FUNC(func){ \
    if((func) != EOK) { \
        omx_print(OMX_ERR, "check  err\n"); \
    } \
}

#define UINT64_PTR(ptr) ((hi_void *)(hi_ulong)(ptr))
#define PTR_UINT64(ptr) ((hi_u64)(hi_ulong)(ptr))

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define UADDR         hi_u64
#else
#define UADDR         hi_u32
#endif

/*
    g_trace_option

    1:      OMX_FATAL
    2:      OMX_ERR
    4:      OMX_WARN
    8:      OMX_INFO      (常用于查看创建配置项及跟踪最后一帧情况)
    16:     OMX_TRACE
    32:     OMX_INBUF
    64:     OMX_OUTBUF
    128:    OMX_VPSS      (常用于跟踪图像不输出原因)
    256:    OMX_RAWCTRL   (原用于码流输入控制，现已废弃)
    512:    OMX_PTS       (常用于查看输入输出PTS值)

    3:      OMX_FATAL & OMX_ERR
    7:      OMX_FATAL & OMX_ERR & OMX_WARN
    11:     OMX_FATAL & OMX_ERR & OMX_INFO
    19:     OMX_FATAL & OMX_ERR & OMX_TRACE
    96:     OMX_INBUF & OMX_OUTBUF
    35:     OMX_FATAL & OMX_ERR & OMX_INBUF
    67:     OMX_FATAL & OMX_ERR & OMX_OUTBUF
    99:     OMX_FATAL & OMX_ERR & OMX_INBUF & OMX_OUTBUF
*/
typedef enum {
    TASK_STATE_INVALID = 0,
    TASK_STATE_SLEEPING,
    TASK_STATE_ONCALL,
    TASK_STATE_RUNNING,
} e_task_state;

typedef struct {
    e_task_state task_state;
    osal_wait task_wait;
    struct task_struct *task_thread;
} OMXVDEC_TASK;

typedef enum {
    ALLOC_INVALID = 0,
    ALLOC_BY_MMZ,
    ALLOC_BY_MMZ_UNMAP,
    ALLOC_BY_PRE,
    ALLOC_BY_SEC,
} e_mem_alloc;

typedef enum {
    SAVE_FLIE_RAW = 0,
    SAVE_FLIE_YUV,
    SAVE_FLIE_IMG,
} e_save_flie;

typedef struct {
    hi_u32 open_count;
    hi_u32 total_chan_num;
    OMXVDEC_TASK task;
    unsigned long chan_bitmap;
    struct list_head chan_list;
    osal_spinlock lock;
    osal_spinlock channel_lock;
    osal_spinlock stream_lock;
    osal_semaphore dev_sema;
    struct cdev cdev;
    struct device *device;
} OMXVDEC_ENTRY;

typedef struct {
    hi_void *p_decoder_func;   /* decoder external functions */
    hi_void *p_processor_func; /* processor external functions */
    hi_void *p_vdec_func;
} OMXVDEC_FUNC;

typedef struct {
    hi_u32 usrtag_start_time;
    hi_u32 interval;
    hi_u32 current_tag;
    hi_u32 time_record[OMX_LOWDELAY_REC_NODE_NUM + 1][OMX_LOWDELAY_REC_ITEM];
    hi_u32 time_cost_sum;
    hi_u32 average_time_cost;
    hi_u32 rec_index;
} OMXVDEC_LOWDELAY_PROC_RECORD;

hi_void omxvdec_release_mem(hi_void *p_mmz_buf, e_mem_alloc e_mem_alloc, omx_mem_free_type free_type);

hi_u32 omx_get_time_in_ms(hi_void);

#endif

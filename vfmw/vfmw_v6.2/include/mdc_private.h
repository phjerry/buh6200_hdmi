#ifndef __MCU_PRIVATE_H__
#define __MCU_PRIVATE_H__

#include "vfmw_osal.h"
#include "comm_share.h"
#ifdef VFMW_PROC_SUPPORT
#include "vfmw_proc.h"
#endif

#define MDC_READY_RETRY_TIMES 5

#define WR_MDC_REG(data, reg) *(volatile hi_u32 *)(reg) = data
#define RD_MDC_REG(reg)       *(volatile hi_u32 *)(reg)

#define MAX_RAW_NUM   (1024)
#define MAX_EVENT_NUM (512)
#define MAX_PARA_SIZE (1024)

#define MDC_ES_BUFFER_MAX 12

#define MDC_DBG_CNT 5

/*  MUST be multiple of 2  */
#define MAX_QUEUE_IMAGE_NUM (64)

#define BOTTOM_FIELD_MASK   (2)
#define TOP_FIELD_MASK      (1)
#define FRAME_MASK          (3)
#define INVALID_SHARE_INDEX (-1)

#define MDC_THREAD_DESTROY_TRY_TIME 6000
#define MDC_MEM_ALIGN_SIZE          0x1000

#define MDC_THREAD_WAIT_TIME 10

#define MDC_LOG_MAGIC1 (0x1234abcd)
#define MDC_LOG_MAGIC2 (0xa5a55a5a)

/* MDC MAX DATA LEN */
#define MDC_DATA_MAX_LEN 128

/* MDC MSG OPTION */
#define MDC_DATA_TYPE_MSG 0
#define MDC_DATA_TYPE_ACK 1

typedef struct {
    hi_u32 magic1;
    hi_u32 write_head_s;
    hi_u32 write_tail_s;
    hi_u32 read_c;
    hi_u32 end_pos;
    hi_u32 magic2;
} mdc_log_option;

typedef enum {
    MDC_INDEX_NOR = 0,
    MDC_INDEX_SEC,
    MDC_INDEX_MAX,
} mdc_dec_index;

/* MDC THREAD */
typedef enum {
    MDC_THREAD_STA_EXIT,
    MDC_THREAD_STA_START,
    MDC_THREAD_STA_STOP,
    MDC_THREAD_STA_BUTT,
} mdc_thread_state;

typedef enum {
    MDC_THREAD_WORKING,
    MDC_THREAD_SLEEPING,
} mdc_thread_work_pos;

typedef struct {
    hi_void *handle;
    hi_u32 run_cnt;
    OS_EVENT event;
    mdc_thread_state state;
    mdc_thread_work_pos work_pos;
} mdc_thread;

/* MDC COMMAND ID */
typedef enum {
    MDC_CID_FRM_MEM_ALLOC,
    MDC_CID_FRM_MEM_FREE
} MDC_SID_MEM_CID; /* MDC_SID_MEM */

typedef enum {
    MDC_CID_PROC_READ,
    MDC_CID_PROC_WRITE,
    MDC_CID_FLUSH_LOG,
} MDC_SID_DFX_CID; /* MDC_SID_DFX */

typedef enum {
    MDC_CID_TEST_SAMPLE,
    MDC_CID_TEST_FEATURE,
    MDC_CID_TEST_OSAL,
    MDC_CID_TEST_COMM,
    MDC_CID_TEST_PERF
} MDC_SID_TEST_CID; /* MDC_SID_TEST */

/* MDC SERVICE ID */
typedef enum {
    MDC_SID_INVALID,
    MDC_SID_INIT,
    MDC_SID_DEINIT,
    MDC_SID_RESUME,
    MDC_SID_SUSPEND,
    MDC_SID_CONTROL,
    MDC_SID_DFX,
    MDC_SID_MEM,
    MDC_SID_ENTER_WFI,
    MDC_SID_SHAKE_HAND,
    MDC_SID_TEST,
    MDC_SID_BUFF
} MDC_SERVICE_ID;

typedef struct {
    hi_u32 unid          : 16;
    hi_u32 reserved      : 12;
    hi_u32 args_in_msg   :  1;
    hi_u32 args_in_shr   :  1;
    hi_u32 need_ack      :  1;
    hi_u32 data_type     :  1;
} mdc_msg_option;

/* MDC COMMUNICATION DATA */
typedef struct {
    hi_s32 chan_id;
    hi_u32 service_id;
    hi_u32 command_id;
    hi_void *args;
    hi_u32 length;
    hi_void *valid_comm_mem;
    hi_u32 comm_mem_length;
} __attribute__ ((packed)) comm_cmd_param;

typedef hi_s32 (*mdc_comm_intf)(comm_cmd_param *param);
typedef hi_s32 (*notify_cb)(hi_void);

typedef struct {
    hi_u8 tran_id;
    hi_u8 magic;
    hi_s16 chan_id;
    mdc_msg_option ops;
    hi_s32 ret_val;
    hi_u16 service_id;
    hi_u16 command_id;
    hi_s32 args_length;
} __attribute__ ((packed)) mdc_msg_base;

#define MDC_MSG_BASE_LEN                 (sizeof(mdc_msg_base))
#define MDC_MSG_ARGS_SIZE_MAX            (MDC_DATA_MAX_LEN - MDC_MSG_BASE_LEN)

#define MDC_COMM_SHARE_MEM_LENGTH        (4 * 1024)
#define MDC_CREATE_ARGS_LENGTH           (128)
#define MDC_COMM_TMP_SHR_MEM_LENGTH      (MDC_CREATE_ARGS_LENGTH * VFMW_CHAN_NUM)
#define MDC_COMM_CHAN_SHARE_MEM_LENGTH   (4 * 1024)

typedef struct {
    hi_u8           mdc_args_buf[MDC_MSG_ARGS_SIZE_MAX];
} __attribute__ ((packed)) mdc_msg_args;

typedef struct {
    mdc_msg_base msg_base;
    mdc_msg_args msg_args;
} __attribute__ ((packed)) mdc_msg;

/* MDC APPICATION DATA */
typedef struct {
    hi_u32 mdc_index;
} mdc_shake_param;

typedef struct {
    UADDR  phyaddr;
    hi_s32 length;
    vfmw_mem_mode mode;
    hi_u32 ree_pt_addr;
    hi_u32 chan_max_size;
    hi_u32 hal_max_size;
    hi_u64 arm_time_us;
    hi_u64 mdc_time_tick;
} mdc_init_param;

typedef struct {
    UADDR  phyaddr;
    hi_s32 length;
    vfmw_mem_mode mode;
    hi_s32 is_sec;
} mdc_create_param;

typedef enum {
    MDC_MEM_TYPE_DEC = 0,
    MDC_MEM_TYPE_DISP,
    MDC_MEM_TYPE_PMV,
    MDC_MEM_TYPE_NOR,
    MDC_MEM_TYPE_SEC,
    MDC_MEM_TYPE_MAX,
} mdc_mem_type;

typedef struct {
    hi_u8  sec_flag;
    hi_u8  is_cache;
    hi_u8  is_map;
    hi_u8  reserve_2;
    hi_u32 size;
    hi_s64 unique_id;
    UADDR  phyaddr;
    hi_u64 vir_addr;
    hi_u64 dma_buf;
    hi_handle vdec_handle;
    hi_handle ssm_handle;
} mdc_mem_param;

typedef struct {
    hi_s32 module;
} mdc_proc_read_param;

typedef struct {
    hi_s32 module;
    hi_s32 count;
    hi_s8  cmd_str[CMD_STR_LEN];
} mdc_proc_write_param;

/* MDC DBG DATA */
typedef enum {
    MDC_TIME_TYPE_GET  = 0,
    MDC_TIME_TYPE_GET_ACK  = 1,
    MDC_TIME_TYPE_SEND = 2,
    MDC_TIME_TYPE_SEND_NOACK = 3,
    MDC_TIME_TYPE_MAX
} mdc_time_type;

#endif

#ifndef __HI_DRV_IR_H__
#define __HI_DRV_IR_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_unf_ir.h"

#define HI_FATAL_IR(fmt ...)    HI_FATAL_PRINT(HI_ID_IR, fmt)
#define HI_ERR_IR(fmt ...)      HI_ERR_PRINT(HI_ID_IR, fmt)
#define HI_WARN_IR(fmt ...)     HI_WARN_PRINT(HI_ID_IR, fmt)
#define HI_INFO_IR(fmt ...)     HI_INFO_PRINT(HI_ID_IR, fmt)
#define HI_DEBUG_IR(fmt ...)    HI_DBG_PRINT(HI_ID_IR, fmt)

/* How many NEC ir code we will support at the same time.
 * NOTES: NEC ir code means protocol is nec,
 * but infrared code is not the same, including header and burst
 * are not simalar or wanna_bits is  not the same.
 */
#define MAX_NEC_INFR_NR 20
#define MAX_RC6_INFR_NR 3
#define MAX_RC5_INFR_NR 3
#define MAX_SONY_INFR_NR 3
#define MAX_TC9012_INFR_NR 3
#define MAX_CREDIT_INFR_NR 3

/* some buit-in protocol names defination */
#define NEC_FULL_2HEADERS "nec full 2headers"
#define NEC_SIMPLE_2HEADERS_DISTINGUISH_REPEAT "nec simple 2headers changshu"
#define NEC_SIMPLE_UPD6121G "nec simple uPD6121G like"
#define NEC_SIMPLE_LC7461MCL13 "nec simple LC7461MCL13 like"
#define NEC_FULL_DEZHOU "nec full dezhou like"
#define NEC_FULL_HUIZHOU "nec full huizhou like"
#define NEC_SIMPLE_2HEADERS_SAME_REPEAT "nec simple 2headers gd"
#define CREDIT_CARD "credit"
#define SONY_D7C8 "sony d7c8"
#define SONY_D7C5 "sony d7c5"
#define TC9012 "tc9012"
#define RC6_32BIT "rc6 32bit data"
#define RC6_20BIT "rc6 20bit data"
#define RC6_16BIT "rc6 16bit data"
#define RC5_14BIT "rc5 14bit data"
#define RSTEP_18BIT "rstep"
#define EXTENDED_RC5_14BIT "extended rc5 14bit"
#define XMP_1_16BIT "xmp-1"

/*
 * rc6's burst may at offset of x of the expect position.
 * for example: the expect burst is at 32 from the frame start.
 * but, the actural position may be appear at one of
 * [.. 29, 30, 31, 32, 33, 34, 35 ..].
 * Why would this happe?? If you know it, please tell me why :(.
 */
#define RSTEP_BURST_OFFSET 5
#define RC6_BURST_OFFSET 4

/* the same as RC6_BURST_OFFSET */
#define RC5_BURST_OFFSET 4

/* irq define */
#define IR_IRQ_NO   83
#define IR_IO_BASE  0x00850000

/* some default parameter defines */
#define DFT_SYMBOL_WIDTH 0x3e80
#define DFT_SYMBOL_FMT 0
#define DFT_INT_LEVEL 1
#define DFT_FREQ 1
#define DFT_MV300_FREQ 28

#define MAX_DATA_BITS (sizeof(hi_u64) * 8 * 2)
#define PROTOCOL_NAME_SZ 32
#define PROTOCOL_NAME_MAX_SZ 128

#define IR_DEVICE_NAME_SZ 64

/*
 * the max bits of an infrared code we have meet is 48,
 * set this macro to 100 may hold 2 key press event.
 */
#define MAX_SYMBOL_NUM 500

#define SYMBOL_PARSE_TIMEOUT 200

#define IR_ADDR_SIZE    0x1000

typedef enum {
    KEY_FETCH_PARSED,
    KEY_FETCH_RAW
} key_fetch_mode;

typedef enum {
    IR_PROT_COMSIMPLE,
    IR_PROT_TC9012,
    IR_PROT_COMFULL,
    IR_PROT_SONY,

    IR_PROT_RC5,
    IR_PROT_RC5X,
    IR_PROT_RC6,
    IR_PORT_RSTEP,
    IR_PROT_XMP1,
    IR_PROT_CREDIT,
    /* add your new protocol here. */
    IR_PROT_USER_DEFINED1 = 100,
    IR_PROT_USER_DEFINED2,
    IR_PROT_USER_DEFINED3,

    IR_PROT_MAX
} ir_protocol_idx;

typedef enum {
    /* chip only report keys analyzed by itself. */
    IR_MODE_KEY,

    /* chip only report symbols */
    IR_MODE_SYMBOL,
    IR_MODE_MAX
} ir_mode;

typedef enum {
    KEY_STAT_DOWN,
    KEY_STAT_HOLD,
    KEY_STAT_UP,
    KEY_STAT_MAX
} key_status;

/* refer to register IR_CFG. */
typedef struct {
    hi_u32 ir_max_symbol;

    /*
     * -store infrared code type while key mode.
     * -store symbol 'byte-order'
     */
    hi_u32 ir_format;
    hi_u32 ir_max_bits;
    ir_mode work_mode;
    hi_u32 ir_freq;
}ir_chip_param;

typedef struct {
    /* upper 16bit data under key mode */
    hi_u64 upper;

    /*
     * lower 32bit data under key mode
     * or symbol value under symbol mode
     */
    hi_u64 lower;

    hi_char protocol_name[PROTOCOL_NAME_SZ];

    /* indentify key status. */
    key_status key_stat;
} key_attr;

/* for ir_std */
typedef struct {
    hi_u32               ir_key_data_h;
    hi_u32               ir_key_data_l;
    hi_unf_ir_protocol   ir_protocol;
    hi_unf_key_status    ir_key_state;
} ir_key;

#ifdef __KERNEL__

typedef struct {
    key_attr buf[MAX_SYMBOL_NUM];
    hi_s32             reader;
    hi_s32             writer;
} ir_buffer;

typedef irqreturn_t (*isr_proc)(hi_s32 irq, hi_void *dev_id);
typedef hi_void (*data_proc)(hi_ulong data);
typedef struct {
    isr_proc  isr_handle;
    data_proc data_handle;
} ir_callbacks;

typedef struct {
    /* register base */
    hi_void *__iomem base;
    /* module irq number. */
    hi_s32      irq;
    hi_s32      major;
    hi_char     dev_name[IR_DEVICE_NAME_SZ];
    atomic_t refcnt;
    ir_chip_param chip_param;
    ir_buffer *symbol_buf;
    /* key buffer to store keys driver analyzed. */
    ir_buffer *key_buf;
    /* key down-state hold timeout time, ms */
    hi_u32 key_hold_timeout_time;
    struct timer_list symbol_timer;
    key_fetch_mode key_fetch;
    /* wait for read call while no symbol or key received. */
    wait_queue_head_t read_wait;
    /* how many ms read call can wait. */
    u32 key_read_timeout;
    /* only allow one user to fetch keys or symbols. */
    struct semaphore sem;
    ir_callbacks callbacks;
    struct tasklet_struct ir_bh;
    /* enable keyup event? */
    hi_s32 key_up_event;
    /* enable repeat key event? */
    hi_s32 key_repeat_event;
    /* repeat key receive interval. */
    hi_s32 key_repeat_interval;
    /* count of tring to get key */
    hi_u32 key_debug_trycount;
    /* count of succeed to get key */
    hi_u32 key_debug_succeed_count;

}ir_priv;

extern hi_u32      g_time_out_flag;
extern ir_priv     g_ir_local;
extern spinlock_t  irlock;
extern hi_u32      g_redundant_frame_flag;
extern key_attr    *g_last_key_state;

#endif

#endif /* __HI_DRV_IR_H__ */

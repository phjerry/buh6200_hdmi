/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio basic definition.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_TSIO_DEFINE_H__
#define __DRV_TSIO_DEFINE_H__

#include "hi_osal.h"
#include "hi_type.h"
#include "hi_drv_module.h"
#include "hi_debug.h"
#include "hi_drv_tsio.h"
#include "hi_errno.h"

#include "drv_tsio_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TSIO_HANDLE_MAGIC    (0x5)
#define ID_2_HANDLE(id)      ((HI_ID_TSIO << 28) | (TSIO_HANDLE_MAGIC << 24) | (id & 0x00ffffff))
#define HANDLE_2_ID(handle)  ((handle) & 0x00ffffff)

static inline hi_s32 check_handle(hi_handle handle)
{
    hi_s32 ret = HI_SUCCESS;
    if (((handle >> 28) & 0x0000000f) != HI_ID_TSIO || /* Shift 28 bits to determine if the mod id are correct */
        ((handle >> 24) & 0x0000000f) != TSIO_HANDLE_MAGIC) { /* Shift 24 bits to determine if the handle are correct */
        ret = HI_ERR_TSIO_INVALID_PARA;
    }
    return ret;
}

#define CHECK_HANDLE(handle) check_handle(handle)

#define INVALID_PKT_SIZE         1

#define TS_SYNC_BYTE             0x47
#define SE_LOW_NUM                 32

#define TSIO_FATAL_ERROR(condition) \
    do { \
        if (condition) { \
            HI_FATAL_TSIO("TSIO FATAL ERROR: %s\n", #condition); \
        } \
    } while (0)

struct tsio_session;
struct tsio_slot;
struct tsio_r_base;
struct tsio_r_tsi_port;
struct tsio_r_ram_port;
struct tsio_r_pid_channel;
struct tsio_r_raw_channel;
struct tsio_r_sp_channel;
struct tsio_r_ivr_channel;
struct tsio_r_se;
struct tsio_r_cc;
struct tsio_mgmt;

/*
 * general definition.
 */
struct obj_node_helper {
    hi_void             *key;
    struct list_head    node;
};

typedef union {
    struct {
        unsigned int    playnums                : 8   ; /* [7..0]  */
        unsigned int    sid                     : 6   ; /* [13..8]  */
        unsigned int    reserved_0              : 2   ; /* [15 14]  */
        unsigned int    tsid                    : 8   ; /* [23..16]  */
        unsigned int    tstype                  : 4   ; /* [27..24]  */
        unsigned int    flush                   : 1   ; /* [28]  */
        unsigned int    desep                   : 1   ; /* [29]  */
        unsigned int    reserved_2              : 2   ; /* [31 30]  */
    } bits;

    unsigned int    u32;
} U_DMA_DSC_WORD_0;

/*
 * handles associated with the session.
 */
enum  tsio_session_state {
    TSIO_SESSION_INACTIVED = 0x0,  /* support remove slot only. */
    TSIO_SESSION_ACTIVED,          /* support add or remove slot. */
};

struct tsio_session_ops {
    hi_s32(*add_slot)(struct tsio_session *session, struct tsio_slot *slot);
    hi_s32(*del_slot)(struct tsio_session *session, struct tsio_slot *slot);
    hi_void(*release)(struct tsio_session *session);  /* release all R obj from session. */
    hi_s32(*suspend)(struct tsio_session *session);
    hi_s32(*resume)(struct tsio_session *session);
};

struct tsio_session {
    osal_atomic                  ref_count;
    osal_mutex                   lock;
    enum  tsio_session_state     state;
    struct list_head             head;

    struct tsio_session_ops      *ops;
};

/*
 * slot definition.
 */
struct tsio_slot {
    osal_mutex            lock;
    struct tsio_session     *session;
    struct tsio_r_base      *obj;

    hi_handle handle;
    hi_s32(*release)(hi_handle handle);  /* private : maybe changed for different handle object. */
};

struct tsio_slot_table {
    osal_mutex       lock;
    hi_u32           slot_cnt;
    DECLARE_BITMAP(slot_bitmap, TSIO_MAX_SLOT_CNT);
    struct tsio_slot table[TSIO_MAX_SLOT_CNT];
};

/*
 * Resource base definition.
 */
struct tsio_r_base_ops {
    hi_s32(*get)(struct tsio_r_base *obj);
    hi_void(*put)(struct tsio_r_base *obj);
};

struct tsio_r_base {
    osal_atomic             ref_count;
    struct tsio_r_base_ops *ops;
    hi_s32(*release)(struct tsio_r_base *obj);

    hi_u32                 id;
    struct tsio_mgmt       *mgmt;
};

/*
 * TSI port definition.
 */
#define TO_TSIPORT_HW_ID(id)      (id)

#define IS_TSIPORT(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct tsio_r_base *base = (struct tsio_r_base*)obj; \
    struct tsio_r_tsi_port *tsi_port = container_of(base, struct tsio_r_tsi_port, base); \
    TSIO_FATAL_ERROR(get_tsio_r_base_ops() != base->ops); \
    if (&g_tsio_tsi_port_ops == tsi_port->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct tsio_r_tsi_port_ops {
    hi_s32(*open)(struct tsio_r_tsi_port *tsi_port);
    hi_s32(*set_attrs)(struct tsio_r_tsi_port *tsi_port, const tsio_tsi_port_attrs *attrs);
    hi_s32(*get_attrs)(struct tsio_r_tsi_port *tsi_port, tsio_tsi_port_attrs *attrs);
    hi_s32(*get_status)(struct tsio_r_tsi_port *tsi_port, tsio_tsi_port_status *status);
    hi_s32(*close)(struct tsio_r_tsi_port *tsi_port);

    hi_s32(*suspend)(struct tsio_r_tsi_port *tsi_port);
    hi_s32(*resume)(struct tsio_r_tsi_port *tsi_port);
};

struct tsio_r_tsi_port {
    struct tsio_r_base         base;   /* !!! it must be first entry. */
    struct tsio_r_tsi_port_ops *ops;
    osal_mutex                 lock;
    struct list_head           node;

    hi_bool                    staled;

    dmx_port                source_port;

    hi_u32                     dmx_if_port_cnt;
    hi_u32                     dmx_tsi_port_cnt;
};

/*
 * RAM port definition.
 */
#define MIN_RAM_BUFFER_SIZE       0x100000
#define MAX_RAM_BUFFER_SIZE       0x400000   /* hi_u16  head_blk_idx maybe overflow if you increase this settings. */

/*
 * dont change this arbitrarily, refer to mpi_tsio.c::CalcPace.
 */
#define MIN_RAM_DATE_RATE         1400 /* kbps, hw limited. */
#define MAX_RAM_DATA_RATE         400000 /* kbps, hw limited. */

#define RAM_DSC_GUIDE_NUMBER      0xa5aa5a5

#define TSIO_DSC_GUIDE_NUM_LEN    28

#define TO_RAMPORT_HW_ID(id)      (id)

#define IS_RAMPORT(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct tsio_r_base *base = (struct tsio_r_base*)obj; \
    struct tsio_r_ram_port *ram_port = container_of(base, struct tsio_r_ram_port, base); \
    TSIO_FATAL_ERROR(get_tsio_r_base_ops() != base->ops); \
    if (&g_tsio_ram_port_ops == ram_port->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

/*
 * hw support max depth is 1024, single dsc size is 16 bytes.
 * only alloc one page(4K) for save memory. 4K/16Bytes = 256.
 */
#define DEFAULT_RAM_DSC_DEPTH     1024
#define DEFAULT_RAM_DSC_SIZE      16

#define BLK_FREE_FLAG             0x0
#define BLK_ALLOC_FLAG            0xab
#define BLK_ADD_INT_FLAG          0x17
#define BLK_ADD_RELCAIM_INT_FLAG  0x27
#define BLK_INVALID_HEAD_IDX      0xffff

#define BLK_DEFAULT_PKT_NUM       16

struct tsio_r_ram_port_ops {
    hi_s32(*open)(struct tsio_r_ram_port *ram_port);
    hi_s32(*get_attrs)(struct tsio_r_ram_port *ram_port, tsio_ram_port_attrs *attrs);
    hi_s32(*set_attrs)(struct tsio_r_ram_port *ram_port, const tsio_ram_port_attrs_ex *attrs);
    hi_s32(*get_status)(struct tsio_r_ram_port *ram_port, tsio_ram_port_status *status);
    hi_s32(*get_buffer)(struct tsio_r_ram_port *ram_port, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out);
    hi_s32(*push_buffer)(struct tsio_r_ram_port *ram_port, tsio_buffer *buf);
    hi_s32(*put_buffer)(struct tsio_r_ram_port *ram_port, tsio_buffer *buf);
    hi_s32(*close)(struct tsio_r_ram_port *ram_port);
    hi_s32(*reset_buffer)(struct tsio_r_ram_port *ram_port);

    hi_s32(*begin_bulk_seg)(struct tsio_r_ram_port *ram_port, tsio_key_type key, const hi_u8 *IV);
    hi_s32(*end_bulk_seg)(struct tsio_r_ram_port *ram_port);
    hi_s32(*add_bulk_target)(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se);
    hi_s32(*del_bulk_target)(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se);

    hi_s32(*add_ivr_target)(struct tsio_r_ram_port *ram_port, struct tsio_r_se *se);
    hi_s32(*del_ivr_target)(struct tsio_r_ram_port *ram_port, const struct tsio_r_se *se);

    hi_s32(*pre_mmap)(struct tsio_r_ram_port *ram_port, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr);
    hi_s32(*pst_mmap)(struct tsio_r_ram_port *ram_port, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct tsio_r_ram_port *ram_port);
    hi_s32(*resume)(struct tsio_r_ram_port *ram_port);
};

struct tsio_r_ram_port {
    struct tsio_r_base         base;   /* !!! it must be first entry. */
    struct tsio_r_ram_port_ops *ops;
    osal_mutex                 lock;
    struct list_head           node;

    hi_bool                    staled;

    tsio_port                  port;
    tsio_source_type           source_type;
    hi_u32                     max_data_rate; /* kbps */
    hi_u32                     pace;
    hi_u32                     pkt_size;

    hi_mem_handle_t            buf_handle;
    hi_void                    *buf_obj;
    hi_u8                      *buf_usr_addr;
    hi_u8                      *buf_ker_addr;
    hi_u64                     buf_phy_addr;
    hi_u32                     buf_size;

    hi_mem_handle_t            dsc_handle;
    hi_void                    *dsc_buf_obj;
    hi_u8                      *dsc_ker_addr;
    hi_u64                     dsc_phy_addr;
    hi_u32                     dsc_depth;

    osal_mutex                 blk_map_lock;
    hi_u32                     total_blk_nr;
    hi_u32                     free_blk_nr;
    hi_u32                     blk_size;
    hi_u32                     last_dsc_write_idx;
    hi_u32                     last_dsc_read_idx;
    hi_u32                     iter_blk_map_helper;
    struct {
        hi_u8   flag;       /* include alloc flag and debug flag ... */
        hi_u8   ref;        /* blk put reference */
        hi_u16  head_blk_idx; /* point to the buff's head blk idx which this blk belong to */
        hi_u32  req_len;     /* req_len less than or equal to alloc_blk_buf */
    }                          *blk_map;

    wait_queue_head_t          wait_queue;
    hi_u32                     wait_cond;

    struct work_struct         flow_control_worker;
    struct work_struct         dsc_end_worker;

    osal_mutex                 bulk_srv_lock;
    struct list_head           bulk_srv_list; /* used when TSIO_SOURCE_BULK. */
    hi_u32                     bulk_srv_cnt;

    struct tsio_r_se           *ivrestart_se;

    hi_u32                     all_pkt_cnt;

    hi_u32                     get_count;
    hi_u32                     get_valid_count;
    hi_u32                     push_count;
    hi_u32                     put_count;
};

/*
 * PID channel definition.
 */
#define TO_PIDCHN_HW_ID(id)      (id)

#define IS_PIDCHANNEL(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct tsio_r_base *base = (struct tsio_r_base*)obj; \
    struct tsio_r_pid_channel *pid_channel = container_of(base, struct tsio_r_pid_channel, base); \
    TSIO_FATAL_ERROR(get_tsio_r_base_ops() != base->ops); \
    if (&g_tsio_pid_channel_ops == pid_channel->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct tsio_r_pid_channel_ops {
    hi_s32(*open)(struct tsio_r_pid_channel *pid_channel);
    hi_s32(*get_status)(struct tsio_r_pid_channel *pid_channel, tsio_pid_channel_status *status);
    hi_s32(*close)(struct tsio_r_pid_channel *pid_channel);

    hi_s32(*connect)(struct tsio_r_pid_channel *pid_channel, struct tsio_r_se *se);
    hi_s32(*dis_connect)(struct tsio_r_pid_channel *pid_channel);

    hi_s32(*suspend)(const struct tsio_r_pid_channel *pid_channel);
    hi_s32(*resume)(const struct tsio_r_pid_channel *pid_channel);
};

struct tsio_r_pid_channel {
    struct tsio_r_base            base;   /* !!! it must be first entry. */
    struct tsio_r_pid_channel_ops *ops;
    osal_mutex                    lock;
    struct list_head              node;

    hi_u32                        pid;
    struct tsio_r_base            *port;     /* the source port associated with this channel. */
    struct tsio_r_se              *se;       /* the secure engine attached */
};

/*
 * RAW channel definition.
 */
#define IS_RAWCHANNEL(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct tsio_r_base *base = (struct tsio_r_base*)obj; \
    struct tsio_r_raw_channel *raw_channel = container_of(base, struct tsio_r_raw_channel, base); \
    TSIO_FATAL_ERROR(get_tsio_r_base_ops() != base->ops); \
    if (&g_tsio_raw_channel_ops == raw_channel->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct tsio_r_raw_channel_ops {
    hi_s32(*open)(struct tsio_r_raw_channel *raw_channel);
    hi_s32(*close)(struct tsio_r_raw_channel *raw_channel);

    hi_s32(*connect)(struct tsio_r_raw_channel *raw_channel, struct tsio_r_se *se);
    hi_s32(*dis_connect)(struct tsio_r_raw_channel *raw_channel);

    hi_s32(*suspend)(const struct tsio_r_raw_channel *raw_channel);
    hi_s32(*resume)(const struct tsio_r_raw_channel *raw_channel);
};

struct tsio_r_raw_channel {
    struct tsio_r_base            base;   /* !!! it must be first entry. */
    struct tsio_r_raw_channel_ops *ops;
    osal_mutex                    lock;
    struct list_head              node;

    struct tsio_r_base            *port;     /* the source port associated with this channel. */
    struct tsio_r_se              *se;       /* the secure engine attached */
};

/*
 * SP channel definition.
 */
#define IS_SPCHANNEL(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct tsio_r_base *base = (struct tsio_r_base*)obj; \
    struct tsio_r_sp_channel *sp_channel = container_of(base, struct tsio_r_sp_channel, base); \
    TSIO_FATAL_ERROR(get_tsio_r_base_ops() != base->ops); \
    if (&g_tsio_sp_channel_ops == sp_channel->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct tsio_r_sp_channel_ops {
    hi_s32(*open)(struct tsio_r_sp_channel *sp_channel);
    hi_s32(*close)(struct tsio_r_sp_channel *sp_channel);

    hi_s32(*connect)(struct tsio_r_sp_channel *sp_channel, struct tsio_r_se *se);
    hi_s32(*dis_connect)(struct tsio_r_sp_channel *sp_channel);

    hi_s32(*suspend)(const struct tsio_r_sp_channel *sp_channel);
    hi_s32(*resume)(const struct tsio_r_sp_channel *sp_channel);
};

struct tsio_r_sp_channel {
    struct tsio_r_base            base;   /* !!! it must be first entry. */
    struct tsio_r_sp_channel_ops  *ops;
    osal_mutex                    lock;
    struct list_head              node;

    struct tsio_r_base            *port;     /* the source port associated with this channel. */
    struct tsio_r_se              *se;       /* the secure engine attached */
};

/*
 * IVRestart channel definition.
 */
#define IS_IVRCHANNEL(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct tsio_r_base *base = (struct tsio_r_base*)obj; \
    struct tsio_r_ivr_channel *ivr_channel = container_of(base, struct tsio_r_ivr_channel, base); \
    TSIO_FATAL_ERROR(get_tsio_r_base_ops() != base->ops); \
    if (&g_tsio_ivr_channel_ops == ivr_channel->ops) \
        ret = HI_TRUE; \
    ret; \
})

struct tsio_r_ivr_channel_ops {
    hi_s32(*open)(struct tsio_r_ivr_channel *ivr_channel);
    hi_s32(*close)(struct tsio_r_ivr_channel *ivr_channel);

    hi_s32(*connect)(struct tsio_r_ivr_channel *ivr_channel, struct tsio_r_se *se);
    hi_s32(*dis_connect)(struct tsio_r_ivr_channel *ivr_channel);

    hi_s32(*suspend)(const struct tsio_r_ivr_channel *ivr_channel);
    hi_s32(*resume)(const struct tsio_r_ivr_channel *ivr_channel);
};

struct tsio_r_ivr_channel {
    struct tsio_r_base            base;   /* !!! it must be first entry. */
    struct tsio_r_ivr_channel_ops *ops;
    osal_mutex                    lock;
    struct list_head              node;

    struct tsio_r_base            *port;     /* the source port associated with this channel. */
    struct tsio_r_se              *se;       /* the secure engine attached */
};

/*
 * secure engine definition.
 */
#define TO_SE_HW_ID(id)         (id)

#define MIN_SE_BUFFER_SIZE       0x100000
#define MAX_SE_BUFFER_SIZE       0x10000000    /* hw restrict */

/*
 * hw not require a guaranteed pkt alignment, but suggest at lease 64 * pktsize.
 * so we choose (64 * MAX(188, 192, 212))
 */
#define DEFAULT_SE_OBUF_THRESH   (64 * 212)

struct tsio_r_se_ops {
    hi_s32(*pre_open)(struct tsio_r_se *se);
    hi_s32(*open)(struct tsio_r_se *se);
    hi_s32(*get_attrs)(struct tsio_r_se *se, tsio_secure_engine_attrs *attrs);
    hi_s32(*set_attrs)(const struct tsio_r_se *se, const tsio_secure_engine_attrs *attrs);
    hi_s32(*get_status)(struct tsio_r_se *se, tsio_secure_engine_status *status);
    hi_s32(*add_channel)(struct tsio_r_se *se, struct tsio_r_base *chn);
    hi_s32(*del_channel)(struct tsio_r_se *se, struct tsio_r_base *chn);
    hi_s32(*del_all_channels)(struct tsio_r_se *se);
    hi_s32(*acquire_buffer)(struct tsio_r_se *se, hi_u32 req_len, tsio_buffer *buf, hi_u32 time_out);
    hi_s32(*release_buffer)(struct tsio_r_se *se, const tsio_buffer *buf);
    hi_s32(*close)(struct tsio_r_se *se);
    hi_s32(*pst_close)(struct tsio_r_se *se);

    hi_s32(*pre_mmap)(struct tsio_r_se *se, hi_handle *buf_handle, hi_u32 *buf_size, hi_void **usr_addr);
    hi_s32(*pst_mmap)(struct tsio_r_se *se, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct tsio_r_se *se);
    hi_s32(*resume)(struct tsio_r_se *se);

    hi_void(*handle_buf_ovfl)(struct tsio_r_se *se);
    hi_s32(*handle_flow_control)(struct tsio_r_se *se, struct tsio_r_ram_port *ram_port);
};

struct tsio_r_se {
    struct tsio_r_base            base;   /* !!! it must be first entry. */
    struct tsio_r_se_ops          *ops;
    osal_mutex                    lock;
    struct list_head              node;

    hi_bool                       staled;
    tsio_secure_engine_state      stat;
    tsio_secure_engine_output_mod mod;

    struct list_head              chn_head;

    dmx_port                   dest_port;

    hi_mem_handle_t               buf_handle;
    hi_void                       *buf_obj;
    hi_u8                         *buf_usr_addr;
    hi_u8                         *buf_ker_addr;
    hi_u64                        buf_phy_addr;
    hi_u32                        buf_size;

    hi_u32                        pkt_size;
    hi_u32                        valid_buf_size; /* hw only use pkt align size of buf_size. */

    hi_u32                        cur_sw_read;
    struct work_struct            buf_ovfl_worker;

    hi_u32                        wait_cond;
    wait_queue_head_t             wait_queue;

    struct list_head              bulk_srv_node; /* used when data source is TSIO_SOURCE_BULK. */
    hi_u32                        wait_flush_cond;
    wait_queue_head_t             wait_flush_queue;

    hi_u32                        dma_end_int_cnt;
    hi_u32                        overflow_cnt;

    hi_u32                        acq_count;
    hi_u32                        acq_valid_count;
    hi_u32                        rel_count;
};

/*
 * CC header based definition.
 */
#define CC_ERR_MID_LAST           8
#define CC_ERR_LAST_LAST          4
#define CC_ERR_FIRST_FIRST        2
#define CC_ERR_LENGTH_OVFL        1
#define CC_OK                     0

struct tsio_r_cc_para {
    const hi_uchar *cmd;
    hi_u32 cmd_len;
    hi_uchar *resp;
    hi_u32 *resp_len;
};

struct tsio_r_cc_ops {
    hi_s32(*send)(struct tsio_r_cc *ccsingleton, const struct tsio_r_cc_para *cc_para, hi_u32 time_out);
};

struct tsio_r_cc {
    osal_mutex                    lock;
    struct tsio_r_cc_ops          *ops;
    struct tsio_mgmt              *mgmt;

    hi_u32                        ccerr_int_cnt;
    hi_u32                        ccerr_type;

    wait_queue_head_t             wait_queue;
    hi_u32                        wait_cond;
};

/*
 * global tsio resource managation.
 */
enum  tsio_mgmt_state {
    TSIO_MGMT_CLOSED = 0x0,
    TSIO_MGMT_OPENED,
};

struct tsio_mgmt_ops {
    hi_s32(*init)(struct tsio_mgmt *mgmt);
    hi_s32(*exit)(struct tsio_mgmt *mgmt);

    hi_s32(*get_config)(struct tsio_mgmt *mgmt, tsio_config *config);
    hi_s32(*set_config)(struct tsio_mgmt *mgmt, const tsio_config *config);

    hi_s32(*get_cap)(struct tsio_mgmt *mgmt, tsio_capability *cap);
    hi_s32(*get_out_of_sync_cnt)(struct tsio_mgmt *mgmt, hi_u32 *cnt);

    hi_s32(*suspend)(struct tsio_mgmt *mgmt);
    hi_s32(*resume)(struct tsio_mgmt *mgmt);

    hi_s32(*create_tsi_port)(struct tsio_mgmt *mgmt, tsio_port port, const tsio_tsi_port_attrs_ex *attrs,
                             struct tsio_r_tsi_port **tsi_port);
    hi_s32(*destroy_tsi_port)(struct tsio_mgmt *mgmt, struct tsio_r_tsi_port *tsi_port);

    hi_s32(*create_ram_port)(struct tsio_mgmt *mgmt, tsio_port port, const tsio_ram_port_attrs_ex *attrs,
                             struct tsio_r_ram_port **ram_port);
    hi_s32(*destroy_ram_port)(struct tsio_mgmt *mgmt, struct tsio_r_ram_port *ram_port);

    hi_s32(*create_pid_channel)(struct tsio_mgmt *mgmt, struct tsio_r_base *port, hi_u32 pid,
                                struct tsio_r_pid_channel **pid_channel);
    hi_s32(*destroy_pid_channel)(struct tsio_mgmt *mgmt, struct tsio_r_pid_channel *pid_channel);

    hi_s32(*create_raw_channel)(struct tsio_mgmt *mgmt, struct tsio_r_base *port,
        struct tsio_r_raw_channel **raw_channel);
    hi_s32(*destroy_raw_channel)(struct tsio_mgmt *mgmt, struct tsio_r_raw_channel *raw_channel);

    hi_s32(*create_sp_channel)(struct tsio_mgmt *mgmt, struct tsio_r_base *port, struct tsio_r_sp_channel **sp_channel);
    hi_s32(*destroy_sp_channel)(struct tsio_mgmt *mgmt, struct tsio_r_sp_channel *sp_channel);

    hi_s32(*create_ivr_channel)(struct tsio_mgmt *mgmt, struct tsio_r_base *port,
        struct tsio_r_ivr_channel **ivr_channel);
    hi_s32(*destroy_ivr_channel)(struct tsio_mgmt *mgmt, struct tsio_r_ivr_channel *ivr_channel);

    hi_s32(*create_se)(struct tsio_mgmt *mgmt, tsio_sid sid, const tsio_secure_engine_attrs *attrs,
        struct tsio_r_se **se);
    hi_s32(*destroy_se)(struct tsio_mgmt *mgmt, struct tsio_r_se *se);

    /* debug helper */
    hi_void(*show_info)(struct tsio_mgmt *mgmt);
};

struct tsio_mgmt {
    osal_mutex            lock;
    enum  tsio_mgmt_state state;
    osal_atomic           ref_count;
    struct tsio_mgmt_ops  *ops;

    tsio_bw               band_width;
    tsio_sid              stuff_sid;
    hi_u32                sync_thres;

    hi_void               *io_base;
    hi_u32                cb_ttbr;

    hi_u32                tsi_port_cnt;
    osal_mutex            tsi_port_list_lock;
    struct list_head      tsi_port_head;
    DECLARE_BITMAP(tsi_port_bitmap, TSIO_TSIPORT_CNT);

    hi_u32                ram_port_cnt;
    osal_mutex            ram_port_list_lock;
    osal_spinlock         ram_port_list_lock2;
    struct list_head      ram_port_head;
    DECLARE_BITMAP(ram_port_bitmap, TSIO_RAMPORT_CNT);

    hi_u32                pid_channel_cnt;
    osal_mutex            pid_channel_list_lock;
    struct list_head      pid_channel_head;
    DECLARE_BITMAP(pid_channel_bitmap, TSIO_PID_CHANNEL_CNT);

    hi_u32                raw_channel_cnt;
    osal_mutex            raw_channel_list_lock;
    struct list_head      raw_channel_head;
    DECLARE_BITMAP(raw_channel_bitmap, TSIO_RAW_CHANNEL_CNT);

    hi_u32                sp_channel_cnt;
    osal_mutex            sp_channel_list_lock;
    struct list_head      sp_channel_head;
    DECLARE_BITMAP(sp_channel_bitmap, TSIO_SP_CHANNEL_CNT);

    hi_u32                ivr_channel_cnt;
    osal_mutex            ivr_channel_list_lock;
    struct list_head      ivr_channel_head;
    DECLARE_BITMAP(ivr_channel_bitmap, TSIO_IVR_CHANNEL_CNT);

    hi_u32                se_cnt;
    osal_mutex            se_list_lock;
    osal_spinlock         se_list_lock2;
    struct list_head      se_head;
    DECLARE_BITMAP(se_bitmap, TSIO_SE_CNT);

    struct tsio_r_cc      *ccsingleton;

    osal_task             *monitor;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_TSIO_DEFINE_H__

/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-06-05
 */
#ifndef __DRV_DEMUX_FUNC_H__
#define __DRV_DEMUX_FUNC_H__

#include "hi_osal.h"
#include "hi_drv_demux.h"
#include "drv_demux_scd.h"
#include "drv_demux_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* RAM port definition. */
#define MIN_RAM_BUFFER_SIZE       0x80000
#define MAX_RAM_BUFFER_SIZE       0x1000000   /* hi_u16  head_blk_idx maybe overflow if you increase this settings. */

#define MIN_RAM_DATE_RATE         976 /*  kpbs, 250mbps*8/(2047+1). */
#define MAX_RAM_DATA_RATE         500000 /* kpbs, 250mbps*8/(3+1). */
#define RAM_DSC_GUIDE_NUMBER      0xa   /* 4 bits in dsc word1 */

#define TO_RAMPORT_HW_ID(id)      (id)

#define IS_RAMPORT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_ram_port *ram_port = container_of(base, struct dmx_r_ram_port, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_ram_port_ops == ram_port->ops) \
        ret = HI_TRUE; \
    ret; \
})

/* hw support max depth is 1024, single dsc size is 8 bytes. */
#define DMX_TS_BUFFER_GAP         0x100
#define MIN_RAM_DSC_DEPTH         16
#define MAX_RAM_DSC_DEPTH         1024
#define DEFAULT_RAM_DSC_DEPTH     1024
#define DEFAULT_RAM_DSC_SIZE      8
#define RAM_DSC_ADDR_ALIGN        0x1000

#define DMX_RAM_MIN_LEN           188
#define DMX_RAM_MAX_LEN           255
#define DMX_RAM_AUTO_REGION       60
#define DMX_RAM_AUTO_STEP_1       0
#define DMX_RAM_AUTO_STEP_2       1
#define DMX_RAM_AUTO_STEP_4       2
#define DMX_RAM_AUTO_STEP_8       3

#define BLK_FREE_FLAG             0x0
#define BLK_ALLOC_FLAG            0xAB
#define BLK_ADD_INT_FLAG          0x17
#define BLK_ADD_RELCAIM_INT_FLAG  0x27
#define BLK_INVALID_HEAD_IDX      0xFFFF

struct dmx_ram_port_dsc {
    hi_u64 buf_phy_addr;
    hi_u32 buf_len;
    hi_bool desep;
    hi_bool flush_flag;
    hi_bool sync_data_flag;
    hi_u32 *cur_dsc_addr;
};

/* The second word of RAM description, and the first word is ipaddr */
typedef union {
    struct {
        unsigned int    iplength                : 20  ; /* [19.. 0] */
        unsigned int    desep                   : 1   ; /* [20] */
        unsigned int    flush                   : 1   ; /* [21] */
        unsigned int    syncdata                : 1   ; /* [22] */ /* sync flag before flush */
        unsigned int    reserved_0              : 1   ; /* [23]  */
        unsigned int    session                 : 4   ; /* [27..24] */
        unsigned int    check_data              : 4   ; /* [31..28] */
    } bits;

    unsigned int    u32;
} U_RAM_DSC_WORD_1;

struct dmx_r_ram_port_ops {
    hi_s32(*open)(struct dmx_r_ram_port *ram_port);
    hi_s32(*get_attrs)(struct dmx_r_ram_port *ram_port, dmx_port_attr *attrs);
    hi_s32(*set_attrs)(struct dmx_r_ram_port *ram_port, const dmx_port_attr *attrs);
    hi_s32(*get_status)(struct dmx_r_ram_port *ram_port, dmx_ram_port_status *status);
    hi_s32(*get_buffer)(struct dmx_r_ram_port *ram_port, hi_u32 req_len, dmx_ram_buffer *buf, hi_u32 time_out);
    hi_s32(*push_buffer)(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf);
    hi_s32(*put_buffer)(struct dmx_r_ram_port *ram_port, hi_u32 valid_datalen, hi_u32 start_pos);
    hi_s32(*release_buffer)(struct dmx_r_ram_port *ram_port, dmx_ram_buffer *buf);
    hi_s32(*flush_buffer)(struct dmx_r_ram_port *ram_port);
    hi_s32(*reset_buffer)(struct dmx_r_ram_port *ram_port);
    hi_s32(*close)(struct dmx_r_ram_port *ram_port);
    hi_s32(*get_buf_handle)(struct dmx_r_ram_port *ram_port,
        hi_mem_handle_t *buf_handle, hi_mem_handle_t *dsc_buf_handle, hi_mem_handle_t *flush_buf_handle);

    hi_s32(*pre_mmap)(struct dmx_r_ram_port *ram_port, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_ram_port *ram_port, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct dmx_r_ram_port *ram_port);
    hi_s32(*resume)(struct dmx_r_ram_port *ram_port);
};

struct dmx_r_ram_port {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_ram_port_ops  *ops;
    osal_mutex                 lock;
    struct list_head           node;
    hi_handle                  handle;
    hi_bool                    staled;

    dmx_secure_mode            secure_mode;
    hi_u32                     max_data_rate; /* kbps */
    hi_u32                     pkt_size;
    dmx_ts_crc_scb             crc_scb;

    hi_mem_handle_t            buf_handle;
    hi_void                    *buf_obj;
    hi_u8                      *buf_usr_addr;
    hi_u8                      *buf_ker_addr;
    hi_u64                     buf_phy_addr;
    hi_u32                     buf_size;
    hi_u32                     buf_used;

    hi_mem_handle_t            dsc_buf_handle;
    hi_void                    *dsc_buf_obj;
    hi_u8                      *dsc_buf_ker_addr;
    hi_u64                     dsc_buf_phy_addr;
    hi_u64                     dsc_sec_buf_phy_addr;
    hi_u32                     dsc_depth;
    hi_u32                     blk_size;
    hi_u32                     last_dsc_write_idx;
    hi_u32                     last_dsc_read_idx;
    osal_mutex                 blk_map_lock;
#ifdef HI_DMX_TSBUF_MULTI_THREAD_SUPPORT
    hi_u32                     total_blk_nr;
    hi_u32                     free_blk_nr;

    hi_u32                     iter_blk_map_helper;
    struct {
        hi_u8   flag;       /* include alloc flag and debug flag ... */
        hi_u8   ref;        /* blk put reference */
        hi_u16  head_blk_idx; /* point to the buff's head blk idx which this blk belong to */
        hi_u32  req_len;     /* req_len <= alloc_blk_buf */
    }                          *blk_map;
#else
    hi_u32                     read;
    hi_u32                     write;
    hi_u64                     req_addr;
    hi_u32                     req_len;
#endif
    wait_queue_head_t          wait_queue;
    hi_u32                     wait_cond;
    hi_u32                     wait_flush;

    struct work_struct         flow_control_worker;
    struct work_struct         dsc_end_worker;

    /* ramport flush buffer */
    hi_mem_handle_t            flush_buf_handle;
    hi_void                    *flush_buf_obj;
    hi_u8                      *flush_buf_ker_addr;
    hi_u64                     flush_buf_phy_addr;
    hi_u32                     flush_buf_size;

    hi_u32                     all_pkt_cnt;

    hi_u32                     get_count;
    hi_u32                     get_valid_count;
    hi_u32                     push_count;
    hi_u32                     put_count;
    hi_u32                     flush_count;

    hi_u32                     min_len;
    hi_u32                     max_len;
    hi_dmx_port_type           port_type;
    hi_u32                     sync_lock_th;
    hi_u32                     sync_lost_th;
    hi_u32                     sync_lost_cnt;
};

typedef enum {
    DMX_SYNC_MODE_SYNC,           /* Parallel sync mode */
    DMX_SYNC_MODE_NOSYNC_188,     /* Self-sync 188 mode */
    DMX_SYNC_MODE_NOSYNC_204,     /* Self-sync 204 mode */
    DMX_SYNC_MODE_NOSYNC_188_204, /* Self-sync 188/204 auto-identification mode */

    DMX_SYNC_MODE_MAX
} dmx_sync_mode;

typedef enum {
    DMX_BIT_SELECT_0,  /* select data[0] as bit source */
    DMX_BIT_SELECT_1,  /* select data[1] as bit source */
    DMX_BIT_SELECT_2,  /* select data[2] as bit source */
    DMX_BIT_SELECT_3,  /* select data[3] as bit source */
    DMX_BIT_SELECT_4,  /* select data[4] as bit source */
    DMX_BIT_SELECT_5,  /* select data[5] as bit source */
    DMX_BIT_SELECT_6,  /* select data[6] as bit source */
    DMX_BIT_SELECT_7,  /* select data[7] as bit source */

    DMX_BIT_SELECT_MAX
} dmx_bit_select;

typedef enum {
    DMX_SERIAL_1BIT = 1,  /* select 1bit */
    DMX_SERIAL_2BIT = 2,  /* select 2bit */
    DMX_SERIAL_BIT_MASK = 0x3,
    DMX_SERIAL_BIT_MAX
} dmx_serial_bit_mode;

hi_s32 drv_dmx_tsi_get_port_attrs(dmx_port port, dmx_port_attr *attrs);
hi_s32 drv_dmx_tsi_set_port_attrs(dmx_port port, const dmx_port_attr *attrs);
hi_s32 drv_dmx_if_get_port_attrs(dmx_port port, dmx_port_attr *attrs);
hi_s32 drv_dmx_if_set_port_attrs(dmx_port port, const dmx_port_attr *attrs);
hi_s32 drv_dmx_tso_get_port_attrs(dmx_port port, dmx_tso_port_attr *attrs);
hi_s32 drv_dmx_tso_set_port_attrs(dmx_port port, const dmx_tso_port_attr *attrs);
hi_s32 drv_dmx_tag_get_port_attrs(dmx_port port, dmx_tag_port_attr *attrs);
hi_s32 drv_dmx_tag_set_port_attrs(dmx_port port, const dmx_tag_port_attr *attrs);

hi_s32 dmx_ram_open_port(dmx_port port, const dmx_ram_port_attr *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_ram_get_port_attrs(hi_handle handle, dmx_port_attr *attrs);
hi_s32 dmx_ram_set_port_attrs(hi_handle handle, const dmx_port_attr *attrs);
hi_s32 dmx_ram_get_port_status(hi_handle handle, dmx_ram_port_status *status);
hi_s32 dmx_ram_get_buffer(hi_handle handle, hi_u32 req_len, dmx_ram_buffer *buf, hi_u32 time_out);
hi_s32 dmx_ram_push_buffer(hi_handle handle, dmx_ram_buffer *buf);
hi_s32 dmx_ram_put_buffer(hi_handle handle, hi_u32 valid_datalen, hi_u32 start_pos);
hi_s32 dmx_ram_release_buffer(hi_handle handle, dmx_ram_buffer *buf);
hi_s32 dmx_ram_flush_buffer(hi_handle handle);
hi_s32 dmx_ram_reset_buffer(hi_handle handle);
hi_s32 dmx_ram_close_port(hi_handle handle);
hi_s32 dmx_ram_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
hi_s32 dmx_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);
hi_s32 dmx_ram_get_port_id(hi_handle handle, dmx_port *port);
hi_s32 dmx_ram_get_port_handle(dmx_port port, hi_handle *handle);
hi_s32 dmx_port_get_packet_num(dmx_port port, dmx_port_packet_num *port_packet);
hi_s32 dmx_ram_get_bufhandle(hi_handle handle,
    hi_mem_handle_t *buf_handle, hi_mem_handle_t *dsc_buf_handle, hi_mem_handle_t *flush_buf_handle);

/* band begin */
extern struct dmx_r_band_ops g_dmx_band_ops;

#define DMX_R_BAND_GET(handle, rband) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base**)&rband); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_band_ops != rband->ops) { \
            dmx_r_put((struct dmx_r_base*)rband);\
            HI_ERR_DEMUX("handle is in active, but not a valid band handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

#define TO_BAND_HW_ID(id)      ((id) + DMX_BAND_0)

#define IS_BAND(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_band *rband = container_of(base, struct dmx_r_band, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_band_ops == rband->ops) \
        ret = HI_TRUE; \
    ret; \
})

struct dmx_r_band_ops {
    hi_s32(*open)(struct dmx_r_band *rband);
    hi_s32(*attach_port)(struct dmx_r_band *rband, dmx_port port);
    hi_s32(*detach_port)(struct dmx_r_band *rband);
    hi_s32(*get_attrs)(struct dmx_r_band *rband, dmx_band_attr *attrs);
    hi_s32(*set_attrs)(struct dmx_r_band *rband, const dmx_band_attr *attrs);
    hi_s32(*get_status)(struct dmx_r_band *rband, dmx_band_status *status);
    hi_s32(*attach_rec)(struct dmx_r_band *rband, struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*detach_rec)(struct dmx_r_band *rband, struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*close)(struct dmx_r_band *rband);

    hi_s32(*suspend)(struct dmx_r_band *rband);
    hi_s32(*resume)(struct dmx_r_band *rband);
};

struct dmx_r_band {
    struct dmx_r_base          base;  /* !!! it must be first entry. */
    struct dmx_r_band_ops      *ops;
    osal_mutex                 lock;
    struct list_head           node;  /* list in mgmt */

    hi_bool                    staled;
    osal_atomic                band_handle_ref_cnt;  /* for band handle */
    hi_handle                  band_handle;

    dmx_port                   port;
    enum dmx_port_type_e       port_type;

    hi_u32                     band_attr;
};

hi_s32 dmx_band_open(dmx_band port, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_band_ref_inc(dmx_band port, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_band_attach_port(hi_handle handle, dmx_port port);
hi_s32 dmx_band_detach_port(hi_handle handle);
hi_s32 dmx_band_get_attrs(hi_handle handle, dmx_band_attr *attrs);
hi_s32 dmx_band_set_attrs(hi_handle handle, const dmx_band_attr *attrs);
hi_s32 dmx_band_get_status(hi_handle handle, dmx_band_status *status);
hi_s32 dmx_band_close(hi_handle handle);
hi_s32 dmx_band_ref_dec(hi_handle handle);
hi_s32 dmx_band_tei_set(hi_handle handle, hi_bool tei);

/* pidchannel begin */
extern struct dmx_r_pid_ch_ops g_dmx_pid_ch_ops;

#define DMX_R_PID_CH_GET(handle, rpid_ch) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rpid_ch); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_pid_ch_ops != rpid_ch->ops) { \
            dmx_r_put((struct dmx_r_base*)rpid_ch);\
            HI_ERR_DEMUX("handle is in active, but not a valid pid_ch handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

#define IS_PIDCH(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_pid_ch *rpid_ch = container_of(base, struct dmx_r_pid_ch, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_pid_ch_ops == rpid_ch->ops) \
        ret = HI_TRUE; \
    ret; \
})

#define DMX_RAW_PID_CHANNEL_CNT        32

enum dmx_rawpidch_type {
    DMX_RAWPIDCH_MASTER_TYPE = 1,
    DMX_RAWPIDCH_SLAVE_TYPE = 2,
    DMX_RAWPIDCH_MAX_TYPE
};

/*  */
enum dmx_rawpidch_state {
    DMX_RAWPIDCH_MASTER_STATE = 1,
    DMX_RAWPIDCH_EXIST_SLAVE_STATE = 2,
    DMX_RAWPIDCH_CREATE_SLAVE_STATE = 3,
    DMX_RAWPIDCH_DESTROY_SLAVE_STATE = 4,
    DMX_RAWPIDCH_DESTROY_MASTER_STATE = 5,
    DMX_RAWPIDCH_MAX_STATE
};

struct dmx_r_raw_pid_ch;

struct dmx_r_raw_pid_ch_ops {
    hi_s32(*open)(struct dmx_r_raw_pid_ch *rraw_pid_ch);
    hi_s32(*close)(struct dmx_r_raw_pid_ch *rraw_pid_ch);

    hi_s32(*suspend)(struct dmx_r_raw_pid_ch *rraw_pid_ch);
    hi_s32(*resume)(struct dmx_r_raw_pid_ch *rraw_pid_ch);
};

struct dmx_r_raw_pid_ch {
    struct dmx_r_base           base;   /* !!! it must be first entry. */
    struct dmx_r_raw_pid_ch_ops *ops;
    osal_mutex                  lock;
    struct list_head            node0;  /* matain in mgmt->raw_pid_ch_total_head */
    struct list_head            node1;  /* matain in rpid_ch->raw_pid_ch_head */

    hi_bool                     staled;
    hi_bool                     force_unlive;
    struct dmx_r_pid_ch         *rpid_ch;
    struct dmx_r_play_fct       *rplay_fct;
    struct dmx_r_rec_fct        *rrec_fct;

    hi_u32                      raw_pid_chan_id;
    osal_atomic                 ref_play_cnt;
    osal_atomic                 ref_rec_cnt;

    enum dmx_rawpidch_type      raw_pid_ch_type;
};

struct dmx_r_pid_ch_ops {
    hi_s32(*open)(struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*get_status)(struct dmx_r_pid_ch *rpid_ch, dmx_pidch_status *status);
    hi_s32(*lock_out)(struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*un_lock_out)(struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*close)(struct dmx_r_pid_ch *rpid_ch);
    /* private interface */
    hi_s32(*attach)(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj);
    hi_s32(*detach)(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_base *obj);

    hi_s32(*suspend)(struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*resume)(struct dmx_r_pid_ch *rpid_ch);
};

struct dmx_r_pid_ch {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_pid_ch_ops    *ops;
    hi_bool                    staled;
    osal_mutex                 lock;

    struct list_head           node0;  /* list in mgmt */
    struct list_head           node2;  /* list in dmx_r_dsc_fct to record the pid channel dscfct attached */

    struct dmx_r_band          *rband;
    struct dmx_r_dsc_fct       *rdsc_fct;
    osal_atomic                pid_ch_handle_ref_cnt;  /* for pidch handle for same band and same pid */
    hi_handle                  pid_ch_handle;
    hi_u32                     pid;

    hi_bool                    pid_copy_en;
    hi_bool                    pid_copy_en_status;
    hi_u32                     pid_copy_chan_id;
    /* pid copy buffer address and size */
    hi_handle                  pid_copy_buf_handle;
    hi_void                    *pid_copy_buf_obj;
    hi_u8                      *pid_copy_buf_vir_addr;
    hi_u64                     pid_copy_buf_phy_addr;
    hi_u32                     pid_copy_buf_size;

    hi_u32                     master_raw_pid_ch_id; /* the master raw pid channel ID */
    enum dmx_rawpidch_state    tmp_raw_pid_ch_state;

    hi_u32                     pid_ch_attr;

    hi_u32                     raw_pid_ch_cnt;
    osal_mutex                 raw_pid_ch_list_lock;
    struct list_head           raw_pid_ch_head;    /* the first rampid channel node is master and othes are slave */
    DECLARE_BITMAP(raw_pid_ch_bitmap, DMX_RAW_PID_CHANNEL_CNT);

    hi_u32                     sync_lost_cnt;
    hi_u32                     cc_disc_err;
};

hi_s32 dmx_pid_ch_create(hi_handle band_handle, const hi_u32 pid, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_pid_ch_ref_inc(hi_handle band_handle, const hi_u32 pid, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_pid_ch_get_handle(hi_handle band_handle, hi_u32 pid, hi_handle *handle);
hi_s32 dmx_pid_ch_get_status(hi_handle handle, dmx_pidch_status *status);
hi_s32 dmx_pid_ch_lock_out(hi_handle handle);
hi_s32 dmx_pid_ch_un_lock_out(hi_handle handle);
hi_s32 dmx_pid_ch_destroy(hi_handle handle);
hi_s32 dmx_pid_ch_ref_dec(hi_handle handle);
hi_s32 dmx_pid_ch_get_free_cnt(hi_u32 *free_cnt);

/* buffer begin */
struct dmx_buf_status {
    hi_u8  *buf_usr_addr;
    hi_u8  *buf_ker_addr;
    hi_u32 buf_size;
    hi_u32 buf_used_size;
    hi_u32 hw_read;
    hi_u32 hw_write;
    hi_u32 hw_end_addr;
    hi_u64 buf_phy_addr;
    hi_u64 offset;
    hi_s64 buf_handle;
};

struct es_block_info_node {
    struct list_head     node;
    hi_u32               es_data_leng;
    hi_u32               pes_head_len;
    hi_u64               buf_phy_addr;
};

struct sec_block_info_node {
    struct list_head     node;
    hi_u32               sec_data_leng;
    hi_u8                *buf_usr_addr;
};

/* sub buf type */
typedef enum {
    DMX_BUF_TYPE_AVPES      = 0x1,  /* section buf type */
    DMX_BUF_TYPE_REC        = 0x2,  /* record buf type */
    DMX_BUF_TYPE_SCD        = 0x4,  /* scd buf type */
    DMX_BUF_TYPE_PES        = 0x8,  /* pes buf type */
    DMX_BUF_TYPE_SEC        = 0x10, /* section buf type */
    DMX_BUF_TYPE_TS         = 0x20, /* ts buf type */
    DMX_BUF_TYPE_MAX        = 0x3F
} dmx_buf_type;

struct dmx_r_buf_ops {
    hi_s32(*init)(struct dmx_r_buf *rbuf, dmx_buf_type buf_type, const dmx_tee_mem_info *tee_mem_info);
    hi_s32(*open)(struct dmx_r_buf *rbuf);
    hi_s32(*get_attrs)(struct dmx_r_buf *rbuf, struct dmx_buf_attrs *attrs);
    hi_s32(*get_status)(struct dmx_r_buf *rbuf, struct dmx_buf_status *status);
    hi_s32(*attach)(struct dmx_r_buf *rbuf, dmx_port port);
    hi_s32(*detach)(struct dmx_r_buf *rbuf);
    hi_s32(*pre_mmap)(struct dmx_r_buf *rbuf, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_buf *rbuf, hi_void *usr_addr);
    hi_s32(*acquire_buf)(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out,
        hi_u32 *acqed_num, dmx_buffer *dmx_buf);
    hi_s32(*release_buf)(struct dmx_r_buf *rbuf, hi_u32 rel_num, dmx_buffer *dmx_buf);
    hi_s32(*acquire_scd)(struct dmx_r_buf *rbuf, hi_u32 acq_num, hi_u32 time_out,
        hi_u32 *acqed_num, dmx_index_scd **scd_data);
    hi_s32(*release_scd)(struct dmx_r_buf *rbuf, hi_u32 rel_num, const dmx_index_scd *scd_data);
    hi_s32(*reset)(struct dmx_r_buf *rbuf, struct dmx_r_base *obj);
    hi_s32(*close)(struct dmx_r_buf *rbuf);
    hi_s32(*deinit)(struct dmx_r_buf *rbuf);

    hi_s32(*suspend)(struct dmx_r_buf *rbuf);
    hi_s32(*resume)(struct dmx_r_buf *rbuf);
};

struct dmx_disp_control {
    hi_u32          disp_time;
    hi_u32          disp_enable_flag;
    hi_u32          disp_frame_distance;
    hi_u32          distance_before_first_frame;
    hi_u32          gop_num;
};

struct dmx_r_buf {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *ops;
    struct dmx_r_buf_ops       inner_ops;  /* to protect the g_dmx_buf_ops not to be replace  */
    osal_mutex                 lock;
    struct list_head           node;  /* the list of mgmt */

    hi_bool                    staled;

    /* for ramport antipressure clear */
    dmx_port                   port;

    dmx_buf_type               buf_type;
    dmx_secure_mode            secure_mode;  /* TEE or REE */

    /* play_rec buffer */
    hi_mem_handle_t            buf_handle;
    hi_void                    *buf_obj;
    hi_u8                      *buf_usr_addr;
    hi_u8                      *buf_ker_addr;
    hi_u64                     buf_phy_addr;
    hi_u32                     buf_size;
    hi_u32                     real_buf_size;

    /* message queue  */
    wait_queue_head_t          wait_queue;
    hi_u32                     wait_cond;

    osal_mutex                 interrupt_lock;
    struct work_struct         buf_worker;
    struct work_struct         buf_ovfl_worker;
    hi_bool                    select_wait_queue_flag;

    /* all type interrupt th */
    hi_u32                     time_int_th;
    hi_u32                     seop_int_th;
    hi_u32                     ts_cnt_int_th;

    /* general */
    hi_u64                     last_addr;
    hi_u32                     sw_read;
    hi_u32                     last_read;
    hi_u32                     last_end;
    hi_bool                    rool_flag;
    hi_bool                    eos_flag;
    hi_bool                    ovfl_flag;
    hi_u32                     eos_cnt;
    hi_u32                     eos_record_idx;
    hi_u32                     ovfl_cnt;
    dmx_event_callback         callback;

    /* proc debug info */
    hi_u32                     acquire_cnt;
    hi_u32                     acquire_ok_cnt;
    hi_u32                     release_cnt;
    hi_u32                     pkt_size;
};

/* avpes buf */
struct dmx_buf_avpes {
    /* mq descript */
    hi_bool                    mq_use_flag;
    hi_mem_handle_t            mq_buf_handle;
    hi_void                    *mq_buf_obj;
    hi_u8                      *mq_buf_ker_addr;
    hi_u64                     mq_buf_phy_addr;
    hi_u32                     mq_dsc_depth;
    hi_u32                     last_mq_dsc_write_idx;
    hi_u32                     last_mq_dsc_read_idx;

    /* for avpes playfct */
    hi_u32                     last_pes_head_len;
    hi_s64                     last_pts;
    hi_bool                    first_pes_flag;
    struct list_head           es_block_info_head;
    struct dmx_disp_control    disp_contrl;
};

struct dmx_r_avpes_buf {
    struct dmx_r_buf           buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *avpes_ops;

    struct dmx_buf_avpes       buf_avpes;
};

/* record buf */
struct dmx_buf_rec {
};

struct dmx_r_rec_buf {
    struct dmx_r_buf           buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *rec_ops;

    struct dmx_buf_rec         buf_rec;
};

/* scd buf */
struct dmx_buf_scd {
};

struct dmx_r_scd_buf {
    struct dmx_r_buf           buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *scd_ops;

    struct dmx_buf_scd         buf_scd;
};

/* section buf */
struct dmx_buf_sec {
    struct list_head           sec_block_info_head;
};

struct dmx_r_sec_buf {
    struct dmx_r_buf           buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *sec_ops;

    struct dmx_buf_sec         buf_sec;
};

/* pes buf */
struct dmx_buf_pes {
};

struct dmx_r_pes_buf {
    struct dmx_r_buf           buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *pes_ops;

    struct dmx_buf_pes         buf_pes;
};

/* ts buf */
struct dmx_buf_ts {
};

struct dmx_r_ts_buf {
    struct dmx_r_buf           buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops       *ts_ops;

    struct dmx_buf_ts          buf_ts;
};

struct dmx_r_sub_buf {
    struct dmx_r_buf              buf_base;   /* !!! it must be first entry. */
    struct dmx_r_buf_ops          *sub_ops;

    union {
        struct dmx_buf_avpes     buf_avpes;     /* avpes buf type */
        struct dmx_buf_rec       buf_rec;       /* record buf type */
        struct dmx_buf_scd       buf_scd;       /* scd buf type */
        struct dmx_buf_pes       buf_pes;       /* pes buf type */
        struct dmx_buf_sec       buf_sec;       /* section buf type */
        struct dmx_buf_scd       buf_ts;        /* scd buf type */
    } buf_type;
};

/***********************buffer end******************************************/

/***********************pcr_fct begin******************************************/
extern struct dmx_r_pcr_fct_ops g_dmx_pcr_fct_ops;

#define DMX_R_PCR_FCT_GET(handle, rpcr_fct) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base**)&rpcr_fct); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_pcr_fct_ops != rpcr_fct->ops) { \
            dmx_r_put((struct dmx_r_base*)rpcr_fct);\
            HI_ERR_DEMUX("handle is in active, but not a valid pcr_fct handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

#define IS_PCR_FCT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_pcr_fct *rpcr_fct = container_of(base, struct dmx_r_pcr_fct, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_pcr_fct_ops == rpcr_fct->ops) \
        ret = HI_TRUE; \
    ret; \
})

struct dmx_r_pcr_fct_ops {
    hi_s32(*create)(struct dmx_r_pcr_fct *rpcr_fct);
    hi_s32(*attach)(struct dmx_r_pcr_fct *rpcr_fct, hi_handle sync_handle);
    hi_s32(*detach)(struct dmx_r_pcr_fct *rpcr_fct);
    hi_s32(*get_status)(struct dmx_r_pcr_fct *rpcr_fct, dmx_pcr_status *status);
    hi_s32(*get_pcr_scr)(struct dmx_r_pcr_fct *rpcr_fct, hi_u64 *pcr_ms, hi_u64 *scr_ms);
    hi_s32(*destroy)(struct dmx_r_pcr_fct *rpcr_fct);

    hi_s32(*suspend)(struct dmx_r_pcr_fct *rpcr_fct);
    hi_s32(*resume)(struct dmx_r_pcr_fct *rpcr_fct);
};

struct dmx_r_pcr_fct {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_pcr_fct_ops   *ops;
    osal_mutex                 lock;
    struct list_head           node;

    hi_bool                     staled;
    hi_handle                   handle;
    struct dmx_r_band          *rband;
    dmx_port                    port;
    hi_u32                      pid;
    hi_u64                      pcr_val;
    hi_u64                      scr_val;
    hi_handle                   sync_handle;
    dmx_event_callback          callback;
};

hi_s32 dmx_pcr_fct_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_pcr_fct_attach(hi_handle handle, hi_handle sync_handle);
hi_s32 dmx_pcr_fct_detach(hi_handle handle);
hi_s32 dmx_pcr_fct_get_status(hi_handle handle, dmx_pcr_status *status);
hi_s32 dmx_pcr_fct_get_pcr_scr(hi_handle handle, hi_u64 *pcr_ms, hi_u64 *scr_ms);
hi_s32 dmx_pcr_fct_destroy(hi_handle handle);
hi_s32 dmx_pcr_fct_set_callback(hi_handle handle, hi_handle user_handle, drv_dmx_callback cb);

/*
 * global dmx resource management.
 */
/* buffer status cls interface */
hi_void dmx_cls_buf_ap_status(struct dmx_mgmt *mgmt, struct dmx_r_buf *rbuf);

/* pid copy buffer status cls interface */
hi_void dmx_cls_pcbuf_ap_status(struct dmx_mgmt *mgmt, struct dmx_r_pid_ch *rpid_ch);

struct dmx_r_raw_pid_ch *_get_raw_pid_obj_for_play(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_play_fct *rplayfct);
struct dmx_r_raw_pid_ch *_get_raw_pid_obj_for_rec(struct dmx_r_pid_ch *rpid_ch, struct dmx_r_rec_fct *rrecfct);

struct dmx_mgmt *_get_dmx_mgmt(hi_void);
struct dmx_mgmt *get_dmx_mgmt(hi_void);

hi_s32  dmx_mgmt_init(hi_void);
hi_void dmx_mgmt_exit(hi_void);

hi_s32  dmx_mgmt_get_cap(dmx_capability *cap);
hi_s32  dmx_mgmt_get_free_flt_cnt(hi_u32 *free_cnt);

hi_s32  dmx_mgmt_suspend(hi_void);
hi_s32  dmx_mgmt_resume(hi_void);

hi_s32  dmx_mgmt_create_ram_port(dmx_port port, const dmx_ram_port_attr *attrs, struct dmx_r_ram_port **ram_port);
hi_s32  dmx_mgmt_destroy_ram_port(struct dmx_r_base *obj);

hi_s32  dmx_mgmt_create_band(dmx_band band, const dmx_band_attr *attrs, struct dmx_r_band **rband);
hi_s32  dmx_mgmt_destroy_band(struct dmx_r_base *obj);

/* the virtual pid channel */
hi_s32  dmx_mgmt_create_pid_ch(struct dmx_r_band *rband, const hi_u32 pid, struct dmx_r_pid_ch **rpid_ch);
hi_s32  dmx_mgmt_destroy_pid_ch(struct dmx_r_base *obj);

/* pcrfct management */
hi_s32  dmx_mgmt_create_pcr_fct(struct dmx_r_band *rband, hi_u32 pid, struct dmx_r_pcr_fct **rpcr_fct);
hi_s32  dmx_mgmt_destroy_pcr_fct(struct dmx_r_base *obj);

/* buffer management */
hi_s32  dmx_mgmt_create_buf(struct dmx_buf_attrs *attrs, struct dmx_r_buf **rbuf);
hi_s32  dmx_mgmt_destroy_buf(struct dmx_r_base *obj);


/* some general code for dmx   */
hi_s32  dmx_get_port_type_and_id(const dmx_port port, enum dmx_port_type_e *port_mode, hi_u32 *port_id);

#ifdef HI_DEMUX_PROC_SUPPORT
hi_void dmx_ram_port_save_ip_ts(struct dmx_r_ram_port *ram_port, hi_u8 *buf, hi_u32 len, hi_u32 port_id);
hi_s32 dmx_ram_port_start_save_ip_ts(hi_u32 port_id);
hi_void dmx_ram_port_stop_save_ip_ts(hi_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_FUNC_H__


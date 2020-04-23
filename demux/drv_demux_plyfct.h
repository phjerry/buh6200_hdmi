/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-06-05
 */
#ifndef __DRV_DEMUX_playFCT_H__
#define __DRV_DEMUX_playFCT_H__

#include "drv_demux_define.h"
#include "hi_drv_demux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* play_fct begin */
#define PLAY_MIN_BUFFER_SIZE       0x4000    /* 16K */
#define PLAY_MAX_BUFFER_SIZE       0x4000000 /* 64M */

extern struct dmx_r_play_fct_ops        g_dmx_play_fct_ops;
extern struct dmx_r_playfct_ts_ops      g_dmx_play_fct_ts_ops;
extern struct dmx_r_playfct_pes_sec_ops g_dmx_play_fct_sec_ops;
extern struct dmx_r_playfct_pes_sec_ops g_dmx_play_fct_pes_ops;
extern struct dmx_r_playfct_avpes_ops   g_dmx_play_fct_av_pes_ops;

/* playfct data type */
enum dmx_data_state {
    DMX_DATA_SECTION = 0x0,
    DMX_DATA_PES = 0x1,
    DMX_DATA_TS  = 0x2,
    DMX_DATA_INDEX = 0x3
};

struct dmx_r_play_fct_ops {
    hi_s32(*create)(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attrs);
    hi_s32(*get_pid_ch)(struct dmx_r_play_fct *rplay_fct, hi_handle *pid_handle);
    hi_s32(*open)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*get_attrs)(struct dmx_r_play_fct *rplay_fct, dmx_play_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attr);
    hi_s32(*get_status)(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status);
    hi_s32(*get_scrambled_flag)(struct dmx_r_play_fct *rplay_fct, hi_dmx_scrambled_flag *scramble_flag);
    hi_s32(*attach_pid_ch)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*detach_pid_ch)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*add_flt)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt);
    hi_s32(*del_flt)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt);
    hi_s32(*del_all_flt)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*update_flt)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs);
    hi_s32(*get_flt_attrs)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt, dmx_filter_attrs *attrs);
    hi_s32(*get_handle)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt, hi_handle *play_handle);
    hi_s32(*acquire_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_fct_buf);
    hi_s32(*release_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num, dmx_buffer *play_fct_buf);
    hi_s32(*reset_buf)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*start_idx)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*recv_idx)(struct dmx_r_play_fct *rplay_fct, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
        dmx_index_data *index);
    hi_s32(*stop_idx)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*close)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*destroy)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*get_buf_handle)(struct dmx_r_play_fct *rplay_fct, hi_handle *buf_handle, dmx_play_type *type);

    hi_s32(*pre_mmap)(struct dmx_r_play_fct *rplay_fct, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_play_fct *rplay_fct, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*resume)(struct dmx_r_play_fct *rplay_fct);
};

#define IS_PLAYFCT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_play_fct *rplay_fct = container_of(base, struct dmx_r_play_fct, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&rplay_fct->inner_ops == rplay_fct->ops) \
        ret = HI_TRUE; \
    ret; \
})

struct dmx_r_play_fct {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_play_fct_ops  *ops;
    struct dmx_r_play_fct_ops  inner_ops;  /* to protect the g_dmx_play_fct_ops not to be replace  */
    hi_bool                    staled;
    osal_mutex                 lock;
    struct list_head           node;  /* list in mgmt */

    struct dmx_r_buf           *rbuf;
    struct dmx_r_band          *rband;

    struct dmx_r_pid_ch        *rpid_ch;
    hi_handle                  play_fct_handle;

    dmx_play_type              play_fct_type;
    hi_bool                    is_opened;
    dmx_flt_crc_mode           crc_mode;
    hi_bool                    live_play;
    dmx_secure_mode            secure_mode;  /* TEE or REE */
    dmx_data_mode              data_mode;
    dmx_vcodec_type            vcodec_type;
    dmx_event_callback         callback;
#ifdef HI_DEMUX_PROC_SUPPORT
    struct file                *save_es_handle;
#endif
};

/* sub_playfct_ts_ops */
struct dmx_r_playfct_ts_ops {
    hi_s32(*open)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*get_attrs)(struct dmx_r_play_fct *rplay_fct, dmx_play_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attr);
    hi_s32(*get_status)(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status);
    hi_s32(*attach_pid_ch)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*detach_pid_ch)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*acquire_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_fct_buf);
    hi_s32(*release_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num, dmx_buffer *play_fct_buf);
    hi_s32(*close)(struct dmx_r_play_fct *rplay_fct);

    hi_s32(*pre_mmap)(struct dmx_r_play_fct *rplay_fct, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_play_fct *rplay_fct, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*resume)(struct dmx_r_play_fct *rplay_fct);
};

#define IS_PLAYFCT_TS(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_play_fct *play_fct_base = (struct dmx_r_play_fct*)obj; \
    struct dmx_r_playfct_ts *rplay_fct_ts = container_of(play_fct_base, struct dmx_r_playfct_ts, play_fct_base); \
    WARN_ON(get_dmx_rbase_ops() != play_fct_base->base.ops); \
    if (&g_dmx_play_fct_ts_ops == rplay_fct_ts->ops) \
        ret = HI_TRUE; \
    ret; \
})

/* sub_playfct_pes_sec_ops */
struct dmx_r_playfct_pes_sec_ops {
    hi_s32(*open)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*get_attrs)(struct dmx_r_play_fct *rplay_fct, dmx_play_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attr);
    hi_s32(*get_status)(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status);
    hi_s32(*attach_pid_ch)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*detach_pid_ch)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*add_flt)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt);
    hi_s32(*del_flt)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt);
    hi_s32(*del_all_flt)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*update_flt)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs);
    hi_s32(*get_flt_attrs)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt, dmx_filter_attrs *attrs);
    hi_s32(*get_handle)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_flt *rflt, hi_handle *play_handle);
    hi_s32(*acquire_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_fct_buf);
    hi_s32(*release_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num, dmx_buffer *play_fct_buf);
    hi_s32(*close)(struct dmx_r_play_fct *rplay_fct);

    hi_s32(*pre_mmap)(struct dmx_r_play_fct *rplay_fct, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_play_fct *rplay_fct, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*resume)(struct dmx_r_play_fct *rplay_fct);
};

#define IS_PLAYFCT_PES(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_play_fct *play_fct_base = (struct dmx_r_play_fct*)obj; \
    struct dmx_r_playfct_pes_sec *rplay_fct_pes = \
        container_of(play_fct_base, struct dmx_r_playfct_pes_sec, play_fct_base); \
    WARN_ON(get_dmx_rbase_ops() != play_fct_base->base.ops); \
    if (&g_dmx_play_fct_pes_ops == rplay_fct_pes->ops) \
        ret = HI_TRUE; \
    ret; \
})

#define IS_PLAYFCT_SEC(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_play_fct *play_fct_base = (struct dmx_r_play_fct*)obj; \
    struct dmx_r_playfct_pes_sec *rplay_fct_sec = \
        container_of(play_fct_base, struct dmx_r_playfct_pes_sec, play_fct_base); \
    WARN_ON(get_dmx_rbase_ops() != play_fct_base->base.ops); \
    if (&g_dmx_play_fct_sec_ops == rplay_fct_sec->ops) \
        ret = HI_TRUE; \
    ret; \
})

/* sub_playfct_avpes_ops */
struct dmx_r_playfct_avpes_ops {
    hi_s32(*open)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*get_attrs)(struct dmx_r_play_fct *rplay_fct, dmx_play_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_play_fct *rplay_fct, const dmx_play_attrs *attr);
    hi_s32(*get_status)(struct dmx_r_play_fct *rplay_fct, dmx_play_status *status);
    hi_s32(*attach_pid_ch)(struct dmx_r_play_fct *rplay_fct, struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*detach_pid_ch)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*acquire_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
        dmx_buffer *play_fct_buf);
    hi_s32(*release_buf)(struct dmx_r_play_fct *rplay_fct, hi_u32 rel_num, dmx_buffer *play_fct_buf);
    hi_s32(*start_idx)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*recv_idx)(struct dmx_r_play_fct *rplay_fct, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
        dmx_index_data *index);
    hi_s32(*stop_idx)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*close)(struct dmx_r_play_fct *rplay_fct);

    hi_s32(*pre_mmap)(struct dmx_r_play_fct *rplay_fct, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_play_fct *rplay_fct, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct dmx_r_play_fct *rplay_fct);
    hi_s32(*resume)(struct dmx_r_play_fct *rplay_fct);
};

#define IS_PLAYFCT_AVPES(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_play_fct *play_fct_base = (struct dmx_r_play_fct*)obj; \
    struct dmx_r_playfct_avpes *rplay_fct_av_pes = \
        container_of(play_fct_base, struct dmx_r_playfct_avpes, play_fct_base); \
    WARN_ON(get_dmx_rbase_ops() != play_fct_base->base.ops); \
    if (&g_dmx_play_fct_av_pes_ops == rplay_fct_av_pes->ops) \
        ret = HI_TRUE; \
    ret; \
})

/*
 *  hw support max mq depth is 4K(0x1000), and the min mq depth is 16, single mq dsc size is 64 bytes.
 *
 */
#define MQ_MIN_RAM_DSC_DEPTH           0x10
#define MQ_MAX_RAM_DSC_DEPTH           0x1000
#define MQ_DEFAULT_RAM_DSC_DEPTH       0x1000 /* we use the max mq desc depth as default */
#define MQ_DEFAULT_RAM_DSC_SIZE        64     /* 64 bytes */
#define PES_OFFSET_IN_MQ               16     /* 16 bytes */
#define MQ_DSC_ADDR_ALIGN              0x1000
#define DMX_PES_HEAD_FIX_LEN           0x6      /* 6 bytes */
#define DMX_PES_HEADER_LENGTH          0x9
#define DMX_PES_HEADER_AND_PTS_LENGTH  0xE
#define DMX_SEC_HEAD_FIX_LEN           0x3      /* 3 bytes */
#define DMX_PES_PACKAGE_MAX_LEN       (64 * 1024)     /* 64 KB */

/*
Passing decode parameters
*/
struct dmx_play_ts {
    hi_u32            ts_chan_id;
};

struct dmx_play_pes_sec {
    hi_u32               pes_sec_chan_id;
    dmx_flt_crc_mode     crc_mode;
    osal_mutex           play_flt_list_lock;
    struct list_head     play_flt_head;    /* filter list head */
    hi_u32               play_flt_cnt;     /* filter count of one playfct */
    DECLARE_BITMAP(play_flt_bitmap, DMX_FLT_CNT_PER_PLAYFCT);
};

struct dmx_play_avpes {
    hi_u32             av_pes_chan_id;
    hi_u32             av_pes_scd_id;

    struct dmx_r_buf  *scd_rbuf;
};

/* sub playfct */
struct dmx_r_sub_play_fct {
    struct dmx_r_play_fct         play_fct_base;   /* !!! it must be first entry. */
    struct dmx_r_play_fct_ops     *ops;

    union {
        struct dmx_play_ts       play_ts;;     /* whole ts channle type */
        struct dmx_play_pes_sec  play_pes_sec;  /* pes section channle type */
        struct dmx_play_avpes    play_avpes;   /* avpes channle type */
    } play_type;
};

/* sub playfct ts */
struct dmx_r_playfct_ts {
    struct dmx_r_play_fct           play_fct_base;   /* !!! it must be first entry. */
    struct dmx_r_playfct_ts_ops     *ops;

    struct dmx_play_ts              play_ts;
};

/* sub playfct pes_sec */
struct dmx_r_playfct_pes_sec {
    struct dmx_r_play_fct         play_fct_base;   /* !!! it must be first entry. */
    struct dmx_r_playfct_pes_sec_ops *ops;

    struct dmx_play_pes_sec       play_pes_sec;
};

/* sub playfct avpes */
struct dmx_r_playfct_avpes {
    struct dmx_r_play_fct             play_fct_base;   /* !!! it must be first entry. */
    struct dmx_r_playfct_avpes_ops    *ops;

    struct dmx_play_avpes             play_avpes;
};

/* interface */
hi_s32 dmx_play_fct_create(const dmx_play_attrs *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_play_fct_open(hi_handle handle);
hi_s32 dmx_play_fct_get_attrs(hi_handle handle, dmx_play_attrs *attrs);
hi_s32 dmx_play_fct_set_attrs(hi_handle handle, const dmx_play_attrs *attrs);
hi_s32 dmx_play_fct_get_status(hi_handle handle, dmx_play_status *status);
hi_s32 dmx_play_get_scrambled_flag(hi_handle handle, hi_dmx_scrambled_flag *scramble_flag);
hi_s32 dmx_play_get_packet_num(hi_handle handle, dmx_chan_packet_num *chn_stat);
hi_s32 dmx_play_fct_get_pid_ch(hi_handle handle, hi_handle *pid_handle);
hi_s32 dmx_play_fct_attach_pid_ch(hi_handle handle, hi_handle pid_handle);
hi_s32 dmx_play_fct_detach_pid_ch(hi_handle handle);

hi_s32 dmx_play_fct_create_filter(const dmx_filter_attrs *attrs, hi_handle *flt_handle, struct dmx_session *session);
hi_s32 dmx_play_fct_add_filter(hi_handle handle, hi_handle flt_handle);
hi_s32 dmx_play_fct_del_filter(hi_handle handle, hi_handle flt_handle);
hi_s32 dmx_play_fct_del_all_filter(hi_handle handle);
hi_s32 dmx_play_fct_update_filter(hi_handle flt_handle, const dmx_filter_attrs *attrs);
hi_s32 dmx_play_fct_get_filter_attrs(hi_handle flt_handle, dmx_filter_attrs *attrs);
hi_s32 dmx_play_fct_destroy_filter(hi_handle flt_handle);
hi_s32 dmx_play_fct_get_handle_by_filter(hi_handle flt_handle, hi_handle *play_handle);

hi_s32 dmx_play_fct_acquire_buf(hi_handle handle, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf);
hi_s32 dmx_play_fct_release_buf(hi_handle handle, hi_u32 rel_num, dmx_buffer *play_fct_buf);
hi_s32 dmx_play_fct_reset_buf(hi_handle handle);
hi_s32 dmx_play_fct_start_idx(hi_handle handle);
hi_s32 dmx_play_fct_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
    dmx_index_data *index);
hi_s32 dmx_play_fct_stop_idx(hi_handle handle);

hi_s32 dmx_play_fct_close(hi_handle handle);
hi_s32 dmx_play_fct_destroy(hi_handle handle);

hi_s32 dmx_play_fct_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
hi_s32 dmx_play_fct_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);
hi_s32 dmx_play_fct_get_data_handle(hi_handle *valid_array, hi_u32 *valid_num, hi_u32 watch_num, hi_u32 time_out_ms);
hi_s32 dmx_play_fct_select_data_handle(hi_u32 *watch_array, hi_u32 watch_num, hi_handle *valid_array,
    hi_u32 *valid_num, hi_u32 time_out_ms);
hi_s32 dmx_play_fct_set_eos_flag(hi_handle handle);
hi_s32 dmx_play_fct_set_callback(hi_handle play_handle, hi_handle user_handle, drv_dmx_callback cb);
hi_s32 dmx_play_fct_get_bufhandle(hi_handle hadnle, hi_handle *buf_handle, dmx_play_type *type);
hi_s32 dmx_play_cc_repeat_set(hi_handle handle, dmx_chan_cc_repeat_mode mode);
hi_void dmx_play_pusi_set(hi_bool no_pusi);

/* playfct mgmt  */
hi_s32 dmx_mgmt_create_play_fct(const dmx_play_attrs *attrs, struct dmx_r_play_fct **rplay_fct);
hi_s32 dmx_mgmt_destroy_play_fct(struct dmx_r_base *obj);

/* flt begin */
extern struct dmx_r_flt_ops g_dmx_flt_ops;

typedef enum {
    DMX_FLT_ATTR_INIT                 =    0,
    DMX_FLT_ATTR_SET                  =    1,
    DMX_FLT_ATTR_ATTACH               =    2,
    DMX_FLT_ATTR_MAX
} flt_attr_status;

typedef struct {
    hi_u32                     pes_sec_id;
    hi_u32                     flt_index; /* flt index of per pes_sec_id, 0~31 */
    hi_u32                     buf_id;
    dmx_flt_crc_mode           crc_mode;
} flt_attach_attr;

struct dmx_r_flt {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_flt_ops       *ops;
    hi_bool                    staled;
    osal_mutex                 lock;
    struct list_head           node0;  /* list in mgmt */
    struct list_head           node1;  /* list in dmx_r_play_fct */

    hi_handle                  flt_handle;

    struct dmx_r_play_fct      *rplay_fct;

    hi_u32                     pes_sec_id;
    hi_u32                     flt_index; /* flt index of per pes_sec_id, 0~31 */
    hi_u32                     buf_id;
    flt_attr_status            attr_status;
    dmx_flt_crc_mode           crc_mode;

    hi_u32                     depth;
    hi_u8                      match[DMX_FILTER_MAX_DEPTH];
    hi_u8                      mask[DMX_FILTER_MAX_DEPTH];
    hi_u8                      negate[DMX_FILTER_MAX_DEPTH];
};

#define IS_FLT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_flt *rflt = container_of(base, struct dmx_r_flt, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_flt_ops == rflt->ops) \
        ret = HI_TRUE; \
    ret; \
})

/* filter ops */
struct dmx_r_flt_ops {
    hi_s32(*create)(struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs);
    hi_s32(*attach)(struct dmx_r_flt *rflt, struct dmx_r_play_fct *rplay_fct, const flt_attach_attr *attach_attr);
    hi_s32(*detach)(struct dmx_r_flt *rflt);
    hi_s32(*set_attrs)(struct dmx_r_flt *rflt, const dmx_filter_attrs *attrs);
    hi_s32(*get_attrs)(struct dmx_r_flt *rflt, dmx_filter_attrs *attrs);
    hi_s32(*destroy)(struct dmx_r_flt *rflt);

    hi_s32(*suspend)(struct dmx_r_flt *rflt);
    hi_s32(*resume)(struct dmx_r_flt *rflt);
};

/* flt mgmt  */
hi_s32 dmx_mgmt_create_flt(const dmx_filter_attrs *attrs, struct dmx_r_flt **rflt);
hi_s32 dmx_mgmt_destroy_flt(struct dmx_r_base *obj);

#ifdef HI_DEMUX_PROC_SUPPORT
hi_void dmx_play_fct_save_es(struct dmx_r_play_fct *rplay_fct, dmx_buffer *play_buf);
hi_s32 dmx_play_fct_start_save_es(hi_void);
hi_void dmx_play_fct_stop_save_es(hi_void);
#endif /* HI_DEMUX_PROC_SUPPORT */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* __DRV_DEMUX_playFCT_H__ */


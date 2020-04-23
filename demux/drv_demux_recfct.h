/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-06-05
 */
#ifndef __DRV_DEMUX_RECFCT_H__
#define __DRV_DEMUX_RECFCT_H__

#include "drv_demux_define.h"
#include "hi_drv_demux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/***********************rec_fct begin******************************************/
extern struct dmx_r_rec_fct_ops g_dmx_rec_fct_ops;

#define DMX_REC_MIN_BUF_SIZE                0x4000    /* 16K */
#define DMX_REC_MAX_BUF_SIZE                0x4000000 /* 64M */

/* both 4K, 47K, 188 align */
#define DMX_REC_TS_PACKETS_UNIT             (16 * 188)
#define DMX_REC_BUF_SIZE_ALIGN              (64 * DMX_REC_TS_PACKETS_UNIT)

/* both 4K, 48K, 192 align */
#define DMX_REC_TS_WITH_TIMESTAMP_UNIT      (16 * 192)
#define DMX_REC_BUF_SIZE_TIMESTAMP_ALIGN    (64 * DMX_REC_TS_WITH_TIMESTAMP_UNIT)

#define DMX_MAX_INDEX_CNT                    256

/* recorded ts time stamp mode */
typedef enum hi_dmx_rec_timestamp_mode {
    DMX_REC_TIMESTAMP_NONE,           /* no time stamp added before each recoreded  ts packet */
    DMX_REC_TIMESTAMP_ZERO,           /* use 4 byte 0  added before each recoreded  ts packet */
    DMX_REC_TIMESTAMP_HIGH32BIT_SCR,  /* use high 32 bit of scr_base (4 byte) added before each recoreded ts packet */
    DMX_REC_TIMESTAMP_LOW32BIT_SCR,   /* use low 32 bit of scr_base (4 byte) added before each recoreded ts packet */
} dmx_rec_timestamp_mode;

struct dmx_r_rec_fct_ops {
    hi_s32(*create)(struct dmx_r_rec_fct *rrec_fct, const dmx_rec_attrs *attrs);
    hi_s32(*open)(struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*get_attrs)(struct dmx_r_rec_fct *rrec_fct, dmx_rec_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_rec_fct *rrec_fct, const dmx_rec_attrs *attr);
    hi_s32(*get_status)(struct dmx_r_rec_fct *rrec_fct, dmx_rec_status *status);
    hi_s32(*add_ch)(struct dmx_r_rec_fct *rrec_fct, struct dmx_r_base *obj);
    hi_s32(*del_ch)(struct dmx_r_rec_fct *rrec_fct, struct dmx_r_base *obj);
    hi_s32(*del_all_ch)(struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*acquire_buf)(struct dmx_r_rec_fct *rrec_fct, hi_u32 req_len, hi_u32 time_out, dmx_buffer *rec_fct_buf);
    hi_s32(*release_buf)(struct dmx_r_rec_fct *rrec_fct, dmx_buffer *rec_fct_buf);
    hi_s32(*recv_idx)(struct dmx_r_rec_fct *rrec_fct, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
        dmx_index_data *index);
    hi_s32(*peek_idx_buf)(struct dmx_r_rec_fct *rrec_fct, hi_u32 time_out, hi_u32 *index_num, hi_u32 *rec_data_len);
    hi_s32(*close)(struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*destroy)(struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*get_buf_handle)(struct dmx_r_rec_fct *rrec_fct, hi_handle *buf_handle);

    hi_s32(*pre_mmap)(struct dmx_r_rec_fct *rrec_fct, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
    hi_s32(*pst_mmap)(struct dmx_r_rec_fct *rrec_fct, hi_void *buf_usr_addr);

    hi_s32(*suspend)(struct dmx_r_rec_fct *rrec_fct);
    hi_s32(*resume)(struct dmx_r_rec_fct *rrec_fct);
};

#define IS_RECFCT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_rec_fct *rrec_fct = container_of(base, struct dmx_r_rec_fct, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_rec_fct_ops == rrec_fct->ops) \
        ret = HI_TRUE; \
    ret; \
})

#define REC_MAX_PID_CNT 32

struct dmx_r_rec_fct {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_rec_fct_ops   *ops;
    hi_bool                    staled;
    osal_mutex                 lock;
    struct list_head           node; /* list in mgmt */

    hi_handle                  rec_fct_handle;

    struct dmx_r_buf           *rbuf;

    hi_u32                     scd_id;
    struct dmx_r_buf           *scd_rbuf;
#if (DMX_REC_BUF_GAP_EXIST == 1)
    hi_u32                     ts_cnt_compensate;
    hi_u64                     last_scd_offset;
#endif
    /* for all pid record */
    struct dmx_r_band          *rband;

    /* for select pid record */
    osal_mutex                 pid_ch_list_lock;
    struct dmx_r_pid_ch       *pid_chan_array[REC_MAX_PID_CNT]; /* record the pidch that add to the recfct */
    DECLARE_BITMAP(pid_bit_index, REC_MAX_PID_CNT);

    /* for video index */
    osal_mutex                 index_list_lock;
    struct list_head           index_list_head;
    hi_u32                     index_cnt;
    osal_task                 *rec_index_thread;

    /* recfct attribute */
    hi_bool                    descramed;
    hi_u32                     index_src_pid;
    dmx_rec_index_type         index_type;
    dmx_rec_type               rec_type;
    dmx_vcodec_type            vcodec_type;
    dmx_secure_mode            secure_mode;  /* TEE or REE */
    dmx_ts_crc_scb             crc_scb;

    /* recfct index */
    hi_u32                     pic_parser;
    hi_u64                     prev_scr_clk;
    hi_u32                     first_frame_ms;
    hi_u32                     prev_frame_ms;
    hi_u32                     cnt32to63helper;

    dmx_index_data             last_frame_info;
    dmx_rec_timestamp_mode     rec_time_stamp;
};

/* interface */
hi_s32 dmx_rec_fct_create(const dmx_rec_attrs *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_rec_fct_open(hi_handle handle);
hi_s32 dmx_rec_fct_get_attrs(hi_handle handle, dmx_rec_attrs *attrs);
hi_s32 dmx_rec_fct_set_attrs(hi_handle handle, const dmx_rec_attrs *attrs);
hi_s32 dmx_rec_fct_set_eos_flag(hi_handle handle, hi_bool eos_flag);
hi_s32 dmx_rec_fct_get_status(hi_handle handle, dmx_rec_status *status);

hi_s32 dmx_rec_fct_add_ch(hi_handle handle, hi_handle ch_handle);
hi_s32 dmx_rec_fct_del_ch(hi_handle handle, hi_handle ch_handle);
hi_s32 dmx_rec_fct_del_all_ch(hi_handle handle);

hi_s32 dmx_rec_fct_acquire_buf(hi_handle handle, hi_u32 req_len, hi_u32 time_out, dmx_buffer *rec_fct_buf);
hi_s32 dmx_rec_fct_release_buf(hi_handle handle, dmx_buffer *rec_fct_buf);

hi_s32 dmx_rec_fct_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
                            dmx_index_data *index);
hi_s32 drv_rec_fct_peek_idx_and_buf(hi_handle handle, hi_u32 time_out, hi_u32 *index_num, hi_u32 *rec_data_len);

hi_s32 dmx_rec_fct_close(hi_handle handle);
hi_s32 dmx_rec_fct_destroy(hi_handle handle);


hi_s32 dmx_rec_fct_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
hi_s32 dmx_rec_fct_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);

/* recfct mgmt  */
hi_s32 dmx_mgmt_create_rec_fct(const dmx_rec_attrs *attrs, struct dmx_r_rec_fct **rrec_fct);
hi_s32 dmx_mgmt_destroy_rec_fct(struct dmx_r_base *obj);

hi_s32 dmx_rec_fct_get_bufhandle(hi_handle handle, hi_handle *buf_handle);

#ifdef HI_DEMUX_PROC_SUPPORT
hi_s32 dmx_rec_fct_start_save_all_ts(hi_u32 port_id);
hi_void dmx_rec_fct_stop_save_all_ts(hi_void);
hi_s32 dmx_rec_fct_start_save_dmx_ts(hi_u32 band);
hi_void dmx_rec_fct_stop_save_dmx_ts(hi_void);
#endif /* HI_DEMUX_PROC_SUPPORT */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* __DRV_DEMUX_RECFCT_H__ */


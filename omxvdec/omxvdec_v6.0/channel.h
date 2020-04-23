/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "omxvdec.h"
#include "message.h"
#include "dec_sync.h"

#define LAST_FRAME_FLAG_NULL      (0)
#define DECODER_REPORT_LAST_FRAME (1)
#define OMX_REPORT_LAST_FRAME     (2)

#define DECODER_REPORT_LAST_FRAME_SUCCESS (0)
#define DECODER_REPORT_LAST_FRAME_FAIL    (1)
#define DECODER_REPORT_LAST_FRAME_WITH_ID (2)
#define MAX_MEM_RECORDE_NUM               (32)

#define OMX_MAKE_HANDLE(chan_id) HI_HANDLE_INIT(HI_ID_OMXVDEC, 0, chan_id)
#define OMX_GET_CHANID_BY_HANDLE(handle) HI_HANDLE_GET_CHAN_ID(handle)

#define VDEC_INIT_MUTEX(p_sem) \
    do {                      \
        osal_sem_init(p_sem, 1);   \
    } while (0)

#define VDEC_DOWN_INTERRUPTIBLE(p_sem)                            \
    do {                                                         \
        if (osal_sem_down_interruptible(p_sem)) {                          \
            omx_print(OMX_FATAL, "down_interruptible failed!\n"); \
            return HI_FAILURE;                                   \
        }                                                        \
    } while (0)

#define VDEC_DOWN_INTERRUPTIBLE_RETURN(p_sem, err_ret)             \
    do {                                                         \
        if (osal_sem_down_interruptible(p_sem)) {                          \
            omx_print(OMX_FATAL, "down_interruptible failed!\n"); \
            return err_ret;                                       \
        }                                                        \
    } while (0)

#define VDEC_UP_INTERRUPTIBLE(p_sem) \
    do {                            \
        osal_sem_up(p_sem);                   \
    } while (0)

typedef enum {
    CHAN_STATE_INVALID = 0,
    CHAN_STATE_IDLE,
    CHAN_STATE_WORK,
    CHAN_STATE_PAUSE,
    CHAN_STATE_PAUSE_PENDING,
} e_chan_state;

typedef enum {
    BUF_STATE_INVALID = 0,
    BUF_STATE_IDLE,
    BUF_STATE_QUEUED,
    BUF_STATE_USING,
} e_buffer_state;

typedef enum {
    STATUS_INVALID = 0,
    DECODER_REPORT_FAILED = 1,
    DECODER_REPORT_SUCCESS,
    DECODER_REPORT_WITH_ID,
    CHAN_REPORT_SUCCESS,
} e_eos_report_state;

typedef struct {
    hi_u32 buf_fd;
    hi_u8 *pu8_start_vir_addr;
    hi_u8 *pu8_dma_buf;
    hi_u32 offset;
} omxvdec_ion_buf;

typedef struct {
    hi_u64 data;
    hi_u32 metadata_phy_addr;
    hi_mem_handle_t metadata_fd;
    hi_bool last_frame;
} ext_info;

typedef struct {
    e_buffer_type buf_type;
    e_buffer_state status;
    hi_u32 phy_addr;
    hi_u32 private_phy_addr;  // for store video struct
    hi_u32 buf_len;
    hi_u32 act_len;
    hi_u32 private_len;
    hi_s64 private_fd;
    hi_u32 offset;
    hi_u32 flags;
    hi_u32 buf_id;
    hi_s64 time_stamp;
    hi_u32 usr_tag;
    struct list_head list;

    hi_void *user_vaddr;
    hi_void *kern_vaddr;
    hi_void *client_data;
    hi_void *private_vaddr; /* kernel viradd of metadata buffer */
    hi_void *hfbc_vaddr;
    hi_u32 frame_rate;
    hi_bool is_bind;
    omxvdec_ion_buf frame_ion_buffer;
    omxvdec_ion_buf ion_buffer;
    omxvdec_ion_buf hfbc_ion_buffer;
    ext_info ext_info;
    hi_bool is_first_buf;
    vfmw_image_attached attach_buf;
    hi_u64 dma_buf;
    hi_s64 fd;
    hi_s32 release_fd;
} omxvdec_buf;

typedef struct {
    hi_u32 ETB;
    hi_u32 EBD;
    hi_u32 FTB;
    hi_u32 FBD;
    hi_u32 decoder_in;
    hi_u32 decoder_out;

    hi_u32 get_stream_try;
    hi_u32 get_stream_ok;
    hi_u32 release_stream_try;
    hi_u32 release_stream_ok;
    hi_u32 report_image_try;
    hi_u32 report_image_ok;
    hi_u32 release_image_try;
    hi_u32 release_image_ok;
} OMXVDEC_CHAN_STATINFO;

typedef struct {
    e_mem_alloc e_extra_mem_alloc;
    e_mem_alloc e_eos_mem_alloc;
    omxvdec_buffer channel_extra_buf;
    omxvdec_buffer decoder_eos_buf;
    hi_u32 alloc_num;
    OMXVDEC_BUF_DESC chan_buf[MAX_MEM_RECORDE_NUM];
} OMXVDEC_MEM_RECORDER;

typedef struct {
    e_eos_report_state status;
    hi_u32 image_id;
    hi_bool eos_in_list;
    hi_u8 eos_receive_num;
    hi_u8 eos_send_num;
    hi_u8 eos_try_num;
} OMXVDEC_EOS_INFO;

typedef struct {
    hi_s32 is_valid;
    hi_s64 fd;
    hi_u64 dma_buf;
    hi_u64 phy_addr;
} omxvdec_ext_buf_info;

typedef struct {
    hi_s32 channel_id;
    hi_s32 decoder_id;
    hi_handle ssm_handle;
    hi_handle omx_handle;
    hi_bool m_use_native_buf;
    hi_u32 out_width;
    hi_u32 out_height;
    hi_u32 out_stride;
    hi_u32 protocol;
    hi_s32 rotation;
    hi_u32 bReversed;
    hi_u32 input_buf_num;
    hi_u32 output_buf_num;
    hi_u32 max_in_buf_num;
    hi_u32 max_out_buf_num;
    hi_u32 msg_queue_depth;
    hi_u32 raw_use_cnt;
    hi_u32 yuv_use_cnt;
    hi_u32 input_flush_pending : 1,
           output_flush_pending : 1,
           recfg_flag : 1,  // reconfig flag
           port_enable_flag : 1,
           reset_pending : 1,  // reset flag
           seek_pending : 1;
    hi_u32 *file_dec;

    hi_bool is_tvp;
    hi_bool is_overlay;
    hi_bool is_10bit;
    hi_bool anw_store_meta;
    OMXVDEC_EOS_INFO eos_info;
    hi_s32 b_lowdelay;
    hi_bool b_is_on_live;
    hi_bool b_is_gst;
    hi_bool b_meta_data_alloc;
    hi_bool b_vpss_bypass;
    hi_bool output_afbc;
    omx_pix_format color_format;
    e_chan_state state;

    hi_void *in_buf_table;
    hi_void *out_buf_table;
    msg_queue_s *msg_queue;
    OMXVDEC_ENTRY *vdec;

    struct list_head chan_list;
    struct list_head raw_queue;
    struct list_head yuv_queue;
    omxvdec_buf last_frame;

    osal_semaphore chan_sema;
    osal_spinlock chan_lock;
    osal_spinlock raw_lock;
    osal_spinlock yuv_lock;

    vfmw_stream_intf stream_ops;

    OMXVDEC_CHAN_STATINFO omx_chan_statinfo;
    OMXVDEC_LOWDELAY_PROC_RECORD *omx_vdec_lowdelay_proc_rec;
    hi_u32 tunnel_mode_enable;
    hi_bool is_net_work;
    hi_bool android_test;
    OMXVDEC_MEM_RECORDER mem_recorder;
    hi_void *in_buf_start_vir;
    hi_u32 in_buf_start_phy;
    hi_drv_hdr_type src_frame_type;
    hi_drv_color_descript st_colour_info;
    hi_drv_hdr_static_metadata st_hdr10_info;
    hi_bool set_corlour_info;
    hi_bool set_hdr_static_info;
    hi_drv_video_frame *frame;
    omxvdec_ext_buf_info ext_buf[OMXVDEC_MAX_EXT_BUF_NUM];
#ifdef HI_DEC_SYNC_SUPPORT
    dec_sync_info sync_info;
    struct workqueue_struct *release_queue;
    hi_u32 work_number;
#endif
} OMXVDEC_CHAN_CTX;

#ifdef HI_DEC_SYNC_SUPPORT
typedef struct {
    OMXVDEC_CHAN_CTX *chan_info;
    OMXVDEC_BUF_DESC buf_info;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    struct dma_fence *fence;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    struct fence *fence;
#else
    struct hi_sync_fence *fence;
#endif

    struct work_struct sync_work;
} omx_vdec_work_queue;
#endif

typedef struct tag_vdec_premmz_node {
    hi_u32 start_vir_addr;
    hi_u32 start_phy_addr;
    hi_u32 size;
    hi_u32 node_state; /* 0:have MMZ not used,1:have MMZ but used 2:invalid */
} vdec_premmz_node;

hi_s32 channel_init(hi_void);

hi_void channel_exit(hi_void);

hi_s32 channel_create_inst(struct file *fd, OMXVDEC_DRV_CFG *pcfg);

hi_s32 channel_release_inst(OMXVDEC_CHAN_CTX *pchan, omx_mem_free_type free_type);

hi_s32 channel_start_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 channel_stop_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 channel_reset_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 channel_pause_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 channel_resume_inst(OMXVDEC_CHAN_CTX *pchan);

hi_s32 channel_bind_user_buffer(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf);

hi_s32 channel_unbind_user_buffer(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf, omx_mem_free_type free_type);

hi_s32 channel_empty_this_stream(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf);

hi_s32 channel_fill_this_frame(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf);

hi_s32 channel_flush_inst(OMXVDEC_CHAN_CTX *pchan, e_port_dir dir);

hi_s32 channel_get_message(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_MSG_INFO *pmsg);

hi_s32 channel_report_message(OMXVDEC_CHAN_CTX *pchan, hi_u32 msgcode, const hi_void *priv);

OMXVDEC_CHAN_CTX *channel_find_inst_by_channel_id(OMXVDEC_ENTRY *vdec, hi_s32 handle);

OMXVDEC_CHAN_CTX *channel_find_inst_by_decoder_id(OMXVDEC_ENTRY *vdec, hi_s32 handle);

OMXVDEC_CHAN_CTX *channel_find_inst_by_processor_id(OMXVDEC_ENTRY *vdec, hi_s32 handle);

hi_void channel_add_lowdelay_tag_time(OMXVDEC_CHAN_CTX *pchan, hi_u32 tag, hi_u32 type, hi_u32 time);

hi_void channel_proc_entry(struct seq_file *p, OMXVDEC_CHAN_CTX *pchan);

hi_s32 channel_alloc_buf(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf);

hi_s32 channel_release_buf(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_BUF_DESC *puser_buf, omx_mem_free_type free_type);

hi_s32 channel_vdec_report(hi_s32 chan_id, hi_u32 event_id, hi_s32 result, hi_void *private_data);

hi_s32 channel_config_inst(OMXVDEC_CHAN_CTX *pchan, OMXVDEC_DRV_CFG *pcfg);

hi_s32 channel_fill_anw_store_meta(OMXVDEC_CHAN_CTX *chan, OMXVDEC_BUF_DESC *user_buf);

#endif


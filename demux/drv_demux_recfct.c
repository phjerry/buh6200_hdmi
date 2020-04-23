/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */
#include "drv_demux_recfct.h"
#include <linux/kernel.h>
#include <linux/kthread.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/printk.h"
#include "linux/ratelimit.h"
#include "linux/hisilicon/securec.h"

#include "hi_drv_sys.h"
#include "hi_drv_module.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_file.h"
#include "hi_drv_demux.h"
#include "hi_type.h"

#include "hal_demux.h"
#include "drv_demux_index.h"
#include "drv_demux_scd.h"
#include "drv_demux_func.h"
#include "drv_demux_define.h"
#include "drv_demux_utils.h"
#include "drv_demux_tee.h"

#define DMX_R_REC_FCT_GET(handle, rrec_fct) ({ \
    hi_s32 ret = HI_FAILURE; \
    ret = dmx_r_get(handle, (struct dmx_r_base **)&rrec_fct); \
    if (ret == HI_SUCCESS) { \
        if (&g_dmx_rec_fct_ops != rrec_fct->ops) { \
            dmx_r_put((struct dmx_r_base*)rrec_fct);\
            HI_ERR_DEMUX("handle is in active, but not a valid rec_fct handle(0x%x).\n", handle); \
            ret = HI_ERR_DMX_INVALID_PARA; \
        } \
    } \
    ret;\
})

static hi_s32 dmx_rec_trim_attrs(const dmx_rec_attrs *in_rec_attr, dmx_rec_attrs *out_rec_attr)
{
    dmx_secure_mode secure_mode = DMX_SECURE_NONE;

    if ((in_rec_attr->rec_buf_size < DMX_REC_MIN_BUF_SIZE) || (in_rec_attr->rec_buf_size > DMX_REC_MAX_BUF_SIZE)) {
        HI_ERR_DEMUX("invalid rec buffer size:0x%x\n", in_rec_attr->rec_buf_size);
        return HI_ERR_DMX_INVALID_PARA;
    }

    if (in_rec_attr->ts_packet_type == DMX_TS_PACKET_192 && in_rec_attr->secure_mode == DMX_SECURE_TEE) {
        HI_ERR_DEMUX("tee not support DMX_TS_PACKET_192\n");
        return HI_ERR_DMX_NOT_SUPPORT;
    }

    memcpy(out_rec_attr, in_rec_attr, sizeof(dmx_rec_attrs));

    switch (in_rec_attr->rec_type) {
        case DMX_REC_TYPE_SELECT_PID: {
            switch (in_rec_attr->index_type) {
                case DMX_REC_INDEX_TYPE_VIDEO: {
                    if (in_rec_attr->index_src_pid >= DMX_INVALID_PID) {
                        HI_ERR_DEMUX("invalid index pid:0x%x\n", in_rec_attr->index_src_pid);
                        return HI_ERR_DMX_INVALID_PARA;
                    }

                    switch (in_rec_attr->vcodec_type) {
                        case DMX_VCODEC_TYPE_MPEG2:
                        case DMX_VCODEC_TYPE_MPEG4:
                        case DMX_VCODEC_TYPE_AVS:
                        case DMX_VCODEC_TYPE_H264:
                        case DMX_VCODEC_TYPE_HEVC: {
                            break;
                        }
                        default : {
                            HI_ERR_DEMUX("invalid vcodec type:%d\n", in_rec_attr->vcodec_type);
                            return HI_ERR_DMX_INVALID_PARA;
                        }
                    }
                    break;
                }

                case DMX_REC_INDEX_TYPE_AUDIO: {
                    if (in_rec_attr->index_src_pid >= DMX_INVALID_PID) {
                        HI_ERR_DEMUX("invalid index pid:0x%x\n", in_rec_attr->index_src_pid);
                        return HI_ERR_DMX_INVALID_PARA;
                    }
                    break;
                }

                case DMX_REC_INDEX_TYPE_NONE: {
                    break;
                }

                case DMX_REC_INDEX_TYPE_MAX: {
                    return HI_ERR_DMX_INVALID_PARA;
                }
            }
            break;
        }

        case DMX_REC_TYPE_ALL_PID:
        case DMX_REC_TYPE_ALL_DATA: {
            out_rec_attr->index_type = DMX_REC_INDEX_TYPE_NONE;
            break;
        }

        default : {
            HI_ERR_DEMUX("invalid rec_attr->rec_type:%d\n", in_rec_attr->rec_type);
            return HI_ERR_DMX_INVALID_PARA;
        }
    }

    switch (in_rec_attr->secure_mode) {
        case DMX_SECURE_TEE:
            switch (in_rec_attr->rec_type) {
                case DMX_REC_TYPE_SELECT_PID:
                    break;

                default:
                    HI_ERR_DEMUX("invalid secure rec type.\n");
                    return HI_ERR_DMX_INVALID_PARA;
            }

            secure_mode = DMX_SECURE_TEE;
            break;

        case DMX_SECURE_NONE:
            secure_mode = DMX_SECURE_NONE;
            break;

        default:
            HI_ERR_DEMUX("invalid secure rec type.\n");
            return HI_ERR_DMX_INVALID_PARA;
    }

    out_rec_attr->secure_mode = secure_mode;

    return HI_SUCCESS;
}

hi_s32 dmx_mgmt_create_rec_fct(const dmx_rec_attrs *attrs, struct dmx_r_rec_fct **rrec_fct)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();
    dmx_rec_attrs invalid_attrs = {0};

    if (attrs == HI_NULL) {
        HI_ERR_DEMUX("invalid parameter:attrs(%#x).\n", attrs);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    ret = dmx_rec_trim_attrs(attrs, &invalid_attrs);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("invalid attrs.\n");
        goto out;
    }

    ret = mgmt->ops->create_rec_fct(mgmt, &invalid_attrs, rrec_fct);

out:
    return ret;
}

hi_s32 dmx_mgmt_destroy_rec_fct(struct dmx_r_base *obj)
{
    hi_s32 ret;
    struct dmx_mgmt *mgmt = get_dmx_mgmt();

    ret = mgmt->ops->destroy_rec_fct(mgmt, (struct dmx_r_rec_fct *)obj);

    return ret;
}

/*************************dmx_mgmt_rec_fct*************************************************/
hi_s32 dmx_rec_fct_create(const dmx_rec_attrs *attrs, hi_handle *handle, struct dmx_session *session)
{
    hi_s32 ret;
    struct dmx_slot  *slot = HI_NULL;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = dmx_mgmt_create_rec_fct(attrs, &rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* get the rec_fct obj */
    dmx_r_get_raw((struct dmx_r_base *)rrec_fct);

    /* create the buf and others */
    ret = rrec_fct->ops->create(rrec_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* associate a slot to the object  */
    ret = dmx_slot_create((struct dmx_r_base *)rrec_fct, &slot);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    /* associate an slot to the session. */
    slot->release = dmx_rec_fct_destroy;
    ret = dmx_session_add_slot(session, slot);
    if (ret != HI_SUCCESS) {
        goto out3;
    }

    *handle = slot->handle;
    rrec_fct->rec_fct_handle = slot->handle;

    /* release the rec_fct obj */
    dmx_r_put((struct dmx_r_base *)rrec_fct);

    return HI_SUCCESS;

out3:
    dmx_slot_destroy(slot);
out2:
    rrec_fct->ops->destroy(rrec_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rrec_fct);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out0:
    return ret;
}

hi_s32 dmx_rec_fct_open(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->open(rrec_fct);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_get_attrs(hi_handle handle, dmx_rec_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->get_attrs(rrec_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_set_attrs(hi_handle handle, const dmx_rec_attrs *attrs)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->set_attrs(rrec_fct, attrs);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_set_eos_flag(hi_handle handle, hi_bool eos_flag)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    osal_mutex_lock(&rrec_fct->lock);
    ret = dmx_r_get_raw((struct dmx_r_base *)rrec_fct->rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get obj failed.\n");
        goto out1;
    }
    osal_mutex_lock(&rrec_fct->rbuf->lock);
    rrec_fct->rbuf->eos_flag = eos_flag;
    osal_mutex_unlock(&rrec_fct->rbuf->lock);
    dmx_r_put((struct dmx_r_base *)rrec_fct->rbuf);

    if (rrec_fct->scd_rbuf != HI_NULL) {
        ret = dmx_r_get_raw((struct dmx_r_base *)rrec_fct->scd_rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("get obj failed.\n");
            goto out1;
        }
        osal_mutex_lock(&rrec_fct->scd_rbuf->lock);
        rrec_fct->scd_rbuf->eos_flag = eos_flag;
        osal_mutex_unlock(&rrec_fct->scd_rbuf->lock);
        dmx_r_put((struct dmx_r_base *)rrec_fct->scd_rbuf);
    }
    ret = HI_SUCCESS;
out1:
    osal_mutex_unlock(&rrec_fct->lock);
    dmx_r_put((struct dmx_r_base *)rrec_fct);
out0:
    return ret;
}

hi_s32 dmx_rec_fct_get_status(hi_handle handle, dmx_rec_status *status)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->get_status(rrec_fct, status);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_add_ch(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;
    struct dmx_r_pid_ch   *rpid_ch   = HI_NULL;
    struct dmx_r_band    *rband    = HI_NULL;
    struct dmx_r_rec_fct  *rrec_fct  = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (rrec_fct->rec_type == DMX_REC_TYPE_SELECT_PID) {
        ret = DMX_R_PID_CH_GET(ch_handle, rpid_ch);
        if (ret != HI_SUCCESS) {
            goto out1;
        }

        ret = rrec_fct->ops->add_ch(rrec_fct, (struct dmx_r_base *)rpid_ch);
        dmx_r_put((struct dmx_r_base *)rpid_ch);
    } else if (rrec_fct->rec_type == DMX_REC_TYPE_ALL_PID ||
               rrec_fct->rec_type == DMX_REC_TYPE_ALL_DATA) {
        ret = DMX_R_BAND_GET(ch_handle, rband);
        if (ret != HI_SUCCESS) {
            goto out1;
        }
        ret = rrec_fct->ops->add_ch(rrec_fct, (struct dmx_r_base *)rband);
        dmx_r_put((struct dmx_r_base *)rband);
    } else {
        HI_ERR_DEMUX("invalid rec_type[0x%x]\n", rrec_fct->rec_type);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rrec_fct);
out0:
    return ret;
}

hi_s32 dmx_rec_fct_del_ch(hi_handle handle, hi_handle ch_handle)
{
    hi_s32 ret;
    struct dmx_r_pid_ch   *rpid_ch   = HI_NULL;
    struct dmx_r_band    *rband    = HI_NULL;
    struct dmx_r_rec_fct  *rrec_fct  = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (rrec_fct->rec_type == DMX_REC_TYPE_SELECT_PID) {
        ret = DMX_R_PID_CH_GET(ch_handle, rpid_ch);
        if (ret != HI_SUCCESS) {
            goto out1;
        }
        ret = rrec_fct->ops->del_ch(rrec_fct, (struct dmx_r_base *)rpid_ch);
        dmx_r_put((struct dmx_r_base *)rpid_ch);
    } else if (rrec_fct->rec_type == DMX_REC_TYPE_ALL_PID ||
               rrec_fct->rec_type == DMX_REC_TYPE_ALL_DATA) {
        ret = DMX_R_BAND_GET(ch_handle, rband);
        if (ret != HI_SUCCESS) {
            goto out1;
        }
        ret = rrec_fct->ops->del_ch(rrec_fct, (struct dmx_r_base *)rband);
        dmx_r_put((struct dmx_r_base *)rband);
    } else {
        HI_ERR_DEMUX("invalid rec_type[0x%x]\n", rrec_fct->rec_type);
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out1;
    }

out1:
    dmx_r_put((struct dmx_r_base *)rrec_fct);
out0:
    return ret;
}

hi_s32 dmx_rec_fct_del_all_ch(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_rec_fct  *rrec_fct  = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    if (rrec_fct->rec_type != DMX_REC_TYPE_SELECT_PID) {
        HI_ERR_DEMUX("only select pid rectype support delallch\n");
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out1;
    }

    ret = rrec_fct->ops->del_all_ch(rrec_fct);

out1:
    dmx_r_put((struct dmx_r_base *)rrec_fct);
out0:
    return ret;
}

hi_s32 dmx_rec_fct_acquire_buf(hi_handle handle, hi_u32 req_len, hi_u32 time_out, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->acquire_buf(rrec_fct, req_len, time_out, rec_fct_buf);

    dmx_r_put((struct dmx_r_base *)rrec_fct);

out:
    return ret;
}

hi_s32 dmx_rec_fct_release_buf(hi_handle handle, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->release_buf(rrec_fct, rec_fct_buf);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num, dmx_index_data *index)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("DMX_R_REC_FCT_GET failed!\n");
        goto out;
    }

    ret = rrec_fct->ops->recv_idx(rrec_fct, req_num, time_out, reqed_num, index);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 drv_rec_fct_peek_idx_and_buf(hi_handle handle, hi_u32 time_out, hi_u32 *index_num, hi_u32 *rec_data_len)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("DMX_R_REC_FCT_GET failed!\n");
        goto out;
    }

    ret = rrec_fct->ops->peek_idx_buf(rrec_fct, time_out, index_num, rec_data_len);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_close(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->close(rrec_fct);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct dmx_slot *slot = HI_NULL;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = dmx_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    dmx_session_del_slot(slot->session, slot);

    dmx_slot_destroy(slot);

    ret = rrec_fct->ops->destroy(rrec_fct);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out1:
    dmx_r_put((struct dmx_r_base *)rrec_fct);
out0:
    return ret;
}

hi_s32 dmx_rec_fct_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->pre_mmap(rrec_fct, buf_handle, buf_size, buf_usr_addr);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_pst_mmap(hi_handle handle, hi_void *buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->pst_mmap(rrec_fct, buf_usr_addr);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
}

hi_s32 dmx_rec_fct_get_bufhandle(hi_handle handle, hi_handle *buf_handle)
{
#ifdef DMX_TEE_SUPPORT
    hi_s32 ret;
    struct dmx_r_rec_fct *rrec_fct = HI_NULL;

    ret = DMX_R_REC_FCT_GET(handle, rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = rrec_fct->ops->get_buf_handle(rrec_fct, buf_handle);

    dmx_r_put((struct dmx_r_base *)rrec_fct);
out:
    return ret;
#else
    return HI_ERR_DMX_NOT_SUPPORT;
#endif
}

/*************************dmx_r_rec_fct_xxx_impl*************************************************/
static inline hi_bool rec_fct_staled(struct dmx_r_rec_fct *rrec_fct)
{
    if (unlikely(rrec_fct->staled == HI_TRUE)) {
        HI_ERR_DEMUX("rec_fct is staled.\n");
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 _dmx_r_rec_fct_create_impl(struct dmx_r_rec_fct *rrec_fct, const dmx_rec_attrs *attrs)
{
    hi_u32 ret;
    struct dmx_mgmt *mgmt = rrec_fct->base.mgmt;
    struct dmx_r_buf *new_rbuf = HI_NULL;
    struct dmx_r_buf *new_scd_rbuf = HI_NULL;
    struct dmx_buf_attrs buf_attrs = {0};
    dmx_tee_mem_info rec_mem_info = {0};

    DMX_NULL_POINTER_RETURN(mgmt);

    buf_attrs.buf_size = attrs->rec_buf_size;
    buf_attrs.secure_mode = attrs->secure_mode;

    if (attrs->ts_packet_type == DMX_TS_PACKET_192) {
        /* both 4K 48K 192 align */
        buf_attrs.buf_size = buf_attrs.buf_size - buf_attrs.buf_size % DMX_REC_BUF_SIZE_TIMESTAMP_ALIGN;
    } else {
        /* both 4K 47K 188 align */
        buf_attrs.buf_size = buf_attrs.buf_size - buf_attrs.buf_size % DMX_REC_BUF_SIZE_ALIGN;
    }
#if (DMX_REC_BUF_GAP_EXIST == 1)
    buf_attrs.pkt_size = (attrs->ts_packet_type == DMX_TS_PACKET_192) ? DMX_TTS_PKT_SIZE : DMX_TS_PKT_SIZE;
#endif
    /* create the buffer one time */
    ret = dmx_mgmt_create_buf(&buf_attrs, &new_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("create recfct buf failed!\n");
        goto out;
    }

    /* assign the buffer object */
    rrec_fct->rbuf = new_rbuf;

    if (new_rbuf->secure_mode == DMX_SECURE_TEE) {
        ret = dmx_tee_create_rec_chan(rrec_fct->base.id, new_rbuf->real_buf_size,
                                      &rec_mem_info);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("create tee record buf failed!!\n");
            goto destroy_recbuf;
        }
    }

    /* init the buffer */
    dmx_r_get_raw((struct dmx_r_base *)new_rbuf);
    ret = new_rbuf->ops->init(new_rbuf, DMX_BUF_TYPE_REC, &rec_mem_info);
    dmx_r_put((struct dmx_r_base *)new_rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("init the record buf object failed!!\n");
        goto destroy_recchan;
    }

    /* clar ts rec channel */
    dmx_hal_scd_clear_chan(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);

    if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_NONE) {
        hi_u32                   id = 0;
        struct dmx_buf_attrs     scd_buf_attrs;
        dmx_tee_mem_info scd_mem_info = {0};

        /* clar ts index channel */
        dmx_hal_scd_clear_chan(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_TRUE);

        scd_buf_attrs.buf_size      =
            (rrec_fct->index_type == DMX_REC_INDEX_TYPE_VIDEO) ? DMX_VID_SCD_BUF_SIZE : DMX_AUD_SCD_BUF_SIZE;
        scd_buf_attrs.secure_mode   = DMX_SECURE_NONE;
        /* create the buffer one time */
        ret = dmx_mgmt_create_buf(&scd_buf_attrs, &new_scd_rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("create recfct buf failed!\n");
            goto deinit_recbuf;
        }

        /* assign the buffer object */
        rrec_fct->scd_rbuf = new_scd_rbuf;

        /* init the buffer */
        dmx_r_get_raw((struct dmx_r_base *)new_scd_rbuf);
        ret = new_scd_rbuf->ops->init(new_scd_rbuf, DMX_BUF_TYPE_SCD, &scd_mem_info);
        dmx_r_put((struct dmx_r_base *)new_scd_rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("create scd buf object failed!!\n");
            goto destroy_scdbuf;
        }

        /* create an scd channel */
        osal_mutex_lock(&mgmt->scd_chan_lock);
        id = find_first_zero_bit(mgmt->scd_bitmap, mgmt->scd_chan_cnt);
        if (!(id < mgmt->scd_chan_cnt)) {
            osal_mutex_unlock(&mgmt->scd_chan_lock);
            HI_ERR_DEMUX("there is no available scd channel now!\n");
            ret = HI_ERR_DMX_NO_RESOURCE;
            goto deinit_scdbuf;
        }
        set_bit(id, mgmt->scd_bitmap);
        osal_mutex_unlock(&mgmt->scd_chan_lock);

        rrec_fct->scd_id = id;
    }

    return HI_SUCCESS;

deinit_scdbuf:
    new_scd_rbuf->ops->deinit(new_scd_rbuf);
destroy_scdbuf:
    dmx_mgmt_destroy_buf((struct dmx_r_base *)new_scd_rbuf);
deinit_recbuf:
    new_rbuf->ops->deinit(new_rbuf);
destroy_recchan:
    if (new_rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_destroy_rec_chan(rrec_fct->base.id, &rec_mem_info);
    }
destroy_recbuf:
    dmx_mgmt_destroy_buf((struct dmx_r_base *)new_rbuf);
out:
    return ret;
}

static hi_s32 dmx_r_rec_fct_create_impl(struct dmx_r_rec_fct *rrec_fct, const dmx_rec_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_create_impl(rrec_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 __dmx_r_recfct_scd_open(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 pic_parser;
    vidstd vid_std = VIDSTD_BUTT;

    if (rrec_fct->index_type == DMX_REC_INDEX_TYPE_VIDEO) {
        switch (rrec_fct->vcodec_type) {
            case DMX_VCODEC_TYPE_MPEG2: {
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 0, 0x00, 0x00, 0x00);
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 1, 0xb3, 0xb3, 0x00);
                vid_std = VIDSTD_MPEG2;
                break;
            }
            case DMX_VCODEC_TYPE_AVS: {
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 1, 0xb3, 0xb3, 0x00);
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 2, 0xb6, 0xb6, 0x00); /* filter id 2 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 3, 0xb1, 0xb0, 0x00); /* filter id 3 */
                vid_std = VIDSTD_AVS;
                break;
            }
            case DMX_VCODEC_TYPE_MPEG4: {
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 4, 0xb0, 0xb0, 0x00); /* filter id 4 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 5, 0xb6, 0xb5, 0x00); /* filter id 5 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 6, 0x2f, 0x00, 0x00); /* filter id 6 */
                vid_std = VIDSTD_MPEG4;
                break;
            }
            case DMX_VCODEC_TYPE_H264: {
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 7, 0x01, 0x01, 0x00); /* filter id 7 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 8, 0x05, 0x05, 0x00); /* filter id 8 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 9, 0x08, 0x07, 0x00); /* filter id 9 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 10, 0x11, 0x11, 0x00); /* filter id 10 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 11, 0x15, 0x15, 0x00); /* filter id 11 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 12, 0x18, 0x17, 0x00); /* filter id 12 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 13, 0x21, 0x21, 0x00); /* filter id 13 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 14, 0x25, 0x25, 0x00); /* filter id 14 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 15, 0x28, 0x27, 0x00); /* filter id 15 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 16, 0x78, 0x31, 0x00); /* filter id 16 */
                vid_std = VIDSTD_H264;
                break;
            }
            case DMX_VCODEC_TYPE_HEVC: {
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 17, 0x12, 0x0, 0x1);  /* [0x0 ~ 0x9] << 1, id 17 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 18, 0x2a, 0x20, 0x1); /* [0x10 ~ 0x15] << 1, id 18 */
                dmx_hal_set_scd_range_filter(rrec_fct->base.mgmt, 19, 0x50, 0x40, 0x1); /* [0x20 ~ 0x28] << 1, id 19 */
                vid_std = VIDSTD_HEVC;
                break;
            }
            default: {
                WARN(1, "Invalid vcodec type[%#x]!\n", rrec_fct->vcodec_type);
                ret = HI_ERR_DMX_INVALID_PARA;
                goto out;
            }
        }

        pic_parser = fidx_open_instance(vid_std, STRM_TYPE_ES, (hi_u32 *)&rrec_fct->last_frame_info);
        if (pic_parser < 0) {
            HI_ERR_DEMUX("pic_parser is invlid\n");
            ret = HI_ERR_DMX_NOFREE_CHAN;
            goto out;
        }
        rrec_fct->pic_parser = pic_parser;

        /* configure the index, disable tpit */
        dmx_hal_scd_set_rec_tab(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_FALSE, HI_TRUE, HI_TRUE);
        /* enable the filter */
        dmx_hal_scd_set_flt_en(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_TRUE);
    } else {
        /* configure the index, disable tpit, scd_es_long_en */
        dmx_hal_scd_set_rec_tab(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_FALSE, HI_TRUE, HI_FALSE);
    }

    /* open the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rrec_fct->scd_rbuf);
    ret = rrec_fct->scd_rbuf->ops->open(rrec_fct->scd_rbuf);
    dmx_r_put((struct dmx_r_base *)rrec_fct->scd_rbuf);

    dmx_hal_scd_set_buf_id(rrec_fct->base.mgmt, rrec_fct->scd_id, rrec_fct->scd_rbuf->base.id);

    /* enable the index */
    dmx_hal_scd_en(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_TRUE);

out:
    return ret;
}

static hi_s32 get_index_thread(hi_void *args);
static hi_s32 __dmx_r_recfct_scd_close(struct dmx_r_rec_fct *rrec_fct);
static hi_s32 _dmx_r_rec_fct_open_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    /* open the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rrec_fct->rbuf);
    ret = rrec_fct->rbuf->ops->open(rrec_fct->rbuf);
    dmx_r_put((struct dmx_r_base *)rrec_fct->rbuf);

    if (rrec_fct->rec_type == DMX_REC_TYPE_SELECT_PID) {
        /* Be sure to start the index and then start recording to prevent data misalignment. */
        if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_NONE) {
#if (DMX_REC_BUF_GAP_EXIST == 1)
            rrec_fct->last_scd_offset = 0;
            rrec_fct->ts_cnt_compensate = 0;
#endif
            ret = __dmx_r_recfct_scd_open(rrec_fct);
            if (ret != HI_SUCCESS) {
                HI_ERR_DEMUX("scd open failed!\n, scd_id[0x%x], rec_id[0x%x]", rrec_fct->scd_id, rrec_fct->base.id);
                goto out0;
            }
            rrec_fct->index_cnt = 0;
            rrec_fct->rec_index_thread = osal_kthread_create(get_index_thread, rrec_fct, "dmx_recindex%u", 0);
            if (IS_ERR(rrec_fct->rec_index_thread)) {
                HI_FATAL_DEMUX("kthread_create error!\n");
                ret = HI_FAILURE;
                goto out1;
            }
        }

        /* CRC is used to protect the transport_scrambing_ctrl bits be modified */
        if (rrec_fct->descramed == HI_TRUE && rrec_fct->crc_scb == DMX_TS_CRC_EN_SCB_EN) {
            /* support crc mode, when rrec_fct->descramed == HI_TRUE. */
            dmx_hal_scd_set_rec_chn_crc(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);

            dmx_hal_scd_set_rec_ctrl_mode(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
        } else if (rrec_fct->descramed == HI_TRUE && rrec_fct->crc_scb == DMX_TS_CRC_EN_SCB_DIS) {
            /* support crc mode, when rrec_fct->descramed == HI_TRUE. */
            dmx_hal_scd_set_rec_chn_crc(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
        }

        if (rrec_fct->rec_time_stamp >= DMX_REC_TIMESTAMP_ZERO) {
            dmx_hal_scd_set_tts_27m_en(rrec_fct->base.mgmt, HI_TRUE);
            dmx_hal_scd_set_rec_avpes_len_dis(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
            if (rrec_fct->rec_time_stamp == DMX_REC_TIMESTAMP_ZERO) {
                dmx_hal_scd_set_rec_avpes_cut_dis(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
                dmx_hal_scd_set_rec_avpes_drop_en(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
            } else if (rrec_fct->rec_time_stamp == DMX_REC_TIMESTAMP_HIGH32BIT_SCR) {
                dmx_hal_scd_set_rec_avpes_cut_dis(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
                dmx_hal_scd_set_rec_avpes_drop_en(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
            } else {
                dmx_hal_scd_set_rec_avpes_cut_dis(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
                dmx_hal_scd_set_rec_avpes_drop_en(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE);
            }
        }
        /* enable the ts rec */
        dmx_hal_scd_set_ts_rec_cfg(rrec_fct->base.mgmt, rrec_fct->base.id, HI_TRUE, rrec_fct->rbuf->base.id);
    } else {
        /* disable the rec ts */
        dmx_hal_scd_set_ts_rec_cfg(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE, 0);
        /* record all data */
        if (rrec_fct->rec_type == DMX_REC_TYPE_ALL_DATA && rrec_fct->rband != HI_NULL) {
            if (rrec_fct->rband->port_type == DMX_PORT_TSI_TYPE) {
                dmx_hal_dvb_port_set_dummy_force(rrec_fct->base.mgmt, rrec_fct->rband->port - DMX_TSI_PORT_0, HI_TRUE);
                dmx_hal_dvb_port_set_dummy_sync(rrec_fct->base.mgmt, rrec_fct->rband->port - DMX_TSI_PORT_0, HI_TRUE);
            }
        }
    }

    return ret;
out1:
    if (__dmx_r_recfct_scd_close(rrec_fct) != HI_SUCCESS) {
        HI_ERR_DEMUX("scd close failed!\n");
    }
out0:
    rrec_fct->rbuf->ops->close(rrec_fct->rbuf);
    return ret;
}

static hi_s32 dmx_r_rec_fct_open_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_open_impl(rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_get_attrs_impl(struct dmx_r_rec_fct *rrec_fct, dmx_rec_attrs *attrs)
{
    hi_s32 ret;

    struct dmx_buf_attrs buf_attrs;
    struct dmx_r_buf       *rbuf = rrec_fct->rbuf;

    /* get buffer attrs */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_attrs(rbuf, &buf_attrs);
    if (ret == HI_SUCCESS) {
        attrs->rec_buf_size = buf_attrs.buf_size;
        attrs->secure_mode  = buf_attrs.secure_mode;
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    attrs->descramed   = rrec_fct->descramed;
    attrs->index_src_pid = rrec_fct->index_src_pid;
    attrs->index_type   = rrec_fct->index_type;
    attrs->rec_type     = rrec_fct->rec_type;
    attrs->vcodec_type  = rrec_fct->vcodec_type;

    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rec_fct_get_attrs_impl(struct dmx_r_rec_fct *rrec_fct, dmx_rec_attrs *attrs)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_get_attrs_impl(rrec_fct, attrs);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 dmx_r_rec_fct_set_attrs_impl(struct dmx_r_rec_fct *rrec_fct, const dmx_rec_attrs *attrs)
{
    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    HI_ERR_DEMUX("rec_fct not support setattrs yet!\n");

    osal_mutex_unlock(&rrec_fct->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 _dmx_r_rec_fct_get_status_impl(struct dmx_r_rec_fct *rrec_fct, dmx_rec_status *status)
{
    hi_s32 ret;

    struct dmx_buf_status  buf_status;
    struct dmx_r_buf       *rbuf = rrec_fct->rbuf;

    /* get buffer attrs */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_status(rbuf, &buf_status);
    if (ret == HI_SUCCESS) {
        status->buf_size  = buf_status.buf_size;
        status->used_size = dmx_get_queue_lenth(buf_status.hw_read, buf_status.hw_write, buf_status.buf_size);
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    /* todo something about hardware */
    ret = HI_SUCCESS;

    return ret;
}

static hi_s32 dmx_r_rec_fct_get_status_impl(struct dmx_r_rec_fct *rrec_fct, dmx_rec_status *status)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_get_status_impl(rrec_fct, status);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_add_ch_impl(struct dmx_r_rec_fct *rrec_fct, struct dmx_r_base *obj)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_band  *rband  = HI_NULL;
    struct dmx_r_buf   *rbuf = rrec_fct->rbuf;
    hi_u32 i;

    /* get the band or pid_ch obj */
    dmx_r_get_raw(obj);

    if (IS_BAND(obj)) {
        rband = (struct dmx_r_band *)obj;
        DMX_NULL_POINTER_GOTO(rband, out);
        rrec_fct->rband = rband;
        ret = rrec_fct->rband->ops->attach_rec(rrec_fct->rband, rrec_fct);
        if (ret != HI_SUCCESS) {
            rrec_fct->rband = HI_NULL;
            HI_ERR_DEMUX("rec attach band failed!\n");
            goto out;
        }
    } else if (IS_PIDCH(obj)) {
        rpid_ch = (struct dmx_r_pid_ch *)obj;
        DMX_NULL_POINTER_GOTO(rpid_ch, out);
        rband = rpid_ch->rband;
        DMX_NULL_POINTER_GOTO(rband, out);

        osal_mutex_lock(&rrec_fct->pid_ch_list_lock);
        for_each_set_bit(i, rrec_fct->pid_bit_index, REC_MAX_PID_CNT) {
            if (rrec_fct->pid_chan_array[i] == rpid_ch) {
                osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);
                HI_ERR_DEMUX("pid channel[%x] has already added!\n", rpid_ch->pid_ch_handle);
                ret = HI_ERR_DMX_OCCUPIED_PID;
                goto out;
            }
        }

        i = find_first_zero_bit(rrec_fct->pid_bit_index, REC_MAX_PID_CNT);
        if (i >= REC_MAX_PID_CNT) {
            osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);
            HI_ERR_DEMUX("rec(%x) already add %u pids.\n", rrec_fct->rec_fct_handle, REC_MAX_PID_CNT);
            ret = HI_ERR_DMX_NO_RESOURCE;
            goto out;
        }
        osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);

        /* call pidch ops attach */
        ret = rpid_ch->ops->attach(rpid_ch, (struct dmx_r_base *)rrec_fct);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("rec attach pid channel failed!\n");
            goto out;
        }

        osal_mutex_lock(&rrec_fct->pid_ch_list_lock);
        rrec_fct->pid_chan_array[i] = rpid_ch;
        set_bit(i, rrec_fct->pid_bit_index);
        osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);
    } else {
        HI_ERR_DEMUX("invalid obj, not band or pidch\n");
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    /* get the buf obj */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    rbuf->ops->attach(rbuf, rband->port);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return HI_SUCCESS;

out:
    /* put the band or pid_ch obj */
    dmx_r_put(obj);
    return ret;
}

static hi_s32 dmx_r_rec_fct_add_ch_impl(struct dmx_r_rec_fct *rrec_fct, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_add_ch_impl(rrec_fct, obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_del_ch_impl(struct dmx_r_rec_fct *rrec_fct, struct dmx_r_base *obj)
{
    hi_s32 ret = HI_FAILURE;
    struct dmx_r_pid_ch *rpid_ch = HI_NULL;
    struct dmx_r_band *rband = HI_NULL;

    if (IS_BAND(obj)) {
        rband = (struct dmx_r_band *)obj;
        ret = rband->ops->detach_rec(rband, rrec_fct);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("rec detach band failed!\n");
            goto out;
        }

        /* prepare to clean the channel in band */
        dmx_hal_band_clear_chan(rrec_fct->base.mgmt, rband->base.id);
        rrec_fct->rband = HI_NULL;

        /* put the band or pid_ch obj */
        dmx_r_put(obj);
    } else if (IS_PIDCH(obj)) {
        hi_u32 i;
        rpid_ch = (struct dmx_r_pid_ch *)obj;
        DMX_NULL_POINTER_GOTO(rpid_ch, out);

        i = find_first_bit(rrec_fct->pid_bit_index, REC_MAX_PID_CNT);
        if (i >= REC_MAX_PID_CNT) {
            HI_ERR_DEMUX("The pid array is empty of record!\n");
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        for_each_set_bit(i, rrec_fct->pid_bit_index, REC_MAX_PID_CNT) {
            if (rrec_fct->pid_chan_array[i] == rpid_ch) {
                /* detach the pidch from record channle */
                ret = rpid_ch->ops->detach(rpid_ch, (struct dmx_r_base *)rrec_fct);
                if (ret != HI_SUCCESS) {
                    HI_ERR_DEMUX("rec detach pid channel failed!\n");
                    goto out;
                }

                clear_bit(i, rrec_fct->pid_bit_index);
                rrec_fct->pid_chan_array[i] = HI_NULL;

                /* prepare to clean the channel in band */
                if (find_next_bit(rrec_fct->pid_bit_index, REC_MAX_PID_CNT, i + 1) >= REC_MAX_PID_CNT) {
                    dmx_hal_band_clear_chan(rrec_fct->base.mgmt, rpid_ch->rband->base.id);
                }

                /* put the band or pid_ch obj */
                dmx_r_put(obj);
            }
        }
    } else {
        HI_ERR_DEMUX("invalid obj, not band or pidch\n");
        ret = HI_ERR_DMX_INVALID_PARA;
        goto out;
    }

    return HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_rec_fct_del_ch_impl(struct dmx_r_rec_fct *rrec_fct, struct dmx_r_base *obj)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);
    osal_mutex_lock(&rrec_fct->pid_ch_list_lock);
    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_del_ch_impl(rrec_fct, obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_del_all_ch_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;
    hi_u32 i;

    osal_mutex_lock(&rrec_fct->pid_ch_list_lock);
    for_each_set_bit(i, rrec_fct->pid_bit_index, REC_MAX_PID_CNT) {
        struct dmx_r_pid_ch *rpid_ch = (struct dmx_r_pid_ch *)rrec_fct->pid_chan_array[i];
        /* detach the pidch from record channle */
        ret = _dmx_r_rec_fct_del_ch_impl(rrec_fct, (struct dmx_r_base *)rpid_ch);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("delete pid[0x%x] from record chan failed!\n", rpid_ch->pid);
            osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);
            goto out;
        }
    }

    osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 dmx_r_rec_fct_del_all_ch_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    /* detach all the pidch from record channle */
    ret = _dmx_r_rec_fct_del_all_ch_impl(rrec_fct);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("delete the pid from record chan failed!\n");
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_acquire_buf_impl(struct dmx_r_rec_fct *rrec_fct, hi_u32 req_len, hi_u32 time_out,
        dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;
    hi_u32 req_len_align;

    struct dmx_r_buf *rbuf = rrec_fct->rbuf;

    DMX_NULL_POINTER_RETURN(rrec_fct);
    DMX_NULL_POINTER_RETURN(rbuf);

    req_len_align = (rrec_fct->rec_time_stamp >= DMX_REC_TIMESTAMP_ZERO) ? DMX_TTS_PKT_SIZE : DMX_TS_PKT_SIZE;
    /* requesting length must be aligned with ts packet size */
    req_len = req_len - (req_len % req_len_align);

    /* get the buf obj */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->acquire_buf(rbuf, req_len, time_out, HI_NULL, rec_fct_buf);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_rec_fct_acquire_buf_impl(struct dmx_r_rec_fct *rrec_fct, hi_u32 req_len, hi_u32 time_out,
        dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_acquire_buf_impl(rrec_fct, req_len, time_out, rec_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_release_buf_impl(struct dmx_r_rec_fct *rrec_fct, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;
    hi_u32 i;

    struct dmx_mgmt        *mgmt = rrec_fct->base.mgmt;
    struct dmx_r_buf       *rbuf = rrec_fct->rbuf;
    struct dmx_r_pid_ch     *rpid_ch = HI_NULL;

    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(rbuf);

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->release_buf(rbuf, 1, rec_fct_buf);
    if (ret == HI_SUCCESS) {
        osal_mutex_lock(&rrec_fct->pid_ch_list_lock);
        for_each_set_bit(i, rrec_fct->pid_bit_index, REC_MAX_PID_CNT) {
            rpid_ch = (struct dmx_r_pid_ch *)rrec_fct->pid_chan_array[i];
            /* check the pc buffer ap status */
            dmx_cls_pcbuf_ap_status(mgmt, rpid_ch);
        }
        osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);
    }
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_rec_fct_release_buf_impl(struct dmx_r_rec_fct *rrec_fct, dmx_buffer *rec_fct_buf)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_release_buf_impl(rrec_fct, rec_fct_buf);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

/*
 * reference linux kernel definition(cnt32_to_63.h).
 */
static hi_u32 dmx_get_scr_mono_ms(struct dmx_r_rec_fct *rrec_fct, const volatile dmx_index_scd *sc_data)
{
    union cnt32_to_63 {
        struct {
            hi_u32 lo, hi;
        };
        hi_u64 val;
    } __x;
    hi_u64 cur_scr_cnt;

    __x.hi = rrec_fct->cnt32to63helper;
    osal_smp_rmb();
    __x.lo = sc_data->scr_clk;

    if (unlikely((hi_s32)(__x.hi ^ __x.lo) < 0)) {
        rrec_fct->cnt32to63helper = __x.hi = (__x.hi ^ 0x80000000) + (__x.hi >> 31); /* 31 bits */
    } else if (unlikely(sc_data->scr_clk < (rrec_fct->prev_scr_clk & 0x00000000FFFFFFFF))) {
        if (((sc_data->scr_clk >> 31) & 0x1) == 0) { /* 31 bits */
            __x.hi = (__x.hi & 0x7FFFFFFF);
        } else {
            __x.hi = (__x.hi | 0x80000000);
        }
        __x.hi = __x.hi + 1;
        rrec_fct->cnt32to63helper = __x.hi;
    }

    cur_scr_cnt =  __x.val & 0x7fffffffffffffffULL;

    /*
     * we found that both maybe same when signal is weak.
     */
    if (unlikely(!(rrec_fct->prev_scr_clk <= cur_scr_cnt))) {
        WARN(1, "prev_scr_clk(0x%llx) should less or equal than cur_scr_cnt(0x%llx).\n", rrec_fct->prev_scr_clk,
             cur_scr_cnt);
    }

    rrec_fct->prev_scr_clk = cur_scr_cnt;

    /* trans 90khz count to ms. */
    do_div(cur_scr_cnt, 90);

    return (hi_u32)cur_scr_cnt;
}

static hi_s32 dmx_format_secure_hevc_index(struct dmx_r_rec_fct *rrec_fct, const findex_scd *fidx)
{
    hi_s32 ret;

    ret = dmx_fixup_secure_hevc_index(rrec_fct, fidx);
    if (ret != HI_SUCCESS) {
        goto out;
    }

out:
    return ret;
}

static hi_s32 dmx_format_hevc_index(struct dmx_r_rec_fct *rrec_fct, findex_scd *fidx)
{
    hi_s32 ret;

    fidx->extra_scdata_size = sizeof(hi_u8) * HEVC_DUP_DATA_TOTAL_LEN;
    fidx->extra_real_scdata_size = sizeof(hi_u8) * HEVC_DUP_DATA_TOTAL_LEN;
    fidx->extra_scdata = HI_KZALLOC(HI_ID_DEMUX, fidx->extra_scdata_size, GFP_KERNEL);
    if (fidx->extra_scdata == HI_NULL) {
        HI_ERR_DEMUX("create hevc index extra buffer failed.\n");
        ret = HI_FAILURE;
        goto out0;
    }

    ret = dmx_fixup_hevc_index(rrec_fct, fidx);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    fidx_feed_start_code(rrec_fct->pic_parser, fidx);

out1:
    HI_KFREE(HI_ID_DEMUX, fidx->extra_scdata);
    fidx->extra_scdata = HI_NULL;
out0:
    return ret;
}

static hi_s32 dmx_format_vid_index(struct dmx_r_rec_fct *rrec_fct, findex_scd *fidx)
{
    hi_s32 ret;
    dmx_index_data *curr_frame = &rrec_fct->last_frame_info;
    hi_u64 last_frame_offset = curr_frame->global_offset;

    osal_mb();

    if (rrec_fct->vcodec_type == DMX_VCODEC_TYPE_HEVC) {
        if (rrec_fct->secure_mode == DMX_SECURE_TEE) {
            ret = dmx_format_secure_hevc_index(rrec_fct, fidx);
            if (ret != HI_SUCCESS) {
                goto out;
            }
        } else {
            ret = dmx_format_hevc_index(rrec_fct, fidx);
            if (ret != HI_SUCCESS) {
                goto out;
            }
        }
    } else {
        fidx_feed_start_code(rrec_fct->pic_parser, fidx);
    }

    /* curr_frame will be update after fidx_feed_start_code */
    ret = last_frame_offset < curr_frame->global_offset ? HI_SUCCESS : HI_FAILURE;

out:
    return ret;
}

static hi_s32 parse_video_frame_index(struct dmx_r_rec_fct *rrec_fct, hi_u32 scd_num,
    const dmx_index_scd *scd_data, dmx_index_data *index, hi_u32 *used_scd_num)
{
    hi_s32 ret;
    hi_u32 i = 0;
    hi_bool get_valid_index = HI_FALSE;
    hi_u32 cur_frame_ms = 0;
    findex_scd es_scd = {0};
    dmx_index_data tmp_index;
    dmx_index_data *curr_frame   = &rrec_fct->last_frame_info;

    for (i = 0; i < scd_num; i++) {
        if (dmx_scd_to_video_index(rrec_fct, scd_data, &es_scd) == HI_SUCCESS) {
            if (dmx_format_vid_index(rrec_fct, &es_scd) == HI_SUCCESS) {
                cur_frame_ms = dmx_get_scr_mono_ms(rrec_fct, scd_data);
                memcpy(&tmp_index, curr_frame, sizeof(dmx_index_data));
                get_valid_index = HI_TRUE;
            }
        }

        scd_data++;

        if (get_valid_index == HI_TRUE) {
            if (unlikely(rrec_fct->first_frame_ms == 0)) {
                rrec_fct->first_frame_ms = cur_frame_ms;
            }

            /* for some short frame, the scr clock(90khz) maybe same, the frame add 1 for keeping unique timestamp. */
            if (unlikely(cur_frame_ms <= rrec_fct->prev_frame_ms)) {
                HI_DBG_DEMUX("prev_frame_ms:0x%x, cur_frame_ms:0x%x, offset:0x%llx, frame_size:%d.\n",
                             rrec_fct->prev_frame_ms, cur_frame_ms, tmp_index.global_offset, tmp_index.frame_size);
                cur_frame_ms = rrec_fct->prev_frame_ms + 1;
            }

            rrec_fct->prev_frame_ms = cur_frame_ms;

            tmp_index.data_time_ms = cur_frame_ms - rrec_fct->first_frame_ms;
            memcpy(&index[0], &tmp_index, sizeof(dmx_index_data));
            break;
        }
    }

    if (i >= scd_num) {
        *used_scd_num = scd_num;
    } else {
        *used_scd_num = i + 1;
    }

    if (get_valid_index != HI_TRUE) {
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 parse_audio_frame_index(struct dmx_r_rec_fct *rrec_fct, hi_u32 scd_num,
    const dmx_index_scd *scd_data, dmx_index_data *index, hi_u32 *used_scd_num)
{
    hi_s32 ret;
    hi_u32 i = 0;
    hi_bool get_valid_index = HI_FALSE;
    hi_u32  cur_frame_ms  = 0;
    dmx_index_data tmp_index;
    dmx_index_data this_frame = {0};

    for (i = 0; i < scd_num; i++) {
        if (dmx_scd_to_audio_index(&this_frame, scd_data) == HI_SUCCESS) {
            /* only for the first scd (index) will enter this condition */
            if (unlikely(rrec_fct->last_frame_info.data_time_ms == 0)) {
                memcpy(&rrec_fct->last_frame_info, &this_frame, sizeof(dmx_index_data));
            } else {
                cur_frame_ms = dmx_get_scr_mono_ms(rrec_fct, scd_data);

                rrec_fct->last_frame_info.frame_size = this_frame.global_offset -
                    rrec_fct->last_frame_info.global_offset;
                memcpy(&tmp_index, &rrec_fct->last_frame_info, sizeof(dmx_index_data));
                memcpy(&rrec_fct->last_frame_info, &this_frame, sizeof(dmx_index_data));

                get_valid_index = HI_TRUE;
            }
        }

        scd_data++;

        if (get_valid_index == HI_TRUE) {
            if (unlikely(rrec_fct->first_frame_ms == 0)) {
                rrec_fct->first_frame_ms = cur_frame_ms;
            }

            /*
             * for some short frame, the scr clock(90khz) count maybe same,
             * the frame time stamp plus 1 for keeping uniqueness of the time stamp.
             */
            if (unlikely(cur_frame_ms <= rrec_fct->prev_frame_ms)) {
                HI_DBG_DEMUX("prev_frame_ms:0x%x, cur_frame_ms:0x%x, offset:0x%llx, frame_size:%d.\n",
                             rrec_fct->prev_frame_ms, cur_frame_ms, tmp_index.global_offset, tmp_index.frame_size);

                cur_frame_ms = rrec_fct->prev_frame_ms + 1;
            }

            rrec_fct->prev_frame_ms = cur_frame_ms;

            tmp_index.data_time_ms = cur_frame_ms - rrec_fct->first_frame_ms;
            memcpy(&index[0], &tmp_index, sizeof(dmx_index_data));
            break;
        }
    }

    if (i >= scd_num) {
        *used_scd_num = scd_num;
    } else {
        *used_scd_num = i + 1;
    }

    if (get_valid_index != HI_TRUE) {
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 __dmx_r_rec_recv_idx(struct dmx_r_rec_fct *rrec_fct, hi_u32 time_out, dmx_index_data *index)
{
    hi_s32 ret;
    hi_u32 scd_num = 0;
    hi_u32 rel_scd_num = 0;
    hi_u32 i = 0;
    hi_bool get_valid_index = HI_FALSE;

    struct dmx_mgmt        *mgmt = rrec_fct->base.mgmt;
    struct dmx_r_buf       *scd_rbuf = rrec_fct->scd_rbuf;
    struct dmx_r_pid_ch    *rpid_ch = HI_NULL;

    dmx_index_scd *scd_data = HI_NULL;
    DMX_NULL_POINTER_RETURN(mgmt);
    DMX_NULL_POINTER_RETURN(scd_rbuf);

    /* first step: get the scd data */
    dmx_r_get_raw((struct dmx_r_base *)scd_rbuf);
    ret = scd_rbuf->ops->acquire_scd(scd_rbuf, DMX_MAX_SCD_CNT, time_out, &scd_num, &scd_data);
    if (ret != HI_SUCCESS) {
        HI_WARN_DEMUX("scd buf acquire failed!\n");
        goto out;
    }

    DMX_FATAL_CON_GOTO(scd_data == HI_NULL, HI_ERR_DMX_NULL_PTR, out);

    if (rrec_fct->index_type == DMX_REC_INDEX_TYPE_VIDEO) {
        ret = parse_video_frame_index(rrec_fct, scd_num, scd_data, index, &rel_scd_num);
        if (ret != HI_SUCCESS) {
            HI_WARN_DEMUX("parse vide frame index failed!\n");
            get_valid_index = HI_FALSE;
        } else {
            get_valid_index = HI_TRUE;
        }
    } else {
        ret = parse_audio_frame_index(rrec_fct, scd_num, scd_data, index, &rel_scd_num);
        if (ret != HI_SUCCESS) {
            HI_WARN_DEMUX("parse vide frame index failed!\n");
            get_valid_index = HI_FALSE;
        } else {
            get_valid_index = HI_TRUE;
        }
    }

    ret = scd_rbuf->ops->release_scd(scd_rbuf, rel_scd_num, scd_data);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("scd buf release failed!\n");
        goto out;
    }

    osal_mutex_lock(&rrec_fct->pid_ch_list_lock);
    for_each_set_bit(i, rrec_fct->pid_bit_index, REC_MAX_PID_CNT) {
        rpid_ch = (struct dmx_r_pid_ch *)rrec_fct->pid_chan_array[i];
        /* check the pc buffer ap status */
        dmx_cls_pcbuf_ap_status(mgmt, rpid_ch);
    }
    osal_mutex_unlock(&rrec_fct->pid_ch_list_lock);

    if (get_valid_index != HI_TRUE) {
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }

    ret = HI_SUCCESS;
out:

    dmx_r_put((struct dmx_r_base *)scd_rbuf);
    return ret;
}

static hi_void clear_rec_index_list(struct dmx_r_rec_fct *rrecfct)
{
    dmx_index_helper *entry = HI_NULL;
    dmx_index_helper *tmp = HI_NULL;

    /* clear rec data&idx */
    rrecfct->index_cnt = 0;
    osal_mutex_lock(&rrecfct->index_list_lock);
    list_for_each_entry_safe(entry, tmp, &rrecfct->index_list_head, list) {
        list_del(&entry->list);
        HI_KFREE(HI_ID_DEMUX, entry);
    }
    osal_mutex_unlock(&rrecfct->index_list_lock);
}

/* 10 frame per schedule time */
#define DMX_INDEX_NUM_PER_SCHEDULE          10
/* cache osal_sem_up to 2000 rec index */
#define DMX_INDEX_MAX_CACHE_NUM             2000
/* this kernel thread for record index interface */
static hi_s32 get_index_thread(hi_void *args)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 dmx_idx_cnt = 0;
    struct dmx_r_rec_fct *rrecfct = HI_NULL;
    dmx_index_helper *helper = HI_NULL;
    dmx_index_data *new_index = HI_NULL;
    struct dmx_r_buf *rscd_buf = HI_NULL;

    DMX_NULL_POINTER_RETURN(args);
    rrecfct = (struct dmx_r_rec_fct *)args;

    /* for check SCD index interrupt */
    rscd_buf  = rrecfct->scd_rbuf;
    DMX_NULL_POINTER_RETURN(rscd_buf);

    while (!kthread_should_stop()) {
        if (rrecfct->index_cnt >= DMX_INDEX_MAX_CACHE_NUM) {
            goto out;
        }

        helper = HI_KZALLOC(HI_ID_DEMUX, sizeof(dmx_index_helper), GFP_KERNEL);
        if (unlikely(!helper)) {
            HI_ERR_DEMUX("malloc rec data&idx helper failed.\n");
            goto out;
        }

        memset(helper, 0, sizeof(dmx_index_helper));

        INIT_LIST_HEAD(&helper->list);
        new_index = &helper->index;

        /* timeout 1000ms */
        ret = __dmx_r_rec_recv_idx(rrecfct, 1000, new_index);
        if (ret == HI_SUCCESS) {
            osal_mutex_lock(&rrecfct->index_list_lock);
            list_add_tail(&helper->list, &rrecfct->index_list_head);
            rrecfct->index_cnt++;
            osal_mutex_unlock(&rrecfct->index_list_lock);

            /* if always has index data, process 20 frames per shedule time */
            if (dmx_idx_cnt++ < DMX_INDEX_NUM_PER_SCHEDULE) {
                continue;
            }
        } else {
            if (likely(helper)) {
                HI_KFREE(HI_ID_DEMUX, helper);
                helper = HI_NULL;
            }
        }
out:
        /* schedule cycle 10ms */
        (hi_void)wait_event_interruptible_timeout(rscd_buf->wait_queue, rscd_buf->wait_cond, osal_msecs_to_jiffies(10));
        dmx_idx_cnt = 0;
        osal_mutex_lock(&rscd_buf->interrupt_lock);
        rscd_buf->wait_cond = HI_FALSE;
        osal_mutex_unlock(&rscd_buf->interrupt_lock);

        if (rscd_buf->ovfl_flag == HI_TRUE) {
            clear_rec_index_list(rrecfct);
            rscd_buf->ovfl_flag = HI_FALSE;
        }

        yield();
    }
    return ret;
}

static hi_s32 _dmx_r_rec_fct_recv_idx_impl(struct dmx_r_rec_fct *rrec_fct, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_data *index)
{
    hi_s32 ret;
    hi_u32 i = 0;
    hi_u32 index_num = 0;

    struct dmx_mgmt *mgmt = rrec_fct->base.mgmt;
    DMX_NULL_POINTER_RETURN(mgmt);

    /* index list is not empty */
    osal_mutex_lock(&rrec_fct->index_list_lock);
    if (!list_empty(&rrec_fct->index_list_head)) {
        if (req_num <= rrec_fct->index_cnt) {
            index_num = req_num;
            rrec_fct->index_cnt -= index_num;
        } else {
            index_num = rrec_fct->index_cnt;
            rrec_fct->index_cnt = 0x0;
        }

        for (i = 0; i < index_num; i++) {
            dmx_index_helper *entry = list_first_entry(&rrec_fct->index_list_head, dmx_index_helper, list);
            index[i] = entry->index;
            list_del(&entry->list);
            HI_KFREE(HI_ID_DEMUX, entry);
        }
    } else {
        *reqed_num = 0;
        osal_mutex_unlock(&rrec_fct->index_list_lock);
        HI_WARN_DEMUX("there is no valid frame index!\n");
        ret = HI_ERR_DMX_NOAVAILABLE_DATA;
        goto out;
    }
    osal_mutex_unlock(&rrec_fct->index_list_lock);
    *reqed_num = index_num;
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_rec_fct_recv_idx_impl(struct dmx_r_rec_fct *rrec_fct, hi_u32 req_num, hi_u32 time_out,
    hi_u32 *reqed_num, dmx_index_data *index)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_recv_idx_impl(rrec_fct, req_num, time_out, reqed_num, index);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_peek_idx_buf_impl(struct dmx_r_rec_fct *rrec_fct, hi_u32 time_out, hi_u32 *index_num,
    hi_u32 *rec_data_len)
{
    hi_s32 ret;
    struct dmx_buf_status buf_status = {0};

    struct dmx_r_buf *rbuf = rrec_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    osal_mutex_lock(&rrec_fct->index_list_lock);
    *index_num = rrec_fct->index_cnt;
    osal_mutex_unlock(&rrec_fct->index_list_lock);

    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->get_status(rbuf, &buf_status);
    dmx_r_put((struct dmx_r_base *)rbuf);
    if (ret == HI_SUCCESS) {
        *rec_data_len = buf_status.buf_used_size;
    }

    return ret;
}

static hi_s32 dmx_r_rec_fct_peek_idx_buf_impl(struct dmx_r_rec_fct *rrec_fct, hi_u32 time_out, hi_u32 *index_num,
    hi_u32 *rec_data_len)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_peek_idx_buf_impl(rrec_fct, time_out, index_num, rec_data_len);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 __dmx_r_recfct_scd_close(struct dmx_r_rec_fct *rrec_fct)
{
    /* close the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rrec_fct->scd_rbuf);
    rrec_fct->scd_rbuf->ops->close(rrec_fct->scd_rbuf);
    dmx_r_put((struct dmx_r_base *)rrec_fct->scd_rbuf);

    /* disable the rec index */
    dmx_hal_scd_en(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_FALSE);
    dmx_hal_scd_set_rec_tab(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_FALSE, HI_FALSE, HI_FALSE);
    dmx_hal_scd_set_buf_id(rrec_fct->base.mgmt, rrec_fct->scd_id, 0);

    /* disable the filter tmp */
    dmx_hal_scd_set_flt_en(rrec_fct->base.mgmt, rrec_fct->scd_id, HI_FALSE);

    /* close the index instance */
    fidx_close_instance(rrec_fct->pic_parser);

    return HI_SUCCESS;
}

static hi_s32 _dmx_r_rec_fct_close_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret = HI_FAILURE;

    /* disable the rec ts */
    dmx_hal_scd_set_ts_rec_cfg(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE, 0);

    /* disable the CRC and SCB */
    if (rrec_fct->descramed == HI_TRUE && rrec_fct->crc_scb == DMX_TS_CRC_EN_SCB_EN) {
        /* support crc mode, when rrec_fct->descramed == HI_TRUE. */
        dmx_hal_scd_set_rec_chn_crc(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);

        dmx_hal_scd_set_rec_ctrl_mode(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
    } else if (rrec_fct->descramed == HI_TRUE && rrec_fct->crc_scb == DMX_TS_CRC_EN_SCB_DIS) {
        /* support crc mode, when rrec_fct->descramed == HI_TRUE. */
        dmx_hal_scd_set_rec_chn_crc(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
    }

    if (rrec_fct->rec_time_stamp >= DMX_REC_TIMESTAMP_ZERO) {
        dmx_hal_scd_set_rec_avpes_len_dis(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
        dmx_hal_scd_set_rec_avpes_cut_dis(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
        dmx_hal_scd_set_rec_avpes_drop_en(rrec_fct->base.mgmt, rrec_fct->base.id, HI_FALSE);
        dmx_hal_scd_set_tts_27m_en(rrec_fct->base.mgmt, HI_FALSE);
    }

    /* close the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rrec_fct->rbuf);
    rrec_fct->rbuf->ops->close(rrec_fct->rbuf);
    dmx_r_put((struct dmx_r_base *)rrec_fct->rbuf);

    /* set the index */
    if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_NONE) {
        if (likely(rrec_fct->rec_index_thread)) {
            osal_kthread_destroy(rrec_fct->rec_index_thread, HI_TRUE);
            rrec_fct->rec_index_thread = HI_NULL;
            HI_DBG_DEMUX("Stop the RecIndexThread\n");
        }

        clear_rec_index_list(rrec_fct);

        ret = __dmx_r_recfct_scd_close(rrec_fct);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("scd close failed!\n, scd_id[0x%x], rec_id[0x%x]", rrec_fct->scd_id, rrec_fct->base.id);
            goto out;
        }
    }

    /* record all data */
    if (rrec_fct->rec_type == DMX_REC_TYPE_ALL_DATA && rrec_fct->rband != HI_NULL) {
        if (rrec_fct->rband->port_type == DMX_PORT_TSI_TYPE) {
            dmx_hal_dvb_port_set_dummy_sync(rrec_fct->base.mgmt, rrec_fct->rband->port - DMX_TSI_PORT_0, HI_FALSE);
            dmx_hal_dvb_port_set_dummy_force(rrec_fct->base.mgmt, rrec_fct->rband->port - DMX_TSI_PORT_0, HI_FALSE);
        }
    }

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_rec_fct_close_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_close_impl(rrec_fct);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = HI_SUCCESS;

out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_destroy_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_u32 ret = HI_FAILURE;

    unsigned long mask = 0;
    unsigned long *p = HI_NULL;

    struct dmx_mgmt   *mgmt = rrec_fct->base.mgmt;

    /* do something about hardware */
    DMX_NULL_POINTER_RETURN(rrec_fct);
    DMX_NULL_POINTER_RETURN(rrec_fct->rbuf);
    DMX_NULL_POINTER_RETURN(mgmt);

    /* post desroy begin */
    _dmx_r_rec_fct_close_impl(rrec_fct);

    /* all ts record */
    if (unlikely(rrec_fct->rband != HI_NULL)) {
        _dmx_r_rec_fct_del_ch_impl(rrec_fct, (struct dmx_r_base *)rrec_fct->rband);
    }

    /* select pid record */
    _dmx_r_rec_fct_del_all_ch_impl(rrec_fct);

    /* clar ts rec channel */
    dmx_hal_scd_clear_chan(mgmt, rrec_fct->base.id, HI_FALSE);

    if (rrec_fct->index_type != DMX_REC_INDEX_TYPE_NONE) {
        DMX_NULL_POINTER_RETURN(rrec_fct->scd_rbuf);

        /* clear the scd index id */
        dmx_hal_scd_clear_chan(mgmt, rrec_fct->scd_id, HI_TRUE);

        /* deinit the buffer */
        dmx_r_get_raw((struct dmx_r_base *)rrec_fct->scd_rbuf);
        ret = rrec_fct->scd_rbuf->ops->deinit(rrec_fct->scd_rbuf);
        dmx_r_put((struct dmx_r_base *)rrec_fct->scd_rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("scd buf destroy failed!\n");
            goto out;
        }
        /* destroy the buffer */
        ret = dmx_mgmt_destroy_buf((struct dmx_r_base *)rrec_fct->scd_rbuf);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("destroy scd buf failed!\n");
            goto out;
        }

        rrec_fct->scd_rbuf = HI_NULL;

        /* clear the bitmap */
        mask = BIT_MASK(rrec_fct->scd_id);
        osal_mutex_lock(&mgmt->scd_chan_lock);
        p = ((unsigned long *)mgmt->scd_bitmap) + BIT_WORD(rrec_fct->scd_id);
        if (!(*p & mask)) {
            osal_mutex_unlock(&mgmt->scd_chan_lock);
            HI_ERR_DEMUX("scd channel(%d) is invalid.\n", rrec_fct->scd_id);
            ret = HI_ERR_DMX_INVALID_PARA;
            goto out;
        }

        clear_bit(rrec_fct->scd_id, mgmt->scd_bitmap);
        osal_mutex_unlock(&mgmt->scd_chan_lock);
    }

    if (rrec_fct->rbuf->secure_mode == DMX_SECURE_TEE) {
        dmx_tee_mem_info rec_mem_info = {0};
        rec_mem_info.buf_id = rrec_fct->rbuf->base.id;
        rec_mem_info.buf_phy_addr = rrec_fct->rbuf->buf_phy_addr;
        rec_mem_info.buf_size = rrec_fct->rbuf->real_buf_size;

        dmx_tee_destroy_rec_chan(rrec_fct->base.id, &rec_mem_info);
    }

    /* deinit the buffer */
    dmx_r_get_raw((struct dmx_r_base *)rrec_fct->rbuf);
    ret = rrec_fct->rbuf->ops->deinit(rrec_fct->rbuf);
    dmx_r_put((struct dmx_r_base *)rrec_fct->rbuf);

    /* destroy the buffer */
    ret = dmx_mgmt_destroy_buf((struct dmx_r_base *)rrec_fct->rbuf);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("destroy recfct buf failed!\n");
        goto out;
    }

    rrec_fct->rbuf = HI_NULL;

    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 dmx_r_rec_fct_destroy_impl(struct dmx_r_rec_fct *rrec_fct)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_destroy_impl(rrec_fct);
    if (ret == HI_SUCCESS) {
        rrec_fct->staled = HI_TRUE;
    }

    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_pre_mmap_impl(struct dmx_r_rec_fct *rrec_fct, hi_s64 *buf_handle, hi_u32 *buf_size,
                                           hi_void **buf_usr_addr)
{
    hi_s32 ret;
    struct dmx_r_buf       *rbuf = rrec_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* pre_map buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->pre_mmap(rbuf, buf_handle, buf_size, buf_usr_addr);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_rec_fct_pre_mmap_impl(struct dmx_r_rec_fct *rrec_fct, hi_s64 *buf_handle, hi_u32 *buf_size,
                                          hi_void **buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_pre_mmap_impl(rrec_fct, buf_handle, buf_size, buf_usr_addr);

    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 _dmx_r_rec_fct_pst_mmap_impl(struct dmx_r_rec_fct *rrect_fct, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    struct dmx_r_buf       *rbuf = rrect_fct->rbuf;
    DMX_NULL_POINTER_RETURN(rbuf);

    /* pst_map buffer */
    dmx_r_get_raw((struct dmx_r_base *)rbuf);
    ret = rbuf->ops->pst_mmap(rbuf, buf_usr_addr);
    dmx_r_put((struct dmx_r_base *)rbuf);

    return ret;
}

static hi_s32 dmx_r_rec_fct_pst_mmap_impl(struct dmx_r_rec_fct *rrec_fct, hi_void *buf_usr_addr)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    ret = _dmx_r_rec_fct_pst_mmap_impl(rrec_fct, buf_usr_addr);

    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

static hi_s32 dmx_r_rec_fct_suspend_impl(struct dmx_r_rec_fct *rrec_fct)
{
    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    _dmx_r_rec_fct_close_impl(rrec_fct);

    osal_mutex_unlock(&rrec_fct->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_rec_fct_resume_impl(struct dmx_r_rec_fct *rrec_fct)
{
    osal_mutex_lock(&rrec_fct->lock);

    WARN_ON(rrec_fct->staled == HI_TRUE);

    _dmx_r_rec_fct_open_impl(rrec_fct);

    osal_mutex_unlock(&rrec_fct->lock);

    return HI_ERR_DMX_NOT_SUPPORT;
}

static hi_s32 dmx_r_rec_fct_get_buf_handle_impl(struct dmx_r_rec_fct *rrec_fct, hi_handle *buf_handle)
{
    hi_s32 ret;

    osal_mutex_lock(&rrec_fct->lock);
    if (rrec_fct->secure_mode != DMX_SECURE_TEE) {
        HI_ERR_DEMUX("none secure buf can't getbufhandle.\n");
        ret = HI_FAILURE;
        goto out;
    }

    *buf_handle = rrec_fct->rbuf->buf_handle;

    ret = HI_SUCCESS;
out:
    osal_mutex_unlock(&rrec_fct->lock);

    return ret;
}

struct dmx_r_rec_fct_ops g_dmx_rec_fct_ops = {
    .create         = dmx_r_rec_fct_create_impl,
    .open           = dmx_r_rec_fct_open_impl,
    .get_attrs      = dmx_r_rec_fct_get_attrs_impl,
    .set_attrs      = dmx_r_rec_fct_set_attrs_impl,
    .get_status     = dmx_r_rec_fct_get_status_impl,
    .add_ch         = dmx_r_rec_fct_add_ch_impl,
    .del_ch         = dmx_r_rec_fct_del_ch_impl,
    .del_all_ch     = dmx_r_rec_fct_del_all_ch_impl,
    .acquire_buf    = dmx_r_rec_fct_acquire_buf_impl,
    .release_buf    = dmx_r_rec_fct_release_buf_impl,
    .recv_idx       = dmx_r_rec_fct_recv_idx_impl,
    .peek_idx_buf   = dmx_r_rec_fct_peek_idx_buf_impl,
    .close          = dmx_r_rec_fct_close_impl,
    .destroy        = dmx_r_rec_fct_destroy_impl,
    .get_buf_handle = dmx_r_rec_fct_get_buf_handle_impl,

    .pre_mmap       = dmx_r_rec_fct_pre_mmap_impl,
    .pst_mmap       = dmx_r_rec_fct_pst_mmap_impl,

    .suspend        = dmx_r_rec_fct_suspend_impl,
    .resume         = dmx_r_rec_fct_resume_impl,
};

#ifdef HI_DEMUX_PROC_SUPPORT
#define DMX_DEFAULT_RECFCT_SIZE         (4 * 1024 * 1024)
#define DMX_DEFAULT_ACQUIRE_SIZE        (47 * 1024)
#define DMX_DEFAULT_ACQUIRE_TIMEOUT     500

hi_handle g_all_ts_rec_fct_hdl = HI_INVALID_HANDLE;
hi_handle g_all_ts_band_hdl    = HI_INVALID_HANDLE;

static struct file        *g_all_ts_file_hdl = HI_NULL;
static osal_task          *g_all_ts_thread = HI_NULL;
static struct dmx_session *g_all_ts_session = HI_NULL;


static hi_void rec_fct_get_default_attrs(dmx_rec_attrs *attrs, dmx_rec_type rec_type)
{
    memset(attrs, 0, sizeof(dmx_rec_attrs));

    attrs->descramed        = HI_TRUE;
    attrs->index_type       = DMX_REC_INDEX_TYPE_NONE;
    attrs->rec_buf_size     = DMX_DEFAULT_RECFCT_SIZE;
    attrs->rec_type         = rec_type;
    attrs->secure_mode      = DMX_SECURE_NONE;
    attrs->index_src_pid    = DMX_INVALID_PID;
    attrs->vcodec_type      = DMX_VCODEC_TYPE_MPEG2;
}

static hi_s32 create_rec_file(hi_s8 *file_name, struct file **file_hdl)
{
    *file_hdl = hi_drv_file_open(file_name, 1);
    if ((*file_hdl) == HI_NULL) {
        HI_ERR_DEMUX("open %s error\n", file_name);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 create_rec_thread(hi_s32 (*thread_fn)(hi_void *arg), hi_s8 *thread_name, osal_task **rec_thread)
{
    *rec_thread = osal_kthread_create(thread_fn, HI_NULL, thread_name, 0);
    if (IS_ERR(*rec_thread)) {
        HI_ERR_DEMUX("create kthread failed\n");
        *rec_thread = HI_NULL;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 save_all_ts_routine(hi_void *arg)
{
    dmx_buffer rec_fct_buf;
    hi_s32 ret;
    while (1) {
        if (kthread_should_stop()) {
            break;
        }

        if (g_all_ts_rec_fct_hdl == HI_INVALID_HANDLE) {
            continue;
        }

        ret = dmx_rec_fct_acquire_buf(g_all_ts_rec_fct_hdl,
            DMX_DEFAULT_ACQUIRE_SIZE, DMX_DEFAULT_ACQUIRE_TIMEOUT, &rec_fct_buf);
        if (ret == HI_SUCCESS) {
            if (g_all_ts_file_hdl) {
                hi_drv_file_write(g_all_ts_file_hdl,
                    (hi_u8 *)rec_fct_buf.ker_vir_addr, rec_fct_buf.length);
            }

            dmx_rec_fct_release_buf(g_all_ts_rec_fct_hdl, &rec_fct_buf);
        }

        yield();
    }

    return HI_SUCCESS;
}

hi_s32 save_all_ts_route_create(hi_u32 port_id, hi_u8 *file_name, hi_u32 file_name_length)
{
    hi_s32 ret;
    dmx_band_attr band_attr = {0};
    dmx_rec_attrs   rec_attr = {0};

    dmx_dofunc_goto(create_rec_file(file_name, &g_all_ts_file_hdl), ret, exit);

    dmx_dofunc_goto(dmx_session_create(&g_all_ts_session), ret, file_destroy);

    dmx_dofunc_goto(dmx_band_open(DMX_ANY_BAND, &band_attr, &g_all_ts_band_hdl, g_all_ts_session),
        ret, session_destroy);

    dmx_dofunc_goto(dmx_band_attach_port(g_all_ts_band_hdl, port_id), ret, band_close);

    rec_fct_get_default_attrs(&rec_attr, DMX_REC_TYPE_ALL_PID);
    dmx_dofunc_goto(dmx_rec_fct_create(&rec_attr, &g_all_ts_rec_fct_hdl, g_all_ts_session),
        ret, band_detach);

    dmx_dofunc_goto(dmx_rec_fct_add_ch(g_all_ts_rec_fct_hdl, g_all_ts_band_hdl), ret, rec_fct_destroy);

    dmx_dofunc_goto(dmx_rec_fct_open(g_all_ts_rec_fct_hdl), ret, del_pid_chn);

    dmx_dofunc_goto(create_rec_thread(save_all_ts_routine, "save_all_ts", &g_all_ts_thread), ret, rec_fct_close);

    return HI_SUCCESS;
rec_fct_close:
    dmx_dofunc_no_return(dmx_rec_fct_close(g_all_ts_rec_fct_hdl));
del_pid_chn:
    dmx_dofunc_no_return(dmx_rec_fct_del_ch(g_all_ts_rec_fct_hdl, g_all_ts_band_hdl));
rec_fct_destroy:
    dmx_dofunc_no_return(dmx_rec_fct_destroy(g_all_ts_rec_fct_hdl));
    g_all_ts_rec_fct_hdl = HI_INVALID_HANDLE;
band_detach:
    dmx_dofunc_no_return(dmx_band_detach_port(g_all_ts_band_hdl));
band_close:
    dmx_dofunc_no_return(dmx_band_close(g_all_ts_band_hdl));
    g_all_ts_band_hdl = HI_INVALID_HANDLE;
session_destroy:
    dmx_dofunc_no_return(dmx_session_destroy(g_all_ts_session));
    g_all_ts_session = HI_NULL;
file_destroy:
    hi_drv_file_close(g_all_ts_file_hdl);
    g_all_ts_file_hdl = HI_NULL;
exit:
    return HI_FAILURE;
}

hi_void save_all_ts_route_destroy(hi_void)
{
    osal_kthread_destroy(g_all_ts_thread, HI_TRUE);
    g_all_ts_thread = HI_NULL;

    dmx_dofunc_no_return(dmx_rec_fct_close(g_all_ts_rec_fct_hdl));

    dmx_dofunc_no_return(dmx_rec_fct_del_ch(g_all_ts_rec_fct_hdl, g_all_ts_band_hdl));

    dmx_dofunc_no_return(dmx_rec_fct_destroy(g_all_ts_rec_fct_hdl));
    g_all_ts_rec_fct_hdl = HI_INVALID_HANDLE;

    dmx_dofunc_no_return(dmx_band_detach_port(g_all_ts_band_hdl));

    dmx_dofunc_no_return(dmx_band_close(g_all_ts_band_hdl));
    g_all_ts_band_hdl = HI_INVALID_HANDLE;

    dmx_dofunc_no_return(dmx_session_destroy(g_all_ts_session));
    g_all_ts_session = HI_NULL;

    hi_drv_file_close(g_all_ts_file_hdl);
    g_all_ts_file_hdl = HI_NULL;
}

hi_s32 dmx_rec_fct_start_save_all_ts(hi_u32 port_id)
{
    hi_s32           ret;
    struct tm        now;
    hi_char          file_name[DMX_FILE_NAME_LEN] = {0};

    if (g_all_ts_file_hdl != HI_NULL) {
        HI_ERR_DEMUX("already started\n");
        return HI_FAILURE;
    }

    ret = hi_drv_file_get_store_path(file_name, DMX_FILE_NAME_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get path failed\n");
        return ret;
    }

    time_to_tm(get_seconds(), 0, &now);
    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/dmx_allts_%u-%02u_%02u_%02u.ts",
                     file_name, port_id, now.tm_hour, now.tm_min, now.tm_sec);
    if (ret < 0) {
        HI_ERR_DEMUX("snprintf_s failed, ret = %d\n", ret);
        return ret;
    }

    ret = save_all_ts_route_create(port_id, file_name, sizeof(file_name));
    if (ret != HI_SUCCESS) {
        save_all_ts_route_destroy();
    }

    return ret;
}

hi_void dmx_rec_fct_stop_save_all_ts(hi_void)
{
    if (g_all_ts_thread == HI_NULL) {
        HI_WARN_DEMUX("already stoped!\n");
        return;
    }
    save_all_ts_route_destroy();
}

hi_handle g_dmx_ts_rec_fct_hdl = HI_INVALID_HANDLE;
hi_handle g_dmx_ts_band_hdl    = HI_INVALID_HANDLE;
hi_handle g_dmx_ts_pid_ch_hdl[REC_MAX_PID_CNT];
hi_u32    g_dmx_ts_pid_ch_count = 0;

static struct file        *g_dmx_ts_file_hdl = HI_NULL;
static osal_task          *g_dmx_ts_thread = HI_NULL;
static struct dmx_session *g_dmx_ts_session = HI_NULL;

static hi_s32 save_dmx_ts_routine(hi_void *arg)
{
    dmx_buffer rec_fct_buf;
    hi_s32 ret;
    while (1) {
        if (kthread_should_stop()) {
            break;
        }

        if (g_dmx_ts_rec_fct_hdl == HI_INVALID_HANDLE) {
            continue;
        }

        ret = dmx_rec_fct_acquire_buf(g_dmx_ts_rec_fct_hdl,
            DMX_DEFAULT_ACQUIRE_SIZE, DMX_DEFAULT_ACQUIRE_TIMEOUT, &rec_fct_buf);
        if (ret == HI_SUCCESS) {
            if (g_dmx_ts_file_hdl) {
                hi_drv_file_write(g_dmx_ts_file_hdl,
                    (hi_u8 *)rec_fct_buf.ker_vir_addr, rec_fct_buf.length);
            }

            dmx_rec_fct_release_buf(g_dmx_ts_rec_fct_hdl, &rec_fct_buf);
        }

        yield();
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_ts_get_port_with_band(dmx_band band, struct dmx_mgmt *rmgmt, dmx_port *port)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *cur = HI_NULL;

    osal_mutex_lock(&rmgmt->band_list_lock);
    list_for_each(cur, &rmgmt->band_head) {
        struct dmx_r_band *r_band = list_entry(cur, struct dmx_r_band, node);

        if (band == r_band->base.id + DMX_BAND_0) {
            *port = r_band->port;
            ret = HI_SUCCESS;
            break;
        }
    }
    osal_mutex_unlock(&rmgmt->band_list_lock);

    return ret;
}

static hi_s32 dmx_ts_create_pid_chn(dmx_band band, struct dmx_mgmt *rmgmt)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 pid_count = 0;
    hi_u32 pid[REC_MAX_PID_CNT];
    struct list_head *cur = HI_NULL;

    osal_mutex_lock(&rmgmt->pid_channel_list_lock);
    list_for_each(cur, &rmgmt->pid_channel_head) {
        struct dmx_r_pid_ch *rpid_ch = list_entry(cur, struct dmx_r_pid_ch, node0);

        if (rpid_ch->rband) {
            if (band == rpid_ch->rband->base.id + DMX_BAND_0) {
                pid[pid_count++] = rpid_ch->pid;
                if (pid_count >= REC_MAX_PID_CNT) {
                    break;
                }
            }
        }
    }
    osal_mutex_unlock(&rmgmt->pid_channel_list_lock);

    for (g_dmx_ts_pid_ch_count = 0; g_dmx_ts_pid_ch_count < pid_count; g_dmx_ts_pid_ch_count++) {
        ret = dmx_pid_ch_create(g_dmx_ts_band_hdl, pid[g_dmx_ts_pid_ch_count],
            &g_dmx_ts_pid_ch_hdl[g_dmx_ts_pid_ch_count], g_dmx_ts_session);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_pid_ch_create failed with ret:0x%x\n", ret);
            return ret;
        }
    }

    return ret;
}

static hi_void dmx_ts_destroy_pid_chn(hi_void)
{
    hi_s32 ret;
    hi_u32 pid_chn_count = 0;

    for (pid_chn_count = 0; pid_chn_count < g_dmx_ts_pid_ch_count; pid_chn_count++) {
        ret = dmx_pid_ch_destroy(g_dmx_ts_pid_ch_hdl[pid_chn_count]);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_pid_ch_destroy failed with ret:0x%x\n", ret);
        }
    }

    g_dmx_ts_pid_ch_count = 0;

    return;
}

static hi_s32 dmx_ts_add_pid_chn(hi_void)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 pid_chn_count = 0;

    for (pid_chn_count = 0; pid_chn_count < g_dmx_ts_pid_ch_count; pid_chn_count++) {
        ret = dmx_rec_fct_add_ch(g_dmx_ts_rec_fct_hdl, g_dmx_ts_pid_ch_hdl[pid_chn_count]);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_rec_fct_add_ch failed with ret:0x%x\n", ret);
            break;
        }
    }

    return ret;
}

static hi_void dmx_ts_del_pid_chn(hi_void)
{
    hi_s32 ret;
    hi_u32 pid_chn_count = 0;

    for (pid_chn_count = 0; pid_chn_count < g_dmx_ts_pid_ch_count; pid_chn_count++) {
        ret = dmx_rec_fct_del_ch(g_dmx_ts_rec_fct_hdl, g_dmx_ts_pid_ch_hdl[pid_chn_count]);
        if (ret != HI_SUCCESS) {
            HI_ERR_DEMUX("dmx_rec_fct_del_ch failed with ret:0x%x\n", ret);
        }
    }

    return;
}

hi_s32 save_dmx_ts_route_create(dmx_band band, hi_u8 *file_name, hi_u32 file_name_length, struct dmx_mgmt *rmgmt)
{
    hi_s32 ret;
    dmx_port port = DMX_PORT_MAX;
    dmx_band_attr band_attr = {0};
    dmx_rec_attrs rec_attr  = {0};

    dmx_dofunc_goto(create_rec_file(file_name, &g_dmx_ts_file_hdl), ret, exit);

    dmx_dofunc_goto(dmx_session_create(&g_dmx_ts_session), ret, file_destroy);

    dmx_dofunc_goto(dmx_band_open(DMX_ANY_BAND, &band_attr, &g_dmx_ts_band_hdl, g_dmx_ts_session),
        ret, session_destroy);

    dmx_dofunc_goto(dmx_ts_get_port_with_band(band, rmgmt, &port), ret, band_close);

    dmx_dofunc_goto(dmx_band_attach_port(g_dmx_ts_band_hdl, port), ret, band_close);

    rec_fct_get_default_attrs(&rec_attr, DMX_REC_TYPE_SELECT_PID);
    dmx_dofunc_goto(dmx_rec_fct_create(&rec_attr, &g_dmx_ts_rec_fct_hdl, g_dmx_ts_session),
        ret, band_detach);

    dmx_dofunc_goto(dmx_ts_create_pid_chn(band, rmgmt), ret, rec_fct_destroy);

    dmx_dofunc_goto(dmx_ts_add_pid_chn(), ret, pid_chn_destroy);

    dmx_dofunc_goto(dmx_rec_fct_open(g_dmx_ts_rec_fct_hdl), ret, del_pid_chn);

    dmx_dofunc_goto(create_rec_thread(save_dmx_ts_routine, "save_dmx_ts", &g_dmx_ts_thread), ret, rec_fct_close);

    return HI_SUCCESS;
rec_fct_close:
    dmx_dofunc_no_return(dmx_rec_fct_close(g_dmx_ts_rec_fct_hdl));
del_pid_chn:
    dmx_ts_del_pid_chn();
pid_chn_destroy:
    dmx_ts_destroy_pid_chn();
rec_fct_destroy:
    dmx_dofunc_no_return(dmx_rec_fct_destroy(g_dmx_ts_rec_fct_hdl));
    g_dmx_ts_rec_fct_hdl = HI_INVALID_HANDLE;
band_detach:
    dmx_dofunc_no_return(dmx_band_detach_port(g_dmx_ts_band_hdl));
band_close:
    dmx_dofunc_no_return(dmx_band_close(g_dmx_ts_band_hdl));
    g_dmx_ts_band_hdl = HI_INVALID_HANDLE;
session_destroy:
    dmx_dofunc_no_return(dmx_session_destroy(g_dmx_ts_session));
    g_dmx_ts_session = HI_NULL;
file_destroy:
    hi_drv_file_close(g_dmx_ts_file_hdl);
    g_dmx_ts_file_hdl = HI_NULL;
exit:
    return HI_FAILURE;
}

hi_void save_dmx_ts_route_destroy(hi_void)
{
    osal_kthread_destroy(g_dmx_ts_thread, HI_TRUE);
    g_dmx_ts_thread = HI_NULL;

    dmx_dofunc_no_return(dmx_rec_fct_close(g_dmx_ts_rec_fct_hdl));

    dmx_ts_del_pid_chn();

    dmx_ts_destroy_pid_chn();

    dmx_dofunc_no_return(dmx_rec_fct_destroy(g_dmx_ts_rec_fct_hdl));
    g_dmx_ts_rec_fct_hdl = HI_INVALID_HANDLE;

    dmx_dofunc_no_return(dmx_band_detach_port(g_dmx_ts_band_hdl));

    dmx_dofunc_no_return(dmx_band_close(g_dmx_ts_band_hdl));
    g_dmx_ts_band_hdl = HI_INVALID_HANDLE;

    dmx_dofunc_no_return(dmx_session_destroy(g_dmx_ts_session));
    g_dmx_ts_session = HI_NULL;

    hi_drv_file_close(g_dmx_ts_file_hdl);
    g_dmx_ts_file_hdl = HI_NULL;
}

hi_s32 dmx_rec_fct_start_save_dmx_ts(hi_u32 band)
{
    hi_s32           ret;
    struct tm        now;
    hi_char          file_name[DMX_FILE_NAME_LEN] = {0};
    struct dmx_mgmt *rmgmt = HI_NULL;

    if (g_dmx_ts_file_hdl != HI_NULL) {
        HI_ERR_DEMUX("already started\n");
        return HI_FAILURE;
    }

    ret = hi_drv_file_get_store_path(file_name, DMX_FILE_NAME_LEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_DEMUX("get path failed\n");
        return ret;
    }

    time_to_tm(get_seconds(), 0, &now);
    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/dmx_dmxts_%u-%02u_%02u_%02u.ts",
                     file_name, band, now.tm_hour, now.tm_min, now.tm_sec);
    if (ret < 0) {
        HI_ERR_DEMUX("snprintf_s failed, ret = %d\n", ret);
        return ret;
    }

    rmgmt = _get_dmx_mgmt();
    if (rmgmt->state != DMX_MGMT_OPENED) {
        HI_ERR_DEMUX("demux is not opened\n");
        return HI_FAILURE;
    }

    ret = save_dmx_ts_route_create(band, file_name, sizeof(file_name), rmgmt);

    return ret;
}

hi_void dmx_rec_fct_stop_save_dmx_ts(hi_void)
{
    if (g_dmx_ts_thread == HI_NULL) {
        HI_WARN_DEMUX("already stoped!\n");
        return;
    }

    save_dmx_ts_route_destroy();
}

#endif /* HI_DEMUX_PROC_SUPPORT */


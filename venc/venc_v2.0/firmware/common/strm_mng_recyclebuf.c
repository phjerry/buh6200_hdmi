/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#include "strm_mng_recyclebuf.h"
#include "hi_drv_mem.h"
#include <linux/delay.h>
#include "hi_errno.h"
#include <linux/types.h>
#include "linux/hisilicon/securec.h"

/***************************** static definition *****************************/
static recycle_buf_global g_st_rb_param = {
    .inst_handle = 0,
    .is_init        = 0,
    .inst_head = LIST_HEAD_INIT(g_st_rb_param.inst_head)
};

#define HI_KMALLOC_ATOMIC_RECYCLEBUF(size)     HI_KMALLOC(HI_ID_VENC, size, GFP_ATOMIC)
#define HI_KFREE_RECYCLEBUF(addr)       HI_KFREE(HI_ID_VENC, addr)

#define RECYCLEBUF_FIND_INST(h_inst, inst, flags) { \
        struct list_head* pos = HI_NULL; \
        struct list_head* n = HI_NULL; \
        recycle_buf_init* tmp = HI_NULL; \
        (inst) = HI_NULL; \
        RECYCLEBUF_SPIN_LOCK(g_st_rb_param.spin_lock, (flags)); \
        list_for_each_safe(pos, n, &g_st_rb_param.inst_head) { \
            tmp = list_entry(pos, recycle_buf_init, inst_node); \
            if ((h_inst) == tmp->buf) { \
                (inst) = tmp; \
                break; \
            } \
        } \
        RECYCLEBUF_SPIN_UNLOCK(g_st_rb_param.spin_lock, (flags)); \
    }

#define RECYCLEBUF_ASSERT_RETURN(condition, ret_value) do {   \
    if (!(condition))                                                \
    {                                                              \
        HI_ERR_VENC("assert warning\n");\
        return ret_value;                                           \
    }                                                              \
} while (0)

/************************************static func**********************************************/
static hi_s32 buffer_init(valg_crcl_buf* cb, strm_mng_inst_config* config)
{
    if (((config->size & (WORD_ALIGN - 1)) != 0) || (config->size < config->rsv_byte)) {
        return HI_FAILURE;
    } else {
        cb->buf_len = config->size;
    }

    if ((config->rsv_byte == 0) || (config->rsv_byte & (WORD_ALIGN - 1)) != 0) {
        return HI_FAILURE;
    } else {
        cb->rsv_byte = config->rsv_byte;
    }

    cb->base = config->knl_vir_addr;
    cb->phy_addr = config->phy_addr;
    cb->rd_head = 0;
    cb->rd_tail = 0;
    cb->wr_head = 0;
    cb->wr_tail = 0;

    return HI_SUCCESS;
}

static hi_void buf_update_wp(valg_crcl_buf* cb)
{
    cb->wr_tail = cb->wr_head;

    return;
}

static hi_u32 buf_get_free_len(valg_crcl_buf* cb)
{
    hi_u32 free_len;

    if (cb->wr_head >= cb->rd_tail) /* write head before read tail, that did not turn back buffer */ {
        /* calculate data length in buffer when not turn back yet */
        free_len = cb->buf_len - (cb->wr_head - cb->rd_tail) - cb->rsv_byte;
    } else {
        /* calculate data length in buffer when not turn back yet */
        free_len = cb->rd_tail - cb->wr_head - cb->rsv_byte;
    }

    return free_len;
}

static hi_s32 buf_write(valg_crcl_buf* cb, hi_void* virt_src, hi_u32 wr_len)
{
    hi_u32 free_len;
    hi_u32 i;
    valg_cb_info wr_info;
    cpu_word*  dst = NULL;
    cpu_word*  src = NULL;
    hi_u32 wr_head_new = 0;

    /* added by juchaodong */
    if (virt_src == NULL) /* when VEDU finish encoding, update write head */ {
        cb->wr_head = wr_len; /* wr_len is reuse as VEDU's write pointer */
        return HI_SUCCESS;
    }

    /* add end */
    if ((wr_len & (WORD_ALIGN - 1)) != 0) {
        return HI_FAILURE;
    }

    /* call get buffer data len function, get the actual len */
    free_len = buf_get_free_len(cb);

    /* no enough space to write */
    if (free_len < wr_len) {
        return HI_FAILURE;
    }

    /* get enough space to write two data packets */
    /* if write head cross or even reach the bottom of buffer, turn back */
    if ((cb->wr_head + wr_len) >= cb->buf_len) {
        /* calculate two packets's length and addr */
        wr_info.src[0]   = (addr_unit*)cb->base + cb->wr_head;
        wr_info.len[0] = cb->buf_len - cb->wr_head;
        wr_info.src[1]   = cb->base;
        wr_info.len[1] = wr_len - wr_info.len[0];

        /* write head point to start of the next packet */
        wr_head_new = wr_info.len[1];
    } else {
        wr_info.src[0]   = (addr_unit*)cb->base + cb->wr_head;
        wr_info.len[0] = wr_len;
        wr_info.src[1]   = (addr_unit*)wr_info.src[0] + wr_len;
        wr_info.len[1] = 0;       /* the second packet length is 0 */

        /* write head point to start of the next packet */
        wr_head_new = cb->wr_head + wr_len;
    } /* y45339 copy end */

    /* the first input packet copy to buffer */
    src = (cpu_word*)virt_src;
    dst = (cpu_word*)wr_info.src[0];
    i = wr_info.len[0] / sizeof(cpu_word);
    while (i--) {
        *dst++ = *src++;
    }

    /* the second input packet copy to buffer */
    dst = (cpu_word*)wr_info.src[1];
    i = wr_info.len[1] / sizeof(cpu_word);
    while (i--) {
        *dst++ = *src++;
    }

    cb->wr_head = wr_head_new;

    return HI_SUCCESS;
}

static hi_s32 get_para_set_write_buffer(valg_crcl_buf* str_buf, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info)
{
    if (!input_info->recode_cnt) {
        if ((input_info->prepend_sps_pps_enable == HI_TRUE) || (input_info->get_frame_num_ok == 1) ||
            (stream_info->i_frm_insert_by_save_stream)) {
            vedu_efl_nalu_hdr nalu_hdr;

            memset_s(&nalu_hdr, sizeof(vedu_efl_nalu_hdr), 0, sizeof(vedu_efl_nalu_hdr));

            if (input_info->protocol != VEDU_H265) {
                nalu_hdr.packet_len  = 128; /* 128 is packert len */
                nalu_hdr.invld_byte  = 64 - input_info->sps_bits / 8; /* 8 ,64 is calc invld_byte */
                nalu_hdr.last_slice = 0;
                nalu_hdr.type = 0x7;

                nalu_hdr.pts0 = input_info->pts0;
                nalu_hdr.pts1 = input_info->pts1;

                buf_write(str_buf, &nalu_hdr, 64); /* 64 is size of write buf */
                buf_write(str_buf, input_info->sps_stream, 64); /* 64 is size of write buf */

                nalu_hdr.invld_byte = 64 - input_info->pps_bits / 8; /* 8 ,64 is calc invld_byte */
                nalu_hdr.type = 8; /* 8 is one of hdr type */

                buf_write(str_buf, &nalu_hdr, 64); /* 64 is size of write buf */
                buf_write(str_buf, input_info->pps_stream, 64); /* 64 is size of write buf */
            } else {
                // vps
                nalu_hdr.packet_len  = 128; /* 128 is packert len */
                nalu_hdr.invld_byte  = 64 - input_info->vps_bits / 8; /* 8 ,64 is calc invld_byte */
                nalu_hdr.last_slice = 0;
                nalu_hdr.type       = 32; /* 32 is one of hdr type */
                nalu_hdr.bot_field  = 0;
                nalu_hdr.field     = 0;

                nalu_hdr.pts0  = input_info->pts0;
                nalu_hdr.pts1  = input_info->pts1;

                buf_write(str_buf, &nalu_hdr, 64); /* 64 is size of write buf */
                buf_write(str_buf, input_info->vps_stream, 64); /* 64 is size of write buf */

                // sps
                nalu_hdr.invld_byte  = 64 - input_info->sps_bits / 8; /* 8 ,64 is calc invld_byte */
                nalu_hdr.type       = 33;  /* 33 is one of hdr type */

                buf_write(str_buf, &nalu_hdr, 64); /* 64 is size of write buf */
                buf_write(str_buf, input_info->sps_stream, 64); /* 64 is size of write buf */

                // pps
                nalu_hdr.invld_byte = 64 - input_info->pps_bits / 8; /* 8 ,64 is calc invld_byte */
                nalu_hdr.type      = 34; /* 34 is one of hdr type */

                buf_write(str_buf, &nalu_hdr, 64); /* 64 is size of write buf */
                buf_write(str_buf, input_info->pps_stream, 64); /* 64 is size of write buf */
            }

            buf_update_wp(str_buf);

            stream_info->i_frm_insert_by_save_stream = 0;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 get_stream_write_buffer(valg_crcl_buf* str_buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    *(stream_info->strm_buf_rp_vir_addr) = str_buf->rd_tail;
    *(stream_info->strm_buf_wp_vir_addr) = str_buf->wr_head;

    stream_info->cur_strm_buf_size = D_VENC_ALIGN_UP(input_info->target_bits *
        BUF_SIZE_MULT / 8, VEDU_MMZ_ALIGN); /* 8 is calc target bit */

    return HI_SUCCESS;
}

static hi_s32 is_buf_vaild(valg_crcl_buf* cb)
{
    /*  hi_u32 rh; */
    /* judge input length is correct
     * notes: read head cannot cross write tail after update
     * notes:
     * 1. the write tail pointer does not turn back
     * 2. the write tail pointer turn back
     *    1)read head does not turn back, read head will not cross write tail
     *    2)read head turn back;
     */
    if (((cb->rd_tail <= cb->rd_head)
         && (cb->rd_head <= cb->wr_tail)
         && (cb->wr_tail <= cb->wr_head))
        || ((cb->wr_head <= cb->rd_tail)
            && (cb->rd_tail <= cb->rd_head)
            && (cb->rd_head <= cb->wr_tail))
        || ((cb->wr_tail <= cb->wr_head)
            && (cb->wr_head <= cb->rd_tail)
            && (cb->rd_tail <= cb->rd_head))
        || ((cb->rd_head <= cb->wr_tail)
            && (cb->wr_tail <= cb->wr_head)
            && (cb->wr_head <= cb->rd_tail))) {
        return HI_SUCCESS;
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void* get_buf_rd_head(valg_crcl_buf* cb)
{
    hi_void* rh;

    rh = (addr_unit*)(cb->base) + cb->rd_head;
    return rh;
}

static hi_s32 buf_read(valg_crcl_buf* cb, hi_u32 rd_len, valg_cb_info* rd_info)
{
    hi_u32 rh;

    /* judge input length is correct
     * notes: read head cannot cross write tail after update
     * notes:
     * 1. the write tail pointer does not turn back
     * 2. the write tail pointer turn back
     *    1)read head does not turn back, read head will not cross write tail
     *    2)read head turn back;
     */
    rh = cb->rd_head + rd_len;
    if (cb->rd_head <= cb->wr_tail) {
        if (rh > cb->wr_tail) {
            return HI_FAILURE;
        }
    } else if (rh >= cb->buf_len) {
        rh -= cb->buf_len;
        if (rh > cb->wr_tail) {
            return HI_FAILURE;
        }
    }

    /* according to input out-packet length, to determine whether the loop buffer turn back */
    /* reach the bottom is also turn back */
    if ((cb->rd_head + rd_len) >= cb->buf_len) {
        /* calculate two packets's length and addr */
        rd_info->src[0]   = (addr_unit *)cb->base + cb->rd_head;
        rd_info->len[0] = cb->buf_len - cb->rd_head;

        /* the second input packet addr equal to buffer start addr */
        rd_info->src[1]   = cb->base;
        rd_info->len[1] = rd_len - rd_info->len[0];

        /* read head point to the next packet start position */
        cb->rd_head = rd_info->len[1];
    } else {
        rd_info->src[0]   = (addr_unit*)cb->base + cb->rd_head;
        rd_info->len[0] = rd_len;
        rd_info->src[1] = (addr_unit*)rd_info->src[0] + rd_len;

        /* the second input packet length is 0 */
        rd_info->len[1] = 0;

        /* read head point to the next packet start position */
        cb->rd_head += rd_len;
    }

    return HI_SUCCESS;
}

static hi_u32 get_written_length(valg_crcl_buf *recycle_buf)
{
    hi_u32 data_len = 0;

    if (!recycle_buf) {
        return data_len;
    }
    if (recycle_buf->wr_tail >= recycle_buf->rd_head) /* write tail before read head, that did not turn back buffer */ {
        /* calculate data length in buffer when turn back */
        data_len = recycle_buf->wr_tail - recycle_buf->rd_head;
    } else {
        /* calculate data length in buffer when turn back */
        data_len = recycle_buf->buf_len - (recycle_buf->rd_head - recycle_buf->wr_tail);
    }

    return data_len;
}

static hi_void* get_buf_rd_tail(valg_crcl_buf* cb)
{
    hi_void* rt;

    rt = (addr_unit*)(cb->base) + cb->rd_tail;
    return rt;
}

static hi_s32 update_buf_rp(valg_crcl_buf* cb, hi_u32 rd_len)
{
    hi_u32 rt;

    if ((rd_len & (WORD_ALIGN - 1)) != 0) {
        return HI_FAILURE;
    }

    /* the length of the correctness of judgments */
    rt = cb->rd_tail + rd_len;

    /* judge input length is correct
     * notes: read head cannot cross write tail after update
     * notes:
     * 1. the write tail pointer does not turn back
     * 2. the write tail pointer turn back
     *    1)read head does not turn back, read head will not cross write tail
     *    2)read head turn back;
     */
    if (cb->rd_head >= cb->rd_tail) {
        if (rt > cb->rd_head) {
            return HI_FAILURE;
        }
    } else if (rt >= cb->buf_len) {
        rt -= cb->buf_len;
        if (rt > cb->rd_head) {
            return HI_FAILURE;
        }
    }

    cb->rd_tail = rt;

    return HI_SUCCESS;
}

static hi_s32 skp_bit_stream(valg_crcl_buf *str_buf, strm_mng_stream_info *stream_info, vedu_efl_nalu* nalu)
{
    hi_u32 input_len = 0;

    if (is_buf_vaild(str_buf) != HI_SUCCESS) {
        HI_WARN_VENC("===> skp_bit_stream error1!input_len = %d, WH = %x,WT = %x,RH = %x, RT = %x, len = %d\n",
            input_len, str_buf->wr_head, str_buf->wr_tail, str_buf->rd_head, str_buf->rd_tail, str_buf->buf_len);
        return HI_FAILURE;
    }

    input_len = nalu->slc_len[0] + nalu->slc_len[1] + 64; /* 64 is calc input_len */
    input_len = input_len & 63 ? (input_len | 63) + 1 : input_len; /* 63 is calc input_len */

    /* ****** check start addr ****** */
    if ((hi_void*)((hi_u8*)nalu->virt_addr[0] - 64) != get_buf_rd_tail(str_buf)) { /* 64 is calc size */
        HI_WARN_VENC("===> skp_bit_stream error2!nalu->virt_addr[0]-64 = %x-64 = %x, rd_tail = %x\n",
            nalu->virt_addr[0], ((hi_u8*)nalu->virt_addr[0] - 64), get_buf_rd_tail(str_buf)); /* 64 is calc size */
        return HI_FAILURE;
    }

    /* ****** update read index ****** */
    if (HI_FAILURE == update_buf_rp(str_buf, input_len)) {
        HI_WARN_VENC("===> skp_bit_stream error3!input_len = %d, WH = %x,WT = %x,RH = %x, RT = %x, len = %d\n",
            input_len, str_buf->wr_head, str_buf->wr_tail, str_buf->rd_head, str_buf->rd_tail, str_buf->buf_len);
        return HI_FAILURE;
    }

    HI_DBG_VENC("===> skp_bit_stream success!input_len = %d, WH = %x,WT = %x,RH = %x, RT = %x\n",
        input_len, str_buf->wr_head, str_buf->wr_tail, str_buf->rd_head, str_buf->rd_tail);

    return HI_SUCCESS;
}

static hi_void get_nalu(vedu_efl_nalu* nalu, valg_cb_info *rd_info, strm_mng_stream_info *stream_info,
                        vedu_efl_nalu_hdr  *nalu_hdr)
{
    hi_u32 bit_offset;
    hi_u32 bit_offset1;

    nalu->virt_addr[0] = rd_info->src[0];
    nalu->virt_addr[1] = rd_info->src[1];
    nalu->slc_len   [0] = rd_info->len[0];
    nalu->slc_len   [1] = rd_info->len[1];
    nalu->invld_byte    = nalu_hdr->invld_byte;
    if (nalu->slc_len[1] > 0) {
        nalu->slc_len[1] -= nalu_hdr->invld_byte;
    } else {
        nalu->slc_len[0] -= nalu_hdr->invld_byte;
    }

    nalu->pts0 = nalu_hdr->pts0;
    nalu->pts1 = nalu_hdr->pts1;
    nalu->ext_flag = nalu_hdr->pts2;
    nalu->ext_fill_len = nalu_hdr->pts3;
    nalu->frame_end = (nalu_hdr->last_slice) & 1;
    nalu->nalu_type = nalu_hdr->type;

    /* add by j35383, discard nal header of 64 byte */
    nalu->slc_len   [0] -= 64; /* 64 is calc slc len */
    nalu->virt_addr[0] = (hi_void*)(nalu->virt_addr[0] + 64); /* 64 is calc virt addr */
    bit_offset = (hi_u32)((hi_u8*)nalu->virt_addr[0] - (hi_u8*)stream_info->stream_mmz_buf.start_vir_addr);
    bit_offset1 = (hi_u32)((hi_u8*)nalu->virt_addr[1] - (hi_u8*)stream_info->stream_mmz_buf.start_vir_addr);

    nalu->phy_addr[0] = stream_info->stream_mmz_buf.start_phy_addr + bit_offset;
    nalu->phy_addr[1] = stream_info->stream_mmz_buf.start_phy_addr + bit_offset1;

    return;
}

static hi_s32 get_bit_stream(valg_crcl_buf *str_buf, hi_u32 strm_buf_ext_len,
    strm_mng_stream_info *stream_info, vedu_efl_nalu* nalu)
{
    vedu_efl_nalu_hdr  *nalu_hdr = NULL;
    valg_cb_info     rd_info;

    if (is_buf_vaild(str_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (get_written_length(str_buf) == 0) {
        return HI_FAILURE;
    }

    nalu_hdr = (vedu_efl_nalu_hdr*) get_buf_rd_head(str_buf);  /* parse the head   */

    if (nalu_hdr == NULL) {
        HI_ERR_VENC("nalu ptr is NULL\n");
        return HI_ERR_VENC_NULL_PTR;
    }

    if ((stream_info->protocol != VEDU_JPGE) && (nalu_hdr->type != 1) &&
        (nalu_hdr->type != 5) && (nalu_hdr->type != 7) && (nalu_hdr->type != 8)  &&  /* 5 7 8 is one of hdr type */
        (nalu_hdr->type != 32) && (nalu_hdr->type != 33) && (nalu_hdr->type != 34)) { /* 32 33 34 is one of hdr type */
        HI_ERR_VENC("nalu type is wrong!\n");
        return HI_FAILURE;
    }

    /* ****** get addr and len, updata readhead ****** */
    if (HI_SUCCESS != buf_read(str_buf, nalu_hdr->packet_len, &rd_info)) {
        return HI_FAILURE;
    }

    get_nalu(nalu, &rd_info, stream_info, nalu_hdr);

    stream_info->stream_total_byte += nalu->slc_len[0];
    stream_info->stream_total_byte += nalu->slc_len[1];

    if (nalu->slc_len[1] > 0) {
        if (nalu->slc_len[1] < strm_buf_ext_len) {
            memcpy_s((nalu->virt_addr[0] + nalu->slc_len[0]), nalu->slc_len[1], nalu->virt_addr[1], nalu->slc_len[1]);
        } else {
            HI_ERR_VENC("slc_len[1] = %d larger than strm_buf_ext_len(%d) can't copy! the stream maybe lost!\n",
                nalu->slc_len[1], strm_buf_ext_len);
            if (skp_bit_stream(str_buf, stream_info, nalu) != HI_SUCCESS) {
                HI_ERR_VENC("call skp_bit_stream failed!\n");
            }
            return HI_FAILURE;
        }
    }

    HI_DBG_VENC("success===>WH = %x, WT = %x, RH = %x, RT = %x, size = %d\n", str_buf->wr_head,
        str_buf->wr_tail, str_buf->rd_head, str_buf->rd_tail, str_buf->buf_len);
    return HI_SUCCESS;
}

#ifdef VENC_SUPPORT_JPGE
static hi_s32 get_buf_next_rp(valg_crcl_buf* cb, hi_u32 rd_len)
{
    hi_u32 rt;

    if ((rd_len & (WORD_ALIGN - 1)) != 0) {
        return HI_FAILURE;
    }

    /* the length of the correctness of judgments */
    rt = cb->rd_head + rd_len;

    /* judge input length is correct
     * notes: read head cannot cross write tail after update
     * notes:
     * 1. the write tail pointer does not turn back
     * 2. the write tail pointer turn back
     *    1)read head does not turn back, read head will not cross write tail
     *    2)read head turn back;
     */
    if (cb->wr_tail >= rt) {
        if (cb->wr_tail == rt) {
            return HI_FAILURE;
        }
    } else if (rt >= cb->buf_len) {
        rt -= cb->buf_len;
        if (rt > cb->wr_tail) {
            return HI_FAILURE;
        }
    }

    cb->rd_head = rt;
    return HI_SUCCESS;
}
#endif

/* open vedu use */
hi_void recycle_buffer_init(hi_void)
{
    if (g_st_rb_param.is_init == 0) {
        /* init global parameter */
        g_st_rb_param.inst_handle = 0;

        /* init global mutex */
        venc_drv_osal_sem_init(&g_st_rb_param.sem);
        if (venc_drv_osal_lock_create(&g_st_rb_param.spin_lock) != HI_SUCCESS) {
            HI_ERR_VENC("g_st_rb_param.spin_lock init failed!\n");
        }

        /* init global inst */
        INIT_LIST_HEAD(&g_st_rb_param.inst_head);

        g_st_rb_param.is_init = 1;
    }

    return;
}

hi_void recycle_buf_deinit(hi_void)
{
    struct list_head *pos = HI_NULL;
    struct list_head *n = HI_NULL;
    struct list_head *head = HI_NULL;
    recycle_buf_init *inst = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    if (g_st_rb_param.is_init) {
        RECYCLEBUF_LOCK(g_st_rb_param.sem, ret);
        if (ret != HI_SUCCESS) {
            return;
        }
        head = &g_st_rb_param.inst_head;
        if (!list_empty(head)) {
            list_for_each_safe(pos, n, head) {
                inst = list_entry(pos, recycle_buf_init, inst_node);
                RECYCLEBUF_UNLOCK(g_st_rb_param.sem);
                recycle_buf_destroy(inst->buf);
                RECYCLEBUF_LOCK(g_st_rb_param.sem, ret);
                if (ret != HI_SUCCESS) {
                    return;
                }
            }
        }

        venc_drv_osal_lock_destory(g_st_rb_param.spin_lock);
        g_st_rb_param.inst_handle = 0;
        RECYCLEBUF_UNLOCK(g_st_rb_param.sem);

        g_st_rb_param.is_init = 0;
    }
}

static hi_void init_recycle_buf_inst_para(recycle_buf_init *inst, strm_mng_inst_config *config)
{
    /* init stream packet info for release */
    memset_s(&inst->st_chn_packet, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu));

    venc_drv_osal_lock_create(&inst->spin_lock);

    /* init block list parameter */
    inst->get_try  = 0;
    inst->get_ok   = 0;
    inst->put_try  = 0;
    inst->put_ok   = 0;
    inst->recv_try = 0;
    inst->recv_ok  = 0;
    inst->rls_try  = 0;
    inst->rls_ok   = 0;

    inst->strm_buf_ext_len = config->strm_ext_len;

    return;
}
hi_s32 recycle_buf_create(hi_handle handle, strm_mng_inst_config *config)
{
    hi_s32 ret = 0;
    recycle_buf_init *inst = HI_NULL;
    struct list_head* pos = HI_NULL;
    struct list_head* n = HI_NULL;
    recycle_buf_init* tmp = HI_NULL;

    if (config == HI_NULL) {
        return HI_FAILURE;
    }

    list_for_each_safe(pos, n, &g_st_rb_param.inst_head) {
        tmp = list_entry(pos, recycle_buf_init, inst_node);
        if (handle == tmp->buf) {
            HI_ERR_VENC("private handle already exists!\n");
            return HI_FAILURE;
        }
    }
    /* allocate an instance */
    inst = HI_KMALLOC_ATOMIC_RECYCLEBUF(sizeof(recycle_buf_init));

    if (inst == HI_NULL) {
        HI_FATAL_RECYCLEBUF("no memory.\n");

        return HI_FAILURE;
    }

    memset_s(inst, sizeof(recycle_buf_init), 0, sizeof(recycle_buf_init));

    /* set instant number */
    RECYCLEBUF_LOCK(g_st_rb_param.sem, ret);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* init instance parameter */
    inst->buf = handle;

#ifdef HI_TEE_SUPPORT
    inst->tvp = config->tvp;
#endif

    /* init recycle buffer info */
    ret = buffer_init(&inst->st_recycle_buf, config);

    if (ret != HI_SUCCESS) {
        RECYCLEBUF_UNLOCK(g_st_rb_param.sem);

        HI_KFREE_RECYCLEBUF(inst);

        return ret;
    }

    init_recycle_buf_inst_para(inst, config);

    /* add this instance to instance list */
    list_add_tail(&inst->inst_node, &g_st_rb_param.inst_head);

    g_st_rb_param.inst_handle++;

    RECYCLEBUF_UNLOCK(g_st_rb_param.sem);

    return HI_SUCCESS;
}

hi_s32 recycle_buf_destroy(hi_handle handle)
{
    recycle_buf_init *inst = HI_NULL;
    hi_size_t flags;
    hi_s32 ret = HI_FAILURE;

    /* find instance by handle */
    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    /* set instant number */
    RECYCLEBUF_LOCK(g_st_rb_param.sem, ret);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    if (g_st_rb_param.inst_handle == 0) {
        HI_ERR_VENC("destroy failed! there is no channel!");
        RECYCLEBUF_UNLOCK(g_st_rb_param.sem);

        return HI_FAILURE;
    }

    g_st_rb_param.inst_handle--;

    /* remove instance from list */
    list_del(&inst->inst_node);

    RECYCLEBUF_UNLOCK(g_st_rb_param.sem);

    /* free resource */
    HI_KFREE_RECYCLEBUF(inst);

    return HI_SUCCESS;
}

hi_s32 recycle_buf_get_write_buf(hi_handle handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_u32 ret = HI_SUCCESS;
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_FAILURE;
    }

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    if (input_info->is_intra_pic) {
        ret = get_para_set_write_buffer(&inst->st_recycle_buf, input_info, stream_info);

        if (ret != HI_SUCCESS) {
            RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

            return ret;
        }
    }

    ret =  get_stream_write_buffer(&inst->st_recycle_buf, input_info, stream_info);

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return ret;
}

hi_s32 recycle_buf_put_write_buf(hi_handle handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_u32 wrptr;
    hi_size_t flags;

    recycle_buf_init *inst = HI_NULL;
    valg_crcl_buf* str_buf = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || input_info == HI_NULL || stream_info == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    str_buf = &inst->st_recycle_buf;

    if (!input_info->venc_buf_full && !input_info->venc_pbit_overflow) {
        /* read wp which be changed by HW */
        wrptr = *(stream_info->strm_buf_wp_vir_addr);

        buf_write   (str_buf, NULL, wrptr);
        buf_update_wp(str_buf);
    } else {
        stream_info->too_few_buffer_skip++;
    }

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_u32 recycle_buf_get_written_length(hi_handle handle)
{
    hi_u32 data_len;
    recycle_buf_init *inst = HI_NULL;
    hi_size_t flags;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK  (inst->spin_lock, flags);
    data_len = get_written_length(&inst->st_recycle_buf);
    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return data_len;
}

hi_s32 recycle_buf_acq_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu* nalu)
{
    hi_size_t flags;
    hi_s32 ret;
    recycle_buf_init *inst = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || stream_info == HI_NULL || nalu == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);
    ret = get_bit_stream(&inst->st_recycle_buf, inst->strm_buf_ext_len, stream_info, nalu);
    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return ret;
}

hi_s32 recycle_buf_rls_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu* nalu)
{
    hi_size_t flags;
    hi_s32 ret;
    recycle_buf_init *inst = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || stream_info == HI_NULL || nalu == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK (inst->spin_lock, flags);
    ret = skp_bit_stream(&inst->st_recycle_buf, stream_info, nalu);
    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return ret;
}

hi_s32 recycle_buf_get_acq_strm_info(handle_protocol *h_protocol, hi_u8* start_vir_addr,
    vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset* buf_offset)
{
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;

    hi_handle handle = h_protocol->handle;
    hi_u32 protocol = h_protocol->protocol;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || vedu_packet == HI_NULL || stream == HI_NULL || buf_offset == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (vedu_packet->slc_len[1] > 0) {
        memcpy_s(&inst->st_chn_packet, sizeof(vedu_efl_nalu), vedu_packet, sizeof(vedu_efl_nalu));
    }

    stream->virt_addr      = (hi_size_t)(ULONG)vedu_packet->virt_addr[0];
    stream->slc_len    = vedu_packet->slc_len[0] + vedu_packet->slc_len[1];
    stream->pts_ms     = vedu_packet->pts0;

    stream->frame_end = (0 == vedu_packet->frame_end) ? HI_FALSE : HI_TRUE;


    if (protocol == VEDU_H265) {
#ifdef VENC_SUPPORT_H265
        stream->data_type.h265e_type = trans_nalu_type_num_for_h265(vedu_packet->nalu_type);
#endif
    } else {
        stream->data_type.h264e_type = vedu_packet->nalu_type;
    }

    if (stream->slc_len > 0) {
        buf_offset->strm_buf_offset[0] = vedu_packet->phy_addr[0] - inst->st_recycle_buf.phy_addr;
    }

    return HI_SUCCESS;
}

hi_s32 recycle_buf_get_rls_strm_info(hi_handle handle, hi_u32 protocol, strm_mng_stream_info *stream_info,
    venc_ioctl_acquire_stream* acq_strm, vedu_efl_nalu* vedu_packet)
{
    hi_size_t flags;
    hi_venc_stream  stream;
    recycle_buf_init *inst = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || acq_strm == HI_NULL || vedu_packet == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    stream = acq_strm->stream;

    stream.virt_addr  = (hi_size_t)(inst->st_recycle_buf.base + acq_strm->h264_stream_off);

    if ((hi_size_t)inst->st_chn_packet.virt_addr[0] == stream.virt_addr) {
        memcpy_s(vedu_packet, sizeof(vedu_efl_nalu), &inst->st_chn_packet, sizeof(vedu_efl_nalu));
        memset_s(&inst->st_chn_packet, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu));
    } else {
        vedu_packet->virt_addr[0] = (hi_void*)(ULONG)stream.virt_addr ;
        vedu_packet->slc_len[0]    = stream.slc_len;
        vedu_packet->virt_addr[1] = vedu_packet->virt_addr[0] + vedu_packet->slc_len[0];
        vedu_packet->slc_len[1]    = 0;
        vedu_packet->pts0         = stream.pts_ms;
        vedu_packet->pts1         = 0;
        vedu_packet->frame_end    = (hi_u32)stream.frame_end;
    }

    return HI_SUCCESS;
}

hi_s32 recycle_buf_get_rls_strm_info_intar(hi_handle handle, hi_venc_stream *stream, vedu_efl_nalu* vedu_packet)
{
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || stream == HI_NULL || vedu_packet == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if ((hi_size_t)(ULONG)inst->st_chn_packet.virt_addr[0] == stream->virt_addr) {
        memcpy_s(vedu_packet, sizeof(vedu_efl_nalu), &inst->st_chn_packet, sizeof(vedu_efl_nalu));
        memset_s(&inst->st_chn_packet, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu));
    } else {
        vedu_packet->virt_addr[0] = (hi_void*)(ULONG)stream->virt_addr ;
        vedu_packet->slc_len[0]    = stream->slc_len;
        vedu_packet->virt_addr[1] = vedu_packet->virt_addr[0] + vedu_packet->slc_len[0];
        vedu_packet->slc_len[1]    = 0;
        vedu_packet->pts0         = stream->pts_ms;
        vedu_packet->pts1         = 0;
        vedu_packet->frame_end    = (hi_u32)stream->frame_end;
    }

    return HI_SUCCESS;
}

hi_s32 recycle_buf_check_buf_size(hi_handle handle, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size)
{
    hi_u32 free_len;
    hi_u32 check_buf_size;
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    free_len = buf_get_free_len(&inst->st_recycle_buf);
    check_buf_size = cur_strm_buf_size;

    if (check_buf_size > strm_buf_size - 64) { /* 64 is calc buf size */
        check_buf_size = strm_buf_size - 64; /* 64 is calc buf size */
    }

    if (free_len < D_VENC_ALIGN_DOWN(check_buf_size, VEDU_MMZ_ALIGN)) {
        RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

        return HI_FAILURE;
    }

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_u32 recycle_buf_cal_slc_size(hi_handle handle, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size)
{
    return unf_split_size;
}

hi_s32 recycle_buf_reset_buffer(hi_handle handle, hi_u32 protocol)
{
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;
    valg_crcl_buf *recycle_buf = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    recycle_buf = &inst->st_recycle_buf;

    if (protocol != VEDU_JPGE) {
        recycle_buf->wr_head = recycle_buf->rd_head;
        recycle_buf->wr_tail = recycle_buf->rd_head;
    } else {
#ifdef VENC_SUPPORT_JPGE
        vedu_efl_nalu_hdr *nalu_hdr = HI_NULL;
        hi_u32    new_wr_head;
        valg_crcl_buf cyc_buf_1;

        memset_s(&cyc_buf_1, sizeof(valg_crcl_buf), 0, sizeof(valg_crcl_buf));
        memcpy_s(&cyc_buf_1, sizeof(valg_crcl_buf), recycle_buf, sizeof(valg_crcl_buf));

        nalu_hdr = (vedu_efl_nalu_hdr*)(cyc_buf_1.base + cyc_buf_1.rd_head);
        if (nalu_hdr->last_slice == 1) /*  当前包是图像数据 */ {
            if (HI_SUCCESS == get_buf_next_rp(&cyc_buf_1, nalu_hdr->packet_len)) {
                new_wr_head = cyc_buf_1.rd_head;
            } else   /*  下一包已没数据 */ {
                new_wr_head = cyc_buf_1.wr_head;
            }
        } else        /* 当前包不是有效数据:或者是头，或者没有数据 */ {
            new_wr_head = cyc_buf_1.rd_head;
        }

        recycle_buf->wr_head = new_wr_head;
        recycle_buf->wr_tail = new_wr_head;
#endif
    }

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_u32 recycle_buf_get_bit_buf_size(hi_handle handle, hi_u32 stream_buf_size, hi_u32 slc_split_en,
    hi_u32 luma_size, hi_u32 *ext_len)
{
    hi_u32 bit_buf_size;
    hi_u32 strm_buf_ext_len = 0;

    if (ext_len == HI_NULL) {
        return 0;
    }

    if (slc_split_en) {
        strm_buf_ext_len = D_VENC_ALIGN_UP((luma_size * 3 / 2 / 8), VEDU_MMZ_ALIGN); /* 3 2 8 is calc luma size */
    } else {
        strm_buf_ext_len = D_VENC_ALIGN_UP((luma_size * 5 / 3), VEDU_MMZ_ALIGN); /* 5 3  is calc luma size */
    }
    bit_buf_size = stream_buf_size + strm_buf_ext_len;
    *ext_len   = strm_buf_ext_len;

    return bit_buf_size;
}

hi_s32 recycle_buf_alloc_stream_buf(hi_handle handle, venc_buffer *stream_mmz_buf,
    strm_mng_alloc_info *alloc_info, hi_bool enable_rcn_ref_share_buf)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 alloc_buf_size = 0;

    if (stream_mmz_buf == HI_NULL || alloc_info == HI_NULL) {
        return HI_FAILURE;
    }


    if (alloc_info->protocol == VEDU_JPGE) {
#ifdef VENC_SUPPORT_JPGE
        alloc_buf_size = alloc_info->bit_buf_size;
        ret = hi_drv_venc_mmz_alloc_and_map("venc_jpeg_buf", 0, alloc_buf_size, VEDU_MMZ_ALIGN, stream_mmz_buf);

        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("JPGE hi_drv_venc_alloc_and_map failed\n");

            return HI_FAILURE;
        }
#endif
    } else {
        if (enable_rcn_ref_share_buf == HI_TRUE) {
            alloc_buf_size = alloc_info->bit_buf_size + alloc_info->rcn_size + alloc_info->rcn_block_size +
                alloc_info->me_mv_size;
        } else {
            alloc_buf_size = alloc_info->bit_buf_size + alloc_info->rcn_size + alloc_info->me_mv_size;
        }

        ret = hi_drv_venc_alloc_and_map("venc_steam_buf", 0, alloc_buf_size, VEDU_MMZ_ALIGN, stream_mmz_buf);

        if (ret !=  HI_SUCCESS) {
            HI_ERR_VENC("hi_drv_venc_alloc_and_map failed\n");

            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 recycle_buf_put_buf_for_recode(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info)
{
    return HI_SUCCESS;
}

hi_s32 recycle_buf_get_stream_state(hi_handle handle, strm_mng_state* strm_stat_info)
{
    return HI_SUCCESS;
}

hi_s32 recycle_buf_alloc_para_set_buf_addr(hi_handle handle, venc_buffer *para_set_buf, hi_u32 *para_set_addr)
{
    return HI_SUCCESS;
}

hi_s32 recycle_buf_rls_para_set_buf_addr(hi_handle handle, venc_buffer* para_set_buf)
{
    return HI_SUCCESS;
}

/* FOR JPGE */
#ifdef VENC_SUPPORT_JPGE
hi_s32 recycle_buf_write_header(hi_handle handle, strm_mng_input_info* input_info, strm_mng_stream_info *stream_info)
{
    hi_u32 jfif_hdr_len;
    vedu_efl_nalu_hdr nalu_hdr;
    recycle_buf_init *inst = HI_NULL;
    valg_crcl_buf* str_buf = HI_NULL;
    hi_size_t flags;
    hi_s32 ret;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || stream_info == HI_NULL || input_info == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    jfif_hdr_len = input_info->slc_split_en ? 698 : 698 - 6; /* 698 6 is hdr len */

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    str_buf = &inst->st_recycle_buf;

    HI_DBG_VENC("prepare to write! base = %p, wp = %x, rp = %x, WH = %x, WT = %x, RH = %x, RT = %x, \
        buffer_len = %x\n", str_buf->base, *(stream_info->strm_buf_wp_vir_addr), *(stream_info->strm_buf_rp_vir_addr),\
        str_buf->wr_head, str_buf->wr_tail, str_buf->rd_head, str_buf->rd_tail, str_buf->buf_len);

    memset_s(&nalu_hdr, sizeof(vedu_efl_nalu_hdr), 0, sizeof(vedu_efl_nalu_hdr));
    nalu_hdr.packet_len  = 64 + D_VENC_ALIGN_UP(jfif_hdr_len, VEDU_MMZ_ALIGN); /* 64 is calc packet_len */
    nalu_hdr.invld_byte  = D_VENC_ALIGN_UP(jfif_hdr_len, VEDU_MMZ_ALIGN) - jfif_hdr_len;
    nalu_hdr.last_slice = 0;
    nalu_hdr.pts0       = input_info->pts0;
    nalu_hdr.pts1       = input_info->pts1;

    ret = buf_write(str_buf, &nalu_hdr, 64); /* 64 is size of buf write size */
    ret = buf_write(str_buf, input_info->jfif_hdr, D_VENC_ALIGN_UP(jfif_hdr_len, VEDU_MMZ_ALIGN));

    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("buf write  fail, ret= %#x.\n", ret);
        return ret;
    }

    /* buf_update_wp( str_buf );    updata wp when hardware encode success! */
    *(stream_info->strm_buf_rp_vir_addr) = str_buf->rd_tail;
    *(stream_info->strm_buf_wp_vir_addr) = str_buf->wr_head;
    HI_DBG_VENC("wp = %x, rp = %x, WH = %x,WT = %x, RH = %x, RT = %x\n", str_buf->wr_head, str_buf->rd_tail,
        str_buf->wr_head, str_buf->wr_tail, str_buf->rd_head, str_buf->rd_tail);

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_s32 recycle_buf_put_jpge_write_buffer(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    hi_u32 wrptr;
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;
    valg_crcl_buf* str_buf = HI_NULL;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || input_info == HI_NULL || stream_info == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    str_buf = &inst->st_recycle_buf;

    if (!input_info->venc_buf_full && !input_info->venc_pbit_overflow) {
        /* read wp which be changed by HW */
        wrptr = *(stream_info->strm_buf_wp_vir_addr);
        buf_write   (str_buf, NULL, wrptr);
        buf_update_wp(str_buf);
    } else {
        stream_info->too_few_buffer_skip++;
    }

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_s32 recycle_buf_jpge_remove_header(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    hi_size_t flags;
    recycle_buf_init *inst = HI_NULL;
    valg_crcl_buf* str_buf = HI_NULL;
    hi_u32 jfif_hdr_len;
    hi_u32 len;

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL || input_info == HI_NULL || stream_info == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    jfif_hdr_len = input_info->slc_split_en ? 698 : 698 - 6; /* 698 6 is hdr len */
    len     = 64 + D_VENC_ALIGN_UP(jfif_hdr_len, VEDU_MMZ_ALIGN); /* 64 is calc len */

    RECYCLEBUF_SPIN_LOCK(inst->spin_lock, flags);

    str_buf = &inst->st_recycle_buf;

    if (str_buf->wr_tail == str_buf->rd_head) {
        HI_WARN_VENC("JPEG header is already read, can't remove the header,wr_tail = %d,rd_head = %d\n",
            str_buf->wr_tail, str_buf->rd_head);
        str_buf->wr_head = str_buf->wr_tail;
        RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        return HI_FAILURE;
    }

    if (str_buf->wr_head < len)   /* turn back */ {
        str_buf->wr_head = str_buf->buf_len + str_buf->wr_head - len;
    } else {
        str_buf->wr_head -= len;
    }

    buf_update_wp(str_buf);
    *(stream_info->strm_buf_rp_vir_addr) = str_buf->rd_tail;
    *(stream_info->strm_buf_wp_vir_addr) = str_buf->wr_head;

    RECYCLEBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

static hi_void get_stream_read_buffer(hi_u32 *recycle_cnt, strm_mng_stream_info *stream_info, hi_u32 mode_handle,
                              vedu_efl_nalu *vedu_packet)
{
    hi_s32 ret;

    memset_s(vedu_packet, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu));

    ret = strm_acq_read_buffer(mode_handle, stream_info, vedu_packet);
    if (ret != HI_SUCCESS) {
        osal_msleep_uninterruptible(5); /* 5 is 5us */
        (*recycle_cnt)++;
    }

    if (ret == HI_SUCCESS && vedu_packet->frame_end == 0) {
        (*recycle_cnt)++;
        HI_ERR_VENC("===>while recycle to strm_acq_read_buffer success!ret = %d, b_frame_end = %d, \
            recycle_cnt = %d, \n", ret, vedu_packet->frame_end, *recycle_cnt);
    }
}

static hi_s32 copy_packet_part1(hi_u32 *stream_temp, vedu_efl_nalu *vedu_packet, hi_u32 stream_buf_len,
                                 hi_void* vir_addr)
{
    if ((vedu_packet->slc_len[0] + vedu_packet->slc_len[1]) > stream_buf_len) {
        HI_ERR_VENC("get stream failed, dst_buf_len= %d(need %d), not enough\n", stream_buf_len,
            (vedu_packet->slc_len[0] + vedu_packet->slc_len[1]));

        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    if (vedu_packet->slc_len[0] > 0) {
        memcpy_s(vir_addr, vedu_packet->slc_len[0], vedu_packet->virt_addr[0], vedu_packet->slc_len[0]);
    }

    if (vedu_packet->slc_len[1] > 0) {
        memcpy_s(vir_addr + vedu_packet->slc_len[0], vedu_packet->slc_len[1], vedu_packet->virt_addr[1],
            vedu_packet->slc_len[1]);
    }

    *stream_temp = vedu_packet->slc_len[0] + vedu_packet->slc_len[1];

    return HI_SUCCESS;
}

static hi_void copy_packet_part2(hi_u32 *stream_temp, vedu_efl_nalu *vedu_packet, hi_void* vir_addr)
{
    if (vedu_packet->slc_len[0] > 0) {
        memcpy_s(vir_addr + *stream_temp, vedu_packet->slc_len[0], vedu_packet->virt_addr[0], vedu_packet->slc_len[0]);
        *stream_temp += vedu_packet->slc_len[0];
    }

    if (vedu_packet->slc_len[1] > 0) {
        memcpy_s(vir_addr + *stream_temp, vedu_packet->slc_len[1], vedu_packet->virt_addr[1], vedu_packet->slc_len[1]);
        *stream_temp += vedu_packet->slc_len[1];
    }
}

static hi_s32 get_jpge_stream(hi_handle handle, strm_mng_stream_info *stream_info,
    vedu_efl_nalu *vedu_packet, hi_venc_stream *stream, venc_ioctl_buf_offset *buf_offset)
{
    hi_u32 mode_handle, stream_temp, stream_buf_len;
    hi_void* vir_addr = HI_NULL;
    hi_u32 recycle_cnt = 0;
    hi_s32 ret;

    vir_addr = (hi_void*)stream_info->jpeg_mmz_buf->start_vir_addr;
    stream_buf_len = stream_info->jpeg_mmz_buf->size;

    mode_handle = 0x10000 + handle;

    ret = copy_packet_part1(&stream_temp, vedu_packet, stream_buf_len, vir_addr);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("copy_packet_part1 fail\n");
        return ret;
    }

    ret = strm_rls_read_buffer(mode_handle, stream_info, vedu_packet);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("release stream failed, ret= %#x.\n", ret);

        return HI_ERR_VENC_CHN_RELEASE_ERR;
    } do {
        get_stream_read_buffer(&recycle_cnt, stream_info, mode_handle, vedu_packet);
    }
    while (((ret != HI_SUCCESS) || (vedu_packet->frame_end != HI_TRUE)) && (recycle_cnt < 1000)); /* 1000 is max num */

    if (recycle_cnt >= 1000) { /* 1000 is min num */
        HI_ERR_VENC("the time of getting stream is too long!\n");
        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    if ((vedu_packet->slc_len[0] + vedu_packet->slc_len[1]) > (stream_buf_len - stream_temp)) {
        HI_ERR_VENC("jpeg 2nd get stream failed, dst_buf_len= %d(need %d), not enough\n", stream_buf_len,
            (vedu_packet->slc_len[0] + vedu_packet->slc_len[1]));

        return HI_ERR_VENC_CHN_INVALID_STAT;
    }

    copy_packet_part2(&stream_temp, vedu_packet, vir_addr);

    ret = strm_rls_read_buffer(mode_handle, stream_info, vedu_packet);
    if (HI_SUCCESS != ret) {
        HI_ERR_VENC("release stream failed, ret= %#x.\n", ret);

        return HI_ERR_VENC_CHN_RELEASE_ERR;
    }

    stream->virt_addr      = (hi_size_t)(uintptr_t)vir_addr;
    stream->slc_len    = stream_temp;
    stream->pts_ms     = vedu_packet->pts0;

    stream->frame_end    = (0 == vedu_packet->frame_end) ? HI_FALSE : HI_TRUE;
    stream->data_type.h264e_type = HI_VENC_H264E_NALU_MAX;

    return HI_SUCCESS;
}

hi_s32 recycle_buf_get_jpge_stream(hi_handle handle, strm_mng_stream_info *stream_info,
    vedu_efl_nalu *vedu_packet, hi_venc_stream *stream, venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret;
    recycle_buf_init *inst = HI_NULL;
    hi_size_t flags;

    RECYCLEBUF_ASSERT_RETURN(stream_info != HI_NULL, HI_ERR_VENC_INVALID_PARA);
    RECYCLEBUF_ASSERT_RETURN(vedu_packet != HI_NULL, HI_ERR_VENC_INVALID_PARA);
    RECYCLEBUF_ASSERT_RETURN(stream != HI_NULL, HI_ERR_VENC_INVALID_PARA);
    RECYCLEBUF_ASSERT_RETURN(buf_offset != HI_NULL, HI_ERR_VENC_INVALID_PARA);

    RECYCLEBUF_FIND_INST(handle, inst, flags);

    if (inst == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    ret = get_jpge_stream(handle, stream_info, vedu_packet, stream, buf_offset);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("get_jpge_stream fail \n");
        return ret;
    }

    if (stream->slc_len > 0) {
        buf_offset->strm_buf_offset[0] = vedu_packet->phy_addr[0] - inst->st_recycle_buf.phy_addr;
    }

    return HI_SUCCESS;
}

#endif
